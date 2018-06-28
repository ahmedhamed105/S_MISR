//-----------------------------------------------------------------------------
//  File          : sysutil.c
//  Module        :
//  Description   : Include Enhance System utitilies function.
//  Author        : Lewis
//  Notes         : No UI is include in the routines & call SYSTEM or UTILIB
//                  Only.
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |         Struct/Array : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g  (variable use outside one C file)      |
// |                                                                          |
// |   BYTE( 8 Bits) size : Leading b                                         |
// |   CHAR( 8 Bits) size : Leading c                                         |
// |   WORD(16 Bits) size : Leading w                                         |
// |  DWORD(32 Bits) size : Leading d                                         |
// |    int(32 Bits) size : Leading i                                         |
// | DDWORD(64 Bits) size : Leading dd                                        |
// |              Pointer : Leading p                                         |
// |                                                                          |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |                                                                          |
// | Examples:                                                                |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "hwdef.h"
#include "midware.h"
#include "hardware.h"
#include "util.h"
#include "kbdutil.h"
#include "wifi.h"
#include "mcp.h"
#include "sysutil.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define DEBUG_POINT         1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
#define FLASH_START_ADDRESS         0x00100000
#define SRAM_END_ADDRESS            0x002FFFFF

static const BYTE KMonthText[13][3] = {
  {' ',' ',' '},
  {'J','A','N'},
  {'F','E','B'},
  {'M','A','R'},
  {'A','P','R'},
  {'M','A','Y'},
  {'J','U','N'},
  {'J','U','L'},
  {'A','U','G'},
  {'S','E','P'},
  {'O','C','T'},
  {'N','O','V'},
  {'D','E','C'}
};


//*****************************************************************************
//  Function        : LongBeep
//  Description     : Generate long beep sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : 30ms;
//  Globals Changed : N/A
//*****************************************************************************
void LongBeep(void)
{
  BeepMW(30, 9, 1);
}
//*****************************************************************************
//  Function        : AcceptBeep
//  Description     : Generate a beep sound for accept.
//  Input           : N/A
//  Return          : N/A
//  Note            : 3 X 100ms
//  Globals Changed : N/A
//*****************************************************************************
void AcceptBeep(void)
{
  BeepMW(10, 10, 3);
}
//*****************************************************************************
//  Function        : Short2Beep
//  Description     : Generate two short beep sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : 2 X 20ms
//  Globals Changed : N/A
//*****************************************************************************
void Short2Beep(void)
{
  BeepMW(2, 2, 2);
}
//*****************************************************************************
//  Function        : Short1Beep
//  Description     : Generate one short beep sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : 1 X 20ms
//  Globals Changed : N/A
//*****************************************************************************
void Short1Beep(void)
{
  BeepMW(2, 2, 1);
}
//*****************************************************************************
//  Function        : Delay1Sec
//  Description     : Delay for certain second or key pressed.
//  Input           : time;         // seconds to Delay
//                    beep_reqd;    // beep required ?
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Delay1Sec(DWORD aTime, BOOLEAN aBeepReqd)
{
  DWORD free_run, curr;
  if (aBeepReqd)
    LongBeep();

  aTime *= 200;  // convert to 5ms units
  free_run = FreeRunMW();
  while (GetCharMW() != MWKEY_CANCL) {
    SleepMW();
    curr = FreeRunMW();
    curr -= free_run;
    if (curr >= aTime)
      break;
  }
}
//*****************************************************************************
//  Function        : ErrorDelay
//  Description     : Special Delay routine for error.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ErrorDelay(void)
{
  Delay1Sec(10, 1);
}
//*****************************************************************************
//  Function        : Delay10ms
//  Description     : Delay a number of 10ms .
//  Input           : cnt; // count of 10ms to Delay
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Delay10ms(DWORD aCnt)
{
  DWORD curr, free_run = FreeRunMW();
  aCnt *= 2;
  do {
    SleepMW();
    curr = FreeRunMW();
    curr -= free_run;
  } while (curr < aCnt);
}
//*****************************************************************************
//  Function        : ShowStatus
//  Description     : Show message on status line.
//  Input           : aMsg;     // pointer to string message
//                    aMode;    // Display Mode
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowStatus(char *aMsg, DWORD aMode)
{
  DispLineMW(aMsg, MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SPFONT);
}
//*****************************************************************************
//  Function        : DispPutStr
//  Description     : Put a String on display.
//  Input           : aDispStr;         // message string
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispPutStr(const char *aDispStr)
{
  DWORD len = strlen(aDispStr)+1;
  BYTE *ptr = (BYTE *)MallocMW(len);
  if (ptr != NULL) {
    memcpy(ptr, aDispStr, len);
    DispPutNCMW(ptr, len);
    FreeMW(ptr);
  }
}
//*****************************************************************************
//  Function        : ShowBWLogo
//  Description     : Display Black&White Logo.
//  Input           : aLogoDat;     // pointer to logo data
//                    aLogoSize;    // Size of logo data
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowBWLogo(const BYTE *aLogoDat, DWORD aLogoSize)
{
  BYTE *logo_ptr;
  logo_ptr = (BYTE *)MallocMW(aLogoSize);
  if (logo_ptr != NULL) {
    memcpy(logo_ptr, aLogoDat, aLogoSize);
    DispLogoMW(logo_ptr, 1, 1);
    FreeMW(logo_ptr);
  }
}
//*****************************************************************************
//  Function        : ShowGraphicT800
//  Description     : Display Color graphic on LCD.
//  Input           : aDat;     // pointer to graphic data (USing T800 converter)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowGraphicT800(struct GLOC *aLoc, const BYTE *aDat)
{
  #if (CLCD_SUPPORT)
  struct CGRAPH *pGraphic;
  DWORD  logo_size;
  #if (T300|T810|A5T1000|T1000)
  DWORD  i;
  WORD   color;
  WORD   temp;
  #endif

  logo_size = aLoc->wWidth*aLoc->wHeight*sizeof(WORD);
  pGraphic = (struct CGRAPH *)MallocMW(logo_size+8);
  pGraphic->w_hstart = aLoc->wPosX;
  pGraphic->w_vstart = aLoc->wPosY;
  pGraphic->w_hwidth = aLoc->wWidth;
  pGraphic->w_vwidth = aLoc->wHeight;
  #if (T300|T810|A5T1000|T1000)
  for (i = 0; i < logo_size; i+=2){
    temp = aDat[i]+(aDat[i+1]*256);
    color = T800_TO_T1000(temp);
    memcpy(&pGraphic->sb_data[i], (BYTE *)&color, 2);
  }
  #else
  memcpy(pGraphic->sb_data, aDat, logo_size);
  #endif

  DispPutGMW(pGraphic);
  FreeMW(pGraphic);
  #endif
}
//*****************************************************************************
//  Function        : Disp2x16Msg
//  Description     : Display 2x16 message on lcd using DispLine
//  Input           : aMsg;         // pointer to 2x16 message
//                    aOffset;      // DispLine's offset
//                    aCtrl;        // DispLine's control
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Disp2x16Msg(const BYTE *aMsg, DWORD aOffset, DWORD aCtrl)
{
  DWORD next_line;
  BYTE  tmpbuf[17];

  memset(tmpbuf, 0, sizeof(tmpbuf));
  memcpy(tmpbuf, aMsg, 16);
  DispLineMW(tmpbuf,  aOffset, aCtrl|MW_CENTER);
  next_line = (aCtrl&MW_BIGFONT)?(MW_LINE3-MW_LINE1):(MW_LINE2-MW_LINE1);
  aCtrl &= ~MW_CLRDISP;
  memcpy(tmpbuf, &aMsg[16], 16);
  DispLineMW(tmpbuf, aOffset+next_line, aCtrl|MW_CENTER);
}
//*****************************************************************************
//  Function        : SetRTC
//  Description     : Set Real Time clock.
//  Input           : aDtg;     // pointer to structure DATETIME.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetRTC(struct DATETIME *aDtg)
{
  BYTE buffer[15];

  aDtg->bCentury = (aDtg->bYear>0x50)?0x19:0x20;
  split(buffer,(BYTE*)aDtg,7);
  RtcSetMW(buffer);
}
//*****************************************************************************
//  Function        : ReadRTC
//  Description     : Get Real Time clock.
//  Input           : aDtg;     // pointer to structure struct DATETIME buffer.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ReadRTC(struct DATETIME *aDtg)
{
  BYTE buffer[14];

  RtcGetMW(buffer);
  compress((BYTE*)aDtg,buffer,7);
}
//*****************************************************************************
//  Function        : ConvDateTime
//  Description     : Convert input DATETIME to specify string.
//  Input           : aBuf;       // pointer to output buf
//                    aDtg;       // pointer struct DATETIME value
//                    aLongYear;  // convert to LONG year fmt when TRUE.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ConvDateTime(BYTE *aBuf, struct DATETIME *aDtg, BOOLEAN aLongYear)
{
  memset(aBuf,' ',18);
  memcpy(aBuf, KMonthText[(BYTE)bcd2bin(aDtg->bMonth)], 3); /* month */
  split(&aBuf[4], &aDtg->bDay, 1); /* day */
  if (aBuf[4] == '0')
    aBuf[4] = ' ';
  aBuf[6] = ',';
  split(&aBuf[8],&aDtg->bCentury,2);
  split(&aBuf[13], &aDtg->bHour, 1);
  aBuf[15] = ':';
  split(&aBuf[16], &aDtg->bMinute, 1);
  if (!aLongYear)
    memmove(&aBuf[8],&aBuf[10], 8);
}
//*****************************************************************************
//  Function        : SetDTG
//  Description     : Sync RTC with input date time.
//  Input           : aDataTime;   // pointer to 6 byte date_time
//                                 // in bcd YYMMDDHHMMSS
//  Return          : System Year BCD value;
//  Note            : Year is adjust when necessary.
//  Globals Changed : N/A
//*****************************************************************************
BYTE SetDTG(BYTE *aDateTime)
{
  BYTE tmp;
  struct DATETIME dtg;

  ReadRTC(&dtg);
  tmp = 1;
  if ((aDateTime[1] == 0x12) && (dtg.bMonth == 0x01))
    bcdsub(aDateTime,&tmp,1);
  else
    if ((aDateTime[1] == 0x01) && (dtg.bMonth == 0x12))
      bcdadd(aDateTime,&tmp,1);
  memcpy(&dtg.bYear,aDateTime,6);
  SetRTC(&dtg);

  return dtg.bYear;
}
//*****************************************************************************
//  Function        : FreeRunMark
//  Description     : Get the current free run mark.
//  Input           : N/A
//  Return          : current mark.
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
DWORD FreeRunMark(void)
{
  return FreeRunMW();
}
//*****************************************************************************
//  Function        : FreeRunElapsed
//  Description     : Calc the Time elapsed in ms start from mark.
//  Input           : aStartMark.
//  Return          : time in ms.
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
DWORD FreeRunElapsed(DWORD aStartMark)
{
  DWORD curr_mark = FreeRunMW();

  return ((curr_mark - aStartMark) * 5);   // 5ms per tick.
}
//*****************************************************************************
//  Function        : ShowIp
//  Description     : Show ip addr.
//  Input           : aIp;        // pointer to 4 byte ip.
//                    aPos;       // line pos.
//                    aReverse;   // TRUE=> reverse on.
//  Return          : -1;         // Cancel;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowIp(unsigned char *aIp, int aPos, int aReverse)
{
  BYTE buf[128];

  DispGotoMW(aPos, MW_SPFONT);
  if (aReverse) DispCtrlMW(MW_REV_ON);
  sprintf(buf, "%3d.%3d.%3d.%3d", aIp[0], aIp[1], aIp[2], aIp[3]);
  DispLineMW(buf, aPos, MW_SPFONT);
  if (aReverse) DispCtrlMW(MW_REV_OFF);
}
//*****************************************************************************
//  Function        : EditIp
//  Description     : Edit IP address.
//  Input           : aip;    // pointer to 4 bytes IP address buffer.
//                    aPos;   // position to display.
//  Return          : TRUE;
//                    FALSE;    // USER CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EditIp(BYTE *aIp, DWORD aPos)
{
  DWORD curr_field;
  DWORD cursor=0;
  BYTE data[IP_LEN], buf[128];
  DWORD i;
  DWORD keyin;

  memcpy(data,aIp,IP_LEN);

  curr_field=IP_LEN;
  while (1) {
    if (curr_field<IP_LEN) {
      /* Show all data */
      ShowIp(data, aPos, FALSE);
      /* Show editing field */
      sprintf(buf, "%-3d", data[curr_field]);
      DispLineMW(buf, aPos+(curr_field*4)+EDITIP_COL, MW_REVERSE|MW_SPFONT);
    }
    else
      ShowIp(data,aPos,TRUE);

    keyin = WaitKey(KBD_TIMEOUT);

    /* Valid numeric key */
    if ((keyin>=MWKEY_0)&&(keyin<=MWKEY_9)) {
      /* Start edit 1st field at all field selected */
      if (curr_field>=IP_LEN)
        curr_field=0;
      /* Editing digit */
      if (cursor==0) {
        data[curr_field]=(BYTE) (keyin-'0');
        cursor++;
      }
      else
        if (cursor<EDITIP_CURSOR_MAX) {
          i=data[curr_field]*10+(BYTE) (keyin-'0');
          if (i>255)
            i=255;
          data[curr_field]=(BYTE)i;
          cursor++;
        }
      /* Last digit, auto ENTER */
      if (cursor==EDITIP_CURSOR_MAX)
        keyin=MWKEY_ENTER;
    }

    /* Other keys */
    switch (keyin) {
      case MWKEY_ENTER:
        if (curr_field>=IP_LEN) {
          memcpy(aIp,data,IP_LEN);
          ShowIp(data,aPos,FALSE);
          return TRUE;
        }
        curr_field++;
        cursor=0;
        break;
      case MWKEY_CLR:
        if (curr_field >= IP_LEN) {
          curr_field = 0;
        }
        else if ((cursor == 0)&&(curr_field>0))
          curr_field--;
        else if (curr_field<IP_LEN) {
          data[curr_field]=0;
          cursor=0;
        }
        break;
      case MWKEY_CANCL:
        if (curr_field<IP_LEN) {
          memcpy(data,aIp,IP_LEN);
          curr_field=IP_LEN;
          cursor=0;
        }
        else
          return FALSE;
        break;
    }
  }  // while
}
//*****************************************************************************
//  Function        : EditPort
//  Description     : Edit port number.
//  Input           : port;     // pointer to port number buffer
//                    aRow;     // Row number to display.
//  Return          : TRUE;
//                    FALSE;    // USER CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EditPort(DWORD *aPort, DWORD aPos)
{
  BYTE tmp[32];

  sprintf ((char *)&tmp[1], "%d", *aPort);
  tmp[0] = strlen(&tmp[1]);
  if (!GetKbd(ECHO+NUMERIC_INPUT+MW_SPFONT+aPos, IMAX(5), tmp))
    return FALSE;
  tmp[tmp[0]+1] = 0;
  *aPort = atoi ((char *)&tmp[1]);
  return TRUE;
}
//*****************************************************************************
//  Function        : CheckPointerAddr
//  Description     : Check whether pointer is syster pointer (stack or malloc)
//                    or not
//  Input           : aPtr
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CheckPointerAddr(void *aPtr)
{
  #ifndef WIN32
  DWORD address;

  address = (DWORD)aPtr;
  if ((address < FLASH_START_ADDRESS) || (address > SRAM_END_ADDRESS))
    return;

  DispLineMW("Not A Valid Pointer!!", MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SPFONT);
  LongBeep();
  while (GetCharMW() == 0) SleepMW();
  #endif
}
//*****************************************************************************
//  Function        : MemFatalErr
//  Description     : Check for valid memory allocated pointer.
//                    Err: Prompt Error Message & Reset.
//  Input           : aMemPtr;          // pointer to allocated memory
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void MemFatalErr(void *aMemPtr)
{
  if (aMemPtr == NULL) {
    DispLineMW("Memory Error !", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    LongBeep();
    while (GetCharMW() == 0) SleepMW();
    ResetMW();
  }
}
//*****************************************************************************
//  Function        : FreeRunSec
//  Description     : Return Number of seconds since 1 Jan 1970.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD FreeRunSec(void)
{
  BYTE rtc[15];

  RtcGetMW(rtc);
  return RTC2UNIX(rtc);
}
//*****************************************************************************
//  Function        : ShowTxRxLed
//  Description     : Show TX/RX Led indicator.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowTxRxLed(void)
{
  #define ALL_ICON                  (DWORD)0x00FFFFC0
  static DWORD ostatus;
  DWORD status, tmp;
  int i;

  if (GetLcdTypeMW()==MW_DISP_COLOR) {
    return;
  }

  // Show UDP, TCP, USBS, AUX2, AUX1, MDM
  #ifdef os_pseudo_led_status
  status = os_pseudo_led_status((DWORD)-1);
  #elif defined os_led_status
  status = os_led_status((DWORD)-1);
  #else
  status = 0;
  #endif

  #if (TIRO|PR608)
  tmp   =  (status & 0x3F);
  tmp   |= ((status&0x00C00)>>4);
  tmp   |= ((status&0x3C000)>>6);
  #else
  tmp    = ((status >> 20) & 0x03);                 // mdm
  tmp   |= ((status & 0x0F) << 2);                  // aux2 & aux1
  tmp   |= ((status & 0x30000)>>10);                // USBS
  tmp   |= (status & 0x550) ? 0x100 : 0x000;        // TCP TX
  tmp   |= (status & 0x660) ? 0x200 : 0x000;        // TCP RX
  tmp   |= (status & 0x5000) ? 0x400 : 0x000;       // UDP TX
  tmp   |= (status & 0x6000) ? 0x800 : 0x000;       // UDP RX
  #endif

  status = 0;
  for (i = 0; i < 6; i++) {
    status <<= 3;
    if (tmp & 0x01)
      status |= 0x05;
    if (tmp & 0x02)
      status |= 0x02;
    tmp >>= 2;
  }
  status <<= 6;
  //ignore gprs signal and battery
  ostatus &= ALL_ICON;
  status &= ALL_ICON;

  #ifdef os_disp_icon_set
  if (status != ostatus) {
    #if (T800)
    os_disp_icon_clear(((ostatus>>6)&0x3FFFF), 0);
    #else
    os_disp_icon_clear(ostatus);
    #endif

    ostatus = status;
    #if (T800)
    os_disp_icon_set(((status>>6)&0x3FFFF), 0);
    #else
    os_disp_icon_set(status);
    #endif
  }
  #endif

  os_thread_return();
}
//*****************************************************************************
//  Function        : DebugPoint
//  Description     : Display Msg on line 9 and wait for key press.
//  Input           : aStr;         // pointer to message string.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DebugPoint(char *aStr)
{
  #if (DEBUG_POINT)
  DispLineMW(aStr, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
  while (GetCharMW() == 0) SleepMW();
  #endif // DEBUG_POINT
}
//*****************************************************************************
//  Function        : ReadSFile
//  Description     : Read from structure data file.
//  Input           : aFileHandle;      // file handle
//                    aOffset;          // file offset from beginning
//                    aBuf;             // data buffer
//                    aLen;             // len to read.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ReadSFile(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen)
{
  DWORD len;

  fSeekMW(aFileHandle, aOffset);
  len = fReadMW(aFileHandle, aBuf, aLen);
  return (len == aLen) ? TRUE : FALSE;
}
//*****************************************************************************
//  Function        : WriteSFile
//  Description     : Write to structure data file.
//  Input           : aFileHandle;      // file handle
//                    aOffset;          // file offset from beginning
//                    aBuf;             // data buffer
//                    aLen;             // len to read.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WriteSFile(DWORD aFileHandle, DWORD aOffset, const void *aBuf, DWORD aLen)
{
  DWORD len;

  fSeekMW(aFileHandle, aOffset);
  len = fWriteMW(aFileHandle, aBuf, aLen);
  if (len != aLen) {
    DispLineMW("File WR Err!", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    ErrorDelay();
  }
  return (len == aLen) ? TRUE : FALSE;
}
//*****************************************************************************
//  Function        : WGet
//  Description     : Retrieve WORD from input buffer.
//  Input           : aBuf;     // Data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD WGet(void *aBuf)
{
  return *(BYTE *)aBuf * 256 + *((BYTE *)aBuf + 1);
}
//*****************************************************************************
//  Function        : DWGet
//  Description     : Retrieve DWORD from input buffer.
//  Input           : aBuf;     // Data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD DWGet(void *aBuf)
{
  DWORD ret;
  BYTE i;

  ret = 0;
  for (i=0; i<4; i++) {
    ret <<= 8;
    ret |= *((BYTE *)aBuf+i);
  }
  return ret;
}
//*****************************************************************************
//  Function        : WaitLanNetRdy
//  Description     : Wait LAN Network Ready;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WaitLanNetRdy(DWORD aTimeout, BOOLEAN aNoCancel)
{
  BYTE dhcp;
  struct MW_NIF_INFO netinfo;

  // wait until LAN cable is plugged & DHCP ready
  GetSysCfgMW(MW_SYSCFG_DHCP_ENABLE, &dhcp);
  TimerSetMW(gTimerHdl[TIMER_COMM], aTimeout);
  do {
    SleepMW();
    if (GetCharMW()==MWKEY_CANCL) {
      if (!aNoCancel)
        break;
      LongBeep();
    }
    if (!LanCableInsertedMW()) {
      DispLineMW("Check Lan Cable!\x05", MW_LINE7, MW_CENTER|MW_SPFONT);
    }
    else {
      if (dhcp) {
        NetInfoMW(MW_NIF_ETHERNET, &netinfo);
        if (netinfo.d_ip == 0) {
          DispLineMW("Waiting LAN DHCP..\x05", MW_LINE7, MW_CENTER|MW_SPFONT);
          continue;
        }
      }
      DispClrBelowMW(MW_LINE7);
      Delay10ms(50);
      break;
    }
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  if (TimerGetMW(gTimerHdl[TIMER_COMM]) == 0)
    return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : WaitWifiNetRdy
//  Description     : Wait Wifi Network Ready;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WaitWifiNetRdy(DWORD aTimeout, BOOLEAN aNoCancel)
{
  struct MW_NIF_INFO netinfo;

  // check if AP is connected
  if ((GetWifiStatus() & MW_WIFI_CONNECTED) == 0) {
    DispLineMW("Wifi AP Disconnected!", MW_LINE7, MW_CLREOL|MW_CENTER|MW_SPFONT);
    Delay10ms(100);
    return FALSE;
  }

  // wait until DHCP ready
  TimerSetMW(gTimerHdl[TIMER_COMM], aTimeout);
  do {
    SleepMW();
    if (GetCharMW()==MWKEY_CANCL) {
      if (!aNoCancel)
        break;
      LongBeep();
    }
    NetInfoMW(MW_NIF_WIFI, &netinfo);
    if (netinfo.d_ip == 0) {
      DispLineMW("Waiting Wifi DHCP..\x05", MW_LINE7, MW_CENTER|MW_SPFONT);
      continue;
    }
    else
      break;
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  if (TimerGetMW(gTimerHdl[TIMER_COMM]) == 0)
    return FALSE;

  return TRUE;
}
#if (MCP_SUPPORT)
//*****************************************************************************
//  Function        : WaitMcpNetRdy
//  Description     : Wait MCP Network Ready;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WaitMcpNetRdy(DWORD aTimeout, BOOLEAN aNoCancel)
{
  // wait until MCP ready
  TimerSetMW(gTimerHdl[TIMER_COMM], aTimeout);
  do {
    SleepMW();
    if (GetCharMW()==MWKEY_CANCL) {
      if (!aNoCancel)
        break;
      LongBeep();
    }
    if (GetMCPStatus() != K_MCP_PHASE_CONNECTED) {
      DispLineMW("Waiting MCP Rdy..\x05", MW_LINE7, MW_CENTER|MW_SPFONT);
      continue;
    }
    else
      break;
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  if (TimerGetMW(gTimerHdl[TIMER_COMM]) == 0)
    return FALSE;

  return TRUE;
}
#endif
//*****************************************************************************
//  Function        : HotPlug
//  Description     : Handle LAN cable Hot Plug.
//  Input           : N/A
//  Return          : N/A
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
enum {
 HP_WAITON   = 1,
 HP_CHKSTATE    ,
 HP_WAITCLOSE   ,
 HP_WAITOFF     ,
};
static DWORD gHpState = HP_WAITON;
static DWORD gHotPlugCnt;
void HotPlug(void)
{
  #define KSecIn1ms(x)    x*1000
  DWORD status;
  //BYTE buf[32];

  //SprintfMW(buf, "HotPlug: %d\x05", gHpState);
  //DispLineMW(buf, MW_LINE8, MW_SPFONT);
  switch (gHpState) {
    case HP_WAITON:
       bs_lan_open();
       gHpState = HP_CHKSTATE;
       gHotPlugCnt = FreeRunMark();
       break;
    case HP_CHKSTATE:
       status = bs_lan_status(K_BsLanStatus);
       if (status & K_LanPlugIn) {
         gHpState = HP_WAITOFF;
         break;
       }
       if (FreeRunElapsed(gHotPlugCnt) > KSecIn1ms(5)) {
         bs_lan_close();
         gHpState = HP_WAITCLOSE;
         gHotPlugCnt = FreeRunMark();
       }
       break;
    case HP_WAITOFF:
       status = bs_lan_status(K_BsLanStatus);
       if (!(status & K_LanPlugIn)) {
         bs_lan_close();
         gHpState = HP_WAITCLOSE;
         gHotPlugCnt = FreeRunMark();
       }
       break;
    case HP_WAITCLOSE:
       if (FreeRunElapsed(gHotPlugCnt) > KSecIn1ms(2)) {
         gHpState = HP_WAITON;
       }
       break;
  }
  os_thread_return();
}

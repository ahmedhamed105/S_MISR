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
#include <string.h>
#include "midware.h"
#include "util.h"
#include "hardware.h"
#include "lptutil.h"
#include "sysutil.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define DEBUG_POINT         1
//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
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
//  Function        : SetRTC
//  Description     : Set Real Time clock.
//  Input           : aTDtg;     // pointer to structure DATETIME.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetRTC(struct DATETIME *aTDtg)
{
  BYTE buffer[14];

  aTDtg->b_century = (aTDtg->b_year>0x50)?0x19:0x20;
  split(buffer,(BYTE*)aTDtg,7);
  RtcSetMW(buffer);
}
//*****************************************************************************
//  Function        : ReadRTC
//  Description     : Get Real Time clock.
//  Input           : dtg;     // pointer to structure struct DATETIME buffer.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ReadRTC(struct DATETIME *aTDtg)
{
  BYTE buffer[14];

  RtcGetMW(buffer);
  compress((BYTE*)aTDtg,buffer,7);
}
//*****************************************************************************
//  Function        : ConvDateTime
//  Description     : Convert input DATETIME to specify string.
//  Input           : aBuf;       // pointer to output buf
//                    aTDtg;       // pointer struct DATETIME value
//                    aLongYear;  // convert to LONG year fmt when TRUE.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ConvDateTime(BYTE *aBuf, struct DATETIME *aTDtg, BOOLEAN aLongYear)
{
  memset(aBuf,' ',18);
  memcpy(aBuf, KMonthText[(BYTE)bcd2bin(aTDtg->b_month)], 3); /* month */
  split(&aBuf[4], &aTDtg->b_day, 1); /* day */
  if (aBuf[4] == '0')
    aBuf[4] = ' ';
  aBuf[6] = ',';
  split(&aBuf[8],&aTDtg->b_century,2);
  split(&aBuf[13], &aTDtg->b_hour, 1);
  aBuf[15] = ':';
  split(&aBuf[16], &aTDtg->b_minute, 1);
  if (!aLongYear)
    memmove(&aBuf[8],&aBuf[10], 8);
}

//*****************************************************************************
//  Function        : ConvDateTime
//  Description     : Convert input DATETIME to specify string.
//  Input           : aBuf;       // pointer to output buf
//                    aTDtg;       // pointer struct DATETIME value
//                    aLongYear;  // convert to LONG year fmt when TRUE.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ConvDateTimeT(BYTE *aBuf, struct DATETIME *aTDtg, BOOLEAN aLongYear)
{
  memset(aBuf,' ',11);
  split(aBuf, &aTDtg->b_month, 1); /* month */
  aBuf[2] = '/';
 // memcpy(aBuf, KMonthText[(BYTE)bcd2bin(aTDtg->b_month)], 3); /* month */
  split(&aBuf[3], &aTDtg->b_day, 1); /* day */
  aBuf[5] = ' ';
 // split(&aBuf[8],&aTDtg->b_century,2);
  split(&aBuf[6], &aTDtg->b_hour, 1);
  aBuf[8] = ':';
  split(&aBuf[9], &aTDtg->b_minute, 1);
 
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
  if ((aDateTime[1] == 0x12) && (dtg.b_month == 0x01))
    bcdsub(aDateTime,&tmp,1);
  else
    if ((aDateTime[1] == 0x01) && (dtg.b_month == 0x12))
      bcdadd(aDateTime,&tmp,1);
  memcpy(&dtg.b_year,aDateTime,6);
  SetRTC(&dtg);

  return dtg.b_year;
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
//  Function        : ByteCopy
//  Description     : Same as memcpy. Use to avoid compiler bug.
//  Input           : dest;     // pointer to destination buffer
//                    src;      // pointer to source buffer
//                    len;      // number of bytes to copy
//  Return          : N/A
//  Note            : typedef struct {
//                      BYTE abc;
//                      WORD C;
//                    } a; b;
//                    memcpy(&a, &b, sizeof(a));
//                    IAR fail to generate correct code for about memcpy statement.
//  Globals Changed : N/A
//*****************************************************************************
void ByteCopy(BYTE *aDest, BYTE *aSrc, DWORD aLen)
{
  memcpy(aDest, aSrc, aLen);
}
//*****************************************************************************
//  Function        : CompressInputData
//  Description     : Fill the data with '0' & compress it.
//  Input           : aDest;    // pointer to destinate buffer;
//                    aSrc;     // pointer to src buffer.
//                              // 1st byte is len of input
//                    aLen;     // number of byte to compress.
//  Return          : N/A
//  Note            : Max len for input is 12.
//  Globals Changed : N/A
//*****************************************************************************
void CompressInputData(void *aDest, BYTE *aSrc, BYTE aLen)
{
  BYTE buffer[12];

  memset(buffer, '0', 12); /* 12 is the max len */
  memcpy(&buffer[aLen*2-aSrc[0]], &aSrc[1], aSrc[0]);
  compress(aDest, buffer, aLen);
}
//*****************************************************************************
//  Function        : CompressInputFData
//  Description     : Fill the data with 'F' & compress it.
//  Input           : aDest;    // pointer to destinate buffer;
//                    aSrc;     // pointer to src buffer.
//                              // 1st byte is len of input
//                    aLen;     // number of byte to compress.
//  Return          : N/A
//  Note            : Max len for input is 24.
//  Globals Changed : N/A
//*****************************************************************************
void CompressInputFData(void *aDest, BYTE *aSrc,  BYTE aLen)
{
  BYTE tmp[25];

  memcpy(tmp, aSrc, aSrc[0]+1);
  memset(&tmp[tmp[0]+1], 'F', 24-tmp[0]);
  compress(aDest, &tmp[1], aLen);
}
//*****************************************************************************
//  Function        : GetMonthText
//  Description     : Return pointer to three byte month text.
//  Input           : mm;    // month
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE *GetMonthText(BYTE aMM)
{
  return(BYTE *)KMonthText[aMM];
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
    DispLineMW("Memory Error!", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    LongBeep();
    while (GetCharMW() == 0) SleepMW();
    ResetMW();
  }
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
#ifdef DEBUG_POINT
  DispLineMW(aStr, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
  while (GetCharMW() == 0) SleepMW();
#endif // DEBUG_POINT
}
//*****************************************************************************
//  Function        : ConvAmount
//  Description     : Converts a formated amount to a format easy for display
//                    and printing.
//  Input           : aAmt;       // DDWORD amount
//                    aDispAmt;   // pointer to sturct DISP_AMT buffer.
//                    aDecPos;    // Decimal Position
//                    aCurrSym;   // Currency Symbol.
//  Return          : TURE  => convertion ok
//                    FALSE => conversion fail, return all '9'
//  Note            : result = > len byte + $NNNN.NN
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ConvAmount(DDWORD aAmt, struct DISP_AMT *aDispAmt, DWORD aDecPos, BYTE aCurrSym)
{
  BYTE buffer[21];
  BYTE lead0len;
  BOOLEAN overflow;

  overflow = FALSE;
  dbin2asc(buffer,aAmt);

  if (memcmp(buffer, "00000000", 8)!=0) {
    overflow = TRUE;
    memset(&buffer[8], '9', 12);
  }
  lead0len = (BYTE)skpb(buffer, '0', 17);      /* leading zero count */
  aDispAmt->content[0] = aCurrSym;
  aDispAmt->len = 19 -lead0len;          /* excluding decimal digits */
  memcpy(&aDispAmt->content[1], &buffer[lead0len], aDispAmt->len - 1);
  if (aDecPos != 0) {
    buffer[17] = '.';
    memcpy(&aDispAmt->content[aDispAmt->len], &buffer[17], 3);
    aDispAmt->len += 3;
  }
  return overflow;
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
//  Function        : Transform
//  Description     : Convert T_LCDG data to byte array.
//  Input           : aDest;    // dest byte array
//                    aLcd;     // source lcd graphic data.
//  T_LCDG->p_data:                 aDest:
//        XXXXXX40 |            01234567
//        XXXXXX51 v            xxxxxxxx
//        XXXXXX62              xxxxxxxx
//  last->XXXXXXX3              xxxxxxxx  <- last byte
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void Transform(BYTE *aDest, T_LCDG *aLcd, WORD aLdZeroPixel)
{
  DWORD x,y;
  WORD  *ptr = aLcd->p_data;
  
  memset(aDest, 0, (aLcd->w_hwidth+aLdZeroPixel)*aLcd->w_vwidth);
  for (x=aLcd->w_hwidth; x>0; x--) {
    for (y=0; y < aLcd->w_vwidth; y++) {
      aDest[y*(aLcd->w_hwidth+aLdZeroPixel)+aLdZeroPixel+x-1]=(*ptr++==0x0000) ? 0x01 : 0x00;
    }
  }
}

//*****************************************************************************
//  Function        : PrintLcd2LptCtr
//  Description     : Print Lcd Pixel to printer.
//  Input           : aPosX;    // left top x
//                    aPosY;    // left top y
//                    aHPixel;  // horizontal width
//                    aVPixel;  // veritcal width
//  Return          : 0:        // 0 success
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int PrintLcd2LptCtr(WORD aPosX, WORD aPosY, WORD aHPixel, WORD aVPixel)
{
  #define MAX_PRN_WIDTH        384
  #if(T300|T810)
  struct TMLPTCFG lptcfg= {5, 0, 2, 2, 2, 2};
  #else
  struct TMLPTCFG lptcfg = {5, 0, 0, 0, 0, 0}; 
  #endif  
  #if(T300)
  DWORD hd_cfg;
  #endif

  T_LCDG *pScreenBuf;
  BYTE   *pLptBuf, *pTranBuf, *pSrc, *pDest;
  WORD   lptLdZeroPixel = (MAX_PRN_WIDTH - aHPixel) / 2;          // leading zeros
  WORD   lptwidth_bytes = (aHPixel + lptLdZeroPixel + 7) / 8;     // center-aligned
  DWORD  lptbuf_size    = lptwidth_bytes*aVPixel+4;
  DWORD  offset, x, y, i;
  int    ret = 0;

  #if(T300)
  hd_cfg = os_hd_config_extend();
  if (((hd_cfg & K_XHdModelT300)||(hd_cfg & K_XHdModelT300C)))
    return 0;
  #endif
  
  pScreenBuf = (T_LCDG *) os_malloc(K_LogoSize);
  pTranBuf   = (BYTE *) os_malloc((aHPixel+lptLdZeroPixel)*aVPixel);
  pLptBuf    = (BYTE *) os_malloc(lptbuf_size);
  MemFatalErr(pScreenBuf);
  MemFatalErr(pTranBuf);
  MemFatalErr(pLptBuf);

  pScreenBuf->w_hstart = aPosX;
  pScreenBuf->w_vstart = aPosY;
  pScreenBuf->w_hwidth = aHPixel;
  pScreenBuf->w_vwidth = aVPixel;
  if (!os_disp_getg(pScreenBuf)) {
    return -1;
  }

  ret = -2;     // lpt open fail

      
  if (LptOpen(&lptcfg)) {
    
    LptPutN("\x1B\x44\x01\n", 4); // enable half dot mode
         
    Transform(pTranBuf, pScreenBuf, lptLdZeroPixel);
    memset(pLptBuf, 0, lptbuf_size);
    pLptBuf[0] = 0x1B;
    pLptBuf[1] = 'K';
    pLptBuf[2] = lptwidth_bytes;
    pLptBuf[3] = aVPixel;
    pDest = &pLptBuf[4];
    memset(pDest, 0, lptwidth_bytes*aVPixel);
    pSrc = pTranBuf;
    for (y=0; y<aVPixel; y++) {     // row
      for (x=0; x<(aHPixel+lptLdZeroPixel); x+=8) {  // col
        offset = y*(aHPixel+lptLdZeroPixel)+x;
        for (i=0; i<8; i++) {
          *pDest >>= 1;
          if (((x+i)<(aHPixel+lptLdZeroPixel))&&(pSrc[offset+i]&0x01)) {
            *pDest |= 0x80;
          }
        }
        pDest++;
      }
    } 
    LptPutN(pLptBuf, lptbuf_size);
    lptbuf_size = 10;
    memset(pLptBuf, '\n', lptbuf_size);
    LptPutN(pLptBuf, lptbuf_size);
    LptClose();
  }
  ret = 0; // success
  os_free(pScreenBuf);
  os_free(pTranBuf);
  os_free(pLptBuf);
  return ret;
}
/******************************************************************************
 *  Function        : debugAsc
 *  Description     : Debug ASCII data.
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void debugAsc(BYTE *aTitle, BYTE *aData, DWORD aLen)
{
  BYTE buf[30];
  //WORD i;
  BYTE prncfg[5] = { 4, 7, 7, 4, 4 };
  
  if (aLen == 0)
    return;
    
#if 1
  // Print out TLV
  LptOpen(prncfg);
  LptPutS("\x1B""F1\n");
  sprintf(buf, "\n%s-asc(%d):\n", aTitle, aLen);
  LptPutS(buf);
  LptPutN(aData, aLen);
  LptPutS("\n");
  LptPutS("-- END --\n");
#else
  // Display TLV
  for (i=1; i<=pEMV->wLen; i++) {
    if ((i%64) == 1)
      DispLineMW("TLV-Out:\n", MW_LINE1, MW_CLRDISP|MW_SPFONT);
    SprintfMW(buf, "%02X ", (BYTE)pEMV->pbMsg[i-1]);
    DispPutNCMW(buf, 3);
    if ((i%8) == 0) PrintfMW("\n");
    if ((i%64) == 0)
      APM_WaitKey((DWORD)-1,0);
  }
  PrintfMW("\n[End]\x05");
  APM_WaitKey((DWORD)-1,0);
#endif
}
/******************************************************************************
 *  Function        : debugHex
 *  Description     : Debug HEX data.
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void debugHex(BYTE *aTitle, BYTE *aData, DWORD aLen)
{
  BYTE buf[30];
  WORD i;
  BYTE prncfg[5] = { 4, 7, 7, 4, 4 };
  
  if (aLen == 0)
    return;
    
  // Print out TLV
  LptOpen(prncfg);
  LptPutS("\x1B""F1\n");
  sprintf(buf, "%s(%d):\n", aTitle, aLen);
  LptPutS(buf);
  for (i=0; i<aLen; i++) {
    sprintf(buf, "%02X ", aData[i]);
    LptPutN(buf, 3);
  }
  LptPutS("\n");
  LptPutS("-- END --\n");
  LptClose();
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
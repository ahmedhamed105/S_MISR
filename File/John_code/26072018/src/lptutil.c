//-----------------------------------------------------------------------------
//  File          : lptutil.c
//  Module        :
//  Description   : Lpt utilities functions.
//  Author        : Lewis
//  Notes         :
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "midware.h"
#include "apm.h"
#include "sysutil.h"
#include "message.h"
#include "logo.h"
#include "lptutil.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
// assume space between 2 bookmark is 33 lines * 12 dot per line
#define MAX_BOOKMARK_GAP     33 * 12
#define BOOKMARK_STEPS       20

// Printer Related
#if (LPT_SUPPORT)
#define MAX_PRN_WIDTH        360
#else
#define MAX_PRN_WIDTH        384
#endif
#define LINE_STEPS           12

#define MAX_ENG_FONT         3
#define MAX_DOT_MODE         2

// Test Print state
enum {
  PRINT_LOGO         = 0,
  PRINT_CONSTLINE       ,
  PRINT_ATTR            ,
  PRINT_ERR             ,
  PRINT_END             ,
};

#if (A5T1000|T300|T810)
#define BATT_VOLT_THRESHOLD  35       //3.5V
#endif

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static int gLptHandle=-1;
static BYTE  gDispChgd;

//*****************************************************************************
//  Function        : RestartLpt
//  Description     : Restart Printer and Beep
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void RestartLpt(void)
{
  StatMW(gLptHandle, MW_TMLPT_ERR_RESTART, NULL);
  LongBeep();
  Delay10ms(300);
}
//*****************************************************************************
//  Function        : WaitPrnFinish
//  Description     : Wait Print finish or printer error.
//  Input           : N/A
//  Return          : current printer status.
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
void WaitPrnFinish(void)
{
  DWORD status;

  if (gLptHandle == -1)
    return;

  Delay10ms(50);
  status = StatMW(gLptHandle, 0, NULL);
  if (status & MW_LPT_PRINTING) {
    do {
      SleepMW();
      status = StatMW(gLptHandle, 0, NULL);
      if (status & MW_LPT_PAPER_OUT) {
        Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
        LongBeep();
        APM_WaitKey(200,0);
        StatMW(gLptHandle, MW_TMLPT_ERR_RESTART, NULL);
        break;
      }
      if ((status & 0xFF) == MW_TMLPT_STIME) {  // New printer error
        // time error for new printer
        DispLineMW(GetConstMsg(EDC_PN_PLS_WAIT), MW_LINE5, MW_BIGFONT);
        RestartLpt();
        continue;
      }
      if ((status & 0xFF) != 0)    // Error
        break;
    } while ((status & MW_LPT_FINISH) == 0);
  }
}
//*****************************************************************************
//  Function        : GetBatteryLevel
//  Description     : Get battery level before opening printer.
//  Input           : N/A
//  Return          : Battery level in units of 100mV
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BYTE GetBatteryLevel(void)
{
#if (A5T1000)
  if (os_batt_level() & K_PowerPluggedIn)
    return 0xFF;          // allow printing when DC connected
#endif
  return (BYTE)(os_batt_level() & 0xFF);
}
#if (A5T1000|T300|T810)
//*****************************************************************************
//  Function        : PrintLowBatt
//  Description     : Check if battery level too low for printing.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN PrintLowBatt(void)
{
#ifdef K_BattPrintLow
  return ((os_batt_level() & K_BattPrintLow)? TRUE: FALSE);
#else
  return ((GetBatteryLevel() < BATT_VOLT_THRESHOLD)? TRUE: FALSE);
#endif
}
#endif
//*****************************************************************************
//  Function        : LptClose
//  Description     : Close the printer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
void LptClose(void)
{
  if (gLptHandle >= 0) {
    WaitPrnFinish();
    CloseMW(gLptHandle);
    gLptHandle = -1;
  }
}
//*****************************************************************************
//  Function        : LptOpen
//  Description     : Open and reset the printer.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
DWORD LptOpen(void *aCfg)
{
  char filename[32];
  DWORD status;
  
  //25-07-17 JC ++
#if (A5T1000|T300|T810)
  // not allow to open printer if battery too low
  if (PrintLowBatt() == TRUE) {
    Disp2x16Msg(GetConstMsg(EDC_PN_LOW_BATT), MW_LINE5, MW_BIGFONT);
    LongBeep();
    Delay1Sec(1, 1);
    return FALSE;
  }
#endif
  //25-07-17 JC --

  LptClose();
  #if (TMLPT_SUPPORT)  
  strcpy(filename, DEV_LPT2);
  #elif (LPT_SUPPORT)
  strcpy(filename, DEV_LPT1);
  #endif
  
  gLptHandle = OpenMW(filename, MW_WRONLY);
  if (gLptHandle >= 0) {
    IOCtlMW(gLptHandle, IO_LPT_RESET, NULL);
    Delay10ms(20);
    status = StatMW(gLptHandle, MW_TMLPT_NORSTATUS, NULL);
    if (status & MW_TMLPT_FPAPEROUT) {
      Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
      LongBeep();
      APM_WaitKey(200,0);
      CloseMW(gLptHandle);
      gLptHandle = -1;
      return FALSE;
    }
    if (aCfg)
      IOCtlMW(gLptHandle, IO_TMLPT_CFG, aCfg);
    return TRUE;
  }
  //25-07-17 JC ++
  //Open failed when battery too low
  Disp2x16Msg(GetConstMsg(EDC_PN_LOW_BATT), MW_LINE5, MW_BIGFONT);
  LongBeep();
  Delay1Sec(1, 1);
  //25-07-17 JC --
  return FALSE;
}
//*****************************************************************************
//  Function        : LptPutN
//  Description     : Send the chars to printer.
//  Input           : aDat;   // pointer to byte
//                    aLen;   // len of data
//  Return          : lpt status;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
DWORD LptPutN(const BYTE *aDat, DWORD aLen)
{
  DWORD status=0;
  BYTE *ptr = (BYTE *) MallocMW(aLen);

  if (gLptHandle == -1)
    return 0;
  MemFatalErr(ptr);
  memcpy(ptr, aDat, aLen);
  if (WriteMW(gLptHandle, ptr, aLen)!=aLen) {
    //status = StatMW(gLptHandle, LPT_STS_GET, NULL);
    //sprintf(tmp, "LptWrite: %04X", status);
    //DispLineMW(tmp, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
    //ErrorDelay();
  }
  FreeMW(ptr);
  status = StatMW(gLptHandle, LPT_STS_GET, NULL);
  return status;
}
//*****************************************************************************
//  Function        : LptPutS
//  Description     : Print a string.
//  Input           : ptr;    // pointer to string buffer
//  Return          : lpt status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD LptPutS(const BYTE *aStr)
{
  return LptPutN(aStr, strlen(aStr));
}
//*****************************************************************************
//  Function        : Adv2Bookmark
//  Description     : Align the paper to the next book mark.
//  Input           : aMaxStep; // Max Advance steps to find the bookmark
//  Return          : 0:Fail, 1:Paper Out, 2:Aligned
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
DWORD Adv2Bookmark(DWORD aMaxSteps)
{
  DWORD status, ret;
  BYTE  cmd[3];

  if (gLptHandle == -1)
    return 0;

  WaitPrnFinish();

  ret = 0; // assume fail
  do {
    cmd[0] = 0x1B;
    cmd[1] = 'J';
    cmd[2] = 0x08;
    status = LptPutN(cmd, 3);
    if (status&MW_LPT_PAPER_OUT) {  // bookmark or paper out
      ret = 1;
      cmd[2] = (BYTE) BOOKMARK_STEPS;
      status = LptPutN(cmd, 3);
      break;
    }
  } while (aMaxSteps--);

  // Double Check
  status = StatMW(gLptHandle, LPT_STS_GET, NULL);
  if (!(status & MW_LPT_STAT_PAPER_OUT) && (ret == 1))
    ret = 2;
  return ret;
}
//*****************************************************************************
//  Function        : PrintLogo
//  Description     : Print logo.
//  Input           : aWidth;     // in number of Byte
//                    aHeight;    // in number of Byte
//                    *aLogoPtr;  // pointer to constant data
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD PrintLogo(DWORD aWidth, DWORD aHeight, const BYTE *aLogoPtr)
{
  DWORD status, logobuf_size;
  BYTE *ptr;
  BYTE cmd[4] = { 0x1B, 'K', 0x00, 0x00 };

  if (gLptHandle == -1)
    return 0;

  cmd[2] = (BYTE) (aWidth/8);
  cmd[3] = (BYTE) (aHeight>248?248:aHeight);

  status = LptPutN("\x0a", 1);
  status = LptPutN(cmd, 4);
  if ((status & 0xFF) != MW_LPT_STAT_START_FINISH)
    return status;

  logobuf_size = aWidth/8 * cmd[3];
  ptr = MallocMW(logobuf_size);
  if (ptr != NULL) {
    memcpy(ptr, aLogoPtr, logobuf_size);
    status = LptPutN(ptr, logobuf_size);
    FreeMW(ptr);
  }
  return status;
}
//*****************************************************************************
//  Function        : ResetLpt
//  Description     : Reset Printer and Beep
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ResetLpt(void)
{
  IOCtlMW(gLptHandle, IO_LPT_RESET, NULL);
  LongBeep();
  Delay10ms(300);
}
//*****************************************************************************
//  Function        : PrintBuf
//  Description     : Print buffer.
//  Input           : aBuf;        // pointer to receipt content buffer
//                    aLen;        // len of content
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintBuf(BYTE *aBuf, DWORD aLen, BOOLEAN aLoopPrint,BOOLEAN aLogo)
{
  DWORD status;
  #if(T300)
  DWORD hd_cfg;
  #endif
  #if(A5T1000|T300|T810)
     struct TMLPTCFG lptcfg= {5, 0, 4, 4, 16, 16};
  #else
     struct TMLPTCFG lptcfg= {5, 0, 4, 4, 16, 16};
  #endif  

  #if(T300)
  hd_cfg = os_hd_config_extend();
  if (((hd_cfg & K_XHdModelT300)||(hd_cfg & K_XHdModelT300C)))
    return TRUE;
  #endif
  
  if (aLen == 0)
    return TRUE;

  if (LptOpen(&lptcfg) == FALSE)
    return FALSE;
    
  #if (TMLPT_SUPPORT)
  if(!aLogo){
 //   LptPutN("\x1B\x44\x01", 3); // Half Dot Mode On
  //PrintLogo(384, 48, KPrintLogo);
  PrintLogo(384, 48, KPrintLogo);       // MISR Bank Logo
  LptPutN("\x1B\x44\x00\n", 4); // Half Dot Mode On
  }
  #endif
  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(GetConstMsg(EDC_PN_PRINTING), MW_LINE5, MW_BIGFONT);
  gDispChgd = FALSE;

  status = LptPutN(aBuf, aLen);
  do {
    status = StatMW(gLptHandle, 0, NULL);
    switch (status & 0xFF) {
      case MW_LPT_STAT_START_FINISH :
        //26-06-15 JC ++
        if (status & MW_TMLPT_FFINISH) {
          if (!aLoopPrint) {
            IOCtlMW(gLptHandle, IO_LPT_RESET, NULL);
          }
          LptClose();                 //25-07-17 JC ++
          return TRUE;
        }
        break;
        //26-06-15 JC --
      case MW_LPT_STAT_HIGH_TEMP : // high temperature
        Disp2x16Msg(GetConstMsg(EDC_PN_HIGHTEMP), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        LptClose();                   //25-07-17 JC ++
        return FALSE;
      #if (PR608D==0)
      case MW_LPT_STAT_LOW_VOL :
        Disp2x16Msg(GetConstMsg(EDC_PN_PLS_REPRINT), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        LptClose();                   //25-07-17 JC ++
        return FALSE;
      #endif
      case MW_LPT_STAT_PAPER_OUT : // no paper error
        Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        LptClose();                   //25-07-17 JC ++
        return FALSE;
      case MW_LPT_STAT_NOT_START : //wait print job
        SleepMW();
        break;
      case MW_TMLPT_STIME :        // time error for new printer
        DispLineMW(GetConstMsg(EDC_PN_PLS_WAIT), MW_LINE5, MW_BIGFONT);
        RestartLpt();
        break;
      default :
        Disp2x16Msg(GetConstMsg(EDC_PN_PLS_REPRINT), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        LptClose();                   //25-07-17 JC ++
        return FALSE;
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : PrintRcptLF
//  Description     : Print Receipt LineFeed
//  Input           : aCount;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintRcptLF(DWORD aCount)
{
  BYTE tmp[] = { "\n\x00" };
  while (aCount--) LptPutS(tmp);
}
//*****************************************************************************
//  Function        : PrintRcptFF
//  Description     : Print Receipt FormFeed
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintRcptFF(void)
{
  #if (LPT_SUPPORT)
  LptPutN("\x0C", 1);
  #else
  PrintRcptLF(6);
  #endif
}

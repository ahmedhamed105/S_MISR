//-----------------------------------------------------------------------------
//  File          : sysutil.h
//  Module        :
//  Description   : Declrartion & Defination for SYSUTIL.C
//  Author        : Lewis
//  Notes         : No UI is include in the routines, call util lib
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_SYSUTIL_H_
#define _INC_SYSUTIL_H_
#include "common.h"
#include "midware.h"

//-----------------------------------------------------------------------------
//  Beep & delay Functions
//-----------------------------------------------------------------------------
extern void LongBeep(void);
extern void AcceptBeep(void);
extern void Short2Beep(void);
extern void Short1Beep(void);
extern void Delay1Sec(DWORD aTime, BOOLEAN aBeepReqd);
extern void ErrorDelay(void);
extern void Delay10ms(DWORD aCnt);

//-----------------------------------------------------------------------------
//  Display Handling Functions.
//-----------------------------------------------------------------------------
#if (T300|T810|A5T1000|T1000)
// RRRRRGGGGGIBBBBB
#define RGB(r,g,b)      (((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>3) | ((r&g&b&4)<<3))
#define T800_TO_T1000(x) ((((x&0x001F)<<11) | ((x&0x03E0)<<1) | ((x&0x7C00)>>10) | ((x&0x8000)>>10)))
#else  // T800
// IBBBBBGGGGGRRRRR
#define RGB(r,g,b)      (((r&0xF8)>>3) | ((g&0xF8)<<2) | ((b&0xF8)<<7) | ((r&g&b&4)<<13))
#endif

//  Color Defines (2 bytes)
#define COLOR_BLACK    RGB(0x00, 0x00, 0x00)
#define COLOR_WHITE    RGB(0xFF, 0xFF, 0xFF)
#define COLOR_RED      RGB(0xFF, 0x00, 0x00)
#define COLOR_YELLOW   RGB(0x00, 0xFF, 0xFF)
#define COLOR_GREEN    RGB(0x00, 0xFF, 0x00)
#define COLOR_BLUE     RGB(0x00, 0xFF, 0x00)
#define COLOR_BACKGR   RGB(0xB0, 0xC1, 0xF0) 
//0x7B36

#define STS_NORMAL    0
#define STS_OK        1
#define STS_ERROR     2
extern void ShowStatus(char *aMsg, DWORD aMode);
extern void DispPutStr(const char *aDispStr);
extern void ShowBWLogo(const BYTE *aLogoDat, DWORD aLogoSize);
struct GLOC {
  WORD wPosX;
  WORD wPosY;
  WORD wWidth;
  WORD wHeight;
};
extern void ShowGraphicT800(struct GLOC *aLoc, const BYTE *aDat);
extern void Disp2x16Msg(const BYTE *aMsg, DWORD aOffset, DWORD aCtrl);

//-----------------------------------------------------------------------------
//  Date/Time Handling Functions
//-----------------------------------------------------------------------------
struct DATETIME {
  BYTE  bCentury;
  BYTE  bYear;
  BYTE  bMonth;
  BYTE  bDay;
  BYTE  bHour;
  BYTE  bMinute;
  BYTE  bSecond;
};
extern void SetRTC(struct DATETIME *aDtg);
extern void ReadRTC(struct DATETIME *aDtg);
extern void ConvDateTime(BYTE *aBuf, struct DATETIME *aDtg, BOOLEAN aLongYear);
extern BYTE SetDTG(BYTE *aDateTime);

extern DWORD FreeRunMark(void);                   //25-07-17 JC ++
extern DWORD FreeRunElapsed(DWORD aStartMark);    //25-07-17 JC ++

//-----------------------------------------------------------------------------
//   IP Related
//-----------------------------------------------------------------------------
// IP related
#define EDITIP_CURSOR_MAX     3
#define EDITIP_COL            0
#define EDITPORT_CURSOR_MAX   3
#define EDITPORT_COL          0
#define IP_LEN                4

extern void ShowIp(unsigned char *aIp, int aPos, int aReverse);
extern BOOLEAN EditIp(BYTE *aIp, DWORD aPos);
extern BOOLEAN EditPort(DWORD *aPort, DWORD aPos);

//-----------------------------------------------------------------------------
//   Misc
//-----------------------------------------------------------------------------
extern void CheckPointerAddr(void *aPtr);
extern void MemFatalErr(void *aMemPtr);
extern DWORD FreeRunSec(void);
extern void ShowTxRxLed(void);
extern void DebugPoint(char *aStr);
extern void HotPlug(void);	                      //25-07-17 JC ++

//-----------------------------------------------------------------------------
// Structure file handling
//-----------------------------------------------------------------------------
extern BOOLEAN ReadSFile(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen);
extern BOOLEAN WriteSFile(DWORD aFileHandle, DWORD aOffset, const void *aBuf, DWORD aLen);

//-----------------------------------------------------------------------------
//   Data Handle
//-----------------------------------------------------------------------------
extern WORD WGet(void *aBuf);
extern DWORD DWGet(void *aBuf);

//-----------------------------------------------------------------------------
//   Network Status Checking
//-----------------------------------------------------------------------------
extern BOOLEAN WaitLanNetRdy(DWORD aTimeout, BOOLEAN aNoCancel);
extern BOOLEAN WaitWifiNetRdy(DWORD aTimeout, BOOLEAN aNoCancel);
extern BOOLEAN WaitMcpNetRdy(DWORD aTimeout, BOOLEAN aNoCancel);

#endif // _INC_SYSUTIL_H_


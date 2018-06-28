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

//-----------------------------------------------------------------------------
//  System Timer Usage
//-----------------------------------------------------------------------------

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
//  Date/Time Handling Functions
//-----------------------------------------------------------------------------
struct DATETIME {
  BYTE  b_century;
  BYTE  b_year;
  BYTE  b_month;
  BYTE  b_day;
  BYTE  b_hour;
  BYTE  b_minute;
  BYTE  b_second;
};
extern void SetRTC(struct DATETIME *aTDtg);
extern void ReadRTC(struct DATETIME *aTDtg);
extern void ConvDateTime(BYTE *aBuf, struct DATETIME *aTDtg, BOOLEAN aLongYear);
extern void ConvDateTimeT(BYTE *aBuf, struct DATETIME *aTDtg, BOOLEAN aLongYear);
extern BYTE SetDTG(BYTE *aDateTime);

extern DWORD FreeRunMark(void);
extern DWORD FreeRunElapsed(DWORD aStartMark);

//-----------------------------------------------------------------------------
//  Display Handling Functions.
//-----------------------------------------------------------------------------
#define RGB(r,g,b)      (((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>3) | ((r&g&b&4)<<3))
#define RGB_WHITE       RGB(255,255,255)
#define RGB_BLACK       RGB(0x00, 0x00, 0x00)
#define RGB_BACKGR      RGB(0xB0, 0xC1, 0xF0) 

extern void Disp2x16Msg(const BYTE *aMsg, DWORD aOffset, DWORD aCtrl);

//-----------------------------------------------------------------------------
//    Data Buffer related Functions
//-----------------------------------------------------------------------------
extern void ByteCopy(BYTE *dest, BYTE *src, DWORD len);
extern void CompressInputData(void *aDest, BYTE *aSrc, BYTE aLen);
extern void CompressInputFData(void *aDest, BYTE *aSrc,  BYTE aLen);
extern int PrintLcd2LptCtr(WORD aPosX, WORD aPosY, WORD aHPixel, WORD aVPixel);

//-----------------------------------------------------------------------------
//    Misc Functions
//-----------------------------------------------------------------------------
extern BYTE *GetMonthText(BYTE aMM);
extern void MemFatalErr(void *aMemPtr);
extern void DebugPoint(char *aStr);
extern void debugAsc(BYTE *aTitle, BYTE *aData, DWORD aLen);
extern void debugHex(BYTE *aTitle, BYTE *aData, DWORD aLen);

//-----------------------------------------------------------------------------
//     Amount related functions.
//-----------------------------------------------------------------------------
struct DISP_AMT {
  BYTE len;
  BYTE content[21];
};
extern BOOLEAN ConvAmount(DDWORD aAmt, struct DISP_AMT *aDispAmt, DWORD aDecPos, BYTE aCurrSym);

#endif // _SYSUTIL_H_


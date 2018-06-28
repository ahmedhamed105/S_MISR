//-----------------------------------------------------------------------------
//  File          : lptutil.h
//  Module        :
//  Description   : Declrartion & Defination for lptutil.c
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
#ifndef _INC_LPTUTIL_H_
#define _INC_LPTUTIL_H_
#include "hwdef.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
#if (PR608D|TIRO)
#define PRINT_WIDTH               30
#else
#define PRINT_WIDTH               32
#endif

//-----------------------------------------------------------------------------
//  Function API
//-----------------------------------------------------------------------------
extern DWORD LptOpen(void *aCfg);
extern void LptClose(void);
extern DWORD LptPutN(const BYTE *aDat, DWORD aLen);
extern DWORD LptPutS(const BYTE *aStr);
extern void  WaitPrnFinish(void);
extern DWORD Adv2Bookmark(DWORD aMaxSteps);
extern DWORD PrintLogo(DWORD aWidth, DWORD aHeight, const BYTE *aLogoPtr);
extern BOOLEAN PrintBuf(BYTE *aBuf, DWORD aLen, BOOLEAN aLoopPrint);
extern void PrintRcptLF(DWORD aCount);
extern void PrintRcptFF(void);

#endif // _INC_LPTUTIL_H_

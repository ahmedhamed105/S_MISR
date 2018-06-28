//-----------------------------------------------------------------------------
//  File          : SPGLib.h
//  Module        :
//  Description   : Header file for SPECTRA Graphic Library.
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
//  08 Jul  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_SPGLIB_H_
#define _INC_SPGLIB_H_
#include "common.h"
#include "system.h"

//----------------------------------------------------------------------------
//  Defines
//----------------------------------------------------------------------------
struct POINT {
  DWORD dwPosX;
  DWORD dwPosY;
};

//----------------------------------------------------------------------------
//  Generic API
//----------------------------------------------------------------------------
// Setup API
extern void GD_SetDotSize(DWORD aPixel);
extern void GD_SetDotColor(DWORD aRed, DWORD aGreen, DWORD aBlue);

// Graphic API
extern BOOLEAN GD_Dot(DWORD aPosX, DWORD aPosY);
extern BOOLEAN GD_Point(struct POINT *aPoint);
extern BOOLEAN GD_Line(struct POINT *aPt1, struct POINT *aPt2);
extern BOOLEAN GD_Rectangle(struct POINT *aPtTL, struct POINT *aPtBR);
extern BOOLEAN GD_Triangle(struct POINT *aPt1, struct POINT *aPt2, struct POINT *aPt3);
extern BOOLEAN GD_Circle(struct POINT *aCenter, DWORD aRadius);

// GIF API
extern BOOLEAN GIFLoadMem(const BYTE *aBuf, DWORD aLen);
extern void GIFUnLoad(void);
extern void GIFSetPos(WORD aPosX, WORD aPosY);
extern BOOLEAN GIFPlay(void);
extern BOOLEAN GIFPlayOn(WORD aPosX, WORD aPosY);
extern void GIFStop(BOOLEAN aRestore);


#endif      // _INC_GLIB_H_

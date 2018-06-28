//-----------------------------------------------------------------------------
//  File          : apmconfig.h
//  Module        :
//  Description   : Declrartion & Defination for apmconfig.c
//  Author        : Lewis
//  Notes         : N/A
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
#ifndef _INC_APMCONFIG_H_
#define _INC_APMCONFIG_H_
#include "common.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      API
//-----------------------------------------------------------------------------
extern void LcdSetup(void);
extern void KbdBacklight(void);
extern void DispTermIP(BYTE *aIP);
extern BOOLEAN ConfigTermIP(BYTE *aIP);
extern BOOLEAN ConfigTermIP2(BYTE *aIP);
extern BOOLEAN ToggleDHCP(BYTE *aDHCP, BYTE *aIP);
extern void SetCodecVol(void);

#endif // _INC_APMCONFIG_H_


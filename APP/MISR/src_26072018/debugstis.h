//-----------------------------------------------------------------------------
//  File          : debugstis.h
//  Module        :
//  Description   : Declrartion & Defination for debugstis.c
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |        Struct define : Leading T                                         |
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
//  19 Aug  2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_DEBUGSTIS_H_
#define _INC_DEBUGSTIS_H_
#include "common.h"

//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------
// STIS init mode
#define MODE_ALL_PARAM         0x01
#define MODE_EMV_PARAM_ONLY    0x02
#define MODE_EMV_KEY_ONLY      0x03
#define MODE_EMV_PARAM_KEY     0x04
#define MODE_EXTRA_PARAM       0x05


//-----------------------------------------------------------------------------
//      Function API
//-----------------------------------------------------------------------------
extern void SaveSTISDbgData(DWORD aInitMode);

#endif  // _INC_DEBUGSTIS_H_

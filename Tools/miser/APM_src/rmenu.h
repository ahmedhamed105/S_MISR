//-----------------------------------------------------------------------------
//  File          : rmenu.h
//  Module        :
//  Description   : Declrartion & Defination for rmenu.c
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
#ifndef _INC_RMENU_H_
#define _INC_RMENU_H_
#include "common.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
struct RMENUDAT {
  int   iID;
  BYTE  sbName[21];
};

#define RMNU_POWEROFF   0xFF

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      API
//-----------------------------------------------------------------------------
// Last item in the table with i_id = -1;
extern int RMenuSel(struct RMENUDAT *aTbl, BYTE *aIcon, BYTE *aIconDesc);

#endif // _INC_RMENU_H_


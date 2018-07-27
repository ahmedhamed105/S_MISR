//-----------------------------------------------------------------------------
//  File          : menu.h
//  Module        :
//  Description   : Declaration & defines for menu.c
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
#ifndef _INC_MENU_H_
#define _INC_MENU_H_
#include "common.h"

//----------------------------------------------------------------------------
//  Defines
//----------------------------------------------------------------------------
#define MAX_MENU_ITEMS      99

struct MENU_ITEM {
  int   iID;                            // unique id for menu item (+ve value)
  char  *pcDesc;                        // menu item description (max MW_MAX_LINE - 5)
  char  scDescDat[MW_MAX_LINESIZE-2];   // Display when *desc == NULL;
};

struct MENU_DAT
{
  char   scHeader[MW_MAX_LINESIZE-3];   // menu header
  const struct MENU_ITEM *psMenuItem;   // pointer to NULL terminate menu line
};

//----------------------------------------------------------------------------
//  Generic API
//----------------------------------------------------------------------------
extern int MenuSelect(const struct MENU_DAT *aMenu, int aCurrIndex);

#endif // _INC_MENU_H_

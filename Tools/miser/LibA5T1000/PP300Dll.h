//-----------------------------------------------------------------------------
//  File          : PP300DLL.h
//  Module        :
//  Description   : Header for for PP300DLL export functions.
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
//  29 Sept 2008  Lewis       Initial Version (with new CD nameing convention)
//-----------------------------------------------------------------------------
#ifndef _INC_PP300DLL_H_
#define _INC_PP300DLL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define PP300DLL_ID          0x7E

//-----------------------------------------------------------------------------
//   Function Number     
//-----------------------------------------------------------------------------
enum {
PP300_CMD,
PP300_FUNC_COUNT
};

//-----------------------------------------------------------------------------
//   Functions
//-----------------------------------------------------------------------------
extern int PP300Cmd(void *aDat, int aLen);
#define PP300Cmd(x, y)         (int) lib_app_call(PP300DLL_ID, PP300_CMD, (DWORD)x, (DWORD)y, 0)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //_INC_PP300DLL_H_


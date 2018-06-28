//-----------------------------------------------------------------------------
//  File          : dllexp.h
//  Module        : 
//  Description   : Header for for DllExp_61 Export Functions.
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
#ifndef _INC_DLLEXP_H_
#define _INC_DLLEXP_H_
#include <string.h>
#include "stdio.h"		
#include "common.h"
#include "system.h"


//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define DLLEXP_ID          0x61

//-----------------------------------------------------------------------------
//   TestMain Export Functions define 
//-----------------------------------------------------------------------------
enum {
  EXPORTFUNCDE,
  MAX_DLLEXP_FUNC
};

//extern void ExportFuncTM(void);
#define ExportFuncDE()        lib_app_call(DLLEXP_ID, EXPORTFUNCDE, (DWORD)0, (DWORD)0, (DWORD)0)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //_INC_DLLEXP_H_




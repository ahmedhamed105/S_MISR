//-----------------------------------------------------------------------------
//  File          : TPadTest.h
//  Module        :
//  Description   :
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
//  13 Dec  2012  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_TPADTEST_H_
#define _INC_TPADTEST_H_
#include <string.h>
#include "common.h"
#include "system.h"

//#define TPAD_SUPPORT    TRUE

//-----------------------------------------------------------------------------
//      Function API
//-----------------------------------------------------------------------------
extern void TPadTest(void);
extern BOOLEAN TPadTestEnable(void);
extern void SPadTest(void);
extern BOOLEAN SPadTestEnable(void);


#endif // _INC_TPADTEST_H_

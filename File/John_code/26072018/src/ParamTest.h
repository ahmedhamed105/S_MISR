//-----------------------------------------------------------------------------
//  File          : ParamTest.h
//  Module        :
//  Description   : Declaration & defines for ParamTest.c
//  Author        : John
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
//  01 Dec  2010  John        Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_PARAMTEST_H_
#define _INC_PARAMTEST_H_

//----------------------------------------------------------------------------
//  Defines
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  Generic API
//----------------------------------------------------------------------------
extern BOOLEAN DispParamFile(BYTE aFileNo);
extern BOOLEAN LptParamFile(BYTE aFileNo);


#endif      // _INC_PARAMTEST_H_

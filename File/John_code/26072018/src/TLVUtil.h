//-----------------------------------------------------------------------------
//  File          : TLVUtil.h
//  Module        :
//  Description   : Header for TLVUtil.c
//  Author        : Pody
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
//  XX Apr  2008  Pody        Initial Version.
//  25 Sept 2008  Lewis
//-----------------------------------------------------------------------------
#ifndef _INC_TLVUTIL_H_
#define _INC_TLVUTIL_H_
#include "common.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// API
//----------------------------------------------------------------------------
// Data Convertion Func
extern void DDWORD2bcd(BYTE *aDest, DDWORD aVal);
extern DDWORD bcd2DDWORD(BYTE *aDest, DWORD aLen);
//extern WORD WGet(BYTE *aSrc);
extern void WPut(BYTE *aDest, WORD aVal);

// TLV Related Func
extern WORD TlvSizeOf(BYTE *aTLV);
extern WORD TlvTSize(BYTE *aTLV);
extern WORD TlvTag(BYTE *aTLV);
extern WORD TlvLSize(BYTE *aTLV);
extern WORD TlvLen(BYTE *aTLV);
extern BYTE *TlvVPtr(BYTE *aTLV);
extern BYTE *TlvSeek(BYTE *aTlvList, WORD aLen, WORD aTag);
extern BYTE *TlvNext(BYTE *aTlvList);
extern BOOLEAN TlvIsCTag(WORD aTag);
extern WORD TlvPack(WORD aTag, WORD aLen, BYTE *aVar, BYTE *aOut);

//-----------------------------------------------------------------------------
#endif //_INC_TLVUTIL_H_




//-----------------------------------------------------------------------------
//  File          : EMVtrans.h
//  Module        :
//  Description   : Declrartion & Defination for EMVtrans.C
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
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _EMVTRANS_H_
#define _EMVTRANS_H_
#include "apm.h"

//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
extern BOOLEAN ValidEMVData(void);
extern void EMVFatal(void);
extern DWORD EMVTrans(DWORD aTrans);
extern BYTE *TagPut(BYTE *aBuf, WORD aTag);
extern TLIST *TagSeek(TLIST *aSrc, WORD aTag);

#endif

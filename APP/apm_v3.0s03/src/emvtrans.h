//-----------------------------------------------------------------------------
//  File          : EMVTrans.h
//  Module        :
//  Description   : Declrartion & Defination for EMVTrans.c
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
#include "common.h"

//-----------------------------------------------------------------------------
//  Common Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Functions
//-----------------------------------------------------------------------------
extern BYTE TagSize(BYTE *aBuf);
extern WORD TagGet(BYTE *aBuf);
extern BYTE TLSize(BYTE *aLD);
extern WORD TLGet(BYTE *aLD);
extern BOOLEAN LDLoad(BYTE *aDest, BYTE *aSrc, WORD aTag);
extern void ConvDecimal(BYTE *aBuf, BYTE aLen);

extern void ClrEMVKey(void);
extern void ClrEMVParam(void);
extern void SetEMVLoaded(BYTE aFlag);
extern void ClrEMVParam(void);
extern void EMVClrDnloadBuf (void);
extern void EMVKillDnloadBuf (void);
extern void EMVData2DnloadBuf (BYTE aType, BYTE *aData, WORD aLen);
extern void EMVDataUpdate (void);
extern BOOLEAN PrintEMVParam(BOOLEAN aDetail, BOOLEAN aToDisplay);
extern BOOLEAN EMVUtilStart(void);
extern void EMVInit(void);

#endif //_EMVTRANS_H_

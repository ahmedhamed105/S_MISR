//-----------------------------------------------------------------------------
//  File          : CTLCfg.h
//  Module        :
//  Description   : Declaration & Definition for CTLCfg.c
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
#ifndef _CTLCFG_H_
#define _CTLCFG_H_
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
extern void ClrCTLKey(void);
extern void ClrCTLParam(void);
extern void SetCTLLoaded(BYTE aFlag);
extern void ClrCTLParam(void);
extern void CTLClrDnloadBuf (void);
extern void CTLKillDnloadBuf (void);
extern void CTLData2DnloadBuf (BYTE aType, BYTE *aData, WORD aLen);
extern void CTLDataUpdate (void);
extern BOOLEAN PrintCTLParam(BOOLEAN aDetail, BOOLEAN aToDisplay);
extern BOOLEAN CTLUtilStart(void);

#endif //_CTLCFG_H_

//-----------------------------------------------------------------------------
//  File          : iface_util.h
//  Module        :
//  Description   : header for iface_util.c
//  Author        : Pody
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g                                         |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |            Char size : Leading c                                         |
// |             Int size : Leading i                                         |
// |            Byte size : Leading b                                         |
// |            Word size : Leading w                                         |
// |           Dword size : Leading d                                         |
// |          DDword size : Leading dd                                        |
// |                Array : Leading a, (ab = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  2008 Apr      Pody        Initial Version.
//-----------------------------------------------------------------------------


#ifndef _IFACE_UTIL_H_
#define _IFACE_UTIL_H_

#include "system.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Generic API
//----------------------------------------------------------------------------
// data convertion func
extern DWORD UGet(const BYTE *x);
extern void UPut(BYTE *x, DWORD y);
extern WORD WGet(const BYTE *x);
//extern void WPut(BYTE *x, WORD y);

extern void Des(BYTE *aKey, BYTE *aInOut, WORD aLen);
extern void Des2(BYTE *aKey, BYTE *aInOut, WORD aLen);
extern void TDes(BYTE *aDKey, BYTE *aInOut, WORD aLen);
extern void TDes2(BYTE *aDKey, BYTE *aInOut, WORD aLen);

extern WORD Crc16(BYTE *aData, WORD aLen, WORD aInitial);
extern void Random(BYTE *aOut, WORD aLen);

//-----------------------------------------------------------------------------
#endif //_IFACE_UTIL_H_




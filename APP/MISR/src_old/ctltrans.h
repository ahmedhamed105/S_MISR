//-----------------------------------------------------------------------------
//  File          : ctltrans.h
//  Module        :
//  Description   : Declaration & Definition for ctltrans.C
//  Author        : John
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
//  21 Oct 2010   John        Initial Version.
//-----------------------------------------------------------------------------
#ifndef _CTLTRANS_H_
#define _CTLTRANS_H_
#include "apm.h"

#define CTL_LED_IDLE        0
#define CTL_LED_TAP_CARD    1
#define CTL_LED_PROC        2
#define CTL_LED_TC          3
#define CTL_LED_ARQC        4
#define CTL_LED_AAC         5
#define CTL_LED_RM_CARD     6
#define CTL_LED_TRANS_IDLE  7

#define DP_LOGO_LANDINGZONE	0

//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
extern BOOLEAN CL_Init(void);
extern BOOLEAN ValidCTLData(BOOLEAN aCheckCard);
extern void CTLFatal(void);
extern DWORD CTLTrans(DWORD aTrans);
extern BOOLEAN CtlUnderTransLimit(DWORD aAmount);

extern BOOLEAN CTLPreProcess(DWORD aTrans);
extern BOOLEAN CTLWaitCard(void);
extern int CL_Close(DWORD aWaitRemove10ms);
extern void CL_Off(void);
extern void CL_LedSet(DWORD aSet);
extern void CL_Led(BYTE *aIdOnOffCycle);
extern int DispDefinedLogo(DWORD aIdx, BOOLEAN aIsCheckAvailOnly);

#endif

//-----------------------------------------------------------------------------
//  File          : auxcom.h
//  Module        :
//  Description   : Declaration & Definition for auxcom.C
//  Author        : John
//  Notes         : N/A
//
//  Naming conventions
//  ~~~~~~~~~~~~~~~~~~
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  21 Feb  2011  John        Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_AUXCOM_H_
#define _INC_AUXCOM_H_
#include "common.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//    Functions
//-----------------------------------------------------------------------------
extern BOOLEAN AuxOpen(void);
extern BOOLEAN AuxClose(void);
extern DWORD AuxRead(BYTE *aBuf, DWORD aLen);
extern BOOLEAN AuxWrite(BYTE *aBuf, DWORD aLen);
extern BOOLEAN AuxRxRdy(void);
extern BOOLEAN AuxTxRdy(void);
extern void AuxEnable(void);
extern void AuxDisable(void);
extern void dbgHex(BYTE *aTitle, BYTE *aData, DWORD aLen);

#endif // _INC_AUXCOM_H_


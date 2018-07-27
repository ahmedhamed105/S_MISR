//-----------------------------------------------------------------------------
//  File          : print.h
//  Module        :
//  Description   : Declrartion & Defination for PRINT.C
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
#ifndef _PRINT_H_
#define _PRINT_H_
#include "common.h"
#include "corevar.h"

//-----------------------------------------------------------------------------
// Receipt control CODE
// 1. KRcptMark must pack for transaction receipt.
// 2. Data between two skip mark will not print for customer copy
// 3. logo mark will replace with logo data download from stis
// 4. replace mark will replace by some constant message.
//-----------------------------------------------------------------------------
extern const BYTE KLogoMark[3]     ;
extern const BYTE KConstLogoMark[3];
extern const BYTE KRcptMark[3]     ;
extern const BYTE KSkipMark[3]     ;
extern const BYTE KReplaceMark[3]  ;
extern const BYTE KDuplicateMark[3];
extern const BYTE KSignatureMark[3];

extern const BYTE KHalfDotMode[3];   // ESC D 1
extern const BYTE KFullDotMode[3];   // ESC D 0

#define MAX_CHAR_LINE_NORMAL    30
#define MAX_CHAR_LINE_SMALL     48

/****** slip type ******/
#define CUS_SLIP        0
#define REPRINT_SLIP    1

extern void PackLogo(void);
extern void PackDtg(struct DATETIME *aDtg);
extern void PackCurrentDate(void);
extern void PackSeperator(BYTE aChar);
extern void PackRcptHeader();
extern BYTE PackAmt(DDWORD aAmount, BYTE aOffset);
extern void PackFF(void);
extern void PackTotals(struct TOTAL_STRUCT *aTot);
extern void PackEndOfRep(void);
extern void SaveLastRcpt(BYTE *aDat, WORD aLen);
extern int GetLastRcpt(BYTE *aDat, WORD aLen);
extern void ReprintLast(void);
extern void ManualFeed(void);
#endif // _PRINT_H_

//-----------------------------------------------------------------------------
//  File          : termdata.h
//  Module        :
//  Description   : Declrartion & Defination for termdata.c
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
#ifndef _INC_TERMDATA_H_
#define _INC_TERMDATA_H_
#include "common.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//  Common Defines
//-----------------------------------------------------------------------------
// LCD contrast define
#define MAX_LCD_CONTRAST    34
#define DEF_LCD_CONTRAST    12

// Set Mode option
#define TOGGLE_TRAINING     0x00
#define TOGGLE_DEBUG        0x01

#define APPL_UPLOAD_PENDING   0x01
#define APPL_UPLOAD_DONE      0x00

//-----------------------------------------------------------------------------
//  Functions
//-----------------------------------------------------------------------------
// Terminal Data
extern void CleanTermData(void);
extern BOOLEAN UpdTermData(struct TERM_DATA *aDat);
extern BOOLEAN GetTermData(struct TERM_DATA *aDat);
extern void CreateTermData(void);
extern void CloseTermData(void);
extern void TermDataInit(void);
extern BYTE GetSTISMode(void);
extern void SetSTISMode(BOOLEAN aOn);
extern void SetDebugMode(BYTE aMode);
extern BOOLEAN DebugModeON(void);
extern BOOLEAN TrainingModeON(void);
void SetLcdContrast(void);
extern DWORD TermDataUpd(void *aDat);

extern void SetupSTIS(void);
extern void SetModeOnOff(DWORD aWhich);
extern void AdjustContrast(void);
extern void AdjustVol(void);
extern BOOLEAN UpdateFData(BYTE *aDest, BYTE aLen);
extern BOOLEAN UpdatePABX(void);
extern void GetPortDevName(DWORD aPort, char *aFileName);
extern void SetupTerm(void);
extern void SetROC(void *aNewROC);
extern void SetTrace(void *aNewTrace);
extern void GetTrace(void *aTrace);
extern void IncTrace(void);

extern void STISFTPSetup(void);
extern BYTE AppListUploadReq(void);
extern void SetAppListUploadReq(BYTE aUploadReq);
extern WORD GetMirrorAppListChksum(void);
extern WORD GetActualAppListChksum(void);
extern void UpdateAppListChksum(void);

// Param Date Checksum functions
extern BOOLEAN CompareEDCDateChksum(BYTE *aChksum, BYTE *aDate);
extern void SetEDCDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize);
extern BOOLEAN CompareEMVAPPDateChksum(BYTE *aChksum, BYTE *aDate);
extern void SetEMVAPPDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize);
extern BOOLEAN CompareEMVKEYDateChksum(BYTE *aChksum, BYTE *aDate);
extern void SetEMVKEYDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize);
extern BOOLEAN CompareXAPPDateChksum(BYTE *aChksum, BYTE *aDate);
extern void SetXAPPDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize);
extern BOOLEAN CompareLXAPPDateChksum(BYTE *aChksum, BYTE *aDate);
extern void SetLXAPPDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize);

// Access Control
extern void GenDLL(void);
extern BOOLEAN GetAccessCode(void);
extern void PrintSTIS(BOOLEAN aDetail, BOOLEAN aToDisplay);

#endif //_INC_TERMDATA_H_

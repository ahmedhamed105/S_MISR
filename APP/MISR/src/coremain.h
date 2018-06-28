//-----------------------------------------------------------------------------
//  File          : coremain.h
//  Module        :
//  Description   :
//  Author        : Auto Generate by coremain_h.bat
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
//  25 Jul  2017  John        Modify hardware default off on startup (EDC v3.1)
//-----------------------------------------------------------------------------
#ifndef _INC_COREMAIN_H_
#define _INC_COREMAIN_H_
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <string.h>
#include "common.h"
#include "system.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
extern const BYTE  KSoftName[];
extern const DWORD KExportFunc[];
extern BOOLEAN gDispLogo;
extern DWORD gIdleCnt;                  //25-07-17 JC ++
extern int fuctiongo;

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------
extern int VirginInit(void);
extern void GlobalsInit(void);
extern void GlobalsCleanUp(void);
extern int Coremain(DWORD aParam1, DWORD aParam2, DWORD aParam3);
extern void TransModeDisp(BOOLEAN aIdleMode);

//-----------------------------------------------------------------------------
//      Defines will be called by cstartup.c as well.
//			** Don't change the names!! **
//-----------------------------------------------------------------------------
#define MY_APPL_ID               0x11           // application ID
#if (T300|A5T1000|T1000)
#define SOFT_VERSION             0x03           // Software Version
#else
#define SOFT_VERSION             0x01           // Software Version
#endif
#define SOFT_SUBVERSION          0x01           // Software Sub Version
#define APPL_TYPE                0x00           // 0x00=Application 0x01=DLL
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif // _INC_COREMAIN_H_

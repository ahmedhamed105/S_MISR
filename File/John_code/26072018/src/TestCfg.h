//-----------------------------------------------------------------------------
//  File          : testcfg.h
//  Module        :
//  Description   : Test Key & Config header file.
//  Author        : Lewis
//  Notes         : N/A
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
//  15 Jan  2009  Lewis       Add External PP300 support
//-----------------------------------------------------------------------------
#ifndef _INC_TESTCFG_H_
#define _INC_TESTCFG_H_
#include "common.h"
#include "emv2dll.h"
#include "emvcl2dll.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define DEMOTEST          1
//#define EMVL2CFG          1

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef DEMOTEST
#define PCONFIG         1
#define MAX_AID_SUPPORT 5
#define KEY_ROOM_SIZE   11
#define MAX_CTL_AID_SUPPORT   5
#define CTL_KEY_ROOM_SIZE     17
struct EMV_TEST_CFG {
  BYTE     cConfig;
  WORD     chksum;
};

extern struct EMV_TEST_CFG gEMVTestCfg;

extern const APP_CFG  KApplCfg[PCONFIG][MAX_AID_SUPPORT];
extern const KEY_ROOM KKeyRoom[];

extern const CL_CFGP  KCtlApplCfg[PCONFIG][MAX_CTL_AID_SUPPORT];
extern const CL_KEY_ROOM KCtlKeyRoom[];

#endif   // DEMOTEST
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
#ifdef EMVL2CFG
#define PCONFIG         2
#define MAX_AID_SUPPORT 10
#define KEY_ROOM_SIZE   20
struct EMV_TEST_CFG {
  BYTE     cConfig;
  WORD     chksum;
};

extern struct EMV_TEST_CFG gEMVTestCfg;

extern const APP_CFG  KApplCfg[PCONFIG][MAX_AID_SUPPORT];
extern const KEY_ROOM KKeyRoom[];

#endif   // EMVL2CFG
//
//-----------------------------------------------------------------------------

// Debug Auto Run
extern void DebugTestSetup(void);
extern BOOLEAN DebugAutoRun(void);
extern int   GetAutoTrans(void);
extern DWORD GetAutoAmount(void);
extern DWORD GetAutoPin(void);
extern BOOLEAN DebugEnd(void);
extern void LoadEMVKey(void);
extern void LoadEMVCfg(void);
extern void ExtPPadSetup(void);
extern void LoadCTLKey(void);
extern void LoadCTLCfg(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif // _INC_TESTCFG_H_

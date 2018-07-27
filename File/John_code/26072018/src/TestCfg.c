//-----------------------------------------------------------------------------
//  File          : TestCfg.c
//  Module        :
//  Description   : Include Debug Test Config Setup.
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
//  09 Apr  2008  Lewis       Initial Version.
//  15 Jan  2009  Lewis       Add External PP300 support
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apm.h"
#include "menu.h"
#include "corevar.h"
#include "emvtrans.h"
#include "ctltrans.h"
#include "testcfg.h"

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Globals
//-----------------------------------------------------------------------------
static BOOLEAN bDebugAuto;
static int     iDebugAutoTrans;
static DWORD   dDebugAutoAmt;
static DWORD   dDebugAutoPin;
static DWORD   dDebugMaxAmt;

////*****************************************************************************
////  Function        : GetTermCap
////  Description     : Get terminal capabality from input buffer.
////  Input           : aDat;     // pointer to TLV data.
////  Return          : capabality byte.
////  Note            : N/A
////  Globals Changed : N/A
////*****************************************************************************
//static BYTE GetTermCap(BYTE *aDat)
//{
//  BYTE *ptr;
//  ptr = TLFind(aDat, 0x9F40);   // additional terminal capabality.
//  return ptr[1];
//}
////*****************************************************************************
////  Function        : DebugTestSetup
////  Description     : Setup debug auto run option.
////  Input           : N/A
////  Return          : N/A
////  Note            : N/A
////  Globals Changed : N/A
////*****************************************************************************
//void DebugTestSetup(void)
//{
//  BYTE term_cap;
//  struct MENU_ITEM eTransItem[7];
//  struct MENU_DAT  eTransMenu;
//  BYTE   menu_idx;
//  int    select;
//  BYTE   kbdbuf[32];
//
//  term_cap = GetTermCap((BYTE *)KApplCfg[gEMVTestCfg.cConfig][0].eACFG);
//
//  menu_idx = 0;
//  // Add Manual option
//  eTransItem[menu_idx].id = menu_idx+1;
//  eTransItem[menu_idx].desc = "Manual Select";
//  menu_idx++;
//
//  if (term_cap & 0x40) {
//    eTransItem[menu_idx].id = menu_idx+1;
//    eTransItem[menu_idx].desc = "Goods Sale";
//    menu_idx++;
//  }
//  if (term_cap & 0x20) {
//    eTransItem[menu_idx].id = menu_idx+1;
//    eTransItem[menu_idx].desc = "Services Sale";
//    menu_idx++;
//  }
//  if (term_cap & 0x80) {
//    eTransItem[menu_idx].id = menu_idx+1;
//    eTransItem[menu_idx].desc = "Cash Withdraw";
//    menu_idx++;
//  }
//  if (((term_cap & 0x50) == 0x50) || ((term_cap & 0x30) == 0x30)) {
//    eTransItem[menu_idx].id = menu_idx+1;
//    eTransItem[menu_idx].desc = "CashBack";
//    menu_idx++;
//  }
////  if (gPBOCApp) {
////    eTransItem[menu_idx].id = menu_idx+1;
////    eTransItem[menu_idx].desc = "Read Card Log";
////    menu_idx++;
////  }
//  eTransItem[menu_idx].id = -1;
//  eTransItem[menu_idx].desc = NULL;
//
//  eTransMenu.line_mode = MENU_MODE_6LINE;
//  memcpy(eTransMenu.header, "AUTO RUN CONFIG  ", 17);
//
//  eTransMenu.menu_item = eTransItem;
//  select = MenuSelect(&eTransMenu);
//  if (select == -1)
//    return;
//
//  if (select == 1) {
//    bDebugAuto = FALSE;
//    dDebugMaxAmt = 0;
//  }
//  else {
//    bDebugAuto = TRUE;
//    iDebugAutoTrans = select-1;
//    dDebugAutoAmt = 100;
//    dDebugAutoPin = 0;
//    printf("\fTxn Count :");
//    kbdbuf[0]  = 0;
//    dDebugMaxAmt = 0;
//    if (APM_GetKbd(NUMERIC_INPUT, 4, kbdbuf)) {
//      kbdbuf[kbdbuf[0]+1] = 0;
//      dDebugMaxAmt = dDebugAutoAmt + atoi((char *)&kbdbuf[1]);
//    }
//  }
//}
//*****************************************************************************
//  Function        : DebugAutoRun
//  Description     : Return Debug Auto run status;
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DebugAutoRun(void)
{
  return bDebugAuto;
}
//*****************************************************************************
//  Function        : GetAutoTrans
//  Description     : Return Auto run Transaction Selection.
//  Input           : N/A
//  Return          : select index.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetAutoTrans(void)
{
  return iDebugAutoTrans;
}
//*****************************************************************************
//  Function        : GetDebugAmount
//  Description     : Return Debug Auto run Amount
//  Input           : N/A
//  Return          : Amount value;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetAutoAmount(void)
{
  dDebugAutoAmt++;
  return dDebugAutoAmt;
}
//*****************************************************************************
//  Function        : GetAutoPin
//  Description     : Return Auto run PIN.
//  Input           : N/A
//  Return          : PIN value;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetAutoPin(void)
{
  dDebugAutoPin++;
  return dDebugAutoPin;
}
//*****************************************************************************
//  Function        : DebugEnd
//  Description     : Return debug end condition
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DebugEnd(void)
{
  return (dDebugMaxAmt <= dDebugAutoAmt)?TRUE:FALSE;
}
//*****************************************************************************
//  Function        : KernelReady
//  Description     : Check Whether EMV/CTL Kernel exist
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN KernelReady(BYTE aAppID)
{
  if (!IsAppIdExistMW(aAppID)) {
    return FALSE;
  }
  return TRUE;
}
//*****************************************************************************
//  Function        : DoSetup
//  Description     : Send the setup command to EMV Kernel.
//  Input           : aCmd;             // Setup Command
//  Return          : N/A
//  Note            :
//  Globals Changed : N/A
//*****************************************************************************
static void DoSetup(DWORD aCmd)
{
  if (EMV_DllSetup(aCmd,  &gGDS->s_EMVIn, &gGDS->s_EMVOut) == FALSE) {
    if (aCmd !=IOCMD_READ_AIDIDX) {
      EMVFatal();
    }
    else
      gGDS->s_EMVOut.wLen = 0;
  }
}
//*****************************************************************************
//  Function        : ClrEMVKey
//  Description     : Clear All EMV Keys.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ClrEMVKey(void)
{
  if (!KernelReady(EMVDLL_ID))
    return;

  memset(gGDS->s_EMVIn.bMsg, 0, 6);
  gGDS->s_EMVIn.wLen = 6;
  if (EMV_DllSetup(IOCMD_KEY_LOADING, &gGDS->s_EMVIn, &gGDS->s_EMVOut) == FALSE)     // clear all key
    EMVFatal();
}
//*****************************************************************************
//  Function        : ClrEMVParam
//  Description     : Clear all EMV dll's parameters.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ClrEMVParam(void)
{
  if (!KernelReady(EMVDLL_ID))
    return;

  os_config_read(K_CF_SerialNo, gGDS->s_EMVIn.bMsg);
  gGDS->s_EMVIn.wLen = 8;

  if (EMV_DllSetup(IOCMD_IFDSN_RW, &gGDS->s_EMVIn, &gGDS->s_EMVOut) == FALSE) // set IFDSN
    EMVFatal();

  EMV_DllSetup(IOCMD_CLR_AID, &gGDS->s_EMVIn, &gGDS->s_EMVOut); // clear AID parameters
}
//*****************************************************************************
//  Function        : LoadEMVKey
//  Description     : Load Debug EMV keys.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void LoadEMVKey(void)
{
  BYTE i, room_size;
  const KEY_ROOM *key_room;

  ClrEMVKey();
  room_size = KEY_ROOM_SIZE;
  key_room  = KKeyRoom;

  for (i = 0; i < room_size; ++i) {
    memcpy(gGDS->s_EMVIn.bMsg, &key_room[i], sizeof(KEY_ROOM));
    gGDS->s_EMVIn.wLen = sizeof(KEY_ROOM);
    DoSetup(IOCMD_KEY_LOADING);  // Key Loading
  }
}
//*****************************************************************************
//  Function        : EMVDLLSetup
//  Description     : Setup EMVDLL
//  Input           : aConfirm;       // TRUE => Prompt for confirm & set IFNSN
//  Return          : N/A
//  Note            :
//  Globals Changed : N/A
//*****************************************************************************
void LoadEMVCfg(void)
{
  BYTE i, max_aid;

  max_aid = MAX_AID_SUPPORT;
  ClrEMVParam();
  //DoSetup(IOCMD_CLR_AID); // Clear AID parameters
  gEMVTestCfg.cConfig = 0;
  for (i = 0; i < max_aid; ++i) {
    memcpy(gGDS->s_EMVIn.bMsg, &KApplCfg[gEMVTestCfg.cConfig][i], sizeof(APP_CFG));
    gGDS->s_EMVIn.wLen = sizeof(APP_CFG);
    DoSetup(IOCMD_LOAD_AID);  // Load AID parameters
  }
  gGDS->s_EMVIn.wLen = 0;
  DoSetup(IOCMD_CLOSE_LOAD);  // End AID parameter load
}
//*****************************************************************************
//  Function        : ExtPPadSetup
//  Description     : Setup EMVDLL to use External PPad for PIN input.
//  Input           : N/A
//  Return          : N/A
//  Note            :
//  Globals Changed : N/A
//*****************************************************************************
void ExtPPadSetup(void)
{
  struct TERM_DATA term_data;
  BYTE   pinpad;

  pinpad = 0;  // default disable
  if (APM_GetTermData(&term_data)) {
    if (term_data.b_ppad_port!=0) {
      pinpad = MIDWARE_ID;
    }
  }
  gGDS->s_EMVIn.wLen = 3;
  gGDS->s_EMVIn.bMsg[0] = pinpad;
  gGDS->s_EMVIn.bMsg[1] = SCHNLGETPINMW;
  gGDS->s_EMVIn.bMsg[2] = MW_PIN_BOC;   // Use PP300 default get pin mode
  DoSetup(IOCMD_SET_PINPAD);
}
//*****************************************************************************
//  Function        : DoSetup
//  Description     : Send the setup command to EMV Kernel.
//  Input           : aCmd;             // Setup Command
//  Return          : N/A
//  Note            :
//  Globals Changed : N/A
//*****************************************************************************
static void DoCTLSetup(DWORD aCmd)
{
  if (emvclSetup(aCmd,  &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE) {
    if (aCmd != CLCMD_READ_AIDIDX) {
      CTLFatal();
    }
    else
      gGDS->s_CTLOut.wLen = 0;
  }
}
//*****************************************************************************
//  Function        : ClrCTLKey
//  Description     : Clear All Contactless Keys.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ClrCTLKey(void)
{
  if (!KernelReady(EMVCLDLL_ID))
    return;

  memset(gGDS->s_CTLIn.pbMsg, 0, 6);
  gGDS->s_CTLIn.wLen = 6;
  if (emvclSetup(CLCMD_KEY_LOADING, &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE)     // clear all key
    CTLFatal();
}
//*****************************************************************************
//  Function        : ClrCTLParam
//  Description     : Clear all Contactless dll's parameters.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ClrCTLParam(void)
{
  if (!KernelReady(EMVCLDLL_ID))
    return;

  os_config_read(K_CF_SerialNo, gGDS->s_CTLIn.pbMsg);
  gGDS->s_CTLIn.wLen = 8;

  if (emvclSetup(CLCMD_IFDSN_RW, &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE) // set IFDSN
    CTLFatal();

  emvclSetup(CLCMD_CLR_AID, &gGDS->s_CTLIn, &gGDS->s_CTLOut); // clear AID parameters
}
//*****************************************************************************
//  Function        : LoadCTLKey
//  Description     : Load Debug Contactless keys.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void LoadCTLKey(void)
{
  BYTE i, room_size;
  const CL_KEY_ROOM *key_room;

  ClrCTLKey();
  room_size = CTL_KEY_ROOM_SIZE;
  key_room  = KCtlKeyRoom;

  for (i = 0; i < room_size; ++i) {
    memcpy(gGDS->s_CTLIn.pbMsg, &key_room[i], sizeof(CL_KEY_ROOM));
    gGDS->s_CTLIn.wLen = sizeof(CL_KEY_ROOM);
    DoCTLSetup(CLCMD_KEY_LOADING);  // Key Loading
  }
}
//*****************************************************************************
//  Function        : LoadCTLCfg
//  Description     : Setup EMVCLDLL
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void LoadCTLCfg(void)
{
  BYTE i, max_aid;

  max_aid = MAX_CTL_AID_SUPPORT;
  ClrCTLParam();
  //DoSetup(IOCMD_CLR_AID); // Clear AID parameters
  gEMVTestCfg.cConfig = 0;
  for (i = 0; i < max_aid; ++i) {
    memcpy(gGDS->s_CTLIn.pbMsg, &KCtlApplCfg[gEMVTestCfg.cConfig][i], sizeof(CL_CFGP));
    gGDS->s_CTLIn.wLen = sizeof(CL_CFGP);
    DoCTLSetup(CLCMD_LOAD_AID);  // Load AID parameters
  }
  gGDS->s_CTLIn.wLen = 0;
  DoCTLSetup(CLCMD_CLOSE_LOAD);  // End AID parameter load
}

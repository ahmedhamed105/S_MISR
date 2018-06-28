//-----------------------------------------------------------------------------
//  File          : apmfunc.c
//  Module        :
//  Description   : APM Functions.
//  Author        : Lewis
//  Notes         : N/A
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "hwdef.h"
#include "midware.h"
#include "hardware.h"
#include "kbdutil.h"
#include "sysutil.h"
#include "toggle.h"
#include "menu.h"
#include "message.h"
#include "termdata.h"
#include "apmtest.h"
#include "apmconfig.h"
#include "stis.h"
#include "keydlltest.h"
#include "tranutil.h"
#include "inittran.h"
#include "emvtrans.h"
#include "batchsys.h"
#include "coremain.h"
#include "apmfunc.h"
#include "wifi.h"
#include "comm.h"
#include "ftptran.h"
#include "param.h"
#include "ctlcfg.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
//  Application control related
#define MAX_APPL_SUPPORT    128
struct APPL_LIST {
  BOOLEAN    bActive;
  struct MW_APPL_INFO sAppInfo;
};
static struct APPL_LIST *gAppList;
static DWORD  gApplCount;

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
// Default APM Function Menu
static const struct MENU_ITEM KApmFuncItem [] =
{
  {  0, "STIS Init"},
  {  2, "STIS Setup"},
  #if (LAN_SUPPORT)             // Condition Compile for LAN Support
  {  3, "Set Term. IP"},
  #endif
  {  7, "TMS FTP Setup"},       //29-09-16 JC ++
  {  8, "TMS FTP Dnload"},      //29-09-16 JC ++
  {  9, "TMS Load Param"},      //29-09-16 JC ++
  #if (GPRS_SUPPORT)            // Condition Compile for GPRS Support
  { 10, "GPRS/WIFI Setup"},
  #endif
  { 11, "Show STIS"},
  #if (LPT_SUPPORT|TMLPT_SUPPORT)  // Condition Compile for Printer Support
  { 12, "Print STIS"},
  #endif
  { 61, "Training Mode"},
  { 63, "Debug Mode"},
  { 64, "Reboot"},
  { 80, "Cal Check Digit"},
  { 81, "LCD Setup"},
  #if (CDC_SUPPORT)           // Condition Compile for Codec Support
  { 82, "Codec Vol"},
  #endif
  #if (T300|T810|A5T1000|T1000|T800)
  { 83, "KBD BackL"},
  #endif
  { 84, "KeyDLL Test"},
  { 89, "Malloc Test"},
  #ifdef EXT_PPAD_PORT        // Condition compile for External PINPad Support
  { 93, "Setup ExtPPad"},
  #endif
  { 94, "Update PABX"},
  { 98, "Term. Setup"},
  { 99, "Clean Up"},
  {-1, NULL},
};

static const struct MENU_DAT KApmFuncMenu  = {
  "APM Functions",
  KApmFuncItem,
};
// Clean Up Menu
static const struct MENU_ITEM KCleanUpFuncItem [] =
{
  {  1, "STIS Param"},
  {  2, "EMV Param"},
  {-1, NULL},
};

static const struct MENU_DAT KCleanUpMenu  = {
  "    Clean Up     ",
  KCleanUpFuncItem,
};

#if (LAN_SUPPORT)
// TermIP Menu
static const struct MENU_ITEM KTermIPFuncItem [] =
{
  {  1, "TermIP"},
  {  2, "DHCP"},
  {-1, NULL},
};

static const struct MENU_DAT KTermIPMenu  = {
  "  TERM IP SETUP  ",
  KTermIPFuncItem,
};

//*****************************************************************************
//  Function        : TermIPFunc
//  Description     : Config Terminal IP Address.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void TermIPFunc(void)
{
  int  func_sel;
  struct MW_NIF_INFO netinfo;
  BYTE    dhcp;
  BYTE    ip[12];
  BOOLEAN reboot_required=FALSE;


  func_sel = MenuSelect(&KTermIPMenu, 0);
  if (func_sel == -1)
    return;

  #if (DOCK_SUPPORT)
  bs_config_read(K_CF_BS_DHCPEnable, &dhcp);
  if (dhcp) {
    NetInfoMW(MW_NIF_ETHERNET, &netinfo);
    memcpy(ip, (BYTE *)&netinfo.d_ip, 12);
  }
  else {
    bs_config_read(K_CF_BS_IP,      &ip[0]);
    bs_config_read(K_CF_BS_Netmask, &ip[4]);
    bs_config_read(K_CF_BS_Gateway, &ip[8]);
  }
  #else
  GetSysCfgMW(MW_SYSCFG_DHCP_ENABLE, &dhcp);
  dhcp = (dhcp != 0)? 1: 0;
  if (dhcp) {
    NetInfoMW(MW_NIF_ETHERNET, &netinfo);
    memcpy(ip, (BYTE *)&netinfo.d_ip, 12);
  }
  else {
    GetSysCfgMW(MW_SYSCFG_IP,      &ip[0]);
    GetSysCfgMW(MW_SYSCFG_NETMASK, &ip[4]);
    GetSysCfgMW(MW_SYSCFG_GATEWAY, &ip[8]);
  }
  #endif

  switch (func_sel) {
    case 1:
      if (ConfigTermIP(ip))
        reboot_required=TRUE;
      break;
    case 2:
      if (ToggleDHCP(&dhcp, ip))
        reboot_required=TRUE;
      break;
  }
  if (reboot_required) {
    #if (DOCK_SUPPORT)
    bs_config_write(K_CF_BS_DHCPEnable, &dhcp, 1);
    bs_config_write(K_CF_BS_IP,      &ip[0], 4);
    bs_config_write(K_CF_BS_Netmask, &ip[4], 4);
    bs_config_write(K_CF_BS_Gateway, &ip[8], 4);
    bs_config_update();
    DispLineMW("Reboot BASE", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
    DispLineMW("PLEASE WAIT", MW_LINE7, MW_CENTER|MW_REVERSE|MW_BIGFONT);
    bs_reboot();
    // wait base ready.
    while (bs_loopback(ip, sizeof(ip), ip) == 0) {
      Delay1Sec(1, TRUE);
    } 
    #else
    PutSysCfgMW(MW_SYSCFG_DHCP_ENABLE, &dhcp);
    PutSysCfgMW(MW_SYSCFG_IP, &ip[0]);
    PutSysCfgMW(MW_SYSCFG_NETMASK, &ip[4]);
    PutSysCfgMW(MW_SYSCFG_GATEWAY, &ip[8]);
    UpdSysCfgMW();
    Disp2x16Msg(KReboot, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    if (WaitKey(KBD_TIMEOUT) == MWKEY_ENTER) {
      ResetMW();
    }
    #endif
  }
}
#endif //(LAN_SUPPORT)
//*****************************************************************************
//  Function        : CleanUp
//  Description     : Clean up init data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CleanUp(void)
{
  int func_sel;

  func_sel = MenuSelect(&KCleanUpMenu, 0);
  if (func_sel == -1)
    return;

  switch (func_sel) {
    case 1:
      DispLineMW(KResetAppData, MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      DispLineMW(KConfirm, MW_LINE5, MW_BIGFONT);
      if (ToggleYesNo() == 2) {   // Enter Pressed
        DispClrBelowMW(MW_LINE3);
        Disp2x16Msg(KADataReset, MW_LINE3, MW_BIGFONT);
        fCommitAllMW();
        CloseTermData();
        CloseSTISFiles();
        CloseExParamFiles();

        CreateTermData();
        CreateSTISFiles();
        CreateExParamFiles();
        fCommitAllMW();

        DispClrBelowMW(MW_LINE3);
        DispLineMW(KDataReseted, MW_LINE5, MW_CENTER|MW_BIGFONT);
        AcceptBeep();
        Delay1Sec(2, 0);
      }
      break;
    case 2:               // 01-08-07++ BF (3)
      DispLineMW(KResetEMVData, MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      DispLineMW(KConfirm, MW_LINE5, MW_BIGFONT);
      if (ToggleYesNo() == 2) { // Enter Pressed
        ClrEMVParam();    // Reset EMV param
        ClrEMVKey();   // Reset EMV key
        ClrCTLParam();    // Reset EMV Contactless param
        ClrCTLKey();   // Reset EMV Contactless key
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KDataReseted, MW_LINE5, MW_CENTER|MW_BIGFONT);
        AcceptBeep();
        Delay1Sec(2, 0);
      }
      break;
    default:
      break;
  }
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : MerchFunc
//  Description     : Process APM fuction.
//  Input           : aFuncId;        // Function Index. -1 => Prompt Select
//                    aGetAccess;     // TRUE prompt access code if necessary
//  Return          : 0;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD MerchFunc(int aFuncId, BOOLEAN aGetAccess)
{
  int func_sel;
  char filename[32], tmp[MW_MAX_LINESIZE+1];

  while (1) {
    if (aFuncId == -1) {
      func_sel = MenuSelect(&KApmFuncMenu, 0);
      if (func_sel == -1)
        break;
    }
    else
      func_sel = aFuncId;
   
    if (aGetAccess) {
      // Prompt Access code if call from other appl.
      switch (func_sel) {
        // No need for following functions
        case 11:  case 12:  case 64:  case 80:  case 81: case 82: case 83:
        case 85:  case 86:  case 88:  case 89:  case 94:
          break;
        default:
          DispCtrlMW(MW_CLR_DISP);
          if (!GetAccessCode())
            return 0;
      }
    }

    switch (func_sel) {
      case  0:
        if (GetRecCount()>0) {
          Disp2x16Msg(KSettleBatch, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
          LongBeep();
          WaitKey(KBD_TIMEOUT);
          break;
        }
        InitTrans();
        break;
      case  2:
        SetupSTIS();
        fCommitAllMW();
        break;
      #if (LAN_SUPPORT)
      case  3:
        TermIPFunc();
        break;
      #endif //(LAN_SUPPORT)
      case  7:
        STISFTPSetup();                                               //29-09-16 JC ++
        break;
      case  8:
        STISFTPDownloadTrans(FALSE, MODE_ALL_PARAM_APP, FALSE);       //29-09-16 JC ++
        break;
      case  9:
        STISLoadParam();                                              //29-09-16 JC ++
        break;
      case  10:
        if (os_hd_config_extend() & K_XHdWifi)
          WifiStatus();
        else
          SetGPRS();
        break;
      case 11:
        PrintSTIS(TRUE, TRUE);
        break;
      case 12:
        if (Return2Base()) {
          PrintSTIS(TRUE, FALSE);
        }
        break;
      case 61:
        if (GetRecCount()>0) {
          Disp2x16Msg(KSettleBatch, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
          LongBeep();
          WaitKey(KBD_TIMEOUT);
          break;
        }
        SetModeOnOff(TOGGLE_TRAINING);
        break;
      case 63:
        SetModeOnOff(TOGGLE_DEBUG);
        break;
      case 64:
        Disp2x16Msg(KReboot, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
        if (WaitKey(KBD_TIMEOUT) == MWKEY_ENTER) {
          fCommitAllMW();
          ResetMW();
        }
        break;
      case 80:
        CalCheckDigit();
        break;
      case 81:
        LcdSetup();
        break;
      #if (CDC_SUPPORT)           // Condition Compile for Codec Support
      case 82:
        SetCodecVol();
        break;
      #endif
      #if (T300|T810|A5T1000|T1000|T800)
      case 83:
        KbdBacklight();
        break;
      #endif
      case 84:
        KeyDllTest();
        break;
      case 89:
        DispLineMW("Mem Avail :", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
        sprintf(tmp, "%d", GetMaxMalloc());
        DispLineMW(tmp, MW_LINE5, MW_BIGFONT);
        WaitKey(KBD_TIMEOUT);
        break;
        break;
      case 93:
        // Key Inject for external pinpad channel
        strcpy(filename, DEV_AUX2);
        SChnlSetupMW(filename, TRUE);
        WaitKey(KBD_TIMEOUT);
        break;
      case 94:
        DispCtrlMW(MW_CLR_DISP);
        UpdatePABX();
        break;
      case 98:
        SetupTerm();
        fCommitAllMW();
        break;
      case 99:
        CleanUp();
        break;
      default:
        LongBeep();
        break;
    }
    if (aFuncId != -1)   // Not menu selection.
      break;
  }
  return 0;
}
//*****************************************************************************
//  Function        : InitApplList
//  Description     : Init Application list for application selection.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void InitApplList(void)
{
  int i;

#if ENABLE_DEFAULT_APP
  BOOLEAN defaultAppFound = false;
  memset(&gFileData, 0, sizeof(gFileData));
  LoadFile();
#if FORCE_DEFAULT_APP
#warning forcing default app is on!
  gFileData.bDefaultAppID = FORCE_DEFAULT_APP;
#endif
#endif

  gApplCount = 0;
  gAppList = (struct APPL_LIST *) MallocMW(sizeof(struct APPL_LIST) * MAX_APPL_SUPPORT);
  MemFatalErr(gAppList);
  for (i = 0; i < MAX_APPL_SUPPORT; i++)
  {
    gAppList[i].bActive = FALSE;
    if (AppInfoGetMW(i + MY_APPL_ID, &gAppList[gApplCount].sAppInfo))
    {
      if ((gAppList[gApplCount].sAppInfo.b_type & 0x01) == 0)
      {
#if ENABLE_DEFAULT_APP
        if (gFileData.bDefaultAppID == gAppList[gApplCount].sAppInfo.b_appid)
          defaultAppFound = 1;
#endif
        gAppList[gApplCount++].bActive = TRUE;
      }
    }
  }

#if ENABLE_DEFAULT_APP
  if (!defaultAppFound)
  {
    gFileData.bDefaultAppID = 0;
    SaveFile();
  }
#endif
}
//*****************************************************************************
//  Function        : SelectApp
//  Description     : Select Application.
//  Input           : aIgnoreID;
//  Return          : appl. Id
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD SelectAppl(DWORD aIgnoreID)
{
  int i, select, len;
  struct MENU_ITEM *SelAppItem;
  struct MENU_DAT  SelAppMenu;

  if (gApplCount == 0)
    return -1;

  // Check Application Count
  if (gApplCount <= 1)
    return gAppList[0].sAppInfo.b_appid;

  SelAppItem = (struct MENU_ITEM *) MallocMW(sizeof(struct MENU_ITEM) * (gApplCount+1)); // + rec end
  if (SelAppItem == NULL)
    return -1;

  select = 0;
  for (i = 0; i < gApplCount; i++) {
    if (gAppList[i].sAppInfo.b_appid == aIgnoreID)
      continue;
    SelAppItem[select].iID = gAppList[i].sAppInfo.b_appid;
    SelAppItem[select].pcDesc = NULL;
    memset(SelAppItem[select].scDescDat, ' ', sizeof(SelAppItem[i].scDescDat));
    len = strlen(&gAppList[i].sAppInfo.sb_app_name[2]);
    if (len > 10)
      len = 10;
    memcpy(SelAppItem[select].scDescDat, &gAppList[i].sAppInfo.sb_app_name[2], len);
    SelAppItem[select].scDescDat[MW_MAX_LINESIZE-1]=0;
    select++;
  }
  SelAppItem[select].iID = -1;
  SelAppItem[select].pcDesc = NULL;

  strcpy(SelAppMenu.scHeader, "Select Appl");
  SelAppMenu.psMenuItem = SelAppItem;
  select  = MenuSelect(&SelAppMenu, 0);
  FreeMW(SelAppItem);
  return select;
}

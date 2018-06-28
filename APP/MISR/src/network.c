//-----------------------------------------------------------------------------
//  File          : network.c
//  Module        :
//  Description   : GPRS command test.
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
#if (T300|T810|A5T1000|T1000|T800|TIRO)
#include <stdio.h>
#include <string.h>
#include "midware.h"
#include "util.h"
#if (T300|A5T1000|T1000|TIRO)
#include "apm.h"
#else
#include "apm_lite.h"
#endif
#include "sysutil.h"
#include "hardware.h"
#include "corevar.h"
#include "tranutil.h"
#include "message.h"
#include "toggle.h"
#include "input.h"
#include "wavecom.h"
#include "network.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define MIN_TEST_BUF        16
#define MAX_TEST_BUF        8*16

#define OK_DELAY            500         //10ms 
#define RX_RETRY_DELAY      300         //10ms
#define RX_TIMEOUT_TICK     (RX_RETRY_DELAY * 10)

#define MODULE_TIMEOUT      6000          // 60 seconds
#define SIM_DETECT_TIMEOUT  1500          // 15 seconds

enum {
  G510,
  H330,
  UC15,
  HL6,
  HL8,
};

#define MAX_CHAR_ON_BANNER  19

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
BYTE gSimSlot=0;                              // active SIM slot
BYTE gOptrName1[32];
BYTE gOptrName2[32];
BYTE gOptrMain[32];
BYTE gOptrBkup[32];
//BYTE gGprsCfg1[32][3] = {"xxxx", "", ""};     // for SIM 1
//BYTE gGprsCfg2[32][3] = {"yyyy", "", ""};     // for SIM 2
BYTE gGprsCfg1[3][32] = {"aaa", "", ""};     // for SIM 1
BYTE gGprsCfg2[3][32] = {"bbb", "", ""};     // for SIM 2

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
static const char * const KPPPState[] = {
  "      Dead      ",
  "  Establishing  ",
  " Authenticating ",
  "    Network     ",
  "  Terminating   ",
  "   PPP Ready    "
};

//*****************************************************************************
//  Function        : SetBannerMode
//  Description     : Enable/Disable Banner mode in System
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetBannerMode(BOOLEAN aEnable)
{
  BYTE textmode;

  os_config_read(K_CF_LcdBannerEnable, &textmode);
  if ((textmode == 0) && aEnable) {
    textmode = 1;
    os_config_write(K_CF_LcdBannerEnable, &textmode);
    os_config_update();
  }
  else if ((textmode == 1) && (aEnable == 0)) {
    textmode = 0;
    os_config_write(K_CF_LcdBannerEnable, &textmode);
    os_config_update();
  }
  os_disp_icon_textc(RGB_BLACK);
}
//*****************************************************************************
//  Function        : SetBannerText
//  Description     : Set Banner text
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IconPuts(BYTE *aText)
{
  DWORD len, i;
  
  os_disp_icon_putc('\f');
  os_disp_icon_putc(0x12);
  len = i = strlen(aText);
  while (i--)
    os_disp_icon_putc(*aText++);
  if (len < MAX_CHAR_ON_BANNER)
    os_disp_icon_putc(0x05);
}
//*****************************************************************************
//  Function        : SetBannerText
//  Description     : Set Banner text
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetBannerText(BYTE *aMsg)
{
  SetBannerMode(TRUE);
  IconPuts(aMsg);
}
//*****************************************************************************
//  Function        : GPRSReset
//  Description     : Test GPRS Reset command
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GPRSReset(void)
{
  DWORD last_status, status, signal, last_signal;
  DWORD keyin;
  BYTE  disp_buf[MW_MAX_LINESIZE+1];

  last_signal = last_status = (DWORD) -1;

  DispLineMW("Press Enter To Reset", MW_LINE3, MW_SPFONT);
  while (1) {
    keyin = GetCharMW();
    if (keyin == MWKEY_CANCL)
      break;
    if (keyin == MWKEY_ENTER) {
      DispLineMW("Module Reseted", MW_LINE4, MW_SPFONT);
      LongBeep();
      IOCtlMW(gGprsHandle, IO_GPRS_RESET, 0);
      APM_WaitKey(KBD_TIMEOUT, 0);
      DispClrLineMW(MW_LINE4);
    }
    status = StatMW(gGprsHandle, (DWORD) -1, NULL);
    signal = IOCtlMW(gGprsHandle, IO_GPRS_SIGNAL, NULL);
    if ((status != last_status)||(signal != last_signal)) {
      sprintf(disp_buf, "Status: %08X %02d", status, signal);
      DispLineMW(disp_buf, MW_MAX_LINE, MW_SPFONT);
      last_status = status;
    }
  }
}
//*****************************************************************************
//  Function        : SetPPPConfig
//  Description     : Get APN/username/password for the operator
//  Input           : aApn;         // operator APN
//                    aUsername;    // username
//                    aPassword;    // password
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetPPPConfig(BYTE *aApn, BYTE *aUsername, BYTE *aPassword)
{
  struct PPPSREG sPPP;
  DWORD eidx;

  memset(&sPPP, 0, sizeof(struct PPPSREG));
  sPPP.bKeepAlive  = TRUE; // Never disconnect
  sPPP.dDevice      = MW_PPP_DEVICE_GPRS;
  sPPP.dSpeed       = 230400;
  sPPP.dMode        = MW_PPP_MODE_NORMAL;
  //31-05-18 JC ++
  eidx = skpb(aApn, ' ', 32);                     // max size of apn in TERM_CFG
  eidx = (eidx < strlen(aApn))? eidx: strlen(aApn);
  memcpy(sPPP.scAPN, aApn, eidx);
  eidx = skpb(aUsername, ' ', 32);                // max size of username in TERM_CFG
  eidx = (eidx < strlen(aUsername))? eidx: strlen(aUsername);
  memcpy(sPPP.scUserID, aUsername, eidx);
  eidx = skpb(aPassword, ' ', 32);                // max size of password in TERM_CFG
  eidx = (eidx < strlen(aPassword))? eidx: strlen(aPassword);
  memcpy(sPPP.scPwd, aPassword, eidx);
  //31-05-18 JC --
  sPPP.psLogin      = NULL;
  sPPP.dLoginPair   = 0;
  sPPP.psDialUp     = NULL;
  sPPP.dDialupPair  = 0;
  sPPP.psDialParam  = NULL;
  
  PPPCfgUpdate(&sPPP);
}
//*****************************************************************************
//  Function        : NetworkInfo
//  Description     : Get network info from GPRS network
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void NetworkInfo(void)
{
  const char *KSimSel[] = {"01", "02", NULL};
  BYTE buf[64], disp_buf[MW_MAX_LINESIZE+1];
  BYTE *gprs_cfg1[] = {"internet", "", ""};
  BYTE *gprs_cfg2[] = {"pccw", "", ""};
  DWORD signal, status;
  int iOpt;
  BYTE bSimNum;

  DispLineMW("Network Info", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  
  // select sim slot
  #if (T700|A5T1000|T300|T810)
  {      
    GetSysCfgMW(MW_SYSCFG_SIMSEL, &bSimNum);
    iOpt = bSimNum;
    iOpt = ToggleOption("SIM Num", KSimSel, iOpt);
    if (iOpt >= 0) {
      if (bSimNum != iOpt) {
        DispClrBelowMW(MW_LINE3);
        DispLineMW("Pls Wait..", MW_LINE3, MW_CENTER|MW_BIGFONT);
        if (IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL) == MW_PPP_PHASE_READY) {
          IOCtlMW(gPppHandle, IO_PPP_DISCONNECT, NULL);
          while (1) {
            status = IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
            if ((status == MW_PPP_PHASE_IDLE) || (status == MW_PPP_PHASE_DEAD))// || (status == MW_PPP_PHASE_TERMINATE))
              break;
            if (GetCharMW() != MWKEY_CANCL)
              break;
          }
        }
        DispLineMW("Pls Wait...", MW_LINE3, MW_CENTER|MW_BIGFONT);
        bSimNum = iOpt;
        IOCtlMW(gGprsHandle, IO_GPRS_SIM_SEL, &bSimNum);      
      }
      DispClrBelowMW(MW_LINE3);
    }
    else
      return;
  }
  #elif (T1000)
  {      
    os_config_read(K_CF_SimSelect, &bSimNum);
    iOpt = bSimNum;
    iOpt = ToggleOption("SIM Num", KSimSel, iOpt);
    if (iOpt >= 0) {
      if (bSimNum != iOpt) {
        DispClrBelowMW(MW_LINE3);
        DispLineMW("Pls Wait", MW_LINE3, MW_CENTER|MW_BIGFONT);
        if (IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL) == MW_PPP_PHASE_READY) {
          IOCtlMW(gPppHandle, IO_PPP_DISCONNECT, NULL);
          while (1) {
            status = IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
            if ((status == MW_PPP_PHASE_IDLE) || (status == MW_PPP_PHASE_DEAD))// || (status == MW_PPP_PHASE_TERMINATE))
              break;
            if (GetCharMW() != MWKEY_CANCL)
              break;
          }
        }
        DispLineMW("Pls Wait...", MW_LINE3, MW_CENTER|MW_BIGFONT);
        bSimNum = iOpt;
        os_config_write(K_CF_SimSelect, &bSimNum);
        os_config_update();
        os_gprs_reset(K_GPRS_MODE_NORMAL);
      }
      DispClrBelowMW(MW_LINE3);
    }      
    else
      return;
  }
  #endif
  sprintf(disp_buf, "SIM Slot: %d", bSimNum+1);
  DispLineMW(disp_buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
  
  DispLineMW("Wait AT Rdy..", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  TimerSetMW(gTimerHdl[TIMER_GPRS], MODULE_TIMEOUT);
  while ((StatMW(gGprsHandle, MW_GSTS_ATCMDRDY, NULL)&MW_GSTS_ATCMDRDY) == 0) {
    if (TimerGetMW(gTimerHdl[TIMER_GPRS]) == 0) {
      DispLineMW("Timeout!", MW_MAX_LINE, MW_RIGHT|MW_SPFONT);
      APM_WaitKey(KBD_TIMEOUT, 0);
      return;
    }
  }
  
  DispLineMW("Check SIM..", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  TimerSetMW(gTimerHdl[TIMER_GPRS], MODULE_TIMEOUT);
  while ((StatMW(gGprsHandle, MW_GSTS_SIMINSERTED, NULL)&MW_GSTS_SIMINSERTED) == 0) {
    if (TimerGetMW(gTimerHdl[TIMER_GPRS]) == 0) {
      DispLineMW("Timeout!", MW_MAX_LINE, MW_RIGHT|MW_SPFONT);
      DispLineMW("NoSIM", MW_LINE2, MW_SPFONT);
      APM_WaitKey(KBD_TIMEOUT, 0);
      return;
    }
  }
  
  DispLineMW("Wait Network Rdy..", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  TimerSetMW(gTimerHdl[TIMER_GPRS], MODULE_TIMEOUT);
  while ((StatMW(gGprsHandle, MW_GSTS_NETWORKAVAIL, NULL)&MW_GSTS_NETWORKAVAIL) == 0) {
    if (TimerGetMW(gTimerHdl[TIMER_GPRS]) == 0) {
      DispLineMW("Timeout!", MW_MAX_LINE, MW_RIGHT|MW_SPFONT);
      APM_WaitKey(KBD_TIMEOUT, 0);
      return;
    }
  }
  
  DispLineMW("Get Operator Name..", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  if (GetOptrName(buf, OPTR_NAME_SIZE) == TRUE) {
    sprintf(disp_buf, "OptrName: %s", buf);
    DispLineMW(disp_buf, MW_LINE2, MW_SPFONT);
  }
  else {
    DispLineMW("OptrName: N/A", MW_LINE2, MW_SPFONT);
  }
  
  signal = IOCtlMW(gGprsHandle, IO_GPRS_SIGNAL, NULL);
  sprintf(disp_buf, "Signal  : %02d", signal);
  DispLineMW(disp_buf, MW_LINE3, MW_SPFONT);
  
  DispLineMW("Use for Txn?!", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  if (APM_YesNo() != 2)
    return;
  if (bSimNum == 0)
    SetPPPConfig(gprs_cfg1[0], gprs_cfg1[1], gprs_cfg1[2]);
  else
    SetPPPConfig(gprs_cfg2[0], gprs_cfg2[1], gprs_cfg2[2]);
}
//*****************************************************************************
//  Function        : SelSIM
//  Description     : Select SIM slot on terminal
//  Input           : aSIM    (0: SIM1; 1: SIM2)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SelSIM(BYTE aSIM, BYTE *aOperName, DWORD *aSignal)
{
  struct TERM_CFG term_cfg;
  BYTE buf[64], disp_buf[32];
  DWORD signal, status;
  BYTE bSimNum;

  // select sim slot
  #if (T700|A5T1000|T300|T810)
  {      
    GetSysCfgMW(MW_SYSCFG_SIMSEL, &bSimNum);
    gSimSlot = bSimNum;
    if (bSimNum != aSIM) {
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "Pls Wait..");
      SetBannerText(disp_buf);
      status = IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
      if ((status != MW_PPP_PHASE_IDLE) && (status != MW_PPP_PHASE_DEAD)) {
        IOCtlMW(gPppHandle, IO_PPP_DISCONNECT, NULL);
        while (1) {
          status = IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
          if ((status == MW_PPP_PHASE_IDLE) || (status == MW_PPP_PHASE_DEAD))// || (status == MW_PPP_PHASE_TERMINATE))
            break;
        }
      }
      gSimSlot = bSimNum = aSIM;
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "Pls Wait...");
      SetBannerText(disp_buf);
      IOCtlMW(gGprsHandle, IO_GPRS_SIM_SEL, &bSimNum);
      os_gprs_reset(K_GPRS_MODE_NORMAL);
    }
  }
  #elif (T1000)
  {      
    os_config_read(K_CF_SimSelect, &bSimNum);
    if (bSimNum != aSIM) {
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "Pls Wait..");
      SetBannerText(disp_buf);
      status = IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
      if ((status != MW_PPP_PHASE_IDLE) && (status != MW_PPP_PHASE_DEAD)) {
        IOCtlMW(gPppHandle, IO_PPP_DISCONNECT, NULL);
        while (1) {
          status = IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
          if ((status == MW_PPP_PHASE_IDLE) || (status == MW_PPP_PHASE_DEAD))// || (status == MW_PPP_PHASE_TERMINATE))
            break;
        }
      }
      gSimSlot = bSimNum = aSIM;
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "Pls Wait...");
      SetBannerText(disp_buf);
      os_config_write(K_CF_SimSelect, &bSimNum);
      os_config_update();
      os_gprs_reset(K_GPRS_MODE_NORMAL);
    }
  }
  #endif
  
  sprintf(disp_buf, "S%d:%s", bSimNum+1, "Wait AT Rdy..");
  SetBannerText(disp_buf);
  TimerSetMW(gTimerHdl[TIMER_GPRS], MODULE_TIMEOUT);
  while ((StatMW(gGprsHandle, MW_GSTS_ATCMDRDY, NULL)&MW_GSTS_ATCMDRDY) == 0) {
    if (TimerGetMW(gTimerHdl[TIMER_GPRS]) == 0) {
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "Timeout1!");
      SetBannerText(disp_buf);
      //APM_WaitKey(100, 0);
      return FALSE;
    }
    SleepMW();
  }
  
  sprintf(disp_buf, "S%d:%s", bSimNum+1, "Check SIM..");
  SetBannerText(disp_buf);
  TimerSetMW(gTimerHdl[TIMER_GPRS], SIM_DETECT_TIMEOUT);
  while ((StatMW(gGprsHandle, MW_GSTS_SIMINSERTED, NULL)&MW_GSTS_SIMINSERTED) == 0) {
    if (TimerGetMW(gTimerHdl[TIMER_GPRS]) == 0) {
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "NoSIM");
      SetBannerText(disp_buf);
      //APM_WaitKey(100, 0);
      return FALSE;
    }
    SleepMW();
  }
  
  sprintf(disp_buf, "S%d:%s", bSimNum+1, "Wait Net Rdy..");
  SetBannerText(disp_buf);
  TimerSetMW(gTimerHdl[TIMER_GPRS], MODULE_TIMEOUT);
  while ((StatMW(gGprsHandle, MW_GSTS_NETWORKAVAIL, NULL)&MW_GSTS_NETWORKAVAIL) == 0) {
    if (TimerGetMW(gTimerHdl[TIMER_GPRS]) == 0) {
      sprintf(disp_buf, "S%d:%s", bSimNum+1, "Timeout2!");
      SetBannerText(disp_buf);
      //APM_WaitKey(100, 0);
      return FALSE;
    }
    SleepMW();
  }
  
  sprintf(disp_buf, "S%d:%s", bSimNum+1, "Get Operator Info..");
  SetBannerText(disp_buf);
  if (GetOptrName(buf, OPTR_NAME_SIZE) == TRUE) {
    signal = IOCtlMW(gGprsHandle, IO_GPRS_SIGNAL, NULL);
    sprintf(disp_buf, "%d:%s", bSimNum+1, buf);
    strcpy(aOperName, buf);
    *aSignal = signal;
  }
  else {
    sprintf(disp_buf, "%d:%s", bSimNum+1, "N/A");
  }
  SetBannerText(disp_buf);
  
  APM_GetTermCfg(&term_cfg);
  if (bSimNum == 0) {
    //SetPPPConfig(gGprsCfg1[0], gGprsCfg1[1], gGprsCfg1[2]);
    SetPPPConfig(term_cfg.apnconfig, term_cfg.apn_username, term_cfg.apn_password);
  }
  else {
    //SetPPPConfig(gGprsCfg2[0], gGprsCfg2[1], gGprsCfg2[2]);
    SetPPPConfig(term_cfg.apnconfig2, term_cfg.apn2_username, term_cfg.apn2_password);
  }
    
  return TRUE;
}
//*****************************************************************************
//  Function        : DispDTG
//  Description     : Show data/time or training mode status on idle menu.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispDTG(void)
{
  struct DATETIME dtg;
  BYTE  tmp[MW_MAX_LINESIZE+1];

  TimerSetMW(gTimerHdl[TIMER_MAIN], 50);

  memset(tmp, 0, sizeof(tmp));
  ReadRTC(&dtg);
  ConvDateTime(tmp, &dtg, 1);
  memmove(&tmp[3], &tmp[4], 14);
  memmove(&tmp[6], &tmp[7], 10);

  tmp[16] = 0;
  os_disp_putc(K_PushCursor);
  os_disp_backc(RGB_WHITE);
  DispLineMW(tmp, MW_MAX_LINE, MW_CLREOL|MW_CENTER|MW_SPFONT);
  os_disp_putc(K_PopCursor);
}
//*****************************************************************************
//  Function        : GetBatteryPercent
//  Description     : Get battery level in percentage.
//  Input           : N/A
//  Return          : Battery level in percentage
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BYTE GetBatteryPercent(void)
{
  // 3.30V - <3.50V =>  0 - 24%
  // 3.50V - <3.55V => 25 - 49%
  // 3.55V - <3.65V => 50 - 74%
  // 3.65V - <3.85V => 75 - 99%
  // >3.85V         => 100%
  DWORD volt;
  BYTE percent=0;

  volt = os_batt_level2() & 0xFFFF;
  if (volt < 335)
    percent = 0;
  else if (volt < 345)
    percent = 10;
  else if ((volt >= 345) && (volt < 385))
    percent = 25 + ((volt-345)*50)/(385-345);        // 25% - 75%
  else if ((volt >= 385) && (volt < 410))
    percent = 75 + ((volt-385)*25)/(410-385);        // 75% - 100%
  else if (volt >= 410)
    percent = 100;
  
  return percent;
}
//*****************************************************************************
//  Function        : ShowSimStatus
//  Description     : Show network status on terminal
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowSimStatus(void)
{
  BYTE buf[32], batt[8];
  
  memset(buf, 0, sizeof(buf));
  // put SIM1 / SIM2 operator names (max 7 chars each)
  if (strlen(gOptrMain) > 7)
    memcpy(buf, gOptrMain, 7);
  else
    strcpy(buf, gOptrMain);
  strcat(buf, ",");
  if (strlen(gOptrBkup) > 7)
    memcpy(&buf[strlen(buf)], gOptrBkup, 7);
  else
    strcpy(&buf[strlen(buf)], gOptrBkup);
  memset(&buf[strlen(buf)], ' ', MAX_CHAR_ON_BANNER-strlen(buf));
  // put battery percentage
  SprintfMW(batt, "%d%%", GetBatteryPercent());
  strcpy(&buf[MAX_CHAR_ON_BANNER-strlen(batt)], batt);
  SetBannerText(buf);
  DispDTG();
  os_thread_return();
}
//*****************************************************************************
//  Function        : SimStatusThrd
//  Description     : Start/Stop SIM status thread on terminal
//  Input           : aStart  (0: stop; 1: start)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SimStatusThrd(BOOLEAN aStart)
{
  if (aStart)
    ThreadCreateMW(ShowSimStatus, 200);     // run thread function once per 1 second
  else
    ThreadDeleteMW(ShowSimStatus);
}
//*****************************************************************************
//  Function        : NetworkInit
//  Description     : Select SIM which have the greatest signal strength on terminal
//  Input           : aSIM    (0: SIM1; 1: SIM2)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void NetworkInit(void)
{
  struct ACQUIRER_TBL acq_tbl;
  BYTE buf[32], bSimNum, count=2;
  DWORD signal1, signal2;
  
  APM_GetAcqTbl(INPUT.w_host_idx, &acq_tbl);
  if ((acq_tbl.b_ssl_key_idx & 0x80) == 0)
    return;                                   // return if GPRS is not used for transaction

  Disp2x16Msg(GetConstMsg(EDC_NET_INIT), MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  memset(gOptrName1, 0, sizeof(gOptrName1));
  memset(gOptrName2, 0, sizeof(gOptrName2));
  memset(gOptrMain, 0, sizeof(gOptrMain));
  memset(gOptrBkup, 0, sizeof(gOptrBkup));
  signal1 = signal2 = 0;
  
  // check current sim slot
#if (T700|A5T1000|T300|T810)
  GetSysCfgMW(MW_SYSCFG_SIMSEL, &bSimNum);
#elif (T1000)
  os_config_read(K_CF_SimSelect, &bSimNum);
#endif
  if ((os_hd_config() & K_HdSim) == K_Hd2SimInstalled) {
    // dual sim unit
    while (count) {
      if (bSimNum == 0) {
        if (SelSIM(1, gOptrName2, &signal2) == TRUE) {
          sprintf(buf, "2:%s", gOptrName2);
          strcpy(gOptrMain, buf);
        }
        bSimNum = 1;
        count--;
      }
      else if (bSimNum == 1) {
        if (SelSIM(0, gOptrName1, &signal1) == TRUE) {
          sprintf(buf, "1:%s", gOptrName1);
          strcpy(gOptrMain, buf);
        }
        bSimNum = 0;
        count--;
      }
    }
  }
  else {
    // single sim unit
    if (SelSIM(0, gOptrName1, &signal1) == TRUE) {
      sprintf(buf, "1:%s", gOptrName1);
      strcpy(gOptrMain, buf);
    }
    bSimNum = 0;
  }
  
  if ((signal1 >= signal2) && signal1) {
    sprintf(buf, "1:%s", gOptrName1);
    strcpy(gOptrMain, buf);
    if (signal2 > 0)
      sprintf(buf, "2:%s", gOptrName2);
    else
      sprintf(buf, "2:NoSIM");
    DispLineMW(buf, MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    strcpy(gOptrBkup, buf);
    if (bSimNum == 1)
      SelSIM(0, gOptrName1, &signal1);
  }
  else if ((signal2 > signal1) && signal2) {
    sprintf(buf, "2:%s", gOptrName2);
    strcpy(gOptrMain, buf);
    if (signal1 > 0)
      sprintf(buf, "1:%s", gOptrName1);
    else
      sprintf(buf, "1:NoSIM");
    DispLineMW(buf, MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    strcpy(gOptrBkup, buf);
    if (bSimNum == 0)
      SelSIM(1, gOptrName2, &signal2);
  }
  else {
    // no SIM at SIM1 & SIM2
    strcpy(gOptrName1, "1:NoSIM");
    strcpy(gOptrName2, "2:NoSIM");
    strcpy(gOptrMain, "1:NoSIM");
    strcpy(gOptrBkup, "2:NoSIM");
  }
  SimStatusThrd(TRUE);
}
//*****************************************************************************
//  Function        : GetSubNum
//  Description     : Get Subscriber's Number
//                  : *aStr;    // starting ptr to search
//                  : aLen;     // length of string to search
//                  : *aNum;    // buffer to hold the number
//  Return          : 0;        // Not found
//                    1;        // TEL READY
//                    2;        // TEL & NAME ready
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
void GetSubNum(BYTE *aStr, DWORD aLen, BYTE *aNum)
{
  BYTE buf[32];
  BYTE num[32];
  BYTE num_len;
  BYTE *ptr;
  BYTE state;

  ptr = (BYTE *)strstr((char const *)aStr, "+CNUM:");
  if (ptr == NULL)
    return;
    
  memset(num, 0, sizeof(num));
  num_len = 0;
  state = 0;
  ptr = ptr + 6;
  while ((DWORD)(ptr - aStr) < aLen) {
    if (*ptr == ',') {
      state++;
      ptr++;
      continue;
    }
    switch (state) {
      case 1:
        if (num_len < sizeof(num))
          num[num_len++] = *ptr;
        break;
      default:
        break;
    }
    ptr++;
  }

  sprintf(buf, "Num : %s", num);
  DispLineMW(buf, MW_LINE4, MW_SPFONT);
  Delay10ms(50);
  strcpy(aNum, num);
}
//*****************************************************************************
//  Function        : GetMobileNum
//  Description     : Get Mobile Subscriber's Number
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetMobileNum(BYTE *aNum)
{
  #define LOOP_DELAY        600
  char tmp[256], disp_buf[MW_MAX_LINESIZE+1];
  DWORD status, last_status;
  DWORD loop_cnt, ret;
  struct GPRSATCMD  at_cmd;
  struct GPRSATRESP at_resp;
  BYTE endl=0;

  last_status = (DWORD) -1;
  loop_cnt    = 0;
  ret         = 0;
  while (GetCharMW()!=MWKEY_CANCL) {
    status = StatMW(gGprsHandle, (DWORD) -1, NULL);
    if (status != last_status) {
      sprintf(disp_buf, "Status: %08X", status);
      DispLineMW(disp_buf, MW_MAX_LINE, MW_SPFONT);
      last_status = status;
    }
    if ((loop_cnt == 0)&&(status&MW_GSTS_ATREADY1)) {
      strcpy(tmp, "AT+CNUM\r\n");                    // for G510/H330
      DispClrBelowMW(MW_LINE3);
      sprintf(disp_buf, "Send Cmd : %s", tmp);
      DispLineMW(disp_buf, MW_LINE2,  MW_SPFONT);
      at_cmd.d_timeout = LOOP_DELAY;
      memset(at_cmd.sb_content, 0x00, sizeof(at_cmd.sb_content));
      strcpy(at_cmd.sb_content, tmp);
      IOCtlMW(gGprsHandle, IO_GPRS_CMDSEND, &at_cmd);
      Delay10ms(50);
      while (!endl) {
        memset(at_resp.sb_content, 0, sizeof(at_resp.sb_content));
        at_resp.d_maxlen = sizeof(at_resp.sb_content) - 1;  // 1 byte for NULL.
        ret = IOCtlMW(gGprsHandle, IO_GPRS_CMDRESP, &at_resp);
        switch (ret) {
          case 0:       // no response;
            break;
          case -1:      // timeout
            endl = 1;
            break;
          default:
            at_resp.sb_content[ret] = 0;
//             sprintf(disp_buf, "Rcvd Rsp: %s", at_resp.sb_content);
//             DispLineMW(disp_buf, MW_LINE4, MW_SPFONT);
            endl = 1;
            break;
        }
      }
    }
    else if ((status&MW_GSTS_ATREADY1) == 0) {
      DispLineMW("Insert SIM Card!", MW_LINE5, MW_CENTER|MW_SMFONT);
      APM_WaitKey(200, 0);
      break;
    }
    loop_cnt -= loop_cnt > 0 ? 1: 0;
    SleepMW();
    if (endl)
      break;
  }
  if ((ret != 0) && (ret != -1)) {
    // valid response received
    GetSubNum(at_resp.sb_content, strlen(at_resp.sb_content), aNum);
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : GetSimCCID
//  Description     : Get SIM CCID
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetSimCCID(BYTE *aCCID)
{
  #define LOOP_DELAY        600
  char tmp[256], disp_buf[MW_MAX_LINESIZE+1];
  DWORD status, last_status;
  DWORD loop_cnt, ret;
  struct GPRSATCMD  at_cmd;
  struct GPRSATRESP at_resp;
  char ccid[20+1];
  char *s_ptr;
  BYTE model=G510, endl=0;

  last_status = (DWORD) -1;
  loop_cnt    = 0;
  ret         = 0;
  memset(ccid, 0, sizeof(ccid));
  if ((os_hd_config() & K_HdGprs) == K_HdFibocomG510)
    model = G510;
  else if ((os_hd_config() & K_HdGprs) == K_HdFibocomH330)
    model = H330;
  else if ((os_hd_config() & K_HdGprs) == K_HdQuectelUC15)
    model = UC15;
  else if ((os_hd_config() & K_HdGprs) == K_HdSierraHL6528)
    model = HL6;
  else if ((os_hd_config() & K_HdGprs) == K_HdSierraHL8548)
    model = HL8;
  else
    return FALSE;
  
  while (GetCharMW()!=MWKEY_CANCL) {
    status = StatMW(gGprsHandle, (DWORD) -1, NULL);
    if (status != last_status) {
      sprintf(disp_buf, "Status: %08X", status);
      DispLineMW(disp_buf, MW_MAX_LINE, MW_SPFONT);
      last_status = status;
    }
    if ((loop_cnt == 0)&&(status&MW_GSTS_ATREADY1)) {
      if ((model == G510) || (model == H330) || (model == HL6) || (model == HL8))
        strcpy(tmp, "AT+CCID?\r\n");                    // for G510/H330/HL6/HL8
      else if (model == UC15)
        strcpy(tmp, "AT+QCCID\r\n");                    // for UC15
      DispClrBelowMW(MW_LINE3);
      sprintf(disp_buf, "Send Cmd : %s", tmp);
      DispLineMW(disp_buf, MW_LINE2,  MW_SPFONT);
      at_cmd.d_timeout = LOOP_DELAY;
      memset(at_cmd.sb_content, 0x00, sizeof(at_cmd.sb_content));
      strcpy(at_cmd.sb_content, tmp);
      IOCtlMW(gGprsHandle, IO_GPRS_CMDSEND, &at_cmd);
      Delay10ms(50);
      while (!endl) {
        memset(at_resp.sb_content, 0, sizeof(at_resp.sb_content));
        at_resp.d_maxlen = sizeof(at_resp.sb_content) - 1;  // 1 byte for NULL.
        ret = IOCtlMW(gGprsHandle, IO_GPRS_CMDRESP, &at_resp);
        switch (ret) {
          case 0:       // no response;
            break;
          case -1:      // timeout
            endl = 1;
            break;
          default:
            at_resp.sb_content[ret] = 0;
//             sprintf(disp_buf, "Rcvd Rsp: %s", at_resp.sb_content);
//             DispLineMW(disp_buf, MW_LINE4, MW_SPFONT);
            endl = 1;
            break;
        }
      }
    }
    else if ((status&MW_GSTS_ATREADY1) == 0) {
      DispLineMW("Insert SIM Card!", MW_LINE5, MW_CENTER|MW_SMFONT);
      APM_WaitKey(200, 0);
      break;
    }
    loop_cnt -= loop_cnt > 0 ? 1: 0;
    SleepMW();
    if (endl)
      break;
  }
  if ((ret != 0) && (ret != -1)) {
    // valid response received
    if ((model == G510) || (model == H330) || (model == HL6) || (model == HL8)) {
      // G510/H330/HL6/HL8
      s_ptr = strstr(at_resp.sb_content, "+CCID:");
      if (s_ptr != NULL) {
        memcpy(aCCID, s_ptr+7, 20);
      }
    }
    else {
      // UC15
      s_ptr = strstr(at_resp.sb_content, "+QCCID:");
      if (s_ptr != NULL) {
        memcpy(aCCID, s_ptr+8, 20);
      }
    }
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : MobileNumTest
//  Description     : Get Mobile Subscriber's Number
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void MobileNumTest(void)
{
  BYTE buf[64], num[32];
  
  DispClrBelowMW(MW_LINE2);
  memset(num, 0, sizeof(num));
  if (GetMobileNum(num) == TRUE) {
    DispLineMW("Mobile No:", MW_LINE4, MW_SMFONT);
    SprintfMW(buf, "%s", num);
    DispLineMW(buf, MW_LINE5, MW_SMFONT);
  }
  else {
    DispLineMW("Get Num Failed!", MW_LINE5, MW_CENTER|MW_SMFONT);
  }
  APM_WaitKey(KBD_TIMEOUT, 0);
}
//*****************************************************************************
//  Function        : SNTest
//  Description     : Get SIM serial
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SNTest(void)
{
  BYTE buf[64], sn[32];
  
  DispClrBelowMW(MW_LINE2);
  memset(sn, 0, sizeof(sn));
  if (GetSimCCID(sn) == TRUE) {
    DispLineMW("SIM SN:", MW_LINE4, MW_SMFONT);
    SprintfMW(buf, "%s", sn);
    DispLineMW(buf, MW_LINE5, MW_SPFONT);
  }
  else {
    DispLineMW("Get SN Failed!", MW_LINE5, MW_CENTER|MW_SMFONT);
  }
  APM_WaitKey(KBD_TIMEOUT, 0);
}
#endif //(T300|T810|A5T1000|T1000|T800|T700|TIRO)

//*****************************************************************************
//  Function        : GetPPPStatus
//  Description     : Return current PPP status
//  Input           : N/A
//  Return          : PPP status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetPPPStatus(void)
{
  return IOCtlMW(gPppHandle, IO_PPP_GET_PHASE, NULL);
}
//*****************************************************************************
//  Function        : DispNifInfo2
//  Description     : Display NIF INFO2
//  Input           : aNifInfo2;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : PC1210 new added
//*****************************************************************************
#ifdef bs_net_nif_info2 //PC1210 new added
void DispNifInfo2(T_NIF_INFO2 aNifInfo2)
{
  DispClrBelowMW(MW_LINE2);
  DispLineMW("IP Address", MW_LINE2, MW_SMFONT);
  ShowIp((BYTE*)&aNifInfo2.d_ip, MW_LINE3, FALSE);
  DispLineMW("Netmask", MW_LINE4, MW_SMFONT);
  ShowIp((BYTE*)&aNifInfo2.d_netmask, MW_LINE5, FALSE);
  DispLineMW("Gateway IP", MW_LINE6, MW_SMFONT);
  ShowIp((BYTE*)&aNifInfo2.d_gateway, MW_LINE7, FALSE);
}
#endif //bs_net_nif_info2
//*****************************************************************************
//  Function        : ShowPPPStatus
//  Description     : Show PPP Status.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowPPPStatus(void)
{
  DWORD status, ostatus, uptime;
#ifdef bs_net_nif_info2
  T_NIF_INFO2 nif_info;
#else
  struct MW_NIF_INFO nif_info;
#endif
  BYTE tmp[MW_MAX_LINESIZE+1];

  ClearDispMW();
  ostatus = -2;
  status  = -1;
  while (GetCharMW() != MWKEY_CANCL) {
    SleepMW();

    // Show Status DWORD
    status = GetPPPStatus();
    if (ostatus != status) {
      // Show IP Info
      if (status == MW_PPP_PHASE_READY) {
#ifdef bs_net_nif_info2
        bs_net_nif_info2(bs_ppp_get_nif(), &nif_info);       // for 3G
#else
        NetInfoMW(MW_NIF_PPP, &nif_info);
#endif
      }
      else {
        memset(&nif_info, 0, sizeof(nif_info));
      }
#ifdef bs_net_nif_info2
      DispNifInfo2(nif_info);                                // for 3G
#else
      DispTermIP((BYTE *)&nif_info);
#endif

      // Show Status
      if (status < 6) {
        DispLineMW(KPPPState[status], MW_LINE1, MW_REVERSE|MW_CENTER|MW_SMFONT);
      }
      else {
        DispLineMW("IDLE", MW_LINE1, MW_REVERSE|MW_CENTER|MW_SMFONT);
      }
      ostatus = status;
    }

    if (status == MW_PPP_PHASE_READY)
      uptime = IOCtlMW(gPppHandle, IO_PPP_UPTIME, NULL);
    else
      uptime = 0;
    sprintf(tmp, "Up Time: %05d Secs", uptime);
    DispLineMW(tmp, MW_MAX_LINE, MW_CENTER|MW_SPFONT);
  }
}
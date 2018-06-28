//-----------------------------------------------------------------------------
//  File          : hardware.c
//  Module        :
//  Description   : Include hardware handle routines.
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
#include "sysutil.h"
#include "kbdutil.h"
#include "wifi.h"
#include "mcp.h"
#include "comm.h"
#include "toggle.h"
#include "ecrdll.h"
#include "termdata.h"
#include "hardware.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
// Device Handle
int    gDevHandle[APM_SDEV_MAX];
int    gTimerHdl[MAX_TIMER_SUPPORT];

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------

//*****************************************************************************
//  Function        : InitAllHw
//  Description     : Initialize all hardware drivers.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void InitAllHw(void)
{
  BYTE filename[32];
  int i;

  for (i=0; i < APM_SDEV_MAX; i++) {
    gDevHandle[i] = -1;
  }

  //25-07-17 JC ++
#if 0
  strcpy(filename, DEV_MSR1);
  gDevHandle[APM_SDEV_MSR] = OpenMW(filename, MW_RDONLY);
  strcpy(filename, DEV_ICC1);
  gDevHandle[APM_SDEV_ICC] = OpenMW(filename, MW_RDWR);
#endif
  //25-07-17 JC --
  strcpy(filename, DEV_GPRS);
  gDevHandle[APM_SDEV_GPRS] = OpenMW(filename, MW_RDWR);
  strcpy(filename, DEV_PPP1);
  gDevHandle[APM_SDEV_PPP] = OpenMW(filename, MW_RDWR);
  //25-07-17 JC ++
#if 0
  #if (CDC_SUPPORT)
  strcpy(filename, DEV_CDC1);
  gDevHandle[APM_SDEV_CDC] = OpenMW(filename, MW_WRONLY);
  #endif
  //07-12-16 JC ++
  if ((os_hd_config() & K_HdModem) != K_HdNoModem) {
    strcpy(filename, DEV_MDM1);
    gDevHandle[APM_SDEV_MDM] = OpenMW(filename, MW_RDWR);
  }
  //07-12-16 JC --
#endif
  //25-07-17 JC --
  //05-09-14 JC ++
  if (os_hd_config_extend() & K_XHdWifi) {
    strcpy(filename, DEV_WIFI);
    gDevHandle[APM_SDEV_WIFI] = OpenMW(filename, MW_RDWR);
    WifiOn();
  }
  //05-09-14 JC --

  for (i=0; i < MAX_TIMER_SUPPORT; i++) {
    gTimerHdl[i] = TimerOpenMW();
  }
  //#if (CLCD_SUPPORT)  
  if (GetLcdTypeMW() == MW_DISP_COLOR)
    os_disp_backc(COLOR_BACKGR);
  //#endif

  //25-07-17 JC ++
  os_clicc_close();           // Already open by emvcldll
  //#ifdef bs_lan_open
  //bs_lan_open();            // Enable LAN support, should be off if not use.
  //#endif
  //25-07-17 JC --
}
//*****************************************************************************
//  Function        : CloseAllHw
//  Description     : Close All hardware driver.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CloseAllHw(void)
{
  int i;

  for (i=0; i < APM_SDEV_MAX; i++) {
    if (gDevHandle[i] >= 0)
      CloseMW(gDevHandle[i]);
  }

  for (i=0; i < MAX_TIMER_SUPPORT; i++) {
    if (gTimerHdl[i] >=0)
      CloseMW(gTimerHdl[i]);
  }
}
//*****************************************************************************
//  Function        : GetHwHandle
//  Description     : Return hardware handle id.
//  Input           : aHwId;
//                    aClose;       // 1=> CLOSE handle
//  Return          : handle;
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
int GetHwHandle(DWORD aHwId, DWORD aClose)
{
  BYTE filename[32];

  if (aHwId >= APM_SDEV_MAX)
    return -1;

  if (aClose==1) {
    if (gDevHandle[aHwId] != -1) {
      CloseMW(gDevHandle[aHwId]);
      gDevHandle[aHwId] = -1;
    }
    return -1;
  }

  if (gDevHandle[aHwId] == -1) {
    switch (aHwId) {
      case APM_SDEV_MSR:
        strcpy(filename, DEV_MSR1);
        gDevHandle[APM_SDEV_MSR] = OpenMW(filename, MW_RDONLY);
        break;
      case APM_SDEV_GPRS:
        strcpy(filename, DEV_GPRS);
        gDevHandle[APM_SDEV_GPRS] = OpenMW(filename, MW_RDWR);
        break;
      case APM_SDEV_PPP:
        strcpy(filename, DEV_PPP1);
        gDevHandle[APM_SDEV_PPP] = OpenMW(filename, MW_RDWR);
      case APM_SDEV_CDC:
        #if (CDC_SUPPORT)
        strcpy(filename, DEV_CDC1);
        gDevHandle[APM_SDEV_CDC] = OpenMW(filename, MW_WRONLY);
        #endif
        break;
      case APM_SDEV_MDM:
        #if (MDM_SUPPORT)
        strcpy(filename, DEV_MDM1);
        gDevHandle[APM_SDEV_MDM] = OpenMW(filename, MW_RDWR);
        #endif
        break;
      case APM_SDEV_WIFI:
        #if defined(os_hd_config_extend) && defined(K_XHdWifi)
        if (os_hd_config_extend() & K_XHdWifi) {
          strcpy(filename, DEV_WIFI);
          gDevHandle[APM_SDEV_WIFI] = OpenMW(filename, MW_RDWR);
          WifiOn();
        }
        #endif
        break;
    }
  }

  return gDevHandle[aHwId];
}

//*****************************************************************************
//  Function        : StopEcrDll
//  Description     : Stop EcrDll process.
//  Input           : N/A
//  Return          : N/A
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
void StopEcrDll(void)
{
  ECR_Stop();
}
//*****************************************************************************
//  Function        : InitEcrDll
//  Description     : Start EcrDll process.
//  Input           : N/A
//  Return          : N/A
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN InitEcrDll(void)
{
  DWORD   keyin;
  T_APP_INFO app_info;
  struct TERM_DATA term_data;

  // Resume ECR communication
  if (os_app_info(ECRDLL_ID, &app_info)) {
    if (GetTermData(&term_data)) {
      if ((term_data.b_ecr_port&0x3F) != 0) {
        switch (term_data.b_ecr_port&0x3F)  {
        case TOG_AUXD:
          keyin = ECR_AUXD;
          break;
        case TOG_AUX2:
          keyin = ECR_AUX2;
          break;
        case TOG_USBS:
          keyin = ECR_USBS;
          break;
        case TOG_LAN:
          keyin = ECR_LAN;
          break;
        case TOG_WIFI:
          keyin = ECR_WIFI;
          break;
        case TOG_BT:
          keyin = ECR_BT;
          break;
        default:
        case TOG_AUX1:
          keyin = ECR_AUX1;
          break;
        }
        keyin |= (term_data.b_ecr_port & 0xC0);
        if (term_data.b_ecr_ssl == 1) {
          keyin |= ECR_SSL_SVR_AUTH;
        }
        else if (term_data.b_ecr_ssl == 2) {
          keyin |= ECR_SSL_CLN_AUTH;
        }
        ECR_Start(keyin);
        return TRUE;
      }
    }
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : PwrSleep
//  Description     : Enter power sleep to save battery power.
//  Input           : aSec;         // seconds to sleep before wake up
//  Return          : N/A
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
int PwrSleep(DWORD aSec)
{
#if (MCP_SUPPORT)
  struct TERM_DATA term_data;
#endif
  int i, ret = 0;
  DWORD   ppp_phase;
  BOOLEAN ppp_reqd = FALSE;
 
  #ifdef os_batt_level
  #ifdef K_PowerPluggedIn
  if (os_batt_level()&K_PowerPluggedIn)
    #ifdef K_PowerSleepPowerPluggedIn
    return K_PowerSleepPowerPluggedIn;
    #else
    return 0;
    #endif
  #else
  if (os_batt_level()&K_BatteryCharging)
    return 0;
  #endif
  #endif

  // Close all current connection
  ResetComm();

  if (os_hd_config()&K_HdLanInstalled) {
    ThreadDeleteMW(HotPlug);
    #ifdef bs_lan_close
    bs_lan_close();
    #endif
  }

  // Handle PPP
  ppp_phase = IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL);
  switch (ppp_phase) {
    case MW_PPP_PHASE_IDLE:
    case MW_PPP_PHASE_DEAD:
       break;
    case MW_PPP_PHASE_READY:
       ppp_reqd = TRUE;
       // fall thru
    default:
       DisconnectPPP();
  }

  if (os_hd_config_extend() & K_XHdWifi)
    WifiOff();
  
  //08-01-18 JC ++
#if (MCP_SUPPORT)
  GetTermData(&term_data);
  if (((term_data.b_ecr_port&0x3F) != TOG_BT) || (GetMCPStatus() != K_MCP_PHASE_CONNECTED))
#endif
    StopEcrDll();
  //08-01-18 JC --

  for (i=0; i < APM_SDEV_MAX; i++) {
    if (gDevHandle[i] < 0)
      continue;
    CloseMW(gDevHandle[i]);
  }

  Delay1Sec(1, 0);
#ifdef os_power_sleep_ak_pwake_wto_bt
  ret = os_power_sleep_ak_pwake_wto_bt(aSec); // with BT handling       //08-01-18 JC ++
#elif defined os_power_sleep_ak_pwake_wto
  ret = os_power_sleep_ak_pwake_wto(aSec);    // shorter wake up time
#elif defined os_power_sleep_awake_wto
  ret = os_power_sleep_awake_wto(aSec);
#elif defined os_power_sleep_wto
  ret = os_power_sleep_wto(aSec);
#else
  ret = os_power_sleep();
#endif

  // reopen or init if done before
  for (i=0; i < APM_SDEV_MAX; i++) {
    if (gDevHandle[i] < 0)
      continue;
    // reopen or init;
    gDevHandle[i] = -1;
    GetHwHandle(i, 0);  
  }

  //08-01-18 JC ++
#if (MCP_SUPPORT)
  if (((term_data.b_ecr_port&0x3F) != TOG_BT) || (GetMCPStatus() != K_MCP_PHASE_CONNECTED))
#endif
    InitEcrDll();
  //08-01-18 JC --
  if (os_hd_config_extend() & K_XHdWifi)
    WifiOn();
  // re-connect ppp 
  if (ppp_reqd) {
    ConnectPPP(FALSE);
  }
  if (os_hd_config()&K_HdLanInstalled) {
    ThreadCreateMW(HotPlug, 100);
  }

  return ret;
}

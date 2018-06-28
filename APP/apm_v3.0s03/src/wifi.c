//-----------------------------------------------------------------------------
//  File          : wifi.c
//  Module        :
//  Description   : WIFI connection related routines.
//  Author        : Lewis
//  Notes         :
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
//  15 Aug  2014  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "basecall.h"
#include "util.h"
#include "midware.h"
#include "hwdef.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "toggle.h"
#include "hardware.h"
#include "apmconfig.h"
#include "wifi.h"
#include "wifilogo.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
#define K_AUTO_RECONNECT_SEC        3
#define WIFI_CONNECT_TIMEOUT        6000

//-----------------------------------------------------------------------------
//    Globals
//-----------------------------------------------------------------------------
static int                 gMaxAp=0;
static struct MW_AP_INFO   *gApPool=NULL;

//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------
static char *KConnStsMsg[] = { "Disconnected", "Connected", "Wrong Key", "Timeout", "Connecting", "Wrong SSID", "Wrong BSSID", "Profile Not Found", "Invalid Key Len", "Invalid Config" };

//*****************************************************************************
//  Function        : ShowProfileList
//  Description     : Show AP list.
//  Input           : aCur;         // current highlight item
//                    aStart;       // Display start from ap list index
//  Return          : Num Item on the list.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD ShowProfileList(BOOLEAN aUpd, DWORD aCur, struct MW_AP_INFO *aCurInfo)
{
  #define SCAN_TIMEOUT      20 
  #define MAX_AP_LINE       5
  struct MW_AP_PROFILE profile;
  DWORD i, status, line_no, ssid_len, start;
  BYTE  tmp[MW_MAX_LINESIZE+1];
  BYTE  Enc[MW_MAX_LINESIZE+1];
  BYTE  rssi;
  
  struct CGRAPH *pGraphic;
  struct CGRAPH *pLock;

  if (aUpd == TRUE) {
    //31-12-14 JC ++
    status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);
    
    if (status == MW_AP_CONNECTING) {
      // reset Wifi module if can't connect to AP
      IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_OFF, NULL);
      IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_ON, NULL);
      Delay10ms(100);
    }
    //31-12-14 JC --
    gMaxAp = 0;
    DispLineMW("Scanning...", MW_LINE1, MW_CLRDISP|MW_SMFONT|MW_REVERSE|MW_CENTER);
    for (i = 0; i < SCAN_TIMEOUT; i++) {  // wait 10 sec for scan completed
      gMaxAp = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_SCAN, gApPool);
      if (gMaxAp >= 0) break;
      Delay10ms(100);
    }
  }

  if (gMaxAp<=0) {
    memset(tmp, 0, sizeof(tmp));
    IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_FW_VER, tmp);
    DispLineMW(tmp, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
    DispLineMW(" ", MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SMFONT);
    return 0;
  }

  // adjust aCur;
  aCur  = aCur < (DWORD)gMaxAp ? aCur : 0;
  start = 0;
  start = (start >= aCur) ? aCur : ((aCur - start >= MAX_AP_LINE) ? aCur-MAX_AP_LINE+1 : start);

  status=IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);  // get default connect AP
  DispLineMW((status==MW_AP_CONNECTED?profile.ssid:KConnStsMsg[status]), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);

  // Show AP List
  line_no = MW_LINE3;
  for (i = start; i < start+MAX_AP_LINE ; i++) {
    // Show ssid list
    memset(tmp, ' ', sizeof(tmp));
    tmp[MW_MAX_LINESIZE-1] = 0;
    ssid_len = strlen(gApPool[i].ssid);
    memcpy(tmp, gApPool[i].ssid, ssid_len>MW_MAX_LINESIZE?MW_MAX_LINESIZE:ssid_len);
    if (ssid_len > MW_MAX_LINESIZE) {
      memset(&tmp[MW_MAX_LINESIZE-4], '.', 3);
    }
    DispLineMW(tmp,line_no, (MW_CENTER|MW_SPFONT)|((aCur==i)?MW_REVERSE:0));
    line_no += MW_LINE3-MW_LINE2;
  }


  // Show Current BSSID, RSSI, ENC TYPE
  // conv rssi 0 to 5          check 0-5, which one?
  rssi = '0';
  if (gApPool[aCur].rssi < -100) {
    rssi += 0;
  }else if (gApPool[aCur].rssi > -50){  
    rssi += 5;
  }else if ((gApPool[aCur].rssi > -100) && (gApPool[aCur].rssi < -50)){
    rssi += ((gApPool[aCur].rssi + 100)/13)+1;
  }
  
  switch(rssi){
    case '1':
      pGraphic = (struct CGRAPH *) MallocMW(sizeof(signal_1));
      memcpy(pGraphic, signal_1, sizeof(signal_1));
      break;
    case '2':
      pGraphic = (struct CGRAPH *) MallocMW(sizeof(signal_2));
      memcpy(pGraphic, signal_2, sizeof(signal_2));
      break;
    case '3':
      pGraphic = (struct CGRAPH *) MallocMW(sizeof(signal_3));
      memcpy(pGraphic, signal_3, sizeof(signal_3));
      break;
    case '4':
      pGraphic = (struct CGRAPH *) MallocMW(sizeof(signal_4));
      memcpy(pGraphic, signal_4, sizeof(signal_4));
      break;
    case '5':
      pGraphic = (struct CGRAPH *) MallocMW(sizeof(signal_5));
      memcpy(pGraphic, signal_5, sizeof(signal_5));
      break;
    default:
      pGraphic = (struct CGRAPH *) MallocMW(sizeof(signal_0));  
      memcpy(pGraphic, signal_0, sizeof(signal_0));
      break;    
  }
  pGraphic->w_hstart = 0;  
  pGraphic->w_vstart = 192;
  
  // Enc_type
  memset(Enc, 0, sizeof(Enc));
  switch (gApPool[aCur].types) {
    case 0:
      memcpy(Enc, "OPEN", 4);     
      break;
    case 1:
      memcpy(Enc, "WEP", 3);
      break;
    default :
      memcpy(Enc, "WPA2", 4);
      break;
  }
  DispLineMW(Enc,MW_MAX_LINE,MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispPutGMW(pGraphic);
  if(gApPool[aCur].types != 0){
    pLock = (struct CGRAPH *) MallocMW(sizeof(lock));
    memcpy(pLock, lock, sizeof(lock));
    pLock->w_hstart = 24;  
    pLock->w_vstart = 192;
    DispPutGMW(pLock);
    FreeMW(pLock);
  }
  FreeMW(pGraphic);

  // Save & return current selection
  memcpy(aCurInfo, &gApPool[aCur], sizeof(struct MW_AP_INFO));
  return (DWORD)gMaxAp;
}

//*****************************************************************************
//  Function        : EditProfile
//  Description     : Edit Prfile
//  Input           : aProfile;         // wifi ap profile
//  Return          : Connection status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN EditProfile(struct MW_AP_PROFILE *aProfile)
{
  static const char *KEncType[]  = { "   OPEN", "    WEP", "   WPA2", "WPS_PBC", "WPS_PIN", NULL };
  static const char *KTypeMode[] = { "INFRA", "ADHOC", NULL };
  BYTE kbdbuf[33];
  int  option, mode;

  DispLineMW("WIFI Setup", MW_LINE1, MW_CLRDISP|MW_SMFONT|MW_REVERSE|MW_CENTER);

  // ssid
  kbdbuf[0] = strlen(aProfile->ssid);
  memcpy(&kbdbuf[1], aProfile->ssid, kbdbuf[0]);
  DispLineMW("SSID:", MW_LINE5, MW_BIGFONT);
  if (!GetKbd(ALPHA_INPUT_NEW|ECHO|MW_LINE7|MW_BIGFONT, IMAX(20), kbdbuf)) {
    return FALSE;
  }
  memcpy(aProfile->ssid, &kbdbuf[1], kbdbuf[0]);
  aProfile->ssid[kbdbuf[0]] = 0;

  // Mode 
  mode = aProfile->enc_type & WIFI_TYPE_MODE_ADHOC;
  DispClrBelowMW(MW_LINE5);
  option = mode ? 0x01 : 0x00;
  option = ToggleOption("Type:", KTypeMode, option);
  if (option == -1) {
    return FALSE;
  }
  mode   = option==0x01?WIFI_TYPE_MODE_ADHOC:WIFI_TYPE_MODE_INFRA;
  

  // enc_type
  DispClrBelowMW(MW_LINE5);
  option = aProfile->enc_type&0x0F;
  option = ToggleOption("Encryption Type:", KEncType, option);
  if (option == -1) {
    return FALSE;
  }
  aProfile->enc_type = option | mode;

  // pass
  kbdbuf[0] = 0;
  if ((option==1) || (option==2) || (option==4)) {
    DispClrBelowMW(MW_LINE5);
    kbdbuf[0] = 0;
    DispLineMW("KEY:", MW_LINE5, MW_BIGFONT);
    if (!GetKbd(ALPHA_INPUT_NEW|ECHO|MW_LINE7|MW_BIGFONT, IMAX(MW_MAX_LINESIZE*2), kbdbuf)) {
      return FALSE;
    }
    memcpy(aProfile->pass, &kbdbuf[1], kbdbuf[0]);
    aProfile->pass[kbdbuf[0]] = 0;
  }

  // Key Index
  if ((option==1)) {
    kbdbuf[0] = 0;
    DispClrBelowMW(MW_LINE5);
    kbdbuf[0] = 0;
    DispLineMW("KEY INDEX:", MW_LINE5, MW_BIGFONT);
    if (!GetKbd(ALPHA_INPUT_NEW|ECHO|MW_LINE7|MW_BIGFONT, IMIN(1)|IMAX(1), kbdbuf)) {
      return FALSE;
    }
    aProfile->key_index = kbdbuf[1];
  }
  return TRUE;
}
//*****************************************************************************
//  Function        : WifiOn
//  Description     : Power ON Wifi module
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void WifiOn(void)
{
  DWORD status, retry=3;

  if (gDevHandle[APM_SDEV_WIFI] == -1)
    return;
  
  while (retry--) {
    status = (DWORD) StatMW(gDevHandle[APM_SDEV_WIFI], 0, 0);
    if ((status & MW_WIFI_ON) == 0x00) {
      DispLineMW("Power On WIFI", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
      IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_ON, NULL);
      Delay10ms(200);
      // Reconnect
      IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_RECONNECT, K_AUTO_RECONNECT_SEC);  // retry connect every x seconds.
    }
    else
      break;
  }
}
//*****************************************************************************
//  Function        : WifiOff
//  Description     : Power OFF Wifi module
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void WifiOff(void)
{
  struct MW_AP_PROFILE profile;
  DWORD status;

  if (gDevHandle[APM_SDEV_WIFI] == -1)
    return;

  // disconnect connected AP before power off
  status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);
  if (status == MW_AP_CONNECTED) {
    DispLineMW("Disconnect WIFI", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
    IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_DISCONNECT, NULL);
  }
      
  status = (DWORD) StatMW(gDevHandle[APM_SDEV_WIFI], 0, 0);
  if (status & MW_WIFI_ON) {
    DispLineMW("Power Off WIFI", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
    IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_OFF, NULL);
    if (gApPool != NULL)
      FreeMW(gApPool);
  }
  Delay10ms(200);
}
//*****************************************************************************
//  Function        : WifiIPFunc
//  Description     : Config Wifi IP Address.
//  Input           : N/A
//  Return          : N/A
//  Note            : //10-04-15 JC ++
//  Globals Changed : N/A
//*****************************************************************************
static void WifiIPFunc(void)
{
  int option;
  BYTE    dhcp;
  BYTE    ip[20];
  BOOLEAN onoff_required=FALSE;

  DispLineMW("IP SETUP", MW_LINE1, MW_CLRDISP|MW_SMFONT|MW_REVERSE|MW_CENTER);
  
  memset(ip, 0, sizeof(ip));
  os_config_read(K_CF_WIFI_IP_CONFIG, &dhcp);
  if (!dhcp) {
    os_config_read(K_CF_WIFI_IP_ADDR, &ip[0]);
    os_config_read(K_CF_WIFI_IP_MASK, &ip[4]);
    os_config_read(K_CF_WIFI_GATEWAY, &ip[8]);
    os_config_read(K_CF_WIFI_DNS1, &ip[12]);
    os_config_read(K_CF_WIFI_DNS2, &ip[16]);
  }

  option = dhcp;
  option = ToggleOption("DHCP:", KEnable, option);
  if (option == -1) {
    return;
  }
  if (dhcp != (BYTE)option)
    onoff_required=TRUE;
    
  dhcp = (BYTE)option;
  if (!dhcp) {
    if (ConfigTermIP2(ip))
      onoff_required=TRUE;
    else
      onoff_required=FALSE;
  }
  if (onoff_required) {
    DispClrBelowMW(MW_LINE5);
    os_config_write(K_CF_WIFI_IP_CONFIG, &dhcp);
    os_config_write(K_CF_WIFI_IP_ADDR, &ip[0]);
    os_config_write(K_CF_WIFI_IP_MASK, &ip[4]);
    os_config_write(K_CF_WIFI_GATEWAY, &ip[8]);
    os_config_write(K_CF_WIFI_DNS1, &ip[12]);
    os_config_write(K_CF_WIFI_DNS2, &ip[16]);
    os_config_update();
    DispClrBelowMW(MW_LINE2);
    DispLineMW("Updating Config", MW_LINE5, MW_CENTER|MW_SMFONT);
    IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_OFF, NULL);
    IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_ON, NULL);
    Delay10ms(100);
  }
}
//*****************************************************************************
//  Function        : WifiConfig
//  Description     : Config Wifi APs
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void WifiConfig(void)
{
  struct MW_AP_PROFILE profile;
  struct MW_AP_INFO    apInfo;
  struct MW_NIF_INFO   netinfo;
  BOOLEAN changed;
  DWORD keyin, status;
  DWORD max_sel, idx;
  BOOLEAN update;
  BYTE ip[12];

  WifiIPFunc();             //10-04-15 JC ++
  
  gMaxAp = 0;
  if (gApPool==NULL) {
    gApPool = (struct MW_AP_INFO *)MallocMW(sizeof(struct MW_AP_INFO)*(MW_MAX_AP_LIST+1));
    if (gApPool==NULL) {
      CloseMW(gDevHandle[APM_SDEV_WIFI]);
      return;
    }
  }

  // should already power on 
  max_sel = idx = 0;
  update  = changed = TRUE;
  keyin   = 0;
  do {
    if (changed) {
      max_sel = ShowProfileList(update, idx, &apInfo);
      update  = FALSE;
      changed = FALSE;
    }
    keyin  = WaitKey(KBD_TIMEOUT);
    switch (keyin) {
      case MWKEY_DN: 
      case MWKEY_8:
        idx+=(idx<max_sel-1)?1:0; 
        changed=TRUE; 
        break;
      case MWKEY_UP: 
      case MWKEY_2:
        idx-=(idx>0)?1:0;     
        changed=TRUE; 
        break;
      case MWKEY_ENTER:
        status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);

        // selected ssid already connected.
        if ((status == MW_AP_CONNECTED) && (strcmp(profile.ssid, apInfo.ssid)==0)) {  
          DispLineMW("Disconnect?", MW_LINE3, MW_CLRDISP|MW_BIGFONT|MW_CENTER);
          if (WaitKey(KBD_TIMEOUT)==MWKEY_ENTER) {
            IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_DISCONNECT, NULL);
            Delay10ms(50);
            changed=TRUE; 
            break;
          }
        }

        memset(&profile, 0, sizeof(profile));
        strcpy(profile.ssid, apInfo.ssid);
        profile.enc_type = apInfo.types;
        // Set Profile
        if (EditProfile(&profile)) {
          //31-12-14 JC ++
          if (status == MW_AP_CONNECTED) {  
            // Disconnect the current before connecting to new AP
            IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_DISCONNECT, NULL);
            Delay10ms(50);
          }
          IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_CONNECT, &profile);
          ClearDispMW();
          status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);
          DispLineMW(KConnStsMsg[status], MW_LINE1, MW_REVERSE|MW_CENTER|MW_SMFONT);
          DispLineMW("Pls Wait..", MW_LINE5, MW_SMFONT|MW_CENTER);
          TimerSetMW(gTimerHdl[TIMER_WIFI], WIFI_CONNECT_TIMEOUT);
          do {
            status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);
            DispLineMW(KConnStsMsg[status], MW_LINE1, MW_REVERSE|MW_CENTER|MW_SMFONT);
            if (TimerGetMW(gTimerHdl[TIMER_WIFI]) == 0)
              break;
            if (GetCharMW() == MWKEY_CANCL)
              break;
            SleepMW();
          } while (status == MW_AP_CONNECTING);
          if (status == MW_AP_CONNECTED) {
            do {
              NetInfoMW(MW_NIF_WIFI, &netinfo);
              if (TimerGetMW(gTimerHdl[TIMER_WIFI]) == 0)
                break;
              if (GetCharMW() == MWKEY_CANCL)
                break;
            } while (netinfo.d_ip == 0);
            status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);
            memcpy(ip, (BYTE *)&netinfo.d_ip, 12);
            DispLineMW(profile.ssid, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
            DispTermIP(ip);
            break;
          }
          //31-12-14 JC --
        }
        // fall thru       
      case MWKEY_CLR: 
        gMaxAp = 0;
        changed = update = TRUE;
        break;
    }
  } while (keyin != MWKEY_CANCL);
}
//*****************************************************************************
//  Function        : WifiStatus
//  Description     : Show Wifi status.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void WifiStatus(void)
{
  struct MW_NIF_INFO netinfo;
  BYTE ip[12];
  DWORD status;
  struct MW_AP_PROFILE profile;
  
  if (gDevHandle[APM_SDEV_WIFI] == -1)
    return;

  NetInfoMW(MW_NIF_WIFI, &netinfo);
  memcpy(ip, (BYTE *)&netinfo.d_ip, 12);

  status=IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);  // get default connect AP
  DispLineMW((status==MW_AP_CONNECTED?profile.ssid:KConnStsMsg[status]), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispTermIP(ip);
  DispLineMW("[ENTER] to scan APs...", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
  if (WaitKey(KBD_TIMEOUT) == MWKEY_ENTER)
    WifiConfig();
}
//*****************************************************************************
//  Function        : GetWifiStatus
//  Description     : Get Wifi Status;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetWifiStatus(void)
{ 
  DWORD status = 0;
  if (gDevHandle[APM_SDEV_WIFI] != -1) {
    status = (DWORD) StatMW(gDevHandle[APM_SDEV_WIFI], 0, 0);
  }
  return status;
}
//*****************************************************************************
//  Function        : GetWifiConnStatus
//  Description     : Get Wifi Connection Status;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetWifiConnStatus(void)
{ 
  DWORD status;
  struct MW_AP_PROFILE profile;
  
  if (gDevHandle[APM_SDEV_WIFI] == -1)
    return MW_AP_DISCONNECTED;

  status = IOCtlMW(gDevHandle[APM_SDEV_WIFI], IO_WIFI_STATUS, &profile);
  return status;
}

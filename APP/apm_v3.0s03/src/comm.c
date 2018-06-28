//-----------------------------------------------------------------------------
//  File          : comm.c
//  Module        :
//  Description   : APM Communication Functions.
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
#include "apm.h"
#include "util.h"
#include "hardware.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "message.h"
#include "termdata.h"
#include "tranutil.h"
#include "expect_send.h"
#include "wifi.h"
#include "mcp.h"
#include "comm.h"
#include "toggle.h"

// For DEMO 
extern BYTE gReplaceAllComm;
extern void SetTestCommParam(struct COMMPARAM *aParam);

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
struct GPRSPPP {
  char scUserName[256];
  char scPassword[256];
  char scAPN[256];
  char scDialNum[256];
};

#define TYPE_VISA1       0x00
#define TYPE_SDLC        0x01
#define TYPE_ASYN        0x02
#define TYPE_VOICE       0x03

#define MODE_BELL        0x00
#define MODE_CCITT       0x01

#define BPS_300          0x00
#define BPS_1200         0x01
#define BPS_2400         0x02
#define BPS_9600         0x03
#define BPS_AUTO         0x04
#define BPS_AUTO_V42     0x05

static const struct {
  BYTE b_protocol;
  BYTE b_mode;
  BYTE b_speed;
} KCommParam[8] = {
  {TYPE_SDLC, MODE_BELL,  BPS_1200},
  {TYPE_ASYN, MODE_BELL,  BPS_1200},
  {TYPE_SDLC, MODE_CCITT, BPS_1200},
  {TYPE_ASYN, MODE_CCITT, BPS_1200},
  {TYPE_ASYN, MODE_BELL,  BPS_300},
  {TYPE_ASYN, MODE_CCITT, BPS_300},
  {TYPE_SDLC, MODE_CCITT, BPS_2400},
  {TYPE_ASYN, MODE_CCITT, BPS_2400}
};

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
static struct GPRSPPP   gGPRSDat;
int    gTcpHandle=-1;
int    gAuxHdl[MAX_APM_AUX];
struct COMMPARAM gCommParam;

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
struct OPTR_APN_MAP {
  const char *pcOptrName;
  const char *pcAPN;
};

static const char * const CommandResult[] = {
  "OK", "ERROR",
  NULL
};

static const signed char JumpArray[] = {
  '\\', 'j', 1, -1
};

static const char * const SearchingResult[] = {
  "+CGREG: 0,1", "+CGREG: 1,1", "+CGREG: 2,1",
  "+CGREG: 0,5", "+CGREG: 1,5", "+CGREG: 2,5",
  "+CGREG: 0,2", "+CGREG: 1,2", "+CGREG: 2,2",
  "+CGREG: 0,0", "+CGREG: 1,0", "+CGREG: 2,0",
  "+CGREG: 0,3", "+CGREG: 1,3", "+CGREG: 2,3",
  NULL
};

static const signed char JumpArray2[] = {
  '\\', 'j', 1, 1, 1, 1, 1, 1, -1, -1, -1,
  -3, -3, -3, -3, -3, -3
};

static const signed char JumpArray3[] = {
  '\\', 'j', 1, -8
};

static const char * const ExpModemConnected[] = {
  "CONNECT", "ERROR", NULL
};

static char gPDPContext[256] = {"AT+CGDCONT=1,\"IP\",\"sgprs\""};
static char gDialNum[256]    = {"ATD*99***1#"};

static const T_EXPECT_SEND KWaveDialScript[] = {
  {NULL, 0, 0, (signed char*)"AT+CGACT=0,1", NULL, 10, 10, 0},   // deactivate PDP Context before dialup  //25-07-17 JC ++
  {NULL, 0, 0, (signed char*)gPDPContext, NULL, 10, 10, 0},     // define context
  {(char **)CommandResult, 2, 5, (signed char*)JumpArray, NULL, 10, 0, 0},
//  {NULL, 0, 0, "\\pAT+CGATT=1", NULL, 20, 3, 0},    // attach gprs
//  {(char **)CommandResult, 2, 5, (signed char*)JumpArray, NULL, 10, 0, 0},
//  {NULL, 0, 0, "AT+CGREG?", NULL, 10, 1, 0},
//  {(char **)SearchingResult, 11, 11, (signed char*)JumpArray2, "ERROR", 30, 0, 0},
  {NULL, 0, 0, (signed char*)gDialNum, "NO CARRIER", 20, 1, 0},
  {(char **)ExpModemConnected, 5, 7, (signed char*)JumpArray3, "NO CARRIER", 30, 1, 1}
};
#define WAVESCP_SIZE    (sizeof(KWaveDialScript)/sizeof(KWaveDialScript[0]))

const struct MDMSREG KDefaultSReg =  {
  sizeof(struct MW_MDM_PORT),                 // len
  {
    0x01,                                     // Protocol = SDLC
    0x01,                                     // Modem Type = CCITT
    0x01,                                     // Speed = 1200
    12,                                       // Inactivity Timeout in unit of 2.5 sec, total 30sec
    15,                                       // Redial Pause time in 100 msec, total 1.5secs */
    0x00,                                     // Primary Tel Len
    {'F'},                                    // Primary Phone #
    0x03,                                     // Primary Dial Attempts
    20,                                       // Primary Connection Time
    0x00,                                     // Secondary Tel Len
    {'F'},                                    // Secondary Phone #
    0x03,                                     // Secondary Dial Attempts
    20,                                       // Secondary Connection Time
    3,                                        // CD on qualify time
    4,                                        // CD off qualify time
    10,                                       // Async Message qualify time in 10msec, total 100ms
    15,                                       // Wait Online time in sec, total 15 secs */
    10,                                       // (DTMF) tone duration in 10msec, total 100ms */
    5,                                        // hold line time in minutes */
    MW_SHORT_DIALTONE,                        // busy tone + check line + blink dialing + short dial tone */
  }
};

static const BYTE *KPPPStatusMsg[7] = {
  "  Disconnected  ",
  "    Calling     ",
  " Authenticating ",
  "   Negotiating  ",
  "    Exiting     ",
  "   Connected    ",
  "  PPP Not Ready ",
};

static const BYTE KGPRSFile[] = {"GPRSFile"};

//*****************************************************************************
//  Function        : InitGPRSDat
//  Description     : Initialize GPRS data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void InitGPRSDat(void)
{
  BYTE filename[32];
  struct GPRSPPP gprs_dat;
  DWORD len;
  int  fd;

  strcpy(filename, KGPRSFile);
  fd = fOpenMW(filename);
  if (fd < 0) {
    fd = fCreateMW(filename, 0);
    memset(&gprs_dat, 0, sizeof(struct GPRSPPP));
    //strcpy(gprs_dat.scAPN, "CMNET");
    strcpy(gprs_dat.scAPN, "CSW0036");
    strcpy(gprs_dat.scDialNum, "*99***1#");
    len = fWriteMW(fd, &gprs_dat, sizeof(struct GPRSPPP));
    if (len != sizeof(struct GPRSPPP)) {
      DispLineMW("File WR Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      ErrorDelay();
    }
    fCommitAllMW();
  }
  else {
    len = fReadMW(fd, &gprs_dat, sizeof(struct GPRSPPP));
    if (len != sizeof(struct GPRSPPP)) {
      DispLineMW("File RD Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      ErrorDelay();
    }
  }
  memcpy(&gGPRSDat, &gprs_dat, sizeof(struct GPRSPPP));
  sprintf(gPDPContext, "AT+CGDCONT=1,\"IP\",\"%s\"", gGPRSDat.scAPN);
  sprintf(gDialNum, "ATD%s", gGPRSDat.scDialNum);
  fCloseMW(fd);
}
//*****************************************************************************
//  Function        : CommInit
//  Description     : Initialize communication data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CommInit(void)
{
  int i;
  memset(&gGPRSDat,   0, sizeof(gGPRSDat));
  memset(&gCommParam, 0, sizeof(gCommParam));
  for (i= 0; i < MAX_APM_AUX; i++)
    gAuxHdl[i] = -1;
 
  InitGPRSDat();
}
//*****************************************************************************
//  Function        : WaitNetworkReady
//  Description     : Wait network ready before connecting PPP
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WaitNetworkReady(void)
{
  #define NET_READY_TIMEOUT 30    // 30 sec to wait for network ready
  DWORD free_run, status;
  
  free_run = FreeRunMW();
  while (((FreeRunMW() - free_run) < NET_READY_TIMEOUT*200)) {
    status = os_gprs_status(-1);
    if (status & K_GPRS_STATUS_TXRDY1) {
      while (((FreeRunMW() - free_run) < NET_READY_TIMEOUT*200)) {
        DispLineMW("  Wait Net Rdy  ", MW_LINE3, MW_CENTER|MW_SPFONT);
        status = os_gprs_status(-1);
        if (status & K_GPRS_STATUS_NETWORK_READY) {
          DispClrLineMW(MW_LINE3);
          Delay10ms(10);
          return TRUE;
        }
      }
    }
  }
  DispClrLineMW(MW_LINE3);
  return FALSE;
}
//*****************************************************************************
//  Function        : WaitPPPReady
//  Description     : Wait PPP connection ready.
//  Input           : aTimeout;     // timeout value seconds.
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD WaitPPPReady(DWORD aTimeout)
{
  DWORD ret, ppp_phase;
  //BYTE  tmpbuf[MW_MAX_LINESIZE+1];

  ppp_phase = IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL);
  if (ppp_phase==MW_PPP_PHASE_READY)
    return COMM_OK;

  ret = COMM_TIMEOUT;
  if ((ppp_phase != MW_PPP_PHASE_IDLE) &&
      (ppp_phase != MW_PPP_PHASE_DEAD) &&
      (ppp_phase != MW_PPP_PHASE_TERMINATE) ) {
    aTimeout *= TIMER_1SEC;
    TimerSetMW(gTimerHdl[TIMER_COMM], aTimeout);
    do {
      ppp_phase = IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL);
      DispLineMW(KPPPStatusMsg[ppp_phase], MW_LINE3, MW_CENTER|MW_SPFONT);
      if (ppp_phase==MW_PPP_PHASE_READY) {
        ret = COMM_OK;
        break;
      }
      SleepMW();
    } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);
  }
  return ret;
}
//*****************************************************************************
//  Function        : ConnectPPP
//  Description     : Make the PPP connection.
//  Input           : aWaitReady;   // Wait PPP ready
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ConnectPPP(BOOLEAN aWaitReady)
{
  struct MW_PPP_DATA ppp_data;
  char   username[256], password[256];
  BYTE   redial[] = { 3, 3, 0, 10, 30 };
  int  fd, len;
  struct GPRSPPP gprs_dat;
  char tmp[32];

  #if (GPRS_SUPPORT)
  // GPRS PPP & NO Sim Card inserted
  if ((gCommParam.sPPP.dDevice == MW_PPP_DEVICE_GPRS) && (StatMW(gDevHandle[APM_SDEV_PPP], MW_GSTS_SIMINSERTED, NULL) == 0))
    return COMM_NO_DIALTONE;
  #endif

  if (WaitPPPReady(gCommParam.bTimeoutVal)==COMM_OK)
    return COMM_OK;

  ppp_data.device     = gCommParam.sPPP.dDevice;
  ppp_data.dev_speed  = gCommParam.sPPP.dSpeed;
  ppp_data.id         = 0;
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_DEVICE, &ppp_data);

  ppp_data.mode = gCommParam.sPPP.dMode;
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_MODE, &ppp_data);
        
  strcpy(username, gCommParam.sPPP.scUserID);
  strcpy(password,  gCommParam.sPPP.scPwd);
  ppp_data.username = username;
  ppp_data.userpwd  = password;
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_LOGINID, &ppp_data);

  if (gCommParam.sPPP.scAPN[0] != 0) {
    sprintf(gPDPContext, "AT+CGDCONT=1,\"IP\",\"%s\"", gCommParam.sPPP.scAPN);
    // Save GPRSFile
    strcpy(tmp, KGPRSFile);
    fd = fOpenMW(tmp);
    if (fd < 0) 
      fd = fCreateMW(tmp, 0);
    if (fd >= 0) {
      strcpy(gGPRSDat.scAPN, gCommParam.sPPP.scAPN);
      strcpy(gGPRSDat.scUserName, gCommParam.sPPP.scUserID);
      strcpy(gGPRSDat.scPassword, gCommParam.sPPP.scPwd);
      memcpy(&gprs_dat, &gGPRSDat, sizeof(struct GPRSPPP)); 
      len = fWriteMW(fd, &gprs_dat, sizeof(struct GPRSPPP));
      if (len != sizeof(struct GPRSPPP)) {
        DispLineMW("File WR Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
        ErrorDelay();
      } 
      fCommitAllMW();
    }
    fCloseMW(fd);
  }

  ppp_data.login_pair    = gCommParam.sPPP.dLoginPair;
  if (gCommParam.sPPP.psLogin == NULL) {
    ppp_data.login_scripts = NULL;
    IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_LOGIN_SCRIPT, &ppp_data);
  }
  else {
    ppp_data.login_scripts = (struct MW_EXPECT_SEND *) expect_send_alloc((T_EXPECT_SEND *)gCommParam.sPPP.psLogin, ppp_data.login_pair);
    IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_LOGIN_SCRIPT, &ppp_data);
    expect_send_release((T_EXPECT_SEND **)ppp_data.login_scripts, ppp_data.login_pair);
  }

  if (gCommParam.sPPP.psDialUp == NULL) {
    ppp_data.dial_pair = WAVESCP_SIZE;
    ppp_data.dial_scripts = (struct MW_EXPECT_SEND *) expect_send_alloc((T_EXPECT_SEND *)KWaveDialScript, ppp_data.dial_pair);
  }
  else {
    ppp_data.dial_pair = gCommParam.sPPP.dDialupPair;
    ppp_data.dial_scripts = (struct MW_EXPECT_SEND *) expect_send_alloc((T_EXPECT_SEND *)gCommParam.sPPP.psDialUp, ppp_data.dial_pair);
  }
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_DIAL_SCRIPT, &ppp_data);
  expect_send_release((T_EXPECT_SEND **)ppp_data.dial_scripts, ppp_data.dial_pair);

  if (gCommParam.sPPP.psDialParam != NULL) {
    memcpy(&ppp_data.line_config, gCommParam.sPPP.psDialParam, sizeof(struct MW_MDM_PORT));
    IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_DIAL_PARAM, &ppp_data);
  }

  ppp_data.reconnect_waittime = 7;
  ppp_data.reconnect_retry   = 0;
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_RECONNECT, &ppp_data);

  ppp_data.redial_param = redial;
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_REDIAL, &ppp_data);

  if (gCommParam.sPPP.bKeepAlive) {
    ppp_data.idle_timeout = 0;
    ppp_data.min_timeout = 0;
  }
  else {
    ppp_data.idle_timeout = 180;
    ppp_data.min_timeout = 0;
  }
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_SET_TIMEOUT, &ppp_data);

  if ((gCommParam.sPPP.dDevice == MW_PPP_DEVICE_GPRS) && (WaitNetworkReady() == FALSE)) {
    return COMM_NO_DIALTONE;
  }
  if (!IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_CONNECT, NULL)) {
    return COMM_NO_DIALTONE;
  }
  if (aWaitReady)
    return WaitPPPReady(gCommParam.bTimeoutVal);
  return COMM_OK;
}
//*****************************************************************************
//  Function        : DisconnectPPP
//  Description     : Disconnect the PPP connection.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD DisconnectPPP(void)
{
  IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_DISCONNECT, NULL);
  return COMM_OK;
}
//*****************************************************************************
//  Function        : ConnectAux
//  Description     : Send Connection command to Aux Port
//  Input           : aWaitReady;   // Wait for line ready.
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD ConnectAux(BOOLEAN aWaitReady)
{
  struct AUXSREG aux_param;
  BYTE filename[32];
  BOOLEAN ret;

  ret = FALSE;
  if (gAuxHdl[gCommParam.sAux.bPort] < 0) {
    switch (gCommParam.sAux.bPort) {
      case APM_AUX2:
        //strcpy(filename, DEV_AUX2);
        strcpy(filename, DEV_AUX0);     //T1000 COM2 is AUX0  //13-08-14 JC ++
        break;
      case APM_AUX1:
      default:
        strcpy(filename, DEV_AUX1);
        break;
    }
    gAuxHdl[gCommParam.sAux.bPort] = OpenMW(filename, MW_RDWR);
    if (gAuxHdl[gCommParam.sAux.bPort] >= 0) {
      // prepare for parameter passing between Appl.
      memcpy(&aux_param, &gCommParam.sAux, sizeof(aux_param));
      if (IOCtlMW(gAuxHdl[gCommParam.sAux.bPort], IO_AUX_CFG, &aux_param.sAuxCfg) >= 0)
        ret = TRUE;
    }
  }
  if (ret == TRUE)
    return COMM_OK;

  return COMM_NOT_CONNECT;
}
//*****************************************************************************
//  Function        : ResetAux
//  Description     : Reset Aux Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ResetAux(void)
{
  CloseMW(gAuxHdl[gCommParam.sAux.bPort]);
  gAuxHdl[gCommParam.sAux.bPort] = -1;
  return TRUE;
}
//*****************************************************************************
//  Function        : SendAux
//  Description     : Send Data to Aux Port
//  Input           : aMsg;         // pointer to COMMBUF data;
//  Return          : TRUE/FALSE;   // TRUE=>sended
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SendAux(struct COMMBUF *aMsg)
{
  BOOLEAN ret;

  ret = FALSE;
  if (gCommParam.sAux.bPort < MAX_APM_AUX) {
    WriteMW(gAuxHdl[gCommParam.sAux.bPort], aMsg->sbContent, aMsg->wLen);
    ret = TRUE;
  }
  return ret;
}
//*****************************************************************************
//  Function        : RcvdAux
//  Description     : Receive Data from Aux Port
//  Input           : aDat;         // pointer to receive buffer
//                    aMaxLen;      // max receive len;
//  Return          : actual len received.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD RcvdAux(BYTE *aDat, WORD aMaxLen)
{
  WORD len, cur;

  len = cur = 0;
  if (gCommParam.sAux.bPort < MAX_APM_AUX) {
    do {
      cur = ReadMW(gAuxHdl[gCommParam.sAux.bPort], aDat+len, aMaxLen-len);
      if (cur) {
        len += cur;
        Delay10ms(5);
      }
    } while (cur>0);
  }
  return len;
}
//*****************************************************************************
//  Function        : FlushAux
//  Description     : Flush incomming AUX data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void FlushAux(void)
{
  WORD len;
  BYTE tmp[1600];

  if (gCommParam.sAux.bPort < MAX_APM_AUX) {
    len = 0;
    do {
      len = ReadMW(gAuxHdl[gCommParam.sAux.bPort], tmp, sizeof(tmp));
    } while (len > 0);
  }
}
//*****************************************************************************
//  Function        : IsLineDropAux
//  Description     : Check for AUX line drop.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : !TODO
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN IsLineDropAux(void)
{
  return FALSE;
}
//*****************************************************************************
//  Function        : ResetTcpip
//  Description     : Reset TCPIP Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ResetTcpip(void)
{
  DWORD status;

  if (gTcpHandle >= 0) {
    IOCtlMW(gTcpHandle, IO_TCP_DISCONNECT, NULL);
    do{
      status = StatMW(gTcpHandle, MW_TCP_STATUS, NULL);
      status &= 0xFF;
    } while ((status != MW_TCP_STAT_IDLE) && (status != MW_TCP_STAT_CLOSED));
    CloseMW(gTcpHandle);
    gTcpHandle = -1;
  }
  if (((gCommParam.sPPP.dDevice&0x0F) != 0) && !gCommParam.sPPP.bKeepAlive) {
    DisconnectPPP();
  }
  return TRUE;
}
//*****************************************************************************
//  Function        : ConnectTcpip
//  Description     : Send Connection command to TCPIP Port
//  Input           : aWaitReady;   // Wait for line ready.
//                    aNoCancel;    // TRUE => No Cancel Allow
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD ConnectTcpip(BOOLEAN aWaitReady, BOOLEAN aNoCancel)
{
  BOOLEAN ret=TRUE;
  DWORD host_timeout;
  BYTE filename[32];
  struct COMMPARAM  comm_param;
  DWORD status;
  struct MW_NIF_INFO netinfo;
  BYTE nif;

  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(KCommConnectWait, MW_LINE5, MW_CENTER|MW_BIGFONT);

  if ((gCommParam.sPPP.dDevice & 0x0F) != 0) {
    if (ConnectPPP(TRUE) != COMM_OK)
      return COMM_NO_LINE;
  }
  
  //07-12-16 JC ++
  nif = getTCP_NIF(&gCommParam.sTcp.sTcpCfg);
  // check LAN cable inserted
  //if (((gCommParam.sTcp.sTcpCfg.b_option & MW_TCP_PPP_MODE) == 0) && !LanCableInsertedMW()) {
  if (nif == MW_NIF_ETHERNET) {
    if (!LanCableInsertedMW()) {
      // reset handle whenever LAN cable disconnected
      if (gTcpHandle >= 0) {
        CloseMW(gTcpHandle);
        gTcpHandle = -1;
      }
    }
    if (aWaitReady == FALSE)
      ret = FALSE;
    else {
      // wait until LAN cable is plugged & DHCP ready
      host_timeout = gCommParam.bHostConnTime;
      host_timeout *= TIMER_1SEC;
      if (WaitLanNetRdy(host_timeout, aNoCancel) == FALSE)
        ret = FALSE;
    }
  }
  else if (nif == MW_NIF_WIFI) {
    if (NetInfoMW(MW_NIF_WIFI, &netinfo) && (netinfo.d_ip == 0)) {
      // reset handle whenever Wifi disconnected
      if (gTcpHandle >= 0) {
        CloseMW(gTcpHandle);
        gTcpHandle = -1;
      }
    }
    if (aWaitReady == FALSE)
      ret = FALSE;
    else {
      // wait until DHCP ready
      host_timeout = gCommParam.bHostConnTime;
      host_timeout *= TIMER_1SEC;
      if (WaitWifiNetRdy(host_timeout, aNoCancel) == FALSE)
        ret = FALSE;
    }
  }
#if (MCP_SUPPORT)
  else if (nif == K_NIF_MCP) {
    if (GetMCPStatus() != K_MCP_PHASE_CONNECTED) {
      // reset handle whenever MCP is disconnected
      if (gTcpHandle >= 0) {
        CloseMW(gTcpHandle);
        gTcpHandle = -1;
      }
      ret = FALSE;
    }
    //21-04-17 JC ++
    if (aWaitReady == FALSE)
      ret = FALSE;
    else {
      // wait until MCP ready
      host_timeout = gCommParam.bHostConnTime;
      host_timeout *= TIMER_1SEC;
      if (WaitMcpNetRdy(host_timeout, aNoCancel) == FALSE)
        ret = FALSE;
    }
    //21-04-17 JC ++
  }
#endif
  //07-12-16 JC --
  
  // new make connection
  if (ret) {
    if (gTcpHandle < 0) {
      // open
      strcpy(filename, DEV_TCP1);
      gTcpHandle = OpenMW(filename, MW_RDWR);
      ret = (gTcpHandle >= 0)? TRUE : FALSE;
      // connect
      if (ret) {
        memcpy(&comm_param, &gCommParam, sizeof(struct COMMPARAM));
        if (IOCtlMW(gTcpHandle, IO_TCP_CONNECT2, &comm_param.sTcp.sTcpCfg) < 0) {
          ret = FALSE;
        }
      }
    }
  }

  // wait for connection ready
  if (ret && aWaitReady) {
    host_timeout = gCommParam.bHostConnTime;
    host_timeout *= TIMER_1SEC;
    TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);
    ret = FALSE;
    do {
      SleepMW();
      //15-03-13 JC ++
      status = StatMW(gTcpHandle, MW_TCP_STATUS, NULL);
      if (gCommParam.sTcp.sTcpCfg.b_option & MW_TCP_SSL_MODE) {
        // SSL mode
        if (status & MW_TCP_STAT_SSL_CONNECTED) {
          ret = TRUE;
          break;
        }
      }
      else {
        // normal mode
        if ((status & 0xFF) == MW_TCP_STAT_CONNECTED) {
          ret = TRUE;
          break;
        }
      }
      //15-03-13 JC --
      if (GetCharMW()==MWKEY_CANCL) {
        if (!aNoCancel)
          break;
        LongBeep();
      }
    } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);
  }

  // return
  if (ret) {
    return COMM_OK;
  }
  else {
    ResetTcpip();
    return COMM_NOT_CONNECT;
  }
}
//*****************************************************************************
//  Function        : SendTcpip
//  Description     : Send Data to TCPIP Port
//  Input           : aMsg;         // pointer to COMMBUF data;
//  Return          : TRUE/FALSE;   // TRUE=>sended
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SendTcpip(struct COMMBUF *aMsg)
{
  BOOLEAN ret=TRUE;

  if (WriteMW(gTcpHandle, aMsg->sbContent, aMsg->wLen) != aMsg->wLen) {
    ret = FALSE;
  }
  return ret;
}
//*****************************************************************************
//  Function        : RcvdTcpip
//  Description     : Receive Data from TCPIPPort
//  Input           : aDat;         // pointer to receive buffer
//                    aMaxLen;      // max receive len;
//  Return          : actual len received.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD RcvdTcpip(BYTE *aDat, WORD aMaxLen)
{
  WORD len, cur;

  len = cur = 0;
  do {
    cur = ReadMW(gTcpHandle, aDat+len, aMaxLen-len);
    if (cur) {
      len += cur;
      Delay10ms(5);
    }
  } while (cur>0);

  return len;
}
//*****************************************************************************
//  Function        : FlushTcpip
//  Description     : Flush incomming TCPIP data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void FlushTcpip(void)
{
  BYTE tmp[1600];

  if (gTcpHandle < 0)
    return;

  while (ReadMW(gTcpHandle, tmp, sizeof(tmp)) > 0) {
    SleepMW();
  }
}
//*****************************************************************************
//  Function        : IsLineDropTcpip
//  Description     : Check for TCPIP line drop.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN IsLineDropTcpip(void)
{
  BOOLEAN ret=TRUE;
  int status;

  if (gCommParam.bCommMode==APM_COMM_GPRS) {
    if (IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL)!=MW_PPP_PHASE_READY)
      return FALSE;
  }

  status = StatMW(gTcpHandle, MW_TCP_STATUS, NULL);
  //15-03-13 JC ++
  if (gCommParam.sTcp.sTcpCfg.b_option & MW_TCP_SSL_MODE) {
    // SSL mode
    if (status & MW_TCP_STAT_SSL_CONNECTED)
      ret = FALSE;
  }
  else {
    // normal mode
    if ((status & 0xFF) == MW_TCP_STAT_CONNECTED)
      ret = FALSE;
  }
  //15-03-13 JC --
  return ret;
}
//*****************************************************************************
//  Function        : GetMdmLineStatusRsp
//  Description     : Convert Line Status to response code.
//  Input           : aStatus;        // mdm line status.
//  Return          : response code.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD GetMdmLineStatusRsp(DWORD aStatus)
{
  DWORD dial_status;
  WORD  ret_val;

  ret_val = COMM_NOT_CONNECT;  // Default
  dial_status = (aStatus & 0xFF);
  if ((dial_status >= 0x80) || (aStatus & 0xFF00)){
    ret_val = COMM_NOT_CONNECT;  // Default
    switch (dial_status) {
      case MW_LINE_CD_LOST :
        ret_val = COMM_LINEDROP;
        break;
      case MW_LINE_HOST_OFFLINE:
        ret_val = COMM_HOST_OFF;
        break;
      case MW_LINE_INACT_TIMEOUT:
      case MW_LINE_CONN_TIMEOUT:
        ret_val = COMM_TIMEOUT;
        break;
    }
    switch (aStatus & 0xFF00) {
      case MW_LINE_OCCUPIED     :
        ret_val = COMM_LINEOCCP;
        break;
      case MW_LINE_NO_LINE      :
        ret_val = COMM_NO_LINE;
        break;
      case MW_LINE_NO_DAILTONE  :
        ret_val = COMM_NO_DIALTONE;
        break;
      case MW_LINE_BUSY         :
        ret_val = COMM_LINEBUSY;
        break;
    }
  }
  return ret_val;
}
//*****************************************************************************
//  Function        : ResetMdmLine
//  Description     : Reset Modem Line.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ResetMdmLine(void)
{
  DWORD hold_line;

  hold_line = FALSE;
  if (gDevHandle[APM_SDEV_MDM] >= 0) {
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_DISCONNECT, &hold_line);
    CloseMW(gDevHandle[APM_SDEV_MDM]);            //25-07-17 JC ++
    gDevHandle[APM_SDEV_MDM] = -1;
  }
  return TRUE;
}
//*****************************************************************************
//  Function        : ConnectMdmLine
//  Description     : Send Connection command to Modem Port
//  Input           : aWaitReady;   // Wait for line ready.
//                    aNoCancel;    // TRUE => User cancel Not allow
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD ConnectMdmLine(BOOLEAN aWaitReady, BOOLEAN aNoCancel)
{
  DWORD host_timeout, status, dial_status;
  WORD  ret_val;
  BYTE filename[32];
  struct COMMPARAM  comm_param;
  BYTE tmp[MW_MAX_LINESIZE+1];

  if (gDevHandle[APM_SDEV_MDM] < 0) {
    // open
    strcpy(filename, DEV_MDM1);
    gDevHandle[APM_SDEV_MDM] = OpenMW(filename, MW_RDWR);
  }

  if (gDevHandle[APM_SDEV_MDM] < 0) {
    return COMM_NOT_CONNECT;
  }

  #if (R700)
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SET_OPR_MODE, 22);    // Set Data Mode
  #endif

  //if (!aWaitReady)
  //  return COMM_OK;

  host_timeout = (gCommParam.sMdm.sMdmCfg.b_pconn_limit * gCommParam.sMdm.sMdmCfg.b_pconn_tval) +
                 (gCommParam.sMdm.sMdmCfg.b_sconn_limit * gCommParam.sMdm.sMdmCfg.b_sconn_tval) +
                 (gCommParam.sMdm.sMdmCfg.b_redial_pause_tval/10 * (gCommParam.sMdm.sMdmCfg.b_pconn_tval + gCommParam.sMdm.sMdmCfg.b_sconn_tval));
  host_timeout *= TIMER_1SEC;

  TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);

  ret_val = COMM_NOT_CONNECT;
  while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0) {
    SleepMW();

    // Check User Cancel
    if (!aNoCancel) {
      if (GetCharMW() == MWKEY_CANCL)
        break;
    }

    // Show Status DWORD
    status = StatMW(gDevHandle[APM_SDEV_MDM], 0, NULL);
    if (aWaitReady) {           //15-03-13 JC ++
      sprintf(tmp, "Status: %08X", status);
      DispLineMW(tmp, MW_LINE8, MW_REVERSE|MW_CENTER|MW_SPFONT);
    }

    if (status & (MW_LINE_WAIT_DISC | MW_LINE_WAIT_DIAL))
      continue;

    dial_status = (status & 0xFF);
    //26-11-14 JC ++
    if (((dial_status >= 0x80) || (status & 0xFF00)) && (status & MW_LINE_DIALING_P2)) {
      ret_val = GetMdmLineStatusRsp(status);
      LongBeep();
      break;
    }
    //26-11-14 JC --

    // Idle
    if (dial_status == MW_LINE_IDLE) {
      memcpy(&comm_param, &gCommParam, sizeof(comm_param));
      IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CONNECT, &comm_param.sMdm.sMdmCfg);
      if (!aWaitReady)          //15-03-13 JC ++
        return COMM_OK;
    }
    // Connected
    if (dial_status == MW_LINE_CONNECTED) {
      ret_val = COMM_OK;
      break;
    }
  }

  if (ret_val != COMM_OK) {
    ResetMdmLine();
  }

  return ret_val;
}
//*****************************************************************************
//  Function        : SendMdmLine
//  Description     : Send Data to Modem Line Port
//  Input           : aMsg;         // pointer to COMMBUF data;
//  Return          : TRUE/FALSE;   // TRUE=>sended
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SendMdmLine(struct COMMBUF *aMsg)
{
  BOOLEAN ret=TRUE;

  if (WriteMW(gDevHandle[APM_SDEV_MDM], aMsg->sbContent, aMsg->wLen) != aMsg->wLen) {
    ret = FALSE;
  }
  return ret;
}
//*****************************************************************************
//  Function        : RcvdMdmLine
//  Description     : Receive Data from Modem Line.
//  Input           : aDat;         // pointer to receive buffer
//                    aMaxLen;      // max receive len;
//  Return          : actual len received.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static WORD RcvdMdmLine(BYTE *aDat, WORD aMaxLen)
{
  WORD len, cur;

  len = cur = 0;
  do {
    cur = ReadMW(gDevHandle[APM_SDEV_MDM], aDat+len, aMaxLen-len);
    if (cur) {
      len += cur;
      if (len >= aMaxLen)
        break;
      Delay10ms(5);
    }
  } while (cur>0);

  return len;
}
//*****************************************************************************
//  Function        : FlushMdmLine
//  Description     : Flush incomming Modem Line.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void FlushMdmLine(void)
{
  BYTE tmp[1600];

  if (gDevHandle[APM_SDEV_MDM] < 0)
    return;

  while (ReadMW(gDevHandle[APM_SDEV_MDM], tmp, sizeof(tmp)) > 0) {
    SleepMW();
  }
}
//*****************************************************************************
//  Function        : IsMdmLineDrop
//  Description     : Check for Modem line drop.
//  Input           : aStatus;        // current modem status;
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN IsMdmLineDrop(DWORD *aStatus)
{
  BOOLEAN ret=FALSE;

  *aStatus = (WORD) StatMW(gDevHandle[APM_SDEV_MDM], 0, NULL);
  if (*aStatus != MW_LINE_CONNECTED) {
    ret = TRUE;
  }
  return ret;
}
//******************************************************************************
//  Function        : ConvB2Comma
//  Description     : Convert the special character in buffer.
//  Input           : aPtr;    // pointer to buffer need to be convert.
//  Return          : N/A
//  Note            : 'B' => ','
//                    'D' => '*'
//                    'E' => '#'
//  Globals Changed : N/A
//******************************************************************************
static void ConvB2Comma(BYTE *aPtr)
{
  while (*aPtr) {
    if (*aPtr == 'B') *aPtr = ',';
    else if (*aPtr == 'D') *aPtr = '*';
    else if (*aPtr == 'E') *aPtr = '#';
    aPtr++;
  }
}
//******************************************************************************
//  Function        : PackTelNo
//  Description     : Pack telephone number together AT cmd & pabx.
//  Input           : aTermData
//                    aTelBuf;       // pointer to result buffer.
//                    aTel;          // pointer to tel num with 'F' packed.
//  Return          : length of packed buffer.
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
BYTE PackTelNo(struct TERM_DATA aTermData, BYTE *aTelBuf, BYTE *aTel)
{
  BYTE count;

  memset(aTelBuf,0,34);
  count = 0;
  aTelBuf[count++] = aTermData.b_dial_mode == 0 ? 'T' : 'P';
  split(&aTelBuf[count], aTermData.sb_pabx, 4);
  count = (BYTE)fndb(aTelBuf,'F', 9);
  split(&aTelBuf[count],aTel,12);
  count = (BYTE) fndb(aTelBuf, 'F', (BYTE)(count+24));
  aTelBuf[count] = 0;
  ConvB2Comma(aTelBuf);

  return(count);
}
//*****************************************************************************
//  Function        : SetGPRS
//  Description     : Setup GPRS communication related parameters
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SetGPRS(void)
{
  int  fd, len;
  struct GPRSPPP gprs_dat;
  char tmp[256];
  DWORD status;
  int iOpt;
  BYTE bSimNum;

  DispLineMW("GPRS Setup", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);

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
        if (IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL) == MW_PPP_PHASE_READY) {
          IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_DISCONNECT, NULL);
          while (1) {
            status = IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL);
            if ((status == MW_PPP_PHASE_IDLE) || (status == MW_PPP_PHASE_DEAD))// || (status == MW_PPP_PHASE_TERMINATE))
              break;
            if (GetCharMW() != MWKEY_CANCL)
              break;
          }
        }
        DispLineMW("Pls Wait...", MW_LINE3, MW_CENTER|MW_BIGFONT);
        bSimNum = iOpt;
        IOCtlMW(gDevHandle[APM_SDEV_GPRS], IO_GPRS_SIM_SEL, &bSimNum);      
      }
      DispClrBelowMW(MW_LINE3);
    }
    else
      return FALSE;
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
        if (IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL) == MW_PPP_PHASE_READY) {
          IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_DISCONNECT, NULL);
          while (1) {
            status = IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL);
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
      return FALSE;
  }
  #endif
  
  // APN
  DispLineMW(KApn, MW_LINE5, MW_BIGFONT);
  strcpy(&tmp[1], gGPRSDat.scAPN);
  tmp[0] = strlen(gGPRSDat.scAPN);
  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
    return FALSE;
  memset(gGPRSDat.scAPN, 0x00, sizeof(gGPRSDat.scAPN));
  memcpy(gGPRSDat.scAPN, &tmp[1], tmp[0]);
  sprintf(gPDPContext, "AT+CGDCONT=1,\"IP\",\"%s\"", gGPRSDat.scAPN);

  // Login Name
  DispClrBelowMW(MW_LINE3);
  DispLineMW(KUserName, MW_LINE5, MW_BIGFONT);
  tmp[0]=strlen(gGPRSDat.scUserName);
  strcpy(&tmp[1], gGPRSDat.scUserName);
  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
    return FALSE;
  memset(gGPRSDat.scUserName, 0x00, sizeof(gGPRSDat.scUserName));
  memcpy(gGPRSDat.scUserName, &tmp[1], tmp[0]);

  // Login Password
  DispClrBelowMW(MW_LINE3);
  DispLineMW(KPassword, MW_LINE5, MW_BIGFONT);
  strcpy(&tmp[1], gGPRSDat.scPassword);
  tmp[0] = strlen(gGPRSDat.scPassword);
  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
    return FALSE;
  memset(gGPRSDat.scPassword, 0x00, sizeof(gGPRSDat.scPassword));
  memcpy(gGPRSDat.scPassword, &tmp[1], tmp[0]);

  // Dial Number
  DispClrBelowMW(MW_LINE3);
  DispLineMW(KDialNum, MW_LINE5, MW_BIGFONT);
  strcpy(&tmp[1], gGPRSDat.scDialNum);
  tmp[0] = strlen(gGPRSDat.scDialNum);
  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
    return FALSE;
  memset(gGPRSDat.scDialNum, 0x00, sizeof(gGPRSDat.scDialNum));
  memcpy(gGPRSDat.scDialNum, &tmp[1], tmp[0]);
  sprintf(gDialNum, "ATD%s", gGPRSDat.scDialNum);

  // Save GPRSFile
  strcpy(tmp, KGPRSFile);
  fd = fOpenMW(tmp);
  if (fd < 0) 
    fd = fCreateMW(tmp, 0);
  if (fd >= 0) {
    memcpy(&gprs_dat, &gGPRSDat, sizeof(struct GPRSPPP)); 
    len = fWriteMW(fd, &gprs_dat, sizeof(struct GPRSPPP));
    if (len != sizeof(struct GPRSPPP)) {
      DispLineMW("File WR Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      ErrorDelay();
    } 
    fCommitAllMW();
  }
  fCloseMW(fd);
  return TRUE;
}
//******************************************************************************
//  Function        : PackInitComm
//  Description     : Pack communication param.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : comm;
//******************************************************************************
void PackInitComm(void)
{
  struct TERM_DATA term_data;

  if (!GetTermData(&term_data))
    return;

  // common
  memset(&gCommParam, 0, sizeof(gCommParam));
  gCommParam.wLen          = sizeof(struct COMMPARAM);
  gCommParam.bCommMode     = term_data.b_init_mode;
  gCommParam.bHostConnTime = 60;
  gCommParam.bTimeoutVal   = 30;
  gCommParam.bAsyncDelay = term_data.b_async_delay;

  // Modem setting
  memcpy((BYTE *)&gCommParam.sMdm, (BYTE *)&KDefaultSReg, sizeof(struct MDMSREG));
  if (gCommParam.bCommMode == APM_COMM_ASYNC) {
    gCommParam.sMdm.sMdmCfg.b_protocol = TYPE_ASYN;
    gCommParam.sMdm.sMdmCfg.b_ccitt    = MODE_CCITT;
    gCommParam.sMdm.sMdmCfg.b_speed    = BPS_AUTO_V42;    //04-12-13 JC ++
  }
  else {
    gCommParam.sMdm.sMdmCfg.b_protocol = TYPE_SDLC;
    gCommParam.sMdm.sMdmCfg.b_ccitt    = MODE_CCITT;
    gCommParam.sMdm.sMdmCfg.b_speed    = BPS_1200;
  }
  gCommParam.sMdm.sMdmCfg.b_ptel_len     = PackTelNo(term_data, gCommParam.sMdm.sMdmCfg.s_ptel, term_data.sb_pri_no);
  gCommParam.sMdm.sMdmCfg.b_pconn_tval   = term_data.b_pri_conn_time;
  gCommParam.sMdm.sMdmCfg.b_pconn_limit  = term_data.b_pri_redial;
  gCommParam.sMdm.sMdmCfg.b_stel_len     = PackTelNo(term_data, gCommParam.sMdm.sMdmCfg.s_stel, term_data.sb_sec_no);
  gCommParam.sMdm.sMdmCfg.b_sconn_tval   = term_data.b_sec_conn_time;
  gCommParam.sMdm.sMdmCfg.b_sconn_limit  = term_data.b_sec_redial;

  // TCP setting
  gCommParam.sTcp.bLen                = sizeof(gCommParam.sTcp);
  memcpy(&gCommParam.sTcp.sTcpCfg.d_ip, term_data.sb_ip, 4);
  gCommParam.sTcp.sTcpCfg.w_port     = WGet(term_data.sb_port);
  if (gCommParam.bCommMode == APM_COMM_GPRS) 
    gCommParam.sTcp.sTcpCfg.b_option   = MW_TCP_PPP_MODE;
#if (MCP_SUPPORT)
  else if (gCommParam.bCommMode == APM_COMM_BT) {
    setTCP_NIF(&gCommParam.sTcp.sTcpCfg, K_NIF_MCP);
  }
#endif
  else if (gCommParam.bCommMode == APM_COMM_WIFI) {
    setTCP_NIF(&gCommParam.sTcp.sTcpCfg, K_NIF_WIFI);
  }
  else
    gCommParam.sTcp.sTcpCfg.b_option   = 0;
  gCommParam.sTcp.sTcpCfg.b_sslidx   = 0;
  if ((gCommParam.bCommMode==APM_COMM_BT) || (gCommParam.bCommMode==APM_COMM_WIFI) || 
      (gCommParam.bCommMode==APM_COMM_TCPIP) || (gCommParam.bCommMode==APM_COMM_GPRS))
    gCommParam.sTcp.bAdd2ByteLen = 1;
  else
    gCommParam.sTcp.bAdd2ByteLen = 0;

  // Aux setting
  gCommParam.sAux.bLen               = sizeof(gCommParam.sAux);
  gCommParam.sAux.bPort              = APM_AUX1;//term_data.sb_port[0] + 1;
  gCommParam.sAux.sAuxCfg.b_len      = sizeof(struct MW_AUX_CFG);
  gCommParam.sAux.sAuxCfg.b_mode     = MW_AUX_ENAC;//MW_AUX_NO_PARITY;
  gCommParam.sAux.sAuxCfg.b_speed    = MW_AUX_115200; //term_data.sb_port[1];
  gCommParam.sAux.sAuxCfg.b_rx_gap   = 50;   // 50 ms
  gCommParam.sAux.sAuxCfg.b_rsp_gap  = 150;
  gCommParam.sAux.sAuxCfg.b_tx_gap   = 50;
  gCommParam.sAux.sAuxCfg.b_retry    = 0;

  // Config PPP parameter
  gCommParam.sPPP.bKeepAlive  = TRUE; // Never disconnect
  if (gCommParam.bCommMode == APM_COMM_GPRS) 
    gCommParam.sPPP.dDevice      = MW_PPP_DEVICE_GPRS;
  else
    gCommParam.sPPP.dDevice      = MW_PPP_DEVICE_NONE;
  gCommParam.sPPP.dSpeed       = 230400;
  gCommParam.sPPP.dMode        = MW_PPP_MODE_NORMAL;
  memset(gCommParam.sPPP.scUserID, 0, sizeof(gCommParam.sPPP.scUserID));
  memset(gCommParam.sPPP.scPwd,    0, sizeof(gCommParam.sPPP.scPwd));
  gCommParam.sPPP.psLogin      = NULL;
  gCommParam.sPPP.dLoginPair   = 0;
  gCommParam.sPPP.psDialUp     = NULL;
  gCommParam.sPPP.dDialupPair  = 0;
  gCommParam.sPPP.psDialParam  = NULL;
}
//*****************************************************************************
//  Function        : PackComm
//  Description     : Pack the comm parameter.
//  Input           : aDat;       // pointer to comm parameters.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void  PackComm(struct COMMPARAM *aDat)
{
  memset(&gCommParam, 0, sizeof(gCommParam));

  // Use Test Setup ?
  if ((memcmp(&gCommParam, aDat, sizeof(gCommParam))==0)||gReplaceAllComm) {
    // For Demo ONLY
    SetTestCommParam(&gCommParam);
    return;
  }
  
  memcpy(&gCommParam, aDat, sizeof(gCommParam));
}
//*****************************************************************************
//  Function        : PreConnect
//  Description     : PreConnect
//  Input           : Make a Pre-Connection.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void  PreConnect(void)
{
  if (TrainingModeON())
    return;

  switch (gCommParam.bCommMode) {
    case APM_COMM_AUX :
      ConnectAux(FALSE);
      break;
    case APM_COMM_BT:
    case APM_COMM_WIFI:
    case APM_COMM_TCPIP:
      ConnectTcpip(FALSE, FALSE);
      break;
    case APM_COMM_SYNC:
    case APM_COMM_ASYNC:
      ConnectMdmLine(FALSE, FALSE);
      break;
    default :
      break;
  }
}
//*****************************************************************************
//  Function        : ConnectOK
//  Description     : Make connection and return status.
//  Input           : aNoCancel;    // User Cancel allow or not (TRUE/FALSE)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ConnectOK(DWORD aNoCancel)
{
  DWORD ret;

  if (TrainingModeON()) {
    DispClrBelowMW(MW_LINE3);
    DispLineMW(KConnected, MW_LINE5, MW_CENTER|MW_BIGFONT);
    return COMM_OK;
  }

  if (!Return2Base()) {
    return COMM_NO_LINE;
  }

  ret = COMM_NO_LINE;
  switch (gCommParam.bCommMode) {
    case APM_COMM_AUX:
      ret = ConnectAux(TRUE);
      break;
    case APM_COMM_BT:
    case APM_COMM_WIFI:
    case APM_COMM_GPRS:
    case APM_COMM_TCPIP:
      ret = ConnectTcpip(TRUE, aNoCancel);
      break;
    case APM_COMM_SYNC:
    case APM_COMM_ASYNC:
      ret = ConnectMdmLine(TRUE, aNoCancel);
      break;
    default:
      break;
  }
  return ret;
}
//*****************************************************************************
//  Function        : FlushComm
//  Description     : Flush Communication buffer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void  FlushComm(void)
{
  if (TrainingModeON())
    return;

  switch (gCommParam.bCommMode) {
    case APM_COMM_AUX:
      FlushAux();
      break;
    case APM_COMM_BT:
    case APM_COMM_WIFI:
    case APM_COMM_GPRS :
    case APM_COMM_TCPIP:
      FlushTcpip();
      break;
    case APM_COMM_SYNC:
    case APM_COMM_ASYNC:
      FlushMdmLine();
      break;
  }
}
//*****************************************************************************
//  Function        : SendRcvd
//  Description     : Send and wait response from host.
//  Input           : aSend;            // pointer to send buffer
//                    aRcvd;            // pointer to receive buffer.
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD SendRcvd(struct COMMBUF *aSend, struct COMMBUF *aRcvd)
{
  WORD ret;
  BYTE  refresh_rate = 5;
  DWORD toggle = 0;
  DWORD host_timeout, status;

  DispClrBelowMW(MW_LINE3);
  DispLineMW(KConnected, MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (TrainingModeON()) {
    DispLineMW(KCommWait, MW_LINE7, MW_CENTER|MW_BIGFONT);
    Delay1Sec(1, 0);
    return COMM_OK;
  }

  if ((aSend == NULL) && (aRcvd == NULL))
    return COMM_OK;

  CheckPointerAddr(aSend);
  CheckPointerAddr(aRcvd);

  if (aSend!=NULL) {
    DispLineMW(KCommSend,  MW_LINE7, MW_CENTER|MW_BIGFONT);
  }
  else
    DispClrLineMW(MW_LINE7);
  Delay10ms(50);

  // Send Msg
  if (aSend != NULL) {
    switch (gCommParam.bCommMode) {
      case APM_COMM_AUX :
        SendAux(aSend);
        break;
      case APM_COMM_BT:
      case APM_COMM_WIFI:
      case APM_COMM_GPRS  :
      case APM_COMM_TCPIP :
        SendTcpip(aSend);
        break;                  //15-03-13 JC ++
      case APM_COMM_SYNC  :
      case APM_COMM_ASYNC :
        SendMdmLine(aSend);
        break;
      default :
        return COMM_NOT_CONNECT;
    }
  }

  if (aRcvd == NULL)
    return COMM_OK;

  aRcvd->wLen = 0;
  host_timeout = gCommParam.bTimeoutVal;
  host_timeout *= TIMER_1SEC;
  TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);
  ret = COMM_TIMEOUT;
  do {
    // Check Receive Msg
    switch (gCommParam.bCommMode) {
      case APM_COMM_AUX :
        aRcvd->wLen = RcvdAux(aRcvd->sbContent, COMMBUF_SIZE);
        refresh_rate = 5;
        break;
      case APM_COMM_BT  :
      case APM_COMM_WIFI  :
      case APM_COMM_GPRS  :
      case APM_COMM_TCPIP :
        aRcvd->wLen = RcvdTcpip(aRcvd->sbContent, COMMBUF_SIZE);
        refresh_rate = 5;
        break;
      case APM_COMM_SYNC :
      case APM_COMM_ASYNC:
        aRcvd->wLen = RcvdMdmLine(aRcvd->sbContent, COMMBUF_SIZE);
        refresh_rate = 5;
        break;
    }
    if (aRcvd->wLen) {
      DispLineMW(KCommRcvd,  MW_LINE7, MW_CENTER|MW_BIGFONT);
      ret = COMM_OK;
      break;
    }

    //Check Line drop
    switch (gCommParam.bCommMode) {
      case APM_COMM_AUX :
        if (IsLineDropAux()) {
          ret = COMM_LINEDROP;
          return ret;
        }
        break;
      case APM_COMM_BT    :
      case APM_COMM_WIFI  :
      case APM_COMM_GPRS  :
      case APM_COMM_TCPIP :
        if (IsLineDropTcpip()) {
          ret = COMM_LINEDROP;
          return ret;
        }
        break;
      case APM_COMM_SYNC :
      case APM_COMM_ASYNC:
        if (IsMdmLineDrop(&status)) {
          ret = GetMdmLineStatusRsp(status);
          return ret;
        }
        break;
    }

    // Check Cancel Key
    if (GetCharMW() == MWKEY_CANCL) {
      Disp2x16Msg(KCommNoCancel, MW_LINE5, MW_CENTER|MW_BIGFONT);
      Delay1Sec(1, TRUE);
      DispLineMW(KConnected, MW_LINE5, MW_CENTER|MW_BIGFONT);
      DispClrBelowMW(MW_LINE7);
    }

    // Toggle Message
    if (TimerGetMW(gTimerHdl[TIMER_COMM]) != host_timeout) {
      if (TimerGetMW(gTimerHdl[TIMER_COMM])%refresh_rate == 0) {
        if ((toggle++ % 20) < 10) {
          DispLineMW(KCommWait,  MW_LINE7, MW_CENTER|MW_BIGFONT);
        }
        else
          DispClrLineMW(MW_LINE7);
      }
    }
    SleepMW();
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  return ret;
}
//*****************************************************************************
//  Function        : ResetComm
//  Description     : Reset the communication line.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ResetComm(void)
{
  DWORD ret;

  if (TrainingModeON())
    return TRUE;

  switch (gCommParam.bCommMode) {
    case APM_COMM_AUX :
      ret = ResetAux();
      break;
    case APM_COMM_BT    :
    case APM_COMM_WIFI  :
    case APM_COMM_GPRS  :
    case APM_COMM_TCPIP :
      ret = ResetTcpip();
      break;
    case APM_COMM_SYNC :
    case APM_COMM_ASYNC:
      ret = ResetMdmLine();
      break;
    default :
      ret = FALSE;
      break;
  }
  return ret;
}
//*****************************************************************************
//  Function        : SetGPRSInfo
//  Description     : Set the GPRS parameters.
//  Input           : username = username
//                    password = password
//                    apn = apn
//                    dialNum = dialNum
//  Return          : N/A
//  Note            : any parameter input with 0 will not be changed
//  Globals Changed : N/A
//*****************************************************************************
void SetGPRSInfo(const char* userName, const char* password, const char* apn, const char* dialNum)
{
  if (userName)
    strcpy(gGPRSDat.scUserName, userName);

  if (password)
    strcpy(gGPRSDat.scPassword, password);

  if (apn)
    strcpy(gGPRSDat.scAPN, apn);

  if (dialNum)
    strcpy(gGPRSDat.scDialNum, dialNum);
}

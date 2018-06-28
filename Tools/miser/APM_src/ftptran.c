//-----------------------------------------------------------------------------
//  File          : FtpTran.c
//  Module        :
//  Description   : FTP Init Transactions..
//  Author        : John
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
//  18 Dec 2014   John        Initial Version.
//-----------------------------------------------------------------------------
#include <ctype.h>
#include "midware.h"
#include "emv2dll.h"
#include "emvcl2dll.h"
#include "util.h"
#include "hardware.h"
#include "sysutil.h"
#include "menu.h"
#include "termdata.h"
#include "stis.h"
#include "tranutil.h"
#include "comm.h"
#include "constant.h"
#include "emvtrans.h"
#include "batchsys.h"
#include "coremain.h"
#include "message.h"
#include "ftptran.h"
#include "kbdutil.h"
#include "remotedl.h"
#include "ctlcfg.h"
#include "param.h"
#include "cakey.h"
#include "auxcom.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define SVR_PATH              "./"

#if (T1000)
#define TERM_MODEL            "T1000"
#elif (A5T1000)
#define TERM_MODEL            "T1000-A5"
#elif (T300)
#define TERM_MODEL            "T300"
#endif

enum {
  IDX_EDC      = 0,
  IDX_EMV_APP     ,
  IDX_EMV_KEY     ,
//  IDX_XAPP        ,
//  IDX_LXAPP       ,
//  IDX_VBIN_VER    ,
  NUM_PARAM_FILES ,
  IDX_APP = NUM_PARAM_FILES,
};

#define MAX_APP_DL          50
#define WAIT_DATA_TIME      30

#define FTP_DL_SUCCESS      '0'*256+'0'
#define FTP_DL_NOT_REQ      '0'*256+'1'
#define FTP_DL_NOT_EXIST    '1'*256+'1'
#define FTP_DL_DL_FAIL      '1'*256+'2'
#define FTP_DL_CHKSUM_FAIL  '1'*256+'3'
#define FTP_DL_SERVER_KEY   '9'*256+'1'
#define FTP_DL_UNEXPECTED   '9'*256+'9'

//#define DBG_AUX(w,x,y,z)    dbgAux(w,x,y,z)
#define DBG_AUX(w,x,y,z)

//-----------------------------------------------------------------------------
//    Global Variables
//-----------------------------------------------------------------------------
static int    gFtpHandle=-1;  //22-04-13++ SC (1)
struct MW_MDM_PORT gPPPMdmCfg; //22-04-13++ SC (1)

static struct COMMBUF *psTxBuf=NULL;
static struct COMMBUF *psRxBuf=NULL;
struct BATCH_NO {
  BYTE sb_curr[3];
  BYTE sb_next[3];
};
static struct BATCH_NO *psBatchNo=NULL;
//static int    iExParamCount=0;
//static int    iLongParamCount=0; //29-11-10 BW (1)

struct DL_FILES_INFO {
  BYTE exist;
  BYTE remote_path[128];
  BYTE remote_file[64];
  BYTE local_file[64];
  BYTE date[4];
  BYTE checksum[4];
  DDWORD size;
  BYTE download_required;
  BYTE updated;
  BYTE default_app;
  BYTE app_name[14];
  WORD result;
};

static struct DL_FILES_INFO DL_file_list[NUM_PARAM_FILES+MAX_APP_DL];

struct FTP_DL_LOG_INFO {
  BYTE tid[8];
  BYTE datetime[14];
  BYTE version[3];
  BYTE mode[2];
  BYTE err;
  BYTE err_desc[64];
  BYTE remote_file_name[64];
};

static struct FTP_DL_LOG_INFO FTP_dl_log;

static char FTP_DL_VERSION[] = {"2.2"};

enum {
  FTP_DL_NO_ERROR = 0            ,
  FTP_DL_FILE_ERROR              ,
  FTP_DOWNLOAD_ERROR             ,
  FTP_DOWNLOAD_CHKSUM_ERROR      ,
  FTP_DOWNLOAD_AUTH_ERROR        ,
  FTP_DOWNLOAD_UNEXPECTED_ERROR  ,
  FTP_NUM_ERROR                  ,
};

static char *FtpDownloadErrorCode[FTP_NUM_ERROR] = {
  "00",
  "11",
  "12",
  "13",
  "91",
  "99",
};

static char *FtpDownloadHdr[] = {
  "DL EDC",
  "DL EMV App",
  "DL EMV Key",
  "DL Application",
};

static const BYTE KFTPConnectingMsg[] = {"FTP: Connecting    "}; //18-07-14++ SC (1)

extern const struct MENU_ITEM KInitItem[];
extern const struct MENU_DAT KInitMenu;

//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------
// Default Init Selection Menu
const BYTE KAuthKey[16] = {
#if (T1000)
  0xF7, 0x75, 0xF6, 0xFF, 0x31, 0x43, 0x43, 0x39,
  0xF8, 0xCE, 0x0B, 0xA5, 0x72, 0x59, 0x83, 0xC4
#elif (A5T1000)
  0x1C, 0x14, 0x8A, 0x11, 0x2F, 0x4D, 0x18, 0x0A,
  0xF8, 0xCE, 0x0B, 0xA5, 0x72, 0x59, 0x83, 0xC4
#elif (T300)
  0xCC, 0x1A, 0xDA, 0x72, 0x6B, 0x8D, 0xA7, 0xEC,
  0xF8, 0xCE, 0x0B, 0xA5, 0x72, 0x59, 0x83, 0xC4
#endif
};

static char KParamEDCFile[]      = { "ftpEDC" };
static char KParamEMVAPPFile[]   = { "ftpEMVA" };
static char KParamEMVKEYFile[]   = { "ftpEMVK" };
static char KFtpDownloadLog[]    = { "ftpLOG" };
static char KFtpMirrorList[]     = { "ftpMList" };

const struct MW_MDM_PORT KPPPDefaultSReg =  {
  0x02,                                                 /* Protocol: ASYNC */
  0x01,                                                 /* CCITT */
  0x05,                                                 /* 2400 baud rate */
  72,                                                   /* Drop after idle, unit in 2.5s, 3 min */
  15,                                                   /* Pause between re-dial, unit in 100ms */
  0x00,                                                 /* Pri-phone number length */
  {'F','F','F','F','F','F','F','F','F','F', // Primary Phone #
   'F','F','F','F','F','F','F','F','F','F',
   'F','F','F','F','F','F','F','F','F','F',
   'F','F','F','F'
  },
  3,                                                    /* Pri-phone# Dial attempts */
  20,                                                   /* Pri-phone# Time to wait CD, unit in 2s */
  0x00,                                                 /* Sec-phone number length */
  {'F','F','F','F','F','F','F','F','F','F', // Secondary Phone #
   'F','F','F','F','F','F','F','F','F','F',
   'F','F','F','F','F','F','F','F','F','F',
   'F','F','F','F'
  },
  3,                                                    /* Sec-phone# Dial attempts */
  20,                                                   /* Sec-phone# Time to wait CD, unit in 2s */
  3,                                                    /* CD on qualify time, unit in 250ms*/
  4,                                                    /* CD off delay, unit in 250ms */
  5,                                                    /* Message qualify time, unit in 10ms */
  15,                                                   /* Wait time for link up after CD, in sec */
  0xff,                                                 /* DTMF duration, unit in 10ms */
  0,                                                    /* Hold line after drop, in sec */
  MW_NO_LINE_CHECK | MW_BLIND_DIALING                   /* Line Detection mode */
};

//*****************************************************************************
//  Function        : DisconnectFtpData
//  Description     : Reset FTP Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetupFtpTls(struct MW_TCP_PORT2 *aTcpPort)
{
  //07-07-2017 JC ++
  //aTcpPort->b_option |= (MW_TCP_NO_CLOSE_WAIT+MW_TCP_SSL_MODE+MW_FTP_SSL_MODE+0x80);
  aTcpPort->b_option |= (MW_TCP_NO_CLOSE_WAIT+MW_TCP_SSL_MODE+MW_FTP_SSL_MODE+0x90);  //enable large MTU
  //07-07-2017 JC --
  aTcpPort->b_eoption = 0;                            // clear eoption
  aTcpPort->b_eoption |= K_TLS_Enable_TLS1_0;         // enable TLS/1.0
  aTcpPort->b_eoption |= K_TLS_Enable_TLS1_1;         // enable TLS/1.1
  aTcpPort->b_eoption |= K_TLS_Enable_TLS1_2;         // enable TLS/1.2
  aTcpPort->b_sslidx = SSL_CA_IDX_TMS;                // ca key idx
  aTcpPort->b_certidx = 0;                            // client cert idx
  aTcpPort->b_keyidx = 0;                             // client key idx
}
//*****************************************************************************
//  Function        : DebugTcpCfg
//  Description     : Debug FTP Connect Info.
//  Input           : N/A
//  Return          : N/A
//  Note            : //29-09-16 JC ++
//  Globals Changed : N/A
//*****************************************************************************
void DebugTcpCfg(struct MW_TCP_PORT2 *aTcpPort, BYTE aRef)
{
  BYTE buf[64];
  
  SprintfMW(buf, "IP  : %d.%d.%d.%d", (BYTE)aTcpPort->d_ip, (BYTE)(aTcpPort->d_ip>>8), (BYTE)(aTcpPort->d_ip>>16), (BYTE)(aTcpPort->d_ip>>24));
  DispLineMW(buf, MW_LINE3, MW_CLRDISP|MW_SPFONT);
  SprintfMW(buf, "Port: %d", aTcpPort->w_port);
  DispLineMW(buf, MW_LINE4, MW_SPFONT);
  SprintfMW(buf, "Optn: %02X", aTcpPort->b_option);
  DispLineMW(buf, MW_LINE5, MW_SPFONT);
  SprintfMW(buf, "SSLx: %02X", aTcpPort->b_sslidx);
  DispLineMW(buf, MW_LINE6, MW_SPFONT);
  SprintfMW(buf, "Ref : %d", aRef);
  DispLineMW(buf, MW_LINE7, MW_SPFONT);
  WaitKey(300);
}

//******************************************************************************
//  Function        : PackFTPInitComm
//  Description     : Pack communication param.
//  Input           : N/A
//  Return          : N/A
//  Note            : 22-04-13++ SC (1)
//  Globals Changed : comm;
//******************************************************************************
void PackFTPInitComm(BOOLEAN aCheckIn, BOOLEAN aFroceDialup)
{
  struct TERM_DATA term_data;
  struct DATETIME dtg;

  DBG_AUX("PackFTPInitComm", NULL, 0, FALSE);
  
  if (!GetTermData(&term_data))
    return;

  // common
  memset(&gCommParam, 0, sizeof(gCommParam));
  gCommParam.wLen          = sizeof(struct COMMPARAM);
  gCommParam.bCommMode     = term_data.b_init_mode;
  gCommParam.bHostConnTime = 60;
  gCommParam.bTimeoutVal   = 60;
  gCommParam.bAsyncDelay   = term_data.b_async_delay;

  // Modem setting
  // We use the default structure for dummy fill only
  // the modem setting will be in PPP setting
  memcpy((BYTE *)&gCommParam.sMdm, (BYTE *)&KDefaultSReg, sizeof(struct MDMSREG));

  // TCP setting
  gCommParam.sTcp.sTcpCfg.w_port     = WGet(term_data.sb_port);
  if (gCommParam.bCommMode == APM_COMM_GPRS)
    gCommParam.sTcp.sTcpCfg.b_option   = MW_TCP_PPP_MODE;
  else if (gCommParam.bCommMode == APM_COMM_WIFI) {
    setTCP_NIF(&gCommParam.sTcp.sTcpCfg, K_NIF_WIFI);
  }
  else
    gCommParam.sTcp.sTcpCfg.b_option   = 0;
  //gCommParam.sTcp.sTcpCfg.b_sslidx   = 0;
  SetupFtpTls(&gCommParam.sTcp.sTcpCfg);
  if (((gCommParam.bCommMode==APM_COMM_TCPIP) || (gCommParam.bCommMode==APM_COMM_GPRS) || (gCommParam.bCommMode==APM_COMM_WIFI)) && !aFroceDialup) {
    gCommParam.sTcp.bLen                = sizeof(gCommParam.sTcp);
    memcpy(&gCommParam.sTcp.sTcpCfg.d_ip, term_data.sb_ip, 4);
    gCommParam.sTcp.bAdd2ByteLen = 1;
  } else {
    memcpy(&gCommParam.sTcp.sTcpCfg.d_ip, term_data.sb_ppp_ip, 4);
    gCommParam.sTcp.sTcpCfg.w_port     = WGet(term_data.sb_ppp_port);
    gCommParam.sTcp.bAdd2ByteLen = 0;
  }

  // Aux setting
  gCommParam.sAux.bLen                = sizeof(gCommParam.sAux);
  gCommParam.sAux.bPort               = term_data.sb_port[0] + 1;
  gCommParam.sAux.sAuxCfg.b_len      = sizeof(struct MW_AUX_CFG);
  gCommParam.sAux.sAuxCfg.b_mode     = MW_AUX_NO_PARITY;
  gCommParam.sAux.sAuxCfg.b_speed    = term_data.sb_port[1];
  gCommParam.sAux.sAuxCfg.b_rx_gap   = 5;   // 50 ms
  gCommParam.sAux.sAuxCfg.b_rsp_gap  = 5;
  gCommParam.sAux.sAuxCfg.b_tx_gap   = 5;
  gCommParam.sAux.sAuxCfg.b_retry    = 0;

  // Config PPP parameter
  if ((term_data.b_init_mode == APM_COMM_ASYNC) || aFroceDialup){

    gCommParam.sPPP.bKeepAlive  = FALSE; // Never disconnect
    gCommParam.sTcp.sTcpCfg.b_option   = MW_TCP_PPP_MODE;
    gCommParam.sPPP.dDevice      = MW_PPP_DEVICE_LINE;
    gCommParam.sPPP.dSpeed       = 115200;
    gCommParam.sPPP.dMode        = MW_PPP_MODE_NORMAL;

    if (term_data.sb_ppp_user[0] == 0x00) {
      ReadRTC(&dtg);

      strcpy(gCommParam.sPPP.scUserID, "stisYYYYMM");
      strcpy(gCommParam.sPPP.scPwd, "YYYYMMstis");

      split(&gCommParam.sPPP.scUserID[4], (BYTE *)&dtg, 3);
      split(&gCommParam.sPPP.scPwd[0], (BYTE *)&dtg, 3);
    } else {
      memcpy(gCommParam.sPPP.scUserID, term_data.sb_ppp_user, sizeof(term_data.sb_ppp_user));
      memcpy(gCommParam.sPPP.scPwd, term_data.sb_ppp_pwd, sizeof(term_data.sb_ppp_pwd));
    }
    memcpy((BYTE *)&gPPPMdmCfg, (BYTE *)&KPPPDefaultSReg, sizeof(struct MW_MDM_PORT));

    gPPPMdmCfg.b_protocol = MW_ASYNC;
    gPPPMdmCfg.b_ccitt    = MW_CCITT;

    //27-06-14++ SC (7)
    // gPPPMdmCfg.b_speed    = BPS_AUTO;
    gPPPMdmCfg.b_speed    = MW_33600;
    //27-06-14-- SC (7)


    if (aCheckIn) {
      //14-07-14++ SC (1)
      //gPPPMdmCfg.b_ptel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_checkin_pri_no);
      //gPPPMdmCfg.b_stel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_checkin_sec_no);
      switch (term_data.sb_term_id[7]) {
        case '1':
        case '3':
        case '5':
        case '7':
        case '9':
        default :
          gPPPMdmCfg.b_ptel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_checkin_pri_no);
          gPPPMdmCfg.b_stel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_checkin_pri_no);
          break;
        case '0':
        case '2':
        case '4':
        case '6':
        case '8':
          gPPPMdmCfg.b_ptel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_checkin_sec_no);
          gPPPMdmCfg.b_stel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_checkin_sec_no);
          break;
      }
      //14-07-14-- SC (1)
      gPPPMdmCfg.b_pconn_tval   = term_data.b_checkin_pri_conn_time;
      gPPPMdmCfg.b_pconn_limit  = term_data.b_checkin_pri_redial;
      gPPPMdmCfg.b_sconn_tval   = term_data.b_checkin_sec_conn_time;
      gPPPMdmCfg.b_sconn_limit  = term_data.b_checkin_sec_redial;
    } else {
      //14-07-14++ SC (1)
      //gPPPMdmCfg.b_ptel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_pri_no);
      //gPPPMdmCfg.b_stel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_sec_no);
      switch (term_data.sb_term_id[7]) {
        case '1':
        case '3':
        case '5':
        case '7':
        case '9':
        default :
          gPPPMdmCfg.b_ptel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_pri_no);
          gPPPMdmCfg.b_stel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_pri_no);
          break;
        case '0':
        case '2':
        case '4':
        case '6':
        case '8':
          gPPPMdmCfg.b_ptel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_sec_no);
          gPPPMdmCfg.b_stel_len     = PackTelNo(term_data, gPPPMdmCfg.s_ptel, term_data.sb_sec_no);
          break;
      }
      //14-07-14-- SC (1)

      gPPPMdmCfg.b_pconn_tval   = term_data.b_pri_conn_time;
      gPPPMdmCfg.b_pconn_limit  = term_data.b_pri_redial;
      gPPPMdmCfg.b_sconn_tval   = term_data.b_sec_conn_time;
      gPPPMdmCfg.b_sconn_limit  = term_data.b_sec_redial;
    }

    gCommParam.sPPP.psLogin      = NULL;
    gCommParam.sPPP.dLoginPair   = 0;
    gCommParam.sPPP.psDialUp     = NULL;
    gCommParam.sPPP.dDialupPair  = 0;
    gCommParam.sPPP.psDialParam  = &gPPPMdmCfg;
  } else {
    gCommParam.sPPP.bKeepAlive  = TRUE; // Never disconnect
    //gCommParam.sTcp.sTcpCfg.b_option = 0;
    if (gCommParam.bCommMode == APM_COMM_GPRS)
      gCommParam.sPPP.dDevice      = MW_PPP_DEVICE_GPRS;
    else {
      gCommParam.sPPP.dDevice      = MW_PPP_DEVICE_NONE;
    }
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
}
//*****************************************************************************
//  Function        : ResetFtp
//  Description     : Reset FTP Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ResetFtp(void)
{
  DWORD channel;

  DBG_AUX("ResetFtp", NULL, 0, FALSE);
  
  if (gFtpHandle >= 0) {
    channel = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &channel);
    SleepMW();

    channel = MW_FTP_CMD_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &channel);
    SleepMW();
    CloseMW(gFtpHandle);
    gFtpHandle = -1;
  }
  if (((gCommParam.sPPP.dDevice&0x0F) != 0) && !gCommParam.sPPP.bKeepAlive) {
    DisconnectPPP();
  }
  return TRUE;
}

//*****************************************************************************
//  Function        : DisconnectFtpData
//  Description     : Reset FTP Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DisconnectFtpData(void)
{
  DWORD channel;

  DBG_AUX("DisconnectFtpData", NULL, 0, FALSE);
  
  if (gFtpHandle >= 0) {
    channel = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &channel);
  }
}

//*****************************************************************************
//  Function        : IsLineDropFtp
//  Description     : Check for TCPIP line drop.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN IsLineDropFtp(void)
{
  BOOLEAN ret=TRUE;
  int status;

  status = StatMW(gFtpHandle, MW_FTP_STATUS, NULL);

//27-06-14++ SC (5)
//  if ((status != MW_FTP_STAT_CONNECTED) || (status != MW_FTP_STAT_DATA_CONNED)) {
  if (((status & 0x7F) != MW_FTP_STAT_IDLE) && ((status & 0x7F) != MW_FTP_STAT_CLOSING)) {
//27-06-14-- SC (5)
    ret = FALSE;
  }
  return ret;
}

//*****************************************************************************
//  Function        : SendFtp
//  Description     : Send Cmd Data to Ftp Port
//  Input           : aMsg;         // pointer to COMMBUF data;
//  Return          : TRUE/FALSE;   // TRUE=>sended
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SendFtp(BYTE *aDat)
{
  BOOLEAN ret=TRUE;

  if (IOCtlMW(gFtpHandle, IO_FTP_CMD_SEND, aDat) != 1) {
    ret = FALSE;
  }
  return ret;
}
//*****************************************************************************
//  Function        : RcvdFtp
//  Description     : Receive Cmd Data from Ftp Port
//  Input           : aDat;         // pointer to receive buffer
//                    aMaxLen;      // max receive len;
//  Return          : actual len received.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD RcvdFtp(BYTE *aDat)
{
  int len;

  len = IOCtlMW(gFtpHandle, IO_FTP_CMD_READ, aDat);
  
  if (len < 0) {
    len = -1;
  }
  return (WORD)len;
}

//*****************************************************************************
//  Function        : WaitCmdChnReady
//  Description     : wait for FTP control channel ready for next command signal
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void WaitCmdChnReady(DWORD aTimeout)
{
  DWORD status;
  DWORD start_cnt;

  start_cnt = FreeRunMark();
  do {
    // MW_FTP_STATUS: bit 0x80 cmd channel busy bit flag.
    status = StatMW(gFtpHandle, MW_FTP_STATUS, NULL);
    if ((status & 0x80) == 0)
      break;
    SleepMW();
  } while (FreeRunElapsed(start_cnt) < aTimeout);
}

//*****************************************************************************
//  Function        : FlushAllFtp
//  Description     : Flush All Channel.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void FlushAllFtp(void)
{
  BYTE tmp[2048];

  DBG_AUX("FlushAllFtp", NULL, 0, FALSE);
  
  while (IOCtlMW(gFtpHandle, IO_FTP_CMD_READ, tmp) > 0)
    SleepMW();

  while (ReadMW(gFtpHandle, tmp, sizeof(tmp)) > 0)
    SleepMW();

  WaitCmdChnReady(3000);    // make sure control channel is free
}

//*****************************************************************************
//  Function        : SendRcvdFtp
//  Description     : Send and wait response from Ftp host.
//  Input           : aSend;            // pointer to send buffer
//                    aRcvd;            // pointer to receive buffer.
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD SendRcvdFtpCmd(struct COMMBUF *aSend, struct COMMBUF *aRcvd)
{
  WORD ret;
  //BYTE  refresh_rate = 5;
  DWORD host_timeout;

  if ((aSend == NULL) && (aRcvd == NULL))
    return COMM_OK;

  CheckPointerAddr(aSend);
  CheckPointerAddr(aRcvd);

  DBG_AUX("SendRcvdFtpCmd-Tx", aSend->sbContent, strlen(aSend->sbContent), FALSE);
  
  Delay10ms(30);

  FlushAllFtp();

  // Send Msg
  if (aSend != NULL) {
    SendFtp(aSend->sbContent);
  }

  if (aRcvd == NULL)
    return COMM_OK;

  //host_timeout = gCommParam.bTimeoutVal;
  host_timeout = WAIT_DATA_TIME;
  host_timeout *= TIMER_1SEC;
  TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);
  ret = COMM_TIMEOUT;


  do {
    // Check off base.
    //if (!Return2Base(TRUE)) {
    //  ret = COMM_LINEDROP;
    //  return ret;
    //}

    // Check Receive Msg
    aRcvd->wLen = RcvdFtp(aRcvd->sbContent);
    //refresh_rate = 5;

    if ((aRcvd->wLen > 0) && (aRcvd->wLen != (WORD)-1)) {
      DBG_AUX("SendRcvdFtpCmd-Rx", aRcvd->sbContent, aRcvd->wLen, FALSE);
      ret = COMM_OK;
      break;
    }
    else if (aRcvd->wLen == (WORD)-1) {
      ret = COMM_LINEDROP;
      aRcvd->wLen = 0;
      break;
    }
    if (IsLineDropFtp()) {
      ret = COMM_LINEDROP;
      aRcvd->wLen = (WORD)-1;
      break;
    }

    if (aRcvd->wLen == (WORD)-1) {
      aRcvd->wLen = 0;
      break;
    }

    SleepMW();
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  return ret;
}


//*****************************************************************************
//  Function        : WaitConnectFtpDataChn
//  Description     : Wait the ftp data channel to be connected
//  Input           : N/A
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD WaitConnectFtpDataChn(void)
{
  WORD ret;
//  BYTE  refresh_rate = 5;
  DWORD host_timeout;

  DBG_AUX("WaitConnectFtpDataChn", NULL, 0, FALSE);
  
//09-07-14++ SC (1)
//  host_timeout = gCommParam.bTimeoutVal;
  host_timeout = WAIT_DATA_TIME;
//09-07-14-- SC (1)
  host_timeout *= TIMER_1SEC;
  TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);
  ret = COMM_TIMEOUT;

  do {
    // Check off base.
    //if (!Return2Base(TRUE)) {
    //  ret = COMM_LINEDROP;
    //  return ret;
    //}

    // Check Receive Msg
//    refresh_rate = 5;

    if ((StatMW(gFtpHandle, MW_FTP_STATUS, NULL)&0x7F) == MW_FTP_STAT_DATA_CONNED) {
      ret = COMM_OK;
      break;
    }

    SleepMW();
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  return ret;
}

//*****************************************************************************
//  Function        : ReadFtpData
//  Description     : Wait the ftp data channel to be connected
//  Input           : aRcvd
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ReadFtpData(struct COMMBUF *aRcvd)
{
  WORD ret;
  //BYTE  refresh_rate = 5;
  DWORD host_timeout;

  DBG_AUX("ReadFtpData", NULL, 0, FALSE);
  
  //host_timeout = gCommParam.bTimeoutVal;
  host_timeout = WAIT_DATA_TIME;
  host_timeout *= TIMER_1SEC;
  TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);
  ret = COMM_TIMEOUT;

  do {
    // Check off base.
    //if (!Return2Base(TRUE)) {
    //  ret = COMM_LINEDROP;
    //  return ret;
    //}

    // Check Receive Msg
    //refresh_rate = 5;

    if ((StatMW(gFtpHandle, MW_FTP_STATUS, NULL)&0x7F) != MW_FTP_STAT_DATA_CONNED) {
      DBG_AUX("ReadFtpData-Dropped!", NULL, 0, FALSE);
      ret = COMM_LINEDROP;
      aRcvd->wLen = (WORD)-1;
      break;
    }

    aRcvd->wLen = ReadMW(gFtpHandle, aRcvd->sbContent, COMMBUF_SIZE);
    if (aRcvd->wLen > 0) {
      DBG_AUX("ReadFtpData-Done!", NULL, aRcvd->wLen, FALSE);
      ret = COMM_OK;
      break;
    }

    SleepMW();
  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);

  return ret;
}

//*****************************************************************************
//  Function        : SendFtpData
//  Description     : Wait the ftp data channel to be connected
//  Input           : aRcvd
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD SendFtpData(struct COMMBUF *aSend)
{
  WORD pos;
  WORD len_written;

  // Check off base.
  //if (!Return2Base(TRUE)) {
  //  return COMM_LINEDROP;
  //}

  DBG_AUX("SendFtpData", NULL, aSend->wLen, FALSE);

  pos = 0;
  TimerSetMW(gTimerHdl[TIMER_COMM], 10*TIMER_1SEC);
  do {
    if ((StatMW(gFtpHandle, MW_FTP_STATUS, NULL)&0x7F) != MW_FTP_STAT_DATA_CONNED) {
      DBG_AUX("SendFtpData-Dropped!", NULL, 0, FALSE);
      return COMM_LINEDROP;
    }

    len_written = WriteMW(gFtpHandle, &aSend->sbContent[pos], aSend->wLen);
    aSend->wLen -= len_written;
    pos += len_written;

    if (aSend->wLen == 0) {
      DBG_AUX("SendFtpData-Done!", NULL, 0, FALSE);
      return COMM_OK;
    }

    if (len_written != 0) { //sth is sent, reset the timer.
      TimerSetMW(gTimerHdl[TIMER_COMM], 10*TIMER_1SEC);
    }

  } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);
  DBG_AUX("SendFtpData-Timeout!", NULL, 0, FALSE);
  return COMM_TIMEOUT;
}

//*****************************************************************************
//  Function        : GetFtpTxDataBufSize
//  Description     : Return data buffer size in FTP TX data channel
//  Input           : aRcvd
//  Return          : status;
//  Note            : // 07-09-15++ FL (2)
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetFtpTxDataBufSize()
{
  return StatMW(gFtpHandle, MW_FTP_DATA_TXBUF_LEFT, NULL);
}

//*****************************************************************************
//  Function        : DownloadAuth
//  Description     : Auth for app download.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int DownloadAuth(void)
{
  BYTE challenge[8];
  DWORD crypto;

  os_get_challenge(challenge);
  os_3deskey(KAuthKey);
  os_3des(challenge);
  crypto = conv_bl(challenge);

  if (os_auth(K_AuthAppDll, crypto) == FALSE)
    return -1;

  return 0;
}

//*****************************************************************************
//  Function        : LoginFtp
//  Description     : Login STIS FTP.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN LoginFtp(char *aUser, char *aPass)
{
  int response = -1;
  BOOLEAN txBufOwner = FALSE, rxBufOwner = FALSE;

  if (psTxBuf == NULL)
  {
    psTxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
	txBufOwner = TRUE;
  }
  if (psRxBuf == NULL)
  {
    psRxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
	rxBufOwner = TRUE;
  }
  MemFatalErr(psTxBuf);
  MemFatalErr(psRxBuf);

  //"LOGIN <user> <pass>
  SprintfMW(psTxBuf->sbContent, "LOGIN %s %s", aUser, aPass);
  if (SendRcvdFtpCmd(psTxBuf, psRxBuf) == COMM_OK)
    response = dec2bin(psRxBuf->sbContent, 3);

#if 0
  FreeMW(psTxBuf);
  FreeMW(psRxBuf);
  psTxBuf = NULL;
  psRxBuf = NULL;

  if ((response >= 400) || (response == -1))
    return FALSE;
#else
  if ((response >= 400) || (response == -1)) {
    if (txBufOwner)
    {
      FreeMW(psTxBuf);
      psTxBuf = NULL;
    }
    if (rxBufOwner)
    {
      FreeMW(psRxBuf);
      psRxBuf = NULL;
    }
    return FALSE;
  }
  
  //Send PBSZ command to enable SSL in Data channel
  strcpy(psTxBuf->sbContent, "PBSZ 0");
  if (SendRcvdFtpCmd(psTxBuf, psRxBuf) == COMM_OK)
    response = dec2bin(psRxBuf->sbContent, 3);
  if ((response >= 400) || (response == -1)) {
    if (txBufOwner)
    {
      FreeMW(psTxBuf);
      psTxBuf = NULL;
    }
    if (rxBufOwner)
    {
      FreeMW(psRxBuf);
      psRxBuf = NULL;
    }
    return FALSE;
  }
  
  //Send PROT command to enable SSL in Data channel
  strcpy(psTxBuf->sbContent, "PROT P");
  if (SendRcvdFtpCmd(psTxBuf, psRxBuf) == COMM_OK)
    response = dec2bin(psRxBuf->sbContent, 3);
  if ((response >= 400) || (response == -1)) {
    if (txBufOwner)
    {
      FreeMW(psTxBuf);
      psTxBuf = NULL;
    }
    if (rxBufOwner)
    {
      FreeMW(psRxBuf);
      psRxBuf = NULL;
    }
    return FALSE;
  }
#endif

  if (txBufOwner)
  {
    FreeMW(psTxBuf);
    psTxBuf = NULL;
  }
  if (rxBufOwner)
  {
    FreeMW(psRxBuf);
    psRxBuf = NULL;
  }
  return TRUE;
}

//*****************************************************************************
//  Function        : ConnectFtp
//  Description     : Send Connection command to Ftp Port
//  Input           : aWaitReady;   // Wait for line ready.
//                    aNoCancel;    // TRUE => No Cancel Allow
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD ConnectFtp(BOOLEAN aWaitReady, BOOLEAN aNoCancel, BOOLEAN aCheckIn, BOOLEAN aSkipPPP)
{
  BOOLEAN ret=TRUE;
  BOOLEAN cancel=FALSE;
  DWORD host_timeout;
  BYTE filename[64];
  struct COMMPARAM  comm_param;
  struct MW_NIF_INFO netinfo;
  BYTE nif;

  DBG_AUX("ConnectFtp", NULL, 0, FALSE);

  if (aWaitReady) {
    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg(KCommConnectWait, MW_LINE5, MW_CENTER|MW_BIGFONT);
  }

  while (1) {
    if ((gCommParam.sPPP.dDevice & 0x0F) != 0) {
      //09-07-14++ SC (1)
      if (aSkipPPP) {
        if (IOCtlMW(gDevHandle[APM_SDEV_PPP], IO_PPP_GET_PHASE, NULL) != MW_PPP_PHASE_READY)
          return COMM_NO_LINE;
      } else
        //09-07-14-- SC (1)
      {
        if (ConnectPPP(TRUE) != COMM_OK) {
          return COMM_NO_LINE;
        }
      }
    }
    //27-06-14++ SC (4)
    else {
      //05-09-14 JC ++
      nif = getTCP_NIF(&gCommParam.sTcp.sTcpCfg);
      // check LAN cable inserted
      if ((nif == MW_NIF_ETHERNET) && !LanCableInsertedMW()) {
        // reset handle whenever LAN cable disconnected
        if (gFtpHandle >= 0) {
          CloseMW(gFtpHandle);
          gFtpHandle = -1;
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
      else if ((nif == MW_NIF_WIFI) && (NetInfoMW(MW_NIF_WIFI, &netinfo) && (netinfo.d_ip == 0))) {
        // reset handle whenever Wifi disconnected
        if (gFtpHandle >= 0) {
          CloseMW(gFtpHandle);
          gFtpHandle = -1;
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
      //05-09-14 JC --
    }
    //27-06-14-- SC (4)

    // new make connection
    if (ret) {
      if (gFtpHandle < 0) {
        // open
        strcpy(filename, DEV_FTP1);
        gFtpHandle = OpenMW(filename, MW_RDWR);
        ret = (gFtpHandle >= 0)? TRUE : FALSE;
  
        // connect
        if (ret) {
          memcpy(&comm_param, &gCommParam, sizeof(struct COMMPARAM));
          //SetupFtpTls(&comm_param.sTcp.sTcpCfg);
          //DebugTcpCfg(&comm_param.sTcp.sTcpCfg, 0);
          if (IOCtlMW(gFtpHandle, IO_FTP_CONNECT2, &comm_param.sTcp.sTcpCfg) < 0) {
            ret = FALSE;
          }
        }
      }
      else {
        if ((StatMW(gFtpHandle, MW_FTP_STATUS, NULL)&0x7F) != MW_FTP_STAT_CONNECTED) {
          memcpy(&comm_param, &gCommParam, sizeof(struct COMMPARAM));
          //SetupFtpTls(&comm_param.sTcp.sTcpCfg);
          //DebugTcpCfg(&comm_param.sTcp.sTcpCfg, 1);
          if (IOCtlMW(gFtpHandle, IO_FTP_CONNECT2, &comm_param.sTcp.sTcpCfg) < 0 ) {
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

        DispLineMW(KFTPConnectingMsg, MW_LINE3, MW_SPFONT); //18-04-14++ SC (1)

        if ((StatMW(gFtpHandle, MW_FTP_STATUS, NULL)&0x7F) == MW_FTP_STAT_CONNECTED) {
          ret = TRUE;
          break;
        }

        if (GetCharMW()==MWKEY_CANCL) {
          if (!aNoCancel) {
            ret = FALSE;
            cancel = TRUE;
            break;
          }
          else {
            Disp2x16Msg(KCommNoCancel, MW_LINE5, MW_CENTER|MW_BIGFONT);
            Delay1Sec(1, TRUE);
            Disp2x16Msg(KCommConnectWait, MW_LINE5, MW_CENTER|MW_BIGFONT);
          }
          LongBeep();
        }
      } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);
    }

    // return
    if (ret) {
      return COMM_OK;
    }
    else {
      ResetFtp();
      if (cancel) {
        return COMM_CANCEL;
      }
      else {
        return COMM_NOT_CONNECT;
      }
    }

    //should reach here if and only if the connection is failed and fallback is allowed (only happens for TCPIP mode)
    PackFTPInitComm(aCheckIn, TRUE);
  }
  return COMM_NOT_CONNECT;
}

//*****************************************************************************
//  Function        : GetFileFtp
//  Description     : Download parameters from STIS FTP.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD GetFileFtp(char *aRemoteFolder, char *aRemoteFile, char *aLocalFile, BOOLEAN bUseSystem, DDWORD aExpectSize)
{
  //#define FTP_DL_BUFFER_SIZE   1024*1024
  #define FTP_DL_BUFFER_SIZE   1024*10
  char filename[64];
  int iFileFd=-1;
  WORD ret;
  BYTE retry_count=3; //09-07-14++ SC (1)

  BYTE buffer[FTP_DL_BUFFER_SIZE];
  int buffer_pos;
  DDWORD file_length;

  char display[22];
  int response = -1;
  struct TERM_DATA term_data;
  char ftp_user[32];
  char ftp_password[32];
  struct DATETIME dtg;
  BOOLEAN conn_ok= FALSE;
  DWORD max_line_size = (GetLcdTypeMW()==MW_DISP_COLOR)?MW_MAXCOLOR_LINESIZE:MW_MAXMONO_LINESIZE;

  BOOLEAN txBufOwner = FALSE, rxBufOwner = FALSE;
  if (psTxBuf == NULL)
  {
    psTxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
	txBufOwner = TRUE;
  }
  if (psRxBuf == NULL)
  {
    psRxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
	rxBufOwner = TRUE;
  }
  MemFatalErr(psTxBuf);
  MemFatalErr(psRxBuf);

  psRxBuf->wLen = 0;

  DBG_AUX("GetFileFtp", aRemoteFile, strlen(aRemoteFile), FALSE);

  ret = FTP_DL_UNEXPECTED;
  while (1) {
    ret = FTP_DL_UNEXPECTED;
    //09-07-14++ SC (1)
    retry_count--;
    // 07-09-15++ FL (1)
    // Sometimes not return line drop but FTP data channel still not connected, try reset FTP only (Keep PPP).
    //if (IsLineDropFtp()) {
    if (IsLineDropFtp() || (retry_count < 2)) {  // retry always re-connect FTP!
      gCommParam.sPPP.bKeepAlive = TRUE;
      ResetFtp();
      gCommParam.sPPP.bKeepAlive = FALSE;
    // 07-09-15-- FL (1)
      conn_ok = FALSE;
      if ((gsTransData.s_rsp_data.w_rspcode=ConnectFtp(TRUE, TRUE, FALSE, TRUE)) == COMM_OK) {
        if (GetTermData(&term_data)) {
          if (term_data.sb_ftp_user[0] != 0x00) {
            strcpy(ftp_user, term_data.sb_ftp_user);
            strcpy(ftp_password, term_data.sb_ftp_pwd);
          } else {
            ReadRTC(&dtg);
            strcpy(ftp_user, "stisYYYYMM");
            strcpy(ftp_password, "YYYYMMstis");
    
            split(&ftp_user[4], (BYTE *)&dtg, 3);
            split(&ftp_password[0], (BYTE *)&dtg, 3);
          }
        }

        if (LoginFtp(ftp_user, ftp_password))
          conn_ok = TRUE;
      }

      if ((!conn_ok) && (retry_count != 0)){
        continue;
      // 07-09-15++ FL (1)
      // (conn_ok == true) && (retry_count == 0) still treated as failure
      //} else {
      } else if (!conn_ok) {
      // 07-09-15-- FL (1)
        break;
      }
    }
    //09-07-14-- SC (1)

    SprintfMW(psTxBuf->sbContent, "GET I %s%s", aRemoteFolder, aRemoteFile);
#if 0
    DispClrBelowMW(MW_LINE3);
    DispGotoMW(MW_LINE3, MW_SPFONT);
    DispPutNCMW(psTxBuf->sbContent, strlen(psTxBuf->sbContent));
    WaitKey(300);
#endif

    if (SendRcvdFtpCmd(psTxBuf, psRxBuf) != COMM_OK) {
      //09-07-14++ SC (1)
      if (retry_count != 0)
        continue;
      else
        //09-07-14-- SC (1)
        break;
    }

    response = dec2bin(psRxBuf->sbContent, 3);

    //09-07-14++ SC (1)
    //if (response == 550)
    //  ret = FTP_DL_NOT_EXIST;
    //
    //if ((response >= 300) || (response == -1))
    //  break;
    //
    // if (WaitConnectFtpDataChn() != COMM_OK)
    //   break;
    //02-06-15 JC ++
    if (response >= 500) {
      ret = FTP_DL_NOT_EXIST;                           //29-09-16 JC ++
      DispClrBelowMW(MW_LINE5);
      SprintfMW(buffer, "\"%s\"", aRemoteFile);
      DispLineMW(buffer, MW_LINE5, MW_CLREOL|MW_SPFONT);
      DispLineMW("Not Exist!", MW_LINE6, MW_CLREOL|MW_SPFONT);
      Delay10ms(100);
      DispClrBelowMW(MW_LINE5);
      break;
    }
    //02-06-15 JC --
    if (WaitConnectFtpDataChn() != COMM_OK) {
      if (retry_count != 0)
        continue;
      else
        break;
    }
    //09-07-14++ SC (1)

    strcpy(filename, aLocalFile);
    if (bUseSystem) {
      os_file_delete(filename);
      iFileFd = os_file_create(filename, K_FILE_USR_READ|K_FILE_USR_WRITE|K_FILE_ACTIVE);
    } else {
      fDeleteMW(filename);
      iFileFd = fCreateMW(filename, 0);
    }

    buffer_pos = 0;

    DispLineMW("Download...", MW_LINE5, MW_CLREOL|MW_SPFONT);
    DispLineMW(aRemoteFile, MW_LINE6, MW_CLREOL|MW_SPFONT);
    if (strlen(aRemoteFile) > max_line_size) {
      DispLineMW(&aRemoteFile[max_line_size], MW_LINE7, MW_CLREOL|MW_SPFONT);
    }
    else
      DispClrLineMW(MW_LINE7);
    while (1) {
      if (aExpectSize != 0) {
        if (bUseSystem)
          SprintfMW(display, "%7d/%7dbytes", os_file_length(iFileFd)+buffer_pos, (DWORD)aExpectSize);
        else
          SprintfMW(display, "%7d/%7dbytes", fLengthMW(iFileFd)+buffer_pos, (DWORD)aExpectSize);
        DispLineMW(display, MW_LINE8, MW_CLREOL|MW_SPFONT|MW_CENTER);
      }

      if (ReadFtpData(psRxBuf) != COMM_OK) {
        if (bUseSystem) {
          if (buffer_pos) {
            os_file_write(iFileFd, buffer, buffer_pos);
          }
        } else {
          if (buffer_pos) {
            fWriteMW(iFileFd, buffer, buffer_pos);
          }
        }
        break;
      }

      if ((buffer_pos+psRxBuf->wLen) >= FTP_DL_BUFFER_SIZE) {
        if (bUseSystem) {
          os_file_write(iFileFd, buffer, buffer_pos);
          os_file_write(iFileFd, psRxBuf->sbContent, psRxBuf->wLen);
          while (!os_file_flushed(iFileFd));
        } else {
          fWriteMW(iFileFd, buffer, buffer_pos);
          fWriteMW(iFileFd, psRxBuf->sbContent, psRxBuf->wLen);
          fCommitAllMW();
        }
        buffer_pos = 0;
      } else {
        memcpy(&buffer[buffer_pos], psRxBuf->sbContent, psRxBuf->wLen);
        buffer_pos += psRxBuf->wLen;
      }
    }

    if (bUseSystem) {
      while (!os_file_flushed(iFileFd));
      file_length = os_file_length(iFileFd);
      os_file_close(iFileFd);
    } else {
      fCommitAllMW();
      file_length = fLengthMW(iFileFd);
      fCloseMW(iFileFd);
    }

    if ((aExpectSize != 0) && (aExpectSize != file_length)) {
      ret = FTP_DL_DL_FAIL;
      if (bUseSystem)
        os_file_delete(aLocalFile);
      else
        fDeleteMW(aLocalFile);
    } else
      ret = FTP_DL_SUCCESS;
    break;
  }
#if 0
  DispClrBelowMW(MW_LINE8);
  SprintfMW(filename, "GetF: %s", aLocalFile);
  DispLineMW(filename, MW_LINE8, MW_CLREOL|MW_SPFONT);
  SprintfMW(filename, "Size: %d, Ret=%d", file_length, ret);
  DispLineMW(filename, MW_LINE9, MW_CLREOL|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
#endif

  if (txBufOwner)
  {
    FreeMW(psTxBuf);
    psTxBuf = NULL;
  }
  if (rxBufOwner)
  {
    FreeMW(psRxBuf);
    psRxBuf = NULL;
  }
  return ret;
}

//*****************************************************************************
//  Function        : CalFileChksum
//  Description     : Download parameters from STIS FTP.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD CalFileChksum(char *aLocalFile, BOOLEAN bUseSystem)
{
  char filename[64];
  int iFileFd=-1;
  int len;
  WORD chksum = 0x0000;
  BYTE buffer[2];

  strcpy(filename, aLocalFile);
  if (bUseSystem)
    iFileFd = os_file_open(filename,K_O_RDONLY);
  else
    iFileFd = fOpenMW(filename);

  if (iFileFd == -1)
    return 0xFFFF;

  while (1) {
    if (fSeekMW(iFileFd, MW_FSEEK_CUR) == fLengthMW(iFileFd))
      break;

    if (bUseSystem)
      len = os_file_read(iFileFd, buffer, 2);
    else
      len = fReadMW(iFileFd, buffer, 2);

    if (len == 1)
      buffer[1] = 0x00;

    chksum += (WORD) (buffer[0] << 8) | buffer[1];
  }

  if (bUseSystem)
    os_file_close(iFileFd);
  else
    fCloseMW(iFileFd);

#if 0
  DispClrBelowMW(MW_LINE6);
  SprintfMW(filename, "ChkSum(%s):", aLocalFile);
  DispLineMW(filename, MW_LINE6, MW_CLREOL|MW_SPFONT);
  SprintfMW(filename, "=> %04X", chksum);
  DispLineMW(filename, MW_LINE8, MW_CLREOL|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
#endif
  return chksum;
}

//*****************************************************************************
//  Function        : PutFileFtp
//  Description     : Download parameters from STIS FTP.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PutFileFtp(char *aRemoteFolder, char *aRemoteFile, char *aLocalFile, BOOLEAN bUseSystem)
{
#define FTP_PUT_BLOCK_SIZE 500
  struct TERM_DATA term_data;
  struct DATETIME dtg;
  char filename[64];
  int iFileFd=-1;
  int len;
  BOOLEAN ret;
  BYTE *pmem;
  //int response = -1;
  BYTE retry_count=3; //09-07-14++ SC (1)
  DWORD fullsize;     // 07-09-15++ FL (2)

  char display[22];
  DWORD file_length;
  char ftp_user[32];
  char ftp_password[32];
  DWORD max_line_size = (GetLcdTypeMW()==MW_DISP_COLOR)?MW_MAXCOLOR_LINESIZE:MW_MAXMONO_LINESIZE;

  BOOLEAN txBufOwner = FALSE, rxBufOwner = FALSE;
  if (psTxBuf == NULL)
  {
    psTxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
	txBufOwner = TRUE;
  }
  if (psRxBuf == NULL)
  {
    psRxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
	rxBufOwner = TRUE;
  }
  MemFatalErr(psTxBuf);
  MemFatalErr(psRxBuf);

  DBG_AUX("PutFileFtp", aRemoteFile, strlen(aRemoteFile), FALSE);

  //"LOGIN <user> <pass>
  psRxBuf->wLen = 0;

  ret = FALSE;
  pmem = MallocMW(FTP_PUT_BLOCK_SIZE);

  while (1) {
    retry_count--; //09-07-14++ SC (1)
    SprintfMW(psTxBuf->sbContent, "PUT I %s%s", aRemoteFolder, aRemoteFile);
#if 0
    DispClrBelowMW(MW_LINE3);
    DispGotoMW(MW_LINE3, MW_SPFONT);
    DispPutNCMW(psTxBuf->sbContent, strlen(psTxBuf->sbContent));
    WaitKey(300);
#endif
    if (SendRcvdFtpCmd(psTxBuf, psRxBuf) != COMM_OK)
      break;

    //response = dec2bin(psRxBuf->sbContent, 3);

    //09-07-14++ SC (1)
    //if ((response >= 300) || (response == -1))
    //  break;
    //
    // if (WaitConnectFtpDataChn() != COMM_OK)
    //   break;
    if (WaitConnectFtpDataChn() != COMM_OK) {
      if (retry_count != 0) {
        // 07-09-15++ FL (1)
        // Add reset FTP and re-connect FTP only.
        gCommParam.sPPP.bKeepAlive = TRUE;
        ResetFtp();
        gCommParam.sPPP.bKeepAlive = FALSE;
        if (ConnectFtp(TRUE, TRUE, FALSE, TRUE) == COMM_OK) {
          if (GetTermData(&term_data)) {
            if (term_data.sb_ftp_user[0] != 0x00) {
              strcpy(ftp_user, term_data.sb_ftp_user);
              strcpy(ftp_password, term_data.sb_ftp_pwd);
            } else {
              ReadRTC(&dtg);
              strcpy(ftp_user, "stisYYYYMM");
              strcpy(ftp_password, "YYYYMMstis");
              split(&ftp_user[4], (BYTE *)&dtg, 3);
              split(&ftp_password[0], (BYTE *)&dtg, 3);
            }
          }
          LoginFtp(ftp_user, ftp_password);
        }
        // 07-09-15-- FL (1)
        continue;
      }
      else
        break;
    }
    //09-07-14++ SC (1)

    strcpy(filename, aLocalFile);
    if (bUseSystem)
      iFileFd = os_file_open(filename,K_O_RDONLY);
    else
      iFileFd = fOpenMW(filename);

    DispLineMW("Upload...", MW_LINE5, MW_CLREOL|MW_SPFONT);
    DispLineMW(aRemoteFile, MW_LINE6, MW_CLREOL|MW_SPFONT);
    if (strlen(aRemoteFile) > max_line_size) {
      DispLineMW(&aRemoteFile[max_line_size], MW_LINE7, MW_CLREOL|MW_SPFONT);
    }
    else
      DispClrLineMW(MW_LINE7);
    file_length = fLengthMW(iFileFd);
    // 07-09-15++ FL (2)
    fullsize = GetFtpTxDataBufSize();
    // 07-09-15-- FL (2)
    while (1) {
      SprintfMW(display, "%7d/%7dbyte", fSeekMW(iFileFd, MW_FSEEK_CUR), file_length);
      DispLineMW(display, MW_LINE8, MW_CLREOL|MW_SPFONT|MW_CENTER);

      if (bUseSystem)
        len = os_file_read(iFileFd, pmem,FTP_PUT_BLOCK_SIZE);
      else
        len = fReadMW(iFileFd, pmem, FTP_PUT_BLOCK_SIZE);
      if (len > 0) {
        psTxBuf->wLen = len;
        memcpy(psTxBuf->sbContent, pmem, len);
        if (SendFtpData(psTxBuf) != COMM_OK)
          break;
      } else {
        // 07-09-15++ FL (2)
        //if (len == 0) //no more bytes to read
        //  ret = TRUE;
        //break;
        if (len == 0) { //no more bytes to read
          ret = TRUE;
          TimerSetMW(gTimerHdl[TIMER_COMM], 20*TIMER_1SEC);  // 20s
          do {
            if (GetFtpTxDataBufSize() >= fullsize) {  // check and make sure all data sent
              DispLineMW("DONE!", MW_LINE5+16, MW_SPFONT);
              break;
            }
            SleepMW();
          } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);
          if (TimerGetMW(gTimerHdl[TIMER_COMM]) == 0) {
            DispLineMW("JAM!", MW_LINE5+17, MW_SPFONT);
            Delay1Sec(3, TRUE);
          }
          break;
        }
        // 07-09-15-- FL (2)
      }
    }

    if (bUseSystem)
      os_file_close(iFileFd);
    else
      fCloseMW(iFileFd);
    DisconnectFtpData();
    break;
  }

  FreeMW(pmem);
  if (txBufOwner)
  {
    FreeMW(psTxBuf);
    psTxBuf = NULL;
  }
  if(rxBufOwner)
  {
	FreeMW(psRxBuf);
    psRxBuf = NULL;
  }
  return ret;
}

//*****************************************************************************
//  Function        : MatchDMK
//  Description     : MatchDMK.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MatchDMK(char *aLocalFile)
{
#if 0
  int file_len, file_pos;
  int iFileFd=-1;
  BYTE line[25];
  int len;
  BOOLEAN ret = FALSE;
  int byte_read;
  struct TERM_DATA term_data;

  if (!GetTermData(&term_data))
    return FALSE;

  iFileFd = fOpenMW(aLocalFile);

  if (iFileFd == -1)
    return FALSE;

  file_len = fLengthMW(iFileFd);
  file_pos = fSeekMW(iFileFd, MW_FSEEK_SET);


  while (file_pos < file_len) {
    memset(line, 0x00, sizeof(line));
    byte_read = fReadMW(iFileFd, line, sizeof(line));

    if (byte_read == 0)
      break;

    len = 0;
    while (len < sizeof(line)) {
      if (byte_read <= len)
        break;
      if ((line[len]==0x0a)||(line[len]==0x0d)||(line[len]==0x00))
        break;
      len++;
    }

    if (strcmp(line,term_data.sb_server_key) == 0) {
      ret = TRUE;
      break;
    }

    file_pos = fSeekMW(iFileFd, file_pos+len+1);
  }

  fCloseMW(iFileFd);
  fDeleteMW(aLocalFile); //remove the file after use.

  return ret;
#else

  return TRUE;
#endif
}

//*****************************************************************************
//  Function        : GetTID
//  Description     : GetTID.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetTID(char *aLocalFile)
{
  int file_len;
  int iFileFd=-1;
  int byte_read;
  struct TERM_DATA term_data;
  BOOLEAN ret = FALSE;

  if (!GetTermData(&term_data))
    return FALSE;

  iFileFd = fOpenMW(aLocalFile);

  if (iFileFd == -1)
    return FALSE;

  file_len = fLengthMW(iFileFd);
  fSeekMW(iFileFd, MW_FSEEK_SET);

  if (file_len >= sizeof(term_data.sb_term_id)-1) { //-1 for check digit
    byte_read = fReadMW(iFileFd, term_data.sb_term_id, sizeof(term_data.sb_term_id)-1);
    if (byte_read == sizeof(term_data.sb_term_id)-1) {
      term_data.sb_term_id[sizeof(term_data.sb_term_id)-1] = chk_digit(term_data.sb_term_id, sizeof(term_data.sb_term_id)-1);
      UpdTermData(&term_data);
      ret = TRUE;
    }
  }

  fCloseMW(iFileFd);
  fDeleteMW(aLocalFile); //remove the file after use.

  return ret;
}

//*****************************************************************************
//  Function        : DisplayFTPProcess
//  Description     : Display FTP Process
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DisplayFTPProcess(BYTE *aLine)
{
  DispLineMW(aLine, MW_LINE3, MW_CLREOL|MW_SPFONT);
  DispClrBelowMW(MW_LINE4);
}

//*****************************************************************************
//  Function        : CheckXmlParam
//  Description     : CheckXmlParam.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckXmlParam(char *aLocalFile)
{
  int iFileFd=-1;
  int file_len, len;
  BYTE *ptr;
  BOOLEAN ret;

  iFileFd = fOpenMW(aLocalFile);
  file_len = fLengthMW(iFileFd);
  if ((ptr = MallocMW(file_len+1)) == NULL) {
    fCloseMW(iFileFd);
    return FALSE;
  }
  fSeekMW(iFileFd, MW_FSEEK_SET);
  len = fReadMW(iFileFd, ptr, file_len);
  if (len != file_len) {
    fCloseMW(iFileFd);
    FreeMW(ptr);
    return FALSE;
  }
  fCloseMW(iFileFd);
  ptr[len] = 0;
  ret = XmlStringToCfg(ptr);
  FreeMW(ptr);
  return ret;
}

//*****************************************************************************
//  Function        : CheckEDCFile
//  Description     : CheckEDCFile.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckEDCFile(char *aLocalFile)
{
  int iFileFd=-1;
  int file_len;
  int iSaveAcqCount=0;

  BYTE sub_type;
  WORD sub_field_len;
  void *p_mem;
  BYTE i;
  DWORD logo_index;
  int field_end;
  BYTE buffer[5];
  static BYTE card_idx;  //20-08-15++ SF(1)

  iFileFd = fOpenMW(aLocalFile);
  file_len = fLengthMW(iFileFd);

  while (fSeekMW(iFileFd, MW_FSEEK_CUR) < file_len) {
    fReadMW(iFileFd, buffer, 2);
    field_end = fSeekMW(iFileFd, MW_FSEEK_CUR) + (WORD)bcd2bin(buffer[1] + buffer[0] * 256);
    while (fSeekMW(iFileFd, MW_FSEEK_CUR) < field_end) {
      fReadMW(iFileFd, &sub_type, 1);
      fReadMW(iFileFd, buffer, 2);

      sub_field_len = (WORD)bcd2bin(buffer[1] + buffer[0] * 256);

      switch (sub_type) {
        case 0:
          if (psBatchNo != NULL) {
            FreeMW(psBatchNo);
            psBatchNo = NULL;
          }
          iSaveAcqCount = GetAcqCount();
          if (iSaveAcqCount > 0) {
            psBatchNo = (struct BATCH_NO *) MallocMW(sizeof(struct BATCH_NO)*iSaveAcqCount);
            for (i = 0; i < iSaveAcqCount; i++) {
              GetBatchNo(i, psBatchNo[i].sb_curr);
            }
            FreeMW(psBatchNo);
          }
          fCommitAllMW();
          CloseSTISFiles();
          CreateSTISFiles();
          SetSTISMode(INIT_MODE);
          fCommitAllMW();
          card_idx = 0;  // 20-08-15++ SF (1)
          break;
        case 1:
          if ((p_mem = (void *)MallocMW(sizeof(struct TERM_CFG))) == NULL) {
            return FALSE;
          }
          fReadMW(iFileFd, p_mem, sub_field_len);
          ((struct TERM_CFG*)p_mem)->b_trans_amount_len = (BYTE)bcd2bin(((struct TERM_CFG*)p_mem)->b_trans_amount_len);
          ((struct TERM_CFG*)p_mem)->b_settle_amount_len = (BYTE)bcd2bin(((struct TERM_CFG*)p_mem)->b_settle_amount_len);
          UpdTermCfg((struct TERM_CFG*)p_mem);
          FreeMW(p_mem);
          break;
        case 2:
          if ((p_mem = (void *)MallocMW(sizeof(struct CARD_TBL))) == NULL) {
            return FALSE;
          }
          // convert idx into HEX for storage
          fReadMW(iFileFd, &i, 1);
          fReadMW(iFileFd, p_mem, sub_field_len-1);
          ((struct CARD_TBL*)p_mem)->b_acquirer_id = (BYTE)bcd2bin(((struct CARD_TBL*)p_mem)->b_acquirer_id);
          ((struct CARD_TBL*)p_mem)->b_issuer_id = (BYTE)bcd2bin(((struct CARD_TBL*)p_mem)->b_issuer_id);
          //20-08-15++ SF(1)
          //UpdCardTbl(i-1, (struct CARD_TBL*)p_mem);
          UpdCardTbl(card_idx++, (struct CARD_TBL*)p_mem);
          //20-08-15-- SF(1)
          FreeMW(p_mem);
          break;
        case 3:
          if ((p_mem = (void *)MallocMW(sizeof(struct ISSUER_TBL))) == NULL) {
            return FALSE;
          }
          fReadMW(iFileFd, &i, 1);
          fReadMW(iFileFd, p_mem, sub_field_len-1);
          // convert idx into HEX for storage
          ((struct ISSUER_TBL*)p_mem)->b_id = (BYTE)bcd2bin(((struct ISSUER_TBL*)p_mem)->b_id);
          UpdIssuerTbl(i-1, (struct ISSUER_TBL*)p_mem);
          FreeMW(p_mem);
          break;
        case 4:
          if ((p_mem = (void *)MallocMW(sizeof(struct ACQUIRER_TBL))) == NULL) {
            return FALSE;
          }
          fReadMW(iFileFd, &i, 1);
          fReadMW(iFileFd, p_mem, sub_field_len-1);
          // convert idx into HEX for storage
          ((struct ACQUIRER_TBL*)p_mem)->b_id = (BYTE)bcd2bin(((struct ACQUIRER_TBL*)p_mem)->b_id);
          if ((skpb(((struct ACQUIRER_TBL*)p_mem)->sb_curr_batch_no, 0, 3) == 3) && (i < iSaveAcqCount))
            memcpy(((struct ACQUIRER_TBL*)p_mem)->sb_curr_batch_no, (BYTE *)&psBatchNo[i].sb_curr[0], 3);
          if ((skpb(((struct ACQUIRER_TBL*)p_mem)->sb_next_batch_no, 0, 3) == 3) && (i < iSaveAcqCount))
            memcpy(((struct ACQUIRER_TBL*)p_mem)->sb_next_batch_no, (BYTE *)&psBatchNo[i].sb_next[0], 3);
          ((struct ACQUIRER_TBL*)p_mem)->b_status=UP;
          ((struct ACQUIRER_TBL*)p_mem)->b_pending=NO_PENDING;
          UpdAcqTbl(i-1, (struct ACQUIRER_TBL*)p_mem);
          FreeMW(p_mem);
          break;
        case 5:
          if ((p_mem = (void *)MallocMW(sizeof(struct DESC_TBL))) == NULL) {
            return FALSE;
          }
          fReadMW(iFileFd, &i, 1);
          fReadMW(iFileFd, p_mem, sub_field_len-1);
          UpdDescTbl(i-1, (struct DESC_TBL*)p_mem);
          FreeMW(p_mem);
          break;
        case 6:
          if (sub_field_len != 143) {
            if ((p_mem = (void *)MallocMW(sizeof(struct LOGO_TBL))) == NULL) {
              return FALSE;
            }
            fReadMW(iFileFd, &logo_index, 1);
            ((struct LOGO_TBL*)p_mem)->b_flag = 0x01;
            fReadMW(iFileFd, &i, 1);
            ((struct LOGO_TBL*)p_mem)->b_type = i;
            fReadMW(iFileFd, &((struct LOGO_TBL*)p_mem)->sb_image, sub_field_len-2);
            UpdLogoTbl(logo_index, (struct LOGO_TBL*)p_mem);
            FreeMW(p_mem);
          }
          break;
      }
    }
  }
  fCloseMW(iFileFd);
  fCommitAllMW();

  return TRUE;
}

//*****************************************************************************
//  Function        : CheckEMVAppFile
//  Description     : CheckEMVAppFile.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckEMVAppFile(char *aLocalFile)
{
  int iFileFd=-1;
  int file_len;
  BYTE buffer[5];
  BYTE sub_type;
  WORD sub_field_len;
  APP_CFG app_cfg;

  int field_end;

  SetEMVLoaded(0x80);

  iFileFd = fOpenMW(aLocalFile);
  file_len = fLengthMW(iFileFd);

  while (fSeekMW(iFileFd, MW_FSEEK_CUR) < file_len) {
    fReadMW(iFileFd, buffer, 2);
    field_end = fSeekMW(iFileFd, MW_FSEEK_CUR) + (WORD)bcd2bin(buffer[1] + buffer[0] * 256);
    while (fSeekMW(iFileFd, MW_FSEEK_CUR) < field_end) {
      fReadMW(iFileFd, &sub_type, 1);
      fReadMW(iFileFd, buffer, 2);

      sub_field_len = (WORD)bcd2bin(buffer[1] + buffer[0] * 256);

      switch (sub_type) {
        case 8: // AID TABLE
          if (sub_field_len == 223) {
            fReadMW(iFileFd, &app_cfg.eType, 1);
            fReadMW(iFileFd, &app_cfg.eBitField, 1);
            fReadMW(iFileFd, buffer, 4);
            app_cfg.eRSBThresh = DWGet(buffer);
            fReadMW(iFileFd, &app_cfg.eRSTarget, 1);
            fReadMW(iFileFd, &app_cfg.eRSBMax, 1);
            fReadMW(iFileFd, &app_cfg.eTACDenial, 5);
            fReadMW(iFileFd, &app_cfg.eTACOnline, 5);
            fReadMW(iFileFd, &app_cfg.eTACDefault, 5);
            fReadMW(iFileFd, &app_cfg.eACFG, 200);
            EMVData2DnloadBuf(IOCMD_LOAD_AID, (BYTE *)&app_cfg, sizeof(APP_CFG));
            SetEMVLoaded(0x01); // lower nibble for status
          }
          else {
            return FALSE;
          }
          break;
      }
    }
  }
  fCloseMW(iFileFd);
  fCommitAllMW();
  return TRUE;
}

//*****************************************************************************
//  Function        : CheckEMVKeyFile
//  Description     : CheckEMVKeyFile.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckEMVKeyFile(char *aLocalFile)
{
  int iFileFd=-1;
  int file_len;
  BYTE buffer[5];

  BYTE sub_type;
  WORD sub_field_len;
  KEY_ROOM key_room;

  int field_end;

  SetEMVLoaded(0x40);

  iFileFd = fOpenMW(aLocalFile);
  file_len = fLengthMW(iFileFd);

  while (fSeekMW(iFileFd, MW_FSEEK_CUR) < file_len) {
    fReadMW(iFileFd, buffer, 2);
    field_end = fSeekMW(iFileFd, MW_FSEEK_CUR) + (WORD)bcd2bin(buffer[1] + buffer[0] * 256);
    while (fSeekMW(iFileFd, MW_FSEEK_CUR) < field_end) {
      fReadMW(iFileFd, &sub_type, 1);
      fReadMW(iFileFd, buffer, 2);

      sub_field_len = (WORD)bcd2bin(buffer[1] + buffer[0] * 256);

      switch (sub_type) {
        case 9:// KEY TABLE
          if (sub_field_len == 275) {
            fReadMW(iFileFd, &key_room.KeyIdx, 1);
            fReadMW(iFileFd, &key_room.RID, 5);
            fReadMW(iFileFd, buffer, 4);
            key_room.CAKey.Exponent = DWGet(buffer);
            fReadMW(iFileFd, buffer, 2);
            key_room.CAKey.KeySize = WGet(buffer);
            fReadMW(iFileFd, &key_room.CAKey.Key, 256);
            fReadMW(iFileFd, buffer, 1); // skip CC
            fReadMW(iFileFd, buffer, 2);
            key_room.ExpiryDate = buffer[1]*256+buffer[0];
            fReadMW(iFileFd, buffer, 1); // skip CC
            fReadMW(iFileFd, buffer, 2);
            key_room.EffectDate = buffer[1]*256+buffer[0];
            fReadMW(iFileFd, &key_room.Chksum, 1);
            EMVData2DnloadBuf(IOCMD_KEY_LOADING, (BYTE *)&key_room, sizeof(KEY_ROOM));
          }
          else {
            return FALSE;
          }
          break;
      }
    }
  }
  fCloseMW(iFileFd);
  fCommitAllMW();
  return TRUE;
}

//*****************************************************************************
//  Function        : CheckTMSExt
//  Description     : Check if filename ended with .TMS
//  Input           : aFile;      // filename
//  Return          : sucess/fail
//  Note            : //18-05-17 JC ++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckTMSExt(char *aFile)
{
  DWORD len=strlen(aFile);
  
  if (len < 4)
    return FALSE;
    
  if (memcmp(&aFile[len-4], ".TMS", 4) == 0)
    return TRUE;

  return FALSE;
}

//*****************************************************************************
//  Function        : CheckDLLFileLine
//  Description     : CheckDLLFileLine.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckDLLFileLine(char *aLine, struct DL_FILES_INFO *aInfo, BYTE aLineIdx)
{
#define MAX_SEP   5

  BYTE *ptr_checksum, *ptr_date;
  BYTE i,j,k;
  BYTE *field_sep[MAX_SEP];
#ifdef AppInfoGet1MW
  struct MW_APPL_INFO1 app_info;
#else
  struct MW_APPL_INFO app_info;
#endif
  BYTE tmp[4];
  BYTE temp_app_name[14];

  for (i=0;  i<MAX_SEP; i++)
    field_sep[i] = NULL;

  i = 0;
  j = 0;
  k = 0;
  while (aLine[i] != 0) {
    if (aLine[i] != ' ') {
      aLine[j] = aLine[i];
      if (aLine[j] == ',')
        field_sep[k++] = &aLine[j];
      j++;
    } else if (k == 0) {
      aLine[j] = aLine[i];
      j++;
    }
    i++;
  }
  aLine[j] = 0;

  //size
  //download required
  memcpy(aInfo->remote_file, aLine, field_sep[0]-(BYTE*)aLine);
  aInfo->remote_file[field_sep[0]-(BYTE*)aLine] = 0x00;

  aInfo->size = decbin8b(field_sep[0]+1, field_sep[1]-(field_sep[0]+1));
  aInfo->download_required = (*(field_sep[1]+1) == '1');

  //default *only for application download
  if (field_sep[2] != NULL)
    aInfo->default_app = (*(field_sep[2]+1) == '1');

  //date and checksum for parameter
  if (aLineIdx < NUM_PARAM_FILES) {
    ptr_checksum = ptr_date = NULL;
    i=0;
    while (aInfo->remote_file[i] != 0x00) {
      if (aInfo->remote_file[i] == '_') {
        ptr_date = ptr_checksum;
        ptr_checksum = &aInfo->remote_file[i+1];
      }
      i++;
    }
    compress(aInfo->date, ptr_date, 4);
    compress(aInfo->checksum, ptr_checksum, 2);
  } else if (aLineIdx >= NUM_PARAM_FILES) {
    ptr_checksum = ptr_date = NULL;
    i=0;
    while (aInfo->remote_file[i] != 0x00) {
      if (aInfo->remote_file[i] == '_') {
        ptr_date = ptr_checksum;
        ptr_checksum = &aInfo->remote_file[i+1];
      }
      i++;
    }
    //18-05-17 JC ++
    if (ptr_checksum != NULL) {
#ifdef AppInfoGet1MW
      compress(aInfo->checksum, ptr_checksum, 4);
#else
      compress(aInfo->checksum, ptr_checksum, 2);
#endif
    }
    //18-05-17 JC --
  }

  aInfo->updated = TRUE;

  switch (aLineIdx) {
    case IDX_EDC:
      strcpy(aInfo->local_file, KParamEDCFile);
      if (CompareEDCDateChksum(aInfo->checksum,aInfo->date))
        aInfo->updated = FALSE;
      break;
    case IDX_EMV_APP:
      strcpy(aInfo->local_file, KParamEMVAPPFile);
      if (CompareEMVAPPDateChksum(aInfo->checksum,aInfo->date))
        aInfo->updated = FALSE;
      break;
    case IDX_EMV_KEY:
      strcpy(aInfo->local_file, KParamEMVKEYFile);
      if (CompareEMVKEYDateChksum(aInfo->checksum,aInfo->date))
        aInfo->updated = FALSE;
      break;
    default: //applications
      //18-05-17 JC ++
      if (CheckTMSExt(aInfo->remote_file) == TRUE) {
        // normal TMS file
        i=0;
        while (aInfo->remote_file[i++] != '_'); //kick out the model number, 32byte is not enough
        strcpy(aInfo->local_file, &aInfo->remote_file[i]);
        j=0;
        memset(aInfo->app_name, 0x00, sizeof(aInfo->app_name));
        while ((aInfo->remote_file[i] != '_') || (aInfo->remote_file[i+1] != '0'))
          aInfo->app_name[j++] = aInfo->remote_file[i++];
  
        for (i=0; i<0xFF; i++) {
#ifdef AppInfoGet1MW
          if (AppInfoGet1MW(i, &app_info)) {
            strcpy(temp_app_name, (i==0)?app_info.sb_app_name:&app_info.sb_app_name[2]);  // cater 1st 2 bytes are ID except system.
            if (strcmp(aInfo->app_name, temp_app_name) == 0) {
              tmp[0] = (BYTE)(app_info.d_cs32 >> 24);
              tmp[1] = (BYTE)(app_info.d_cs32 >> 16);
              tmp[2] = (BYTE)(app_info.d_cs32 >> 8);
              tmp[3] = (BYTE)(app_info.d_cs32 & 0xFF);
              if (memcmp(tmp, aInfo->checksum, 4) == 0) {
                aInfo->updated = FALSE;
              }
              break;
            }
          }
#else
          if (AppInfoGetMW(i, &app_info)) {
            strcpy(temp_app_name, (i==0)?app_info.sb_app_name:&app_info.sb_app_name[2]);  // cater 1st 2 bytes are ID except system.
            if (strcmp(aInfo->app_name, temp_app_name) == 0) {
              tmp[0] = (BYTE)(app_info.w_checksum >> 8);
              tmp[1] = (BYTE)(app_info.w_checksum & 0xFF);
              if (memcmp(tmp, aInfo->checksum, 2) == 0) {
                aInfo->updated = FALSE;
              }
              break;
            }
          }
#endif
        }
      }
      else {
        // binary file
        strcpy(aInfo->local_file, aInfo->remote_file);
        strcpy(aInfo->app_name, aInfo->remote_file);
      }
      //18-05-17 JC ++
      break;
  }
#if 0
  BYTE buf[64];
  DispClrBelowMW(MW_LINE3);
  SprintfMW(buf, "Line: %d", aLineIdx);
  DispLineMW(buf, MW_LINE3, MW_CLREOL|MW_SPFONT);
  SprintfMW(buf, "Date: %02x%02x%02x%02x", aInfo->date[0],aInfo->date[1],aInfo->date[2],aInfo->date[3]);
  DispLineMW(buf, MW_LINE4, MW_CLREOL|MW_SPFONT);
  SprintfMW(buf, "CS  : %02x%02x%02x%02x", aInfo->checksum[0],aInfo->checksum[1],aInfo->checksum[2],aInfo->checksum[3]);
  DispLineMW(buf, MW_LINE5, MW_CLREOL|MW_SPFONT);
  SprintfMW(buf, "Upd : %d", aInfo->updated);
  DispLineMW(buf, MW_LINE6, MW_CLREOL|MW_SPFONT);
  WaitKey(9000);
#endif

  return TRUE;
}

//*****************************************************************************
//  Function        : CheckDLLFile
//  Description     : CheckDLLFile.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckDLLFile(char *aLocalFile, int aDownloadMode)
{
  int iFileFd=-1;
  int file_len;
  BYTE buffer[5];
  BYTE line_idx;
  BYTE line_pos;
  BYTE line[256];
  BYTE last_char;

  iFileFd = fOpenMW(aLocalFile);
  file_len = fLengthMW(iFileFd);

  memset(&DL_file_list, 0, sizeof(DL_file_list));
  for (line_idx=0; line_idx<NUM_PARAM_FILES+MAX_APP_DL; line_idx++) {
    DL_file_list[line_idx].exist = FALSE;
    DL_file_list[line_idx].result = FTP_DL_NOT_REQ;
  }

  line_idx = 0;
  last_char = 0x00;
  line_pos = 0;
  memset(line, 0x00, sizeof(line));

  while (fSeekMW(iFileFd, MW_FSEEK_CUR) < file_len) {
    last_char = buffer[0];
    fReadMW(iFileFd, buffer, 1);

    if ((buffer[0] == 0x0A) && (last_char == 0x0D)) {
      buffer[0] = 0xFF;
      continue;
    }

    if ((buffer[0] == 0x0D) && (last_char == 0x0A)) {
      buffer[0] = 0xFF;
      continue;
    }

    if ((buffer[0] == 0x0A) || (buffer[0] == 0x0D)) {
      if (line_pos != 0) {
        DL_file_list[line_idx].exist = TRUE;
        DL_file_list[line_idx].result = FTP_DL_UNEXPECTED;
        CheckDLLFileLine(line, &DL_file_list[line_idx], line_idx);
      }
      line_idx++;
      line_pos = 0;
      memset(line, 0x00, sizeof(line));
    } else {
      line[line_pos++] = buffer[0];
    }
  }

  if (line_pos != 0)
    CheckDLLFileLine(line, &DL_file_list[line_idx], line_idx);

  //suppress unwanted items
  if (aDownloadMode != MODE_ALL_PARAM_APP) {

    if (aDownloadMode != MODE_APPLICATION) {
      for (line_idx=NUM_PARAM_FILES; line_idx<NUM_PARAM_FILES+MAX_APP_DL; line_idx++) {
        DL_file_list[line_idx].download_required = FALSE;
      }
    }

    if (aDownloadMode != MODE_ALL_PARAM) {
      DL_file_list[IDX_EDC].download_required = FALSE;

      if ((aDownloadMode != MODE_EMV_PARAM_ONLY) && (aDownloadMode != MODE_EMV_PARAM_KEY)) {
        DL_file_list[IDX_EMV_KEY].download_required = FALSE;
      }

      if ((aDownloadMode != MODE_EMV_KEY_ONLY) && (aDownloadMode != MODE_EMV_PARAM_KEY)) {
        DL_file_list[IDX_EMV_APP].download_required = FALSE;
      }
    }
  }

  fCloseMW(iFileFd);

  return TRUE;
}

//*****************************************************************************
//  Function        : InitFTPLogInfo
//  Description     : InitFTPLogInfo.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void InitFTPLogInfo(BYTE* aTID, BYTE* aMode)
{
  struct DATETIME dtg;
  memcpy(FTP_dl_log.tid, aTID, 8);
  ReadRTC(&dtg);
  split(FTP_dl_log.datetime, (BYTE *)&dtg, 7);
  memcpy(FTP_dl_log.version, FTP_DL_VERSION, 3);
  memcpy(FTP_dl_log.mode, aMode, 2);
  FTP_dl_log.err = FTP_DL_NO_ERROR;
  FTP_dl_log.err_desc[0] = 0x00;

  strcpy(FTP_dl_log.remote_file_name, "CCYYMMDD_hhmmss_TTTTTTTT.LOG");
  memcpy(&FTP_dl_log.remote_file_name[0], &FTP_dl_log.datetime[0], 8);
  memcpy(&FTP_dl_log.remote_file_name[9], &FTP_dl_log.datetime[8], 6);
  memcpy(&FTP_dl_log.remote_file_name[16], FTP_dl_log.tid, 8);
}

//*****************************************************************************
//  Function        : UpdateFTPLogInfoError
//  Description     : UpdateFTPLogInfoError.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void UpdateFTPLogInfoError(BYTE aError, BYTE* aDesciprtion)
{
  FTP_dl_log.err = aError;

  if (aDesciprtion != NULL)
    strcpy(FTP_dl_log.err_desc, aDesciprtion);
  else
    FTP_dl_log.err_desc[0] = 0x00;
}

//*****************************************************************************
//  Function        : WriteFTPDLLog
//  Description     : WriteFTPDLLog
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void WriteFTPDLLog(void)
{
  BYTE tmp[128];
  int iFileFd=-1;

  strcpy(tmp, KFtpDownloadLog);
  fDeleteMW(tmp);

  iFileFd = fCreateMW(tmp, 0);
  fSeekMW(iFileFd, 0);

  //TID
  strcpy(tmp, "TID=TTTTTTTT\x0d\x0a");
  memcpy(&tmp[4], FTP_dl_log.tid, 8);
  fWriteMW(iFileFd, tmp, strlen(tmp));

  //Date Time
  strcpy(tmp, "DT=CCYYMMDDhhmmss\x0d\x0a");
  memcpy(&tmp[3], FTP_dl_log.datetime, 14);
  fWriteMW(iFileFd, tmp, strlen(tmp));

  //Version
  strcpy(tmp, "VER=VVV\x0d\x0a");
  memcpy(&tmp[4], FTP_dl_log.version, 3);
  fWriteMW(iFileFd, tmp, strlen(tmp));

  //Mode
  strcpy(tmp, "MODE=MM\x0d\x0a");
  memcpy(&tmp[5], FTP_dl_log.mode, 2);
  fWriteMW(iFileFd, tmp, strlen(tmp));

  //Error
  strcpy(tmp, "ERR=MM");
  memcpy(&tmp[4], FtpDownloadErrorCode[FTP_dl_log.err], 2);

  if (FTP_dl_log.err_desc[0] != 0x00) {
    strcat(tmp, ",");
    strcat(tmp, FTP_dl_log.err_desc);
  }
  strcat(tmp, "\x0d\x0a");
  fWriteMW(iFileFd, tmp, strlen(tmp));

  fCommitAllMW();
  fCloseMW(iFileFd);
}

//*****************************************************************************
//  Function        : CreateFTPMirrorList
//  Description     : CreateFTPMirrorList.
//  Input           : N/A
//  Return          : sucess/fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CreateFTPMirrorList(void)
{
  BYTE tmp[32];
  BYTE tmp_chksum[3];//, tmp_date[4];
  //DDWORD tmp_size = 0;
  int iFileFd=-1;
  struct TERM_DATA term_data;
  BYTE var_i;
  struct MW_APPL_INFO app_info;

  strcpy(tmp, KFtpMirrorList);
  fDeleteMW(tmp);
  iFileFd = fCreateMW(tmp, 0);

  if (GetTermData(&term_data)) {
    //EDC
    if (memcmp(term_data.sb_edc_chksum,"\xFF\xFF",2) != 0x00) {
      //term_data.sb_term_id + '_' + param date + '_' + chksum
      //           01234567890123456789012345
      strcpy(tmp, "TTTTTTTT_CCYYMMDD_SSSS.edc, ");
      memcpy(&tmp[0], term_data.sb_term_id, 8);
      split(&tmp[9], term_data.sb_edc_date, 4);
      split(&tmp[18], term_data.sb_edc_chksum, 2);
      fWriteMW(iFileFd, tmp, strlen(tmp));

      dbin2asc(&tmp[1], term_data.sb_edc_size);
      tmp[0] = (BYTE) skpb(&tmp[1], '0', 19);
      fWriteMW(iFileFd, &tmp[1+tmp[0]], 20-tmp[0]);

    }
    tmp[0] = 0x0D;
    tmp[1] = 0x0A;
    fWriteMW(iFileFd, tmp, 2);

    //EMV application
    if (memcmp(term_data.sb_emvapp_chksum,"\xFF\xFF",2) != 0x00) {
      //term_data.sb_term_id + '_' + param date + '_' + chksum
      //           01234567890123456789012345
      strcpy(tmp, "TTTTTTTT_CCYYMMDD_SSSS.emvapp, ");
      memcpy(&tmp[0], term_data.sb_term_id, 8);
      split(&tmp[9], term_data.sb_emvapp_date, 4);
      split(&tmp[18], term_data.sb_emvapp_chksum, 2);
      fWriteMW(iFileFd, tmp, strlen(tmp));

      dbin2asc(&tmp[1], term_data.sb_emvapp_size);
      tmp[0] = (BYTE) skpb(&tmp[1], '0', 19);
      fWriteMW(iFileFd, &tmp[1+tmp[0]], 20-tmp[0]);

    }
    tmp[0] = 0x0D;
    tmp[1] = 0x0A;
    fWriteMW(iFileFd, tmp, 2);

    //EMV key
    if (memcmp(term_data.sb_emvkey_chksum,"\xFF\xFF",2) != 0x00) {
      //term_data.sb_term_id + '_' + param date + '_' + chksum
      //           01234567890123456789012345
      strcpy(tmp, "TTTTTTTT_CCYYMMDD_SSSS.emvkey, ");
      memcpy(&tmp[0], term_data.sb_term_id, 8);
      split(&tmp[9], term_data.sb_emvkey_date, 4);
      split(&tmp[18], term_data.sb_emvkey_chksum, 2);
      fWriteMW(iFileFd, tmp, strlen(tmp));

      dbin2asc(&tmp[1], term_data.sb_emvkey_size);
      tmp[0] = (BYTE) skpb(&tmp[1], '0', 19);
      fWriteMW(iFileFd, &tmp[1+tmp[0]], 20-tmp[0]);

    }
    tmp[0] = 0x0D;
    tmp[1] = 0x0A;
    fWriteMW(iFileFd, tmp, 2);

  }

  //Application
  for (var_i=0; var_i<0xFF; var_i++) {
    if (AppInfoGetMW(var_i, &app_info)) {
      strcpy(tmp, TERM_MODEL"_");

      fWriteMW(iFileFd, tmp, strlen(tmp));

      strcpy(tmp, (var_i==0)?app_info.sb_app_name:&app_info.sb_app_name[2]);  // cater 1st 2 bytes are ID except system.
      fWriteMW(iFileFd, tmp, strlen(tmp));
      strcpy(tmp, "_");
      fWriteMW(iFileFd, tmp, strlen(tmp));

      //           0123456789012345
      strcpy(tmp, "VVV_SS_CCCC_DDDD.tms, 0 \x0D\x0A");
      bin2dec(app_info.b_version,&tmp[0],3);
      bin2dec(app_info.b_sub_version,&tmp[4],2);

      tmp_chksum[0] = app_info.w_checksum >> 8;
      tmp_chksum[1] = app_info.w_checksum & 0xFF;
      split(&tmp[7], tmp_chksum, 2);

      tmp_chksum[0] = app_info.w_disp_checksum >> 8;
      tmp_chksum[1] = app_info.w_disp_checksum & 0xFF;
      split(&tmp[12], tmp_chksum, 2);
      fWriteMW(iFileFd, tmp, strlen(tmp));
    }
  }

  fCommitAllMW();
  fCloseMW(iFileFd);
}

//*****************************************************************************
//  Function        : STISFTPDisplayResult
//  Description     : Download parameters from STIS FTP.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void STISFTPDisplayResult(const BYTE* aMsg, BYTE aSuccess)
{
  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(aMsg, MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (aSuccess)
    AcceptBeep();
  else
    LongBeep();
  Delay1Sec(10, FALSE);
}

//*****************************************************************************
//  Function        : STISFTPAppDelAllow
//  Description     : Check if the application with the index can be deleted.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN STISFTPAppDelAllow(BYTE aIdx)
{
  switch (aIdx) {
    case 0x00: //  SysID
    case 0x01: //  ProdCode
    case 0x02: //  DriverID
    case 0x03: //  BaseID
    //case 0x04: //  B5/GB Font
    case 0x05: //  SP Font
    case 0x06: //  SamDrv
    case 0x07: //  CliccDrv
    case 0x08: //  BtLoader
    case 0x09: //  MfgID
    //case 0x0A: //  InternalID
    case 0x0E: //  EmvDll
    case 0x0F: //  MidID
    case 0x10: //  ApmID
    case 0x11: //  EDC
    case 0x64: //  EmvclDll
    case 0x66: //  PPassDll
    case 0x67: //  PWaveDll
    case 0x7D: //  Ecrdll
    case 0x7E: //  Pp300dll
    case 0x7F: //  Keydll
      return FALSE;
  }

  return TRUE;
}

//*****************************************************************************
//  Function        : STISFTPDownloadTrans
//  Description     : Download parameters from STIS FTP.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD STISFTPDownloadTrans(BOOLEAN aManual, BYTE aDownloadMode, BOOLEAN aBurnInRoomMode)
{
  int i, func_sel,j ;
  struct TERM_DATA term_data;
  DWORD response = TRANS_FAIL;

  char remote_folder[128];
  char remote_file[64];
  char local_file[64];
  char ftp_user[32];
  char ftp_password[32];
  char serial_number[9];
  //BYTE buf[32];

  BOOLEAN reboot_required = FALSE;
  char temp_app_name[14];
  BOOLEAN app_found;
  struct MW_APPL_INFO app_info;
  WORD ftp_get_result;
  BYTE log_retry=0;
  int acq_count;
  struct ACQUIRER_TBL acq_tbl;
  struct DATETIME dtg;

  BOOLEAN mirror_done = FALSE;
  BOOLEAN log_done = FALSE;
  BOOLEAN server_key_ok = FALSE; //19-06-14++ SC (1)
  BOOLEAN dll_file_done = FALSE; //24-06-14++ SC (2)
  BOOLEAN dl_item_not_enabled = FALSE; //18-08-14++ SC (1)

  if (!aBurnInRoomMode) //25-06-14++ SC (1), burn in room will use default setting to load application/paramter
    if (GetSTISMode() == SETUP_MODE)
      return response;


  if (aManual) {
    if ((func_sel = MenuSelect(&KInitMenu, 0)) == -1)
      return response;
  } else {
    func_sel = aDownloadMode;
  }

  gsTransData.s_input.b_trans = EDC_INIT;
  gsTransData.s_input.b_stis_init_mode = func_sel; //23-06-14++ SC (1)


  DispCtrlMW(MW_CLR_DISP);
  DispHeader();
//23-06-14++ SC (1)
//  gsTransData.s_input.b_stis_init_mode = func_sel;
//23-06-14-- SC (1)

  if (!GetTermData(&term_data))
    return response;

  if (!aBurnInRoomMode)  //25-06-14++ SC (1)
    memcpy(gsTransData.s_input.sb_term_id,term_data.sb_term_id, 8);

  GetSysCfgMW(K_CF_SerialNo, serial_number);
  serial_number [8] = 0x00;

  PackFTPInitComm(FALSE, FALSE);

  if ((gsTransData.s_rsp_data.w_rspcode=ConnectFtp(TRUE, FALSE, FALSE, FALSE)) != COMM_OK) {
    //19-06-14++ SC (1)
    ResetFtp();
    if (gsTransData.s_rsp_data.w_rspcode == COMM_CANCEL)
      STISFTPDisplayResult(KTxnCanceled, FALSE);
    else
      STISFTPDisplayResult(KTryAgainNC, FALSE);
    //19-06-14-- SC (1)
    return response;
  }

  //GenDLL(); //26-06-14++ SC (3)

  while (1) {
    //"LOGIN <user> <pass>
    DisplayFTPProcess("Log In");

    if (term_data.sb_ftp_user[0] != 0x00) {
      strcpy(ftp_user, term_data.sb_ftp_user);
      strcpy(ftp_password, term_data.sb_ftp_pwd);
    } else {
      ReadRTC(&dtg);
      strcpy(ftp_user, "stisYYYYMM");
      strcpy(ftp_password, "YYYYMMstis");

      split(&ftp_user[4], (BYTE *)&dtg, 3);
      split(&ftp_password[0], (BYTE *)&dtg, 3);
    }

    if (!LoginFtp(ftp_user, ftp_password)) {
      //19-06-14++ SC (1)
      // break;
      ResetFtp();
      STISFTPDisplayResult(KLogInFailed, FALSE);
      return response;
      //19-06-14-- SC (1)
    }

    //25-06-14++ SC (1)
    if (aBurnInRoomMode) {
      DisplayFTPProcess("Get TID");
      strcpy(remote_folder, SVR_PATH"SN/SSSSSSSS/");
      memcpy(&remote_folder[strlen(SVR_PATH)+3], serial_number, 8);
      strcpy(remote_file, "SSSSSSSS.tid");
      memcpy(remote_file, serial_number, 8);
      strcpy(local_file, "SSSSSSSS.tid");
      memcpy(local_file, serial_number, 8);
      ftp_get_result = GetFileFtp(remote_folder,remote_file,local_file, FALSE, 0);
      while (1) {
        if (ftp_get_result == FTP_DL_SUCCESS) {
          if (GetTID(local_file)) {
            SetSTISMode(INIT_MODE);
            fCommitAllMW();
            if (GetTermData(&term_data)) //must do it as the terminal ID have been changed
              break;
          }
        }
        STISFTPDisplayResult(KTIDFileFailed, FALSE);
        ResetFtp();
        return response;
      }
    }
    //25-06-14-- SC (1)

    if (func_sel == MODE_UPLOAD)
      InitFTPLogInfo(term_data.sb_term_id, "00");
    else
      InitFTPLogInfo(term_data.sb_term_id, "11");

#if 0
    DisplayFTPProcess("Get Server Key");
    strcpy(remote_folder, SVR_PATH);
    strcpy(remote_file, "server.key");
    strcpy(local_file, "server.key");
    ftp_get_result = GetFileFtp(remote_folder,remote_file,local_file, FALSE, 0);
    if (ftp_get_result == FTP_DL_SUCCESS) {
      if (!MatchDMK(local_file)) {
        UpdateFTPLogInfoError(FTP_DOWNLOAD_AUTH_ERROR, NULL);
        //19-06-14++ SC (1)
        //break;
        //19-06-14-- SC (1)
      }
      //19-06-14++ SC (1)
      else {
        server_key_ok = TRUE;
      }
    } else if (ftp_get_result == FTP_DL_NOT_EXIST) {
      UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
      //19-06-14++ SC (1)
      //break;
      //19-06-14-- SC (1)
    } else {
      UpdateFTPLogInfoError(FTP_DOWNLOAD_UNEXPECTED_ERROR, "Server Key");
      //19-06-14++ SC (1)
      //break;
      //19-06-14-- SC (1)
    }

    if (!server_key_ok) {
      break;
    }
#else
    server_key_ok = TRUE;
#endif

    // If the upload list selected, download nothing from the host.
    if (func_sel == MODE_UPLOAD)
      break;

    // "GET I /POS/00/00/00/00/00000000.dll"
    DisplayFTPProcess("Get Download List");
    //25-06-14++ SC (1)
    if (aBurnInRoomMode) {
      strcpy(remote_folder, SVR_PATH"SN/00000000/");
      memcpy(&remote_folder[strlen(SVR_PATH)+3], &serial_number, 8);
    } else
      //25-06-14-- SC (1)
    {
      strcpy(remote_folder, SVR_PATH"POS/00000000/");
      memcpy(&remote_folder[strlen(SVR_PATH)+4], &term_data.sb_term_id[0], 8);
    }

    strcpy(remote_file, "00000000_SSSSSSSS.DLL");
    memcpy(&remote_file, &term_data.sb_term_id, 8);
    memcpy(&remote_file[9], &serial_number, 8);
    strcpy(local_file, "downloadlist");

    ftp_get_result = GetFileFtp(remote_folder,remote_file,local_file, FALSE, 0);
    if (ftp_get_result == FTP_DL_NOT_EXIST) {
      UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
      break;
    } else if (ftp_get_result != FTP_DL_SUCCESS){
      UpdateFTPLogInfoError(FTP_DOWNLOAD_UNEXPECTED_ERROR, "File");
      break;
    }

    dll_file_done = TRUE; //24-06-14++ SC (2)
    CheckDLLFile(local_file, func_sel);

    //18-08-14++ SC (1)
    dl_item_not_enabled = FALSE;
    if ((!aBurnInRoomMode) && (func_sel != MODE_APPLICATION)) {
      switch (func_sel) {
        case MODE_ALL_PARAM     :
          if ((!DL_file_list[IDX_EDC].download_required && DL_file_list[IDX_EDC].exist)
              ||  (!DL_file_list[IDX_EMV_APP].download_required && DL_file_list[IDX_EMV_APP].exist)
              ||  (!DL_file_list[IDX_EMV_KEY].download_required && DL_file_list[IDX_EMV_KEY].exist)
          )
          {
            dl_item_not_enabled = TRUE;
          }
          break;
        case MODE_EMV_PARAM_ONLY:
//26-08-14++ SC (1)
//          if (!DL_file_list[IDX_EMV_APP].download_required) {
          if (!DL_file_list[IDX_EMV_APP].download_required && DL_file_list[IDX_EMV_APP].exist) {
//26-08-14-- SC (1)
            dl_item_not_enabled = TRUE;
          }
          break;
        case MODE_EMV_KEY_ONLY  :
//26-08-14++ SC (1)
//          if (!DL_file_list[IDX_EMV_KEY].download_required) {
          if (!DL_file_list[IDX_EMV_KEY].download_required && DL_file_list[IDX_EMV_KEY].exist) {
//26-08-14-- SC (1)
            dl_item_not_enabled = TRUE;
          }
          break;
        case MODE_EMV_PARAM_KEY :
//26-08-14++ SC (1)
//          if ((!DL_file_list[IDX_EMV_APP].download_required)
//          ||  (!DL_file_list[IDX_EMV_KEY].download_required)) {
          if ((!DL_file_list[IDX_EMV_APP].download_required && DL_file_list[IDX_EMV_APP].exist)
              ||  (!DL_file_list[IDX_EMV_KEY].download_required && DL_file_list[IDX_EMV_KEY].exist)) {
//26-08-14-- SC (1)
            dl_item_not_enabled = TRUE;
          }
          break;
          //for below mode, do not check
        case MODE_ALL_PARAM_APP :
        case MODE_APPLICATION   :
        case MODE_UPLOAD        :
          break;
      }

      if (dl_item_not_enabled) {
        UpdateFTPLogInfoError(FTP_DOWNLOAD_UNEXPECTED_ERROR, "Item Not Enabled");
        break; // break the infinite loop, we will then get to upload mirror list and log
      }
    }
    //18-08-14-- SC (1)

    EMVClrDnloadBuf();
    CTLClrDnloadBuf();
    for (i=0; i<NUM_PARAM_FILES+MAX_APP_DL; i++) {
      //stepping here means there may be changes in application or parameters, set the flag such that
      //if the mirroring below fail, the teminal will perform daily polling to upload the mirror list.
      SetAppListUploadReq(APPL_UPLOAD_PENDING);
      while (1){
        if (!DL_file_list[i].exist) {
          if (i<NUM_PARAM_FILES) {
            switch (i) {
              case IDX_EDC:
                CloseSTISFiles();
                CreateSTISFiles();
                SetSTISMode(INIT_MODE);
                SetEDCDateChksumSize("\xFF\xFF", "\x00\x00\x00\x00", 0);
                fCommitAllMW();
                break;
              case IDX_EMV_APP:
                SetEMVAPPDateChksumSize("\xFF\xFF", "\x00\x00\x00\x00", 0);
                break;
              case IDX_EMV_KEY:
                SetEMVKEYDateChksumSize("\xFF\xFF", "\x00\x00\x00\x00", 0);
                break;
            }
          }
          break;
        }

        if (!DL_file_list[i].download_required) {
          DL_file_list[i].result = FTP_DL_NOT_REQ;
          break;
        }

        //if the parameter download is triggered manually, download all parameter even if
        //it may be the same
        if (!DL_file_list[i].updated) {
          if ((!aManual) || (i>=NUM_PARAM_FILES)) {
            DL_file_list[i].result = FTP_DL_NOT_REQ;
            break;
          }
        }

        switch (i) {
          case IDX_EDC:
            CloseSTISFiles();
            CreateSTISFiles();
            SetSTISMode(INIT_MODE);
            fCommitAllMW();
            break;
        }

        if (i>=NUM_PARAM_FILES)
          DisplayFTPProcess(FtpDownloadHdr[NUM_PARAM_FILES]);
        else
          DisplayFTPProcess(FtpDownloadHdr[i]);

        strcpy(local_file, DL_file_list[i].local_file);
        //        strcpy(remote_folder, DL_file_list[i].remote_path);
        switch (i) {
          case IDX_EDC        :
          case IDX_EMV_APP    :
          case IDX_EMV_KEY    :
            //25-06-14++ SC (1)
            if (aBurnInRoomMode) {
              strcpy(remote_folder, SVR_PATH"SN/00000000/");
              memcpy(&remote_folder[strlen(SVR_PATH)+3], &serial_number, 8);
            } else
              //25-06-14-- SC (1)
            {
              strcpy(remote_folder, SVR_PATH"POS/TTTTTTTT/");
              memcpy(&remote_folder[strlen(SVR_PATH)+4], &term_data.sb_term_id, 8);
            }
            strcpy(remote_file, DL_file_list[i].remote_file);
            break;
          default:
            strcpy(remote_folder, SVR_PATH"APP/");
            strcpy(remote_file, DL_file_list[i].remote_file);
            break;
        }
        ftp_get_result = GetFileFtp(remote_folder, remote_file, local_file, i >= NUM_PARAM_FILES, DL_file_list[i].size);

        if (ftp_get_result == FTP_DL_SUCCESS) {
          switch (i) {
            case IDX_EDC:
              if (CalFileChksum(local_file, FALSE) == ((WORD)(DL_file_list[i].checksum[0]<< 8) | DL_file_list[i].checksum[1])) {
                if (CheckXmlParam(local_file)) {
                  SetEDCDateChksumSize(DL_file_list[i].checksum, DL_file_list[i].date, DL_file_list[i].size);
                  DL_file_list[i].result = FTP_DL_SUCCESS;
                }
              } else {
                DL_file_list[i].result = FTP_DL_CHKSUM_FAIL;
                UpdateFTPLogInfoError(FTP_DOWNLOAD_CHKSUM_ERROR, remote_file);
              }
              break;
            default:
              DL_file_list[i].result = FTP_DL_UNEXPECTED;
              //18-07-17 JC ++
              if (CheckTMSExt(local_file) == FALSE) {
                DL_file_list[i].result = FTP_DL_SUCCESS;
                break;
              }
              //18-07-17 JC --
              if (DownloadAuth() == 0) { //success auth
                j = os_appdl_verify_app(local_file);
                if (j == K_APPDL_RESULT_OKAY) {
                  j = os_appdl_write_app(local_file);
                  DL_file_list[i].result = FTP_DL_SUCCESS;
                  reboot_required = TRUE;
                }
              } else {
                UpdateFTPLogInfoError(FTP_DOWNLOAD_UNEXPECTED_ERROR, "Internal Auth Error");
                DispLineMW("Auth Error-1", MW_LINE7, MW_CLREOL|MW_SPFONT);
                WaitKey(300);
              }
              break;
          }

          if (i < NUM_PARAM_FILES) { //26-06-14++ SC (2)
            //if ((i != IDX_VBIN_VER) || ((i==IDX_VBIN_VER) && (VBIN_file_dl.result == FTP_DL_SUCCESS))){
              //strcpy(remote_folder, SVR_PATH"MIRROR/00000000/");
              //memcpy(&remote_folder[strlen(SVR_PATH)+7], &term_data.sb_term_id[0], 8);
              strcpy(remote_folder, SVR_PATH"MIRROR/");

              PutFileFtp(remote_folder, remote_file, local_file, FALSE);
            //}
          }

          //clean up the download files
          strcpy(local_file, DL_file_list[i].local_file);
          if (i<NUM_PARAM_FILES) {
            fDeleteMW(local_file);
          } else {
            if (CheckTMSExt(local_file) == TRUE)              //18-07-17 JC ++
              os_file_delete(local_file);
          }
        } else if (ftp_get_result == FTP_DL_NOT_EXIST) {
          DL_file_list[i].result = FTP_DL_NOT_EXIST;
          UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
        } else {
          DL_file_list[i].result = FTP_DL_DL_FAIL;
          UpdateFTPLogInfoError(FTP_DOWNLOAD_ERROR, remote_file);
        }
        break;
      }

      if (DL_file_list[i].exist && DL_file_list[i].result != FTP_DL_NOT_REQ) {
        if (DL_file_list[i].result != FTP_DL_SUCCESS) {
          break;
        }
      }
    }

    EMVDataUpdate();
    EMVInit();
    EMVKillDnloadBuf();
    CTLDataUpdate();
    CTLKillDnloadBuf();

    if ((gsTransData.s_input.b_stis_init_mode == MODE_APPLICATION) || (gsTransData.s_input.b_stis_init_mode == MODE_ALL_PARAM_APP)){
      for (i=0; i<0xFF; i++) {
        if (STISFTPAppDelAllow(i)) { //26-06-14++ SC (1)
          app_found = FALSE;
          if (AppInfoGetMW(i, &app_info)) {
            strcpy(temp_app_name, (i==0)?app_info.sb_app_name:&app_info.sb_app_name[2]);  // cater 1st 2 bytes are ID except system.

            for (j=NUM_PARAM_FILES; j<NUM_PARAM_FILES+MAX_APP_DL; j++) {
              if (!DL_file_list[j].exist)
                continue;
              if (strcmp(DL_file_list[j].app_name, temp_app_name) == 0) {
                if (DL_file_list[j].default_app) {
                  term_data.b_default_app = app_info.b_appid;
                  UpdTermData(&term_data);
                }
                app_found = TRUE;
              }
            }

            if (!app_found) {
              if (DownloadAuth() == 0) { //success auth
                DisplayFTPProcess("Delete Application");
                DispLineMW(temp_app_name, MW_LINE4, MW_CLREOL|MW_SPFONT);
                j = os_appdl_delete_app(temp_app_name);
                reboot_required = TRUE;
              } else {
                UpdateFTPLogInfoError(FTP_DOWNLOAD_UNEXPECTED_ERROR, "Internal Auth Error");
                DispLineMW("Auth Error-2", MW_LINE7, MW_CLREOL|MW_SPFONT);
                WaitKey(300);
              }
            }
          }
        }
      }
    }
    //loop end
    break;
  }
  // End

  if (!IsLineDropFtp()) { //27-06-14++ SC (2)
    CreateFTPMirrorList();

    DisplayFTPProcess("Upload Mirror List");
    //strcpy(remote_folder, SVR_PATH"MIRROR/00000000/");
    //memcpy(&remote_folder[strlen(SVR_PATH)+7], &term_data.sb_term_id[0], 8);
    strcpy(remote_folder, SVR_PATH"MIRROR/");
    strcpy(remote_file, "00000000_SSSSSSSS.MFL");
    memcpy(&remote_file, &term_data.sb_term_id, 8);
    memcpy(&remote_file[9], &serial_number, 8);
    strcpy(local_file, KFtpMirrorList);
    if (PutFileFtp(remote_folder, remote_file, local_file, FALSE))
      mirror_done = TRUE;
    //24-06-14++ SC (1)
    else
      UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
    //24-06-14-- SC (1)
  }

  DisplayFTPProcess("Upload Log");
  WriteFTPDLLog();
  strcpy(remote_folder, SVR_PATH"LOG/");
  strcpy(remote_file, FTP_dl_log.remote_file_name);
  strcpy(local_file, KFtpDownloadLog);

  log_retry = 0;
  do {
//27-06-14++ SC (1)
//    if (log_retry != 0) {
    if ((log_retry != 0) || (IsLineDropFtp())) {
//27-06-14-- Sc (1)
      ResetFtp(); //27-06-14++ SC (5)
      if ((gsTransData.s_rsp_data.w_rspcode=ConnectFtp(TRUE, TRUE, FALSE, FALSE)) != COMM_OK)
        continue;

      if (term_data.sb_ftp_user[0] != 0x00) {
        strcpy(ftp_user, term_data.sb_ftp_user);
        strcpy(ftp_password, term_data.sb_ftp_pwd);
      } else {
        ReadRTC(&dtg);
        strcpy(ftp_user, "stisYYYYMM");
        strcpy(ftp_password, "YYYYMMstis");

        split(&ftp_user[4], (BYTE *)&dtg, 3);
        split(&ftp_password[0], (BYTE *)&dtg, 3);
      }

      if (!LoginFtp(ftp_user, ftp_password))
        continue;
    }

    if (PutFileFtp(remote_folder, remote_file, local_file, FALSE)) {
      log_done = TRUE;
      break;
    }
  } while (++log_retry < 3);

  //In the STIS logic, it will first check if the log exist and then find the mirror list.
  //if the log cannot be uploaded to the host, retry upload
  if (mirror_done && log_done) {
    SetAppListUploadReq(APPL_UPLOAD_DONE);
    UpdateAppListChksum();
  }

  if (server_key_ok && (!dl_item_not_enabled) && dll_file_done && (func_sel != MODE_UPLOAD)) {//18-08-14++ SC (2)
    for (i=0; i<NUM_PARAM_FILES; i++) {
      if (!DL_file_list[i].exist)
        continue;

      if ((DL_file_list[i].result == FTP_DL_SUCCESS) ||
          (DL_file_list[i].result == FTP_DL_NOT_REQ))
        continue;

      break;
    }

    if (i == NUM_PARAM_FILES) {
      acq_count = GetAcqCount();
      for (i=0; i < acq_count; i++) {
        if (GetAcqTbl(i, &acq_tbl)) {
          if (acq_tbl.b_status != NOT_LOADED) {
            SetSTISMode(TRANS_MODE);
            SetDebugMode(FALSE);
            GenDLL();
            //17-07-14++ SC (3)
            //          response = TRANS_ACP;
            //17-07-14-- SC (3)
            break;
          }
        }
      }
    } else if ((gsTransData.s_input.b_stis_init_mode == MODE_ALL_PARAM) || (gsTransData.s_input.b_stis_init_mode == MODE_ALL_PARAM_APP)){
      CloseSTISFiles();
      CreateSTISFiles();
      SetSTISMode(INIT_MODE);
      fCommitAllMW();
      response = TRANS_FAIL;
    }
  }

  for (i=0; i<NUM_PARAM_FILES+MAX_APP_DL; i++) {
//19-06-14++ SC (3)
//    if (DL_file_list[i].exist)
//      continue;
    if (!DL_file_list[i].exist)
      continue;
//19-06-14-- SC (3)

    if ((DL_file_list[i].result == FTP_DL_SUCCESS) ||
        (DL_file_list[i].result == FTP_DL_NOT_REQ))
      continue;
    break;
  }
  ResetFtp(); //18-07-14++ SC (3)
  BatSysInit(GetAcqCount());

  if (!server_key_ok)
    STISFTPDisplayResult(KServerKeyError, FALSE);
  else {
    if (!dll_file_done && (func_sel != MODE_UPLOAD)) {
      STISFTPDisplayResult(KDLLFileFailed, FALSE);
    }
    else
      if (dl_item_not_enabled) {
        STISFTPDisplayResult(KItemNotEnabled, FALSE);
      }
      else
        if (i==NUM_PARAM_FILES+MAX_APP_DL) {
          STISFTPDisplayResult(KComplted, TRUE); //19-06-14++ SC (1)
          response = TRANS_ACP;
          SetRemoteDownload(DLOAD_NO);
        } else {
          STISFTPDisplayResult(KDownloadFailed, FALSE); //19-06-14++ SC (1)
        }
  }

  //21-08-14++ SC (1)
  if (aBurnInRoomMode) {
    do {
      i = GetCharMW();
      SleepMW();
    } while (i == 0);
  }
  
  //21-08-14-- SC (1)
//18-07-14++ SC (3)
//  ResetFtp();
//18-07-14-- SC (3)

#ifndef WIN32
  if (reboot_required) {
    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg(KReboot, MW_LINE3, MW_BIGFONT);
    if (WaitKey(KBD_TIMEOUT) == MWKEY_ENTER) {
      fCommitAllMW();
      ResetMW();
    }
  }
#endif
  return response;
}

//*****************************************************************************
//  Function        : STISFTPPolling
//  Description     : Perform daily polling and mirroring
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void STISFTPPolling(struct REMOTEDL_DATA *aRemoteDl)
{
  int i, j;
  struct TERM_DATA term_data;
  BYTE log_retry=0;

  char remote_folder[128];
  char remote_file[64];
  char local_file[64];
  char temp_app_name[14];
  BOOLEAN app_found;
  struct MW_APPL_INFO app_info;
  BOOLEAN polling = FALSE;
  char ftp_user[32];
  char ftp_password[32];
  char serial_number[9];
  BOOLEAN mirror_done = FALSE;
  BOOLEAN log_done = FALSE;
  struct DATETIME dtg;

  if (GetSTISMode() == SETUP_MODE)
    return;

  gsTransData.s_input.b_trans = LOAD_SCHEDULE;
  DispCtrlMW(MW_CLR_DISP);
  DispHeader();

  if (!GetTermData(&term_data))
    return;

  memcpy(gsTransData.s_input.sb_term_id,term_data.sb_term_id, 8);

  GetSysCfgMW(K_CF_SerialNo, serial_number);
  serial_number [8] = 0x00;


  PackFTPInitComm(TRUE, FALSE);

  //16-07-14++ SC (3)
  // Increase the DL_daily here,
  // if fail, the count should be increase by 1
  // if success, the DL_dailywill be changed to DL_DONE at the end.
  aRemoteDl->DL_daily++;
  //16-07-14-- SC (3)

  if ((gsTransData.s_rsp_data.w_rspcode=ConnectFtp(TRUE, TRUE, TRUE, FALSE)) != COMM_OK) {
    //16-07-14++ SC (3)
    //aRemoteDl->DL_daily++;
    //16-07-14-- SC (3)

    //20-06-14++ SC (1)
    STISFTPDisplayResult(KTryAgainNC, FALSE);
    ResetFtp();
    return;
    //20-06-14-- SC (1)
  }

  InitFTPLogInfo(term_data.sb_term_id, "00");

  while (1) {
    //"LOGIN <user> <pass>
    //if (!LoginFtp("PR608I", "Spect_SD"))
    DisplayFTPProcess("Log In");


    if (term_data.sb_ftp_user[0] != 0x00) {
      strcpy(ftp_user, term_data.sb_ftp_user);
      strcpy(ftp_password, term_data.sb_ftp_pwd);
    } else {
      ReadRTC(&dtg);
      strcpy(ftp_user, "stisYYYYMM");
      strcpy(ftp_password, "YYYYMMstis");

      split(&ftp_user[4], (BYTE *)&dtg, 3);
      split(&ftp_password[0], (BYTE *)&dtg, 3);
    }

    if (!LoginFtp(ftp_user, ftp_password))
      //20-06-14++ SC (1)
      //  continue;
      //26-06-14++ SC (4)
      //  return;
    {
      ResetFtp();
      //27-06-14++ SC (2)
      STISFTPDisplayResult(KLogInFailed, FALSE);
      //27-06-14-- Sc (2)
      return;
    }
    //26-06-14-- SC (4)
    //20-06-14-- SC (1)

#if 0
    strcpy(remote_folder, "/");
    strcpy(remote_file, "server.key");
    strcpy(local_file, "server.key");

    DisplayFTPProcess("Get Server Key");
    if (!GetFileFtp(remote_folder,remote_file,local_file, FALSE, 0)) {
      UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
      break;
    }

    if (!MatchDMK(local_file)) {
      UpdateFTPLogInfoError(FTP_DOWNLOAD_AUTH_ERROR, NULL);
      break;
    }
#endif

    // "GET I /POS/00/00/00/00/00000000.dll"
    strcpy(remote_folder, SVR_PATH"POS/00000000/");
    memcpy(&remote_folder[strlen(SVR_PATH)+4], &term_data.sb_term_id[0], 8);

    strcpy(remote_file, "00000000_SSSSSSSS.DLL");
    memcpy(&remote_file, &term_data.sb_term_id, 8);
    memcpy(&remote_file[9], &serial_number, 8);
    strcpy(local_file, "downloadlist");

    DisplayFTPProcess("Get Download List");
    if (!GetFileFtp(remote_folder,remote_file,local_file, FALSE, 0)) {
      UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
      break;
    }

    CheckDLLFile(local_file, MODE_ALL_PARAM_APP);

    for (i=0; i<NUM_PARAM_FILES+MAX_APP_DL; i++) {
      if (!DL_file_list[i].exist)
        continue;

      if (!DL_file_list[i].download_required)
        continue;

      if (!DL_file_list[i].updated)
        continue;

      polling = TRUE;
    }

    aRemoteDl->DL_daily = DL_DONE;
    break;
  }

  if (!IsLineDropFtp()) { //27-06-14++ SC (2)
    CreateFTPMirrorList();
    DisplayFTPProcess("Upload Mirror List");

    //strcpy(remote_folder, SVR_PATH"MIRROR/00000000/");
    //memcpy(&remote_folder[strlen(SVR_PATH)+7], &term_data.sb_term_id[0], 8);
    strcpy(remote_folder, SVR_PATH"MIRROR/");
    strcpy(remote_file, "00000000_SSSSSSSS.MFL");
    memcpy(&remote_file, &term_data.sb_term_id, 8);
    memcpy(&remote_file[9], &serial_number, 8);
    strcpy(local_file, KFtpMirrorList);
    if (PutFileFtp(remote_folder, remote_file, local_file, FALSE))
      mirror_done = TRUE;
    //24-06-14++ SC (1)
    else
      UpdateFTPLogInfoError(FTP_DL_FILE_ERROR, remote_file);
    //24-06-14-- SC (1)
  }

  DisplayFTPProcess("Upload Log");
  WriteFTPDLLog();
  strcpy(remote_folder, SVR_PATH"LOG/");
  strcpy(remote_file, FTP_dl_log.remote_file_name);
  strcpy(local_file, KFtpDownloadLog);

  log_retry = 0;
  do {
//27-06-14++ SC (1)
//    if (log_retry++ != 0) {
    if ((log_retry != 0) || (IsLineDropFtp())) {
//27-06-14-- Sc (1)
      ResetFtp();
      if ((gsTransData.s_rsp_data.w_rspcode=ConnectFtp(TRUE, TRUE, TRUE, FALSE)) != COMM_OK)
        continue;

      if (term_data.sb_ftp_user[0] != 0x00) {
        strcpy(ftp_user, term_data.sb_ftp_user);
        strcpy(ftp_password, term_data.sb_ftp_pwd);
      } else {
        ReadRTC(&dtg);
        strcpy(ftp_user, "stisYYYYMM");
        strcpy(ftp_password, "YYYYMMstis");

        split(&ftp_user[4], (BYTE *)&dtg, 3);
        split(&ftp_password[0], (BYTE *)&dtg, 3);
      }

      if (!LoginFtp(ftp_user, ftp_password))
        continue;
    }

    if (PutFileFtp(remote_folder, remote_file, local_file, FALSE)) {
      log_done = TRUE;
      break;
    }
  } while (log_retry < 3);

  //In the STIS logic, it will first check if the log exist and then find the mirror list.
  //if the log cannot be uploaded to the host, retry upload
  if (mirror_done && log_done) {
    SetAppListUploadReq(APPL_UPLOAD_DONE);
    UpdateAppListChksum();
  }

  for (i=0; i<0xFF; i++) {
    if (STISFTPAppDelAllow(i)) { //26-06-14++ SC (1)
      app_found = FALSE;
      if (AppInfoGetMW(i, &app_info)) {
        strcpy(temp_app_name, (i==0)?app_info.sb_app_name:&app_info.sb_app_name[2]);  // cater 1st 2 bytes are ID except system.
        for (j=NUM_PARAM_FILES; j<NUM_PARAM_FILES+MAX_APP_DL; j++) {
          if (!DL_file_list[j].exist)
            continue;
          if (strcmp(DL_file_list[j].app_name, temp_app_name) == 0) {
            app_found = TRUE;
          }
        }

        if (!app_found) {
          polling = TRUE;
        }
      }
    }
  }

  if (polling) {
    aRemoteDl->scheduler = DLOAD_ALL;
  }
  // End
  ResetFtp();

  if (aRemoteDl->DL_daily != DL_DONE) {
    DispClrBelowMW(MW_LINE3);
    DispLineMW(" POLLING FAILED ", MW_LINE5, MW_BIGFONT);
  } else {
    DispClrBelowMW(MW_LINE3);
    DispLineMW("POLLING COMPLETE", MW_LINE5, MW_BIGFONT);
  }
  return;
}

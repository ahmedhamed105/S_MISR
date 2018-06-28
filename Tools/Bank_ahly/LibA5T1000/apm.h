//-----------------------------------------------------------------------------
//  File          : apm.h
//  Module        : ARM9Term
//  Description   : Header for APM.
//  Author        : Lewis
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_MANAGER_H_
#define _INC_MANAGER_H_
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include <string.h>
#include "common.h"
#include "midware.h"

//-----------------------------------------------------------------------------
//      Configuration Defines
//-----------------------------------------------------------------------------
#if (PR608D)
  #define ECR_COMM_PORT       DEV_ECR1
#elif (TIRO)
  #define ECR_COMM_PORT       DEV_AUX1
#elif (PR608)
  #define ECR_COMM_PORT       DEV_AUX1
#endif

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define APM_ID          0x10


//-----------------------------------------------------------------------------
//   Application manager Functions define 
//-----------------------------------------------------------------------------
enum {
  APM_GETHWHANDLE             , // 01       Misc Func
  APM_CLRKEY                  , // 02       Kbd Func
  APM_GETKEY                  , // 03
  APM_WAITKEY                 , // 04
  APM_YESNO                   , // 05
  APM_GETKBD                  , // 06
  APM_SETKBDTO                , // 07
  APM_GETKBDTO                , // 08
  APM_PACKCOMM                , // 09       Comm Func
  APM_PRECONNECT              , // 10       
  APM_CONNECTOK               , // 11       
  APM_SENDRCVD                , // 12       
  APM_FLUSHCOMM               , // 13
  APM_RESETCOMM               , // 14
  APM_GETTERMDATA             , // 15       STIS Func
  APM_GETTERMCFG              , // 16
  APM_GETCARDCOUNT            , // 17
  APM_GETCARDTBL              , // 18
  APM_GETISSUERCOUNT          , // 19
  APM_GETISSUER               , // 20   
  APM_GETACQCOUNT             , // 16
  APM_GETACQTBL               , // 17
  APM_GETDESCCOUNT            , // 18
  APM_GETDESCTBL              , // 19
  APM_GETLOGOCOUNT            , // 20
  APM_GETLOGOTBL              , // 21
  APM_GETEXPARAMCOUNT         , // 22
  APM_GETEXPARAM              , // 23
  APM_GETPENDING              , // 24
  APM_SETPENDING              , // 25
  APM_GETBATCH                , // 26
  APM_SETBATCH                , // 27
  APM_SELACQUIRER             , // 28
  APM_SETROC                  , // 29
  APM_SETTRACE                , // 30
  APM_STIS_CLR                , // 31
  APM_STIS_ADDTBL             , // 32    
  APM_TERMDATA_CLR            , // 33
  APM_TERMDATA_UPD            , // 34    
  APM_BATCH_CLEAR             , // 35       Batch Rec Func
  APM_BATCH_FULL              , // 36
  APM_GET_REC_COUNT           , // 37
  APM_GET_BATCH_REC           , // 38
  APM_GET_REV_REC             , // 39
  APM_CLR_REV_REC             , // 40
  APM_SAVE_BAT_REC            , // 41
  APM_UPD_BAT_REC             , // 42
  APM_TRACE_IN_BATCH          , // 43
  APM_TRACE_IN_REV            , // 44
  APM_GETACCESSCODE           , // 45
  APM_SELYESNO                , // 46
  APM_SELAPP                  , // 47
  APM_MERCHFUNC               , // 48
  APM_PWRSLEEP                , // 49
  APM_FUNC_COUNT                // Must be the last item
};

//-----------------------------------------------------------------------------
//  Misc Functions
//-----------------------------------------------------------------------------
enum {
  APM_SDEV_MSR  = 0 ,
  APM_SDEV_ICC      ,
  APM_SDEV_GPRS     ,
  APM_SDEV_PPP      ,
  APM_SDEV_CDC      ,
  APM_SDEV_MDM      ,
  APM_SDEV_WIFI     ,
  APM_SDEV_MAX
};

// extern int GetHwHandle(DWORD aHwId, DWORD aClose);
#define APM_GetHwHandle(x)      (int) lib_app_call(APM_ID, APM_GETHWHANDLE, (DWORD)x, (DWORD)0, (DWORD)0)
#define APM_CloseHwHandle(x)    (int) lib_app_call(APM_ID, APM_GETHWHANDLE, (DWORD)x, (DWORD)1, (DWORD)0)

//-----------------------------------------------------------------------------
//  Kbd Functions
//-----------------------------------------------------------------------------
// Define time out value
#define TIME_1SEC       100
#define KBD_TIMEOUT     (90*TIME_1SEC)      // 90 sec

//extern void SetKbdTimeout(DWORD aTimeout);
#define APM_SetKbdTimeout(x)            lib_app_call(APM_ID, APM_SETKBDTO, (DWORD)x, (DWORD)0, (DWORD)0)

//extern DWORD GetKbdTimeout(void);
#define APM_GetKbdTimeout()             lib_app_call(APM_ID, APM_GETKBDTO, (DWORD)0, (DWORD)0, (DWORD)0)

#if (A5T1000|T1000)
#define KEY_ALPHA       MWKEY_LEFT
#else
#define KEY_ALPHA       MWKEY_FUNC1
#endif

// WaitKey Check List
#define WAIT_ICC_INSERT 0x0100
#define WAIT_ICC_REMOVE 0x0200
#define WAIT_MSR_READY  0x0400

// extern void ClearKeyin(void);
#define APM_ClearKeyin()                lib_app_call(APM_ID, APM_CLRKEY, (DWORD)0, (DWORD)0, (DWORD)0)

// extern DWORD GetKeyin(void);
#define APM_GetKeyin()                  lib_app_call(APM_ID, APM_GETKEY, (DWORD)0, (DWORD)0, (DWORD)0)

// extern DWORD WaitKey(DWORD aTimeout, DWORD aCheckList);
#define APM_WaitKey(x,y)                lib_app_call(APM_ID, APM_WAITKEY, (DWORD)x, (DWORD)y, (DWORD)0)

//extern DWORD YesNo(void);
#define APM_YesNo()                     lib_app_call(APM_ID, APM_YESNO, (DWORD)0, (DWORD)0, (DWORD)0)

// GetKbd Mode
#define NUMERIC_INPUT   0x00000000
#define AMOUNT_INPUT    0x00010000
#define ALPHA_INPUT     0x00020000
#define HIDE_NUMERIC    0x00030000
#define HEX_INPUT       0x00040000
#define BINARY_INPUT    0x00050000
#define ALPHA_INPUT_NEW 0x00060000
#define HEX_INPUT_NEW   0x00070000
#define DECIMAL_INPUT   0x00080000
// GetKbd Option
#define JUSTIFIED       0x00100000
#define ECHO            0x00200000
#define PREFIX_ENB      0x00400000
#define NULL_ENB        0x00800000
// GetKbd Decimal Pos
#define DECIMAL_NONE    0x00000000
#define DECIMAL_POS1    0x01000000
#define DECIMAL_POS2    0x02000000
#define DECIMAL_POS3    0x03000000

#define MAX_INPUT_LEN   (MW_MAX_LINESIZE*2)
#define PREFIX_SIZE      4
#define MAX_INPUT_PREFIX   (MAX_INPUT_LEN - PREFIX_SIZE)

#define IMIN(x)         (x<<8)
#define IMAX(x)         (x&0xFF)

#define RIGHT_JUST      (0xFF)

// aEntryMode = Decimal_pos | Option | Mode | MW_xxFONT | MW_LINEx | x position (0xFF=>Right Justify)
// aLen       = IMIN(a) + IMAX(b)
extern BOOLEAN GetKbd(DWORD aEntryMode, DWORD aLen, BYTE *aOutBuf);
#define APM_GetKbd(x,y,z)               (BOOLEAN) lib_app_call(APM_ID, APM_GETKBD, (DWORD)x, (DWORD)y, (DWORD)z)


//-----------------------------------------------------------------------------
//  Comm Functions
//-----------------------------------------------------------------------------
// Comm Mode
#define APM_COMM_SYNC         0
#define APM_COMM_ASYNC        1
#define APM_COMM_TCPIP        2
#define APM_COMM_GPRS         3
#define APM_COMM_AUX          4
#define APM_COMM_WIFI         5
#define APM_COMM_BT           6

// Comm Return Value
#define COMM_OK               ('0'*256+'0')
#define COMM_CANCEL           ('C'*256+'N')
#define COMM_NO_DIALTONE      ('D'*256+'N')
#define COMM_HOST_OFF         ('H'*256+'O')
#define COMM_LINEBUSY         ('L'*256+'B')
#define COMM_LINEDROP         ('L'*256+'C')
#define COMM_NO_LINE          ('L'*256+'N')
#define COMM_LINEOCCP         ('L'*256+'O')
#define COMM_NOT_CONNECT      ('N'*256+'C')
#define COMM_TIMEOUT          ('T'*256+'O')

struct MDMSREG {
  BYTE bLen;
  struct MW_MDM_PORT sMdmCfg;
};

struct TCPSREG {
  BYTE bLen;
  BYTE bAdd2ByteLen;
  struct MW_TCP_PORT2 sTcpCfg;
};

struct AUXSREG {
  BYTE bLen;
  BYTE bPort;
  struct MW_AUX_CFG sAuxCfg;
};
enum {
  APM_AUX1  = 0,
  APM_AUX2     ,
  MAX_APM_AUX  ,
};

struct PPPSREG {
  BYTE  bKeepAlive;       // TRUE => PPP connection never close.
  DWORD dDevice;
  DWORD dSpeed;
  DWORD dMode;
  char  scAPN[128];
  char  scUserID[128];
  char  scPwd[128];
  struct MW_EXPECT_SEND *psLogin;
  DWORD dLoginPair;
  struct MW_EXPECT_SEND *psDialUp;
  DWORD dDialupPair;
  struct MW_MDM_PORT    *psDialParam;
};

struct COMMPARAM {
  WORD wLen;              // size of comm param (sizeof(struct COMMPARAM))
  BYTE bCommMode;         // COMM mode
  BYTE bHostConnTime;     // host connection timeout in seconds
  BYTE bTimeoutVal;       // host response timeout in seconds
  BYTE bAsyncDelay;       // delay for async after connection in seconds 
  struct MDMSREG sMdm;
  struct TCPSREG sTcp;
  struct AUXSREG sAux;
  struct PPPSREG sPPP;
};

#define COMMBUF_SIZE      3*1024
struct COMMBUF {
  WORD wLen;
  BYTE sbContent[COMMBUF_SIZE];
};

// Return Value
#define COMM_OK               ('0'*256+'0')
#define COMM_NO_DIALTONE      ('D'*256+'N')
#define COMM_HOST_OFF         ('H'*256+'O')
#define COMM_LINEBUSY         ('L'*256+'B')
#define COMM_LINEDROP         ('L'*256+'C')
#define COMM_NO_LINE          ('L'*256+'N')
#define COMM_LINEOCCP         ('L'*256+'O')
#define COMM_NOT_CONNECT      ('N'*256+'C')
#define COMM_TIMEOUT          ('T'*256+'O')

//extern void PackComm(struct COMMPARAM *aDat);
#define APM_PackComm(x)      lib_app_call(APM_ID, APM_PACKCOMM, (DWORD)x, 0, 0)

//extern void PreConnect(void);
#define APM_PreConnect()     lib_app_call(APM_ID, APM_PRECONNECT, (DWORD)0, (DWORD)0, (DWORD)0)

//extern DWORD ConnectOK(BOOLEAN aNoCancel);
#define APM_ConnectOK(x)     lib_app_call(APM_ID, APM_CONNECTOK, (DWORD)x, (DWORD)0, (DWORD)0)

//extern DWORD SendRcvd(struct COMMBUF *aSendMsg, struct COMMBUF *aRcvdMsg);
#define APM_SendRcvd(x, y)   lib_app_call(APM_ID, APM_SENDRCVD, (DWORD)x, (DWORD)y, (DWORD)0)

//extern void FlushConnection(void);
#define APM_FlushComm()      lib_app_call(APM_ID, APM_FLUSHCOMM, (DWORD)0, (DWORD)0, (DWORD)0)

//extern DWORD ResetComm(void);
#define APM_ResetComm()      lib_app_call(APM_ID, APM_RESETCOMM, (DWORD)0, (DWORD)0, (DWORD)0)

//-----------------------------------------------------------------------------
//      STIS Func
//-----------------------------------------------------------------------------
// STIS Mode
#define SETUP_MODE      0
#define INIT_MODE       1
#define TRANS_MODE      2

// Init Mode
#define SYNC_DIALUP     0
#define TCPIP_DIALUP    1
#define ASYNC_DIALUP    2

// Dial Mode
#define DIAL_TONE       0
#define DIAL_PULSE      1

#define MAGIC                 0x5A5A

// SSL Key Index
#define SSL_CA_IDX_TMS        0xFC
#define SSL_CA_IDX_ECR        0xFD
#define SSL_CA_IDX_HOST       0xFF
#define SSL_CLN_CERT_IDX      0xFF
#define SSL_CLN_PRV_KEY_IDX   0xFF

// Table ID
enum {
  APM_TERM_CFG = 0,
  APM_CARD_TBL,
  APM_ISSUER_TBL,
  APM_ACQUIRER_TBL,
  APM_DESC_TBL,
  APM_LOGO_TBL,
};

struct TERM_DATA {
  BYTE b_stis_mode;
  BYTE b_disp_contrast;
  BYTE sb_term_id[9];
  BYTE b_init_mode;
  BYTE sb_pri_no[12];
  BYTE b_pri_conn_time;
  BYTE b_pri_redial;
  BYTE sb_sec_no[12];
  BYTE b_sec_conn_time;
  BYTE b_sec_redial;
  BYTE b_async_delay;
  BYTE b_dial_mode;
  BYTE sb_pabx[4];
  BYTE sb_trace_no[3];
  BYTE sb_roc_no[3];
  BYTE sb_ip[4];
  BYTE sb_port[2];
  BYTE sb_init_nii[2];
  BYTE b_emv_enable;
  BYTE b_default_app;
  BYTE b_return_timeout;
  WORD w_training_status;
  WORD w_debug_status;
  DDWORD dd_default_amount;
  BYTE b_ppad_port;             // 0 - Not support, 1-Aux1, 2, Aux2, 3-Auxdbg
  BYTE b_ecr_port;              // 0 - Not support, 1-Aux1, 2, Aux2, 3-Auxdbg
  //29-09-16 JC ++
  BYTE b_ecr_ssl;               // 0-No SSL, 1-server auth, 2-client auth
  BYTE sb_edc_chksum[2];        // BCD
  BYTE sb_edc_date[4];          // BCD ccyymmdd
  DDWORD sb_edc_size;         // DDWORD
  BYTE sb_emvapp_chksum[2];     // BCD
  BYTE sb_emvapp_date[4];       // BCD ccyymmdd
  DDWORD sb_emvapp_size;         // DDWORD
  BYTE sb_emvkey_chksum[2];     // BCD
  BYTE sb_emvkey_date[4];       // BCD ccyymmdd
  DDWORD sb_emvkey_size;         // DDWORD
  BYTE sb_xapp_chksum[2];       // BCD
  BYTE sb_xapp_date[4];         // BCD ccyymmdd
  DDWORD sb_xapp_size;         // DDWORD
  BYTE sb_lxapp_chksum[2];      // BCD
  BYTE sb_lxapp_date[4];        // BCD ccyymmdd
  DDWORD sb_lxapp_size;         // DDWORD
  BYTE sb_ftp_user[32];
  BYTE sb_ftp_pwd[32];
  BYTE sb_ppp_user[32];
  BYTE sb_ppp_pwd[32];
  BYTE sb_ppp_ip[4];
  BYTE sb_ppp_port[2];
  BYTE sb_server_key[25];       // 24 ascii key
  BYTE sb_checkin_pri_no[12];
  BYTE b_checkin_pri_conn_time;
  BYTE b_checkin_pri_redial;
  BYTE sb_checkin_sec_no[12];
  BYTE b_checkin_sec_conn_time;
  BYTE b_checkin_sec_redial;
  BYTE b_init_fallback;
  WORD w_app_list_chksum;
  BYTE b_app_list_upload;
  //29-09-16 JC --
  DWORD w_crc;
};

#define NOT_LOADED      0xFF
#define UP              0
#define DOWN            1

#define NO_PENDING      0
#define REV_PENDING     1
#define SETTLE_PENDING  2

struct TERM_CFG {
   BYTE b_dll;
  BYTE b_init_control;
  BYTE b_rsvd0;
  BYTE b_rsvd1;
  BYTE sb_date_time[6];
  BYTE b_dial_option;
  BYTE sb_password[2];
  BYTE sb_help_tel[12];
  BYTE sb_options[4];
  BYTE sb_name_loc[46];
  BYTE sb_dflt_name[23];
  BYTE b_currency;
  BYTE b_trans_amount_len;
  BYTE b_decimal_pos;
  BYTE b_rsvd2;
  BYTE b_settle_amount_len;
  BYTE sb_currency_name[3];
  BYTE b_local_option;
  BYTE sb_additional_prompt[20];
  BYTE sb_reserved[30];
  BYTE sb_psw_refund[16];
  BYTE sb_psw_void[16];
  BYTE apn_username[10];
  BYTE apn_password[12];
  BYTE apn2_username[10];
  BYTE apn2_password[12];
  BYTE apnconfig[32];
  BYTE apnconfig2[32];
  WORD w_crc;
};

struct CARD_TBL {
  BYTE sb_pan_range_low[5];
  BYTE sb_pan_range_high[5];
  BYTE b_issuer_id;
  BYTE b_acquirer_id;
  BYTE b_pan_len;
  BYTE sb_reserved[20];
  WORD w_crc;
};

struct ISSUER_TBL {
  BYTE b_id;
  BYTE sb_card_name[10];
  BYTE sb_ref_tel_no[12];
  BYTE sb_options[4];
  BYTE b_default_acc;
  BYTE sb_pan_format[2];
  BYTE sb_floor_limit[2];
  BYTE sb_reserved[20];
  WORD w_crc;
};

// Pending defines 
#define NO_PENDING      0
#define REV_PENDING     1
// status define
#define NOT_LOADED      0xFF
#define UP              0
#define DOWN            1

struct ACQUIRER_TBL {
  BYTE b_id;
  BYTE sb_program[10];
  BYTE sb_name[10];
  BYTE sb_pri_trans_tel[12];
  BYTE b_pri_trans_conn_time;
  BYTE b_pri_trans_redial;
  BYTE sb_sec_trans_tel[12];
  BYTE b_sec_trans_conn_time;
  BYTE b_sec_trans_redial;
  BYTE sb_pri_settle_tel[12];
  BYTE b_pri_settle_conn_time;
  BYTE b_pri_settle_redial;
  BYTE sb_sec_settle_tel[12];
  BYTE b_sec_settle_conn_time;
  BYTE b_sec_settle_redial;
  BYTE b_trans_mdm_mode;
  BYTE sb_options[4];
  BYTE sb_nii[2];
  BYTE sb_term_id[8];
  BYTE sb_acceptor_id[15];
  BYTE b_timeout_val;
  BYTE sb_curr_batch_no[3];
  BYTE sb_next_batch_no[3];
  BYTE sb_visa1_term_id[23];
  BYTE b_settle_mdm_mode;
  BYTE sb_reserved[21];
  BYTE b_host_type;
  BYTE b_reserved1;
  BYTE b_ssl_key_idx;
  BYTE sb_ip[4];
  BYTE sb_port[2];
  BYTE b_status;        /* 0 - loaded, OTHER - empty */
  BYTE b_pending;
  WORD w_crc;
};

struct DESC_TBL {
  BYTE b_key;
  BYTE sb_host_tx_code[2];
  BYTE sb_text[20];
  WORD w_crc;
};

struct LOGO_TBL {
  BYTE b_flag;
  BYTE b_type;
  BYTE sb_image[384];
  WORD w_crc;
};

struct EXTRA_PARAM {
  BYTE b_app_id;
  BYTE b_seq_no;
  BYTE b_len;
  BYTE b_prefix;
  BYTE sb_content[20];
  WORD w_crc;
};

// extern BOOLEAN GetTermData(struct TERM_DATA *aDat);
#define APM_GetTermData(x)       (BOOLEAN) lib_app_call(APM_ID, APM_GETTERMDATA, (DWORD)x, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetTermCfg(struct TERM_CFG *aDat);
#define APM_GetTermCfg(x)       (BOOLEAN) lib_app_call(APM_ID, APM_GETTERMCFG, (DWORD)x, (DWORD)0, (DWORD)0)

// extern int GetCardCount(void);
#define APM_GetCardCount()      (int) lib_app_call(APM_ID, APM_GETCARDCOUNT, (DWORD)0, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetCardTbl(DWORD aIdx, struct CARD_TBL *aDat);
#define APM_GetCardTbl(x,y)     (BOOLEAN) lib_app_call(APM_ID, APM_GETCARDTBL, (DWORD)x, (DWORD)y, (DWORD)0)

// extern int GetIssuerCount(void);
#define APM_GetIssuerCount()    (int) lib_app_call(APM_ID, APM_GETISSUERCOUNT, (DWORD)0, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetIssuerTbl(DWORD aIdx, struct ISSUER_TBL *aDat);
#define APM_GetIssuer(x,y)      (BOOLEAN) lib_app_call(APM_ID, APM_GETISSUER, (DWORD)x, (DWORD)y, (DWORD)0)

// extern int GetAcqCount(void);
#define APM_GetAcqCount()       (int) lib_app_call(APM_ID, APM_GETACQCOUNT, (DWORD)0, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetAcqTbl(DWORD aIdx, struct ACQUIRER_TBL *aDat);
#define APM_GetAcqTbl(x,y)      (BOOLEAN) lib_app_call(APM_ID, APM_GETACQTBL, (DWORD)x, (DWORD)y, (DWORD)0)

// extern int GetDescCount(void);
#define APM_GetDescCount()      (int) lib_app_call(APM_ID, APM_GETDESCCOUNT, (DWORD)0, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetDescTbl(DWORD aIdx, struct DESC_TBL *aDat);
#define APM_GetDescTbl(x,y)     (BOOLEAN) lib_app_call(APM_ID, APM_GETDESCTBL, (DWORD)x, (DWORD)y, (DWORD)0)

// extern int GetLogoCount(void); 
#define APM_GetLogoCount()      (int) lib_app_call(APM_ID, APM_GETLOGOCOUNT, (DWORD)0, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetLogoTbl(DWORD aIdx, struct LOGO_TBL *aDat); 
#define APM_GetLogoTbl(x,y)     (BOOLEAN) lib_app_call(APM_ID, APM_GETLOGOTBL, (DWORD)x, (DWORD)y, (DWORD)0)

// extern int GetExParamCount(void);
#define APM_GetExParamCount()   (int) lib_app_call(APM_ID, APM_GETEXPARAMCOUNT, (DWORD)0, (DWORD)0, (DWORD)0)

// extern BOOLEAN GetExtraParam(DWORD aIdx, struct EXTRA_PARAM *aDat);
#define APM_GetExtraParam(x,y)  (BOOLEAN) lib_app_call(APM_ID, APM_GETEXPARAM, (DWORD)x, (DWORD)y, (DWORD)0)

// extern BYTE GetPending(int aIdx);
#define APM_GetPending(x)       (BYTE) lib_app_call(APM_ID, APM_GETPENDING, (DWORD)x, (DWORD)0, (DWORD)0)

// extern void SetPending(int aIdx, BYTE aVal);
#define APM_SetPending(x,y)     lib_app_call(APM_ID, APM_SETPENDING, (DWORD)x, (DWORD)y, (DWORD)0)

// extern BOOLEAN GetBatchNo(DWORD aIdx, BYTE *aVal);
#define APM_GetBatchNo(x,y)     (BOOLEAN) lib_app_call(APM_ID, APM_GETBATCH, (DWORD)x, (DWORD)y, (DWORD)0)

// extern void SetBatchNo(DWORD aIdx, BYTE *aVal);
#define APM_SetBatchNo(x,y)     lib_app_call(APM_ID, APM_SETBATCH, (DWORD)x, (DWORD)y, (DWORD)0)

// extern int SelectAcquirer(int aInclAll);
#define APM_SelectAcquirer(x)   (int) lib_app_call(APM_ID, APM_SELACQUIRER, (DWORD)x, (DWORD)0, (DWORD)0)

// extern void SetROC(void *aNewROC);
#define APM_SetROC(x)           lib_app_call(APM_ID, APM_SETROC, (DWORD)x, (DWORD)0, (DWORD)0)

// extern void SetTrace(void *aNewTrace);
#define APM_SetTrace(x)         lib_app_call(APM_ID, APM_SETTRACE, (DWORD)x, (DWORD)0, (DWORD)0)

//extern void CleanStisTable(void);
#define APM_CleanStisTbl()      lib_app_call(APM_ID, APM_STIS_CLR, 0, 0, 0)

//extern DWORD StisAdd(int aTableId, void *aDat);
#define APM_StisAdd(x, y)       lib_app_call(APM_ID, APM_STIS_ADDTBL, (DWORD)x, (DWORD)y, 0)

//extern void CleanTermData(void);
#define APM_CleanTermData()      lib_app_call(APM_ID, APM_TERMDATA_CLR, 0, 0, 0)

//extern DWORD TermDataUpd(void *aDat);
#define APM_TermDataUpd(x)    lib_app_call(APM_ID, APM_TERMDATA_UPD, (DWORD)x, 0, 0)

//-----------------------------------------------------------------------------
//      Batch Rec Func
//-----------------------------------------------------------------------------
#define STS_REC_EMPTY       'E'
#define STS_REC_BATCH       'B'
#define STS_REC_REVERSAL    'R'
#define STS_REC_UNKNOWN     'U'

struct BATSYS_HDR {
  BYTE b_status;
  BYTE sb_trace_no[3];
  BYTE sb_roc_no[3];
  WORD w_acq_id;
  WORD w_app_id;
};
//extern void BatSysClear(int aAcqId);   // aAcqId == -1 => All
#define APM_BatchClear(x)     lib_app_call(APM_ID, APM_BATCH_CLEAR, (DWORD)x, 0, 0)

//extern BOOLEAN BatchFull(int aAcqId);
#define APM_BatchFull(x)      lib_app_call(APM_ID, APM_BATCH_FULL, (DWORD)x, 0, 0)

//extern WORD GetRecCount(void);
#define APM_GetRecCount()     lib_app_call(APM_ID, APM_GET_REC_COUNT, 0, 0, 0)

//extern BOOLEAN GetBatchRec(int aIdx, void *aRec, int aRecSize);
#define APM_GetBatchRec(x, y, z) (BOOLEAN) lib_app_call(APM_ID, APM_GET_BATCH_REC, (DWORD)x, (DWORD)y, (DWORD)z)

//extern int GetRevRec(int aAcqId, void *aRec);
#define APM_GetRevRec(x, y, z)   (int) lib_app_call(APM_ID, APM_GET_REV_REC, (DWORD)x, (DWORD)y, (DWORD)z)

//extern void ClearRevRec(int aAcqId);
#define APM_ClearRevRec(x)    lib_app_call(APM_ID, APM_CLEAR_REV_REC, (DWORD)x, 0, 0)

//extern BOOLEAN SaveBatRec(void *aRec, struct BATSYS_HDR *aHdr);
#define APM_SaveBatRec(x, y)  (BOOLEAN) lib_app_call(APM_ID, APM_SAVE_BAT_REC, (DWORD)x, (DWORD)y, 0)

//extern BOOLEAN UpdateBatRec(int aIdx, void *aRec, struct BATSYS_HDR *aHdr);
#define APM_UpdateBatRec(x, y, z)   (BOOLEAN) lib_app_call(APM_ID, APM_UPD_BAT_REC, (DWORD)x, (DWORD)y, (DWORD)z)

//extern int TraceInBatch(BYTE *aTraceNo);
#define APM_TraceInBatch(x)   (int) lib_app_call(APM_ID, APM_TRACE_IN_BATCH, (DWORD)x, 0, 0)

//extern BOOLEAN TraceInReversal(BYTE *aTraceNo);
#define APM_TraceInReversal(x)   (BOOLEAN) lib_app_call(APM_ID, APM_TRACE_IN_REV, (DWORD)x, 0, 0)

//extern BOOLEAN GetAccessCode(void);
#define APM_GetAccessCode()      (BOOLEAN) lib_app_call(APM_ID, APM_GETACCESSCODE, 0, 0, 0)

//extern BOOLEAN APM_SelYesNo(void);
#define APM_SelYesNo()           lib_app_call(APM_ID, APM_SELYESNO, 0, 0, 0)

//extern BOOLEAN APM_SelApp(void);
#define APM_SelApp(x)            lib_app_call(APM_ID, APM_SELAPP, x, 0, 0)

//extern BOOLEAN APM_MerchFunc(DWORD aId);
#define APM_MerchFunc(x)         lib_app_call(APM_ID, APM_MERCHFUNC, x, 0, 0)

//extern int APM_PwrSleep(DWORD aSec);
#define APM_PwrSleep(x)          lib_app_call(APM_ID, APM_PWRSLEEP, x, 0, 0)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //_INC_MANAGER_H_




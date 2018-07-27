//-----------------------------------------------------------------------------
//  File          : debugstis.c
//  Module        :
//  Description   : Include DEBUG STIS data handling routines.
//  Author        : Lewis
//  Notes         :
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
//  19 Aug  2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "apm.h"
#include "ecrdll.h"
#include "testcfg.h"
#include "debugstis.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
// Default Host Connection Setting
#if (T300|A5T1000|T1000|T800)
 //#define USE_MODEM       1
 //#define USE_TCPIP       2
 #define USE_GPRS          3
#else
 #define USE_MODEM       1
 //#define USE_TCPIP       2
 //#define USE_GRPS        3
#endif

// Debug Data 
#define MAX_CONF_DATA   1

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------

static const struct TERM_CFG KTermCfg =
{
  0x00,                                                                     // DLL
  0x00,                                                                     // Init Control 00:No Action 01:Clear Batch
  0x00,                                                                     // Rsvd 0
  0x00,                                                                     // Rsvd 1
  {0x08, 0x01, 0x01, 0x00, 0x00, 0x00},                                     // Date Time YYMMDDHHMMSS
  0x00,                                                                     // Dial Option
  {'0', '0'},                                                               // Password
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Help Desk Phone
  {0xA3,                                                                    // Option 1 (Confirm Total, BS MMDD, print time, N/A, N/A, N/A, TIPs, Not DispMsr)
   0x02,                                                                    // Option 2 (N/A,N/A,N/A,N/A,N/A,N/A, No Printer, ECR ref)
   0x00,                                                                    // Option 3
   0x00                                                                     // Option 4
  },
  {"  AHL DEMO APPLICATION   SPECTRA TECHNOLOGIES "},                       // Name/Location
  {"      EMV/MSR CARD     "},                                              // Default Name
  '$',                                                                     // Currency Symbol
  8,                                                                        // Trans Amount len
  2,                                                                        // Decimal Pos
  0x00,                                                                     // Rsvd 2
  12,                                                                       // Settlement Amount len
  {"   "},                                                                  // Currency Name
  0x00,                                                                     // Local Option
  {"                    "},                                                 // Additional Data
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              // Reserved field [30]
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              //
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },             //
  {"1111111111111111"},                                                     // Password - Refund
  {"1111111111111111"},                                                     // Password - Void
  {0x00},                                                                   // username APN1
  {0x00},                                                                   // Password APN1
  {0x00},                                                                   // username APN2
  {0x00},                                                                   // Password APN2
  {0x00},                                                                   // apnconfig
  {0x00},                                                                   // apnconfig2
  {210, 3, 29, 89},                                                         // apn2_ip
  {0x15, 0xB3},                                                             // apn2_port
  0x0000,                                                                   // CRC
};
static const struct CARD_TBL KCardCfg[MAX_CONF_DATA] =
{
  {
    {0x00,0x00,0x00,0x00,0x00},                                             // BYTE pan_range_low[5];
    {0x99,0x99,0x99,0x99,0x99},                                             // BYTE pan_range_high[5];
    0x00,                                                                   // BYTE issuer_id;
    0x00,                                                                   // BYTE acquirer_id;
    0x00,                                                                   // BYTE pan_len;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,            // Reserved field [20]
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },           //
    0x0000,
  }
};
static const struct ISSUER_TBL KIssuerCfg[MAX_CONF_DATA] =
{
  {
    0x00,                                                                     // BYTE id;
    {"ALL CARD  "},                                                           // BYTE card_name[10];
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Reference No
    {0xBC,0x1C,0x00,0x00},                                                    // BYTE options[4];
    0x00,                                                                     // BYTE default_acc;
    {0x00,0x00},                                                              // BYTE pan_format[2];
    {0x00,0x00},                                                              // BYTE floor_limit[2];
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              // Reserved field [20]
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },             //
    0x0000,
  },
};
static const struct ACQUIRER_TBL KAcquirerCfg[MAX_CONF_DATA] =
{
  {
    0x00,                                                                     // BYTE id;
    {"EMV TESTER"},                                                           // BYTE program[10];
    {"SPECTRA   "},                                                           // BYTE name[10];
    {0x35, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Primary Trans Phone
    0x30,                                                                     // BYTE pri_trans_conn_time;
    0x03,                                                                     // BYTE pri_trans_redial;
    {0x35, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Secondary Trans Phone
    0x30,                                                                     // BYTE sec_trans_conn_time;
    0x03,                                                                     // BYTE sec_trans_redial;
    {0x35, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Primary Settle Phone
    0x30,                                                                     // BYTE pri_settle_conn_time;
    0x03,                                                                     // BYTE pri_settle_redial;
    {0x35, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Secondary Settle Phone
    0x30,                                                                     // BYTE sec_settle_conn_time;
    0x03,                                                                     // BYTE sec_settle_redial;
    0x01,                                                                     // BYTE trans_mdm_mode;
    {0x80,0x00,0x00,0x00},                                                    // BYTE options[4];
    {0x00,0x01},                                                              // BYTE nii[2];
    {"40000000"},                                                             // BYTE term_id[8];
    {"9795818996     "},                                                      // BYTE acceptor_id[15];
    0x10,                                                                     // BYTE timeout_val;
    {0x00,0x00,0x01},                                                         // BYTE curr_batch_no[3];
    {0x00,0x00,0x02},                                                         // BYTE next_batch_no[3];
    {"                       "},                                              // BYTE visa1_term_id[23];
    0x01,                                                                     // BYTE trans_mdm_mode;
    #if (USE_GPRS)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // Reserved Field [21];
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    0x00,                                                                     // BYTE host_type;  (Visa/Master)
    0x80,                                                                     // BYTE reserved1;  (Extra Len)
    0x80,                                                                     // BYTE ssl_key_idx;(PPP)
    {210, 3, 29, 89},                                                         // BYTE ip[4]       (TCPIP)
    {0x15, 0xB3},                                                             // BYTE port[2]     (5555)
    #elif (USE_TCPIP)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // Reserved Field [21];
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    0x00,                                                                     // BYTE host_type;  (Visa/Master)
    0x80,                                                                     // BYTE reserved1;  (Extra Len)
    0x00,                                                                     // BYTE ssl_key_idx;
    {210, 3, 29, 89},                                                         // BYTE ip[4]       (TcpIP)
    {0x15, 0xB3},                                                             // BYTE port[2]     (5555)
    #elif (USE_MODEM)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // Reserved Field [21];
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    0x00,                                                                     // BYTE host_type;  (Visa/Master)
    0x00,                                                                     // BYTE reserved1;  (Extra Len)
    0x00,                                                                     // BYTE ssl_key_idx;
    {0x00, 0x00, 0x00, 0x00},                                                 // BYTE ip[4]       (Modem)    
    //{0xFF, 0xFF, 0xFF, 0xFF},                                                 // BYTE ip[4]       (Aux)
    {0x15, 0xB3},                                                             // BYTE port[2]     (5555)
    #endif
    0x00,                                                                     // BYTE status 0 - Loaded, others - empty
    0x00,                                                                     // BYTE pending
    0x0000,                                                                   // CRC
  },
};

static const struct TERM_DATA KDefaultTermData = {
  TRANS_MODE,                                                               // STIS mode
  12,                                                                       // LCD contrast
  {'5', '0', '0', '0', '0', '0', '0', '0', '5'},                            // Term ID
  APM_COMM_SYNC,                                                                // Init Mode(COMM_SYNC = 0, COMM_TCPIP, COMM_ASYNC)
  {0x22, 0x64, 0x32, 0x95, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Primary Phone
  0x30, 0x02,                                                               // Pri Conn time, redial
  {0x22, 0x64, 0x32, 0x95, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Secondary Phone
  0x30, 0x02,                                                               // Sec Conn time, redial
  0x01,                                                                     // Async Conn Delay
  DIAL_TONE,                                                                // Dial Mode (TONE=0, PULSE)
  {0xFF, 0xFF, 0xFF, 0xFF},                                                 // PABX
  {0x00, 0x00, 0x01}, {0x00, 0x00, 0x01},                                   // Trace & ROC Num.
  {0x0A, 0x01, 0x01, 0x49}, {0x27, 0x11},                                   // InitIP & Port
  {0x00, 0x00},                                                             // Init NII
  0x00,                                                                     // EMV Enable
  0x10,                                                                     // Default Manager
  0x05,                                                                     // Return to Base Timeout
  0x0000,                                                                   // Training Status
  0x0000,                                                                   // Debug Status
  0x00,                                                                     // Default Amount
};

//*****************************************************************************
//  Function        : SaveSTISDbgData
//  Description     : Save STIS data for debugging purpose.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveSTISDbgData(DWORD aInitMode)
{
  int i;
  struct TERM_DATA    term_data;
  struct TERM_CFG     term_cfg;
  struct CARD_TBL     card_tbl;
  struct ISSUER_TBL   issuer_tbl;
  struct ACQUIRER_TBL acq_tbl;
#if (MCP_SUPPORT)
  T_APP_INFO app_info;
#endif

  APM_CleanTermData();
  memcpy(&term_data, &KDefaultTermData, sizeof(struct TERM_DATA));
#if (MCP_SUPPORT)
  // check if both Bluetooth & ECRDLL exist
  if ((os_hd_config() & K_HdBt) && os_app_info(ECRDLL_ID, &app_info)) {
    term_data.b_ecr_port = (6|ECR_RAW_MODE);     // turn on Bluetooth by default
  }
#endif
  APM_TermDataUpd(&term_data);

  APM_CleanStisTbl();
  if (aInitMode == MODE_ALL_PARAM) {
    memcpy(&term_cfg, &KTermCfg, sizeof(struct TERM_CFG));
    APM_StisAdd(APM_TERM_CFG, &term_cfg);

    for (i = 0; i < MAX_CONF_DATA; i++) {
      memcpy(&card_tbl, &KCardCfg[i], sizeof(struct CARD_TBL));
      APM_StisAdd(APM_CARD_TBL, &card_tbl);

      memcpy(&issuer_tbl, &KIssuerCfg[i], sizeof(struct ISSUER_TBL));
      APM_StisAdd(APM_ISSUER_TBL, &issuer_tbl);

      memcpy(&acq_tbl, &KAcquirerCfg[i], sizeof(struct ACQUIRER_TBL));
      APM_StisAdd(APM_ACQUIRER_TBL, &acq_tbl);
    }
  }
}


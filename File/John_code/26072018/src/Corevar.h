//-----------------------------------------------------------------------------
//  File          : corevar.h
//  Module        :
//  Description   : Declrartion & Defination for corevar.c
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
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _COREVAR_H_
#define _COREVAR_H_
#include "emv2dll.h"
#include "emvcl2dll.h"
#include "ecrdll.h"
#include "apm.h"
#include "hardware.h"
#include "sysutil.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define HANDLE_HOST_TYPE  0x00      // Visa/Master Host
//#define HANDLE_HOST_TYPE  0x0B      // Amex Host

#define COMMON_DATA                    \
    BYTE            b_trans;           \
    BYTE            b_trans_status;    \
    BYTE            sb_trace_no[3];    \
    BYTE            b_acc_ind;         \
    BYTE            sb_roc_no[3];      \
    BYTE            sb_pan[10];        \
    BYTE            sb_exp_date[2];    \
	BYTE            currency;          \
    DDWORD          dd_amount;         \
    DDWORD          dd_tip;            \
    DDWORD          dd_org_amount;     \
    BYTE            sb_product[4];     \
    BYTE            b_entry_mode;      \
    BYTE            sb_holder_name[26];\
    BYTE            sb_ecr_ref[16];    \
    BYTE            sb_auth_code[6];   \
    WORD            w_rspcode;         \
    BYTE            b_PinVerified;     \
    BYTE            sb_rrn[12];        \
    WORD            w_host_idx;        \
    WORD            w_issuer_idx;      \
    struct DATETIME s_dtg;             \
    struct ICC_DATA s_icc_data;        \

//-----------------------------------------------------------------------------
//      Transaction ENTRY mode
//-----------------------------------------------------------------------------
#define MANUAL            'M'
#define SWIPE             'S'
#define ICC               'I'
#define MANUAL_4DBC       'm'  // manual with 4DBC
#define SWIPE_4DBC        's'  // swipe with 4DBC
#define FALLBACK          'F'  // Swipe during ICC not available
#define FALLBACK_4DBC     'f'  // Swipe during ICC with 4DBC
#define CONTACTLESS       'T'  // contactless

//-----------------------------------------------------------------------------
//      Common element for struct
//-----------------------------------------------------------------------------
struct ICC_DATA {
  BYTE sb_label[16];
  BYTE b_aid_len;
  BYTE sb_aid[7];
  BYTE sb_tag_95[5];
  BYTE sb_tag_9b[2];
  BYTE sb_tag_9f26[8];
  WORD w_misc_len;
  BYTE sb_misc_content[330];
};

struct TXN_RECORD {       // Transaction record
  COMMON_DATA
  WORD                  w_crc;
};

struct ACQUIRER_REV {     // Reversal data
  COMMON_DATA
};

struct PRN_DATA {         // Print data
  COMMON_DATA
  BYTE                  b_type;
  DDWORD                dd_base_amount;
};

struct TOTAL {            // Total
  WORD                  w_count;
  DDWORD                dd_amount;
};

struct TOTAL_STRUCT {     // Transaction totals
  DWORD                 d_adjust_count;
  struct TOTAL          s_sale_tot;
  struct TOTAL          s_tips_tot;
  struct TOTAL          s_refund_tot;
  struct TOTAL          s_void_sale;
  struct TOTAL          s_void_refund;
  struct TOTAL          s_ALL;
};



struct STIS_DATA {        // STIS data
  struct TERM_CFG       s_term_cfg;
  struct DESC_TBL       s_desc_tbl[16];  // !TODO
  struct ACQUIRER_TBL   s_acquirer_tbl[2];
  struct ISSUER_TBL     s_issuer_tbl[2];
  struct CARD_TBL       s_card_tbl[2];
};

struct BS_DATE {          // Business date
  BYTE                  b_mm;
  BYTE                  b_dd;
};

struct CTL_DATA {
  BYTE                  b_enable;
};

//-----------------------------------------------------------------------------
//      Global Data Structure
//-----------------------------------------------------------------------------
#define MSG_BUF_LEN        3*1024
struct MSG_BUF {
  DWORD d_len;
  BYTE sb_content[MSG_BUF_LEN];
};

#define MAX_TAG_LIST   150
struct GDS {
  // Add Global Variables here
  struct STIS_DATA    s_stis_data;
  struct TERM_DATA    s_term_data;
  //struct SELECTMENU   select_menu[NBR_ACQUIRER+1];
  struct TXN_RECORD   s_record_buf;
  struct ACQUIRER_REV s_acq_rev;
  struct PRN_DATA     s_p_data;
  struct TOTAL_STRUCT s_term_tot;
  struct TOTAL_STRUCT t_term_tot;
  struct MSG_BUF      s_MsgBuf;
  struct COMMBUF      s_TxBuf;
  struct COMMBUF      s_RxMsg;
  BYTE   sb_IOBuf[1500];
  sIO_t  s_EMVIn;
  sIO_t  s_EMVOut;
  CL_IO  s_CTLIn;
  CL_IO  s_CTLOut;
  TLIST  s_TagList[MAX_TAG_LIST];
  BOOLEAN b_disp_chgd;
  struct BS_DATE s_bs_date;
  struct ECRDATA s_ecr_data;
  int    i_ecr_len;
};
extern struct GDS *gGDS;

#define STIS_TERM_CFG     gGDS->s_stis_data.s_term_cfg
#define STIS_ACQ_TBL(x)   gGDS->s_stis_data.s_acquirer_tbl[x]
#define STIS_ISS_TBL(x)   gGDS->s_stis_data.s_issuer_tbl[x]
#define STIS_CARD_TBL(x)  gGDS->s_stis_data.s_card_tbl[x]
#define STIS_TERM_DATA    gGDS->s_term_data

#define RECORD_BUF        gGDS->s_record_buf
#define P_DATA            gGDS->s_p_data
#define TERM_TOT          gGDS->s_term_tot
#define TTERM_TOT         gGDS->t_term_tot
#define RX_BUF            gGDS->s_RxMsg
#define TX_BUF            gGDS->s_TxBuf
#define MSG_BUF           gGDS->s_MsgBuf

//-----------------------------------------------------------------------------
//      Protected RAM area structure
//-----------------------------------------------------------------------------
struct APPDATA {
  BYTE                  auto_auth_code[3];
  WORD                  reversal_count;
  struct CTL_DATA       s_ctl_data;
  struct PPPSREG        s_ppp_cfg;
};
extern struct APPDATA gAppDat;

//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
extern void DataFileInit(void);
extern void DataFileUpdate(void);
extern void DataFileClose(void);
extern BOOLEAN CorrectHost(BYTE aHostType);
extern void GenAuthCode(BYTE *aAuthCode);
extern void SetDefault(void);

extern void MsgBufSetup(void);
extern void PackMsgBufLen(void);
extern void TxBufSetup(BOOLEAN aAdd2ByteLen);
extern void PackTxBufLen(BOOLEAN aAdd2ByteLen);
extern void RxBufSetup(BOOLEAN aAdd2ByteLen);
extern void PackRxBufLen(BOOLEAN aAdd2ByteLen);

extern int SearchRecord(BOOLEAN aIncVoidTxn);
extern BOOLEAN ValidRecordCRC(WORD aIdx);
extern void RefreshDispAfter(DWORD aSec);

extern void FreeGDS(void);
extern BOOLEAN MallocGDS(void);
extern BOOLEAN TrainingModeON(void);
extern void IncAPMTraceNo(void);
extern void IncAPMTraceNo(void);
extern void CTLEnable(BOOLEAN aEnable);
extern BOOLEAN CTLEnabled(void);

extern void PPPCfgUpdate(struct PPPSREG *aPppCfg);

#endif // _COREVAR_H_

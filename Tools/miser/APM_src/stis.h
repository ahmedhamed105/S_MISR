//-----------------------------------------------------------------------------
//  File          : stis.h
//  Module        :
//  Description   : Declrartion & Defination for stis.c
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
#ifndef _INC_STIS_H_
#define _INC_STIS_H_
#include "common.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      STIS Data  Define
//-----------------------------------------------------------------------------
//// Items move to manager.h
//struct TERM_CFG {
//  BYTE dll;
//  BYTE init_control;
//  BYTE rsvd0;
//  BYTE rsvd1;
//  BYTE date_time[6];
//  BYTE dial_option;
//  BYTE password[2];
//  BYTE help_tel[12];
//  BYTE options[4];
//  BYTE name_loc[46];
//  BYTE dflt_name[23];
//  BYTE currency;
//  BYTE trans_amount_len;
//  BYTE decimal_pos;
//  BYTE rsvd2;
//  BYTE settle_amount_len;
//  BYTE currency_name[3];
//  BYTE local_option;
//  BYTE additional_prompt[20];
//  BYTE reserved[30];
//  WORD crc;
//};
//
//struct CARD_TBL {
//  BYTE pan_range_low[5];
//  BYTE pan_range_high[5];
//  BYTE issuer_id;
//  BYTE acquirer_id;
//  BYTE pan_len;
//  BYTE reserved[20];
//  WORD crc;
//};
//
//struct ISSUER_TBL {
//  BYTE id;
//  BYTE card_name[10];
//  BYTE ref_tel_no[12];
//  BYTE options[4];
//  BYTE default_acc;
//  BYTE pan_format[2];
//  BYTE floor_limit[2];
//  BYTE reserved[20];
//  WORD crc;
//};
//
//// Pending defines
//#define NO_PENDING      0
//#define REV_PENDING     1
//// status define
//#define NOT_LOADED      0xFF
//#define UP              0
//#define DOWN            1
//
//struct ACQUIRER_TBL {
//  BYTE id;
//  BYTE program[10];
//  BYTE name[10];
//  BYTE pri_trans_tel[12];
//  BYTE pri_trans_conn_time;
//  BYTE pri_trans_redial;
//  BYTE sec_trans_tel[12];
//  BYTE sec_trans_conn_time;
//  BYTE sec_trans_redial;
//  BYTE pri_settle_tel[12];
//  BYTE pri_settle_conn_time;
//  BYTE pri_settle_redial;
//  BYTE sec_settle_tel[12];
//  BYTE sec_settle_conn_time;
//  BYTE sec_settle_redial;
//  BYTE trans_mdm_mode;
//  BYTE options[4];
//  BYTE nii[2];
//  BYTE term_id[8];
//  BYTE acceptor_id[15];
//  BYTE timeout_val;
//  BYTE curr_batch_no[3];
//  BYTE next_batch_no[3];
//  BYTE visa1_term_id[23];
//  BYTE settle_mdm_mode;
//  BYTE reserved[12];
//  BYTE ssl_key_idx;
//  BYTE ip[4];
//  BYTE port[2];
//  BYTE host_type;
//  BYTE reserved1[10];
//  BYTE status;        /* 0 - loaded, OTHER - empty */
//  BYTE pending;
//  WORD crc;
//};
//
//struct DESC_TBL {
//  BYTE key;
//  BYTE host_tx_code[2];
//  BYTE text[20];
//  WORD crc;
//};
//
//struct LOGO_TBL {
//  BYTE flag;
//  BYTE type;
//  BYTE image[384];
//  WORD crc;
//};
//
//struct EXTRA_PARAM {
//  BYTE app_id;
//  BYTE seq_no;
//  BYTE len;
//  BYTE prefix;
//  BYTE content[20];
//  WORD crc;
//};

//-----------------------------------------------------------------------------
//      Function API
//-----------------------------------------------------------------------------

extern void CleanStisTable(void);
extern BOOLEAN UpdTermCfg(struct TERM_CFG *aDat);
extern BOOLEAN UpdCardTbl(DWORD aIdx, struct CARD_TBL *aDat);
extern BOOLEAN UpdIssuerTbl(DWORD aIdx, struct ISSUER_TBL *aDat);
extern BOOLEAN UpdAcqTbl(DWORD aIdx, struct ACQUIRER_TBL *aDat);
extern BOOLEAN UpdDescTbl(DWORD aIdx, struct DESC_TBL *aDat);
extern BOOLEAN UpdLogoTbl(DWORD aIdx, struct LOGO_TBL *aDat);
extern BOOLEAN UpdExtraParam(DWORD aIdx, struct EXTRA_PARAM *aDat);
extern DWORD StisAdd(int aTableId, void *aDat);

extern int GetCardCount(void);
extern int GetIssuerCount(void);
extern int GetAcqCount(void);
extern int GetDescCount(void);
extern int GetLogoCount(void);
extern int GetExParamCount(void);

extern void CloseSTISFiles(void);
extern void CreateSTISFiles(void);
extern void CloseExParamFiles(void);
extern void CreateExParamFiles(void);
extern void STISInit(void);
extern void ExParamInit(void);

extern BOOLEAN GetTermCfg(struct TERM_CFG *aDat);
extern BOOLEAN GetCardTbl(DWORD aIdx, struct CARD_TBL *aDat);
extern BOOLEAN GetIssuerTbl(DWORD aIdx, struct ISSUER_TBL *aDat);
extern BOOLEAN GetAcqTbl(DWORD aIdx, struct ACQUIRER_TBL *aDat);
extern BOOLEAN GetDescTbl(DWORD aIdx, struct DESC_TBL *aDat);
extern BOOLEAN GetLogoTbl(DWORD aIdx, struct LOGO_TBL *aDat);
extern BOOLEAN GetExtraParam(DWORD aIdx, struct EXTRA_PARAM *aDat);

extern BOOLEAN PrintSTISParam(BOOLEAN aDetail, BOOLEAN aToDisplay);

extern BYTE GetPending(int aIdx);
extern void SetPending(int aIdx, BYTE aVal);
extern BOOLEAN GetBatchNo(DWORD aIdx, BYTE *aVal);
extern void SetBatchNo(DWORD aIdx, BYTE *aVal);

extern void SaveSTISDbgData(DWORD aInitMode);
extern int SelectAcquirer(int aInclAll);
extern BOOLEAN ExtraMsgLen(int aAcqIdx);

#endif  // _INC_STIS_H_

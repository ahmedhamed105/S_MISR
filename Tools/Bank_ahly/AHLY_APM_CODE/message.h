//-----------------------------------------------------------------------------
//  File          : message.h
//  Module        :
//  Description   : Declrartion & Defination for message.c
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
#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include "common.h"

//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------
struct RSP_TEXT {
  WORD  w_idx;
  DWORD d_text_id;
};
extern const struct RSP_TEXT KRspText[];

//-----------------------------------------------------------------------------
//   Message Id
//-----------------------------------------------------------------------------
enum {
  // Coremain.c
  EDC_CO_SWIPE_INS         ,
  EDC_CO_SWIPE_INS_TAP     ,
  EDC_CO_REMOVE_ICC        ,
  EDC_CO_PROCESSING        ,

  // Constant.c
  EDC_CN_AUTH              ,
  EDC_CN_SALE              ,
  EDC_CN_REFUND            ,
  EDC_CN_OFFLINE           ,
  EDC_CN_SALE_COMP         ,
  EDC_CN_VOID              ,
  EDC_CN_ADJUST            ,
  EDC_CN_UPLOAD            ,
  EDC_CN_SETTLE            ,
  EDC_CN_REVERSAL          ,
  EDC_CN_TEST_MSG          ,

  // input.c
  EDC_IN_CHK_DIGIT_ERROR   ,
  EDC_IN_USE_ICC           ,
  EDC_IN_SWIPE_CARD        ,
  EDC_IN_CORRECT           ,
  EDC_IN_APP_CODE          ,
  EDC_IN_BASE_AMT          ,
  EDC_IN_ENTER_AMOUNT      ,
  EDC_IN_ENTER_TIPS        ,
  EDC_IN_INV_TIPS          ,
  EDC_IN_TOTAL_PROMPT      ,
  EDC_IN_EXPIRY_DATE       ,
  EDC_IN_INV_EXPIRE        ,
  EDC_IN_CARD_EXPIRED      ,
  EDC_IN_ENTER_DESC        ,
  EDC_IN_4DBC              ,
  EDC_IN_INVALID_CARD      ,
  EDC_IN_TRACE             ,
  EDC_IN_ECRREF            ,

  // Tranutil.c
  EDC_TU_NOT_ALLOW         ,
  EDC_TU_CLOSE_BATCH       ,

  // Adjust.c
  EDC_AJ_TOTAL_PROMPT      ,
  EDC_AJ_NOT_ALLOW         ,
  EDC_AJ_ENTER_NAMT        ,
  EDC_AJ_ENTER_NTIP        ,
  EDC_AJ_BASE_AMT          ,
  EDC_AJ_TIPS_AMT          ,
  EDC_AJ_OVERLIMIT         ,
  EDC_AJ_TXN_VOIDED        ,
  EDC_AJ_CLOSE_BATCH       ,
  EDC_AJ_COMPLETED         ,

  // Void.c
  EDC_VD_VOIDED            ,
  EDC_VD_NOT_ALLOW         ,

  // Func.c
  EDC_FN_START_BATCH       ,
  EDC_FN_END_BATCH         ,
  EDC_FN_CARD_TOTAL        ,
  EDC_FN_CARD_REC          ,
  EDC_FN_CLR_BATCH         ,
  EDC_FN_BATCH_ERASE       ,
  EDC_FN_NO_REV            ,
  EDC_FN_CLR_REV           ,
  EDC_FN_REV_CLEAR         ,
  EDC_FN_DISP_BATCH        ,
  EDC_FN_REPRINT_BATCH     ,

  // Print.c
  EDC_PN_PRINTING          ,
  EDC_PN_MANUALCUT         ,
  EDC_PN_HIGHTEMP          ,
  EDC_PN_NO_PAPER          ,
  EDC_PN_PLS_REPRINT       ,
  EDC_PN_PLS_WAIT          ,
  EDC_PN_LOW_BATT          ,

  // Message.c
  EDC_RSP_COMPLETED        ,
  EDC_RSP_CALL_ISSUER      ,
  EDC_RSP_REFERRAL         ,
  EDC_RSP_INVALID_MERCH    ,
  EDC_RSP_PICKUP_CARD      ,
  EDC_RSP_NOT_HONOUR       ,
  EDC_RSP_APPV_ID          ,
  EDC_RSP_BAD_PROC         ,
  EDC_RSP_BAD_AMOUNT       ,
  EDC_RSP_INVALID_ACC      ,
  EDC_RSP_RETRY            ,
  EDC_RSP_INVALID_TERM     ,
  EDC_RSP_EDIT_ERROR       ,
  EDC_RSP_UNKNOWN_CARD     ,
  EDC_RSP_CALL_LC          ,
  EDC_RSP_CALL_CC          ,
  EDC_RSP_DECLINED         ,
  EDC_RSP_CARD_EXPIRED     ,
  EDC_RSP_PIN_ERROR        ,
  EDC_RSP_INV_SERVICE      ,
  EDC_RSP_CONTACT_ACQ      ,
  EDC_RSP_BAD_PROD         ,
  EDC_RSP_TOT_MISMATCH     ,
  EDC_RSP_ROC_NOT_FOUND    ,
  EDC_RSP_BAD_BATCH_NO     ,
  EDC_RSP_BATCH_NOT_FOUND  ,
  EDC_RSP_CALL_AMEX        ,
  EDC_RSP_BAD_TERMINAL_ID  ,
  EDC_RSP_SYS_NOT_AVAIL    ,
  EDC_RSP_DUP_TRACE        ,
  EDC_RSP_BATCH_XFER       ,
  EDC_RSP_OTHER_REASON     ,
  EDC_RSP_APP_CODE         ,
  EDC_RSP_LOW_BATT         ,
  EDC_RSP_CTL_TOI          ,
  EDC_RSP_CTL_DDA          ,
  EDC_RSP_CTL_OCARD        ,
  EDC_RSP_CTL_REFER_MOBILE ,
  EDC_RSP_TXN_CANCEL       ,
  EDC_RSP_DATA_ERROR       ,
  EDC_RSP_TRY_AGAIN_NC     ,
  EDC_RSP_CALL_IP          ,
  EDC_RSP_CALL_IR          ,
  EDC_RSP_CALL_IS          ,
  EDC_RSP_CALL_IT          ,
  EDC_RSP_TRY_AGAIN_LC     ,
  EDC_RSP_NO_DIALTONE      ,
  EDC_RSP_LINE_OCCUP       ,
  EDC_RSP_SWIPE_CARD       ,
  EDC_RSP_READ_ERROR       ,
  EDC_RSP_CARD_BLOCKED     ,
  EDC_RSP_SWIPT_AE         ,
  EDC_RSP_SYS_ERROR        ,
  EDC_RSP_RSP_TXN_ACP      ,
  EDC_RSP_TRY_AGAING_TO    ,
  EDC_RSP_VERIFY_SIGN      ,
  EDC_RSP_UNSUPPORT_CARD   ,
  EDC_RSP_INVALID_CARD     ,
  EDC_RSP_CALL_IC          ,
  EDC_RSP_NO_REC           ,

  // Settle.c
  EDC_SE_BUS_DATE          ,
  EDC_SE_NO_BATCH          ,
  EDC_SE_SALE_TOTAL        ,
  EDC_SE_REFUND_TOTAL      ,
  EDC_SE_TOTAL_WRONG       ,
  EDC_SE_ENTER_SALE_TOTAL  ,
  EDC_SE_ENTER_REF_TOTAL   ,

  // emvtrans.c
  EDC_EMV_FATAL            ,
  EDC_EMV_PROCESSING       ,
  EDC_EMV_SEL_APP          ,
  EDC_EMV_CANNOT_SEL       ,

  // ctltrans.c
  EDC_CTL_FATAL            ,
  EDC_CTL_PROCESSING       ,
  EDC_CTL_PREPROCESS_ERR   ,
  EDC_CTL_TAP_CARD         ,
  EDC_CTL_AMT_EXCEED       , 
  EDC_CTL_CARD_READ_OK     ,

    // network.c
  EDC_NET_INIT             ,

  EDC_MSG_END              ,
};
//-----------------------------------------------------------------------------
//   Functions
//-----------------------------------------------------------------------------
extern BYTE *GetConstMsg(DWORD aMsgId);

#endif // _MESSAGE_H_


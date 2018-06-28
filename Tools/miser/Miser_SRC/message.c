//-----------------------------------------------------------------------------
//  File          : message.c
//  Module        :
//  Description   : Include Message defination use by Application.
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
#include "message.h"

//-----------------------------------------------------------------------------
//   Generic message struct
//-----------------------------------------------------------------------------
struct CONST_MSG {
  DWORD d_msg_id;
  const char *pb_Msg;
};
//-----------------------------------------------------------------------------
//   Constants  messages
//-----------------------------------------------------------------------------
static const struct CONST_MSG KMessage[] = {
  // coremain.c
  {EDC_CO_SWIPE_INS         ,"SWIPE | INS CARD"},
  {EDC_CO_SWIPE_INS_TAP     ," SWIPE|INS|TAP  "},
  {EDC_CO_REMOVE_ICC        ,"                 PLS REMOVE ICC "},
  {EDC_CO_PROCESSING        ,"                 PROCESSING...  "},

  // Constant.c
  {EDC_CN_AUTH              ,"AUTH            "},
  {EDC_CN_SALE              ,"SALE            "},
  {EDC_CN_REFUND            ,"OFFLINE REFUND  "},
  {EDC_CN_OFFLINE           ,"OFFLINE SALE    "},
  {EDC_CN_SALE_COMP         ,"SALE COMP       "},
  {EDC_CN_VOID              ,"VOID            "},
  {EDC_CN_ADJUST            ,"ADJUST          "},
  {EDC_CN_UPLOAD            ,"UPLOAD          "},
  {EDC_CN_SETTLE            ,"SETTLEMENT      "},
  {EDC_CN_REVERSAL          ,"REVERSAL        "},
  {EDC_CN_TEST_MSG          ,"TEST MSG        "},

  // input.c
  {EDC_IN_CHK_DIGIT_ERROR   ,"                CHECK DIGIT ERR."},
  {EDC_IN_USE_ICC           ,"                 PLS INSERT ICC "},
  {EDC_IN_SWIPE_CARD        ,"                 PLS SWIPE CARD "},
  {EDC_IN_CORRECT           ,"                CORRECT ?       "},
  {EDC_IN_APP_CODE          ,"                APPV CODE       "},
  {EDC_IN_BASE_AMT          ,"                 BASE AMOUNT ?  "},
  {EDC_IN_ENTER_AMOUNT      ,"                  ENTER AMOUNT  "},
  {EDC_IN_ENTER_TIPS        ,"                ENTER TIP AMOUNT"},
  {EDC_IN_INV_TIPS          ,"                INVALID TIPS AMT"},
  {EDC_IN_TOTAL_PROMPT      ,"           TOTAL"},
  {EDC_IN_EXPIRY_DATE       ,"                EXPIRY DATE:MMYY"},
  {EDC_IN_INV_EXPIRE        ,"   EXPIRY DATE      INVALID!    "},
  {EDC_IN_CARD_EXPIRED      ,"                 CARD EXPIRED !!"},
  {EDC_IN_ENTER_DESC        ," SELECT PRODUCT "},
  {EDC_IN_4DBC              ,"                CARD SECURITY NO"},
  {EDC_IN_INVALID_CARD      ,"                  INVALID CARD  "},
  {EDC_IN_TRACE             ,"                RECEIPT NUMBER  "},
  {EDC_IN_ECRREF            ,"                ECR REFERENCE NO"},
  {EDC_IN_CUR_ERROR         ,"               CURRECY ERROR    "},

  // Tranutil.c
  {EDC_TU_NOT_ALLOW         ,"  NOT ALLOWED                   "},
  {EDC_TU_CLOSE_BATCH       ,"PLS SETTLE BATCH                "},

  // Adjust.c
  {EDC_AJ_TOTAL_PROMPT      ,"           TOTAL"},
  {EDC_AJ_NOT_ALLOW         ,"   NOT ALLOWED                  "},
  {EDC_AJ_ENTER_NAMT        ,"      NEW AMOUNT"},
  {EDC_AJ_ENTER_NTIP        ,"        NEW TIP "},
  {EDC_AJ_BASE_AMT          ,"     BASE AMOUNT"},
  {EDC_AJ_TIPS_AMT          ,"      TIP AMOUNT"},
  {EDC_AJ_OVERLIMIT         ,"ADJUST OVR LIMIT                "},
  {EDC_AJ_TXN_VOIDED        ,"                 TRANS. VOIDED  "},
  {EDC_AJ_CLOSE_BATCH       ,"                PLS SETTLE BATCH"},
  {EDC_AJ_COMPLETED         ,"                TRANS. ACCEPTED "},

  // Void.c
  {EDC_VD_VOIDED            ," TRANS. VOIDED                  "},
  {EDC_VD_NOT_ALLOW         ,"   NOT ALLOWED                  "},

  // Func.c
  {EDC_FN_START_BATCH       ,"                 START OF BATCH "},
  {EDC_FN_END_BATCH         ,"                  END OF BATCH  "},
  {EDC_FN_CARD_TOTAL        ," TOTAL  BY CARD "},
  {EDC_FN_CARD_REC          ," BATCH  RECORDS "},
  {EDC_FN_CLR_BATCH         ,"  CLEAR BATCH ? "},
  {EDC_FN_BATCH_ERASE       ,"  BATCH ERASED! "},
  {EDC_FN_NO_REV            ,"                  NO  REVERSAL  "},
  {EDC_FN_CLR_REV           ,"CLEAR REVERSAL? "},
  {EDC_FN_REV_CLEAR         ,"REVERSAL CLEARED"},
  {EDC_FN_DISP_BATCH        ," DISPLAY  BATCH "},
  {EDC_FN_REPRINT_BATCH     ," REPRINT  BATCH "},
  {EDC_FN_PRINT_REPORT      ," CONFIG   REPORT"},

  // Lptutil.c
  {EDC_PN_PRINTING          ,"  PRINTING...                   "},
  {EDC_PN_MANUALCUT         ," PLS TEAR PAPER                 "},
  {EDC_PN_HIGHTEMP          ,"    HIGH TEMP                   "},
  {EDC_PN_NO_PAPER          ,"   NO PAPER                     "},
  {EDC_PN_PLS_REPRINT       ," PLEASE REPRINT                 "},
  {EDC_PN_PLS_WAIT          ,"  PLEASE WAIT                   "},
  {EDC_PN_LOW_BATT          ,"  LOW BATTERY                   "},

 // Message.c
  {EDC_RSP_COMPLETED        ,"                TRANS. ACCEPTED "},
  {EDC_RSP_CALL_ISSUER      ,"                PLS CALL ISSUER "},
  {EDC_RSP_REFERRAL         ,"                REFERRAL        "},
  {EDC_RSP_INVALID_MERCH    ,"                INVALID MERCHANT"},
  {EDC_RSP_PICKUP_CARD      ,"                  PICK UP CARD  "},
  {EDC_RSP_NOT_HONOUR       ,"                 DO NOT HONOUR  "},
  {EDC_RSP_APPV_ID          ,"                APPROVED WITH ID"},
  {EDC_RSP_BAD_PROC         ,"                BAD PROCESS CODE"},
  {EDC_RSP_BAD_AMOUNT       ,"                 INVALID AMOUNT "},
  {EDC_RSP_INVALID_ACC      ,"                INVALID ACCOUNT "},
  {EDC_RSP_RETRY            ,"                RE-ENTER TRANS. "},
  {EDC_RSP_INVALID_TERM     ,"                INVALID TERMINAL"},
  {EDC_RSP_EDIT_ERROR       ,"                   EDIT ERROR   "},
  {EDC_RSP_UNKNOWN_CARD     ,"                  UNKNOWN CARD  "},
  {EDC_RSP_CALL_LC          ,"                PLEASE CALL - LC"},
  {EDC_RSP_CALL_CC          ,"                PLEASE CALL - CC"},
  {EDC_RSP_DECLINED         ,"                    DECLINED    "},
  {EDC_RSP_CARD_EXPIRED     ,"                 CARD EXPIRED !!"},
  {EDC_RSP_PIN_ERROR        ,"                 INCORRECT PIN  "},
  {EDC_RSP_INV_SERVICE      ,"                INVALID SERVICE "},
  {EDC_RSP_CONTACT_ACQ      ,"                CONTACT ACQUIRER"},
  {EDC_RSP_BAD_PROD         ,"                BAD PRODUCT CODE"},
  {EDC_RSP_TOT_MISMATCH     ,"                RECONCILE ERROR "},
  {EDC_RSP_ROC_NOT_FOUND    ,"                TRACE NOT FOUND "},
  {EDC_RSP_BAD_BATCH_NO     ,"                BAD BATCH NUMBER"},
  {EDC_RSP_BATCH_NOT_FOUND  ,"                BATCH NOT FOUND "},
  {EDC_RSP_CALL_AMEX        ,"                APPRV, CALL AMEX"},
  {EDC_RSP_BAD_TERMINAL_ID  ,"                BAD TERMINAL ID "},
  {EDC_RSP_SYS_NOT_AVAIL    ,"                SYSTEM NOT AVAIL"},
  {EDC_RSP_DUP_TRACE        ,"                DUPLICATE TRACE "},
  {EDC_RSP_BATCH_XFER       ,"                 BATCH TRANSFER "},
  {EDC_RSP_OTHER_REASON     ,"                INVALID MESSAGE "},
  {EDC_RSP_APP_CODE         ,"                APPV CODE       "},
  {EDC_RSP_LOW_BATT         ,"LOW BATTERY                     "},
  {EDC_RSP_CTL_TOI          ,"                TRY OTHER INTERF"},
  {EDC_RSP_CTL_DDA          ,"                   DDA FAILED   "},
  {EDC_RSP_CTL_OCARD        ,"                 OVERSEAS CARD  "},
  {EDC_RSP_CTL_REFER_MOBILE ,"                REFER TO MOBILE "},
  {EDC_RSP_TXN_CANCEL       ,"                TRANS. CANCELLED"},
  {EDC_RSP_DATA_ERROR       ,"                   DATA ERROR   "},
  {EDC_RSP_TRY_AGAIN_NC     ,"                PLS TRY AGAIN-NC"},
  {EDC_RSP_CALL_IP          ,"                CALL HELP - IP  "},
  {EDC_RSP_CALL_IR          ,"                CALL HELP - IR  "},
  {EDC_RSP_CALL_IS          ,"                CALL HELP - IS  "},
  {EDC_RSP_CALL_IT          ,"                CALL HELP - IT  "},
  {EDC_RSP_TRY_AGAIN_LC     ,"                PLS TRY AGAIN-LC"},
  {EDC_RSP_NO_DIALTONE      ,"  NO DIAL TONE                  "},
  {EDC_RSP_LINE_OCCUP       ,"                 LINE  OCCUPIED "},
  {EDC_RSP_SWIPE_CARD       ," PLS SWIPE CARD                 "},
  {EDC_RSP_READ_ERROR       ,"                CARD READ ERROR "},
  {EDC_RSP_CARD_BLOCKED     ,"                  CARD BLOCKED  "},
  {EDC_RSP_SWIPT_AE         ,"SWIPE  AMEX CARD                "},
  {EDC_RSP_SYS_ERROR        ,"                  SYSTEM ERROR  "},
  {EDC_RSP_RSP_TXN_ACP      ,"                TRANS. ACCEPTED "},
  {EDC_RSP_TRY_AGAING_TO    ,"PLS TRY AGAIN-TO                "},
  //{EDC_RSP_VERIFY_SIGN      ,"                VERIFY SIGNATURE"},
  {EDC_RSP_VERIFY_SIGN      ,"                TRANS. ACCEPTED "},
  {EDC_RSP_UNSUPPORT_CARD   ,"                UNSUPPORTED CARD"},
  {EDC_RSP_INVALID_CARD     ,"                 INVALID  CARD  "},
  {EDC_RSP_CALL_IC          ,"                CALL HELP - IC  "},
  {EDC_RSP_NO_REC           ,"                NO TRANS. RECORD"},

  // Settle.c
  {EDC_SE_BUS_DATE          ,"                BUS. DATE:  MMDD"},
  {EDC_SE_NO_BATCH          ,"                 NO BATCH TOTAL "},
  {EDC_SE_SALE_TOTAL        ,"            SALE"},
  {EDC_SE_REFUND_TOTAL      ,"          REFUND"},
  {EDC_SE_TOTAL_WRONG       ,"                TOTAL INCORRECT "},
  {EDC_SE_ENTER_SALE_TOTAL  ,"                SALE TOTAL:     "},
  {EDC_SE_ENTER_REF_TOTAL   ,"                REFUND TOTAL:   "},

  // emvtrans.c
  {EDC_EMV_FATAL            ," EMV2 FATAL ERR!PLS CALL SERVICE"},
  {EDC_EMV_PROCESSING       ,"                 PROCESSING...  "},
  {EDC_EMV_SEL_APP          ,"                PLS SELECT?[Y/N]"},
  {EDC_EMV_CANNOT_SEL       ,"CANNOT SEL. APP.                "},

  // ctltrans.c
  {EDC_CTL_FATAL            ,"EMVCL FATAL ERR!PLS CALL SERVICE"},
  {EDC_CTL_PROCESSING       ,"                 PROCESSING...  "},
  {EDC_CTL_PREPROCESS_ERR   ,"                PRE-PROCESS ERR "},
  {EDC_CTL_TAP_CARD         ,"                  PLS TAP CARD  "},
  {EDC_CTL_AMT_EXCEED       ,"                AMT EXCEED LIMIT"},
  {EDC_CTL_CARD_READ_OK     ,"CARD READ OK\n  REMOVE CARD"},

  // network.c
  {EDC_NET_INIT             ,"  NETWORK INIT      PLS WAIT    "},

  {EDC_MSG_END              ,NULL},
} ;

// Response Message
const struct RSP_TEXT KRspText[] = {
  {'0'*256+'0',EDC_RSP_COMPLETED               },
  {'0'*256+'1',EDC_RSP_CALL_ISSUER             },
  {'0'*256+'2',EDC_RSP_REFERRAL                },
  {'0'*256+'3',EDC_RSP_INVALID_MERCH           },
  {'0'*256+'4',EDC_RSP_PICKUP_CARD             },
  {'0'*256+'5',EDC_RSP_NOT_HONOUR              },
  {'0'*256+'8',EDC_RSP_APPV_ID                 },
  {'1'*256+'2',EDC_RSP_BAD_PROC                },
  {'1'*256+'3',EDC_RSP_BAD_AMOUNT              },
  {'1'*256+'4',EDC_RSP_INVALID_ACC             },
  {'1'*256+'9',EDC_RSP_RETRY                   },
  {'2'*256+'5',EDC_RSP_INVALID_TERM            },
  {'3'*256+'0',EDC_RSP_EDIT_ERROR              },
  {'3'*256+'1',EDC_RSP_UNKNOWN_CARD            },
  {'4'*256+'1',EDC_RSP_CALL_LC                 },
  {'4'*256+'3',EDC_RSP_CALL_CC                 },
  {'5'*256+'1',EDC_RSP_DECLINED                },
  {'5'*256+'4',EDC_RSP_CARD_EXPIRED            },
  {'5'*256+'5',EDC_RSP_PIN_ERROR               },
  {'5'*256+'8',EDC_RSP_INV_SERVICE             },
  {'6'*256+'0',EDC_RSP_CONTACT_ACQ             },
  {'7'*256+'6',EDC_RSP_BAD_PROD                },
  {'7'*256+'7',EDC_RSP_TOT_MISMATCH            },
  {'7'*256+'8',EDC_RSP_ROC_NOT_FOUND           },
  {'8'*256+'0',EDC_RSP_BAD_BATCH_NO            },
  {'8'*256+'5',EDC_RSP_BATCH_NOT_FOUND         },
  {'8'*256+'8',EDC_RSP_CALL_AMEX               },
  {'8'*256+'9',EDC_RSP_BAD_TERMINAL_ID         },
  {'9'*256+'1',EDC_RSP_SYS_NOT_AVAIL           },
  {'9'*256+'4',EDC_RSP_DUP_TRACE               },
  {'9'*256+'5',EDC_RSP_BATCH_XFER              },
  {'9'*256+'6',EDC_RSP_OTHER_REASON            },
  {'A'*256+'P',EDC_RSP_APP_CODE                },
  {'B'*256+'L',EDC_RSP_LOW_BATT                },
  {'C'*256+'0',EDC_RSP_CTL_TOI                 },
  {'C'*256+'1',EDC_RSP_CTL_DDA                 },
  {'C'*256+'2',EDC_RSP_CTL_OCARD               },
  {'C'*256+'3',EDC_RSP_CTL_REFER_MOBILE        },
  {'C'*256+'N',EDC_RSP_TXN_CANCEL              },
  {'D'*256+'E',EDC_RSP_DATA_ERROR              },
  {'H'*256+'O',EDC_RSP_TRY_AGAIN_NC            },
  {'I'*256+'P',EDC_RSP_CALL_IP                 },
  {'I'*256+'R',EDC_RSP_CALL_IR                 },
  {'I'*256+'S',EDC_RSP_CALL_IS                 },
  {'I'*256+'T',EDC_RSP_CALL_IT                 },
  {'L'*256+'C',EDC_RSP_TRY_AGAIN_LC            },
  {'L'*256+'N',EDC_RSP_NO_DIALTONE             },
  {'L'*256+'O',EDC_RSP_LINE_OCCUP              },
  {'M'*256+'A',EDC_RSP_SWIPE_CARD              },
  {'N'*256+'C',EDC_RSP_TRY_AGAIN_NC            },
  {'N'*256+'T',EDC_RSP_NO_REC                  },
  {'R'*256+'E',EDC_RSP_READ_ERROR              },
  {'S'*256+'B',EDC_RSP_CARD_BLOCKED            },
  {'S'*256+'C',EDC_RSP_SWIPT_AE                },
  {'S'*256+'E',EDC_RSP_SYS_ERROR               },
  {'T'*256+'A',EDC_RSP_RSP_TXN_ACP             },
  {'T'*256+'O',EDC_RSP_TRY_AGAING_TO           },
  {'V'*256+'S',EDC_RSP_VERIFY_SIGN             },
  {'U'*256+'C',EDC_RSP_UNSUPPORT_CARD          },
  {'*'*256+'*',EDC_RSP_CALL_IC                 },
};
//*****************************************************************************
//  Function        : GetConstMsg
//  Description     : Get Message pointer base on the config.
//  Input           : aMsgId;     // KMessage id;
//  Return          : pointer to KMessage
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE *GetConstMsg(DWORD aMsgId)
{
  return ((BYTE *)KMessage[aMsgId].pb_Msg);
}

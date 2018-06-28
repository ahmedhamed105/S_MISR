//-----------------------------------------------------------------------------
//  File          : emvcl2dll.h
//  Module        : 
//  Description   : header for emv contactless dll
//  Author        : Pody
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g                                         |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |            Char size : Leading c                                         |
// |             Int size : Leading i                                         |
// |            Byte size : Leading b                                         |
// |            Word size : Leading w                                         |
// |           Dword size : Leading d                                         |
// |          DDword size : Leading dd                                        |
// |                Array : Leading a, (ab = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  2008 Apr      Pody        Initial Version.
//-----------------------------------------------------------------------------


#ifndef _EMVCL2DLL_H_
#define _EMVCL2DLL_H_

#include "system.h"

//-----------------------------------------------------------------------------
// Defines - EMVCLDLL_ID also defined in KPackVer.h
//-----------------------------------------------------------------------------
#ifndef EMVCLDLL_ID
  #if (CREON)
    #define EMVCLDLL_ID                     (BYTE)19
  #elif (SR300)
    #define EMVCLDLL_ID                     (BYTE)17
  #elif (SP530)
    #define EMVCLDLL_ID                     (BYTE)0x08
  #else //not specified
    #define EMVCLDLL_ID                     (BYTE)100
  #endif
  #ifdef CREON //CREON
    #define JWAVEDLL_ID                   (BYTE)99 //not support, set dummy
    #define PPASSDLL_ID                   (BYTE)20 //not support, set dummy
    #define PWAVEDLL_ID                   (BYTE)16
    #define XPPAYDLL_ID                   (BYTE)99 //not support, set dummy
  #else //!CREON
    #define JWAVEDLL_ID                   (BYTE)(EMVCLDLL_ID + KTYPE_JWAVE) //+1
    #define PPASSDLL_ID                   (BYTE)(EMVCLDLL_ID + KTYPE_PPASS) //+2
    #define PWAVEDLL_ID                   (BYTE)(EMVCLDLL_ID + KTYPE_PWAVE) //+3
    #define XPPAYDLL_ID                   (BYTE)(EMVCLDLL_ID + KTYPE_XPPAY) //+4
  #endif //CREON
#endif

#if (CREON)
  #define emvcldll_call                   app19_call
#else
  #define emvcldll_call(x1,x2,x3,x4)      lib_app_call(EMVCLDLL_ID, x1, x2, x3, x4)
#endif

// define
#define MAX_CL_CFG                    (BYTE)16  // max emvcl config PC1201 8->16
#define MAX_CL_KEY                    (BYTE)32  // max emvcl keyset
#define CL_IFDSN_LEN                  (BYTE)8   // emvcl IFDSN length
#define MAX_CL_REVRID                 (BYTE)5   // max revocate rid 
#define MAX_CL_REVKEY                 (BYTE)30  // max revocate key/RID

// define kernel legacy type that defined by emv
// - first byte in CLT_KERNEL_INFO
#define KTYPE_ANY             (BYTE)0xFF
#define KTYPE_EMVCL           (BYTE)0x00
#define KTYPE_JWAVE           (BYTE)0x01
#define KTYPE_PPASS           (BYTE)0x02
#define KTYPE_PWAVE           (BYTE)0x03
#define KTYPE_XPPAY           (BYTE)0x04

// define CLT, contactless user tags
#define CLT_START              (WORD)0xDFA0    //  pls ensure this value can be used! user tag start number
#define DBT_START              (WORD)0xDFF0    //  pls ensure this value can be used! user tag start number
enum {
//PC1404 move INTERNAL tag to IT_xxx that defined in emvcldll.h
//DFA0
  _del_CLT_GFLAG = CLT_START,           // 0-INTERNAL: appl global flag
  _del_CLT_RLOC,                        // 1-INTERNAL: return error location
  _del_CLT_RACT,                        // 2-INTERNAL: return action code
  _del_CLT_SETUP_TBUF,                  // 3-INTERNAL: temp buffer used in emvclSetup
  CLT_KERNEL_INFO,                 // 4-APP_CFG: kernel info = [EmvKernelType(1)|KernelDllId(1)] 
  CLT_PP_IN_FLAG,                  // 5-APP_CFG: pre-processing input flag
  CLT_CL_FLOOR_LIMIT,              // 6-APP_CFG: pre-processing terminal contactless floor limit
  CLT_CL_TRAN_LIMIT,               // 7-APP_CFG: pre-processing terminal contactless trans limit
  CLT_CL_CVM_LIMIT,                // 8-APP_CFG: pre-processing terminal cvm required limit
  _del_CLT_PPR,                         // 9-INTERNAL: pre-processing result struct PP_RESULT
  _del_CLT_PPR_ITEMS,                   //PC1403 change to IT_MATCH_PPR_ITEMS and use ABUF  // A-INTERNAL: pre-processing result items: 1st + 2nd +...
  _del_CLT_T61_ITEMS,                   //PC1403 change to IT_MATCH_T61_ITEMS and use ABUF  // B-INTERNAL: matched appl Tag 61 itmes: 1st + 2nd + ..
  _del_CLT_MATCH_ITEMS,                 //PC1403 change to IT_MATCH_ITEMS and use ABUF      // C-INTERNAL: sorted match items, MATCH_ITEM [PPR_ID(1)|T61_ID(1)|PRIO(1)]: 1st + 2nd + ..
  CLT_TRAN_TYPE,                   // D-INPUT: trans type: void, refund, adjust, service sale, cash withdraw, goods sales with cashback, service sale with cashback
  CLT_PAY_SCHEME,                  // E-OUTPUT: payment scheme
  _del_CLT_ODA_DATA,                    // F-INTERNAL: data for read records for ODA usage
//DFB0
  CLT_MAGS_IDR,                    // 0-APP_CFG: paypass mag-stripe indicator (support or not: 0x01/0x00), PC1405 No Use in PaypassV3
  CLT_T1DD,                        // 1-OUTPUT: paypass magstripe track 1 discretionary data from card
  CLT_T2DD,                        // 2-OUTPUT: paypass magstripe track 2 discretionary data from card in ascii format
  CLT_DEF_UDOL,                    // 3-APP_CFG: paypass default terminal UDOL, PC1405 No Use in PaypassV3
  _del_CLT_ODA_METHOD,                  // 4-INTERNAL: ODA method should be used: 0(no need), 1(CDA), 2(DDA), 3(SDA) 
  CLT_TRAN_CVM,                    // 5-OUTPUT: transaction CVM, indicate the outcome CVM selection
  CLT_CFG_HDR,                     // 6-INPUT&INTERNAL: cl config header, same as CL_CFGP struct, but without abTLV
  _del_CLT_GENAC_RCP,                   // 7-INTERNAL: gen AC ref control param, b7-8:00(AAC), 40(TC), 80(ARQC), C0(AAR)
  _del_CLT_GENAC_RESP,                  // 8-INTERNAL: gen AC resp excluding SDAD (tag 9F4B) for CDA use
  _del_CLT_RET_WTAGS,                   // 9-INTERNAL: return wtags list based on payment type 
  CLT_RET_TAGS,                    // A-APP_CFG: return tags list based on customer
  _del_CLT_GPO_PDOL,                    // B-INTERNAL: pdol data when GPO, need for verify CDA
  _del_CLT_GAC1_CDOL,                   // C-INTERNAL: cdol data when GAC1, need for verify CDA
  CLT_REMOVAL_SIGNAL,              // D-APP_CFG: customize the removal signal
  CLT_VFLAG,                       // E-APP_CFG: visa config flag
  CLT_TERM_ENTRY_CAP,              // F-APP_CFG: pwave Terminal Entry Capability: SupVSDC[05], NotVSDC[08]
//DFC0
  CLT_T2_PAN,                      // 0-OUTPUT: track2 PAN
  CLT_T2_EXPDATE,                  // 1-OUTPUT: track2 exp date 
  CLT_PPSE_FCI,                    // 2-OUTPUT: PC1106 provide extra info when PPSE sel appl fail
  CLT_VT95,                        // 3-OUTPUT: PC1106 Visa T95(TVR), provide TVR for Pwave Kernel, support DDA failed only. 
  CLT_PP_TCAP_BLCVM,               // 4-APP_CFG: PC1109 paypass Term Capa(9F33) will be used if below CVM Lmt (optional) 
  CLT_APDU2APP,                    // 5-APP_CFG: PC1201 If exist, call back appl when apdu cmd. 
                                   //   Fmt:[ApId&Num(2)|Class&Ins(2)|Class&Ins(2)...] If Class&Ins=FFFF, return to appl for all ApduCmd. 
  CLT_XRATE_TBL,                   // 6-APP_CFG: PC1201 ExhangeRateTable, Fmt:[CurrCode(2)+CurrCode2(2)+BcdRate(4)|...]
                                   //   BcdRate = DecimalPlace(1digit)+Rate(7digits), eg "\x71\x21\x23\x45"=0.1212345
  CLT_XRET_DATA,                   // 7-OUTPUT: PC1301 ECR13-0005 Mc NFC Coupon, ExtraReturnData = Type(2)|LvFixedData(x)|TlvData(x)
                                   //   XRetData(NfcCouple) = Type(0000,2)| LFixed(03,1)+QStep(1)+QRetCode(1)+QNum(1)| TlvRedeemInfo1(x)+TlvRedeemInfo2(x)+...|
                                   //   where TlvRedeemInfo = QTagCfg + Len + Data[RedType(1)+QDisAmt(3)+QDisPer(3)+TotDisAmt(3)], eg FF00 0A 01 001000 000000 001000
  CLT_BLIST_INFO,                  // 8-APP_CFG: PC1303 ECR13-0002 Cup Blacklist, Fmt=LvFixedData(ChkLoc(1)|PanSize(1)|SeqNumSize(1))|LvFilename
                                   //   where ChkLoc = 0x00 ReadRec; 0x01 Restriction;
  CLT_ABNORMAL_INFO,               // 9-APP_CFG: PC1303 ECR13-0002 Cup AbnomalDeduction, Fmt=LvFixedData(RecNum(1)|MaxSec(2))
  CLT_ALLOW_ETYPE,                 // A-INPUT: PC1303 Allowed EType for this transaction, Fmt=EType1(1)|EType2(1)... 
  _del_CLT_LAST_RESULT,                 // B-INTERNAL: last result for emvclMsg & emvclSetup
  _del_CLT_CUP_TEAR_REC,                // C-INTERNAL: PC1305 CupTearRec(can more then one):LvFData(x)|TlvData(x)
  CLT_MUTUAUTH_INFO,               // D-APP_CFG: PC1307 ECR13-0002 Cup MuthAuth, Fmt=LvFixedData(SamId(1))
  CLT_RNM_RANGE,                   // E-APP_CFG: PC1306 Xppay RNM range, Fmt=BinMaxValue; By default is 60.
  _del_CLT_TRY_AGAIN_INFO,              // F-INTERNAL: PC1307 Xppay try again info(Pan+Amt)
//DFD0
  _del_CLT_ERR_RET_TAGS,                // 0-INTERNAL: PC1307 error return tags list
  CLT_DRL_TBL,                     // 1-APP_CFG: !LC_DRL If exist => DRL support. Dynamic Reader Limit Table.
  CLT_SUB_TYPE,                    // 2-APP_CFG: !2014-01 Sub Type, 0x01 => UISC
  CLT_ERR_EXTRA_INFO,              // 3-OUTPUT: PC1403 ECR14-0029 extra info for error, Fmt=ApduCmd(5)|Sw12(2)
  CLT_TXNTYPE_XPARAM,              // 4-APP_CFG: PC1403 TxnType ExtraParam: TlvTxnTypeExtraParam1(x)|TlvTxnTypeExtraParam2(x)|..
                                   //            where TlvTxnTypeExtraParam = Tag(FF|ValueOfTag9C) Len(x) VPtr(TlvDatas for such txn type, ie replace original data in ACfg); 
                                   //                  If Len=0, means TxnType is not allowed. 
  CLT_DEL_TAGLIST,                 // 5-INPUT: PC1404 delete tag list: TlvList for deletion.
  CLT_PP_CALLBACK,                 // 6-APP_CFG: PC1402 PpassCallback:SignalOut(4)|SignalMsg(4)|SignalDe(4)|SignalApdu(4) where (4)=ApplId(2)|FuncNum(2)
  CLT_END,
//======[Debug Tags]=========
//DFF0
  DBT_RESULT = DBT_START,          // 0-DebugModeOnly: debug output the result information
  //DBT_CARDTIME,                    // 1-DebugModeOnly: debug output the card required time
  DBT_PERFMODE,                    // LC !201406 1-Use as Performance Mode config, 0x01:enable, 0x02:Enh 1, 0x04: Enh2
  DBT_APDU_DBCOM,                  // 2-debug apdu command out to port, eg 0x01 or 0x02 for COM1 or COM2
  DBT_LOG_DBCOM,                   // 3-debug log to port, if 0xff, then save log for CLCMD_GET_TAGS(DBT_LOG) 
  DBT_LOG,                         // 4-debug log tag, can use CLCMD_GET_TAGS to get debug log info
  DBT_9F37,                        // 5-debug 9F37 for auto-run
  DBT_END,
};

// define preprocess bitwide input flag in tag CLT_PP_IN_FLAG
enum {
  PPF_STATUS_CHECK_SUP = 1,       // 80 status check support flag
  PPF_ZERO_AMT_CHECK_SUP,         // 40 zero amount check support flag
  PPF_CL_TRAN_LIMIT_NA,           // 20 tran limit disable
  PPF_CL_CVM_LIMIT_NA,            // 10 cvm limit disable
  PPF_CL_FLOOR_LIMIT_NA,          // 08 floor limit disable
  PPF_ZERO_AMT_OPTION2,           // 04 use zero amount option2 (terminated)
  PPF_ZERO_AMT_AUTH_NOT_SUP,      // 02 zero amount authorized support flag - added for visa pwave CLQ.K.001.02 case 7 and case 7a - eric 09-02-2015
  PPF_END
};

//CLT_PAY_SCHEME values - assume mask 0xF0 to different payment associations 
#define V_MSI                   (BYTE)0x10    // visa MSI  - not supported by kernel
#define V_DDA                   (BYTE)0x11    // visa DDA+MSI or Fast DDA - not supported by kernel
#define V_DCVV                  (BYTE)0x13    // visa dCVV - not supported by kernel
#define V_FVSDC                 (BYTE)0x14    // visa full VSDC - not supported by kernel
#define V_VLP                   (BYTE)0x15    // visa fast DDA + VLP  - not supported by kernel
#define V_WAVE2                 (BYTE)0x16    // visa wave 2
#define V_WAVE3                 (BYTE)0x17    // visa wave 3 (qVSDC)
#define V_MSD                   (BYTE)0x18    // visa MSD
#define M_MAGS                  (BYTE)0x20    // mastercard magstripe
#define M_MCHIP                 (BYTE)0x21    // mastercard mchip
#define M_MXI                   (BYTE)0x22    // mastercard MXI
#define J_WAVE1                 (BYTE)0x60    // jcb wave 1 - appl need to map from VSDC
#define J_WAVE2                 (BYTE)0x61    // jcb wave 2 - appl need to map from VSDC
#define J_WAVE3                 (BYTE)0x62    // jcb wave 3 - appl need to map from VSDC
#define P_PBOC                  (BTYE)0x90    // pboc - PC1106 not supported by kernel
#define P_QPBOC                 (BYTE)0x91    // qPBOC - PC1106 appl need to map from VSDC
#define P_MSD                   (BYTE)0x92    // MSD - PC1106 appl need to map from VSDC
#define P_UPCARD                (BYTE)0x93    // upcard - PC1106 not supported by kernel
#define A_MAGS                  (BYTE)0x40    // PC1306 expresspay - magstripe mode
#define A_EMV_FULL              (BYTE)0x41    // PC1306 expresspay - emv full mode, not yet support
#define A_EMV_PARTIAL           (BYTE)0x42    // PC1306 expresspay - emv partial mode
#define A_XPM_MAGS              (BYTE)0x43    // PC1306 expresspay - mobile magstripe mode
#define A_XPM_EMV_FULL          (BYTE)0x44    // PC1306 expresspay - mobile emv full mode, not yet support
#define A_XPM_EMV_PARTIAL       (BYTE)0x45    // PC1306 expresspay - mobile emv partial mode

//IT_ODA_METHOD
#define ODA_NONE      (BYTE)0x00
#define ODA_CDA       (BYTE)0x01
#define ODA_DDA       (BYTE)0x02
#define ODA_SDA       (BYTE)0x03

//CLT_TRAN_CVM (bitwise)
enum {
  TRAN_CVM_SIGN = 1, 
  TRAN_CVM_ONL_PIN,
  TRAN_CVM_MOBILE, //PC1307 xppay
  TRAN_CVM_END
};
#define TRAN_NO_CVM         (BYTE)0x00          //PC1311 no bit on means no_cvm
#define TRAN_CVM_CCV        TRAN_CVM_MOBILE     //PC1311 confirm code verified

//CLT_TRAN_TYPE
#define TRAN_DEFAULT              (BYTE)0x00
#define TRAN_VOID                 (BYTE)0x01
#define TRAN_REFUND               (BYTE)0x02
#define TRAN_ADJUST               (BYTE)0x03
#define TRAN_SALE                 (BYTE)0x04
#define TRAN_CASH                 (BYTE)0x05
#define TRAN_GOODS_CASH           (BYTE)0x06
#define TRAN_SERVICE_CASH         (BYTE)0x07
#define TRAN_AUTH                 (BYTE)0x08

// CLT_VFLAG (bitwise)
enum {
  VFLAG_WAVE2_SUP = 1,  //80 support wave2
  VFLAG_CVN17_NA,       //40 CVN17 not allow
  VFLAG_DUPLICATE_ENB,  //20 !qPBOC Allow Duplicate Tag data from card
  VFLAG_END,
};

// EMV Process Error Loc
#define CLTRC_APPFIND             (BYTE)0x01    // application search 
#define CLTRC_APPSEL              (BYTE)0x02    // application select 
#define CLTRC_CONFIG              (BYTE)0x03    // configuration 
#define CLTRC_IAP                 (BYTE)0x04    // initiate appl processing
#define CLTRC_RDAPPL              (BYTE)0x05    // read appl data 
#define CLTRC_ODA                 (BYTE)0x06    // offline data authentication 
#define CLTRC_SDA                 (BYTE)0x07    // static data authentication 
#define CLTRC_DDA                 (BYTE)0x08    // dynamic data authentication 
#define CLTRC_RESTRIC             (BYTE)0x09    // processing restrictions 
#define CLTRC_CVM                 (BYTE)0x0A    // cardholder verification 
#define CLTRC_RISK                (BYTE)0x0B    // terminal risk management
#define CLTRC_ACTION              (BYTE)0x0C    // action analysis 
#define CLTRC_ONLINE              (BYTE)0x0D    // online processing 
#define CLTRC_COMPLETE            (BYTE)0x0E    // completion processing 

// EMV Process Error Code
#define CLERR_CONFIG              (BYTE)0x00
#define CLERR_MEMORY              (BYTE)0x01
#define CLERR_SMCIO               (BYTE)0x02
#define CLERR_DATA                (BYTE)0x03
#define CLERR_CANCEL              (BYTE)0x04
#define CLERR_NOAPPL              (BYTE)0x05    // no matching application
#define CLERR_NOMORE              (BYTE)0x06    // no more application
#define CLERR_BLOCKED             (BYTE)0x07    // card blocked
#define CLERR_CARDL1              (BYTE)0x08    // level 1 card 
#define CLERR_RESTART             (BYTE)0x09    // card restart failure
#define CLERR_SEQ                 (BYTE)0x10    // command sequence
#define CLERR_SETUP               (BYTE)0x11    // parameter not loaded
#define CLERR_KEY                 (BYTE)0x12    // key error
#define CLERR_NOLOG               (BYTE)0x13    // no transaction log on card
#define CLERR_NOTMATCH            (BYTE)0x14    
//contactless error
#define CLERR_TOI                 (BYTE)0x40    // try other interface
#define CLERR_DDA                 (BYTE)0x41    // DDA Auth Failure
#define CLERR_PDCL                (BYTE)0x42    // Power Down CL
#define CLERR_OCARD               (BYTE)0x43    // overses card
#define CLERR_WTXTO               (BYTE)0x44    // PC1008 Wtx Timeout
#define CLERR_CARDEXP             (BYTE)0x45    // !qPBOC Card Expired
#define CLERR_PP_OVER_LMT         (BYTE)0x46    // PC1106 preprocess all over limit
#define CLERR_SCH_NOTMATCH        (BYTE)0x47    // !clPBOC
#define CLERR_AP_CALL             (BYTE)0x48    // PC1201 appl call return error
#define CLERR_PROPMATCH           (BYTE)0x49    // PC1301 ECR13-0005 proprietary match error
#define CLERR_BLACKLIST           (BYTE)0x4A    // PC1303 ECR13-0002 Blacklist
#define CLERR_TEARING             (BYTE)0x4B    // PC1305 ECR13-0002 tearing
#define CLERR_TRY_AGAIN						(BYTE)0x4C		// PC1307 Xppay
#define CLERR_DECLINE			(BYTE)0x4D		// QPBOC, CLQF001.00, Harris 20140805.

// Action Code
#define CLACT_SIGNATURE           (BYTE)0x01    // signature
#define CLACT_DDAAC               (BYTE)0x02    // Combine DDA/AC
#define CLACT_2AC                 (BYTE)0x04    // Second AC
#define CLACT_DEFCVM              (BYTE)0x08    // Default CVM 
#define CLACT_ONLINE              (BYTE)0x10    // Online processing
#define CLACT_REFERRAL            (BYTE)0x20    // Referral
#define CLACT_HOSTAPP             (BYTE)0x40    // Host Approved
#define CLACT_APPROVED            (BYTE)0x80    // Trans Approved 

// struct define
typedef struct {
  WORD  wLen;
  BYTE  *pbMsg;
} CL_IO; //same as sIO_t in emvdll

// define CL_CFGP.bEType
#define ETYPE_ANY             (BYTE)0xFF //PC1309 internal use only 
#define ETYPE_EMV             	(BYTE)0x01
#define ETYPE_VSDC            	(BYTE)0x02
#define ETYPE_MCHIP           	(BYTE)0x03
#define ETYPE_JSMART          	(BYTE)0x04
#define ETYPE_PBOC            	(BYTE)0x05
#define ETYPE_AMEX            	(BYTE)0x06 
#define ETYPE_DZIP            	(BYTE)0x07
#define ETYPE_XDATA           	(BYTE)0x10	//PC1301 for extra data
#define ETYPE_PROPMATCH        	(BYTE)0x11	//Propiretary Match, PC1301 ECR13-0005 McNfcCouple
#define ETYPE_CL_BIT         	(BYTE)0x80
#define ETYPE_EMVCL           	(BYTE)(ETYPE_CL_BIT|ETYPE_EMV)
#define ETYPE_PWAVE           	(BYTE)(ETYPE_CL_BIT|ETYPE_VSDC)
#define ETYPE_PPASS           	(BYTE)(ETYPE_CL_BIT|ETYPE_MCHIP)
#define ETYPE_JWAVE           	(BYTE)(ETYPE_CL_BIT|ETYPE_JSMART)
#define ETYPE_QPBOC           	(BYTE)(ETYPE_CL_BIT|ETYPE_PBOC)
#define ETYPE_XPPAY           	(BYTE)(ETYPE_CL_BIT|ETYPE_AMEX)
#define ETYPE_DZPAY           	(BYTE)(ETYPE_CL_BIT|ETYPE_DZIP)
#define ETYPE_CL_XDATA      		(BYTE)(ETYPE_CL_BIT|ETYPE_XDATA) //PC1301
#define ETYPE_CL_PROPMATCH      (BYTE)(ETYPE_CL_BIT|ETYPE_PROPMATCH) //PC1301 ECR13-0005 McNfcCouple 

//PC1303 expected TermAID(Tag 9F06) format for ETYPE_XDATA & ETYPE_PROPMATCH
//= HDR_9F06(2) | EType(1) | AID(Max13) 
enum {
  HDR_9F06_NONE = 0xFF00,   //00:Dont care
  HDR_9F06_MCVAS,           //01:MCVAS eCoupon
  //HDR_9F06_CUP_PKEY,        //no more supported! 02:CUP Send Record PublicKey, L1(1)..L6(1) V1(L1)..V6(L6) where V2 TxnDate is dummy value
  //add here
};

// define CL_CFGP.bBitField
#define BIT_PARTIAL_NA            (BYTE)0x01  //partial not allow
#define BIT_REFERRAL_NA           (BYTE)0x02  //referral not allow

#define CLCFG_HDR_SIZE            (WORD)23    //PC1109 size from bEtype to abTACDefault
#define CLCFG_TLV_LEN             (WORD)200
#define CLCFGP_TLV_LEN            (WORD)512
#pragma pack(1)
typedef struct {
  BYTE  bEType;
  BYTE  bBitField;
  DWORD dRSBThresh;       // Threshold for Biased Random Sel
  BYTE  bRSTarget;        // Target for Random Sel
  BYTE  bRSBMax;          // Max target for Biased Random Sel
  BYTE  abTACDenial[5];
  BYTE  abTACOnline[5];
  BYTE  abTACDefault[5];
  BYTE  abTLV[CLCFG_TLV_LEN];
} CL_CFG; //same as APP_CFG in emvdll
typedef struct {
  BYTE  bEType;
  BYTE  bBitField;
  DWORD dRSBThresh;       // Threshold for Biased Random Sel
  BYTE  bRSTarget;        // Target for Random Sel
  BYTE  bRSBMax;          // Max target for Biased Random Sel
  BYTE  abTACDenial[5];
  BYTE  abTACOnline[5];
  BYTE  abTACDefault[5];
  BYTE  abTLV[CLCFGP_TLV_LEN];
} CL_CFGP; //PC1109 increase TLV data buffer size
#pragma pack()

typedef struct {
  DWORD dExpo;
  DWORD dKeySize;
  BYTE  abKey[256];
} CL_RSA_KEY; //same as RSA_KEY in emvdll

typedef struct {
  BYTE  bKeyIdx;
  BYTE  abRID[5];
  CL_RSA_KEY sCAKey;
  WORD  wExpiryMMYY;
  WORD  wEffectMMYY;
  WORD  wChksum;
} CL_KEY_ROOM;

typedef struct {
  BYTE  bKeyIdx;
  BYTE  abRID[5];
  WORD  wExpiryMMYY;
  WORD  wEffectMMYY;
} CL_KEY_INFO;

// !qPBOC++
// Key Revocate Return Code
#define CL_CRL_SETUP_OK         0
#define CL_CRL_LENGTH		        1
#define CL_CRL_RIDFULL          2
#define CL_CRL_CRLFULL          3
#define CL_CRL_NOTFOUND         4
#define CL_CRL_EXIST            5

typedef struct {
  BYTE abRID[5];
  BYTE bCertIdx;
  BYTE abCertSN[3];
} CL_REVO_INFO;

typedef struct {
  BYTE bUsed;
  BYTE bCertIdx;
  BYTE abCertSN[3];
} CL_REVO_LIST;

typedef struct {
  BYTE abRID[5];
  CL_REVO_LIST sCrl[MAX_CL_REVKEY];
} CL_REVO_TBL;

// !qPBOC--


//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Generic API
//----------------------------------------------------------------------------
enum {
  CLXF_SETUP = 0,
  CLXF_MSG,
  CLXF_OSF_OWNER_EXPORT,  //PC1202 for internal use only
  CLXF_CL_WAITCARD,       //PC1203 for appl to wait cl card
  CLXF_CL_CLOSE,          //PC1203 for appl to close cl
  CLXF_CL_APDUCMD,        //PC1203 for appl to send cl cmd
  CLXF_NUM,
};

//extern BOOLEAN emvclSetup(DWORD bEntry, sIO_t* sio_in, sIO_t* sio_out);
// bEntry define
#define CLCMDX_INT_CALL             (DWORD)0x80000000    //internal setup call
#define CLCMDX_CFG_PLUS             (DWORD)0x40000000    //PC1109 use CL_CFGP instead of CL_CFG for AID read func
#define CLCMD_IFDSN_RW              (BYTE)0x01
#define CLCMD_KEY_LOADING           (BYTE)0x02
#define CLCMD_CLR_AID               (BYTE)0x03
#define CLCMD_LOAD_AID              (BYTE)0x04
#define CLCMD_CLOSE_LOAD            (BYTE)0x05
#define CLCMD_ADD_AID               (BYTE)0x06
#define CLCMD_DEL_AID               (BYTE)0x07
#define CLCMD_READ_AID              (BYTE)0x08
#define CLCMD_READ_AIDIDX           (BYTE)0x09
#define CLCMD_READ_KEY              (BYTE)0x0A
#define CLCMD_CONFIG_STATUS         (BYTE)0x0B
#define CLCMD_DNLOAD_PARAM          (BYTE)0x0C
#define CLCMD_SET_KEY_VERSION       (BYTE)0x0D
#define CLCMD_DEL_MULTI_ACQ         (BYTE)0x0E
#define CLCMD_READ_MULTI_ACQ        (BYTE)0x0F
#define CLCMD_READ_CHKSUM           (BYTE)0x10
#define CLCMD_CLR_CRL               (BYTE)0x11
#define CLCMD_ADD_CRL               (BYTE)0x12
#define CLCMD_DEL_CRL               (BYTE)0x13
#define CLCMD_SET_PINPAD            (BYTE)0x14
#define CLCMD_EXIST_CRL             (BYTE)0x17
//!DUPLICATE WITH emvdll 
//#define IOCMD_SET_INTERFACE         23
//#define IOCMD_PUT_PROPRIETARY_TAGS  24  //Reserved
//#define IOCMD_SET_PINBYPASS         25  // !2011-11-14 ECR11-0069 support Subsequent PIN Bypass
#define CLCMD_SYNC_STATE            (BYTE)0x1A  // !clPBOC
#define CLCMD_GET_VERINFO           (BYTE)0x1B  // !2012-10
#define CLCMD_FILE_ACT              (BYTE)0x1C  //PC1303 File Action, ECR13-0002 CupBlacklist
#define CLCMD_SAM_ACT               (BYTE)0x1D  //PC1306 SAM Action, ECR13-0002 CupMuthAuth 
//#define IOCMD_SET_GETPIN            30  // !2009-01-04  //BenL: 10/2013 CREON Set_GetPIN
#define CLCMD_PKI_ACT               (BYTE)0x1F  // Functions for Read/Write Private/Public key & Cert, Rsa Calc, etc. Harris 20140630
//define for EMVCL ONLY
#define CLCMD_GET_TAGS              (BYTE)0x80
#define CLCMD_EPTYPE_RW             (BYTE)0x81  //PC1109 set or get entrypoint type, use kernel legacy type KTYPE_xx
#define CLCMD_PUT_TAGS              (BYTE)0x82  //PC1201 put tlv parameters
#define CLCMD_CALL_KTYPE            (BYTE)0x83  //PC1403 call kernel setup by KTYPE (PPASS=2, PWAVE=3, XPPAY=4)
  //SIn = KType(1)|Entry(4)|Data(x) 
  #define PPCMD_DEL_ALL_TORN          (DWORD)0x1001 //PC1403 delete all torn record
  #define PPCMD_CLEAN_TORN            (DWORD)0x1002 //PC1404 clean torn record

//start from 0x90 for contact features
//#define IOCMD_SREC_MSR              (BYTE)0x90  //PC1504 SRec MSR
#define CLCMD_LAST_RESULT           (BYTE)0xFF  //PC1303 last result, sOut=Type:CLXF_SETUP/CLXF_MSG(1)|Return:T/F(1)|Entry(4)|DLen(2)|Data(x)
#define emvclSetup(x,y,z)           (BOOLEAN) emvcldll_call(CLXF_SETUP,(DWORD)x,(DWORD)y,(DWORD)z)

//PC1305 For CLCMD_FILE_ACT command, sio_in and sio_out have same format.  
//SIn/SOut = FACT_Code(1) | LvVar1(x)..LvVarN(x) | 0000(2) | WLvVar1(x)..WLvVarN(x) 
//where LvVar=Len(1)+Data(x) and WLvVar=Len(2)+Data(x)
enum {
  FACT_INFO = 1,
    //get file info, SIn=01(1)|LvFilename(x)|0000(2)
    //SOut=01(1)|LvFileSize(x)|0000(2)
  FACT_NEW, //02
    //create new file, SIn=02(1)|LvFilename(x)|0000(2)|WLvFileData(x)
    //SOut=02(1)
  FACT_APPEND, //03
    //append data to file, SIn=03(1)|LvFilename(x)|0000(2)|WLvAppendData(x)
    //SOut=03(1)
  FACT_MERGE_IDXFILE, //04
    //merge index rec file, SIn=04(1)|LvOrgFilename(x)|LvActFilename(x)|LvFinalFilename(x)|LvRecSize(x)|LvKeyWOSetNWLen(0or5)|0000(2)
    //  ActFile = 00(Add)/01(Del)+RecData(x)| 00/01+RecData ...;
    //  LvFinalFilename can be same as LvOrgFilename for replacement
    //SOut=04(1)
  FACT_DEL, //05
    //del file, SIn=05(1)|LvFilename(x)|0000(2)
    //SOut=05(1)
  FACT_RENAME, //06
    //rename file, SIn=06(1)|LvOrgFilename(x)|LvNewFilename(x)|0000(2)
    //SOut=06(1)
  FACT_GET, //07
    //GetRec, SIn=07(1)|LvFilename(x)|LvRecSize(x)|LvRecNum(x)|0000(2)
    //SOut=07(1)|LvRecData(x)|
  FACT_GET_BY_SKEY, //08
    //GetBySortedKey, SIn=08(1)|LvFilename(x)|LvRecSize(x)|LvKOSet(x)|LvKey(x)|0000(2)
    //SOut=08(1)|LvRecNum(1+4)|0000(2)|WLvRecData(x)
};

//PC1307 For CLCMD_SAM_ACT command, sio_in and sio_out have same format.  
//SIn/SOut = SACT_Code(1) | LvVar1(x)..LvVarN(x) | 0000(2) | WLvVar1(x)..WLvVarN(x) 
//where LvVar=Len(1)+Data(x) and WLvVar=Len(2)+Data(x)
enum {
  SACT_ON = 1,
    //sam on, SIn=01(1)|LvSamId(x)|0000(2)
    //SOut=01(1)|LvRet(x)|LvHistory(x)
  SACT_OFF,
    //sam off, SIn=02(1)|LvSamId(x)|0000(2)
    //SOut=02(1)|LvRet(x)
  SACT_APDU,
    //sam apdu, SIn=03(1)|LvSamId(x)|LvApduCmd(x)|0000(2)
    //SOut=03(1)|LvSw12(x)|LvApduOut(x)
  SACT_CUP_INIT, //power up, select AID, auth with SAM
    //sam cup init, SIn=04(1)|LvSamId(x)|LvForceInit(x)|0000(2)
    //SOut=04(1)|LvRet(x)
    //LvRet: 0 inited Ok; 1 AlreadyInited; 
  SACT_CCITT_CRC, //get ccitt crc, can be used to verify your crc
    //get ccitt crc, SIn=05(1)|LvData(x)|0000(2)
    //SOut=05(1)|LvCrc(x)
};


/****************************************************************************************/
/* For CLCMD_PKI_ACT command, sio_in and sio_out have same format.                      */
/* Function : BOOLEAN CmdPKIAct(CL_IO *aSin, CL_IO *aSout)                              */
/* SIn/SOut = PKIACT_Code(1) | ELvVar(x) | 0000(2)                                      */
/*            where ELv = Len(1 or 3)(i.e Extension Lc) + Data(x)                       */
/* By Harris 20140702.                                                                  */
/****************************************************************************************/
// PKIACT_ON, PKIACT_OFF, PKIACT_CUP_INIT functions are dummy for compatible with the
// Old SAM Card functions. i.e. SACT_ON, SACT_OFF, SACT_CUP_INIT.
// If the new Apps support the full function of CmdPKIAct, then these dummy functions can be removed.
enum {
  PKIACT_ON = 1,
    // PKI ON, SIn = PKIACT_ON(1) | 0000(2)
    // SOut = PKIACT_ON(1) | LvRet(5) | LvHistory(2)
  PKIACT_OFF,
    // PKI Off, SIn = PKIACT_OFF(1) | 0000(2)
    // SOut = PKIACT_OFF(1) | LvRet(5)
  PKIACT_APDU,
    // PKI APDU Handler, SIn = PKIACT_APDU(1) | ELvApduCmd(x) | 0000(2)
    // SOut = PKIACT_APDU(1) | LvSw12(3) | ELvApduOut(x)
  PKIACT_CUP_INIT, //power up
    // PKI CUP Init, SIn = PKIACT_CUP_INIT(1) | ELvForceInit(x) | 0000(2)
    // SOut = PKIACT_CUP_INIT(1) | LvRet(5)
    // LvRet: 0 inited Ok; 1 AlreadyInited;
  PKIACT_CCITT_CRC, // Get CCITT CRC, can be used to verify your crc
    // Get CCITT CRC, SIn = PKIACT_CCITT_CRC(1) | ELvData(x) | 0000(2)
    // SOut = PKIACT_CCITT_CRC(1) | LvCrc(3)
};

//extern BOOLEAN emvclMsg(DWORD bEntry, sIO_t * pin, sIO_t * pout);
// bEntry define
#define CLMSGX_AUTO_TO_FINISH       (DWORD)0x80000000    //auto until finish
#define CLMSG_PROC_INIT             (BYTE)0x00
#define CLMSG_PROC_START            (BYTE)0x01
#define CLMSG_ENTER_SEL             (BYTE)0x02
#define CLMSG_ONL_PIN               (BYTE)0x03
#define CLMSG_ONL_RESULT            (BYTE)0x04
#define CLMSG_VOICE_REFER           (BYTE)0x05
#define CLMSG_AMT_IAP               (BYTE)0x06
#define CLMSG_VALID_CARD            (BYTE)0x07
#define CLMSG_ENTER_AMT             (BYTE)0x08
#define CLMSG_RESTART               (BYTE)0x09
//#define IOMSG_LOGFORMAT             11  //PC1306 use in emvdll
//#define IOMSG_LOGENTRY              12  //PC1306 use in emvdll
//#define IOMSG_BALENQ                13  //PC1306 use in emvdll
#define emvclMsg(x,y,z)             (BOOLEAN) emvcldll_call(CLXF_MSG,(DWORD)x,(DWORD)y,(DWORD)z)

// contactless function return
#define EMVCL_CL_OK             (int)0
#define EMVCL_CL_ERR            (int)-1
#define EMVCL_CL_TIMEOUT        (int)-2
#define EMVCL_CL_MORE_CARDS     (int)-3
#define EMVCL_CL_WTX_TIMEOUT    (int)-4
//extern int emvclWaitCard(DWORD aTout10ms, BYTE *aLvParam);
#define emvclWaitCard(x,y)          (int) emvcldll_call(CLXF_CL_WAITCARD,(DWORD)x,(DWORD)y,(DWORD)0)
//extern int emvclClose(DWORD aWaitRemove10ms);
#define emvclClose(x)               (int) emvcldll_call(CLXF_CL_CLOSE,(DWORD)x,(DWORD)0,(DWORD)0)
//extern int emvclApduCmd(BYTE *aInOut, WORD aLen, WORD *aSw12);
#define emvclApduCmd(x,y,z)         (int) emvcldll_call(CLXF_CL_APDUCMD,(DWORD)x,(DWORD)y,(DWORD)z)


//*********************************************************************************************
// ECR13-0005 Define for MCVAS NFC Coupon 
//*********************************************************************************************
//QRedeemption Type
#define QREDTYPE_FREEGIFT         (BYTE)0x00
#define QREDTYPE_DISAMT           (BYTE)0x01
#define QREDTYPE_DISPER           (BYTE)0x02
#define QREDTYPE_BOTH             (BYTE)(QREDTYPE_DISAMT|QREDTYPE_DISPER)

//QProcess step
enum {
  //Coupon - Not executed
  QSTEP_INIT        = 0x00, //0
  QSTEP_SEL_PPASS, //1
  QSTEP_GET_PAN,   //2
  QSTEP_SEL_MCVAS, //3
  QSTEP_GET_STATE, //4
  QSTEP_GET_COUNT, //5
  QSTEP_GET_LEN,   //6
  QSTEP_GET_PPCFG, //7
  QSTEP_READ,      //8
  QSTEP_VERIFY,    //9
  QSTEP_UPDATE,    //A
  QSTEP_SET_PPASS_ONLY, //B
  //Coupon - Unconfirmed 
  QSTEP_SET_DONE    = 0x80,
  //Coupon - Redeemed
  QSTEP_DONE        = 0xC0,
};

//QProcess function return
enum {
  QRET_OK = 0,          //done and ok
  QRET_ERR,             //general error
  QRET_NR,              //no related
  QRET_ERR_INVALID,     //invalid coupon
  QRET_ERR_CFG,         
  QRET_ERR_MEM,
};

//CLT_XRET_DATA = QRetType(2) | LvFixData(x) | TlvRedeemInfo(x)..
//- TlvRedeemInfo = QTagCfg + Len + Data[RedType(1)+QDisAmt(3)+QDisPer(3)+TotDisAmt(3)], eg FF00 0A 01 001000 000000 001000
#define QRET_TYPE           (WORD)0x0000
#pragma pack(1)
typedef struct {
  BYTE bQStep;
  BYTE bQRetCode;
  BYTE bQNum;
  BYTE bCountryCode;
  BYTE abDloadId[7]; //CouponId(3)|DloadId(4)
} QRET_FDATA;
#pragma pack()
#define QRET_FDATA_LEN        (BYTE)0x0B
//*********************************************************************************************

//PC1307 define for RetType of CLT_XRET_DATA
//CLT_XRET_DATA = RetType(2) | LvData(x) | TlvData(x)..
#define XRET_TYPE_QRET              QRET_TYPE //0x0000

//*********************************************************************************************
// Paypass 3 DE - Data Exchange
//*********************************************************************************************
//CLT_PP_CALLBACK = SignalOut(4)|SignalMsg(4)|SignalDe(4)|SignalApdu(4)
// where Signal(4)=ApplId(2)|FuncNum(2) and FFFFFFFF means not support
#define CALLBACK_NA            (WORD)0xFFFF //callback not support
#define OSET_CALLBACK_SOUT            (BYTE)0
#define OSET_CALLBACK_SMSG            (BYTE)4
#define OSET_CALLBACK_SDE             (BYTE)8
#define OSET_CALLBACK_SAPDU           (BYTE)12
//Signal or DE type
#define MAX_SIGNAL_SIZE             (WORD)2048  //max signal buffer size
#define KSIGNAL_ERR_STOP            (int)-100   //return value if STOP signal received
#define KSIGNAL_RX_MASK             (DWORD)0x80000000 //get any receive data
enum {
  KSIGNAL_DEK = 0,
  KSIGNAL_MSG,
  KSIGNAL_OUT,
  KSIGNAL_CAPDU,
  KSIGNAL_RAPDU,
  KSIGNAL_END,
  //response data get
  KSIGNAL_DET = (KSIGNAL_RX_MASK|KSIGNAL_DEK),
};


//-----------------------------------------------------------------------------
#endif //_EMVCL2DLL_H_




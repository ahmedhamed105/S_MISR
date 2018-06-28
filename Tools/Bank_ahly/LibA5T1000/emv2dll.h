/*
************************************
*       Module : emv2dll.h         *
*       Name   : TF                *
*       Date   : 04-10-2004        *
************************************
*  Description   : EMV dll function call by application
*/

#ifndef _EMV2DLL_H_
#define _EMV2DLL_H_

#include "common.h"

//-[build options]-------------------------------------------------------------
//#ifndef CREON     //for release(appl call): define hw type
//  #define CREON         1
//#endif

//--------------------------------------------------------------
// PC1412 EMVDLL_ID is also defined in KPackVer.h
//--------------------------------------------------------------
#ifndef EMVDLL_ID
  #ifdef CREON
    #define EMVDLL_ID                 0x17
  #elif (SR300)
    #define EMVDLL_ID                 0x16
  #elif (SP530)
    #define EMVDLL_ID                 0x07
  #else
    #define EMVDLL_ID                 0x0E
  #endif
#endif

#ifdef CREON // CREON **********************************************************************
  //pinpad func number
  #define PINPAD_FUNC_OPEN		  0
  #define PINPAD_FUNC_FLUSH		  1
  #define PINPAD_FUNC_RXRDY		  2
  #define PINPAD_FUNC_RECV		  3
  #define PINPAD_FUNC_SEND		  4
  #define PINPAD_FUNC_CLOSE		  5
#endif // End CREON ************************************************************************

//-[Export Functions]-------------------------------------------------------------
enum {
  EMV_DLLSETUP                , // 00
  EMV_DLLMSG                  , // 01
  EMV_PACKREC                 , // 02
  EMV_FINDREC                 , // 03
  EMV_ICC_ON                  , // 04
  EMV_ICC_OFF                 , // 05
  EMV_ICC_CMD                 , // 06
  EMV_OSF_OWNER_EXPORT        , // 07 PC1203 for internal use only
  EMV_FUNC_COUNT              , // Must be the last item
};
#ifndef _EMV2ALONE_ //for appl call
  //--------------------------------------------------------------
  #ifdef CREON // CREON **********************************************************************
    //--------------------------------------------------------------
    #define dllSetup(x,y,z)       (BOOLEAN) app23_call(0,(DWORD)x,(DWORD)y,(DWORD)z)
    #define dllMsg(x,y,z)         (BOOLEAN) app23_call(1,(DWORD)x,(DWORD)y,(DWORD)z)
    #define ePackRec(x,y,z)       (BYTE *)  app23_call(2,(DWORD)x,(DWORD)y,(DWORD)z)
    #define eFindRec(x,y,z)       (BYTE *)  app23_call(3,(DWORD)x,(DWORD)y,(DWORD)z)
    //--------------------------------------------------------------
    #define emvdll_call(x1,x2,x3,x4)            app23_call(x1,x2,x3,x4)
  #else // !CREON ****************************************************************************
    #define emvdll_call(x1,x2,x3,x4)            lib_app_call(EMVDLL_ID,x1,x2,x3,x4)
  #endif // End CREON ************************************************************************
  //--------------------------------------------------------------
  //extern BOOLEAN dllSetup(DWORD bEntry, sIO_t* sio_in, sIO_t* sio_out);
  #define EMV_DllSetup(x,y,z)   (BOOLEAN) emvdll_call(EMV_DLLSETUP, (DWORD)x, (DWORD)y, (DWORD)z)
  //extern BOOLEAN dllMsg(DWORD bEntry, sIO_t * pin, sIO_t * pout);
  #define EMV_DllMsg(x,y,z)     (BOOLEAN) emvdll_call(EMV_DLLMSG, (DWORD)x, (DWORD)y, (DWORD)z)
  //extern BYTE * ePackRec(BYTE * pbPtr, BYTE *pout, WORD *pTag);
  #define EMV_PackRec(x,y,z)    (BYTE *) emvdll_call(EMV_PACKREC, (DWORD)x, (DWORD)y, (DWORD)z)
  //extern BYTE * eFindRec(BYTE *pbPtr, WORD wTag, WORD *size)
  #define EMV_FindRec(x,y,z)    (BYTE *) emvdll_call(EMV_FINDREC, (DWORD)x, (DWORD)y, (DWORD)z)
  //extern DWORD dllICCOn(BYTE *aDat);
  #define EMV_ICCOn(x)          (DWORD) emvdll_call(EMV_ICC_ON, (DWORD)x, 0, 0)
  //extern DWORD dllICCOff(BYTE *aDat);
  #define EMV_ICCOff()          (DWORD) emvdll_call(EMV_ICC_OFF, 0, 0, 0)
  //extern int dllICCCmd(BYTE *aDat, WORD aLen, WORD *aSw12);
  #define EMV_ICCCmd(x,y,z)     (int) emvdll_call(EMV_ICC_CMD, (DWORD)x, (DWORD)y, (DWORD)z)
#endif //End _EMV2ALONE_

//*** emv msg and setup struct ***
typedef struct {
  WORD wLen;
  BYTE *bMsg;
} sIO_t;

//*** for emvdll or appl call ***
#ifdef _EMV2ALONE_ //for emvdll call
  extern sIO_t *sIOin, *sIOout;
	#define ERR_CONFIG      0x80
	#define ERR_MEMORY      0x81
	#define ERR_SMCIO       0x82
	#define ERR_DATA        0x83
	#define ERR_CANCEL      0x84
	#define ERR_NOAPPL      0x85    /* no matching application */
	#define ERR_NOMORE      0x86    /* no more application */
	#define ERR_BLOCKED     0x87    /* card blocked */
	#define ERR_CARDL1      0x88    /* level 1 card */
	#define ERR_RESTART     0x89    /* card restart failure 09-JUN-2005 AK */
	#define ERR_SEQ         0x90    /* command sequence */
	#define ERR_SETUP       0x91    /* parameter not loaded */
	#define ERR_KEY         0x92    /* key error */
	#define ERR_NOLOG       0x93    /* no transaction log on card */ // 4-JAN-2006 Ray ++
	#define ERR_NOTMATCH    0x94    //HQ, 19-Sep-2006, 2CL.127.01, 2CL.134.01.01/02/03
  //!clPBOC++
  #define ERR_TOI                 0xC0    // try other interface
  #define ERR_DDA                 0xC1    // DDA Auth Failure
  #define ERR_PDCL                0xC2    // Power Down CL
  #define ERR_OCARD               0xC3    // overses card
  #define ERR_WTXTO               0xC4    // PC1008 Wtx Timeout
  #define ERR_CARDEXP             0xC5    // !qPBOC Card Expired
  #define ERR_PP_OVER_LMT         0xC6    // PC1106 preprocess all over limit
  #define ERR_SCH_NOTMATCH        0xC7    // !clPBOC
	#define ERR_AP_CALL             0xC8    // PC1201 appl call return error
	#define ERR_PROPMATCH           0xC9    // PC1301 ECR13-0005 proprietary match error
	#define ERR_BLACKLIST           0xCA    // PC1303 ECR13-0002 Blacklist
  //!clPBOC--
#else //for appl call
	#define ERR_CONFIG      0x00
	#define ERR_MEMORY      0x01
	#define ERR_SMCIO       0x02
	#define ERR_DATA        0x03
	#define ERR_CANCEL      0x04
	#define ERR_NOAPPL      0x05    /* no matching application */
	#define ERR_NOMORE      0x06    /* no more application */
	#define ERR_BLOCKED     0x07    /* card blocked */
	#define ERR_CARDL1      0x08    /* level 1 card */
	#define ERR_RESTART     0x09    /* card restart failure 09-JUN-2005 AK */
	#define ERR_SEQ         0x10    /* command sequence */
	#define ERR_SETUP       0x11    /* parameter not loaded */
	#define ERR_KEY         0x12    /* key error */
	#define ERR_NOLOG       0x13    /* no transaction log on card */ // 4-JAN-2006 Ray ++
	#define ERR_NOTMATCH    0x14    //HQ, 19-Sep-2006, 2CL.127.01, 2CL.134.01.01/02/03
  //!clPBOC++
  #define ERR_TOI                 0x40    // try other interface
  #define ERR_DDA                 0x41    // DDA Auth Failure
  #define ERR_PDCL                0x42    // Power Down CL
  #define ERR_OCARD               0x43    // overses card
  #define ERR_WTXTO               0x44    // PC1008 Wtx Timeout
  #define ERR_CARDEXP             0x45    // !qPBOC Card Expired
  #define ERR_PP_OVER_LMT         0x46    // PC1106 preprocess all over limit
  #define ERR_SCH_NOTMATCH        0x47    // !clPBOC
	#define ERR_AP_CALL             0x48    // PC1201 appl call return error
	#define ERR_PROPMATCH           0x49    // PC1301 ECR13-0005 proprietary match error
	#define ERR_BLACKLIST           0x4A    // PC1303 ECR13-0002 Blacklist
	#define ERR_TEARING             (BYTE)0x4B    // PC1305 ECR13-0002 tearing
	#define ERR_TRY_AGAIN						(BYTE)0x4C		// PC1307 Xppay
  //!clPBOC--
#define ERR_DECLINE			(BYTE)0x4D		// QPBOC, CLQF001.00, Harris 20140805.
#endif //End _EMV2ALONE_

// 09-JUN-2005 AK
#define MAX_SEL         8               /* max nbr of selection */
#define APPL_TLEN       512             /* Appl config area */// !201403 200=>512
#define MAX_SIO_SIZE_CERT     3000      //PC1310 max sio_out buffer size for cert ver
#define MAX_SIO_SIZE_REL      1500      //PC1310 max sio_out buffer size for rel ver

#if (CREON) // CREON **********************************************************************
  #define MAX_APPL        32              /* max nbr of Appl allowed */
#elif (SR300|SP530) 
  #define MAX_APPL        18              /* max nbr of Appl allowed, PC1501 24->18 */
#else // !CREON ****************************************************************************
  #define MAX_APPL        128              /* max nbr of Appl allowed */
#endif // End CREON ************************************************************************
#define CLIP_TLEN       12000     //PC0905 work area increase from 3000 to 12000 2CC.144.01

typedef unsigned char   TL;
#define TLMax           0xFF            /* 0xFFFE if TL is WORD */

typedef struct {                        /* end indicator = 0 Tag */
  WORD wTag;
  TL   sLen;
  BYTE *pbLoc;
} TLIST;                                /* tag list */

typedef struct {
  DWORD Exponent;
  DWORD KeySize;
  BYTE  Key[256];
} RSA_KEY;

typedef struct {
  BYTE  KeyIdx;
  BYTE  RID[5];
  RSA_KEY CAKey;
  WORD  ExpiryDate;     // in MMYY
  WORD  EffectDate;     // in MMYY
  WORD  Chksum;
} KEY_ROOM;

typedef struct {
  BYTE  KeyIdx;
  BYTE  RID[5];
  WORD  ExpiryDate;     // in MMYY
  WORD  EffectDate;     // in MMYY
} KEY_INFO;

// EMV Application Type
#define TYPE_EMV                1
#define TYPE_VSDC               2
#define TYPE_MCHIP              3
#define TYPE_JSMART             4
#define TYPE_PBOC               5 
//++PC1303 defined in EMVCL
#define TYPE_AMEX            	(BYTE)0x06 
#define TYPE_DZIP            	(BYTE)0x07
#define TYPE_XDATA           	(BYTE)0x10	//PC1301 for extra data
#define TYPE_PROPMATCH        (BYTE)0x11	//Propiretary Match, PC1301 ECR13-0005 McNfcCouple
//--PC1303 defined in EMVCL

// tommy 21-9-2006
#define bit_eASI        	0x01
#define bit_eReferral   	0x02
#define bit_ePinBypass  	0x04
#define bit_eForceTRM   	0x08
#define bit_eTACDenExist	0x10	
#define bit_eTACOnlExist	0x20	
#define bit_eTACDefExist	0x40	
//#define bit_eReserved   	0x70
#define bit_eMultiAcq   	0x80

#define eNO_PARTIAL_AID    bit_eASI
#define eNO_REFERRAL       bit_eReferral
#define eENB_PIN_BYPASS    bit_ePinBypass
#define eFORCE_TRM         bit_eForceTRM
#define eENB_ADVICE        bit_eAdvice
#define eMULTI_ACQ         bit_eMultiAcq

#pragma pack(1)
// 21-3-2007 V02 R1
typedef struct {
  BYTE eType;                           /* 0: not in use; 1:EMV; 2: VSDC */
  BYTE eBitField;
  DWORD eRSBThresh;                     /* Threshold for Biased Random Sel */
  BYTE eRSTarget;                       /* Target for Random Sel */
  BYTE eRSBMax;                         /* Max target for Biased Random Sel */
  BYTE eTACDenial[5];
  BYTE eTACOnline[5];
  BYTE eTACDefault[5];
  BYTE eACFG[APPL_TLEN];
} APP_CFG;
#pragma pack()

//for certification revocation list support
#define BIT_PSE_SUPPORT		0x80
#define BIT_DISP_RANDOM		0x40

#define MAX_RID			4  // BenL: 8/2013 - CL PBOC3.0. Add one more RID. Change from 3 to 4.
#define MAX_CRL_PER_RID		30

#define CRL_SETUP_OK		  0
#define CRL_ERR_LENGTH		1
#define CRL_ERR_RID_FULL	2
#define CRL_ERR_CRL_FULL	3
#define CRL_NOT_FOUND		  4
#define CRL_EXIST		      5

typedef struct {
  BYTE rid[5];
  BYTE cert_idx;
  BYTE cert_sn[3];
} CERTREVOC_LIST;

#define IOCMD_IFDSN_RW              1
#define IOCMD_KEY_LOADING           2
#define IOCMD_CLR_AID               3
#define IOCMD_LOAD_AID              4
#define IOCMD_CLOSE_LOAD            5
#define IOCMD_ADD_AID               6
#define IOCMD_DEL_AID               7
#define IOCMD_READ_AID              8
#define IOCMD_READ_AIDIDX           9
#define IOCMD_READ_KEY              10	//0x0A
#define IOCMD_CONFIG_STATUS         11	//0x0B
#define IOCMD_DNLOAD_PARAM          12	//0x0C
#define IOCMD_SET_KEY_VERSION       13	//0x0D
#define IOCMD_DEL_MULTI_ACQ         14	//0x0E
#define IOCMD_READ_MULTI_ACQ        15	//0x0F
#define IOCMD_READ_CHKSUM           16	//0x10
#define IOCMD_CLR_CRL               17	//0x11
#define IOCMD_ADD_CRL               18	//0x12
#define IOCMD_DEL_CRL               19	//0x13
#define IOCMD_SET_PINPAD            20	//0x14
#define IOCMD_GET_PROPRIETARY_TAGS  21  //0x15 +PC0808 ECR08-0024
#define IOCMD_SET_EC                22  //0x16 !2009-08-31 PBOCV2.0 Small Amount Payment
#define IOCMD_SET_INTERFACE         23	//0x17
//#define IOCMD_PUT_PROPRIETARY_TAGS  24  //0x18 Reserved
#define IOCMD_SET_PINBYPASS         25  //0x19 !2011-11-14 ECR11-0069 support Subsequent PIN Bypass
#define IOCMD_SYNC_STATE            26  //0x1A !clPBOC
#define IOCMD_GET_VERINFO           27  //0x1B !2012-10
//#define CLCMD_FILE_ACT            28   (BYTE)0x1C  //PC1303 File Action, ECR13-0002 CupBlacklist
//#define CLCMD_SAM_ACT             29   (BYTE)0x1D  //PC1306 SAM Action, ECR13-0002 CupMuthAuth 
#define IOCMD_SET_GETPIN            30  // !2009-01-04  //BenL: 10/2013 CREON Set_GetPIN
//#define CLCMD_PKI_ACT             31  //0x1F  // Functions for Read/Write Private/Public key & Cert, Rsa Calc, etc. Harris 20140630

//PC0904 start from 0x80 for alnormal features
#define IOCMD_AUTODATA              0x80  //+PC0904 add debug auto-run           
//Warning: Dont add here as conflict with EMVCL

//start from 0x90 for contact features


#define IOMSG_PROC_INIT             0
#define IOMSG_PROC_START            1
#define IOMSG_ENTER_SEL             2
#define IOMSG_ONL_PIN               3
#define IOMSG_ONL_RESULT            4
#define IOMSG_VOICE_REFER           5
#define IOMSG_AMT_IAP               6
#define IOMSG_VALID_CARD            7
#define IOMSG_ENTER_AMT             8
#define IOMSG_RESTART               9
#define IOMSG_LOGFORMAT             11
#define IOMSG_LOGENTRY              12
#define IOMSG_BALENQ                13

//-----------------------------------------------------------------------------
//   Kernel Support Transaction Types
//-----------------------------------------------------------------------------
#define EGOOD_SALE        0
#define EVOID             1
#define EREFUND           2
#define EADJUST           3
#define ESERVICE_SALE     4
#define ECASH_WDRAW       5
#define EGS_WCASHBACK     6
#define ESS_WCASHBACK     7
#define EAUTH             8
#define EPBOCBALENQ       9
#define EDEPOSIT          10

// Transaction Control  
// ERD_CARDLOG is PBOC function.
#define EFORCE_ONL        0x40
#define EPIN_BYPASS       0x20
#define EREAD_CARDLOG     0x10          

#define TRC_APPFIND     1               /* application search */
#define TRC_APPSEL      2               /* application select */
#define TRC_CONFIG      3               /* configuration */
#define TRC_IAP         4               /* initiate appl processing */
#define TRC_RDAPPL      5               /* read appl data */
#define TRC_ODA         6               /* offline data authentication */
#define TRC_SDA         7               /* static data authentication */
#define TRC_DDA         8               /* dynamic data authentication */
#define TRC_RESTRIC     9               /* processing restrictions */
#define TRC_CVM         10              /* cardholder verification */
#define TRC_RISK        11              /* terminal risk management */
#define TRC_ACTION      12              /* action analysis */
#define TRC_ONLINE      13              /* online processing */
#define TRC_COMPLETE    14              /* completion processing */
#define TRC_APPSCAN     15      				/* Scan PSE & DDF */

// Action Code
#define ACT_SIGNATURE           0x01    /* signature */
#define ACT_DDAAC               0x02    /* Combine DDA/AC */
#define ACT_2AC                 0x04    /* Second AC */
#define ACT_DEFCVM              0x08    /* Default CVM */
#define ACT_ONLINE              0x10    /* Online processing */
#define ACT_REFERRAL            0x20    /* Referral */
#define ACT_HOSTAPP             0x40    /* Host Approved */
#define ACT_APPROVED            0x80    /* Trans Approved */

// Private Tag
#define TagIFLimit              0xDF7F  // IFloorLimit 09-JUN-2005 AK
#define TAG_BITCTRL             0xDF7E  // Bit Control Field !2008-11-20
#define TAG_ECPIN               0xDF7D  // Tag for EC Online PIN required !2009-08-31
#define TAG_ECAMT               0xDF7C  // Tag for PBOC EC Txn Amount
#define TAG_XTYPE               0xDFD2  // Sub Type match wiht contactless CLT_SUB_TYPE 0x01:-UICS
#ifdef GO_CERT_VER
#define PTAG_TXNRES              0xDF01
#endif

// Get Propiretary Tag 
//#define PCID_1STGAC              0xFF01 //Not support
//#define PCID_2NDGAC              0xFF02 //Not support
#define PTAG_SW1SW2              0xFF03  // r:Last APDU SW1SW2 
#define PTAG_PIN_FAIL_COUNT      0xFF04  //ECR11-0002 r:Pin Fail Count(1)
#define PTAG_THRESHOLD_INFO      0xFF05  //ECR11-0002 w:Biased(4)|Target(1)|MaxTarget(1)

// !2011-07_01++
// SET_INTERFACE
#define GCTRL_CONTACT          0x00      
#define GCTRL_CONTACTLESS      0x80      
// !2011-07_01--

#endif // _EMV2DLL_H_

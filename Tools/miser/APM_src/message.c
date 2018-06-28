//-----------------------------------------------------------------------------
//  File          : Message.c
//  Module        :
//  Description   : Include Message defination use by Application.
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
#include "midware.h"
#include "message.h"

// coremain.c
const BYTE KVirginInit[]         = {"    APM INIT.    PLEASE WAIT... "};
const BYTE KSystemInit[]         = {"  SYSTEM INIT.   PLEASE WAIT... "};

// apmconfig.c
const BYTE KIpHdr[]              = {"IP SETUP"};
const BYTE KDHCPHdr[]            = {"DHCP SETUP"};
const BYTE KKbdBacklight[]       = {"Kbd BackLight"};
const BYTE KEnterSave[]          = {"Enter To Save!"};
const BYTE KReboot[]             = {"  Reboot Now ?        (Y/N)     "};
const BYTE KDateTime[]           = {"DATE/TIME"};
const BYTE KUpdated[]            = {"UPDATED"};
const BYTE KGPRSHdr[]            = {"GPRS SETUP"};

// ApmFunc.c
const BYTE KResetAppData[]       = {"RESET APPL. DATA"};
const BYTE KConfirm[]            = {"CONFIRM ?  (Y/N)"};
const BYTE KDataReseted[]        = {"DATA RESET !"};
const BYTE KResetEMVData[]       = {"RESET EMV DATA"};
const BYTE KADataReset[]         = {"Appl. Data Reset PLEASE WAIT... "};
const BYTE KSettleBatch[]        = {"PLS SETTLE BATCHBEFORE CONTINUE."};

// inittrans.c
const BYTE KProcessing[]         = {"                 PROCESSING...  "};

// Comm.c
const BYTE KApn[]                = {"APN"};
const BYTE KUserName[]           = {"User Name"};
const BYTE KPassword[]           = {"User Password"};
const BYTE KDialNum[]            = {"Dial Number"};
const BYTE KCommNoCancel[]       = {"                ** NO  CANCEL **"};
const BYTE KCommConnectWait[]    = {"      CONNECTING      ...       "};
const BYTE KConnected[]          = {"       CONNECTED"};
const BYTE KCommSend[]           = {"    (Tx)        "};
const BYTE KCommRcvd[]           = {"            (Rx)"};
const BYTE KCommWait[]           = {"    (Tx)    (Rx)"};

// apmtest.c
const BYTE KSamTest[]            = {"SAM TEST"};
const BYTE KTestICC[]            = {"IC CARD TEST"};
const BYTE KInsertICC[]          = {"INSERT IC CARD"};
const BYTE KTestMSR[]            = {"TRACK 1,2,3 TEST"};
const BYTE KPlsSwipe[]           = {"PLS SWIPE CARD"};

// lptutil.c
const BYTE KPrintingMsg[]        = {"                  PRINTING...   "};
const BYTE KManualCut[]          = {"                 PLS TEAR PAPER "};
const BYTE KHighTemp[]           = {"                    HIGH TEMP   "};
const BYTE KNoPaper[]            = {"                  NO PAPER     "};

// stis.c
const BYTE KSelAcquirer[]        = {"SELECT ACQUIRER"};

// termdata.c
const BYTE KTermId[]             = {"TERMINAL ID."};
const BYTE KChkSumErr[]          = {"CHECK SUM ERROR"};
const BYTE KInitModeStr[]        = {"INIT MODE"};
const BYTE KInitPriNum[]         = {"INIT PRI PHONE #"};
const BYTE KInitSecNum[]         = {"INIT SEC PHONE #"};
const BYTE KDialModeStr[]        = {"DIAL MODE :"};
const BYTE KTimeDelay[]          = {"ASYNC DELAY TIME"};
const BYTE KInitIPHdr[]          = {"Init IP Setup"};
const BYTE KInitIP[]             = {"Init IP:"};
const BYTE KInitPort[]           = {"Init Port:"};
const BYTE KInitSpeed[]          = {"Speed:"};
const BYTE KInitEnterSave[]      = {"Enter To Save!"};
const BYTE KCancelExit[]         = {"EXIT -- CANCEL"};
const BYTE KPABX[]               = {"PABX ACCESS CODE"};
const BYTE KInitConnTime[]       = {"INIT. CONN. TIME"};
const BYTE KInitNII[]            = {"INIT. NII"};
const BYTE KReturnBaseTO[]       = {"RETURN TO BASE  TIMEOUT - MINUTE"};
const BYTE KDefaultAmt[]         = {"DEFAULT AMOUNT: "};
const BYTE KCurrBatchNo[]        = {"BATCH NO        "};
const BYTE KTrainingMode[]       = {"TRAINING MODE"};
const BYTE KDebugMode[]          = {"DEBUG MODE"};
const BYTE KPrintStis[]          = {"PRINT STIS"};
const BYTE KPPadPort[]           = {"EXT PPAD PORT: "};
const BYTE KEcrPort[]            = {"ECR PORT: "};
const BYTE KDefaultApp[]         = {"DEFAULT APP: "};
const BYTE KCheckInPriNum[]      = {"CHECKIN PHONE #1"};
const BYTE KCheckInSecNum[]      = {"CHECKIN PHONE #2"};
const BYTE KFTPUser[]            = {"FTP USER NAME"};
const BYTE KFTPPwd[]             = {"FTP PASSWORD"};
const BYTE KFTPServerKey[]       = {"FTP SERVER KEY"};
const BYTE KPPPUser[]            = {"PPP USER NAME"};
const BYTE KPPPPwd[]             = {"PPP PASSWORD"};
const BYTE KInitPPPIPHdr[]       = {"PPP Init IP Setu"};
const BYTE KInitPPPIP[]          = {"PPP Init IP:"};
const BYTE KInitPPPPort[]        = {"PPP Init Port:"};
const BYTE KNonWifiModel[]       = {"Non-Wifi Model!"};
const BYTE KHostParamHdr[]       = {"Host Param"};
const BYTE KHostIP[]             = {"IP  :"};
const BYTE KHostPort[]           = {"Port:"};
const BYTE KHostSslKeyIdx[]      = {"SSL KeyIdx:"};
const BYTE KExtraMsgLen[]        = {"2-Byte Len:"};
const BYTE KNonBtModel[]         = {"Non-BT Model!"};
const BYTE KEcrSslMode[]         = {"ECR SSL Mode:"};

// constant.c
const BYTE KComplted[]           = {"                TRANS. ACCEPTED "};
const BYTE KBadAmount[]          = {"                 INVALID AMOUNT "};
const BYTE KTxnNotFound[]        = {"                TRACE NOT FOUND "};
const BYTE KTxnAccepted[]        = {"                TRANS. ACCEPTED "};
const BYTE KLowBattery[]         = {"                  LOW BATTERY   "};
const BYTE KTxnCanceled[]        = {"                TRANS. CANCELLED"};
const BYTE KBadProcCode[]        = {"                BAD PROCESS CODE"};
const BYTE KDataError[]          = {"                   DATA ERROR   "};
const BYTE KECRCommErr[]         = {"                ECR COMM. ERROR "};
const BYTE KInvalidSeq[]         = {"                CALL HELP - IR  "};
const BYTE KInvalidTrace[]       = {"                CALL HELP - IS  "};
const BYTE KInvalidTerm[]        = {"                INVALID TERMINAL"};
const BYTE KHostNotFound[]       = {"                 HOST NOT FOUND "};
const BYTE KSwipeCard[]          = {"                 PLS SWIPE CARD "};
const BYTE KWrongMAC[]           = {"                  MAC MISMATCH  "};
const BYTE KNotAllowed[]         = {"                  NOT ALLOWED   "};
const BYTE KNoTransRec[]         = {"                NO TRANS. RECORD"};
const BYTE KInvalidPWD[]         = {"                INVALID PASSWORD"};
const BYTE KCardReadErr[]        = {"                CARD READ ERROR "};
const BYTE KCardBlocked[]        = {"                  CARD BLOCKED  "};
const BYTE KSysError[]           = {"                  SYSTEM ERROR  "};
const BYTE KUnSupportCard[]      = {"                UNSUPPORTED CARD"};
const BYTE KUpdateRequest[]      = {"                 UPDATE REQUEST "};
const BYTE KInvalidResp[]        = {"                CALL HELP - IC  "};
const BYTE KNoDialTone[]         = {"                  NO DIAL TONE  "};
const BYTE KHostOffline[]        = {"                  HOST OFFLINE  "};
const BYTE KLineBusy[]           = {"                   LINE  BUSY   "};
const BYTE KTryAgainLC[]         = {"                PLS TRY AGAIN-LC"};
const BYTE KLineOccup[]          = {"                 LINE  OCCUPIED "};
const BYTE KNoLine[]             = {"                    NO LINE     "};
const BYTE KTryAgainNC[]         = {"                PLS TRY AGAIN-NC"};
const BYTE KTryAgainTO[]         = {"                PLS TRY AGAIN-TO"};
const BYTE KDnloadBIN[]          = {"        BIN FILE"};
const BYTE KDnloadSchedule[]     = {" REMOTE DOWNLOAD"};

// tranutil.c
const BYTE KEDCInit[]            = {"      INITIALIZE"};
const BYTE KLoadEMVKey[]         = {"LOAD EMV KEY"};
const BYTE KLoadEMVParam[]       = {"LOAD EMV PARAM"};
const BYTE KLoadExtraParam[]     = {"LOAD EXTRA PARAM"};
const BYTE KReturn2Base[]        = {"                RETURN TO BASE "};

// ftptran.c
const BYTE KLogInFailed[]        = {"                  LOGIN FAILED  "};
const BYTE KDownloadFailed[]     = {"                DOWNLOAD  FAILED"};
const BYTE KServerKeyError[]     = {"                 SERVER KEY ERR "};
const BYTE KDLLFileFailed[]      = {"                 DLL DL. FAILED "};
const BYTE KTIDFileFailed[]      = {"                 TID DL. FAILED "};
const BYTE KItemNotEnabled[]     = {"                ITEM NOT ENABLED"};

//remotedl.c
const BYTE KRemoteDLSch[]        = {"DWNLOAD SCHEDULE"};
const BYTE KRemoteDLHeader[]     = {"REMOTE DOWNLOAD "};

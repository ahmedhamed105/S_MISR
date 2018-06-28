//-----------------------------------------------------------------------------
//  File          : message.h
//  Module        :
//  Description   : Include Message defination.
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_MESSAGE_H_
#define _INC_MESSAGE_H_
#include "common.h"


//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
// coremain.c
extern const BYTE KVirginInit[]         ;
extern const BYTE KSystemInit[]         ;

// apmconfig.c
extern const BYTE KIpHdr[]              ;
extern const BYTE KDHCPHdr[]            ;
extern const BYTE KKbdBacklight[]       ;
extern const BYTE KEnterSave[]          ;
extern const BYTE KReboot[]             ;
extern const BYTE KDateTime[]           ;
extern const BYTE KUpdated[]            ;
extern const BYTE KGPRSHdr[]            ;

// ApmFunc.c
extern const BYTE KResetAppData[]       ;
extern const BYTE KConfirm[]            ;
extern const BYTE KDataReseted[]        ;
extern const BYTE KResetEMVData[]       ;
extern const BYTE KADataReset[]         ;
extern const BYTE KSettleBatch[]        ;
 
// inittrans.c
extern const BYTE KProcessing[]         ;

// Comm.c
extern const BYTE KApn[]                ;
extern const BYTE KUserName[]           ;
extern const BYTE KPassword[]           ;
extern const BYTE KDialNum[]            ;
extern const BYTE KCommNoCancel[]       ;
extern const BYTE KCommConnectWait[]    ;
extern const BYTE KConnected[]          ;
extern const BYTE KCommSend[]           ;
extern const BYTE KCommRcvd[]           ;
extern const BYTE KCommWait[]           ;

// apmtest.c
extern const BYTE KSamTest[]            ;
extern const BYTE KTestICC[]            ;
extern const BYTE KInsertICC[]          ;
extern const BYTE KTestMSR[]            ;
extern const BYTE KPlsSwipe[]           ;

// lptutil.c
extern const BYTE KPrintingMsg[]        ;
extern const BYTE KManualCut[]          ;
extern const BYTE KHighTemp[]           ;
extern const BYTE KNoPaper[]            ;

// stis.c
extern const BYTE KSelAcquirer[]        ;

// termdata.c
extern const BYTE KTermId[]             ;
extern const BYTE KChkSumErr[]          ;
extern const BYTE KInitModeStr[]        ;
extern const BYTE KInitPriNum[]         ;
extern const BYTE KInitSecNum[]         ;
extern const BYTE KDialModeStr[]        ;
extern const BYTE KTimeDelay[]          ;
extern const BYTE KInitIPHdr[]          ;
extern const BYTE KInitIP[]             ;
extern const BYTE KInitPort[]           ;
extern const BYTE KInitSpeed[]          ;
extern const BYTE KInitEnterSave[]      ;
extern const BYTE KCancelExit[]         ;
extern const BYTE KPABX[]               ;
extern const BYTE KInitConnTime[]       ;
extern const BYTE KInitNII[]            ;
extern const BYTE KReturnBaseTO[]       ;
extern const BYTE KDefaultAmt[]         ;
extern const BYTE KCurrBatchNo[]        ;
extern const BYTE KTrainingMode[]       ;
extern const BYTE KDebugMode[]          ;
extern const BYTE KPrintStis[]          ;
extern const BYTE KPPadPort[]           ;
extern const BYTE KEcrPort[]            ;
extern const BYTE KDefaultApp[]         ;
extern const BYTE KCheckInPriNum[]      ;
extern const BYTE KCheckInSecNum[]      ;
extern const BYTE KFTPUser[]            ;
extern const BYTE KFTPPwd[]             ;
extern const BYTE KFTPServerKey[]       ;
extern const BYTE KPPPUser[]            ;
extern const BYTE KPPPPwd[]             ;
extern const BYTE KInitPPPIPHdr[]       ;
extern const BYTE KInitPPPIP[]          ;
extern const BYTE KInitPPPPort[]        ;
extern const BYTE KNonWifiModel[]       ;
extern const BYTE KHostParamHdr[]       ;
extern const BYTE KHostIP[]             ;
extern const BYTE KHostPort[]           ;
extern const BYTE KHostSslKeyIdx[]      ;
extern const BYTE KExtraMsgLen[]        ;
extern const BYTE KNonBtModel[]         ;
extern const BYTE KEcrSslMode[]         ;

// constant.c
extern const BYTE KComplted[]           ;
extern const BYTE KBadAmount[]          ;
extern const BYTE KTxnNotFound[]        ;
extern const BYTE KTxnAccepted[]        ;
extern const BYTE KLowBattery[]         ;
extern const BYTE KTxnCanceled[]        ;
extern const BYTE KBadProcCode[]        ;
extern const BYTE KDataError[]          ;
extern const BYTE KECRCommErr[]         ;
extern const BYTE KInvalidSeq[]         ;
extern const BYTE KInvalidTrace[]       ;
extern const BYTE KInvalidTerm[]        ;
extern const BYTE KHostNotFound[]       ;
extern const BYTE KSwipeCard[]          ;
extern const BYTE KWrongMAC[]           ;
extern const BYTE KNotAllowed[]         ;
extern const BYTE KNoTransRec[]         ;
extern const BYTE KInvalidPWD[]         ;
extern const BYTE KCardReadErr[]        ;
extern const BYTE KCardBlocked[]        ;
extern const BYTE KSysError[]           ;
extern const BYTE KUnSupportCard[]      ;
extern const BYTE KUpdateRequest[]      ;
extern const BYTE KInvalidResp[]        ;
extern const BYTE KNoDialTone[]         ;
extern const BYTE KHostOffline[]        ;
extern const BYTE KLineBusy[]           ;
extern const BYTE KTryAgainLC[]         ;
extern const BYTE KLineOccup[]          ;
extern const BYTE KNoLine[]             ;
extern const BYTE KTryAgainNC[]         ;
extern const BYTE KTryAgainTO[]         ;
extern const BYTE KDnloadBIN[]          ;
extern const BYTE KDnloadSchedule[]     ;

// tranutil.c
extern const BYTE KEDCInit[]            ;
extern const BYTE KLoadEMVKey[]         ;
extern const BYTE KLoadEMVParam[]       ;
extern const BYTE KLoadExtraParam[]     ;
extern const BYTE KReturn2Base[]        ;

// ftptran.c
extern const BYTE KLogInFailed[]        ;
extern const BYTE KDownloadFailed[]     ;
extern const BYTE KServerKeyError[]     ;
extern const BYTE KDLLFileFailed[]      ;
extern const BYTE KTIDFileFailed[]      ;
extern const BYTE KItemNotEnabled[]     ;

//remotedl.c
extern const BYTE KRemoteDLSch[]        ;
extern const BYTE KRemoteDLHeader[]     ;

#endif // _INC_MESSAGE_H_

//-----------------------------------------------------------------------------
//  File          : chkoptn.h
//  Module        :
//  Description   : Declrartion & Defination for CHKOPTN.C
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
#ifndef _CHKOPTN_H_
#define _CHKOPTN_H_
#include "common.h"

//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
// terminal related checking
extern BOOLEAN ManualPSWReqd(void);
extern BOOLEAN DispMSRReqd(void);
extern BOOLEAN TIPsReqd(void);
extern BOOLEAN PrintTimeReqd(void);
extern BOOLEAN DDMMBusinessDate(void);
extern BOOLEAN ComfirmTotal(void);
extern BOOLEAN EcrRefReqd(void);
extern BOOLEAN PromptTips(void);
extern BOOLEAN BlockLocalTrans(void);
extern BOOLEAN EMVOfflineAllow(void);
extern BOOLEAN EMVRefundAllow(void);
extern BOOLEAN BlockFaultyICC(void);
extern BYTE NumOfRcptCopy(void);
extern DWORD FallbackTimeout(void);
extern BOOLEAN ReferralDial(void);

// Acquirer related checking
extern BOOLEAN HostSettlePending(void);
extern BOOLEAN RefundOffline(void);
extern BOOLEAN VoidOffline(void);
extern BOOLEAN PiggybackOffline(void);
extern BOOLEAN Prompt4DBC(void);
extern BOOLEAN MultiAdjustAllowed(void);
extern BOOLEAN ExtraMsgLen(void);
extern BOOLEAN FallbackDialup(void);
extern BOOLEAN CheckDuplicate(void);
extern BYTE GetHostType(BYTE aIdx);

// Issuer related checking
extern BOOLEAN UnderFloorLimit(DDWORD aAmount);
extern BOOLEAN ManualEntryAllow(void);
extern BOOLEAN OfflineAllowed(void);
extern BOOLEAN ExpDateReqd(void);
extern BOOLEAN VoiceReferral(WORD aRspCode);
extern BOOLEAN DescriptorReqd(void);
extern BOOLEAN AdjustAllowed(void);
extern BOOLEAN CheckPAN(void);
extern BOOLEAN PrintReqd(void);
extern BOOLEAN DefaultCapture(void);
extern BOOLEAN CheckExpDate(void);
extern BOOLEAN RefundBlocked(void);
extern BOOLEAN AuthBlocked(void);
extern BOOLEAN VoidBlocked(void);
extern BOOLEAN BlockAuthCode(void);
extern BOOLEAN MaskCardNo(void);
extern BOOLEAN ExpiryReject(void);
extern BOOLEAN BlockLocalManual(void);
extern BOOLEAN BlockLocalSwipe(void);
extern BOOLEAN Track1Enable(void);
extern BOOLEAN EMVProcDisable(void);

#endif // _CHKOPTN_H_

//-----------------------------------------------------------------------------
//  File          : chkoptn.c
//  Module        :
//  Description   : Include routines to check Applicaiton Config OPTION.
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
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "corevar.h"
#include "chkoptn.h"

//*****************************************************************************
//  Function        : AdjustPSWReqd
//  Description     : Check whether password requiered for adjust transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AdjustPSWReqd(void)
{
  return (STIS_TERM_CFG.b_local_option & 0x08);
}
//*****************************************************************************
//  Function        : SettlePSWReqd
//  Description     : Check whether password requiered for settlement.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SettlePSWReqd(void)
{
  return (STIS_TERM_CFG.b_local_option & 0x10);
}
//*****************************************************************************
//  Function        : ManualPSWReqd
//  Description     : Check whether password requiered for manual entry.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ManualPSWReqd(void)
{
  return (STIS_TERM_CFG.b_local_option & 0x20);
}
//*****************************************************************************
//  Function        : DispMSRReqd
//  Description     : Check whether config show MSR data.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN DispMSRReqd(void)
{
  return (STIS_TERM_CFG.sb_options[0] & 0x02);
}
//*****************************************************************************
//  Function        : TIPsReqd
//  Description     : Check whether config for TIPS processing.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN TIPsReqd(void)
{
  return (STIS_TERM_CFG.sb_options[0] & 0x04);
}
//*****************************************************************************
//  Function        : PrintTimeReqd
//  Description     : Check whether config for print date/time on receipt.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintTimeReqd(void)
{
  return (STIS_TERM_CFG.sb_options[0] & 0x20);
}
//*****************************************************************************
//  Function        : DDMMBusinessDate
//  Description     : Check whether config to use DDMM business date format.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN DDMMBusinessDate(void)
{
  return (STIS_TERM_CFG.sb_options[0] & 0x40);
}
//*****************************************************************************
//  Function        : ComfirmTotal
//  Description     : Check whether config require user confirm totals.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN ComfirmTotal(void)
{
  return (STIS_TERM_CFG.sb_options[0] & 0x80);
}
//*****************************************************************************
//  Function        : PromptTips
//  Description     : Check whether config for Prompt tips entry.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN EcrRefReqd(void)
{
  return (STIS_TERM_CFG.sb_options[1] & 0x04);
}
//*****************************************************************************
//  Function        : PromptTips
//  Description     : Check whether config for Prompt tips entry.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN PromptTips(void)
{
  return (STIS_TERM_CFG.sb_reserved[1] & 0x02);
}
//*****************************************************************************
//  Function        : BlockLocalTrans
//  Description     : Check for disable local initialize transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : Not allow message is display when TRUE.
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BlockLocalTrans(void)
{
  return(STIS_TERM_CFG.sb_reserved[1] & 0x04);
}
//*****************************************************************************
//  Function        : NumOfRcptCopy
//  Description     : Retrieve the number of receipt copy config.
//  Input           : N/A
//  Return          : num of copy.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE NumOfRcptCopy(void)
{
  return (STIS_TERM_CFG.sb_reserved[4] >> 4);
}
//*****************************************************************************
//  Function        : ReferralDial
//  Description     : Check if auto dial for referral.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ReferralDial(void)
{
    return(STIS_TERM_CFG.b_dial_option & 0x10);
}
//*****************************************************************************
//  Function        : HostSettlePending
//  Description     : Check whether settle settlement is require for
//                    current acquirer;
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : Error message is display when TRUE.
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN HostSettlePending(void)
{
  return (STIS_ACQ_TBL(0).b_pending & SETTLE_PENDING);
}
//*****************************************************************************
//  Function        : RefundOffline
//  Description     : Check whether config REFUND process offline.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN RefundOffline(void)
{
  return (STIS_ACQ_TBL(0).sb_options[0] & 0x10);
}
//*****************************************************************************
//  Function        : VoidOffline
//  Description     : Check whether config VOID process offline.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN VoidOffline(void)
{
  return (STIS_ACQ_TBL(0).sb_options[0] & 0x20);
}
//*****************************************************************************
//  Function        : PiggybackOffline
//  Description     : Check whether config for piggyback offline transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PiggybackOffline(void)
{
  return (STIS_ACQ_TBL(0).sb_options[0] & 0x80);
}
//*****************************************************************************
//  Function        : Prompt4DBC
//  Description     : Check whether config for 4DBC entry.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN Prompt4DBC(void)
{
  return (STIS_ACQ_TBL(0).sb_reserved[0]&0x10);
}
//*****************************************************************************
//  Function        : MultiAdjustAllowed
//  Description     : Check Multiple adjustment allowed.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MultiAdjustAllowed(void)
{
  return (STIS_ACQ_TBL(0).sb_reserved[3] & 0x80);
}
//*****************************************************************************
//  Function        : ExtraMsgLen
//  Description     : Check config for extra message len byte in host msg.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ExtraMsgLen(void)
{
  return (STIS_ACQ_TBL(0).b_reserved1? TRUE: FALSE);
}
//*****************************************************************************
//  Function        : CheckDuplicate
//  Description     : Check Duplicate transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckDuplicate(void)
{
  return (STIS_ACQ_TBL(0).sb_reserved[11]&0x20);
}
//*****************************************************************************
//  Function        : FallbackDialup
//  Description     : Config. for TCP connection fallback dialup.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN FallbackDialup(void)
{
  return (STIS_ACQ_TBL(0).sb_reserved[11]&0x40);
}
//*****************************************************************************
//  Function        : GetHostType
//  Description     : Return Current Host Type.
//  Input           : aIdx;     // Acquirer Idx
//  Return          : host type.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE GetHostType(BYTE aIdx)
{
  return (STIS_ACQ_TBL(aIdx).b_host_type);
}
//*****************************************************************************
//  Function        : UnderFloorLimit
//  Description     : Check input aAmount is under issuer's floor limit.
//  Input           : 64 bit amount.
//  Return          : TRUE/FALSE;
//                    FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UnderFloorLimit(DDWORD aAmount)
{
  DDWORD limit;

  limit = (bcd2val(STIS_ISS_TBL(0).sb_floor_limit[0])*100 + bcd2val(STIS_ISS_TBL(0).sb_floor_limit[1]));
  limit *= 100;
  return (aAmount < limit);
}
//*****************************************************************************
//  Function        : ManualEntryAllow
//  Description     : Check whether config for manual entry.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ManualEntryAllow(void)
{
  return (STIS_ISS_TBL(0).sb_options[0] & 0x04);
}
//*****************************************************************************
//  Function        : OfflineAllowed
//  Description     : Check whether config for OFFLINE entry.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN OfflineAllowed(void)
{
  return (STIS_ISS_TBL(0).sb_options[0] & 0x10);
}
//*****************************************************************************
//  Function        : ExpDateReqd
//  Description     : Check whether config require expiry date.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ExpDateReqd(void)
{
  return (STIS_ISS_TBL(0).sb_options[0] & 0x08);
}
//*****************************************************************************
//  Function        : VoiceReferral
//  Description     : Check whether Voice referral required.
//  Input           : host response code
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN VoiceReferral(WORD aRspCode)
{
  return ((aRspCode <= '0'*256+'2') && (STIS_ISS_TBL(0).sb_options[0] & 0x20));
}
//*****************************************************************************
//  Function        : DescriptorReqd
//  Description     : Check whether config require product code.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DescriptorReqd(void)
{
  return (STIS_ISS_TBL(0).sb_options[0] & 0x40);
}
//*****************************************************************************
//  Function        : AdjustAllowed
//  Description     : Check whether config allow adjustment.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AdjustAllowed(void)
{
  return (STIS_ISS_TBL(0).sb_options[0] & 0x80);
}
//*****************************************************************************
//  Function        : CheckPAN
//  Description     : Check whether config require check PAN.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckPAN(void)
{
  return (STIS_ISS_TBL(0).sb_options[1] & 0x01);
}
//*****************************************************************************
//  Function        : PrintReqd
//  Description     : Check whether config require print receipt.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintReqd(void)
{
  return (STIS_ISS_TBL(0).sb_options[1] & 0x04);
}
//*****************************************************************************
//  Function        : DefaultCapture
//  Description     : Check whether config default as capture transaction(sale)
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DefaultCapture(void)
{
  return (STIS_ISS_TBL(0).sb_options[1] & 0x08);
}
//*****************************************************************************
//  Function        : CheckExpDate
//  Description     : Check whether config require check expiry date.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckExpDate(void)
{
  return (STIS_ISS_TBL(0).sb_options[1] & 0x10);
}
//*****************************************************************************
//  Function        : RefundBlocked
//  Description     : Check whether config BLOCK refund transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN RefundBlocked(void)
{
  return (STIS_ISS_TBL(0).sb_options[1] & 0x40);
}
//*****************************************************************************
//  Function        : AuthBlocked
//  Description     : Check whether config BLOCK non default AUTH transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AuthBlocked(void)
{
  return (STIS_ISS_TBL(0).sb_options[1] & 0x80);
}
//*****************************************************************************
//  Function        : VoidBlocked
//  Description     : Check whether config BLOCK VOID transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN VoidBlocked(void)
{
  return (STIS_ISS_TBL(0).sb_options[2] & 0x01);
}
//*****************************************************************************
//  Function        : BlockAuthCode
//  Description     : Check whether config BLOCK authorization code.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BlockAuthCode(void)
{
  return (STIS_ISS_TBL(0).sb_options[2] & 0x08);
}
//*****************************************************************************
//  Function        : MaskCardNo
//  Description     : Check whether config for mask PAN.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MaskCardNo(void)
{
  return (STIS_ISS_TBL(0).sb_reserved[0] & 0x01);
}
//*****************************************************************************
//  Function        : ExpiryReject
//  Description     : Check for reject trans for expired card.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
BOOLEAN ExpiryReject(void)
{
  return(STIS_ISS_TBL(0).sb_reserved[1] & 0x10);
}
//*****************************************************************************
//  Function        : BlockLocalManual
//  Description     : Block Local Manual Keyin transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BlockLocalManual(void)
{
  return (STIS_ISS_TBL(0).sb_reserved[0]&0x40);
}
//*****************************************************************************
//  Function        : BlockLocalSwipe
//  Description     : Block Local SWIPE transaction.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BlockLocalSwipe(void)
{
  return (STIS_ISS_TBL(0).sb_reserved[0]&0x20);
}
//*****************************************************************************
//  Function        : Track1Enable
//  Description     : Check for Track 1 enable parameter.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN Track1Enable(void)
{
  return (STIS_ISS_TBL(0).sb_reserved[1] & 0x20);
}
//*****************************************************************************
//  Function        : EMVProcDisable
//  Description     : Check whether EMV process is disable.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EMVProcDisable(void)
{
  return (STIS_ISS_TBL(0).sb_reserved[3]&0x02);
}


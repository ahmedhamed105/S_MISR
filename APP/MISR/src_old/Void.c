//-----------------------------------------------------------------------------
//  File          : Void.c
//  Module        :
//  Description   : Include routines for Void transaction.
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
#include <string.h>
#include "util.h"
#include "sysutil.h"
//#include "key2dll.h"
#include "message.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "input.h"
#include "print.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "reversal.h"
#include "offline.h"
#include "emvtrans.h"
//#include "emvutil.h"
#include "record.h"
#include "input.h"

//*****************************************************************************
//  Function        : VoidTrans
//  Description     : Do a Void transaction.
//  Input           : aRecIdx;          // record index
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void VoidTrans(DWORD aRecIdx)
{
  struct TXN_RECORD org_rec;

  INPUT.b_trans = VOID;

  if (STIS_TERM_DATA.b_stis_mode < TRANS_MODE) {
    DispErrorMsg(GetConstMsg(EDC_VD_NOT_ALLOW));
    return;
  }

  if (aRecIdx == -1) {
    DispHeader(NULL);
    aRecIdx = SearchRecord(FALSE);
    if (aRecIdx == -1)
      return;
  }
 // IncTraceNo();
  DispHeader(NULL);
//  PackHeader();

  APM_GetBatchRec(aRecIdx, (BYTE *)&org_rec, sizeof(org_rec));
  if (org_rec.b_trans_status & VOIDED) {
    DispErrorMsg(GetConstMsg(EDC_AJ_TXN_VOIDED));
    return;
  }

  APM_GetTermCfg(&STIS_TERM_CFG);
  APM_GetAcqTbl(org_rec.w_host_idx, &STIS_ACQ_TBL(0));
  APM_GetIssuer(org_rec.w_issuer_idx, &STIS_ISS_TBL(0));
  INPUT.w_host_idx   = org_rec.w_host_idx;
  INPUT.w_issuer_idx = org_rec.w_issuer_idx;

  if (VoidBlocked() || (org_rec.b_trans < SALE_SWIPE)) {
    DispErrorMsg(GetConstMsg(EDC_VD_NOT_ALLOW));
    return;
  }

  if (org_rec.b_trans_status & VOIDED) {
    DispErrorMsg(GetConstMsg(EDC_VD_VOIDED));
    return;
  }

  if (HostSettlePending()) {
    DispErrorMsg(GetConstMsg(EDC_AJ_CLOSE_BATCH));
    return;
  }

  DispLineMW(GetConstMsg(EDC_AJ_TOTAL_PROMPT), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  DispAmount(org_rec.dd_amount,((SaleType(org_rec.b_trans)) ? MW_LINE3 : MW_LINE3+0x01), MW_BIGFONT);
  if (PromptYesNo() != 2)
    return;

  DispHeader(STIS_ISS_TBL(0).sb_card_name);
  P_DATA.b_type = CUS_SLIP;
  if (((org_rec.b_trans_status & OFFLINE) != 0) || VoidOffline()) {
    org_rec.b_trans_status |= (VOIDED | ADJUSTED_OFF);
    RSP_DATA.b_response = TRANS_ACP;
    RSP_DATA.w_rspcode = '0'*256+'0';
    ReadRTC(&org_rec.s_dtg);
    IncTraceNoEND();
    memcpy(&RECORD_BUF, &org_rec, sizeof(struct TXN_RECORD));
    UpdateRecord(aRecIdx);
    PackRecordP(FALSE);
    TransEnd(TRUE);
    return;
  }
  ClearResponse();
  PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
    if (ReversalOK()) {
      OfflineSent(0);
      //APM_GetBatchRec(aRecIdx, (BYTE *)&RECORD_BUF);
      ByteCopy((BYTE *)&TX_DATA, (BYTE *)&org_rec, TX_DATA.sb_pin-(BYTE *)&TX_DATA);
      TX_DATA.dd_amount = TX_DATA.dd_org_amount;
      TX_DATA.b_trans  = VOID;
      PackProcCode(org_rec.b_trans, TX_DATA.b_acc_ind);
      TX_DATA.sb_proc_code[0] = TX_DATA.sb_proc_code[0] | 0x02;
      IncTraceNo();
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);

      ClearResponse();
      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        if (RSP_DATA.b_response <= TRANS_REJ)
          UpdateHostStatus(NO_PENDING);
      }
      if (RSP_DATA.b_response == TRANS_ACP) {
        org_rec.b_trans_status |= VOIDED;
        memcpy(org_rec.sb_trace_no, TX_DATA.sb_trace_no, sizeof(org_rec.sb_trace_no));
        memcpy(org_rec.sb_auth_code, RSP_DATA.sb_auth_code, sizeof(org_rec.sb_auth_code));
        ReadRTC(&org_rec.s_dtg);
        memcpy(&RECORD_BUF, &org_rec, sizeof(struct TXN_RECORD));
        UpdateRecord(aRecIdx);
        PackRecordP(FALSE);
      }
    }
  TransEnd(TRUE);
  Delay10ms(50);
}

//-----------------------------------------------------------------------------
//  File          : offline.c
//  Module        :
//  Description   : Include routines send "OFFLINE" transaction to host.
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
#include "sysutil.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "record.h"
#include "offline.h"

//*****************************************************************************
//  Function        : OfflineSent
//  Description     : Send "OFFLINE" transaction to host.
//  Input           : aSendAll;      // TRUE:- send all
//                                   // FALSE; send only one.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN OfflineSent(BOOLEAN aSendAll)
{
  WORD trans_status, rec_idx;

  if (!aSendAll)
    if ((STIS_ACQ_TBL(0).b_status == DOWN) || !PiggybackOffline())
      return TRUE;

  rec_idx = 0;
  while (TRUE) {
    trans_status = 0;
    while (trans_status == 0) {
      if (!APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF)))
        return TRUE;
      if (RECORD_BUF.w_host_idx == INPUT.w_host_idx)
        trans_status = RECORD_BUF.b_trans_status;

      trans_status &= (OFFLINE | ADJUSTED_OFF);

      if (trans_status == 0)
        rec_idx++;
    }
    ByteCopy((BYTE *)&TX_DATA, (BYTE *)&RECORD_BUF,
             (BYTE *)&RECORD_BUF.s_dtg-(BYTE *)&RECORD_BUF+sizeof(struct DATETIME));
    ByteCopy((BYTE *)&TX_DATA.s_icc_data, (BYTE *)&RECORD_BUF.s_icc_data, sizeof(struct ICC_DATA));
    TX_DATA.b_org_trans = TX_DATA.b_trans;

    PackProcCode(TX_DATA.b_org_trans, TX_DATA.b_acc_ind);

    if ((TX_DATA.b_trans_status & VOIDED) != 0) {
      TX_DATA.dd_amount=0; /* offline void */
      if (TX_DATA.b_org_trans == REFUND)
        TX_DATA.sb_proc_code[0] = 0x20;
    }
    if ((TX_DATA.b_trans_status & OFFLINE)==0) {
//        IncTraceNo();
      TX_DATA.b_trans = ADJUST;
      TX_DATA.sb_proc_code[0] |= 0x02;
    }
    else { /* offline - trans not sent to host yet */
      TX_DATA.b_trans = TX_DATA.b_org_trans;
    }

    PackHostMsg();
    ClearResponse();
    if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
      RSP_DATA.b_response = CheckHostRsp();
      if (RSP_DATA.b_response <= TRANS_REJ)
        UpdateHostStatus(NO_PENDING);
    }
    if (RSP_DATA.b_response <= TRANS_REJ) {
      Short1Beep();
      if (STIS_ACQ_TBL(0).b_status == DOWN)
        return TRUE;   /* return if host still down */
      else
        RECORD_BUF.b_trans_status &= 0x3F;  // RETAIN ADJUSTED & VOIDED flag

      if (TX_DATA.b_trans != ADJUST)
        memcpy(RECORD_BUF.sb_rrn, RSP_DATA.sb_rrn, 12);
      RECORD_BUF.dd_org_amount=TX_DATA.dd_amount;
      memcpy(RECORD_BUF.sb_trace_no, TX_DATA.sb_trace_no, 3);
      UpdateRecord(rec_idx);
      rec_idx += 1;
      if (aSendAll)
        continue;
      return TRUE;
    }
    return FALSE;
  }
}

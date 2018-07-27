//-----------------------------------------------------------------------------
//  File          : reversal.c
//  Module        :
//  Description   : Include routines send reversal transaction to host.
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
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "emvtrans.h"
#include "record.h"
#include "reversal.h"

//*****************************************************************************
//  Function        : ReversalOK
//  Description     : Send reversal transaction to host if available.
//  Input           : N/A
//  Return          : TRUE;     // sended;
//                    FALSE;    // send error.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ReversalOK(void)
{
  DWORD host_idx;

  host_idx = INPUT.w_host_idx;
  if (APM_GetPending(host_idx) != REV_PENDING)
    return TRUE;
  APM_GetRevRec(host_idx, &RECORD_BUF, sizeof(RECORD_BUF));
  memcpy(&TX_DATA.b_trans, &RECORD_BUF.b_trans, (DWORD) ((DWORD)&RECORD_BUF.w_crc - (DWORD)&RECORD_BUF));

  if (TX_DATA.b_trans == VOID) {
    PackProcCode(RECORD_BUF.b_trans, RECORD_BUF.b_acc_ind);
    TX_DATA.sb_proc_code[0] |= 0x02;
    TX_DATA.dd_amount = 0;
  }
  else
    PackProcCode(TX_DATA.b_trans, RECORD_BUF.b_acc_ind);

  TX_DATA.b_org_trans = TX_DATA.b_trans;
  TX_DATA.b_trans = EDC_REV;

  PackHostMsg();
  ClearResponse();
  if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
    RSP_DATA.b_response = CheckHostRsp();
    if (RSP_DATA.b_response <= TRANS_REJ)
      UpdateHostStatus(NO_PENDING);
  }
  if (RSP_DATA.b_response <= TRANS_REJ) {
    gAppDat.reversal_count++;
    return TRUE;
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
//  File          : refund.c
//  Module        :
//  Description   : Include Refund Transaction routine.
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
#include "message.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "print.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "record.h"
#include "reversal.h"
#include "offline.h"
#include "emvtrans.h"
#include "refund.h"

//*****************************************************************************
//  Function        : RefundTrans
//  Description     : Refund transaction.
//  Input           : aInputMode;       // Transaction start mode
//  return          : TRUE/FALSE;       // FALSE => Cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD RefundTrans(DWORD aInputMode)
{
  INPUT.b_trans = REFUND;

  DispHeader(NULL);
//  PowerFailure();

  if (!TransAllowed(aInputMode)) {
    RefreshDispAfter(0);
    return FALSE;
  }

  if (RefundOffline()) {
	 IncTraceNo();
	   PackHeader();
    RSP_DATA.b_response = TRANS_ACP;
    IncTraceNoEND();
    GenAuthCode(RSP_DATA.sb_auth_code);
    PackDTGAA();
    SaveOfflRecord();   //17-12-12 JC ++
    AcceptBeep();
    PackInputP();
    TransEnd(TRUE);
    return FALSE;
  }

  if ((INPUT.b_entry_mode == MANUAL) || (INPUT.b_entry_mode == MANUAL_4DBC))
    INPUT.b_trans = ONL_REFUND_MNL;
  else
    INPUT.b_trans = ONL_REFUND_SW;
  ClearResponse();
  //PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    if (ReversalOK()) {
      OfflineSent(0);
	  IncTraceNoEND();
      MoveInput2Tx();
      PackProcCode(TX_DATA.b_trans,0xFF);
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);

      ClearResponse();
      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        if (RSP_DATA.b_response <= TRANS_REJ)
          UpdateHostStatus(NO_PENDING);
      }
      //SaveTraceNo();
      if (RSP_DATA.b_response == TRANS_ACP) {
        if (RSP_DATA.w_rspcode == '0'*256+'0')
          RSP_DATA.w_rspcode = 'A'*256+'P';
        SaveRecord();
        PackInputP();
      }
    }
  }
  TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  return TRUE;
}

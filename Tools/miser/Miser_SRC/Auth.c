//-----------------------------------------------------------------------------
//  File          : Auth.c
//  Module        :
//  Description   : Include Auth transaction.
//  Author        : Lewis
//  Notes         : N/A
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
#include "apm.h"
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
#include "sale.h"
#include "emvtrans.h"
#include "auth.h"

//*****************************************************************************
//  Function        : AuthTrans
//  Description     : Do authorization transaction.
//  Input           : aInputMode;       // Transaction start mode
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD AuthTrans(DWORD aInputMode)
{
  INPUT.b_trans = AUTH_SWIPE;

  DispHeader(NULL);
//  PowerFailure();

  if (!TransAllowed(aInputMode)) {
    RefreshDispAfter(0);
    return FALSE;
  }

  if (aInputMode == MANUAL)
    INPUT.b_trans = AUTH_MANUAL;

  ClearResponse();
  //PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    if (ReversalOK()) {
      OfflineSent(0);
      IncTraceNo();
      MoveInput2Tx();
      PackProcCode(TX_DATA.b_trans, 0xFF);
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);
      fCommitAllMW();

      ClearResponse();
      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        if (RSP_DATA.b_response <= TRANS_REJ)
          UpdateHostStatus(NO_PENDING);
      }
      //SaveTraceNo();
      if (RSP_DATA.b_response == TRANS_ACP) {
        RSP_DATA.w_rspcode = 'A'*256+'P';
        PackInputP();
        UpdateHostStatus(NO_PENDING);
      }
      else
        if (RSP_DATA.b_response == TRANS_REJ) {
          UpdateHostStatus(NO_PENDING);
          if (VoiceReferral(RSP_DATA.w_rspcode)) {
            ReferralProcess();
            return TRUE;
          }
        }
    }
  }
  TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  return TRUE;
}

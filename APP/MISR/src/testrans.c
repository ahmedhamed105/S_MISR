//-----------------------------------------------------------------------------
//  File          : testrans.c
//  Module        :
//  Description   : Include Host Test Transactions
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
#include "corevar.h"
#include "constant.h"
#include "input.h"
#include "tranutil.h"
#include "record.h"
#include "hostmsg.h"
#include "testrans.h"

//*****************************************************************************
//  Function        : TestTrans
//  Description     : Do a test transaction with host.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TestTrans(void)
{
  int host_idx;

  INPUT.b_trans = TEST_MSG;

  host_idx = APM_SelectAcquirer(FALSE);
  if (host_idx == -1) {
    LongBeep();
    RefreshDispAfter(0);
    return FALSE;
  }

  DispHeader(NULL);

  APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0));
  PackComm(host_idx, FALSE);
  IncTraceNo();
  ClearResponse();
  MoveInput2Tx();
  PackProcCode(INPUT.b_trans, 0x00);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    PackHostMsg();
    if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
      RSP_DATA.b_response = CheckHostRsp();
    }
    //SaveTraceNo();
  }
  TransEnd(FALSE);
  return TRUE;
}

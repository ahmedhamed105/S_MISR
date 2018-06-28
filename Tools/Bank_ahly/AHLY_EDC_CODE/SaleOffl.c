//-----------------------------------------------------------------------------
//  File          : saleoffl.c
//  Module        :
//  Description   : Include Offline Sale transaction.
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
//  10 May  2006  Lewis       Initial Version for new CONTROL
//-----------------------------------------------------------------------------
#include <string.h>
#include "util.h"
#include "sysutil.h"
#include "message.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "input.h"
#include "print.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "record.h"
#include "reversal.h"
#include "offline.h"


//*****************************************************************************
//  Function        : SaleOfflTrans
//  Description     : Sale Offline transaction.
//  Input           : aInputMode;       // Transaction start mode
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD SaleOfflTrans(DWORD aInputMode)
{
  INPUT.b_trans = SALE_OFFLINE;

  DispHeader(NULL);
  //PowerFailure();

  if (!TransAllowed(aInputMode)) {
    RefreshDispAfter(0);
    return FALSE;
  }

  if (!GetAuthCode()) {
    RefreshDispAfter(0);
    return FALSE;
  }

  DispHeader(STIS_ISS_TBL(0).sb_card_name);
  RSP_DATA.b_response = TRANS_ACP;
  INPUT.b_trans_status = OFFLINE;
  IncTraceNo();
  PackDTGAA();
  SaveOfflRecord();   //17-12-12 JC ++
  PackInputP();
  TransEnd(TRUE);
  return TRUE;
}

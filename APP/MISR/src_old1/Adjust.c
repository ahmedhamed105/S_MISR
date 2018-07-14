//-----------------------------------------------------------------------------
//  File          : Adjust.c
//  Module        :
//  Description   : Include routines for Adjust transaction.
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
#include "message.h"
#include "chkoptn.h"
#include "input.h"
#include "tranutil.h"
#include "record.h"
#include "adjust.h"

//*****************************************************************************
//  Function        : DipsCurrTotal
//  Description     : Display current transaction totals.
//  Input           : aOrgTrans;      // trasaction id;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispCurrTotal(BYTE aOrgTrans)
{
  DDWORD amount;

  DispLineMW(GetConstMsg(EDC_AJ_TOTAL_PROMPT), MW_LINE1, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  amount = INPUT.dd_amount;
  if (SaleType(aOrgTrans)) {
    amount += INPUT.dd_tip;
    DispAmount(amount, MW_LINE3, MW_BIGFONT);
  }
  else
    DispAmount(amount, MW_LINE3+0x01, MW_BIGFONT); /* MW_LLINE1+1 = -ve */
}
//*****************************************************************************
//  Function        : AdjustRecord
//  Description     : Adjust the transaction record.
//  Input           : aIdx;       // record index
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void AdjustRecord(DWORD aIdx)
{
  static const DWORD KDecimalPos[4] = { DECIMAL_NONE, DECIMAL_POS1, DECIMAL_POS2, DECIMAL_POS3};
  BOOLEAN input_ok;
  BYTE adjust_type, org_trans;
  DDWORD amount;
  BYTE   kbdbuf[32];

  org_trans = RECORD_BUF.b_trans;

  // Check multi-adjustment allow
  if ( ((RECORD_BUF.b_trans_status & ADJUSTED) && !MultiAdjustAllowed()) ||
       ((org_trans <= REFUND) && (org_trans >=ONL_REFUND_SW)) ) {
    DispErrorMsg(GetConstMsg(EDC_AJ_NOT_ALLOW));
    return;
  }
  INPUT.dd_amount = RECORD_BUF.dd_amount;
  INPUT.dd_tip    = RECORD_BUF.dd_tip;
  INPUT.dd_amount -= INPUT.dd_tip;

  adjust_type = 0;
  if (TIPsReqd() && SaleType(org_trans))
    adjust_type = 2; /* adjust total */
  else
    adjust_type = 1; /* adjust tip */

  while (TRUE) {
    input_ok = FALSE;
    switch (adjust_type) {
      case 0: /* 0 adjust amount */
        DispCurrTotal(org_trans);
        DispLineMW(GetConstMsg(EDC_AJ_ENTER_NAMT), MW_LINE5, MW_BIGFONT);
        if (!APM_GetKbd(AMOUNT_INPUT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos], IMIN(1)+IMAX(STIS_TERM_CFG.b_trans_amount_len), kbdbuf))
          return;
        if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
          memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
          kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
        }
        INPUT.dd_amount = decbin8b(&kbdbuf[1], kbdbuf[0]);
        input_ok = TRUE;
        break;

      case 1: /* 1 adjust tip */
        if (INPUT.dd_tip == 0) {
          DispLineMW(GetConstMsg(EDC_AJ_BASE_AMT), MW_LINE1, MW_CENTER|MW_BIGFONT);
          DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
          DispLineMW(GetConstMsg(EDC_AJ_TIPS_AMT), MW_LINE5, MW_BIGFONT);
        }
        else {
          DispLineMW(GetConstMsg(EDC_AJ_TIPS_AMT), MW_LINE1, MW_BIGFONT);
          DispAmount(INPUT.dd_tip, MW_LINE3, MW_BIGFONT);
          DispLineMW(GetConstMsg(EDC_AJ_ENTER_NTIP), MW_LINE5, MW_BIGFONT);
        }
        if (!APM_GetKbd(AMOUNT_INPUT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos], IMIN(1)+IMAX(STIS_TERM_CFG.b_trans_amount_len), kbdbuf))
          return;
        if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
          memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
          kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
        }
        INPUT.dd_tip = decbin8b(&kbdbuf[1], kbdbuf[0]);
        input_ok = TRUE;
        break;

      case 2: /* 2 - adjust total */
        DispLineMW(GetConstMsg(EDC_AJ_BASE_AMT), MW_LINE1, MW_CENTER|MW_BIGFONT);
        DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
        DispLineMW(GetConstMsg(EDC_AJ_TOTAL_PROMPT), MW_LINE5, MW_CENTER|MW_BIGFONT);
        if (!APM_GetKbd(AMOUNT_INPUT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos], IMIN(1)+IMAX(STIS_TERM_CFG.b_trans_amount_len), kbdbuf))
          return;
        if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
          memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
          kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
        }
        amount = decbin8b(&kbdbuf[1], kbdbuf[0]);
        if (amount == 0)
          adjust_type = 1; /* adjust TIP */
        else
          if (amount < INPUT.dd_amount)
            LongBeep();
          else {
            INPUT.dd_tip = amount - INPUT.dd_amount;
            input_ok = TRUE;
          }
        break;

      case 3:  /* adjust base */
        DispLineMW(GetConstMsg(EDC_AJ_BASE_AMT), MW_LINE1, MW_CENTER|MW_BIGFONT);
        DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
        DispLineMW(GetConstMsg(EDC_AJ_ENTER_NAMT), MW_LINE5, MW_BIGFONT);
        if (!APM_GetKbd(AMOUNT_INPUT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos], IMIN(1)+IMAX(STIS_TERM_CFG.b_trans_amount_len), kbdbuf))
          return;
        if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
          memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
          kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
        }
        INPUT.dd_amount = decbin8b(&kbdbuf[1], kbdbuf[0]);
        input_ok = TRUE;
        break;
    }
    if (input_ok) {
      if (OverMargin()) {
        DispErrorMsg(GetConstMsg(EDC_AJ_OVERLIMIT));
        continue;
      }

      DispCurrTotal(org_trans);

      switch (PromptYesNo()) {
        case 0: /* cancel key */
          return;
        case 1: /* no */
          continue;
      }
      INPUT.dd_amount += INPUT.dd_tip;
      RECORD_BUF.dd_tip    = INPUT.dd_tip;
      RECORD_BUF.dd_amount = INPUT.dd_amount;
      if ((RECORD_BUF.b_trans_status & OFFLINE) == 0)
        RECORD_BUF.b_trans_status = ADJUSTED_OFF|ADJUSTED;
      else
        RECORD_BUF.b_trans_status |= ADJUSTED;
      //IncTraceNo();
      //memcpy(RECORD_BUF.sb_trace_no, INPUT.sb_trace_no, sizeof(RECORD_BUF.sb_trace_no));
      UpdateRecord(aIdx);
      DispHeader(NULL);
      DispClrBelowMW(MW_LINE3);
      Disp2x16Msg(GetConstMsg(EDC_AJ_COMPLETED), MW_LINE5, MW_BIGFONT);
      AcceptBeep();
      Delay1Sec(3,0);
      return;
    }
  }
}
//*****************************************************************************
//  Function        : AdjustTrans
//  Description     : Do a adjustment transaction.
//  Input           : aRecIdx;      // record index
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void AdjustTrans(DWORD aRecIdx)
{
  INPUT.b_trans = ADJUST;

  if (STIS_TERM_DATA.b_stis_mode < TRANS_MODE) {
    DispErrorMsg(GetConstMsg(EDC_AJ_NOT_ALLOW));
    return;
  }
  if (aRecIdx == -1) {
    DispHeader(NULL);
    aRecIdx = SearchRecord(FALSE);
    if (aRecIdx == -1)
      return;
  }

  DispHeader(NULL);
  APM_GetBatchRec(aRecIdx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
  if (RECORD_BUF.b_trans_status & VOIDED) {
    DispErrorMsg(GetConstMsg(EDC_AJ_TXN_VOIDED));
    return;
  }

  APM_GetTermData(&STIS_TERM_DATA);
  APM_GetAcqTbl(RECORD_BUF.w_host_idx, &STIS_ACQ_TBL(0));
  APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(0));

  if (!AdjustAllowed()) {
    DispErrorMsg(GetConstMsg(EDC_AJ_NOT_ALLOW));
    return;
  }

  if (HostSettlePending() || APM_BatchFull(INPUT.w_host_idx)) {
    DispErrorMsg(GetConstMsg(EDC_AJ_CLOSE_BATCH));
    return;
  }
  AdjustRecord(aRecIdx);
}


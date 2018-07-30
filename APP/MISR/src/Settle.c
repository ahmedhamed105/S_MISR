//-----------------------------------------------------------------------------
//  File          : settle.c
//  Module        :
//  Description   : Include settlement releated routines.
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
//  10 May  2006  Lewis       Initial Version for new CONTROL
//-----------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
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
#include "emvtrans.h"
#include "settle.h"

//*****************************************************************************
//  Function        : FormSettleSlip
//  Description     : Pack the settlement slip.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
static void FormSettleSlip(void)
{
  static const BYTE KBusinessDate[]  = {"BUSINESS DATE - "};
  static const BYTE KSoc[]            = {"****** SUMMARY OF CHARGE ******"};
  static const BYTE KCloseBatch[]     = {"**** CLOSE BATCH CONFIRMED ****"};
  static const BYTE KNII[]            = {"NII "};
  BYTE tmp[31];
  struct DATETIME dtg;

  if (gGDS->s_bs_date.b_mm == 0 && gGDS->s_bs_date.b_dd == 0) {
    ReadRTC(&dtg);
    gGDS->s_bs_date.b_mm = dtg.b_month;
    gGDS->s_bs_date.b_dd = dtg.b_day;
  }

  MsgBufSetup();
  PackLogo();
  pack_lf();
  PackRcptHeader();
  pack_lf();
  pack_space(2);
  pack_mem("\x1BW1SETTLEMENT\x1BW0", 16);
  pack_lfs(2);

  pack_mem(STIS_ACQ_TBL(0).sb_name,10);
  pack_space(MAX_CHAR_LINE_NORMAL-18);
  pack_mem((BYTE *)KNII, 4);
  split_data(STIS_ACQ_TBL(0).sb_nii,2);
  pack_lf();

  pack_mem((BYTE *)KBusinessDate, 16);
  pack_mem((BYTE *)GetMonthText((BYTE)bcd2bin(gGDS->s_bs_date.b_mm)),  3);
  pack_byte(' ');
  split_data(&gGDS->s_bs_date.b_dd, 1);
  pack_lf();

  ReadRTC(&dtg);
  ConvDateTime(tmp, &dtg, 1);
  pack_mem(tmp, 18);
  pack_lfs(2);

  pack_mem((BYTE *)KSoc, 31);
  pack_lf();
  pack_mem((BYTE *)KCloseBatch, 31);
  pack_lf();
  pack_mem(&RSP_DATA.text[1], 20);
  pack_lf();
  pack_mem(&RSP_DATA.text[21], 20);
  pack_lfs(2);
  PackTotals(&TERM_TOT);
  PackFF();
  PackIssuerSum(0);
  pack_lf();
  PackFF();

  PackMsgBufLen();
}
//*****************************************************************************
//  Function        : UploadTransOK
//  Description     : Upload transaction to host.
//  Input           : N/A
//  Return          : TRUE;  // upload success.
//                    FALSE;
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
static BOOLEAN UploadTransOK(void)
{
  DWORD i = 0;
  int count;
    
    count = APM_GetRecCount();

  while (TRUE) {
    if (!APM_GetBatchRec(i, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF)))
      break;
      
    i++;
      if(i=count){
          
          if (RECORD_BUF.w_host_idx == INPUT.w_host_idx) {
              if (!ValidRecordCRC((WORD)i)) {         // check record's crc
                  SetRspCode('D'*256+'E');
                  PackRspText();
                  return(FALSE);
              }
              ByteCopy((BYTE *)&TX_DATA, (BYTE *)&RECORD_BUF, (BYTE *)TX_DATA.sb_pin-(BYTE *)&TX_DATA);
              memcpy(TX_DATA.sb_org_trace_no, TX_DATA.sb_trace_no, 3);
              if (RECORD_BUF.b_trans_status & VOIDED)
                  TX_DATA.dd_amount = 0;
              
              TX_DATA.b_org_trans = RECORD_BUF.b_trans;
              PackProcCode(TX_DATA.b_org_trans, RECORD_BUF.b_acc_ind);
              TX_DATA.sb_proc_code[2] = 0x01;            /* more message to come */
              TX_DATA.b_trans = TRANS_UPLOAD_LAST;
              IncTraceNo();
              PackHostMsg();
              UpdateHostStatus(SETTLE_PENDING);
              
              ClearResponse();
              if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
                  RSP_DATA.b_response = CheckHostRsp();
                  //SaveTraceNo();
                  if (RSP_DATA.b_response == TRANS_ACP)
                      UpdateHostStatus(NO_PENDING);
              }
              if ((RSP_DATA.b_response == TRANS_ACP) || (RSP_DATA.b_response == MORE_RSP))
                  Short1Beep();
              else
                  return FALSE;
          }
          
          
          
      }else{
      
    if (RECORD_BUF.w_host_idx == INPUT.w_host_idx) {
      if (!ValidRecordCRC((WORD)i)) {         // check record's crc
        SetRspCode('D'*256+'E');
        PackRspText();
        return(FALSE);
      }
      ByteCopy((BYTE *)&TX_DATA, (BYTE *)&RECORD_BUF, (BYTE *)TX_DATA.sb_pin-(BYTE *)&TX_DATA);
      memcpy(TX_DATA.sb_org_trace_no, TX_DATA.sb_trace_no, 3);
      if (RECORD_BUF.b_trans_status & VOIDED)
        TX_DATA.dd_amount = 0;

      TX_DATA.b_org_trans = RECORD_BUF.b_trans;
      PackProcCode(TX_DATA.b_org_trans, RECORD_BUF.b_acc_ind);
      TX_DATA.sb_proc_code[2] = 0x01;            /* more message to come */
      TX_DATA.b_trans = TRANS_UPLOAD;
      IncTraceNo();
      PackHostMsg();
      UpdateHostStatus(SETTLE_PENDING);

      ClearResponse();
      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        //SaveTraceNo();
        if (RSP_DATA.b_response == TRANS_ACP)
          UpdateHostStatus(NO_PENDING);
      }
      if ((RSP_DATA.b_response == TRANS_ACP) || (RSP_DATA.b_response == MORE_RSP))
        Short1Beep();
      else
        return FALSE;
    }
          
      }
  }
  return TRUE;
}
//*****************************************************************************
//  Function        : ValidDate
//  Description     : Validate the date value.
//  Input           : aDate;    // pointer to date_t buffer.
//  Return          : TRUE;     // valid input.
//                    FALSE;
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
static BOOLEAN ValidDate(struct BS_DATE *aDate)
{
  static const BYTE days_limit [13] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  DWORD var_x;

  var_x = (BYTE)bcd2bin(aDate->b_mm);
  if ((var_x == 0) || (var_x > 12))
    return FALSE;
  if (bcd2bin(aDate->b_dd) > days_limit[var_x])
    return FALSE;
  return TRUE;
}
//*****************************************************************************
//  Function        : GetBusinessDate
//  Description     : Prompt user to enter bussiness date.
//  Input           : N/A
//  Return          : TRUE;  // valid INPUT.
//                    FALSE; // user cancel
//  Note            : result store in globals gBusDate
//  Globals         : gBusDate;
//*****************************************************************************
static BOOLEAN GetBusinessDate(void)
{
  static const BYTE Kddmm[]            = {"DDMM"};
  struct DATETIME dtg;
  BYTE kbdbuf[31];

  while (1) {
    kbdbuf[0] = 0;
    DispHeader(NULL);
    Disp2x16Msg(GetConstMsg(EDC_SE_BUS_DATE), MW_LINE3, MW_BIGFONT);
    if (DDMMBusinessDate())
      DispLineMW(Kddmm, MW_LINE5+12, MW_BIGFONT);
    if (!APM_GetKbd(NUMERIC_INPUT+NULL_ENB+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMIN(4)+IMAX(4), kbdbuf))
      return FALSE;

    if (kbdbuf[0] == 0) {
      ReadRTC(&dtg);
      gGDS->s_bs_date.b_mm = dtg.b_month;
      gGDS->s_bs_date.b_dd = dtg.b_day;
    } else if (DDMMBusinessDate()) {
      compress(&kbdbuf[1],&kbdbuf[1], 2);
      gGDS->s_bs_date.b_dd = kbdbuf[1];
      gGDS->s_bs_date.b_mm = kbdbuf[2];
    } else
      CompressInputData(&gGDS->s_bs_date, kbdbuf, 2);

    if (ValidDate(&gGDS->s_bs_date)) {
      return TRUE;
    }
    LongBeep();
  }
}
//*****************************************************************************
//  Function        : GetSettleAmount
//  Description     : Prompt user to enter settlement amount.
//  Input           : N/A
//  Return          : TRUE;  // valid INPUT.
//                    FALSE; // user cancel
//  Note            : N/A
//  Globals         : INPUT.dd_amount;
//*****************************************************************************
static BOOLEAN GetSettleAmount(void)
{
  static const DWORD KDecimalPos[4] = { DECIMAL_NONE, DECIMAL_POS1, DECIMAL_POS2, DECIMAL_POS3};
  BYTE kbdbuf[31];

  if (!APM_GetKbd(AMOUNT_INPUT+NULL_ENB+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos],
                  IMIN(1)+IMAX(STIS_TERM_CFG.b_settle_amount_len), kbdbuf))
    return FALSE;
  if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
    memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
    kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
  }
  INPUT.dd_amount = decbin8b(&kbdbuf[1], kbdbuf[0]);
  return TRUE;
}
//*****************************************************************************
//  Function        : PackBatchText
//  Description     : Pack settlement batch # in rspdata.text.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals         : rsp_data;
//*****************************************************************************
static void PackBatchText(void)
{
  RSP_DATA.text[0] = 40;
  memset(&RSP_DATA.text[1+5], ' ', 15);
  memcpy(&RSP_DATA.text[1], "BATCH", 5);
  split(&RSP_DATA.text[7],STIS_ACQ_TBL(0).sb_curr_batch_no, 3);
}
//*****************************************************************************
//  Function        : PrintSettle
//  Description     : Print Settlement slip.
//  Input           : aWorldHost
//  Return          : N/A
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
static void CloseBatch(void)
{
  memcpy(P_DATA.sb_roc_no, TX_DATA.sb_roc_no, 3);
  FormSettleSlip(); /* print it */
  memcpy(STIS_ACQ_TBL(0).sb_curr_batch_no, STIS_ACQ_TBL(0).sb_next_batch_no, 3);
  bcdinc(STIS_ACQ_TBL(0).sb_next_batch_no, 3);
  APM_SetBatchNo(INPUT.w_host_idx,STIS_ACQ_TBL(0).sb_curr_batch_no);
  APM_BatchClear(INPUT.w_host_idx);
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : AllTotalZero
//  Description     : Return TRUE when all totals are zero.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN AllTotalZero(void)
{
  return((TERM_TOT.s_sale_tot.dd_amount == 0)    &&
         (TERM_TOT.s_refund_tot.dd_amount == 0)  &&
         (TERM_TOT.s_void_sale.dd_amount == 0)   &&
         (TERM_TOT.s_void_refund.dd_amount == 0)
        );
}
//*****************************************************************************
//  Function        : ComfirmSetlTot
//  Description     : Confirm/Enter Settlement total & Bussiness Date.
//  Input           : N/A
//  Return          : TRUE/FALSE;       // TRUE => confirmed.
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
BOOLEAN ConfirmSetlTot(void)
{
  DDWORD total;
  APM_GetAcqTbl(INPUT.w_host_idx, &STIS_ACQ_TBL(0));

  INPUT.b_trans = SETTLEMENT;

  CalTotals(FALSE);
  if (AllTotalZero()) {
    DispLineMW(GetConstMsg(KTransHeader[SETTLEMENT]), MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
    DispErrorMsg(GetConstMsg(EDC_SE_NO_BATCH));
    RSP_DATA.w_rspcode = 'N'*256 + 'T';  // For ECR response
    return FALSE;
  }

  // No Confirmation for ECR Settlement
  if (gGDS->i_ecr_len > 0)
    return TRUE;

  if (ComfirmTotal()) {
    DispLineMW(GetConstMsg(EDC_SE_SALE_TOTAL), MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispAmount(TERM_TOT.s_sale_tot.dd_amount, MW_LINE3, MW_BIGFONT);
    if (PromptYesNo() != 2)
      return FALSE;
    DispLineMW(GetConstMsg(EDC_SE_REFUND_TOTAL), MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispAmount(TERM_TOT.s_refund_tot.dd_amount, MW_LINE3, MW_BIGFONT);
    if (PromptYesNo() != 2)
      return FALSE;
  }
  else {
    Disp2x16Msg(GetConstMsg(EDC_SE_ENTER_SALE_TOTAL), MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    if (!GetSettleAmount())
      return FALSE;
    if (INPUT.dd_amount != TERM_TOT.s_sale_tot.dd_amount) {
      DispErrorMsg(GetConstMsg(EDC_SE_TOTAL_WRONG));
      return FALSE;
    }
    total = TERM_TOT.s_refund_tot.dd_amount;
    Disp2x16Msg(GetConstMsg(EDC_SE_ENTER_REF_TOTAL), MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    if (!GetSettleAmount())
      return FALSE;
    if (INPUT.dd_amount != total) {
      DispErrorMsg(GetConstMsg(EDC_SE_TOTAL_WRONG));
      return FALSE;
    }
  }

  if (!GetBusinessDate())
    return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : SettleTrans
//  Description     : Do the settlement transaction.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
BOOLEAN SettleTrans(void)
{
  DWORD upload_count;
  BOOLEAN connected, settle_all, settle_done = FALSE;
     int i=0;
  WORD max_acq = APM_GetAcqCount();
    

  settle_all = (INPUT.w_host_idx == (WORD) -2);

  if (settle_all)
    INPUT.w_host_idx = 0;
  else {
    if (!ConfirmSetlTot())
      return FALSE;
  }

  //ProtectPutL(&gPRAM->reprint_buf.len, 0);
  for (; INPUT.w_host_idx < max_acq; INPUT.w_host_idx++) {
    if (!APM_GetAcqTbl(INPUT.w_host_idx, &STIS_ACQ_TBL(0)))
      continue;
    if (STIS_ACQ_TBL(0).b_status == NOT_LOADED)
      continue;
    if (!CorrectHost(GetHostType(0)))
      continue;

    INPUT.b_trans = SETTLEMENT;
    if (settle_all) {
      DispHeader(STIS_ACQ_TBL(0).sb_name);
      Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING),MW_LINE5, MW_BIGFONT);
      CalTotals(FALSE);
      if (AllTotalZero()) {
        Disp2x16Msg(GetConstMsg(EDC_SE_NO_BATCH), MW_LINE5, MW_BIGFONT);
        Delay1Sec(3, 1);
        continue;
      }
    } else
      DispHeader(NULL);

    upload_count = 0;
    PackComm(INPUT.w_host_idx, TRUE);
    settle_done = FALSE;
    connected   = FALSE;
    ClearResponse();
    if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
      if (ReversalOK())
        if (OfflineSent(TRUE))
          connected = TRUE;
    }
    while (connected) {
      TX_DATA.b_trans = SETTLEMENT;
      if (upload_count == 0)
        PackProcCode(SETTLEMENT, 0x00);
      else
        memcpy(TX_DATA.sb_proc_code, KSetlPCode2, 3);

      memcpy(TX_DATA.sb_roc_no, INPUT.sb_roc_no, 3);
      IncTraceNo();
      PackHostMsg();
      UpdateHostStatus(SETTLE_PENDING);

      ClearResponse();
      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))!=COMM_OK)
        break;
      RSP_DATA.b_response = CheckHostRsp();
      //SaveTraceNo();
      if (RSP_DATA.b_response==TRANS_ACP) {

          TX_DATA.b_trans = SETT_ADV;
          IncTraceNo();
          PackHostMsg();
          UpdateHostStatus(SETTLE_PENDING);
          ClearResponse();
         if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))!=COMM_OK)
           break;
         RSP_DATA.b_response = CheckHostRsp();
           if (RSP_DATA.b_response==TRANS_ACP) {
          
          if (RSP_DATA.Actioncode[0] == 0x35 && RSP_DATA.Actioncode[1] == 0x30 && RSP_DATA.Actioncode[2] == 0x30)
          {
              
              
          }else{
              
              printf("\f");
              i=0;
              while (i < 3)
              {
                  printf("%02X:",(int)RSP_DATA.Actioncode[i]);
                  i++;
              }
              APM_WaitKey(9000, 0);
              
              Short1Beep();
              if (UploadTransOK())
                  continue;
              
          }
          
           }
          
          

        UpdateHostStatus(NO_PENDING);
        if (RSP_DATA.text[0] == 0) {
          PackBatchText();
          memcpy(&RSP_DATA.text[21], "CLOSED          ", 16);
        }
        CloseBatch();
      } else if (RSP_DATA.b_response == TRANS_REJ) {
        if (RSP_DATA.w_rspcode == '9'*256+'5') {
          upload_count += 1;
          if (upload_count == 4) {
            UpdateHostStatus(NO_PENDING);
            RSP_DATA.w_rspcode = '7'*256+'7';
          } else {
            Short1Beep();
            if (UploadTransOK())
              continue;
          }
        } else if (RSP_DATA.w_rspcode == '7'*256+'7') {
        

          TX_DATA.b_trans = SETT_ADV;
          IncTraceNo();
          PackHostMsg();
          UpdateHostStatus(SETTLE_PENDING);
          ClearResponse();
         if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))!=COMM_OK)
           break;
         RSP_DATA.b_response = CheckHostRsp();
            if (RSP_DATA.b_response==TRANS_ACP) {
                
                if (RSP_DATA.Actioncode[0] == 0x35 && RSP_DATA.Actioncode[1] == 0x30 && RSP_DATA.Actioncode[2] == 0x30)
                {
                    
                    
                }else{
                    
                    printf("\f");
                    i=0;
                    while (i < 3)
                    {
                        printf("%02X:",(int)RSP_DATA.Actioncode[i]);
                        i++;
                    }
                    APM_WaitKey(9000, 0);
                    
                    Short1Beep();
                    if (UploadTransOK())
                        continue;
                    
                }
                
            }
          



          UpdateHostStatus(NO_PENDING);
          if (RSP_DATA.text[0] == 0) {
            PackRspText();
            PackBatchText();
          }

          CloseBatch();
        }
      }
      break;
    } // End While





    TransEnd(FALSE);

    if (!settle_all) {
      RefreshDispAfter((RSP_DATA.b_response != TRANS_ACP)? 90 : 30);
      settle_done = TRUE;
      break;
    } else
      Delay1Sec(3, 0);
  }
  return (settle_done==TRUE);
}

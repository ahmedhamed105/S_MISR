//-----------------------------------------------------------------------------
//  File          : func.c
//  Module        :
//  Description   : Include Merchant Functions.
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
#include <stdio.h>
#include "util.h"
#include "sysutil.h"
#include "message.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "input.h"
#include "lptutil.h"
#include "print.h"
#include "tranutil.h"
#include "record.h"
#include "settle.h"
#include "menu.h"
#include "testrans.h"
#include "debugstis.h"
#include "testcfg.h"
#include "adjust.h"
#include "func.h"
#include "keytrans.h"
#include "tpadtest.h"
#include "camtest.h"
#include "network.h"

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------
static const BYTE KAppvCode[]  = {"APPR CODE:"};

// Default manager Function Menu
static const struct MENU_ITEM KFuncItem [] =
{
  {  1, "Display Batch"},
  {  5, "Display Total"},
  {  6, "TMS FTP Setup"},       //29-09-16 JC ++
  {  7, "TMS FTP Dnload"},      //29-09-16 JC ++
  {  9, "Clear Reversal"},
  { 21, "Trans Detail"},
 // { 22, "Adjust Tips"},
 { 50, "Set Demo EDC"},
 { 51, "Set Demo CTL"},
 // { 52, "Load TMK & Keys"},
 // { 53, "Encr Data"},
 // { 54, "Decr Data"},
//  { 55, "Get PIN"},
 // { 56, "Reset Keys"},
 // { 57, "Set Dukpt Host Idx"},
//  { 58, "Load Dukpt Keys"},
 // { 59, "Get Dukpt PIN"},
 // { 60, "Reset Dukpt Keys"},
 // { 61, "ICC TMK Inject"},
//  { 62, "Signpad Test"},
 // { 63, "Barcode Test"},
  { 64, "Reboot"},
  { 65, "Config report"},
  { 66, "Show IP Addr"},
  #if (LPT_SUPPORT|TMLPT_SUPPORT)    // Conditional Compile for Printer support
  { 72, "Reprint Last"},
  { 73, "Reprint Any"},
  { 74, "Summary Rep"},
  { 75, "Details Rep"},
  #endif                    // PRINTER_SUPPORT
  { 96, "Test Trans"},
  { 99, "Clear Batch"},
  {-1, NULL},
};

static const struct MENU_DAT KFuncMenu  = {
  "EDC Functions",
  KFuncItem,
};
//*****************************************************************************
//  Function        : DispTransData
//  Description     : Display transaction record.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispTransData(struct TXN_RECORD *aRec)
{
  static const BYTE KTraceNum[]   = {"TRACE NUM:"};
  static const BYTE KVoided[]     = {"VOIDED "};
  BYTE buffer[MW_MAX_LINESIZE+1], *ptr;
  DWORD var_j;

  DispLineMW(KTraceNum, MW_LINE1, MW_CLRDISP|MW_SMFONT);
  split(buffer,aRec->sb_trace_no, 3);
  buffer[6] = 0;
  DispLineMW(buffer, MW_LINE1, MW_RIGHT|MW_SMFONT);
  if (aRec->b_trans_status & ADJUSTED)
    var_j = ADJUST;
  else
    var_j = aRec->b_trans;

  if ((aRec->b_trans_status & VOIDED) != 0) {
    memcpy(buffer, KVoided, sizeof(KVoided));
    DispLineMW(buffer, MW_LINE2, MW_SMFONT);
  }
  ptr = buffer;
  memcpy(ptr, GetConstMsg(KTransHeader[var_j])+4, 12);
  ptr += 12;
  *ptr++ = 0;
  DispLineMW(buffer, MW_LINE2, MW_RIGHT|MW_SMFONT);

  DispAmount(aRec->dd_amount, MW_LINE3, MW_SMFONT);

  switch (aRec->b_entry_mode) {
    case ICC:
      buffer[0] = 'C';
      break;
    case MANUAL:
      buffer[0] = 'M';
      break;
    case SWIPE:
      buffer[0] = 'S';
      break;
    case FALLBACK:
      buffer[0] = 'F';
      break;
    case CONTACTLESS:
      buffer[0] = 'T';
      break;
    default:
      buffer[0] = ' ';
      break;
  }
  split(&buffer[1],aRec->sb_pan, 10);
  var_j = (BYTE) fndb(&buffer[1], 'F', 19);
  if (MaskCardNo())
    memset(&buffer[7], 'X', 6);
  buffer[var_j+1] = 0;
  DispLineMW(buffer, MW_LINE4, MW_CLREOL|MW_SPFONT);
}
//*****************************************************************************
//  Function        : DispReversal
//  Description     : Display reversal batch record.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispReversal(void)
{
  static const BYTE KTraceNum[] = {"TRACE NUM:"};
  static const BYTE KVoided[]   = {"VOIDED "};
  BYTE i, var_j;
  BYTE tmp[MW_MAX_LINESIZE+1];
  BOOLEAN no_reversal;
  WORD max_acq = APM_GetAcqCount();

  no_reversal = TRUE;
  for (i=0;i<max_acq; i++) {
    if (APM_GetPending(i) != REV_PENDING)
      continue;
    APM_GetAcqTbl(i, &STIS_ACQ_TBL(0));
    if (!CorrectHost(GetHostType(0)))
      continue;

    no_reversal = FALSE;
    APM_GetRevRec(i, &RECORD_BUF,sizeof(RECORD_BUF));

    DispLineMW("REVERSAL", MW_LINE1, MW_REVERSE|MW_CENTER|MW_SMFONT);
    DispLineMW("HOST: ", MW_LINE2, MW_SMFONT);
    memcpy(tmp, STIS_ACQ_TBL(0).sb_name, 10);
    tmp[10] = 0;
    DispLineMW(tmp, MW_LINE2+6, MW_SMFONT);
    DispLineMW(KTraceNum, MW_LINE3, MW_SMFONT);
    split(tmp,RECORD_BUF.sb_roc_no, 3);
    tmp[6]= 0;
    DispLineMW(tmp, MW_LINE3+10, MW_SMFONT);
    if (RECORD_BUF.b_trans_status & ADJUSTED)
      var_j = ADJUST;
    else
      var_j = RECORD_BUF.b_trans;
    memcpy(tmp, GetConstMsg(KTransHeader[var_j]), 16);
    tmp[16]=0;
    DispLineMW(&tmp[4], MW_LINE4+4, MW_SMFONT);

    if ((RECORD_BUF.b_trans_status & VOIDED) != 0) {
      DispLineMW(KVoided, MW_LINE4, MW_SMFONT);
    }
    DispAmount(RECORD_BUF.dd_amount, MW_LINE5, MW_SMFONT);

    split(tmp,RECORD_BUF.sb_pan, 10);
    var_j = (BYTE) fndb(tmp, 'F', 19);
    tmp[var_j] = 0;
    DispLineMW(tmp, MW_LINE6, MW_RIGHT|MW_SPFONT);

    ConvDateTime(tmp, &RECORD_BUF.s_dtg, FALSE);
    tmp[16] = 0;
    DispLineMW(tmp, MW_LINE7, MW_SMFONT);
    memcpy(tmp, RECORD_BUF.sb_ecr_ref, 16);
    DispLineMW(tmp, MW_LINE8, MW_SMFONT);
    APM_WaitKey(KBD_TIMEOUT,0);
  }
  if (no_reversal) {
    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg(GetConstMsg(EDC_FN_NO_REV), MW_LINE3, MW_BIGFONT);
    APM_WaitKey(KBD_TIMEOUT,0);
  }
}
//*****************************************************************************
//  Function        : ClearReversal
//  Description     : Clear acquirer reversal flag.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClearReversal(void)
{
  int i;

  DispLineMW("CLEAR REVERSAL", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  if (!APM_GetAccessCode())
    return;

  while (1) {
    if ((i = APM_SelectAcquirer(FALSE)) == -1)
      return;
    DispLineMW("CLEAR REVERSAL", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
    if (APM_GetPending(i) != REV_PENDING) {
      Disp2x16Msg(GetConstMsg(EDC_FN_NO_REV), MW_LINE3, MW_CENTER|MW_BIGFONT);
      APM_WaitKey(KBD_TIMEOUT,0);
      break;
    }
    else {
      DispLineMW(GetConstMsg(EDC_FN_CLR_REV), MW_LINE5, MW_CENTER|MW_BIGFONT);
      switch (APM_YesNo()) {
        case 0 :
          return;
        case 2 :
          APM_SetPending(i, NO_PENDING);
          DispLineMW(GetConstMsg(EDC_FN_REV_CLEAR), MW_LINE5, MW_CENTER|MW_BIGFONT);
          ErrorDelay();
      }
    }
  }
}
//*****************************************************************************
//  Function        : DispStatusMsg
//  Description     : Display status message
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispStatusMsg(BYTE *aMsg)
{
  Disp2x16Msg(aMsg, MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  Delay1Sec(1, 1);
}
//*****************************************************************************
//  Function        : DisplayBatch
//  Description     : Display batch record.
//  Input           : record index;     // display with specify record index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int DisplayRecordno(DWORD aRecIdx)
{
  static const BYTE KRRN[] = {"RRN:"};
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  int rec_cnt, rec_idx;
  DWORD keyin;

  rec_cnt = APM_GetRecCount();
  if (rec_cnt == 0) {
    RSP_DATA.w_rspcode = 'N'*256+'T';
    DispCtrlMW(MW_CLR_DISP);
    DispRspText(FALSE);
    ErrorDelay();
    return -1;
  }

  if (aRecIdx == -1) {
    DispLineMW(GetConstMsg(EDC_FN_DISP_BATCH), MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
    rec_idx = SearchRecord(FALSE);
    if (rec_idx == -1)
      return -1;
  }
  else
    rec_idx = rec_cnt - 1;

  while (1) {
    APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
    APM_GetAcqTbl(RECORD_BUF.w_host_idx, &STIS_ACQ_TBL(0));
    if (!CorrectHost(GetHostType(0))) {
      if (rec_idx == 0) {
        DispStatusMsg(GetConstMsg(EDC_FN_START_BATCH));
       return -1;
      }   
    }
    APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(0));
    DispTransData(&RECORD_BUF);
    DispLineMW(KAppvCode, MW_LINE5, MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_auth_code, sizeof(RECORD_BUF.sb_auth_code));
    DispLineMW(tmpbuf, MW_LINE5, MW_RIGHT|MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    ConvDateTime(tmpbuf, &RECORD_BUF.s_dtg, FALSE);
    tmpbuf[16] = 0;
    DispLineMW(tmpbuf, MW_LINE6, MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_ecr_ref, sizeof(RECORD_BUF.sb_ecr_ref));
    DispLineMW(tmpbuf, MW_LINE7, MW_CENTER|MW_SMFONT);
    DispLineMW(KRRN, MW_LINE8, MW_SPFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_rrn, sizeof(RECORD_BUF.sb_rrn));
    DispLineMW(tmpbuf, MW_LINE8+4, MW_SPFONT);
    keyin=APM_WaitKey(KBD_TIMEOUT,0);
    if (keyin == MWKEY_CANCL)
      return -1;
    if (keyin == MWKEY_ENTER)
      return rec_idx;
  }
}
//*****************************************************************************
//  Function        : DisplayBatch
//  Description     : Display batch record.
//  Input           : record index;     // display with specify record index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DisplayBatch(DWORD aRecIdx)
{
  static const BYTE KRRN[] = {"RRN:"};
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  int rec_cnt, rec_idx;
  DWORD keyin;

  rec_cnt = APM_GetRecCount();
  if (rec_cnt == 0) {
    RSP_DATA.w_rspcode = 'N'*256+'T';
    DispCtrlMW(MW_CLR_DISP);
    DispRspText(FALSE);
    ErrorDelay();
    return;
  }

  if (aRecIdx == -1) {
    DispLineMW(GetConstMsg(EDC_FN_DISP_BATCH), MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
    rec_idx = SearchRecord(FALSE);
    if (rec_idx == -1)
      return;
  }
  else
    rec_idx = rec_cnt - 1;

  while (1) {
    APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
    APM_GetAcqTbl(RECORD_BUF.w_host_idx, &STIS_ACQ_TBL(0));
    if (!CorrectHost(GetHostType(0))) {
      if (rec_idx == 0) {
        DispStatusMsg(GetConstMsg(EDC_FN_START_BATCH));
        break;
      }
      else
        rec_idx -= 1;
      continue;
    }
    APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(0));
    DispTransData(&RECORD_BUF);
    DispLineMW(KAppvCode, MW_LINE5, MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_auth_code, sizeof(RECORD_BUF.sb_auth_code));
    DispLineMW(tmpbuf, MW_LINE5, MW_RIGHT|MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    ConvDateTime(tmpbuf, &RECORD_BUF.s_dtg, FALSE);
    tmpbuf[16] = 0;
    DispLineMW(tmpbuf, MW_LINE6, MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_ecr_ref, sizeof(RECORD_BUF.sb_ecr_ref));
    DispLineMW(tmpbuf, MW_LINE7, MW_CENTER|MW_SMFONT);
    DispLineMW(KRRN, MW_LINE8, MW_SPFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_rrn, sizeof(RECORD_BUF.sb_rrn));
    DispLineMW(tmpbuf, MW_LINE8+4, MW_SPFONT);
    keyin=APM_WaitKey(KBD_TIMEOUT,0);
    if (keyin == MWKEY_CANCL)
      break;
    if ((keyin == MWKEY_ENTER)||(keyin == MWKEY_UP)) {
      if (rec_idx == 0) {
        DispStatusMsg(GetConstMsg(EDC_FN_START_BATCH));
      }
      else
        rec_idx -= 1;
    }
    if ((keyin ==MWKEY_CLR)||(keyin == MWKEY_DN)) {
      if (rec_idx == (rec_cnt - 1))
        DispStatusMsg(GetConstMsg(EDC_FN_END_BATCH));
      else
        rec_idx += 1;
    }
  }
}
//*****************************************************************************
//  Function        : ClearBatch
//  Description     : Clear EDC batch record.
//  Input           : aHeader;          // Header Line.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClearBatch(void)
{
  int i;
  WORD max_acq;

  DispCtrlMW(MW_CLR_DISP);
  if (!APM_GetAccessCode())
    return;
  DispClrBelowMW(MW_LINE3);
  DispLineMW(GetConstMsg(EDC_FN_CLR_BATCH), MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (APM_YesNo() == 2) {
    DispClrBelowMW(MW_LINE3);
    INPUT.w_host_idx = -1;
    SetDefault();
    max_acq = APM_GetAcqCount();
    for (i = 0; i < max_acq; i++) {
      if (!APM_GetAcqTbl(i, &STIS_ACQ_TBL(0)))
        break;
      if (CorrectHost(GetHostType(0))) {
        APM_SetPending(i, NO_PENDING);
        APM_BatchClear(i);
      }
    }
    fCommitAllMW();
    DispLineMW(GetConstMsg(EDC_FN_BATCH_ERASE), MW_LINE5, MW_CENTER|MW_BIGFONT);
    AcceptBeep();
    Delay1Sec(10, 0);
  }
}
//*****************************************************************************
//  Function        : DispTransTotal
//  Description     : Display transaction totals.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispTransTotal(void)
{
  static const BYTE KSale[]    = {"SALE:"};
  static const BYTE KRefund[]  = {"REFUND:"};
  BYTE buffer[5];
  int host_idx;
  BOOLEAN all_acq;

  do {
    all_acq=FALSE;
    host_idx = APM_SelectAcquirer(TRUE);
    if (host_idx == -1)
      break;
    if (host_idx == -2) {
      all_acq = TRUE;
      INPUT.w_host_idx = 0;
    }
    else {
      INPUT.w_host_idx = (WORD) host_idx;
      APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0));
    }

    CalTotals(all_acq);
    bin2dec(TERM_TOT.s_sale_tot.w_count, buffer, 3);
    buffer[3] = 0;
    DispLineMW(KSale, MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispLineMW(buffer, MW_LINE1, MW_RIGHT|MW_BIGFONT);
    DispAmount(TERM_TOT.s_sale_tot.dd_amount, MW_LINE3, MW_BIGFONT);

    bin2dec(TERM_TOT.s_refund_tot.w_count, buffer, 3);
    buffer[3] = 0;
    DispLineMW(KRefund, MW_LINE5, MW_BIGFONT);
    DispLineMW(buffer, MW_LINE5, MW_RIGHT|MW_BIGFONT);
    DispAmount(TERM_TOT.s_refund_tot.dd_amount, MW_LINE7, MW_BIGFONT);

    if (APM_WaitKey(KBD_TIMEOUT, 0) == MWKEY_CANCL)
      break;
  } while (1);
}
//*****************************************************************************
//  Function        : ReprintTxn
//  Description     : Reprint specified receipt.
//  Input           : aLastBatch;     // TRUE=>Last Batch Record
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ReprintTxn(int aLastBatch)
{
  int rec_cnt, rec_idx;
rec_idx = aLastBatch;

  APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
  APM_GetAcqTbl(RECORD_BUF.w_host_idx, &STIS_ACQ_TBL(0));
  APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(0));
  PackRecordP(TRUE);
   PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE, FALSE);
}
//*****************************************************************************
//  Function        : PrintTotals
//  Description     : Print Transaction total information.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintTotals(void)
{
  DispLineMW(GetConstMsg(EDC_FN_CARD_TOTAL), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  MsgBufSetup();
  PackLogo();
 // pack_lfs(1);
  PackIssuerSum(TRUE);
  PackFF();
  PackMsgBufLen();
  PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE, FALSE);
}
//*****************************************************************************
//  Function        : PrintTxnRecord
//  Description     : Print transaction Log.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintTxnRecord(void)
{
  static const BYTE c_nii[] = {"NII "};
  static const BYTE KBase[] = {"BASE  "};
  static const BYTE c_tip[] = {"TIP   "};
  DDWORD amount;
  WORD line_cnt, rec_cnt;
  BYTE tmp[21];
  BYTE buf[21];
  DWORD keyin, i = 0;
  struct DATETIME dtg;
  struct DISP_AMT disp_amt;
  int  host_idx;

  host_idx = APM_SelectAcquirer(FALSE);
  if (host_idx == -1)
    return;

  APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0));
  DispLineMW(GetConstMsg(EDC_FN_CARD_REC), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  MsgBufSetup();

PackLogo();
 // pack_lfs(2);
  pack_str("POS/Terminal ID: ");
  pack_mem(STIS_ACQ_TBL(0).sb_term_id, 8);
  pack_lf();
  pack_str("MERCHANT ID/MID: ");
  pack_mem(STIS_ACQ_TBL(0).sb_acceptor_id, 8);
  pack_lf();
   pack_str("BATCH: ");
  split_data(STIS_ACQ_TBL(0).sb_curr_batch_no, 3);
  pack_lf();
  pack_str("LE/EGP BATCH");
  pack_lf();
  PackCurrentDate();
  //pack_lfs(2);

  pack_str("\x1bW1 DETAILS REPORT\x1bW0");
  pack_lf();
  PackSeperator('=');
  //pack_lfs(2);
 

  //pack_lf();
 // pack_str("TRACE#  APP.CODE   TRANS  TIME");
 // pack_lf();
  //pack_str("ACCT #               EXP. DATE");
 // pack_lf();
 // pack_str("AMOUNT                    DATE");
 // pack_lf();
 // PackSeperator('-');

  line_cnt = 18;
  rec_cnt = APM_GetRecCount();
  for (i = 0; i < rec_cnt; i++) {
    APM_GetBatchRec(i, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
    if (RECORD_BUF.w_host_idx != host_idx)
      continue;

    if (line_cnt > 80) {
      PackMsgBufLen();
      PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, TRUE, FALSE);
      MsgBufSetup();
      line_cnt = 0;
    }
	// pack_str("Record No.");
    split_data(RECORD_BUF.sb_roc_no, 3);
    pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-6-11+2));
	    ByteCopy((BYTE*) &dtg, (BYTE*)&RECORD_BUF.s_dtg, sizeof(struct DATETIME));
		 ConvDateTimeT(tmp, &dtg, TRUE);
	 pack_mem(tmp,11);
    pack_lf();

pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-16)/2);
  memset(tmp,0, sizeof(tmp));
    // PAN
    split(tmp, RECORD_BUF.sb_pan, 8);
   // if (MaskCardNo())
	 memset(&tmp[6],'*', 6);     // mask PAN
	sprintf(buf, "%.4s %.4s %.4s %.4s", tmp, &tmp[4],&tmp[8],&tmp[12]);
    pack_str(buf);
    pack_lf();

	APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(1));
  
	memset(buf, 0x20, sizeof(buf));
	sprintf(buf, "%s", STIS_ISS_TBL(1).sb_card_name);
	pack_mem(buf, 10);
	pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-10-3-8+1)/2);
	pack_str("MAG");
	pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-10-3-8+1)/2);
    pack_str("EXP:");
    split_data((BYTE *)&RECORD_BUF.sb_exp_date[1], 1);
	pack_str("/");
    split_data((BYTE *)&RECORD_BUF.sb_exp_date[0],1);
    pack_lf();

    //pack_mem(RECORD_BUF.sb_auth_code,6);
  //  pack_space(1);

    keyin = RECORD_BUF.b_trans;
    if (RECORD_BUF.b_trans_status & ADJUSTED) {
      keyin = ADJUST;
    }
	memset(tmp, 0x20, sizeof(tmp)); 
    memcpy(tmp, GetConstMsg(KTransHeader[keyin]), 16);
   // memmove(tmp, &tmp[6], 10);
   // if (RECORD_BUF.b_trans_status & VOIDED) {
  //    for (keyin=1;keyin<10;keyin++) {
   //     if (tmp[keyin]!=' ') {
   //       tmp[keyin-1]='V';
    //      break;
   ///     }
   ///   }
   // }
    pack_mem(tmp, 16);
	if (RECORD_BUF.b_trans_status & VOIDED) {
		pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-12-2-4));
		 pack_str("VOID");
	}
	pack_lf();
    //pack_space(1);

   // ByteCopy((BYTE*) &dtg, (BYTE*)&RECORD_BUF.s_dtg, sizeof(struct DATETIME));
   // ConvDateTime(tmp, &dtg, TRUE);
   // pack_mem(&tmp[12],6);
   // pack_lf();

   // split(get_pptr(),RECORD_BUF.sb_pan, 10);
  //  inc_pptr(keyin = (BYTE)fndb(get_pptr(), 'F', 19));
   // pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-keyin-4));
  //  split_data(&RECORD_BUF.sb_exp_date[1], 1);
  //  split_data(&RECORD_BUF.sb_exp_date[0],1);
  //  pack_lf();

//	pack_str("AMOUNT ");
	 pack_mem(gCurInfo[RECORD_BUF.currency].name, 3); 
    ConvAmount(RECORD_BUF.dd_amount, &disp_amt, STIS_TERM_CFG.b_decimal_pos, STIS_TERM_CFG.b_currency);
    pack_mem(disp_amt.content, disp_amt.len);
    pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-9-6+2));
   // pack_mem(tmp, 6);
    pack_mem(RECORD_BUF.sb_auth_code,6);
   // pack_space(1);
    pack_lf();
/*
    if (SaleType((BYTE)(RECORD_BUF.b_trans)) && TIPsReqd()) {
      amount = RECORD_BUF.dd_amount - RECORD_BUF.dd_tip;
      pack_mem((BYTE *)KBase, 6);
      pack_space(6);
      PackAmt(amount, 19);
      pack_lf();
      pack_mem((BYTE *)c_tip, 6);
      pack_space(6);
      PackAmt(RECORD_BUF.dd_tip, 19);
      pack_lf();
    }
    pack_str("ECR/INV NO:");
    if (skpb(RECORD_BUF.sb_ecr_ref,' ', 16)==16)
      pack_str(" NIL");
    else
      pack_mem(RECORD_BUF.sb_ecr_ref,16);
  
pack_lf();
*/

	pack_lfs(2);
    line_cnt += 7;
  }
PackSeperator('*');
pack_lf();

  PackIssuerSumT(host_idx);
  PackFF();
  PackMsgBufLen();
   PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE, FALSE);
}
extern void ReferralCall(void);
//*****************************************************************************
//  Function        : MerchantFunc
//  Description     : Process function base on the input function id.
//  Input           : aFuncId;        // Function Id
//  Return          : TRUE;           // indicate func process complete
//                    FALSE;          // func not define.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MerchantFunc(DWORD aFuncId)
{
  int select;
  int recordno;

  select = aFuncId;
  if (aFuncId == -1) {
    select = 0;
    select = MenuSelect(&KFuncMenu, select);
  }

  gGDS->b_disp_chgd = TRUE;
  if (select == -1)
    return TRUE;

  switch (select) {
    case 1:
      DisplayBatch(0);  // Start from last rec
      break;
    case 5:
      DispTransTotal();
      break;
	    case  6:
        APM_MerchFunc(7);
        break;
		 case  7:
        APM_MerchFunc(8);
        break;
    case 8:
      DispReversal();
      break;
    case 9 :
      ClearReversal();
      break;
    case 21:
      DisplayBatch(-1);  // Prompt User for TRACE
      break;
    case 22:
      AdjustTrans(-1);
      break;
      //case 23:
      //if (TIPsReqd())
      //  AdjustTrans(TRUE);   /* adjust base amount */
      //  break;
      // debug only ++

    case 50:
      DispLineMW("Reset DEMO Data", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      DispLineMW("PROCESSING...",  MW_LINE5, MW_CENTER|MW_BIGFONT);

      // Set STIS data
      SaveSTISDbgData(MODE_ALL_PARAM);
      fCommitAllMW();
      LoadEMVKey();
      LoadEMVCfg();
      DispClrBelowMW(MW_LINE5);
      DispLineMW("Enter to Reboot!", MW_LINE5, MW_CENTER|MW_BIGFONT);
      APM_WaitKey(9000, 0);
      fCommitAllMW();
      APM_MerchFunc(64);
      break;
    case 51:
	  DispLineMW("Reset CTL Data", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      DispLineMW("PROCESSING...",  MW_LINE5, MW_CENTER|MW_BIGFONT);
      // Set EMVCL data
      LoadCTLKey();
      LoadCTLCfg();
      CTLEnable(TRUE);
      break;
    case 52:
      if (LoadTmk() == TRUE)
        LoadKeys();
      break;
    case 53:
      EncData();
      break;
    case 54:
      DecData();
      break;
    case 55:
      GetPIN();
      break;
    case 56:
      ResetKey(0xFF);
      break;
    case 57:
      SetDhostIdx();
      break;
    case 58:
      LoadDukptIkey();
      break;
    case 59:
      GetDukptPin();
      break;
    case 60:
      ResetDukptIkey();
      break;
    case 61:
      IccTmkInject();
      break;
    case 62:
      SPadTest();
      break;
    case 63:
      //BRdrTest();
      CamTestAll();
      break;
    case 64:
      fCommitAllMW();
      APM_MerchFunc(select);
      break;
	  case 65:
       Packprintconfig();
	   Packprintissuer();
      break;
    case 66:
      ShowPPPStatus();
      break;
     #if (LPT_SUPPORT|TMLPT_SUPPORT)    // Conditional Compile for Printer support
    case 72:
      ReprintLast();
      break;
    case 73:
      recordno = DisplayRecordno(-1);
	  if(recordno == -1){
	  break;
	  }
      ReprintTxn(recordno);
      break;
    case 74:
      PrintTotals();
      break;
    case 75:
      PrintTxnRecord();
      break;
    #endif                    // PRINTER_SUPPORT
    case 96:
      return TestTrans();
    case 99:
      ClearBatch();
      break;
    default:
      RefreshDispAfter(0);
      return(FALSE);
  }
  return(TRUE);
}

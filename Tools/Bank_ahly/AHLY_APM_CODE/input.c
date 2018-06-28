//-----------------------------------------------------------------------------
//  File          : input.c
//  Module        :
//  Description   : Input related functions.
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
#include "coremain.h"
#include "chkoptn.h"
#include "constant.h"
#include "emvtrans.h"
#include "message.h"
#include "tranutil.h"
#include "util.h"
#include "ecr.h"
#include "tlvutil.h"
#include "sale.h"
#include "saleoffl.h"
#include "auth.h"
#include "refund.h"
#include "emvtrans.h"
#include "ctltrans.h"
#include "iface.h"
#include "input.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
struct PRODUCT {
  BOOLEAN b_selected;
  struct DESC_TBL s_prod;
};

#define SEPARATOR_5E   '^'

//-----------------------------------------------------------------------------
//    Constant
//-----------------------------------------------------------------------------
static const DWORD KDecimalPos[4] = { DECIMAL_NONE, DECIMAL_POS1, DECIMAL_POS2, DECIMAL_POS3};

//-----------------------------------------------------------------------------
//      Globals Variables
//-----------------------------------------------------------------------------


//*****************************************************************************
//  Function        : Trk1Ready
//  Description     : Return TRUE when TRK1 data ready.
//  Input           : N/A
//  Return          : TRUE;  // trk1 data ready.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN Trk1Ready(void)
{
  return ((INPUT.s_trk1buf.b_len != 0) && (INPUT.s_trk1buf.b_len != 0xFF));
}
//*****************************************************************************
//  Function        : Trk2Ready
//  Description     : Return TRUE when TRK2 data ready.
//  Input           : N/A
//  Return          : TRUE;  // trk2 data ready.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN Trk2Ready(void)
{
  return ((INPUT.s_trk2buf.b_len != 0) && (INPUT.s_trk2buf.b_len != 0xFF));
}
//*****************************************************************************
//  Function        : FindSeperator
//  Description     : Find card data seperator & return its position.
//  Input           : aBuf;             // Buffer pointer
//                    aIdx;             // start position.
//  Return          : seperator's pos.
//                    0xFF;    // not found
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BYTE FindSeperator(BYTE *aBuf, BYTE aIdx)
{
  BYTE val;
  BYTE i;

  for (i = 0; i != 22 + 1; i++) {
    val = aBuf[i+aIdx];
    //  *, ;, =, ^
    if ((val == 0x2a) || (val == 0x3b) || (val == 0x3d) || (val == 0x5e))
      return i;
  }
  return 0xff;
}
//*****************************************************************************
//  Function        : CardServiceCode
//  Description     : Return TRACK-2 SERVICE CODE.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : Track 1 & 2 must be checked before calling this func
//                  : n/a
//  Globals Changed : N/A;
//*****************************************************************************
static BYTE CardServiceCode(void)
{
  BYTE len;
  BYTE scode = 0;

  // get service code
  if ((INPUT.s_trk2buf.b_len == 0xff) || (INPUT.s_trk2buf.b_len == 0)) {
    // track 2 missing, get it from track 1
    len = (BYTE) fndb(INPUT.s_trk1buf.sb_content, (BYTE)SEPARATOR_5E, 22);
    if (len < 22) {
      scode = (BYTE) fndb(&INPUT.s_trk1buf.sb_content[len+1], (BYTE)SEPARATOR_5E, 30);
      if (scode < 30)
        scode = INPUT.s_trk1buf.sb_content[len+scode+2]; // SERVICE CODE POSITION
      else
        scode = 0;
    }
  }
  else{
    len = (BYTE) fndb(INPUT.s_trk2buf.sb_content, SEPERATOR2, 20);
    if (len < 20)
      scode = INPUT.s_trk2buf.sb_content[len+5]; // SERVICE CODE POSITION
  }
  return scode;
}
//*****************************************************************************
//  Function        : ExtractPAN
//  Description     : Extract PAN & expiry data from track data.
//  Input           : aPAN;     // pointer to 10 bytes BCD PAN
//                              // with packed trailing F;
//                    aExpDate; // pointer 2 bytes BCD expiry date
//  Return          : PAN len;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD ExtractPAN(BYTE *aPAN, BYTE *aExpDate)
{
  DWORD idx;
  DWORD len;
  BYTE tmp[20];

  if (INPUT.s_trk2buf.b_len==0) {
    for (idx=0;idx < INPUT.s_trk1buf.b_len; idx++) {
      if (INPUT.s_trk1buf.sb_content[idx] == 'B')
        break;
    }

    idx++; // start of card #
    len = 0;
    memset(tmp, 'F', sizeof(tmp));
    while (TRUE) {
      if ((INPUT.s_trk1buf.sb_content[idx] == 0x2a) || (INPUT.s_trk1buf.sb_content[idx] == 0x3b) ||
          (INPUT.s_trk1buf.sb_content[idx] == 0x3d) || (INPUT.s_trk1buf.sb_content[idx] == 0x5e) ||
          (idx > 20)
         )
        break;
      else {
        if ((INPUT.s_trk1buf.sb_content[idx] >= '0') && (INPUT.s_trk1buf.sb_content[idx] <= '9'))
          tmp[len++] = INPUT.s_trk1buf.sb_content[idx];
      }
      idx++;
      if (idx >= INPUT.s_trk1buf.b_len)  // !2006-02-17
        break;
    }
    compress(aPAN, tmp, (len+1)/2);
    idx++;
    for ( ;idx < 76; idx++) {
      if (INPUT.s_trk1buf.sb_content[idx] == 0x5e)
        break;
    }
    compress(aExpDate, &INPUT.s_trk1buf.sb_content[idx+1], 2);
  }
  else {
    len = fndb(INPUT.s_trk2buf.sb_content, SEPERATOR2, INPUT.s_trk2buf.b_len);
    if (len <= 19) {
      INPUT.s_trk2buf.sb_content[len] = 0x3f; /* cater for odd len */
      memset(aPAN, 0xFF, 10);
      compress(aPAN, INPUT.s_trk2buf.sb_content, (BYTE)((len+1)/2));
      INPUT.s_trk2buf.sb_content[len] = SEPERATOR2; /* cater for odd len */
      compress(aExpDate, &INPUT.s_trk2buf.sb_content[1+len], 2);
    }
    else
      len = 0;
  }
  return (len);
}
//*****************************************************************************
//  Function        : InCardTable
//  Description     : Check card exist in table.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN InCardTable(void)
{
  BYTE card_idx, issuer_idx, acq_idx;
  BYTE pan_len, pan[21];
  WORD max_card   = APM_GetCardCount();
  WORD max_issuer = APM_GetIssuerCount();
  WORD max_acq    = APM_GetAcqCount();


  split(&pan[1], INPUT.sb_pan, 10);
  pan[0] = (BYTE) fndb(&pan[1], 'F', 19);

  for (card_idx = 0; card_idx < max_card; card_idx++) {
    if (!APM_GetCardTbl(card_idx, &STIS_CARD_TBL(0)))
      continue;
    pan_len = (BYTE)bcd2bin(STIS_CARD_TBL(0).b_pan_len);
    if ((memcmp(INPUT.sb_pan, STIS_CARD_TBL(0).sb_pan_range_low, 5) >= 0)  &&
        (memcmp(INPUT.sb_pan, STIS_CARD_TBL(0).sb_pan_range_high, 5) <= 0) &&
        ((pan[0] == pan_len) || (pan_len == 0))) {
      // locate issuer
      for (issuer_idx = 0; issuer_idx < max_issuer; issuer_idx++) {
        if (!APM_GetIssuer(issuer_idx, &STIS_ISS_TBL(0)))
          continue;
        if (STIS_ISS_TBL(0).b_id == STIS_CARD_TBL(0).b_issuer_id)
          break;
      }
      if (issuer_idx == max_issuer)
        continue;
      // locate acquirer
      for (acq_idx = 0; acq_idx < max_acq; acq_idx++) {
        if (!APM_GetAcqTbl(acq_idx, &STIS_ACQ_TBL(0)))
          continue;
        if (STIS_ACQ_TBL(0).b_status == NOT_LOADED)
          continue;
        if (!CorrectHost(GetHostType(0)))
          continue;
        if (STIS_ACQ_TBL(0).b_id == STIS_CARD_TBL(0).b_acquirer_id)
          break;
      }
      if (acq_idx == max_acq)
        continue;
      // found
      INPUT.w_host_idx   = acq_idx;
      INPUT.w_issuer_idx = issuer_idx;
      return TRUE;
    }
  }
  DispErrorMsg(GetConstMsg(EDC_RSP_UNSUPPORT_CARD));
  RSP_DATA.w_rspcode = 'U'*256+'C';
  return FALSE;
}
//*****************************************************************************
//  Function        : ValidCard
//  Description     : Check card format.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ValidCard(void)
{
  BYTE idx, tlen;
  BYTE disp_pan[21];

  memset(INPUT.sb_holder_name, ' ', sizeof(INPUT.sb_holder_name));

  if (INPUT.b_entry_mode == MANUAL || INPUT.b_entry_mode == ICC) {
    if (!InCardTable())
      return FALSE;
    if (CheckPAN()) {
      split(&disp_pan[1], INPUT.sb_pan, 10);
      disp_pan[0] = (BYTE)fndb(&disp_pan[1], 'F', 19);
      if (!chk_digit_ok(&disp_pan[1], disp_pan[0])) {
        DispErrorMsg(GetConstMsg(EDC_IN_CHK_DIGIT_ERROR));
        return FALSE;
      }
    }
    if ((INPUT.b_entry_mode == ICC)&&EMVProcDisable()) {
      DispErrorMsg(GetConstMsg(EDC_IN_SWIPE_CARD));
      return FALSE;
    }
    if (INPUT.b_entry_mode == ICC) {
      if (!ValidEMVData()) {
        DispErrorMsg(GetConstMsg(EDC_RSP_READ_ERROR));
        return FALSE;
      }
    }
    return TRUE;
  }

  if (!Trk2Ready() && !Trk1Ready()) {
    DispErrorMsg(GetConstMsg(EDC_RSP_READ_ERROR));
    return FALSE;
  }
  if (!Trk2Ready())
    INPUT.s_trk2buf.b_len = 0;
  if (!Trk1Ready())
    INPUT.s_trk1buf.b_len = 0;

  memset(INPUT.sb_pan, 0xFF, 10);
  disp_pan[0] = (BYTE) ExtractPAN(INPUT.sb_pan, INPUT.sb_exp_date);

  if (!InCardTable())
    return FALSE;


  if ((INPUT.b_entry_mode!=ICC)&&(INPUT.b_entry_mode!=FALLBACK)&&!EMVProcDisable()) {
    idx = CardServiceCode();
    if (idx == 0) {   // err chk: should be ASCII data
      DispErrorMsg(GetConstMsg(EDC_RSP_INVALID_CARD));
      return FALSE;
    }
    if ((idx == '2') || (idx == '6')) {
      if (INPUT.b_entry_mode != FALLBACK) {
        DispErrorMsg(GetConstMsg(EDC_IN_USE_ICC));
        return FALSE; // EMV SMC
      }
    } else
      INPUT.b_entry_mode = SWIPE;
  }
  if (CheckPAN()) {
    split(&disp_pan[1], INPUT.sb_pan, 10);
    if (!chk_digit_ok(&disp_pan[1], disp_pan[0])) {
      DispErrorMsg(GetConstMsg(EDC_IN_CHK_DIGIT_ERROR));
      return FALSE;
    }
  }
  if ((INPUT.s_trk1buf.b_len != 0) && (INPUT.s_trk1buf.b_len != 0xFF)) {
    idx = FindSeperator(INPUT.s_trk1buf.sb_content, 0) + 1;
    if (idx) {
      tlen = FindSeperator(INPUT.s_trk1buf.sb_content, idx);
      if ((tlen == 0xFF) || (tlen > sizeof(INPUT.sb_holder_name)))
        tlen = sizeof(INPUT.sb_holder_name);
      //FormatHolderName(INPUT.sb_holder_name, &INPUT.s_trk1buf.sb_content[idx], (BYTE)tlen);
      memcpy(INPUT.sb_holder_name, &INPUT.s_trk1buf.sb_content[idx], (BYTE)tlen);
    }
  }
  if (INPUT.s_trk2buf.b_len == 0)
    return TRUE;
  else if (isbdigit(&INPUT.s_trk2buf.sb_content[1+disp_pan[0]], 4))
    return TRUE;

  DispErrorMsg(GetConstMsg(EDC_IN_INVALID_CARD));
  return FALSE;
}
//*****************************************************************************
//  Function        : PromptYesNo
//  Description     : Prompt user to Confirm.
//  Input           : N/A
//  Return          : 0 - Cancel key pressed,
//                    1 - Clear key pressed(NO)
//                    2 - Enter key pressed(YES)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD PromptYesNo(void)
{
  Disp2x16Msg(GetConstMsg(EDC_IN_CORRECT), MW_LINE5, MW_BIGFONT);
  return APM_SelYesNo();
}
//*****************************************************************************
//  Function        : GetAuthCode
//  Description     : Prompt user to entry authorization code.
//  Input           : N/A
//  Return          : TRUE;  // valid Input.
//                    FALSE; // user cancel
//  Note            : N/A
//  Globals Changed : RSP_DATA.w_rspcode;
//*****************************************************************************
BOOLEAN GetAuthCode(void)
{
  BYTE kbdbuf[32];
  BYTE *pbTlv;
  DWORD ecrdata_len;

  // ECR data ready
  if (gGDS->i_ecr_len >= 2) {
    ecrdata_len = gGDS->i_ecr_len - 2;
    if (ecrdata_len) {
      pbTlv = TlvSeek(gGDS->s_ecr_data.sb_content, ecrdata_len, ECRTAG_AUTH_RESP);
      if (pbTlv != NULL) {
        TlvVPtr(pbTlv);
        kbdbuf[0] = TlvLen(pbTlv);
        if (kbdbuf[0] > 6)
          kbdbuf[0] = 6;
        memset(RSP_DATA.sb_auth_code, ' ',6);
        memcpy(RSP_DATA.sb_auth_code, TlvVPtr(pbTlv), kbdbuf[0]);
        return TRUE;
      }
    }
  }
  while (1) {
    kbdbuf[0] = 0;
    Disp2x16Msg(GetConstMsg(EDC_IN_APP_CODE), MW_LINE3, MW_BIGFONT);
    if (!APM_GetKbd(ALPHA_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(2)+IMAX(6), kbdbuf))
      return 0;
    if (kbdbuf[0] >= 2) {
      memset(RSP_DATA.sb_auth_code, ' ',6);
      memcpy(RSP_DATA.sb_auth_code, &kbdbuf[1], kbdbuf[0]);
      return 1;
    }
  }
}
//*****************************************************************************
//  Function        : GetAmount
//  Description     : Prompt user to input amount.
//  Input           : aTipsNeeded;   // TRUE => also prompt tips.
//                    aFirstKey;     // first key pressed
//  Return          : TRUE;  // valid input.
//                    FALSE; // user cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetAmount(BOOLEAN aTipsNeeded, DWORD aFirstKey)
{
  BYTE kbdbuf[32];
  DWORD ecrdata_len;

  INPUT.dd_tip=0;

  // ECR data ready
  if (gGDS->i_ecr_len >= 2) {
    ecrdata_len = gGDS->i_ecr_len - 2;
    if (ecrdata_len) {
      INPUT.dd_amount = EcrGetAmount();
      if (INPUT.dd_amount != 0)
        return TRUE;
    }
  }
  if (STIS_TERM_DATA.dd_default_amount!=0) {
    INPUT.dd_amount = STIS_TERM_DATA.dd_default_amount;
    return TRUE;
  }

  DispClrBelowMW(MW_LINE3);
  while (1) {
    if (aTipsNeeded)
      Disp2x16Msg(GetConstMsg(EDC_IN_BASE_AMT), MW_LINE3, MW_BIGFONT);
    else
      Disp2x16Msg(GetConstMsg(EDC_IN_ENTER_AMOUNT), MW_LINE3, MW_BIGFONT);

    kbdbuf[0] = aFirstKey? 5: 4;
    memcpy(&kbdbuf[1], STIS_TERM_CFG.sb_currency_name, 3);
    kbdbuf[4] = STIS_TERM_CFG.b_currency;
    kbdbuf[5] = (BYTE)aFirstKey;
    if (!APM_GetKbd(PREFIX_ENB+ECHO+AMOUNT_INPUT+MW_BIGFONT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos], 
                    IMIN(1)+IMAX(STIS_TERM_CFG.b_trans_amount_len),  kbdbuf))
      return 0;

    if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
      memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
      kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
    }
    INPUT.dd_amount = decbin8b(&kbdbuf[1], kbdbuf[0]);

    if (!aTipsNeeded || !PromptTips())
      return 1;

    Disp2x16Msg(GetConstMsg(EDC_IN_ENTER_TIPS), MW_LINE3, MW_BIGFONT);
    if (!APM_GetKbd(AMOUNT_INPUT+MW_BIGFONT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos], 
                    IMIN(1)+IMAX(STIS_TERM_CFG.b_trans_amount_len),  kbdbuf))
      return 0;
    if (kbdbuf[0] == 0)
      INPUT.dd_tip = 0;
    else {
      if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
        memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
        kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
      }
      INPUT.dd_tip = decbin8b(&kbdbuf[1], kbdbuf[0]);
    }

    if (OverMargin()) {
      DispErrorMsg(GetConstMsg(EDC_IN_INV_TIPS));
      return(FALSE);
    }

    INPUT.dd_amount+=INPUT.dd_tip;
    DispLineMW(GetConstMsg(EDC_IN_TOTAL_PROMPT), MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
    switch (PromptYesNo()) {
      case 0: /* cancel key */
        return FALSE;
      case 2:
        return TRUE;
    }
  }
}
//*****************************************************************************
//  Function        : GetExpDate
//  Description     : Prompt user to input card expire date.
//  Input           : N/A
//  Return          : TRUE;  // valid input
//                    FALSE; // user cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetExpDate(void)
{
  struct DATETIME dtg;
  DWORD var_i, var_j;
  BYTE kbdbuf[31];

  INPUT.b_card_expired = FALSE;
  if (!ExpDateReqd()) {
    memset(INPUT.sb_exp_date, 0, sizeof(INPUT.sb_exp_date));
    return TRUE;
  }
  while (1) {
    if (INPUT.b_entry_mode == MANUAL) {
      kbdbuf[0] = 0;
      Disp2x16Msg(GetConstMsg(EDC_IN_EXPIRY_DATE), MW_LINE3, MW_BIGFONT);
      if (!APM_GetKbd(NUMERIC_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST,IMIN(4)+IMAX(4), kbdbuf))
        return FALSE;
      compress(&kbdbuf[5],&kbdbuf[1], 2);
      INPUT.sb_exp_date[0] = kbdbuf[6];  // YY
      INPUT.sb_exp_date[1] = kbdbuf[5];  // MM
      if (((kbdbuf[6] & 0xf) > 9)  || (kbdbuf[6] > 0x99) ||
          ((kbdbuf[5] & 0xf) > 9)  || !(kbdbuf[5]) || (kbdbuf[5] > 0x12) ) {
        DispErrorMsg(GetConstMsg(EDC_IN_INV_EXPIRE));
        continue;
      }
    }

    if ((var_i= (BYTE)(INPUT.sb_exp_date[1])) &&
        (var_i < 0x13) && ((var_i & 0xf) < 0xa)) {
      if (!CheckExpDate())
        return TRUE;

      ReadRTC(&dtg);
      var_i = (BYTE)bcd2bin(dtg.b_year);
      var_j = (BYTE)bcd2bin(INPUT.sb_exp_date[0]);
      if (var_i == var_j) {
        if (INPUT.sb_exp_date[1] >= dtg.b_month)
          return TRUE;
      } else if (var_i > 49) {  // 1950-1999
        if ((var_j < 50 ) || (var_j > var_i))
          return TRUE;
      } else { // 2000-2049
        if ((var_j > var_i) && (var_j < 50))
          return TRUE;
      }
    }

    DispErrorMsg(GetConstMsg(EDC_IN_CARD_EXPIRED));
    if (!ExpiryReject()) {
      INPUT.b_card_expired = TRUE;
      return TRUE;
    }

    if (INPUT.b_entry_mode != MANUAL)
      return FALSE;
  }
}
//*****************************************************************************
//  Function        : GetProductCode
//  Description     : Prompt user to entry product code.
//  Input           : N/A
//  Return          : TRUE;  // valid INPUT.
//                    FALSE; // user cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetProductCode(void)
{
  DWORD keyin, max_select, line_no;
  BYTE  i, offset, start_offset, select_cnt;
  BYTE  tmp[MW_MAX_LINESIZE+1];
  struct PRODUCT *product_select;

  memset(INPUT.sb_product, 0xFF, sizeof(INPUT.sb_product));
  if (!DescriptorReqd())
    return TRUE;

  max_select = APM_GetDescCount();
  if (max_select == 0)
    return TRUE;

  if (max_select == 1) {
    INPUT.sb_product[0]=0;
    return TRUE;
  }

  product_select = (struct PRODUCT *) MallocMW(sizeof(struct PRODUCT) * max_select);
  memset(product_select, 0, sizeof(struct PRODUCT)*max_select);

  offset = 0;
  for (i=0; i<max_select; i++) {
    offset = i;
    APM_GetDescTbl(i, &product_select[i].s_prod);
  }

  DispLineMW(GetConstMsg(EDC_IN_ENTER_DESC), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  line_no = MW_LINE3;
  start_offset = offset = 0;
  select_cnt = 0;
  do {
    DispClrBelowMW(MW_LINE3);
    while ((line_no<MW_MAX_LINE)&&(offset<max_select)) {
      memset(tmp, ' ', sizeof(tmp));
      tmp[MW_MAX_LINESIZE] = 0;
      tmp[0] = product_select[offset].s_prod.b_key;
      tmp[1] = ' ';
      memcpy(&tmp[2], product_select[offset].s_prod.sb_text, 19);
      DispLineMW(tmp, line_no, (product_select[offset].b_selected?MW_REVERSE:0)|MW_SPFONT);
      line_no += (MW_LINE3-MW_LINE2);
      offset++;
    }
    if (max_select > 5) {
      if (offset < max_select) {
        DispLineMW("<CLR>Down<ENT>Confirm", MW_MAX_LINE, MW_REVERSE|MW_SPFONT);
      }
      else {
        DispLineMW("<CLR>Up  <ENT>Confirm", MW_MAX_LINE, MW_REVERSE|MW_SPFONT);
      }
    }
    else
      DispLineMW("         <ENT>Confirm", MW_MAX_LINE, MW_REVERSE|MW_SPFONT);

    keyin = APM_WaitKey(KBD_TIMEOUT, 0);
    if ((keyin>='0')&&(keyin<='0'+max_select)) {
      for (i=0; i < max_select; i++) {
        if (product_select[i].s_prod.b_key == keyin) {
          if ((!product_select[i].b_selected) && (select_cnt == 4)) {
            LongBeep();
            break;
          }
          product_select[i].b_selected = product_select[i].b_selected ? FALSE: TRUE;
          select_cnt += product_select[i].b_selected?1:-1;
          break;
        }
      }
      if (i == max_select)
        LongBeep();
    }
    if ((keyin == MWKEY_ENTER)&&(select_cnt>0))
      break;
    if (keyin == MWKEY_CLR)
      start_offset=offset = (offset == max_select) ? 0 : offset;
    else
      offset = start_offset;
    line_no = MW_LINE3;
  } while (keyin != MWKEY_CANCL);

  if (keyin == MWKEY_CANCL) {
    FreeMW(product_select);
    return FALSE;
  }

  offset=0;
  for (i=0; i < max_select; i++) {
    if (select_cnt == offset)
      break;
    if (product_select[i].b_selected) {
      INPUT.sb_product[offset]=i;
      offset++;
    }
  }
  FreeMW(product_select);
  return TRUE;
}
//*****************************************************************************
//  Function        : Get4DBC
//  Description     : Prompt user to entry 4DBC number.
//  Input           : N/A
//  Return          : TRUE/FALSE;  // valid input or cancel
//  Note            : N/A
//  Globals Changed : INPUT.b_entry_mode;
//*****************************************************************************
BOOLEAN Get4DBC(void)
{
  BYTE kbdbuf[32];

  DispHeader(NULL);
  Disp2x16Msg(GetConstMsg(EDC_IN_4DBC), MW_LINE3, MW_BIGFONT);
  kbdbuf[0] = 0;
  memset(INPUT.sb_amex_4DBC, 0x00, sizeof(INPUT.sb_amex_4DBC));
  if (!APM_GetKbd(NUMERIC_INPUT+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(4)+IMAX(4), kbdbuf))
    return FALSE;

  memcpy(INPUT.sb_amex_4DBC, &kbdbuf[1], kbdbuf[0]);
  switch (INPUT.b_entry_mode) {
    case MANUAL:
      INPUT.b_entry_mode = MANUAL_4DBC;
      break;
    case SWIPE:
      INPUT.b_entry_mode = SWIPE_4DBC;
      break;
    case FALLBACK:
      INPUT.b_entry_mode = FALLBACK_4DBC;
      break;
  }
  return TRUE;
}
//*****************************************************************************
//  Function        : SearchRecord
//  Description     : Search roc number in batch.
//  Input           : aIncVoidTxn;      // Include Voided transaction.
//  Return          : -1;               // Not Found
//                    other value;      // index of record with same roc no.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int SearchRecord(BOOLEAN aIncVoidTxn)
{
  BYTE kbdbuf[MAX_INPUT_LEN+1];
  int   rec_idx;

  RSP_DATA.w_rspcode = '7'*256+'8';
  while (1) {
    kbdbuf[0] = 0;
    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg(GetConstMsg(EDC_IN_TRACE), MW_LINE3, MW_BIGFONT);
    if (!APM_GetKbd(NUMERIC_INPUT+ECHO+JUSTIFIED+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(6)+IMAX(6), kbdbuf))
      return -1;
    CompressInputData(&kbdbuf[7], kbdbuf, 3);
    rec_idx = APM_TraceInBatch(&kbdbuf[7]);
    if (rec_idx != -1) {
      APM_GetBatchRec(rec_idx, &RECORD_BUF, sizeof(RECORD_BUF));
      ClearRspData();
      memcpy(INPUT.sb_pan, RECORD_BUF.sb_pan, 10);
      if (InCardTable()) {
        if (((RECORD_BUF.b_trans_status & VOIDED) != 0) && (!aIncVoidTxn)) {
          RSP_DATA.w_rspcode = 'V'*256+'D';
          return -1;
        }
        memcpy(INPUT.sb_trace_no, &kbdbuf[7], 3);
        return rec_idx;
      }
      else {
        RSP_DATA.w_rspcode = '1'*256+'4';
      }
    }
    DispClrBelowMW(MW_LINE3);
    DispRspText(FALSE);
    ErrorDelay();
  }
}
//*****************************************************************************
//  Function        : DispKeyinPAN
//  Description     : Display the keyin PAN.
//  Input           : aPan;  // pointer to buffer with 1st byte = len;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispKeyinPAN(BYTE *aPan)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];

  memcpy(tmpbuf, &aPan[1], aPan[0]);
  tmpbuf[aPan[0]] = 0;
  DispClrBelowMW(MW_LINE7);
  if (aPan[0] <= MW_NOR_LINESIZE) {
    DispLineMW(tmpbuf, MW_LINE7, MW_RIGHT|MW_BIGFONT); 
  }
  else {
	  DispLineMW(tmpbuf, MW_LINE7, MW_RIGHT|MW_SPFONT); 
  }
}
//*****************************************************************************
//  Function        : GetCardNbr
//  Description     : Get MANUAL key in card number & store to input.sb_pan.
//  Input           : aFirstKey;            // 1st keyin char.
//  Return          : MWKEY_CANCL/MWKEY_CLR/MWKEY_ENTER;
//  Note            : N/A
//  Globals Changed : INPUT.
//*****************************************************************************
DWORD GetCardNbr(DWORD aFirstKey)
{
  DWORD keyin;
  BYTE kbdbuf[MAX_INPUT_LEN+1];

  kbdbuf[0] = 0;
  keyin     = aFirstKey;
  DispClrBelowMW(MW_LINE3);
  while (TRUE) {
    if (keyin == 0)
      keyin=WaitKey(KBD_TIMEOUT, 0);
    switch (keyin) {
      case MWKEY_00:
        if (kbdbuf[0] >= 18) {
          DispErrorMsg(GetConstMsg(EDC_IN_INVALID_CARD));
          return(MWKEY_CLR);
        }
        memcpy(&kbdbuf[++kbdbuf[0]], "00", 2);
        ++kbdbuf[0];
        DispKeyinPAN(kbdbuf);
        break;
      case MWKEY_0:
      case MWKEY_1:
      case MWKEY_2:
      case MWKEY_3:
      case MWKEY_4:
      case MWKEY_5:
      case MWKEY_6:
      case MWKEY_7:
      case MWKEY_8:
      case MWKEY_9:
        if (kbdbuf[0] >= 19) {
          DispErrorMsg(GetConstMsg(EDC_RSP_INVALID_CARD));
          return(MWKEY_CLR);
        }
        kbdbuf[++kbdbuf[0]] = (BYTE) keyin;
        DispKeyinPAN(kbdbuf);
        break;
      case MWKEY_ENTER:
        if (kbdbuf[0] != 0) {
          CompressInputFData(INPUT.sb_pan, kbdbuf, 10);
          INPUT.b_entry_mode = MANUAL;
          return(MWKEY_ENTER);
        }
        break;
      case MWKEY_CLR:
        if (kbdbuf[0] < 1)
          return MWKEY_CLR;
        kbdbuf[0]--;
        DispKeyinPAN(kbdbuf);
        break;
      case MWKEY_CANCL:
        return MWKEY_CANCL;
      default:
        LongBeep();
    }
    keyin = 0;
  }
}
//*****************************************************************************
//  Function        : GetCard
//  Description     : Get & validate card data.
//  Input           : debit_card;   // debit card required.
//  Return          : TRUE;  // valid card input, manual (cr card ) or swipe
//                    FALSE; // user cancel
//  Note            : //21-08-12 JC ++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetCard(int aFirstKey, BOOLEAN aFallback)
{
  DWORD keyin;

  DispHeader(NULL);
  memset(INPUT.sb_holder_name, ' ', sizeof(INPUT.sb_holder_name));
  memset(INPUT.sb_pan, 0xFF, sizeof(INPUT.sb_pan));

  if (aFirstKey > 0)
    keyin = aFirstKey;
  else
    keyin = 0;

  OnDevice(HW_MSR);                //01-08-17 JC ++
  while (1) {
    // already pressed
    if ((keyin >= MWKEY_0)&&(keyin <= MWKEY_9)) {
      keyin = GetCardNbr(keyin);
      if (keyin == MWKEY_ENTER) {
        INPUT.b_entry_mode = MANUAL;
        break;
      }
    }

    if (IOCtlMW(gMsrHandle, IO_MSR_STATUS, NULL) != 0) {
      if (MSRSwiped(&INPUT.s_trk1buf, &INPUT.s_trk2buf)) {
        if (INPUT.s_trk2buf.b_len > 0) {
          if (!aFallback)
            INPUT.b_entry_mode = SWIPE;
          else
            INPUT.b_entry_mode = FALLBACK;
          break;
        }
      }
      // Error
      LongBeep();
    }

    Disp2x16Msg(GetConstMsg(EDC_IN_SWIPE_CARD), MW_LINE5, MW_BIGFONT);
    IOCtlMW(gMsrHandle, IO_MSR_RESET, NULL);
    keyin = WaitKey(9000,WAIT_MSR_READY);
    if (keyin == MWKEY_CANCL) {
      OffDevice(HW_MSR);                //01-08-17 JC ++
      return(FALSE);
    }
  }
  OffDevice(HW_MSR);                //01-08-17 JC ++
  Short1Beep();
  if (ValidCard())
    return TRUE;
  RSP_DATA.w_rspcode = 'W'*256+'C';
  LongBeep();
  return FALSE;
}
//*****************************************************************************
//  Function        : WaitCardTrans
//  Description     : Wait user to input card data.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD WaitCardTrans(int aTxnType)
{
  WORD keyin;
  DDWORD ecr_amt;
  DWORD ret=TXN_CANCEL;
  BYTE ctl_ctrl=0;

  OnDevice(HW_MSR|HW_CLICC);    //25-07-17 JC ++
  INPUT.b_trans = aTxnType;
  RefreshDispAfter(0);
  gDispLogo = FALSE;
  while (1) {
    // Disp amount from ECR exist
    if (gGDS->i_ecr_len > 2) {
      ecr_amt = EcrGetAmount();
      if (ecr_amt != 0)
        INPUT.dd_amount = ecr_amt;
    }
    #ifdef os_disp_bl_control
    os_disp_bl_control(256); // on now
    #endif
    TransModeDisp(FALSE);

    APM_SetKbdTimeout(KBD_TIMEOUT);
    keyin = APM_GetKeyin();
    if (keyin == MWKEY_CANCL) {
      INPUT.dd_amount = 0;
      ret = TXN_CANCEL;
      break;
    }
    else if ((keyin>=MWKEY_0) && (keyin <=MWKEY_9)) {
      if (INPUT.dd_amount==0) {
        GetAmount(TIPsReqd(), keyin);
        RefreshDispAfter(0);
      }
      else {
        DispHeader(NULL);
        if (GetCardNbr(keyin) == MWKEY_ENTER) {
          if (ValidCard()) {
            if (aTxnType == SALE_OFFLINE)
              SaleOfflTrans(INPUT.b_entry_mode);
            else if (aTxnType == AUTH_SWIPE)
              AuthTrans(INPUT.b_entry_mode);
            else if (aTxnType == REFUND)
              RefundTrans(INPUT.b_entry_mode);
            else
              SaleTrans(INPUT.b_entry_mode);
          }
          TransEnd(FALSE);
          ret = TXN_OK;
          break;
        }
      }
    }

    // Contactless transaction
    if (CTLEnabled() && INPUT.dd_amount && CtlUnderTransLimit((DWORD)(INPUT.dd_amount))) {
      if (!ctl_ctrl) {
        if (aTxnType != REFUND)
          ctl_ctrl = SALE_CTL;
        else
          ctl_ctrl = REFUND_CTL;
        if (CTLPreProcess(ctl_ctrl) == FALSE) {
          DispErrorMsg(GetConstMsg(EDC_CTL_PREPROCESS_ERR));
          ret = TXN_ERR;
          break;
        }
        ctl_ctrl = 1;
      }
      else if (CTLWaitCard()) {
        OffDevice(HW_MSR);              //01-08-17 JC ++
        if ((aTxnType == SALE_SWIPE)||(aTxnType == SALE_OFFLINE))
          CTLTrans(SALE_CTL);
        else if (aTxnType == AUTH_SWIPE)
          CTLTrans(AUTH_CTL);
        else if (aTxnType == REFUND)
          CTLTrans(REFUND_CTL);
        else {
          CL_Close(0);
          OffDevice(HW_CLICC);          //01-08-17 JC ++
          LongBeep();
          ret = TXN_ERR;
          break;
        }
        OffDevice(HW_CLICC);            //01-08-17 JC ++
        ret = TXN_OK;
        break;
      }
    }
    
    // ICC
    if (ICCInserted()) {
      OffDevice(HW_MSR|HW_CLICC);     //01-08-17 JC ++
      if (PowerOnICC()) {
        if ((aTxnType == SALE_SWIPE)||(aTxnType == SALE_OFFLINE))
          EMVTrans(SALE_ICC);
        else if (aTxnType == AUTH_SWIPE)
          EMVTrans(AUTH_ICC);
        else if (aTxnType == REFUND)
          EMVTrans(REFUND_ICC);
        else
          EMVTrans(aTxnType);
        PowerOffICC();
        ret = TXN_OK;
      }
      else {
        RSP_DATA.w_rspcode = 'R'*256+'E';
        //Added for MTIP02 Test01 S04 & S05
        DispErrorMsg(GetConstMsg(EDC_RSP_READ_ERROR));
        // do fallback
        INPUT.b_entry_mode = ICC;
        INPUT.b_trans = SALE_ICC;
        if (GetCard(0, TRUE))
          SaleTrans(INPUT.b_entry_mode);
        TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
        ret = TXN_OK;
      }
      while (!ICCReaderRdy()) {
        if (TimerGetMW(gTimerHdl[TIMER_DISP]) == 0) {  // Already Disp Timeout
          DispClrBelowMW(MW_LINE3);
          Disp2x16Msg(GetConstMsg(EDC_CO_REMOVE_ICC), MW_LINE5, MW_BIGFONT);
          gGDS->b_disp_chgd = TRUE;
          WaitICCRemove();
          break;
        }
        if (GetCharMW()==MWKEY_CANCL)
          RefreshDispAfter(0);
      }
      break;
    }

    // MSR
    if (IOCtlMW(gMsrHandle, IO_MSR_RXLEN, NULL) > 0) {
      if (MSRSwiped(&INPUT.s_trk1buf, &INPUT.s_trk2buf)) {
        if (CTLEnabled())
          CL_Close(0);
        OffDevice(HW_MSR|HW_CLICC);     //01-08-17 JC ++
        if (INPUT.s_trk2buf.b_len > 0) {
          if (ValidCard()) {
            if (aTxnType == SALE_OFFLINE)
              SaleOfflTrans(INPUT.b_entry_mode);
            else if (aTxnType == AUTH_SWIPE)
              AuthTrans(INPUT.b_entry_mode);
            else if (aTxnType == REFUND)
              RefundTrans(INPUT.b_entry_mode);
            else
              SaleTrans(INPUT.b_entry_mode);
          }
          TransEnd(FALSE);
          ret = TXN_OK;
        }
        else {
          RSP_DATA.w_rspcode = 'R'*256+'E';
          ret = TXN_ERR;
        }
      }
      IOCtlMW(gMsrHandle, IO_MSR_RESET, NULL);
      TransEnd(FALSE);
      break;
    }
  }

  OffDevice(HW_MSR|HW_CLICC);       //25-07-17 JC ++
  gIdleCnt = FreeRunMark();         //25-07-17 JC ++
  return ret;
}
//*****************************************************************************
//  Function        : GetEcrRef
//  Description     : Prompt user to entry ECR reference number.
//  Input           : TRUE;  // prompt user to get ECR ref if need
//                    FALSE; // not prompt user
//  Return          : TRUE;  // valid input.
//                    FALSE; // user cancel
//  Note            : N/A
//  Globals Changed : INPUT.sb_ecr_ref;
//*****************************************************************************
BOOLEAN GetEcrRef(BOOLEAN aPrompt)
{
  BYTE kbdbuf[31];

  memset(INPUT.sb_ecr_ref, ' ', 16);

  if (gGDS->i_ecr_len> 0) {
    EcrGetRef(INPUT.sb_ecr_ref);
    return TRUE;
  }
  if (!aPrompt)
    return FALSE;

  DispHeader(NULL);
  if (!EcrRefReqd())
    return TRUE;

  Disp2x16Msg(GetConstMsg(EDC_IN_ECRREF), MW_LINE3, MW_BIGFONT);

  kbdbuf[0] = 0;
  if (!APM_GetKbd(ALPHA_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(16), kbdbuf))
    return 0;

  memcpy(INPUT.sb_ecr_ref, &kbdbuf[1], kbdbuf[0]);
  return TRUE;
}
//*****************************************************************************
//  Function        : GetCashBackAmount
//  Description     : Prompt user to enter cashback amount.
//  Input           : N/A
//  Return          : TRUE/FALSE;  // valid input or cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetCashBackAmount(DDWORD *aCashbackAmt)
{
  BYTE kbdbuf[32];
  BYTE var_i = 0, var_j = 5;
  DDWORD amt = 0;

  while (1) {    
    amt = 0;
    kbdbuf[0] = 4;
    memcpy(&kbdbuf[1], STIS_TERM_CFG.sb_currency_name, 3);
    kbdbuf[4] = STIS_TERM_CFG.b_currency;
    
    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg("Enter Cash Back Amount", MW_LINE3, MW_BIGFONT);

    if (!APM_GetKbd(PREFIX_ENB+AMOUNT_INPUT+MW_BIGFONT+MW_LINE7+KDecimalPos[STIS_TERM_CFG.b_decimal_pos],
            IMIN(var_i)+IMAX(var_j), kbdbuf))
      return FALSE;
    if (kbdbuf[0] == 0)
      amt = 0;
    else {
      if (STIS_TERM_CFG.b_decimal_pos < 2) {  // 8583 expect 2 decimal pos.
        memset(&kbdbuf[kbdbuf[0]+1], '0', 2);
        kbdbuf[0] += (2-STIS_TERM_CFG.b_decimal_pos);
      }
      amt = decbin8b(&kbdbuf[1], kbdbuf[0]);
    }
    INPUT.dd_amount += amt;
    *aCashbackAmt = amt;
    return TRUE;
  }

  return FALSE;
}

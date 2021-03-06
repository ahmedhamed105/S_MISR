//-----------------------------------------------------------------------------
//  File          : hostmsg.c
//  Module        :
//  Description   : Include routines to Pack/Unpack Host Message.
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
#include <stdio.h>
#include <string.h>
#include "corevar.h"
#include "chkoptn.h"
#include "constant.h"
#include "record.h"
#include "message.h"
#include "tranutil.h"
#include "util.h"
#include "sysutil.h"
#include "emvtrans.h"
#include "hostmsg.h"

//*****************************************************************************
//  Function        : TrainiingRsp
//  Description     : Generate training response.
//  Input           : N/A
//  Return          : Transaction status
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BYTE TrainingRsp(void)
{
  struct DATETIME dtg;

  ReadRTC(&dtg);
  memcpy(&RSP_DATA.s_dtg.b_year, &dtg.b_year, 6);
  memset(RSP_DATA.sb_auth_code, '0', sizeof(RSP_DATA.sb_auth_code));
  split(RSP_DATA.sb_rrn, &dtg.b_year, 6);
  RSP_DATA.b_response = TRANS_ACP;
  RSP_DATA.w_rspcode = '0'*256+'0';
  RSP_DATA.dd_amount = 0;
  memset(&RSP_DATA.text[21], ' ', 20);
  return TRANS_ACP;
}
//*****************************************************************************
//  Function        : Match
//  Description     : Compare current pointer to buffer.
//  Input           : aPtr;    // pointer to buffer needs to Match.
//                    aLen;    // no. of byte to Match.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN Match(void *aPtr, BYTE aLen)
{
  BOOLEAN result;

  result = (memcmp(aPtr, get_pptr(), aLen) == 0);
  inc_pptr(aLen);
  return(result);
}
//*****************************************************************************
//  Function        : BcdBin8b
//  Description     : Convert max 6 bytes of bcd number to DDWORD.
//  Input           : aSrc;        // pointer src bcd number
//                    aLen;        // no. of byte of the bcd number to be conv.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DDWORD BcdBin8b(void *aSrc,BYTE aLen)
{
  BYTE buffer[12];

  if (aLen < 7) {
    split(buffer,aSrc,aLen);
    return decbin4b(buffer,(BYTE)(aLen*2));
  }
  return 0;
}
//*****************************************************************************
//  Function        : SyncHostDtg
//  Description     : Sync RTC with HOST date & time.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SyncHostDtg(void)
{
  BYTE tmp;
  struct DATETIME dtg;

  ReadRTC(&dtg);
  RSP_DATA.s_dtg.b_century = dtg.b_century;
  RSP_DATA.s_dtg.b_year = dtg.b_year;
  tmp = 1;
  if ((RSP_DATA.s_dtg.b_month == 0x12) && (dtg.b_month == 0x01))
    bcdsub(&RSP_DATA.s_dtg.b_year,&tmp,1);
  else
    if ((RSP_DATA.s_dtg.b_month == 0x01) && (dtg.b_month == 0x12))
      bcdadd(&RSP_DATA.s_dtg.b_year,&tmp,1);
  ByteCopy((BYTE *)&dtg, (BYTE *)&RSP_DATA.s_dtg, sizeof(struct DATETIME));
  SetRTC(&dtg);
}
//*****************************************************************************
//  Function        : PackProcCode
//  Description     : Pack the processing code for input transaction type,
//  Input           : aTrans;      // transaction type
//                    aProcCode;   // 2nd byte of processing code
//  Return          : N/A
//  Note            : when aProcCode != 0xFF => use INPUT.b_acc_ind
//  Globals Changed : TX_DATA.sb_proc_code[];
//*****************************************************************************
void PackProcCode(BYTE aTrans, BYTE aProcCode)
{
  TX_DATA.sb_proc_code[0] = KTransBitmap[aTrans].b_proc_code;

  if (aTrans == ADJUST)
    TX_DATA.sb_proc_code[0] = 2;
  else
    if ((aTrans <= AUTH_MANUAL) || (aTrans == AUTH_ICC))  // !2005-09-21
      TX_DATA.sb_proc_code[0] = 0x30;

  if (aProcCode == 0xFF)
    aProcCode = INPUT.b_acc_ind;

  TX_DATA.sb_proc_code[1] = aProcCode;
  TX_DATA.sb_proc_code[2] = 0;
}
/*********************************************************************/
/*  This procedure fills stxbuf with information required according  */
/*  to the BOOLEAN map of the trans.                                 */
/*  02. primary account number   03. processing code                 */
/*  04. amount, transaction      11. system trace number             */
/*  12. local transaction time   13. local transaction date          */
/*  14. card expiration date     22. entry mode                      */
/*  24. netword int'l id.        25. condition code                  */
/*  35. track 2 data             37. retrieval reference number      */
/*  38. authorization id.        39. response code                   */
/*  41. terminal id              42. card acceptor id                */
/*  44. additional resp data     45. track 1 data                    */
/*  52. PIN                                                          */
/*  54. additional amount        60. private - org. amount           */
/*  61. product code             62. private - invoice no            */
/*  63. settlement totals        64. MAC                             */
/*********************************************************************/
void PackHostMsg(void)
{
  BYTE * bitmap_ptr;
  BYTE tx_bitmap[8];
  BYTE var_i, var_j;
  BYTE tmp[10];  // request by dbin2bcd
  struct DESC_TBL prod;

  TxBufSetup(ExtraMsgLen());

  /* TPDU header */
  pack_byte(0x60);
  pack_mem((BYTE *)&STIS_ACQ_TBL(0).sb_nii, 2);
  pack_null(2);

  /* msg type id, bitmap */
  pack_word(KTransBitmap[TX_DATA.b_trans].w_txmsg_id);
  memcpy(tx_bitmap, KTransBitmap[TX_DATA.b_trans].sb_txbitmap, 8);
  bitmap_ptr = get_pptr();
  inc_pptr(8);

  // Send PAN and expire date if TRK2 not available of ICC trans.
  if ((TX_DATA.b_trans >= SALE_ICC) && (TX_DATA.b_trans <= REFUND_ICC)) {
    if (INPUT.s_trk2buf.b_len == 0) {
      tx_bitmap[0] |= 0x40;
      tx_bitmap[1] |= 0x04;
      tx_bitmap[4] &= ~0x20;
    }
  }

  /* 02. primary account number */
  if (tx_bitmap[0] & 0x40) {
    var_i = (BYTE)fndb(TX_DATA.sb_pan, 0xff, 10);
    if (var_i == 0xff)
      var_i = 10;
    var_j = var_i * 2;
    if ((TX_DATA.sb_pan[var_i-1] & 0x0f) == 0x0f) {
      var_j -= 1;
      if ((STIS_ACQ_TBL(0).sb_nii[0]*256+STIS_ACQ_TBL(0).sb_nii[1]) == 0x016)
        TX_DATA.sb_pan[var_i - 1] &= 0xf0;
    }
    pack_byte((BYTE)bin2bcd(var_j));
    pack_mem(TX_DATA.sb_pan, var_i);
  }

  /* 03. processing code */
  if (tx_bitmap[0] & 0x20) {
    pack_mem(TX_DATA.sb_proc_code, 3);
  }

  /* 04. amount, transaction */
  if (tx_bitmap[0] & 0x10) {
    if (TX_DATA.b_trans != VOID)
      dbin2bcd(tmp,TX_DATA.dd_amount);
    else
      memset(&tmp[4], 0, 6);
    pack_mem(&tmp[4], 6);
  }

  /* 11. systems trace audit number */
  if (tx_bitmap[1] & 0x20) {
    pack_mem(TX_DATA.sb_trace_no, 3);
  }

  /* 12. local transaction time */
  if (tx_bitmap[1] & 0x10) {
    pack_mem(&TX_DATA.s_dtg.b_hour, 3);
  }

  /* 13. local transaction date */
  if (tx_bitmap[1] & 0x08) {
    pack_mem(&TX_DATA.s_dtg.b_month, 2);
  }

  /* 14. expiry date */
  if (tx_bitmap[1] & 0x04) {
    pack_mem(TX_DATA.sb_exp_date, 2);
  }

  /* 22. Entry mode */
  if (tx_bitmap[2] & 0x04) {
    switch (TX_DATA.b_entry_mode) {
      case MANUAL:
        pack_word(0x0012);
        break;
      case SWIPE:
        pack_word(0x0022);
        break;
      case MANUAL_4DBC:
        pack_word(0x0573);
        break;
      case SWIPE_4DBC:
        pack_word(0x0563);
        break;
      case FALLBACK_4DBC:
        pack_word(0x0663);
        break;
      case FALLBACK:
        pack_word(0x0802);
        break;
      default :
        pack_word(0x0051);
        break;
    }
  }

  /* 23. PAN seqence number */
  if (tx_bitmap[2] & 0x02)
  {
    TLIST* psLp;
    if (((psLp = TagSeek(gGDS->s_TagList, 0x5f34)) != NULL) && (psLp->sLen > 0))
      pack_word((DWORD)*psLp->pbLoc);
    else
      tx_bitmap[2] &= ~0x02;
  }

  /* 24. network international identifier */
  if (tx_bitmap[2] & 0x01) {
    pack_mem((BYTE *)&STIS_ACQ_TBL(0).sb_nii, 2);
  }

  /* 25. pos condition code */
  if (tx_bitmap[3] & 0x80) {
    if ((TX_DATA.b_trans <= AUTH_MANUAL)||
        (TX_DATA.b_trans == SALE_COMPLETE) ||
        (TX_DATA.b_trans == ESALE_COMPLETE) ||
        (TX_DATA.b_trans == AUTH_ICC) ||
        (TX_DATA.b_trans == ESALE_UNDER_LMT) ||
        (TX_DATA.b_trans == SALE_UNDER_LMT))
      pack_byte(0x06);
    else
      if (((TX_DATA.b_trans == TRANS_UPLOAD)||
           (TX_DATA.b_trans == EDC_REV)
          ) &&
          ((TX_DATA.b_org_trans<=AUTH_MANUAL)   ||
           (TX_DATA.b_org_trans==ESALE_COMPLETE) ||
           (TX_DATA.b_org_trans==ESALE_UNDER_LMT) ||
           (TX_DATA.b_org_trans==SALE_COMPLETE) ||
           (TX_DATA.b_org_trans==SALE_UNDER_LMT)
          )
         )
        pack_byte(0x06);
      else
        pack_byte(0x00);
  }

  /* 35. track 2 data */
  if (tx_bitmap[4] & 0x20) {
    if (INPUT.s_trk2buf.b_len) {
      pack_byte((BYTE)bin2bcd(INPUT.s_trk2buf.b_len));
      compress(get_pptr(),INPUT.s_trk2buf.sb_content, (BYTE)((INPUT.s_trk2buf.b_len+1)/2));
      inc_pptr((BYTE)(INPUT.s_trk2buf.b_len/2));
      if ((INPUT.s_trk2buf.b_len % 2) != 0) {
        *(BYTE *)get_pptr() |= 0x0f;
        inc_pptr(1);
      }
    } else
      tx_bitmap[4] &= ~0x20;
  }

  /* 37. authorization code */
  if (tx_bitmap[4] & 0x08) {
    if ((TX_DATA.b_trans == EDC_REV) && (TX_DATA.b_org_trans != VOID))
      tx_bitmap[4] &= ~0x08; /* not applicable, clear BIT */
    else
      pack_mem(TX_DATA.sb_rrn, 12);
  }

  /* 38. authorization code */
  if (tx_bitmap[4] & 0x04) {
    pack_mem(TX_DATA.sb_auth_code, 6);
  }

  /* 41. terminal identification */
  if (tx_bitmap[5] & 0x80) {
    pack_mem(STIS_ACQ_TBL(0).sb_term_id, 8);
  }

  /* 42. card acceptor identification */
  if (tx_bitmap[5] & 0x40) {
    pack_mem(STIS_ACQ_TBL(0).sb_acceptor_id, 15);
  }

  /* 44. additional resp. data */
  if (tx_bitmap[5] & 0x10) { /* indicates under floor limit approval */
    if (TX_DATA.b_trans == TRANS_UPLOAD) {
      if ((TX_DATA.b_org_trans == REFUND) ||
          (TX_DATA.b_org_trans == SALE_OFFLINE) ||
          (TX_DATA.b_org_trans == SALE_COMPLETE) ||
          (TX_DATA.b_org_trans == SALE_UNDER_LMT) ||
          (TX_DATA.b_org_trans == ESALE_COMPLETE) ||
          (TX_DATA.b_org_trans == ESALE_UNDER_LMT)
         )
        tx_bitmap[5] &= ~0x10; /* not applicable, clear bit */
    }
//      if (((TX_DATA.b_trans == TRANS_UPLOAD) &&
//          (!((TX_DATA.b_org_trans >= REFUND) && (TX_DATA.b_org_trans <= SALE_UNDER_LMT))))
//         )
//        tx_bitmap[5] &= ~0x10; /* not applicable, clear bit */
    else /* indicates under floor limit approval */
      if ((TX_DATA.b_org_trans == SALE_UNDER_LMT) ||(TX_DATA.b_org_trans == ESALE_UNDER_LMT))
        pack_str("\x02\x30\x33");
      else
        pack_str("\x02\x30\x30");
  }

  /* 45. track 1 data */
  if (tx_bitmap[5] & 0x08) {
    // Send Track1 when available
    if ((INPUT.s_trk1buf.b_len==0)||!Track1Enable())
      tx_bitmap[5] &= ~0x08;
    else {
      pack_byte((BYTE)bin2bcd(INPUT.s_trk1buf.b_len));
      pack_mem(INPUT.s_trk1buf.sb_content, INPUT.s_trk1buf.b_len);
    }
  }

  /* 52. pin block */
  if (tx_bitmap[6] & 0x10) {
    pack_mem (TX_DATA.sb_pin, 8);
  }

  /* 54. additional amount */
  if (tx_bitmap[6] & 0x04) {
    if (TX_DATA.dd_tip !=0) {
      pack_word(0x0012);
      dbin2bcd(tmp,TX_DATA.dd_tip);
      split_data(&tmp[4],6);
    }
    else
      tx_bitmap[6] &= ~0x04; /* no tip, clear BIT */
  }

  /* 55. EMV data */
  if (tx_bitmap[6] & 0x02) {
    // EMVEDC data higher priority
    if (TX_DATA.s_icc_data.w_misc_len) {
      pack_word(bin2bcd(TX_DATA.s_icc_data.w_misc_len));
      pack_mem(TX_DATA.s_icc_data.sb_misc_content, TX_DATA.s_icc_data.w_misc_len);
    }
    else
      if (Prompt4DBC() &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x0220) &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x0400) &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x0320)) {
        pack_word(0x0004);
        pack_mem(INPUT.sb_amex_4DBC, 4);
      }
      else
        tx_bitmap[6] &= ~0x02; // no 4DBC
  }

  /* 60. reserved private */
  if (tx_bitmap[7] & 0x10) {
    if (TX_DATA.b_trans == SETTLEMENT) {
      pack_word(0x0006); /* length */
      split_data(STIS_ACQ_TBL(0).sb_curr_batch_no, 3);
    }
    else
      if (TX_DATA.b_trans == TRANS_UPLOAD) {
        pack_word(0x0022);
        tmp [0] = (BYTE)(KTransBitmap[TX_DATA.b_org_trans].w_txmsg_id >> 8);
        tmp [1] = (BYTE)(KTransBitmap[TX_DATA.b_org_trans].w_txmsg_id &0xF);
        split_data(tmp, 2);
        split_data(TX_DATA.sb_org_trace_no, 3);
        pack_space(12);
      }
      else { /* adjust original amount */
        pack_word(0x0012);
        dbin2bcd(tmp,TX_DATA.dd_org_amount);
        split_data(&tmp[4], 6);
      }
  }

  /* 61. private - product codes */
  if (tx_bitmap[7] & 0x08) {
    if (TX_DATA.sb_product[0] == 0xFF)
      tx_bitmap[7] &= ~0x08; /* clear BIT */
    else {
      pack_word(0x0008); /* length */
      for (var_i = 0; var_i != 4; var_i++) {
        if (TX_DATA.sb_product[var_i] != 0xFF) {
          APM_GetDescTbl(TX_DATA.sb_product[var_i], &prod);
          pack_mem((BYTE *)&prod.sb_host_tx_code, 2);
        }
        else
          pack_space(2);
      }
    }
  }

  /* 62. private - ROC number of financial/SOC number of settlement */
  if (tx_bitmap[7] & 0x04) {
    pack_word(0x0006); /* length */
    split_data(TX_DATA.sb_roc_no, 3);
  }

  /* 63. transaction counts and totals for settlement */
  if (tx_bitmap[7] & 0x02) {
    pack_word(0x0036); /* length */
    bindec_data(TERM_TOT.s_sale_tot.w_count, 3);
    dbin2bcd(tmp,TERM_TOT.s_sale_tot.dd_amount);
    split_data(&tmp[4], 6);
    bindec_data(TERM_TOT.s_refund_tot.w_count, 3);
    dbin2bcd(tmp,TERM_TOT.s_refund_tot.dd_amount);
    split_data(&tmp[4], 6);
    bindec_data(gAppDat.reversal_count, 3);
    bindec_data((WORD)TERM_TOT.d_adjust_count, 3);
  }

  memcpy(bitmap_ptr, tx_bitmap, 8);
  PackTxBufLen(ExtraMsgLen());
}
/******************************************************************/
/*  This procedure extracts or checks items in response message.  */
/*  Enter with type of message to be retrieved:                   */
/*  02. P.A.N.                                                    */
/*  03. Processing code          04. amount                       */
/*  11. system trace number      12. trans time                   */
/*  13. trans date               14. expiry date                  */
/*  15. settlement date                                           */
/*  22. pos entry mode           24. network id                   */
/*  25. pos condition code                                        */
/*  32.                          37. retrieval reference number   */
/*  38. auth code                39. response code                */
/*  41. terminal id              42. card acceptor id             */
/*  48. CITI instalment info.                                     */
/*  54. additional amount        60. init. param / original trans */
/*  61. product descriptor code  62. record of charge number      */
/******************************************************************/
BYTE CheckHostRsp(void)
{
  DWORD var_i;
  WORD msg_id;
  BYTE bitmap[8];
  BYTE tmp;
  BOOLEAN more_msg, sync_datetime;

  Disp2x16Msg(GetConstMsg(EDC_CO_PROCESSING), MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (TrainingModeON())
    return TrainingRsp();

  RxBufSetup(ExtraMsgLen());
  more_msg = sync_datetime = 0;
  inc_pptr(5); // 5 BYTE TO MSGID

  if ((msg_id=get_word()) == 0x0820)
    return 0xff; /* please wait message */
  if ((KTransBitmap[TX_DATA.b_trans].w_txmsg_id + 0x10) != msg_id) {
    RSP_DATA.w_rspcode = 'I'*256+'R';
    return TRANS_FAIL;
  }

  get_mem(bitmap, 8);

  /* 02. pan */
  if (bitmap[0] & 0x40) {
    var_i = bcd2bin(get_byte());
    memset(RSP_DATA.sb_pan, 0xFF, 10);
    get_mem(RSP_DATA.sb_pan, ((var_i+1)/2));
  }

  /* 03. processing code */
  if (bitmap[0] & 0x20) {
    if (memcmp(get_pptr(), TX_DATA.sb_proc_code, 2)!=0) {
      if ((TX_DATA.b_trans != SETTLEMENT)|| (memcmp(get_pptr(),KSetlPCode2,2)!=0)) {
        RSP_DATA.w_rspcode = 'I'*256+'P';
        return TRANS_FAIL;
      }
    }
    inc_pptr(2);
    tmp = get_byte();
    if ((TX_DATA.b_trans == TRANS_UPLOAD) && (tmp == 0x01))
      more_msg = 1;
    else if ((tmp != 0x01) && tmp) {
      RSP_DATA.w_rspcode = 'I'*256+'P';
      return TRANS_FAIL;
    }
  }

  /* 04. amount */
  if (bitmap[0] & 0x10) {
    RSP_DATA.dd_amount = BcdBin8b(get_pptr(),6);
    inc_pptr(6);
  }

  /* 11. check system trace no */
  if (bitmap[1] & 0x20) {
    if (TX_DATA.b_trans != TRANS_UPLOAD) {
      if (!Match(TX_DATA.sb_trace_no, 3)) {
        RSP_DATA.w_rspcode = 'I'*256+'S';
        return TRANS_FAIL;
      }
    } else
      inc_pptr(3);
  }

  /* 12. trans time */
  if (bitmap[1] & 0x10) {
    sync_datetime |= 0x10;
    get_mem(&RSP_DATA.s_dtg.b_hour, 3);
  }

  /* 13. trans date */
  if (bitmap[1] & 0x08) {
    sync_datetime |= 0x08;
    get_mem(&RSP_DATA.s_dtg.b_month, 2);
  }

  /* 14. expiry date */
  if (bitmap[1] & 0x04) {
    get_mem(RSP_DATA.sb_exp_date, 2);
  }

  /* 24. bypass netword id */
  if (bitmap[2] & 0x01) {
    inc_pptr(2);
  }

  /* 25. pos condition code */
  if (bitmap[3] & 0x80) {
    inc_pptr(1);
  }

  /* 37. retrieval reference number */
  if (bitmap[4] & 0x08) {
    get_mem(RSP_DATA.sb_rrn, 12);
  }

  /* 38. auth code */
  memset(RSP_DATA.sb_auth_code, ' ', 6);
  if (bitmap[4] & 0x04) {
    get_mem(RSP_DATA.sb_auth_code, 6);
  }

  /* 39. response code  */
  RSP_DATA.w_rspcode = '0'*256+'0'; // upload response does not have response code
  if (bitmap[4] & 0x02) {
    RSP_DATA.w_rspcode=get_word();
//     printf("\x1b\xc0%02x: %c%c", RSP_DATA.w_rspcode, RSP_DATA.w_rspcode>>8, RSP_DATA.w_rspcode&0xFF); //!TT
//     Delay1Sec(3, 0);
  }

  /* 41. term id */
  if (bitmap[5] & 0x80) {
    if (!Match(STIS_ACQ_TBL(0).sb_term_id, 8)) {
      RSP_DATA.w_rspcode = 'I'*256+'T';
      return TRANS_FAIL;
    }
  }

  /* 54. additional amount */
  RSP_DATA.dd_tip=0;
  if (bitmap[6] & 0x04) {
    RSP_DATA.dd_tip = BcdBin8b(get_pptr(),(BYTE)(bcd2bin(get_word())/2));
  }

  /* 55. EMV relative data */
  RSP_DATA.s_icc_data.w_misc_len = 0;
  if (bitmap[6] & 0x02) {
    RSP_DATA.s_icc_data.w_misc_len  = bcd2bin(get_word());
    if (RSP_DATA.s_icc_data.w_misc_len > sizeof(RSP_DATA.s_icc_data.sb_misc_content)) {
      RSP_DATA.w_rspcode = '9'*256 + '6';
      return(TRANS_FAIL);
    }
    get_mem(RSP_DATA.s_icc_data.sb_misc_content, RSP_DATA.s_icc_data.w_misc_len);
  }

  /* 62. private field */
  if (bitmap[7] & 0x04) {
    memset(RSP_DATA.sb_roc_no, 0xFF, 3);
    var_i = bcd2bin(get_word());
    if ((var_i == 6) && (peek_byte() != ' '))
      compress(RSP_DATA.sb_roc_no, get_pptr(), 3);
    inc_pptr(var_i);
  }

  /* 63. private field */
  if ((bitmap[7] & 0x02) != 0) {
    var_i = bcd2bin(get_word());
    RSP_DATA.text[0] = (BYTE)((var_i > 69) ? 69 : var_i);
    get_mem(&RSP_DATA.text[1], RSP_DATA.text[0]);
  }

  if (sync_datetime == 0x18) // Date/time receive from Host.
    SyncHostDtg();
  if ((RSP_DATA.w_rspcode == '0'*256+'0') ||
      (RSP_DATA.w_rspcode == '0'*256+'8') ||
      (RSP_DATA.w_rspcode == '8'*256+'8') )
  {
    STIS_ACQ_TBL(0).b_status = UP;
    if (more_msg)
      return MORE_RSP;
    return TRANS_ACP;
  }
  else if (RSP_DATA.w_rspcode == 'Z'*256+'1') {
    RSP_DATA.w_rspcode = '0'*256+'0';
    STIS_ACQ_TBL(0).b_status = DOWN;
    return TRANS_ACP;
  }
  else
    return TRANS_REJ;
}

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
#include "auxcom.h"
#include "coremain.h"

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
   BYTE *len_ptr, *bitmap_ptr;
  BYTE tx_bitmap[16];
  BYTE var_i, var_j;
  BYTE buf[128], tmp[10],length[4];  // request by dbin2bcd
  struct DESC_TBL prod;

  TxBufSetup(ExtraMsgLen());

 // len_ptr = get_pptr();     // 4-byte length
/* PowerCard header */
pack_byte(0x49);
pack_byte(0x53);
pack_byte(0x4F);


//pack_byte(0x30);
//pack_byte(0x34);
//pack_byte(0x36);
//pack_byte(0x35);

len_ptr = get_pptr();     // 4-byte length
inc_pptr(4);

pack_byte(0x32);
pack_byte(0x31);
pack_byte(0x31);
pack_byte(0x31);
pack_byte(0x30);
pack_byte(0x35);
pack_byte(0x30);
pack_byte(0x30);
pack_byte(0x30);
/* PowerCard header */

/* PowerCard Message id */
//pack_byte(0x31);
//pack_byte(0x32);
//pack_byte(0x30);
//pack_byte(0x30);
  SprintfMW(buf, "%04X", KTransBitmap[TX_DATA.b_trans].w_txmsg_id);
  pack_mem(buf, 4);
/* PowerCard Message id */

  
/* PowerCard Map */

//pack_byte(0xF2);
//pack_byte(0x34);
//pack_byte(0x87);
//pack_byte(0x41);
//pack_byte(0x68);
//pack_byte(0xE1);
//pack_byte(0x9A);
//pack_byte(0x03);
//pack_byte(0x01);
//pack_byte(0x00);
//pack_byte(0x00);
//pack_byte(0x00);
//pack_byte(0x00);
//pack_byte(0x00);
//pack_byte(0x00);
//pack_byte(0x00);

memcpy(tx_bitmap, KTransBitmap[TX_DATA.b_trans].sb_txbitmap, 16);
  bitmap_ptr = get_pptr();
  if (tx_bitmap[0] & 0x80)
   inc_pptr(16);
  else
    inc_pptr(8);

/* PowerCard Map */
	  
 //  pack_mem("ISO0433211105000", 16);
 //  
 // /* PowerCard header */
 //// pack_mem("ISO", 3);
 // len_ptr = get_pptr();     // 4-byte length
 // inc_pptr(16);
 //// inc_pptr(4);
 //// pack_byte('2');           // POS
 //// SprintfMW(buf, "%02X%02X0", SOFT_VERSION, SOFT_SUBVERSION);   // SW Version
 //// pack_mem(buf, 5);
 //// pack_mem("000", 3);       // single message
 // 
 // /* msg type id */
 // SprintfMW(buf, "%04X", KTransBitmap[TX_DATA.b_trans].w_txmsg_id);
 // pack_mem(buf, 4);
 // 
 // /* bitmap */
 // memcpy(tx_bitmap, KTransBitmap[TX_DATA.b_trans].sb_txbitmap, 16);
 // bitmap_ptr = get_pptr();
 // if (tx_bitmap[0] & 0x80)
 //   inc_pptr(16);
 // else
 //   inc_pptr(8);

 // // Send PAN and expire date if TRK2 not available of ICC trans.
 // if ((TX_DATA.b_trans >= SALE_ICC) && (TX_DATA.b_trans <= AUTH_ICC)) {
 //   if (INPUT.s_trk2buf.b_len == 0) {
 //     tx_bitmap[0] |= 0x40;
 //     tx_bitmap[1] |= 0x04;
 //     tx_bitmap[4] &= ~0x20;
 //   }
 // }

 if (memcmp(&TX_DATA.s_dtg.b_year, "\x00\x00\x00\x00\x00\x00", 6) == 0)
   ReadRTC(&TX_DATA.s_dtg);



  /* 02. primary account number */

  if (tx_bitmap[0] & 0x40) {
    var_i = (BYTE)fndb(TX_DATA.sb_pan, 0xff, 10);
    if (var_i == 0xff)
      var_i = 10;
    var_j = var_i * 2;
    if ((TX_DATA.sb_pan[var_i-1] & 0x0f) == 0x0f) {
      var_j -= 1;
      //if ((STIS_ACQ_TBL(0).sb_nii[0]*256+STIS_ACQ_TBL(0).sb_nii[1]) == 0x016)
      //  TX_DATA.sb_pan[var_i - 1] &= 0xf0;
    }
    pack_byte((BYTE)bin2bcd(var_j));
    split(buf, TX_DATA.sb_pan, var_i);
    pack_mem(buf, var_j);                     // PAN in ascii format
  }




  /* 03. processing code */
  if (tx_bitmap[0] & 0x20) {
    split_data(TX_DATA.sb_proc_code, 3);
  }

  /* 04. amount, transaction */
  if (tx_bitmap[0] & 0x10) {
    if (TX_DATA.b_trans != VOID)
      dbin2bcd(tmp,TX_DATA.dd_amount);
    else
      memset(&tmp[4], 0, 6);
    split_data(&tmp[4], 6);
  }

  /* 07. transmission date & time */
  if (tx_bitmap[1] & 0x02) {
  //pack_byte(0x31);
  //pack_byte(0x36);
  //pack_byte(0x30);
  //pack_byte(0x35);
  //pack_byte(0x32);
  //pack_byte(0x33);
  //pack_byte(0x31);
  //pack_byte(0x31);
  //pack_byte(0x32);
  //pack_byte(0x37);
    split_data(&TX_DATA.s_dtg.b_year, 5);
  }



//  pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x31);pack_byte(0x31);pack_byte(0x38);pack_byte(0x31);pack_byte(0x36);pack_byte(0x30);pack_byte(0x35);pack_byte(0x32);pack_byte(0x33);pack_byte(0x31);pack_byte(0x31);pack_byte(0x32);pack_byte(0x37);pack_byte(0x35);pack_byte(0x38);pack_byte(0x32);pack_byte(0x31);pack_byte(0x30);pack_byte(0x33);pack_byte(0x30);pack_byte(0x35);pack_byte(0x32);pack_byte(0x33);pack_byte(0x38);pack_byte(0x31);pack_byte(0x30);pack_byte(0x31);pack_byte(0x30);pack_byte(0x31);pack_byte(0x35);pack_byte(0x31);pack_byte(0x30);pack_byte(0x31);pack_byte(0x34);pack_byte(0x63);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x32);pack_byte(0x30);pack_byte(0x30);pack_byte(0x34);pack_byte(0x38);pack_byte(0x31);pack_byte(0x34);pack_byte(0x06);pack_byte(0x30);pack_byte(0x31);pack_byte(0x31);pack_byte(0x32);pack_byte(0x30);pack_byte(0x30);pack_byte(0x08);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x32);pack_byte(0x34);pack_byte(0x32);pack_byte(0x32);pack_byte(0x33);pack_byte(0x33);pack_byte(0x30);pack_byte(0x31);pack_byte(0x33);pack_byte(0x36);pack_byte(0x35);pack_byte(0x39);pack_byte(0x38);pack_byte(0x34);pack_byte(0x38);pack_byte(0x31);pack_byte(0x39);pack_byte(0x3D);pack_byte(0x32);pack_byte(0x31);pack_byte(0x30);pack_byte(0x33);pack_byte(0x32);pack_byte(0x32);pack_byte(0x36);pack_byte(0x31);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x39);pack_byte(0x32);pack_byte(0x34);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x31);pack_byte(0x31);pack_byte(0x38);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x32);pack_byte(0x38);pack_byte(0x31);pack_byte(0x37);pack_byte(0x34);pack_byte(0x39);pack_byte(0x37);pack_byte(0x39);pack_byte(0x31);pack_byte(0x32);pack_byte(0x37);pack_byte(0x30);pack_byte(0x37);pack_byte(0x32);pack_byte(0x38);pack_byte(0x30);pack_byte(0x30);pack_byte(0x37);pack_byte(0x20);pack_byte(0x20);pack_byte(0x20);pack_byte(0x20);pack_byte(0x20);pack_byte(0x11);pack_byte(0x31);pack_byte(0x32);pack_byte(0x33);pack_byte(0x34);pack_byte(0x35);pack_byte(0x36);pack_byte(0x37);pack_byte(0x38);pack_byte(0x45);pack_byte(0x47);pack_byte(0x59);pack_byte(0x00);pack_byte(0x38);pack_byte(0x50);pack_byte(0x31);pack_byte(0x30);pack_byte(0x30);pack_byte(0x33);pack_byte(0x32);pack_byte(0x41);pack_byte(0x35);pack_byte(0x35);pack_byte(0x34);pack_byte(0x41);pack_byte(0x39);pack_byte(0x36);pack_byte(0x34);pack_byte(0x35);pack_byte(0x39);pack_byte(0x45);pack_byte(0x33);pack_byte(0x36);pack_byte(0x38);pack_byte(0x42);pack_byte(0x46);pack_byte(0x36);pack_byte(0x30);pack_byte(0x42);pack_byte(0x39);pack_byte(0x32);pack_byte(0x37);pack_byte(0x45);pack_byte(0x39);pack_byte(0x46);pack_byte(0x34);pack_byte(0x41);pack_byte(0x44);pack_byte(0x31);pack_byte(0x34);pack_byte(0x45);pack_byte(0x39);pack_byte(0x38);pack_byte(0x31);pack_byte(0x38);pack_byte(0x2D);pack_byte(0x6B);pack_byte(0xA9);pack_byte(0x3C);pack_byte(0xE9);pack_byte(0xCF);pack_byte(0x18);pack_byte(0x45);pack_byte(0x09);pack_byte(0x30);pack_byte(0x31);pack_byte(0x30);pack_byte(0x31);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x01);pack_byte(0x40);pack_byte(0x5F);pack_byte(0x2A);pack_byte(0x02);pack_byte(0x08);pack_byte(0x18);pack_byte(0x82);pack_byte(0x02);pack_byte(0x3C);pack_byte(0x00);pack_byte(0x84);pack_byte(0x07);pack_byte(0xA0);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x03);pack_byte(0x10);pack_byte(0x10);pack_byte(0x95);pack_byte(0x05);pack_byte(0x08);pack_byte(0xA0);pack_byte(0x24);pack_byte(0x80);pack_byte(0x00);pack_byte(0x9A);pack_byte(0x03);pack_byte(0x16);pack_byte(0x05);pack_byte(0x23);pack_byte(0x9C);pack_byte(0x01);pack_byte(0x00);pack_byte(0x9F);pack_byte(0x02);pack_byte(0x06);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x14);pack_byte(0x00);pack_byte(0x9F);pack_byte(0x03);pack_byte(0x06);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x9F);pack_byte(0x09);pack_byte(0x02);pack_byte(0x00);pack_byte(0x8C);pack_byte(0x9F);pack_byte(0x10);pack_byte(0x07);pack_byte(0x06);pack_byte(0x01);pack_byte(0x0A);pack_byte(0x03);pack_byte(0xA0);pack_byte(0xE8);pack_byte(0x00);pack_byte(0x9F);pack_byte(0x1A);pack_byte(0x02);pack_byte(0x08);pack_byte(0x18);pack_byte(0x9F);pack_byte(0x1E);pack_byte(0x08);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x9F);pack_byte(0x26);pack_byte(0x08);pack_byte(0x9B);pack_byte(0x17);pack_byte(0x71);pack_byte(0xFB);pack_byte(0x41);pack_byte(0xD5);pack_byte(0xD7);pack_byte(0xA5);pack_byte(0x9F);pack_byte(0x27);pack_byte(0x01);pack_byte(0x80);pack_byte(0x9F);pack_byte(0x33);pack_byte(0x03);pack_byte(0xE0);pack_byte(0xF8);pack_byte(0xC8);pack_byte(0x9F);pack_byte(0x34);pack_byte(0x03);pack_byte(0x42);pack_byte(0x03);pack_byte(0x00);pack_byte(0x9F);pack_byte(0x35);pack_byte(0x01);pack_byte(0x22);pack_byte(0x9F);pack_byte(0x36);pack_byte(0x02);pack_byte(0x00);pack_byte(0x45);pack_byte(0x9F);pack_byte(0x37);pack_byte(0x04);pack_byte(0x9A);pack_byte(0x66);pack_byte(0x1B);pack_byte(0x9D);pack_byte(0x9F);pack_byte(0x41);pack_byte(0x04);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x01);pack_byte(0x9F);pack_byte(0x53);pack_byte(0x01);pack_byte(0x52);pack_byte(0x5F);pack_byte(0x34);pack_byte(0x01);pack_byte(0x00);pack_byte(0x00);pack_byte(0x25);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x30);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);pack_byte(0x00);


  /* 11. systems trace audit number */
  if (tx_bitmap[1] & 0x20) {
    split_data(TX_DATA.sb_trace_no, 3);
  }

  /* 12. local transaction date & time */
  if (tx_bitmap[1] & 0x10) {
    split_data(&TX_DATA.s_dtg.b_year, 6);
  }

  /* 14. expiry date */
  if (tx_bitmap[1] & 0x04) {
    split_data(TX_DATA.sb_exp_date, 2);
  }

  /* 17. capture date (MMDD) */
  if (tx_bitmap[2] & 0x80) {
    split_data(&TX_DATA.s_dtg.b_month, 2);
  }


  //81010151014c
  /* 22. Entry mode */
  if (tx_bitmap[2] & 0x04) {
    pack_byte('8');       // manual / swipe / EMV
    pack_byte('1');       // PIN authentication
    pack_byte('0');       // no card capture
    pack_byte('1');       // attended
    pack_byte('0');       // cardholder present
    pack_byte('1');       // card present
    switch (TX_DATA.b_entry_mode) {
      case MANUAL:
      case MANUAL_4DBC:
        pack_byte('1');
        pack_byte('5');   // signature
        break;
      case SWIPE:
      case SWIPE_4DBC:
        pack_byte('2');
        pack_byte('5');   // signature
        break;
      case FALLBACK:
      case FALLBACK_4DBC:
        pack_byte('8');
        pack_byte('5');   // signature
        break;
      default :
        pack_byte('5');   // chip reading
        pack_byte('1');   // PIN verified
        break;
    }
    pack_byte('0');       // cardholder auth entity
    pack_byte('1');       // rewrite capability
    pack_byte('4');       // display & printing
    pack_byte('c');       // 12-digit PIN
  }

  /* 23. pan sequence number */
  if (tx_bitmap[2] & 0x02) {
    pack_mem("000", 3);                       //testing only
  }

  /* 24. function code */
  if (tx_bitmap[2] & 0x01) {
    if ((TX_DATA.b_trans == AUTH_SWIPE) || (TX_DATA.b_trans == AUTH_MANUAL))
      pack_mem("101", 3);
    else if (TX_DATA.b_trans == EDC_REV)
      pack_mem("400", 3);
    else if (TX_DATA.b_trans == SETTLEMENT)
      pack_mem("880", 3);
    else
      pack_mem("200", 3);
  }

  /* 26. merchant business code */
  if (tx_bitmap[3] & 0x40) {
    pack_mem("4814", 4);                      //testing only
  }

  /* 32. acquirer id */
  if (tx_bitmap[3] & 0x01) {
    pack_byte(0x06);
    pack_mem("011200", 6);                    //testing only
  }

  /* 34. pan extension */
  if (tx_bitmap[4] & 0x40) {
    pack_byte(0x08);
    pack_mem("00000000", 8);                  //testing only
  }

  /* 35. track 2 data */
  if (tx_bitmap[4] & 0x20) {
    if (INPUT.s_trk2buf.b_len) {
      pack_byte((BYTE)bin2bcd(INPUT.s_trk2buf.b_len));
      pack_mem(INPUT.s_trk2buf.sb_content, INPUT.s_trk2buf.b_len);
    } else
      tx_bitmap[4] &= ~0x20;
  }

  /* 37. RRN */
  if (tx_bitmap[4] & 0x08) {
    if ((TX_DATA.b_trans == EDC_REV) && (TX_DATA.b_org_trans != VOID))
      tx_bitmap[4] &= ~0x08; /* not applicable, clear BIT */
    else {
      if (memcmp(TX_DATA.sb_rrn, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12))
        pack_mem(TX_DATA.sb_rrn, 12);
      else
        tx_bitmap[4] &= ~0x08; /* not applicable, clear BIT */
    }
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

  /* 43. card acceptor name & address */
  if (tx_bitmap[5] & 0x20) {
    pack_byte((BYTE)bin2bcd(sizeof(STIS_TERM_CFG.sb_name_loc)));
    pack_mem(STIS_TERM_CFG.sb_name_loc, sizeof(STIS_TERM_CFG.sb_name_loc));
  }
 // 
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
      split_data(INPUT.s_trk1buf.sb_content, INPUT.s_trk1buf.b_len);
    }
  }


  //P10032A554A96459E368BF60B927E9F4AD14E9
  /* 48. additional resp. data */
  if (tx_bitmap[5] & 0x01) {
    pack_mem("0038", 4);                                //Total length of TLVs
    pack_mem("P10", 3);                                 //Tag P10 - PIN encryption key
    pack_mem("032", 3);                                 //length of data
    pack_mem("A554A96459E368BF60B927E9F4AD14E9", 32);   //testing only
  }

  /* 49. transaction currency code */
  if (tx_bitmap[6] & 0x80) {
    pack_mem("818", 3);                       //testing only (EGP)
  }

  /* 52. pin block */
  if (tx_bitmap[6] & 0x10) {
    if (memcmp(TX_DATA.sb_pin, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
      pack_mem (TX_DATA.sb_pin, 8);
    else
      tx_bitmap[6] &= ~0x10; /* not applicable, clear BIT */
  }

  /* 53. security data */
  if (tx_bitmap[6] & 0x08) {
    pack_byte(0x09);
    pack_mem("010100000", 9);               //testing only (encrypted by TPK, ANSI format)
  }

  /* 54. additional amount */
  if (tx_bitmap[6] & 0x04) {
    if (TX_DATA.dd_tip !=0) {
      pack_word(0x0020);
      pack_mem("04", 2);                  // account type   //testing only
      pack_mem("04", 2);                  // account type   //testing only
      pack_mem("818", 3);                 // currency code  //testing only
      pack_mem("C", 1);                   // credit         //testing only
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
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x1220) &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x1400) &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x1320)) {
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

  /* 63. versions */
  if (tx_bitmap[7] & 0x02) {
    pack_word(0x0025);
    pack_nbyte('0', 25);                              //testing only
  }

  /* 64. MAC */
  if (tx_bitmap[7] & 0x01) {
    pack_mem("\x00\x00\x00\x00\x00\x00\x00\x00", 8);  //testing only
  }

  /* 72. Data Record */
  if (tx_bitmap[8] & 0x01) {
    if ((TX_DATA.b_trans == AUTH_SWIPE) || (TX_DATA.b_trans == AUTH_MANUAL)) {
      pack_word(0x0040); /* length */
      pack_mem("6013260019010000001259708402000000036575", 40);   //testing only
    }
  }
 // 
 
PackTxBufLen(ExtraMsgLen());


   SprintfMW(length, "%04d", TX_BUF.wLen - 2);
   memcpy(len_ptr, length, 4);

  if (tx_bitmap[0] & 0x80)
    memcpy(bitmap_ptr, tx_bitmap, 16);
  else
    memcpy(bitmap_ptr, tx_bitmap, 8);


    
  dbgHex("TxMsg", TX_BUF.sbContent, TX_BUF.wLen);         /* JJJ */
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
  BYTE msg_id[4];
  BYTE buf[4];
  BYTE words[12];
  BYTE bitmap[8];
  BYTE tmp;
  BOOLEAN more_msg, sync_datetime;

  // to get in binary mao & with {80,40,20,10,08,04,02,01}

  Disp2x16Msg(GetConstMsg(EDC_CO_PROCESSING), MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (TrainingModeON())
    return TrainingRsp();

  dbgHex("RxMsg", RX_BUF.sbContent, RX_BUF.wLen);         /* JJJ */
  RxBufSetup(ExtraMsgLen());
  more_msg = sync_datetime = 0;

  inc_pptr(14); // 14 BYTE Header

   msg_id[0]=get_byte();
   msg_id[1]=get_byte();
   msg_id[2]=get_byte();
   msg_id[3]=get_byte();

 if (msg_id[0] == 0x31 && msg_id[1] == 0x38 && msg_id[2] == 0x32 && msg_id[3] == 0x30)
    return 0xff; /* please wait message */

   SprintfMW(buf, "%04X", KTransBitmap[TX_DATA.b_trans].w_txmsg_id + 0x10);

   //   printf("%02X:%02X:%02X:%02X", msg_id[0], msg_id[1], msg_id[2], msg_id[3]);
	//  APM_WaitKey(9000, 0);
	//  printf("%02X:%02X:%02X:%02X", buf[0], buf[1], buf[2], buf[3]);
	//  APM_WaitKey(9000, 0);


	  if (memcmp(msg_id, buf, 4)!=0) {
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

	//  SprintfMW(buf, "%04X", TX_DATA.sb_proc_code);

   /* if (memcmp(get_pptr(), buf, 6)!=0) {
      if ((TX_DATA.b_trans != SETTLEMENT)|| (memcmp(get_pptr(),KSetlPCode2,2)!=0)) {
        RSP_DATA.w_rspcode = 'I'*256+'P';
        return TRANS_FAIL;
      }
    }*/
    inc_pptr(6);
   /* tmp = get_byte();
    if ((TX_DATA.b_trans == TRANS_UPLOAD) && (tmp == 0x01))
      more_msg = 1;
    else if ((tmp != 0x01) && tmp) {
      RSP_DATA.w_rspcode = 'I'*256+'P';
      return TRANS_FAIL;
    }*/
  }




  /* 04. amount */
  if (bitmap[0] & 0x10) {

	//  compress(RSP_DATA.dd_amount, get_pptr(), 6);
    RSP_DATA.dd_amount = BcdBin8b(get_pptr(),6);
    inc_pptr(12);
  }


   /* 05. Settlement amount */
  if (bitmap[0] & 0x08) {
    //RSP_DATA.dd_amount = BcdBin8b(get_pptr(),6);
    inc_pptr(12);
  }
     /* 06. unknown */
  if (bitmap[0] & 0x04) {
    //RSP_DATA.dd_amount = BcdBin8b(get_pptr(),6);
    inc_pptr(12);
  }

     /* 07. Transmission date and time */
  if (bitmap[0] & 0x02) {
    //RSP_DATA.dd_amount = BcdBin8b(get_pptr(),6);
    inc_pptr(10);
  }

   

  /* 11. check system trace no */
  if (bitmap[1] & 0x20) {
   /* if (TX_DATA.b_trans != TRANS_UPLOAD) {
      if (!Match(TX_DATA.sb_trace_no, 6)) {
        RSP_DATA.w_rspcode = 'I'*256+'S';
        return TRANS_FAIL;
      }
    } else*/
      inc_pptr(6);
  }

  /* 12. trans time */
  if (bitmap[1] & 0x10) {
    sync_datetime |= 0x10;
    get_mem(&RSP_DATA.s_dtg.b_hour, 12);
  }

  /* 13. trans date */
  if (bitmap[1] & 0x08) {
    sync_datetime |= 0x08;
    get_mem(&RSP_DATA.s_dtg.b_month, 2);
  }

  /* 14. expiry date */
  if (bitmap[1] & 0x04) {
	   compress(RSP_DATA.sb_exp_date, get_pptr(), 2);
       inc_pptr(4);
  }

   /* 17. Capture date */
  if (bitmap[2] & 0x80) {
    inc_pptr(4);
  }
   /* 18. unknown */
  if (bitmap[2] & 0x40) {
   inc_pptr(4);
  }
   /* 22. POS Entry mode */
  if (bitmap[2] & 0x04) {
    inc_pptr(12);
  }
   /* 23. Pan Sequence Number */
  if (bitmap[2] & 0x02) {
    inc_pptr(3);
  }
  /* 24. bypass netword id */
  if (bitmap[2] & 0x01) {
    inc_pptr(3);
  }

  /* 25. Message reason code */
  if (bitmap[3] & 0x80) {
   msg_id[0]=get_byte();
   msg_id[1]=get_byte();
   msg_id[2]=get_byte();
   msg_id[3]=get_byte();
	  printf("%02X:%02X:%02X:%02X", msg_id[0], msg_id[1], msg_id[2], msg_id[3]);
	  APM_WaitKey(9000, 0);
    inc_pptr(4);
  }

   /* 26. Merchant business code */
  if (bitmap[3] & 0x40) {
    inc_pptr(4);
  }

   /* 32.Acquiring institution identification code */
  if (bitmap[3] & 0x01) {
    inc_pptr(6);
  }

   /* 35. 	Track 2 data */
  if (bitmap[4] & 0x20) {
    inc_pptr(3);
  }

  /* 37. retrieval reference number */
  if (bitmap[4] & 0x08) {
	//    compress(RSP_DATA.sb_rrn, get_pptr(), 6);
    //   inc_pptr(12);
    get_mem(RSP_DATA.sb_rrn, 12);
  }



  /* 38. auth code */
  memset(RSP_DATA.sb_auth_code, ' ', 6);
  if (bitmap[4] & 0x04) {
	//    compress(RSP_DATA.sb_auth_code, get_pptr(), 3);
    //   inc_pptr(6);
    get_mem(RSP_DATA.sb_auth_code, 6);
  }

    

  /* 39. 	Action code  */
  RSP_DATA.w_rspcode = '0'*256+'0'; // upload response does not have response code
  if (bitmap[4] & 0x02) {
  //  RSP_DATA.w_rspcode=get_word();

	 get_mem(words, 3);
   //  printf("\x1b\xc0%02x: %c%c", RSP_DATA.w_rspcode, RSP_DATA.w_rspcode>>8, RSP_DATA.w_rspcode&0xFF); //!TT
    // Delay1Sec(3, 0);
  }

  /* 41. 	Card acceptor device identification */
  if (bitmap[5] & 0x80) {
    //if (!Match(STIS_ACQ_TBL(0).sb_term_id, 8)) {
    //  RSP_DATA.w_rspcode = 'I'*256+'T';
    //  return TRANS_FAIL;
    //}

	 inc_pptr(15);
  }


 /* 42.Card acceptor identification */
  if (bitmap[5] & 0x40) {
	 inc_pptr(15);
  }

 
   /* 43.	Card acceptor name and address */
  if (bitmap[5] & 0x20) {
	 inc_pptr(15);
  }

   /* 48.	Provite Additional Data */
  if (bitmap[5] & 0x01) {
	 inc_pptr(75);
  }

   /* 49.	Transaction currency code */
  if (bitmap[6] & 0x80) {
	 inc_pptr(3);
  }

   /* 50 .Settlement currency code */
  if (bitmap[6] & 0x40) {
	 inc_pptr(3);
  }

    /* 52 .	Pin Data */
  if (bitmap[6] & 0x10) {
	 inc_pptr(3);
  }

    /* 53 .	Security data*/
  if (bitmap[6] & 0x08) {
	    var_i = bcd2bin(get_byte());
	 inc_pptr(((var_i+1)/2)); //confirm lenght
  }


  /* 54. additional amount */
  RSP_DATA.dd_tip=0;
  if (bitmap[6] & 0x04) {
	  var_i = bcd2bin(get_word());
	  get_mem(RSP_DATA.dd_tip, ((var_i+1)/2));
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
  if (bitmap[7] & 0x02) {
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

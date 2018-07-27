//-----------------------------------------------------------------------------
//  File          : print.c
//  Module        :
//  Description   : Include print utility functions.
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
#include "hardware.h"
#include "corevar.h"
#include "chkoptn.h"
#include "constant.h"
#include "logo.h"
#include "message.h"
#include "lptutil.h"
#include "print.h"

//-----------------------------------------------------------------------------
// Print related
//-----------------------------------------------------------------------------
static char KLastRcptName[]     = { "EDCReprn" };


//*****************************************************************************
//  Function        : PackLogo
//  Description     : Pack receipt logo.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackLogo(void)
{
  pack_nbyte(' ', ((MAX_CHAR_LINE_NORMAL-23)/2));
  pack_mem(STIS_TERM_CFG.sb_dflt_name, 23);
  pack_lf();
  pack_nbyte(' ', ((MAX_CHAR_LINE_NORMAL-23)/2));
  pack_mem(STIS_TERM_CFG.sb_name_loc, 23); /* line 2 */
  pack_lf();
  pack_nbyte(' ', ((MAX_CHAR_LINE_NORMAL-23)/2));
  pack_mem(&STIS_TERM_CFG.sb_name_loc[23], 23); /* line 3 */
  pack_lf();
}
//*****************************************************************************
//  Function        : PackDtg
//  Description     : Pack DATE/TIME on receipt.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackDtg(struct DATETIME *aDtg)
{
  BYTE tmp[18];

  ConvDateTime(tmp, aDtg, 1);
  pack_mem(tmp, 12);
  if (PrintTimeReqd())
    pack_mem(&tmp[12], 6);
  else
    pack_nbyte(' ', 6);
}
//*****************************************************************************
//  Function        : PackCurrentDate
//  Description     : Pack current date/time on receipt.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackCurrentDate(void)
{
  BYTE buf[64];
  

   pack_str("Date:");
   os_rtc_get(buf);
    pack_mem(buf, 4);
    pack_space(1);
    pack_mem(&buf[4], 2);
    pack_space(1);
    pack_mem(&buf[6], 2);
    pack_space(4);
    pack_str("Time:");
    pack_mem(&buf[8], 2);
    pack_str(":");
    pack_mem(&buf[10], 2);
    pack_str(":");
    pack_mem(&buf[12], 2);//ss
	pack_lf();
}
//*****************************************************************************
//  Function        : PackSeperator
//  Description     : Pack receipt seperator line.
//  Input           : aChar;        // character to be print.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackSeperator(BYTE aChar)
{
  pack_nbyte(aChar, MAX_CHAR_LINE_NORMAL);
  pack_lf();
}
//*****************************************************************************
//  Function        : PackRcptHeader
//  Description     : Pack EDC receipt header.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackRcptHeader(void)
{
  pack_mem("TERM ID :", 9);
  pack_mem(STIS_ACQ_TBL(0).sb_term_id, 8);
  pack_lf();
  pack_mem("MERCH ID:", 9);
  pack_mem(STIS_ACQ_TBL(0).sb_acceptor_id, 15);
  pack_lf();
}
//*****************************************************************************
//  Function        : PackAmt
//  Description     : Pack amount to predefine buffer.
//  Input           : aAmount;
//                    aOffset;          // start offset
//  Return          : len of amount string;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE PackAmt(DDWORD aAmount, BYTE aOffset)
{
  struct DISP_AMT disp_amt;

  ConvAmount(aAmount, &disp_amt, STIS_TERM_CFG.b_decimal_pos, STIS_TERM_CFG.b_currency);
  pack_space((BYTE)(aOffset - disp_amt.len));
  pack_mem(disp_amt.content, disp_amt.len);
  return(disp_amt.len);
}
//*****************************************************************************
//  Function        : PackFF
//  Description     : Pack Form Seperator line.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackFF(void)
{
  #if (LPT_SUPPORT)    
  pack_byte(0x0C);
  #else
  pack_lfs(2);
  pack_nbyte('_', MAX_CHAR_LINE_NORMAL);
  pack_lfs(4);
  #endif  
}
//*****************************************************************************
//  Function        : PackCountAmt
//  Description     : Pack count & amount;
//  Input           : count;
//                    amount;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PackCountAmt(WORD aCount, DDWORD aAmount)
{
  pack_space(2);
  if(aCount<10){
  bindec_data(aCount, 1);
  PackAmt(aAmount, 16);
  }else if(aCount >= 10){
   bindec_data(aCount, 2);
   PackAmt(aAmount, 15);
  }else{
   bindec_data(aCount, 3);
   PackAmt(aAmount, 14);
  }
  
  pack_lf();
}


//*****************************************************************************
//  Function        : PackTotals
//  Description     : Pack total data.
//  Input           : aTot;    // pointer to data struct.
//  Return          : amount str's len;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackTotals(struct TOTAL_STRUCT *aTot)
{
  static const BYTE KBase[]            = {"BASE         "};
  static const BYTE KTips[]            = {"TIPS         "};
  static const BYTE KSales[]           = {"SALES        "};
  static const BYTE KRefund[]          = {"REFND        "};
   static const BYTE KTotal[]          = {"TOTAL        "};
  static const BYTE c_voided_sales[]   = {"VOIDED SALES "};
  static const BYTE c_voided_refund[]  = {"VOIDED REFUND"};
  DDWORD amount;



  if (TIPsReqd()) {
    pack_mem((BYTE *)KBase, 6);
    amount = aTot->s_sale_tot.dd_amount;
    amount -= aTot->s_tips_tot.dd_amount;
    PackCountAmt(aTot->s_sale_tot.w_count, amount);
    pack_mem((BYTE *)KTips, 6);
    PackCountAmt(aTot->s_tips_tot.w_count, aTot->s_tips_tot.dd_amount);
  }

  pack_mem((BYTE *)KSales, 13);
  PackCountAmt(aTot->s_sale_tot.w_count, aTot->s_sale_tot.dd_amount);


  pack_mem((BYTE *)(KRefund), 13);
  PackCountAmt(aTot->s_refund_tot.w_count, aTot->s_refund_tot.dd_amount);


  pack_mem((BYTE *)c_voided_sales, 13);
  PackCountAmt(aTot->s_void_sale.w_count, aTot->s_void_sale.dd_amount);


  pack_mem((BYTE *)(c_voided_refund), 13);
  PackCountAmt(aTot->s_void_refund.w_count, aTot->s_void_refund.dd_amount);
  PackSeperator('-');

  pack_mem((BYTE *)KTotal, 13);
  PackCountAmt(aTot->s_ALL.w_count, aTot->s_ALL.dd_amount);

}
//*****************************************************************************
//  Function        : PackEndOfRep
//  Description     : Pack END OF REPORT line.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackEndOfRep(void)
{
  char KEOR[]        = {"\x1BW1END OF REPORT\x1BW0"};
  pack_lf();
  pack_space(2);
  pack_mem(KEOR, strlen(KEOR));
  pack_lf();
}
//*****************************************************************************
//  Function        : SaveLastRcpt
//  Description     : Save Last Rcpt Copy.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveLastRcpt(BYTE *aDat, WORD aLen)
{
  int fd;
  char filename[32];

  if (aLen == 0)
    return;

  strcpy(filename, KLastRcptName);
  fd = fDeleteMW(filename);
  fd = fCreateMW(filename, FALSE);
  if (fd < 0) {
    DispLineMW("Reprint File ERR!", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return;
  }
  fWriteMW(fd, aDat, aLen);
  fCloseMW(fd);
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : GetLastRcpt
//  Description     : Get Last Rcpt Copy.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetLastRcpt(BYTE *aDat, WORD aLen)
{
  int fd, len;
  char filename[32];

  strcpy(filename, KLastRcptName);
  fd = fOpenMW(filename);
  if (fd < 0) {
    return -1;
  }
  len = fReadMW(fd, aDat, aLen);
  fCloseMW(fd);
  return len;
}
//*****************************************************************************
//  Function        : ReprintLast
//  Description     : Reprint Last Saved Receipt.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ReprintLast(void)
{
  int len;

  len = GetLastRcpt(MSG_BUF.sb_content, sizeof(MSG_BUF.sb_content));
  if (len == -1)
    return;
  DispLineMW(GetConstMsg(EDC_FN_REPRINT_BATCH), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING),MW_LINE5, MW_BIGFONT);
  PrintBuf(MSG_BUF.sb_content, len, FALSE, FALSE);
}
//*****************************************************************************
//  Function        : ManualFeed
//  Description     : Feed Paper.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ManualFeed(void)
{
  BYTE tmp[] = {"\x1BJ\x20" };

  IOCtlMW(gLptHandle, MW_TMLPT_RESET, NULL);
  LptPutN(tmp, 3);
}


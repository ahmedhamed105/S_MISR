//-----------------------------------------------------------------------------
//  File          : Others.c
//  Module        :
//  Description   : Include routines for other transactions.
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
#include "apm.h"
#include "sysutil.h"
#include "constant.h"
#include "corevar.h"
#include "tranutil.h"
#include "input.h"
#include "sale.h"
#include "saleoffl.h"
#include "auth.h"
#include "refund.h"
#include "void.h"
#include "adjust.h"
#include "settle.h"
#include "menu.h"
#include "emvtrans.h"
#include "brdrtest.h"
#include "others.h"


// Default Other Trans Menu
const struct MENU_ITEM KOtherTxnItem [] = {
  {  0, "Settlement"},
  {  1, "Void"},
  {  2, "SALE OFFLINE"},
  {  3, "Refund"},
  {  4, "Auth"},
  {  5, "Adjustment"},
  {  6, "Installment"},
  {  7, "Reversal"},
  {  8, "Barcode Txn"},
  {-1, NULL},
};

const struct MENU_ITEM KOtherTxncurr [] = {
  {  1, "EGP"},
  {  2, "USD"},
  {  3, "EURO"},
  {-1, NULL},
};

const struct MENU_ITEM KOthersett [] = {
  {  0, "Settlement"},
  {  1, "Settlement Void"},
  {-1, NULL},
};


const struct MENU_ITEM KOtherrefund [] = {
  {  0, "online"},
  {  1, "offline"},
  {-1, NULL},
};


const struct MENU_ITEM KOtherinstall [] = {
  {  1, "6"},
  {  2, "9"},
  {  3, "10"},
  {  4, "12"},
  {-1, NULL},
};


const struct MENU_DAT KOtherTransMenu  = {
  "Trans Selection",
  KOtherTxnItem,
};

const struct MENU_DAT KOthercurrMenu  = {
  "Currency Select",
  KOtherTxncurr,
};


const struct MENU_DAT KOtherTranssett  = {
  "Settlement Select",
  KOthersett,
};


const struct MENU_DAT KOtherTransrefund  = {
  "refund Select",
  KOthersett,
};

const struct MENU_DAT KOtherTransinstall  = {
  "Installment Select",
  KOtherinstall,
};

BOOLEAN GetAccessrefund(void)
{
  DWORD retry;
  BYTE kbdbuf[MW_MAX_LINESIZE+1];
  os_disp_putc('\f');
        ClearDispMW();
 memset(kbdbuf, 0, sizeof(kbdbuf));
  DispLineMW("ACCESS CODE", MW_LINE3, MW_CENTER|MW_BIGFONT);
  DispLineMW(kbdbuf, MW_LINE5, MW_CENTER|MW_BIGFONT);

  retry = 3;
  while (retry--) {
    if (!APM_GetKbd(HIDE_NUMERIC+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(16)+IMAX(16), kbdbuf))
      return FALSE;

    if (memcmp(STIS_TERM_CFG.sb_psw_refund, &kbdbuf[1], 16) == 0)
      return TRUE;

    LongBeep();
  }

  return FALSE;
}
BOOLEAN GetAccessrevoid(void)
{
  DWORD retry;
  BYTE kbdbuf[MW_MAX_LINESIZE+1];

  os_disp_putc('\f');
        ClearDispMW();
		memset(kbdbuf, 0, sizeof(kbdbuf));
  DispLineMW("ACCESS CODE", MW_LINE3, MW_CENTER|MW_BIGFONT);
  DispLineMW(kbdbuf, MW_LINE5, MW_CENTER|MW_BIGFONT);

  retry = 3;
  while (retry--) {
    if (!APM_GetKbd(HIDE_NUMERIC+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(16)+IMAX(16), kbdbuf))
      return FALSE;

	if (memcmp(STIS_TERM_CFG.sb_psw_void, &kbdbuf[1], 16) == 0)
      return TRUE;

    LongBeep();
  }

  return FALSE;
}
//*****************************************************************************
//  Function        : OtherTrans
//  Description     : Show transaction select screen & run user's choice.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void OtherTrans(void)
{
  int select;
  int host_idx;
  DWORD keyin;

  select = 0;
  select = MenuSelect(&KOtherTransMenu, select);
  if (select == -1)
    return;

  switch (select) {
case 0:
  select = 0;
  select = MenuSelect(&KOtherTranssett, select);
  if (select == -1)
    return;
  switch (select) {
    case 0:
		if (GetAccessrefund()){
      host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
		}
      break;
	   case 1:
		 DispLineMW("Not Permit", MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT);
            	while(true){
   keyin = APM_GetKeyin();
  if (keyin == MWKEY_ENTER)
    break;
  }
      break;
  }

		break;

case 1:
		if (GetAccessrevoid())
      VoidTrans(-1);

		DispCtrlMW(K_SelSmFont);
      break;
case 2:

      WaitCardTrans(SALE_OFFLINE);
      ResetTerm();
      break;
case 3:
	if (GetAccessrefund())
      WaitCardTrans(REFUND);
		break;

	  DispCtrlMW(K_SelSmFont);
      ResetTerm();
      break;
case 4:
      WaitCardTrans(AUTH_SWIPE);
      ResetTerm();
      break;
case 5:
      AdjustTrans(-1);
      break;
case 6:
      select = 0;
  select = MenuSelect(&KOtherTransinstall, select);
  if (select == -1)
    return;
  switch (select) {
    case 0:
	 WaitCardTrans(INST_six);
      ResetTerm();
      break;
	   case 1:
		 DispLineMW("Not Permit", MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT);
            	while(true){
   keyin = APM_GetKeyin();
  if (keyin == MWKEY_ENTER)
    break;
  }
      break;
	    case 2:
		 DispLineMW("Not Permit", MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT);
            	while(true){
   keyin = APM_GetKeyin();
  if (keyin == MWKEY_ENTER)
    break;
  }
      break;
	    case 3:
		 DispLineMW("Not Permit", MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT);
            	while(true){
   keyin = APM_GetKeyin();
  if (keyin == MWKEY_ENTER)
    break;
  }
  }
      break;
case 7:
      WaitCardTrans(rev_ICC);
      ResetTerm();
      break;
case 8:
      if (BRdrGetAmount() == TRUE)
        WaitCardTrans(SALE_OFFLINE);
      break;

   
  }
}


void OtherCurrency(void)
{
  int select;

  select = 0;
  select = MenuSelect(&KOthercurrMenu, select);
  if (select == -1)
    return;

  switch (select) {
	  case 1:
      INPUT.currency=0;
      break;
	  case 2:
      INPUT.currency=1;
	  
      break;
	  case 3:
     INPUT.currency=2;
      break;
   
  }
}
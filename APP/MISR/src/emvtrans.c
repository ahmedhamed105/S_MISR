//-----------------------------------------------------------------------------
//  File          : emvtrans.c
//  Module        :
//  Description   : Include routines for EMV transactions.
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
#include "apm.h"
#include "util.h"
#include "sysutil.h"
#include "lptutil.h"
#include "message.h"
#include "corevar.h"
#include "keytrans.h"
#include "constant.h"
#include "chkoptn.h"
#include "input.h"
#include "print.h"
#include "hostmsg.h"
#include "record.h"
#include "reversal.h"
#include "offline.h"
#include "sale.h"
#include "tranutil.h"
//#include "emvutil.h"
#include "tlvutil.h"
#include "auxcom.h"
#include "emvtrans.h"

//-----------------------------------------------------------------------------
//    Globals Variables
//-----------------------------------------------------------------------------
static BOOLEAN   bAcceptReferral;
BOOLEAN gIsMaestro;

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
#define bAction         gGDS->s_EMVOut.bMsg[1]

// state machine def (order may not be as follow)
#define EMV_COMPLETE      0x00
#define EMV_START         0x01
#define EMV_SELECT_APP    0x02
#define EMV_ONLINE_PIN    0x03
#define EMV_ONLINE        0x04
#define EMV_REFERRAL      0x05
#define EMV_AMOUNT_AIP    0x06
#define EMV_CONFIRM_CARD  0x07
#define EMV_AMOUNT        0x08
#define EMV_RESTART       0x09

//-----------------------------------------------------------------------------
//    Constant
//-----------------------------------------------------------------------------
const WORD wVMOnlineTags[] = { 
  0x71,
  0x72,
  0x82,
  0x84,
  0x86,
  0x8A,
  0x91,
  0x95,
  0x9A,
  0x9C,
  0x5F2A,
  0x9F02,
  0x9F03,
  0x9F09,
  0x9F10,
  0x9F1A,
  0x9F1E,
  0x57,
  0x9F26,
  0x9F27,
  0x9F33,
  0x9F34,
  0x9F35,
  0x9F36,
  0x9F37,
  0x9F41,
  0x9F53,
  0
};

const BYTE KPinMsg[] = {     /* PIN related message (max 48 bytes x 4) */
  "**** Enter PIN ****\x0"                /*   1. Enter PIN      */
  "---- PIN  OK ----\x0"                  /*   2. PIN OK         */
  " Incorrect  PIN \x0"                   /*   3. Incorrect PIN  */
  "  Last PIN Try  \x0"                   /*   4. Last PIN Try   */
};

//*****************************************************************************
//  Function        : DeAss
//  Description     : decode and count TLV object from the message buffer
//  Input           : *pbMsg;   // pointer to message
//                    wLen;     // len of message
//  Return          : number of Tag in message
//  Note            : N/A
//  Globals Changed : decode TLV objects store to asTagC structure
//*****************************************************************************
BYTE DeAss(TLIST *aList, BYTE *pbMsg, WORD wLen)
{
  BYTE bIdx;
  BYTE * pbMark;
  WORD wInc;

  if (wLen) {
    bIdx = 0;
    do {
      if (*pbMsg == 0 || *pbMsg == 0xFF) {
        ++pbMsg;
        --wLen;
      } else {
        if (bIdx == MAX_TAG_LIST - 1)
          break;
        //get Tag
        aList[bIdx].wTag = TlvTag(pbMark = pbMsg);
        //get Length
        wInc = TlvLen(pbMsg);
        aList[bIdx].sLen = (BYTE) wInc;
        //get buffer pointer
        aList[bIdx].pbLoc = TlvVPtr(pbMsg);
        pbMsg += TlvSizeOf(pbMsg);
        if (pbMsg - pbMark <= wLen)
          wLen -= pbMsg - pbMark;
        else
          break;
        if (wInc > TLMax)               /* length out of range */
          break;
        ++bIdx;
      }
      if (wLen == 0) {
        aList[bIdx].wTag = 0;
        return bIdx;
      }
    } while (TRUE);
  }
  aList[0].wTag = 0;
  return 0;
}
//*****************************************************************************
//  Function        : DispLabel
//  Description     : Display Application Label.
//  Input           : TLIST pointer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static void DispLabel(TLIST *psLp)
{
  BYTE i;
  BYTE len;
  BYTE buffer[MW_MAX_LINESIZE+1];

  memcpy(buffer,psLp->pbLoc,len = ((psLp->sLen < MW_MAX_LINESIZE) ? psLp->sLen : MW_MAX_LINESIZE));
  for (i=0;i<len;i++)
    if ((buffer[i] & 0x7f) < 0x20)
      buffer[i] = ' ';
  buffer[len] = 0;
  DispClrBelowMW(MW_LINE7);
  DispLineMW(buffer, MW_LINE7, MW_BIGFONT);
}
//*****************************************************************************
//  Function        : EMVChoice
//  Description     : Selection Application
//  Input           : N/A
//  Return          : FALSE => CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN EMVChoice(void)
{
  BYTE bChoice;
  BYTE bIdx;
  DWORD keyin;

  //no of tag in response buffer not in group of 2 (AID and display label)
  if ((bChoice = DeAss(gGDS->s_TagList, &gGDS->s_EMVOut.bMsg[2],
                       (WORD)(gGDS->s_EMVOut.wLen - 2)) / 2) == 0) {
    EMVFatal();
  }
  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(GetConstMsg(EDC_EMV_SEL_APP), MW_LINE3, MW_BIGFONT);
  bIdx = 0;
  do {
    //pointer to the 2nd tag (display label), each step increment by 2
    DispLabel(&gGDS->s_TagList[bIdx * 2 + 1]);
    do {
      keyin = APM_WaitKey(KBD_TIMEOUT,0);
      if (keyin == MWKEY_CANCL)
        return FALSE;
      if (keyin == MWKEY_ENTER) {
        gGDS->s_EMVIn.bMsg[0] = bIdx;
        return TRUE;
      }
      if (keyin == MWKEY_CLR)
        break;
    } while (TRUE);
    if (++bIdx == bChoice) {
      if (bChoice == 1)
        return FALSE;
      bIdx = 0;
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : EMVInput
//  Description     : Get user input for EMV transaction.
//  Input           : N/A
//  Return          : FALSE => CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EMVInput(BOOLEAN aSelAgain)
{
  if (aSelAgain) {
    DispErrorMsg(GetConstMsg(EDC_EMV_CANNOT_SEL));
  }
  //one application and confirmation required or more application for selection
  if ((gGDS->s_EMVIn.bMsg[0]=gGDS->s_EMVOut.bMsg[1])) {
    if (EMVChoice() == FALSE)
      return FALSE;
  }
  memcpy(&gGDS->s_EMVIn.bMsg[1], "\x9F\x41\x03", 3);
  memcpy(&gGDS->s_EMVIn.bMsg[4], STIS_TERM_DATA.sb_trace_no, 3);
  gGDS->s_EMVIn.wLen = 7;
  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING),MW_LINE5, MW_BIGFONT);
  return TRUE;
}
/******************************************************************************
 *  Function        : TagPut
 *  Description     : Save Tag to input buffer.
 *  Input           : aBuf;     // Data buffer
 *                    aTag;     // Tag
 *  Return          : new buffer pointer
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BYTE *TagPut(BYTE *aBuf, WORD aTag)
{
  if (aTag / 256)
    *aBuf++ = aTag / 256;
  *aBuf = (BYTE)aTag;
  return aBuf + 1;
}
/******************************************************************************
 *  Function        : TagSeek
 *  Description     : Tag List Seek
 *  Input           : Tag;
 *  Return          : TLIST pointer
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
TLIST *TagSeek(TLIST *aSrc, WORD aTag)
{
  while (aSrc->wTag) {
    if (aSrc->wTag == aTag)
      return aSrc;
    ++aSrc;
  }
  return NULL;
}
/******************************************************************************
 *  Function        : TagData
 *  Description     : Seek for Tag Data
 *  Input           : aTag;   // Tag
 *  Return          : Pointer to Tag data
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BYTE *TagData(TLIST *aSrc, WORD aTag)
{
  while (aSrc->wTag) {
    if (aSrc->wTag == aTag)
      return aSrc->pbLoc;
    ++aSrc;
  }
  return NULL;
}
//*****************************************************************************
//  Function        : PackTagsData
//  Description     : Prepare tag data
//  Input           : aBuf;         // pointer to dest buffer.
//                    aTags;        // list of tags
//  Return          : End of buffer pointer;
//  Note            : N/A
//  Globals Changed : N/a
//*****************************************************************************
BYTE *PackTagsData(BYTE *aBuf, WORD *aTags)
{
  WORD tag;
  TLIST *psLp;

  while ((tag = *aTags) != 0) {
    if ((psLp = TagSeek(gGDS->s_TagList, tag)) != NULL) {
      aBuf = TagPut(aBuf, tag);
      *aBuf++ = psLp->sLen; // assume byte len
      memcpy(aBuf, psLp->pbLoc, psLp->sLen);
      aBuf += psLp->sLen;
    }
    aTags++;  // next tag word
  }
  return aBuf;
}
//*****************************************************************************
//  Function        : EMVFatal
//  Description     : EMV error handler.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
void EMVFatal(void)
{
  Disp2x16Msg(GetConstMsg(EDC_EMV_FATAL), MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  while (APM_WaitKey((DWORD)-1,0) != MWKEY_CANCL) SleepMW();
  ResetMW();
}
//*****************************************************************************
//  Function        : GetApplNameTag
//  Description     : Prepare app name tag if issuer code table is supported.
//  Input           : N/A.
//  Return          : tag.
//  Note            : Pack buffer must be setup before calling this func.
//  Globals Changed : N/A
//*****************************************************************************
WORD GetApplNameTag(void)
{
  TLIST * ptag;   // temp tag data ptr

  ptag = TagSeek(gGDS->s_TagList, 0x9F11);  // issuer code table
  if ( ptag != NULL )                // tag value is found
    if (*ptag->pbLoc==1)             // only supported these 1 issuer code tbl
      if (TagSeek(gGDS->s_TagList, 0x9F12)) // app preferred name exists
        return 0x9F12;               // app preferred name
  return 0x50;                       // app label
}
//*****************************************************************************
//  Function        : ePANExtract
//  Description     : Extract PAN data from EMV data.
//  Input           : N/A
//  Return          : FALSE => Error;
//  Note            : N/A
//  Globals Changed : gInput;
//*****************************************************************************
static BOOLEAN ePANExtract(void)
{
  TLIST *psLp;
  BYTE bLen;

  if (DeAss(gGDS->s_TagList, gGDS->s_EMVOut.bMsg+2, gGDS->s_EMVOut.wLen - 2)) {
    if ((psLp = TagSeek(gGDS->s_TagList, 0x5A)) != NULL) {
      if ((bLen = psLp->sLen) <= 10) {
        memset(INPUT.sb_pan, 0xFF, 10);
        memcpy(INPUT.sb_pan, psLp->pbLoc, bLen);
        //INPUT.sb_pan_len = bLen;
        if ((INPUT.sb_pan[9] & 0x0F) == 0x0F) {
          if (InCardTable()) {
            //if ((psLp = TagSeek(gGDS->s_TagList, 0x5F34)) != NULL)
            //  INPUT.sb_pan_seqno = psLp->pbLoc[0];
            //else
            //  INPUT.sb_pan_seqno = 0;
            return TRUE;
          }
        }
        RSP_DATA.w_rspcode = 'U'*256+'C';
        return FALSE;
      }
    }
  }
  EMVFatal();
  return FALSE;
}
//*****************************************************************************
//  Function        : ValidEMVData
//  Description     : Validate & Extract EMV Data
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : gGTS;
//*****************************************************************************
BOOLEAN ValidEMVData(void)
{
  TLIST * psLp;
  BYTE bLen;

  if (ePANExtract() == FALSE) {
    return FALSE;
  }

  memcpy(&INPUT.sb_exp_date, TagData(gGDS->s_TagList, 0x5F24), 2); //App. Exp. Date


////field71[128];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x71)) != NULL) {      //Issuer Script Template 1 (Tag 71)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field71))
//		bLen = sizeof(INPUT.field71);
//
//	memcpy(INPUT.field71, psLp->pbLoc, bLen);
//}
////field72[128];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x72)) != NULL) {      //Issuer Script Template 2 (Tag 72)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field72))
//		bLen = sizeof(INPUT.field72);
//
//	memcpy(INPUT.field72, psLp->pbLoc, bLen);
//}
////field82[2];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x82)) != NULL) {      //Application Interchange Profile (Tag 82)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field82))
//		bLen = sizeof(INPUT.field82);
//
//	memcpy(INPUT.field82, psLp->pbLoc, bLen);
//}
////field84[16];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x84)) != NULL) {      //Dedicates File Name (Tag 84)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field84))
//		bLen = sizeof(INPUT.field84);
//
//	memcpy(INPUT.field84, psLp->pbLoc, bLen);
//}
////field86[21];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x86)) != NULL) {      //Issuer script command (Tag 86)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field86))
//		bLen = sizeof(INPUT.field86);
//
//	memcpy(INPUT.field86, psLp->pbLoc, bLen);
//}
////field8A[2];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x8A)) != NULL) {      //Authorization response code (Tag 8A)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field8A))
//		bLen = sizeof(INPUT.field8A);
//
//	memcpy(INPUT.field8A, psLp->pbLoc, bLen);
//}
////field91[16];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x91)) != NULL) {      //Issuer Authentication Data (Tag 91)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field91))
//		bLen = sizeof(INPUT.field91);
//
//	memcpy(INPUT.field91, psLp->pbLoc, bLen);
//}
////field95[5];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x95)) != NULL) {      //Terminal Verification Results (Tag 95)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field95))
//		bLen = sizeof(INPUT.field95);
//
//	memcpy(INPUT.field95, psLp->pbLoc, bLen);
//}
////field9A[3];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9A)) != NULL) {      //Transaction Date (Tag 9A)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9A))
//		bLen = sizeof(INPUT.field9A);
//
//	memcpy(INPUT.field9A, psLp->pbLoc, bLen);
//}
////field9C;
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9C)) != NULL) {      //Transaction Type (Tag 9C)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9C))
//		bLen = sizeof(INPUT.field9C);
//
//	memcpy(INPUT.field9C, psLp->pbLoc, bLen);
//}
////field5F2A[2];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x5F2A)) != NULL) {      //Transaction Currency Code (Tag 5F2A)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field5F2A))
//		bLen = sizeof(INPUT.field5F2A);
//
//	memcpy(INPUT.field5F2A, psLp->pbLoc, bLen);
//}
////field9F02[6];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F02)) != NULL) {      //Transaction Amount (Tag 9F02)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F02))
//		bLen = sizeof(INPUT.field9F02);
//
//	memcpy(INPUT.field9F02, psLp->pbLoc, bLen);
//}
////field9F03[6];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F03)) != NULL) {      //Other Amount (Tag 9F03)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F03))
//		bLen = sizeof(INPUT.field9F03);
//
//	memcpy(INPUT.field9F03, psLp->pbLoc, bLen);
//}
////field9F09[2];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F09)) != NULL) {      //Terminal Application Version Number (Tag 9F09)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F09))
//		bLen = sizeof(INPUT.field9F09);
//
//	memcpy(INPUT.field9F09, psLp->pbLoc, bLen);
//}
////field9F10[32];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F10)) != NULL) {      //Issuer Application Data (Tag 9F10)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F10))
//		bLen = sizeof(INPUT.field9F10);
//
//	memcpy(INPUT.field9F10, psLp->pbLoc, bLen);
//}
////field9F1A[2];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F1A)) != NULL) {      //Terminal Country Code (Tag 9F1A)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F1A))
//		bLen = sizeof(INPUT.field9F1A);
//
//	memcpy(INPUT.field9F1A, psLp->pbLoc, bLen);
//}
////field9F1E[8];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F1E)) != NULL) {      //Interface Device (IFD) Serial number (Tag 9F1E)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F1E))
//		bLen = sizeof(INPUT.field9F1E);
//
//	memcpy(INPUT.field9F1E, psLp->pbLoc, bLen);
//}
////field57[19];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x57)) != NULL) {      //Track II Equivalent Data (Tag 57)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field57))
//		bLen = sizeof(INPUT.field57);
//
//	memcpy(INPUT.field57, psLp->pbLoc, bLen);
//}
////field9F26[8];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F26)) != NULL) {      //Application cryptogram (Tag 9F26)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F26))
//		bLen = sizeof(INPUT.field9F26);
//
//	memcpy(INPUT.field9F26, psLp->pbLoc, bLen);
//}
////field9F27;
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F27)) != NULL) {      //Cryptogram Information Data (Tag 9F27)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F27))
//		bLen = sizeof(INPUT.field9F27);
//
//	memcpy(INPUT.field9F27, psLp->pbLoc, bLen);
//}
////field9F33[3];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F33)) != NULL) {      //Terminal Capabilities  (Tag 9F33)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F33))
//		bLen = sizeof(INPUT.field9F33);
//
//	memcpy(INPUT.field9F33, psLp->pbLoc, bLen);
//}
////field9F34[4];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F34)) != NULL) {      //Cardholder Verification Method Results (Tag 9F34)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F34))
//		bLen = sizeof(INPUT.field9F34);
//
//	memcpy(INPUT.field9F34, psLp->pbLoc, bLen);
//}
////field9F35;
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F35)) != NULL) {      //Terminal Type (Tag 9F35)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F35))
//		bLen = sizeof(INPUT.field9F35);
//
//	memcpy(INPUT.field9F35, psLp->pbLoc, bLen);
//}
////field9F36[2];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F36)) != NULL) {      //Application Transaction Counter (ATC) (Tag 9F36)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F36))
//		bLen = sizeof(INPUT.field9F36);
//
//	memcpy(INPUT.field9F36, psLp->pbLoc, bLen);
//}
////field9F37[4];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F37)) != NULL) {      //Unpredictable Number (Tag 9F37)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F37))
//		bLen = sizeof(INPUT.field9F37);
//
//	memcpy(INPUT.field9F37, psLp->pbLoc, bLen);
//}
////field9F41[4];
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F41)) != NULL) {      //Transaction Sequence Number (Tag 9F41)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F41))
//		bLen = sizeof(INPUT.field9F41);
//
//	memcpy(INPUT.field9F41, psLp->pbLoc, bLen);
//}
////field9F53;
//if ((psLp = TagSeek(gGDS->s_TagList, 0x9F53)) != NULL) {      //Transaction Category Code (Tag 9F53)
//	if ((bLen = psLp->sLen) > sizeof(INPUT.field9F53))
//		bLen = sizeof(INPUT.field9F53);
//
//	memcpy(INPUT.field9F53, psLp->pbLoc, bLen);
//}




  if ((psLp = TagSeek(gGDS->s_TagList, 0x5F20)) != NULL) {      //card holder name
    if ((bLen = psLp->sLen) > sizeof(INPUT.sb_holder_name))
      bLen = sizeof(INPUT.sb_holder_name);
    //FormatHolderName(INPUT.sb_holder_name, psLp->pbLoc, bLen);
    memcpy(INPUT.sb_holder_name, psLp->pbLoc, bLen);
  }

  INPUT.s_trk2buf.b_len = 0;
  if ((psLp = TagSeek(gGDS->s_TagList, 0x57)) != NULL) { /* make it compatable with swipe */
    split(INPUT.s_trk2buf.sb_content, psLp->pbLoc, bLen = psLp->sLen);
    INPUT.s_trk2buf.b_len = (BYTE) fndb(INPUT.s_trk2buf.sb_content, 'F', (BYTE)(bLen * 2));
    if (INPUT.s_trk2buf.b_len < (bLen*2) -1) {
      RSP_DATA.w_rspcode='U'*256+'C';
      RSP_DATA.b_response = TRANS_FAIL;
      return FALSE;
    }
  }

  memset(INPUT.s_icc_data.sb_label, ' ',sizeof(INPUT.s_icc_data.sb_label));
  psLp = TagSeek(gGDS->s_TagList, GetApplNameTag());
  if (psLp != NULL) {
    if ((bLen = psLp->sLen) > 16)
      bLen = 16;
    memcpy(INPUT.s_icc_data.sb_label, psLp->pbLoc, bLen);
  }

  // KC++ for Maestro
  if ((psLp = TagSeek(gGDS->s_TagList, 0x9F06)) != NULL) {    // search AID
    if ((memcmp(psLp->pbLoc, "\xA0\x00\x00\00\x04\x22\x03", 7) == 0) ||
        (memcmp(psLp->pbLoc, "\xA0\x00\x00\00\x04\x30\x60", 7) == 0)) {
        gIsMaestro = TRUE;
    }
  }
  // KC--

  ClearResponse();
  memcpy(&RSP_DATA.s_dtg.b_year, TagData(gGDS->s_TagList, 0x9A), 3);
  memcpy(&RSP_DATA.s_dtg.b_hour, TagData(gGDS->s_TagList, 0x9F21), 3);
  memset(RSP_DATA.sb_auth_code, ' ', sizeof(RSP_DATA.sb_auth_code));
  RSP_DATA.w_rspcode = '5'*256+'1';
  RSP_DATA.b_response = TRANS_FAIL;
  return TRUE;
}
//*****************************************************************************
//  Function        : emvPIN
//  Description     : Get EMV Online PIN.
//  Input           : N/A
//  Return          : FALSE => fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN emvPIN(void)
{
  if (GetPIN()) {
    gGDS->s_EMVIn.bMsg[0] = 0;
    gGDS->s_EMVIn.wLen = 1;
    DispHeader(NULL);
    Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING), MW_LINE5, MW_BIGFONT);
    return TRUE;
  }
  DispHeader(NULL);
  RSP_DATA.w_rspcode = 'C'*256+'N';
  return FALSE;
}
//*****************************************************************************
//  Function        : PackEMVData
//  Description     : Put EMV related data to  ICC DATA buffer
//  Input           : aDat;     // pointer to ICC DATA struct
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
void PackEMVData(struct ICC_DATA  *aDat)
{
  BYTE *ptr;

  aDat->w_misc_len = 0;
  ptr = PackTagsData(aDat->sb_misc_content, (WORD *)wVMOnlineTags);
  aDat->w_misc_len = (BYTE)((DWORD)ptr - (DWORD)aDat->sb_misc_content);
}
//*****************************************************************************
//  Function        : EMVOnline
//  Description     : Online Auth
//  Input           : N/A
//  Return          : FALSE: fail to go online, else TRUE
//  Note            : Zero length response => online failure
//  Globals Changed : N/A;
//*****************************************************************************
static BOOLEAN EMVOnline(void)
{
  static BYTE abResp[4]; //benny

  if (DeAss(gGDS->s_TagList, gGDS->s_EMVOut.bMsg+2, (WORD)(gGDS->s_EMVOut.wLen - 2)) == 0)
    EMVFatal();
  gGDS->s_EMVOut.wLen = 1;
  DispHeader(NULL);
  PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    if (ReversalOK()) {
      OfflineSent(FALSE);
      memcpy(INPUT.sb_trace_no, TagData(gGDS->s_TagList, 0x9F41), 3); /* restore TSN */
      memcpy(TX_DATA.sb_trace_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
      memcpy(INPUT.sb_roc_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));  // restore ROC
      PackEMVData(&INPUT.s_icc_data);  // field 55 ICC data
      IncAPMTraceNo();
      MoveInput2Tx();
      memcpy(TX_DATA.sb_pin, INPUT.sb_pin, sizeof(INPUT.sb_pin)); // online pin
      //PackEMVData(&TX_DATA.s_icc_data);  // field 55 ICC data
      PackProcCode(TX_DATA.b_trans, TX_DATA.b_acc_ind);
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);

      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
      }
      if (RSP_DATA.b_response <= TRANS_REJ) {
        memcpy(abResp, "\x8A\x02", 2);  //Authorisation Response Code, 2 bytes
        abResp[2] = (BYTE)(RSP_DATA.w_rspcode>>8);
        abResp[3] = (BYTE)(RSP_DATA.w_rspcode&0xFF);
        memcpy(gGDS->s_EMVIn.bMsg+1, abResp, sizeof(abResp));
        gGDS->s_EMVIn.wLen = sizeof(abResp) + 1;
        if (RSP_DATA.b_response == TRANS_ACP) {
          memcpy(gGDS->s_EMVIn.bMsg+gGDS->s_EMVIn.wLen, "\x89\x06", 2); //Auth. Code, 6 bytes
          memcpy(gGDS->s_EMVIn.bMsg+gGDS->s_EMVIn.wLen+2, RSP_DATA.sb_auth_code, 6);
          gGDS->s_EMVIn.wLen += 8;
        }
        //send issuer script and authentication data to EMV module
        if (RSP_DATA.s_icc_data.w_misc_len) {
          memcpy(gGDS->s_EMVIn.bMsg+gGDS->s_EMVIn.wLen, RSP_DATA.s_icc_data.sb_misc_content, RSP_DATA.s_icc_data.w_misc_len);
          gGDS->s_EMVIn.wLen += RSP_DATA.s_icc_data.w_misc_len;
        }
      }
    }
    APM_ResetComm();
    return TRUE;
  }
  return FALSE; // unable to go online
}
//*****************************************************************************
//  Function        : EMVReferral
//  Description     : Voice Referral
//  Input           : N/A
//  Return          : FALSE => ERROR
//  Note            : Zero length response => decline
//  Globals Changed : N/A
//******************************************************************************
static BOOLEAN EMVReferral(void)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];

  PackRspText();
  memcpy(tmpbuf, &RSP_DATA.text[1], 16);
  tmpbuf[16]=0;
  DispLineMW(tmpbuf, MW_LINE1, MW_CENTER|MW_BIGFONT);

  APM_ResetComm();
  RSP_DATA.w_rspcode = 'C'*256+'N';
  RSP_DATA.text[0] = 0;

  if (GetAuthCode()) {
    if (INPUT.b_trans >= SALE_SWIPE) {
      INPUT.b_trans = ESALE_COMPLETE;
      INPUT.b_trans_status = OFFLINE;
    }
    DispHeader(STIS_ISS_TBL(0).sb_card_name);
    RSP_DATA.b_response = TRANS_ACP;
  }
  else
    DispHeader(STIS_ISS_TBL(0).sb_card_name);

  if (RSP_DATA.b_response != TRANS_ACP){
    LongBeep();
    return FALSE;
  }
  PackDTGAA();
  return TRUE;
}
//*****************************************************************************
//  Function        : EMVAccepted
//  Description     : Store EMV Data into Batch.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gds.emvdll, input;
//*****************************************************************************
static void EMVAccepted(BOOLEAN aOnlTxn)
{
  BYTE *tag_ptr;
  TLIST * psLp;
  BYTE  *ptr;

  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING), MW_LINE5, MW_BIGFONT);
  memcpy(INPUT.sb_trace_no, TagData(gGDS->s_TagList, 0x9F41), 3);  // restore TSN
  // mark as offline for offline approved
  if (!(bAction & ACT_HOSTAPP)){  // offline approved
    if (INPUT.b_trans==SALE_ICC)
      INPUT.b_trans = ESALE_UNDER_LMT;
  }

  RSP_DATA.b_response = TRANS_ACP;
  tag_ptr = TagData(gGDS->s_TagList, 0x89); // locate auth code
  if (bAcceptReferral == TRUE) {
    if (tag_ptr!=NULL)
      memcpy(tag_ptr, RSP_DATA.sb_auth_code, 6); // update auth code
  }
  else {
    if (tag_ptr!=NULL)
      memcpy(RSP_DATA.sb_auth_code, tag_ptr, 6); // store Auth Code
    else
      GenAuthCode(RSP_DATA.sb_auth_code);
    if (!aOnlTxn) {
      memset(RSP_DATA.sb_auth_code, ' ', 6);
      memcpy(RSP_DATA.sb_auth_code, "Y1", 2);
      IncAPMTraceNo();
      memcpy(TX_DATA.sb_trace_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
    }
  }


  if ((psLp = TagSeek(gGDS->s_TagList, 0x9f26)) != NULL) // app Cryptogram
    memcpy(INPUT.s_icc_data.sb_tag_9f26, psLp->pbLoc, psLp->sLen);
  if ((psLp = TagSeek(gGDS->s_TagList, 0x9F06)) != NULL) { //AID for printing
    memcpy(INPUT.s_icc_data.sb_aid, psLp->pbLoc, psLp->sLen);
    INPUT.s_icc_data.b_aid_len = psLp->sLen;
  }

  SetRspCode('0'*256+'0');
  PackEMVData(&INPUT.s_icc_data);  // field 55 ICC data
  if (bAction & ACT_SIGNATURE)
    RSP_DATA.w_rspcode = 'V'*256+'S';
  else {
    RSP_DATA.w_rspcode = 'T'*256+'A';
    // !2007-05-23++
    // Check for PIN Verified
    if ((ptr = TagData(gGDS->s_TagList, 0x9F10)) != NULL) {
      if (ptr[4]&0x04)
        RSP_DATA.b_PinVerified = 1;
    }
    // !2007-05-23--
  }

  PackInputP();
  MoveInput2Tx();
  if (INPUT.b_trans != AUTH_ICC){
    if (bAcceptReferral == TRUE) {
      IncTraceNo();
    }
    SaveRecord();
    if ((INPUT.b_trans==ESALE_UNDER_LMT)||((INPUT.b_trans==ESALE_COMPLETE)&&(bAcceptReferral!=TRUE))){
      if (BlockAuthCode())
        memset(RSP_DATA.sb_auth_code, ' ', sizeof(RSP_DATA.sb_auth_code));
    }
  }
  else {  // fill record buf even if not storing for ECR resp
    Input2RecordBuf();
  }

  /*
  if (bAcceptReferral==TRUE){
    AcceptBeep();
    #if (LPT_SUPPORT|TMLPT_SUPPORT)  // Conditional Compile for Printer support
    PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
    #endif                    // PRINTER_SUPPORT
    MsgBufSetup();
    PackMsgBufLen();
  }
  */
}
//*****************************************************************************
//  Function        : EMVCancel
//  Description     : Handle EMV Cancel response
//  Input           : EMV process next state.
//  Return          : EMV process next state.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BYTE EMVCancel(BYTE aNextState)
{
  DispClrBelowMW(MW_LINE3);  // clr Pls Select display
  RSP_DATA.w_rspcode = 'C'*256+'N';
  LongBeep();
  if (gGDS->s_EMVOut.wLen > 2)
    if (bAction & ACT_HOSTAPP)
      UpdateHostStatus(REV_PENDING);
  if (IOCtlMW(gIccHandle, IO_ICC_CHANGE, NULL))
    aNextState = EMV_COMPLETE;  // no fallback

  RSP_DATA.b_response = TRANS_REJ; // for print msg disable
  return aNextState;
}
//*****************************************************************************
//  Function        : EMVError
//  Description     : Handle EMV error response
//  Input           : EMV process next state.
//  Return          : EMV process next state.
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static BYTE EMVError(BYTE aNextState)
{
  DispHeader(NULL);
  switch (gGDS->s_EMVOut.bMsg[0] & 0x7F) {  //short error message response
    case ERR_SMCIO:
    case ERR_CARDL1:
    case ERR_DATA:
      RSP_DATA.w_rspcode = 'R'*256+'E';
      break;
    case ERR_CANCEL:
      DispClrBelowMW(MW_LINE3);
      RSP_DATA.w_rspcode = 'C'*256+'N';
      break;
    case ERR_SEQ:
      RSP_DATA.w_rspcode = 'S'*256+'C';
      break;
    case ERR_NOMORE:
      RSP_DATA.w_rspcode = 'N'*256+'A'; // for ADVT #08
      break;
    case ERR_NOAPPL:  // not supported if no matching app
      RSP_DATA.w_rspcode = 'M'*256+'A'; // fallback
      break;
    case ERR_BLOCKED:
      RSP_DATA.w_rspcode = 'S'*256+'B';
      break;
    case ERR_CONFIG:
    case ERR_MEMORY:
    default:
      RSP_DATA.w_rspcode = 'S'*256+'E';
      IOCtlMW(gIccHandle, IO_ICC_OFF, NULL);
      break;
  }
  LongBeep();
  if (gGDS->s_EMVOut.wLen > 2)
    if (bAction & ACT_HOSTAPP)
      UpdateHostStatus(REV_PENDING);
  if (IOCtlMW(gIccHandle, IO_ICC_CHANGE, NULL))
    aNextState = EMV_COMPLETE;  // no fallback

  RSP_DATA.b_response = TRANS_REJ; // for print msg disable
  return aNextState;
}
//*****************************************************************************
//  Function        : EMVComplete
//  Description     : Handle EMV COMPLETE response
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void EMVComplete(void)
{
  IOCtlMW(gIccHandle, IO_ICC_OFF, NULL);
  if (DeAss(gGDS->s_TagList, gGDS->s_EMVOut.bMsg+2, (WORD)(gGDS->s_EMVOut.wLen - 2)) == 0)
    EMVFatal();
  DispHeader(NULL);

  if (bAcceptReferral==TRUE){ // referral requested
    if (!EMVReferral())
      return;
    bAction |= ACT_APPROVED;  // so it will print & update batch
  }

  if (bAction & ACT_APPROVED) {
    EMVAccepted((BOOLEAN)(bAction & ACT_ONLINE)); // 07-06-05++ BW (1) for offline approved txn
    //RSP_DATA.w_rspcode = 'T'*256+'A';
    if (bAction & ACT_HOSTAPP)
      UpdateHostStatus(NO_PENDING); // clear reversal gen after host appr txn
    AcceptBeep();
  }
  else {
    if ((bAction&ACT_HOSTAPP) || (RSP_DATA.w_rspcode=='0'*256+'0'))
      RSP_DATA.w_rspcode = '5'*256+'1'; //rejected by ICC -> set rsp. ocde to declined
    LongBeep();
  }
}
/******************************************************************************
 *  Function        : debugEMV
 *  Description     : EMV Processing state machine.
 *  Input           : N/A
 *  Return          : next action
 *  Note            : N/A
 *  Globals Changed : gds.emvdll, input;
 ******************************************************************************
 */
void debugEMV2(BYTE aState, BYTE aIn, sIO_t *pEMV)
{
  BYTE buf[30];
  WORD i;
  
  LptOpen(NULL);
  LptPutS("\x1B""F1\n");
  sprintf(buf, "dllMsg(%d)-%s\n", aState, aIn? "In": "Out");
  LptPutS(buf);
  for (i=1; i<=pEMV->wLen; i++) {
    sprintf(buf, "%02X ", (BYTE)pEMV->bMsg[i-1]);
    LptPutN(buf, 3);
  }
  LptPutS("\n");
}
/******************************************************************************
 *  Function        : GetTxnCode
 *  Description     : Get the code of the Transaction Type.
 *  Input           : N/A
 *  Return          : Txn Code
 *  Note            : N/A
 *  Globals Changed : gds.emvdll, input;
 ******************************************************************************
 */
BYTE GetTxnCode(void)
{
  BYTE code;
  
  if (INPUT.b_trans == REFUND_ICC)
    code = 0x20;
  else if (INPUT.b_trans == VOID)
    code = 0x02;
  else
    code = 0x00;      // SALE / AUTH
    
  return code;
}
//*****************************************************************************
//  Function        : EMVProcess
//  Description     : EMV Processing state machine.
//  Input           : N/A
//  Return          : next action
//  Note            : N/A
//  Globals Changed :
//*****************************************************************************
BYTE EMVProcess(void)
{
  BYTE next_state;
  BYTE tmp[10];
  BOOLEAN again = FALSE;
  DDWORD cashback_amt=0;
  
  gIsMaestro = FALSE;                   // KC++ for Maestro

  // init vars
  memset(INPUT.sb_pin, 0, sizeof(INPUT.sb_pin));
  bAcceptReferral = FALSE;
  next_state = EMV_START;
  if (INPUT.b_trans == SALE_ICC)
    //gGDS->s_EMVIn.bMsg[0] = EGOOD_SALE;                                      // KC-- MTIP05 T08 S01
    gGDS->s_EMVIn.bMsg[0] = EGS_WCASHBACK;      // goods sale with cashback    // KC++ MTIP05 T08 S01
  else if (INPUT.b_trans == REFUND_ICC)
    gGDS->s_EMVIn.bMsg[0] = EREFUND;
  else
    gGDS->s_EMVIn.bMsg[0] = EAUTH;
  gGDS->s_EMVIn.wLen    = 1;

  do {
    //if (next_state != EMV_START) {
    //debugEMV2(next_state, 1, &gGDS->s_EMVIn);
    if (EMV_DllMsg(next_state, &gGDS->s_EMVIn, &gGDS->s_EMVOut) == FALSE) {
      next_state=EMVError(next_state);
      return next_state;
    }
    next_state = gGDS->s_EMVOut.bMsg[0];
    //}
//    else {
//      next_state = gGDS->s_EMVOut.bMsg[0];
//    }
    //debugEMV2(next_state, 0, &gGDS->s_EMVOut);
    switch (gGDS->s_EMVOut.bMsg[0]&0x7F) {
      case EMV_COMPLETE: /* completed */
        EMVComplete();
        return EMV_COMPLETE;
      case EMV_SELECT_APP: // select application
        if (EMVInput(again) == FALSE)
          return EMVCancel(next_state);
        again = TRUE;
        break;
      case EMV_AMOUNT_AIP: // IAP need amount
      case EMV_AMOUNT: // must have amount by now
        if (INPUT.dd_amount == 0) {
          if ((GetAmount(TIPsReqd(), 0) == FALSE) && (INPUT.dd_amount==0))
            return EMVCancel(next_state);
        }
        DispHeader(NULL);
        // KC++ MTIP05 T08 S01
        //gGDS->s_EMVIn.bMsg[0] = 0;
        if (gIsMaestro && (INPUT.b_trans == SALE_ICC)) {
          if (GetCashBackAmount(&cashback_amt)) {
            // confirm cashback
            memcpy(gGDS->s_EMVIn.bMsg+1, "\x9F\x02\x06", 3);
            dbin2bcd(tmp, INPUT.dd_amount);
            memcpy(gGDS->s_EMVIn.bMsg+4, &tmp[4], 6);
            gGDS->s_EMVIn.wLen = 10;
            if (cashback_amt != 0) {                // cash back amount is non-zero
              memcpy(&gGDS->s_EMVIn.bMsg[10], "\x9F\x03\x06", 3);
              dbin2bcd(tmp, cashback_amt);
              memcpy(&gGDS->s_EMVIn.bMsg[13], &tmp[4], 6);
              gGDS->s_EMVIn.wLen += 9;
            }
            else {                                                  // cash back amount is zero
              memcpy(&gGDS->s_EMVIn.bMsg[10], "\x9C\x01\x00", 3);   // change back to purchase only
              gGDS->s_EMVIn.wLen += 3;
            }
          }
          else {
            // reset trans type to SALE
            memcpy(gGDS->s_EMVIn.bMsg+1, "\x9F\x02\x06", 3);
            dbin2bcd(tmp, INPUT.dd_amount);
            memcpy(gGDS->s_EMVIn.bMsg+4, &tmp[4], 6);
            gGDS->s_EMVIn.wLen = 10;
            memcpy(&gGDS->s_EMVIn.bMsg[10], "\x9C\x01\x00", 3);     // change back to purchase only
            gGDS->s_EMVIn.wLen += 3;
          }
        }
        else {      // not Maestro
          memcpy(gGDS->s_EMVIn.bMsg+1, "\x9F\x02\x06", 3);
          dbin2bcd(tmp, INPUT.dd_amount);
          memcpy(gGDS->s_EMVIn.bMsg+4, &tmp[4], 6);
          gGDS->s_EMVIn.wLen = 10;
          memcpy(&gGDS->s_EMVIn.bMsg[10], "\x9C\x01", 2);           // change back txn type
          gGDS->s_EMVIn.bMsg[12] = GetTxnCode();
          gGDS->s_EMVIn.wLen += 3;
        }
        // KC--
        if (!GetProductCode())
          return EMVCancel(next_state);
        DispHeader(NULL);
        Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING), MW_LINE5, MW_BIGFONT);
        break;
      case EMV_CONFIRM_CARD: /* validate application */
        if (!ValidEMVData()) {
          Disp2x16Msg(GetConstMsg(EDC_RSP_READ_ERROR), MW_LINE5, MW_BIGFONT);
          return FALSE;
        }

        PackComm(INPUT.w_host_idx, FALSE);
        APM_PreConnect();

      //  if (!ConfirmCard()) {  // leave exp date chk to kernel
      //    RSP_DATA.w_rspcode = 'C'*256+'N';
      //    return EMV_COMPLETE; // 25-04-05++ txn end
      //  }

        if (INPUT.b_trans == AUTH_ICC) {// AUTH always online
          gGDS->s_EMVIn.bMsg[0] = 0x88;     // Override transaction type & force online
          gGDS->s_EMVIn.wLen = 1;
        }
        else {
          gGDS->s_EMVIn.bMsg[0] = 0;
          gGDS->s_EMVIn.wLen = 0;
        }
        break;
      case EMV_ONLINE_PIN: // online PIN
        if (emvPIN() == FALSE) {
          LongBeep();
          return EMV_ONLINE_PIN; // 25-04-05++ return next action code
        }
        break;
      case EMV_ONLINE: // online process
        // only proceed if unable to go online or txn approved
			PackHeader();

        if ( (EMVOnline() == FALSE)) {
          gGDS->s_EMVIn.bMsg[0] = 0;
          gGDS->s_EMVIn.wLen = 0;
          return EMV_ONLINE;
        }
        break;
      case EMV_REFERRAL: // referral
        gGDS->s_EMVIn.bMsg[0] = 0;
        gGDS->s_EMVIn.wLen = 1;   // request AAC
        bAcceptReferral = TRUE; //referral requested indicator
        break;
      default:
        EMVFatal();
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : EMVTrans
//  Description     : EMV trans handler.
//  Input           : trans;    // trans type
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
DWORD EMVTrans(DWORD aTrans)
{
  BYTE next_action;
  
  INPUT.b_trans = (BYTE) aTrans;
  if (STIS_TERM_DATA.b_stis_mode < TRANS_MODE) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  if (BlockLocalTrans()) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  if (INPUT.b_trans==AUTH_ICC) {
    if (AuthBlocked()) {
      DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
      return FALSE;
    }
  }

  if (HostSettlePending() || APM_BatchFull(INPUT.w_host_idx)) {
    DispErrorMsg(GetConstMsg(EDC_TU_CLOSE_BATCH));
    return FALSE;
  }
  if (!Expire_diff())
    return FALSE;

  INPUT.b_entry_mode = ICC;
  RSP_DATA.b_response = TRANS_FAIL;
  RSP_DATA.w_rspcode = '5'*256+'1';
  GetEcrRef(FALSE);

  DispHeader(NULL); // show correct header

  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING),MW_LINE5, MW_BIGFONT);
  Short1Beep();
  // no fallback if get processing option step performed
  //21-08-12 JC ++
  next_action = EMVProcess();
  if ((next_action != EMV_COMPLETE) &&
      (next_action != EMV_ONLINE  ) &&
      (next_action != EMV_REFERRAL)) {
    if ((RSP_DATA.w_rspcode == 'S'*256+'C') ||
        (RSP_DATA.w_rspcode == 'M'*256+'A') ||  // no matching application
        (RSP_DATA.w_rspcode == 'N'*256+'E') ||
        (RSP_DATA.w_rspcode == 'R'*256+'E') ||
        (RSP_DATA.w_rspcode == 'N'*256+'A')) {
      os_sam_off();
      if (GetCard(0, TRUE)) {
        switch (INPUT.b_trans) {
          case AUTH_ICC:
          case REFUND_ICC:
            RSP_DATA.w_rspcode = '0'*256+'0';
            return TRUE;
          case SALE_ICC:
            SaleTrans(INPUT.b_entry_mode);
            break;
          default:
            break;
        }
      }
      return TRUE;
    }
  }
  //21-08-12 JC --

  if (RSP_DATA.w_rspcode != '0'*256+'0')
    TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  return TRUE;
}
//*****************************************************************************
//  Function        : EMVInit
//  Description     : Kernel initialization.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EMVInit(void)
{
  BYTE init_mode=0;

  gGDS->s_EMVIn.bMsg[0] = 0xFF;     // customized message
  memcpy(&gGDS->s_EMVIn.bMsg[1], KPinMsg, sizeof(KPinMsg));
  gGDS->s_EMVIn.wLen = 1 + sizeof(KPinMsg);
  init_mode |= BIT_PSE_SUPPORT;
  EMV_DllMsg(init_mode, &gGDS->s_EMVIn, &gGDS->s_EMVOut);
}

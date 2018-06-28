//-----------------------------------------------------------------------------
//  File          : sale.c
//  Module        :
//  Description   : Include routines for sale transaction.
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
//  10 May  2006  Lewis       Initial Version.
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
#include "record.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "reversal.h"
#include "offline.h"
#include "emvtrans.h"
#include "sale.h"

//*****************************************************************************
//  Function        : OffHookState
//  Description     : Reture TRUE if Phone is in Off Hook State.
//                    (Off Hook or Speaker Phone On)
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN OffHookState(void)
{
  DWORD status;
  int handle;

  handle = APM_GetHwHandle(APM_SDEV_MDM);

  if (handle < 0) {
    return FALSE;
  }

  if (IOCtlMW(handle, IO_MDM_HSET_STS, 0)==1) {
    return TRUE;
  }

  status = IOCtlMW(handle, IO_MDM_GET_STATUS, 0);
  if ((status == MWMDM_VoiceHs) || (status == MWMDM_VoiceFrSpPh) ||
      (status == MWMDM_VoiceSpPh) || (status == MWMDM_VoiceSpPhFrHs) ) {
    return TRUE;
  }
  return FALSE;
}
#if (R700)
//*****************************************************************************
//  Function        : SetSpeakerGain
//  Description     : 
//  Input           : N/A
//  Return          : N/A
//  Note            : Gain may change after sending AT Cmd.
//  Globals Changed : N/A
//*****************************************************************************
static void SetSpeakerGain(int aHandle, DWORD aGain)
{
  DWORD status = IOCtlMW(aHandle, IO_MDM_GET_STATUS, 0);

  if ((status == K_VoiceHs) || (status == K_VoiceFrSpPh)) {
    IOCtlMW(aHandle, IO_MDM_SPR_GAIN, aGain);
  }
  else if ((status == K_VoiceSpPh) || (status == K_VoiceSpPhFrHs)) {
    IOCtlMW(aHandle, IO_MDM_SPR_GAIN, aGain); 
  }
}
#endif //(R700)
//*****************************************************************************
//  Function        : MakeVoiceCall
//  Description     : Make Voice Call
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void MakeVoiceCall(BYTE *aPhoneNum)
{
  int handle;
  // int i;  // For R700-P

  handle = APM_GetHwHandle(APM_SDEV_MDM);
  if (handle >= 0) {
    #if (R700)
    SetSpeakerGain(handle, 9*15);
    #endif

    IOCtlMW(handle, IO_MDM_PHONE_DIAL, aPhoneNum);  // For R700

    #if (R700)
    // For R700-P
    //for (i = 0; i < strlen(aPhoneNum); i++) {
    //  IOCtlMW(handle, IO_MDM_DIALCHAR, aPhoneNum[i]);
    //}
    SetSpeakerGain(handle, 9*15);
    #endif
  }
}
void ReferralCall(void)
{
  int handle;

  handle = APM_GetHwHandle(APM_SDEV_MDM);
  if (handle >= 0) {
    IOCtlMW(handle, IO_MDM_SET_OPR_MODE, 21);  // set voice mode
    DispClrBelowMW(MW_LINE3);
    DispLineMW("PLS LIFT HANDSET", MW_LINE5, MW_CENTER|MW_BIGFONT);
    while (1) {
      SleepMW();
      if (OffHookState()) {  // Handset in use
        DispClrBelowMW(MW_LINE3);
        DispLineMW("Dialing...      ", MW_LINE5, MW_CENTER|MW_BIGFONT);
        break;
      }
      if (APM_GetKeyin() == MWKEY_CANCL)
        return;
    }
    DispClrBelowMW(MW_LINE3);
    MakeVoiceCall(STIS_ISS_TBL(0).sb_ref_tel_no);
    while (os_kbd_getkey()==0) os_sleep();
    return;
  }
  DispLineMW("Phone Error!", MW_LINE5, MW_CENTER|MW_BIGFONT);
  ErrorDelay();
}
//*****************************************************************************
//  Function        : ReferralProcess
//  Description     : Process referral response.
//  Input           : N/A
//  return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ReferralProcess(void)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];

  PackRspText();
  memcpy(tmpbuf, &RSP_DATA.text[1], 16);
  tmpbuf[16]=0;
  DispLineMW(tmpbuf, MW_LINE1, MW_CENTER|MW_BIGFONT);
  LongBeep();

  APM_ResetComm();
  RSP_DATA.w_rspcode = 'C'*256+'N';
  RSP_DATA.text[0] = 0;

  if (ReferralDial()) {
    ReferralCall();
  }

  if (GetAuthCode()) {
    if (INPUT.b_trans >= SALE_SWIPE) {
      INPUT.b_trans = SALE_COMPLETE;
      INPUT.b_trans_status = OFFLINE;
    }

    DispHeader(STIS_ISS_TBL(0).sb_card_name);
    RSP_DATA.b_response = TRANS_ACP;
    PackDTGAA();
    PackInputP();
    IncTraceNo();
    if (INPUT.b_trans >= SALE_SWIPE)
      SaveRecord();
  }
  else
    DispHeader(STIS_ISS_TBL(0).sb_card_name);
  TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
}
//*****************************************************************************
//  Function        : SaleTrans
//  Description     : Do Online Sale transaction.
//  Input           : aInputMode;       // Transaction start mode
//  return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD SaleTrans(DWORD aInputMode)
{
  INPUT.b_trans = SALE_SWIPE;

  DispHeader(NULL);

  if (!TransAllowed(aInputMode)) {
    RefreshDispAfter(0);
    return FALSE;
  }

  if (aInputMode == MANUAL)
    INPUT.b_trans = SALE_MANUAL;

  if (UnderFloorLimit(INPUT.dd_amount) && (!INPUT.b_card_expired) &&
      (aInputMode != FALLBACK)) {
    INPUT.b_trans = SALE_UNDER_LMT;
    SetRspCode('0'*256 + '0');
    RSP_DATA.b_response = TRANS_ACP;
    GenAuthCode(RSP_DATA.sb_auth_code);
    if (BlockAuthCode())
      memset(RSP_DATA.sb_auth_code, ' ', 6);
    PackDTGAA();
    IncTraceNoEND();
    SaveOfflRecord();   //17-12-12 JC ++
    PackInputP();
    TransEnd(TRUE);
    return TRUE;
  }

  ClearResponse();
  //PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    if (ReversalOK()) {
      OfflineSent(0);
      IncTraceNoEND();
      MoveInput2Tx();
      PackProcCode(TX_DATA.b_trans,0xFF);
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);

      ClearResponse();
      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        if (RSP_DATA.b_response <= TRANS_REJ)
          UpdateHostStatus(NO_PENDING);
      }
      //SaveTraceNo();
      if (RSP_DATA.b_response == TRANS_ACP) {
        if (RSP_DATA.w_rspcode == '0'*256+'0')
          RSP_DATA.w_rspcode = 'A'*256+'P';
        SaveRecord();
        PackInputP();
      }
      else
        if (RSP_DATA.b_response == TRANS_REJ) {
          if (VoiceReferral(RSP_DATA.w_rspcode)) {
            ReferralProcess();
            return FALSE;
          }
        }
    }
  }
  TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  return TRUE;
}

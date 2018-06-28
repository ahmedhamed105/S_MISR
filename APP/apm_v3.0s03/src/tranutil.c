//-----------------------------------------------------------------------------
//  File          : tranutil.c
//  Module        :
//  Description   : Utilities function for transactions.
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
#include "hwdef.h"
#include "midware.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "message.h"
#include "constant.h"
#include "termdata.h"
#include "tranutil.h"

//-----------------------------------------------------------------------------
//      Globals Variables
//-----------------------------------------------------------------------------
struct TRANS_DATA  gsTransData;

//*****************************************************************************
//  Function        : ResetTrans
//  Description     : Reset terminal data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : hst_rsp_data;
//*****************************************************************************
void ResetTrans(void)
{
  memset(&gsTransData, 0, sizeof(struct TRANS_DATA));
  gsTransData.s_rsp_data.w_rspcode = 'C'*256+'N';
  gsTransData.s_rsp_data.sb_text[0] = 0;
  SetKbdTimeout(KBD_TIMEOUT);
}
//*****************************************************************************
//  Function        : ClearResponse
//  Description     : Clear host gsTransData.b_response data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : rsp_data;
//*****************************************************************************
void ClearResponse(void)
{
  gsTransData.s_rsp_data.sb_text[0] = 0;
  memset(&gsTransData.s_rsp_data.sb_text[1], ' ', 69);
  memset(gsTransData.s_rsp_data.sb_rrn, ' ', 12);
  gsTransData.b_response = TRANS_FAIL;
}
//*****************************************************************************
//  Function        : PackRspText
//  Description     : Update response text base on rspcode.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PackRspText(void)
{
  BYTE var_i;

  if (gsTransData.s_rsp_data.sb_text[0] != 0)
    return;

  var_i = 0;
  while ((KRspText[var_i].w_idx != gsTransData.s_rsp_data.w_rspcode) &&
         (KRspText[var_i].w_idx != '*'*256+'*'))
    var_i += 1;

  memset(&gsTransData.s_rsp_data.sb_text[1], ' ', 40);

  memcpy(&gsTransData.s_rsp_data.sb_text[1], &KRspText[var_i].pc_text[0], 16);
  memcpy(&gsTransData.s_rsp_data.sb_text[21],&KRspText[var_i].pc_text[16], 16);
  if (gsTransData.s_rsp_data.w_rspcode == 'A'*256+'P')
    memcpy(&gsTransData.s_rsp_data.sb_text[31], gsTransData.s_rsp_data.sb_auth_code, 6);
  gsTransData.s_rsp_data.sb_text[0] = 40;
}
//*****************************************************************************
//  Function        : SetRspCode
//  Description     : Update the rspdata.rspcode with INPUT.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : rsp_data;
//*****************************************************************************
void SetRspCode(WORD aRspCode, BOOLEAN aPackText)
{
  gsTransData.b_response = TRANS_ACP;
  if ((aRspCode != '0'*256+'0') && (aRspCode != 'A'*256+'P'))
    gsTransData.b_response = TRANS_FAIL;
  gsTransData.s_rsp_data.w_rspcode = aRspCode;
  if (aPackText) {
    gsTransData.s_rsp_data.sb_text[0] = 0;
    PackRspText();
  }
}
//*****************************************************************************
//  Function        : DispHeader
//  Description     : Display transaction header base on the input param.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispHeader(void)
{
  if (gsTransData.s_input.b_trans==EDC_INIT) {
    switch (gsTransData.s_input.b_stis_init_mode) {
      case MODE_EMV_KEY_ONLY:
        DispLineMW(KLoadEMVKey, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
        return;
      case MODE_EMV_PARAM_KEY:
        DispLineMW(KLoadEMVParam, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
        return;
      case MODE_EXTRA_PARAM:
        DispLineMW(KLoadExtraParam, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
        return;
      default :
        break;
    }
  }

  DispLineMW(KTransHeader[gsTransData.s_input.b_trans], MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
}
//*****************************************************************************
//  Function        : DispRspText
//  Description     : Display Response text.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispRspText(BOOLEAN aDispHdr)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  PackRspText();
  if (aDispHdr)
    DispHeader();
  DispClrBelowMW(MW_LINE3);
  memcpy(tmpbuf, &gsTransData.s_rsp_data.sb_text[1], 16);
  tmpbuf[16] = 0;
  DispLineMW(tmpbuf, MW_LINE5, MW_CENTER|MW_BIGFONT);
  memcpy(tmpbuf, &gsTransData.s_rsp_data.sb_text[21], 16);
  tmpbuf[16] = 0;
  DispLineMW(tmpbuf, MW_LINE7, MW_CENTER|MW_BIGFONT);
}
//*****************************************************************************
//  Function        : TransEnd
//  Description     : Transaction end process.
//  Input           : N/A
//  Return          : N/A
//  Note            : !TODO
//  Globals Changed : N/A
//*****************************************************************************
void TransEnd(void)
{
  if (gsTransData.b_response == TRANS_ACP)
    AcceptBeep();
  else
    LongBeep();

  //ResetLine();
  DispRspText(TRUE);

  // Commit All File Changes
  fCommitAllMW();
  Delay1Sec(2, FALSE);
  ClearKeyin();  // flush key to avoid display refresh caused by key event
  ResetTrans();
}
//*****************************************************************************
//  Function        : Return2Base
//  Description     : Prompt Return Handset to base if necessary.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN Return2Base(void)
{
  #if (DOCK_SUPPORT)
  struct TERM_DATA term_data;
  int handle;

  handle = TimerOpenMW();
  if (handle >= 0) {
    if (!os_onbase()) {
      GetTermData(&term_data);
      //DispLineMW("RETURN TO BASE", MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SPFONT);
      DispClrBelowMW(MW_LINE3);
      Disp2x16Msg(KReturn2Base, MW_LINE5, MW_BIGFONT);
      TimerSetMW(handle, term_data.b_return_timeout*100*60);
      while (!os_onbase()) {
        Short1Beep();
        if (WaitKey(0) == MWKEY_CANCL)
          break;
        if (TimerGetMW(handle) == 0)
          break;
        Delay10ms(100);
      }
      DispClrBelowMW(MW_LINE3);
    }
    if (!os_onbase())
      return FALSE;
  }
  #endif  // DOCK_SUPPORT
  return TRUE;
}

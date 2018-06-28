//-----------------------------------------------------------------------------
//  File          : ecr.c
//  Module        :
//  Description   : Include routines for ecr handling.
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
//  24 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "apm.h"
#include "util.h"
#include "ecrdll.h"
#include "tlvutil.h"
#include "corevar.h"
#include "constant.h"
#include "tranutil.h"
#include "input.h"
#include "sale.h"
#include "saleoffl.h"
#include "emvtrans.h"
#include "settle.h"
#include "ctltrans.h"
#include "ecr.h"
#include "hardware.h"
//#include "welcome.h"
//#include "transacp.h"
//#include "transrej.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
struct ECRRESPTBL {
  WORD *pw_tag_list;
};
//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------
const WORD KSaleResp[] = {
  ECRTAG_PAN,       ECRTAG_TXNAMT,   ECRTAG_TRACE,     ECRTAG_TXNTIME,
  ECRTAG_TXNDATE,   ECRTAG_EXPDATE,  ECRTAG_RRN,       ECRTAG_AUTH_RESP,
  ECRTAG_RESP_CODE, ECRTAG_TERMID,   ECRTAG_MERCHID,   ECRTAG_CARDHOLDERNAME,
  ECRTAG_ADDAMT,    ECRTAG_BATCHNO,  ECRTAG_ECRREF,    0x00
};

const WORD KOfflSaleResp[] = {
  ECRTAG_PAN,       ECRTAG_TXNAMT,   ECRTAG_TRACE,     ECRTAG_TXNTIME,
  ECRTAG_TXNDATE,   ECRTAG_EXPDATE,                    ECRTAG_AUTH_RESP,
  ECRTAG_RESP_CODE, ECRTAG_TERMID,   ECRTAG_MERCHID,   ECRTAG_CARDHOLDERNAME,
  ECRTAG_ADDAMT,    ECRTAG_BATCHNO,  ECRTAG_ECRREF,    0x00
};

const WORD KLastTrace[] = {
  ECRTAG_TRACE, ECRTAG_ECRREF, ECRTAG_RESP_CODE, 0x00
};

const WORD KHostNameResp[] = {
  ECRTAG_RESP_CODE, ECRTAG_HOSTNAME, 0x00
};

const WORD KHostTotResp[] = {
  ECRTAG_RESP_CODE, ECRTAG_HOSTTOT, 0x00
};
const WORD KSettleResp[] = {
  ECRTAG_RESP_CODE, 0x00
};
const WORD *KEcrRespTbl[MAX_ECREDC_CMD] = {
  KSaleResp,      // ECR_EDC_SALE
  KOfflSaleResp,  // ECR_EDC_OFFLSALE
  KSaleResp,      // ECR_EDC_REFUND
  KSaleResp,      // ECR_EDC_VOID
  KSaleResp,      // ECR_EDC_GETTXN
  KLastTrace,     // ECR_EDC_LASTTRACE
  KHostNameResp,  // ECR_EDC_HOSTNAME
  KHostTotResp,   // ECR_EDC_HOSTTOTAL
  KSaleResp,      // ECR_EDC_AUTH
  KSettleResp,    // ECR_EDC_SETTLE
};
//*****************************************************************************
//  Function        : EcrGetAmount
//  Description     : Retrieve Transaction Amount from ECR data.
//  Input           : N/A
//  Return          : amount
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DDWORD EcrGetAmount(void)
{
  BYTE *pbTlv;
  DWORD ecrdata_len;

  if (gGDS->i_ecr_len <= 2)
    return 0;

  ecrdata_len = gGDS->i_ecr_len - 2;
  if (ecrdata_len) {
    pbTlv = TlvSeek(gGDS->s_ecr_data.sb_content, ecrdata_len, ECRTAG_TXNAMT);
    if (pbTlv != NULL) {
      TlvVPtr(pbTlv);
      return bcd2DDWORD(TlvVPtr(pbTlv), TlvLen(pbTlv));
    }
  }
  return 0;
}
//*****************************************************************************
//  Function        : EcrGetRef
//  Description     : Retrieve Ecr Reference number from ECR data.
//  Input           : N/A
//  Return          : amount
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD EcrGetRef(BYTE *aEcrRef)
{
  BYTE *pb, *pbTlv;
  DWORD ecrdata_len;

  memset(aEcrRef, ' ', 16);
  if (gGDS->i_ecr_len <= 2)
    return 0;
  ecrdata_len = gGDS->i_ecr_len - 2;
  if (ecrdata_len) {
    pbTlv = TlvSeek(gGDS->s_ecr_data.sb_content, ecrdata_len, ECRTAG_ECRREF);
    if (pbTlv != NULL) {
      pb = TlvVPtr(pbTlv);
      memcpy(aEcrRef, pb, 16);
      return 16;
    }
  }
  return 0;
}
//*****************************************************************************
//  Function        : EcrResp
//  Description     : Pack and send ECR resp.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EcrResp(void)
{
  const WORD *pw;
  BYTE *pb, ab_tmp[32], ab_totresp[32], *pb_tmp;
  WORD w_rspcode, w_tmp;
#if 0
  BYTE *wav_dat;
  const BYTE *wav_ptr;
  DWORD wav_size;
#endif

  if ((gGDS->i_ecr_len == 0) || (gGDS->s_ecr_data.b_cmd >= MAX_ECREDC_CMD))
    return;

  w_rspcode = 'L'*256+'C';
  pb = gGDS->s_ecr_data.sb_content;
  pw = KEcrRespTbl[gGDS->s_ecr_data.b_cmd];
  while (*pw != 0x00) {
    switch (*pw) {
      case ECRTAG_PAN:
        split(ab_tmp, INPUT.sb_pan, 10);
        w_tmp = fndb(&ab_tmp[1], 'F', 19);
        w_tmp = (w_tmp/2) + (w_tmp%2);
        pb += TlvPack(*pw, w_tmp, INPUT.sb_pan, pb);
        break;
      case ECRTAG_TXNAMT:
        DDWORD2bcd(ab_tmp, INPUT.dd_amount);
        pb += TlvPack(*pw, 6, &ab_tmp[4], pb);
        break;
      case ECRTAG_TRACE:
        pb += TlvPack(*pw, 3, INPUT.sb_trace_no, pb);
        break;
      case ECRTAG_TXNTIME:
        pb += TlvPack(*pw, 3, &RSP_DATA.s_dtg.b_hour, pb);
        break;
      case ECRTAG_TXNDATE:
        pb += TlvPack(*pw, 2, &RSP_DATA.s_dtg.b_month, pb);
        break;
      case ECRTAG_EXPDATE:
        pb += TlvPack(*pw, 2, INPUT.sb_exp_date, pb);
        break;
      case ECRTAG_RRN:
        pb += TlvPack(*pw, 12, RSP_DATA.sb_rrn, pb);
        break;
      case ECRTAG_AUTH_RESP:
        pb += TlvPack(*pw, 6, RSP_DATA.sb_auth_code, pb);
        break;
      case ECRTAG_RESP_CODE:
        if ((RSP_DATA.w_rspcode == 'A'*256+'P') || (RSP_DATA.w_rspcode == 'T'*256+'A'))
          w_rspcode = '0'*256+'0';
        else
          w_rspcode = RSP_DATA.w_rspcode;
        ab_tmp[0] = (BYTE)((w_rspcode>>8)&0xFF);
        ab_tmp[1] = (BYTE)(w_rspcode&0xFF);
        pb += TlvPack(*pw, 2, ab_tmp, pb);
        break;
      case ECRTAG_TERMID :
        pb += TlvPack(*pw, 8, STIS_ACQ_TBL(0).sb_term_id, pb);
        break;
      case ECRTAG_MERCHID:
        pb += TlvPack(*pw, 15, STIS_ACQ_TBL(0).sb_acceptor_id, pb);
        break;
      case ECRTAG_CARDHOLDERNAME :
        pb += TlvPack(*pw, 26, INPUT.sb_holder_name, pb);
        break;
      case ECRTAG_ADDAMT :
        DDWORD2bcd(ab_tmp, INPUT.dd_tip);
        pb += TlvPack(*pw, 6, &ab_tmp[4], pb);
        break;
      case ECRTAG_BATCHNO:
        pb += TlvPack(*pw, 3, STIS_ACQ_TBL(0).sb_curr_batch_no, pb);
        break;
      case ECRTAG_ECRREF:
        pb += TlvPack(*pw, 16, INPUT.sb_ecr_ref, pb);
        break;
      case ECRTAG_HOSTNAME:
        pb += TlvPack(*pw, 10, STIS_ACQ_TBL(0).sb_name, pb);
        break;
      case ECRTAG_HOSTTOT:
        pb_tmp = ab_totresp;
        DDWORD2bcd(ab_tmp, TERM_TOT.s_sale_tot.w_count);
        memcpy(pb_tmp, &ab_tmp[8], 2);
        pb_tmp += 2;
        DDWORD2bcd(ab_tmp, TERM_TOT.s_sale_tot.dd_amount);
        memcpy(pb_tmp, &ab_tmp[4], 6);
        pb_tmp += 6;
        DDWORD2bcd(ab_tmp, TERM_TOT.s_refund_tot.w_count);
        memcpy(pb_tmp, &ab_tmp[8], 2);
        pb_tmp += 2;
        DDWORD2bcd(ab_tmp, TERM_TOT.s_refund_tot.dd_amount);
        memcpy(pb_tmp, &ab_tmp[4], 6);
        pb_tmp += 6;
        pb += TlvPack(*pw, 16, ab_totresp, pb);
        break;
    }
    pw++;
  }
  gGDS->i_ecr_len = (DWORD)pb - (DWORD)&gGDS->s_ecr_data;
  if (gGDS->i_ecr_len > 0) {
#if 0
    if (gCdc1Handle != -1) {
      if (w_rspcode == '0'*256+'0') {
        wav_size = sizeof(KTransAcpWav);
        wav_ptr  = KTransAcpWav;
      }
      else {
        wav_size = sizeof(KTransRejWav);
        wav_ptr  = KTransRejWav;
      }
      wav_dat = (BYTE *)MallocMW(wav_size);
      if (wav_dat != NULL) {
        memcpy(wav_dat, wav_ptr, wav_size);
        while (StatMW(gCdc1Handle, MW_CODEC_STATUS, NULL) & MW_CODEC_PLAYING) SleepMW();
        //printf("\x0E\x12\x1B\xE0T2:%X\x05\x0F", StatMW(gCdc1Handle, MW_CODEC_STATUS, NULL));
        WriteMW(gCdc1Handle, wav_dat, wav_size);
        FreeMW(wav_dat);
      }
    }
#endif
    ECR_Send(&gGDS->s_ecr_data, gGDS->i_ecr_len);
    gGDS->i_ecr_len = 0;
  }
}
//*****************************************************************************
//  Function        : GetLastTrace
//  Description     : Get Last Trace Number.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GetLastTrace(void)
{
  int rec_cnt, rec_idx;

  memset(RSP_DATA.sb_trace_no, 0x00, sizeof(RSP_DATA.sb_trace_no));
  rec_cnt = APM_GetRecCount();
  if (rec_cnt > 0) {
    rec_idx = rec_cnt - 1;
    APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
    memcpy(INPUT.sb_trace_no, RECORD_BUF.sb_trace_no, sizeof(INPUT.sb_trace_no));
    memcpy(INPUT.sb_ecr_ref, RECORD_BUF.sb_ecr_ref, sizeof(INPUT.sb_trace_no));
    RSP_DATA.w_rspcode = '0'*256+'0';
  }
  else
    RSP_DATA.w_rspcode = 'N'*256+'T';
  EcrResp();
}
//*****************************************************************************
//  Function        : GetHostIdx
//  Description     : Get Host Index.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetHostIdx(void)
{
  BYTE *pb, *pbTlv;
  DWORD d_ecrdata_len;

  if (gGDS->i_ecr_len <= 2)
    return -1;

  d_ecrdata_len = gGDS->i_ecr_len - 2;
  if (d_ecrdata_len) {
    pbTlv = TlvSeek(gGDS->s_ecr_data.sb_content, d_ecrdata_len, ECRTAG_HOSTIDX);
    if (pbTlv != NULL) {
      pb = TlvVPtr(pbTlv);
      return *pb;
    }
  }
  return (DWORD) -1;
}
//*****************************************************************************
//  Function        : GetHostName
//  Description     : Get Host Name
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GetHostName(void)
{
  int host_idx;

  RSP_DATA.w_rspcode = 'D'*256 + 'E';
  host_idx = GetHostIdx();
  if (host_idx != -1) {
    if (APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0)))
      RSP_DATA.w_rspcode = '0'*256 + '0';
  }
  EcrResp();
}
//*****************************************************************************
//  Function        : GetHostTot
//  Description     : Get Host Total.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GetHostTot(void)
{
  int host_idx;

  RSP_DATA.w_rspcode = 'D'*256 + 'E';
  host_idx = GetHostIdx();
  if (host_idx != -1) {
    if (APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0))) {
      RSP_DATA.w_rspcode = '0'*256 + '0';
      CalTotals(FALSE);
    }
  }
  EcrResp();
}
//*****************************************************************************
//  Function        : EcrSettle
//  Description     : Process Settlement request from ECR.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EcrSettle(void)
{
  int host_idx;

  RSP_DATA.w_rspcode = 'D'*256 + 'E';
  host_idx = GetHostIdx();
  if (host_idx != -1) {
    INPUT.w_host_idx = host_idx;
    SettleTrans();
  }
  EcrResp();
}
//*****************************************************************************
//  Function        : EcrCmdProcess
//  Description     : Process ECR Command.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EcrCmdProcess(void)
{
  //BYTE *wav_dat;

  ECR_Clear();
  if (STIS_TERM_DATA.b_stis_mode!=TRANS_MODE) {
    ResetTerm();
    return;
  }

#if 0
  if (gCdc1Handle != -1) {
    wav_dat = (BYTE *)MallocMW(sizeof(KWelcomeWav));
    if (wav_dat != NULL) {
      memcpy(wav_dat, KWelcomeWav, sizeof(KWelcomeWav));
      while (StatMW(gCdc1Handle, MW_CODEC_STATUS, NULL) & MW_CODEC_PLAYING) SleepMW();
      WriteMW(gCdc1Handle, wav_dat, sizeof(KWelcomeWav));
      FreeMW(wav_dat);
    }
  }
#endif

  ClearResponse();
  switch (gGDS->s_ecr_data.b_cmd) {
    case ECR_EDC_SALE:
      LongBeep();
      WaitCardTrans(SALE_SWIPE);
      EcrResp();
      ResetTerm();
      break;
    case ECR_EDC_OFFLSALE:
      LongBeep();
      WaitCardTrans(SALE_OFFLINE);
      EcrResp();
      ResetTerm();
      break;
    case ECR_EDC_LASTTRACE:
      LongBeep();
      GetLastTrace();
      RefreshDispAfter(0);
      break;
    case ECR_EDC_HOSTNAME:
      LongBeep();
      GetHostName();
      RefreshDispAfter(0);
      break;
    case ECR_EDC_HOSTTOTAL:
      LongBeep();
      GetHostTot();
      RefreshDispAfter(0);
      break;
    case ECR_EDC_SETTLE:
      LongBeep();
      EcrSettle();
      RefreshDispAfter(0);
      break;
  }
  ResetTerm();
}

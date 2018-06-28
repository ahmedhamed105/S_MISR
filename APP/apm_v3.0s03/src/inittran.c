//-----------------------------------------------------------------------------
//  File          : InitTran.c
//  Module        :
//  Description   : STIS Init Transactions..
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
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
#include "midware.h"
#include "emv2dll.h"
#include "util.h"
#include "sysutil.h"
#include "menu.h"
#include "termdata.h"
#include "stis.h"
#include "tranutil.h"
#include "comm.h"
#include "constant.h"
#include "emvtrans.h"
#include "batchsys.h"
#include "coremain.h"
#include "message.h"
#include "inittran.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Global Variables
//-----------------------------------------------------------------------------
static struct COMMBUF *psTxBuf;
static struct COMMBUF *psRxBuf;
struct BATCH_NO {
  BYTE sb_curr[3];
  BYTE sb_next[3];
};
static struct BATCH_NO *psBatchNo=NULL;
static int    iSaveAcqCount=0;
static int    iExParamCount=0;

//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------
// Default Init Selection Menu
const struct MENU_ITEM KInitItem [] =
{
  {  MODE_ALL_PARAM,       "All Parameters"},
  {  MODE_EMV_PARAM_ONLY,  "EMV Param Only"},
  {  MODE_EMV_KEY_ONLY,    "EMV Keys Only"},
  {  MODE_EMV_PARAM_KEY,   "EMV Param & Key"},
  {  MODE_EXTRA_PARAM,     "Extra Parameters"},
  { -1, NULL},
};

const struct MENU_DAT KInitMenu  = {
  "STIS Init.",
  KInitItem,
};

//*****************************************************************************
//  Function        : PackHostMsg
//  Description     : Pack Host Message Base on global input.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackHostMsg(void)
{
  BYTE txtrans;
  BYTE tx_bitmap[8];
  BYTE *spptr = NULL;
  BYTE *epptr;
  BYTE tmplrc;
  WORD tmplen;

  // set pointer
  set_pptr(psTxBuf->sbContent, COMMBUF_SIZE);

  // reserve for 2 bytes len
  if (((gCommParam.bCommMode==APM_COMM_GPRS)||(gCommParam.bCommMode == APM_COMM_TCPIP)||(gCommParam.bCommMode==APM_COMM_WIFI)||(gCommParam.bCommMode==APM_COMM_BT)) && 
      (gCommParam.sTcp.bAdd2ByteLen)) {
    inc_pptr(2);
  }

  // STX LEN or TPDU
  if (gCommParam.bCommMode == APM_COMM_ASYNC) {  // pack stx & len
    pack_byte(0x02);
    pack_byte(0x00);
    pack_byte(0x00);
    spptr = get_pptr();
  }
  else {
    pack_byte(0x60); // TPDU header
    pack_mem(gsTransData.s_input.sb_nii, 2);
    pack_null(2);
  }

  // msg type id, bit map
  pack_word(KTransBitmap[txtrans=gsTransData.s_input.b_trans].w_txmsg_id);
  pack_mem((BYTE *)KTransBitmap[txtrans].sb_txbitmap, 8);
  memcpy(tx_bitmap, KTransBitmap[txtrans].sb_txbitmap, 8);

  // 03. processing code
  if ((tx_bitmap[0] & 0x20) != 0) {
    pack_byte(KTransBitmap[txtrans].b_proc_code);
    if (gsTransData.s_input.b_trans==EDC_INIT)
      pack_byte(gsTransData.s_input.b_stis_init_mode);
    else
      pack_byte(0x00);
    pack_byte(gsTransData.s_input.b_proc_code2);
  }

  // 11. systems trace audit number
  if ((tx_bitmap[1] & 0x20) != 0) {
    pack_mem(gsTransData.s_input.sb_trace_no, 3);
  }

  // 24. network international identifier
  if ((tx_bitmap[2] & 0x01) != 0){
    pack_mem(gsTransData.s_input.sb_nii, 2);
  }

  // 41. terminal identification
  if ((tx_bitmap[5] & 0x80) != 0) {
    pack_mem(gsTransData.s_input.sb_term_id, 8);
  }

  // 60. reserved private
  if ((tx_bitmap[7] & 0x10) != 0) {
    if ((gsTransData.s_input.b_trans==EDC_INIT)&&(gsTransData.s_rsp_data.w_rspcode == 'Z'*256+'1')) {
      pack_word(0x0001); /* length */
      pack_byte('Y');
    }
    else {
      pack_word(0x0010); /* length */
      pack_mem((BYTE *)KSoftName,10);   //N2C check this or KVersion
    }
  }

  // pack etx & lrc
  if (gCommParam.bCommMode == APM_COMM_ASYNC) {
    epptr = get_pptr();
    tmplen = epptr-spptr;
    tmplrc = (BYTE)lrc(spptr, tmplen);
    pack_byte(tmplrc);
    pack_byte(0x03);
    *(spptr-2) = tmplen/256;
    *(spptr-1) = tmplen%256;
  }

  // set total length
  psTxBuf->wLen = (WORD)get_distance();

  // pack 2 bytes len
  if (((gCommParam.bCommMode==APM_COMM_GPRS)||(gCommParam.bCommMode == APM_COMM_TCPIP)||(gCommParam.bCommMode==APM_COMM_WIFI)||(gCommParam.bCommMode==APM_COMM_BT)) && (gCommParam.sTcp.bAdd2ByteLen)) {
    tmplen = psTxBuf->wLen - 2;
    psTxBuf->sbContent[0] = (BYTE)((tmplen >> 8) & 0xFF);
    psTxBuf->sbContent[1] = (BYTE)(tmplen & 0xFF);
  }
}
//*****************************************************************************
//  Function        : CheckHostRsp
//  Description     : UnPack Host Message to rsp_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE CheckHostRsp(void)
{
  int i;
  WORD sub_field_len;
  BYTE sub_type;
  DWORD logo_index;
  DWORD card_index;
  DWORD more_msg;
  APP_CFG app_cfg;
  KEY_ROOM key_room;
  BYTE *pDat;
  struct DATETIME dtg;
  WORD msg_id;
  BYTE bitmap[8];
  BYTE tmp;
  BYTE *ptr_end;
  void *p_mem;

  more_msg = 0;
  set_pptr(psRxBuf->sbContent, COMMBUF_SIZE);

  // pack additonal 2 byte msg len
  if (((gCommParam.bCommMode==APM_COMM_GPRS)||(gCommParam.bCommMode == APM_COMM_TCPIP)||(gCommParam.bCommMode==APM_COMM_WIFI)||(gCommParam.bCommMode==APM_COMM_BT)) && 
      (gCommParam.sTcp.bAdd2ByteLen))
    inc_pptr(2);

  // LRC or TPDU
  if (gCommParam.bCommMode == APM_COMM_ASYNC) { // verify LRC
    inc_pptr(1); // skip STX
    i = get_word();
    pDat = get_pptr();
    if (lrc(pDat, i+1)!=0) { // include LRC
      SetRspCode('I'*256+'R', FALSE);
      return TRANS_FAIL;
    }
  }
  else {
    inc_pptr(5); // 5 bytes tpdu_header;
  }

  // Msg ID
  if ((msg_id=(WORD)get_word()) == 0x0820)
    return 0xff; /// please wait message
  if ((KTransBitmap[gsTransData.s_input.b_trans].w_txmsg_id + 0x10) != msg_id) {
    SetRspCode('I'*256+'R', FALSE);
    return TRANS_FAIL;
  }

  get_mem(bitmap, 8);

  // 03. processing code
  if (bitmap[0] & 0x20) {
    tmp = (BYTE)get_byte();
    if (tmp != KTransBitmap[gsTransData.s_input.b_trans].b_proc_code) {
      SetRspCode('I'*256+'P', FALSE);
      return TRANS_FAIL;
    }
    inc_pptr(1);
    tmp = (BYTE)get_byte();
    if ((gsTransData.s_input.b_trans == EDC_INIT)&&(tmp == 0x01))
      more_msg = 1;
    else
      if (tmp != 0x00) {
        SetRspCode('I'*256+'P', FALSE);
        return TRANS_FAIL;
      }
  }

  // 11. check system trace no
  if (bitmap[1] & 0x20) {
    if (memcmp(get_pptr(), gsTransData.s_input.sb_trace_no, 3)) {
      SetRspCode('I'*256+'S', FALSE);
      return TRANS_FAIL;
    }
    inc_pptr(3);
  }

  // 12. trans time
  if (bitmap[1] & 0x10)
    get_mem(gsTransData.s_rsp_data.sb_time, 3);

  // 13. trans date
  if (bitmap[1] & 0x08)
    get_mem(gsTransData.s_rsp_data.sb_date, 2);


  // 24. bypass netword id
  if (bitmap[2] & 0x01)
    inc_pptr(2);

  // 25. pos condition code
  if (bitmap[3] & 0x80)
    inc_pptr(1);

  // 32.
  if (bitmap[3] & 0x01)
    inc_pptr((BYTE)((bcd2bin(peek_byte()) + 1)/2 + 1));

  // 37. ret. ref. no.
  if (bitmap[4] & 0x08)
    get_mem(gsTransData.s_rsp_data.sb_rrn, 12);

  // 38. auth code
  memset(gsTransData.s_rsp_data.sb_auth_code, ' ', 6);
  if (bitmap[4] & 0x04)
    get_mem(gsTransData.s_rsp_data.sb_auth_code, 6);

  // 39. response code
  gsTransData.s_rsp_data.w_rspcode = '0'*256+'0'; // upload response does not have response code
  if (bitmap[4] & 0x02) {
    gsTransData.s_rsp_data.w_rspcode = (WORD)get_word();
  }

  // 41. term id
  if (bitmap[5] & 0x80) {
    if (memcmp(gsTransData.s_input.sb_term_id, get_pptr(), 8)) {
      SetRspCode('I'*256+'T', FALSE);
      return TRANS_FAIL;
    }
    inc_pptr(8);
  }

  // 60. private field
  if (bitmap[7] & 0x10)
    if (gsTransData.s_input.b_trans == EDC_INIT) { // initialization parameter
      ptr_end = get_pptr() + bcd2bin(get_word());
      while (get_pptr() < ptr_end) {
        sub_type = (BYTE)get_byte(); // sub-type
        sub_field_len = (WORD)bcd2bin(get_word());
        i = (BYTE)bcd2bin(peek_byte());
        switch (sub_type) {
          case 0:
            if (psBatchNo != NULL) {
              FreeMW(psBatchNo);
              psBatchNo = NULL;
            }
            iSaveAcqCount = GetAcqCount();
            if (iSaveAcqCount > 0) {
              psBatchNo = (struct BATCH_NO *) MallocMW(sizeof(struct BATCH_NO)*iSaveAcqCount);
              for (i = 0; i < iSaveAcqCount; i++) {
                GetBatchNo(i, psBatchNo[i].sb_curr);
              }
            }
            fCommitAllMW();
            CloseSTISFiles();
            CreateSTISFiles();
            SetSTISMode(INIT_MODE);
            fCommitAllMW();
            break;
          case 1:
            if ((p_mem = (void *)MallocMW(sizeof(struct TERM_CFG))) == NULL) {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            memcpy(p_mem, get_pptr(), sizeof(struct TERM_CFG));
            memcpy(&gsTransData.s_rsp_data.b_year, ((struct TERM_CFG*)p_mem)->sb_date_time, 6);
            ReadRTC(&dtg);
            memcpy(&dtg.bYear, &gsTransData.s_rsp_data.b_year, 6);
            SetRTC(&dtg);
            ((struct TERM_CFG*)p_mem)->b_trans_amount_len = (BYTE)bcd2bin(((struct TERM_CFG*)p_mem)->b_trans_amount_len);
            ((struct TERM_CFG*)p_mem)->b_settle_amount_len = (BYTE)bcd2bin(((struct TERM_CFG*)p_mem)->b_settle_amount_len);
            UpdTermCfg((struct TERM_CFG*)p_mem);
            FreeMW(p_mem);
            break;
          case 2:
            if ((p_mem = (void *)MallocMW(sizeof(struct CARD_TBL))) == NULL) {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            memcpy(p_mem, get_pptr()+1, sizeof(struct CARD_TBL));
            // convert idx into HEX for storage
            ((struct CARD_TBL*)p_mem)->b_acquirer_id = (BYTE)bcd2bin(((struct CARD_TBL*)p_mem)->b_acquirer_id);
            ((struct CARD_TBL*)p_mem)->b_issuer_id = (BYTE)bcd2bin(((struct CARD_TBL*)p_mem)->b_issuer_id);
            //26-08-14 JC ++
            //fix bug if more than 99 card tables
            //UpdCardTbl(i-1, (struct CARD_TBL*)p_mem);
            card_index = GetCardCount();
            UpdCardTbl(card_index, (struct CARD_TBL*)p_mem);
            //26-08-14 JC --
            FreeMW(p_mem);
            break;
          case 3:
            i--;
            if ((p_mem = (void *)MallocMW(sizeof(struct ISSUER_TBL))) == NULL) {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            memcpy(p_mem, get_pptr()+1, sizeof(struct ISSUER_TBL));
            // convert idx into HEX for storage
            ((struct ISSUER_TBL*)p_mem)->b_id = (BYTE)bcd2bin(((struct ISSUER_TBL*)p_mem)->b_id);
            UpdIssuerTbl(i, (struct ISSUER_TBL*)p_mem);
            FreeMW(p_mem);
            break;
          case 4:
            i--;
            if ((p_mem = (void *)MallocMW(sizeof(struct ACQUIRER_TBL))) == NULL) {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            memcpy(p_mem, get_pptr()+1, sizeof(struct ACQUIRER_TBL));
            // convert idx into HEX for storage
            ((struct ACQUIRER_TBL*)p_mem)->b_id = (BYTE)bcd2bin(((struct ACQUIRER_TBL*)p_mem)->b_id);
            if ((skpb(((struct ACQUIRER_TBL*)p_mem)->sb_curr_batch_no, 0, 3) == 3) && (i < iSaveAcqCount))
              memcpy(((struct ACQUIRER_TBL*)p_mem)->sb_curr_batch_no, (BYTE *)&psBatchNo[i].sb_curr[0], 3);
            if ((skpb(((struct ACQUIRER_TBL*)p_mem)->sb_next_batch_no, 0, 3) == 3) && (i < iSaveAcqCount))
              memcpy(((struct ACQUIRER_TBL*)p_mem)->sb_next_batch_no, (BYTE *)&psBatchNo[i].sb_next[0], 3);
            ((struct ACQUIRER_TBL*)p_mem)->b_status=UP;
            ((struct ACQUIRER_TBL*)p_mem)->b_pending=NO_PENDING;
            UpdAcqTbl(i, (struct ACQUIRER_TBL*)p_mem);
            FreeMW(p_mem);
            break;
          case 5:
            i--;
            if ((p_mem = (void *)MallocMW(sizeof(struct DESC_TBL))) == NULL) {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            memcpy(p_mem, get_pptr()+1, sizeof(struct DESC_TBL));
            UpdDescTbl(i, (struct DESC_TBL*)p_mem);
            FreeMW(p_mem);
            break;
          case 6:
            if (sub_field_len != 143) {
              if ((p_mem = (void *)MallocMW(sizeof(struct LOGO_TBL))) == NULL) {
                gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
                return TRANS_FAIL;
              }
              logo_index = *get_pptr();
              ((struct LOGO_TBL*)p_mem)->b_flag = 0x01;
              ((struct LOGO_TBL*)p_mem)->b_type = *(get_pptr()+1);
              memcpy(((struct LOGO_TBL*)p_mem)->sb_image, get_pptr()+2, sub_field_len-2);
              UpdLogoTbl(logo_index, (struct LOGO_TBL*)p_mem);
              FreeMW(p_mem);
            }
            break;
          case 7: // clear all EMVL2 table
            if (((gsTransData.s_input.b_stis_init_mode!=MODE_ALL_PARAM)&&(gsTransData.s_input.b_stis_init_mode!=MODE_EMV_PARAM_KEY)) ||
                (gsTransData.s_rsp_data.w_rspcode!=('0'*256+'0')))
              break;
            SetEMVLoaded(0x80|0x40);  // clear both EMV param & key
            break;
          case 8: // AID TABLE
            if ((gsTransData.s_input.b_stis_init_mode == MODE_EXTRA_PARAM) || (gsTransData.s_input.b_stis_init_mode == MODE_EMV_KEY_ONLY) ||
                (gsTransData.s_rsp_data.w_rspcode!=('0'*256+'0')))
              break;
            if (sub_field_len == 223) {
              pDat = get_pptr();
              app_cfg.eType = *pDat++;
              tmp = *pDat++;
              app_cfg.eBitField = tmp;
              app_cfg.eRSBThresh = DWGet(pDat);
              pDat+= 4;
              app_cfg.eRSTarget = *pDat++;
              app_cfg.eRSBMax = *pDat++;
              memcpy(app_cfg.eTACDenial, pDat, 5);
              pDat += 5;
              memcpy(app_cfg.eTACOnline, pDat, 5);
              pDat += 5;
              memcpy(app_cfg.eTACDefault, pDat, 5);
              pDat += 5;
              memcpy(app_cfg.eACFG, pDat, 200);
              EMVData2DnloadBuf(IOCMD_LOAD_AID, (BYTE *)&app_cfg, sizeof(APP_CFG));
              SetEMVLoaded(0x01); // lower nibble for status
            }
            else {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            break;
          case 9:// KEY TABLE
            if ((gsTransData.s_input.b_stis_init_mode==MODE_EXTRA_PARAM)||
                ((gsTransData.s_rsp_data.w_rspcode!='0'*256+'0')&&(gsTransData.s_rsp_data.w_rspcode!='Z'*256+'1')))
              break;
            if (sub_field_len == 275) {
              pDat = get_pptr();
              key_room.KeyIdx = *pDat++;
              memcpy(key_room.RID, pDat, 5);
              pDat += 5;
              key_room.CAKey.Exponent = DWGet(pDat);
              pDat += 4;
              key_room.CAKey.KeySize = WGet(pDat);
              pDat += 2;
              memcpy(key_room.CAKey.Key, pDat, 256);
              pDat += 256;
              pDat++; // skip CC
              key_room.ExpiryDate = pDat[1]*256+pDat[0];  // CC to be ignore
              pDat += 2;
              pDat++; // skip CC
              key_room.EffectDate = pDat[1]*256+pDat[0];  // CC to be ignore
              pDat += 2;
              key_room.Chksum = *pDat++;
              EMVData2DnloadBuf(IOCMD_KEY_LOADING, (BYTE *)&key_room, sizeof(KEY_ROOM));
            }
            else {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            break;
          case 0x10: // Clear AID Table
            if ((gsTransData.s_input.b_stis_init_mode==MODE_EXTRA_PARAM) || (gsTransData.s_input.b_stis_init_mode==MODE_EMV_KEY_ONLY))
              break;
            SetEMVLoaded(0x80);
            break;
          case 0x11: // Clear Key Table
            if (gsTransData.s_input.b_stis_init_mode==MODE_EXTRA_PARAM)
              break;
            SetEMVLoaded(0x40);
            break;
          case 0x12: // Clear all extra param
            if ((gsTransData.s_input.b_stis_init_mode!=MODE_ALL_PARAM) && (gsTransData.s_input.b_stis_init_mode!=MODE_EXTRA_PARAM))
              break;
            iExParamCount = 0;
            CloseExParamFiles();
            CreateExParamFiles();
            break;
          case 0x13:
            if (((gsTransData.s_input.b_stis_init_mode!=MODE_ALL_PARAM)&&(gsTransData.s_input.b_stis_init_mode!=MODE_EXTRA_PARAM)) ||
                (gsTransData.s_rsp_data.w_rspcode!=('0'*256+'0')))
              break;
            if (sub_field_len > 53) {
              gsTransData.s_rsp_data.w_rspcode = 'I'*256+'T';
              return TRANS_FAIL;
            }
            if ((p_mem = (void *)MallocMW(sizeof(struct EXTRA_PARAM))) == NULL) {
              gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
              return TRANS_FAIL;
            }
            pDat = get_pptr();
            ((struct EXTRA_PARAM*)p_mem)->b_app_id = *pDat++;
            ((struct EXTRA_PARAM*)p_mem)->b_seq_no = *pDat++;
            ((struct EXTRA_PARAM*)p_mem)->b_len = (BYTE)bcd2bin(WGet(pDat));
            pDat += 2;
            // Check if there is prefix.
            if ((sub_field_len - (((struct EXTRA_PARAM*)p_mem)->b_len+4)) > 0)
              ((struct EXTRA_PARAM*)p_mem)->b_prefix = *pDat++;
            else
              ((struct EXTRA_PARAM*)p_mem)->b_prefix = 0;
            memcpy(((struct EXTRA_PARAM*)p_mem)->sb_content, pDat, ((struct EXTRA_PARAM*)p_mem)->b_len);
            UpdExtraParam(iExParamCount++, (struct EXTRA_PARAM*)p_mem);
            FreeMW(p_mem);
            break;
        }
        inc_pptr(sub_field_len);
      }
    }

  // 62. private field
  if (bitmap[7] & 0x04) {
    i = (BYTE)get_word();
    inc_pptr(i);
  }

  // 63. private field
  if (bitmap[7] & 0x02) {
    gsTransData.s_rsp_data.sb_text[0] = (BYTE)get_word();
    get_mem(&gsTransData.s_rsp_data.sb_text[1], 69);
  }

  // avoid year = 00 if only download EMV or EXTRA param
  if ((p_mem = (void *)MallocMW(sizeof(struct TERM_CFG))) == NULL) {
    gsTransData.s_rsp_data.w_rspcode = 'D'*256+'E';
    return TRANS_FAIL;
  }
  GetTermCfg((struct TERM_CFG*)p_mem);
  memcpy(&gsTransData.s_rsp_data.b_year, ((struct TERM_CFG*)p_mem)->sb_date_time, 6);
  gsTransData.s_rsp_data.b_year=SetDTG(&gsTransData.s_rsp_data.b_year);
  FreeMW(p_mem);

  if (more_msg==0) {
    if (psBatchNo != NULL) {
      FreeMW(psBatchNo);
      psBatchNo = NULL;
    }
  }

  if (gsTransData.s_rsp_data.w_rspcode == '0'*256+'0') {
    if (more_msg)
      return MORE_RSP;
    return TRANS_ACP;
  }
  else if (gsTransData.s_rsp_data.w_rspcode == 'Z'*256+'1') {
    if (gsTransData.s_input.b_trans==EDC_INIT)
    {
      if (more_msg)
        return(MORE_RSP);
      else
        return(TRANS_ACP);
    }
    gsTransData.s_rsp_data.w_rspcode = '0'*256+'0';
    return TRANS_ACP;
  }
  else
    return TRANS_REJ;

}
//*****************************************************************************
//  Function        : InitTrans
//  Description     : Download parameters from STIS.
//  Input           : N/A
//  Return          : response (result)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD InitTrans(void)
{
  int i, func_sel, acq_count;
  struct TERM_DATA term_data;
  struct ACQUIRER_TBL acq_tbl;
  DWORD response = TRANS_FAIL;
  BYTE trace_no[3];

  if (GetSTISMode() == SETUP_MODE)
    return response;

  if ((func_sel = MenuSelect(&KInitMenu, 0)) == -1)
    return response;

  DispCtrlMW(MW_CLR_DISP);
  DispHeader();
  if (DebugModeON()&&TrainingModeON()) {
    SetSTISMode(TRANS_MODE);
    SetRspCode('0'*256+'0', TRUE);
    TransEnd();
    Delay1Sec(10, 0);
    return TRANS_ACP;
  }

  gsTransData.s_input.b_trans = EDC_INIT;
  gsTransData.s_input.b_stis_init_mode = func_sel;
  if (!GetTermData(&term_data))
    return response;

  memcpy(gsTransData.s_input.sb_term_id,term_data.sb_term_id, 8);

  PackInitComm();
  gsTransData.s_input.b_proc_code2 = 0;
  ClearResponse();

  if ((gsTransData.s_rsp_data.w_rspcode=ConnectOK(FALSE)) == COMM_OK) {
    EMVClrDnloadBuf();
    // Clear STIS data 1st;
    psTxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
    psRxBuf  = (struct COMMBUF *) MallocMW(sizeof(struct COMMBUF));
    MemFatalErr(psTxBuf);
    MemFatalErr(psRxBuf);
    while (TRUE) {
      GetTrace(trace_no);
      memcpy(gsTransData.s_input.sb_trace_no, trace_no, 3);
      PackHostMsg();
      if ((gsTransData.s_rsp_data.w_rspcode=SendRcvd(psTxBuf,psRxBuf))==COMM_OK) {
        IncTrace();
        response = gsTransData.b_response = CheckHostRsp();
      }
      else {
        if (GetSTISMode() == INIT_MODE) {
          fCommitAllMW();
          CloseSTISFiles();
          CreateSTISFiles();
          CloseSTISFiles();
          STISInit();
          SetSTISMode(INIT_MODE);
          fCommitAllMW();
        }
        response = gsTransData.b_response = TRANS_FAIL;
        break;
      }
      if ((gsTransData.s_input.b_stis_init_mode == MODE_ALL_PARAM) && (gsTransData.b_response == TRANS_ACP)) {
        acq_count = GetAcqCount();
        for (i=0; i < acq_count; i++) {
          if (GetAcqTbl(i, &acq_tbl)) {
            if (acq_tbl.b_status != NOT_LOADED) {
              SetSTISMode(TRANS_MODE);
              // N2C not support VBin yet
              //VBinAllowed();
              break;
            }
          }
        }
      }
      else if (gsTransData.b_response == MORE_RSP) {
        gsTransData.s_input.b_proc_code2 = 0x01;
        continue; /* more msg to receive */
      }
      break;
    }

    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg(KProcessing, MW_LINE5, MW_CENTER|MW_BIGFONT);
    fCommitAllMW();  // Commit All save data.
    if (gsTransData.b_response == TRANS_ACP) {
      EMVDataUpdate();
      EMVInit();
    }
    EMVKillDnloadBuf();
    fCommitAllMW();  // Commit All save data.

    FreeMW(psTxBuf);
    FreeMW(psRxBuf);
  }
  ResetComm();
  BatSysInit(GetAcqCount());
  TransEnd();
  return response;
}

//-----------------------------------------------------------------------------
//  File          : iface.c
//  Module        :
//  Description   : interface for spectra proprietary
//  Author        : Pody
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g                                         |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |            Char size : Leading c                                         |
// |             Int size : Leading i                                         |
// |            Byte size : Leading b                                         |
// |            Word size : Leading w                                         |
// |           Dword size : Leading d                                         |
// |          DDword size : Leading dd                                        |
// |          Struct size : Leading s                                         |
// |                Array : Leading a, (ab = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  2008 Apr      Pody        Initial Version.
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include "util.h"
#include "midware.h"
#include "sysutil.h"
#include "key2dll.h"
#include "emv2dll.h"
#include "apm.h"
#include "corevar.h"
#include "tranutil.h"
#include "tlvutil.h"
#include "constant.h"
#include "input.h"
#include "ecrdll.h"
#include "ecr.h"
#include "void.h"
#include "settle.h"
#include "auxcom.h"
#include "iface_util.h"
#include "iface.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define S3_FMT_ID                 (DWORD)1      // spectra 3 format id
#define SKEY_MASK                 (BYTE)0x80    //Session Key bit
#define ANY_SKEY                  (BYTE)0xFF    //for get any Session Key
#define MRMK                      (BYTE)0x00    //Master Reader Message Key
#define RMK                       (BYTE)0x01    //Reader Message Key

// offset
#define OSET_STX                  (WORD)0
#define OSET_FMT_ID               (WORD)1
#define OSET_TX_ID                (WORD)5
#define OSET_RX_ID                (WORD)6
#define OSET_SEQ_NUM              (WORD)7
#define OSET_INS                  (WORD)9
#define OSET_DATA_FMT             (WORD)10
#define OSET_DATA_LEN             (WORD)11
#define OSET_REQ_DATA             (WORD)13
#define OSET_RSP_CODE             (WORD)13
#define OSET_RSP_DATA             (WORD)14
#define MIN_MSG_LEN               (WORD)(OSET_DATA_LEN + 7)
#define MAX_VDATA_LEN             (WORD)(2048 - MIN_MSG_LEN)

// variable parameter tag
#define MAX_VTAG_SIZE             (WORD)256
#define VTAG_START                (WORD)0xFFA0
#define VTAG_UIM_START            (WORD)VTAG_START      //ui msg tag start
#define VTAG_UIM_END              (WORD)(VTAG_UIM_START+UI_END)
#define VTAG_END                  (WORD)VTAG_UIM_END                  

#define PORT_PDATA_MASK         0xFF

#define DbFErr
#define DbStep

//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------
static const BYTE KIfaceFile[] = {"IfaceFile"};

const WORD KItfSaleResp[] = {
  ITFTAG_PAN,       ITFTAG_TXNAMT,   ITFTAG_TRACE,     ITFTAG_TXNTIME,
  ITFTAG_TXNDATE,   ITFTAG_EXPDATE,  ITFTAG_RRN,       ITFTAG_AUTH_RESP,
  ITFTAG_RESP_CODE, ITFTAG_TERMID,   ITFTAG_MERCHID,   ITFTAG_CARDHOLDERNAME,
  ITFTAG_ADDAMT,    ITFTAG_ENTRYMODE,ITFTAG_BATCHNO,   ITFTAG_ECRREF,
  ITFTAG_TXNTYPE,   ITFTAG_AID,      0x00
};

const WORD KItfOfflSaleResp[] = {
  ITFTAG_PAN,       ITFTAG_TXNAMT,   ITFTAG_TRACE,     ITFTAG_TXNTIME,
  ITFTAG_TXNDATE,   ITFTAG_EXPDATE,                    ITFTAG_AUTH_RESP,
  ITFTAG_RESP_CODE, ITFTAG_TERMID,   ITFTAG_MERCHID,   ITFTAG_CARDHOLDERNAME,
  ITFTAG_ADDAMT,    ITFTAG_ENTRYMODE,ITFTAG_BATCHNO,  ITFTAG_ECRREF,
  ITFTAG_TXNTYPE,   ITFTAG_AID,      0x00
};

const WORD KItfLastTrace[] = {
  ITFTAG_TRACE, ITFTAG_ECRREF, ITFTAG_RESP_CODE, ITFTAG_TXNTYPE, 0x00
};

const WORD KItfHostNameResp[] = {
  ITFTAG_RESP_CODE, ITFTAG_HOSTNAME, ITFTAG_TXNTYPE, 0x00
};

const WORD KItfHostTotResp[] = {
  ITFTAG_RESP_CODE, ITFTAG_HOSTTOT, ITFTAG_TXNTYPE, 0x00
};
const WORD KItfSettleResp[] = {
  ITFTAG_RESP_CODE, ITFTAG_HOSTTOT, ITFTAG_TXNTYPE, 0x00
};
const WORD *KItfRespTbl[MAX_ECREDC_CMD] = {
  KItfSaleResp,      // ITF_EDC_SALE
  KItfOfflSaleResp,  // ITF_EDC_OFFLSALE
  KItfSaleResp,      // ITF_EDC_REFUND
  KItfSaleResp,      // ITF_EDC_VOID
  KItfSaleResp,      // ITF_EDC_GETTXN
  KItfLastTrace,     // ITF_EDC_LASTTRACE
  KItfHostNameResp,  // ITF_EDC_HOSTNAME
  KItfHostTotResp,   // ITF_EDC_HOSTTOTAL
  KItfSaleResp,      // ITF_EDC_AUTH
  KItfSettleResp,    // ITF_EDC_SETTLE
};

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------
static BYTE gbMode = 0x00;  // mode
static WORD gwSeqNum = 0;   // seq num
static S3KEY gsSKey;        // session key
static BYTE gabRRand[8];    // reader random
static BYTE gabTRand[8];    // terminal random

struct IFACE_DATA *gIfaceData=NULL;
static BYTE gIfBuf[MAX_IFACE_BUF];      // Tx/Rx Buffer
static BYTE gReqBuf[MAX_IFACE_BUF];     // Rx Buffer
static BYTE gRspBuf[MAX_IFACE_BUF];     // Tx Buffer
static struct TOTAL_STRUCT gTermTotal;

//----------------------------------------------------------------------------
// Generic API
//----------------------------------------------------------------------------


//*****************************************************************************
//  Function        : IfaceInitData
//  Description     : Load Interface data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IfaceInitData(void)
{
  BYTE filename[32];
  DWORD len;
  int  fd;

  if (gIfaceData == NULL) {
    gIfaceData = (struct IFACE_DATA*) MallocMW(sizeof(struct IFACE_DATA));
    if (gIfaceData == NULL) {
      MemFatalErr(gIfaceData);  // Check Memory
      return;
    }
  }
  strcpy(filename, KIfaceFile);
  fd = fOpenMW(filename);
  if (fd < 0) {
    fd = fCreateMW(filename, 0);
    memset(gIfaceData, 0, sizeof(struct IFACE_DATA));
    len = fWriteMW(fd, gIfaceData, sizeof(struct IFACE_DATA));
    if (len != sizeof(struct IFACE_DATA)) {
      DispLineMW("File WR Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      ErrorDelay();
    }
    fCommitAllMW();
  }
  else {
    len = fReadMW(fd, gIfaceData, sizeof(struct IFACE_DATA));
    if (len != sizeof(struct IFACE_DATA)) {
      DispLineMW("File RD Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      ErrorDelay();
    }
  }
  fCloseMW(fd);
}
//*****************************************************************************
//  Function        : UpdIfaceDat
//  Description     : Update Interface data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void UpdIfaceDat(void)
{
  BYTE filename[32];
  DWORD len;
  int  fd;

  strcpy(filename, KIfaceFile);
  fd = fOpenMW(filename);
  if (fd >= 0) {
    len = fWriteMW(fd, gIfaceData, sizeof(struct IFACE_DATA));
    if (len != sizeof(struct IFACE_DATA)) {
      DispLineMW("File WR Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      ErrorDelay();
    }
  }
  fCloseMW(fd);
}
//=============================================================================
//  Function        : errRspData
//  Description     : pack response error data
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int errRspData(BYTE aRCode, BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int iLen;
  BYTE *pRCode;

  // rsp code
  iLen = 0;
  pRCode = &aRspMsg[OSET_RSP_CODE];
  pRCode[iLen++] = aRCode;

  // rsp data len
  WPut(&aRspMsg[OSET_DATA_LEN], iLen);
  iLen += 2;
  return iLen;
}

//=============================================================================
//  Function        : keyGet
//  Description     :
//  Input           : aType, aIdx
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
#define KEK         (BYTE*)"Spectra\x00"
static BOOLEAN keyGet(BYTE *aKey, BYTE aType, BYTE aIdx)
{
  S3KEY sKey;

  
  // get MRMK or RMK
  switch (aType) {
    case MRMK:
      //PDataGet(&sKey, &gpsPD->sFS3PData.sMrmk, sizeof(S3KEY));
      memcpy(&sKey, &gIfaceData->sFS3PData.sMrmk, sizeof(S3KEY));
      if (!(sKey.bStatus & S3KEY_INJECTED)) { //initial master key
        memcpy(aKey, sKey.abDKey, 16);
        return TRUE;
      }
      else { // injected master key
        if ((sKey.bType == aType) && (sKey.bIdx == aIdx)) {
          memcpy(aKey, sKey.abDKey, 16);
          Des2(KEK, aKey, 16);
          return TRUE;
        }
      }
      break;
    case RMK:
      //PDataGet(&sKey, &gpsPD->sFS3PData.sRmk, sizeof(S3KEY));
      memcpy(&sKey, &gIfaceData->sFS3PData.sRmk, sizeof(S3KEY));
      if ((sKey.bStatus & S3KEY_INJECTED)
          && (sKey.bType == aType)
          && (sKey.bIdx == aIdx)) {
        memcpy(aKey, sKey.abDKey, 16);
        Des2(KEK, aKey, 16);
        return TRUE;
      }
      break;
    default:
      break;
  }

  // get SKEY
  if (aType & SKEY_MASK) {
    if (gsSKey.bStatus & S3KEY_INJECTED) {
      if ((aType == ANY_SKEY) // any skey
          || (gsSKey.bType < aType) // lower level key
          || ((gsSKey.bType == aType) && (gsSKey.bIdx == aIdx))) { // same level key and same idx
        memcpy(aKey, gsSKey.abDKey, 16);
        return TRUE;
      }
    }
  }

  return FALSE;
}

//=============================================================================
//  Function        : keyPut
//  Description     :
//  Input           : aKey - if NULL, erase key
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static BOOLEAN keyPut(BYTE *aKey, BYTE aType, BYTE aIdx)
{
  S3KEY sKey;

  // format key
  memset(&sKey, 0x00, sizeof(S3KEY));
  if (aKey != NULL) { // not erase key
    memcpy(sKey.abDKey, aKey, 16);
    sKey.bType = aType;
    sKey.bIdx = aIdx;
    sKey.bStatus |= S3KEY_INJECTED;
  }

  // save key based on type
  switch (aType) {
    case MRMK:
      Des(KEK, sKey.abDKey, 16);
      //PDataPut(&gpsPD->sFS3PData.sMrmk, &sKey, sizeof(S3KEY));
      //PDataCrcUpdate(&gpsPD->bFStart, &gpsPD->wFCrc);
      memcpy(&gIfaceData->sFS3PData.sMrmk, &sKey, sizeof(S3KEY));
      UpdIfaceDat();
      return TRUE;
    case RMK:
      Des(KEK, sKey.abDKey, 16);
      //PDataPut(&gpsPD->sFS3PData.sRmk, &sKey, sizeof(S3KEY));
      //PDataCrcUpdate(&gpsPD->bFStart, &gpsPD->wFCrc);
      memcpy(&gIfaceData->sFS3PData.sRmk, &sKey, sizeof(S3KEY));
      UpdIfaceDat();
      return TRUE;
    default:
      break;
  }

  // save skey
  if (aType & SKEY_MASK) {
    memcpy(&gsSKey, &sKey, sizeof(S3KEY));
    return TRUE;
  }

  return FALSE;
}

//=============================================================================
//  Function        : encrypt
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
#define S3DES           (BYTE)0x01
#define S3DES2          (BYTE)0x02
#define S3TDES          (BYTE)0x03
#define S3TDES2         (BYTE)0x04
static BOOLEAN encrypt(BYTE aMode, BYTE aType, BYTE aIdx, BYTE *aInOut, WORD aLen)
{
  BYTE abKey[16];

  if (keyGet(abKey, aType, aIdx) == FALSE) {
    return FALSE;
  }

  switch (aMode) {
    case S3DES:
      Des(abKey, aInOut, aLen);
      break;
    case S3DES2:
      Des2(abKey, aInOut, aLen);
      break;
    case S3TDES:
      TDes(abKey, aInOut, aLen);
      break;
    case S3TDES2:
      TDes2(abKey, aInOut, aLen);
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

//=============================================================================
//  Function        : reqCheck
//  Description     :
//  Input           :
//  Return          : 0 - request ok
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int reqCheck(BYTE *aReq, WORD aLen)
{
  BYTE buf[16];

  // STX
  if (aReq[OSET_STX] != 0x02) {
    DbFErr("reqCheck(S3):StxErr\n");
    return IF_ERR_FMT;
  }

  // ETX
  if (aReq[aLen-1] != 0x03) {
    DbFErr("reqCheck(S3):EtxErr\n");
    return IF_ERR_FMT;
  }

  // check format id
  if (UGet(aReq+OSET_FMT_ID) != S3_FMT_ID) {
    DbFErr("reqCheck(S3):FmtIdErr\n");
    return IF_ERR_FMT;
  }

  // len check
  if (WGet(aReq+OSET_DATA_LEN)+MIN_MSG_LEN != aLen) {
    DbFErr("reqCheck(S3):LErr(%d,%d)\n", WGet(aReq+OSET_DATA_LEN)+MIN_MSG_LEN, aLen);
    return IF_ERR_FMT;
  }

  // check seq number
  if (WGet(aReq+OSET_SEQ_NUM) == 0) {
    if ((aReq[OSET_INS] & S3INS_CLASS_MASK) != S3INS_CLASS_GENERAL) {
      DbFErr("reqCheck(S3):SeqErr\n");
      return IF_ERR_SEQ;
    }
  }
  else {
    if (WGet(aReq+OSET_SEQ_NUM) < gwSeqNum) {
      DbFErr("reqCheck(S3):SeqErr\n");
      return IF_ERR_SEQ;
    }
  }

  // check ins
  switch (aReq[OSET_INS] & S3INS_CLASS_MASK) {
    case S3INS_CLASS_GENERAL:
      break;
    case S3INS_CLASS_SETUP:
      if ((gbMode & S3MODE_SETUP_ALLOW) == 0) {
        DbFErr("reqCheck(S3):InsErr\n");
        return IF_ERR_INS;
      }
      break;
    case S3INS_CLASS_TRANS:
      if ((gbMode & S3MODE_TRANS_ALLOW) == 0) {
        DbFErr("reqCheck(S3):InsErr\n");
        return IF_ERR_INS;
      }
      break;
    default:
      DbFErr("reqCheck(S3):InsErr\n");
      return IF_ERR_INS;
  }

  // CRC or MAC
  if ( (WGet(aReq+aLen-5) == 0) // use CRC
  && (((aReq[OSET_INS] & S3INS_CLASS_MASK) == S3INS_CLASS_GENERAL)) // !NormalMode or general cmd
  && (WGet(aReq+aLen-3) == Crc16(aReq+1, aLen-6, 0)) ) { // crc ok
      DbStep("reqCheck(S3): CRC OK\n");
  }
  else { // check MAC
    //MAC = TDEA(SKey, Seq(2) | Ins(1) | DataFmt(1) | DataLen(2) | Crc(2))
    memcpy(buf, aReq+OSET_SEQ_NUM, 6);
    WPut(buf+6, Crc16(aReq+1, aLen-6, 0));
    if ((encrypt(S3TDES, ANY_SKEY, 0x00, buf, 8) == FALSE)
    || (memcmp(buf, aReq+aLen-5, 4) != 0)) {
      DbFErr("reqCheck(S3):CrcErr\n");
      return IF_ERR_CRC;
    }
    DbStep("reqCheck(S3): MAC OK\n");
  }

  // decrypt input data
  if (WGet(aReq+OSET_DATA_LEN) > 0) {
    switch (aReq[OSET_DATA_FMT] & 0x0F) {
      case S3DFMT_PLAIN:
        break;
      case S3DFMT_ECB_DES:
        if (((WGet(aReq+OSET_DATA_LEN)%8) != 0)
            || (encrypt(S3DES2, ANY_SKEY, 0x00, aReq+OSET_REQ_DATA, WGet(aReq+OSET_DATA_LEN)) == FALSE)) {
          DbFErr("reqCheck(S3):KeyErr\n");
          return IF_ERR_KEY;
        }
        break;
      case S3DFMT_ECB_TDES:
        if (((WGet(aReq+OSET_DATA_LEN)%8) != 0)
            || (encrypt(S3TDES2, ANY_SKEY, 0x00, aReq+OSET_REQ_DATA, WGet(aReq+OSET_DATA_LEN)) == FALSE)) {
          DbFErr("reqCheck(S3):KeyErr\n");
          return IF_ERR_KEY;
        }
        break;
      default:
        DbFErr("reqCheck(S3):KeyErr\n");
        return IF_ERR_KEY;
    }
  }

  return IF_OK;
}

//=============================================================================
//  Function        : rspEchoData
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspEchoData(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int i, iLen;
  BYTE *pRCode;

  // output data: RCode(1) | Mode(1) | Echo back msg(x)
  iLen = 0;
  pRCode = aRspMsg + OSET_RSP_CODE;
  pRCode[iLen++] = S3RC_OK;
  pRCode[iLen++] = gbMode;

  // echo back message
  i = WGet(aReqMsg + OSET_DATA_LEN);
  if (i > 0) {
    memcpy(pRCode+iLen, aReqMsg+OSET_REQ_DATA, i);
  }
  iLen += i;

  // rsp data len
  WPut(&aRspMsg[OSET_DATA_LEN], iLen);
  iLen += 2;

  return iLen;
}

//=============================================================================
//  Function        : rspInitModeData
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspInitModeData(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int iLen;
  BYTE *pRCode;

  // action
  gbMode = 0x00;
  keyPut(NULL, ANY_SKEY, 0x00);

  // output data: RCode(1)
  iLen = 0;
  pRCode = aRspMsg + OSET_RSP_CODE;
  pRCode[iLen++] = S3RC_OK;

  // rsp data len
  WPut(&aRspMsg[OSET_DATA_LEN], iLen);
  iLen += 2;

  return iLen;
}

//=============================================================================
//  Function        : rspInitAuthData
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspInitAuthData(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int i, iLen;
  BYTE *pRCode, bKType, bKIdx, *pTRand, abRRand[8], buf[16];

  // input: KeyType(1) | KeyIdx(1) | TRand(8)
  i = WGet(aReqMsg + OSET_DATA_LEN);
  if (i < 10) {
    return errRspData(S3RC_ERR_DATA, aRspMsg, aReqMsg, aReqLen);
  }
  bKType = aReqMsg[OSET_REQ_DATA];
  bKIdx = aReqMsg[OSET_REQ_DATA+1];
  pTRand = aReqMsg + OSET_REQ_DATA + 2;

  // generate SKey = TDEA(MRMK/RMK, RRand[5-8] | TRand[1-4] | RRand[1-4] | TRand[5-8])
  Random(abRRand, 8);
  memcpy(buf, abRRand+4, 4);
  memcpy(buf+4, pTRand, 4);
  memcpy(buf+8, abRRand, 4);
  memcpy(buf+12, pTRand+4, 4);
  if ((encrypt(S3TDES, bKType, bKIdx, buf, 16) == FALSE)
      || (keyPut(buf, bKType|SKEY_MASK, bKIdx) == FALSE)) {
    return errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
  }

  // EncRand = TDEA(SKey, TRand[1-8] | RRand[1-8])
  memcpy(buf, pTRand, 8);
  memcpy(buf+8, abRRand, 8);
  if (encrypt(S3TDES, bKType|SKEY_MASK, bKIdx, buf, 16) == FALSE) {
    return errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
  }

  // action
  gbMode = 0x00; // reset mode
  memcpy(gabRRand, abRRand, 8); // save for mutu auth
  memcpy(gabTRand, pTRand, 8);  // save for mutu auth

  // output data: RCode(1) | KeyType(1) | KeyIdx(1) | RRand(8) | EncRand(16)
  iLen = 0;
  pRCode = aRspMsg + OSET_RSP_CODE;
  pRCode[iLen++] = S3RC_OK;

  pRCode[iLen++] = bKType;
  pRCode[iLen++] = bKIdx;
  memcpy(pRCode+iLen, abRRand, 8);
  iLen += 8;
  memcpy(pRCode+iLen, buf, 16);
  iLen += 16;

  // rsp data len
  WPut(&aRspMsg[OSET_DATA_LEN], iLen);
  iLen += 2;

  return iLen;
}

//=============================================================================
//  Function        : rspMutuAuthData
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspMutuAuthData(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int i, iLen;
  BYTE *pRCode, bKType, bKIdx, bMode, *pERand, buf[16];

  // input: KeyType(1) | KeyIdx(1) | EncRand(16) | Mode(1)
  i = WGet(aReqMsg + OSET_DATA_LEN);
  if (i < 19) {
    return errRspData(S3RC_ERR_DATA, aRspMsg, aReqMsg, aReqLen);
  }
  bKType = aReqMsg[OSET_REQ_DATA];
  bKIdx = aReqMsg[OSET_REQ_DATA+1];
  pERand = aReqMsg + OSET_REQ_DATA + 2;
  bMode = aReqMsg[OSET_REQ_DATA+18];

  // EncRand = TDEA(SKey, TRand[5-8], RRand[1-4], TRand[1-4], RRand[5-8])
  memcpy(buf, gabTRand+4, 4);
  memcpy(buf+4, gabRRand, 4);
  memcpy(buf+8, gabTRand, 4);
  memcpy(buf+12, gabRRand+4, 4);
  if (encrypt(S3TDES, bKType|SKEY_MASK, bKIdx, buf, 16) == FALSE) {
    return errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
  }
  if (memcmp(pERand, buf, 16) != 0) { //EncRand not match
    return errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
  }

  // action
  gbMode = bMode;
  gwSeqNum = 1;
  memset(gabTRand, 0x00, 8);
  memset(gabRRand, 0x00, 8);

  // output data: RCode(1)
  iLen = 0;
  pRCode = aRspMsg + OSET_RSP_CODE;
  pRCode[iLen++] = S3RC_OK;

  // rsp data len
  WPut(&aRspMsg[OSET_DATA_LEN], iLen);
  iLen += 2;

  return iLen;
}

//=============================================================================
//  Function        : rspGenKeyData
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspGenKeyData(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int i, iLen;
  BYTE *pRCode, bKType, bKIdx, *pEKey, *pKvc, buf[16], abKvc[8];

  // input: KeyType(1) | KeyIdx(1) | EncKey(16) | KVC(4)
  i = WGet(aReqMsg + OSET_DATA_LEN);
  if (i < 22) {
    return errRspData(S3RC_ERR_DATA, aRspMsg, aReqMsg, aReqLen);
  }
  bKType = aReqMsg[OSET_REQ_DATA];
  bKIdx = aReqMsg[OSET_REQ_DATA+1];
  pEKey = aReqMsg + OSET_REQ_DATA + 2;
  pKvc = aReqMsg + OSET_REQ_DATA + 18;

  // MRMK/RMK = TDEA2(SKey, EncKey)
  memcpy(buf, pEKey, 16);
  if (encrypt(S3TDES2, bKType|SKEY_MASK, bKIdx, buf, 16) == FALSE) {
    return errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
  }

  // KVC check
  memset(abKvc, 0x00, 8);
  TDes(buf, abKvc, 8);
  if (memcmp(pKvc, abKvc, 4) != 0) { // KVC not match
    return errRspData(S3RC_ERR_KVC, aRspMsg, aReqMsg, aReqLen);
  }

  // action
  keyPut(buf, bKType, bKIdx);

  // output data: RCode(1)
  iLen = 0;
  pRCode = aRspMsg + OSET_RSP_CODE;
  pRCode[iLen++] = S3RC_OK;

  // rsp data len
  WPut(&aRspMsg[OSET_DATA_LEN], iLen);
  iLen += 2;

  return iLen;
}

//=============================================================================
//  Function        : setFullEmvData
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int setFullEmvData(BYTE aRCode, DWORD aEId, BYTE *aRspMsg, sIO_t *asOut)
{
  int iLen;
  BYTE *pRCode;

  //=> output init
  // output data: RCode(1) | EId(4) | Len(2) | Data(x)
  //DbOut("RC[%02X] EId[%02X] Len(%d)\n", aRCode, aEId, asOut->wLen);

  iLen = 0;
  pRCode = aRspMsg+OSET_RSP_CODE;
  pRCode[iLen++] = aRCode;
  UPut(pRCode+iLen, aEId);
  iLen += 4;
  WPut(pRCode+iLen, asOut->wLen);
  iLen += 2;
  memcpy(pRCode+iLen, asOut->bMsg, asOut->wLen);
  iLen += asOut->wLen;

  // rsp data len
  WPut(aRspMsg+OSET_DATA_LEN, iLen);
  iLen += 2;
  return iLen;
}
//*****************************************************************************
//  Function        : packFullEmvData
//  Description     : Pack response
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void packFullEmvData(sIO_t *asOut)
{
  const WORD *pw;
  BYTE *pb, ab_tmp[32], ab_totresp[64], *pb_tmp;
  WORD w_rspcode, w_tmp;

  asOut->wLen = 0;
  if (gGDS->s_ecr_data.b_cmd >= MAX_ECREDC_CMD)
    return;

  w_rspcode = 'L'*256+'C';
  if (INPUT.b_entry_mode == ICC)
    memcpy(asOut->bMsg, gGDS->s_EMVOut.bMsg, 2);
  else if (INPUT.b_entry_mode == CONTACTLESS)
    memcpy(asOut->bMsg, gGDS->s_CTLOut.pbMsg, 2);
  else
    memcpy(asOut->bMsg, "\x00\x01", 2);           // default signature for swipe/manual
  asOut->wLen = 2;
  
  if (RSP_DATA.b_response == TRANS_ACP)
    asOut->bMsg[1] |= ACT_APPROVED;
  pb = asOut->bMsg + 2;
  pw = KItfRespTbl[gGDS->s_ecr_data.b_cmd];
  while (*pw != 0x00) {
    switch (*pw) {
      case ITFTAG_PAN:
        split(ab_tmp, INPUT.sb_pan, 10);
        w_tmp = fndb(&ab_tmp[1], 'F', 19);
        w_tmp = (w_tmp/2) + (w_tmp%2);
        pb += TlvPack(*pw, w_tmp, INPUT.sb_pan, pb);
        break;
      case ITFTAG_TXNAMT:
        DDWORD2bcd(ab_tmp, INPUT.dd_amount);
        pb += TlvPack(*pw, 6, &ab_tmp[4], pb);
        break;
      case ITFTAG_TRACE:
        pb += TlvPack(*pw, 3, INPUT.sb_trace_no, pb);
        break;
      case ITFTAG_TXNTIME:
        pb += TlvPack(*pw, 3, &RSP_DATA.s_dtg.b_hour, pb);
        break;
      case ITFTAG_TXNDATE:
        pb += TlvPack(*pw, 2, &RSP_DATA.s_dtg.b_month, pb);
        break;
      case ITFTAG_EXPDATE:
        pb += TlvPack(*pw, 2, INPUT.sb_exp_date, pb);
        break;
      case ITFTAG_RRN:
        pb += TlvPack(*pw, 12, RSP_DATA.sb_rrn, pb);
        break;
      case ITFTAG_AUTH_RESP:
        pb += TlvPack(*pw, 6, RSP_DATA.sb_auth_code, pb);
        break;
      case ITFTAG_RESP_CODE:
        if ((RSP_DATA.w_rspcode == 'A'*256+'P') || (RSP_DATA.w_rspcode == 'T'*256+'A') || (RSP_DATA.w_rspcode == 'V'*256+'S'))
          w_rspcode = '0'*256+'0';
        else
          w_rspcode = RSP_DATA.w_rspcode;
        ab_tmp[0] = (BYTE)((w_rspcode>>8)&0xFF);
        ab_tmp[1] = (BYTE)(w_rspcode&0xFF);
        pb += TlvPack(*pw, 2, ab_tmp, pb);
        break;
      case ITFTAG_TERMID :
        pb += TlvPack(*pw, 8, STIS_ACQ_TBL(0).sb_term_id, pb);
        break;
      case ITFTAG_MERCHID:
        pb += TlvPack(*pw, 15, STIS_ACQ_TBL(0).sb_acceptor_id, pb);
        break;
      case ITFTAG_CARDHOLDERNAME :
        pb += TlvPack(*pw, 26, INPUT.sb_holder_name, pb);
        break;
      case ITFTAG_ADDAMT :
        DDWORD2bcd(ab_tmp, INPUT.dd_tip);
        pb += TlvPack(*pw, 6, &ab_tmp[4], pb);
        break;
      case ITFTAG_BATCHNO:
        pb += TlvPack(*pw, 3, STIS_ACQ_TBL(0).sb_curr_batch_no, pb);
        break;
      case ITFTAG_ECRREF:
        pb += TlvPack(*pw, 16, INPUT.sb_ecr_ref, pb);
        break;
      case ITFTAG_HOSTNAME:
        pb += TlvPack(*pw, 10, STIS_ACQ_TBL(0).sb_name, pb);
        break;
      case ITFTAG_HOSTTOT:
        pb_tmp = ab_totresp;
        DDWORD2bcd(ab_tmp, gTermTotal.s_sale_tot.w_count);
        memcpy(pb_tmp, &ab_tmp[8], 2);
        pb_tmp += 2;
        DDWORD2bcd(ab_tmp, gTermTotal.s_sale_tot.dd_amount);
        memcpy(pb_tmp, &ab_tmp[4], 6);
        pb_tmp += 6;
        DDWORD2bcd(ab_tmp, gTermTotal.s_refund_tot.w_count);
        memcpy(pb_tmp, &ab_tmp[8], 2);
        pb_tmp += 2;
        DDWORD2bcd(ab_tmp, gTermTotal.s_refund_tot.dd_amount);
        memcpy(pb_tmp, &ab_tmp[4], 6);
        pb_tmp += 6;
        pb += TlvPack(*pw, 16, ab_totresp, pb);
        break;
      case ITFTAG_ENTRYMODE:
        switch (INPUT.b_entry_mode) {
          case MANUAL:
          case MANUAL_4DBC:
            ab_tmp[0] = 0x01;
            break;
          case SWIPE:
          case SWIPE_4DBC:
          case FALLBACK_4DBC:
          case FALLBACK:
            ab_tmp[0] = 0x02;
            break;
          default :
            ab_tmp[0] = 0x05;
            break;
        }
        pb += TlvPack(*pw, 1, ab_tmp, pb);
        break;
      case ITFTAG_TXNTYPE:
        pb += TlvPack(*pw, 1, &gGDS->s_ecr_data.b_cmd, pb);
        break;
      case ITFTAG_AID:
        pb += TlvPack(*pw, INPUT.s_icc_data.b_aid_len, INPUT.s_icc_data.sb_aid, pb);
        break;
    }
    pw++;
  }
  asOut->wLen = (DWORD)pb - (DWORD)asOut->bMsg;
}
//*****************************************************************************
//  Function        : ItfVoid
//  Description     : Process Void request from ECR.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ItfVoid(void)
{
  VoidTrans(-1);
  return TXN_OK;
}
//*****************************************************************************
//  Function        : ItfHostTot
//  Description     : Process Host Total request from ECR.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ItfHostTot(void)
{
  CalTotals(FALSE);
  memcpy(&gTermTotal, &TERM_TOT, sizeof(struct TOTAL_STRUCT));
  return TXN_OK;
}
//*****************************************************************************
//  Function        : ItfSettle
//  Description     : Process Settlement request from ECR.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ItfSettle(void)
{
  CalTotals(FALSE);
  memcpy(&gTermTotal, &TERM_TOT, sizeof(struct TOTAL_STRUCT));    // backup total in batch
  RSP_DATA.w_rspcode = 'D'*256 + 'E';
  INPUT.w_host_idx = 0;
  if (SettleTrans() == TRUE)
    return TXN_OK;
  return TXN_ERR;
}
//=============================================================================
//  Function        : rspPackData
//  Description     : pack response data
//  Input           :
//  Return          : >=0 - total data len included len(2) and rsp code(1) field
//                    < 0 - error status
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspPackData(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  sIO_t sOut;
  int iLen;
  BYTE *pb, *pbTlv;
  DWORD ret=TXN_ERR;

  sOut.bMsg = gRspBuf;
  // instruction
  switch (aReqMsg[OSET_INS]) {
      // general ins
    case S3INS_ECHO:
      iLen = rspEchoData(aRspMsg, aReqMsg, aReqLen);
      break;
    case S3INS_INIT_MODE:
      iLen = rspInitModeData(aRspMsg, aReqMsg, aReqLen);
      break;
    case S3INS_INIT_AUTH:
      iLen = rspInitAuthData(aRspMsg, aReqMsg, aReqLen);
      break;
    case S3INS_MUTU_AUTH:
      iLen = rspMutuAuthData(aRspMsg, aReqMsg, aReqLen);
      break;
      // setup ins
    case S3INS_GEN_KEY:
      iLen = rspGenKeyData(aRspMsg, aReqMsg, aReqLen);
      break;
      // transaction ins
    case S3INS_TRANS:
    case S3INS_FULL_EMV:  
      ClearResponse();
      LongBeep();
      // convert to existing ECR interface
      gGDS->s_ecr_data.b_cla = ECR_CLA_EDC;
      memcpy(gGDS->s_ecr_data.sb_content, "\xDF\x04\x06", 3);             // ECR Amount
      memcpy(&gGDS->s_ecr_data.sb_content[3], aReqMsg+OSET_REQ_DATA, 6);  // Copy from Request
      pbTlv = TlvSeek(aReqMsg+OSET_REQ_DATA+6, aReqLen-OSET_REQ_DATA-6, ITFTAG_TXNTYPE);
      if (pbTlv != NULL) {
        pb = TlvVPtr(pbTlv);
        gGDS->s_ecr_data.b_cmd = *pb;
      }
      else
        gGDS->s_ecr_data.b_cmd = ECR_EDC_SALE;
      gGDS->i_ecr_len = 11;
      memset(&gTermTotal, 0, sizeof(struct TOTAL_STRUCT));
      switch (gGDS->s_ecr_data.b_cmd) {
        case ECR_EDC_SALE:
          ret = WaitCardTrans(SALE_SWIPE);
          break;
        case ECR_EDC_OFFLSALE:
          ret = WaitCardTrans(SALE_OFFLINE);
          break;
        case ECR_EDC_REFUND:
          ret = WaitCardTrans(REFUND);
          break;
        case ECR_EDC_VOID:
          pbTlv = TlvSeek(aReqMsg+OSET_REQ_DATA+6, aReqLen-OSET_REQ_DATA-6, ITFTAG_TRACE);
          if (pbTlv != NULL)
            memcpy(INPUT.sb_roc_no, TlvVPtr(pbTlv), TlvLen(pbTlv));
          ret = ItfVoid();
          break;
        case ECR_EDC_HOSTTOTAL:
          ret = ItfHostTot();
          break;
        case ECR_EDC_AUTH:
          ret = WaitCardTrans(AUTH_SWIPE);
          break;
        case ECR_EDC_SETTLE:
          ret = ItfSettle();
          break;
      }
      if (ret != TXN_CANCEL) {
        packFullEmvData(&sOut);
        iLen = setFullEmvData(S3RC_OK, 0, aRspMsg, &sOut);
      }
      else
        iLen = errRspData(S3RC_CANCEL, aRspMsg, aReqMsg, aReqLen);
      gGDS->i_ecr_len = 0;
      ResetTerm();
      break;
    //// !2011-09--
    default: // unknown
      iLen = errRspData(S3RC_ERR_INS, aRspMsg, aReqMsg, aReqLen);
      break;
  }

  return iLen;
}

//=============================================================================
//  Function        : RspFmtPack
//  Description     : pack response format
//  Input           :
//  Return          : >= 0 - total rsp len
//                    <0   - error status
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
static int rspPackFmt(BYTE *aRspMsg, BYTE *aReqMsg, WORD aReqLen)
{
  int i, iLen;
  BYTE buf[16];

  // stx
  iLen = 0;
  aRspMsg[iLen++] = 0x02;

  // format id
  UPut(aRspMsg+iLen, S3_FMT_ID);
  iLen += 4;

  // sender & receiver
  aRspMsg[iLen++] = aReqMsg[OSET_RX_ID];
  aRspMsg[iLen++] = aReqMsg[OSET_TX_ID];

  // seq num
  i = WGet(aReqMsg+OSET_SEQ_NUM);
  if (i == 0) {
    WPut(aRspMsg+iLen, gwSeqNum-1);
  }
  else if (i == 0xFFFF) {
    gbMode = 0x00;
    gwSeqNum = i;
    WPut(aRspMsg+iLen, i);
  }
  else {
    gwSeqNum = i + 1;
    WPut(aRspMsg+iLen, gwSeqNum-1);
  }
  iLen += 2;

  // ins
  aRspMsg[iLen++] = aReqMsg[OSET_INS];

  // data format
  aRspMsg[iLen++] = aReqMsg[OSET_DATA_FMT]>>4;

  // rsp data len(2) + rsp code(1) + rsp data(x)
  i = WGet(aRspMsg+OSET_DATA_LEN);
  if (i > 1) {
    switch (aRspMsg[OSET_DATA_FMT] & 0x0F) {
      case S3DFMT_PLAIN:
        break;
      case S3DFMT_ECB_DES:
        i = i - 1; // ignore rsp code
        memset(aRspMsg+OSET_RSP_DATA+i, 0x00, 8);
        i = ((i+7)/8)*8; // rsp data len
        if (encrypt(S3DES, ANY_SKEY, 0x00, aRspMsg+OSET_RSP_DATA, i) == FALSE) {
          errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
          i = 1;
          break;
        }
        i ++; // include rsp code
        WPut(aRspMsg+OSET_DATA_LEN, i);
        break;
      case S3DFMT_ECB_TDES:
        i = i - 1; // ignore rsp code
        memset(aRspMsg+OSET_RSP_DATA+i, 0x00, 8);
        i = ((i+7)/8)*8; // rsp data len
        if (encrypt(S3TDES, ANY_SKEY, 0x00, aRspMsg+OSET_RSP_DATA, i) == FALSE) {
          errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
          i = 1;
          break;
        }
        i ++; // include rsp code
        WPut(aRspMsg+OSET_DATA_LEN, i);
        break;
      default:
        errRspData(S3RC_ERR_KEY, aRspMsg, aReqMsg, aReqLen);
        i = 1;
        break;
    }
  }
  iLen += i + 2;

  // crc or mac
  UPut(aRspMsg+iLen ,Crc16(aRspMsg+1, iLen-1, 0)); // put crc
  //MAC = TDEA(SKey, Seq(2) | Ins(1) | DataFmt(1) | DataLen(2) | Crc(2))
  if (aRspMsg[OSET_INS] != S3INS_INIT_AUTH) { // use CRC if init auth
    memcpy(buf, aRspMsg+OSET_SEQ_NUM, 6);
    memcpy(buf+6, aRspMsg+iLen+2, 2);
    if (encrypt(S3TDES, ANY_SKEY, 0x00, buf, 8) == TRUE) { // MAC ok
      memcpy(aRspMsg+iLen, buf, 4);
    }
  }
  iLen += 4;

  // etx
  aRspMsg[iLen++] = 0x03;
  return iLen;
}

//=============================================================================
//  Function        : Spt3MsgHdr
//  Description     : Spectra message interface handler
//  Input           :
//  Return          : <0 error
//                    >0 response message len
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
int Spt3MsgHdr(BYTE *aInOutMsg, WORD aInLen)
{
  int iRet;
  BYTE *pmReq;

  // init
  pmReq = gReqBuf;
  memcpy(pmReq, aInOutMsg, aInLen);

  // check input format
  iRet = reqCheck(pmReq, aInLen);
  if (iRet < 0) { // if error
    switch (iRet) {
      case IF_ERR_FMT: // format error -> return to try other interface
        return iRet;
      case IF_ERR_CRC:
        iRet = errRspData(S3RC_ERR_CSUM, aInOutMsg, pmReq, aInLen);
        break;
      case IF_ERR_SEQ:
        iRet = errRspData(S3RC_ERR_SEQ, aInOutMsg, pmReq, aInLen);
        break;
      case IF_ERR_INS:
        iRet = errRspData(S3RC_ERR_INS, aInOutMsg, pmReq, aInLen);
        break;
      case IF_ERR_KEY:
        iRet = errRspData(S3RC_ERR_KEY, aInOutMsg, pmReq, aInLen);
        break;
      default:
        iRet = errRspData(S3RC_ERR_FMT, aInOutMsg, pmReq, aInLen);
        break;
    }
    if (iRet < 0) {
      return iRet;
    }
    iRet = rspPackFmt(aInOutMsg, pmReq, aInLen);
    return iRet;
  }

  // response pack
  iRet = rspPackData(aInOutMsg, pmReq, aInLen);
  if (iRet < 0) {
    return iRet;
  }
  iRet = rspPackFmt(aInOutMsg, pmReq, aInLen);
  return iRet;
}

//=============================================================================
//  Function        : IfaceInit
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed :
//  * PC1506 add force init input
//=============================================================================
BOOLEAN IfaceInit(BOOLEAN aForceInit)
{
  //return APM_IFACE_Open(aForceInit);
  return TRUE;
}

//=============================================================================
//  Function        : IfaceClose
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
BOOLEAN IfaceClose(void)
{
  //return APM_IFACE_Close();
  return TRUE;
}

/******************************************************************************
 *  Function        : debugAuxIface
 *  Description     : Debug Iface Tx/Rx to COM1.
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void debugAuxIface(BYTE aIn, BYTE *pData, DWORD aLen)
{
  BYTE buf[20];
  BYTE *txbuf;
  DWORD free_run;
  
  txbuf = MallocMW(aLen*2+20);
  if (txbuf == NULL)
    return;
  
  set_pptr(txbuf, aLen*2+20);
  memset(buf, 0, sizeof(buf));
  if (aIn)
    sprintf(buf, "Iface-In (%d):", aLen);
  else
    sprintf(buf, "Iface-Out(%d):", aLen);
  pack_mem(buf, strlen(buf));
  split_data(pData, aLen);
  pack_mem("\r\n", 2);

  if (!AuxOpen()) {
    DispLineMW("AuxOpen Err!", MW_LINE5, MW_CENTER|MW_CLRDISP|MW_BIGFONT);
    APM_WaitKey(9000, 0);
    return;
  }

  free_run = FreeRunMW();
  while (FreeRunMW() < free_run + 200*2) {   // 2 sec timeout for send
    if (AuxTxRdy()) {
      AuxWrite(txbuf, get_distance());
      break;
    }
    SleepMW();
  }
  AuxClose();
  FreeMW(txbuf);
}
//=============================================================================
//  Function        : IfaceTx
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
int IfaceTx(BYTE *aData, WORD aLen)
{
  BYTE buf[MAX_IFACE_BUF];
  
  memcpy(buf, aData, aLen);
  //debugAuxIface(0, aData, aLen);
  return ECR_Send(buf, aLen);
}

//=============================================================================
//  Function        : IfaceRx
//  Description     :
//  Input           :
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
int IfaceRx(BYTE *aData, WORD aMaxLen)
{
  BYTE buf[MAX_IFACE_BUF];
  int ret;
  
  if (!ECR_RxRdy())
    return 0;
    
  ret = ECR_Rcvd(buf, aMaxLen);
  if (ret > 0) {
    memcpy(aData, buf, ret);
    //debugAuxIface(1, aData, ret);
  }
  return ret;
}

//=============================================================================
//  Function        : IfaceProcess
//  Description     : interface process
//  Input           :
//  Return          : >=0 - length of response sent
//                    <0  - error status
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
int IfaceProcess(void)
{
  int iLen;

  iLen = IfaceRx(gIfBuf, MAX_IFACE_BUF);

  if (iLen <= 0) {
    return 0;
  }

  ECR_Clear();
  ClearResponse();
  
  // pack response
  iLen = Spt3MsgHdr(gIfBuf, iLen);
  if (iLen < 0) {
    gIdleCnt = FreeRunMark();             //25-07-17 JC ++
    return iLen;
  }

  // send response
  if (IfaceTx(gIfBuf, iLen) <= 0) {
    gIdleCnt = FreeRunMark();             //25-07-17 JC ++
    return -1;
  }
  gIdleCnt = FreeRunMark();               //25-07-17 JC ++
  return iLen;
}

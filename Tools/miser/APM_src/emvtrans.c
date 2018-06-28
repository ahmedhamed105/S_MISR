//-----------------------------------------------------------------------------
//  File          : EMVTrans.c
//  Module        :
//  Description   : EMV Transactions Process.
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
#include "midware.h"
#include "util.h"
#include "hardware.h"
#include "emv2dll.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "infodata.h"
#include "lptutil.h"
#include "emvtrans.h"

//-----------------------------------------------------------------------------
//      Common Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Constant Message
//-----------------------------------------------------------------------------
const BYTE KPinMsg[4][16] = {     /* PIN related message */
  " 请 输 入 密 码 ",                   /*   1. Enter PIN      */
  " 密  码  正  确 ",                   /*   2. PIN OK         */
  " 密  码  错  误 ",                   /*   3. Incorrect PIN  */
  "  最后一次密码  "                    /*   4. Last PIN Try   */
};
const BYTE KCardholderIDName[6][11] = {
  "身份证    ",
  "军官证    ",
  "护照      ",
  "入境证    ",
  "临时身份证",
  "其他      ",
};

//-----------------------------------------------------------------------------
//    Global Variables
//-----------------------------------------------------------------------------
static BYTE    *sbIOBuf;
static sIO_t   *sEMVIn;
static sIO_t   *sEMVOut;
static BYTE   bEmvLoaded;
static BYTE   bMaxNumBlk, bDnloadBlk;
static WORD   wChunkSize;
static int    iDnloadFile = -1;

static const BYTE KEMVTemp[] = {"emv_tmp"};


//*****************************************************************************
//  Function        : TagSize
//  Description     : Retrieve Tag Size from input buffer.
//  Input           : aBuf;     // Data buffer
//  Return          : Tag Size;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE TagSize(BYTE *aBuf)
{
  return ((*aBuf & 0x1F) == 0x1F) ? 2 : 1;
}
//*****************************************************************************
//  Function        : TagGet
//  Description     : Retrieve Tag value from input buffer.
//  Input           : aBuf;     // Data buffer
//  Return          : Tag Size/Orignal Tag Value
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD TagGet(BYTE *aBuf)
{
  if ((*aBuf & 0x1F) != 0x1F)
    return *aBuf;
  return WGet(aBuf);
}
//*****************************************************************************
//  Function        : TLSize
//  Description     : Get L size
//  Input           : *aLD;   // LD pointer
//  Return          : L Size
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE TLSize(BYTE *aLD)
{
  //bit 8 is 1, Lenght size if bit 7 to bit 1 plus one
  if (*aLD & 0x80)
    return (*aLD & 0x7F) + 1;
  return 1;
}
//*****************************************************************************
//  Function        : TLGet
//  Description     : Get L value
//  Input           : *aLD;   // LD pointer
//  Return          : Length value
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD TLGet(BYTE *aLD)
{
  //if bit 8 = 0, one byte length field
  if ((*aLD & 0x80) == 0)
    return *aLD;

  //Otherwise, no. of lenght field depends on value of bit 7 to bit 1
  if (*aLD == 0x81)
    return *(aLD + 1);

  if (*aLD == 0x82)
    return WGet(aLD + 1);

  return 0xFFFF;
}
//*****************************************************************************
//  Function        : LDLoad
//  Description     : Retrieve Tag value from TLD buffer (record) to abKbd
//  Input           : aBuf;     // Data buffer
//                    aTag;     // require tag Value
//  Return          : TRUE/FALSE;
//  Note            : Data length in record < 128 for each tag value
//  Globals Changed : N/A
//*****************************************************************************
#define MAX_OUT_SIZE      50
BOOLEAN LDLoad(BYTE *aDest, BYTE *aSrc, WORD aTag)
{
  WORD wNew;
  BYTE bLen;
  BYTE bCnt;

  bCnt = 100;                           /* safety limit */
  while ((wNew = TagGet(aSrc)) != 0 && bCnt-- != 0) {
    aSrc += TagSize(aSrc);
    bLen = *aSrc;
    if (aTag == wNew) {
      if (bLen >= MAX_OUT_SIZE)
        break;
      memcpy(aDest, aSrc, bLen + 1);
      return TRUE;
    }
    aSrc += bLen + 1;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : ConvDecimal
//  Description     : Convert the value in aBuf to decimal and insert
//                    decimal point.
//  Input           : aBuf;         // Input Buffer.
//                    var_i;        // Len of result
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ConvDecimal(BYTE *aBuf, BYTE aLen)
{
  BYTE buffer[13];
  WORD version;
  DWORD tmp, lead0len;

  if (aLen > 5){
    tmp = DWGet(&aBuf[1]);
    lbin2asc(buffer, tmp);
  }
  else {
    version = WGet(&aBuf[1]);
    bin2dec(version, buffer, 5);
  }

  if ((lead0len = skpb(buffer, '0', aLen)) == aLen)   /* leading zero count */
    lead0len = aLen;    /* max leading zeros to ignore */

  aBuf[0] = (BYTE)(aLen+1-lead0len);   /* excluding decimal digits */
  memcpy(&aBuf[1], &buffer[lead0len], aBuf[0]);
  buffer[aLen] = '.';
  memcpy(&aBuf[aBuf[0]+1], &buffer[aLen], 3);
  aBuf[0] += 3;

}
//*****************************************************************************
//  Function        : KernelReady
//  Description     : Check Whether EMV Kernel exist
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN KernelReady(void)
{
  if (!IsAppIdExistMW(EMVDLL_ID))
    return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : EMVFatal
//  Description     : EMV Fatal Error.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void EMVFatal(void)
{
  DispPutStr("\f\nEMV2 FATAL ERR\nPLS CALL SERVICE");
  while (WaitKey(KBD_TIMEOUT) != MWKEY_CANCL);
  ResetMW();
}
//*****************************************************************************
//  Function        : ClrEMVKey
//  Description     : Clear All EMV Keys.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClrEMVKey(void)
{
  if (!KernelReady())
    return;

  sEMVIn->bMsg = sEMVOut->bMsg = sbIOBuf;
  memset(sEMVIn->bMsg, 0, 6);
  sEMVIn->wLen = 6;
  if (EMV_DllSetup(IOCMD_KEY_LOADING,sEMVIn,sEMVOut) == FALSE)     // clear all key
    EMVFatal();
}
//*****************************************************************************
//  Function        : ClrEMVParam
//  Description     : Clear all EMV dll's parameters.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClrEMVParam(void)
{
  if (!KernelReady())
    return;

  sEMVIn->bMsg = sEMVOut->bMsg = sbIOBuf;

  GetSysCfgMW(K_CF_SerialNo, sEMVIn->bMsg);
  sEMVIn->wLen = 8;

  if (EMV_DllSetup(IOCMD_IFDSN_RW,sEMVIn,sEMVOut) == FALSE)     // set IFDSN
    EMVFatal();

  EMV_DllSetup(IOCMD_CLR_AID,sEMVIn,sEMVOut);              // clear AID parameters
}
//*****************************************************************************
//  Function        : SetEMVLoaded
//  Description     : set emv loaded flag
//  Input           : aFlag
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetEMVLoaded(BYTE aFlag)
{
  bEmvLoaded |= aFlag;
}
//*****************************************************************************
//  Function        : EMVClrDnloadBuf
//  Description     : Prepare download buffer for EMV data download.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EMVClrDnloadBuf (void)
{
  BYTE filename[32];
  if (sizeof(APP_CFG) < sizeof(KEY_ROOM))
    wChunkSize = sizeof(KEY_ROOM)+3;  // 3B for type and len
  else
    wChunkSize = sizeof(APP_CFG)+3;   // 3B for type and len
  //bMaxNumBlk = sizeof(sbIOBuf)/wChunkSize;          //29-09-16 JC ++
  bMaxNumBlk = 1500/wChunkSize;
  bDnloadBlk = bEmvLoaded = 0;

  if (iDnloadFile >= 0) {
    fCloseMW(iDnloadFile);
    iDnloadFile = -1;
  }

  strcpy(filename, KEMVTemp);
  fDeleteMW(filename);
  iDnloadFile = fCreateMW(filename, 0);
}
//*****************************************************************************
//  Function        : EMVKillDnloadBuf
//  Description     : Kill EMV data download buffer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EMVKillDnloadBuf (void)
{
  BYTE filename[32];
  strcpy(filename, KEMVTemp);
  if (iDnloadFile >= 0) {
    fCloseMW(iDnloadFile);
    iDnloadFile = -1;
  }
  fDeleteMW(filename);
}
//*****************************************************************************
//  Function        : EMVData2DnloadBuf
//  Description     : Store EMV param/key to download buffer in file system.
//  Input           : aType;        // EMV param or key
//                    aData;        // pointer to EMV param or key
//                    aLen;         // length of EMV param or key
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EMVData2DnloadBuf (BYTE aType, BYTE *aData, WORD aLen)
{
  WORD offset;

  offset = wChunkSize*bDnloadBlk;
  sbIOBuf[offset] = aType;
  memcpy(&sbIOBuf[offset+1], &aLen, 2);
  memcpy(&sbIOBuf[offset+3], aData, aLen);

  if (++bDnloadBlk >= bMaxNumBlk) {
    if (fWriteMW(iDnloadFile, sbIOBuf, wChunkSize*bDnloadBlk) != (wChunkSize*bDnloadBlk))
      bEmvLoaded = 0xFF;  // indicate fail
    bDnloadBlk = 0;
  }
}
//*****************************************************************************
//  Function        : EMVDataUpdate
//  Description     : Clear/Store param/key to EMVDLL.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void EMVDataUpdate (void)
{
  int size, len;
  BYTE type;

  if (!KernelReady() || (bEmvLoaded == 0xFF) || (bEmvLoaded == 0x00))
    return;

  if ((bDnloadBlk != 0) && (bDnloadBlk < bMaxNumBlk)) { // store remaining data chunk
    if (fWriteMW(iDnloadFile, sbIOBuf, wChunkSize*bDnloadBlk) != (wChunkSize*bDnloadBlk))
      return;
  }

  if (bEmvLoaded & 0x80)
    ClrEMVParam();
  if (bEmvLoaded & 0x40)
    ClrEMVKey();

  //fFlushMW(iDnloadFile);
  size = fLengthMW(iDnloadFile);
  fSeekMW(iDnloadFile, 0);
  len = wChunkSize-1;
  while (size > 0) {
    fReadMW(iDnloadFile, &type, 1);
    fReadMW(iDnloadFile, &sEMVIn->wLen, 2);
    if (fReadMW(iDnloadFile, sEMVIn->bMsg, len-2) == (len-2)) {
      if (EMV_DllSetup(type, sEMVIn, sEMVOut)) {
        size -= (len+1);
        continue;
      }
    }
    bEmvLoaded |= 0x0F;   // indicate fail
    break;
  }
  if ((bEmvLoaded & 0x0F) == 0x01) {
    if (EMV_DllSetup(IOCMD_CLOSE_LOAD, sEMVIn, sEMVOut) == FALSE) // close AID loading
      bEmvLoaded |= 0x0F; // indicate fail
  }
  if ((bEmvLoaded & 0x0F) == 0x0F) {
    //if (bEmvLoaded & 0x80)
    //  ClrAllParam();
    if (bEmvLoaded & 0x40)
      ClrEMVKey();
    EMVKillDnloadBuf();
    EMVFatal();
  }
}
//*****************************************************************************
//  Function        : GetAppCfg
//  Description     : Retrieve Configuration from EMV KERNEL.
//  Input           : aCfg;     // pointer to buffer
//                    aIdx;     // config index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN GetEMVAppCfg(APP_CFG *aCfg, DWORD aIdx)
{
  sEMVIn->bMsg = sEMVOut->bMsg = sbIOBuf;
  sEMVIn->bMsg[0] = (BYTE) aIdx;
  sEMVIn->wLen = 1;
  if (EMV_DllSetup(IOCMD_READ_AIDIDX,sEMVIn,sEMVOut) == FALSE)     // load APP parameters
    return FALSE;
  memcpy(aCfg, sEMVOut->bMsg, sizeof(APP_CFG));
  return TRUE;
}
//*****************************************************************************
//  Function        : GetEMVKeyInfo
//  Description     : Retrieve Key info. from EMV KERNEL.
//  Input           : aKeyInfo;     // pointer to buffer
//                    aIdx;         // config index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetEMVKeyInfo(KEY_INFO *aKeyInfo, DWORD aIdx)
{
  sEMVIn->bMsg = sEMVOut->bMsg = sbIOBuf;
  sEMVIn->bMsg[0] = (BYTE) aIdx|0x80;
  sEMVIn->wLen = 1;

  if ((EMV_DllSetup(IOCMD_READ_KEY,sEMVIn,sEMVOut) == FALSE) || (sEMVOut->wLen == 0))     // Read Key Info
    return FALSE;

  memcpy(aKeyInfo, sEMVOut->bMsg, sizeof(KEY_INFO));
  return TRUE;
}
//*****************************************************************************
//  Function        : PrintEMVParam
//  Description     : print or display EMV parameter report.
//  Input           : aEmvCfg
//                    aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintEMVAppCfg(APP_CFG aEmvCfg, BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, buf[64], data[64];
  WORD len, pack_width;
  struct INFO_DATA info_data;

  // allocate large memory
  if ((p_mem = MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // AID
  if (LDLoad(buf, aEmvCfg.eACFG, 0x9F06)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "AID: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // ACQ
  strcpy(p_mem+len, "Def ACQ Profile: ");
  data[0] = (!(aEmvCfg.eBitField&eMULTI_ACQ))? 'Y' : 'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // type & ver
  SprintfMW(p_mem+len, "Type: %02X ", aEmvCfg.eType);
  memset(data, 0x00, sizeof(data));
  if (LDLoad(buf, aEmvCfg.eACFG, 0x9F09)) {
    strcpy(data, "Ver: ");
    split(data+strlen(data), buf+1, buf[0]);
  }
  len += PackInfoData(p_mem+len, data, pack_width);

  // floor limit
  if (LDLoad(buf, aEmvCfg.eACFG, 0x9F1B)) {
    memset(data, 0x00, sizeof(data));
    ConvDecimal(buf, 7);
    memcpy(data, buf+1, buf[0]);
    strcpy(p_mem+len, "FLR LMT: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // oversea floor limit
  if (LDLoad(buf, aEmvCfg.eACFG, 0xDF7F)) {
    memset(data, 0x00, sizeof(data));
    ConvDecimal(buf, 7);
    memcpy(data, buf+1, buf[0]);
    strcpy(p_mem+len, "FLR LMT(O): ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // basic random
  strcpy(p_mem+len, "Basic Random: ");
  SprintfMW(data, "%0d.%02d", aEmvCfg.eRSBThresh/100, aEmvCfg.eRSBThresh%100);
  len += PackInfoData(p_mem+len, data, pack_width);

  // target & max target
  strcpy(p_mem+len, "Target %: ");
  SprintfMW(data, "%02d-%02d", aEmvCfg.eRSTarget, aEmvCfg.eRSBMax);
  len += PackInfoData(p_mem+len, data, pack_width);

  // country
  if (LDLoad(buf, aEmvCfg.eACFG, 0x9F1A)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Country: ");
    len += PackInfoData(p_mem+len, data+1, pack_width);
  }

  //currency
  if (LDLoad(buf, aEmvCfg.eACFG, 0x5F2A)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Currency: ");
    len += PackInfoData(p_mem+len, data+1, pack_width);
  }

  // blank line
  len += PackInfoData(p_mem+len, "", pack_width);

  // allow partial AID
  strcpy(p_mem+len, "Allow Partial AID: ");
  data[0] = (!(aEmvCfg.eBitField&eNO_PARTIAL_AID))? 'Y':'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // referral enable
  strcpy(p_mem+len, "Referral Enable: ");
  data[0] = (!(aEmvCfg.eBitField&eNO_REFERRAL))? 'Y':'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // PIN bypass enable
  strcpy(p_mem+len, "PIN Bypass Enable: ");
  data[0] = (aEmvCfg.eBitField&eENB_PIN_BYPASS)? 'Y':'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // Force TRM
  strcpy(p_mem+len, "Force TRM: ");
  data[0] = (aEmvCfg.eBitField&eFORCE_TRM)? 'Y':'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // TAC denial
  strcpy(p_mem+len, "TAC Denial: ");
  split(data, aEmvCfg.eTACDenial, 5);
  data[5*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // TAC Online
  strcpy(p_mem+len, "TAC Online: ");
  split(data, aEmvCfg.eTACOnline, 5);
  data[5*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // TAC Default
  strcpy(p_mem+len, "TAC Denial: ");
  split(data, aEmvCfg.eTACDefault, 5);
  data[5*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // Term Cap
  if (LDLoad(buf, aEmvCfg.eACFG, 0x9F33)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Term Cap: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // Add Cap
  if (LDLoad(buf, aEmvCfg.eACFG, 0x9F40)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Add Cap: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // setup data info
  SprintfMW(data, " EMV APP CFG");
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (aToDisplay) {
    if (DispInfoData(info_data) == MWKEY_CANCL) {
      ret_ok = FALSE;
    }
  }
  else  {
    PrintRcptLF(2);
    LptPutS(&data[1]);
    PrintRcptLF(1);
    ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintEMVKeyInfo
//  Description     : print or display EMV key info.
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintEMVKeyInfo(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;
  KEY_INFO key_info;

  // allocate large memory
  if ((p_mem = MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  if (!aToDisplay) {
    PrintRcptLF(2);
    strcpy(data, "EMV KEY INFO");
    LptPutS(data);
    PrintRcptLF(1);
  }

  // get key info one by one
  for (i=0; i<MAX_APPL*6; i++) {
    // get key info
    if (!GetEMVKeyInfo(&key_info, i)) {
      break;
    }

    // set description
    if ((i==0) || ((len==0)&&(aToDisplay))) {
      strcpy(p_mem+len, "Public Key-ID");
      len += PackInfoData(p_mem+len, "Eff   Exp  ", pack_width);
    }

    // set key info
    split(p_mem+len, key_info.RID, 5);
    p_mem[len+10] = '-';
    SprintfMW(p_mem+len+11, "%02X", key_info.KeyIdx);
    SprintfMW(data, "%02X/%02X ", key_info.EffectDate/256, key_info.EffectDate%256);
    SprintfMW(data+strlen(data), "%02X/%02X ", key_info.ExpiryDate/256, key_info.ExpiryDate%256);
    len += PackInfoData(p_mem+len, data, pack_width);

    // print first to prevent overfall


    if (len > 960) {
      SprintfMW(data, " EMV KEY INFO");
      info_data.b_line_width  = (unsigned char)pack_width;
      info_data.w_info_len    = len;
      info_data.pb_header     = data;
      info_data.pb_info       = p_mem;
      len = 0;
      if (aToDisplay) {
        if (DispInfoData(info_data) == MWKEY_CANCL) {
          ret_ok = FALSE;
          break;
        }
      }
      else  {
        ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
      }
    }
  }

  // setup data info
  SprintfMW(data, " EMV KEY INFO");
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (len && ret_ok) {
    if (aToDisplay) {
      if (DispInfoData(info_data) == MWKEY_CANCL) {
        ret_ok = FALSE;
      }
    }
    else  {
      ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
    }
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintEMVParam
//  Description     : print or display EMV parameter report.
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintEMVParam(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  int i;
  APP_CFG emv_cfg;

  // EMV app cfg
  for (i=0; i<MAX_APPL && ret_ok; i++) {
    if (!GetEMVAppCfg(&emv_cfg, i)) {
      break;
    }
    if (emv_cfg.eType == 0) {
      continue;
    }
    ret_ok = PrintEMVAppCfg(emv_cfg, aDetail, aToDisplay);
  }

  //EMV key info
  if (ret_ok) {
    ret_ok = PrintEMVKeyInfo(aDetail, aToDisplay);
  }

  return ret_ok;
}
//*****************************************************************************
//  Function        : EMVUtilStart
//  Description     : Initialize all Globals variable for parameters passing.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EMVUtilStart(void)
{
  sbIOBuf  = (BYTE *)  MallocMW(1500);
  sEMVIn  = (sIO_t *) MallocMW(sizeof(sIO_t));
  sEMVOut = (sIO_t *) MallocMW(sizeof(sIO_t));

  MemFatalErr(sbIOBuf);  // Check Memory Pointer
  MemFatalErr(sEMVIn);  // Check Memory Pointer
  MemFatalErr(sEMVOut);  // Check Memory Pointer
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
  BYTE init_mode;

  init_mode = 0;

  sEMVIn->bMsg = sEMVOut->bMsg = sbIOBuf;
  GetSysCfgMW(K_CF_SerialNo, sEMVIn->bMsg);
  sEMVIn->wLen = 8;

  if (EMV_DllSetup(IOCMD_IFDSN_RW, sEMVIn, sEMVOut) == FALSE) // set IFDSN
    EMVFatal();

  memcpy(sEMVIn->bMsg, KPinMsg, sizeof(KPinMsg));
  sEMVIn->wLen = sizeof(KPinMsg);
  memcpy(sEMVIn->bMsg+sEMVIn->wLen, KCardholderIDName, sizeof(KCardholderIDName));
  sEMVIn->wLen+=sizeof(KCardholderIDName);
  init_mode |= BIT_PSE_SUPPORT;
  EMV_DllMsg(init_mode,  sEMVIn, sEMVOut);
}

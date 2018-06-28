//-----------------------------------------------------------------------------
//  File          : CTLCfg.c
//  Module        :
//  Description   : CTL Configuration Process.
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
#include "emvcl2dll.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "infodata.h"
#include "lptutil.h"
#include "emvtrans.h"
#include "ctlcfg.h"

//-----------------------------------------------------------------------------
//      Common Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Constant Message
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Global Variables
//-----------------------------------------------------------------------------
static BYTE   *sbIOBuf;
static CL_IO  *sCTLIn;
static CL_IO  *sCTLOut;
static BYTE   bCtlLoaded;
static BYTE   bMaxNumBlk, bDnloadBlk;
static WORD   wChunkSize;
static int    iDnloadFile = -1;

static const BYTE KCTLTemp[] = {"ctl_tmp"};


//*****************************************************************************
//  Function        : KernelReady
//  Description     : Check Whether CTL Kernel exist
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN KernelReady(void)
{
  if (!IsAppIdExistMW(EMVCLDLL_ID))
    return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : CTLFatal
//  Description     : CTL Fatal Error.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CTLFatal(void)
{
  DispPutStr("\f\nCTL2 FATAL ERR\nPLS CALL SERVICE");
  while (WaitKey(KBD_TIMEOUT) != MWKEY_CANCL);
  ResetMW();
}
//*****************************************************************************
//  Function        : ClrCTLKey
//  Description     : Clear All CTL Keys.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClrCTLKey(void)
{
  if (!KernelReady())
    return;

  sCTLIn->pbMsg = sCTLOut->pbMsg = sbIOBuf;
  memset(sCTLIn->pbMsg, 0, 6);
  sCTLIn->wLen = 6;
  if (emvclSetup(CLCMD_KEY_LOADING,sCTLIn,sCTLOut) == FALSE)     // clear all key
    CTLFatal();
}
//*****************************************************************************
//  Function        : ClrCTLParam
//  Description     : Clear all CTL dll's parameters.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClrCTLParam(void)
{
  if (!KernelReady())
    return;

  sCTLIn->pbMsg = sCTLOut->pbMsg = sbIOBuf;

  GetSysCfgMW(K_CF_SerialNo, sCTLIn->pbMsg);
  sCTLIn->wLen = 8;

  if (emvclSetup(CLCMD_IFDSN_RW,sCTLIn,sCTLOut) == FALSE)     // set IFDSN
    CTLFatal();

  emvclSetup(CLCMD_CLR_AID,sCTLIn,sCTLOut);              // clear AID parameters
}
//*****************************************************************************
//  Function        : SetCTLLoaded
//  Description     : set CTL loaded flag
//  Input           : aFlag
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetCTLLoaded(BYTE aFlag)
{
  bCtlLoaded |= aFlag;
}
//*****************************************************************************
//  Function        : CTLClrDnloadBuf
//  Description     : Prepare download buffer for CTL data download.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CTLClrDnloadBuf (void)
{
  BYTE filename[32];
  if (sizeof(CL_CFGP) < sizeof(CL_KEY_ROOM))
    wChunkSize = sizeof(CL_KEY_ROOM)+3;  // 3B for type and len
  else
    wChunkSize = sizeof(CL_CFGP)+3;   // 3B for type and len
  //bMaxNumBlk = sizeof(sbIOBuf)/wChunkSize;
  bMaxNumBlk = 1500/wChunkSize;                       //29-09-16 JC ++
  bDnloadBlk = bCtlLoaded = 0;

  if (iDnloadFile >= 0) {
    fCloseMW(iDnloadFile);
    iDnloadFile = -1;
  }

  strcpy(filename, KCTLTemp);
  fDeleteMW(filename);
  iDnloadFile = fCreateMW(filename, 0);
}
//*****************************************************************************
//  Function        : CTLKillDnloadBuf
//  Description     : Kill CTL data download buffer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CTLKillDnloadBuf (void)
{
  BYTE filename[32];
  strcpy(filename, KCTLTemp);
  if (iDnloadFile >= 0) {
    fCloseMW(iDnloadFile);
    iDnloadFile = -1;
  }
  fDeleteMW(filename);
}
//*****************************************************************************
//  Function        : CTLData2DnloadBuf
//  Description     : Store CTL param/key to download buffer in file system.
//  Input           : aType;        // CTL param or key
//                    aData;        // pointer to CTL param or key
//                    aLen;         // length of CTL param or key
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CTLData2DnloadBuf (BYTE aType, BYTE *aData, WORD aLen)
{
  WORD offset;

  offset = wChunkSize*bDnloadBlk;
  sbIOBuf[offset] = aType;
  memcpy(&sbIOBuf[offset+1], &aLen, 2);
  memcpy(&sbIOBuf[offset+3], aData, aLen);

  if (++bDnloadBlk >= bMaxNumBlk) {
    if (fWriteMW(iDnloadFile, sbIOBuf, wChunkSize*bDnloadBlk) != (wChunkSize*bDnloadBlk))
      bCtlLoaded = 0xFF;  // indicate fail
    bDnloadBlk = 0;
  }
}
//*****************************************************************************
//  Function        : CTLDataUpdate
//  Description     : Clear/Store param/key to EMVCLDLL.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CTLDataUpdate (void)
{
  int size, len;
  BYTE type;

  if (!KernelReady() || (bCtlLoaded == 0xFF) || (bCtlLoaded == 0x00))
    return;

  if ((bDnloadBlk != 0) && (bDnloadBlk < bMaxNumBlk)) { // store remaining data chunk
    if (fWriteMW(iDnloadFile, sbIOBuf, wChunkSize*bDnloadBlk) != (wChunkSize*bDnloadBlk))
      return;
  }

  if (bCtlLoaded & 0x80)
    ClrCTLParam();
  if (bCtlLoaded & 0x40)
    ClrCTLKey();

  //fFlushMW(iDnloadFile);
  size = fLengthMW(iDnloadFile);
  fSeekMW(iDnloadFile, 0);
  len = wChunkSize-1;
  while (size > 0) {
    fReadMW(iDnloadFile, &type, 1);
    fReadMW(iDnloadFile, &sCTLIn->wLen, 2);
    if (fReadMW(iDnloadFile, sCTLIn->pbMsg, len-2) == (len-2)) {
      if (emvclSetup(type, sCTLIn, sCTLOut)) {
        size -= (len+1);
        continue;
      }
    }
    bCtlLoaded |= 0x0F;   // indicate fail
    break;
  }
  if ((bCtlLoaded & 0x0F) == 0x01) {
    if (emvclSetup(CLCMD_CLOSE_LOAD, sCTLIn, sCTLOut) == FALSE) // close AID loading
      bCtlLoaded |= 0x0F; // indicate fail
  }
  if ((bCtlLoaded & 0x0F) == 0x0F) {
    //if (bCtlLoaded & 0x80)
    //  ClrAllParam();
    if (bCtlLoaded & 0x40)
      ClrCTLKey();
    CTLKillDnloadBuf();
    CTLFatal();
  }
}
//*****************************************************************************
//  Function        : GetCTLAppCfg
//  Description     : Retrieve Configuration from CTL KERNEL.
//  Input           : aCfg;     // pointer to buffer
//                    aIdx;     // config index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN GetCTLAppCfg(CL_CFGP *aCfg, DWORD aIdx)
{
  sCTLIn->pbMsg = sCTLOut->pbMsg = sbIOBuf;
  sCTLIn->pbMsg[0] = (BYTE) aIdx;
  sCTLIn->wLen = 1;
  if (emvclSetup(CLCMD_READ_AIDIDX,sCTLIn,sCTLOut) == FALSE)     // load APP parameters
    return FALSE;
  memcpy(aCfg, sCTLOut->pbMsg, sizeof(CL_CFGP));
  return TRUE;
}
//*****************************************************************************
//  Function        : GetCTLKeyInfo
//  Description     : Retrieve Key info. from CTL KERNEL.
//  Input           : aKeyInfo;     // pointer to buffer
//                    aIdx;         // config index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetCTLKeyInfo(CL_KEY_INFO *aKeyInfo, DWORD aIdx)
{
  sCTLIn->pbMsg = sCTLOut->pbMsg = sbIOBuf;
  sCTLIn->pbMsg[0] = (BYTE) aIdx|0x80;
  sCTLIn->wLen = 1;

  if ((emvclSetup(CLCMD_READ_KEY,sCTLIn,sCTLOut) == FALSE) || (sCTLOut->wLen == 0))     // Read Key Info
    return FALSE;

  memcpy(aKeyInfo, sCTLOut->pbMsg, sizeof(CL_KEY_INFO));
  return TRUE;
}
//*****************************************************************************
//  Function        : PrintCTLParam
//  Description     : print or display CTL parameter report.
//  Input           : aCtlCfg
//                    aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintCTLAppCfg(CL_CFGP aCtlCfg, BOOLEAN aDetail, BOOLEAN aToDisplay)
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
  if (LDLoad(buf, aCtlCfg.abTLV, 0x9F06)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "AID: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // type & ver
  SprintfMW(p_mem+len, "Type: %02X ", aCtlCfg.bEType);
  memset(data, 0x00, sizeof(data));
  if (LDLoad(buf, aCtlCfg.abTLV, 0x9F09)) {
    strcpy(data, "Ver: ");
    split(data+strlen(data), buf+1, buf[0]);
  }
  len += PackInfoData(p_mem+len, data, pack_width);

  // floor limit
  if (LDLoad(buf, aCtlCfg.abTLV, 0x9F1B)) {
    memset(data, 0x00, sizeof(data));
    ConvDecimal(buf, 7);
    memcpy(data, buf+1, buf[0]);
    strcpy(p_mem+len, "FLR LMT: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // oversea floor limit
  if (LDLoad(buf, aCtlCfg.abTLV, 0xDF7F)) {
    memset(data, 0x00, sizeof(data));
    ConvDecimal(buf, 7);
    memcpy(data, buf+1, buf[0]);
    strcpy(p_mem+len, "FLR LMT(O): ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // basic random
  strcpy(p_mem+len, "Basic Random: ");
  SprintfMW(data, "%0d.%02d", aCtlCfg.dRSBThresh/100, aCtlCfg.dRSBThresh%100);
  len += PackInfoData(p_mem+len, data, pack_width);

  // target & max target
  strcpy(p_mem+len, "Target %: ");
  SprintfMW(data, "%02d-%02d", aCtlCfg.bRSTarget, aCtlCfg.bRSBMax);
  len += PackInfoData(p_mem+len, data, pack_width);

  // country
  if (LDLoad(buf, aCtlCfg.abTLV, 0x9F1A)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Country: ");
    len += PackInfoData(p_mem+len, data+1, pack_width);
  }

  //currency
  if (LDLoad(buf, aCtlCfg.abTLV, 0x5F2A)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Currency: ");
    len += PackInfoData(p_mem+len, data+1, pack_width);
  }

  // blank line
  len += PackInfoData(p_mem+len, "", pack_width);

  // allow partial AID
  strcpy(p_mem+len, "Allow Partial AID: ");
  data[0] = (!(aCtlCfg.bBitField&BIT_PARTIAL_NA))? 'Y':'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // referral enable
  strcpy(p_mem+len, "Referral Enable: ");
  data[0] = (!(aCtlCfg.bBitField&BIT_REFERRAL_NA))? 'Y':'N';
  data[1] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // TAC denial
  strcpy(p_mem+len, "TAC Denial: ");
  split(data, aCtlCfg.abTACDenial, 5);
  data[5*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // TAC Online
  strcpy(p_mem+len, "TAC Online: ");
  split(data, aCtlCfg.abTACOnline, 5);
  data[5*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // TAC Default
  strcpy(p_mem+len, "TAC Denial: ");
  split(data, aCtlCfg.abTACDefault, 5);
  data[5*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // Term Cap
  if (LDLoad(buf, aCtlCfg.abTLV, 0x9F33)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Term Cap: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // Add Cap
  if (LDLoad(buf, aCtlCfg.abTLV, 0x9F40)) {
    memset(data, 0x00, sizeof(data));
    split(data, buf+1, buf[0]);
    strcpy(p_mem+len, "Add Cap: ");
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // setup data info
  SprintfMW(data, " CTL APP CFG");
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
//  Function        : PrintCTLKeyInfo
//  Description     : print or display CTL key info.
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintCTLKeyInfo(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;
  CL_KEY_INFO key_info;

  // allocate large memory
  if ((p_mem = MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  if (!aToDisplay) {
    PrintRcptLF(2);
    strcpy(data, "CTL KEY INFO");
    LptPutS(data);
    PrintRcptLF(1);
  }

  // get key info one by one
  for (i=0; i<MAX_CL_KEY; i++) {
    // get key info
    if (!GetCTLKeyInfo(&key_info, i)) {
      break;
    }

    // set description
    if ((i==0) || ((len==0)&&(aToDisplay))) {
      strcpy(p_mem+len, "Public Key-ID");
      len += PackInfoData(p_mem+len, "Eff   Exp  ", pack_width);
    }

    // set key info
    split(p_mem+len, key_info.abRID, 5);
    p_mem[len+10] = '-';
    SprintfMW(p_mem+len+11, "%02X", key_info.bKeyIdx);
    SprintfMW(data, "%02X/%02X ", key_info.wEffectMMYY/256, key_info.wEffectMMYY%256);
    SprintfMW(data+strlen(data), "%02X/%02X ", key_info.wExpiryMMYY/256, key_info.wExpiryMMYY%256);
    len += PackInfoData(p_mem+len, data, pack_width);

    // print first to prevent overfall


    if (len > 960) {
      SprintfMW(data, " CTL KEY INFO");
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
  SprintfMW(data, " CTL KEY INFO");
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
//  Function        : PrintCTLParam
//  Description     : print or display CTL parameter report.
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintCTLParam(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  int i;
  CL_CFGP ctl_cfg;

  // CTL app cfg
  for (i=0; i<MAX_CL_CFG && ret_ok; i++) {
    if (!GetCTLAppCfg(&ctl_cfg, i)) {
      break;
    }
    if (ctl_cfg.bEType == 0) {
      continue;
    }
    ret_ok = PrintCTLAppCfg(ctl_cfg, aDetail, aToDisplay);
  }

  //CTL key info
  if (ret_ok) {
    ret_ok = PrintCTLKeyInfo(aDetail, aToDisplay);
  }

  return ret_ok;
}
//*****************************************************************************
//  Function        : CTLUtilStart
//  Description     : Initialize all Globals variable for parameters passing.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CTLUtilStart(void)
{
  sbIOBuf  = (BYTE *)  MallocMW(1500);
  sCTLIn  = (CL_IO *) MallocMW(sizeof(CL_CFGP));
  sCTLOut = (CL_IO *) MallocMW(sizeof(CL_CFGP));

  MemFatalErr(sbIOBuf);  // Check Memory Pointer
  MemFatalErr(sCTLIn);  // Check Memory Pointer
  MemFatalErr(sCTLOut);  // Check Memory Pointer
  return TRUE;
}

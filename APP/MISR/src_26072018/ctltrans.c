//-----------------------------------------------------------------------------
//  File          : ctltrans.c
//  Module        :
//  Description   : Include routines for Contactless transactions.
//  Author        : John
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
//  21 Oct 2010   John        Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "apm.h"
#include "util.h"
#include "sysutil.h"
#include "message.h"
#include "corevar.h"
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
#include "tlvutil.h"
#include "lptutil.h"
#include "emvtrans.h"
#include "ctltrans.h"
#include "auxcom.h"
#include "keytrans.h"
#include "system.h"

//-----------------------------------------------------------------------------
//    Globals Variables
//-----------------------------------------------------------------------------
static BYTE gTagBuf[256];
static TLIST gTagList;
static DWORD gCtlStartCnt=0;              //25-07-17 JC ++
static DWORD gCtlOffNow=0;                //25-07-17 JC ++

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
#define bAction         gGDS->s_CTLOut.pbMsg[1]

// contactless function return
#define CL_OK             (int)0
#define CL_ERR            (int)-1
#define CL_TIMEOUT        (int)-2
#define CL_MORE_CARDS     (int)-3
#define CL_WTX_TIMEOUT    (int)-4

#define CTL_COMPLETE      0

// contactless transaction amount limit
#define CTL_TRANS_LIMIT   50000           // $500.00

#define CTL_ON_ONCE_INTERVAL   300        // 300 msec

//-----------------------------------------------------------------------------
//    Constant
//-----------------------------------------------------------------------------
const WORD wCTLOnlineTags[] = { 
  0x5F2A,                                 /* Transaction Currency Code */
  0x5F34,                                 /* PAN Sequence Number */
  0x82,                                   /* AIP */
  0x84,                                   /* Dedicated File Name */
  0x95,                                   /* TVR */
  0x9A,                                   /* Transaction Date */
  0x9C,                                   /* Transaction Type */
  0x9F02,                                 /* Amount Authorised */
  0x9F03,                                 /* Amount, Other */
  0x9F08,                                 /* Application Version # (ICC) */
  0x9F10,                                 /* Issuer Application Data */
  0x9F1A,                                 /* Terminal Country Code */
  0x9F21,                                 /* Trans Time */
  0x9F26,                                 /* Application Cryptogram */
  0x9F27,                                 /* CID */
  0x9F34,                                 /* CVM Result */
  0x9F35,                                 /* Terminal Type */
  0x9F36,                                 /* Application Transaction Counter */
  0x9F37,                                 /* Unpredicatable Number */
  0
};

// pack zero data for compliance if not returned by EMVCLDLL
const WORD wCTLDummyTags[][2] = {
  {0x9F08, 2},                              /* Application Version # (ICC) */
  {0x9F34, 3},                              /* CVM Result */
  {0,      0},
};

static void CTLComplete(void);
TLIST *CTLTagSeek(WORD aTag);
extern TLIST *TagSeek(TLIST *aSrc, WORD aTag);
extern BYTE DeAss(TLIST *aList, BYTE *pbMsg, WORD wLen);


//=============================================================================
//  Function        : CL_Led
//  Description     :
//  Input           : aIdOnOffCycle - Id | On | Off | Cycle
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
void CL_Led(BYTE *aIdOnOffCycle)
{
#ifndef WIN32 // simulator not supported yet
 
  BYTE bLedId;
  DWORD dOnOff, dCycle;
  switch (aIdOnOffCycle[0]) { //map to led id
    case 0: //blue
      bLedId = 1;
      break;
    case 1: //yellow
      bLedId = 2;
      break;
    case 2: //green
      bLedId = 0;
      break;
    case 3: //red
      bLedId = 3;
      break;
    default: //unknown, use red as default
      bLedId = 3;
      break;
  }  
  if (aIdOnOffCycle[1] == 0xFF) { //on
    os_clicc_led_set(bLedId);
  }
  else if (aIdOnOffCycle[2] == 0xFF) { //off
    os_clicc_led_clear(bLedId);
  }
  else { //flash
    dOnOff = ((DWORD)(aIdOnOffCycle[1]*10)<<16) | (DWORD)(aIdOnOffCycle[2]*10); //change to 100ms unit 
    dCycle = (aIdOnOffCycle[3] == 0xFF)? 0xFFFFFF: aIdOnOffCycle[3];
    os_clicc_led_on(bLedId, dOnOff, dCycle);
  }

#endif // #ifndef WIN32 
}
//=============================================================================
//  Function        : CL_Led
//  Description     :
//  Input           : aSet
//                    
//  Return          :
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
void CL_LedSet(DWORD aSet)
{
  switch(aSet) {
    case CTL_LED_IDLE:
      // all led off
      CL_Led("\x00\x00\xFF\x00");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\x00\xFF\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    case CTL_LED_TAP_CARD:
      // blue led on
      CL_Led("\x00\xFF\x00\x00");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\x00\xFF\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    case CTL_LED_PROC:
      // blue & yellow leds on
      CL_Led("\x00\xFF\x00\x00");
      CL_Led("\x01\xFF\x00\x00");
      CL_Led("\x02\x00\xFF\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    case CTL_LED_TC:
      // blue & green leds on
      CL_Led("\x00\xFF\x00\x00");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\xFF\x00\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    case CTL_LED_ARQC:
      // blue & green leds on
      CL_Led("\x00\xFF\x00\x00");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\xFF\x00\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    case CTL_LED_AAC:
      // blue & red leds on
      CL_Led("\x00\xFF\x00\x00");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\x00\xFF\x00");
      CL_Led("\x03\xFF\x00\x00");
      break;
    case CTL_LED_RM_CARD:
      // blue & green leds on
      CL_Led("\x00\xFF\x00\x00");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\xFF\x00\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    case CTL_LED_TRANS_IDLE:
      // all led off except blue led blinks
      CL_Led("\x00\x02\x32\xFF");
      CL_Led("\x01\x00\xFF\x00");
      CL_Led("\x02\x00\xFF\x00");
      CL_Led("\x03\x00\xFF\x00");
      break;
    default:
      break;
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
  gGDS->s_CTLIn.pbMsg[0] = (BYTE) aIdx;
  gGDS->s_CTLIn.wLen = 1;
  if (emvclSetup(CLCMD_READ_AIDIDX, &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE)     // load APP parameters
    return FALSE;
  memcpy(aCfg, gGDS->s_CTLOut.pbMsg, sizeof(CL_CFGP));
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
  gGDS->s_CTLIn.pbMsg[0] = (BYTE) aIdx|0x80;
  gGDS->s_CTLIn.wLen = 1;

  if ((emvclSetup(CLCMD_READ_KEY, &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE) || (gGDS->s_CTLOut.wLen == 0))     // Read Key Info
    return FALSE;

  memcpy(aKeyInfo, gGDS->s_CTLOut.pbMsg, sizeof(CL_KEY_INFO));
  return TRUE;
}
//=============================================================================
//  Function        : CL_Init
//  Description     :
//  Input           : 
//  Return          : >=0 - OK
//                    <0  - Err
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
BOOLEAN CL_Init(void)
{
#ifndef WIN32 // simulator not supported yet
  CL_CFGP ctl_cfg;
  CL_KEY_INFO key_info;

  // clear all LEDs
  CL_LedSet(CTL_LED_IDLE);

  // check if 1 set of config & key exists in kernel
  if (!GetCTLAppCfg(&ctl_cfg, 0) || !GetCTLKeyInfo(&key_info, 0)) {
    CTLEnable(FALSE);
    return FALSE;
  }
  
  // open contactless interface
  if (os_clicc_open() == K_ERR_CLICC_OK) {
    os_clicc_led_open();
    CTLEnable(TRUE);
    return TRUE;
  }
  CTLEnable(FALSE);
  return FALSE;
#endif // #ifndef WIN32 
}
//=============================================================================
//  Function        : CL_WaitCard
//  Description     :
//  Input           : aTout10ms - timeout in 10ms
//                    aLvParam - LV param for open
//  Return          : >=0 - OK
//                    <0  - Err
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
int CL_WaitCard(DWORD aTout10ms, BYTE *aLvParam)
{
#ifndef WIN32 // simulator not supported yet

  int iRet=CL_TIMEOUT;
  BYTE bLen, abIStr[20];

  // turn on blue LED
  CL_LedSet(CTL_LED_TAP_CARD);

  //Param = [Len(1)] | [PollTime(2)] | [Timeout(2)] | [WtxTimeout(2) |..
  bLen = 0;
  abIStr[bLen++] = 0;   // num of bytes - set to 0 first
  WPut(abIStr+bLen, aTout10ms); //poll time in 10ms
  bLen += 2;
  // set parameter if need
  if ((aLvParam != NULL) && (aLvParam[0] > 0x02)) {
    memcpy(abIStr+bLen, aLvParam+3, aLvParam[0]-2); //ignore Poll time
    bLen += (aLvParam[0]-2);
  }
  abIStr[0] = bLen - 1;   // update final len
  //25-07-17 JC ++
  if (gCtlOffNow == 0) {
    gCtlStartCnt = FreeRunMark();
    gCtlOffNow = 1;
    iRet = os_clicc_poll(abIStr);
    switch (iRet) {
      case K_ERR_CLICC_OK:        
        iRet = CL_OK;
        break;
      case K_ERR_CLICC_NO_CARD:   
        iRet = CL_TIMEOUT;
        break;
      case K_ERR_CLICC_COLLISION: 
        iRet = CL_MORE_CARDS;
        break;
      case K_ERR_CLICC_TRANS:     
      case K_ERR_CLICC_PROTOCOL:  
      case K_ERR_CLICC_TIMEOUT:   
      case K_ERR_CLICC_INTERNAL:  
      case K_ERR_CLICC_NOT_READY: 
      case K_ERR_CLICC_INPUT:     
      default:    
        iRet = CL_ERR;
        break;
    }
  }
  else {
    if (FreeRunElapsed(gCtlStartCnt) > CTL_ON_ONCE_INTERVAL)
      gCtlOffNow = 0;
  }
  //25-07-17 JC --
  return iRet;

#endif // #ifndef WIN32 
}
//=============================================================================
//  Function        : CL_Close
//  Description     :
//  Input           : aWaitRemove10ms - If zero, no waiting and return ok.
//                      Other wait time, return ok if removed or err not yet remove
//  Return          : >=0 - OK
//                    <0  - Err
//  Note            : N/A
//  Globals Changed : N/A
//=============================================================================
int CL_Close(DWORD aWaitRemove10ms)
{
#ifndef WIN32 // simulator not supported yet

  int iRet;
  gCtlOffNow = 0;                           //25-07-17 JC ++
  if (aWaitRemove10ms == 0) { //PC1004 for reset command
    os_clicc_remove(1);    
  }
  iRet = os_clicc_remove(aWaitRemove10ms);
  switch (iRet) {
    case K_ERR_CLICC_OK:        
      iRet = CL_OK;
      break;
    case K_ERR_CLICC_NO_CARD:   
    case K_ERR_CLICC_COLLISION: 
    case K_ERR_CLICC_TRANS:     
    case K_ERR_CLICC_PROTOCOL:  
    case K_ERR_CLICC_TIMEOUT:   
    case K_ERR_CLICC_INTERNAL:  
    case K_ERR_CLICC_NOT_READY: 
    case K_ERR_CLICC_INPUT:     
    default:    
      iRet = CL_ERR;
      break;
  }
  os_disp_backc(RGB_BACKGR);  //25-10-12 JC ++  // set backgd colour when close
  return iRet;

#endif // #ifndef WIN32 
}
//=============================================================================
//  Function        : CL_Off
//  Description     :
//  Input           : N/A
//  Return          : N/A
//  Note            : //25-07-17 JC ++
//  Globals Changed : N/A
//=============================================================================
void CL_Off(void)
{
  os_clicc_led_close();
  os_clicc_close();
}
/******************************************************************************
 *  Function        : debugCTL
 *  Description     : EMV debug printing routines.
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void debugCTL(BYTE aState, BYTE aIn, CL_IO *pCTL)
{
  WORD i, len, max=70;
  BYTE buf[64];
  
  SprintfMW(buf, "emvclMsg(%d)-%s\n", aState, aIn? "In": "Out");
  DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
  len = pCTL->wLen;
  for (i=1; i<=len; i++) {
    PrintfMW("%02X", (BYTE)pCTL->pbMsg[i-1]);
    if ((i%max) == 0) {
      PrintfMW("..");
      APM_WaitKey(9000,0);
      ClearDispMW();
      DispGotoMW(MW_LINE2, MW_SPFONT);
    }
  }
  APM_WaitKey(9000,0);
}
/******************************************************************************
 *  Function        : debugAuxCTL
 *  Description     : EMV Processing state machine.
 *  Input           : N/A
 *  Return          : next action
 *  Note            : N/A
 *  Globals Changed : gds.emvdll, input;
 ******************************************************************************
 */
void debugAuxCTL(BYTE *pData, DWORD aLen)
{
  BYTE buf[20];
  BYTE *txbuf;
  DWORD free_run;
  
  txbuf = MallocMW(aLen*2+20);
  if (txbuf == NULL)
    return;
  
  set_pptr(txbuf, aLen*2+20);
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "CTL Msg Out(%d):", aLen);
  pack_mem(buf, strlen(buf));
  split_data(pData, aLen);
  pack_mem("\r\n", 2);

  if (!AuxOpen()) {
    printf("\fAuxOpen Err\n");
    while (GetCharMW()!=MWKEY_CANCL) SleepMW();
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
//*****************************************************************************
//  Function        : CTLTagSeek
//  Description     : Search CTL tags from EMVCLDLL.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
TLIST *CTLTagSeek(WORD aTag)
{
  TLIST *psLp;
  BYTE *pMsg, sCtlBuf[256];
  CL_IO s_CTLIn, s_CTLOut;

  if ((psLp = TagSeek(gGDS->s_TagList, aTag)) != NULL)
    return psLp;
  
  s_CTLIn.pbMsg = s_CTLOut.pbMsg = sCtlBuf;
  memset(s_CTLIn.pbMsg, 0, sizeof(sCtlBuf));
  pMsg = s_CTLIn.pbMsg;
  pMsg = TagPut(pMsg, aTag);
  s_CTLIn.wLen = pMsg - s_CTLIn.pbMsg;
  if (emvclSetup(CLCMD_GET_TAGS, &s_CTLIn, &s_CTLOut)) {
    // get valid 'V' if return len longer than 'T+L'
    if (s_CTLOut.wLen > (s_CTLIn.wLen + 1)) {
      gTagList.wTag = aTag;
      gTagList.sLen = s_CTLOut.pbMsg[s_CTLIn.wLen];
      memcpy(gTagBuf, &s_CTLOut.pbMsg[s_CTLIn.wLen+1], s_CTLOut.wLen-s_CTLIn.wLen-1);
      gTagList.pbLoc = gTagBuf;
      return &gTagList;
    }
  }
  
  return NULL;
}
//*****************************************************************************
//  Function        : CTLFatal
//  Description     : CTL error handler.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
void CTLFatal(void)
{
  Disp2x16Msg(GetConstMsg(EDC_CTL_FATAL), MW_LINE3, MW_BIGFONT);
  while (APM_WaitKey((DWORD)-1,0) != MWKEY_CANCL) SleepMW();
  ResetMW();
}
//*****************************************************************************
//  Function        : ePANExtract
//  Description     : Extract PAN data from CTL data.
//  Input           : N/A
//  Return          : FALSE => Error;
//  Note            : N/A
//  Globals Changed : gInput;
//*****************************************************************************
static BOOLEAN ePANExtract(BOOLEAN aCheckCard)
{
  #define TRK2_SEPARATOR  'D'
  TLIST *psLp;
  BYTE bLen, bSepLen;

  if (DeAss(gGDS->s_TagList, gGDS->s_CTLOut.pbMsg+2, gGDS->s_CTLOut.wLen - 2) == 0) {
    if (INPUT.b_trans != REFUND_CTL)
      return FALSE;
    memset(&gGDS->s_TagList, 0, sizeof(gGDS->s_TagList));
  }
  if ((psLp = CTLTagSeek(0x57)) != NULL) {
    if ((bLen = psLp->sLen)*2 <= sizeof(INPUT.s_trk2buf.sb_content)) {
      memset(INPUT.s_trk2buf.sb_content, 'F', sizeof(INPUT.s_trk2buf.sb_content));
      split(INPUT.s_trk2buf.sb_content, psLp->pbLoc, bLen);
      INPUT.s_trk2buf.b_len = bLen*2;
      INPUT.s_trk2buf.b_len = fndb(INPUT.s_trk2buf.sb_content, 'F', INPUT.s_trk2buf.b_len); // fix bug when trk2 len is odd number
      bSepLen = fndb(INPUT.s_trk2buf.sb_content, TRK2_SEPARATOR, INPUT.s_trk2buf.b_len);
      if (bSepLen <= 19) {
        // Get PAN and expiry date from Track 2
        INPUT.s_trk2buf.sb_content[bSepLen] = 'F';            /* cater for odd len */
        memset(INPUT.sb_pan, 0xFF, 10);
        compress(INPUT.sb_pan, INPUT.s_trk2buf.sb_content, (BYTE)((bSepLen+1)/2));
        INPUT.s_trk2buf.sb_content[bSepLen] = TRK2_SEPARATOR; /* cater for odd len */
        compress(INPUT.sb_exp_date, &INPUT.s_trk2buf.sb_content[1+bSepLen], 2);
      }
      if ((INPUT.sb_pan[9] & 0x0F) == 0x0F) {
        if (!aCheckCard || (aCheckCard && InCardTable()))
          return TRUE;
      }
      RSP_DATA.w_rspcode = 'U'*256+'C';
      return FALSE;
    }
  }
  
  return FALSE;
}
//*****************************************************************************
//  Function        : ValidCTLData
//  Description     : Validate & Extract CTL Data
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : gGTS;
//*****************************************************************************
BOOLEAN ValidCTLData(BOOLEAN aCheckCard)
{
  TLIST * psLp;
  BYTE bLen;
  
  memset(INPUT.sb_holder_name, ' ', sizeof(INPUT.sb_holder_name));		// KC++ 29-05-2012

  if (ePANExtract(aCheckCard) == FALSE) {
    return FALSE;
  }

  if ((psLp = CTLTagSeek(0x5F24)) != NULL) {    //App. Exp. Date
    memcpy(&INPUT.sb_exp_date, psLp->pbLoc, 2); 
  }
  if ((psLp = CTLTagSeek(0x5F20)) != NULL) {    //card holder name
    if ((bLen = psLp->sLen) > sizeof(INPUT.sb_holder_name))
      bLen = sizeof(INPUT.sb_holder_name);	
    memcpy(INPUT.sb_holder_name, psLp->pbLoc, bLen);
  }

  memset(INPUT.s_icc_data.sb_label, ' ',sizeof(INPUT.s_icc_data.sb_label));
  psLp = CTLTagSeek(0x50);      // app label
  if (psLp != NULL) {
    if ((bLen = psLp->sLen) > 16)
      bLen = 16;
    memcpy(INPUT.s_icc_data.sb_label, psLp->pbLoc, bLen);
  }
  
  return TRUE;
}
//*****************************************************************************
//  Function        : ctlPIN
//  Description     : Get CTL Online PIN.
//  Input           : N/A
//  Return          : FALSE => fail
//  Note            : KC++ Added for Contactless online PIN
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ctlPIN(void)
{
  if (GetPIN())
    return TRUE;
  
  RSP_DATA.w_rspcode = 'C'*256+'N';
  return FALSE;
}
//*****************************************************************************
//  Function        : PackCTLDummyData
//  Description     : Prepare CTL dummy data (filled with '0')
//  Input           : aBuf;         // pointer to dest buffer.
//                    aTags;        // list of tags
//  Return          : End of buffer pointer;
//  Note            : N/A
//  Globals Changed : N/a
//*****************************************************************************
BYTE *PackCTLDummyData(BYTE *aBuf, WORD aTag)
{
  WORD i;
  
  i = 0;
  while (wCTLDummyTags[i][0] != 0) {
    if (wCTLDummyTags[i][0] == aTag) {
      aBuf = TagPut(aBuf, aTag);
      *aBuf++ = wCTLDummyTags[i][1];
      memset(aBuf, 0, wCTLDummyTags[i][1]);
      aBuf += wCTLDummyTags[i][1];
      break;
    }
    i++;
  }
  return aBuf;
}
//*****************************************************************************
//  Function        : PackCTLTagsData
//  Description     : Prepare CTL tag data
//  Input           : aBuf;         // pointer to dest buffer.
//                    aTags;        // list of tags
//  Return          : End of buffer pointer;
//  Note            : N/A
//  Globals Changed : N/a
//*****************************************************************************
BYTE *PackCTLTagsData(BYTE *aBuf, WORD *aTags)
{
  WORD tag;
  TLIST *psLp;

  while ((tag = *aTags) != 0) {
    if ((psLp = CTLTagSeek(tag)) != NULL) {
      aBuf = TagPut(aBuf, tag);
      *aBuf++ = psLp->sLen; // assume byte len
      memcpy(aBuf, psLp->pbLoc, psLp->sLen);
      aBuf += psLp->sLen;
    }
    else {
      aBuf = PackCTLDummyData(aBuf, tag);   // pack zero data if need
    }
    aTags++;  // next tag word
  }
  return aBuf;
}
//*****************************************************************************
//  Function        : PackCTLData
//  Description     : Put CTL related data to  ICC DATA buffer
//  Input           : aDat;     // pointer to ICC DATA struct
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
void PackCTLData(struct ICC_DATA  *aDat)
{
  BYTE *ptr;

  aDat->w_misc_len = 0;
  ptr = PackCTLTagsData(aDat->sb_misc_content, (WORD *)wCTLOnlineTags);
  aDat->w_misc_len = (DWORD)ptr - (DWORD)aDat->sb_misc_content;
}
//*****************************************************************************
//  Function        : CTLOnline
//  Description     : Online Auth
//  Input           : N/A
//  Return          : FALSE: fail to go online, else TRUE
//  Note            : Zero length response => online failure
//  Globals Changed : N/A;
//*****************************************************************************
static BOOLEAN CTLOnline(void)
{
  if (DeAss(gGDS->s_TagList, gGDS->s_CTLOut.pbMsg+2, gGDS->s_CTLOut.wLen - 2) == 0) {
    if (INPUT.b_trans != REFUND_CTL)
      CTLFatal();
    memset(&gGDS->s_TagList, 0, sizeof(gGDS->s_TagList));
  }
  DispHeader(NULL);
  PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    if (ReversalOK()) {
      OfflineSent(FALSE);
      memcpy(TX_DATA.sb_trace_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
      memcpy(INPUT.sb_roc_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
      IncAPMTraceNo();
      MoveInput2Tx();
      memcpy(TX_DATA.sb_pin, INPUT.sb_pin, sizeof(INPUT.sb_pin)); // online pin
      PackCTLData(&TX_DATA.s_icc_data);  // field 55 ICC data  
      PackProcCode(TX_DATA.b_trans, TX_DATA.b_acc_ind);
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);

      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        if (RSP_DATA.b_response <= TRANS_REJ)
          UpdateHostStatus(NO_PENDING);
      }
      if (RSP_DATA.b_response <= TRANS_REJ)
        UpdateHostStatus(NO_PENDING);
    }
    APM_ResetComm();
    return TRUE;
  }
  return FALSE; // unable to go online
}
//*****************************************************************************
//  Function        : CTLReferral
//  Description     : Voice Referral
//  Input           : N/A
//  Return          : FALSE => ERROR
//  Note            : Zero length response => decline
//  Globals Changed : N/A
//******************************************************************************
static BOOLEAN CTLReferral(void)
{
  PackRspText();
  DispLineMW(&RSP_DATA.text[1], MW_LINE1, MW_CENTER|MW_BIGFONT);

  APM_ResetComm();
  RSP_DATA.w_rspcode = 'C'*256+'N';
  RSP_DATA.text[0] = 0;

  if (GetAuthCode()) {
    INPUT.b_trans_status = OFFLINE;
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
//  Function        : CTLAccepted
//  Description     : Store CTL Data into Batch.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gds.CTLdll, input;
//*****************************************************************************
static void CTLAccepted(void)
{
  TLIST *psLp;

  RSP_DATA.b_response = TRANS_ACP;
  if ((bAction & CLACT_ONLINE) == 0) {
    memset(RSP_DATA.sb_auth_code, ' ', 6);
    memcpy(RSP_DATA.sb_auth_code, "Y1", 2);
    IncAPMTraceNo();
    memcpy(TX_DATA.sb_trace_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
  }

  if ((psLp = CTLTagSeek(0x9f26)) != NULL)                  // app Cryptogram
    memcpy(INPUT.s_icc_data.sb_tag_9f26, psLp->pbLoc, psLp->sLen);
  if ((psLp = CTLTagSeek(0x4F)) != NULL) {                  //AID for printing
    memcpy(INPUT.s_icc_data.sb_aid, psLp->pbLoc, psLp->sLen);
    INPUT.s_icc_data.b_aid_len = psLp->sLen;
  }

  SetRspCode('0'*256+'0');
  PackCTLData(&INPUT.s_icc_data);  // field 55 ICC data
  if (bAction & ACT_SIGNATURE)
    RSP_DATA.w_rspcode = 'V'*256+'S';
  else
    RSP_DATA.w_rspcode = 'T'*256+'A';

  PackInputP();
  MoveInput2Tx();
  if (INPUT.b_trans != AUTH_CTL)
    SaveRecord();
}
//*****************************************************************************
//  Function        : CTLError
//  Description     : Handle CTL error response
//  Input           : CTL process next state.
//  Return          : CTL process next state.
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static DWORD CTLError(DWORD aNextState)
{
  BYTE buf[64];
  
  DispHeader(NULL);
  switch (gGDS->s_CTLOut.pbMsg[0]) {  //error message response
    case CLERR_SMCIO:
    case CLERR_CARDL1:
    case CLERR_DATA:
      RSP_DATA.w_rspcode = 'R'*256+'E';
      break;
    case CLERR_CANCEL:
      DispClrBelowMW(MW_LINE3);
      RSP_DATA.w_rspcode = 'C'*256+'N';
      break;
    case CLERR_SEQ:
      RSP_DATA.w_rspcode = 'S'*256+'C';
      break;
    case CLERR_NOMORE:
      RSP_DATA.w_rspcode = 'U'*256+'C';
      break;
    case CLERR_NOAPPL:  // not supported if no matching app
      RSP_DATA.w_rspcode = 'M'*256+'A'; // fallback
      break;
    case CLERR_BLOCKED:
      RSP_DATA.w_rspcode = 'S'*256+'B';
      break;
    case CLERR_TOI:
      RSP_DATA.w_rspcode = 'C'*256+'0';
      break;
    case CLERR_DDA:
      RSP_DATA.w_rspcode = 'C'*256+'1';
      break;
    case CLERR_OCARD:
      RSP_DATA.w_rspcode = 'C'*256+'2';
      break;
    case CLERR_PDCL:
      RSP_DATA.w_rspcode = 'C'*256+'3';     // CDET Test Case 12
      break;
    case CLERR_CONFIG:
    case CLERR_MEMORY:
    default:
      RSP_DATA.w_rspcode = 'S'*256+'E';
      break;
  }
  // MTIP TC025 (Paypass Refund handling)
  if ((gGDS->s_CTLOut.pbMsg[0] == CLERR_DATA) && (INPUT.b_trans == REFUND_CTL)) {
    RSP_DATA.w_rspcode = '0'*256+'0';
    aNextState = CTL_COMPLETE;
    RSP_DATA.b_response = TRANS_ACP;
    bAction |= CLACT_ONLINE;
    CTLComplete();
    return aNextState;
  }
  if (RSP_DATA.w_rspcode == 'C'*256+'3')
    Disp2x16Msg(GetConstMsg(EDC_RSP_CTL_REFER_MOBILE), MW_LINE3, MW_BIGFONT);
  else {
    SprintfMW(buf, "CLERR = %02x", gGDS->s_CTLOut.pbMsg[0]);
    DispLineMW(buf, MW_LINE5, MW_CLREOL|MW_CENTER|MW_BIGFONT);
  }
  LongBeep();
  APM_WaitKey(200,0);

  aNextState = CTL_COMPLETE;  // no fallback
  RSP_DATA.b_response = TRANS_REJ; // for print msg disable
  return aNextState;
}
//*****************************************************************************
//  Function        : CTLComplete
//  Description     : Handle CTL COMPLETE response
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CTLComplete(void)
{
  TLIST *psLp;
  
  if (DeAss(gGDS->s_TagList, gGDS->s_CTLOut.pbMsg+2, gGDS->s_CTLOut.wLen - 2) == 0) {
    if (INPUT.b_trans != REFUND_CTL)
      CTLFatal();
    memset(&gGDS->s_TagList, 0, sizeof(gGDS->s_TagList));
  }
  
  ClearResponse();
  if ((psLp = CTLTagSeek(0x9A)) != NULL)
    memcpy(&RSP_DATA.s_dtg.b_year, psLp->pbLoc, psLp->sLen);
  if ((psLp = CTLTagSeek(0x9F21)) != NULL)
    memcpy(&RSP_DATA.s_dtg.b_hour, psLp->pbLoc, psLp->sLen);
  memset(RSP_DATA.sb_auth_code, ' ', sizeof(RSP_DATA.sb_auth_code));
  RSP_DATA.w_rspcode = '5'*256+'1';
  RSP_DATA.b_response = TRANS_FAIL;
  
  DispHeader(NULL);
  Disp2x16Msg(GetConstMsg(EDC_CTL_CARD_READ_OK), MW_LINE3, MW_BIGFONT);

  ValidCTLData(TRUE);
  
  // check transaction CVM
  if ((psLp = CTLTagSeek(CLT_TRAN_CVM)) != NULL) {
    if (psLp->pbLoc[0] & (0x01 << (8 - TRAN_CVM_ONL_PIN))) {  
      // online PIN
      if (ctlPIN() == FALSE) {
        RSP_DATA.w_rspcode = 'C'*256+'N';
        LongBeep();
        return;
      }
    }
    if (psLp->pbLoc[0] & (0x01 << (8 - TRAN_CVM_MOBILE))) {  
      // mobile CVM
      DispLineMW("Mobile CVM Done", MW_LINE5, MW_CLREOL|MW_CENTER|MW_BIGFONT);
      APM_WaitKey(100,0);
    }
  }
  
  if (bAction & CLACT_REFERRAL) { // referral requested
    if (!CTLReferral())
      return;
    bAction |= CLACT_APPROVED;  // so it will print & update batch
  }
  if (bAction & CLACT_ONLINE) {
    // online
    CL_LedSet(CTL_LED_ARQC);
    if (CTLOnline()) {
      if (RSP_DATA.b_response == TRANS_ACP)
        bAction |= CLACT_APPROVED;
    }
  }
  
  if (bAction & CLACT_APPROVED) {
    CTLAccepted();
  }
  else {
    // declined
    LongBeep();
  }
}
//*****************************************************************************
//  Function        : CTLPreProcess
//  Description     : Pre-Processing for Contactless Txn.
//  Input           : trans;    // trans type
//  Return          : next action
//  Note            : N/A
//  Globals Changed :
//*****************************************************************************
BOOLEAN CTLPreProcess(DWORD aTrans)
{
  DWORD state;
  BYTE tmp[10];

  // init vars
  state = CLMSG_PROC_INIT;
  memset(gGDS->sb_IOBuf, 0, sizeof(gGDS->sb_IOBuf));
  gGDS->s_CTLOut.wLen = 0;
  memcpy(gGDS->s_CTLIn.pbMsg+1, "\x9F\x02\x06", 3);
  dbin2bcd(tmp, INPUT.dd_amount);
  memcpy(gGDS->s_CTLIn.pbMsg+4, &tmp[4], 6);
  gGDS->s_CTLIn.wLen = 10;
#ifdef ENABLE_LOG2COM1  // card log from COM1
  memcpy(&gGDS->s_CTLIn.pbMsg[gGDS->s_CTLIn.wLen], "\xDF\xF2\x01\x01\xDF\xF3\x01\x01", 8);
  gGDS->s_CTLIn.wLen += 8;
#endif //ENABLE_LOG2COM1
  if (aTrans == REFUND_CTL) {
    // setting refund txn type
    memcpy(&gGDS->s_CTLIn.pbMsg[gGDS->s_CTLIn.wLen], "\x9C\x01\x20", 3);
    gGDS->s_CTLIn.wLen += 3;
    // MTIP TC025 (Paypass Refund)
    // setting reader contactless floor limit for Mastercard
    memcpy(&gGDS->s_CTLIn.pbMsg[gGDS->s_CTLIn.wLen], "\x1F\xA3\x06\x00\x00\x00\x00\x00\x00", 9);
    gGDS->s_CTLIn.wLen += 9;
    // setting TAC - Denial for Mastercard
    memcpy(&gGDS->s_CTLIn.pbMsg[gGDS->s_CTLIn.wLen], "\x1F\xA1\x05\xFF\xFF\xFF\xFF\xFF", 8);
    gGDS->s_CTLIn.wLen += 8;
  }
  gGDS->s_CTLIn.pbMsg[0] = 0;
  if (emvclMsg(state, &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE) {
    CTLError(state);
    return FALSE;
  }
  os_disp_backc(RGB_WHITE); //25-10-12 JC ++  // set white colour when reader active
  return TRUE;
}
//*****************************************************************************
//  Function        : CTLWaitCard
//  Description     : Activate reader and wait for card.
//  Input           : N/A
//  Return          : next action
//  Note            : N/A
//  Globals Changed :
//*****************************************************************************
BOOLEAN CTLWaitCard(void)
{
  #define TX_TIMEOUT    500         // 5 secs
  int iClRet;
  BYTE bRetry, abLvClParam[16];

  // init
  //Param = [Len(1)] | [PollTime(2)] | [Timeout(2)] | [WtxTimeout(2)] | ..
  memset(abLvClParam, 0x00, sizeof(abLvClParam));
  abLvClParam[0] = 6;
  //PollTime will be set by CL_WaitCard()!
  WPut(abLvClParam+3, TX_TIMEOUT);  //Timeout(2)
  WPut(abLvClParam+5, TX_TIMEOUT);  //WtxTimeout is min 5 sec
  //memcpy(abLvClParam, "\x06\x00\x00\x05\xDC\x01\xF4", 7);   // Pody
  
  // wait card tag
  bRetry = 0;
  // tag card
  iClRet = CL_WaitCard(0, abLvClParam);
  //iClRet = CL_WaitCard(0, NULL);
  if (iClRet == CL_OK) { // card tagged
    return TRUE;
  }
  else if (iClRet == CL_MORE_CARDS) {
    if (bRetry++ >= 10) {
      DispLineMW("Too Many Cards!", MW_LINE5, MW_CLREOL|MW_CENTER|MW_BIGFONT);
      APM_WaitKey(9000,0);
      return FALSE;
    }
  } 
  else if (iClRet != CL_TIMEOUT) {
    if (bRetry++ >= 10) {
      return FALSE;
    }
  } 
    
  return FALSE;
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
void debugEMV(BYTE aState, BYTE aIn, CL_IO *pEMV)
{
  BYTE buf[30];
  WORD i;
  
#if 0
  // Print out TLV
  LptOpen(NULL);
  LptPutS("\x1B""F1\n");
  sprintf(buf, "dllMsg(%d)-%s\n", aState, aIn? "In": "Out");
  LptPutS(buf);
  for (i=1; i<=pEMV->wLen; i++) {
    sprintf(buf, "%02X ", (BYTE)pEMV->pbMsg[i-1]);
    LptPutN(buf, 3);
  }
  LptPutS("\n");
#else
  // Display TLV
  for (i=1; i<=pEMV->wLen; i++) {
    if ((i%64) == 1)
      DispLineMW("TLV-Out:\n", MW_LINE1, MW_CLRDISP|MW_SPFONT);
    SprintfMW(buf, "%02X ", (BYTE)pEMV->pbMsg[i-1]);
    DispPutNCMW(buf, 3);
    if ((i%8) == 0) PrintfMW("\n");
    if ((i%64) == 0)
      APM_WaitKey((DWORD)-1,0);
  }
  PrintfMW("\n[End]\x05");
  APM_WaitKey((DWORD)-1,0);
#endif
}
//*****************************************************************************
//  Function        : CTLDebug
//  Description     : Debug contactless txn data
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed :
//*****************************************************************************
void CTLDebug(void)
{
  #define PWAVE_DEBUG   1
  
  BYTE buf[128];
  TLIST *psLp;
  
  if (INPUT.dd_amount != 123)   // only debug when amount is $1.23
    return;
  
  SprintfMW(buf, "bAction=%02x", bAction);
  DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "TVR=", 4);
  if ((psLp = CTLTagSeek(0x95)) != NULL)
    split(buf+4, psLp->pbLoc, psLp->sLen); 
  // Tag CLT_VT95 only appear when DDA failed
  if ((psLp = CTLTagSeek(0xDFC3)) != NULL) {     // TVR for Paywave
    buf[14] = '/';
    split(buf+15, psLp->pbLoc, psLp->sLen); 
  }
  DispLineMW(buf, MW_LINE2, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "CID=", 4);
  if ((psLp = CTLTagSeek(0x9F27)) != NULL)
    split(buf+4, psLp->pbLoc, psLp->sLen); 
  DispLineMW(buf, MW_LINE3, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "P.Sch=", 6);
  if ((psLp = CTLTagSeek(0xDFAE)) != NULL)
    split(buf+6, psLp->pbLoc, psLp->sLen); 
  DispLineMW(buf, MW_LINE4, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "Trk2=", 5);
  if ((psLp = CTLTagSeek(0x57)) != NULL) {
    split(buf+5, psLp->pbLoc, psLp->sLen);
  }
  DispLineMW(buf, MW_LINE5, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "IAD=", 4);
  if ((psLp = CTLTagSeek(0x9F10)) != NULL)
    split(buf+4, psLp->pbLoc, psLp->sLen); 
  DispLineMW(buf, MW_LINE6, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "Txn.CVM=", 8);
  if ((psLp = CTLTagSeek(0xDFB5)) != NULL)
    split(buf+8, psLp->pbLoc, psLp->sLen);
  DispLineMW(buf, MW_LINE7, MW_SPFONT);
  
#ifdef PWAVE_DEBUG
  // for Paywave only
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "TTQ=", 4);
  if ((psLp = CTLTagSeek(0x9F66)) != NULL)
    split(buf+4, psLp->pbLoc, psLp->sLen);
  DispLineMW(buf, MW_LINE8, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, ", CTQ=", 6);
  if ((psLp = CTLTagSeek(0x9F6C)) != NULL)
    split(buf+6, psLp->pbLoc, psLp->sLen);
  DispLineMW(buf, MW_LINE8+12, MW_SPFONT);
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "CAP=", 4);
  if ((psLp = CTLTagSeek(0x9F68)) != NULL)
    split(buf+4, psLp->pbLoc, psLp->sLen);
  DispLineMW(buf, MW_LINE9, MW_SPFONT);
#endif
  WaitKey(9000,0);
}
//*****************************************************************************
//  Function        : CTLProcess
//  Description     : Contactless Processing state machine.
//  Input           : N/A
//  Return          : next action
//  Note            : N/A
//  Globals Changed :
//*****************************************************************************
DWORD CTLProcess(void)
{
  DWORD next_state;

  // turn on blue & yellow LEDs
  CL_LedSet(CTL_LED_PROC);
  
  // init vars
  memset(INPUT.sb_pin, 0, sizeof(INPUT.sb_pin));
  memcpy(INPUT.sb_trace_no, STIS_TERM_DATA.sb_trace_no, 3);
  next_state = CLMSG_PROC_START | CLMSGX_AUTO_TO_FINISH;
  //next_state = CLMSG_PROC_START;
  gGDS->s_CTLIn.pbMsg[0] = 0;
  gGDS->s_CTLIn.wLen    = 1;

  do {
    //debugCTL(next_state, 1, &gGDS->s_CTLIn);
    if (emvclMsg(next_state, &gGDS->s_CTLIn, &gGDS->s_CTLOut) == FALSE) {
      CL_Close(0);
      next_state=CTLError(next_state);
      CTLDebug();
      return next_state;
    }
    //debugCTL(next_state, 0, &gGDS->s_CTLOut);
    next_state = gGDS->s_CTLOut.pbMsg[0];
    switch (gGDS->s_CTLOut.pbMsg[0]&0x7F) {
      case CTL_COMPLETE:   /* completed */
        CL_Close(0);
        CTLComplete();
        CTLDebug();
        //debugEMV(next_state, 1, &gGDS->s_CTLOut);
        return CTL_COMPLETE;
      case CLMSG_ENTER_SEL: // select application
        gGDS->s_CTLIn.pbMsg[0] = 0;
        gGDS->s_CTLIn.wLen = 1;
        break;
      case CLMSG_VALID_CARD: /* validate application */
        if (ValidCTLData(TRUE)) {
         // if (!ConfirmCard()) {  // leave exp date chk to kernel
        //    CL_Close(0);
        //    RSP_DATA.w_rspcode = 'C'*256+'N';
        //    return CTL_COMPLETE; // 25-04-05++ txn end
        //  }
        }
        gGDS->s_CTLIn.pbMsg[0] = 0;
        gGDS->s_CTLIn.wLen = 1;
        break;
      default:
        CTLFatal();
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : CTLTrans
//  Description     : CTL trans handler.
//  Input           : trans;    // trans type
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
DWORD CTLTrans(DWORD aTrans)
{
  INPUT.b_trans = (BYTE) aTrans;
  if (STIS_TERM_DATA.b_stis_mode < TRANS_MODE) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  if (BlockLocalTrans()) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  if (HostSettlePending() || APM_BatchFull(INPUT.w_host_idx)) {
    DispErrorMsg(GetConstMsg(EDC_TU_CLOSE_BATCH));
    return FALSE;
  }
  
  INPUT.b_entry_mode = CONTACTLESS;
  RSP_DATA.b_response = TRANS_FAIL;
  RSP_DATA.w_rspcode = '5'*256+'1';
  GetEcrRef(FALSE);

  DispHeader(NULL); // show correct header

  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(GetConstMsg(EDC_CTL_PROCESSING), MW_LINE3, MW_BIGFONT);
  CTLProcess();
    
  if (bAction & CLACT_APPROVED)
    CL_LedSet(CTL_LED_TC);    // accept
  else
    CL_LedSet(CTL_LED_AAC);   // decline

  if ((RSP_DATA.w_rspcode != '0'*256+'0') && (RSP_DATA.w_rspcode != 'C'*256+'3'))
    TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  
  return TRUE;
}
//*****************************************************************************
//  Function        : CtlUnderTransLimit
//  Description     : CTL trans limit checking.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
BOOLEAN CtlUnderTransLimit(DWORD aAmount)
{
  if (INPUT.dd_amount > CTL_TRANS_LIMIT){
    //DispErrorMsg(GetConstMsg(EDC_CTL_AMT_EXCEED), MWLINE5);
    return FALSE;
  }

  return TRUE;
}
//=============================================================================
//  Function        : DispDefinedLogo
//  Description     : display and/or check pre-defined logo
//  Input           : 
//  Return          : -ve not support or err, 0 ok
//  Note            : N/A
//  Globals Changed : PC1205 new created
//=============================================================================
int DispDefinedLogo(DWORD aIdx, BOOLEAN aIsCheckAvailOnly)
{
  BYTE *pmLogo = NULL;

  switch (aIdx) {
    case DP_LOGO_LANDINGZONE:
      #ifdef K_LandZoneSize
        if (aIsCheckAvailOnly) {
          return 0;
        }
        pmLogo = os_malloc(K_LandZoneSize+4);
        if (pmLogo == NULL) {
          return -1;
        }
        if (os_config_read(K_CF_LandingZone, pmLogo) == TRUE) {
          os_disp_putc(K_ClrHome);
          os_disp_putg(pmLogo);
        }
        os_free(pmLogo);
        return 0;
      #else
        break;
      #endif
    default:
      break;
  }

  if (pmLogo != NULL) {
    os_free(pmLogo);
  }
  return -1;
}

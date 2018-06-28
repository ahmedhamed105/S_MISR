//-----------------------------------------------------------------------------
//  File          : coremain.c
//  Module        :
//  Description   : Main Entry.
//  Author        : Auto Generate by coremain_c.bat
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
//  DD MMM  YYYY  coremain_c  Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "hwdef.h"
#include "midware.h"
#include "sysutil.h"
#include "toggle.h"
#include "spectralogo.h"
#include "hardware.h"
#include "message.h"
#include "kbdutil.h"
#include "apmfunc.h"
#include "comm.h"
#include "stis.h"
#include "termdata.h"
#include "batchsys.h"
#include "emvtrans.h"
#include "ecrdll.h"
#include "pp300dll.h"
#include "key2dll.h"
//#if (CLCD_SUPPORT)
#include "spectrasym.h"
//#endif
#if (R700)
#include "phone.h"
#endif
#include "remotedl.h"
#include "ctlcfg.h"
#include "cakey.h"
#include "coremain.h"

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
struct APM_FILE_DATA gFileData;

//-----------------------------------------------------------------------------
//      Export Func
//-----------------------------------------------------------------------------
//void XF_GetHwHdl(DWORD x)                 { os_appcall_return(GetHwHandle(x)); }    //25-07-17 JC ++
void XF_GetHwHdl(DWORD x, DWORD y)        { os_appcall_return(GetHwHandle(x, y)); }   //25-07-17 JC ++
// Kbd Func
void XF_ClrKeyin()                        { ClearKeyin(); os_appcall_return(0); }
void XF_GetKeyin()                        { os_appcall_return(GetKeyin()); }
void XF_WaitKey(DWORD x, DWORD y)         { os_appcall_return(WaitKeyCheck(x, y)); }
void XF_YesNo()                           { os_appcall_return(YesNo()); }
void XF_GetKbd(DWORD x, DWORD y, BYTE *z) { os_appcall_return(GetKbd(x, y, z)); }
void XF_SetKbdTO(DWORD x)                 { SetKbdTimeout(x); os_appcall_return(0); };
void XF_GetKbdTO()                        { os_appcall_return(GetKbdTimeout()); }

// Comm Func
void XF_PackComm(struct COMMPARAM *x)     { PackComm(x); os_appcall_return(0); };
void XF_PreConnect()                      { PreConnect(); os_appcall_return(0); };
void XF_ConnectOK(DWORD x)                { os_appcall_return(ConnectOK(x)); };
void XF_SendRcvd(struct COMMBUF *x, struct COMMBUF *y) {os_appcall_return(SendRcvd(x, y)); };
void XF_FlushComm()                       { FlushComm(); os_appcall_return(0); };
void XF_ResetComm()                       { os_appcall_return(ResetComm()); };

// STIS Func
void XF_GetTermData(struct TERM_DATA *x)              { os_appcall_return(GetTermData(x)); }
void XF_GetTermCfg(struct TERM_CFG *x)                { os_appcall_return(GetTermCfg(x)); }
void XF_GetCardCount(void)                            { os_appcall_return(GetCardCount()); }
void XF_GetCardTbl(DWORD x, struct CARD_TBL *y)       { os_appcall_return(GetCardTbl(x, y)); }
void XF_GetIssuerCount(void)                          { os_appcall_return(GetIssuerCount()); }
void XF_GetIssuerTbl(DWORD x, struct ISSUER_TBL *y)   { os_appcall_return(GetIssuerTbl(x, y)); }
void XF_GetAcqCount(void)                             { os_appcall_return(GetAcqCount()); }
void XF_GetAcqTbl(DWORD x, struct ACQUIRER_TBL *y)    { os_appcall_return(GetAcqTbl(x, y)); }
void XF_GetDescCount(void)                            { os_appcall_return(GetDescCount()); }
void XF_GetDescTbl(DWORD x, struct DESC_TBL *y)       { os_appcall_return(GetDescTbl(x, y)); }
void XF_GetLogoCount(void)                            { os_appcall_return(GetLogoCount()); }
void XF_GetLogoTbl(DWORD x, struct LOGO_TBL *y)       { os_appcall_return(GetLogoTbl(x, y)); }
void XF_GetExParamCount(void)                         { os_appcall_return(GetExParamCount()); }
void XF_GetExtraParam(DWORD x, struct EXTRA_PARAM *y) { os_appcall_return(GetExtraParam(x, y)); }
void XF_GetPending(int x)                             { os_appcall_return(GetPending(x)); }
void XF_SetPending(int x, BYTE y)                     { SetPending(x,y); os_appcall_return(0); }
void XF_GetBatchNo(DWORD x, BYTE *y)                  { os_appcall_return(GetBatchNo(x, y)); }
void XF_SetBatchNo(DWORD x, BYTE *y)                  { SetBatchNo(x, y); os_appcall_return(0); }
void XF_SelectAcquirer(int x)                         { os_appcall_return(SelectAcquirer(x)); }
void XF_SetROC(void *x)                               { SetROC(x); os_appcall_return(0); }
void XF_SetTrace(void *x)                             { SetTrace(x); os_appcall_return(0); }
void XF_StisClr(void)                                 { CleanStisTable(); os_appcall_return(0); };
void XF_StisAddTbl(int x, void *y)                    { os_appcall_return(StisAdd(x, y)); };
void XF_TermDataClr(void)                             { CleanTermData(); os_appcall_return(0); };
void XF_TermDataUpd(void *x)                          { os_appcall_return(TermDataUpd(x)); };

// BatchSys Func
void XF_BatSysClear(int x)                            { BatSysClear(x); os_appcall_return(0);};
void XF_BatchFull(int x)                              { os_appcall_return(BatchFull(x));};
void XF_GetRecCount()                                 { os_appcall_return(GetRecCount());};
void XF_GetBatchRec(int x, void *y, int z)            { os_appcall_return(GetBatchRec(x, y, z));};
void XF_GetRevRec(int x, void *y, int z)              { os_appcall_return(GetRevRec(x, y, z));};
void XF_ClearRevRec(int x)                            { ClearRevRec(x); os_appcall_return(0);};
void XF_SaveBatRec(void *x, struct BATSYS_HDR *y)     { os_appcall_return(SaveBatRec(x, y));};
void XF_UpdateBatRec(int x, void *y, struct BATSYS_HDR *z) {os_appcall_return(UpdateBatRec(x, y, z));};
void XF_TraceInBatch(BYTE *x)                         { os_appcall_return(TraceInBatch(x)); };
void XF_TraceInReversal(BYTE *x)                      { os_appcall_return(TraceInReversal(x)); };

// Misc Func
void XF_GetAccessCode(void)                           { os_appcall_return(GetAccessCode()); };
void XF_SelYesNo(void)                                { os_appcall_return(ToggleYesNo()); };
void XF_SelApp(DWORD x)                               { os_appcall_return(SelectAppl(x)); };
void XF_MerchFunc(DWORD x)                            { os_appcall_return(MerchFunc(x, TRUE)); };
void XF_PwrSleep(DWORD x)                             { os_appcall_return(PwrSleep(x)); };  //25-07-17 JC ++


//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
const BYTE KSoftName[] = {"APM"};
const BYTE KApmDataFile[] = {"APM.DAT"};

const DWORD KExportFunc[] =    // Export Function Table
{
  (DWORD) APM_FUNC_COUNT,      // Importance!! Number of export functions!!!
  (DWORD) XF_GetHwHdl,
  (DWORD) XF_ClrKeyin,
  (DWORD) XF_GetKeyin,
  (DWORD) XF_WaitKey,
  (DWORD) XF_YesNo,
  (DWORD) XF_GetKbd,
  (DWORD) XF_SetKbdTO,
  (DWORD) XF_GetKbdTO,
  (DWORD) XF_PackComm,
  (DWORD) XF_PreConnect,
  (DWORD) XF_ConnectOK,
  (DWORD) XF_SendRcvd,
  (DWORD) XF_FlushComm,
  (DWORD) XF_ResetComm,
  (DWORD) XF_GetTermData          , // STIS Func
  (DWORD) XF_GetTermCfg           , 
  (DWORD) XF_GetCardCount         , 
  (DWORD) XF_GetCardTbl           , 
  (DWORD) XF_GetIssuerCount       , 
  (DWORD) XF_GetIssuerTbl         , 
  (DWORD) XF_GetAcqCount          , 
  (DWORD) XF_GetAcqTbl            , 
  (DWORD) XF_GetDescCount         , 
  (DWORD) XF_GetDescTbl           , 
  (DWORD) XF_GetLogoCount         , 
  (DWORD) XF_GetLogoTbl           , 
  (DWORD) XF_GetExParamCount      , 
  (DWORD) XF_GetExtraParam        , 
  (DWORD) XF_GetPending           , 
  (DWORD) XF_SetPending           , 
  (DWORD) XF_GetBatchNo           , 
  (DWORD) XF_SetBatchNo           , 
  (DWORD) XF_SelectAcquirer       , 
  (DWORD) XF_SetROC               , 
  (DWORD) XF_SetTrace             , 
  (DWORD) XF_StisClr              , 
  (DWORD) XF_StisAddTbl           , 
  (DWORD) XF_TermDataClr          , 
  (DWORD) XF_TermDataUpd          , 
  (DWORD) XF_BatSysClear          , // BatchSys Func
  (DWORD) XF_BatchFull            , 
  (DWORD) XF_GetRecCount          , 
  (DWORD) XF_GetBatchRec          , 
  (DWORD) XF_GetRevRec            , 
  (DWORD) XF_ClearRevRec          , 
  (DWORD) XF_SaveBatRec           , 
  (DWORD) XF_UpdateBatRec         , 
  (DWORD) XF_TraceInBatch         , 
  (DWORD) XF_TraceInReversal      , 
  (DWORD) XF_GetAccessCode        , // Misc Func
  (DWORD) XF_SelYesNo             ,
  (DWORD) XF_SelApp               ,
  (DWORD) XF_MerchFunc            ,
  (DWORD) XF_PwrSleep             ,
  (DWORD) NULL,
};


static const BYTE KEcrAppMap[ECR_MAXCMD_SUPPORT] = {
  0x00,    // ECR_PP300_CMD => No App / DLL only
  0x11,    // ECR_EDC_CMD => EDC
  0x12,    // ECR_EPS_CMD => EPS
};
//*****************************************************************************
//  Function        : LoadFile
//  Description     : Load file data to memory.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void LoadFile(void)
{
  char filename[32];
  int  fd;
  BYTE *ptr;

  strcpy(filename, KApmDataFile);
  fd = fOpenMW(filename);
  if (fd >= 0)
  {
    ptr = (BYTE*) MallocMW(sizeof(gFileData));
    MemFatalErr(ptr);
    fReadMW(fd, ptr, sizeof(gFileData));
    memcpy(&gFileData, ptr, sizeof(gFileData));
    fCloseMW(fd);
    FreeMW(ptr);
  }
}

//*****************************************************************************
//  Function        : SaveFile
//  Description     : Save data from memory to file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveFile(void)
{
  char filename[32];
  int  fd;
  BYTE *ptr;

  strcpy(filename, KApmDataFile);
  fd = fOpenMW(filename);
  if (fd < 0)
  {
    fd = fCreateMW(filename, 0);
  }
  if (fd >= 0)
  {
    ptr = (BYTE *)MallocMW(sizeof(gFileData));
    MemFatalErr(ptr);
    memcpy(ptr, &gFileData, sizeof(gFileData));
    fWriteMW(fd, ptr, sizeof(gFileData));
    fCommitAllMW();
    fCloseMW(fd);
    FreeMW(ptr);
  }
}

//*****************************************************************************
//  Function        : VirginInit
//  Description     : Perform virgin initialization.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int VirginInit(void)
{
  struct MW_FILE_HDR file_hdr;
  T_FILE *psInfo, sfile_info;
  int    found;

  Disp2x16Msg(KVirginInit, MW_LINE3, MW_CLRDISP|MW_BIGFONT);

  // Clear files create use MIDWARE
  found = fDSearchMW(TRUE, &file_hdr);
  while (found != -1) {
    fDeleteMW(file_hdr.sb_name);
    found = fDSearchMW(TRUE, &file_hdr);
  }
  fCommitAllMW();


  // Clear File create via OS call
  psInfo = os_file_search_data(K_SearchReset, &sfile_info);
  while (psInfo != NULL) {
    os_file_delete(sfile_info.s_name);
    psInfo = os_file_search_data(K_SearchReset, &sfile_info);
  }
  return 0;
}
//*****************************************************************************
//  Function        : GlobalsInit
//  Description     : Initialization global variables.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GlobalsInit(void)
{
  char filename[32];
  struct TERM_DATA term_data;

  #ifndef PR608D
  #ifdef os_batt_level
  if (os_batt_level()&K_PowerPluggedIn) {
    os_disp_bl_control(0xFF);
  }
  #endif
  #endif

  Disp2x16Msg(KSystemInit, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  EMVUtilStart();
  CTLUtilStart();
  DispLineMW("Init Term Data", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  TermDataInit();
  DispLineMW("Init STIS Data", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  STISInit();
  ExParamInit();
  DispLineMW("Init RemoteDL Data", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  RemoteDlInit();
  DispLineMW("Init Batch", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  if (!BatSysInit(GetAcqCount())) {
    DispLineMW("Batch Init Fail!", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_BIGFONT);
    WaitKey(KBD_TIMEOUT);
    ResetMW();
  }

  // Init Application List
  InitApplList();

  DispLineMW("Init Hardware", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
  InitAllHw();
  DispLineMW("Init Comm", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
  CommInit();
  #if (R700)
  DispLineMW("Init Phone", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
  PhoneInit();
  #endif

  InjectCAKey();

  // Init External Secure Channel
  // Aux Port connect to external pinpad (PP300)
  if (GetTermData(&term_data)) {
    if (term_data.b_ppad_port != 0) {
      GetPortDevName(term_data.b_ppad_port, filename);
      if (!SChnlSetupMW(filename, FALSE)) {   // Do not inject new key
        Disp2x16Msg("EPPAD Init Err! Please Check... ", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
        WaitKey(KBD_TIMEOUT);
      }
      KDLL_ExtPPadSetup(MIDWARE_ID, SCHNLGETPINMW);  // Use External pinpad to get PIN
    }
    else
      KDLL_ExtPPadSetup(0, 0);
  }

  GenDLL();
}
//*****************************************************************************
//  Function        : GlobalsCleanUp
//  Description     : Clean Up all allocated globals.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GlobalsCleanUp(void)
{
  //CloseAllHw();
}
//*****************************************************************************
//  Function        : ECRRequest
//  Description     : Process ECR request if available.
//  Input           : aECRData;     // Buffer for ecrdata.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ECRRequest(struct ECRDATA *aECRData)
{
  struct TERM_DATA term_data;
  T_APP_INFO app_info;
  DWORD ecr_len;

  if (ECR_RxRdy()) {
    ecr_len = ECR_Rcvd(aECRData, sizeof(struct ECRDATA));
    if (ecr_len > 0) {
      //08-09-15 JC ++
      GetTermData(&term_data);
      if (term_data.b_ecr_port & ECR_RAW_MODE) {
        if (KEcrAppMap[ECR_EDC_CMD] != 0x00) {
          if (os_app_info(KEcrAppMap[ECR_EDC_CMD], &app_info)) {
            SwitchAppMW(KEcrAppMap[ECR_EDC_CMD], 0, 0, 0);
            ECR_Clear();
            return TRUE;
          }
        }
        return FALSE;
      }
      //08-09-15 JC --
      switch (aECRData->b_cla) {
        case ECR_PP300_CMD:
          //23-03-18 JC ++
          if (os_app_info(PP300DLL_ID, &app_info)) {
            ECR_Clear();
            ecr_len=PP300Cmd(aECRData, ecr_len);
            if (ecr_len > 0) {
              ECR_Send(aECRData, ecr_len);
              ECR_Clear();
            }
          }
          else
            ECR_Clear();
          //23-03-18 JC --
          break;
        case ECR_EDC_CMD:
          if (KEcrAppMap[ECR_EDC_CMD] != 0x00) {
            if (os_app_info(KEcrAppMap[ECR_EDC_CMD], &app_info)) {
              SwitchAppMW(KEcrAppMap[ECR_EDC_CMD], 0, 0, 0);
              ecr_len = ECR_Rcvd(aECRData, sizeof(struct ECRDATA));
            }
            if ((ecr_len > 0) && (aECRData->b_cla == ECR_EDC_CMD))
              ECR_Clear();
            break;
          }
          // Fall Thru;
        default:
          DispLineMW("APP Not Found!", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
          LongBeep();
          ECR_Clear();
      }
      return TRUE;
    }
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : ChangeApp
//  Description     : Switch to other application.
//  Input           : aId;      // Applcation idx
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ChangeApp(DWORD aId)
{
  while ((aId != 0) && (aId != MY_APPL_ID)) {
    aId = SwitchAppMW(aId, 0, 0, 0);
  }
}
//*****************************************************************************
//  Function        : Coremain
//  Description     : Your application code start here.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int Coremain(DWORD aParam1, DWORD aParam2, DWORD aParam3)
{
  int     select;
  BOOLEAN disp_chg;
  DWORD   keyin;
  T_APP_INFO app_info;
  struct ECRDATA *pECRData = NULL;
  struct TERM_DATA term_data;
  //#if (CLCD_SUPPORT)
  struct  GLOC loc;
  //#endif

  // Init ECR DLL
  //25-07-17 JC ++
  if (InitEcrDll()) {
    pECRData = (struct ECRDATA *)MallocMW(sizeof(struct ECRDATA));
  }
  //25-07-17 JC --

  //#if (!CLCD_SUPPORT)
  if (GetLcdTypeMW() != MW_DISP_COLOR)
    ThreadCreateMW(ShowTxRxLed, 2);
  //#endif

  //25-07-17 JC ++
  if (os_hd_config()&K_HdLanInstalled)
    ThreadCreateMW(HotPlug, 100);
  //25-07-17 JC --

  if (GetSTISMode() == TRANS_MODE) {
    EMVInit();
  }
  AcceptBeep();

  select = -1;
  disp_chg = TRUE;
  TimerSetMW(gTimerHdl[TIMER_DEFAULTAPP], KBD_TIMEOUT);  // Jump to default app after 90s
  while (1)
  {
     SleepMW();
#if ENABLE_DEFAULT_APP
#warning default app enabled!
    LoadFile();
    if (gFileData.bDefaultAppID)
    {
      select = gFileData.bDefaultAppID;
      ChangeApp(select);
      disp_chg = TRUE;
    }
#endif
     // Process ECR Req
     if (pECRData != NULL) {
       if (ECRRequest(pECRData)) {
         disp_chg = TRUE;
       }
     }

     if (disp_chg) {
       // Idle Display
       DispLineMW("Appl.", MW_LINE4+13, MW_CLRDISP|MW_SPFONT);
       DispLineMW("Manager", MW_LINE5+13, MW_SPFONT);
       if (GetLcdTypeMW() == MW_DISP_COLOR) {
         loc.wPosX = 18;
         loc.wPosY = 10;
         loc.wWidth = KSpectraSym[5]*256+KSpectraSym[4];
         loc.wHeight= KSpectraSym[7]*256+KSpectraSym[6];
         ShowGraphicT800(&loc, &KSpectraSym[8]);
       }
       else
         ShowBWLogo(KBWSpectraLogo, sizeof(KBWSpectraLogo));
       #if (R700)
       if (VMailCount() != 0) {
         DispLineMW("You Got Mail!",  MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
       }
       #endif
       disp_chg = FALSE;
     }
     #if (R700)
     #ifndef WIN32
     if (OffHookState()) {
       PhoneFunc(0);
       disp_chg = TRUE;
       TimerSetMW(gTimerHdl[TIMER_DEFAULTAPP], KBD_TIMEOUT);
       continue;
     }
     if (AnswerCall()) {
       disp_chg = TRUE;
       TimerSetMW(gTimerHdl[TIMER_DEFAULTAPP], KBD_TIMEOUT);
       continue;
     }
     #endif
     #endif
     keyin = GetCharMW();
     if (keyin == 0) {
       if (TimerGetMW(gTimerHdl[TIMER_DEFAULTAPP]) != 0)
         continue;
       #if (DOCK_SUPPORT)
       if (!os_onbase()) {
         PowerOffMW();
       }
       #endif
       keyin = MWKEY_CANCL;  // switch app if timeout
     }
     switch (keyin) {
       case MWKEY_POWER:
          DispLineMW("Power Off ?", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
          if (WaitKey(KBD_TIMEOUT) == MWKEY_ENTER) {
            fCommitAllMW();
            PowerOffMW();
          }
          disp_chg = TRUE;
          break;
       #if (PR500)
       case MWKEY_FUNC1:
       #endif
       #if (T800)
       case MWKEY_FUNC1:
       #endif
       #if (T300|T810|A5T1000|T1000)
       case MWKEY_LEFT:
       #endif
       case MWKEY_LEFT1:
       case MWKEY_FUNC:
         #if (R700)
         PhoneHouseKeep();
         #endif
         DispCtrlMW(MW_CLR_DISP);
         MerchFunc(-1, TRUE);
         #if (R700)
         DispLineMW("Init Phone", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
         PhoneInit();
         #endif
         disp_chg = TRUE;
         break;
       case MWKEY_ENTER:   // Change Application
       #if (R700)
       case MWKEY_LEFT3:
       #endif
         select = SelectAppl(MY_APPL_ID);
         if (select != -1) {
           #if (R700)
           PhoneHouseKeep();
           #endif
#if ENABLE_DEFAULT_APP
           if (select != 16)
           {
            gFileData.bDefaultAppID = select;
            SaveFile();
           }
#endif
           ChangeApp(select);
           #if (R700)
           DispLineMW("Init Phone", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
           PhoneInit();
           #endif
         }
         disp_chg = TRUE;
         break;
       case MWKEY_CANCL:   // Switch to default App
         if (GetTermData(&term_data)) {
           if ((term_data.b_default_app != MY_APPL_ID)
               && (os_app_info(term_data.b_default_app, &app_info)) ) {
             ChangeApp(term_data.b_default_app);
           }
         }
         disp_chg = TRUE;
         break;
       #if (R700)
       case MWKEY_LEFT2:
         PhoneSetup();
         break;
       case MWKEY_RIGHT1:
       case MWKEY_RIGHT2:
       case MWKEY_RIGHT3:
       case MWKEY_DN:
         PhoneFunc(keyin);
         disp_chg = TRUE;
         break;
       case MWKEY_UP:
         VMailFunc();
         disp_chg = TRUE;
         break;
       #endif
     }
     TimerSetMW(gTimerHdl[TIMER_DEFAULTAPP], KBD_TIMEOUT);
  }
  if (pECRData != NULL) {
    StopEcrDll();                 //25-07-17 JC ++
    FreeMW(pECRData);
  }
  //#if (!CLCD_SUPPORT)
  if (GetLcdTypeMW() != MW_DISP_COLOR)
    ThreadDeleteMW(ShowTxRxLed);
  //#endif
  //25-07-17 JC ++
  if (os_hd_config()&K_HdLanInstalled)
    ThreadDeleteMW(HotPlug);
  //25-07-17 JC --
  ResetMW();
  return 0;
}

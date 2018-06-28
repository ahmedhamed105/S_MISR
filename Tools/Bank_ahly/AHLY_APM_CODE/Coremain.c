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
#include "system.h"
#include "ecrdll.h"
#include "sysutil.h"
#include "hardware.h"
#include "apm.h"
#include "corevar.h"
#include "constant.h"
#include "func.h"
#include "others.h"
#include "message.h"
#include "emvtrans.h"
#include "input.h"
#include "chkoptn.h"
#include "sale.h"
#include "auth.h"
#include "print.h"
#include "tranutil.h"
#include "ecr.h"
#include "testcfg.h"
#include "ctltrans.h"
#include "coremain.h"
#include "landingzone.h"
#include "iface.h"
#include "network.h" 
#include "ParamTest.h"
#include "lptutil.h"
#include "settle.h"
#if (T300|A5T1000|T1000|TIRO)
#include "apm.h"
#else
#include "apm_lite.h"
#endif

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define RETURN_MASK         0xF0000000
#define RETURN_2_APM        0x80000000

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
BOOLEAN gDispLogo = FALSE;
//ahmed 
int advc=0;
BYTE bytes[]={0x03,0x04};
int fuctiongo=0;
int  setgo=0;
int initstep;

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
const BYTE KSoftName[] = {"Ahly EGP"};        // Application Name

//-----------------------------------------------------------------------------
//      Export Functions
//-----------------------------------------------------------------------------
//void XF_ExportTest(void)                 { os_appcall_return(ExportTest()); }

const DWORD KExportFunc[] =    // Export Function Table
{
  (DWORD) 0,                   // Importance!! Number of export functions!!!
  //(DWORD) 1,                   // Importance!! Number of export functions!!!
  //(DWORD) XF_ExportTest,       // 01
  (DWORD) NULL,
};

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
  int    found;
  struct TERM_DATA    term_data;
  struct ACQUIRER_TBL acq_tbl;
  int    max_acq, i;

  DispLineMW("EDC INIT.", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  DispLineMW("PLEASE WAIT...", MW_LINE5, MW_CENTER|MW_BIGFONT);

  // Clear Up all old files
  found = fDSearchMW(TRUE, &file_hdr);
  while (found != -1) {
    fDeleteMW(file_hdr.sb_name);
    found = fDSearchMW(TRUE, &file_hdr);
  }
  
  if (GetLcdTypeMW()==MW_DISP_COLOR)
    os_config_write(K_CF_LandingZone, landingzone_logo);    // landing zone logo for contactless
  else
    os_config_write(K_CF_LandingZone, landingzone_logo_bw); // landing zone logo (b/w) for contactless
  os_config_update();                                   // write the data into FLASH

  //// Clean old batch record if trans_mode
  APM_GetTermData(&term_data);
  if (term_data.b_stis_mode==TRANS_MODE) {
    max_acq = APM_GetAcqCount();
    for (i = 0; i < max_acq; i++) {
      if (!APM_GetAcqTbl(i, &acq_tbl))
        break;
      if (CorrectHost(acq_tbl.b_host_type)) {
        APM_SetPending(i, NO_PENDING);
        APM_BatchClear(i);
      }
    }
  }
  fCommitAllMW();
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
  // Init Global Memory
  MallocGDS();
  APM_GetTermCfg(&STIS_TERM_CFG);
  APM_GetTermData(&STIS_TERM_DATA);

  // Check All files
  Disp2x16Msg("CHECK EDC FILES! PLEASE WAIT... ", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  DataFileInit();
  InitAllHw();
  NetworkInit();
  IfaceInitData();
  ExtPPadSetup();  // Set EMVDLL to use external PINPAD
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
  FreeGDS();
  CloseAllHw();
  DataFileClose();
  SimStatusThrd(FALSE);
}
//*****************************************************************************
//  Function        : SetupModeProcess
//  Description     : Process for SETUP/INIT MODE.
//  Input           : N/A
//  Return          : 0;        // normal return
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int SetupModeProcess(void)
{
  static const char *KModeText[4] = {
    "SETUP MODE",
    "INIT MODE ",
    "TRANS MODE",
    "KEYS SYNC "
  };
  int ret;

  if (gGDS->b_disp_chgd) {
    gGDS->b_disp_chgd = 0;
    DispLineMW("EDC:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispLineMW(KModeText[STIS_TERM_DATA.b_stis_mode], MW_LINE1+6, MW_BIGFONT);
    DispLineMW(KSoftName, MW_LINE7, MW_BIGFONT);
  }

  ret = 0;
  APM_SetKbdTimeout(KBD_TIMEOUT);
  switch (APM_GetKeyin()) {
    #if (PR608D|TIRO|PR608)
    case MWKEY_UP:
    case MWKEY_DN:
    case MWKEY_LEFT:
    case MWKEY_RIGHT:
    case MWKEY_SELECT:
    #elif (R700|T800)
    case MWKEY_LEFT1:
    case MWKEY_FUNC:
    case MWKEY_FUNC1:
    #endif
    #if (T300|A5T1000|T1000)
    case MWKEY_LEFT:
    #endif
      MerchantFunc(-1);
      break;
    case MWKEY_CANCL:
      RefreshDispAfter(0);
      break;
    case MWKEY_ENTER:
    case MWKEY_LEFT3:
    case MWKEY_FUNC4:
    case MWKEY_POWER:
      ret = APM_SelApp(MY_APPL_ID);
      if ((ret != -1) && (ret != MY_APPL_ID))
        ret |= RETURN_2_APM;
      RefreshDispAfter(0);
  }
  return(ret);
}
//*****************************************************************************
//  Function        : DispDTG
//  Description     : Show data/time or training mode status on idle menu.
//  Input           : aOn;        // TRUE => show ':'
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispDTG(DWORD aOn)
{
  static BYTE hdr_cnt;
  struct DATETIME dtg;
  BYTE  tmp[MW_MAX_LINESIZE+1];

  TimerSetMW(gTimerHdl[TIMER_MAIN],(TrainingModeON()?100:50));

  memset(tmp, 0, sizeof(tmp));
  if ((hdr_cnt > 6) && (TrainingModeON()))
    strcpy(tmp, "*TRAINING  MODE*");
  else {
    ReadRTC(&dtg);
    ConvDateTime(tmp, &dtg, 1);
    memmove(&tmp[3], &tmp[4], 14);
    memmove(&tmp[6], &tmp[7], 10);
    if (aOn == FALSE) {
      if (hdr_cnt % 2)
        tmp[13] = ' ';
    }
  }

  tmp[16] = 0;
  DispLineMW(tmp, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);

  if (++hdr_cnt > 12)
    hdr_cnt = 0;
}
//*****************************************************************************
//  Function        : TransModeDisp
//  Description     : Handle idle menu for TRANS MODE.
//  Input           : TRUE:  Idle screen display;
//                    FALSE: From MENU or ECR
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TransModeDisp(BOOLEAN aIdleMode)
{
  if (TimerGetMW(gTimerHdl[TIMER_DISP]) == 0) {
    if (gGDS->b_disp_chgd) {
      gGDS->b_disp_chgd = FALSE;
      if (aIdleMode) {
        DispCtrlMW(MW_CLR_DISP);
        DispDTG(FALSE);           // display date/time on idle screen
      }
      else
        DispHeader(NULL);         // display specific trans type
      if (CTLEnabled()) {
        CL_LedSet(CTL_LED_TRANS_IDLE);      // blink blue LED only
        if (INPUT.dd_amount==0) {
          if (TIPsReqd())
            Disp2x16Msg(GetConstMsg(EDC_IN_BASE_AMT), MW_LINE3, MW_BIGFONT);
          else
            Disp2x16Msg(GetConstMsg(EDC_IN_ENTER_AMOUNT), MW_LINE3, MW_BIGFONT);
        }
        else {
          TimerSetMW(gTimerHdl[TIMER_DISP], 200);
          gGDS->b_disp_chgd = TRUE;
          if (CtlUnderTransLimit((DWORD)(INPUT.dd_amount))) {
            if (gDispLogo)
              DispDefinedLogo(DP_LOGO_LANDINGZONE, FALSE);
            else {
              DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
              Disp2x16Msg(GetConstMsg(EDC_CO_SWIPE_INS_TAP), MW_LINE5, MW_BIGFONT);
            }
            gDispLogo = ~gDispLogo;
          }
          else {
            DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
            Disp2x16Msg(GetConstMsg(EDC_CO_SWIPE_INS), MW_LINE5, MW_BIGFONT);
          }
        }
      }
      else {
        if (ICCReaderRdy()) {
          if (INPUT.dd_amount==0) {
            Disp2x16Msg(GetConstMsg(EDC_IN_ENTER_AMOUNT), MW_LINE3, MW_BIGFONT);
          }
          else {
            DispAmount(INPUT.dd_amount, MW_LINE3, MW_BIGFONT);
            Disp2x16Msg(GetConstMsg(EDC_CO_SWIPE_INS), MW_LINE5, MW_BIGFONT);
          }
        }
        else
          Disp2x16Msg(GetConstMsg(EDC_CO_REMOVE_ICC), MW_LINE5, MW_BIGFONT);
      }
    }
    if (aIdleMode && (TimerGetMW(gTimerHdl[TIMER_MAIN]) == 0))
      DispDTG(FALSE);
  }
}
//*****************************************************************************
//  Function        : TransModeProcess
//  Description     : Process for TRANS MODE.
//  Input           : N/A
//  Return          : 0;     // Normal return;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int TransModeProcess(void)
{
//  DWORD keyin;
  int  ret;
  BYTE kbdbuf[64];
  BYTE buf[64];
  int host_idx;
  


fuctiongo=0;
 // TransModeDisp(TRUE);

//INPUT.currency=1;

	   os_disp_putc('\f');
        ClearDispMW();
        if (DispParamFile(bytes[advc]) == TRUE) {
 
        }
        else {
			  while(true)
{
   DispCtrlMW(MW_CLR_DISP);
          DispLineMW("Error in Display", MW_LINE3, MW_REVERSE|MW_CENTER|MW_BIGFONT);
		  if(APM_GetKeyin()==MWKEY_ENTER)
			  break;
}
            
        }

		  if(advc==1){
				 
                advc=0;
		  }else{
		  	advc++;
		  }

	


  // EMV transaction
  if (INPUT.dd_amount == 0) {
    if (ICCInserted()) {
      // Close contactless polling
      if (CTLEnabled())
        CL_Close(0);

      DispDTG(TRUE);
      if (PowerOnICC()) {	
	
        EMVTrans(SALE_ICC);
        PowerOffICC();
      }
      else {
        RSP_DATA.w_rspcode = 'R'*256+'E';
        //KC++ Added for MTIP02 Test01 S04 & S05
        DispErrorMsg(GetConstMsg(EDC_RSP_READ_ERROR));
        // do fallback
        INPUT.b_entry_mode = ICC;
        INPUT.b_trans = SALE_ICC;
        if (GetCard(0, TRUE))
          SaleTrans(INPUT.b_entry_mode);
        TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
        //KC--
      }
      while (!ICCReaderRdy()) {
        if (TimerGetMW(gTimerHdl[TIMER_DISP]) == 0) {  // Already Disp Timeout
          DispClrBelowMW(MW_LINE3);
          Disp2x16Msg(GetConstMsg(EDC_CO_REMOVE_ICC), MW_LINE5, MW_BIGFONT);
          gGDS->b_disp_chgd = TRUE;
          WaitICCRemove();
          break;
        }
        if (GetCharMW()==MWKEY_CANCL)
          RefreshDispAfter(0);
      }
      ResetTerm();
      return 0;
    }
  }

  // MSR transaction
  if (INPUT.dd_amount == 0) {
    if (MSRSwiped(&INPUT.s_trk1buf, &INPUT.s_trk2buf)) {
      // Close contactless polling
      if (CTLEnabled())
        CL_Close(0);

      if (INPUT.s_trk2buf.b_len > 0) {
        DispDTG(TRUE);
        if (ValidCard()) {

          if (DefaultCapture())
            SaleTrans(INPUT.b_entry_mode);
          else
            AuthTrans(INPUT.b_entry_mode);


          TransEnd(FALSE);
          ResetTerm();

          return 0;
        }
      }
      else
        RSP_DATA.w_rspcode = 'R'*256+'E';
      TransEnd(FALSE);
      ResetTerm();

      return 0;
    }
  }

  // Contactless transaction
  if (CTLEnabled() && INPUT.dd_amount == 0) {
    if (CTLWaitCard()) {
      CTLTrans(SALE_CTL);
      ResetTerm();

      return 0;
    }
  }


  //setttelment time

os_rtc_get(buf);

	if( memcpy(&buf[8], "06", 2)< 0 && setgo==1){
	 host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=2;
	}else if(memcpy(&buf[8], "06", 2)==0){
	host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=2;
	}else if( memcpy(&buf[8], "12", 2)< 0 && setgo==2){
	 host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=3;
	}else if(memcpy(&buf[8], "12", 2)==0){
	host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=3;
	}else if( memcpy(&buf[8], "18", 2)< 0 && setgo==3){
	 host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=4;
	}else if(memcpy(&buf[8], "18", 2)==0){
	host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=4;
	}else if( memcpy(&buf[8], "00", 2)< 0 && setgo==4){
	 host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=1;
	}else if(memcpy(&buf[8], "00", 2)==0){
	host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      setgo=1;
	}

  if(fuctiongo==1){
	 
      MerchantFunc(-1);

	  ResetTerm();
  }

  if(fuctiongo==2){
  	
       OtherTrans();

      ResetTerm();
	 
  }

   if(fuctiongo==3){
  	
		OtherCurrency();

  }



  return(ret);
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
  struct TERM_DATA term_data;
  T_APP_INFO app_info;
  int ret = 0;

  RefreshDispAfter(0);
  ResetTerm();
  gIdleCnt = FreeRunMark();             //25-07-17 JC ++
  APM_GetTermData(&term_data);
 initstep=1;

  while (1) {
    if (os_app_info(ECRDLL_ID, &app_info)) {   // Check & process valid ECR cmd.
      if ((term_data.b_ecr_port & ECR_RAW_MODE) == 0) {
        // standard ECR command format
        gGDS->i_ecr_len  = ECR_Rcvd(&gGDS->s_ecr_data, sizeof(struct ECRDATA));
        if (gGDS->i_ecr_len > 0) {
          if (gGDS->s_ecr_data.b_cla != ECR_EDC_CMD) {  // return to APM if unknow ECR cmd.
            ret = 0;
            break;
          }
          EcrCmdProcess();
          gGDS->i_ecr_len = 0;
        }
      }
      else {
        // raw ECR command format
        IfaceProcess();
      }
    }
    SleepMW();
    APM_SetKbdTimeout(KBD_TIMEOUT);
    if (STIS_TERM_DATA.b_stis_mode==TRANS_MODE)
      ret=TransModeProcess();
    else
      ret=SetupModeProcess();  /* setup or init mode */
    if ((ret & RETURN_MASK) == RETURN_2_APM)
      break;
  }
  DispLineMW("RETURN TO APM", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
  Disp2x16Msg(GetConstMsg(EDC_CO_PROCESSING), MW_LINE5, MW_BIGFONT);
  return(ret&(~RETURN_2_APM));
}

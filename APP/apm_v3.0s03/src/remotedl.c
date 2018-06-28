/*-----------------------------------------------------------------------------
 *  File          : remoteDl.c
 *  Module        : CREON
 *  Description   : Config Terminal.
 *  Author        : Lewis
 *  Notes         :
 *
 *  Naming conventions
 *  ~~~~~~~~~~~~~~~~~~
 *               Struct : Leading T
 *             Constant : Leading K
 *      Global Variable : Leading g
 *    Function argument : Leading a
 *       Local Variable : All lower case
 *
 *  Date          Who         Action
 *  ------------  ----------- -------------------------------------------------
 *  10 May  2006  Lewis       Initial Version for new CONTROL
 *-----------------------------------------------------------------------------
 */
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "menu.h"
#include "message.h"
#include "kbdutil.h"
#include "infodata.h"
#include "lptutil.h"
#include "termdata.h"
#include "constant.h"
#include "stis.h"
#include "tranutil.h"
#include "comm.h"
#include "apm.h"
#include "remotedl.h"
#include "hardware.h"
#include "ftptran.h"
#include "batchsys.h"


static char KRemoteDLFile[] = { "RDL" };
static int  iRemoteDLFd = -1;


//*****************************************************************************
//  Function        : DeleteRemoteDLFile
//  Description     : Delete Bin files
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DeleteRemoteDLFile(void)
{
  char filename[32];

  strcpy(filename, KRemoteDLFile);
  fDeleteMW(filename);
}

/******************************************************************************
 *  Function        : RemoteDlInit
 *  Description     : Init Remote Dl
 *  Input           : delay in sec
 *  Return          : None
 *  Note            : None
 *  Globals         : None
 *****************************************************************************/
void RemoteDlInit(void)
{
  BYTE filename[32];
  // 16-06-14++ FL (1)
  //DWORD len;
  // 16-06-14-- FL (1)

  strcpy(filename, KRemoteDLFile);

  iRemoteDLFd = fOpenMW(filename);
  while (iRemoteDLFd < 0) {
    fCloseMW(iRemoteDLFd);
    fDeleteMW(filename);
    iRemoteDLFd = fCreateMW(filename, 0);
    RemoteDlResetData();
  }
}

/******************************************************************************
 *  Function        : RemoteDlStartTimer
 *  Description     : Start the polling schedule timer
 *  Input           : delay in sec
 *  Return          : None
 *  Note            : None
 *  Globals         : None
 *****************************************************************************/
static void RemoteDlStartTimer(DWORD aDelayInSec)
{
  TimerSetMW(gTimerHdl[TIMER_REMOTEDL], aDelayInSec*100);
}

/******************************************************************************
 *  Function        : RemoteDownload
 *  Description     : remote update (App or data)
 *  Input           : update: TRUE if perform remote update
 *                  : FALSE only set batch status
 *  Return          : Job performed
 *  Note            : Batch status flag is set regardless
 *  Globals         : cmdBuf
 *****************************************************************************/
void RemoteDownload(void)
{
  // 16-06-14++ FL (1)
  //BYTE scheduler, DL_status, DL_retry_time;
  // 16-06-14-- FL (1)

  struct REMOTEDL_DATA remoteDl;
  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));

  if ((remoteDl.scheduler != DLOAD_NO) && (GetRecCount()==0)) {  // batch all cleared
    DWORD keyin;

    DispClrBelowMW(MW_LINE3);
    DispLineMW(KRemoteDLHeader , MW_LINE1, MW_CLREOL|MW_LEFT|MW_BIGFONT|MW_REVERSE);

    // check prompt
    //if (RemoteDownloadPrompt()) {
    if (TRUE) {
      DispLineMW("PERFORM?  Y OR N" , MW_LINE7, MW_CLREOL|MW_LEFT|MW_BIGFONT);
      keyin = WaitKey(KBD_TIMEOUT);
    }
    else {
      DispLineMW("Starting........" , MW_LINE7, MW_CLREOL|MW_LEFT|MW_SPFONT);
      Delay1Sec(2, 0);
      keyin = MWKEY_ENTER;
    }

    if (keyin == MWKEY_ENTER) {
      remoteDl.DL_retry_time = 0;

      // download params
      remoteDl.DL_status    |= (DL_PARAM_FAIL | DL_APP_FAIL); // Default assume download fail
      WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
      fCommitAllMW();
      DispLineMW(KRemoteDLHeader , MW_LINE1, MW_CLREOL|MW_LEFT|MW_BIGFONT|MW_REVERSE);

      //25-06-14++ SC (1)
      // if (STISFTPDownloadTrans(FALSE, MODE_ALL_PARAM_APP) == TRANS_ACP) {
      if (STISFTPDownloadTrans(FALSE, MODE_ALL_PARAM_APP, FALSE) == TRANS_ACP) {
        //25-06-14-- SC (1)
//        if (GetSTISMode() == TRANS_MODE) {
//          InitEMV();
//          InitApplTbl();
//          InitAcqSupport();
//        }
        remoteDl.scheduler = DLOAD_NO;
        remoteDl.DL_status = 0x00;
        WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
        fCommitAllMW();
        ResetMW();
      }
    } // if keyin == enter
  } // if scheduler != DLOAD_NO

  WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  fCommitAllMW();
}

/******************************************************************************
 *  Function        : RemoteDlSetScheduler
 *  Description     : Set remote update scheduler. (func 67)
 *  Input           : N/A
 *  Return          : TRUE if success
 *  Note            : N/A
 *  Globals Changed : gds
 *******************************************************************************/
BOOLEAN RemoteDlSetScheduler(void)
{
  BYTE kbdbuf[32];
  struct DATETIME dtg;
  struct REMOTEDL_DATA remoteDl;
  WORD wrnd;

  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  // daily polling
  DispClrBelowMW(MW_LINE3);
  DispLineMW(KRemoteDLSch, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
  DispLineMW("1. DISABLE      " , MW_LINE3, MW_CLREOL|MW_LEFT|MW_SPFONT);
  DispLineMW("2. DAILY POLLING" , MW_LINE4, MW_CLREOL|MW_LEFT|MW_SPFONT);

  kbdbuf[0] = 1;
  kbdbuf[1] = (remoteDl.DL_daily == DL_DISABLE) ? MWKEY_1 : MWKEY_2;
  if (!GetKbd(NUMERIC_INPUT+MW_BIGFONT+ECHO+MW_LINE7, IMIN(1)+IMAX(1), kbdbuf)) {
    return FALSE;
  }

  switch (kbdbuf[1]) {
    case MWKEY_1:
      // no scheduler set
      remoteDl.DL_daily = DL_DISABLE;
      WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
      fCommitAllMW();
      return FALSE;
    case MWKEY_2:
      remoteDl.DL_daily = DL_PENDING;
      break;
    default:
      return FALSE;
  }

  // scheduler time
  DispClrBelowMW(MW_LINE3);
  DispLineMW("1. 0 -11:59 A.M."     , MW_LINE3, MW_CLREOL|MW_LEFT|MW_SPFONT);
  DispLineMW("2. 12-23:59 P.M."     , MW_LINE4, MW_CLREOL|MW_LEFT|MW_SPFONT);
  DispLineMW("3. NOW (1 time only)" , MW_LINE5, MW_CLREOL|MW_LEFT|MW_SPFONT);
  kbdbuf[9] = remoteDl.DL_time_slot;
  kbdbuf[8] = 1;
  if (!GetKbd(NUMERIC_INPUT+ECHO+MW_BIGFONT+NULL_ENB+MW_LINE7, IMIN(1)+IMAX(1), &kbdbuf[8]))
    return FALSE;
  switch (kbdbuf[9]) {
    case MWKEY_1:
    case MWKEY_2:
      remoteDl.DL_time_slot = kbdbuf[9]; // 05-07-05++ BF (3)
      RandMW(kbdbuf);
      memcpy( (BYTE *)&wrnd, kbdbuf, 2);
      remoteDl.DL_min   = wrnd%60;
      remoteDl.DL_hour  = wrnd/60;
      remoteDl.DL_hour %= 12;
      if (remoteDl.DL_time_slot == MWKEY_2) // PM
        remoteDl.DL_hour += 12;
      // convert data to BCD format for ease of comparison
      remoteDl.DL_min  = remoteDl.DL_min/10*16 + remoteDl.DL_min%10;
      remoteDl.DL_hour = remoteDl.DL_hour/10*16 + remoteDl.DL_hour%10;
      break;
    case MWKEY_3:
      ReadRTC(&dtg);
      remoteDl.DL_time_slot = kbdbuf[9]; // 05-07-05++ BF (3)
      remoteDl.DL_hour = dtg.bHour;
      remoteDl.DL_min  = dtg.bMinute;
      break;
    default:
      return FALSE;
  }

  WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  fCommitAllMW();
  return TRUE;
}

/******************************************************************************
 *  Function        : RemoteDlSetConfig
 *  Description     : Set remote update scheduler. (func 68)
 *  Input           : N/A
 *  Return          : TRUE if success
 *  Note            : N/A
 *  Globals Changed : gds
 *******************************************************************************/
BOOLEAN RemoteDlSetConfig(void)
{
//  BYTE kbdbuf[8];
//  BYTE scheduler, DL_status;
//
  struct REMOTEDL_DATA  remoteDL_data;
  // 16-06-14++ FL (1)
  //BYTE buffer[24];
  //WORD wrnd;
  // 16-06-14-- FL (1)

//  scheduler = ProtectGetB(&gPRAM->remoteDL_data.scheduler);

  ReadSFile(iRemoteDLFd, 0, &remoteDL_data, sizeof(struct REMOTEDL_DATA));

//
//  if (A21_PSWGet(NULL, P_SETTLE) == PASSWORD_OK) {
//    ClrBelow(LINE3);
  DispClrBelowMW(MW_LINE3);
//    DispCMsg(GetConstMsg(A6_RD_MENU1), LINE1, 16+REVERSE);
//    DispMsg("PROMPT DL. MSG.?", LINE3);
//    DispMsg("0 - NO   1 - YES", LINE5);
//////      DispLineMW(KRemoteDLSch, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
//////      DispLineMW("PROMPT DL. MSG.?", MW_LINE3, MW_CENTER|MW_BIGFONT);
//////      DispLineMW("0 - NO   1 - YES", MW_LINE5, MW_CENTER|MW_BIGFONT);
//////
//////    kbdbuf[0] = 1;
//////    kbdbuf[1] = (gGDS.stis_data.term_cfg.reserved[10] & 0x08? '1' : '0');
//////    if (!GetKbd(NUMERIC_INPUT+ECHO+LINE7, IMIN(1)+IMAX(1), kbdbuf))
//////      return FALSE;

//    if (kbdbuf[1] == '0')
//      gGDS.stis_data.term_cfg.reserved[10] &= ~0x08;
//    else
//      gGDS.stis_data.term_cfg.reserved[10] |= 0x08;
//    UpdTermCfg();
//
//    // 2. Set remote download options (disable, app/param or both)
//    ClrBelow(LINE3);
//    DispEMsg("1. NONE",             LINE3, 7);
//    DispEMsg("2. APP & PARAM",      LINE4, 14);
  DispLineMW("1. NONE"       , MW_LINE3, MW_CLREOL|MW_CENTER|MW_BIGFONT);
  DispLineMW("2. APP & PARAM", MW_LINE4, MW_CLREOL|MW_CENTER|MW_BIGFONT);
//    kbdbuf[0] = 1;
//    kbdbuf[1] = (scheduler == 0x00) ? '1' : scheduler+1;
//    if (!GetKbd(NUMERIC_INPUT+ECHO+LINE7, 1, kbdbuf))
//      return FALSE;
//    switch (kbdbuf[1]) {
//    case MWKEY_1:
//      scheduler = DLOAD_NO;
//      break;
//    case MWKEY_2:
//      scheduler = DLOAD_ALL;
//      break;
//    }
//    if (kbdbuf[1] >= MWKEY_2 || kbdbuf[1] <= MWKEY_4)
//      DL_status = 0; // Reset download status if manually assign the job again.
//    ProtectPutB(&gPRAM->remoteDL_data.scheduler, scheduler);
//    ProtectPutB(&gPRAM->remoteDL_data.DL_status, DL_status);
//  } // password_ok
//  else {
//    return FALSE;
//  }
//
//  SaveFsRemoteDlParam();               // 12-02-08 CP (3)
  return TRUE;
}

/******************************************************************************
 *  Function        : RemoteDlFailHdl
 *  Description     : Remote download fail handler
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 *******************************************************************************/
void RemoteDlFailHdl(void)
{
  struct REMOTEDL_DATA remoteDl;

  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  remoteDl.DL_retry_time = (remoteDl.DL_retry_time >= DL_FAIL_RETRY) ? DL_FAIL_RETRY : remoteDl.DL_retry_time+1;

  if ((remoteDl.DL_retry_time >= DL_FAIL_RETRY) && (GetRecCount()==0)) {  // empty batch
    remoteDl.DL_retry_time = 0;
    WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
    if (remoteDl.DL_status != 0x00) {
      if (remoteDl.scheduler != DLOAD_NO) {
        //25-06-14++ SC (1)
        //if (STISFTPDownloadTrans(FALSE, MODE_ALL_PARAM_APP) == TRANS_ACP) {
        if (STISFTPDownloadTrans(FALSE, MODE_ALL_PARAM_APP, FALSE) == TRANS_ACP) {
          //25-06-14-- SC (1)

//          if (GetSTISMode() == TRANS_MODE) {
//            InitEMV();
//            InitApplTbl();
//            InitAcqSupport();
//          }
          remoteDl.scheduler = DLOAD_NO;
          remoteDl.DL_status = 0x00;
          WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
          fCommitAllMW();
          ResetMW();
        }
      }
    }
  } // if empty batch

  WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
}

/******************************************************************************
 *  Function        : RemoteDlChkScheduler
 *  Description     : Check remote update scheduler.
 *  Input           : N/A
 *  Return          : TRUE if polling started
 *  Note            : N/A
 *  Globals Changed : gds
 *******************************************************************************/
BYTE RemoteDlChkScheduler(void)
{
  WORD wrnd;
  BYTE result = FALSE;
  BYTE tmp[8];
  struct DATETIME dtg;
  struct REMOTEDL_DATA remoteDl;

  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));

  // no key update if training on
  if ((GetSTISMode() != SETUP_MODE) && !TrainingModeON()) {
    ReadRTC(&dtg);

    switch (remoteDl.DL_daily){
      case DL_DONE: // already done
        //only change flag in non-scheduled time-slot
        if ((remoteDl.DL_time_slot == MWKEY_1 && dtg.bHour == 0x23) ||
            (remoteDl.DL_time_slot == MWKEY_2 && dtg.bHour == 0x11)) { // KEY1 = AM, KEY2 = PM
          remoteDl.DL_daily = DL_PENDING;       // clear flag
        }
//16-07-14++ SC (2)
//        // New requirement: if any application change detected, upload the application list via the daily polling
//        if (AppListUploadReq() == APPL_UPLOAD_PENDING) {
        if (GetRDLForceUpload()) {
//16-07-14-- SC (2)
          remoteDl.DL_daily = DL_PENDING;       // clear flag
        }
        break;
      case DL_PENDING:  // daily schedule
        if ( ((dtg.bHour == remoteDl.DL_hour) && (dtg.bMinute >= remoteDl.DL_min))  // time due
             || ((dtg.bHour > remoteDl.DL_hour) && ((dtg.bHour < 0x12) || (remoteDl.DL_hour > 0x11))) ){ //dtg is in BCD
          STISFTPPolling(&remoteDl);
          result   = TRUE;
        }
//16-07-14++ SC (2)
//        // New requirement: if any application change detected, upload the application list via the daily polling
//        if (AppListUploadReq() == APPL_UPLOAD_PENDING) {
        else if (GetRDLForceUpload()) {
//16-07-14-- SC (2)
          STISFTPPolling(&remoteDl);
          result   = TRUE;
        }
        remoteDl.DL_force_upload = 0x00; //force upload triggered, reset flag and perform the uplaod according to the schedule next time
        break;
//16-07-14++ SC (2)
//In direct logic, the terminal should have re-schedule the checkin/polling time in retry 3 if the application list upload
//is not required.
//However, for simplicity, here we just ignore the 3rd to 5th retry to wait remoteDl.DL_daily to count to DL_FAIL.
      case DL_RETRY3:
      case DL_RETRY4:
      case DL_RETRY5:
        if (AppListUploadReq() != APPL_UPLOAD_PENDING) {
          remoteDl.DL_daily++;
          break;
        }
        //drop through intentionally
//16-07-14-- SC (2)
      case DL_RETRY1: // retry count reach
      case DL_RETRY2:
        STISFTPPolling(&remoteDl);
        result   = TRUE;
        break;
      case DL_DISABLE:  // no schedule set
        break;
      case DL_FAIL:
//16-07-14++ SC (2)
//        if (AppListUploadReq() == APPL_UPLOAD_PENDING) {
//          remoteDl.DL_daily = DL_PENDING; // Let the loop to restart to force the upload is made every 5 minutes
//          STISFTPPolling(&remoteDl);
//        } else
//16-07-14-- SC (2)
        if ((remoteDl.DL_time_slot == MWKEY_1 && dtg.bHour == 0x23) ||
            (remoteDl.DL_time_slot == MWKEY_2 && dtg.bHour == 0x11)) { // KEY1 = AM, KEY2 = PM
          RandMW(&tmp);
          memcpy((BYTE *)&wrnd, &tmp, 2);
          remoteDl.DL_min   = wrnd%60;
          remoteDl.DL_hour  = wrnd/60;
          remoteDl.DL_hour %= 12;
          if (remoteDl.DL_time_slot == MWKEY_2) // PM
            remoteDl.DL_hour += 12;
          // convert data to BCD format for ease of comparison
          remoteDl.DL_min   = remoteDl.DL_min/10*16  + remoteDl.DL_min%10;
          remoteDl.DL_hour  = remoteDl.DL_hour/10*16 + remoteDl.DL_hour%10;
          remoteDl.DL_daily = DL_PENDING; // clear flag
          WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
        }
        else if (remoteDl.DL_time_slot == MWKEY_3) {
          remoteDl.DL_daily = DL_DISABLE;
        }
        break;
      default:  // waiting for next retry or give up
        remoteDl.DL_daily ++; // increment every 1s
        break;
    }
  }

  RemoteDlStartTimer(POLLING_INTERVAL);
  WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  fCommitAllMW();
  return result;
}

/******************************************************************************
 *  Function        : RemoteDlResetData
 *  Description     : remote download initialization
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : gPram
 *******************************************************************************/
void RemoteDlResetData(void)
{
  struct REMOTEDL_DATA  remoteDL_data;
  BYTE buffer[24];
  WORD wrnd;

  memset(&remoteDL_data, 0, sizeof(struct REMOTEDL_DATA));

  //default enable daily polling
  remoteDL_data.DL_daily = DL_DISABLE;    //for fail safe, disable it
#if 0
  remoteDL_data.DL_daily = DL_PENDING;
  remoteDL_data.DL_time_slot = MWKEY_1;
#endif
  RandMW(buffer);
  memcpy((BYTE *)&wrnd, buffer, 2);
  remoteDL_data.DL_min   = wrnd%60;
  remoteDL_data.DL_hour  = wrnd/60;
  remoteDL_data.DL_hour %= 12;
  // convert data to BCD format for ease of comparison
  remoteDL_data.DL_min  = remoteDL_data.DL_min/10*16 + remoteDL_data.DL_min%10;
  remoteDL_data.DL_hour = remoteDL_data.DL_hour/10*16 + remoteDL_data.DL_hour%10;
  remoteDL_data.scheduler = DLOAD_NO;

  remoteDL_data.DL_force_upload = 0x00;

  WriteSFile(iRemoteDLFd, 0, &remoteDL_data, sizeof(struct REMOTEDL_DATA));
  fCommitAllMW();
}

/******************************************************************************
 *  Function        : SetRemoteDownload
 *  Description     : export function for app to set download schedule:
 *                    #define DLOAD_NO      0x00  // nothing to dload
 *                    #define DLOAD_PARAM   0x31  // remote update parameter
 *                    #define DLOAD_APP     0x32  // remote udpate app
 *                    #define DLOAD_ALL     0x33  // remote update app & parameter
 *  Input           : new schedule
 *  Return          : TRUE if success, FALSE otherwise
 *  Note            : N/A
 *  Globals Changed : gPram
 *******************************************************************************/
BOOLEAN SetRemoteDownload(BYTE aDload)
{
  struct REMOTEDL_DATA remoteDl;

  if ((aDload == DLOAD_NO) || ((aDload >= DLOAD_PARAM) && (aDload <= DLOAD_ALL))) {
    ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
    remoteDl.scheduler = aDload;
    WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
    return TRUE;
  }
  return FALSE;
}
/******************************************************************************
 *  Function        : SetRDLForceUpload
 *  Description     : set the force upload flag
 *  Input           : new force upload flag
 *  Return          : N/A
 *  Note            : //16-07-14++ SC (2)
 *  Globals Changed : N/A
 *******************************************************************************/
void SetRDLForceUpload(BYTE aForceUpload)
{
  struct REMOTEDL_DATA remoteDl;

  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  remoteDl.DL_force_upload = aForceUpload;
  WriteSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
}
/******************************************************************************
 *  Function        : GetRDLForceUpload
 *  Description     : get the force upload flag
 *  Input           : N/A
 *  Return          : force upload flag
 *  Note            : //16-07-14++ SC (2)
 *  Globals Changed : N/A
 *******************************************************************************/
BYTE GetRDLForceUpload(void)
{
  struct REMOTEDL_DATA remoteDl;

  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));
  return remoteDl.DL_force_upload;
}
/******************************************************************************
 *  Function        : SetRemoteDLAllowAtSetupMode
 *  Description     : Set &gPRAM->virgin_remoteDL
 *  Input           : BOOLEAN
 *  Return          : N/A
 *  Note            : 06-03-08 CP (4)
 *  Globals Changed : gPram
 *******************************************************************************/
void SetRemoteDLAllowAtSetupMode(BOOLEAN aAllow)
{
//  if (aAllow)
//    ProtectPutW(&gPRAM->virgin_remoteDL, MAGIC);
//  else
//    ProtectPutW(&gPRAM->virgin_remoteDL, 0);
}
/******************************************************************************
 *  Function        : GetRemoteDLAllowAtSetupMode
 *  Description     : return &gPRAM->virgin_remoteDL
 *  Input           : N/A
 *  Return          : TRUE if MAGIC
 *  Note            : 06-03-08 CP (4)
 *  Globals Changed : gPram
 *******************************************************************************/
BOOLEAN GetRemoteDLAllowAtSetupMode(void)
{
//  return (ProtectGetW(&gPRAM->virgin_remoteDL) == MAGIC);
  return FALSE;
}

#if DEBUG_MODE
/******************************************************************************
 *  Function        : ShowPollingTime
 *  Description     :
 *  Input           : N/A
 *  Return          :
 *  Note            :
 *  Globals Changed :
 *******************************************************************************/
void ShowPollingTime(void)
{
  BYTE tmpbuf[128];
  struct REMOTEDL_DATA  remoteDl;

  ReadSFile(iRemoteDLFd, 0, &remoteDl, sizeof(struct REMOTEDL_DATA));

  sprintf(tmpbuf, "PTime:%02x:%02x,%02x,%02x,%02x", remoteDl.DL_hour, remoteDl.DL_min, remoteDl.scheduler, remoteDl.DL_status,remoteDl.DL_daily);
  DispLineMW(tmpbuf, MW_LINE7, MW_SPFONT);
  sprintf(tmpbuf, "T_REMOTEDL:%06X", TimerGetMW(gTimerHdl[TIMER_REMOTEDL]));
  DispLineMW(tmpbuf, MW_LINE8, MW_SPFONT);
}
#endif


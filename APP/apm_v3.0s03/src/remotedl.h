/*-----------------------------------------------------------------------------
 *  File          : remotedl.h
 *  Module        : CREON
 *  Description   : Declrartion & Defination for remotedl.C
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
#ifndef _REMOTEDL_H_
#define _REMOTEDL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//  Common Defines
//-----------------------------------------------------------------------------
#define DLOAD_NO      0x00  // nothing to dload
#define DLOAD_PARAM   0x31  // remote update parameter
#define DLOAD_APP     0x32  // remote udpate app
#define DLOAD_ALL     0x33  // remote update app & parameter  14-06-05++ BF
#define DL_PENDING    0x00  // waiting for next download
//16-07-14++ SC (2)
//#define DL_FAIL       0x0F  // 05-07-05++ retry fail
//#define DL_DONE       0x10  // download okay
//#define DL_RETRY1     0x05  // 03-03-05++ 1st retry
//#define DL_RETRY2     0x0A  // 03-03-05++ 2nd retry
#define DL_RETRY1     0x05  // 03-03-05++ 1st retry
#define DL_RETRY2     0x0A  // 03-03-05++ 2nd retry
#define DL_RETRY3     0x0F
#define DL_RETRY4     0x14
#define DL_RETRY5     0x19
#define DL_FAIL       0x1E  // 05-07-05++ retry fail
#define DL_DONE       0x1F  // download okay
//16-07-14-- SC (2)
#define DL_DISABLE    0xFF
#define DL_CHK_FREQ   60    // scheduler check freqency
#define DL_PARAM_FAIL 0x01  // Parameter download fail
#define DL_APP_FAIL   0x02  // Application download fail
#define DL_FAIL_RETRY 300/DL_CHK_FREQ

#define POLLING_INTERVAL             60  // unit in second(s)

// Main remote download structure
struct  REMOTEDL_DATA {
  BYTE              DL_hour;        // daily scheduler time
  BYTE              DL_min;
  BYTE              DL_time_slot;   // 05-07-05++
  BYTE              DL_daily;       // daily polling status
  BYTE              scheduler;      // scheduler option (0xFF=no update; '1'=param; '2'=app d/n; '3'=param & app)
  BYTE              DL_status;      // Download status
  BYTE              DL_result;      // Download result (get from sys_remote_dll_result())
  BYTE              DL_retry_time;  // Download retry time increment every DLOAD_TIMER overflow (DL_CHK_FREQ)
  BYTE              DL_force_upload;// Force upload application list immediately
};

//-----------------------------------------------------------------------------
//  Functions
//-----------------------------------------------------------------------------
extern void DeleteRemoteDLFile(void);
extern void RemoteDlInit(void);
extern void RemoteDownload(void);
extern void RemoteDlFailHdl(void);
extern BYTE RemoteDlChkScheduler(void);
extern void RemoteDlChkResult(void);
extern void RemoteDlResetData(void);
extern BOOLEAN RemoteDlSetScheduler(void);
extern BOOLEAN RemoteDlSetConfig(void);
extern void SetRemoteDLAllowAtSetupMode(BOOLEAN aAllow);
extern BOOLEAN GetRemoteDLAllowAtSetupMode(void);
extern BOOLEAN SetRemoteDownload(BYTE aDload);
extern void SetRDLForceUpload(BYTE aForceUpload);
extern BYTE GetRDLForceUpload(void);
#if DEBUG_MODE
extern void ShowPollingTime(void);
#endif
#endif //_REMOTEDL_H_

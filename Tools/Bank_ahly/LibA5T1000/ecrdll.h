//-----------------------------------------------------------------------------
//  File          : ECRDLL.h
//  Module        : 
//  Description   : Header for for ECRDLL export functions.
//  Author        : Lewis
//  Notes         :
//
//  Naming conventions
//  ~~~~~~~~~~~~~~~~~~
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  17 Sept 2008  Lewis       Initial Version.
//  10 Nov  2008  Lewis       V1.1   
//                            - Support message sequence number.
//                            - Bug Fix.
//  24 Feb  2009  Lewis       V1.2 
//                            - Add USB support.
//-----------------------------------------------------------------------------
#ifndef _INC_ECRDLL_H_
#define _INC_ECRDLL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define ECRDLL_ID          0x7D
#define MAX_ECR_BUF        1536

struct ECRDATA {
  BYTE b_cla;
  BYTE b_cmd;
  BYTE sb_content[MAX_ECR_BUF];
};

// ECR class
enum {
  ECR_PP300_CMD=0x00,
  ECR_EDC_CMD       ,
  ECR_EPS_CMD       ,
 ECR_MAXCMD_SUPPORT,
};

//-----------------------------------------------------------------------------
//   ECRDLL Functions ID
//-----------------------------------------------------------------------------
enum {
  ECR_START                   , // 01       Kbd Func
  ECR_STOP                    , // 02
  ECR_SEND                    , // 03
  ECR_RCVD                    , // 04
  ECR_CLEAR                   , // 05
  ECR_RXRDY                   , // 06
  ECR_TXRDY                   , // 07
  ECR_FUNC_COUNT                // Must be the last item
};

//-----------------------------------------------------------------------------
//  API
//-----------------------------------------------------------------------------
// PR608D
// ECR_AUX1 => ECR1
// ECR_AUX2 => ECR2
// ECR_USBS => Usb Serial
// ECR_AUXD => ECRD
// ECR_LAN  => LAN
// ECR_WIFI => Wifi
// ECR_BT   => Bluetooth
#define ECR_AUX1            0x01
#define ECR_AUX2            0x02
#define ECR_USBS            0x04
#define ECR_AUXD            0x08
#define ECR_LAN             0x10
#define ECR_WIFI            0x20
#define ECR_BT              0x0100

#define ECR_USE_SEQ         0x80
#define ECR_RAW_MODE        0x40
#define ECR_SSL_SVR_AUTH    0x8000        // over BT only
#define ECR_SSL_CLN_AUTH    0x4000        // over BT only

//extern int ECRStart(int aPort);
#define ECR_Start(x)         (int) lib_app_call(ECRDLL_ID, ECR_START, (DWORD)x, (DWORD)0, (DWORD)0)

//extern int ECRStop(void);
#define ECR_Stop()           (int) lib_app_call(ECRDLL_ID, ECR_STOP, (DWORD)0, (DWORD)0, (DWORD)0)

//extern int ECRSend(void *aDat, int aLen);
#define ECR_Send(x, y)       (int) lib_app_call(ECRDLL_ID, ECR_SEND, (DWORD)x, (DWORD)y, (DWORD)0)

//extern int ECRRcvd(void *aDat, int aLen);
#define ECR_Rcvd(x, y)       (int) lib_app_call(ECRDLL_ID, ECR_RCVD, (DWORD)x, (DWORD)y, (DWORD)0)

//extern void ECRClear(void);
#define ECR_Clear()          lib_app_call(ECRDLL_ID, ECR_CLEAR, (DWORD)0, (DWORD)0, (DWORD)0)

//extern int ECRRxRdy(void);
#define ECR_RxRdy()          (int) lib_app_call(ECRDLL_ID, ECR_RXRDY, (DWORD)0, (DWORD)0, (DWORD)0)

//extern int ECRTxRdy(void);
#define ECR_TxRdy()          (int) lib_app_call(ECRDLL_ID, ECR_TXRDY, (DWORD)0, (DWORD)0, (DWORD)0)

#endif // _INC_ECRDLL_H_


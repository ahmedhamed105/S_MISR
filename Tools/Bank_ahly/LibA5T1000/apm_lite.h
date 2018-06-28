//-----------------------------------------------------------------------------
//  File          : apm.h
//  Module        : ARM9Term
//  Description   : Header for APM.
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_MANAGER_LITE_H_
#define _INC_MANAGER_LITE_H_
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include <string.h>
#include "common.h"
#include "midware.h"
#include "hwdef.h"


//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define APM_ID          0x10


//-----------------------------------------------------------------------------
//   Application manager Functions define 
//-----------------------------------------------------------------------------
enum {
  APM_GETHWHANDLE             , // 01       Misc Func
  APM_CLRKEY                  , // 02       Kbd Func
  APM_GETKEY                  , // 03
  APM_WAITKEY                 , // 04
  APM_YESNO                   , // 05
  APM_GETKBD                  , // 06
  APM_SETKBDTO                , // 07
  APM_GETKBDTO                , // 08
  APM_PACKCOMM                , // 09       Comm Func
  APM_PRECONNECT              , // 10       
  APM_CONNECTOK               , // 11       
  APM_SENDRCVD                , // 12       
  APM_FLUSHCOMM               , // 13
  APM_RESETCOMM               , // 14
  APM_GETACCESSCODE           , // 15
  APM_SELYESNO                , // 16
  APM_SELAPP                  , // 17
  APM_MERCHFUNC               , // 18
  APM_PWRSLEEP                , // 19
  APM_FUNC_COUNT                // Must be the last item
};

//-----------------------------------------------------------------------------
//  Misc Functions
//-----------------------------------------------------------------------------
enum {
  APM_SDEV_MSR  = 0 ,
  APM_SDEV_ICC      ,
  APM_SDEV_GPRS     ,
  APM_SDEV_PPP      ,
  APM_SDEV_CDC      ,
  APM_SDEV_MDM      ,
  APM_SDEV_WIFI     ,
  APM_SDEV_MAX
};

// extern int GetHwHandle(DWORD aHwId, DWORD aClose);
#define APM_GetHwHandle(x)      (int) lib_app_call(APM_ID, APM_GETHWHANDLE, (DWORD)x, (DWORD)0, (DWORD)0)
#define APM_CloseHwHandle(x)    (int) lib_app_call(APM_ID, APM_GETHWHANDLE, (DWORD)x, (DWORD)1, (DWORD)0)

//-----------------------------------------------------------------------------
//  Kbd Functions
//-----------------------------------------------------------------------------
// Define time out value
#define TIME_1SEC       100
#define KBD_TIMEOUT     (90*TIME_1SEC)      // 90 sec

//extern void SetKbdTimeout(DWORD aTimeout);
#define APM_SetKbdTimeout(x)            lib_app_call(APM_ID, APM_SETKBDTO, (DWORD)x, (DWORD)0, (DWORD)0)

//extern DWORD GetKbdTimeout(void);
#define APM_GetKbdTimeout()             lib_app_call(APM_ID, APM_GETKBDTO, (DWORD)0, (DWORD)0, (DWORD)0)

#define KEY_ALPHA       MWKEY_FUNC1

// WaitKey Check List
#define WAIT_ICC_INSERT 0x0100
#define WAIT_ICC_REMOVE 0x0200
#define WAIT_MSR_READY  0x0400

// extern void ClearKeyin(void);
#define APM_ClearKeyin()                lib_app_call(APM_ID, APM_CLRKEY, (DWORD)0, (DWORD)0, (DWORD)0)

// extern DWORD GetKeyin(void);
#define APM_GetKeyin()                  lib_app_call(APM_ID, APM_GETKEY, (DWORD)0, (DWORD)0, (DWORD)0)

// extern DWORD WaitKey(DWORD aTimeout, DWORD aCheckList);
#define APM_WaitKey(x,y)                lib_app_call(APM_ID, APM_WAITKEY, (DWORD)x, (DWORD)y, (DWORD)0)

//extern DWORD YesNo(void);
#define APM_YesNo()                     lib_app_call(APM_ID, APM_YESNO, (DWORD)0, (DWORD)0, (DWORD)0)

// GetKbd Mode
#define NUMERIC_INPUT   0x00000000
#define AMOUNT_INPUT    0x00010000
#define ALPHA_INPUT     0x00020000
#define HIDE_NUMERIC    0x00030000
#define HEX_INPUT       0x00040000
#define BINARY_INPUT    0x00050000
#define ALPHA_INPUT_NEW 0x00060000
#define HEX_INPUT_NEW   0x00070000
#define DECIMAL_INPUT   0x00080000
// GetKbd Option
#define JUSTIFIED       0x00100000
#define ECHO            0x00200000
#define PREFIX_ENB      0x00400000
#define NULL_ENB        0x00800000
// GetKbd Decimal Pos
#define DECIMAL_NONE    0x00000000
#define DECIMAL_POS1    0x01000000
#define DECIMAL_POS2    0x02000000
#define DECIMAL_POS3    0x03000000

#define MAX_INPUT_LEN   (MW_MAX_LINESIZE*2)
#define PREFIX_SIZE      4
#define MAX_INPUT_PREFIX   (MAX_INPUT_LEN - PREFIX_SIZE)

#define IMIN(x)         (x<<8)
#define IMAX(x)         (x&0xFF)

#define RIGHT_JUST      (0xFF)

// aEntryMode = Decimal_pos | Option | Mode | MW_xxFONT | MW_LINEx | x position (0xFF=>Right Justify)
// aLen       = IMIN(a) + IMAX(b)
extern BOOLEAN GetKbd(DWORD aEntryMode, DWORD aLen, BYTE *aOutBuf);
#define APM_GetKbd(x,y,z)               (BOOLEAN) lib_app_call(APM_ID, APM_GETKBD, (DWORD)x, (DWORD)y, (DWORD)z)


//-----------------------------------------------------------------------------
//  Comm Functions
//-----------------------------------------------------------------------------
// Comm Mode
#define APM_COMM_SYNC         0
#define APM_COMM_ASYNC        1
#define APM_COMM_TCPIP        2
#define APM_COMM_GPRS         3
#define APM_COMM_AUX          4
#define APM_COMM_WIFI         5

// Comm Return Value
#define COMM_OK               ('0'*256+'0')
#define COMM_CANCEL           ('C'*256+'N')
#define COMM_NO_DIALTONE      ('D'*256+'N')
#define COMM_HOST_OFF         ('H'*256+'O')
#define COMM_LINEBUSY         ('L'*256+'B')
#define COMM_LINEDROP         ('L'*256+'C')
#define COMM_NO_LINE          ('L'*256+'N')
#define COMM_LINEOCCP         ('L'*256+'O')
#define COMM_NOT_CONNECT      ('N'*256+'C')
#define COMM_TIMEOUT          ('T'*256+'O')

struct MDMSREG {
  BYTE bLen;
  struct MW_MDM_PORT sMdmCfg;
};

struct TCPSREG {
  BYTE bLen;
  BYTE bAdd2ByteLen;
  struct MW_TCP_PORT2 sTcpCfg;
};

struct AUXSREG {
  BYTE bLen;
  BYTE bPort;
  struct MW_AUX_CFG sAuxCfg;
};
enum {
  APM_AUX1  = 0,
  APM_AUX2     ,
  MAX_APM_AUX  ,
};

struct PPPSREG {
  BYTE  bKeepAlive;       // TRUE => PPP connection never close.
  DWORD dDevice;
  DWORD dSpeed;
  DWORD dMode;
  char  scAPN[128];
  char  scUserID[128];
  char  scPwd[128];
  struct MW_EXPECT_SEND *psLogin;
  DWORD dLoginPair;
  struct MW_EXPECT_SEND *psDialUp;
  DWORD dDialupPair;
  struct MW_MDM_PORT    *psDialParam;
};

struct COMMPARAM {
  WORD wLen;              // size of comm param (sizeof(struct COMMPARAM))
  BYTE bCommMode;         // COMM mode
  BYTE bHostConnTime;     // host connection timeout in seconds
  BYTE bTimeoutVal;       // host response timeout in seconds
  BYTE bAsyncDelay;       // delay for async after connection in seconds 
  struct MDMSREG sMdm;
  struct TCPSREG sTcp;
  struct AUXSREG sAux;
  struct PPPSREG sPPP;
};

#define COMMBUF_SIZE      3*1024
struct COMMBUF {
  WORD wLen;
  BYTE sbContent[COMMBUF_SIZE];
};

// Return Value
#define COMM_OK               ('0'*256+'0')
#define COMM_NO_DIALTONE      ('D'*256+'N')
#define COMM_HOST_OFF         ('H'*256+'O')
#define COMM_LINEBUSY         ('L'*256+'B')
#define COMM_LINEDROP         ('L'*256+'C')
#define COMM_NO_LINE          ('L'*256+'N')
#define COMM_LINEOCCP         ('L'*256+'O')
#define COMM_NOT_CONNECT      ('N'*256+'C')
#define COMM_TIMEOUT          ('T'*256+'O')

//extern void PackComm(struct COMMPARAM *aDat);
#define APM_PackComm(x)      lib_app_call(APM_ID, APM_PACKCOMM, (DWORD)x, 0, 0)

//extern void PreConnect(void);
#define APM_PreConnect()     lib_app_call(APM_ID, APM_PRECONNECT, (DWORD)0, (DWORD)0, (DWORD)0)

//extern DWORD ConnectOK(BOOLEAN aNoCancel);
#define APM_ConnectOK(x)     lib_app_call(APM_ID, APM_CONNECTOK, (DWORD)x, (DWORD)0, (DWORD)0)

//extern DWORD SendRcvd(struct COMMBUF *aSendMsg, struct COMMBUF *aRcvdMsg);
#define APM_SendRcvd(x, y)   lib_app_call(APM_ID, APM_SENDRCVD, (DWORD)x, (DWORD)y, (DWORD)0)

//extern void FlushConnection(void);
#define APM_FlushComm()      lib_app_call(APM_ID, APM_FLUSHCOMM, (DWORD)0, (DWORD)0, (DWORD)0)

//extern DWORD ResetComm(void);
#define APM_ResetComm()      lib_app_call(APM_ID, APM_RESETCOMM, (DWORD)0, (DWORD)0, (DWORD)0)

//-----------------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------------
//extern BOOLEAN GetAccessCode(void);
#define APM_GetAccessCode()      (BOOLEAN) lib_app_call(APM_ID, APM_GETACCESSCODE, 0, 0, 0)

//extern BOOLEAN APM_SelYesNo(void);
#define APM_SelYesNo()           lib_app_call(APM_ID, APM_SELYESNO, 0, 0, 0)

//extern BOOLEAN APM_SelApp(DWORD aIgnoreID);
#define APM_SelApp(x)            lib_app_call(APM_ID, APM_SELAPP, x, 0, 0)

//extern BOOLEAN APM_MerchFunc(DWORD aId);
#define APM_MerchFunc(x)         lib_app_call(APM_ID, APM_MERCHFUNC, x, 0, 0)

//extern int APM_PwrSleep(DWORD aSec);
#define APM_PwrSleep(x)          lib_app_call(APM_ID, APM_PWRSLEEP, x, 0, 0)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //_INC_MANAGER_LITE_H_




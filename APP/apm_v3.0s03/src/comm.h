//-----------------------------------------------------------------------------
//  File          : comm.h
//  Module        :
//  Description   : Declrartion & Defination for comm.c
//  Author        : Lewis
//  Notes         : N/A
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
#ifndef _INC_COMM_H_
#define _INC_COMM_H_
#include "common.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
extern struct COMMPARAM gCommParam;
extern int gTcpHandle;
extern int gAuxHdl[MAX_APM_AUX];

//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------
extern const struct MDMSREG KDefaultSReg;

//-----------------------------------------------------------------------------
//      API
//-----------------------------------------------------------------------------
extern void  CommInit(void);
extern DWORD ConnectPPP(BOOLEAN aWaitReady);
extern DWORD DisconnectPPP(void);
extern BOOLEAN SetGPRS(void);

extern void  PackComm(struct COMMPARAM *aDat);
extern void  PreConnect(void);
extern DWORD ConnectOK(DWORD aNoCancel);
extern DWORD SendRcvd(struct COMMBUF *aSend, struct COMMBUF *aRcvd);
extern void  FlushComm(void);
extern DWORD ResetComm(void);
extern void PackInitComm(void);
extern BYTE PackTelNo(struct TERM_DATA aTermData, BYTE *aTelBuf, BYTE *aTel);

extern void SetGPRSInfo(const char* userName, const char* password, const char* apn, const char* dialNum);

#endif // _INC_COMM_H_


//-----------------------------------------------------------------------------
//  File          : Phone.h
//  Module        :
//  Description   : Declrartion & Defination for phone.c
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
//  01 Jan  2010  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_PHONE_H_
#define _INC_PHONE_H_
#include "common.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------
#define MAX_PB_ITEM  20
struct PHONE_BOOK {
  BYTE sbName[16+1];
  BYTE sbNumber[16+1];
};

#define MIN_CID_RINGCOUNT    2
#define MAX_VMAIL_SUPPORT    10

enum {
  SPEAKER_GAIN       = 0,
  MIC_GAIN              ,
  PLAYBACK_GAIN         ,
  RECORD_GAIN           ,
  MAX_GAIN_IDX          ,
};

struct PHONE_CFG {
  BOOLEAN bCallerIDEnb;
  BOOLEAN bAutoAnsEnb;
  BYTE    bAutoAnsRing;
  struct  PHONE_BOOK sPhoneBk[MAX_PB_ITEM];
  DWORD   dwVMailCount;
  BYTE    sbVMailFile[MAX_VMAIL_SUPPORT][15];
  DWORD   sdwGainVal[MAX_GAIN_IDX];
};

// Call Hist Book Index
enum {
  DIAL_IN  = 0,
  DIAL_OUT,
  MAX_HIST_BOOK,
};

//-----------------------------------------------------------------------------
//      Function API
//-----------------------------------------------------------------------------
extern void PhoneInit(void);
extern void PhoneHouseKeep(void);
extern BOOLEAN OffHookState(void);
extern void PhoneFunc(DWORD aCtrl);
extern BOOLEAN AnswerCall(void);
extern void PhoneSetup(void);
extern DWORD VMailCount(void);
extern void  VMailFunc(void);

#endif  // _INC_PHONE_H_

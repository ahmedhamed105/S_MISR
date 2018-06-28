//-----------------------------------------------------------------------------
//  File          : hardware.h
//  Module        :
//  Description   : Declrartion & Defination for hardware.c
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
#ifndef _INC_HARDWARE_H_
#define _INC_HARDWARE_H_
#include "common.h"
#include "apm.h"


//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
// Timer ID
enum {
  TIMER_GEN         = 0,
  TIMER_KBD            ,
  TIMER_COMM           ,
  TIMER_DEFAULTAPP     ,
  TIMER_WIFI           ,
  TIMER_REMOTEDL       ,
  MAX_TIMER_SUPPORT    ,
};
#define TIMER_1SEC           100
#define TIMER_10MS           1

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
// Device Handle
extern int    gDevHandle[APM_SDEV_MAX];
extern int    gTimerHdl[MAX_TIMER_SUPPORT];

//-----------------------------------------------------------------------------
//      API
//-----------------------------------------------------------------------------
extern void InitAllHw(void);
extern void CloseAllHw(void);

//25-07-17 JC ++
extern int GetHwHandle(DWORD aHwId, DWORD aClose);
extern BOOLEAN InitEcrDll(void);
extern void StopEcrDll(void);
extern int PwrSleep(DWORD aSec);
//25-07-17 JC --

#endif // _INC_HARDWARE_H_


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
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_HARDWARE_H_
#define _INC_HARDWARE_H_
#include "common.h"
#include "midware.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define TIMER_DISP           0x00
#define TIMER_MAIN           0x01
#define TIMER_PPAD           0x02
#define TIMER_COMM           0x03
#define TIMER_CTL            0x04
#define TIMER_GPRS           0x05
#define TIMER_GEN            0x06
#define MAX_TIMER_SUPPORT    TIMER_GEN+1

#define TIMER_1SEC           100
#define TIMER_10MS           1

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
// Device Handle
extern int    gTimerHdl[MAX_TIMER_SUPPORT];
extern int    gMsrHandle;
extern int    gIccHandle;
extern int    gAux1Handle;
extern int    gAux2Handle;
extern int    gLptHandle;
extern int    gCdc1Handle;
extern int    gGprsHandle;
extern int    gPppHandle;

extern DWORD gIdleCnt;                  //25-07-17 JC ++

//-----------------------------------------------------------------------------
//      API
//-----------------------------------------------------------------------------
extern void InitAllHw(void);
extern void CloseAllHw(void);

//25-07-17 JC ++
#define HW_MSR          0x0001
#define HW_CLICC        0x0002
extern void OffDevice(DWORD aHw);
extern void OnDevice(DWORD aHw);
extern void PwrSleep(void);
//25-07-17 JC --

// Keyboard
extern DWORD WaitKey(DWORD aTimeout, DWORD aCheckList);

// MSR
extern BOOLEAN MSRSwiped(struct MW_TRK1BUF *aTrk1, struct MW_TRK2BUF *aTrk2);
extern void MSRTest(void);

// ICC
extern BOOLEAN ICCInserted(void);
extern BOOLEAN ICCReaderRdy(void);
extern void WaitICCRemove(void);
extern void WaitDebugICCRemove(void);
extern DWORD PowerOnICC(void);
extern DWORD PowerOffICC(void);

#endif // _INC_HARDWARE_H_


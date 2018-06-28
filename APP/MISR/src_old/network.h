//-----------------------------------------------------------------------------
//  File          : network.h
//  Module        :
//  Description   : Declrartion & Defination for network.c
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
#ifndef _INC_NETWORK_H_
#define _INC_NETWORK_H_

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
extern BYTE gSimSlot;         // active SIM slot
extern BYTE gSim1Rdy;         // SIM 1 ready flag
extern BYTE gSim2Rdy;         // SIM 2 ready flag
extern BYTE gOptrMain[32];
extern BYTE gOptrBkup[32];

//-----------------------------------------------------------------------------
//      API
//-----------------------------------------------------------------------------
extern void SetBannerMode(BOOLEAN aEnable);
extern void NetworkInfo(void);
extern void NetworkInit(void);
extern BOOLEAN SelSIM(BYTE aSIM, BYTE *aOperName, DWORD *aSignal);
extern void SimStatusThrd(BOOLEAN aStart);
extern void NetworkSetup(void);
extern void MobileNumTest(void);
extern void SNTest(void);
extern BOOLEAN GetSimCCID(BYTE *aCCID);
extern BOOLEAN GetMobileNum(BYTE *aNum);
extern BYTE GetBatteryPercent(void);

#endif // _INC_NETWORK_H_


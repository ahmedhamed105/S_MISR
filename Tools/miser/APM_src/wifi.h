//-----------------------------------------------------------------------------
//  File          : wifi.h
//  Module        :
//  Description   : Declrartion & Defination for WIFIConnect.c
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
//  15 Aug  2014  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_WIFI_H_
#define _INC_WIFI_H_
#include "common.h"

//-----------------------------------------------------------------------------
//  Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Functions
//-----------------------------------------------------------------------------
extern void WifiOn(void);
extern void WifiOff(void);
extern void WifiStatus(void);
extern DWORD GetWifiStatus(void);
extern DWORD GetWifiConnStatus(void);

#endif //_INC_WIFI_H_

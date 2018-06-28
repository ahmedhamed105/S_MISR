//-----------------------------------------------------------------------------
//  File          : toggle.h
//  Module        :
//  Description   : Declrartion & Defination for toggle.c
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
#ifndef _INC_TOGGLE_H_
#define _INC_TOGGLE_H_
#include "common.h"


//-----------------------------------------------------------------------------
//     Option toggle Function
//-----------------------------------------------------------------------------
#define MAX_OPTION_LIST     10

#define TOGGLE_MODE_4LINE   0x0000
#define TOGGLE_MODE_8LINE   0x0100

extern const char *KInitMode[];
extern const char *KInitModeBt[];
extern const char *KDialMode[];
extern const char *KEnable[];
extern const char *KAuxPort[];
extern const char *KAuxUsbPort[];
extern const char *KFTPInitMode[];
extern const char *KSimSel[];
#if (T300|T810|A5T1000|T1000)
extern const char *KAuxUsbWifiPort[];
extern const char *KAuxUsbWifiBtPort[];
#endif

#if (PR608D|R700|PR608)
enum {
  TOG_AUX1 = 1,
  TOG_AUX2,
  TOG_USBS,
  TOG_AUXD,
};
#elif (TIRO)
enum {
  TOG_AUXD = 1,
  TOG_AUX1,
  TOG_AUX2,
  TOG_USBS,
};
#elif (T700|T800)
enum {
  TOG_AUX1 = 1,
  TOG_AUXD,
  TOG_USBS,
  TOG_AUX2,
};
#elif (T300|T810|A5T1000|T1000)
enum {
  TOG_AUX1 = 1,
  TOG_AUXD,
  TOG_USBS,
  TOG_LAN,
  TOG_WIFI,
  TOG_BT,
  TOG_AUX2,
};
#elif (PR500)
enum {
  TOG_AUXD = 1,
  TOG_USBS,
  TOG_AUX1,
  TOG_AUX2,
};
#else
NEW_TERMINAL_HERE
#endif
extern char ToggleOption(const char *aMsg, const char **aOptTbl, DWORD aCurVal);
extern DWORD ToggleYesNo(void);

#endif // _INC_TOGGLE_H_


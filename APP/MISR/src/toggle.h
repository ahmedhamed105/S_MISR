//-----------------------------------------------------------------------------
//  File          : toggle.h
//  Module        :
//  Description   : Declrartion & Defination for toggle.c
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
//  01 Dec  2007 Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_TOGGLE_H_
#define _INC_TOGGLE_H_
#include "common.h"


//-----------------------------------------------------------------------------
//     Option toggle Function
//-----------------------------------------------------------------------------
#define MAX_OPTION_LIST     16

#define TOGGLE_MODE_4LINE   0x000000
#define TOGGLE_MODE_8LINE   0x010000
#define TOGGLE_MODE_MASK    0xFF0000
#define OPTION_VAL_MASK     0x0000FF

extern const char *KInitMode[];
extern const char *KTrueFalse[];
extern const char *KDialMode[];
extern const char *KOnOff[];
extern const char *KEnable[];
extern const char *KAuxSpeed[];
extern const char *KAuxPort[];
extern const char *KPPPport[];
extern const char *KAuxCommMode[];
extern const char *KAuxDataLen[];

extern const char *KProtoMode[];
extern const char *KCcittMode[];
extern const char *KMdmSpeed[]   ;

extern char ToggleOption(const char *aMsg, const char **aOptTbl, DWORD aCurVal);

#endif // _INC_TOGGLE_H_


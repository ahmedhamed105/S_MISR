//-----------------------------------------------------------------------------
//  File          : cakey.h
//  Module        :
//  Description   : Declrartion & Defination for cakey.c
//  Author        : Lewis
//  Notes         : N/A
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
//  01 Dec  2007  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_CAKEY_H_
#define _INC_CAKEY_H_
#include "common.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
//#define SSL_CA_IDX_TMS        0xFC
//#define SSL_CA_IDX_ECR        0xFD
//#define SSL_CA_IDX_HOST       0xFF
//#define SSL_CLN_CERT_IDX      0xFF
//#define SSL_CLN_PRV_KEY_IDX   0xFF

//-----------------------------------------------------------------------------
//    Functions
//-----------------------------------------------------------------------------
extern BOOLEAN InjectCAKey(void);

#endif // _INC_CAKEY_H_


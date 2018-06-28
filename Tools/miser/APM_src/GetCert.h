//-----------------------------------------------------------------------------
//  File          : GetCert.h
//  Module        :
//  Description   : Declaration & defines for GetCert.c
//  Author        : Kenneth
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
//  10 Jul  2012  Kenneth     Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_GETCERT_H_
#define _INC_GETCERT_H_

#include "midware.h"

//----------------------------------------------------------------------------
//  Defines
//----------------------------------------------------------------------------
struct FILELIST
{
	DWORD	len;
	BYTE	filename[256];
};

// CA Key mapping to Parameter file
enum {
  CA_ECR,
  CA_HOST,
  CA_TMS,
  MAX_CA,
};

//----------------------------------------------------------------------------
//  Generic API
//----------------------------------------------------------------------------
extern BOOLEAN GetKey(struct MW_KEY *aptr, BYTE aCAKeyID);
extern BOOLEAN GetClientCert(T_CLIENT_CERT *aptr);
extern BOOLEAN GetClientPriKey(T_RSA_PRIVATE_KEY *aptr);

#endif      // _INC_GETCERT_H_

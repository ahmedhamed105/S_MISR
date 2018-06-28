//***************************************
// Generate RSAPrivateKey C header
// by Harris Lee 
// Date : 12 Sep 2011 
// Last update : 23 Mar 2012 
//***************************************
#ifndef _INC_PSCS_H_
#define _INC_PSCS_H_
#include "common.h"
#include "system.h"
#include "basecall.h"

#define PKCS_OK                     0
#define ERR_PKCS_KEY_SEQUENCE       1
#define ERR_PKCS_KEY_VERSION        2
#define ERR_PKCS_PUB_KEY            3
#define ERR_PKCS_PRI_KEY            4
#define ERR_PKCS_PRI_MODULUS        5
#define ERR_PKCS_PRI_PRIME1         6
#define ERR_PKCS_PRI_PRIME2         7
#define ERR_PKCS_PRI_EXPONENT1      8
#define ERR_PKCS_PRI_EXPONENT2      9
#define ERR_PKCS_PRI_COEFFICIENT    10
#define ERR_PKCS_KEY_END            11

#define FILL_PRE_ZERO               0
#define FILL_POST_ZERO              1

#define MaxRsaPrimeLen(a)           (a/2)

extern DWORD x509ParseKey(BYTE *ptr, DWORD size, T_KEY *pubKey, T_RSA_PRIVATE_KEY *priKey);

#endif

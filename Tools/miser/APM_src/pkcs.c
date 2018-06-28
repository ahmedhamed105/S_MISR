//***************************************
// Generate RSAPrivateKey C header
// by Harris Lee, 15 Sep 2011
// Last update : 20 Dec 2011 
//***************************************

#include <stdio.h>
#include <string.h>
#include "pkcs.h"

#define ASN_PRIMITIVE        0x00
#define ASN_CONSTRUCTED      0x20

#define ASN_UNIVERSAL        0x00
#define ASN_APPLICATION      0x40
#define ASN_CONTEXT_SPECIFIC 0x80
#define ASN_PRIVATE          0xC0

#define ASN_BOOLEAN          1
#define ASN_INTEGER          2
#define ASN_BIT_STRING       3
#define ASN_OCTET_STRING     4
#define ASN_NULL             5
#define ASN_OID              6
#define ASN_UTF8STRING       12
#define ASN_SEQUENCE         16
#define ASN_SET              17
#define ASN_PRINTABLESTRING  19
#define ASN_T61STRING        20
#define ASN_IA5STRING        22
#define ASN_UTCTIME          23
#define ASN_GENERALIZEDTIME  24

#define ATTRIB_COUNTRY_NAME    6
#define ATTRIB_LOCALITY        7
#define ATTRIB_ORGANIZATION   10
#define ATTRIB_ORG_UNIT       11
#define ATTRIB_DN_QUALIFIER   46
#define ATTRIB_STATE_PROVINCE  8
#define ATTRIB_COMMON_NAME     3

#define IMPLICIT_ISSUER_ID      1
#define IMPLICIT_SUBJECT_ID     2
#define EXPLICIT_EXTENSION      3

#define EXT_CERT_POLICIES       146
#define EXT_BASIC_CONSTRAINTS   133
#define EXT_AUTH_KEY_ID         149
#define EXT_KEY_USAGE           129
#define EXT_ALT_SUBJECT_NAME    131
#define EXT_ALT_ISSUER_NAME     132
#define EXT_SUBJ_KEY_ID         128


int getASNLength(BYTE **p, DWORD size, int *valLen) {
  BYTE  *c, *end; // , value;
  int len, olen;

  c = *p;
  end = c + size;
  if (end - c < 1)
    return 0;

  /*
    If the length byte has high bit only set, it's an indefinite length
    We don't support this!
  */
  if (*c == 0x80) {
    *valLen = -1;
    return 0;
  }
  /*
    If the high bit is set, the lower 7 bits represent the number of 
    bytes that follow and represent length
    If the high bit is not set, the lower 7 represent the actual length
  */
  len = *c & 0x7F;
  if ((*(c++) & 0x80) != 0) {
    /*
      Make sure there aren't more than 4 bytes of length specifier,
      and that we have that many bytes left in the buffer
    */
    if (len > 4 || len == 0x7f || (end - c) < len) { 
      return 0;
    }

    olen = 0;
    while (len > 0) {
      olen = (olen << 8) | *c;
      c++; len--;
    }
    if (olen < 0 || olen > 0x7FFFFF) {
      return 0;
    }
    len = olen;
  }

  *p = c;
  *valLen = len;
  return -1;
}

/* getSequence -- Find Sequence Object From Buffer
 * pp     - Pointer to Buffer
 * len    - Size of Buffer
 * seqlen - Return the Length of Sequence Object
 */
int getSequence(BYTE **pp, DWORD len, int *seqlen) {
  BYTE *p = *pp;

  if (len < 1 || *(p++) != (ASN_SEQUENCE | ASN_CONSTRUCTED) || 
      getASNLength(&p, len - 1, seqlen) == 0 || len < *seqlen) {
    return 0;
  }

  *pp = p;
  return -1;
}

int getVersionNum(BYTE **pp, int len)
{
  BYTE *p = *pp;
  int  vlen;
  
  if (len < 1 || *(p++) != ASN_INTEGER ||
      getASNLength(&p, len - 1, &vlen) == 0) {
    return 0;
  }

  p += vlen;
  *pp = p;
  return -1;
}

int getBig(BYTE **pp, int len, BYTE *big, DWORD *keylen, unsigned int keylen_hdr, BYTE fillzero)
{
  BYTE *p = *pp;
  int   vlen;

  if (len < 1 || *(p++) != ASN_INTEGER ||
      getASNLength(&p, len - 1, &vlen) == 0) {
    return 0;
  }

  if (*p == 0) {
    p++; vlen--;
  }

  if (vlen > *keylen)
    return 0;  

  if (fillzero == FILL_PRE_ZERO) {
    memset(big, 0, (keylen_hdr -vlen));
    memcpy((big+(keylen_hdr -vlen)), (void*)p, vlen);
  }
  else {
    memcpy(big, (void*)p, vlen);
    memset((big+vlen), 0, (keylen_hdr -vlen));
  }

  *keylen = vlen;  
  p += vlen;
  *pp = p;

  return -1;
}

int getPubKey(BYTE**pp, int len, DWORD *keyLen, T_KEY *pubKey) {
  BYTE  *p = *pp;
  BYTE  value[4];
  DWORD expLen, count;

  expLen = 4;
  if ((getBig(&p, len, (BYTE*)pubKey->s_key, keyLen, K_MaxRsaModulusLen, FILL_PRE_ZERO/*FILL_POST_ZERO*/)) == 0 ||
       (getBig(&p, len, (BYTE*)value, &expLen, expLen, FILL_POST_ZERO) == 0)) {
    return 0;
  }
  pubKey->d_keysize  = *keyLen;
  pubKey->d_exponent = 0;
  for (count = 0; count < expLen; count++)
    pubKey->d_exponent = (pubKey->d_exponent << 8) | value[count];
  
  *pp = p;
  return -1;
}

DWORD x509ParseKey(BYTE *ptr, DWORD size,  T_KEY *pubKey, T_RSA_PRIVATE_KEY *priKey) {
  BYTE *p = ptr;
  BYTE *KeyEnd, *end;
  int len;
  DWORD keyLen, keyLen_read;

/*
RSAPrivateKey ::= SEQUENCE {
    version           Version,
    modulus           INTEGER,  -- n
    publicExponent    INTEGER,  -- e
    privateExponent   INTEGER,  -- d
    prime1            INTEGER,  -- p
    prime2            INTEGER,  -- q
    exponent1         INTEGER,  -- d mod (p-1)
    exponent2         INTEGER,  -- d mod (q-1)
    coefficient       INTEGER,  -- (inverse of q) mod p
    otherPrimeInfos   OtherPrimeInfos OPTIONAL
}

Version ::= INTEGER { two-prime(0), multi(1) }
    (CONSTRAINED BY {
        -- version must be multi if otherPrimeInfos present --
    })

OtherPrimeInfos ::= SEQUENCE SIZE(1..MAX) OF OtherPrimeInfo

OtherPrimeInfo ::= SEQUENCE {
    prime             INTEGER,  -- ri
    exponent          INTEGER,  -- di
    coefficient       INTEGER   -- ti
}
*/
  len = 0;
  end = p + size;

  if (getSequence(&p, size, &len) == 0)
    return ERR_PKCS_KEY_SEQUENCE;

  KeyEnd = p + len;

/*
	Version ::= INTEGER { two-prime(0), multi(1) }
*/
  if (getVersionNum(&p, (int)(end - p)) == 0)
    return ERR_PKCS_KEY_VERSION;

  keyLen = K_MaxRsaModulusLen;
  if (getPubKey(&p, (int)(end - p), &keyLen, pubKey) == 0) {
    return ERR_PKCS_PUB_KEY;
  }
  keyLen_read = keyLen;
  priKey->d_bits = pubKey->d_keysize * 8;

  keyLen = keyLen_read;
  if ((getBig(&p, (int)(end - p), (BYTE*) priKey->s_modulus, &keyLen, K_MaxRsaModulusLen, FILL_PRE_ZERO/*FILL_POST_ZERO*/)) == 0)
    return ERR_PKCS_PRI_MODULUS;

  keyLen = MaxRsaPrimeLen(keyLen_read);
  if ((getBig(&p, (int)(end - p), (BYTE*) priKey->s_prime[0], &keyLen, K_MaxRsaPrimeLen, FILL_PRE_ZERO)) == 0)
    return ERR_PKCS_PRI_PRIME1; 

  keyLen = MaxRsaPrimeLen(keyLen_read);
  if ((getBig(&p, (int)(end - p), (BYTE*) priKey->s_prime[1], &keyLen, K_MaxRsaPrimeLen, FILL_PRE_ZERO)) == 0)
    return ERR_PKCS_PRI_PRIME2; 

  keyLen = MaxRsaPrimeLen(keyLen_read);
  if ((getBig(&p, (int)(end - p), (BYTE*) priKey->s_prime_exponent[0], &keyLen, K_MaxRsaPrimeLen, FILL_PRE_ZERO)) == 0)
    return ERR_PKCS_PRI_EXPONENT1; 

  keyLen = MaxRsaPrimeLen(keyLen_read);
  if ((getBig(&p, (int)(end - p), (BYTE*) priKey->s_prime_exponent[1], &keyLen, K_MaxRsaPrimeLen, FILL_PRE_ZERO)) == 0)
    return ERR_PKCS_PRI_EXPONENT2;

  keyLen = MaxRsaPrimeLen(keyLen_read);
  if ((getBig(&p, (int)(end - p), (BYTE*) priKey->s_coefficient, &keyLen, K_MaxRsaPrimeLen, FILL_PRE_ZERO)) == 0)
    return ERR_PKCS_PRI_COEFFICIENT;     

  if (KeyEnd != p)
      return ERR_PKCS_KEY_END;

  return PKCS_OK;
}



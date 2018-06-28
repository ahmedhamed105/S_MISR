/*
************************************
*       Module : x509.c            *
*       Name   : TF                *
*       Date   : 31-01-2013        *
************************************
*/
/*==========================================================================*/
/* Naming conventions                                                       */
/* ~~~~~~~~~~~~~~~~~~                                                       */
/*         Class define : Leading C                                         */
/*        Struct define : Leading T                                         */
/*               Struct : Leading s                                         */
/*               Class  : Leading c                                         */
/*             Constant : Leading K                                         */
/*      Global Variable : Leading g                                         */
/*    Function argument : Leading a                                         */
/*       Local Variable : All lower case                                    */
/*            Byte size : Leading b                                         */
/*            Word size : Leading w                                         */
/*           Dword size : Leading d                                         */
/*              Pointer : Leading p                                         */
/*==========================================================================*/

/* ----------------------------------------------------------------------
 * Revision History
 * ----------------------------------------------------------------------
 * 22 Jan 2015 - Kong
 *   1. remove certificate verification code from x509_parse_cert().
 *   2. add x509_verify_cert_by_key() and x509_verify_cert_by_cert()
 *      for certificate verficiation against system injected CA public
 *      key and its signer CA for chained certification scenario.
 *   3. re-enable parsing BasicConstraint and KeyUsage extension info.
 *   4. add support of RSASSA-PSS signature padding scheme.
 *   5. add support of the following signature algorithm,
 *      a. sha224WithRSAEncryption,
 *      b. sha256WithRSAEncryption,
 *      c. sha384WithRSAEncryption, and
 *      d, sha512WithRSAEncryption.
 *   6. return BasicConstraint::CA boolean result for classification of
 *      the subject being a CA.
 *   7. return SHA1 digest of subject and isser distinguished name info
 *      for chained certificate verification and self-signed certificate
 *      recognition.
 *
 * 16 Feb 2015 - Kong
 *   1. Release server's certificate public key length constrain to 4096
 *      since RSA routine implemented in system can support public key
 *      lenght up to 4096 bits.
 * ---------------------------------------------------------------------- */

#include <string.h>
#include "system.h"
#include "x509.h"
#include "hashops.h"

/* #define KONG_DEBUG_X509_MODULE */

#if defined(KONG_DEBUG_X509_MODULE)
#include "ssl_tls_utils.h"
#endif

#if defined(KONG_DEBUG_X509_MODULE)
#define THIS_DBG_LVL   4
#define dbg_printf(...)      SslTlsUtils_TraceLog_Printf(THIS_DBG_LVL,__VA_ARGS__)
#define dbg_hexvect(v,l)     SslTlsUtils_TraceLog_Hexvect(THIS_DBG_LVL,(v),(l))
#define dbg_hexdump(v,l)     SslTlsUtils_TraceLog_Hexdump(THIS_DBG_LVL,(v),(l))
#define dbg_println()        SslTlsUtils_TraceLog_Printf(THIS_DBG_LVL,"\r\n")
#else
#define dbg_printf(...)      do {} while (0)
#define dbg_hexvect(v,l)     do {} while (0)
#define dbg_hexdump(v,l)     do {} while (0)
#define dbg_println()        do {} while (0)
#endif

/* MACROS to wrap key routines based on platform difference */
#define EXEC_RSA(x,y)             os_rsa((x),(y))
#define EXEC_SHA1_INIT(x)         os_sha1_init((x))
#define EXEC_SHA1_UPDATE(x,y,z)   os_sha1_process((x),(y),(z))
#define EXEC_SHA1_FINISH(x,y)     os_sha1_finish((x),(y))
#define EXEC_RTC_GET(x)           os_rtc_get((x))


/* Kong: we are interested in keyUsage extension info */
#define USE_FULL_CERT_PARSE
/* #define USE_FULL_CERT_PARSE_AUTHKEYID */
#define USE_FULL_CERT_PARSE_KEYUSAGE
/* #define USE_FULL_CERT_PARSE_SUBJKEYID */

#define USE_OID_HASH_TYPE   1   /* use new OID string hash function */
//#define USE_OID_HASH_TYPE   0   /* use old OID string hash function */

#define K_AsnPrimitive            0x00
#define K_AsnConstructed          0x20

#define K_AsnUniversal            0x00
#define K_AsnApplication          0x40
#define K_AsnContextSpecific      0x80
#define K_AsnPrivate              0xC0

#define K_AsnBoolean              1
#define K_AsnInteger              2
#define K_AsnBitString            3
#define K_AsnOctetString          4
#define K_AsnNull                 5
#define K_AsnOId                  6
#define K_AsnUtf8String           12
#define K_AsnSequence             16
#define K_AsnSet                  17
#define K_AsnPrintableString      19
#define K_AsnT61String            20
#define K_AsnIa5String            22
#define K_AsnUtcTime              23
#define K_AsnGeneralizedTime      24

#define K_AttribCountryName       6
#define K_AttribLocality          7
#define K_AttribOrganization      10
#define K_AttribOrgUnit           11
#define K_AttribDnQualifier       46
#define K_AttribStateProvince     8
#define K_AttribCommonName        3
// syspack V1.2 9-7-2012
#define K_AttribSerialNumber      5

#define K_ImplicitIssuerId        1
#define K_ImplicitSubjectId       2
#define K_ExplicitExtension       3

//#define K_OIdExtCertPolicies         146
//#define K_OIdExtBasicConstraints     133
//#define K_OIdExtAuthKeyId            149
//#define K_OIdExtKeyUsage             129
//#define K_OIdExtAltSubjectName       131
//#define K_OIdExtAltIssuerName        132
//#define K_OIdExtSubjKeyId            128

#if defined(USE_OID_HASH_TYPE) && (USE_OID_HASH_TYPE == 1)

/* OID string hash value using new general string hash function */

#define K_OIdExtBasicConstraints  0x0b87f19e
#define K_OIdExtAuthKeyId         0x0b87f1ae
#define K_OIdExtKeyUsage          0x0b87f182
#define K_OIdExtSubjKeyId         0x0b87f183
#define K_OIdExtAltSubjectName    0x0b87f19c

#define K_OIdRsaEncryption        0x8b757efc
#define K_OIdRsaMd2               0x8b757eff
#define K_OIdRsaMd5               0x8b757ef9
#define K_OIdRsaSha1              0x8b757ef8
#define K_OIdRsaSha224            0x8b757ef3
#define K_OIdRsaSha256            0x8b757ef6
#define K_OIdRsaSha384            0x8b757ef1
#define K_OIdRsaSha512            0x8b757ef0
#define K_OIdRsaPSS               0x8b757ef7
#define K_OIdMgf1                 0x8b757ef5

#define K_OIdMd2                  0x691313bd
#define K_OIdMd5                  0x691313ba
#define K_OIdSha1                 0x06572bdb
#define K_OIdSha224               0xf39ad36e
#define K_OIdSha256               0xf39ad36b
#define K_OIdSha384               0xf39ad368
#define K_OIdSha512               0xf39ad369

#else

/* OID hash values using simple addition as hash function */

#define K_OIdExtBasicConstraints  133
#define K_OIdExtAuthKeyId         149
#define K_OIdExtKeyUsage          129
#define K_OIdExtSubjKeyId         128
#define K_OIdExtAltSubjectName    131

#define K_OIdRsaEncryption        645
#define K_OIdRsaMd2               646
#define K_OIdRsaMd5               648
#define K_OIdRsaSha1              649
#define K_OIdRsaSha224            658
#define K_OIdRsaSha256            655
#define K_OIdRsaSha384            656
#define K_OIdRsaSha512            657
#define K_OIdRsaPSS               654

#define K_OIdMgf1                 652

#define K_OIdMd2                  646
#define K_OIdMd5                  649
#define K_OIdSha1                 88
#define K_OIdSha224               417
#define K_OIdSha256               414
#define K_OIdSha384               415
#define K_OIdSha512               416

#endif

/* static T_KEY            s_x509_cakey; */

static BYTE             s_x509_sign[K_X509_CertSignMaxSize];

#if defined(USE_OID_HASH_TYPE) && (USE_OID_HASH_TYPE == 1)
static DWORD oid_to_hash_value(BYTE *p, int len)
{
  DWORD hash = 5381;
  int i;

  for ( i=0; i<len; ++i, ++p ) {
    hash = (hash * 33) ^ (*p);
  };
  return hash;
}
#else
static DWORD oid_to_hash_value(BYTE *p, int len)
{
  DWORD hash = 0;
  int i;

  for ( i=0; i<len; ++i, ++p ) {
    hash += *p;
  };
  return hash;
}
#endif

static DWORD x509_bit_reverse(DWORD x)
{
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return((x >> 16) | (x << 16));
}

/*---------------------------------------------------------------------*/
static int x509_get_asn_length(BYTE ** a_p,DWORD a_size,int * a_valLen)
{
  BYTE  *c, *end;
  int   len, olen;

  c = *a_p;
  end = c + a_size;
  if (end - c < 1)
    return 0;

  /*
    If the length byte has high bit only set, it's an indefinite length
    We don't support this!
  */
  if (*c == 0x80)
  {
    *a_valLen = -1;
    return 0;
  }
  /*
    If the high bit is set, the lower 7 bits represent the number of
    bytes that follow and represent length
    If the high bit is not set, the lower 7 represent the actual length
  */
  len = *c & 0x7F;
  if ((*(c++) & 0x80) != 0)
  {
    /*
      Make sure there aren't more than 4 bytes of length specifier,
      and that we have that many bytes left in the buffer
    */
    if (len > 4 || len == 0x7f || (end - c) < len)
      return 0;

    olen = 0;
    while (len > 0)
    {
      olen = (olen << 8) | *c;
      c++; len--;
    }
    if (olen < 0 || olen > 0x7FFFFF)
    {
      return 0;
    }
    len = olen;
  }

  *a_p = c;
  *a_valLen = len;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_boolean(BYTE **a_pp, int a_len, int *a_bool)
{
  BYTE *p;
  int blen;

  p = *a_pp;
  
  if ( a_len < 3 || *(p++) != K_AsnBoolean ||
       x509_get_asn_length(&p,a_len-1, &blen) == 0 )
    return 0;
  if ( blen != 1 )
    return 0;

  *a_bool = *p++;
  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_integer(BYTE ** a_pp,int a_len,int * a_val)
{
  BYTE  *p, *end;
  DWORD ui;
  int   vlen;

  p = *a_pp;
  end = p + a_len;
  if (a_len < 1 || *(p++) != K_AsnInteger ||
      x509_get_asn_length(&p, a_len - 1, &vlen) == 0)
    return 0;

  /*
    This check prevents us from having a big positive integer where the
    high bit is set because it will be encoded as 5 bytes (with leading
    blank byte).  If that is required, a getUnsigned routine should be used
  */
  if (vlen > sizeof(int) || end - p < vlen)
    return 0;

  ui = 0;
  /*
    If high bit is set, it's a negative integer, so perform the two's compliment
    Otherwise do a standard big endian read (most likely case for RSA)
  */
  if (*p & 0x80)
  {
    while (vlen-- > 0)
    {
      ui = (ui << 8) | (*p ^ 0xFF);
      p++;
    }
    vlen = (int)ui;
    vlen++;
    vlen = -vlen;
    *a_val = vlen;
  }
  else
  {
    while (vlen-- > 0)
    {
      ui = (ui << 8) | *p;
      p++;
    }
    *a_val = (int)ui;
  }
  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_set(BYTE ** a_pp,int a_len,int * a_setlen)
{
  BYTE * p;

  p = *a_pp;
  if (a_len<1 || *(p++)!=(K_AsnSet|K_AsnConstructed) ||
      x509_get_asn_length(&p,a_len-1,a_setlen)==0 || a_len<*a_setlen)
    return 0;

  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
/* x509_get_sequence -- Find Sequence Object From Buffer
 * pp     - Pointer to Buffer
 * len    - Size of Buffer
 * seqlen - Return the Length of Sequence Object
 */
static int x509_get_sequence(BYTE ** a_pp,DWORD a_len,int * a_seqlen)
{
  BYTE * p;

  p = *a_pp;
  if (a_len<1 || *(p++)!=(K_AsnSequence|K_AsnConstructed) ||
      x509_get_asn_length(&p,a_len-1,a_seqlen)==0 || a_len<*a_seqlen)
    return 0;

  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
/* x509_get_oid -- Find OID object from Buffer
 * pp      - Pointer to Buffer
 * len     - Size of Buffer
 * seqlen  - return the length of OID
 */
static int x509_get_oid(BYTE **a_pp, DWORD a_len, int *a_seqlen)
{
  BYTE *p;

  p = *a_pp;
  if ( a_len < 1 || *(p++) != K_AsnOId ||
       x509_get_asn_length(&p,a_len-1,a_seqlen)==0 || a_len<*a_seqlen )
    return 0;

  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_dn_attrib(BYTE ** a_pp, int a_len, BYTE *a_dn_mac)
{
/* sslSha1Context_t hash; */
  BYTE *  p;
  BYTE *  dnEnd;
  int     llen,setlen,arcLen,id,stringType;
  T_SHA1_CONTEXT sha1_ctx;

  p = *a_pp;
  if (x509_get_sequence(&p,a_len,&llen) == 0)
    return 0;

  dnEnd = p + llen;

  if ( a_dn_mac != NULL )
  {
    EXEC_SHA1_INIT(&sha1_ctx);
    EXEC_SHA1_UPDATE(&sha1_ctx,p,llen);
    EXEC_SHA1_FINISH(&sha1_ctx,a_dn_mac);
  }

/* matrixSha1Init(&hash); */
  while (p < dnEnd)
  {
    if (x509_get_set(&p,(int)(dnEnd - p),&setlen) == 0)
      return 0;
    if (x509_get_sequence(&p,(int)(dnEnd - p),&llen) == 0)
      return 0;
    if (dnEnd <= p || (*(p++) != K_AsnOId) ||
        x509_get_asn_length(&p,(int)(dnEnd - p),&arcLen) == 0 ||
        (dnEnd - p) < arcLen)
      return 0;
/*
  id-at                        OBJECT IDENTIFIER ::= {joint-iso-ccitt(2) ds(5) 4}
  id-at-commonName             OBJECT IDENTIFIER ::= {id-at 3}
  id-at-countryName            OBJECT IDENTIFIER ::= {id-at 6}
  id-at-localityName           OBJECT IDENTIFIER ::= {id-at 7}
  id-at-stateOrProvinceName    OBJECT IDENTIFIER ::= {id-at 8}
  id-at-organizationName       OBJECT IDENTIFIER ::= {id-at 10}
  id-at-organizationalUnitName OBJECT IDENTIFIER ::= {id-at 11}
*/
    *a_pp = p;
/*
  FUTURE: Currently skipping OIDs not of type {joint-iso-ccitt(2) ds(5) 4}
  However, we could be dealing with an OID we MUST support per RFC.
  domainComponent is one such example.
*/
    if (dnEnd - p < 2)
      return 0;
    if ((*p++ != 85) || (*p++ != 4) )
    {
      p = *a_pp;
/*
  Move past the OID and string type, get data size, and skip it.
  NOTE: Have had problems parsing older certs in this area.
*/
      if (dnEnd - p < arcLen + 1)
        return 0;
      p += arcLen + 1;
      if (x509_get_asn_length(&p,(int)(dnEnd - p),&llen) == 0 ||
          dnEnd - p < llen)
        return 0;
      p = p + llen;
      continue;
    }
/*
  Next are the id of the attribute type and the ASN string type
*/
    if (arcLen != 3 || dnEnd - p < 2)
      return 0;
    id = (int)*p++;
    /*
      Done with OID parsing
    */
    stringType = (int)*p++;

    x509_get_asn_length(&p,(int)(dnEnd - p),&llen);
    if (dnEnd - p < llen)
      return 0;
    switch (stringType)
    {
      case K_AsnPrintableString:
      case K_AsnUtf8String:
      case K_AsnT61String:
        p = p + llen;
        break;
      default:
        return 0;
    }
// syspack V1.2 9-7-2012
#if 0
    switch (id)
    {
      case K_AttribCountryName:
/*       if (attribs->country) { */
/*         psFree(attribs->country); */
/*       } */
/*       attribs->country = stringOut; */
        break;
      case K_AttribStateProvince:
/*       if (attribs->state) { */
/*         psFree(attribs->state); */
/*       } */
/*       attribs->state = stringOut; */
        break;
      case K_AttribLocality:
/*       if (attribs->locality) { */
/*         psFree(attribs->locality); */
/*       } */
/*       attribs->locality = stringOut; */
        break;
      case K_AttribOrganization:
/*       if (attribs->organization) { */
/*         psFree(attribs->organization); */
/*       } */
/*       attribs->organization = stringOut; */
        break;
      case K_AttribOrgUnit:
/*       if (attribs->orgUnit) { */
/*         psFree(attribs->orgUnit); */
/*       } */
/*       attribs->orgUnit = stringOut; */
        break;
      case K_AttribCommonName:
/*       if (attribs->commonName) { */
/*         psFree(attribs->commonName); */
/*       } */
/*       attribs->commonName = stringOut; */
        break;
//
    case K_AttribSerialNumber:
      break;
/*
  Not a MUST support
*/
      default:
/*       psFree(stringOut); */
        return 0;
    }
/*
  Hash up the DN.  Nice for validation later
*/
/*     if (stringOut != NULL) { */
/*       matrixSha1Update(&hash, (BYTE*)stringOut, llen); */
/*     } */
#else
     if (id > 53)
       return 0;
#endif
  }
/*   matrixSha1Final(&hash, (BYTE*)attribs->hash); */
  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_explicit_version(BYTE ** a_pp,int a_len,int a_expVal,int * a_val)
{
  BYTE *  p;
  int     exLen;

  if (a_len < 1)
    return 0;

  p = * a_pp;

  /*
    This is an optional value, so don't error if not present.  The default
    value is version 1
  */
  if (*p != (K_AsnContextSpecific | K_AsnConstructed | a_expVal))
  {
    *a_val = 0;
    return -1;
  }
  p++;

  if (x509_get_asn_length(&p,a_len-1,&exLen)==0 || (a_len-1)<exLen)
    return 0;

  if (x509_get_integer(&p,exLen,a_val) == 0)
    return 0;

  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_implicit_bit_string(BYTE ** a_pp,int a_len,int a_impVal)
{
  BYTE *  p ;
  int     ignore_bits;
  int     bitLen;

  if (a_len < 1) return 0;
  p = * a_pp;
  /*
    We don't treat this case as an error, because of the optional nature.
  */
  if (*p != (K_AsnContextSpecific | K_AsnConstructed | a_impVal))
    return -1;

  p++;
  if (x509_get_asn_length(&p, a_len, &bitLen) == 0)
    return 0;

  ignore_bits = *p++;
  if (ignore_bits != 0)
    return 0;

  *a_pp = p + bitLen;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_explicit_extensions(BYTE ** a_pp,int a_inlen, int a_expVal, T_X509_CERT_INFO *a_cert_info)
{
  BYTE *  p;
  BYTE *  end;
  BYTE *  extEnd;
  int     len, oid, tmpLen;
// syspack V1.2
//  int     critical;
  int     pathLenConstraint;
  int     bool_val;

  if (a_inlen < 1) return 0;
  p   = *a_pp;
  end = p + a_inlen;
/*
  Not treating this as an error because it is optional.
*/
  if (*p != (K_AsnContextSpecific | K_AsnConstructed | a_expVal))
    return -1;

  p++;
  if (x509_get_asn_length(&p,(int)(end - p),&len) == 0 || (end-p) < len)
    return 0;
/*
  Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension

  Extension  ::= SEQUENCE {
               extnID    OBJECT IDENTIFIER,
     extnValue OCTET STRING }
*/
  if (x509_get_sequence(&p,(int)(end - p),&len) == 0)
    return 0;
  extEnd = p + len;
  while ((p != extEnd) && *p == (K_AsnSequence|K_AsnConstructed))
  {
    if (x509_get_sequence(&p,(int)(extEnd - p),&len) == 0)
      return 0;
/*
  Conforming CAs MUST support key identifiers, basic constraints,
  key usage, and certificate policies extensions

  id-ce-authorityKeyIdentifier OBJECT IDENTIFIER ::= { id-ce 35 }
  id2-ce-basicConstraints      OBJECT IDENTIFIER ::= { id-ce 19 } 133
  id-ce-keyUsage               OBJECT IDENTIFIER ::= { id-ce 15 }
  id-ce-certificatePolicies    OBJECT IDENTIFIER ::= { id-ce 32 }
  id-ce-subjectAltName         OBJECT IDENTIFIER ::= { id-ce 17 } 131
*/
    if (extEnd - p < 1 || *p++ != K_AsnOId)
      return 0;

    oid = 0;
    if (x509_get_asn_length(&p,(int)(extEnd - p),&len) == 0 ||
        (extEnd - p) < len)
      return 0;

#if 0
    while (len-- > 0)
      oid += (int)*p++;
#else
    oid = oid_to_hash_value(p,len);
    p += len;
#endif
/*
  Possible boolean value here for 'critical' id.  It's a failure if a
  critical extension is found that is not supported
*/
// syspack V1.2 9-7-2012
//    critical = 0;
    if (*p == K_AsnBoolean)
    {
      p++;
      if (*p++ != 1)
        return 0;
      if (*p++ > 0);
// syspack V1.2 9-7-2012
//        critical = 1;
    }
    if (extEnd - p < 1 || (*p++ != K_AsnOctetString) ||
        x509_get_asn_length(&p,(int)(extEnd - p),&len) == 0 ||
        extEnd - p < len)
      return 0;

    switch (oid)
    {
/*
  BasicConstraints ::= SEQUENCE {
                          cA BOOLEAN DEFAULT FALSE,
                          pathLenConstraint INTEGER (0..MAX) OPTIONAL }
*/
      case K_OIdExtBasicConstraints:
        if (x509_get_sequence(&p,(int)(extEnd - p),&len) == 0)
          return 0;

        /* dbg_printf(" -- basic constrains (len=%u)\n",len); */
        /* dbg_hexdump(p,len); */
        
/*
  "This goes against PKIX guidelines but some CAs do it and some
  software requires this to avoid interpreting an end user
  certificate as a CA."
    - OpenSSL certificate configuration doc
    basicConstraints=CA:FALSE
*/
        if (len == 0)
          break;

        if ( x509_get_boolean(&p,len,&bool_val) == 0 )
          return 0;
        if ( bool_val ) {
          a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_IsCA;
        } else {
          a_cert_info->w_key_usage &= ~K_X509_KeyUsageFlag_IsCA;
        };
        
        /* if (extEnd - p < 3) */
        /*   return 0; */
        /* if (*p++ != K_AsnBoolean) */
        /*   return 0; */
        /* if (*p++ != 1) */
        /*   return 0; */
        /* // *p++; */
	/* p++; */
/*
  Now need to check if there is a path constraint. Only makes
  sense if cA is true.  If it's missing, there is no limit to
  the cert path
*/
        if (*p == K_AsnInteger)
        {
          if (x509_get_integer(&p,(int)(extEnd - p),&pathLenConstraint) == 0)
            return 0;
        }
        else
          pathLenConstraint = -1;
        break;
        
      case K_OIdExtAltSubjectName:
        
        if (x509_get_sequence(&p,(int)(extEnd - p),&len) == 0)
          return 0;
/*
  Looking only for DNS, URI, and email here to support
  FQDN for Web clients

  FUTURE:  Support all subject alt name members
  GeneralName ::= CHOICE {
    otherName[0]                    OtherName,
    rfc822Name[1]                   IA5String,
    dNSName[2]                      IA5String,
    x400Address [3]                 ORAddress,
    directoryName[4]                Name,
    ediPartyName[5]                 EDIPartyName,
    uniformResourceIdentifier[6]    IA5String,
    iPAddress[7]                    OCTET STRING,
    registeredID[8]                 OBJECT IDENTIFIER }
*/
        while (len > 0)
        {
          if (*p == (K_AsnContextSpecific | K_AsnPrimitive | 2))
          {
            p++;
            tmpLen = *p++;
            if (extEnd - p < tmpLen)
            return 0;
          }
          else
            if (*p == (K_AsnContextSpecific | K_AsnPrimitive | 6))
            {
              p++;
              tmpLen = *p++;
              if (extEnd - p < tmpLen)
                return 0;
            }
            else
              if (*p == (K_AsnContextSpecific | K_AsnPrimitive | 1))
              {
                p++;
                tmpLen = *p++;
                if (extEnd - p < tmpLen)
                  return 0;
              }
              else
              {
                p++;
                tmpLen = *p++;
                if (extEnd - p < tmpLen)
                  return 0;
              }
          p = p + tmpLen;
          len -= tmpLen + 2; /* the magic 2 is the type and length */
        }
        break;

/***********************************************************************/
#ifdef USE_FULL_CERT_PARSE

#if defined(USE_FULL_CERT_PARSE_AUTHKEYID)
      case K_OIdExtAuthKeyId:
/*
  AuthorityKeyIdentifier ::= SEQUENCE {
    keyIdentifier[0] K            KeyIdentifier OPTIONAL,
    authorityCertIssuer[1]        GeneralNames OPTIONAL,
    authorityCertSerialNumber[2]  CertificateSerialNumber OPTIONAL }

  KeyIdentifier ::= OCTET STRING
*/
        if (x509_get_sequence(&p,(int)(extEnd - p),&len) == 0 || len < 1)
          return 0;
/*
  All memebers are optional
*/
        if (*p == (K_AsnContextSpecific|K_AsnPrimitive | 0))
        {
          p++;
          if (x509_get_asn_length(&p,(int)(extEnd - p),&keyLen) == 0 ||
              extEnd - p < keyLen)
            return 0;
/* extensions->ak.keyId = psMalloc(extensions->ak.keyLen); */
/* memcpy(extensions->ak.keyId, p, extensions->ak.keyLen); */
          p = p + keyLen;
        }

        if (*p == (K_AsnContextSpecific | K_AsnConstructed | 1))
        {
          p++;
          if (x509_get_asn_length(&p,(int)(extEnd - p),&len) == 0 ||
              len < 1 || extEnd - p < len)
          return 0;
          if ((*p ^ K_AsnContextSpecific ^ K_AsnConstructed) != 4)
            /*
              FUTURE: support other name types
              We are just dealing with DN formats here
            */
            return 0;
          p++;
          if (x509_get_asn_length(&p,(int)(extEnd - p),&len) == 0 ||
              extEnd - p < len)
            return 0;
          if (x509_get_dn_attrib(&p,(int)(extEnd - p),NULL) == 0)
            return 0;
        }

        /* Certficiate Serial Number can be a really big big integer
         * practically.  Since we don't need this piece of
         * information, just skip this block entirely unless we are
         * interested in this entity.
         */
#if 0
        if (*p == (K_AsnContextSpecific|K_AsnPrimitive | 2))
        {
          BYTE * ptr = p, temp;
          temp = *p;
          /*
            Standard x509_get_integer doesn't like CONTEXT_SPECIFIC tag
          */
          *p &= K_AsnInteger;
          if (x509_get_integer(&p,(int)(extEnd - p),&serialNum) == 0)
          {
            *ptr = temp;
            return 0;
          }
          *ptr = temp;
        }
#else
        if ( *p == (K_AsnContextSpecific|K_AsnPrimitive | 2) )
        {
          int bilen;            /* big integer length */
          p++;
          if ( x509_get_asn_length(&p,(int)(extEnd-p),&bilen) == 0 )
            return 0;
          p += bilen;
        }
#endif
        break;
#endif  /* USE_FULL_CERT_PARSE_AUTHKEYID */

#if defined(USE_FULL_CERT_PARSE_KEYUSAGE)
      case K_OIdExtKeyUsage:
/*
  KeyUsage ::= BIT STRING {
    digitalSignature    (0),
    nonRepudiation      (1),
    keyEncipherment     (2),
    dataEncipherment    (3),
    keyAgreement        (4),
    keyCertSign         (5),
    cRLSign             (6),
    encipherOnly        (7),
    decipherOnly        (8) }
*/
        if (*p++ != K_AsnBitString)
          return 0;
        if (x509_get_asn_length(&p,(int)(extEnd - p),&len) == 0 ||
            extEnd - p < len)
          return 0;
        
        if ( len < 1 )
        {
          
          return 0;
          
        } else {
          
          /* Bit string may be of variable length due to future
           * expansion.  According to RFC5280, only 9 bits are
           * defined.  It is possible to have this bit-string size
           * more than 2 octets for the time being.
           */
          DWORD d_usage = 0;
          
          /* dbg_printf(" -- key usage pattern\r\n"); */
          /* dbg_hexdump(p,len); */
          
          /* bit string structure -
           *   nn mm ll ....
           *    -- nn is a initial byte descibing the how many unused
           *       bits at the last bit-string content byte.
           *    -- bit 0x80 of mm is the bit at position 0.
           *       bit 0x40 of mm is the bit at position 1.
           *       ...
           *       bit 0x01 of mm is the bit at position 7.
           *
           *    -- bit 0x80 of nn is the bit at position 
           * leading initial byte (nn) must take the value within
           * 0 to 7 which denotes number of unused bits in last bytes
           */
          if ( len < 1 || *p > 7 )
            return 0;           /* initial byte must be within 0 to 7 */
          if ( len > 1 )
            d_usage |= p[1] << 24;
          if ( len > 2 )
            d_usage |= p[2] << 16;
          /* do a 16-bits reversal */
          d_usage = x509_bit_reverse(d_usage);
          a_cert_info->w_key_usage &= 0xFE00;
          a_cert_info->w_key_usage |= d_usage & 0x1FF;
          a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_HasKeyUsage;
        };
        p = p + len;
        break;
#endif  /* USE_FULL_CERT_PARSE_EXTKEYUSAGE */

#if defined(USE_FULL_CERT_PARSE_SUBJKEYID)
      case K_OIdExtSubjKeyId:
/*
  The value of the subject key identifier MUST be the value
  placed in the key identifier field of the Auth Key Identifier
  extension of certificates issued by the subject of
  this certificate.
*/
        if (*p++ != K_AsnOctetString ||
            x509_get_asn_length(&p,(int)(extEnd - p),&keyLen) == 0 ||
            extEnd - p < keyLen)
          return 0;
/* extensions->sk.id = psMalloc(extensions->sk.len); */
/* memcpy(extensions->sk.id, p, extensions->sk.len); */
        p = p + keyLen;
        break;
#endif
#endif /* USE_FULL_CERT_PARSE */
/*
  Unsupported or skipping because USE_FULL_CERT_PARSE is undefined
*/
/***********************************************************************/

      default:
// syspack V1.2 9-7-2012
#if 0
        if (critical)
          return 0;
#endif
        p++;
        if (x509_get_asn_length(&p,(int)(extEnd - p),&len) == 0 ||
            extEnd - p < len)
          return 0;
        p = p + len;
        break;
    }
  }
  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_SerialNum(BYTE ** a_pp,int a_len)
{
  BYTE *  p;
  int     vlen;

  p = *a_pp;
  if (a_len < 1 || *(p++) != K_AsnInteger ||
      x509_get_asn_length(&p,a_len-1,&vlen) == 0)
    return 0;

   p += vlen;
  *a_pp = p;
  return -1;
}

static BOOLEAN x509_rsapss_param_identical(T_RSAPSS_PARAM *s1, T_RSAPSS_PARAM *s2)
{
  if ( s1 == NULL || s2 == NULL )
    return FALSE;
  if ( s1 == s2 )
    return TRUE;
  return (memcmp(s1,s2,sizeof(T_RSAPSS_PARAM))) ? FALSE : TRUE;
}

static int x509_get_rsapss_param(BYTE ** a_pp, int a_len, T_RSAPSS_PARAM *param)
{

  /* -------------------------------------------------------------------
   * RFC3447 PKCS#1 v2.1 (Appendix A.2.3)
   * RSASSA-PSS-params ::= SEQUENCE {
   *   hashAlgorithm [0] HashAlgorithm DEFAULT sha1,
   *   maskGenAlgorithm [1] MaskGenAlgorithm DEFAULT mgf1SHA1,
   *   saltLength [2] INTEGER DEFAULT 20,
   *   trailerField [3] TrailerField DEFAULT trailerFieldBC
   * }
   * ------------------------------------------------------------------- */
  BYTE *p, *p_e, *p_pe;
  BYTE *end;
  int arcLen, llen;
  BYTE tag;
  const BYTE b_ctx_struct = K_AsnContextSpecific | K_AsnConstructed;

  p = *a_pp;
  end = p + a_len;

  if ( a_len < 2 || x509_get_sequence(&p,a_len,&llen) == 0 )
    return 0;

  param->d_hash_algo = K_OIdSha1;
  param->d_mask_gen_func = K_OIdMgf1;
  param->d_mgf_digest = K_OIdSha1;
  param->d_salt_length = 20;
  param->d_trailer_field = 1;

  if ( llen == 0 )
  {
    /* dbg_printf("empty PSS param, use default settings...\n"); */
    *a_pp = p;
    /* use default PSS parameters */
    return -1;
  };

  /* dbg_printf("PSS param content size = %d\n",llen); */
  if ( end - p < llen )
    return 0;

  /* all context-specific constucture are using tag 0~3, minimal
   * length of tag id and length field is 2 bytes */
  p_e = p + llen;
  while ( (int)(p_e - p) >= 2 ) {

    /* dbg_printf("\ntag %x\n",*p); */
    tag = *p++;

    if ( (tag & b_ctx_struct) != b_ctx_struct )
      return 0;
    
    tag &= 0x1f;
    if ( tag > 3 )
      return 0;                 /* error! */

    if ( x509_get_asn_length(&p,(int)(p_e - p),&arcLen) == 0 )
      return 0;

    /* p_e  - points to end of PSS-PARAMS
     * p_pe - points to end of this context-specific struct */
    p_pe = p + arcLen;
    /* dbg_printf("\ntag %d len = %d\n",tag, arcLen); */
    /* dbg_hexdump(p,arcLen); */

    switch ( tag )
    {
      case 0:
      /* hashAlgorithm [0] HashAlgorithm DEFAULT sha1
       *  - an OID in the set of OAEP-PSSDigestAlgorithms */
      if ( (int)(p_pe - p) < 2 || x509_get_sequence(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;
      if ( (int)(p_pe - p) < 2 || x509_get_oid(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;

      param->d_hash_algo = oid_to_hash_value(p,arcLen);
      /* dbg_printf("tag %u oid hash 0x%08x\n",tag,param->d_hash_algo); */
      break;

      case 1:
      /* maskGenAlgorithm [1] MaskGenAlgorithm DEFAULT mgf1SHA1 */
      if ( (int)(p_pe - p) < 2 || x509_get_sequence(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;
      if ( (int)(p_pe - p) < 2 || x509_get_oid(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;
        
      param->d_mask_gen_func = oid_to_hash_value(p,arcLen);
      /* dbg_printf("tag %u oid hash #1 0x%08x\n",tag,param->d_mask_gen_func); */
      p += arcLen;

      if ( (int)(p_pe - p) < 2 || x509_get_sequence(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;
      if ( (int)(p_pe - p) < 2 || x509_get_oid(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;
      param->d_mgf_digest = oid_to_hash_value(p,arcLen);
      /* dbg_printf("tag %u oid hash #2 0x%08x\n",tag,param->d_mgf_digest); */
      break;

    case 2:
      /* saltLength [2] INTEGER DEFAULT 20 */
      if ( (int)(p_pe - p) < 2 || x509_get_integer(&p,(int)(p_pe-p),&arcLen) == 0 )
      return 0;
      param->d_salt_length = arcLen;
      /* dbg_printf("tag %u salt-length = %u\n",tag,param->d_salt_length); */
      break;

    case 3:
      /* trailerField [3] TrailerField DEFAULT trailerFieldBC */
      if ( (int)(p_pe - p) < 2 || x509_get_integer(&p,(int)(p_pe-p),&arcLen) == 0 )
	return 0;
      param->d_trailer_field = arcLen;
      /* dbg_printf("tag %u trailer-field = %u\n",tag,param->d_trailer_field); */
      break;
    };

    p = p_pe;
    
  };

  *a_pp = p_e;

  if ( param->d_hash_algo != param->d_mgf_digest )
    return 0;
  /* only 0x1 are defined for the d_trailer_field so far. */ 
  if ( param->d_trailer_field != 1 )
    return 0;
  /* only id-mgf1 are defined for mask function */
  if( param->d_mask_gen_func != K_OIdMgf1 )
    return 0;

  return -1;
  
}

/*---------------------------------------------------------------------*/
static int x509_get_algorithm_identifier(BYTE **a_pp, int a_len, int *a_oi, T_RSAPSS_PARAM *a_pss, int a_isPubKey)
{
  BYTE *  p;
  BYTE *  end;
  int     arcLen,llen;

  p = *a_pp;
  end = p + a_len;

  if (a_len < 1 || x509_get_sequence(&p,a_len,&llen) == 0)
    return 0;

  if (end - p < 1)
    return 0;

  if (*(p++) != K_AsnOId || x509_get_asn_length(&p,(int)(end - p),&arcLen) == 0 ||
      llen < arcLen)
    return 0;
  /*
    List of expected (currently supported) OIDs
    algorithm               summed  length  hex
    s_x509_sha1             88      05      2b0e03021a
    md2                     646     08      2a864886f70d0202
    s_x509_md5              649     08      2a864886f70d0205
    rsaEncryption           645     09      2a864886f70d010101
    md2WithRSAEncryption    646     09      2a864886f70d010102
    md5WithRSAEncryption    648     09      2a864886f70d010104
    sha-1WithRSAEncryption  649     09      2a864886f70d010105
    sha224WithRSAEncryption 658     09      2a864886f70d01010e
    sha256WithRSAEncryption 655     09      2a864886f70d01010b
    sha384WithRSAEncryption 656     09      2a864886f70d01010c
    sha512WithRSAEncryption 657     09      2a864886f70d01010d

    Yes, the summing isn't ideal (as can be seen with the duplicate 649),
    but the specific implementation makes this ok.
  */
  if (end - p < 2)
    return 0;

  if (a_isPubKey && (*p != 0x2a) && (*(p + 1) != 0x86))
    /*
      Expecting DSA here if not RSA, but OID doesn't always match
    */
    return 0;

#if 0
  *a_oi = 0;
  while (arcLen-- > 0)
    *a_oi += (int)*p++;
#else
  *a_oi = oid_to_hash_value(p,arcLen);
  p += arcLen;
#endif

  /* Kong: PKCS #1 v2.1 has specified two kind of signature padding schemem.
   *       Either RSASSA-PSS or RSASSA-PKCS1-V1_5.
   *       Special care should be applied to RSASSA-PSS algorithm ID.
   *       RSASSA-PKCS1-V1_5 all have a NULL following the algorithm OID.
   *       However, it is another sequence of parameters block after
   *       RSASSA-PSS OID.
   */

  if ( *a_oi != K_OIdRsaPSS )
  {

    /* assume RSASSA-PKCS1-V1_5 algorithm IDs */

    /*
      Each of these cases should have a trailing NULL parameter.  Skip it
    */
    if (end - p < 2)
      return 0;

    if (*p != K_AsnNull)
    {
      return 0;
    };
    *a_pp = p + 2;
    return -1;
    
  } else {

    /* parameter description of PSS is a constructred sequence */
    if ( x509_get_rsapss_param(&p,(int)(end-p),a_pss) == 0 )
    {
      return 0;
    }

    *a_pp = p;
    return -1;
  }
}

/*---------------------------------------------------------------------*/
static void x509_generalize_time(BYTE a_timeFormat, int a_timeLen, BYTE* a_inTime, BYTE* a_outTime)
{
  if (a_timeFormat == K_AsnUtcTime)
  {
    if (a_inTime[0] >= '5')
    {
      a_outTime[0] = '1';
      a_outTime[1] = '9';
    }
    else
    {
      a_outTime[0] = '2';
      a_outTime[1] = '0';
    }
    memcpy(&a_outTime[2], a_inTime, a_timeLen);
    a_outTime[a_timeLen+1] = 0;
  }
  else
  {
    memcpy(a_outTime, a_inTime, a_timeLen);
    a_outTime[a_timeLen-1] = 0;
  }
}

/*---------------------------------------------------------------------*/
static int x509_get_validity(BYTE ** a_pp,int a_len)
{
  BYTE *  p;
  BYTE *  end;
  int     seqLen,timeLen;
  BYTE    timeFormat;
  BYTE    currTime[15];
  BYTE    notBefore[15];
  BYTE    notAfter[15];

  p = *a_pp;
  end = p + a_len;
  if (a_len<1 || *(p++)!=(K_AsnSequence|K_AsnConstructed) ||
      x509_get_asn_length(&p,a_len-1,&seqLen)==0 || (end-p)<seqLen)
    return 0;
/*
  Have notBefore and notAfter times in UTCTime or GeneralizedTime formats
*/
  if ((end-p)<1 || ((*p!=K_AsnUtcTime) && (*p!=K_AsnGeneralizedTime)))
    return 0;

  timeFormat = *p;

  p++;
/*
  Allocate them as null terminated strings
*/
  if (x509_get_asn_length(&p,seqLen,&timeLen)==0 || (end-p)<timeLen)
    return 0;

  /* os_rtc_get(currTime); */
  EXEC_RTC_GET(currTime);
  currTime[14] = 0;
  x509_generalize_time(timeFormat, timeLen, p, notBefore);
  if (memcmp(currTime, notBefore, 14) < 0)
    return 0;

  p = p + timeLen;
  if ((end-p)<1 || ((*p!=K_AsnUtcTime) && (*p!=K_AsnGeneralizedTime)))
    return 0;

  timeFormat = *p;

  p++;
  if (x509_get_asn_length(&p,seqLen-timeLen,&timeLen)==0 || (end-p)<timeLen)
    return 0;

  x509_generalize_time(timeFormat, timeLen, p, notAfter);
  if (memcmp(currTime, notAfter, 14) > 0)
    return 0;

  p = p + timeLen;
  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_big(BYTE ** a_pp,int a_len,BYTE * a_big,DWORD * a_keylen)
{
  BYTE *  p;
  int     vlen;

  p = *a_pp;
  if (a_len < 1 || *(p++) != K_AsnInteger ||
      x509_get_asn_length(&p,a_len-1,&vlen) == 0)
    return 0;

  if (*p == 0)
  {
    p++;
    vlen--;
  }

  if (vlen > *a_keylen)
  {
#if defined(KONG_DEBUG_X509_MODULE)
    dbg_printf("*** get_big() length error %d vs %d\r\n",
               vlen, *a_keylen);
#endif
    return 0;
  };

  *a_keylen = vlen;
  memcpy(a_big,(void*)p,vlen);

  p += vlen;
  *a_pp = p;

  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_signature(BYTE ** a_pp,int a_len,BYTE * a_sig)
{
  BYTE *  p;
  BYTE *  end;
  int     ignore_bits,llen,sigLen;

  p = *a_pp;
  end = p + a_len;
  if (a_len<1 || (*(p++)!=K_AsnBitString) ||
      x509_get_asn_length(&p,a_len-1,&llen)==0 || (end-p)<llen)
    return 0;

  ignore_bits = *p++;
/*
  We assume this is always 0.
*/
  if (ignore_bits != 0)
    return 0;

/*
  Length included the ignore_bits byte
*/
  sigLen = llen - 1;
  memcpy(a_sig,&sigLen,4);
  memcpy(a_sig + 4,p,sigLen);
  *a_pp = p + sigLen;

  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_get_pubkey(BYTE** a_pp, int a_len, T_KEY * a_pubKey)
{
  BYTE *  p;
  BYTE    value[4];
  int     pubKeyLen,ignore_bits,seqLen;
  DWORD   keyLen,expLen,count;

  p = *a_pp;
  if (a_len<1 || (*(p++)!=K_AsnBitString) ||
      x509_get_asn_length(&p,a_len-1,&pubKeyLen)==0 || (a_len-1)<pubKeyLen)
    return 0;

  ignore_bits = *p++;
/*
  We assume this is always zero
*/
  if (ignore_bits != 0)
    return 0;

  expLen = 4;
#if 0
  keyLen = 258;
#else
  keyLen = sizeof(a_pubKey->s_key);
#endif
  if (x509_get_sequence(&p,pubKeyLen,&seqLen) == 0 ||
      x509_get_big(&p,seqLen,(BYTE*)a_pubKey->s_key,&keyLen) == 0 ||
      x509_get_big(&p,seqLen,(BYTE*)value,&expLen) == 0)
    return 0;

#ifdef _PCI_  /* Accept only 2048 bits key or above */
  if (keyLen < 256)
    return 0;
#endif

  a_pubKey->d_keysize  = keyLen;
  a_pubKey->d_exponent = 0;
  for (count=0;count<expLen;count++)
    a_pubKey->d_exponent = (a_pubKey->d_exponent<<8) | value[count];

  *a_pp = p;
  return -1;
}

/*---------------------------------------------------------------------*/
static int x509_unpad_rsa(BYTE * a_ptr,DWORD a_len)
{
  BYTE *c, *end;

  c   = a_ptr;
  end = c + a_len;
  if (*c++ != 0x00 || *c != 0x01)
    return -1;

  c++;
  while (c < end && *c != 0x00)
  {
    if (*c != 0xFF)
      return -1;
    c++;
  }
  c++;

  return end - c;
}

static BOOLEAN x509_select_hashops(T_HASH_OPS *a_hash_ops, DWORD algo_oid_hash)
{
  switch ( algo_oid_hash )
  {
#ifndef _PCI_
    case K_OIdMd5:
    case K_OIdRsaMd5:
      return hashops_get(a_hash_ops,E_HashOps_AlgoId_md5);
#endif

    case K_OIdSha1:
    case K_OIdRsaSha1:
      return hashops_get(a_hash_ops,E_HashOps_AlgoId_sha1);

    case K_OIdSha224:
    case K_OIdRsaSha224:
      return hashops_get(a_hash_ops,E_HashOps_AlgoId_sha224);

    case K_OIdSha256:
    case K_OIdRsaSha256:
      return hashops_get(a_hash_ops,E_HashOps_AlgoId_sha256);

    case K_OIdSha384:
    case K_OIdRsaSha384:
      return hashops_get(a_hash_ops,E_HashOps_AlgoId_sha384);

    case K_OIdSha512:
    case K_OIdRsaSha512:
      return hashops_get(a_hash_ops,E_HashOps_AlgoId_sha512);
  };

  return FALSE;
}

/*---------------------------------------------------------------------*/
DWORD x509_parse_cert(BYTE *a_ptr, DWORD a_size, T_X509_CERT_INFO *a_cert_info)
{
  BYTE *p;
  BYTE *certStart;
  BYTE *certEnd;
  BYTE *end;
  /* BYTE *sigptr; */
  /* BYTE *sigend; */
  int  len,certLen;
  int  certVersion,certAlgorithm,pubKeyAlgorithm,sigAlgorithm;

  T_RSAPSS_PARAM certAlgorithm_pss_param;
  T_RSAPSS_PARAM anyAlgorithm_pss_param;
  DWORD certDigestAlgo;

  T_HASH_OPS     hash_ops;
  T_HASH_OPS_CTX hash_ctx;
  BYTE           hash_digest[K_HashOps_Digest_Max];

  p   = a_ptr;
  len = 0;
  end = p + a_size;

  /* general input parameters checking */
  if ( a_ptr == NULL || a_size == 0 || a_cert_info == NULL )
    return K_X509ErrGeneral;

  memset(a_cert_info,0,sizeof(*a_cert_info));
  memset(a_cert_info->s_issuer_dn_mac,0xff,K_X509_MacLen_Sha1);

/*
  Certificate ::= SEQUENCE  {
                    tbsCertificate      TBSCertificate,
                    signatureAlgorithm  AlgorithmIdentifier,
                    signatureValue      BIT STRING  }
*/
  if (x509_get_sequence(&p,a_size,&len) == 0)
    return K_X509ErrCertSequence;

  certStart = p;
/*
  TBSCertificate ::=  SEQUENCE  {
      version[0] EXPLICIT Version DEFAULT v1,
      serialNumber CertificateSerialNumber,
      signature AlgorithmIdentifier,
      issuer Name,
      validity Validity,
      subject Name,
      subjectPublicKeyInfo SubjectPublicKeyInfo,
      issuerUniqueID[1] IMPLICIT UniqueIdentifier OPTIONAL,
        -- If present, version shall be v2 or v3
      subjectUniqueID[2] IMPLICIT UniqueIdentifier OPTIONAL,
        -- If present, version shall be v2 or v3
      extensions[3] EXPLICIT Extensions OPTIONAL
        -- If present, version shall be v3  }
*/
  if (x509_get_sequence(&p,(DWORD)(end - p),&len) == 0)
    return K_X509ErrMainSequence;

  certEnd = p + len;
  certLen = (DWORD)(certEnd - certStart);

/*
  Version ::= INTEGER  {  v1(0), v2(1), v3(2)  }
*/
  if (x509_get_explicit_version(&p,(int)(end - p),0,&certVersion) == 0)
    return K_X509ErrExplicit;

/*
  CertificateSerialNumber ::= INTEGER
*/
  if (x509_get_SerialNum(&p,(int)(end - p)) == 0)
    return K_X509ErrSerialNum;

/*
  AlgorithmIdentifier ::= SEQUENCE  {
                  algorithm   OBJECT IDENTIFIER,
                  parameters  ANY DEFINED BY algorithm OPTIONAL }
*/
  if (x509_get_algorithm_identifier(&p,(int)(end - p),&certAlgorithm,&certAlgorithm_pss_param,0) == 0)
    return K_X509ErrCertAlg;

/*
  Name ::= CHOICE {
    RDNSequence }

    RDNSequence ::= SEQUENCE OF RelativeDistinguishedName

    RelativeDistinguishedName ::= SET OF AttributeTypeAndValue

    AttributeTypeAndValue ::= SEQUENCE {
      type  AttributeType,
      value AttributeValue }

    AttributeType ::= OBJECT IDENTIFIER

    AttributeValue ::= ANY DEFINED BY AttributeType
*/
  if (x509_get_dn_attrib(&p,(int)(end - p),a_cert_info->s_issuer_dn_mac) == 0)
    return K_X509ErrDnAttrib;

/*
  Validity ::= SEQUENCE {
    notBefore Time,
    notAfter  Time  }
*/
  if (x509_get_validity(&p,(int)(end - p)) == 0)
    return K_X509ErrValId;

/*
  Subject DN
*/
  if (x509_get_dn_attrib(&p,(int)(end - p),a_cert_info->s_subject_dn_mac) == 0)
    return K_X509ErrSubDn;

/*
  SubjectPublicKeyInfo  ::=  SEQUENCE  {
    algorithm   AlgorithmIdentifier,
    subjectPublicKey  BIT STRING  }
*/
  if (x509_get_sequence(&p,(int)(end - p),&len) == 0)
    return K_X509ErrPubSeq;

  if (x509_get_algorithm_identifier(&p,(int)(end - p),&pubKeyAlgorithm,&anyAlgorithm_pss_param,1) == 0)
    return K_X509ErrPubAlg;
  if ( pubKeyAlgorithm != K_OIdRsaEncryption )
    return K_X509ErrPubAlg;

  if (x509_get_pubkey(&p,(int)(end - p),&a_cert_info->s_public_key) == 0)
    return K_X509ErrPubKey;
  /* dbg_printf("x509 public key size = %d\r\n", */
  /*            a_cert_info->s_public_key.d_keysize); */

  if (*p != (K_AsnSequence | K_AsnConstructed))
    if (x509_get_implicit_bit_string(&p,(int)(end - p),K_ImplicitIssuerId) == 0 ||
        x509_get_implicit_bit_string(&p,(int)(end - p),K_ImplicitSubjectId) == 0 ||
        x509_get_explicit_extensions(&p,(int)(end - p),K_ExplicitExtension,a_cert_info) == 0)
      return K_X509ErrExternsion;

  if (certEnd != p)
    return K_X509ErrCertEnd;

  if (x509_get_algorithm_identifier(&p,(int)(end - p),&sigAlgorithm,&anyAlgorithm_pss_param,0) == 0)
    return K_X509ErrSigAlg;

  if (certAlgorithm != sigAlgorithm)
    return K_X509ErrSigUnmatch;

  if ( certAlgorithm == K_OIdRsaPSS &&
       !x509_rsapss_param_identical(&certAlgorithm_pss_param,
                                    &anyAlgorithm_pss_param) )
    return K_X509ErrSigUnmatch;

  memset(hash_digest,0,sizeof(hash_digest));
  if ( (certDigestAlgo = certAlgorithm) == K_OIdRsaPSS )
    certDigestAlgo = certAlgorithm_pss_param.d_hash_algo;

  if ( x509_select_hashops(&hash_ops,certDigestAlgo) == FALSE )
    return K_X509ErrPubAlg;
  
  hash_ops.init(&hash_ctx);
  hash_ops.update(&hash_ctx,certStart,certLen);
  hash_ops.finish(&hash_ctx,a_cert_info->s_digest);
  a_cert_info->w_digest_algo_id = hash_ops.algo_type_id;
  a_cert_info->w_digest_len = hash_ops.dgst_len;

  if (x509_get_signature(&p,(int)(end - p),s_x509_sign) == 0)
    return K_X509ErrSignature;

  do {
    DWORD sig_len = s_x509_sign[0] + (s_x509_sign[1]<<8) + (s_x509_sign[2]<<16) + (s_x509_sign[3]<<24);
    a_cert_info->w_sign_len = sig_len;
    memcpy(a_cert_info->s_signature,&s_x509_sign[4],sig_len);
  } while ( 0 );

  if ( certAlgorithm == K_OIdRsaPSS ) {
    a_cert_info->s_rsapss_param = certAlgorithm_pss_param;
    a_cert_info->w_digest_algo_id |= K_X509_AlgoId_PSS_Mask;
  };
  
  dbg_printf("x509 certVersion     = 0x%08x\r\n",certVersion);
  dbg_printf("x509 certAlgorithm   = 0x%08x\r\n",certAlgorithm);
  dbg_printf("x509 pubKeyAlgorithm = 0x%08x\r\n",pubKeyAlgorithm);
  dbg_printf("x509 sigAlgorithm    = 0x%08x\r\n",sigAlgorithm);
  dbg_printf("x509 digest algo     = 0x%08x\r\n",a_cert_info->w_digest_algo_id);
  if ( certAlgorithm == K_OIdRsaPSS ) {
    dbg_printf("x509 -- pss hash     0x%08x\r\n",certAlgorithm_pss_param.d_hash_algo);
    dbg_printf("x509 -- pss mgf      0x%08x\r\n",certAlgorithm_pss_param.d_mask_gen_func);
    dbg_printf("x509 -- pss mgf-dig  0x%08x\r\n",certAlgorithm_pss_param.d_mgf_digest);
    dbg_printf("x509 -- pss salt-len %u\r\n",certAlgorithm_pss_param.d_salt_length);
    dbg_printf("x509 -- pss trailer  %u\r\n",certAlgorithm_pss_param.d_trailer_field);
  };

  /* Find out that this certificate is self-signed or not.  We have
   * simplified to checking issuer and subject DNs SHA1 digest hash
   * value and verify certficate signature against its public key.
   * There is no need to check key usage flag in this kind of checking.
   */
  if ( ( memcmp(a_cert_info->s_issuer_dn_mac,
		a_cert_info->s_subject_dn_mac,
		K_X509_MacLen_Sha1) == 0 ) &&
       (x509_verify_cert_by_key(a_cert_info,&a_cert_info->s_public_key)) )
  {
    a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_SelfSigned;
  };

  /* KeyUsage extension is not mandatory.  It wiil be absent if there
   * is restriction are being imposed to subject's public key usages.
   * Apply default key usage value for normal SSL/TLS application.
   * Additionally, certificate signing and CRL list verficiation
   * capability are added if this certificate is flagged as CA. */
  if ( !(a_cert_info->w_key_usage & K_X509_KeyUsageFlag_HasKeyUsage ) )
  {
    a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_digitalSign;
    a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_contentCommit;
    a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_keyEncipher;  /* for SSL/TLS */
    a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_keyAgreement; /* for DH algo */
    /* a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_dataEncipher; */
    if ( a_cert_info->w_key_usage & K_X509_KeyUsageFlag_IsCA )
    {
      a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_keyCertSign;
      a_cert_info->w_key_usage |= K_X509_KeyUsageFlag_cRLSign;
    };
  };

  return K_X509Ok;
}

static BOOLEAN x509_check_leading_zeros(BYTE *p, int bits)
{
  if ( bits <= 0 )
    return TRUE;

  /* dbg_printf("leading zero %d\n",bits); */
  
  while ( bits >= 8 ) {
    if ( *p++ )
      return FALSE;
    bits -= 8;
  };

  if ( bits ) {
    BYTE mask;
    mask = (0xff << (8-bits)) & 0xff;
    /* dbg_printf("last mask %02x\n",mask); */
    if ( *p & mask )
      return FALSE;
  };

  return TRUE;
}

static void x509_set_leading_zeros(BYTE *p, int bits)
{
  if ( bits <= 0 )
    return;

  /* dbg_printf("leading zero %d\n",bits); */
  
  while ( bits >= 8 ) {
    *p++ = 0;
    bits -= 8;
  };

  if ( bits ) {
    BYTE mask;
    mask = (0xff >> bits) & 0xff;
    /* dbg_printf("last mask %02x\n",mask); */
    *p &= mask;
  };
}

/* ----------------------------------------------------------------------
 * x509_mgf1_xor
 * ----------------------------------------------------------------------
 *   - calculate the MGF1(hash) noise mask and XOR the target content
 *     block.
 *   - definition of MGF1 can be found from PKCS#1 v2.1 (RFC3447).
 * ---------------------------------------------------------------------- */
static void x509_mgf1_xor(BYTE *seed,
                          int seed_len,
                          int mask_len,
                          BYTE *xor_this,
			  T_HASH_OPS *p_hash_ops)
{
  BYTE cnt_buf[4];
  DWORD cnt_max, cnt;
  BYTE *d;
  int out_bytes, i;

  if ( p_hash_ops == NULL ||
       p_hash_ops->dgst_len == 0 ||
       p_hash_ops->p_ctx == NULL ||
       p_hash_ops->p_digest == NULL )
    return;

  cnt_max = mask_len / p_hash_ops->dgst_len;
  if ( mask_len % p_hash_ops->dgst_len )
    cnt_max++;

  d = xor_this;
  memset(cnt_buf,0,sizeof(cnt_buf));

  for ( cnt = 0; cnt < cnt_max; ++cnt )
  {
    p_hash_ops->init(p_hash_ops->p_ctx);
    p_hash_ops->update(p_hash_ops->p_ctx,seed,seed_len);
    p_hash_ops->update(p_hash_ops->p_ctx,cnt_buf,sizeof(cnt_buf));
    p_hash_ops->finish(p_hash_ops->p_ctx,p_hash_ops->p_digest);

    if ( mask_len > p_hash_ops->dgst_len )
      out_bytes = p_hash_ops->dgst_len;
    else
      out_bytes = mask_len;

    mask_len -= out_bytes;

    for ( i=0; i<out_bytes; ++i )
      *d++ ^= p_hash_ops->p_digest[i];

    for ( i=sizeof(cnt_buf)-1; i>=0; i-- ) {
      cnt_buf[i]++;
      if ( cnt_buf[i] != 0 )
	break;
    };
  }
}

static BOOLEAN x509_verify_cert_with_PSS(T_X509_CERT_INFO *a_cert, T_KEY *a_key)
{
  /* perform EMSA-PSS-VERIFY(hash,EM,modBits-1) */
  BYTE *EM    = s_x509_sign;
  int   emLen = a_cert->w_sign_len;
  BYTE *mHash = a_cert->s_digest;
  int   hLen  = a_cert->w_digest_len;
  int modBits = a_cert->w_sign_len << 3;
  int emBits = modBits - 1;
  int zBits, dbLen, zBytes;
  T_RSAPSS_PARAM *p_pss = &a_cert->s_rsapss_param;
  BYTE *H, *DB;
  BYTE *salt;
  T_HASH_OPS hash_ops;
  T_HASH_OPS_CTX hash_ctx;
  BYTE digest[K_HashOps_Digest_Max];

  dbg_printf("x509 verify using RSARSS-PSS padding scheme.\r\n");

  if ( !x509_select_hashops(&hash_ops,p_pss->d_hash_algo) )
  {
    dbg_printf("x509 hash algo operations selection error.\r\n");
    return FALSE;
  }

  hash_ops.p_digest = digest;
  hash_ops.p_ctx = &hash_ctx;

  memset(s_x509_sign,0,sizeof(s_x509_sign));
  memcpy(s_x509_sign,a_cert->s_signature,a_cert->w_sign_len);

  /* execute RSA once! */
  EXEC_RSA(s_x509_sign,a_key);  

  /* M->mHash and hLen, as a side-effect of certificate parsing */
  if ( emLen < hLen + p_pss->d_salt_length + 2 )
    return FALSE;

  /* dbg_printf("EM (len=%d)-\n",emLen); */
  /* dbg_hexdump(EM,emLen); */
    
  /* p_pss->d_trail_field == 1 => trail_field is 0xbc */
  if ( EM[emLen-1] != 0xbc )
    return FALSE;

  DB = EM;                      /* DB's length = emLen - hLen - 1 */
  dbLen = emLen - hLen - 1;
  H = DB + dbLen;

  zBits = (emLen << 3) - emBits;
  if ( !x509_check_leading_zeros(DB,zBits) )
    return FALSE;

  /* dbg_printf("before masking -\n"); */
  /* dbg_hexdump(EM,emLen); */

  /* unmask DB against MGF1-algo mask function */
  x509_mgf1_xor(H,hLen,dbLen,DB,&hash_ops);
  x509_set_leading_zeros(DB,zBits);

  /* dbg_printf("demasked - (dbLen=%d)\n",dbLen); */
  /* dbg_hexdump(EM,emLen); */

  zBytes = emLen - hLen - p_pss->d_salt_length - 2;
  if ( !x509_check_leading_zeros(DB,zBytes<<3) )
    return FALSE;

  if ( DB[zBytes] != 0x01 )
    return FALSE;

  salt = DB + dbLen - p_pss->d_salt_length;
  dbg_printf("x509 salt -\r\n");
  dbg_hexdump(salt,p_pss->d_salt_length);

  dbg_printf("x509 H -\r\n");
  dbg_hexdump(H,hLen);

  memset(hash_ops.p_digest,0,8);
  hash_ops.init(hash_ops.p_ctx);
  hash_ops.update(hash_ops.p_ctx,hash_ops.p_digest,8);
  hash_ops.update(hash_ops.p_ctx,mHash,hLen);
  hash_ops.update(hash_ops.p_ctx,salt,p_pss->d_salt_length);
  hash_ops.finish(hash_ops.p_ctx,hash_ops.p_digest);

  dbg_printf("x509 H' -\r\n");
  dbg_hexdump(hash_ops.p_digest,hLen);

  /* dbg_printf("left most %d must be zero\n", */
  /*            emLen - hLen - p_pss->d_salt_length - 2); */

  if ( memcmp(hash_ops.p_digest,H,hLen) == 0 )
    return TRUE;
  else
    return FALSE;
}

static BOOLEAN x509_verify_cert_with_PKCS1_v1_5(T_X509_CERT_INFO *a_cert,
                                                T_KEY *a_key)
{
  int sign_len, l, algo;
  BYTE *p, *p_e;
  T_RSAPSS_PARAM pss;
  
  memset(s_x509_sign,0,sizeof(s_x509_sign));
  memcpy(s_x509_sign,a_cert->s_signature,a_cert->w_sign_len);

  dbg_printf("x509 verify using PKCS#1 v1.5 padding scheme.\r\n");

  /* execute RSA once! */
  EXEC_RSA(s_x509_sign,a_key);
  sign_len = x509_unpad_rsa(s_x509_sign,a_cert->w_sign_len);
  if ( sign_len <= 0 )
  {
    dbg_printf("x509 RSA unpack fails\r\n");
    return FALSE;
  };

  p = s_x509_sign;
  p_e = p + a_cert->w_sign_len;
  p = p_e - sign_len;

  /* dbg_printf("Digest Info location...\n"); */
  /* dbg_hexdump(p,sign_len); */
  
  if ( x509_get_sequence(&p,sign_len,&l) == 0 )
    return FALSE;

  if ( x509_get_algorithm_identifier(&p,(int)(p_e-p),&algo,&pss,FALSE) == 0 )
    return FALSE;

  if ( (*p++ != K_AsnOctetString) ||
       x509_get_asn_length(&p,(int)(p_e-p),&l) == 0 ||
       (int)(p_e-p) < l )
    return FALSE;
  
  dbg_printf("x509 last --\r\n");
  dbg_hexdump(p,(int)(p_e-p));
  
  dbg_printf("x509 cert mac --\r\n");
  dbg_hexdump(a_cert->s_digest,a_cert->w_digest_len);

  if ( a_cert->w_digest_len != l ||
       memcmp(p,a_cert->s_digest,l) != 0 )
  {
    dbg_printf("x509 digest pattern unmatched!\r\n");
    return FALSE;
  };
  
  return TRUE;
}

BOOLEAN x509_verify_cert_by_key(T_X509_CERT_INFO *a_cert, T_KEY *a_key)
{
  if ( a_cert->w_sign_len != a_key->d_keysize ) {
    dbg_printf("x509 unmatched key size...\r\n");
    return FALSE;
  };
  
  if ( a_cert->w_digest_algo_id & K_X509_AlgoId_PSS_Mask )
    return x509_verify_cert_with_PSS(a_cert,a_key);
  else
    return x509_verify_cert_with_PKCS1_v1_5(a_cert,a_key);
}

BOOLEAN x509_verify_cert_by_cert(T_X509_CERT_INFO *a_cert,
                                 T_X509_CERT_INFO *a_cert_signer)
{  
  if ( a_cert == NULL || a_cert_signer == NULL )
    return FALSE;

  /* signer must be marked as CA */
  if ( !(a_cert_signer->w_key_usage & K_X509_KeyUsageFlag_IsCA) )
    return FALSE;

  /* signer must be keyCerSign bit enabled */
  if ( !(a_cert_signer->w_key_usage & K_X509_KeyUsageFlag_keyCertSign) )
    return FALSE;

  /* signer's subject digest must match up with issuer digest */
  if ( memcmp(a_cert->s_issuer_dn_mac,
              a_cert_signer->s_subject_dn_mac,
              K_X509_MacLen_Sha1) )
    return FALSE;

  return x509_verify_cert_by_key(a_cert,&a_cert_signer->s_public_key);
}

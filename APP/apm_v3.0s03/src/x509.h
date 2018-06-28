/*
************************************
*       Module : x509.h            *
*       Name   : TF                *
*       Date   : 31-01-2013        *
************************************
*/

#ifndef _X509_H_
#define _X509_H_

#define K_X509Ok                0
#define K_X509ErrMainSequence   1
#define K_X509ErrCertSequence   2
#define K_X509ErrExplicit       3
#define K_X509ErrSerialNum      4
#define K_X509ErrCertAlg        5
#define K_X509ErrDnAttrib       6
#define K_X509ErrValId          7
#define K_X509ErrSubDn          8
#define K_X509ErrPubSeq         9
#define K_X509ErrPubAlg         10
#define K_X509ErrPubKey         11
#define K_X509ErrExternsion     12
#define K_X509ErrCertEnd        13
#define K_X509ErrSigAlg         14
#define K_X509ErrSigUnmatch     15
#define K_X509ErrSignature      16
#define K_X509ErrLenMismatch    17
#define K_X509ErrGeneral        18

/* certificate's signature value size is the same as it signer's
 * public key lenght.  This system only support 4096 bits key length.
 * Therefore the signature size is max. public key length + 4
 * bytes(for signature length field) */
#define K_X509_CertSignMaxSize     (((K_MaxKeyRsaModulusBits+7)/8) + 4)

/* max. length of digest algorithm output in bytes.  Currently, the
 * maximal is output from a SHA512 digest calculation, so it is 64
 * bytes. */
#define K_X509_CertDigestMax       64

/* signature digest length */
#define K_X509_MacLen_Md5             16
#define K_X509_MacLen_Sha1            20
#define K_X509_MacLen_Sha224          28
#define K_X509_MacLen_Sha256          32
#define K_X509_MacLen_Sha384          48
#define K_X509_MacLen_Sha512          64

/* structure to hold RSASSA-PSS padding scheme parameters */
typedef struct {
  DWORD  d_hash_algo;
  DWORD  d_mask_gen_func;
  DWORD  d_mgf_digest;
  DWORD  d_salt_length;
  DWORD  d_trailer_field;
} T_RSAPSS_PARAM;

/* structure to hold minimal information of a X.509 certificate for
 * further processing.  Element arrangment is in strange order for
 * memory packing efficiency */
typedef struct {

  WORD   w_key_usage;           /* key usage bitmask of this cert. */
  WORD   w_sign_len;            /* cert's signature size in octet */
  WORD   w_digest_algo_id;      /* cert's signature digest algorithm ID */
  WORD   w_digest_len;          /* cert's signature digest length */

  /* cert's signature octet pattern */
  BYTE   s_signature[K_X509_CertSignMaxSize];

  /* Hash value of issuer and subject distinguished names */
  BYTE   s_issuer_dn_mac[K_X509_MacLen_Sha1];
  BYTE   s_subject_dn_mac[K_X509_MacLen_Sha1];

  /* digest of the certificate structure octet streams for signature
   * verification */
  BYTE   s_digest[K_X509_CertDigestMax];

  /* if w_digest_algo_id has K_X509_AlgoId_PSS_Mask set, signature
   * is using RSASSA-PSS padding scheme.   Then, we need parameters
   * corresponding to this padding scheme. */
  T_RSAPSS_PARAM s_rsapss_param;

  /* public key of this certificate */
  T_KEY          s_public_key;
  
} T_X509_CERT_INFO;

/* to mark signature are using RSASSA-PSS signature padding scheme. */
#define K_X509_AlgoId_PSS_Mask        0x100

/* extracted certificate info bitmask value for w_key_usage */
#define K_X509_KeyUsageFlag_digitalSign   0x0001
#define K_X509_KeyUsageFlag_contentCommit 0x0002 /* or nonRepudiation */
#define K_X509_KeyUsageFlag_keyEncipher   0x0004
#define K_X509_KeyUsageFlag_dataEncipher  0x0008
#define K_X509_KeyUsageFlag_keyAgreement  0x0010
#define K_X509_KeyUsageFlag_keyCertSign   0x0020
#define K_X509_KeyUsageFlag_cRLSign       0x0040
#define K_X509_KeyUsageFlag_encipherOnly  0x0080
#define K_X509_KeyUsageFlag_decipherOnly  0x0100
#define K_X509_KeyUsageFlag_HasKeyUsage   0x2000
#define K_X509_KeyUsageFlag_SelfSigned    0x4000
#define K_X509_KeyUsageFlag_IsCA          0x8000

#if defined(__cplusplus)
extern "C" {
#endif

  extern DWORD x509_parse_cert(BYTE *a_ptr, DWORD a_size,
                               T_X509_CERT_INFO *a_cert_info);
  extern BOOLEAN x509_verify_cert_by_key(T_X509_CERT_INFO *a_cert, T_KEY *a_key);
  extern BOOLEAN x509_verify_cert_by_cert(T_X509_CERT_INFO *a_cert, T_X509_CERT_INFO *a_cert_signer);

#if defined(__cplusplus)
}
#endif

#endif


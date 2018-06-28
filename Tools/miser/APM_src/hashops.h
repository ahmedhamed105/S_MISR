#ifndef _GENERALIZED_HASH_OPS_H_
#define _GENERALIZED_HASH_OPS_H_

/* max digest size is sha512 */
#define K_HashOps_Digest_Max     (512/8)

typedef enum {
  E_HashOps_AlgoId_MIN = 0,
  E_HashOps_AlgoId_md5,         /* digest - 16 bytes */
  E_HashOps_AlgoId_sha1,        /* digest - 20 bytes */
  E_HashOps_AlgoId_sha224,      /* digest - 28 bytes */
  E_HashOps_AlgoId_sha256,      /* digest - 32 bytes */
  E_HashOps_AlgoId_sha384,      /* digest - 48 bytes */
  E_HashOps_AlgoId_sha512,      /* digest - 64 bytes */
  E_HashOps_AlgoId_MAX
} E_HashOps_AlgoId;

typedef union {
  T_MD5_CONTEXT    s_md5;
  T_SHA1_CONTEXT   s_sha1;
  T_SHA224_CONTEXT s_sha224;
  T_SHA256_CONTEXT s_sha256;
  T_SHA384_CONTEXT s_sha384;
  T_SHA512_CONTEXT s_sha512;
} T_HASH_OPS_CTX;

typedef struct {
  WORD dgst_len;
  WORD algo_type_id;
  BYTE *p_digest;
  T_HASH_OPS_CTX *p_ctx;
  void (*init)(T_HASH_OPS_CTX *ctx);
  void (*update)(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
  void (*finish)(T_HASH_OPS_CTX *ctx, BYTE *digest);
} T_HASH_OPS;

#if defined(__cplusplus)
extern "C" {
#endif

  BOOLEAN hashops_get(T_HASH_OPS *ops, E_HashOps_AlgoId algo_id);

#if defined(__cplusplus)
}
#endif

#endif

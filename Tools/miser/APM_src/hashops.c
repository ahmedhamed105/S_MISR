#include "system.h"
#include "x509.h"
#include "hashops.h"

#define EXEC_MD5_INIT(x)          os_md5_init((x))
#define EXEC_MD5_UPDATE(c,p,l)    os_md5_process((c),(p),(l))
#define EXEC_MD5_FINISH(c,d)      os_md5_finish((c),(d))

#define EXEC_SHA1_INIT(x)         os_sha1_init((x))
#define EXEC_SHA1_UPDATE(c,p,l)   os_sha1_process((c),(p),(l))
#define EXEC_SHA1_FINISH(c,d)     os_sha1_finish((c),(d))

#define EXEC_SHA224_INIT(x)       os_sha224_init((x))
#define EXEC_SHA224_UPDATE(c,p,l) os_sha224_process((c),(p),(l))
#define EXEC_SHA224_FINISH(c,d)   os_sha224_finish((c),(d))

#define EXEC_SHA256_INIT(x)       os_sha256_init((x))
#define EXEC_SHA256_UPDATE(c,p,l) os_sha256_process((c),(p),(l))
#define EXEC_SHA256_FINISH(c,d)   os_sha256_finish((c),(d))

#define EXEC_SHA384_INIT(x)       os_sha384_init((x))
#define EXEC_SHA384_UPDATE(c,p,l) os_sha384_process((c),(p),(l))
#define EXEC_SHA384_FINISH(c,d)   os_sha384_finish((c),(d))

#define EXEC_SHA512_INIT(x)       os_sha512_init((x))
#define EXEC_SHA512_UPDATE(c,p,l) os_sha512_process((c),(p),(l))
#define EXEC_SHA512_FINISH(c,d)   os_sha512_finish((c),(d))

static void hashops_init_md5(T_HASH_OPS_CTX *ctx);
static void hashops_update_md5(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
static void hashops_finish_md5(T_HASH_OPS_CTX *ctx, BYTE *digest);

static void hashops_init_sha1(T_HASH_OPS_CTX *ctx);
static void hashops_update_sha1(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
static void hashops_finish_sha1(T_HASH_OPS_CTX *ctx, BYTE *digest);

static void hashops_init_sha224(T_HASH_OPS_CTX *ctx);
static void hashops_update_sha224(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
static void hashops_finish_sha224(T_HASH_OPS_CTX *ctx, BYTE *digest);

static void hashops_init_sha256(T_HASH_OPS_CTX *ctx);
static void hashops_update_sha256(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
static void hashops_finish_sha256(T_HASH_OPS_CTX *ctx, BYTE *digest);

static void hashops_init_sha384(T_HASH_OPS_CTX *ctx);
static void hashops_update_sha384(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
static void hashops_finish_sha384(T_HASH_OPS_CTX *ctx, BYTE *digest);

static void hashops_init_sha512(T_HASH_OPS_CTX *ctx);
static void hashops_update_sha512(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len);
static void hashops_finish_sha512(T_HASH_OPS_CTX *ctx, BYTE *digest);

static T_HASH_OPS s_hash_ops_table[E_HashOps_AlgoId_MAX] = {
  {				/* E_HashOps_AlgoId_MIN */
    .dgst_len = 0,
    .init     = 0,
    .update   = 0,
    .finish   = 0,
    .algo_type_id = 0
  },
  {				/* E_HashOps_AlgoId_md5 */
    .dgst_len = K_X509_MacLen_Md5,
    .init     = hashops_init_md5,
    .update   = hashops_update_md5,
    .finish   = hashops_finish_md5,
    .algo_type_id = E_HashOps_AlgoId_md5
  },
  {				/* E_HashOps_AlgoId_sha1 */
    .dgst_len = K_X509_MacLen_Sha1,
    .init     = hashops_init_sha1,
    .update   = hashops_update_sha1,
    .finish   = hashops_finish_sha1,
    .algo_type_id = E_HashOps_AlgoId_sha1
  },
  {				/* E_HashOps_AlgoId_sha224 */
    .dgst_len = K_X509_MacLen_Sha224,
    .init     = hashops_init_sha224,
    .update   = hashops_update_sha224,
    .finish   = hashops_finish_sha224,
    .algo_type_id = E_HashOps_AlgoId_sha224
  },
  {				/* E_HashOps_AlgoId_sha256 */
    .dgst_len = K_X509_MacLen_Sha256,
    .init     = hashops_init_sha256,
    .update   = hashops_update_sha256,
    .finish   = hashops_finish_sha256,
    .algo_type_id = E_HashOps_AlgoId_sha256
  },
  {				/* E_HashOps_AlgoId_sha384 */
    .dgst_len = K_X509_MacLen_Sha384,
    .init     = hashops_init_sha384,
    .update   = hashops_update_sha384,
    .finish   = hashops_finish_sha384,
    .algo_type_id = E_HashOps_AlgoId_sha384
  },
  {				/* E_HashOps_AlgoId_sha512 */
    .dgst_len = K_X509_MacLen_Sha512,
    .init     = hashops_init_sha512,
    .update   = hashops_update_sha512,
    .finish   = hashops_finish_sha512,
    .algo_type_id = E_HashOps_AlgoId_sha512
  }
};

static void hashops_init_md5(T_HASH_OPS_CTX *ctx)
{
  EXEC_MD5_INIT(&ctx->s_md5);
}

static void hashops_update_md5(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len)
{
  EXEC_MD5_UPDATE(&ctx->s_md5,data,len);
}

static void hashops_finish_md5(T_HASH_OPS_CTX *ctx, BYTE *digest)
{
  EXEC_MD5_FINISH(&ctx->s_md5,digest);
}

static void hashops_init_sha1(T_HASH_OPS_CTX *ctx)
{
  EXEC_SHA1_INIT(&ctx->s_sha1);
}

static void hashops_update_sha1(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len)
{
  EXEC_SHA1_UPDATE(&ctx->s_sha1,data,len);
}

static void hashops_finish_sha1(T_HASH_OPS_CTX *ctx, BYTE *digest)
{
  EXEC_SHA1_FINISH(&ctx->s_sha1,digest);
}

static void hashops_init_sha224(T_HASH_OPS_CTX *ctx)
{
  EXEC_SHA224_INIT(&ctx->s_sha224);
}

static void hashops_update_sha224(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len)
{
  EXEC_SHA224_UPDATE(&ctx->s_sha224,data,len);
}

static void hashops_finish_sha224(T_HASH_OPS_CTX *ctx, BYTE *digest)
{
  EXEC_SHA224_FINISH(&ctx->s_sha224,digest);
}

static void hashops_init_sha256(T_HASH_OPS_CTX *ctx)
{
  EXEC_SHA256_INIT(&ctx->s_sha256);
}

static void hashops_update_sha256(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len)
{
  EXEC_SHA256_UPDATE(&ctx->s_sha256,data,len);
}

static void hashops_finish_sha256(T_HASH_OPS_CTX *ctx, BYTE *digest)
{
  EXEC_SHA256_FINISH(&ctx->s_sha256,digest);
}

static void hashops_init_sha384(T_HASH_OPS_CTX *ctx)
{
  EXEC_SHA384_INIT(&ctx->s_sha384);
}

static void hashops_update_sha384(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len)
{
  EXEC_SHA384_UPDATE(&ctx->s_sha384,data,len);
}

static void hashops_finish_sha384(T_HASH_OPS_CTX *ctx, BYTE *digest)
{
  EXEC_SHA384_FINISH(&ctx->s_sha384,digest);
}

static void hashops_init_sha512(T_HASH_OPS_CTX *ctx)
{
  EXEC_SHA512_INIT(&ctx->s_sha512);
}

static void hashops_update_sha512(T_HASH_OPS_CTX *ctx, BYTE *data, DWORD len)
{
  EXEC_SHA512_UPDATE(&ctx->s_sha512,data,len);
}

static void hashops_finish_sha512(T_HASH_OPS_CTX *ctx, BYTE *digest)
{
  EXEC_SHA512_FINISH(&ctx->s_sha512,digest);
}

BOOLEAN hashops_get(T_HASH_OPS *ops, E_HashOps_AlgoId algo_id)
{
  if ( (ops == NULL) && (algo_id > E_HashOps_AlgoId_MIN) && (algo_id < E_HashOps_AlgoId_MAX) )
  {
    if ( ops != NULL )
      ops->dgst_len = 0;
    return FALSE;
  } else {
    *ops = s_hash_ops_table[algo_id];
    return TRUE;
  }
}

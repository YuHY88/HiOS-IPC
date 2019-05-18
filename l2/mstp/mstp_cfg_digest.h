/*
*  Copyright (C) 2016~2018  Beijing Huahuan Electronics Co., Ltd 
*
*/
#ifndef _MSTP_CFG_DIGEST_H_
#define _MSTP_CFG_DIGEST_H_

#define CONFIG_DIGEST_KEY_LEN 16

#ifdef USE_OPENSSL
#include <openssl/hmac.h>

#define hmac_init_ctx(CTXPTR,KEY,LEN,EVP)                                     \
	do {                                                                      \
		HMAC_CTX_init((CTXPTR));                                                \
      	HMAC_Init((CTXPTR), (KEY), (LEN), (EVP));                               \
	} while (0)

#define hmac_process_bytes(CTXPTR,DPTR,LEN)  HMAC_Update((CTXPTR),(DPTR),(LEN))
#define hmac_finish_ctx(CTXPTR,CPTR,LENPTR)                                   \
	do {                                                                      \
      	HMAC_Final((CTXPTR),(CPTR),(LENPTR));                                   \
      	HMAC_CTX_cleanup((CTXPTR));                                             \
    } while (0)
#endif 

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
	 (a) += F ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	 (a) = ROTATE_LEFT ((a), (s)); \
	 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
	 (a) += G ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	 (a) = ROTATE_LEFT ((a), (s)); \
	 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
	 (a) += H ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	 (a) = ROTATE_LEFT ((a), (s)); \
	 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
	 (a) += I ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	 (a) = ROTATE_LEFT ((a), (s)); \
	 (a) += (b); \
  }


typedef struct
{
	uint32_t state[4];
  	uint32_t count[2];
  	uint8_t buffer[64];
} MSTP_MD5_CTX;

void mstp_md5_init (MSTP_MD5_CTX *);
void mstp_md5_update (MSTP_MD5_CTX *, const void *, unsigned int);
void mstp_md5_final (uint8_t *, MSTP_MD5_CTX *);

void mstp_hmac_md5 (uint8_t *, int, uint8_t *, int, uint8_t *);

void mstp_gen_digest( uint8_t * data , int len , uint8_t * digest);

void mstp_gen_cfg_digest (struct mstp_bridge *br);

#endif





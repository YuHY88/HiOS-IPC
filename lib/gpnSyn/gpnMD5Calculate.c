/**********************************************************
* file name: gpnMD5Calculate.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan coyp in 2014-12-31
*    $Id: md5.c,v 1.1 2006/06/27 13:07:43 t8m Exp $
*
*    This code implements the MD5 message-digest algorithm.
*    The algorithm is due to Ron Rivest.  This code was
*    written by Colin Plumb in 1993, no copyright is claimed.
*    This code is in the public domain; do with it what you wish.
*
*    Equivalent code is available from RSA Data Security, Inc.
*    This code has been tested against that, and is equivalent,
*    except that you don't need to include two pages of legalese
*    with every copy.
*
*    To compute the message digest of a chunk of bytes, declare an
*    MD5Context structure, pass it to MD5Init, call MD5Update as
*    needed on buffers full of bytes, and then call MD5Final, which
*    will fill a supplied 16-byte array with the digest.
*    function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_MD5_CALCULATE_C_
#define _GPN_MD5_CALCULATE_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "gpnSyn/gpnMD5Calculate.h"

/*
 * big/little-endian
 */
static void gpnMD5ByteReverse(unsigned char *buf, unsigned longs)
{
    unsigned int t;

    do
    {
        t = (unsigned int)((unsigned) buf[3] << 8 | buf[2]) << 16 |
            ((unsigned) buf[1] << 8 | buf[0]);
        *(unsigned int *) buf = t;
        buf += 4;
    }
    while (--longs);
}
/* The four core functions - F1 is optimized somewhat */

/* #define  GPN_MD5C_F1(x, y, z) (x & y | ~x & z) */
#define GPN_MD5C_F1(x, y, z) (z ^ (x & (y ^ z)))
#define GPN_MD5C_F2(x, y, z)  GPN_MD5C_F1(z, x, y)
#define GPN_MD5C_F3(x, y, z) (x ^ y ^ z)
#define GPN_MD5C_F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define GPN_MD5C_STEP(f, w, x, y, z, data, s) \
         ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void gpnMD5CalculateTransform(unsigned int buf[4], unsigned int const in[16])
{
    register unsigned int a, b, c, d;
    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];
    GPN_MD5C_STEP(GPN_MD5C_F1, a, b, c, d, in[ 0] + 0xd76aa478U,  7);
    GPN_MD5C_STEP(GPN_MD5C_F1, d, a, b, c, in[ 1] + 0xe8c7b756U, 12);
    GPN_MD5C_STEP(GPN_MD5C_F1, c, d, a, b, in[ 2] + 0x242070dbU, 17);
    GPN_MD5C_STEP(GPN_MD5C_F1, b, c, d, a, in[ 3] + 0xc1bdceeeU, 22);
    GPN_MD5C_STEP(GPN_MD5C_F1, a, b, c, d, in[ 4] + 0xf57c0fafU,  7);
    GPN_MD5C_STEP(GPN_MD5C_F1, d, a, b, c, in[ 5] + 0x4787c62aU, 12);
    GPN_MD5C_STEP(GPN_MD5C_F1, c, d, a, b, in[ 6] + 0xa8304613U, 17);
    GPN_MD5C_STEP(GPN_MD5C_F1, b, c, d, a, in[ 7] + 0xfd469501U, 22);
    GPN_MD5C_STEP(GPN_MD5C_F1, a, b, c, d, in[ 8] + 0x698098d8U,  7);
    GPN_MD5C_STEP(GPN_MD5C_F1, d, a, b, c, in[ 9] + 0x8b44f7afU, 12);
    GPN_MD5C_STEP(GPN_MD5C_F1, c, d, a, b, in[10] + 0xffff5bb1U, 17);
    GPN_MD5C_STEP(GPN_MD5C_F1, b, c, d, a, in[11] + 0x895cd7beU, 22);
    GPN_MD5C_STEP(GPN_MD5C_F1, a, b, c, d, in[12] + 0x6b901122U,  7);
    GPN_MD5C_STEP(GPN_MD5C_F1, d, a, b, c, in[13] + 0xfd987193U, 12);
    GPN_MD5C_STEP(GPN_MD5C_F1, c, d, a, b, in[14] + 0xa679438eU, 17);
    GPN_MD5C_STEP(GPN_MD5C_F1, b, c, d, a, in[15] + 0x49b40821U, 22);
    GPN_MD5C_STEP(GPN_MD5C_F2, a, b, c, d, in[ 1] + 0xf61e2562U,  5);
    GPN_MD5C_STEP(GPN_MD5C_F2, d, a, b, c, in[ 6] + 0xc040b340U,  9);
    GPN_MD5C_STEP(GPN_MD5C_F2, c, d, a, b, in[11] + 0x265e5a51U, 14);
    GPN_MD5C_STEP(GPN_MD5C_F2, b, c, d, a, in[ 0] + 0xe9b6c7aaU, 20);
    GPN_MD5C_STEP(GPN_MD5C_F2, a, b, c, d, in[ 5] + 0xd62f105dU,  5);
    GPN_MD5C_STEP(GPN_MD5C_F2, d, a, b, c, in[10] + 0x02441453U,  9);
    GPN_MD5C_STEP(GPN_MD5C_F2, c, d, a, b, in[15] + 0xd8a1e681U, 14);
    GPN_MD5C_STEP(GPN_MD5C_F2, b, c, d, a, in[ 4] + 0xe7d3fbc8U, 20);
    GPN_MD5C_STEP(GPN_MD5C_F2, a, b, c, d, in[ 9] + 0x21e1cde6U,  5);
    GPN_MD5C_STEP(GPN_MD5C_F2, d, a, b, c, in[14] + 0xc33707d6U,  9);
    GPN_MD5C_STEP(GPN_MD5C_F2, c, d, a, b, in[ 3] + 0xf4d50d87U, 14);
    GPN_MD5C_STEP(GPN_MD5C_F2, b, c, d, a, in[ 8] + 0x455a14edU, 20);
    GPN_MD5C_STEP(GPN_MD5C_F2, a, b, c, d, in[13] + 0xa9e3e905U,  5);
    GPN_MD5C_STEP(GPN_MD5C_F2, d, a, b, c, in[ 2] + 0xfcefa3f8U,  9);
    GPN_MD5C_STEP(GPN_MD5C_F2, c, d, a, b, in[ 7] + 0x676f02d9U, 14);
    GPN_MD5C_STEP(GPN_MD5C_F2, b, c, d, a, in[12] + 0x8d2a4c8aU, 20);
    GPN_MD5C_STEP(GPN_MD5C_F3, a, b, c, d, in[ 5] + 0xfffa3942U,  4);
    GPN_MD5C_STEP(GPN_MD5C_F3, d, a, b, c, in[ 8] + 0x8771f681U, 11);
    GPN_MD5C_STEP(GPN_MD5C_F3, c, d, a, b, in[11] + 0x6d9d6122U, 16);
    GPN_MD5C_STEP(GPN_MD5C_F3, b, c, d, a, in[14] + 0xfde5380cU, 23);
    GPN_MD5C_STEP(GPN_MD5C_F3, a, b, c, d, in[ 1] + 0xa4beea44U,  4);
    GPN_MD5C_STEP(GPN_MD5C_F3, d, a, b, c, in[ 4] + 0x4bdecfa9U, 11);
    GPN_MD5C_STEP(GPN_MD5C_F3, c, d, a, b, in[ 7] + 0xf6bb4b60U, 16);
    GPN_MD5C_STEP(GPN_MD5C_F3, b, c, d, a, in[10] + 0xbebfbc70U, 23);
    GPN_MD5C_STEP(GPN_MD5C_F3, a, b, c, d, in[13] + 0x289b7ec6U,  4);
    GPN_MD5C_STEP(GPN_MD5C_F3, d, a, b, c, in[ 0] + 0xeaa127faU, 11);
    GPN_MD5C_STEP(GPN_MD5C_F3, c, d, a, b, in[ 3] + 0xd4ef3085U, 16);
    GPN_MD5C_STEP(GPN_MD5C_F3, b, c, d, a, in[ 6] + 0x04881d05U, 23);
    GPN_MD5C_STEP(GPN_MD5C_F3, a, b, c, d, in[ 9] + 0xd9d4d039U,  4);
    GPN_MD5C_STEP(GPN_MD5C_F3, d, a, b, c, in[12] + 0xe6db99e5U, 11);
    GPN_MD5C_STEP(GPN_MD5C_F3, c, d, a, b, in[15] + 0x1fa27cf8U, 16);
    GPN_MD5C_STEP(GPN_MD5C_F3, b, c, d, a, in[ 2] + 0xc4ac5665U, 23);
    GPN_MD5C_STEP(GPN_MD5C_F4, a, b, c, d, in[ 0] + 0xf4292244U,  6);
    GPN_MD5C_STEP(GPN_MD5C_F4, d, a, b, c, in[ 7] + 0x432aff97U, 10);
    GPN_MD5C_STEP(GPN_MD5C_F4, c, d, a, b, in[14] + 0xab9423a7U, 15);
    GPN_MD5C_STEP(GPN_MD5C_F4, b, c, d, a, in[ 5] + 0xfc93a039U, 21);
    GPN_MD5C_STEP(GPN_MD5C_F4, a, b, c, d, in[12] + 0x655b59c3U,  6);
    GPN_MD5C_STEP(GPN_MD5C_F4, d, a, b, c, in[ 3] + 0x8f0ccc92U, 10);
    GPN_MD5C_STEP(GPN_MD5C_F4, c, d, a, b, in[10] + 0xffeff47dU, 15);
    GPN_MD5C_STEP(GPN_MD5C_F4, b, c, d, a, in[ 1] + 0x85845dd1U, 21);
    GPN_MD5C_STEP(GPN_MD5C_F4, a, b, c, d, in[ 8] + 0x6fa87e4fU,  6);
    GPN_MD5C_STEP(GPN_MD5C_F4, d, a, b, c, in[15] + 0xfe2ce6e0U, 10);
    GPN_MD5C_STEP(GPN_MD5C_F4, c, d, a, b, in[ 6] + 0xa3014314U, 15);
    GPN_MD5C_STEP(GPN_MD5C_F4, b, c, d, a, in[13] + 0x4e0811a1U, 21);
    GPN_MD5C_STEP(GPN_MD5C_F4, a, b, c, d, in[ 4] + 0xf7537e82U,  6);
    GPN_MD5C_STEP(GPN_MD5C_F4, d, a, b, c, in[11] + 0xbd3af235U, 10);
    GPN_MD5C_STEP(GPN_MD5C_F4, c, d, a, b, in[ 2] + 0x2ad7d2bbU, 15);
    GPN_MD5C_STEP(GPN_MD5C_F4, b, c, d, a, in[ 9] + 0xeb86d391U, 21);
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}
/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void gpnMD5CalculateMD5Final(unsigned char digest[16], GPN_MD5C_CTX *ctx)
{
    unsigned count;
    unsigned char *p;
    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;
    /* Set the first char of padding to 0x80.  This is safe since there is
            always at least one byte free */
    p = ctx->in + count;
    *p++ = 0x80;
    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8)
    {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(p, 0, count);
        gpnMD5ByteReverse(ctx->in, 16);
        gpnMD5CalculateTransform(ctx->buf, (unsigned int *) ctx->in);
        /* Now fill the next block with 56 bytes */
        memset(ctx->in, 0, 56);
    }

    else
    {
        /* Pad block to 56 bytes */
        memset(p, 0, count - 8);
    }

    gpnMD5ByteReverse(ctx->in, 14);
    /* Append length in bits and transform */
    ((unsigned int *) ctx->in)[14] = ctx->bits[0];
    ((unsigned int *) ctx->in)[15] = ctx->bits[1];
    gpnMD5CalculateTransform(ctx->buf, (unsigned int *) ctx->in);
    gpnMD5ByteReverse((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(ctx));    /* In case it's sensitive */
}
/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void gpnMD5CalculateMD5Init(GPN_MD5C_CTX *ctx)
{
    ctx->buf[0] = 0x67452301U;
    ctx->buf[1] = 0xefcdab89U;
    ctx->buf[2] = 0x98badcfeU;
    ctx->buf[3] = 0x10325476U;
    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
    memset(ctx->in, 0x0, sizeof(ctx->in));
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void gpnMD5CalculateMD5Update(GPN_MD5C_CTX *ctx, unsigned const char *buf, unsigned len)
{
    unsigned int t;
    /* Update bitcount */
    t = ctx->bits[0];

    if ((ctx->bits[0] = t + ((unsigned int) len << 3)) < t)
        ctx->bits[1]++;     /* Carry from low to high */

    ctx->bits[1] += len >> 29;
    t = (t >> 3) & 0x3f;    /* Bytes already in shsInfo->data */

    /* Handle any leading odd-sized chunks */
    if (t)
    {
        unsigned char *p = (unsigned char *) ctx->in + t;
        t = 64 - t;

        if (len < t)
        {
            memcpy(p, buf, len);
            return;
        }

        memcpy(p, buf, t);
        gpnMD5ByteReverse(ctx->in, 16);
        gpnMD5CalculateTransform(ctx->buf, (unsigned int *) ctx->in);
        buf += t;
        len -= t;
    }

    /* Process data in 64-byte chunks */
    while (len >= 64)
    {
        memcpy(ctx->in, buf, 64);
        gpnMD5ByteReverse(ctx->in, 16);
        gpnMD5CalculateTransform(ctx->buf, (unsigned int *) ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(ctx->in, buf, len);
}

void gpnMD5CalculateMD5(unsigned const char *buf, unsigned len, unsigned char digest[16])
{
    GPN_MD5C_CTX ctx;
    gpnMD5CalculateMD5Init(&ctx);
    gpnMD5CalculateMD5Update(&ctx, buf, len);
    gpnMD5CalculateMD5Final(digest, &ctx);
}

int gpnMD5CalculateBaseFd(int fd, int file_size, char *md5)
{
    //int             i;
    int             count = 0;
    int             leavecount = 0;
    GPN_MD5C_CTX    ctx;
    unsigned char   buf[1024*16];
    unsigned char   md[16] = {0};
    int             seg_size= 1024 * 16;
	char            l_md5[GPN_MD5C_LENGTH+1];

    if (NULL == md5)
        return GPN_MD5C_ERR;

    /*init*/
    gpnMD5CalculateMD5Init(&ctx);
    lseek(fd, 0, SEEK_SET);

    while (1)
    {
        memset(buf, 0, seg_size);
        leavecount  = file_size - count * seg_size;

        if (leavecount > seg_size)
        {
            read(fd, buf, seg_size);
            gpnMD5CalculateMD5Update(&ctx, buf, seg_size);
            count++;
        }

        else if (leavecount <= seg_size)
        {
            read(fd, buf, leavecount);
            gpnMD5CalculateMD5Update(&ctx, buf, leavecount);
            break;
        }
    }

    gpnMD5CalculateMD5Final(&(md[0]), &ctx);
#if 0
    for (i = 0; i < GPN_MD5C_DIGEST_LENGTH; i++)
    {
        sprintf(l_md5 + i*2,   "%x", 0x0F & (md[i] >> 4));
        sprintf(l_md5 + i*2 + 1, "%x", 0x0F & md[i]);
    }
#endif

	sprintf(l_md5, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
		(0x0F & (md[0] >> 4)), (0x0F & md[0]), (0x0F & (md[1] >> 4)), (0x0F & md[1]),
		(0x0F & (md[2] >> 4)), (0x0F & md[2]), (0x0F & (md[3] >> 4)), (0x0F & md[3]),
		(0x0F & (md[4] >> 4)), (0x0F & md[4]), (0x0F & (md[5] >> 4)), (0x0F & md[5]),
		(0x0F & (md[6] >> 4)), (0x0F & md[6]), (0x0F & (md[7] >> 4)), (0x0F & md[7]));
	
	sprintf(l_md5 + 16, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
		(0x0F & (md[8] >> 4)), (0x0F & md[8]), (0x0F & (md[9] >> 4)), (0x0F & md[9]),
		(0x0F & (md[10] >> 4)), (0x0F & md[10]), (0x0F & (md[11] >> 4)), (0x0F & md[11]),
		(0x0F & (md[12] >> 4)), (0x0F & md[12]), (0x0F & (md[13] >> 4)), (0x0F & md[13]),
		(0x0F & (md[14] >> 4)), (0x0F & md[14]), (0x0F & (md[15] >> 4)), (0x0F & md[15]));

	memcpy(md5, l_md5, GPN_MD5C_LENGTH);
	
    return GPN_MD5C_OK;
}

int gpnMD5CalculateBaseFile(const char *full_file, char *md5)
{
    int             rc;
    int             fd;
    struct stat     info;

    if (NULL == full_file || NULL == md5)
        return GPN_MD5C_ERR;

    rc  = stat(full_file, &info);

    if (rc == -1)
    {
        return GPN_MD5C_ERR;
    }

    fd  = open(full_file, O_RDONLY);

    if (fd == -1)
    {
        return GPN_MD5C_ERR;
    }

    rc  = gpnMD5CalculateBaseFd(fd, info.st_size, md5);
    close(fd);

    return GPN_MD5C_OK;
}

int gpnMD5CalculateBaseMemSpace(char *mem_p, int mem_size, char *md5)
{
	//int             i;
    int             count = 0;
    int             leavecount = 0;
    GPN_MD5C_CTX    ctx;
    unsigned char   buf[1024*16] = {0};
    unsigned char   md[16] = {0};
    int             seg_size = 1024 * 16;
	char			l_md5[GPN_MD5C_LENGTH+1];

    if ((NULL == md5) ||\
		(NULL == mem_p))
        return GPN_MD5C_ERR;

    /*init*/
    gpnMD5CalculateMD5Init(&ctx);

    while (1)
    {
        memset(buf, 0, seg_size);
        leavecount  = mem_size - count * seg_size;

        if (leavecount > seg_size)
        {
			memcpy(buf, mem_p, seg_size);
			
            gpnMD5CalculateMD5Update(&ctx, buf, seg_size);
			
            count++;
			mem_p += seg_size;
        }

        else if (leavecount <= seg_size)
        {
            memcpy(buf, mem_p, leavecount);
			
            gpnMD5CalculateMD5Update(&ctx, buf, leavecount);
			
            break;
        }
    }

    gpnMD5CalculateMD5Final(&(md[0]), &ctx);

	sprintf(l_md5, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
		(0x0F & (md[0] >> 4)), (0x0F & md[0]), (0x0F & (md[1] >> 4)), (0x0F & md[1]),
		(0x0F & (md[2] >> 4)), (0x0F & md[2]), (0x0F & (md[3] >> 4)), (0x0F & md[3]),
		(0x0F & (md[4] >> 4)), (0x0F & md[4]), (0x0F & (md[5] >> 4)), (0x0F & md[5]),
		(0x0F & (md[6] >> 4)), (0x0F & md[6]), (0x0F & (md[7] >> 4)), (0x0F & md[7]));
	
	sprintf(l_md5 + 16, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
		(0x0F & (md[8] >> 4)), (0x0F & md[8]), (0x0F & (md[9] >> 4)), (0x0F & md[9]),
		(0x0F & (md[10] >> 4)), (0x0F & md[10]), (0x0F & (md[11] >> 4)), (0x0F & md[11]),
		(0x0F & (md[12] >> 4)), (0x0F & md[12]), (0x0F & (md[13] >> 4)), (0x0F & md[13]),
		(0x0F & (md[14] >> 4)), (0x0F & md[14]), (0x0F & (md[15] >> 4)), (0x0F & md[15]));

	memcpy(md5, l_md5, GPN_MD5C_LENGTH);

    return GPN_MD5C_OK;
}

int gpnMD5CalculateTest()
{
	int fd;
	char mem[1024];
	int mem_size;
	char md5[GPN_MD5C_LENGTH];
	
	/* creat a file */
	fd  = open("/tmp/gpn_md5_test", O_CREAT);
	write(fd, "test message!\n", 14);
	close(fd);

	/* test gpn md5 calculate base file path */
	memset(md5, 0, GPN_MD5C_LENGTH);
	gpnMD5CalculateBaseFile("/tmp/gpn_md5_test", md5);
	printf("file base : %s\n", md5);

	/* test gpn md5 calculate base mem */
	memset(md5, 0, GPN_MD5C_LENGTH);
	fd  = open("/tmp/gpn_md5_test", O_RDONLY);
	mem_size = read(fd, mem, 1024);
	gpnMD5CalculateBaseMemSpace(mem, mem_size, md5);
	printf(" mem base : %s\n", md5);

	return GPN_MD5C_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_MD5_CALCULATE_C_ */



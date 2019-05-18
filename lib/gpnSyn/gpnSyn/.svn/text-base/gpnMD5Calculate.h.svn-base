#ifndef _GPN_MD5_CALCULATE_H_
#define _GPN_MD5_CALCULATE_H_

#define GPN_MD5C_OK        1
#define GPN_MD5C_ERR       2

struct gpnMD5CalculateContext
{
    unsigned int    buf[4];
    unsigned int    bits[2];
    unsigned char   in[64];
};

typedef struct gpnMD5CalculateContext GPN_MD5C_CTX;

/**/
#define GPN_MD5C_DIGEST_LENGTH   16  /*bytes hex*/
#define GPN_MD5C_LENGTH          32

#if defined(__i386) || defined(__i386__) || defined(__x86_64) || defined(__x86_64__)
/*big-endian*/
#define GPN_MD5_BYTE_REVERSE(buf, len)
#else
/*little-endian*/
#define GPN_MD5_BYTE_REVERSE(buf, len) 
#endif


/* get md5(32 bytes ascii) from fd or file
 * return  1 : ok
 *         2 : failure
 */
int gpnMD5CalculateBaseFd(int fd, int file_size, char *md5);
int gpnMD5CalculateBaseFile(const char *full_file, char *md5);
int gpnMD5CalculateBaseMemSpace(char *mem_p, int mem_size, char *md5);

int gpnMD5CalculateTest();

#endif  /* _GPN_MD5_CALCULATE_H_*/


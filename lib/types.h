/*  *********************************************************************

    ********************************************************************* */

#ifndef HIOS_TYPES_H
#define HIOS_TYPES_H

#include <endian.h>

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define BIG_ENDIAN_ON     0    /* 0 表示小端序 */
#else
#define BIG_ENDIAN_ON     1    /* 1 表示大端序 */
#endif

#define NAME_STRING_LEN  32   /* 定义命令行各种名字的长度，统一为 32 字节 */
#define STRING_LEN       256   /* string 长度 */
#define MAC_LEN          6     /* mac 地址的长度 */
#define IFM_ALIAS_STRING_LEN   256

typedef unsigned int __be32;
typedef unsigned short __be16;

typedef unsigned int u_int;

typedef unsigned int    u_int32_t;
typedef unsigned short  u_int16_t;
typedef unsigned char   u_int8_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long long u_int64_t;

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned char uchar;

typedef unsigned short u_int16;
typedef unsigned int  u_int32;
typedef unsigned char u_char;

typedef int   s_int32_t; /* 32 bit signed integer */
typedef short s_int16_t; /* 16 bit signed integer */
typedef char  s_int8_t;  /* 8 bit signed integer */

typedef char		sint8;
typedef short int	sint16;
typedef int			sint32;


/* 保护回切/不回切定义 */
enum FAILBACK_E
{
	FAILBACK_ENABLE = 0,  /* 0: 回切 */
	FAILBACK_DISABLE/* 1: 不回切 */
};


/* 保护状态定义 */
enum BACKUP_E
{
	BACKUP_STATUS_INVALID = 0,
	BACKUP_STATUS_MASTER,   /* 主工作 */
	BACKUP_STATUS_SLAVE,    /* 备工作 */	
};


/* 工作状态 */
enum ACTIVE_E
{
	ACTIVE_STATUS_INVALID = 0,
	ACTIVE_STATUS_ACTIVE,       /* 工作 */
	ACTIVE_STATUS_INACTIVE,     /* 不工作 */
};


/* up/down 状态定义 */
enum LINK_E
{
	LINK_UP = 0,
	LINK_DOWN
};


/* 操作码定义 */
enum OPCODE_E
{
    OPCODE_ADD = 0,  /* add  */
    OPCODE_DELETE,   /* delete  */
    OPCODE_UPDATE,   /* update  */
	OPCODE_UP,	     /* up  */
	OPCODE_DOWN,	 /* down */
};


/* in/out 方向定义 */
enum DIRECT_E
{
	DIRECT_IN = 0,
	DIRECT_OUT
};


/* 统计数据结构 */
struct counter_t
{
    uint64_t rx_packets;   /* total packets received       */
    uint64_t tx_packets;   /* total packets transmitted    */
    uint64_t rx_bytes;     /* total bytes received         */
    uint64_t tx_bytes;     /* total bytes transmitted      */
    uint64_t rx_dropped;   /* drop packets received        */
    uint64_t tx_dropped;   /* drop packets transmitted     */
};


/* 使能状态定义 */
#define ENABLE  1
#define DISABLE 0


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef	NULL
#define NULL 0
#endif



/* Maximum of two values */
#ifndef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#endif 
	
/* Minimum of two comparable values */
#ifndef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif 


/* Some systems do not define UINT32_MAX, etc.. from inttypes.h
 * e.g. this makes life easier for FBSD 4.11 users.
 */
#ifndef INT8_MAX
#define INT8_MAX	(127)
#endif
#ifndef INT16_MAX
#define INT16_MAX	(32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX	(2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX	(255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX	(65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX	(4294967295U)
#endif


/* Flag manipulation macros. */
#define CHECK_FLAG(V,F)      ((V) & (F))
#define SET_FLAG(V,F)        (V) = (V) | (F)
#define UNSET_FLAG(V,F)      (V) = (V) & ~(F)
#define FLAG_ISSET(V,F)      (((V) & (F)) == (F))



#endif




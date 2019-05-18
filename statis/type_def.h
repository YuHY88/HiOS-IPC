/**********************************************************
* file name: type_def.h
* Copyright: 
	 Copyright 2012 huahuan.
* author: 
*    huahuan liuyf 2014-03-12
* function: 
*    system unity define
* modify:
*
***********************************************************/
#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef UINT8
#define UINT8 	unsigned char
#endif

#ifndef UINT16
#define UINT16	unsigned short
#endif

#ifndef UINT32
#define UINT32	unsigned int
#endif

#ifndef UINT64
#define UINT64  unsigned long long
#endif

#ifndef INT8
#define INT8 	char
#endif

#ifndef INT16
#define INT16	short
#endif

#ifndef INT32
#define INT32	int
#endif

#ifndef INT64
#define INT64   long long
#endif

#ifndef FLOAT
#define FLOAT   float
#endif

#ifndef PRAGMA_ATTR_PACKED
#define PRAGMA_ATTR_PACKED __attribute__ ((packed))
#endif

#define GEN_SYS_OK 					1
#define GEN_SYS_ERR					2

#define GEN_SYS_YES 				1
#define GEN_SYS_NO					2

#define GEN_SYS_CANCEL 				0
#define GEN_SYS_ENABLE 				1
#define GEN_SYS_DISABLE				2
#define GEN_SYS_DELETE				3


#define GEN_SYS_ACTIVE 				1
#define GEN_SYS_DEACTIVE 			2

#define GEN_SYS_NULL				0

#define GEN_SYS_08_NULL			0x00
#define GEN_SYS_16_NULL			0x0000
#define GEN_SYS_32_NULL			0x00000000
/*#define GPN_SYS_64_NULL		0x0000000000000000*/

#define GEN_SYS_08_VALID		0xed
#define GEN_SYS_16_VALID		0xeeed
#define GEN_SYS_32_VALIDL		0xeeeeeeed
/*#define GPN_SYS_64_VALID		0xeeeeeeeeeeeeeeed*/

#define GEN_SYS_08_FFFF			0xFF
#define GEN_SYS_16_FFFF			0xFFFF
#define GEN_SYS_32_FFFF			0xFFFFFFFF
/*#define GPN_SYS_64_FFFF		0xFFFFFFFFFFFFFFFF*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _TYPE_DEF_H_ */


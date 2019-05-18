/**********************************************************
* file name: gpnGlobalDevDriver.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-09
* function: 
*    define GPN_DEV_DRIVER method
* modify:
*
***********************************************************/
#ifndef _GPN_GLOBAL_DEV_DRIVER_H_
#define _GPN_GLOBAL_DEV_DRIVER_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"

#define GPN_DEV_DRV_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_DEV_DRV_GEN_ERR 				GPN_SOCK_SYS_ERR

#define GPN_DEV_DRV_GEN_YES					GPN_SOCK_SYS_YES
#define GPN_DEV_DRV_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_DEV_DRV_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_DEV_DRV_AGP 					GEN_SOCK_SYS_AGP
#define GPN_DEV_DRV_SVP 					GEN_SOCK_SYS_SVP
#define GPN_DEV_DRV_CMP 					GEN_SOCK_SYS_CMP
#define GPN_DEV_DRV_CUP 					GEN_SOCK_SYS_CUP

UINT32 gpnDevDrvGetEth0Mac(char *macAddr);
UINT32 gpnDevDrvGetEthHHMac(char *macAddr);

UINT32 gpnDevDrvIndependentTypeDriver(void);
UINT32 gpnDevDrvDefaultGetFixPort(void);

UINT32 gpnDevDrvH20PN1660PDriver(void);
UINT32 gpnDevDrvH20PN1660PGetFixPort(void);

UINT32 gpnDevDrvH20PN1660CDriver(void);
UINT32 gpnDevDrvH20PN1660CGetFixPort(void);

UINT32 gpnDevDrvH20PN1610Driver(void);
UINT32 gpnDevDrvH20PN1610GetFixPort(void);
UINT32 gpnDevDrvH20PN1610GetMgtInfo(void);

UINT32 gpnDevDrvH20CE1604CDriver(void);
UINT32 gpnDevDrvH20CE1604CGetFixPort(void);
UINT32 gpnDevDrvH20CE1604CGetMgtInfo(void);

typedef struct _s_2000_fpga_
{
    UINT8   cs;         /* fpga is 0*/
    UINT16  addr;       /* register address to read or write*/
    UINT16  size;       /* length to read or write*/
    UINT8   buff[64];   /* buffer to store the data*/
}s_2000_fpga;

UINT32 gpnDevDrvH20PN2000Driver(void);
UINT32 gpnDevDrvH20PN2000GetSelfSlotDriver(UINT32 *pslot);
UINT32 gpnDevDrvH20PN2000GetPMacDriver(UINT8 *pMac);
UINT32 gpnDevDrvH20PN2000GetFixPort(void);
UINT32 gpnDevDrvH20PN2000GetMgtInfo(void);

UINT32 gpnDevDrvH18EDD0402BDriver(void);
UINT32 gpnDevDrvH18EDD0402BGetFixPort(void);

UINT32 gpnDevDrvH18EDD2402BDriver(void);
UINT32 gpnDevDrvH18EDD2402BGetFixPort(void);

UINT32 gpnDevDrvH9MONM81Driver(void);
UINT32 gpnDevDrvH9MONM81GetFixPort(void);

UINT32 gpnDevDrvH9MOCPX10Driver(void);
UINT32 gpnDevDrvH9MOCPX10GetSelfSlotDriver(UINT32 *pslot);
UINT32 gpnDevDrvH9MOCPX10GetFixPort(void);
UINT32 gpnDevDrvH9MOCPX10GetMgtInfo(void);


/* add for ipran, by lipf, 2018/4/10 */
UINT32 gpnDevDrvHT201Driver(void);
UINT32 gpnDevDrvHT201GetFixPort(void);
UINT32 gpnDevDrvHT201GetMgtInfo(void);

UINT32 gpnDevDrvHT201EDriver(void);
UINT32 gpnDevDrvHT201EGetFixPort(void);
UINT32 gpnDevDrvHT201EGetMgtInfo(void);

UINT32 gpnDevDrvH20RN2000Driver(void);
UINT32 gpnDevDrvH20RN2000GetSelfSlotDriver(UINT32 *pslot);
UINT32 gpnDevDrvH20RN2000GetPMacDriver(UINT8 *pMac);
UINT32 gpnDevDrvH20RN2000GetFixPort(void);
UINT32 gpnDevDrvH20RN2000GetMgtInfo(void);

UINT32 gpnDevDrvHT157Driver(void);
UINT32 gpnDevDrvHT157GetFixPort(void);
UINT32 gpnDevDrvHT157GetMgtInfo(void);

UINT32 gpnDevDrvHT158Driver(void);
UINT32 gpnDevDrvHT158GetFixPort(void);
UINT32 gpnDevDrvHT158GetMgtInfo(void);

UINT32 gpnDevDrvVXDriver(void);
UINT32 gpnDevDrvVXGetSelfSlotDriver(UINT32 *pslot);
UINT32 gpnDevDrvVXGetPMacDriver(UINT8 *pMac);
UINT32 gpnDevDrvVXGetFixPort(void);
UINT32 gpnDevDrvVXGetMgtInfo(void);






#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_GLOBAL_DEV_DRIVER_H_ */



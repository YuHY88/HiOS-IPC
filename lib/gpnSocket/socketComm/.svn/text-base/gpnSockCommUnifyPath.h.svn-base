/**********************************************************
* file name: gpnInitUnifyPath.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-23
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_SOCKCOMM_UNIFY_PATH_H_
#define _GPN_SOCKCOMM_UNIFY_PATH_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"


#include "lib/gpnSocket/socketComm/gpnUnifyPathDef.h"
#include "lib/gpnLog/gpnLog/gpnLogFuncApi.h"


#define GPN_SC_UP_OK 						GPN_SOCK_SYS_OK
#define GPN_SC_UP_ERR 						GPN_SOCK_SYS_ERR

#define GPN_SC_UP_ENABLE					GPN_SOCK_SYS_ENABLE
#define GPN_SC_UP_DISABLE					GPN_SOCK_SYS_DISABLE

#define GPN_SC_UP_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_SC_UP_AGP 						GEN_SOCK_SYS_AGP
#define GPN_SC_UP_SVP 						GEN_SOCK_SYS_SVP
#define GPN_SC_UP_CMP 						GEN_SOCK_SYS_CMP
#define GPN_SC_UP_CUP 						GEN_SOCK_SYS_CUP

/*gpn_socket communication unify path define*/
/*ROM*/
#define GPN_SC_UP_ROM_NODE					GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_socket/)
#define GPN_SC_UP_ROM_DIR(fName)			GPN_SC_UP_ROM_NODE#fName

#define GPN_SC_UP_CFG_NODE					GPN_SC_UP_ROM_NODE
#define GPN_SC_UP_CFG_DIR(cfgFile)			GPN_SC_UP_CFG_NODE#cfgFile

/*RAM*/
#define GPN_SC_UP_RAM_NODE					GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_socket/)
#define GPN_SC_UP_RAM_DIR(fName)			GPN_SC_UP_RAM_NODE#fName

#define GPN_SC_UP_SOCK_FD_NODE				GPN_SC_UP_RAM_DIR(socket_fd/)
#define GPN_SC_UP_SOCK_FD_DIR(fdName)		GPN_SC_UP_SOCK_FD_NODE#fdName

UINT32 gpnSockCommUPPathInit(void);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SOCKCOMM_UNIFY_PATH_H_*/


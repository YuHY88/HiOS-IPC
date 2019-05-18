/**********************************************************
* file name: gpnUnifyPathDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-19
* function: 
*    define gpn unify file path method and opration
* modify:
*
***********************************************************/
#ifndef _GPN_UNIFY_PATH_DEF_H_
#define _GPN_UNIFY_PATH_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#define GPN_UNIFYP_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_UNIFYP_GEN_ERR 					GPN_SOCK_SYS_ERR

#define GPN_UNIFYP_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_UNIFYP_AGP 						GEN_SOCK_SYS_AGP
#define GPN_UNIFYP_SVP 						GEN_SOCK_SYS_SVP
#define GPN_UNIFYP_CMP 						GEN_SOCK_SYS_CMP
#define GPN_UNIFYP_CUP 						GEN_SOCK_SYS_CUP

#define GPN_UNIFYP_MAX_PATH_DEEP			10
#define GPN_UNIFYP_MAX_PATHN_LEN			256

#define GPN_UNIFYP_CMD_LEN					256
#define GPN_UNIFYP_DEMO_FILE_LEN			((GPN_UNIFYP_CMD_LEN) * 6)

/*
 *    XXX_NODE_TOKEN : do GPN_UP path test
 *    XXX_ROOT : we define this in makefile, used to fix some drictory
 *    XXX_NODE : base XXX_ROOT, define some drictory start point
 *    XXX_DIR    : user can use this to define a special file' dirctory
 *    XXX_PATH : define a special file' dirctory(can not use GPN_UP to re-define)
 */
#ifndef GPN_UNIFY_PATH_ROM_ROOT
#define GPN_UNIFY_PATH_ROM_NODE(subPath)	"/home/"#subPath
#define GPN_UNIFY_PATH_ROM_NODE_TOKEN		"rom_unify_path_default"
#else
#define GPN_UNIFY_PATH_ROM_NODE(subPath)	GPN_UNIFY_PATH_ROM_ROOT#subPath
#define GPN_UNIFY_PATH_ROM_NODE_TOKEN		"rom_unify_path_config"
#endif

#ifndef GPN_UNIFY_PATH_RAM_ROOT
#define GPN_UNIFY_PATH_RAM_NODE(subPath)	"/tmp/"#subPath
#define GPN_UNIFY_PATH_RAM_NODE_TOKEN		"ram_unify_path_default"
#else
#define GPN_UNIFY_PATH_RAM_NODE(subPath)	GPN_UNIFY_PATH_RAM_ROOT#subPath
#define GPN_UNIFY_PATH_RAM_NODE_TOKEN		"ram_unify_path_config"
#endif

#ifndef GPN_UNIFY_PATH_LOG_ROOT
#define GPN_UNIFY_PATH_LOG_NODE(subPath)	"/var/log/"#subPath
#else
#define GPN_UNIFY_PATH_LOG_NODE(subPath)	GPN_UNIFY_PATH_LOG_ROOT#subPath
#endif

#ifndef GPN_UNIFY_PATH_BIN_ROOT
#define GPN_UNIFY_PATH_BIN_PATH				"/home/"
#else
#define GPN_UNIFY_PATH_BIN_PATH				GPN_UNIFY_PATH_BIN_ROOT
#endif

#define GPN_UP_ROM_NODE						GPN_UNIFY_PATH_ROM_NODE()
#define GPN_UP_ROM_DIR(name)				GPN_UP_ROM_NODE#name

#define GPN_UP_RAM_NODE						GPN_UNIFY_PATH_RAM_NODE()
#define GPN_UP_RAM_DIR(name)				GPN_UP_RAM_NODE#name

UINT32 gpnUnifyPathTestDemo(void);
UINT32 gpnUnifyPathRomNodeTest(void);
UINT32 gpnUnifyPathRamNodeTest(void);
UINT32 gpnUnifyPathCreat(char *path);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_UNIFY_PATH_DEF_H_ */


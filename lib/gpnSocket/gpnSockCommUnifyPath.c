/**********************************************************
* file name: gpnSockCommUnifyPath.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-26
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_COMM_UNIFY_PATH_C_
#define _GPN_SOCK_COMM_UNIFY_PATH_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockCommUnifyPath.h"
    
UINT32 gpnSockCommUPPathInit(void)
{
	/*used for gpn_socket function*/
	gpnUnifyPathCreat((char *)GPN_SC_UP_ROM_NODE);
	gpnUnifyPathCreat((char *)GPN_SC_UP_RAM_NODE);

	/*used for save gpn_socket func about config file--ROM*/
	gpnUnifyPathCreat((char *)GPN_SC_UP_CFG_NODE);
	/*used for save gpn_socket comm fd--RAM*/
	gpnUnifyPathCreat((char *)GPN_SC_UP_SOCK_FD_NODE);
	
	return GPN_SC_UP_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SOCK_COMM_UNIFY_PATH_C_*/


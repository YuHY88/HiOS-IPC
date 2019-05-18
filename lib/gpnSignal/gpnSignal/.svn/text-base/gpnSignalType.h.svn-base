/**********************************************************
* file name: gpnSignalType.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-23
* function: 
*    define signal type used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_TYPE_H_
#define _GPN_SIGNAL_TYPE_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <signal.h>

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"



#define GPN_SIG_TYPE_GEN_OK                             GPN_SOCK_SYS_OK
#define GPN_SIG_TYPE_GEN_ERR                            GPN_SOCK_SYS_ERR

#define GPN_SIG_TYPE_GEN_YES                            GPN_SOCK_SYS_YES
#define GPN_SIG_TYPE_GEN_NO                             GPN_SOCK_SYS_NO

#define GPN_SIG_TYPE_PRINT(level, info...)              GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_SIG_TYPE_EGP                                GEN_SYS_DEBUG_EGP
#define GPN_SIG_TYPE_SVP                                GEN_SYS_DEBUG_SVP
#define GPN_SIG_TYPE_CMP                                GEN_SYS_DEBUG_CMP
#define GPN_SIG_TYPE_CUP                                GEN_SYS_DEBUG_CUP
#define GPN_SIG_TYPE_UD5                                GEN_SYS_DEBUG_UD5
#define GPN_SIG_TYPE_UD4                                GEN_SYS_DEBUG_UD4
#define GPN_SIG_TYPE_UD3                                GEN_SYS_DEBUG_UD3
#define GPN_SIG_TYPE_UD2                                GEN_SYS_DEBUG_UD2
#define GPN_SIG_TYPE_UD1                                GEN_SYS_DEBUG_UD1
#define GPN_SIG_TYPE_UD0                                GEN_SYS_DEBUG_UD0

/*GPN_SIG_TYPE globla define */

/*see /usr/include/bits/signum.h                                                                                                       */
/*#define	xxxx           	                                                                        x                                      */
/*..................                                                                                          .......                                 */
/*#define SIGUNUSED                                                                               31                                    */
/*#define	_NSIG		                                                                        65                                    */
/*#define SIGRTMIN                                                                                 (__libc_current_sigrtmin ()) */
/*#define SIGRTMAX                                                                                (__libc_current_sigrtmax ()) */
/* These are the hard limits of the kernel.  These values should not be                                                    */
/* used directly at user level.                                                                                                               */
/*#define __SIGRTMIN	                                                                       32                                      */
/*#define __SIGRTMAX                                                                            (_NSIG - 1)                         */

/*GPN_USR_DEF_SIG*/
#define GPN_DEF_SIG_64                                 __SIGRTMAX
#define GPN_DEF_SIG_63                                 GPN_DEF_SIG_64-1
#define GPN_DEF_SIG_62                                 GPN_DEF_SIG_63-1
#define GPN_DEF_SIG_61                                 GPN_DEF_SIG_62-1
#define GPN_DEF_SIG_60                                 GPN_DEF_SIG_61-1
/*.......*/

/*GPN_SYS_DEF_SIG*/
/*#define GPN_SIG_TYPE_POW_DOWN                                                    SIGPWR*/
#define GPN_SIG_TYPE_POW_DOWN                          SIGUSR1
#define GPN_SIG_TYPE_REBOOT                            SIGTERM

#define GPN_SIG_TYPE_SDK_PAIN                          GPN_DEF_SIG_64
#define GPN_SIG_TYPE_XXX_XXX1                          GPN_DEF_SIG_63
#define GPN_SIG_TYPE_XXX_XXX2                          GPN_DEF_SIG_62
#define GPN_SIG_TYPE_XXX_XXX3                          GPN_DEF_SIG_61
#define GPN_SIG_TYPE_XXX_XXX4                          GPN_DEF_SIG_60

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_TYPE_H_ */



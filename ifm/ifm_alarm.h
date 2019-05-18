#ifndef _IFM_ALARM_H_
#define _IFM_ALARM_H_

#include "lib/gpnSocket/socketComm/gpnSockCommRoleMan.h"
#include "lib/gpnSocket/socketComm/gpnSockCommFuncApi.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"

//int ifm_alarm_socket_msg_rcv( struct thread * t );
int ifm_alarm_socket_msg_rcv( struct ipc_mesg_n * pmsg, int imlen);
uint32_t ifm_gpn_function_init( void );


#endif



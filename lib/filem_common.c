/* add by suxq 2017/09/19*/
#if 0
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <linux/sysctl.h>
#include <lib/module_id.h>

static int ipc_filem_id = -1;

int ipc_send_filem(void *pdata, int data_len, int data_num, int module, int sender,
                   enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode)
{
	struct ipc_msghdr msghdr;

	if(ipc_filem_id < 0)
    {
        ipc_filem_id = ipc_connect(IPC_MSG_FILEM);
        
        if(ipc_filem_id < 0) return(-1);
    }

	msghdr.data_len    = data_len;
	msghdr.module_id   = module;
	msghdr.msg_type    = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index   = 0;
	msghdr.data_num    = data_num;
	msghdr.opcode      = opcode;
	msghdr.sender_id   = sender;

	return(ipc_send(ipc_filem_id, &msghdr, pdata));
}

int ipc_send_filemext(struct ipc_pkt *pmsg, int dlen) 
{
    int ret = 0;
    
	if(ipc_filem_id < 0)
    {
        ipc_filem_id = ipc_connect(IPC_MSG_FILEM);
        
        if(ipc_filem_id < 0) return(-1);
    }

    
    ret = msgsnd(ipc_filem_id, (void *)pmsg, dlen, IPC_NOWAIT);

    return ret;
}

#endif


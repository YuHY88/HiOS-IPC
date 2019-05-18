/*Ospf's external public api*/
#include <string.h>
#include <stdlib.h>
#include <lib/msg_ipc_n.h>
#include "lib/ifm_common.h"
#include "lib/ospf_common.h"

/**
 * @brief      : Get ospf-dcn status
 * @param[in ] : module_id   - The acquirer module ID.
 * @param[out] :
 * @return     : dcn enable == OSPF_DCN_ENABLE; disable == OSPF_DCN_DISABLE
 * @author     : Zhangzl
 * @date       : 2019.1.22
 * @note       :
 */
int get_ospf_dcn_status(int module_id)
{
	int ret = 0;

	struct ipc_mesg_n* pmsg = ipc_sync_send_n2(NULL, 0,  1, MODULE_ID_OSPF, module_id,
				IPC_TYPE_OSPF_DCN, IPC_SUB_TYPE_DCN_STATUS, IPC_OPCODE_GET, 0, 1000);
	if(!pmsg)
	{
		return OSPF_DCN_DISABLE;
	}

	if(pmsg->msghdr.data_num == 0)
	{
		mem_share_free(pmsg, module_id);
		return OSPF_DCN_DISABLE;
	}

	ret = *(u_char *)pmsg->msg_data;

    mem_share_free(pmsg, module_id);
	
	return ret;
}




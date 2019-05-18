#include <zebra.h>
#include "lib/vty.h"
#include "vtysh.h"
#include "vtysh_snmp.h"
#include <string.h>
#include <malloc.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/inet_ip.h>
#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/msg_ipc.h>
#include <errcode.h>
#include <lib/snmp_common.h>


extern struct list g_u0_list;


//获取vty comm 配置标量信息并返回给snmp
static int vty_comm_cfg_reply(struct ipc_msghdr_n *phdr)
{
	VTY_MIB_COMM_CFG info;
	memset(&info, 0, sizeof(VTY_MIB_COMM_CFG));

	memcpy(info.hostname, host.device_name, strlen(host.device_name));
	
	if(ipc_send_reply_n2(&info, sizeof(VTY_MIB_COMM_CFG), 1, phdr->sender_id,
		phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->sequence, phdr->msg_index, phdr->opcode) < 0)
	{
		zlog_err("%s[%d] : vty send common config info to snmp failed\n", __FUNCTION__, __LINE__);
		return ERRNO_IPC;
	}
	return ERRNO_SUCCESS;
}


//获取 U0 配置表信息并返回给snmp
static int vty_u0_table_info_reply(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = (struct ipc_msghdr_n *)&pmsg->msghdr;

	VTY_U0_INFO_INDEX *pindex = (VTY_U0_INFO_INDEX *)pmsg->msg_data;

	if(pindex)
	{		
		int data_cnt_max = IPC_MSG_LEN/sizeof(VTY_U0_INFO);

		int data_cnt = 0;

		VTY_U0_INFO u0_info[data_cnt_max];

		memset(u0_info, 0, data_cnt_max*sizeof(VTY_U0_INFO));
		
		struct listnode *node = NULL;
		
		U0_INFO *pinfo = NULL;

		int f_data_found = 0;

		if(0 == pindex->ipv4)
		{
			f_data_found = 1;
		}
		
		for (node = listhead(&g_u0_list); node; node = listnextnode(node))
		{
			pinfo = (U0_INFO *)node->data;

			if(0 == f_data_found)
			{
				if(pindex->ipv4 == pinfo->ipv4)
				{
					f_data_found = 1;
				}
				continue;
			}
			else
			{
				u0_info[data_cnt].index.ipv4 = pinfo->ipv4;
				memcpy(u0_info[data_cnt].data.hostname, pinfo->hostname, strlen(pinfo->hostname));

				data_cnt++;

				if(data_cnt >= data_cnt_max)
				{
					break;
				}
			}
		}

		if(data_cnt)
		{
			if(ipc_send_reply_n2(u0_info, data_cnt*sizeof(VTY_U0_INFO), data_cnt, 
				phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->sequence, phdr->msg_index, phdr->opcode) < 0)
			{
				zlog_err("%s[%d] : vty send U0 table info to snmpd failed\n", __FUNCTION__, __LINE__);
				return ERRNO_IPC;
			}
				
			return ERRNO_SUCCESS;
		}
	}

	if (ipc_send_reply_n2(NULL,0,0, phdr->sender_id, phdr->module_id, phdr->msg_type,
			phdr->msg_subtype, phdr->sequence, phdr->msg_index, phdr->opcode) < 0)
	{
		zlog_err("%s[%d] : vty send U0 table info to snmpd failed\n", __FUNCTION__, __LINE__);
		return ERRNO_IPC;
	}
						
	return ERRNO_SUCCESS;
}



//处理snmp ipc 消息
void vty_handle_snmp_msg(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = (struct ipc_msghdr_n *)&pmsg->msghdr;

	if(NULL == phdr)
	{
		return;
	}

	if(IPC_OPCODE_GET == phdr->opcode)		//scalar get
	{
		switch(phdr->msg_subtype)
		{
			case VTY_MIB_COMM_CFG_SCALAR:
				vty_comm_cfg_reply(phdr);
				break;
				
			default:
				break;
		}
	}
	else if(IPC_OPCODE_GET_BULK == phdr->opcode)	//table get
	{
		switch(phdr->msg_subtype)
		{
			case VTY_MIB_U0_INFO_TABLE:
				vty_u0_table_info_reply(pmsg);
				break;
				
			default:
				break;
		}
	}
}



#if 0
int vtysh_config_info_get(struct ipc_msghdr *pmsghdr)
{
	int send_int;

	if(pmsghdr->msg_subtype == VTYSH_USER_TIME_OUT)
	{
		/*minites*/
		send_int = vty_timeout_val / 60;
		zlog_debug(0, "VTYSH_USER_TIME_OUT send_int:%d\n", send_int);
	}
	else if(pmsghdr->msg_subtype == VTYSH_USER_MODE)
	{
//		send_int = host.aaa_authmode;
		zlog_debug(0, "VTYSH_USER_MODE send_int:%d\n", send_int);
	}

	if(ipc_send_reply(&send_int, sizeof(send_int), pmsghdr->sender_id,
					pmsghdr->module_id, pmsghdr->msg_type, pmsghdr->msg_subtype, pmsghdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : aaa send config info to vty failed\n", __FUNCTION__, __LINE__);
		return ERRNO_IPC;
	}

}

int vtysh_config_info_get_bulk(struct ipc_mesg *mesg)
{
	struct vtysh_snmp_user_info *user_info[10];
	struct vtysh_snmp_online_user_info *user_online_info[10];
	struct listnode *node, *nnode;
	struct host_users *users_loop;
	struct login_session *session_loop;
	uchar send_buf[1024];
	int i = 0;
	int replay = 0;
	char *name;
	int send_len;
	struct ipc_msghdr *pmsghdr = &(mesg->msghdr);
	int user_count = 0;
	int from_head = 0;
	
	memset(send_buf, 0, 1024);

	if(pmsghdr->msg_subtype == VTYSH_ONLINE_USER)
	{
		/*record usernum*/
		int usernum = 0;
		zlog_debug(0, "VTYSH_USER *((int *)(mesg->msg_data):%d\n", *((int *)(mesg->msg_data)));
		
		if(*((int *)(mesg->msg_data)) == 0)
		{
			replay = 1;
			/*search all local user*/
			for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
			{
				if(session_loop->user_name != NULL)
				{
					user_online_info[user_count] = (struct vtysh_snmp_online_user_info *)malloc \
							(sizeof(struct vtysh_snmp_online_user_info));
					memset(user_online_info[user_count], 0, (sizeof(struct vtysh_snmp_online_user_info)));
					user_online_info[user_count]->index.usernum = user_count + 1;
					memcpy(user_online_info[user_count]->data.userfrom, session_loop->address, \
						strlen(session_loop->address));
					zlog_debug(0, "user_online_info[%d]->data.userfrom:%s\n", user_count, user_online_info[user_count]->data.userfrom);
					memcpy(user_online_info[user_count]->data.username, session_loop->user_name, \
						strlen(session_loop->user_name));
					zlog_debug(0, "user_online_info[%d]->data.username:%s\n", user_count, user_online_info[user_count]->data.username);

					user_online_info[user_count]->data.userprivilege = session_loop->user_login->privilege;
					/*copy all structs into send buf*/
					memcpy(send_buf + (sizeof(struct vtysh_snmp_online_user_info) * user_count), \
					user_online_info[user_count], (sizeof(struct vtysh_snmp_online_user_info)));
					user_count++;
					zlog_debug(0, "user_count:%d\n", user_count);					
				}
				send_len = sizeof(struct vtysh_snmp_online_user_info) * user_count;
			}	
		}
		else
		{
			replay = 0;
		}	
	}
	else if(pmsghdr->msg_subtype == VTYSH_USER)
	{
		zlog_debug(0, "subtype:VTYSH_USER\n");
		name = mesg->msg_data;
		if(strlen(name) == 0)
		{
			from_head = 1;
		}
		
		for(ALL_LIST_ELEMENTS (host.registe_user, node, nnode, users_loop))
		{
			if(users_loop == NULL)
				continue;

			if(replay == 1 && nnode != NULL && listgetdata(nnode) != NULL)
			{
				replay ++;
			}
			/*find username and makesure it is not the last one, then we start
			 *to record user number*/
			 if(from_head == 0)
			 {
				if(replay == 0	\
					&& strcmp(name, users_loop->name)
					&& nnode != NULL 
					&& listgetdata(nnode) != NULL)
				{
					replay = 0;
					continue;
				}
			 }
			 else
			 {	
				replay = 1;				
			 	zlog_debug(0, "from_head:%d\t replay:%d\n", from_head, replay);
			 }
			/*we have find the user, fill structs*/
			if(replay > 1 || from_head)
			{
				user_info[user_count] = (struct vtysh_snmp_user_info *)malloc \
					(sizeof(struct vtysh_snmp_user_info));
				memset(user_info[user_count], 0, (sizeof(struct vtysh_snmp_user_info)));
				memcpy(user_info[user_count]->index.username, users_loop->name, strlen(users_loop->name));
				zlog_debug(0, "user_info[%d]->index.username:%s\n", user_count, user_info[user_count]->index.username);
				user_info[user_count]->data.privilege = users_loop->privilege;
				user_count++;
				zlog_debug(0, "user_count:%d\n", user_count);				
			}
		}

		/*copy all structs into send buf*/
		for (i = 0; i < user_count; i++)
		{
			memcpy(send_buf + (sizeof(struct vtysh_snmp_user_info) * i), \
				user_info[i], sizeof(struct vtysh_snmp_user_info));
		}
		send_len = sizeof(struct vtysh_snmp_user_info) * user_count;
		zlog_debug(0, "send_len:%d\n", send_len);
	}
	else
	{
		zlog_debug(0, "wrong msg_subtype\n");
		return -1;
	}

replay_snmp:
	/* replay snmp */
	if(replay)
	{
		zlog_debug(0, "ipc_send_reply_bulk\n");
		if(ipc_send_reply_bulk(send_buf, send_len,
			user_count, pmsghdr->sender_id, pmsghdr->module_id, pmsghdr->msg_type, pmsghdr->msg_subtype, pmsghdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : vtysh send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return ERRNO_IPC;
		}	
	}
	else
	{
		zlog_debug(0, "ipc_send_noack\n");
		if(ipc_send_noack(0, pmsghdr->sender_id,
					pmsghdr->module_id, pmsghdr->msg_type, pmsghdr->msg_subtype, pmsghdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : vtysh send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return ERRNO_IPC;
		}

	}
}


#if 0
int vtysh_config_info_get_bulk(struct ipc_mesg *mesg)
{
	struct vtysh_snmp_user_info **user_info;
	struct vtysh_snmp_online_user_info **user_online_info;
	struct listnode *node, *nnode;
	struct host_users *users_loop;
	struct login_session *session_loop;
	uchar send_buf[1024];
	int i = 0;
	int replay = 0;
	char *name;
	int send_len;
	struct ipc_msghdr *pmsghdr = &(mesg->msghdr);
	int user_count = 0;

	printf("vtysh_config_info_get_bulk\n");
	if(pmsghdr->msg_subtype == VTYSH_ONLINE_USER)
	{
		name = mesg->msg_data;
		/*record index of struct*/

		printf("VTYSH_ONLINE_USER name:%s\n", name);
		/*record usernum*/
		int usernum = 0;
		
		for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
		{
			if(session_loop->user_name != NULL \
				&& strcmp(name, session_loop->user_name)
				&& nnode != NULL 
				&& listgetdata(nnode) != NULL)
			{
				replay = 1;
				usernum++;
				continue;
			}
			if(replay == 0)
				usernum++;
			
			if(replay == 1)
			{
				user_online_info[user_count] = (struct vtysh_snmp_online_user_info *)malloc \
						(sizeof(struct vtysh_snmp_online_user_info));
				user_online_info[user_count]->index.usernum = usernum;
				memcpy(user_online_info[user_count]->data.userfrom, session_loop->address, \
					strlen(session_loop->address));
				memcpy(user_online_info[user_count]->data.username, session_loop->user_name, \
					strlen(session_loop->user_name));
				user_online_info[user_count]->data.userprivilege = session_loop->user_login->privilege;
				user_count++;
				printf("user_count:%d\n", user_count);
			}
		}
		if(replay)
		{
			printf("replay\n");
			memcpy(send_buf + (sizeof(struct vtysh_snmp_online_user_info) * i), \
				user_info[i], sizeof(struct vtysh_snmp_online_user_info));	
			send_len = sizeof(struct vtysh_snmp_online_user_info) * user_count;
		}
	}
	else if(pmsghdr->msg_subtype == VTYSH_USER)
	{
		printf("VTYSH_USER *((int *)(mesg->msg_data):%d\n", *((int *)(mesg->msg_data));
		memset(send_buf, 0, 1024);
		if(*((int *)(mesg->msg_data)) == 0)
		{
			for (ALL_LIST_ELEMENTS (host.registe_user, node, nnode, users_loop))
			{
				if(users_loop != NULL)
				{
					user_info[user_count] = (struct vtysh_snmp_user_info *)malloc \
						(sizeof(struct vtysh_snmp_user_info));
					memcpy(user_info[user_count]->index.username, users_loop->name, strlen(users_loop->name));
					user_info[user_count]->data.privilege = users_loop->privilege;
					user_count++;
					printf("user_count:%d\n", user_count);
				}
			}
			for (i = 0; i <= user_count; i++)
			{
				memcpy(send_buf + (sizeof(struct vtysh_snmp_user_info) * i), \
					user_info[i], sizeof(struct vtysh_snmp_user_info));
			}
			send_len = sizeof(struct vtysh_snmp_user_info) * user_count;
			replay = 1;
		}
		else
		{
			replay = 0;
		}
	}
	else
	{
		printf("wrong msg_subtype\n");
		return -1;
	}

	/* replay snmp */
	if(replay)
	{
		printf("ipc_send_reply_bulk\n");
		if(ipc_send_reply_bulk(send_buf, send_len,
			user_count, pmsghdr->sender_id, pmsghdr->module_id, pmsghdr->msg_type, pmsghdr->msg_subtype, pmsghdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : vtysh send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return ERRNO_IPC;
		}	
	}
	else
	{
		printf("ipc_send_noack\n");
		if(ipc_send_noack(0, pmsghdr->sender_id,
					pmsghdr->module_id, pmsghdr->msg_type, pmsghdr->msg_subtype, pmsghdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return ERRNO_IPC;
		}

	}
}
#endif
#endif


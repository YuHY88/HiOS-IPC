#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/memory.h>
#include <lib/prefix.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/index.h>

#include "pim.h"
#include "pim_cmd.h"
#include "../ipmc_if.h"
#include "pim_mroute.h"
#include "inet_ip.h"

#if	0

struct cmd_node pim_instance_node =
{ 
	PIM_INSTANCE_NODE,
	"%s(config-pim-instance)# ",  
	1,
	0,
	0
};
		
DEFUN (pim_instance_view,
	pim_instance_cmd,
	"pim instance <1-255>",
	PIM_STR
	"PIM instance view\n"
	"PIM instance id, <1-255>\n"
	)
{
	uint8_t pim_id = 0;
	char *pprompt = NULL;
	struct pim_instance *ppim_instance = NULL;
	/*创建并进入pim instance视图*/
	
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "pim_instance", pim_id, argv[0], 1, 255 );
	
	/*检测pim instance 是否已经存在*/
	ppim_instance = pim_instance_array[pim_id];
	if(ppim_instance == NULL)
	{
		/*根据local-id创建会话*/
		ppim_instance = pim_instance_create(pim_id);
		if(ppim_instance == NULL)
		{
			vty_out ( vty, "Create pim instance %d failed!%s", pim_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		else
		{
			pim_instance_array[pim_id] = ppim_instance;
		}
	}
	/*进入会话视图*/
    vty->node = PIM_INSTANCE_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%s(config-pim-instance-%d)# ", "huahuan", pim_id);
	}
	vty->index = (void *)(uint32_t)pim_id;
	
	return CMD_SUCCESS;
}

DEFUN (undo_pim_instance_view,
	undo_pim_instance_cmd,
	"no pim instance <1-255>",
	NO_STR
	PIM_STR
	"PIM instance\n"
	"PIM instance id, <1-255>\n"
	)
{
	/*删除一个实例*/
	uint8_t pim_id = 0;
	char *pprompt = NULL;
	struct pim_instance *ppim_instance = NULL;

    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "pim_instance", pim_id, argv[0], 1, 255 );
	
	/*检测pim instance 是否已经存在*/
	ppim_instance = pim_instance_array[pim_id];
	if(ppim_instance == NULL)
	{
		/*实例不存在，则直接退出*/
		vty_out(vty, "pim instance %d not exist%s", pim_id, VTY_NEWLINE);
	}
	else
	{
		/*删除实例*/
		pim_instance_delete(pim_id);
	}
	return CMD_SUCCESS;
}
DEFUN(pim_instance_mcif_add_cmd_func,
	pim_instance_mcif_add_cmd,
	"pim enable instance <1-255>",
	PIM_STR
	"pim enable\n"
	"pim instance\n"
	"PIM instance id, <1-255>\n"
	)
{
	uint8_t pim_id = 0;
	uint32_t ifindex = 0;
    char ifname[IFNET_NAMESIZE] = "";
	struct ipmc_if *pif = NULL;
	struct pim_instance *ppim_instance = NULL;

    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "pim_instance", pim_id, argv[0], 1, 255 );
	
	/*获取接口索引*/
    ifindex = (uint32_t)vty->index;

	/*获取接口名*/
	ifm_get_name_by_ifindex ( ifindex , ifname);

	/*判断接口是否有ip地址*/
	if(ipmc_if_main_addr_get(ifindex) == 0)
	{
		vty_out(vty, "please set ip address on if %s first!%s", ifname, VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*检测pim instance 是否已经存在*/
	ppim_instance = pim_instance_array[pim_id];
	if(ppim_instance == NULL)
	{
		/*实例不存在，则直接退出*/
		vty_out(vty, "pim instance %d not exist%s", pim_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	pif = ipmc_if_lookup(ifindex);
	if(pif == NULL)
	{
		pif = ipmc_if_add(ifindex);
		if(pif == NULL)
		{
			vty_out(vty, "ipmc_if_add(0x%x) return NULL%s", ifindex, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	/*接口使能pim或者接口使能igmp都会创建接口*/
	/*将接口添加到pim实例中去*/
	pim_instance_ipmc_if_add(pif, ppim_instance);
	vty_out(vty, "if %s is in pim instance %d%s", ifname, pim_id, VTY_NEWLINE);
	
	return CMD_SUCCESS;
}


DEFUN(pim_instance_mode_cmd_func,
	pim_instance_mode_cmd,
	"pim (sm|dm)",
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)
{
	uint8_t pim_id = 0, mode = 0;
	struct pim_instance *ppim_instance = NULL;
	
	/*获取到pim-id*/
	pim_id = (uint8_t)(uint32_t)vty->index;
	if(pim_id < 1 || pim_id > 255)
	{
		vty_out(vty, "invalid pim_id %d!%s", pim_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查pim instance 是否已经存在*/
	ppim_instance = pim_instance_array[pim_id];
	if(ppim_instance == NULL)
	{
		/*实例不存在，则直接退出*/
		vty_out(vty, "pim instance %d not exist%s", pim_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(strncmp(argv[0], "sm", 2) == 0)
	{
		mode = 1;
	}
	else if(strncmp(argv[0], "dm", 2) == 0)
	{
		mode = 2;
		vty_out(vty, "Current pim-dm not support, only pim-sm support!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	ppim_instance->pim_conf.mode = mode;
	/*使用默认值*/
	ppim_instance->pim_conf.dr_priority = PIM_DR_PRIORITY_DEFAULT;
	ppim_instance->pim_conf.hello_itv = PIM_HELLO_PERIOD_DEFAULT;
	ppim_instance->pim_conf.lan_delay = PIM_LAN_DELAY_DEFAULT;
	ppim_instance->pim_conf.override = PIM_OVERRIDE_DEFAULT;

	return CMD_SUCCESS;
}

DEFUN(undo_pim_instance_mode_cmd_func,
	undo_pim_instance_mode_cmd,
	"no pim (sm|dm)",
	NO_STR
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)
{
	return CMD_SUCCESS;
}
#endif

DEFUN(pim_debug_cmd_func,
	pim_debug_cmd,
	"debug pim packet",
	DEBUG_STR
	PIM_STR
	"pim packet\n"
	)
{
	return CMD_SUCCESS;
}

DEFUN(undo_pim_debug_cmd_func,
	undo_pim_debug_cmd,
	"no debug pim packet",
	NO_STR
	DEBUG_STR
	PIM_STR
	"pim packet\n"
	)
{
	return CMD_SUCCESS;
}

DEFUN(pim_ssm_cmd_func,
	pim_ssm_cmd,
	"pim ssm",
	PIM_STR
	"Configure Source Specific Multicast\n"
	)
{
	/*配置模式下配置*/
	/*pim ssm 与igmpv3，指定源组播*/
	/*开启pim ssm 后默认的232.0.0.0/8范围的组地址都为指定源组播*/
	if(pim_instance_global->pim_conf.ssm_state)
	{
		return CMD_SUCCESS;
	}
	else
	{
		pim_instance_global->pim_conf.ssm_state = PIM_SSM_ENABLE;
	}
	return CMD_SUCCESS;
}

DEFUN(undo_pim_ssm_cmd_func,
	undo_pim_ssm_cmd,
	"no pim ssm",
	NO_STR
	PIM_STR
	"Configure Source Specific Multicast\n"
	)
{
	if(pim_instance_global->pim_conf.ssm_state == PIM_SSM_ENABLE)
	{
		pim_instance_global->pim_conf.ssm_state = PIM_SSM_DISABLE;
	}
	return CMD_SUCCESS;
}

/*下发 iif 1,oif 2 ,(1.1.1.1,225.1.1.1)*/
DEFUN(pim_debug_mfib_func,
	pim_debug_mfib_cmd,
	"pim switch-entry source A.B.C.D group A.B.C.D iif interface gigabitethernet USP oif interface gigabitethernet USP ",
	PIM_STR
	"Switch-entry\n"
	"Source Address, Unicast address\n"
	"Format A.B.C.D\n"
	"Group Address ,Mulitcast address\n"
	"Format A.B.C.D\n"
	"In port\n"
	CLI_INTERFACE_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	"Out port\n"
	CLI_INTERFACE_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	)
{
	int ret = 0;
    uint32_t src = 0, grp = 0;
    uint32_t iif = 0, oif = 0;
	uint32_t mode = PIM_SM;
	struct ipmc_if *piif = NULL, *poif = NULL;
	struct if_set oiflist = {};
    struct mc_cache_t cache = {0};

    /*参数获取及合法性检查*/
    src = inet_strtoipv4((char *)argv[0]);
    ret = inet_valid_ipv4(src);
	if (1 != ret)
	{
		vty_out(vty, "Error:The src 0x%x, ip is invalid.%s", src, VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(src == 0)
	{
		vty_out(vty, "Error:The source address can't be zero.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	grp = inet_strtoipv4((char *)argv[1]);
	if(!ipv4_is_multicast(grp))
	{
		vty_out(vty, "Error:The group address should be a multicast address.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	iif = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[2]);
	oif = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[3]);
	
	piif = ipmc_if_pim_enable( iif, mode);
	poif = ipmc_if_pim_enable( oif, mode);
	if(piif == NULL || poif == NULL)
	{
		vty_out(vty, "Error:pim sm enable failed.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	memset(&oiflist, 0, sizeof(struct if_set));
	memset(&cache, 0, sizeof(struct mc_cache_t));
	IF_SET(poif->index, &oiflist);
	cache.dip = grp;
	cache.sip = src;
	cache.iif = piif->index;
	IF_COPY(&(cache.oif), &(oiflist), sizeof(struct if_set));
	ret = ipmc_msg_send_to_hal((void *)(&cache), sizeof(struct mc_cache_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
						 IPMC_SUBTYPE_MFIB, IPC_OPCODE_ADD, 1);
	if(ret) 
	{
		IPMC_LOG_ERROR("Failed to send add mc group to hal.\n");
		return ERRNO_IPC;
	}
	return CMD_SUCCESS;
}

DEFUN(pim_instance_mcif_add_cmd_func,
	pim_instance_mcif_add_cmd,
	"pim (sm|dm)",
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)
{
	uint8_t mode = 0;
	uint32_t ifindex = 0;
    char ifname[IFNET_NAMESIZE] = "";
	struct ipmc_if *pif = NULL;

	/*获取接口索引*/
    ifindex = (uint32_t)vty->index;

	/*获取接口名*/
	ifm_get_name_by_ifindex ( ifindex , ifname);

	/*判断接口是否有ip地址*/
	if(ipmc_if_main_addr_get(ifindex) == 0)
	{
		vty_out(vty, "please set ip address on if %s first!%s", ifname, VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*检查pim instance 是否已经存在*/
	if(pim_instance_global == NULL)
	{
		/*实例不存在，则直接退出*/
		vty_out(vty, "pim_instance_global NULL%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(strncmp(argv[0], "sm", 2) == 0)
	{
		mode = PIM_SM;
	}
	else if(strncmp(argv[0], "dm", 2) == 0)
	{
		vty_out(vty, "Current pim-dm not support, only pim-sm support!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	pif = ipmc_if_pim_enable( ifindex, mode);
	if(pif == NULL)
	{
		vty_out(vty, "ipmc_if_pim_enable(%x, sm) return NULL%s", ifindex, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN(pim_instance_mcif_del_cmd_func,
	pim_instance_mcif_del_cmd,
	"no pim (sm|dm)",
	NO_STR
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)
{
	uint8_t mode = 0;
	uint32_t ifindex = 0;
    char ifname[IFNET_NAMESIZE] = "";
	sint32 ret = ERRNO_SUCCESS;

	/*获取接口索引*/
    ifindex = (uint32_t)vty->index;

	/*获取接口名*/
	ifm_get_name_by_ifindex ( ifindex , ifname);
	
	if(strncmp(argv[0], "sm", 2) == 0)
	{
		mode = PIM_SM;
	}
	else if(strncmp(argv[0], "dm", 2) == 0)
	{
		vty_out(vty, "Current pim-dm not support, only pim-sm support!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	ret = ipmc_if_pim_disable( ifindex, mode);
	if(ret != ERRNO_SUCCESS)
	{
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}

DEFUN(pim_instance_hello_interval_cmd_func,
	pim_instance_hello_interval_cmd,
	"pim hello interval <1-3600>",
	PIM_STR
	"pim hello message\n"
	"pim hello message interval\n"
	"interval value, <1-3600>, default 30(s)"
	)
{
	if(vty->node == CONFIG_NODE)
	{
		/*全局模式*/
	}
	else
	{
		/*接口模式*/
	}
	return CMD_SUCCESS;
}

DEFUN(undo_pim_instance_hello_interval_cmd_func,
	undo_pim_instance_hello_interval_cmd,
	"no pim hello interval",
	NO_STR
	PIM_STR
	"pim hello message\n"
	"pim hello message interval\n"
	)
{
	return CMD_SUCCESS;
}

DEFUN(pim_instance_dr_priority_cmd_func,
	pim_instance_dr_priority_cmd,
	"pim dr priority <0-255>",
	PIM_STR
	"pim DR(Designated Router)\n"
	"pim DR priority\n"
	"priority value, <0-255>, default 1"
	)
{
	return CMD_SUCCESS;
}

DEFUN(undo_pim_instance_dr_priority_cmd_func,
	undo_pim_instance_dr_priority_cmd,
	"no pim dr priority",
	NO_STR
	PIM_STR
	"pim DR(Designated Router)\n"
	"pim DR priority\n"
	)
{
	return CMD_SUCCESS;
}

DEFUN(pim_instance_static_rp_cmd_func,
	pim_instance_static_rp_cmd,
	"pim static-rp A.B.C.D",
	PIM_STR
	"pim static RP(Rendezvous Point)\n"
	"RP address, IP Unicast address\n"
	)
{
	uint32_t old_rp = 0, rp_addr = 0, ret = 0, ifindex = 0;
	struct ipmc_if *pif = NULL;

	rp_addr = inet_strtoipv4((char *)argv[0]);
	ret = inet_valid_ipv4(rp_addr);
	if (ret != 1)
	{
		vty_out(vty, "Warning: Please input a Unicast address.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(rp_addr == 0)
	{
		vty_out(vty, "Warning: Please input a non-zero address.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*rp地址不一定在本地，因此不需要检查接口是否为本地*/
	/*根据rpaddr查找接口,检查是否为本地地址*/
#if 0
	ifindex = ip4_addr_local_main(rp_addr);
	if(ifindex == 0)
	{
		vty_out(vty, "Warning: Please input a interface address of the device.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*检查接口是否为ipmc_if*/
	pif = ipmc_if_lookup(ifindex);
	if(pif == NULL || pif->pim_mode == 0)
	{
		vty_out(vty, "Warning: The RP address must be a pim if.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
#endif
	if(pim_instance_global->rp_addr == rp_addr)
	{
		/*rp已经配置*/
		return CMD_SUCCESS;
	}
	old_rp = pim_instance_global->rp_addr;
	/*保存rp_addr*/
	pim_instance_global->rp_addr = rp_addr;
	pim_mrt_rp_change(pim_instance_global);
	
	return CMD_SUCCESS;
}

DEFUN(undo_pim_instance_static_rp_cmd_func,
	undo_pim_instance_static_rp_cmd,
	"no pim static-rp A.B.C.D",
	NO_STR
	PIM_STR
	"pim static RP(Rendezvous Point)\n"
	"RP address, IP Unicast address\n"
	)
{
	return CMD_SUCCESS;
}

DEFUN(pim_instance_jp_interval_cmd_func,
	pim_instance_jp_interval_cmd,
	"pim join-prune interval <1-600>",
	PIM_STR
	"pim join/prune message\n"
	"pim join/prune message send interval\n"
	"interval value, <1-600>, default 30(s)\n"
	)
{
	return CMD_SUCCESS;
}

DEFUN(undo_pim_instance_jp_interval_cmd_func,
	undo_pim_instance_jp_interval_cmd,
	"no pim join-prune interval",
	NO_STR
	PIM_STR
	"pim join/prune message\n"
	"pim join/prune message send interval\n"
	)
{
	return CMD_SUCCESS;
}
char *ipmc_route_parse_flages(uint32_t flages, char *fla)
{
	uint8_t len = 0;
	
	if(flages & PIM_MRT_SM)//S
	{
		*(fla + len++) = 'S';
	}
	if(flages & PIM_MRT_SSM)//SSM
	{
		*(fla + len++) = 's';
	}
	if(flages & PIM_MRT_SPT)//T
	{
		*(fla + len++) = 'T';
	}
	if(flages & PIM_MRT_LOCAL)//local
	{
		*(fla + len++) = 'L';
	}
	if(flages & PIM_MRT_UNUSED)//unused
	{
		*(fla + len++) = 'U';
	}
	if(flages & PIM_MRT_CONNECTED)//C
	{
		*(fla + len++) = 'C';
	}
	if(flages & PIM_MRT_CACHE)//c
	{
		*(fla + len++) = 'c';
	}
	if(flages & PIM_MRT_RPT)//R
	{
		*(fla + len++) = 'R';
	}
	if(flages & PIM_MRT_PRUNED)//P
	{
		*(fla + len++) = 'P';
	}
	if(flages & PIM_MRT_SWT)//J
	{
		*(fla + len++) = 'J';
	}
	if(flages & PIM_MRT_REG)//r
	{
		*(fla + len++) = 'r';
	}
	*(fla + len) = '\0';
	return fla;
}

DEFUN(pim_mrt_show_cmd_func,
	pim_mrt_show_cmd,
	"show pim routing-table",
	SHOW_STR
	PIM_STR
	"PIM routing information\n"
	)
{
	uint16_t hash1 = 0, hash2 = 0, i = 0, wc_cnt = 0, sg_grp_cnt = 0, sg_cnt = 0;
	struct hash_bucket *bucket1 = NULL, *bucket2 = NULL;
	struct hash_table *wc_tab = NULL, *sg_tab = NULL;
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_sg *sg = NULL;
	uint32_t src_addr = 0, grp_addr = 0, rp = 0, flags = 0, iif = 0, rpf = 0;
	struct if_set res_oif;
	char tmp[36], addr[16], addr1[16], addr2[16], addr3[16], addr4[16], *oif = NULL;
	char ifname[IFNET_NAMESIZE+1] = "";
	struct pim_mrt_sg_grp *sg_grp = NULL;
	
	
    vty_out(vty, "Flags: S-SM D-DM s-SSM C-Connected c-Cache P-Pruned R-RPT T-SPT r-Register J-Switch U-Unused  L-Local .%s", VTY_NEWLINE);
	vty_out(vty, "%-33s %-16s %-8s %-23s %-16s %-22s%s", "(*,G)/(S,G)", "RP", "Flags", 
		"Incoming Interface", "RPF Neighbor", "Outcoming Interface", VTY_NEWLINE);
	if(pim_instance_global== NULL)
	{
		return CMD_SUCCESS;
	}
	wc_tab = &(pim_instance_global->pim_wc_table);
	sg_tab = &(pim_instance_global->pim_sg_table);
	if(pim_instance_global->pim_wc_table.num_entries)
	{
		wc_cnt = 0;
		/*(*,G)表项先显示，然后显示(*,G)下的sg*/
		for(hash1 = 0; hash1 < HASHTAB_SIZE; hash1++)
		{
			for(bucket1 = wc_tab->buckets[hash1]; bucket1; bucket1 = bucket1->next)
			{
				wc = (struct pim_mrt_wc *)(bucket1->data);
				wc_cnt++;
				if(wc)
				{
					grp_addr = wc->grp_addr;
					rp = wc->rp_addr;
					flags = wc->flag;
					iif = wc->upstream_ifindex;
					rpf = wc->rpf_nbr;
					sprintf(tmp, "(*, %s)", inet_ipv4tostr(grp_addr, addr1));
					vty_out(vty, "%-33s", tmp);
					if(rp == 0)
					{
						vty_out(vty, "%-16s", "NULL");
					}
					else
					{
						sprintf(tmp, "%s", inet_ipv4tostr(rp, addr1));
						vty_out(vty, "%-16s", tmp);
					}
					if(flags == 0)
					{
						vty_out(vty, "%-8s", "NULL");
					}
					else
					{
						ipmc_route_parse_flages(flags, tmp);
						vty_out(vty, "%-8s", tmp);
					}
					/*根据ifindex获取ifname*/
					if(iif == 0)
					{
						vty_out(vty, "%-23s", "NULL");
					}
					else
					{
						ifm_get_name_by_ifindex(iif, ifname);
						vty_out(vty, "%-23s", ifname);
					}
					if(rpf == 0)
					{
						vty_out(vty, "%-20s", "NULL");
					}
					else
					{
						sprintf(tmp, "%s", inet_ipv4tostr(rpf, addr1));
						vty_out(vty, "%-16s", tmp);
					}
					if(wc->oif_num == 0)
					{
						vty_out(vty, "%-22s%s", "NULL", VTY_NEWLINE);
					}
					else
					{
						oif = (char *)malloc((22+1) * wc->oif.oif_num);
						if(oif)
						{
							*oif = '\0';
							for(i = 0; i < IPMC_IF_MAX; i++)
							{
								if(IF_ISSET(i, &wc->oif.res_oif))
								{
									/*根据索引*/
									ifm_get_name_by_ifindex(ipmc_if_s.ipmc_if_array[i], ifname);
									strcat(oif, ifname);
									strcat(oif, ",");
								}
							}	
							if(strlen(oif))
							{
								*(oif + strlen(oif) - 1) = '\0';
							}
							vty_out(vty, "%-22s%s", oif, VTY_NEWLINE);
							free(oif);
							oif = NULL;
						}
					}

					/*(*,G)下的(S,G)*/
					sg_grp = pim_mrt_sg_grp_lookup(sg_tab, grp_addr);
					if(sg_grp)
					{
						if(sg_grp->sg_num)
						{
							sg_cnt = 0;
							for(hash2 = 0; hash2 < HASHTAB_SIZE; hash2++)
							{
								for(bucket2 = sg_grp->sg_table.buckets[hash2]; bucket2; bucket2 = bucket2->next)
								{
									sg = (struct pim_mrt_sg *)(bucket2->data);
									sg_cnt++;
									if(sg)
									{
										src_addr = sg->src_addr;
										grp_addr = sg->grp_addr;
										rp = sg->rp_addr;
										flags = sg->flag;
										iif = sg->upstream_ifindex;
										rpf = sg->rpf_nbr;
										sprintf(tmp, "(%s,%s)", inet_ipv4tostr(src_addr, addr1), 
																inet_ipv4tostr(grp_addr, addr));
										vty_out(vty, "%-33s", tmp);
										if(rp == 0)
										{
											vty_out(vty, "%-16s", "NULL");
										}
										else
										{
											sprintf(tmp, "%s", inet_ipv4tostr(rp, addr1));
											vty_out(vty, "%-16s", tmp);
										}
										if(flags == 0)
										{
											vty_out(vty, "%-8s", "NULL");
										}
										else
										{
											ipmc_route_parse_flages(flags, tmp);
											vty_out(vty, "%-8s", tmp);
										}
										/*根据ifindex获取ifname*/
										if(iif == 0)
										{
											vty_out(vty, "%-23s", "NULL");
										}
										else
										{
											ifm_get_name_by_ifindex(iif, ifname);
											vty_out(vty, "%-23s", ifname);
										}
										if(rpf == 0)
										{
											vty_out(vty, "%-20s", "NULL");
										}
										else
										{
											sprintf(tmp, "%s", inet_ipv4tostr(rpf, addr1));
											vty_out(vty, "%-16s", tmp);
										}
										if(sg->oif.oif_num == 0)
										{
											vty_out(vty, "%-22s%s", "NULL", VTY_NEWLINE);
										}
										else
										{
											oif = (char *)malloc((22+1) * sg->oif.oif_num);
											if(oif)
											{
												*oif = '\0';
												for(i = 0; i < IPMC_IF_MAX; i++)
												{
													if(IF_ISSET(i, &sg->oif.res_oif))
													{
														/*根据索引*/
														ifm_get_name_by_ifindex(ipmc_if_s.ipmc_if_array[i], ifname);
														strcat(oif, ifname);
														strcat(oif, ",");
													}
												}	
												if(strlen(oif))
												{
													*(oif + strlen(oif) - 1) = '\0';
												}
												vty_out(vty, "%-22s%s", oif, VTY_NEWLINE);
												free(oif);
												oif = NULL;
											}
										}
									}
									if(sg_cnt == sg_grp->sg_num)
									{
										break;
									}
								}
								if(sg_cnt == sg_grp->sg_num)
								{
									break;
								}
							}
						}
					}
				}
				if(wc_cnt == pim_instance_global->pim_wc_table.num_entries)
				{
					break;
				}
			}
			if(wc_cnt == pim_instance_global->pim_wc_table.num_entries)
			{
				break;
			}
		}
	}

	if(pim_instance_global->pim_sg_table.num_entries)
	{
		sg_grp_cnt = 0;
		for(hash1 = 0; hash1 < HASHTAB_SIZE; hash1++)
		{
			for(bucket1 = sg_tab->buckets[hash1]; bucket1; bucket1 = bucket1->next)
			{
				sg_grp = (struct pim_mrt_sg_grp *)(bucket1->data);
				sg_grp_cnt++;
				if(sg_grp)
				{
					if(sg_grp->sg_num)
					{
						sg_cnt = 0;
						for(hash2 = 0; hash2 < HASHTAB_SIZE; hash2++)
						{
							for(bucket2 = sg_grp->sg_table.buckets[hash2]; bucket2; bucket2 = bucket2->next)
							{
								sg = (struct pim_mrt_sg *)(bucket2->data);
								sg_cnt++;
								if(sg->wc_mrt == NULL)
								{
									src_addr = sg->src_addr;
									grp_addr = sg->grp_addr;
									rp = sg->rp_addr;
									flags = sg->flag;
									iif = sg->upstream_ifindex;
									rpf = sg->rpf_nbr;
									inet_ipv4tostr(src_addr, addr1);
									inet_ipv4tostr(grp_addr, addr);
									sprintf(tmp, "(%s,%s)", addr1, addr);
									vty_out(vty, "%-33s", tmp);
									if(rp == 0)
									{
										vty_out(vty, "%-16s", "NULL");
									}
									else
									{
										inet_ipv4tostr(rp, addr2);
										sprintf(tmp, "%s", addr2);
										vty_out(vty, "%-16s", tmp);
									}
									if(flags == 0)
									{
										vty_out(vty, "%-8s", "NULL");
									}
									else
									{
										ipmc_route_parse_flages(flags, tmp);
										vty_out(vty, "%-8s", tmp);
									}
									/*根据ifindex获取ifname*/
									if(iif == 0)
									{
										vty_out(vty, "%-23s", "NULL");
									}
									else
									{
										ifm_get_name_by_ifindex(iif, ifname);
										vty_out(vty, "%-23s", ifname);
									}
									if(rpf == 0)
									{
										vty_out(vty, "%-20s", "NULL");
									}
									else
									{
										inet_ipv4tostr(rpf, addr3);
										sprintf(tmp, "%s", addr3);
										vty_out(vty, "%-16s", tmp);
									}
									if(sg->oif.oif_num == 0)
									{
										vty_out(vty, "%-22s%s", "NULL", VTY_NEWLINE);
									}
									else
									{
										oif = (char *)malloc((22+1) * sg->oif.oif_num);
										if(oif)
										{
											*oif = '\0';
											for(i = 0; i < IPMC_IF_MAX; i++)
											{
												if(IF_ISSET(i, &sg->oif.res_oif))
												{
													/*根据索引*/
													ifm_get_name_by_ifindex(ipmc_if_s.ipmc_if_array[i], ifname);
													strcat(oif, ifname);
													strcat(oif, ",");
												}
											}	
											if(strlen(oif))
											{
												*(oif + strlen(oif) - 1) = '\0';
											}
											vty_out(vty, "%-22s%s", oif, VTY_NEWLINE);
											free(oif);
											oif = NULL;
										}
									}
								}
								if(sg_cnt == sg_grp->sg_num)
								{
									break;
								}
							}
							if(sg_cnt == sg_grp->sg_num)
							{
								break;
							}
						}
					}
				}
				if(sg_grp_cnt == pim_instance_global->pim_sg_table.num_entries)
				{
					break;
				}
			}
			if(sg_grp_cnt == pim_instance_global->pim_sg_table.num_entries)
			{
				break;
			}
		}
	}
	return CMD_SUCCESS;
}

DEFUN(pim_if_nbr_show_cmd_func,
	pim_if_nbr_show_cmd,
	"show pim interface neighbor",
	SHOW_STR
	PIM_STR
	"interface\n"
	"pim neighbor information\n"
	)
{
	uint32_t index = 0, ifindex = 0, count = 0;
	struct ipmc_if *pif = NULL;
	char ifname[IFNET_NAMESIZE+1] = "";
	char dr_addr[20] = "";
	char mode[10] = "";

	if(pim_instance_global == NULL)
	{
		return CMD_SUCCESS;
	}
	vty_out(vty, "%-4s %-16s %-4s %-10s %-15s %-12s %-s%s", "No", "Interface", "Mode", 
		"NBR Count", "Hello Interval", "DR Priority", "DR Address", VTY_NEWLINE);
	
	/*遍历组播接口，查找ipmc接口*/
	for(index = 0; index < IPMC_IF_MAX; index++)
	{
		//if(pim_instance_global->pim_if_array[index] != 0)
		if(ipmc_if_s.ipmc_if_array[index] != 0)
		{
			ifindex = ipmc_if_s.ipmc_if_array[index];
			/*根据ifindex获取ipmc_if*/
			pif = ipmc_if_lookup(ifindex);
			if(pif == NULL)
			{
				ipmc_if_s.ipmc_if_array[index] = 0;
				continue;
			}
			if(pif->pim_mode == 0 || (pif->pim_mode != PIM_SM && pif->pim_mode != PIM_DM))
			{
				vty_out(vty, "ifindex %x is not a pim if,igmp mode:%u, pim mode:%u%s", 
					ifindex, pif->igmp_flag, pif->pim_mode, VTY_NEWLINE);
				continue;
			}
			if(pif->pim_nbr == NULL)
			{
				vty_out(vty, "ifindex %x pif->pim_nbr == NULL%s", ifindex, VTY_NEWLINE);
				pif->pim_mode = 0;
				continue;
			}
			if(pif->pim_conf == NULL)
			{
				vty_out(vty, "ifindex %x pif->pim_conf == NULL%s", ifindex, VTY_NEWLINE);
				pif->pim_mode = 0;
				continue;
			}
			if(pif->pim_mode == PIM_SM)
			{
				strcpy(mode, "SM");
			}
			else
			{
				strcpy(mode, "DM");
			}
			/*根据ifindex获取ifname*/
			ifm_get_name_by_ifindex(ifindex, ifname);
			inet_ipv4tostr(pif->pim_nbr->cur_dr.dr_addr, dr_addr);
			vty_out(vty, "%-4d %-16s %-4s %-10u %-15u %-12u %s%s",
				++count, ifname, mode, pif->pim_nbr->nbr_num,
				pif->pim_conf->hello_itv, pif->pim_conf->dr_priority, dr_addr, VTY_NEWLINE);
		}
	}
	
	return CMD_SUCCESS;
}

int pim_instance_config_write (struct vty *vty)
{
	return CMD_SUCCESS;
}

void pim_cmd_init()
{
	//install_node(&pim_instance_node, pim_instance_config_write);
	
	//install_default(PIM_INSTANCE_NODE);
	
	/*在配置模式下注册命令*/
	/*[no] pim instance <1-255>*/
	//install_element(CONFIG_NODE, &pim_instance_cmd);
	//install_element(CONFIG_NODE, &undo_pim_instance_cmd);
	
	/*[no] debug pim packet*/
	install_element(CONFIG_NODE, &pim_debug_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &undo_pim_debug_cmd, CMD_LOCAL);

	install_element(CONFIG_NODE, &pim_ssm_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &undo_pim_ssm_cmd, CMD_LOCAL);
	
	install_element(CONFIG_NODE, &pim_debug_mfib_cmd, CMD_LOCAL);
	
	/*接口加入到pim instance*/
	/*[no] pim enable instance <1-255>*/
	install_element(PHYSICAL_IF_NODE, &pim_instance_mcif_add_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &pim_instance_mcif_del_cmd, CMD_LOCAL);

	/*PIM instance视图下配置pim*/
	/*[no] pim (sm|dm)*/
	//install_element(PHYSICAL_IF_NODE, &pim_instance_mode_cmd);
	//install_element(PHYSICAL_IF_NODE, &undo_pim_instance_mode_cmd);

	/*[no] pim hello interval <1-3600>*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_hello_interval_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_hello_interval_cmd);
	install_element(CONFIG_NODE, &pim_instance_hello_interval_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &undo_pim_instance_hello_interval_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &pim_instance_hello_interval_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &undo_pim_instance_hello_interval_cmd, CMD_LOCAL);
	
	/*[no] pim dr priority <0-255>*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_dr_priority_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_dr_priority_cmd);
	install_element(CONFIG_NODE, &pim_instance_dr_priority_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &undo_pim_instance_dr_priority_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &pim_instance_dr_priority_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &undo_pim_instance_dr_priority_cmd, CMD_LOCAL);
	
	/*[no] pim static-rp A.B.C.D*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_static_rp_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_static_rp_cmd);
	install_element(CONFIG_NODE, &pim_instance_static_rp_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &undo_pim_instance_static_rp_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &pim_instance_static_rp_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &undo_pim_instance_static_rp_cmd, CMD_LOCAL);

	/*[no] pim join-prune interval <1-600>*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_jp_interval_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_jp_interval_cmd);
	install_element(CONFIG_NODE, &pim_instance_jp_interval_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &undo_pim_instance_jp_interval_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &pim_instance_jp_interval_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &undo_pim_instance_jp_interval_cmd, CMD_LOCAL);

	/*show pim if neighbor info*/	
	install_element(CONFIG_NODE, &pim_if_nbr_show_cmd, CMD_LOCAL);
	/*show pim mrt*/
	install_element(CONFIG_NODE, &pim_mrt_show_cmd, CMD_LOCAL);
#if 0	
	/*[no] pim spt-switch threshold <0-1000>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_spt_switch_threshold_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_spt_switch_threshold_cmd);
	
	/*[no] pim prune delay <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_prune_delay_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_prune_delay_cmd);
	
	/*[no] pim prune hold-time <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_prune_holdtime_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_prune_holdtime_cmd);
	
	/*[no] pim source age-time <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_src_agetime_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_src_agetime_cmd);
	
	/*[no] pim group  A.B.C.D/<1-32>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_pim_group_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_pim_group_cmd);

	/*[no] pim c-rp interface if*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_crp_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_crp_cmd);

	/*[no] pim c-rp priority <0-255>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_crp_priority_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_crp_priority_cmd);

	/*[no] pim advertise interval <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_crp_adv_interval_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_crp_adv_interval_cmd);

	/*[no] pim c-bsr  interface if*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_cbsr_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_cbsr_cmd);
	
	/*[no] pim c-bsr priority <0-255>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_cbsr_priority_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_cbsr_priority_cmd);
	
	/*[no] pim c-bsr hash <1-255>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_cbsr_hash_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_cbsr_hash_cmd);
	
	/*[no] pim bootstrap interval <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_bsr_bootstrap_interval_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_bsr_bootstrap_interval_cmd);
#endif
}


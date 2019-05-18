/*
 * =====================================================================================
 *
 *       Filename:  igmp_cmd.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/28/2017 09:10:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <lib/command.h>
#include <lib/vty.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/prefix.h>
#include <lib/index.h>
#include <lib/linklist.h>
#include <lib/memory.h>

#include "igmp.h"
#include "igmpv3.h"
#include "ipmc_ip.h"
#include "ipmc_if.h"
#include "ipmc_group.h"
#include "ipmc_main.h"
#include "igmp_cmd.h"
#include "../pim/pim.h"

const struct message igmp_dbg_name[] = {
         {.key = IGMP_DBG_RCV, 	.str = "rcvmsg"},
         {.key = IGMP_DBG_SND, 	.str = "sendmsg"},
         {.key = IGMP_DBG_OTHER,.str = "other"},
         {.key = IGMP_DBG_ALL,  .str = "all_debug"},
};

static struct cmd_node igmp_node =
{ 
	IGMP_NODE,  
	"%s(config-igmp)# ",  
	1, 
};

static uint8_t if_vpn_get(uint32_t ifindex)
{
	uint8_t vpn;

	vpn = 0;

	return vpn;
}

/*igmp vpn instance command*/
DEFUN (igmp_instance_cfg,
	igmp_instance_cfg_cmd,
	"igmp instance [<1-128>]",
	"Igmp instance command\n"
	"Igmp instance\n"
	"Igmp instance id\n")
{
	struct igmp_instance *pinst = NULL;
	char *pprompt = NULL;
	uint32_t vpn = 0;

	if (NULL != argv[0])
	{
		vpn = atoi(argv[0]);

		if ((vpn < IGMP_INS_MIN) || (vpn > IGMP_INS_MAX))
		{
			vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
		    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
			
			return CMD_WARNING;
		}
	}

	/*check vpn instance create or not, maybe get from other process, default exsit*/


	/*chek wether igmp vpn instance exist or not*/
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		pinst = igmp_instance_create(vpn);
		if (NULL == pinst)
		{
			vty_out(vty, "IGMP VPN instance Create failure%s", VTY_NEWLINE);
			
	    	return CMD_WARNING;
		}

		listnode_add(igmp_inst, pinst);
	}
	
	/*进入会话视图*/
    vty->node = IGMP_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%s(config-igmp-instance-%d)# ", "huahuan", vpn);
	}
	
	vty->index = (void *)vpn;
	
	return CMD_SUCCESS;
}


DEFUN (no_igmp_instance,
	no_igmp_instance_cmd,
	"no igmp instance [<1-128>]",
	"No igmp instance command\n"
	"Igmp\n"
	"Igmp instance\n"
	"Igmp instance id:1-128\n")
{
	struct igmp_instance *pinst = NULL;
	uint32_t vpn = 0;
	int ret;

	if (NULL != argv[0])
	{
		vpn = atoi(argv[0]);
	}

	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check vpn instance create or not, maybe get from other process, default exsit*/


	/*chek wether igmp vpn instance exist or not*/
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "IGMP VPN Instance not exist%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	ret = igmp_instance_delete(vpn);
	if (ret == ERRNO_PARAM_ILLEGAL)
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

static void inst_query_reschedule(struct igmp *pigmp)
{
	if (pigmp->query_timer) 
	{
		if (pigmp->other_querier_timer)
		{
			return;
		}

    	//igmp_general_query_off(pigmp);
    	//igmp_general_query_on(pigmp);
		
		igmp_general_query_timer_stop(pigmp);
		igmp_general_query_timer_start(pigmp);
  	}
  	else 
	{
		/* other querier present */
    	if (NULL == pigmp->other_querier_timer)
		{
			return;
		}

    	//igmp_other_querier_timer_off(pigmp);
    	//igmp_other_querier_timer_on(pigmp);
    	
		igmp_other_querier_timer_stop(pigmp);
		igmp_other_querier_timer_start(pigmp);
  	}
}

static int igmp_inst_version_chg(struct igmp_instance *pinst, uint8_t version)
{
	struct listnode	*node = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t *pifindex = NULL;
	uint32_t grpip;
	uint32_t dip;
	
	pinst->version = version;

	/*check wether config version on interface*/
	/*if not, change igmp pkts processing on the ports of igmp inst*/
	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex))	
	{
	    pif = ipmc_if_lookup(*pifindex);
		if (NULL == pif)
		{
			continue;
		}
		
		if (IGMP_FLAG_ENABLE != pif->igmp_flag)
		{
			continue;
		}

 		pigmp = pif->igmp;
		if (NULL != pigmp)
		{
			/*check wether config keepalive on interface*/
			if (IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_VERSION))	
			{
				continue;
			}

			igmp_startup_mode_on(pigmp);
#if 0
			/*send first query packet*/
			dip = IGMP_GENERAL_QUERY_IPADDRESS;
		  	grpip = 0;
			igmp_send_membership_query(pigmp, 0 /* igmp_group */,NULL, /*src ip*/
					 0 /* num_sources */, dip, grpip, pigmp->send_alert/* s_flag: always set for general queries */);
			if (pigmp->start_qry_cnt > 0)
			{
				pigmp->start_qry_cnt--;
			}
#endif
			/*set up query timer*/	
			inst_query_reschedule(pigmp);
		}	
	}

	return CMD_SUCCESS;
}

DEFUN (igmp_inst_version,
	igmp_inst_version_cmd,
	"version (1|2|3)",
	"Igmp vpn instance version command\n"
	"Igmp vpn instance version1\n"
	"Igmp vpn instance version2\n"
	"Igmp vpn instance version3\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t version;
	uint8_t vpn;
	int ret;

	/*check version wether invalid*/
	version = atoi(argv[0]);
	if ((version != IGMP_VERSION1) && 
		(version != IGMP_VERSION2) &&
		(version != IGMP_VERSION3))
	{
		vty_out(vty, "Igmp version %d is invalid%s", version, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*now just support igmp v3*/
	version = 3;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "IGMP VPN instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (pinst->version == version)
	{
		vty_out(vty, "IGMP VPN instance %d version is %d already%s", vpn, version, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	ret = igmp_inst_version_chg(pinst, version);
	if (ret)
	{
		vty_out(vty, "IGMP VPN instance %d version is set error%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_version,
	no_igmp_inst_version_cmd,
	"no version",
	"No command\n"
	"Igmp version default\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t version = IGMP_VERSION3;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*set version to 3, now just support version 3*/
	if (IGMP_VERSION3 != pinst->version)
	{
		igmp_inst_version_chg(pinst, version);
	}
	
	return CMD_SUCCESS;
}

static void inst_querier_keepalive_chg(struct igmp_instance *pinst, uint16_t keepalive)
{
	struct listnode	*node = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t *pifindex = NULL;
	uint32_t grpip;
	uint32_t dip;
	
	pinst->keepalive = keepalive;
	pinst->keepalive_set = 1;

	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex)) 
	{
		pif = ipmc_if_lookup(*pifindex);
		if (NULL == pif)
		{
			continue;
		}
		
		if (IGMP_FLAG_ENABLE != pif->igmp_flag)
		{
			continue;
		}

 		pigmp = pif->igmp;
		if (NULL != pigmp)
		{
			/*check wether config keepalive on interface*/
			if (IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_OTHER_QUERIER_ALIVE))	
			{
				continue;
			}
			
			igmp_startup_mode_on(pigmp);

			/*set up other query timer*/	
			inst_query_reschedule(pigmp);
		}
	}
}

DEFUN (igmp_inst_querier_keepalive,
	igmp_inst_querier_keepalive_cmd,
	"other-querier keepalive <60-300>",
	"Igmp other querier keepalive command\n"
	"Igmp querier keepalive\n"
	"Igmp querier keepalive time\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t keepalive;
	uint8_t vpn;

	/*check keep alive time wether invalid*/
	keepalive = atoi(argv[0]);
	if ((keepalive < IGMP_QUARY_MIN_KEEPALIVE) || 
		(keepalive > IGMP_QUARY_MAX_KEEPALIVE))
	{
		vty_out(vty, "Igmp keepalive time %d is invalid%s", keepalive, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	inst_querier_keepalive_chg(pinst, keepalive);
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_querier_keepalive,
	no_igmp_inst_querier_keepalive_cmd,
	"no other-querier keepalive",
	"No igmp other-querier keepalive command\n"
	"Igmp other-querier\n"
	"Igmp other-querier keepalive\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t keepalive;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (pinst->keepalive_set)
	{
		/*获取的存活时间为ms，需转化为s进行保存*/
		keepalive = IGMP_OQPI_MSEC(pinst->robust_count, pinst->qqi, pinst->max_resp)/1000;
		inst_querier_keepalive_chg(pinst, keepalive);
	}
	
	return CMD_SUCCESS;
}

static void inst_query_interval_chg(struct igmp_instance *pinst, int query_interval)
{
	struct listnode	*node = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t *pifindex = NULL;

	pinst->qqi = query_interval;

	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex)) 
	{
		pif = ipmc_if_lookup(*pifindex);
		if (NULL == pif)
		{
			continue;
		}
		
		if (IGMP_FLAG_ENABLE != pif->igmp_flag)
		{
			continue;
		}

 		pigmp = pif->igmp;
		if (NULL != pigmp)
		{
			/*check wether config keepalive on interface*/
			if (IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_QUERY))	
			{
				continue;
			}

			if (!pinst->keepalive_set)
			{
				pinst->keepalive = IGMP_OQPI_MSEC(pinst->robust_count, pinst->qqi, pinst->max_resp);
			}
		
			igmp_startup_mode_on(pigmp);
			
			inst_query_reschedule(pigmp);
		}
	}
}

DEFUN (igmp_inst_query_interval,
	igmp_inst_query_interval_cmd,
	"query interval <1-18000>",
	"Igmp query interval command\n"
	"Igmp query interval\n"
	"Igmp query interval seconds:1-18000\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t query_interval;
	uint8_t vpn;

	/*check query  interval wether invalid*/
	query_interval = atoi(argv[0]);
	if ((query_interval < IGMP_QUERY_MIN_INTERVAL) || 
		(query_interval > IGMP_QUERY_MAX_INTERVAL))
	{
		vty_out(vty, "Igmp query interval time %d is invalid%s", query_interval, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (query_interval != pinst->qqi)
	{
		inst_query_interval_chg(pinst, query_interval);
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_query_interval,
	no_igmp_inst_query_interval_cmd,
	"no query interval",
	"No igmp query interval command\n"
	"Igmp query\n"
	"Igmp query interval\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t query_interval;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	query_interval = IGMP_QUERY_DEFAULT_INTERVAL;
	if (query_interval != pinst->qqi)
	{
		inst_query_interval_chg(pinst, query_interval);
	}
	
	return CMD_SUCCESS;
}

static void inst_robust_count_chg(struct igmp_instance *pinst,  uint8_t count)
{
	struct listnode	*node = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t *pifindex = NULL;

	pinst->robust_count = count;

	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex)) 
	{
		pif = ipmc_if_lookup(*pifindex);
		if (NULL == pif)
		{
			continue;
		}
		
		if (IGMP_FLAG_ENABLE != pif->igmp_flag)
		{
			continue;
		}

 		pigmp = pif->igmp;
		if (NULL != pigmp)
		{
			/*check wether config keepalive on interface*/
			if (IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_ROBUST_CNT))	
			{
				continue;
			}

			if (!pinst->keepalive_set)
			{
				pinst->keepalive = IGMP_OQPI_MSEC(pinst->robust_count, pinst->qqi, pinst->max_resp);
			}
		
			igmp_startup_mode_on(pigmp);
			
			inst_query_reschedule(pigmp);
		}
	}
}

DEFUN (igmp_inst_robust_count,
	igmp_isnt_robust_count_cmd,
	"robust-count <2-5>",
	"Igmp robust count command\n"
	"Igmp robust-count number:2-5\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t robust_count;
	uint8_t vpn;

	/*check query  interval wether invalid*/
	robust_count = atoi(argv[0]);
	if ((robust_count < IGMP_ROBUST_MIN_COUNT) || 
		(robust_count > IGMP_ROBUST_MAX_COUNT))
	{
		vty_out(vty, "Igmp robust count %d is invalid%s", robust_count, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (robust_count != pinst->robust_count)
	{
		inst_robust_count_chg(pinst, robust_count);
	}
		
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_robust_count,
	no_igmp_inst_robust_count_cmd,
	"no roubust-count",
	"No roubust count command\n"
	"Igmp roubust-count\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t robust_count;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	robust_count = IGMP_ROBUST_DEFAULT_COUNT;
	if (robust_count != pinst->robust_count)
	{
		inst_robust_count_chg(pinst, robust_count);
	}
	
	return CMD_SUCCESS;
}

static void inst_lastmember_query_interval_chg(struct igmp_instance *pinst, int query_interval)
{
	pinst->last_mem_qqi = query_interval;
}

DEFUN (igmp_inst_last_member_query_interval,
	igmp_inst_last_member_query_interval_cmd,
	"last member-query interval <1-5>",
	"Igmp last member query interval command\n"
	"Igmp last member query\n"
	"Igmp last member query interval\n"
	"Igmp last member query interval seconds:1-5\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t query_interval;
	uint8_t vpn;

	/*check query  interval wether invalid*/
	query_interval = atoi(argv[0]);
	if ((query_interval < IGMP_LAST_MEMBER_QUERY_MIN_INTERVAL) || 
		(query_interval > IGMP_LAST_MEMBER_QUERY_MAX_INTERVAL))
	{
		vty_out(vty, "Igmp last member query interval time %d is invalid%s", query_interval, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (query_interval != pinst->last_mem_qqi)
	{
		inst_lastmember_query_interval_chg(pinst, query_interval);
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_last_member_query_interval,
	no_igmp_inst_last_member_query_interval_cmd,
	"no last member-query interval",
	"No igmp last member query interval command\n"
	"Igmp last member\n"
	"Igmp last member query\n"
	"Igmp last member query interval seconds:1-5\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t query_interval;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	query_interval = IGMP_LAST_MEMBER_QUERY_DEFAULT_INTERVAL;
	if (query_interval != pinst->last_mem_qqi)
	{
		inst_lastmember_query_interval_chg(pinst, query_interval);
	}
	
	return CMD_SUCCESS;
}

static void inst_max_resp_time_chg(struct igmp_instance *pinst, int resp_time)
{
	pinst->max_resp = resp_time;
}

DEFUN (igmp_inst_max_resp_time,
	igmp_inst_max_resp_time_cmd,
	"max-response-time <1-25>",
	"Igmp max response time command\n"
	"Igmp max response time seconds:1-25\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t resp_time;
	uint8_t vpn;

	/*check query  interval wether invalid*/
	resp_time = atoi(argv[0]);
	if ((resp_time < IGMP_RESPONSE_MIN_TIME) || 
		(resp_time > IGMP_RESPONSE_MAX_TIME))
	{
		vty_out(vty, "Igmp max response time %d is invalid%s", resp_time, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (resp_time != pinst->max_resp)
	{
		inst_max_resp_time_chg(pinst, resp_time);
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_max_resp_time,
	no_igmp_inst_max_resp_time_cmd,
	"no max-response-time",
	"No igmp max response time command\n"
	"Igmp max response time\n")
{
	struct igmp_instance *pinst = NULL;
	uint16_t resp_time = IGMP_RESPONSE_DEFAULT_TIME;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (resp_time != pinst->max_resp)
	{
		inst_max_resp_time_chg(pinst, resp_time);
	}
	
	return CMD_SUCCESS;
}

static void inst_send_alert_chg(struct igmp_instance *pinst, int enable)
{
	pinst->send_alert = enable;
}

DEFUN (igmp_inst_send_router_alert,
	igmp_inst_send_router_alert_cmd,
	"send-router-alter",
	"Igmp send router alter enable command\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (!pinst->send_alert)
	{
		inst_send_alert_chg(pinst, 1);
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_send_router_alert,
	no_igmp_inst_send_router_alert_cmd,
	"no send-router-alter",
	"No igmp send router alter enable command\n"
	"Igmp send router alter\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (pinst->send_alert)
	{
		inst_send_alert_chg(pinst, 0);
	}
	
	return CMD_SUCCESS;
}

static void inst_req_alert_chg(struct igmp_instance *pinst, int enable)
{
	pinst->require_alert = enable;
}

DEFUN (igmp_inst_require_router_alert,
	igmp_inst_require_router_alert_cmd,
	"require-router-alter",
	"Igmp require router alter enable command\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (!pinst->require_alert)
	{
		inst_req_alert_chg(pinst, 1);
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_inst_require_router_alert,
	no_igmp_inst_require_router_alert_cmd,
	"no require-router-alter",
	"No igmp require router alter enable command\n"
	"Igmp require router alter\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t vpn;

	/*check igmp instance wether exsit*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (pinst->require_alert)
	{
		inst_req_alert_chg(pinst, 0);
	}
	
	return CMD_SUCCESS;
}

DEFUN (igmp_ssm_mapping_group,
	igmp_ssm_mapping_group_cmd,
	"igmp ssm-mapping group A.B.C.D/<4-32> source A.B.C.D",
	"Igmp ssm-mapping group command\n"
	"Igmp ssm-mapping\n"
	"Igmp ssm-mapping group\n"
	"Igmp ssm-mapping group ip and mask\n"
	"Igmp ssm-mapping source\n"
	"Igmp ssm-mapping source ip\n")
{
	struct igmp_instance *pinst = NULL;
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	struct igmp_ssm_map gmap;
	uint8_t ret = 0;
	uint8_t vpn;

	/*get group ip address and length of ip address mask*/
	VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
	gmap.grpip = ntohl(gipv4.prefix.s_addr);
	gmap.prefix = gipv4.prefixlen;
	if(gmap.prefix < 4 || gmap.prefix > 32)
	{
        vty_out (vty, "The mask is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!ipv4_is_multicast(gmap.grpip))
	{
		vty_out (vty, "The multicast address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get source ip address*/
	VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
	gmap.sip = ntohl(sipv4.prefix.s_addr);

	/*get vpn info*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	/*add ssm-map info to igmp instance */
	ret = igmp_instance_ssm_map_add(pinst, &gmap);
	if (ret)
	{
		vty_out(vty, "Igmp instance add group ip-address info error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_ssm_mapping_group,
	no_igmp_ssm_mapping_group_cmd,
	"no igmp ssm-mapping group A.B.C.D/<4-32> source A.B.C.D",
	"No igmp ssm-mapping group command\n"
	"Igmp\n"
	"Igmp ssm-mapping\n"
	"Igmp ssm-mapping group\n"
	"Igmp ssm-mapping group ip and mask\n"
	"Igmp ssm-mapping source\n"
	"Igmp ssm-mapping source ip\n")

{
	struct igmp_instance *pinst = NULL;
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	struct igmp_ssm_map gmap;
	uint8_t ret = 0;
	uint8_t vpn;

	/*get group ip address and length of ip address mask*/
	VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
	gmap.grpip = ntohl(gipv4.prefix.s_addr);
	gmap.prefix = gipv4.prefixlen;
	if(gmap.prefix < 4 || gmap.prefix > 32)
	{
        vty_out (vty, "The mask is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!ipv4_is_multicast(gmap.grpip))
	{
		vty_out (vty, "The multicast address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get source ip address*/
	VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
	gmap.sip = ntohl(sipv4.prefix.s_addr);

	/*get vpn info*/
	vpn = (uint8_t)vty->index;
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exsit%s", vpn, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	/*delete ssm-map info from igmp instance */
	igmp_instance_ssm_map_del(pinst, &gmap);
	
	return CMD_SUCCESS;
}

/*igmp interface command*/
static int igmp_enable(struct ipmc_if *pif, int enable)
{
	if (NULL == pif)
	{
		return -1;
	}

	if (IGMP_FLAG_ENABLE == enable)
	{
		if (NULL == pif->igmp)
		{
			pif->igmp = igmp_new(pif);
			if (pif->igmp)
			{
				/*igmp_startup_mode_on() will reset QQI*/
				igmp_startup_mode_on(pif->igmp);

				/*set up query timer*/
				igmp_general_query_timer_start(pif->igmp);
			}
		}
	}
	else if (IGMP_FLAG_DISABLE == enable)
	{
		if (NULL != pif->igmp)
		{
			igmp_delete(pif->igmp);
		}
	}

	return 0;
}

DEFUN (igmp_enable_instance,
	igmp_enable_instance_cmd,
	"igmp enable",
	"Igmp instance enable command\n"
	"Igmp enable\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*add mcif to igmp instance*/
	igmp_instance_mcif_get(vpn, ifindex);
	if (IGMP_FLAG_ENABLE == pif->igmp_flag)
	{
		vty_out(vty, "Igmp is already enable%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	ret = igmp_enable(pif, IGMP_FLAG_ENABLE);
	if (ret)
	{
		igmp_instance_mcif_del(vpn, ifindex);
		
		vty_out(vty, "Igmp enable failure%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pif->igmp_flag = IGMP_FLAG_ENABLE;
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_enable_instance,
	no_igmp_enable_instance_cmd,
	"igmp disable",
	"Igmp disable command\n"
	"Igmp disable\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pmcif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;
	
	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	if (IGMP_FLAG_DISABLE == pmcif->igmp_flag)
	{
		vty_out(vty, "Igmp is already enable%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}
	
	ret = igmp_enable(pmcif, IGMP_FLAG_DISABLE);
	if (ret)
	{
		vty_out(vty, "Igmp disable failure%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*disable igmp*/
	pmcif->igmp_flag = IGMP_FLAG_DISABLE;

	/*delete mcif from igmp instance*/
	igmp_instance_mcif_del(vpn, ifindex);
	
	/*delete interface from mc*/
	if (pmcif->pim_mode == 0)
	{
		ipmc_if_delete(ifindex);
	}
	
	return CMD_SUCCESS;
}

static int igmp_version_chg(struct ipmc_if *pif, uint8_t version)
{
	struct igmp *igmp = NULL;
//	uint32_t grpip;
//	uint32_t dip;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return CMD_WARNING;
	}

	if (version != igmp->version)
	{
		igmp->version = version;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_VERSION);

		/*change interface igmp processing*/
		igmp_startup_mode_on(igmp);
	
#if 0
		/*send first query packet*/
		dip = IGMP_GENERAL_QUERY_IPADDRESS;
	  	grpip = 0;
		igmp_send_membership_query(igmp, 0 /* igmp_group */,NULL, /*src ip*/
				 0 /* num_sources */, dip, grpip, igmp->send_alert/* s_flag: always set for general queries */);
		if (igmp->start_qry_cnt > 0)
		{
			igmp->start_qry_cnt--;
		}
#endif
		/*set up query timer*/	
		inst_query_reschedule(igmp);
	}

	return CMD_SUCCESS;
}

DEFUN (igmp_version,
	igmp_version_cmd,
	"igmp version (1|2|3)",
	"Igmp version command\n"
	"Igmp version\n"
	"Igmp version1\n"
	"Igmp version2\n"
	"Igmp version3\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t version;
	uint8_t vpn;
	int ret;

	/*check version wether invalid*/
	version = atoi(argv[0]);
	if ((version != IGMP_VERSION1) && 
		(version != IGMP_VERSION2) &&
		(version != IGMP_VERSION3))
	{
		vty_out(vty, "Igmp version %d is invalid%s", version, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	ret = igmp_version_chg(pif, version);
	if (ret)
	{
		vty_out(vty, "Igmp version is set error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	return CMD_SUCCESS;
}

DEFUN (no_igmp_version,
	no_igmp_version_cmd,
	"no igmp version",
	"No igmp version command\n"
	"Igmp\n"
	"Igmp version\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	ret = igmp_version_chg(pif, IGMP_VERSION3);
	if (ret)
	{
		vty_out(vty, "Igmp version is set error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	return CMD_SUCCESS;
}

DEFUN (igmp_port_static_group,
	igmp_port_static_group_cmd,
	"igmp join group A.B.C.D {source A.B.C.D}",
	"Igmp port static group command\n"
	"Igmp join\n"
	"Igmp group\n"
	"Igmp multicast group ip\n"
	"Igmp source\n"
	"Igmp source ip\n")
{
	struct igmp_instance *pinst = NULL;
	struct igmp_join *pjion = NULL;
	struct mcgroup_t *grp = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	struct mcip_t *pip = NULL;
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	struct igmp_join join;
	struct mcip_t mcip;
	uint32_t group_id;
	uint32_t ifindex;
	uint8_t ret = 0;
	int vpn;

	/*get group ip address and length of ip address mask*/
	VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
	join.grpip = ntohl(gipv4.prefix.s_addr);

	/*get source ip address*/
	if(argv[1] != NULL)
	{
		VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
		join.sip = ntohl(sipv4.prefix.s_addr);
	}
	else
	{
		join.sip = 0;
	}

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		vty_out(vty, "Cannot find multicast interface%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (0 == pmcif->igmp_flag)
	{
		vty_out(vty, "Igmp on interface does not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*look up interface vpn instance*/
	vpn = igmp_instance_find(ifindex);
	if (vpn < 0)
	{
		vty_out(vty, "Interface is not add to any igmp instance%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	/*check wether igmp vpn instance exsit*/
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}
	/*use*/
	pigmp = pmcif->igmp;

	/*if pim ssm is not enable, must querier can support this command*/
	if (pmcif->pim_mode != 1)
	{
		if (NULL == pigmp->query_timer)
		{
			vty_out(vty, "This device role is not querier%s", VTY_NEWLINE);
		
			return CMD_WARNING;
		}
	}
	
	mcip.dip = join.grpip;
	mcip.sip = join.sip;
	mcip.vpnid = vpn;
	mcip.source_port_nocheck = 1;

	printf("dip :%d:%d:%d:%d\n", mcip.dip>>24 & 0xff, mcip.dip>>16 & 0xff, mcip.dip>>8 & 0xff, mcip.dip & 0xff);

	pip = mc_ip_lookup(&mcip);
	if (NULL == pip)
	{
		/*add mcip to mc_ip_table*/
		/*check mc_ip_table to lookup group id*/
		group_id = index_alloc(INDEX_TYPE_IPMC);
		if (group_id == 0)
		{
			vty_out(vty, "Alloc group id failed%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
		
    	ret = mc_group_create(group_id);
		if (ret)
		{
			index_free(INDEX_TYPE_IPMC, group_id);

			vty_out(vty, "Create mutilcast group %d failed%s", group_id, VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
		
		/*save group id*/
		mcip.group_id = group_id;

		/*add to mc ip table*/
		mc_ip_add(&mcip);

		pip = mc_ip_lookup(&mcip);
	}
		
	/*lookup group id struct wether in mc_ip_table*/
	grp = mc_group_lookup(pip->group_id);
	if (NULL == grp)
	{
		vty_out(vty, "Can not find groupid in mc group table%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	/*add mcip to mc_group_table */
	ret = mc_group_add_if(grp, ifindex);
	if (ret)
	{
		vty_out(vty, "Igmp add group ip-address info to mc_ip_table error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	/*add to igmp struct*/
	if (NULL == pigmp)
	{
		vty_out(vty, "Igmp struct on interface %d error%s", pmcif->ifindex, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	igmp_join_add(pigmp, &join);
	/*配置静态组*/
	if(pim_instance_global->pim_conf.ssm_state == PIM_SSM_ENABLE && IN_PIM_SSM_RANGE(join.grpip))
	{
		vty_out(vty, "ifindex:%x, (%x, %x) %s", ifindex, join.sip, join.grpip, VTY_NEWLINE);
		pim_ssm_sg_add(ifindex, join.sip, join.grpip);
	}
	else
	{
		/*配置一个(*,G)*/
		pim_mrt_igmp_mem_add(pmcif, join.grpip);
	}
	return CMD_SUCCESS;
}

DEFUN (no_igmp_port_static_group,
	no_igmp_port_static_group_cmd,
	"no igmp join group A.B.C.D {source A.B.C.D}",
	"No igmp port static group command\n"
	"Igmp\n"
	"Igmp join\n"
	"Igmp group\n"
	"Igmp multicast group ip and mask length\n"
	"Igmp source\n"
	"Igmp source ip\n")
{
	struct igmp_instance *pinst = NULL;
	struct mcgroup_t *grp = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	struct mcip_t *pip = NULL;
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	struct igmp_join join;
	struct mcip_t mcip;
	uint32_t ifindex;
	uint8_t ret = 0;
	int vpn;

	/*get group ip address and length of ip address mask*/
	VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
	join.grpip = ntohl(gipv4.prefix.s_addr);

	/*get source ip address*/
	if(argv[1] != NULL)
	{
		VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
		join.sip = ntohl(sipv4.prefix.s_addr);
	}
	else
	{
		join.sip = 0;
	}

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	if (0 == pmcif->igmp_flag)
	{
		vty_out(vty, "Igmp on interface does not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*vpn check*/
	vpn = igmp_instance_find(ifindex);
	if (vpn < 0)
	{
		vty_out(vty, "Interface is not add to any igmp instance%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*get igmp vpn instance*/
	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	mcip.dip = join.grpip;
	mcip.sip = join.sip;
	mcip.vpnid = vpn;

	printf("dip :%d:%d:%d:%d\n", mcip.dip>>24 & 0xff, mcip.dip>>16 & 0xff, mcip.dip>>8 & 0xff, mcip.dip & 0xff);
	pim_ssm_sg_del( ifindex, join.sip, join.grpip);

	pip = mc_ip_lookup(&mcip);
	if (NULL == pip)
	{
		vty_out(vty, "Can not find mc ip in mc ip table%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	/*lookup group id struct wether in mc_ip_table*/
	grp = mc_group_lookup(pip->group_id);
	if (NULL == grp)
	{
		vty_out(vty, "Can not find mc group%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	/*delete mcip from mc_group_table */
	ret = mc_group_delete_if(grp, ifindex);
	if (ret)
	{
		vty_out(vty, "Interface delete group address error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*delete from igmp struct*/
	pigmp = pmcif->igmp;
	if (NULL == pigmp)
	{
		vty_out(vty, "Igmp struct on interface %d error%s", pmcif->ifindex, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	igmp_join_delete(pigmp, &join);

	/*check wether mcip used by other interface, if not delete it*/
	if (grp->iflist.count < 1)
	{
		mc_group_delete(pip->group_id);
	
		mcip.group_id = pip->group_id;
		mc_ip_delete(&mcip);
	}
	
	return CMD_SUCCESS;
}

static int igmp_req_alert_chg(struct ipmc_if *pif, int enable)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return CMD_WARNING;
	}

	if (enable != igmp->req_alert)
	{
		igmp->req_alert = enable;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_REQ_ALERT);
	}

	return CMD_SUCCESS;
}

DEFUN (igmp_require_router_alert,
	igmp_require_router_alert_cmd,
	"igmp require-router-alertr",
	"Igmp require router alert enable command\n"
	"Igmp require router alert\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	ret = igmp_req_alert_chg(pif, 1);
	if (ret)
	{
		vty_out(vty, "Igmp require-alert is set error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	return CMD_SUCCESS;
}

DEFUN (no_igmp_require_router_alert,
	no_igmp_require_router_alert_cmd,
	"no igmp require-router-alter",
	"No igmp require router alter enable command\n"
	"Igmp require router alter\n"
	"Require router alter\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	ret = igmp_req_alert_chg(pif, 0);
	if (ret)
	{
		vty_out(vty, "Igmp require-alert is set error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
		
	return CMD_SUCCESS;
}

static void igmp_send_alert_chg(struct ipmc_if *pif, int enable)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return;
	}

	if (enable != igmp->send_alert)
	{
		igmp->send_alert = enable;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_SND_ALERT);
	}
}

DEFUN (igmp_send_router_alert,
	igmp_send_router_alert_cmd,
	"igmp send-router-alter",
	"Igmp send router alter enable command\n"
	"Igmp send router alter\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	
	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	igmp_send_alert_chg(pif, 1);
		
	return CMD_SUCCESS;
}

DEFUN (no_igmp_send_router_alert,
	no_igmp_send_router_alert_cmd,
	"no igmp send-router-alter",
	"No igmp send router alter enable command\n"
	"Igmp send router alter\n"
	"Send router alter\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	igmp_send_alert_chg(pif, 0);

	return CMD_SUCCESS;
}

#if 0
DEFUN (igmp_static_group,
	igmp_static_group_cmd,
	"igmp group A.B.C.D/<1-32> {source A.B.C.D}",
	"Igmp static group command\n"
	"Igmp group\n"
	"Igmp multicast group ip and mask\n"
	"Igmp source\n"
	"Igmp source ip\n")
{
	struct hash_bucket *bucket = NULL;
	struct listnode *node = NULL;
	struct mcip_t *pip = NULL;
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	uint32_t group_id  = 0;
	struct igmp_join grp;
	struct in_addr mask;
	struct mcip_t mcip;
	uint8_t ret = 0;
	int cursor = 0;
	uint32_t addr1;
	uint8_t vpn;
	int len;
	int i;

	/*get group ip address and length of ip address mask*/
	VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
	grp.grpip = ntohl(gipv4.prefix.s_addr);
	grp.prefixlen = gipv4.prefixlen;

	/*get source ip address*/
	grp.sip = 0;
	if (NULL != argv[1])
	{
		VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
		grp.sip = ntohl(sipv4.prefix.s_addr);
		//printf("sip :%d:%d:%d:%d\n", grp.sip>>24 & 0xff, grp.sip>>16 & 0xff, grp.sip>>8 & 0xff, grp.sip & 0xff);
	}

	/*get vpn info*/
	vpn = (uint8_t)vty->index;
	
	/*add mcip to igmp instance */
	ret = igmp_instance_mip_add(vpn, &grp);
	if (ret)
	{
		vty_out(vty, "Igmp instence add group ip-address info error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*save to mc ip table*/
	mcip.sip = grp.sip;
	mcip.vpnid = vpn;

	len = 32 - grp.prefixlen;
	if (2 << len > IPMC_MGROUP_NUM+1)
	{
		vty_out(vty, "Igmp max group number is 1024%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	addr1 = grp.grpip;
	masklen2ip(grp.prefixlen, &mask);
	mask.s_addr = ntohl(mask.s_addr);
	addr1 &= mask.s_addr;
	
	for (i = 1; i < (2 << len); i++)
	{
		mcip.dip = addr1;
		printf("dip :%d:%d:%d:%d\n", mcip.dip>>24 & 0xff, mcip.dip>>16 & 0xff, mcip.dip>>8 & 0xff, mcip.dip & 0xff);

		pip = mc_ip_lookup(&mcip);
		if (NULL != pip)
		{
			continue;
		}
		
		/*check mc_ip_table to lookup group id*/
		group_id = index_alloc(INDEX_TYPE_IPMC);
		if (group_id == 0)
		{
			vty_out(vty, "Alloc group id failed%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
		
		//printf("alloc group id:%d\n\r", group_id);
    	ret = mc_group_create(group_id);
		if (ret)
		{
			index_free(INDEX_TYPE_IPMC, group_id);

			vty_out(vty, "Create mutilcast group %d failed%s", group_id, VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
		
		/*save group id*/
		mcip.group_id = group_id;

		/*add to mc ip table*/
		mc_ip_add(&mcip);
		
		addr1++;
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_static_group,
	no_igmp_static_group_cmd,
	"no igmp group A.B.C.D/<1-32> {source A.B.C.D}",
	"No igmp static command\n"
	"Igmp static group\n"
	"Igmp group\n"
	"Igmp multicast group ip and mask\n"
	"Igmp source\n"
	"Igmp source ip\n")
{
	struct mcip_t *pip = NULL;
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	struct igmp_join grp;
	struct in_addr mask;
	struct mcip_t mcip; 
	uint16_t group_id;
	uint8_t ret = 0;
	uint32_t addr1;
	uint8_t vpn;
	int len;
	int i;

	/*get group ip address and length of ip address mask*/
	VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
	grp.grpip = ntohl(gipv4.prefix.s_addr);
	grp.prefixlen = gipv4.prefixlen;

	/*get source ip address*/
	grp.sip = 0;
	if (NULL != argv[1])
	{
		VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
		grp.sip = ntohl(sipv4.prefix.s_addr);
	}

	/*get vpn info*/
	vpn = (uint8_t)vty->index;

	/*delete mcip from interface*/
	mcip.sip = grp.sip;
	mcip.vpnid = vpn;

	len = 32 - grp.prefixlen;
	if (2 << len > IPMC_MGROUP_NUM+1)
	{
		vty_out(vty, "Igmp max group number is 1024%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	addr1 = grp.grpip;
	masklen2ip(grp.prefixlen, &mask);
	mask.s_addr = ntohl(mask.s_addr);
	addr1 &= mask.s_addr;

	for (i = 1; i < (2 << len); i++)
	{	
		mcip.dip = addr1;
		printf("dip :%d:%d:%d:%d\n", mcip.dip>>24 & 0xff, mcip.dip>>16 & 0xff, mcip.dip>>8 & 0xff, mcip.dip & 0xff);
		pip = mc_ip_lookup(&mcip);
		if (NULL == pip)
		{
			continue;
		}
		
		/*delete mcgroup from mc_group_table */
		mc_group_delete(pip->group_id);
	
		mcip.group_id = pip->group_id;
		mc_ip_delete(&mcip);

		addr1++;
	}
	
	/*delete group ip info from igmp instance */
	ret = igmp_instance_mip_delete(vpn, &grp);
	if (ret)
	{
		vty_out(vty, "Igmp instence add group ip-address info error%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}
#endif

static void query_reschedule(struct igmp *pigmp)
{
	if (pigmp->query_timer) 
	{
		if (pigmp->other_querier_timer)
		{
			return;
		}

    	//igmp_general_query_off(pigmp);
    	//igmp_general_query_on(pigmp);
		
		igmp_general_query_timer_stop(pigmp);
		igmp_general_query_timer_start(pigmp);
  	}
  	else 
	{
		/* other querier present */
    	if (NULL == pigmp->other_querier_timer)
		{
			return;
		}

    	//igmp_other_querier_timer_off(pigmp);
    	//igmp_other_querier_timer_on(pigmp);
    	
		igmp_other_querier_timer_stop(pigmp);
		igmp_other_querier_timer_start(pigmp);
  	}
}

static int querier_keepalive_chg(struct ipmc_if *pif, uint16_t keepalive)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return CMD_WARNING;
	}

	if (keepalive != igmp->keepalive)
	{
		igmp->keepalive = keepalive;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_OTHER_QUERIER_ALIVE);

		igmp_startup_mode_on(igmp);

		/*set up other query timer*/	
		inst_query_reschedule(igmp);
	}

	return CMD_SUCCESS;
}

DEFUN (igmp_other_querier_keepalive,
	igmp_other_querier_keepalive_cmd,
	"igmp other-querier keepalive <60-300>",
	"Igmp querier keepalive command\n"
	"Igmp other-querier\n"
	"Igmp other-querier keepalive\n"
	"Igmp other-querier keepalive time\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint16_t keepalive;
	uint32_t ifindex;
	uint8_t vpn;

	/*check keepalive wether invalid*/
	keepalive = atoi(argv[0]);
	if ((keepalive > IGMP_QUARY_MAX_KEEPALIVE) || 
		(keepalive < IGMP_QUARY_MIN_KEEPALIVE))
	{
		vty_out(vty, "Igmp keepalive %d is invalid%s", keepalive, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if (vpn > IGMP_INS_MAX)
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	querier_keepalive_chg(pif, keepalive);
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_other_querier_keepalive,
	no_igmp_other_querier_keepalive_cmd,
	"no igmp other-querier keepalive",
	"No igmp other-querier keepalive command\n"
	"Igmp other-querier keepalive config\n"
	"Igmp other-querier\n"
	"Igmp other-querier keepalive\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;

	/*check multicast interface*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if (vpn > IGMP_INS_MAX)
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	querier_keepalive_chg(pif, IGMP_QUARY_DEFAULT_KEEPALIVE);
	
	return CMD_SUCCESS;
}

static void query_interval_chg(struct ipmc_if *pif, int query_interval)
{
	struct igmp *pigmp = NULL;
//	uint32_t grpip;
//	uint32_t dip;

	pigmp = pif->igmp;
	if (NULL != pigmp)
	{
		if (query_interval == pigmp->qqi)
		{
			return;
		}

		IGMP_FLAG_CONFIG_SET(pigmp->cfg_flag, IGMP_FLAG_CONFIG_QUERY);
		
		igmp_startup_mode_on(pigmp);
		
#if 0
		/*send first query packet*/
		dip = IGMP_GENERAL_QUERY_IPADDRESS;
	  	grpip = 0;
		igmp_send_membership_query(pigmp, 0 /* igmp_group */,NULL, /*src ip*/
				 0 /* num_sources */, dip, grpip, pigmp->send_alert/* s_flag: always set for general queries */);
		if (pigmp->start_qry_cnt > 0)
		{
			pigmp->start_qry_cnt--;
		}
#endif
		/*set up query timer*/	
		inst_query_reschedule(pigmp);
	}
}

DEFUN (igmp_query_interval,
	igmp_query_interval_cmd,
	"igmp timer query <1-18000>",
	"Igmp query interval command\n"
	"Igmp query timer\n"
	"Igmp query interval\n"
	"Igmp query interval seconds:1-18000\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint8_t query_interval;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check query  interval wether invalid*/
	query_interval = atoi(argv[0]);
	if ((query_interval < IGMP_QUERY_MIN_INTERVAL) || 
		(query_interval > IGMP_QUERY_MAX_INTERVAL))
	{
		vty_out(vty, "Igmp query interval time %d is invalid%s", query_interval, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	query_interval_chg(pif, query_interval);
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_query_interval,
	no_igmp_query_interval_cmd,
	"no igmp timer query",
	"No igmp query interval command\n"
	"Igmp query interval config\n"
	"Igmp timer\n"
	"Igmp timer query\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint8_t query_interval;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	query_interval_chg(pif, IGMP_QUERY_DEFAULT_INTERVAL);
}

static void robust_count_chg(struct  ipmc_if *pif,  uint8_t count)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return;
	}

	if (count != igmp->qrv)
	{
		igmp->qrv = count;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_ROBUST_CNT);
	}
}

DEFUN (igmp_robust_count,
	igmp_robust_count_cmd,
	"igmp robust-count <2-5>",
	"Igmp robust count command\n"
	"Igmp robust-count\n"
	"Igmp robust-count number:2-5\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint8_t robust_count;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check query  interval wether invalid*/
	robust_count = atoi(argv[0]);
	if ((robust_count < IGMP_ROBUST_MIN_COUNT) || 
		(robust_count > IGMP_ROBUST_MAX_COUNT))
	{
		vty_out(vty, "Igmp robust count %d is invalid%s", robust_count, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	robust_count_chg(pif, robust_count);
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_robust_count,
	no_igmp_robust_count_cmd,
	"no igmp roubust-count",
	"No igmp roubust count command\n"
	"Igmp roubust-count config\n"
	"Igmp roubust-count\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	robust_count_chg(pif, IGMP_ROBUST_DEFAULT_COUNT);
	
	return CMD_SUCCESS;
}

static void max_resp_time_chg(struct ipmc_if *pif, int resp_time)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return;
	}

	if (resp_time != igmp->max_resp_dsec)
	{
		igmp->max_resp_dsec = resp_time;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_MAX_RESP);
	}
}

DEFUN (igmp_max_resp_time,
	igmp_max_resp_time_cmd,
	"igmp max-response-time <1-25>",
	"Igmp max response time command\n"
	"Igmp max response time\n"
	"Igmp max response time seconds:1-25\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint8_t max_resp;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check query  interval wether invalid*/
	max_resp = atoi(argv[0]);
	if ((max_resp < IGMP_RESPONSE_MIN_TIME) || 
		(max_resp > IGMP_RESPONSE_MAX_TIME))
	{
		vty_out(vty, "Igmp max response time %d is invalid%s", max_resp, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	max_resp_time_chg(pif, max_resp);
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_max_resp_time,
	no_igmp_max_resp_time_cmd,
	"no igmp max-response-time",
	"No igmp max response time command\n"
	"Igmp configure\n"
	"Igmp max response time\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	max_resp_time_chg(pif, IGMP_RESPONSE_DEFAULT_TIME);
	
	return CMD_SUCCESS;
}

static void lastmember_query_interval_chg(struct ipmc_if *pif, int query_interval)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return;
	}

	if (query_interval != igmp->spec_max_resp_dsec)
	{
		igmp->spec_max_resp_dsec = query_interval;
		
		IGMP_FLAG_CONFIG_SET(igmp->cfg_flag, IGMP_FLAG_CONFIG_LASTMEM_QUERY);
	}
}

DEFUN (igmp_last_member_query_interval,
	igmp_last_member_query_interval_cmd,
	"igmp last member-query interval <1-5>",
	"Igmp last member query interval command\n"
	"Igmp last member\n"
	"Igmp last member query\n"
	"Igmp last member query interval\n"
	"Igmp last member query interval seconds:1-5\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint8_t query_interval;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check query  interval wether invalid*/
	query_interval = atoi(argv[0]);
	if ((query_interval < IGMP_LAST_MEMBER_QUERY_MIN_INTERVAL) || 
		(query_interval > IGMP_LAST_MEMBER_QUERY_MAX_INTERVAL))
	{
		vty_out(vty, "Igmp last member query interval %d is invalid%s", query_interval, VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	lastmember_query_interval_chg(pif, query_interval);
	
	return CMD_SUCCESS;
}

DEFUN (no_igmp_last_member_query_interval,
	no_igmp_last_member_query_interval_cmd,
	"no igmp last member-query interval",
	"No igmp last member query interval command\n"
	"Igmp configure\n"
	"Igmp last member\n"
	"Igmp last member query\n"
	"Igmp last member query interval\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	lastmember_query_interval_chg(pif, IGMP_LAST_MEMBER_QUERY_DEFAULT_INTERVAL);
	
	return CMD_SUCCESS;
}

static void fast_leave_enable(struct ipmc_if *pif, uint8_t en)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return;
	}

	if (en != igmp->fast_leave)
	{
		igmp->fast_leave = en;
	}
}

DEFUN(igmp_fast_leave,
	igmp_fast_leave_cmd,
	"igmp fast-leave",
	"Igmp fast leave command\n"
	"Igmp fast-leave\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	fast_leave_enable(pif, 1);
	
	return CMD_SUCCESS;
}

DEFUN(no_igmp_fast_leave,
	no_igmp_fast_leave_cmd,
	"no igmp fast-leave",
	"No igmp fast leave command\n"
	"Igmp fast-leave config\n"
	"Igmp fast-leave\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	fast_leave_enable(pif, 0);
	
	return CMD_SUCCESS;
}

static void ssm_mapping_enable(struct ipmc_if *pif, uint8_t en)
{
	struct igmp *igmp = NULL;

	igmp = pif->igmp;
	if (NULL == igmp)
	{
		return;
	}

	if (en != igmp->ssm_map_enable)
	{
		igmp->ssm_map_enable = en;
	}
}

DEFUN(igmp_ssm_mapping,
	igmp_ssm_mapping_cmd,
	"igmp ssm-mapping enable",
	"Igmp ssm-mapping command\n"
	"Igmp ssm-mapping\n"
	"Ssm mapping enable\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	ssm_mapping_enable(pif, 1);
	
	return CMD_SUCCESS;
}

DEFUN(no_igmp_ssm_mapping,
	no_igmp_ssm_mapping_cmd,
	"no igmp ssm-mapping enable",
	"No igmp ssm-mapping command\n"
	"Igmp configure\n"
	"Igmp ssm-mapping\n"
	"Ssm mapping enable\n")
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t ifindex;
	uint8_t vpn;
	int ret;

	/*check igmp instance wether exsit*/
	ifindex = (uint32_t)vty->index;
	pif = ipmc_if_lookup(ifindex);
	if (NULL == pif)
	{
		pif = ipmc_if_add(ifindex);
		if (NULL == pif)
		{
			vty_out(vty, "Cannot found multicast interface%s", VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
	}

	/*check instance whether exisit*/
	vpn = if_vpn_get(ifindex);
	if ((vpn < IGMP_INS_MIN - 1) || (vpn > IGMP_INS_MAX))
	{
		vty_out(vty, "Igmp instance-id ranges is %d and %d%s",
	    	IGMP_INS_MIN, IGMP_INS_MAX, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "Igmp instance %d is not exist%s", vpn, VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/*check wether igmp enable*/
	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
    	return CMD_WARNING;
	}

	ssm_mapping_enable(pif, 0);
	
	return CMD_SUCCESS;
}

DEFUN(debug_igmp_packet,
	debug_igmp_packet_cmd,
	"debug igmp packet {recv|send}",
	"Debug igmp packet command\n"
	"Igmp\n"
	"Igmp packet\n"
	"Igmp packet receive\n"
	"Igmp packet send\n")
{
	int enable = 0;
    unsigned int zlog_num = 0;

	if ((argv[0] == NULL) && (argv[1] == NULL))
	{
		igmp_pkt_dbg.pkt_rsv = 1;
		igmp_pkt_dbg.pkt_snd = 1;

		enable = 1;
		zlog_num = 2;
	}

	if (argv[0] != NULL)
	{
		if (strncmp(argv[0], "recv", 4) == 0)
        {
            igmp_pkt_dbg.pkt_rsv = 1;

			enable = 1;
			zlog_num = 0;
        }
	}

	if (argv[1] != NULL)
	{
		if (strncmp(argv[1], "send", 4) == 0)
		{
			igmp_pkt_dbg.pkt_snd = 1;

			enable = 1;
			zlog_num = 1;
		}
	}

	zlog_debug_set( vty, igmp_dbg_name[zlog_num].key, enable);

	return CMD_SUCCESS;
}

DEFUN(no_debug_igmp_packet,
	no_debug_igmp_packet_cmd,
	"no debug igmp packet {recv|send}",
	"No debug igmp packet command\n"
	"Debug\n"
	"Igmp\n"
	"Igmp packet\n"
	"Igmp packet receive\n"
	"Igmp packet send\n")
{
	int enable = 0;
    unsigned int zlog_num = 0;
	
	if (argv[0] == NULL)
    {
    	igmp_pkt_dbg.pkt_rsv = 0;
        igmp_pkt_dbg.pkt_snd = 0;

		enable = 0;
		zlog_num = 2;
    }
    else
    {
        if (strncmp(argv[0], "recv", 4) == 0)
        {
            igmp_pkt_dbg.pkt_rsv = 0;

			enable = 0;
			zlog_num = 0;
        }
        else if (strncmp(argv[0], "send", 4) == 0)
        {
            igmp_pkt_dbg.pkt_snd = 0;

			enable = 0;
			zlog_num = 1;
        }
		else
		{
			vty_out (vty, "No debug typd find %s", VTY_NEWLINE);
		}
    }

	zlog_debug_set( vty, igmp_dbg_name[zlog_num].key, enable);
	
	return CMD_SUCCESS;
}

DEFUN(show_igmp_debug,
	show_igmp_debug_cmd,
	"show igmp debug",
	"Show igmp debug command\n"
	"Igmp\n"
	"Igmp debug info\n")
{
	int type_num;
			
	vty_out(vty, "debug type		 status %s", VTY_NEWLINE);
	
	for(type_num = 0; type_num < array_size(igmp_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s	 %-10s %s", igmp_dbg_name[type_num].str, 
					   !!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}
	
	return CMD_SUCCESS;
}


static void igmp_group_info_show(struct vty *vty, uint32_t gip, uint32_t sip)
{
	struct igmp_instance *pinst = NULL;
	int i;
	
	vty_out(vty, "----IGMP group information----%s", VTY_NEWLINE);

	for (i = 0; i < IGMP_INS_MAX + 1; i++)
	{
		pinst = igmp_instance_lookup(i);
		if (NULL != pinst)
		{
			if (pinst->version)
			{
				vty_out(vty, "IGMP Instance: %d%s", i, VTY_NEWLINE);
				
				/*src ip, grp ip/mask, outinterface info*/
				if (gip)
				{
					;
				}
				else if (sip)
				{
					;
				}
				else
				{
				}
			}
		}
	}
}

DEFUN(show_igmp_group,
	show_igmp_group_cmd,
	"show igmp group {A.B.C.D |source A.B.C.D}",
	"Show igmp group command\n"
	"Igmp\n"
	"Igmp group\n"
	"Igmp group ip address\n"
	"Igmp source\n"
	"Igmp source ip address\n")
{
	struct prefix_ipv4 gipv4;
	struct prefix_ipv4 sipv4;
	uint32_t gip = 0;
	uint32_t sip = 0;
		
	if ((NULL != argv[0]) && (NULL == argv[1]))
	{
		/*get group ip address and length of ip address mask*/
		VTY_GET_IPV4_PREFIX ("group ip-address", gipv4, argv[0]);
		gip = ntohl(gipv4.prefix.s_addr);
	}
	else if ((NULL == argv[0]) && (NULL != argv[1]))
	{
		/*get source ip address and length of ip address mask*/
		VTY_GET_IPV4_PREFIX ("source ip-address", sipv4, argv[1]);
		sip = ntohl(gipv4.prefix.s_addr);
	}

	igmp_group_info_show(vty, gip, sip);
	
	return CMD_SUCCESS;
}

DEFUN(show_igmp_routing_table,
	show_igmp_routing_table_cmd,
	"show igmp routing-table",
	"Show igmp routing table command\n"
	"Igmp\n"
	"Igmp routing table\n")
{
	struct hash_bucket *pbucket = NULL;
	struct igmp_instance *pinst = NULL;
	struct mcgroup_t *pgrp = NULL;
	struct listnode *node = NULL;
	struct mcip_t *pip = NULL;
	uint32_t *pifindex = NULL;
	char ifname[IFNET_NAMESIZE];
	char ipv4_addr1[20];
	char ipv4_addr2[20];
	int cursor = 0;
	int vpn = 0;  //public vpn instance
	int i = 1;

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		vty_out(vty, "IGMP VPN instance public net not exist%s", VTY_NEWLINE);
			
	    return CMD_WARNING;
	}

	if (NULL != pinst->mcif)
	{
		if (!list_isempty(pinst->mcif))
		{	
			vty_out(vty, "Routing table of VPN-Instance: public net%s", VTY_NEWLINE);

			if (mc_ip_table.num_entries > 0)
			{
				vty_out(vty, "Total %u: entry%s", mc_ip_table.num_entries, VTY_NEWLINE);
				vty_out(vty, "%s", VTY_NEWLINE);
			}

			HASH_BUCKET_LOOP (pbucket, cursor, mc_ip_table)
			{
				if(NULL != pbucket->data)
				{
					pip = (struct mcip_t *)pbucket->data;
					if (NULL == pip)
					{
						continue;
					}

					if (pip->vpnid == vpn)
					{
						inet_ipv4tostr(pip->dip, ipv4_addr1);
						if (pip->sip)
						{
							inet_ipv4tostr(pip->sip, ipv4_addr2);
							vty_out(vty, "%d. (%s, %s)%s", i, ipv4_addr2, ipv4_addr1, VTY_NEWLINE);
						}
						else
						{
							vty_out(vty, "%d. (*, %s)%s", i, ipv4_addr1, VTY_NEWLINE);
						}

						pgrp = mc_group_lookup(pip->group_id);
						if (NULL == pgrp)
						{
							continue;
						}

						vty_out(vty, "%4s List of %d down interface%s", "", 
								listcount(&(pgrp->iflist)), VTY_NEWLINE);

						for (ALL_LIST_ELEMENTS_RO(&(pgrp->iflist), node, pifindex))
						{
							if (NULL != pifindex)
							{
								ifm_get_name_by_ifindex(*pifindex, ifname);
	
								vty_out(vty, "%4s%2s%s", "", ifname, VTY_NEWLINE);
								vty_out(vty, "%8s%s%s", "", "Protocol: IGMP", VTY_NEWLINE);
							}
						}
						
						i++;
					}
				}
			}
		}
	}

	return CMD_SUCCESS;
}

static int igmp_if_statistics_show(struct vty *vty, struct ipmc_if *pif)
{
	struct igmp *pigmp;
	struct igmp_pkt_count *pcount;

	pigmp = pif->igmp;
	if (NULL == pigmp)
	{
		vty_out(vty, "Igmp struct is null%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	pcount = &(pigmp->count);
	vty_out(vty, "IGMP packet statistics:%s", VTY_NEWLINE);
	vty_out(vty, " Query:%s", VTY_NEWLINE);
	vty_out(vty, "  igmp v1 query packets received: %d%s", pcount->qry_v1_rsv_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v1 query packets sent: %d%s", pcount->qry_v1_snd_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v2 query packets received: %d%s", pcount->qry_v2_rsv_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v2 query packets sent: %d%s", pcount->qry_v2_snd_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 general query packets received: %d%s", pcount->qry_v3_rsv_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 general query packets sent: %d%s", pcount->qry_v3_snd_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 group specfic query packets received: %d%s", pcount->gqry_v3_rsv_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 group specfic query packets sent: %d%s", pcount->gqry_v3_snd_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 group source specfic query packets received: %d%s", pcount->gsqry_v3_rsv_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 group source specfic query packets sent: %d%s", pcount->gsqry_v3_snd_pkts, VTY_NEWLINE);
	vty_out(vty, " Report:%s", VTY_NEWLINE);
	vty_out(vty, "  igmp v1 report packets received: %d%s", pcount->rpt_v1_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v2 report packets received: %d%s", pcount->rpt_v2_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report packets received: %d%s", pcount->rpt_v3_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report isinclude packets received: %d%s", pcount->rpt_isin_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report isexclude packets received: %d%s", pcount->rpt_isexl_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report toinclude packets received: %d%s", pcount->rpt_toin_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report toexclude packets received: %d%s", pcount->rpt_toexl_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report allow packets received: %d%s", pcount->rpt_alw_pkts, VTY_NEWLINE);
	vty_out(vty, "  igmp v3 report block packets received: %d%s", pcount->rpt_blk_pkts, VTY_NEWLINE);
	vty_out(vty, " Leave:%s", VTY_NEWLINE);	
	vty_out(vty, "  igmp v2 leave packets received: %d%s", pcount->lev_v2_pkts, VTY_NEWLINE);
	vty_out(vty, " Invalid %s", VTY_NEWLINE);
	vty_out(vty, "  igmp invalid packets received: %d%s", pcount->invalid_pkts, VTY_NEWLINE);
}

DEFUN(show_igmp_statistics,
	show_igmp_statistics_cmd,
	"show igmp statistics interface ethernet USP",
	"Show igmp interface statistics command\n"
	"Igmp\n"
	"Igmp statistics\n"
	"interface\n"
	"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")
{
	uint32_t ifindex = 0;
    struct ipmc_if *pif = NULL;
    int ret = 0;

    ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *)argv[0]);
    
    pif = ipmc_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

	if (IGMP_FLAG_ENABLE != pif->igmp_flag)
	{
		vty_out(vty, "Igmp is not enable%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}
	
    ret = igmp_if_statistics_show(vty, pif);
    if(ret)
    {
    	return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

static void igmp_config_instance_show (struct vty *vty, uint8_t id)
{
	struct igmp_instance *pinst = NULL;
	struct igmp_ssm_map *pssm_map = NULL;
	struct listnode *node = NULL;
	char ifname[IFNET_NAMESIZE];
	uint32_t *pifindex = NULL;
	int curr = 0;
	int len = 0;
	char ipv4_addr[20];

	pinst = igmp_instance_lookup(id);

	vty_out(vty, "IGMP Instance: %d%s", id, VTY_NEWLINE);
	
	/*igmp version*/
	vty_out(vty, "IGMP version: %d%s", pinst->version, VTY_NEWLINE);

	/*igmp querier query interval*/
	vty_out(vty, "IGMP query interval: %d%s", pinst->qqi, VTY_NEWLINE);
	
	/*igmp other querier keepalive time*/
	vty_out(vty, "IGMP querier keepalive time: %d%s", pinst->keepalive, VTY_NEWLINE);

	/*igmp robust count */
	vty_out(vty, "IGMP robust count: %d%s", pinst->robust_count, VTY_NEWLINE);

	/*igmp member query interval*/
	vty_out(vty, "IGMP last member query interval: %u%s", pinst->last_mem_qqi, VTY_NEWLINE);

	/*igmp max response time*/
	vty_out(vty, "IGMP max response time: %u%s", pinst->max_resp, VTY_NEWLINE);

	/*igmp send router alter*/
	vty_out(vty, "IGMP send router alter: %s%s", pinst->send_alert?"Enable":"Disable", VTY_NEWLINE);

	/*igmp require-router-alter*/
	vty_out(vty, "IGMP require-router-alter: %s%s", pinst->require_alert?"Enable":"Disable", VTY_NEWLINE);

	/*igmp ssm-mapping group/source */
	if (pinst->ssm_map)
	{
		for (ALL_LIST_ELEMENTS_RO(pinst->ssm_map, node, pssm_map))
		{
			inet_ipv4tostr(pssm_map->grpip, ipv4_addr);
			vty_out(vty, "IGMP ssm-mapping: group %s/%u", ipv4_addr, pssm_map->prefix); 			
			if (pssm_map->sip)
			{
				inet_ipv4tostr(pssm_map->sip, ipv4_addr);
				vty_out(vty, " source %s", ipv4_addr);
			}
	
			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}

	/*igmp fast leave*/
	//vty_out(vty, "IGMP fast leave: %s%s", (pinst->fast_leave ? "enable" : "disable"), VTY_NEWLINE);

	/*igmp enable interfaces*/
	if ((NULL == pinst->mcif) || listcount(pinst->mcif) == 0)
	{
		vty_out(vty, "IGMP instance %d enable ports: NULL%s", id, VTY_NEWLINE);
	}
	else if (listcount(pinst->mcif) > 0)
	{
		vty_out(vty, "IGMP instance %d enable ports: %s", id, VTY_NEWLINE);
	}

		
	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex))
	{
		ifm_get_name_by_ifindex(*pifindex, ifname);
		len = strlen (ifname) + 1;
		if ((curr + len) <= 50)
		{
			vty_out(vty, "%s%2s", ifname, "");
			curr += len;
		}	
        else
        {
        	vty_out(vty, "%s", VTY_NEWLINE);
          	vty_out (vty, "%s%2s", ifname, "");
          	curr = len;
        }
	}

	vty_out(vty, "%s", VTY_NEWLINE);
	
	/*igmp group info*/
//	if ((NULL == pinst->grplist) || listcount(pinst->grplist) == 0)
//	{
//		vty_out(vty, "IGMP instance %d group information(G, S): NULL%s", id, VTY_NEWLINE);
//	}
//	else if (listcount(pinst->grplist) > 0)
//	{
//		vty_out(vty, "IGMP instance %d group information(G, S): %s", id, VTY_NEWLINE);
//	}
//	
//	for (ALL_LIST_ELEMENTS_RO(pinst->grplist, node, pjoin))
//	{
//		vty_out(vty, "(%d.%d.%d.%d/%d, %d.%d.%d.%d)%s", 
//			(pjoin->grpip >> 24 & 0xFF), (pjoin->grpip >> 16 & 0xFF), 
//			(pjoin->grpip >> 8 & 0xFF), (pjoin->grpip & 0xFF), pjoin->prefixlen, 
//			(pjoin->sip >> 24 & 0xFF), (pjoin->sip >> 16 & 0xFF), 
//			(pjoin->sip >> 8 & 0xFF), pjoin->sip & 0xFF, VTY_NEWLINE);
//	}

	vty_out(vty, "%s", VTY_NEWLINE);
}

static void igmp_config_instance_interface_show (struct vty *vty, uint8_t id, uint32_t ifindex)
{
	struct igmp_instance *pinst = NULL;
	struct listnode *node = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp_join *pjoin = NULL;
	char ifname[IFNET_NAMESIZE];
	uint32_t *pifindex = NULL;
	char ipv4_addr[20];

	pinst = igmp_instance_lookup(id);
	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex))
	{
		if(*pifindex != ifindex)
		{
			continue;
		}
		
		ifm_get_name_by_ifindex(*pifindex, ifname);		
		vty_out(vty, "----IGMP %s Config Information----%s", ifname, VTY_NEWLINE);	
		pif = ipmc_if_lookup(*pifindex);
		if(pif == NULL)
		{
			continue;
		}
		
		vty_out(vty, "IGMP Instance: %d%s", id,VTY_NEWLINE);
		vty_out(vty, "IGMP Version: %d%s", pif->igmp->version,VTY_NEWLINE);
		vty_out(vty, "IGMP Require Router Alert: %s%s",pif->igmp->req_alert?"Enable":"Disable",VTY_NEWLINE);
		vty_out(vty, "IGMP Send Router Alter: %s%s",pif->igmp->send_alert?"Enable":"Disable",VTY_NEWLINE);
		vty_out(vty, "IGMP Other Querier Keepalive: %d%s", pif->igmp->keepalive,VTY_NEWLINE);		
		vty_out(vty, "IGMP Timer Query: %d%s", pif->igmp->qqi,VTY_NEWLINE);
		vty_out(vty, "IGMP Robust Count: %d%s", pif->igmp->qrv,VTY_NEWLINE);
		vty_out(vty, "IGMP Max Response Time: %d%s", pif->igmp->max_resp_dsec,VTY_NEWLINE);
		vty_out(vty, "IGMP Last Member Query Interval: %d%s", pif->igmp->spec_max_resp_dsec,VTY_NEWLINE);
		vty_out(vty, "IGMP Fast Leave: %s%s", pif->igmp->fast_leave?"Enable":"Disable",VTY_NEWLINE);
		vty_out(vty, "IGMP Ssm Mapping: %s%s", pif->igmp->ssm_map_enable?"Enable":"Disable",VTY_NEWLINE);
		for (ALL_LIST_ELEMENTS_RO(pif->igmp->join_list, node, pjoin))
	  	{
		    inet_ipv4tostr(pjoin->grpip, ipv4_addr);
		    vty_out(vty, "IGMP join: group %s", ipv4_addr);
			
			if (pjoin->sip)
			{
				inet_ipv4tostr(pjoin->sip, ipv4_addr);
				vty_out(vty, " source %s", ipv4_addr);
			}

			vty_out(vty, "%s", VTY_NEWLINE);
	  	}
	}

}

DEFUN(show_igmp_config_instance,
	show_igmp_config_instance_cmd,
	"show igmp config {instance <1-128>}",
	"Show igmp config command\n"
	"Igmp\n"
	"Igmp config\n"
	"Igmp instance\n"
	"Igmp instance id:1-128\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t instance_id = 0;
	uint32_t ifindex = 0;

	if(NULL != argv[0])
	{
		instance_id = atoi(argv[0]);
		if ((instance_id < IGMP_INS_MIN) || 
			(instance_id > IGMP_INS_MAX))
		{
			vty_out(vty, "IGMP instance %d is invalid%s", instance_id, VTY_NEWLINE);
		
			return CMD_WARNING;
		}
	}

	pinst = igmp_instance_lookup(instance_id);
	if (NULL != pinst)
	{
		if (pinst->version)
		{
			igmp_config_instance_show(vty, instance_id);
		}
	}

	return CMD_SUCCESS;
}

DEFUN(show_igmp_config_interface,
	show_igmp_config_interface_cmd,
	"show igmp config interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
	"Show igmp config command\n"
	"Igmp\n"
	"Igmp config\n"
	"Interface\n"
	"Ethernet\n"
	"Interface Name\n"
	"Gigabitethernet\n"
	"Interface Name\n"
	"xgigabitethernet\n"
	"Interface Name\n")
{
	struct igmp_instance *pinst = NULL;
	uint8_t instance_id = 0;
	uint32_t ifindex = 0;

	pinst = igmp_instance_lookup(instance_id);
	if (NULL != pinst)
	{
		if (pinst->version)
		{	
			if(NULL != argv[0])
			{
				ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
			}

			if(NULL != argv[1])
			{
				ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
			}

			
			if(NULL != argv[2])
			{
				ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
			}	
			
			igmp_config_instance_interface_show(vty, instance_id, ifindex);
		}
	}
	
	return CMD_SUCCESS;
}



DEFUN(show_igmp_config_instance_interface,
	show_igmp_config_instance_interface_cmd,
	"show igmp config instance <1-128> interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
	"Show igmp config command\n"
	"Igmp\n"
	"Igmp config\n"
	"Igmp instance\n"
	"Igmp instance id:1-128\n"
	"Interface\n"
	"Ethernet\n"
	"Interface Name\n"
	"Gigabitethernet\n"
	"Interface Name\n"
	"xgigabitethernet\n"
	"Interface Name\n")

{
	struct igmp_instance *pinst = NULL;
	uint8_t instance_id;
	uint32_t ifindex = 0;
	int i;

	vty_out(vty, "%s", VTY_NEWLINE);
	vty_out(vty, "----IGMP Config Information----%s", VTY_NEWLINE);
	vty_out(vty, "%s", VTY_NEWLINE);

	if (NULL != argv[0])
	{
		instance_id = atoi(argv[0]);
		if ((instance_id < IGMP_INS_MIN) || 
			(instance_id > IGMP_INS_MAX))
		{
			vty_out(vty, "IGMP instance %d is invalid%s", instance_id, VTY_NEWLINE);
		
    		return CMD_WARNING;
		}
			
		pinst = igmp_instance_lookup(instance_id);
		if (NULL != pinst)
		{
			if (pinst->version)
			{
				if(NULL != argv[1])
				{
					ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[1] );
				}

				if(NULL != argv[2])
				{
					ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[2] );
				}

				if(NULL != argv[3])
				{
					ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[3] );
				}	

				igmp_config_instance_interface_show(vty, instance_id, ifindex);
			}
		}

	}

	return CMD_SUCCESS;
}

/* igmp port  config write*/
static int igmp_if_config_write(struct vty *vty)
{
	struct hash_bucket *pbucket = NULL;
	struct igmp_join *pjoin = NULL;
	struct listnode *node = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t *pifindex = NULL;
	struct igmp *pigmp = NULL;
	char ifname[IFNET_NAMESIZE];
	char ipv4_addr[20];
	int cursor = 0;
	int vpn;

	HASH_BUCKET_LOOP (pbucket, cursor, ipmc_if_table)
	{
		if(NULL != pbucket->data)
		{
			pif = (struct ipmc_if *)pbucket->data;
			if (IGMP_FLAG_DISABLE == pif->igmp_flag)
			{
				continue;
			}
			
			ifm_get_name_by_ifindex(pif->ifindex, ifname);

			vty_out (vty, "interface %s%s", ifname, VTY_NEWLINE);

			if (IGMP_FLAG_ENABLE == pif->igmp_flag)
			{
				vpn = igmp_instance_find(pif->ifindex);
				if (vpn < 0)
				{
					continue;
				}

				if(vpn == 0)
				{
					vty_out (vty, " igmp enable%s", VTY_NEWLINE);
				}
				else
				{
					vty_out (vty, " igmp enable instance %u%s", vpn, VTY_NEWLINE);
				}
			}

			if (NULL != pif->igmp)
			{
				pigmp = pif->igmp;
				for (ALL_LIST_ELEMENTS_RO(pigmp->join_list, node, pjoin))
			  	{
				    inet_ipv4tostr(pjoin->grpip, ipv4_addr);
				    vty_out(vty, " igmp join group %s", ipv4_addr);
					
					if (pjoin->sip)
					{
						inet_ipv4tostr(pjoin->sip, ipv4_addr);
						vty_out(vty, " source %s", ipv4_addr);
					}

					vty_out(vty, "%s", VTY_NEWLINE);
			  	}

				if(IGMP_VERSION3 != pigmp->version)
				{
					vty_out(vty, " igmp version %u%s",pigmp->version, VTY_NEWLINE);
				}
				
				if(pigmp->req_alert)
				{
					vty_out(vty, " igmp require-router-alertr%s", VTY_NEWLINE);
				}
				
				if(pigmp->send_alert)
				{
					vty_out(vty, " igmp send-router-alter%s", VTY_NEWLINE);
				}

				if(IGMP_QUARY_DEFAULT_KEEPALIVE != pigmp->keepalive)
				{
					vty_out(vty, " igmp other-querier keepalive %u%s", pigmp->keepalive, VTY_NEWLINE);
				}
				
				if(IGMP_QUERY_DEFAULT_INTERVAL != pigmp->qqi)
				{
					vty_out(vty, " igmp timer query %u%s", pigmp->qqi, VTY_NEWLINE);
				}

				if(IGMP_ROBUST_DEFAULT_COUNT != pigmp->qrv)
				{
					vty_out(vty, " igmp robust-count %u%s", pigmp->qrv, VTY_NEWLINE);
				}

				if(IGMP_RESPONSE_DEFAULT_TIME != pigmp->max_resp_dsec)
				{
					vty_out(vty, " igmp max-response-time %u%s", pigmp->max_resp_dsec, VTY_NEWLINE);
				}

				if(IGMP_LAST_MEMBER_QUERY_DEFAULT_INTERVAL != pigmp->spec_max_resp_dsec)
				{
					vty_out(vty, " igmp last member-query interval %u%s", pigmp->spec_max_resp_dsec, VTY_NEWLINE);
				}

				if(pigmp->fast_leave)
				{
					vty_out(vty, " igmp fast-leave%s", VTY_NEWLINE);
				}

				if(pigmp->ssm_map_enable)
				{
					vty_out(vty, " igmp ssm-mapping enable%s", VTY_NEWLINE);
				}
			}
			
		}
	}

    return ERRNO_SUCCESS;
}

/*igmp config write*/
static int igmp_config_write(struct vty *vty)
{
	struct igmp_instance *pinst = NULL;
	struct igmp_ssm_map *pssm_map = NULL;
	struct listnode *node = NULL;
	char ipv4_addr[20];
	int i;

	if (NULL == igmp_inst)
	{
		return ERRNO_SUCCESS;
	}

	for (i = 0; i < IGMP_INS_MAX + 1; i++)
	{
		pinst = igmp_instance_lookup(i);
		if (NULL == pinst)
		{
			continue;
		}

		if( i == 0 )
		{
			vty_out(vty, "igmp instance%s",  VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "igmp instance %u%s", i, VTY_NEWLINE);
		}

		if (IGMP_VERSION3 != pinst->version)
		{
			vty_out(vty, " version %u%s", pinst->version, VTY_NEWLINE);
		}

		if (IGMP_QUARY_DEFAULT_KEEPALIVE != pinst->keepalive	)
		{
			vty_out(vty, " other-querier keepalive %u%s", pinst->keepalive, VTY_NEWLINE);
		}

		if (IGMP_QUERY_DEFAULT_INTERVAL != pinst->qqi)
		{
			vty_out(vty, " query interval %u%s", pinst->qqi, VTY_NEWLINE);
		}

		if (IGMP_ROBUST_DEFAULT_COUNT != pinst->robust_count)
		{
			vty_out(vty, " robust-count %u%s", pinst->robust_count, VTY_NEWLINE);
		}

		if (IGMP_LAST_MEMBER_QUERY_DEFAULT_INTERVAL != pinst->last_mem_qqi)
		{
			vty_out(vty, " last member-query interval %u%s", pinst->last_mem_qqi, VTY_NEWLINE);
		}

		if (IGMP_RESPONSE_DEFAULT_TIME != pinst->max_resp)
		{
			vty_out(vty, " max-response-time %u%s", pinst->max_resp, VTY_NEWLINE);
		}

		if (pinst->send_alert)
		{
			vty_out(vty, " send-router-alter%s", VTY_NEWLINE);
		}

		if (pinst->require_alert)
		{
			vty_out(vty, " require-router-alter%s", VTY_NEWLINE);
		}

		if (pinst->ssm_map)
		{
			for (ALL_LIST_ELEMENTS_RO(pinst->ssm_map, node, pssm_map))
		  	{
		  		inet_ipv4tostr(pssm_map->grpip, ipv4_addr);
			    vty_out(vty, " igmp ssm-mapping group %s/%u", ipv4_addr, pssm_map->prefix);				
				if (pssm_map->sip)
				{
					inet_ipv4tostr(pssm_map->sip, ipv4_addr);
					vty_out(vty, " source %s", ipv4_addr);
				}

				vty_out(vty, "%s", VTY_NEWLINE);
		  	}
		}
	}
	
	igmp_if_config_write(vty);
	return ERRNO_SUCCESS;
}

/* igmp command init */
void igmp_cmd_init(void)
{
	install_node(&igmp_node, igmp_config_write);
	install_default (IGMP_NODE);

	/*igmp instance command*/
	install_element (CONFIG_NODE, &igmp_instance_cfg_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_igmp_instance_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_version_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_version_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_querier_keepalive_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_querier_keepalive_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_query_interval_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_query_interval_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_isnt_robust_count_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_robust_count_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_last_member_query_interval_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_last_member_query_interval_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_max_resp_time_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_max_resp_time_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_send_router_alert_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_send_router_alert_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_inst_require_router_alert_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_inst_require_router_alert_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &igmp_ssm_mapping_group_cmd, CMD_SYNC);
	install_element (IGMP_NODE, &no_igmp_ssm_mapping_group_cmd, CMD_SYNC);

	/*interface command*/
	install_element (PHYSICAL_IF_NODE, &igmp_enable_instance_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_enable_instance_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_version_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_version_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_port_static_group_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_port_static_group_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_require_router_alert_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_require_router_alert_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_send_router_alert_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_send_router_alert_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_other_querier_keepalive_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_other_querier_keepalive_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_query_interval_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_query_interval_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_robust_count_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_robust_count_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_max_resp_time_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_max_resp_time_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_last_member_query_interval_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_last_member_query_interval_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_fast_leave_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_fast_leave_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &igmp_ssm_mapping_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_igmp_ssm_mapping_cmd, CMD_SYNC);

	/*debug and show igmp command*/
	install_element (CONFIG_NODE, &debug_igmp_packet_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &no_debug_igmp_packet_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_debug_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_group_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_config_instance_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_config_interface_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_config_instance_interface_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_routing_table_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_igmp_statistics_cmd, CMD_LOCAL);
}


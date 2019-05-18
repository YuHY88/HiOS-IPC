#include <string.h>
#include <stdlib.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/types.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "l2/l2_if.h"
#include "l2/trunk.h"
#include "efm_cmd.h"
#include "efm_link_monitor.h"
#include "efm.h"
#include "efm_state.h"
#include "efm_agent.h"

DEFUN (efm_enable,
		efm_enable_cmd,
		"efm enable",
		EFM_STR
		"Enable EFM\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	ret = efm_protocol_enable(pif);

	if(ret)
    {
	vty_info_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_SUCCESS;
    }
	
	return ret;
}

DEFUN (no_efm_enable,
		no_efm_enable_cmd,
		"no efm enable",
		NO_STR
		EFM_STR
		"Enable EFM\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = CMD_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	ret = efm_protocol_disable(pif);

	if(ret)
    {
	vty_info_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_SUCCESS;
    }
	
	return ret;
}

DEFUN (efm_mode_active,
		efm_mode_active_cmd,
		"efm mode (active |passive)",
		EFM_STR
		"EFM Mode\n"
		"Active\n"
		"Passive\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret =CMD_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(!strncmp("a",argv[0],strlen("a")))
    {
		ret = efm_mode_active_set (pif->pefm);
    }
	else 
    {
		ret = efm_mode_passive_set (pif->pefm);	
    }

	if(ret)
    {
	vty_info_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_SUCCESS;
    }
	
	return ret;
}

DEFUN (efm_agent,
		efm_agent_cmd,
		"efm agent enable",
		EFM_STR
		"EFM Agent\n"
		"Enable\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	efm_agent_info * aefm = NULL;
	
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm)
	{
		vty_info_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(NULL == pif->pefm->aefm)
	{
		pif->pefm->aefm = (efm_agent_info *)XMALLOC (MTYPE_EFM, sizeof (efm_agent_info));
		if(NULL == pif->pefm->aefm)
		{
		vty_error_out (vty, "%%EFM malloc agent data space fail!%s", VTY_NEWLINE);
		return CMD_WARNING;
		}
		memset(pif->pefm->aefm,0,sizeof(efm_agent_info));
		memcpy(pif->pefm->aefm->efm_agent_hostname, "Hios", strlen("Hios"));
	}
	else 
	{
		vty_info_out (vty, "%%EFM agent  has enabled!%s", VTY_NEWLINE);
		return EFM_SUCCESS;
	}
	aefm = pif->pefm->aefm;
	
	if(NULL == aefm->aefm_RbaseInfo)
	{
		aefm->aefm_RbaseInfo= (efm_agent_remote_base_info *)XMALLOC(MTYPE_EFM,sizeof(efm_agent_remote_base_info));
		if(NULL == aefm->aefm_RbaseInfo)
		{
				zlog_err("%s[%d]aefm malloc baseinfo  space  fail!",__FUNCTION__,__LINE__);
				return -1;
		}
		memset(aefm->aefm_RbaseInfo,0,sizeof(efm_agent_remote_base_info));
	}
	if(NULL == aefm->aefm_RstInfo)
	{
		aefm->aefm_RstInfo= (efm_agent_remote_state_info *)XMALLOC(MTYPE_EFM,sizeof(efm_agent_remote_state_info));
		if(NULL == aefm->aefm_RstInfo)
		{
				zlog_err("%s[%d]aefm malloc getinfo space  fail!",__FUNCTION__,__LINE__);
				return -1;
		}
		memset(aefm->aefm_RstInfo,0,sizeof(efm_agent_remote_state_info));
	}
	if(NULL == aefm->aefm_RsetGetInfo)
	{
		aefm->aefm_RsetGetInfo= (efm_agent_remote_SetGet_info *)XMALLOC(MTYPE_EFM,sizeof(efm_agent_remote_SetGet_info));
		if(NULL == aefm->aefm_RsetGetInfo)
		{
				zlog_err("%s[%d]aefm malloc setget space  fail!",__FUNCTION__,__LINE__);
				return -1;
		}
		memset(aefm->aefm_RsetGetInfo,0,sizeof(efm_agent_remote_SetGet_info));
	}
	if(NULL == aefm->aefm_RsetInfo)
	{
		aefm->aefm_RsetInfo= (efm_agent_remote_set_info *)XMALLOC(MTYPE_EFM,sizeof(efm_agent_remote_set_info));
		if(NULL == aefm->aefm_RsetInfo)
		{
				zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
				return -1;
		}
		memset(aefm->aefm_RsetInfo,0,sizeof(efm_agent_remote_set_info));
	}
	
	/*first get remote device info:  device_id device_type fpga soft_ver etc*/
	
	aefm->efm_agent_pdu_type  = EFM_AGENT_DEVICE_INFO_PDU;
	efm_agent_pdu_tx(pif->pefm);
		
	aefm->efm_agent_pdu_time = EFM_AGENT_BASE_PDU_TIME;

	EFM_TIMER_ON_LOOP (aefm->efm_agent_pdu_timer, efm_agent_pdu_timer_expiry,
			pif->pefm,aefm->efm_agent_pdu_time);
	
	return EFM_SUCCESS;
}

DEFUN (efm_no_agent,
		efm_no_agent_cmd,
		"no efm agent enable",
		NO_STR
		EFM_STR
		"EFM Agent\n"
		"Enable\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret =CMD_SUCCESS;
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm)
	{
		vty_info_out (vty, "%%EFM agent not enabled%s", VTY_NEWLINE);
		return EFM_SUCCESS;
	}
	pif->pefm->tx_count[EFM_SPECIFIC_PDU] = 0;
	pif->pefm->rx_count[EFM_SPECIFIC_PDU] = 0;
	if(pif->pefm->aefm != NULL)
	{	
		efm_agent_disable(pif->pefm);
		XFREE (MTYPE_EFM, pif->pefm->aefm);
		pif->pefm->aefm = NULL;
	}
	return ret;
}
DEFUN (efm_agent_id,
		efm_agent_id_cmd,
		"efm agent-id <65537-16711678>",
		EFM_STR
		"Set NE ID\n"
  		"Net-Element ID value\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	uint32_t agent_id = 0;
	efm_agent_info * aefm = NULL;


	
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm)
	{
		vty_info_out (vty, "%%EFM agent not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	aefm = pif->pefm->aefm;
	agent_id = atoll(argv[0]);

	
	aefm->efm_agent_update_id = agent_id;
	
	return CMD_SUCCESS;
}
DEFUN (efm_agent_hostname,
		efm_agent_hostname_cmd,
		"efm agent-hostname NAMES",
		EFM_STR
		"EFM Agent Hostname\n"
		"Agent Name\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	efm_agent_info * aefm = NULL;
	uint8_t agent_name[VTYSH_HOST_NAME_LEN + 1] = {0};

	
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm)
	{
		vty_info_out (vty, "%%EFM agent not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	aefm = pif->pefm->aefm;

	if(strlen(argv[0]) > (VTYSH_HOST_NAME_LEN + 1))
	{
		vty_info_out (vty, "%%EFM agent hostname exceed %d byte%s",VTYSH_HOST_NAME_LEN + 1, VTY_NEWLINE);
		return CMD_WARNING;

	}
	memcpy(agent_name,argv[0],strlen(argv[0]));

		
	/*compare hostname*/
	if(!memcmp(aefm->efm_agent_hostname,agent_name,VTYSH_HOST_NAME_LEN + 1))
	{
		vty_info_out (vty, "%%EFM agent-hostname : %s has config%s",agent_name, VTY_NEWLINE);
		return EFM_SUCCESS;
	}
		
	memcpy(aefm->efm_agent_hostname,agent_name,VTYSH_HOST_NAME_LEN + 1);
	if(aefm->efm_agent_link_flag && aefm->efm_agent_last_link_flag)
	{
		efm_agent_u0_info_send(pif->pefm->if_index,aefm,IPC_OPCODE_UPDATE);
	}
	
	return CMD_SUCCESS;
}
DEFUN (no_efm_agent_hostname,
		no_efm_agent_hostname_cmd,
		"no efm agent-hostname ",
		NO_STR
		EFM_STR
		"EFM Agent Hostname\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	efm_agent_info * aefm = NULL;

	
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm)
	{
		vty_info_out (vty, "%%EFM agent not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	aefm = pif->pefm->aefm;

	memset(aefm->efm_agent_hostname,0,VTYSH_HOST_NAME_LEN + 1);
	memcpy(aefm->efm_agent_hostname,"Hios",strlen("Hios"));
	if(aefm->efm_agent_link_flag && aefm->efm_agent_last_link_flag)
	{
		efm_agent_u0_info_send(pif->pefm->if_index,aefm,IPC_OPCODE_UPDATE);
	}
		
	return CMD_SUCCESS;
}


DEFUN (efm_agent_ip,
		efm_agent_ip_cmd,
		"efm agent-ip A.B.C.D",
		EFM_STR
		"EFM Agent Ip\n"
		"IP format A.B.C.D\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	uint32_t agent_ip = 0;
	efm_agent_info * aefm = NULL;
	uint8_t *ip = NULL;

	
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm)
	{
		vty_info_out (vty, "%%EFM agent not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	aefm = pif->pefm->aefm;
	agent_ip = inet_strtoipv4((char *)argv[0]);

	if(agent_ip)
	{	
		ip = (uint8_t *)&agent_ip;
		
		/*compare ip*/
		if(!memcmp(aefm->efm_agent_update_ip,&agent_ip,4))
		{
			//ip = (uint8_t *)&agent_ip;
			vty_info_out (vty, "%%EFM agent ip: %d.%d.%d.%d has config%s",ip[0],ip[1],ip[2],ip[3], VTY_NEWLINE);
			return EFM_SUCCESS;
		}
		
		memcpy(aefm->efm_agent_update_ip,&agent_ip,4);
		/*if remote agent device has exist and send config new ip to ospf*/
		if(aefm->efm_agent_link_flag && aefm->efm_agent_last_link_flag)
		{
			efm_agent_u0_info_send(pif->pefm->if_index,aefm,IPC_OPCODE_UPDATE);

		}

	}
	
	return CMD_SUCCESS;
}



DEFUN (no_efm_agent_ip,
		no_efm_agent_ip_cmd,
		"no efm agent-ip",
		NO_STR
		EFM_STR
		"EFM Agent Ip\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	efm_agent_info * aefm = NULL;

	
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm ||  NULL == pif->pefm->aefm->aefm_RsetGetInfo)
	{
		vty_info_out (vty, "%%EFM agent not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	aefm = pif->pefm->aefm;
	
	/*update id is differ with default ip*/
	if(memcmp(aefm->efm_agent_update_ip,aefm->efm_agent_default_ip,4))
	{
		memcpy(aefm->efm_agent_update_ip,aefm->efm_agent_default_ip,4);

		/*if remote agent device has exist and send config new ip to ospf*/
		if(aefm->efm_agent_link_flag && aefm->efm_agent_last_link_flag)
		{
			efm_agent_u0_info_send(pif->pefm->if_index,aefm,IPC_OPCODE_UPDATE);

		}
		
	}
	return CMD_SUCCESS;
}





#if 0
DEFUN (efm_agent_pdu_timer,
		efm_agent_pdu_timer_cmd,
		"efm agent rate <2-5>",
		EFM_STR
		"EFM Agent\n"
		"Set the time for efm agent send pdu-timer\n"
		"Timer value (2-5)sec\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint8_t secs = 0;
	if_index = (uint32_t)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	if (NULL == pif->pefm || NULL == pif->pefm->aefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	secs = atoi(argv[0]);
	if((secs < EFM_AGENT_PDU_TIMER_MIN) ||(secs > EFM_AGENT_PDU_TIMER_MAX))
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <2-5>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	pif->pefm->aefm->efm_agent_pdu_time = secs;
	
	return ret;
}
#endif

DEFUN(efm_rem_loopback,
		efm_rem_loopback_cmd,
		"efm remote-loopback support ",
		"Efm node\n"
		"Remote Loopback\n"
		"Configure Remote Loopback Support\n")
{        
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = CMD_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	ret = efm_remote_loopback_set (pif->pefm, EFM_TRUE);
	
        if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
	
    }
	
	return ret;
}
DEFUN(efm_rem_loopback_time,
		efm_rem_loopback_time_cmd,
		"efm remote-loopback timeout <1-10>",
		"Efm node\n"
		"Remote Loopback\n"
		"Remote Loopback Timeout\n"
		"Timeout value (1-10) secs\n")
{        
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = CMD_SUCCESS;
	uint8_t timeout = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL != argv[0])
    {
		timeout = atoi(argv[0]);	
    }

	if((timeout < EFM_REM_LB_TIMEOUT_MIN) || (timeout > EFM_REM_LB_TIMEOUT_MAX))
	{	
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <1-10>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_remote_loopback_timeout_set (pif->pefm, timeout);
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_rem_loopback,
		no_efm_rem_loopback_st_cmd,
		"no efm remote-loopback support {timeout}",
		NO_STR
		EFM_STR
		EFM_REMOTE_LB_STR
		"Configure Remote Loopback Support\n"
		EFM_REM_LB_TIMEOUT_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = CMD_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	ret = efm_remote_loopback_set (pif->pefm, EFM_FALSE);

	if (NULL != argv[0])
	{
		ret = efm_remote_loopback_timeout_set (pif->pefm, EFM_REM_LB_TIMEOUT_NONE);
	}

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}
DEFUN (no_efm_rem_loopback_t,
		no_efm_rem_loopback_ts_cmd,
		"no efm remote-loopback  timeout {support}",
		NO_STR
		EFM_STR
		EFM_REMOTE_LB_STR
		EFM_REM_LB_TIMEOUT_STR
		"Configure Remote Loopback Support\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = CMD_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	ret = efm_remote_loopback_timeout_set (pif->pefm, EFM_REM_LB_TIMEOUT_NONE);



	if (NULL != argv[0])
	{
		ret = efm_remote_loopback_set (pif->pefm, EFM_FALSE);
	}



	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_remote_loopback_start,
		efm_remote_loopback_start_cmd,
		"efm remote-loopback enable",
		EFM_STR
		EFM_REMOTE_LB_STR
		"Start Remote Loopback\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_remote_loopback_start_api (pif->pefm);
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_remote_loopback_start,
		no_efm_remote_loopback_start_cmd,
		"no efm remote-loopback enable",
		NO_STR
		EFM_STR
		EFM_REMOTE_LB_STR
		"Start Remote Loopback\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_remote_loopback_exit (pif->pefm);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}
DEFUN (efm_remote_loopback_mac_swap,
		efm_remote_loopback_mac_swap_cmd,
		"efm remote-loopback mac-swap",
		EFM_STR
		EFM_REMOTE_LB_STR
		"Start Mac Swap\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	uint8_t ret =CMD_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	pif->pefm->efm_mac_swap = EFM_TRUE;
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
	
}
DEFUN (efm_remote_loopback_no_mac_swap,
		efm_remote_loopback_no_mac_swap_cmd,
		"no efm remote-loopback mac-swap",
		NO_STR
		EFM_STR
		EFM_REMOTE_LB_STR
		"Start Mac Swap\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
         uint8_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	pif->pefm->efm_mac_swap = EFM_FALSE;
         if(ret)
         {
		vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
		return EFM_FAILURE;
         }
		 
	return ret;
	
}

DEFUN (efm_link_monitor,
		efm_link_monitor_cmd,
		"efm link-monitor support",
		EFM_STR
		EFM_LINK_MONITOR_STR
		"Configure Link Monitor Support\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_link_monitor_support_set (pif->pefm, EFM_TRUE);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_link_monitor,
		no_efm_link_monitor_cmd,
		"no efm link-monitor support",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		"Configure Link Monitor Support\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_link_monitor_support_set (pif->pefm, EFM_FALSE);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_link_monitor_on,
		efm_link_monitor_on_cmd,
		"efm link-monitor enable",
		EFM_STR
		EFM_LINK_MONITOR_STR
		"Start Link Monitoring\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(pif->pefm->efm_link_monitor_t.link_monitor_on)
	{
		vty_error_out (vty, "%%EFM has enable link monitor%s", VTY_NEWLINE);
				return CMD_WARNING;
              
	}
         if(!CHECK_FLAG (pif->pefm->local_info.oam_config, EFM_CFG_LINK_EVENT_SUPPORT))
	{
	         vty_error_out (vty, "%%EFM not support  link monitor enable%s", VTY_NEWLINE);
		 return CMD_WARNING;

         }
	ret = efm_link_monitor_on_set (pif->pefm, EFM_TRUE);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_link_monitor_on,
		no_efm_link_monitor_on_cmd,
		"no efm link-monitor enable",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		"Start Link Monitoring\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_link_monitor_on_set (pif->pefm, EFM_FALSE);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}


DEFUN (efm_event_log_size,
		efm_event_log_size_cmd,
		"efm event-log-number <1-256>",
		EFM_STR
		"Set the maximum number of log entries to be collected in Event Log\n"
		"Log size (1-256)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint32_t log_size = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	log_size = atoi(argv[0]);
	if((log_size < EFM_EVENT_LOG_SIZE_MIN) || (log_size > EFM_EVENT_LOG_SIZE_MAX))
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, event log size support <1-256>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_event_log_size_set (pif->pefm, log_size);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_event_log_size,
		no_efm_event_log_size_cmd,
		"no efm event-log-number",
		NO_STR
		EFM_STR
		"Set the maximum number of log entries to be collected in Event Log\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_event_log_size_set (pif->pefm, EFM_MAX_EVLOG_ENTRIES_DEF);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (clear_efm_event_log,
		clear_efm_event_log_cmd,
		"clear efm event-log",
		CLEAR_STR
		EFM_STR
		"Event logs\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_clear_event_log(pif->pefm);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_sym_period_event_tw,
		efm_sym_period_event_tw_cmd,
		"efm link-monitor symbol-period threshold <0-4294967295> {window <1-4294967295>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_SYMBOL_PERIOD_STR
		EFM_THRESHOLD_STR
		"Error symbol threshold value, (0-4294967295)\n"
		EFM_WINDOW_STR
		"Error symbol window value, (1-4294967295)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint64_t sym_thres = 0;
	uint64_t sym_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL == argv[0])
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	sym_thres = atoll(argv[0]);


	if(NULL !=argv[1])
	{

		sym_window = atoll(argv[1]);
		if(sym_window < EFM_SYM_WINDOW_MIN||sym_window > EFM_SYM_WINDOW_MAX)
		{	
			vty_error_out(vty,"%%Invalid EFM parameter range, interval is <1-4294967295>. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}


	}


	if(NULL!=argv[1])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, 
						sym_window,sym_thres)))
		{
			efm_symbol_period_threshold_set(pif->pefm, sym_thres, EFM_TRUE);
			ret = efm_symbol_period_window_set(pif->pefm, sym_window, EFM_TRUE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, 
						pif->pefm->local_link_info.err_symbol_window,sym_thres)))
		{
			ret = efm_symbol_period_threshold_set(pif->pefm, sym_thres, EFM_TRUE);
		}

	}
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;

}
DEFUN (efm_sym_period_event_wt,
		efm_sym_period_event_wt_cmd,
		"efm link-monitor symbol-period window <0-4294967295> {threshold <0-4294967295>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_SYMBOL_PERIOD_STR
		EFM_WINDOW_STR
		"Error symbol window value, (1-4294967295)\n"
		EFM_THRESHOLD_STR
		"Error symbol threshold value, (0-4294967295)\n")
{

	uint32_t if_index = 0;  
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint64_t sym_thres = 0;
	uint64_t sym_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL== argv[0])
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	sym_window = atoll(argv[0]);
	if(sym_window < EFM_SYM_WINDOW_MIN || sym_window > EFM_SYM_WINDOW_MAX)
	{	
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <1-4294967295>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(NULL !=argv[1])
	{
		sym_thres = atoll(argv[1]);		

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, 
						sym_window,sym_thres)))
		{
			 efm_symbol_period_threshold_set(pif->pefm, sym_thres, EFM_TRUE);
			ret = efm_symbol_period_window_set(pif->pefm, sym_window, EFM_TRUE);
		}

	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, sym_window,
						pif->pefm->local_link_info.err_symbol_threshold)))
		{
			ret = efm_symbol_period_window_set(pif->pefm, sym_window, EFM_TRUE);
		}

	}

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;



}


DEFUN (no_efm_sym_period_event_tw,
		no_efm_sym_period_event_tw_cmd,
		"no efm link-monitor symbol-period threshold {window}",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_SYMBOL_PERIOD_STR
		EFM_THRESHOLD_STR
		EFM_WINDOW_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}




	if(NULL!=argv[0])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT,
						(uint64_t)EFM_SYMBOL_PERIOD_WINDOW_DEF,(uint64_t)EFM_SYMBOL_PERIOD_THRES_DEF)))
		{
			efm_symbol_period_threshold_set(pif->pefm, 
					EFM_SYMBOL_PERIOD_THRES_DEF, EFM_FALSE);
			ret = efm_symbol_period_window_set(pif->pefm, 
					EFM_SYMBOL_PERIOD_WINDOW_DEF, EFM_FALSE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, 
						pif->pefm->local_link_info.err_symbol_window,(uint64_t)EFM_SYMBOL_PERIOD_THRES_DEF)))
		{
			ret = efm_symbol_period_threshold_set(pif->pefm, 
					EFM_SYMBOL_PERIOD_THRES_DEF, EFM_FALSE);
		}

	}


	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}
DEFUN (no_efm_sym_period_event_wt,
		no_efm_sym_period_event_wt_cmd,
		"no efm link-monitor symbol-period  window {threshold}",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_SYMBOL_PERIOD_STR
		EFM_WINDOW_STR
		EFM_THRESHOLD_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	if (NULL != argv[0])
	{	     		
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, EFM_SYMBOL_PERIOD_WINDOW_DEF,
						EFM_SYMBOL_PERIOD_THRES_DEF)))
		{
			efm_symbol_period_threshold_set(pif->pefm, 
					EFM_SYMBOL_PERIOD_THRES_DEF, EFM_FALSE);
			ret = efm_symbol_period_window_set(pif->pefm, 
					EFM_SYMBOL_PERIOD_WINDOW_DEF, EFM_FALSE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, EFM_SYMBOL_PERIOD_WINDOW_DEF,
						pif->pefm->local_link_info.err_symbol_threshold)))
		{
			ret = efm_symbol_period_window_set(pif->pefm, 
					EFM_SYMBOL_PERIOD_WINDOW_DEF, EFM_FALSE);
		}
	}			   

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}


DEFUN (efm_err_frame_event_tw,
		efm_err_frame_event_tw_cmd,
		"efm link-monitor frame threshold <0-4294967295> {window <1-60>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_STR
		EFM_THRESHOLD_STR
		"Error frame threshold value, (0-4294967295)\n"
		EFM_WINDOW_STR
		"Error frame window value, 1-60(sec)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint32_t  frame_thres = 0;
	uint16_t frame_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL == argv[0])
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	frame_thres = (uint32_t)atoll(argv[0]);


	if (NULL != argv[1])
	{
		frame_window = (uint16_t)atoi(argv[1]);
		if((frame_window < EFM_FRAME_WINDOW_MIN) || (frame_window > EFM_FRAME_WINDOW_MAX))
		{ 
			vty_error_out(vty,"%%Invalid EFM parameter range, interval is <1-60>. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}


	}



	if(NULL!=argv[1])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)frame_window ,(uint64_t)frame_thres)))
		{
			 efm_err_frame_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
			ret = efm_err_frame_window_set(pif->pefm, frame_window, EFM_TRUE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)pif->pefm->local_link_info.err_frame_window,(uint64_t)frame_thres)))
		{
			ret = efm_err_frame_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
		}

	}

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}
DEFUN (efm_err_frame_event_wt,
		efm_err_frame_event_wt_cmd,
		"efm link-monitor frame window <1-60> {threshold <0-4294967295>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_STR
		EFM_WINDOW_STR
		"Error frame window value, 1-60(sec)\n"
		EFM_THRESHOLD_STR
		"Error frame threshold value, (0-4294967295)\n")

{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint32_t frame_thres = 0;
	uint16_t frame_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL == argv[0])
	{
			vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
			return CMD_WARNING;
	}

	frame_window = atoi(argv[0]);
	if((frame_window < EFM_FRAME_WINDOW_MIN) || (frame_window > EFM_FRAME_WINDOW_MAX))
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <1-60>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	if (NULL != argv[1])
	{
		frame_thres = atoll(argv[1]);

	}

	
	if(NULL!=argv[1])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)frame_window ,(uint64_t)frame_thres)))
		{
			efm_err_frame_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
			ret = efm_err_frame_window_set(pif->pefm, frame_window, EFM_TRUE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)frame_window,(uint64_t)pif->pefm->local_link_info.err_frame_threshold)))
		{
			ret = efm_err_frame_window_set(pif->pefm, frame_window, EFM_TRUE);
		}

	}




	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_err_frame_event_tw,
		no_efm_err_frame_event_tw_cmd,
		"no efm link-monitor frame  threshold {window}",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_STR
		EFM_THRESHOLD_STR
		EFM_WINDOW_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}




	if(NULL!=argv[0])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)EFM_ERR_FRAME_WINDOW_DEF ,(uint64_t)EFM_ERR_FRAME_THRES_DEF)))
		{

			efm_err_frame_threshold_set(pif->pefm, 
					EFM_ERR_FRAME_THRES_DEF, EFM_FALSE);

			ret = efm_err_frame_window_set(pif->pefm, 
					EFM_ERR_FRAME_WINDOW_DEF, EFM_FALSE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)pif->pefm->local_link_info.err_frame_window ,(uint64_t)EFM_ERR_FRAME_THRES_DEF)))				 	
		{
			ret = efm_err_frame_threshold_set(pif->pefm, 
					EFM_ERR_FRAME_THRES_DEF, EFM_FALSE);

		}


	}	 	
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}
DEFUN (no_efm_err_frame_event_wt,
		no_efm_err_frame_event_wt_cmd,
		"no efm link-monitor frame window { threshold }",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_STR
		EFM_WINDOW_STR
		EFM_THRESHOLD_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}





	if(NULL!=argv[0])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)EFM_ERR_FRAME_WINDOW_DEF ,(uint64_t)EFM_ERR_FRAME_THRES_DEF)))
		{

			efm_err_frame_threshold_set(pif->pefm, 
					EFM_ERR_FRAME_THRES_DEF, EFM_FALSE);

			ret = efm_err_frame_window_set(pif->pefm, 
					EFM_ERR_FRAME_WINDOW_DEF, EFM_FALSE);
		}
	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_EVENT, 
						(uint64_t)EFM_ERR_FRAME_WINDOW_DEF ,(uint64_t)pif->pefm->local_link_info.err_frame_threshold)))				 	
		{
			ret = efm_err_frame_window_set(pif->pefm, 
					EFM_ERR_FRAME_WINDOW_DEF, EFM_FALSE);


		}
	}
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}


DEFUN (efm_err_frame_period_event_tw,
		efm_err_frame_period_event_tw_cmd,
		"efm link-monitor frame-period threshold <0-4294967295> {window <10000-4294967295>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_PERIOD_STR
		EFM_THRESHOLD_STR
		"Error frame period threshold value, (0-4294967295)\n"
		EFM_WINDOW_STR
		"Error frame period window value, 10000-4294967295(frame)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint32_t frame_thres = 0;
	uint32_t frame_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(NULL == argv[0])
	{
			vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
			return CMD_WARNING;
	}

	frame_thres = (uint32_t)atoll(argv[0]);


	if (NULL != argv[1])
	{
		frame_window = (uint32_t)atoll(argv[1]);
		if(frame_window < EFM_FRAME_PERIOD_WINDOW_MIN)
		{ 
			vty_error_out(vty,"%%Invalid EFM parameter range, interval is <10000-4294967295>. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}


	}

	
	if(NULL!=argv[1])
	{

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)frame_window ,(uint64_t)frame_thres)))
		{
			efm_err_frame_period_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
			ret = efm_err_frame_period_window_set(pif->pefm, frame_window, EFM_TRUE);
		}       


	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)pif->pefm->local_link_info.err_frame_period_window ,(uint64_t)frame_thres)))
		{
			ret = efm_err_frame_period_threshold_set(pif->pefm, frame_thres, EFM_TRUE);

		}       


	}



	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}


DEFUN (efm_err_frame_period_event_wt,
		efm_err_frame_period_event_wt_cmd,
		"efm link-monitor frame-period  window <10000-4294967295> {threshold <0-4294967295>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_PERIOD_STR
		EFM_WINDOW_STR
		"Error frame period window value, 10000-4294967295(frame)\n"
		EFM_THRESHOLD_STR
		"Error frame period threshold value, (0-4294967295)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint64_t frame_thres = 0;
	uint64_t frame_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL == argv[0])
	{
			vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
			return CMD_WARNING;
	}

	frame_window = (uint32_t)atoll(argv[0]);
	if(frame_window < EFM_FRAME_PERIOD_WINDOW_MIN ||frame_window > EFM_FRAME_PERIOD_WINDOW_MAX )
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <10000-4294967295>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}



	if (NULL != argv[1])
	{
		frame_thres = (uint32_t)atoll(argv[1]);

	}

	
	if(NULL!=argv[1])
	{

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)frame_window ,(uint64_t)frame_thres)))
		{
			efm_err_frame_period_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
			ret = efm_err_frame_period_window_set(pif->pefm, frame_window, EFM_TRUE);
		}	   


	}
	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)frame_window ,(uint64_t)pif->pefm->local_link_info.err_frame_period_threshold)))
		{
			ret = efm_err_frame_period_window_set(pif->pefm, frame_window, EFM_TRUE);

		}	   


	}

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_err_frame_period_event_tw,
		no_efm_err_frame_period_event_tw_cmd,
		"no efm link-monitor frame-period threshold {window}",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_PERIOD_STR
		EFM_THRESHOLD_STR
		EFM_WINDOW_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}




	if (NULL != argv[0])
	{


		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)EFM_FRAME_PERIOD_WINDOW_DEF ,(uint64_t)EFM_FRAME_PERIOD_THRES_DEF)))
		{
			efm_err_frame_period_window_set(pif->pefm, 
					EFM_FRAME_PERIOD_WINDOW_DEF, EFM_FALSE);

			ret = efm_err_frame_period_threshold_set(pif->pefm, 
					EFM_FRAME_PERIOD_THRES_DEF, EFM_FALSE);

		}
	}
	else
	{

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)pif->pefm->local_link_info.err_frame_period_window ,(uint64_t)EFM_FRAME_PERIOD_THRES_DEF)))
		{
			ret = efm_err_frame_period_threshold_set(pif->pefm, 
					EFM_FRAME_PERIOD_THRES_DEF, EFM_FALSE);

		}

	}

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}


DEFUN (no_efm_err_frame_period_event_wt,
		no_efm_err_frame_period_event_wt_cmd,
		"no efm link-monitor frame-period  window {threshold}",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_PERIOD_STR
		EFM_WINDOW_STR
		EFM_THRESHOLD_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	if (NULL != argv[0])
	{


		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)EFM_FRAME_PERIOD_WINDOW_DEF ,(uint64_t)EFM_FRAME_PERIOD_THRES_DEF)))
		{
			efm_err_frame_period_window_set(pif->pefm, 
					EFM_FRAME_PERIOD_WINDOW_DEF, EFM_FALSE);

			ret = efm_err_frame_period_threshold_set(pif->pefm, 
					EFM_FRAME_PERIOD_THRES_DEF, EFM_FALSE);

		}
	}
	else
	{

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_PERIOD_EVENT, 
						(uint64_t)EFM_FRAME_PERIOD_WINDOW_DEF ,(uint64_t)pif->pefm->local_link_info.err_frame_period_threshold)))
		{
			ret = efm_err_frame_period_window_set(pif->pefm, 
					EFM_FRAME_PERIOD_WINDOW_DEF, EFM_FALSE);

		}

	}
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_err_frame_second_event_tw,
		efm_err_frame_second_event_tw_cmd,
		"efm link-monitor frame-seconds threshold <0-65535> {window <10-900>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_SECOND_STR
		EFM_THRESHOLD_STR
		"Error frame seconds threshold value, (0-65535)\n"
		EFM_WINDOW_STR
		"Error frame seconds window value, 10-900(sec)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint16_t frame_thres = 0;
	uint16_t frame_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	frame_thres = (uint16_t)atoi(argv[0]);



	if (NULL != argv[1])
	{
		frame_window = (uint16_t)atoi(argv[1]);
		if((frame_window < EFM_FRAME_SEC_WINDOW_MIN) || 
				(frame_window > EFM_FRAME_SEC_WINDOW_MAX))
		{ 
			vty_error_out(vty,"%%Invalid EFM parameter range, interval is <10000-4294967295>. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}


	}

	if(NULL == argv[0])
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL!=argv[1])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)frame_window ,(uint64_t)frame_thres)))
		{

			efm_err_frame_second_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
			ret = efm_err_frame_second_window_set(pif->pefm, frame_window, EFM_TRUE);

		}	
	}

	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)pif->pefm->local_link_info.err_frame_sec_sum_window ,(uint64_t)frame_thres)))
		{
			ret = efm_err_frame_second_threshold_set(pif->pefm, frame_thres, EFM_TRUE);

		}

	}
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_err_frame_second_event_wt,
		efm_err_frame_second_event_wt_cmd,
		"efm link-monitor frame-seconds  window <10-900> {threshold <0-65535>}",
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_SECOND_STR
		EFM_WINDOW_STR
		"Error frame seconds window value, 10-900(sec)\n"
		EFM_THRESHOLD_STR
		"Error frame seconds threshold value, (0-65535)\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint16_t frame_thres = 0;
	uint32_t frame_window = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	frame_window = (uint16_t)atoi(argv[0]);
	if((frame_window < EFM_FRAME_SEC_WINDOW_MIN) || 
			(frame_window > EFM_FRAME_SEC_WINDOW_MAX))
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <10000-4294967295>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	if (NULL != argv[1])
	{
		frame_thres = (uint16_t)atoi(argv[1]);
	}

	if(NULL == argv[0])
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NULL!=argv[1])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)frame_window ,(uint64_t)frame_thres)))
		{

			efm_err_frame_second_threshold_set(pif->pefm, frame_thres, EFM_TRUE);
			ret = efm_err_frame_second_window_set(pif->pefm, frame_window, EFM_TRUE);

		}	
	}

	else
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)frame_window ,(uint64_t)pif->pefm->local_link_info.err_frame_sec_sum_threshold)))
		{
			ret = efm_err_frame_second_window_set(pif->pefm, frame_window, EFM_TRUE);
		}

	}
	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_err_frame_second_event_tw,
		no_efm_err_frame_second_event_tw_cmd,
		"no efm link-monitor frame-seconds threshold {window}",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_SECOND_STR
		EFM_THRESHOLD_STR
		EFM_WINDOW_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}




	if (NULL != argv[0])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)EFM_FRAME_SECOND_WINDOW_DEF ,(uint64_t)EFM_FRAME_SECOND_THRES_DEF)))
		{
			efm_err_frame_second_window_set(pif->pefm, 
					EFM_FRAME_SECOND_WINDOW_DEF, EFM_FALSE);
			ret = efm_err_frame_second_threshold_set(pif->pefm, 
					EFM_FRAME_SECOND_THRES_DEF, EFM_FALSE);
		}

	}
	else
	{

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)pif->pefm->local_link_info.err_frame_sec_sum_window ,(uint64_t)EFM_FRAME_SECOND_THRES_DEF)))
		{
			ret = efm_err_frame_second_threshold_set(pif->pefm, 
					EFM_FRAME_SECOND_THRES_DEF, EFM_FALSE);
		}

	}


	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_err_frame_second_event_wt,
		no_efm_err_frame_second_event_wt_cmd,
		"no efm link-monitor frame-seconds window {threshold }",
		NO_STR
		EFM_STR
		EFM_LINK_MONITOR_STR
		EFM_ERR_FRAME_SECOND_STR
		EFM_WINDOW_STR
		EFM_THRESHOLD_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}



	if (NULL != argv[0])
	{
		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)EFM_FRAME_SECOND_WINDOW_DEF ,(uint64_t)EFM_FRAME_SECOND_THRES_DEF)))
		{
			efm_err_frame_second_window_set(pif->pefm, 
					EFM_FRAME_SECOND_WINDOW_DEF, EFM_FALSE);
			ret = efm_err_frame_second_threshold_set(pif->pefm, 
					EFM_FRAME_SECOND_THRES_DEF, EFM_FALSE);
		}

	}
	else
	{

		if(!(efm_send_link_monitor_data_to_hal(pif->pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, 
						(uint64_t)EFM_FRAME_SECOND_WINDOW_DEF ,(uint64_t)pif->pefm->local_link_info.err_frame_sec_sum_threshold)))
		{
			ret = efm_err_frame_second_window_set(pif->pefm, 
					EFM_FRAME_SECOND_WINDOW_DEF, EFM_FALSE);
		}

	}


	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
    return ret;
}

DEFUN (efm_link_event_notify_set,
		efm_link_event_notify_set_cmd,
		"efm event-notify (symbol |frame|frame-period|frame-second|dying-gasp|critical-event)",
		EFM_STR
		"Event notify\n"
		"Error symbol\n"
		"Error frame\n"
		"Error frame period\n"
		"Error frame second\n"
		"Dying gasp\n"
		"Critical event\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_event_notify_set(pif->pefm, argv[0], EFM_TRUE);
	if (ret)
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return EFM_FAILURE;
	}

	return CMD_SUCCESS;
}

DEFUN (no_efm_link_event_notify,
		no_efm_link_event_notify_cmd,
		"no efm event-notify (symbol|frame|frame-period|frame-second|dying-gasp|critical-event)",
		NO_STR
		EFM_STR
		"Event notify\n"
		"Error symbol\n"
		"Error frame\n"
		"Error frame period\n"
		"Error frame second\n"
		"Dying gasp\n"
		"Critical event\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
         
	ret = efm_event_notify_set(pif->pefm, argv[0], EFM_FALSE);	 
	if (ret)
	{
		vty_error_out (vty, "%%Invalid EFM parameter%s", VTY_NEWLINE);
		return EFM_FAILURE;
	}

	return CMD_SUCCESS;
}

DEFUN (efm_uni_dir,
		efm_uni_dir_cmd,
		"efm unidirectional-link support",
		EFM_STR
		EFM_UNI_DIR_STR
		"Configure undirectional link Support\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif->pefm->if_index= if_index;
	ret = efm_remote_unidir_link_set (pif->pefm, EFM_TRUE);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_uni_dir,
		no_efm_uni_dir_cmd,
		"no efm unidirectional-link support",
		NO_STR
		EFM_STR
		EFM_UNI_DIR_STR
		"Configure undirectional link Support\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_remote_unidir_link_set (pif->pefm, EFM_FALSE);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_link_timer,
		efm_link_timer_cmd,
		"efm timeout <2-30>",
		EFM_STR
		"Local link lost timer\n"
		"Timer value (2-30)sec\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint32_t secs = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	secs = atoi(argv[0]);

	if((secs < EFM_TIMEOUT_MIN) ||(secs > EFM_TIMEOUT_MAX))
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <2-30>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_set_link_timer(pif->pefm, secs);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_link_timer,
		no_efm_link_timer_cmd,
		"no efm timeout",
		NO_STR
		EFM_STR
		"Local link lost timer\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_set_link_timer (pif->pefm, EFM_LINK_TIMER);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (efm_pdu_timer,
		efm_pdu_timer_cmd,
		"efm rate <1-10>",
		EFM_STR
		"Set the time for pdu-timer\n"
		"Timer value (1-10)sec\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;
	uint8_t secs = 0;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	secs = atoi(argv[0]);

	if((secs < EFM_PDU_TIMER_MIN) ||(secs > EFM_PDU_TIMER_MAX))
	{ 
		vty_error_out(vty,"%%Invalid EFM parameter range, interval is <1-10>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_pdu_timer_set(pif->pefm, secs);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (no_efm_pdu_timer,
		no_efm_pdu_timer_cmd,
		"no efm rate",
		NO_STR
		EFM_STR
		"Set the time for pdu-timer\n")
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = efm_pdu_timer_set(pif->pefm, EFM_PDU_TIMER);

	if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	
	return ret;
}

DEFUN (clear_efm_statistics,
		clear_efm_statsitics_cmd,
		"clear efm statistics",
		CLEAR_STR
		EFM_STR
		EFM_STATS_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;
	s_int32_t ret = EFM_SUCCESS;

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if (NULL == pif->pefm)
	{
		vty_error_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    ret = efm_clear_statistics(pif->pefm);
   if(ret)
    {
	vty_error_out(vty,errcode_get_string(ret),VTY_NEWLINE);
	return EFM_FAILURE;
    }
	return CMD_SUCCESS;
}

DEFUN (show_efm_statistics,
		show_efm_statsitics_cmd,
		"show efm statistics",
		CLEAR_STR
		EFM_STR
		EFM_STATS_STR)
{      
        uint32_t ifindex = 0;
        struct l2if *pl2_if = NULL;
		
    if(CONFIG_NODE == vty->node)
        	{
        (void)efm_show_statistics(vty);
        	}
	else
	{
		ifindex = (uint32_t)vty->index;
		pl2_if = l2if_lookup(ifindex);
        if(NULL == pl2_if || NULL == pl2_if->pefm )
		{
			vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);	
                   	return CMD_WARNING;
		}
		else
        {
            (void)efm_show_if_statistics(pl2_if->pefm, vty);
        }
	}
	
	/*~{TZ9~O#1mKQQ0~}ifindex~{6TS&=Z5c~}*/
	

	return CMD_SUCCESS;
}


DEFUN (show_efm_ethernet_statistics,
		show_efm_ethernet_statsitics_cmd,
		"show efm statistics interface gigabitethernet USP",
		CLEAR_STR
		EFM_STR
		EFM_STATS_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

	if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[0]);
    if(0 == if_index)   // 0 means interface not exist
	{
		zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

    if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

    (void)efm_show_if_statistics(pif->pefm, vty);

	return CMD_SUCCESS;
}


DEFUN (show_efm_trunk_statistics,
		show_efm_trunk_statsitics_cmd,
		"show efm statistics interface trunk TRUNK",
		CLEAR_STR
		EFM_STR
		EFM_STATS_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

	if(trunk_lookup(atoi(argv[0])) == NULL)
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[0]);
    if(0 == if_index)
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	} 

    (void)efm_show_if_statistics(pif->pefm, vty);

	return CMD_SUCCESS;
}



DEFUN (show_efm_interface,
		show_efm_interface_cmd,
		"show efm",
		SHOW_STR
		EFM_STR)
{
        uint32_t ifindex = 0;
        struct l2if * pl2_if = NULL;
    if(CONFIG_NODE == vty->node)
	{
        (void)efm_show_interface(vty);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		pl2_if = l2if_lookup(ifindex);
        if(pl2_if == NULL || NULL == pl2_if->pefm)
		{
			vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);	
                   	return CMD_WARNING;
		}
		else
		{
            (void)efm_show_if_interface(pl2_if->pefm, vty);
		}
	}
	return CMD_SUCCESS;
}


DEFUN (show_efm_interface_ethernet,
		show_efm_interface_ethernet_cmd,
		"show efm interface gigabitethernet USP",
		SHOW_STR
		EFM_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

	if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[0]);
    if(0 == if_index)   // 0 means interface not exist
	{
		zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

    (void)efm_show_if_interface(pif->pefm, vty);

	return CMD_SUCCESS;
}


DEFUN (show_efm_interface_trunk,
		show_efm_interface_trunk_cmd,
		"show efm interface trunk TRUNK",
		SHOW_STR
		EFM_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

    if(NULL ==  trunk_lookup(atoi(argv[0])))
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[0]);
	if(if_index == 0) 
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	} 

    (void)efm_show_if_interface(pif->pefm, vty);

	return CMD_SUCCESS;
}



DEFUN (show_efm_discovery,
		show_efm_discovery_cmd,
		"show efm discovery",
		SHOW_STR
		EFM_STR
		EFM_DISCOVERY_STR)
{
	
          uint32_t ifindex = 0;
        struct l2if * pl2_if = NULL;
    if(CONFIG_NODE == vty->node)
	{
        (void)efm_show_discovery(vty);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		pl2_if = l2if_lookup(ifindex);
        if(NULL == pl2_if|| NULL == pl2_if->pefm)
		{
			vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);	
                   	return CMD_WARNING;
		}
		else
		{
            (void)efm_show_if_discovery(pl2_if->pefm, vty);
		}
	}
	return CMD_SUCCESS;
}


DEFUN (show_efm_ethernet_discovery,
		show_efm_ethernet_discovery_cmd,
		"show efm discovery interface gigabitethernet USP",
		SHOW_STR
		EFM_STR
		EFM_DISCOVERY_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

	if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[0]);
    if(0 == if_index)   // 0 means interface not exist
	{
		zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);
	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

    (void)efm_show_if_discovery(pif->pefm, vty);

	return CMD_SUCCESS;

}


DEFUN (show_efm_trunk_discovery,
		show_efm_trunk_discovery_cmd,
		"show efm discovery interface trunk TRUNK",
		SHOW_STR
		EFM_STR
		EFM_DISCOVERY_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

    if(NULL == trunk_lookup(atoi(argv[0])))
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[0]);
    if(0 == if_index)
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	} 

    (void)efm_show_if_discovery(pif->pefm, vty);

	return CMD_SUCCESS;
}



DEFUN (show_efm_status,
		show_efm_status_cmd,
		"show efm status",
		SHOW_STR
		EFM_STR
		EFM_STATUS_STR)
{
	
	   uint32_t ifindex = 0;
        struct l2if * pl2_if = NULL;
    if( CONFIG_NODE == vty->node)
	{
        (void)efm_show_status(vty);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		pl2_if = l2if_lookup(ifindex);
        if(pl2_if == NULL || NULL == pl2_if->pefm)
		{
			vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);	
                   	return CMD_WARNING;
		}
		else
		{
            (void)efm_show_if_status(pl2_if->pefm, vty);
		}
	}
	return CMD_SUCCESS;
}

DEFUN (show_efm_ethernet_status,
		show_efm_ethernet_status_cmd,
		"show efm status interface gigabitethernet USP",
		SHOW_STR
		EFM_STR
		EFM_STATUS_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

	if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[0]);
    if(0 == if_index)   // 0 means interface not exist
	{
		zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);
	if(NULL == pif ||NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

    (void)efm_show_if_status(pif->pefm, vty);

	return CMD_SUCCESS;
}


DEFUN (show_efm_trunk_status,
		show_efm_trunk_status_cmd,
		"show efm status interface trunk TRUNK",
		SHOW_STR
		EFM_STR
		EFM_STATUS_STR
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

    if(NULL == trunk_lookup(atoi(argv[0])))
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[0]);
    if(0 == if_index)
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);
	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	} 

    (void)efm_show_if_status(pif->pefm, vty);

	return CMD_SUCCESS;
}



DEFUN (show_efm_event_log,
		show_efm_event_log_cmd,
		"show efm eventlog",
		SHOW_STR
		EFM_STR
		"EFM event logs\n")
{
	
           uint32_t ifindex = 0;
        struct l2if * pl2_if = NULL;
    if(CONFIG_NODE == vty->node)
	{
        (void)efm_show_event_log(vty);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		pl2_if = l2if_lookup(ifindex);
        if(NULL == pl2_if|| NULL == pl2_if->pefm)
		{
			vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);	
                   	return CMD_WARNING;
		}
		else
		{
            (void)efm_show_if_event_log(pl2_if->pefm, vty);
		}
	}
	return CMD_SUCCESS;
}


DEFUN (show_efm_ethernet_event_log,
		show_efm_ethernet_event_log_cmd,
		"show efm eventlog interface gigabitethernet USP",
		SHOW_STR
		EFM_STR
		"EFM event logs\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

	if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[0]);
    if(0 == if_index)   // 0 means interface not exist
	{
		zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

	if(NULL == pif || NULL == pif->pefm)
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

    (void)efm_show_if_event_log(pif->pefm, vty);

	return CMD_SUCCESS;
}

DEFUN (show_efm_trunk_event_log,
		show_efm_trunk_event_log_cmd,
		"show efm eventlog interface trunk TRUNK",
		SHOW_STR
		EFM_STR
		"EFM event logs\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	uint32_t if_index = 0;	
	struct l2if *pif = NULL;

    if(NULL == trunk_lookup(atoi(argv[0])))
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[0]);
	if(if_index == 0) 
	{
		vty_error_out(vty, "%%ERROR: Trunk not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif = l2if_lookup(if_index);

	if(NULL == pif || NULL == pif->pefm )
	{
		vty_error_out(vty, "%%EFM not enabled%s", VTY_NEWLINE);
		return CMD_WARNING;
	} 

    (void)efm_show_if_event_log(pif->pefm, vty);

	return CMD_SUCCESS;
}



void efm_cmd_init(void)
{
	/* efm  enable/disable commands */
	install_element (PHYSICAL_IF_NODE, &efm_enable_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_efm_enable_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &efm_agent_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &efm_no_agent_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &efm_agent_hostname_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_efm_agent_hostname_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &efm_agent_ip_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_efm_agent_ip_cmd,CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &efm_agent_id_cmd,CMD_SYNC);
	
//	install_element(PHYSICAL_IF_NODE, &efm_agent_pdu_timer_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_mode_active_cmd,CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &efm_mode_passive_cmd);

	/* efm remote loopback  enable/disable commands */
	install_element (PHYSICAL_IF_NODE, &efm_rem_loopback_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_rem_loopback_time_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_rem_loopback_st_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_rem_loopback_ts_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_remote_loopback_start_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_remote_loopback_start_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE,&efm_remote_loopback_mac_swap_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE,&efm_remote_loopback_no_mac_swap_cmd,CMD_SYNC);

	/* efm Link Monitor related commands */
	install_element (PHYSICAL_IF_NODE, &efm_link_monitor_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_link_monitor_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_link_monitor_on_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_link_monitor_on_cmd,CMD_SYNC);

	/*efm event log commands*/
	install_element (PHYSICAL_IF_NODE, &efm_event_log_size_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_event_log_size_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &clear_efm_event_log_cmd,CMD_SYNC);

	/*efm link monitor for err-frame and err-symbol*/
	install_element (PHYSICAL_IF_NODE, &efm_sym_period_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_sym_period_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_sym_period_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_sym_period_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_err_frame_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_err_frame_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_err_frame_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_err_frame_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_err_frame_period_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_err_frame_period_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_err_frame_period_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_err_frame_period_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_err_frame_second_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_err_frame_second_event_tw_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_err_frame_second_event_wt_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_err_frame_second_event_tw_cmd,CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &efm_link_event_notify_set_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_link_event_notify_cmd,CMD_SYNC);

	/*efm unidirection link support commands*/
	install_element (PHYSICAL_IF_NODE, &efm_uni_dir_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_uni_dir_cmd,CMD_SYNC);

	/* timers commands. */
	install_element (PHYSICAL_IF_NODE, &efm_link_timer_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_link_timer_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &efm_pdu_timer_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_efm_pdu_timer_cmd,CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &clear_efm_statsitics_cmd,CMD_SYNC);
	/* display commands */
	install_element (PHYSICAL_IF_NODE, &show_efm_statsitics_cmd,CMD_LOCAL);
	//install_element (PHYSICAL_IF_NODE, &show_efm_ethernet_statsitics_cmd,CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &show_efm_trunk_statsitics_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &show_efm_interface_cmd,CMD_LOCAL);
	//install_element (PHYSICAL_IF_NODE, &show_efm_interface_ethernet_cmd,CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &show_efm_interface_trunk_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &show_efm_discovery_cmd,CMD_LOCAL);
	//install_element (PHYSICAL_IF_NODE, &show_efm_ethernet_discovery_cmd,CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &show_efm_trunk_discovery_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &show_efm_status_cmd,CMD_LOCAL);
	//install_element (PHYSICAL_IF_NODE, &show_efm_ethernet_status_cmd,CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &show_efm_trunk_status_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &show_efm_event_log_cmd,CMD_LOCAL);
	//install_element (PHYSICAL_IF_NODE, &show_efm_ethernet_event_log_cmd,CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &show_efm_trunk_event_log_cmd,CMD_SYNC);

	install_element (CONFIG_NODE, &show_efm_statsitics_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_efm_ethernet_statsitics_cmd,CMD_LOCAL);
	//install_element (CONFIG_NODE, &show_efm_trunk_statsitics_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &show_efm_interface_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_efm_interface_ethernet_cmd,CMD_LOCAL);
	//install_element (CONFIG_NODE, &show_efm_interface_trunk_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &show_efm_discovery_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_efm_ethernet_discovery_cmd,CMD_LOCAL);
	//install_element (CONFIG_NODE, &show_efm_trunk_discovery_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &show_efm_status_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_efm_ethernet_status_cmd,CMD_LOCAL);
	//install_element (CONFIG_NODE, &show_efm_trunk_status_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &show_efm_event_log_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_efm_ethernet_event_log_cmd,CMD_LOCAL);
	//install_element (CONFIG_NODE, &show_efm_trunk_event_log_cmd,CMD_SYNC);

	#if 0
	/* efm	enable/disable commands */
	install_element (TRUNK_IF_NODE, &efm_enable_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_enable_cmd,CONFIG_LEVE_5);

	/* efm mode set commands */
	install_element (TRUNK_IF_NODE, &efm_mode_active_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &efm_mode_passive_cmd);

	/* efm remote loopback	enable/disable commands */
	install_element (TRUNK_IF_NODE, &efm_rem_loopback_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_rem_loopback_time_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_rem_loopback_st_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_rem_loopback_ts_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_remote_loopback_start_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_remote_loopback_start_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE,&efm_remote_loopback_mac_swap_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE,&efm_remote_loopback_no_mac_swap_cmd,CONFIG_LEVE_5);

	/* efm Link Monitor related commands */
	install_element (TRUNK_IF_NODE, &efm_link_monitor_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_link_monitor_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_link_monitor_on_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_link_monitor_on_cmd,CONFIG_LEVE_5);

	/*efm event log commands*/
	install_element (TRUNK_IF_NODE, &efm_event_log_size_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_event_log_size_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &clear_efm_event_log_cmd,CONFIG_LEVE_5);

	/*efm link monitor for err-frame and err-symbol*/
	install_element (TRUNK_IF_NODE, &efm_sym_period_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_sym_period_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_sym_period_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_sym_period_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_err_frame_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_err_frame_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_err_frame_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_err_frame_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_err_frame_period_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_err_frame_period_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_err_frame_period_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_err_frame_period_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_err_frame_second_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_err_frame_second_event_tw_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_err_frame_second_event_wt_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_err_frame_second_event_tw_cmd,CONFIG_LEVE_5);

	install_element (TRUNK_IF_NODE, &efm_link_event_notify_set_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_link_event_notify_cmd,CONFIG_LEVE_5);

	/*efm unidirection link support commands*/
	install_element (TRUNK_IF_NODE, &efm_uni_dir_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_uni_dir_cmd,CONFIG_LEVE_5);

	/* timers commands. */
	install_element (TRUNK_IF_NODE, &efm_link_timer_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_link_timer_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &efm_pdu_timer_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &no_efm_pdu_timer_cmd,CONFIG_LEVE_5);

	install_element (TRUNK_IF_NODE, &clear_efm_statsitics_cmd,CONFIG_LEVE_5);
	/* display commands */
	install_element (TRUNK_IF_NODE, &show_efm_statsitics_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_ethernet_statsitics_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_trunk_statsitics_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &show_efm_interface_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_interface_ethernet_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_interface_trunk_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &show_efm_discovery_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_ethernet_discovery_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_trunk_discovery_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &show_efm_status_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_ethernet_status_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_trunk_status_cmd,CONFIG_LEVE_5);
	install_element (TRUNK_IF_NODE, &show_efm_event_log_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_ethernet_event_log_cmd,CONFIG_LEVE_5);
	//install_element (TRUNK_IF_NODE, &show_efm_trunk_event_log_cmd,CONFIG_LEVE_5);
	#endif
}


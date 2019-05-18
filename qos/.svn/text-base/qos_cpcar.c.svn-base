#include <stdlib.h>
#include <string.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/ifm_common.h>
#include <lib/qos_common.h>
#include "qos_cpcar.h"
#include <qos/qos_main.h>
#include "qos_msg.h"


struct qos_cpcar_outband outband_config;
struct qos_cpcar gcpcar;
struct qos_cpcar_queue  cpcar_queue[17];


void qos_cpcar_init ( void )
{
	memset(&gcpcar ,0 ,sizeof(struct qos_cpcar));
	memset(cpcar_queue ,0 ,sizeof(struct qos_cpcar_queue)*17);
	memset(&outband_config ,0 ,sizeof(struct qos_cpcar_outband));
	outband_config.arp_pps 	= CPCAR_OUTBAND_ARP_PPS;
	outband_config.ipv6_pps = CPCAR_OUTBAND_IPV6_PPS;
	outband_config.dhcp_pps = CPCAR_OUTBAND_DHCP_PPS;
	outband_config.icmp_pps = CPCAR_OUTBAND_ICMP_PPS;
	outband_config.snmp_pps = CPCAR_OUTBAND_SNMP_PPS;
	outband_config.ssh_pps 	= CPCAR_OUTBAND_SSH_PPS;
	outband_config.tftp_pps = CPCAR_OUTBAND_TFTP_PPS;
	outband_config.telnet_pps = CPCAR_OUTBAND_TELNET_PPS;
	outband_config.ipother_pps 	= CPCAR_OUTBAND_IP_PPS;
}

int qos_cpcar_set(uint32_t ifindex, uint16_t pps, enum CPCAR_TYPE type)
{
	int ret;
#if 0
	ret = ipc_send_hal_wait_ack( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,type);
#else
	ret = qos_ipc_send_and_wait_ack( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,type);
#endif

	if(ret)
    {
		QOS_LOG_ERR("ipc_send_hal_wait_ack failed.\n");
		return ERRNO_HAL;
    }

	switch(type)
	{
		case CPCAR_TYPE_ARP:
			gcpcar.arp_pps = pps;
			break;
		case CPCAR_TYPE_ICMP:
			gcpcar.icmp_pps = pps;
			break;
		case CPCAR_TYPE_DHCP:
			gcpcar.dhcp_pps = pps;
			break;
		case CPCAR_TYPE_IGMP:
			gcpcar.igmp_pps = pps;
			break;
		case CPCAR_TYPE_TCP:
			gcpcar.tcp_pps = pps;
			break;
		case CPCAR_TYPE_ALL:
			gcpcar.all_pps = pps;
			break;
		default:
           return ERRNO_FAIL;
	}
	return ERRNO_SUCCESS;
}


int qos_cpcar_set_queue(uint8_t id, uint16_t pps)
{
	int ret;
#if 0
	ret = ipc_send_hal_wait_ack( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,id);
#else
	ret = qos_ipc_send_and_wait_ack( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,id);
#endif

	if(ret)
    {
		QOS_LOG_ERR("ipc_send_hal_wait_ack failed.\n");
		return ERRNO_HAL;
    }

	cpcar_queue[id].cosq_id = id;
	cpcar_queue[id].cosq_pps_config = pps;

	return ERRNO_SUCCESS;
}


static int qos_cpcar_config_write (struct vty *vty)
{
	int id;

	if(gcpcar.arp_pps != 0)
	{
		vty_out(vty, "cpu-car arp %d%s", gcpcar.arp_pps, VTY_NEWLINE);
	}
	if(gcpcar.dhcp_pps != 0)
	{
		vty_out(vty, "cpu-car dhcp %d%s", gcpcar.dhcp_pps, VTY_NEWLINE);
	}
	if(gcpcar.icmp_pps != 0)
	{
		vty_out(vty, "cpu-car icmp %d%s", gcpcar.icmp_pps, VTY_NEWLINE);
	}
	if(gcpcar.tcp_pps != 0)
	{
		vty_out(vty, "cpu-car tcp %d%s", gcpcar.tcp_pps, VTY_NEWLINE);
	}
	if(gcpcar.igmp_pps != 0)
	{
		vty_out(vty, "cpu-car igmp %d%s", gcpcar.igmp_pps, VTY_NEWLINE);
	}
	if(gcpcar.all_pps != 0)
	{
		vty_out(vty, "cpu-car all %d%s", gcpcar.all_pps, VTY_NEWLINE);
	}

	for(id = 0;id < 16; id++)
	{
		if(((id >= 0)&&(id <= 5))||((id >= 11)&&(id <= 15)))
		{
			if(cpcar_queue[id].cosq_pps_config != 0)
			{
				vty_out(vty, "cpu-car queue %d %d%s", id,cpcar_queue[id].cosq_pps_config,VTY_NEWLINE);
			}
		}
	}

	if(outband_config.arp_pps != CPCAR_OUTBAND_ARP_PPS)
	{
		vty_out(vty, "cpu-car outband arp pps %d%s", outband_config.arp_pps, VTY_NEWLINE);
	}
	if(outband_config.ipv6_pps != CPCAR_OUTBAND_IPV6_PPS)
	{
		vty_out(vty, "cpu-car outband ipv6 pps %d%s", outband_config.ipv6_pps, VTY_NEWLINE);
	}
	if(outband_config.dhcp_pps != CPCAR_OUTBAND_DHCP_PPS)
	{
		vty_out(vty, "cpu-car outband dhcp pps %d%s", outband_config.dhcp_pps, VTY_NEWLINE);
	}
	if(outband_config.snmp_pps != CPCAR_OUTBAND_SNMP_PPS)
	{
		vty_out(vty, "cpu-car outband snmp pps %d%s", outband_config.snmp_pps, VTY_NEWLINE);
	}
	if(outband_config.icmp_pps != CPCAR_OUTBAND_ICMP_PPS)
	{
		vty_out(vty, "cpu-car outband icmp pps %d%s", outband_config.icmp_pps, VTY_NEWLINE);
	}
	if(outband_config.ssh_pps != CPCAR_OUTBAND_SSH_PPS)
	{
		vty_out(vty, "cpu-car outband ssh pps %d%s", outband_config.ssh_pps, VTY_NEWLINE);
	}
	if(outband_config.tftp_pps != CPCAR_OUTBAND_TFTP_PPS)
	{
		vty_out(vty, "cpu-car outband tftp pps %d%s", outband_config.tftp_pps, VTY_NEWLINE);
	}
	if(outband_config.telnet_pps != CPCAR_OUTBAND_TELNET_PPS)
	{
		vty_out(vty, "cpu-car outband telnet pps %d%s", outband_config.telnet_pps, VTY_NEWLINE);
	}
	if(outband_config.ipother_pps != CPCAR_OUTBAND_IP_PPS)
	{
		vty_out(vty, "cpu-car outband ip-other pps %d%s", outband_config.ipother_pps, VTY_NEWLINE);
	}


	return ERRNO_SUCCESS;
}


static struct cmd_node qos_cpcar_node =
{
	QOS_CPCAR_NODE,
	"%s(config-cpu-car)# ",
	1,
};


DEFUN (qos_cpcar_mode,
	qos_cpcar_mode_cmd,
	"cpu-car",
	"Cpu-car command\n")
{
	vty->node = QOS_CPCAR_NODE;
	return CMD_SUCCESS;
}


DEFUN(qos_cpu_car,
		qos_cpu_car_cmd,
		"cpu-car (arp|dhcp|icmp|tcp|igmp|all) <1-65535>",
		"Cpu car config\n"
		"ARP protocol pps enter cpu queue 8\n"
		"DHCP protocol pps enter cpu queue 6\n"
		"ICMP protocol pps enter cpu queue 9\n"
		"TCP protocol pps enter cpu queue 10\n"
		"IGMP protocol pps enter cpu queue 7\n"
		"Global all cpu queue pps\n"
		"CPU queue pps value <1-65535>\n")
{
	enum CPCAR_TYPE type = 0;
	uint16_t pps;
	int ret;

	if( strncmp ( argv[0], "arp", 2 ) == 0)
	{
		type = CPCAR_TYPE_ARP;
	}
	else if(strncmp ( argv[0], "dhcp", 1 ) == 0)
	{
		type = CPCAR_TYPE_DHCP;
	}
	else if(strncmp ( argv[0], "icmp", 2 ) == 0)
	{
		type = CPCAR_TYPE_ICMP;
	}
	else if(strncmp ( argv[0], "tcp", 1 ) == 0)
	{
		type = CPCAR_TYPE_TCP;
	}
	else if(strncmp ( argv[0], "igmp", 2 ) == 0)
	{
		type = CPCAR_TYPE_IGMP;
	}
	else if(strncmp ( argv[0], "all", 2 ) == 0)
	{
		type = CPCAR_TYPE_ALL;
	}

	pps = (uint16_t)atoi(argv[1]);

	ret = qos_cpcar_set (0, pps, type);
	if(ret != 0)
	{
		vty_error_out ( vty, "qos cpcar set,please check out!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(no_qos_cpu_car,
		no_qos_cpu_car_cmd,
		"no cpu-car (arp|dhcp|icmp|tcp|igmp|all)",
		NO_STR
		"Qos cpu car\n"
		"ARP protocol pps\n"
		"DHCP protocol pps\n"
		"ICMP protocol pps\n"
		"TCP protocol pps\n"
		"IGMP protocol pps\n"
		"Global cpu queue pps\n")
{
	enum CPCAR_TYPE type = 0;
	int ret = 0;

	if( strncmp ( argv[0], "arp", 2 ) == 0)
	{
		type = CPCAR_TYPE_ARP;
	}
	else if(strncmp ( argv[0], "dhcp", 1 ) == 0)
	{
		type = CPCAR_TYPE_DHCP;
	}
	else if(strncmp ( argv[0], "icmp", 2 ) == 0)
	{
		type = CPCAR_TYPE_ICMP;
	}
	else if(strncmp ( argv[0], "tcp", 1 ) == 0)
	{
		type = CPCAR_TYPE_TCP;
	}
	else if(strncmp ( argv[0], "igmp", 2 ) == 0)
	{
		type = CPCAR_TYPE_IGMP;
	}
	else if(strncmp ( argv[0], "all", 2 ) == 0)
	{
		type = CPCAR_TYPE_ALL;
	}

	ret = qos_cpcar_set (0, 0, type);
	if(ret != 0)
	{
		vty_error_out ( vty, "qos cpcar set fail,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}



DEFUN(qos_cpu_car_queue,
		qos_cpu_car_queue_cmd,
		"cpu-car queue <0-15> <1-65535>",
		"Cpu car config\n"
		"Cpu queue\n"
		"Cpu queue id <0-15> except queue <6-10>\n"
		"CPU queue pps value <1-65535>\n")
{
	int ret;
	uint8_t cosq_id;
	uint16_t pps;

	cosq_id = (uint8_t)atoi(argv[0]);
	if((cosq_id >= 6)&&(cosq_id <= 10))
	{
		vty_out ( vty, "Error:This command does not support configure 6 to 10 queues,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pps = (uint16_t)atoi(argv[1]);

	ret = qos_cpcar_set_queue(cosq_id,pps);
	if(ret != 0)
	{
		vty_error_out ( vty, "qos cpcar set queue config fail,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(no_qos_cpu_car_queue,
		no_qos_cpu_car_queue_cmd,
		"no cpu-car queue <0-15>",
		NO_STR
		"Cpu car config\n"
		"Cpu queue\n"
		"Cpu queue id <0-15> except queue <6-10>\n")
{
	int ret;
	uint8_t cosq_id;

	cosq_id = (uint8_t)atoi(argv[0]);
	if((cosq_id >= 6)&&(cosq_id <= 10))
	{
		vty_out ( vty, "Error:This command does not support configure 6 to 10 queues,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = qos_cpcar_set_queue(cosq_id,0);
	if(ret != 0)
	{
		vty_error_out ( vty, "qos cpcar set queue fail,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN  (show_qos_cpu_car,
		show_qos_cpu_car_cmd,
       "show cpu-car config",
       SHOW_STR
       "Cpu car pps\n"
       "Cpu car pps config\n")
{
	qos_cpcar_config_write (vty);

	return CMD_SUCCESS;
}

DEFUN  (show_qos_cpu_car_queue,
		show_qos_cpu_car_queue_cmd,
       "show cpu-car queue",
       SHOW_STR
       "Qos Cpu car \n"
       "Cpu queue config add statistics\n")
{
	uint8_t id = 0;
	struct qos_cpcar_queue  *qos_cpcar_queue = NULL;
	struct qos_cpcar_queue  cpu_queue[17];
	struct ipc_mesg_n* pMsgRep = NULL;
	
#if 0
	qos_cpcar_queue = ipc_send_hal_wait_reply( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_GET,0);
	if(qos_cpcar_queue == NULL)
    {
		vty_error_out ( vty, "cannot get cpu queue statistics ,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
    }
#else
	pMsgRep = ipc_sync_send_n2( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_GET, 0, 0);
	if (NULL == pMsgRep)
    {
    	vty_error_out ( vty, "cannot get cpu queue statistics ,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
    }
    qos_cpcar_queue = (struct qos_cpcar_queue *)(pMsgRep->msg_data);
#endif
	memset(cpu_queue ,0 ,sizeof(struct qos_cpcar_queue)*17);
	memcpy(cpu_queue, qos_cpcar_queue, sizeof(struct qos_cpcar_queue)*17);

	cpcar_queue[16].cosq_pps = cpu_queue[16].cosq_pps;
	cpcar_queue[16].rx_packets = cpu_queue[16].rx_packets;

	vty_out ( vty, "All cpu queue pps %-5d ,  TotPkts  %-10d %s",
		 	cpcar_queue[16].cosq_pps,cpcar_queue[16].rx_packets,VTY_NEWLINE );
	vty_out ( vty, "-------------------------------------------------------%s",VTY_NEWLINE );

	for(id = 0;id < 16;id++)
	{
		cpcar_queue[id].cosq_id = cpu_queue[id].cosq_id;
		cpcar_queue[id].cosq_pps = cpu_queue[id].cosq_pps;
		cpcar_queue[id].rx_packets = cpu_queue[id].rx_packets;

		vty_out ( vty, " Cpu queue %-2d :   PPS  %-5d  ,  TotPkts  %-10d %s",
		 	id, cpcar_queue[id].cosq_pps,cpcar_queue[id].rx_packets,VTY_NEWLINE );
	}

	mem_share_free(pMsgRep, MODULE_ID_QOS);

	return CMD_SUCCESS;
}


DEFUN  (qos_cpu_car_clear,
		qos_cpu_car_clear_cmd,
       "cpu-car clear {queue <0-15>|all}",
       "Cpu car\n"
       "Statistics clear\n"
       "Qos cpu queue\n"
       "Cpu queue id<0-15>\n"
       "All cpu queue statistics \n")
{
	int ret;
	uint8_t cosq_id;

	if(NULL != argv[0])
	{
		cosq_id = (uint8_t)atoi(argv[0]);
	}
	else
	{
		cosq_id = QOS_CPCAR_QUEUE_GLOBAL;
	}
#if 0
	ret = ipc_send_hal_wait_ack( NULL , 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_CLEAR,cosq_id);
#else
	ret = qos_ipc_send_and_wait_ack( NULL , 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_CLEAR,cosq_id);
#endif

	if(ret)
    {
		vty_error_out ( vty, "cannot clear cpu queue statistics ,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
    }

	return CMD_SUCCESS;
}



DEFUN(qos_cpu_car_outband,
		qos_cpu_car_outband_cmd,
		"cpu-car outband (arp|ipv6|dhcp|tftp|ssh|telnet|snmp|icmp|ip-other) pps <1-2000>",
		"Cpu car config\n"
		"Outband interface\n"
		"ARP protocol outband pps\n"
		"Ipv6 protocol outband pps\n"
		"Dhcp protocol outband pps\n"
		"Tftp protocol outband pps\n"
		"Ssh protocol outband pps\n"
		"Telnet protocol outband pps\n"
		"Snmp protocol outband pps\n"
		"Icmp protocol outband pps\n"
		"Ip-other outband pps\n"
		"Pps\n"
		"Outband interface pps value <1-2000>\n")
{
	enum CPCAR_TYPE type = 0;
	uint16_t pps;
	int ret;

	pps = (uint16_t)atoi(argv[1]);

	if( strncmp ( argv[0], "arp", 1 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_ARP;
		outband_config.arp_pps = pps;
	}
	else if(strncmp ( argv[0], "ipv6", 3 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_IPV6;
		outband_config.ipv6_pps = pps;
	}
	else if(strncmp ( argv[0], "dhcp", 1 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_DHCP;
		outband_config.dhcp_pps = pps;
	}
	else if(strncmp ( argv[0], "tftp", 2 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_TFTP;
		outband_config.tftp_pps = pps;
	}
	else if(strncmp ( argv[0], "ssh", 2 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_SSH;
		outband_config.ssh_pps = pps;
	}
	else if(strncmp ( argv[0], "telnet", 2 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_TELNET;
		outband_config.telnet_pps = pps;
	}
	else if(strncmp ( argv[0], "snmp", 2 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_SNMP;
		outband_config.snmp_pps = pps;
	}
	else if(strncmp ( argv[0], "icmp", 2 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_ICMP;
		outband_config.icmp_pps = pps;
	}
	else if(strncmp ( argv[0], "ip-other", 2 ) == 0)
	{
		type = CPCAR_TYPE_OUTBAND_IP_OTHER;
		outband_config.ipother_pps = pps;
	}
#if 0
	ret = ipc_send_hal( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,type);
#else
	ret = ipc_send_msg_n2( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,type);
#endif
	if(ret)
    {
    	vty_error_out(vty, "ipc send hal fail!%s", VTY_NEWLINE);
		return CMD_WARNING;
    }

	return CMD_SUCCESS;
}


DEFUN(no_qos_cpu_car_outband,
		no_qos_cpu_car_outband_cmd,
		"no cpu-car outband (arp|ipv6|dhcp|tftp|ssh|telnet|snmp|icmp|ip-other)",
		NO_STR
		"Cpu car config\n"
		"Outband interface\n"
		"ARP protocol outband initial pps setting 300\n"
		"Ip protocol outband initial pps setting 3000\n"
		"Ipv6 protocol outband initial pps setting 300\n"
		"Dhcp protocol outband initial pps setting 200\n"
		"Tftp protocol outband initial pps setting 500\n"
		"Ssh protocol outband initial pps setting 500\n"
		"Telnet protocol outband initial pps setting 500\n"
		"Snmp protocol outband initial pps setting 500\n"
		"Icmp protocol outband initial pps setting 200\n"
		"Ip-other outband pps initial pps setting 500\n")
{
	enum CPCAR_TYPE type = 0;
	uint16_t pps;
	int ret;

	if( strncmp ( argv[0], "arp", 1 ) == 0)
	{
		pps = CPCAR_OUTBAND_ARP_PPS;
		type = CPCAR_TYPE_OUTBAND_ARP;
		outband_config.arp_pps = CPCAR_OUTBAND_ARP_PPS;

	}
	else if(strncmp ( argv[0], "ipv6", 3 ) == 0)
	{
		pps = CPCAR_OUTBAND_IPV6_PPS;
		type = CPCAR_TYPE_OUTBAND_IPV6;
		outband_config.ipv6_pps = CPCAR_OUTBAND_IPV6_PPS;
	}
	else if(strncmp ( argv[0], "dhcp", 1 ) == 0)
	{
		pps = CPCAR_OUTBAND_DHCP_PPS;
		type = CPCAR_TYPE_OUTBAND_DHCP;
		outband_config.dhcp_pps = CPCAR_OUTBAND_DHCP_PPS;
	}
	else if(strncmp ( argv[0], "tftp", 2 ) == 0)
	{
		pps = CPCAR_OUTBAND_TFTP_PPS;
		type = CPCAR_TYPE_OUTBAND_TFTP;
		outband_config.tftp_pps = CPCAR_OUTBAND_TFTP_PPS;
	}
	else if(strncmp ( argv[0], "ssh", 2 ) == 0)
	{
		pps = CPCAR_OUTBAND_SSH_PPS;
		type = CPCAR_TYPE_OUTBAND_SSH;
		outband_config.ssh_pps = CPCAR_OUTBAND_SSH_PPS;
	}
	else if(strncmp ( argv[0], "telnet", 2 ) == 0)
	{
		pps = CPCAR_OUTBAND_TELNET_PPS;
		type = CPCAR_TYPE_OUTBAND_TELNET;
		outband_config.telnet_pps = CPCAR_OUTBAND_TELNET_PPS;
	}
	else if(strncmp ( argv[0], "snmp", 2 ) == 0)
	{
		pps = CPCAR_OUTBAND_SNMP_PPS;
		type = CPCAR_TYPE_OUTBAND_SNMP;
		outband_config.snmp_pps = CPCAR_OUTBAND_SNMP_PPS;
	}
	else if(strncmp ( argv[0], "icmp", 2 ) == 0)
	{
		pps = CPCAR_OUTBAND_ICMP_PPS;
		type = CPCAR_TYPE_OUTBAND_ICMP;
		outband_config.icmp_pps = CPCAR_OUTBAND_ICMP_PPS;
	}
	else if(strncmp ( argv[0], "ip-other", 2 ) == 0)
	{
		pps = CPCAR_OUTBAND_IP_PPS;
		type = CPCAR_TYPE_OUTBAND_IP_OTHER;
		outband_config.ipother_pps = CPCAR_OUTBAND_IP_PPS;
	}
#if 0
	ret = ipc_send_hal( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,type);
#else
	ret = ipc_send_msg_n2( &pps , sizeof(uint16_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_QOS, QOS_INFO_CPCAR, IPC_OPCODE_UPDATE,type);
#endif
	if(ret)
    {
    	vty_error_out(vty, "ipc send hal fail!%s", VTY_NEWLINE);
		return CMD_WARNING;
    }

	return CMD_SUCCESS;
}


DEFUN  (show_qos_cpu_car_outband,
		show_qos_cpu_car_outband_cmd,
       "show cpu-car outband config",
       SHOW_STR
       "Cpu car pps\n"
       "Outband interface\n"
       "Cpu car pps config\n")
{
	vty_out(vty, "cpu-car outband pps config:%s", VTY_NEWLINE);
	vty_out(vty, " arp      pps:    %d%s", outband_config.arp_pps, VTY_NEWLINE);
	vty_out(vty, " ipv6     pps:    %d%s", outband_config.ipv6_pps, VTY_NEWLINE);
	vty_out(vty, " tftp     pps:    %d%s", outband_config.tftp_pps, VTY_NEWLINE);
	vty_out(vty, " telnet   pps:    %d%s", outband_config.telnet_pps, VTY_NEWLINE);
	vty_out(vty, " ssh      pps:    %d%s", outband_config.ssh_pps, VTY_NEWLINE);
	vty_out(vty, " snmp     pps:    %d%s", outband_config.snmp_pps, VTY_NEWLINE);
	vty_out(vty, " icmp     pps:    %d%s", outband_config.icmp_pps, VTY_NEWLINE);
	vty_out(vty, " dhcp     pps:    %d%s", outband_config.dhcp_pps, VTY_NEWLINE);
	vty_out(vty, " ip-other pps:    %d%s", outband_config.ipother_pps, VTY_NEWLINE);

	return CMD_SUCCESS;
}


void qos_cpcar_cmd_init(void)
{
	install_node(&qos_cpcar_node, qos_cpcar_config_write);
	install_default(QOS_CPCAR_NODE);

	install_element(CONFIG_NODE, &qos_cpu_car_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &qos_cpu_car_outband_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &no_qos_cpu_car_outband_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &show_qos_cpu_car_outband_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &no_qos_cpu_car_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &qos_cpu_car_queue_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &no_qos_cpu_car_queue_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &qos_cpu_car_clear_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &show_qos_cpu_car_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_qos_cpu_car_queue_cmd, CMD_LOCAL);
}



#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/prefix.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/index.h>

#include <mpls/bfd/bfd.h>
#include <mpls/bfd/bfd_session.h>
#include <mpls/bfd/bfd_packet.h>
#include "mpls_if.h"
#include "pw.h"
#include "lspm.h"

struct bfd_global gbfd;

struct cmd_node bfd_session_node =
{ 
	BFD_SESSION_NODE,
	"%s(config-bfd-session)# ",  
	1,
	0,
	0
};

struct cmd_node bfd_template_node =
{ 
	BFD_TEMPLATE_NODE,
	"%s(config-bfd-template)# ",  
	1,
	0,
	0
};

	
DEFUN (bfd_enable,
	enable_bfd_cmd,
	"bfd enable",
	BFD_STR
	"Enable BFD global function\n"
	)
{
	/*使能bfd全局功能*/
	if(vty->node == CONFIG_NODE)
	{
		if(!gbfd.enable)
		{
			gbfd.enable = 1;
			gbfd.multiplier = 3;
			gbfd.priority = 7;
			gbfd.cc_interval = BFD_CC_INTERVAL;
			gbfd.wtr = 30;
			gbfd.debug_fsm = 0;
			gbfd.debug_packet = 0;
			gbfd.up_count = 0;
			gbfd.down_count = 0;
			gbfd.ttl = 255;
			bfd_pkt_register();
		}
	}
	/*接收bfd报文*/
	return CMD_SUCCESS;
}

DEFUN (bfd_disable,
	disable_bfd_cmd,
	"no bfd enable",
	NO_STR
	BFD_STR
	"Disable BFD global function\n"
	)
{

	/*去使能bfd全局功能,删除所有会话*/
	if(vty->node == CONFIG_NODE)
	{
		if(gbfd.enable)
		{
			/*删除所有的会话*/
			bfd_global_disable();
			gbfd.enable = 0;
			gbfd.multiplier = 3;
			gbfd.priority = 7;
			gbfd.cc_interval = BFD_CC_INTERVAL;
			gbfd.wtr = 30;
			gbfd.debug_fsm = 0;
			gbfd.debug_packet = 0;
			gbfd.up_count = 0;
			gbfd.down_count = 0;
			gbfd.ttl = 255;
		}
	}
	/*不再处理bfd报文*/
	return CMD_SUCCESS;
}

DEFUN (bfd_debug,
		bfd_debug_cmd,
		"debug bfd (packet | fsm)",
		DEBUG_STR
		BFD_STR
		"BFD packet\n"
		"BFD session fsm\n"
		)
{
	if(!gbfd.enable)
	{
        vty_warning_out ( vty, "Please enable BFD global function at first%s", VTY_NEWLINE );
        return CMD_WARNING;
	}
	if(strncmp(argv[0], "packet", 6) == 0)
	{
		gbfd.debug_packet = 1;
	}
	else if(strncmp(argv[0], "fsm", 3) == 0)
	{
		gbfd.debug_fsm = 1;
	}
	return CMD_SUCCESS;
}


DEFUN (undo_bfd_debug,
		undo_bfd_debug_cmd,
		"no debug bfd (packet | fsm)",
		NO_STR
		DEBUG_STR
		BFD_STR
		"BFD packet\n"
		"BFD session fsm\n"
		)
{
	if(!gbfd.enable)
	{
        vty_warning_out ( vty, "BFD global function is disabled%s", VTY_NEWLINE );
        return CMD_WARNING;
	}
	if(strncmp(argv[0], "packet", 6) == 0)
	{
		gbfd.debug_packet = 0;
	}
	else if(strncmp(argv[0], "fsm", 3) == 0)
	{
		gbfd.debug_fsm = 0;
	}
	return CMD_SUCCESS;
}


DEFUN (bfd_ip_router_alert_enable,
	bfd_ip_router_alert_enable_cmd,
	"bfd ip-router-alert ",
	BFD_STR
	"Enable BFD ip-router-alert function\n"
	)
{
	gbfd.ip_router_alert = BFD_ENABLE;

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_ip_router_alert_enable,
	undo_bfd_ip_router_alert_enable_cmd,
	"undo bfd ip-router-alert ",
	BFD_STR
	"Disable BFD ip-router-alert function\n"
	)
{
	gbfd.mpls_bfd_enable = BFD_DISABLE;
	gbfd.bfd_h3c_enable = 1;

	return CMD_SUCCESS;
}

DEFUN (mpls_bfd_enable,
	mpls_bfd_enable_cmd,
	"mpls bfd enable ",
	BFD_STR
	"Enable MPLS BFD function\n"
	)
{
	gbfd.mpls_bfd_enable = BFD_MPLS_ENABLE;

	return CMD_SUCCESS;
}

DEFUN (undo_mpls_bfd_enable,
	undo_mpls_bfd_enable_cmd,
	"undo mpls bfd enable ",
	BFD_STR
	"Disable MPLS BFD function\n"
	)
{
	gbfd.mpls_bfd_enable = BFD_MPLS_DISABLE;

	return CMD_SUCCESS;
}

DEFUN (bfd_template_config,
	bfd_template_config_cmd,
	"bfd template NAME",
	BFD_STR
	"Config BFD template\n"
	"Template name\n"
	)
{	/*系统视图下*/
	struct bfd_template *ptemplate = NULL;
	char *pprompt = NULL;
	int ret = 0;

	if(NULL == argv[0])
	{
		vty_warning_out ( vty, "pointer NULL%s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	/*检测会话是否已经存在*/
	ptemplate = bfd_template_lookup((uchar *)argv[0]);
	if(NULL == ptemplate)
	{
		ptemplate = bfd_template_create();
		if(NULL == ptemplate)
		{
			vty_warning_out ( vty, "bfd template create fail%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
	}

	memcpy(ptemplate->template_name, argv[0], strlen(argv[0]));
	ret = bfd_template_add(ptemplate);
	if(0 != ret)
	{
		vty_warning_out ( vty, "bfd template add fail%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	/*进入会话视图*/
	vty->node = BFD_TEMPLATE_NODE;
	pprompt = vty->change_prompt;
	if ( pprompt )
	{
		snprintf ( pprompt, VTY_BUFSIZ, "%s(config-bfd-template-%s)# ", "huahuan", argv[0]);
	}
	vty->index = (void *)ptemplate;
	
	return CMD_SUCCESS;
}

DEFUN (undo_bfd_template_config,
	undo_bfd_template_config_cmd,
	"undo bfd template NAME",
	BFD_STR
	"Delete BFD template\n"
	"Template name\n"
	)
{	/*系统视图下*/
	struct bfd_template *ptem = NULL;

	if(NULL == argv[0])
	{
		vty_warning_out ( vty, "pointer NULL%s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	ptem = bfd_template_lookup((uchar *)argv[0]);
	if(NULL == ptem)
	{
		return CMD_SUCCESS;
	}
	//先对每一条会话判断是否更新模板；
	bfd_update_sess_when_del_template(ptem);

	/*删除bfd template*/
	bfd_template_delete((uchar *)argv[0]);
	
	return CMD_SUCCESS;
}


DEFUN (bfd_detect_multiplier,
	bfd_detect_multiplier_cmd,
	"bfd detect-multiplier <3-50>",
	BFD_STR
	"BFD detect-multiplier function\n"
	"detect_mult <3-50>, default 5\n"
	)
{	/*template视图下*/
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	struct bfd_template *ptemplate = NULL;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t detect_mult = 0, cursor = 0;

	VTY_GET_INTEGER_RANGE ( "det_mult", detect_mult, argv[0], 3, 50 );

	ptemplate = (struct bfd_template *)vty->index;
	
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{	psess->cc_multiplier_cfg = detect_mult;
				bfd_send_poll(psess, event);
			}
		}
	}
	
	ptemplate->detect_mult = detect_mult;

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_detect_multiplier,
	undo_bfd_detect_multiplier_cmd,
	"undo bfd detect-multiplier",
	BFD_STR
	"Restore BFD detect-multiplier function\n"
	)
{	/*template视图下*/
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	struct bfd_template *ptemplate = NULL;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t cursor = 0;
		

	ptemplate = (struct bfd_template *)vty->index;
	
	/*遍历所有会话*/
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{
				psess->cc_multiplier_cfg = BFD_DETECT_MULTIPLIER;
				bfd_send_poll(psess, event);
			}
		}
	}
	
	ptemplate->detect_mult = BFD_DETECT_MULTIPLIER;

	return CMD_SUCCESS;
}




DEFUN (bfd_min_receive_interval,
	bfd_min_receive_interval_cmd,
	"bfd min-receive-interval <3-10000>",
	BFD_STR
	"Bfd min-receive-interval function\n"
	"interval <3-10000> ms,default 1000ms\n"
	)
{	/* template 视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_template *ptemplate = NULL;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cc_interval = 0, cursor = 0;

	VTY_GET_INTEGER_RANGE ( "interval", cc_interval, argv[0], 3, 1000 );

	cc_interval *= 1000;
	ptemplate = (struct bfd_template *)vty->index;

	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{
				psess->cc_interval_cfg = cc_interval;
				if(cc_interval > psess->ptem->cc_tx_interval)
				{
					event = BFD_EVENT_INTERVAL_INC;
				}
				else
				{
					event = BFD_EVENT_INTERVAL_DEC;
				}
				bfd_send_poll(psess, event);
			}
		}
	}
	
	ptemplate->cc_rx_interval = cc_interval;

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_min_receive_interval,
	undo_bfd_min_receive_interval_cmd,
	"undo bfd min-receive-interval",
	BFD_STR
	"Restore Bfd min-receive-interval function\n"
	)
{	/* template 视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_template *ptemplate = NULL;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cursor = 0;
	
	ptemplate = (struct bfd_template *)vty->index;
	
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{
				psess->cc_interval_cfg = BFD_CC_INTERVAL;
				if(BFD_CC_INTERVAL > psess->ptem->cc_tx_interval)
				{
					event = BFD_EVENT_INTERVAL_INC;
				}
				else
				{
					event = BFD_EVENT_INTERVAL_DEC;
				}
				bfd_send_poll(psess, event);
			}
		}
	}
	
	ptemplate->cc_rx_interval = BFD_CC_INTERVAL;

	return CMD_SUCCESS;
}

	
DEFUN (bfd_min_transmit_interval,
	bfd_min_transmit_interval_cmd,
	"bfd min-transmit-interval <3-10000>",
	BFD_STR
	"Bfd min-transmit-interval function\n"
	"interval <3-10000> ms,default 1000ms\n"
	)
{	/* template 视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_template *ptemplate = NULL;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cc_interval = 0, cursor = 0;

	VTY_GET_INTEGER_RANGE ( "interval", cc_interval, argv[0], 3, 10000 );

	cc_interval *= 1000;
	ptemplate = (struct bfd_template *)vty->index;

	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{
				psess->cc_interval_cfg = cc_interval;
				if(cc_interval > psess->ptem->cc_tx_interval)
				{
					event = BFD_EVENT_INTERVAL_INC;
				}
				else
				{
					event = BFD_EVENT_INTERVAL_DEC;
				}
				bfd_send_poll(psess, event);
			}
		}
	}
	
	ptemplate->cc_tx_interval = cc_interval;

	return CMD_SUCCESS;
}


DEFUN (undo_bfd_min_transmit_interval,
	undo_bfd_min_transmit_interval_cmd,
	"undo bfd min-transmit-interval",
	BFD_STR
	"Restore Bfd min-transmit-interval function\n"
	)
{	/* template 视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_template *ptemplate = NULL;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cursor = 0;

	ptemplate = (struct bfd_template *)vty->index;

	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{
				psess->cc_interval_cfg = BFD_CC_INTERVAL;
				if(BFD_CC_INTERVAL > psess->ptem->cc_tx_interval)
				{
					event = BFD_EVENT_INTERVAL_INC;
				}
				else
				{
					event = BFD_EVENT_INTERVAL_DEC;
				}
				bfd_send_poll(psess, event);
			}
		}
	}
	
	ptemplate->cc_tx_interval = BFD_CC_INTERVAL;

	return CMD_SUCCESS;
}
	

DEFUN (bfd_multi_hop_authentication_mode,
	bfd_multi_hop_authentication_mode_cmd,
	"bfd multi-hop authentication-mode (m-md5 | m-sha1 | md5 | sha1 | simple)",
	BFD_STR
	"BFD multi-hop authentication-mode function\n"
	)
{	/*系统视图下*/
	gbfd.mult_auth = 0;

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_multi_hop_authentication_mode,
	undo_bfd_multi_hop_authentication_mode_cmd,
	"undo bfd multi-hop authentication-mode",
	BFD_STR
	"BFD multi-hop authentication-mode function\n"
	)
{	/*系统视图下*/
	gbfd.mult_auth = 0;

	return CMD_SUCCESS;
}

DEFUN (bfd_multi_hop_dst_port,
	bfd_multi_hop_dst_port_cmd,
	"bfd multi-hop destination-port (3784 | 4784)",
	BFD_STR
	"BFD multi-hop dest-port function\n"
	"port(3784 | 4784),default 4784\n"
	)
{	/*系统视图下*/
	uint16_t port = 0;

	VTY_GET_INTEGER_RANGE ( "port_num", port, argv[0], 1, 65535 );

	gbfd.mult_hop_port = port;

	return CMD_SUCCESS;
}


DEFUN (undo_bfd_multi_hop_dst_port,
	undo_bfd_multi_hop_dst_port_cmd,
	"undo bfd multi-hop destination-port",
	BFD_STR
	"Restore BFD multi-hop dest-port function\n"
	)
{	/*系统视图下*/
	gbfd.mult_hop_port = BFD_MULTIHOP_DSTPORT;

	return CMD_SUCCESS;
}


DEFUN (bfd_multi_hop_multiplier,
	bfd_multi_hop_multiplier_cmd,
	"bfd multi-hop detect-multiplier <3-50>",
	BFD_STR
	"BFD multi-hop detect-multiplier function\n"
	"detect_mult <3-50>, default 5\n"
	)
{	/*系统视图下*/
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t detect_mult = 0, cursor = 0;

	VTY_GET_INTEGER_RANGE ( "port_num", detect_mult, argv[0], 3, 50 );

	
	if(gbfd.multiplier != detect_mult)
	{
		gbfd.multiplier = gbfd.mult_detect = detect_mult;
		/*遍历所有会话*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket->data != NULL)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(NULL == psess->ptem)
				{
					bfd_send_poll(psess, event);
				}
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN (undo_bfd_multi_hop_multiplier,
	undo_bfd_multi_hop_multiplier_cmd,
	"undo_bfd multi-hop detect-multiplier",
	BFD_STR
	"Restore BFD multi-hop detect-multiplier function\n"
	)
{	/*系统视图下*/
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t cursor = 0;
	
	if(gbfd.multiplier != BFD_DETECT_MULTIPLIER)
	{
		gbfd.multiplier = gbfd.mult_detect = BFD_DETECT_MULTIPLIER;
		/*遍历所有会话*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket->data != NULL)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(NULL == psess->ptem)
				{
					bfd_send_poll(psess, event);
				}
			}
		}
	}

	return CMD_SUCCESS;
}



DEFUN (bfd_multi_hop_min_receive_interval,
	bfd_multi_hop_min_receive_interval_cmd,
	"bfd multi-hop min-receive-interval <3-10000>",
	BFD_STR
	"BFD multi-hop min-receive-interval function\n"
	"interval <3-10000> ms, default 1000ms\n"
	)
{	/*系统视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t interval = 0, cursor = 0;

	VTY_GET_INTEGER_RANGE ( "interval", interval, argv[0], 3, 10000 );

	interval *= 1000;
	/*全局配置改变，对应会话的接收间隔改变*/
	if(gbfd.cc_interval != interval)
	{
		if(interval > gbfd.cc_interval)
		{
			event = BFD_EVENT_INTERVAL_INC;
		}
		else
		{
			event = BFD_EVENT_INTERVAL_DEC;
		}
		
		gbfd.cc_interval = gbfd.mult_hop_cc_rx_interval = interval;
		/*遍历所有会话*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket->data != NULL)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(NULL == psess->ptem)
				{
					/*发送poll报文*/
					bfd_send_poll(psess, event);
				}
			}
		}
	}

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_multi_hop_min_receive_interval,
	undo_bfd_multi_hop_min_receive_interval_cmd,
	"undo bfd multi-hop min-receive-interval",
	BFD_STR
	"Restore BFD multi-hop min-receive-interval function\n"
	)
{	/*系统视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cursor = 0;
	
	/*全局配置改变，对应会话的接收间隔改变*/
	if(gbfd.cc_interval != BFD_CC_INTERVAL)
	{
		if(BFD_CC_INTERVAL > gbfd.cc_interval)
		{
			event = BFD_EVENT_INTERVAL_INC;
		}
		else
		{
			event = BFD_EVENT_INTERVAL_DEC;
		}
		
		gbfd.cc_interval = gbfd.mult_hop_cc_rx_interval = BFD_CC_INTERVAL;
		/*遍历所有会话*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket->data != NULL)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(NULL == psess->ptem)
				{
					/*发送poll报文*/
					bfd_send_poll(psess, event);
				}
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN (bfd_multi_hop_min_transmit_interval,
	bfd_multi_hop_min_transmit_interval_cmd,
	"bfd multi-hop min-transmit-interval <3-10000>",
	BFD_STR
	"BFD multi-hop min-transmit-interval function\n"
	"interval <3-10000> ms, default 1000ms\n"
	)
{	/*系统视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t interval = 0, cursor = 0;

	VTY_GET_INTEGER_RANGE ( "interval", interval, argv[0], 3, 10000 );

	interval *= 1000;
	/*全局配置改变，对应会话的接收间隔改变*/
	if(gbfd.cc_interval != interval)
	{
		if(interval > gbfd.cc_interval)
		{
			event = BFD_EVENT_INTERVAL_INC;
		}
		else
		{
			event = BFD_EVENT_INTERVAL_DEC;
		}
		
		gbfd.cc_interval = gbfd.mult_hop_cc_tx_interval = interval;
		/*遍历所有会话*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket->data != NULL)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(NULL == psess->ptem)
				{
					/*发送poll报文*/
					bfd_send_poll(psess, event);
				}
			}
		}
	}

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_multi_hop_min_transmit_interval,
	undo_bfd_multi_hop_min_transmit_interval_cmd,
	"undo bfd multi-hop min-transmit-interval",
	BFD_STR
	"Restore BFD multi-hop min-transmit-interval function\n"
	)
{	/*系统视图下*/
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cursor = 0;
	
	/*全局配置改变，对应会话的接收间隔改变*/
	if(gbfd.cc_interval != BFD_CC_INTERVAL)
	{
		if(BFD_CC_INTERVAL > gbfd.cc_interval)
		{
			event = BFD_EVENT_INTERVAL_INC;
		}
		else
		{
			event = BFD_EVENT_INTERVAL_DEC;
		}
		
		gbfd.cc_interval = gbfd.mult_hop_cc_tx_interval = BFD_CC_INTERVAL;
		/*遍历所有会话*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket->data != NULL)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(NULL == psess->ptem)
				{
					/*发送poll报文*/
					bfd_send_poll(psess, event);
				}
			}
		}
	}

	return CMD_SUCCESS;
}

DEFUN (bfd_session_init_mode,
	bfd_session_init_mode_cmd,
	"bfd session init-mode (active | passive)",
	BFD_STR
	"BFD session init-mode function\n"
	"(active | passive), default mode active\n"
	)
{	/*系统视图下*/
	if(0 == strncmp(argv[0], "active", 6))
	{
		gbfd.bfd_init_mode = BFD_ACTIVE;
	}
	else
	{
		gbfd.bfd_init_mode = BFD_PASSIVE;
	}

	return CMD_SUCCESS;
}

DEFUN (undo_bfd_session_init_mode,
	undo_bfd_session_init_mode_cmd,
	"undo_bfd session init-mode",
	BFD_STR
	"Restore BFD session init-mode default function\n"
	)
{	/*系统视图下*/
	gbfd.bfd_init_mode = BFD_ACTIVE;

	return CMD_SUCCESS;
}

DEFUN (bfd_up_down_clear,
	bfd_up_down_clear_cmd,
	"bfd status clear",
	BFD_STR
	"BFD up/down counter\n"
	)
{
	uint32_t cursor = 0;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = pbucket->data;
			if(BFD_STATUS_UP == psess->status)
			{
				psess->up_count = 1;
				psess->down_count = 0;
				psess->send_count = 0;
				psess->recv_count = 0;
			}
			else
			{
				psess->up_count = 0;
				psess->down_count = 0;
				psess->send_count = 0;
				psess->recv_count = 0;
			}
		}
	}
	
	return CMD_SUCCESS;
}

DEFUN (bfd_ttl_set,
	bfd_ttl_cmd,
	"bfd ttl <1-255>",
	BFD_STR
	"BFD ttl\n"
	"BFD ttl value, default is 255\n"
	)
{
    uint16_t ttl = 255;
	uint32_t cursor = 0;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "ttl", ttl, argv[0], 1, 255 );
	gbfd.ttl = ttl;

	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = pbucket->data;
			if(BFD_STATUS_UP == psess->status)
			{
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
		}
	}
	
	return CMD_SUCCESS;
}

DEFUN (undo_bfd_ttl_set,
	undo_bfd_ttl_cmd,
	"no bfd ttl",
	BFD_STR
	"BFD ttl\n"
	"BFD ttl value, default is 255\n"
	)
{
	uint32_t cursor = 0;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	
	/*恢复默认ttl = 255*/
	gbfd.ttl = 255;
	
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = pbucket->data;
			if(BFD_STATUS_UP == psess->status)
			{
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
		}
	}

	return CMD_SUCCESS;
}

DEFUN (bfd_priority_set,
	bfd_priority_cmd,
	"bfd priority <0-7>",
	BFD_STR
	"BFD priority\n"
	"BFD priority value, default is 7\n"
	)
{
    uint8_t priority = 7;
	uint32_t cursor = 0;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	
	if( !gbfd.enable )
	{
        vty_warning_out ( vty, "BFD global function is disabled%s", VTY_NEWLINE );
        return CMD_WARNING;
	}

    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "priority", priority, argv[0], 0, 7 );
	
	gbfd.priority = priority;

	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = pbucket->data;
			if(BFD_STATUS_UP == psess->status)
			{
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
		}
	}
	
	return CMD_SUCCESS;
}

DEFUN (undo_bfd_priority_set,
	undo_bfd_priority_cmd,
	"no bfd priority",
	BFD_STR
	"BFD priority\n"
	"BFD priority value, default is 7\n"
	)
{
	uint8_t priority = 7;
	uint32_t cursor = 0;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	
	if( !gbfd.enable )
	{
		vty_warning_out ( vty, "BFD global function is disabled%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	/*恢复默认优先级 priority = 7*/
	gbfd.priority = priority;
	
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket->data != NULL)
		{
			psess = pbucket->data;
			if(BFD_STATUS_UP == psess->status)
			{
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
		}
	}

	return CMD_SUCCESS;
}
	
DEFUN (bfd_session,
	bfd_session_cmd,
	"bfd session <1-65535>",
	BFD_STR
	"BFD session\n"
	"config session id, <1-65535>"
	)
{
	struct bfd_sess *psess = NULL;
	char *pprompt = NULL;
	uint16_t session_id = 0;

	if( !gbfd.enable )
	{
        vty_out ( vty, "BFD global function is disabled%s", VTY_NEWLINE );
        return CMD_WARNING;
	}
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "session", session_id, argv[0], 1, 65535 );
	/*检测会话是否已经存在*/
	psess = bfd_session_lookup(session_id);
	if(psess == NULL)
	{
		/*根据local-id创建会话*/
		psess = bfd_session_create(session_id);
		if(psess == NULL)
		{
			vty_warning_out ( vty, "Create BFD session failed! The entry number exceed limit %d, or Memory alloc failed !%s", BFD_NUM_MAX, VTY_NEWLINE );
			return CMD_WARNING;
		}
	}
	/*进入会话视图*/
    vty->node = BFD_SESSION_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%s(config-bfd-session-%d)# ", "huahuan", session_id);
	}
	vty->index = (void *)(uint32_t)session_id;
	
	return CMD_SUCCESS;
}

DEFUN (bfd_session_del,
	undo_bfd_session_cmd,
	"no bfd session <1-65535>",
	NO_STR
	BFD_STR
	"BFD session\n"
	"config session id, <1-65535>"
	)
{
	uint16_t sess_id = 0;
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pitem = NULL;
	
	if( !gbfd.enable )
	{
        vty_warning_out ( vty, "BFD global function is disabled%s", VTY_NEWLINE );
        return CMD_WARNING;
	}
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "session", sess_id, argv[0], 1, 65535 );
	/*检测会话是否已经存在*/
	pitem = hios_hash_find(&bfd_session_table, (void *)(uint32_t)sess_id);
	if (NULL == pitem)
	{
        vty_warning_out ( vty, "BFD session %d does not exist%s", sess_id, VTY_NEWLINE );
		return ERRNO_SUCCESS;
	}
	
	psess = (struct bfd_sess *)pitem->data;
	if(NULL == psess)
	{
        vty_warning_out ( vty, "BFD session %d does not exist%s", sess_id, VTY_NEWLINE );
        return CMD_WARNING;
	}
	
	if(BFD_TYPE_INVALID != psess->type)
	{
		vty_warning_out(vty, "BFD session %d is bind, can`t delete!%s", sess_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(0 != psess->session_id)
	{
		vty_warning_out(vty, "BFD session %d is bind, can`t delete!%s", sess_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	else
	{
		bfd_local_id_delete(psess, local_id_list);
		hios_hash_delete(&bfd_session_table, pitem);
		index_free(INDEX_TYPE_OAM, (uint32_t)sess_id);
		XFREE(MTYPE_BFD_SESS_ENTRY, psess);
		pitem->prev = NULL;
		pitem->next = NULL;
		XFREE(MTYPE_HASH_BACKET, pitem);
	}

	return CMD_SUCCESS;
}

DEFUN (bfd_session_local_discriminator,
	bfd_session_local_disc_cmd,
	"local-discriminator <1-4294967295>",
	"Set BFD session local discriminator\n"
	"local discriminator, <1-4294967295>\n"
	)
{
	struct bfd_sess *psess = NULL;
	uint16_t sess_id = 0;
	uint32_t local_id = 0;
	int ret = 0;

    VTY_GET_INTEGER ( "local_id", local_id, argv[0] );
	sess_id = (uint16_t)(uint32_t)vty->index;
	/*检测会话是否存在*/
	psess = bfd_session_lookup(sess_id);
	if(NULL == psess)
	{
		vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
		return CMD_SUCCESS;
	}

	if(psess->local_id)
	{
		vty_warning_out(vty, "BFD session %u is self_negotiation, local_id can`t change%s", psess->local_id, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if(0 != psess->local_id_cfg && BFD_TYPE_INVALID != psess->type)
	{
		vty_warning_out( vty, "BFD session local_id %u has been bind, please unbind%s", psess->local_id_cfg, VTY_NEWLINE );
		return CMD_SUCCESS;
	}

	ret = bfd_local_id_create(psess, local_id_list, local_id);
	if(0 != ret)
	{
		vty_warning_out(vty, "local_id has been used by another session%s", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}

DEFUN (bfd_session_local_discriminator_del,
	undo_bfd_session_local_disc_cmd,
	"no local-discriminator",
	NO_STR
	"Set BFD session local discriminator\n"
	)
{
	struct bfd_sess *psess = NULL;
	uint32_t local_id = 0;
	uint16_t sess_id = 0;
	
	sess_id = (uint16_t)(uint32_t)vty->index;

	/*检测会话是否已经存在*/
	psess = bfd_session_lookup(sess_id);
	if(psess == NULL)
	{
		vty_warning_out( vty, "BFD session %d do not exist!%s", local_id, VTY_NEWLINE );
		return CMD_SUCCESS;
	}

	if (BFD_TYPE_INVALID != psess->type)
	{
		vty_warning_out( vty, "BFD session is enable, please disable bfd session first%s", VTY_NEWLINE );
		return CMD_SUCCESS;
	}

	if(0 != psess->local_id_cfg)
	{
		bfd_local_id_delete(psess, local_id_list);
	}
	else
	{
		vty_warning_out( vty, "BFD session local discriminator has been not configed%s", VTY_NEWLINE );
	}
	
	return CMD_SUCCESS;
}

DEFUN (bfd_session_remote_discriminator,
	bfd_session_remote_disc_cmd,
	"remote-discriminator <1-4294967295>",
	"Set BFD session remote discriminator\n"
	"remote discriminator, <1-4294967295>\n"
	)
{
	struct bfd_sess *psess = NULL;
	uint32_t remote_id = 0;
	uint16_t sess_id = 0;

    VTY_GET_INTEGER ( "remote_id", remote_id, argv[0] );
	/*设置会话的对端标识*/
	sess_id = (uint16_t)(uint32_t)vty->index;
	/*检测会话是否已经存在*/
	psess = bfd_session_lookup(sess_id);
	if(psess == NULL)
	{
		vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
		return CMD_SUCCESS;
	}
	
	if(psess->local_id)
	{
		vty_warning_out(vty, "BFD session %u is self_negotiation, remote_id can`t change%s", psess->local_id, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if(BFD_TYPE_INVALID != psess->type)
	{
		vty_warning_out( vty, "BFD session has been bind %s", VTY_NEWLINE );
		return CMD_SUCCESS;
	}
	/*若远端标识符已经设置过，不能重新设置*/
	if(psess->remote_id_cfg != 0 && psess->type != BFD_TYPE_INVALID)
	{
		vty_warning_out( vty, "BFD session remote discriminator has been set to %u%s", psess->remote_id_cfg, VTY_NEWLINE );
		return CMD_SUCCESS;
	}
	
	psess->remote_id_cfg = remote_id;
	
	return CMD_SUCCESS;
}

DEFUN (bfd_session_remote_discriminator_del,
	undo_bfd_session_remote_disc_cmd,
	"no remote-discriminator",
	NO_STR
	"Set BFD session remote discriminator\n"
	)
{
	uint16_t sess_id = 0;
	struct bfd_sess *psess = NULL;
	
	sess_id = (uint16_t)(uint32_t)vty->index;

	/*检测会话是否已经存在*/
	psess = bfd_session_lookup(sess_id);
	if(psess == NULL)
	{
		vty_info_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
		return CMD_SUCCESS;
	}

	if (BFD_TYPE_INVALID != psess->type)
	{
		vty_info_out( vty, "BFD session is enable, please disable bfd session first%s", VTY_NEWLINE );
		return CMD_SUCCESS;
	}

	if(0 != psess->remote_id_cfg)
	{
		psess->remote_id_cfg = 0;
	}
	else
	{
		vty_info_out( vty, "BFD session remote discriminator has been not configed%s", VTY_NEWLINE );
	}

	return CMD_SUCCESS;
}

DEFUN (bfd_session_cc_interval_set,
	bfd_session_cc_interval_cmd,
	"bfd cc-interval (3 | 10 | 20 | 100 | 300 | 1000)",
	BFD_STR
	"Set BFD session minimum receive interval\n"
	"3.3ms\n"
	"10ms\n"
	"20ms\n"
	"100ms\n"
	"300ms\n"
	"1000ms, default is 1000ms\n"
	)
{
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t interval = 0, cursor = 0;
	uint16_t sess_id = 0;
	
	/*间隔改变*/
	if((strncmp(argv[0], "3", 1) == 0) ||
		(strncmp(argv[0], "10", 2) == 0) ||
		(strncmp(argv[0], "20", 2) == 0) ||
		(strncmp(argv[0], "100", 3) == 0) ||
		(strncmp(argv[0], "300", 3) == 0) ||
		(strncmp(argv[0], "1000", 4) == 0))
	{
		interval = (atoi(argv[0])) * 1000;
	}
	if(vty->node == CONFIG_NODE)
	{
		/*全局配置改变，对应会话的接收间隔改变*/
		if(gbfd.cc_interval != interval)
		{
			if(interval > gbfd.cc_interval)
			{
				event = BFD_EVENT_INTERVAL_INC;
			}
			else
			{
				event = BFD_EVENT_INTERVAL_DEC;
			}
			gbfd.cc_interval = interval;
			/*遍历所有会话*/
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket->data != NULL)
				{
					psess = pbucket->data;
					if(psess->cc_interval_cfg == 0)
					{
						/*发送poll报文*/
						bfd_send_poll(psess, event);
					}
				}
			}
		}
	}
	else if(vty->node == BFD_SESSION_NODE)
	{
		/*获取会话状态*/
		sess_id = (uint16_t)(uint32_t)vty->index;
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		/*第一次设置，默认使用的是全局配置*/
		if(psess->cc_interval_cfg == 0)
		{
			psess->cc_interval_cfg = interval;
			if(interval != gbfd.cc_interval)
			{
				if(interval > gbfd.cc_interval)
				{
					event = BFD_EVENT_INTERVAL_INC;
				}
				else
				{
					event = BFD_EVENT_INTERVAL_DEC;
				}
				bfd_send_poll(psess, event);
			}
		}
		else if(psess->cc_interval_cfg != interval)
		{
			if(interval > psess->cc_interval_cfg)
			{
				event = BFD_EVENT_INTERVAL_INC;
			}
			else
			{
				event = BFD_EVENT_INTERVAL_DEC;
			}
			psess->cc_interval_cfg = interval;
			bfd_send_poll(psess, event);
		}
	}
	return CMD_SUCCESS;
}

DEFUN (bfd_session_cc_interval_reset,
	undo_bfd_session_cc_interval_cmd,
	"no bfd cc-interval",
	NO_STR
	BFD_STR
	"Set BFD session minimum receive interval\n"
	)
{
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t cursor = 0;
	uint16_t sess_id = 0;
	
	/*间隔设置为默认值10*/
	if(vty->node == CONFIG_NODE)
	{
		/*全局配置改变，对应会话的接收间隔改变*/
		if(gbfd.cc_interval != BFD_CC_INTERVAL)
		{
			if(BFD_CC_INTERVAL > gbfd.cc_interval)
			{
				event = BFD_EVENT_INTERVAL_INC;
			}
			else
			{
				event = BFD_EVENT_INTERVAL_DEC;
			}
			gbfd.cc_interval = BFD_CC_INTERVAL;
			/*遍历所有会话*/
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket->data != NULL)
				{
					psess = pbucket->data;
					if(psess->cc_interval_cfg == 0)
					{
						bfd_send_poll(psess, event);
					}
				}
			}
		}
	}
	else if(vty->node == BFD_SESSION_NODE)
	{
		/*获取会话状态*/
		sess_id = (uint16_t)(uint32_t)vty->index;
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		/*删除会话的配置使用全局的配置*/
		if(psess->cc_interval_cfg != 0 && psess->cc_interval_cfg != gbfd.cc_interval)
		{
			if(gbfd.cc_interval > psess->cc_interval_cfg)
			{
				event = BFD_EVENT_INTERVAL_INC;
			}
			else
			{
				event = BFD_EVENT_INTERVAL_DEC;
			}
			/*触发会话接收时间改变事件*/
			psess->cc_interval_cfg = 0;
			bfd_send_poll(psess, event);
		}
		psess->cc_interval_cfg = 0;
	}
	return CMD_SUCCESS;
}

DEFUN (bfd_session_detect_multiplier_set,
	bfd_session_detect_multiplier_cmd,
	"bfd detect-multiplier <1-10>",
	BFD_STR
	"Set BFD session detect multiplier\n"
	"Detect multiplier, default is 5\n"
	)
{
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t multiplier = 0, cursor = 0;
	uint16_t sess_id = 0;
	
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "detect-multiplier", multiplier, argv[0], 1, 10 );
	if(vty->node == CONFIG_NODE)
	{
		if(gbfd.multiplier != multiplier)
		{
			gbfd.multiplier = multiplier;
			/*遍历所有会话*/
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket->data != NULL)
				{
					psess = pbucket->data;
					if(psess->cc_multiplier_cfg == 0)
					{
						bfd_send_poll(psess, event);
					}
				}
			}
		}
	}
	else if(vty->node == BFD_SESSION_NODE)
	{
		/*获取会话状态*/
		sess_id = (uint16_t)(uint32_t)vty->index;
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			vty_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(psess->cc_multiplier_cfg == 0)
		{
			psess->cc_multiplier_cfg = multiplier;
			if(multiplier != gbfd.multiplier)
			{
				bfd_send_poll(psess, event);
			}
		}
		else if(psess->cc_multiplier_cfg != multiplier)
		{
			psess->cc_multiplier_cfg = multiplier;
			bfd_send_poll(psess, event);
		}
	}
	return CMD_SUCCESS;
}

DEFUN (bfd_session_detect_multiplier_reset,
	undo_bfd_session_detect_multiplier_cmd,
	"no bfd detect-multiplier",
	NO_STR
	BFD_STR
	"Set BFD session detect multiplier\n"
	)
{
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint32_t cursor = 0;
	uint16_t sess_id = 0;

	/*间隔设置为默认值10*/
	if(vty->node == CONFIG_NODE)
	{
		/*全局配置改变，对应会话的接收间隔改变*/
		if(gbfd.multiplier != BFD_DETECT_MULTIPLIER)
		{
			gbfd.multiplier = BFD_DETECT_MULTIPLIER;
			/*遍历所有会话*/
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket->data != NULL)
				{
					psess = pbucket->data;
					if(psess->cc_multiplier_cfg == 0)
					{
						bfd_send_poll(psess, event);
					}
				}
			}
		}
	}
	else if(vty->node == BFD_SESSION_NODE)
	{
		/*获取会话状态*/
		sess_id = (uint16_t)(uint32_t)vty->index;
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(psess->cc_multiplier_cfg != 0 && psess->cc_multiplier_cfg != gbfd.multiplier)
		{
			psess->cc_multiplier_cfg = 0;
			bfd_send_poll(psess, event);
		}
		psess->cc_multiplier_cfg = 0;
	}
	return CMD_SUCCESS;
}

DEFUN (bfd_session_delay_up_set,
	bfd_session_delay_up_cmd,
	"bfd wtr <0-3600>",
	BFD_STR
	"BFD session WTR(Wait To Restore) time\n"
	"Specify the WTR interval in seconds\n"
	)
{
	struct bfd_sess *psess = NULL;
	uint32_t wtr = 0;
	uint16_t sess_id = 0;
	
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "wtr", wtr, argv[0], 0, 3600 );
	if(vty->node == CONFIG_NODE)
	{
		if(gbfd.wtr != wtr)
		{
			gbfd.wtr = wtr;
		}
	}
	else if(vty->node == BFD_SESSION_NODE)
	{
		/*获取会话状态*/
		sess_id = (uint16_t)(uint32_t)vty->index;
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(psess->wtr != wtr)
		{
			psess->wtr = wtr;
			/*触发会话接收时间改变事件*/
		}
	}
	return CMD_SUCCESS;
}

DEFUN (bfd_session_delay_up_reset,
	undo_bfd_session_delay_up_cmd,
	"no bfd wtr",
	NO_STR
	BFD_STR
	"BFD session WTR(Wait To Restore) time\n"
	)
{
	struct bfd_sess *psess = NULL;
	uint16_t sess_id = 0;

	/*间隔设置为默认值10*/
	if(vty->node == CONFIG_NODE)
	{
		/*全局配置改变，对应会话的接收间隔改变*/
		if(gbfd.wtr != BFD_WTR_INTERVAL)
		{
			gbfd.wtr = BFD_WTR_INTERVAL;
		}
	}
	else if(vty->node == BFD_SESSION_NODE)
	{
		/*获取会话状态*/
		sess_id = (uint16_t)(uint32_t)vty->index;
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			vty_warning_out( vty, "BFD session %d do not exist!%s", sess_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(psess->wtr != BFD_WTR_INTERVAL)
		{
			psess->wtr = BFD_WTR_INTERVAL;
			/*触发会话接收时间改变事件*/
		}
	}
	return CMD_SUCCESS;
}


DEFUN (bfd_intf_session_enable,
	bfd_intf_session_enable_cmd,
	"bfd enable { session <1-65535> | source-ip A.B.C.D }",
	BFD_STR
	"Interface enable BFD session function\n"
	"Specify BFD session\n"
	"BFD session local discriminator\n"
	"Set source IP address of the BFD packet\n"
	"IP Unicast address\n"
	)
{
	struct bfd_sess *psess = NULL;
	struct mpls_if *pif = NULL;
	uint16_t session_id = 0;
    uint32_t ifindex = 0;
	uint32_t ip;
	int ret = 0;
	/*获取接口索引*/
    ifindex = (uint32_t)vty->index;

	/*检查接口是否有会话*/
	pif = mpls_if_lookup(ifindex);
	if(NULL != pif)
	{
        if(0 != pif->bfd_sessid)
		{
			vty_warning_out( vty, "Interface BFD session exist!%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
	}
	
	/*源ip地址*/
	memset(&ip, 0, sizeof(struct in_addr));
	if(argv[1] != NULL)
	{
		ip = inet_strtoipv4((char *)argv[1]);
		
		/*检查是否为单播地址, 0.0.0.0表示当前主机地址*/
		ret = inet_valid_ipv4(ip);
		if (ret != 1)
		{
			vty_warning_out(vty, "Please input a Unicast address.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	if(argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE ( "session", session_id, argv[0], 1, 65535 );
		
		/*根据sess_id 查找会话*/
		psess = bfd_session_lookup(session_id);
		if(psess == NULL)
		{
			vty_warning_out( vty, "BFD session %d do not exist!%s", session_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		else if(psess->local_id_cfg == 0)
		{
			vty_warning_out( vty, "Please config BFD session %d local_id !%s", session_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
		else if(psess->type != BFD_TYPE_INVALID)
		{
			vty_warning_out( vty, "BFD session %d has been bound!%s", session_id, VTY_NEWLINE );
			return CMD_WARNING;
		}
	}
	else
	{
		/*表示新创建的会话，若绑定失败，删除会话*/
 		psess = bfd_session_create(0);
		if(psess == NULL)
		{
			vty_warning_out ( vty, "Create BFD session  failed%s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		/*表示创建会话的local_id，若失败，返回 */
		bfd_local_id_create(psess, local_id_list, 0);
		if(0 == psess->local_id)
		{
			bfd_session_delete(psess->session_id);
			vty_warning_out ( vty, "Create BFD local_id  failed%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
	}
	
	/*将会话绑定在接口，仅在此处进行绑定*/
	/*仅在在删除会话时解除绑定*/
	ret = mpls_if_bind_bfd_sess(ifindex, psess->session_id_cfg? psess->session_id_cfg : psess->session_id);
	if(ret != 0)
	{
		/*接口绑定会话失败，删除会话*/
		if(psess->local_id)
		{
			bfd_session_delete(psess->session_id);
		}
		vty_warning_out ( vty, "BFD session	bind failed%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	psess->index = ifindex;
	psess->srcip = ip;
	
	/*接口建立会话，invalid->down,发送down报文,主动模式*/
	bfd_fsm(BFD_EVENT_BIND_IF, psess);

    //通知适配下表
    ret = bfd_send_hal(psess, IPC_OPCODE_ADD, 0);
	if(0 < ret)
	{
		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN (bfd_intf_session_disable,
	undo_bfd_intf_session_enable_cmd,
	"no bfd enable",
	NO_STR
	BFD_STR
	"Interface enable BFD session function\n"
	)
{
    uint32_t ifindex = 0;
	struct bfd_sess *psess = NULL;
	struct mpls_if *pif = NULL;
    struct bfd_sess sessdel;

	/*配置模式直接返回*/
	if(vty->node == CONFIG_NODE)
	{
		return CMD_SUCCESS;
	}
	/*获取接口索引*/
    ifindex = (uint32_t)vty->index;
	/*判断接口下是否已创建会话*/
	pif = mpls_if_lookup(ifindex);
	if(pif == NULL || pif->bfd_sessid == 0)
	{
		vty_warning_out( vty, "Interface BFD session does not exist!%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	psess = bfd_session_lookup(pif->bfd_sessid);
    if(NULL == psess)
    {
		vty_warning_out( vty, "BFD session does not exist!%s", VTY_NEWLINE );
		return CMD_WARNING;    
    }

    memset ( &sessdel, 0, sizeof ( struct bfd_sess ) );
    sessdel.index = psess->index;
    sessdel.type = BFD_TYPE_INTERFACE;
    
    
	if(psess->session_id)
	{
		sessdel.session_id = psess->session_id;
	    sessdel.local_id = psess->local_id;
		bfd_fsm(BFD_EVENT_SESSION_DELETE, psess);
	}
	else if(psess->session_id_cfg)
	{
		sessdel.session_id = psess->session_id_cfg;
	    sessdel.local_id = psess->local_id_cfg;
		bfd_fsm(BFD_EVENT_DISABLE, psess);
	}

    //通知适配删表
    bfd_send_hal(&sessdel, IPC_OPCODE_DELETE, 0);
    
	return CMD_SUCCESS;
}

DEFUN (bfd_info_show,
	bfd_info_show_cmd,
	"show bfd ( session | statistics | config ) [<1-65535>]",
	SHOW_STR
	BFD_STR
	"Display session information\n"
	"Display the statistics information of session\n"
	"Display configuration view information\n"
	"local_id for view information\n"
	)
{
	struct bfd_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	struct lsp_entry *plsp = NULL;
	char ifname[INTERFACE_NAMSIZE+1] = "";
	uint32_t local_id = 0, remote_id = 0, count = 0;
	uint16_t cursor = 0,  sess_id = 0, sess_id_node = 0;
	char state[BFD_SIZE]= "";
	char type[BFD_SIZE]= "";
	char diag[BFD_SIZE]= "";
	struct list_bfd_sess *sort_sess = NULL;
	struct list_bfd_sess *sort_sess_node = NULL;
	struct list_bfd_sess *sort_sess_head = NULL;
	
    if(strncmp(argv[0], "session", 7) == 0)
    {
		/*show bfd session*/
		if(!gbfd.enable)
		{
			return CMD_SUCCESS;
		}

		if( argv[1] )
		{
			VTY_GET_INTEGER_RANGE ( "session", sess_id, argv[1], 1, 65535 );
			
			psess = bfd_session_lookup(sess_id);
			if(NULL == psess)
			{
        		vty_out ( vty, "BFD session %d does not exist%s", sess_id, VTY_NEWLINE );
        		return CMD_WARNING;
			}

			vty_out(vty, "-------------------------------------------%s", VTY_NEWLINE);
			vty_out(vty, "Session-id   Local-id   Remote-id   State    Bind-Type   Bind-Name%s", VTY_NEWLINE);

			sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
			local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
			remote_id = psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id;
			strcpy(state, "");
			strcpy(type, "");
			strcpy(ifname, "NULL");
			switch(psess->status)
			{
				case BFD_STATUS_ADMINDOWN:
					strcpy(state, "ADMINDOWN");
					break;
				case BFD_STATUS_DOWN:
					strcpy(state, "DOWN");
					break;
				case BFD_STATUS_INIT:
					strcpy(state, "INIT");
					break;
				case BFD_STATUS_UP:
					strcpy(state, "UP");
					break;
			}
			switch(psess->type)
			{
				case BFD_TYPE_INVALID:
					strcpy(type, "Invalid");
					break;
				case BFD_TYPE_INTERFACE:
					strcpy(type, "For_IF");
					ifm_get_name_by_ifindex(psess->index, ifname);
					break;
				case BFD_TYPE_LSP:
					strcpy(type, "For_LSP");
					plsp = mpls_lsp_lookup(psess->index);
					if(NULL == plsp)
					{
						break;
					}
					strcpy(ifname, (char *)plsp->name);
					break;
				case BFD_TYPE_PW:
					strcpy(type, "For_PW");
					strcpy(ifname, (char *)(pw_get_name(psess->index)));
					break;
				case BFD_TYPE_TUNNEL:
					strcpy(type, "For_TUNNEL");
					ifm_get_name_by_ifindex(psess->index, ifname);
					break;
				case BFD_TYPE_IP:
					if(0 != psess->ldp_flag)
					{
						strcpy(type, "For_LDP");
						inet_ipv4tostr(psess->dstip, ifname);
						break;
					}
					strcpy(type, "For_IF_IP");
					inet_ipv4tostr(psess->dstip, ifname);
					break;
				default:
					break;
			}
			
			vty_out(vty, "%-13d%-11u%-12u%-9s%-12s%s%s", 
				sess_id, local_id, remote_id, state, type, ifname, VTY_NEWLINE);

			return CMD_SUCCESS;
		}

		if(bfd_session_table.num_entries)
		{
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket && pbucket->data)
				{
					psess = (struct bfd_sess *)pbucket->data;
					sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
					sort_sess = malloc(sizeof(struct list_bfd_sess));
					if(NULL == sort_sess)
					{
						vty_out(vty, "malloc fail!%s", VTY_NEWLINE);
						return CMD_WARNING;
					}
					
					sort_sess->psess = psess;

					if(NULL == sort_sess_head)
					{
						sort_sess_head = sort_sess;
						sort_sess_head->prev = NULL;
						sort_sess_head->next = NULL;
					}
					else
					{
						for(sort_sess_node = sort_sess_head;sort_sess_node;sort_sess_node = sort_sess_node->next)
						{
							sess_id_node = sort_sess_node->psess->session_id_cfg ? sort_sess_node->psess->session_id_cfg : sort_sess_node->psess->session_id;
							if(sess_id < sess_id_node)
							{
								if(sort_sess_node == sort_sess_head)
								{
									sort_sess_node->prev = sort_sess;
									sort_sess->next = sort_sess_node;
									sort_sess->prev = NULL;
									sort_sess_head = sort_sess;
								}
								else
								{
									sort_sess->prev = sort_sess_node->prev;
									sort_sess->next = sort_sess_node;
									sort_sess_node->prev->next = sort_sess;
									sort_sess_node->prev = sort_sess;
								}
								break;
							}

							if(NULL == sort_sess_node->next)
							{
								sort_sess->prev = sort_sess_node;
								sort_sess->next = NULL;
								sort_sess_node->next = sort_sess;
								break;
							}
						}
					}
				}
			}
		}
		
		vty_out(vty, "Total BFD Session Number: %u%s", bfd_session_table.num_entries, VTY_NEWLINE);
		vty_out(vty, "Current BFD Session UP/DOWN Number: %u/%u%s", gbfd.up_count, gbfd.down_count, VTY_NEWLINE);
		if(bfd_session_table.num_entries)
		{
			vty_out(vty, "-------------------------------------------%s", VTY_NEWLINE);
			vty_out(vty, "Session-id   Local-id   Remote-id   State    Bind-Type   Bind-Name%s", VTY_NEWLINE);
			/*遍历所有的会话节点*/
			//HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			for(sort_sess_node = sort_sess_head;sort_sess_node;sort_sess_node = sort_sess_node->next)
			{
				//if(pbucket && pbucket->data)
				{
				//	psess = (struct bfd_sess *)pbucket->data;
					psess = sort_sess_node->psess;
					sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
					local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
					remote_id = psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id;
					strcpy(state, "");
					strcpy(type, "");
					strcpy(ifname, "NULL");
					switch(psess->status)
					{
						case BFD_STATUS_ADMINDOWN:
							strcpy(state, "ADMINDOWN");
							break;
						case BFD_STATUS_DOWN:
							strcpy(state, "DOWN");
							break;
						case BFD_STATUS_INIT:
							strcpy(state, "INIT");
							break;
						case BFD_STATUS_UP:
							strcpy(state, "UP");
							break;
					}
					switch(psess->type)
					{
						case BFD_TYPE_INVALID:
							strcpy(type, "Invalid");
							break;
						case BFD_TYPE_INTERFACE:
							strcpy(type, "For_IF");
							ifm_get_name_by_ifindex(psess->index, ifname);
							break;
						case BFD_TYPE_LSP:
							strcpy(type, "For_LSP");
							plsp = mpls_lsp_lookup(psess->index);
							if(NULL == plsp)
							{
								break;
							}
							strcpy(ifname, (char *)plsp->name);
							break;
						case BFD_TYPE_PW:
							strcpy(type, "For_PW");
							strcpy(ifname, (char *)(pw_get_name(psess->index)));
							break;
						case BFD_TYPE_TUNNEL:
							strcpy(type, "For_TUNNEL");
							ifm_get_name_by_ifindex(psess->index, ifname);
							break;
						case BFD_TYPE_IP:
							if(0 != psess->ldp_flag)
							{
								strcpy(type, "For_LDP");
								inet_ipv4tostr(psess->dstip, ifname);
								break;
							}
							strcpy(type, "For_IF_IP");
							inet_ipv4tostr(psess->dstip, ifname);
							break;
						default:
							break;
					}
					
					vty_out(vty, "%-13d%-11u%-12u%-9s%-12s%s%s", 
						sess_id, local_id, remote_id, state, type, ifname, VTY_NEWLINE);
					
					if(count >= bfd_session_table.num_entries)
					{
						while(NULL != sort_sess_head)
						{
							sort_sess_node = sort_sess_head->next;
							free(sort_sess_head);
							sort_sess_head = sort_sess_node;
						}
						
						return CMD_SUCCESS;
					}
				}
			}
		}
    }
    else if(strncmp(argv[0], "statistics", 9) == 0)
    {
		/*show bfd statistcs*/
		vty_out(vty, "Total BFD Session Number: %u%s", bfd_session_table.num_entries, VTY_NEWLINE);
		vty_out(vty, "Current BFD Session UP/DOWN Number: %u/%u%s", gbfd.up_count, gbfd.down_count, VTY_NEWLINE);
		/*显示每个会话的up/down次数，收发包次数*/
		/*遍历所有的会话节点*/

		if( argv[1] )
		{
			VTY_GET_INTEGER_RANGE ( "session", sess_id, argv[1], 1, 65535 );
			
			psess = bfd_session_lookup(sess_id);
			if(NULL == psess)
			{
        		vty_warning_out ( vty, "BFD session %d does not exist%s", sess_id, VTY_NEWLINE );
        		return CMD_WARNING;
			}
			
			vty_out(vty, "Total BFD Session Number: %u%s", bfd_session_table.num_entries, VTY_NEWLINE);
			vty_out(vty, "Current BFD Session UP/DOWN Number: %u/%u%s", gbfd.up_count, gbfd.down_count, VTY_NEWLINE);

			sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
			local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
			remote_id = psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id;
			++count;
			/*会话上一次down的原因*/
			strcpy(diag, "");
			switch(psess->diag)
			{
				case BFD_DIAG_NO_DIAG:
					strcpy(diag, "no diagnostics");
					break;
				case BFD_DIAG_DETECT_EXPIRE:
					strcpy(diag, "detect timer expire");
					break;
				case BFD_DIAG_NEIGHBOR_DOWN:
					strcpy(diag, "neighbor down");
					break;
				case BFD_DIAG_ADMIN_DOWN:
					strcpy(diag, "admin down");
					break;
					default:
						break;
			}
			
			vty_out(vty, "%d.%s", count, VTY_NEWLINE);
			vty_out(vty, "  BFD Session ID: %d%s", sess_id, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Local Discriminator:  %u%s", local_id, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Remote Discriminator: %u%s", remote_id, VTY_NEWLINE);
			vty_out(vty, "	BFD Session CC Interval Configuration: %dms%s", (psess->cc_interval_cfg)/1000, VTY_NEWLINE);
			vty_out(vty, "	BFD Session CC Send Interval: %dms%s", (psess->cc_interval_send)/1000, VTY_NEWLINE);
			vty_out(vty, "	BFD Session CC Detect Time: %dms%s", (psess->cc_interval_recv)*(psess->remote_multiplier)/1000, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Multiplier Configuration: %d%s", psess->cc_multiplier_cfg, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Remote Multiplier: %d%s", psess->remote_multiplier, VTY_NEWLINE);
			vty_out(vty, "	BFD Session UP Times: %lld%s", psess->up_count, VTY_NEWLINE);
			vty_out(vty, "	BFD Session DOWN Times: %lld%s", psess->down_count, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Diagnostics: %s%s", diag, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Received BFD Negotiation Packets: %lld%s", psess->recv_count, VTY_NEWLINE);
			vty_out(vty, "	BFD Session Send BFD Negotiation Packets: %lld%s", psess->send_count, VTY_NEWLINE);
			vty_out(vty, "%s", VTY_NEWLINE);
			
			return CMD_SUCCESS;
		}

		if(bfd_session_table.num_entries)
		{
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket && pbucket->data)
				{
					psess = (struct bfd_sess *)pbucket->data;
					sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
					sort_sess = malloc(sizeof(struct list_bfd_sess));
					if(NULL == sort_sess)
					{
						vty_out(vty, "malloc fail!%s", VTY_NEWLINE);
						return CMD_WARNING;
					}
					
					sort_sess->psess = psess;

					if(NULL == sort_sess_head)
					{
						sort_sess_head = sort_sess;
						sort_sess_head->prev = NULL;
						sort_sess_head->next = NULL;
					}
					else
					{
						for(sort_sess_node = sort_sess_head;sort_sess_node;sort_sess_node = sort_sess_node->next)
						{
							sess_id_node = sort_sess_node->psess->session_id_cfg ? sort_sess_node->psess->session_id_cfg : sort_sess_node->psess->session_id;
							if(sess_id < sess_id_node)
							{
								if(sort_sess_node == sort_sess_head)
								{
									sort_sess_node->prev = sort_sess;
									sort_sess->next = sort_sess_node;
									sort_sess->prev = NULL;
									sort_sess_head = sort_sess;
								}
								else
								{
									sort_sess->prev = sort_sess_node->prev;
									sort_sess->next = sort_sess_node;
									sort_sess_node->prev->next = sort_sess;
									sort_sess_node->prev = sort_sess;
								}
								break;
							}

							if(NULL == sort_sess_node->next)
							{
								sort_sess->prev = sort_sess_node;
								sort_sess->next = NULL;
								sort_sess_node->next = sort_sess;
								break;
							}
						}
					}
				}
			}
		}

		if(bfd_session_table.num_entries)
		{
			//HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			for(sort_sess_node = sort_sess_head;sort_sess_node;sort_sess_node = sort_sess_node->next)
			{
				//if(pbucket && pbucket->data)
				{
				//	psess = (struct bfd_sess *)pbucket->data;
					psess = sort_sess_node->psess;
					sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
					local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
					remote_id = psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id;
					++count;
					/*会话上一次down的原因*/
					strcpy(diag, "");
					switch(psess->diag)
					{
						case BFD_DIAG_NO_DIAG:
							strcpy(diag, "no diagnostics");
							break;
						case BFD_DIAG_DETECT_EXPIRE:
							strcpy(diag, "detect timer expire");
							break;
						case BFD_DIAG_NEIGHBOR_DOWN:
							strcpy(diag, "neighbor down");
							break;
						case BFD_DIAG_ADMIN_DOWN:
							strcpy(diag, "admin down");
							break;
							default:
								break;
					}
					
					vty_out(vty, "%d.%s", count, VTY_NEWLINE);
					vty_out(vty, "  BFD Session ID: %d%s", sess_id, VTY_NEWLINE);
					vty_out(vty, "  BFD Session Local Discriminator:  %u%s", local_id, VTY_NEWLINE);
					vty_out(vty, "  BFD Session Remote Discriminator: %u%s", remote_id, VTY_NEWLINE);
					vty_out(vty, "  BFD Session CC Interval Configuration: %dms%s", (psess->cc_interval_cfg)/1000, VTY_NEWLINE);
					vty_out(vty, "  BFD Session CC Send Interval: %dms%s", (psess->cc_interval_send)/1000, VTY_NEWLINE);
					vty_out(vty, "  BFD Session CC Detect Time: %dms%s", (psess->cc_interval_recv)*(psess->remote_multiplier)/1000, VTY_NEWLINE);
					vty_out(vty, "  BFD Session Multiplier Configuration: %d%s", (psess->cc_multiplier_cfg), VTY_NEWLINE);
					vty_out(vty, "  BFD Session Remote Multiplier: %d%s", psess->remote_multiplier, VTY_NEWLINE);
					vty_out(vty, "  BFD Session UP Times: %lld%s", psess->up_count, VTY_NEWLINE);
					vty_out(vty, "  BFD Session DOWN Times: %lld%s", psess->down_count, VTY_NEWLINE);
					vty_out(vty, "  BFD Session Diagnostics: %s%s", diag, VTY_NEWLINE);
					vty_out(vty, "  BFD Session Received BFD Negotiation Packets: %lld%s", psess->recv_count, VTY_NEWLINE);
					vty_out(vty, "  BFD Session Send BFD Negotiation Packets: %lld%s", psess->send_count, VTY_NEWLINE);
					vty_out(vty, "%s", VTY_NEWLINE);
						
					if(count >= bfd_session_table.num_entries)
					{
						while(NULL != sort_sess_head)
						{
							sort_sess_node = sort_sess_head->next;
							free(sort_sess_head);
							sort_sess_head = sort_sess_node;
						}
						
						return CMD_SUCCESS;
					}
				}
			}
		}
    }
    else if(strncmp(argv[0], "config", 6) == 0)
    {
		/*show bfd config*/
		vty_out(vty, "BFD Configuration%s", VTY_NEWLINE);
		vty_out(vty, "-------------------------------------------%s", VTY_NEWLINE);
		
		if(gbfd.enable == 0)
		{
			vty_out(vty, "BFD Disable%s", VTY_NEWLINE);
		}
		else if(gbfd.enable == 1)
		{
			if( argv[1] )
			{
				VTY_GET_INTEGER_RANGE ( "session", sess_id, argv[1], 1, 65535 );
				
				psess = bfd_session_lookup(sess_id);				
				if(NULL == psess)
				{
					vty_warning_out ( vty, "BFD session %d does not exist%s", sess_id, VTY_NEWLINE );
					return CMD_WARNING;
				}
				
				vty_out(vty, "BFD Session ID: %d%s", psess->session_id_cfg, VTY_NEWLINE);
				vty_out(vty, "BFD Session Local Discriminator:  %u%s", psess->local_id_cfg, VTY_NEWLINE);
				vty_out(vty, "BFD Session Remote Discriminator: %u%s", psess->remote_id_cfg, VTY_NEWLINE);
				vty_out(vty, "BFD CC Interval %dms%s", psess->cc_interval_cfg/1000, VTY_NEWLINE);
				vty_out(vty, "BFD Detect Multiplier %d%s", psess->cc_multiplier_cfg, VTY_NEWLINE);
				vty_out(vty, "-------------------------------------------%s", VTY_NEWLINE);
			}
			vty_out(vty, "BFD Global Config%s", VTY_NEWLINE);
			vty_out(vty, "BFD Enable%s", VTY_NEWLINE);
			vty_out(vty, "BFD Ttl %d%s", gbfd.ttl, VTY_NEWLINE);
			vty_out(vty, "BFD Priority %d%s", gbfd.priority, VTY_NEWLINE);
			vty_out(vty, "BFD CC Interval %dms%s", (gbfd.cc_interval)/1000, VTY_NEWLINE);
			vty_out(vty, "BFD Detect Multiplier %d%s", gbfd.multiplier, VTY_NEWLINE);
			if(gbfd.debug_fsm)
			{
				vty_out(vty, "BFD Debug Fsm Enable%s", VTY_NEWLINE);
			}
			else
			{
				vty_out(vty, "BFD Debug Fsm Disable%s", VTY_NEWLINE);
			}
			if(gbfd.debug_packet)
			{
				vty_out(vty, "BFD Debug Packet Enable%s", VTY_NEWLINE);
			}
			else
			{
				vty_out(vty, "BFD Debug Packet Disable%s", VTY_NEWLINE);
			}
		}
    }
	
	return CMD_SUCCESS;
}

/* bfd 配置文件 */
int bfd_config_write (struct vty *vty)
{
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	int cursor = 0;

	if(gbfd.enable == 1)
	{
		//vty_out(vty, "bfd enable%s", VTY_NEWLINE);
		if(BFD_CC_INTERVAL != gbfd.cc_interval)
		{
			vty_out(vty, "bfd cc-interval %d%s", (gbfd.cc_interval)/1000, VTY_NEWLINE);
		}
		if(BFD_DETECT_MULTIPLIER != gbfd.multiplier)
		{
			vty_out(vty, "bfd detect-multiplier %d%s", gbfd.multiplier, VTY_NEWLINE);
		}
		if(BFD_PRIORITY != gbfd.priority)
		{
			vty_out(vty, "bfd priority %d%s", gbfd.priority, VTY_NEWLINE);
		}

		if(BFD_TTL != gbfd.ttl)
		{
			vty_out(vty, "bfd ttl %d%s", gbfd.ttl, VTY_NEWLINE);
		}
		if(gbfd.debug_packet)
		{
			vty_out(vty, "debug bfd packet%s", VTY_NEWLINE);
		}
		if(gbfd.debug_fsm)
		{
			vty_out(vty, "debug bfd fsm%s", VTY_NEWLINE);
		}
		/*显示bfd静态会话的配置*/
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
		    if(!pbucket)
	    	{
	    	    continue;
	    	}
		    if(!(pbucket->data))
	    	{
	    	    continue;
	    	}			

			psess = (struct bfd_sess *)pbucket->data;
			if(psess->session_id_cfg)
			{
				/*静态创建会话*/
				vty_out(vty, "bfd session %d%s", psess->session_id_cfg, VTY_NEWLINE);
			}				
			/*会话配置*/
			if(psess->local_id_cfg)
			{
				vty_out(vty, " local-discriminator %u%s", psess->local_id_cfg, VTY_NEWLINE);
			}
			if(psess->remote_id_cfg)
			{
				vty_out(vty, " remote-discriminator %u%s", psess->remote_id_cfg, VTY_NEWLINE);
			}
			if(psess->cc_interval_cfg)
			{
				vty_out(vty, " bfd cc-interval %d%s", (psess->cc_interval_cfg)/1000, VTY_NEWLINE);
			}
			if(psess->cc_multiplier_cfg)
			{
				vty_out(vty, " bfd detect-multiplier %d%s", psess->cc_multiplier_cfg, VTY_NEWLINE);
			}

		}
	}
	return CMD_SUCCESS;
}

int bfd_config_write_h3c(struct vty *vty)
{
	struct hash_bucket *pbucket = NULL;
	struct bfd_template *ptem = NULL;
	int cursor = 0;

	if(0 == gbfd.bfd_h3c_enable)
	{
		return CMD_SUCCESS;
	}

	if(1 != gbfd.ip_router_alert)
	{
		vty_out(vty, "undo bfd ip-router-alert%s", VTY_NEWLINE);
	}
	if(BFD_ACTIVE != gbfd.bfd_init_mode)
	{
		vty_out(vty, "bfd session init-mode passive%s", VTY_NEWLINE);
	}
	if(BFD_MPLS_DISABLE != gbfd.mpls_bfd_enable)
	{
		vty_out(vty, "mpls bfd enable%s", VTY_NEWLINE);
	}
	if(BFD_SNMP_DISABLE != gbfd.snmp_trap)
	{
		
	}
	if(0 != gbfd.mult_auth)
	{

	}
	if(BFD_DETECT_MULTIPLIER != gbfd.mult_detect)
	{
		vty_out(vty, "bfd multi-hop detect-multiplier %d%s", gbfd.mult_detect, VTY_NEWLINE);
	}
	if(BFD_MULTIHOP_DSTPORT != gbfd.mult_hop_port)
	{
		vty_out(vty, "bfd multi-hop destination-port %d%s",gbfd.mult_hop_port, VTY_NEWLINE);
	}
	if(BFD_CC_INTERVAL != gbfd.mult_hop_cc_rx_interval)
	{
		vty_out(vty, "bfd multi-hop min-receive-interval %u%s", (gbfd.mult_hop_cc_rx_interval)/1000, VTY_NEWLINE);
	}
	if(BFD_CC_INTERVAL != gbfd.mult_hop_cc_tx_interval)
	{
		vty_out(vty, "bfd multi-hop min-transmit-interval %u%s", (gbfd.mult_hop_cc_tx_interval)/1000, VTY_NEWLINE);
	}
	if(gbfd.debug_packet)
	{
		vty_out(vty, "debug bfd packet%s", VTY_NEWLINE);
	}
	if(gbfd.debug_fsm)
	{
		vty_out(vty, "debug bfd fsm%s", VTY_NEWLINE);
	}
	
	/*显示bfd模板的配置*/
	HASH_BUCKET_LOOP(pbucket, cursor, bfd_template_table)
	{
		if(!pbucket)
		{
			continue;
		}
		if(!(pbucket->data))
		{
			continue;
		}			

		ptem = (struct bfd_template *)pbucket->data;
		vty_out(vty, "bfd template %s%s", ptem->template_name, VTY_NEWLINE);
		if(0 != ptem->auth_mode)
		{
			
		}
		if(BFD_CC_INTERVAL != ptem->cc_rx_interval)
		{
			vty_out(vty, " bfd min-receive-interval %d%s", ptem->cc_rx_interval/1000, VTY_NEWLINE);
		}				
		if(BFD_CC_INTERVAL != ptem->cc_tx_interval)
		{
			vty_out(vty, " bfd min-transmit-interval %d%s", ptem->cc_tx_interval/1000, VTY_NEWLINE);
		}
		if(BFD_DETECT_MULTIPLIER != ptem->detect_mult)
		{
			vty_out(vty, " bfd detect-multiplier %d%s", ptem->detect_mult, VTY_NEWLINE);
		}
	}

	return CMD_SUCCESS;
}

void bfd_cmd_init(void)
{
	install_node(&bfd_template_node, bfd_config_write_h3c);
	
	install_default(BFD_TEMPLATE_NODE);

	install_element(CONFIG_NODE, &bfd_ip_router_alert_enable_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_ip_router_alert_enable_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &mpls_bfd_enable_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_mpls_bfd_enable_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_template_config_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_template_config_cmd, CMD_SYNC);
	install_element(BFD_TEMPLATE_NODE, &bfd_detect_multiplier_cmd, CMD_SYNC);
	install_element(BFD_TEMPLATE_NODE, &undo_bfd_detect_multiplier_cmd, CMD_SYNC);
	install_element(BFD_TEMPLATE_NODE, &bfd_min_receive_interval_cmd, CMD_SYNC);
	install_element(BFD_TEMPLATE_NODE, &undo_bfd_min_receive_interval_cmd, CMD_SYNC);
	install_element(BFD_TEMPLATE_NODE, &bfd_min_transmit_interval_cmd, CMD_SYNC);
	install_element(BFD_TEMPLATE_NODE, &undo_bfd_min_transmit_interval_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_multi_hop_authentication_mode_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_multi_hop_authentication_mode_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_multi_hop_dst_port_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_multi_hop_dst_port_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_multi_hop_multiplier_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_multi_hop_multiplier_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_multi_hop_min_receive_interval_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_multi_hop_min_receive_interval_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_multi_hop_min_transmit_interval_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_multi_hop_min_transmit_interval_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_session_init_mode_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_session_init_mode_cmd, CMD_SYNC);


	install_node(&bfd_session_node, bfd_config_write);

	install_default(BFD_SESSION_NODE);

	/*在配置模式下注册命令*/
	/*[no] bfd enable*/
	//install_element(CONFIG_NODE, &enable_bfd_cmd);
	//install_element(CONFIG_NODE, &disable_bfd_cmd);
	/*[no] debug bfd {packet | fsm }*/
	install_element(CONFIG_NODE, &bfd_debug_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_debug_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_up_down_clear_cmd, CMD_SYNC);
	/*bfd priority <0-7>*/
	install_element(CONFIG_NODE, &bfd_priority_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_priority_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &bfd_ttl_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_ttl_cmd, CMD_SYNC);
	/*bfd session <1-65535>, create a session and enter the session view*/
	install_element(CONFIG_NODE, &bfd_session_cmd, CMD_SYNC);
	/*no bfd session <1-65535>, delete a session*/
	install_element(CONFIG_NODE, &undo_bfd_session_cmd, CMD_SYNC);
	/*会话视图remote-discriminator <1-65535>*/
	install_element(BFD_SESSION_NODE, &bfd_session_remote_disc_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &undo_bfd_session_remote_disc_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &bfd_session_local_disc_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &undo_bfd_session_local_disc_cmd, CMD_SYNC);
	
	/*配置视图会话视图同时存在以下命令*/
	/*[no] bfd recv-interval (3 | 10 | 30 | 100 | 300 | 1000)*/
	install_element(CONFIG_NODE, &bfd_session_cc_interval_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_session_cc_interval_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &bfd_session_cc_interval_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &undo_bfd_session_cc_interval_cmd, CMD_SYNC);
	/*[no] bfd detect-multiplier <3-5>*/
	install_element(CONFIG_NODE, &bfd_session_detect_multiplier_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_bfd_session_detect_multiplier_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &bfd_session_detect_multiplier_cmd, CMD_SYNC);
	install_element(BFD_SESSION_NODE, &undo_bfd_session_detect_multiplier_cmd, CMD_SYNC);
	
	/*[no] bfd delay-up <0-3600>*/
	//install_element(CONFIG_NODE, &bfd_session_delay_up_cmd, CMD_SYNC);
	//install_element(CONFIG_NODE, &undo_bfd_session_delay_up_cmd, CMD_SYNC);
	//install_element(BFD_SESSION_NODE, &bfd_session_delay_up_cmd, CMD_SYNC);
	//install_element(BFD_SESSION_NODE, &undo_bfd_session_delay_up_cmd, CMD_SYNC);
	/*接口视图*/
	/*bfd enable {session <1-65535> | source-ip A.B.C.D }*/
	//install_element(PHYSICAL_SUBIF_NODE, &bfd_intf_session_enable_cmd);
	//install_element(TRUNK_IF_NODE, &bfd_intf_session_enable_cmd);
	install_element(PHYSICAL_IF_NODE, &bfd_intf_session_enable_cmd, CMD_SYNC);
	//install_element(PHYSICAL_SUBIF_NODE, &undo_bfd_intf_session_enable_cmd);
	//install_element(TRUNK_IF_NODE, &undo_bfd_intf_session_enable_cmd);
	install_element(PHYSICAL_IF_NODE, &undo_bfd_intf_session_enable_cmd, CMD_SYNC);
	/*bfd 显示命令*/
	install_element(CONFIG_NODE, &bfd_info_show_cmd, CMD_LOCAL);
	install_element(BFD_SESSION_NODE, &bfd_info_show_cmd, CMD_LOCAL);
	install_element(PHYSICAL_IF_NODE, &bfd_info_show_cmd, CMD_LOCAL);
	install_element(TRUNK_IF_NODE, &bfd_info_show_cmd, CMD_LOCAL);
	//install_element(PHYSICAL_SUBIF_NODE, &bfd_info_show_cmd);
}

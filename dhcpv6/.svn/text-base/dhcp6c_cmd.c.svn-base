/******************************************************************************
 * Filename: dhcp6c_cmd.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.23  wumingming created
 *
******************************************************************************/

#include <assert.h>
#include <string.h>
#include <lib/queue.h>
#include <lib/thread.h>
#include <lib/log.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/module_id.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/syslog.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>

#include "dhcp6.h"
#include "common.h"
#include "dhcp6c.h"
#include "auth.h"
#include "dhcpv6_if.h"
#include "dhcp6c_ia.h"
#include "dhcp6c_cmd.h"
#include "base64.h"

extern struct duid client_duid;
extern struct keyinfo ctlkey;
extern int ctldigestlen;
extern struct cmd_node physical_if_node;


static struct cmd_node dhcpv6_node =
{
	DHCPV6_NODE,
	"%s(config-dhcpv6)# ",
	1,
};

int dhcpv6_duid_set(char *duid_string)
{
	FILE *fp = NULL;
	int len;
	
	if (duid_string == NULL) {
		LOG(LOG_ERR, "duid string is NULL!");
		return 1;
	}
	
	fp = fopen(DUID_FILE, "w+");
	if (fp == NULL) {
		LOG(LOG_ERR, "fopen %s is failed!", DUID_FILE);
		return 1;
	}
	len = strlen(duid_string);
	LOG(LOG_DEBUG, "write len %d duid_string %s to %s!", len, duid_string, DUID_FILE);
	if (fwrite(&len, sizeof(len), 1, fp) != 1) {
		LOG(LOG_ERR, "Write duid len to %s is failed!", DUID_FILE);
		fclose(fp);
		return 1;
	}

	if (fwrite(duid_string, len, 1, fp) != 1) {
		LOG(LOG_ERR, "Write duid string to %s is failed!", DUID_FILE);
		fclose(fp);
		return 1;
	}

	fclose(fp);
	return 0;
}

char *show_dhcpv6_duid(void)
{
	FILE *fp = NULL;
	struct duid *duid;
	int len;
	char *duid_string;
	duid_string = malloc(64);
	if (duid_string == NULL) {
		LOG(LOG_ERR, "[%s] Enter %s %d, malloc duid_string is failed!", __FILE__, __FUNCTION__, __LINE__);
		return NULL;
	}
	memset(duid_string, 0, 64);
	fp = fopen(DUID_FILE, "r");
	if (fp == NULL) {
		LOG(LOG_ERR, "fopen %s is failed!", DUID_FILE);
		free(duid_string);
		return NULL;
	}

	if (fread(&len, sizeof(len), 1, fp) <= 0) {
		LOG(LOG_ERR, "fread len is failed!");
	}

	if (len == 0) {
		LOG(LOG_ERR, "len is 0!");
		free(duid_string);
		fclose(fp);
		return NULL;
	}
	duid = malloc(sizeof(len) + len);
	
	if (duid == NULL) {
		LOG(LOG_ERR, "malloc duid is failed!");
		free(duid_string);
		fclose(fp);
		return NULL;
	}

	memset(duid, 0, sizeof(struct duid));

	if ((duid->duid_id = malloc(len)) == NULL) {
		LOG(LOG_ERR, "malloc duid_id is failed!");
		if (duid) {
			free(duid);
		}
		fclose(fp);
		free(duid_string);
		return NULL;
	}
	
	if (fread(duid->duid_id, len, 1, fp) != 1) {
		LOG(LOG_ERR, "fread duid string is failed!");
		if (duid->duid_id) {
			free(duid->duid_id);
		}
		if (duid) {
			free(duid);
		}
		fclose(fp);
		free(duid_string);
		return NULL;
	} else {
		memcpy(duid_string, duid->duid_id, len);
		LOG(LOG_DEBUG, "duid_string is %s", duid_string);
	}
	free(duid->duid_id);
	free(duid);
	fclose(fp);
	return duid_string;
}

int dhcpv6_key_set(char *key_string)
{
	int secretlen;
	ctldigestlen = MD5_DIGESTLENGTH;
	if(key_string != NULL)
	{
		/*
		if ((secretlen = base64_decodestring(key_string, secret, sizeof(secret)))
		    < 0) {
			LOG(LOG_ERR, "failed to decode base64 string");
			return 1;
		}
		*/
		secretlen = strlen(key_string);
		if(ctlkey.secret == NULL){
			ctlkey.secret = malloc(secretlen);
			if(ctlkey.secret == NULL){
				LOG(LOG_ERR, "malloc ctlkey.secret is error!");
				return 1;
			}
		}
		if(ctlkey.secret != NULL){
			memcpy(ctlkey.secret, key_string, secretlen);
		}
		ctlkey.secretlen = secretlen;
		return 0;
	}
	return 1;
}

char *show_dhcpv6_key(void)
{
	char *secret = NULL;
	ctldigestlen = MD5_DIGESTLENGTH;

	if(ctlkey.secretlen != 0){
		secret = malloc(ctlkey.secretlen);
		memcpy(secret, ctlkey.secret, ctlkey.secretlen);
	}
	
	return secret;
}

/* 获取接口模式 */
int dhcpv6_if_get_intf_mode(struct vty *vty, uint32_t module_id)
{
    uint8_t *pmode = NULL;

    pmode = ifm_get_mode((uint32_t)vty->index, MODULE_ID_DHCPV6);
    if (NULL == pmode)
    {
        vty_out(vty, "Error: Get interface mode timeout.%s", VTY_NEWLINE);
        return ERRNO_FAIL;
    }
    else if (*pmode != IFNET_MODE_L3)
    {
        vty_out(vty, "Error: Please select mode l3.%s", VTY_NEWLINE);
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}

int show_dhcpv6_client_status(struct vty *vty, struct dhcpv6_if *ifp)
{
	int state = 0;
//	state = ifp->if_state;
	state = ifp->event.state;
	
	switch (state) {
		case IAS_ACTIVE:
			vty_out (vty, "IA/PD Address status :  %s%s", "ACTIVE", VTY_NEWLINE);
			vty_out (vty, "Bind Address :  %s%s", ifp->ia.ia.iana.addr, VTY_NEWLINE);
			break;
		case IAS_REBIND:
			vty_out (vty, "IA/PD Address status :  %s%s", "REBIND", VTY_NEWLINE);
			break;
		case IAS_RENEW:
			vty_out (vty, "IA/PD Address status :  %s%s", "RENEW", VTY_NEWLINE);
			break;
		default:
			vty_out (vty, "IA/PD Address status :  %s%s", "HALT", VTY_NEWLINE);
			break;
	}
	
	return 0;
}

DEFUN (dhcpv6_mode_enable,
		dhcpv6_mode_enable_cmd,
		"dhcpv6",
		"dhcpv6 command node\n")
{
	vty->node = DHCPV6_NODE;

	return CMD_SUCCESS;
}

DEFUN (dhcpv6_duid,
		dhcpv6_duid_cmd,
		"dhcpv6 duid STRING<1-64>",
		"Dynamic Host Configuration Protocol for IPv6\n"
		"DHCPv6 unique identifier\n"
		"DUID hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")
{
	uint32_t size;
	char *str = NULL;
	
	size = strlen(argv[0]);
	if(size == 0)
	{
		vty_out(vty, "Error:  please input dhcpv6 duid%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	str = (char*)XCALLOC(MTYPE_IF, size + 1);
	if(NULL == str) 
	{
		LOG (LOG_ERR, "Error: There is no ram space\n");		  
	
		return CMD_WARNING;
	}
	memcpy(str, argv[0], size);
	if(0 != dhcpv6_duid_set(str))
	{
		vty_out(vty, "Error: the input secret is invaild.%s", VTY_NEWLINE);
		free(str);
		return CMD_WARNING;
	}
	
	free(str);
	return CMD_SUCCESS;
}

DEFUN (show_dhcpv6_duid_string,
		show_dhcpv6_duid_string_cmd,
		"show dhcpv6 duid",
		"show duid string\n"
		"Dynamic Host Configuration Protocol for IPv6\n"
		"DUID hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")
{
	char *duid_string;
	
	duid_string = show_dhcpv6_duid();
	if (duid_string == NULL) {
		vty_out(vty, "Error: The dhcpv6 duid is NULL.%s", VTY_NEWLINE);
		return CMD_WARNING;
	} else {
		vty_out(vty, "dhcpv6 duid %s%s", duid_string, VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}

DEFUN (dhcpv6_keyinfo,
		dhcpv6_keyinfo_cmd,
		"dhcpv6 key STRING<1-64>",
		"Dynamic Host Configuration Protocol for IPv6\n"
		"DHCPv6  key configure command\n"
		"key hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")
{
	uint32_t size;
	char *str = NULL;
	
	size = strlen(argv[0]);
	if(size == 0)
	{
		vty_out(vty, "Error:  please input dhcpv6 duid%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	str = (char*)XCALLOC(MTYPE_IF, size + 1);
	if(NULL == str) 
	{
		LOG (LOG_ERR, "Error: There is no ram space\n");		  
	
		return CMD_WARNING;
	}
	memcpy(str, argv[0], size);
	if(0 != dhcpv6_key_set(str))
	{
		vty_out(vty, "Error: the input secret is invaild.%s", VTY_NEWLINE);
		free(str);
		return CMD_WARNING;
	}
	free(str);
	return CMD_SUCCESS;
}

DEFUN (show_dhcpv6_key_string,
		show_dhcpv6_key_string_cmd,
		"show dhcpv6 key",
		"show key string\n"
		"Dynamic Host Configuration Protocol for IPv6\n"
		"key hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")
{
	char *key_string;
	
	key_string = show_dhcpv6_key();
	if (key_string == NULL) {
		vty_out(vty, "Error: The dhcpv6 key is NULL.%s", VTY_NEWLINE);
		return CMD_WARNING;
	} else {
		vty_out(vty, "dhcpv6 key %s%s", key_string, VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}
		
DEFUN (ip_address_dhcpv6_alloc,
		ip_address_dhcpv6_alloc_cmd,
		"ethernet USP ip address dhcpv6",
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "Specify IP configurations for interfaces\n"
	    "Address\n"
	    "IP address allocated by DHCPv6\n")
{
	uint32_t ifindex = 0;
	#if 0
	ret = dhcpv6_if_get_intf_mode(vty, MODULE_ID_DHCPV6);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }
	#endif
	if (NULL != argv[0])
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[0]);
        if (0 == ifindex)
        {
            vty_out(vty, "Error: Specify the illegal interface.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
	DHCPV6_LOG_DEBUG("ifindex 0x%x", ifindex);

	if (dhcpv6_client_enable(ifindex)) {
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}

DEFUN (no_ip_address_dhcpv6_alloc,
		no_ip_address_dhcpv6_alloc_cmd,
		"no ethernet USP ip address dhcpv6",
		"disable interface\n"
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "Specify IP configurations for interfaces\n"
	    "Address\n"
	    "IP address allocated by DHCPv6\n")
{
	uint32_t ifindex = 0;
	if (NULL != argv[0])
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[0]);
        if (0 == ifindex)
        {
            vty_out(vty, "Error: Specify the illegal interface.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
	DHCPV6_LOG_DEBUG("ifindex 0x%x", ifindex);

	dhcpv6_client_disable(ifindex);
	return CMD_SUCCESS;
}
#if 0
int set_interface_ia(uint32_t ifindex, char *iana, iatype_t type)
{
	struct dhv6_interface *ifp;
	struct ia_conf *iac;
	int confsize;
	struct listnode *node;
	
    for (node = dhcpv6_if_list.head ; node; node = node->next,ifp=NULL) {
		ifp=listgetdata(node);
		if (ifp->ifid == ifindex) {
				if (type == IATYPE_NA) {
					confsize = sizeof(struct iana_conf);
					if ((iac = malloc(confsize)) == NULL) {
						LOG(LOG_ERR, "memory allocation for IA failed");
					}

					memset(iac, 0, confsize);
					iac->type = IATYPE_NA;
					iac->iaid = atoi(iana);
					TAILQ_INIT(&iac->iadata);
					TAILQ_INSERT_TAIL(&ifp->iaconf_list, iac, link);
					TAILQ_INIT(&((struct iana_conf *)iac)->iana_address_list);
					if (iac)
						free(iac);
					return 0;
				} else if (type == IATYPE_PD){

				}
		}
	}
	
	return 1;
}
DEFUN (interface_iana_set,
		interface_iana_set_cmd,
		"ethernet USP iana STRING",
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "iana\n"
	    "iana string\n")
{
	int ifindex;
	int ret = -1;
	if (argv[0] == NULL) {
		vty_out(vty, "Please input interface name!");
		return CMD_WARNING;
	}
	ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *)argv[0]);
    if (0 == ifindex)
    {
        vty_out(vty, "Error: Specify the illegal interface.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if (argv[1] == NULL) {
		vty_out(vty, "Please input this interface 0x%x iana string!", ifindex);
		return CMD_WARNING;
	}
	ret = set_interface_ia(ifindex, argv[1], IATYPE_NA);
	if (ret == 0) {
		vty_out(vty, "ethernet %s iana %s%s", argv[0], argv[1], VTY_NEWLINE);
	} else {
		vty_out(vty, "set interface ethernet %s iana %s is failed!%s", argv[0], argv[1], VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}
#endif
DEFUN (show_dhcpv6_client,
		show_dhcpv6_client_cmd,
		"show dhcpv6 client",
		"show dhcpv6 clinet status infomation\n"
		"Dynamic Host Configuration Protocol for IPv6\n"
	    "client\n")
{
	struct dhcpv6_if *ifp;
	int ret;
	char ifname[32];
	struct listnode *node;
	
    for (node = dhcpv6_if_list.head ; node; node = node->next,ifp=NULL) {
		ifp=listgetdata(node);
		if (ifp->ifindex) {
			ret = ifm_get_name_by_ifindex(ifp->ifindex, ifname);
			if(ret < 0) assert(0);
			vty_out (vty, "Intreface:  %s%s", ifname, VTY_NEWLINE);
			show_dhcpv6_client_status(vty, ifp);
		}
	}
	return CMD_SUCCESS;
}

DEFUN (ip_address_pd_dhcpv6_alloc,
		ip_address_pd_dhcpv6_alloc_cmd,
		"ethernet USP ip address dhcpv6 pd",
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
		"Specify IP configurations for interfaces\n"
		"Address\n"
		"IP address allocated by DHCPv6\n"
		"IP address allocated by DHCPv6\n")
{
	uint32_t ifindex = 0;
#if 0
	ret = dhcpv6_if_get_intf_mode(vty, MODULE_ID_DHCPV6);
	if (ERRNO_SUCCESS != ret)
	{
		return CMD_WARNING;
	}
#endif
	if (NULL != argv[0])
	{
		ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[0]);
		if (0 == ifindex)
		{
			vty_out(vty, "Error: Specify the illegal interface.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	DHCPV6_LOG_DEBUG("ifindex 0x%x", ifindex);

	if (dhcpv6_client_pd_enable(ifindex)) {
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}
		

DEFUN (no_ip_address_pd_dhcpv6_alloc,
		no_ip_address_pd_dhcpv6_alloc_cmd,
		"no ethernet USP ip address dhcpv6 pd",
		"disable interface\n"
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
		"Specify IP configurations for interfaces\n"
		"Address\n"
		"IP address allocated by DHCPv6\n"
		"IP address allocated by DHCPv6\n")
{
	uint32_t ifindex = 0;
	if (NULL != argv[0])
	{
		ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[0]);
		if (0 == ifindex)
		{
			vty_out(vty, "Error: Specify the illegal interface.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	DHCPV6_LOG_DEBUG("ifindex 0x%x", ifindex);

	dhcpv6_client_pd_disable(ifindex);
	return CMD_SUCCESS;
}
int dhcp6c_cmd_config_write (struct vty *vty)
{
	struct dhcpv6_if *ifp;
	int ret;
	char ifname[32];
	struct listnode *node;
	char *duid_string;
	char *key_string;
	vty_out(vty,"!%s",VTY_NEWLINE);
	vty_out (vty, "dhcpv6%s", VTY_NEWLINE);
    for (node = dhcpv6_if_list.head ; node; node = node->next,ifp=NULL) {
		ifp=listgetdata(node);
		if (ifp->ifindex) {
			ret = ifm_get_name_by_ifindex(ifp->ifindex, ifname);
			if(ret < 0) assert(0);
			//vty_out (vty, " %s ip address dhcpv6%s", ifname, VTY_NEWLINE);
		}
	}
	
	duid_string = show_dhcpv6_duid();
	if (duid_string != NULL) {
		vty_out(vty, "dhcpv6 duid %s%s", duid_string, VTY_NEWLINE);
	}

	key_string = show_dhcpv6_key();
	if(key_string != NULL){
		vty_out(vty, "dhcpv6 key %s%s", key_string, VTY_NEWLINE);
	}
	vty_out (vty, "!%s", VTY_NEWLINE);		
	return 0;
}
void dhcp6c_cmd_init(void)
{
	printf("-----------%s:%d-----------\n", __FUNCTION__, __LINE__);
	install_node(&dhcpv6_node, dhcp6c_cmd_config_write);
	printf("-----------%s:%d-----------\n", __FUNCTION__, __LINE__);
	install_default(DHCPV6_NODE);
	
	printf("-----------%s:%d-----------\n", __FUNCTION__, __LINE__);
	install_element(CONFIG_NODE, &dhcpv6_mode_enable_cmd, CMD_SYNC);
	printf("-----------%s:%d-----------\n", __FUNCTION__, __LINE__);

	install_element(DHCPV6_NODE, &dhcpv6_duid_cmd, CMD_SYNC);
	install_element(DHCPV6_NODE, &dhcpv6_keyinfo_cmd, CMD_SYNC);
	install_element(DHCPV6_NODE, &ip_address_dhcpv6_alloc_cmd, CMD_SYNC);
	install_element(DHCPV6_NODE, &no_ip_address_dhcpv6_alloc_cmd, CMD_SYNC);
	install_element(DHCPV6_NODE, &show_dhcpv6_duid_string_cmd,CMD_LOCAL);
	//install_element(DHCPV6_NODE, &interface_iana_set_cmd,CMD_LOCAL);
	install_element(CONFIG_NODE, &show_dhcpv6_client_cmd,CMD_LOCAL);
	printf("-----------%s:%d-----------\n", __FUNCTION__, __LINE__);

	install_element(DHCPV6_NODE, &ip_address_pd_dhcpv6_alloc_cmd, CMD_SYNC);
	install_element(DHCPV6_NODE, &no_ip_address_pd_dhcpv6_alloc_cmd, CMD_SYNC);

	printf("-----------%s:%d-----------\n", __FUNCTION__, __LINE__);

}



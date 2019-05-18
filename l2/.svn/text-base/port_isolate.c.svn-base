/**
 * @file      : port_isolate.c
 * @brief     : use for EP-Tree or EVP-Tree
 * @details   : 
 * @author    : geqian
 * @date      : 2018/10/18  15:38:50
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : according to OTN test of China Telecom
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <lib/zebra.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/command.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/module_id.h>
#include <lib/pkt_buffer.h>
#include <lib/ifm_common.h>
#include <lib/msg_ipc.h>

#include "l2_if.h"
#include "port_isolate.h"

struct hash_table 	islt_group_table;
struct hash_table   islt_vlan_table;
struct hash_table	islt_port_table;

struct list *pIsltInfo = NULL;

/*******************************************************************************/
/***********************port isolate table information**************************/
/*******************************************************************************/

/**********isolation group table process*************/
static unsigned int islt_grp_compute(void *hash_key)
{
    if (NULL == hash_key)
    {
        return 0;
    }
	
    return ((unsigned int)hash_key);
}

static int islt_grp_compare(void *item, void *hash_key)
{    
	struct hash_bucket *pbucket = NULL;

    /* 参数非空 */
    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pbucket = (struct hash_bucket *)item;
    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

	return ERRNO_FAIL;
}

static void islt_grp_table_init(unsigned int size)
{
    hios_hash_init(&islt_group_table, size, islt_grp_compute, islt_grp_compare);
}

static struct isolate_group *islt_grp_create(uint32_t grpid)
{
	struct isolate_group *pgrp = NULL;
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&islt_group_table, (void *)grpid);
	if (NULL != pitem)
	{
		pgrp = (struct isolate_group *)pitem->data;
		
		return pgrp;
	}	

	pgrp = (struct isolate_group *)XMALLOC(MTYPE_L2, sizeof(struct isolate_group));
	if (NULL == pgrp)
	{
		return NULL;
	}

	memset(pgrp, 0, sizeof(struct isolate_group));

	pgrp->id 		= grpid;
	pgrp->leaf 		= list_new();

	pitem = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pitem)
	{
		XFREE(MTYPE_L2, pgrp);
		return NULL;
	}

	pitem->hash_key = (void *)pgrp->id;
	pitem->data 	= (void *)pgrp;

	hios_hash_add(&islt_group_table, pitem);
	
	return pgrp;
}

static int islt_grp_leaf_add(struct isolate_group *grp, uint32_t ifindex)
{
	struct listnode    *node = NULL;
	
	uint32_t *pif = NULL;
	
	uint32_t mem = ifindex;

	if (NULL == grp)
	{
		return ERRNO_FAIL;
	}
	
	node = listnode_lookup(grp->leaf, (void *)&mem);
	if (NULL != node)
	{
		return ERRNO_SUCCESS;
	}

	zlog_debug(0,"islt group(%d) add leaf(%d)\n", grp->id, ifindex);

	pif = (uint32_t *)XMALLOC(MTYPE_L2, sizeof(uint32_t));
	if (NULL == pif)
	{
		return ERRNO_FAIL;
	}

	*pif = ifindex;
	listnode_add(grp->leaf, (void *)pif);
	
	return ERRNO_SUCCESS;
}

static int islt_grp_leaf_delete(struct isolate_group *grp, uint32_t ifindex)
{
	struct listnode *pnode = NULL;
	
	uint32_t *pif = NULL;
	void *data = NULL;

	if (NULL == grp)
	{
		return ERRNO_FAIL;
	}
	
	
	for(ALL_LIST_ELEMENTS_RO(grp->leaf, pnode, data))
	{
		if (ifindex == *((uint32_t *)data))
		{
			pif = (uint32_t *)data;

			listnode_delete(grp->leaf, (void *)pif);
			XFREE(MTYPE_L2, pif);
			break;
		}
	}

	return ERRNO_SUCCESS;
}

static int islt_grp_delete(uint32_t grpid)
{
	struct isolate_group *pgrp = NULL;
	struct hash_bucket	 *pitem = NULL;

	pitem = hios_hash_find(&islt_group_table, (void *)grpid);
    if (NULL != pitem)
    {   
    	pgrp = (struct isolate_group *)pitem->data;

		if (NULL != pgrp->leaf)
		{
			list_delete_all_node(pgrp->leaf);
			list_free(pgrp->leaf);
		}

		XFREE(MTYPE_L2, pgrp);
		hios_hash_delete(&islt_group_table, pitem);
        XFREE(MTYPE_HASH_BACKET, pitem);
    }

	return ERRNO_SUCCESS;
}

static struct isolate_group *islt_grp_lookup(uint32_t grpid) 
{
	struct hash_bucket	*islt_item = NULL;

	islt_item = hios_hash_find(&islt_group_table, (void *)grpid);
	if (NULL == islt_item)
	{
		return NULL;
	}	

	return (struct isolate_group *)islt_item->data;
}

static void islt_grp_root_set(struct isolate_group *pgrp, uint32_t port)
{
	if (NULL == pgrp)
	{
		return;
	}

	pgrp->root = port;
}

/**********isolation vlan table process*************/
static unsigned int islt_vlan_compute(void *hash_key)
{
    if (NULL == hash_key)
    {
        return 0;
    }
	
    return ((unsigned int)hash_key);
}

static int islt_vlan_compare(void *item, void *hash_key)
{    
	struct hash_bucket *pbucket = NULL;

    /* 参数非空 */
    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pbucket = (struct hash_bucket *)item;
    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

static void islt_vlan_table_init(unsigned int size)
{
    hios_hash_init(&islt_vlan_table, size, islt_vlan_compute, islt_vlan_compare);
}

static struct isolate_group *islt_vlan_create(uint32_t vlanid)
{
	struct isolate_group *pgrp = NULL;
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&islt_vlan_table, (void *)vlanid);
	if (NULL != pitem)
	{
		pgrp = (struct isolate_group *)pitem->data;
		
		return pgrp;
	}	

	pgrp = (struct isolate_group *)XMALLOC(MTYPE_L2, sizeof(struct isolate_group));
	if (NULL == pgrp)
	{
		return NULL;
	}

	memset(pgrp, 0, sizeof(struct isolate_group));

	pgrp->id 		= vlanid;
	pgrp->leaf 		= list_new();

	pitem = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pitem)
	{
		XFREE(MTYPE_L2, pgrp);
		return NULL;
	}

	pitem->hash_key = (void *)(pgrp->id);
	pitem->data 	= (void *)pgrp;

	hios_hash_add(&islt_vlan_table, pitem);
	
	return pgrp;
}

static int islt_vlan_leaf_add(struct isolate_group *grp, uint32_t ifindex)
{
	struct listnode    *node = NULL;
	
	uint32_t *pif = NULL;
	
	uint32_t mem = ifindex;

	if (NULL == grp)
	{
		return ERRNO_FAIL;
	}
	
	node = listnode_lookup(grp->leaf, (void *)&mem);
	if (NULL != node)
	{
		return ERRNO_SUCCESS;
	}

	pif = (uint32_t *)XMALLOC(MTYPE_L2, sizeof(uint32_t));
	if (NULL == pif)
	{
		return ERRNO_FAIL;
	}

	*pif = ifindex;
	listnode_add(grp->leaf, (void *)pif);
	
	return ERRNO_SUCCESS;
}

static int islt_vlan_leaf_delete(struct isolate_group *grp, uint32_t ifindex)
{
	struct listnode *pnode = NULL;
	
	uint32_t *pif = NULL;
	void *data = NULL;

	if (NULL == grp)
	{
		return ERRNO_FAIL;
	}
	
	
	for(ALL_LIST_ELEMENTS_RO(grp->leaf, pnode, data))
	{
		if (ifindex == *((uint32_t *)data))
		{
			pif = (uint32_t *)data;
			break;
		}
	}

	listnode_delete(grp->leaf, (void *)pif);
	XFREE(MTYPE_L2, pif);

	return ERRNO_SUCCESS;
}

static int islt_vlan_delete(uint32_t vlanid)
{
	struct isolate_group *pgrp = NULL;
	struct hash_bucket	 *pitem = NULL;

	pitem = hios_hash_find(&islt_vlan_table, (void *)vlanid);
    if (NULL != pitem)
    {   
    	pgrp = (struct isolate_group *)pitem->data;

		if (NULL != pgrp->leaf)
		{
			list_delete_all_node(pgrp->leaf);
			list_free(pgrp->leaf);
		}

		XFREE(MTYPE_L2, pgrp);
		hios_hash_delete(&islt_vlan_table, pitem);
        XFREE(MTYPE_HASH_BACKET, pitem );
    }

	return ERRNO_SUCCESS;
}

static struct isolate_group *islt_vlan_lookup(uint32_t vlanid) 
{
	struct hash_bucket	*islt_item = NULL;

	islt_item = hios_hash_find(&islt_vlan_table, (void *)vlanid);
	if (NULL == islt_item)
	{
		return NULL;
	}	

	return (struct isolate_group *)islt_item->data;
}

static void islt_vlan_root_set(struct isolate_group *pgrp, uint32_t port)
{
	if (NULL == pgrp)
	{
		return;
	}

	pgrp->root = port;
}

/**********isolation port table process*************/
static unsigned int islt_port_compute(void *hash_key)
{
	struct islt_key *pkey = NULL;
	unsigned int keyvalue = 0;

	pkey = (struct islt_key *)hash_key;
    if (NULL == pkey)
    {
        return 0;
    }
	
	keyvalue = (pkey->type == EP_TREE) ? (pkey->ifindex + 6000) : (pkey->ifindex + pkey->id);

    return keyvalue;
}

static int islt_port_compare(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = NULL;
	struct islt_key *pkey1 = NULL;
	struct islt_key *pkey2 = NULL;

    /* 参数非空 */
    if ((NULL == item) || (NULL == hash_key))
    {
        return 1;
    }
	
	pbucket = (struct hash_bucket *)item;
	pkey1 = (struct islt_key *)(pbucket->hash_key);
	pkey2 = (struct islt_key *) hash_key;
	if ((pkey1->ifindex == pkey2->ifindex)
			&& (pkey1->type == pkey2->type)
			&& (pkey1->id == pkey2->id))
	{
        return 0;
    }

    return 1;
}

static void islt_port_table_init(unsigned int size)
{
    hios_hash_init(&islt_port_table, size, islt_port_compute, islt_port_compare);
}

static int islt_port_create(uint32_t ifindex, enum ISLT_TYPE type, uint32_t id, uint32_t root)
{
	struct isolate_port *pislt = NULL;
	struct hash_bucket  *pitem = NULL;
	struct islt_key key;

	memset(&key, 0, sizeof(struct islt_key));

	key.ifindex = ifindex;
	key.id      = id;
	key.type    = type;

	pitem = hios_hash_find(&islt_port_table, (void *)(&key));
	if (NULL != pitem)
	{
		if (ifindex == pislt->root_port)
		{
			pislt->root_port = root;
			//pislt->state     = ACTIVE;
		}

		return ERRNO_SUCCESS;
	}	

	pislt = (struct isolate_port *)XMALLOC(MTYPE_L2, sizeof(struct isolate_port));
	if (NULL == pislt)
	{
		return ERRNO_FAIL;
	}

	pislt->key.ifindex = ifindex;
	pislt->key.id      = id;
	pislt->key.type    = type;
	pislt->state       = INACTIVE;
	pislt->root_port   = root;

	if (ifindex == root)
	{
		pislt->role = ROOT;
	}

	pitem = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pitem)
	{
		XFREE(MTYPE_L2, pislt);
		return ERRNO_FAIL;
	}

	pitem->hash_key = (void *)&(pislt->key);
	pitem->data 	= (void *)pislt;

	hios_hash_add(&islt_port_table, pitem);
	
	return ERRNO_SUCCESS;
}

static int islt_port_delete(struct islt_key *key)
{
	struct hash_bucket	*pitem = NULL;
	struct isolate_port *pislt = NULL;

	pitem = hios_hash_find(&islt_port_table, (void *)key);
    if (NULL != pitem)
    {   
		pislt = (struct isolate_port *)pitem->data;
		if (NULL != pislt)
		{
        	XFREE(MTYPE_L2, pislt);
		}

		hios_hash_delete(&islt_port_table, pitem);
        XFREE(MTYPE_HASH_BACKET, pitem);
    }

	return ERRNO_SUCCESS;
}

static struct isolate_port *islt_port_lookup(struct islt_key *key) 
{
	struct hash_bucket	*pitem = NULL;

	pitem = hios_hash_find(&islt_port_table, (void *)key);
	if (NULL == pitem)
	{
		return NULL;
	}	

	return (struct isolate_port *)pitem->data;
}



/*******************************************************************************/
/***********************port isolate command information************************/
/*******************************************************************************/
static int port_isolate_group_enable(uint32_t ifindex, uint32_t grpid, enum ISLT_ROLE role)
{
	struct isolate_group *pgrp = NULL;
	struct isolate_port *pislt = NULL;
	struct listnode *pnode = NULL;

	struct port_islt_msg sndmsg;
	struct islt_key key;
	
	void *data = NULL;

	uint32_t leaf_port = 0;
	int i = 0;

	memset(&key, 0, sizeof(struct islt_key));
	memset(&sndmsg, 0, sizeof(struct port_islt_msg));

	key.type = EP_TREE;
	key.id   = grpid;
	
	/*save message to isolate global table*/
	pgrp = islt_grp_create(grpid);
	if (NULL == pgrp)
	{
		return CMD_WARNING;
	}

	if (pgrp->root && (ROOT == role) && (pgrp->root != ifindex))
	{
		zlog_err("Group %d had set root port\n", grpid);
		return CMD_WARNING;
	}

	if (pgrp->root == ifindex)
	{
		zlog_err("Port has been set root\n");
		return CMD_WARNING;
	}

	for(ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
	{
		if (ifindex == *((uint32_t *)data))
		{
			zlog_err("Port has been set leaf\n");
			return CMD_WARNING;
		}
	}

	zlog_debug(0,"%s(%d):create isolation group(%d) role(%s)\n\r", \
			__FUNCTION__, __LINE__, grpid, role ? "root": "leaf");

	switch (role)
	{
		case ROOT:
			islt_grp_root_set(pgrp, ifindex);
			break;
		
		case LEAF:
			islt_grp_leaf_add(pgrp, ifindex);
			break;
			
		default:
			return CMD_WARNING;
	}

	/*save message to isolate port table*/
	islt_port_create(ifindex, EP_TREE, grpid, pgrp->root);

	zlog_debug(0,"%s(%d): save message to isolate port table ifindex:%d root:%d\n",\
			__FUNCTION__, __LINE__, ifindex, pgrp->root);

	/*send message to hal*/
	zlog_debug(0,"root:%d, leaf num:%d\n", pgrp->root, listcount(pgrp->leaf));

	if ((pgrp->root) && (listcount(pgrp->leaf) > 0))
	{
		for(ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
		{
			leaf_port = *(uint32_t *)data;
			key.ifindex = leaf_port;
			pislt = islt_port_lookup(&key);
			if (NULL == pislt)
			{
				continue;
			}

			if (INACTIVE == pislt->state)
			{
				sndmsg.leafid[i++] = *(uint32_t *)data;

				pislt->state     = ACTIVE;
				pislt->root_port = pgrp->root;
			}
		}
		
		if (i)
		{
			/*send to hal for setting leaf-port forwarding to dest-port: root*/
			sndmsg.type   = EP_TREE;
			sndmsg.rootid = pgrp->root;
			sndmsg.vlanid = 0;
			sndmsg.opt	  = ISLT_ADD;

			#if 0
			ipc_send_hal((void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, \
				MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			#endif

			ipc_send_msg_n2( (void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, MODULE_ID_L2,	
						IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			
			zlog_debug(0,"%s(%d):send message leaf port num(%d) isolate root-port(%d)\n", \
					__FUNCTION__, __LINE__, i, pgrp->root);
		}
	}

	return CMD_SUCCESS;
}

static int port_isolate_vlan_enable(uint32_t ifindex, uint32_t vid, enum ISLT_ROLE role)
{
	struct isolate_group *pgrp = NULL;
	struct isolate_port *pislt = NULL;
	struct listnode *pnode = NULL;

	struct port_islt_msg sndmsg;
	struct islt_key key;
	
	void *data = NULL;

	int i = 0;

	memset(&key, 0, sizeof(struct islt_key));
	memset(&sndmsg, 0, sizeof(struct port_islt_msg));

	key.type = EVP_TREE;
	key.id   = vid;

	/*save message to isolate global table*/
	pgrp = islt_vlan_create(vid);
	if (NULL == pgrp)
	{
		return CMD_WARNING;
	}

	if (pgrp->root == ifindex)
	{
		zlog_err("Port has been set root\n");
		return CMD_WARNING;
	}

	if (pgrp->root && (ROOT == role) && (pgrp->root != ifindex))
	{
		zlog_err("Group %d had set root port\n", vid);
		return CMD_WARNING;
	}

	for(ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
	{
		if (ifindex == *((uint32_t *)data))
		{
			zlog_err("Port has been set leaf\n");
			return CMD_WARNING;
		}
	}

	zlog_debug(0,"%s(%d):create isolation vid(%d) role(%s)\n\r", \
			__FUNCTION__, __LINE__, vid, role ? "root": "leaf");

	switch (role)
	{
		case ROOT:
			islt_vlan_root_set(pgrp, ifindex);
			break;
		
		case LEAF:
			islt_vlan_leaf_add(pgrp, ifindex);
			break;
			
		default:
			return CMD_WARNING;
	}

	/*save message to isolate port table*/
	islt_port_create(ifindex, EVP_TREE, vid, role);

	zlog_debug(0,"%s(%d): save message to isolate port table\n", __FUNCTION__, __LINE__);

	/*send message to hal*/
	zlog_debug(0,"%s(%d): root:%d, leaf num:%d\n", \
			__FUNCTION__, __LINE__, pgrp->root, listcount(pgrp->leaf));
	
	if ((pgrp->root) && (listcount(pgrp->leaf) > 0))
	{
		for (ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
		{
			key.ifindex = *((uint32_t *)data);
			pislt = islt_port_lookup(&key);
			if (NULL == pislt)
			{
				continue;
			}

			if (INACTIVE == pislt->state)
			{
				pislt->state     = ACTIVE;
				pislt->root_port = pgrp->root;
				sndmsg.leafid[i++] = *((uint32_t *)data);
			}
		}

		/*send to hal for setting leaf-port forwarding to dest-port: root*/
		if (i)
		{
			sndmsg.type   = EVP_TREE;
			sndmsg.rootid = pgrp->root;
			sndmsg.vlanid = vid;
			sndmsg.opt	  = ISLT_ADD;

			#if 0
			ipc_send_hal((void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, \
					MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			#endif

			ipc_send_msg_n2( (void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, MODULE_ID_L2,	
				IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);

			zlog_debug(0,"%s(%d):send to hal leaf port num(%d) isolate root-port(%d)\n", \
					__FUNCTION__, __LINE__, i, pgrp->root);
		}
	}

	return CMD_SUCCESS;
}

DEFUN(port_isolate,
		port_isolate_cmd,
		"port-isolation (group| vlan) ID {root}",
		"Port isolate config\n"
		"Port isolation group\n"
		"Port isolation vlan\n"
		"Port isolation group id or vlan id: <1-4094>\n"
		"Port role\n")
{
	struct port_islt pislt_info;
	enum ISLT_ROLE role = LEAF;
	enum ISLT_TYPE type;

	uint32_t ifindex;
	uint32_t grpid = 0;
	uint16_t vid = 0;
	uint8_t flag = 0;

	struct port_islt *pislt = NULL;
	struct listnode *node = NULL;
	struct l2if *pif = NULL;
	uint8_t mode = 0;
	void *data = NULL;
	int ret = 0;
	
	ifindex = (uint32_t)vty->index;

	pif = l2if_lookup(ifindex);
	if (NULL == pif)
	{
		vty_error_out(vty, "Invalid ifindex%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (strncmp(argv[0], "group", 5) == 0 )
	{
		VTY_GET_INTEGER_RANGE ("group", grpid, argv[1], ISLT_GROUP_MIN, ISLT_GROUP_MAX);
		type = EP_TREE;
	}
	else if (strncmp(argv[0], "vlan", 4) == 0 )
	{
		VTY_GET_INTEGER_RANGE ("vlan", vid, argv[1], 1, 4094);
		type = EVP_TREE;
	}
	else
	{
		vty_error_out (vty, "Input para error%s", VTY_NEWLINE);
		
		return CMD_WARNING; 
	}

	if (NULL != argv[2])
	{
		role = ROOT;
	}

	/*check port mode and vlan*/
	if (ifm_get_mode(ifindex, MODULE_ID_L2, &mode))
	{
		vty_error_out(vty, "Fail to get port mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if ((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		vty_error_out(vty, "Port mode error%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (EVP_TREE == type)
	{
		if (NULL != listhead(pif->switch_info.vlan_list))
		{
			for (ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, node, data))
			{
				if (vid == (uint32_t)data)
				{
					flag = 1;
					break;
				}
			}
		}

		if (0 == flag)
		{
			vty_error_out(vty, "Vlan %d is not add to port%s", vid, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	zlog_debug(0,"port isolate enable: %s, id:%d, role:%s\n", \
			(EP_TREE == type) ? "EP-Tree": "EVP-Tree", (EP_TREE == type) ? grpid : vid, (ROOT == role) ? "root": "leaf");
	
	ret =  (EP_TREE == type) ? port_isolate_group_enable(ifindex, grpid, role): port_isolate_vlan_enable(ifindex, vid, role);
	if (CMD_SUCCESS != ret)
	{
		vty_error_out(vty, "Port isolate enable failed%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*use for start config*/
	if (NULL == pif->pislt)
	{
		pif->pislt = list_new();
		if (NULL == pif->pislt)
		{
			vty_error_out(vty, "Save config information fail%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		memset(&pislt_info, 0, sizeof(struct port_islt));
		pislt_info.id = (EP_TREE == type) ? grpid : vid;
		pislt_info.type  = type;
		pislt_info.role  = role;

		if (listnode_lookup(pif->pislt, &pislt))
		{
			return CMD_SUCCESS;
		}
	}

	
	pislt = (struct port_islt *)XMALLOC(MTYPE_L2, sizeof(struct port_islt));
	if (NULL == pislt)
	{
		vty_error_out(vty, "Save config information fail%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	pislt->id = (EP_TREE == type) ? grpid : vid;
	pislt->type  = type;
	pislt->role  = role;

	listnode_add(pif->pislt, (void *)pislt);

	return CMD_SUCCESS;
}

static int port_isolate_group_disable(struct vty *vty, uint32_t ifindex, uint32_t grpid)
{
	struct isolate_group *pgrp = NULL;
	struct isolate_port  *pislt = NULL;
	struct listnode      *pnode = NULL;

	struct port_islt_msg sndmsg;
	struct islt_key key;
	enum ISLT_ROLE role;
	
	void *data = NULL;

	int i = 0;

	memset(&key, 0, sizeof(struct islt_key));
	memset(&sndmsg, 0, sizeof(struct port_islt_msg));

	key.type = EP_TREE;
	key.id   = grpid;

	pgrp = islt_grp_lookup(grpid);
	if (NULL == pgrp)
	{
		vty_error_out(vty, "Not found group(%d) information%s", grpid, VTY_NEWLINE);
		return CMD_WARNING;
	}

	role = (pgrp->root == ifindex) ? ROOT : LEAF;

	/*clear all config on chip*/
	if (ROOT == role)
	{
		if (listcount(pgrp->leaf))
		{
			/*send msg to hal*/
			for (ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
			{
				key.ifindex = *((uint32_t *)data);
				pislt = islt_port_lookup(&key);
				if (NULL == pislt)
				{
					continue;
				}

				if (ACTIVE == pislt->state)
				{
					sndmsg.leafid[i++] = *((uint32_t *)data);
					pislt->state 	   = INACTIVE;
					pislt->root_port   = 0;
				}
			}
		
			if (i)
			{
				/*send to hal for setting leaf-port forwarding to dest-port: root*/
				sndmsg.type   = EP_TREE;
				sndmsg.rootid = pgrp->root;
				sndmsg.opt	  = ISLT_DELETE;

				#if 0
				ipc_send_hal((void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, \
						MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
				#endif

				ipc_send_msg_n2( (void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, MODULE_ID_L2,	
						IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
				
				zlog_debug(0,"%s(%d): port isolate disable: %s, id:%d, role:%s\n", \
						__FUNCTION__, __LINE__, "EP-Tree", grpid, (ROOT == role) ? "root": "leaf");
			}
		}
		
		/*delete all group*/
		if (0 == listcount(pgrp->leaf))
		{
			islt_grp_delete(pgrp->id);
		}
		else
		{
			pgrp->root = 0;
		}
	}
	else
	{
		/*clear config between leaf(this port) and root*/
		key.ifindex = ifindex;

		if (pgrp->root)
		{
			pislt = islt_port_lookup(&key);
			if (NULL == pislt)
			{
				return CMD_SUCCESS;
			}

			if (ACTIVE == pislt->state)
			{
				sndmsg.leafid[0] = ifindex;
				pislt->state 	 = INACTIVE;

				/*send msg to hal*/
				sndmsg.type   = EP_TREE;
				sndmsg.rootid = pgrp->root;
				sndmsg.opt	  = ISLT_DELETE;

				#if 0
				ipc_send_hal((void *)&sndmsg, sizeof(struct port_islt_msg), 1, MODULE_ID_HAL, \
						MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
				#endif

				ipc_send_msg_n2( (void *)&sndmsg, sizeof(struct port_islt_msg), 1, MODULE_ID_HAL, MODULE_ID_L2,	
						IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
				
				zlog_debug(0,"%s(%d): port isolate disable: %s, id:%d, role:%s\n", \
						__FUNCTION__, __LINE__, "EP-Tree", grpid, (ROOT == role) ? "root": "leaf");
			}
		}
		
		/*clear islt_glb_table info and islt_port_table info*/
		islt_grp_leaf_delete(pgrp, ifindex);

		islt_port_delete(&key);

		if ((0 == pgrp->root) && (0 == listcount(pgrp->leaf)))
		{
			islt_grp_delete(pgrp->id);
		}
	}

	return CMD_SUCCESS;
}

static int port_isolate_vlan_disable(struct vty *vty, uint32_t ifindex, uint32_t vid)
{
	struct isolate_group *pgrp = NULL;
	struct isolate_port  *pislt = NULL;
	struct listnode      *pnode = NULL;

	struct port_islt_msg sndmsg;
	struct islt_key key;
	enum ISLT_ROLE role;
	
	void *data = NULL;

	int i = 0;

	memset(&key, 0, sizeof(struct islt_key));
	memset(&sndmsg, 0, sizeof(struct port_islt_msg));

	key.type = EVP_TREE;
	key.id   = vid;

	pgrp = islt_vlan_lookup(vid);
	if (NULL == pgrp)
	{
		vty_error_out(vty, "Not find vlan(%d) information%s", vid, VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&sndmsg, 0, sizeof(struct port_islt_msg));

	role = (pgrp->root == ifindex) ? ROOT : LEAF;

	/*clear all config on chip*/
	if (ROOT == role)
	{
		/*send msg to hal*/
		for (ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
		{
			key.ifindex = *((uint32_t *)data);
			pislt = islt_port_lookup(&key);
			if (NULL == pislt)
			{
				continue;
			}

			if (ACTIVE == pislt->state)
			{
				sndmsg.leafid[i++] = *(uint32_t *)data;
				pislt->state 	   = INACTIVE;
				pislt->root_port   = 0;
			}
		}
		
		/*send to hal for setting leaf-port forwarding to dest-port: root*/
		if (i)
		{
			sndmsg.type   = EVP_TREE;
			sndmsg.rootid = pgrp->root;
			sndmsg.opt	  = ISLT_DELETE;
			sndmsg.vlanid = vid;

			#if 0
			ipc_send_hal((void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, \
				MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			#endif

			ipc_send_msg_n2( (void *)&sndmsg, sizeof(struct port_islt_msg), i, MODULE_ID_HAL, MODULE_ID_L2,	
						IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			
			zlog_debug(0,"%s(%d): port isolate disable: %s, id:%d, role:%s\n", \
						__FUNCTION__, __LINE__, "EVP-Tree", vid, (ROOT == role) ? "root": "leaf");
		}
		
		/*delete all group*/
		if (0 == listcount(pgrp->leaf))
		{
			islt_vlan_delete(pgrp->id);
		}
		else
		{
			pgrp->root = 0;
		}
	}
	else
	{
		/*clear config between leaf(this port) and root*/
		key.ifindex = ifindex;

		if (pgrp->root)
		{
			pislt = islt_port_lookup(&key);
			if (NULL == pislt)
			{
				return CMD_SUCCESS;
			}

			if (ACTIVE == pislt->state)
			{
				sndmsg.leafid[0] = ifindex;
				pislt->state  = INACTIVE;

				/*send msg to hal*/
				sndmsg.type   = EVP_TREE;
				sndmsg.rootid = pgrp->root;
				sndmsg.opt	  = ISLT_DELETE;
				sndmsg.vlanid = vid;
			}

			#if 0
			ipc_send_hal((void *)&sndmsg, sizeof(struct port_islt_msg), 1, MODULE_ID_HAL, \
				MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			#endif

			ipc_send_msg_n2( (void *)&sndmsg, sizeof(struct port_islt_msg), 1, MODULE_ID_HAL, MODULE_ID_L2,	
						IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_UPDATE, ifindex);
			
			zlog_debug(0,"%s(%d): port isolate disable: %s, id:%d, role:%s\n", \
						__FUNCTION__, __LINE__, "EVP-Tree", vid, (ROOT == role) ? "root": "leaf");
		}
		
		/*clear islt_vlan_table info and islt_port_table info*/
		islt_vlan_leaf_delete(pgrp, ifindex);

		islt_port_delete(&key);

		if ((0 == pgrp->root) && (0 == listcount(pgrp->leaf)))
		{
			islt_grp_delete(pgrp->id);
		}
	}

	return CMD_SUCCESS;
}

DEFUN(no_port_isolate,
		no_port_isolate_cmd,
		"no port-isolation (group| vlan) ID",
		NO_STR
		"port isolate config\n"
		"Port isolation group\n"
		"Port isolation vlan\n"
		"Port isolation group id or vlan id: <1-4094>\n")
{
	struct port_islt pislt_info;
	enum ISLT_TYPE type;
	enum ISLT_ROLE role;
	
	uint32_t ifindex;
	uint32_t grpid = 0;
	uint32_t vid = 0;
	uint8_t flag = 0;

	struct isolate_group *pgrp = NULL;
	struct port_islt *pislt = NULL;
	struct listnode *node = NULL;
	struct l2if *pif = NULL;
	uint8_t mode = 0;
	void *data = NULL;
	int ret;
		
	ifindex = (uint32_t)vty->index;

	if (strncmp(argv[0], "group", 5) == 0 )
	{
		VTY_GET_INTEGER_RANGE ("group", grpid, argv[1], ISLT_GROUP_MIN, ISLT_GROUP_MAX);
		type = EP_TREE;
	}
	else if (strncmp(argv[0], "vlan", 4) == 0 )
	{
		VTY_GET_INTEGER_RANGE ("vlan", vid, argv[1], 1, 4094);
		type = EVP_TREE;
	}
	else
	{
		vty_error_out (vty, "Input para error%s", VTY_NEWLINE);
		
		return CMD_WARNING; 
	}

	/*check port mode and vlan*/
	if (ifm_get_mode(ifindex, MODULE_ID_L2, &mode))
	{
		vty_error_out(vty, "Fail to get port mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if ((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		vty_error_out(vty, "Port mode error%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	pif = l2if_lookup(ifindex);
	if (NULL == pif)
	{
		vty_error_out(vty, "Invalid ifindex%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (EVP_TREE == type)
	{
		if (NULL != listhead(pif->switch_info.vlan_list))
		{
			for (ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, node, data))
			{
				if (vid == (uint32_t)data)
				{
					flag = 1;
					break;
				}
			}
		}

		if (0 == flag)
		{
			vty_error_out(vty, "Vlan %d is not add to port%s", vid, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/*judge port role*/
	pgrp = ((EP_TREE == type) ? islt_grp_lookup(grpid) : islt_vlan_lookup(vid));
	if (NULL == pgrp)
	{
		vty_error_out(vty, "Not find group information%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	role = (pgrp->root == ifindex) ? ROOT : LEAF;

	zlog_debug(0,"port isolate disable: %s, id:%d\n", \
			(EP_TREE == type) ? "EP-Tree": "EVP-Tree", (EP_TREE == type) ? grpid : vid);

	ret = (EP_TREE == type) ? port_isolate_group_disable(vty, ifindex, grpid): port_isolate_vlan_disable(vty, ifindex, vid);
	if (ret != CMD_SUCCESS)
	{
		vty_error_out(vty, "Port isolate disable fail%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*delete config information*/
	role = (pgrp->root == ifindex) ? ROOT : LEAF;
	memset(&pislt_info, 0, sizeof(struct port_islt));
	pislt_info.id = ((EP_TREE == type) ? grpid : vid);
	pislt_info.type  = type;
	pislt_info.role  = role;

	node = listnode_lookup(pif->pislt, &pislt_info);
	if (node)
	{
		pislt = (struct port_islt *) (node->data);
		XFREE(MTYPE_L2, pislt);

		list_delete_node(pif->pislt, node);
	}

	if (0 == listcount(pif->pislt))
	{
		XFREE(MTYPE_L2, pif->pislt);
	}

	return CMD_SUCCESS;
}

static void port_isolation_group_show(struct vty *vty, uint32_t grpid)
{
	struct isolate_group *pgrp = NULL;
	struct listnode *pnode = NULL;
	void *data = NULL;
	
	char name[20];
	uint8_t state = 0;
	int curr = 0;
  	int len = 0;
	
	pgrp = islt_grp_lookup(grpid);
	if (NULL == pgrp)
	{
		return;
	}

	if (pgrp->root && (listcount(pgrp->leaf) > 0))
	{
		state = 1;
	}

	ifm_get_name_by_ifindex(pgrp->root, name);
		
	//vty_out(vty," %-15s%-10s%-10s%-23s%-23s%s", "isolate-group", "type", "state", "root", "leaf", VTY_NEWLINE);

	vty_out(vty," %-15d%-10s%-10s%-23s", grpid, "EP-TREE", state? "ACTIVE": "INACTIVE", name);

	curr = 59;
	for (ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
	{
		ifm_get_name_by_ifindex(*((uint32_t *)data), name);

		/*1 for blank.*/
		len = strlen(name) + 1;

		if ((curr + len) <= 82)
        {
        	vty_out (vty, "%s", name);
            curr += len;
        }
        else
        {
            vty_out (vty, "%s", VTY_NEWLINE);
            vty_out (vty, "%59s%s", " ", name);
            curr = len;
        }
	}

	vty_out(vty,"%s", VTY_NEWLINE);
}

static void port_isolation_group_all_show(struct vty *vty)
{
	struct hash_bucket *pbucket = NULL;
	struct isolate_group *pgrp = NULL;
	void *cursor = NULL;

	pbucket = hios_hash_start(&islt_group_table, &cursor);
	if (NULL == pbucket)
	{
		return;
	}

	for (; pbucket; pbucket = hios_hash_next(&islt_group_table, &cursor))
	{
		pgrp = (struct isolate_group *)pbucket->data;
		if (NULL == pgrp)
		{
			continue;
		}

		port_isolation_group_show(vty, pgrp->id);
	}
}

static void port_isolation_vlan_show(struct vty *vty, uint32_t vid)
{
	struct isolate_group *pgrp = NULL;
	struct listnode *pnode = NULL;
	void *data = NULL;
	
	char name[20];
	uint8_t state = 0;
	int curr = 0;
  	int len = 0;
	
	if (vid)
	{
		pgrp = islt_vlan_lookup(vid);
		if (NULL == pgrp)
		{
			return;
		}

		if (pgrp->root && (listcount(pgrp->leaf) > 0))
		{
			state = 1;
		}

		ifm_get_name_by_ifindex(pgrp->root, name);
		
		//vty_out(vty," %-8s%-10s%-10s%-23s%-32s%s", "vlan", "type", "state", "root", "leaf", VTY_NEWLINE);

		vty_out(vty," %-8d%-10s%-10s%-23s", vid, "EVP-TREE", state? "ACTIVE": "INACTIVE", name);

		curr = 52;
		for (ALL_LIST_ELEMENTS_RO(pgrp->leaf, pnode, data))
		{
			ifm_get_name_by_ifindex(*((uint32_t *)data), name);

			/*1 for blank.*/
			len = strlen (name) + 1;

			if ((curr + len) <= 84)
            {
              	vty_out (vty, "%s", name);
              	curr += len;
            }
         	else
            {
              	vty_out (vty, "%s", VTY_NEWLINE);
              	vty_out (vty, "%52s%s", " ", name);
              	curr = len;
            }
		}

		vty_out(vty, "%s", VTY_NEWLINE);
	}
}

static void port_isolation_vlan_all_show(struct vty *vty)
{
	struct hash_bucket *pbucket = NULL;
	struct isolate_group *pgrp = NULL;
	void *cursor = NULL;

	pbucket = hios_hash_start(&islt_vlan_table, &cursor);
	if (NULL == pbucket)
	{
		return;
	}

	for (; pbucket; pbucket = hios_hash_next(&islt_vlan_table, &cursor))
	{
		pgrp = (struct isolate_group *)pbucket->data;
		if (NULL == pgrp)
		{
			continue;
		}

		port_isolation_vlan_show(vty, pgrp->id);
	}
}

DEFUN(show_port_isolate,
		show_port_isolate_cmd,
		"show port-isolation (group| vlan) [ID]",
		SHOW_STR
		"port isolation\n"
		"Port isolation group\n"
		"Port isolation vlan\n"
		"Port isolation group number or vlan id\n")
{
	enum ISLT_TYPE type = EP_TREE;
	uint32_t id = 0;

	if (strncmp(argv[0], "group", 5) == 0 )
	{
		type = EP_TREE;
	}
	else if (strncmp(argv[0], "vlan", 4) == 0 )
	{
		type = EVP_TREE;
  }

	if (NULL != argv[1])
	{
		VTY_GET_INTEGER_RANGE ("islt-grp", id, argv[1], 1, 4094);
	}

	if (EP_TREE == type)
	{	
		vty_out(vty," %-15s%-10s%-10s%-23s%-23s%s", "isolate-group", "type", "state", "root", "leaf", VTY_NEWLINE);

		id ? port_isolation_group_show(vty, id): port_isolation_group_all_show(vty);
	}
	else
	{
		vty_out(vty," %-8s%-10s%-10s%-23s%-32s%s", "vlan", "type", "state", "root", "leaf", VTY_NEWLINE);
		id ? port_isolation_vlan_show(vty, id): port_isolation_vlan_all_show(vty);
	}

	return CMD_SUCCESS;
}


static void port_isolation_if_show(struct vty *vty, uint32_t ifindex)
{
	struct hash_bucket *pbucket = NULL;
	struct isolate_port *pif = NULL;
	void *cursor = NULL;
	
	char root_name[25];
	
	pbucket = hios_hash_start(&islt_port_table, &cursor);
	if (NULL == pbucket)
	{
		return;
	}

	vty_out(vty," %-10s%-15s%-10s%-32s%s", "type", "isolate_group", "state", "root", VTY_NEWLINE);

	for (; pbucket; pbucket = hios_hash_next(&islt_port_table, &cursor))
	{
		pif = (struct isolate_port *)pbucket->data;
		if (NULL == pif)
		{
			continue;
		}
		
		if (ifindex == pif->key.ifindex)
		{
			ifm_get_name_by_ifindex(pif->root_port, root_name);

			vty_out(vty," %-10s%-15d%-10s%-32s%s", (EP_TREE == pif->key.type) ?"EP-TREE" :"EVP-TREE", 
						pif->key.id, (!pif->state)? "ACTIVE": "INACTIVE", root_name, VTY_NEWLINE);
		}
	}

}

DEFUN(show_port_isolate_interface,
		show_port_isolate_interface_cmd,
		"show port-isolation interface (ethernet| gigabitethernet| xgigabitethernet) USP",
		SHOW_STR
		"port isolation\n"
		"Port isolation interface\n"
		CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port of interface, format:0-7>/<0-31>/<1-255>\n")
{
	uint32_t ifindex = 0;

	if (strncmp(argv[0], "eth", 3) == 0)
	{
		ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[1]);
	}
	else if (strncmp(argv[0], "giga", 4) == 0)
	{
		ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
	}
	else if (strncmp(argv[0], "xgiga", 5) == 0)
	{
		ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[1]);
	}
	else
	{
		vty_error_out ( vty, "Please check out port format.%s", VTY_NEWLINE );
		
		return CMD_WARNING; 
	}

	port_isolation_if_show(vty, ifindex);
	
	return CMD_SUCCESS;
}

static void port_isolate_cmd_init(void)
{
    install_element(PHYSICAL_IF_NODE, &port_isolate_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_port_isolate_cmd, CMD_SYNC);

	install_element(CONFIG_NODE, &show_port_isolate_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_port_isolate_interface_cmd, CMD_LOCAL);
}


void port_isolate_init(void)
{
	islt_grp_table_init(ISLT_GRP_NUM);
	islt_vlan_table_init(ISLT_GRP_NUM);
	islt_port_table_init(ISLT_PORT_NUM);

	port_isolate_cmd_init();
}

void l2if_reply_isolate_info_bulk(struct ipc_msghdr_n *phdr, void *pdata)
{
	struct port_islt_snmp *psnmp = NULL;
	struct hash_bucket *pbucket = NULL;
	struct isolate_port *pislt = NULL;
	struct l2if *pif = NULL;

	uint32_t ifindex;
	int cursor;

	int msg_num = IPC_MSG_LEN/sizeof(struct port_islt_snmp);
	int data_num = 0;
	int flag = 0;

	struct  port_islt_snmp msgsnd[msg_num];

	ifindex = phdr->msg_index;
	psnmp = (struct port_islt_snmp *)pdata;

	memset(msgsnd, 0, msg_num * sizeof(struct port_islt_snmp));

	zlog_debug(0,"%s: ifindex:%d msg: ifindex(%d) id(%d) type(%d)\n\r", \
			__FUNCTION__, ifindex, psnmp->ifindex, psnmp->id, psnmp->type);

	if (0 == ifindex)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, islt_port_table)
		{
			pislt = (struct isolate_port *)pbucket->data;

			pif = l2if_lookup(pislt->key.ifindex);
			if (NULL == pif)
			{
				continue;
			}

			msgsnd[data_num].type    = pislt->key.type;
			msgsnd[data_num].ifindex = pislt->key.ifindex;
			msgsnd[data_num].id  	 = pislt->key.id;
			msgsnd[data_num].role    = (pislt->role == ROOT) ? 1: 2;
			msgsnd[data_num].state   = (pislt->state == ACTIVE) ? 2: 1;

			data_num ++;

			if (data_num == msg_num)
			{
				break;
			}
		}
	}
	else
	{
		HASH_BUCKET_LOOP(pbucket, cursor, islt_port_table)
		{
			pislt = (struct isolate_port *)pbucket->data;

			pif = l2if_lookup(pislt->key.ifindex);
			if (NULL == pif)
			{
				continue;
			}

			if (0 == flag)
			{
				if ((psnmp->ifindex == pislt->key.ifindex) &&
					(psnmp->type == pislt->key.type) &&
					(psnmp->id == pislt->key.id))
				{
					flag = 1;
					continue;
				}
			}
			else
			{
				
				msgsnd[data_num].type    = pislt->key.type;
				msgsnd[data_num].ifindex = pislt->key.ifindex;
				msgsnd[data_num].id  	 = pislt->key.id;
				msgsnd[data_num].role    = (pislt->role == ROOT) ? 1: 2;
				msgsnd[data_num].state   = (pislt->state == ACTIVE) ? 2: 1;

				data_num ++;

				if (data_num == msg_num)
				{
					flag = 0;
					break;
				}
			}
		}

		flag = 0;
	}

	zlog_debug(0,"%s: data_num(%d) flag(%d)\n", __FUNCTION__, data_num, flag);
	if (data_num)
	{
		#if 0
		ipc_send_reply_bulk(msgsnd, data_num*sizeof(struct port_islt_snmp), data_num, phdr->sender_id,
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
		#endif

		ipc_send_reply_n2(msgsnd, data_num*sizeof(struct port_islt_snmp), data_num,  phdr->sender_id,
	                 phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		#if 0
		ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, 
				phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
		#endif

		 ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, phdr->module_id, 
						phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}
}


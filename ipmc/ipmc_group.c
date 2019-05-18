/*
 * =====================================================================================
 *
 *       Filename:  ipmc_group.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2017 01:34:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <string.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/index.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include "ipmc_main.h"
#include "ipmc_group.h"

/* Hash of multicast group table */
struct hash_table mc_group_table;


static unsigned int mc_group_hash(void *hash_key)
{	
    return ((unsigned int) hash_key);
}

static int mc_group_compare(void *item, void *hash_key)
{  
	struct hash_bucket *pbucket = item;

	if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

	if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

/*init multicast group table */
void mc_group_table_init(unsigned int size)
{
	hios_hash_init(&mc_group_table, size, mc_group_hash, mc_group_compare);

	/*group id register*/
	index_register(INDEX_TYPE_IPMC, size);
}

/*create one multicast group struct*/
int mc_group_create(uint16_t group_id)
{
	struct hash_bucket *pitem = NULL;
	struct mcgroup_t *pgrp = NULL;
	int ret;

	pgrp = mc_group_lookup(group_id);
	if (NULL != pgrp)
	{
		return ERRNO_SUCCESS;
	}

	pgrp = (struct mcgroup_t *)XCALLOC(MTYPE_IPMC_GROUP, sizeof(struct mcgroup_t));
	if (NULL == pgrp)
	{
		return ERRNO_MALLOC;
	}
	
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
    	XFREE(MTYPE_IPMC_GROUP, pgrp);
		
        return ERRNO_MALLOC;
    }

	memset(pgrp, 0, sizeof(struct mcgroup_t));
	pgrp->group_id = group_id;
    pitem->hash_key = (void *)(uint32_t)pgrp->group_id;
	pitem->data = (void *)pgrp;

	/*send ipc to add mc group info */
#if 0  /*igmp不发送表项信息，由pim维护表项*/
	ret = ipc_send_hal((void *)(pgrp), sizeof(struct mcgroup_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
                         IPMC_SUBTYPE_MGROUP, IPC_OPCODE_ADD, pgrp->group_id);
	if(ret)
	{
		IPMC_LOG_ERROR("Failed to send create mc group to hal.\n");
		return ERRNO_IPC;
	}
#endif
    hios_hash_add(&mc_group_table, pitem);

    return ERRNO_SUCCESS;
}

/*delete one multicast group from multicast group table*/
int mc_group_delete(uint16_t  group_id) 
{
	struct hash_bucket *pbucket = NULL;
	struct mcgroup_t *pgrp = NULL;
	struct listnode *node = NULL;
	int ret;

    pbucket = hios_hash_find(&mc_group_table, (void *)(uint32_t)group_id);
    if (NULL == pbucket)
    {
        return ERRNO_NOT_FOUND;
    }

	pgrp = (struct mcgroup_t *)pbucket->data;
	if (NULL == pgrp)
	{
		hios_hash_delete(&mc_group_table, pbucket);
		return ERRNO_SUCCESS;
	}

	for (node = pgrp->iflist.head ; node; node = node->next)
	{
		list_delete_node(&(pgrp->iflist), node);
	}

	/*send ipc to add mc group info */
#if 0  /*igmp不发送表项信息，由pim维护表项*/
	ret = ipc_send_hal((void *)(pgrp), sizeof(struct mcgroup_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
                         IPMC_SUBTYPE_MGROUP, IPC_OPCODE_DELETE, pgrp->group_id);
	if(ret)
	{
		IPMC_LOG_ERROR("Failed to send delete mc group to hal.\n");
		return ERRNO_IPC;
	}
#endif
    hios_hash_delete(&mc_group_table, pbucket);
	
	index_free(INDEX_TYPE_IPMC, group_id);
	XFREE(MTYPE_IPMC_GROUP, pbucket->data);
    XFREE(MTYPE_HASH_BACKET, pbucket);

    return ERRNO_SUCCESS;
}


/*lookup a multicast group from multicast group table*/
struct mcgroup_t *mc_group_lookup(uint16_t group_id)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&mc_group_table, (void *)(uint32_t)group_id);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct mcgroup_t *)pitem->data;
}

int mc_group_lookup_if(struct mcgroup_t *pgroup, uint32_t ifindex)
{
	struct listnode *node = NULL;
    uint32_t *pifindex = NULL;
	
    for ( ALL_LIST_ELEMENTS_RO ( & ( pgroup->iflist ), node, pifindex ) )
    {
    	if(*pifindex == ifindex)
    	{
			return ERRNO_SUCCESS;
    	}
    }

	return ERRNO_NOT_FOUND;
}

/*add  an interface to multicast group*/
int mc_group_add_if(struct mcgroup_t *pgroup, uint32_t ifindex)
{
	struct mcgroup_t pgrp;
	uint32_t *pifindex = NULL;
	int ret;
	
	if ((NULL == pgroup) || (ifindex == 0))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	/*check ifindex whether valid or not*/
	switch (IFM_TYPE_ID_GET(ifindex))
    {
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:	
        case IFNET_TYPE_ETH_SUBPORT:
		case IFNET_TYPE_VLANIF:
        case IFNET_TYPE_TRUNK:
            break;
			
        default:
            return ERRNO_PARAM_ILLEGAL;
    }

	memset(&pgrp, 0, sizeof(struct mcgroup_t));
	pgrp.group_id = pgroup->group_id;
	//listnode_add (&(pgrp.iflist), (void *)ifindex);
	
	
	/*send ipc to add mc group info */
#if 0  /*igmp不发送表项信息，由pim维护表项*/
	ret = ipc_send_hal((void *)(&pgrp), sizeof(struct mcgroup_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
                         IPMC_SUBTYPE_MGROUP_IF, IPC_OPCODE_ADD, ifindex);
	if(ret) 
	{
		IPMC_LOG_ERROR("Failed to send add mc group to hal.\n");
		return ERRNO_IPC;
	}
#endif
	ret = mc_group_lookup_if(pgroup, ifindex);
	if(ret != ERRNO_SUCCESS)
	{
		pifindex = ( uint32_t * ) XCALLOC ( MTYPE_IPMC_GROUP_IF_ENTRY, sizeof ( uint32_t ) );
	    if ( pifindex == NULL )
	    {
	        zlog_err ( "%s[%d]:leave %s:error:xcalloc fail\n", __FILE__, __LINE__, __func__ );
	        return -1;
	    }
	    *pifindex = ifindex;
	    listnode_add (&(pgroup->iflist), (void *)pifindex);
	}
	
	return ERRNO_SUCCESS;
}


/*delete  an interface from multicast group*/
int mc_group_delete_if(struct mcgroup_t *pgroup, uint32_t ifindex)
{
	struct mcgroup_t pgrp;
	struct listnode *node = NULL;
    struct listnode *nnode = NULL;
    uint32_t *pifindex = NULL;
	int ret;

	if (NULL == pgroup)
	{
		return ERRNO_SUCCESS;
	}

	/*check ifindex whether valid or not*/
	switch (IFM_TYPE_ID_GET(ifindex))
    {
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
        case IFNET_TYPE_ETH_SUBPORT:
		case IFNET_TYPE_VLANIF:
        case IFNET_TYPE_TRUNK:
            break;
			
        default:
            return ERRNO_PARAM_ILLEGAL;
    }

	memset(&pgrp, 0, sizeof(struct mcgroup_t));
	pgrp.group_id = pgroup->group_id;
	//listnode_add (&(pgrp.iflist), (void *)ifindex);
	
	/*send ipc to add mc group info */
#if 0  /*igmp不发送表项信息，由pim维护表项*/
	ret = ipc_send_hal((void *)(&pgrp), sizeof(struct mcgroup_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
                         IPMC_SUBTYPE_MGROUP_IF, IPC_OPCODE_DELETE, pgrp.group_id);
	if(ret)
	{
		IPMC_LOG_ERROR("Failed to send add mc group to hal.\n");
		return ERRNO_IPC;
	}
#endif	
    for ( ALL_LIST_ELEMENTS ( & ( pgroup->iflist ), node, nnode, pifindex ) )
    {
    	if(*pifindex == ifindex)
    	{
			listnode_delete (&(pgroup->iflist), (void *)pifindex);
			XFREE ( MTYPE_IPMC_GROUP_IF_ENTRY, pifindex );
    	}
    }
	return ERRNO_SUCCESS;
}



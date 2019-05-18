/*
 * =====================================================================================
 *
 *       Filename:  ipmc_ip.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2017 01:32:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <lib/memory.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>

#include "ipmc_main.h"
#include "ipmc_ip.h"

/* Hash of multicast ip table */
struct hash_table mc_ip_table;


static unsigned int mc_ip_hash(void *hash_key)
{	
	struct mcip_t *mcip = NULL;
	uint32_t key = 0;

	mcip = (struct mcip_t *)hash_key;

	assert (NULL != hash_key);

	key = mcip->dip + mcip->sip + mcip->vpnid;
	
    return key;
}

static int mc_ip_compare(void *item, void *hash_key)
{  
	struct hash_bucket *pbucket = item;
	struct mcip_t *mcip1 = NULL;
	struct mcip_t *mcip2 = NULL;

	mcip1 = (struct mcip_t *)pbucket->data;
	mcip2 = (struct mcip_t *)hash_key;

	if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

	if ((mcip1->sip == mcip2->sip) &&
		(mcip1->dip == mcip2->dip) &&
		(mcip1->vpnid == mcip2->vpnid))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/*init multicast ip table */
void mc_ip_table_init(unsigned int size)
{
	hios_hash_init(&mc_ip_table, size, mc_ip_hash, mc_ip_compare);
}

/*add one multicast ip to multicast ip table*/
int mc_ip_add(struct mcip_t *pip)
{
	struct hash_bucket *pitem = NULL;
	struct mcip_t *mcip_tmp = NULL;
	struct mcip_t *pip_new = NULL;
	int ret;

	if (NULL == pip)
    {
        return ERRNO_FAIL;
    }

	mcip_tmp = mc_ip_lookup(pip);
	if (NULL != mcip_tmp)
	{
		return ERRNO_EXISTED;
	}

	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }

	pip_new = (struct mcip_t *)XCALLOC(MTYPE_IPMC_IP_ENTRY, sizeof(struct mcip_t));
	if (NULL == pip_new)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
        return ERRNO_MALLOC;
	}

	memcpy(pip_new, pip, sizeof(struct mcip_t));
    pitem->hash_key = (void *)pip_new;
    pitem->data = (void *)pip_new;

    hios_hash_add(&mc_ip_table, pitem);

	/*send ipc to add mc ip info */
#if 0  /*igmp不发送表项信息，由pim维护表项*/
	ret = ipc_send_hal(pitem->data, sizeof(struct mcip_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
                         IPMC_SUBTYPE_MFIB, IPC_OPCODE_ADD, pip_new->group_id);
	if(ret)
	{
		IPMC_LOG_ERROR("Failed to send add mc ip to hal.\n");
		return ERRNO_IPC;
	}
#endif
    return ERRNO_SUCCESS;
}

/*delete one multicast ip from multicast ip table*/
int mc_ip_delete(struct mcip_t *pip) 
{
	struct hash_bucket *pbucket = NULL;
	int ret;

    pbucket = hios_hash_find(&mc_ip_table, (void *)pip);
    if (NULL == pbucket)
    {
        return ERRNO_NOT_FOUND;
    }

	/*send ipc to add mc ip info */
#if 0  /*igmp不发送表项信息，由pim维护表项*/
	ret = ipc_send_hal((void *)pip, sizeof(struct mcip_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
                         IPMC_SUBTYPE_MFIB, IPC_OPCODE_DELETE, pip->group_id);
	if(ret)
	{
		IPMC_LOG_ERROR("Failed to send delete mc ip to hal.\n");
		return ERRNO_IPC;
	}
#endif
    hios_hash_delete(&mc_ip_table, pbucket);
	
    XFREE(MTYPE_IPMC_IP_ENTRY, pbucket->data);
    XFREE(MTYPE_HASH_BACKET, pbucket);

    return ERRNO_SUCCESS;
}

/*lookup a multicast ip from multicast ip table*/
struct mcip_t *mc_ip_lookup(struct mcip_t *pip)
{
	struct hash_bucket *pitem = NULL;

	if (NULL == pip)
    {
        return NULL;
    }

	pitem = hios_hash_find(&mc_ip_table, pip);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct mcip_t *)pitem->data;
}


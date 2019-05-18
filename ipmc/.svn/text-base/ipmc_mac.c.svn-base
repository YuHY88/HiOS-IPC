/*
 * =====================================================================================
 *
 *       Filename:  ipmc_mac.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2017 01:33:42 PM
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

#include "ipmc_mac.h"

/* Hash of multicast mac table */
struct hash_table mc_mac_table;


static unsigned int mc_mac_hash(void *hash_key)
{
	struct mcmac_t *mcmac = NULL;
	uint32_t key = 0;
    int i;

	mcmac = (struct mcmac_t *)hash_key;

	assert (NULL != hash_key);

	for (i = 0; i < MAC_LEN; i++)
	{
		key += mcmac->smac[i];
	}

	key += mcmac->group_id;
	
    return key;
}

static int mc_mac_compare(void *item, void *hash_key)
{  
	struct hash_bucket *pbucket = item;
	struct mcmac_t *mcmac1 = NULL;
	struct mcmac_t *mcmac2 = NULL;

	mcmac1 = (struct mcmac_t *)pbucket->data;
	mcmac2 = (struct mcmac_t *)hash_key;

	if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

	if ((memcmp(mcmac1->dmac, mcmac2->dmac, MAC_LEN) == 0) &&
		(memcmp(mcmac1->smac, mcmac2->smac, MAC_LEN) == 0) &&
        (mcmac1->group_id == mcmac2->group_id))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/*init multicast mac table */
void mc_mac_table_init(unsigned int size)
{
	hios_hash_init(&mc_mac_table, size, mc_mac_hash, mc_mac_compare);
}

/*add one multicast mac to multicast mac table*/
int mc_mac_add(struct mcmac_t *pmac)
{
	struct hash_bucket *pitem = NULL;
	struct mcmac_t *pmac_tmp;

	if (NULL == pmac)
    {
        return ERRNO_FAIL;
    }

	pmac_tmp = mc_mac_lookup(pmac);
	if (NULL != pmac_tmp)
	{
		return ERRNO_EXISTED;
	}
	
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pmac;
    pitem->data = (void *)pmac;

    hios_hash_add(&mc_mac_table, pitem);

    return ERRNO_SUCCESS;
}

/*delete one multicast mac from multicast mac table*/
int mc_mac_delete(struct mcmac_t *pmac) 
{
	struct hash_bucket *pbucket = NULL;

    pbucket = mc_mac_lookup(pmac);
    if (NULL == pbucket)
    {
        return ERRNO_NOT_FOUND;
    }

    hios_hash_delete(&mc_mac_table, pbucket);

    XFREE(MTYPE_HASH_BACKET, pbucket);

    return ERRNO_SUCCESS;
}

/*lookup a multicast mac from multicast mac table*/
struct mcmac_t *mc_mac_lookup(struct mcmac_t *pmac)
{
	struct hash_bucket *pitem = NULL;

	if (NULL == pmac)
    {
        return NULL;
    }

	pitem = hios_hash_find(&mc_mac_table, (void *)pmac);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct mcmac_t *)pitem->data;
}


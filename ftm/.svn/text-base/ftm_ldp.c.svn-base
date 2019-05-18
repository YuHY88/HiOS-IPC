#include <lib/types.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/msg_ipc_n.h>
#include <lib/mpls_common.h>
#include <lib/hash1.h>
#include <lib/log.h>

#include "ftm_ldp.h"
#include "ftm_debug.h"
#include "ftm.h"
#include "ftm_nhlfe.h"

struct hash_table ldp_frr_table;

static unsigned int ftm_ldp_frr_index_hash(void *hash_key)
{
    return (unsigned int)hash_key;
}

static int ftm_ldp_frr_index_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if (pitem == NULL)
    {
        return ERRNO_FAIL;
    }

    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

void ftm_ldp_frr_table_init(unsigned int size)
{
    hios_hash_init(&ldp_frr_table, size, ftm_ldp_frr_index_hash, ftm_ldp_frr_index_compare);
}

int ftm_ldp_frr_send_hal(struct ldp_frr_t *pldp_frr, int opcode)
{
    int ret = ERRNO_SUCCESS;
    struct nhlfe_frr nhlfe_frr;

    nhlfe_frr.index = pldp_frr->group_index;
    nhlfe_frr.master_lsp_index = pldp_frr->master_lsp;
    nhlfe_frr.backup_lsp_index = pldp_frr->backup_lsp;
    
    ret = ftm_msg_send_to_hal(&nhlfe_frr, sizeof(struct nhlfe_frr), 1, IPC_TYPE_LDP, 0, opcode, 0);
    if (ERRNO_SUCCESS != ret)
    {
        FTM_LOG_ERROR("send to hal\n");
        return ERRNO_IPC;
    }
    return ERRNO_SUCCESS;
}

int ftm_ldp_frr_add(struct ldp_frr_t *pldp_frr)
{
    struct hash_bucket *pitem = NULL;
    struct ldp_frr_t *pfrr = NULL;

    if (NULL == pldp_frr)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ldp_frr_table.hash_size == ldp_frr_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }
    
    pfrr = (struct ldp_frr_t *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct ldp_frr_t));
    if(NULL == pfrr)
    {
        return ERRNO_MALLOC;
    }
    memcpy(pfrr, pldp_frr, sizeof(struct ldp_frr_t));
    
    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        XFREE(MTYPE_LDP_ENTRY, pfrr);
        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pfrr->group_index;
    pitem->data     = pfrr;

    hios_hash_add(&ldp_frr_table, pitem);

    return ERRNO_SUCCESS;

}


int ftm_ldp_frr_del(struct ldp_frr_t *pldp_frr)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ldp_frr_table, (void *)pldp_frr->group_index);
    if (NULL == pitem)
    {
        return ERRNO_NOT_FOUND;
    }

    hios_hash_delete(&ldp_frr_table, pitem);

    XFREE(MTYPE_LDP_ENTRY, pitem->data);
    
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;

}


struct ldp_frr_t * ftm_ldp_frr_lookup(uint32_t frr_index)
{
    struct hash_bucket *pitem = NULL;

    pitem = hios_hash_find(&ldp_frr_table, (void *)frr_index);
    if (NULL == pitem)
    {
        return NULL;
    }
    return pitem->data;
}

int ftm_ldp_frr_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                        enum IPC_OPCODE opcode)
{
    int ret = ERRNO_SUCCESS;
    struct ldp_frr_t *pldp_frr = pdata;

    if(IPC_OPCODE_ADD == opcode)
    {
        ret = ftm_ldp_frr_add(pldp_frr);
    }
    else if(IPC_OPCODE_DELETE == opcode)
    {
        ret = ftm_ldp_frr_del(pldp_frr);
    }
    else
    {
        ret = ERRNO_FAIL;
    }

    if(ERRNO_SUCCESS == ret)
    {
        ftm_ldp_frr_send_hal(pldp_frr, opcode);
    }
    return ret;
}



#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc_n.h>
#include <lib/alarm.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/sdh_vcg_common.h>
#include "lib/snmp_common.h"

#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>

#include "sdhvcg_main.h"
#include "sdhvcg_msg.h"

#include "sdhvcg_if.h"

#define  SDH_VCG_TOTAL_NUM    192

extern void vcg_if_list_free(t_sdh_vcg_interface *pvcgif);
extern int vcg_if_list_cmp(t_sdh_vcg_interface *val1, t_sdh_vcg_interface *val2);


void vcg_if_list_free(t_sdh_vcg_interface *pvcgif)
{
    if (NULL == pvcgif)
    {
        return;
    }

    XFREE(MTYPE_VCG_ENTRY, pvcgif);
}

int vcg_if_list_cmp(t_sdh_vcg_interface *val1, t_sdh_vcg_interface *val2)
{
    if (NULL == val1)
    {
        if (NULL == val2)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (NULL == val2)
        {
            return 1;
        }
        else
        {
            if (val1->ifindex > val2->ifindex)
            {
                return 1;
            }
            else if (val1->ifindex == val2->ifindex)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
    }
}


/* initialize vcg interface table*/
void vcg_if_table_init(int size)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    g_vcg_if_list = list_new();

    g_vcg_if_list->del = (void (*)(void *))vcg_if_list_free;
    g_vcg_if_list->cmp = (int (*)(void *, void *))vcg_if_list_cmp;
}

/* add vcg interface */
t_sdh_vcg_interface *vcg_if_add(void *pdata, uint32_t ifindex)
{
    t_sdh_vcg_interface *pif_new = NULL;

    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL, "vcg_if_add %08x\n", ifindex);

    pif_new = (t_sdh_vcg_interface *)XMALLOC(MTYPE_VCG_ENTRY, sizeof(t_sdh_vcg_interface));

    if (NULL == pif_new)
    {
        return NULL;
    }

    pif_new->ifindex = ifindex;
    memcpy(&pif_new->config, pdata, sizeof(t_sdh_vcgif_config));

    listnode_add_sort(g_vcg_if_list, pif_new);

    if (1 == g_vcg_config_finish)  // after vtysh config end, card plugin
    {
        /*send message to vtysh reconfig*/
        ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_VTY, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_VTYSH_RECONFIG, IPC_OPCODE_INVALID, ifindex);
    }

    return pif_new;
}

t_sdh_vcg_interface *vcg_if_create(void *pdata, uint32_t ifindex)
{
    t_sdh_vcg_interface *pif = NULL;

    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL, "vcg_if_create %08x\n", ifindex);

    pif = vcg_if_lookup(ifindex);

    if (pif != NULL)
    {
        return pif;
    }

    pif = vcg_if_add(pdata, ifindex);

    if (pif == NULL)
    {
        zlog_err("vcg_if_add failure %08x\n", ifindex);
        return NULL;
    }

    return pif;
}

/* delete */
int vcg_if_delete(uint32_t ifindex)
{
    t_sdh_vcg_interface *pvcgif = NULL;

    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"vcg_if_delete %08x\n", ifindex);

    pvcgif = vcg_if_lookup(ifindex);

    if (pvcgif == NULL)
    {
        return -1;
    }

    listnode_delete(g_vcg_if_list, pvcgif);
    XFREE(MTYPE_VCG_ENTRY, pvcgif);
    pvcgif = NULL;

    return 0;
}

int vcg_if_status_update(void *pdata, int data_len, int data_num)
{
    int     i;
    t_sdh_vcgif_status_msg *pvcgif_status;
    t_sdh_vcg_interface    *pvcgif = NULL;

    pvcgif_status = (t_sdh_vcgif_status_msg *)pdata;

    for (i = 0; i < data_num; ++i)
    {
        pvcgif = vcg_if_lookup(pvcgif_status->ifindex);

        if (NULL != pvcgif)
        {
            pvcgif->link_status = pvcgif_status->link_status;
        }

        pvcgif_status++;
    }

    return 0;
}

t_sdh_vcg_interface *vcg_if_lookup(uint32_t ifindex)
{
    t_sdh_vcg_interface *pvcgif = NULL;
    struct listnode *pnode = NULL;

    for (ALL_LIST_ELEMENTS_RO(g_vcg_if_list, pnode, pvcgif))
    {
        if (NULL != pvcgif)
        {
            if (pvcgif->ifindex == ifindex)
            {
                return pvcgif;
            }
        }
    }

    return NULL;
}

int vcg_set_binding(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;

    ret = vcg_msg_send_hal_wait_ack(pvcgif, sizeof(t_sdh_vcg_interface), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_BINDING, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_jx_transmit(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;

    ret = vcg_msg_send_hal_wait_ack(pvcgif, sizeof(t_sdh_vcg_interface), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_JX_TRANSMIT, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_jx_expect(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;

    ret = vcg_msg_send_hal_wait_ack(pvcgif, sizeof(t_sdh_vcg_interface), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_JX_EXPECT, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_scrambling(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;

    ret = vcg_msg_send_hal_wait_ack(&(pvcgif->config.scrambling), sizeof(E_SDH_VCG_SCRAMBLING), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_SCRAMBLING, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_fcs(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;

    ret = vcg_msg_send_hal_wait_ack(&(pvcgif->config.fcs_insert), sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_FCS, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_lcas_ena(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;

    ret = vcg_msg_send_hal_wait_ack(&(pvcgif->config.lcas_enable), sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_LCAS_ENA, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_lcas_reset(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;
    int reset = 1;

    ret = vcg_msg_send_hal_wait_ack(&reset, sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_LCAS_RESET, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_vlan(t_sdh_vcg_interface *pvcgif)
{
    int ret = 0;
    ret = vcg_msg_send_hal_wait_ack(pvcgif, sizeof(t_sdh_vcg_interface), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_VLAN, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

int vcg_set_image(t_sdh_vcg_interface *pvcgif)
{   
    int ret = 0;
    ret = vcg_msg_send_hal_wait_ack(pvcgif, sizeof(t_sdh_vcg_interface), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_INFO_SWITCH_IMAGE, IPC_OPCODE_UPDATE, pvcgif->ifindex);

    return ret;
}

t_sdh_vcg_interface *vcg_get_fpga_image(t_sdh_vcg_interface *pvcgif)
{	  
     t_sdh_vcg_interface  pvcg;
     struct ipc_mesg_n * pmsg = ipc_sync_send_n2(pvcgif, sizeof(t_sdh_vcg_interface), 1, MODULE_ID_HAL, MODULE_ID_VCG, IPC_TYPE_VCG, VCG_GET_FPGA_IMAGE, IPC_OPCODE_GET, pvcgif->ifindex, 5000);

	 memset(&pvcg, 0, sizeof(t_sdh_vcg_interface));
	 if(pmsg)
     {
        memcpy(&pvcg, pmsg->msg_data, sizeof(t_sdh_vcg_interface));
        mem_share_free(pmsg, MODULE_ID_VCG);

        return &pvcg;
    }
    else
    {
        return NULL;
    }
}

int vcg_if_info_set(t_sdh_vcg_interface *pvcgif, E_SDH_VCG_IPC_MSG_SUBTYPE type)
{
    int ret = 0;

    if (pvcgif == NULL)
    {
        return 1;
    }

    switch (type)
    {
        case VCG_INFO_BINDING:
            ret = vcg_set_binding(pvcgif);
            return ret;

        case VCG_INFO_JX_TRANSMIT:
            ret = vcg_set_jx_transmit(pvcgif);
            return ret;

        case VCG_INFO_JX_EXPECT:
            ret = vcg_set_jx_expect(pvcgif);
            return ret;

        case VCG_INFO_SCRAMBLING:
            ret = vcg_set_scrambling(pvcgif);
            return ret;

        case VCG_INFO_FCS:
            ret = vcg_set_fcs(pvcgif);
            return ret;

        case VCG_INFO_LCAS_ENA:
            ret = vcg_set_lcas_ena(pvcgif);
            return ret;

        case VCG_INFO_LCAS_RESET:
            ret = vcg_set_lcas_reset(pvcgif);
            return ret;

        case VCG_INFO_VLAN:
            ret = vcg_set_vlan(pvcgif);
            return ret;
        
		case VCG_INFO_SWITCH_IMAGE:
		    ret = vcg_set_image(pvcgif);
		    return ret;
		
        default:
            break;
    }

    return 0;
}

void vcg_get_config_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
    int                                  num, flag; 
    uint32_t                             index = 0;
    t_sdh_vcg_interface_getbulk_req_msg *pmsg;
    t_sdh_vcg_interface                 *pvcgif = NULL;
	t_sdh_vcg_interface                 *pvcg_new = NULL;
    struct listnode                     *pnode = NULL;
    int                                  data_num = IPC_MSG_LEN / sizeof(t_sdh_vcg_interface);
    t_sdh_vcg_interface                  data[data_num];

    pmsg = (t_sdh_vcg_interface_getbulk_req_msg *)pdata;

    if (NULL == pmsg || 0 == pmsghdr->data_len)
    {
        num = 0;
    }
    else
    {
        num  = 0;
        flag = 0;
        index = pmsg->msg.ifindex;

        for (ALL_LIST_ELEMENTS_RO(g_vcg_if_list, pnode, pvcgif))
        {
            if (NULL == pvcgif || num >= data_num)
            {
                break;
            }

            if (0 == pmsg->req_flag)        // get from index, include index
            {
                if (0 == flag)
                {
                    if (pvcgif->ifindex > index)
                    {
                        flag = 1;
                    }
                    else if (pvcgif->ifindex == index)
                    {
                        flag = 1;
                    }
                }
            }
            else if (2 == pmsg->req_flag)   // get from index, include index
            {
                if (pvcgif->ifindex == index)
                {
                    flag = 1;
                }
            }
            else
            {
                flag = 1;
            }
            pvcg_new = vcg_get_fpga_image(pvcgif);
			if(pvcg_new != NULL)
			{
               pvcgif->image_level = pvcg_new->image_level;
			}
				
            if (1 == flag)
            {
                memcpy(&(data[num]), pvcgif, sizeof(t_sdh_vcg_interface));

                num++;
            }
        }
    }

    if (0 == num)
    {
         ipc_noack_to_snmp(pmsghdr);
    }
    else
    {
         ipc_ack_to_snmp(pmsghdr, data, sizeof(t_sdh_vcg_interface) * num, num);
    }

    return;
}


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <l2/l2_if.h>
#include <l2/l2_snmp.h>
#include <l2/loopd/loop_detect.h>
#include <lib/linklist.h>
#include <lib/msg_ipc_n.h>
#include <ifm/ifm_message.h>
#include <lib/log.h>

#include "ipran_snmp_data_cache.h"


#include "mib_loopdetect.h"
#include "snmp_config_table.h"


static struct ipran_snmp_data_cache *loopdetect_cache = NULL ;

#define SYS_STRING_LEN  256
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;

FindVarMethod hhrLoopDetectTable_get ;

struct variable2 hhrLoopDetectTable_variables[] =
{
    {hhrLoopDetectIfDesrc,      ASN_OCTET_STR,  RONLY, hhrLoopDetectTable_get, 2, {1, 2}},
    {hhrLoopDetectEnable,       ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 3}},
    {hhrLoopDetectVlan,         ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 4}},
    {hhrLoopDetectInterval,     ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 5}},
    {hhrLoopDetectRestoreTime,  ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 6}},
    {hhrLoopDetectAction,       ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 7}},
    {hhrLoopDetectStatus,       ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 8}},
    {hhrLoopDetectPackets,      ASN_INTEGER,    RONLY, hhrLoopDetectTable_get, 2, {1, 9}}
} ;

oid  hhrLoopDetectTable_oid[]   = { LOOPDECT_HHRLOOPDECT , 8};
int  hhrLoopDetectTable_oid_len = sizeof(hhrLoopDetectTable_oid) / sizeof(oid);

void  init_mib_loopdetect(void)
{
    REGISTER_MIB("hhrLoopDetectTable", hhrLoopDetectTable_variables, variable2,
                 hhrLoopDetectTable_oid) ;
}

struct ipc_mesg_n * snmp_get_loopdetect_info_bulk(uint32_t ifindex, int module_id , int *pdata_num)
{

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_L2,module_id,
                                                         IPC_TYPE_SNMP,IPC_TYPE_SNMP_LOOPDETECT_INFO, IPC_OPCODE_GET, ifindex,5000);
    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

int snmp_loopdetect_iftable_isvalid(uint32_t ifindex)
{
    if (IFM_TYPE_IS_ETHERNET(ifindex) && (!IFM_IS_SUBPORT(ifindex)))
    {
        return 0;
    }

    return -1;
}

int ifm_if_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                  struct loop_detect_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct loop_detect_snmp *ploopd_info = NULL;
    int data_num = 0;
    int ret = 0;
	
    pmesg  = snmp_get_loopdetect_info_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

   	if(pmesg)
    {
    	ploopd_info = (struct loop_detect_snmp *)(pmesg->msg_data);
			
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, ploopd_info , sizeof(struct loop_detect_snmp));
            ploopd_info++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }

	return FALSE;
}

struct loop_detect_snmp *ifm_if_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct loop_detect_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct loop_detect_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
        {
            if (1 == exact) //get
            {
                return data1_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return node->next->data ;
                }
            }
        }
    }

    return (NULL);
}


u_char *
hhrLoopDetectTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct loop_detect_snmp index ;
    u_int32_t index_next = 0;
    int ret = 0;
    int temp = 0 ;
    struct loop_detect_snmp *pifm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == loopdetect_cache)
    {
        loopdetect_cache = snmp_cache_init(sizeof(struct loop_detect_snmp) ,
                                           ifm_if_info_get_data_from_ipc ,
                                           ifm_if_info_node_lookup);

        if (NULL == loopdetect_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    pifm_info = snmp_cache_get_data_by_index(loopdetect_cache , exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->ifindex);
    }


    switch (vp->magic)
    {
        case hhrLoopDetectIfDesrc :
            memset(str_value ,  0 , SYS_STRING_LEN);
            ifm_get_name_by_ifindex(pifm_info->ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrLoopDetectEnable :
            int_value = 1;// 1:enable, 2: disable
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrLoopDetectVlan :
            int_value = pifm_info->loopdetect.vlan;
            *var_len = sizeof(int);

            if (int_value >= 4095)
            {
                int_value =  0 ;
            }

            return (u_char *)&int_value;

        case hhrLoopDetectInterval :
            int_value = pifm_info->loopdetect.interval;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrLoopDetectRestoreTime :
            int_value = pifm_info->loopdetect.restore_time;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrLoopDetectAction :
            int_value = 0;
            temp = pifm_info->loopdetect.act;

            if (temp == PORT_ACTION_BLOCK)
            {
                int_value = 1;
            }
            else if (temp == PORT_ACTION_SHUTDOWN)
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrLoopDetectStatus :
            int_value = 0;
            temp = pifm_info->loopdetect.status;

            if (temp == PORT_STATUS_FORWARD)
            {
                int_value = 1;
            }
            else if (temp == PORT_STATUS_BLOCK)
            {
                int_value = 2;
            }
            else if (temp == PORT_STATUS_SHUTDOWN)
            {
                int_value = 3;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrLoopDetectPackets :
            int_value = pifm_info->loopdetect.pkt_recv;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }
}

/*
 *  cli command MIB group implementation - ifm_snmp.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <lib/types.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/log.h>

#include "../../mpls/bfd/bfd_session.h"
#include "../../mpls/mpls_oam/mpls_oam.h"
#include <lib/linklist.h>
#include "ipran_snmp_data_cache.h"
//#include "../../mpls/lspm.h"
//#include "../../mpls/pw.h"

#include "mpls/lspm.h"
#include "mpls/pw.h"
#include "snmp_config_table.h"

#include "mib_oam.h"

enum table_lock
{
    UNLOCK,
    LOCK,
};

#define                   OAM_STRING_LEN         256
#define                   OAM_MAX_NAME_LEGTH      NAME_STRING_LEN

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;
static uint32_t           ip_value = 0;
static uint32_t           uint_value = 0;
static uchar              mac_value[6] = {0};

/* table list define */
struct list *hhrMplsTpOamConfigTable_list = NULL;
struct list *hhrbfdDetectConfigTable_list = NULL;

/* variable to store refresh data time of the last time */
static time_t hhrMplsTpOamConfigTable_time_old = 0;
static time_t hhrbfdDetectConfigTable_time_old = 0;

static enum table_lock hhrMplsTpOamConfigTable_lock = UNLOCK;
static enum table_lock hhrbfdDetectConfigTable_lock = UNLOCK;

/*
 * Object ID definitions
 */

/*hhrMplsTpOamConfigTable*/
static oid hhrMplsTpOamConfigTable_oid[] = {HHR_OAM_ROID, 10};
FindVarMethod hhrMplsTpOamConfigTable_get;
struct variable2 hhrMplsTpOamConfigTable_variables[] =
{
    {hhrTpOamPriority,           ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  2}},
    {hhrTpOamPeerMep,            ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  3}},
    {hhrTpOamCCEnable,           ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  4}},
    {hhrTpOamCCInterval,         ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  5}},
    {hhrTpOamAISEnable,          ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  6}},
    {hhrTpOamPathType,           ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  7}},
    {hhrTpOamPathName,           ASN_OCTET_STR, RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  8}},
    {hhrTpOamSectionDestMAC,     ASN_OCTET_STR, RONLY, hhrMplsTpOamConfigTable_get, 2, {1,  9}},
    {hhrTpOamSectionVlan,        ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 10}},
    {hhrTpOamSectionChannelType, ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 11}},
    {hhrTpOamSectionAlarmEnable, ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 12}},
    {hhrTpOamNodeType,           ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 13}},
    {hhrTpOamMpid,               ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 14}},
    {hhrTpOamPeerMepForward,     ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 15}},
    {hhrTpOamPeerMepReverse,     ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 16}},
    {hhrTpOamAISLevel,           ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 17}},
    {hhrTpOamAISInterval,        ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 18}},
    {hhrTpOamBindMeg,            ASN_OCTET_STR, RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 19}},
    {hhrTpOamPathNameRevLsp,     ASN_OCTET_STR, RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 20}},
    {hhrTpOamState,              ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 21}},
    {hhrTpOamLCKEnable,          ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 22}},
    {hhrTpOamLCKLevel,           ASN_UNSIGNED,  RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 23}},
    {hhrTpOamLCKInterval,        ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 24}},
    {hhrTpOamCSFEnable,          ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 25}},
    {hhrTpOamCSFLevel,           ASN_UNSIGNED,  RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 26}},
    {hhrTpOamCSFInterval,        ASN_INTEGER,   RONLY, hhrMplsTpOamConfigTable_get, 2, {1, 27}}
};

static struct ipran_snmp_data_cache *hhrMplsTpOamMegTable_cache = NULL ;
static oid hhrMplsTpOamMegTable_oid[] = {HHR_OAM_ROID, 11};
FindVarMethod hhrMplsTpOamMegTable_get;
struct variable2 hhrMplsTpOamMegTable_variables[] =
{
    {hhrTpOamMegLevel,           ASN_INTEGER,   RONLY, hhrMplsTpOamMegTable_get, 2, {1,  2}}
};

static oid hhrbfdDetectConfigTable_oid[] = {HHR_BFD_ROID, 10};
FindVarMethod hhrbfdDetectConfigTable_get;
struct variable2 hhrbfdDetectConfigTable_variables[] =
{
    {hhrbfdRemoteDiscriminator,  ASN_UNSIGNED,  RONLY, hhrbfdDetectConfigTable_get, 2, {1, 2}},
    {hhrbfdccInterval,           ASN_INTEGER,   RONLY, hhrbfdDetectConfigTable_get, 2, {1, 3}},
    {hhrbfdDetectMultiplier,     ASN_INTEGER,   RONLY, hhrbfdDetectConfigTable_get, 2, {1, 4}},
    {hhrbfdDetectType,           ASN_INTEGER,   RONLY, hhrbfdDetectConfigTable_get, 2, {1, 5}},
    {hhrbfdDetectObjectDescr,    ASN_OCTET_STR, RONLY, hhrbfdDetectConfigTable_get, 2, {1, 6}},
    {hhrbfdSourceIp,             ASN_IPADDRESS, RONLY, hhrbfdDetectConfigTable_get, 2, {1, 7}},
    {hhrbfdSessionState,         ASN_INTEGER,   RONLY, hhrbfdDetectConfigTable_get, 2, {1, 8}},
    {hhrbfdLocalDiscriminator,   ASN_UNSIGNED,  RONLY, hhrbfdDetectConfigTable_get, 2, {1, 9}},
    {hhrbfdWithIPOrUDPHeaderEnable, ASN_INTEGER, RONLY, hhrbfdDetectConfigTable_get, 2, {1, 10}}
};

static oid hhrBFDGlobal_oid[] = {HHR_BFD_ROID};
FindVarMethod hhrBFDGlobal_get;
struct variable1 hhrBFDGlobal_variables[] =
{
    {hhrbfdEnable,                 ASN_INTEGER,  RONLY, hhrBFDGlobal_get, 1, {1}},
    {hhrbfdGlobalPriority,         ASN_UNSIGNED, RONLY, hhrBFDGlobal_get, 1, {2}},
    {hhrbfdGlobalMinReInterval,    ASN_UNSIGNED, RONLY, hhrBFDGlobal_get, 1, {3}},
    {hhrbfdGlobalDetectMultipier,  ASN_UNSIGNED, RONLY, hhrBFDGlobal_get, 1, {4}},
    {hhrbfdGlobalWTR,              ASN_UNSIGNED, RONLY, hhrBFDGlobal_get, 1, {5}}
};

extern void oam_list_init(void);

void init_mib_oam(void)
{
    REGISTER_MIB("hhrMplsTpOamConfigTable", hhrMplsTpOamConfigTable_variables,
                 variable2, hhrMplsTpOamConfigTable_oid);

    REGISTER_MIB("hhrBFDGlobal", hhrBFDGlobal_variables,
                 variable1, hhrBFDGlobal_oid);

    REGISTER_MIB("hhrbfdDetectConfigTable", hhrbfdDetectConfigTable_variables,
                 variable2, hhrbfdDetectConfigTable_oid);

    REGISTER_MIB("hhrMplsTpOamMegTable", hhrMplsTpOamMegTable_variables,
                 variable2, hhrMplsTpOamMegTable_oid);

    oam_list_init();
}

/* \u83b7\u53d6mplstp-oam session*/
struct ipc_mesg_n *mpls_com_get_tpoam_instance(int session_id, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = NULL ;


    pmesg = ipc_sync_send_n2(&session_id, 4, 1, MODULE_ID_MPLS,
                                        module_id, IPC_TYPE_MPLSOAM, 0, IPC_OPCODE_GET_BULK, session_id, 1);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

int mpls_get_tpoam_session_ifname(int module_id, int session_id, char *ifname)
{
    struct ipc_mesg_n *pmesg = NULL;

    pmesg = ipc_sync_send_n2(&session_id, 4, 1, MODULE_ID_MPLS,
                                        module_id, IPC_TYPE_MPLSOAM, OAM_SUBTYPE_IFNAME, IPC_OPCODE_GET, 0, 1);

    if (NULL != pmesg)
    {
        snprintf(ifname, OAM_STRING_LEN, "%s", (char *)pmesg->msg_data);
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return 1;
    }

    return 0;
}

struct ipc_mesg_n *mpls_com_get_bfd_instance(int session_id, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = NULL ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    pmesg = ipc_sync_send_n2(&session_id, 4, 1, MODULE_ID_MPLS,
                                        module_id, IPC_TYPE_BFD, 0 , IPC_OPCODE_GET_BULK , session_id, 1);

    if (NULL != pmesg)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

struct bfd_global *mpls_com_get_bfd_global(int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = NULL ;
	struct bfd_global pbfd;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS,
                                        module_id, IPC_TYPE_BFD, BFD_SUBTYPE_GLOBAL, IPC_OPCODE_GET , 0, 1);

    if (NULL != pmesg)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);
        *pdata_num = pmesg->msghdr.data_num;

		if(NULL == pmesg->msg_data)
		{
			mem_share_free(pmesg, MODULE_ID_SNMPD);
			return NULL;
		}
		memset(&pbfd, 0, sizeof(struct bfd_global));
		memcpy(&pbfd, pmesg->msg_data, sizeof(struct bfd_global));
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		
        return (&pbfd);
    }

    return NULL;
}

int mpls_get_bfd_session_ifname(int module_id, int session_id, char *ifname)
{
    struct ipc_mesg_n *pmesg = NULL;

    pmesg = ipc_sync_send_n2(&session_id, 4, 1, MODULE_ID_MPLS,
                                        module_id, IPC_TYPE_BFD, BFD_SUBTYPE_IFNAME, IPC_OPCODE_GET, 0, 1);

    if (NULL != pmesg)
    {
		if(NULL == pmesg->msg_data)
		{
			mem_share_free(pmesg, MODULE_ID_SNMPD);
			return 0;
		}
	
        snprintf(ifname, OAM_STRING_LEN, "%s", (char *)pmesg->msg_data);
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return 1;
    }

    return 0;
}

void hhrbfdDetectConfigTable_node_free(struct bfd_sess *node)
{
    XFREE(0, node);
}

void hhrMplsTpOamConfigTable_node_free(struct snmp_oam *node)
{
    XFREE(0, node);
}

static void *oam_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case hhrMplsTpOamConfigTable:
            return XCALLOC(0, sizeof(struct snmp_oam));
            break;

        case hhrbfdDetectConfigTable:
            return XCALLOC(0, sizeof(struct bfd_sess));
            break;

        default:
            break;
    }

    return NULL;
}

static void oam_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case hhrbfdDetectConfigTable:
            node_add = oam_node_xcalloc(hhrbfdDetectConfigTable);
            memcpy(node_add, node, sizeof(struct bfd_sess));
            listnode_add(hhrbfdDetectConfigTable_list, node_add);
            break;

        case hhrMplsTpOamConfigTable:
            node_add = oam_node_xcalloc(hhrMplsTpOamConfigTable);
            memcpy(node_add, node, sizeof(struct snmp_oam));
            listnode_add(hhrMplsTpOamConfigTable_list, node_add);
            break;

        default:
            break;
    }
}

static void oam_node_del_all(int table_num)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    switch (table_num)
    {
        case hhrbfdDetectConfigTable:
            list_delete_all_node(hhrbfdDetectConfigTable_list);
            break;

        case hhrMplsTpOamConfigTable:
            list_delete_all_node(hhrMplsTpOamConfigTable_list);
            break;

        default:
            break;
    }
}

void oam_list_init(void)
{
    hhrbfdDetectConfigTable_list = list_new();
    hhrbfdDetectConfigTable_list->del = (void (*)(void *))hhrbfdDetectConfigTable_node_free;

    hhrMplsTpOamConfigTable_list = list_new();
    hhrMplsTpOamConfigTable_list->del = (void (*)(void *))hhrMplsTpOamConfigTable_node_free;
}

static void *oam_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;


    /* rip instance table */
    struct snmp_oam     *data1_find;
    u_int32_t       index1;

    /* rip instance if table */
    struct bfd_sess     *data2_find;
    u_int32_t       index2;


    switch (table_num)
    {
        case hhrMplsTpOamConfigTable:
            index1 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrMplsTpOamConfigTable_list, node, nnode, data1_find))
            {
                if (0 == index1)
                {
                    return hhrMplsTpOamConfigTable_list->head->data;
                }

                if (data1_find->info.session_id == index1)
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
                            return node->next->data;
                        }
                    }
                }
            }

            break;

        case hhrbfdDetectConfigTable:
            index2 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrbfdDetectConfigTable_list, node, nnode, data2_find))
            {
                if (0 == index2)
                {
                    return hhrbfdDetectConfigTable_list->head->data;
                }

                if (data2_find->session_id == index2)
                {
                    if (1 == exact) //get
                    {
                        return data2_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            break;
    }

    return (NULL);
}



int hhrMplsTpOamConfigTable_get_data_from_ipc(u_int32_t index)
{
    struct ipc_mesg_n *pmesg = NULL ;
    struct snmp_oam *poam_sess = NULL;
    int data_num = 0;
    int ret = 0;

    pmesg = mpls_com_get_tpoam_instance(index, MODULE_ID_SNMPD, &data_num);
	if(NULL == pmesg)
	{
		return FALSE;
	}
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	poam_sess = (struct snmp_oam *)pmesg->msg_data;
	if(NULL == poam_sess)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}

    if (0 == data_num)
    {
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            oam_node_add(hhrMplsTpOamConfigTable, poam_sess);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': add oam_session->info.mp_id [%d]\n", __FILE__, __LINE__, __func__ , poam_sess->info.session_id);
            poam_sess++;
        }
    }
	time(&hhrMplsTpOamConfigTable_time_old);	//refresh time_old after refresh cache data
	mem_share_free(pmesg, MODULE_ID_SNMPD);
	
	return TRUE;
}

struct snmp_oam *hhrMplsTpOamConfigTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct snmp_oam *poam_sess = NULL;
    struct snmp_oam *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old;   //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrMplsTpOamConfigTable_GET_NEXT_INTERVAL))
    {
        hhrMplsTpOamConfigTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrMplsTpOamConfigTable_time_old) > hhrMplsTpOamConfigTable_REFRESH_TIME) && \
            (UNLOCK == hhrMplsTpOamConfigTable_lock))
    {
        if (NULL == hhrMplsTpOamConfigTable_list)
        {
            hhrMplsTpOamConfigTable_list = list_new();
            hhrMplsTpOamConfigTable_list->del = (void (*)(void *))hhrMplsTpOamConfigTable_node_free;
        }

        if (0 != hhrMplsTpOamConfigTable_list->count)
        {
            oam_node_del_all(hhrMplsTpOamConfigTable);
        }
    }

    /* then lock */
    hhrMplsTpOamConfigTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrMplsTpOamConfigTable_list)
    {
        hhrMplsTpOamConfigTable_list = list_new();
        hhrMplsTpOamConfigTable_list->del = (void (*)(void *))hhrMplsTpOamConfigTable_node_free;
    }

    if (0 == hhrMplsTpOamConfigTable_list->count)
    {
        index_temp = 0;
        ret = hhrMplsTpOamConfigTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    poam_sess = (struct snmp_oam *)oam_node_lookup(exact, hhrMplsTpOamConfigTable, index);

    if (NULL == poam_sess)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrMplsTpOamConfigTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct snmp_oam *)hhrMplsTpOamConfigTable_list->tail->data;
            ret = hhrMplsTpOamConfigTable_get_data_from_ipc(entry_temp->info.session_id);

            if (FALSE == ret)
            {
                hhrMplsTpOamConfigTable_lock = UNLOCK;  //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            poam_sess = (struct snmp_oam *)oam_node_lookup(exact, hhrMplsTpOamConfigTable, index);

            if (NULL != poam_sess)
            {
                *index_next = poam_sess->info.session_id;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrMplsTpOamConfigTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return poam_sess;
            }
        }
    }

    *index_next = poam_sess->info.session_id;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrMplsTpOamConfigTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return poam_sess;
}


int hhrbfdDetectConfigTable_get_data_from_ipc(u_int32_t index)
{
    struct ipc_mesg_n *pmesg = NULL ;
    struct bfd_sess *pBfd_sess = NULL;
    int data_num = 0;
    int ret = 0;
	
    pmesg = mpls_com_get_bfd_instance(index, MODULE_ID_SNMPD, &data_num);
	if(NULL == pmesg)
	{
		return FALSE;
	}
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]data_num [%d]\n", __FILE__, __LINE__, __func__ , index, data_num);

	pBfd_sess = (struct bfd_sess *)pmesg->msg_data;
	if(NULL == pBfd_sess)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
	}

    if (0 == data_num)
    {
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            oam_node_add(hhrbfdDetectConfigTable, pBfd_sess);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pBfd_sess.local_id [%d]\n", __FILE__, __LINE__, __func__ , pBfd_sess->local_id);
            pBfd_sess++;
        }
    }
	time(&hhrbfdDetectConfigTable_time_old);	//refresh time_old after refresh cache data
	mem_share_free(pmesg, MODULE_ID_SNMPD);

	return TRUE;
}

struct bfd_sess *hhrbfdDetectConfigTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct bfd_sess *pBfd_sess = NULL;
    struct bfd_sess *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old;   //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrbfdDetectConfigTable_GET_NEXT_INTERVAL))
    {
        hhrbfdDetectConfigTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrbfdDetectConfigTable_time_old) > hhrbfdDetectConfigTable_REFRESH_TIME) && \
            (UNLOCK == hhrbfdDetectConfigTable_lock))
    {
        if (NULL == hhrbfdDetectConfigTable_list)
        {
            hhrbfdDetectConfigTable_list = list_new();
            hhrbfdDetectConfigTable_list->del = (void (*)(void *))hhrbfdDetectConfigTable_node_free;
        }

        if (0 != hhrbfdDetectConfigTable_list->count)
        {
            oam_node_del_all(hhrbfdDetectConfigTable);
        }
    }

    /* then lock */
    hhrbfdDetectConfigTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrbfdDetectConfigTable_list)
    {
        hhrbfdDetectConfigTable_list = list_new();
        hhrbfdDetectConfigTable_list->del = (void (*)(void *))hhrbfdDetectConfigTable_node_free;
    }

    if (0 == hhrbfdDetectConfigTable_list->count)
    {
        index_temp = 0;
        ret = hhrbfdDetectConfigTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pBfd_sess = (struct bfd_sess *)oam_node_lookup(exact, hhrbfdDetectConfigTable, index);

    if (NULL == pBfd_sess)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrbfdDetectConfigTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct bfd_sess *)hhrbfdDetectConfigTable_list->tail->data;
            ret = hhrbfdDetectConfigTable_get_data_from_ipc(entry_temp->session_id);

            if (FALSE == ret)
            {
                hhrbfdDetectConfigTable_lock = UNLOCK;  //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pBfd_sess = (struct bfd_sess *)oam_node_lookup(exact, hhrbfdDetectConfigTable, index);

            if (NULL != pBfd_sess)
            {
                *index_next = pBfd_sess->session_id;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrbfdDetectConfigTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pBfd_sess;
            }
        }
    }

    *index_next = pBfd_sess->session_id;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrbfdDetectConfigTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pBfd_sess;
}

u_char *hhrbfdDetectConfigTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bfd_sess *data = NULL;
    int index = 0 ;
    int index_next = 0 ;
    int ret = 0;
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index , exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);

    if (ret < 0)
    {
        return NULL;
    }


    /* get data by index and renew index_next */
    data = hhrbfdDetectConfigTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == data)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    switch (vp->magic)
    {
        case hhrbfdRemoteDiscriminator:
            *var_len = sizeof(uint32_t);
            uint_value = data->remote_id ;
            return (u_char *)(&uint_value);

        case hhrbfdccInterval:
            *var_len = sizeof(uint32_t);
            int_value = data->cc_interval_cfg ;
            int_value = int_value / 1000 ;
            return (u_char *)(&int_value);

        case hhrbfdDetectMultiplier:
            *var_len = sizeof(uint32_t);
            int_value = data->cc_multiplier_cfg ;
            return (u_char *)(&int_value);

        case hhrbfdDetectType:
            *var_len = sizeof(int);
            int_value = data->type ;

            if (BFD_TYPE_INTERFACE == int_value)
            {
                int_value = 1;
            }
            else if (BFD_TYPE_PW == int_value)
            {
                int_value = 2;
            }
            else if (BFD_TYPE_IP == int_value)
            {
                int_value = 3;
            }
            else if (BFD_TYPE_LSP == int_value)
            {
                int_value = 4;
            }
            else if (BFD_TYPE_TUNNEL == int_value)
            {
                int_value = 5;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)(&int_value);

        case hhrbfdDetectObjectDescr:
            memset(str_value, 0, OAM_STRING_LEN);
            mpls_get_bfd_session_ifname(MODULE_ID_SNMPD, index_next, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrbfdSourceIp:
            *var_len = sizeof(uint32_t);
            ip_value = data->srcip ;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrbfdSessionState:
            *var_len = sizeof(uint32_t);
            int_value = data->status;

            if (BFD_STATUS_DOWN == int_value)
            {
                int_value = 2;
            }
            else if (BFD_STATUS_UP == int_value)
            {
                int_value = 1;
            }
            else if (BFD_STATUS_INIT == int_value)
            {
                int_value = 3;
            }
            else
            {
                int_value = 4;
            }

            return (u_char *)(&int_value);

        case hhrbfdLocalDiscriminator:
            *var_len = sizeof(uint32_t);
            uint_value = data->local_id ;
            return (u_char *)(&uint_value);

        case hhrbfdWithIPOrUDPHeaderEnable:
            *var_len = sizeof(uint32_t);

            if (1 == data->without_ip)
            {
                int_value = 2;
            }
            else
            {
                int_value = 1;
            }

            return (u_char *)(&int_value);

        default:
            return (NULL);
    }
}


u_char *hhrMplsTpOamConfigTable_get(struct variable *vp,
                                    oid *name, size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct snmp_oam *data = NULL;
    int index = 0 ;
    int index_next = 0 ;
    int ret = 0;
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index , exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    data = hhrMplsTpOamConfigTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == data)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index_next [%d]\n", __FILE__, __LINE__, __func__ , index_next);
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    switch (vp->magic)
    {
        case hhrTpOamPriority:
            *var_len = sizeof(int);
            int_value = data->info.priority;
            return (u_char *)(&int_value) ;

        case hhrTpOamPeerMep:
            *var_len = sizeof(uint32_t);
            uint_value = data->info.remote_mep;
            return (u_char *)(&uint_value);

        case hhrTpOamCCEnable:
            *var_len = sizeof(int);
            int_value = (1 == data->info.cc_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrTpOamCCInterval:
            *var_len = sizeof(int);
            int_value = data->info.cc_interval;

            if (3 == int_value)
            {
                int_value = 1;
            }
            else if (10 == int_value)
            {
                int_value = 2;
            }
            else if (30 == int_value)
            {
                int_value = 3;
            }
            else if (100 == int_value)
            {
                int_value = 4;
            }
            else if (300 == int_value)
            {
                int_value = 5;
            }
            else if (1000 == int_value)
            {
                int_value = 6;
            }
            else if (10000 == int_value)
            {
                int_value = 7;
            }
            else if (60000 == int_value)
            {
                int_value = 8;
            }
            else if (600000 == int_value)
            {
                int_value = 9;
            }

            return (u_char *)(&int_value);

        case hhrTpOamAISEnable:
            *var_len = sizeof(int);
            int_value = (1 == data->ais_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrTpOamPathType:
            *var_len = sizeof(int);
            int_value = data->info.type ;

            if (OAM_TYPE_LSP == int_value)
            {
                int_value = 1;
            }
            else if (OAM_TYPE_PW == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrTpOamPathName:
            memset(str_value, 0, OAM_STRING_LEN);

            if (MP_TYPE_MIP == data->info.mp_type)
            {
                if (OAM_TYPE_PW == data->info.type)
                {
                    memcpy(str_value, data->pw_name, NAME_STRING_LEN);
                }

                if (OAM_TYPE_LSP == data->info.type)
                {
                    memcpy(str_value, data->fwd_lsp_name, NAME_STRING_LEN);
                }
            }
            else
            {
                mpls_get_tpoam_session_ifname(MODULE_ID_SNMPD, index_next, str_value);
            }

            *var_len = strlen(str_value);
            return (str_value);

        case hhrTpOamSectionDestMAC:
            *var_len = 6;
            memcpy(&mac_value, data->info.mac, 6);
            return (mac_value);

        case hhrTpOamSectionVlan:
            *var_len = sizeof(int);
            int_value = data->info.vlan ;
            return (u_char *)(&int_value) ;

        case hhrTpOamSectionChannelType:
            *var_len = sizeof(int);
            int_value = (0x8902 == data->info.channel_type) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrTpOamSectionAlarmEnable:
            *var_len = sizeof(int);
            int_value = (1 == data->alarm_rpt) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrTpOamNodeType:
            *var_len = sizeof(int);
            int_value = data->info.mp_type;
            return (u_char *)(&int_value);

        case hhrTpOamMpid:
            *var_len = sizeof(int);
            int_value = data->info.mp_id;
            return (u_char *)(&int_value);

        case hhrTpOamPeerMepForward:
            *var_len = sizeof(int);
            int_value = data->fwd_rmep;
            return (u_char *)(&int_value);

        case hhrTpOamPeerMepReverse:
            *var_len = sizeof(int);
            int_value = data->rev_rmep;
            return (u_char *)(&int_value);

        case hhrTpOamAISLevel:
            *var_len = sizeof(int);
            int_value = data->ais_level;
            return (u_char *)(&int_value);

        case hhrTpOamAISInterval:
            *var_len = sizeof(int);

            if (60 == data->ais_interval)
            {
                int_value = 2;
            }
            else
            {
                int_value = 1;
            }

            return (u_char *)(&int_value);

        case hhrTpOamBindMeg:
            memset(str_value, 0, OAM_STRING_LEN);
            memcpy(str_value, data->info.megname, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTpOamPathNameRevLsp:
            memset(str_value, 0, OAM_STRING_LEN);
            memcpy(str_value, data->rsv_lsp_name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTpOamState:
            *var_len = sizeof(int);

            if (OAM_STATUS_UP == data->info.state)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrTpOamLCKEnable:
            *var_len = sizeof(int);
            int_value = data->lck_enable ? 1 : 2;
            return (uchar *)(&int_value);

        case hhrTpOamLCKLevel:
            *var_len = sizeof(uint32_t);
            uint_value = data->lck_level;
            return (uchar *)(&uint_value);

        case hhrTpOamLCKInterval:
            *var_len = sizeof(int);

            if (1 == data->lck_interval)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (uchar *)(&int_value);

        case hhrTpOamCSFEnable:
            *var_len = sizeof(int);
            int_value = data->csf_enable ? 1 : 2;
            return (uchar *)(&int_value);

        case hhrTpOamCSFLevel:
            *var_len = sizeof(uint32_t);
            uint_value = data->csf_level;
            return (uchar *)(&uint_value);

        case hhrTpOamCSFInterval:
            *var_len = sizeof(int);

            if (1 == data->csf_interval)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (uchar *)(&int_value);

        default:
            return NULL;
    }
}

struct ipc_mesg_n *mpls_com_get_oam_meg_bulk(uchar *megname, int module_id, int *pdata_num)
{
    int name_len = strlen((void *)megname);

    if (name_len > 0)
    {
        name_len += 1;
    }

    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(megname, name_len, 1, MODULE_ID_MPLS,
                                             module_id, IPC_TYPE_MPLSOAM, OAM_SUBTYPE_MEG,
                                             IPC_OPCODE_GET_BULK, 0, 2000);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

int hhrMplsTpOamMegTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                           struct oam_meg *index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct oam_meg *meg_data = NULL;
    int data_num = 0;
    int ret = 0;

    pMsgRcv = mpls_com_get_oam_meg_bulk(index->name, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}
	meg_data = (struct oam_meg *)pMsgRcv->msg_data;

    if (0 == data_num || NULL == meg_data)
    {
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrMplsTpOamMegTable_cache, meg_data , sizeof(struct oam_meg));
            meg_data++;
        }

		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }

}

struct oam_meg *hhrMplsTpOamMegTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                 int exact,
                                                 const struct oam_meg *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;

    struct oam_meg *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0x00 == index_input->name[0])
        {
            return cache->data_list->head->data ;
        }

        if (0 == strcmp(data1_find->name, index_input->name))
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

u_char *hhrMplsTpOamMegTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct oam_meg *meg_data = NULL;
    struct oam_meg meg_index;
    u_char      sIndex[NAME_STRING_LEN] = {0};
    int ret = 0;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, NAME_STRING_LEN, exact);

    if (0 > ret)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%s]\n", __FILE__, __LINE__, __func__, sIndex);
        return NULL;
    }

    memset(&meg_index, 0, sizeof(struct oam_meg));
    memcpy(&(meg_index.name), sIndex, strlen(sIndex));

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': sIndex [%s]\n", __FILE__, __LINE__, __func__ , sIndex);

    if (NULL == hhrMplsTpOamMegTable_cache)
    {
        hhrMplsTpOamMegTable_cache = snmp_cache_init(sizeof(struct oam_meg),
                                                     hhrMplsTpOamMegTable_get_data_from_ipc,
                                                     hhrMplsTpOamMegTable_node_lookup);

        if (NULL == hhrMplsTpOamMegTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    meg_data = snmp_cache_get_data_by_index(hhrMplsTpOamMegTable_cache, exact, &meg_index);

    if (NULL == meg_data)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_octstring_index_set(vp, name, length, meg_data->name, strlen(meg_data->name));
    }

    switch (vp->magic)
    {
        case hhrTpOamMegLevel:
            *var_len = sizeof(int);
            int_value = meg_data->level;
            return (u_char *)(&int_value);

        default :
            return NULL;
    }

    return NULL;
}

u_char *hhrBFDGlobal_get(struct variable *vp,
                         oid *name, size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    int data_num = 0;
    struct bfd_global *global_data = NULL ;

    if (header_generic(vp, name, length, exact, var_len, write_method) ==
            MATCH_FAILED)
    {
        return NULL;
    }

    global_data = mpls_com_get_bfd_global(MODULE_ID_SNMPD, &data_num);

    if (NULL == global_data)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrbfdEnable:
            *var_len = sizeof(int);
            int_value = (1 == global_data->enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrbfdGlobalPriority:
            *var_len = sizeof(uint32_t);
            uint_value = global_data->priority;
            return (u_char *)(&uint_value);

        case hhrbfdGlobalMinReInterval:
            *var_len = sizeof(uint32_t);
            uint_value = global_data->cc_interval;
            uint_value = uint_value / 1000;
            return (u_char *)(&uint_value);

        case hhrbfdGlobalDetectMultipier:
            *var_len = sizeof(uint32_t);
            uint_value = global_data->multiplier;
            return (u_char *)(&uint_value);

        case hhrbfdGlobalWTR:
            *var_len = sizeof(uint32_t);
            uint_value = global_data->wtr;
            return (u_char *)(&uint_value);

        default:
            return (NULL);
    }
}

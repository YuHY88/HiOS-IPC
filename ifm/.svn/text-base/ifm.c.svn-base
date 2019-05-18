/***********************************************************************
*
* Ifm.c
*
*
*
*
* Copyright (C) 2002 Roaring Penguin Software Inc.
*
* This software may be distributed under the terms of the GNU General
* Public License, Version 2 or (at your option) any later version.
*
* LIC: GPL
*
***********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include <lib/inet_ip.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <sys/time.h>
#include <string.h>
#include <lib/msg_ipc.h>
#include <lib/linklist.h>
#include <lib/zassert.h>
#include <lib/devm_com.h>
#include <lib/errcode.h>
#include <lib/alarm.h>
#include <lib/log.h>
#include <qos/qos_mapping.h>
#include "ifm_qos.h"
#include "ifm_message.h"
#include "ifm.h"
#include "ifm_register.h"
#include "ifm_errno.h"
#include "ifm_init.h"
#include "ifm_message.h"
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include "ifm_alarm.h"
#include "lib/gpnSocket/socketComm/gpnAlmTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"
#include "lib/alarm.h"
uint64_t ht201_counter[8] = {0};
uint64_t ht2200_counter[6][8] = {0};

static unsigned int compute_hash ( void *hash_key )
{
    if ( NULL == hash_key )
    {
        return 0;
    }
    return ( ( unsigned int ) hash_key );
}

static int compare ( void *item, void *hash_key )
{
    struct hash_bucket *pbucket = item;

    if ( NULL == pbucket || NULL == hash_key )
    {
        return 1;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return 0;
    }
    else
    { return 1; }
}

int
ifm_intf_cmp_func ( struct ifm_entry *ifp1 , struct ifm_entry *ifp2 )
{
    if ( NULL == ifp1 || NULL == ifp2 )
    {
        return 1;
    }

    if ( ifp1->ifm.ifindex > ifp2->ifm.ifindex )
    {
        return 1;
    }

    return -1;

}
/*接口管理hash表初始化*/
void ifm_table_init ( int size )
{
    hios_hash_init ( &ifm_table, size, compute_hash, compare );
    is_sla_enable = 0;
	dcn_flag = 0;

    memset ( &ifm_smsg_hal_list, 0, sizeof ( struct list ) );
    memset ( &ifm_smsg_ftm_list, 0, sizeof ( struct list ) );
    memset ( &ifm_smsg_comm_list, 0, sizeof ( struct list ) );
    memset ( &ifm_rmsg_list, 0, sizeof ( struct list ) );

	memset ( &snmp_list, 0, sizeof ( struct list ) );
	snmp_list.cmp = ( int ( * ) ( void *, void * ) ) ifm_intf_cmp_func;
	memset ( &snmp_sfp_list, 0, sizeof ( struct list ) );
	snmp_sfp_list.cmp = ( int ( * ) ( void *, void *) ) ifm_intf_cmp_func;
	memset ( &snmp_subencap_list, 0, sizeof( struct list ) );
	snmp_subencap_list.cmp = ( int ( * ) ( void *, void * ) )ifm_intf_cmp_func;
	memset( &port_common_list, 0, sizeof( struct list ) );
	port_common_list.cmp = ( int ( * ) ( void *, void * ) ) ifm_intf_cmp_func;
	memset( &snmp_info_list, 0, sizeof( struct list));
	snmp_info_list.cmp = ( int ( * )( void *, void*) )ifm_intf_cmp_func;
}

/*检查MAC地址是否合法*/
int ifm_checkout_mac ( uchar *mac )
{
    if ( mac == NULL )
    {
        return IFM_ERR_PARAM;
    }

    if ( ( mac[0] & mac[1] & mac[2]
            & mac[3] & mac[4] & mac[5] ) == 0xff )
    {
        zlog_err ( "%s[%d] can't set broadcast mac for port\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM_MAC_BC;
    }
    else if ( mac[0] & 0x01 )
    {
        zlog_err ( "%s[%d] can't set multicasts mac for port\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM_MAC_MC;
    }
    else if ( mac[0] == 0 && mac[1] == 0
              && mac[2] == 0 && mac[3] == 0
              && mac[4] == 0 && mac[5] == 0 )
    {
        zlog_err ( "%s[%d] can't set mac all zero for port\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM_MAC_ZERO;
    }
    return IFM_SUCCESS;
}


/*检查封装vlan是否合法(相同接口下子接口的封装vlan不能重叠)*/

int ifm_checkout_encap ( struct ifm_entry *parent_pifm, struct ifm_entry *pifm, struct ifm_encap *encap )
{
    struct ifm_entry *tpifm;
    struct listnode *node;
    int *pdata = NULL;

    if ( parent_pifm == NULL || pifm == NULL || encap == NULL )
    {
        return IFM_ERR_PARAM;
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pifm->ifm.mode =%d, pifm->ifm.parent = 0x%0x, encap->type =%d svlan_act %d cvlan_act %d svlan [%d-%d] cvlan [%d-%d] svlan_new %d cvlan_new %d\n", __FUNCTION__, __LINE__,
                 pifm->ifm.mode, pifm->ifm.parent, encap->type,
                 encap->svlan_act, encap->cvlan_act,
                 encap->svlan.vlan_start, encap->svlan.vlan_end,
                 encap->cvlan.vlan_start, encap->cvlan.vlan_end,
                 encap->svlan_new.vlan_start, encap->cvlan_new.vlan_start );

    if ( encap->svlan.vlan_start > encap->svlan.vlan_end || encap->cvlan.vlan_start > encap->cvlan.vlan_end )
    {
        zlog_err ( "%s[%d] Can't set end of cvlan <= start of cvlan\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_ENCAP_RANGE;
    }

    if ( pifm->ifm.mode != IFNET_MODE_L2 )
    {
        if ( encap->cvlan_act != VLAN_ACTION_NO
                || encap->svlan_act != VLAN_ACTION_NO )
        {
            zlog_err ( "%s[%d] Can't set vlan translate in l3 mode\n", __FUNCTION__, __LINE__ );
            return IFM_ERR_ENCAP_VLANT_MODE_ERRO;
        }
    }
    else
    {
        if ( encap->type == IFNET_ENCAP_DOT1Q )
        {
            if ( encap->svlan.vlan_start == encap->svlan.vlan_end )
            {
                if ( encap->cvlan_act == VLAN_ACTION_NO && encap->svlan_act == VLAN_ACTION_TRANSLATE )
                {
                    if ( encap->svlan_cos == 8 && encap->svlan.vlan_start == encap->svlan_new.vlan_start )
                    {
                        zlog_err ( "%s[%d] Can't set vlan translate the same vlan\n", __FUNCTION__, __LINE__ );
                        return IFM_ERR_ENCAP_XLVLAN_SAME_ERRO;
                    }
                }
            }
        }
        else if ( encap->type == IFNET_ENCAP_QINQ )
        {
            if ( encap->cvlan.vlan_start == encap->cvlan.vlan_end )
            {
                if ( encap->cvlan_act == VLAN_ACTION_NO && encap->svlan_act == VLAN_ACTION_TRANSLATE )
                {
                    if ( encap->svlan_cos == 8 && encap->svlan.vlan_start == encap->svlan_new.vlan_start )
                    {
                        zlog_err ( "%s[%d] Can't set vlan translate the same vlan\n", __FUNCTION__, __LINE__ );
                        return IFM_ERR_ENCAP_XLVLAN_SAME_ERRO;
                    }
                }

                if ( encap->cvlan_act == VLAN_ACTION_TRANSLATE && encap->svlan_act == VLAN_ACTION_TRANSLATE )
                {
                    if ( encap->svlan_cos == 8 && encap->svlan.vlan_start == encap->svlan_new.vlan_start
                            && encap->cvlan_cos == 8 && encap->cvlan.vlan_start == encap->cvlan_new.vlan_start )
                    {
                        zlog_err ( "%s[%d] Can't set vlan translate the same vlan\n", __FUNCTION__, __LINE__ );
                        return IFM_ERR_ENCAP_XLVLAN_SAME_ERRO;
                    }
                }

            }

        }

    }

    if ( pifm->ifm.mode == IFNET_MODE_L3 )
    {
        if ( encap->type == IFNET_ENCAP_UNTAG )
        {
            zlog_err ( "%s[%d] Can't set vlan utag in l3 mode\n", __FUNCTION__, __LINE__ );
            return IFM_ERR_ENCAP_UNTAG_L3;
        }

        if ( encap->cvlan.vlan_start != encap->cvlan.vlan_end )
        {
            zlog_err ( "%s[%d] Can't set vlan range in l3 mode\n", __FUNCTION__, __LINE__ );
            return IFM_ERR_ENCAP_RANGE_L3;
        }

        if ( encap->svlan.vlan_start != encap->svlan.vlan_end )
        {
            zlog_err ( "%s[%d] Can't set vlan range in l3 mode\n", __FUNCTION__, __LINE__ );
            return IFM_ERR_ENCAP_RANGE_L3;
        }

    }

    if ( parent_pifm->sonlist )
    {
        for ( ALL_LIST_ELEMENTS_RO ( parent_pifm->sonlist, node, pdata ) )
        {
            tpifm = ( struct ifm_entry * ) pdata;
            zlog_debug ( IFM_DBG_COMMON,"%s[%d] pold->mode =%d, son ifindex = 0x%0x type= %d encap->type =%d\n", __FUNCTION__, __LINE__,
                         pifm->ifm.mode, tpifm->ifm.ifindex, tpifm->ifm.encap.type, encap->type );
            if ( pifm->ifm.ifindex != tpifm->ifm.ifindex )
            {
                if ( tpifm->ifm.encap.type == encap->type )
                {
                    switch ( encap->type )
                    {
                        case IFNET_ENCAP_UNTAG:

                            zlog_err ( "%s[%d] only one utag subport can config\n", __FUNCTION__, __LINE__ );
                            return IFM_ERR_ENCAP_UNTAG;

                            break;
                        case IFNET_ENCAP_DOT1Q:
                            /*config single vlan*/
                            if ( encap->svlan.vlan_start == encap->svlan.vlan_end )
                            {
                                /*check subport all single vlan if equ*/
                                if ( tpifm->ifm.encap.svlan.vlan_start == tpifm->ifm.encap.svlan.vlan_end )
                                {
                                    if ( encap->svlan.vlan_start == tpifm->ifm.encap.svlan.vlan_start )
                                    {
                                        zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                        return IFM_ERR_ENCAP_DOT1Q;
                                    }
                                }
                                else/*check subport  vlan range if equ*/
                                {
                                    if ( encap->svlan.vlan_start >= tpifm->ifm.encap.svlan.vlan_start
                                            && encap->svlan.vlan_start <= tpifm->ifm.encap.svlan.vlan_end )
                                    {
                                        zlog_err ( "%s[%d] l2 subport can not re\n", __FUNCTION__, __LINE__ );
                                        return IFM_ERR_ENCAP_DOT1Q;
                                    }
                                }

                                if ( encap->cvlan_act == VLAN_ACTION_NO && encap->svlan_act == VLAN_ACTION_TRANSLATE )
                                {
                                    if ( encap->svlan_cos == 8 && encap->svlan.vlan_start == encap->svlan_new.vlan_start )
                                    {
                                        zlog_err ( "%s[%d] Can't set vlan translate the same vlan\n", __FUNCTION__, __LINE__ );
                                        return IFM_ERR_ENCAP_XLVLAN_SAME_ERRO;
                                    }
                                }


                            }
                            else/*config single vlan range*/
                            {
                                /*check subport all single vlan if equ*/
                                if ( tpifm->ifm.encap.svlan.vlan_start == tpifm->ifm.encap.svlan.vlan_end )
                                {
                                    if ( tpifm->ifm.encap.svlan.vlan_start >=  encap->svlan.vlan_start
                                            &&  tpifm->ifm.encap.svlan.vlan_start <=  encap->svlan.vlan_end )
                                    {
                                        if ( encap->svlan.vlan_start == tpifm->ifm.encap.svlan.vlan_start )
                                        {
                                            zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                            return IFM_ERR_ENCAP_DOT1Q;
                                        }
                                    }

                                }
                                else/*check subport  vlan range if equ*/
                                {
                                    if ( ( encap->svlan.vlan_end >= tpifm->ifm.encap.svlan.vlan_start
                                            && encap->svlan.vlan_end <= tpifm->ifm.encap.svlan.vlan_end )
                                            || ( encap->svlan.vlan_start >= tpifm->ifm.encap.svlan.vlan_start
                                                 && encap->svlan.vlan_start <= tpifm->ifm.encap.svlan.vlan_end ) )
                                    {
                                        zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                        return IFM_ERR_ENCAP_DOT1Q;
                                    }
                                }
                            }
                            break;
                        case IFNET_ENCAP_QINQ:
                            if ( encap->svlan.vlan_start == tpifm->ifm.encap.svlan.vlan_start )
                            {
                                /*config single vlan & cvlan*/
                                if ( encap->cvlan.vlan_start == encap->cvlan.vlan_end )
                                {
                                    /*check subport all single vlan & cvlan if equ*/
                                    if ( tpifm->ifm.encap.cvlan.vlan_start == tpifm->ifm.encap.cvlan.vlan_end )
                                    {
                                        if ( encap->cvlan.vlan_start == tpifm->ifm.encap.cvlan.vlan_start )
                                        {
                                            zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                            return IFM_ERR_ENCAP_DOT1Q;
                                        }
                                    }
                                    else/*check subport  vlan range if equ*/
                                    {
                                        if ( encap->cvlan.vlan_start >= tpifm->ifm.encap.cvlan.vlan_start
                                                && encap->cvlan.vlan_start <= tpifm->ifm.encap.cvlan.vlan_end )
                                        {
                                            zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                            return IFM_ERR_ENCAP_DOT1Q;
                                        }
                                    }

                                }
                                else/*config  cvlan range*/
                                {
                                    /*check subport all single vlan if equ*/
                                    if ( tpifm->ifm.encap.cvlan.vlan_start == tpifm->ifm.encap.cvlan.vlan_end )
                                    {
                                        if ( tpifm->ifm.encap.cvlan.vlan_start >=  encap->cvlan.vlan_start
                                                &&  tpifm->ifm.encap.cvlan.vlan_start <=  encap->cvlan.vlan_end )
                                        {
                                            if ( encap->svlan.vlan_start == tpifm->ifm.encap.svlan.vlan_start )
                                            {
                                                zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                                return IFM_ERR_ENCAP_DOT1Q;
                                            }
                                        }

                                    }
                                    else/*check subport  vlan range if equ*/
                                    {
                                        if ( ( encap->cvlan.vlan_end >= tpifm->ifm.encap.cvlan.vlan_start
                                                && encap->cvlan.vlan_end <= tpifm->ifm.encap.cvlan.vlan_end )
                                                || ( encap->cvlan.vlan_start >= tpifm->ifm.encap.cvlan.vlan_start
                                                     && encap->cvlan.vlan_start <= tpifm->ifm.encap.cvlan.vlan_end ) )

                                        {
                                            zlog_err ( "%s[%d] subport vlan can not be repeated or have an intersection\n", __FUNCTION__, __LINE__ );
                                            return IFM_ERR_ENCAP_DOT1Q;
                                        }
                                    }

                                }
                            }
                            break;
                        default :
                            zlog_err ( "%s[%d] encap type=%d\n", __FUNCTION__, __LINE__, encap->type );
                            return IFM_SUCCESS;
                    }
                }
            }
        }
        zlog_debug (IFM_DBG_COMMON, "%s[%d] parent_pifm->sonlist count is %d\n", __FUNCTION__, __LINE__, parent_pifm->sonlist->count );
    }
    else
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] parent_pifm->sonlist is null\n", __FUNCTION__, __LINE__ );
    }


    return IFM_SUCCESS;

}

/*接口加入trunk后此配置项是否允许配置*/
int ifm_checkout_trunk ( struct ifm_entry *pifm, enum IFNET_INFO type )
{

    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    if (  type != IFNET_INFO_SPEED &&  type != IFNET_INFO_STATUS
            && type != IFNET_INFO_SHUTDOWN &&  type != IFNET_INFO_STATISTICS
            && type != IFNET_INFO_ALIAS )
    {
        if ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex)
                && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
        {
            zlog_err ( "%s[%d] Failed to execute command, due to the interface 0x%0x had binded trunk\n",
                       __FUNCTION__, __LINE__, pifm->ifm.ifindex );
            return IFM_ERR_BINDED_TRUNK_ERRO;
        }
    }

    return IFM_SUCCESS;
}
int ifm_checkout_dcn(struct ifm_entry * pifm)
{
	/*dcn使能时，loopback128口和各物理子接口4094不能访问*/
	if(dcn_flag &&
		((IFM_TYPE_IS_LOOPBCK(pifm->ifm.ifindex) && (IFM_SUBPORT_ID_GET(pifm->ifm.ifindex) == 128))
		||(IFM_TYPE_IS_ETHERNET_SUBPORT(pifm->ifm.ifindex) && (IFM_SUBPORT_ID_GET(pifm->ifm.ifindex) == 4094))))
	{
		zlog_err ( "%s[%d] Failed to execute command, due to the interface 0x%0x had enable dcn\n",
				   __FUNCTION__, __LINE__, pifm->ifm.ifindex );

		return IFM_ERR_DCN_ENABLE;
	}

	return IFM_SUCCESS;

}
/*带外接口的此配置项是否允许配置*/
int ifm_checkout_outband ( struct ifm_entry *pifm, enum IFNET_INFO type )
{
    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    if ( IFM_TYPE_IS_OUTBAND ( pifm->ifm.ifindex ) )
    {
        if (  type != IFNET_INFO_SPEED
                && type != IFNET_INFO_STATUS
                && type != IFNET_INFO_SHUTDOWN
                && type != IFNET_INFO_MTU
                && type != IFNET_INFO_FLAP
                && type != IFNET_INFO_STATISTICS
                && type != IFNET_INFO_ALIAS )
        {
            return IFM_ERR_GENERAL;
        }
    }
    return IFM_SUCCESS;
}
/*此配置项是否允许配置*/
int ifm_checkout_mode ( struct ifm_entry *pifm, enum IFNET_INFO type )
{
    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
	if (pifm->ifm.mode == IFNET_MODE_PHYSICAL)
	{
	    if (  type != IFNET_INFO_SPEED &&  type != IFNET_INFO_STATUS
	            && type != IFNET_INFO_SHUTDOWN &&  type != IFNET_INFO_STATISTICS
	            && type != IFNET_INFO_ALIAS )
	    {
			zlog_err ( "%s[%d] Failed to execute command, due to the interface 0x%0x mode is physical\n",
					   __FUNCTION__, __LINE__, pifm->ifm.ifindex );
			return IFM_ERR_PARAM_NOSUPPORT;
	    }
	}

    return IFM_SUCCESS;
}


/* 创建接口数据结构，初始化 ifindex 和 usp 参数*/
struct ifm_entry *ifm_create ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_info *pinfo;

    pifm = ( struct ifm_entry * ) XMALLOC ( MTYPE_IFM_ENTRY, sizeof ( struct ifm_entry ) );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm XMALLOC ifm_entry  erro\n", __FUNCTION__, __LINE__ );
        return NULL;
    }

    memset ( pifm, 0, sizeof ( struct ifm_entry ) );
    pinfo = &pifm->ifm;

    /* set ifindex */
    pinfo->ifindex  = ifindex;

    pifm->devm_flag = 0;
    /* set usp */
    pinfo->unit     = IFM_UNIT_ID_GET ( ifindex );
    pinfo->slot     = IFM_SLOT_ID_GET ( ifindex );
    pinfo->port     = IFM_PORT_ID_GET ( ifindex );
    pinfo->sub_port = IFM_SUBPORT_ID_GET ( ifindex );
    pinfo->type     = IFM_TYPE_ID_GET ( ifindex );

	pifm->pcounter = NULL;
	if(IFM_TYPE_IS_ETHERNET_PHYSICAL(ifindex))
	{
		pifm->pcounter = ( struct ifm_counter * ) XMALLOC ( MTYPE_IFM_ENTRY, sizeof ( struct ifm_counter ) );
        if ( NULL ==  pifm->pcounter )
        {
            zlog_err ( "%s[%d] ifm xmalloc pcounter  erro\n", __FUNCTION__, __LINE__ );
        }
        else
        {
            memset ( pifm->pcounter , 0, sizeof ( struct ifm_counter ) );
        }
		
	}
    return pifm;
}


/* 查找一个接口数据结构不包括隐藏口 */
struct ifm_entry *ifm_lookup ( uint32_t ifindex )
{
    struct hash_bucket *pbucket = NULL;
    struct ifm_entry *pifm = NULL;

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm ifm_lookup ifindex 0x%0x\n", __FUNCTION__, __LINE__, ifindex );

    pbucket = hios_hash_find ( &ifm_table, ( void * ) ifindex );
    if ( NULL == pbucket )
    {
        return NULL;
    }
    else
    {
        pifm = ( struct ifm_entry * ) pbucket->data;
        if ( NULL == pifm )
        {
            zlog_err ( "%s[%d] ifm pbucket->data is null  ifindex 0x%0x\n",
                       __FUNCTION__, __LINE__, ifindex );
            return NULL;
        }

        if ( !pifm->ifm.hide )
        {
            return pifm;
        }
        else
        {
            zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm get ifindex 0x%0x hide %d hide %d\n", __FUNCTION__, __LINE__,
                         pifm->ifm.ifindex,
                         pifm->port_info.hide,
                         pifm->ifm.hide );
            return NULL;
        }
    }
}

/* 查找一个接口数据结构包括隐藏口 */
struct ifm_entry *ifm_lookup2 ( uint32_t ifindex )
{
    struct hash_bucket *pbucket = NULL;
    struct ifm_entry *pifm = NULL;

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm ifm_lookup ifindex 0x%0x\n", __FUNCTION__, __LINE__, ifindex );

    pbucket = hios_hash_find ( &ifm_table, ( void * ) ifindex );
    if ( NULL == pbucket )
    {
        return NULL;
    }
    else
    {
        pifm = ( struct ifm_entry * ) pbucket->data;

        return pifm;

    }
}


/* 添加到 hash 表 */
int ifm_add_wait_ack ( struct ifm_entry *pifm )
{
    struct hash_bucket *pbucket = NULL;
    struct ifm_event event;
    int ret;

    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pbucket = ( struct hash_bucket * ) XMALLOC ( MTYPE_HASH_BACKET, sizeof ( struct hash_bucket ) );
    if ( NULL == pbucket )
    {
        zlog_err ( "%s[%d] ifm XMALLOC pbucket  erro\n", __FUNCTION__, __LINE__ );
        return 1;
    }

    memset ( pbucket, 0, sizeof ( struct hash_bucket ) );
    pbucket->hash_key = ( void * ) pifm->ifm.ifindex;
    pbucket->data     = pifm;

    ret = hios_hash_add ( &ifm_table, pbucket );
    if ( ret )
    {
        zlog_err ( "ifm hios_hash_add errocode=%d", ret );
        XFREE ( MTYPE_IFM_ENTRY, pifm );
        XFREE ( MTYPE_HASH_BACKET, pbucket );
        return IFM_ERR_INTERFACE_OUTOFNUM_ERRO;
    }

     /*创建子接口或者虚接口需要等待返回*/
    if ( ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT ) || ( pifm->ifm.sub_type == IFNET_SUBTYPE_VPORT ))
    {
		/*ret = ifm_send_hal_wait_ack ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
									  0, IPC_OPCODE_ADD, pifm->ifm.ifindex );*/
		ret = ifm_send_hal_wait_ack(& ( pifm->ifm ), sizeof ( struct ifm_info ), 1, MODULE_ID_HAL, 
					MODULE_ID_IFM, IPC_TYPE_IFM, 0, IPC_OPCODE_ADD, pifm->ifm.ifindex);
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ipc send hal wait ack errocode=%d,ifindex=0x%0x",
                       __FUNCTION__, __LINE__, ret, pifm->ifm.ifindex );

            hios_hash_delete ( &ifm_table, pbucket );
            XFREE ( MTYPE_IFM_ENTRY, pifm );
            XFREE ( MTYPE_HASH_BACKET, pbucket );
            return ret;
        }
    }	
	
    else  /*物理接口需要发送 port 信息 */
    {
        /*ret = ifm_send_hal_wait_ack ( & ( pifm->ifm ), sizeof ( struct ifm_info ) + sizeof ( struct ifm_port ),
                             0, IPC_OPCODE_ADD, pifm->ifm.ifindex );*/
        ret = ifm_send_hal_wait_ack(& ( pifm->ifm ), sizeof ( struct ifm_info ) + sizeof ( struct ifm_port ), 
        		1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM, 0, IPC_OPCODE_ADD, pifm->ifm.ifindex);
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ipc send hal wait ack errocode=%d,ifindex=0x%0x",
                       __FUNCTION__, __LINE__, ret, pifm->ifm.ifindex );

            hios_hash_delete ( &ifm_table, pbucket );
            XFREE ( MTYPE_IFM_ENTRY, pifm );
            XFREE ( MTYPE_HASH_BACKET, pbucket );
            return IFM_ERR_HAL;
        }

    }
    /*通知端口添加事件*/
    event.event = IFNET_EVENT_IF_ADD;
    event.ifindex = pifm->ifm.ifindex;
    event.mode = pifm->ifm.mode;
    ifm_event_notify ( IFNET_EVENT_IF_ADD, &event );
    return 0;
}

/* 添加到 hash 表 */
int ifm_add ( struct ifm_entry *pifm )
{
    struct hash_bucket *pbucket = NULL;
    struct ifm_event event;
    int ret;
    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pbucket = ( struct hash_bucket * ) XMALLOC ( MTYPE_HASH_BACKET, sizeof ( struct hash_bucket ) );
    if ( NULL == pbucket )
    {
        zlog_err ( "%s[%d] ifm XMALLOC pbucket  erro\n", __FUNCTION__, __LINE__ );
        return 1;
    }

    memset ( pbucket, 0, sizeof ( struct hash_bucket ) );
    pbucket->hash_key = ( void * ) pifm->ifm.ifindex;
    pbucket->data     = pifm;

    ret = hios_hash_add ( &ifm_table, pbucket );
    if ( ret )
    {
        zlog_err ( "ifm hios_hash_add errocode=%d", ret );
        XFREE ( MTYPE_IFM_ENTRY, pifm );
        XFREE ( MTYPE_HASH_BACKET, pbucket );
        return IFM_ERR_INTERFACE_OUTOFNUM_ERRO;
    }
	if(pifm->ifm.type == IFNET_TYPE_VCG || pifm->ifm.type == IFNET_TYPE_E1)
	{
		/*no send info to hal*/
	}
	else
	{
	    /*send info to hal wait no ack*/
	    if ( ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT ) || ( pifm->ifm.sub_type == IFNET_SUBTYPE_VPORT ) )
	    {
	        ret = ifm_send_hal ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
	                             0, IPC_OPCODE_ADD, pifm->ifm.ifindex );
	        if ( ret < 0 )
	        {
	            zlog_err ( "%s[%d] ipc_send_hal errocode=%d,ifindex=0x%0x",
	                       __FUNCTION__, __LINE__, ret, pifm->ifm.ifindex );

	            hios_hash_delete ( &ifm_table, pbucket );
	            XFREE ( MTYPE_IFM_ENTRY, pifm );
	            XFREE ( MTYPE_HASH_BACKET, pbucket );
	            return ret;
	        }

	    }
	    else  /*物理接口需要发送 port 信息 */
	    {
	        ret = ifm_send_hal ( & ( pifm->ifm ), sizeof ( struct ifm_info ) + sizeof ( struct ifm_port ),
	                             0, IPC_OPCODE_ADD, pifm->ifm.ifindex );
	        if ( ret < 0 )
	        {
	            zlog_err ( "%s[%d] ipc_send_hal errocode=%d,ifindex=0x%0x",
	                       __FUNCTION__, __LINE__, ret, pifm->ifm.ifindex );
	            hios_hash_delete ( &ifm_table, pbucket );
	            XFREE ( MTYPE_IFM_ENTRY, pifm );
	            XFREE ( MTYPE_HASH_BACKET, pbucket );
	            return IFM_ERR_HAL;
	        }

	    }
	}
    /*send event of interface add*/
    event.event = IFNET_EVENT_IF_ADD;
    event.ifindex = pifm->ifm.ifindex;
    event.mode = pifm->ifm.mode;
    ifm_event_notify ( IFNET_EVENT_IF_ADD, &event );

    return 0;
}


/* 删除一个接口 */
int ifm_delete_if ( uint32_t ifindex )
{
    int ret = 0;
    struct hash_bucket *pbucket = NULL;
    struct ifm_entry *entry = NULL;
    struct ifm_event event;

    zlog_debug ( IFM_DBG_COMMON,"%s[%d]ifm_delete  ifindex 0x%0x \n",__FUNCTION__,__LINE__, ifindex );

    pbucket = hios_hash_find ( &ifm_table, ( void * ) ifindex );
    if ( pbucket == NULL )
    {
        zlog_err ( "%s[%d] ifm delete no find ifindex 0x%0x \n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }

    entry = ( struct ifm_entry * ) pbucket->data;
    if ( entry == NULL )
    {
        zlog_err ( "%s[%d] entry  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    /*通知接口删除事件*/
    event.event = IFNET_EVENT_IF_DELETE;
    event.ifindex = ifindex;
    event.mode = entry->ifm.mode;
	if(entry->ifm.sla_flag)
	{
		entry->ifm.sla_flag = 0;
		is_sla_enable = 0;
	}

    ifm_event_notify ( IFNET_EVENT_IF_DELETE, &event );

    /*下发接口删除消息到hal */
    ret = ifm_send_hal ( NULL, 0,
                         IPC_OPCODE_DELETE, IPC_OPCODE_DELETE, ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",
                   __FUNCTION__, __LINE__, ret, ifindex );
        return IFM_ERR_HAL;
    }

    /*下发接口删除消息到ftm */
    ret = ifm_send_ftm ( NULL, 0, 0, IPC_OPCODE_DELETE, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "ifm send msg to ftm  delete interface 0x%0x errocode=%d", ifindex, ret );
        return IFM_ERR_NOTIFY;
    }

    /*如果接口为为子接口更新父接口son list*/
    if ( entry->ifm.parent )
    {
        ifm_set_son ( entry->ifm.parent, entry, IFM_OPCODE_DELETE );
    }

    if ( QOS_TYPE_INVALID != entry->ifm.qos.domain_type )
    {
        if (!IFM_TYPE_IS_TUNNEL(ifindex))
        {
            ret = qos_domain_ref_operate ( entry->ifm.qos.domain_id, entry->ifm.qos.domain_type, MODULE_ID_IFM, REF_OPER_DECREASE );
            if ( ret )
            {
                zlog_err ( "%s[%d] qos_domain_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
                return ret;
            }
        }
    }

    if ( entry->ifm.qos.phb_enable )
    {
        if (!IFM_TYPE_IS_TUNNEL(ifindex))
        {
            ret = qos_phb_ref_operate ( entry->ifm.qos.phb_id, entry->ifm.qos.phb_type, MODULE_ID_IFM, REF_OPER_DECREASE );
            if ( ret )
            {
                zlog_err ( "%s[%d] qos_phb_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
                return ret;
            }
        }
    }

	/*插拔板卡清除接口所有告警*/

	ret = ifm_link_alarm_report ( entry, IPC_OPCODE_CLEAR );
	if ( ret != 0 )
	{
		zlog_err("%s[%d]ifm_link_alarm_report err %d\n",__FUNCTION__,__LINE__, ret );
		return ret;
	}

    /*free interface*/
    hios_hash_delete ( &ifm_table, pbucket );

	XFREE (MTYPE_IFM_ENTRY, entry->pcounter );
	entry->pcounter = NULL;
    XFREE ( MTYPE_IFM_ENTRY, entry );
	entry = NULL;
    XFREE ( MTYPE_HASH_BACKET, pbucket );
	pbucket = NULL;

    return IFM_SUCCESS;
}

/*删除接口*/
int ifm_delete ( uint32_t ifindex )
{
    int ret = 0;
    struct hash_bucket *pbucket = NULL;
    struct ifm_entry *pifm = NULL;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		
    zlog_debug ( IFM_DBG_COMMON,"%s[%d] delete ifindex 0x%0x \n", __FUNCTION__, __LINE__, ifindex );

    pbucket = hios_hash_find ( &ifm_table, ( void * ) ifindex );
    if ( pbucket == NULL )
    {
        zlog_err ( "%s[%d] ifm delete no find ifindex 0x%0x \n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }

    pifm = ( struct ifm_entry * ) pbucket->data;
    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
   /*设置devm 操作接口标志位*/
    pifm->devm_flag = 1;
	/*接口告警解除注册*/
	if(IFM_TYPE_IS_ETHERNET_PHYSICAL(pifm->ifm.ifindex))
	{
		gPortInfo.iAlarmPort = IFM_ETH_SFP_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);

		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);
	}
	else if ( IFM_TYPE_IS_ETHERNET_SUBPORT(pifm->ifm.ifindex))
	{
		gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);
	}
	else if ( pifm->ifm.type ==  IFNET_TYPE_VLANIF )
	{
		gPortInfo.iAlarmPort = IFM_VLAN_OAM_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);
	}
	else if ( pifm->ifm.type == IFNET_TYPE_TDM )
	{
		gPortInfo.iAlarmPort = IFM_E1_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);
	}
    
    //unregister alarm type: stmn. added by liub 2018-12-10
    else if ( pifm->ifm.type == IFNET_TYPE_STM )
	{
		gPortInfo.iAlarmPort = IFM_STMN_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);
	}

    /*子接口一并删除*/
    ifm_delete_sons ( pifm );

    /*删除接口*/
    ret = ifm_delete_if ( ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ifm delete ifindex 0x%0x erro\n", __FUNCTION__, __LINE__, ifindex );
        return ret;
    }
	
    return IFM_SUCCESS;

}

/*批量 删除子接口调用 */
int ifm_delete_son_if ( uint32_t ifindex )
{
    int ret = 0;
    struct hash_bucket *pbucket = NULL;
    struct ifm_entry *entry = NULL;
    struct ifm_event event;

    zlog_debug ( IFM_DBG_COMMON,"%s[%d]ifm_delete  ifindex 0x%0x \n", __FUNCTION__,__LINE__, ifindex );

    pbucket = hios_hash_find ( &ifm_table, ( void * ) ifindex );
    if ( pbucket == NULL )
    {
        zlog_err ( "%s[%d] ifm delete no find ifindex 0x%0x \n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }

    entry = ( struct ifm_entry * ) pbucket->data;
    if ( entry == NULL )
    {
        zlog_err ( "%s[%d] entry  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    /*通知接口删除事件*/
    event.event = IFNET_EVENT_IF_DELETE;
    event.ifindex = ifindex;
    event.mode = entry->ifm.mode;
    ifm_event_notify ( IFNET_EVENT_IF_DELETE, &event );

	/*下发接口删除消息到hal */
    ret = ifm_send_hal ( NULL, 0,
                         IPC_OPCODE_DELETE, IPC_OPCODE_DELETE, ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",
                   __FUNCTION__, __LINE__, ret, ifindex );
        return IFM_ERR_HAL;
    }

    /*下发接口删除消息到ftm */
    ret = ifm_send_ftm ( NULL, 0, 0, IPC_OPCODE_DELETE, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "ifm send msg to ftm  delete interface 0x%0x errocode=%d", ifindex, ret );
        return IFM_ERR_NOTIFY;
    }

    /*如果接口为为子接口更新父接口son list*/
    if ( entry->ifm.parent )
    {
        ifm_set_son ( entry->ifm.parent, entry, IFM_OPCODE_DELETE );
    }

    if ( QOS_TYPE_INVALID != entry->ifm.qos.domain_type )
    {
        ret = qos_domain_ref_operate ( entry->ifm.qos.domain_id, entry->ifm.qos.domain_type, MODULE_ID_IFM, REF_OPER_DECREASE );
        if ( ret )
        {
            zlog_err ( "%s[%d] qos_domain_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
            return ret;
        }
    }

    if ( entry->ifm.qos.phb_enable )
    {
        ret = qos_phb_ref_operate ( entry->ifm.qos.phb_id, entry->ifm.qos.phb_type, MODULE_ID_IFM, REF_OPER_DECREASE );
        if ( ret )
        {
            zlog_err ( "%s[%d] qos_phb_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
            return ret;
        }
    }


	/*插拔板卡清除接口所有告警*/

	ret = ifm_link_alarm_report ( entry, IPC_OPCODE_CLEAR );
	if ( ret != 0 )
	{
		zlog_err("%s[%d]ifm_link_alarm_report err %d\n",__FUNCTION__,__LINE__, ret );
		return ret;
	}


	/*free interface*/
    hios_hash_delete ( &ifm_table, pbucket );

    XFREE ( MTYPE_IFM_ENTRY, entry );
    XFREE ( MTYPE_HASH_BACKET, pbucket );

    return IFM_SUCCESS;
}


/*删除接口下的所有子接口*/
int ifm_delete_sons ( struct ifm_entry *pifm )
{
    struct ifm_entry *tpifm = NULL;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
    int ret = 0;
	struct ifm_reflector reflector;

	memset(&reflector, 0, sizeof(struct ifm_reflector));
    if ( pifm->ifm.parent == 0 )
    {
        if ( pifm->sonlist )
        {
            for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
            {
                tpifm = ( struct ifm_entry * ) pdata;
               /*清除子接口环回配置*/
                if ( tpifm->ifm.reflector.reflector_flag )
                {
                    ret = ifm_set_loopback ( tpifm, reflector );
                    if ( ret != 0 )
                    {
                        zlog_err ( "%s[%d] set loopback  ifindex=0x%0x 0 erro\n", __FUNCTION__, __LINE__, tpifm->ifm.ifindex );
                        //return IFM_ERR_HAL;
                    }
                }
				if(tpifm->ifm.sla_flag == 1)
				{
					tpifm->ifm.sla_flag = 0;
					is_sla_enable = 0;
				}
                if ( IFM_TYPE_IS_TDM ( pifm->ifm.ifindex ) )
                {
                    ret = ifm_delete_if ( tpifm->ifm.ifindex );
                    if ( ret < 0 )
                    {
                        zlog_err ( "%s[%d] ifm delete  ifindex=0x%0x ret %d\n", __FUNCTION__, __LINE__,
                                   tpifm->ifm.ifindex, ret );
                    }
                }
                else
                {
                    ret = ifm_delete_son_if ( tpifm->ifm.ifindex );
                    if ( ret < 0 )
                    {
                        zlog_err ( "%s[%d] ifm delete  ifindex=0x%0x ret %d\n", __FUNCTION__, __LINE__,
                                   tpifm->ifm.ifindex, ret );
                    }
                }
            }
        }
    }
    else
    {
        zlog_err ( "%s[%d]ifindex 0x%0x pifm->ifm.parent is not 0\n",
                   __FUNCTION__, __LINE__, pifm->ifm.parent );
    }

    return 0;

}


/*删除接口下的所有子接口*/
int ifm_delete_son_all ( int ifindex )
{
    struct ifm_entry *pifm = NULL;
    int ret = 0;

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        zlog_err ( "%s[%d] ifm_lookup ifindex=0x%0x alias\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_ERR_NOT_FOUND;
    }

    ret = ifm_delete_sons ( pifm );
    if ( ret < 0 )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm delete  ifindex=0x%0x sons ret %d\n", __FUNCTION__, __LINE__,
                   ifindex, ret );
    }

    return 0;

}

/*接口下添加/删除子接口*/
int ifm_set_son ( int ifindex, struct ifm_entry *psubifm, enum IFM_OPCODE opcode )
{
    struct ifm_entry *pifm = NULL;

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        zlog_err ( "%s[%d] ifm_lookup ifindex=0x%0x alias\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_ERR_NOT_FOUND;
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x %s son list 0x%0x\n",
                 __FUNCTION__, __LINE__, pifm->ifm.ifindex, opcode == IPC_OPCODE_ADD ? "add" : "delete", psubifm->ifm.ifindex );


    if ( opcode == IFM_OPCODE_ADD )
    {
        if ( pifm->sonlist == NULL )
        {
            pifm->sonlist = list_new ();
            if ( pifm->sonlist == NULL )
            {
                zlog_err ( "%s[%d] pifm  list_new null\n", __FUNCTION__, __LINE__ );
                return IFM_ERR_PARAM;
            }

            pifm->sonlist->cmp = ( int ( * ) ( void *, void * ) ) ifm_intf_cmp_func;
            listnode_add_sort ( pifm->sonlist, psubifm );
        }
        else
        {
            if ( listnode_lookup ( pifm->sonlist, psubifm ) == NULL )
            {
                listnode_add_sort ( pifm->sonlist, psubifm );
            }
            else
            {
                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x already add son list 0x%0x\n",
                             __FUNCTION__, __LINE__, pifm->ifm.ifindex, psubifm->ifm.ifindex );
                return IFM_SUCCESS;
            }
        }
    }
    else if ( opcode == IFM_OPCODE_DELETE )
    {
        if ( pifm->sonlist == NULL )
        {
            return IFM_SUCCESS;
        }
        if ( listnode_lookup ( pifm->sonlist, psubifm ) != NULL )
        {
            listnode_delete ( pifm->sonlist, psubifm );

            if ( list_isempty ( pifm->sonlist ) )
            {
                list_free ( pifm->sonlist );
                pifm->sonlist = NULL;
            }
        }
        else
        {
            return IFM_SUCCESS;
        }
    }
    else
    {
        return IFM_SUCCESS;
    }

    return IFM_SUCCESS;

}

/*配置接口shutdwon*/
int ifm_set_shutdown ( struct ifm_entry *pifm, uint8_t shutdown )
{
    struct ifm_entry *fpifm = NULL;
    struct ifm_event event;
    struct ifm_info *pold = NULL;
    int ret = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    if ( pold->type == IFNET_TYPE_INVALID )
    {
        zlog_err ( "%s[%d] ifm get ifindex=0x%0x interface type erro  type=%d \n", __FUNCTION__, __LINE__, pold->ifindex, pold->type  );
        return IFM_ERR_MISMATCH;
    }

	/*防止重复配置*/
	if ( pold->shutdown == shutdown )
	{
		return IFM_SUCCESS;
	}


    ret = ifm_send_hal ( &shutdown, sizeof ( shutdown ),
                         IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, pold->ifindex );


    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x type=%d shutdown\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
        return IFM_ERR_HAL;
    }
   ret = ifm_send_ftm( &shutdown, sizeof ( shutdown ),
		IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, pold->ifindex );
   if ( ret <0 )
   {
   	zlog_err ( "%s[%d] update ifm info err ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
   }

    pold->shutdown = shutdown;

    switch ( pold->type )
    {
        case IFNET_TYPE_TDM:
        case IFNET_TYPE_STM:
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
        case IFNET_TYPE_TRUNK:
            if ( pold->parent )
            {
                if ( shutdown == IFNET_SHUTDOWN )
                {
                    if ( pold->status == IFNET_LINKUP )
                    {
                        /*通知接口down事件*/
                        event.event = IFNET_EVENT_DOWN;
                        event.ifindex = pold->ifindex;
                        event.mode = pold->mode;
                        ifm_event_notify ( IFNET_EVENT_DOWN, &event );

                        /*上报告警*/
                        ifm_link_alarm_report ( pifm, IPC_OPCODE_ADD );

                        pold->status = IFNET_LINKDOWN;

						time(&pold->last_change);
                       /*发送接口 link status 到ftm*/
                        ret = ifm_send_ftm ( &pold->status, sizeof ( pold->status ),
                                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );
                        if ( ret < 0 )
                        {
                            zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pold->ifindex );
                        }
                        /*发送接口 link status 到hal*/
                        ret = ifm_send_hal ( &pold->status, sizeof ( pold->status ) ,
                                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );

                        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pold->ifindex );

                        if ( ret < 0 )
                        {
                            zlog_err ( "%s[%d] update ifm STATUS to hal erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
                        }

                    }
                }
                else
                {
                    fpifm = ifm_lookup ( pold->parent );
                    if ( !fpifm )
                    {
                        zlog_err ( "ifm get parent ifindex 0x%0x erro", pold->parent );
                        return IFM_ERR_NOT_FOUND;
                    }
                    if ( ( fpifm->ifm.status == IFNET_LINKUP )
                            && ( pold->status == IFNET_LINKDOWN )
                            && ( pold->encap.type != IFNET_ENCAP_INVALID ) )
                    {
                        /*通知接口up事件*/
                        event.event = IFNET_EVENT_UP;
                        event.ifindex = pold->ifindex;
                        event.mode    = pold->mode;
                        ifm_event_notify ( IFNET_EVENT_UP, &event );
                        /*清除告警*/
                        ifm_link_alarm_report ( pifm, IPC_OPCODE_CLEAR );

                        pold->status = IFNET_LINKUP;

						time(&pold->last_change);

                        /*发送接口 link status 到ftm*/
                        ret = ifm_send_ftm ( &pold->status, sizeof ( pold->status ),
                                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );
                        if ( ret < 0 )
                        {
                            zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
                        }

                         /*发送接口 link status 到hal*/
                        ret = ifm_send_hal ( &pold->status, sizeof ( pold->status ),
                                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );

                        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pold->ifindex );

                        if ( ret < 0 )
                        {
                            zlog_err ( "%s[%d] update ifm STATUS to hal erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
                        }

                    }
                }

            }
            break;
        case IFNET_TYPE_VLANIF:
            break;
        case IFNET_TYPE_TUNNEL:
            break;
        case IFNET_TYPE_LOOPBACK:
            break;
        case IFNET_TYPE_VCG:
            break;		
        case IFNET_TYPE_E1:
            break;				
        default:
            zlog_err ( "%s[%d] ifm miss match info type ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pold->ifindex, pold->type );
            return IFM_ERR_MISMATCH;
    }

    return IFM_SUCCESS;
}
/*配置接口上报link告警*/
int ifm_link_alarm_report ( struct ifm_entry *pifm, int opcode )
{
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    if ( opcode == IPC_OPCODE_ADD ) /*report*/
    {
        /*if ( pifm->ifm.status == IFNET_LINKDOWN )
        {
            zlog_debug ( IFM_DBG_COMMON,"%s[%d] :	ifindex 0x%0x set the same status down", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
            return 0;
        }*/

        /*上报告警*/
        switch ( pifm->ifm.type )
        {
            case IFNET_TYPE_TDM:
                if ( IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                {
					//ipran_alarm_report( IFM_E1_SUB_TYPE	, pifm->ifm.slot, pifm->ifm.port , pifm->ifm.sub_port, 0, GPN_ALM_TYPE_PDH_SUBIF_DOWN, GPN_SOCK_MSG_OPT_RISE );

					gPortInfo.iAlarmPort = IFM_E1_SUB_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_PDH_SUBIF_DOWN, GPN_SOCK_MSG_OPT_RISE );
                }
                else
                {                    
					//ipran_alarm_report( IFM_E1_TYPE	, pifm->ifm.slot, pifm->ifm.port , 0, 0, GPN_ALM_TYPE_PDH_LINK_DOWN, GPN_SOCK_MSG_OPT_RISE );
					
					gPortInfo.iAlarmPort = IFM_E1_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_PDH_LINK_DOWN, GPN_SOCK_MSG_OPT_RISE );
                }
                pifm->alarm = 1;
                break;
            case IFNET_TYPE_STM:
                break;
            case IFNET_TYPE_ETHERNET:
			case IFNET_TYPE_GIGABIT_ETHERNET:
			case IFNET_TYPE_XGIGABIT_ETHERNET:
				if ( IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                {
					//ipran_alarm_report( IFM_FUN_ETH_SUB_TYPE, pifm->ifm.slot, pifm->ifm.port , pifm->ifm.sub_port, 0, GPN_ALM_TYPE_FUN_ETH_SUB_DOWN, GPN_SOCK_MSG_OPT_RISE );

					gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_SUB_DOWN, GPN_SOCK_MSG_OPT_RISE );					
                }
                else
                {
					//ipran_alarm_report( IFM_FUN_ETH_TYPE, pifm->ifm.slot, pifm->ifm.port , 0, 0, GPN_ALM_TYPE_FUN_ETH_LOS, GPN_SOCK_MSG_OPT_RISE );
										
					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_LOS, GPN_SOCK_MSG_OPT_RISE );
                }
                pifm->alarm = 1;
                break;
            case IFNET_TYPE_VLANIF:
 				
                //ipran_alarm_report( IFM_VLAN_OAM_TYPE, pifm->ifm.slot, pifm->ifm.port , 0, 0, GPN_ALM_TYPE_FUN_VLANIF_DOWN, GPN_SOCK_MSG_OPT_RISE );

				gPortInfo.iAlarmPort = IFM_VLAN_OAM_TYPE;
				gPortInfo.iIfindex = pifm->ifm.ifindex;
				ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_VLANIF_DOWN, GPN_SOCK_MSG_OPT_RISE );
				
                pifm->alarm = 1;
                break;
            case IFNET_TYPE_TUNNEL:
                /*should mpls report ??*/
//              alarm_report(IPC_OPCODE_ADD, MODULE_ID_IFM, ALM_CODE_RAN_TNL_DOWN, pifm->ifm.ifindex, 0, 0, 0);
//              pifm->alarm = 1;
                break;
            case IFNET_TYPE_TRUNK:
				if( IFM_TYPE_IS_TRUNK( pifm->ifm.parent ) )
				{
					//ipran_alarm_report(IFM_TRUNK_SUBIF_TYPE,0,pifm->ifm.port,pifm->ifm.sub_port,0,GPN_ALM_TYPE_FUN_ETH_LAG_SUB_DOWN,GPN_SOCK_MSG_OPT_RISE);

					gPortInfo.iAlarmPort = IFM_TRUNK_SUBIF_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_SUB_DOWN, GPN_SOCK_MSG_OPT_RISE );
				}

				pifm->alarm = 1;
				/*should trunk report ??*/
//              alarm_report(IPC_OPCODE_ADD, MODULE_ID_IFM, ALM_CODE_LAG_DOWN , pifm->ifm.ifindex, 0, 0, 0);
//              pifm->alarm = 1;
                break;
            case IFNET_TYPE_LOOPBACK:
                break;
			case IFNET_TYPE_VCG:
				break;
			case IFNET_TYPE_E1:
				break;
            default:
                zlog_err ( "%s[%d] ifm miss match info type ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
                return IFM_ERR_MISMATCH;


        }

    }
    else/*clear*/
    {
        /*清除告警*/
        if ( pifm->alarm == 0 )
        {
            return IFM_SUCCESS;
        }

        switch ( pifm->ifm.type )
        {
            case IFNET_TYPE_TDM:
                /*上报告警*/
                if ( IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                {
                	//ipran_alarm_report( IFM_E1_SUB_TYPE	, pifm->ifm.slot, pifm->ifm.port , pifm->ifm.sub_port, 0, GPN_ALM_TYPE_PDH_SUBIF_DOWN, GPN_SOCK_MSG_OPT_CLEAN );			

					gPortInfo.iAlarmPort = IFM_E1_SUB_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_PDH_SUBIF_DOWN, GPN_SOCK_MSG_OPT_CLEAN );
                }
                else
                {
                	//ipran_alarm_report( IFM_E1_TYPE	, pifm->ifm.slot, pifm->ifm.port , 0, 0, GPN_ALM_TYPE_PDH_LINK_DOWN, GPN_SOCK_MSG_OPT_CLEAN );

					gPortInfo.iAlarmPort = IFM_E1_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_PDH_LINK_DOWN, GPN_SOCK_MSG_OPT_CLEAN );
                }
                pifm->alarm = 0;
                break;
            case IFNET_TYPE_STM:
                break;
            case IFNET_TYPE_ETHERNET:
			case IFNET_TYPE_GIGABIT_ETHERNET:
			case IFNET_TYPE_XGIGABIT_ETHERNET:
				if ( IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                {
                    
                	//ipran_alarm_report( IFM_FUN_ETH_SUB_TYPE, pifm->ifm.slot, pifm->ifm.port , pifm->ifm.sub_port, 0, GPN_ALM_TYPE_FUN_ETH_SUB_DOWN, GPN_SOCK_MSG_OPT_CLEAN );

					gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_SUB_DOWN, GPN_SOCK_MSG_OPT_CLEAN );
                }
                else
                {
                  	//ipran_alarm_report( IFM_FUN_ETH_TYPE, pifm->ifm.slot, pifm->ifm.port , 0, 0, GPN_ALM_TYPE_FUN_ETH_LOS, GPN_SOCK_MSG_OPT_CLEAN );	 

					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_LOS, GPN_SOCK_MSG_OPT_CLEAN );	 
                }
                pifm->alarm = 0;
                break;
            case IFNET_TYPE_VLANIF:
				
				//ipran_alarm_report( IFM_VLAN_OAM_TYPE, pifm->ifm.slot, pifm->ifm.port , 0, 0, GPN_ALM_TYPE_FUN_VLANIF_DOWN , GPN_SOCK_MSG_OPT_CLEAN );	           

				gPortInfo.iAlarmPort = IFM_VLAN_OAM_TYPE;
				gPortInfo.iIfindex = pifm->ifm.ifindex;
				ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_VLANIF_DOWN , GPN_SOCK_MSG_OPT_CLEAN );	           
				
                pifm->alarm = 0;
                break;
            case IFNET_TYPE_TUNNEL:
//              alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_IFM, ALM_CODE_RAN_TNL_DOWN, pifm->ifm.ifindex, 0, 0, 0);
//              pifm->alarm = 0;

                break;
            case IFNET_TYPE_TRUNK:
				if( IFM_TYPE_IS_TRUNK( pifm->ifm.parent ) )
				{
					//ipran_alarm_report(IFM_TRUNK_SUBIF_TYPE,0,pifm->ifm.port,pifm->ifm.sub_port,0,GPN_ALM_TYPE_FUN_ETH_LAG_SUB_DOWN,GPN_SOCK_MSG_OPT_CLEAN);

					gPortInfo.iAlarmPort = IFM_TRUNK_SUBIF_TYPE;
					gPortInfo.iIfindex = pifm->ifm.ifindex;
					ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_SUB_DOWN,GPN_SOCK_MSG_OPT_CLEAN);
				}

				pifm->alarm = 0;
//              /*上报告警*/
//              alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_IFM, ALM_CODE_LAG_DOWN , pifm->ifm.ifindex, 0, 0, 0);
//              pifm->alarm = 0;
                break;
            case IFNET_TYPE_LOOPBACK:
                break;
			case IFNET_TYPE_VCG:
                break;
			case IFNET_TYPE_E1:
                break;				
            default:
                zlog_err ( "%s[%d] ifm miss match info type ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
                return IFM_ERR_MISMATCH;

        }

    }

    return IFM_SUCCESS;
}

int ifm_set_sons_status ( struct ifm_entry *pifm, uint8_t status )
{
    struct listnode *node;
    int *pdata = NULL;
    int ret = 0;
    struct ifm_entry *tpifm  = NULL;
    struct ifm_event event;

    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    /*set son interface link status change and send to ftm & notice link event*/
    if ( pifm->sonlist )
    {
        for ( ALL_LIST_ELEMENTS_RO ( pifm->sonlist, node, pdata ) )
        {
            tpifm = ( struct ifm_entry * ) pdata;

            zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x shutdown %d encap.type %d\n", __FUNCTION__, __LINE__,
                         tpifm->ifm.ifindex, tpifm->ifm.shutdown, tpifm->ifm.encap.type );

            if ( tpifm->ifm.shutdown != IFNET_SHUTDOWN
                    && tpifm->ifm.encap.type != IFNET_ENCAP_INVALID )
            {
                /*notify son interface link status event*/
                if ( ( status == IFNET_LINKUP ) )
                {
                	 	event.event = IFNET_EVENT_UP;
                    	event.ifindex = tpifm->ifm.ifindex;
                    	event.mode    = tpifm->ifm.mode;
                    	ifm_event_notify ( IFNET_EVENT_UP, &event );
                   		 /*清除告警*/
                   		 ifm_link_alarm_report ( tpifm, IPC_OPCODE_CLEAR );
                    	zlog_debug (IFM_DBG_COMMON, "%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, tpifm->ifm.ifindex );
                    	tpifm->ifm.status = IFNET_LINKUP;
						time(&tpifm->ifm.last_change);
                 }
                else
                {
                	 	event.event = IFNET_EVENT_DOWN;
                   		 event.ifindex = tpifm->ifm.ifindex;
                    	event.mode    = tpifm->ifm.mode;
                    	ifm_event_notify ( IFNET_EVENT_DOWN, &event );
                   		 /*上报告警*/
                   		 ifm_link_alarm_report ( tpifm, IPC_OPCODE_ADD );
                    	zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, tpifm->ifm.ifindex );
                    	tpifm->ifm.status = IFNET_LINKDOWN;
						time(&tpifm->ifm.last_change);
                  }

                /*send son interface link status to ftm*/
                ret = ifm_send_ftm ( &tpifm->ifm.status, sizeof ( tpifm->ifm.status ),
                                     IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, tpifm->ifm.ifindex );
                if ( ret < 0 )
                {
                    zlog_err ( "%s[%d] update ifm STATUS to ftm erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, tpifm->ifm.ifindex, pifm->ifm.type );
                }
                /*send son interface link status to hal*/
                ret = ifm_send_hal ( &tpifm->ifm.status, sizeof ( tpifm->ifm.status ) ,
                                     IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, tpifm->ifm.ifindex );

                if ( ret < 0 )
                {
                    zlog_err ( "%s[%d] update ifm STATUS to hal erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, tpifm->ifm.ifindex, pifm->ifm.type );
                }
            }

        }
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x pifm->sonlist count %d\n", __FUNCTION__, __LINE__,
                     pifm->ifm.ifindex, pifm->sonlist->count );
    }
    else
    {

        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x pifm->sonlist is null\n", __FUNCTION__, __LINE__,
                     pifm->ifm.ifindex );

    }

    return IFM_SUCCESS;
}


/*更新接口link信息*/
int ifm_set_status ( struct ifm_entry *pifm, uint8_t status )
{
    struct ifm_event event;
    int ret = 0;
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    /*防止重复设置*/
    if ( pifm->ifm.status == status )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] :  ifindex 0x%0x set the same status %d", __FUNCTION__, __LINE__, pifm->ifm.ifindex, status );
        return 0;
    }


    if ( pifm->ifm.type == IFNET_TYPE_INVALID )
    {
        zlog_err ( "%s[%d] ifm get ifindex=0x%0x interface type erro  type=%d \n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
        return IFM_ERR_MISMATCH;
    }


    switch ( pifm->ifm.type )
    {
        case IFNET_TYPE_TDM:
        case IFNET_TYPE_STM:
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
            /*通知接口link状态变化事件*/
            if ( status == IFNET_LINKDOWN )
            {
                	event.event   = IFNET_EVENT_DOWN;
                	event.ifindex = pifm->ifm.ifindex;
               	 event.mode    = pifm->ifm.mode;
               	 ifm_event_notify ( IFNET_EVENT_DOWN, &event );
               	 /*上报告警*/
                	ifm_link_alarm_report ( pifm, IPC_OPCODE_ADD );
               	 zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                	pifm->ifm.status = IFNET_LINKDOWN;
					time(&pifm->ifm.last_change);

            }
            else if ( status == IFNET_LINKUP )
            {
            	event.event   = IFNET_EVENT_UP;
                	event.ifindex = pifm->ifm.ifindex;
                	event.mode    = pifm->ifm.mode;
               	 ifm_event_notify ( IFNET_EVENT_UP, &event );
                	/*清除告警*/
                	ifm_link_alarm_report ( pifm, IPC_OPCODE_CLEAR );
                	zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                	pifm->ifm.status = IFNET_LINKUP;
					time(&pifm->ifm.last_change);

                //recover flow control when link up. added by liub 2018-5-2
                ret = ifm_set_flow_control(pifm, &(pifm->ifm.flow_ctrl));
                if(ret != 0)
                {
                    zlog_err ( "%s[%d] ifm_set_flow_control error ifindex=0x%0x status=%d\n",
                                        __FUNCTION__, __LINE__, pifm->ifm.ifindex,  status );
                }
            }

           /*发送接口link状态变化到ftm*/
            ret = ifm_send_ftm ( &pifm->ifm.status, sizeof ( pifm->ifm.status ),
                                 IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x status=%d\n",
                           __FUNCTION__, __LINE__, pifm->ifm.ifindex,  status );
            }

            ifm_set_sons_status ( pifm, status );

            break;
        case IFNET_TYPE_VLANIF:
           /*通知接口link状态变化事件*/
            if ( status == IFNET_LINKDOWN )
            {
                event.event = IFNET_EVENT_DOWN;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_DOWN, &event );
                /*上报告警*/
                ifm_link_alarm_report ( pifm, IPC_OPCODE_ADD );

                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKDOWN;
				time(&pifm->ifm.last_change);
            }
            else if ( status == IFNET_LINKUP )
            {
                event.event = IFNET_EVENT_UP;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_UP, &event );
                /*清除告警*/
                ifm_link_alarm_report ( pifm, IPC_OPCODE_CLEAR );
                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKUP;
				time(&pifm->ifm.last_change);
            }

              /*发送接口link状态变化到ftm*/
            ret = ifm_send_ftm ( &pifm->ifm.status, sizeof ( pifm->ifm.status ),
                                 IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
            }

            break;
        case IFNET_TYPE_TUNNEL:
           /*通知接口link状态变化事件*/
            if ( status == IFNET_LINKDOWN )
            {
                event.event = IFNET_EVENT_DOWN;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_DOWN, &event );
                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
//              /*上报告警*/
//              ifm_link_alarm_report(pifm, IPC_OPCODE_ADD);

                pifm->ifm.status = IFNET_LINKDOWN;
			    time(&pifm->ifm.last_change);
            }
            else if ( status == IFNET_LINKUP )
            {
                event.event = IFNET_EVENT_UP;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_UP, &event );
//              /*清除告警*/
//              ifm_link_alarm_report(pifm, IPC_OPCODE_CLEAR);

                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKUP;
				time(&pifm->ifm.last_change);
            }

             /*发送接口link状态变化到ftm*/
            ret = ifm_send_ftm ( &pifm->ifm.status, sizeof ( pifm->ifm.status ),
                                 IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
            }

            break;
        case IFNET_TYPE_TRUNK:
           /*通知接口link状态变化事件*/
            if ( status == IFNET_LINKDOWN )
            {
                event.event = IFNET_EVENT_DOWN;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_DOWN, &event );
//              /*上报告警*/
//              ifm_link_alarm_report(pifm, IPC_OPCODE_ADD);

                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKDOWN;
				time(&pifm->ifm.last_change);
            }
            else if ( status == IFNET_LINKUP )
            {
                event.event = IFNET_EVENT_UP;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_UP, &event );
//              /*清除告警*/
//              ifm_link_alarm_report(pifm, IPC_OPCODE_CLEAR);
                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKUP;
				time(&pifm->ifm.last_change);
            }

            /*发送子接口接口link状态变化事件到ftm*/
            ret = ifm_send_ftm ( &pifm->ifm.status, sizeof ( pifm->ifm.status ),
                                 IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
            }

            ifm_set_sons_status ( pifm, status );

            break;
        case IFNET_TYPE_LOOPBACK:
            break;
        case IFNET_TYPE_VCG:
		case IFNET_TYPE_E1:
           /*通知接口link状态变化事件*/
            if ( status == IFNET_LINKDOWN )
            {
                event.event = IFNET_EVENT_DOWN;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_DOWN, &event );
                /*上报告警*/
                ifm_link_alarm_report ( pifm, IPC_OPCODE_ADD );

                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKDOWN;
				time(&pifm->ifm.last_change);
            }
            else if ( status == IFNET_LINKUP )
            {
                event.event = IFNET_EVENT_UP;
                event.ifindex = pifm->ifm.ifindex;
                event.mode    = pifm->ifm.mode;
                ifm_event_notify ( IFNET_EVENT_UP, &event );
                /*清除告警*/
                ifm_link_alarm_report ( pifm, IPC_OPCODE_CLEAR );
                zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
                pifm->ifm.status = IFNET_LINKUP;
				time(&pifm->ifm.last_change);
            }
            break;			
        default:
            zlog_err ( "%s[%d] ifm miss match info type ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex, pifm->ifm.type );
            return IFM_ERR_MISMATCH;
    }

    return IFM_SUCCESS;
}

int ifm_get_subif_mode( struct ifm_entry *pifm )
{
	int ret = 0;
	int mode_flag = 0;
	unsigned int devType = 0;
	uint32_t ifindex = 0;
	uint32_t pifindex = 0;
	struct ifm_entry* tpifm = NULL;
	struct listnode * node = NULL;
	struct listnode * nextnode = NULL;
	int * pdata = NULL;
	struct ifm_entry *entry = NULL;
	
	devm_comm_get_id( 1, 0, MODULE_ID_IFM, &devType );
	
	if( devType == ID_HT157 || devType == ID_HT158 )
	{
		if( ( pifm->ifm.type == IFNET_TYPE_GIGABIT_ETHERNET || pifm->ifm.type == IFNET_TYPE_XGIGABIT_ETHERNET )
				&& ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT ) )
		{
			ifindex = pifm->ifm.ifindex;
			pifindex = IFM_PARENT_IFINDEX_GET(ifindex);
			tpifm = ifm_lookup( pifindex );
			if( tpifm != NULL )
			{
				if( tpifm->ifm.parent == 0 )
				{
					if( tpifm->sonlist )
					{
						for( ALL_LIST_ELEMENTS( tpifm->sonlist, node, nextnode, pdata))
						{
							entry = ( struct ifm_entry* )pdata;
							if( entry->ifm.mode == IFNET_MODE_L3)
							{
								mode_flag = 1;
								break;
							}
							else
							{
								continue;
							}
						}
					}
				}
			}
		}
	}
	if( mode_flag == 1 )
	{
		//ret = ipc_send_hal(&mode_flag, sizeof(mode_flag), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_ACL, QOS_INFO_SUBIF_MODE, 1, pifm->ifm.ifindex);
		
		ret = ipc_send_msg_n2(&mode_flag, sizeof(mode_flag), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_ACL, QOS_INFO_SUBIF_MODE, 1, pifm->ifm.ifindex);

		if( ret != 0 )
		{
			zlog_err("%s[d] ifm_send_hal subid_mode err ret %d\n",__FUNCTION__,__LINE__,ret);
			return ret ;
		}
	}
	return ret ;
	
}


/*配置接口mode*/
int ifm_set_mode ( struct ifm_entry *pifm, uint8_t mode )
{
    struct ifm_info *pold = NULL;
    struct ifm_event event;
    enum QOS_TYPE type = QOS_TYPE_INVALID;
    uchar    mac[6];
    int ret = 0;
    int size = 0;
	struct ifm_reflector reflector;
	struct ifm_loopback loop_info;
    struct ifm_flow_control flow_ctrl;

	memset(&reflector, 0, sizeof(struct ifm_reflector));
	memset(&loop_info, 0, sizeof(struct ifm_loopback));
    memset(&flow_ctrl, 0, sizeof(struct ifm_flow_control));
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
	#if 0
	ret = ifm_get_subif_mode( pifm );
	if( ret != 0 )
	{
		zlog_err("%s[%d]ifm_get_subif_mode err ifindex 0x%0x ret %d\n",
			__FUNCTION__,__LINE__,pifm->ifm.ifindex,ret);
	}
	#endif
	

    pold = & ( pifm->ifm );

    /*防止重复设置*/
    if ( pold->mode == mode )
    {
        return IFM_SUCCESS;
    }
    size = sizeof ( pold->mode );

     /*删除子接口*/
    if ( pifm->ifm.parent == 0 && mode != IFNET_MODE_L3 )
    {
        if ( pifm->sonlist )
        {
            ifm_delete_sons ( pifm );
        }
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pifm->ifm.mode %d  mode %d\n", __FUNCTION__, __LINE__, pifm->ifm.mode,  mode );
    /*清除端口下环回配置*/
    if ( pifm->ifm.reflector.reflector_flag )
    {
        ret = ifm_set_loopback ( pifm, reflector );
        if ( ret != 0 )
        {
            zlog_err ( "%s[%d] set loopback  ifindex=0x%0x 0 erro\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
            //return IFM_ERR_HAL;
        }
    }
	/*清除端口下vlan mac ip 设置*/
	if( pifm->ifm.loop_info.reflector.reflector_flag )
	{
		ret = ifm_set_loopback_info(pifm, &loop_info);
		if ( ret != 0 )
		{
			zlog_err("%s[%d] set loopback info ifindex 0x%0x erro\n",__FUNCTION__,__LINE__, pifm->ifm.ifindex );

		}
	}

	/* 清除接口的sla */
	if(pifm->ifm.sla_flag)
	{
		pifm->ifm.sla_flag = 0;
		is_sla_enable = 0;
	}

    //clear flow control. added by liub 2018-5-2
    if(pifm->ifm.flow_ctrl.status)
    {
        ret = ifm_set_flow_control(pifm, &flow_ctrl);
        if ( ret != 0 )
		{
			zlog_err("%s[%d] set flow control info error, ret=%d\n",__FUNCTION__,__LINE__, ret);
		}
    }

    if ( mode != IFNET_MODE_INVALID )
    {
        /*将配置mode信息下发给hal*/
        ret = ifm_send_hal ( &mode, size,
                             IFNET_INFO_MODE, IPC_OPCODE_UPDATE, pold->ifindex );

        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x  mode\n", __FUNCTION__, __LINE__, pold->ifindex );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mode\n", __FUNCTION__, __LINE__, pold->ifindex  );
            return IFM_ERR_HAL;
        }
    }

    /* unset qos domain */
    if ( QOS_TYPE_INVALID != pifm->ifm.qos.domain_type )
    {
        ret = qos_domain_ref_operate ( pifm->ifm.qos.domain_id, pifm->ifm.qos.domain_type, MODULE_ID_IFM, REF_OPER_DECREASE );
        if ( ret )
        {
            zlog_err ( "%s[%d] qos_domain_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
            return ret;
        }
    }
    QOS_DOMAIN_TYPE_GET(mode, type);
    pifm->ifm.qos.domain_type = type;
    pifm->ifm.qos.domain_id   = 0;

    /* unset qos phb */
    if ( QOS_TYPE_INVALID != pifm->ifm.qos.phb_type )
    {
        ret = qos_phb_ref_operate ( pifm->ifm.qos.phb_id, pifm->ifm.qos.phb_type, MODULE_ID_IFM, REF_OPER_DECREASE );
        if ( ret )
        {
            zlog_err ( "%s[%d] qos_phb_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
            return ret;
        }
    }
    if( IFM_TYPE_IS_SUBPORT( pifm->ifm.ifindex )
	&& ( IFNET_MODE_L3 == mode ))
    {
		pifm->ifm.qos.phb_enable = QOS_PHB_ENABLE;
	    pifm->ifm.qos.phb_type   = QOS_TYPE_COS;
	    pifm->ifm.qos.phb_id     = 0;
    }
    else
    {
    	pifm->ifm.qos.phb_enable = QOS_PHB_DISABLE;
	    pifm->ifm.qos.phb_type   = QOS_TYPE_INVALID;
	    pifm->ifm.qos.phb_id     = 0;
    }

    /* unset qos priority */
    pifm->ifm.qos.untag_pri = DEFAULT_UNTAG_PRI;

    /*保存配置的mode信息*/
    pold->mode       =  mode;

   /*改变模式后封装信息被清除，封装vlan时基于mode的*/
    pold->encap.type =  IFNET_ENCAP_INVALID;

    /*通知接口模式改变事件*/
    event.event    = IFNET_EVENT_MODE_CHANGE;
    event.mode     = mode;
    event.ifindex  = pold->ifindex;
    event.up_flag = pold->status;
    ifm_event_notify ( IFNET_EVENT_MODE_CHANGE, &event );

   /*更新子接口link状态*/

    if ( IFM_TYPE_IS_SUBPORT ( pifm->ifm.ifindex )  && ( pifm->ifm.status == IFNET_LINKUP ) )
    {
         /*通知子接口link状态事件*/
        event.event = IFNET_EVENT_DOWN;
        event.ifindex = pifm->ifm.ifindex;
        event.mode    = pifm->ifm.mode;
        ifm_event_notify ( IFNET_EVENT_DOWN, &event );
		ifm_link_alarm_report ( pifm, IPC_OPCODE_ADD );
        zlog_debug (IFM_DBG_COMMON, "%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
        pifm->ifm.status = IFNET_LINKDOWN;
		time(&pifm->ifm.last_change);

        /*发送子接口link状态到ftm*/
        ret = ifm_send_ftm ( &pifm->ifm.status, sizeof ( pifm->ifm.status ),
                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
        }
        /*发送子接口link状态到hal*/
        ret = ifm_send_hal ( &pifm->ifm.status, sizeof ( pifm->ifm.status ),
                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );

        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] update ifm STATUS to hal erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
        }


    }

    /*发送接口link状态到ftm*/
    ret = ifm_send_ftm ( &mode, size,
                         IFNET_INFO_MODE, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x mode\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mode\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
    }
   /*对于 switch 和无效模式的物理接口，全部用设备 MAC（即背板 MAC）*/
    if ( ( pifm->ifm.parent == 0 || IFM_TYPE_IS_TRUNK(pifm->ifm.parent))
			&& (( pifm->ifm.type == IFNET_TYPE_ETHERNET )||( pifm->ifm.type == IFNET_TYPE_GIGABIT_ETHERNET )
			||( pifm->ifm.type == IFNET_TYPE_XGIGABIT_ETHERNET )))
    {
    	
    	#if 0
        memset ( mac, 0, MAC_LEN );
        if ( mode == IFNET_MODE_INVALID || mode == IFNET_MODE_SWITCH )
        {
            /* 从devm获取设备MAC地址 */
            ret = devm_comm_get_mac ( 1, 0, MODULE_ID_IFM , mac );
            if ( ret )
            {
                zlog_err ( "%s[%d] fail to get mac from devm", __FUNCTION__, __LINE__ );
                return ret;
            }
        }
        else
        {


            /* 从devm获取设备业务子卡MAC地址 */
            ret = devm_comm_get_mac ( pifm->ifm.unit, pifm->ifm.slot, MODULE_ID_IFM , mac );

           zlog_debug (IFM_DBG_COMMON, "%s[%d] Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x \n",
		   __FUNCTION__, __LINE__,
                      pifm->ifm.mac[0], pifm->ifm.mac[1],
                      pifm->ifm.mac[2], pifm->ifm.mac[3],
                      pifm->ifm.mac[4], pifm->ifm.mac[5]);

            if ( ret )
            {
                zlog_err ( "%s[%d] fail to get mac from devm", __FUNCTION__, __LINE__ );
                return ret;
            }
        }
		#endif
        /*配置接口 MAC地址*/
        ret = ifm_set_mac ( pifm,  pifm->ifm.mac );
        if ( ret )
        {
            zlog_err ( "%s[%d] fail to set mac", __FUNCTION__, __LINE__ );
            return ret;
        }
    }

    return IFM_SUCCESS;

}
/*配置接口封装,子接口必须封装vlan才能up*/
int ifm_set_encap ( struct ifm_entry *pifm, struct ifm_encap *encap )
{
    struct ifm_entry *tpifm = NULL;
    struct ifm_info *pold = NULL;
    struct ifm_event event;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    size = sizeof ( struct ifm_encap );

    /*防止重复设置*/
    if ( !memcmp ( &pold->encap, encap, size ) )
    {
        return IFM_SUCCESS;
    }
    if ( encap->type != IFNET_ENCAP_INVALID && pifm->ifm.encap.type != IFNET_ENCAP_INVALID )
    {
        zlog_err ( "%s[%d] ifindex 0x%0x The encapsulate vlan had been setted, please unset the old encapsulate vlan first\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_ENCAP_EXIST;
    }
    tpifm = ifm_lookup ( pold->parent );
    if ( !tpifm )
    {
        zlog_err ( "%s[%d] ifm_lookup ifindex=0x%0x encap\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOT_FOUND;
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pold->mode =%d, pold->parent = 0x%0x\n", __FUNCTION__, __LINE__,
                 pold->mode, pold->parent );

    /*检查封装vlan是否合法(相同接口下子接口的封装vlan不能重叠)*/
    ret = ifm_checkout_encap ( tpifm, pifm, encap );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ifm_checkout_encap err %d\n", __FUNCTION__, __LINE__, ret  );
        return ret;
    }

    /*发送配置封装信息给hal并等待应答*/
    /*ret = ifm_send_hal_wait_ack ( encap, size, IFNET_INFO_ENCAP, IPC_OPCODE_UPDATE, pold->ifindex );
	ret = ifm_send_hal_wait_ack(encap, size, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM,
			IFNET_INFO_ENCAP, IPC_OPCODE_UPDATE, pold->ifindex);*/
			
    ret = ifm_send_hal(encap, size, IFNET_INFO_ENCAP, IPC_OPCODE_UPDATE, pold->ifindex);

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x encap\n", __FUNCTION__, __LINE__, pold->ifindex  );
        return IFM_ERR_HAL;
    }

    /*save info*/
    memcpy ( &pold->encap, encap, sizeof ( struct ifm_encap ) );

    /*发送配置封装信息给ftm*/
    ret = ifm_send_ftm ( encap, size,
                         IFNET_INFO_ENCAP, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x encap\n", __FUNCTION__, __LINE__, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x encap\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }

    /*如果子接口解封装需要更新接口状态*/
    if ( encap->type == IFNET_ENCAP_INVALID )
    {
        if ( pold->status  == IFNET_LINKUP )
        {
            /*通知子接口状态变化事件*/
            event.event = IFNET_EVENT_DOWN;
            event.ifindex = pold->ifindex;
            event.mode = pold->mode;
            ifm_event_notify ( IFNET_EVENT_DOWN, &event );

	      /*上报接口down告警*/
	    ifm_link_alarm_report ( pifm, IPC_OPCODE_ADD );
	    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );

	    pold->status = IFNET_LINKDOWN;

		time(&pifm->ifm.last_change);
            /*send link status to ftm*/
            ret = ifm_send_ftm ( &pold->status, sizeof ( pold->status ),
                                 IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pold->ifindex );
            }
            /*send son interface link status to hal*/
            ret = ifm_send_hal ( &pold->status, sizeof ( pold->status ) ,
                                 IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );

            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm STATUS to hal erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pold->ifindex );
            }
        }

    }
    else if ( ( tpifm->ifm.status == IFNET_LINKUP )
              && ( ( pold->shutdown == IFNET_NO_SHUTDOWN ) && ( pold->status == IFNET_LINKDOWN ) ) )
    {
        /*通知子接口状态变化事件*/
        event.event = IFNET_EVENT_UP;
        event.ifindex = pold->ifindex;
        event.mode    = pold->mode;
        ifm_event_notify ( IFNET_EVENT_UP, &event );
        /*清除告警*/
        ifm_link_alarm_report ( pifm, IPC_OPCODE_CLEAR );
        pold->status = IFNET_LINKUP;
		time(&pold->last_change);

        /*发送子接口状态变化到ftm*/
        ret = ifm_send_ftm ( &pold->status, sizeof ( pold->status ),
                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] update ifm STATUS erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pold->ifindex );
        }
       /*发送子接口状态变化到hal*/
        ret = ifm_send_hal ( &pold->status, sizeof ( pold->status ) ,
                             IFNET_INFO_STATUS, IPC_OPCODE_UPDATE, pold->ifindex );

        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pold->ifindex );

        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] update ifm STATUS to hal erro ifindex=0x%0x\n", __FUNCTION__, __LINE__, pold->ifindex );
        }

    }

    return IFM_SUCCESS;

}
/*配置接口up 震荡间隔*/
int ifm_set_flap ( struct ifm_entry *pifm,  uint8_t  flap_period )
{
    struct ifm_port *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->port_info );

    /*防止重复设置*/
    if ( pold->flap_period == flap_period )
    {
        return IFM_SUCCESS;
    }

    size = sizeof ( pold->flap_period );

    /*下发接口震荡抑制间隔给hal*/
    ret = ifm_send_hal ( &flap_period, size,
                         IFNET_INFO_FLAP, IPC_OPCODE_UPDATE, pifm->ifm.ifindex  );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x flap\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex  );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
    pold->flap_period = flap_period;

    return IFM_SUCCESS;

}
/*配置接口down 震荡间隔*/
int ifm_set_down_flap ( struct ifm_entry *pifm,  uint8_t  flap_period )
{
    struct ifm_port *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->port_info );
    /*checkout if had config the same vaule*/
    if ( pold->down_flap_period == flap_period )
    {
        return IFM_SUCCESS;
    }

    size = sizeof ( pold->down_flap_period );

    /*send info to hal no wait ack*/
    /*ret = ipc_send_hal ( &flap_period, size, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM,
                         IFNET_INFO_DOWN_FLAP, IPC_OPCODE_UPDATE, pifm->ifm.ifindex  );*/
   //ret = ifm_ipc_send(&flap_period, size, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM, 
   //   		IFNET_INFO_DOWN_FLAP, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);
    ret = ipc_send_msg_n2(&flap_period, size, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM, IFNET_INFO_DOWN_FLAP, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x flap\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex  );
        return IFM_ERR_HAL;
    }

    /*save info*/
    pold->down_flap_period = flap_period;

    return IFM_SUCCESS;

}
/*配置接口speed信息*/
int ifm_set_speed ( struct ifm_entry *pifm,  struct ifm_port *speed )
{
    struct ifm_info *pold = NULL;
    struct ifm_port *pport;
    int ret = 0;
    int change = 1;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    pport = & ( pifm->port_info );


    zlog_debug ( IFM_DBG_COMMON,"%s[%d] old autoneg %d speed %d duplex %d\n", __FUNCTION__, __LINE__,
                 pport->autoneg,
                 pport->speed,
                 pport->duplex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] set autoneg %d speed %d duplex %d\n", __FUNCTION__, __LINE__,
                 speed->autoneg,
                 speed->speed,
                 speed->duplex );

    /*接口自协商状态不允许配置speed*/
    if ( speed->speed != IFNET_SPEED_INVALID
            && pport->autoneg != IFNET_SPEED_MODE_FORCE )
    {
        return IFM_ERR_PARAM_SPEED_AUTO;
    }
	/*电口不允许配置强制1000M*/
	if( (pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE)&&
		(pifm->port_info.fiber == IFNET_FIBERTYPE_COPPER || pifm->port_info.fiber_sub == IFNET_FIBERTYPE_COPPER	)&&
		(speed->speed == IFNET_SPEED_GE))
	{
		return IFM_ERR_PARAM_SPEED_GE;
	}
    /*防止重复设置*/
    if ( speed->speed != IFNET_SPEED_INVALID )
    {
        if ( pport->speed != speed->speed )
        {
            change = 0;
        }
    }
    if ( speed->duplex != IFNET_DUPLEX_INVALID )
    {
        if ( pport->duplex != speed->duplex )
        {
            change = 0;
        }

    }
    if ( speed->autoneg != IFNET_SPEED_MODE_INVALID )
    {
        if ( pport->autoneg != speed->autoneg )
        {
            change = 0;
        }

    }
    if ( change )
    {
        // return IFM_SUCCESS;
    }

	/*下发配置信息给hal并等待应答*/
    /*ret = ifm_send_hal_wait_ack ( speed, sizeof ( struct ifm_port ), IFNET_INFO_SPEED, IPC_OPCODE_UPDATE, pold->ifindex );
	ret = ifm_send_hal_wait_ack( speed, sizeof ( struct ifm_port ), 1, MODULE_ID_HAL, MODULE_ID_IFM,
			IPC_TYPE_IFM,  IFNET_INFO_SPEED, IPC_OPCODE_UPDATE,pold->ifindex);*/
    
    ret = ifm_send_hal(speed, sizeof ( struct ifm_port ), IFNET_INFO_SPEED, IPC_OPCODE_UPDATE, pold->ifindex);
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x speed\n", __FUNCTION__, __LINE__, pold->ifindex );
        return ret;
    }

    /*保存配置信息*/
    if ( speed->speed != IFNET_SPEED_INVALID )
    {
        pport->speed = speed->speed;
    }
    if ( speed->duplex != IFNET_DUPLEX_INVALID )
    {
        pport->duplex = speed->duplex;
        pport->speed  = IFNET_SPEED_INVALID;
    }
    if ( speed->autoneg != IFNET_SPEED_MODE_INVALID )
    {
        pport->autoneg = speed->autoneg;
        pport->speed  = IFNET_SPEED_INVALID;
    }

    return IFM_SUCCESS;
}

/*配置接口统计*/
int ifm_set_statistics ( struct ifm_entry *pifm,  uint8_t  statistics )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex 0x%0x , old statistics %s statistics %s\n",
                 __FUNCTION__, __LINE__, pifm->ifm.ifindex,
                 pold->statistics == IFNET_STAT_CLEAR ? "clear" :
                 ( pold->statistics == IFNET_STAT_ENABLE  ? "enable" : "disable" ),
                 statistics == IFNET_STAT_CLEAR ? "clear" :
                 ( statistics == IFNET_STAT_ENABLE  ? "enable" : "disable" ) );
	if( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT )
	{
		if( statistics == IFNET_STAT_ENABLE )
		{
			if( pifm->pcounter == NULL )
			{
				pifm->pcounter = (struct ifm_counter *)XMALLOC(MTYPE_IFM_ENTRY, sizeof ( struct ifm_counter ));
				if ( NULL ==  pifm->pcounter )
                {
                    zlog_err ( "%s[%d] ifm xmalloc ifm_counter  erro", __FUNCTION__, __LINE__ );
                    XFREE ( MTYPE_IFM_ENTRY,  pifm);				
                    return -1;
                }
			}
			memset ( pifm->pcounter , 0, sizeof ( struct ifm_counter ) );
		}
		else if( statistics == IFNET_STAT_DISABLE)
		{
			if(pifm->pcounter != NULL)
			{
				XFREE ( MTYPE_IFM_ENTRY,  pifm->pcounter );
				pifm->pcounter = NULL;
			}
		}
	}

    /*防止重复设置*/
    if ( pold->statistics == statistics )
    {
        return IFM_SUCCESS;
    }

    if ( (IFM_TYPE_IS_LOOPBCK(pold->ifindex)) && (pold->sub_port != 0) )
    {
        return IFM_ERR_PARAM_NOSUPPORT;
    }

    size = sizeof ( pold->statistics );

    ret = ifm_send_hal ( &statistics, size,
                         IFNET_INFO_STATISTICS, IPC_OPCODE_UPDATE, pold->ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x statistics\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息(清除计数不用保存)*/
    if ( statistics != IFNET_STAT_CLEAR )
    {
        pold->statistics = statistics;
    }

    /*下发配置信息给ftm*/
    ret = ifm_send_ftm ( &statistics, size,
                         IFNET_INFO_STATISTICS, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x statistics\n", __FUNCTION__, __LINE__, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x statistics\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }

    return IFM_SUCCESS;

}
/*配置接口tpid*/
int ifm_set_tpid ( struct ifm_entry *pifm,  uint16_t  tpid )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    /*防止重复设置*/
    if ( pold->tpid == tpid )
    {
        return IFM_SUCCESS;
    }

    size = sizeof ( pold->tpid );

    /*下发配置信息给hal*/

    ret = ifm_send_hal ( &tpid, size,
                         IFNET_INFO_TPID, IPC_OPCODE_UPDATE, pold->ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x tpid\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
    pold->tpid = tpid;

    /*发送配置信息给ftm*/
    ret = ifm_send_ftm ( &tpid, size,
                         IFNET_INFO_TPID, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x tpid\n", __FUNCTION__, __LINE__, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x tpid\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }

    return IFM_SUCCESS;

}

int ifm_set_phymaster ( struct ifm_entry *pifm,  uint32_t  ms )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    /*防止重复设置*/
    if ( pold->phymaster == ms )
    {
        return IFM_SUCCESS;
    }

    size = sizeof ( pold->phymaster );

    ret = ifm_send_hal ( &ms, size,
                         IFNET_INFO_PHYMASTER, IPC_OPCODE_UPDATE, pold->ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x tpid\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
    pold->phymaster = ms;

    /*发送配置信息给ftm*/
    ret = ifm_send_ftm ( &ms, size,
                         IFNET_INFO_PHYMASTER, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( "%s[%d] err ifindex=0x%0x \n", __FUNCTION__, __LINE__, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x tpid\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }

//    printf("%s: if %x, ms = %d \n", __func__, pold->ifindex, ms);
    return IFM_SUCCESS;

}

/*配置接口mtu*/
int ifm_set_mtu ( struct ifm_entry *pifm,  uint16_t  mtu )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;
    struct ifm_event event;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
    pold = & ( pifm->ifm );
    /*防止重复设置*/
    if ( pold->mtu == mtu )
    {
        return IFM_SUCCESS;
    }

    size = sizeof ( pold->mtu );

    /*下发配置信息给hal*/

    ret = ifm_send_hal ( &mtu, size,
                         IFNET_INFO_MTU, IPC_OPCODE_UPDATE, pold->ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mtu\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
    pold->mtu = mtu;

    /*通知 mtu 变化事件*/
    event.event    = IFNET_EVENT_MTU_CHANGE;
    event.mtu      = mtu;
    event.ifindex  = pold->ifindex;
    event.mode     = pold->mode;
    ifm_event_notify ( IFNET_EVENT_MTU_CHANGE, &event );


   /*发送配置信息给ftm*/
    ret = ifm_send_ftm ( &mtu, size,
                         IFNET_INFO_MTU, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x mtu %d\n", __FUNCTION__, __LINE__, pold->ifindex, mtu );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mtu\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }
    return IFM_SUCCESS;

}
/*配置接口jumbo*/
int ifm_set_jumbo ( struct ifm_entry *pifm,  uint16_t  jumbo )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    /*防止重复设置*/
    if ( pold->jumbo == jumbo )
    {
        return IFM_SUCCESS;
    }

    size = sizeof ( pold->jumbo );
    /*下发配置信息给hal*/

    ret = ifm_send_hal ( &jumbo, size,
                         IFNET_INFO_JUMBO, IPC_OPCODE_UPDATE, pold->ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mtu\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
    pold->jumbo = jumbo;

    /*发送配置信息给ftm*/
    ret = ifm_send_ftm ( &jumbo, size,
                         IFNET_INFO_JUMBO, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x jumbo\n", __FUNCTION__, __LINE__, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x jumbo\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }
    return IFM_SUCCESS;

}

/*配置接口 MAC地址*/
int ifm_set_mac ( struct ifm_entry *pifm,  uchar *mac )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pold = & ( pifm->ifm );
    size = sizeof ( pold->mac );
    /*防止重复设置*/
    if ( !memcmp ( &pold->mac, mac, size ) )
    {
        return IFM_SUCCESS;
    }

    /*检查MAC地址是否合法*/
    ret = ifm_checkout_mac ( mac );
    if ( ret < 0 )
    {
        return ret;
    }
    /*下发配置信息给hal*/

    ret = ifm_send_hal ( mac, size,
                         IFNET_INFO_MAC, IPC_OPCODE_UPDATE, pold->ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mac\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
    memcpy ( &pold->mac, mac, size );

    /*发送配置信息给ftm*/
    ret = ifm_send_ftm ( mac, size,
                         IFNET_INFO_MAC, IPC_OPCODE_UPDATE, pold->ifindex );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x mac\n", __FUNCTION__, __LINE__, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mac\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_NOTIFY;
    }
    return IFM_SUCCESS;

}
/*配置接口 环回*/
int ifm_set_loopback ( struct ifm_entry *pifm,  struct ifm_reflector reflector)
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;

    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
     if ( ( reflector.reflector_flag !=0 ) && ( pifm->ifm.loop_info.reflector.reflector_flag !=0) )
    {
    	printf("reflector has been seted\n");

		zlog_debug(IFM_DBG_COMMON,"%s[%d]reflector_flag %d  loop_flag %d\n",__FUNCTION__,__LINE__,
			reflector.reflector_flag,pifm->ifm.loop_info.reflector.reflector_flag );

		return IFM_ERR_PARAM;
    }
    /*不支持l3模式下的环回*/

    if (  pifm->ifm.mode == IFNET_MODE_L3 )
    {
        return IFM_ERR_PARAM_NOSUPPORT;
    }


    pold = & ( pifm->ifm );
    size = sizeof ( pold->reflector );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pold->lb_flags %d , lb_flags %d set_flag1 %d set_flag2 %d\n",
		__FUNCTION__, __LINE__, pold->reflector.reflector_flag,
		reflector.reflector_flag,pifm->ifm.reflector.set_flag, reflector.set_flag);

    /*防止重复设置*/
    if ( pold->reflector.reflector_flag  == reflector.reflector_flag )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] old flags %d new flags %d\n", __FUNCTION__, __LINE__,
                     pold->reflector.reflector_flag, reflector.reflector_flag );
        return ERRNO_SUCCESS;
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pold->lb_flags %d , lb_flags %d\n", __FUNCTION__, __LINE__, pold->reflector.reflector_flag, reflector.reflector_flag );

    /*ret = ifm_send_hal_wait_ack ( &reflector, size, IFNET_INFO_LOOPBACK,
                                  IPC_OPCODE_UPDATE, pold->ifindex );
    ret = ifm_send_hal_wait_ack(&reflector, size, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM,
    		IFNET_INFO_LOOPBACK, IPC_OPCODE_UPDATE, pold->ifindex);*/

    ret = ifm_send_hal(&reflector, size, IFNET_INFO_LOOPBACK, IPC_OPCODE_UPDATE, pold->ifindex);

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x ret %d\n", __FUNCTION__, __LINE__, pold->ifindex, ret );
        return ret;
    }

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x lb_flags\n", __FUNCTION__, __LINE__, pold->ifindex );
        return IFM_ERR_HAL;
    }

    /*保存配置信息*/
	memcpy(&pold->reflector, &reflector, sizeof(struct ifm_reflector));
	if(reflector.reflector_flag == 0 )
	{
		pold->reflector.set_flag = 0;
	}

    return IFM_SUCCESS;

}

/*配置接口 别名*/
int ifm_set_alias ( struct ifm_entry *pifm,  char *alias )
{
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    memcpy ( pifm->alias, alias, strlen ( alias ) + 1 );

    return IFM_SUCCESS;

}

/*此以太物理接口加入trunk*/
int ifm_set_trunk ( uint32_t ifindex, uint16_t trunk_id,  int opcode )
{
    int ret = 0;
    uint8_t mode = IFNET_MODE_L3;
    struct ifm_entry *pifm = NULL;

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        zlog_err ( "%s[%d] ifm_lookup ifindex=0x%0x erro\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_ERR_NOT_FOUND;
    }

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pifm->ifm.mode)
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x not support trunk\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
        return IFM_ERR_PARAM_NOSUPPORT;
    }
    // 2018-05-04, end

    if ( opcode == IFM_OPCODE_ADD )
    {
        mode = IFNET_MODE_INVALID;
    }
    else if ( opcode == IFM_OPCODE_DELETE )
    {
        mode = IFNET_MODE_L3;
    }
    /*加入trunk口后此接口mode变为无效，从trunk口中移除后变为l3模式*/
    ret = ifm_set_mode ( pifm, mode );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x opcode=%d\n", __FUNCTION__, __LINE__, ifindex, opcode );
        return ret;
    }

   /*更新trunkid和接口的父接口信息*/
    if ( opcode == IFM_OPCODE_ADD )
    {
        pifm->ifm.trunkid = trunk_id;
        pifm->ifm.parent  = IFM_TRUNK_IFINDEX_GET ( trunk_id );
    }
    else  if ( opcode == IFM_OPCODE_DELETE )
    {
        pifm->ifm.trunkid = 0;
        pifm->ifm.parent  = 0;
    }

    ret = ifm_send_hal ( & ( pifm->ifm.parent ), sizeof ( pifm->ifm.parent ),
                         IFNET_INFO_PARENT, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",
                   __FUNCTION__, __LINE__, ret, pifm->ifm.ifindex );
        return ret;
    }

    /*更新接口的父接口信息到ftm*/
    ret = ifm_send_ftm (  & ( pifm->ifm.parent ), sizeof ( pifm->ifm.parent ),
                          IFNET_INFO_PARENT, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );

    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x mac\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );
        return IFM_ERR_NOTIFY;
    }

    return ret;

}

int ifm_set_dcn (u_char enable )
{
    int ret = 0;
	zlog_debug ( IFM_DBG_COMMON,"[%s %d] dcn enable var is %d", __FUNCTION__, __LINE__, enable );


    dcn_flag = enable;


    return ret;

}
/*更新接口子类型*/
int ifm_set_subtype ( struct ifm_entry *pifm,  int subtype  )
{
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }


    pifm->ifm.sub_type = subtype;

    return IFM_SUCCESS;

}
int ifm_set_sfp_off ( struct ifm_entry *pifm ,  uint32_t sfp_off )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null \n ", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
    pold = & ( pifm->ifm );
    if ( pold->sfp_off == sfp_off )
    {
        return IFM_SUCCESS;
    }
    size = sizeof ( pold->sfp_off );
    ret = ifm_send_hal ( & sfp_off,  size,  IFNET_INFO_SFP_OFF,  IPC_OPCODE_UPDATE,  pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",  __FUNCTION__, __LINE__, ret, pold->ifindex );
        return ret;
    }
    pold->sfp_off = sfp_off;
    return IFM_SUCCESS;
}
int ifm_set_sfp_on ( struct ifm_entry *pifm, uint32_t  sfp_on )
{
    struct ifm_info *pold = NULL;
    int ret = 0;
    int size = 0;
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null \n ", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
    pold = & ( pifm->ifm );
    if ( pold->sfp_on == sfp_on )
    {
        return IFM_SUCCESS;
    }
    size = sizeof ( pold->sfp_on );
    ret = ifm_send_hal ( & sfp_on, size, IFNET_INFO_SFP_ON, IPC_OPCODE_UPDATE, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",  __FUNCTION__, __LINE__, ret, pold->ifindex );
        return ret;
    }
    pold->sfp_on = sfp_on;
    return IFM_SUCCESS;

}
int ifm_set_sfp_als ( struct ifm_entry *pifm, uint8_t sfp_als )
{
    struct ifm_info *pold = NULL;
    int ret = 0 ;
    int size = 0 ;
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
    pold = & ( pifm->ifm );
    size = sizeof ( pold->sfp_als );
    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pold->sfp_als %d, sfp_flags %d", __FUNCTION__, __LINE__, pold->sfp_als, sfp_als );
    ret = ifm_send_hal ( & sfp_als, size, IFNET_INFO_SFP_ALS,  IPC_OPCODE_UPDATE, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",  __FUNCTION__, __LINE__, ret, pold->ifindex );
        return ret;
    }
    pold->sfp_als = sfp_als ;
    return IFM_SUCCESS;
}
int ifm_set_sfp_tx ( struct ifm_entry *pifm, uint8_t sfp_tx )
{
    struct ifm_info *pold;
    int ret = 0 ;
    int size = 0 ;
    if ( !pifm )
    {
        zlog_err ( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
    pold = & ( pifm->ifm );
    size = sizeof ( pold->sfp_tx );
    ret = ifm_send_hal ( &sfp_tx, size, IFNET_INFO_SFP_TX, IPC_OPCODE_UPDATE, pold->ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",  __FUNCTION__, __LINE__, ret, pold->ifindex );
        return ret;
    }
    pold->sfp_tx = sfp_tx ;
    return IFM_SUCCESS;
}
int ifm_set_loopback_info( struct ifm_entry*pifm, struct ifm_loopback * loop_info )
{
	struct ifm_info*pold = NULL;
	int ret = 0;
	int size = 0;
	if ( pifm == NULL ||loop_info ==NULL )
	{
		zlog_err( "%s[%d] pifm or loop_info is null\n", __FUNCTION__, __LINE__ );
		return IFM_ERR_PARAM;
	}
	if ( ( ( pifm->ifm.reflector.reflector_flag != 0 ) || ( pifm->ifm.loop_info.reflector.reflector_flag != 0))
		&& ( loop_info->reflector.reflector_flag != 0 ) )

 	{
		printf("reflector has been seted\n");

		zlog_debug(IFM_DBG_COMMON,"%s[%d]reflector_flag %d  pifm_loop_flag %dloop_flag %d\n",__FUNCTION__,__LINE__,
			pifm->ifm.reflector.reflector_flag,pifm->ifm.loop_info.reflector.reflector_flag,
			loop_info->reflector.reflector_flag );

		return IFM_ERR_PARAM;
    	}
	if ( pifm->ifm.mode != IFNET_MODE_SWITCH )
	{
		 return IFM_ERR_PARAM_NOSUPPORT;
	}
	pold = &( pifm->ifm );
	size = sizeof( pold->loop_info );

	/*防止重复设置*/
	if(pold->loop_info.reflector.reflector_flag == loop_info->reflector.reflector_flag )
	{
		 printf("reflector has been seted\n");
		 zlog_debug (IFM_DBG_COMMON, "%s[%d] old flags %d new flags %d\n", __FUNCTION__, __LINE__,
                     pold->loop_info.reflector.reflector_flag, loop_info->reflector.reflector_flag );
        return ERRNO_SUCCESS;
	}

	ret = ifm_send_hal( loop_info, size, IFNET_INFO_LOOPBACK_FUNC, IFM_OPCODE_UPDATE, pold->ifindex );

	if ( ret < 0 )
	{
		zlog_err( "%s[%d] ifm checkout loopback vlan err %d\n",__FUNCTION__,__LINE__, ret );
		return ret;
	}
	memcpy( &pold->loop_info, loop_info, sizeof( struct ifm_loopback ) );
	return IFM_SUCCESS;
}
int ifm_set_combo( struct ifm_entry*pifm, uint8_t fiber_sub )
{
	int ret = 0;	
	struct gpnPortInfo gPortInfo;
	
	if ( pifm == NULL)
	{
		zlog_err( "%s[%d] pifm is null\n", __FUNCTION__, __LINE__ );
		return IFM_ERR_PARAM;
	}
	
	if ( pifm->port_info.fiber != IFNET_FIBERTYPE_COMBO )
	{
		 return IFM_ERR_PARAM_NOSUPPORT;
	}
	if( fiber_sub == IFNET_FIBERTYPE_FIBER )
	{
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_ETH_SFP_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_register(&gPortInfo);
	}
	else if( fiber_sub == IFNET_FIBERTYPE_COPPER)
	{
		gPortInfo.iAlarmPort = IFM_ETH_SFP_TYPE;
		gPortInfo.iIfindex = pifm->ifm.ifindex;
		ipran_alarm_port_unregister(&gPortInfo);		
	}
	
    if ( pifm->port_info.fiber_sub == fiber_sub )
    {
        return IFM_SUCCESS;
    }

	ret = ifm_send_hal( &fiber_sub, sizeof(uint8_t), IFNET_INFO_COMBO, IFM_OPCODE_UPDATE, pifm->ifm.ifindex );
	
	if ( ret != 0 )
	{
		printf( "%s[%d] ifm set hal ret %d\n",__FUNCTION__,__LINE__, ret );
		return ret;
	}

	pifm->port_info.fiber_sub = fiber_sub;

	return IFM_SUCCESS;	
}

/**
* @brief      设置接口的流控功能
* @param[in ] pifm: 接口信息， flow_ctrl: 流控配置
* @param[out] <+description+>
* @return     <+description+>
* @author     liubo
* @date       2018-4-23
* @note       <+description+>
*/
int ifm_set_flow_control(struct ifm_entry *pifm, struct ifm_flow_control *flow_ctrl)
{
    struct ifm_info *pinfo = NULL;
    int ret = 0;
    
    if(pifm == NULL)
    {
        zlog_err ( "%s[%d] pifm is null \n ", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }
    
    pinfo = & ( pifm->ifm );
#if 0
    /*避免重复下发*/
    if ( (pinfo->flow_ctrl.status == flow_ctrl->status) 
          && ( (pinfo->flow_ctrl.direction == flow_ctrl->direction) 
                 || (pinfo->flow_ctrl.direction == IFM_FLOW_CONTROL_RECEIVE_AND_SEND)) )
    {
        zlog_debug(IFM_DBG_COMMON,"\r\n %s[%d] flow control has been enabled \r\n", __FUNCTION__,__LINE__);
        return IFM_SUCCESS;
    }
#endif
    /*获取已配置信息，避免被覆盖*/
	if(flow_ctrl->status)
	{
		flow_ctrl->direction |= pinfo->flow_ctrl.direction;
    }
                 
    ret = ifm_send_hal ( flow_ctrl, sizeof(struct ifm_flow_control), IFNET_INFO_FLOW_CONTROL, IPC_OPCODE_UPDATE, pinfo->ifindex);
    if ( ret != 0 )
    {
        zlog_err ( "%s[%d] ipc send hal error, code=%d,ifindex=0x%0x",  __FUNCTION__, __LINE__, ret, pinfo->ifindex );
        return ret;
    }
    
    /*disable时置0*/
    if(flow_ctrl->status)
    {
        pinfo->flow_ctrl.direction |= flow_ctrl->direction;
    }
    else
    {
        pinfo->flow_ctrl.direction = 0;
    }

    pinfo->flow_ctrl.status = flow_ctrl->status;

    return IFM_SUCCESS;
}
int ifm_set_sample_interval( struct ifm_entry *pifm, uint8_t interval )
{
	struct ifm_info *pold = NULL;
	int ret = 0;
	if( pifm == NULL )
	{
		zlog_err("%s[%d]pifm is null\n",__FUNCTION__,__LINE__);
		return IFM_ERR_PARAM;
	}
	pold = &(pifm->ifm);
	if( pold->interval == interval )
	{
		return IFM_SUCCESS;
	}
	ret = ifm_send_hal(&interval, sizeof(pold->interval), IFNET_INFO_INTERVAL, 
			IPC_OPCODE_UPDATE, pold->ifindex);
	if(ret)
	{
		zlog_err ( "%s[%d] ipc send hal errocode=%d,ifindex=0x%0x",  __FUNCTION__, __LINE__, ret, pold->ifindex );
        return ret;
	}
	pold->interval = interval;
	return IFM_SUCCESS;
	
}
/*创建物理以太接口*/
int ifm_create_ethernet ( struct ifm_common *pifcom )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;
	unsigned int devType = 0;
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devType );

    if ( !pifcom )
    {
        zlog_err ( "%s[%d] pifcom is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug (IFM_DBG_COMMON, "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
  	/*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
     /*初始化接口信息 */

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;
	pport->fiber_sub = IFNET_FIBERTYPE_FIBER;
    pinfo = & ( pifm->ifm );
    pinfo->hide        = pport->hide;
    pinfo->sub_type    = pusp->subtype;
    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;   /* 驱动检测到 up 才能 up */
    pinfo->mode        = IFNET_MODE_L3;
    pinfo->tpid        = IFM_DEF_TPID;
    pinfo->mtu         = IFM_DEF_MTU;
    pinfo->statistics  = IFNET_STAT_ENABLE;
    pinfo->jumbo       = IFM_DEF_JUMBO;
    pinfo->sfp_als     = IFM_DEF_SFP_ALS;
    pinfo->sfp_off     = IFM_DEF_SFP_OFF;
    pinfo->sfp_on      = IFM_DEF_SFP_ON;
    pinfo->sfp_tx      = IFM_DEF_SFP_TX;
	pinfo->interval    = IFM_DEF_INTERVAL;
	pinfo->interval_time = IFM_DEF_INTERVAL;

	pinfo->phymaster = PORT_MS_AUTO;
    pinfo->phymaster_status = 0;

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifm_create ifindex 0x%0x pusp->subtype 0x%0x\n", __FUNCTION__, __LINE__,
                  ifindex, pusp->subtype );

    /* 设置默认封装信息 */
    pinfo->encap.type             = IFNET_ENCAP_UNTAG;
    pinfo->encap.svlan.vlan_start = IFM_DEF_SVLANID;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_DSCP;
    pinfo->qos.untag_pri    = DEFAULT_UNTAG_PRI;

	if( devType == ID_H9MOLMXE_VX  )
	{
		if( pusp->slot == 0  &&  pusp->port == 2 )
		{
			pinfo->status      = IFNET_LINKUP; 
		}
	}
	
    memcpy ( pinfo->mac, pifcom->mac, 6 );
 	/*set loopback default*/
 	memset( &( pinfo->loop_info ), 0, sizeof(struct ifm_loopback ) );
	memset( &( pinfo->reflector ), 0, sizeof(struct ifm_reflector) );

    /*set flow control default disable, added by liub 2018-4-25*/
    memset( &( pinfo->flow_ctrl ), 0, sizeof(struct ifm_flow_control) );

    ret = ifm_add ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /*发送接口创建信息到ftm*/

    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }
	
	if( pifm->ifm.status == IFNET_LINKDOWN )
	{

		ifm_link_alarm_report(pifm, IPC_OPCODE_ADD );
		
	}

    return IFM_SUCCESS;
}

/* 创建子接口 */
int ifm_create_sub_ethernet ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_entry *pfifm = NULL;
    struct ifm_info *pinfo;
    int ret;

   /*检查父接口是否存在*/
    pfifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( ifindex ) );
    if ( !pfifm )
    {
        zlog_err ( "%s[%d]Physical interface for this sub port not exist\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARNT_NOT_FOUND;
    }
    /*检查父接口是l3模式*/

    if ( pfifm->ifm.mode != IFNET_MODE_L3 )
    {
        zlog_err ( "%s[%d]This physical interface mode isn't l3\n" , __FUNCTION__, __LINE__ );
        return IFM_ERR_PARNT_NO_L3;
    }
   /*检查子接口是否已经存在*/
    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug (IFM_DBG_COMMON, "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return ifm_checkout_dcn(pifm);
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );
     /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }

   /*初始化接口信息 */
    pinfo = & ( pifm->ifm );
    pinfo->parent         = IFM_PARENT_IFINDEX_GET ( ifindex );
    pinfo->shutdown       = IFNET_NO_SHUTDOWN;
    pinfo->sub_type       = IFNET_SUBTYPE_SUBPORT;
    pinfo->mode           = IFNET_MODE_L3;
    pinfo->mtu            = IFM_DEF_MTU;
    pinfo->status         = IFNET_LINKDOWN;  /* 子接口配了封装类型才能 up */
    pinfo->statistics     = IFNET_STAT_DISABLE;
    memcpy ( pinfo->mac, pfifm->ifm.mac, sizeof ( pfifm->ifm.mac ) );

    /* 初始化接口封装信息 */
    pinfo->encap.type     = IFNET_ENCAP_INVALID;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_DSCP;
    pinfo->qos.phb_enable   = ENABLE;
    pinfo->qos.phb_id       = 0;
    pinfo->qos.phb_type     = QOS_TYPE_COS;

    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }


    /*更新父接口的son list*/
    ifm_set_son ( pifm->ifm.parent, pifm, IFM_OPCODE_ADD );
    /*发送接口创建信息到ftm */
    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

	//ipran_alarm_port_register( IFM_FUN_ETH_SUB_TYPE, pifm->ifm.slot, pifm->ifm.port, pifm->ifm.sub_port, 0);
	//ipran_alarm_port_register( IFM_VUNI_TYPE, pifm->ifm.slot, pifm->ifm.port, pifm->ifm.sub_port, pifm->ifm.type);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
	gPortInfo.iIfindex = pifm->ifm.ifindex;
	ipran_alarm_port_register(&gPortInfo);
	
	gPortInfo.iAlarmPort = IFM_VUNI_TYPE;
	ipran_alarm_port_register(&gPortInfo);
    return IFM_SUCCESS;
}

/*创建物理TDM接口*/
int ifm_create_tdm ( struct ifm_common *pifcom )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;

    if ( !pifcom )
    {
        zlog_err ( "%s[%d] pifcom is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }

    /*检查接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
   /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }

     /*初始化接口信息 */

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;

    pinfo = & ( pifm->ifm );
    pinfo->hide        = pport->hide;
    pinfo->sub_type    = pusp->subtype;
    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;   /* 驱动检测到 up 才能 up */
    pinfo->statistics  = IFNET_STAT_ENABLE;
    pinfo->qos.untag_pri = DEFAULT_UNTAG_PRI;

    memcpy ( pinfo->mac, pifcom->mac, 6 );

    ret = ifm_add ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

     /*发送接口创建信息到ftm */

    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    return IFM_SUCCESS;
}


/* 创建TDM子接口 */
int ifm_create_sub_tdm ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_entry *pfifm = NULL;
    struct ifm_info *pinfo;
    int ret;

    /*检查父接口是否存在*/

    pfifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( ifindex ) );
    if ( !pfifm )
    {
        zlog_err ( "%s[%d]Physical interface for this sub port not exist\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARNT_NOT_FOUND;
    }
     /*检查子接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );
   /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }


    /*初始化接口信息 */
    pinfo = & ( pifm->ifm );
    pinfo->parent         = IFM_PARENT_IFINDEX_GET ( ifindex );
    pinfo->shutdown       = IFNET_NO_SHUTDOWN;
    pinfo->sub_type       = IFNET_SUBTYPE_SUBPORT;
    pinfo->status         = IFNET_LINKDOWN;  /* 子接口配了封装类型才能 up */
    pinfo->statistics     = IFNET_STAT_DISABLE;
    pinfo->qos.untag_pri  = DEFAULT_UNTAG_PRI;
    memcpy ( pinfo->mac, pfifm->ifm.mac, sizeof ( pfifm->ifm.mac ) );

    /*创建子接口并等待应答*/
    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }


    /*更新父接口的son list*/
    ifm_set_son ( pifm->ifm.parent, pifm, IFM_OPCODE_ADD );

   /*发送接口创建信息到ftm*/
    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

	//ipran_alarm_port_register( IFM_E1_SUB_TYPE, pifm->ifm.slot, pifm->ifm.port, pifm->ifm.sub_port, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_E1_SUB_TYPE;
	gPortInfo.iIfindex = pifm->ifm.ifindex;
	ipran_alarm_port_register(&gPortInfo);
    return IFM_SUCCESS;
}

/*创建物理STM接口*/
int ifm_create_stm ( struct ifm_common *pifcom )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }

    /*检查接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
       zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
     /*初始化接口信息 */

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;
    pport->fiber = IFNET_FIBERTYPE_FIBER;
	
    pinfo = & ( pifm->ifm );
    pinfo->hide        = pport->hide;
    pinfo->sub_type    = pusp->subtype;
    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;   /* 驱动检测到 up 才能 up */
    pinfo->statistics  = IFNET_STAT_ENABLE;
    pinfo->sfp_als     = IFM_DEF_SFP_ALS;
    pinfo->sfp_off     = IFM_DEF_SFP_OFF;
    pinfo->sfp_on      = IFM_DEF_SFP_ON;
    pinfo->sfp_tx      = IFM_DEF_SFP_TX;  
	
    memcpy ( pinfo->mac, pifcom->mac, 6 );

    /*创建接口*/
    ret = ifm_add ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /*发送接口创建信息到ftm*/
    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    return IFM_SUCCESS;
}
/*创建物理以VCG*/
int ifm_create_vcg ( struct ifm_common *pifcom )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( "%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }

    /*检查接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
     /*初始化接口信息 */

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;

    pinfo = & ( pifm->ifm );
    pinfo->hide        = pport->hide;
    pinfo->sub_type    = pusp->subtype;
    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;   /* 驱动检测到 up 才能 up */
    pinfo->statistics  = IFNET_STAT_ENABLE;

    memcpy ( pinfo->mac, pifcom->mac, 6 );

    /*创建接口*/
    ret = ifm_add ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    return IFM_SUCCESS;
}

/* 创建E1接口 */
int ifm_create_e1 ( struct ifm_common *pifcom )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( "%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }

    /*检查接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
     /*初始化接口信息 */

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;

    pinfo = & ( pifm->ifm );
    pinfo->hide        = pport->hide;
    pinfo->sub_type    = pusp->subtype;
    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;   /* 驱动检测到 up 才能 up */
    pinfo->statistics  = IFNET_STAT_ENABLE;

    memcpy ( pinfo->mac, pifcom->mac, 6 );

    /*创建接口*/
    ret = ifm_add( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    return IFM_SUCCESS;
}

/* 创建STM子接口 */
int ifm_create_sub_stm ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_entry *pfifm = NULL;
    struct ifm_info *pinfo;
    int ret;

    /*检查父接口是否存在*/

    pfifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( ifindex ) );
    if ( !pfifm )
    {
        zlog_err ( "%s[%d]Physical interface for this sub port not exist\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARNT_NOT_FOUND;
    }
     /*检查子接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );
     /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }


     /*初始化接口信息 */
    pinfo = & ( pifm->ifm );
    pinfo->parent         = IFM_PARENT_IFINDEX_GET ( ifindex );
    pinfo->shutdown       = IFNET_NO_SHUTDOWN;
    pinfo->sub_type       = IFNET_SUBTYPE_SUBPORT;
    pinfo->status         = IFNET_LINKDOWN; /* 子接口配了封装类型才能 up */
    pinfo->statistics     = IFNET_STAT_DISABLE;
    memcpy ( pinfo->mac, pfifm->ifm.mac, sizeof ( pfifm->ifm.mac ) );

    /*发送接口创建信息到hal并等待应答*/
    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /*更新父接口son list*/
    ifm_set_son ( pifm->ifm.parent, pifm, IFM_OPCODE_ADD );

     /*发送接口创建信息到ftm*/

    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    return IFM_SUCCESS;
}

/*创建物理以太接口*/
int ifm_create_port ( struct ifm_common *pifcom )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;

    if ( !pifcom )
    {
        zlog_err ( "%s[%d] pifcom is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( "%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
  	/*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
     /*初始化接口信息 */

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;

    pinfo = & ( pifm->ifm );
    pinfo->hide        = pport->hide;
    pinfo->sub_type    = pusp->subtype;
    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;   /* 驱动检测到 up 才能 up */
    pinfo->mode        = IFNET_MODE_PHYSICAL;
    pinfo->tpid        = IFM_DEF_TPID;
    pinfo->mtu         = IFM_DEF_MTU;
    pinfo->statistics  = IFNET_STAT_ENABLE;
    pinfo->jumbo       = IFM_DEF_JUMBO;
    pinfo->sfp_als     = IFM_DEF_SFP_ALS;
    pinfo->sfp_off     = IFM_DEF_SFP_OFF;
    pinfo->sfp_on      = IFM_DEF_SFP_ON;
    pinfo->sfp_tx      = IFM_DEF_SFP_TX; 
	pinfo->interval    = IFM_DEF_INTERVAL;
	pinfo->interval_time = IFM_DEF_INTERVAL;

    zlog_debug ( "%s[%d] ifm_create ifindex 0x%0x pusp->subtype 0x%0x\n", __FUNCTION__, __LINE__,
                  ifindex, pusp->subtype );

    /* 设置默认封装信息 */
    pinfo->encap.type             = IFNET_ENCAP_UNTAG;
    pinfo->encap.svlan.vlan_start = IFM_DEF_SVLANID;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_DSCP;
    pinfo->qos.untag_pri    = DEFAULT_UNTAG_PRI;

    memcpy ( pinfo->mac, pifcom->mac, 6 );
	/*set loopback default*/ 
	memset( &( pinfo->loop_info ), 0, sizeof(struct ifm_loopback ) );
	memset( &( pinfo->reflector ), 0, sizeof(struct ifm_reflector) );
 
    ret = ifm_add ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }
	if( pifm->ifm.status == IFNET_LINKDOWN )
	{
		ifm_link_alarm_report(pifm, IPC_OPCODE_ADD );
		
	}
    return IFM_SUCCESS;
}


/* 创建物理接口 */
int ifm_create_physical ( struct ifm_common *pifcom )
{
    int ret = 0;
	struct gpnPortInfo gPortInfo;

    if ( pifcom == NULL )
    {
        zlog_err ( "%s[%d] pifcom  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }


    if ( ( pifcom->usp.type == IFNET_TYPE_ETHERNET )
		|| ( pifcom->usp.type == IFNET_TYPE_GIGABIT_ETHERNET )
		||( pifcom->usp.type == IFNET_TYPE_XGIGABIT_ETHERNET ))
    {

		/* modify for ipran by lipf, 2018/4/25 */
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&pifcom->usp);
		ipran_alarm_port_register(&gPortInfo);		
		
		if ( pifcom->port_info.fiber == IFNET_FIBERTYPE_FIBER )
		{

			/* modify for ipran by lipf, 2018/4/25 */
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_ETH_SFP_TYPE;
			gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&pifcom->usp);
			ipran_alarm_port_register(&gPortInfo);
		}		
	
    	if(pifcom->usp.subtype == IFNET_SUBTYPE_PHYSICAL)
    	{
			ret = ifm_create_port( pifcom );
			if ( ret < 0 )
			{
				zlog_err ( "%s[%d] ifm_create_ethernet errocode=%d", __FUNCTION__, __LINE__, ret );
			
				return IFM_ERR_HAL;
			}

    	}
		else
		{
			ret = ifm_create_ethernet ( pifcom );
        	if ( ret < 0 )
        	{
            	zlog_err ( "%s[%d] ifm_create_ethernet errocode=%d", __FUNCTION__, __LINE__, ret );

                return IFM_ERR_HAL;
            }
        }		
    }
    else if ( pifcom->usp.type == IFNET_TYPE_TDM )
    {
        ret = ifm_create_tdm ( pifcom );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ifm_create_tdm errocode=%d", __FUNCTION__, __LINE__, ret );

            return IFM_ERR_HAL;
        }
		//ipran_alarm_port_register( IFM_E1_TYPE, pifcom->usp.slot, pifcom->usp.port, 0, 0);

		/* modify for ipran by lipf, 2018/4/25 */
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_E1_TYPE;
		gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&pifcom->usp);
		ipran_alarm_port_register(&gPortInfo);
    }
    else if ( pifcom->usp.type == IFNET_TYPE_STM )
    {
        ret = ifm_create_stm ( pifcom );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ifm_create_ethernet errocode=%d", __FUNCTION__, __LINE__, ret );

            return IFM_ERR_HAL;
        }

        //register alarm type: stmn. added by liub 2018-12-10
        memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
        gPortInfo.iAlarmPort = IFM_STMN_TYPE;
        gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&pifcom->usp);
        ipran_alarm_port_register(&gPortInfo);
    }
    else if ( pifcom->usp.type == IFNET_TYPE_VCG )
    {
        ret = ifm_create_vcg ( pifcom );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ifm_create_ethernet errocode=%d", __FUNCTION__, __LINE__, ret );

            return IFM_ERR_HAL;
        }
    }
    else if ( pifcom->usp.type == IFNET_TYPE_E1)
    {
        ret = ifm_create_e1 ( pifcom );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ifm_create_ethernet errocode=%d", __FUNCTION__, __LINE__, ret );

            return IFM_ERR_HAL;
        }
    }

    return IFM_SUCCESS;
}

/* 创建 vlanif 接口 */
int ifm_create_vlanif ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_info *pinfo;
    int ret;

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug (IFM_DBG_COMMON, "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );
   /*为接口分配内存*/
    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }

   /*初始化接口信息 */

    pinfo = & ( pifm->ifm );
    pinfo->sub_type       = IFNET_SUBTYPE_VPORT;
    pinfo->mode           = IFNET_MODE_L3;
    pinfo->mtu            = IFM_DEF_MTU;
    pinfo->status         = IFNET_LINKDOWN;     /* vlan 成员口数量大于 1 才能 up */
    pinfo->shutdown       = IFM_DEF_SHUTDOWN;
    pinfo->statistics     = IFNET_STAT_DISABLE;

   /* 初始化封装信息 */
    pinfo->encap.type                  = IFNET_ENCAP_VLANIF;
    pinfo->encap.svlan.vlan_start = pinfo->sub_port;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_DSCP;
    pinfo->qos.untag_pri    = DEFAULT_UNTAG_PRI;

    /* 设置接口 mac为整机mac */
    ret = devm_comm_get_mac ( 1, 0, MODULE_ID_IFM , pinfo->mac );
    if ( ret )
    {
        zlog_err ( "%s[%d] fail to get mac", __FUNCTION__, __LINE__ );
        return ret;
    }
    /*发送接口创建信息到hal并等待应答*/
    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

     /*发送接口创建信息到ftm*/
    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

	//ipran_alarm_port_register( IFM_VLAN_OAM_TYPE, pifm->ifm.slot, pifm->ifm.port, 0, 0 );

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_VLAN_OAM_TYPE;
	gPortInfo.iIfindex = pifm->ifm.ifindex;
	ipran_alarm_port_register(&gPortInfo);
    return IFM_SUCCESS;
}


/* 创建 tunnel 接口 */
int ifm_create_tunnel ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_info *pinfo;
    int ret;

   /*检查接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    zlog_debug (IFM_DBG_COMMON, "%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );
    /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
    /*初始化接口信息 */

    pinfo = & ( pifm->ifm );
    pinfo->sub_type       = IFNET_SUBTYPE_VPORT;
    pinfo->mode           = IFNET_MODE_L3;
    pinfo->mtu            = IFM_DEF_MTU;
    pinfo->status         = IFNET_LINKDOWN;    /* tunnel 模块配了协议类型才能 up */
    pinfo->shutdown       = IFM_DEF_SHUTDOWN;
    pinfo->statistics     = IFNET_STAT_DISABLE;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_EXP;
     /*发送接口创建信息到hal并等待应答*/

    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }
   /*发送接口创建信息到ftm*/

    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    return IFM_SUCCESS;
}


/* 创建 trunk 接口 */
int ifm_create_trunk ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_info *pinfo;
    struct ifm_port *pport;
    int ret = 0;

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }

    pport = & ( pifm->port_info );
    memset ( pport, 0, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;

    /*初始化接口信息 */

    pinfo = & ( pifm->ifm );

    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;  /* trunk 成员端口数大于 1 才能 up */
    pinfo->mode        = IFNET_MODE_L3;
    pinfo->mtu         = IFM_DEF_MTU;
    pinfo->sub_type    = IFNET_SUBTYPE_VPORT;
    pinfo->statistics  = IFNET_STAT_ENABLE;
    pinfo->tpid        = IFM_DEF_TPID;
    pinfo->jumbo       = IFM_DEF_JUMBO;

   /* 初始化封装信息 */

    pinfo->encap.type  = IFNET_ENCAP_UNTAG;
    pinfo->encap.svlan.vlan_start = IFM_DEF_SVLANID;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_DSCP;
    pinfo->qos.untag_pri    = DEFAULT_UNTAG_PRI;

    /* 设置接口 mac为整机mac */

    ret = devm_comm_get_mac ( 1, 0, MODULE_ID_IFM , pinfo->mac );
    if ( ret )
    {
        zlog_err ( "%s[%d] fail to get mac", __FUNCTION__, __LINE__ );
        return ret;
    }
    /*发送接口创建信息到hal并等待应答*/

    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

     /*发送接口创建信息到ftm*/

    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    return IFM_SUCCESS;
}

/* 创建 trunk 子接口 */
int ifm_create_sub_trunk ( uint32_t ifindex )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_entry *pfifm = NULL;
    struct ifm_info *pinfo;
    struct ifm_port *pport;
    int ret = 0;

    /*检查父接口是否存在*/

    pfifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( ifindex ) );
    if ( !pfifm )
    {
        zlog_err ( "%s[%d]Physical interface for this sub port not exist\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARNT_NOT_FOUND;
    }

     /*检查父接口是l3模式*/

    if ( pfifm->ifm.mode != IFNET_MODE_L3 )
    {
        zlog_err ( "%s[%d]This physical interface mode isn't l3\n" , __FUNCTION__, __LINE__ );
        return IFM_ERR_PARNT_NO_L3;
    }

    /*检查子接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
    zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }

    pport = & ( pifm->port_info );
    memset ( pport, 0, sizeof ( struct ifm_port ) );
    pport->flap_period = IFM_DEF_FLAP;

    /*初始化接口信息 */

    pinfo = & ( pifm->ifm );

    pinfo->shutdown    = IFNET_NO_SHUTDOWN;
    pinfo->status      = IFNET_LINKDOWN;  /* trunk 成员端口数大于 1 才能 up */
    pinfo->mode        = IFNET_MODE_L3;
    pinfo->parent      = IFM_PARENT_IFINDEX_GET ( ifindex );
    pinfo->sub_type    = IFNET_SUBTYPE_SUBPORT;
    pinfo->statistics  = IFNET_STAT_DISABLE;
    pinfo->mtu         = IFM_DEF_MTU;
    memcpy ( pinfo->mac, pfifm->ifm.mac, sizeof ( pfifm->ifm.mac ) );

     /* 初始化封装信息 */
    pinfo->encap.type             = IFNET_ENCAP_INVALID;

    /* set qos default */
    pinfo->qos.domain_id    = 0;
    pinfo->qos.domain_type  = QOS_TYPE_DSCP;
    pinfo->qos.untag_pri    = DEFAULT_UNTAG_PRI;
    pinfo->qos.phb_enable   = ENABLE;
    pinfo->qos.phb_id       = 0;
    pinfo->qos.phb_type     = QOS_TYPE_COS;

    ret = devm_comm_get_mac ( 1, 0, MODULE_ID_IFM , pinfo->mac );
    if ( ret )
    {
        zlog_err ( "%s[%d] fail to get mac", __FUNCTION__, __LINE__ );
        return ret;
    }
    /*发送接口创建信息到hal并等待应答*/

    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /*更新父接口son list*/
    ifm_set_son ( pifm->ifm.parent, pifm, IFM_OPCODE_ADD );

   /*发送接口创建信息到ftm*/
    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }
	//ipran_alarm_port_register( IFM_TRUNK_SUBIF_TYPE, 0, pifm->ifm.port, pifm->ifm.sub_port, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TRUNK_SUBIF_TYPE;
	gPortInfo.iIfindex = pifm->ifm.ifindex;
	ipran_alarm_port_register(&gPortInfo);
    return IFM_SUCCESS;
}


/* 创建 loopback 接口 */
int ifm_create_loopback ( uint32_t ifindex  )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_info *pinfo;
    int ret;
    struct ifm_event event;

     /*检查接口是否已经存在*/
    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return ifm_checkout_dcn(pifm);
    }
    zlog_debug (IFM_DBG_COMMON, "%s[%d] ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex );

     /*为接口分配内存*/
    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm  erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }

    /*初始化接口信息 */
    pinfo = & ( pifm->ifm );
    pinfo->sub_type       = IFNET_SUBTYPE_VPORT;
    pinfo->mode           = IFNET_MODE_L3;
    pinfo->mtu            = IFM_DEF_MTU;
    pinfo->status         = IFNET_LINKUP;
    pinfo->shutdown       = IFM_DEF_SHUTDOWN;
    pinfo->statistics     = IFNET_STAT_DISABLE;

    /* 获取接口 mac */
    ret = devm_comm_get_mac ( 1, 0, MODULE_ID_IFM , pinfo->mac );
    if ( ret )
    {
        zlog_err ( "%s[%d]:leave %s:ifm get mac from devm error,mac == NULL\n", __FILE__, __LINE__, __func__ );
        return ret;
    }
    else
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d]:%s: ifm sucessfully get mac  from devm :%02x:%02x:%02x:%02x:%02x:%02x.\n", __FILE__, __LINE__, __func__, pinfo->mac[0], pinfo->mac[1], pinfo->mac[2], pinfo->mac[3], pinfo->mac[4], pinfo->mac[5] );
    }
    /*发送接口创建信息到hal并保存*/
    ret = ifm_add_wait_ack ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /*发送接口创建信息到ftm*/
    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    /*环回口一直up，创建后通知环回口up事件*/
    event.event   = IFNET_EVENT_UP;
    event.ifindex = pifm->ifm.ifindex;
    event.mode    = pifm->ifm.mode;
    ifm_event_notify ( IFNET_EVENT_UP, &event );
    zlog_debug (IFM_DBG_COMMON, "%s[%d] ifm_event_notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, pifm->ifm.ifindex );

    return IFM_SUCCESS;
}


/* 创建 clock 接口 */
int ifm_create_clock ( struct ifm_common *pifcom  )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_usp *pusp = NULL;
    struct ifm_port *pport;
    struct ifm_info *pinfo;
    int ret = 0;

    if ( pifcom == NULL )
    {
        zlog_err ( "%s[%d] pifcom  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pusp = & ( pifcom->usp );
    ifindex = ifm_get_ifindex_by_usp ( pusp );

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] pusp->subtype=%d\n", __FUNCTION__, __LINE__, pusp->subtype );
    if ( !ifindex )
    {
        zlog_err ( "%s[%d] ifm get ifindex by usp erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_NOT_FOUND;
    }
   /*检查接口是否已经存在*/

    pifm = ifm_lookup ( ifindex );
    if ( pifm )
    {
        zlog_err ( "%s[%d] ifindex=0x%0x exist\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_SUCCESS;
    }
     /*为接口分配内存*/

    pifm = ifm_create ( ifindex );
    if ( NULL == pifm )
    {
        zlog_err ( "%s[%d] ifm_create XMALLOC pifm	erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_XMALLOC;
    }
    /*根据port_info 初始化接口信息*/

    pport = & ( pifm->port_info );
    memcpy ( pport, &pifcom->port_info, sizeof ( struct ifm_port ) );

    pinfo = & ( pifm->ifm );
    pinfo->sub_type    = pusp->subtype;

    memcpy ( pinfo->mac, pifcom->mac, 6 );

     /*发送接口创建信息到hal并保存*/

    ret = ifm_add ( pifm );
    if ( ret )
    {
        zlog_err ( "%s[%d] ifm_add errocode=%d", __FUNCTION__, __LINE__, ret );
        return ret;
    }
    /*发送接口创建信息到ftm*/

    ret = ifm_send_ftm ( & ( pifm->ifm ), sizeof ( struct ifm_info ),
                         0, IPC_OPCODE_ADD, ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] ipc_send_common errocode=%d", __FUNCTION__, __LINE__, ret );

        return IFM_ERR_HAL;
    }

    return IFM_SUCCESS;
}

/* 创建接口 */
int ifm_create_interface ( uint32_t ifindex )
{
    int ret = ERRNO_SUCCESS;
    int type = IFM_TYPE_ID_GET ( ifindex );
    switch ( type )
    {
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
            if ( IFM_IS_SUBPORT ( ifindex ) )
            {
                ret = ifm_create_sub_ethernet ( ifindex );
            }
            else
            {
                zlog_err ( "%s[%d] ifm_create ifindex 0x%0x  erro\n", __FUNCTION__, __LINE__, ifindex );
                return ERRNO_PARAM_ILLEGAL;

            }
            break;
        case IFNET_TYPE_VLANIF:
            ret = ifm_create_vlanif ( ifindex );
            break;
        case IFNET_TYPE_LOOPBACK:
            ret = ifm_create_loopback ( ifindex );
            break;
        case IFNET_TYPE_TUNNEL:
            ret = ifm_create_tunnel ( ifindex );
            break;
        case IFNET_TYPE_TRUNK:
            if ( IFM_IS_SUBPORT ( ifindex ) )
            {
                ret = ifm_create_sub_trunk ( ifindex );
            }
            else
            {
                ret = ifm_create_trunk ( ifindex );
            }
            break;
        case IFNET_TYPE_TDM:
            if ( IFM_IS_SUBPORT ( ifindex ) )
            {
                ret = ifm_create_sub_tdm ( ifindex );
            }
            else
            {
                zlog_err ( "%s[%d] ifm_create ifindex 0x%0x  erro\n", __FUNCTION__, __LINE__, ifindex );
                return ERRNO_PARAM_ILLEGAL;
            }

            break;
        case IFNET_TYPE_STM:
            if ( IFM_IS_SUBPORT ( ifindex ) )
            {
                ret = ifm_create_sub_stm ( ifindex );
            }
            else
            {
                zlog_err ( "%s[%d] ifm_create ifindex 0x%0x  erro\n", __FUNCTION__, __LINE__, ifindex );
                return ERRNO_PARAM_ILLEGAL;
            }

            break;
        default:
            zlog_err ( "%s[%d] ifm_create ifindex 0x%0x  erro\n", __FUNCTION__, __LINE__, ifindex );
            return ERRNO_PARAM_ILLEGAL;
            break;
    }

    return ret;
}

/* 创建接口 信息*/
int ifm_get_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type )
{
    struct ifm_entry *pifm = NULL;
    struct ifm_info *pold = NULL;
    struct ifm_usp *pusp = NULL;

    if ( pinfo == NULL )
    {
        zlog_err ( "%s[%d] pinfo  is null\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_PARAM;
    }

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        return IFM_ERR_NOT_FOUND;
    }
    pold = & ( pifm->ifm );

    switch ( type )
    {
        case IFNET_INFO_PARENT:
            * ( uint32_t * ) pinfo = pold->parent;
            break;
        case IFNET_INFO_TYPE:
            * ( uint8_t * ) pinfo = pold->type;
            break;
        case IFNET_INFO_SUBTYPE:
            * ( uint8_t * ) pinfo = pold->sub_type;
            break;
        case IFNET_INFO_MODE:
            * ( uint8_t * ) pinfo = pold->mode;
            break;
        case IFNET_INFO_ENCAP:
            memcpy ( pinfo, &pold->encap, sizeof ( struct ifm_encap ) );
            break;
        case IFNET_INFO_SHUTDOWN:
            * ( uint8_t * ) pinfo = pold->shutdown;
            break;
        case IFNET_INFO_FLAP:
            * ( uint8_t * ) pinfo =  pifm->port_info.flap_period;
            break;
        case IFNET_INFO_SPEED:
            memcpy ( ( uchar * ) pinfo, &pifm->port_info, sizeof ( struct ifm_port ) );
            break;
        case IFNET_INFO_STATUS:
            * ( uint8_t * ) pinfo = pold->status;
            break;
        case IFNET_INFO_USP:
            pusp = pinfo;
            pusp->type = pold->type;
            pusp->unit = pold->unit;
            pusp->slot = pold->slot;
            pusp->port = pold->port;
            pusp->sub_port = pold->sub_port;
            break;
        case IFNET_INFO_STATISTICS:
            * ( uint8_t * ) pinfo = pold->statistics;
            break;
        case IFNET_INFO_TPID:
            * ( uint16_t * ) pinfo = pold->tpid;
            break;
        case IFNET_INFO_MTU:
            * ( uint16_t * ) pinfo = pold->mtu;
            break;
        case IFNET_INFO_JUMBO:
            * ( uint16_t * ) pinfo = pold->jumbo;
            break;
        case IFNET_INFO_ALIAS:
            memcpy ( ( uchar * ) pinfo, pifm->alias, strlen ( pifm->alias ) + 1 );
            break;
        case IFNET_INFO_MAC:
            memcpy ( ( uchar * ) pinfo, pold->mac, 6 );
            break;
        case IFNET_INFO_LOOPBACK:
            * ( struct ifm_reflector * ) pinfo = pold->reflector;
            break;
	case IFNET_INFO_SFP_ALS:
		 * (uint8_t *) pinfo = pold->sfp_als;
		 break;
	case IFNET_INFO_SFP_TX:
		 * ( uint8_t * ) pinfo = pold->sfp_tx;
		 break;
	case IFNET_INFO_SFP_ON:
		 * (uint32_t *) pinfo = pold->sfp_on;
		 break;
	case IFNET_INFO_SFP_OFF:
		 * ( uint32_t * ) pinfo = pold->sfp_off;
		 break;
	case IFNET_INFO_LOOPBACK_FUNC:
		memcpy( pinfo, &pold->loop_info, sizeof( struct ifm_loopback ));
		break;
        case IFNET_INFO_FLOW_CONTROL:
            /*added by liub 2018-4-25*/
            memcpy(pinfo, &pold->flow_ctrl, sizeof(struct ifm_flow_control));
            break;
        default:
            zlog_err ( "%s[%d] ifm miss match info type ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
            return IFM_ERR_MISMATCH;
    }

    return IFM_SUCCESS;
}

/* 配置接口 信息*/
int ifm_set_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type, enum IFM_OPCODE opcode )
{
    int ret = 0;
    struct ifm_entry *pifm = NULL;

    if ( pinfo == NULL )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d pinfo is null\n", __FUNCTION__, __LINE__, ifindex, type );
        return IFM_ERR_PARAM;
    }
     /*检查接口是否存在*/
    pifm = ifm_lookup2 ( ifindex );
    if ( !pifm )
    {
        zlog_err ( "%s[%d] ifm_lookup ifindex=0x%0x erro\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_ERR_NOT_FOUND;
    }
   /*检查是否是trunk口的非允许配置项*/
    ret = ifm_checkout_trunk ( pifm, type );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d ret %d\n", __FUNCTION__, __LINE__, ifindex, type, ret );
        return ret;
    }
     /*检查是否是带外口的非允许配置项*/

    ret = ifm_checkout_outband ( pifm, type );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d ret %d\n", __FUNCTION__, __LINE__, ifindex, type, ret );
        return ret;
    }
	
	ret = ifm_checkout_mode ( pifm, type );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d ret %d\n", __FUNCTION__, __LINE__, ifindex, type, ret );
        return ret;
    }

    switch ( type )
    {
        case IFNET_INFO_ALIAS:
            ret = ifm_set_alias ( pifm, ( char * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;

        case IFNET_INFO_STATUS:
            ret = ifm_set_status ( pifm , * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;

        case IFNET_INFO_SUBTYPE:
            ret = ifm_set_subtype ( pifm, * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;
        case IFNET_INFO_MODE:
            ret = ifm_set_mode ( pifm, * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_ENCAP:
            ret = ifm_set_encap ( pifm, ( struct ifm_encap * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;
        case IFNET_INFO_SHUTDOWN:
            ret = ifm_set_shutdown ( pifm , * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;
        case IFNET_INFO_FLAP:
            ret = ifm_set_flap ( pifm , * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;
	case IFNET_INFO_DOWN_FLAP:
		ret = ifm_set_down_flap ( pifm , * ( uint8_t * ) pinfo );
		if ( ret < 0 )
		{
			zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
			return ret;
		}

		break;

        case IFNET_INFO_SPEED:
            ret = ifm_set_speed ( pifm , ( struct ifm_port * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_STATISTICS:
            ret = ifm_set_statistics ( pifm , * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;
        case IFNET_INFO_TPID:
            ret = ifm_set_tpid ( pifm , * ( uint16_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_MTU:

            ret = ifm_set_mtu ( pifm , * ( uint16_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return IFM_ERR_HAL;
            }

            break;
            
        case IFNET_INFO_PHYMASTER:
            ret = ifm_set_phymaster(pifm , * ( uint32_t * ) pinfo);
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return IFM_ERR_HAL;
            }

            break;             
        case IFNET_INFO_JUMBO:

            ret = ifm_set_jumbo ( pifm , * ( uint16_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }

            break;

        case IFNET_INFO_MAC:

            ret = ifm_set_mac ( pifm , ( uchar * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_LOOPBACK:

            ret = ifm_set_loopback ( pifm , * ( struct ifm_reflector * ) pinfo );
            if ( ret != 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_SFP_OFF:
            ret = ifm_set_sfp_off ( pifm , * ( uint32_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_SFP_ON:
            ret = ifm_set_sfp_on ( pifm, * ( uint32_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_SFP_ALS:
            ret = ifm_set_sfp_als ( pifm, * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
        case IFNET_INFO_SFP_TX:
            ret = ifm_set_sfp_tx ( pifm, * ( uint8_t * ) pinfo );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d", __FUNCTION__, __LINE__, ifindex, type );
                return ret;
            }
            break;
       case IFNET_INFO_LOOPBACK_FUNC:
	   ret = ifm_set_loopback_info( pifm, (struct ifm_loopback *)pinfo);
	   if (ret < 0 )
   	  {
   	  	 zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d", __FUNCTION__, __LINE__, ifindex, type );
                	return ret;
   	  }
	   break;
		case IFNET_INFO_COMBO:
			 ret = ifm_set_combo( pifm, *(uint8_t *)pinfo);
			 if (ret != 0 )
			 {
				 zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x type=%d", __FUNCTION__, __LINE__, ifindex, type );
				 return ret;
			 }
			break;

        case IFNET_INFO_FLOW_CONTROL:
            /*added by liub 2018-4-25*/
            ret = ifm_set_flow_control(pifm, (struct ifm_flow_control *)pinfo);
            if(ret != 0)
            {
                zlog_err("%s[%d] set ifm flow control error, ifindex=0x%x, type=%d \n", __FUNCTION__, __LINE__, ifindex, type);
                return ret;
            }
            break;
		case IFNET_INFO_INTERVAL:            
            ret = ifm_set_sample_interval(pifm, *(uint8_t*)pinfo);
            if(ret != 0)
            {
                zlog_err("%s[%d] set ifm sample interval error, ifindex=0x%x, type=%d \n", __FUNCTION__, __LINE__, ifindex, type);
                return ret;
            }
            break;
            
        default:
            zlog_err ( "%s[%d] update ifm info erro ifindex=0x%0x miss match type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
            return IFM_ERR_MISMATCH;
    }

    return IFM_SUCCESS;
}


/* ä»ŽèŠ¯ç‰‡èŽ·å–ç»Ÿè®? */
int ifm_get_counter ( uint32_t ifindex, struct ifm_counter *pCounter)
{
	int ret = -1;

	struct ipc_mesg_n *pMsgRcv = NULL;

    pMsgRcv = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_IFM, 
                               IPC_TYPE_IFM, IFNET_INFO_COUNTER, IPC_OPCODE_GET, ifindex, 0);

	if(pMsgRcv)
	{
	    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
        {
            ret = 0;
            memcpy(pCounter, pMsgRcv->msg_data, sizeof(struct ifm_counter));
        }

        mem_share_free(pMsgRcv, MODULE_ID_IFM);
	}

    return(ret);

    
}
/* ä»? SDK èŽ·å–åˆå§‹é€ŸçŽ‡å‚æ•° */
int ifm_get_speed( uint32_t ifindex, struct ifm_port *pPort)
{
	int ret = -1;
    
	struct ipc_mesg_n *pMsgRcv = NULL;

    pMsgRcv = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_IFM, 
                               IPC_TYPE_IFM, IFNET_INFO_SPEED, IPC_OPCODE_GET, ifindex, 0);

	if(pMsgRcv)
	{
	    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
        {
            ret = 0;
            memcpy(pPort, pMsgRcv->msg_data, sizeof(struct ifm_port));
        }

        mem_share_free(pMsgRcv, MODULE_ID_IFM);
	}

    return(ret);
}

int ifm_get_phymaster(uint32_t ifindex, uint32_t *ms)
{
    int ret = 0;
	struct ipc_mesg_n *pMsgRcv = NULL;

    if(ms == NULL)
    {
        return -1;
    }	

    pMsgRcv = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_IFM, 
                               IPC_TYPE_IFM, IFNET_INFO_PHYMASTER, IPC_OPCODE_GET, ifindex, 0);    
	if(pMsgRcv)
	{
	    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
        {
            memcpy(ms, pMsgRcv->msg_data, sizeof(uint32_t));
        }

        mem_share_free(pMsgRcv, MODULE_ID_IFM);
	}

    return ret;
}

/**
* @brief      获取接口的流控配置
* @param[in ] pifm: 接口信息， flow_ctrl: 流控配置
* @param[out] <+description+>
* @return     <+description+>
* @author     liubo
* @date       2018-4-23
* @note       <+description+>
*/
int ifm_get_flow_control(uint32_t ifindex, struct ifm_flow_control *pFlow)
{
	int ret = -1;
    
	struct ipc_mesg_n * pMsgRcv = NULL;

    pMsgRcv = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_IFM,  
                               IPC_TYPE_IFM, IFNET_INFO_FLOW_CONTROL, IPC_OPCODE_GET, ifindex, 0);

	if(pMsgRcv)
	{
	    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
        {
            ret = 0;
            memcpy(pFlow, pMsgRcv->msg_data, sizeof(struct ifm_flow_control));
        }

        mem_share_free(pMsgRcv, MODULE_ID_IFM);
	}

    return(ret);
}


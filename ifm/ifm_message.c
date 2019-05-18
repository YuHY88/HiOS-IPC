#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <lib/thread.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/devm_com.h>
#include <sys/ipc.h>
#include <devm/devm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "ifm_register.h"
#include "ifm_message.h"
#include "ifm.h"
#include "ifm_qos.h"
#include <ces/stm_if.h>

/*查找下一个接口ifindex，不包括隐藏口*/
static struct ifm_entry *ifm_find_next_ifindex ( uint32_t index, enum IFNET_TYPE if_type )
{
    struct hash_bucket *pbucket = NULL;
    void *cursor = NULL;
    struct ifm_entry *tpifm = NULL;
    uint32_t ifindex = 0;

    zlog_debug ( IFM_DBG_COMMON,"[%s %d] index 0x%0x if_type %d", __FUNCTION__, __LINE__, index, if_type );

    if ( if_type == IFNET_TYPE_MAX )
    {
        if ( index == 0 )
        {
            pbucket = hios_hash_start ( &ifm_table, &cursor );
            if ( pbucket )
            {
                tpifm = pbucket->data;
                if ( tpifm )
                {
                    ifindex = tpifm->ifm.ifindex;
                    if ( ifindex == 0 )
                    {
                        return NULL;
                    }
                    /*不包括隐藏接口*/
                    if ( tpifm->ifm.hide )
                    {
                        tpifm = ifm_find_next_ifindex ( ifindex, if_type );
                        return tpifm;
                    }
                }
                else
                {
                    zlog_err ( "[%s %d] hios_hash_start ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                    return NULL;
                }
            }
        }
        else
        {
            pbucket = hios_hash_find ( &ifm_table, ( void * ) index  );
            if ( pbucket )
            {
                pbucket = hios_hash_next_cursor ( &ifm_table, pbucket );
                if ( pbucket )
                {
                    tpifm = pbucket->data;
                    if ( tpifm )
                    {
                        ifindex = tpifm->ifm.ifindex;
                        for ( ; tpifm->ifm.hide; )
                        {
                            pbucket = hios_hash_next_cursor ( &ifm_table, pbucket );
                            if ( pbucket )
                            {
                                tpifm = pbucket->data;
                                if ( tpifm )
                                {
                                    ifindex = tpifm->ifm.ifindex;
                                    zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_next msg_index 0x%0x interface 0x%0x hide %d", __FUNCTION__, __LINE__,
                                                 index, ifindex, tpifm->ifm.hide  );
                                    return tpifm;
                                }
                                else
                                {
                                    zlog_err ( "[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                                    return NULL;
                                }
                            }
                            else
                            {
                                zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                                return NULL;
                            }
                        }
                        tpifm = pbucket->data;
                        if ( tpifm )
                        {
                            ifindex = tpifm->ifm.ifindex;
                            zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_next msg_index 0x%0x interface 0x%0x ", __FUNCTION__, __LINE__,
                                         index, ifindex );
                            return tpifm;
                        }
                        else
                        {
                            zlog_err ( "[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                            return NULL;
                        }

                    }
                    else
                    {
                        zlog_err ( "[%s %d] hios_hash_start ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                        return NULL;
                    }
                    zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_next msg_index 0x%0x interface 0x%0x ", __FUNCTION__, __LINE__,
                                 index, ifindex );
                }
                else
                {
                    zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                }

            }
            else
            {
                zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
            }

        }
    }
    else if ( if_type == IFNET_TYPE_ETH_SUBPORT )
    {
        if ( index == 0 )
        {
            pbucket = hios_hash_start ( &ifm_table, &cursor );
            if ( pbucket )
            {
                tpifm = pbucket->data;
                if ( tpifm )
                {
                    ifindex = tpifm->ifm.ifindex;
                    if ( ifindex == 0 )
                    {
                        return NULL;
                    }
                    if ( tpifm->ifm.hide|| ! (  ( IFM_TYPE_IS_METHERNET(ifindex) || IFM_TYPE_IS_TRUNK ( ifindex ) )
                                    && IFM_IS_SUBPORT ( ifindex ) ) ) /*when find hide intf get next intf*/
                    {
                        zlog_debug ( IFM_DBG_COMMON,"[%s %d] ifindex 0x%0x if_type %d", __FUNCTION__, __LINE__,
                                     ifindex, if_type );
                        tpifm = ifm_find_next_ifindex ( ifindex, if_type );
                        return tpifm;
                    }
                }
                else
                {
                    zlog_err ( "[%s %d] hios_hash_start ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            pbucket = hios_hash_find ( &ifm_table, ( void * ) index  );
            if ( pbucket )
            {
                pbucket = hios_hash_next_cursor ( &ifm_table, pbucket );
                if ( pbucket )
                {
                    tpifm = pbucket->data;
                    if ( tpifm )
                    {
                        ifindex = tpifm->ifm.ifindex;
                        for ( ;  ; )
                        {
                            //pbucket = hios_hash_next_cursor ( &ifm_table, pbucket );
                            if ( pbucket )
                            {
                                tpifm = pbucket->data;
                                if ( tpifm )
                                {
                                    ifindex = tpifm->ifm.ifindex;
                                    if ( tpifm->ifm.hide
                                            || ! (  ( IFM_TYPE_IS_METHERNET(ifindex) 
                                            		|| IFM_TYPE_IS_TRUNK ( ifindex ) )
                                                    && IFM_IS_SUBPORT ( ifindex ) ) )
                                    {
                                        pbucket = hios_hash_next_cursor ( &ifm_table, pbucket );
                                        continue;
                                    }
                                    zlog_debug ( IFM_DBG_COMMON,"[%s %d] get if_type %d ifindex 0x%0x ", __FUNCTION__, __LINE__, if_type, ifindex );
                                    return tpifm;
                                }
                                else
                                {
                                    zlog_err ( "[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                                    return NULL;
                                }
                            }
                            else
                            {
                                zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                                return NULL;
                            }
                        }


                        tpifm = pbucket->data;
                        if ( tpifm )
                        {
                            ifindex = tpifm->ifm.ifindex;
                            zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_next msg_index 0x%0x interface 0x%0x ", __FUNCTION__, __LINE__,
                                         index, ifindex );
                            return tpifm;
                        }
                        else
                        {
                            zlog_err ( "[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                            return NULL;
                        }

                    }
                    else
                    {
                        zlog_err ( "[%s %d] hios_hash_start ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                        return NULL;
                    }
                    zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_next msg_index 0x%0x interface 0x%0x ", __FUNCTION__, __LINE__,
                                 index, ifindex );
                }
                else
                {
                    zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
                }

            }
            else
            {
                zlog_debug ( IFM_DBG_COMMON,"[%s %d] hios_hash_find ifindex 0x%0x pbucket->data is null", __FUNCTION__, __LINE__, ifindex );
            }

        }
    }

    zlog_debug ( IFM_DBG_COMMON,"[%s %d] get if_type %d ifindex 0x%0x ", __FUNCTION__, __LINE__, if_type, ifindex );

    return tpifm;

}
int ifm_get_port_sort_bulk(uint32_t index, struct port_info * pifm_info, int array_c)
{
	struct ifm_entry *pifm = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cursor;
	int data_num = 0;
	struct listnode * node = NULL;
	struct listnode * nextnode = NULL;
	int * pdata = NULL;
	struct list * plist = NULL;

	plist = &port_common_list;

	if( index == 0 )
	{
		list_delete_all_node( plist );
		IFM_ENTRTY_LOOP( pbucket,cursor )
		{
			pifm = pbucket->data;
			if( pifm == NULL )
			{
				continue;
			}
			if( pifm->ifm.hide == 1 )
			{
				continue;
			}
			listnode_add_sort(plist, pifm);
			
		}
		for( ALL_LIST_ELEMENTS( plist, node, nextnode, pdata ) )
		{
			pifm = (struct ifm_entry *)pdata;
			if( pifm == NULL )
			{
				zlog_err("%s[%d]pifm == NULL\n",__FUNCTION__,__LINE__);
				return 0;
			}
			memcpy(&pifm_info[data_num].ifm,&pifm->ifm,sizeof(struct ifm_info));
			memcpy(&pifm_info[data_num].port_info,&pifm->port_info,sizeof(struct ifm_port));
			strcpy(pifm_info[data_num].alias,pifm->alias);
			data_num ++;
			if( data_num == array_c )
			{
				return data_num;
			}
		}
		return data_num;
	}
	else
	{
		for( ALL_LIST_ELEMENTS(plist, node, nextnode, pdata))
		{
			pifm = ( struct ifm_entry * )pdata;
			if( pifm == NULL )
			{
				zlog_err("%s[%d]pifm is null\n",__FUNCTION__,__LINE__);
				return 0;
			}
			if( pifm->ifm.ifindex >index )
			{
				memcpy(&pifm_info[data_num].ifm,&pifm->ifm,sizeof(struct ifm_info));
				memcpy(&pifm_info[data_num].port_info, & pifm->port_info,sizeof(struct ifm_port));
				strcpy(pifm_info[data_num].alias,pifm->alias );
				if( data_num == array_c )
				{
					return data_num;
				}
			}
		}
		return data_num;
		
	}
	
	return data_num;
}

/* 批量返回 ifindex 后面的 msg_len 个接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */
static int ifm_reply_bulk ( struct ipc_msghdr_n *phdr )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    int i = 0;
    int ret = 0;
    int msg_len = 0;

    msg_len = IPC_MSG_LEN / sizeof ( struct ifm_info );
    struct ifm_info ifm_array[msg_len];

    if ( NULL == phdr )
    {
        zlog_err ( "[%s %d] phdr is null", __FUNCTION__, __LINE__ );
        return ERRNO_FAIL;
    }
    ifindex = phdr->msg_index;
    memset ( ifm_array, 0, msg_len * sizeof ( struct ifm_info ) );
#if 1
    for ( i = 0; i < msg_len ; i++ )
    {
        zlog_debug ( IFM_DBG_COMMON,"[%s %d] get next ifindex by ifindex 0x%0x ", __FUNCTION__, __LINE__,
                     ifindex );
        pifm = ifm_find_next_ifindex ( ifindex, IFNET_TYPE_MAX );

        if ( pifm == NULL )
        {
            break;
        }
        ifindex = pifm->ifm.ifindex;

        zlog_debug ( IFM_DBG_COMMON,"[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs [ifindex %d] act count %d", __FUNCTION__, __LINE__,
                      phdr->msg_index, ifindex, i );

        memcpy ( &ifm_array[i] , &pifm->ifm, sizeof ( struct ifm_info ) );
    }

    zlog_debug ( IFM_DBG_COMMON,"[%s %d] ipc_send_reply_bulk sender_id 0x%0x interface 0x%0x bulk ifindexs act count %d", __FUNCTION__, __LINE__,
                  phdr->sender_id, phdr->msg_index, i );

    /*ret = ipc_send_reply_bulk ( ifm_array, i * sizeof ( struct ifm_info ), i, phdr->sender_id, MODULE_ID_IFM,
                                IPC_TYPE_IFM, IFNET_INFO_MAX, phdr->msg_index );*/
	ret = ipc_send_reply_n2((void *)ifm_array, i * sizeof ( struct ifm_info ),i, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MAX, 0, phdr->msg_index, IPC_OPCODE_REPLY);
    if ( ret < 0 )
    {
        zlog_err ( "[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs ret %d", __FUNCTION__, __LINE__,
                   phdr->msg_index, ret );
        return ERRNO_FAIL;
    }
#else
	i = ifm_get_port_sort_bulk( ifindex, ifm_array, msg_len );

	zlog_debug(IFM_DBG_COMMON,"%s[%d]ipc_send_reply_bulk sender_id 0x%0x intreface 0x%0x bulk ifindex act count %d\n",
		__FUNCTION__,__LINE__,phdr->sender_id,phdr->msg_index,i);
	
	ret = ipc_send_reply_bulk(ifm_array, i * sizeof( struct port_info ), i , phdr->sender_id, MODULE_ID_IFM, 
		IPC_TYPE_IFM, IFNET_INFO_MAX, phdr->msg_index);
	if( ret < 0 )
	{
		zlog_err("%s[%d]ipc_send_reply_bulk intrface 0x%0x bulk ifindex ret %d",__FUNCTION__,__LINE__,
			phdr->msg_index,ret);
		return ERRNO_FAIL;
	}
#endif
    return 0;
}

int ifm_get_snmp_sort_bulk(uint32_t index, struct ifm_snmp_info *pifm_snmp, int array_c)
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;
	int data_num = 0;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
	struct list *plist = NULL;
	struct ifm_port *pspeed = NULL;
	struct ifm_speed *speed_info = NULL;	
	struct ifm_speed *tspeed_info = NULL;
	int pdata_num;
	int i = 0;
	struct ifm_entry * tpifm = NULL;

	plist = &snmp_list;
		
	if(index == 0)
	{
		list_delete_all_node(plist);
		
		IFM_ENTRTY_LOOP ( pbucket, cursor )
		{
			pifm = pbucket->data;
			
			if ( pifm == NULL )
			{
				continue;
			}
			
			if(pifm->ifm.hide == 1)
			{
				continue;
			}
			
			listnode_add_sort ( plist, pifm );
		}
		speed_info = ifm_get_speed_bulk( &index, MODULE_ID_IFM, &pdata_num );
		tspeed_info = speed_info;
		if( (speed_info != NULL) && ( pdata_num != 0 ) )
		{
			for( i = 0; i < pdata_num; i++)
			{
				tpifm = ifm_lookup(speed_info[i].ifindex);
				if( tpifm != NULL )
				{
					tpifm->port_info.autoneg = speed_info[i].autoneg;
					tpifm->port_info.duplex = speed_info[i].duplex;
					tpifm->port_info.speed = speed_info[i].speed;
				}				
			}
			
		}

	}
	
	for ( ALL_LIST_ELEMENTS ( plist, node, nextnode, pdata ) )
	{
		pifm = ( struct ifm_entry * ) pdata;
		if ( pifm == NULL )
		{
			zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
			mem_share_free_bydata(tspeed_info, MODULE_ID_SNMPD);

			return 0;
		}

		if(pifm->ifm.ifindex > index )
		{
			
			memcpy(&pifm_snmp[data_num].ifm, &pifm->ifm, sizeof(struct ifm_info));
			memcpy(&pifm_snmp[data_num].port_info, &pifm->port_info, sizeof(struct ifm_port));
			strcpy( pifm_snmp[data_num].alias,  pifm->alias );
			data_num ++;

			if (data_num == array_c)
			{
				
				mem_share_free_bydata(tspeed_info, MODULE_ID_SNMPD);
				return data_num;
			}
		}
	}
	
	mem_share_free_bydata(tspeed_info, MODULE_ID_SNMPD);
	return data_num;

}


static int ifm_reply_snmp_bulk ( struct ipc_msghdr_n *phdr )
{

    uint32_t ifindex = 0;
//    struct ifm_entry *pifm = NULL;
    int i = 0;
    int ret = 0;
    int msg_len = 0;

    msg_len = IPC_MSG_LEN / sizeof ( struct ifm_snmp_info );

    struct ifm_snmp_info ifm_array[msg_len];

    if ( NULL == phdr )
    {
        zlog_err ( "[%s %d] phdr is null", __FUNCTION__, __LINE__ );
        return ERRNO_FAIL;
    }
    ifindex = phdr->msg_index;
    memset ( ifm_array, 0, msg_len * sizeof ( struct ifm_snmp_info ) );
	
#if 0
    for ( i = 0; i < msg_len ; i++ )
    {
        zlog_debug ( IFM_DBG_COMMON,"[%s %d] get next ifindex by ifindex 0x%0x ", __FUNCTION__, __LINE__,
                     ifindex );

        pifm = ifm_find_next_ifindex ( ifindex, IFNET_TYPE_MAX );

        if ( pifm == NULL )
        {
            break;
        }
        ifindex = pifm->ifm.ifindex;

        zlog_debug ( IFM_DBG_COMMON,"[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs [ifindex %d] act count %d", __FUNCTION__, __LINE__,
                      phdr->msg_index, ifindex, i );

        memcpy ( &ifm_array[i].ifm , &pifm->ifm, sizeof ( struct ifm_info ) );
        memcpy ( &ifm_array[i].port_info , &pifm->port_info, sizeof ( struct ifm_port ) );
    }

    zlog_debug ( IFM_DBG_COMMON,"[%s %d] ipc_send_reply_bulk sender_id 0x%0x interface 0x%0x bulk ifindexs act count %d", __FUNCTION__, __LINE__,
                  phdr->sender_id, phdr->msg_index, i );

    /*ret = ipc_send_reply_bulk ( ifm_array, i * sizeof ( struct ifm_snmp_info ), i, phdr->sender_id, MODULE_ID_IFM,
                                IPC_TYPE_IFM, IFNET_INFO_MAX, phdr->msg_index );*/
	ret = ipc_send_reply_n2((void *)ifm_array, i * sizeof ( struct ifm_snmp_info ),i, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MAX, 0, phdr->msg_index, IPC_OPCODE_REPLY);
    if ( ret < 0 )
    {
        zlog_err ( "[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs ret %d", __FUNCTION__, __LINE__,
                   phdr->msg_index, ret );
        return ERRNO_FAIL;
    }
#endif

	i = ifm_get_snmp_sort_bulk(ifindex, ifm_array, msg_len);

    zlog_debug ( IFM_DBG_COMMON,"[%s %d] ipc_send_reply_bulk sender_id 0x%0x interface 0x%0x bulk ifindexs act count %d\n", __FUNCTION__, __LINE__,
                  phdr->sender_id, phdr->msg_index, i );

    /*ret = ipc_send_reply_bulk ( ifm_array, i * sizeof ( struct ifm_snmp_info ), i, phdr->sender_id, MODULE_ID_IFM,
                                IPC_TYPE_IFM, IFNET_INFO_MAX, phdr->msg_index );*/
     ret = ipc_send_reply_n2((void *)ifm_array, i * sizeof ( struct ifm_snmp_info ),i, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MAX, 0, phdr->msg_index, IPC_OPCODE_REPLY);
    if ( ret < 0 )
    {
        zlog_err ( "[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs ret %d", __FUNCTION__, __LINE__,
                   phdr->msg_index, ret );
        return ERRNO_FAIL;
    }

    return 0;
}
int ifm_get_snmp_sfp_bulk( uint32_t ifindex, struct ifm_snmp_sfp *pifm_snmp, int leng )
{
	struct ifm_entry * pifm = NULL;
	struct hash_bucket * pbucket = NULL;
	uint32_t cursor;
	int data_num = 0;
	struct listnode * node  = NULL;
	struct listnode * nextnode = NULL;
	int * pdata = NULL;
	struct list *plist = NULL;
	unsigned int devType = 0;

	plist = &snmp_sfp_list;
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devType );
	
	if ( ifindex == 0 )
	{
		list_delete_all_node( plist );
		IFM_ENTRTY_LOOP( pbucket, cursor)
		{
			pifm = pbucket->data;
			if ( pifm == NULL )
			{
				continue;
			}
			if ( pifm->ifm.hide == 1)
			{
				continue;
			}
			listnode_add_sort( plist, pifm );
		}
		for( ALL_LIST_ELEMENTS( plist, node, nextnode, pdata ) )
		{
			pifm = ( struct ifm_entry*)pdata;
			if ( pifm == NULL )
			{
				zlog_err("%s[%d] pifm is null\n",__FUNCTION__,__LINE__ );
				return 0;
			}
			if ( pifm->ifm.hide == 1 )
			{
				continue;
			}
			if ( ( pifm->ifm.type != IFNET_TYPE_ETHERNET ) && ( pifm->ifm.type != IFNET_TYPE_GIGABIT_ETHERNET)
					&& ( pifm->ifm.type != IFNET_TYPE_XGIGABIT_ETHERNET ) && (pifm->ifm.type != IFNET_TYPE_STM) )
			{   			    
				continue;
			}
			if (  pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT  )
			{
				continue;
			}
			if( ( devType != ID_HT157 ) && ( devType != ID_HT158 ))
			{
				if ( pifm->port_info.fiber != IFNET_FIBERTYPE_FIBER )
				{
					continue;
				}
				
			}
			
			else if( ( devType == ID_HT157 ) || ( devType == ID_HT158 ))
			{
			
				if ( ( pifm->port_info.fiber != IFNET_FIBERTYPE_FIBER ) && ( pifm->port_info.fiber != IFNET_FIBERTYPE_COMBO) )
				{
					continue;
				}
				if( pifm->port_info.fiber == IFNET_FIBERTYPE_COMBO )
				{
					if( pifm->port_info.fiber_sub != IFNET_FIBERTYPE_FIBER )
					{
						continue;
					}
				}
				
			}
			pifm_snmp[data_num].ifindex = pifm->ifm.ifindex;
			pifm_snmp[data_num].sfp_als = pifm->ifm.sfp_als;
			pifm_snmp[data_num].sfp_tx  = pifm->ifm.sfp_tx;
			pifm_snmp[data_num].sfp_on  = pifm->ifm.sfp_on;
			pifm_snmp[data_num].sfp_off = pifm->ifm.sfp_off;

			data_num ++;
			if ( data_num == leng )
			{
				return data_num;
			}			
		}
		return data_num;
	}
	else
	{
		for( ALL_LIST_ELEMENTS( plist, node, nextnode, pdata ) )
		{
			pifm = ( struct ifm_entry * )pdata;
			if( pifm == NULL )
			{
				zlog_err("%s[%d] pifm is null\n",__FUNCTION__,__LINE__ );
				return 0;
			}
			if ( pifm->ifm.hide == 1 )
			{
				continue;
			}
			if ( ( pifm->ifm.type != IFNET_TYPE_ETHERNET ) && ( pifm->ifm.type != IFNET_TYPE_GIGABIT_ETHERNET)
					&& ( pifm->ifm.type != IFNET_TYPE_XGIGABIT_ETHERNET ) && (pifm->ifm.type != IFNET_TYPE_STM) )
			{
				continue;
			}
			
			if (  pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT )
			{
				continue;
			}
			
			if( ( devType != ID_HT157 ) && ( devType != ID_HT158 ))
			{
				if ( pifm->port_info.fiber != IFNET_FIBERTYPE_FIBER )
				{
					continue;
				}
				
			}
					
			else if( ( devType == ID_HT157 ) || ( devType == ID_HT158 ))
			{
			
				if ( ( pifm->port_info.fiber != IFNET_FIBERTYPE_FIBER ) && ( pifm->port_info.fiber != IFNET_FIBERTYPE_COMBO) )
				{
					continue;
				}
				if( pifm->port_info.fiber == IFNET_FIBERTYPE_COMBO )
				{
					if( pifm->port_info.fiber_sub != IFNET_FIBERTYPE_FIBER )
					{
						continue;
					}
				}
				
			}
			if ( pifm->ifm.ifindex > ifindex )
			{
				pifm_snmp[data_num].ifindex = pifm->ifm.ifindex;
				pifm_snmp[data_num].sfp_als = pifm->ifm.sfp_als;
				pifm_snmp[data_num].sfp_tx  = pifm->ifm.sfp_tx;
				pifm_snmp[data_num].sfp_on  = pifm->ifm.sfp_on;
				pifm_snmp[data_num].sfp_off = pifm->ifm.sfp_tx;

				data_num++;

				if ( data_num == leng )
				{
					return data_num;
				}				
			}
		}
		return data_num;
	}
	return data_num;	
}
static int ifm_reply_snmp_sfp( struct ipc_msghdr_n *phdr )
{
	uint32_t ifindex = 0;
	int ret = 0;
	int msg_len = 0;
	int count = 0;

	msg_len = IPC_MSG_LEN / sizeof( struct ifm_snmp_sfp );
	struct ifm_snmp_sfp ifm_sfp[msg_len];

	if ( phdr ==  NULL )
	{
		zlog_err ( "[%s %d] phdr is null", __FUNCTION__, __LINE__ );
        return ERRNO_FAIL;		
	}
	ifindex = phdr->msg_index;
	memset( ifm_sfp, 0, msg_len * sizeof( struct ifm_snmp_sfp ) );
	count = ifm_get_snmp_sfp_bulk( ifindex, ifm_sfp, msg_len );
	
	zlog_debug(IFM_DBG_COMMON,"%s[%d] ipc_send_reply_bulk sender_id 0x%0x ifindex 0x%0x act count %d\n",__FUNCTION__,
		__LINE__, phdr->sender_id,phdr->msg_index, count);
	
	/*ret = ipc_send_reply_bulk( ifm_sfp, count*sizeof( struct ifm_snmp_sfp), count, phdr->sender_id, MODULE_ID_IFM, 
		IPC_TYPE_IFM, IFNET_INFO_SFP, phdr->msg_index );*/
	ret = ipc_send_reply_n2((void *)ifm_sfp, count*sizeof( struct ifm_snmp_sfp),count, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_SFP, 0, phdr->msg_index, IPC_OPCODE_REPLY);
	if ( ret < 0 )
	{
		zlog_err ( "[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs ret %d", __FUNCTION__, __LINE__,
                   phdr->msg_index, ret );
        return ERRNO_FAIL;
	}
	 return 0;	
}
int ifm_get_snmp_subencap_bulk( uint32_t ifindex, struct ifm_snmp_subencap* pifm_snmp, int leng )
{
	struct ifm_entry * pifm = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t cursor;
	int data_num = 0;
	struct listnode * node = NULL;
	struct listnode *nextnode = NULL;
	int * pdata = NULL;
	struct list * plist = NULL;

	plist = &snmp_subencap_list;

	if ( ifindex == 0 )
	{
		list_delete_all_node( plist );
		IFM_ENTRTY_LOOP( pbucket, cursor )
		{
			pifm = pbucket->data;
			if ( pifm == NULL )
			{
				continue;
			}
			if ( pifm->ifm.hide == 1 )
			{
				continue;
			}
			listnode_add_sort( plist , pifm );
			
		}
		for( ALL_LIST_ELEMENTS(plist, node, nextnode, pdata ) )
		{
			pifm = ( struct ifm_entry * )pdata;
			if ( pifm == NULL )
			{
				zlog_err("%s[%d] pifm is null\n",__FUNCTION__,__LINE__ );
				return -1;
			}
			if ( pifm->ifm.hide == 1 )
			{
				continue;
			}

			
			if (  pifm->ifm.sub_type != IFNET_SUBTYPE_SUBPORT )
			{
				continue;
			}
	
			if(pifm->ifm.encap.type == IFNET_ENCAP_INVALID )
			{	
				continue;
			}
			memcpy( &pifm_snmp[data_num].ifm, &pifm->ifm, sizeof( struct ifm_info ) );
			data_num++;

			if ( data_num == leng )
			{
				return data_num;
			}
		}
		return data_num;
	}
	else
	{
		for(ALL_LIST_ELEMENTS(plist, node, nextnode, pdata ) )
		{
			pifm = (struct ifm_entry*)pdata;
			if ( pifm == NULL )
			{
				zlog_err("%s[%d]pifm is null\n",__FUNCTION__,__LINE__);
				return -1;
			}
			if ( pifm->ifm.hide == 1 )
			{
				continue;
			}

			
			if (  pifm->ifm.sub_type != IFNET_SUBTYPE_SUBPORT )
			{
				continue;
			}
	
			if(pifm->ifm.encap.type == IFNET_ENCAP_INVALID )
			{	
				continue;
			}
			if ( pifm->ifm.ifindex > ifindex )
			{
				memcpy( &pifm_snmp[data_num].ifm, &pifm->ifm, sizeof( struct ifm_info ) );
				data_num++;
			
				if ( data_num == leng )
				{
					return data_num;
				}
			}
		}
		return data_num;
	}
	return data_num;
}
static int ifm_reply_snmp_subencap( struct ipc_msghdr_n*phdr )
{
	uint32_t ifindex = 0;
	int ret = 0;
	int msg_len = 0;
	int count = 0;

	msg_len = IPC_MSG_LEN / sizeof( struct ifm_snmp_subencap );

	struct ifm_snmp_subencap ifm_subencap[msg_len];

	if ( phdr == NULL )
	{
		zlog_err("%s[%d] phdr is null\n",__FUNCTION__,__LINE__ );
		return ERRNO_FAIL;
	}
	ifindex = phdr->msg_index;
	memset( ifm_subencap, 0, msg_len * sizeof( struct ifm_snmp_subencap ) );
	count = ifm_get_snmp_subencap_bulk( ifindex, ifm_subencap,msg_len );
	
	zlog_debug(IFM_DBG_COMMON,"%s[%d] ipc_send_reply_bulk sender_id 0x%0x ifindex 0x%0x act count %d\n",__FUNCTION__,
		__LINE__, phdr->sender_id,phdr->msg_index, count);
	for( int i =0; i < count ;i++ )
	{
		zlog_debug(IFM_DBG_COMMON,"%s[%d]send to snmp ifindex= 0x%0x num %d\n",__FUNCTION__,__LINE__,ifm_subencap[i].ifm.ifindex, count );
	}
	/*ret = ipc_send_reply_bulk( ifm_subencap, msg_len*sizeof(struct ifm_snmp_subencap),count,phdr->sender_id,
		MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_ENCAP, phdr->msg_index );*/
	ret = ipc_send_reply_n2((void *)ifm_subencap, msg_len*sizeof(struct ifm_snmp_subencap),count, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_ENCAP, 0, phdr->msg_index, IPC_OPCODE_REPLY);
	if ( ret < 0 )
	{
		zlog_err ( "[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs ret %d", __FUNCTION__, __LINE__,
                   phdr->msg_index, ret );
        return ERRNO_FAIL;
	}
	return 0;
	
}
int ifm_get_snmp_counter_bulk(uint32_t ifindex, struct ifm_snmp* ifm_info,int msg_len)
{
	struct ifm_entry * pifm = NULL;
	struct hash_bucket * pbucket = NULL;
	uint32_t cursor = 0;
	int data_num = 0;
	struct listnode * node = NULL;
	struct listnode * nextnode = NULL;
	int * pdata = NULL;
	struct list * plist = NULL;
	struct ifm_counter * counter = NULL;

	plist = &snmp_info_list;

	if( ifindex == 0 )
	{
		list_delete_all_node(plist);
		IFM_ENTRTY_LOOP( pbucket, cursor)
		{
			pifm = pbucket->data;
			if( pifm == NULL )
			{
				continue;
				
			}
			if( pifm->ifm.hide == 1 )
			{
				continue;
			}
			
			listnode_add_sort(plist, pifm);
		}
		for(ALL_LIST_ELEMENTS(plist, node, nextnode, pdata))
		{
			if( pdata )
			{
				pifm = ( struct ifm_entry *)pdata;
				if( pifm == NULL )
				{
					zlog_err("%s[%d] pifm is null\n",__FUNCTION__,__LINE__ );
					return -1;
				}
				if( pifm->ifm.hide == 1 )
				{
					continue;
				}
				memcpy( &ifm_info[data_num].ifm, &pifm->ifm, sizeof(struct ifm_info));
				memcpy( &ifm_info[data_num].port_info, &pifm->port_info, sizeof(struct ifm_port));
				strcpy( &ifm_info[data_num].alias,  pifm->alias );
				
				
				if( IFM_TYPE_IS_ETHERNET_PHYSICAL(pifm->ifm.ifindex))
				{
					memcpy(&ifm_info[data_num].pcounter,pifm->pcounter,sizeof(struct ifm_counter));	
					
				}
				else if(IFM_TYPE_IS_ETHERNET_SUBPORT(pifm->ifm.ifindex))
				{
					if( ( pifm->pcounter == NULL ) || ( pifm->ifm.statistics == IFNET_STAT_DISABLE ))
					{
						memset(&ifm_info[data_num].pcounter , 0 ,sizeof(struct ifm_counter));
					}
					else
					{
						memcpy(&ifm_info[data_num].pcounter,pifm->pcounter,sizeof(struct ifm_counter));		
					}
				}
				else
				{
					memset(&ifm_info[data_num].pcounter , 0 ,sizeof(struct ifm_counter));
				}
				data_num++;				
				
				if(data_num == msg_len)
				{					
					return data_num;
				}		
				
			}
			
		}
		
	}
	else
	{
		for(ALL_LIST_ELEMENTS(plist, node, nextnode, pdata))
		{
			if(pdata)
			{
				pifm = (struct ifm_entry *)pdata;
				if( pifm == NULL )
				{
					zlog_err("%s[%d]pifm is null\n",__FUNCTION__,__LINE__);
					return -1;
				}
				if( pifm->ifm.hide == 1 )
				{
					continue;
				}
				if( pifm->ifm.ifindex > ifindex )
				{
					memcpy( &ifm_info[data_num].ifm, &pifm->ifm, sizeof(struct ifm_info));
					memcpy( &ifm_info[data_num].port_info, &pifm->port_info, sizeof(struct ifm_port));
					strcpy( &ifm_info[data_num].alias,  pifm->alias );
				
					if( IFM_TYPE_IS_ETHERNET_PHYSICAL(pifm->ifm.ifindex))
					{
						memcpy(&ifm_info[data_num].pcounter,pifm->pcounter,sizeof(struct ifm_counter));						
					
					}
					else if(IFM_TYPE_IS_ETHERNET_SUBPORT(pifm->ifm.ifindex))
					{
						if( ( pifm->pcounter == NULL ) || ( pifm->ifm.statistics == IFNET_STAT_DISABLE ))
						{
							memset(&ifm_info[data_num].pcounter , 0 ,sizeof(struct ifm_counter));
						}
						else
						{
							memcpy(&ifm_info[data_num].pcounter,pifm->pcounter,sizeof(struct ifm_counter));		
						}
					}
					else
					{
						memset(&ifm_info[data_num].pcounter , 0 ,sizeof(struct ifm_counter));
					}
					data_num++;				
					
					if(data_num == msg_len)
					{
						return data_num;
					}	
					
				}
								
			}
		}
		
	}		
	return data_num;
}

int ifm_reply_snmp_counter( struct ipc_msghdr_n* phdr)
{
	uint32_t ifindex = 0;
	int ret = 0;
	int msg_len = 0;
	int count = 0;

	msg_len = IPC_MSG_LEN /(sizeof(struct ifm_snmp));

	struct ifm_snmp  ifm_info[msg_len];

	if( phdr == NULL )
	{
		zlog_err("%s[%d]phdr is null\n",__FUNCTION__,__LINE__);
		return ERRNO_FAIL;
	}

	ifindex = phdr->msg_index;

	memset(ifm_info, 0, msg_len*sizeof(struct ifm_snmp));
	count = ifm_get_snmp_counter_bulk(ifindex,ifm_info,msg_len);
	
	zlog_debug(IFM_DBG_COMMON,"%s[%d] ipc_send_reply_bulk sender_id 0x%0x ifindex 0x%0x act count %d\n",__FUNCTION__,
		__LINE__, phdr->sender_id,phdr->msg_index, count);
	
	for( int i = 0; i < count; i++)
	{
		zlog_debug (IFM_DBG_COMMON,"%s[%d] ifindex 0x%0x Input  Total: %llu bytes, %llu packets\n",__FUNCTION__,__LINE__,
						ifm_info[i].ifm.ifindex,ifm_info[i].pcounter.rx_bytes, ifm_info[i].pcounter.rx_packets);
        zlog_debug (IFM_DBG_COMMON,"%s[%d] ifindex 0x%0x Output Total: %llu bytes, %llu packets\n",__FUNCTION__,__LINE__,
						ifm_info[i].ifm.ifindex,ifm_info[i].pcounter.tx_bytes, ifm_info[i].pcounter.tx_packets);
		
	}
	//ret = ipc_send_reply_bulk( ifm_info, count * sizeof(struct ifm_snmp), count, 
		//phdr->sender_id,MODULE_ID_IFM, IPC_TYPE_IFM, IFNET_INFO_COUNTER, phdr->msg_index);
	ret = ipc_send_reply_n2((void *)ifm_info, count * sizeof ( struct ifm_snmp_info ),count, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_COUNTER, 0, phdr->msg_index, IPC_OPCODE_REPLY);	
	if ( ret < 0 )
    {
        zlog_err ( "[%s %d] ipc_send_reply_bulk interface 0x%0x bulk ifindexs ret %d", __FUNCTION__, __LINE__,
                   phdr->msg_index, ret );
        return ERRNO_FAIL;
    }	
	
	return 0;	
	
}

/* 处理接口管理的 get 消息 */
int ifm_rcv_getif_msg ( struct ipc_mesg_n *pmesg )
{
    struct ipc_msghdr_n *phdr = NULL;
    struct ifm_entry *pifm = NULL;
    struct ifm_entry *tpifm = NULL;
    uint8_t status;
    uint8_t if_type = 0;
    int ret = ERRNO_SUCCESS;

    if ( NULL == pmesg )
    {
        zlog_err ( "[%s %d] pmesg is null", __FUNCTION__, __LINE__ );
        return ERRNO_FAIL;
    }

    phdr = & ( pmesg->msghdr );
    if ( phdr->msg_subtype == IFNET_INFO_NEXT_IFINDEX ) /*snmp get next ifidex*/
    {
        if_type = pmesg->msg_data[0];

        zlog_debug (IFM_DBG_COMMON, "[%s %d] if_type %d pmesg->msg_data[0] %d,  phdr->msg_index 0x%0x",
                     __FUNCTION__, __LINE__, if_type, pmesg->msg_data[0], phdr->msg_index );

        tpifm = ifm_find_next_ifindex ( phdr->msg_index, if_type );

        if ( tpifm == NULL )
        {
            //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
            //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
            ret = ifm_msg_send_noack(0, phdr);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ret;
            }
        }
        else
        {
            /*ret = ipc_send_reply ( &tpifm->ifm.ifindex, sizeof ( tpifm->ifm.ifindex ), phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void*)&tpifm->ifm.ifindex, sizeof ( tpifm->ifm.ifindex ), 1, phdr->sender_id, 
				MODULE_ID_IFM,  IPC_TYPE_IFM, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_REPLY);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_reply ifindex 0x%0x interface 0x%0x all info erro ret %d", __FUNCTION__, __LINE__,
                           tpifm->ifm.ifindex, phdr->msg_index, ret );

                return ret;
            }
        }
    }
	else if ( phdr->msg_subtype == IFNET_INFO_PORT_COUNT )
	{
		/*ret = ipc_send_reply ( & ( ifm_table.num_entries ), sizeof ( ifm_table.num_entries ), 
				phdr->sender_id, MODULE_ID_IFM, IPC_TYPE_IFM, IFNET_INFO_PORT_COUNT, phdr->msg_index );*/
		ret = ipc_send_reply_n2((void *) & ( ifm_table.num_entries ), sizeof ( ifm_table.num_entries ),1, phdr->sender_id, 
		     MODULE_ID_IFM,IPC_TYPE_IFM, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_REPLY);
		
		zlog_debug(IFM_DBG_COMMON,"%s[%d]ifm_table.num_entries %d",__FUNCTION__,__LINE__,ifm_table.num_entries);
		if ( ret < 0 )
		{
			zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x all info erro ret %d", __FUNCTION__, __LINE__,
					   phdr->msg_index, ret );
	
			return ERRNO_FAIL;
		}
	
	}
    else
    {
        pifm = ifm_lookup ( phdr->msg_index );
        if ( pifm == NULL )
        {
            zlog_debug ( IFM_DBG_COMMON,"[%s %d] get interface 0x%0x pifm erro ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, ret );

            //ret = ipc_send_noack ( ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_IFM,
            //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
            ret = ifm_msg_send_noack(ERRNO_NOT_FOUND, phdr);
            if ( ret < 0 )
            {
                zlog_debug (IFM_DBG_COMMON, "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
            }

            return ERRNO_FAIL;
        }

        if ( phdr->msg_subtype == IFNET_INFO_STATUS )
            /*收到获取接口状态消息*/
        {
            status = pifm->ifm.status;
            /*ret = ipc_send_reply ( &status, sizeof ( pifm->ifm.status ), phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, IFNET_INFO_STATUS, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void *) &status, sizeof ( pifm->ifm.status ),1, phdr->sender_id, 
		     			MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_STATUS, 0, phdr->msg_index, IPC_OPCODE_REPLY);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x status erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ERRNO_FAIL;
            }
        }
        else if ( phdr->msg_subtype == IFNET_INFO_MAC )
            /*收到获取接口 mac 消息*/
        {
            if ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT )
            {
                pifm = ifm_lookup ( pifm->ifm.parent );
                if ( !pifm )
                {
                    zlog_err ( "[%s %d] ifm_lookup interface 0x%0x ", __FUNCTION__, __LINE__,
                               phdr->msg_index );
                    return ERRNO_FAIL;
                }
            }

            /*ret = ipc_send_reply ( pifm->ifm.mac, 6, phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, IFNET_INFO_MAC, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void *) pifm->ifm.mac, 6,1, phdr->sender_id, 
		     			MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MAC, 0, phdr->msg_index, IPC_OPCODE_REPLY);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x mac erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ERRNO_FAIL;
            }

        }
        else if ( phdr->msg_subtype == IFNET_INFO_MODE )
            /*收到获取接口 mode 消息*/
        {
           /* ret = ipc_send_reply ( & ( pifm->ifm.mode ), sizeof ( pifm->ifm.mode ), phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, IFNET_INFO_MODE, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void *) & ( pifm->ifm.mode ), sizeof ( pifm->ifm.mode ),1, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MODE, 0, phdr->msg_index, IPC_OPCODE_REPLY);
			if ( ret < 0 )
			{
				zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x mode erro ret %d", __FUNCTION__, __LINE__,
						phdr->msg_index, ret );

                return ERRNO_FAIL;
            }
        }
        else if ( phdr->msg_subtype == IFNET_INFO_MTU )
            /*收到获取接口 mtu 消息*/
        {
            /*ret = ipc_send_reply ( & ( pifm->ifm.mtu ), sizeof ( pifm->ifm.mtu ), phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, IFNET_INFO_MTU, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void *) & ( pifm->ifm.mtu ), sizeof ( pifm->ifm.mtu ),1, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MTU, 0, phdr->msg_index, IPC_OPCODE_REPLY);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x mtu erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );

                return ERRNO_FAIL;
            }
        }
        else if ( phdr->msg_subtype == IFNET_INFO_MAX )
            /*收到获取接口 接口全部信息 消息*/
        {

            /*ret = ipc_send_reply ( & ( pifm->ifm ), sizeof ( pifm->ifm ), phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, IFNET_INFO_MAX, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void *) &( pifm->ifm ), sizeof ( pifm->ifm ),1, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_MAX, 0, phdr->msg_index, IPC_OPCODE_REPLY);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x all info erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );

                return ERRNO_FAIL;
            }
        }
        else if ( phdr->msg_subtype == IFNET_INFO_PORT_INFO )
            /*收到获取接物理端口的信息消息*/
        {
            /*ret = ipc_send_reply ( & ( pifm->port_info ), sizeof ( pifm->port_info ), phdr->sender_id, MODULE_ID_IFM,
                                   IPC_TYPE_IFM, IFNET_INFO_PORT_INFO, phdr->msg_index );*/
			ret = ipc_send_reply_n2((void *) & ( pifm->port_info ), sizeof ( pifm->port_info ),1, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_IFM, IFNET_INFO_PORT_INFO, 0, phdr->msg_index, IPC_OPCODE_REPLY);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_reply interface 0x%0x all info erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );

                return ERRNO_FAIL;
            }
        }
    }
    return ret;
} 
      
/* 处理设备管理的消息 */
int ifm_rcv_devm_msg ( struct ipc_mesg_n *pmesg )
{
    struct ipc_msghdr_n *phdr = NULL;
    struct ifm_common *pif = NULL;
    struct ifm_common *tpif = NULL;
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = 0;
	//uint32_t tifindex = 0;
    int ret = 0;
    struct ifm_entry *tpifm = NULL;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
	int data_num;
	int port_count;
    struct ifm_event event;
  //struct ipc_msghdr_n	msghdr;

    if ( NULL == pmesg )
    {
        zlog_err ( "[%s %d] pmesg is null", __FUNCTION__, __LINE__ );
        return ERRNO_FAIL;
    }

    phdr = & ( pmesg->msghdr );

	zlog_debug ( IFM_DBG_COMMON,"[%s %d] opcode %d msg_subtype %d", __FUNCTION__, __LINE__,
		phdr->opcode, phdr->msg_subtype);
    if ( phdr->opcode == IPC_OPCODE_EVENT )
    {
        if ( phdr->msg_subtype == DEV_EVENT_PORT_ADD )
            /*创建接口*/
        {
            pif = ( struct ifm_common * ) pmesg->msg_data;

            if ( NULL == pif )
            {
                zlog_err ( "[%s %d] pif is null", __FUNCTION__, __LINE__ );
                return ERRNO_FAIL;
            }

	
            zlog_debug ( IFM_DBG_COMMON,"[%s %d] ret %d unit %d slot %d port %d subport %d type %d subtype %d", __FUNCTION__, __LINE__,   ret,
                         pif->usp.unit, pif->usp.slot, pif->usp.port, pif->usp.sub_port,
                         pif->usp.type, pif->usp.subtype );

            if ( pif->usp.type == IFNET_TYPE_CLOCK )
                /*创建时钟接口*/
            {
                ret = ifm_create_clock ( pif );
			
                zlog_debug ( IFM_DBG_COMMON,"Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x ret %d",
                             pif->mac[0], pif->mac[1], pif->mac[2], pif->mac[3], pif->mac[4], pif->mac[5], ret );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ret %d unit %d slot %d port %d subport %d type %d subtype %d  erro ret %d", __FUNCTION__, __LINE__,    ret,
                               pif->usp.unit, pif->usp.slot, pif->usp.port, pif->usp.sub_port,
                               pif->usp.type, pif->usp.subtype, ret );

                    zlog_err ( "[%s %d] ifm_create_physical  erro ret %d", __FUNCTION__, __LINE__,  ret );
                    return -1;
                }
            }
            else
                /*创建物理接口*/
            {
                ret = ifm_create_physical ( pif );			

                zlog_debug ( IFM_DBG_COMMON,"Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x ret %d",
                             pif->mac[0], pif->mac[1], pif->mac[2], pif->mac[3], pif->mac[4], pif->mac[5], ret );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ret %d unit %d slot %d port %d subport %d type %d subtype %d  erro ret %d", __FUNCTION__, __LINE__,    ret,
                               pif->usp.unit, pif->usp.slot, pif->usp.port, pif->usp.sub_port,
                               pif->usp.type, pif->usp.subtype, ret );

                    zlog_err ( "[%s %d] ifm_create_physical  erro ret %d", __FUNCTION__, __LINE__,  ret );
                    return -1;
                }
            }
        }
        else if ( phdr->msg_subtype == DEV_EVENT_PORT_DELETE )
            /*删除接口*/
        {
            pif = ( struct ifm_common * ) pmesg->msg_data;
			
			if ( NULL == pif )
            {
                zlog_err ( "[%s %d] pif is null", __FUNCTION__, __LINE__ );
                return ERRNO_FAIL;
            }
			
            ifindex = ifm_get_ifindex_by_usp ( &pif->usp );
            zlog_debug ( IFM_DBG_COMMON,"%s[%d]delete ifindex=0x%0x hide %d\n",__FUNCTION__,__LINE__,ifindex, pif->port_info.hide );
            zlog_debug ( IFM_DBG_COMMON,"[%s %d]  ret %d unit %d slot %d port %d subport %d type %d subtype %d", __FUNCTION__, __LINE__,   ret,
                         pif->usp.unit, pif->usp.slot, pif->usp.port, pif->usp.sub_port,
                         pif->usp.type, pif->usp.subtype );

            if ( ifindex == 0 )
            {
                zlog_err ( "[%s %d] unit %d slot %d port %d subport %d type %d subtype %d  erro ret %d", __FUNCTION__, __LINE__,
                           pif->usp.unit, pif->usp.slot, pif->usp.port, pif->usp.sub_port,
                           pif->usp.type, pif->usp.subtype, ret );
                return -1;

            }
            ret = ifm_delete ( ifindex );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] unit %d slot %d port %d subport %d type %d subtype %d  erro ret %d", __FUNCTION__, __LINE__,
                           pif->usp.unit, pif->usp.slot, pif->usp.port, pif->usp.sub_port,
                           pif->usp.type, pif->usp.subtype, ret );
                return -1;
            }

        }
		else if ( phdr->msg_subtype == DEV_EVENT_SLOT_ADD_FINISH )
		{
	       //ifm_msg_send_com ( &msghdr, NULL );
            
            ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_VTY, MODULE_ID_IFM, IPC_TYPE_IFM, IFNET_EVENT_IF_ADD_FINISH, IPC_OPCODE_EVENT, 0);

			zlog_debug( IFM_DBG_COMMON,"[%s %d] ifm_msg_send_com DEV_EVENT_SLOT_ADD_FINISH\n", __FUNCTION__, __LINE__ );
		}
    }
	else if ( phdr->opcode == IPC_OPCODE_UPDATE ) /*更新接口*/
	{		
		data_num = phdr->data_num;		
		if ( phdr->msg_subtype == DEVM_INFO_PORT )
		{
			tpif = ( struct ifm_common * ) pmesg->msg_data;

			if ( NULL == tpif )
			{
				zlog_err ( "[%s %d] pif is null", __FUNCTION__, __LINE__ );
				return ERRNO_FAIL;
			}
			for( port_count=0; port_count<data_num;port_count++)
			{		
					
				ifindex = ifm_get_ifindex_by_usp ( &tpif[port_count].usp );				
				zlog_debug ( IFM_DBG_COMMON," [%s[%d] ifindex=0x%x, %d   hide %d\n", __FUNCTION__, __LINE__, ifindex,ifindex, tpif[port_count].port_info.hide );
				zlog_debug ( IFM_DBG_COMMON,"[%s %d] unit %d slot %d port %d subport %d type %d subtype %d", __FUNCTION__, __LINE__,   
	   			tpif[port_count].usp.unit, tpif[port_count].usp.slot, tpif[port_count].usp.port, tpif[port_count].usp.sub_port,
	   			tpif[port_count].usp.type, tpif[port_count].usp.subtype );
				pifm = ifm_lookup2 ( ifindex );
				if ( !pifm )
				{
					zlog_err ( "[%s %d] ifm_lookup interface 0x%0x parent erro ret %d", __FUNCTION__, __LINE__,ifindex , ret );
					zlog_err ( "[%s %d] unit %d slot %d port %d subport %d type %d subtype %d  erro ret %d", __FUNCTION__, __LINE__,   
	     				tpif[port_count].usp.unit, tpif[port_count].usp.slot, tpif[port_count].usp.port, tpif[port_count].usp.sub_port,
	     				tpif[port_count].usp.type, tpif[port_count].usp.subtype, ret );					
					return -1;
				}
				zlog_debug ( IFM_DBG_COMMON,"%s[%d]ifindex=0x%0x hide %d\n", ifindex, tpif[port_count].port_info.hide );
				/*闅愯棌鎺ュ彛*/
				if ( tpif[port_count].port_info.hide )
				{
					/*设置父接口devm 操作接口标志位*/
					pifm->devm_flag = 1;
					/*先隐藏子接口*/
					if ( pifm->sonlist )
					{
						for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
						{
	 						 tpifm = ( struct ifm_entry * ) pdata;
	 					 	tpifm->port_info.hide = tpif[port_count].port_info.hide;
	  						tpifm->ifm.hide         = tpif[port_count].port_info.hide;
						}
					}

					/*后隐藏父接口*/
					pifm->port_info.hide = tpif[port_count].port_info.hide;
					pifm->ifm.hide = tpif[port_count].port_info.hide;
				}
			else/*恢复接口*/
			{				
				/*设置父接口devm 操作接口标志位*/
				pifm->devm_flag = 0;
				/*先恢复父接口*/
				pifm->port_info.hide = tpif[port_count].port_info.hide;
				pifm->ifm.hide = tpif[port_count].port_info.hide;	
				/*恢复接口shutdown配置(需要恢复FPGA)*/
                		//ifm_set_shutdown ( pifm, pifm->ifm.shutdown );
				
				memset ( &event, 0, sizeof ( event ) );
				event.event   = IFNET_EVENT_IF_HIDE_RECOVER;
				event.ifindex = pifm->ifm.ifindex;
				ifm_event_notify ( IFNET_EVENT_IF_HIDE_RECOVER, &event );
				/*后恢复子接口*/				
				if ( pifm->sonlist )
				{
					for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
					{
	 				 	tpifm = ( struct ifm_entry * ) pdata;
	  					tpifm->port_info.hide =tpif[port_count].port_info.hide;
	  					tpifm->ifm.hide  = tpif[port_count].port_info.hide;
	 				 	/*通知隐藏恢复消息*/
	 				 	memset ( &event, 0, sizeof ( event ) );
					  	event.event   = IFNET_EVENT_IF_HIDE_RECOVER;
	 				 	event.ifindex = tpifm->ifm.ifindex;
	 				 	ifm_event_notify ( IFNET_EVENT_IF_HIDE_RECOVER, &event );
					}
				}
			}
		}
	}
 }
		 return ret;
}

static int ifm_reply_qos_bulk ( struct ipc_msghdr_n *phdr )
{
	int data_num = IPC_MSG_LEN/sizeof(struct qos_entry_snmp);
    struct qos_entry_snmp qos_array[data_num];
    uint32_t ifindex = 0;
    int ret = 0;
    
    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }
    
    ifindex = phdr->msg_index;
    memset ( qos_array, 0, data_num * sizeof ( struct qos_entry_snmp ) );
	
	ret = ifm_qos_get_bulk(ifindex, qos_array);
    if (ret > 0)
    {
        /*ret = ipc_send_reply_bulk(qos_array, ret*sizeof(struct qos_entry_snmp), ret, phdr->sender_id,
                                MODULE_ID_IFM, IPC_TYPE_QOSIF, phdr->msg_subtype, phdr->msg_index);*/
		ret = ipc_send_reply_n2((void *)qos_array, ret*sizeof(struct qos_entry_snmp),ret, phdr->sender_id, 
					MODULE_ID_IFM,IPC_TYPE_QOSIF, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_REPLY);
    }
    else
    {
        //ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_IFM, IPC_TYPE_QOSIF,
        //                        phdr->msg_subtype, phdr->msg_index);
        ret = ifm_msg_send_noack(ERRNO_NOT_FOUND, phdr);
		
    }

    return ret;
}


int ifm_rcv_qosif_msg ( struct ipc_mesg_n *pmesg )
{
	struct ipc_msghdr_n *phdr = NULL;
	int ret = 0;

    phdr = & ( pmesg->msghdr );
    
    if (phdr->opcode == IPC_OPCODE_GET_BULK)
    {
		if (QOS_INFO_MAX == phdr->msg_subtype)
        {	
            ret = ifm_reply_qos_bulk( phdr);
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "ifm_reply_qos_bulk failed, ret %d\n", ret );
		        return ret;
		    }
        }
    }
    
    return ERRNO_SUCCESS;
}
int ifm_rcv_stm_msg(struct ipc_mesg_n *pmesg)
{
	struct ipc_msghdr_n *phdr = NULL;
	int ret = 0;
	uint8_t stm1_loopback = 0;
	struct ifm_entry* pifm = NULL;

    phdr = & ( pmesg->msghdr );
	if(phdr->msg_subtype == STM_INFO_LOOPBACK)
	{
		stm1_loopback = pmesg->msg_data[0];
		pifm = ifm_lookup ( phdr->msg_index );
		if (pifm == NULL)
		{
			zlog_err ( "%s[%d]:%s,ifm_lookup interface 0x%0x error", __FILE__, __LINE__, __func__, phdr->msg_index);
		
		}

		if(pifm->ifm.sub_type != IFNET_SUBTYPE_STM1)
		{
			zlog_err ( "%s[%d]:%s, interface 0x%0x type is not stm-1", __FILE__, __LINE__, __func__, phdr->msg_index);
		
		}

		pifm->ifm.lb_flags = stm1_loopback;
		zlog_debug ( IFM_DBG_COMMON,"%s[%d]:%s, if type is stm-1 loopback=%d\n", __FILE__, __LINE__, __func__, pifm->ifm.lb_flags);

	}
}

#if 0

/* 从公共队列接收消息存入本地缓冲区 */
void ifm_msg_rcv_pthread ( void *arg )
{
    //struct ipc_mesg *pmesg = NULL;
    //struct ipc_mesg  mesg;
    struct ipc_pkt *pmesg = NULL;
	struct ipc_pkt mesg;
    int msg_num = 0;
    int ret;

    memset ( &mesg, 0, sizeof ( struct ipc_pkt ) );
    prctl(PR_SET_NAME,"ifm rcv ipc msg pthread"); 

    while ( 1 )
    {
        if ( ipc_common_id < 0 )
        {
            ipc_connect_common();
        }

        ret = msgrcv ( ipc_common_id, &mesg, IPC_MSG_LEN + IPC_HEADER_LEN - 4, MODULE_ID_IFM, 0 );
        if ( ret == -1 )
        {
            continue;
        }

        if ( ret == -1 )
        {
            //usleep ( 1000 );
            continue;
        }

        pmesg = ( struct ipc_pkt * ) malloc ( sizeof ( struct ipc_pkt ) );
        if ( NULL == pmesg )
        {
            zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );
            continue;
        }

        pthread_mutex_lock ( &ifm_rmsg_lock );
        memcpy ( pmesg, &mesg, sizeof ( struct ipc_pkt ) );
        listnode_add ( &ifm_rmsg_list, pmesg );
        pthread_mutex_unlock ( &ifm_rmsg_lock );

        if ( msg_num++ == 100 )
        {
            msg_num = 0;
            usleep ( 1000 );
        }

    }

}

void ifm_msg_send_list ( int ipc_id, struct list *plist )
{
    int ret;
    int queue_id = 0;
    struct msqid_ds qbuf;
    struct ifm_send_msg  *pmesg_new = NULL;
    struct ifm_send_msg   mesg;
    void *pdata = NULL;
    int count = 100;
    unsigned int queue_len = 3400;

    /* ipc for send control message */
    queue_id = msgget ( ipc_id, 0 );

    if ( queue_id < 0 )
    {
        zlog_err ( "Connect message queue failed, key:%d,id:%d\n", IPC_MSG_COMMON, queue_id );
        return;
    }
    zlog_debug ( IFM_DBG_COMMON,"%s[%d]Connect message queue success, key:%d,id:%d\n", 
    		__FUNCTION__,__LINE__,IPC_MSG_COMMON, queue_id );

    /* 获取队列参数 */
    ret = msgctl ( queue_id, IPC_STAT, &qbuf );
    if ( ret == -1 )
    {
        zlog_err ( "Get ipc msg queue parameter failed: %d\n", ret );
        return;
    }
    //zlog_debug ( IFM_DBG_COMMON,"queue_id %d qbuf.msg_qnum %u\n",
    //             queue_id, qbuf.msg_qnum );

    count = 100;

    if ( ipc_id == IPC_MSG_COMMON )
    {
        queue_len = 3400;
    }
    else if ( ipc_id == IPC_MSG_FTM )
    {
        queue_len = 3400;
    }
    else if ( ipc_id == IPC_MSG_HAL )
    {
        queue_len = 2400;
    }


    if ( qbuf.msg_qnum < queue_len )
    {
        while ( count )
        {
            if ( list_isempty ( plist ) )
            {
                break;
            }

            if ( ipc_id == IPC_MSG_COMMON )
            {
                 /* 从缓冲区取一个消息执行 */
                pthread_mutex_lock ( &ifm_smsg_comm_lock );

                pmesg_new = listnode_head ( plist );

                if ( NULL == pmesg_new )
                {
                    usleep ( 1000 );
                    count --;
                    continue;
                }
                memcpy ( &mesg, pmesg_new, sizeof ( struct ifm_send_msg ) );
                pdata = pmesg_new->pdata;
                listnode_delete ( plist, pmesg_new );
                free ( pmesg_new );

                pthread_mutex_unlock ( &ifm_smsg_comm_lock );

                /*ipc_send_common ( pdata,  mesg.msghdr.data_len,
                                  mesg.msghdr.data_num,  mesg.msghdr.module_id,
                                  mesg.msghdr.sender_id,  mesg.msghdr.msg_type,
                                  mesg.msghdr.msg_subtype,  mesg.msghdr.opcode );*/
                ifm_ipc_send( pdata, mesg.msghdr.data_len, mesg.msghdr.data_num, mesg.msghdr.module_id, 
                mesg.msghdr.sender_id, mesg.msghdr.msg_type, mesg.msghdr.msg_subtype, mesg.msghdr.opcode, mesg.msghdr.msg_index);

			}
            else if ( ipc_id == IPC_MSG_FTM )
            {
               /* 从缓冲区取一个消息执行 */
                pthread_mutex_lock ( &ifm_smsg_ftm_lock );

                pmesg_new = listnode_head ( plist );

                if ( NULL == pmesg_new )
                {
                    usleep ( 1000 );
                    count --;
                    continue;
                }
                memcpy ( &mesg, pmesg_new, sizeof ( struct ifm_send_msg ) );
                pdata = pmesg_new->pdata;
                listnode_delete ( plist, pmesg_new );
                free ( pmesg_new );

                pthread_mutex_unlock ( &ifm_smsg_ftm_lock );

                /*ipc_send_ftm ( pdata,  mesg.msghdr.data_len,
                               mesg.msghdr.data_num,  mesg.msghdr.module_id,
                               mesg.msghdr.sender_id,  mesg.msghdr.msg_type,
                               mesg.msghdr.msg_subtype,  mesg.msghdr.opcode, mesg.msghdr.msg_index );*/
				ifm_ipc_send( pdata, mesg.msghdr.data_len, mesg.msghdr.data_num, mesg.msghdr.module_id, 
								   mesg.msghdr.sender_id, mesg.msghdr.msg_type, mesg.msghdr.msg_subtype, mesg.msghdr.opcode, mesg.msghdr.msg_index);
                              

            }
            else if ( ipc_id == IPC_MSG_HAL )
            {
               /* 从缓冲区取一个消息执行 */
                pthread_mutex_lock ( &ifm_smsg_hal_lock );

                pmesg_new = listnode_head ( plist );

                if ( NULL == pmesg_new )
                {
                    usleep ( 1000 );
                    count --;
                    continue;
                }
                memcpy ( &mesg, pmesg_new, sizeof ( struct ifm_send_msg ) );
                pdata = pmesg_new->pdata;
                listnode_delete ( plist, pmesg_new );
                free ( pmesg_new );

                pthread_mutex_unlock ( &ifm_smsg_hal_lock );

                /*ipc_send_hal ( pdata,  mesg.msghdr.data_len,
                               mesg.msghdr.data_num,  mesg.msghdr.module_id,
                               mesg.msghdr.sender_id,  mesg.msghdr.msg_type,
                               mesg.msghdr.msg_subtype,  mesg.msghdr.opcode, mesg.msghdr.msg_index );*/
				ifm_ipc_send( pdata, mesg.msghdr.data_len, mesg.msghdr.data_num, mesg.msghdr.module_id, 
							mesg.msghdr.sender_id, mesg.msghdr.msg_type, mesg.msghdr.msg_subtype, mesg.msghdr.opcode, mesg.msghdr.msg_index);
												  
                       

            }
           // free ( pdata );

            count --;

        }
    }
    else
    {
        usleep ( 1000 );
    }

}


void ifm_msg_send_pthread ( void *arg )
{
    prctl(PR_SET_NAME,"ifm send ipc msg pthread"); 
    while ( 1 )
    {
        if ( ifm_smsg_hal_list.count == 0
                && ifm_smsg_ftm_list.count == 0
                && ifm_smsg_comm_list.count == 0  )
        {
            usleep ( 1000 );
            continue;
        }
        /* send ipc message to hal */
        ifm_msg_send_list ( IPC_MSG_HAL, &ifm_smsg_hal_list );
        /*send ipc message to ftm */
        ifm_msg_send_list ( IPC_MSG_FTM, &ifm_smsg_ftm_list );
        /*send ipc message to common */
        ifm_msg_send_list ( IPC_MSG_COMMON, &ifm_smsg_comm_list );
    }

}

void ifm_msg_send_com ( struct ipc_msghdr *pmsghdr, void *pdata )
{
    struct ifm_send_msg *pmesg = NULL;

    pmesg = ( struct ifm_send_msg * ) malloc ( sizeof ( struct ifm_send_msg ) );
    if ( NULL == pmesg )
    {
        zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );        
        return;
    }

    if ( pmsghdr->data_len && pdata )
    {
        pmesg->pdata = ( void * ) malloc ( pmsghdr->data_len );
        if ( NULL == pmesg->pdata )
        {
            zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );
	   free( pmesg );
            return;
        }
    }

    pthread_mutex_lock ( &ifm_smsg_comm_lock );
    memcpy ( &pmesg->msghdr, pmsghdr, sizeof ( struct ipc_msghdr ) );
    if ( pmsghdr->data_len && pdata )
    {
        memcpy ( pmesg->pdata, pdata, pmsghdr->data_len );
    }
    else
    {
        pmesg->pdata = NULL;
    }

    listnode_add ( &ifm_smsg_comm_list, pmesg );
    pthread_mutex_unlock ( &ifm_smsg_comm_lock );    

}

void ifm_msg_send_ftm ( struct ipc_msghdr *pmsghdr, void *pdata )
{
    struct ifm_send_msg *pmesg = NULL;
    pmesg = ( struct ifm_send_msg * ) malloc ( sizeof ( struct ifm_send_msg ) );
    if ( NULL == pmesg )
    {
        zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );        
        return;
    }

    if ( pmsghdr->data_len && pdata )
    {
        pmesg->pdata = ( void * ) malloc ( pmsghdr->data_len );
        if ( NULL == pmesg->pdata )
        {
            zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );
	   free( pmesg );
            return;
        }
    }

    pthread_mutex_lock ( &ifm_smsg_ftm_lock );
    memcpy ( &pmesg->msghdr, pmsghdr, sizeof ( struct ipc_msghdr ) );
    if ( pmsghdr->data_len && pdata )
    {
        memcpy ( pmesg->pdata, pdata, pmsghdr->data_len );
    }
    else
    {
        pmesg->pdata = NULL;
    }

    listnode_add ( &ifm_smsg_ftm_list, pmesg );
    pthread_mutex_unlock ( &ifm_smsg_ftm_lock );   

}

void ifm_msg_send_hal ( struct ipc_msghdr *pmsghdr, void *pdata )
{
    struct ifm_send_msg *pmesg = NULL;

    pmesg = ( struct ifm_send_msg * ) malloc ( sizeof ( struct ifm_send_msg ) );
    if ( NULL == pmesg )
    {
        zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );       
        return;
    }

    if ( pmsghdr->data_len && pdata )
    {
        pmesg->pdata = ( void * ) malloc ( pmsghdr->data_len );
        if ( NULL == pmesg->pdata )
        {
            zlog_err ( "[%s %d] The system run out of memory !", __FUNCTION__, __LINE__ );
	   		free ( pmesg );
            return;
        }
    }

    pthread_mutex_lock ( &ifm_smsg_hal_lock );
    memcpy ( &pmesg->msghdr, pmsghdr, sizeof ( struct ipc_msghdr ) );
    if ( pmsghdr->data_len && pdata )
    {
        memcpy ( pmesg->pdata, pdata, pmsghdr->data_len );
    }
    else
    {
        pmesg->pdata = NULL;
    }

    listnode_add ( &ifm_smsg_hal_list, pmesg );
    pthread_mutex_unlock ( &ifm_smsg_hal_lock );  

}
#endif
#if 0
/*接收公共 IPC 消息*/
int ifm_msg_rcv_handle ( struct thread *pthread )
{
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;
    //struct ipc_mesg   *pmesg_new = NULL;
    struct ipc_pkt   *pmesg_new = NULL;
	struct list *plist = NULL;

   // static struct ipc_mesg mesg;
  	static struct ipc_pkt mesg;
    struct ipc_msghdr *phdr = NULL;
    struct ifm_entry *pifm = NULL;
    uint8_t status;
    int ret = 0;
    int token = 100;
    struct ifm_port *pspeed = NULL;
    uint8_t stm1_loopback = 0;
	uint64_t rx_byte = 0;
	struct ifm_counter* tcounter = NULL;
	unsigned int devType = 0;
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devType );

    while ( token )
    {
        if ( list_isempty ( &ifm_rmsg_list ) )
        {
            break;
        }

        /* 从缓冲区取一个消息执行 */
        pthread_mutex_lock ( &ifm_rmsg_lock );
		
		plist = &ifm_rmsg_list;

        for ( ALL_LIST_ELEMENTS ( plist, pnode, pnextnode, pmesg_new ) )
        {
            if ( NULL == pmesg_new )
            {
                continue;
            }

            memcpy ( &mesg, pmesg_new, sizeof ( struct ipc_pkt ) );
            list_delete_node ( plist, pnode );
            free ( pmesg_new );
            break;
        }

        pthread_mutex_unlock ( &ifm_rmsg_lock );

        token--;
        phdr = & ( mesg.msghdr );

        zlog_debug ( IFM_DBG_COMMON,"[%s %d]opcode=%d msg_type=%d sender_id=%d msg_subtype=%d phdr->msg_index=0x%0x\n", __FUNCTION__, __LINE__,
                     phdr->opcode, phdr->msg_type, phdr->sender_id, phdr->msg_subtype, phdr->msg_index );

        if ( phdr->msg_type == IPC_TYPE_IFM ) /* 处理接口管理消息*/
        {
            /* handle ifm event register msg */
            if ( phdr->opcode == IPC_OPCODE_REGISTER ) /* 事件注册消息 */
            {
                ret = ifm_register_msg ( phdr->sender_id, phdr->msg_subtype, phdr->msg_index );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_register_msg interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    goto out;
                }
            }
            else if ( phdr->opcode == IPC_OPCODE_EVENT ) /* 事件通知消息 */
            {
                if ( phdr->msg_subtype == IFNET_EVENT_DOWN )                    /*收到 linkdown 消息*/
                {
                    status = IFNET_LINKDOWN;
                    ret = ifm_set_info ( phdr->msg_index, &status, IFNET_INFO_STATUS, 0 );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ifm_set_info interface 0x%0x status linkdown %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        goto out;
                    }
                }
                else if ( phdr->msg_subtype == IFNET_EVENT_UP ) /*收到 linkup 消息*/
                {
                    status = IFNET_LINKUP;
                    ret = ifm_set_info ( phdr->msg_index, &status, IFNET_INFO_STATUS, 0 );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ifm_set_info interface 0x%0x status linkup %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        goto out;
                    }
                }
				if( devType == ID_H9MOLMXE_VX  )
				{
					int vx_slot = 0;
					int vx_port = 0;
					
					vx_slot = IFM_SLOT_ID_GET( phdr->msg_index );
					vx_port = IFM_PORT_ID_GET( phdr->msg_index );
					
					if( vx_slot == 0 && vx_port == 2 )
					{
						status = IFNET_LINKUP;
						ret = ifm_set_info ( phdr->msg_index, &status, IFNET_INFO_STATUS, 0 );
	                    if ( ret < 0 )
	                    {
	                        zlog_err ( "[%s %d] ifm_set_info interface 0x%0x status linkup %d", __FUNCTION__, __LINE__,
	                                   phdr->msg_index, ret );
	                        goto out;
	                    }
						
					}
				}
            }
            else if ( phdr->opcode == IPC_OPCODE_GET ) /* 处理接口信息获取消息*/
            {
                ret = ifm_rcv_getif_msg ( &mesg );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    goto out;
                }

            }
            else if ( phdr->opcode == IPC_OPCODE_GET_BULK ) /* 处理接口批量信息获取消息*/
            {
                if ( phdr->msg_subtype == IFNET_INFO_MAX )
                {
                    if ( phdr->sender_id == MODULE_ID_SNMPD )
                    {
                        ret = ifm_reply_snmp_bulk ( phdr );
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                            goto out;
                        }

                    }
                    else
                    {
                        ret = ifm_reply_bulk ( phdr );
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                            goto out;
                        }
                    }
                }
				else if ( phdr->msg_subtype == IFNET_INFO_SFP )
				{
					if ( phdr->sender_id == MODULE_ID_SNMPD )
					{
						ret = ifm_reply_snmp_sfp( phdr );
						if ( ret < 0 )
						{
							 zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                            goto out;
						}
					}
				}
				else if ( phdr->msg_subtype == IFNET_INFO_ENCAP)
				{
					if ( phdr->sender_id == MODULE_ID_SNMPD )
					{
						ret = ifm_reply_snmp_subencap( phdr );
						if ( ret != 0 )
						{
							zlog_err("%s[%d]ifm reply subencap ifindex 0x%0x ret %d\n",__FUNCTION__,__LINE__,
								phdr->msg_index,ret );
							goto out;
						}
					}
				}
				else if( phdr->msg_subtype == IFNET_INFO_COUNTER )
				{
					ret = ifm_reply_snmp_counter( phdr);
					if ( ret != 0 )
					{
						zlog_err("%s[%d]ifm reply subencap ifindex 0x%0x ret %d\n",__FUNCTION__,__LINE__,
							phdr->msg_index,ret );
						goto out;
					}
				}
            }
            else if ( phdr->opcode == IPC_OPCODE_UPDATE ) /* 处理接口信息更新消息*/
            {
                ret = ifm_set_info ( phdr->msg_index, ( void * ) mesg.msg_data, phdr->msg_subtype, IFM_OPCODE_UPDATE );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_set_info interface 0x%0x msg_subtype %d ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, phdr->msg_subtype, ret );

                    //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                    //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
					ret = ifm_msg_send_noack(0,phdr);
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                    }

                    goto out;
                }
                //ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
                //                    IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
				ret = ifm_msg_send_ack( phdr );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    goto out;
                }
            }
            else if ( phdr->opcode == IPC_OPCODE_ADD ) /* 处理接口创建消息*/
            {
                if ( phdr->msg_subtype == IFNET_INFO_SUBIF )
                {
                    struct ifm_encap *pencap = ( struct ifm_encap * ) mesg.msg_data;
                    if ( NULL == pencap )
                    {
                        zlog_err ( "[%s %d] pencap is null", __FUNCTION__, __LINE__ );
                        return ERRNO_FAIL;
                    }
                    zlog_debug ( IFM_DBG_COMMON,"%s[%d] set ifindex=0x%0x encap type %d svlan %d cvlan %d\n", __FUNCTION__, __LINE__,
                                 phdr->msg_index, pencap->type, pencap->svlan.vlan_start, pencap->cvlan.vlan_start );

                    ret = ifm_create_interface ( phdr->msg_index );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                        //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
						ret = ifm_msg_send_noack(0,phdr);
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                        }
                        goto out;
                    }

                    ret = ifm_set_info ( phdr->msg_index, pencap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ifm_set_info interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                        //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
						ret = ifm_msg_send_noack(0,phdr);
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                        }

                        goto out;
                    }


                    //ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
                    //                     IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
					ret = ifm_msg_send_ack( phdr );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        goto out;
                    }

                }
				else if ( phdr->msg_subtype == IFNET_INFO_SUBIF2 )
				{
					uint8_t mode = IFNET_MODE_L2;
                    struct ifm_encap *pencap = ( struct ifm_encap * ) mesg.msg_data;
                    if ( NULL == pencap )
                    {
                        zlog_err ( "[%s %d] pencap is null", __FUNCTION__, __LINE__ );
                        return ERRNO_FAIL;
                    }
                    zlog_debug ( IFM_DBG_COMMON,"%s[%d] set ifindex=0x%0x encap type %d svlan %d cvlan %d\n", __FUNCTION__, __LINE__,
                                 phdr->msg_index, pencap->type, pencap->svlan.vlan_start, pencap->cvlan.vlan_start );

                    ret = ifm_create_interface ( phdr->msg_index );
                    if ( ret != 0 )
                    {
                        zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                        //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
						ret = ifm_msg_send_noack(0,phdr);
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                        }
                        goto out;
                    }
					
                    ret = ifm_set_info ( phdr->msg_index, (void *) &mode, IFNET_INFO_MODE, IFM_OPCODE_UPDATE );
                    if ( ret != 0 )
                    {
                        zlog_err ( "[%s %d] ifm_set_info interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                        //                      IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
						ret = ifm_msg_send_noack(0,phdr);
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                        }

                        goto out;
                    }

                    ret = ifm_set_info ( phdr->msg_index, pencap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
                    if ( ret != 0 )
                    {
                        zlog_err ( "[%s %d] ifm_set_info interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                        //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
						ret = ifm_msg_send_noack(0,phdr);
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                        }

                        goto out;
                    }

                    //ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
                    //                    IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
					ret = ifm_msg_send_ack( phdr );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        goto out;
                    }

                }
                else
                {
                    ret = ifm_create_interface ( phdr->msg_index );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                        //                      IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
						ret = ifm_msg_send_noack(0,phdr);
                        if ( ret < 0 )
                        {
                            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                       phdr->msg_index, ret );
                        }

                        goto out;
                    }
                    //ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
                    //                     IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
					ret = ifm_msg_send_ack( phdr );
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                        goto out;
                    }
                }

            }
            else if ( phdr->opcode == IPC_OPCODE_DELETE ) /* 处理接口删除消息*/
            {
                ret = ifm_delete ( phdr->msg_index );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                    //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
					ret = ifm_msg_send_noack(0,phdr);
                    if ( ret < 0 )
                    {
                        zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index, ret );
                    }

                    goto out;
                }
                //ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
                //                    IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
				ret = ifm_msg_send_ack( phdr );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    goto out;
                }

            }
            else if ( phdr->opcode == IPC_OPCODE_INVALID  )
            {

				if ( phdr->sender_id == MODULE_ID_OSPF )
			   {
					if ( phdr->msg_subtype == OSPF_DCN_ENABLE )
					{
						 // dcn_state = 1  dcn使能   dcn_state = 0 dcn禁止
						if(mesg.msg_data)
						{
							ifm_set_dcn(*(u_char*)mesg.msg_data);
						}
						else
						{
							zlog_err ( "[%s %d] dcn enable var is null", __FUNCTION__, __LINE__ );
							return ERRNO_FAIL;
						}
					}

			   }
               else if ( phdr->msg_subtype == IFNET_INFO_SPEED ) /* 处理接口speed初始化消息*/
                {
                    pspeed = ( struct ifm_port * ) mesg.msg_data;
                    if ( NULL == pspeed )
                    {
                        zlog_err ( "[%s %d] pspeed is null", __FUNCTION__, __LINE__ );
                        return ERRNO_FAIL;
                    }

                    /*init speed default config*/
                    pifm = ifm_lookup ( phdr->msg_index );
                    if ( !pifm )
                    {
                        zlog_err ( "[%s %d] ifm_lookup interface 0x%0x parent erro ret %d", __FUNCTION__, __LINE__,
                                   phdr->msg_index , ret );
                        goto out;
                    }

                    pifm->port_info.speed = pspeed->speed;
                    pifm->port_info.autoneg = pspeed->autoneg;
                    pifm->port_info.duplex = pspeed->duplex;

                    zlog_debug ( IFM_DBG_COMMON,"%s[%d]ifindex=0x%0x operation %s %s speed %s\n", __FUNCTION__,__LINE__,phdr->msg_index,
                                 pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO ? "auto" : "force",
                                 pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "duplex" : "simplex",
                                 pifm->port_info.speed == IFNET_SPEED_FE ? "100" : "1000" );

               }
				
            }
			else if( phdr->opcode == IPC_OPCODE_REPORT )
			{
				if( phdr->msg_subtype == IFNET_INFO_STATISTICS )
				{
					tcounter = (struct ifm_counter*)mesg.msg_data;
					pifm = ifm_lookup(phdr->msg_index);
					if ( !pifm )
                    {
                        //zlog_err ( "[%s %d] ifm_lookup interface 0x%0x parent erro ret %d", __FUNCTION__, __LINE__,
                         //          phdr->msg_index , ret );
                        goto out;
                    }
					if( IFM_TYPE_IS_ETHERNET_PHYSICAL(pifm->ifm.ifindex))
					{
						if( tcounter )
						{						
							memcpy(pifm->pcounter,tcounter,sizeof(struct ifm_counter));
						}
						else 
						{
							memset(pifm->pcounter, 0, sizeof(struct ifm_counter));
						}						
					
					}
					if(IFM_TYPE_IS_ETHERNET_SUBPORT(pifm->ifm.ifindex))
					{
						if( pifm->ifm.statistics == IFNET_STAT_ENABLE )
						{
							if( tcounter )
							{						
								memcpy(pifm->pcounter,tcounter,sizeof(struct ifm_counter));
							}
							else 
							{
								memset(pifm->pcounter, 0, sizeof(struct ifm_counter));
							}		
						}
					}					
                          
				}
			}
        }
        else if ( phdr->msg_type == IPC_TYPE_DEVM ) /* 处理设备管理消息*/
        {
            ret = ifm_rcv_devm_msg ( &mesg );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ifm_rcv_devm_msg  messges ret %d", __FUNCTION__, __LINE__, ret );
                goto out;
            }
        }
        else if ( phdr->msg_type == IPC_TYPE_QOSIF ) /* 处理qos mapping消息*/
        {
            ret = ifm_rcv_qosif_msg ( &mesg );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ifm_rcv_qosif_msg  messges ret %d", __FUNCTION__, __LINE__, ret );
                goto out;
            }
        }
        else if ( phdr->msg_type == IPC_TYPE_STM )
        {
            if(phdr->msg_subtype == STM_INFO_LOOPBACK)
            {
                stm1_loopback = mesg.msg_data[0];
                pifm = ifm_lookup ( phdr->msg_index );
                if (pifm == NULL)
                {
                    zlog_err ( "%s[%d]:%s,ifm_lookup interface 0x%0x error", __FILE__, __LINE__, __func__, phdr->msg_index);
                    goto out;
                }

                if(pifm->ifm.sub_type != IFNET_SUBTYPE_STM1)
                {
                    zlog_err ( "%s[%d]:%s, interface 0x%0x type is not stm-1", __FILE__, __LINE__, __func__, phdr->msg_index);
                    goto out;
                }

                pifm->ifm.lb_flags = stm1_loopback;
                zlog_debug ( IFM_DBG_COMMON,"%s[%d]:%s, if type is stm-1 loopback=%d\n", __FILE__, __LINE__, __func__, pifm->ifm.lb_flags);
            }
        }
    }
out:
    usleep ( 1000 ); //让出 CPU
    thread_add_event ( ifm_master, ifm_msg_rcv_handle, NULL, 0 );
    return ret;
}
#endif
int ifm_rcv_ifm_msg( struct ipc_mesg_n * pmesg )
{
	int ret = 0;
	struct ipc_msghdr_n * phdr = NULL;
	uint8_t status;
	struct ifm_port *pspeed = NULL;
	struct ifm_entry *pifm = NULL;
	unsigned int devType = 0;
	struct ifm_counter* tcounter = NULL;	
	
	phdr = &(pmesg->msghdr);
	
	if(IPC_OPCODE_REGISTER == phdr->opcode )/* 浜嬩欢娉ㄥ唽娑堟伅 */
	{
		ret = ifm_register_msg ( phdr->sender_id, phdr->msg_subtype, phdr->msg_index );
        if ( ret < 0 )
        {
            zlog_err ( "[%s %d] ifm_register_msg interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, ret );
            return ret;
        }
	}
	else if( IPC_OPCODE_EVENT == phdr->opcode )/* 浜嬩欢閫氱煡娑堟伅 */
	{
		devm_comm_get_id(1, 0, MODULE_ID_IFM, &devType );
		if ( IFNET_EVENT_DOWN == phdr->msg_subtype  ) /*鏀跺埌 linkdown 娑堟伅*/                   
        {
            status = IFNET_LINKDOWN;
            ret = ifm_set_info ( phdr->msg_index, &status, IFNET_INFO_STATUS, 0 );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ifm_set_info interface 0x%0x status linkdown %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ret;
            }
        }
		else if (  IFNET_EVENT_UP == phdr->msg_subtype  ) /*鏀跺埌 linkup 娑堟伅*/
        {
            status = IFNET_LINKUP;
            ret = ifm_set_info ( phdr->msg_index, &status, IFNET_INFO_STATUS, 0 );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ifm_set_info interface 0x%0x status linkup %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ret;
            }
        }
		if( devType == ID_H9MOLMXE_VX  )
		{
			int vx_slot = 0;
			int vx_port = 0;
			
			vx_slot = IFM_SLOT_ID_GET( phdr->msg_index );
			vx_port = IFM_PORT_ID_GET( phdr->msg_index );
			
			if( vx_slot == 0 && vx_port == 2 )
			{
				status = IFNET_LINKUP;
				ret = ifm_set_info ( phdr->msg_index, &status, IFNET_INFO_STATUS, 0 );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_set_info interface 0x%0x status linkup %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    return ret;
                }
				
			}
		}
	}
	else if(IPC_OPCODE_GET == phdr->opcode )/* 澶勭悊鎺ュ彛淇℃伅鑾峰彇娑堟伅*/
	{
		ret = ifm_rcv_getif_msg ( pmesg );
        if ( ret < 0 )
        {
            zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, ret );
            return ret;
        }
		
	}
	else if( IPC_OPCODE_GET_BULK == phdr->opcode )/* 澶勭悊鎺ュ彛鎵归噺淇℃伅鑾峰彇娑堟伅*/
	{
		if ( IFNET_INFO_MAX ==  phdr->msg_subtype  )
        {
            if ( MODULE_ID_SNMPD == phdr->sender_id  )
            {
                ret = ifm_reply_snmp_bulk ( phdr );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    return ret;
                }

            }
            else
            {
                ret = ifm_reply_bulk ( phdr );
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                    return ret;
                }
            }
        }
		else if( IFNET_INFO_SFP == phdr->msg_subtype )
		{
			if ( MODULE_ID_SNMPD == phdr->sender_id   )
			{
				ret = ifm_reply_snmp_sfp( phdr );
				if ( ret < 0 )
				{
					zlog_err ( "[%s %d] ifm_rcv_getif_msg interface 0x%0x ret %d", __FUNCTION__, __LINE__,
						phdr->msg_index, ret );					
					return ret;
				}
			}
		}
		else if ( IFNET_INFO_ENCAP == phdr->msg_subtype )
		{
			if (  MODULE_ID_SNMPD == phdr->sender_id  )
			{
				ret = ifm_reply_snmp_subencap( phdr );
				if ( ret != 0 )
				{
					zlog_err("%s[%d]ifm reply subencap ifindex 0x%0x ret %d\n",__FUNCTION__,__LINE__,
						phdr->msg_index,ret );
					return ret;
				}
			}
		}
		else if( phdr->msg_subtype == IFNET_INFO_COUNTER )
		{
			ret = ifm_reply_snmp_counter( phdr);
			if ( ret != 0 )
			{
				zlog_err("%s[%d]ifm reply subencap ifindex 0x%0x ret %d\n",__FUNCTION__,__LINE__,
					phdr->msg_index,ret );
				return ret;
			}
		}
		
	}
	else if ( IPC_OPCODE_UPDATE == phdr->opcode ) /* 澶勭悊鎺ュ彛淇℃伅鏇存柊娑堟伅*/
	{
		ret = ifm_set_info ( phdr->msg_index, ( void * ) pmesg->msg_data, phdr->msg_subtype, IFM_OPCODE_UPDATE );
        if ( ret < 0 )
        {
            zlog_err ( "[%s %d] ifm_set_info interface 0x%0x msg_subtype %d ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, phdr->msg_subtype, ret );

            //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
            //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
            ret = ifm_msg_send_noack(0,phdr);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
            }

            return ret;
        }
        //ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
       	//                    IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
       	ret = ifm_msg_send_ack( phdr);
        if ( ret < 0 )
        {
            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, ret );
            return ret;
        }
	}
	else if ( IPC_OPCODE_ADD == phdr->opcode  ) /* 澶勭悊鎺ュ彛鍒涘缓娑堟伅*/
	{
		if ( IFNET_INFO_SUBIF == phdr->msg_subtype )
        {
        	
			struct ifm_encap *pencap = ( struct ifm_encap * ) pmesg->msg_data;
			if ( NULL == pencap )
			{
				zlog_err ( "[%s %d] pencap is null", __FUNCTION__, __LINE__ );
				return ERRNO_FAIL;
			}
			zlog_debug (IFM_DBG_COMMON, "%s[%d] set ifindex=0x%0x encap type %d svlan %d cvlan %d\n", __FUNCTION__, __LINE__,
			phdr->msg_index, pencap->type, pencap->svlan.vlan_start, pencap->cvlan.vlan_start );
			
			ret = ifm_create_interface ( phdr->msg_index );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
                ret = ifm_msg_send_noack(0,phdr);
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                }
                return ret;
            }
			
			ret = ifm_set_info ( phdr->msg_index, pencap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
			if ( ret < 0 )
			{
				zlog_err ( "[%s %d] ifm_set_info interface 0x%0x ret %d", __FUNCTION__, __LINE__,
						   phdr->msg_index, ret );
				//ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
				//					   IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
				ret = ifm_msg_send_noack(0,phdr);
				if ( ret < 0 )
				{
					zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
							   phdr->msg_index, ret );
				}

				return ret;
			}
			//ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
			ret = ifm_msg_send_ack( phdr );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ret;
            }
		}
		else if ( IFNET_INFO_SUBIF2 == phdr->msg_subtype  )
		{
			
			uint8_t mode = IFNET_MODE_L2;
			struct ifm_encap *pencap = ( struct ifm_encap * ) pmesg->msg_data;
			if ( NULL == pencap )
			{
				zlog_err ( "[%s %d] pencap is null", __FUNCTION__, __LINE__ );
				return ERRNO_FAIL;
			}
			zlog_debug ( IFM_DBG_COMMON,"%s[%d] set ifindex=0x%0x encap type %d svlan %d cvlan %d\n", __FUNCTION__, __LINE__,
					phdr->msg_index, pencap->type, pencap->svlan.vlan_start, pencap->cvlan.vlan_start );
			
			ret = ifm_create_interface ( phdr->msg_index );
            if ( ret != 0 )
            {
                zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                //                      IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
                ret = ifm_msg_send_noack(0, phdr);
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                }
                return ret;
            }
			ret = ifm_set_info ( phdr->msg_index, (void *) &mode, IFNET_INFO_MODE, IFM_OPCODE_UPDATE );
            if ( ret != 0 )
            {
                zlog_err ( "[%s %d] ifm_set_info interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
                //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
                 ret = ifm_msg_send_noack(0, phdr);
                if ( ret < 0 )
                {
                    zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                               phdr->msg_index, ret );
                }

                return ret;
            }
			ret = ifm_set_info ( phdr->msg_index, pencap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
	        if ( ret != 0 )
	        {
	            zlog_err ( "[%s %d] ifm_set_info interface 0x%0x ret %d", __FUNCTION__, __LINE__,
	                       phdr->msg_index, ret );
	            //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
	             //                      IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
	            ret = ifm_msg_send_noack(0, phdr);
	            if ( ret < 0 )
	            {
	                zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
	                           phdr->msg_index, ret );
	            }

	            return ret;
	        }
			//ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
            //                             IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
            ret = ifm_msg_send_ack( phdr );
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
                return ret;
            }
		}
		else
		{
			ret = ifm_create_interface ( phdr->msg_index );
			if ( ret < 0 )
			{
				zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
					phdr->msg_index, ret );
				//ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
				//		IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
				 ret = ifm_msg_send_noack(0, phdr);
				if ( ret < 0 )
				{
					zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
							phdr->msg_index, ret );
				}

				return ret;
			}
			//ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
			//				IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
			ret = ifm_msg_send_ack( phdr );
			if ( ret < 0 )
			{
				zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
						phdr->msg_index, ret );
				return ret;
			}
		}
		
	}
	else if ( IPC_OPCODE_DELETE  == phdr->opcode ) /* 澶勭悊鎺ュ彛鍒犻櫎娑堟伅*/
	{
		ret = ifm_delete ( phdr->msg_index );
        if ( ret < 0 )
        {
            zlog_err ( "[%s %d] ifm_create_interface interface 0x%0x ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, ret );
            //ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_IFM,
            //                       IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
            ret = ifm_msg_send_noack(0, phdr);
            if ( ret < 0 )
            {
                zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index, ret );
            }

            return ret;
        }
		//ret = ipc_send_ack ( phdr->sender_id, MODULE_ID_IFM,
        //                             IPC_TYPE_IFM, phdr->msg_subtype, phdr->msg_index );
        ret = ifm_msg_send_ack( phdr );
        if ( ret < 0 )
        {
            zlog_err ( "[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
                       phdr->msg_index, ret );
            return ret;
        }
	}
	else if ( IPC_OPCODE_INVALID == phdr->opcode )
	{
		if (  MODULE_ID_OSPF == phdr->sender_id  )
		{
			if ( OSPF_DCN_ENABLE  == phdr->msg_subtype  )// dcn_state = 1  dcn浣胯兘   dcn_state = 0 dcn绂佹
			{		
				if(pmesg->msg_data)
				{
					ifm_set_dcn(*(u_char*)pmesg->msg_data);
				}
				else
				{
					zlog_err ( "[%s %d] dcn enable var is null", __FUNCTION__, __LINE__ );
					return ERRNO_FAIL;
				}
			}
		}
		else if ( phdr->msg_subtype == IFNET_INFO_SPEED ) /* 澶勭悊鎺ュ彛speed鍒濆鍖栨秷鎭?*/
		{
			pspeed = ( struct ifm_port * )pmesg->msg_data;
            if ( NULL == pspeed )
            {
                zlog_err ( "[%s %d] pspeed is null", __FUNCTION__, __LINE__ );
                return ERRNO_FAIL;
            }

            /*init speed default config*/
            pifm = ifm_lookup ( phdr->msg_index );
            if ( !pifm )
            {
                zlog_err ( "[%s %d] ifm_lookup interface 0x%0x parent erro ret %d", __FUNCTION__, __LINE__,
                           phdr->msg_index , ret );
                return ERRNO_FAIL;
            }

            pifm->port_info.speed = pspeed->speed;
            pifm->port_info.autoneg = pspeed->autoneg;
            pifm->port_info.duplex = pspeed->duplex;

            zlog_debug ( IFM_DBG_COMMON,"%s[%d]ifindex=0x%0x operation %s %s speed %s\n", __FUNCTION__,__LINE__,phdr->msg_index,
                         pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO ? "auto" : "force",
                         pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "duplex" : "simplex",
                         pifm->port_info.speed == IFNET_SPEED_FE ? "100" : "1000" );
		}
	}
	else if( IPC_OPCODE_REPORT == phdr->opcode)
	{
		if( phdr->msg_subtype == IFNET_INFO_STATISTICS )
		{
			tcounter = (struct ifm_counter*)pmesg->msg_data;
			pifm = ifm_lookup(phdr->msg_index);
			if ( !pifm )
            {
                return ERRNO_FAIL;
            }
			if( IFM_TYPE_IS_ETHERNET_PHYSICAL(pifm->ifm.ifindex))
			{
				if( tcounter )
				{						
					memcpy(pifm->pcounter,tcounter,sizeof(struct ifm_counter));
				}
				else 
				{
					memset(pifm->pcounter, 0, sizeof(struct ifm_counter));
				}						
			
			}
			if(IFM_TYPE_IS_ETHERNET_SUBPORT(pifm->ifm.ifindex))
			{
				if( pifm->ifm.statistics == IFNET_STAT_ENABLE )
				{
					if( tcounter )
					{						
						memcpy(pifm->pcounter,tcounter,sizeof(struct ifm_counter));
					}
					else 
					{
						memset(pifm->pcounter, 0, sizeof(struct ifm_counter));
					}		
				}
			}					
                  
		}
	}
	return ret ;

}
/*鎺ユ敹鍏叡 IPC 娑堟伅*/
int ifm_msg_rcv_handle_n ( struct ipc_mesg_n *pmesg, int imlen )
{
	int ret = 0;
	int revln = 0;
	struct ipc_msghdr_n *phdr = NULL;

    phdr = &pmesg->msghdr;

    revln = (int)phdr->data_len + IPC_HEADER_LEN_N; 

	if( revln <= imlen )
	{
		switch( phdr->msg_type)
		{
			case IPC_TYPE_IFM:
				ret = ifm_rcv_ifm_msg ( pmesg );/* 澶勭悊鎺ュ彛绠＄悊娑堟伅*/
	            if ( ret < 0 )
	            {
	                zlog_err ( "[%s %d] ifm_rcv_devm_msg  messges ret %d", __FUNCTION__, __LINE__, ret );
	               //return ret;
	            }
				break;
			case IPC_TYPE_DEVM:
				ret = ifm_rcv_devm_msg( pmesg );
				if ( ret < 0 )
	            {
	                zlog_err ( "[%s %d] ifm_rcv_devm_msg  messges ret %d", __FUNCTION__, __LINE__, ret );
	              //return ret;
	            }
				break;
			case IPC_TYPE_QOSIF:
				ret = ifm_rcv_qosif_msg ( pmesg );
	            if ( ret < 0 )
	            {
	                zlog_err ( "[%s %d] ifm_rcv_qosif_msg  messges ret %d", __FUNCTION__, __LINE__, ret );
	              //return ret;
	            }
				break;
			case IPC_TYPE_STM:
				ret = ifm_rcv_stm_msg(pmesg);
				if ( ret < 0 )
	            {
	                zlog_err ( "[%s %d] ifm_rcv_qosif_msg  messges ret %d", __FUNCTION__, __LINE__, ret );
	              //return ret;
	            }
				break;
			default:
				break;
			
		}
		
	}
	mem_share_free( pmesg, MODULE_ID_IFM );
	return ret;	
	
}
#if 0
/* send message and returen errcode */
int ifm_send_hal_wait_ack ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )
{
    int ret = 0;	
	
    if (data_len &&  NULL == pdata )
    {
        zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
        return -1;
    }
	
	ret = ipc_send_hal_wait_ack ( pdata, data_len, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM, subtype, opcode, msg_index );

	/*fixed bug53778*/
    if(ret != -1)
    {
    	return ret;
    }
	else
	{
		return 0;
	}
	
}


void *ifm_send_hal_wait_reply ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )
{
    if (data_len &&  NULL == pdata )
    {
        zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
        return NULL;
    }

    return ipc_send_hal_wait_reply ( pdata, data_len, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM, subtype, IPC_OPCODE_GET, msg_index );
}


int ifm_send_ftm ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )
{
    struct ipc_msghdr msghdr;

    if ( data_len && NULL == pdata )
    {
        zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
        return -1;
    }

    memset ( &msghdr, 0, sizeof ( struct ipc_msghdr ) );
    msghdr.module_id = MODULE_ID_FTM;
    msghdr.sender_id = MODULE_ID_IFM;
    msghdr.data_len = data_len;
    msghdr.data_num = 1;
    msghdr.msg_type = IPC_TYPE_IFM;
    msghdr.msg_subtype = subtype;
    msghdr.opcode = opcode;
    msghdr.msg_index = msg_index;

    ifm_msg_send_ftm ( &msghdr, pdata );

    return 0;

}

int ifm_send_hal ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )
{
    struct ipc_msghdr msghdr;

    if ( data_len && NULL == pdata )
    {
        zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
        return -1;
    }

    memset ( &msghdr, 0, sizeof ( struct ipc_msghdr ) );
    msghdr.module_id = MODULE_ID_HAL;
    msghdr.sender_id = MODULE_ID_IFM;
    msghdr.data_len = data_len;
    msghdr.data_num = 1;
    msghdr.msg_type = IPC_TYPE_IFM;
    msghdr.msg_subtype = subtype;
    msghdr.opcode = opcode;
    msghdr.msg_index = msg_index;

    ifm_msg_send_hal ( &msghdr, pdata );

    return 0;

}
void ifm_msg_send_com ( struct ipc_msghdr_n *pmsghdr, void *pdata )
{
    int ret = 0;
	struct ipc_mesg_n * pMsgSnd = NULL;

	pMsgSnd = mem_share_malloc( sizeof(struct ipc_msghdr_n) + pmsghdr->data_len , MODULE_ID_IFM);
	if( pMsgSnd == NULL )
	{
		return;
	}
	pMsgSnd->msghdr.data_len      = pmsghdr->data_len;
	pMsgSnd->msghdr.data_num      = pmsghdr->data_num;
	pMsgSnd->msghdr.module_id     = pmsghdr->module_id;
	pMsgSnd->msghdr.sender_id     = pmsghdr->sender_id;
	pMsgSnd->msghdr.msg_type      = pmsghdr->msg_type;
	pMsgSnd->msghdr.msg_subtype   = pmsghdr->msg_subtype;
	pMsgSnd->msghdr.opcode        = pmsghdr->opcode;
	pMsgSnd->msghdr.msg_index     = pmsghdr->msg_index;
	
	if(pdata)
	{
		memcpy(pMsgSnd->msg_data,pdata,pmsghdr->data_len);
	}
	ret = ipc_send_msg_n1(pMsgSnd, sizeof(struct ipc_msghdr_n) + pmsghdr->data_len);
	if( ret != 0)
	{
		mem_share_free(pMsgSnd, MODULE_ID_IFM);
	}
	return;
	
	
}
#endif

int ifm_send_ftm ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )
{
	int ret = 0;

    if ( data_len && NULL == pdata)
    {
        zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
        return -1;
    }
 
    ret = ipc_send_msg_n2(pdata, data_len, 1, MODULE_ID_FTM, MODULE_ID_IFM, IPC_TYPE_IFM, subtype, opcode, msg_index);

    return ret;

}

int ifm_send_hal ( void *pdata, int data_len, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )

{
	int ret = 0;

    if ( data_len && NULL == pdata )
    {
        zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
        return -1;
    }
    
    ret = ipc_send_msg_n2(pdata, data_len, 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_IFM, subtype, opcode, msg_index);
                         
    return ret;
}

/*  下面的代码都可以优化一下              */
/************************************************************
* Function : ifm_msg_send_ack
* return   : 0 ok , not 0 error   
* description: send errcode as reply because of no local data
************************************************************/

int ifm_msg_send_ack( struct ipc_msghdr_n * rcvhdr )
{
    return(ipc_send_reply_n2(NULL, 0, 0, rcvhdr->sender_id, 
                             rcvhdr->module_id, rcvhdr->msg_type, rcvhdr->msg_subtype,  
                             rcvhdr->sequence, rcvhdr->msg_index, IPC_OPCODE_ACK));
}

/************************************************************
* Function : ifm_msg_send_noack
* return   : 0 ok , not 0 error   
* description: send errcode as reply because of no local data
************************************************************/

int ifm_msg_send_noack( uint32_t errcode,struct ipc_msghdr_n * rcvhdr )
{
    return(ipc_send_reply_n2(&errcode, sizeof(uint32_t), 1, rcvhdr->sender_id,
                              rcvhdr->module_id, rcvhdr->msg_type, rcvhdr->msg_subtype, 
                              rcvhdr->sequence,rcvhdr->msg_index, IPC_OPCODE_NACK));
}
/*  下面的代码都可以优化一下              */

/************************************************************
* Function : ifm_send_hal_wait_ack
* return   : 0 ok , not 0 error   
* description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
************************************************************/
int ifm_send_hal_wait_ack ( void *pdata, int data_len,uint16_t data_num,int module_id,int sender_id,
				enum IPC_TYPE msg_type,uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index )
{
	int ret = 0;
	struct ipc_mesg_n * pRcvMsg = NULL;	

    if (data_len &&  NULL == pdata )
	{
		zlog_err ( "[%s %d] pdata is null", __FUNCTION__, __LINE__ );
		return -1;
	}

    pRcvMsg = ipc_sync_send_n2(pdata, data_len, data_num, module_id, 
                               sender_id, msg_type, subtype, opcode, msg_index, 0);
    if(pRcvMsg)
    {
         if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode)
         {
         	memcpy(&ret,pRcvMsg->msg_data,sizeof(int));
         }
         else if( IPC_OPCODE_ACK == pRcvMsg->msghdr.opcode)
         {
         	ret = 0;
         }
         else if( IPC_OPCODE_REPLY == pRcvMsg->msghdr.opcode)
         {
         	ret = 0;
         }
         else  
         {
         	ret = -1;
         }
         mem_share_free(pRcvMsg, MODULE_ID_IFM);           
    }
    else ret = -1;

    return ret;
}
/************************************************************
* Function : ifm_send_hal_wait_reply
* return   : 0 ok , not 0 error   
* description: send set msg to hal and wait reply, used to replace "ipc_send_hal_wait_reply"
************************************************************/
#if 0
int ifm_send_hal_wait_reply(void * pdata,uint32_t data_len,uint16_t data_num,int module_id,
		int sender_id,enum IPC_TYPE msg_type,uint16_t subtype, enum IPC_OPCODE opcode,uint32_t msg_index)
{
	struct ipc_mesg_n * pMsgRcv = NULL;

    pRcvMsg = ipc_sync_send_n2(pdata, data_len, data_num, module_id, 
                               sender_id, msg_type, subtype, opcode, msg_index, 0);
    if(pRcvMsg)
    {




	int iRetv = 0;
	int iRepL = 0;

	struct ipc_mesg_n * pMsgSnd = NULL;
	struct ipc_mesg_n * pMsgRcv = NULL;

	pMsgSnd = mem_share_malloc( sizeof( struct ipc_mesg_n), MODULE_ID_IFM );
	if( pMsgSnd == NULL )
	{
		return -1;
	}
	memset(pMsgSnd, 0,sizeof(struct ipc_mesg_n));

	/*fill msg header*/
	pMsgSnd->msghdr.data_len  	   = data_len;
	pMsgSnd->msghdr.data_num  	   = data_num;
	pMsgSnd->msghdr.module_id 	   = module_id;
	pMsgSnd->msghdr.sender_id 	   = sender_id;
	pMsgSnd->msghdr.msg_type 	   = msg_type;
	pMsgSnd->msghdr.msg_subtype    = msg_subtype;
	pMsgSnd->msghdr.opcode 		   = opcode;
	pMsgSnd->msghdr.msg_index 	   = msg_index;

	/*send info send fail -1 recv fail -2 ok 0*/
	iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);
	switch(iRetv)
	{
		/*send fail*/
		case -1:
		{
			mem_share_free(pMsgSnd, MODULE_ID_IFM);
			pdata = NULL;
			return ERRNO_FAIL;
		}
		case -2:
		{
			/*receive fail*/
			pdata = NULL;
			return ERRNO_FAIL;
		}
		case 0:
		{
			/*receive success*/
			if(pMsgRcv != NULL)
			{
				memcpy(pdata,pMsgRcv->msg_data,\
					pMsgRcv->msghdr.data_len >= data_len ? data_len : pMsgRcv->msghdr.data_len);
				
				/*receive success free memory*/
				mem_share_free(pMsgRcv, MODULE_ID_IFM);
				return ERRNO_SUCCESS;
			}
		}
		default:
			return ERRNO_FAIL;				
	}
	
}
#endif


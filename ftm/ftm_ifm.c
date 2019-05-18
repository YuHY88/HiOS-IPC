/***********************************************************************
*
* ftm_ifm.c
*
*
***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/msg_ipc_n.h>
#include <lib/zassert.h>
#include <lib/linklist.h>
#include <lib/hash1.h>
#include <lib/errcode.h>
#include <route/route_if.h>
#include <route/ndp_cmd.h>
#include <route/arp_cmd.h>
#include "ftm_ifm.h"
#include "ftm.h"
#include "ftm_arp.h"
#include "pkt_arp.h"
#include "ftm_ndp.h"
#include "ftm_pw.h"
#include "ftm_vpls.h"
#include "ftm_fib.h"

struct hash_table ftm_ifm_table;

static unsigned int ftm_ifm_compute_hash ( void *hash_key )
{
    return ( ( unsigned int ) hash_key );
}

static int ftm_ifm_compare ( void *pitem, void *hash_key )
{
    struct hash_bucket *pbucket = ( struct hash_bucket * ) pitem;

    if ( NULL == pitem || NULL == hash_key )
    {
        return ERRNO_FAIL;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/*ftm ?ӿ?hash????ʼ??*/
void ftm_ifm_table_init ( unsigned int size )
{
    hios_hash_init ( &ftm_ifm_table, size, ftm_ifm_compute_hash, ftm_ifm_compare );
}

/*???ӽӿ?*/
int ftm_ifm_add ( struct ifm_info *pifm_info )
{
    struct hash_bucket *pbucket = NULL;
    struct ftm_ifm *pifm = NULL;
    int ret = FTM_IFM_SUCCESS;

    if ( NULL == pifm_info )
    {
        return FTM_IFM_ERR_PARAM;
    }

    pifm = ftm_ifm_lookup ( pifm_info->ifindex );

    if ( pifm != NULL )
    {
        return FTM_IFM_SUCCESS;
    }
    else
    {
        pifm = XMALLOC ( MTYPE_IFM_ENTRY, sizeof ( struct ftm_ifm ) );
    }

    memset ( pifm, 0, sizeof ( struct ftm_ifm ) );
    memcpy ( & ( pifm->ifm ), pifm_info, sizeof ( struct ifm_info ) );
    if ( pifm_info->parent )
    {
        pifm->pparent = ftm_ifm_lookup ( pifm_info->parent );
		zlog_err ( "%s:%s[%d] ifindex 0x%0x parent 0x%0x is NULL  erro\n", 
			__FILE__, __FUNCTION__, __LINE__ , pifm_info->ifindex, pifm_info->parent);
        //assert ( pifm->pparent != NULL );
        if(pifm->pparent == NULL)
        {
	        XFREE ( MTYPE_IFM_ENTRY, pifm );
	        return FTM_IFM_ERR_PARAM;
        }
    }
    else if ( pifm_info->type == IFNET_TYPE_VLANIF )
    {
        ftm_vlanif_create ( pifm );
    }

    pbucket = ( struct hash_bucket * ) XMALLOC ( MTYPE_HASH_BACKET, sizeof ( struct hash_bucket ) );
    if ( NULL == pbucket || NULL == pifm )
    {
        zlog_err ( "%s:%s[%d] pbucket or pifm is NULL  erro\n", __FILE__, __FUNCTION__, __LINE__ );
        return FTM_IFM_ERR_MEM;
    }

    memset ( pbucket, 0, sizeof ( struct hash_bucket ) );
    pbucket->hash_key = ( void * ) pifm->ifm.ifindex;
    pbucket->data     = pifm;

    ret = hios_hash_add ( &ftm_ifm_table, pbucket );
    if ( ret )
    {
        zlog_err ( "%s:%s[%d] hios_hash_add  erro\n", __FILE__, __FUNCTION__, __LINE__ );
		XFREE ( MTYPE_IFM_ENTRY, pifm );
        XFREE ( MTYPE_HASH_BACKET, pbucket );
        return FTM_IFM_ERR_MEM;
    }
    return FTM_IFM_SUCCESS;
}

/*ɾ???ӿ?*/
int ftm_ifm_delete ( uint32_t ifindex )
{
    struct hash_bucket *pbucket = NULL;
    int ret = FTM_IFM_SUCCESS;
    struct ftm_ifm *pifm = NULL;

    pbucket = hios_hash_find ( &ftm_ifm_table, ( void * ) ifindex );
    if ( !pbucket )
    {
        return ERRNO_SUCCESS;
    }

    pifm = ( struct ftm_ifm * ) ( pbucket->data );
    if ( NULL == pifm )
    {
        return ERRNO_FAIL;
    }

    if ( pifm->ifm.type == IFNET_TYPE_VLANIF )
    {
        ftm_vlanif_delete ( ifindex );
    }

    /* ɾ???ӿ?ʱ???ͷ? l2if */
    ftm_ifm_free_l2if ( pifm );

    /* ɾ???ӿ?ʱ???ͷ? l3if */
    ftm_ifm_free_l3if ( pifm );

    zlog_debug (0, "%s[%d]:In function '%s',ifindex:0x%x pifm:%p pifm->parp:%p", __FILE__, __LINE__, __func__, ifindex, pifm, pifm->parp );
    hios_hash_delete ( &ftm_ifm_table, pbucket );
    XFREE ( MTYPE_IFM_ENTRY, pbucket->data );
    XFREE ( MTYPE_HASH_BACKET, pbucket );

    return ret;
}


/* ?ӿ? up ?¼??Ĵ??? */
void ftm_ifm_up ( struct ftm_ifm *pifm )
{
    if ( pifm == NULL )
    {
        return;
    }

    arp_send_if_linkup ( pifm );
}


/* ?ӿ? down ?¼??Ĵ??? */
void ftm_ifm_down ( struct ftm_ifm *pifm )
{
    if ( pifm == NULL )
    {
        return;
    }

    arp_delete_if_linkdown ( pifm->ifm.ifindex);
    ndp_delete_if_linkdown ( pifm->ifm.ifindex);
    arp_delete_vlanif_port_down (pifm->ifm.ifindex);
}


/* ?ӿ? ģʽ?ı? ?¼??Ĵ??? */
void ftm_ifm_mode_change ( struct ftm_ifm *pifm )
{
    zlog_debug (0, "%s[%d]: In function '%s'.\n", __FILE__, __LINE__, __func__ );

    if ( pifm == NULL )
    {
        return;
    }

    if ( pifm->ifm.mode != IFNET_MODE_L2 )
    {
        ftm_ifm_free_l2if ( pifm );
    }

    if ( pifm->ifm.mode != IFNET_MODE_L3 )
    {
        ftm_arpif_addr_delete ( pifm ,0);
		ftm_ndpif_addr_delete ( pifm, 0);
		
        ftm_ifm_free_l3if ( pifm );

    }
}

/*?ӿڲ???*/
struct ftm_ifm *ftm_ifm_lookup ( uint32_t ifindex )
{
    struct hash_bucket *pbucket = NULL;

    pbucket = hios_hash_find ( &ftm_ifm_table, ( void * ) ifindex );

    if ( !pbucket )
    {
        return NULL;
    }
    else
    {
        return pbucket->data;
    }
}


/*  ?????????????ýӿ???Ϣ*/
int ftm_ifm_set_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type )
{
    struct ftm_ifm *pifm = NULL;
    struct ftm_ifm *fpifm = NULL;
    struct ifm_info *pold = NULL;
    int ret = ERRNO_SUCCESS;

    if ( NULL == pinfo )
    {
        return FTM_IFM_ERR_PARAM;
    }

    pifm = ftm_ifm_lookup ( ifindex );
    if ( !pifm )
    {
        zlog_err ( "%s[%d] ftm_ifm_lookup ifindex %d failed\n", __FUNCTION__, __LINE__, ifindex );
        return FTM_IFM_ERR_NOT_FOUND;
    }
    if ( type >=  IFNET_INFO_MAX )
    {
        zlog_err ( "%s[%d] ftm_ifm_lookup ifindex %d failed\n", __FUNCTION__, __LINE__, ifindex );
        return FTM_IFM_ERR_MISMATCH;
    }

    pold = & ( pifm->ifm );
    switch ( type )
    {
        case IFNET_INFO_PARENT:
            if(IFM_TYPE_IS_TRUNK(pifm->ifm.parent)) //trunk??Ա?ڱ?????ɾ????̬arp
            {
                arp_delete_if_linkdown(pifm->ifm.parent);
                ndp_delete_if_linkdown(pifm->ifm.parent);
            }
            if ( * ( uint32_t * ) pinfo == 0 )
            {
                pifm->ifm.parent = 0;
                pifm->pparent    = NULL;
            }
            else
            {
                fpifm = ftm_ifm_lookup ( * ( uint32_t * ) pinfo );
                if ( fpifm )
                {
                    pifm->ifm.parent = * ( uint32_t * ) pinfo;
                    pifm->pparent    = fpifm;
					
				    arp_send_if_linkup(fpifm);
                }
                else
                {
                    zlog_err ( "%s[%d] ftm_ifm_lookup ifindex %d failed\n", __FUNCTION__, __LINE__, * ( uint32_t * ) pinfo );
                    return FTM_IFM_ERR_MISMATCH;
                }
            }
            break;

        case IFNET_INFO_SUBTYPE:
            pold->sub_type =  * ( uint8_t * ) pinfo;
            break;
        case IFNET_INFO_MODE:
            pold->mode =  * ( uint8_t * ) pinfo;
            if ( IFM_IS_SUBPORT ( pifm->ifm.ifindex ) )
            {
                pold->encap.type =  IFNET_ENCAP_INVALID;
            }
            ftm_ifm_mode_change ( pifm );
            break;
        case IFNET_INFO_ENCAP:
            memcpy ( &pold->encap, pinfo, sizeof ( struct ifm_encap ) );
            break;
        case IFNET_INFO_SHUTDOWN:
            pold->shutdown =  * ( uint8_t * ) pinfo;
            break;
        case IFNET_INFO_STATISTICS:
            pold->statistics =  * ( uint8_t * ) pinfo;
            break;
        case IFNET_INFO_TPID:
            pold->tpid =  * ( uint16_t * ) pinfo;
            break;
        case IFNET_INFO_MTU:
            pold->mtu = * ( uint16_t * ) pinfo;
            break;
        case IFNET_INFO_MAC:
            memcpy ( pold->mac, pinfo, 6 );
            break;
        case IFNET_INFO_STATUS:
            {
                pold->status = * ( uint8_t * ) pinfo;

                if ( pold->status == IFNET_LINKUP )
                {
                    ftm_ifm_up ( pifm );
                }
                if ( pold->status == IFNET_LINKDOWN )
                {
                    ftm_ifm_down ( pifm );
                }
                break;
            }
        default:
            return FTM_IFM_ERR_MISMATCH;
    }

    return ret;
}


/*?????????Ż?ȡ??Ϣ*/
int ftm_ifm_get_info ( uint32_t ifindex, void *pinfo, enum IFNET_INFO type )
{
    struct ftm_ifm *pifm = NULL;
    struct ifm_info *pold;

    if ( NULL == pinfo )
    {
        return ERRNO_FAIL;
    }
    pifm = ftm_ifm_lookup ( ifindex );
    if ( !pifm )
    {
        return ERRNO_FAIL;
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
        case IFNET_INFO_USP:
            {
                struct ifm_usp *pusp  = ( struct ifm_usp * ) pinfo;
                pusp->type = pold->type;
                pusp->unit = pold->unit;
                pusp->slot = pold->slot;
                pusp->port = pold->port;
                pusp->sub_port = pold->sub_port;
                break;
            }
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
        case IFNET_INFO_MAC:
            if ( pifm->pparent )
            { memcpy ( ( uchar * ) pinfo, ( uchar * ) ( pifm->pparent->ifm.mac ), MAC_LEN ); }
            else
            { memcpy ( ( uchar * ) pinfo, pold->mac, MAC_LEN ); }
            break;
        case IFNET_INFO_STATUS:
            * ( uint8_t * ) pinfo = pold->status;
            break;
        default:
            return FTM_IFM_ERR_MISMATCH;
    }

    return FTM_IFM_SUCCESS;
}


/* ???? ifm ??Ϣ */
int ftm_ifm_msg ( void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t ifindex )
{
    int ret = 0;
    struct ifm_info *pinfo = NULL;

    /* ifm message do not support batch*/
    if ( data_num != 1 )
    {
        //assert ( 1 );
        zlog_err ( "%s[%d] ifm delete  data_num =%d ifm message do not support batch\n", __FUNCTION__, __LINE__, data_num );
        return -1;
    }

    if ( pdata == NULL )
    {
        zlog_err ( "%s[%d] pdata is NULL\n", __FUNCTION__, __LINE__ );
        return -1;
    }


    if ( opcode == IPC_OPCODE_DELETE )
    {
        ret = ftm_ifm_delete ( ifindex );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ifm delete  erro\n", __FUNCTION__, __LINE__ );
            return ret;
        }
    }
    else if ( opcode == IPC_OPCODE_ADD )
    {
        pinfo = ( struct ifm_info * ) pdata;
        ret = ftm_ifm_add ( pinfo );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ftm_ifm_add  erro\n", __FUNCTION__, __LINE__ );
            return ret;
        }
    }
    else if ( opcode == IPC_OPCODE_UPDATE )
    {
        ret = ftm_ifm_set_info ( ifindex, pdata, subtype );
        if ( ret < 0 )
        {
            zlog_err ( "%s[%d] ftm_ifm_set_info  erro\n", __FUNCTION__, __LINE__ );
            return ret;
        }
    }

    return ret;
}


/* ??ȡ l2if */
struct ftm_ifm_l2 *ftm_ifm_get_l2if ( uint32_t ifindex )
{
    struct ftm_ifm *pifm;

    pifm = ftm_ifm_lookup ( ifindex );
    if ( pifm )
    { return pifm->pl2if; }
    else
    { return NULL; }
}


/* ??ȡ l3if */
struct ifm_l3 *ftm_ifm_get_l3if ( uint32_t ifindex )
{
    struct ftm_ifm *pifm;

    pifm = ftm_ifm_lookup ( ifindex );
    if ( pifm )
    { return pifm->pl3if; }
    else
    { return NULL; }
}


/* ?ҵ??? dip ͬ???εĽӿ? IPv4 ??ַ */
uint32_t ftm_ifm_get_l3if_ipv4(struct ifm_l3 *pl3if, uint32_t dip)
{
    struct ftm_ifm *pifm = NULL;
    int i;
    int ret;

    if ((NULL == pl3if) || (dip == 0))
    {
        return 0;
    }

    if (pl3if->ipv4_flag == IP_TYPE_UNNUMBERED)
    {
        pifm = ftm_ifm_lookup(pl3if->unnumbered_if);
        if ((NULL == pifm) || (NULL == pifm->pl3if))
        {
            return 0;
        }

        pl3if = pifm->pl3if;
    }

	/* ?ҵ??? dip ͬ???εĽӿ? IP */
	for (i=0; i<IFM_IP_NUM; i++)
	{
		if (pl3if->ipv4[i].addr != 0)
		{
			ret = ipv4_is_same_subnet(pl3if->ipv4[i].addr, dip, pl3if->ipv4[i].prefixlen);
			if (0 != ret)
			{
				return pl3if->ipv4[i].addr;
			}
		}
	}

    /*if master or slave ip is not same subnet with dip;
     *     * set outif's master ip as sip;if master ip is NULL,
     *         * set global router id as packet's sip*/
    if (inet_valid_host(pl3if->ipv4[0].addr))
    {
        return (pl3if->ipv4[0].addr);
    }
    else
    {
        return g_fib.router_id;
    }

    return 0;
}


/* ?ҵ??? dip ͬ???εĽӿ? IPv6 ??ַ */
struct ipv6_addr *ftm_ifm_get_l3if_ipv6(struct ifm_l3 *pl3if, struct ipv6_addr *pdip)
{
    int i;
    int ret;

    if ((pl3if == NULL) || (pdip == NULL))
    {
        FTM_LOG_ERROR("pifm->pl3if is Null, drop it!\n");
        return NULL;
    }

    if ((pdip->ipv6[0] == 0xfe) && ((pdip->ipv6[1]&0x80) == 0x80))
    {
        if (ROUTE_IPV6_ADDR_EXIST(pl3if->ipv6_link_local.addr))
        {
            return (struct ipv6_addr *)pl3if->ipv6_link_local.addr;
        }

        return NULL;
    }

    for (i=0; i<IFM_IP_NUM; i++)
    {
        if (ROUTE_IPV6_ADDR_EXIST(pl3if->ipv6[i].addr))
        {
            ret = ipv6_is_same_subnet((struct ipv6_addr *)pl3if->ipv6[i].addr, pdip,
                                        pl3if->ipv6[i].prefixlen);
            if (ret)
            {
                return (struct ipv6_addr *)pl3if->ipv6[i].addr;
            }
        }
    }
#if 0
    /*if master and slave ip is not same subnet with dip or nexthop,then set master 
     * ip as sip;if master sip is NULL,then set global_routeid as sip*/
    if (!ipv6_is_zero((struct ipv6_addr *)pl3if->ipv6[0].addr))
        return (struct ipv6_addr *)pl3if->ipv6[0].addr;
    else
        return (struct ipv6_addr *)(g_fib.routerv6_id.ipv6);
#endif   

    return NULL;
}


/* ???? l2if */
struct ftm_ifm_l2 *ftm_ifm_create_l2if ( struct ftm_ifm *pifm )
{
    struct ftm_ifm_l2 *pl2if = NULL;

    if ( pifm == NULL )
    {
        return NULL;
    }
    else if ( NULL != pifm->pl2if )
    {
        return pifm->pl2if;
    }

    pl2if = ( struct ftm_ifm_l2 * ) XCALLOC ( MTYPE_IFM_ENTRY, sizeof ( struct ftm_ifm_l2 ) );
    if ( NULL != pl2if )
    {
        pifm->pl2if = pl2if;
    }

    return pl2if;
}


/* ???? l3if */
struct ifm_l3 *ftm_ifm_create_l3if ( struct ftm_ifm *pifm )
{
    struct ifm_l3 *pl3if = NULL;

    if ( pifm == NULL )
    {
        return NULL;
    }
    else if ( NULL != pifm->pl3if )
    {
        return pifm->pl3if;
    }

    pl3if = ( struct ifm_l3 * ) XCALLOC ( MTYPE_IFM_ENTRY, sizeof ( struct ifm_l3 ) );
    if ( NULL != pl3if )
    {
        pifm->pl3if = pl3if;
    }

	ftm_arpif_add( pifm );//åå»ºæ¥å£arpæ§å¶ç»æ
	ftm_ndpif_add( pifm );
	
    return pl3if;
}


/* ?ͷ? l2if */
void ftm_ifm_free_l2if ( struct ftm_ifm *pifm )
{
    if ( ( NULL != pifm ) && ( NULL != pifm->pl2if ) )
    {
        if ( pifm->pl2if->backup_index != 0 )
        {
            ftm_pw_delete ( pifm->pl2if->backup_index );
        }

        if ( pifm->pl2if->master_index != 0 )
        {
            ftm_pw_delete ( pifm->pl2if->master_index );
        }

        if ( pifm->pl2if->vsi != 0 )
        {
            ftm_vsi_delete_if ( pifm->pl2if->vsi, pifm->ifm.ifindex );
        }

        XFREE ( MTYPE_IFM_ENTRY, pifm->pl2if );
    }
}


/* ?ͷ? l3if */
void ftm_ifm_free_l3if ( struct ftm_ifm *pifm )
{
    if ( ( NULL != pifm ) && ( NULL != pifm->pl3if ) )
    {
        /*ɾ??l3if ʱ???ͷ? arp*/
		ftm_arpif_addr_delete(pifm, 0);
		ftm_ndpif_addr_delete(pifm, 0);
        ftm_ndpif_addr_delete(pifm, 2);//del linklocal nd entry
		ftm_arpif_delete ( pifm );
		ftm_ndpif_delete ( pifm );

        XFREE ( MTYPE_IFM_ENTRY, pifm->pl3if );
    }
}


/* ???? l3if */
int ftm_l3if_add ( void *pdata, uint32_t ifindex, uint8_t subtype )
{
    struct ftm_ifm *pifm = NULL;
    struct ifm_l3 *pif = NULL;
    int ret;
    int i;

    if ( pdata == NULL )
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    pif = ( struct ifm_l3* )pdata;
    zlog_debug ( 0,"%-15s[%d]: In function 'ftm_l3if_add' ifindex = %d ip: %#x mask: %d vpn: %d\n", __FILE__, __LINE__,
                 ifindex, pif->ipv4[0].addr, pif->ipv4[0].prefixlen, pif->vpn );

    /* ?ӿ???????ϢΪ????????????־ */
    pifm = ftm_ifm_lookup ( ifindex );
    if ( pifm == NULL )
    {
        return ERRNO_NOT_FOUND;
    }

    if ( pifm->pl3if == NULL )
    {
        pifm->pl3if = ftm_ifm_create_l3if ( pifm );
        if ( NULL == pifm->pl3if )
        {
            return ERRNO_MALLOC;
        }
    }


    if ( subtype == L3IF_INFO_IPV4 )        /* ???? IP ??ַ */
    {
        pifm->pl3if->ipv4_flag = pif->ipv4_flag;

        if ( pif->ipv4_flag == IP_TYPE_UNNUMBERED )
        {
            pifm->pl3if->unnumbered_if = pif->unnumbered_if;
		    /*添加arp接口数据结构，如果arp接口数据结构存在则初始化*/
    		ftm_arpif_add ( pifm );
            FTM_ARPIF_CTRL_STRUCT_INIT(pifm);
        }
        else
        {
            for ( i = 0; i < IFM_IP_NUM; i++ )
            {
                if ( pif->ipv4[i].addr != 0 )          /* ???? IP */
                {
					if (pifm->pl3if->ipv4[0].addr != pif->ipv4[0].addr)
					{
						if( i == 0 )
						{
							FTM_ARPIF_CTRL_STRUCT_INIT(pifm);
						}
					}
					
                    pifm->pl3if->ipv4[i] = pif->ipv4[i];

                    if ( pifm->ifm.status == IFNET_LINKUP ) /* ·¢ËÍÃâ·Ñ arp */
                    { arp_send_if_addr ( i, pifm ); }
                }
            }
        }
    }
    else if ( subtype == L3IF_INFO_IPV6 )   /* ???? IPv6 ??ַ */
    {
        pifm->pl3if->ipv6_flag = pif->ipv6_flag;

        if ( pif->ipv6_flag == IP_TYPE_UNNUMBERED )
        {
            pifm->pl3if->unnumbered_if = pif->unnumbered_if;
			ftm_ndpif_add ( pifm );//添加主ip或借用ip时初始化ndp数据结构
        }
        else
        {
            for ( i = 0; i < IFM_IP_NUM; i++ )
            {
                if ( ROUTE_IPV6_ADDR_EXIST(pif->ipv6[i].addr) )          /* ???? IP */
                {
					/*if master ip update and linklocal not exist,init ndpif ctrl struct*/
					if (ROUTE_IPV6_ADDR_EXIST(pif->ipv6[0].addr) &&  
                       !ROUTE_IPV6_ADDR_EXIST(pifm->pl3if->ipv6_link_local.addr)) 
					{
                        FTM_NDP_ZLOG_DEBUG();
						FTM_NDPIF_CTRL_STRUCT_INIT(pifm);
					}
				
                    pifm->pl3if->ipv6[i] = pif->ipv6[i];
                }
            }
        }
    }
    else if ( subtype == L3IF_INFO_VPN )    /* ???? VPN */
    {
        pifm->pl3if->vpn = pif->vpn;

		ret = ftm_msg_send_to_hal ( &pifm->pl3if->vpn, 2, 1, IPC_TYPE_IFM,
                             IFNET_INFO_VPN, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
        if ( ERRNO_SUCCESS != ret )
        {
            zlog_err ( "%s[%d]: In function 'ftm_l3if_add' send vpn failed.\n",
                       __FILE__, __LINE__ );
            return ERRNO_IPC;
        }
    }
    else if ( subtype == L3IF_INFO_IPV6_LINK_LOCAL )
    {
		if (!ROUTE_IPV6_ADDR_EXIST(pifm->pl3if->ipv6[0].addr)) 
		{
            FTM_NDP_ZLOG_DEBUG();
			FTM_NDPIF_CTRL_STRUCT_INIT(pifm);
		}
      
        memcpy(pifm->pl3if->ipv6_link_local.addr, pif->ipv6_link_local.addr, IPV6_ADDR_LEN);
    }

    zlog_debug ( 0,"%-15s[%d]: In function 'ftm_l3if_add' ifindex = %d ip: %#x mask: %d vpn: %d\n", __FILE__, __LINE__,
                 ifindex, pifm->pl3if->ipv4[0].addr, pifm->pl3if->ipv4[0].prefixlen, pifm->pl3if->vpn );

    return ERRNO_SUCCESS;
}

 /* ɾ??3???ӿ???Ϣ */
int ftm_l3if_delete ( void *pdata, uint32_t ifindex, uint8_t subtype )
{
    struct ftm_ifm *pifm = NULL;
    struct ifm_l3 *pif = ( struct ifm_l3 * ) pdata;
    int ret, i;

    assert ( pif != NULL );

    zlog_debug ( 0,"%-15s[%d]: In function 'ftm_l3if_delete' ifindex: %d\n", __FILE__, __LINE__, ifindex );

    pifm = ftm_ifm_lookup ( ifindex );
    if ( pifm == NULL )
    {
        return ERRNO_SUCCESS;
    }

    if ( pifm->pl3if == NULL )
    {
        return ERRNO_SUCCESS;
    }

    /* ɾ??3???ӿ???Ϣ */
    if ( subtype == L3IF_INFO_IPV4 )
    {
        pifm->pl3if->ipv4_flag = 0;

        if ( pif->ipv4_flag == IP_TYPE_UNNUMBERED )
        {
            pifm->pl3if->unnumbered_if = 0;
            ftm_arpif_addr_delete(pifm, 0);
            FTM_ARPIF_CTRL_STRUCT_CLR(pifm);
        }
        else
        {
            for ( i = 0; i < IFM_IP_NUM; i++ )
            {
                if ( pif->ipv4[i].addr != 0 )          /* ???? IP */
                {
                	FTM_ARP_ZLOG_DEBUG("pifm->ip[0]:0x%x pifm->ip[1]:0x%x  pif->ip[0]:0x%x pif->ip[1]:0x%x\n", 
										pifm->pl3if->ipv4[0].addr ,pifm->pl3if->ipv4[1].addr,pif->ipv4[0].addr,pif->ipv4[1].addr);
                   //Ö÷ipÉ¾³ý£¬É¾³ýËùÓÐarp±íÏî²¢Çå¿Õ¿ØÖÆ½á¹¹
					if (pifm->pl3if->ipv4[0].addr == pif->ipv4[0].addr)
					{
						ftm_arpif_addr_delete(pifm, 0);
						ftm_arpif_addr_delete(pifm, 1);
						FTM_ARPIF_CTRL_STRUCT_CLR(pifm);
					}
					else
					{
						//´ÓipÉ¾³ý£¬Ö»É¾³ýÏà¹Øarp±íÏî
						ftm_arpif_addr_delete(pifm, 1);
					}

                    pifm->pl3if->ipv4[i].addr = 0;
                    pifm->pl3if->ipv4[i].prefixlen = 0;
                }
            }
        }
    }
    else if ( subtype == L3IF_INFO_IPV6 )
    {
        pifm->pl3if->ipv6_flag = 0;

        if ( pif->ipv6_flag == IP_TYPE_UNNUMBERED )
        {
            pifm->pl3if->unnumbered_if = 0;
        }
        else
        {
            for ( i = 0; i < IFM_IP_NUM; i++ )
            {
                if ( ROUTE_IPV6_ADDR_EXIST(pif->ipv6[i].addr) )          /* ???? IP */
                {
                	if (ROUTE_IPV6_ADDR_EXIST(pif->ipv6[0].addr)) 
            		{
						ftm_ndpif_addr_delete(pifm, NDP_IPV6_ADDR_MASTER);
                        if ((pifm->parp) && !ROUTE_IPV6_ADDR_EXIST(pifm->pl3if->ipv6_link_local.addr))
                        {
                            FTM_NDP_ZLOG_DEBUG();
                            FTM_NDPIF_CTRL_STRUCT_CLR(pifm);
                        }
					}
					else
					{
						ftm_ndpif_addr_delete(pifm, 1);
					}
				
                    memset(&pifm->pl3if->ipv6[i], 0, sizeof(struct inet_prefixv6));
                }
            }
        }
    }
    else if ( subtype == L3IF_INFO_VPN )
    {
        pifm->pl3if->vpn = 0;

        /*ret = ipc_send_hal ( &pifm->pl3if->vpn, 2, 1, MODULE_ID_HAL, MODULE_ID_FTM, IPC_TYPE_IFM,
                             IFNET_INFO_VPN, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );*/
        ret = ftm_msg_send_to_hal ( &pifm->pl3if->vpn, 2, 1, IPC_TYPE_IFM,
                             IFNET_INFO_VPN, IPC_OPCODE_UPDATE, pifm->ifm.ifindex );
        if ( ERRNO_SUCCESS != ret )
        {
            zlog_err ( "%s[%d]: In function 'ftm_l3if_add' send vpn failed.\n",
                       __FILE__, __LINE__ );
            return ERRNO_IPC;
        }
    }
    else if ( subtype == L3IF_INFO_IPV6_LINK_LOCAL )
    {
        /*linklocal delete, delete related ipv6 neighbors;
         *if master ip not exist,clear ndpif ctrl struct*/
		ftm_ndpif_addr_delete(pifm, NDP_IPV6_ADDR_LINKLOCAL);
        if ((pifm->parp) && !ROUTE_IPV6_ADDR_EXIST(pifm->pl3if->ipv6[0].addr))
        {
            FTM_NDP_ZLOG_DEBUG();
            FTM_NDPIF_CTRL_STRUCT_CLR(pifm);
        }

        memset(pifm->pl3if->ipv6_link_local.addr, 0, IPV6_ADDR_LEN);
    }

    return ERRNO_SUCCESS;
}


/* ???? l3if ??Ϣ */
int ftm_l3if_msg ( void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t index )
{
    int i;

    zlog_debug ( 0,"%-15s[%d]: In function 'ftm_l3_msg' opcode = %d\n", __FILE__, __LINE__, opcode );
    if ( pdata == NULL )
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    for ( i = 0; i < data_num; i++ )
    {
        if ( opcode == IPC_OPCODE_ADD )
        {
            ftm_l3if_add ( pdata, index, subtype );
        }
        else if ( opcode == IPC_OPCODE_DELETE )
        {
            ftm_l3if_delete ( pdata, index, subtype );
        }

        pdata = ( struct ifm_l3 * ) pdata + i;
    }

    return ERRNO_SUCCESS;
}




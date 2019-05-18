/*
*    app call the proto register function to receive protocol packet
*/
#include <string.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/msg_ipc.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include "ftm.h"
#include "pkt_tcp.h"
#include "proto_reg.h"


/* Hash of 协议注册表 */
struct hash_table ip_proto_hash;   /* ip and udp protocol hash table */
struct hash_table eth_proto_hash;  /* eth protocol hash table */
struct hash_table mpls_proto_hash; /* mpls protocol hash table */


/*************************************************************
*函数名:
*	compute
*参数:
*	hash_key--哈希表键值
*返回值:
*	val--转换后的整数
*描述:
*	使用 protocol 作为 hash 键值
**************************************************************/
static unsigned int ip_compute(void *hash_key)
{
	struct ip_proto *proto = (struct ip_proto *)hash_key;

	return proto->protocol; //dhcp注册时dport非空，sport为空，查询时都是非空
}

static unsigned int eth_compute(void *hash_key)
{
	struct eth_proto *proto = (struct eth_proto *)hash_key;

	return proto->ethtype;
}

static unsigned int mpls_compute(void *hash_key)
{
	struct mpls_proto *proto = (struct mpls_proto *)hash_key;

	return proto->chtype;
}


/*************************************************************
*函数名:
*	compare
*参数:
*	item--哈希表节点
*	hash_key--键值
*返回值:
*	0:键值相同
*	1:键值不同
*描述:
*	比较两个键值是否相同
**************************************************************/
static int ip_compare(void *item, void *hash_key)
{
	struct hash_bucket *tmp = (struct hash_bucket *)item;
	struct ip_proto *proto1 = (struct ip_proto *)(tmp->hash_key);
	struct ip_proto *proto2 = (struct ip_proto *)hash_key;

	if(proto1->protocol == proto2->protocol)
	{
		if(proto2->flag == 0)
		{
			if((proto1->dip) && (proto1->dip != proto2->dip))
				return 1;
			else if((proto1->sip) && (proto1->sip != proto2->sip))
				return 1;	
			else if((proto1->dport) && (proto1->dport != proto2->dport))
				return 1;		
			else if((proto1->sport) && (proto1->sport != proto2->sport))
				return 1;
			else if (proto1->type != proto2->type)
				return 1;
			else if((proto1->icmpv6_type_valid) && (proto1->icmpv6_type != proto2->icmpv6_type))
				return 1;
			else if((proto1->sipv6_valid) && !IPV6_ADDR_SAME(&proto1->sipv6, &proto2->sipv6))
				return 1;	
			else if((proto1->dipv6_valid) && !IPV6_ADDR_SAME(&proto1->dipv6, &proto2->dipv6))
				return 1;	
			else
				return 0;
		}
		else
		{
			if ((proto1->dip == proto2->dip)
			&& (proto1->sip == proto2->sip)
			&& (proto1->dport == proto2->dport)
			&& (proto1->sport == proto2->sport)
			&& (proto1->icmpv6_type == proto2->icmpv6_type)
			&& IPV6_ADDR_SAME(&proto1->sipv6, &proto2->sipv6)
			&& IPV6_ADDR_SAME(&proto1->dipv6, &proto2->dipv6))
			{
				return 0;
			}
		}
	}

    return 1;
}


static int eth_compare(void *item, void *hash_key)
{
	struct hash_bucket *tmp = (struct hash_bucket *)item;
	struct eth_proto *proto1 = (struct eth_proto *)(tmp->hash_key);
	struct eth_proto *proto2 = (struct eth_proto *)hash_key;

	if(proto2->flag == 0)
	{
		if((proto1->ethtype) && (proto1->ethtype != proto2->ethtype))
	    	return 1;
		else if((proto1->sub_ethtype) && (proto1->sub_ethtype != proto2->sub_ethtype))			
			return 1;		
		else if((proto1->dmac_valid) && (memcmp(proto1->dmac, proto2->dmac, 6)))
			return 1;
		else if((proto1->smac_valid) && (memcmp(proto1->smac, proto2->smac, 6)))
			return 1;	
		else if((proto1->vlan) && (proto1->vlan != proto2->vlan))
		    return 1;
		else if((proto1->cvlan) && (proto1->cvlan != proto2->cvlan))
		    return 1;
		else if((proto1->oam_opcode) && (proto1->oam_opcode != proto2->oam_opcode))
		    return 1;
		else
			return 0;
	}
	else
	{
		if ((proto1->ethtype == proto2->ethtype)
		&& (proto1->sub_ethtype == proto2->sub_ethtype)
		&& (!memcmp(proto1->dmac, proto2->dmac, 6))
		&& (!memcmp(proto1->smac, proto2->smac, 6))
		&& (proto1->vlan == proto2->vlan)
		&& (proto1->cvlan == proto2->cvlan)
		&& (proto1->oam_opcode == proto2->oam_opcode))
		{
			return 0;
		}
	}
	
	return 1;
}


static int mpls_compare(void *item, void *hash_key)
{
	struct hash_bucket *tmp = (struct hash_bucket *)item;
	struct mpls_proto *proto1 = (struct mpls_proto *)(tmp->hash_key);
	struct mpls_proto *proto2 = (struct mpls_proto *)hash_key;

	if(proto2->flag == 0)
	{
		if(proto1->chtype != proto2->chtype)
			return 1;
		else if(proto1->oam_opcode && (proto1->oam_opcode != proto2->oam_opcode))
			return 1;
		else if(proto1->ttl && (proto1->ttl != proto2->ttl))
			return 1;
		else
			return 0;
	}
	else
	{
		if ((proto1->if_type == proto2->if_type)
		&& (proto1->chtype == proto2->chtype)
		&& (proto1->protocol == proto2->protocol)
		&& (proto1->dip == proto2->dip)
		&& (proto1->dport == proto2->dport)
		&& (proto1->sport == proto2->sport)
		&& (proto1->oam_opcode == proto2->oam_opcode)
		&& (proto1->ttl == proto2->ttl)
		&& (proto1->out_ttl == proto2->out_ttl)
		&& (proto1->inlabel_value == proto2->inlabel_value))
		{
			return 0;
		}
	}
	
	return 1;
}


/* init ip proto hash */
static void ip_proto_hash_init(unsigned int size)
{
    hios_hash_init(&ip_proto_hash, size, ip_compute, ip_compare);
}


/* init eth proto hash */
static void eth_proto_hash_init(unsigned int size)
{
    hios_hash_init(&eth_proto_hash, size, eth_compute, eth_compare);
}


/* init mpls proto hash */
static void mpls_proto_hash_init(unsigned int size)
{
    hios_hash_init(&mpls_proto_hash, size, mpls_compute, mpls_compare);
}

/* init proto hash */
void proto_hash_init(void)
{
	ip_proto_hash_init(proto_hash_size);
	eth_proto_hash_init(proto_hash_size);
	mpls_proto_hash_init(proto_hash_size);
	return;
}


/* add proto to ip_proto_hash */
static int ip_proto_add(struct ip_proto *proto, int module_id)
{
	struct hash_bucket *bucket = NULL;
	struct ip_proto *ip_proto = NULL;
	struct ip_proto *proto1 = NULL;
	int type_flag = 0;
	int ret = 0;

	if( NULL == proto)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	proto->flag = 1;
	bucket = hios_hash_find(&ip_proto_hash, proto);

	/* 已经存在 */
	if(bucket) 
	{
		proto1 = (struct ip_proto *)(bucket->hash_key);
		/*type不一样，其他匹配项一样，只下发到ftm，不再下发acl*/
		if(proto1->type == proto->type)
		{
			bucket->data = (void *)module_id;
			return ERRNO_EXISTED;
		}
		else
		{
			type_flag = 1;
		}
	}

	bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	ip_proto = (struct ip_proto *)XMALLOC(MTYPE_FTM_PROTO_REG, sizeof(struct ip_proto));
	
    if (!bucket || !ip_proto)
    {
        zlog_err("%s[%d]:In function '%s',",__FILE__,__LINE__,__func__);
        return ERRNO_MALLOC;
    }
    memcpy(ip_proto, proto, sizeof(struct ip_proto));	
	bucket->hash_key = ip_proto;
	bucket->data = (void *)module_id;
	bucket->hashval = 0;
	bucket->next = NULL;
	bucket->prev = NULL;
	
    ret = hios_hash_add(&ip_proto_hash, bucket);
	if ( ret != 0 )
    {
    	XFREE ( MTYPE_FTM_PROTO_REG, ip_proto );
		ip_proto = NULL;
        XFREE ( MTYPE_HASH_BACKET, bucket );
		bucket = NULL;
        return ERRNO_FAIL;
    }

	if(type_flag == 1)
	{
		zlog_debug(0,"%s, %d this proto not send to hal !\n",__FUNCTION__, __LINE__);
		return ERRNO_EXISTED;
	}

	return ERRNO_SUCCESS;
}


/* add proto to eth_proto_hash */
static int eth_proto_add(struct eth_proto *proto, int module_id)
{
	struct hash_bucket *bucket = NULL;
	struct eth_proto *eth_proto = NULL;

	proto->flag = 1;
	
	bucket = hios_hash_find(&eth_proto_hash, proto);
	if(bucket) 
	{
		bucket->data = (void *)module_id;
		return ERRNO_EXISTED;
	}

	bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	eth_proto = (struct eth_proto *)XMALLOC(MTYPE_FTM_PROTO_REG, sizeof(struct eth_proto));
    if (!bucket || !eth_proto)
    {
        zlog_err("%s[%d]:In function '%s',",__FILE__,__LINE__,__func__);
        return ERRNO_MALLOC;
    }

    memcpy(eth_proto, proto, sizeof(struct eth_proto));
	bucket->hash_key = eth_proto;
	bucket->data = (void *)module_id;
	bucket->hashval = 0;
	bucket->next = NULL;
	bucket->prev = NULL;
	
    return hios_hash_add(&eth_proto_hash, bucket);
}


/* add proto to ip_proto_hash */
static int mpls_proto_add(struct mpls_proto *proto, int module_id)
{
	struct hash_bucket *bucket = NULL;
	struct mpls_proto *mpls_proto = NULL;
	
	proto->flag = 1;
	
	bucket = hios_hash_find(&mpls_proto_hash, proto);
	if(bucket) 
	{
		bucket->data = (void *)module_id;
		return ERRNO_EXISTED;
	}
	
	bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	mpls_proto = (struct mpls_proto *)XMALLOC(MTYPE_FTM_PROTO_REG, sizeof(struct mpls_proto));
    if (!bucket || !mpls_proto)
    {
        zlog_err("%s[%d]:In function '%s',",__FILE__,__LINE__,__func__);
        return ERRNO_MALLOC;
    }
	
    memcpy(mpls_proto, proto, sizeof(struct mpls_proto));
	bucket->hash_key = mpls_proto;
	bucket->data = (void *)module_id;
	bucket->hashval = 0;
	bucket->next = NULL;
	bucket->prev = NULL;
	
    return hios_hash_add(&mpls_proto_hash, bucket);
}


/* add proto to ip_proto_hash */
static int ip_proto_add_acl_index(struct ip_proto *proto, uint32_t acl_index)
{
	struct hash_bucket *bucket = NULL;
	struct ip_proto *proto1 = NULL;

	if( NULL == proto)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	proto->flag = 1;
	bucket = hios_hash_find(&ip_proto_hash, proto);
	if(bucket) 
	{
		proto1 = (struct ip_proto *)(bucket->hash_key);
		proto1->acl_index = acl_index;
	}

	return ERRNO_SUCCESS;
}


/* add proto to eth_proto_hash */
static int eth_proto_add_acl_index(struct eth_proto *proto, uint32_t acl_index)
{
	struct hash_bucket *bucket = NULL;
	struct eth_proto *eth_proto = NULL;

	proto->flag = 1;
	
	bucket = hios_hash_find(&eth_proto_hash, proto);
	if(bucket) 
	{
		eth_proto = (struct eth_proto *)(bucket->hash_key);
		eth_proto->acl_index = acl_index;
	}

    return ERRNO_SUCCESS;
}


/* add proto to ip_proto_hash */
static int mpls_proto_add_acl_index(struct mpls_proto *proto, uint32_t acl_index)
{
	struct hash_bucket *bucket = NULL;
	struct mpls_proto *mpls_proto = NULL;
	
	proto->flag = 1;
	
	bucket = hios_hash_find(&mpls_proto_hash, proto);
	if(bucket) 
	{
		mpls_proto = (struct mpls_proto *)(bucket->hash_key);
		mpls_proto->acl_index = acl_index;
	}
	
	return ERRNO_SUCCESS;
}


/* return the module_id */
int ip_proto_lookup(struct ip_proto *proto)
{
	proto->flag = 0;
	
   struct hash_bucket *bucket = hios_hash_find(&ip_proto_hash, proto);

   if(bucket) 
   {
      return (int)(bucket->data);
   }

   return 0;
}


/* return the module_id */
int eth_proto_lookup(struct eth_proto *proto)
{
	proto->flag = 0;
	
   struct hash_bucket *bucket = hios_hash_find(&eth_proto_hash, proto);

   if(bucket) 
   {
      return (int)(bucket->data);
   }

   return 0;
}


/* return the module_id */
int mpls_proto_lookup(struct mpls_proto *proto)
{
	proto->flag = 0;
	
   struct hash_bucket *bucket = hios_hash_find(&mpls_proto_hash, proto);

   if(bucket) 
   {
      return (int)(bucket->data);
   }

   return 0;
}


/* 删除协议节点 */
int ip_proto_delete(struct ip_proto *proto)
{
	struct hash_bucket *bucket = NULL;
	struct ip_proto *proto1 = NULL;
	uint32_t acl_index = 0;
	
	if( NULL == proto)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	proto->flag = 1;
	
	bucket = hios_hash_find(&ip_proto_hash, proto);
	if(bucket) 
	{	
		proto1 = (struct ip_proto *)(bucket->hash_key);
		acl_index = proto1->acl_index;

		hios_hash_delete(&ip_proto_hash, bucket);
		if(bucket->hash_key)
		{
			XFREE(MTYPE_FTM_PROTO_REG, bucket->hash_key);
		}
			
		XFREE(MTYPE_HASH_BACKET, bucket);
	}

   return acl_index;
}

int eth_proto_delete(struct eth_proto *proto)
{
	struct hash_bucket *bucket = NULL;
	struct eth_proto *proto1 = NULL;
	uint32_t acl_index = 0;

	proto->flag = 1;
	
	bucket = hios_hash_find(&eth_proto_hash, proto);

	if(bucket) 
	{
		proto1 = (struct eth_proto *)(bucket->hash_key);
		acl_index = proto1->acl_index;

		hios_hash_delete(&eth_proto_hash, bucket);
		if(bucket->hash_key)
			XFREE(MTYPE_FTM_PROTO_REG, bucket->hash_key);

		XFREE(MTYPE_HASH_BACKET, bucket);
	}

   return acl_index;
}


/* return the module_id */
int mpls_proto_delete(struct mpls_proto *proto)
{
   	struct hash_bucket *bucket = NULL;
   	struct mpls_proto *proto1 = NULL;
	uint32_t acl_index = 0;

	proto->flag = 1;
	
	bucket = hios_hash_find(&mpls_proto_hash, proto);
	if(bucket) 
	{
		proto1 = (struct mpls_proto *)(bucket->hash_key);
		acl_index = proto1->acl_index;

		hios_hash_delete(&mpls_proto_hash, bucket);
		if(bucket->hash_key)
			XFREE(MTYPE_FTM_PROTO_REG, bucket->hash_key);

		XFREE(MTYPE_HASH_BACKET, bucket);
	}

   return acl_index;
}


/*
APP  注册接收协议报文的类型
*/
int ftm_proto_msg(void *pdata, int module_id, uint8_t subtype, enum IPC_OPCODE opcode)
{
    int ret = -1;
	void *index = NULL;
	uint32_t acl_index = 0; 
	struct ip_proto *ip_proto = NULL;
	struct eth_proto *eth_proto = NULL;
	struct mpls_proto *mpls_proto = NULL;

	struct ip_proto ip_proto1;
	struct eth_proto eth_proto1;
	struct mpls_proto mpls_proto1;
	struct ipc_mesg_n * pMsgRcv = NULL;

    if(!pdata)
		return -1;
	
	switch (subtype)
	{
		case PROTO_TYPE_IPV4:
		case PROTO_TYPE_IPV6:
		{
			ip_proto = (struct ip_proto *)pdata;
			
			if(opcode == IPC_OPCODE_REGISTER)
			{
				ret = ip_proto_add(ip_proto, module_id);
				if(ret == 0)
				{
					/*index = ipc_send_hal_wait_reply( (void *)ip_proto, sizeof(struct ip_proto), 1, MODULE_ID_HAL, MODULE_ID_FTM, 
							IPC_TYPE_PROTO, subtype, opcode,0);*/
					pMsgRcv = ipc_sync_send_n2( (void *)ip_proto, sizeof(struct ip_proto), 1, MODULE_ID_HAL, MODULE_ID_FTM, 
							IPC_TYPE_PROTO, subtype, opcode, 0, 0);
					if(pMsgRcv)
					{
					    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
				        {
				            ret = ip_proto_add_acl_index(ip_proto, *(uint32_t*)pMsgRcv->msg_data );
				        }

				        mem_share_free( pMsgRcv, MODULE_ID_FTM );
					}
				}
			}
			else if(opcode == IPC_OPCODE_UNREGISTER)
			{
				memcpy(&ip_proto1,ip_proto,sizeof(struct ip_proto));
				
				acl_index = ip_proto_delete(ip_proto);
				
				ip_proto1.acl_index = acl_index;

				if(ip_proto1.protocol == 6)
				{
					tcp_session_close(&ip_proto1);
				}

				ret = ftm_msg_send_to_hal(&ip_proto1, sizeof(struct ip_proto), 1, IPC_TYPE_PROTO, subtype, opcode, 0);
			}
			else 
				return -1;
			
            break;
		}
		case PROTO_TYPE_ETH:
		{
			eth_proto = (struct eth_proto *)pdata;

			if(opcode == IPC_OPCODE_REGISTER)
			{
				ret = eth_proto_add(eth_proto, module_id);
				if(ret == 0)
				{
					/*index = ipc_send_hal_wait_reply( (void *)eth_proto, sizeof(struct eth_proto), 1, MODULE_ID_HAL, MODULE_ID_FTM, 
							IPC_TYPE_PROTO, subtype, opcode,0);*/
					pMsgRcv = ipc_sync_send_n2( (void *)eth_proto, sizeof(struct eth_proto), 1, MODULE_ID_HAL, MODULE_ID_FTM, 
							IPC_TYPE_PROTO, subtype, opcode, 0, 0 );
					if(pMsgRcv)
					{
					    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
				        {
				            ret = eth_proto_add_acl_index(eth_proto, *(uint32_t*)pMsgRcv->msg_data  );
				        }

				        mem_share_free( pMsgRcv, MODULE_ID_FTM );
					}
				}
			}			
			else if(opcode == IPC_OPCODE_UNREGISTER)
			{
				memcpy(&eth_proto1,eth_proto,sizeof(struct eth_proto));

				acl_index = eth_proto_delete(eth_proto);

				eth_proto1.acl_index = acl_index;

				ret = ftm_msg_send_to_hal(&eth_proto1, sizeof(struct eth_proto), 1, IPC_TYPE_PROTO, PROTO_TYPE_ETH, opcode, 0);
				
			}
			else 
				return -1;
			
            break;
		}
		case PROTO_TYPE_MPLS:
		{
			mpls_proto = (struct mpls_proto *)pdata;
			
			if(opcode == IPC_OPCODE_REGISTER)
			{
				ret = mpls_proto_add(mpls_proto, module_id);
				if(ret == 0)

				{
					/* send msg to hal*/ 
					/*index = ipc_send_hal_wait_reply( (void *)mpls_proto, sizeof(struct mpls_proto), 1, MODULE_ID_HAL, MODULE_ID_FTM, 
							IPC_TYPE_PROTO, subtype, opcode,0);*/
					pMsgRcv = ipc_sync_send_n2( (void *)mpls_proto, sizeof(struct mpls_proto), 1, MODULE_ID_HAL, MODULE_ID_FTM, 
							IPC_TYPE_PROTO, subtype, opcode, 0, 0 );
					if(pMsgRcv)
					{
					    if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
				        {
				            ret = mpls_proto_add_acl_index(mpls_proto, *(uint32_t*)pMsgRcv->msg_data );
				        }

				        mem_share_free( pMsgRcv, MODULE_ID_FTM );
					}
				}
			}
			else if(opcode == IPC_OPCODE_UNREGISTER)
			{
				memcpy(&mpls_proto1,mpls_proto,sizeof(struct mpls_proto));

				acl_index = mpls_proto_delete(mpls_proto);

				mpls_proto1.acl_index = acl_index;

				ret = ftm_msg_send_to_hal(&mpls_proto1, sizeof(struct mpls_proto), 1, IPC_TYPE_PROTO, PROTO_TYPE_MPLS, opcode, 0);
			}
			else 
				return -1;

            break;
		}
        default:
        {
            break;
        }
	}
	
    return ret;
}



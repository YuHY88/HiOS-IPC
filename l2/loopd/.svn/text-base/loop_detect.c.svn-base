/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*/
#include <lib/pkt_type.h>
#include <lib/memory.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/thread.h>
#include <lib/vty.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/hash1.h>
#include "l2_if.h"
#include "lib/log.h"
#include "loop_detect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/devm_com.h>
#include <lib/alarm.h>

extern struct thread_master	*l2_master;

/*loopdetect 全局数据结构*/
struct loopdetect_global g_loopdetect_data;

/* 接口下的 loopdetect 配置保存 */
void l2if_loopdetect_config_write(struct vty *vty, struct loop_detect *plpd)
{
	sint8 cmd[128];
	uint32_t len = 0;
		
	if (plpd->vlan != LOOPD_DEF_VLAN_ID)
		len += sprintf(cmd + len, " loop-detect vlan %d ", plpd->vlan);
	else
		len += sprintf(cmd + len, " loop-detect ");
	
	//if (plpd->interval != LOOPD_DEF_INTERVAL)
		len += sprintf(cmd + len, "interval %d ", plpd->interval);

	//if (plpd->restore_time != LOOPD_DEF_RES_TIME)
		len += sprintf(cmd + len, "restore-time %d ", plpd->restore_time);
	
	if (plpd->act == PORT_ACTION_SHUTDOWN)
		len += sprintf(cmd + len, "action shutdown");
	else if(plpd->act == PORT_ACTION_BLOCK)
		len += sprintf(cmd + len, "action block");
	cmd[len] = '\0';
	
	vty_out(vty, "%s%s", cmd, VTY_NEWLINE);

	return ;
}


/* 查找传入 ifindex 之后对应的 n 个数据 */
int loopdetect_snmp_port_info_get(uint32_t ifindex, struct  loop_detect_snmp loopdetect_snmp[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct l2if   *pif     = NULL;
	struct loop_detect_snmp ploopdetect;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct loop_detect_snmp);
    int cursor = 0;
    int val = 0;
	int i = 0;

    if (0 == ifindex)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
        {
            pif = (struct l2if *)pbucket->data;
            if ((NULL == pif) || (NULL == pif->ploopdetect))
            {
                continue;
            }

			ploopdetect.ifindex = pif->ifindex;
			memcpy(&(ploopdetect.loopdetect), pif->ploopdetect, sizeof(struct loop_detect));
			memcpy(&loopdetect_snmp[data_num++], &ploopdetect, sizeof(struct loop_detect_snmp));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else /* 找到 ifindex 后面第一个数据 */
    {
        pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
        if (NULL == pbucket)    /* 未找到 ifindex 对应的数据 */
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val %= l2if_table.compute_hash((void *)ifindex);

            if (NULL != l2if_table.buckets[val])    /* 当前哈希桶链表不为空 */
            {
                pbucket = l2if_table.buckets[val];
            }
            else                                        /* 当前哈希桶链表为空 */
            {
                for (++val; val<HASHTAB_SIZE; ++val)
            	{
            		if (NULL != l2if_table.buckets[val])
            		{
            			pbucket = l2if_table.buckets[val];
            		}
                }
            }
        }

        if (NULL != pbucket)
        {
            for (i=0; i<msg_num; i++)
            {
                pnext = hios_hash_next_cursor(&l2if_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = pnext->data;
			    if ((NULL == pif) || (NULL == pif->ploopdetect))
			    {
				   continue;
			    }

				ploopdetect.ifindex = pif->ifindex;
				memcpy(&(ploopdetect.loopdetect), pif->ploopdetect, sizeof(struct loop_detect));
				memcpy(&loopdetect_snmp[data_num++], &ploopdetect, sizeof(struct loop_detect_snmp));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}

/* 端口环路状态设置 */
int loopdetect_status_set(struct l2if *pl2_if, enum LOOPD_FLAG loop_flag)
{
	sint32 ret = 0;
	char ifname[IFNET_NAMESIZE];

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	/*防止删除端口环路检测，定时器队列导致进程退出*/
	if (pl2_if->ploopdetect == NULL)
	{
		return ERRNO_FAIL;
	}

	if (!pl2_if->ploopdetect->loop_flag && loop_flag)
	{
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pl2_if->ifindex),IFM_PORT_ID_GET(pl2_if->ifindex),0,0,GPN_ALM_TYPE_FUN_ETH_LOOP_BACK,GPN_SOCK_MSG_OPT_RISE);

		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = pl2_if->ifindex;
		ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_LOOP_BACK, GPN_SOCK_MSG_OPT_RISE);
	}
         else if(pl2_if->ploopdetect->loop_flag&&!loop_flag) 
	{
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pl2_if->ifindex),IFM_PORT_ID_GET(pl2_if->ifindex),0,0,GPN_ALM_TYPE_FUN_ETH_LOOP_BACK,GPN_SOCK_MSG_OPT_CLEAN);                        

		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = pl2_if->ifindex;
		ipran_alarm_report( &gPortInfo, GPN_ALM_TYPE_FUN_ETH_LOOP_BACK,GPN_SOCK_MSG_OPT_CLEAN); 
        }		
	pl2_if->ploopdetect->loop_flag = loop_flag;
/*
	if(pl2_if->ploopdetect->act == PORT_ACTION_INVALID)
	{
		return ERRNO_FAIL;
	}
*/
	/*获取端口名称*/
	ifm_get_name_by_ifindex(pl2_if->ifindex, ifname);
	
	if (pl2_if->ploopdetect->act == PORT_ACTION_BLOCK && loop_flag == LOOPD_LOOPBACK)
	{
		ret = ifm_set_block(pl2_if->ifindex, MODULE_ID_L2, IFNET_STP_BLOCK);
		if (ret < 0)
		{
			zlog_err("%s[%d]:%s:LPDT_LOOPED: set block error on %s\n",__FILE__,__LINE__,__func__,ifname);
			return ret;
		}
		pl2_if->ploopdetect->status = PORT_STATUS_BLOCK;
		zlog_warn("%s[%d]:%s:LPDT_LOOPED: Loopback exists on %s\n",__FILE__,__LINE__,__func__,ifname);
		//printf("LPDT_LOOPED: Loopback exists on %s\n", ifname);
	}
	else if (pl2_if->ploopdetect->act == PORT_ACTION_SHUTDOWN && loop_flag == LOOPD_LOOPBACK)
	{
		ret = l2if_set_shutdown(pl2_if->ifindex, IFNET_SHUTDOWN);
		if (ret < 0)
		{
			zlog_err("%s[%d]:%s:LPDT_LOOPED: set shutdown error on %s\n",__FILE__,__LINE__,__func__,ifname);
			return ret;
		}
		pl2_if->ploopdetect->status = PORT_STATUS_SHUTDOWN;
		zlog_warn("%s[%d]:%s:LPDT_LOOPED: Loopback exists on %s\n",__FILE__,__LINE__,__func__,ifname);
		//printf("LPDT_LOOPED: Loopback exists on %s\n", ifname);
	}
	else if (pl2_if->ploopdetect->act == PORT_ACTION_BLOCK && loop_flag == LOOPD_FORWARD)
	{
		ret = ifm_set_block(pl2_if->ifindex, MODULE_ID_L2, IFNET_STP_FORWARD);
		if (ret < 0)
		{
			zlog_err("%s[%d]:%s:LPDT_LOOPED: set forward error on %s\n",__FILE__,__LINE__,__func__,ifname);
			return ret;
		}
		pl2_if->ploopdetect->status = PORT_STATUS_FORWARD;
		zlog_warn("%s[%d]:%s:LPDT_RECOVERED: Loopback on %s recovered\n",__FILE__,__LINE__,__func__,ifname);
		//printf("LPDT_RECOVERED: Loopback on %s recovered\n", ifname);
	}
	else if (pl2_if->ploopdetect->act == PORT_ACTION_SHUTDOWN && loop_flag == LOOPD_FORWARD)
	{
		ret = l2if_set_shutdown(pl2_if->ifindex, IFNET_NO_SHUTDOWN);
		if (ret < 0)
		{
			zlog_err("%s[%d]:%s:LPDT_LOOPED: set forward error on %s\n",__FILE__,__LINE__,__func__,ifname);
			return ret;
		}
		pl2_if->ploopdetect->status = PORT_STATUS_FORWARD;
		zlog_warn("%s[%d]:%s:LPDT_RECOVERED: Loopback on %s recovered\n",__FILE__,__LINE__,__func__,ifname);
		//printf("LPDT_RECOVERED: Loopback on %s recovered\n", ifname);
	}

	

	return ret;
}


int loopdetect_restore_proc(void  * arg)
{
	struct l2if *pl2_if = NULL;

	if (NULL == arg)
	{
		return ERRNO_FAIL;
	}
	
	pl2_if = (struct l2if *)arg;
         if(NULL==pl2_if->ploopdetect)
         {
         	 return ERRNO_FAIL;
	}
	pl2_if->ploopdetect->ptimer2 = NULL;
	loopdetect_status_set(pl2_if, LOOPD_FORWARD);
	
	return ERRNO_SUCCESS;
}

static void loopdetect_pkt_send_dump(union pkt_control *p_pkt_ctl, struct loopd_frame *loopd_pkt, uint32_t ifindex)
{
	char ifname[IFNET_NAMESIZE];
	int ret = 0;
	char *pktinfo = NULL;
	
	/*打印发送报文*/
	if (g_loopdetect_data.loopdetect_debug)
	{
		ret = ifm_get_name_by_ifindex(ifindex, ifname);
		if(ret < 0) assert(0);
		printf("%s send loopdetect packet: \n", ifname);
		pktinfo = pkt_dump((void *)p_pkt_ctl, sizeof(union pkt_control));
		if(pktinfo != NULL)
		{
			printf("%s", pktinfo);
		}
		pktinfo = pkt_dump((void *)loopd_pkt, sizeof(struct loopd_frame));
		if(pktinfo != NULL)
		{
			printf("%s", pktinfo);
		}		
	}

    return ;
}

static void  loopdetect_pkt_recv_dump(union pkt_control *p_pkt_ctl, struct loopd_frame *loopd_pkt, uint32_t ifindex)
{
    char ifname[IFNET_NAMESIZE];
    int ret = 0;
	char *pktinfo = NULL;
	
    /*打印接收报文*/
	if (g_loopdetect_data.loopdetect_debug)
    {
        ret = ifm_get_name_by_ifindex(ifindex, ifname);
        if(ret < 0) assert(0);
        printf("%s receive loopdetect packet: \n",ifname);
        pktinfo = pkt_dump(p_pkt_ctl, sizeof(union pkt_control));
		if(pktinfo != NULL)
		{
			printf("%s", pktinfo);
		}
        pktinfo = pkt_dump(loopd_pkt, sizeof(struct loopd_frame));
		if(pktinfo != NULL)
		{
			printf("%s", pktinfo);
		}
    }

    return ;
}

/* 定期在端口发送检测报文 */
int loopdetect_pkt_send(void * arg)
{
	struct l2if *pl2_if = NULL;
	/*报文控制信息主机序:FTM统一转换为网络序*/
	union pkt_control pktcontrol;
	/*payload网络序:FTM不做转换*/
	struct loopd_frame loopd_pdu;
	uint8_t loopd_dmac[ETH_ALEN] = LOOPD_MC_MAC;
    char ifname[IFNET_NAMESIZE];
	
	pl2_if = (struct l2if *)arg;
	if (pl2_if->ploopdetect == NULL)
	{
		return ERRNO_SUCCESS;
	}
	
	ifm_get_name_by_ifindex (pl2_if->ifindex, ifname);
	
	/*eth header */
	memset(&pktcontrol, 0, sizeof(union pkt_control));
	memcpy(pktcontrol.ethcb.dmac, loopd_dmac, ETH_ALEN);
	memcpy(pktcontrol.ethcb.smac, g_loopdetect_data.source, ETH_ALEN);
	if (pl2_if->ploopdetect->vlan != LOOPD_DEF_VLAN_ID)
	{
		pktcontrol.ethcb.svlan = pl2_if->ploopdetect->vlan;
		pktcontrol.ethcb.svlan_tpid = ETH_P_8021Q;
	}

	pktcontrol.ethcb.ethtype = ETH_P_LOOPD;
	pktcontrol.ethcb.smac_valid = 1;
	pktcontrol.ethcb.is_changed = 1;
	pktcontrol.ethcb.ifindex = pl2_if->ifindex;
	pktcontrol.ethcb.cos = 0;
	
	/*loopd headr*/
	memset(&loopd_pdu, 0, sizeof(struct loopd_frame));
	loopd_pdu.ldhdr.h_code = htons(LOOPD_SUBTYPE);
	loopd_pdu.ldhdr.h_ver = htons(LOOPD_VERSION);
	loopd_pdu.ldhdr.h_resv = 0;
	loopd_pdu.ldhdr.h_length = htons(sizeof(struct loopd_frame));

	/*device id tlv*/
	loopd_pdu.tlv.devid_type = LOOPD_DEV_ID_TLV;
	loopd_pdu.tlv.devid_len = ETH_ALEN;
	memcpy(loopd_pdu.tlv.devid_val, g_loopdetect_data.source, MAC_LEN);
	
	/*port ifindex tlv*/
	loopd_pdu.tlv.if_type = LOOPD_PORT_IFINDEX_TLV;
	loopd_pdu.tlv.if_len = sizeof(uint32_t);
	loopd_pdu.tlv.if_val = htonl(pl2_if->ifindex);
	
	/*port name tlv*/
	loopd_pdu.tlv.pname_type = LOOPD_PORT_NAME_TLV;
	loopd_pdu.tlv.pname_len = IFNET_NAMESIZE;
	memcpy(loopd_pdu.tlv.pname_val, ifname, IFNET_NAMESIZE);
	
	pkt_send(PKT_TYPE_ETH, &pktcontrol, &loopd_pdu, sizeof(struct loopd_frame));
	loopdetect_pkt_send_dump(&pktcontrol, &loopd_pdu, pl2_if->ifindex);
	
	/*tx ++*/
	pl2_if->ploopdetect->pkt_send++;

	return ERRNO_SUCCESS;
}

/* 端口收到报文则说明出现环路 */
int loopdetect_pkt_recv(struct pkt_buffer *pkt)
{
	struct l2if *l2_if = NULL; 
	struct l2if * temp_l2if = NULL;
	struct loopd_frame loopd_pdu;
	sint32 ret = ERRNO_SUCCESS;
	
	/*get pif by real inport*/
	l2_if = l2if_lookup(pkt->in_port);
	if (NULL == l2_if  ||NULL == l2_if->ploopdetect)
	{
		   return ERRNO_FAIL;
	 }
	   
    /* 保存pdu信息,payload为网络字节序需要转换为主机字节序*/
    memcpy(&loopd_pdu, pkt->data, sizeof(struct loopd_frame));
	loopdetect_pkt_recv_dump(&pkt->cb, &loopd_pdu, l2_if->ifindex);
	
	if (ntohs(loopd_pdu.ldhdr.h_code) == LOOPD_SUBTYPE)
	{
			/*rx++*/
		l2_if->ploopdetect->pkt_recv++;
			/*loopback times++*/
		l2_if->ploopdetect->loop_times++;
			
		/*deal with port of ifindex value is lower 2018/6/19 youcheng*/
		
		 temp_l2if = ntohl(loopd_pdu.tlv.if_val) >= l2_if->ifindex ? l2_if : l2if_lookup(ntohl(loopd_pdu.tlv.if_val));	 
		
		 if (NULL == temp_l2if  ||NULL == temp_l2if->ploopdetect)
		 {
		 	   return ERRNO_FAIL;
		 }

		 if ((temp_l2if->ploopdetect->status == PORT_STATUS_BLOCK) ||
			(temp_l2if->ploopdetect->status == PORT_STATUS_SHUTDOWN))
		{      
			if (temp_l2if->ploopdetect->act != PORT_ACTION_INVALID)
			{        
				//THREAD_TIMER_OFF(temp_l2if->ploopdetect->ptimer2);
				//temp_l2if->ploopdetect->ptimer2 = thread_add_timer(l2_master, loopdetect_restore_proc,temp_l2if,temp_l2if->ploopdetect->restore_time);
				LOOPDETECT_TIMER_OFF(temp_l2if->ploopdetect->ptimer2);
				LOOPDETECT_TIMER_ON_UNLOOP(temp_l2if->ploopdetect->ptimer2,loopdetect_restore_proc,temp_l2if,temp_l2if->ploopdetect->restore_time);
					return ERRNO_FAIL;
			}
		}
			
			
		ret = loopdetect_status_set(temp_l2if, LOOPD_LOOPBACK);
		if (ret != 0)
		{
			return ERRNO_FAIL;
		}
			
		    //THREAD_TIMER_OFF(temp_l2if->ploopdetect->ptimer2);
			//temp_l2if->ploopdetect->ptimer2 = thread_add_timer(l2_master, loopdetect_restore_proc,temp_l2if,temp_l2if->ploopdetect->restore_time);
			LOOPDETECT_TIMER_OFF(temp_l2if->ploopdetect->ptimer2);
			LOOPDETECT_TIMER_ON_UNLOOP(temp_l2if->ploopdetect->ptimer2,loopdetect_restore_proc,temp_l2if,temp_l2if->ploopdetect->restore_time);
				
		
	}
	
	return ERRNO_SUCCESS;
}

void loopdetect_data_init(void)
{
	int ret = 0;


    /*获取设备桥MAC ,框式为背板MAC*/
	ret = devm_comm_get_mac ( 1, 0, MODULE_ID_L2,g_loopdetect_data.source);
    if (ret)
    {
		zlog_err ( "%s[%d]:leave %s:ifm get mac from devm error,mac == NULL\n", __FILE__, __LINE__, __func__ );
    }		
}

void loopdetect_pkt_register(void)
{
	union proto_reg proto;
	uint8_t loopd_dmac[ETH_ALEN] = LOOPD_MC_MAC;

	/*报文注册为主机序,hsl下发转换为网络序*/
	memset(&proto, 0, sizeof(union proto_reg));
	memcpy(proto.ethreg.dmac, loopd_dmac, ETH_ALEN);
	memcpy(proto.ethreg.smac, g_loopdetect_data.source, ETH_ALEN);
	proto.ethreg.dmac_valid = 1;
	proto.ethreg.smac_valid = 1;
	proto.ethreg.ethtype = ETH_P_LOOPD;
	
	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);

}

void loopdetect_pkt_unregister(void)
{
	union proto_reg proto;
	uint8_t loopd_dmac[ETH_ALEN] = LOOPD_MC_MAC;
	
	memset(&proto, 0, sizeof(union proto_reg));
	memcpy(proto.ethreg.dmac, loopd_dmac, ETH_ALEN);
	memcpy(proto.ethreg.smac, g_loopdetect_data.source, ETH_ALEN);
	proto.ethreg.dmac_valid = 1;
	proto.ethreg.smac_valid = 1;
	proto.ethreg.ethtype = ETH_P_LOOPD;
	
	pkt_unregister(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);

}

void loopdetect_init(void)
{
	/*环路检测命令行初始化*/
	loopdetect_cli_init();

	/*环路检测数据初始化*/
	loopdetect_data_init();
	
	/*环路检测PKT注册*/
	loopdetect_pkt_register();
}

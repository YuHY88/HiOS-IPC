#include <string.h>
#include <lib/types.h>
#include <l2/vlan.h>
#include <lib/log.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/zassert.h>

#include "ftm_vlan.h"
#include "ftm_ifm.h"
#include "ftm_arp.h"

struct ftm_vlan ftm_vlan_table[VLAN_MAX];


/* 初始化 vlan 数组 */
void ftm_vlan_table_init(void)
{
    uint16_t vlanid=0;
    for(vlanid = 0; vlanid < VLAN_MAX; vlanid++)
    {
		memset(&(ftm_vlan_table[vlanid]), 0 , sizeof(struct ftm_vlan));
        ftm_vlan_table[vlanid].vlan.enable = DISABLE;
        ftm_vlan_table[vlanid].vlan.vlanid = vlanid;
        ftm_vlan_table[vlanid].vlan.mac_learn = ENABLE;
        ftm_vlan_table[vlanid].vlan.limit_num = 0;
    }
	zlog_info("%s[%d]: leave function '%s'.\n", __FILE__, __LINE__, __func__);
}

struct ftm_vlan *ftm_vlan_lookup(uint16_t vlan_id)
{
	zlog_debug(0,"%s[%d]: function '%s'. return ftm_vlan_table[%d]\n", __FILE__, __LINE__, __func__,vlan_id);
	return &ftm_vlan_table[vlan_id];
}


uint32_t ftm_vlan_get_vlanif(uint16_t vlan_id)
{
	return ftm_vlan_table[vlan_id].ifindex;
}


int ftm_vlan_enable(struct vlan_range *pvlan)
{
    uint16_t vlanid=0;

	zlog_debug(0,"%s[%d]: leave function '%s'. vlan_start=%d,vlan_end=%d \n", __FILE__, __LINE__, __func__,pvlan->vlan_start,pvlan->vlan_end);

    for(vlanid=pvlan->vlan_start;vlanid<=pvlan->vlan_end;vlanid++)
    {
        ftm_vlan_table[vlanid].vlan.enable = ENABLE;
    }
    return 0;
}


int ftm_vlan_disable(struct vlan_range *pvlan)
{
    uint16_t vlanid=0;
	
    for(vlanid=pvlan->vlan_start;vlanid<=pvlan->vlan_end;vlanid++)
    {
        ftm_vlan_table[vlanid].vlan.enable = DISABLE;
    }
    return 0;
}


/* vlan 添加端口 */
int ftm_vlan_add_port(struct vlan_range *pvlan, uint32_t ifindex)
{
    uint16_t vlanid = 0;

    for(vlanid=pvlan->vlan_start;vlanid<=pvlan->vlan_end;vlanid++)
    {
        listnode_add (&(ftm_vlan_table[vlanid].portlist), (void*)ifindex);
    }

    return 0;
}


/* vlan 删除端口 */
int ftm_vlan_delete_port(struct vlan_range *pvlan, uint32_t ifindex)
{
    uint16_t vlanid = 0;
    uint32_t vlanif_ifidx = 0;
    
    for(vlanid=pvlan->vlan_start;vlanid<=pvlan->vlan_end;vlanid++)
    {
		listnode_delete (&(ftm_vlan_table[vlanid].portlist), (void*)ifindex);
        
        vlanif_ifidx = ftm_vlan_get_vlanif(vlanid);
        if (vlanif_ifidx && IFM_TYPE_IS_VLANIF(vlanif_ifidx)) 
            arp_delete_if_linkdown(vlanif_ifidx);
    }
	
    return 0;
}


/* 创建 vlanif 接口，vlan 中记录 ifindex */
int ftm_vlanif_create(struct ftm_ifm *pvlanif)
{
	struct ftm_vlan   *pvlan = NULL;
	uint16_t vlan;

	vlan = IFM_VLANIF_ID_GET(pvlanif->ifm.ifindex);
	pvlan = ftm_vlan_lookup(vlan);
	pvlan->ifindex = pvlanif->ifm.ifindex;

	return ERRNO_SUCCESS;
}


/* 删除 vlanif 接口，需要更新 vlan ifindex */
void ftm_vlanif_delete(uint32_t ifindex)
{
	struct ftm_vlan   *pvlan = NULL;
	uint16_t vlan;

	vlan = IFM_VLANIF_ID_GET(ifindex);
	pvlan = ftm_vlan_lookup(vlan);
	pvlan->ifindex = 0;

	return;
}


/* 处理 vlan 的 IPC 消息 */
int ftm_vlan_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	zlog_debug(0,"%s[%d]: entering function '%s'.\n", __FILE__, __LINE__, __func__);
    struct vlan_range *pvlan;
    int ret = ERRNO_SUCCESS;

    switch ( opcode )
    {
		case IPC_OPCODE_ADD:			
			pvlan=( struct vlan_range *) pdata;
			if ( subtype == VLAN_INFO_PORT )
			{
			    ret = ftm_vlan_add_port (pvlan, msg_index);
			}
			else if( subtype == VLAN_INFO_ENABLE )			
			{
			    ret = ftm_vlan_enable (pvlan);
			}
            break;
		case IPC_OPCODE_DELETE:			
			pvlan=( struct vlan_range *) pdata;
			if ( subtype == VLAN_INFO_PORT )
			{
			    ret = ftm_vlan_delete_port (pvlan, msg_index);
			}
			else if( subtype == VLAN_INFO_ENABLE )			
			{
			    ret = ftm_vlan_disable (pvlan);
			}
			break;
     	case IPC_OPCODE_UPDATE:
			break;
		default:
				zlog_err("%s[%d]: function '%s'. IPC_OPCODE error \n", __FILE__, __LINE__, __func__);
    		ret = ERRNO_FAIL;
    		break;
    }

    return ret;
}




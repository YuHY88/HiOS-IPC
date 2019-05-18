#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc_n.h>
#include <lib/alarm.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>
#include "stm_if.h"
#include <lib/index.h>
#include <lib/devm_com.h>
#include <lib/ifm_common.h>

#include "ces_alarm.h"
#include "ces_if.h"

struct list *stm_msp_list = NULL;
struct list *stm_dxc_list = NULL;

/*dxc list初始化*/
int stm_dxc_init(void)
{
    int ret  = 0;
    struct stm_dxc_config *dxc = NULL;
    struct listnode *pnode = NULL;

    index_register ( INDEX_TYPE_DXC, DXC_RESERVED_BASE +1 ); 

    /*初始化msp链表:存储stm msp实例*/
    if(stm_dxc_list == NULL)
    {
        stm_dxc_list = list_new();
        if(stm_dxc_list == NULL)
        {
            zlog_err ( "%s:%s[%d] malloc stm_dxc_list is NULL  erro\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    else
    {
        //else 在板卡插拔情况时重新下发保护，否则保护不生效
        for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, dxc))
        {
            if(dxc != NULL)
            {
                if(dxc->down_flag == LINK_DOWN)
                {
                    ret = ces_msg_send_hal_wait_ack(dxc, sizeof(struct stm_dxc_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_DXC_SET, IPC_OPCODE_UPDATE, dxc->base_if.ifindex);  
                    if(ret != 0)
                    {
                         zlog_err ( "%s:%s[%d] Hotplug card configuration restore error\n", __FILE__, __FUNCTION__, __LINE__);
                        return -1;
                    }
                    dxc->down_flag = LINK_UP;
                }
                else
                {
                    return 0;
                }
            }
        }
    }

    return 0;
}
struct stm_dxc_config *stm_dxc_lookup(char *name)
{
    struct stm_dxc_config *dxc = NULL;
    struct listnode *pnode = NULL;

    //for ( pnode = listhead (stm_dxc_list); pnode; pnode = listnextnode ( pnode ) )
    for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, dxc))
    {
        //dxc = ( struct stm_dxc_config * ) listgetdata ( pnode );
        //if ( dxc != NULL )
        //{
            if(0 == strcmp(name, dxc->name))
            {
                return dxc;
            }
       //}
    }

    return NULL;
}
struct stm_dxc_config *stm_dxc_index_lookup(uint32_t ifindex)
{
    struct stm_dxc_config *dxc = NULL;
    struct listnode *pnode = NULL;

    for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, dxc))
    {
        if((dxc->base_if.ifindex == ifindex)||(dxc->backup_if.ifindex == ifindex) ||(dxc->dst_if.ifindex == ifindex))
        {
            return dxc;
        }
        
    }

    return NULL;
}

int stm_dxc_add(struct stm_dxc_config *dxc_info)
{
    int ret = 0;
    struct stm_dxc_config *dxc = NULL;

    dxc = XMALLOC(MTYPE_IFM_ENTRY, sizeof(struct stm_dxc_config));
    if(dxc == NULL)
    {
        zlog_err("%s:%s[%d] malloc stm_dxc_config erro\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    memcpy(dxc, dxc_info, sizeof(struct stm_dxc_config));
    
    //send msg to hal, return 0 then add to stm_msp_list
    ret = ces_msg_send_hal_wait_ack(dxc, sizeof(struct stm_dxc_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_DXC_SET, IPC_OPCODE_UPDATE, dxc_info->base_if.ifindex); 
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        XFREE(MTYPE_IFM_ENTRY, dxc);
        return -1;
    }
    
    if(stm_dxc_list)
    {
        dxc->down_flag = LINK_UP;
        listnode_add(stm_dxc_list, dxc);
        
    }
    else
    {
        zlog_err("%s:%s[%d] malloc stm_dxc_list is NULL error\n", __FILE__, __FUNCTION__, __LINE__);
        XFREE(MTYPE_IFM_ENTRY, dxc);
        return -1;
    }

    return ret;
}

int stm_dxc_delete(char *name)
{
    int ret = 0;
    struct stm_dxc_config *dxc = NULL;
    
    dxc = stm_dxc_lookup(name);
    if(dxc == NULL)
    {       
        zlog_err("%s:%s[%d] err, dxc group not exist!\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    if(dxc->down_flag == LINK_UP)
    {
        dxc->action = STM_DXC_DISABLE;
        //send msg to hal, return 0 then add to stm_msp_list
        ret = ces_msg_send_hal_wait_ack(dxc, sizeof(struct stm_dxc_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_DXC_SET, IPC_OPCODE_UPDATE, dxc->base_if.ifindex);
        if (ret != 0)
        {
            zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
            return -1;
        }
        if(1 == dxc->index_flag)
        {
            index_free(INDEX_TYPE_DXC, dxc->index);
        }
    }
    
    listnode_delete(stm_dxc_list, dxc);
    XFREE(MTYPE_IFM_ENTRY, dxc);

    return ret;
}

void stm_dxc_status_update(uint32_t ifindex, uint8_t state)
{
    struct stm_dxc_config *dxc = NULL;
    
    dxc = stm_dxc_index_lookup(ifindex);
    if(dxc == NULL)
    {
        zlog_err ( "%s[%d]:leave %s:error:dxc is NULL\n", __FILE__, __LINE__, __func__);
        return;
    }
    dxc->status = state;
}
#if 0
uint8_t *stm_dxc_get_status(struct stm_dxc_config *dxc)
{
    return ipc_send_hal_wait_reply((void *)dxc, sizeof(struct stm_dxc_config), 1 , MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_DXC_STATUS_GET, IPC_OPCODE_GET, dxc->base_if.ifindex);
}
#endif

/*msp list初始化*/
int stm_msp_init(void)
{
    int ret  = 0;
    struct stm_msp_config *pmsp = NULL;
    struct listnode *pnode = NULL;
    
    /*初始化msp链表:存储stm msp实例*/
    if(stm_msp_list == NULL)
    {
        stm_msp_list = list_new();
        if(stm_msp_list == NULL)
        {
            zlog_err ( "%s:%s[%d] malloc stm_msp_list is NULL  erro\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    else
    {
        //else 在板卡插拔情况时重新下发保护，否则保护不生效
        for ( pnode = listhead (stm_msp_list); pnode; pnode = listnextnode ( pnode ) )
        {
            pmsp = ( struct stm_msp_config * ) listgetdata ( pnode );
            if ( pmsp != NULL )
            {
                if(pmsp->down_flag == LINK_DOWN)
                {
                    ret = ces_msg_send_hal_wait_ack(pmsp, sizeof(struct stm_msp_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_MSP, IPC_OPCODE_UPDATE, pmsp->master_if);
                    if (ret != 0)
                    {
                        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
                        return -1;
                    }

                    pmsp->down_flag = LINK_UP;
                }
                else
                {
                    return 0;
                }
            }
        }
    }

    return 0;
}

struct stm_msp_config *stm_msp_lookup(uint32_t ifindex)
{
    struct stm_msp_config *pmsp = NULL;
    struct listnode *pnode = NULL;

    for ( pnode = listhead (stm_msp_list); pnode; pnode = listnextnode ( pnode ) )
    {
        pmsp = ( struct stm_msp_config * ) listgetdata ( pnode );
        if ( pmsp != NULL )
        {
            if((pmsp->master_if == ifindex) || (pmsp->backup_if == ifindex))
            {
                return pmsp;
            }
        }
    }

    return NULL;
}

struct stm_msp_config *stm_msp_lookup_by_mspid(uint16_t msp_id)
{
    struct stm_msp_config *pmsp = NULL;
    struct listnode *pnode = NULL;

    for ( pnode = listhead (stm_msp_list); pnode; pnode = listnextnode ( pnode ) )
    {
        pmsp = ( struct stm_msp_config * ) listgetdata ( pnode );
        if ( pmsp != NULL )
        {
            if(pmsp->msp_id == msp_id)
            {
                return pmsp;
            }
        }
    }

    return NULL;
}

int stm_msp_add(struct stm_msp_config *msp_info, uint16_t msp_id)
{
    int ret = 0;
    static struct stm_msp_config *pmsp = NULL;

    pmsp = XMALLOC(MTYPE_IFM_ENTRY, sizeof(struct stm_msp_config));
    if(pmsp == NULL)
    {
        zlog_err("%s:%s[%d] malloc stm_msp_config erro\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    memcpy(pmsp, msp_info, sizeof(struct stm_msp_config));
    
    //send msg to hal, return 0 then add to stm_msp_list
    ret = ces_msg_send_hal_wait_ack(pmsp, sizeof(struct stm_msp_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_MSP, IPC_OPCODE_UPDATE, msp_info->master_if);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        XFREE(MTYPE_IFM_ENTRY, pmsp);
        return -1;
    }
    
    if(stm_msp_list)
    {
        pmsp->down_flag = LINK_UP;
        listnode_add(stm_msp_list, pmsp);
    }
    else
    {
        zlog_err("%s:%s[%d] malloc stm_msp_list is NULL error\n", __FILE__, __FUNCTION__, __LINE__);
        XFREE(MTYPE_IFM_ENTRY, pmsp);
        return -1;
    }

    return ret;
}

int stm_msp_delete(uint16_t msp_id)
{
    int ret = 0;
    struct stm_msp_config *pmsp = NULL;
    
    pmsp = stm_msp_lookup_by_mspid(msp_id);
    if(pmsp == NULL)
    {
        zlog_err("%s:%s[%d] err, msp group %d not exist!\n", __FILE__, __FUNCTION__, __LINE__, msp_id);
        return -1;
    }

    if(pmsp->down_flag == LINK_UP)
    {
        pmsp->action = STM_MSP_DISABLE;

        //send msg to hal, return 0 then add to stm_msp_list
        ret = ces_msg_send_hal_wait_ack(pmsp, sizeof(struct stm_msp_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_MSP, IPC_OPCODE_UPDATE, pmsp->master_if);
        if (ret != 0)
        {
            zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
            return -1;
        }
    }   

    listnode_delete(stm_msp_list, pmsp);
    XFREE(MTYPE_IFM_ENTRY, pmsp);
    
    return ret;
}

int stm_msp_wtr_set(struct stm_msp_config *msp_info, uint16_t wtr)
{
    int ret = 0;
    struct stm_msp_config pmsp;
    
    if(msp_info == NULL)
    {
        zlog_err("%s:%s[%d] err, msp_info is NULL!\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    zlog_debug(CES_DEBUG_STM, "%s:%s[%d]:msp_index=%d, master_if=%x, backup_if=%x, wtr=%d\n", __FILE__, __FUNCTION__, __LINE__, 
                    msp_info->msp_id, msp_info->master_if, msp_info->backup_if, msp_info->wtr);

    memset(&pmsp, 0, sizeof(struct stm_msp_config));
    pmsp.master_if = msp_info->master_if;
    pmsp.backup_if = msp_info->backup_if;
    pmsp.wtr = wtr;
    
    ret = ces_msg_send_hal_wait_ack(&pmsp, sizeof(struct stm_msp_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_WTR, IPC_OPCODE_UPDATE, msp_info->master_if);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d]:ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        return -1;
    }

    //save config
    msp_info->wtr = wtr;
    
    return ret;
}

struct stm_hp_t *stm_hp_lookup(struct stm_hp_t *hp_list, uint16_t vc4_id)
{
    struct stm_hp_t *cur = hp_list;

    while(cur != NULL)
    {
        if(cur->vc4_id == vc4_id)
        {
            break;
        }

        cur = cur->next;
    }

    return cur;
}

struct stm_hp_t *stm_hp_add(struct stm_hp_t **hp_list, uint16_t vc4_id)
{
    struct stm_hp_t *cur = NULL;
    struct stm_hp_t *new_hp_item = NULL;
    struct stm_hp_t *hp_item = NULL;
    
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    hp_item = stm_hp_lookup(*hp_list, vc4_id);
    if(hp_item != NULL)
    {
        zlog_err("%s:%s[%d] vc4 id:%d already exist!\n", __FILE__, __FUNCTION__, __LINE__, vc4_id);
        return *hp_list;
    }
    
    /* Build new node */
    new_hp_item = XMALLOC(MTYPE_IFM_ENTRY, sizeof(struct stm_hp_t));
    if(new_hp_item == NULL)
    {
        zlog_err("%s[%d]:leave %s, fail to malloc new_hp_item, vc4_id= %d\n", __FILE__, __LINE__, __func__, vc4_id);
        return NULL;
    }
    
    memset(new_hp_item, 0, sizeof(struct stm_hp_t));
    
    new_hp_item->vc4_id = vc4_id;
    new_hp_item->pad_zero = 0;
    memset(&(new_hp_item->hp_info), 0, sizeof(struct hp_info_t)); 
    new_hp_item->hp_info.exc_thr = STM_EXC_THRESHOLD;
    new_hp_item->hp_info.deg_thr = STM_DEG_THRESHOLD;
    new_hp_item->hp_info.c2_tx = STM_OVERHEAD_C2;
    memset(new_hp_item->hp_info.j1_tx, 'b', STM_OVERHEAD_LEN); 
    new_hp_item->next = NULL;

    /*the first node to be added */
    if(*hp_list == NULL)
    {
        *hp_list = new_hp_item;
    }
    else
    {
        cur = *hp_list;
        while(cur->next != NULL)
        {
            cur = cur->next;
        }

        cur->next = new_hp_item;
    }
    
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

    return *hp_list;
    //return NULL;
}

/* static void stm_hp_del(struct stm_hp_t **hp_list, uint16_t vc4_id)
{
    struct stm_hp_t *cur = NULL;
    struct stm_hp_t *next = NULL;
    struct stm_hp_t *last = NULL;
    cur = *hp_list;

    if(cur == NULL)
    {
        return; 
    }
    
    do
    {
        next = cur->next;       
        
        if(cur->vc4_id == vc4_id)
        {
            if(cur == *hp_list)
            {
                *hp_list = next;                
            }
            else
            {
                if(last) 
                {
                    last->next = next;
                }
            }
            
            free(cur);
        }
        else
        {           
            last = cur ;        
        }
        
        cur  = next;

    }while(cur != NULL);
    
    return ;
}*/

static void stm_hp_del_all(struct stm_hp_t **hp_list)
{
    struct stm_hp_t *cur = NULL;
    struct stm_hp_t *next = NULL;
    
    cur = *hp_list;

    *hp_list = NULL;

    while(cur != NULL)
    {   
        next = cur->next;
        
        free(cur);

        cur = next ;
    }
    
    return ;
}

/* 计算哈希key值 */
static unsigned int stm_if_hash(void *hash_key)
{
    if (hash_key == NULL)
    {
        return 0;
    }

    return (uint32_t) hash_key ;
}


/* 比较哈希key值 */
static int stm_if_compare(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = item;

    if (item == NULL || hash_key == NULL)
    {
        return 1;
    }

    if (pbucket->hash_key == hash_key)
    {
        return 0;
    }

    return 1;
}


void stm_if_init(void)
{
    int i = 0;
    int if_num = 0;
    int buffer_len = IPC_MSG_LEN / sizeof ( struct ifm_info );
    uint8_t type = 0;
    uint8_t slot = 0;
    uint8_t same_slot[SLOT_NUM_MAX + 1] = {0};
    uint32_t ifindex = 0;
    struct ifm_info *if_buffer = NULL;
    struct ifm_info p_ifinfo[buffer_len];
    struct stm_if   *pif = NULL;
    struct devm_slot slot_info[SLOT_NUM_MAX + 1];

    //获取接口信息，
    while(1)
    {
        if(if_buffer == NULL)
        {
            if_buffer = ifm_get_bulk(0, MODULE_ID_CES, &if_num);
        }
        else
        {
            if_buffer = ifm_get_bulk(ifindex, MODULE_ID_CES, &if_num);
        }
        
        if (if_buffer == NULL || if_num == 0)     //所有接口已经获取完毕
        {
            zlog_debug(CES_DEBUG_STM, "%s:%s[%d] ifm_get_bulk done!\n", __FILE__, __FUNCTION__, __LINE__);
            break;
        }
        
        //memset(slot_info, 0, sizeof(struct devm_slot));
        memset(p_ifinfo, 0, buffer_len*sizeof(struct ifm_info));
        memcpy(p_ifinfo, if_buffer, if_num*sizeof(struct ifm_info));
        
        for (i = 0; i < if_num; i++)
        {
            if (!IFM_TYPE_IS_STM(p_ifinfo[i].ifindex))
            {
                ifindex = p_ifinfo[i].ifindex;
                continue;
            }

            //get slot type from devm, once for every single slot
            slot = p_ifinfo[i].slot;
            if(same_slot[slot] == 0)
            {
                devm_comm_get_slot(1 , slot, MODULE_ID_CES, &slot_info[slot]);
                same_slot[slot] = 1;
            }

            //sc01q for now
            if(slot_info[slot].id != ID_SC01Q && slot_info[slot].id != ID_SC01QE)
            {
                ifindex = p_ifinfo[i].ifindex;
                continue;
            }

            //创建的stm接口类型
            if (p_ifinfo[i].sub_type == IFNET_SUBTYPE_STM1)
            {
                type = IF_TYPE_STM1;
            }
            else if (p_ifinfo[i].sub_type == IFNET_SUBTYPE_STM4)
            {
                type = IF_TYPE_STM4;
            }
            else if (p_ifinfo[i].sub_type == IFNET_SUBTYPE_STM16)
            {
                type = IF_TYPE_STM16;
            }
            else if (p_ifinfo[i].sub_type == IFNET_SUBTYPE_STM64)
            {
                type = IF_TYPE_STM64;
            }
            else
            {
                ifindex = p_ifinfo[i].ifindex;
                continue;
            }

            pif = stm_if_create(p_ifinfo[i].ifindex, type);
            if (pif == NULL)
            {
                zlog_err("\r\n%s:%s[%d] fail to create stm if: 0x%x!\n", 
                            __FILE__, __FUNCTION__, __LINE__, p_ifinfo[i].ifindex);
                ifindex = p_ifinfo[i].ifindex;
                continue;
            }
            
            ifindex = p_ifinfo[i].ifindex;

            g_stm_slot[slot].slot_id = slot_info[slot].id;
            //g_stm_slot[slot].image_type = 0xff;
        }
        
        mem_share_free_bydata(if_buffer, MODULE_ID_CES);
    }

    //接口创建完成后，初始化stm保护组链表
    stm_msp_init();
    stm_dxc_init();

    return;
}

/* 初始化stm_if_table */
void stm_if_table_init(int size)
{
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    hios_hash_init(&stm_if_table, size, stm_if_hash, stm_if_compare);

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s' init stm_if_table successful.\n", __FILE__, __LINE__, __func__);
}

//初始化stm结构体
int stm_if_info_init(uint32_t ifindex, struct stm_if *pif, enum STM_TYPE type)
{    
    uint8_t i = 1;
    struct stm_rs_t     *stm_rs = NULL;
    struct stm_ms_t     *stm_ms = NULL;
    struct stm_hp_t     *stm_hp = NULL;
    struct gpnPortInfo gPortInfo;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    
    if(pif == NULL)
    {
        zlog_err("%s:%s[%d]: pif is null, error\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    if ( type < IF_TYPE_STM1 || type > IF_TYPE_STM64)
    {
        zlog_err("%s:%s[%d]: stm interface type: %d error!\n", __FILE__, __FUNCTION__, __LINE__, type);
        return -1;
    }

    memset(pif, 0, sizeof(struct stm_if));
    pif->ifindex = ifindex;
    pif->type = type;
    //pif->down_flag = IFNET_LINKDOWN;
    pif->loopback = STM_LB_NOLOOPBACK;

    //设置默认j0，b1统计门限
    stm_rs = &(pif->stm_rs);
    stm_rs->exc_thr = STM_EXC_THRESHOLD;
    stm_rs->deg_thr = STM_DEG_THRESHOLD;
    memset(stm_rs->j0_tx, 'a', STM_OVERHEAD_LEN);

    //设置默认b2统计门限
    stm_ms = &(pif->stm_ms);
    stm_ms->exc_thr = STM_EXC_THRESHOLD;
    stm_ms->deg_thr = STM_DEG_THRESHOLD;

    //STM-N类型，创建N个vc4数据结构存储相应数据
    for ( i = 1; i <= type; i++)
    {
        stm_hp = stm_hp_add(&(pif->stm_hp_list), i);
        if(stm_hp == NULL)
        {
            zlog_err("%s:%s[%d]: stm_hp_add to list failed, vc4_id init error\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        
        //register alarm type: vc4
        memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
        gPortInfo.iAlarmPort = IFM_VC4_TYPE;
        gPortInfo.iIfindex = ifindex;
        gPortInfo.iMsgPara1 = i;        //vc4_id
        ipran_alarm_port_register(&gPortInfo);
    }
    
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return 0;
}

/* 添加接口stm*/
struct stm_if *stm_if_add(uint32_t ifindex, enum STM_TYPE type)
{
    int ret = 0;
    struct hash_bucket *p_hash_bucket = NULL;
    struct stm_if *pif_new = NULL;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    if ( type < IF_TYPE_STM1 || type > IF_TYPE_STM64)
    {
        zlog_err("%s:%s[%d]: stm interface type: %d error!\n", __FILE__, __FUNCTION__, __LINE__, type);
        return NULL;
    }

    p_hash_bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (p_hash_bucket == NULL)
    {
        zlog_err("%s:%s[%d] fail to malloc for the hash_node of the new stm_if!\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    pif_new = (struct stm_if *)XMALLOC(MTYPE_IFM_ENTRY, sizeof(struct stm_if));
    if (pif_new == NULL)
    {
        zlog_err("%s:%s[%d] fail to malloc: memory is insufficient!\n", __FILE__, __FUNCTION__, __LINE__);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }

    //初始化stm信息, j0/j1/c2/loopback。j2 的初始化在tdm视图下完成
    memset(pif_new, 0, sizeof(struct stm_if));
    ret = stm_if_info_init(ifindex, pif_new, type);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] stm_if_info_init error:%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        XFREE(MTYPE_IFM_ENTRY, pif_new);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }

    ret = ces_msg_send_hal_wait_ack(&type, sizeof(type), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_INVALID, IPC_OPCODE_ADD, ifindex);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        XFREE(MTYPE_IFM_ENTRY, pif_new);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }

    memset(p_hash_bucket, 0, sizeof(struct hash_bucket));
    p_hash_bucket->hash_key = (void *)ifindex;
    p_hash_bucket->data = pif_new;

    ret = hios_hash_add(&stm_if_table, p_hash_bucket);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] fail to add the hash_node of new stm_if to the stm_if_table\n", __FILE__, __FUNCTION__, __LINE__);
        XFREE(MTYPE_IFM_ENTRY, pif_new);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }
    
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return pif_new;
}


/* 删除接口信息 */
int stm_if_delete(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;
    struct stm_if *pif = NULL;
    struct stm_msp_config *stm_msp = NULL;
    struct stm_dxc_config *stm_dxc = NULL;
    struct listnode        *pnode = NULL;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pitem = hios_hash_find(&stm_if_table, (void *)ifindex);
    if (pitem == NULL)
    {
        zlog_err("%s:%s[%d] find stm_if info failed\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }

    pif = (struct stm_if *)pitem->data;
    if (pif == NULL)
    {
        zlog_err("%s:%s[%d] stm_if NULL!\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    /*删除接口，清除已经上报的告警信息*/
    stm_if_alarm_clear(pif);

    //删除hp缓存信息
    stm_hp_del_all(&(pif->stm_hp_list));
    pif->stm_hp_list = NULL;

    //msp为全局配置，拔掉板卡时msp缓存信息不删除，不用下发hal
    stm_msp = stm_msp_lookup(ifindex);
    if(stm_msp)
    {
        //listnode_delete(stm_msp_list, stm_msp);
        stm_msp->down_flag = LINK_DOWN;
    }
    for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, stm_dxc))
    {
        if(stm_dxc!= NULL)
        {
            stm_dxc->down_flag = LINK_DOWN; 
        }
    }
    hios_hash_delete(&stm_if_table, pitem);
    XFREE(MTYPE_IFM_ENTRY, pif);
    XFREE(MTYPE_HASH_BACKET, pitem);
    
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s' delete from stm_if_table successful.\n", __FILE__, __LINE__, __func__);
    return 0;
}


/* 查找接口信息*/
struct stm_if *stm_if_lookup(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pitem = hios_hash_find(&stm_if_table, (void *)ifindex);

    if (pitem == NULL)
    {
        zlog_err("%s:%s[%d] hios_hash_find failed \n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s' stm_if lookup successful.\n", __FILE__, __LINE__, __func__);
    return (struct stm_if *)pitem->data;
}


/* 接口down事件 */
int stm_if_down(uint32_t ifindex)
{
    struct stm_if *pif;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = stm_if_lookup(ifindex);

    if (pif == NULL)
    {
        return 1;
    }

    pif->down_flag = IFNET_LINKDOWN;
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return 0;
}


/* 接口up事件 */
int stm_if_up(uint32_t ifindex)
{
    struct stm_if *pif;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = stm_if_lookup(ifindex);

    if (pif == NULL)
    {
        return 1;
    }

    pif->down_flag = IFNET_LINKUP;
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return 0;
}

//stm哈希表是否已经创建，若没有创建则创建
struct stm_if *stm_if_create(uint32_t ifindex, enum STM_TYPE type)
{
    struct stm_if *pif = NULL;

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    //根据索引查找是否已经存在，若不存在进行创建
    pif = stm_if_lookup(ifindex);

    if (pif != NULL)
    {
        return pif;
    }

    if ( type < IF_TYPE_STM1 || type > IF_TYPE_STM64)
    {
        zlog_err("%s:%s[%d]: stm interface type: %d error!\n", __FILE__, __FUNCTION__, __LINE__, type);
        return NULL;
    }

    pif = stm_if_add(ifindex, type);
    if (pif == NULL)
    {
        zlog_err("%s:%s[%d]: failed to create stm interface, ifindex=0x%0x\n", 
                                            __FILE__, __FUNCTION__, __LINE__, ifindex);
        return NULL;
    }

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return pif;
}

/*update stm statistics only for SC01Q, vc4_id == 1*/
void stm_if_statis_update(struct stm_statis_info *statis_info, uint32_t ifindex)
{
    struct stm_if   *pif = NULL;

    if(statis_info == NULL)
    {
        zlog_err("%s:%s[%d] statis_info NULL ifindex=0x%0x\n", __FILE__, __FUNCTION__, __LINE__, ifindex);
        return;
    }
    
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        zlog_err("%s:%s[%d] stm_lookup ifindex=0x%0x\n", __FILE__, __FUNCTION__, __LINE__, ifindex);
        return;
    }

    pif->stm_rs.b1 = statis_info->count_info.B1;
    memcpy(pif->stm_rs.j0_rx, statis_info->stm_status.j0 + 1, STM_OVERHEAD_LEN);

    pif->stm_ms.b2 = statis_info->count_info.B2;
    pif->stm_ms.k1_rx = statis_info->stm_status.k1;
    pif->stm_ms.k2_rx = statis_info->stm_status.k2;    
    pif->stm_ms.m1 = statis_info->stm_status.m1;

    if(pif->stm_hp_list != NULL)
    {
        pif->stm_hp_list->hp_info.b3 = statis_info->count_info.B3;
        pif->stm_hp_list->hp_info.c2_rx = statis_info->stm_status.c2;
        pif->stm_hp_list->hp_info.g1_rx = statis_info->stm_status.g1;
        memcpy(pif->stm_hp_list->hp_info.j1_rx, statis_info->stm_status.j1 + 1, STM_OVERHEAD_LEN);
    }

    return;
}

void stm_if_alarm_clear(struct stm_if *pif)
{    
    unsigned int vc4_id = 0;
    struct stm_hp_t *stm_hp = NULL;
    struct gpnPortInfo gPortInfo;
    
    //clear stmn alarm
    stm_alarm_report(STM_ALARM_OPT_LOS, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_OPT_LOF, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_TU_LOM, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_AU_LOP, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_AU_AIS, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_RS_TIM, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_RS_EXC, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_RS_DEG, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_MS_AIS, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_MS_RDI, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_MS_EXC, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);
    stm_alarm_report(STM_ALARM_MS_DEG, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0, 0);

    //clear vc4 alarm and unregister
    for ( vc4_id = 1; vc4_id <= pif->type; vc4_id++)
    {
        stm_hp = stm_hp_lookup(pif->stm_hp_list, vc4_id);
        if(stm_hp == NULL)
        {
            zlog_err("%s:%s[%d]: stm_hp_add to list failed, vc4_id init error\n", __FILE__, __FUNCTION__, __LINE__);
            return;
        }
        
        stm_alarm_report(STM_ALARM_HP_TIM, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        stm_alarm_report(STM_ALARM_HP_SLM, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        stm_alarm_report(STM_ALARM_HP_UNQ, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        stm_alarm_report(STM_ALARM_HP_REI, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        stm_alarm_report(STM_ALARM_HP_RDI, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        stm_alarm_report(STM_ALARM_HP_EXC, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        stm_alarm_report(STM_ALARM_HP_DEG, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, vc4_id, 0);
        
        //unregister alarm type: vc4
        memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
        gPortInfo.iAlarmPort = IFM_VC4_TYPE;
        gPortInfo.iIfindex = pif->ifindex;
        gPortInfo.iMsgPara1 = vc4_id;
        ipran_alarm_port_unregister(&gPortInfo);
    }

    return;
}

void stm_if_alarm_set(struct stm_alarm_info *alarm_info, uint32_t ifindex)
{
    struct stm_if   *pif = NULL;
    //struct stm_hp_t *hp_info = NULL;
    struct stm_rs_alarm_t   *rs_alarm = NULL;
    struct stm_ms_alarm_t   *ms_alarm = NULL;
    struct stm_hp_alarm_t   *hp_alarm = NULL;
    uint8_t status = 0;
    int type = 0;
    uint16_t vc4_id = 1;    //sc01q have only one vc4

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if(alarm_info == NULL)
    {
        zlog_err("%s:%s[%d] alarm_info NULL ifindex=0x%0x\n", __FILE__, __FUNCTION__, __LINE__, ifindex);
        return;
    }
    
    status = alarm_info->status;
    type = alarm_info->type;
        
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        zlog_err("%s:%s[%d] stm_lookup ifindex=0x%0x type=%d\n", __FILE__, __FUNCTION__, __LINE__, ifindex, type);
        return;
    }

    rs_alarm = &(pif->stm_rs.rs_alarm);
    ms_alarm = &(pif->stm_ms.ms_alarm);
    hp_alarm = &(pif->stm_hp_list->hp_info.hp_alarm);
/*
    //stm-1类型的板卡，vc4_id默认为1, 使用struct alarm_info_t *get_info
    if((get_info->stm_type == IF_TYPE_STM1) && (get_info->vc4_id != 1))
    {
        zlog_err ( "%s:%s[%d] interface type is STM-1, vc4_id error\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    hp_info = stm_hp_lookup(pif->stm_hp_list, get_info->vc4_id);
    if(hp_info == NULL)
    {
        zlog_err ( "%s:%s[%d] find vc4_id: %d error\n", __FILE__, __FUNCTION__, __LINE__, get_info->vc4_id );
        return;
    }

    hp_alarm = &(hp_info->hp_info.hp_alarm);
*/

    switch (type)
    {
        case STM_ALARM_OPT_LOS:
            if (rs_alarm->los != status)
            {
                rs_alarm->los = status;
                pif->stm_rs.rs_alarm.los = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_OPT_LOS, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_OPT_LOS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_OPT_LOF:
            if (rs_alarm->lof != status)
            {
                rs_alarm->lof = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_OPT_LOF, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_OPT_LOF, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_REMOTE_OFF:
            break;
        case STM_ALARM_TU_LOM:
            if (hp_alarm->tu_lom != status)
            {
                hp_alarm->tu_lom = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_TU_LOM, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_TU_LOM, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_AU_LOP:
            if (hp_alarm->au_lop != status)
            {
                hp_alarm->au_lop = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_AU_LOP, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_AU_LOP, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_AU_AIS:
            if (hp_alarm->au_ais != status)
            {
                hp_alarm->au_ais = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_AU_AIS, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_AU_AIS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_RS_TIM:
            if (rs_alarm->rs_tim != status)
            {
                rs_alarm->rs_tim = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_RS_TIM, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_RS_TIM, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_RS_EXC:
            if (rs_alarm->rs_exc != status)
            {
                rs_alarm->rs_exc = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_RS_EXC, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_RS_EXC, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_RS_DEG:
            if (rs_alarm->rs_deg != status)
            {
                rs_alarm->rs_deg = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_RS_DEG, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_RS_DEG, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_MS_AIS:
            if (ms_alarm->ms_ais != status)
            {
                ms_alarm->ms_ais = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_MS_AIS, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_MS_AIS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_MS_RDI:
            if (ms_alarm->ms_rdi != status)
            {
                ms_alarm->ms_rdi = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_MS_RDI, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_MS_RDI, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_MS_REI:
            if (ms_alarm->ms_rei != status)
            {
                ms_alarm->ms_rei = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_MS_REI, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_MS_REI, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_MS_EXC:
            if (ms_alarm->ms_exc != status)
            {
                ms_alarm->ms_exc = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_MS_EXC, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_MS_EXC, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_MS_DEG:
            if (ms_alarm->ms_deg != status)
            {
                ms_alarm->ms_deg = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_MS_DEG, GPN_SOCK_MSG_OPT_RISE, ifindex, 0, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_MS_DEG, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0, 0);
                }
            }
            break;
        case STM_ALARM_HP_TIM:
            if (hp_alarm->hp_tim != status)
            {
                hp_alarm->hp_tim = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_TIM, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_TIM, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_HP_SLM:
            if (hp_alarm->hp_slm != status)
            {
                hp_alarm->hp_slm = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_SLM, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_SLM, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_HP_UNQ:
            if (hp_alarm->hp_unq != status)
            {
                hp_alarm->hp_unq = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_UNQ, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_UNQ, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_HP_REI:
            if (hp_alarm->hp_rei != status)
            {
                hp_alarm->hp_rei = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_REI, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_REI, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_HP_RDI:
            if (hp_alarm->hp_rdi != status)
            {
                hp_alarm->hp_rdi = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_RDI, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_RDI, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_HP_EXC:
            if (hp_alarm->hp_exc != status)
            {
                hp_alarm->hp_exc = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_EXC, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_EXC, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        case STM_ALARM_HP_DEG:
            if (hp_alarm->hp_deg != status)
            {
                hp_alarm->hp_deg = status;

                if (status)
                {
                    stm_alarm_report(STM_ALARM_HP_DEG, GPN_SOCK_MSG_OPT_RISE, ifindex, vc4_id, 0);
                }
                else
                {
                    stm_alarm_report(STM_ALARM_HP_DEG, GPN_SOCK_MSG_OPT_CLEAN, ifindex, vc4_id, 0);
                }
            }
            break;
        default:
            break;
    }

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'.(ifindex:0x%x, type=%u, status=%u)\n", 
                                            __FILE__, __LINE__, __func__, ifindex, type, status);

    return;
}


/*设置j0*/
int stm_j0_set(struct stm_if *pif, char *j0)
{
    int ret = 0;

    if (pif == NULL || j0 == NULL)
    {
        zlog_err("%s:%s[%d]: pif is null, error\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    /*checkout if had config the same vaule, 第0位是crc，不用比较*/
    if(!strncmp(pif->stm_rs.j0_tx, j0 + 1, STM_OVERHEAD_LEN))
    {
        zlog_err("%s:%s[%d] The same value j0 has been set already!\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    ret = ces_msg_send_hal_wait_ack(j0, STM_OVERHEAD_LEN+1, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_J0, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d]:ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        return -1;
    }

    /*save info, 只保存后15位字符*/
    memcpy(pif->stm_rs.j0_tx, j0 + 1, STM_OVERHEAD_LEN);
    zlog_debug(CES_DEBUG_STM, "\r\n %s:%d, j0=%s, %s \r\n",__FUNCTION__,__LINE__,pif->stm_rs.j0_tx, j0 + 1);

    return 0;
}


/*设置j1, 后续扩展如果在命令行添加vc4_id参数，这里第二个参数可以考虑用stm_hp_set_t 代替*/
int stm_j1_set(struct stm_if *pif, struct stm_hp_t *hp_info)
{
    int ret = 0;
    struct stm_hp_t *stm_hp = NULL;
    struct stm_oh_info_t oh_tmp;

    if (pif == NULL || hp_info == NULL)
    {
        zlog_err("%s:%s[%d] set j1 failed as info is null\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    /*checkout if had config the same vaule*/
    stm_hp = stm_hp_lookup(pif->stm_hp_list, 1);        //这里查找的vc4_id暂时固定为1，后面根据stm类型修改
    if(stm_hp == NULL)
    {
        zlog_err("%s:%s[%d] stm_hp_lookup failed as stm_hp is null\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    /*checkout if had config the same vaule, 第0位是crc，不用比较*/
    if(!strncmp(stm_hp->hp_info.j1_tx, hp_info->hp_info.j1_tx + 1, STM_OVERHEAD_LEN))
    {
        zlog_err("%s:%s[%d] The same overhead j1 has been set already!\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    //使用stm_hp_set_t目的是带上vc4_id，暂时不需要指定
    memset(&oh_tmp, 0, sizeof(struct stm_oh_info_t));
    memcpy(oh_tmp.j1, hp_info->hp_info.j1_tx, STM_OVERHEAD_LEN+1);
    
    ret = ces_msg_send_hal_wait_ack(&oh_tmp, sizeof(struct stm_oh_info_t), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_J1, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        return -1;
    }

    /*save info*/
    memcpy(stm_hp->hp_info.j1_tx, hp_info->hp_info.j1_tx + 1, STM_OVERHEAD_LEN);
    zlog_debug(CES_DEBUG_STM, "\r\n %s:%d, j1=%s, %s \r\n",__FUNCTION__,__LINE__, stm_hp->hp_info.j1_tx, hp_info->hp_info.j1_tx + 1);

    return 0;
}


/*设置c2, 后续扩展如果在命令行添加vc4_id参数，这里第二个参数可以考虑用stm_hp_set_t 代替*/
int stm_c2_set(struct stm_if *pif, struct stm_hp_t *hp_info)
{
    int ret = 0;
    struct stm_hp_t *stm_hp = NULL;
    struct stm_oh_info_t oh_tmp;

    if (pif == NULL || hp_info == NULL)
    {
        zlog_err("%s:%s[%d] set c2 failed as info is null\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    /*checkout if had config the same vaule*/
    stm_hp = stm_hp_lookup(pif->stm_hp_list, 1);
    if(stm_hp == NULL)
    {
        zlog_err("%s:%s[%d] stm_hp_lookup failed as stm_hp is null\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    if(stm_hp->hp_info.c2_tx == hp_info->hp_info.c2_tx)
    {
        zlog_debug(CES_DEBUG_STM, "%s:%s[%d] The same overhead c2 has been set already!\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    //使用stm_hp_set_t目的是带上vc4_id，暂时不需要指定
    memset(&oh_tmp, 0, sizeof(struct stm_oh_info_t));
    oh_tmp.c2 = hp_info->hp_info.c2_tx;
    
    ret = ces_msg_send_hal_wait_ack(&oh_tmp, sizeof(struct stm_oh_info_t), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_C2, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        return -1;
    }

    /*save info*/
    stm_hp->hp_info.c2_tx = hp_info->hp_info.c2_tx;

    return 0;
}

/*设置loopback*/
int stm_loopback_set(struct stm_if *pif, uint8_t loopback)
{
    int ret = 0;

    if (pif == NULL)
    {
        zlog_err("%s:%s[%d]: pif is null, error\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    /*checkout if had config the same vaule*/
    if(pif->loopback == loopback)        
    {
        zlog_err("%s:%s[%d] The same mode of loopback has been set already!\n", __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }

    ret = ces_msg_send_hal_wait_ack(&loopback, sizeof(uint8_t), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_LOOPBACK, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s:%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        return -1;
    }

    /*save info*/
    pif->loopback = loopback;


    ipc_send_msg_n2 ( &loopback, sizeof(loopback), 1 , MODULE_ID_IFM, MODULE_ID_CES,
                                  IPC_TYPE_STM, STM_INFO_LOOPBACK, IPC_OPCODE_UPDATE, pif->ifindex );

    return 0;
}

/* 设置stm信息*/
int stm_set_info(struct stm_if *pif, void *pinfo, enum STM_INFO type)
{
    int ret = 0;
    char *j0 = NULL;
    uint8_t loopback = 0;
    struct stm_hp_t *hp_info = NULL;
    
    zlog_debug(CES_DEBUG_STM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if (pif == NULL || pinfo == NULL)
    {
        zlog_err("%s:%s[%d] Failed to set, set info is NULL\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    switch (type)
    {
        case STM_INFO_J0:
            j0 = (char *)pinfo;            
            ret = stm_j0_set(pif, j0);
            if (ret != 0)
            {
                zlog_err("%s:%s[%d] Failed to set j0\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            break;

        case STM_INFO_J1:
            hp_info = (struct stm_hp_t *)pinfo;            
            ret = stm_j1_set(pif, hp_info);
            if (ret != 0)
            {
                zlog_err("%s:%s[%d] Failed to set j1\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            break;

        case STM_INFO_C2:
            hp_info = (struct stm_hp_t *)pinfo;            
            ret = stm_c2_set(pif, hp_info);
            if (ret != 0)
            {
                zlog_err("%s:%s[%d] Failed to set c2\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            break;

        case STM_INFO_LOOPBACK:
            loopback = * (uint8_t *) pinfo;
            ret = stm_loopback_set(pif, loopback);
            if (ret != 0)
            {
                zlog_err("%s:%s[%d] Failed to set loopback\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            break;
            
        default:
            return -1;
    }

    zlog_debug(CES_DEBUG_STM, "%s[%d]: Leaving function '%s'\n", __FILE__, __LINE__, __func__);

    return ret;
}

struct stm_count_info *stm_if_get_count(uint32_t ifindex)
{
	struct ipc_mesg_n * pmesg = NULL;
	static struct stm_count_info info;
	
	memset(&info, 0, sizeof(struct stm_count_info));

	pmesg = ipc_sync_send_n2( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_CES,
						IPC_TYPE_STM, STM_INFO_COUNT_GET, IPC_OPCODE_GET, ifindex, 3000);
	if(pmesg)
	{
		memcpy(&info, pmesg->msg_data, sizeof(struct stm_count_info));
		mem_share_free(pmesg, MODULE_ID_CES);
		
		return &info;
	}
	
	return NULL;
	
#if 0
    return ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_COUNT_GET, IPC_OPCODE_GET, ifindex);
#endif
}

struct stm_oh_info_t *stm_if_get_status(uint32_t ifindex)
{
	struct ipc_mesg_n * pmesg = NULL;
	static struct stm_oh_info_t info;
	
	memset(&info, 0, sizeof(struct stm_oh_info_t));

	pmesg = ipc_sync_send_n2( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_CES,
						IPC_TYPE_STM, STM_INFO_STATUS_GET, IPC_OPCODE_GET, ifindex, 3000);
	if(pmesg)
	{
		memcpy(&info, pmesg->msg_data, sizeof(struct stm_oh_info_t));
		mem_share_free(pmesg, MODULE_ID_CES);
		
		return &info;
	}
	
	return NULL;
	
#if 0
    return ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_STATUS_GET, IPC_OPCODE_GET, ifindex);
#endif
}

uint8_t *stm_msp_get_status(uint32_t ifindex)
{
	struct ipc_mesg_n * pmesg = NULL;
	static uint8_t info;

	pmesg = ipc_sync_send_n2( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_CES,
						IPC_TYPE_STM, STM_INFO_MSP_STATUS_GET, IPC_OPCODE_GET, ifindex, 3000);
	if(pmesg)
	{
		info = pmesg->msg_data[0];

		mem_share_free(pmesg, MODULE_ID_CES);
		
		return &info;
	}
	
	return NULL;
	
#if 0
    return ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_MSP_STATUS_GET, IPC_OPCODE_GET, ifindex);
#endif
}


/* 查找传入 ifindex 之后对应的 n 个数据 */
int stm_if_snmp_base_bulk_get(uint32_t ifindex, struct stm_base_info stm_base[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct stm_if   *pif = NULL;
    struct stm_base_info    pif_base;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct stm_base_info);
    int cursor;
    int val = 0;

    if (ifindex == 0)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
        {
            pif = (struct stm_if *)pbucket->data;
            if (pif == NULL)
            {
                zlog_err ( "%s[%d] pif is NULL\n", __FUNCTION__, __LINE__ );
                continue;
            }

            memset(&pif_base, 0, sizeof(struct stm_base_info));
            pif_base.ifindex = pif->ifindex;
            pif_base.stm_type = pif->type == IF_TYPE_STM1 ? 1:(pif->type == IF_TYPE_STM4 ? 
                               2:(pif->type == IF_TYPE_STM16 ? 3:(pif->type == IF_TYPE_STM64 ? 4:-1)));
            
            if(pif->stm_rs.rs_alarm.los)
            {
                pif_base.stm_alarm = 0x80;
            }
            
            memcpy(&stm_base[data_num++], &pif_base, sizeof(struct stm_base_info));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else /* 找到 ifindex 后面第一个数据 */
    {
        pbucket = hios_hash_find(&stm_if_table, (void *)ifindex);
        if (pbucket == NULL)    /* 未找到 ifindex 对应的数据 */
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val %= stm_if_table.compute_hash((void *)ifindex);

            if (NULL != stm_if_table.buckets[val])    /* 当前哈希桶链表不为空 */
            {
                pbucket = stm_if_table.buckets[val];
            }
            else                                        /* 当前哈希桶链表为空 */
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != stm_if_table.buckets[val])
                    {
                        pbucket = stm_if_table.buckets[val];
                    }
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&stm_if_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = (struct stm_if *)pnext->data;
                
                memset(&pif_base, 0, sizeof(struct stm_base_info));
                pif_base.ifindex = pif->ifindex;
                pif_base.stm_type = pif->type == IF_TYPE_STM1 ? 1:(pif->type == IF_TYPE_STM4 ? 
                                   2:(pif->type == IF_TYPE_STM16 ? 3:(pif->type == IF_TYPE_STM64 ? 4:-1)));
                
                if(pif->stm_rs.rs_alarm.los)
                {
                    pif_base.stm_alarm = 0x80;
                }
                
                memcpy(&stm_base[data_num], &pif_base, sizeof(struct stm_base_info));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}

/*get 再生段配置，部分数据(j0、b1)需要从底层获取*/
int stm_if_snmp_rs_get(struct stm_if *pif, struct stm_rs_info *pif_rs)
{
    //struct stm_oh_info_t   *oh_status = NULL;
    //struct stm_count_info  *stm_count = NULL;
    
    pif_rs->ifindex = pif->ifindex;
    pif_rs->exc_thr = -(pif->stm_rs.exc_thr);   //过量与劣化门限值设置均为负数，这里直接取正
    pif_rs->deg_thr = -(pif->stm_rs.deg_thr);
    memcpy(pif_rs->j0_tx, pif->stm_rs.j0_tx, STM_OVERHEAD_LEN);
#if 0
    //收到的j0需要从适配层获取
    oh_status = stm_if_get_status(pif->ifindex);
    if(oh_status != NULL)
    {
        memcpy(pif_rs->j0_rx, oh_status->j0 + 1, STM_OVERHEAD_LEN);
    }
    
    
    //b1的统计计数同样需要从适配层获取
    stm_count = stm_if_get_count(pif->ifindex);
    if(stm_count != NULL)
    {
        pif_rs->b1 = stm_count->B1;
    }
#endif

    pif_rs->b1 = pif->stm_rs.b1;
    memcpy(pif_rs->j0_rx, pif->stm_rs.j0_rx, STM_OVERHEAD_LEN);

    if(pif->stm_rs.rs_alarm.lof)
    {
        pif_rs->rs_alarm |= 0x80;
    }

    if(pif->stm_rs.rs_alarm.oof)
    {
        pif_rs->rs_alarm |= 0x40;
    }
    
    if(pif->stm_rs.rs_alarm.rs_exc)
    {
        pif_rs->rs_alarm |= 0x20;
    }
    
    if(pif->stm_rs.rs_alarm.rs_deg)
    {
        pif_rs->rs_alarm |= 0x10;
    }
    
    if(pif->stm_rs.rs_alarm.rs_tim)
    {
        pif_rs->rs_alarm |= 0x08;
    }

    return 0;
}

int stm_if_snmp_rs_bulk_get(uint32_t ifindex, struct stm_rs_info stm_rs[])
{
    struct hash_bucket     *pbucket = NULL;
    struct hash_bucket     *pnext   = NULL;
    struct stm_if          *pif = NULL;
    struct stm_rs_info      pif_rs;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct stm_rs_info);
    int cursor;
    int val = 0;
    
    if (ifindex == 0)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
        {
            pif = (struct stm_if *)pbucket->data;
            if (pif == NULL)
            {
                zlog_err ( "%s[%d] pif is NULL\n", __FUNCTION__, __LINE__ );
                continue;
            }
            
            memset(&pif_rs, 0, sizeof(struct stm_rs_info));
            stm_if_snmp_rs_get(pif, &pif_rs);
            memcpy(&stm_rs[data_num++], &pif_rs, sizeof(struct stm_rs_info));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else /* 找到 ifindex 后面第一个数据 */
    {
        pbucket = hios_hash_find(&stm_if_table, (void *)ifindex);
        if (pbucket == NULL)    /* 未找到 ifindex 对应的数据 */
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val %= stm_if_table.compute_hash((void *)ifindex);

            if (NULL != stm_if_table.buckets[val])    /* 当前哈希桶链表不为空 */
            {
                pbucket = stm_if_table.buckets[val];
            }
            else                                        /* 当前哈希桶链表为空 */
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != stm_if_table.buckets[val])
                    {
                        pbucket = stm_if_table.buckets[val];
                    }
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&stm_if_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = (struct stm_if *)pnext->data;
                
                memset(&pif_rs, 0, sizeof(struct stm_rs_info));
                stm_if_snmp_rs_get(pif, &pif_rs);
                memcpy(&stm_rs[data_num], &pif_rs, sizeof(struct stm_rs_info));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}

/*get 复用段配置，部分数据(k2、b2、rei_count)需要从底层获取*/
int stm_if_snmp_ms_get(struct stm_if *pif, struct stm_ms_info *pif_ms)
{
    //struct stm_oh_info_t   *oh_status = NULL;
    //struct stm_count_info  *stm_count = NULL;
    
    pif_ms->ifindex = pif->ifindex;
    pif_ms->exc_thr = -(pif->stm_ms.exc_thr);   //过量与劣化门限值设置均为负数，这里直接取正
    pif_ms->deg_thr = -(pif->stm_ms.deg_thr);
#if 0
    //收到的k2需要从适配层获取
    oh_status = stm_if_get_status(pif->ifindex);
    if(oh_status != NULL)
    {
        pif_ms->k2_rx = oh_status->k2;
    }
    
    
    //b1、rei_count的统计计数同样需要从适配层获取
    stm_count = stm_if_get_count(pif->ifindex);
    if(stm_count != NULL)
    {
        pif_ms->b2 = stm_count->B2;
        //pif_ms->rei_count = stm_count->MS_REI;
    }
#endif

    pif_ms->b2 = pif->stm_ms.b2;
    pif_ms->k2_rx = pif->stm_ms.k2_rx;
    
    if(pif->stm_ms.ms_alarm.ms_ais)
    {
        pif_ms->ms_alarm |= 0x80;
    }

    if(pif->stm_ms.ms_alarm.ms_rdi)
    {
        pif_ms->ms_alarm |= 0x40;
    }
    
    if(pif->stm_ms.ms_alarm.ms_rei)
    {
        pif_ms->ms_alarm |= 0x20;
    }
    
    if(pif->stm_ms.ms_alarm.ms_exc)
    {
        pif_ms->ms_alarm |= 0x10;
    }
    
    if(pif->stm_ms.ms_alarm.ms_deg)
    {
        pif_ms->ms_alarm |= 0x08;
    }

    return 0;
}

int stm_if_snmp_ms_bulk_get(uint32_t ifindex, struct stm_ms_info stm_ms[])
{
    struct hash_bucket     *pbucket = NULL;
    struct hash_bucket     *pnext   = NULL;
    struct stm_if          *pif = NULL;
    struct stm_ms_info      pif_ms;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct stm_ms_info);
    int cursor;
    int val = 0;
    
    if (ifindex == 0)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
        {
            pif = (struct stm_if *)pbucket->data;
            if (pif == NULL)
            {
                zlog_err ( "%s[%d] pif is NULL\n", __FUNCTION__, __LINE__ );
                continue;
            }
            
            memset(&pif_ms, 0, sizeof(struct stm_ms_info));
            stm_if_snmp_ms_get(pif, &pif_ms);
            memcpy(&stm_ms[data_num++], &pif_ms, sizeof(struct stm_ms_info));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else /* 找到 ifindex 后面第一个数据 */
    {
        pbucket = hios_hash_find(&stm_if_table, (void *)ifindex);
        if (pbucket == NULL)    /* 未找到 ifindex 对应的数据 */
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val %= stm_if_table.compute_hash((void *)ifindex);

            if (NULL != stm_if_table.buckets[val])    /* 当前哈希桶链表不为空 */
            {
                pbucket = stm_if_table.buckets[val];
            }
            else                                        /* 当前哈希桶链表为空 */
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != stm_if_table.buckets[val])
                    {
                        pbucket = stm_if_table.buckets[val];
                    }
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&stm_if_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = (struct stm_if *)pnext->data;
                
                memset(&pif_ms, 0, sizeof(struct stm_ms_info));
                stm_if_snmp_ms_get(pif, &pif_ms);
                memcpy(&stm_ms[data_num], &pif_ms, sizeof(struct stm_ms_info));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}


/*get 高阶配置，部分数据需要从底层获取*/
int stm_if_snmp_hp_get(struct stm_if *pif, struct stm_hp_info *pif_hp, uint16_t vc4_id)
{
    struct stm_hp_t        *stm_hp = NULL;
    //struct stm_oh_info_t   *oh_status = NULL;
    //struct stm_count_info  *stm_count = NULL;
    
    stm_hp = stm_hp_lookup(pif->stm_hp_list, vc4_id);
    if(stm_hp == NULL)
    {
        zlog_err("%s:%s[%d] stm_hp_lookup error!\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    pif_hp->ifindex = pif->ifindex;
    pif_hp->vc4_id  = stm_hp->vc4_id;
    pif_hp->c2_tx   = stm_hp->hp_info.c2_tx;
    pif_hp->exc_thr = -(stm_hp->hp_info.exc_thr);   //过量与劣化门限值设置均为负数，这里直接取正
    pif_hp->deg_thr = -(stm_hp->hp_info.deg_thr);
    memcpy(pif_hp->j1_tx, stm_hp->hp_info.j1_tx , STM_OVERHEAD_LEN);
#if 0
    //收到的k2需要从适配层获取
    if (1 == vc4_id)    // FIXME
    {
        oh_status = stm_if_get_status(pif->ifindex);
        if(oh_status != NULL)
        {
            memcpy(pif_hp->j1_rx, oh_status->j1 + 1, STM_OVERHEAD_LEN);
            pif_hp->c2_rx = oh_status->c2;
            pif_hp->g1_rx = oh_status->g1;
        }
    }

    //b3、rei_count的统计计数同样需要从适配层获取
    if (1 == vc4_id)    // FIXME
    {
        stm_count = stm_if_get_count(pif->ifindex);
        if(stm_count != NULL)
        {
            pif_hp->b3 = stm_count->B3;
            //pif_hp->rei_count = stm_count->VC4_REI;
        }
    }
#endif

    pif_hp->b3 = stm_hp->hp_info.b3;
    pif_hp->c2_rx = stm_hp->hp_info.c2_rx;
    pif_hp->g1_rx = stm_hp->hp_info.g1_rx;
    memcpy(pif_hp->j1_rx, stm_hp->hp_info.j1_rx, STM_OVERHEAD_LEN);

    if(stm_hp->hp_info.hp_alarm.au_lop)
    {
        pif_hp->hp_alarm |= 0x8000;
    }

    if(stm_hp->hp_info.hp_alarm.au_ais)
    {
        pif_hp->hp_alarm |= 0x4000;
    }
    
    if(stm_hp->hp_info.hp_alarm.tu_lom)
    {
        pif_hp->hp_alarm |= 0x2000;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_unq)
    {
        pif_hp->hp_alarm |= 0x1000;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_slm)
    {
        pif_hp->hp_alarm |= 0x0800;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_rdi)
    {
        pif_hp->hp_alarm |= 0x0400;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_rei)
    {
        pif_hp->hp_alarm |= 0x0200;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_exc)
    {
        pif_hp->hp_alarm |= 0x0100;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_deg)
    {
        pif_hp->hp_alarm |= 0x0080;
    }
    
    if(stm_hp->hp_info.hp_alarm.hp_tim)
    {
        pif_hp->hp_alarm |= 0x0040;
    }

    return 0;
}

int stm_if_snmp_hp_bulk_get(void *pdata, struct stm_hp_info stm_hp[])
{
    struct stm_hp_index    hp_index = *((struct stm_hp_index *)pdata);
    struct hash_bucket     *pbucket = NULL;
    struct hash_bucket     *pnext   = NULL;
    struct stm_if          *pif = NULL;
    struct stm_hp_info      pif_hp;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct stm_hp_info);
    int cursor;
    uint8_t vc4_id = 0;
    
    if (hp_index.ifindex == 0)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
        {
            pif = (struct stm_if *)pbucket->data;
            if (pif == NULL)
            {
                zlog_err ( "%s[%d] pif is NULL\n", __FUNCTION__, __LINE__ );
                continue;
            }

            for (vc4_id = 1; vc4_id <= pif->type; vc4_id++)
            {
                memset(&pif_hp, 0, sizeof(struct stm_hp_info));
                stm_if_snmp_hp_get(pif, &pif_hp, vc4_id);
                memcpy(&stm_hp[data_num++], &pif_hp, sizeof(struct stm_hp_info));
                if (data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&stm_if_table, (void *)(hp_index.ifindex));
        if (pbucket == NULL)
        {
            return 0;
        }
        
        if (0 == hp_index.vc4_id)
        {
            vc4_id = 1;
        }
        else
        {
            vc4_id = hp_index.vc4_id + 1;
        }

        while (pbucket != NULL)
        {
            pif = (struct stm_if *)pbucket->data;

            for (; vc4_id <= pif->type; vc4_id++)
            {
                memset(&pif_hp, 0, sizeof(struct stm_hp_info));
                stm_if_snmp_hp_get(pif, &pif_hp, vc4_id);
                memcpy(&stm_hp[data_num++], &pif_hp, sizeof(struct stm_hp_info));
                if (data_num == msg_num)
                {
                    return data_num;
                }
            }

            pnext = hios_hash_next_cursor(&stm_if_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            
            pbucket = pnext;
            vc4_id  = 1;
        }
    }

    return data_num;
}


/* 查找传入 msp_id 之后对应的 n 个数据 */
int stm_if_snmp_msp_bulk_get(uint32_t msp_id, struct stm_msp_config stm_msp[])
{
    struct listnode *node = NULL;
    struct listnode *pnode = NULL;
    int *pdata = NULL;
    struct stm_msp_config   *pmsp = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct stm_msp_config);
    uint8_t *msp_status = NULL;


    if (msp_id == 0)   /* 初次获取 */
    {
        for (ALL_LIST_ELEMENTS_RO (stm_msp_list, node, pdata))
        {
            pmsp  = (struct stm_msp_config *)pdata;
            if (pmsp == NULL)
            {
                zlog_err ( "%s[%d] pmsp is NULL\n", __FUNCTION__, __LINE__ );
                continue;
            }

            msp_status = stm_msp_get_status(pmsp->master_if);
            if(msp_status == NULL)
            {
                zlog_err("%s:%s[%d] Get status of msp-group %d error!\n", __FILE__, __FUNCTION__, __LINE__, pmsp->msp_id);
                *msp_status = -1;
            }

            pmsp->status = *msp_status;
            memcpy(&stm_msp[data_num++], pmsp, sizeof(struct stm_msp_config));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }

    }
    else /* 找到 msp_id 后面第一个数据 */
    {
        for (pnode = listhead(stm_msp_list); pnode; pnode = listnextnode(pnode))
        {
            pmsp = ( struct stm_msp_config *) listgetdata(pnode);
            if ( pmsp != NULL )
            {
                if(pmsp->msp_id == msp_id)
                {
                    break;
                }
            }
        }
        
        if (pnode != NULL)
        {
            for (pnode = listnextnode(pnode); pnode != NULL; pnode = listnextnode(pnode))
            {
                pmsp = (struct stm_msp_config *)listgetdata(pnode);
                msp_status = stm_msp_get_status(pmsp->master_if);
                if(msp_status == NULL)
                {
                    zlog_err("%s:%s[%d] Get status of msp-group %d error!\n", __FILE__, __FUNCTION__, __LINE__, pmsp->msp_id);
                    *msp_status = -1;
                }
                
                pmsp->status = *msp_status;
                memcpy(&stm_msp[data_num++], pmsp, sizeof(struct stm_msp_config));
                if (data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }

    return data_num;
}

/* 查找传入 msp_id 之后对应的 n 个数据 */

int stm_if_snmp_dxc_bulk_get(void *data, uint32_t data_len, struct stm_dxc_config stm_dxc[])
{
    struct listnode *node = NULL;
    struct listnode *pnode = NULL;
    int *pdata = NULL;
    struct stm_dxc_config   *pdxc = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct stm_dxc_config);
//    uint8_t *dxc_status = NULL;
    char *pname = (char *)data;

    if(NULL == pname)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    if(0 == data_len)  /* 初次获取 */
    {
        for (ALL_LIST_ELEMENTS_RO (stm_dxc_list, node, pdata))
        {
            pdxc  = (struct stm_dxc_config *)pdata;
            if (pdxc == NULL)
            {
                zlog_err ( "%s[%d] pdxc is NULL\n", __FUNCTION__, __LINE__ );
                continue;
            }
#if 0
            dxc_status = stm_dxc_get_status(pdxc);
            if(dxc_status == NULL)
            {
                zlog_err("%s:%s[%d] Get status of msp-group %s error!\n", __FILE__, __FUNCTION__, __LINE__, pdxc->name);
                *dxc_status = -1;
            }
            pdxc->status = *dxc_status;         
#endif      
            memcpy(&stm_dxc[data_num++], pdxc, sizeof(struct stm_dxc_config));
            if (data_num == msg_num)
            {
                return data_num;
                
            }
        }

    }
    else /* 找到 msp_id 后面第一个数据 */
    {
        
        for (pnode = listhead(stm_dxc_list); pnode; pnode = listnextnode(pnode))
        {
            pdxc = ( struct stm_dxc_config *) listgetdata(pnode);
            if ( pdxc != NULL )
            {
                if(0 == strcmp(pdxc->name, pname))
                {
                    break;
                }
            }
        }
        
        if (pnode != NULL)
        {
            for (pnode = listnextnode(pnode); pnode != NULL; pnode = listnextnode(pnode))
            {
                pdxc = (struct stm_dxc_config *)listgetdata(pnode);
#if 0               
                dxc_status = stm_dxc_get_status(pdxc);
                if(dxc_status == NULL)
                {
                    zlog_err("%s:%s[%d] Get status of dxc-group %s error!\n", __FILE__, __FUNCTION__, __LINE__, pdxc->name);
                    *dxc_status = -1;
                }
                
                pdxc->status = *dxc_status;
#endif              
                memcpy(&stm_dxc[data_num++], pdxc, sizeof(struct stm_dxc_config));
                if (data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    return data_num;
}

int stm_if_loopback_status_get(uint32_t ifindex, uint32_t *lb_status)
{
    struct stm_if   *pif = NULL;

    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        zlog_err("%s:%s[%d] Find ifindex %x fail!\n", __FILE__, __FUNCTION__, __LINE__, ifindex);
        *lb_status = -1;
    }

    *lb_status = pif->loopback;
    return 0;
}

/*update slot info when stm interface added success*/
void stm_fpga_image_type_set(int slot, uint8_t image_type)
{
    g_stm_slot[slot].image_type = image_type;
}


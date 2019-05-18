#include "vtysh_sync.h"
#include "vtysh_session.h"
#include <lib/devm_com.h>
#include <lib/module_id.h>
#include "vtysh_session.h"
#include <lib/msg_ipc.h>
#include <error.h>
#include <lib/command.h>
#include <lib/memtypes.h>
#include <assert.h>
#include <lib/memory.h>
#include <lib/sys_ha.h>
#include <sys/prctl.h>
#include <lib/buffer.h>
#include <lib/linklist.h>

struct vtysh_sync syncvty;

void vtysh_init_sync()
{
    syncvty.vty_slot = VTY_UNKNOW_SLOT;
    syncvty.self_slot = 0;
    syncvty.peer_slot = 0;
    syncvty.main_slot_num = 0;
    syncvty.slave_slot_num = 0;
    syncvty.cmd_main_send_cnt = 0;
    syncvty.cmd_slave_exec_cnt = 0;
    syncvty.cmd_slave_recv_cnt = 0;
    syncvty.cmd_slave_success_cnt = 0;
    syncvty.cmd_slave_failed_cnt = 0;
    syncvty.all_cards_ready_flag = 0;
    syncvty.ms_x_flag = 0;
    devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_VTY, MODULE_ID_VTY);
    /*注册卡拔出事件*/
    devm_event_register(DEV_EVENT_SLOT_DELETE, MODULE_ID_VTY, MODULE_ID_VTY);

}

/*init slave vtysh, creat vtylist and sessionlist for sync-type vty*/
void vtysh_init_slave()
{
    vtyvec_sync = vector_init(VECTOR_MIN_SIZE);
    user_session_sync_list = list_new();
}

void vtysh_get_slot()
{
    struct devm_unit devm_slot;
    int ret = 0;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    ret = devm_comm_get_unit(1, MODULE_ID_VTY, &devm_slot);
    VTY_DEBUG(VD_SYNC, "slot_main_board:%d, slot_slave_board:%d myslot:%d\n", devm_slot.slot_main_board, devm_slot.slot_slave_board, devm_slot.myslot);
    printf("slot_main_board:%d, slot_slave_board:%d myslot:%d ret=%d\r\n", devm_slot.slot_main_board, devm_slot.slot_slave_board, devm_slot.myslot, ret);
#ifdef VTYSH_SYNC_DEBUG
    printf("slot_main_board:%d, slot_slave_board:%d myslot:%d\n", devm_slot.slot_main_board, devm_slot.slot_slave_board, devm_slot.myslot);
#endif

    /*no slot device*/
    if(ret == 0)
    {
        /*check slot*/
        if(devm_slot.myslot == devm_slot.slot_main_board)
        {
            syncvty.vty_slot = VTY_MAIN_SLOT;

            /*record slot number of peer slot*/
            syncvty.self_slot = devm_slot.slot_main_board;
            syncvty.peer_slot = devm_slot.slot_slave_board;
            syncvty.slave_slot_num = devm_slot.slot_slave_board;
            syncvty.main_slot_num = devm_slot.slot_main_board;
            VTY_DEBUG(VD_SYNC, "syncvty.self_slot:%d syncvty.peer_slot:%d vty_slot: main:%d\n",
                       syncvty.self_slot, syncvty.peer_slot, syncvty.vty_slot);
#ifdef VTYSH_SYNC_DEBUG
            printf("syncvty.self_slot:%d syncvty.peer_slot:%d vty_slot: main:%d\n",
                   syncvty.self_slot, syncvty.peer_slot, syncvty.vty_slot);
#endif
        }
        else if(devm_slot.myslot == devm_slot.slot_slave_board)
        {
            syncvty.vty_slot = VTY_SLAVE_SLOT;

            syncvty.self_slot = devm_slot.slot_slave_board;
            syncvty.peer_slot = devm_slot.slot_main_board;
            syncvty.slave_slot_num = devm_slot.slot_slave_board;
            syncvty.main_slot_num = devm_slot.slot_main_board;
            VTY_DEBUG(VD_SYNC, "syncvty.peer_slot:%d\n", syncvty.peer_slot);
            VTY_DEBUG(VD_SYNC, "vty_slot slave:%d\n", syncvty.vty_slot);
#ifdef VTYSH_SYNC_DEBUG
            printf("syncvty.self_slot:%d syncvty.peer_slot:%d vty_slot: main:%d\n", syncvty.self_slot, syncvty.peer_slot, syncvty.vty_slot);
#endif

        }
        else
        {
            syncvty.vty_slot = VTY_UNKNOW_SLOT;
            VTY_DEBUG(VD_SYNC, "VTY_UNKNOW_SLOT!\n");
#ifdef VTYSH_SYNC_DEBUG
            printf("VTY_UNKNOW_SLOT!\n");
#endif

        }
    }
}


int vtysh_get_slot_type(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    switch(devm_com->ha_role)
    {
        case HA_ROLE_MASTER:
            if(syncvty.vty_slot == VTY_UNKNOW_SLOT)
            {
                syncvty.vty_slot = VTY_MAIN_SLOT;
                /*record slot number of peer slot*/
                syncvty.peer_slot = devm_com->slave_slot;
                syncvty.self_slot = devm_com->main_slot;

                syncvty.main_slot_num = devm_com->main_slot;
                syncvty.slave_slot_num = devm_com->slave_slot;
                VTY_DEBUG(VD_SYNC, "syncvty.peer_slot:%d devm_com->slave_slot:%d\n",
                           syncvty.peer_slot, devm_com->slave_slot);
                VTY_DEBUG(VD_SYNC, "syncvty.self_slot:%d devm_com->main_slot:%d\n",
                           syncvty.self_slot, devm_com->main_slot);

                VTY_DEBUG(VD_SYNC, "vty_slot: main:%d\n", syncvty.vty_slot);
            }

            break;

        case HA_ROLE_SLAVE:
            if(syncvty.vty_slot == VTY_MAIN_SLOT)
            {
                VTY_DEBUG(VD_SYNC, "master vty revc slave start syncvty.peer_slot:%d\n", syncvty.peer_slot);
                syncvty.peer_slot = devm_com->slave_slot;
            }
            else if(syncvty.vty_slot == VTY_SLAVE_SLOT)
            {
                VTY_DEBUG(VD_SYNC, "slave vty recv slave start\n");
                syncvty.vty_slot = VTY_SLAVE_SLOT;
                syncvty.peer_slot = devm_com->main_slot;
                syncvty.self_slot = devm_com->slave_slot;

                syncvty.main_slot_num = devm_com->main_slot;
                syncvty.slave_slot_num = devm_com->slave_slot;
                VTY_DEBUG(VD_SYNC, "syncvty.peer_slot:%d\n", syncvty.peer_slot);
                VTY_DEBUG(VD_SYNC, "vty_slot slave:%d\n", syncvty.vty_slot);
            }

            break;

        default:
            syncvty.vty_slot = VTY_UNKNOW_SLOT;
            VTY_DEBUG(VD_SYNC, "VTY_UNKNOW_SLOT!\n");
            return -1;
    }

    syncvty.self_slot = devm_com->slot;

    return 0;
}

void vtysh_send_start_to_ha()
{
    int ret;
    struct devm_com devm_com;

    syncvty.self_status = VTYSH_START;
    memset(&devm_com, 0, sizeof(devm_com));
    devm_com.ha_status = HA_BOOTSTA_END;

    VTY_DEBUG(VD_SYNC, "vtysh_send_start_to_ha\n");
#ifdef VTYSH_SYNC_DEBUG
    printf("vtysh_send_start_to_ha\n");
#endif

//    if((ret = ipc_send_common1(&devm_com, sizeof(devm_com), 0, MODULE_ID_HA,
//                               MODULE_ID_VTY, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0)) < 0)

    if((ret = ipc_send_msg_n2(&devm_com, sizeof(devm_com), 0, MODULE_ID_DEVM,
                               MODULE_ID_VTY, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0)) < 0)                               
    {
        zlog_err("vtysh_send_start_to_ha err:%d\n", ret);
    }
}

void vtysh_send_dataok_to_ha()
{
    int ret;
    struct devm_com devm_com;
    memset(&devm_com, 0, sizeof(devm_com));
    devm_com.ha_status = HA_BOOTSTA_DATAOK;
    VTY_DEBUG(VD_SYNC, "vtysh_send_dataok_to_ha\n");
#ifdef VTYSH_SYNC_DEBUG
    printf("vtysh_send_start_to_ha\n");
#endif

//    ret = ipc_send_common1(&devm_com, sizeof(devm_com), 0, MODULE_ID_HA,
//                           MODULE_ID_VTY, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
    ret = ipc_send_msg_n2(&devm_com, sizeof(devm_com), 0, MODULE_ID_DEVM,
                             MODULE_ID_VTY, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
    if(ret < 0)
    {
        zlog_err("vtysh_send_start_to_ha err:%d\n", ret);
    }
}

void vtysh_send_batchfile_to_filem()
{
    struct ha_batch_req batch_config;

    memset(&batch_config, 0, sizeof(batch_config));
    strcpy(batch_config.bFile, HA_CONFIG_BATCHSYNC_FILE);
    batch_config.slot = syncvty.peer_slot;
    batch_config.unit = 1;

    VTY_DEBUG(VD_SYNC, "ipc_send_filem syncvty.peer_slot:%d\n", syncvty.peer_slot);

    printf("send batchfile to filem, peer_slot:%d\n", syncvty.peer_slot);  


//    ipc_send_filem(&batch_config, sizeof(batch_config), 1, MODULE_ID_FILE,
//                   MODULE_ID_VTY, IPC_TYPE_HA, HA_SMSG_BATCH_START, 0);
    if(ipc_send_msg_n2(&batch_config, sizeof(batch_config), 1, MODULE_ID_FILE,
                   MODULE_ID_VTY, IPC_TYPE_HA, HA_SMSG_BATCH_START, 0, 0) < 0)
    {
        zlog_err("%s-%d:send msg to filem fail \n",__FUNCTION__, __LINE__);
    }
    
    syncvty.self_status = VTYSH_BATCH_CONFIG_SENT;
}

#if 0
void vtysh_cmd_sync_slot(void *arg)
{
    while(1)
    {
        /*exchange main/slave slot*/
        if(syncvty.slot_change_cnt == 1)
        {
            vtysh_change_main_slave();
        }

        /*slave slot*/
        if(syncvty.vty_slot == VTY_SLAVE_SLOT)
        {
            /*Real-time sync after Batch sync*/
            if(syncvty.self_status == VTYSH_CONFIG
                    || syncvty.self_status == VTYSH_CONFIG_FINISH)
            {
                vtysh_recv_data_from_ha();
            }
        }
        else if(syncvty.vty_slot == VTY_MAIN_SLOT)
        {
            syncvty.peer_status = vtysh_check_slave_status();

            /*sync config file after slave start*/
            if(syncvty.peer_status == HA_BOOTSTA_END)
            {
                vtysh_sync_config_to_slave();
            }


            if(syncvty.peer_status == HA_BOOTSTA_DATAOK)
            {
            }
        }
        else
        {
            zlog_err("vtysh get wrong slot type!");
            break;
        }
    }
}


/*封装消息，并发送给cdp*/
int vtysh_master_send_cdp(struct vty *vty, enum vty_sync_msg_type sync_type)
{
    int index;
    struct vty_sync_msg *sync_msg;
    struct login_session *session;
    int ret;

    session = (struct login_session *)vty->session;
    sync_msg = (struct vty_sync_msg *)malloc(sizeof(struct vty_sync_msg));
    memset(sync_msg, 0, sizeof(struct vty_sync_msg));

    sync_msg->type = sync_type;
    sync_msg->vty_type = vty->type;
    strcpy(sync_msg->ttyname, session->address);
    sync_msg->node = vty->node;

    if(sync_type == VTY_SYNC_CMD)
    {
        strcpy(sync_msg->cmd, vty->buf);
    }

    /*send msg to cdp*/
    ret = ipc_send_cdp(sync_msg, sizeof(struct vty_sync_msg), 1, MODULE_ID_VTY, IPC_TYPE_SYNC_DATA, sync_type, 0, index);

    if(ret < 0)
    {
        zlog_err("send ha error:type");
    }

    free(sync_msg);

    return ret;
}
#endif

/*创建批量同步配置文件，并通知filem*/
void vtysh_master_handle_batch_sync()
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    vtysh_creat_batch_config_file();
    syncvty.slave_batch_busy_flag = 1;
    VTY_DEBUG(VD_SYNC, "syncvty.slave_batch_busy_flag = 1\n");

    vtysh_send_batchfile_to_filem();
}

/*主卡创建批量配置同步文件时使用的临时vty*/
struct vty *
vty_batch_create()
{
    struct vty *vty;
    vty = vty_new();

    vty->type = VTY_FILE;

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    /* Do not connect until we have passed authentication. */
    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        exit(1);
    }

    return vty;

}

/*删除临时vty*/
void vty_batch_delete(struct vty *vty)
{
    assert(vty != NULL);

    unsigned int i;

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        if(vty->connect_fd[i] != -1)
        {
            close(vty->connect_fd[i]);
        }
    }

    buffer_free(vty->obuf);

    for(i = 0; i < VTY_MAXHIST; i++)
        if(vty->hist[i])
        {
            XFREE(MTYPE_VTY_HIST, vty->hist[i]);
        }

    if(vty->buf)
    {
        XFREE(MTYPE_VTY, vty->buf);
    }

    if(vty->buf_tmp)
    {
        XFREE(MTYPE_VTY, vty->buf_tmp);
    }

    XFREE(MTYPE_VTY, vty);
}

/*sync vty to salve*/
/*发送VTY_SYNC_CREATE消息*/
int vtysh_master_sync_vty(struct vty *vty, enum vty_sync_msg_type type, char *cmd)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
    VTY_DEBUG(VD_SYNC, "vty->type:%d\n\n", vty->type);
    return(vtysh_ha_send(vty, type, cmd));
//  return(vtysh_cdp_send(vty, type, cmd));
}

/*sync vty to salve*/
/*发送VTY_SYNC_CREATE消息*/
int vtysh_master_sync_cmd(struct vty *vty, enum vty_sync_msg_type type, const char *cmd)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
    VTY_DEBUG(VD_SYNC, "vtysh_master_sync_cmd:%s\n", cmd);
    return(vtysh_ha_send(vty, type, cmd));
}

/*发送VTY_SYNC_CREATE消息*/
int vtysh_master_sync_exit(struct vty *vty, enum vty_sync_msg_type type, char *cmd)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
    return(vtysh_ha_send(vty, type, cmd));
//  return(vtysh_cdp_send(vty, type, cmd));
}


/*send data to slave vtysh from ha*/
int vtysh_ha_send(struct vty *vty, enum vty_sync_msg_type type, const char *cmd)
{
    struct vty_sync_msg vty_sync_msg;
    int ret = -1;

    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

    memset(&vty_sync_msg, 0, sizeof(struct vty_sync_msg));
    vty_sync_msg.msg_type = type;
    /*put vty msg*/
    vty_sync_msg.vty_type = vty->type;
    strcpy(vty_sync_msg.ttyname, vty->ttyname);

    /*put cmd data*/
    if(type == VTY_SYNC_CMD)
    {
        if(cmd == NULL)
        {
            zlog_err("vtysh_cdp_send VTY_SYNC_CMD empty cmd\n");
        }
        else
        {
            strcpy(vty_sync_msg.cmd, cmd);
        }
    }

//    ret = ipc_send_ha(&vty_sync_msg, sizeof(vty_sync_msg), 1, MODULE_ID_VTY,
//                      IPC_TYPE_SYNC_DATA, type, 0, 0); 
    ret = ipc_send_msg_ha_n2(&vty_sync_msg, sizeof(vty_sync_msg), 1, MODULE_ID_VTY, MODULE_ID_VTY, 
                                               IPC_TYPE_SYNC_DATA, (uint16_t)type, 0, 0, 0);   
    VTY_DEBUG(VD_SYNC, "type IPC_TYPE_SYNC_DATA \n");  
    if(ret < 0)
    {
        zlog_err("%s:send ha msg failed \n",__FUNCTION__);
    }
    if(ret < 0)
    {
        zlog_err("%s:send ha msg failed \n",__FUNCTION__);
    }
    VTY_DEBUG(VD_SYNC, "ipc_send_ha ret:%d\n", ret);

    return ret;
}


/*Encapsulate sync message, and send to cdp*/
int vtysh_cdp_send(struct vty *vty, enum vty_sync_msg_type type, char *cmd)
{
    struct vty_sync_msg vty_sync_msg;
    int ret = -1;

    vty_sync_msg.msg_type = type;
    /*put vty msg*/
    vty_sync_msg.vty_type = vty->type;
    strcpy(vty_sync_msg.ttyname, vty->ttyname);

    /*put cmd data*/
    if(type == VTY_SYNC_CMD)
    {
        if(cmd == NULL)
        {
            zlog_err("vtysh_cdp_send VTY_SYNC_CMD empty cmd\n");
        }
        else
        {
            strcpy(vty_sync_msg.cmd, cmd);
        }
    }

    /*msg encapsulate ok, send to cdp*/
//  if(ret = cdp_send() < 0)
//      zlog_err("cdp_send err\n");

    return ret;
}

/*master vtysh recv slave data ok, change status*/
/*主卡收到备卡批量同步完成消息后，同步主卡的vty*/
void vtysh_master_handle_slave_dataok()
{
    struct vty *v;
    unsigned int i;

    printf("slave batch file complete !\n", __FILE__, __LINE__, __func__);

    /*change slave status to config_finish*/
    /*备卡批量同步完成后，进入批量同步状态*/
    syncvty.peer_status = VTYSH_REALTIME_SYNC_ENABLE;
    syncvty.self_status = VTYSH_SYNC_CATCH_CMD;

    for(i = 0; i < vector_active(vtyvec); i++)
    {
        if((v = vector_slot(vtyvec, i)) != NULL)
        {
            if(v->type == VTY_FILE)
            {
                continue;
            }

            vtysh_master_sync_vty(v, VTY_SYNC_CREAT, NULL);

            struct listnode *node, *nnode;
            char *cmd;

            /*同步vty之后，将该vty缓存的实时命令同步到备卡*/
            if(v->catch_buf != NULL
                    && v->catch_buf->head != NULL)
                for(ALL_LIST_ELEMENTS(v->catch_buf, node, nnode, cmd))
                {
                    vtysh_master_sync_cmd(v, VTY_SYNC_CMD, cmd);
                }
        }
    }

    /*vty 和缓存命令同步完成后，主卡进入实时同步状态*/
    syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
    /*进入实时同步，不再需要关心批量同步的真空期*/
    syncvty.slave_batch_busy_flag = 0;

}

/*主卡收到备卡启动完成消息，启动批量同步，创建线程后退出*/
int vtysh_master_recv_slave_start()
{
//  int err;
//  pthread_attr_t attr;
//  pthread_t tid;

    /*slave status change to start*/
    syncvty.peer_status = VTYSH_START;

    printf("mater get slave boot complt msg\n");   


    /* 批量同步需要：                    *
     * 1、所有板卡加载完毕             *
     * 2、主卡配置恢复完成             *
     * 3、备卡状态为start             */
    if(syncvty.all_cards_ready_flag == 1
            && syncvty.vty_slot == VTY_MAIN_SLOT
            && syncvty.peer_status == VTYSH_START
            && syncvty.self_status >= VTYSH_BATCH_CONFIG_FINISH)
    {

        vtysh_batch_sync_start();
    }

    return 0;
}

/*准备开始批量同步*/
void vtysh_batch_sync_start()
{
    /*create batch sync thread*/
    vtysh_thread_creat(&vtysh_master_handle_peer_start);
}

/*thread, sync batch config to slave*/
/*线程：处理批量同步*/
void *vtysh_master_handle_peer_start(void *arg)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

    /*main vty has already finish config here*/
    if(syncvty.self_status >= VTYSH_BATCH_CONFIG_FINISH)
    {
        /*collect / create tmp config file, tell filem to send*/
        vtysh_master_handle_batch_sync();
    }
    else
    {
        printf("master cant not send batch files, status %d \n", syncvty.self_status);
    }

    return (void *) 0;
}

/*主卡缓存命令或实时同步命令*/
int vtysh_mster_catch_realtime_cmd(const char *line, struct vty *vty)
{
    if(syncvty.vty_slot != VTY_MAIN_SLOT)
    {
        return -1;
    }

    /*只有主卡会发送命令行*/
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            && syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE)
    {
        if(!strncmp(line, "setmx", strlen("setmx")))
        {
            return 0;
        }

        VTY_DEBUG(VD_SYNC, "send cmd to slave\n");
        vtysh_master_sync_cmd(vty, VTY_SYNC_CMD, line);
    }

    /*批量同步完成后，同步缓存的节点命令，并清空缓存，需要加锁*/
    //      pthread_mutex_lock (&vty->catch_buf_lock);
    /*节点回到CONFIG后，清空缓存*/
    /*1、所有命令都缓存
     *2、只有在slave_batch_busy_flag==0的时候才会清缓存*/
    if(syncvty.slave_batch_busy_flag == 1)
    {
        VTY_DEBUG(VD_SYNC, "slave_batch_busy_flag==1 record all cmd\n");
        VTY_DEBUG(VD_SYNC, "record cmd:%s\n", line);

        /*记录缓存*/
        if(vty->catch_buf == NULL)
        {
            vty->catch_buf = list_new();
        }

        /*add cmd into catch vty->catch_buf*/

        char *catch_cmd = NULL;
        catch_cmd = (char *)malloc(strlen(line) + 1);
        strcpy(catch_cmd, line);
        listnode_add(vty->catch_buf, catch_cmd);
    }
    else
    {
        if(vty->node == CONFIG_NODE)
        {
            if(syncvty.slave_batch_busy_flag == 0)
            {
                if(vty->catch_buf == NULL || vty->catch_buf->head == NULL)
                {
                    return 0;
                }

                VTY_DEBUG(VD_SYNC, "config node, clear all\n");
                list_delete_all_node(vty->catch_buf);
            }
        }
        else
        {
            VTY_DEBUG(VD_SYNC, "record cmd:%s\n", line);

            /*记录缓存*/
            if(vty->catch_buf == NULL)
            {
                vty->catch_buf = list_new();
            }

            /*add cmd into catch vty->catch_buf*/

            char *catch_cmd = NULL;
            catch_cmd = (char *)malloc(strlen(line) + 1);
            strcpy(catch_cmd, line);
            listnode_add(vty->catch_buf, catch_cmd);
        }
    }

    return 0;
}

#if 0
/*主卡缓存命令或实时同步命令*/
int vtysh_mster_catch_realtime_cmd(const char *line, struct vty *vty)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);

    /*只有主记录*/
    if(syncvty.vty_slot != VTY_MAIN_SLOT)
    {
        return -1;
    }

    /*主卡和备卡已经准备好，直接发送命令到备卡*/
//  if(syncvty.peer_status == VTYSH_REALTIME_SYNC_ENABLE
//      && syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE)
//  {

    if(!strncmp(line, "setmx", strlen("setmx")))
    {
        VTY_DEBUG(VD_SYNC, "cmd:setmx skip\n");
        return 0;
    }

    VTY_DEBUG(VD_SYNC, "send cmd to slave\n");
//      return(vtysh_master_sync_cmd(vty, VTY_SYNC_CMD, line));
    vtysh_master_sync_cmd(vty, VTY_SYNC_CMD, line);
//  }

    pthread_mutex_lock(&vty->catch_buf_lock);

    if(syncvty.self_status == VTYSH_BATCH_CONFIG_SENT)
    {
        /*主卡已发送配置文件，但备卡未完成配置恢复，这段时间是真空期，所有命令都要缓存*/
        VTY_DEBUG(VD_SYNC, "VTYSH_BATCH_CONFIG_SENT record cmd:%s\n", line);

        /*记录缓存*/
        if(vty->catch_buf == NULL)
        {
            vty->catch_buf = list_new();
        }

        /*add cmd into catch vty->catch_buf*/

        char *catch_cmd = NULL;
        catch_cmd = (char *)malloc(strlen(line));
        strcpy(catch_cmd, line);
        listnode_add(vty->catch_buf, catch_cmd);
    }
    else
    {
        VTY_DEBUG(VD_SYNC, "vty->node:%d vty->pre_node:%d\n", vty->node, vty->pre_node);

        /*只记录改变的node*/
        if(vty->node == vty->pre_node)
        {
            VTY_DEBUG(VD_SYNC, "node unchange\n");
            return 0;
        }

        /*批量同步完成后，同步缓存的节点命令，并清空缓存，需要加锁*/
        //      pthread_mutex_lock (&vty->catch_buf_lock);
        /*节点回到CONFIG后，清空缓存*/
        if(vty->node == CONFIG_NODE)
        {
            if(vty->catch_buf == NULL || vty->catch_buf->head == NULL)
            {
                return 0;
            }

            VTY_DEBUG(VD_SYNC, "config node, clear all\n");
            list_delete_all_node(vty->catch_buf);
        }
        else
        {
            VTY_DEBUG(VD_SYNC, "record cmd:%s\n", line);

            /*记录缓存*/
            if(vty->catch_buf == NULL)
            {
                vty->catch_buf = list_new();
            }

            /*add cmd into catch vty->catch_buf*/

            char *catch_cmd = NULL;
            catch_cmd = (char *)malloc(strlen(line));
            strcpy(catch_cmd, line);
            listnode_add(vty->catch_buf, catch_cmd);
        }
    }

    pthread_mutex_unlock(&vty->catch_buf_lock);

    struct listnode *node, *nnode;
    char *cmd;

    for(ALL_LIST_ELEMENTS(vty->catch_buf, node, nnode, cmd))
    {
        VTY_DEBUG(VD_SYNC, "vty:%s catch cmd:%s\n", vty->ttyname, cmd);
    }

    return 0;
}
#endif

void vtysh_master_slave_exchange(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

//  syncvty.slot_change_flag = 1;
    switch(syncvty.vty_slot)
    {
        case VTY_MAIN_SLOT:
            vtysh_master_slave_exchange_mtos(devm_com);
            break;

        case VTY_SLAVE_SLOT:
            vtysh_master_slave_exchange_stom(devm_com);
            break;

        default:
            zlog_err("error slot:%d, failed to exchange slot!", syncvty.vty_slot);
            break;
    }

    VTY_DEBUG(VD_SYNC, "exchange:syncvty.main_slot_num:%d devm_com->main_slot:%d syncvty.slave_slot_num:%d devm_com->slave_slot:%d\n",
               syncvty.main_slot_num, devm_com->main_slot,
               syncvty.slave_slot_num, devm_com->slave_slot);
#ifdef VTYSH_SYNC_DEBUG
    printf("exchange:syncvty.main_slot_num:%d devm_com->main_slot:%d syncvty.slave_slot_num:%d devm_com->slave_slot:%d\n",
           syncvty.main_slot_num, devm_com->main_slot,
           syncvty.slave_slot_num, devm_com->slave_slot);
#endif

}

/*主卡切换到备卡*/
void vtysh_master_slave_exchange_mtos(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    if(syncvty.vty_slot != VTY_MAIN_SLOT)
    {
#ifdef VTYSH_SYNC_DEBUG
        printf("vtysh_master_slave_exchange_m not master slot!");
#endif
        zlog_err("vtysh_master_slave_exchange_m not master slot!");
        return;
    }

    syncvty.ms_x_flag = 1;
    syncvty.vty_slot = VTY_SLAVE_SLOT;
    syncvty.main_slot_num = devm_com->main_slot;
    syncvty.slave_slot_num = devm_com->slave_slot;
    VTY_DEBUG(VD_SYNC, "syncvty main_slot_num:%d slave_slot_num:%d self_slot:%d peer_slot:%d\n",
               syncvty.main_slot_num, syncvty.slave_slot_num, syncvty.self_slot, syncvty.peer_slot);
#ifdef VTYSH_SYNC_DEBUG
    printf("syncvty main_slot_num:%d slave_slot_num:%d self_slot:%d peer_slot:%d\n",
           syncvty.main_slot_num, syncvty.slave_slot_num, syncvty.self_slot, syncvty.peer_slot);
#endif

//  syncvty.self_slot = devm_com->slave_slot;
//  syncvty.peer_slot = devm_com->main_slot;
    printf("master -> slave: status %d \n", syncvty.self_status);
    switch(syncvty.self_status)
    {
        /*完成数据同步前，主卡直接切换到备卡最初始的状态*/
        case VTYSH_UNREADY:
        case VTYSH_START:
            break;

        case VTYSH_BATCH_CONFIG_START:
        case VTYSH_BATCH_CONFIG_FINISH:
 //       case VTYSH_BATCH_CONFIG_SENT:
            syncvty.self_status = VTYSH_START;
            break;
        case VTYSH_BATCH_CONFIG_SENT:  // 这个阶段也发生倒换，
        case VTYSH_SYNC_CATCH_CMD:
        case VTYSH_REALTIME_SYNC_ENABLE:
            /*close all connections*/
            /*主降备时，关掉所有的连接，包括串口*/
            vtysh_close_all_connection();
            syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
            break;

        default:
            break;

    }
}

/*配置恢复*/
void vtysh_master_config_recover()
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    /*reading_config_file表明正在进行配置恢复*/
    reading_config_file = 1;

    /*only main slot, vtysh will read config while starting*/
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            || syncvty.vty_slot == VTY_UNKNOW_SLOT)
    {
        VTY_DEBUG(VD_SYNC, "VTY_MAIN_SLOT read conifg\n");
#ifdef VTYSH_SYNC_DEBUG
        printf("VTY_MAIN_SLOT read conifg\n");
#endif
        syncvty.self_status = VTYSH_BATCH_CONFIG_START;
        /* Read vtysh configuration file after connecting to daemons. */
        vtysh_read_config(config_default, config_init);
        syncvty.self_status = VTYSH_BATCH_CONFIG_FINISH;
    }

    reading_config_file = 0;

    /*配置恢复完成标志位*/
    config_recover_finish = 1;

    /*主卡读完配置文件后向ha发送DATA_OK消息*/
    if(syncvty.vty_slot == VTY_MAIN_SLOT)
    {
        vtysh_send_dataok_to_ha();
    }
}

/*收到devm的start消息后，主卡执行配置恢复的线程*/
void *vtysh_master_config_recover_thread(void *arg)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

//  printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
//  prctl(PR_SET_NAME," vtysh_master_config_recover_thread");
    vtysh_master_config_recover();
    return (void *)0;
}

/*创建线程接口函数*/
int vtysh_thread_creat(void *(*func)(void *))
{
    int err;
    pthread_attr_t attr;
    pthread_t tid;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
    /*After batch-sycn, Creat real_time_sync thread, ready for real_time_sync*/
    err = pthread_attr_init(&attr);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(-1);
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(err == 0)
    {
        err = pthread_create(&tid, &attr, func, NULL);
    }

    if(err != 0)
    {
        perror("read pthread_create fail:");
        exit(-1);
    }

    return err;
}

int vtysh_get_slot_msg_type(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
    printf("syncvty.vty_slot:%d devm_com->type:%d\n",
           syncvty.vty_slot, devm_com->type);
#endif

    VTY_DEBUG(VD_SYNC, "syncvty.vty_slot:%d devm_com->type:%d\n",
               syncvty.vty_slot, devm_com->type);

    /*主备槽位是否发生变化？*/
    switch(vtysh_check_slot_change(devm_com))
    {
        case 0:
            /*unchange, return boot type*/
            VTY_DEBUG(VD_SYNC, "VTY_SLOT_MSG_TYPE_BOOT\n");
#ifdef VTYSH_SYNC_DEBUG
            printf("VTY_SLOT_MSG_TYPE_BOOT\n");
#endif
            return VTY_SLOT_MSG_TYPE_BOOT;

        case 1:
            /*slot change, return */
            VTY_DEBUG(VD_SYNC, "VTY_SLOT_MSG_TYPE_EXCHANGE\n");
#ifdef VTYSH_SYNC_DEBUG
            printf("VTY_SLOT_MSG_TYPE_EXCHANGE\n");
#endif
            return VTY_SLOT_MSG_TYPE_EXCHANGE;

        default:
            VTY_DEBUG(VD_SYNC, "VTY_SLOT_MSG_TYPE_ERR\n");
#ifdef VTYSH_SYNC_DEBUG
            printf("VTY_SLOT_MSG_TYPE_ERR\n");
#endif

            return VTY_SLOT_MSG_TYPE_ERR;
    }
}

/*return 0:unchange; return 1:change; return -1:err*/
int vtysh_check_slot_change(struct devm_com *devm_com)
{
    int ret = -1;

    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
    VTY_DEBUG(VD_SYNC, "syncvty.main_slot_num:%d devm_com->main_slot:%d syncvty.slave_slot_num:%d devm_com->slave_slot:%d\n",
               syncvty.main_slot_num, devm_com->main_slot,
               syncvty.slave_slot_num, devm_com->slave_slot);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
    printf("syncvty.main_slot_num:%d devm_com->main_slot:%d syncvty.slave_slot_num:%d devm_com->slave_slot:%d\n",
           syncvty.main_slot_num, devm_com->main_slot,
           syncvty.slave_slot_num, devm_com->slave_slot);
#endif

    /*wrong devm slot type */
    if(devm_com->type > SLOT_TYPE_BOARD_SLAVE)
    {
        VTY_DEBUG(VD_SYNC, "wrong devm_com->type:%d\n", devm_com->type);
        return ret;
    }

    switch(syncvty.vty_slot)
    {
        case VTY_UNKNOW_SLOT:
            zlog_err("vtysh_check_slot_change unknow slot type\n");
            break;

        case VTY_MAIN_SLOT:
        case VTY_SLAVE_SLOT:

            /*尚未获取主备信息, unchange*/
            if(syncvty.main_slot_num == 0
                    && syncvty.slave_slot_num == 0)
            {
                VTY_DEBUG(VD_SYNC, "have not get slot num yet\n");
#ifdef VTYSH_SYNC_DEBUG
                printf("have not get slot num yet\n");
#endif

                ret = 0;
            }
            else
            {
                if(syncvty.main_slot_num != devm_com->main_slot
                        && syncvty.slave_slot_num != devm_com->slave_slot)
                {
                    ret = 1;
                }
                else
                {
#ifdef VTYSH_SYNC_DEBUG
                    printf("not exchange\n");
#endif

                    VTY_DEBUG(VD_SYNC, "not exchange\n");
                    ret = 0;
                }
            }

            VTY_DEBUG(VD_SYNC, "devm_com->slot:%d syncvty.self_slot:%d\n", devm_com->slot, syncvty.self_slot);
#ifdef VTYSH_SYNC_DEBUG
            printf("devm_com->slot:%d syncvty.self_slot:%d\n", devm_com->slot, syncvty.self_slot);
#endif

            /*do not care peer slot exchang msg*/
            if(ret == 1 && devm_com->slot != syncvty.self_slot)
            {
                syncvty.peer_slot = devm_com->slot; 
#ifdef VTYSH_SYNC_DEBUG
                printf("do not care peer slot exchang msg\n");
#endif
                VTY_DEBUG(VD_SYNC, "do not care peer slot exchang msg\n");
                ret = -1;
            }

            break;

        default:
#ifdef VTYSH_SYNC_DEBUG
            printf("wrong syncvty.vty_slot:%d\n", syncvty.vty_slot);
#endif
            zlog_err("wrong syncvty.vty_slot:%d\n", syncvty.vty_slot);
            break;
    }

    return ret;
}

void vtysh_handle_bootend(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    syncvty.peer_status = VTYSH_START;

    /*only main vty care only peer status*/
    if(devm_com->type == syncvty.vty_slot
            || syncvty.vty_slot == VTY_SLAVE_SLOT)
    {
#ifdef VTYSH_SYNC_DEBUG
        printf("vtysh not VTY_MASTER_SLOT, break\n");
#endif
        VTY_DEBUG(VD_SYNC, "vtysh not VTY_MASTER_SLOT, break\n");
        return;
    }

#ifdef VTYSH_SYNC_DEBUG
    printf("all_cards_ready_flag:%d slot_type:%d peer_status:%d self_status:%d\n",
           syncvty.all_cards_ready_flag, syncvty.vty_slot,
           syncvty.peer_status, syncvty.self_status);
#endif
    VTY_DEBUG(VD_SYNC, "all_cards_ready_flag:%d slot_type:%d peer_status:%d self_status:%d\n",
               syncvty.all_cards_ready_flag, syncvty.vty_slot,
               syncvty.peer_status, syncvty.self_status);
    vtysh_master_recv_slave_start();
    return;
}

void vtysh_handle_bootok(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "vtysh HA_BOOTSTA_DATAOK\n");
#ifdef VTYSH_SYNC_DEBUG
    printf("vtysh HA_BOOTSTA_DATAOK\n");
#endif

    /*状态与之前相同，drop*/
    if(devm_com->slot == syncvty.self_slot)
    {
        if(syncvty.self_status == VTYSH_BATCH_CONFIG_FINISH
                || syncvty.self_status == VTYSH_BATCH_CONFIG_SENT
                || syncvty.self_status == VTYSH_SYNC_CATCH_CMD
                || syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE)
        {
            VTY_DEBUG(VD_SYNC, "devm_com->slot == syncvty.self_slot unchange, return\n");
#ifdef VTYSH_SYNC_DEBUG
            printf("devm_com->slot == syncvty.self_slot unchange, return\n");
#endif
            return;
        }
    }
    else if(devm_com->slot == syncvty.peer_slot)
    {
        if(syncvty.peer_status == VTYSH_BATCH_CONFIG_FINISH
                || syncvty.peer_status == VTYSH_BATCH_CONFIG_SENT
                || syncvty.peer_status == VTYSH_SYNC_CATCH_CMD
                || syncvty.peer_status == VTYSH_REALTIME_SYNC_ENABLE)
        {
#ifdef VTYSH_SYNC_DEBUG
            printf("devm_com->slot == syncvty.peer_slot unchange, return\n");
#endif
            VTY_DEBUG(VD_SYNC, "devm_com->slot == syncvty.peer_slot unchange, return\n");
            return;
        }
    }

    /*只有主vty关心备vty批量同步完成的标志*/
    if(devm_com->type == syncvty.vty_slot
            || syncvty.vty_slot == VTY_SLAVE_SLOT)
    {
        syncvty.peer_status = VTYSH_BATCH_CONFIG_FINISH;
#ifdef VTYSH_SYNC_DEBUG
        printf("master vty do not care master dataok, break\n");
#endif
        VTY_DEBUG(VD_SYNC, "master vty do not care master dataok, break\n");
        return;
    }

    vtysh_master_handle_slave_dataok();
    return;
}

void vtysh_handle_bootstart(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    VTY_DEBUG(VD_SYNC, "syncvty.self_slot:%d syncvty.peer_slot:%d\n",
               syncvty.self_slot, syncvty.peer_slot);
#ifdef VTYSH_SYNC_DEBUG
    printf("syncvty.self_slot:%d syncvty.peer_slot:%d\n",
           syncvty.self_slot, syncvty.peer_slot);
#endif

#if 0

    /*只获取一次主备状态*/
    if(syncvty.self_slot != 0 || syncvty.peer_slot != 0)
    {
        printf("already get slot type %d, drop\n", syncvty.vty_slot);
        return;
    }

#endif

    /*获取主备卡槽位信息*/
//  vtysh_get_slot_type(devm_com);
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            && devm_com->type == SLOT_TYPE_BOARD_SLAVE)
    {
        /*主卡vty收到备卡start消息，更新槽位号*/
        syncvty.slave_slot_num = devm_com->slave_slot;
        syncvty.peer_slot = devm_com->slave_slot;
        VTY_DEBUG(VD_SYNC, "main vty update slave slot num syncvty.slave_slot_num:%d\n",
                   syncvty.slave_slot_num);

        printf("slave start boot, slot :%d\n", syncvty.slave_slot_num);    

    }

#if 0

    /*如果是主卡或者UNKNOW(盒式)，启动配置恢复*/
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            || syncvty.vty_slot == VTY_UNKNOW_SLOT)
    {
        recv_slot_msg_flag = 1;

        /*在线程中完成配置恢复*/
        vtysh_thread_creat(&vtysh_master_config_recover_thread);
    }

#endif
}


void vtysh_devm_msg_recv(struct ipc_mesg_n *mesg)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    struct devm_com *devm_com = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
    int ret;

    if(mesg == NULL)
    {
        return;
    }
    pmsghdr = &(mesg->msghdr);
    devm_com = (struct devm_com *)mesg->msg_data;

    VTY_DEBUG(VD_SYNC, "pmsghdr->msg_subtype:%d pmsghdr->msg_type:%d pmsghdr->opcode:%d devm_com->ha_status:%d\n",
               pmsghdr->msg_subtype, pmsghdr->msg_type, pmsghdr->opcode, devm_com->ha_status);

    printf("devmsg:%d-%d-%d:slot %d, hastatus:%d, harole %d,cardstatus %d, cardtype %d, mslot %d, sslot %d\n",
            pmsghdr->msg_subtype, pmsghdr->msg_type, pmsghdr->opcode, devm_com->slot, devm_com->ha_status, 
            devm_com->ha_role, devm_com->status, devm_com->type, devm_com->main_slot,
            devm_com->slave_slot);
    printf("sync:vtyslot %d, selfslot %d, peer %d, mslot %d, sslot %d, status %d-%d\n",
            syncvty.vty_slot, syncvty.self_slot, syncvty.peer_slot, syncvty.main_slot_num, 
            syncvty.slave_slot_num, syncvty.self_status, syncvty.peer_status);        

    if(DEV_EVENT_HA_BOOTSTATE_CHANGE != pmsghdr->msg_subtype)
    {
        return;
    }

    if(devm_com->type == SLOT_TYPE_BOARD_MAIN)
    {
        VTY_DEBUG(VD_SYNC, "msg from SLOT_TYPE_BOARD_MAIN\n");
    }
    else if(devm_com->type == SLOT_TYPE_BOARD_SLAVE)
    {
        VTY_DEBUG(VD_SYNC, "msg from SLOT_TYPE_BOARD_SLAVE\n");
    }

    ret = vtysh_get_slot_msg_type(devm_com);

    if(ret == VTY_SLOT_MSG_TYPE_BOOT)
    {
        switch(devm_com->ha_status)
        {
            case HA_BOOTSTA_START:
                vtysh_handle_bootstart(devm_com);
                break;

            case HA_BOOTSTA_END:
                vtysh_handle_bootend(devm_com);
                break;

            case HA_BOOTSTA_DATAOK:
                vtysh_handle_bootok(devm_com);
                break;

            default:
                break;
        }
    }
    else if(ret == VTY_SLOT_MSG_TYPE_EXCHANGE)
    {
        vtysh_master_slave_exchange(devm_com);
    }
    else
    {
        return;
    }
}

void vtysh_devm_msg_delete_slot(struct ipc_mesg_n *mesg)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    struct devm_com *devm_com = NULL;
//  struct ipc_msghdr *pmsghdr = NULL;

//  pmsghdr = &(mesg->msghdr);
    devm_com = (struct devm_com *)mesg->msg_data;

    /*只有主关心备卡的拔出事件*/
    if(syncvty.vty_slot != VTY_MAIN_SLOT)
    {
        return;
    }

#ifdef VTYSH_SYNC_DEBUG
    printf("devm_com->slot:%d devm_com->slave_slot:%d devm_com->main_slot:%d\n",
           devm_com->slot, devm_com->slave_slot, devm_com->main_slot);
#endif

    VTY_DEBUG(VD_SYNC, "devm_com->slot:%d devm_com->slave_slot:%d devm_com->main_slot:%d\n",
               devm_com->slot, devm_com->slave_slot, devm_com->main_slot);

    /*主卡判断是否备卡被拔出,备卡可以不关心该消息*/
    if(devm_com->slot == devm_com->slave_slot)
    {
        syncvty.peer_status = VTYSH_UNREADY;
        syncvty.peer_slot = 0;
    }
}

void vtysh_master_ack_slave_sync_req(void)
{
    if(syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE)
    {
        vtysh_master_handle_slave_dataok();
    }
}
void vtysh_sync_master_recv_msg(struct ipc_mesg_n *mesg)
{
//  struct vty_sync_msg *sync_msg;
    struct ipc_msghdr_n *msghdr = NULL;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif
    /*recv data from ha*/
    msghdr = &(mesg->msghdr);
//  sync_msg = (struct vty_sync_msg *)mesg->msg_data;

    switch(msghdr->msg_subtype)
    {
        case VTY_SYNC_REBOOT_ACK:
            syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
            printf("mast VTYSH_REALTIME_SYNC_ENABLE \n");
            break;
        case VTY_SYNC_REQ :
            vtysh_master_ack_slave_sync_req();  
            break;
        default:
            zlog_err("vtysh_recv_data_from_ha err msg_subtype:%d\n", msghdr->msg_subtype);
            break;
    }
    
}


int vtysh_sync_compelete(void)
{
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            || syncvty.vty_slot == VTY_SLAVE_SLOT)
    {
        if(syncvty.self_status == VTYSH_UNREADY
                || syncvty.self_status == VTYSH_START
                || syncvty.self_status == VTYSH_BATCH_CONFIG_START
                || syncvty.self_status == VTYSH_BATCH_CONFIG_RECV)
        {
            if(syncvty.vtysh_start_record == 1)
            {
                return 0;
            }
        }
    }
    else if(syncvty.vty_slot == VTY_UNKNOW_SLOT
            && config_recover_finish == 0)
    {
        if(syncvty.vtysh_start_record == 1)
        {
            return 0;               
        }
    }

    return 1;
}

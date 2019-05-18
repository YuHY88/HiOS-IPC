#include "vtysh_sync.h"
#include "vtysh_session.h"
#include <lib/devm_com.h>
#include <lib/module_id.h>
#include "vtysh_session.h"
#include <lib/msg_ipc.h>
#include <error.h>
#include <lib/command.h>
#include <lib/memtypes.h>
#include <lib/buffer.h>
#include <assert.h>
#include <lib/memory.h>
#include <lib/linklist.h>


/*slave vty read batch conifg file*/
int vtysh_slave_batch_config()
{
    FILE *confp = NULL;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    syncvty.all_cards_ready_flag = 1;
    reading_config_file = 1;
    confp = fopen(HA_CONFIG_BATCHSYNC_FILE , "r");

    if(confp == NULL)
    {
        zlog_err("syncfile open failed\n");
        return (-1);
    }

    vtysh_read_file(confp, NULL);
    fclose(confp);
    reading_config_file = 0;

    return (0);
}

/*slave vty start batch conifg*/
void vtysh_slave_sync_start()
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    syncvty.self_status = VTYSH_BATCH_CONFIG_RECV;
    syncvty.peer_status = VTYSH_BATCH_CONFIG_SENT;
    /*slave config from batch config file*/
    vtysh_slave_batch_config();

    /*是否在备批量同步的时候发生了主备倒换？*/
    if(syncvty.vty_slot == VTY_MAIN_SLOT)
    {
        /*此时原主已经data_ok，无需再做批量同步,可直接切换到实时同步*/
        syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
        return;
    }

    syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
    /*备不关心主是否处于处理缓存或实时状态，统一当做实时来处理*/
    syncvty.peer_status = VTYSH_REALTIME_SYNC_ENABLE;

    /*slave send dataok msg to ha*/
    vtysh_send_dataok_to_ha();

#if 0
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
        err = pthread_create(&tid, &attr, vtysh_slave_realtime_sync, NULL);
    }

    if(err != 0)
    {
        perror("read pthread_create fail:");
        exit(-1);
    }

#endif

}
#if 0
/*slave thread for recv vty and cmd msg from master*/
/*线程，备卡接收主卡sync消息的函数，针对不同消息进行不同处理*
 *VTY_SYNC_CREAT：创建vty和命令处理线程
 *VTY_SYNC_CMD：找到对应vty，将命令传入该vty
 *VTY_SYNC_EXIT：找到对应vty，改变session状态为closed*/
void *vtysh_slave_realtime_sync(void *arg)
{
    static struct ipc_pkt msg;
    struct ipc_msghdr msghdr;
    struct vty_sync_msg *sync_msg;
    struct login_session *session_loop;
    struct listnode *node, *nnode;
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);

    while(1)
    {
        for(ALL_LIST_ELEMENTS(user_session_sync_list, node, nnode, session_loop))
        {
            if(session_loop->tid_r_exit)
            {
                vtysh_slave_sync_session_delete();
            }
        }

        /*recv msg from cdp*/
//      cdp_recv();

        /*recv data from ha*/
        msghdr = msg.msghdr;
        sync_msg = (struct vty_sync_msg *)msg.msg_data;

        switch(msghdr.msg_subtype)
        {
            case VTY_SYNC_CREAT:
                vtysh_slave_sync_vty_thread_creat(sync_msg);
                break;

            case VTY_SYNC_CMD:
                vtysh_slave_recv_realtime_cmd(sync_msg);
                break;

            case VTY_SYNC_EXIT:
                vtysh_slave_sync_exit(sync_msg);
                break;

            default:
                zlog_err("vtysh_recv_data_from_ha err msg_subtype:%d\n", msghdr.msg_subtype);
                break;
        }
    }
}
#endif

struct login_session *vtysh_slave_sync_vty_thread_creat(struct vty_sync_msg *sync_msg)
{
    struct login_session *session = NULL;
    struct vty *vty;
    pthread_attr_t attr;
    int err;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    session = vty_sync_session_creat();
    vty = session->vty;

    session->session_type = SESSION_SYNC;

    vty->type = VTY_SYNC;
    vty->sub_type = sync_msg->vty_type;
    vty->node = CONFIG_NODE;
    memset(vty->ttyname, 0, SU_ADDRSTRLEN);
    strcpy(vty->ttyname, sync_msg->ttyname);
    memset(session->address, 0, SU_ADDRSTRLEN);
    strcpy(session->address, sync_msg->ttyname);
#ifdef VTYSH_SYNC_DEBUG
    printf("sync_msg->ttyname:%s vty->node:%d\n", sync_msg->ttyname, vty->node);
#endif

    VTY_DEBUG(VD_SYNC, "sync_msg->ttyname:%s vty->node:%d\n", sync_msg->ttyname, vty->node);
    /*creat cmd exec thread*/
    err = pthread_attr_init(&attr);

    if(err != 0)
    {
        perror("pthread_attr_init fail:");
        exit(-1);
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(err == 0)
    {
        err = pthread_create(&(session->tid_w), &attr, vtysh_slave_sync_exec_cmd, (void *) session);
    }

    if(err != 0)
    {
        perror("write pthread_create fail:");
        exit(-1);
    }

    pthread_attr_destroy(&attr);

    return session;
}

/*线程：备卡每个sync_vty执行主卡发过来的命令*/
void *vtysh_slave_sync_exec_cmd(void *arg)
{
    struct login_session *session = (struct login_session *)arg;
    char *cmd;
    /*each data should only handle one cmd*/
    struct buffer_data *data;
    struct buffer *buffer;
    struct vty *vty = session->vty;
    buffer = session->buf_ipc_recv;
    int return_no = 0;

    while(1)
    {
        usleep(10000);

        while(session->read_bufdata)
        {
            /*收到VTY_SYNC_EXIT消息后，vtysh_slave_sync_exit将对应的session状态置为CLOSE*
             *如果为CLOSE，退出线程*/
            if(session->session_status == SESSION_CLOSE)
            {
                /*只有在该标志为1时，才会执行session的回收*/
                session->tid_r_exit = 1;
                return (void *)0;
            }

            if(session->recv_copy_busy && \
                    session->read_bufdata == session->recv_bufdata)
            {
                continue;
            }

            data = session->read_bufdata;
            cmd = (char *)data->data;

            syncvty.cmd_slave_exec_cnt++;

            printf("sync cmd:%s\n", cmd);
            return_no = vtysh_execute_no_pager(cmd, vty);

            if(return_no == CMD_SUCCESS)
            {
                syncvty.cmd_slave_success_cnt++;
            }
            else
            {
#ifdef VTYSH_SYNC_DEBUG
                printf("slave sync cmd err:%s address:%s vty->node:%d\n", cmd, session->address, vty->node);
#endif
                zlog_err("slave sync cmd err:%s address:%s vty->node:%d\n", cmd, session->address, vty->node);
                syncvty.cmd_slave_failed_cnt++;
            }

            /*free buffer after exec cmd*/
            pthread_mutex_lock(&session->bufdata_delete_lock);

            if(data->next)
            {
                session->read_bufdata = session->read_bufdata->next;
            }
            else
            {
                session->read_bufdata = session->recv_bufdata = NULL;
            }

            buffer_data_delete(buffer, data);
            session->bufdata_cnt--;

            pthread_mutex_unlock(&session->bufdata_delete_lock);
        }

        if(session->session_status == SESSION_CLOSE)
        {
            /*只有在该标志为1时，才会执行session的回收*/
            session->tid_r_exit = 1;
            return (void *)0;
        }
    }
}

/*copy cmd to session cache after recv from sync_cmd msg*/
/*备卡收到命令，找到对应的session，将命令传入session中*/
int vtysh_slave_recv_realtime_cmd(struct vty_sync_msg *sync_msg)
{
    struct login_session *session;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif
    session = vty_sync_session_get(sync_msg);

    if(session == NULL)
    {
#ifdef VTYSH_SYNC_DEBUG
        printf("failed to find session vty_type:%d address:%s\n", sync_msg->vty_type, sync_msg->ttyname);
#endif
        VTY_DEBUG(VD_SYNC, "failed to find session vty_type:%d address:%s\n", sync_msg->vty_type, sync_msg->ttyname);
        return -1;
    }

    /* wait here if read thread lock bufdata_delete_lock */
    pthread_mutex_lock(&session->bufdata_delete_lock);
    session->recv_copy_busy = 1;
    char *buf_tmp = sync_msg->cmd;
    size_t buf_len_tmp = strlen(sync_msg->cmd);

    /*alloc bufdata until we put all the size of buf_len data*/
    session->recv_bufdata = \
                            buffer_put_new_bufdata(session->buf_ipc_recv, buf_tmp, &buf_len_tmp);

    if(!session->read_bufdata)
    {
        session->read_bufdata = session->recv_bufdata;
    }

    session->bufdata_cnt++;
    syncvty.cmd_slave_recv_cnt++;
#ifdef VTYSH_SYNC_DEBUG
    printf("recv cmd from ha. bufdata_cnt:%d cmd_slave_recv_cnt:%d\n", \
           session->bufdata_cnt, syncvty.cmd_slave_recv_cnt);
#endif
    VTY_DEBUG(VD_SYNC, "recv cmd from ha. bufdata_cnt:%d cmd_slave_recv_cnt:%d\n", \
               session->bufdata_cnt, syncvty.cmd_slave_recv_cnt);
    session->recv_copy_busy = 0;

    pthread_mutex_unlock(&session->bufdata_delete_lock);
    return 0;
}

/*收到sync_exit消息后，备卡相应的vty退出*/
int vtysh_slave_sync_exit(struct vty_sync_msg *sync_msg)
{
    struct login_session *session;
    int ret = -1;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    VTY_DEBUG(VD_SYNC, "sync_msg->ttyname:%s exit\n", sync_msg->ttyname);
    session = vty_sync_session_get(sync_msg);

    if(session == NULL)
    {
        VTY_DEBUG(VD_SYNC, "vtysh_sync_exit find session err, ttyname:%s\n", sync_msg->ttyname);
        return ret;
    }

    session->session_status = SESSION_CLOSE;
    return ret;
}

void vtysh_clear_sync_vty_reboot(void)
{
    struct login_session *session_loop;
    struct listnode *node, *nnode;
    struct vty *vty;
    int tmp = 0;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    /*close all sync session*/
    for(ALL_LIST_ELEMENTS(user_session_sync_list, node, nnode, session_loop))
    {
        vty = session_loop->vty;

        if(vty->sub_type == VTY_TERM
                || vty->sub_type == VTY_SSH)
        {
            session_loop->session_status = SESSION_CLOSE;
        }
        else
        {
            vty->node = CONFIG_NODE;
        }
    }

//    ipc_send_ha(&tmp, sizeof(tmp), 1, MODULE_ID_VTY,
//                IPC_TYPE_SYNC_DATA, VTY_SYNC_REBOOT_ACK, 0, 0); 

    VTY_DEBUG(VD_SYNC, "type IPC_TYPE_SYNC_DATA \n");  
    if(ipc_send_msg_ha_n2(&tmp, sizeof(tmp), 1, MODULE_ID_VTY, MODULE_ID_VTY, 
                                               IPC_TYPE_SYNC_DATA, VTY_SYNC_REBOOT_ACK, 0, 0,0) < 0)
    {
        zlog_err("%s:send ha msg failed \n", __FUNCTION__);
    }
}

/*在common队列收到ha数据*/
void vtysh_slave_recv_msg(struct ipc_mesg_n *mesg)
{
    struct vty_sync_msg *sync_msg;
    struct ipc_msghdr_n *msghdr = NULL;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif
    /*recv data from ha*/
    msghdr = &(mesg->msghdr);
    sync_msg = (struct vty_sync_msg *)mesg->msg_data;

    switch(msghdr->msg_subtype)
    {
        case VTY_SYNC_CREAT:
            vtysh_slave_sync_vty_thread_creat(sync_msg);
            break;

        case VTY_SYNC_CMD:
            vtysh_slave_recv_realtime_cmd(sync_msg);
            break;

        case VTY_SYNC_EXIT:
            vtysh_slave_sync_exit(sync_msg);
            break;

        case VTY_SYNC_REBOOT:
            vtysh_clear_sync_vty_reboot();
            break;

        default:
            zlog_err("vtysh_recv_data_from_ha err msg_subtype:%d\n", msghdr->msg_subtype);
            break;
    }
}

/*备卡切换到主卡*/
void vtysh_master_slave_exchange_stom(struct devm_com *devm_com)
{
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif
    unsigned int i;
    struct vty *v;

    if(syncvty.vty_slot != VTY_SLAVE_SLOT)
    {
        zlog_err("vtysh_master_slave_exchange_m not master slot!");
        return;
    }
    syncvty.ms_x_flag = 1;
    syncvty.vty_slot = VTY_MAIN_SLOT;
    syncvty.main_slot_num = devm_com->main_slot;
    syncvty.slave_slot_num = devm_com->slave_slot;
    VTY_DEBUG(VD_SYNC, "syncvty main_slot_num:%d slave_slot_num:%d self_slot:%d peer_slot:%d\n",
               syncvty.main_slot_num, syncvty.slave_slot_num, syncvty.self_slot, syncvty.peer_slot);
#ifdef VTYSH_SYNC_DEBUG
    printf("syncvty main_slot_num:%d slave_slot_num:%d self_slot:%d peer_slot:%d\n",
           syncvty.main_slot_num, syncvty.slave_slot_num, syncvty.self_slot, syncvty.peer_slot);
#endif

//  syncvty.self_slot = devm_com->main_slot;
    syncvty.peer_slot = devm_com->slave_slot;
    printf("slave -> master: status %d \n", syncvty.self_status);  
/* 防止备vty重启过*/
    if(syncvty.self_status != VTYSH_REALTIME_SYNC_ENABLE && syncvty.vtysh_start_record > 1)
    {
        syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
    }
    switch(syncvty.self_status)
    {
        case VTYSH_UNREADY:
            break;

        case VTYSH_START:
            syncvty.peer_status = VTYSH_UNREADY;
            /*未收到原主卡发来的配置文件，直接读取startup.conf*/
            /*让cpu时间，避免与主线程同时配置恢复*/
            /*配置恢复，并发送data_ok消息*/
            /*备卡配置恢复的时候，是否需要阻塞其他线程？*/
            vtysh_master_config_recover();
            break;

        case VTYSH_BATCH_CONFIG_RECV:
            /*备已收到配置文件，在消息处理函数中读取配置文件，此处无需再做特别处理*/
            break;

        case VTYSH_REALTIME_SYNC_ENABLE:
            /*do not read config file again*/
            config_recover_finish = 1;
            recv_slot_msg_flag = 1;
            /*备卡清掉所有sync_vty，切换到主卡*/
            vtysh_clear_sync_vty();
            usleep(100000);
            VTY_DEBUG(VD_SYNC, "sync_all_local_vty_to_peer\n");

            /*sync all local vty to peer*/
            for(i = 0; i < vector_active(vtyvec); i++)
            {
                if((v = vector_slot(vtyvec, i)) != NULL
                        && v->type != VTY_FILE)
                {
                    vtysh_master_sync_vty(v, VTY_SYNC_CREAT, NULL);

#if 0
                    struct listnode *node, *nnode;
                    char *cmd;

                    /*同步vty之后，将该vty缓存的实时命令同步到备卡*/
                    if(v->catch_buf != NULL
                            && v->catch_buf->head != NULL)
                        for(ALL_LIST_ELEMENTS(v->catch_buf, node, nnode, cmd))
                        {
                            VTY_DEBUG(VD_SYNC, "catch cmd:%s\n", cmd);
                            vtysh_master_sync_cmd(v, VTY_SYNC_CMD, cmd);
                        }
                    else
                    {
                        VTY_DEBUG(VD_SYNC, "vty catch empty: vty->ttyname:%s\n", v->ttyname);
                    }

#endif
                }
            }

            syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
            syncvty.peer_status = VTYSH_REALTIME_SYNC_ENABLE;
            break;

        default:
            break;
    }

}

void vtysh_clear_sync_vty()
{
    struct login_session *session_loop;
    struct listnode *node, *nnode;
    VTY_DEBUG(VD_SYNC, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
#ifdef VTYSH_SYNC_DEBUG
    printf("%s[%d]%s\n", __FILE__, __LINE__, __func__);
#endif

    /*close all sync session*/
    for(ALL_LIST_ELEMENTS(user_session_sync_list, node, nnode, session_loop))
    {
        session_loop->session_status = SESSION_CLOSE;
    }

#if 0

    /*wait until all vty close*/
    while(user_session_sync_list->count)
    {
        usleep(10000);
    }

#endif
}

void vtysh_check_sync_vty_close()
{
    struct login_session *session_loop;
    struct listnode *node, *nnode;

    for(ALL_LIST_ELEMENTS(user_session_sync_list, node, nnode, session_loop))
    {
        if(session_loop->tid_r_exit)
        {
            vtysh_slave_sync_session_delete(session_loop);
        }
    }
}

void vtysh_slave_req_sync_vty(void)
{
    int tmp = 0;
    if(ipc_send_msg_ha_n2(&tmp, sizeof(tmp), 1, MODULE_ID_VTY, MODULE_ID_VTY, 
                                               IPC_TYPE_SYNC_DATA, VTY_SYNC_REQ, 0, 0,0) < 0)
    {
        zlog_err("%s:send ha msg VTY_SYNC_REQ failed \n", __FUNCTION__);
    }                
}

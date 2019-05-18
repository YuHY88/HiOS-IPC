#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <net/if.h>// struct ifreq  
#include <sys/ioctl.h> // ioctl、SIOCGIFADDR  
#include <sys/socket.h> // socket  
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/prctl.h>
#include <netpacket/packet.h> // struct sockaddr_ll 
#include <lib/errcode.h>
#include <lib/inet_ip.h>
#include "lib/memory.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include <lib/log.h>
#include <ftm/pkt_eth.h>
#include "devm_ha.h"
#include "devm.h"

/* eth1 sock 通信 */
struct sockaddr_in eth1_myboard_serveraddr;
struct sockaddr_in eth1_otherboard_serveraddr;
struct ipc_mesg_n *sock_recv_pkt = NULL;  /* 用于接收 ha 报文的缓存 */
unsigned char gNeighberSlot = 0 , gLocalSlot  = 0;


struct sockaddr_in eth1_myslot_serveraddr;
struct sockaddr_in eth1_send_serveraddr;

/*mainboard status*/
enum HA_BOOTSTATE myboard_dev_start_status = HA_BOOTSTA_START;
enum HA_BOOTSTATE otherboard_dev_start_status = HA_BOOTSTA_START;
enum HA_ROLE ha_dev_ha_role = HA_ROLE_INIT;

/* eth1 sock 通信 */
int eth1_sock = -1;         /* 主备板之间 HA 通信的 socket */
int devmhainitok = 0;
/*
pthread_mutex_t ha_mutex;   // HA 通道发包互斥信号量  
*/

int ha_heartbeat_count = 0;        /* 心跳报文计数 */
struct thread *ha_heartbeat_timer = NULL; /* 心跳报文定时器 */

extern int devmGetEepFlag;
extern struct devm device;
extern unsigned char a_ha_bootstate[][25];
extern unsigned char a_ha_dev_ha_role[][25];

unsigned int devm_ha_app_init_count[HA_MODULE_ID_MAX] = {0};

extern unsigned char myslot;
extern unsigned char partnerslot;

/* 主控板和备用主控互相发送心跳报文 */
void ha_send_heartbeat(int slot)
{
    struct ha_btb_hbmsg t_ha_btb_hbmsg;
    struct ipc_msghdr_n msghdr;

    memset(&t_ha_btb_hbmsg, 0, sizeof(struct ha_btb_hbmsg));
    t_ha_btb_hbmsg.boot = myboard_dev_start_status;
    t_ha_btb_hbmsg.role = ha_dev_ha_role;

    memset(&msghdr, 0, sizeof(struct ipc_msghdr_n));
    msghdr.module_id = MODULE_ID_HA;
    msghdr.sender_id = MODULE_ID_HA;
    msghdr.data_len = sizeof(struct ha_btb_hbmsg);
    msghdr.msg_type = IPC_TYPE_DEVM;
    msghdr.msg_subtype = DEVM_INFO_HEART_BEAT;
    msghdr.opcode = IPC_OPCODE_UPDATE;
    msghdr.data_num = 1;
    msghdr.unit = device.myunit;
    msghdr.slot = device.myslot;

//    ha_send_pkt(&t_ha_btb_hbmsg, &msghdr);        /* need modify---20181011*/
    ha_heartbeat_count++;

    return;
}
#if 0
void ha_send_heartbeat(int slot)
{
    static struct ha_ipc_mesg t_ha_mesg;
    struct ha_btb_hbmsg t_ha_btb_hbmsg;
    struct ipc_msghdr *pmsghdr = NULL;

    memset(&t_ha_mesg, 0, sizeof(struct ha_ipc_mesg));

    t_ha_mesg.target_slot = partnerslot;
    t_ha_mesg.target_queue_id = 0;

    memset(&t_ha_btb_hbmsg, 0, sizeof(struct ha_btb_hbmsg));
    t_ha_btb_hbmsg.boot = myboard_dev_start_status;
    t_ha_btb_hbmsg.role = ha_dev_ha_role;
    memcpy(t_ha_mesg.ipc_mesg_data.msg_data, &t_ha_btb_hbmsg, sizeof(struct ha_btb_hbmsg));

    pmsghdr = &t_ha_mesg.ipc_mesg_data.msghdr;
    pmsghdr->module_id = MODULE_ID_HA;
    pmsghdr->sender_id = MODULE_ID_HA;
    pmsghdr->data_len = sizeof(struct ha_btb_hbmsg);
    pmsghdr->msg_type = IPC_TYPE_DEVM;
    pmsghdr->msg_subtype = DEVM_INFO_HEART_BEAT;
    pmsghdr->opcode = IPC_OPCODE_UPDATE;
    pmsghdr->data_num = 1;
    pmsghdr->unit = device.myunit;
    pmsghdr->slot = device.myslot;

    if (0 != t_ha_mesg.target_slot)
    {
        ha_send_pkt(&t_ha_mesg);
        ha_heartbeat_count++;
    }

    return;
}
#endif

/* 主控板启动心跳报文定时器 */
int ha_start_heartbeat_timer(struct thread *thread)
{
    int slot = 0;

    if (device.pmyunit && device.pmyunit->myslot == device.pmyunit->slot_main_board)
    {
        slot = device.pmyunit->myslot;
        ha_send_heartbeat(slot);
    }

    if (ha_heartbeat_count >= 3) /* 心跳超时处理 */
    {
#if 0    
        ha_heartbeat_count = 0;
        if(0 != device.pmyunit->slot_slave_board)
        {
            devm_delete_slot(device.myunit, device.pmyunit->slot_slave_board);
        }
#endif        
    }

//    ha_heartbeat_timer = thread_add_timer(devm_master, ha_start_heartbeat_timer, (void *)slot, 15);
    return ERRNO_SUCCESS;
}


/* 从 HA IPC 接收消息*/
#if 0
//static int ipc_recv_ha(struct ipc_pkt *pmsg)
static int ipc_recv_ha(struct ipc_mesg_n *pmsg)
{
//  int ret = -1;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    return msgrcv(ipc_ha_id, (void *)pmsg, IPC_MSG_LEN1 + IPC_HEADER_LEN - 4, 0, 0);
}
#endif

/* 处理 HA 通道收到的 devm 消息 */
#if 0
int ha_handle_ha_msg(struct ipc_mesg_n *pmsg)
{
//  int ret = 0;
    struct ipc_msghdr_n *phdr = NULL;
    void *data = NULL;

    phdr = &pmsg->msghdr;
    data = pmsg->msg_data;

    /* 处理板卡/端口创建消息 */
    if (phdr->opcode == IPC_OPCODE_ADD)
    {

    }
    else if (phdr->opcode == IPC_OPCODE_DELETE)
    {

    }
    else if (phdr->opcode == IPC_OPCODE_UPDATE)
    {
        if (phdr->msg_subtype == DEVM_INFO_HEART_BEAT)  /* 收到心跳报文 */
        {
            //      struct ha_btb_hbmsg *p_ha_btb_hbmsg = (struct ha_btb_hbmsg *)data;
            /*心跳报文处理，待添加*/

            ha_heartbeat_count = 0;   /* 心跳计数清 0  */

            if (device.pmyunit && device.pmyunit->myslot == device.pmyunit->slot_slave_board)
            {
                ha_send_heartbeat(0);      /* 回心跳报文 */
            }
        }
    }
    else if (phdr->opcode == IPC_OPCODE_EVENT)
    {
        if (phdr->msg_subtype == HA_SMSG_BOOT_STATE)  /* 收到HA转换 */
        {
            struct devm_com recv_devm_com;
            enum DEV_EVENT t_dev_event = DEV_EVENT_INVALID;

            memcpy(&recv_devm_com, (struct devm_com *)data, sizeof(struct devm_com));

            otherboard_dev_start_status = recv_devm_com.ha_status;

            /*send HA_BOOTSTATE change msg */
            t_dev_event = DEV_EVENT_HA_BOOTSTATE_CHANGE ;

            zlog_debug(DEVM_DBG_HA, "%s[%d],func %s:recv DEV_EVENT_HA_BOOTSTATE_CHANGE msg from otherboard, slot = %d, main slot = %d, slave slot = %d\n", __FILE__, __LINE__, __func__, recv_devm_com.slot, recv_devm_com.main_slot, recv_devm_com.slave_slot);
            devm_event_notify(t_dev_event, &recv_devm_com);
        }
    }

    return 0;
}
#endif
#if 0

void select_ip_by_slot(unsigned char slot)
{
    unsigned int source_ip[32] = {0x1000000,0x1000001,0x1000002,0x0,0x0,0x0,0x0,0x0,0x1000808,
                                                  0x1000909,0x0,0x0,0x0,0x0,0x0,0x0,0x1000010,0x1000011};
    switch(slot)
    {
        /*slot 1 and slot 2 for HT2200----slot8/9/16/17 for VX*/
        case 0:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot] | gNeighberslot);
            break;
        case 1:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot]);
            break;
        case 2:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot]);
            break;           
        case 8:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot]);
            break;
        case 9:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot]);
            break;
        case 16:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot]);
            break;
        case 17:
            eth1_send_serveraddr.sin_addr.s_addr = htonl(source_ip[slot]);
            break;
        default:
            break;
    }
}

/* 向 HA 通道写入报文 */
int ha_send_pkt(void *pdata, struct ipc_msghdr_n *phdr)
{
    static struct ipc_pkt t_ipc_pkt;
    int ret, size;

    if ((phdr == NULL) || (phdr->data_len > IPC_MSG_LEN1))
    {
        zlog_err("ha_send_pkt error\n");
        return -1;
    }

    if (eth1_sock < 0)
    {
        return -1;
    }

    /*保护临界资源 msg */
    pthread_mutex_lock(&ha_mutex);

    size = phdr->data_len;
    memcpy(&t_ipc_pkt.msghdr, phdr, sizeof(struct ipc_msghdr_n));

    if (pdata)
    {
        memcpy(t_ipc_pkt.msg_data, pdata, size);
    }

    ret = sendto(eth1_sock, &t_ipc_pkt, sizeof(struct ipc_msghdr_n) + phdr->data_len, 0, (struct sockaddr *)&eth1_otherboard_serveraddr, sizeof(eth1_otherboard_serveraddr));

    if (ret < 0)
    {
        perror("sendto");
        zlog_err("failed to send to eth1 socket, ret = %d! datalen:0x%x\n", ret, size + IPC_HEADER_LEN);
        printf("failed to send to eth1 socket, ret = %d! datalen:0x%x\n", ret, size + IPC_HEADER_LEN);
        pthread_mutex_unlock(&ha_mutex);
        return -1;
    }
    else
    {
        zlog_notice("%s[%d]:leave %s: sucessfully send msg byha,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x, ret = %d\n", __FILE__, __LINE__, __func__, phdr->sender_id, phdr->module_id
                    , phdr->msg_type, phdr->msg_subtype, phdr->opcode, phdr->msg_index, ret);
    }

    pthread_mutex_unlock(&ha_mutex);
    return 0;
}
#endif

/* 从 HA 通道接收报文 */
#if 0
int ha_recv_pkt(struct ipc_mesg_n *pmsg)
{
    struct ipc_msghdr_n *phdr = NULL;
    void *pdata = NULL;
    int ret = ERRNO_FAIL;

    phdr = &pmsg->msghdr;
    pdata = pmsg->msg_data;

    switch (pmsg->msghdr.sender_id)
    {
        case MODULE_ID_HA:
            ha_handle_ha_msg(pmsg);
            break;

//      case MODULE_ID_DEVM:
//          devm_msg_rcv_ha ( &pmsg->msghdr, pmsg->msg_data );
//          break;
        case MODULE_ID_HAL:
            ret = ipc_send_msg_n2(pmsg->msg_data, pmsg->msghdr.data_len, pmsg->msghdr.data_num, pmsg->msghdr.sender_id, MODULE_ID_HA
                               , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);

            if (ret != 0)
            {
                zlog_err("%s[%d]:leave %s: fail to send ha msg to hal,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x \n", __FILE__, __LINE__, __func__, pmsg->msghdr.sender_id, pmsg->msghdr.module_id
                         , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);
            }
            else
            {
                zlog_notice("%s[%d]:leave %s: sucessfully send ha msg to hal,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x \n", __FILE__, __LINE__, __func__, pmsg->msghdr.sender_id, pmsg->msghdr.module_id
                            , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);
            }

            break;

        case MODULE_ID_FILE:
            ret = ipc_send_msg_n2(pmsg->msg_data, pmsg->msghdr.data_len, pmsg->msghdr.data_num, pmsg->msghdr.sender_id, MODULE_ID_HA
                               , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);

            if (ret != 0)
            {
                zlog_err("%s[%d]:leave %s: fail to send ha msg to filem,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x \n", __FILE__, __LINE__, __func__, pmsg->msghdr.sender_id, pmsg->msghdr.module_id
                         , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);
            }
            else
            {
                zlog_notice("%s[%d]:leave %s: sucessfully send ha msg to filem,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x \n", __FILE__, __LINE__, __func__, pmsg->msghdr.sender_id, pmsg->msghdr.module_id
                            , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);
            }

            break;

        default:
            ret = ipc_send_msg_n2(pmsg->msg_data, pmsg->msghdr.data_len, pmsg->msghdr.data_num, pmsg->msghdr.sender_id, MODULE_ID_HA
                                   , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);

            if (ret != 0)
            {
                zlog_err("%s[%d]:leave %s: fail to send ha msg to common,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x \n", __FILE__, __LINE__, __func__, pmsg->msghdr.sender_id, pmsg->msghdr.module_id
                         , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);
            }
            else
            {
                zlog_notice("%s[%d]:leave %s: sucessfully send ha msg to common,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x \n", __FILE__, __LINE__, __func__, pmsg->msghdr.sender_id, pmsg->msghdr.module_id
                            , pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype, pmsg->msghdr.opcode, pmsg->msghdr.msg_index);
            }

            break;
    }

    return 0;
}
#endif

static void ha_exit()
{
    /* 关闭 socket */
    eth1_sock = -1;
}

#if 0
static void ha_ipc_exit()
{
    /* 关闭 ha ipc */
    ipc_ha_id = -1;
}

static void ha_ipc_thread(void *arg)
{
    int   ret = ERRNO_FAIL;
    int token = 30;
    struct sched_param param;
//    static struct ipc_pkt t_ipc_pkt;  /* 用于接收 ha 报文的缓存 */
    static struct ipc_mesg_n pmesg;  /* 用于接收 ha 报文的缓存 */    
//    struct ipc_msghdr *phdr = NULL;
    struct ipc_msghdr_n *phdr = NULL;
    void *pdata = NULL;

#ifndef HAVE_KERNEL_3_0
    param.sched_priority = 60;

    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        zlog_err("%s, %d priority set:%s\n", __FUNCTION__, __LINE__, strerror(errno));
    }
#endif

    prctl(PR_SET_NAME, "ha ipc pthread");

    while (1)
    {
        if (device.pbox)  /* 判断是盒式设备，则退出 HA 线程 */
        {
            goto out;
        }

        /* 接收 HA IPC */
        memset(&pmesg, 0, sizeof(struct ipc_mesg_n));
        ret = ipc_recv_ha(&pmesg);

        if ((ret == -1))
        {
            usleep(10000);    /* 没有收到报文则让出 CPU */
            continue;
        }
        else
        {
            /* process the ipc message */
            phdr = &(pmesg.msghdr);
            pdata = pmesg.msg_data;

            ret = ha_send_pkt(pdata, phdr);/* 转发给备主控 */
        }
        mem_share_free(&pmesg, pmesg.msghdr.sender_id);         /*must modify,free mem from MODULE*/

        /* 每接收 30 个报文让一次 CPU */
        token--;

        if (token == 0)
        {
            usleep(10000);
            token = 30;
        }
    }

out:
    ha_ipc_exit();
    return;
}
#endif

static void ha_netbyteorder(struct ipc_msghdr_n *pMsgHd)
{
    pMsgHd->module_id   = htonl(pMsgHd->module_id);
    pMsgHd->sender_id   = htonl(pMsgHd->sender_id);
    pMsgHd->data_len    = htonl(pMsgHd->data_len);
    pMsgHd->msg_type    = htonl(pMsgHd->msg_type);
    pMsgHd->msg_subtype = htons(pMsgHd->msg_subtype);
    pMsgHd->data_num    = htons(pMsgHd->data_num);

    pMsgHd->spare       = htons(pMsgHd->spare);
    pMsgHd->sequence    = htonl(pMsgHd->sequence);
    pMsgHd->msg_index   = htonl(pMsgHd->msg_index);
    pMsgHd->offset      = htonl(pMsgHd->offset);
    pMsgHd->msgflag     = htonl(pMsgHd->msgflag);
    pMsgHd->result      = htonl(pMsgHd->result);
}

static void ha_hostbyteorder(struct ipc_msghdr_n *pMsgHd)
{
    pMsgHd->module_id   = ntohl(pMsgHd->module_id);
    pMsgHd->sender_id   = ntohl(pMsgHd->sender_id);
    pMsgHd->data_len    = ntohl(pMsgHd->data_len);
    pMsgHd->msg_type    = ntohl(pMsgHd->msg_type);
    pMsgHd->msg_subtype = ntohs(pMsgHd->msg_subtype);
    pMsgHd->data_num    = ntohs(pMsgHd->data_num);

    pMsgHd->spare       = ntohs(pMsgHd->spare);
    pMsgHd->sequence    = ntohl(pMsgHd->sequence);
    pMsgHd->msg_index   = ntohl(pMsgHd->msg_index);
    pMsgHd->offset      = ntohl(pMsgHd->offset);
    pMsgHd->msgflag     = ntohl(pMsgHd->msgflag);
    pMsgHd->result      = ntohl(pMsgHd->result);
}
int ha_thread ( struct thread *thread )
{
  //int sock;
    struct ipc_mesg_n *recv = NULL, *pmesg = NULL;  /* 用于接收 ha 报文的缓存 */
    int len = 0, ret = 0;
    socklen_t size = 0;
    struct sockaddr_in  clientaddr;
    
    size = sizeof(struct sockaddr_in);
#if 0
    if (device.pbox)  /* 判断是盒式设备，则退出 HA 线程 */
    {
        goto out;
    }
#endif
    recv = sock_recv_pkt;
//    sock_recv_pkt = mem_share_malloc(sizeof(struct ipc_msghdr_n) + IPC_MSG_LEN_N, MODULE_ID_HA);
    /* 从 eth1 接口接收报文 */
    len = recvfrom(eth1_sock, recv, (sizeof(struct ipc_msghdr_n) + IPC_MSG_LEN_N), 0, (struct sockaddr *)&clientaddr, &size);
    if (len < 0)
    {
        perror("recvfrom");
    }

    if (0 != memcmp(&clientaddr, &eth1_otherboard_serveraddr, sizeof(struct sockaddr_in)))
    {
        goto next;
    }

    if (len >= IPC_HEADER_LEN_N)
    {
//        printf("###errno=%d####len=%d###recv->msghdr.sender_id=%d####recv->msghdr.msg_subtype=%d##########rrecv->msghdr.msg_type=%d#########recv->msghdr.data_len=%d#################\n",errno,len,recv->msghdr.sender_id,recv->msghdr.msg_subtype,recv->msghdr.msg_type,recv->msghdr.data_len);
        pmesg = mem_share_malloc(len, MODULE_ID_HA);

        if(pmesg)
        {
            memcpy(pmesg, recv, len);
            
            ha_hostbyteorder(&pmesg->msghdr);
            
            ret = ipc_send_msg_n1(pmesg, len);
            
            if(ret != 0) 
            {
                mem_share_free(pmesg, MODULE_ID_HA);
            }
        }
    }
    
next:    
    thread_add_read ( devm_master, ha_thread, NULL, eth1_sock );
    
    return 0;

}

#if 0
/* ha 报文接收线程 */
static void ha_thread(void *arg)
{
    struct sched_param param;
//    static struct ipc_pkt t_ipc_pkt;  /* 用于接收 ha 报文的缓存 */
    static struct ipc_mesg_n t_ipc_pkt;  /* 用于接收 ha 报文的缓存 */
    int token = 30;
    int len = 0, ret = 0;
    socklen_t size = 0;
    struct sockaddr_in  clientaddr;

#ifndef HAVE_KERNEL_3_0
    param.sched_priority = 60;

    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        zlog_err("%s, %d priority set:%s\n", __FUNCTION__, __LINE__, strerror(errno));
    }
#endif

    prctl(PR_SET_NAME, "ha pthread");

//    size = sizeof(struct ha_msg_t);
    size = sizeof(struct sockaddr_in);

    while (1)
    {
        if (device.pbox)  /* 判断是盒式设备，则退出 HA 线程 */
        {
            goto out;
        }

        /* 从 eth1 接口接收报文 */
        len = recvfrom(eth1_sock, &t_ipc_pkt, sizeof(t_ipc_pkt), 0, (struct sockaddr *)&clientaddr, &size);

        if (len < 0)
        {
            perror("recvfrom");
        }

        if (0 != memcmp(&clientaddr, &eth1_otherboard_serveraddr, sizeof(struct sockaddr_in)))
        {
            continue;
        }

        if (len >= IPC_HEADER_LEN)
        {
            ret = ipc_send_msg_n2(t_ipc_pkt.msg_data, t_ipc_pkt.msghdr.data_len, t_ipc_pkt.msghdr.data_num, t_ipc_pkt.msghdr.sender_id, MODULE_ID_HA
                               , t_ipc_pkt.msghdr.msg_type, t_ipc_pkt.msghdr.msg_subtype, t_ipc_pkt.msghdr.opcode, t_ipc_pkt.msghdr.msg_index);
//            ha_recv_pkt(&t_ipc_pkt);  /* 处理接收的 HA 通道报文 */
        }

        /* 每接收 30 个报文让一次 CPU */
        token--;

        if (token == 0)
        {
            usleep(10000);
            token = 30;
        }
    }

out:
    ha_exit();
    return;
}
#endif

static unsigned int devm_ha_IPtoInt(char *str_ip)
{
    struct in_addr  addr;
    uint32_t  int_ip = 0;

    if (inet_aton(str_ip, &addr))
    {
        int_ip = ntohl(addr.s_addr);
    }

    return int_ip;
}

/* receive ha IPC messages */
int ha_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int retva = 0;

    if(NULL == pmsg) return 0;
    
//revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 
//  if(revln <= imlen)
    {
        /*send pkt through sock*/
//        select_ip_by_slot(pmsg->msghdr.slot);

        if(pmsg->msghdr.slot == 0)
        {
            pmsg->msghdr.slot =  gNeighberSlot ;
            pmsg->msghdr.unit = 1;
        }

        if(pmsg->msghdr.slot != 0)
        {
            pmsg->msghdr.srcslot = gLocalSlot;
            pmsg->msghdr.srcunit = 1;

            ha_netbyteorder(&pmsg->msghdr);
            
            retva = sendto(eth1_sock, pmsg, imlen, 0, (struct sockaddr *)&eth1_otherboard_serveraddr, sizeof(eth1_otherboard_serveraddr));

            if (retva < 0)
            {
                perror("sendto");
                zlog_err("failed to send to eth1 socket, ret = %d! datalen:0x%x\n", retva, imlen);
                printf("failed to send to eth1 socket, ret = %d! datalen:0x%x\n", retva, imlen);
            }
            else
            {
                zlog_debug(DEVM_DBG_HA, "%s[%d]:leave %s: sucessfully send msg byha,sender id = %d, moduleid = %d,msg type = %d, subtype = %d, opcode = %d, index = 0x%x, ret = %d\n", 
                           __FILE__, __LINE__, __func__, ntohl(pmsg->msghdr.sender_id), ntohl(pmsg->msghdr.module_id),
                           ntohl(pmsg->msghdr.msg_type), ntohs(pmsg->msghdr.msg_subtype), pmsg->msghdr.opcode, ntohl(pmsg->msghdr.msg_index), retva);
            }
        }
    }

    mem_share_free(pmsg, MODULE_ID_HA);    
    return retva;
}


/* 主控板收到备用主控板插入事件，则调此函数创建 ha 线程 */
int ha_init(unsigned int slot)
{
   // int ret = 0;
   // unsigned int otherboard_slot = 0;
  //  pthread_t ha_id;
  //  pthread_attr_t attr_ha;
 //   pthread_attr_t attr_ha_ipc;
   // int rv = 0;
    char eth1_ip[16] = {0};
//  enum HA_BOOTSTATE ha_bootstate = HA_BOOTSTA_START;

    /* 已经初始化过了 */
    if(devmhainitok) return ERRNO_SUCCESS;
    
    devmhainitok = 1;
    
    gLocalSlot = (slot >> 8) & 0xff;
    gNeighberSlot = slot & 0xff;

    if(((slot >> 16) & 0xff) != 0) 
    {
        /* 创建 raw socket */
        eth1_sock = socket(AF_INET, SOCK_DGRAM, 0);

        if (eth1_sock < 0)
        {
            zlog_err("%s, %d socket connect error:%s\n", __FUNCTION__, __LINE__, strerror(errno));
            return ERRNO_FAIL;
        }

        myboard_dev_start_status = HA_BOOTSTA_START;
        otherboard_dev_start_status = HA_BOOTSTA_START;
        ha_dev_ha_role = HA_ROLE_INIT;

        printf("%s[%d]: gLocalSlot = %d, gNeighberSlot = %d\n",__func__,__LINE__,gLocalSlot,gNeighberSlot);
        /* 将 raw socket 与 eth1 绑定 */
        memset(eth1_ip, 0, sizeof(eth1_ip));
        sprintf(eth1_ip, "%s%d", ETH1_IP, (int)gLocalSlot);
        

        
        memset(&eth1_myboard_serveraddr, 0, sizeof(struct sockaddr_in));
        eth1_myboard_serveraddr.sin_family = AF_INET;
        inet_pton(AF_INET, eth1_ip, &eth1_myboard_serveraddr.sin_addr.s_addr);
        eth1_myboard_serveraddr.sin_port = htons(9877);


        memset(eth1_ip, 0, sizeof(eth1_ip));
    // otherboard_slot = (1 == gLocalSlot) ? 2 : 1;
        sprintf(eth1_ip, "%s%d", ETH1_IP, (int)gNeighberSlot);

        memset(&eth1_otherboard_serveraddr, 0, sizeof(struct sockaddr_in));
        eth1_otherboard_serveraddr.sin_family = AF_INET;

    //  eth1_otherboard_serveraddr.sin_addr.s_addr = inet_addr(eth1_ip);
        eth1_otherboard_serveraddr.sin_addr.s_addr = devm_ha_IPtoInt(eth1_ip);

        eth1_otherboard_serveraddr.sin_port = htons(9877);


        if (bind(eth1_sock, (struct sockaddr *)&eth1_myboard_serveraddr, sizeof(eth1_myboard_serveraddr)) < 0)
        {
            perror("bind");
            zlog_err("%s[%d]:leave %s:error:fail to bind eth1 sock,error = %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        }
        sock_recv_pkt = XMALLOC(MODULE_ID_HA, sizeof(struct ipc_msghdr_n) + IPC_MSG_LEN_N);

        thread_add_read ( devm_master, ha_thread, NULL, eth1_sock );
    }

    if(ipc_recv_thread_start("HaIpcRev", MODULE_ID_HA, SCHED_OTHER, -1, ha_msg_rcv, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }

#if 0
    /* 初始化一个互斥锁，用于 socket 发包 */
    rv = pthread_mutex_init(&ha_mutex, NULL);

    if (rv != 0)
    {
        perror("Mutex initialization failed");
        zlog_err("%s[%d]:leave %s:error:fail to init ha_mutex,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, rv, strerror(errno));
    }

    /* 创建 ha 线程读 socket */
    ret = pthread_attr_init(&attr_ha);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:error:fail to init attr_ha,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
    }
    else
    {
        ret = pthread_attr_setdetachstate(&attr_ha, PTHREAD_CREATE_DETACHED);

        if (0 == ret)
        {
            ret = pthread_create(&ha_id, &attr_ha, (void *)ha_thread, NULL);

            if (0 != ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to create pthread ha,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
            }
        }

        ret = pthread_attr_destroy(&attr_ha);

        if (0 != ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to destory attr_ha,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
        }
    }

    ret = pthread_attr_init(&attr_ha_ipc);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:error:fail to init attr_ha_ipc,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
    }
    else
    {
        ret = pthread_attr_setdetachstate(&attr_ha_ipc, PTHREAD_CREATE_DETACHED);

        if (0 == ret)
        {
            ret = pthread_create(&ha_id, &attr_ha_ipc, (void *)ha_ipc_thread, NULL);

            if (0 != ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to create pthread ha_ipc,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
            }
        }

        ret = pthread_attr_destroy(&attr_ha_ipc);

        if (0 != ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to destory attr_ha_ipc,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
        }
    }
#endif

    return ERRNO_SUCCESS;
}



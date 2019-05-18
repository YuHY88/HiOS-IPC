#include <zebra.h>
#include "vty.h"
#include <pthread.h>
#include <lib/module_id.h>
#include "command.h"
#include "buffer.h"
#include "msg_ipc_n.h"
#include <arpa/telnet.h>
#include "vtysh.h"
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/devm_com.h>
#include <lib/sdh_vcg_common.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_tcp.h>
#include "hptimer.h"
#include "vtysh_telnet.h"
#include "thread.h"
#include "ifm_common.h"
#include "module_id.h"
#include "errcode.h"
#include "vtysh_msg.h"
#include "vtysh_session.h"
#include "sshd/vtysh_sshd.h"
#include "vtysh_auth.h"
#include "vtysh_sync.h"
#include "vtysh_telnet_client.h"
#include "vtysh_snmp.h"

//extern struct thread_master *vtysh_master;
extern struct list *user_session_list;
int netconf_ipc_id;

extern int gVtyLocalUserLogin;


struct list ifm_event;
pthread_mutex_t ifm_event_lock;

/*ע��Ҫ����Э�鱨�ĵ�����*/
static void
vty_pkt_register(int port)
{
    union proto_reg proto;
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = port;

    pkt_register(MODULE_ID_VTY, PROTO_TYPE_IPV4, &proto);

    if(port == TCP_PORT_TELNET)
    {
        memset(&proto, 0, sizeof(union proto_reg));
        proto.ipreg.protocol = IP_P_TCP;
        proto.ipreg.sport = port;
        pkt_register(MODULE_ID_VTY, PROTO_TYPE_IPV4, &proto);
    }
    return;
}

void
vty_msg_init()
{
    pthread_t thread_id;

    netconf_ipc_id = -1;

    /*init vtysh ipc connect*/
//    ipc_connect_common();

    /*init pkt ipc for telnet*/
    vty_pkt_register(TCP_PORT_TELNET);

    /*init pkt ipc for ssh*/
    vty_pkt_register(TCP_PORT_SSH);

    /*init pkt ipc for netconf*/
    vty_pkt_register(830);

    memset(&ifm_event, 0, sizeof(struct list));

    pthread_mutex_init(&ifm_event_lock, NULL);
    pthread_create(&thread_id, NULL, ifm_event_config, NULL);

    /*regist interface add event*/
    ifm_event_register(IFNET_EVENT_IF_ADD, MODULE_ID_VTY, IFNET_IFTYPE_PORT);
    ifm_event_register(IFNET_EVENT_IF_ADD, MODULE_ID_VTY, IFNET_IFTYPE_CES);

    pthread_mutex_lock(&session_timer_lock);
    /*check user idle time, and send to aaa*/
//    thread_add_timer(vtysh_master, vtysh_check_idle_user, NULL, CHECK_IDLE_TIME);
    high_pre_timer_add("vtyshIdleTimer", LIB_TIMER_TYPE_LOOP, vtysh_check_idle_user, NULL, 5000);
    pthread_mutex_unlock(&session_timer_lock);

}

#if 0
static void vty_pkt_unregister(struct login_session *session)
{
    union proto_reg proto;
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_TELNET;
    proto.ipreg.dip = session->server_ip;
    proto.ipreg.sport = session->client_port;

    pkt_unregister(MODULE_ID_VTY, PROTO_TYPE_IPV4, &proto);

    return;
}
#endif

int vty_tcp_close(uint32_t dip, uint16_t dport, uint16_t sport)
{
    struct ip_proto tcp_info;

    //printf("%s from %d to (%#x:%d) \n\n", __FUNCTION__, sport, dip, dport);
    tcp_info.dip = dip;
    tcp_info.dport = dport;
    tcp_info.sport = sport;
    tcp_info.protocol = IP_P_TCP;
    tcp_info.sip = 0;//0xc0c004d7;

    return pkt_close_tcp(MODULE_ID_VTY, &tcp_info);
}

int vty_tcp_open(uint32_t dip, uint16_t dport, uint16_t sport)
{
    struct ip_proto tcp_info;

    tcp_info.dip = dip;
    tcp_info.dport = dport;
    tcp_info.sport = sport;
    tcp_info.protocol = IP_P_TCP;
    tcp_info.sip = 0;//0xc0c004d7;

    return pkt_open_tcp(MODULE_ID_VTY, &tcp_info);
}

void *ifm_event_config(void *arg)
{
//    struct ipc_msghdr *phdr      = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;
    struct ipc_mesg_n   *pmesg_new = NULL;
    void *pdata = NULL;
    struct ipc_mesg_n mesg;

    prctl(PR_SET_NAME, " ifm_event_config");

    while(1)
    {
        if(list_isempty(&ifm_event))
        {
            usleep(100000);
            continue;
        }

        pthread_mutex_lock(&ifm_event_lock);

        for(ALL_LIST_ELEMENTS(&ifm_event, pnode, pnextnode, pmesg_new))
        {
            if(NULL == pmesg_new)
            {
                continue;
            }

            memcpy(&mesg, pmesg_new, sizeof(struct ipc_mesg_n));
            list_delete_node(&ifm_event, pnode);
            free(pmesg_new);
            break;
        }

        pthread_mutex_unlock(&ifm_event_lock);

        pdata = mesg.msg_data;

        vtysh_reconfig_ifm(pdata);
        memset(&mesg, 0, sizeof(struct ipc_mesg_n));
    }
}

int
vtysh_reconfig_ifm(void *pdata)
{
    uint32_t ifindex;
    struct ifm_event   *pevent   = NULL;
    char ifname[IFNET_NAMESIZE];
    char ifname_tmp[IFNET_NAMESIZE];
    FILE *confp = NULL;
    int8_t ret = -1;

    pevent = (struct ifm_event *)pdata;
    ifindex = pevent->ifindex;
    zlog_debug(VD_MSG, "re-config interface\n");

    ifm_get_name_by_ifindex(ifindex, ifname_tmp);

	if(!IFM_TYPE_IS_METHERNET(ifindex) && !IFM_TYPE_IS_TDM(ifindex) && !IFM_TYPE_IS_STM(ifindex))
    {
        return 1;
    }

    if(IFM_TYPE_IS_METHERNET(ifindex) && _SHR_PORT_IS_SUBPORT(ifindex))
    {
        return 1;
    }

    sprintf(ifname, "interface %s", ifname_tmp);
    zlog_debug(VD_MSG, "ifname size:%d str:%s\n", strlen(ifname), ifname);

    confp = fopen(VTYSH_STARTUP_CONFIG, "r");

    if(confp == NULL)
    {
        printf("startup.conf not exist\n");
        confp = fopen(VTYSH_INIT_CONFIG, "r");

        if(confp == NULL)
        {
            return (1);
        }
    }

    vtysh_read_file(confp, ifname);

//    ipc_send_common1(NULL, 0, 1, MODULE_ID_L2, MODULE_ID_VTY, IPC_TYPE_CFM, 0, IPC_OPCODE_UPDATE, ifindex);
    if(ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_L2, MODULE_ID_VTY, IPC_TYPE_CFM, 0, IPC_OPCODE_UPDATE, ifindex) < 0)
    {
        zlog_err("%s:send l2 msg failed \n",__FUNCTION__);
    }
    if(syncvty.vty_slot == VTY_SLAVE_SLOT)
    {
//        ret = ipc_send_ha(NULL, 0, 0, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, IPC_OPCODE_EVENT, 0);
        ret = ipc_send_msg_ha_n2(NULL, 0, 0, MODULE_ID_FTM, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, IPC_OPCODE_EVENT, 0, 0);
        if(ret < 0)
        {
            printf("In func %s,Slave notify master reconfig finish error!Arp sync maybe error!\n", __func__);
        }
    }


    fclose(confp);
    return (0);
}

int vtysh_reconfig_index(uint32_t ifindex)
{
    char ifname[IFNET_NAMESIZE];
    char ifname_tmp[IFNET_NAMESIZE];
    FILE *confp = NULL;
    int8_t ret = -1;

    ret = ifm_get_name_by_ifindex(ifindex, ifname_tmp);

    if (0 != ret)
    {
        return 1;
    }

    confp = fopen (VTYSH_STARTUP_CONFIG, "r");
    if (confp == NULL)
    {
        return 1;
    }

    sprintf(ifname, "interface %s", ifname_tmp);
    vtysh_read_file (confp, ifname);
    fclose (confp);

	return (0);
}

int vtysh_aaa_msg_recv_n(struct ipc_mesg_n *mesg)
{
    void *pdata = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;

    pmsghdr = &mesg->msghdr;
    pdata = mesg->msg_data;
    VTY_DEBUG(VD_AUTH, "msg type %d", pmsghdr->msg_subtype);

	switch(pmsghdr->msg_subtype)
	{
		case AAA_MSG_AUTH_RESPONSE:
			vtysh_handle_aaa_login_response(pdata);
			break;

		case AAA_MSG_MAX_IDLE_TIME:
			vtysh_handle_aaa_max_idle_time((AAA_MAX_IDLE_MSG *)pdata);
			break;

		case AAA_MSG_FORCE_EXIT:
			vtysh_handle_aaa_force_exit(pdata);
			break;

		default:
			break;
	}
}

#if 0
void vty_cmd_msg_recv(struct ipc_mesg *mesg)
{
    void *pdata = NULL;
    struct ipc_msghdr *pmsghdr = NULL;
    int data_len;

    pmsghdr = &(mesg->msghdr);
    pdata = mesg->msg_data;
    data_len = pmsghdr->data_len;

    if(pmsghdr->msg_subtype == VTY_MSG_CMD)
    {
        struct login_session *session = NULL;

        if(pmsghdr->sender_id == MODULE_ID_OPENFLOW)
        {
            zlog_debug(VD_MSG, "get MODULE_ID_OPENFLOW\n");
            session = vty_ofp_session_get();
        }
        else if(pmsghdr->sender_id == MODULE_ID_SNMPD)
        {
            zlog_debug(VD_MSG, "get MODULE_ID_SNMPD\n");
            session = vty_snmp_session_get();
        }
        else if(pmsghdr->sender_id == MODULE_ID_OSPF)
        {
            zlog_debug(VD_MSG, "get MODULE_ID_OSPF\n");
            session = vty_ospf_session_get();
        }

        if(!session)
        {
            zlog_err("fail to find session.\n");
            return;
        }

        /* wait here if read thread lock bufdata_delete_lock */
        pthread_mutex_lock(&session->bufdata_delete_lock);
        session->recv_copy_busy = 1;
        char *buf_tmp = pdata;
        size_t buf_len_tmp = data_len;

        /*alloc bufdata until we put all the size of buf_len data*/
        while(buf_len_tmp)
        {
            usleep(100);

            session->recv_bufdata = \
                                    buffer_put_new_bufdata(session->buf_ipc_recv, buf_tmp, &buf_len_tmp);

            if(!session->read_bufdata)
            {
                session->read_bufdata = session->recv_bufdata;
            }

            session->bufdata_cnt++;
            zlog_debug(VD_MSG, "##IPCCMD:after buffer_put_new_bufdata bufdata_cnt:%d\n", session->bufdata_cnt);
        }

        session->recv_copy_busy = 0;
        pthread_mutex_unlock(&session->bufdata_delete_lock);
    }
}
#endif
int vtysh_cmd_msg_recv_n(struct ipc_mesg_n *mesg)
{
    void *pdata = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
    int data_len;
    pmsghdr = &mesg->msghdr;
    pdata = mesg->msg_data;
    data_len = pmsghdr->data_len;
    if(pmsghdr->msg_subtype == VTY_MSG_CMD)
    {
        struct login_session *session = NULL;
        if(pmsghdr->sender_id == MODULE_ID_OPENFLOW)
        {
            zlog_debug(VD_MSG, "get MODULE_ID_OPENFLOW\n");
            session = vty_ofp_session_get();
        }
        else if(pmsghdr->sender_id == MODULE_ID_SNMPD)
        {
            zlog_debug(VD_MSG,"get MODULE_ID_SNMPD\n");
            session = vty_snmp_session_get();
        }
        else if(pmsghdr->sender_id == MODULE_ID_OSPF)
        {
            zlog_debug(VD_MSG,"get MODULE_ID_OSPF\n");
            session = vty_ospf_session_get();
        }
        if(!session)
        {
            zlog_err("fail to find session.\n");
            return -1;
        }
        pthread_mutex_lock(&session->bufdata_delete_lock);
        session->recv_copy_busy = 1;
        char *buf_tmp = pdata;
        size_t buf_len_tmp = data_len;
        while(buf_len_tmp)
        {
            usleep(100);
            session->recv_bufdata = \
                                    buffer_put_new_bufdata(session->buf_ipc_recv, buf_tmp, &buf_len_tmp);
            if(!session->read_bufdata)
            {
                session->read_bufdata = session->recv_bufdata;
            }
            session->bufdata_cnt++;
            zlog_debug(VD_MSG, "##IPCCMD:after buffer_put_new_bufdata bufdata_cnt:%d\n", session->bufdata_cnt);
        }
        session->recv_copy_busy = 0;
        pthread_mutex_unlock(&session->bufdata_delete_lock);
    }
    return 0;
}
int vtysh_ifm_msg_recv_n(struct ipc_mesg_n *mesg)
{
    void *pdata = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
    struct ipc_mesg_n *pmesg;
    pmsghdr = &mesg->msghdr;
    pdata = mesg->msg_data;
    if(pmsghdr->opcode == IPC_OPCODE_EVENT && 
                    pmsghdr->msg_subtype == IFNET_EVENT_IF_ADD)
    {
        if(syncvty.vty_slot == VTY_MAIN_SLOT
                && (syncvty.all_cards_ready_flag == 0
                    || syncvty.self_status < VTYSH_BATCH_CONFIG_FINISH))
        {
            return 0;
        }
        pmesg = (struct ipc_mesg_n *)malloc(sizeof(struct ipc_mesg_n));
        pthread_mutex_lock(&ifm_event_lock);
        memcpy(pmesg, mesg, sizeof(struct ipc_mesg_n));
        listnode_add(&ifm_event, pmesg);
        pthread_mutex_unlock(&ifm_event_lock);
    }
    if(pmsghdr->opcode == IPC_OPCODE_EVENT &&  \
            pmsghdr->msg_subtype == IFNET_EVENT_IF_ADD_FINISH)
    {
        if(syncvty.all_cards_ready_flag == 1)
        {
            zlog_debug(VD_MSG,"already get IFM_EVENT_IF_ADD_FINISH, drop\n");
            return 0;
        }
#ifdef VTYSH_SYNC_DEBUG
        printf("recv IFM_EVENT_IF_ADD_FINISH\n");
        printf("all_cards_ready_flag:%d slot_type:%d peer_status:%d self_status:%d\n",
               syncvty.all_cards_ready_flag, syncvty.vty_slot,
               syncvty.peer_status, syncvty.self_status);
#endif
        syncvty.all_cards_ready_flag = 1;
        zlog_debug(VD_MSG,"recv IFM_EVENT_IF_ADD_FINISH\n");
        zlog_debug(VD_MSG, "all_cards_ready_flag:%d slot_type:%d peer_status:%d self_status:%d\n",
                   syncvty.all_cards_ready_flag, syncvty.vty_slot,
                   syncvty.peer_status, syncvty.self_status);
        if((syncvty.vty_slot == VTY_MAIN_SLOT
                || syncvty.vty_slot == VTY_UNKNOW_SLOT)
                && syncvty.vtysh_start_record == 1
                && syncvty.all_cards_ready_flag == 1)
        {
            vtysh_master_config_recover();
        }
        if(syncvty.all_cards_ready_flag == 1
                && syncvty.vty_slot == VTY_MAIN_SLOT
                && syncvty.peer_status == VTYSH_START
                && syncvty.self_status >= VTYSH_BATCH_CONFIG_FINISH
                && syncvty.vtysh_start_record == 1)
        {
            vtysh_batch_sync_start();
        }
    }    
    return 0;
}
int vtysh_devm_msg_recv_n(struct ipc_mesg_n *mesg)
{
    void *pdata = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
  //struct ipc_mesg_n *pmesg;
    pmsghdr = &mesg->msghdr;
    pdata = mesg->msg_data;
    if(pmsghdr->opcode == IPC_OPCODE_EVENT)
    {
        if(DEV_EVENT_HA_BOOTSTATE_CHANGE == pmsghdr->msg_subtype)
        {
            vtysh_devm_msg_recv(mesg);
        }
        else if(DEV_EVENT_SLOT_DELETE == pmsghdr->msg_subtype)
        {
            vtysh_devm_msg_delete_slot(mesg);
        }
        else
        {
            zlog_debug(VD_MSG,"wrong devm msg subtype:%d\n", pmsghdr->msg_subtype);
        }
    }
    return 0;
}
int vtysh_ha_msg_recv_n(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *pmsghdr = NULL;
    pmsghdr = &mesg->msghdr;
    if(MODULE_ID_FILE == pmsghdr->sender_id
            && HA_SMSG_BATCH_END == pmsghdr->msg_subtype)
    {
        zlog_debug(VD_MSG,"vtysh recv msg from filem\n");
        if(syncvty.vty_slot != VTY_SLAVE_SLOT)
        {
            return ERRNO_FAIL;
        }
        printf("get filem batch end \n");
        vtysh_slave_sync_start();
    }    
    return 0;
}
int vtysh_sync_msg_recv_n(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *pmsghdr = NULL;
    
    pmsghdr = &mesg->msghdr;
    if(MODULE_ID_VTY == pmsghdr->sender_id
            && MODULE_ID_VTY == pmsghdr->module_id)
    {
        if((syncvty.vty_slot == VTY_SLAVE_SLOT) && (syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE))
        {
            vtysh_slave_recv_msg(mesg);
        }
        if(syncvty.vty_slot == VTY_MAIN_SLOT)
        {
            vtysh_sync_master_recv_msg(mesg);
        }
    }    
    return 0;
}

int vtysh_netconf_msg_recv_n(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *pmsghdr = NULL;
    union  pkt_control *pkt_ctrl = NULL;
    int len, i;
    unsigned char *buf, *buf_end;
    uint32_t client_ip;
    uint16_t client_port;
    struct login_session *session;
    struct ssh_packet *ssh_packet;
    Channel *c;
   
    pmsghdr = &mesg->msghdr; 

    pkt_ctrl = (union  pkt_control *)mesg->msg_data;   

    client_ip = pkt_ctrl->ipcb.sip;
    client_port = pkt_ctrl->ipcb.dport;
    buf = mesg->msg_data + sizeof(union  pkt_control);
    len = pmsghdr->data_len - sizeof(union  pkt_control);
    buf_end = buf + len ;
    *buf_end = 0;

    session = vty_ssh_session_get(client_ip, client_port);

    if(session == NULL)
    {
        zlog_err("find session failed!\n");
        return -1;
    }

    ssh_packet = (struct ssh_packet *)session->ssh_packet;

    /*find channel, copy data to channel*/
    for(i = 0; i < ssh_packet->channels_alloc; i++)
    {
        if((c = ssh_packet->channels[i]) != NULL && c->type == CHANNEL_SUB_NETCONF)
        {
            pthread_mutex_lock(&c->netconf_data_lock);
            channel_out(c, "%s", buf);

            pthread_mutex_unlock(&c->netconf_data_lock);
            break;
        }
    } 

    return 0;
}

#if 0
static int
vty_msg_rcv_msg()
{
    static struct ipc_mesg mesg;
    struct ipc_mesg *pmesg;
    struct ipc_msghdr *pmsghdr = NULL;
    int token = 8;
    int ret;

    memset(&mesg, 0, sizeof(struct ipc_mesg));

    while(token)
    {
        ret = ipc_recv_common(&mesg, MODULE_ID_VTY);

        if(ret == -1)
        {
            return ERRNO_FAIL;
        }

        token--;

        /* process the ipc message */
        pmsghdr = &(mesg.msghdr);

        zlog_debug(VD_MSG, "##IPC:pmsghdr->msg_type:%d\tpmsghdr->msg_subtype:%d\n", pmsghdr->msg_type, pmsghdr->msg_subtype);

        if(pmsghdr->msg_type == IPC_TYPE_IFM)
        {
            zlog_debug(VD_MSG, "recv form ifm\n");
#ifdef VTYSH_SYNC_DEBUG
            printf("recv form ifm\n");
#endif

            if(pmsghdr->opcode == IPC_OPCODE_EVENT &&  \
                    pmsghdr->msg_subtype == IFNET_EVENT_IF_ADD)
            {
                if(syncvty.vty_slot == VTY_MAIN_SLOT
                        && (syncvty.all_cards_ready_flag == 0
                            || syncvty.self_status < VTYSH_BATCH_CONFIG_FINISH))
                {
                    continue;
                }

                pmesg = (struct ipc_mesg *)malloc(sizeof(struct ipc_mesg));

                pthread_mutex_lock(&ifm_event_lock);
                memcpy(pmesg, &mesg, sizeof(struct ipc_mesg));
                listnode_add(&ifm_event, pmesg);
                pthread_mutex_unlock(&ifm_event_lock);

            }

            if(pmsghdr->opcode == IPC_OPCODE_EVENT &&  \
                    pmsghdr->msg_subtype == IFNET_EVENT_IF_ADD_FINISH)
            {
                if(syncvty.all_cards_ready_flag == 1)
                {
                    zlog_debug(VD_MSG, "already get IFM_EVENT_IF_ADD_FINISH, drop\n");
                    continue;
                }

#ifdef VTYSH_SYNC_DEBUG
                printf("recv IFM_EVENT_IF_ADD_FINISH\n");
                printf("all_cards_ready_flag:%d slot_type:%d peer_status:%d self_status:%d\n",
                       syncvty.all_cards_ready_flag, syncvty.vty_slot,
                       syncvty.peer_status, syncvty.self_status);
#endif
                syncvty.all_cards_ready_flag = 1;
                zlog_debug(VD_MSG, "recv IFM_EVENT_IF_ADD_FINISH\n");
                zlog_debug(VD_MSG, "all_cards_ready_flag:%d slot_type:%d peer_status:%d self_status:%d\n",
                           syncvty.all_cards_ready_flag, syncvty.vty_slot,
                           syncvty.peer_status, syncvty.self_status);

                /*config recover until all cards are ready*/
                if((syncvty.vty_slot == VTY_MAIN_SLOT
                        || syncvty.vty_slot == VTY_UNKNOW_SLOT)
                        && syncvty.vtysh_start_record == 1
                        && syncvty.all_cards_ready_flag == 1)
                {
                    vtysh_master_config_recover();
                }

                /**/
                if(syncvty.all_cards_ready_flag == 1
                        && syncvty.vty_slot == VTY_MAIN_SLOT
                        && syncvty.peer_status == VTYSH_START
                        && syncvty.self_status >= VTYSH_BATCH_CONFIG_FINISH
                        && syncvty.vtysh_start_record == 1)
                {
                    vtysh_batch_sync_start();
                }
            }
        }
        else if(pmsghdr->msg_type == IPC_TYPE_AAA)
        {
            zlog_debug(VD_MSG, "recv user msg from aaa\n");
            vty_aaa_msg_recv(&mesg);
        }
        else if(pmsghdr->msg_type == IPC_TYPE_VTY)
        {
            vty_cmd_msg_recv(&mesg);
        }
        else if(IPC_TYPE_DEVM == pmsghdr->msg_type && pmsghdr->opcode == IPC_OPCODE_EVENT)
        {
#ifdef VTYSH_SYNC_DEBUG
            printf("recv_from_devm_pmsghdr->msg_subtype:%d\n", pmsghdr->msg_subtype);
#endif
            zlog_debug(VD_MSG, "recv_from_devm_pmsghdr->msg_subtype:%d\n", pmsghdr->msg_subtype);

            if(DEV_EVENT_HA_BOOTSTATE_CHANGE == pmsghdr->msg_subtype)
            {
                vtysh_devm_msg_recv(&mesg);
            }
            else if(DEV_EVENT_SLOT_DELETE == pmsghdr->msg_subtype)
            {
                vtysh_devm_msg_delete_slot(&mesg);
            }
            else
            {
                zlog_debug(VD_MSG, "wrong devm msg subtype:%d\n", pmsghdr->msg_subtype);
            }
        }
        else if(IPC_TYPE_HA == pmsghdr->msg_type)
        {
            zlog_debug(VD_MSG, "revc IPC_TYPE_HA\n");

            /*msg from filem, batch configfile transport finish*
             *slave vtysh start to batch configure*/
            if(MODULE_ID_FILE == pmsghdr->sender_id
                    && HA_SMSG_BATCH_END == pmsghdr->msg_subtype)
            {
#ifdef VTYSH_SYNC_DEBUG
                printf("vtysh recv msg from filem\n");
#endif
                zlog_debug(VD_MSG, "vtysh recv msg from filem\n");

                if(syncvty.vty_slot != VTY_SLAVE_SLOT)
                {
                    return ERRNO_FAIL;
                }

                vtysh_slave_sync_start();
            }
        }
        else if(IPC_TYPE_SYNC_DATA == pmsghdr->msg_type)
        {
#ifdef VTYSH_SYNC_DEBUG
            printf("recv sync_data from ha\n");
            printf("pmsghdr->sender_id:%d pmsghdr->module_id:%d\n",
                   pmsghdr->sender_id, pmsghdr->module_id);
#endif

            zlog_debug(VD_MSG, "recv sync_data from ha\n");
            zlog_debug(VD_MSG, "pmsghdr->sender_id:%d pmsghdr->module_id:%d\n",
                       pmsghdr->sender_id, pmsghdr->module_id);

            if(MODULE_ID_HA == pmsghdr->sender_id
                    && MODULE_ID_VTY == pmsghdr->module_id)
            {
#if 0

                if(syncvty.vty_slot != VTY_SLAVE_SLOT)
                {
                    return ERRNO_FAIL;
                }

                if(syncvty.self_status != VTYSH_REALTIME_SYNC_ENABLE)
                {
                    return ERRNO_FAIL;
                }

#endif

                if((syncvty.vty_slot == VTY_SLAVE_SLOT) && (syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE))
                {
                    vtysh_slave_recv_msg(&mesg);
                }

                if(syncvty.vty_slot == VTY_MAIN_SLOT)
                {
                    vtysh_sync_master_recv_msg(&mesg);
                }
            }
		}
        else if (IPC_TYPE_VCG == pmsghdr->msg_type)
        {
            if (VCG_VTYSH_RECONFIG == pmsghdr->msg_subtype)
            {
                vtysh_reconfig_index(pmsghdr->msg_index);
            }
        }
		else if (IPC_TYPE_EFM == pmsghdr->msg_type)
		{
			vtysh_handle_ospf_msg(&mesg);
		}
		else if (IPC_TYPE_SNMP == pmsghdr->msg_type)
		{
			vty_handle_snmp_msg(&mesg);
		}
    }

    return ERRNO_SUCCESS;
}
#endif
void vty_create_connection(struct ipc_mesg_n *mesg)
{
    void *pdata = NULL;
//  struct ipc_msghdr *pmsghdr = NULL;
//  int data_len;
    struct tcp_session *tcp_session;
    uint32_t sip;
    uint32_t dip;
    uint16_t sport, dport;
    struct login_session *session = NULL;

	uint16_t vpn = vtysh_vpn_get();

    zlog_debug(VD_MSG, "vty_create_connection \n");
//  pmsghdr = &(mesg->msghdr);
    pdata = mesg->msg_data;

    tcp_session = (struct tcp_session *)pdata;
    sip = tcp_session->sip;
    dip = tcp_session->dip;
    sport = tcp_session->sport;
    dport = tcp_session->dport;

    zlog_debug(VD_MSG,"client_ip:%x client_port:%d dip %x, server_port:%d\n", sip, sport, dip, dport);

    switch(dport)
    {
        case TCP_PORT_TELNET:
            session = vty_telnet_session_get(sip, sport);

            if(!session)
            {
                if(host.login_cnt <= USER_LOGIN_MAX)
                {
                    session = vty_telnet_thread_creat_connect(sip, sport, dip);
                    if(session == NULL)
                    {
                        break;
                    }
                }
                else
                {
                    VTY_DEBUG(VD_TELNET, "Reach max connections, cnt = %d ", host.login_cnt);
                    printf("Reach max connections\n");
                    struct send_type send_buf;
                    char buf[] = "\r\n% Reach max connections\r\n";

                    send_buf.send_buf = buf;
                    send_buf.send_len = strlen(buf);

                    buffer_pkt_send(sip, sport, TCP_PORT_TELNET, vpn, send_buf);

                    vty_tcp_close(sip, sport, TCP_PORT_TELNET);
                }
            }
            else
            {
                zlog_err("Repeated session client_ip:%d client_port:%d server_port:%d\n", sip, sport, dport);
            }

            break;

        case TCP_PORT_SSH:
        case 830:
            session = vty_ssh_session_get(sip, sport);

            if(!session)
            {
 //               vty_tcp_open(sip, sport, TCP_PORT_SSH);

                if(host.login_cnt <= USER_LOGIN_MAX)
                {
                    session = vty_ssh_thread_creat_connect(sip, sport, dip);
                }
                else
                {
                    struct send_type send_buf;
                    char buf[] = "\r\n% Reach max connections\r\n";

                    send_buf.send_buf = buf;
                    send_buf.send_len = strlen(buf);

                    printf("Reach max connections \n");
                    buffer_pkt_send(sip, sport, TCP_PORT_SSH, vpn, send_buf);  // xiawl

                    vty_tcp_close(sip, sport, TCP_PORT_SSH);
                }

            }
            else
            {
                zlog_err("Repeated session client_ip:%d client_port:%d server_port:%d\n", sip, sport, dport);
            }

            break;

        default:
            zlog_err("Unknow server_port:%d, drop!", dport);
            break;
    }

}

void vty_connect_break_from_client(struct ipc_mesg_n *mesg)
{
    void *pdata = NULL;
//  struct ipc_msghdr *pmsghdr = NULL;
    struct tcp_session *tcp_session;
    uint32_t sip;
    uint16_t sport;
    struct login_session *session = NULL;

    zlog_debug(VD_MSG, "vty_connect_break_from_client\n");
//  pmsghdr = &(mesg->msghdr);
    pdata = mesg->msg_data;

    tcp_session = (struct tcp_session *)pdata;
    sip = tcp_session->sip;
    sport = tcp_session->sport;
    zlog_debug(VD_MSG, "connection sip:%x sport:%d break\n", sip, sport);

    session = vty_telnet_session_get(sip, sport);

    if(!session)
    {
        zlog_err("vty_connect_break_from_client get session fail. sip:%x sport:%d\n", sip, sport);
        telnetc_server_exit(tcp_session->dip, sip, tcp_session->dport);
        return;
    }

	if(session->user_name && (0 == strcmp(session->user_name, DEF_VTY_USER_NAME)))
	{
		gVtyLocalUserLogin = 0;
	}

    session->session_status = SESSION_CLOSE;
}



struct login_session *
vty_ssh_thread_creat_connect(uint32_t client_ip, uint16_t client_port, uint32_t server_ip)
{
    struct login_session *session;
    uint32_t sip, dip;
    uint16_t sport;
    uint16_t local_port;
    pthread_attr_t attr;
    int err;
    sip = client_ip;
	dip = server_ip;
    sport = client_port;
    local_port = TCP_PORT_SSH;

    session = vty_ssh_session_creat(sip, sport, local_port, dip);
	if(NULL == session)
	{
		return NULL;
	}

    /*Creat read thread*/
    err = pthread_attr_init(&attr);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(err == 0)
    {
        err = pthread_create(&(session->tid_r), &attr, vty_ssh_read, (void *) session);
    }

    if(err != 0)
    {
        perror("ssh read pthread_create fail:");
        exit(-1);
    }

    pthread_attr_destroy(&attr);

    return session;
}

struct login_session *
vty_ssh_thread_creat(struct pkt_buffer *pkt)
{
    struct login_session *session;
    uint32_t sip;
    uint16_t sport;
    uint16_t local_port;
    uint32_t dip;
    pthread_attr_t attr;
    int err;
    sip = pkt->cb.ipcb.sip;
    sport = pkt->cb.ipcb.sport;
    local_port = pkt->cb.ipcb.dport;
    dip = pkt->cb.ipcb.dip;

    session = vty_ssh_session_creat(sip, sport, local_port,dip);

    /*Creat read thread*/
    err = pthread_attr_init(&attr);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(0);
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(err == 0)
    {
        err = pthread_create(&(session->tid_r), NULL, vty_ssh_read, (void *) session);
    }

    if(err != 0)
    {
        perror("ssh read pthread_create fail:");
        exit(-1);
    }

    pthread_attr_destroy(&attr);

    return session;
}

void vtysh_handle_tcp_ctrl_msg(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *phdr = NULL;
    phdr = &(mesg->msghdr);

    if(!vtysh_sync_compelete())
    {
        return;
    }
	
    VTY_DEBUG(VD_TELNET, "opcode %d, host.login_cnt: %d",phdr->opcode, host.login_cnt);
    switch(phdr->opcode)
    {
        case IPC_OPCODE_DISCONNECT:
            vty_connect_break_from_client(mesg);
            break;

        case IPC_OPCODE_CONNECT:
            vty_create_connection(mesg);
            break;

        default:
            zlog_err("recv unknow ftm msg, drop\n");
            break;
    }
}

void vtysh_handle_tcp_data_msg(struct ipc_mesg_n *mesg)
{
	char *buf_tmp;
	size_t buf_len_tmp;
    struct pkt_buffer *pkt = NULL;
    uint32_t sip;
    uint32_t dip;
    uint16_t sport, dport;
    unsigned int buf_len;
    struct login_session *session;
//  struct login_session *session_loop;
//  struct listnode *node, *nnode;
    struct vty *vty = NULL;
    struct telnet_client *tc = NULL;
    if(!vtysh_sync_compelete())
    {
        return;
    }

    pkt = (struct pkt_buffer *)(mesg->msg_data);
	if(NULL == pkt)
	{
		return;
	}
	APP_RECV_PKT_FROM_FTM_DATA_SET(pkt);
	
    zlog_debug(VD_MSG, "#RECV:vty_tel_msg_rcv %s\n", pkt_dump(pkt->data, pkt->data_len));

    sip = pkt->cb.ipcb.sip;
    dip = pkt->cb.ipcb.dip;
    sport = pkt->cb.ipcb.sport;
    dport = pkt->cb.ipcb.dport;
    buf_len = pkt->data_len;
    zlog_debug(VD_MSG, "#serport%d cliport:%d\n", dport, sport);

    /*lisen to port 23(telnet) 22(ssh) and 830(netconf)*/
    if(sport == TCP_PORT_TELNET)
    {
        tc = telnetc_match(dip, sip, dport); // 对端发过来的，所以dip sip 互换
        if(tc != NULL)
        {
            session = tc->ss;
            vty = session->vty;
            telnetc_recv_from_server(session, pkt->data, pkt->data_len);
            return ;
        }
    }
    if(dport != TCP_PORT_TELNET && dport != TCP_PORT_SSH && dport != 830)
    {
        return;
    }

    switch(dport)
    {
        case TCP_PORT_TELNET:
            session = vty_telnet_session_get(sip, sport);

            if(!session)
            {
                zlog_err("No session found! client_ip:%d client_port:%d server_port:%d\n", sip, sport, dport);
                return;
            }
            session->server_ip = dip;
            session->server_port = dport;
            if(session->session_status == SESSION_CLOSE)
            {
                return;
            }

            break;

        case TCP_PORT_SSH:
        case 830:
            session = vty_ssh_session_get(sip, sport);

            if(!session)
            {
                zlog_err("No session found! client_ip:%d client_port:%d server_port:%d\n", sip, sport, dport);
                return;
            }

            if(session->session_status == SESSION_CLOSE)
            {
                return;
            }

            break;

        default:
            zlog_err("Unknow server_port:%d, drop!", dport);
            return;
    }

    if(session->session_type == SESSION_TELNET)
    {
        vty = session->vty;
    }

    if(session->session_type == SESSION_TELNET && vty != NULL)
    {
        int have_int_sig = 0;

        /*handle ctrl c to app as interrupt*/
        for(unsigned int i = 0; i < buf_len; i++)
        {
            if(*((char *)(pkt->data) + i) == 'C' - '@')
            {
                if(session->client == SERVER)
                {
                    if(vty->node > AUTH_NODE)
                    {
                        /*send sigint flag to apps*/
                        vty->sig_int_recv = 1;
                        vtysh_execute_sigint(vty);
                        vty->sig_int_recv = 0;
                    }
                }
                else
                {
                    telnetc_send_ctrlc(session->tc);
                }

                have_int_sig = 1;
            }
        }

        //add timer
        if(vty->v_timeout && vty->type == VTY_TERM)
        {
            if(session->timer_count > 0)
            {
                session->timer_count = 0;
            }
        }

        if(have_int_sig)
        {
            have_int_sig = 0;
            return;
        }
    }

    /*session->vty->buf_tmp will process additional msg user put*/
    if(vty != NULL && vty->telnet_recv_tmp == 1)
    {
        if(VTY_BUFSIZ - strlen(vty->buf_tmp) > buf_len)
        {
            /*some block place need specific input*/
            memcpy(vty->buf_tmp, pkt->data, buf_len);
        }
    }
    else
    {
        /* wait here if read thread lock bufdata_delete_lock */
        pthread_mutex_lock(&session->bufdata_delete_lock);

        if(session->bufdata_cnt < BUF_DATA_IPC_MAX)
        {
            session->recv_copy_busy = 1;
            buf_tmp = pkt->data;
            buf_len_tmp = buf_len;

			buffer_put_current_bufdata(session->buf_ipc_recv, pkt->data, &buf_len_tmp);

			while(buf_len_tmp)
			{
	//			usleep(100);
			
				buf_tmp = buf_tmp + (buf_len - buf_len_tmp);
				session->recv_bufdata = \
										buffer_put_new_bufdata(session->buf_ipc_recv, buf_tmp, &buf_len_tmp);
			
				if(!session->read_bufdata)
				{
					session->read_bufdata = session->recv_bufdata;
				}
			
				session->bufdata_cnt++;
			}

            session->recv_copy_busy = 0;
        }

        pthread_mutex_unlock(&session->bufdata_delete_lock);
    }
}

#if 0
/* ���� telnet ���� */
static int
vty_tcp_msg_rcv()
{
    struct ipc_pkt *mesg = NULL;
    struct ipc_msghdr *phdr = NULL;
    struct login_session *session_loop;
    struct listnode *node, *nnode;
    int token = 8;

    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
    {
        if(session_loop->session_status == SESSION_CLOSE)
        {
            if((session_loop->session_type == SESSION_TELNET \
                    && session_loop->tid_r_exit == 1 \
                    && session_loop->tid_w_exit == 1) \
                    || session_loop->session_type == SESSION_SSH) // xiawl
            {
                pthread_mutex_lock(&session_delete_lock);
                vty_session_delete(session_loop);
                pthread_mutex_unlock(&session_delete_lock);

                return 0;
            }
        }
    }

    //����Ϣ�������հ���ÿ���� 8 ��
    while(token)
    {
        token--;
        //pkt = pkt_rcv(MODULE_ID_VTY);
        mesg = pkt_rcv_vty(MODULE_ID_VTY);

        if(mesg == NULL)
        {
            return 0;
        }

        if(syncvty.vty_slot == VTY_MAIN_SLOT
                || syncvty.vty_slot == VTY_SLAVE_SLOT)
        {
            /*只有配置恢复或者批量同步完成才允许用户登入*/
            if(syncvty.self_status == VTYSH_UNREADY
                    || syncvty.self_status == VTYSH_START
                    || syncvty.self_status == VTYSH_BATCH_CONFIG_START
                    || syncvty.self_status == VTYSH_BATCH_CONFIG_RECV)
            {
                if(syncvty.vtysh_start_record == 1)
                {
                    usleep(10000);
                    continue;
                }
            }
        }
        else if(syncvty.vty_slot == VTY_UNKNOW_SLOT
                && config_recover_finish == 0)
        {
            if(syncvty.vtysh_start_record == 1)
            {
                usleep(10000);
                continue;
            }
        }

        phdr = &(mesg->msghdr);

        if(phdr->msg_type != IPC_TYPE_FTM && phdr->msg_type != IPC_TYPE_PACKET)
        {
            continue;
        }

        switch(phdr->msg_type)
        {
            case IPC_TYPE_FTM:
                vtysh_handle_tcp_ctrl_msg(mesg);
                break;

            case IPC_TYPE_PACKET:
                vtysh_handle_tcp_data_msg(mesg);
                break;

            default:
                zlog_err("recv unknow packet, drop!\n");
                break;
        }
    }

    return 0;
}
#endif
extern struct hash *cpu_record;

#if 0
// vty_msg_rcv -->vtysh_msg_recv_n
int
vty_msg_rcv(struct thread *thread)
{

    /*recv contrl msg from aaa ifm snmp*/
    vty_msg_rcv_msg();
    /*recv telnet msg from ftm*/
    vty_tcp_msg_rcv();

    vty_netconf_pck_rcv();

    /*check closed vty, and recycling resources*/
    vtysh_check_sync_vty_close();
    usleep(10000);//�ó� CPU 10ms

    pthread_mutex_lock(&session_timer_lock);
    thread_add_event(vtysh_master, vty_msg_rcv, NULL, 0);
    pthread_mutex_unlock(&session_timer_lock);
//  fprintf(stderr, "after thread_add_event\n");
    return ERRNO_SUCCESS;

}
#endif

int vtysh_msg_recv_n(struct ipc_mesg_n *pmsg, int len)
{
    int ret;
    int revln = 0;

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    VTY_DEBUG(VD_COMM, "msgtype:%d, revlen %d,len %d",pmsg->msghdr.msg_type, revln, len);

    if(revln <= len)
    {
        switch(pmsg->msghdr.msg_type)
        {
            case IPC_TYPE_IFM:
                ret = vtysh_ifm_msg_recv_n(pmsg);            
                break;               
            case IPC_TYPE_AAA:
                ret = vtysh_aaa_msg_recv_n(pmsg);
                break;
            case IPC_TYPE_VTY:
                ret = vtysh_cmd_msg_recv_n(pmsg);   
                break;
            case IPC_TYPE_DEVM:
                ret = vtysh_devm_msg_recv_n(pmsg); 
                break;
            case IPC_TYPE_HA:
                ret = vtysh_ha_msg_recv_n(pmsg);
                break;

            case IPC_TYPE_SYNC_DATA:
                ret = vtysh_sync_msg_recv_n(pmsg);
                break;

            case IPC_TYPE_FTM:
                vtysh_handle_tcp_ctrl_msg(pmsg);
				break;
                
            case IPC_TYPE_PACKET:
				if(pmsg->msghdr.msg_subtype == 0)
                {
                    vtysh_handle_tcp_data_msg(pmsg);
                }
                else
                {
                    vtysh_netconf_msg_recv_n(pmsg);
                }
                break;
               
            default:
                break;
        }
    }

	if(pmsg)
	{
    	mem_share_free(pmsg, MODULE_ID_VTY);
	}
    
    return ret;
}

#if 0
int channel_out(Channel *c, const char *format, ...)
{
    va_list args;
    int len = 0;
    int size = 1024;
    char buf[1024];
    char *p = NULL;

    /* Try to write to initial buffer.  */
    va_start(args, format);
    len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    /* Initial buffer is not enough.  */
    if(len < 0 || len >= size)
    {
        while(1)
        {
            if(len > -1)
            {
                size = len + 1;
            }
            else
            {
                size = size * 2;
            }

            p = XREALLOC(MTYPE_VTY, p, size);

            if(! p)
            {
                return -1;
            }

            va_start(args, format);
            len = vsnprintf(p, size, format, args);
            va_end(args);

            if(len > -1 && len < size)
            {
                break;
            }
        }
    }

    /* When initial buffer is enough to store all output.  */
    if(! p)
    {
        p = buf;
    }

//  pthread_mutex_lock (&vty->obuf_lock);
    /* Pointer p must point out buffer. */
    buffer_put(c->channel_output_data, (u_char *) p, len);
//  pthread_mutex_unlock (&vty->obuf_lock);

    /* If p is not different with buf, it is allocated buffer.  */
    if(p != buf)
    {
        XFREE(MTYPE_VTY, p);
    }

    return len;
}
#endif

// xiawl 
#if 0
int
vty_netconf_pck_rcv()
{
//  struct pkt_buffer *pkt = NULL;
    int len, i;
    unsigned char *buf, *buf_end;
    uint32_t client_ip;
    uint16_t client_port;
    struct login_session *session;
    struct ssh_packet *ssh_packet;
    Channel *c;
    static struct ipc_pkt mesg;
    struct ipc_msghdr *msg_head;
    union  pkt_control *pkt_ctrl;

    if(netconf_ipc_id == -1)
    {
        return -1;
    }

    if(-1 == ipc_recv_pkt(netconf_ipc_id, &mesg, MODULE_ID_VTY))
    {
        return -1;
    }

    msg_head = &(mesg.msghdr);
    pkt_ctrl = (union  pkt_control *)mesg.msg_data;
    client_ip = pkt_ctrl->ipcb.sip;
    client_port = pkt_ctrl->ipcb.dport;
    buf = mesg.msg_data + sizeof(union  pkt_control);
    len = msg_head->data_len - sizeof(union  pkt_control);
    buf_end = buf + len ;
    *buf_end = 0;

    session = vty_ssh_session_get(client_ip, client_port);

    if(session == NULL)
    {
        zlog_err("find session failed!\n");
        return -1;
    }

    ssh_packet = (struct ssh_packet *)session->ssh_packet;

    /*find channel, copy data to channel*/
    for(i = 0; i < ssh_packet->channels_alloc; i++)
    {
        if((c = ssh_packet->channels[i]) != NULL && c->type == CHANNEL_SUB_NETCONF)
        {
            pthread_mutex_lock(&c->netconf_data_lock);
//          printf("buf:%s\n",buf);
            channel_out(c, "%s", buf);

//          struct buffer_data *data;
            pthread_mutex_unlock(&c->netconf_data_lock);
            break;
        }
    }

//  memset(&mesg, 0, msg_head->data_len);

    return 0;
}
#endif
void vtysh_send_start_record_ha(void)
{
    struct ha_com *p_ha_com;
    struct ha_com t_ha_com;
    struct ipc_mesg_n *pmsgrcv = NULL;
    
    memset(&t_ha_com, 0, sizeof(struct ha_com));
    t_ha_com.module_id = MODULE_ID_VTY;

    
//    p_ha_com = ipc_send_common_wait_reply(&t_ha_com, sizeof(struct ha_com), 1, 
//                MODULE_ID_HA, MODULE_ID_VTY, IPC_TYPE_HA, HA_INFO_APP_INIT, IPC_OPCODE_EVENT, 0x10101010);
    pmsgrcv = ipc_sync_send_n2(&t_ha_com, sizeof(struct ha_com), 1,
                MODULE_ID_DEVM, MODULE_ID_VTY, IPC_TYPE_DEVM, DEVM_INFO_REBOOT_COUNT, IPC_OPCODE_GET, 0x10101010, 2000);
                
    if(pmsgrcv != NULL)  
    {
        if(pmsgrcv->msghdr.opcode != IPC_OPCODE_REPLY)
        {
            zlog_err("%s[%d]p_ha_com msg null\n", __func__, __LINE__);
        }
        else
        {
            p_ha_com = (struct ha_com *)pmsgrcv->msg_data;
            
            if(MODULE_ID_VTY == p_ha_com->module_id)
            {
                syncvty.vtysh_start_record = p_ha_com->module_init_count;

                if(syncvty.vtysh_start_record > 1)
                {
                    int ret = -1;
                    AAA_ERR_REPORT_MSG send_aaa;
                    send_aaa.errNo = ERR_VTY_RESET;
//                    ret = ipc_send_common(&send_aaa, sizeof(send_aaa), 1, MODULE_ID_AAA,
//                                          MODULE_ID_VTY, IPC_TYPE_VTY, AAA_MSG_ERRNO_REPORT, 0);
                    ret = ipc_send_msg_n2(&send_aaa, sizeof(send_aaa), 1, MODULE_ID_AAA,
                                          MODULE_ID_VTY, IPC_TYPE_VTY, AAA_MSG_ERRNO_REPORT, 0, 0);
                    if(ret < 0)
                    {
                        zlog_err("ipc_send_common failed!\n");
                    }

                    int tmp = 0;
//                    ret = ipc_send_ha(&tmp, sizeof(tmp), 1, MODULE_ID_VTY,
//                                      IPC_TYPE_SYNC_DATA, VTY_SYNC_REBOOT, 0, 0); 
                    ret = ipc_send_msg_ha_n2(&tmp, sizeof(tmp), 1, MODULE_ID_VTY, MODULE_ID_VTY, 
                                                IPC_TYPE_SYNC_DATA, VTY_SYNC_REBOOT, 0, 0, 0);
                    VTY_DEBUG(VD_SYNC, "type IPC_TYPE_SYNC_DATA \n");                            
                    if(ret < 0)
                    {
                        zlog_err("ipc_send_ha failed!\n");
                    }

    //              syncvty.self_status = VTYSH_REALTIME_SYNC_ENABLE;
                }

                /*config recover until all cards are ready*/
                if((syncvty.vty_slot == VTY_MAIN_SLOT
                        || syncvty.vty_slot == VTY_UNKNOW_SLOT)
                        && syncvty.vtysh_start_record == 1
                        && syncvty.all_cards_ready_flag == 1)
                {
                    vtysh_master_config_recover();
                }

                /**/
                if(syncvty.all_cards_ready_flag == 1
                        && syncvty.vty_slot == VTY_MAIN_SLOT
                        && syncvty.peer_status == VTYSH_START
                        && syncvty.self_status >= VTYSH_BATCH_CONFIG_FINISH
                        && syncvty.vtysh_start_record == 1)
                {
                    vtysh_batch_sync_start();
                }
            }
        }
        mem_share_free(pmsgrcv, MODULE_ID_VTY);
    }
}



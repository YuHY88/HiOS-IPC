/*
*   define of trunk
*/

#ifndef HIOS_TRUNK_H
#define HIOS_TRUNK_H


#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/hptimer.h>
#include "l2_if.h"

#define TRUNK_DISABLE               0               /*��ʹ��*/
#define TRUNK_ENABLE                1               /*ʹ��*/

#define TRUNK_WTR_DEFAULT           30              /*����ʱ��Ĭ��ֵ*/
#define TRUNK_PRIORITY_DEFAULT      32768           /*���ȼ�Ĭ��ֵ*/
#define TRUNK_INTERVAL_DEFAULT    1             /*���ķ��ͼ��Ĭ��ֵ*/
#define TRUNK_MEMBER_MAX        8               /*���طֵ�����Ա��*/
#define TRUNK_BACK_MEMBER_MAX   2               /*��������Ա��*/

/* trunk ��Ϣ���� */
enum TRUNK_INFO
{
    TRUNK_INFO_INVALID = 0,
    TRUNK_INFO_TRUNK,                               /*��ɾ�ۺ���������ȡtrunk��Ϣ*/
    TRUNK_INFO_PORT,                                /*��ɾ�ۺϳ�Ա��������ȡtrunk��Ա����Ϣ*/
    TRUNK_INFO_GLOBAL_LOADBALANCE,
    TRUNK_INFO_WORK_MODE,                           /*����ģʽ*/
    TRUNK_INFO_ECMP_MODE,                           /*���طֵ���ʽ*/
    TRUNK_INFO_BACKUP_PORT_SELECT,              /*������ʽ�˿�ѡ����*/
    TRUNK_INFO_PORT_SELECT_SET,                 /*��Ա�˿�ѡ��״̬����*/
    TRUNK_INFO_MASTER_PORT_SET,                 /*���˿�ѡ������*/
    TRUNK_INFO_LACP_ENABLE,                      /*lacpЭ��ʹ��*/
    TRUNK_INFO_GLOABLE_ECMP_MODE
};


/* trunk ����ģʽ */
enum TRUNK_MODE
{
    TRUNK_MODE_ECMP = 0,                            /* ���طֵ���ʽ*/
    TRUNK_MODE_BACKUP                               /* ������ʽ*/
};


/*trunk ����ģʽ */
enum TRUNK_SWITCH
{
    TRUNK_SWITCH_NOFAILBACK = 0,                /*������*/
    TRUNK_SWITCH_FAILBACK                       /*����*/
};


/* trunk ���طֵ��ķ�ʽ */
enum TRUNK_ECMP
{
    TRUNK_BALANCE_LABEL = 0,                        /*����label���и��طֵ���Ĭ��ֵ*/
    TRUNK_BALANCE_DIP,                              /*����Ŀ��ip���и��طֵ�*/
    TRUNK_BALANCE_SIP,                              /*����Դip���и��طֵ�*/
    TRUNK_BALANCE_SIP_DIP,                          /*����Դip��Ŀ��ip���и��طֵ�*/
    TRUNK_BALANCE_SMAC,                         /*����Դmac���и��طֵ�*/
    TRUNK_BALANCE_DMAC,                         /*����Ŀ��mac���и��طֵ�*/
    TRUNK_BALANCE_SMAC_DMAC,                        /*����Դmac��Ŀ��mac���и��طֵ�*/
    TRUNK_BALANCE_SPORT_DMAC,                       /*����Դport��Ŀ��mac���и��طֵ�*/
    TRUNK_BALANCE_SPORT_DPORT,
    TRUNK_BALANCE_MAX = 16
};

/*LACP�¼�*/
enum LACP_EVENT
{
    LACP_EVENT_PORT_LINKUP = 0,                         /*�ӿ�linkup�¼�*/
    LACP_EVENT_PORT_LINKDOWN,                           /*�ӿ�linkdown�¼�*/
    LACP_EVENT_RX_TIMEOUT,                                  /*���ճ�ʱ�¼�*/
    LACP_EVENT_RX_PKT,                                      /*���ձ����¼�*/
    LACP_EVENT_ADD_PORT,                                    /*��Ӷ˿��¼�*/
    LACP_EVENT_ENABLE_LACP,                             /*ʹ��lacpЭ���¼�*/
    LACP_EVENT_WORK_MODE                                    /*����ģʽ�ı��¼�*/
};


/*�˿�ѡ��״̬*/
enum LACP_PORT_SELECTED
{
    LACP_UNSELECTED = 0,                                    /*δѡ��*/
    LACP_SELECTED                                           /*ѡ��*/
};

/*lacp״̬��״̬*/
enum LACP_STATE
{
    LACP_INVALID = 0,
    LACP_PORT_DISABLED,
    LACP_RX_EXPIRED,
    LACP_DEFAULT,
    LACP_CURRENT
};

struct lacp_link
{
    enum LACP_STATE  state;                                 /*״̬��״̬*/
    enum IFNET_STP_STATUS  port_state;                  /*�˿�״̬:0:IFNET_STP_BLOCK 1:IFNET_STP_FORWARD*/
    enum LACP_PORT_SELECTED  selected;                      /*�ۺ϶˿��Ƿ�ѡ��*/
    uint32_t  sync_state;                                   /*ͬ��״̬*/
    uint32_t  rx_timer;                                     /*lacp���Ľ��ճ�ʱ����*/
    uint32_t  tx_timer;                                     /*lacp���ķ��ͼ���*/
    uint32_t  rx_flag;                                      /*lacp���ձ�־*/
    struct lacp_pkt    *pdu;                                /*���յ�lacpЭ�鱨��*/
    uint16_t  actor_port;                                   /*Actor�Ķ˿ں�*/
    uint16_t  actor_key;                                    /*Actor������key*/
    uint8_t   actor_state;                                  /*Actor�Ķ˿�״̬*/
    uint8_t   partner_admin_state;                          /*Actor��Ϊ��Ĭ��Partner״̬*/
    uint8_t   partner_state;                                /*Actor��Ϊ��Partner״̬*/
    uint8_t   pad;
    uint8_t   partner_admin_sys[MAC_LEN];                   /*Partner��Ĭ��MAC��ַ*/
    uint8_t   partner_sys[MAC_LEN];                         /*Partner��ϵͳMAC��ַ*/
    uint16_t  partner_admin_spri;                           /*Partner��Ĭ��ϵͳ���ȼ�*/
    uint16_t  partner_spri;                             /*Partner����Ա���õ�ϵͳ�Ż���*/
    uint16_t  partner_admin_key;                            /*PartnerĬ��key*/
    uint16_t  partner_key;                                  /*Partner������key*/
    uint16_t  partner_admin_port;                           /*Partner��Ĭ�϶˿ں�*/
    uint16_t  partner_port;                             /*Partner�Ķ˿ں�*/
    uint16_t  partner_admin_ppri;                           /*Partner��Ĭ�϶˿����ȼ�*/
    uint16_t  partner_ppri;                             /*Partner�Ķ˿����ȼ�*/

    uint32_t pkt_tx;                                        /*���͵�lacp������*/
    uint32_t pkt_rx;                                        /*���յ�lacp������*/
};


/* trunk ��Ա�˿����ݽṹ */
struct trunk_port
{
    uint32_t ifindex;                               /*��Ա�˿�����*/
    uint16_t priority;                              /* �˿����ȼ���Ĭ�� 32768 */
    uint8_t  passive;                               /* 1: lacp ����ģʽ���� */
    uint8_t  linkstatus;                            /*��Ա�ڵ�link״̬*/
    uint32_t failback;                              /*���б�־�����ж�ʱ�ڼ䣬��������*/
    struct lacp_link link;                          /*lacpЭ����ؽṹ��*/
};


/* trunk ���ݽṹ */
struct trunk
{
    uint32_t           ifindex;                    /*trunk�ӿ�����*/
    uint16_t           trunkid;                     /* trunk �ӿڵ� id */
    uint16_t           pad;
    uint32_t           speed;                       /*trunk����*/
    uint8_t            down_flag;                   /* 0: linkup, 1: linkdown��ecmp up ��Ա��Ϊ 0 ʱ,���� backup ��Ա��С�� 2 ʱ��trunk linkdown */
    uint8_t            lacp_enable;                 /* 1: lacp Э��ʹ�ܣ� 0: ��ʹ�� */
    uint8_t            lacp_interval;               /* lacp ���ķ��ͼ����Ĭ�� 1s */
    uint8_t            upport_num;                  /* up �˿��� */
    enum TRUNK_MODE    work_mode;                   /* 1: ������ʽ, 0: ���طֵ���ʽ */
    enum TRUNK_SWITCH  failback;                    /* 0: ������, 1: ���� */
    enum TRUNK_ECMP    ecmp_mode;                   /* ���طֵ���ʽ */
    enum BACKUP_E      status;                      /* ��/������״̬ */
    struct list        portlist;                    /* trunk ��Ա������� struct trunk_port ���ݽṹ */
    uint16_t           wtr;                         /* ����ʱ�䣬Ĭ�� 30s*/
    uint16_t           priority;                    /* lacp sys ���ȼ���Ĭ�� 32768 */
    uint32_t           master_if;                   /* ���˿����� */
    uint32_t           member[TRUNK_MEMBER_MAX];    /*���湤���˿ڳ�Ա�б�*/
    /*struct thread      *fb_timer;*/                   /*���ж�ʱ��*/
   TIMERID fb_timer;
   uint8_t  trunk_under_interface_flag;
};

struct trunk_reserve_vlan_t{
	uint16_t trunkid;
	uint16_t reserve_vlan;
	uint16_t reserve_vlan_bak;
	uint8_t  flag;
};

struct trunk_reserve_vlan_t trunk_reserve_vlan_table[3];

struct trunk *trunk_table[IFM_TRUNK_MAX];

struct trunk_config
{
	uint8_t gloable_balance_mode;
};
extern struct trunk_config trunk_gloable_config;
enum gloable_balance_config
{
    GLOABLE_BALANCE_DEFAULT = 0,
    GLOABLE_BALANCE_CONFIG
};
#define  TRUNK_UNDER_INTERFACE_ENABLE  0x01

/*trunkl debug define */
	
#define TRUNK_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_TRUNK, fmt, ##__VA_ARGS__ ); \
    } while(0)


/* �� trunk �Ĳ������� */
void trunk_init(void);                                                              /*trunk��ʼ��*/
void trunk_table_init(void);                                                     /*trunk���ʼ��*/
struct trunk *trunk_add(uint16_t trunkid);                                       /*trunk����*/
int trunk_delete(uint16_t trunkid);                                                  /*trunkɾ��*/
struct trunk *trunk_lookup(uint16_t trunkid);                                    /*trunk����*/

/* ������ trunk �Ĳ������� */
int trunk_backup_switch(struct trunk *ptrunk, enum BACKUP_E status);         /* trunk��Ա�����л�*/
int trunk_linkstatus_set(struct trunk *trunk_entry);                            /*trunk link״̬����*/
int trunk_backup_port_select(struct trunk *trunk_entry);                    /*trunk����ģʽ���˿�����*/
int trunk_ecmp_port_select(struct trunk *trunk_entry);                      /*trunk���طֵ�ģʽ�����˿�����*/


/* �� trunk ��Ա�˿ڵĲ������� */
int trunk_port_add(uint16_t trunkid, struct trunk_port *pport);              /*trunk��Ա�˿����*/
int trunk_port_delete(uint16_t trunkid, uint32_t ifindex);                   /*trunk��Ա�˿�ɾ��*/
int trunk_port_up(uint16_t trunkid, uint32_t ifindex);                           /* trunk ��Ա�˿� up */
int trunk_port_down(uint16_t trunkid, uint32_t ifindex);                     /* trunk ��Ա�˿� down */
struct trunk_port *trunk_port_lookup(uint16_t trunkid, uint32_t ifindex);   /*trunk��Ա�˿ڲ���*/
struct trunk_port *trunk_port_init(uint16_t trunkid, uint32_t ifindex);             /*trunk��Ա�˿���Ϣ��ʼ��*/

/*trunk_cmd.c����غ���*/
void trunk_cmd_init(void);                                                      /*trunk�����г�ʼ��*/
int trunk_config_write(struct vty *vty);                                        /*trunk���ñ���*/
void trunk_config_show(struct vty *vty, struct trunk *trunk_entry);         /*trunk������Ϣ��ʾ*/
void trunk_info_show(struct vty *vty, struct trunk *trunk_entry);               /*trunk��Ϣ��ʾ*/
void trunk_member_alarm_report(uint16_t trunkid, uint32_t ifindex);     /*�ϱ�/����澯*/
int trunk_info_bulk_get(uint16_t trunkid, struct trunk pif[]);              /*���Ҵ��� trunkid ֮���Ӧ�� n ������ */
int trunk_port_info_bulk_get(uint16_t trunkid, struct trunk_port tport[]);   /*���Ҵ��� trunk�����г�Ա�˿� */

int trunk_speed_set(uint16_t trunkid);    /*trunk�ӿ���������*/
int trunk_config_write_all(struct vty *vty);

int trunk_comm_send_msg_wait_ack(uint16_t msg_subtype, uint8_t opcode,uint32_t length, void *pdata);

#endif

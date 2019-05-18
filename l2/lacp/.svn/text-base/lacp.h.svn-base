#ifndef HIOS_LACP_H
#define HIOS_LACP_H
#include "../l2_if.h"

#define LACP_FALSE 0
#define LACP_TRUE  1
#define LACP_MAC  {0x01, 0x80, 0xC2, 0x00, 0x00, 0x02}	/*LACP组播地址*/
#define LACP_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_LACP, fmt, ##__VA_ARGS__ ); \
    } while(0)

/*******************协议相关***********************/
#define LACP_TYPE                          		0x8809		/*协议类型*/
#define LACP_SUBTYPE                      		0x01		/*协议子类型 */
#define LACP_VERSION                        	0x01		/*LACP协议的版本号*/
#define LACP_ACTOR_INFO_TLV          			0x01		/*Actor TLV类型*/
#define LACP_ACTOR_INFO_LENGTH       			0x14		/*Actor的信息TLV长度 */
#define LACP_PARTNER_INFO_TLV        			0x02		/*Partner TLV类型 */
#define LACP_PARTNER_INFO_LENGTH     			0x14		/*Partner的信息TLV长度 */
#define LACP_COLLECTOR_INFO_TLV      			0x03		/*端口Collector TLV类型*/
#define LACP_COLLECTOR_INFO_LENGTH   			0x10		/*端口Collector信息TLV长度 */
#define LACP_COLLECTOR_MAX_DELAY            	0x00		/*端口Collector最大时延*/
#define LACP_TERMINATOR_TLV                 	0x00		/*端口Terminator TLV类型*/
#define LACP_TERMINATOR_LENGTH              	0x00		/*端口Terminator 长度*/

#define LACP_GET_ACTIVITY(p)          		((p>>0) & 1)	/*获取主动/被动方式*/
#define LACP_GET_TIMEOUT(p)           		((p>>1) & 1)	/*获取长/短超时*/
#define LACP_GET_AGGREGATION(p)          	((p>>2) & 1)	/*获取链路是否可聚合*/
#define LACP_GET_SYNCHRONIZATION(p)      	((p>>3) & 1)	/*获取端口聚合是否完成*/
#define LACP_GET_COLLECTING(p)           	((p>>4) & 1)	/*获取当前链路收包是否enable*/
#define LACP_GET_DISTRIBUTING(p)         	((p>>5) & 1)	/*获取当前链路发包是否enable*/
#define LACP_GET_DEFAULTED(p)             	((p>>6) & 1)	/*获取Actor使用的Partner信息来自管理员配置/接收的LACPDU*/
#define LACP_GET_EXPIRED(p)               	((p>>7) & 1)	/*获取Actor RX状态机处于超时/未超时状态*/

#define LACP_ACTIVITY_PASSIVE             0				/*被动方式*/
#define LACP_ACTIVITY_ACTIVE              1				/*主动方式*/

#define LACP_TIMEOUT_LONG                 	0				/*长超时*/
#define LACP_TIMEOUT_SHORT                	1				/*短超时*/
#define LACP_AGGREGATION_INDIVIDUAL      	0				/*独立链路，不可聚合*/
#define LACP_AGGREGATION_AGGREGATABLE    	1				/*链路可聚合*/
#define LACP_SYNCHRONIZATION_OUT_OF_SYNC 	0				/*端口聚合未完成*/
#define LACP_SYNCHRONIZATION_IN_SYNC     	1				/*端口聚合完成*/
#define LACP_COLLECTING_DISABLED         	0				/*当前链路收包disable*/
#define LACP_COLLECTING_ENABLED          	1				/*当前链路收包enable*/
#define LACP_DISTRIBUTING_DISABLED       	0				/*当前链路发包disable*/
#define LACP_DISTRIBUTING_ENABLED        	1				/*当前链路发包enable*/
#define LACP_NOT_DEFAULTED                	0				/*Actor使用的Partner信息来自接收的LACPDU*/
#define LACP_DEFAULTED                    	1				/*Actor使用的Partner信息来自管理员配置的默认值*/
#define LACP_NOT_EXPIRED                  	0				/*Actor RX状态机不处于超时状态*/
#define LACP_EXPIRED                      	1				/*Actor RX状态机处于超时状态*/

#define LACP_SET_ACTIVITY(p)          		(p |= (1<<0))	/*设置为主动方式*/
#define LACP_SET_TIMEOUT(p)           		(p |= (1<<1))	/*设置为短超时*/
#define LACP_SET_AGGREGATION(p)       		(p |= (1<<2))	/*设置为链路是可聚合的*/
#define LACP_SET_SYNCHRONIZATION(p)   	(p |= (1<<3))	/*设置为链路聚合完成*/
#define LACP_SET_COLLECTING(p)         	(p |= (1<<4))	/*设置为当前链路收包enable*/
#define LACP_SET_DISTRIBUTING(p)          (p |= (1<<5))	/*设置为当前链路发包enable*/
#define LACP_SET_DEFAULTED(p)             	(p |= (1<<6))	/*设置为Actor使用的Partner信息来自管理员配置的默认值*/
#define LACP_SET_EXPIRED(p)               	(p |= (1<<7))	/*设置为Actor RX状态机处于超时状态*/

#define LACP_CLR_ACTIVITY(p)          		(p &= ~(1<<0))	/*设置为被动方式*/
#define LACP_CLR_TIMEOUT(p)           		(p &= ~(1<<1))	/*设置为长超时*/
#define LACP_CLR_AGGREGATION(p)           	(p &= ~(1<<2))	/*设置为链路为独立链路，不可聚合*/
#define LACP_CLR_SYNCHRONIZATION(p)      	(p &= ~(1<<3))	/*设置为链路聚合未完成*/
#define LACP_CLR_COLLECTING(p)            	(p &= ~(1<<4))	/*设置为当前链路收包disable*/
#define LACP_CLR_DISTRIBUTING(p)         	(p &= ~(1<<5))	/*设置为当前链路发包disable*/
#define LACP_CLR_DEFAULTED(p)             	(p &= ~(1<<6))	/*设置为Actor使用的Partner信息来自接收的LACPDU*/
#define LACP_CLR_EXPIRED(p)               	(p &= ~(1<<7))	/*设置为Actor RX状态机不处于超时状态*/
	
#define  LACP_FAST_PERIODIC_TIME   		1				/*快发送间隔*/
#define  LACP_SHORT_TIMEOUT_TIME   		3				/*短超时时间*/

/*打印报文类型*/
enum LACP_DUMP_PKT
{
	LACP_DUMP_RECEIVE = 0,
	LACP_DUMP_SEND
};

/* lacp bpdu结构体*/
struct lacp_pkt
{
    uint8_t subtype;               							/*LACP协议子类型为0x01*/
    uint8_t version;		  						        /*LACP协议的版本号为0x01*/

    /* Actor information */
	uint8_t actor_tlv_type;	    						/*值为0x01*/
	uint8_t actor_infor_len; 								/*Actor的信息TLV长度，固定为0x14*/
    uint16_t  actor_spri;									/*Actor管理员配置的系统优化级*/
    uint8_t   actor_sys[MAC_LEN];							/*Actor的系统MAC地址*/
    uint16_t  actor_key;									/*Actor的运行key*/
    uint16_t  actor_ppri;									/*Actor的端口优先级*/
    uint16_t  actor_port;									/*Actor的端口号*/
    uint8_t   actor_state;									/*Actor的端口状态*/
	uint8_t res_3_1[3];										/*保留*/
	
    /* Partner information */
	uint8_t partner_tlv_type;     							/*值为0x02*/
	uint8_t partner_infor_len;								/*Partner的信息TLV长度，固定为0x14*/
    uint16_t  partner_spri;								/*Partner管理员配置的系统优化级*/
    uint8_t   partner_sys[MAC_LEN];						/*Partner的系统MAC地址*/
    uint16_t  partner_key;									/*Partner的运行key*/
    uint16_t  partner_ppri;								/*Partner的端口优先级*/
    uint16_t  partner_port;								/*Partner的端口号*/
	uint8_t   partner_state;								/*Actor认为的Partner状态*/
	uint8_t res_3_2[3];										/*保留*/
	
    /* Collector information */
	uint8_t col_tlv_type;	 								/*值为0x03*/
	uint8_t col_infor_len; 								/*端口Collector信息TLV长度,固定为0x10*/
    uint16_t  col_max_delay;								/*发送端的帧从聚合解析层到MAC客户端的发送延迟*/
	uint8_t res_12[12];										/*保留*/

	/* Terminator information */
	uint8_t ter_tlv_type;	      							/*值为0x00*/
	uint8_t ter_len;	          							/*使用固定值0x00*/
	uint8_t res_50[50];	          							/*保留*/
};

uint32_t	lacp_recv_dbg;									/*lacp接收报文开关*/
uint32_t	lacp_send_dbg;									/*lacp发送报文开关*/

void lacp_pkt_debug_init(void);					/*lacp报文调试开关初始化*/
void lacp_pkt_rx_register(void);					/*lacp收包注册*/
//int lacp_timer_start(struct thread *thread);		/*lacp定时器启动*/	
int lacp_timer_start(void *para);
int lacp_tx (struct trunk *trunk_entry, uint32_t ifindex);	/*lacp协议报文封装*/
void lacp_fsm(struct trunk *trunk_entry, uint32_t ifindex, enum LACP_EVENT event);	/*lacp状态机*/
void lacp_pkt_tx(struct trunk *trunk_entry, uint32_t ifindex);/*lacp报文发送*/
int lacp_pkt_rx (struct l2if *l2_if, struct pkt_buffer *pkt); /*lacp报文接收*/
void lacp_port_delect_set(struct trunk *trunk_entry, uint32_t ifindex);
void lacp_port_linkdown_timeout_set(struct trunk *trunk_entry, uint32_t ifindex);
int lacp_backup_master_port_select(struct trunk *trunk_entry, struct trunk_port *tport);
int lacp_backup_failback_timer(void *para);

#endif

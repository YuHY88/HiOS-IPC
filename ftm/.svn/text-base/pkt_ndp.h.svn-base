/*
*       ndp packet process
*/

#ifndef PKT_NDP_H
#define PKT_NDP_H

#include <lib/types.h>
#include "pkt_icmp.h"
#include "ftm_ndp.h"

//节点常量,参考RFC4861
#define NDP_STALE_TIMER      30000    //邻居可达时间，超时进入STALE状态
#define NDP_RETRY_TIMER      1000     //NS报文重传时间间隔
#define NDP_FIRST_PROBE_TIME 5000000  //发送第一个探测报文前的等待时间
#define NDP_ANYCAST_DELAY    1000000  //发送任播报文前的延时

#define NDP_ADVERTISE_MAX   3        //最大非请求邻居状态通告次数
#define NDP_SOLICIT_MAX     3        //最大 solicit 请求次数

#define NDP_HOP_LIMIT       255       //NDP消息默认 ttl


/*icmpv6 message type*/
#define  NDP_ROUTER_SOLICI         133
#define  NDP_ROUTER_ADVERTISE      134
#define  NDP_NEIGHBOR_SOLICIT      135
#define  NDP_NEIGHBOR_ADVERTISE    136
#define  NDP_REDIRECT              137

/*NDP options type*/
#define NDP_OPT_SRC_LADDR     1
#define NDP_OPT_TARGET_LADDR  2
#define NDP_OPT_PREFIX_INFO   3
#define NDP_OPT_REDIRCT_HDR   4
#define NDP_OPT_MTU           5


#define NDP_PKT_LEN (sizeof(struct ndp_pkt) + sizeof(struct ndp_opt))



/*选项字段
Type 1    源链路层地址
	 2    目标链路层地址
length：选项长度（以8byte为单位） 
*/

/*源/目标链路层地址选项*/
struct ndp_opt
{
	uint8_t type;
	uint8_t len;
	uint8_t mac[MAC_LEN];
};


/* ND 报文格式 */
struct ndp_pkt
{
	struct icmpv6_hdr nd_hdr; 
    
#if BIG_ENDIAN_ON
    uint32_t  router:1,
              solicited:1,
              override:1,
              reserved:29;
#else
    uint32_t
              reserved:5,
              override:1,
              solicited:1,
              router:1,
              reserved2:24;
#endif
	uint8_t           dip[16];
	uint8_t           option[];  //后面可跟源/目标链路层地址选项
}__attribute__((packed));


/* NDP 报文 opcodes. */
enum NDP_OPCODE
{
    NDP_OPCODE_INVALID = 0,
    NDP_OPCODE_NSOLICIT,     /* NDP request	*/
    NDP_OPCODE_NADVERTISE,   /* NDP reply	*/
    NDP_OPCODE_RSOLICT,
    NDP_OPCODE_RADVERTISE,
    NDP_OPCODE_REDIRECT,
    NDP_OPCODE_MAX = 8
};


/*NDP 状态机事件类型 */
enum NDP_EVENT
{
	NDP_EVENT_INVALID = 0,
	NDP_EVENT_MISS,
	NDP_EVENT_RCV_NSOLICIT,
	NDP_EVENT_RCV_NADVERTISE,
	NDP_EVENT_RCV_RSOLICI,
    NDP_EVENT_RCV_RADVERTISE,
    NDP_EVENT_RCV_REDIRECT,
    
    NDP_EVENT_PKT_SEND,//引用nd 表项报文发送事件
    NDP_EVENT_REACHABLE_CONFIRM,//可达性证实事件
    NDP_EVENT_REACHABLE_TIMEOUT,//可达性时间到
    NDP_EVENT_REACHCONFIRM_TIMEOUT,//可达性确认时间到
	NDP_EVENT_STALE_TIMEOUT,
	
	NDP_EVENT_MAX = 15
};


//查找nd表项失败，触发ndp_miss添加INCOMPLETE表项
int ndp_miss(struct ipv6_addr *pdip, uint16_t vpn, uint32_t ifindex);

//ndp协议状态机
int ndp_fsm(struct ndp_neighbor *pndp, uint8_t dip[16], enum NDP_EVENT event_type, uint8_t rso_flag);


//发送ndp协议报文
int ndp_send(struct ndp_neighbor *pnd_entry,uint8_t opcode,uint8_t sip[16],uint8_t *smac);

//接收ndp协议报文
int ndp_recv(struct pkt_buffer *pkt);


//ndp通知hal注册包类型
int ndp_register(void);

//邻居可达性确认
int ndp_reachable_confim(struct ipv6_addr *pdip, uint16_t vpn, uint32_t ifindex);

/*delay状态等待可达性确认定时器*/
int ndp_delay_status_timer( );


#endif /* PKT_NDP_H */




/*
*       ndp global config and data
*/

#ifndef NDP_H
#define NDP_H
#include <lib/types.h>
#include <lib/route_com.h>

/*nd 表项老化规格*/
#define NDP_REACHABLE_TIME         30*60  /* 动态表项老化时间 */
#define NDP_STALE_TIME              3*60   /*动态表项stale 状态停留时间*/
#define NDP_INCOMPLETE_AGE_TIME 5
#define NDP_PROBE_AGE_TIME      60
#define NDP_TIMER            1*10   /* 定时器周期  */
#define NDP_DELAY_FIRST_PROBE_TIME 1 /*等待可达性确认的时间，单位秒*/


/* neighbor 表项规格 */
#define ND_TOTAL_NUM  1024   /* nd 表项总数，包含动态和静态 */
#define ND_STATIC_NUM 256    /* 静态 nd 表项数量 */


/*NDP 错误码*/
#define NDP_HASH_BUCKET_FULL 1 


/*ndp 模块宏*/
#define NDP_ENABLE_DEF   1
#define NDP_DISABLE_DEF  0

#define NDP_NO_PROXY_DEF 0
#define NDP_PROXY_DEF    1


/* neighbor 表项状态*/
enum NDP_STATUS
{
	NDP_STATUS_INVALID = 0,
	NDP_STATUS_INCOMPLETE,
	NDP_STATUS_REACHABLE,
	NDP_STATUS_STALE,
	NDP_STATUS_DELAY,
	NDP_STATUS_PROBE,
	NDP_STATUS_PERMANENT,
	NDP_STATUS_STATIC,
	NDP_STATUS_MAX = 8
};


/* ndp 的 subtype */
enum NDP_INFO
{
	NDP_INFO_INVALID = 0,
	NDP_INFO_STATIC_ND,       /* 静态 neighbor */
	NDP_INFO_LEARN_LIMIT,
	NDP_INFO_NUM_LIMIT,       /* 学习数量限制 */
	NDP_INFO_REACH_TIME,      /*邻居可达时间*/
	NDP_INFO_STALE_TIME,      /*邻居无效状态时间*/
	NDP_INFO_AGENT,
	NDP_INFO_FAKE_TIME,
    NDP_INFO_MAX = 16
};


/* 邻居表项的 key */
struct ndp_key
{
	uint8_t   ipv6_addr[16];
	uint32_t  vpnid;
	uint32_t  ifindex;     /* output interface */
};


/* 邻居表项 */
struct ndp_neighbor
{
	struct ndp_key   key;	
	enum NDP_STATUS  status;      /* 邻居状态 */
	uint32_t         port;        /* 用于 vlanif 接口的物理成员口 */
	uint32_t         nd_index;    /* nd 索引 */
	uint8_t          mac[MAC_LEN];	
	uint16_t         time;        /* 剩余老化时间 */
    uint8_t          count;       /* 发送 ndp 请求的次数 */
	uint8_t          isrouter;    /* 标识此邻居是路由器还是主机 */	
	uint8_t          pad[2];
};


/* ndp 的全局参数 */
struct ndp_global
{
    uint32_t num_limit;     /* 动态 ndp 学习限制 */
    uint16_t reach_time;      /* 动态 nd 表项可达时间 */
    uint16_t stale_time;    /*动态nd 表项stale 状态等待时间*/
    uint16_t fake_time;     /* incomplete entry age time*/
	uint16_t timer;         /* ndp timer 的周期 */
	uint32_t count;         /* 动态 ndp 数量 */
	uint32_t num_static;    /* 静态 ndp 数量 */
	uint32_t num_complete;  /* 全连接的 ndp 数量*/
	uint32_t num_incomplete;/* 半连接的 ndp 数量*/
};



#endif /* NDP_H */


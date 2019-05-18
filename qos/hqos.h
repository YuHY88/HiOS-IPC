
#ifndef HIOS_HQOS_H
#define HIOS_HQOS_H

#include <lib/types.h>
#include <lib/qos_common.h>
#include <qos/qos_main.h>


#define HQOS_WRED_PROFILE_NUM    10    /* WRED 模板数量 */
#define HQOS_QUEUE_PROFILE_NUM   100   /* 队列模板数量 */
#define HQOS_WRED_DROP_LOW    1   	   /* WRED丢弃下限 */
#define HQOS_WRED_DROP_HIGH   100      /* WRED丢弃上限 */
#define HQOS_QUEUE_MIN    0   		   /* 最小队列 */
#define HQOS_QUEUE_MAX    7     	   /* 最大队列 */
#define HQOS_QUEUE_WEIGHT_MIN    0     /* 队列最小权重 */
#define HQOS_QUEUE_WEIGHT_MAX    127   /* 队列最大权重 */
#define HQOS_RATE_MIN    0             /* 队列最小权重 */
#define HQOS_RATE_MAX    10000000      /* 队列最小权重 */


/* 大数量回显时判断条件，一次最多回显 60 行 */
#define PROFILE_LINE_CHECK(line, line_num)\
    if ((line) == ((line_num)+60)) {(line)=0; return CMD_CONTINUE;}\
    if (((line)<((line_num)+60)) && ((++(line))>line_num))

/* wred rule repeat check */
#define WRED_RULE_REPEAT_CHECK(_pold, _pnew)         \
       ((_pold->drop_low == _pnew->drop_low)         \
    && (_pold->drop_high == _pnew->drop_high)        \
    && (_pold->drop_ratio == _pnew->drop_ratio))


/* hqos 队列调度类型 */
enum HQOS_SCHEDULER
{
	HQOS_SCHEDULER_INVALID = 0,	
	HQOS_SCHEDULER_PQ,
	HQOS_SCHEDULER_WRR,	
	HQOS_SCHEDULER_WDRR
};


enum WRED_PROTOCOL
{
	WRED_PROTOCOL_TCP = 0,
	WRED_PROTOCOL_NON_TCP = 1,
	WRED_PROTOCOL_ALL
};


/* wred 丢弃规则 */
struct wred_rule
{
    enum QOS_COLOR color;
    enum WRED_PROTOCOL protocol;
	uint8_t drop_low;     /* 丢弃低门限 1-100% */	
	uint8_t drop_high;    /* 丢弃高门限 1-100% */
    uint8_t drop_ratio;   /* 丢弃比例 1-100% */
	uint8_t pad;
};


/* wred 模板数据结构 */
struct wred_profile
{
    uint8_t id;                   /* profile ID */	
    uint8_t ref_cnt;              /* 引用计数 */
    uint8_t pad[2];               /* for 4 byte align */
    struct wred_rule wred[3][2];  /* 3 种颜色的丢弃规则 */
};


/* hqos 队列的数据结构 */
struct hqos_queue
{
    uint8_t  id;               /* 队列 ID */
	uint8_t  weight;           /* 队列权重 */	
	uint8_t  wred_id;          /* wred 模板 id */	
    uint8_t  pad;
    uint32_t cir;
    uint32_t pir;
};


/* hqos 队列模板数据结构 */
struct queue_profile
{
    uint8_t  id;                /* profile ID */	
    uint8_t  pad;               /* for 4 byte align */	
    uint16_t ref_cnt;           /* 引用计数 */
	enum HQOS_SCHEDULER scheduler;  /* 调度方式*/
    struct hqos_queue queue[8];    /* 8 个队列 */
};



extern struct wred_profile   *gwred_profile[HQOS_WRED_PROFILE_NUM]; /* 全局 wred 模板，使用数组存储 */
extern struct queue_profile  *gque_profile[HQOS_QUEUE_PROFILE_NUM]; /* 全局队列模板，使用数组存储 */


void hqos_table_init(void);
struct hqos_t *hqos_table_lookup ( uint32_t index );
int hqos_table_create ( struct hqos_t *phqos );
int hqos_table_add ( struct hqos_t *phqos );
int hqos_table_delete ( uint16_t index );
void hqos_wred_profile_attached (struct wred_profile *pwred, int flag);
void hqos_queue_profile_attached (struct queue_profile *pqueue, int flag);



/* wred 的函数 */
void hqos_wred_profile_init(void);             /* 初始化 wred 模板 */
int hqos_wred_profile_add(struct wred_profile *pwred); /* 添加 wred profile */
int hqos_wred_profile_delete(uint8_t wred_id);       /* 删除 wred profile  */
struct wred_profile *hqos_wred_profile_lookup(uint8_t wred_id);/* 查找 wred profile */
int hqos_wred_profile_set_rule(struct wred_profile *pwred, struct wred_rule *prule);
int hqos_wred_profile_get_bulk ( uint8_t index, struct wred_profile *wred_array );


/* hqos 队列的函数 */
void hqos_queue_profile_init(void);             /* 初始化队列模板 */
int hqos_queue_profile_add(struct queue_profile *pque_profile); /* 添加 queue profile */
int hqos_queue_profile_delete(uint8_t profile_id);       /* 删除 queue profile  */
struct queue_profile *hqos_queue_profile_lookup(uint8_t profile_id);/* 查找 queue profile */
int hqos_queue_profile_set_queue(struct queue_profile *pque_profile, struct hqos_queue *pqueue);
int hqos_queue_profile_set_scheduler(struct queue_profile *pque_profile, enum HQOS_SCHEDULER sheculer);
int hqos_queue_profile_get_bulk ( uint8_t index, struct queue_profile *queue_array );


void hqos_cmd_init(void);


#endif




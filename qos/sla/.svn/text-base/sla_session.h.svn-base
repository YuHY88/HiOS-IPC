/*
*   管理 sla 会话
*/
#ifndef HIOS_SLA_H
#define HIOS_SLA_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/ether.h>
#include <lib/oam_common.h>
#include <ftm/pkt_eth.h>
#include <lib/linklist.h>
#include <ftm/pkt_udp.h>
#include <lib/msg_ipc.h>


#define SLA_TEST_IS_RUNNING(vty, state)\
    if((state) == SLA_STATUS_ENABLE)\
    {\
        vty_error_out(vty, "Can't modify configuration when sla test is running.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }


#define SLA_NUM                (128)
#define SLA_DEF_DURATION       (10)
#define SLA_DEF_PKT_RATE       (1000)
#define SLA_DEF_PKT_SIZE       (512)
#define SLA_DEF_IP_TTL         (255)
#define SLA_DEF_COS            (5)
#define SLA_DEF_DSCP           (46)
#define SLA_Y1564_CONFIG_STEP  (8)

//#define SLA_PKT_RATE_ENLAGE    (1000)
#define SLA_LATENCY_ENLAGE     (1000)
#define SLA_PKT_LOSS_ENLAGE    (10000)/*放大10000倍*/
#define SLA_PORT_BANDWIDTH_MAX (1000000) //单位Kbps = 1G

/*默认服务验收标准*/
#define SLA_SAC_FLR_BENCHMARK (100000) /*十万分之一*/
#define SLA_SAC_DEF_FLR (1000) /*丢包率:十万分之一千*/
#define SLA_SAC_DEF_FTD (200) /*延时:us*/
#define SLA_SAC_DEF_FDV (20) /*抖动:us*/

#define SLA_TEST_FLOW_MAX   (8) /* 支持sla测试流的最大数 */
#define SLA_SESSION_ID_MAX		(65535)

/* IPC 的 subtype */
enum SLA_SUBTYPE_E
{
	SLA_SUBTYPE_INVALID = 0,
	SLA_SUBTYPE_SESS,
	SLA_SUBTYPE_SESS_RESULT,
	SLA_SUBTYPE_SESS_IF_INFO
};

/* SLA 测量状态*/
enum SLA_MEASURE_STATUS
{
	SLA_MEASURE_DISABLE = 0,
	SLA_MEASURE_ENABLE = 1
};

/* SLA 类型 */
enum SLA_TYPE
{
	SLA_TYPE_INVALID = 0,	
	SLA_TYPE_INTERFACE,	 /* 接口 */
	SLA_TYPE_PW,	     /* PW */
	SLA_TYPE_LSP,	     /* LSP */
	SLA_TYPE_MAX = 8
};

/* SLA 协议类型 */
enum SLA_PROTO
{
	SLA_PROTO_INVALID = 0,	
	SLA_PROTO_RFC2544,	 /* RFC2544 */
	SLA_PROTO_Y1564,	 /* Y.1564 */
	SLA_PROTO_MAX = 8
};

/* SLA 会话状态 */
enum SLA_STATUS
{
	SLA_STATUS_DISABLE = 0,   /* 未使能状态 */	
	SLA_STATUS_ENABLE,    	/* 使能状态 */	
	SLA_STATUS_UNFINISH,      /* 测试未完成状态 */
	SLA_STATUS_FINISH 	      /* 测试完成状态 */
};


/* Y.1564测试阶段:配置测试 or 性能测试 */
enum SLA_Y1564
{
	SLA_Y1564_TESTTYPE_INVALID = 0, /* 没有配置测试类型*/
	SLA_Y1564_CONFIGURATION,  /* 仅进行配置测试 */
	SLA_Y1564_PERFORMANCE     /* 仅进行性能测试 */
};




/* sla 以太报文 */
struct sla_pkt_eth
{
	uchar dmac[ETH_ALEN];
	uchar smac[ETH_ALEN];
	uint16_t cvlan;
	uint16_t c_tpid;
	uint16_t svlan;
	uint16_t s_tpid;
	uint8_t  c_cos;
    uint8_t  s_cos;
};

/* sla ip 报文 */
struct sla_pkt_ip
{
	uint32_t sip;	
	uint32_t dip;
	uint16_t sport;	
	uint16_t dport;
	uint8_t  dscp;
	uint8_t  ttl;
    uint8_t  pad[2];
};

/* sla 会话的基本数据结构 */
struct sla_info
{
    enum SLA_PROTO    proto;             /* 协议类型 */
	enum SLA_STATUS   state; 	         /* 会话状态 */
	enum SLA_Y1564    sla_y1564_stage;   /* y1564测试阶段 */
	uint16_t          sla_2544_size[7];  /* 2544连续测字节 */
	uint16_t          session_id;        /* 会话 id */
	uint16_t		  pkt_size;          /* 测试报文长度:rfc2544下发的单个字节，1564测试报文字节 */
	uint32_t		  pkt_rate_upper;    /* 测试报文速率上限:kbps */
	uint32_t		  pkt_rate_lower;    /* 测试报文速率下限:kbps */
	uint32_t		  cir;
	uint32_t		  eir;
	uint32_t		  duration;          /* 测试间隔 */
	uint8_t		      lm_enable;         /* 丢包测试 */
	uint8_t		      dm_enable;         /* 延时测试 */
	uint8_t		      throughput_enable; /* 吞吐量测试 */
	uint8_t           traf_policing_enable;/* 流量监管测试使能 */
	uint32_t          sac_loss;          /* 验收标准:丢包率 */
	uint32_t          sac_delay;         /* 验收标准:延迟 */
	uint32_t          sac_jitter;        /* 验收标准:抖动 */
};

struct sla_2544_measure
{
	uint64_t        pkt_tx[SLA_Y1564_CONFIG_STEP];      /*发包计数*/
	uint64_t        pkt_rx[SLA_Y1564_CONFIG_STEP];		/*收包计数*/

	uint32_t        throughput[SLA_Y1564_CONFIG_STEP];  /* 吞吐量 */
	
	uint32_t        lm[SLA_Y1564_CONFIG_STEP];	    /* 最大丢包率 */
	//uint32_t        lm_mean[SLA_Y1564_CONFIG_STEP];     /* 平均丢包率 */
	//uint32_t        lm_min[SLA_Y1564_CONFIG_STEP];	    /* 最小丢包率 */
	//uint64_t		lm_sum[SLA_Y1564_CONFIG_STEP];	    /* 丢包率和 */
	
	uint32_t        dm_max[SLA_Y1564_CONFIG_STEP];	    /* 最大延时 */
	uint32_t        dm_mean[SLA_Y1564_CONFIG_STEP];     /* 平均延时 */
	uint32_t        dm_min[SLA_Y1564_CONFIG_STEP];	    /* 最小延时 */
	//uint64_t		dm_sum[SLA_Y1564_CONFIG_STEP];	    /* 延时和 */
};

struct sla_1564_measure
{
	/* 配置测试：步进cir测试结果 */
	enum SLA_STATUS cir_flag;  /* 步进cir测试状态 */
	enum SLA_STATUS eir_flag;  /* eir测试状态 */
	enum SLA_STATUS traf_flag; /* 流量监管测试状态 */
	enum SLA_STATUS perf_flag; /* 性能测试状态 */
	
	uint32_t c_cir_ir[4];
	uint32_t c_cir_lm[4];

	uint32_t c_cir_dm_max[4];
	uint32_t c_cir_dm_mean[4];
	uint32_t c_cir_dm_min[4];

	uint32_t c_cir_jm_max[4];
	uint32_t c_cir_jm_mean[4];
	uint32_t c_cir_jm_min[4];

	/* 配置测试：eir测试结果 */
	uint32_t c_eir_ir;
	uint32_t c_eir_lm;

	uint32_t c_eir_dm_max;
	uint32_t c_eir_dm_mean;
	uint32_t c_eir_dm_min;

	uint32_t c_eir_jm_max;
	uint32_t c_eir_jm_mean;
	uint32_t c_eir_jm_min;

	uint8_t result_counter;
	/* 配置测试：流量监管测试结果 */
	uint32_t c_traf_ir;
	uint32_t c_traf_lm;

	uint32_t c_traf_dm_max;
	uint32_t c_traf_dm_mean;
	uint32_t c_traf_dm_min;
	
	uint32_t c_traf_jm_max;
	uint32_t c_traf_jm_mean;
	uint32_t c_traf_jm_min;

	/* 性能测试结果 */
	uint32_t p_ir;
	uint32_t p_lm;
	
	uint32_t p_dm_max;
	uint32_t p_dm_mean;
	uint32_t p_dm_min;

	uint32_t p_jm_max;
	uint32_t p_jm_mean;
	uint32_t p_jm_min;
};


struct sla_measure_t
{
	struct sla_2544_measure sla_2544_result;
	struct sla_1564_measure sla_1564_result;
};


#if 0

/* sla 测量的结果 */
struct sla_measure_t
{
	uint32_t        count;	        /* 测试次数 */

	uint64_t        pkt_tx[SLA_Y1564_CONFIG_STEP];      /*发包计数*/
	uint64_t        pkt_rx[SLA_Y1564_CONFIG_STEP];		/*收包计数*/

	uint32_t        throughput[SLA_Y1564_CONFIG_STEP];  /* 吞吐量 */
	
	uint32_t        lm_max[SLA_Y1564_CONFIG_STEP];	    /* 最大丢包率 */
	uint32_t        lm_mean[SLA_Y1564_CONFIG_STEP];     /* 平均丢包率 */
	uint32_t        lm_min[SLA_Y1564_CONFIG_STEP];	    /* 最小丢包率 */
	uint64_t		lm_sum[SLA_Y1564_CONFIG_STEP];	    /* 丢包率和 */
	
	uint32_t        dm_max[SLA_Y1564_CONFIG_STEP];	    /* 最大延时 */
	uint32_t        dm_mean[SLA_Y1564_CONFIG_STEP];     /* 平均延时 */
	uint32_t        dm_min[SLA_Y1564_CONFIG_STEP];	    /* 最小延时 */
	uint64_t		dm_sum[SLA_Y1564_CONFIG_STEP];	    /* 延时和 */
	
	uint32_t        jm_max[SLA_Y1564_CONFIG_STEP];	    /* 最大延时 抖动*/
	uint32_t        jm_mean[SLA_Y1564_CONFIG_STEP];     /* 平均延时 抖动*/
	uint32_t        jm_min[SLA_Y1564_CONFIG_STEP];	    /* 最小延时 抖动*/
	uint64_t		jm_sum[SLA_Y1564_CONFIG_STEP];	    /*抖动和*/
};
#endif

/* SLA session 数据结构 */
struct sla_session
{
	struct sla_info      info;       /* 会话的基本信息 */
	struct sla_pkt_ip    pkt_ip;	
	struct sla_pkt_eth   pkt_eth;
	struct sla_measure_t result;
};

/* sla if 的数据结构 */
struct sla_if_entry
{
	uint16_t start_id; /*开始session id*/
	uint16_t end_id;   /*结束session id*/
	uint32_t ifindex;
	enum SLA_STATUS  state; /* 会话状态 */
};

struct sla_loopback
{
	struct list *if_list;
	uint8_t g_sla_loopback_count;
	uint8_t pad[3];
};


extern struct hash_table sla_session_table; /* sla 会话 hash 表，用 session_id 作为 key */
extern struct sla_if_entry sla_entry;/* 接口表*/
extern struct sla_loopback if_loopback;

/* 对 sla 会话的操作 */
void sla_session_table_init (unsigned int size);/* sla 会话表初始化 */
void sla_session_send_down(uint16_t start_id, uint16_t end_id, enum IPC_OPCODE opcode, enum SLA_STATUS status);
struct sla_session *sla_session_create(uint16_t sess_id);/* 创建一个会话 */
int sla_session_add(struct sla_session *psess); /* 添加到 hash 表 */
int sla_session_delete(uint16_t sess_id);     /* 删除一个会话 */
struct sla_session *sla_session_lookup(uint16_t sess_id); /* 查找一个会话 */
void sla_session_finish(struct sla_session *psess, struct sla_measure_t *presult); /* 测量结束，芯片上报测试结果 */
void sla_session_enable(struct sla_if_entry *pentry); /* 会话 enable */
void sla_session_disable(struct sla_if_entry *pentry); /* 会话 disable */
void sla_cli_init(void);
sint8 *sla_display_format(uint32_t decimal, uint8_t format, uint8_t precision);
void sla_session_send_event(uint32_t ifindex);
int sla_config_write (struct vty *vty);
void sla_if_delete(uint32_t ifindex);


int sla_result_debug(int num, int pkt_zise, struct sla_measure_t *sla_result);
#endif

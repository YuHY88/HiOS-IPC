/*
*   define of CES interface
*/

#ifndef HIOS_CES_IF_H
#define HIOS_CES_IF_H


#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <ces/stm_if.h>
#include <lib/devm_com.h>

#define CES_ENABLE 1
#define CES_DISABLE 0

#define TDM_LABLE_GET(ifindex) ((ifindex >> 12) & 0x1fff)

#define CES_DEBUG_TDM  0x01    //for tdm interface configuration
#define CES_DEBUG_STM  0x02    //for stm interface configuration    
#define CES_DEBUG_OTHER     0x04    //reserve
#define CES_DEBUG_ALL       0x07    //for all debug info

/* CES 信息类型 */
enum CES_INFO
{
	CES_INFO_INVALID = 0,
	CES_INFO_ENCAP,
	CES_INFO_FRAME,
	CES_INFO_RTP,
	CES_INFO_MODE,
	CES_INFO_JITBUFFER,
	CES_INFO_PRBS,
	CES_INFO_TIMESLOT,
	CES_INFO_LOOPBACK,
	CES_INFO_SENDCLOCK,
	CES_INFO_RECVCLOCK,
    CES_INFO_CONFIGRESTORE,
    CES_INFO_J2
};


/* CES 成帧和非成帧模式 */
enum CES_MODE
{
    CES_MODE_INVALID = 0,
    CES_MODE_UNFRAMED,       /*非成帧*/
    CES_MODE_FRAMED,         /*成帧*/
    CES_MODE_MULTIFRAMED    /*成复帧*/
};


/* CES 封装协议 */
enum CES_ENCAP
{
    CES_ENCAP_INVALID = 0,
    CES_ENCAP_SATOP = 0x4,
    CES_ENCAP_CESOPSN = 0x14,
    CES_ENCAP_MAX
};


/* CES 成帧模式下的帧格式 */
enum CES_FRAME
{
    CES_FRAME_INVALID = 0,
    CES_FRAME_PCM30,
    CES_FRAME_PCM30_CRC,
    CES_FRAME_PCM31,
    CES_FRAME_PCM31_CRC,
    CES_FRAME_MAX = 8
};


/* CES 时钟提取方式 */
enum CES_SEND_CLK
{
    CES_CLK_LOOPBACK = 0,
    CES_CLK_PHY = 0x1,
    CES_CLK_DCR = 0x2,
    CES_CLK_ACR = 0x3,
    CES_CLK_LOCAL = 0x4
};

/*CES接收时钟*/
enum CES_RECEIVE_CLK
{
    CES_RECEIVE_CLK_LINE = 0,
    CES_RECEIVE_CLK_EXTERNAL = 0x1,
    CES_RECEIVE_CLK_LOCAL = 0x2
};

/*CES环回方式*/
enum CES_LOOPBACK
{
    CES_LB_NOLOOPBACK = 0,
    CES_LB_INTERBAL,
    CES_LB_EXTERNAL
};

/*CES告警类型*/
enum CES_ALARM_TYPE
{
	CES_PCK_FULLLOS = 0,     	/*接收数据包全部丢失*/
	CES_PCK_LOS,				/*接收数据包丢失*/
	CES_PCK_FULLLOS_REMOTE,   /*远端接收数据包丢失*/
	CES_LOS_AIS_LOCAL,			/*LOS/AIS*/
	CES_E1_LOS_REMOTE,        /*远端无信号*/
	CES_E1_LOF,				   	/*LOF*/
	CES_E1_LOMF,				/*LOFM*/
	CES_E1_CRCERR,				/*复帧CRC*/
    CES_E1_HDBE_ERR,         /*HDB3-信号劣化*/
    CES_E1_J2_ERR               /*收到的低阶通道踪迹字节J2与发送不一致*/
};

/*ces告警信息*/
struct ces_alarm
{
    uint8_t pck_fulllos;            /*约定时间内无包，高告警(Nopacket)*/
    uint8_t pck_los;                /*丢包告警，高告警(PacketLoss)*/
    uint8_t pck_fulllos_r;          /*远端以太网包全丢失告警信号，高为告警(Remote-PacketLoss)*/
    uint8_t los_ais_local;          /*本端E1/T1 los或者ais状态指示，“00”表示los，“11”表示ais，其他情况表示正常*/
    uint8_t e1_los_r;               /*远端E1的错误指示，高有告警(RemoteErr)*/
    uint8_t e1_lof;                 /*成帧模式下LOF告警，高有告警(Lof)*/
    uint8_t e1_lomf;                /*成复帧模式下LOMF告警，高有告警(Lofm)*/
    uint8_t e1_crcerr;              /*成复帧模式下crc告警，高有告警(Crc)*/
    uint8_t e1_hdbe_err;             /*hdb3/b8zs，err */
};

/*ces相关计数*/
struct ces_count
{
    uint32_t hdb3_count;            /*hdb3错误计数*/
    uint32_t es_count;             /*误码秒计数*/
    uint32_t ses_count;            /*严重误码秒计数*/
    uint32_t uas_count;            /*不可用秒计数，连续10个ses,ses-10且uas++直至产生es打断*/
};

/*ces相关计数*/
struct ces_count_info
{
    uint32_t hdb3_count;           /*hdb3错误计数*/
    uint32_t es_count;             /*误码秒计数*/
    uint32_t ses_count;            /*严重误码秒计数*/
    uint32_t uas_count;            /*不可用秒计数，连续10个ses,ses-10且uas++直至产生es打断*/
    uint32_t prbs_count;           /*prbs计数*/
    uint16_t clock_sync_count;     /*时钟同步(在0~20为保持，20~80为跟踪，其它为自由震荡)*/
};


/* CES 数据结构 */
struct ces_entry
{
    enum CES_ENCAP    encap; /* CES 封装协议 */
    enum CES_FRAME    frame; /* CES 成帧模式下的帧格式 */
    enum CES_MODE     mode;   /* CES 成帧和非成帧模式 */
    enum CES_SEND_CLK clock;   /* CES 时钟提取方式 */
    enum CES_RECEIVE_CLK recvclock;/*CES接收时钟(0:无效，1:设置)*/
    enum CES_LOOPBACK loopback;/*CES环回方式*/
    uint8_t  jitbuffer;    /*抖动时延*/
    uint8_t  rtp;          /* 是否使能实时协议(1: enable, 0: disable) */
    uint8_t  prbs;         /* 1: enable, 0: disable */
    uint8_t  priority;     /* 接口优先级 */
	uint32_t timeslot;     /* 用 bitmap 表示，每个 bit 代表一个时隙 */
	uint32_t label;        /* tdm 接口的保留标签 */
	uint32_t lsp_index;    /* tdm 接口的保留标签索引 */
	uint32_t frame_num;    /*tdm接口frame num*/
    char     j2[16];       /*低阶通道踪迹字节，对于SDH格式取值范围为1～15个字符。*/

    struct ces_alarm ces_alarm;   /*ces告警信息*/
};


/* CES 接口 */
struct ces_if
{
	 uint32_t ifindex;
	 uint8_t down_flag;      /* 0: up, 1: down */		 
     uint8_t j2_pad;         /*命令行配置j2时是否选择用0补齐*/   
	 uint8_t pad[2];
	 struct ces_entry ces;
};

struct hash_table ces_if_table;

//store alarm status of all ports, use bit to represent port num
struct ces_fpga_alarm_sc01q
{
    uint16_t pck_fulllos[8];            /*约定时间内无包，高告警(Nopacket)*/
    uint16_t pck_los[8];                /*丢包告警，高告警(PacketLoss)*/
    uint16_t pck_fulllos_r[8];          /*远端以太网包全丢失告警信号，高为告警(Remote-PacketLoss)*/
    uint16_t e1_los_r[8];               /*远端E1的错误指示，高有告警(RemoteErr)*/
    uint16_t e1_lof[8];                 /*成帧模式下LOF告警，高有告警(Lof)*/
    uint16_t e1_lomf[8];                /*成复帧模式下LOMF告警，高有告警(Lofm)*/
    uint16_t e1_crcerr[8];              /*成复帧模式下crc告警，高有告警(Crc)*/
    uint16_t los_ais_local[16];          /*本端E1/T1 los或者ais状态指示，“00”表示los，“11”表示ais，其他情况表示正常*/
};

//for ipc msg
struct ces_sc01q_alarm_info
{
    uint16_t port_mask[16];             //alarm value
    enum CES_ALARM_TYPE type;   //alarm type
};

//all tdm ports, use bit to represent port num
struct ces_e1_port_mask
{
    uint16_t e1_port_mask[8];
};

//use bitmap to store all tdm interface for sc01q
#define CES_PORT_MASK_SET(bitmap, port)                                     \
    bitmap.e1_port_mask[(port -1) >> 4] |= (0x1 << ((port - 1) & 0x0F))      \
    
#define CES_PORT_MASK_CLR(bitmap, port)                                     \
    bitmap.e1_port_mask[(port -1) >> 4] &= ~(0x1 << ((port - 1) & 0x0F))     \
    
#define CES_ALARM_MASK_GET(bitmap, port)                                     \
    (bitmap[(port -1) >> 4] & (0x1 << ((port - 1) & 0x0F))) ? 1 : 0     \

void ces_if_table_init(int size);
struct ces_if *ces_if_add(uint32_t ifindex);
int ces_if_delete(uint32_t ifindex);
struct ces_if *ces_if_lookup(uint32_t ifindex);
int ces_if_down(uint32_t ifindex);
int ces_if_up(uint32_t ifindex);
int ces_if_info_set(struct ces_if *pif, void *pinfo, enum CES_INFO type);
int ces_set_encap(struct ces_if *pif, uint32_t encap);
int ces_set_frame(struct ces_if *pif, uint32_t frame);
int ces_set_mode(struct ces_if *pif, uint32_t mode);
int ces_set_sendclock(struct ces_if *pif, uint32_t clock);
int ces_set_recvclock(struct ces_if *pif, uint32_t clock);
int ces_set_jitbuffer(struct ces_if *pif, uint8_t jitbuffer);
int ces_set_rtp(struct ces_if *pif, uint8_t rtp);
int ces_set_prbs(struct ces_if *pif, uint8_t prbs);
int ces_set_loopback(struct ces_if *pif, uint32_t loopback);
int ces_set_timeslot(struct ces_if *pif, uint32_t timeslot);
void ces_if_info_init(uint32_t ifindex, struct ces_if *pif);
struct ces_if *ces_if_create(uint32_t ifindex);
void ces_alarm_info_show(struct vty *vty,  struct ces_if *pif);
void ces_config_info_show(struct vty *vty,  struct ces_if *pif);
void ces_if_alarm_set(uint8_t value, uint32_t ifindex, enum CES_ALARM_TYPE type);
void ces_if_alarm_clear(uint32_t ifindex);
/* 查找传入 ifindex 之后对应的 n 个数据 */
int ces_if_bulk_get(uint32_t ifindex, struct ces_if cesif[]);
int ces_if_timeslot_set(uint32_t ifindex, uint32_t timeslot);
int ces_if_timeslot_clear(uint32_t ifindex, uint32_t timeslot);
int ces_if_timeslot_valid(uint32_t ifindex, uint32_t timeslot);
struct ces_count_info *ces_if_get_count(uint32_t ifindex);
int ces_if_config_restore(uint32_t ifindex);
int ces_set_j2(struct ces_if *pif, char *j2);
int ces_if_sc01q_alarm_set(struct ces_sc01q_alarm_info *alarm_info, uint32_t slot_index);


#endif

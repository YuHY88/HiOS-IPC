#ifndef HIOS_STM_IF_H
#define HIOS_STM_IF_H

//#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/devm_com.h>

/*
*    B1/B2/B3门限对应计数关系,
*    {
*       //0  -1  -2  -3     -4    -5    -6   -7 -8 -9
*         0,  0,  0, 50000, 7200, 1000, 100 ,10 ,1 ,0
*    }
*/

#define STM_OVERHEAD_LEN        15                  //开销字节长度，第0位是后15位字符串的校验码
#define STM_OVERHEAD_C2         0x02                //信号标记字节C2
#define STM_OVERHEAD_J0         "aaaaaaaaaaaaaaa"   //j0再生段踪迹字节
#define STM_OVERHEAD_J1         "bbbbbbbbbbbbbbb"   //J1高阶通道踪迹字节
#define STM_OVERHEAD_J2         "ccccccccccccccc"   //E1通道开销，低阶通道踪迹字节J2

#define STM_EXC_THRESHOLD       -3      //超过EXC门限50000，误码过量
#define STM_DEG_THRESHOLD       -6      //低于DEG门限100，信号劣化

#define STM_SET_PADDING_J0      (1 << 0)
#define STM_SET_PADDING_J1      (1 << 1)
#define STM_SET_PADDING_J2      (1 << 2)

#define STM_PT_MODE      		1
#define STM_NO_PT_MODE      	2

#define STM_PT_RECOVER     		1
#define STM_NO_PT_RECOVER     	2

#define STM_WRT_TIME     		1

#define STM_DXC_DISABLE 		2
#define STM_DXC_ENABLE			1

#define STM_VC12_NUM_MAX   	    63
#define STM_TDM_PORT_MAX   	    126
#define STM_TDM_PORT_64  	    64

#define STM_TDM_VC4_64   	    4
#define STM_TDM_VC4_128   	    5

#define STM_TDM_NAME_LENGTH  	256

#define  DXC_RESERVED_BASE		1000

#define STM_DXC_VC4  	    	32
#define STM_DXC_VC12  	    	64

#define STM_IMAGE_TYPE_E1       0
#define STM_IMAGE_TYPE_STM1     1


/*stm snmp info type*/
enum STM_SNMP_TYPE
{
    STM_SNMP_TYPE_BASE_GET = 0,
    STM_SNMP_TYPE_RS_GET,
    STM_SNMP_TYPE_MS_GET,
    STM_SNMP_TYPE_HP_GET,
    STM_SNMP_TYPE_MSP_GET,
    STM_SNMP_TYPE_SNCP_GET
};

/* STM 消息类型 */
enum STM_INFO
{
    STM_INFO_INVALID = 0,
    STM_INFO_J0,                //设置j0
    STM_INFO_J1,                //设置j1
    STM_INFO_C2,                //设置c2
    STM_INFO_LOOPBACK,          //设置环回
    STM_INFO_MSP,               //设置保护组
    STM_INFO_WTR,               //设置保护组回切时间
    STM_INFO_COUNT_GET,         //获取统计计数
    STM_INFO_STATUS_GET,        //获取j0/j1/c2/g1/...的接收值
    STM_INFO_MSP_STATUS_GET,    //get msp status: working at master or backup
    STM_INFO_DXC_SET,
    STM_INFO_DXC_WRT,
    STM_INFO_DXC_STATUS_GET,
    STM_INFO_ALARM,             //SEND ALARM INFO FROM HAL TO CES ON TIME
    STM_INFO_STATISTICS,        //REPORT STATISTICS FROM HAL TO CES ON TIME
    STM_INFO_IMAGE_SWITCH       //IMAGE SWITCH FOR SC01Q
};

/*STM 环回方式*/
enum STM_LOOPBACK
{
    STM_LB_NOLOOPBACK = 0,
    STM_LB_REMOTE,
    STM_LB_LOCAL
};

/*接口类型*/
enum STM_TYPE
{
    IF_TYPE_STM1  = 1,
    IF_TYPE_STM4  = 4,
    IF_TYPE_STM16 = 16,
    IF_TYPE_STM64 = 64
};
    
/*msp 保护模式*/
enum STM_MSP_MODE
{
    MSP_MODE_OPO = 0,
    MSP_MODE_OTO
};
    
/*stm 告警消息类型*/
enum STM_ALARM_TYPE
{
    STM_ALARM_OPT_LOS = 0,
    STM_ALARM_OPT_LOF,
    STM_ALARM_REMOTE_OFF,
    STM_ALARM_TU_LOM,
    STM_ALARM_AU_LOP,
    STM_ALARM_AU_AIS,
    
    STM_ALARM_RS_TIM,
    STM_ALARM_RS_EXC,
    STM_ALARM_RS_DEG,
    
    STM_ALARM_MS_AIS,
    STM_ALARM_MS_RDI,
    STM_ALARM_MS_REI,
    STM_ALARM_MS_EXC,
    STM_ALARM_MS_DEG,
    
    STM_ALARM_HP_TIM,
    STM_ALARM_HP_SLM,
    STM_ALARM_HP_UNQ,
    STM_ALARM_HP_REI,
    STM_ALARM_HP_RDI,
    STM_ALARM_HP_EXC,
    STM_ALARM_HP_DEG    
};


/*----------start: defined for mib----------*/
struct stm_base_info
{
    uint32_t    ifindex;    //stm ifindex
    uint8_t     stm_type;   //STM1(1), STM4(2), STM16(3), STM64(4)
    uint8_t     stm_alarm;  //bit, los(0x80)
    uint8_t     pad[2];
};

struct stm_rs_info
{
    uint32_t    ifindex;                    //stm ifindex
    char        j0_tx[STM_OVERHEAD_LEN+1];
    char        j0_rx[STM_OVERHEAD_LEN+1];
    int         exc_thr;
    int         deg_thr;
    uint32_t    b1;
    uint8_t     rs_alarm;  //BITS { LOF( 0 ), OOF( 1 ), RS_EXC( 2 ), RS_DEG( 3 ), RS_TIM( 4 ) } 
    uint8_t     pad[3];
};

struct stm_ms_info
{
    uint32_t    ifindex;    //stm ifindex
    int         exc_thr;
    int         deg_thr;
    uint32_t    b2;
    uint32_t    rei_count;  //暂未实现
    uint8_t     k2_rx;
    uint8_t     ms_alarm;   //BITS { MS_AIS( 0 ), MS_RDI( 1 ), MS_REI( 2 ), MS_EXC( 3 ), MS_DEG( 4 ) } 
    uint8_t     pad[2];
};

struct stm_hp_info
{
    uint32_t    ifindex;    //stm ifindex
    uint16_t    vc4_id;
    char        j1_tx[STM_OVERHEAD_LEN+1];
    char        j1_rx[STM_OVERHEAD_LEN+1];
    int         exc_thr;
    int         deg_thr;
    uint32_t    b3;
    uint32_t    rei_count;  //暂未实现
    uint32_t    plu_count;
    uint32_t    nga_count;
    uint8_t     c2_tx;
    uint8_t     c2_rx;
    uint8_t     g1_rx;
    uint8_t     pad[1];
    uint16_t    hp_alarm;   //BITS { AU_LOP(0), AU_AIS(1), TU_LOM(2) ,HP_UNEQ(3), HP_SLM(4), HP_RDI(5), HP_REI(6), HP_EXC(7), HP_DEG(8), HP_TIM(9)} 
};

struct stm_hp_index
{
    uint32_t    ifindex;    //stm ifindex
    uint16_t    vc4_id;
};

/*----------end: defined for mib----------*/

/*stm 统计计数*/
struct stm_count_info
{
    uint32_t    B1;             //B1误码统计
    uint32_t    B2;             //B2误码统计
    uint32_t    B3;             //B3误码统计
    uint32_t    MS_REI;         //???
    uint32_t    VC4_REI;
    uint32_t    VC4_ADJ;
};

enum STM_MSP_ACTION
{
    STM_MSP_DISABLE = 0,
    STM_MSP_ENABLE
};

/*stm msp 配置信息*/
struct stm_msp_config
{
    uint16_t                msp_id;        //msp保护索引<1-32>
    uint16_t                wtr;           //回切时间 <1-12> 单位分钟
    uint32_t                master_if;     //主端口/工作端口
    uint32_t                backup_if;     //备份端口
    enum STM_MSP_MODE       mode;          //保护模式，默认1+1，暂不支持1:1
    enum STM_MSP_ACTION     action;        //使能或关闭保护
    uint8_t                 set_flag;      //mode参数是否配置
    uint8_t                 down_flag;     //用于配置恢复，板卡拔掉为link_down，重插时恢复msp配置并置位link_up
    uint8_t                 status;        //msp status: working at master or backup interface
    uint8_t                 pad;
};
struct stm_dxc_if
{
	uint32_t ifindex;
	uint8_t  dxc_vc4;  /*提供给底层用的vc4*/
	uint8_t  vc12_vc4; /*提供命令行显示用的vc4*/
	uint8_t	 dxc_vc12;
	uint8_t pad;
};
struct stm_dxc_config
{
	char name[STM_TDM_NAME_LENGTH];	
	struct stm_dxc_if base_if;
	struct stm_dxc_if backup_if;
	struct stm_dxc_if dst_if;
	uint8_t num;
	uint8_t mode;			/*是否配置保护1：保护 2：未保护*/
	uint8_t wtr;			/*恢复时间*/
	uint8_t failback;		/*恢复模式*/
	uint8_t action;			/*是否配置dxc业务，1：配置，0：未配置*/
	uint8_t index;			/*配置dxc没有配置名字时申请的索引*/
	uint8_t index_flag;		/*配置dxc没有配置名字时的标记，释放索引的时候用 */
	uint8_t status;			/*wtr status */
	uint8_t down_flag;      // 0: up, 1: down
	
	
};

struct stm_rs_alarm_t
{
    uint8_t         los;        // 对端光丢失
    uint8_t         lof;        // e1帧失步
    uint8_t         oof;        // 可不实现(a1 != 0xf6 || a2 != 0x28)
    uint8_t         rs_exc;     // B1每秒计数超过EXC门限，误码过量
    uint8_t         rs_deg;     // B1每秒计数超过DEG门限，信号劣化
    uint8_t         rs_tim;     // J0接收和期望不一致，期望可以暂时认为就是发送的数据
};

struct stm_ms_alarm_t
{
    uint8_t         ms_ais;     // (k2 & 0x07)==0x07
    uint8_t         ms_rdi;     // (k2 & 0x07)==0x06
    uint8_t         ms_rei;     // 可不实现
    uint8_t         ms_exc;     // B2每秒计数超过EXC门限，误码过量
    uint8_t         ms_deg;     // B2每秒计数超过DEG门限，信号劣化
};

struct stm_hp_alarm_t
{
    uint8_t         au_lop;          // H1和H2字节代表的指针值非法（不在0~782内），且连续8帧收到非法帧
    uint8_t         au_ais;          // AU指针H1和H2字节全为“1”，并下插全“1”信号
    uint8_t         tu_lom;          // H4（用来指示当前的TU-12是当前复帧的第几帧）字节非法，上报高阶通道复帧丢失，并下插全“1”和正常的H4字节
    uint8_t         hp_unq;          // (c2 == 0x00)    高阶通道未装载
    uint8_t         hp_slm;          // (c2 != 0x02 && c2 != 0x01 && c2 != 0x00)    高阶通道信号标记失配
    uint8_t         hp_rdi;          // (g1 & 0x08) G1的bit5 == 1
    uint8_t         hp_rei;          // G1 bit1~bit4 == 1~8
    uint8_t         hp_exc;          // B3每秒计数超过EXC门限，误码过量
    uint8_t         hp_deg;          // B3每秒计数超过DEG门限，信号劣化
    uint8_t         hp_tim;          // J1接收和期望不一致，期望可以暂时认为就是发送的数据
};

//读取所有告警
struct stm_alarm_t
{
    struct stm_rs_alarm_t rs_alarm;
    struct stm_ms_alarm_t ms_alarm;
    struct stm_hp_alarm_t hp_alarm;
};

/*再生段信息*/
struct stm_rs_t
{
    char            j0_tx[STM_OVERHEAD_LEN+1];   // 16个字符，第一个是crc8，后面15个是可见字符，
                                                   //crc8是根据后面的15个字符（不足补0或者空格）计算出来的，用来接收定位
    char            j0_rx[STM_OVERHEAD_LEN+1];
    uint8_t         pad_zero;           //命令行是否使用0补齐
    uint8_t         pad[3];
    int             exc_thr;           // -3, 表示10的-3次方，负数不好做就写作3，获得一个固定的32位门限数据，见上面
    int             deg_thr;           // -6
    uint32_t        b1;
    struct stm_rs_alarm_t  rs_alarm;
};

/*复用段信息*/
struct stm_ms_t
{
    uint8_t         k1_tx;      // 
    uint8_t         k1_rx;      // 
    uint8_t         k2_tx;      // 每秒回写，根据实际告警写0x00或者0x06（los/lof/ms ais/rs exc/ms exc）
    uint8_t         k2_rx;      // 读，产生ms rdi/ms ais
    uint8_t         m1;         // 远端误块指示，产生ms-rei
    uint8_t         pad[3];     
    int             exc_thr;    // -3, 表示10的-3次方，软件可以对应3，获得一个固定的32位门限数据
    int             deg_thr;    // -6
    uint32_t        b2;
    struct stm_ms_alarm_t  ms_alarm;
};

/*hal上报信息时使用，单个stm接口下的vc4_id*/
struct alarm_info_t
{
    uint16_t        vc4_id;     //如果stm_type是4/16/64，vc4_id在hal层读取时指定, stm_type是1，则默认为1
    enum STM_TYPE   stm_type;   //hal层读取FPGA时指定读取的stm类型，并指定vc4_id
    uint8_t         value;      //读取的值
    uint8_t         pad[2];
};

struct hp_info_t
{
    char            j1_tx[STM_OVERHEAD_LEN+1];
    char            j1_rx[STM_OVERHEAD_LEN+1];
    uint8_t         c2_tx;          // 固定0x02
    uint8_t         c2_rx;          // 读，产生hp_uneq/hp slm
    uint8_t         g1_tx;          // 每秒回写，根据实际告警写0x00或者0x08(au ais/au lop/hp unq)
    uint8_t         g1_rx;          // 读，产生hp_rdi
    int             exc_thr;        // -3, 表示10的-3次方，软件可以对应-3，获得一个固定的32位门限数据
    int             deg_thr;        // -6
    uint32_t        b3;
    uint32_t        plu_cnt;         // 正指针调整
    uint32_t        nga_cnt;         // 负指针调整
    struct stm_hp_alarm_t  hp_alarm;
};

/*高阶通道信息*/
struct stm_hp_t
{
    uint16_t            vc4_id;
    uint8_t             pad_zero;           //命令行是否使用0补齐
    uint8_t             pad;                //保留
    struct hp_info_t    hp_info;
    struct stm_hp_t     *next;
};

//向hal发送/获取配置信息，避免直接使用字节数较多的stm_hp_t
struct stm_oh_info_t
{
    uint16_t    vc4_id;
    uint8_t     c2;
    uint8_t     g1;
    uint8_t     k1;
    uint8_t     k2;
    uint8_t     m1;
    uint8_t     pad;
    char        j0[STM_OVERHEAD_LEN+1];
    char        j1[STM_OVERHEAD_LEN+1];
};

/*stm 接口信息*/
struct stm_if
{
    uint32_t            ifindex;        //stm 接口索引
    uint8_t             msp_id;         //接口所属的 msp 保护组索引
    uint8_t             down_flag;      // 0: up, 1: down
    enum STM_TYPE       type;           //stm 接口类型
    enum STM_LOOPBACK   loopback;
                 
    struct stm_rs_t     stm_rs;
    struct stm_ms_t     stm_ms;
    struct stm_hp_t     *stm_hp_list;
};

/*hal send alarm info to update rs_alarm\ms_alarm\hp_alarm*/
struct stm_alarm_info
{
    uint8_t status;             //alarm value
    enum STM_ALARM_TYPE type;   //alarm type
};

/*hal send statistics info to update stm statistics*/
struct stm_statis_info
{
    struct stm_oh_info_t    stm_status;
    struct stm_count_info   count_info;
};

/*used for image type*/
struct stm_slot_info
{
    uint16_t    slot_id;
    uint8_t     image_type;
    uint8_t     pad;
};

struct hash_table stm_if_table;
extern struct list *stm_msp_list;
extern struct list *stm_dxc_list;
extern struct cmd_node sdh_dxc_node;

struct stm_slot_info g_stm_slot[SLOT_NUM_MAX + 1];

int stm_msp_init(void);
int stm_dxc_init(void);

struct stm_msp_config *stm_msp_lookup(uint32_t ifindex);
struct stm_msp_config *stm_msp_lookup_by_mspid(uint16_t msp_id);
int stm_msp_add(struct stm_msp_config *msp_info, uint16_t msp_id);
int stm_dxc_delete(char *name);
struct stm_dxc_config *stm_dxc_lookup(char *name);
int stm_dxc_add(struct stm_dxc_config *dxc_info);
struct stm_dxc_config *stm_dxc_index_lookup(uint32_t ifindex);
int stm_msp_delete(uint16_t msp_id);
int stm_msp_wtr_set(struct stm_msp_config *msp_info, uint16_t wtr);
struct stm_hp_t *stm_hp_lookup(struct stm_hp_t *hp_list, uint16_t vc4_id);
struct stm_hp_t *stm_hp_add(struct stm_hp_t **hp_list, uint16_t vc4_id);

void stm_if_init(void);
void stm_if_table_init(int size);
int stm_if_info_init(uint32_t ifindex, struct stm_if *pif, enum STM_TYPE type);
struct stm_if *stm_if_add(uint32_t ifindex, enum STM_TYPE type);
int stm_if_delete(uint32_t ifindex);
struct stm_if *stm_if_lookup(uint32_t ifindex);
int stm_if_down(uint32_t ifindex);
int stm_if_up(uint32_t ifindex);
struct stm_if *stm_if_create(uint32_t ifindex, enum STM_TYPE type);
void stm_if_alarm_clear(struct stm_if *pif);
void stm_if_alarm_set(struct stm_alarm_info *alarm_info, uint32_t ifindex);
int stm_j0_set(struct stm_if *pif, char *j0);
int stm_j1_set(struct stm_if *pif, struct stm_hp_t *hp_info);
int stm_c2_set(struct stm_if *pif, struct stm_hp_t *hp_info);
int stm_loopback_set(struct stm_if *pif, uint8_t loopback);
int stm_set_info(struct stm_if *pif, void *pinfo, enum STM_INFO type);
struct stm_count_info *stm_if_get_count(uint32_t ifindex);
struct stm_oh_info_t *stm_if_get_status(uint32_t ifindex);
int stm_if_snmp_base_bulk_get(uint32_t ifindex, struct stm_base_info stm_base[]);
int stm_if_snmp_rs_get(struct stm_if *pif, struct stm_rs_info *pif_rs);
int stm_if_snmp_rs_bulk_get(uint32_t ifindex, struct stm_rs_info stm_rs[]);
int stm_if_snmp_ms_get(struct stm_if *pif, struct stm_ms_info *pif_ms);
int stm_if_snmp_ms_bulk_get(uint32_t ifindex, struct stm_ms_info stm_ms[]);
int stm_if_snmp_hp_get(struct stm_if *pif, struct stm_hp_info *pif_hp, uint16_t vc4_id);
int stm_if_snmp_hp_bulk_get(void *pdata, struct stm_hp_info stm_hp[]);
int stm_if_snmp_msp_bulk_get(uint32_t msp_id, struct stm_msp_config stm_msp[]);
int stm_if_loopback_status_get(uint32_t ifindex, uint32_t *lb_status);
void stm_if_statis_update(struct stm_statis_info *statis_info, uint32_t ifindex);
uint8_t *stm_msp_get_status(uint32_t ifindex);


void stm_interface_alarm_show(struct vty *vty, struct stm_if *pif);
void stm_interface_status_show(struct vty *vty, struct stm_if *pif);
void stm_interface_statis_show(struct vty *vty, struct stm_if *pif);
void stm_interface_show(struct vty *vty, struct stm_if *pif);
void stm_interface_config_show(struct vty *vty, struct stm_if *pif);
void stm_dxc_show_status(struct vty *vty, struct stm_dxc_config *dxc);
int stm_dxc_cross_check(struct stm_dxc_config *pdxc_new,struct vty *vty);
void stm_cmd_init(void);
int stm_config_write(struct vty *vty);
int c2_to_hex(char *str, uchar *hexbuf);
void stm_dxc_status_update(uint32_t ifindex, uint8_t state);
int stm_if_snmp_dxc_bulk_get(void *data, uint32_t data_len, struct stm_dxc_config stm_dxc[]);
void stm_fpga_image_type_set(int slot, uint8_t image_type);


#endif

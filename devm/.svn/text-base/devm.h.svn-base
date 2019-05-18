/*
*     define of device management
*
*/

#ifndef HIOS_DEVM_H
#define HIOS_DEVM_H


#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>


#define MAX_TIMER_CNT 8
#define MAX_FUNC_ARG_LEN 100
#define PLUGOUT_TIME 60


/*风扇转速上下限*/
#define FAN_SPEED_MAX (10000)
#define FAN_SPEED_MIN (1000)

/*POWER*/
#define POWER1_ADDR  0x0009
#define POWER2_ADDR  0x000a

typedef enum
{
    POWER_TYPE_48v_12V_75W = 0,
    POWER_TYPE_resv1,
    POWER_TYPE_48v_12V_150W,
    POWER_TYPE_resv2,
    POWER_TYPE_220v_12V_75W,
    POWER_TYPE_resv3,
    POWER_TYPE_220v_12V_150W,
    POWER_TYPE_NONE,
    POWER_TYPE_PWR01A,  // 48v/350w
    POWER_TYPE_PWR02A,  // 220v/350w
}   e_POWER_TYPE;

#define VOL_48_MAX                      (60)
#define VOL_48_MIN                      (40)

#define VOL_220_MAX                     (242)
#define VOL_220_MIN                     (198)

#define VOL_NORMAL                      0
#define VOL_INPUT_LOW_ALARM             1
#define VOL_INPUT_HIGH_ALARM            2

#define VOL_OUTPUT                      12

#define VOL_OUTPUT_POWER_DOWN_ALARM     1
#define VOL_OUTPUT_POWER_UP_ALARM       0

/*TEMPERATURE*/
#define TEMPERATURE_NORMAL              0
#define TEMPERATURE_INPUT_LOW_ALARM     1
#define TEMPERATURE_INPUT_HIGH_ALARM    2


/*heart beat num*/
#define DEVM_HEART_BEAT 60


/*EEPROM OP CMD*/
#define EEP_INFO_CMD_SET     1
#define EEP_INFO_CMD_GET     2
#define EEP_INFO_CMD_ERASE   3
#define EEP_INFO_CMD_CLEAR   4

/*timezone path*/
#define TIMEZONE_PATH   "/etc/localtime"

struct slot_status
{
    enum HA_ROLE slot_status;
    unsigned char myslot;
    unsigned char pad[3];
};

struct slot_ha_status
{
    enum HA_ROLE  my_status;
    unsigned char my_unit;
    unsigned char my_slot;
    unsigned char neighbour_slot;
    unsigned char neighbour_exsit;  /*1:exist*/
};

/*eeprom info type*/
enum EEP_INFO_TYPE
{
    EEP_INFO_NONE    = 0,
    EEP_INFO_DEVNAME = 1,
    EEP_INFO_OEM_NAME = 2,
    EEP_INFO_HW_VER = 3,
    EEP_INFO_VENDOR = 4,
    EEP_INFO_DATE = 5,
    EEP_INFO_MAC = 6,
    EEP_INFO_ID = 7,
    EEP_INFO_DVER = 8,
    EEP_INFO_SN = 9,
    EEP_INFO_BOM = 10,
    EEP_INFO_LIC = 11,
	EEP_INFO_CFGINIT = 12, 
	EEP_INFO_SLOT_NAME = 13,
	EEP_INFO_MAC_NUM = 14,	
    EEP_INFO_ALL = 15,
};

/*EEPROM SIZE*/
#define EEP_SIZE_MAX 256
#define EEP_SIZE_MIN 128

#define PRODUCT_INFO_KEY_WORDS_MAX  30
#define PRODUCT_INFO_BLOCKS_MAX     12
#define INPUT_INFO_LEN_MIN          20
#define INPUT_INFO_LEN_MAX          50
#define EEPROM_PRODUCT_INFO_MAX     256 //128
#define EEPROM_PRODUCT_INFO_MIN     128


#if 1
#define PRODUCT_KEY_DEV_NAME        "device"
#define PRODUCT_KEY_OEM_NAME        "oem"
#define PRODUCT_KEY_HW_VERSION      "hardware_version"
#define PRODUCT_KEY_VENDOR          "vendor"
#define PRODUCT_KEY_SLOT_NAME       "slot_name"
#define PRODUCT_KEY_DEV_TYPE        "hwcfg"
#define PRODUCT_KEY_MAC_NUM         "macnum"

#define PRODUCT_KEY_DEV                 "dev"
#define PRODUCT_KEY_HVER                "hver"
#define PRODUCT_KEY_VND                 "vnd"
#define PRODUCT_KEY_DATE                "date"
#define PRODUCT_KEY_MAC                 "mac"
#define PRODUCT_KEY_DVER                "dver"
#define PRODUCT_KEY_SN                  "sn"
#define PRODUCT_KEY_BOM                 "bom"
#define PRODUCT_KEY_LIC                 "lic"
#define PRODUCT_KEY_CFGINIT             "cfginit"


#endif


//#define BACK_PRODUCT_KEY_VENDOR          "vendor"
//#define BACK_PRODUCT_KEY_DEV_NAME        "device"
//#define BACK_PRODUCT_KEY_OEM_NAME        "oem"
//#define BACK_PRODUCT_KEY_DEV_TYPE        "hwcfg"
//#define BACK_PRODUCT_KEY_MAC             "1st_mac"
//#define BACK_PRODUCT_KEY_MAC_NUM         "macnum"
//#define BACK_PRODUCT_KEY_HW_VERSION      "hardware_version"

#if 0
#if defined(HT201) || defined (HT153) || defined (HT157)
#define BACK_PRODUCT_KEY_VENDOR          "vendor"
#define BACK_PRODUCT_KEY_DEV_NAME        "device"
#define BACK_PRODUCT_KEY_OEM_NAME        "oem"
#define BACK_PRODUCT_KEY_DEV_TYPE        "hwcfg"
#define BACK_PRODUCT_KEY_MAC             "1st_mac"
#define BACK_PRODUCT_KEY_MAC_NUM         "macnum"
#define BACK_PRODUCT_KEY_HW_VERSION      "hardware_version"
#elif defined(H20RN2000) || defined (HT2200) || defined (HT2100)
#define BACK_PRODUCT_KEY_VENDOR          "back_vendor"
#define BACK_PRODUCT_KEY_DEV_NAME        "back_device"
#define BACK_PRODUCT_KEY_OEM_NAME        "back_oem"
#define BACK_PRODUCT_KEY_DEV_TYPE        "back_hwcfg"
#define BACK_PRODUCT_KEY_MAC             "back_1st_mac"
#define BACK_PRODUCT_KEY_MAC_NUM         "back_macnum"
#define BACK_PRODUCT_KEY_HW_VERSION      "back_hardware_version"
#endif
#endif

#define DEVM_DBG_COMM   		(1 << 0)		//common func  debug
#define DEVM_DBG_HA		       (1 << 1)		//ha trans debug
#define DEVM_DBG_MSG		       (1 << 2)		//msg trans debug
#define DEVM_DBG_REG    	       (1 << 3)		//register  debug
#define DEVM_DBG_TIMER	       (1 << 4)		//timer   debug
#define DEVM_DBG_ALL			(DEVM_DBG_COMM |\
							        DEVM_DBG_HA |\
							        DEVM_DBG_MSG |\
							        DEVM_DBG_REG |\
							        DEVM_DBG_TIMER) 

#define DEVMMYPRINTF_BASE( format, args... )  printf( format, ##args )
#define DEVMMYPRINTF_0( format, args... ) \
{ \
        time_t timep; \
        time( &timep ); \
        struct tm *pTM = gmtime( &timep ); \
        DEVMMYPRINTF_BASE( "[%4d-%02d-%02d %02d:%02d:%02d]"format, \
                pTM->tm_year+1900, pTM->tm_mon, pTM->tm_mday, pTM->tm_hour+8, pTM->tm_min, pTM->tm_sec, ##args ) ; \
}
//const char t_devm_use_name[4096] = "Fox Test";
//#define USER_PRINT_1( format, args... )  USER_PRINT_0( "[%s]"format, t_devm_use_name, ##args )
#define DEVMMYPRINTF_1( format, args... )  DEVMMYPRINTF_0( "[%s]"format, "FOR TEST", ##args )
#define DEVMMYPRINTF_2( format, args... )  DEVMMYPRINTF_1( "[PID:%d]"format, getpid(), ##args ) //getpid() 获取当前进程ID
#define DEVMMYPRINTF_3( format, args... )  DEVMMYPRINTF_2( "[func:%s]"format, __func__, ##args ) //getpid() 获取当前进程ID
#define DEVMMYPRINTF_4( format, args... )  DEVMMYPRINTF_3( "[line:%d]"format, __LINE__, ##args ) //getpid() 获取当前进程ID
#define DEV_MYPRINTF( format, args...)  DEVMMYPRINTF_4( format, ##args )

struct t_hsl_devm_port
{
    enum IFNET_SUBTYPE port_type;
    unsigned char mac[6];
    unsigned char unit;
    unsigned char slot;     /* 逻辑 slot */
    unsigned char port;     /* 面板端口号 */
    unsigned char fiber;    /* 端口的光电类型，0光，1电, 2其他 */
    unsigned char hide;     /* 1:表示隐藏端口 */
    unsigned char phyid;    /* 无配置 0xff */
    int           gport;    /* 芯片端口号 */
};


struct plugout_timer_info
{
    int state; /* on or off */
    int interval;
    int elapse; /* 0~interval */
    int (* timer_proc)(void *arg, int arg_len);
    char func_arg[MAX_FUNC_ARG_LEN];
    int arg_len;
};


typedef struct
{
    int             eeprom_max_size;
    unsigned char   num;
    struct
    {
        unsigned char        key_word[PRODUCT_INFO_KEY_WORDS_MAX];
        int         max_size;
    }   unit[PRODUCT_INFO_BLOCKS_MAX];
}   EEP_KEY;


extern struct devm device;

extern struct thread_master *devm_master;

/* Prototypes. */
void devm_thread_init(void);
void devm_init_dev(void);
void devm_init_default_event_module(void);
int devm_get_slot_app_version(unsigned char *sys_version, unsigned char slot);
int devm_box_create(const struct devm_box *pbox);
int devm_unit_create(uint8_t unit, const struct devm_unit *punit);
int devm_slot_plugin(uint8_t unit, uint8_t slot);
int devm_slot_plugout(uint8_t unit, uint8_t slot);
int devm_add_slot(uint8_t unit, uint8_t slot, const struct devm_slot *pslot);
int devm_delete_slot(uint8_t unit, uint8_t slot);
int devm_add_card(uint8_t unit, uint8_t slot, uint8_t card, const struct devm_card *t_devm_card); /*保留待用*/
int devm_delete_card(uint8_t unit, uint8_t slot, uint8_t card);/*保留待用*/
int devm_add_port(uint8_t unit, uint8_t slot, struct list *p_port_list, struct devm_port *t_devm_port);
int devm_delete_port(uint8_t unit, uint8_t slot, struct list *p_port_list, struct devm_port *t_devm_port);
int devm_add_power(uint8_t unit, uint8_t powerslot, const struct devm_power *ppower);
int devm_del_power(uint8_t unit, uint8_t powerslot);
int devm_update_power(uint8_t unit, uint8_t powerslot, const struct devm_power *ppower);
int devm_get_fan_speed(unsigned short *fan_speed);
int devm_add_fan(uint8_t unit, uint8_t slot, const struct devm_fan *pfan);
int devm_del_fan(uint8_t unit, uint8_t slot);
int devm_update_fan(uint8_t unit, uint8_t slot, const struct devm_fan *pfan);
int devm_update_slot_partnerslot(void);
int devm_pthread_create(pthread_t *pthread_id, void *(*pthread_func)(void *arg), void *arg);
int devm_check_temp_alarm(signed char temperature, struct devm_temp *temp);
int devm_update_temp(uint8_t unit, uint8_t slot, const char *ptemp);
int devm_check_power_alarm(uint8_t unit, uint8_t powerslot , struct devm_power *prevpower, struct devm_power *postpower, int power_type);

int devm_get_hal_slot_plug_status_hash(unsigned int slot, unsigned char *slot_plug_status);
int devm_get_hal_device_status(unsigned int *para_hal_status, unsigned int array_num);
int devm_get_hal_box_info(struct devm_box *para_devm_box);
int devm_get_hal_slot_info(unsigned int slot, struct devm_slot *para_devm_slot);
int devm_get_hal_port_info(unsigned int slot, struct t_hsl_devm_port *para_hsl_devm_port);

/*devm_com.c中调用函数获取设备参数*/
struct devm_slot *devm_get_slot(uint8_t unit, uint8_t slot);
int devm_get_device_id(void);
uchar *devm_get_slot_name(uint8_t unit, uint8_t slot);
uchar *devm_get_device_name(uint8_t unit, uint8_t slot);
struct devm_fan *devm_get_fan(uint8_t unit);
struct devm_power *devm_get_power(uint8_t unit);
struct devm_unit *devm_get_unit(uint8_t unit);
struct devm_backplane *devm_get_backplane(uint8_t unit);
uchar *devm_get_manufacture(uint8_t unit);
int devm_get_slot_status(uint8_t unit, uint8_t slot);
uchar *devm_get_mac(uint8_t unit, uint8_t slot);
int devm_get_timezone(int *timezone);


void devm_set_hash_del_slot(uint8_t slot);
void devm_set_hash_add_slot(uint8_t slot);
void devm_set_slot_fpga_reconfig(unsigned int slot);
void devm_reset_master(enum DEV_TYPE type, uint8_t unit);
void devm_reset_device(enum DEV_TYPE type, uint8_t unit, uint8_t slot);
void devm_setmx_m2s(enum DEV_TYPE type, uint8_t unit);
void devm_reboot_device(enum DEV_TYPE type, uint8_t unit, uint8_t slot);


/*devm_cmd.c的命令行注册*/
void devm_cli_init(void);


/*devm_register.c函数*/
int devm_event_notify(enum DEV_EVENT type, struct devm_com *pdev);
void devm_event_list_init(void);
int devm_register_msg(int module_id, uint8_t subtype , uint8_t queue_id);

/*devm_msg.c 函数*/
//int devm_msg_send_hal(void *pdata, struct ipc_msghdr *phdr);
//int devm_msg_rcv(struct thread *p_thread);
int devm_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);


/*devm send port info to ifm*/
void devm_send_ifm_add_port(struct ifm_common *ifcom, struct devm_port *p_devm_port);
void devm_send_ifm_del_port(struct ifm_common *ifcom, struct devm_port *t_port);

/*send watchdog value to hal devm*/
void devm_set_watchdog(uint8_t wdi, uint8_t unit, uint8_t slot);

/*send heart beart value to hal devm*/
void devm_set_heart_beat(uint8_t heart, uint8_t unit, uint8_t slot);

/*send switch dynamic voltage msg to hal devm*/
int devm_send_hal_set_dynamic_voltage(unsigned char powerslot, unsigned char voltage_cmd);
/*send update voltage limit msg to hal devm*/
int devm_update_voltage_limit(int unit, int powerslot, struct devm_power *p_power);
/*send update temperature limit msg to hal devm*/
int devm_update_temperature_limit(struct devm_temp *p_temp);
/*send ctrl slot power up/down msg to hal*/
int devm_send_set_slot_power_up_down(int slot, int slot_cmd);

int devm_get_power_alarm_type_by_mode(uint8_t  mode);

/*get info from eeprom buffer*/
int devm_find_info_in_orig_buf(unsigned char *orig_buf, unsigned char *key_word, unsigned char *find_buf , int len);
/*get op eeprom info type*/
int devm_get_op_eeprom_type(char *info_type);
int devm_get_op_eeprom_type_digital(char *info_type);

/*check if slot need this eeprom info type*/
int devm_op_eeprom_check_type(int cmd_type, unsigned char *str_info_type, EEP_KEY *g_peep_info);
/*check if eeprom info form is ok*/
int devm_op_eeprom_check_data_form(int slot, int info_type, unsigned char *info_data, int data_len);
/*send op eeprom msg to hal*/
int devm_op_eeprom_info_func(int cmd_type, int slot, int info_type, unsigned char *info_data, int data_len);



/*plout 定时器函数*/
int devm_sig_func(struct thread *para_thread);
/* success, return 0; failed, return -1 */
int devm_init_mul_timer(void);
/* success, return 0; failed, return -1 */
int devm_destroy_mul_timer(void);
/* success, return timer handle(>=0); failed, return -1 */
int devm_set_a_timer(uint32_t interval, int (* timer_proc)(void *arg, int arg_len), void *arg, uint32_t arg_len, uint8_t slot);
/* success, return 0; failed, return -1 */
int devm_del_a_timer(uint8_t slot);

int devm_plug_timer_func(void *arg, int arg_len);

int devm_get_sysObjectid(void);

int devm_config_write(struct vty *vty);

int devm_alarm_socket_msg_rcv(struct thread *t);
uint32_t devm_gpn_function_init(void);
int devm_sendto_hal_and_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
int temper_send_to_master(struct thread *para_thread);

#endif





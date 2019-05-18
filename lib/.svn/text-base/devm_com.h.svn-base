/*
*     common define and function of device management 
*/


#ifndef HIOS_DEV_COM_H
#define HIOS_DEV_COM_H

#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/sys_ha.h>


#define SLOT_PORT_NUM  135   /* 每槽位的端口数量 */
#define SLOT_CARD_NUM  8    /* 每板的子卡数量 */
#define SLOT_NUM_MAX   32   /* 整机最大槽位数量 */
#define UNIT_NUM_MAX   1    /* 整机最大机框数量 */

#define HA_MODULE_ID_MAX 64

#define SUBTYPE_HT157 1

/* 原有设备 id 定义，从 eeprom 读取 */
#define H20PN_1610_4GX_4GE                    0x00
#define H20PN_1611_4GX_4GE_8E1                0x01
#define H20PN_1613_4GX_4GE_2STM1              0x03
#define H20PN_181_GX_4GE_P                    0x30
#define H20PN_181_2GX_4GE_P                   0x32
#define H20PN_181_2GX_2GE_4E1_SP              0x34

#define H20PN_181_GX_4GE_AC                   0x40
#define H20PN_181_GX_4GE_DC                   0x41
#define H20PN_181_GX_4GE_AC_DC                0x42


#define H20PN_181_FX_4GE_P                    0x31
#define H20PN_181_2FX_4GE_P                   0x33
#define H20PN_181_GX_2GE_4E1_SP               0x35
#define H20PN_181_2FX_2GE_4E1_SP              0x36

#define H20PN_181_FX_4GE_AC                   0x43
#define H20PN_181_FX_4GE_DC                   0x44
#define H20PN_181_FX_4GE_AC_DC                0x45

#define H20PN_181_2FX_4GE_AC                  0x49
#define H20PN_181_2FX_4GE_DC                  0x4a
#define H20PN_181_2FX_4GE_AC_DC               0x4b


#define H20PN_181_GX_2GE_4E1_S_AC             0x4c
#define H20PN_181_GX_2GE_4E1_S_DC             0x4d
#define H20PN_181_GX_2GE_4E1_SE_AC_DC         0x4e

#define H20PN_181_2GX_2GE_4E1_S_AC            0x4f
#define H20PN_181_2GX_2GE_4E1_S_DC            0x50
#define H20PN_181_2GX_2GE_4E1_SE_AC_DC        0x51

#define H20PN_181_2FX_2GE_4E1_S_AC            0x58
#define H20PN_181_2FX_2GE_4E1_S_DC            0x59
#define H20PN_181_2FX_2GE_4E1_S_AC_DC         0x5a

#define H20PN_181_2GX_4GE_AC_OLD              0x46
#define H20PN_181_2GX_4GE_DC_OLD              0x47
#define H20PN_181_2GX_4GE_AC_DC_OLD           0x48

#define H20PN_181_4GX_4GE_AC_OLD              0x52
#define H20PN_181_4GX_4GE_DC_OLD              0x53
#define H20PN_181_4GX_4GE_AC_DC_OLD           0x54

#define H20PN_181_4GX_4GE_4E1_AC_OLD          0x55
#define H20PN_181_4GX_4GE_4E1_DC_OLD          0x56
#define H20PN_181_4GX_4GE_4E1_AC_DC_OLD       0x57

/*1000 & 2000 equipment type id*/
#define ID_H20RN2000                          0x0200
#define ID_H20RN1000                          0x0201

/*IPRAN device id*/
#define ID_H20RN_181_2GX_4GE                  0x0401
#define ID_H20RN_181_4GX_4GE                  0x0402
#define ID_H20RN_181_4GX_4GE_4E1              0x0403
#define ID_HT201                              0x0404
#define ID_HT201E                             0x0405
#define ID_HT153                              0x0406
#define ID_HT157                              0x0407
#define ID_HT158                              0x0408

#define ID_HT2200                             0x05F0
#define ID_HT2100                             0x05F1
#define ID_H9MOLMXE_VX                        0x05F2
#define ID_HT2200V2                           0x05F3
#define ID_HT2100V2                           0x05F4

/*slot card type id, for device ID_H20RN2000/ID_H20RN1000*/
#define ID_TU02A                              0x0102
#define ID_TU02                               0x0103
#define ID_GU08                               0x0104
#define ID_GU04                               0x0105
#define ID_FU08                               0x0106
#define ID_CP16                               0x0107
#define ID_CS1Q                               0x0108
#define ID_CS1QT                              0x0109
#define ID_CS12                               0x010a
#define ID_CS22                               0x010b
#define ID_CK01                               0x010c
#define ID_GU08C                              0x0114
#define ID_TU01A                              0x0115
#define ID_GU08E                              0x0117
#define ID_GU04C                              0x0118
#define ID_TU02C                              0x0119
#define ID_TU04C                              0x011a
#define ID_PXM03                              0x0101
#define ID_PXM10                              0x0501

/*slot card type id, for device ID_HT2200/ID_HT2100/ID_H9MOLMXE_VX/ID_HT2200V2*/
#define ID_MX01                               0x0502 /*master and ethernet xchange, ID_HT2200/ID_HT2100/ID_HT2200V2*/
#define ID_GE08                               0x011b /*8*GE, ethernet opt card*/
#define ID_XGE02                              0x011c /*2*10GE, ethernet opt card*/
#define ID_XGE04                              0x011d /*4*10GE, ethernet opt card*/
#define ID_GE08G                              0x011e /*8*GE, ethernet opt card, support GRE*/
#define ID_XGE02G                             0x011f /*2*10GE, ethernet opt card, support GRE*/
#define ID_XGE04G                             0x0120 /*4*10GE, ethernet opt card, support GRE*/
#define ID_GE08E                              0x0121 /*8*GE, ethernet electrical card*/
#define ID_EC16                               0x0122 /*16*e1, 16*ces card*/
#define ID_SC01Q                              0x0123 /*4*stm1, 126*ces card*/
#define ID_SC01QE                             0x0124 /*4*stm1 or 4*stm4*/
#define ID_PW63                               0x0125 /*63*ces card, e1 from VX-T-SDHDXC, only for H9MOLMXE-VX */
#define ID_GE04                               0x0126 /*4*GE, ethernet opt card*/
#define ID_GE04G                              0x0127 /*4*GE, ethernet opt card*/
#define ID_GE04E                              0x0128 /*4*GE, ethernet electrical card*/


/*slot card type id, for device ID_H9MOLMXE_VX*/
#define ID_MX02                               0x0584 /*master and ethernet xchange, ID_H9MOLMXE_VX*/
#define ID_FE01                               0X1105 /*H9MO-LMXE.FE01:     4*FE, 4*EoS-GFP-VCG*/
#define ID_FE64                               0X116A /*H9MO-LMXE.VI.FE64:  */
#define ID_GX05C                              0X1175 /*H9MO-LMXE.VI.GX05C: 2*GE, 16*EoS-GFP-VCG*/
#define ID_TX02                               0X1183 /*H9MO-LMXE.VX.TX02:  SDH master and sdh xchange*/
#define ID_OS16A                              0X1185 /*H9MO-LMXE.VX.OS16A: 4*STM-16*/
#define ID_OS04A                              0X1186 /*H9MO-LMXE.VX.OS04A: 4*STM-4*/
#define ID_OS01A                              0X1187 /*H9MO-LMXE.VX.OS01A: 4*STM-1*/
#define ID_OS01S                              0X11A4 /*H9MO-LMXE.VX.OS01S: 2*STM-1*/
#define ID_ES08                               0X1188 /*H9MO-LMXE.VX.ES08:  8*GE, 8*EoS-GFP-VCG, 32*VC4*/
#define ID_ES32S                              0X1189 /*H9MO-LMXE.VX.ES32S: 32*EoS-GFP-VCG, 32*VC4*/
#define ID_ES08S                              0X118A /*H9MO-LMXE.VX.ES08S: 8*GE fiber, 8*EoS-GFP-VCG, 32*VC4*/

#define ID_LA01                               0X020B /*H9MO-LMXE.VI.LA01   */
#define ID_FE02                               0x020F /*H9MO-LMXE.VR.FE02   */
#define ID_SD02                               0X025C /*H9MO-LMXE.VI.SD02:  */
#define ID_CHD01                              0X025D /*H9MO-LMXE.VI.CHD01  */
#define ID_DIO20                              0x025F /*H9MO-LMXE.VR.DIO20  */
#define ID_C37D                               0X0265 /*H9MO-LMXE.VI.C37D:  */
#define ID_SD03                               0X026B /*H9MO-LMXE.VI.SD03   */
#define ID_CHU02                              0X026E /*H9MO-LMXE.VI.CHU02: */
#define ID_DIO04                              0X0273 /*H9MO-LMXE.VI.DIO04: */
#define ID_CH4W02                             0X0274 /*H9MO-LMXE.VI.CH4W02 */
#define ID_CHU03                              0X027F /*H9MO-LMXE.VI.CHU03: */
#define ID_CH4W03                             0X028C /*H9MO-LMXE.VI.CH4W03 */
#define ID_EP01                               0x0304 /*H9MO-LMXE.VI.EP01:  24*E1*/
#define ID_EP03                               0x0308 /*H9MO-LMXE.VI.EP03:  12*E1*/



/* 设备类型定义 */
enum DEV_TYPE
{
	DEV_TYPE_INVALID = 0,
	DEV_TYPE_BOX,        /* 盒式设备，例如 181 系列 */
	DEV_TYPE_SHELF,      /* 单框设备，例如 2000 系列有背板的设备 */	
	DEV_TYPE_STACK       /* 多框堆叠设备，暂时没有 */
};


/* 板卡类型定义 */
enum SLOT_TYPE
{
	SLOT_TYPE_INVALID = 0,
    SLOT_TYPE_BOARD_MAIN,      /* 主控板 */    
    SLOT_TYPE_BOARD_SLAVE,     /* 备用主控板*/
    SLOT_TYPE_CARD_ETH,        /*以太接口卡*/
    SLOT_TYPE_CARD_TDM,        /*TDM 接口卡*/
    SLOT_TYPE_CARD_STM,        /*STM 接口卡*/
    SLOT_TYPE_CARD_CLOCK,      /*时钟接口卡*/    
    SLOT_TYPE_POWER,           /*电源板*/
    SLOT_TYPE_FAN,             /*风扇板*/
    SLOT_TYPE_SDH,             /*VX T-side card*/
    SLOT_TYPE_MAX = 255
};


/* 设备状态定义 */
enum DEV_STATUS
{
	DEV_STATUS_UNKOWN = 0, /* 不能识别的板卡 */
	DEV_STATUS_PLUGIN,     /* 板卡插入 */
	DEV_STATUS_PLUGOUT,	   /* 板卡拔出 */	
	DEV_STATUS_HIDE,	   /* 板卡隐藏 */
	DEV_STATUS_REBOOT,     /* 板卡 reboot */
	DEV_STATUS_RESET,      /* 板卡 reset */
	DEV_STATUS_WORK,       /* 板卡正常工作 */
	DEV_STATUS_MAX = 16
};


/* 设备管理信息类型 */
enum DEVM_INFO
{
	DEVM_INFO_INVALID = 0,		
	DEVM_INFO_MAC,		 /* MAC 信息 */	
	DEVM_INFO_BOX,		 /* 盒式设备信息 */	
	DEVM_INFO_UNIT,		 /* unit 信息 */
	DEVM_INFO_SLOT,		 /* slot 信息 */	
	DEVM_INFO_MAINSLOT,	 /* 主卡slot槽位号 ? */	
	DEVM_INFO_ID,		 /* ID 信息 */
	DEVM_INFO_SLOT_NAME, /* slot name 信息*/
	DEVM_INFO_DEV_NAME,  /* device name 信息*/
	DEVM_INFO_STATUS,    /* slot 状态信息*/
	DEVM_INFO_MANU,      /* 厂商名信息 */
	
	DEVM_INFO_TEMP,      /* temprature信息 */
	DEVM_INFO_POWER,     /* power信息 */
	DEVM_INFO_FAN,       /* fan信息 */
	DEVM_INFO_RESET,     /*equipment reset msg*/
	DEVM_INFO_REBOOT,    /*equipment reboot msg*/
	DEVM_INFO_SETMX,
	DEVM_INFO_RESET_MASTER,
	DEVM_INFO_PORT,
	DEVM_INFO_PHYID,
	DEVM_INFO_FIBER,
	
	DEVM_INFO_GPORT,	
	DEVM_INFO_CHIPID,
	DEVM_INFO_WATCHDOG,
	DEVM_INFO_HEART_BEAT,
	DEVM_INFO_DEV_TYPE, /*设备类型信息*/
	DEVM_INFO_GRE,      /* 待删除 */
	DEVM_INFO_RUNNINGTIME,
	DEVM_INFO_PLUGOUT,
	DEVM_INFO_PLUGIN,
	DEVM_INFO_UP_FPGA_VER,
	
	DEVM_INFO_RTC,	
	DEVM_INFO_HA,        /* HA 消息 */ 
	DEVM_INFO_MASTER_BACKUP, /*主备倒换*/
	DEVM_INFO_FPGA_RECONFIG, /*主备倒换*/
	DEVM_INFO_GE08E_PORT,
	DEVM_INFO_HAL_DEVICE_STATUS,
	DEVM_INFO_HAL_SLOT_PLUGSTATUS_HASH,
	DEVM_INFO_EEPROM_INFO,
	DEVM_INFO_STRUCT_DEVM,	 /* get struct devm device */
	DEVM_INFO_MYUNIT,

	DEVM_INFO_SYS_OBJECTID,
	DEVM_INFO_HA_BOOTSTATE_CHANGE,
	DEVM_INFO_SET_DYNAMIC_VOLTAGE,
	DEVM_INFO_CTRL_SLOT_POWER,
	DEVM_INFO_SET_HAL_HASH,
	DEVM_INFO_FINISH_CREATE_SLOTS,
	DEVM_INFO_EEPINFO_OP,
	DEVM_INFO_SYS_VERSION,
    DEVM_INFO_PARTNERSLOT,
	DEVM_INFO_FPGA_UPGRADE_REGISTER,

    DEVM_INFO_SDHMGT,
    DEVM_INFO_SDHMGT_SDH_MASTER,
    DEVM_INFO_SLOT_VERSION,

    DEVM_INFO_E2DATA_HAGET,

    DEVM_INFO_REBOOT_COUNT,
	DEVM_INFO_SLAVE_TEMP,

    DEVM_INFO_MAX = 255
};


/* 设备管理事件定义 */
enum DEV_EVENT
{
	DEV_EVENT_INVALID = 0,
	DEV_EVENT_SLOT_ADD,        /* 板卡插入 */
	DEV_EVENT_SLOT_DELETE,     /* 板卡拔出 */
	DEV_EVENT_PORT_ADD,        /* 端口创建 */
	DEV_EVENT_PORT_DELETE,     /* 端口删除 */
	DEV_EVENT_UNIT_UP,         /* 设备上电 */
	DEV_EVENT_UNIT_DOWN,       /* 设备掉电*/
	DEV_EVENT_SLOT_ADD_FINISH,       /* 设备上电板卡加载完成*/
	DEV_EVENT_HA_BOOTSTATE_CHANGE,   /* 设备状态切换*/
	
	DEV_EVENT_MAX = 16
};


enum HA_INFO
{
	HA_INFO_APP_INIT,

	HA_INFO_MAX = 5
};

enum DEVM_DIGITLABL
{
	DEVM_DIGITAL_LABEL_HWCFG = 0,
	DEVM_DIGITAL_LABEL_MAC,
	DEVM_DIGITAL_LABEL_OEM,
	DEVM_DIGITAL_LABEL_VND,
	DEVM_DIGITAL_LABEL_BOM,
	DEVM_DIGITAL_LABEL_DEV,
	DEVM_DIGITAL_LABEL_DATE,	
	DEVM_DIGITAL_LABEL_HVER,	
	DEVM_DIGITAL_LABEL_DVER,
	DEVM_DIGITAL_LABEL_SN,
	DEVM_DIGITAL_LABEL_LIC,
	DEVM_DIGITAL_LABEL_CFGINIT,
	DEVM_DIGITAL_LABEL_MAX = 15,
};

/* 设备管理公共数据结构 */
struct devm_com
{
	enum DEV_STATUS status;
	enum SLOT_TYPE  type;
	enum HA_BOOTSTATE ha_status;
	enum HA_ROLE ha_role;
	uint8_t unit;
	uint8_t slot;
	uint8_t main_slot;
	uint8_t slave_slot;
	uint8_t port;
	uchar mac[6];
	uchar pad[1];
};


/* HA module event broadcast struct*/
struct ha_com
{
	int module_id;
	int module_init_count;
};


/* 端口信息 */
struct devm_port
{
     enum IFNET_SUBTYPE port_type;  
	 uint8_t unit;
	 uint8_t slot;
     uint8_t port;/* 面板端口号 */
     uint8_t fiber;   /* 0: 光口， 1: 电口 */
     uint8_t hide;    /* 1: 隐藏 */
     uint8_t pad;
     uchar mac[6];
};


/* 硬件信息 */
struct hw_info
{
	uint8_t hw_ver[64];			  /* 硬件版本, 从 EEPROM 读取，0 表示无效 */
    uint16_t fpga_ver;			  /* FPGA 版本, 从 FPGA 读取，0 表示无效 */
	uint8_t  cpu_type;            /* cpu 型号*/	
	uint8_t  flash_type;          /* flash 型号*/
	uint8_t  sd_type;             /* sd 卡型号*/	
	uint8_t  mem_type;            /* 内存 型号*/
	uint8_t  pad[2];  
};


/* 温度信息 */
struct devm_temp
{
	signed char temprature;/*当前温度*/	
	signed char temp_low;  /*最低温度*/
	signed char temp_high; /*最高温度*/
	signed char pad;
};

typedef struct devm_temp_snmp
{
	signed int temprature;
	signed int temp_low; 
	signed int temp_high; 
}devmtemper;

struct devm_oid
{
    uint32_t devId;
    uint8_t  oidLen;
    uint32_t oid[32];
};

/* 风扇信息 */
struct devm_fan
{
	uint16_t mode;      /*风扇工作模式*/
	uint16_t speed;     /*风扇当前转速*/	
	uint16_t speed_min; /*风扇最小转速*/
	uint16_t speed_max; /*风扇最大转速*/
};


/* 电源信息 */
struct devm_power
{
	uint8_t  mode;         /*电源工作模式*/
	uint8_t  voltage_output;      /*当前电压*/	
	uint32_t voltage_input;      /*当前电压*/	
	uint8_t voltage_low;  /*最低电源*/
	uint8_t voltage_high; /*最高电压*/
    uint8_t voltage_dynamic; /* dynamic  1?enable : disable */	
};
struct devm_version /*current only for message*/
{
    char    softver[NAME_STRING_LEN];
    char    fpgaver[NAME_STRING_LEN];
    char    hwver[NAME_STRING_LEN];
    char    kernelver[NAME_STRING_LEN];
};


/* 子卡信息, 子卡没有 CPU，只有 FPGA */
struct devm_card
{
	enum SLOT_TYPE  type;
	uchar    name[NAME_STRING_LEN];       /* 子卡名, 从 FPGA 读取 */	
	uint16_t id;				          /* 子卡 ID, 从 FPGA 读取, 0 表示无效  */
	uint16_t hw_ver;			          /* 硬件版本, 从 FPGA 读取，0 表示无效 */
    uint16_t fpga_ver;			          /* FPGA 版本, 从 FPGA 读取，0 表示无效 */
    uint8_t  slot;                        /* 子卡的逻辑槽位号 */	
    uint8_t  slot_hw;                     /* 子卡的硬件槽位号 */
	uint8_t  port_num;                    /* 端口数量 */
	uint8_t  pad[3];
	struct list port_list;				  /* 端口信息*/
};


/* 盒式设备信息 */
struct devm_box
{
	enum DEV_TYPE     type;                   /* 设备类型 */	
	uchar    dev_name[NAME_STRING_LEN];       /* 设备名, 从 EEPROM 读取*/
	uchar    oem_name[NAME_STRING_LEN];       /* OEM设备名, 从 EEPROM 读取*/
	uchar    manufacture[NAME_STRING_LEN];    /* 厂商名, 从 EEPROM 读取*/
	uchar    mac[6];                          /* 设备 mac, 从 EEPROM 读取 */	
	uint16_t id;                              /* 设备 ID, 从 EEPROM 读取 */
	uint16_t mac_num;	                      /* mac 数量, 从 EEPROM 读取 */
	uint8_t  port_num;                        /* 最大端口数量 */	
	uint8_t  card_num;                        /* 最大子卡数量 */
	uint8_t  watchdog;                        /* 看门狗喂狗间隔，0 表示不使能*/
    uint8_t  dev_subtype;                      /* HT157/HT158 subtype = 1 */
	time_t run_time;                        /* 运行时间，单位: 秒 */
	struct list card_list;                    /* 子卡链表，保存 devm_card 指针 */
	struct list port_list;                    /* 端口链表，保存 devm_port 指针 */
	struct hw_info    hw;                     /* 硬件信息*/
	struct devm_temp  temp;                   /* 板卡温度 */
	struct devm_fan   fan;                    /* 风扇，根据设备 ID 获取 */
	struct devm_power power[2];               /* 主备电源，根据设备 ID 获取 */
};


/* 板卡信息 */
struct devm_slot
{
	enum SLOT_TYPE  type;            /* 板卡类型 */
	enum DEV_STATUS status;          /* 板卡状态 */
	uchar name[NAME_STRING_LEN];     /* 板卡名, 从 EEPROM 读取 */
	uchar dev_name[NAME_STRING_LEN]; /* 设备名, 从 EEPROM 获取 */
	uchar mac[6];                    /* 板卡 MAC，从 EEPROM 读取 */
	uint16_t id;                     /* 板卡 ID, 从 EEPROM 读取 */
	uint8_t unit;                    /* unit 编号 */
	uint8_t slot;                    /* slot 编号 */
    uint8_t slot_hw;                 /* 硬件槽位号 */
	uint8_t port_num;                /* 最大端口数量 */
	uint8_t card_num;                /* 最大子卡数量 */	
	uint8_t watchdog;                /* 看门狗喂狗间隔，0 表示不使能*/
	uint8_t heartbeat;               /* 待删除 */
	uint8_t plugout;                 /* 1 表示slot 隐藏, 使用 DEV_STATUS_HIDE 状态, 待删除 */
	struct list card_list;           /* 子卡链表，保存 devm_card 指针 */
	struct list port_list;           /* 端口链表，保存 devm_port 指针 */
	struct hw_info    hw;            /* 硬件信息*/
	struct devm_temp  temp;          /* 板卡温度 */
	time_t run_time;               /* 板卡启动时间 */
};


/*背板信息*/
struct devm_backplane
{
	uint16_t id;                              /* 设备 ID, 从背板 EEPROM 读取 */
	uint16_t version;                         /* 硬件版本, 从背板 EEPROM 读取 version*/
	uint16_t mac_num;	                      /* mac 数量, 从背板 EEPROM 读取 mac_num*/	
	uchar  mac[6];                            /* 设备 mac, 从背板 EEPROM 读取 mac*/
    uchar  HardwVersion[64];
	uchar dev_name[NAME_STRING_LEN];          /* 设备名, 从背板 EEPROM 读取 device*/
	uchar oem_name[NAME_STRING_LEN];          /* OEM设备名, 从 EEPROM 获取       Oem_device_name*/
	uchar manufacture[NAME_STRING_LEN];       /* 厂商名, 从背板 EEPROM 读取 Vendor*/
};



/* 设备信息 */
struct devm
{
	enum DEV_TYPE     type;                 /* 设备类型 */	
	uint8_t           unit_num;             /* 机框数量 */
	uint8_t           main_unit;            /* 主框号，用于 stack 设备类型 */	
	uint8_t           myunit;               /* 当前所在的框号 */
	uint8_t           myslot;               /* 当前所在的 slot 号 */	
	enum SLOT_TYPE    slot_type;            /* 当前所在 slot 的类型 */
	struct devm_unit *pmyunit;              /* 当前所在的机框 */	
	struct devm_unit *punit[UNIT_NUM_MAX];  /* 机框信息 */
	struct devm_box  *pbox;                 /*盒式设备信息*/
};


/* 设备的主备状态 */
enum DEVM_HA_E
{
	DEVM_HA_INVALID = 0,    /* 非主控板 */
	DEVM_HA_MASTER,         /* 主用主控板 */
	DEVM_HA_SLAVE           /* 备用主控板 */
};


/* 机框信息 */
struct devm_unit
{	
	uint8_t slot_num;                  /* all slot num of the device */	
	uint8_t slot_main_board;	       /* mainboard slot */
	uint8_t slot_slave_board;          /*  slaveboard slot */	
	uint8_t myslot;                    /*my board slot num */
	enum DEVM_HA_E slot_ha;            /* the ha role of myslot is main or slave*/
	uint8_t heart_beat;                /* 心跳报文发送间隔，0 表示不发送*/
	uint8_t watchdog;                  /* 看门狗喂狗间隔，0 表示不使能*/
	uint8_t pad[2];
	struct devm_fan fan;               /* fan info，根据设备 ID 获取 */
	struct devm_power power[2];        /*power info，根据设备 ID 获取 */
	struct devm_backplane bplane;      /* back board info,read from backboard EEPROM*/
	struct devm_slot *pslot[SLOT_NUM_MAX];  /* slots info */
};

/* H9MOLMXE_VX, mount or unmount SDH/PCM card message */
struct devm_sdhmgt_card_mnt		
{
    uint8_t     opcode;         // 1:add, 2:delete
    uint8_t     slot;           // [1-15]
    uint8_t     id[4];
    uint8_t     soft_version;
    uint8_t     fpgaa_version;  // 1st fpga version
    uint8_t     fpgab_version;  // 2nd fpga version
};

struct devm_sdhmgt_sdh_master		
{
    uint8_t     slot;           // [0/8/9], only for H9MOLMXE_VX
    uint8_t     pad[3];
};

typedef struct devm_digtlabl
{
	uchar hwcfg[NAME_STRING_LEN];
	uchar mac[NAME_STRING_LEN];
	uchar oem[NAME_STRING_LEN];
	uchar vnd[NAME_STRING_LEN];
	uchar bom[NAME_STRING_LEN];
	uchar dev[NAME_STRING_LEN];
	time_t date;
	uchar hver[NAME_STRING_LEN];
	uchar dver[NAME_STRING_LEN];
	uchar sn[NAME_STRING_LEN];    
	uchar lic[NAME_STRING_LEN+1];
	uchar cfginit[NAME_STRING_LEN];	
}devdigtlabl;

typedef struct devm_digtlabl_index
{
	uint32_t slot;
	
}devdigtlablindex;

typedef struct devm_digtlabl_data
{
    devdigtlabl digtlabldata;
        
}devdigtlabldata;


typedef struct devm_digtlabl_info
{
	devdigtlablindex 	index;	
	devdigtlabldata 	data;	
}devdigtlablinfo;


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

int devm_event_register ( enum DEV_EVENT type, int module_id, uint8_t queue_id );


/* app call this function to get device oid */
/*unit start from 1, slot 0 means backplane mac*/ 
int devm_comm_get_mac(uint8_t unit, uint8_t slot, int module_id, uchar *mac);
int devm_comm_get_id(uint8_t unit, uint8_t slot, int module_id, unsigned int *para_dev_id);
int devm_comm_get_running_time(unsigned char unit, unsigned char slot, unsigned int module_id, time_t *current_secs);
int devm_comm_get_runtime(uint8_t unit, uint8_t slot, int module_id, unsigned int * para_time);
int devm_comm_get_oem_name(uint8_t unit, uint8_t slot, int module_id, uchar * para_oem_name);
int devm_comm_get_dev_name(uint8_t unit, uint8_t slot, int module_id, uchar * para_dev_name);
int devm_comm_get_slot_name(uint8_t unit, uint8_t slot, int module_id,uchar * para_slot_name);
int devm_comm_get_slot_status(uint8_t unit, uint8_t slot, int module_id, enum DEV_STATUS *slot_status);
int devm_comm_get_manufacture(uint8_t unit, int module_id, uchar *para_manu);
int devm_comm_get_dev_type(int module_id, enum DEV_TYPE *para_dev_type);
unsigned char devm_comm_get_mainslot(int module_id);
int devm_comm_get_fpga_ver(uint8_t unit,uint8_t slot,uint32_t module_id, int16_t *fpgaid);
int devm_comm_get_cpu(uint32_t module_id, uint8_t unit,uint8_t slot,int *cpu_type);
int devm_comm_get_Hardware_Version(uint8_t unit, uint8_t slot,uint32_t module_id, unsigned char *Hw_Ver, unsigned int data_len);
int devm_comm_get_dev_temprature(uint8_t unit,uint32_t module_id, signed char *temprature, unsigned int data_len);
void *devm_comm_get_dev_scalar(uint32_t module_id, uint32_t sub_type);
int devm_comm_set_rtc (int module_id, uint8_t unit, time_t rtc_time);
int devm_comm_set_fpga_upgrade_register(int module_id, unsigned char pos);

int devm_comm_get_device(int module_id,struct devm_unit *para_devm_unit,struct devm_box *para_devm_box,enum DEV_TYPE *para_dev_type);
int devm_comm_get_box(int module_id,struct devm_box *para_dev_box);
int devm_comm_get_unit(uint8_t unit, int module_id,struct devm_unit *para_devm_unit);
int devm_comm_get_slot(uint8_t unit, uint8_t slot, int module_id,struct devm_slot *para_devm_slot);
int devm_comm_get_mydevice_info(int module_id, struct devm *p_devm);
int devm_comm_get_partner_slot(unsigned int unit, unsigned int module_id, unsigned char *para_partnerslot);
int devm_comm_get_myunit_info(int module_id, struct devm_unit *para_myunit);
int devm_comm_get_sys_version ( uchar* sys_ver);
int devm_comm_get_sysObjectid ( int module_id, unsigned int *oidLen, unsigned int *sysobjectid);

int devm_comm_get_slot_version(uint8_t unit, uint8_t slot, int module_id,struct devm_version *pversion);
devdigtlablinfo *devm_comm_reply_snmp_bulk (devdigtlablindex *index, int *lablnum);


#endif

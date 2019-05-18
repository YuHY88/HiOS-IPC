/*
 *  define IPC message queue operation
 */


#ifndef HIOS_SYSHA_H_
#define HIOS_SYSHA_H_


/*HA 子消息类型*/
enum HA_SUBMSG
{
    HA_SMSG_HEARTBEAD = 1,     /*板间心跳消息,周期性发送,超时未收到可导致倒换*/
                               /*主板板卡的DEVM之间交互  */
    HA_SMSG_ROLE_NOTI,         /*CPU板卡HA角色通知 DEVM->APP*/

    HA_SMSG_BOOT_STATE,        /*本板的启动状态 VTYSH->DEVM*/

    HA_SMSG_BATCH_START,       /*配置数据批备内部请求 VTYSH->FILEM*/

    HA_SMSG_BATCH_REQ,         /*配置数据批备内部请求 DEVM->VTYSH or DEVM->DEVM*/
    HA_SMSG_BATCH_REQACK,      /*配置数据批备内部应答 VTYSH->DEVM or DEVM->DEVM 需要两个应答 一个启动配置文件 一个运行配置文件*/

    HA_SMSG_BATCH_DAT,         /*配置数据批备板间传送, DEVM->DEVM*/
    HA_SMSG_BATCH_DATACK,      /*配置数据批备板间传送, DEVM->DEVM*/
    HA_SMSG_BATCH_END,         /*配置数据批备板间传送结束, FILEM->VTYSH*/
    HA_SMSG_BATCH_ENDACK,      /*配置数据批备板间传送结应答, DEVM->DEVM or DEVM->VTYSH*/

    HA_SMSG_FILE_REQ,          /*文件传输请求 FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_REQACK,       /*文件传输请求应答 FILEM->DEVM<->DEVM->FILEM*/

    HA_SMSG_FILE_DAT,          /*文件数据传送, FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_DATACK,       /*文件数据传送应答, FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_END,          /*文件数据传送结束, FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_ENDACK,       /*文件数据传送结束应答, FILEM->DEVM<->DEVM->FILEM*/

   
    HA_SMSG_REALT_PERMIT,      /*配置数据实时备份请求 DEVM->VTYSH 批备完成后,通知VTYSH可以实时备份了*/
    HA_SMSG_REALT_REJECT,      /*配置数据实时备份禁止 DEVM->VTYSH*/
    
    HA_SMSG_REALT_DATA,        /*配置数据实时备份数据 VTYSH->DEVM ->备用板 -> DEVM -> VTYSH */
                               /*小于9K的实时数据*/
    HA_SMSG_APPRUN_DATA,       /*APP 运行数据备份 APP->DEVM ->备用板 -> DEVM -> APP */
                               /*只能同一模块间发送, 数据格式内部定义*/
    HA_SMSG_REALT_CMD,         /*配置数据实时操作指令 VTYSH->DEVM ->备用板 -> DEVM -> VTYSH */

    HA_SMSG_DATA_REPORT,       /*向主CPU通报信息*/

    HA_SMSG_EVENT_REPORT,      /*向主CPU通报事件*/

    HA_SMSG_TXFILE_REQ         /*SDHMGT->DEVM*/
};

/*HA 主控CPU较色*/
enum HA_ROLE
{
    HA_ROLE_INIT = 1,          /*HA 角色*/
        
    HA_ROLE_MASTER,

    HA_ROLE_SLAVE 
};

/* HA 消息类型 */
enum HA_BOOTSTATE
{        
	HA_BOOTSTA_START = 1,     /* 设备启动开始 */
	
	HA_BOOTSTA_END,      	  /* 设备启动完成 */
	
	HA_BOOTSTA_DATAOK         /* 数据加载完成 */
};


struct ha_btb_hbmsg
{
    enum HA_ROLE       role;   /* 发送者HA角色*/

    enum HA_BOOTSTATE  boot;   /* 发送者启动状态*/

    /*可添加信息*/
};

struct ha_batch_req            /*批备数据请求*/
{
    int                unit;   
    
    int                slot;   /*目的槽位号 */
                               /*传输文件名 */
    char               bFile[128];
};

#define HA_CONFIG_BATCHSYNC_FILE  "/data/cfg/config_batch_sync.tmp"

#endif /* _MSG_IPC_H_ */

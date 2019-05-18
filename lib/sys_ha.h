/*
 *  define IPC message queue operation
 */


#ifndef HIOS_SYSHA_H_
#define HIOS_SYSHA_H_


/*HA ����Ϣ����*/
enum HA_SUBMSG
{
    HA_SMSG_HEARTBEAD = 1,     /*���������Ϣ,�����Է���,��ʱδ�յ��ɵ��µ���*/
                               /*����忨��DEVM֮�佻��  */
    HA_SMSG_ROLE_NOTI,         /*CPU�忨HA��ɫ֪ͨ DEVM->APP*/

    HA_SMSG_BOOT_STATE,        /*���������״̬ VTYSH->DEVM*/

    HA_SMSG_BATCH_START,       /*�������������ڲ����� VTYSH->FILEM*/

    HA_SMSG_BATCH_REQ,         /*�������������ڲ����� DEVM->VTYSH or DEVM->DEVM*/
    HA_SMSG_BATCH_REQACK,      /*�������������ڲ�Ӧ�� VTYSH->DEVM or DEVM->DEVM ��Ҫ����Ӧ�� һ�����������ļ� һ�����������ļ�*/

    HA_SMSG_BATCH_DAT,         /*��������������䴫��, DEVM->DEVM*/
    HA_SMSG_BATCH_DATACK,      /*��������������䴫��, DEVM->DEVM*/
    HA_SMSG_BATCH_END,         /*��������������䴫�ͽ���, FILEM->VTYSH*/
    HA_SMSG_BATCH_ENDACK,      /*��������������䴫�ͽ�Ӧ��, DEVM->DEVM or DEVM->VTYSH*/

    HA_SMSG_FILE_REQ,          /*�ļ��������� FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_REQACK,       /*�ļ���������Ӧ�� FILEM->DEVM<->DEVM->FILEM*/

    HA_SMSG_FILE_DAT,          /*�ļ����ݴ���, FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_DATACK,       /*�ļ����ݴ���Ӧ��, FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_END,          /*�ļ����ݴ��ͽ���, FILEM->DEVM<->DEVM->FILEM*/
    HA_SMSG_FILE_ENDACK,       /*�ļ����ݴ��ͽ���Ӧ��, FILEM->DEVM<->DEVM->FILEM*/

   
    HA_SMSG_REALT_PERMIT,      /*��������ʵʱ�������� DEVM->VTYSH ������ɺ�,֪ͨVTYSH����ʵʱ������*/
    HA_SMSG_REALT_REJECT,      /*��������ʵʱ���ݽ�ֹ DEVM->VTYSH*/
    
    HA_SMSG_REALT_DATA,        /*��������ʵʱ�������� VTYSH->DEVM ->���ð� -> DEVM -> VTYSH */
                               /*С��9K��ʵʱ����*/
    HA_SMSG_APPRUN_DATA,       /*APP �������ݱ��� APP->DEVM ->���ð� -> DEVM -> APP */
                               /*ֻ��ͬһģ��䷢��, ���ݸ�ʽ�ڲ�����*/
    HA_SMSG_REALT_CMD,         /*��������ʵʱ����ָ�� VTYSH->DEVM ->���ð� -> DEVM -> VTYSH */

    HA_SMSG_DATA_REPORT,       /*����CPUͨ����Ϣ*/

    HA_SMSG_EVENT_REPORT,      /*����CPUͨ���¼�*/

    HA_SMSG_TXFILE_REQ         /*SDHMGT->DEVM*/
};

/*HA ����CPU��ɫ*/
enum HA_ROLE
{
    HA_ROLE_INIT = 1,          /*HA ��ɫ*/
        
    HA_ROLE_MASTER,

    HA_ROLE_SLAVE 
};

/* HA ��Ϣ���� */
enum HA_BOOTSTATE
{        
	HA_BOOTSTA_START = 1,     /* �豸������ʼ */
	
	HA_BOOTSTA_END,      	  /* �豸������� */
	
	HA_BOOTSTA_DATAOK         /* ���ݼ������ */
};


struct ha_btb_hbmsg
{
    enum HA_ROLE       role;   /* ������HA��ɫ*/

    enum HA_BOOTSTATE  boot;   /* ����������״̬*/

    /*�������Ϣ*/
};

struct ha_batch_req            /*������������*/
{
    int                unit;   
    
    int                slot;   /*Ŀ�Ĳ�λ�� */
                               /*�����ļ��� */
    char               bFile[128];
};

#define HA_CONFIG_BATCHSYNC_FILE  "/data/cfg/config_batch_sync.tmp"

#endif /* _MSG_IPC_H_ */

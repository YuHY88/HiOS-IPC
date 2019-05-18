#include <string.h>
#include <stdlib.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/oam_common.h>
#include <lib/types.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <ftm/pkt_eth.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/alarm.h>
#include <lib/ospf_common.h>
#include <lib/hash1.h>
#include <lib/vty.h>

//#include <lib/snmp_common.h>
#include "efm_agent.h"
#include "../l2_snmp.h"


uint8_t oui_data[5] = {0x00,0x1d,0x80,0x81,0x00};
uint8_t msdh_DstAdrr[] = {0xc0,0xc0,0xc0,0xc0};
uint8_t msdh_SrcAdrr[] = {0xc0,0xc0,0x04,0xff};

/*==================================================*/
/*name : efm_parse_remote_device_info                                                                     */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse and get remote device info                                            */ 
/*==================================================*/

int efm_parse_remote_device_info(efm_agent_remote_base_info * aefm_RbaseInfo,uint8_t * buf,uint16_t msdh_length)
{
	int i = 0;

	if(NULL == aefm_RbaseInfo)
	{
		zlog_err("%s[%d]aefm remote struct  is NULL!",__FUNCTION__,__LINE__);
		return -1;
	}

	EFM_LOG_DBG("%s[%d]:parse remote  device info",__FUNCTION__,__LINE__);
	/*record get remote device info*/

	aefm_RbaseInfo->efm_remote_reply_words = buf[i++];
	aefm_RbaseInfo->efm_remote_fpga_ver = buf[i++];
	aefm_RbaseInfo->efm_remote_soft_ver = buf[i++];
	aefm_RbaseInfo->efm_remote_hard_ver = buf[i++];
	aefm_RbaseInfo->efm_remote_base_info_len = buf[i++];
	aefm_RbaseInfo->efm_remote_state_info_len = buf[i++];
	aefm_RbaseInfo->efm_remote_set_info_len = buf[i++];
	aefm_RbaseInfo->efm_remote_specific_info_len = buf[i++];
	aefm_RbaseInfo->efm_remote_E1_num = buf[i++];
	aefm_RbaseInfo->efm_remote_V35_num = buf[i++];
	aefm_RbaseInfo->efm_remote_eth_num = buf[i++];
	aefm_RbaseInfo->efm_remote_olt_num = buf[i++];
	aefm_RbaseInfo->efm_remote_eth_internal_num = buf[i++];
	aefm_RbaseInfo->efm_remote_tabs_data_len = buf[i++];
	aefm_RbaseInfo->efm_remote_device_channel_num = buf[i++];
	aefm_RbaseInfo->efm_remote_device_channel_type = buf[i++];


	if(i>msdh_length)
		return -1;

	return i;
}
/*==================================================*/
/*name : efm_parse_msdh_device_info                                                                     */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote device info                                            */ 
/*==================================================*/

int efm_parse_msdh_device_info(efm_agent_info *aefm,uint8_t * buf,uint16_t msdh_length)
{
	int i = 0;
	int ret = 0;
	
	if(NULL == buf || NULL == aefm )
	{
		zlog_err("%s[%d] buf is NULL",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm->efm_agent_remote_device_sid= buf[i++];
	aefm->efm_agent_remote_device_tid=  buf[i++];
	EFM_LOG_DBG("%s:DEID = %02x DETID = %02x\n",__FUNCTION__,aefm->efm_agent_remote_device_sid,\
			aefm->efm_agent_remote_device_tid);

	msdh_length -= i;

	if(NULL == aefm->aefm_RbaseInfo)
	{

		zlog_err("%s[%d]aefm  device space  is NULL!",__FUNCTION__,__LINE__);
		return -1;

	}
	memset(aefm->aefm_RbaseInfo,0,sizeof(efm_agent_remote_base_info));
	/*parse and record get remote device info*/
	ret = efm_parse_remote_device_info(aefm->aefm_RbaseInfo,buf+i,msdh_length);
	if(ret < 0)
	{
		zlog_err("%s[%d]aefm parse device info fail",__FUNCTION__,__LINE__);
		return ret;
	}

	return i;
}
/*==================================================*/
/*name : efm_parse_remote_getinfo                                                                    */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote status info                                            */ 
/*==================================================*/

int efm_parse_remote_getinfo( efm_agent_remote_state_info * aefm_RstInfo,uint8_t * buf,uint8_t msdh_length)
{
	int i = 0;
	int j = 0;
	int m = 0;
	uint8_t addr_start = 0;
	uint8_t addr_len = 0;

	aefm_RstInfo->efm_remote_reply_words = buf[i++];		
	aefm_RstInfo->efm_remote_start_addr_num = buf[i++];

	for(j = 0;j <aefm_RstInfo->efm_remote_start_addr_num;j++ )
	{
		aefm_RstInfo->efm_remote_start_addr = buf[i++];
		aefm_RstInfo->efm_remote_addr_len = buf[i++];
		addr_start = aefm_RstInfo->efm_remote_start_addr;
		addr_len = aefm_RstInfo->efm_remote_addr_len;
		
		if(addr_start > EFM_AGENT_STATE_DATA_MAX || addr_len > EFM_AGENT_STATE_DATA_MAX )
		{
			zlog_err("%s[%d] get remote agent info error ! data is over length 0x77",__FUNCTION__,__LINE__);
			return -1;
		}
		for(m = 0;m<addr_len;m++)
		{
			aefm_RstInfo->efm_agent_remote_state_data[addr_start+m] = buf[i++];
		}
	}
	if(i > msdh_length)
		return -1;
	
	return i;

}
/*==================================================*/
/*name : efm_parse_msdh_getinfo                                                                         */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote status info                                                                       */ 
/*==================================================*/
int efm_parse_msdh_getinfo(efm_agent_info * aefm,uint8_t *  buf,uint16_t msdh_length)
{

	int i = 0;
	int ret= 0;

	if(NULL == buf || NULL == aefm)
	{
		zlog_err("%s[%d] buf is NULL",__FUNCTION__,__LINE__);
		return -1;
	}
	aefm->efm_agent_remote_device_sid = buf[i++];
	aefm->efm_agent_remote_device_tid = buf[i++];


	msdh_length -=i;                                //cancel  device id and type two bits

	                
	if(NULL == aefm->aefm_RstInfo)
	{

		zlog_err("%s[%d]aefm get  space  fail!",__FUNCTION__,__LINE__);
		return -1;

	}				
	
	ret =   efm_parse_remote_getinfo(aefm->aefm_RstInfo,buf+i,msdh_length);
	if(ret < 0)
	{
		zlog_err("%s[%d]aefm parse status info fail",__FUNCTION__,__LINE__);
		return ret;
	}
	return ret;

}
/*==================================================*/
/*name : efm_parse_remote_setinfo                                                                         */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote config set reply info                                                                       */ 
/*==================================================*/
int efm_parse_remote_setinfo( efm_agent_remote_set_info * aefm_RsetInfo,uint8_t * buf,uint8_t msdh_length)
{
	int i = 0;
	int j = 0;
	uint8_t addr_num = 0;
	uint8_t addr_start = 0;
	uint8_t addr_len = 0;

	aefm_RsetInfo->efm_remote_set_reply_words = buf[i++];		
	addr_num = buf[i++];

	if(addr_num != aefm_RsetInfo->efm_remote_set_start_addr_num)
	{
		EFM_LOG_DBG("%s[%d]##############reply info is error:   addr_num = %d",\
				__FUNCTION__,__LINE__,addr_num);
		return EFM_FAILURE;
	}

	for(j = 0;j <aefm_RsetInfo->efm_remote_set_start_addr_num;j++ )
	{
		addr_start = buf[i++];
		addr_len = buf[i++];

		if(addr_start > EFM_AGENT_STATE_DATA_MAX || addr_len > EFM_AGENT_STATE_DATA_MAX )
		{
			zlog_err("%s[%d] get remote agent info error ! data is over length 0x77",__FUNCTION__,__LINE__);
			return -1;
		}

	}
	if(i > msdh_length)
		return -1;

	return i;
}
/*==================================================*/
/*name : efm_parse_msdh_setinfo                                                                         */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote config set reply  info                                                                       */ 
/*==================================================*/

int efm_parse_msdh_setinfo(efm_agent_info * aefm,uint8_t *  buf,uint16_t msdh_length)
{

	int i = 0;
	int ret= 0;

	if(NULL == buf || NULL == aefm)
	{
		zlog_err("%s[%d] buf is NULL",__FUNCTION__,__LINE__);
		return -1;
	}
	
	aefm->efm_agent_remote_device_sid = buf[i++];
	aefm->efm_agent_remote_device_tid = buf[i++];


	msdh_length -=i;                                //cancel  device id and type two bits

	if(NULL == aefm->aefm_RsetInfo)
	{

		zlog_err("%s[%d]aefm set space is NULL!",__FUNCTION__,__LINE__);
		return -1;

	}

	ret =   efm_parse_remote_setinfo(aefm->aefm_RsetInfo,buf+i,msdh_length);
	if(ret < 0)
	{
		zlog_err("%s[%d]aefm parse set reply info fail",__FUNCTION__,__LINE__);
		return ret;
	}
	
	return ret;

}
/*==================================================*/
/*name : efm_parse_remote_SetGetinfo                                                                         */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote config info                                                                       */ 
/*==================================================*/

int efm_parse_remote_SetGetinfo( struct efm_if * pefm,uint8_t * buf,uint8_t msdh_length)
{
	int i = 0;
	int j = 0;
	int m = 0;
	uint8_t addr_start = 0;
	uint8_t addr_len = 0;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;
	int num = 0;
	 aefm_RsetGetInfo = pefm->aefm->aefm_RsetGetInfo;
	 	
	memset(aefm_RsetGetInfo,0,sizeof( efm_agent_remote_SetGet_info ));

	aefm_RsetGetInfo->efm_remote_reply_words = buf[i++];		
	aefm_RsetGetInfo->efm_remote_start_addr_num = buf[i++];

	for(j = 0;j <aefm_RsetGetInfo->efm_remote_start_addr_num;j++ )
	{
		aefm_RsetGetInfo->efm_remote_start_addr = buf[i++];
		aefm_RsetGetInfo->efm_remote_addr_len = buf[i++];
		addr_start = aefm_RsetGetInfo->efm_remote_start_addr;
		addr_len = aefm_RsetGetInfo->efm_remote_addr_len;

		if(addr_start > EFM_AGENT_SET_DATA_MAX || addr_len > EFM_AGENT_SET_DATA_MAX )
		{
			zlog_err("%s[%d] get remote agent info error ! data is over length 0x77",__FUNCTION__,__LINE__);
			return -1;
		}
		
		for(m = 0;m<addr_len;m++)
		{
			
			aefm_RsetGetInfo->efm_agent_remote_SetGet_data[addr_start+m] = buf[i++];
			EFM_LOG_DBG("############## member = %d\n",aefm_RsetGetInfo->efm_agent_remote_SetGet_data[addr_start+m] );
		
		}
	}

	pefm->efm_agent_remote_DeviceSid = pefm->aefm->efm_agent_remote_device_sid ;
	pefm->efm_agent_remote_DeviceTid =  pefm->aefm->efm_agent_remote_device_tid;
	
	memcpy(pefm->efm_agent_remote_config_data,aefm_RsetGetInfo->efm_agent_remote_SetGet_data,\
					 EFM_AGENT_RESUME_SET_MAX );
	/*debug config info*/
	while(num<  EFM_AGENT_RESUME_SET_MAX )
	{
		EFM_LOG_DBG("%s[%d]######%02x = %d\n",__FUNCTION__,__LINE__,\
			num,pefm->efm_agent_remote_config_data[num]);
		num++;
	}

	if(i > msdh_length)
		return -1;

	return i;

}
/*==================================================*/
/*name : efm_parse_msdh_SetGetinfo                                                                         */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm parse remote config info                                                                       */ 
/*==================================================*/

int efm_parse_msdh_SetGetinfo(struct efm_if * pefm,uint8_t * buf,uint16_t msdh_length)
{
	int i = 0;
	int ret = 0;
	efm_agent_info * aefm = NULL;

	if(NULL == buf)
	{
		zlog_err("%s[%d] buf is NULL",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm = pefm->aefm;
	if(NULL == aefm)
	{
		zlog_err("%s[%d] aefm is NULL",__FUNCTION__,__LINE__);
		return -1;
	}
	
	aefm->efm_agent_remote_device_sid = buf[i++];
	aefm->efm_agent_remote_device_tid = buf[i++];

	msdh_length -=i;                                //cancel  device id and type two bits


	if(NULL == aefm->aefm_RsetGetInfo)
	{					

		zlog_err("%s[%d]aefm setget info space  is NULL!",__FUNCTION__,__LINE__);
		return -1;					
	}

	ret =   efm_parse_remote_SetGetinfo(pefm,buf+i,msdh_length);
	if(ret < 0)
	{
		zlog_err("%s[%d]aefm parse set info fail",__FUNCTION__,__LINE__);
		return ret;
	}
	
	return ret;

}
/*==================================================*/
/*name : efm_agent_pdu_package                                                                         */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm agent subpackage depend on different types                                          */ 
/*==================================================*/

int efm_agent_pdu_package(struct efm_if  * pefm,uint8_t * buf)
{
	int i = 0;
	int j = 0;
	int m = 0;
	efm_agent_info * aefm = NULL;
	
	aefm = pefm->aefm;
	if(NULL == aefm)
	{
		return -1;
	}
	
	memcpy(buf,oui_data,EFM_MSDH_OUI_DATA_LEN);
	i += EFM_MSDH_OUI_DATA_LEN;
	buf[i++] = EFM_MSDH_FRM_HEAD;
	buf[i++] = 0x00;
	for(j=0;j<4;j++)
	{
		buf[i++] = msdh_DstAdrr[j];
	}
	for(j=0;j<4;j++)
	{
		buf[i++] = msdh_SrcAdrr[j];
	}
	buf[i++] = EFM_MSDH_VER;
	buf[i++] = 0x01;                                           //tiaoshu
	buf[i++] = (uint8_t)(EFM_MSDH_CMD_ID>>8);
	buf[i++] = (uint8_t)(EFM_MSDH_CMD_ID);
	buf[i++] = (uint8_t)(aefm->efm_agent_Msdhfrmnum>>8);
	buf[i++] = (uint8_t)(aefm->efm_agent_Msdhfrmnum);
	buf[i++] = (uint8_t)((sizeof(oui_data)/sizeof(uint8_t))>>8);
	buf[i++] = (uint8_t)(sizeof(oui_data)/sizeof(uint8_t));
	/*communication protocal H0FL_P 2014117*/
	buf[i++] = 0x69;
	buf[i++] = 0x11;
	buf[i++] = 0x01;
	/*basic information get PDU*/
	if(EFM_AGENT_DEVICE_INFO_PDU == aefm->efm_agent_pdu_type)
	{
		buf[i++] = (uint8_t)(EFM_MSDH_DEVICE_GET_CMD>>8);
		buf[i++] = (uint8_t)EFM_MSDH_DEVICE_GET_CMD;
		aefm->efm_tx_DifSpecific_pdu[EFM_AGENT_DEVICE_INFO_PDU]++;
	}
	/*status infomation get PDU*/
	else if(EFM_AGENT_GET_INFO_PDU == aefm->efm_agent_pdu_type)
	{
		buf[i++] = (uint8_t)(EFM_MSDH_INFO_GET_CMD>>8);
		buf[i++]= (uint8_t)EFM_MSDH_INFO_GET_CMD;
		buf[i++] = aefm->efm_agent_remote_device_sid;
		buf[i++] = aefm->efm_agent_remote_device_tid;
		buf[i++] = 1;
		buf[i++] = 0;
		buf[i++] = EFM_AGENT_STATE_DATA_MAX ;
		aefm->efm_tx_DifSpecific_pdu[EFM_AGENT_GET_INFO_PDU]++;
	}
	/*config infomation set PDU*/
	else if(EFM_AGENT_SET_INFO_PDU == aefm->efm_agent_pdu_type)
	{	


		if(NULL == aefm->aefm_RsetInfo)
		{
			zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
			return -1;

		}
		

		buf[i++] = (uint8_t)(EFM_MSDH_INFO_SET_CMD>>8);
		buf[i++]= (uint8_t)EFM_MSDH_INFO_SET_CMD;
		buf[i++]= aefm->efm_agent_remote_device_sid;
		buf[i++] = aefm->efm_agent_remote_device_tid;

		buf[i++] = aefm->aefm_RsetInfo->efm_remote_set_start_addr_num;
		buf[i++] = aefm->aefm_RsetInfo->efm_remote_set_start_addr;
		buf[i++] = aefm->aefm_RsetInfo->efm_remote_set_addr_len;

		for(m = 0;m <  aefm->aefm_RsetInfo->efm_remote_set_addr_len;m++)
		{
			buf[i++] = aefm->aefm_RsetInfo->efm_remote_set_value[aefm->aefm_RsetInfo->efm_remote_set_start_addr+m];
			EFM_LOG_DBG("%s[%d]############ set_addr = %d  set_value = %d\n",__FUNCTION__,__LINE__,\
					aefm->aefm_RsetInfo->efm_remote_set_start_addr+m,\
					aefm->aefm_RsetInfo->efm_remote_set_value[aefm->aefm_RsetInfo->efm_remote_set_start_addr+m]);
			/*update local config data*/
			aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data[aefm->aefm_RsetInfo->efm_remote_set_start_addr+m] =\
																aefm->aefm_RsetInfo->efm_remote_set_value[aefm->aefm_RsetInfo->efm_remote_set_start_addr+m];
			/*updata efm config data*/
			if(0<aefm->aefm_RsetInfo->efm_remote_set_start_addr+m &&\
				aefm->aefm_RsetInfo->efm_remote_set_start_addr+m < EFM_AGENT_RESUME_SET_MAX)
			{
				pefm->efm_agent_remote_config_data[aefm->aefm_RsetInfo->efm_remote_set_start_addr+m] =\
					aefm->aefm_RsetInfo->efm_remote_set_value[aefm->aefm_RsetInfo->efm_remote_set_start_addr+m];
			}
		}

		aefm->efm_tx_DifSpecific_pdu[EFM_AGENT_SET_INFO_PDU]++;
	}
	/*config information get PDU*/
	else if(EFM_AGENT_SET_GET_INFO_PDU == aefm->efm_agent_pdu_type)
	{
		buf[i++] = (uint8_t)(EFM_MSDH_INFO_SET_GET_CMD>>8);
		buf[i++]= (uint8_t)EFM_MSDH_INFO_SET_GET_CMD;
		buf[i++] = aefm->efm_agent_remote_device_sid;
		buf[i++] = aefm->efm_agent_remote_device_tid;
		buf[i++] = 1;
		buf[i++] = 0;
		buf[i++] = EFM_AGENT_SET_DATA_MAX ;
		aefm->efm_tx_DifSpecific_pdu[EFM_AGENT_SET_GET_INFO_PDU]++;
	}
	else if(EFM_AGENT_CONFIG_ALL_DATA_PDU == aefm->efm_agent_pdu_type)
	{
		buf[i++] = (uint8_t)(EFM_MSDH_INFO_SET_CMD>>8);
		buf[i++]= (uint8_t)EFM_MSDH_INFO_SET_CMD;
		buf[i++]= aefm->efm_agent_remote_device_sid;
		buf[i++] = aefm->efm_agent_remote_device_tid;
		buf[i++] = 1;
		buf[i++] = 1;  /*Dataresume not need to config*/
		buf[i++] =  EFM_AGENT_RESUME_SET_MAX-1 ;
		
		for(m = 1;m <   EFM_AGENT_RESUME_SET_MAX;m++)
		{
			buf[i++] = aefm->aefm_RsetInfo->efm_remote_set_value[m];
			EFM_LOG_DBG("%s[%d]############ set_addr = %d  set_value = %d\n",__FUNCTION__,__LINE__,\
					m,aefm->aefm_RsetInfo->efm_remote_set_value[m]);
			/*update local config data*/
			aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data[m] = aefm->aefm_RsetInfo->efm_remote_set_value[m];
		}
		aefm->efm_tx_DifSpecific_pdu[EFM_AGENT_SET_INFO_PDU]++;
	}
	return i;

}

/*==================================================*/
/*name :efm_format_organization_specific_pdu                                                        */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                      */
/*desc :  efm agent subpackage head--efm send specific OAMPDU code = 0xfe           */                                                              
/*==================================================*/
int efm_format_organization_specific_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
	int i = 0;
	efm_agent_info * aefm = NULL;

	if(NULL == pefm || NULL == bufptr || NULL == pefm->aefm)
	{
		return -1;
	}
	aefm = pefm->aefm;
	EFM_LOG_DBG("%s[%d] EFM[TX]: efm send format organization specific PDU\n",__FUNCTION__,__LINE__);
	bufptr[i++] = EFM_SUBTYPE;
	bufptr[i++]  = (uint8_t)(pefm->efm_flags >> 8);
	bufptr[i++] = (uint8_t)(pefm->efm_flags & 0xff);
	bufptr[i++]= EFM_ORG_SPECIFIC_INFO ;


	i += efm_agent_pdu_package(pefm,bufptr+i);    
	bufptr[i] = 0;

	aefm->efm_agent_Msdhfrmnum++;

	pefm->tx_count [EFM_SPECIFIC_PDU] += 1;
	return i;
}

/*==================================================*/
/*name :efm_agent_disable                                                                                   */
/*para :                                                                                                                */
/*retu : NULL                                                                                                       */
/*desc :  efm agent disable                                                                                    */ 
/*==================================================*/

void efm_agent_disable(struct efm_if * pefm)
{
	efm_agent_info * aefm = NULL;

	pefm->tx_count[EFM_SPECIFIC_PDU] = 0;
	pefm->rx_count[EFM_SPECIFIC_PDU] = 0;
	
	aefm = pefm->aefm;
	
	EFM_TIMER_OFF(aefm->efm_agent_pdu_timer);

	/*cancel trap*/
	//efm_agent_trap(0,hpmcoltalm,pefm->if_index);
	//efm_agent_trap(0,hpmcethalm,pefm->if_index);
	efm_trap_event_to_snmpd(0,hpmcethalm,pefm->if_index);
	efm_trap_event_to_snmpd(0,hpmcoltalm,pefm->if_index);

	/*send cancel info to ospf*/
	efm_agent_u0_info_send(pefm->if_index,aefm,IPC_OPCODE_DELETE);
	
	if(aefm->aefm_RbaseInfo != NULL)
	{
		XFREE (MTYPE_EFM,aefm->aefm_RbaseInfo);
		aefm->aefm_RbaseInfo= NULL;
	}
	if(aefm->aefm_RstInfo != NULL)
	{
		XFREE (MTYPE_EFM, aefm->aefm_RstInfo);
		aefm->aefm_RstInfo= NULL;

	}
	if(aefm->aefm_RsetGetInfo != NULL)
	{
		XFREE (MTYPE_EFM, aefm->aefm_RsetGetInfo);
		aefm->aefm_RsetGetInfo= NULL;

	}
	if(aefm->aefm_RsetInfo != NULL)
	{
		XFREE (MTYPE_EFM, aefm->aefm_RsetInfo);
		aefm->aefm_RsetInfo= NULL;

	}
	
	return ;

}

/*==================================================*/
/*name :efm_agent_disable                                                                                   */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :  efm agent send sepcific PDU                                                                      */ 
/*==================================================*/
int efm_agent_pdu_tx (struct efm_if *pefm)
{
	int ret = 0, len = 0;
	uint8_t *bufptr;
	register uint8_t *buf;

	if(NULL == pefm)
	{
		return -1;
	}

	EFM_LOG_DBG("EFM[TX]: Transmitting EFM PDU");

	bufptr = XCALLOC (MTYPE_EFM_MSG_BUF, EFM_BUF_SIZE);
	if(NULL == bufptr)
	{
		zlog_err ("Could not allocate memory for EFM MSG BUF\n");
		return -1;
	}

	/*initialise allocated memory*/  
	memset(bufptr, 0, EFM_BUF_SIZE);

	buf = bufptr;


	if ((len = efm_format_organization_specific_pdu (pefm, buf)) < 0)
	{
		XFREE (MTYPE_EFM_MSG_BUF, bufptr);
		return -1;
	}
       /*all get and set information need to depend on efm send any status*/
	if ( pefm->local_pdu != EFM_ANY)
	{
		XFREE (MTYPE_EFM_MSG_BUF, bufptr);
		return -1;
	}

	/* Send it. */
	ret = efm_send (pefm, bufptr, len<64 ? 64 : len);
	if(ret)
	{
		zlog_err("%s[%d]###########efm not is send any status",__FUNCTION__,__LINE__);
		XFREE (MTYPE_EFM_MSG_BUF, bufptr);  
		return -1;
	}
	XFREE (MTYPE_EFM_MSG_BUF, bufptr);  

	pefm->valid_pdu_req = EFM_FALSE;

	return ret;
}

#if 0 /*modify by youcheng 2018/8/26 due to timer change*/
/*==================================================*/
/*name :efm_agent_pdu_timer_expiry                                                                      */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :  efm agent send specific PDU every 3s                                                         */ 
/*==================================================*/
int efm_agent_pdu_timer_expiry (struct thread *thread)
{
	int i = 0;
	struct efm_if *pefm = NULL;
	efm_agent_info * aefm = NULL;
	//efm_agent_remote_state_info * aefm_RstInfo = NULL;

	if(NULL == thread || NULL == thread->arg)
	{
		return -1;
	}

	pefm = (struct efm_if *)THREAD_ARG (thread);


	if(NULL == pefm || NULL == pefm->aefm )
	{
		return -1;
	}

	aefm = pefm->aefm;
	//aefm_RstInfo = aefm->aefm_RstInfo;
	
	if(EFM_FALSE == aefm->efm_agent_link_flag && aefm->efm_agent_last_link_flag)
	{	
	/*need to check remote device has DyingGasp or LinkFault,clear orignal data */
		clear_efm_agent_data(aefm);
	}
	
	EFM_TIMER_OFF(aefm->efm_agent_pdu_timer);
	
	aefm->efm_agent_pdu_type  = EFM_AGENT_DEVICE_INFO_PDU;
	efm_agent_pdu_tx(pefm);

	zlog_debug ("EFM[EVENT]: EFM Sepcific PDU timer expired\n");
	
	/* only buid intial connection with remote*/
	if(aefm->efm_agent_link_flag)
	{
		
	   /*the first to connect remote device*/
	    if(!aefm->efm_agent_last_link_flag)
	    {
			/*if new discovery device is equal with last device,need to set all config for new device*/
			if(pefm->efm_agent_remote_DeviceSid ==  aefm->efm_agent_remote_device_sid && \
				pefm->efm_agent_remote_DeviceTid == aefm->efm_agent_remote_device_tid &&\
				pefm->efm_agent_remote_DeviceSid  && pefm->efm_agent_remote_DeviceTid)	
			{
				
				memcpy(aefm->aefm_RsetInfo->efm_remote_set_value,pefm->efm_agent_remote_config_data,\
					EFM_AGENT_RESUME_SET_MAX);
				/*debug memcpy info*/
				for(i = 0;i<EFM_AGENT_RESUME_SET_MAX;i++)
				{
					zlog_debug("%02x--%d\n",i,aefm->aefm_RsetInfo->efm_remote_set_value[i]);
				}
				/*send local config data to remote device*/
				aefm->efm_agent_pdu_type  =  EFM_AGENT_CONFIG_ALL_DATA_PDU;
				efm_agent_pdu_tx(pefm);	
			}
			
	    }
		/*if remote is 11000A/11000An/1101n device then get and set data*/
		aefm->efm_agent_pdu_type = EFM_AGENT_GET_INFO_PDU;	
	    	 efm_agent_pdu_tx(pefm);

	      	aefm->efm_agent_pdu_type = EFM_AGENT_SET_GET_INFO_PDU;
	     	 efm_agent_pdu_tx (pefm);

		/*has already bulid fully connection with remote*/
		//aefm->efm_agent_last_link_flag = EFM_TRUE;
	}
	EFM_TIMER_ON (aefm->efm_agent_pdu_timer, efm_agent_pdu_timer_expiry,
			pefm, aefm->efm_agent_pdu_time );

	return EFM_SUCCESS;
}
#endif

/*==================================================*/
/*name :efm_agent_pdu_timer_expiry                                                                      */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :  efm agent send specific PDU every 3s                                                         */ 
/*==================================================*/
int efm_agent_pdu_timer_expiry (void *arg)
{
	int i = 0;
	struct efm_if *pefm = NULL;
	efm_agent_info * aefm = NULL;
	//efm_agent_remote_state_info * aefm_RstInfo = NULL;

	if(NULL == arg)
	{
		return -1;
	}

	pefm = (struct efm_if *)arg;


	if(NULL == pefm || NULL == pefm->aefm )
	{
		return -1;
	}

	aefm = pefm->aefm;
	//aefm_RstInfo = aefm->aefm_RstInfo;
	
	if(EFM_FALSE == aefm->efm_agent_link_flag && aefm->efm_agent_last_link_flag)
	{	
	/*need to check remote device has DyingGasp or LinkFault,clear orignal data */
		clear_efm_agent_data(aefm);
	}
	

	
	aefm->efm_agent_pdu_type  = EFM_AGENT_DEVICE_INFO_PDU;
	efm_agent_pdu_tx(pefm);

	EFM_LOG_DBG ("EFM[EVENT]: EFM Sepcific PDU timer expired\n");
	
	/* only buid intial connection with remote*/
	if(aefm->efm_agent_link_flag)
	{
		
	   /*the first to connect remote device*/
	    if(!aefm->efm_agent_last_link_flag)
	    {
			/*if new discovery device is equal with last device,need to set all config for new device*/
			if(pefm->efm_agent_remote_DeviceSid ==  aefm->efm_agent_remote_device_sid && \
				pefm->efm_agent_remote_DeviceTid == aefm->efm_agent_remote_device_tid &&\
				pefm->efm_agent_remote_DeviceSid  && pefm->efm_agent_remote_DeviceTid)	
			{
				
				memcpy(aefm->aefm_RsetInfo->efm_remote_set_value,pefm->efm_agent_remote_config_data,\
					EFM_AGENT_RESUME_SET_MAX);
				/*debug memcpy info*/
				for(i = 0;i<EFM_AGENT_RESUME_SET_MAX;i++)
				{
					EFM_LOG_DBG("%02x--%d\n",i,aefm->aefm_RsetInfo->efm_remote_set_value[i]);
				}
				/*send local config data to remote device*/
				aefm->efm_agent_pdu_type  =  EFM_AGENT_CONFIG_ALL_DATA_PDU;
				efm_agent_pdu_tx(pefm);	
			}
			
	    }
		/*if remote is 11000A/11000An/1101n device then get and set data*/
		aefm->efm_agent_pdu_type = EFM_AGENT_GET_INFO_PDU;	
	    	 efm_agent_pdu_tx(pefm);

	      	aefm->efm_agent_pdu_type = EFM_AGENT_SET_GET_INFO_PDU;
	     	 efm_agent_pdu_tx (pefm);

		/*has already bulid fully connection with remote*/
		//aefm->efm_agent_last_link_flag = EFM_TRUE;
#if 0
	EFM_TIMER_ON (aefm->efm_agent_pdu_timer, efm_agent_pdu_timer_expiry,
			pefm, aefm->efm_agent_pdu_time );
#endif
	}
	return EFM_SUCCESS;
}


/*==================================================*/
/*name :efm_agent_trap_ThreeAlarm                                                                     */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :  efm agent trap 3 event                                                                             */ 
/*==================================================*/

int efm_agent_trap_ThreeAlarm(efm_agent_info * aefm,uint16_t event_type,uint8_t trap_act,uint32_t ifindex)
{
	
	efm_agent_remote_state_info * aefm_RstInfo = NULL;

	EFM_LOG_DBG("%s[%d]########## event_type = %d\n",__FUNCTION__,__LINE__,event_type);

	/*event occur but remote device is not H0FL-S and no trap */
	if(trap_act)
	{
		if( NULL == aefm || EFM_FALSE == aefm->efm_agent_link_flag)
		{		
			zlog_err("%s[%d]##########remote is not H0FL-P device(11000A 11000An 1101n)",__FUNCTION__,__LINE__);
			return -1;
		}

		aefm_RstInfo = aefm->aefm_RstInfo;

		switch(event_type)
		{
			case EFM_LINK_FAULT_EVENT:
				/*report or cancel event (olt lose) */
				aefm_RstInfo->LinkFault_flag = EFM_TRUE;
				//efm_agent_trap(1,hpmcoltalm,ifindex);
				efm_trap_event_to_snmpd(1,hpmcoltalm,ifindex);
				aefm->efm_agent_link_flag = EFM_FALSE;
				EFM_LOG_DBG("%s[%d]###################EFM_LINK_FAULT\n",__FUNCTION__,__LINE__);
				/*send cancel info to osfp*/
				efm_agent_u0_info_send(ifindex,aefm,IPC_OPCODE_DELETE);
				break;
			case EFM_CRITICAL_LINK_EVENT:
				/*report or cancel event (eth down) */
				//efm_agent_trap(1,hpmcethalm,ifindex);
				efm_trap_event_to_snmpd(1,hpmcethalm,ifindex);
				break;
			case EFM_DYING_GASP_EVENT:
				/*report or cancel event (dying gasp) */
				aefm_RstInfo->DyingGasp_flag  = EFM_TRUE;
				//efm_agent_trap(2,hpmcethalm,ifindex);
				efm_trap_event_to_snmpd(2,hpmcethalm,ifindex);
				aefm->efm_agent_link_flag = EFM_FALSE;
				EFM_LOG_DBG("%s[%d]###################EFM_DYING_GASP\n",__FUNCTION__,__LINE__);
				efm_agent_u0_info_send(ifindex,aefm,IPC_OPCODE_DELETE);
				break;
			default:
				return EFM_FAILURE;
		}
		


	}
	else
	{	
		switch(event_type)
		{
			case EFM_LINK_FAULT_EVENT:
				/*cancel event (olt lose) */
				//efm_agent_trap(0,hpmcoltalm,ifindex);
				efm_trap_event_to_snmpd(0,hpmcoltalm,ifindex);
				if(aefm != NULL && aefm->aefm_RstInfo != NULL)
				aefm->aefm_RstInfo->LinkFault_flag = EFM_FALSE;
				break;
			case EFM_CRITICAL_LINK_EVENT:
				/*report or cancel event (eth down) */
				//efm_agent_trap(0,hpmcethalm,ifindex);
				efm_trap_event_to_snmpd(0,hpmcethalm,ifindex);
				break;
			case EFM_DYING_GASP_EVENT:
				/*report or cancel event (dying gasp) */
				//efm_agent_trap(0,hpmcethalm,ifindex);
				efm_trap_event_to_snmpd(0,hpmcethalm,ifindex);
				if(aefm != NULL && aefm->aefm_RstInfo != NULL)
				aefm->aefm_RstInfo->DyingGasp_flag  = EFM_FALSE;
				break;
			default:
				return EFM_FAILURE;
		}

	}

	return EFM_SUCCESS;
}
/*==============================================================*/
/*name :efm_agent_trap_ThreeAlarm                                                                                                    */
/*para :                                                                                                                                              */
/*retu : NULL                                                                                                                                      */
/*desc :  port link fault or poweroff or agent remote is not 11000A/11000An/1101N need to clear data        */                             
/*===============================================================*/

void clear_efm_agent_data(efm_agent_info * aefm)
{
	int8_t  solution = -1;
	if(aefm->aefm_RstInfo->LinkFault_flag)
	{
		solution = hpmcoltalm;
	}
	if(aefm->aefm_RstInfo->DyingGasp_flag)
	{
		solution = hpmcethalm;
	}
		
	/*link has fault,so need to set last_link_flag 0*/
	aefm->efm_agent_last_link_flag = EFM_FALSE;

	memset(aefm->aefm_RbaseInfo,0,sizeof(efm_agent_remote_base_info));
	memset(aefm->aefm_RsetGetInfo,0,sizeof(efm_agent_remote_SetGet_info));
	memset(aefm->aefm_RstInfo,0,sizeof(efm_agent_remote_state_info));
	memset(aefm->aefm_RsetInfo,0,sizeof(efm_agent_remote_set_info));	
	switch(solution)
	{
		case hpmcoltalm:
			aefm->aefm_RstInfo->LinkFault_flag = EFM_TRUE;
			break;
		case hpmcethalm:
			aefm->aefm_RstInfo->DyingGasp_flag = EFM_TRUE;
			break;
		default:
			break;		
	}
	
	
	return ;
}
/*====================================================================*/
/*name :efm_agent_save_remote_config_info                                                                                                   */
/*para :                                                                                                                                                          */
/*retu : NULL                                                                                                                                                  */
/*desc : if connect remote device suddenly fail,  local agent device need to save remote last config data  */                             
/*====================================================================*/
void efm_agent_save_remote_config_info(struct efm_if *pefm)
{
	efm_agent_info * aefm = NULL;
	int num = 0;
	
	aefm = pefm->aefm;
	if(NULL == aefm)
	{
		return ;
	}
	aefm->efm_agent_link_flag = EFM_FALSE;
	
	/*when linkfault or remote device dyinggasp,need to reserve remote device info*/
	pefm->efm_agent_remote_DeviceSid =  aefm->efm_agent_remote_device_sid ;
	pefm->efm_agent_remote_DeviceTid =  aefm->efm_agent_remote_device_tid;
	
	memcpy(pefm->efm_agent_remote_config_data,aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data,\
					 EFM_AGENT_RESUME_SET_MAX );
	/*debug config info*/
	while(num<  EFM_AGENT_RESUME_SET_MAX )
	{
		EFM_LOG_DBG("%s[%d]######%02x = %d\n",__FUNCTION__,__LINE__,\
			num,pefm->efm_agent_remote_config_data[num]);
		num++;
	}
	return ;
}
/*new trap module writed by lipf*/
#if 0
/*==================================================*/
/*name : efm_agent_trap                                                                      */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                        */
/*desc :  efm agent report alarm status to snmp agent                                            */ 
/*==================================================*/
int efm_agent_trap(uint8_t data,enum efm_agent_trap_type trap_node, uint32_t ifindex)
{

	/* First : add data struct to send trap msg to snmp */
	struct snmp_trap_hdr trap_hdr;
	struct snmp_trap_data trap_data;
	memset(&trap_hdr, 0, sizeof(struct snmp_trap_hdr));
	memset(&trap_data, 0, sizeof(struct snmp_trap_data));

	uchar buf[IPC_MSG_LEN];
	memset(buf, 0, IPC_MSG_LEN);

	/* Second : add trap msg header */
	oid oid_alarm_trap_efm_agent[] = {1, 3, 6, 1, 4, 1, 9966, 2, 100, 1, 2, 12};

	trap_hdr.trap_oid = oid_alarm_trap_efm_agent;
	trap_hdr.len_trap_oid = OID_LENGTH(oid_alarm_trap_efm_agent);	
	if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
	{
		return -1;
	}	
	
	/* Third : add trap msg data */
	oid oid_alarm_trap_efm_agent_row[][EFM_AGENT_ALM_MIBNODE_LEN] = {	
		{1, 3, 6, 1, 4, 1, 9966, 2, 100, 1, 1, 11, 2, 2, 2, 1, 3,0,1},
		{1, 3, 6, 1, 4, 1, 9966, 2, 100, 1, 1, 11, 2, 2, 4, 1, 3,0,1}
	};

	oid_alarm_trap_efm_agent_row[trap_node][EFM_AGENT_ALM_MIBNODE_LEN - 2]	= ifindex;

	trap_data.type = SNMP_TYPE_OCTET_STR;

	oid_alarm_trap_efm_agent_row[trap_node][EFM_AGENT_ALM_MIBNODE_LEN - 2]	= ifindex;

	trap_data.type = SNMP_TYPE_OCTET_STR;

	trap_data.data_oid = oid_alarm_trap_efm_agent_row[trap_node];
	trap_data.len_data_oid = EFM_AGENT_ALM_MIBNODE_LEN;

	trap_data.data = &data;
	trap_data.len_data = sizeof(uint8_t);

	if(0 == snmp_add_trap_msg_data(buf, &trap_data))
	{
		return -1;
	}

	/* Last : send msg to snmp */
	snmp_send_trap_msg(buf, MODULE_ID_L2, 0, 0);
	
	return EFM_SUCCESS;
}

#endif

int efm_trap_event_to_snmpd(uint8_t trap_data, enum efm_agent_trap_type trap_node,uint32_t ifindex)
{
	uint8_t pdata[2] = {0};
	pdata[0] = trap_node;
	pdata[1] = trap_data;
	if(-1 == ipc_send_msg_n2(pdata, sizeof(pdata), sizeof(pdata),
					MODULE_ID_SNMPD_TRAP, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_EFMAGENT_TRAP, IPC_OPCODE_INVALID,ifindex))
	{
		zlog_err("%s[%d] : EFM agent trap  failed", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}
int efm_agent_write_data(efm_agent_info * aefm)
{
	
	FILE *fp = NULL;
	char str[EFM_AGENT_SET_DATA_MAX]   = {0};	
	
	fp = fopen(CONFIG_WRITE_TXT ,"w");
	if(NULL == fp)
	{
		zlog_err("[%s][%d]####efm agent fopen config write txt fail",__FUNCTION__,__LINE__);
		return -1;
	}
	memcpy(str,aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data,sizeof(str));
	

	fwrite(str,sizeof(str),1,fp);
	fclose(fp);
	
	return 0;
}
int efm_agent_u0_info_send(uint32_t ifindex,efm_agent_info *aefm,enum IPC_OPCODE opcode)
{
	uint8_t  u0_ip[4] = {0};
	struct u0_device_info u0_info;
	char * u0_mac = NULL;
	//uint8_t  * set_ip = NULL;
	uint32_t ip_cmp = 0;

	if(NULL == aefm || NULL == aefm->aefm_RsetGetInfo || NULL == aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]#########aefm NULL",__FUNCTION__,__LINE__);
		return -1;
	}
	
	u0_mac = (char *)&aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_6hpmcrtMacAddr];

	
	memset(&u0_info,0,sizeof(u0_info));
	if(IPC_OPCODE_ADD == opcode)
	{


		u0_ip[0] = 136;         /*126.mac[3],mac[4].mac[5]*/
			memcpy( &u0_ip[1],&u0_mac[3],3);
		/*debug config ip info*/
			EFM_LOG_DBG("%s[%d] ADD IP : %d,%d,%d,%d\n",\
			__FUNCTION__,__LINE__,u0_ip[0],u0_ip[1],u0_ip[2],u0_ip[3]);
			memcpy(aefm->efm_agent_default_ip,u0_ip,4);
		
		/*add update ip*/
		if(memcmp(aefm->efm_agent_update_ip,&ip_cmp,4) && memcmp(&aefm->efm_agent_update_id,&ip_cmp,4))
		{
			EFM_LOG_DBG("%s[%d]:ADD IP %d %d %d %d:\n",__FUNCTION__,__LINE__,\
				aefm->efm_agent_update_ip[0],aefm->efm_agent_update_ip[1],aefm->efm_agent_update_ip[2],aefm->efm_agent_update_ip[3]);
			
			memcpy(&u0_info.ne_ip,aefm->efm_agent_update_ip,4);
			memcpy(&u0_info.ne_id,&aefm->efm_agent_update_id,4);
		}
		else
		{
			memcpy(&u0_info.ne_ip,aefm->efm_agent_default_ip,4);
			/*update ip*/
			memcpy(aefm->efm_agent_update_ip,aefm->efm_agent_default_ip,4);
			/*update id*/
			//memcpy(&(aefm->efm_agent_update_id>>8),&u0_ip[1],3);
			aefm->efm_agent_update_id = u0_ip[1] << 16;
			aefm->efm_agent_update_id |= u0_ip[2] << 8;
			aefm->efm_agent_update_id |= u0_ip[3];
			if (aefm->efm_agent_update_id < 0x10001)
			{
				aefm->efm_agent_update_id += 0x10000;
			}
			else if(aefm->efm_agent_update_id > 0xFEFFFE)
			{
				aefm->efm_agent_update_id -= 0x10001;
			}
			memcpy(&u0_info.ne_id.s_addr,&aefm->efm_agent_update_id,4);
			
			
			
		}
		
		/*record new set ip*/
		memcpy(&aefm->aefm_RsetInfo->efm_remote_set_value[efm_remote_set_4IpAddr],aefm->efm_agent_update_ip,4);
		memcpy(u0_info.hostname,aefm->efm_agent_hostname,strlen(aefm->efm_agent_hostname));

	}
	else if(IPC_OPCODE_UPDATE == opcode)
	{
		/*debug config ip info*/
		EFM_LOG_DBG("%s[%d] UP IP : %d,%d,%d,%d\n",\
		__FUNCTION__,__LINE__,aefm->efm_agent_update_ip[0],aefm->efm_agent_update_ip[1],aefm->efm_agent_update_ip[2],aefm->efm_agent_update_ip[3]);
		
		memcpy(&u0_info.ne_ip,aefm->efm_agent_update_ip,4);	
		memcpy(&u0_info.ne_id,&aefm->efm_agent_update_id,4);
		memcpy(u0_info.hostname,aefm->efm_agent_hostname,strlen(aefm->efm_agent_hostname));
	}
	else if(IPC_OPCODE_DELETE == opcode)
	{
		EFM_LOG_DBG("%s[%d]:DEL IP %d %d %d %d:\n",__FUNCTION__,__LINE__,\
		aefm->efm_agent_update_ip[0],aefm->efm_agent_update_ip[1],aefm->efm_agent_update_ip[2],aefm->efm_agent_update_ip[3]);

		memcpy(&u0_info.ne_ip,aefm->efm_agent_update_ip,4);
		memcpy(&u0_info.ne_id,&aefm->efm_agent_update_id,4);
		memcpy(u0_info.hostname,"Hios",strlen("Hios"));
		u0_info.ifindex = ifindex;

		if(efm_agent_ipc_send_ospf(&u0_info,sizeof(u0_info),opcode)<0)
		{
			zlog_err("%s[%d]########### send ipc to ospf fail",__FUNCTION__,__LINE__);
			return -1;
		}
		return 0;
	}
	#if 0
	switch (aefm->efm_agent_remote_device_tid)
	{
		case EFM_REMOTE_11000A_TYPE:
			memcpy(u0_info.dcn_ne_device_type,"11000A",strlen("11000A"));
			break;		
		case EFM_REMOTE_1101n_TYPE:
			memcpy(u0_info.dcn_ne_device_type,"1101n",strlen("1101n"));
			break;
		case EFM_REMOTE_11000An_TYPE:
			memcpy(u0_info.dcn_ne_device_type,"11000An",strlen("11000An"));
			break;
			
		default:
			break;
	}	  
	#endif
	memcpy(u0_info.dcn_ne_device_type,"H20RN-161-S",strlen("H20RN-161-S"));
	memcpy(u0_info.dcn_ne_vendor,"Huahuan",strlen("Huahuan"));
	memcpy(u0_info.mac,u0_mac,MAC_LEN);

	u0_info.ifindex = ifindex;

	if(efm_agent_ipc_send_ospf(&u0_info,sizeof(u0_info),opcode)<0)
	{
		zlog_err("%s[%d]########### send ipc to ospf fail",__FUNCTION__,__LINE__);
		return -1;

	}
	return 0;
}

int efm_agent_ipc_send_ospf(void * pdata,int data_len,enum IPC_OPCODE opcode)
{

	int ret = 0;
	
	ret = ipc_send_msg_n2 (pdata, data_len, 1, MODULE_ID_OSPF, MODULE_ID_L2, IPC_TYPE_EFM, 0, opcode, 0);
	return ret;
}

void efm_agent_update_ip(struct u0_device_info * u0_device,uint16_t data_len)
{
	uint32_t        	 ifindex = 0;
	int 				cursor =0;
	
	struct 	hash_bucket	*pbucket = NULL;
	struct l2if      	*pif = NULL;
	efm_agent_info    	*aefm = NULL;

	if(!u0_device || data_len != sizeof(struct u0_device_info))
	{
		EFM_LOG_DBG("%s[%d]# no recv dcn config ip info\n",__FUNCTION__,__LINE__);
		return ;
	}

	ifindex = u0_device->ifindex;
	EFM_LOG_DBG("%s[%d]# ifindex = %x\n",__FUNCTION__,__LINE__,ifindex);


	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif && pif->pefm && pif->pefm->if_index == ifindex && pif->pefm->aefm)
		{
			aefm = pif->pefm->aefm;
			memcpy(aefm->efm_agent_update_ip,&u0_device->ne_ip,4);
			memcpy(&aefm->efm_agent_update_id,&u0_device->ne_id,4);
			
			EFM_LOG_DBG("%s[%d] UP IP : %d,%d,%d,%d\n",\
			__FUNCTION__,__LINE__,aefm->efm_agent_update_ip[0],aefm->efm_agent_update_ip[1],aefm->efm_agent_update_ip[2],aefm->efm_agent_update_ip[3]);
			break;
		}
	}

	return ;
}
void efm_agent_update_hostname(struct u0_device_info * u0_device,uint16_t data_len)
{

	int 				cursor =0;
	
	struct 	hash_bucket	*pbucket = NULL;
	struct l2if      	*pif = NULL;
	efm_agent_info    	*aefm = NULL;

	if (NULL == u0_device || 0 == data_len)
	{
		printf("%s[%d]# u0_device is NULL or data_len == 0.\n",__FUNCTION__,__LINE__);
		return ;
	}

	pif = l2if_lookup(u0_device->ifindex);
	if(pif && pif->pefm && pif->pefm->aefm )
	{
		//pif->pefm->aefm->efm_agent_hostname = data_len;
		printf("%s[%d]# u0_device name set successfully.\n",__FUNCTION__,__LINE__);
		if(strlen(u0_device->hostname) < sizeof(pif->pefm->aefm->efm_agent_hostname))
		{
			memset(pif->pefm->aefm->efm_agent_hostname, 0 ,sizeof(pif->pefm->aefm->efm_agent_hostname));
			memcpy(pif->pefm->aefm->efm_agent_hostname,u0_device->hostname,strlen(u0_device->hostname));
		}
	}

	return ;
}


void efm_agent_send_info_reply_dcn( )
{

	int 				cursor =0;
	
	struct 	hash_bucket	*pbucket = NULL;
	struct l2if      	*pif = NULL;
	efm_agent_info    	*aefm = NULL;



	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif && pif->pefm && pif->pefm->aefm)
		{
			aefm = pif->pefm->aefm;

			/*discovery remote device and build connect*/
			if(aefm->efm_agent_last_link_flag && aefm->efm_agent_link_flag)
			{
				if(efm_agent_u0_info_send(pif->pefm->if_index,aefm,IPC_OPCODE_ADD)<0)
				{
					EFM_LOG_DBG("%s[%d]# no recv dcn config ip info\n",__FUNCTION__,__LINE__);
					return ;
				}
			}
		}
	}

	return ;
}

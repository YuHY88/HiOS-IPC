/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*/
#include <string.h>
#include <lib/command.h>
#include <lib/vty.h>
#include <errno.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>
#include <ftm/pkt_udp.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <qos/qos_main.h>

#include "sla_session.h"
#include "lib/log.h"

uint8_t g_sla_flag;

static struct cmd_node sla_session_node =
{
  SLA_SESSION_NODE,
  "%s(config-sla-session)# ",
  1 /* VTYSH */
};

/* 从 SDK 获取初始速率参数 */
static int 
get_if_speed_from_sdk ( uint32_t ifindex ,struct ifm_port * pIfmPort)
{
	struct ipc_mesg_n * pmesg = NULL;

	if(ifindex == 0 || NULL == pIfmPort)
	{
		return -1;
	}
	
	#if 0
	return ipc_send_hal_wait_reply ( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS, IPC_TYPE_IFM, IFNET_INFO_SPEED, IPC_OPCODE_GET, ifindex );
	#endif
	
	pmesg = ipc_sync_send_n2( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_IFM, IFNET_INFO_SPEED, IPC_OPCODE_GET, ifindex, 0);
	if(pmesg)
	{
		memcpy(pIfmPort,pmesg->msg_data,sizeof(struct ifm_port));
		mem_share_free(pmesg, MODULE_ID_QOS);
		return 0;
	}

	return -1;
}

static uint32_t if_speed_get(uint32_t ifindex)
{
	int ret = -1;
	struct ifm_port if_port_info;
	uint32_t if_speed = 0;

	memset(&if_port_info, 0, sizeof(struct ifm_port));
	if((ret = get_if_speed_from_sdk(ifindex, &if_port_info)) != 0)
	{
		return 0;
	}

	switch(if_port_info.speed)
	{
		case IFNET_SPEED_GE:
			if_speed = 1000*1000;
			break;
		case IFNET_SPEED_FE:
			if_speed = 100*1000;
			break;
		case IFNET_SPEED_10GE:
			if_speed = 10000*1000;
			break;
		case IFNET_SPEED_10M:
			if_speed = 10*1000;
			break;
		default:
			break;
	}
	return if_speed;
}

/*格式转换*/
sint8 *sla_display_format(uint32_t decimal, uint8_t format, uint8_t precision)
{
	static sint8 buf[12];
	uint8_t len;
	uint8_t i;
	
	/*先转换为字符串*/
	sprintf(buf, "%u", decimal);
	len = strlen(buf);
	
	if (len < (format + 1))
	{
		switch (format)
		{
			case 1:
				sprintf(buf, "0.%01u", decimal);
				break;
			case 2:
				sprintf(buf, "0.%02u", decimal);
				break;
			case 3:
				sprintf(buf, "0.%03u", decimal);
				break;
			case 4:
				sprintf(buf, "0.%04u", decimal);
				break;
			case 5:
				sprintf(buf, "0.%05u", decimal);
				break;
			default:
				break;
		}

		/*精确度*/
		len = strlen(buf);
	    buf[len-format+precision] = '\0';
		return buf;
	}
	
	/*字符串移位*/
	for (i=0; i<(format+1); i++)
	{
		buf[len + 1 -i] = buf[len - i];
		
	}
	
	buf[len - format] = '.';

	/*精确度*/
	len = strlen(buf);
    buf[len-format+precision] = '\0';
		
	return buf;
}

static void sla_2544_result_show(struct vty *vty, struct sla_session *psess)
{
	struct sla_2544_measure *sla_result = NULL;
    uint32_t sla_state = 0;
	uint16_t data_size = 0;
	int id = 0;
	int num = 0;
	
	if(NULL == psess)
	{
		return ;
	}

	sla_state = psess->info.state;
    sla_result = &(psess->result.sla_2544_result);

	vty_out(vty, "------------------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, "%-15s%-15s%s%s","Session Id", "protocol", "State", VTY_NEWLINE);
	vty_out(vty, "%5d", psess->info.session_id);
    vty_out(vty, "%17s", "RFC2544");
    vty_out(vty, "%14s",sla_state == SLA_STATUS_ENABLE ? "Running":(sla_state == SLA_STATUS_FINISH ? 
					"Finish":(sla_state == SLA_STATUS_UNFINISH ? "Running":"Disable")));
    vty_out(vty, "%s", VTY_NEWLINE);
	vty_out(vty, "------------------------------------------------%s", VTY_NEWLINE);
	
	for(id = 0; id < 7; id++)
	{
		if(psess->info.sla_2544_size[id] == 0)
		{
			continue;
		}
		
		data_size = psess->info.sla_2544_size[id];
	
		if(data_size != 0)
		{
			num++;
			vty_out(vty, "%3s  %d%s", "Id", num, VTY_NEWLINE);
			vty_out(vty, "%4s%-25s: %d%s", "", "Size", data_size, VTY_NEWLINE);
			
			if(psess->info.throughput_enable == 1)
			{
				vty_out(vty, "%4s%-25s: %s%s", "", "Throughput(Mbps)",
					sla_display_format(sla_result->throughput[id], 3, 2), VTY_NEWLINE);
			
				vty_out(vty, "%4s%-25s: %lld/%lld%s", "", "TxCount/RxCount",
					psess->result.sla_2544_result.pkt_tx[id], psess->result.sla_2544_result.pkt_rx[id], VTY_NEWLINE);
			}
			if(psess->info.lm_enable == 1)
			{
				vty_out(vty, "%4s%-25s: %s%%/", "", "Max/min/mean Loss Ratio",
					sla_display_format(sla_result->lm[id], 2, 2));
				vty_out(vty, "%s%%/", sla_display_format(sla_result->lm[id], 2, 2));
				vty_out(vty, "%s%%%s", sla_display_format(sla_result->lm[id], 2, 2), VTY_NEWLINE);
			}
			if(psess->info.dm_enable == 1)
			{
				vty_out(vty, "%4s%-25s: %s/", "", "Max/min/mean Delay(us)",
					sla_display_format(sla_result->dm_max[id], 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->dm_min[id], 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->dm_mean[id], 3, 3), VTY_NEWLINE);
			}
			vty_out(vty, "%s", VTY_NEWLINE);
			data_size = 0;	
		}
	}

}

static void sla_1564_result_show(struct vty *vty, struct sla_session *psess)
{
	struct sla_1564_measure *sla_result = NULL;
    uint32_t sla_state = 0;
    int i = 0;
    
    if(NULL == psess)
    {
        return ;
    }

    sla_state = psess->info.state;
    sla_result = &(psess->result.sla_1564_result);
    
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %d%s", "", "Session Id", psess->info.session_id, VTY_NEWLINE);
    vty_out(vty, "%4s%-30s: %s%s", "", "Protocol",   "y1564", VTY_NEWLINE);
	
	vty_out(vty, "%4s%-30s: %s%s", "", "Test-Type",  psess->info.sla_y1564_stage  == SLA_Y1564_CONFIGURATION ?
            "Configuration Test" : (psess->info.sla_y1564_stage == SLA_Y1564_PERFORMANCE ? "Performance Test":"y1564 all"), VTY_NEWLINE);
    vty_out(vty, "%4s%-30s: %s%s", "", "Test-State", sla_state == SLA_STATUS_ENABLE ? 
            "Running":(sla_state == SLA_STATUS_FINISH ? "Finish":(sla_state == SLA_STATUS_UNFINISH ? 
                "Running":"Disable")), VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

	/*configuration test 配置测试进行的是阶跃测试*/
    if(psess->info.sla_y1564_stage != SLA_Y1564_PERFORMANCE)
    {
    	/* 步进cir测试 */
    	vty_out(vty, "%4s%-30s%s", "", "CIR step load test", VTY_NEWLINE);
		for(i = 0; i < 4; i++)
		{
			vty_out(vty, "%4s%-30s: %d%%CIR%s", "", "Bandwidth",
					100/4*(4 - i), VTY_NEWLINE);
			vty_out(vty, "%5s%-29s: %u%s", "", "IR(Kbps)",
					//sla_display_format(sla_result->c_cir_ir[i], 0, 0), VTY_NEWLINE);
			sla_result->c_cir_ir[i], VTY_NEWLINE);
			//if(psess->info.state == SLA_STATUS_FINISH
				//&& psess->result.sla_1564_result.cir_flag == SLA_STATUS_FINISH)
				if(psess->info.state != SLA_STATUS_DISABLE)
			{
				#if 0
				if(sla_result->c_cir_lm[i] <= psess->info.sac_loss
					&& sla_result->c_cir_dm_mean[i] <= psess->info.sac_delay
						&& sla_result->c_cir_jm_mean[i] <= psess->info.sac_jitter)
				{
					vty_out(vty, "%4s%-30s: %s%s", "", "Pass/Fail", "Pass", VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, "%4s%-30s: %s%s", "", "Pass/Fail", "Fail", VTY_NEWLINE);
				}
				#endif

				#if 1
				/* 参照华为形式 ,与rfc2544形式一致 */
				vty_out(vty, "%5s%-29s: %s%%%s", "", "The Frame Loss Ratio", 
						sla_display_format(sla_result->c_cir_lm[i], 2, 2), VTY_NEWLINE);
				
				vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Delay(us)",
					sla_display_format(sla_result->c_cir_dm_max[i], 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->c_cir_dm_min[i], 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->c_cir_dm_mean[i], 3, 3), VTY_NEWLINE);
				
				vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Jitter(us)",
					sla_display_format(sla_result->c_cir_jm_max[i], 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->c_cir_jm_min[i], 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->c_cir_jm_mean[i], 3, 3), VTY_NEWLINE);
				#endif
			}
			else
			{
				#if 0
				vty_out(vty, "%4s%-30s: %s%s", "", "Pass/Fail", "--", VTY_NEWLINE);
				#endif
				#if 1
				/* 参照华为形式 ,与rfc2544形式一致 */
				vty_out(vty, "%5s%-29s: %s%s", "", "The Frame Loss Ratio", "--", VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Delay(us)", "--", "--", "--", VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Jitter(us)", "--", "--", "--", VTY_NEWLINE);
				#endif
			}
			vty_out(vty, "%s", VTY_NEWLINE);
		}

	
		/* eir测试的色盲测试 , 不需要步进 */
		if(psess->info.eir != 0)
		{
	        vty_out(vty, "%4s%-30s%s", "", "EIR non-colour-aware", VTY_NEWLINE);
			
	        vty_out(vty, "%4s%-30s: CIR + EIR%s", "", "Bandwidth", VTY_NEWLINE);
	        vty_out(vty, "%5s%-29s: %u%s", "", "IR(Kbps)",
	                //sla_display_format(sla_result->c_eir_ir[7], 3, 2), VTY_NEWLINE);
			sla_result->c_eir_ir, VTY_NEWLINE);
	        //if(psess->info.state == SLA_STATUS_FINISH
				//&& psess->result.sla_1564_result.eir_flag == SLA_STATUS_FINISH)
			if(psess->info.state == SLA_STATUS_FINISH)
	        {
	        	#if 0
	        	if((sla_display_format(sla_result->throughput[7 - i], 3, 2) > (psess->info.cir * (1 - psess->info.sac_loss)))
					&& (sla_display_format(sla_result->throughput[7 - i], 3, 2) < (psess->info.cir + psess->info.eir)))
	        	{
	        		vty_out(vty, "%5s%-29s: %s%s", "", "Pass/Fail", "Pass", VTY_NEWLINE);
	        	}
				else
				{
					vty_out(vty, "%5s%-29s: %s%s", "", "Pass/Fail", "Fail", VTY_NEWLINE);
				}
				#endif

				#if 1
				/* 参照华为形式 ,与rfc2544形式一致 */
				vty_out(vty, "%5s%-29s: %s%%%s", "", "The Frame Loss Ratio", 
						sla_display_format(sla_result->c_eir_lm, 2, 2), VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Delay(us)",
					sla_display_format(sla_result->c_eir_dm_max, 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->c_eir_dm_min, 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->c_eir_dm_mean, 3, 3), VTY_NEWLINE);
				
				vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Jitter(us)",
					sla_display_format(sla_result->c_eir_jm_max, 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->c_eir_jm_min, 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->c_eir_jm_mean, 3, 3), VTY_NEWLINE);

				#endif
	        }
	        else
	        {
				#if 0
	        	vty_out(vty, "%5s%-29s: %s%s", "", "Pass/Fail", "--", VTY_NEWLINE);
				#endif

				#if 1
				/* 参照华为形式 ,与rfc2544形式一致 */
				vty_out(vty, "%5s%-29s: %s%s", "", "The Frame Loss Ratio", "--", VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Delay(us)", "--", "--", "--", VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Jitter(us)", "--", "--", "--", VTY_NEWLINE);
				#endif
	        }
	        vty_out(vty, "%s", VTY_NEWLINE);
		}

		
		/* 流量监管色盲测试 */
		if(psess->info.traf_policing_enable == SLA_MEASURE_ENABLE)
		{
			vty_out(vty, "%4s%-30s %s", "", "Traffic policing test", VTY_NEWLINE);
			if(psess->info.cir != 0 && ((psess->info.cir/5) > psess->info.eir))
			{
				vty_out(vty, "%4s%-30s: 125%%CIR + EIR%s", "", "Bandwidth", VTY_NEWLINE);
			}
			else
			{
	        	vty_out(vty, "%4s%-30s: CIR + 125%%EIR%s", "", "Bandwidth", VTY_NEWLINE);
			}
	        vty_out(vty, "%5s%-29s: %u%s", "", "IR(Kbps)",
	                //sla_display_format(sla_result->c_traf_ir, 3, 2), VTY_NEWLINE);
			sla_result->c_traf_ir, VTY_NEWLINE);
			
	        //if(psess->info.state == SLA_STATUS_FINISH
				//&& psess->result.sla_1564_result.traf_flag == SLA_STATUS_FINISH)
			if(psess->info.state == SLA_STATUS_FINISH)
	        {
	        	#if 0
	        	if((sla_display_format(sla_result->throughput[7 - i], 3, 2) > (psess->info.cir * (1 - psess->info.sac_loss)))
					&& (sla_display_format(sla_result->throughput[7 - i], 3, 2) < (psess->info.cir + psess->info.eir)))
	        	{
	        		vty_out(vty, "%5s%-29s: %s%s", "", "Pass/Fail", "Pass", VTY_NEWLINE);
	        	}
				else
				{
					vty_out(vty, "%5s%-29s: %s%s", "", "Pass/Fail", "Fail", VTY_NEWLINE);
				}
				#endif

				#if 1
				/* 参照华为形式 ,与rfc2544形式一致 */
				vty_out(vty, "%5s%-29s: %s%%%s", "", "The Frame Loss Ratio", 
						sla_display_format(sla_result->c_traf_lm, 2, 2), VTY_NEWLINE);
				
				vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Delay(us)",
					sla_display_format(sla_result->c_traf_dm_max, 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->c_traf_dm_min, 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->c_traf_dm_mean, 3, 3), VTY_NEWLINE);
				
				vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Jitter(us)",
					sla_display_format(sla_result->c_traf_jm_max, 3, 3));
				vty_out(vty, "%s/", sla_display_format(sla_result->c_traf_jm_min, 3, 3));
				vty_out(vty, "%s%s", sla_display_format(sla_result->c_traf_jm_mean, 3, 3), VTY_NEWLINE);
				#endif
	        }
	        else
	        {
	        	#if 0
	        	vty_out(vty, "%5s%-29s: %s%s", "", "Pass/Fail", "--", VTY_NEWLINE);
				#endif

				#if 1
				/* 参照华为形式 ,与rfc2544形式一致 */
				vty_out(vty, "%5s%-29s: %s%s", "", "The Frame Loss Ratio", "--", VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Delay(us)", "--", "--", "--", VTY_NEWLINE);
				vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Jitter(us)", "--", "--", "--", VTY_NEWLINE);
				#endif
	        }
	        vty_out(vty, "%s", VTY_NEWLINE);
		}
    }
    else if(psess->info.sla_y1564_stage != SLA_Y1564_CONFIGURATION)
    {
        /*information rate 实时信息速率*/
        vty_out(vty, "%5s%-29s: %u%s", "", "IR(Kbps)",
                //sla_display_format(sla_result->p_ir, 3, 2), VTY_NEWLINE);
		sla_result->p_ir, VTY_NEWLINE);
		#if 0
		if(psess->info.state == SLA_STATUS_FINISH)
		{

			if(sla_result->p_lm <= psess->info.sac_loss
				&& sla_result->p_dm_mean <= psess->info.sac_delay
					&& sla_result->p_jm_mean <= psess->info.sac_jitter)
			{
				vty_out(vty, "%4s%-30s: %s%s", "", "Pass/Fail", "Pass", VTY_NEWLINE);
			}
			else
			{
				vty_out(vty, "%4s%-30s: %s%s", "", "Pass/Fail", "Fail", VTY_NEWLINE);
			}

		}
		else
		{
			vty_out(vty, "%4s%-30s: %s%s", "", "Pass/Fail", "--", VTY_NEWLINE);
		}
		#endif

		//if(psess->info.state == SLA_STATUS_FINISH
			//&& psess->result.sla_1564_result.perf_flag == SLA_STATUS_FINISH)
		if(psess->info.state == SLA_STATUS_FINISH )
		{
			#if 1
			/* 参照华为形式 ,与rfc2544形式一致 */
			vty_out(vty, "%5s%-29s: %s%%%s", "", "The Frame Loss Ratio", 
					sla_display_format(sla_result->p_lm, 2, 2), VTY_NEWLINE);
		
			vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Delay(us)",
				sla_display_format(sla_result->p_dm_max, 3, 3));
			vty_out(vty, "%s/", sla_display_format(sla_result->p_dm_min, 3, 3));
			vty_out(vty, "%s%s", sla_display_format(sla_result->p_dm_mean, 3, 3), VTY_NEWLINE);
			
			vty_out(vty, "%5s%-29s: %s/", "", "Max/min/mean Jitter(us)",
				sla_display_format(sla_result->p_jm_max, 3, 3));
			vty_out(vty, "%s/", sla_display_format(sla_result->p_jm_min, 3, 3));
			vty_out(vty, "%s%s", sla_display_format(sla_result->p_jm_mean, 3, 3), VTY_NEWLINE);
			#endif
		}
		else
		{
			#if 1
			/* 参照华为形式 ,与rfc2544形式一致 */
			vty_out(vty, "%5s%-29s: %s%s", "", "The Frame Loss Ratio", "--", VTY_NEWLINE);
			vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Delay(us)", "--", "--", "--", VTY_NEWLINE);
			vty_out(vty, "%5s%-29s: %s/%s/%s%s", "", "Max/min/mean Jitter(us)", "--", "--", "--", VTY_NEWLINE);
			#endif
		}

		
		#if 0
        /* loss measure 帧丢失*/
        if(psess->info.lm_enable == SLA_MEASURE_DISABLE)
        {
            vty_out(vty, "%4s%-30s: %s%s", "", "Loss Measure Status", "Disable", VTY_NEWLINE);    
        }
        else if(psess->info.lm_enable == SLA_MEASURE_ENABLE)
        {
            vty_out(vty, "%4s%-30s: %s%s", "", "Loss Measure Status", "Enable", VTY_NEWLINE);
        }
        if(psess->info.lm_enable == SLA_MEASURE_ENABLE &&
                psess->info.state == SLA_STATUS_FINISH)
        {
            vty_out(vty, "%5s%-29s: %s%%%s", "", "The Frame Loss Ratio",
                    sla_display_format(sla_result->p_lm, 2, 2), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%5s%-29s: %s%s", "", "The Frame Loss Ratio", "--", VTY_NEWLINE);
        }

        /* delay measure 帧延时*/
        if(psess->info.dm_enable == SLA_MEASURE_DISABLE)
        {
            vty_out(vty, "%4s%-30s: %s%s", "", "Delay Measure Status", "Disable", VTY_NEWLINE);    
        }
        else if(psess->info.dm_enable == SLA_MEASURE_ENABLE)
        {
            vty_out(vty, "%4s%-30s: %s%s", "", "Delay Measure Status", "Enable", VTY_NEWLINE);
        }
        if(psess->info.dm_enable == SLA_MEASURE_ENABLE &&
                psess->info.state == SLA_STATUS_FINISH)
        {
            vty_out(vty, "%5s%-29s: %s%s", "", "The Max Delay(us)",
                    sla_display_format(sla_result->p_dm_max, 3, 3),  VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Min Delay(us)",
                    sla_display_format(sla_result->p_dm_min, 3, 3),  VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Mean Delay(us)",
                    sla_display_format(sla_result->p_dm_mean, 3, 3), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%5s%-29s: %s%s", "", "The Max Delay(us)",  "--", VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Min Delay(us)",  "--", VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Mean Delay(us)", "--", VTY_NEWLINE);
        }

        /* delay jitter 时延抖动*/
        if(psess->info.dm_enable == SLA_MEASURE_DISABLE)
        {
            vty_out(vty, "%4s%-30s: %s%s", "", "Delay Jitter Measure Status", "Disable", VTY_NEWLINE);    
        }
        else if(psess->info.dm_enable == SLA_MEASURE_ENABLE)
        {
            vty_out(vty, "%4s%-30s: %s%s", "", "Delay Jitter Measure Status", "Enable", VTY_NEWLINE);
        }
        if(psess->info.dm_enable == SLA_MEASURE_ENABLE &&
                psess->info.state == SLA_STATUS_FINISH)
        {
            vty_out(vty, "%5s%-29s: %s%s", "", "The Max Delay Jitter(us)",
                    sla_display_format(sla_result->p_jm_max, 3, 3),  VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Min Delay Jitter(us)",
                    sla_display_format(sla_result->p_jm_min, 3, 3),  VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Mean Delay Jitter(us)",
                    sla_display_format(sla_result->p_jm_mean, 3, 3), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%5s%-29s: %s%s", "", "The Max Delay Jitter(us)",  "--", VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Min Delay Jitter(us)",  "--", VTY_NEWLINE);
            vty_out(vty, "%5s%-29s: %s%s", "", "The Mean Delay Jitter(us)", "--", VTY_NEWLINE);
        }
		#endif
    }
    
}

/*显示sla配置信息*/
static void sla_config_info_show(struct vty *vty, struct sla_session *psess)
{
    char sourceip[INET_ADDRSTRLEN] = "";
    char destip[INET_ADDRSTRLEN]   = "";
	//sint8 measure[50]              = "";
	int i = 0;
	
	if (psess == NULL)
	{
		return ;
	}

	vty_out(vty, "%-24s: %d%s", "Session-id", psess->info.session_id, VTY_NEWLINE);
	vty_out(vty, "%-24s: %s%s", "Protocol",psess->info.proto == SLA_PROTO_INVALID ? "Invalid":
		(psess->info.proto == SLA_PROTO_RFC2544 ? "RFC2544":"y.1564"), VTY_NEWLINE);
	
	/*rfc2544 配置*/
	if (psess->info.proto == SLA_PROTO_RFC2544)
	{
		vty_out(vty, "%-24s: ", "Size");
		for(i = 0; i < 7; i++)
		{
			if(psess->info.sla_2544_size[i] != 0)
			{
				vty_out(vty, "%d,", psess->info.sla_2544_size[i]);
			}
		}
		vty_out(vty, "%s", VTY_NEWLINE);
		vty_out(vty, "%-24s: %u%s", "Duration", psess->info.duration, VTY_NEWLINE);

		if(psess->info.throughput_enable == SLA_MEASURE_ENABLE)
		{
			vty_out(vty, "%-24s: %u%s", "Packet-rate upper", psess->info.pkt_rate_upper, VTY_NEWLINE);
			vty_out(vty, "%-24s: %u%s", "Packet-rate lower", psess->info.pkt_rate_lower, VTY_NEWLINE);
			vty_out(vty, "%-24s: %s%s", "Measure", "Throughput", VTY_NEWLINE);
		}
		else if(psess->info.dm_enable == SLA_MEASURE_ENABLE)
		{
			vty_out(vty, "%-24s: %u%s", "Packet-rate", psess->info.pkt_rate_upper, VTY_NEWLINE);
			vty_out(vty, "%-24s: %s%s", "Measure", "Delay", VTY_NEWLINE);
		}
		else if(psess->info.lm_enable == SLA_MEASURE_ENABLE)
		{
			vty_out(vty, "%-24s: %u%s", "Packet-rate", psess->info.pkt_rate_upper, VTY_NEWLINE);
			vty_out(vty, "%-24s: %s%s", "Measure", "Loss", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%-24s: %s%s", "Packet-rate", "--", VTY_NEWLINE);
			vty_out(vty, "%-24s: %s%s", "Measure", "--", VTY_NEWLINE);
		}
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	}
	/*Y1564 配置*/
	else if(psess->info.proto == SLA_PROTO_Y1564)
	{
		vty_out(vty, "%-24s: %u%s", "Size", psess->info.pkt_size, VTY_NEWLINE);
		vty_out(vty, "%-24s: %u%s", "Duration", psess->info.duration, VTY_NEWLINE);
		vty_out(vty, "%-24s: %u%s", "CIR", psess->info.cir, VTY_NEWLINE);
		vty_out(vty, "%-24s: %u%s", "EIR", psess->info.eir, VTY_NEWLINE);
		if(psess->info.sla_y1564_stage == SLA_Y1564_CONFIGURATION)
		{
			vty_out(vty, "%-24s: %s%s", "Configuration-test", "Enable", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%-24s: %s%s", "Configuration-test", "Disable", VTY_NEWLINE);
		}
		if(psess->info.sla_y1564_stage == SLA_Y1564_PERFORMANCE)
		{
			vty_out(vty, "%-24s: %s%s", "Performance-test", "Enable", VTY_NEWLINE);
		}
		else
		{	
			vty_out(vty, "%-24s: %s%s", "Performance-test", "Disable", VTY_NEWLINE);
		}	
	}
	else
	{
		return ;
	}
	
	if (psess->info.proto == SLA_PROTO_Y1564)
	{
		vty_out (vty, "SAC :%s", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %u%s%s", "", "Loss", psess->info.sac_loss * SLA_SAC_FLR_BENCHMARK/SLA_PKT_LOSS_ENLAGE, "/100000", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %-6u us%s", "", "Delay", psess->info.sac_delay/SLA_LATENCY_ENLAGE, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %-6u us%s", "", "Jitter", psess->info.sac_jitter/SLA_LATENCY_ENLAGE, VTY_NEWLINE);
		
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	}

	/*ethernet报文配置*/
	vty_out (vty, "Ethernet Packet :%s", VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %02X:%02X:%02X:%02X:%02X:%02X%s", "", "Dmac",psess->pkt_eth.dmac[0], psess->pkt_eth.dmac[1],psess->pkt_eth.dmac[2], \
	psess->pkt_eth.dmac[3],psess->pkt_eth.dmac[4],psess->pkt_eth.dmac[5],VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %02X:%02X:%02X:%02X:%02X:%02X%s", "", "Smac",psess->pkt_eth.smac[0], psess->pkt_eth.smac[1],psess->pkt_eth.smac[2], \
	psess->pkt_eth.smac[3],psess->pkt_eth.smac[4],psess->pkt_eth.smac[5],VTY_NEWLINE);

	if (psess->pkt_eth.svlan != 0 && psess->pkt_eth.cvlan != 0)
	{
		vty_out (vty, "%2s%-22s: %d%s", "", "Cvlan",psess->pkt_eth.cvlan, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: 0x%x%s", "", "C-tpid",psess->pkt_eth.c_tpid, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %d%s", "", "C-cos",psess->pkt_eth.c_cos, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %d%s", "", "Svlan",psess->pkt_eth.svlan, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: 0x%x%s", "", "S-tpid",psess->pkt_eth.s_tpid, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %d%s", "", "S-cos",psess->pkt_eth.s_cos, VTY_NEWLINE);
	}
	else if (psess->pkt_eth.svlan != 0 && psess->pkt_eth.cvlan == 0)
	{
		vty_out (vty, "%2s%-22s: %s%s", "", "Cvlan", "-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "C-tpid","-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "C-cos", "-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %d%s", "", "Svlan", psess->pkt_eth.svlan, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: 0x%x%s", "", "S-tpid",psess->pkt_eth.s_tpid, VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %d%s", "", "S-cos",psess->pkt_eth.s_cos, VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "%2s%-22s: %s%s", "", "Cvlan", "-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "C-tpid","-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "C-cos", "-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "Svlan", "-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "S-tpid","-", VTY_NEWLINE);
		vty_out (vty, "%2s%-22s: %s%s", "", "S-cos", "-", VTY_NEWLINE);
	}
	
	vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	
	/*ip报文配置*/
	vty_out (vty, "IP Packet :%s", VTY_NEWLINE);
        
	vty_out (vty, "%2s%-22s: %s%s", "", "Sip",   inet_ipv4tostr(psess->pkt_ip.sip, sourceip), VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %s%s", "", "Dip",   inet_ipv4tostr(psess->pkt_ip.dip, destip),   VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %d%s", "", "Sport", psess->pkt_ip.sport, VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %d%s", "", "Dport", psess->pkt_ip.dport, VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %d%s", "", "Dscp",  psess->pkt_ip.dscp,  VTY_NEWLINE);
	vty_out (vty, "%2s%-22s: %d%s", "", "Ttl",   psess->pkt_ip.ttl,   VTY_NEWLINE);
	vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	
	return ;
}

#if 0
/*显示sla配置信息*/
void sla_config_info_show(struct vty *vty, struct sla_session *psess)
{
    char sourceip[INET_ADDRSTRLEN] = "";
    char destip[INET_ADDRSTRLEN]   = "";
	sint8 measure[50]              = "";
	
	if (psess == NULL)
	{
		return ;
	}
	
	/*rfc2544 配置*/
	if (psess->info.proto == SLA_PROTO_RFC2544)
	{
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
		vty_out (vty, "%-5s %-8s %-5s %-11s %-11s %-8s %-5s %-17s%s","ID","Protocol","Size","Rate upper","Rate lower","Duration","Interval","Measure",VTY_NEWLINE);
		vty_out (vty, "%-5d ", psess->info.session_id);
		vty_out (vty, "%-8s ", "RFC2544");
		vty_out (vty, "%-5s ", "--");
		vty_out (vty, "%-11d ", psess->info.pkt_rate_upper);
		vty_out (vty, "%-11d ", psess->info.pkt_rate_lower);
		vty_out	(vty, "%-10d ", psess->info.r2544_dur_interval);
		vty_out (vty, "%-8d ", psess->info.interval);
		
		if (psess->info.throughput_enable == SLA_MEASURE_ENABLE)
		{
			strcpy (measure, "Throughput");
		}
		
		if (psess->info.throughput_enable == SLA_MEASURE_DISABLE && \
			psess->info.dm_enable == SLA_MEASURE_ENABLE)
		{
			strcpy (measure, "Delay");
		}
		else if (psess->info.throughput_enable == SLA_MEASURE_ENABLE && \
			psess->info.dm_enable == SLA_MEASURE_ENABLE)
		{
			strcat (measure, "/Delay");
		}
		
		if (psess->info.throughput_enable == SLA_MEASURE_DISABLE && \
			psess->info.dm_enable == SLA_MEASURE_DISABLE && \
			psess->info.lm_enable == SLA_MEASURE_ENABLE)
		{
			strcat (measure, "Loss");
		}
		else if ((psess->info.throughput_enable == SLA_MEASURE_ENABLE || \
			psess->info.dm_enable == SLA_MEASURE_ENABLE) && \
			psess->info.lm_enable == SLA_MEASURE_ENABLE)
		{
			strcat (measure, "/Loss");
		}
		
		if (psess->info.throughput_enable == SLA_MEASURE_DISABLE && \
			psess->info.dm_enable == SLA_MEASURE_DISABLE && \
			psess->info.lm_enable == SLA_MEASURE_DISABLE)
		{
			strcpy (measure, "-");
		}
		
		strcat (measure, "\0");
	}
	/*Y1564 配置*/
	else
	{
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
		vty_out (vty, "%-5s %-8s %-5s %-9s %-8s %-5s %-17s%s","ID","Protocol","Size","CIR/EIR","Duration","Interval","Measure",VTY_NEWLINE);
		vty_out (vty, "%-5u ", psess->info.session_id);
		vty_out (vty, "%-8s ", "Y.1564");
		
		/*配置测试*/
		if(psess->info.sla_y1564_stage == SLA_Y1564_CONFIGURATION)
		{
			strcpy (measure, "Configuration");
		}
		/*性能测试*/
		else if ((psess->info.sla_y1564_stage == SLA_Y1564_PERFORMANCE)
			&& (SLA_MEASURE_ENABLE == psess->info.lm_enable || SLA_MEASURE_ENABLE == psess->info.dm_enable))
		{
			if (SLA_MEASURE_ENABLE == psess->info.lm_enable && SLA_MEASURE_ENABLE == psess->info.dm_enable)
			{
				strcpy (measure, "Performance : Loss/Jitter");
			}
			else if (SLA_MEASURE_ENABLE == psess->info.lm_enable)
			{
				strcpy (measure, "Performance : Loss");
			}
			else
			{
				strcpy (measure, "Performance : Jitter");
			}
		}
		else
		{
			strcpy (measure, "-");
		}
		strcat (measure, "\0");
		vty_out (vty, "%-5u ", psess->info.pkt_size);
		vty_out (vty, "%-4u/%-4u ", psess->info.cir, psess->info.eir);
		vty_out (vty, "%-8u ", psess->info.y1564_con_duration);
		vty_out (vty, "%-8u ", psess->info.y1564_per_duration);
		vty_out (vty, "%-8u ", psess->info.interval);
	}
	
	vty_out (vty, "%-17s %s", measure, VTY_NEWLINE);
	vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	
	if (psess->info.proto == SLA_PROTO_Y1564)
	{
		vty_out (vty, "SAC :%s", VTY_NEWLINE);
		vty_out (vty, "%-6s: %u%s%s", "Loss", psess->info.sac_loss * SLA_SAC_FLR_BENCHMARK/SLA_PKT_LOSS_ENLAGE, "/100000", VTY_NEWLINE);
		vty_out (vty, "%-6s: %-6u us%s", "Delay", psess->info.sac_delay/SLA_LATENCY_ENLAGE, VTY_NEWLINE);
		vty_out (vty, "%-6s: %-6u us%s", "Jitter", psess->info.sac_jitter/SLA_LATENCY_ENLAGE, VTY_NEWLINE);
		
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	}

	/*ethernet报文配置*/
	vty_out (vty, "Ethernet Packet :%s", VTY_NEWLINE);
	vty_out (vty, "%-6s: %02X:%02X:%02X:%02X:%02X:%02X%s", "Dmac",psess->pkt_eth.dmac[0], psess->pkt_eth.dmac[1],psess->pkt_eth.dmac[2], \
	psess->pkt_eth.dmac[3],psess->pkt_eth.dmac[4],psess->pkt_eth.dmac[5],VTY_NEWLINE);
	vty_out (vty, "%-6s: %02X:%02X:%02X:%02X:%02X:%02X%s", "Smac",psess->pkt_eth.smac[0], psess->pkt_eth.smac[1],psess->pkt_eth.smac[2], \
	psess->pkt_eth.smac[3],psess->pkt_eth.smac[4],psess->pkt_eth.smac[5],VTY_NEWLINE);

	if (psess->pkt_eth.svlan != 0 && psess->pkt_eth.cvlan != 0)
	{
		vty_out (vty, "%-6s: %d%s", "Cvlan",psess->pkt_eth.cvlan, VTY_NEWLINE);
		vty_out (vty, "%-6s: 0x%x%s", "C-tpid",psess->pkt_eth.c_tpid, VTY_NEWLINE);
		vty_out (vty, "%-6s: %d%s", "C-cos",psess->pkt_eth.c_cos, VTY_NEWLINE);
		vty_out (vty, "%-6s: %d%s", "Svlan",psess->pkt_eth.svlan, VTY_NEWLINE);
		vty_out (vty, "%-6s: 0x%x%s", "S-tpid",psess->pkt_eth.s_tpid, VTY_NEWLINE);
		vty_out (vty, "%-6s: %d%s", "S-cos",psess->pkt_eth.s_cos, VTY_NEWLINE);
	}
	else if (psess->pkt_eth.svlan != 0 && psess->pkt_eth.cvlan == 0)
	{
		vty_out (vty, "%-6s: %s%s", "Cvlan", "-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "C-tpid","-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "C-cos", "-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %d%s", "Svlan", psess->pkt_eth.svlan, VTY_NEWLINE);
		vty_out (vty, "%-6s: 0x%x%s", "S-tpid",psess->pkt_eth.s_tpid, VTY_NEWLINE);
		vty_out (vty, "%-6s: %d%s", "S-cos",psess->pkt_eth.s_cos, VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "%-6s: %s%s", "Cvlan", "-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "C-tpid","-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "C-cos", "-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "Svlan", "-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "S-tpid","-", VTY_NEWLINE);
		vty_out (vty, "%-6s: %s%s", "S-cos", "-", VTY_NEWLINE);
	}
	
	vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	
	/*ip报文配置*/
	vty_out (vty, "IP Packet :%s", VTY_NEWLINE);
        
	vty_out (vty, "%-6s: %s%s", "Sip",   inet_ipv4tostr(psess->pkt_ip.sip, sourceip), VTY_NEWLINE);
	vty_out (vty, "%-6s: %s%s", "Dip",   inet_ipv4tostr(psess->pkt_ip.dip, destip),   VTY_NEWLINE);
	vty_out (vty, "%-6s: %d%s", "Sport", psess->pkt_ip.sport, VTY_NEWLINE);
	vty_out (vty, "%-6s: %d%s", "Dport", psess->pkt_ip.dport, VTY_NEWLINE);
	vty_out (vty, "%-6s: %d%s", "Dscp",  psess->pkt_ip.dscp,  VTY_NEWLINE);
	vty_out (vty, "%-6s: %d%s", "Ttl",   psess->pkt_ip.ttl,   VTY_NEWLINE);
	vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	
	return ;
}
#endif



/**************************公共命令行************************/
DEFUN (sla_session_get,
	sla_session_get_cmd,
	"sla session <1-65535>",
	SLA_STR
	"Sla session\n"
	"Session id <1-65535>\n"
	)
{
	uint16_t sla_id = 0;
	struct sla_session *p_session = NULL;
	sint32 ret = ERRNO_SUCCESS;
	
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ("sla session", sla_id, argv[0], 1, 65535);
	
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d\n\n",__FILE__,__LINE__,__func__, sla_id);
	
	/* 查找一个会话 */
	p_session = sla_session_lookup(sla_id);
	if (p_session == NULL)
	{
		/* 创建一个会话 */
		p_session = sla_session_create(sla_id);
		if (p_session == NULL)
		{
			vty_error_out(vty, "%%Create sla session failed%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		/*添加一个会话*/
		ret = sla_session_add(p_session);
		if (ret != ERRNO_SUCCESS)
		{
		    vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
    
	vty->index = p_session;
    vty->node = SLA_SESSION_NODE;
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_session_get,
	no_sla_session_get_cmd,
	"no sla session <1-65535> {to <1-65535>}",
	"No command\n"
	SLA_STR
	"Sla session\n"
	"Session id <1-65535>\n"
	"To"
	"Session id <1-65535>\n"
	)
{
	uint32_t sess_id =0;
	sint32 ret = ERRNO_SUCCESS;
	struct sla_session *p_session = NULL;
	struct sla_if_entry sla_if;
	
    /*判断输入参数是否合法*/
	memset(&sla_if, 0, sizeof(struct sla_if_entry));
    VTY_GET_INTEGER_RANGE ("sla session", sla_if.start_id, argv[0], 1, 65535);

	if (argv[1] == NULL)
	{
		sla_if.end_id = sla_if.start_id;
	}
	else
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla session", sla_if.end_id, argv[1], 1, 65535);
	}
	
	if(sla_if.end_id < sla_if.start_id)
	{
		vty_error_out(vty, "%%Sla input error,sla_end(%d) must be not less than sla_start(%d) !%s",
                sla_if.end_id, sla_if.start_id, VTY_NEWLINE);
		return CMD_WARNING;
	}

    if(sla_entry.ifindex != 0)
    {
        if((sla_if.start_id >= sla_entry.start_id && sla_if.start_id <= sla_entry.end_id) ||
            (sla_if.end_id >= sla_entry.start_id && sla_if.end_id <= sla_entry.end_id))
        {
            vty_error_out(vty, "%%Please disable sla test before deleting sla session.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    
	/*判断输入节点是否不存在*/
	for (sess_id = sla_if.start_id; (sess_id != 0) && (sess_id < (uint32_t)sla_if.end_id + 1); sess_id++)
	{
		p_session = sla_session_lookup(sess_id);
		if(p_session == NULL)
		{
			vty_error_out(vty, "%%Sla input error, sla session %d do not exist!%s", sess_id, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/*不包含*/
	if ((sla_if.end_id < sla_entry.start_id) || (sla_if.start_id > sla_entry.end_id))
	{
		for (sess_id = sla_if.start_id; (sess_id != 0) && (sess_id < (uint32_t)sla_if.end_id + 1); sess_id++)
		{
			ret = sla_session_delete(sess_id);
			if (ret != ERRNO_SUCCESS)
			{
				vty_warning_out(vty, "%%Can't find session id %d%s", sess_id, VTY_NEWLINE);
			}
		}
        
        return CMD_SUCCESS;
	}

    /*半包含*/
	vty_error_out(vty, "%%Sla input error, sla session id %d to %d,must be include %d to %d,or not!%s", 
	        sla_if.start_id, sla_if.end_id, sla_entry.start_id, sla_entry.end_id, VTY_NEWLINE);
	return CMD_WARNING;
}


/* change */
DEFUN (sla_protocol_select,
	sla_protocol_select_cmd,
	"protocol (rfc2544 | y1564)",
	"Sla protocol\n"
	"Protocol rfc2544\n"
	"Protocol y1564\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uint16_t session_id = sla_sess->info.session_id;
	int ret = 0;

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.proto != SLA_PROTO_INVALID)
	{
		vty_warning_out(vty, "Protocol is already config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(sla_sess, 0, sizeof(struct sla_session));
	sla_sess->info.session_id = session_id;
	sla_sess->info.state = SLA_STATUS_DISABLE;

	if (0 == strncmp("r", argv[0], strlen("r")))
	{
		sla_sess->info.proto = SLA_PROTO_RFC2544;
		
		/* 默认七个字节连续测 */
		sla_sess->info.pkt_size = 64;
		sla_sess->info.sla_2544_size[0] = 64;
		sla_sess->info.sla_2544_size[1] = 128;
		sla_sess->info.sla_2544_size[2] = 256;
		sla_sess->info.sla_2544_size[3] = 512;
		sla_sess->info.sla_2544_size[4] = 1024;
		sla_sess->info.sla_2544_size[5] = 1280;
		sla_sess->info.sla_2544_size[6] = 1518;		
	}
	else if (0 == strncmp("y", argv[0], strlen("y")))
	{	
		sla_sess->info.proto = SLA_PROTO_Y1564;
		sla_sess->info.pkt_size = SLA_DEF_PKT_SIZE;
		memset(sla_sess->info.sla_2544_size, 0, 14);
		
		sla_sess->info.sac_loss = SLA_SAC_DEF_FLR * SLA_PKT_LOSS_ENLAGE/SLA_SAC_FLR_BENCHMARK;
		sla_sess->info.sac_delay = SLA_SAC_DEF_FTD * SLA_LATENCY_ENLAGE;
		sla_sess->info.sac_jitter = SLA_SAC_DEF_FDV * SLA_LATENCY_ENLAGE;	
	}
	else 
	{
		vty_error_out(vty, "%%Command err, please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* 发包速率没有默认值 */
	//sla_sess->info.pkt_rate_upper = SLA_DEF_PKT_RATE;
	sla_sess->info.pkt_rate_lower = 0;/*速率默认1000*/
	sla_sess->info.duration = SLA_DEF_DURATION;/*默认测试间隔*/

	sla_sess->pkt_eth.c_tpid = 0x8100;
	sla_sess->pkt_eth.s_tpid = 0x8100;
	sla_sess->pkt_eth.c_cos = SLA_DEF_COS;
	sla_sess->pkt_eth.s_cos = SLA_DEF_COS;
	
	sla_sess->pkt_ip.dscp = SLA_DEF_DSCP;
	sla_sess->pkt_ip.dport = UDP_PORT_ECHO;
	sla_sess->pkt_ip.sport = UDP_PORT_RFC2544;
	sla_sess->pkt_ip.ttl = SLA_DEF_IP_TTL;

	ret = devm_comm_get_mac ( 1, 0, MODULE_ID_QOS, sla_sess->pkt_eth.smac);
	if (ret)
	{
		zlog_err( "%s[%d]:Sla get smac from devm error,mac == NULL\n", __FILE__, __LINE__);
	}
	
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d protocol %s\n\n", __FILE__, __LINE__, __func__, 
		sla_sess->info.session_id, sla_sess->info.proto == SLA_PROTO_RFC2544 ? "rfc2544" : "y1564");
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_protocol_select,
	no_sla_protocol_select_cmd,
	"no protocol",
	"No command\n"
	"Sla protocol\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uint16_t session_id = sla_sess->info.session_id;

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	memset(sla_sess, 0, sizeof(struct sla_session));

	sla_sess->info.session_id = session_id;
	
	return CMD_SUCCESS;
}


/* change */
DEFUN (sla_ethernet_format_get,
	sla_ethernet_format_get_cmd,
	"service ethernet {smac XX:XX:XX:XX:XX:XX | dmac XX:XX:XX:XX:XX:XX | cvlan <1-4094> | c-tpid (0x9100|0x9200|0x88a8|0x8100) | c-cos <0-7> | svlan <1-4094> | s-tpid (0x9100|0x9200|0x88a8|0x8100) | s-cos <0-7>}",
	"Service\n"
	"Ethernet protocol\n"
	"Smac\n"
	"Smac XX:XX:XX:XX:XX:XX\n"
	"Dmac\n"
	"Dmac XX:XX:XX:XX:XX:XX\n"
	"Cvlan\n"
	"Cvlan <1-4094>\n"
	"C-tpid\n"
	"C-tpid 0x9100\n"
	"C-tpid 0x9200\n"
	"C-tpid 0x88a8\n"
	"C-tpid 0x8100\n"
	"C-cos\n"
	"C-cos <0-7>\n"
	"Svlan\n"
	"Svlan <1-4094>\n"
	"S-tpid\n"
	"S-tpid 0x9100\n"
	"S-tpid 0x9200\n"
	"S-tpid 0x88a8\n"
	"S-tpid 0x8100\n"
	"S-cos\n"
	"S-cos <0-7>\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uchar smac_o[ETH_ALEN] = {0};
	uchar dmac_o[ETH_ALEN] = {0};
	uchar smac[ETH_ALEN] = {0};
	uchar dmac[ETH_ALEN] = {0};
	uchar mac[ETH_ALEN]  = {0x00,0x00,0x00,0x00,0x00,0x01};

	if(sla_sess->info.proto == SLA_PROTO_INVALID)
	{
		vty_warning_out(vty, "Please config protocol at first.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	/* 先保存配置前已配的MAC值*/
	memcpy(smac_o, sla_sess->pkt_eth.smac, ETH_ALEN);
	memcpy(dmac_o, sla_sess->pkt_eth.dmac, ETH_ALEN);
	
	/*设置源MAC*/
	if (argv[0] != NULL)
	{
		/* mac 地址校验 */
		if (ether_valid_mac(argv[0]))
		{
			vty_error_out(vty, "Invalid smac value%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		memset(smac, 0, ETH_ALEN);
		ether_string_to_mac(argv[0], smac);
		
		/*判断是否为广播mac*/
		if (!ether_is_broadcast_mac(smac))
		{
			vty_error_out(vty, "Can't set smac is broadcast mac.%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		
		/*判断是否为组播mac*/
		if (!ether_is_muticast_mac(smac))
		{
			vty_error_out(vty, "Can't set smac is multicast mac.%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(!memcmp(mac, smac, ETH_ALEN))
		{
			vty_error_out(vty, "Invalid smac value%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
		memcpy(sla_sess->pkt_eth.smac, smac, ETH_ALEN);
	}
	
	/*设置目的MAC*/
	if (argv[1] != NULL)
	{		
		/* mac 地址校验 */
		if (ether_valid_mac(argv[1]))
		{
			vty_error_out(vty, "Invalid dmac value%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
		memset(dmac, 0, ETH_ALEN);
		ether_string_to_mac(argv[1], dmac);
		/*判断是否为广播mac*/
		if (!ether_is_broadcast_mac(dmac))
		{
			vty_error_out(vty, "Can't set dmac is broadcast mac.%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		/*判断是否为组播mac*/
		if (!ether_is_muticast_mac(dmac))
		{
			vty_error_out(vty, "Can't set dmac is multicast mac.%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(!memcmp(mac, dmac, ETH_ALEN))
		{
			vty_error_out(vty, "Invalid dmac value%s",VTY_NEWLINE);
			return CMD_WARNING;
		}		
		memcpy(sla_sess->pkt_eth.dmac, dmac, ETH_ALEN);
	}

	if((argv[0] != NULL || argv[1] != NULL) && 
		!memcmp(sla_sess->pkt_eth.smac, sla_sess->pkt_eth.dmac, ETH_ALEN))
	{
		memcpy(sla_sess->pkt_eth.smac, smac_o, ETH_ALEN);
		memcpy(sla_sess->pkt_eth.dmac, dmac_o, ETH_ALEN);
		
		vty_error_out(vty, "Source mac address can't be equal to destination.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if (argv[2] != NULL)
	{
		/*不能只配置内层vlan*/
		if (sla_sess->pkt_eth.svlan == 0 && argv[5] == NULL)
		{
			vty_error_out(vty, "Please set svlan frist.%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla cvlan", sla_sess->pkt_eth.cvlan, argv[2], 1, 4094);
	}
	
	/*设置内层vlan tpid*/
	if ( argv[3] != NULL)
	{
		if (strncmp ( argv[3], "0x9100", 6 ) == 0 )
		{
			sla_sess->pkt_eth.c_tpid = 0x9100;
		}
		else if (strncmp ( argv[3], "0x9200", 6 ) == 0 )
		{
			sla_sess->pkt_eth.c_tpid = 0x9200;
		}
		else if (strncmp ( argv[3], "0x88a8", 6 ) == 0 )
		{
			sla_sess->pkt_eth.c_tpid = 0x88a8;
		}
		else if (strncmp ( argv[3], "0x8100", 6 ) == 0 )
		{
			sla_sess->pkt_eth.c_tpid = 0x8100;
		}
		else
		{
			vty_error_out(vty, "%%Invalid c-tpid value%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
	}
	
	if (argv[4] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla cos", sla_sess->pkt_eth.c_cos, argv[4], 0, 7);
	}
	
	if (argv[5] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla svlan", sla_sess->pkt_eth.svlan, argv[5], 1, 4094);
	}
	
	if ( argv[6] != NULL)
	{
		if (strncmp ( argv[6], "0x9100", 6 ) == 0 )
		{
			sla_sess->pkt_eth.s_tpid = 0x9100;
		}
		else if (strncmp ( argv[6], "0x9200", 6 ) == 0 )
		{
			sla_sess->pkt_eth.s_tpid = 0x9200;
		}
		else if (strncmp ( argv[6], "0x88a8", 6 ) == 0 )
		{
			sla_sess->pkt_eth.s_tpid = 0x88a8;
		}
		else if (strncmp ( argv[6], "0x8100", 6 ) == 0 )
		{
			sla_sess->pkt_eth.s_tpid = 0x8100;
		}
		else
		{
			vty_error_out(vty, "%%Invalid s-tpid value%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
	}
	
	if (argv[7] != NULL)
	{
		/*不能只配置内层vlan*/
		if (sla_sess->pkt_eth.svlan == 0 && argv[5] == NULL)
		{
			vty_error_out(vty, "Please set svlan frist.%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
	
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla cos", sla_sess->pkt_eth.s_cos, argv[7], 0, 7);
	}

	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d dmac %02X:%02X:%02X:%02X:%02X:%02X smac %02X:%02X:%02X:%02X:%02X:%02X cvlan %d c-tpid %d c-cos%d svlan %d s-tpid %d s-cos %d\n\n", \
		__FILE__, __LINE__, __func__,  sla_sess->info.session_id, sla_sess->pkt_eth.dmac[0], sla_sess->pkt_eth.dmac[1], sla_sess->pkt_eth.dmac[2], \
		sla_sess->pkt_eth.dmac[3], sla_sess->pkt_eth.dmac[4], sla_sess->pkt_eth.dmac[5], sla_sess->pkt_eth.smac[0], sla_sess->pkt_eth.smac[1], \
		sla_sess->pkt_eth.smac[2], sla_sess->pkt_eth.smac[3], sla_sess->pkt_eth.smac[4], sla_sess->pkt_eth.smac[5], sla_sess->pkt_eth.cvlan, \
		sla_sess->pkt_eth.c_tpid, sla_sess->pkt_eth.c_cos,sla_sess->pkt_eth.svlan, sla_sess->pkt_eth.s_tpid,sla_sess->pkt_eth.s_cos);

	return CMD_SUCCESS;
}

DEFUN (no_sla_ethernet_format_get,
	no_sla_ethernet_format_get_cmd,
	"no service ethernet",
	"No command\n"
	"Service\n"
	"Ethernet protocol\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	sint32 ret = ERRNO_SUCCESS;

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	memset(&sla_sess->pkt_eth, 0, sizeof(struct sla_pkt_eth));
	
	/* sla 测试例不配置协议时，没有任何默认值 */
	if(sla_sess->info.proto != SLA_PROTO_INVALID)
	{
		sla_sess->pkt_eth.c_cos = SLA_DEF_COS;
		sla_sess->pkt_eth.s_cos = SLA_DEF_COS;
		sla_sess->pkt_eth.c_tpid = 0x8100;
		sla_sess->pkt_eth.s_tpid = 0x8100;
		ret = devm_comm_get_mac ( 1, 0, MODULE_ID_QOS, sla_sess->pkt_eth.smac);
		if (ret)
		{
			zlog_err ( "%s[%d]:sla get smac from devm error,mac == NULL.", __FILE__, __LINE__);
		}
	}
	
	return CMD_SUCCESS;
}


DEFUN (sla_ip_format_get,
	sla_ip_format_get_cmd,
	"service ip {sip A.B.C.D | dip A.B.C.D | sport <1-65535> | dport <1-65535> | ttl <1-255> | dscp <0-63>}",
	"Service\n"
	"Ip protocol\n"
	"Sip\n"
	"Sip A.B.C.D\n"
	"Dip\n"
	"Dip A.B.C.D\n"
	"Sport\n"
	"Sport <1-65535>\n"
	"Dport\n"
	"Dport <1-65535>\n"
	"Ttl\n"
	"Ttl <1-255>\n"
	"Dscp\n"
	"Dscp <0-63>\n")
{
	struct sla_session *p_session = (struct sla_session *)vty->index;
	uint32_t sip = 0, dip = 0;

	if(p_session->info.proto == SLA_PROTO_INVALID)
	{
		vty_warning_out(vty, "Please config protocol at first.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, p_session->info.state);
	
	if (argv[0] != NULL)
	{
		sip = inet_strtoipv4((char *)argv[0]);
		if (1 != inet_valid_ipv4(sip))
		{
			vty_error_out(vty, "The sip is invalid.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		p_session->pkt_ip.sip = sip;
	}
	
	if (argv[1] != NULL)
	{
		dip = inet_strtoipv4((char *)argv[1]);
		if (1 != inet_valid_ipv4(dip))
		{
			vty_error_out(vty, "The dip is invalid.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		p_session->pkt_ip.dip = dip;
	}
	
	if (argv[2] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla sport", p_session->pkt_ip.sport, argv[2], 1, 65535);
	}

	if (argv[3] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla dport", p_session->pkt_ip.dport, argv[3], 1, 65535);
	}

	if (argv[4] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla ttl", p_session->pkt_ip.ttl, argv[4], 1, 255);
	}
	
	if (argv[5] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla dscp", p_session->pkt_ip.dscp, argv[5], 0, 63);
	}
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_ip_format_get,
	no_sla_ip_format_get_cmd,
	"no service ip",
	"No command\n"
	"Service\n"
	"Ip protocol\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	memset(&sla_sess->pkt_ip, 0, sizeof(struct sla_pkt_ip));

	if(sla_sess->info.proto != SLA_PROTO_INVALID)
	{
		sla_sess->pkt_ip.dscp = SLA_DEF_DSCP;
		sla_sess->pkt_ip.dport = UDP_PORT_ECHO;
		sla_sess->pkt_ip.sport = UDP_PORT_RFC2544;
		sla_sess->pkt_ip.ttl = SLA_DEF_IP_TTL;
	}
	
	return CMD_SUCCESS;
}


/*****************************RFC2544命令行*******************************/
/* 专用于rfc2544测试例配置测试包长 */
DEFUN (sla_2544_packet_size_get,
	sla_2544_packet_size_get_cmd,
	"rfc2544 packet {size1 <64-9600> | size2 <64-9600> | size3 <64-9600> | size4 <64-9600> | size5 <64-9600> | size6 <64-9600> | size7 <64-9600>}",
    "Rfc2544\n"
    "Packet\n"
	"Size1\n"
	"Range of size1:<64-9600>\n"
	"Size2\n"
	"Range of size2:<64-9600>\n"
	"Size3\n"
	"Range of size3:<64-9600>\n"
	"Size4\n"
	"Range of size4:<64-9600>\n"
	"Size5\n"
	"Range of size5:<64-9600>\n"
	"Size6\n"
	"Range of size6:<64-9600>\n"
	"Size7\n"
	"Range of size7:<64-9600>\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uint16_t size[7] = {0};
	int config_flag = 0;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_error_out(vty, "Only RFC2544 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	memset(sla_sess->info.sla_2544_size, 0, 14);
	
	if(argv[0] != NULL)
	{
		size[0] = atoi(argv[0]);
		config_flag = 1;
	}
	if(argv[1] != NULL)
	{
		size[1] = atoi(argv[1]);
		if(size[1] == size[0])
		{
			vty_out(vty, "Error:Include repeated parameters.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		config_flag = 1;
	}
	if(argv[2] != NULL)
	{
		size[2] = atoi(argv[2]);
		if(size[2] == size[0] || size[2] == size[1])
		{
			vty_out(vty, "Error:Include repeated parameters.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		config_flag = 1;
	}
	if(argv[3] != NULL)
	{
		size[3] = atoi(argv[3]);
		if(size[3] == size[0] || size[3] == size[1]
			|| size[3] == size[2])
		{
			vty_out(vty, "Error:Include repeated parameters.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		config_flag = 1;
	}
	if(argv[4] != NULL)
	{
		size[4] = atoi(argv[4]);
		if(size[4] == size[0] || size[4] == size[1]
			|| size[4] == size[2] || size[4] == size[3])
		{
			vty_out(vty, "Error:Include repeated parameters.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		config_flag = 1;
	}
	if(argv[5] != NULL)
	{
		size[5] = atoi(argv[5]);
		if(size[5] == size[0] || size[5] == size[1]
			|| size[5] == size[2] || size[5] == size[3]
				|| size[5] == size[4])
		{
			vty_out(vty, "Error:Include repeated parameters.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		config_flag = 1;
	}
	if(argv[6] != NULL)
	{
		size[6] = atoi(argv[6]);
		if(size[6] == size[0] || size[6] == size[1]
			|| size[6] == size[2] || size[6] == size[3]
				|| size[6] == size[4] || size[6] == size[5])
		{
			vty_out(vty, "Error:Include repeated parameters.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		config_flag = 1;
	}

	if(0 == config_flag)
	{
		size[0] = 64;
		size[1] = 128;
		size[2] = 256;
		size[3] = 512;
		size[4] = 1024;
		size[5] = 1280;
		size[6] = 1518;
	}
	
	memcpy(sla_sess->info.sla_2544_size, size, 14);

	return CMD_SUCCESS;
}

DEFUN (no_sla_2544_packet_size_get,
	no_sla_2544_packet_size_get_cmd,
	"no rfc2544 packet size",
	"Delete\n"
    "Rfc2544\n"
    "Packet\n"
	"Size\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uint16_t size[7] = {0};

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_error_out(vty, "Only RFC2544 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	memset(sla_sess->info.sla_2544_size, 0, 14);
	
	size[0] = 64;
	size[1] = 128;
	size[2] = 256;
	size[3] = 512;
	size[4] = 1024;
	size[5] = 1280;
	size[6] = 1518;

	memcpy(sla_sess->info.sla_2544_size, size, 14);

	return CMD_SUCCESS;
}


/* change */
/* 用于RFC2544配置丢包、时延、吞吐量的发包速率:支持kbps */
DEFUN (sla_packet_rate_get,
	sla_packet_rate_get_cmd,
	"packet rate <1-10000000> {to <1-10000000>}",
	"Packet\n"
	"Packet rate\n"
	"Packet rate upper limit <1-10000000> kbps\n"
	"To\n"
	"Packet rate lower limit <1-10000000> kbps\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uint32_t lower_rate = 0, upper_rate = 0;
	
	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_error_out(vty, "Only RFC2544 support config packet rate.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	/* 配置RFC2544发包速率前必须先使能测试项 */
	if((sla_sess->info.lm_enable == SLA_MEASURE_DISABLE)
		&& (sla_sess->info.dm_enable == SLA_MEASURE_DISABLE)
			&& (sla_sess->info.throughput_enable == SLA_MEASURE_DISABLE))
	{
		vty_warning_out(vty, "Please enable RFC2544 testing item before config packet rate.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* 丢包率和时延只指定一个发包速率 */
	/*单位kbps*/
	sla_sess->info.pkt_rate_upper = 0;
	sla_sess->info.pkt_rate_lower = 0;		
	VTY_GET_INTEGER_RANGE ("sla packet rate", upper_rate, argv[0], 1, 10000000);

	/*单位kbps*/
	if (argv[1] != NULL)
	{
		/* 只有吞吐量测试才可以指定两个发包速率:lower和upper */
		if (sla_sess->info.throughput_enable == SLA_MEASURE_ENABLE)
		{
			VTY_GET_INTEGER_RANGE ("sla packet rate", lower_rate, argv[1], 1, 10000000);
		}
		else
		{
			vty_warning_out(vty, "%%Rate lower is only effective to throughput.%s",VTY_NEWLINE);
		}
		
		if (upper_rate <= lower_rate)
		{
			vty_error_out(vty, "Packet rate upper can't be lower than lower rate%s",VTY_NEWLINE);
			return CMD_WARNING;
		}

		sla_sess->info.pkt_rate_lower = lower_rate;
	}
	
	sla_sess->info.pkt_rate_upper = upper_rate;

	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d packet rate upper%d, lower %d\n\n",
		__FILE__, __LINE__, __func__, sla_sess->info.session_id, sla_sess->info.pkt_rate_upper, \
		sla_sess->info.pkt_rate_lower);
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_packet_rate_get,
	no_sla_packet_rate_get_cmd,
	"no packet rate",
	"No command\n"
	"Packet\n"
	"Packet rate\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_warning_out(vty, "Only RFC2544 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	/*速率放大*/
	sla_sess->info.pkt_rate_upper = 0;
	sla_sess->info.pkt_rate_lower = 0;
	
	return CMD_SUCCESS;
}

/* change */
DEFUN (sla_packet_loss,
	sla_packet_loss_cmd,
	"measure packet-loss",
	"Measure\n"
	"Packet-loss\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_error_out(vty, "Only RFC2544 support configuring packet loss.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.dm_enable == SLA_MEASURE_ENABLE)
	{
		sla_sess->info.dm_enable = SLA_MEASURE_DISABLE;
		vty_warning_out(vty, "Test:packet loss will takes effect and test:packet delay will be invalid.%s", VTY_NEWLINE);
	}

	if(sla_sess->info.throughput_enable == SLA_MEASURE_ENABLE)
	{
		sla_sess->info.throughput_enable = SLA_MEASURE_DISABLE;
		vty_warning_out(vty, "Test:packet loss will takes effect and test:throughput will be invalid.%s", VTY_NEWLINE);
	}
	
	sla_sess->info.lm_enable = SLA_MEASURE_ENABLE;
	
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d measure packet-loss %s\n\n",__FILE__,__LINE__,__func__,
		sla_sess->info.session_id, sla_sess->info.lm_enable == SLA_MEASURE_ENABLE ? "enable" : "disable");
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_packet_loss,
	no_sla_packet_loss_cmd,
	"no measure packet-loss",
	"No command\n"
	"Measure\n"
	"Packet-loss\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_warning_out(vty, "Only RFC2544 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	sla_sess->info.lm_enable = SLA_MEASURE_DISABLE;
		
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d measure packet-loss %s\n\n",__FILE__,__LINE__,__func__,
		sla_sess->info.session_id, sla_sess->info.lm_enable == SLA_MEASURE_ENABLE ? "enable" : "disable");
	
	return CMD_SUCCESS;
}



/* change */
DEFUN (sla_packet_delay,
	sla_packet_delay_cmd,
	"measure packet-delay",
	"Measure\n"
	"Packet-delay\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_error_out(vty, "Only RFC2544 support configuring packet delay.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.lm_enable == SLA_MEASURE_ENABLE)
	{
		sla_sess->info.lm_enable = SLA_MEASURE_DISABLE;
		vty_warning_out(vty, "Test:packet delay will takes effect and test:packet loss will be invalid.%s", VTY_NEWLINE);
	}

	if(sla_sess->info.throughput_enable == SLA_MEASURE_ENABLE)
	{
		sla_sess->info.throughput_enable = SLA_MEASURE_DISABLE;
		vty_warning_out(vty, "Test:packet delay will takes effect and test:throughput will be invalid.%s", VTY_NEWLINE);
	}
	
	sla_sess->info.dm_enable = SLA_MEASURE_ENABLE;
		
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d measure packet-delay %s\n\n",__FILE__, __LINE__, __func__,
		sla_sess->info.session_id, sla_sess->info.dm_enable == SLA_MEASURE_ENABLE ? "enable" : "disable");
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_packet_delay,
	no_sla_packet_delay_cmd,
	"no measure packet-delay",
	"Delete\n"
	"Measure\n"
	"Packet-delay\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_warning_out(vty, "Only RFC2544 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	sla_sess->info.dm_enable = SLA_MEASURE_DISABLE;
		
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d measure packet-delay %s\n\n", __FILE__, __LINE__, __func__,
		sla_sess->info.session_id, sla_sess->info.dm_enable == SLA_MEASURE_ENABLE ? "enable" : "disable");
	
	return CMD_SUCCESS;
}

/* change */
DEFUN (sla_packet_throughput,
	sla_packet_throughput_cmd,
	"measure throughput",
	"Measure\n"
	"Throughput\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	
	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_error_out(vty, "Only RFC2544 support configuring throughput.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.lm_enable == SLA_MEASURE_ENABLE)
	{
		sla_sess->info.lm_enable = SLA_MEASURE_DISABLE;
		vty_warning_out(vty, "Test:throughput will takes effect and test:packet loss will be invalid.%s", VTY_NEWLINE);
	}

	if(sla_sess->info.dm_enable == SLA_MEASURE_ENABLE)
	{
		sla_sess->info.dm_enable = SLA_MEASURE_DISABLE;
		vty_warning_out(vty, "Test:throughput will takes effect and test:packet delay will be invalid.%s", VTY_NEWLINE);
	}
	
	sla_sess->info.throughput_enable = SLA_MEASURE_ENABLE;
		
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d measure throughput %s\n\n",__FILE__,__LINE__,__func__,
		sla_sess->info.session_id, sla_sess->info.throughput_enable == SLA_MEASURE_ENABLE ? "enable" : "disable");
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_packet_throughput,
	no_sla_packet_throughput_cmd,
	"no measure throughput",
	"Delete\n"
	"Measure\n"
	"Throughput\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_RFC2544)
	{
		vty_warning_out(vty, "Only RFC2544 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	sla_sess->info.throughput_enable = SLA_MEASURE_DISABLE;
		
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d measure throughput %s\n\n", __FILE__, __LINE__, __func__, 
		sla_sess->info.session_id, sla_sess->info.throughput_enable == SLA_MEASURE_ENABLE ? "enable" : "disable");
	
	return CMD_SUCCESS;
}

/* 超时时长 */
DEFUN (sla_packet_duration,
	sla_packet_duration_cmd,
	"duration <1-60>",
	"Duration\n"
	"Duration <1-60> Unit: s\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto == SLA_PROTO_INVALID)
	{
		vty_error_out(vty, "Please config protocol at firat.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	sla_sess->info.duration = atoi(argv[0]);
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_packet_duration,
	no_sla_packet_duration_cmd,
	"no duration",
	"Delete\n"
	"Duration\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto == SLA_PROTO_INVALID)
	{
		vty_error_out(vty, "Please config protocol at firat.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	sla_sess->info.duration = SLA_DEF_DURATION;

	return CMD_SUCCESS;
}



/******************************y.1564命令行*******************************/

/* 专用于y.1564,   配置1564测试例的任意包长           */
DEFUN (sla_y1564_packet_size_get,
	sla_y1564_packet_size_get_cmd,
	"y1564 packet size <64-9600>",
    "Y1564\n"
	"Packet\n"
	"Packet size\n"
	"Range of packet size:<64-9600>\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;	

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	/*单位Byte */
	VTY_GET_INTEGER_RANGE ("sla pkt size", sla_sess->info.pkt_size, argv[0], 64, 9600);
	
	zlog_debug(QOS_DBG_SLA, "%s[%d]:%s:sla session id %d y1564 pkt size %d\n\n",
		__FILE__, __LINE__, __func__, sla_sess->info.session_id, sla_sess->info.pkt_size);
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_y1564_packet_size_get,
	no_sla_y1564_packet_size_get_cmd,
	"no y1564 packet size",
	"No command\n"
    "Y1564\n"
	"Packet\n"
	"Packet size\n"
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	sla_sess->info.pkt_size = SLA_DEF_PKT_SIZE;
	
	return CMD_SUCCESS;
}


/* y.1564使能流量监管测试:此流量监管测试为色盲模式 */
DEFUN (sla_y1564_traffic_policing_enable,
	sla_y1564_traffic_policing_enable_cmd,
	"traffic-policing-test enable",
	"Traffic-policing test of Configuration-test\n"
	"Enable test\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support configuring this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.sla_y1564_stage == SLA_Y1564_PERFORMANCE)
	{
		vty_error_out(vty, "Performance test can't support traffic policing test.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	sla_sess->info.traf_policing_enable = SLA_MEASURE_ENABLE;

	return CMD_SUCCESS;
}

DEFUN (no_sla_y1564_traffic_policing_enable,
	no_sla_y1564_traffic_policing_enable_cmd,
	"no traffic-policing-test enable",
	"Delete\n"
	"Traffic-policing test of Configuration-test\n"
	"Enable test\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support configuring this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(sla_sess->info.sla_y1564_stage == SLA_Y1564_PERFORMANCE)
	{
		vty_error_out(vty, "Performance test can't support traffic policing test.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	sla_sess->info.traf_policing_enable = SLA_MEASURE_DISABLE;

	return CMD_SUCCESS;
}
	

/* 使能配置测试或性能测试，暂不支持自动连续进行 */
DEFUN (sla_y1564_test_type,
	sla_y1564_test_type_cmd,
	"y1564 enable (configuration-test | performance-test)",
	"Protocol:y.1564\n"
	"Enable test\n"
	"configuration-test\n"
	"performance-test\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support configuring this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	if(argv[0][0] == 'c')
	{
		sla_sess->info.sla_y1564_stage = SLA_Y1564_CONFIGURATION;
	}
	else if(argv[0][0] == 'p')
	{
		sla_sess->info.sla_y1564_stage = SLA_Y1564_PERFORMANCE;
	}

	return CMD_SUCCESS;
}
	
DEFUN (no_sla_y1564_test_type,
	no_sla_y1564_test_type_cmd,
	"no y1564 enable",
	"Delete\n"
	"Protocol:y.1564\n"
	"Enable test\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support configuring this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	sla_sess->info.sla_y1564_stage = SLA_Y1564_TESTTYPE_INVALID;

	return CMD_SUCCESS;
}


/* 配置 y.1564 的cir 和 eir:eir配置为0，则不进行eir测试。eir测试为色盲测试 */
DEFUN (sla_bandwidth_profile,
	sla_bandwidth_profile_cmd,
	"bandwidth profile cir <256-1000000> eir <0-1000000>",
	"Bandwidth\n"
	"Profile\n"
	"Committed information rate\n"
	"256-1000000 kbps\n"
	"Excess information rate\n"
	"0-1000000 kbps\n")
{
	struct sla_session *sla_sess  = (struct sla_session *)vty->index;
	
	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support config cir and eir.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	VTY_GET_INTEGER_RANGE ("sla cir", sla_sess->info.cir, argv[0], 256, 1000000);
	VTY_GET_INTEGER_RANGE ("sla eir", sla_sess->info.eir, argv[1], 0, 1000000);
	
	return CMD_SUCCESS;
}

DEFUN (no_sla_bandwidth_profile,
	no_sla_bandwidth_profile_cmd,
	"no bandwidth profile",
	"No command\n"
	"Bandwidth\n"
	"Profile\n"
	) 
{
	struct sla_session *p_session  = (struct sla_session *)vty->index;
	
	if(p_session->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support configuring this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	p_session->info.cir = 0;
	p_session->info.eir = 0;
	
	return CMD_SUCCESS;
}

/* 配置服务验收标准 */
DEFUN (sla_sac,
	sla_sac_cmd,
	"sac {loss <0-100000> | delay <1-10000000> | jitter <1-10000000>}",
	SAC_STR
	"Frame loss ratio\n"
	"loss <0-100000> Unit: hundred thousandth\n"
	"Frame transfer delay\n"
	"delay <1-10000000> Unit: us\n"
	"Frame delay variation\n"
	"jitter <1-10000000> Unit: us\n")
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;
	uint32_t sac_loss = 0;	/*十万分之一*/
	uint32_t sac_delay = 0; /*延迟*/
	uint32_t sac_jitter = 0;	/*抖动*/

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support config sac parameters.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);
	
	if (argv[0] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla sac loss", sac_loss, argv[0], 0, 100000);
		/*放大1000倍*/
		sla_sess->info.sac_loss = (sac_loss * SLA_PKT_LOSS_ENLAGE)/SLA_SAC_FLR_BENCHMARK;
	}

	if (argv[1] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla sac delay", sac_delay, argv[1], 1, 10000000);
		sla_sess->info.sac_delay = (sac_delay * SLA_LATENCY_ENLAGE);
	}
	
	if (argv[2] != NULL)
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla sac jitter", sac_jitter, argv[2], 1, 10000000);
		sla_sess->info.sac_jitter = (sac_jitter * SLA_LATENCY_ENLAGE);
	}
	
	return CMD_SUCCESS;
}


DEFUN (no_sla_sac,
	no_sla_sac_cmd,
	"no sac",
	"No command\n"
	SAC_STR
	)
{
	struct sla_session *sla_sess = (struct sla_session *)vty->index;

	if(sla_sess->info.proto != SLA_PROTO_Y1564)
	{
		vty_error_out(vty, "Only y.1564 support this command line.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	SLA_TEST_IS_RUNNING(vty, sla_sess->info.state);

	sla_sess->info.sac_loss = SLA_SAC_DEF_FLR * SLA_PKT_LOSS_ENLAGE/SLA_SAC_FLR_BENCHMARK;
	sla_sess->info.sac_delay = SLA_SAC_DEF_FTD * SLA_LATENCY_ENLAGE;
	sla_sess->info.sac_jitter = SLA_SAC_DEF_FDV * SLA_LATENCY_ENLAGE;
	
	return CMD_SUCCESS;
}



/****************************接口下使能命令行*******************************/

DEFUN (sla_interface_session_enable,
	sla_interface_session_enable_cmd,
	"sla enable session <1-65535> {to <1-65535>}",
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n")
{
	uint32_t ifindex = (uint32_t)vty->index;
	struct sla_session *p_session = NULL;
	char ifname[IFNET_NAMESIZE] = "";
	struct sla_if_entry sla_if;
	struct ifm_info if_info;
	uint32_t i = 0;
	uint32_t if_speed = 0;

	/* 1/0/1不支持sla */
	if(IFM_TYPE_IS_METHERNET(ifindex) && (0 == IFM_SLOT_ID_GET(ifindex)))
	{
		vty_error_out(vty, "%%Interface:1/0/1 is not support sla.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    if (g_sla_flag)
    {
		vty_error_out(vty, "%%Please disable sla session %d to %d%s", sla_entry.start_id, sla_entry.end_id, VTY_NEWLINE);
		return CMD_WARNING;
    }
    
	/* l3接口不支持sla */
	if(ifm_get_all_info(ifindex, MODULE_ID_QOS, &if_info) != 0)
	{
		vty_error_out(vty, "Fail to get interface information.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(if_info.mode == IFNET_MODE_L3)
	{
		vty_error_out(vty, "L3 mode can't support sla function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*判断接口是否配置环回*/
	if (if_loopback.g_sla_loopback_count)
	{
		vty_error_out(vty, "%%Sla can't set with loopback meanwhile!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检测会话是否已经存在*/
	memset(&sla_if, 0, sizeof(struct sla_if_entry));
	sla_if.ifindex = ifindex;
	
	if ((sla_entry.ifindex != 0) && (sla_entry.ifindex != sla_if.ifindex))
	{
		ifm_get_name_by_ifindex(sla_entry.ifindex, ifname);
		vty_error_out(vty, "%%Please disable sla session %d to %d in %s!%s",
				sla_entry.start_id, sla_entry.end_id, ifname,VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*端口已使能,需先disable*/
	if(sla_entry.state == SLA_STATUS_ENABLE || sla_entry.state == SLA_STATUS_UNFINISH)
	{
		vty_error_out(vty, "%%Sla session %d to %d unfinish, please disable frist%s",
				sla_entry.start_id, sla_entry.end_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*判断输入参数是否合法*/
	VTY_GET_INTEGER_RANGE ("sla session", sla_if.start_id, argv[0], 1, SLA_SESSION_ID_MAX);

	if (argv[1] == NULL)
	{
		sla_if.end_id = sla_if.start_id;
	}
	else
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla session", sla_if.end_id, argv[1], 1, SLA_SESSION_ID_MAX);
		if(sla_if.end_id <= sla_if.start_id)
		{
			vty_error_out(vty, "%%Sla input error,sla_end(%d) must be not less than sla_start(%d) !%s",
				sla_if.end_id, sla_if.start_id, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	if(sla_entry.state == SLA_STATUS_FINISH)
	{
		if(sla_if.start_id != sla_entry.start_id || sla_if.end_id != sla_entry.end_id)
		{
			vty_error_out(vty, "%%Please disable session %d to %d before enable session %d to %d.%s",
				sla_entry.start_id, sla_entry.end_id, sla_if.start_id, sla_if.end_id, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/*最大支持8条y1564 flow*/
	if((sla_if.end_id - sla_if.start_id) > (SLA_TEST_FLOW_MAX - 1))
	{
		vty_error_out(vty, "%%Sla input error,sla session id list must limit in eight !%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*获取接口带宽，用来在下面判断配置的2544的速率是否超过接口带宽*/
	if((if_speed = if_speed_get(ifindex)) == 0)
	{
		vty_error_out(vty, "%%Interface speed get fail, cannot perform test operations. !%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*判断输入节点是否不存在*/
	for (i = sla_if.start_id; i < ((uint32_t)sla_if.end_id + 1); i++)
	{
		p_session = sla_session_lookup(i);
		if(p_session == NULL)
		{
			vty_error_out(vty, "%%Sla input error, sla session %d do not exist!%s", i, VTY_NEWLINE);
			return CMD_WARNING;
		}

		/* rfc2544 协议与y.1564协议配置测试只能同时使能1条流 */
		/* y.1564协议性能测试最大同时支持8条流 */		
		if(p_session->info.proto == SLA_PROTO_RFC2544)
		{
			if(sla_if.start_id != sla_if.end_id)
			{
				vty_error_out(vty, "%%Sla input error, a flow :rfc2544 protocol or y.1564 configuration; multiple flow: y.1564 performance%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			if(p_session->info.pkt_rate_upper < 1)
			{
				vty_warning_out(vty, "Please config the packet rate.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}

			/*判断配置的速率是否大于接口带宽，bug#58312*/
			if(p_session->info.pkt_rate_upper > if_speed)
			{
				vty_error_out(vty, "%%The configured rate more than the bandwidth, bandwidth is %d kbps.%s", if_speed, VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
		else
		{
			if(p_session->info.sla_y1564_stage != SLA_Y1564_CONFIGURATION
				&& p_session->info.sla_y1564_stage != SLA_Y1564_PERFORMANCE)
			{
				vty_error_out(vty, "Please choose y1564 test stage:configuration-test or performance test.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			if(p_session->info.sla_y1564_stage == SLA_Y1564_CONFIGURATION && (sla_if.start_id != sla_if.end_id))
			{
				vty_error_out(vty, "Configuration-test only support one service.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
            if (p_session->info.cir == 0)
            {
				vty_warning_out(vty, "bandwidth profile cir and eir .%s", VTY_NEWLINE);
				return CMD_WARNING;
            }
		}

		
		if(p_session->info.proto == SLA_PROTO_RFC2544){
			/*必须设置测试项*/
			if (p_session->info.throughput_enable == SLA_MEASURE_DISABLE &&
					p_session->info.lm_enable == SLA_MEASURE_DISABLE &&
						p_session->info.dm_enable == SLA_MEASURE_DISABLE)
			{
				vty_error_out(vty, "%%Sla input error, sla must specify test item%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
		
		/*目的mac不能为0*/
		if (!ether_is_zero_mac(p_session->pkt_eth.dmac))
		{
			vty_error_out(vty, "Session id %u, input dmac is invalid.%s", i, VTY_NEWLINE );
			return CMD_WARNING;
		}
	}

	/*HT157 vop enable 与 evc建立*/
	sla_session_send_event(ifindex);
	
	/*sla session 下发配置*/
	sla_session_send_down(sla_if.start_id, sla_if.end_id, IPC_OPCODE_ADD, SLA_STATUS_ENABLE);
		
	/* 会话 enable */
	sla_session_enable(&sla_if);
	sla_if.state = SLA_STATUS_ENABLE;
    g_sla_flag = 1;
	/*保存配置*/
	memcpy(&sla_entry, &sla_if, sizeof(struct sla_if_entry));
	
	return CMD_SUCCESS;
}


DEFUN (no_sla_interface_session_enable,
	no_sla_interface_session_enable_cmd,
	"no sla enable session <1-65535> {to <1-65535>}",
	"No command\n"
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)
{
	char ifname[IFNET_NAMESIZE] = "";
	struct sla_if_entry sla_if;
	
	memset(&sla_if, 0, sizeof(struct sla_if_entry));
	sla_if.ifindex = (uint32_t)vty->index;
	
	/*判断输入参数是否合法*/
	VTY_GET_INTEGER_RANGE ("sla session", sla_if.start_id, argv[0], 1, 65535);
	
	if (argv[1] == NULL)
	{
		sla_if.end_id = sla_if.start_id;
	}
	else
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla session", sla_if.end_id, argv[1], 1, 65535);
	}

	if(sla_if.end_id < sla_if.start_id)
	{
		vty_error_out(vty, "%%Sla input error,sla_end(%d) must be not less than sla_start(%d) !%s",
				sla_if.end_id, sla_if.start_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*最大支持8条y1564 flow*/
	if((sla_if.end_id - sla_if.start_id) > (SLA_TEST_FLOW_MAX - 1))
	{
		vty_error_out(vty, "%%Sla input error,sla session id num must limit in eight !%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*查看是否为保存sla配置*/
	if (sla_if.ifindex != sla_entry.ifindex)
	{
		ifm_get_name_by_ifindex(sla_entry.ifindex, ifname);
		vty_error_out(vty, "%%Sla input error,Please disable sla session in %s!%s", ifname, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if((sla_entry.start_id == sla_if.start_id) && (sla_entry.end_id == sla_if.end_id))
	{	
		/* 会话 disable */
		sla_session_disable(&sla_if);
		
		/*删除sla session*/
		sla_session_send_down(sla_if.start_id, sla_if.end_id, IPC_OPCODE_DELETE, SLA_STATUS_DISABLE);

		/*清空保存配置*/
		memset(&sla_entry, 0, sizeof(struct sla_if_entry));
        g_sla_flag = 0;
		return CMD_SUCCESS;
	}
    
	vty_error_out(vty, "%%Sla input error, sla delete node must be %d to %d%s", 
			sla_entry.start_id, sla_entry.end_id, VTY_NEWLINE);
	
	return CMD_WARNING;
}



ALIAS (sla_interface_session_enable,
	sla_sub_interface_session_enable_cmd,
	"sla enable session <1-65535> {to <1-65535>}",
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n")

	
ALIAS (no_sla_interface_session_enable,
	no_sla_sub_interface_session_enable_cmd,
	"no sla enable session <1-65535> {to <1-65535>}",
	"No command\n"
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n")






DEFUN (sla_show_config,
	sla_show_config_cmd,
	"show sla config [session <1-65535>]",
	"Show command\n"
	SLA_STR
	"Config\n"
	"Session\n"
	"Session <1-65535>\n"
	)
{
	struct sla_session *p_session = NULL;
	uint16_t sess_id;
	
	if (argv[0] == NULL)
	{	
		sla_config_write(vty);
		
		return CMD_SUCCESS;
	}
	
	/*判断输入参数是否合法*/
	VTY_GET_INTEGER_RANGE ("sla session", sess_id, argv[0], 1, 65535);
	
	p_session = sla_session_lookup(sess_id);
	if(p_session == NULL)
	{
		vty_error_out(vty, "%%Sla input error, sla session %d do not exist!%s", sess_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	sla_config_info_show(vty, p_session);
	
	return CMD_SUCCESS;
}

DEFUN (sla_show_session,
	sla_show_session_cmd,
	"show sla session <1-65535> {to <1-65535>}",
	"Show command\n"
	SLA_STR
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)
{
    struct sla_session *p_session = NULL;
	uint16_t start_id = 0;
	uint16_t end_id = 0;
	uint32_t i = 0;
	
    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ("sla session", start_id, argv[0], 1, 65535);

	if (argv[1] == NULL)
	{
		end_id = start_id;
	}
	else
	{
		/*判断输入参数是否合法*/
		VTY_GET_INTEGER_RANGE ("sla session", end_id, argv[1], 1, 65535);
        if(start_id > end_id)
        {
            vty_error_out(vty, "%%Sla input error,sla_end(%d) must be not less than sla_start(%d)!%s",
                    start_id, end_id, VTY_NEWLINE);
            return CMD_WARNING;
        }
	}
    
	for (i = start_id; i < (uint32_t)end_id+1; i++)
	{
		p_session = sla_session_lookup(i);
		if(p_session == NULL)
		{
			vty_error_out(vty, "%%Sla session %d doesn't exist!%s", i, VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		if (p_session->info.proto == SLA_PROTO_RFC2544)
		{
			sla_2544_result_show(vty, p_session);
		}
		else if(p_session->info.proto == SLA_PROTO_Y1564)
		{
			sla_1564_result_show(vty, p_session);
		}
        vty_out(vty, "%s", VTY_NEWLINE);
    }
	return CMD_SUCCESS;
}

/*设备配置环回*/
DEFUN ( reflector,
        reflector_cmd,
        "reflector (external|internal) {exchange-mac} {exchange-ip}",
        "Interface set reflector\n"
        "Set external reflector\n"
        "Set internal reflector\n" 
        "Exchange mac\n" 
        "Exchange ip\n" )
{
	uint32_t ifindex = (uint32_t)vty->index;
	struct listnode *pnode = NULL;

	if(if_loopback.if_list == NULL)
	{
		if_loopback.if_list = list_new();
	}

	pnode = listnode_lookup(if_loopback.if_list, (void *)ifindex);
	if(pnode == NULL)
	{
		listnode_add(if_loopback.if_list, (void *)ifindex);
		if_loopback.g_sla_loopback_count++;
	}
	
	return CMD_SUCCESS;
}
		
/*设备取消环回*/
DEFUN ( no_reflector,
        no_reflector_cmd,
        "no reflector (external|internal)",
        NO_STR
        "Interface set reflector\n"
        "Set external reflector\n"
        "Set internal reflector\n" )
{
	uint32_t ifindex = (uint32_t)vty->index;
	struct listnode *pnode = NULL;

	if(if_loopback.if_list == NULL)
	{
		return CMD_SUCCESS;
	}

	pnode = listnode_lookup(if_loopback.if_list, (void *)ifindex);
	if(pnode == NULL)
	{
		return CMD_SUCCESS;
	}
	
	listnode_delete(if_loopback.if_list, (void *)ifindex);
	if_loopback.g_sla_loopback_count--;
	
	return CMD_SUCCESS;
}


/*sla 删除子接口配置*/
void sla_if_delete(uint32_t ifindex)
{
	struct listnode *pnode = NULL;

	if(if_loopback.if_list != NULL)
	{
		pnode = listnode_lookup(if_loopback.if_list, (void *)ifindex);
		if(pnode != NULL)
		{
			listnode_delete(if_loopback.if_list, (void *)ifindex);
			if_loopback.g_sla_loopback_count--;
		}
	}

	/*查看是否为保存sla配置*/
	if ((ifindex != sla_entry.ifindex) || (ifindex == 0) || (sla_entry.ifindex == 0))
	{
		return ;
	}

	if (sla_entry.state == SLA_STATUS_ENABLE)
	{
		/*更新sla session*/
		//sla_session_send_down(sla_entry.start_id, sla_entry.end_id, IPC_OPCODE_UPDATE, SLA_STATUS_DISABLE);
			
		/* 会话 disable */
		//g_sla_flag = 0;
		sla_session_disable(&sla_entry);
	}
	g_sla_flag = 0;	//Modify by zzl 20190109
	
	/*清空保存配置*/
	memset(&sla_entry, 0, sizeof(struct sla_if_entry));
	
	return ;
}

/* write the configuration of each interface */ 
/* static int sla_if_config_write (struct vty *vty)
{
    char ifname[IFNET_NAMESIZE] = "";

	if (sla_entry.ifindex)
	{
		ifm_get_name_by_ifindex(sla_entry.ifindex, ifname);
		vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );
		if (sla_entry.start_id == sla_entry.end_id)
		{
			vty_out ( vty, " sla enable session %d%s", sla_entry.start_id, VTY_NEWLINE );
		}
		else
		{
			vty_out ( vty, " sla enable session %d to %d%s", sla_entry.start_id, sla_entry.end_id, VTY_NEWLINE );
		}
	}
	
	return ERRNO_SUCCESS;
}
*/ 


/* write the configuration */
int sla_config_write (struct vty *vty)
{
	uint16_t def_size[7]           = {64, 128, 256, 512, 1024, 1280, 1518};
	struct sla_session *p_session  = NULL;
	struct hash_bucket *pbucket    = NULL;
    char sourceip[INET_ADDRSTRLEN] = "";
    char destip[INET_ADDRSTRLEN]   = "";
	uint8_t mac_addr[ETH_ALEN]     = {0};
	int cursor                     = 0;
    int i                          = 0;

	HASH_BUCKET_LOOP(pbucket, cursor, sla_session_table)
	{
		p_session = pbucket->data;
		vty_out (vty, "sla session %d%s", p_session->info.session_id, VTY_NEWLINE);

		if(p_session->info.proto == SLA_PROTO_RFC2544)
		{
			vty_out (vty, " protocol %s%s", "rfc2544", VTY_NEWLINE);
		}
		else if (p_session->info.proto == SLA_PROTO_Y1564)
		{
			vty_out (vty, " protocol %s%s", "y1564", VTY_NEWLINE);
		}

		/*获取设备源mac*/
		devm_comm_get_mac ( 1, 0, MODULE_ID_QOS, mac_addr);
		if (memcmp(p_session->pkt_eth.smac, mac_addr, ETH_ALEN))
		{
			vty_out (vty, " service ethernet smac %02X:%02X:%02X:%02X:%02X:%02X", \
				p_session->pkt_eth.smac[0], p_session->pkt_eth.smac[1], p_session->pkt_eth.smac[2], \
				p_session->pkt_eth.smac[3], p_session->pkt_eth.smac[4], p_session->pkt_eth.smac[5]);
		}
		else if (ether_is_zero_mac(p_session->pkt_eth.dmac))
		{
			vty_out (vty, " service ethernet dmac %02X:%02X:%02X:%02X:%02X:%02X", \
				p_session->pkt_eth.dmac[0], p_session->pkt_eth.dmac[1], p_session->pkt_eth.dmac[2], \
				p_session->pkt_eth.dmac[3], p_session->pkt_eth.dmac[4], p_session->pkt_eth.dmac[5]);
		}
		
		if ((memcmp(p_session->pkt_eth.smac, mac_addr, ETH_ALEN)) && (ether_is_zero_mac(p_session->pkt_eth.dmac)))
		{
			vty_out (vty, " dmac %02X:%02X:%02X:%02X:%02X:%02X", \
				p_session->pkt_eth.dmac[0], p_session->pkt_eth.dmac[1], p_session->pkt_eth.dmac[2], \
				p_session->pkt_eth.dmac[3], p_session->pkt_eth.dmac[4], p_session->pkt_eth.dmac[5]);
		}
		
		if (p_session->pkt_eth.cvlan != 0)
		{
			vty_out (vty, " cvlan %d", p_session->pkt_eth.cvlan);
			if (p_session->pkt_eth.c_tpid != 0x8100)
			{
				vty_out (vty, " c-tpid 0x%x", p_session->pkt_eth.c_tpid);
			}
			
			if (p_session->pkt_eth.c_cos != SLA_DEF_COS)
			{
				vty_out (vty, " c-cos %d", p_session->pkt_eth.c_cos);
			}
		}
		
		if (p_session->pkt_eth.svlan != 0)
		{
			vty_out (vty, " svlan %d", p_session->pkt_eth.svlan);
			if (p_session->pkt_eth.s_tpid != 0x8100)
			{
				vty_out (vty, " s-tpid 0x%x", p_session->pkt_eth.s_tpid);
			}
			if (p_session->pkt_eth.s_cos != SLA_DEF_COS)
			{
				vty_out (vty, " s-cos %d", p_session->pkt_eth.s_cos);
			}
		}
        
		vty_out (vty, "%s", VTY_NEWLINE);
        
        if (!ipv4_is_zeronet(p_session->pkt_ip.sip))
		{
			vty_out (vty, " service ip sip %s", inet_ipv4tostr(p_session->pkt_ip.sip, sourceip));
		}
		else if (!ipv4_is_zeronet(p_session->pkt_ip.dip))
        {      
			vty_out (vty, " service ip dip %s", inet_ipv4tostr(p_session->pkt_ip.dip, destip));
		}

		if ((!ipv4_is_zeronet(p_session->pkt_ip.sip)) && (!ipv4_is_zeronet(p_session->pkt_ip.dip)))
		{
		    vty_out (vty, " dip %s", inet_ipv4tostr(p_session->pkt_ip.dip, destip));
		}

		if (p_session->pkt_ip.dport != UDP_PORT_ECHO && p_session->pkt_ip.dport != 0)
		{
			vty_out (vty, " dport %d", p_session->pkt_ip.dport);
		}

		if (p_session->pkt_ip.sport != UDP_PORT_RFC2544 && p_session->pkt_ip.sport != 0)
		{
			vty_out (vty, " sport %d", p_session->pkt_ip.sport);
		}
		
		if (SLA_DEF_IP_TTL != p_session->pkt_ip.ttl && p_session->pkt_ip.ttl != 0)
		{
			vty_out (vty, " ttl %d", p_session->pkt_ip.ttl);
		}

		if (SLA_DEF_DSCP != p_session->pkt_ip.dscp)
		{
			vty_out (vty, " dscp %d", p_session->pkt_ip.dscp);
		}
		
		vty_out (vty, "%s", VTY_NEWLINE);

		if (p_session->info.lm_enable != SLA_MEASURE_DISABLE)
		{
			vty_out (vty, " measure packet-loss %s", VTY_NEWLINE);
		}

		if (p_session->info.dm_enable != SLA_MEASURE_DISABLE)
		{
			vty_out (vty, " measure packet-delay %s", VTY_NEWLINE);
		}

		if (p_session->info.throughput_enable != SLA_MEASURE_DISABLE)
		{
			vty_out (vty, " measure throughput %s", VTY_NEWLINE);
		}
        
		if (p_session->info.proto == SLA_PROTO_Y1564
			&& p_session->info.pkt_size != SLA_DEF_PKT_SIZE)
		{
			vty_out (vty, " y1564 packet size %d%s", p_session->info.pkt_size, VTY_NEWLINE);
		}

		/*2544閰嶇疆閫熺巼*/
		if (p_session->info.proto == SLA_PROTO_RFC2544)
		{
			if(memcmp(p_session->info.sla_2544_size, def_size, 14))
			{
					vty_out(vty, " rfc2544 packet");
					for(i = 0; i < 7; i++)
					{
						if(p_session->info.sla_2544_size[i] != 0)
						{
							vty_out(vty, " size%d %d", i+1, p_session->info.sla_2544_size[i]);
						}
					}
					vty_out(vty, "%s", VTY_NEWLINE);
			}
			if(p_session->info.pkt_rate_lower != 0
				|| p_session->info.pkt_rate_upper != 0)
			{
				vty_out(vty, " packet rate %d", p_session->info.pkt_rate_upper);
				if(p_session->info.pkt_rate_lower != 0)
				{
					vty_out(vty, " to %d", p_session->info.pkt_rate_lower);
				}
				vty_out(vty, "%s", VTY_NEWLINE);
			}
			#if 0
			if (p_session->info.throughput_enable != SLA_MEASURE_DISABLE)
			{
				if ((p_session->info.pkt_rate_upper != SLA_DEF_PKT_RATE ) || (p_session->info.pkt_rate_lower != 0))
				{
					vty_out (vty, " packet rate %d to %d%s", p_session->info.pkt_rate_upper, p_session->info.pkt_rate_lower, VTY_NEWLINE);
				}
			}
			else
			{
				if (p_session->info.pkt_rate_upper != 0)
				{
					vty_out (vty, " packet rate %d%s", p_session->info.pkt_rate_upper, VTY_NEWLINE);
				}
			}
			#endif
		}
		/*1564 CIR/EIR*/
		else if(p_session->info.cir != 0)
		{
			vty_out (vty, " bandwidth profile cir %u eir %u%s", p_session->info.cir, p_session->info.eir, VTY_NEWLINE);
		}


		#if 0
		if ((p_session->info.interval != SLA_DEF_INTERVAL && p_session->info.interval != 0)
			|| (p_session->info.frequency != SLA_DEF_FREQUENCY && p_session->info.frequency != 0))
		{
			vty_out (vty, " schedule ");
			if(p_session->info.interval != SLA_DEF_INTERVAL && p_session->info.interval != 0)
			{
				vty_out(vty, "interval %d ", p_session->info.interval);
			}
			if(p_session->info.frequency != SLA_DEF_FREQUENCY && p_session->info.frequency != 0)
			{
				vty_out(vty, "frequency %d", p_session->info.frequency);
			}
			vty_out(vty, "%s", VTY_NEWLINE);
		}
		#endif

		if(p_session->info.duration != 0 && p_session->info.duration != SLA_DEF_DURATION)
		{
			vty_out(vty, " duration %d%s", p_session->info.duration, VTY_NEWLINE);
		}

		if(p_session->info.traf_policing_enable == SLA_MEASURE_ENABLE)
		{
			vty_out(vty, " traffic-policing-test enable%s", VTY_NEWLINE);
		}

		if(p_session->info.sla_y1564_stage == SLA_Y1564_CONFIGURATION)
		{
			vty_out(vty, " y1564 enable configuration-test%s", VTY_NEWLINE);
		}
		else if(p_session->info.sla_y1564_stage == SLA_Y1564_PERFORMANCE)
		{
			vty_out(vty, " y1564 enable performance-test%s", VTY_NEWLINE);
		}

		/*sac 配置恢复*/
		if((p_session->info.sac_delay != 0
			&& p_session->info.sac_delay != (SLA_SAC_DEF_FTD * SLA_LATENCY_ENLAGE))
				|| (p_session->info.sac_delay != 0
					&& p_session->info.sac_loss != (SLA_SAC_DEF_FLR * SLA_PKT_LOSS_ENLAGE/SLA_SAC_FLR_BENCHMARK))
						|| (p_session->info.sac_delay != 0
							&& p_session->info.sac_jitter != (SLA_SAC_DEF_FDV * SLA_LATENCY_ENLAGE)))
		{
			vty_out(vty, " sac");
			if(p_session->info.sac_delay != 0
				&& p_session->info.sac_loss != (SLA_SAC_DEF_FLR * SLA_PKT_LOSS_ENLAGE/SLA_SAC_FLR_BENCHMARK))
			{
				vty_out(vty, " loss %d", p_session->info.sac_loss * SLA_SAC_FLR_BENCHMARK/SLA_PKT_LOSS_ENLAGE);
			}
			if(p_session->info.sac_delay != 0
				&& p_session->info.sac_delay != (SLA_SAC_DEF_FTD * SLA_LATENCY_ENLAGE))
			{
				vty_out(vty, " delay %d", p_session->info.sac_delay/SLA_LATENCY_ENLAGE);
			}
			if(p_session->info.sac_delay != 0
				&& p_session->info.sac_jitter != (SLA_SAC_DEF_FDV * SLA_LATENCY_ENLAGE))
			{
				vty_out(vty, " jitter %d", p_session->info.sac_jitter/SLA_LATENCY_ENLAGE);
			}
			vty_out(vty, "%s", VTY_NEWLINE);
		}
		
		vty_out (vty, "%s", VTY_NEWLINE);
	}
	
	/*接口下sla配置*/
	//sla_if_config_write(vty);
		
	return ERRNO_SUCCESS;
}

void sla_cli_init(void)
{
	/*注册sla节点下命令*/
	install_node (&sla_session_node, sla_config_write);
	install_default (SLA_SESSION_NODE);
    install_element (CONFIG_NODE, &sla_session_get_cmd , CMD_SYNC);
    install_element (CONFIG_NODE, &no_sla_session_get_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_protocol_select_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_protocol_select_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_ethernet_format_get_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_ethernet_format_get_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_ip_format_get_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_ip_format_get_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_y1564_packet_size_get_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_y1564_packet_size_get_cmd , CMD_SYNC);

	install_element (SLA_SESSION_NODE, &sla_2544_packet_size_get_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_2544_packet_size_get_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_packet_rate_get_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_packet_rate_get_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_packet_loss_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_packet_loss_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_packet_delay_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_packet_delay_cmd , CMD_SYNC);
	
    install_element (SLA_SESSION_NODE, &sla_packet_throughput_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_packet_throughput_cmd , CMD_SYNC);

	install_element (SLA_SESSION_NODE, &sla_y1564_traffic_policing_enable_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_y1564_traffic_policing_enable_cmd , CMD_SYNC);

	install_element (SLA_SESSION_NODE, &sla_y1564_test_type_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_y1564_test_type_cmd , CMD_SYNC);

	install_element (SLA_SESSION_NODE, &sla_packet_duration_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_packet_duration_cmd , CMD_SYNC);

	/*设置SAC*/
    install_element (SLA_SESSION_NODE, &sla_sac_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_sac_cmd , CMD_SYNC);

	/*甯﹀妯℃澘*/
    install_element (SLA_SESSION_NODE, &sla_bandwidth_profile_cmd , CMD_SYNC);
    install_element (SLA_SESSION_NODE, &no_sla_bandwidth_profile_cmd , CMD_SYNC);
	
	/*物理接口*/
    install_element (PHYSICAL_IF_NODE, &sla_interface_session_enable_cmd , CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_sla_interface_session_enable_cmd , CMD_SYNC);

	/*注册环回通知命令*/
    install_element (PHYSICAL_IF_NODE, &reflector_cmd , CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_reflector_cmd , CMD_SYNC);
	
    install_element ( PHYSICAL_SUBIF_NODE, &reflector_cmd  , CMD_SYNC);
    install_element ( PHYSICAL_SUBIF_NODE, &no_reflector_cmd  , CMD_SYNC);
	
	/*子接口*/
    install_element (PHYSICAL_SUBIF_NODE, &sla_sub_interface_session_enable_cmd , CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_sla_sub_interface_session_enable_cmd , CMD_SYNC);

    install_element (CONFIG_NODE, &sla_show_config_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &sla_show_session_cmd, CMD_LOCAL);
	
	return ;
}

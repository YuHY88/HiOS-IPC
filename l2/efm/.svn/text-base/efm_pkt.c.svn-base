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
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/alarm.h>
#include "efm.h"
#include "efm_state.h"
#include "efm_def.h"
#include "efm_pkt.h"
#include "efm_agent.h"
#include "lldp/lldp_msg.h"

static struct efm_oam_pdu efmpdu;
extern unsigned char		l2_mac[MAC_LEN];
extern uint8_t oui_data[5];
extern uint8_t msdh_DstAdrr[];
extern uint8_t msdh_SrcAdrr[];

void efm_pkt_register(void)
{
	union proto_reg proto;



	/*报文注册为主机序,hsl下发转换为网络序*/
	memset(&proto, 0, sizeof(union proto_reg));



	proto.ethreg.ethtype = EFM_TYPE;
	proto.ethreg.sub_ethtype = EFM_SUBTYPE;

	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);	

	return;
}


void efm_pkt_unregister(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));

	proto.ethreg.ethtype = EFM_TYPE;
	proto.ethreg.sub_ethtype = EFM_SUBTYPE;

	pkt_unregister(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);
	return;
}

/*efm information PDU subpackage 802.3ah 57.5.2 Information TLVs*/
	int
efm_format_information_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
	int  j = 0;
	struct efm_info *local_info= NULL;
	struct efm_info *remote_info = NULL;

	EFM_LOG_DBG("EFM[TX]: Encoding Information EFMPDU\n");

    if(NULL == pefm)
    {
		return -1;
    }

    if(NULL == bufptr)
    {
		return -1;
    }

	local_info = &pefm->local_info;
	remote_info = &pefm->rem_info;

	if((pefm->efm_local_par_action_t != pefm->efm_local_par_action_tlv_lst) ||    \
			(pefm->efm_local_mux_action_t != pefm->efm_local_mux_action_tlv_lst) ||    \
			(pefm->local_info.oam_config != pefm->efm_config_tlv_lst) ||                     \
			(pefm->local_info.oam_pdu_config != pefm->efm_pdu_config_tlv_lst))
	{
		pefm->local_info.revision ++;
		pefm->efm_local_par_action_tlv_lst = pefm->efm_local_par_action_t;
		pefm->efm_local_mux_action_tlv_lst = pefm->efm_local_mux_action_t;
		pefm->efm_config_tlv_lst = pefm->local_info.oam_config;
		pefm->efm_pdu_config_tlv_lst = pefm->local_info.oam_pdu_config;
	}

	j = 0;

	/* Subtype */
	bufptr[j++] = EFM_SUBTYPE;
	bufptr[j++] = (uint8_t)(pefm->efm_flags >> 8);           //first save high 8 bits  00 
	bufptr[j++] = (uint8_t)(pefm->efm_flags & 0xff);    //then save low 8 bits
    if(EFM_INFO == pefm->local_pdu
            ||  EFM_ANY == pefm->local_pdu)
	{
		EFM_LOG_DBG("EFM[TX]: Encoding Local Information TLV\n");

		bufptr[j++]  = EFM_INFO_CODE;
		bufptr[j++]  = EFM_LOCAL_INFORMATION_TLV;
		bufptr[j++]  = EFM_INFO_TLV_LENGTH;
		bufptr[j++]  = EFM_VERSION;
		bufptr[j++]  = (uint8_t)(local_info->revision >> 8);
		bufptr[j++]  = (uint8_t)(local_info->revision & 0xff);
		bufptr[j++]  = ((pefm->efm_local_par_action_t                             // state
					& EFM_PAR_MASK)
				|(pefm->efm_local_mux_action_t
					<< EFM_MUX_BIT_SHIFT));
		bufptr[j++]  = local_info->oam_config;                                              // mode loopback   undir   link event     
		bufptr[j++]  = (uint8_t)(local_info->oam_pdu_config >> 8);
		bufptr[j++]  = (uint8_t)(local_info->oam_pdu_config & 0xff); 

		/*OUI,Vendor specific information Not specified currently*/
		memcpy (bufptr + j, pefm->mac, EFM_OUI_LENGTH);
		j += EFM_OUI_LENGTH;
		memset (bufptr + j, 0, EFM_VSI_LENGTH);
		j += EFM_VSI_LENGTH;


        if(EFM_TRUE == pefm->efm_remote_state_valid)
		{
			EFM_LOG_DBG("EFM[TX]: Encoding Remote Information TLV\n");

			bufptr[j++]  = EFM_REMOTE_INFORMATION_TLV;
			bufptr[j++]  = EFM_INFO_TLV_LENGTH;
			bufptr[j++]  = remote_info->oam_version;
			bufptr[j++]  = (uint8_t)(remote_info->revision >> 8);
			bufptr[j++]  = (uint8_t)(remote_info->revision & 0xff);
			bufptr[j++]  = remote_info->state;
			bufptr[j++]  = remote_info->oam_config;
			bufptr[j++]  = (uint8_t)(remote_info->oam_pdu_config >> 8);
			bufptr[j++]  = (uint8_t)(remote_info->oam_pdu_config & 0xff); 
			memcpy (bufptr + j, remote_info->oui, EFM_OUI_LENGTH);
			j += EFM_OUI_LENGTH;
			memcpy (bufptr + j, remote_info->vend_spec_info, EFM_VSI_LENGTH);
			j += EFM_VSI_LENGTH;
		}
	}

	else
	{

		while(j<64)
		{
			bufptr[j++] = 0;
			
		}

	}
	pefm->tx_count [EFM_INFORMATION_PDU] += 1;

	return j;
}
/*efm loopback PDU subpackage 802.3ah 57.4.3.5 Loopback Control OAMPDU*/

	int
efm_format_loopback_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
	int j;

    if(NULL == pefm)
    {
		return -1;
    }

    if(NULL == bufptr)
    {
		return -1;
    }

	EFM_LOG_DBG("EFM[TX]: Encoding Remote Loopback EFMPDU with remote "
			"loopback flag %s\n", pefm->efm_rem_loopback ? "Enabled"
			: "Disabled");

	j = 0;

	/* Subtype */
	bufptr[j++] = EFM_SUBTYPE;
	bufptr[j++] = (uint8_t)(pefm->efm_flags >> 8);
	bufptr[j++] = (uint8_t)(pefm->efm_flags & 0xff);
	bufptr[j++] = EFM_LOOPBACK_CONTROL;

	if (pefm->efm_rem_loopback)
    {
		bufptr[j++] = EFM_REMOTE_LOOPBACK_ENABLE;
    }
	else
    {
		bufptr[j++] = EFM_REMOTE_LOOPBACK_DISABLE;
    }

	pefm->tx_count [EFM_LOOPBACK_PDU] += 1;
	return j;
}

/*efm errored symbol period  PDU subpackage 802.3ah 57.5.3.1*/
	int
efm_format_err_sym_period_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
    int j = 0;
	struct efm_link_event_info *local_link_info = NULL;

	EFM_LOG_DBG("EFM[TX]: Encoding Error symbol period PDU\n");

    if(NULL == pefm)
    {
		return -1;
    }

    if(NULL == bufptr)
    {
		return -1;
    }

	pefm->local_link_info.seq_no++;
	local_link_info = &pefm->local_link_info;

	j = 0;

	/* Subtype */
	bufptr[j++] = EFM_SUBTYPE;
	bufptr[j++] = (uint8_t)(pefm->efm_flags >> 8);
	bufptr[j++] = (uint8_t)(pefm->efm_flags & 0xff);
	bufptr[j++] = EFM_EVENT_NOTIFICATION_CODE;
	bufptr[j++] = (uint8_t)(local_link_info->seq_no >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->seq_no & 0xff);
	bufptr[j++] = EFM_ERRORED_SYMBOL_PERIOD_EVENT;                              //  event_type 1 bits
	bufptr[j++] = EFM_ERR_SYMBOL_PERIOD_TLV_LEN;                                     //  event_length    0x28 = 40
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_period_event_ts >> 8);       // 2 bits
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_period_event_ts & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 56);                     // 8
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 48);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 40);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 32);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_window & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 56);                 // 8
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 48);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 40);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 32);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbol_threshold & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 56);                           //   8
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 48);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 40);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 32);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 56);                               //8
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 48);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 40);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 32);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_symbols_total & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_sym_event_total >> 24);                       // 4
	bufptr[j++] = (uint8_t)(local_link_info->err_sym_event_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_sym_event_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_sym_event_total & 0xff);

	pefm->tx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] += 1;
	return j;
}

/*efm errored event  PDU subpackage 802.3ah 57.5.3.2*/
	int
efm_format_err_frame_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
    int j = 0;
	struct efm_link_event_info *local_link_info = NULL;

	EFM_LOG_DBG("EFM[TX]: Encoding Error Frame PDU\n");

    if(NULL == pefm)
    {
		return -1;
    }

    if(NULL == bufptr)
    {
		return -1;
    }

	pefm->local_link_info.seq_no++;
	local_link_info = &pefm->local_link_info;

	j = 0;

	bufptr[j++] = EFM_SUBTYPE;
	bufptr[j++] = (uint8_t)(pefm->efm_flags >> 8);
	bufptr[j++] = (uint8_t)(pefm->efm_flags & 0xff);
	bufptr[j++] = EFM_EVENT_NOTIFICATION_CODE;                          //0x01
	bufptr[j++] = (uint8_t)(local_link_info->seq_no >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->seq_no & 0xff);
	bufptr[j++] = EFM_ERRORED_FRAME_EVENT;                                    //0x02
	bufptr[j++] = EFM_ERR_FRAME_TLV_LEN;                                            //26 length = 0x1A
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_event_ts >> 8);               //
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_event_ts & 0xff);
	//err_frame_window should encode 100ms
	bufptr[j++] = (uint8_t)((local_link_info->err_frame_window * 10) >> 8);                           //1*10
	bufptr[j++] = (uint8_t)((local_link_info->err_frame_window * 10) & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_threshold >> 24);                            // 4
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_threshold >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_threshold >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_threshold & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frames >> 24);                                // 4
	bufptr[j++] = (uint8_t)(local_link_info->err_frames >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frames >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frames & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 56);                              // 8
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 48);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 40);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 32);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_total & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_event_total >> 24);                     //  4
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_event_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_event_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_event_total & 0xff);

	pefm->tx_count [EFM_ERR_FRAME_EVENT_PDU] += 1;
	return j;
}

/*efm errored frame period  PDU subpackage 802.3ah 57.5.3.3*/
	int
efm_format_err_frame_period_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
	int j;
	struct efm_link_event_info *local_link_info = NULL;

	EFM_LOG_DBG("EFM[TX]: Encoding Error Frame period PDU\n");

    if(NULL == pefm)
    {
        return -1;
    }

    if(NULL == bufptr)
    {
		return -1;
    }

	pefm->local_link_info.seq_no++;
	local_link_info = &pefm->local_link_info;

	j = 0;

	bufptr[j++] = EFM_SUBTYPE;
	bufptr[j++] = (uint8_t)(pefm->efm_flags >> 8);
	bufptr[j++] = (uint8_t)(pefm->efm_flags & 0xff);
	bufptr[j++] = EFM_EVENT_NOTIFICATION_CODE;
	bufptr[j++] = (uint8_t)(local_link_info->seq_no >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->seq_no & 0xff);
	bufptr[j++] = EFM_ERRORED_FRAME_PERIOD_EVENT;                                  // 1
	bufptr[j++] = EFM_ERR_FRAME_PERIOD_TLV_LEN;                                          //  1
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_event_ts >> 8);           //     2
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_event_ts & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_window >> 24);                 //  4
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_window >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_window >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_window & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_threshold >> 24);                 // 4
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_threshold >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_threshold >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_threshold & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_frames >> 24);                       //  4
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_frames >> 16);     
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_frames >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_frames & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 56);                        // 8
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 48);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 40);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 32);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_error_total & 0xff);                             
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_event_total >> 24);               //4
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_event_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_event_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_period_event_total & 0xff);

	pefm->tx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] += 1;
	return j;
}
/*efm errored frame seconds  PDU subpackage 802.3ah 57.5.3.4*/
	int
efm_format_err_frame_seconds_pdu (struct efm_if *pefm, register uint8_t *bufptr)
{
    int j = 0;
	struct efm_link_event_info *local_link_info = NULL;

	EFM_LOG_DBG("EFM[TX]: Encoding Errored Frame Seconds PDU\n");

    if(NULL == pefm)
    {
		return -1;
    }

    if(NULL == bufptr)
    {
		return -1;
    }

	pefm->local_link_info.seq_no++;
	local_link_info = &pefm->local_link_info;

	j = 0;

	bufptr[j++] = EFM_SUBTYPE;
	bufptr[j++] = (uint8_t)(pefm->efm_flags >> 8);
	bufptr[j++] = (uint8_t)(pefm->efm_flags & 0xff);
	bufptr[j++] = EFM_EVENT_NOTIFICATION_CODE;
	bufptr[j++] = (uint8_t)(local_link_info->seq_no >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->seq_no & 0xff);
	bufptr[j++] = EFM_ERRORED_FRAME_SECONDS_EVENT;
	bufptr[j++] = EFM_ERR_FRAME_SUMMARY_TLV_LEN;
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_sum_event_ts >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_sum_event_ts & 0xff);
	//make err_frame_sec_sum_window 100ms to seconds
	bufptr[j++] = (uint8_t)((local_link_info->err_frame_sec_sum_window * 10) >> 8);
	bufptr[j++] = (uint8_t)((local_link_info->err_frame_sec_sum_window * 10) & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_sum_threshold >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_sum_threshold & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_error >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_error & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_error_total >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_error_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_error_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_error_total & 0xff);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_event_total >> 24);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_event_total >> 16);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_event_total >> 8);
	bufptr[j++] = (uint8_t)(local_link_info->err_frame_sec_event_total & 0xff);

	pefm->tx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU] += 1;
	return j;
}

/*efm send OAMPDU*/
	int
efm_tx (struct efm_if *pefm, enum efm_pdu pdu_type)
{
    int ret = 0, len = 0;
	uint8_t *bufptr;
	register uint8_t *buf;

    if(NULL == pefm)
    {
		return EFM_ERR_IF_NOT_FOUND;
    }

	EFM_LOG_DBG("EFM[TX]: Transmitting EFM PDU");

	bufptr = XCALLOC (MTYPE_EFM_MSG_BUF, EFM_BUF_SIZE);
    if(NULL == bufptr)
	{
		zlog_err ("Could not allocate memory for EFM MSG BUF\n");
		return EFM_ERR_MEMORY;
	}

	/*initialise allocated memory*/  
	memset(bufptr, 0, EFM_BUF_SIZE);

	buf = bufptr;

	if (pdu_type != EFM_INFORMATION_PDU
			&& pefm->local_pdu != EFM_ANY)
	{
		XFREE (MTYPE_EFM_MSG_BUF, bufptr);
		return 0;
	}

	switch (pdu_type)
	{
		case EFM_INFORMATION_PDU:
			if ((len = efm_format_information_pdu (pefm, buf)) < 0)
			{
				XFREE (MTYPE_EFM_MSG_BUF, bufptr);
				return -1;
			}
			break;
		case EFM_LOOPBACK_PDU:
			if ((len = efm_format_loopback_pdu (pefm, buf)) < 0)
			{
				XFREE (MTYPE_EFM_MSG_BUF, bufptr);
				return -1;
			}
			break;
		case EFM_ERR_SYMBOL_PERIOD_EVENT_PDU:
			if ((len = efm_format_err_sym_period_pdu (pefm, buf)) < 0)
			{
				XFREE (MTYPE_EFM_MSG_BUF, bufptr);
				return -1;
			}
			break;
		case EFM_ERR_FRAME_EVENT_PDU:
			if ((len = efm_format_err_frame_pdu (pefm, buf)) < 0)
			{
				XFREE (MTYPE_EFM_MSG_BUF, bufptr);
				return -1;
			}
			break;
		case EFM_ERR_FRAME_PERIOD_EVENT_PDU:
			if ((len = efm_format_err_frame_period_pdu (pefm, buf)) < 0)
			{
				XFREE (MTYPE_EFM_MSG_BUF, bufptr);
				return -1;
			}
			break;
		case EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU:
			if ((len = efm_format_err_frame_seconds_pdu (pefm, buf)) < 0)
			{
				XFREE (MTYPE_EFM_MSG_BUF, bufptr);
				return -1;
			}
			break;
		default:
			XFREE (MTYPE_EFM_MSG_BUF, bufptr);
			break;

			return 0;
	}
	/* Send it. */
	ret = efm_send (pefm, bufptr, len<64 ? 64 : len);
   
	XFREE (MTYPE_EFM_MSG_BUF, bufptr);  

	pefm->valid_pdu_req = EFM_FALSE;

	return ret;
}


/* Parse a oampdu into a more palatable format. Returns ERROR
   if OAMPDU is invalid, RESULT_OK if not. */
	int
efm_parse_efmpdu (uint8_t *buf, const int len,
		struct efm_oam_pdu *efm_pdu_t, struct efm_if *pefm)
{
    int j = 0;
    s_int32_t length = 0;
    efm_agent_info * aefm = NULL;
    uint16_t msdh_length = 0;
    uint16_t msdh_frm_cmd = 0;
	int ret = 0;
	uint8_t * aefm_data = NULL;

	EFM_LOG_DBG("EFM[RX]: Decoding the EFMPDU\n");

	length = len;

    if((NULL == buf) || (NULL == efm_pdu_t) || (NULL == pefm))
	{
		return -1;
	}

	j = 0;

	if(buf[0] != EFM_SUBTYPE)       
    {
		return -1;
    }

	efm_pdu_t->subtype = buf[j++];
	efm_pdu_t->efm_flags = (uint16_t)(buf[j++] << 8);
	efm_pdu_t->efm_flags |= (uint16_t)(buf[j++]);

	/*dying gasp recv*/
	if (efm_pdu_t->efm_flags & 0x2)
    {
		return j;
    }

	efm_pdu_t->code = buf[j++];

    if(EFM_INFO_CODE == efm_pdu_t->code)
	{
		EFM_LOG_DBG("EFM[RX]: Received Information TLV\n");

		pefm->rx_count [EFM_INFORMATION_PDU] += 1;
		efm_pdu_t->info_pdu.info_type =  buf[j++];


		if( !(CHECK_FLAG((efm_pdu_t->efm_flags & 0x01), EFM_LINK_FAULT)) )
		{
			if (efm_pdu_t->info_pdu.info_type != EFM_LOCAL_INFORMATION_TLV)
            {
				return -1;
            }
		}

		efm_pdu_t->info_pdu.info_length = buf[j++];

		if( !(CHECK_FLAG((efm_pdu_t->efm_flags & 0x01), EFM_LINK_FAULT)) )
		{
			if (efm_pdu_t->info_pdu.info_length != EFM_LOCAL_TLV_LENGTH)
            {
				return -1;
            }
		}

		efm_pdu_t->info_pdu.oam_version = buf[j++];

		efm_pdu_t->info_pdu.revision = (uint16_t)(buf[j++] << 8);
		efm_pdu_t->info_pdu.revision |= (uint16_t)(buf[j++]);

		efm_pdu_t->info_pdu.state = buf[j++];
		efm_pdu_t->info_pdu.oam_config = buf[j++];

		efm_pdu_t->info_pdu.oam_pdu_config = (uint16_t)(buf[j++] << 8);
		efm_pdu_t->info_pdu.oam_pdu_config |= (uint16_t)(buf[j++]);

		memcpy (efm_pdu_t->info_pdu.oui, &buf[j], EFM_OUI_LENGTH);
		j += EFM_OUI_LENGTH;

		memcpy (efm_pdu_t->info_pdu.vend_spec_info, &buf[j], EFM_VSI_LENGTH);
		j += EFM_VSI_LENGTH;

		efm_pdu_t->info_pdu.rem_info_type =  buf[j++];

		if (efm_pdu_t->info_pdu.rem_info_type != EFM_REMOTE_INFORMATION_TLV)
        {
			return j;
        }

		efm_pdu_t->info_pdu.rem_info_length = buf[j++];

		if (efm_pdu_t->info_pdu.rem_info_length == 0)
        {
			return j;
        }

		efm_pdu_t->info_pdu.rem_oam_version = buf[j++];

		efm_pdu_t->info_pdu.rem_revision = (uint16_t)(buf[j++] << 8);
		efm_pdu_t->info_pdu.rem_revision |= (uint16_t)(buf[j++]);

		efm_pdu_t->info_pdu.rem_state = buf[j++];
		efm_pdu_t->info_pdu.rem_oam_config = buf[j++];

		efm_pdu_t->info_pdu.rem_oam_pdu_config = (uint16_t)(buf[j++] << 8);
		efm_pdu_t->info_pdu.rem_oam_pdu_config |= (uint16_t)(buf[j++]);

		memcpy (efm_pdu_t->info_pdu.rem_oui, &buf[j], EFM_OUI_LENGTH);
		j += EFM_OUI_LENGTH;
		memcpy (efm_pdu_t->info_pdu.rem_vend_spec_info, &buf[j], EFM_VSI_LENGTH);
		j += EFM_VSI_LENGTH;
	}
    else if(EFM_LOOPBACK_CONTROL == efm_pdu_t->code)
	{
		EFM_LOG_DBG("EFM[RX]: Received Loopback control TLV\n");

		pefm->rx_count [EFM_LOOPBACK_PDU] += 1;
		efm_pdu_t->lb_pdu.enable = buf[j++];
	}
    else if(EFM_EVENT_NOTIFICATION_CODE == efm_pdu_t->code)
    {
		EFM_LOG_DBG ("EFM[RX]: Received Event Notification PDU\n");

		length -= j;

		EFM_TLV_UINT16_DECODE(buf, efm_pdu_t->event_pdu.seq_no);

		while (length > 0)
		{
			EFM_TLV_UCHAR_DECODE(buf, efm_pdu_t->event_pdu.event_type);
			EFM_TLV_UCHAR_DECODE(buf, efm_pdu_t->event_pdu.event_length);

            if(EFM_ERRORED_SYMBOL_PERIOD_EVENT == efm_pdu_t->event_pdu.event_type)
			{
				EFM_LOG_DBG("EFM[RX]: Received Errored symbol period event\n");

				pefm->rx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] += 1;
				SET_FLAG (efm_pdu_t->event_pdu.pdu_list, EFM_SYM_PERIOD_EVENT_RCVD);

				EFM_TLV_UINT16_DECODE(buf, efm_pdu_t->sym_period_pdu.err_sym_period_event_ts);

				EFM_TLV_UINT64_DECODE(buf, efm_pdu_t->sym_period_pdu.err_sym_window);

				EFM_TLV_UINT64_DECODE(buf, efm_pdu_t->sym_period_pdu.err_sym_threshold);

				EFM_TLV_UINT64_DECODE(buf, efm_pdu_t->sym_period_pdu.err_symbols);

				EFM_TLV_UINT64_DECODE(buf, efm_pdu_t->sym_period_pdu.err_sym_total);

				EFM_TLV_UINT32_DECODE(buf, efm_pdu_t->sym_period_pdu.err_sym_event_total);
			}
            else if(EFM_ERRORED_FRAME_EVENT == efm_pdu_t->event_pdu.event_type)
			{
				EFM_LOG_DBG("EFM[RX]: Received Errored Frame event\n");

				pefm->rx_count [EFM_ERR_FRAME_EVENT_PDU] += 1;
				SET_FLAG (efm_pdu_t->event_pdu.pdu_list, EFM_FRAME_EVENT_RCVD);

				EFM_TLV_UINT16_DECODE(buf, efm_pdu_t->frame_pdu.err_frame_event_ts);

				EFM_TLV_UINT16_DECODE(buf, efm_pdu_t->frame_pdu.err_frame_window);
				//err_frame_window in pdu is 100ms, should make second
				efm_pdu_t->frame_pdu.err_frame_window = efm_pdu_t->frame_pdu.err_frame_window/10;

				EFM_TLV_UINT32_DECODE(buf, efm_pdu_t->frame_pdu.err_frame_thres);

				EFM_TLV_UINT32_DECODE(buf, efm_pdu_t->frame_pdu.err_frames);

				EFM_TLV_UINT64_DECODE(buf, efm_pdu_t->frame_pdu.err_frame_total);

				EFM_TLV_UINT32_DECODE(buf, efm_pdu_t->frame_pdu.err_frame_event_total);
			}
            else if(EFM_ERRORED_FRAME_PERIOD_EVENT == efm_pdu_t->event_pdu.event_type)
			{
				EFM_LOG_DBG("EFM[RX]: Received Errored Frame Period event\n");

				pefm->rx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] += 1;
				SET_FLAG (efm_pdu_t->event_pdu.pdu_list, EFM_FRAME_PERIOD_EVENT_RCVD);

				EFM_TLV_UINT16_DECODE(buf, 
						efm_pdu_t->frame_period_pdu.err_frame_period_event_ts);

				EFM_TLV_UINT32_DECODE(buf, 
						efm_pdu_t->frame_period_pdu.err_frame_period_window);

				EFM_TLV_UINT32_DECODE(buf, 
						efm_pdu_t->frame_period_pdu.err_frame_period_thres);

				EFM_TLV_UINT32_DECODE(buf, 
						efm_pdu_t->frame_period_pdu.err_frame_period_frames);

				EFM_TLV_UINT64_DECODE(buf,
						efm_pdu_t->frame_period_pdu.err_frame_period_error_total);

				EFM_TLV_UINT32_DECODE(buf, 
						efm_pdu_t->frame_period_pdu.err_frame_period_event_total);
			}
            else if(EFM_ERRORED_FRAME_SECONDS_EVENT == efm_pdu_t->event_pdu.event_type)
			{
				EFM_LOG_DBG("EFM[RX]: Received Errored Frame Seconds event\n");

				pefm->rx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU] += 1;
				SET_FLAG (efm_pdu_t->event_pdu.pdu_list, EFM_FRAME_SEC_SUM_EVENT_RCVD);

				EFM_TLV_UINT16_DECODE(buf, 
						efm_pdu_t->frame_sec_pdu.err_frame_sec_event_ts);

				EFM_TLV_UINT16_DECODE(buf, 
						efm_pdu_t->frame_sec_pdu.err_frame_sec_window);

				efm_pdu_t->frame_sec_pdu.err_frame_sec_window = 
					efm_pdu_t->frame_sec_pdu.err_frame_sec_window/10;

				EFM_TLV_UINT16_DECODE(buf, 
						efm_pdu_t->frame_sec_pdu.err_frame_sec_thres);

				EFM_TLV_UINT16_DECODE(buf, 
						efm_pdu_t->frame_sec_pdu.err_frame_sec_error);

				EFM_TLV_UINT32_DECODE(buf, 
						efm_pdu_t->frame_sec_pdu.err_frame_sec_error_total);

				EFM_TLV_UINT32_DECODE(buf, 
						efm_pdu_t->frame_sec_pdu.err_frame_sec_event_total);
			}
		}

	}
	else if(EFM_ORG_SPECIFIC  == efm_pdu_t->code)
	{
		pefm->rx_count [EFM_SPECIFIC_PDU] += 1;

		while(j++ < length)
		{

			/*recv is reply info*/
			if(!memcmp(&buf[j],msdh_SrcAdrr,4))
			{
			
				if(NULL == pefm->aefm)
				{
					zlog_err("%s[%d]:efm agent has NULL!",__FUNCTION__,__LINE__);
					return -1;
				}
		EFM_LOG_DBG("EFM[RX]: Received Org Specific PDU");

				aefm = pefm->aefm;
				aefm->efm_agent_link_flag = EFM_TRUE ;

				

				while(j++ < length)
				{
					if(0x69 == buf[j] && 0x11 == buf[j+1]&& 1 == buf[j+2])
					{
						j+=3;
						
						msdh_frm_cmd = (uint16_t)(buf[j++]<<8); /*1101 4040 1040 1840*/
				msdh_frm_cmd |= (uint16_t)buf[j++] ;
				
				msdh_length = length-j;
				
				/*get remote device info*/
				switch(msdh_frm_cmd)
				{
					
					case EFM_MSDH_DEVICE_GET_CMD:						
					aefm->efm_rx_DifSpecific_pdu[EFM_AGENT_DEVICE_INFO_PDU]++;
					ret = efm_parse_msdh_device_info(aefm,buf+j,msdh_length);      //get 11000A or 11000An or 1101n
					break;
				
					case EFM_MSDH_INFO_GET_CMD:				
					aefm->efm_rx_DifSpecific_pdu[EFM_AGENT_GET_INFO_PDU]++;
					ret =  efm_parse_msdh_getinfo(aefm,buf+j,msdh_length);
					break;
					 
					case EFM_MSDH_INFO_SET_CMD:
					aefm->efm_rx_DifSpecific_pdu[EFM_AGENT_SET_INFO_PDU]++;
					ret =  efm_parse_msdh_setinfo(aefm,buf+j,msdh_length);
					break;
					
					case EFM_MSDH_INFO_SET_GET_CMD:			
					aefm->efm_rx_DifSpecific_pdu[EFM_AGENT_SET_GET_INFO_PDU]++;
					ret =  efm_parse_msdh_SetGetinfo(pefm,buf+j,msdh_length);
					
					break;
					
					default:
						return -1;
						
				}
				if(NULL == aefm || NULL == aefm->aefm_RsetGetInfo || ret < 0)
						{
					return ret;
				}

				aefm_data = aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data;
				if(aefm_data[efm_remote_set_6hpmcrtMacAddr] || 
					aefm_data[efm_remote_set_5hpmcrtMacAddr] || 
					aefm_data[efm_remote_set_4hpmcrtMacAddr] ||
					aefm_data[efm_remote_set_3hpmcrtMacAddr] ||
					aefm_data[efm_remote_set_2hpmcrtMacAddr] ||
					aefm_data[efm_remote_set_1hpmcrtMacAddr])
				{	
					

					/*send info to ospf*/
					if(msdh_frm_cmd ==  EFM_MSDH_INFO_SET_GET_CMD && !aefm->efm_agent_last_link_flag)
					{			
						/*first has already bulid fully connection with remote*/
						efm_agent_u0_info_send(pefm->if_index,pefm->aefm,IPC_OPCODE_ADD);
						aefm->efm_agent_last_link_flag = EFM_TRUE;
					}
					
					/*update lldp*/ 
					lldp_pkt_rcv_efm_agent(pefm->if_index, pefm->aefm);

						}
						return EFM_SUCCESS;
					}
					
				}

			}



		}
	
	}
	else
	{
		pefm->rx_count [EFM_PDU_INVALID] += 1;
	}

	return j;
}

/*efm record receive's pdu*/
	void
efm_record_pdu (struct efm_if *pefm, struct efm_oam_pdu *efm_pdu_t, struct pkt_buffer *ppkt)
{
	int event_occurred = 0;
	uint8_t set_ignore = EFM_FALSE;
	enum efm_link_event_type event_type = EFM_UNKNOWN_EVENT; 
	uint8_t oui[3] = {0x01, 0x80, 0xc2};
	uint8_t loopback_act = 0;
	efm_agent_info * aefm = NULL;
	EFM_LOG_DBG("EFM[RX]: Parsing the Received EFM PDU");

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pefm->if_index;
    if(NULL == pefm || NULL == efm_pdu_t)
    {
		return;
    }

	pefm->efm_remote_state_valid = EFM_TRUE;
	if(pefm->aefm !=NULL)
	{
		aefm = pefm->aefm;
	}

	memcpy (pefm->rem_info.mac_addr, ppkt->cb.ethcb.smac, 6);
	memcpy (oui, ppkt->cb.ethcb.smac, 3);

	/* check for an error condition as per table 57.3 of Std 802.3ah.
	   If both of the following bits are set, don't set the newly
	   received values */
	if ( (CHECK_FLAG (efm_pdu_t->efm_flags, EFM_LOCAL_EVALUATING)) &&
			(CHECK_FLAG (efm_pdu_t->efm_flags, EFM_LOCAL_STABLE)) )
	{
		set_ignore = EFM_TRUE;
	}

    if(EFM_FALSE == set_ignore)
	{
		/* only LOCAL_STABLE & EFM_LOCAL_EVALUATING bits need to be considered 
		 * here. REMOTE_STABLE & REMOTE_EVALUATING bits need not be considered. 
		 * It is because REMOTE bits in the incoming PDU means the info 
		 * pertaining to this OAM client which processes current PDU 
		 * i.e. Local end.*/

		if (CHECK_FLAG (efm_pdu_t->efm_flags, EFM_LOCAL_STABLE))
		{

			pefm->efm_remote_stable = EFM_TRUE;
			SET_FLAG (pefm->efm_flags, EFM_REMOTE_STABLE);
			UNSET_FLAG (pefm->efm_flags, EFM_REMOTE_EVALUATING);
		}
		else
        {
			pefm->efm_remote_stable = EFM_FALSE;
        }

		if (CHECK_FLAG (efm_pdu_t->efm_flags, EFM_LOCAL_EVALUATING))
        {
			SET_FLAG (pefm->efm_flags, EFM_REMOTE_EVALUATING);
        }
	}

	/*
	   when receive efm critical/dying gasp/link-fault pdu
	   need check efm is enable disable interface config
	   if config interface down,should shutdown interface
	   Not support now!*/


	/*Remote Event has been detected. This has to be logged.*/  
	if (CHECK_FLAG (efm_pdu_t->efm_flags,EFM_CRITICAL_EVENT)  &&
			!(CHECK_FLAG(pefm->efm_flags_last, EFM_CRITICAL_EVENT)))
	{
		event_type = EFM_CRITICAL_LINK_EVENT;  
		event_occurred = 1;
	}
	else if (CHECK_FLAG (efm_pdu_t->efm_flags, EFM_DYING_GASP) &&
			!(CHECK_FLAG(pefm->efm_flags_last, EFM_DYING_GASP)))
	{
		event_type = EFM_DYING_GASP_EVENT; 
		event_occurred = 1;
	}
	else if (CHECK_FLAG (efm_pdu_t->efm_flags, EFM_LINK_FAULT) &&
			!(CHECK_FLAG(pefm->efm_flags_last, EFM_LINK_FAULT)))
	{
		event_type = EFM_LINK_FAULT_EVENT;
		event_occurred = 1;
	}

	if(event_occurred)
	{
		efm_link_event_log_add(pefm, NULL, event_type, EFM_EVENT_REMOTE, oui);

        if(EFM_LINK_FAULT_EVENT == event_type)
        {
			//send efm link-fault alarm rise
			
			//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_LINK_FAULT ,GPN_SOCK_MSG_OPT_RISE);

			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_LINK_FAULT, GPN_SOCK_MSG_OPT_RISE);
			//send l3-rem-rx-los alarm rise
			
		}

        if(EFM_CRITICAL_LINK_EVENT == event_type)
		{

			//send efm critical alarm rise
			
			//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_CRITICAL ,GPN_SOCK_MSG_OPT_RISE);
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_CRITICAL, GPN_SOCK_MSG_OPT_RISE);
		}

        if(EFM_DYING_GASP_EVENT == event_type)
		{      
			//send efm dying-gasp alarm rise
			
			//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_DYINGGASP ,GPN_SOCK_MSG_OPT_RISE);
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_DYINGGASP, GPN_SOCK_MSG_OPT_RISE);
			//send efm l3-rem-pd alarm rise
		}
		efm_agent_trap_ThreeAlarm( aefm,event_type,EFM_TRUE,pefm->if_index);
	}

	if (!(CHECK_FLAG(efm_pdu_t->efm_flags, EFM_LINK_FAULT)) &&
			(CHECK_FLAG(pefm->efm_flags_last, EFM_LINK_FAULT)))
	{
		//send efm link-fault alarm clean
		
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_LINK_FAULT ,GPN_SOCK_MSG_OPT_CLEAN);
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_LINK_FAULT, GPN_SOCK_MSG_OPT_CLEAN);
		//send efm l3-rem-rx-los alarm clean
		/*trap cancel*/
		efm_agent_trap_ThreeAlarm( aefm,EFM_LINK_FAULT_EVENT,EFM_FALSE,pefm->if_index);
	}

	if (!(CHECK_FLAG(efm_pdu_t->efm_flags, EFM_CRITICAL_EVENT)) &&
			(CHECK_FLAG(pefm->efm_flags_last, EFM_CRITICAL_EVENT)))
	{
		//send efm critical alarm clean
		
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_CRITICAL ,GPN_SOCK_MSG_OPT_CLEAN);
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_CRITICAL, GPN_SOCK_MSG_OPT_CLEAN);
		/*trap cancel*/
		if(event_type != EFM_DYING_GASP_EVENT  && !event_occurred)
		{
			efm_agent_trap_ThreeAlarm( aefm,EFM_CRITICAL_LINK_EVENT,EFM_FALSE,pefm->if_index);
		}
	}
      
	if (!(CHECK_FLAG(efm_pdu_t->efm_flags, EFM_DYING_GASP)) &&
			(CHECK_FLAG(pefm->efm_flags_last, EFM_DYING_GASP)))
	{
		//send efm dying-gasp alarm clean
		
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_DYINGGASP ,GPN_SOCK_MSG_OPT_CLEAN);
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_DYINGGASP, GPN_SOCK_MSG_OPT_CLEAN);
		//send efm l3-rem-pd alarm clean
		/*trap cancel*/
		efm_agent_trap_ThreeAlarm( aefm,EFM_DYING_GASP_EVENT,EFM_FALSE,pefm->if_index);
	}

	pefm->efm_flags_last = efm_pdu_t->efm_flags;

    if(EFM_INFO_CODE == efm_pdu_t->code)
	{
		/* if ((efm_oam_pdu->info_pdu.oam_config & EFM_CFG_COMPARE_MASK)
		   == (efm_oam->local_info.oam_config & EFM_CFG_COMPARE_MASK)) */
		if ((efm_pdu_t->info_pdu.oam_config & 0x01)
				|| (pefm->local_info.oam_config & 0x01))
        {
			pefm->efm_local_satisfied = EFM_TRUE;
        }
		else
        {
			pefm->efm_local_satisfied = EFM_FALSE;
        }

		pefm->rem_info.oam_version = efm_pdu_t->info_pdu.oam_version;

		/* if reserved bits are set, ignore them*/
		pefm->rem_info.oam_config = (efm_pdu_t->info_pdu.oam_config & EFM_CONFIG_FIELD_MASK);

		pefm->rem_info.revision = efm_pdu_t->info_pdu.revision;

		/* if reserved bits are set, ignore them*/
		pefm->rem_info.state = (efm_pdu_t->info_pdu.state & EFM_STATE_FIELD_MASK);
		/*if local is remote_loopback  remote has disable(before no disbale remote_loopback) and then enable*/
				if((EFM_REM_LB == pefm->local_info.loopback) && (EFM_LOCAL_LB ==  pefm->rem_info.loopback))
				{	   
					/*efm_rem_par_action_t = EFM_PAR_FWD  && pefm->efm_rem_mux_action_t =  EFM_MUX_FWD*/
					if(!pefm->rem_info.state)	   
					{
						if(pefm->efm_local_par_action_t != EFM_PAR_FWD \
						|| pefm->efm_local_mux_action_t != EFM_MUX_FWD)
						{
							 if(EFM_REM_LB == pefm->local_info.loopback)
							{
									//if local mac-swap so cancel mac-swap	  else should cancel local loopback
										  if(pefm->efm_mac_swap_flag)
										 {				
										 loopback_act = EFM_NO_MAC_SWAP;
							#if 0
										   if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
								IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
										 {
												 zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
														return ;
		
								}
							#endif
							if(ipc_send_msg_n2( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
										IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
							{
								zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
								return ;

							}
								pefm->efm_mac_swap_flag = EFM_FALSE;
							}
							else 
							{
								loopback_act = EFM_DISABLE_LB;
							#if 0
										if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
								IPC_TYPE_EFM,	REMOTE_LOOPBACK , IPC_OPCODE_CHANGE, pefm->if_index ))
								{								  
											zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel loopback failure!\n", __FILE__, __LINE__, __func__ );
									return ;
								}
							#endif
							if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
										IPC_TYPE_EFM,	REMOTE_LOOPBACK , IPC_OPCODE_CHANGE, pefm->if_index ))
							{								  
								zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel loopback failure!\n", __FILE__, __LINE__, __func__ );
								return ;
							}
							pefm->efm_loopback_flag = EFM_FALSE;
						}


					}

					pefm->efm_rem_loopback = EFM_FALSE;
					pefm->local_info.loopback = EFM_NONE_LB;
					pefm->rem_info.loopback = EFM_NONE_LB;
					//should set fwd state to hsl, cancel loopback state
					pefm->efm_local_par_action_t = EFM_PAR_FWD;
					pefm->efm_local_mux_action_t = EFM_MUX_FWD;
					pefm->rem_info.loopback = EFM_NONE_LB;			

				}

			}
		}

		/* if reserved bits are set, ignore them*/
		pefm->rem_info.oam_pdu_config = (efm_pdu_t->info_pdu.oam_pdu_config
				& EFM_PDU_CONFIG_FIELD_MASK);

		//memcpy (pefm->rem_info.oui, ppkt->cb.ethcb.smac, EFM_OUI_LENGTH);
		memcpy (pefm->rem_info.oui, efm_pdu_t->info_pdu.oui, EFM_OUI_LENGTH);

		memcpy (pefm->rem_info.vend_spec_info,
				efm_pdu_t->info_pdu.vend_spec_info, EFM_VSI_LENGTH);
	}
    else if(EFM_LOOPBACK_CONTROL == efm_pdu_t->code)
	{    
                   if(EFM_REMOTE_LOOPBACK_ENABLE == efm_pdu_t->lb_pdu.enable)
		{
			pefm->rem_info.loopback = EFM_LOCAL_LB;

		}
		else
		{
			pefm->rem_info.loopback = EFM_NONE_LB;

		}

	}
    else if(EFM_EVENT_NOTIFICATION_CODE == efm_pdu_t->code)
	{	
		SET_FLAG (pefm->efm_excess_errrors, EFM_OAM_REMOTE_EXCESS_ERRORS);

		if (CHECK_FLAG (efm_pdu_t->event_pdu.pdu_list, EFM_SYM_PERIOD_EVENT_RCVD))
		{
		
			event_type = EFM_ERR_SYMBOL_PERIOD_EVENT;    
			
			pefm->rem_link_info.err_symbol_period_event_ts =
				efm_pdu_t->sym_period_pdu.err_sym_period_event_ts;
			memcpy (&pefm->rem_link_info.err_symbol_window,
					&efm_pdu_t->sym_period_pdu.err_sym_window,
					sizeof (uint64_t));

			memcpy (&pefm->rem_link_info.err_symbol_threshold,
					&efm_pdu_t->sym_period_pdu.err_sym_threshold,
					sizeof (uint64_t));

			memcpy (&pefm->rem_link_info.err_symbols,
					&efm_pdu_t->sym_period_pdu.err_symbols,
					sizeof (uint64_t));
			memcpy (&pefm->rem_link_info.err_symbols_total,
					&efm_pdu_t->sym_period_pdu.err_sym_total,
					sizeof (uint64_t));
			pefm->rem_link_info.err_sym_event_total =
				efm_pdu_t->sym_period_pdu.err_sym_event_total;
		
			//send efm rem-symb-err event to alarm
			efm_send_link_event_to_alarm(pefm,EFM_EVENT_REMOTE,event_type);  
			/*Remote Event has been detected. This has to be logged.*/
			efm_link_event_log_add(pefm, &pefm->rem_link_info,event_type, EFM_EVENT_REMOTE, oui);
		}

		if (CHECK_FLAG (efm_pdu_t->event_pdu.pdu_list,
					EFM_FRAME_EVENT_RCVD))
		{
			event_type = EFM_ERR_FRAME_EVENT;
			
			pefm->rem_link_info.err_frame_event_ts =
				efm_pdu_t->frame_pdu.err_frame_event_ts;
			pefm->rem_link_info.err_frame_window =
				efm_pdu_t->frame_pdu.err_frame_window;

			pefm->rem_link_info.err_frame_threshold =
				efm_pdu_t->frame_pdu.err_frame_thres;

			pefm->rem_link_info.err_frames =
				efm_pdu_t->frame_pdu.err_frames;
			memcpy (&pefm->rem_link_info.err_frame_total,
					&efm_pdu_t->frame_pdu.err_frame_total,
					sizeof (uint64_t));
			pefm->rem_link_info.err_frame_event_total =
				efm_pdu_t->frame_pdu.err_frame_event_total;

			//send efm rem-frame-err event to alarm
			efm_send_link_event_to_alarm(pefm,EFM_EVENT_REMOTE,event_type);  
			/*Remote Event has been detected. This has to be logged.*/
			efm_link_event_log_add(pefm, &pefm->rem_link_info,event_type, EFM_EVENT_REMOTE, oui);
		}

		if (CHECK_FLAG (efm_pdu_t->event_pdu.pdu_list,
					EFM_FRAME_PERIOD_EVENT_RCVD))
		{
			event_type = EFM_ERR_FRAME_PERIOD_EVENT;
			pefm->rem_link_info.err_frame_period_event_ts =
				efm_pdu_t->frame_period_pdu.err_frame_period_event_ts;
			pefm->rem_link_info.err_frame_period_window =
				efm_pdu_t->frame_period_pdu.err_frame_period_window;

			pefm->rem_link_info.err_frame_period_threshold =
				efm_pdu_t->frame_period_pdu.err_frame_period_thres;

			pefm->rem_link_info.err_frame_period_frames =
				efm_pdu_t->frame_period_pdu.err_frame_period_frames;
			memcpy (&pefm->rem_link_info.err_frame_period_error_total,
					&efm_pdu_t->frame_period_pdu.err_frame_period_error_total,
					sizeof (uint64_t));
			pefm->rem_link_info.err_frame_period_event_total =
				efm_pdu_t->frame_period_pdu.err_frame_period_event_total;

			//send efm rem-frame-period-err event to alarm
			efm_send_link_event_to_alarm(pefm,EFM_EVENT_REMOTE,event_type);  
			/*Remote Event has been detected. This has to be logged.*/
			efm_link_event_log_add(pefm, &pefm->rem_link_info,event_type, EFM_EVENT_REMOTE, oui);
		}

		if (CHECK_FLAG (efm_pdu_t->event_pdu.pdu_list,
					EFM_FRAME_SEC_SUM_EVENT_RCVD))
		{ 
			event_type = EFM_ERR_FRAME_SECONDS_SUM_EVENT;
			pefm->rem_link_info.err_frame_sec_sum_event_ts =
				efm_pdu_t->frame_sec_pdu.err_frame_sec_event_ts;
			pefm->rem_link_info.err_frame_sec_sum_window =
				efm_pdu_t->frame_sec_pdu.err_frame_sec_window;

			pefm->rem_link_info.err_frame_sec_sum_threshold =
				efm_pdu_t->frame_sec_pdu.err_frame_sec_thres;

			pefm->rem_link_info.err_frame_sec_error =
				efm_pdu_t->frame_sec_pdu.err_frame_sec_error;

			pefm->rem_link_info.err_frame_sec_error_total = 
				efm_pdu_t->frame_sec_pdu.err_frame_sec_error_total;

			pefm->rem_link_info.err_frame_sec_event_total =
				efm_pdu_t->frame_sec_pdu.err_frame_sec_event_total;

			//send efm rem-frame-second-err event to alarm
			efm_send_link_event_to_alarm(pefm,EFM_EVENT_REMOTE,event_type);  
			/*Remote Event has been detected. This has to be logged.*/
			efm_link_event_log_add(pefm, &pefm->rem_link_info,event_type, EFM_EVENT_REMOTE, oui);
		}

	}

	return;
}


int efm_recv(struct pkt_buffer *ppkt)
{
	struct l2if *pif = NULL;
	
              pif = l2if_lookup(ppkt->in_port);
	if (!pif || !ppkt || !pif->pefm || !ppkt->data)
    {
		return -1;
    }

	EFM_LOG_DBG("EFM[RX]: Received EFM PDU. data_len = %d\n", ppkt->data_len);

	memset (&efmpdu, 0, sizeof (struct efm_oam_pdu));

    if(0 == memcmp(pif->mac, ppkt->cb.ethcb.smac, 6))
    {
		return -1;
    }

	/* Decode the packet and mark the appropriate bits in the 
	 * link structure */
	if (efm_parse_efmpdu ((uint8_t *)ppkt->data, ppkt->data_len, &efmpdu, pif->pefm) < EFM_SUCCESS)
	{ 
		return -1;
	}

	efm_record_pdu (pif->pefm, &efmpdu, ppkt);

	/*not deal with specific PDU*/
	if(EFM_ORG_SPECIFIC  == efmpdu.code)
	{
		return EFM_SUCCESS;
	}

    if(EFM_LOOPBACK_CONTROL == efmpdu.code
			|| pif->pefm->local_info.loopback != EFM_NONE_LB
			|| pif->pefm->rem_info.loopback != EFM_NONE_LB
			|| CHECK_FLAG(pif->pefm->local_info.oam_config, EFM_CFG_REM_LB_SUPPORT))
	{
		efm_process_rem_loopback (pif->pefm);
	}

	EFM_TIMER_OFF (pif->pefm->efm_link_timer);
	EFM_TIMER_ON_UNLOOP(pif->pefm->efm_link_timer, efm_link_timer_expiry,
			pif->pefm, pif->pefm->efm_link_time);

	efm_run_state_machine (pif->pefm);

	return EFM_SUCCESS;
}


int efm_send(struct efm_if *pefm, uint8_t *bufptr, int len)
{
	union pkt_control pkt_ctrl;
	sint32 ret = ERRNO_FAIL;

	if (!pefm || !pefm->if_index)
    {
		return EFM_ERR_IF_NOT_FOUND;
    }

	/*encode pkt header*/
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	memcpy(pkt_ctrl.ethcb.dmac, efm_mac, 6);
	memcpy(pkt_ctrl.ethcb.smac,l2_mac,6);
	pkt_ctrl.ethcb.ethtype = EFM_TYPE;
	pkt_ctrl.ethcb.sub_ethtype = EFM_SUBTYPE;
	pkt_ctrl.ethcb.smac_valid = 1;
	pkt_ctrl.ethcb.is_changed = 1;
	pkt_ctrl.ethcb.ifindex = pefm->if_index;
	pkt_ctrl.ethcb.cos = 6;

	/*send pkt*/
	ret = pkt_send(PKT_TYPE_ETH, &pkt_ctrl, (void *)bufptr, len);
	if (ret != 0)
	{
		zlog_err("%s[%d]:leave %s:pkt_send error\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
        
	return ERRNO_SUCCESS;
}





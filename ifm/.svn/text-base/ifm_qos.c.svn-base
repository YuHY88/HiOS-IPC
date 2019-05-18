#include <lib/types.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>
#include <lib/module_id.h>
#include <qos/qos_mapping.h>
#include <qos/qos_if.h>
#include <ifm/ifm.h>
#include "command.h"
#include "memory.h"
#include "memtypes.h"
#include "vty.h"
#include "hash1.h"
#include "ifm_common.h"
#include "ifm_qos.h"
#include "ifm_errno.h"
#include <string.h>
#include <stdlib.h>
#include <lib/log.h>


static int ifm_qos_domain_type_check(uint32_t ifindex, uint8_t mode, enum QOS_TYPE type);
static int ifm_qos_phb_type_check(uint32_t ifindex, uint8_t mode, enum QOS_TYPE type);


static int ifm_qos_domain_type_check(uint32_t ifindex, uint8_t mode, enum QOS_TYPE type)
{
	if(IFM_TYPE_IS_VLANIF(ifindex))
	{
		if((QOS_TYPE_TOS != type) &&
            (QOS_TYPE_DSCP != type))
            return IFM_ERR_WRONG_QOS_TYPE;
	}
    else if(IFM_TYPE_IS_PHYSICAL(ifindex))
    {
        if ((IFNET_MODE_SWITCH == mode) ||
        	(IFNET_MODE_L2 == mode))
        {
            if(QOS_TYPE_EXP == type)
                return IFM_ERR_WRONG_QOS_TYPE;
        }
        else if(IFNET_MODE_L3 == mode)
        {
            if((QOS_TYPE_TOS != type) &&
                (QOS_TYPE_DSCP != type))
                return IFM_ERR_WRONG_QOS_TYPE;
        }
    }
    else if(IFM_TYPE_IS_SUBPORT(ifindex))
    {
        if(IFNET_MODE_L3 == mode)
        {
            if((QOS_TYPE_TOS != type) &&
                (QOS_TYPE_DSCP != type))
                return IFM_ERR_WRONG_QOS_TYPE;
        }
        else if(IFNET_MODE_L2 == mode)
        {
            if(QOS_TYPE_EXP == type)
                return IFM_ERR_WRONG_QOS_TYPE;
        }
    }

	return ERRNO_SUCCESS;
}


static int ifm_qos_phb_type_check(uint32_t ifindex, uint8_t mode, enum QOS_TYPE type)
{
	if(IFM_TYPE_IS_VLANIF(ifindex))
	{
		if(QOS_TYPE_EXP == type)
            return IFM_ERR_WRONG_QOS_TYPE;
	}
	else if(IFM_TYPE_IS_PHYSICAL(ifindex))
    {
        if(IFNET_MODE_SWITCH == mode)
        {
            if(QOS_TYPE_EXP == type)
                return IFM_ERR_WRONG_QOS_TYPE;
        }
        else if(IFNET_MODE_L2 == mode)
        {
        	if(QOS_TYPE_COS != type)
            	return IFM_ERR_WRONG_QOS_TYPE;
        }
        else if(IFNET_MODE_L3 == mode)
        {
        	if((QOS_TYPE_TOS != type) &&
                (QOS_TYPE_DSCP != type))
        		return IFM_ERR_WRONG_QOS_TYPE;
        }
    }
    else if(IFM_TYPE_IS_SUBPORT(ifindex))
    {
        if(IFNET_MODE_L3 == mode)
        {
            if(QOS_TYPE_EXP == type)
                return IFM_ERR_WRONG_QOS_TYPE;
        }
        else if(IFNET_MODE_L2 == mode)
        {
        	if(QOS_TYPE_COS != type)
                return IFM_ERR_WRONG_QOS_TYPE;
        }
    }

	return ERRNO_SUCCESS;
}


int ifm_qos_set_domain(struct ifm_entry *pifm, struct qos_entry *pqos)
{
    int 		      ret = 0;
    int      		 *exist = NULL;
    struct qos_entry *pold = NULL;

    /* 检查配置是否与原配置相同 */
    pold = &(pifm->ifm.qos);
    if(pold->domain_id == pqos->domain_id &&
        pold->domain_type == pqos->domain_type)
    {
        return IFM_SUCCESS;
    }

    /* 检查配置的domain参数是否合法 */
    ret = ifm_qos_domain_type_check(pifm->ifm.ifindex, pifm->ifm.mode, pqos->domain_type);
    if(ret)
    {
    	zlog_err ("%s[%d] Unsupportted qos mapping type!\n", __FUNCTION__, __LINE__);
    	return ret;
    }

	exist = qos_domain_exist_get(pqos->domain_id, pqos->domain_type, MODULE_ID_IFM);
	if(NULL == exist)
	{
		zlog_err ("%s[%d] Get qos domain info timeout!\n", __FUNCTION__, __LINE__);
		return ERRNO_IPC;
	}

	if(0 == *exist)
	{
		zlog_err("%s[%d] The appointed qos domain is not exist!\n", __FUNCTION__, __LINE__);
		return ERRNO_NOT_FOUND;
	}
    /*ret = ipc_send_hal(pqos, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM,
                        IPC_TYPE_QOSIF, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);*/
  //ret = ifm_ipc_send(pqos, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM, 
  //   		IPC_TYPE_QOSIF, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);
    ret = ipc_send_msg_n2(pqos, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);

    if(ret)
    {
        zlog_err ( "%s[%d] ipc_send_hal errocode=%d\n", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    if(pold->domain_type != QOS_TYPE_INVALID)
    {
    	ret = qos_domain_ref_operate(pold->domain_id, pold->domain_type, MODULE_ID_IFM, REF_OPER_DECREASE);
	    if(ret)
	    {
	    	zlog_err ( "%s[%d] qos_domain_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
	        return ret;
	    }
    }

    ret = qos_domain_ref_operate(pqos->domain_id, pqos->domain_type, MODULE_ID_IFM, REF_OPER_INCREASE);
    if(ret)
    {
    	zlog_err ( "%s[%d] qos_domain_ref_operate increace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /* update qos_domain info */
    pifm->ifm.qos.domain_type = pqos->domain_type;
    pifm->ifm.qos.domain_id   = pqos->domain_id;

    return ret;
}


int ifm_qos_set_phb(struct ifm_entry *pifm, struct qos_entry *pqos)
{
    int 			  ret = 0;
    int      		 *exist = NULL;
    struct qos_entry *pold = NULL;

    /* 检查配置与原配置是否相同 */
    pold = &(pifm->ifm.qos);
    if(pold->phb_enable == pqos->phb_enable &&
        pold->phb_type == pqos->phb_type  &&
        pold->phb_id == pqos->phb_id)
    {
        return IFM_SUCCESS;
    }

	/* 检查配置的phb参数是否合法 */
    ret = ifm_qos_phb_type_check(pifm->ifm.ifindex, pifm->ifm.mode, pqos->phb_type);
    if(ret)
    {
    	zlog_err ("%s[%d] Unsupportted qos mapping type!\n", __FUNCTION__, __LINE__);
    	return ret;
    }

	exist = qos_phb_exist_get(pqos->phb_id, pqos->phb_type, MODULE_ID_IFM);
    if(NULL == exist)
    {
    	zlog_err ("%s[%d] Get qos phb info timeout!\n", __FUNCTION__, __LINE__);
		return ERRNO_IPC;
    }

    if(0 == *exist)
    {
    	zlog_err("%s[%d] The appointed qos phb is not exist!\n", __FUNCTION__, __LINE__);
		return ERRNO_NOT_FOUND;
    }

    /*ret = ipc_send_hal(pqos, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM,
                                    IPC_TYPE_QOSIF, QOS_INFO_PHB, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);*/
    //ret = ifm_ipc_send(pqos, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM, 
    //  		IPC_TYPE_QOSIF, QOS_INFO_PHB, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);              
    ret = ipc_send_msg_n2(pqos, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, QOS_INFO_PHB, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);

    if(ret)
    {
        zlog_err ( "%s[%d] ipc_send_hal errocode=%d\n", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    if(pold->phb_enable)
    {
    	ret = qos_phb_ref_operate(pold->phb_id, pold->phb_type, MODULE_ID_IFM, REF_OPER_DECREASE);
	    if(ret)
	    {
	    	zlog_err ( "%s[%d] qos_phb_ref_operate decreace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
	        return ret;
	    }
    }

    ret = qos_phb_ref_operate(pqos->phb_id, pqos->phb_type, MODULE_ID_IFM, REF_OPER_INCREASE);
    if(ret)
    {
    	zlog_err ( "%s[%d] qos_phb_ref_operate increace failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /* update qos_phb info */
    pifm->ifm.qos.phb_enable = pqos->phb_enable;
    pifm->ifm.qos.phb_id     = pqos->phb_id;
    pifm->ifm.qos.phb_type   = pqos->phb_type;

    return ret;
}


int ifm_qos_set_untag_pri(struct ifm_entry *pifm, uint8_t untag_pri)
{
    int ret = 0;
    struct qos_entry pentry;

	if(IFM_IS_SUBPORT(pifm->ifm.ifindex))
	{
		if(IFNET_MODE_L2 != pifm->ifm.mode)
        {
            return IFM_ERR_WRONG_QOS_TYPE;
        }
	}

	pentry.untag_pri = untag_pri;
    /*ret = ipc_send_hal(&pentry, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM,
                                    IPC_TYPE_QOSIF, QOS_INFO_UTAG_PRI, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);*/
	//ret = ifm_ipc_send(&pentry, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM, 
    //  		IPC_TYPE_QOSIF, QOS_INFO_UTAG_PRI, IPC_OPCODE_UPDATE, pifm->ifm.ifindex); 
    ret = ipc_send_msg_n2(&pentry, sizeof(struct qos_entry), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, QOS_INFO_UTAG_PRI, IPC_OPCODE_UPDATE, pifm->ifm.ifindex);
    if(ret)
    {
        zlog_err ( "%s[%d] ipc_send_hal errocode=%d\n", __FUNCTION__, __LINE__, ret );
        return ERRNO_IPC;
    }

    /* update qos_untag_pri info */
    pifm->ifm.qos.untag_pri = untag_pri;

    return ret;
}


int ifm_qos_set_info(uint32_t ifindex, void *pdata, enum QOS_INFO type)
{
    int ret = 0;
    struct ifm_entry *pifm = NULL;

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        zlog_err ( "%s[%d] ifm_lookup ifindex=0x%0x erro\n", __FUNCTION__, __LINE__, ifindex );
        return IFM_ERR_NOT_FOUND;
    }

    if ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
    {
        zlog_err ( "%s[%d] Failed to execute command, due to the interface had binded trunk\n",
                   __FUNCTION__, __LINE__ );
        return IFM_ERR_BINDED_TRUNK_ERRO;
    }

    if(IFM_TYPE_IS_METHERNET(ifindex)&&(IFM_SLOT_ID_GET ( ifindex ) == 0))
	{
		zlog_err( "%s[%d] Error:not support config ethernet 1/0/1,please check out!\n", __FUNCTION__, __LINE__ );
		return IFM_ERR_PARAM;
	}

    switch(type)
    {
        case QOS_INFO_DOMAIN:
            ret = ifm_qos_set_domain(pifm, (struct qos_entry *)pdata);
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update qos entry erro ifindex=0x%0x type=QOS_INFO_DOMAIN\n", __FUNCTION__, __LINE__, ifindex );
                return ret;
            }
            break;
        case QOS_INFO_PHB:
            ret = ifm_qos_set_phb(pifm, (struct qos_entry *)pdata);
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update qos entry erro ifindex=0x%0x type=QOS_INFO_PHB\n", __FUNCTION__, __LINE__, ifindex );
                return ret;
            }
            break;
        case QOS_INFO_UTAG_PRI:
            ret = ifm_qos_set_untag_pri(pifm, *(uint8_t *)pdata);
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] update qos entry erro ifindex=0x%0x type=QOS_INFO_UTAG_PRI\n", __FUNCTION__, __LINE__, ifindex );
                return ret;
            }
            break;
        default:
            zlog_err ( "%s[%d] ifm_qos_set_info ifindex=0x%0x miss match type=%d\n", __FUNCTION__, __LINE__, ifindex, type );
            return IFM_ERR_MISMATCH;
    }

    return ret;
}


int ifm_qos_unset_domain(struct ifm_entry *pifm)
{
    int ret = 0;
    struct qos_entry *pqos = NULL;
    enum QOS_TYPE type;

    if ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] Failed to execute command, due to the interface had binded trunk\n",
                   __FUNCTION__, __LINE__ );
        return ERRNO_SUCCESS;
    }

    if(IFM_TYPE_IS_OUTBAND(pifm->ifm.ifindex))
	{
		zlog_debug( IFM_DBG_COMMON,"%s[%d] Error:not support config ethernet 1/0/1,please check out!\n", __FUNCTION__, __LINE__ );
		return ERRNO_SUCCESS;
	}

    pqos = &(pifm->ifm.qos);

    type = pqos->domain_type;
    /*ret = ipc_send_hal(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF,
                                        QOS_INFO_DOMAIN, IPC_OPCODE_DELETE, pifm->ifm.ifindex);*/
	//ret = ifm_ipc_send(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, 
	//						QOS_INFO_DOMAIN, IPC_OPCODE_DELETE, pifm->ifm.ifindex);
    ret = ipc_send_msg_n2(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, QOS_INFO_DOMAIN, IPC_OPCODE_DELETE, pifm->ifm.ifindex);
    if(ret)
    {
        zlog_err ( "%s[%d] ipc_send_hal errocode=%d\n", __FUNCTION__, __LINE__, ret );
        return ERRNO_IPC;
    }

	ret = qos_domain_ref_operate(pqos->domain_id, pqos->domain_type, MODULE_ID_IFM, REF_OPER_DECREASE);
    if(ret)
    {
    	zlog_err ( "%s[%d] qos_domain_ref_operate failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /* update qos_domain info */
    QOS_DOMAIN_TYPE_GET(pifm->ifm.mode, type);
    pifm->ifm.qos.domain_type = type;
    pifm->ifm.qos.domain_id   = 0;

    return ret;
}


int ifm_qos_unset_phb(struct ifm_entry *pifm)
{
    int ret = 0;
    struct qos_entry *pqos = NULL;
    enum QOS_TYPE type;

    if ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] Failed to execute command, due to the interface had binded trunk\n",
                   __FUNCTION__, __LINE__ );
        return ERRNO_SUCCESS;
    }

    if(IFM_TYPE_IS_OUTBAND(pifm->ifm.ifindex))
	{
		zlog_debug( IFM_DBG_COMMON,"%s[%d] Error:not support config ethernet 1/0/1,please check out!\n", __FUNCTION__, __LINE__ );
		return ERRNO_SUCCESS;
	}

	pqos = &(pifm->ifm.qos);

    type = pqos->phb_type;
    /*ret = ipc_send_hal(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF,
                                        QOS_INFO_PHB, IPC_OPCODE_DELETE, pifm->ifm.ifindex);*/
	//ret = ifm_ipc_send(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, 
	//						QOS_INFO_PHB, IPC_OPCODE_DELETE, pifm->ifm.ifindex);
    ret = ipc_send_msg_n2(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_IFM, IPC_TYPE_QOSIF, QOS_INFO_PHB, IPC_OPCODE_DELETE, pifm->ifm.ifindex);
    if(ret)
    {
        zlog_err ( "%s[%d] ipc_send_hal errocode=%d\n", __FUNCTION__, __LINE__, ret );
        return ERRNO_IPC;
    }

    ret = qos_phb_ref_operate(pqos->phb_id, pqos->phb_type, MODULE_ID_IFM, REF_OPER_DECREASE);
    if(ret)
    {
    	zlog_err ( "%s[%d] qos_domain_ref_operate failed. errno=%d\n", __FUNCTION__, __LINE__, ret );
        return ret;
    }

    /* update qos_phb info */
    if( IFM_TYPE_IS_SUBPORT( pifm->ifm.ifindex )
		&& ( IFNET_MODE_L3 == pifm->ifm.mode ))
    {
		pifm->ifm.qos.phb_enable = QOS_PHB_ENABLE;
	    pifm->ifm.qos.phb_type   = QOS_TYPE_COS;
	    pifm->ifm.qos.phb_id     = 0;
    }
    else
    {
    	pifm->ifm.qos.phb_enable = QOS_PHB_DISABLE;
	    pifm->ifm.qos.phb_type   = QOS_TYPE_INVALID;
	    pifm->ifm.qos.phb_id     = 0;
    }

    return ret;
}


static int ifm_qos_snmp_get(uint32_t ifindex, struct qos_entry *pqos, struct qos_entry_snmp *pqos_snmp)
{
	//struct car_t *pcar = NULL;
	//struct hqos_t *phqos = NULL;

	if(NULL == pqos || NULL == pqos_snmp)
	{
		QOS_LOG_ERR("Input param check error in func\n");
		return ERRNO_PARAM_ILLEGAL;
	}

	memset(pqos_snmp, 0, sizeof(struct qos_entry_snmp));
	pqos_snmp->ifindex = ifindex;
	memcpy (&(pqos_snmp->qos), pqos, sizeof(struct qos_entry));

	return ERRNO_SUCCESS;
}

int ifm_qos_get_bulk ( uint32_t index, struct qos_entry_snmp *qos_array )
{
    int msg_num = IPC_MSG_LEN/sizeof(struct qos_entry_snmp);
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
//    uint32_t ifindex = 0;
    struct ifm_entry *pifm 		= NULL;
    struct qos_entry_snmp qos_snmp;
    int cursor;
    int val = 0;
    int data_num = 0;

    if ( index == 0 )
    {
    	HASH_BUCKET_LOOP(pbucket, cursor, ifm_table)
        {
            pifm = (struct ifm_entry *)pbucket->data;
            if (NULL == pifm || pifm->ifm.hide)
            {
                continue;
            }

            if(ifm_qos_snmp_get(pifm->ifm.ifindex, &(pifm->ifm.qos), &qos_snmp))
            {
            	continue;
            }

            memcpy(&qos_array[data_num++], &qos_snmp, sizeof(struct qos_entry_snmp));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&ifm_table, (void *)index);

        if (NULL == pbucket)
        {
        	val = (ifm_table.compute_hash((void *)index)) % HASHTAB_SIZE;

	        if (ifm_table.buckets[val] != NULL)
	        {
	        	pbucket = ifm_table.buckets[val];
	        }
	        else
	        {
	        	for (++val; val<HASHTAB_SIZE; ++val)
	            {
	                if (NULL != ifm_table.buckets[val])
	                {
	                    pbucket = ifm_table.buckets[val];
	                    break;
	                }
	            }
	        }
        }

        while (pbucket)
        {
            pnext = hios_hash_next_cursor(&ifm_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            pbucket = pnext;

            pifm = (struct ifm_entry *)(pnext->data);

            if(ifm_qos_snmp_get(pifm->ifm.ifindex, &(pifm->ifm.qos), &qos_snmp))
            {
            	continue;
            }

            memcpy(&qos_array[data_num++], &qos_snmp, sizeof(struct qos_entry_snmp));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


void ifm_qos_show_operation(struct vty *vty, struct ifm_entry *pifm)
{
    char ifname[IFNET_NAMESIZE] = "";
    const char *domain_str = NULL;
    const char *phb_str = NULL;

    if(NULL != pifm)
    {
    	if(IFM_TYPE_IS_OUTBAND(pifm->ifm.ifindex))
        {
        	return;
        }

        // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
        if (IFNET_MODE_PHYSICAL == pifm->ifm.mode)
        {
            return;
        }

        ifm_get_name_by_ifindex(pifm->ifm.ifindex, ifname);
        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        /* QoS DOMAIN */
        if(QOS_TYPE_INVALID != pifm->ifm.qos.domain_type)
        {
            IFM_QOS_DOMAIN_STR_GET(pifm->ifm.qos.domain_type, domain_str);
            vty_out(vty, "%18s %s %d%s", "QoS DOMAIN:", domain_str,pifm->ifm.qos.domain_id, VTY_NEWLINE);
        }

        /* QoS PHB */
        if(QOS_PHB_ENABLE == pifm->ifm.qos.phb_enable)
        {
            IFM_QOS_PHB_STR_GET(pifm->ifm.qos.phb_type, phb_str);
            vty_out(vty, "%18s %s %d%s", "QoS PHB:", phb_str, pifm->ifm.qos.phb_id, VTY_NEWLINE);
        }

        /* untag pri */
        if ((IFM_TYPE_IS_PHYSICAL(pifm->ifm.ifindex)) ||
        	(IFM_TYPE_IS_TDM(pifm->ifm.ifindex)) ||
	        (IFM_TYPE_IS_SUBPORT(pifm->ifm.ifindex) && IFNET_MODE_L2 == pifm->ifm.mode))
        {
            vty_out(vty, "%18s %d%s", "Untag priority:", pifm->ifm.qos.untag_pri, VTY_NEWLINE);
        }
    }
}


DEFUN (show_qos_interface_ethernet,
        show_qos_interface_ethernet_cmd,
        "show qos interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;

    if(argv[0] != NULL && argv[1] == NULL && argv[2] == NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( (char *)"ethernet", ( char * ) argv[0] );
	}
	else if(argv[0] == NULL && argv[1] != NULL && argv[2] == NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( (char *)"gigabitethernet", ( char * ) argv[1] );
	}
	else if(argv[0] == NULL && argv[1] == NULL && argv[2] != NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( (char *)"xgigabitethernet", ( char * ) argv[2] );
	}

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        return CMD_SUCCESS;
    }

    ifm_qos_show_operation(vty, pifm);

    return CMD_SUCCESS;
}


DEFUN (show_qos_interface_tdm,
        show_qos_interface_tdm_cmd,
        "show qos interface tdm USP",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR)
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;

    ifindex = ifm_get_ifindex_by_name((char *)"tdm", (char *)argv[0]);

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        return CMD_SUCCESS;
    }

    ifm_qos_show_operation(vty, pifm);

    return CMD_SUCCESS;
}


DEFUN (show_qos_interface_vlanif,
        show_qos_interface_vlanif_cmd,
        "show qos interface vlanif <1-4094>",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR)
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;

    ifindex = ifm_get_ifindex_by_name((char *)"vlanif", (char *)argv[0]);

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        return CMD_SUCCESS;
    }

    ifm_qos_show_operation(vty, pifm);

    return CMD_SUCCESS;
}


DEFUN (show_qos_interface_trunk,
        show_qos_interface_trunk_cmd,
        "show qos interface trunk TRUNK",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;

    ifindex = ifm_get_ifindex_by_name((char *)"trunk", (char *)argv[0]);

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        return CMD_SUCCESS;
    }

    ifm_qos_show_operation(vty, pifm);

    return CMD_SUCCESS;
}


DEFUN(qos_mapping_domain_enable,
        qos_mapping_domain_enable_cmd,
        "qos mapping enable (cos-domain|tos-domain|dscp-domain) <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Attach qos mapping domain to interface\n"
        "Cos to queue mapping domain\n"
        "Tos to queue mapping domain\n"
        "Dscp to queue mapping domain\n"
        "QoS mapping domain ID\n")
{
    uint32_t ifindex;
    struct qos_entry qos;
    int ret;

    memset(&qos, 0, sizeof(struct qos_entry));

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        qos.domain_type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        qos.domain_type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'd')
    {
        qos.domain_type = QOS_TYPE_DSCP;
    }

    /*get argv[1]*/
    qos.domain_id = atoi(argv[1]);
    ifindex = ( uint32_t ) vty->index;

    ret = ifm_qos_set_info(ifindex, (void *)&qos, QOS_INFO_DOMAIN);
    if ( ret )
    {
        if(IFM_ERR_WRONG_QOS_TYPE == ret)
        {
            vty_error_out ( vty, "Not supported QoS type in this port mode %s", VTY_NEWLINE );
        }
        else if(ERRNO_NOT_FOUND == ret)
        {
            vty_error_out ( vty, "The appointed %s is not exist!! %s", argv[0], VTY_NEWLINE );
        }
        else if(IFM_ERR_BINDED_TRUNK_ERRO == ret)
        {
            vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
        }
        else if(IFM_ERR_PARAM == ret)
        {
        	vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
        }
        else
        {
            vty_error_out ( vty, "Failed to execute command.%s", VTY_NEWLINE );
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(no_qos_mapping_domain_enable,
        no_qos_mapping_domain_enable_cmd,
        "no qos mapping enable (cos-domain|tos-domain|dscp-domain)",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Attach qos mapping domain to interface\n"
        "Cos to queue mapping domain\n"
        "Tos to queue mapping domain\n"
        "Dscp to queue mapping domain\n")
{
    int ret;
    uint32_t ifindex;
    struct ifm_entry *pifm = NULL;

    ifindex = (uint32_t)vty->index;
    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        vty_error_out(vty, "Interface info error!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        if(QOS_TYPE_COS != pifm->ifm.qos.domain_type)
        {
            vty_error_out(vty, "Cos domain is not applied to this interface!!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (argv[0][0] == 't')
    {
        if(QOS_TYPE_TOS != pifm->ifm.qos.domain_type)
        {
            vty_error_out(vty, "Tos domain is not applied to this interface!!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (argv[0][0] == 'd')
    {
        if(QOS_TYPE_DSCP != pifm->ifm.qos.domain_type)
        {
            vty_error_out(vty, "Dscp domain is not applied to this interface!!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
    {
        zlog_err ( "%s[%d] Failed to execute command, due to the interface had binded trunk\n",
                   __FUNCTION__, __LINE__ );
        return CMD_WARNING;
    }

    ret = ifm_qos_unset_domain(pifm);
    if ( ret )
    {
        if(IFM_ERR_BINDED_TRUNK_ERRO == ret)
        {
            vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
        }
        else
        {
        	vty_error_out ( vty, "Failed to execute command.%s", VTY_NEWLINE );
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(qos_mapping_phb_enable,
        qos_mapping_phb_enable_cmd,
        "qos mapping enable (cos-phb|tos-phb|dscp-phb) <0-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Attach qos mapping phb to interface\n"
        "Queue to cos mapping\n"
        "Queue to tos mapping\n"
        "Queue to dscp mapping\n"
        "QoS mapping phb ID\n")
{
    uint32_t ifindex;
    struct qos_entry qos;
    int ret;

    memset(&qos, 0, sizeof(struct qos_entry));
    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        qos.phb_type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        qos.phb_type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'd')
    {
        qos.phb_type = QOS_TYPE_DSCP;
    }

    /*get argv[1]*/
    qos.phb_id = atoi(argv[1]);
    qos.phb_enable = QOS_PHB_ENABLE;

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_qos_set_info(ifindex, (void *)&qos, QOS_INFO_PHB);
    if ( ret )
    {
        if(IFM_ERR_WRONG_QOS_TYPE == ret)
        {
            vty_error_out ( vty, "Not supported QoS type in this port mode %s", VTY_NEWLINE );
        }
        else if(ERRNO_NOT_FOUND == ret)
        {
            vty_error_out ( vty, "The appointed %s is not exist!! %s", argv[0], VTY_NEWLINE );
        }
        else if(IFM_ERR_BINDED_TRUNK_ERRO == ret)
        {
            vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
        }
        else if(IFM_ERR_PARAM == ret)
        {
        	vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
        }
        else
        {
            vty_error_out ( vty, "Failed to execute command.%s", VTY_NEWLINE );
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(no_qos_mapping_phb_enable,
        no_qos_mapping_phb_enable_cmd,
        "no qos mapping enable (cos-phb|tos-phb|dscp-phb)",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Attach qos mapping phb to interface\n"
        "Queue to cos mapping\n"
        "Queue to tos mapping\n"
        "Queue to dscp mapping\n")
{
    int ret;
    uint32_t ifindex;
    struct ifm_entry *pifm = NULL;

    ifindex = (uint32_t)vty->index;
    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        vty_error_out(vty, "Interface info error!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        if(QOS_TYPE_COS != pifm->ifm.qos.phb_type)
        {
            vty_error_out(vty, "Cos phb is not applied to this interface!!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (argv[0][0] == 't')
    {
        if(QOS_TYPE_TOS != pifm->ifm.qos.phb_type)
        {
            vty_error_out(vty, "Tos phb is not applied to this interface!!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (argv[0][0] == 'd')
    {
        if(QOS_TYPE_DSCP != pifm->ifm.qos.phb_type)
        {
            vty_error_out(vty, "Dscp phb is not applied to this interface!!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
    {
        zlog_err ( "%s[%d] Failed to execute command, due to the interface had binded trunk\n",
                   __FUNCTION__, __LINE__ );
        return IFM_ERR_BINDED_TRUNK_ERRO;
    }

    ret = ifm_qos_unset_phb(pifm);
    if ( ret )
    {
        if(IFM_ERR_BINDED_TRUNK_ERRO == ret)
        {
            vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
        }
        else
        {
        	vty_error_out ( vty, "Failed to execute command.%s", VTY_NEWLINE );
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(no_tunnel_protocol,
    no_tunnel_protocol_cmd,
    "no protocol",
    "\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ifm_entry *pifm = NULL;

    pifm = ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return CMD_SUCCESS;
    }

    pifm->ifm.qos.domain_type = QOS_TYPE_INVALID;
    pifm->ifm.qos.domain_id   = 0;
    pifm->ifm.qos.phb_type    = QOS_TYPE_INVALID;
    pifm->ifm.qos.phb_id      = 0;
    pifm->ifm.qos.phb_enable  = DISABLE;

    return CMD_SUCCESS;
}


DEFUN(qos_mapping_exp_domain_enable,
        qos_mapping_exp_domain_enable_cmd,
        "qos mapping enable exp-domain <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Exp to queue mapping domain\n"
        "QoS mapping domain ID\n")
{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pifm = ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return CMD_SUCCESS;
    }

    pifm->ifm.qos.domain_type = QOS_TYPE_EXP;
    pifm->ifm.qos.domain_id   = atoi(argv[0]);

    return CMD_SUCCESS;
}


DEFUN(no_qos_mapping_exp_domain_enable,
        no_qos_mapping_exp_domain_enable_cmd,
        "no qos mapping enable exp-domain",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Exp to queue mapping domain\n")
{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pifm = ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return CMD_SUCCESS;
    }

    pifm->ifm.qos.domain_type = QOS_TYPE_EXP;
    pifm->ifm.qos.domain_id   = 0;

    return CMD_SUCCESS;
}


DEFUN(qos_mapping_exp_phb_enable,
        qos_mapping_exp_phb_enable_cmd,
        "qos mapping enable exp-phb <0-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Queue to exp mapping\n"
        "QoS mapping phb ID\n")
{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pifm = ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return CMD_SUCCESS;
    }

    pifm->ifm.qos.phb_type   = QOS_TYPE_EXP;
    pifm->ifm.qos.phb_enable = QOS_PHB_ENABLE;
    pifm->ifm.qos.phb_id     = atoi(argv[0]);

    return CMD_SUCCESS;
}


DEFUN(no_qos_mapping_exp_phb_enable,
        no_qos_mapping_exp_phb_enable_cmd,
        "no qos mapping enable exp-phb",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Queue to exp mapping\n")
{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pifm = ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return CMD_SUCCESS;
    }

    pifm->ifm.qos.phb_type   = QOS_TYPE_EXP;
    pifm->ifm.qos.phb_enable = QOS_PHB_DISABLE;
    pifm->ifm.qos.phb_id     = 0;

    return CMD_SUCCESS;
}


DEFUN(qos_port_vlan_priority,
        qos_port_vlan_priority_cmd,
        "qos priority <0-7>",
        "Quality of Service\n"
        "Port vlan priority\n"
        "Port vlan priority value\n")
{
    uint32_t ifindex;
    uint8_t untag_pri;
    int ret;

    /*get argv[0]*/
    untag_pri = atoi(argv[0]);

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_qos_set_info(ifindex, (void *)&untag_pri, QOS_INFO_UTAG_PRI);
    if ( ret )
    {
        if(IFM_ERR_WRONG_QOS_TYPE == ret)
        {
            vty_error_out ( vty, "Not supported QoS type in this port mode.%s", VTY_NEWLINE );
        }
        else if(IFM_ERR_BINDED_TRUNK_ERRO == ret)
        {
            vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
        }
        else if(IFM_ERR_PARAM == ret)
        {
        	vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
        }
        else
        {
        	vty_error_out ( vty, "Failed to execute command.%s", VTY_NEWLINE );
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(no_qos_port_vlan_priority,
        no_qos_port_vlan_priority_cmd,
        "no qos priority",
        NO_STR
        "Quality of Service\n"
        "Port vlan priority\n")
{
    uint32_t ifindex;
    uint8_t untag_pri;
    int ret;

    /*get argv[0]*/
    untag_pri = 5;

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_qos_set_info(ifindex, (void *)&untag_pri, QOS_INFO_UTAG_PRI);
    if ( ret )
    {
        if(IFM_ERR_WRONG_QOS_TYPE == ret)
        {
            vty_error_out ( vty, "Not supported QoS type in this port mode.%s", VTY_NEWLINE );
        }
        else if(IFM_ERR_BINDED_TRUNK_ERRO == ret)
        {
            vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
        }
        else if(IFM_ERR_PARAM == ret)
        {
        	vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
        }
        else
        {
        	vty_error_out ( vty, "Failed to execute command.%s", VTY_NEWLINE );
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/* write the configuration of each interface */
void ifm_qos_config_write (struct ifm_entry *pifm, struct vty *vty)
{
    if(NULL == pifm || IFM_TYPE_IS_TUNNEL(pifm->ifm.ifindex))
    {
        return ;
    }
    
    /* save qos_domain */
    if(0 != pifm->ifm.qos.domain_id)
    {
        switch(pifm->ifm.qos.domain_type)
        {
            case QOS_TYPE_COS:
                vty_out(vty, " qos mapping enable cos-domain %d%s", pifm->ifm.qos.domain_id, VTY_NEWLINE);
                break;
            case QOS_TYPE_TOS:
                vty_out(vty, " qos mapping enable tos-domain %d%s", pifm->ifm.qos.domain_id, VTY_NEWLINE);
                break;
            case QOS_TYPE_EXP:
                vty_out(vty, " qos mapping enable exp-domain %d%s", pifm->ifm.qos.domain_id, VTY_NEWLINE);
                break;
            case QOS_TYPE_DSCP:
                vty_out(vty, " qos mapping enable dscp-domain %d%s", pifm->ifm.qos.domain_id, VTY_NEWLINE);
                break;
            default:
                break;
        }
    }

    /* save qos_phb */
    if(QOS_PHB_ENABLE == pifm->ifm.qos.phb_enable)
    {
        switch(pifm->ifm.qos.phb_type)
        {
            case QOS_TYPE_COS:
				if(!(0 == pifm->ifm.qos.phb_id && pifm->ifm.mode == IFNET_MODE_L3
					&& IFM_TYPE_IS_SUBPORT(pifm->ifm.ifindex)))
				{
                	vty_out(vty, " qos mapping enable cos-phb %d%s", pifm->ifm.qos.phb_id, VTY_NEWLINE);
                }
				break;
            case QOS_TYPE_TOS:
                vty_out(vty, " qos mapping enable tos-phb %d%s", pifm->ifm.qos.phb_id, VTY_NEWLINE);
                break;
            case QOS_TYPE_EXP:
                vty_out(vty, " qos mapping enable exp-phb %d%s", pifm->ifm.qos.phb_id, VTY_NEWLINE);
                break;
            case QOS_TYPE_DSCP:
                vty_out(vty, " qos mapping enable dscp-phb %d%s", pifm->ifm.qos.phb_id, VTY_NEWLINE);
                break;
            default:
                break;
        }
    }

    /* save qos_pri */
    if ((IFM_TYPE_IS_PHYSICAL(pifm->ifm.ifindex)) ||
    	(IFM_TYPE_IS_TDM(pifm->ifm.ifindex)) ||
        (IFM_TYPE_IS_SUBPORT(pifm->ifm.ifindex) && IFNET_MODE_L2 == pifm->ifm.mode))
    {
        if(DEFAULT_UNTAG_PRI != pifm->ifm.qos.untag_pri)
        {
            vty_out(vty, " qos priority %d%s", pifm->ifm.qos.untag_pri, VTY_NEWLINE);
        }
    }
}


void ifm_qos_init(void)
{
    install_element (PHYSICAL_IF_NODE, &qos_mapping_domain_enable_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &qos_mapping_domain_enable_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &qos_mapping_domain_enable_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &qos_mapping_domain_enable_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_mapping_domain_enable_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &qos_mapping_phb_enable_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_mapping_phb_enable_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_qos_mapping_domain_enable_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_qos_mapping_domain_enable_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_qos_mapping_domain_enable_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_qos_mapping_domain_enable_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &no_qos_mapping_domain_enable_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_qos_mapping_phb_enable_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_qos_mapping_phb_enable_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &qos_port_vlan_priority_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &qos_port_vlan_priority_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &qos_port_vlan_priority_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &qos_port_vlan_priority_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_qos_port_vlan_priority_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_qos_port_vlan_priority_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_qos_port_vlan_priority_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &no_qos_port_vlan_priority_cmd, CMD_SYNC);

	/* add exp domain/phb by zhangfj */
    install_element (TUNNEL_IF_NODE, &qos_mapping_exp_domain_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_qos_mapping_exp_domain_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &qos_mapping_exp_phb_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_qos_mapping_exp_phb_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_protocol_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &show_qos_interface_ethernet_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_qos_interface_tdm_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_qos_interface_vlanif_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_qos_interface_trunk_cmd, CMD_LOCAL);
}



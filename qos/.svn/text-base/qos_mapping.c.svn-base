#include <qos/qos_mapping.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <qos/qos_if.h>
#include <string.h>


struct qos_domain *pdomain_cos[QOS_DOMAIN_NUM];
struct qos_domain *pdomain_tos[QOS_DOMAIN_NUM];
struct qos_domain *pdomain_exp[QOS_DOMAIN_NUM];
struct qos_domain *pdomain_dscp[QOS_DOMAIN_NUM];


struct qos_phb *pphb_cos[QOS_PHB_NUM];
struct qos_phb *pphb_tos[QOS_PHB_NUM];
struct qos_phb *pphb_exp[QOS_PHB_NUM];
struct qos_phb *pphb_dscp[QOS_PHB_NUM];


/*设置一个Qos_domain的默认映射*/
int qos_domain_set_default_mapping(uint8_t id, enum QOS_TYPE type, struct qos_domain *pdomain)
{
    int i = 0;
    int color = 0;

    pdomain->id = id;
    pdomain->type = type;
    switch(type)
    {
        case QOS_TYPE_COS:
        case QOS_TYPE_TOS:
        case QOS_TYPE_EXP:
            for(i=0; i<8; i++)
            {
                pdomain->map[i].priority = i;
                pdomain->map[i].queue = i; 
                pdomain->map[i].color = QOS_COLOR_GREEN;
            }
            break;
        case QOS_TYPE_DSCP:
            for(i=0; i<64; i++)
            {
                pdomain->map[i].priority = i; 
                if(((i & 0x01) == 0) && ((i >> 3) > 0) && ((i >> 3) <= 4))
                {
                    pdomain->map[i].queue = i >> 3;
                    color = (i >> 1) & 0x3;
                    if(color == 2)
                        pdomain->map[i].color = QOS_COLOR_YELLOW;
                    else if(color == 3)
                        pdomain->map[i].color = QOS_COLOR_RED;
                    else
                        pdomain->map[i].color = QOS_COLOR_GREEN;
                }
                else if(i == 40 || i == 46)
                {
                    pdomain->map[i].queue = 5;
                    pdomain->map[i].color = QOS_COLOR_GREEN;
                }
                else if(i == 48)
                {
                    pdomain->map[i].queue = 6;
                    pdomain->map[i].color = QOS_COLOR_GREEN;
                }
                else if(i == 56)
                {
                    pdomain->map[i].queue = 7;
                    pdomain->map[i].color = QOS_COLOR_GREEN;
                }
                else 
                {
                    pdomain->map[i].queue = 0;
                    pdomain->map[i].color = QOS_COLOR_GREEN;
                }
            }    
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return ERRNO_PARAM_ILLEGAL;
    }

    return ERRNO_SUCCESS;

}

/*设置一个Qos_phb的默认映射*/
int qos_phb_set_default_mapping(uint8_t id, enum QOS_TYPE type, struct qos_phb *pphb)
{
    int i = 0;

    pphb->id = id;
    pphb->type = type;
    switch(type)
    {
        case QOS_TYPE_COS:
        case QOS_TYPE_TOS:
        case QOS_TYPE_EXP:
            for(i=0; i<24; i++)
            {
                pphb->map[i].queue = i/3;
                pphb->map[i].color = i%3;
                pphb->map[i].priority = i/3; 
            }
            break;
        case QOS_TYPE_DSCP:
            for(i=0; i<24; i++)
            {
                pphb->map[i].queue = i/3;
                pphb->map[i].color = i%3;
                
                if(pphb->map[i].queue == 0)
                {                    
                    pphb->map[i].priority = 0;            
                }
                else if(pphb->map[i].queue == 5)
                {
                    pphb->map[i].priority = 46;
                }
                else if(pphb->map[i].queue == 6)
                {
                    pphb->map[i].priority = 48;
                }
                else if(pphb->map[i].queue == 7)
                {
                    pphb->map[i].priority = 56;
                }
                else
                {
                    pphb->map[i].priority = 8 * pphb->map[i].queue + 2 + pphb->map[i].color * 2;
                }    
            }    
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return ERRNO_PARAM_ILLEGAL;
    }

    return ERRNO_SUCCESS;

}

/*qos_domain初始化，分配空间, 创建默认模板0*/
void qos_domain_init(void)
{
    int i = 0;
    struct qos_domain *pdomain = NULL;
    
    for(i=0; i<QOS_DOMAIN_NUM; i++)
    {
        pdomain_cos[i] = NULL;
        pdomain_tos[i] = NULL;
        pdomain_exp[i] = NULL;
        pdomain_dscp[i] = NULL;
    }

    pdomain = qos_domain_create(0, QOS_TYPE_COS);
    if (NULL == pdomain)
    {
        QOS_LOG_ERR("Failed to create cos domain 0.\n");
        return;
    }

    pdomain = qos_domain_create(0, QOS_TYPE_TOS);
    if (NULL == pdomain)
    {
        QOS_LOG_ERR("Failed to create tos domain 0.\n");
        return;
    }

    pdomain = qos_domain_create(0, QOS_TYPE_EXP);
    if (NULL == pdomain)
    {
        QOS_LOG_ERR("Failed to create exp domain 0.\n");
        return;
    }

    pdomain = qos_domain_create(0, QOS_TYPE_DSCP);
    if (NULL == pdomain)
    {
        QOS_LOG_ERR("Failed to create dscp domain 0.\n");
        return;
    }
    
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
}

/*qos_phb初始化，分配空间, 创建默认模板0*/
void qos_phb_init(void)
{
    int i = 0;
    struct qos_phb *pphb = NULL;
    
    for(i=0; i<QOS_PHB_NUM; i++)
    {
        pphb_cos[i] = NULL;
        pphb_tos[i] = NULL;
        pphb_exp[i] = NULL;
        pphb_dscp[i] = NULL;
    }

    pphb = qos_phb_create(0, QOS_TYPE_COS);
    if (NULL == pphb)
    {
        QOS_LOG_ERR("Failed to create cos phb 0.\n");
        return;
    }

    pphb = qos_phb_create(0, QOS_TYPE_TOS);
    if (NULL == pphb)
    {
        QOS_LOG_ERR("Failed to create tos phb 0.\n");
        return;
    }

    pphb = qos_phb_create(0, QOS_TYPE_EXP);
    if (NULL == pphb)
    {
        QOS_LOG_ERR("Failed to create exp phb 0.\n");
        return;
    }

    pphb = qos_phb_create(0, QOS_TYPE_DSCP);
    if (NULL == pphb)
    {
        QOS_LOG_ERR("Failed to create dscp phb 0.\n");
        return;
    }
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
}

/*创建一个QoS_domain*/
struct qos_domain *qos_domain_create(uint8_t id, enum QOS_TYPE type)
{
    struct qos_domain *pqosd = NULL;
    int ret = 0;

    /*如果非空，直接返回*/
    switch(type)
    {
        case QOS_TYPE_COS:
            if (NULL != pdomain_cos[id])
                return pdomain_cos[id];
            break;
        case QOS_TYPE_TOS:
            if (NULL != pdomain_tos[id])
                return pdomain_tos[id];
            break;
        case QOS_TYPE_EXP:
            if (NULL != pdomain_exp[id])
                return pdomain_exp[id];
            break;
        case QOS_TYPE_DSCP:
            if (NULL != pdomain_dscp[id])
                return pdomain_dscp[id];
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return NULL;
    }

    /*malloc & set*/
    pqosd = (struct qos_domain *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_domain));
    if (NULL == pqosd)
    {
        QOS_LOG_ERR("In function '%s' the memory is insufficient.\n", __func__);
        return NULL;
    }
    memset(pqosd, 0, sizeof(struct qos_domain));

    /*set parameters*/
    ret = qos_domain_set_default_mapping(id, type, pqosd);
    if (ret != 0)
    {
        QOS_LOG_ERR("Qos Domain default mapping set failed.\n");
        XFREE(MTYPE_QOS_ENTRY, pqosd);
        return NULL;
    }
	
	if(!((QOS_TYPE_DSCP == type) && (0 == id)))
	{
		#if 0
		ret = ipc_send_hal((void *)pqosd, sizeof(struct qos_domain), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_ADD, id);
    	#else
    	ret = ipc_send_msg_n2((void *)pqosd, sizeof(struct qos_domain), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_ADD, id);
    	#endif
	    if(ret != 0)
	    {
	        QOS_LOG_ERR ("ipc_send_hal errocode=%d DOMAIN_TYPE=%d DOMAIN_ID=%d\n", ret, type, id );
	        XFREE(MTYPE_QOS_ENTRY, pqosd);
	        return NULL;
	    }
	}
    
    /*save qos domain*/
    switch(type)
    {
        case QOS_TYPE_COS:
            pdomain_cos[id] = pqosd;
            break;
        case QOS_TYPE_TOS:
            pdomain_tos[id] = pqosd;
            break;
        case QOS_TYPE_EXP:
            pdomain_exp[id] = pqosd;
            break;
        case QOS_TYPE_DSCP:
            pdomain_dscp[id] = pqosd;
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            XFREE(MTYPE_QOS_ENTRY, pqosd);
            return NULL;
    }
    
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return pqosd;
}

/*删除一个Qos_domain*/
int qos_domain_delete(uint8_t id, enum QOS_TYPE type)
{
    struct qos_domain *pqosd = NULL;
    int ret = 0;

    QOS_LOG_DBG("Entering function '%s'.\n", __func__);
    
    pqosd = qos_domain_get(id, type);
    if (NULL == pqosd)
    {
        QOS_LOG_ERR("Qos Domain with ID%d is not exist!!\n", id);
        return ERRNO_NOT_FOUND;
    }

    /* check whether the qos_domain is applied or not  */
    if( QOS_MAPPING_DETACHED != pqosd->ref_cnt )
    {
        QOS_LOG_ERR("QoS domain is applied to interface, please detach first!!\n");
        return QOS_ERR_ATTACHED;
    }
    #if 0
    ret = ipc_send_hal( &type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_QOS, IPC_TYPE_QOS,
                                      QOS_INFO_DOMAIN, IPC_OPCODE_DELETE, id );
    #else
    ret = ipc_send_msg_n2( &type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_QOS, IPC_TYPE_QOS,
                                      QOS_INFO_DOMAIN, IPC_OPCODE_DELETE, id );
    #endif
    if ( ret < 0 )
    {
        QOS_LOG_ERR ( "ipc_send_hal errocode=%d,domain_id=%d\n", ret, id );
        return ERRNO_IPC;
    }

    XFREE(MTYPE_QOS_ENTRY, pqosd);
    /*NULL pdomain_xxx*/
    switch(type)
    {
        case QOS_TYPE_COS:
            pdomain_cos[id] = NULL;
            break;
        case QOS_TYPE_TOS:
            pdomain_tos[id] = NULL;
            break;
        case QOS_TYPE_EXP:
            pdomain_exp[id] = NULL;
            break;
        case QOS_TYPE_DSCP:
            pdomain_dscp[id] = NULL;
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type.\n");
            return ERRNO_PARAM_ILLEGAL;
    }
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return ret;
}

/*获取一个Qos_domain信息*/
struct qos_domain *qos_domain_get(uint8_t id, enum QOS_TYPE type)
{
    struct qos_domain *pqosd = NULL;
    
    /*return specify type qos_domain*/
    switch(type)
    {
        case QOS_TYPE_COS:            
            pqosd = pdomain_cos[id];
            break;
        case QOS_TYPE_TOS:
            pqosd = pdomain_tos[id];
            break;
        case QOS_TYPE_EXP:
            pqosd = pdomain_exp[id];
            break;
        case QOS_TYPE_DSCP:
            pqosd = pdomain_dscp[id];
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return NULL;
    }

    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return pqosd;
    
}

/*在指定Qos_domain中添加一组映射关系*/
int qos_domain_add_mapping(uint8_t id, enum QOS_TYPE type, struct qos_map *pmap)
{
    int ret = 0;
    int pindex = 0;
    struct qos_domain sdomain;
    struct qos_domain *pdomain = NULL;

    memset(&sdomain, 0, sizeof(struct qos_domain));
    
    /*use qos_map.priority as index of pdomain.map[]*/
    pdomain = qos_domain_get(id, type);
    if(NULL == pdomain)
    {
    	QOS_LOG_ERR("Qos domain info error.\n");
    	return ERRNO_NOT_FOUND;
    }
    
    if(pdomain->ref_cnt)
    {
    	QOS_LOG_ERR("Qos domain applied, not permit to modify.\n");
    	return QOS_ERR_ATTACHED;
    }
    
    /* update qos domain info */
    pindex = pmap->priority;
    memcpy(&pdomain->map[pindex], pmap, sizeof(struct qos_map));
	
	/* send to hal */
	memset(&sdomain, 0, sizeof(struct qos_domain));
	sdomain.id = id;
	sdomain.type = type;
    memcpy(&(sdomain.map[0]), pmap, sizeof(struct qos_map));       /*sdomain.map[0] represent qos_map to add*/
	#if 0
    ret = ipc_send_hal((void *)(&sdomain), sizeof(struct qos_domain), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, id);
	#else
	ret = ipc_send_msg_n2((void *)(&sdomain), sizeof(struct qos_domain), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, id);
	#endif
    if(ret != 0)
    {
        QOS_LOG_ERR ( "ipc_send_hal_wait_ack errocode=%d DOMAIN_ID=%d\n", ret, id );
        return ERRNO_IPC;
    }
    
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return ret;

}

/*删除指定QoS_domain中的一组映射关系*/
int qos_domain_delete_mapping(uint8_t id, enum QOS_TYPE type, uint8_t priority)
{
    int ret = 0;
    int color = 0;
    struct qos_domain sdomain;
    struct qos_domain *pdomain = NULL;
    
    pdomain = qos_domain_get(id, type);
    if(NULL == pdomain)
    {
    	QOS_LOG_ERR("Qos domain info error.\n");
    	return ERRNO_NOT_FOUND;
    }
    
    if(pdomain->ref_cnt)
    {
    	QOS_LOG_ERR("Qos domain applied, not permit to modify.\n");
    	return QOS_ERR_ATTACHED;
    }
    
    switch(type)
    {
        case QOS_TYPE_COS:
            memcpy(&sdomain, pdomain_cos[id], sizeof(struct qos_domain));    /* for IPC */
            pdomain_cos[id]->map[priority].priority = priority;
            pdomain_cos[id]->map[priority].queue = priority;
            pdomain_cos[id]->map[priority].color = QOS_COLOR_GREEN;
            memcpy(&(sdomain.map[0]), &(pdomain_cos[id]->map[priority]), sizeof(struct qos_map));   /* sdomain.map[0] represent qos_map to delete */
            break;
        case QOS_TYPE_TOS:
            memcpy(&sdomain, pdomain_tos[id], sizeof(struct qos_domain));
            pdomain_tos[id]->map[priority].priority = priority;
            pdomain_tos[id]->map[priority].queue = priority;
            pdomain_tos[id]->map[priority].color = QOS_COLOR_GREEN;
            memcpy(&(sdomain.map[0]), &(pdomain_tos[id]->map[priority]), sizeof(struct qos_map));   
            break;
        case QOS_TYPE_EXP:
            memcpy(&sdomain, pdomain_exp[id], sizeof(struct qos_domain));
            pdomain_exp[id]->map[priority].priority = priority;
            pdomain_exp[id]->map[priority].queue = priority;
            pdomain_exp[id]->map[priority].color = QOS_COLOR_GREEN;
            memcpy(&(sdomain.map[0]), &(pdomain_exp[id]->map[priority]), sizeof(struct qos_map));  
            break;
        case QOS_TYPE_DSCP:
            memcpy(&sdomain, pdomain_dscp[id], sizeof(struct qos_domain));
            pdomain_dscp[id]->map[priority].priority = priority;
            if(((priority & 0x01) == 0) && ((priority>> 3) > 0) && ((priority >> 3) <= 4))
            {
                pdomain_dscp[id]->map[priority].queue = priority >> 3;
                color = (priority >> 1) & 0x3;
                if(color == 2)
                    pdomain_dscp[id]->map[priority].color = QOS_COLOR_YELLOW;
                else if(color == 3)
                    pdomain_dscp[id]->map[priority].color = QOS_COLOR_RED;
                else
                    pdomain_dscp[id]->map[priority].color = QOS_COLOR_GREEN;
            }
            else if(priority == 40 || priority == 46)
            {
                pdomain_dscp[id]->map[priority].queue = 5;
                pdomain_dscp[id]->map[priority].color = QOS_COLOR_GREEN;
            }
            else if(priority == 48)
            {
                pdomain_dscp[id]->map[priority].queue = 6;
                pdomain_dscp[id]->map[priority].color = QOS_COLOR_GREEN;
            }
            else if(priority == 56)
            {
                pdomain_dscp[id]->map[priority].queue = 7;
                pdomain_dscp[id]->map[priority].color = QOS_COLOR_GREEN;
            }
            else 
            {
                pdomain_dscp[id]->map[priority].queue = 0;
                pdomain_dscp[id]->map[priority].color = QOS_COLOR_GREEN;
            }
            memcpy(&(sdomain.map[0]), &(pdomain_dscp[id]->map[priority]), sizeof(struct qos_map));
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return ERRNO_PARAM_ILLEGAL;
    }
	#if 0
    ret = ipc_send_hal((void *)(&sdomain), sizeof(struct qos_domain), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, id);
    #else
    ret = ipc_send_msg_n2((void *)(&sdomain), sizeof(struct qos_domain), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_UPDATE, id);
    #endif
    if(ret != 0)
    {
        QOS_LOG_ERR ( "ipc_send_hal_wait_ack errocode=%d DOMAIN_ID=%d\n", ret, id );
        return ERRNO_IPC;
    }
    
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return ret;
}


/************************************************
 * Function: qos_domain_get_bulk
 * Input:   
 *      index: domain profile index
 *		type:  domain profile type
 * Output:
 *      domain_array: domain data 
 * Return:  
 *      data num
 * Description:  
 *      Get qos domain profile data.
 ************************************************/
int qos_domain_get_bulk ( uint8_t index, enum QOS_TYPE type, struct qos_domain *domain_array )
{
	int msg_num = IPC_MSG_LEN/sizeof(struct qos_domain);
    struct qos_domain *pdomain = NULL;
    int data_num = 0;
    int i = 0;

    if ( index == 0 )
    {
    	for (i=0; i<QOS_DOMAIN_NUM; i++)
	    {
	    	pdomain = qos_domain_get(i, type);
	        if (pdomain != NULL)
	        {
	        	
	        		        	
	        	memcpy(&domain_array[data_num], pdomain, sizeof(struct qos_domain));    
				if (0 == pdomain->id)
	        	{
					domain_array[data_num].id = QOS_DOMAIN_NUM;
				}

				data_num++;
	        }
	        
	        if (data_num == msg_num)
	        {
	        	return data_num;
	        }
	    }
    }
    else
    {
    	for (i=++index; i<QOS_DOMAIN_NUM; i++)
	    {
	        pdomain = qos_domain_get(i, type);
	        if(NULL == pdomain)
	        {
	        	continue;
	        }
	        
	        memcpy(&domain_array[data_num], pdomain, sizeof(struct qos_phb));
	        if (0 == pdomain->id)
        	{
				domain_array[data_num].id = QOS_DOMAIN_NUM;
			}

			data_num++;
			
	        if (data_num == msg_num)
	        {
	        	return data_num;
	        }
	    }
    }  

    return data_num;
}


/*创建一个Qos_phb*/
struct qos_phb *qos_phb_create(uint8_t id, enum QOS_TYPE type)
{
    struct qos_phb *pqosp = NULL;
    int ret = 0;

    /*如果非空，直接返回*/
    switch(type)
    {
        case QOS_TYPE_COS:
            if (NULL != pphb_cos[id])
                return pphb_cos[id];
            break;
        case QOS_TYPE_TOS:
            if (NULL != pphb_tos[id])
                return pphb_tos[id];
            break;
        case QOS_TYPE_EXP:
            if (NULL != pphb_exp[id])
                return pphb_exp[id];
            break;
        case QOS_TYPE_DSCP:
            if (NULL != pphb_dscp[id])
                return pphb_dscp[id];
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return NULL;
    }

    /*malloc & set*/
    pqosp = (struct qos_phb *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_phb));
    if (NULL == pqosp)
    {
        QOS_LOG_ERR("In function '%s' the memory is insufficient.\n", __func__);
        return NULL;
    }
    memset(pqosp, 0, sizeof(struct qos_phb));

    /*set parameters*/
    ret = qos_phb_set_default_mapping(id, type, pqosp);
    if (ret != 0)
    {
        QOS_LOG_ERR("Qos Phb default mapping set failed.\n");
        XFREE(MTYPE_QOS_ENTRY, pqosp);
        return NULL;
    }
	#if 0
    ret = ipc_send_hal((void *)pqosp, sizeof(struct qos_phb), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_ADD, id);
	#else
	ret = ipc_send_msg_n2((void *)pqosp, sizeof(struct qos_phb), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_ADD, id);
	#endif
    if(ret != 0)
    {
        QOS_LOG_ERR ( "ipc_send_hal errocode=%d PHB_TYPE=%d PHB_ID=%d\n", ret, type, id );
        XFREE(MTYPE_QOS_ENTRY, pqosp);
        return NULL;
    }
    
    /*save qos phb*/
    switch(type)
    {
        case QOS_TYPE_COS:
            pphb_cos[id] = pqosp;
            break;
        case QOS_TYPE_TOS:
            pphb_tos[id] = pqosp;
            break;
        case QOS_TYPE_EXP:
            pphb_exp[id] = pqosp;
            break;
        case QOS_TYPE_DSCP:
            pphb_dscp[id] = pqosp;
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            XFREE(MTYPE_QOS_ENTRY, pqosp);
            return NULL;
    }

    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return pqosp;

}

/*删除一个Qos_phb*/
int qos_phb_delete(uint8_t id, enum QOS_TYPE type)
{
    struct qos_phb *pqosp = NULL;
    int ret = 0;

    QOS_LOG_DBG("Entering function '%s'.\n", __func__);
    
    pqosp = qos_phb_get(id, type);
    if (NULL == pqosp)
    {
        QOS_LOG_ERR("Qos phb with ID%d is not exist!!\n", id);
        return ERRNO_NOT_FOUND;
    }

    /* check whether the qos_domain is applied or not  */
    if( QOS_MAPPING_DETACHED != pqosp->ref_cnt)
    {
        QOS_LOG_ERR("QoS domain is applied to interface, please detach first!!\n");
        return QOS_ERR_ATTACHED;
    }
    #if 0
    ret = ipc_send_hal( &type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_QOS, IPC_TYPE_QOS,
                                      QOS_INFO_PHB, IPC_OPCODE_DELETE, id );
	#else
	ret = ipc_send_msg_n2( &type, sizeof(enum QOS_TYPE), 1, MODULE_ID_HAL, MODULE_ID_QOS, IPC_TYPE_QOS,
                                      QOS_INFO_PHB, IPC_OPCODE_DELETE, id );
	#endif
    if ( ret < 0 )
    {
        QOS_LOG_ERR ("ipc_send_hal errocode=%d,domain_id=%d", ret, id );
        return ERRNO_IPC;
    }

    XFREE(MTYPE_QOS_ENTRY, pqosp);
    /* NULL pdomain_xxx */
    switch(type)
    {
        case QOS_TYPE_COS:
            pphb_cos[id] = NULL;
            break;
        case QOS_TYPE_TOS:
            pphb_tos[id] = NULL;
            break;
        case QOS_TYPE_EXP:
            pphb_exp[id] = NULL;
            break;
        case QOS_TYPE_DSCP:
            pphb_dscp[id] = NULL;
            break;
        default:
            QOS_LOG_ERR(" Invalid Qos Type. \n");
            return ERRNO_PARAM_ILLEGAL;
    }
    
    QOS_LOG_DBG(" Exit function '%s'.\n", __func__);
    return ret;
    
}

/*获取一个Qos_phb的信息*/
struct qos_phb *qos_phb_get(uint8_t id, enum QOS_TYPE type)
{
    struct qos_phb *pqosp = NULL;

    /*return specify type qos_domain*/
    switch(type)
    {
        case QOS_TYPE_COS:
            pqosp = pphb_cos[id];
            break;
        case QOS_TYPE_TOS:
            pqosp = pphb_tos[id];
            break;
        case QOS_TYPE_EXP:
            pqosp = pphb_exp[id];
            break;
        case QOS_TYPE_DSCP:
            pqosp = pphb_dscp[id];
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return NULL;
    }

    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return pqosp;

}

/*在指定Qos_phb中添加一组映射关系*/
int qos_phb_add_mapping(uint8_t id, enum QOS_TYPE type, struct qos_map *pmap)
{
    int ret = 0;
    int pindex = 0;
    struct qos_phb sphb;
    struct qos_phb *pphb = NULL;

	pphb = qos_phb_get(id, type);
	if(NULL == pphb)
    {
    	QOS_LOG_ERR("Qos phb info error.\n");
    	return ERRNO_NOT_FOUND;
    }
    
    if(pphb->ref_cnt)
    {
    	QOS_LOG_ERR("Qos pphb applied, not permit to modify.\n");
    	return QOS_ERR_ATTACHED;
    }
    
    /*use qos_map.queue an qos_map.color to generate the index of pphb.map[]*/
    pindex = pmap->queue * 3 + pmap->color;
    memcpy(&(pphb->map[pindex]), pmap, sizeof(struct qos_map));
    
    memset(&sphb, 0, sizeof(struct qos_phb));
    sphb.id = id;
    sphb.type = type;
    memcpy(&(sphb.map[0]), pmap, sizeof(struct qos_map));        /*sphb.map[0] represent qos_map to add*/
    #if 0       
    ret = ipc_send_hal((void *)(&sphb), sizeof(struct qos_phb), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_UPDATE, id);
	#else
	ret = ipc_send_msg_n2((void *)(&sphb), sizeof(struct qos_phb), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_UPDATE, id);
	#endif
    if(ret != 0)
    {
        QOS_LOG_ERR ("ipc_send_hal errocode=%d PHB_ID=%d\n", ret, id );
        return ERRNO_IPC;
    }
    
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return ret;    
}

int qos_phb_add_all_color_mapping(uint8_t index, enum QOS_TYPE type, uint8_t queue, uint8_t priority)
{
    struct qos_map pmap;
    int ret = 0;

    memset(&pmap, 0, sizeof(struct qos_map));

    pmap.queue    = queue;
    pmap.priority = priority;

    pmap.color = QOS_COLOR_GREEN;
    ret = qos_phb_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
        QOS_LOG_ERR(" Failed to add qos phb mapping. type = %d\n", type);
        return ret;
    }

    pmap.color = QOS_COLOR_YELLOW;
    ret = qos_phb_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
        QOS_LOG_ERR(" Failed to add qos phb mapping. type = %d\n", type);
        return ret;
    }

    pmap.color = QOS_COLOR_RED;
    ret = qos_phb_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
        QOS_LOG_ERR(" Failed to add qos phb mapping. type = %d\n", type);
        return ret;
    }

    return ret;
}


/*删除指定Qos_phb中的一组映射关系*/
int qos_phb_delete_mapping(uint8_t id, enum QOS_TYPE type, uint8_t queue, enum QOS_COLOR color)
{
    int ret = 0;
    uint8_t index = 0;
    struct qos_phb sphb;
    struct qos_phb *pphb = NULL;
    
    pphb = qos_phb_get(id, type);
    if(NULL == pphb)
    {
    	QOS_LOG_ERR("Qos pphb info error.\n");
    	return ERRNO_NOT_FOUND;
    }
    
    if(pphb->ref_cnt)
    {
    	QOS_LOG_ERR("Qos pphb applied, not permit to modify.\n");
    	return QOS_ERR_ATTACHED;
    }

    index = queue * 3 + color;
    
    switch(type)
    {
        case QOS_TYPE_COS:
            memcpy(&sphb, pphb_cos[id], sizeof(struct qos_phb));   /* for Ipc */
            pphb_cos[id]->map[index].priority = queue;
            pphb_cos[id]->map[index].queue = queue;
            pphb_cos[id]->map[index].color = color;
            memcpy(&(sphb.map[0]), &(pphb_cos[id]->map[index]), sizeof(struct qos_map));    /* sphb.map[0] represent the qos_map to be deleted*/
            break;
        case QOS_TYPE_TOS:
            memcpy(&sphb, pphb_tos[id], sizeof(struct qos_phb));
            pphb_tos[id]->map[index].priority = queue;
            pphb_tos[id]->map[index].queue = queue;
            pphb_tos[id]->map[index].color = color;
            memcpy(&(sphb.map[0]), &(pphb_tos[id]->map[index]), sizeof(struct qos_map));
            break;
        case QOS_TYPE_EXP:
            memcpy(&sphb, pphb_exp[id], sizeof(struct qos_phb));
            pphb_exp[id]->map[index].priority = queue;
            pphb_exp[id]->map[index].queue = queue;
            pphb_exp[id]->map[index].color = color;
            memcpy(&(sphb.map[0]), &(pphb_exp[id]->map[index]), sizeof(struct qos_map));
            break;
        case QOS_TYPE_DSCP:
            memcpy(&sphb, pphb_dscp[id], sizeof(struct qos_phb));
            pphb_dscp[id]->map[index].queue = queue;
            pphb_dscp[id]->map[index].color = color;
            if(queue == 0)
            {                    
                pphb_dscp[id]->map[index].priority = 0;            
            }
            else if(queue == 5)
            {
                pphb_dscp[id]->map[index].priority = 46;
            }
            else if(queue == 6)
            {
                pphb_dscp[id]->map[index].priority = 48;
            }
            else if(queue == 7)
            {
                pphb_dscp[id]->map[index].priority = 56;
            }
            else
            {
                pphb_dscp[id]->map[index].priority = 8 * queue + 2 + color * 2;
            }
            memcpy(&(sphb.map[0]), &(pphb_dscp[id]->map[index]), sizeof(struct qos_map));
            break;
        default:
            QOS_LOG_ERR("Invalid Qos Type. \n");
            return ERRNO_PARAM_ILLEGAL;
    }
	#if 0
    ret = ipc_send_hal((void *)(&sphb), sizeof(struct qos_phb), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_UPDATE, id);
    #else
    ret = ipc_send_msg_n2((void *)(&sphb), sizeof(struct qos_phb), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                    IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_UPDATE, id);
    #endif
    if(ret != 0)
    {
        QOS_LOG_ERR ( "ipc_send_hal_wait_ack errocode=%d PHB_ID=%d\n", ret, id );
        return ERRNO_IPC;
    }
    
    QOS_LOG_DBG("Exit function '%s'.\n", __func__);
    return ret;
}

int qos_phb_delete_all_color_mapping(uint8_t index, enum QOS_TYPE type, uint8_t queue)
{
    int ret = 0;

    ret = qos_phb_delete_mapping(index, type, queue, QOS_COLOR_GREEN);
    if (0 != ret)
    {
        QOS_LOG_ERR("Failed to delete qos phb mapping. type = %d\n", type);
        return ret;
    }

    ret = qos_phb_delete_mapping(index, type, queue, QOS_COLOR_YELLOW);
    if (0 != ret)
    {
        QOS_LOG_ERR("Failed to delete qos phb mapping. type = %d\n", type);
        return ret;
    }

    ret = qos_phb_delete_mapping(index, type, queue, QOS_COLOR_RED);
    if (0 != ret)
    {
        QOS_LOG_ERR("Failed to delete qos phb mapping. type = %d\n", type);
        return ret;
    }

    return ret;
}


/************************************************
 * Function: qos_phb_get_bulk
 * Input:   
 *      index: phb profile index
 *		type:  phb profile type
 * Output:
 *      phb_array: phb data 
 * Return:  
 *      data num
 * Description:  
 *      Get qos phb profile data.
 ************************************************/
int qos_phb_get_bulk ( uint8_t index, enum QOS_TYPE type, struct qos_phb *phb_array )
{
    struct qos_phb *pphb = NULL;
    int data_num = 0;
    int i = 0;

    if ( index == 0 )
    {
    	for (i=0; i<QOS_PHB_NUM; i++)
	    {
	    	pphb = qos_phb_get(i, type);
	        if (pphb != NULL)
	        {
	        	memcpy(&phb_array[data_num], pphb, sizeof(struct qos_phb));
				if (0 == pphb->id)
	        	{
					phb_array[data_num].id = QOS_PHB_NUM;
				}

				data_num++;
	        }    
	    }
    }
    else
    {
    	for (i=++index; i<QOS_PHB_NUM; i++)
	    {
	        pphb = qos_phb_get(i, type);
	        if(NULL == pphb)
	        {
	        	continue;
	        }
	        
	        memcpy(&phb_array[data_num], pphb, sizeof(struct qos_phb));
			if (0 == pphb->id)
        	{
				phb_array[data_num].id = QOS_PHB_NUM;
			}

			data_num++;
	    }
    }  

    return data_num;
}




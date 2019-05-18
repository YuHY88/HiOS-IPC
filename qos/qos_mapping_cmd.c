#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qos/qos_mapping.h>
#include <qos/qos_mapping_cmd.h>
#include <lib/command.h>
#include <lib/errcode.h>

static void qos_domain_show_operation(struct vty *vty, struct qos_domain *pdomain)
{
    uint8_t i, num = 0;
    enum QOS_TYPE type;

    type = pdomain->type;

    QOS_DOMAIN_MAPPING_NUM_GET(type,num);

    for(i = 0; i < num; i++)
    {
                                             /* priority */                   /* queue */
        vty_out(vty, " %-5d%2s%-5d%-7s%s", pdomain->map[i].priority, "", pdomain->map[i].queue,
                                       (QOS_COLOR_GREEN == pdomain->map[i].color) ? "green" :
                                       (QOS_COLOR_YELLOW == pdomain->map[i].color) ? "yellow" : "red", VTY_NEWLINE);  /* color */
    }
}

static void qos_phb_show_operation(struct vty *vty, struct qos_phb *pphb)
{
    uint8_t i = 0;

    for(i = 0; i < 8; i++)
    {
        vty_out(vty, "%2s%-5d%-7s%-3s%d%s", "", i, "green", "", pphb->map[i*3].priority, VTY_NEWLINE);
        vty_out(vty, "%-7s%-7s%-3s%d%s", "", "yellow", "", pphb->map[i*3+1].priority, VTY_NEWLINE);
        vty_out(vty, "%-7s%-7s%-3s%d%s", "", "red", "", pphb->map[i*3+2].priority, VTY_NEWLINE);
    }
}


DEFUN(qos_mapping_domain,
        qos_mapping_domain_cmd,
        "qos mapping (cos-domain|tos-domain|exp-domain|dscp-domain) <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Cos to queue mapping domain\n"
        "Tos to queue mapping domain\n"
        "Exp to queue mapping domain\n"
        "Dscp to queue mapping domain\n"
        "QoS mapping domain ID\n")
{

    uint8_t index = 0;
    int type = 0;
    char *pprompt = NULL;
    const char *prom_str = NULL;
    struct qos_domain *pdomain = NULL;

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'e')
    {
        type = QOS_TYPE_EXP;
    }
    else if (argv[0][0] == 'd')
    {
        type = QOS_TYPE_DSCP;
    }

    /*get argv[1]*/
    index = atoi(argv[1]);

    /*save qos domain dis info*/
    vty->index = (void *)(int)index;
    vty->index_sub = (void *)type;

    /*create qos_domain*/
    pdomain = qos_domain_create(index, type);
    if (NULL == pdomain)
    {
        vty_error_out(vty, "Failed to create qos domain.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vty->node = QOS_DOMAIN_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        QOS_MAPPING_PROMPT_GET(type, prom_str);

		if (NULL != prom_str)
		{
        	snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-qos-domain-%s)#", prom_str );
		}
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_mapping_domain,
        no_qos_mapping_domain_cmd,
        "no qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <1-5>",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Cos to queue mapping domain\n"
        "Tos to queue mapping domain\n"
        "Exp to queue mapping domain\n"
        "Dscp to queue mapping domain\n"
        "QoS mapping domain ID\n")
{
    uint8_t index = 0;
    enum QOS_TYPE type = QOS_TYPE_INVALID;
    int ret = 0;

    /*get argv[1]*/
    index = atoi(argv[1]);

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'e')
    {
        type = QOS_TYPE_EXP;
    }
    else if (argv[0][0] == 'd')
    {
        type = QOS_TYPE_DSCP;
    }

    ret = qos_domain_delete(index, type);
    if (ERRNO_NOT_FOUND == ret)
    {
        return CMD_SUCCESS;
    }
    else if(ERRNO_HAL == ret)
    {
        vty_error_out(vty, "Get qos domain attached info failed!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(QOS_ERR_ATTACHED == ret)
    {
        vty_error_out(vty, "QoS domain was applied to interface, please detach first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(ERRNO_IPC == ret)
    {
        vty_error_out(vty, "Delete qos domain failed!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}

DEFUN(qos_cos_to_queue,
        qos_cos_to_queue_cmd,
        "cos <0-7> to queue <0-7> {color (green|yellow|red)}",
        "Class of Service\n"
        "Specify cos value\n"
        "Mapping\n"
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_COS != type)
    {
        vty_error_out(vty, "Please enter a cos domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.priority = atoi(argv[0]);

    /*get argv[1]*/
    pmap.queue = atoi(argv[1]);

    /*check argv[2]*/
    if (NULL == argv[2])
    {
        pmap.color = QOS_COLOR_GREEN;
    }
    else
    {
        switch(argv[2][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }
    }

    /*add cos_to_queue mapping*/
    ret = qos_domain_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
        if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to add cos_to_queue mapping.%s", VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_cos_to_queue,
        no_qos_cos_to_queue_cmd,
        "no cos <0-7>",
        NO_STR
        "Class of Service\n"
        "Specify cos value\n")
{
    uint8_t index, priority;
    enum QOS_TYPE type;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_COS != type)
    {
        vty_error_out(vty, "Please enter a cos domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    priority = atoi(argv[0]);

    /*delete cos_to_queue mapping*/
    ret = qos_domain_delete_mapping(index, type, priority);
    if (0 != ret)
    {
        if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to delete cos_to_queue mapping %d.%s", priority, VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(qos_tos_to_queue,
        qos_tos_to_queue_cmd,
        "tos <0-7> to queue <0-7> {color (green|yellow|red)}",
        "Type of Service\n"
        "Specify tos value\n"
        "Mapping\n"
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_TOS != type)
    {
        vty_error_out(vty, "Please enter a tos domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.priority = atoi(argv[0]);

    /*get argv[1]*/
    pmap.queue = atoi(argv[1]);

    /*check argv[2]*/
    if (NULL == argv[2])
    {
        pmap.color = QOS_COLOR_GREEN;
    }
    else
    {
        switch(argv[2][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }
    }

    /*add tos_to_queue mapping*/
    ret = qos_domain_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
    	if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to add tos_to_queue mapping.%s", VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_tos_to_queue,
        no_qos_tos_to_queue_cmd,
        "no tos <0-7>",
        NO_STR
        "Type of Service\n"
        "Specify tos value\n")
{
    uint8_t index, priority;
    enum QOS_TYPE type;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_TOS != type)
    {
        vty_error_out(vty, "Please enter a tos domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check argv[0]*/
    priority = atoi(argv[0]);

    /*delete tos_to_queue mapping*/
    ret = qos_domain_delete_mapping(index, type, priority);
    if (0 != ret)
    {
    	if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to delete tos_to_queue mapping %d.%s", priority, VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(qos_exp_to_queue,
        qos_exp_to_queue_cmd,
        "exp <0-7> to queue <0-7> {color (green|yellow|red)}",
        "MPLS label experimental bits\n"
        "Specify exp value\n"
        "Mapping\n"
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_EXP != type)
    {
        vty_error_out(vty, "Please enter an exp domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.priority = atoi(argv[0]);

    /*get argv[1]*/
    pmap.queue = atoi(argv[1]);

    /*check argv[2]*/
    if (NULL == argv[2])
    {
        pmap.color = QOS_COLOR_GREEN;
    }
    else
    {
        switch(argv[2][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }
    }

    /*add exp_to_queue mapping*/
    ret = qos_domain_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
    	if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to add exp_to_queue mapping.%s", VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_exp_to_queue,
        no_qos_exp_to_queue_cmd,
        "no exp <0-7>",
        NO_STR
        "MPLS label experimental bits\n"
        "Specify exp value\n")
{
    uint8_t index, priority;
    enum QOS_TYPE type;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_EXP != type)
    {
        vty_error_out(vty, "Please enter an exp domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check argv[0]*/
    priority = atoi(argv[0]);

    /*delete exp_to_queue mapping*/
    ret = qos_domain_delete_mapping(index, type, priority);
    if (0 != ret)
    {
    	if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to delete exp_to_queue mapping %d.%s", priority, VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN(qos_dscp_to_queue,
        qos_dscp_to_queue_cmd,
        "dscp <0-63> to queue <0-7> {color (green|yellow|red)}",
        "Differentiated Services Code Point\n"
        "Specify dscp value\n"
        "Mapping\n"
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_DSCP != type)
    {
        vty_error_out(vty, "Please enter a dscp domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.priority = atoi(argv[0]);

    /*get argv[1]*/
    pmap.queue = atoi(argv[1]);

    /*check argv[2]*/
    if (NULL == argv[2])
    {
        pmap.color = QOS_COLOR_GREEN;
    }
    else
    {
        switch(argv[2][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }
    }

    /*add dscp_to_queue mapping*/
    ret = qos_domain_add_mapping(index, type, &pmap);
    if (0 != ret)
    {
    	if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to add dscp_to_queue mapping.%s", VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_dscp_to_queue,
        no_qos_dscp_to_queue_cmd,
        "no dscp <0-63>",
        NO_STR
        "Differentiated Services Code Point\n"
        "Specify dscp value\n")
{
    uint8_t index, priority;
    enum QOS_TYPE type;
    int ret = 0;

    /*load qos_domain distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_DSCP != type)
    {
        vty_error_out(vty, "Please enter a dscp domain first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    priority = atoi(argv[0]);

    /*delete dscp_to_queue mapping*/
    ret = qos_domain_delete_mapping(index, type, priority);
    if (0 != ret)
    {
    	if(QOS_ERR_ATTACHED == ret)
	    {
	        vty_error_out(vty, "QoS domain was applied, not permit to modify!!%s", VTY_NEWLINE);
	    }
	    else
	    {
	    	vty_error_out(vty, "Failed to delete dscp_to_queue mapping %d.%s", priority, VTY_NEWLINE);
	    }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(show_qos_mapping_domain,
        show_qos_mapping_domain_cmd,
        "show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>",
        "Show qos mapping domain infomation\n"
        "Quality of Service\n"
        "QoS mapping\n"
        "Cos to queue mapping domain\n"
        "Exp to queue mapping domain\n"
        "Tos to queue mapping domain\n"
        "Dscp to queue mapping domain\n"
        "QoS mapping domain ID\n")
{
    uint8_t index;
    enum QOS_TYPE type = QOS_TYPE_INVALID;
    struct qos_domain *pdomain = NULL;

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'e')
    {
        type = QOS_TYPE_EXP;
    }
    else if (argv[0][0] == 'd')
    {
        type = QOS_TYPE_DSCP;
    }

    /*get argv[1]*/
    index = atoi(argv[1]);

    /*get qos_domain information*/
    pdomain = qos_domain_get(index, type);

    if(NULL == pdomain)
    {
        return CMD_SUCCESS;
    }

    /*print qos_domain information*/
    vty_out(vty, "ID: %d", index);
    if(index)
    {
    	vty_out(vty, "  ref_cnt:%d", pdomain->ref_cnt);
    }
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "===================%s", VTY_NEWLINE);
    switch (type)
    {
        case QOS_TYPE_COS:
            vty_out(vty,"%-6s", "cos");
            break;
        case QOS_TYPE_TOS:
            vty_out(vty,"%-6s", "tos");
            break;
        case QOS_TYPE_EXP:
            vty_out(vty,"%-6s", "exp");
            break;
        case QOS_TYPE_DSCP:
            vty_out(vty,"%-6s", "dscp");
            break;
        default:
            return CMD_ERR_NO_MATCH;
    }
    vty_out(vty, "%-7s", "queue");
    vty_out(vty, "%-7s", "color");

    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "-------------------%s", VTY_NEWLINE);

    qos_domain_show_operation(vty, pdomain);

    vty_out(vty, "===================%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(qos_mapping_phb,
        qos_mapping_phb_cmd,
        "qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Queue to cos mapping\n"
        "Queue to exp mapping\n"
        "Queue to tos mapping\n"
        "Queue to dscp mapping\n"
        "QoS mapping phb ID\n")
{
    enum QOS_TYPE type = QOS_TYPE_INVALID;
    char *pprompt = NULL;
    const char *prom_str = NULL;
    uint8_t index;
    struct qos_phb *pphb;

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'e')
    {
        type = QOS_TYPE_EXP;
    }
    else if (argv[0][0] == 'd')
    {
        type = QOS_TYPE_DSCP;
    }

    /*get argv[1]*/
    index = atoi(argv[1]);

    /*save qos phb dis info*/
    vty->index = (void *)(int)index;
    vty->index_sub = (void *)type;

    /*create qos_phb*/
    pphb = qos_phb_create(index, type);
    if (NULL == pphb)
    {
        vty_error_out(vty, "Failed to create qos phb.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vty->node = QOS_PHB_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        QOS_MAPPING_PROMPT_GET(type, prom_str);

		if (NULL != prom_str)
		{
        	snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-qos-phb-%s)#", prom_str );
		}
    }
    return CMD_SUCCESS;
}

DEFUN(no_qos_mapping_phb,
        no_qos_mapping_phb_cmd,
        "no qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Queue to cos mapping\n"
        "Queue to exp mapping\n"
        "Queue to tos mapping\n"
        "Queue to dscp mapping\n"
        "QoS mapping phb ID\n")
{
    enum QOS_TYPE type = QOS_TYPE_INVALID;
    uint8_t index;
    int ret = 0;

    /*get argv[1]*/
    index = atoi(argv[1]);

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'e')
    {
        type = QOS_TYPE_EXP;
    }
    else if (argv[0][0] == 'd')
    {
        type = QOS_TYPE_DSCP;
    }

    ret = qos_phb_delete(index, type);
    if (ERRNO_NOT_FOUND == ret)
    {
        return CMD_SUCCESS;
    }
    else if(ERRNO_HAL == ret)
    {
        vty_error_out(vty, "Get qos phb attached info failed!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(QOS_ERR_ATTACHED == ret)
    {
        vty_error_out(vty, "QoS phb was applied to interface, please detach first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(ERRNO_IPC == ret)
    {
        vty_error_out(vty, "Delete qos phb failed!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(qos_queue_to_cos,
        qos_queue_to_cos_cmd,
        "queue <0-7> {color (green|yellow|red)} to cos <0-7>",
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "Class of Service\n"
        "Specify cos value\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_COS != type)
    {
        vty_error_out(vty, "Please enter a cos phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.queue = atoi(argv[0]);

    /*get argv[2]*/
    pmap.priority = atoi(argv[2]);

    /*check argv[1]*/
    if(NULL == argv[1])     /* config all color */
    {
        ret = qos_phb_add_all_color_mapping(index, type, pmap.queue, pmap.priority);
        if (0 != ret)
        {
        	if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_cos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*add queue_to_cos mapping*/
        ret = qos_phb_add_mapping(index, type, &pmap);
        if (0 != ret)
        {
        	if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_cos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_queue_to_cos,
        no_qos_queue_to_cos_cmd,
        "no queue <0-7> {color (green|yellow|red)} to cos",
        NO_STR
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "Class of Service\n")
{
    uint8_t queue, index;
    enum QOS_TYPE type;
    enum QOS_COLOR color;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_COS != type)
    {
        vty_error_out(vty, "Please enter a cos phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    queue = atoi(argv[0]);

    /*check argv[1]*/
    if(NULL == argv[1])
    {
        ret = qos_phb_delete_all_color_mapping(index, type, queue);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_cos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                color = QOS_COLOR_GREEN;
                break;
            case 'y':
                color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*delete queue_to_cos mapping*/
        ret = qos_phb_delete_mapping(index, type, queue, color);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_cos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(qos_queue_to_exp,
        qos_queue_to_exp_cmd,
        "queue <0-7> {color (green|yellow|red)} to exp <0-7>",
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "MPLS label experimental bits\n"
        "Specify exp value\n")

{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_EXP != type)
    {
        vty_error_out(vty, "Please enter an exp phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.queue = atoi(argv[0]);

    /*check argv[2]*/
    pmap.priority = atoi(argv[2]);

    /*check argv[1]*/
    if(NULL == argv[1])     /* config all color */
    {
        ret = qos_phb_add_all_color_mapping(index, type, pmap.queue, pmap.priority);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_exp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*add queue_to_exp mapping*/
        ret = qos_phb_add_mapping(index, type, &pmap);
        if (0 != ret)
        {
        	if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_exp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_queue_to_exp,
        no_qos_queue_to_exp_cmd,
        "no queue <0-7> {color (green|yellow|red)} to exp",
        NO_STR
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "MPLS label experimental bits\n")
{
    uint8_t queue, index;
    enum QOS_TYPE type;
    enum QOS_COLOR color;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_EXP != type)
    {
        vty_error_out(vty, "Please enter an exp phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    queue = atoi(argv[0]);

    /*check argv[1]*/
    if(NULL == argv[1])
    {
        ret = qos_phb_delete_all_color_mapping(index, type, queue);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_exp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                color = QOS_COLOR_GREEN;
                break;
            case 'y':
                color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*delete queue_to_exp mapping*/
        ret = qos_phb_delete_mapping(index, type, queue, color);
        if (0 != ret)
        {
        	if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_exp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(qos_queue_to_tos,
        qos_queue_to_tos_cmd,
        "queue <0-7> {color (green|yellow|red)} to tos <0-7>",
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "Type of Service\n"
        "Specify tos value\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_TOS != type)
    {
        vty_error_out(vty, "Please enter a tos phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.queue = atoi(argv[0]);

    /*get argv[2]*/
    pmap.priority = atoi(argv[2]);

    /*check argv[1]*/
    if(NULL == argv[1])     /* config all color */
    {
        ret = qos_phb_add_all_color_mapping(index, type, pmap.queue, pmap.priority);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_tos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*add queue_to_tos mapping*/
        ret = qos_phb_add_mapping(index, type, &pmap);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_tos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;

}

DEFUN(no_qos_queue_to_tos,
        no_qos_queue_to_tos_cmd,
        "no queue <0-7> {color (green|yellow|red)} to tos",
        NO_STR
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "Type of service\n")
{
    uint8_t queue, index;
    enum QOS_COLOR color;
    enum QOS_TYPE type;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_TOS != type)
    {
        vty_error_out(vty, "Please enter a tos phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    queue = atoi(argv[0]);

    /*check argv[1]*/
    if(NULL == argv[1])
    {
        ret = qos_phb_delete_all_color_mapping(index, type, queue);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_tos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                color = QOS_COLOR_GREEN;
                break;
            case 'y':
                color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*delete queue_to_tos mapping*/
        ret = qos_phb_delete_mapping(index, type, queue, color);
        if (0 != ret)
        {
        	if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_tos mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(qos_queue_to_dscp,
        qos_queue_to_dscp_cmd,
        "queue <0-7> {color (green|yellow|red)} to dscp <0-63>",
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "Differentiated Services Code Point\n"
        "Specify dscp value\n")
{
    uint8_t index;
    enum QOS_TYPE type;
    struct qos_map pmap;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_DSCP != type)
    {
        vty_error_out(vty, "Please enter a dscp phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    pmap.queue = atoi(argv[0]);

    /*get argv[2]*/
    pmap.priority = atoi(argv[2]);

    /*check argv[1]*/
    if(NULL == argv[1])
    {
        ret = qos_phb_add_all_color_mapping(index, type, pmap.queue, pmap.priority);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_dscp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                pmap.color = QOS_COLOR_GREEN;
                break;
            case 'y':
                pmap.color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                pmap.color = QOS_COLOR_RED;
                break;
            default:
                break;
        }
        /*add queue_to_dscp mapping*/
        ret = qos_phb_add_mapping(index, type, &pmap);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to add queue_to_dscp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_queue_to_dscp,
        no_qos_queue_to_dscp_cmd,
        "no queue <0-7> {color (green|yellow|red)} to dscp",
        NO_STR
        "Queue\n"
        "Specify queue value\n"
        "Color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Mapping\n"
        "Differentiated Services Code Point\n")
{
    uint8_t queue, index;
    enum QOS_COLOR color;
    enum QOS_TYPE type;
    int ret = 0;

    /*load qos_phb distinguish info*/
    index = (uint8_t)(int)vty->index;
    type = (enum QOS_TYPE)vty->index_sub;

    /*check type*/
    if (QOS_TYPE_DSCP != type)
    {
        vty_error_out(vty, "Please enter a dscp phb first!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get argv[0]*/
    queue = atoi(argv[0]);

    /*check argv[1]*/
    if(NULL == argv[1])
    {
        ret = qos_phb_delete_all_color_mapping(index, type, queue);
        if (0 != ret)
        {
            if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_dscp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }
    else
    {
        switch(argv[1][0])
        {
            case 'g':
                color = QOS_COLOR_GREEN;
                break;
            case 'y':
                color = QOS_COLOR_YELLOW;
                break;
            case 'r':
                color = QOS_COLOR_RED;
                break;
            default:
                return CMD_WARNING;
        }

        /*delete queue_to_dscp mapping*/
        ret = qos_phb_delete_mapping(index, type, queue, color);
        if (0 != ret)
        {
        	if(QOS_ERR_ATTACHED == ret)
		    {
		        vty_error_out(vty, "QoS phb was applied, not permit to modify!!%s", VTY_NEWLINE);
		    }
		    else
		    {
		    	vty_error_out(vty, "Failed to delete queue_to_dscp mapping.%s", VTY_NEWLINE);
		    }
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_qos_mapping_phb,
        show_qos_mapping_phb_cmd,
        "show qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>",
        "Show qos mapping phb infomation\n"
        "Quality of Service\n"
        "QoS mapping\n"
        "Queue to cos mapping\n"
        "Queue to exp mapping\n"
        "Queue to tos mapping\n"
        "Queue to dscp mapping\n"
        "QoS mapping phb ID\n")
{
    uint8_t index;
    enum QOS_TYPE type = QOS_TYPE_INVALID;
    struct qos_phb *pphb = NULL;

    /*check argv[0]*/
    if (argv[0][0] == 'c')
    {
        type = QOS_TYPE_COS;
    }
    else if (argv[0][0] == 't')
    {
        type = QOS_TYPE_TOS;
    }
    else if (argv[0][0] == 'e')
    {
        type = QOS_TYPE_EXP;
    }
    else if (argv[0][0] == 'd')
    {
        type = QOS_TYPE_DSCP;
    }

    /*get argv[1]*/
    index = atoi(argv[1]);

    /*get qos_phb information*/
    pphb = qos_phb_get(index, type);

    if(NULL == pphb)
    {
        return CMD_SUCCESS;
    }

    /*print qos_phb information*/
    vty_out(vty, "ID: %d", index);
    if(index)
    {
    	vty_out(vty, "  ref_cnt: %d", pphb->ref_cnt);
    }
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "=====================%s", VTY_NEWLINE);
    vty_out(vty, "%-7s", "queue");
    vty_out(vty, "%-9s", "color");

    switch (type)
    {
        case QOS_TYPE_COS:
            vty_out(vty,"%-10s", "cos");
            break;
        case QOS_TYPE_TOS:
            vty_out(vty,"%-10s", "tos");
            break;
        case QOS_TYPE_EXP:
            vty_out(vty,"%-10s", "exp");
            break;
        case QOS_TYPE_DSCP:
            vty_out(vty,"%-10s", "dscp");
            break;
        default:
            return CMD_ERR_NO_MATCH;
    }
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "---------------------%s", VTY_NEWLINE);

    qos_phb_show_operation(vty, pphb);

    vty_out(vty, "=====================%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(show_qos_mapping_configure,
        show_qos_mapping_configure_cmd,
        "show qos mapping configure",
        "Show qos mapping infomation\n"
        "Quality of Service\n"
        "QoS mapping\n"
        "Mapping of configured\n")
{
    uint8_t index=0;
	uint8_t range=0;   //range of damain (0-5)
    enum QOS_TYPE type = QOS_TYPE_INVALID;
	struct qos_domain *pdomain = NULL;
	struct qos_phb *pphb = NULL;

	for(type = 1; type <= 4; type++)   //control type of cos-phb
	{
		switch(type)
		{
	        case 1:
				type = QOS_TYPE_COS;break;
	   		case 2:
		        type = QOS_TYPE_TOS;break;
			case 3:
		        type = QOS_TYPE_EXP;break;
			case 4:
	        	type = QOS_TYPE_DSCP;break;
			default:
            	return CMD_ERR_NO_MATCH;
	    }

		for(range = 1; range <= 5; range++)      //control range of domain
		{
		    index = range;
		    /*get qos_domain information*/
		    pdomain = qos_domain_get(index, type);

			if(NULL == pdomain)
			{
				continue;
			}
		    switch (type)
		    {
		        case QOS_TYPE_COS:
		            vty_out(vty,"%-12s", "cos-domain");
		            break;
		        case QOS_TYPE_TOS:
		            vty_out(vty,"%-12s", "tos-domain");
		            break;
		        case QOS_TYPE_EXP:
		            vty_out(vty,"%-12s", "exp-domain");
		            break;
			    case QOS_TYPE_DSCP:
		            vty_out(vty,"%-12s", "dscp-domain");
		            break;
		        default:
		            return CMD_ERR_NO_MATCH;
		    }
			vty_out(vty, "%-7d",pdomain->id);

		    vty_out(vty, "%s", VTY_NEWLINE);
		}
	}

	for(type = 1; type <= 4; type++)   //control type of cos-phb
	{
		switch(type)
		{
			case 1:
				type = QOS_TYPE_COS;break;
			case 2:
				type = QOS_TYPE_TOS;break;
			case 3:
				type = QOS_TYPE_EXP;break;
			case 4:
				type = QOS_TYPE_DSCP;break;
			default:
				return CMD_ERR_NO_MATCH;
		}

		for(range = 1; range <= 5; range++) 	 //control range of domain
		{
			index = range;
			/*get qos_phb information*/
			pphb = qos_phb_get(index, type);

			if(NULL == pphb)
			{
				continue;
			}
			switch (type)
			{
				case QOS_TYPE_COS:
					vty_out(vty,"%-12s", "cos-phb");
					break;
				case QOS_TYPE_TOS:
					vty_out(vty,"%-12s", "tos-phb");
					break;
				case QOS_TYPE_EXP:
					vty_out(vty,"%-12s", "exp-phb");
					break;
				case QOS_TYPE_DSCP:
					vty_out(vty,"%-12s", "dscp-phb");
					break;
				default:
					return CMD_ERR_NO_MATCH;
			}
			vty_out(vty, "%-7d",pphb->id);

			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}
	return CMD_SUCCESS;
}

static int qos_mapping_config_write ( struct vty *vty )
{
    int i = 0, j = 0;
    uint8_t def_q, def_c, tmp_c;   /*default dscp_queue, default dscp_color, temp dscp_color*/
    uint8_t def_p;                 /*default dscp_priority*/

    /*i stands for index of qos_domain/phb*/
    for (i=1; i<QOS_DOMAIN_NUM; i++)
    {
        /*遍历 pdomain_cos*/
        if (NULL != pdomain_cos[i])
        {
            vty_out(vty, "qos mapping cos-domain %d%s", pdomain_cos[i]->id, VTY_NEWLINE);

            /*j stands for index of qos domaim/phb.map*/
            for (j=0; j<8; j++)
            {
                if(pdomain_cos[i]->map[j].queue != pdomain_cos[i]->map[j].priority ||
                    pdomain_cos[i]->map[j].color != QOS_COLOR_GREEN)
                {
                    vty_out(vty, " cos %d to queue %d color %s%s", j, pdomain_cos[i]->map[j].queue,
                                    pdomain_cos[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pdomain_cos[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red", VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_DOMAIN_NUM; i++)
    {
        /*遍历 pdomain_tos*/
        if (NULL != pdomain_tos[i])
        {
            vty_out(vty, "qos mapping tos-domain %d%s", pdomain_tos[i]->id, VTY_NEWLINE);

            /*j stands for index of qos_domaim/phb.map*/
            for (j=0; j<8; j++)
            {
                if(pdomain_tos[i]->map[j].queue != pdomain_tos[i]->map[j].priority ||
                    pdomain_tos[i]->map[j].color != QOS_COLOR_GREEN)
                {
                    vty_out(vty, " tos %d to queue %d color %s%s", j, pdomain_tos[i]->map[j].queue,
                                    pdomain_tos[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pdomain_tos[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red", VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_DOMAIN_NUM; i++)
    {
        /*遍历 pdomain_exp*/
        if (NULL != pdomain_exp[i])
        {
            vty_out(vty, "qos mapping exp-domain %d%s", pdomain_exp[i]->id, VTY_NEWLINE);

            /*j stands for index of qos_domaim/phb.map*/
            for (j=0; j<8; j++)
            {

                if(pdomain_exp[i]->map[j].queue != pdomain_exp[i]->map[j].priority ||
                    pdomain_exp[i]->map[j].color != QOS_COLOR_GREEN)
                {
                    vty_out(vty, " exp %d to queue %d color %s%s", j, pdomain_exp[i]->map[j].queue,
                                    pdomain_exp[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pdomain_exp[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red", VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_DOMAIN_NUM; i++)
    {
        /*遍历 pdomain_dscp*/
        if (NULL != pdomain_dscp[i])
        {
            vty_out(vty, "qos mapping dscp-domain %d%s", pdomain_dscp[i]->id, VTY_NEWLINE);

            /*j stands for index of qos_domaim/phb.map*/
            for (j=0; j<64; j++)
            {
                /*get default mapping*/
                if(((j & 0x01) == 0) && ((j >> 3) > 0) && ((j >> 3) <= 4))
                {
                    def_q = j >> 3;
                    tmp_c = (j >> 1) & 0x3;
                    if(tmp_c == 2)
                        def_c = QOS_COLOR_YELLOW;
                    else if(tmp_c == 3)
                        def_c = QOS_COLOR_RED;
                    else
                        def_c = QOS_COLOR_GREEN;
                }
                else if(j == 40 || j == 46)
                {
                    def_q = 5;
                    def_c = QOS_COLOR_GREEN;
                }
                else if(j == 48)
                {
                    def_q = 6;
                    def_c = QOS_COLOR_GREEN;
                }
                else if(j == 56)
                {
                    def_q = 7;
                    def_c = QOS_COLOR_GREEN;
                }
                else
                {
                    def_q = 0;
                    def_c = QOS_COLOR_GREEN;
                }

                if(pdomain_dscp[i]->map[j].queue != def_q ||
                    pdomain_dscp[i]->map[j].color != def_c)
                {
                    vty_out(vty, " dscp %d to queue %d color %s%s", j, pdomain_dscp[i]->map[j].queue,
                                    pdomain_dscp[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pdomain_dscp[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red", VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_PHB_NUM; i++)
    {
        /*遍历 pphb_cos*/
        if (NULL != pphb_cos[i])
        {
            vty_out(vty, "qos mapping cos-phb %d%s", pphb_cos[i]->id, VTY_NEWLINE);

            /*j stands for index of qos domaim/phb.map*/
            for (j=0; j<24; j++)
            {
                if(pphb_cos[i]->map[j].priority != j/3)
                {
                    vty_out(vty, " queue %d color %s to cos %d%s", pphb_cos[i]->map[j].queue,
                                    pphb_cos[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pphb_cos[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red",
                                        pphb_cos[i]->map[j].priority, VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_PHB_NUM; i++)
    {
        /*遍历 pphb_tos*/
        if (NULL != pphb_tos[i])
        {
            vty_out(vty, "qos mapping tos-phb %d%s", pphb_tos[i]->id, VTY_NEWLINE);

            /*j stands for index of qos domaim/phb.map*/
            for (j=0; j<24; j++)
            {

                if(pphb_tos[i]->map[j].priority != j/3)
                {
                    vty_out(vty, " queue %d color %s to tos %d%s", pphb_tos[i]->map[j].queue,
                                    pphb_tos[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pphb_tos[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red",
                                        pphb_tos[i]->map[j].priority, VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_PHB_NUM; i++)
    {
        /*遍历 pphb_exp*/
        if (NULL != pphb_exp[i])
        {
            vty_out(vty, "qos mapping exp-phb %d%s", pphb_exp[i]->id, VTY_NEWLINE);

            /*j stands for index of qos domaim/phb.map*/
            for (j=0; j<24; j++)
            {

                if(pphb_exp[i]->map[j].priority != j/3)
                {
                    vty_out(vty, " queue %d color %s to exp %d%s", pphb_exp[i]->map[j].queue,
                                    pphb_exp[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pphb_exp[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red",
                                        pphb_exp[i]->map[j].priority, VTY_NEWLINE);
                }
            }
        }
    }

    for (i=1; i<QOS_PHB_NUM; i++)
    {
        /*遍历 pphb_dscp*/
        if (NULL != pphb_dscp[i])
        {
            vty_out(vty, "qos mapping dscp-phb %d%s", pphb_dscp[i]->id, VTY_NEWLINE);

            /*j stands for index of qos domaim/phb.map*/
            for (j=0; j<24; j++)
            {


                /*get default mapping*/
                if(pphb_dscp[i]->map[j].queue == 0)
                {
                    def_p = 0;
                }
                else if(pphb_dscp[i]->map[j].queue == 5)
                {
                    def_p = 46;
                }
                else if(pphb_dscp[i]->map[j].queue == 6)
                {
                    def_p = 48;
                }
                else if(pphb_dscp[i]->map[j].queue == 7)
                {
                    def_p = 56;
                }
                else
                {
                    def_p = 8 * pphb_dscp[i]->map[j].queue + 2 + pphb_dscp[i]->map[j].color * 2;
                }

                if(pphb_dscp[i]->map[j].priority != def_p)
                {
                    vty_out(vty, " queue %d color %s to dscp %d%s", pphb_dscp[i]->map[j].queue,
                                    pphb_dscp[i]->map[j].color == QOS_COLOR_GREEN ? "green" :
                                    pphb_dscp[i]->map[j].color == QOS_COLOR_YELLOW ? "yellow" : "red",
                                        pphb_dscp[i]->map[j].priority, VTY_NEWLINE);
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


static struct cmd_node qos_domain_node =
{
    QOS_DOMAIN_NODE,
    "%s(config-qos-domain)# ",
    1
};


static struct cmd_node qos_phb_node =
{
    QOS_PHB_NODE,
    "%s(config-qos-phb)# ",
    1
};


void qos_mapping_cmd_init(void)
{
    /*install node*/
    install_node(&qos_domain_node, qos_mapping_config_write);
    install_node(&qos_phb_node, NULL);

    /*install default*/
    install_default(QOS_DOMAIN_NODE);
    install_default(QOS_PHB_NODE);

    /*install element "qos_domain related"*/
    install_element(CONFIG_NODE, &qos_mapping_domain_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_qos_mapping_domain_cmd, CMD_SYNC);

    install_element(QOS_DOMAIN_NODE, &qos_cos_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &no_qos_cos_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &qos_tos_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &no_qos_tos_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &qos_exp_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &no_qos_exp_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &qos_dscp_to_queue_cmd, CMD_SYNC);
    install_element(QOS_DOMAIN_NODE, &no_qos_dscp_to_queue_cmd, CMD_SYNC);

    /*install element "qos_phb related"*/
    install_element(CONFIG_NODE, &qos_mapping_phb_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_qos_mapping_phb_cmd, CMD_SYNC);

    install_element(QOS_PHB_NODE, &qos_queue_to_cos_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &no_qos_queue_to_cos_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &qos_queue_to_tos_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &no_qos_queue_to_tos_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &qos_queue_to_exp_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &no_qos_queue_to_exp_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &qos_queue_to_dscp_cmd, CMD_SYNC);
    install_element(QOS_PHB_NODE, &no_qos_queue_to_dscp_cmd, CMD_SYNC);

    /*install show cmd*/
    install_element(CONFIG_NODE, &show_qos_mapping_domain_cmd, CMD_LOCAL);
    install_element(QOS_DOMAIN_NODE, &show_qos_mapping_domain_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &show_qos_mapping_phb_cmd, CMD_LOCAL);
    install_element(QOS_PHB_NODE, &show_qos_mapping_phb_cmd, CMD_LOCAL);

	install_element(CONFIG_NODE, &show_qos_mapping_configure_cmd, CMD_LOCAL);

}


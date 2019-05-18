#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/command.h>
#include <sys/socket.h>
#include <lib/sockunion.h>
#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>


#include "snmp/include/snmp_config_table.h"
#include "system_group.h"

extern struct thread_master *snmp_cli_master;
extern void update_config(void);
extern void snmpd_delete_trapsinks(void);

/* check calidity of IP */
int snmp_check_input_ip_valid(int ip)
{
    unsigned int a1 = 0, a2 = 0, a3 = 0, a4 = 0;

    a1 = (ip >> 24) & 0xff;
    a2 = (ip >> 16) & 0xff;
    a3 = (ip >> 8)  & 0xff;
    a4 = ip         & 0xff;

    if (0 == a1)    //0.x.x.x invalib
    {
        return SNMP_ERROR;
    }
    else if (255 == a1 && 255 == a2 && 255 == a3 && 255 == a4) //255.255.255.255 invalib
    {
        return SNMP_ERROR;
    }
    else if (a1 >= 224 && a1 <= 239) //224.0.0.0 ~ 239.255.255.255 invalib
    {
        return SNMP_ERROR;
    }
    else if (a1 >= 240 && a1 <= 255) //240.0.0.0 ~ 255.255.255.255 invalib
    {
        return SNMP_ERROR;/*experimental address*/
    }
    else if (127 == a1)             //127.x.x.x invalib
    {
        return SNMP_ERROR;
    }

    return SNMP_OK;
}


/*
 * Function: snmp_check_legal_of_char
 *
 * Purpose: check legal of input charactors
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_check_legal_of_char(char *p, unsigned int length)
{
#if 0	//cancel check, if need, open it
    while (length--)
    {
        if (((*p >= '0') && (*p <= '9')) || \
                ((*p >= 'a') && (*p <= 'z')) || \
                ((*p >= 'A') && (*p <= 'Z')))
        {
            p++;
        }
        else
        {
            //printf("illegal char\n");
            return SNMP_ERROR;
        }
    }
#endif

    return SNMP_OK;
}

/*
 * Function: snmp_config_item_renew
 *
 * Purpose: renew snmp config items
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_config_item_renew(void)
{
    update_config();
    snmpd_delete_trapsinks();
    //snmp_init_config();

    switch (snmp_version)
    {
        case V1:
            snmp_community_item_config_all();
            snmp_trap_item_config_all();
            break;

        case V2C:
            snmp_community_item_config_all();
            snmp_trap_item_config_all();
            //snmp_inform_item_config_all();
            break;

        case V3:
            snmp_trap_item_config_all();
            snmp_user_item_config_all();
            break;

        case ALL:
            snmp_community_item_config_all();
            snmp_user_item_config_all();
            snmp_trap_item_config_all();
            //snmp_inform_item_config_all();
            break;

        default:
            break;
    }

    return SNMP_OK;
}


static struct cmd_node snmp_node =
{
    SNMPD_NODE,
    "%s(config-snmp)# ",
    1
};

DEFUN(snmp_mode_enable,
      snmp_mode_enable_cmd,
      "snmp",
      "snmp command node\n")
{
    vty->node = SNMPD_NODE;
    return CMD_SUCCESS;
}

DEFUN(snmp_version_config_fun,
      snmp_version_config_cmd ,
      "snmp version (v1|v2c|v3|all)",
      "snmp\n"
      "version of snmp\n"
      "version v1\n"
      "version v2c\n"
      "version v3\n"
      "version v1/v2c/v3\n")
{
    SNMP_VERSION snmp_version_temp = snmp_version;  //record the old version

    if (strcmp(argv[0], "v1") == 0)     //check the input version
    {
        snmp_version = V1;
    }
    else if (strcmp(argv[0], "v2c") == 0)
    {
        snmp_version = V2C;
    }
    else if (strcmp(argv[0], "v3") == 0)
    {
        snmp_version = V3;
    }
    else if (strcmp(argv[0], "all") == 0)
    {
        snmp_version = ALL;
    }

    if (snmp_version_temp != snmp_version)  //if version has been changed
    {
        snmp_config_item_renew();
    }

    return CMD_SUCCESS;
}

DEFUN(no_snmp_version_config_fun,
      no_snmp_version_config_cmd ,
      "no snmp version",
      "delete\n"
      "snmp\n"
      "version of snmp\n")
{
    if (ALL != snmp_version)
    {
        snmp_version = ALL;
        snmp_config_item_renew();
    }

    return CMD_SUCCESS;
}

/* ip唯一确定一个服务器，因为发送trap时是按照ip发送的 */
DEFUN(snmp_v1v2c_server_config_fun,
      snmp_v1v2c_server_config_cmd ,
      "snmp server A.B.C.D {udp-port <1-65535> | vpn <1-128>} version (v1|v2c) community WORD",
      "snmp\n"
      "snmp server\n"
      "snmp server IP\n"
      "server port\n"
      "port number\n"
      "server vpn\n"
      "vpn number\n"
      "version of snmp\n"
      "version v1\n"
      "version v2c\n"
      "community item\n"
      "community name <1-31>\n")
{
    uint32_t    ipv4;
    uint32_t    port;
    uint32_t    vpn;
    TRAP_ITEM   trap_item;
    //INFORM_ITEM inform_item;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support v1/v2c config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_trap_list->count >= ITEM_SERVER_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of server, max %d%s", ITEM_SERVER_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* init trap_item */
    memset(trap_item.ip_addr.addr, 0, 4 * sizeof(uint32_t));
    memset(trap_item.name, '\0', TRAP_NAME_MAX_LEN);

    /* init inform_item */
    //memset(inform_item.ip_addr.addr, 0, 4*sizeof(uint32_t));
    //memset(inform_item.name, '\0', TRAP_NAME_MAX_LEN);

    if (inet_pton(AF_INET, argv[0], &ipv4) != 1)
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ipv4 = ntohl(ipv4);

    if (SNMP_ERROR == snmp_check_input_ip_valid(ipv4))
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[1])
    {
        port = (uint32_t)atoi(argv[1]);

        if ((port < 1) || (port > 65535))
        {
            vty_error_out(vty, "The input port is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.port = port;
        //inform_item.port = port;
    }
    else
    {
        trap_item.port = 162;
        //inform_item.port = 162;
    }

    if (NULL != argv[2])
    {
        vpn = (uint32_t)atoi(argv[2]);

        if ((vpn < 1) || (vpn > 1024))
        {
            vty_error_out(vty, "The input vpn is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.vpn = vpn;
        //inform_item.vpn = vpn;
    }
    else
    {
        trap_item.vpn = 0;
        //inform_item.vpn = 0;
    }

    if (strlen(argv[4]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* put value into trap_item */
    trap_item.ip_addr.type = IPv4;
    trap_item.ip_addr.addr[0] = ipv4;
    strcpy(trap_item.name, argv[4]);

    /* put value into inform_item */
    //inform_item.ip_addr.type = IPv4;
    //inform_item.ip_addr.addr[0] = ipv4;
    //strcpy(inform_item.name, argv[4]);

    if (strcmp(argv[3], "v1") == 0)
    {
        if (V2C == snmp_version)
        {
            vty_error_out(vty, "Current version[v2c] does not support v1 config%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.version = V1;

        if (NULL == snmp_config_item_get(SNMP_TRAP_ITEM, &trap_item))
        {
            snmp_config_item_add(SNMP_TRAP_ITEM, &trap_item);
            snmp_trap_item_config_new(&trap_item);
        }
        else
        {
            vty_error_out(vty, "Server %s already exist%s" , argv[0], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (strcmp(argv[3], "v2c") == 0)
    {
        if (V1 == snmp_version)
        {
            vty_error_out(vty, "Current version[v1] does not support v2c config%s" , VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.version = V2C;

        if (NULL == snmp_config_item_get(SNMP_TRAP_ITEM, &trap_item))
        {
            snmp_config_item_add(SNMP_TRAP_ITEM, &trap_item);
            snmp_trap_item_config_new(&trap_item);
        }
        /*if(NULL == snmp_config_item_get(SNMP_INFORM_ITEM, &inform_item))
        {
            snmp_config_item_add(SNMP_INFORM_ITEM, &inform_item);
            snmp_inform_item_config_new(&inform_item);
        }*/
        else
        {
            vty_error_out(vty, "Server %s already exist%s" , argv[0], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_snmp_v1v2c_server_config_fun,
      no_snmp_v1v2c_server_config_cmd ,
      "no snmp server A.B.C.D {vpn <1-128>} version (v1|v2c) community WORD",
      "delete\n"
      "snmp\n"
      "snmp server\n"
      "snmp server IP\n"
      "server vpn\n"
      "vpn number 1-1024\n"
      "version of snmp\n"
      "version v1\n"
      "version v2c\n"
      "community item\n"
      "community name <1-31>\n")
{
    uint32_t    ipv4;
    uint32_t    vpn;
    TRAP_ITEM   trap_item;
    //INFORM_ITEM inform_item;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support v1/v2c config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(trap_item.name, '\0', TRAP_NAME_MAX_LEN);
    memset(trap_item.ip_addr.addr, 0, 4 * sizeof(uint32_t));
    //memset(inform_item.name, '\0', TRAP_NAME_MAX_LEN);
    //memset(inform_item.ip_addr.addr, 0, 4*sizeof(uint32_t));

    if (inet_pton(AF_INET, argv[0], &ipv4) != 1)
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ipv4 = ntohl(ipv4);

    if (SNMP_ERROR == snmp_check_input_ip_valid(ipv4))
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[3]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[1])
    {
        vpn = (uint32_t)atoi(argv[1]);

        if ((vpn < 1) || (vpn > 1024))
        {
            vty_error_out(vty, "The input vpn is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.vpn = vpn;
        //inform_item.vpn = vpn;
    }
    else
    {
        trap_item.vpn = 0;
        //inform_item.vpn = 0;
    }

    trap_item.ip_addr.type = IPv4;
    trap_item.ip_addr.addr[0] = ipv4;
    strcpy(trap_item.name, argv[3]);

    //inform_item.ip_addr.type = IPv4;
    //inform_item.ip_addr.addr[0] = ipv4;
    //strcpy(inform_item.name, argv[3]);

    if ((strcmp(argv[2], "v1") == 0))
    {
        if (V2C == snmp_version)
        {
            vty_error_out(vty, "Current version[v2c] does not support v1 config%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.version = V1;
    }
    else if (strcmp(argv[2], "v2c") == 0)
    {
        if (V1 == snmp_version)
        {
            vty_error_out(vty, "Current version[v1] does not support v2c config%s" , VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.version = V2C;
    }

    if (SNMP_OK == snmp_config_item_del(SNMP_TRAP_ITEM, &trap_item))
    {
        snmp_config_item_renew();
    }
    else
    {
        vty_error_out(vty, "Community does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(snmp_v3_server_config_fun,
      snmp_v3_server_config_cmd ,
      "snmp server A.B.C.D {udp-port <1-65535> | vpn <1-128>} version v3 user WORD",
      "snmp\n"
      "snmp server\n"
      "snmp server IP\n"
      "server port\n"
      "port number\n"
      "server vpn\n"
      "vpn number\n"
      "version of snmp\n"
      "version v3\n"
      "user item\n"
      "user name <1-31>\n")
{
    uint32_t    ipv4;
    uint32_t    port;
    uint32_t    vpn;
    TRAP_ITEM   trap_item;
    USER_ITEM   user_item;

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_trap_list->count >= ITEM_SERVER_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of server, max %d%s", ITEM_SERVER_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (inet_pton(AF_INET, argv[0], &ipv4) != 1)
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ipv4 = ntohl(ipv4);

    if (SNMP_ERROR == snmp_check_input_ip_valid(ipv4))
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(trap_item.name, '\0', TRAP_NAME_MAX_LEN);
    memset(trap_item.ip_addr.addr, 0, 4 * sizeof(uint32_t));

    memset(user_item.name, '\0', USER_NAME_MAX_LEN);
    strcpy(user_item.name, argv[3]);

    if (NULL == snmp_config_item_get(SNMP_USER_ITEM, &user_item))
    {
        vty_error_out(vty, "No such user.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[1])
    {
        port = (uint32_t)atoi(argv[1]);

        if ((port < 1) || (port > 65535))
        {
            vty_error_out(vty, "The input port is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.port = port;
    }
    else
    {
        trap_item.port = 162;
    }

    if (NULL != argv[2])
    {
        vpn = (uint32_t)atoi(argv[2]);

        if ((vpn < 1) || (vpn > 1024))
        {
            vty_error_out(vty, "The input vpn is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.vpn = vpn;
    }
    else
    {
        trap_item.vpn = 0;
    }

    if (strlen(argv[3]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "User name is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    trap_item.ip_addr.type = IPv4;
    trap_item.ip_addr.addr[0] = ipv4;
    strcpy(trap_item.name, argv[3]);
    trap_item.version = V3;

    if (NULL == snmp_config_item_get(SNMP_TRAP_ITEM, &trap_item))
    {
        snmp_config_item_add(SNMP_TRAP_ITEM, &trap_item);
        snmp_trap_item_config_new(&trap_item);
    }
    else
    {
        vty_error_out(vty, "Server %s already exist%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_snmp_v3_server_config_fun,
      no_snmp_v3_server_config_cmd ,
      "no snmp server A.B.C.D {vpn <1-128>} version v3 user WORD",
      "delete\n"
      "snmp\n"
      "snmp server\n"
      "snmp server IP\n"
      "server vpn\n"
      "vpn number\n"
      "version of snmp\n"
      "version v3\n"
      "user item\n"
      "user name <1-31> \n")
{
    uint32_t    ipv4;
    uint32_t    vpn;
    TRAP_ITEM   item;

    memset(item.name, '\0', TRAP_NAME_MAX_LEN);
    memset(item.ip_addr.addr, 0, 4 * sizeof(uint32_t));

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (inet_pton(AF_INET, argv[0], &ipv4) != 1)
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ipv4 = ntohl(ipv4);

    if (SNMP_ERROR == snmp_check_input_ip_valid(ipv4))
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[2]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[1])
    {
        vpn = (uint32_t)atoi(argv[1]);

        if ((vpn < 1) || (vpn > 1024))
        {
            vty_error_out(vty, "The input vpn is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        item.vpn = vpn;
    }
    else
    {
        item.vpn = 0;
    }

    item.ip_addr.type = IPv4;
    item.ip_addr.addr[0] = ipv4;
    item.version = V3;
    strcpy(item.name, argv[2]);

    if (SNMP_OK == snmp_config_item_del(SNMP_TRAP_ITEM, &item))
    {
        snmp_config_item_renew();
    }
    else
    {
        vty_error_out(vty, "Target does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(snmp_community_config_fun,
      snmp_community_config_cmd ,
      "snmp community WORD level (ro|rw)",
      "snmp\n"
      "snmp community\n"
      "community name <1-31>\n"
      "level\n"
      "read only\n"
      "read and write\n")
{
    COMMUNITY_ITEM item;

    memset(&item, 0, sizeof(COMMUNITY_ITEM));
    memset(item.name, '\0', COMMUNITY_NAME_MAX_LEN);
    item.attribute = RO;
    item.acl_num = 0;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support community config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_community_list->count >= ITEM_COMMUNITY_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of community, max %d%s", ITEM_COMMUNITY_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > COMMUNITY_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , COMMUNITY_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    //cannot same with init community
    if (strcmp(argv[0], "hios") == 0)
    {
        vty_error_out(vty, "Reserved keywords[%s], forbidden to use%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strcmp(argv[1], "ro") == 0)
    {
        item.attribute = RO;
    }
    else
    {
        item.attribute = RW;
    }

    strcpy(item.name, argv[0]);

    if (NULL == snmp_config_item_get(SNMP_COMMUNITY_ITEM, &item))
    {
        snmp_config_item_add(SNMP_COMMUNITY_ITEM, &item);
        snmp_community_item_config_new(&item);
    }
    else
    {
        vty_error_out(vty, "Community %s already exist%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_snmp_community_config_fun,
      no_snmp_community_config_cmd ,
      "no snmp community WORD",
      "delete\n"
      "snmp\n"
      "snmp community\n"
      "community name <1-31>\n")
{
    COMMUNITY_ITEM  item;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support community config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > COMMUNITY_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , COMMUNITY_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL == argv[0])
    {
        vty_error_out(vty, "Please input a valiable community name%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > COMMUNITY_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community name is too long[max %d] %s" , COMMUNITY_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(item.name, '\0', COMMUNITY_NAME_MAX_LEN);
    strcpy(item.name, argv[0]);

    if (SNMP_OK == snmp_config_item_del(SNMP_COMMUNITY_ITEM, &item))
    {
        snmp_config_item_renew();
    }
    else
    {
        vty_error_out(vty, "Target does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(snmp_user_config_fun,
      snmp_user_config_cmd ,
      "snmp user WORD",
      "snmp\n"
      "snmp user\n"
      "user name <1-31>\n")
{
    USER_ITEM item;
    USER_ITEM *item_get = NULL;
    memset(item.name, '\0', TRAP_NAME_MAX_LEN);
    item.permission = GUEST;
    item.auth_method = NO_AUTH;
    item.encry_method = NO_ENCRY;
    memset(item.auth_secret, '\0', AUTH_SECRET_MAX_LEN);
    memset(item.encry_secret, '\0', ENCRY_SECRET_MAX_LEN);

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_user_list->count >= ITEM_USER_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of user, max %d%s", ITEM_USER_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check illegal characters*/
    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    //cannot same with init user
    if (strcmp(argv[0], "hios") == 0)       //add snmp version
    {
        vty_error_out(vty, "Reserved keywords[%s], forbidden to use%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "User name is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    strcpy(item.name, argv[0]);                 //add user name

    item_get = (USER_ITEM *)snmp_config_item_get(SNMP_USER_ITEM, &item);

    if (NULL == item_get)
    {
        snmp_config_item_add(SNMP_USER_ITEM, &item);
    }
    else
    {
        //strcpy(item_get->name, item.name);
        //item_get->permission = item.permission;
        item_get->auth_method = item.auth_method;
        item_get->encry_method = item.encry_method;
        strcpy(item_get->auth_secret, item.auth_secret);
        strcpy(item_get->encry_secret, item.encry_secret);
    }

    snmp_user_item_config_new(&item);
    return CMD_SUCCESS;
}

DEFUN(snmp_user_config2_fun,
      snmp_user_config2_cmd ,
      "snmp user WORD auth (md5|sha) auth-password WORD {priv des priv-password WORD}",
      "snmp\n"
      "snmp user\n"
      "user name<1-31>\n"
      "auth protocol\n"
      "md5\n"
      "sha\n"
      "auth-password<8-31>\n"
      "password\n"
      "priv protocol\n"
      "des\n"
      "priv-password<8-31>\n"
      "password\n")
{
    USER_ITEM item;
    USER_ITEM *item_get = NULL;
    memset(item.name, '\0', TRAP_NAME_MAX_LEN);
    item.permission = GUEST;
    item.auth_method = NO_AUTH;
    item.encry_method = NO_ENCRY;
    memset(item.auth_secret, '\0', AUTH_SECRET_MAX_LEN);
    memset(item.encry_secret, '\0', ENCRY_SECRET_MAX_LEN);

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_user_list->count >= ITEM_USER_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of user, max %d%s", ITEM_USER_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check illegal characters*/
    if ((SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0]))) || \
            (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[2], strlen(argv[2]))))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[3])
    {
        if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[3], strlen(argv[3])))
        {
            vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    //cannot same with init user
    if (strcmp(argv[0], "hios") == 0)       //add snmp version
    {
        vty_error_out(vty, "Reserved keywords[%s], forbidden to use%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "User name is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    strcpy(item.name, argv[0]);                 //add user name

    if (NULL == argv[1])
    {
        item.auth_method = NO_AUTH;
    }
    else if (strcmp(argv[1], "md5") == 0)           //add user auth_method
    {
        item.auth_method = MD5;
    }
    else if (strcmp(argv[1], "sha") == 0)
    {
        item.auth_method = SHA;
    }

    if (NULL != argv[2])
    {
        if (strlen(argv[2]) < AUTH_SECRET_MIN_LEN)      //auth_secret (8 ~ 50)
        {
            vty_error_out(vty, "Auth secret is too short[min %d] %s" , AUTH_SECRET_MIN_LEN, VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (strlen(argv[2]) > AUTH_SECRET_MAX_LEN)
        {
            vty_error_out(vty, "Auth secret is too long[max %d] %s" , AUTH_SECRET_MAX_LEN, VTY_NEWLINE);
            return CMD_WARNING;
        }

        strcpy(item.auth_secret, argv[2]);          //add user auth_secret
    }

    if (NULL != argv[3])
    {
        item.encry_method = DES;

        if (strlen(argv[3]) < AUTH_SECRET_MIN_LEN)      //auth_secret (8 ~ 50)
        {
            vty_error_out(vty, "Auth secret is too short[min %d] %s" , AUTH_SECRET_MIN_LEN, VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (strlen(argv[3]) > AUTH_SECRET_MAX_LEN)
        {
            vty_error_out(vty, "Auth secret is too long[max %d] %s" , AUTH_SECRET_MAX_LEN, VTY_NEWLINE);
            return CMD_WARNING;
        }

        strcpy(item.encry_secret, argv[3]);
    }

    item_get = (USER_ITEM *)snmp_config_item_get(SNMP_USER_ITEM, &item);

    if (NULL == item_get)
    {
        snmp_config_item_add(SNMP_USER_ITEM, &item);
    }
    else
    {
        //strcpy(item_get->name, item.name);
        //item_get->permission = item.permission;
        item_get->auth_method = item.auth_method;
        item_get->encry_method = item.encry_method;
        strcpy(item_get->auth_secret, item.auth_secret);
        strcpy(item_get->encry_secret, item.encry_secret);
    }

    snmp_user_item_config_new(&item);
    return CMD_SUCCESS;
}

DEFUN(no_snmp_user_config_fun,
      no_snmp_user_config_cmd ,
      "no snmp user WORD",
      "delete\n"
      "snmp\n"
      "snmp user\n"
      "user name <1-31>\n")
{
    USER_ITEM item;
    TRAP_ITEM trap_item;

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(item.name, '\0', USER_NAME_MAX_LEN);
    memset(trap_item.name, '\0', TRAP_NAME_MAX_LEN);
    trap_item.version = V3;

    if (strlen(argv[0]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "User name is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    strcpy(item.name, argv[0]);

    if (SNMP_OK == snmp_config_item_del(SNMP_USER_ITEM, &item))
    {
        /* delete trap v3 */
        strcpy(trap_item.name, argv[0]);
        snmp_trap_v3_del(&trap_item);
        snmp_config_item_renew();
    }
    else
    {
        vty_error_out(vty, "User does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(snmp_user_privilege_config_fun,
      snmp_user_privilege_config_cmd,
      "snmp user WORD privilege (guest|operator|admin)",
      "snmp\n"
      "snmp user\n"
      "user name <1-31>\n"
      "user privilege\n"
      "guest privilege\n"
      "operator privilege\n"
      "admin privilege\n")
{
    USER_ITEM item;
    USER_ITEM *item_get = NULL;

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(item.name, '\0', USER_NAME_MAX_LEN);

    if (strlen(argv[0]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "User name is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    strcpy(item.name, argv[0]);
    item_get = snmp_config_item_get(SNMP_USER_ITEM, &item);

    if (NULL == item_get)
    {
        vty_error_out(vty, "User[%s] does not exist %s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
        if (0 == strcmp(argv[1], "guest"))
        {
            if (GUEST != item_get->permission)
            {
                item_get->permission = GUEST;
                snmp_config_item_renew();
            }
        }
        else if (0 == strcmp(argv[1], "operator"))
        {
            if (OPERATOR != item_get->permission)
            {
                item_get->permission = OPERATOR;
                snmp_config_item_renew();
            }
        }
        else if (0 == strcmp(argv[1], "admin"))
        {
            if (ADMIN != item_get->permission)
            {
                item_get->permission = ADMIN;
                snmp_config_item_renew();
            }
        }
        else
        {
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_snmp_user_privilege_config_fun,
      no_snmp_user_privilege_config_cmd,
      "no snmp user WORD privilege",
      "delete\n"
      "snmp\n"
      "snmp user\n"
      "user name <1-31>\n"
      "user privilege\n")
{
    USER_ITEM user_item;
    USER_ITEM *user_item_get = NULL;

    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(user_item.name, '\0', USER_NAME_MAX_LEN);

    if (strlen(argv[0]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "User name is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    strcpy(user_item.name, argv[0]);
    user_item_get = (USER_ITEM *)snmp_config_item_get(SNMP_USER_ITEM, &user_item);

    if (NULL == user_item_get)
    {
        vty_error_out(vty, "User[%s] does not exist %s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
        if (GUEST != user_item_get->permission)
        {
            user_item_get->permission = GUEST;
            snmp_config_item_renew();
        }
    }

    return CMD_SUCCESS;
}


DEFUN(show_snmp_config_fun,
      show_snmp_config_cmd,
      "show snmp config",
      SHOW_STR
      "snmp\n"
      "snmp config information\n")
{
    snmp_show_all_items(vty);
    return CMD_SUCCESS;
}

DEFUN(show_snmp_user_fun,
      show_snmp_user_cmd,
      "show snmp user",
      SHOW_STR
      "snmp\n"
      "snmp user information\n")
{
    if ((V1 == snmp_version) || (V2C == snmp_version))
    {
        vty_error_out(vty, "Current version[%s] does not support user config%s" ,
                      (V1 == snmp_version) ? "v1" : "v2c", VTY_NEWLINE);
        return CMD_WARNING;
    }

    snmp_show_items(SNMP_USER_ITEM, vty);
    return CMD_SUCCESS;
}

DEFUN(show_snmp_statistics_fun,
      show_snmp_statistics_cmd,
      "show snmp statistics",
      SHOW_STR
      "snmp\n"
      "snmp packet statistics information\n")
{
    vty_out(vty, "------------snmp statistics------------%s", VTY_NEWLINE);
    vty_out(vty, "snmp send packet statistics     : %d %s" , snmp_packet_statistics.packet_send, VTY_NEWLINE);
    vty_out(vty, "snmp reveived packet statistics : %d %s" , snmp_packet_statistics.packet_received, VTY_NEWLINE);
    return CMD_SUCCESS;
}

/*-----------------------------------added by lidingcheng for rfc1213 system group 2018-04-13----------------------------*/
DEFUN(description_snmp_config_fun,
      description_snmp_config_cmd,
      "description SNMP",
      "description\n"
      "snmp\n")
{
    snmp_show_all_items(vty);
    return CMD_SUCCESS;
}

DEFUN(snmp_sysinfo_contact_func,
      snmp_sysinfo_contact_cmd,
      "snmp-agent sys-info contact WORD ",
      "snmp-agent\n"
      "sys-info\n"
      "contact\n"
      "contact <1-255>\n")
{
    int iRet = 0 ;

    if (strlen(argv[0]) > SYS_GRP_STRING_LEN)
    {
        vty_error_out(vty, "name is too long[max %d] %s" , SYS_GRP_STRING_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    iRet = sys_grp_contact_set(argv[0]);

    if (0 != iRet)
    {
        vty_error_out(vty, "set system location failed %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS ;
}

DEFUN(snmp_sysinfo_name_func,
      snmp_sysinfo_name_cmd,
      "snmp-agent sys-info sysname WORD",
      "snmp-agent\n"
      "sys-info\n"
      "sysname\n"
      "sysname <1-255>\n")
{
    int iRet = 0 ;

    if (strlen(argv[0]) > SYS_GRP_STRING_LEN)
    {
        vty_error_out(vty, "name is too long[max %d] %s" , SYS_GRP_STRING_LEN , VTY_NEWLINE);
        return CMD_WARNING;
    }

    iRet = sys_grp_sys_name_set(argv[0]);

    if (0 != iRet)
    {
        vty_error_out(vty, "set system group failed %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS ;
}

DEFUN(snmp_sysinfo_location_func,
      snmp_sysinfo_location_cmd,
      "snmp-agent sys-info location WORD",
      "snmp-agent\n"
      "sys-info\n"
      "location\n"
      "location <1-255>\n")
{
    int iRet = 0 ;

    if (strlen(argv[0]) > SYS_GRP_STRING_LEN)
    {
        vty_error_out(vty, "name is too long[max %d] %s" , SYS_GRP_STRING_LEN , VTY_NEWLINE);
        return CMD_WARNING;
    }

    iRet = sys_grp_location_set(argv[0]);

    if (0 != iRet)
    {
        vty_error_out(vty, "set system location failed %s" ,  VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS ;
}

DEFUN(snmp_sysinfo_show_func,
      snmp_sysinfo_show_cmd,
      "display snmp-agent sys-info",
      "display\n"
      "snmp-agent \n"
      "system group\n")
{
    struct system_group sys_group;

    static u_long   ulret  =  0 ;
    int ret = 0 ;
    memcpy(&sys_group, sys_grp_value_get(), sizeof(struct system_group));
    char str_sysobjid[64] = {""} ;
    sys_grp_sysobjectid_str_get(str_sysobjid) ;
    ret = devm_comm_get_runtime(1, 0, MODULE_ID_DEVM, &ulret);
    vty_out(vty, "system group information:%s", VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %s%s", "sysDesc" ,  sys_group.sysDesc , VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %s%s", "sysObjectID", str_sysobjid ,  VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %u%s", "upTime", ulret ,  VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %s%s", "sysContact", sys_group.sysContact ,  VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %s%s", "sysName" ,  sys_group.sysName , VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %s%s", "sysLocation", sys_group.sysLocation ,  VTY_NEWLINE);
    vty_out(vty, "   %-18s:   %d%s", "sysServices", sys_group.sysServices ,  VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(undo_snmp_sysinfo_contact_func,
      undo_snmp_sysinfo_contact_cmd,
      "undo snmp-agent sys-info contact",
      "undo"
      "snmp-agent\n"
      "sys-info\n"
      "contact\n")
{
    sys_grp_contact_set("");
    return CMD_SUCCESS ;
}

DEFUN(undo_snmp_sysinfo_location_func,
      undo_snmp_sysinfo_location_cmd,
      "undo snmp-agent sys-info location",
      "undo"
      "snmp-agent\n"
      "sys-info\n"
      "location\n")
{
    sys_grp_location_set("");
    return CMD_SUCCESS ;
}

DEFUN(undo_snmp_sysinfo_sysname_func,
      undo_snmp_sysinfo_sysname_cmd,
      "undo snmp-agent sys-info sysname",
      "undo"
      "snmp-agent\n"
      "sys-info\n"
      "contact\n")
{
    sys_grp_sys_name_set("");
    return CMD_SUCCESS ;
}

DEFUN(snmp_trap_enable_func,
      snmp_trap_enable_cmd,
      "snmp-agent trap enable",
      "snmp-agent\n"
      "trap\n"
      "enable\n")
{
    snmp_trap_enable_set(1);
    return CMD_SUCCESS ;
}

DEFUN(undo_snmp_trap_enable_func,
      undo_snmp_trap_enable_cmd,
      "undo snmp-agent trap enable",
      "undo\n"
      "snmp-agent\n"
      "trap\n"
      "enable\n")
{
    snmp_trap_enable_set(0);
    return CMD_SUCCESS ;
}



DEFUN(snmp_linkup_down_enable_func,
      snmp_linkup_down_enable_cmd,
      "enable snmp trap updown",
      "enable\n"
      "snmp\n"
      "trap\n"
      "updown\n")
{
    snmp_trap_link_enable_set(1);
    return CMD_SUCCESS ;
}

DEFUN(undo_snmp_linkup_down_enable_func,
      undo_snmp_linkup_down_enable_cmd,
      "undo enable snmp trap updown",
      "undo\n"
      "enable\n"
      "snmp\n"
      "trap\n"
      "updown\n")
{
    snmp_trap_link_enable_set(0);
    return CMD_SUCCESS ;
}



DEFUN(h3c_community_config_func ,
      h3c_community_config_cmd ,
      "snmp-agent community (read|write) WORD acl <2000-2999>",
      "snmp-agent\n"
      "snmp-agent community\n"
      "read only\n"
      "read and write\n"
      "community name <1-31>\n"
      "acl\n"
      "acl number\n")
{
    COMMUNITY_ITEM item;
//        memset(item.name, '\0', COMMUNITY_NAME_MAX_LEN);
    memset(&item, 0, sizeof(COMMUNITY_ITEM));
    item.attribute = RO;
    item.acl_num = 0;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support community config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_community_list->count >= ITEM_COMMUNITY_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of community, max %d%s", ITEM_COMMUNITY_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[1]) > COMMUNITY_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , COMMUNITY_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    //cannot same with init community
    if (strcmp(argv[1], "hios") == 0)
    {
        vty_error_out(vty, "Reserved keywords[%s], forbidden to use%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strcmp(argv[0], "read") == 0)
    {
        item.attribute = RO;
    }
    else
    {
        item.attribute = RW;
    }

    item.acl_num = atoi((char *)argv[2]);

    strcpy(item.name, argv[1]);

    if (NULL == snmp_config_item_get(SNMP_COMMUNITY_ITEM, &item))
    {
        snmp_config_item_add(SNMP_COMMUNITY_ITEM, &item);
        snmp_community_item_config_new(&item);
    }
    else
    {
        vty_error_out(vty, "Community %s already exist%s" , argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;

}

DEFUN(undo_h3c_community_config_func ,
      undo_h3c_community_config_cmd ,
      "undo snmp-agent community WORD",
      "delete\n"
      "snmp-agent\n"
      "snmp-agent community\n"
      "community name <1-31>\n")
{
    COMMUNITY_ITEM  item;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support community config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > COMMUNITY_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , COMMUNITY_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (SNMP_ERROR == snmp_check_legal_of_char((char *)argv[0], strlen(argv[0])))
    {
        vty_error_out(vty, "Illegal character %s" , VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL == argv[0])
    {
        vty_error_out(vty, "Please input a valiable community name%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > COMMUNITY_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community name is too long[max %d] %s" , COMMUNITY_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(item.name, '\0', COMMUNITY_NAME_MAX_LEN);
    strcpy(item.name, argv[0]);

    if (SNMP_OK == snmp_config_item_del(SNMP_COMMUNITY_ITEM, &item))
    {
        snmp_config_item_renew();
    }
    else
    {
        vty_error_out(vty, "Target does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;

}

DEFUN(h3c_version_config_func,
      h3c_version_config_cmd,
      "snmp-agent sys-info version (v1|v2c|v3|all)",
      "snmp\n"
      "sys-info\n"
      "version of snmp\n"
      "version v1\n"
      "version v2c\n"
      "version v3\n"
      "version v1/v2c/v3\n")
{
    SNMP_VERSION snmp_version_temp = snmp_version;  //record the old version

    if (strcmp(argv[0], "v1") == 0)     //check the input version
    {
        snmp_version = V1;
    }
    else if (strcmp(argv[0], "v2c") == 0)
    {
        snmp_version = V2C;
    }
    else if (strcmp(argv[0], "v3") == 0)
    {
        snmp_version = V3;
    }
    else if (strcmp(argv[0], "all") == 0)
    {
        snmp_version = ALL;
    }

    if (snmp_version_temp != snmp_version)  //if version has been changed
    {
        snmp_config_item_renew();
    }

    return CMD_SUCCESS;

}

DEFUN(h3c_dying_gasp_func,
      h3c_dying_gasp_cmd,
      "dying-gasp host  A.B.C.D {vpn-instance <1-128>} snmp-trap version (v1|v2c) securityname WORD",
      "dying-gasp \n"
      "host \n"
      "target address \n"
      "vpn-instance \n"
      "vpn value \n"
      "snmp-trap \n"
      "trap version \n"
      "v1\n"
      "v2c\n"
      "securityname \n"
      "securityname \n")
{
    uint32_t    ipv4;
    uint32_t    port;
    uint32_t    vpn;
    TRAP_ITEM   trap_item;
    //INFORM_ITEM inform_item;

    if (V3 == snmp_version)
    {
        vty_error_out(vty, "Current version[v3] does not support v1/v2c config%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (snmp_trap_list->count >= ITEM_SERVER_MAX_NUM)
    {
        vty_error_out(vty, "Reach max number of server, max %d%s", ITEM_SERVER_MAX_NUM, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* init trap_item */
    memset(trap_item.ip_addr.addr, 0, 4 * sizeof(uint32_t));
    memset(trap_item.name, '\0', TRAP_NAME_MAX_LEN);

    /* init inform_item */
    //memset(inform_item.ip_addr.addr, 0, 4*sizeof(uint32_t));
    //memset(inform_item.name, '\0', TRAP_NAME_MAX_LEN);

    if (inet_pton(AF_INET, argv[0], &ipv4) != 1)
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ipv4 = ntohl(ipv4);

    if (SNMP_ERROR == snmp_check_input_ip_valid(ipv4))
    {
        vty_error_out(vty, "The input ip is invaild.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    trap_item.port = 162;

    if (NULL != argv[1])
    {
        vpn = (uint32_t)atoi(argv[1]);

        if ((vpn < 1) || (vpn > 128))
        {
            vty_error_out(vty, "The input vpn is invaild.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.vpn = vpn;
        //inform_item.vpn = vpn;
    }
    else
    {
        trap_item.vpn = 0;
        //inform_item.vpn = 0;
    }

    if (strlen(argv[3]) > TRAP_NAME_MAX_LEN)
    {
        vty_error_out(vty, "Community is too long[max %d] %s" , TRAP_NAME_MAX_LEN, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* put value into trap_item */
    trap_item.ip_addr.type = IPv4;
    trap_item.ip_addr.addr[0] = ipv4;
    strcpy(trap_item.name, argv[3]);

    /* put value into inform_item */
    //inform_item.ip_addr.type = IPv4;
    //inform_item.ip_addr.addr[0] = ipv4;
    //strcpy(inform_item.name, argv[4]);

    if (strcmp(argv[2], "v1") == 0)
    {
        if (V2C == snmp_version)
        {
            vty_error_out(vty, "Current version[v2c] does not support v1 config%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.version = V1;

        if (NULL == snmp_config_item_get(SNMP_TRAP_ITEM, &trap_item))
        {
            snmp_config_item_add(SNMP_TRAP_ITEM, &trap_item);
            snmp_trap_item_config_new(&trap_item);
        }
        else
        {
            vty_error_out(vty, "Server %s already exist%s" , argv[0], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (strcmp(argv[2], "v2c") == 0)
    {
        if (V1 == snmp_version)
        {
            vty_error_out(vty, "Current version[v1] does not support v2c config%s" , VTY_NEWLINE);
            return CMD_WARNING;
        }

        trap_item.version = V2C;

        if (NULL == snmp_config_item_get(SNMP_TRAP_ITEM, &trap_item))
        {
            snmp_config_item_add(SNMP_TRAP_ITEM, &trap_item);
            snmp_trap_item_config_new(&trap_item);
        }
        /*if(NULL == snmp_config_item_get(SNMP_INFORM_ITEM, &inform_item))
        {
            snmp_config_item_add(SNMP_INFORM_ITEM, &inform_item);
            snmp_inform_item_config_new(&inform_item);
        }*/
        else
        {
            vty_error_out(vty, "Server %s already exist%s" , argv[0], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(h3c_mib_view_config_func,
      h3c_h3c_mib_view_config_cmd,
      "snmp-agent mib-view include iso-view iso",
      "snmp-agent\n"
      "mib-view \n"
      "include\n"
      "iso-view\n"
      "iso \n")
{
    return CMD_SUCCESS;
}



/*-----------------------------------lidingcheng added end  2018-04-13----------------------------*/


DEFUN(snmp_vpn_config_fun,
      snmp_vpn_config_cmd ,
      "snmp vpn-instance <1-128> ",
      "snmp\n"
      "snmp vpn-instance\n"
      "vpn value\n")
{
    snmp_server_vpn = (uint16_t)atoi(argv[0]);
	return CMD_SUCCESS;
}

DEFUN(no_snmp_vpn_config_fun,
      no_snmp_vpn_config_cmd ,
      "no snmp vpn-instance",
      "no\n"
      "snmp\n"
      "snmp vpn-instance\n")
{
    snmp_server_vpn = 0;
	return CMD_SUCCESS;
}


DEFUN(snmp_trap_source_cfg,
	snmp_trap_source_cfg_cmd ,
	"snmp trap source interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | loopback <0-128>}",
	"Snmp cmd\n"
	"Snmp trap\n"
	"Source interface\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_LOOPBACK_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)
{
	uint32_t ifindex = 0;
	if(argv[0])
	{
		ifindex = ifm_get_ifindex_by_name("ethernet", argv[0]);
	}
	else if(argv[1])
	{
		ifindex = ifm_get_ifindex_by_name("gigabitethernet", argv[1]);
	}
	else if(argv[2])
	{
		ifindex = ifm_get_ifindex_by_name("xgigabitethernet", argv[2]);
	}
	else if(argv[3])
	{
		ifindex = ifm_get_ifindex_by_name("loopback", argv[3]);
	}
	
	snmp_trap_source_if_cfg(vty, ifindex);
	return CMD_SUCCESS;
}

DEFUN(no_snmp_trap_source_cfg,
	no_snmp_trap_source_cfg_cmd ,
	"no snmp trap source",
	"Delete config\n"
	"Snmp cmd\n"
	"Snmp trap\n"
	"Source port\n")
{
	snmp_no_trap_source_if_cfg();
	return CMD_SUCCESS;
}

DEFUN(snmp_trap_source_ipv4_cfg,
	snmp_trap_source_ipv4_cfg_cmd ,
	"snmp trap source ip A.B.C.D",
	"Snmp cmd\n"
	"Snmp trap\n"
	"Source\n"
	"Source ip address\n"
	"Source ip address A.B.C.D\n")
{
	uint32_t ipv4 = 0;
	inet_pton(AF_INET, argv[0], &ipv4);
	snmp_trap_source_ip_cfg(vty, ipv4);
	return CMD_SUCCESS;
}


#if 0
DEFUN (snmp_conf_debug_fun,
       snmp_conf_debug_cmd,
       "debug snmp (enable|disable) (all|receive_send|mib_modules|trap|cache)",
       "Debub config\n"
       "snmp config\n"
       "snmp debug enable\n"
       "snmp debug disable\n"
       "snmp debug type all\n"
       "snmp debug type receive /send to ftm \n"     
       "snmp debug type mib_modules\n"
       "snmp trap\n"
       "snmp cache\n"
)
{
    int enable = 0;
    unsigned int type = 0;

    if(argv[0][0] == 'e') enable = 1;

    if(strcmp(argv[1],"mib_modules") == 0)
    {
        type = SNMP_MTYPE_MIB_MODULE;
    }    
    else if(strcmp(argv[1],"receive_send") == 0)
    {
        type = SNMP_MTYPE_RECEIVE_SEND;
    }
    else if(strcmp(argv[1],"trap") == 0)
    {    
        type = SNMP_MTYPE_TRAP;
    }
    else if(strcmp(argv[1],"cache") == 0)
    {
        type = SNMP_MTYPE_CACHE;        
    }
    else if(strcmp(argv[1],"all") == 0)
    {
        type = SNMP_MTYPE_MAX;        
    }
    else type = SNMP_MTYPE_MAX;

    zlog_debug_set(vty, type,  enable);
    return(CMD_SUCCESS);
}
#endif

/********************************************** debug ***********************************************/

const struct message snmp_debug_name[] =
{
	{.key = SNMP_DBG_MIB_GET,	.str = "get"},
	{.key = SNMP_DBG_MIB_SET,	.str = "set"},
	{.key = SNMP_DBG_TRAP,		.str = "trap"},
	{.key = SNMP_DBG_PACKET,	.str = "packet"},
	{.key = SNMP_DBG_CACHE,		.str = "cache"},
	{.key = SNMP_DBG_U0,		.str = "u0"},
	{.key = SNMP_DBG_ALL,		.str = "all"}
};


DEFUN (snmp_debug_monitor,
	snmp_debug_monitor_cmd,
	"debug snmp (enable|disable) (get|set|trap|packet|cache|u0|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of get messege\n"
	"Type name of set messege\n"
	"Type name of trap messege\n"
	"Type name of packet log queue \n"
	"Type name of cache log queue \n"
	"Type name of u0 log queue \n"
	"Type name of all debug\n")
{
	unsigned int typeid = 0;
	int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(snmp_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], snmp_debug_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, snmp_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* 显示个debug信息的状态  */
DEFUN (show_snmp_debug_monitor,
	show_snmp_debug_monitor_cmd,
	"show snmp debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	int type_num;

	vty_out(vty, "debug type         status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(snmp_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", snmp_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/


/* write the configuration of each interface */
int snmp_config_write(struct vty *vty)
{
    vty_out(vty, "snmp %s", VTY_NEWLINE);
    snmp_write_config_items(vty);
    sys_grp_write(vty);
    return 0;
}


void snmp_cli_init(void)
{
    install_node(&snmp_node, snmp_config_write);
    install_default(SNMPD_NODE);
    
    //install_element(CONFIG_NODE, &snmp_conf_debug_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &snmp_mode_enable_cmd, CMD_LOCAL);
    install_element(SNMPD_NODE, &snmp_version_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_version_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &snmp_v1v2c_server_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_v1v2c_server_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &snmp_v3_server_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_v3_server_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &snmp_community_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_community_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &snmp_user_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &snmp_user_config2_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_user_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &snmp_user_privilege_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_user_privilege_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &show_snmp_config_cmd, CMD_LOCAL);
    install_element(SNMPD_NODE, &show_snmp_user_cmd, CMD_LOCAL);
    install_element(SNMPD_NODE, &show_snmp_statistics_cmd, CMD_LOCAL);

    /*------------------added by lidingcheng for rfc1213 system group 2018-04-13--------------------------*/
    install_element(CONFIG_NODE, &snmp_sysinfo_contact_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &snmp_sysinfo_name_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &snmp_sysinfo_location_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &undo_snmp_sysinfo_contact_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &undo_snmp_sysinfo_sysname_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &undo_snmp_sysinfo_location_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &snmp_trap_enable_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &undo_snmp_trap_enable_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &snmp_linkup_down_enable_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &undo_snmp_linkup_down_enable_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &description_snmp_config_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &h3c_community_config_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &undo_h3c_community_config_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &h3c_version_config_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &h3c_dying_gasp_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &h3c_h3c_mib_view_config_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &snmp_sysinfo_show_cmd, CMD_LOCAL);
    /*-----------------------------------lidingcheng added end  2018-04-13----------------------------*/
    install_element(SNMPD_NODE, &snmp_vpn_config_cmd, CMD_SYNC);
    install_element(SNMPD_NODE, &no_snmp_vpn_config_cmd, CMD_SYNC);

    install_element(SNMPD_NODE, &snmp_trap_source_cfg_cmd, CMD_SYNC);
	install_element(SNMPD_NODE, &snmp_trap_source_ipv4_cfg_cmd, CMD_SYNC);
	install_element(SNMPD_NODE, &no_snmp_trap_source_cfg_cmd, CMD_SYNC);    


	install_element(CONFIG_NODE, &snmp_debug_monitor_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &show_snmp_debug_monitor_cmd, CMD_SYNC);
}


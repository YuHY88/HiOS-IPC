/* Configuration generator.
   Copyright (C) 2000 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#include <zebra.h>

#include "command.h"
#include "linklist.h"
#include "memory.h"
#include "ifm_common.h"

#include "vtysh/vtysh.h"
#include "aaa_common.h"
#include <lib/devm_com.h>
#include <lib/module_id.h>
#include <lib/buffer.h>
#include "vtysh_sync.h"

vector configvec;

extern int vtysh_writeconfig_integrated;
int config_update;

extern int g_read_config_err_flag;
extern struct user_line *vtysh_user_line_get(int line, int line_index);

struct config
{
    /* Configuration node name. */
    char *name;

    /* Configuration string line. */
    struct list *line;

    /* Configuration can be nest. */
    struct config *config;

    /* Index of this config. */
    u_int32_t index;

    int ifm_no_exist;
};

struct list *config_top;

static int
line_cmp(char *c1, char *c2)
{
    return strcmp(c1, c2);
}

static void
line_del(char *line)
{
    XFREE(MTYPE_VTYSH_CONFIG_LINE, line);
}

static struct config *
config_new()
{
    struct config *config;
    config = XCALLOC(MTYPE_VTYSH_CONFIG, sizeof(struct config));
    return config;
}

static int
config_cmp(struct config *c1, struct config *c2)
{
    return strcmp(c1->name, c2->name);
}

static void
config_del(struct config *config)
{
    list_delete(config->line);

    if(config->name)
    {
        XFREE(MTYPE_VTYSH_CONFIG_LINE, config->name);
    }

    XFREE(MTYPE_VTYSH_CONFIG, config);
}

static struct config *
config_get(int index, const char *line, long long daemon_flag)
{
    struct config *config;
    struct config *config_loop;
    struct list *master;
    struct listnode *node, *nnode;

    config = config_loop = NULL;

    master = vector_lookup_ensure(configvec, index);

    if(! master)
    {
        master = list_new();
        master->del = (void (*)(void *))config_del;
        master->cmp = (int (*)(void *, void *)) config_cmp;
        vector_set_index(configvec, index, master);
    }

    for(ALL_LIST_ELEMENTS(master, node, nnode, config_loop))
    {
        if(strcmp(config_loop->name, line) == 0)
        {
            config = config_loop;
        }
    }

    if(! config)
    {
        config = config_new();
        config->line = list_new();
        config->line->del = (void (*)(void *))line_del;
        config->line->cmp = (int (*)(void *, void *)) line_cmp;
        config->name = XSTRDUP(MTYPE_VTYSH_CONFIG_LINE, line);
        config->index = index;
        config->config = NULL;
        config->ifm_no_exist = 0;

        if ((daemon_flag != VTYSH_IFM && daemon_flag != VTYSH_SDHVCG) && \
            !strncmp(line, "interface", strlen ("interface")))
        {
            config->ifm_no_exist = 1;
        }

        listnode_add(master, config);
    }

    return config;
}

static struct config *
config_get_sub_if(int index, const char *line, long long daemon_flag)
{
    struct config *config, *sub_config;
    struct config *config_loop, *config_sub_loop;
    struct list *master;
    struct listnode *node, *nnode;
    const char *dot = ".";
    char *line_eth;
    int  line_eth_len;
    char *pos;

    pos = strstr(line, dot);

    if(pos == NULL)
    {
        fprintf(stderr, "sub interface error\n");
        return NULL;
    }

    line_eth_len = pos - line;
    line_eth = (char *)malloc(line_eth_len + 1);
    memset(line_eth, 0, line_eth_len + 1);

    strncpy(line_eth, line, line_eth_len);

    config = config_loop = NULL;

    master = vector_lookup_ensure(configvec, index);

    if(! master)
    {
        free(line_eth);
        //fprintf(stderr, "Parent interface list is not exist\n");
        zlog_err("%s:Parent interface list is not exist(%d)\n", __FUNCTION__, index);
        g_read_config_err_flag = 1;
        return NULL;
    }

    /*find parent interface config*/
    for(ALL_LIST_ELEMENTS(master, node, nnode, config_loop))
    {
        if(strcmp(config_loop->name, line_eth) == 0)
        {
            config = config_loop;
        }
    }

    sub_config = NULL;

    if(! config)
    {
        free(line_eth);
//      fprintf(stderr, "Parent interface is not exist\n");
        zlog_err("%s:Parent interface is not exist(%d)\n", __FUNCTION__, index);
        g_read_config_err_flag = 1;
        return NULL;
    }
    else
    {
        for(config_sub_loop = config; config_sub_loop != NULL;  \
                config_sub_loop = config_sub_loop->config)
        {
            if(strcmp(config_sub_loop->name, line) == 0)
            {
                sub_config = config_sub_loop;
//              break;
            }
        }

        if(!sub_config)
        {
            /*creat sub config*/
            sub_config = config_new();
            sub_config->line = list_new();
            sub_config->line->del = (void (*)(void *))line_del;
            sub_config->name = XSTRDUP(MTYPE_VTYSH_CONFIG_LINE, line);
            sub_config->index = index;

            config_sub_loop = config;

            while(config_sub_loop->config != NULL)
            {
                config_sub_loop = config_sub_loop->config;
            }

            config_sub_loop->config = sub_config;
            sub_config->config = NULL;
#if 0

            for(config_sub_loop = config; config_sub_loop != NULL;  \
                    config_sub_loop = config_sub_loop->config)
            {
                printf("after add config name:%s\n", config_sub_loop->name);
            }

#endif
        }
    }

    free(line_eth);

    return sub_config;
}

static void
config_add_line(struct list *config, const char *line)
{
    listnode_add(config, XSTRDUP(MTYPE_VTYSH_CONFIG_LINE, line));
}

static void
config_add_line_uniq(struct list *config, const char *line)
{
    struct listnode *node, *nnode;
    char *pnt;

    for(ALL_LIST_ELEMENTS(config, node, nnode, pnt))
    {
        if(strcmp(pnt, line) == 0)
        {
            return;
        }
    }

    listnode_add_sort(config, XSTRDUP(MTYPE_VTYSH_CONFIG_LINE, line));
}

static void
vtysh_config_parse_line(const char *line, long long daemon_flag)
{
    char c;
    static struct config *config = NULL;

    if(! line)
    {
        return;
    }

    c = line[0];

    if(c == '\0')
    {
        return;
    }

    switch(c)
    {
        case '!':
        case '#':
            break;

        case ' ':

            /* Store line to current configuration. */
            if(config)
            {
                if(config->index == INTERFACE_NODE)
                {
                    config_add_line_uniq(config->line, line);
                }
                else
                {
                    config_add_line(config->line, line);
                }
            }
            else
            {
                config_add_line(config_top, line);
            }

            break;

        default:
            if(strncmp(line, "interface ethernet", strlen("interface ethernet")) == 0)
            {
                if(strstr(line, "."))
                {
                    config = config_get_sub_if(PHYSICAL_IF_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(PHYSICAL_IF_CONFIG, line, daemon_flag);
                }

            }
            else if(strncmp(line, "interface gigabitethernet", strlen("interface gigabitethernet")) == 0)
            {
                if(strstr(line, "."))
                {
                    config = config_get_sub_if(PHYSICAL_IF_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(PHYSICAL_IF_CONFIG, line, daemon_flag);
                }

            }
            else if(strncmp(line, "interface xgigabitethernet", strlen("interface xgigabitethernet")) == 0)
            {
                if(strstr(line, "."))
                {
                    config = config_get_sub_if(PHYSICAL_IF_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(PHYSICAL_IF_CONFIG, line, daemon_flag);
                }

            }
            else if(strncmp(line, "interface stm", strlen("interface stm")) == 0)
            {

                if(strstr(line, "."))
                {
                    config = config_get_sub_if(STM_IF_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(STM_IF_CONFIG, line, daemon_flag);
                }

            }
            else if(strncmp(line, "interface tdm", strlen("interface tdm")) == 0)
            {
                if(strstr(line, "."))
                {
                    config = config_get_sub_if(TDM_IF_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(TDM_IF_CONFIG, line, daemon_flag);
                }

            }
            else if(strncmp(line, "interface trunk", strlen("interface trunk")) == 0)
            {
                if(strstr(line, "."))
                {
                    config = config_get_sub_if(TRUNK_IF_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(TRUNK_IF_CONFIG, line, daemon_flag);
                }
            }

            else if(strncmp(line, "ip pool dhcp", strlen("ip pool dhcp")) == 0)
            {
                config = config_get(POOL_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "vlan", strlen("vlan")) == 0)
            {
                config = config_get(VLAN_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "interface tunnel", strlen("interface tunnel")) == 0)
            {
                config = config_get(TUNNEL_IF_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "interface loopback", strlen("interface loopback")) == 0)
            {
                config = config_get(LOOPBACK_IF_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "interface vlanif", strlen("interface vlanif")) == 0)
            {
                config = config_get(VLANIF_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "interface clock", strlen("interface clock")) == 0)
            {
                config = config_get(CLOCK_CONFIG, line, daemon_flag);
            }
            else if (strncmp (line, "interface sdhvcg", strlen ("interface sdhvcg")) == 0)
            {
                config = config_get(VCG_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "interface", strlen("interface")) == 0)
            {
                config = config_get(INTERFACE_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "acl", strlen("acl")) == 0)
            {
                config = config_get(ACL_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "mpls", strlen("mpls")) == 0)
            {
                if(strncmp(line, "mpls-aps", strlen("mpls-aps")) == 0)
                {
                    config = config_get(MPLS_APS_SESS_CONFIG, line, daemon_flag);
                }
                else
                {
                    config = config_get(MPLS_CONFIG, line, daemon_flag);
                }
            }
            else if (strncmp (line, "pw-class", strlen("pw-class")) == 0)
            {
                config = config_get (PW_CLASS_CONFIG, line, daemon_flag);
            }
            else if (strncmp (line, "xconnect-group", strlen("xconnect-group")) == 0)
            {
                config = config_get (XCONNECT_GROUP_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "route", strlen("route")) == 0)
            {
                config = config_get(ROUTE_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "rip instance", strlen("rip instance")) == 0)
            {
                config = config_get(RIP_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "ripng instance", strlen("ripng instance")) == 0)
            {
                config = config_get(RIPNG_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "ospf instance", strlen("ospf instance")) == 0)
            {
                config = config_get(OSPF_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "dcn", strlen("dcn")) == 0)
            {
                config = config_get(DCN_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "ospf6 instance", strlen("ospf6 instance")) == 0)
            {
                config = config_get(OSPF6_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "isis instance", strlen("isis instance")) == 0)
            {
                config = config_get(ISIS_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "bgp as", strlen("bgp as")) == 0)
            {
                config = config_get(BGP_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "qos policy", strlen("qos policy")) == 0)
            {
                config = config_get(QOS_POLICY_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "qos car", strlen("qos car")) == 0)
            {
                config = config_get(QOS_CAR_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "qos wred", strlen("qos wred")) == 0)
            {
                config = config_get(HQOS_WRED_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "qos queue", strlen("qos queue")) == 0)
            {
                config = config_get(HQOS_QUEUE_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "mirror", strlen("mirror")) == 0)
            {
                config = config_get(QOS_MIRROR_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "cpu-car", strlen("cpu-car")) == 0)
            {
                config = config_get(QOS_CPCAR_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "qos mapping", strlen("qos mapping")) == 0)
            {
                if((strncmp(line, "qos mapping cos-domain", strlen("qos mapping cos-domain")) == 0) ||
                        (strncmp(line, "qos mapping tos-domain", strlen("qos mapping tos-domain")) == 0) ||
                        (strncmp(line, "qos mapping exp-domain", strlen("qos mapping exp-domain")) == 0) ||
                        (strncmp(line, "qos mapping dscp-domain", strlen("qos mapping dscp-domain")) == 0))
                {
                    config = config_get(QOS_DOMAIN_CONFIG, line, daemon_flag);
                }
                else if((strncmp(line, "qos mapping cos-phb", strlen("qos mapping cos-phb")) == 0) ||
                        (strncmp(line, "qos mapping tos-phb", strlen("qos mapping tos-phb")) == 0) ||
                        (strncmp(line, "qos mapping exp-phb", strlen("qos mapping exp-phb")) == 0) ||
                        (strncmp(line, "qos mapping dscp-phb", strlen("qos mapping dscp-phb")) == 0))
                {
                    config = config_get(QOS_PHB_CONFIG, line, daemon_flag);
                }
            }
            else if(strncmp(line, "debug", strlen("debug")) == 0)
            {
                config = config_get(DEBUG_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "arp", strlen("arp")) == 0)
            {
                config = config_get(ARP_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "aaa", strlen("aaa")) == 0)
            {
                config = config_get(AAA_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "snmp ", strlen("snmp ")) == 0)
            {
                config = config_get(SNMPD_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "rmon", strlen("rmon")) == 0)
            {
                config = config_get(RMON_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "lldp", strlen("lldp")) == 0)
            {
                config = config_get(LLDP_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "mstp", strlen("mstp")) == 0)
            {
                config = config_get(MSTP_NODE, line, daemon_flag);
            }
            else if((strncmp(line, "mac", strlen("mac")) == 0) 
                || (strncmp(line, "vpls", strlen("vpls")) == 0))
            {
                config = config_get(MAC_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "login", strlen("login")) == 0)
            {
                config = config_get(LOGIN_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "syslog", strlen("syslog")) == 0)
            {
                config = config_get(SYSLOG_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "device", strlen("device")) == 0)
            {
                config = config_get(DEVM_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "ntp", strlen("ntp")) == 0)
            {
                config = config_get(NTP_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "synce", strlen("synce")) == 0)
            {
                config = config_get(SYNCE_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "ndp", strlen("ndp")) == 0)
            {
                config = config_get(NDP_CONFIG, line, daemon_flag);
            }
			else if(strncmp(line, "sdhdxc", strlen("sdhdxc")) == 0)
            {
                config = config_get(SDHDXC_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "system", strlen("system")) == 0)
            {
                config = config_get(SYSTEM_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "elps session", strlen("elps session")) == 0)
            {
                config = config_get(ELPS_SESSION_CONFIG, line, daemon_flag);
            }
            else if(strncmp(line, "erps session", strlen("erps session")) == 0)
            {
                config = config_get(ERPS_SESSION_CONFIG, line, daemon_flag);
            }
            else
            {
                if(strncmp(line, "log", strlen("log")) == 0
                        || strncmp(line, "hostname", strlen("hostname")) == 0
                  )
                {
                    config_add_line_uniq(config_top, line);
                }

                else
                {
                    config_add_line(config_top, line);
                }

                config = NULL;
            }

            break;
    }
}

void
vtysh_config_parse(char *line, long long daemon_flag)
{
    char *begin;
    char *pnt;

    begin = pnt = line;

//  usleep(1000);
    while(*pnt != '\0')
    {
        if(*pnt == '\n' || *pnt == '\r')
        {
            *pnt++ = '\0';
            vtysh_config_parse_line(begin, daemon_flag);
            begin = pnt;
        }
        else
        {
            pnt++;
        }
    }
}

/* Macro to check delimiter is needed between each configuration line
 * or not. */
#define NO_DELIMITER(I)  \
  ((I) == DEBUG_CONFIG \
   || (I) == AAA_CONFIG)
void vtysh_vty_allif_config_dump(struct vty *vty, const char *buf)
{
    struct listnode *node, *nnode;
    struct listnode *mnode, *mnnode;
    struct config *config;
    struct list *master;
    char *line;
    unsigned int i;

    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
            for(ALL_LIST_ELEMENTS(master, node, nnode, config))
            {
                if(config->ifm_no_exist)
                {
                    continue;
                }

                if(strncmp(config->name, buf, strlen(buf)) == 0)
                {
                    while(config)
                    {
                        vty_out(vty, "%s%s", config->name, VTY_NEWLINE);

                        for(ALL_LIST_ELEMENTS(config->line, mnode, mnnode, line))
                        {
                            vty_out(vty, "%s%s", line, VTY_NEWLINE);
                        }

                        if(! NO_DELIMITER(i))
                        {
                            vty_out(vty, "!%s", VTY_NEWLINE);
                        }

                        config = config->config;
                    }
                }
            }
        }

    for(i = 0; i < vector_active(configvec); i++)
    {
        if((master = vector_slot(configvec, i)) != NULL)
        {
            sub_config_list_delete(master);
            vector_slot(configvec, i) = NULL;
        }
    }

    list_delete_all_node(config_top);

}

void vtysh_vty_if_config_dump(struct vty *vty, char *buf)
{
    struct listnode *node, *nnode;
    struct listnode *mnode, *mnnode;
    struct config *config, *config_sub_loop;
    struct list *master;
    char *line;
    unsigned int i;
    char *line_eth = NULL;
    const char *dot = ".";
    int  line_eth_len;
    char *pos;
    int sub_if = 0;

    if((pos = strstr(buf, dot)))
    {
        line_eth_len = pos - buf;
        line_eth = (char *)malloc(line_eth_len + 1);
        memset(line_eth, 0, line_eth_len + 1);
        strncpy(line_eth, buf, line_eth_len);
        sub_if = 1;
    }

    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
            for(ALL_LIST_ELEMENTS(master, node, nnode, config))
            {
                if(config->ifm_no_exist)
                {
                    continue;
                }

                /*show parent interface*/
                if(!sub_if)
                {
                    if(strcmp(config->name, buf) == 0)
                    {
//                  printf("!sub_if\n");
                        vty_out(vty, "%s%s", config->name, VTY_NEWLINE);

                        for(ALL_LIST_ELEMENTS(config->line, mnode, mnnode, line))
                        {
                            vty_out(vty, "%s%s", line, VTY_NEWLINE);
                        }

                        if(! NO_DELIMITER(i))
                        {
                            vty_out(vty, "!%s", VTY_NEWLINE);
                        }
                    }
                }
                else
                {
                    /*find parent config*/
                    if(strcmp(config->name, line_eth) == 0)
                    {
                        /*find sub config*/
                        for(config_sub_loop = config; config_sub_loop != NULL;  \
                                config_sub_loop = config_sub_loop->config)
                        {
                            if(strcmp(config_sub_loop->name, buf) == 0)
                            {
                                /*get sub config echo*/
                                vty_out(vty, "%s%s", config_sub_loop->name, VTY_NEWLINE);

                                for(ALL_LIST_ELEMENTS(config_sub_loop->line, mnode, mnnode, line))
                                {
                                    vty_out(vty, "%s%s", line, VTY_NEWLINE);
                                }
                            }
                        }
                    }
                }
            }
        }

    if(!line_eth)
    {
        free(line_eth);
    }

    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
            sub_config_list_delete(master);
            vector_slot(configvec, i) = NULL;
        }

    list_delete_all_node(config_top);

}


void vtysh_vty_config_dump(struct vty *vty)
{
    struct listnode *node, *nnode;
    struct listnode *mnode, *mnnode;
    struct config *config;
    struct list *master;
    char *line;
    unsigned int i;

    for(ALL_LIST_ELEMENTS(config_top, node, nnode, line))
    {
        vty_out(vty, "%s\r\n", line);
    }

    vty_out(vty, "!\r\n");


    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
            for(ALL_LIST_ELEMENTS(master, node, nnode, config))
            {
                if(config->ifm_no_exist)
                {
                    continue;
                }

                while(config)
                {
                    vty_out(vty, "%s\r\n", config->name);

                    for(ALL_LIST_ELEMENTS(config->line, mnode, mnnode, line))
                    {
                        vty_out(vty, "%s\r\n", line);
                    }

                    if(! NO_DELIMITER(i))
                    {
                        vty_out(vty, "!\r\n");
                    }

                    config =  config->config;
                }
            }

            if(NO_DELIMITER(i))
            {
                vty_out(vty, "!\r\n");
            }
        }

//  vty_out (vty, "end%s", VTY_NEWLINE);

    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
            sub_config_list_delete(master);
            vector_slot(configvec, i) = NULL;
        }

    list_delete_all_node(config_top);

}


/* Display configuration to file pointer. */
void
vtysh_config_dump(FILE *fp)
{
    struct listnode *node, *nnode;
    struct listnode *mnode, *mnnode;
    struct config *config;
    struct list *master;
    char *line;
    unsigned int i;

    for(ALL_LIST_ELEMENTS(config_top, node, nnode, line))
    {
        fprintf(fp, "%s\n", line);
        fflush(fp);
    }

    fprintf(fp, "!\n");
    fflush(fp);

    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
            for(ALL_LIST_ELEMENTS(master, node, nnode, config))
            {
                /*interface not exist, skip*/
                if(config->ifm_no_exist)
                {
                    continue;
                }

                while(config)
                {
                    fprintf(fp, "%s\n", config->name);
                    fflush(fp);

                    for(ALL_LIST_ELEMENTS(config->line, mnode, mnnode, line))
                    {
                        fprintf(fp, "%s\n", line);
                        fflush(fp);
                    }

                    if(! NO_DELIMITER(i))
                    {
                        fprintf(fp, "!\n");
                        fflush(fp);
                    }

                    /*get sub config*/
                    config = config->config;
                }
            }

            if(NO_DELIMITER(i))
            {
                fprintf(fp, "!\n");
                fflush(fp);
            }
        }

//  fprintf(fp, "end\n");
    for(i = 0; i < vector_active(configvec); i++)
        if((master = vector_slot(configvec, i)) != NULL)
        {
//  list_delete (master);
            sub_config_list_delete(master);
            vector_slot(configvec, i) = NULL;
        }

    list_delete_all_node(config_top);
}

void sub_config_list_delete(struct list *list)
{
    struct listnode *node;
    struct listnode *next;
    struct config   *config, *config_next;

    assert(list);

    for(node = list->head; node; node = next)
    {
        for(config = listgetdata(node); config; config = config_next)
        {
            config_next = config->config;
            config_del(config);
        }

        next = node->next;
//      if (list->del)
//  (*list->del) (node->data);
        listnode_free(node);
    }

    list->head = list->tail = NULL;
    list->count = 0;

    list_free(list);
}

int vtysh_user_line_write(void)
{
	int i = 0;
	struct user_line *ul = NULL;
	char line[50];
    
	ul = vtysh_user_line_get(USER_LINE_CONSOLE, 0);
	if(ul->iscfg != -1)
	{
		memset(line, 0, strlen(line));
		sprintf(line, "line console 0");
		vtysh_config_parse_line(line, 0);
		if(ul->auth_mode != AUTH_MODE_SCHEME)
		{
			memset(line, 0, strlen(line));
			sprintf(line, " authentication-mode %s", (ul->auth_mode == AUTH_MODE_NONE)?"none":"password");
			vtysh_config_parse_line(line, 0);			
		}

		if(ul->auth_mode == AUTH_MODE_PASSWORD)
		{
			memset(line, 0, strlen(line));
			if(strlen(ul->password) != 0)
			{
				sprintf(line, " set authentication password simple %s", ul->password);
				vtysh_config_parse_line(line, 0);
			}
		}

		if(ul->user_role != USER_ROLE_NETWORK_ADMIN)
		{
			memset(line, 0, strlen(line));
			sprintf(line, " user-role network-operator");
			vtysh_config_parse_line(line, 0);		
		}

		if(ul->idle_time_set != VTYSH_IDLE_TIMEOUT_DEF)
		{
			memset(line, 0, strlen(line));
			sprintf(line, " idle-timeout %d %d", ul->idle_time_set/60, ul->idle_time_set%60);
			vtysh_config_parse_line(line, 0);            
		}
	}

	for(i = 0;i < USER_LINE_VTY_NUM; i++)
	{
		ul = vtysh_user_line_get(USER_LINE_VTY, i);
		if(ul->iscfg != -1)
		{
			memset(line, 0, strlen(line));
			sprintf(line, "line vty %d", i);
			vtysh_config_parse_line(line, 0);			
			if(ul->auth_mode != AUTH_MODE_SCHEME)
			{
				memset(line, 0, strlen(line));
				sprintf(line, " authentication-mode %s", (ul->auth_mode == AUTH_MODE_NONE)?"none":"password");
				vtysh_config_parse_line(line, 0);
			}

			if(ul->auth_mode == AUTH_MODE_PASSWORD)
			{
				memset(line, 0, strlen(line));
				if(strlen(ul->password) != 0)
				{
					sprintf(line, " set authentication password simple %s", ul->password);
					vtysh_config_parse_line(line, 0);
				}
			}

			if(ul->user_role != USER_ROLE_NETWORK_OPERATOR)
			{
				memset(line, 0, strlen(line));
				sprintf(line, " user-role network-admin");
				vtysh_config_parse_line(line, 0);			
			}

    		if(ul->idle_time_set != VTYSH_IDLE_TIMEOUT_DEF)
    		{
    			memset(line, 0, strlen(line));
    			sprintf(line, " idle-timeout %d %d", ul->idle_time_set/60, ul->idle_time_set%60);
    			vtysh_config_parse_line(line, 0);            
    		}			
		}		
	}

	memset(line, 0, strlen(line));

    vtysh_config_parse_line("!", 0);

	return 0;
}

/* Read up configuration file from file_name. */
void
vtysh_read_file (FILE *confp, char *ifname)
{
    int i, ret;
    struct vty *vty;

    vty = vty_new ();
    vty->fd = 0;			/* stdin */
    vty->type = VTY_FILE;
    vty->node = CONFIG_NODE;

    for (i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }
 
    if (vtysh_connect_all (vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
//        exit(1);
    }

    if(ifname)
    {
        /*only re-config added interface*/
        ret = vtysh_config_from_file_interface (vty, confp, ifname);

        if (CMD_ERR_NO_MATCH2 != ret)
        {
            vtysh_execute_no_pager ("exit", vty);
//          vtysh_execute_no_pager ("disable", vty);
        }
    }
    else
    {
        /* Execute configuration file. */
        ret = vtysh_config_from_file (vty, confp);

        vtysh_execute_no_pager ("end", vty);
//      vtysh_execute_no_pager ("disable", vty);
    }

    vty_close (vty);

    if (ret != CMD_SUCCESS)
    {
        switch (ret)
        {
        case CMD_ERR_AMBIGUOUS:
            fprintf (stderr, "Ambiguous command.\n");
        break;

        case CMD_ERR_NO_MATCH:
            fprintf (stderr, "There is no such command.\n");
	    break;
		case CMD_ERR_NO_MATCH2:	
		break;
        default:
            fprintf (stderr, "Error occured during reading below line.\n%s\n", vty->buf);
        break;
        }

//        exit (1);
    }
}

/* Read up configuration file from config_default_dir. */
int
vtysh_read_config(char *config_default_dir, char *config_init_dir)
{
    FILE *confp = NULL;

#if 0

    /*if we are on slave slot, do not read config until recv the update config_file*/
    if(syncvty.vty_slot == VTY_SLAVE_SLOT && config_update != 1)
    {
        zlog_debug(0, "slave slot, return \n");
        return;
    }

#endif

    /*if startup.config not exist we read config.init for init config*/
    if(config_init_dir != NULL
            && access(config_default_dir, F_OK) < 0)
    {
        /*we only read config.init and then return*/
        return (vtysh_read_init(config_init_dir));
    }

    confp = fopen(config_default_dir, "r");

    if(confp == NULL)
    {
        return (1);
    }

    vtysh_read_file(confp, NULL);
    fclose(confp);

//  host_config_set (config_default_dir);

    return (0);
}

int
vtysh_read_init(char *config_init_dir)
{
    FILE *confp = NULL;

    /*check file exist*/
    if(access(config_init_dir, F_OK) < 0)
    {
        printf("file %s not exist\n", config_init_dir);
        return 1;
    }

    /*config.init exist*/
    confp = fopen(config_init_dir, "r");

    if(confp == NULL)
    {
        return (1);
    }

    vtysh_read_file(confp, NULL);
    fclose(confp);

    return 0;
}

/* We don't write vtysh specific into file from vtysh. vtysh.conf should
 * be edited by hand. So, we handle only "write terminal" case here and
 * integrate vtysh specific conf with conf from daemons.
 */
void
vtysh_config_write()
{
//  extern struct host host;
//  struct listnode *node, *nnode;
//  struct host_users *users_loop, *user;
    char *line;

    line = (char *)malloc(sizeof(char) * 150);
    memset(line, 0, sizeof(char) * 150);

    sprintf(line, "sysrun");
    vtysh_config_parse_line(line, 0);
    memset(line, 0, strlen(line));

    if(strcmp(host.device_name, "Hios"))
    {
        sprintf(line, " hostname %s", host.device_name);
    }

    vtysh_config_parse_line(line, 0);
    memset(line, 0, strlen(line));

    vtysh_config_parse_line("!", 0);

    free(line);
    line = NULL;
}

/* Read up configuration file from file_name. */
void
vty_read_file(FILE *confp)
{
    int ret;
    struct vty *vty;
    unsigned int line_num = 0;

    vty = vty_new();
    vty->wfd = dup(STDERR_FILENO); /* vty_close() will close this */

    if(vty->wfd < 0)
    {
        /* Fine, we couldn't make a new fd. vty_close doesn't close stdout. */
        vty->wfd = STDOUT_FILENO;
    }

    vty->fd = STDIN_FILENO;
    vty->type = VTY_FILE;
    vty->node = CONFIG_NODE;

    /* Execute configuration file */
    ret = config_from_file(vty, confp, &line_num);

    /* Flush any previous errors before printing messages below */
    buffer_flush_all(vty->obuf, vty->fd);

    if(!((ret == CMD_SUCCESS) || (ret == CMD_ERR_NOTHING_TODO)))
    {
        switch(ret)
        {
            case CMD_ERR_AMBIGUOUS:
                fprintf(stderr, "*** Error reading config: Ambiguous command.\n");
                break;

            case CMD_ERR_NO_MATCH:
                fprintf(stderr, "*** Error reading config: There is no such command.\n");
                break;
        }

        fprintf(stderr, "*** Error occured processing line %u, below:\n%s\n",
                line_num, vty->buf);
        vty_close(vty);
        exit(1);
    }

    vty_close(vty);
}


void
vtysh_config_init()
{
    config_top = list_new();
    config_top->del = (void (*)(void *))line_del;
    configvec = vector_init(1);

    if(pthread_mutex_init(&vtysh_config_lock, NULL) != 0)
    {
        perror("vtysh_config_lock init failed\n");
        exit(1);
    }
}

#if 0
void vtysh_get_slot()
{
    struct devm_unit devm_slot;
    int ret = 0;

    ret = devm_comm_get_unit(1, MODULE_ID_VTY, &devm_slot);
    zlog_debug(VD_COMM, "slot_main_board:%d, slot_slave_board:%d myslot:%d", devm_slot.slot_main_board, devm_slot.slot_slave_board, devm_slot.myslot);

    /*no slot device*/
    if(ret == 0)
    {
        /*check slot*/
        if(devm_slot.myslot == devm_slot.slot_main_board)
        {
            vty_slot = VTY_MAIN_SLOT;
            zlog_debug(VD_COMM, "vty_slot: main:%d\n", vty_slot);
        }
        else if(devm_slot.myslot == devm_slot.slot_slave_board)
        {
            vty_slot = VTY_SLAVE_SLOT;
            zlog_debug(VD_COMM, "vty_slot slave:%d\n", vty_slot);
        }
        else
        {
            vty_slot = VTY_UNKNOW_SLOT;
            zlog_debug(VD_COMM, "VTY_UNKNOW_SLOT!");
        }
    }
}

void *vtysh_get_slave_config(void *arg)
{
    prctl(PR_SET_NAME, " vtysh_get_slave_config");

    while(1)
    {
        if(access("/data/dat/check/configcheck", F_OK) != 0)
        {
            sleep(1);
            continue;
        }
        else
        {
            zlog_debug(VD_COMM, "find /data/dat/check/configcheck\n");
            config_update = 1;
            reading_config_file = 1;
            vtysh_read_config(integrate_default, NULL);

            if(remove("/data/dat/check/configcheck") != 0)
            {
                zlog_err("remove error!\n");
            }

            reading_config_file = 0;
            config_update = 0;
        }
    }

    return NULL;
}
#endif

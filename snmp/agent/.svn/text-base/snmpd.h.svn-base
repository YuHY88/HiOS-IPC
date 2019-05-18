/*
 * snmpd.h
 */

#define MASTER_AGENT 0
#define SUB_AGENT    1

#define SNMPD_VTYSH_PATH "/var/run/snmpd.vty"

extern int      agent_role;

extern int      snmp_dump_packet;
extern int      verbose;
extern int (*sd_handlers[])(int);
extern int      smux_listen_sd;

extern unsigned int gDevTypeID ;
extern int      snmp_read_packet(int);

/*
 * config file parsing routines
 */
void            agentBoots_conf(char *, char *);

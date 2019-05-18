/*
 *  System MIB group interface - system.h
 *
 */
#ifndef _MIBGROUP_SYSTEM_MIB_H
#define _MIBGROUP_SYSTEM_MIB_H

//config_require(util_funcs)

extern char     version_descr[];

#define Ifindex                      1
#define IfDescr                      2
#define IfType                       3
#define IfMtu                        4
#define IfSpeed                      5
#define IfPhysAddress                6
#define IfAdminStatus                7
#define IfOperStatus                 8
#define IfLastChange                 9
#define IfInOctets                  10
#define IfInUcastPkts               11
#define IfInNUcastPkts              12
#define IfInDiscards                13
#define IfInErrors                  14
#define IfInUnknownProtos           15
#define IfOutOctets                 16
#define IfOutUcastPkts              17
#define IfOutNUcastPkts             18
#define IfOutDiscards               19
#define IfOutErrors                 20
#define IfOutQLen                   21
#define IfSpecific                  22

#define IfNumber                     1

#define IfName                       1
#define IfInMulticastPkts            2
#define IfInBroadcastPkts            3
#define IfOutMulticastPkts           4
#define IfOutBroadcastPkts           5
#define IfHCInOctets                 6
#define IfHCInUcastPkts              7
#define IfHCInMulticastPkts          8
#define IfHCInBroadcastPkts          9
#define IfHCOutOctets               10
#define IfHCOutUcastPkts            11
#define IfHCOutMulticastPkts        12
#define IfHCOutBroadcastPkts        13
#define IfLinkUpDownTrapEnable      14
#define IfHighSpeed                 15
#define IfPromiscuousMode           16
#define IfConnectorPresent          17
#define IfAlias                     18
//#define IfCounterDiscontinuityTime   20

void            init_system_mib(void);
extern FindVarMethod var_system;

#define VERSION_DESCR       1
#define VERSIONID           2
#define UPTIME              3
#define SYSCONTACT          4
#define SYSTEMNAME          5
#define SYSLOCATION         6
#define SYSSERVICES         7
#define SYSORLASTCHANGE     8


#define IPRAN_SNMP_RETURN_OID(V, L) \
  do { \
    *var_len = (L)*sizeof(oid); \
    return (u_char *)(V); \
  } while (0)


#if !defined(NELEMENTS)
#define NELEMENTS(x) ((sizeof(x))/(sizeof((x)[0])))
#endif


#endif                          /* _MIBGROUP_SYSTEM_MIB_H */

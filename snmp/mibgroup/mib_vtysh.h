/*
 *
 *
 */
#ifndef _HHR_VTYSH_H
#define _HHR_VTYSH_H

#define HHRVTYSH                        1,3,6,1,4,1,9966,5,35,17

/* scalar */
#define hhrVtyshLoginTimeout            1
#define hhrVtyshLoginAuthType           2


/* vtysh user table */
#define hhrVtyshPrivilege               2

/* vtysh user online table */
#define hhrVtyshUserName                2
#define hhrVtyshUserFrom                3
#define hhrVtyshUserPrivilege           4


/* table define */
#define VTYSH_USER_TABLE                0
#define VTYSH_ONLINE_USER_TABLE         1

/* get-next operate interval time(seconds) */
#define VTYSH_TABLE_GET_NEXT_INTERVAL               1
#define VTYSH_USER_TABLE_REFRESH_TIME               2


/*
 * init_xxx(void)
 */
void init_hhr_vtysh(void);

#endif                          /* _HHR_VTYSH_H */

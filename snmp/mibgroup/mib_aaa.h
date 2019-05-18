/*
 *
 *
 */
#ifndef _HHR_AAA_H
#define _HHR_AAA_H

#define HHRAAA                          1,3,6,1,4,1,9966,5,35,15


/* common config item */
#define hhrAAAUserMaxIdleTime           1
#define hhrAAAUserMaxLoginNum           2
#define hhrAAAUserDefaultLoginLevel     3
#define hhrAAAAuthorizeFailedOnline     4
#define hhrAAAAccountFailedOnline       5
#define hhrAAAConsoleAdminMode          6
#define hhrAAALoginMode1                7
#define hhrAAALoginMode2                8
#define hhrAAALoginMode3                9
#define hhrAAALoginMode4                10
#define hhrAAANoneUserLevel             11


/* radius config item */
#define hhrAAARadiusAuthServerIP        1
#define hhrAAARadiusAuthServerL3vpn     2
#define hhrAAARadiusAcctServerIP        3
#define hhrAAARadiusAcctServerL3vpn     4
#define hhrAAARadiusAcctSwitch          5
#define hhrAAARadiusAcctUpdateInterval  6
#define hhrAAARadiusKey                 7
#define hhrAAARadiusRetryTimes          8
#define hhrAAARadiusRetryInterval       9
#define hhrAAARadiusResponseTimeout     10
#define hhrAAARadiusAuthMode            11


/* tacacs+ config item */
#define hhrAAATacacsAuthenServerIP      1
#define hhrAAATacacsAuthenServerL3vpn   2
#define hhrAAATacacsAuthorServerIP      3
#define hhrAAATacacsAuthorServerL3vpn   4
#define hhrAAATacacsAuthorSwitch        5
#define hhrAAATacacsAcctServerIP        6
#define hhrAAATacacsAcctServerL3vpn     7
#define hhrAAATacacsAcctSwitch          8
#define hhrAAATacacsAcctUpdateInterval  9
#define hhrAAATacacsKey                 10
#define hhrAAATacacsResponseTimeout     11


/* enable config item */
#define hhrAAAEnablePasswordLevel0      1
#define hhrAAAEnablePasswordLevel1      2
#define hhrAAAEnablePasswordLevel2      3
#define hhrAAAEnablePasswordLevel3      4
#define hhrAAAEnablePasswordLevel4      5
#define hhrAAAEnablePasswordLevel5      6
#define hhrAAAEnablePasswordLevel6      7
#define hhrAAAEnablePasswordLevel7      8
#define hhrAAAEnablePasswordLevel8      9
#define hhrAAAEnablePasswordLevel9      10
#define hhrAAAEnablePasswordLevel10     11
#define hhrAAAEnablePasswordLevel11     12
#define hhrAAAEnablePasswordLevel12     13
#define hhrAAAEnablePasswordLevel13     14
#define hhrAAAEnablePasswordLevel14     15
#define hhrAAAEnablePasswordLevel15     16


/* user config table */
#define hhrAAAUserConfigUsername        2
#define hhrAAAUserConfigPassword        3
#define hhrAAAUserConfigLevel           4
#define hhrAAAUserConfigMaxRepeatedNum  5


/* user online table */
#define hhrAAAUserOnlineUsername        2
#define hhrAAAUserOnlineLoginFrom       3
#define hhrAAAUserOnlineLevelAuthor     4
#define hhrAAAUserOnlineLevelCurrent    5
#define hhrAAAUserOnlineLoginMethod     6
#define hhrAAAUserOnlineTimeLogin       7
#define hhrAAAUserOnlineTimeOnline      8


/* user log table */
#define hhrAAAUserLogUsername           2
#define hhrAAAUserLogLevel              3
#define hhrAAAUserLogLoginMethod        4
#define hhrAAAUserLogLoginFrom          5
#define hhrAAAUserLogLoginResult        6
#define hhrAAAUserLogTimeLogin          7
#define hhrAAAUserLogTimeExit           8



/* table define */
#define AAA_SNMP_USER_CONFIG_TABLE      0
#define AAA_SNMP_USER_ONLINE_TABLE      1
#define AAA_SNMP_USER_LOG_TABLE         2



/**************************************** H3C ********************************************/

#define H3C_USER_MIB_OID                1,3,6,1,4,1,9966,2,201,2,12,1

/* local user config table */
#define h3c_hhrUserName                 1
#define h3c_hhrPassword                 2
#define h3c_hhrAuthMode                 3
#define h3c_hhrUserLevel                4
#define h3c_hhrUserState                5
#define h3c_hhrUserOnlineRowStatus      6
#define h3c_hhrUserIndex                7


/**************************************** END ********************************************/




/*
 * init_xxx(void)
 */
void init_hhr_aaa(void);

#endif                          /* _HHR_AAA_H */

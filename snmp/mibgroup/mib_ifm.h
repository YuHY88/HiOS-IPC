/*
 *  cli_set group interface - ifm_snmp.h
 *
 */
#ifndef _IFM_SNMP_H
#define _IFM_SNMP_H

#define IFM_REFRESH_TIME                3

#define IFM_HHRIFEXT 1,3,6,1,4,1,9966,5,35,1

/**/
#define hhrBaseIfDescr                   1
#define hhrBaseIfMode                    2
#define hhrBaseIfPortType                3
#define hhrBaseIfMtu                     4
#define hhrBaseIfWorkMode                5
#define hhrBaseIfSpeed                   6
#define hhrBaseIfFlappingControl         7
#define hhrBaseIfJumboFrame              8
#define hhrBaseIfAdminStatus             9
#define hhrBaseIfTPId                   10
#define hhrBaseIfStatistics             11
#define hhrBaseIfL3Vpn                  12
#define hhrBaseIfPhysAddress            13
#define hhrBaseIfOperStatus             14
#define hhrBaseIfLoopbackStatus         15
#define hhrBaseIfLoopbackType           16
#define hhrBaseIfLoopbackvlan           17
#define hhrBaseIfLoopbackIP             18
#define hhrBaseIfLoopbackMAC            19
#define hhrBaseIfAliase                 20
#define hhrBaseIfMasterMode             21
#define hhrBaseIfMasterState            22


/*hhrL3IfIPv4Table*/
#define hhrL3IpAddress                   1
#define hhrL3IfDescr                     2
#define hhrL3IfIPv4Type                  3
#define hhrL3IfUnnumberedIfIndex         4
#define hhrL3IfUnnumberedIfDesc          5


/*hhrSubIfVlanEncapsulateTable*/
#define hhrSubIfDescr                    1
#define hhrSubIfMode                     2
#define hhrSubIfEncapsulate              3
#define hhrSubIfcvlan                    4
#define hhrSubIfsvlan                    5

/*hhrL2SubIfVlanTranslateTable*/
#define hhrVTSubIfDescr                  1
#define hhrVTMode                        2
#define hhrVTEncapsulate                 3
#define hhrVTcvlan                       4
#define hhrVTsvlan                       5
#define hhrVTTranslateType               6
#define hhrVTTranslatedcvlan             7
#define hhrVTTranslatedcvlanCos          8
#define hhrVTTranslatedsvlan             9
#define hhrVTTranslatedsvlanCos         10

/*hhrL2SubIfVlanTranslateTable*/
#define hhrISInputTotalBytes             2
#define hhrISInputTotalPackets           3
#define hhrISInputDropPackets            4
#define hhrISOutputTotalBytes            5
#define hhrISOutputTotalPackets          6
#define hhrISOutputDropPackets           7
#define hhrISInputMulticastPackets       8
#define hhrISInputBroadcastPackets       9
#define hhrISInputUnicastPackets        10
#define hhrISOutputMulticastPackets     11
#define hhrISOutputBroadcastPackets     12
#define hhrISOutputUnicastPackets       13
#define hhrISBandwidthUtlization        14

/*hhrSFPConfigTable*/
#define hhrSFPAlsIfDescr                 1
#define hhSFPAlsEnable                   2
#define hhSFPTsEnable                    3
#define hhrSFPAlsOffTime                 4
#define hhrSFPAlsOnTime                  5

enum IFM_SNMP_VLANT
{
    NO_TRANS = 1,
    TRANS_TO_ONE_LAYER,
    TRANS_TO_TOW_LAYER,
    TRANS_OUT_KEEP_INNER,
    DELETE_OUT_KEEP_INNER
};

enum IFM_SNMP_IFMODE
{
    MODE_SWITCH = 1,
    MODE_L2,
    MODE_L3,
    MODE_PHY
};

enum IFM_SNMP_SPEED
{
    SPEED_INVALID = 0,
    SPEED_10MF,
    SPEED_10MH,
    SPEED_100MF,
    SPEED_100MH,
    SPEED_1000MF,
    SPEED_1000MH,
    SPEED_10GF,
    SPEED_10GH
};

enum IFM_SNMP_OPERSTATUS
{
    OPERSTA_UP = 1,
    OPERSTA_DOWN,
    OPERSTA_TEST
};

enum IFM_SNMP_ADMINSTATUS
{
    ADMINSTA_UP = 1,
    ADMINSTA_DOWN,
    ADMINSTA_TEST
};

enum IFM_SNMP_STATENABLE
{
    STAT_ENABLE = 1,
    STAT_DISABLE
};

enum IFM_SNMP_FIBERTYPE
{
    FIBERTYPE_OTHER = 1,
    FIBERTYPE_COPPER,
    FIBERTYPE_FIBER
};
enum IFM_SNMP_LOOPBACKTYPE
{
    NO_LOOPBACK = 1,
    EXTERNAL_LOOPBACK,
    INTERNAL_LOOPBACK
};

void init_ifm_snmp(void);

#endif                          /* _IFM_SNMP_H */

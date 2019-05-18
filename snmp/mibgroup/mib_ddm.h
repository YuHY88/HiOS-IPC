/*
 * mib_ddm.h
 *
 */
#ifndef _MIB_DDM_H
#define _MIB_DDM_H

/*sfp */
#define SFP_BASE_NAME_SIZE                               16
#define SFP_BASE_PN_SIZE                                 16
#define SFP_BASE_VENDOR_REV_SIZE                          4
#define SFP_BASE_VENDOR_OUI_SIZE                          3
#define SFP_BASE_CONNECTOR_SIZE                          45
#define SFP_BASE_NOMINAL_BR_SIZE                         16
#define SFP_BASE_VENDOR_SPECIFIC_SIZE                    32
#define SFP_BASE_IDENTIFIER_SIZE                         50
#define SFP_BASE_TRANSMISSION_MEDIA_SIZE                 20
#define SFP_BASE_FC_TRANSMISSION_MEDIA_SIZE              30


/*hal sfp */
struct hal_sfp_thershold
{
    int32_t current;
    int32_t high_alarm;
    int32_t low_alarm;
    int32_t high_warn;
    int32_t low_warn;
};

/*sfp eeprom */
#if 0
struct hal_ifm_sfp
{
    /*base eeprom:A0h*/
    int8_t vendor_pn[SFP_BASE_PN_SIZE + 1];
    int8_t connector[SFP_BASE_CONNECTOR_SIZE];
    int8_t nominal_br[SFP_BASE_NOMINAL_BR_SIZE];
    int8_t transmission_media[SFP_BASE_TRANSMISSION_MEDIA_SIZE];
    int8_t vendor_name[SFP_BASE_NAME_SIZE + 1];
    int8_t vendor_specific[SFP_BASE_VENDOR_SPECIFIC_SIZE + 1];
    int8_t vendor_oui[SFP_BASE_VENDOR_OUI_SIZE + 1];
    int8_t vendor_rev[SFP_BASE_VENDOR_REV_SIZE + 1];
    int8_t identifier[SFP_BASE_IDENTIFIER_SIZE];
    int8_t fc_transmission_media[SFP_BASE_FC_TRANSMISSION_MEDIA_SIZE];
    uint32_t wave_length;
    uint32_t transmission_distance;
    /*diag eeprom:A2h*/
    uint32_t  tx_disable;
    uint32_t  tx_fault;
    uint32_t  rx_loss;
    struct hal_sfp_thershold temperature;
    struct hal_sfp_thershold voltage;
    struct hal_sfp_thershold bias;
    struct hal_sfp_thershold tx_power;
    struct hal_sfp_thershold rx_power;
};

struct  hal_sfp_info
{
    uint32_t ifindex;
    struct hal_ifm_sfp info;
};
#endif
#define MIB_HHRDDM_ROOT          1,3,6,1,4,1,9966,5,35,8

/*hhrDDMInfoTable*/
#define hhrDDMIfDescr                                2
#define hhrDDMVendorPN                               3
#define hhrDDMVendorName                             4
#define hhrDDMVendorOUI                              5
#define hhrDDMVendorRev                              6
#define hhrDDMVendorPrivateInformation               7
#define hhrDDMTransceiverIdentifier                  8
#define hhrDDMFibreChannelTransmissionMedia          9
#define hhrDDMTransmissionMedia                     10
#define hhrDDMConnector                             11
#define hhrDDMNominalBR                             12
#define hhrDDMWaveLength                            13
#define hhrDDMTransmissionDistance                  14
#define hhrDDMTXDisableState                        15
#define hhrDDMTXFaultState                          16
#define hhrDDMRxLOSState                            17

#define hhrDDMCurrentSFPmoduleTemperature           18
#define hhrDDMHighTemperatureAlarmThreshold         19
#define hhrDDMLowTemperatureAlarmThreshold          20
#define hhrDDMHighTemperatureWarnThreshold          21
#define hhrDDMLowTemperatureWarnThreshold           22

#define hhrDDMCurrentSFPModuleVoltage               23
#define hhrDDMHighVoltageAlarmThreshold             24
#define hhrDDMLowVoltageAlarmThreshold              25
#define hhrDDMHighVoltageWarnThreshold              26
#define hhrDDMLowVoltageWarnThreshold               27

#define hhrDDMCurrentSFPmoduleTxBias                28
#define hhrDDMHighTxBiasAlarmThreshold              29
#define hhrDDMLowTxBiasAlarmThreshold               30
#define hhrDDMHighTxBiasWarnThreshold               31
#define hhrDDMLowTxBiasWarnThreshold                32

#define hhrDDMCurrentSFPmoduleTxpower               33
#define hhrDDMHighTxpowerAlarmThreshold             34
#define hhrDDMLowTxpowerAlarmThreshold              35
#define hhrDDMHighTxpowerWarnThreshold              36
#define hhrDDMLowTxpowerWarnThreshold               37

#define hhrDDMCurrentSFPmoduleRxPower               38
#define hhrDDMHighRxPowerAlarmThreshold             39
#define hhrDDMLowRxPowerAlarmThreshold              40
#define hhrDDMHighRxPowerWarnThreshold              41
#define hhrDDMLowRxPowerWarnThreshold               42
#define hhrDDMVendorSN                              43

void init_mib_ddm(void);

#endif                          /* _VLAN_SNMP_H */


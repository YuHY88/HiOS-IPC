#!/bin/bash

PREPATH=${PWD}

if [ ! ${USEDTYPE} ]; then
	echo "Error! USEDTYPE undefine."
	exit
fi

if [ "${USEDTYPE}"x = "HT201"x ];then
	MY_ENDIANNESS="little"
else
	MY_ENDIANNESS="big"
fi


chmod +x configure
./configure \
	--build=i686-pc-linux-gnu \
	--host=${ARCH}-linux \
	--target=${ARCH}-linux \
	--with-cc=${CROSS_COMPILE}gcc \
	--with-ar=${CROSS_COMPILE}ar \
	--with-endianness=${MY_ENDIANNESS} \
	--without-rpm \
	--disable-manuals \
	--disable-local-smux \
	--disable-scripts \
	--disable-applications \
	--disable-testing-code \
	--disable-shared \
	--enable-ipv6 \
	--enable-ipc \
	--with-mib-modules="system_mib vacm_conf mib_cliset mib_ifm mib_vlan mib_l3func \
                            mib_lldp mib_mpls mib_dhcp mib_qos mib_system mib_tdm mib_stm mib_cfm \
                            mib_oam mib_alarm mib_ntp mib_filem mib_trunk mib_loopdetect mib_efm mib_efm_agent \
                            mib_synce mib_aaa mib_port_properties mib_l2func mib_sla mib_ddm mib_snmp mib_rmon mib_erps mib_l2cp \
                            mib_spliter mib_stat mib_h3c_ospf mib_route mib_msdhoversnmp mib_vcgif  \
			    mib_isolate mib_mstp mib_flowcontrol" \
	--with-default-snmp-version="3" \
	--with-sys-contact=" " \
	--with-sys-location="china" \
	--with-logfile="/var/net-snmp/snmpd.log" \
	--with-persistent-directory="/var/net-snmp" \
	--with-cflags="-I ${PREPATH}/../ -I ${PREPATH}/../lib/gpnDevInfo -I ${PREPATH}/../lib/gpnLog -I ${PREPATH}/../lib/gpnSocket -DNETSNMP_NO_INLINE " \
	--with-libs=" -L${ZEBOS_PREPATH}/lib -lzebra -lpthread -lgcov" 

make clean
make

if [ ! -f ${PREPATH}/agent/snmpd ]; then
echo "compile snmp failure!"
exit 1
else
cp ${PREPATH}/agent/snmpd ${ZEBOS_PREPATH}/sbin
exit 0
fi


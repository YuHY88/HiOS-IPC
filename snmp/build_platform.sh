#!/bin/bash

if [ ! ${PROJ_PLAT} ]; then
	#select project
	echo "" 
	echo "Please input project: " 
	echo "  1: H20RN181" 
	echo "  2: H20RN2000/H20RN1000" 
	echo "  3: HT201" 
	echo "  4: H9MOLMXE_VX" 

	while true; do
		read -p "BUILD SNMP>" PROJV
		if [ ! ${PROJV} ]; then
			echo "Please input project, Retry: "
			continue
		else
			if [ "${PROJV}"x = "1"x ];then
				PROJ_PLAT="H20RN181"
				break
			elif [ "${PROJV}"x = "2"x ];then
				PROJ_PLAT="H20RN2000"
				break
			elif [ "${PROJV}"x = "3"x ];then
				PROJ_PLAT="HT201"
				break
			elif [ "${PROJV}"x = "4"x ];then
				PROJ_PLAT="H9MOLMXE_VX"
				break
			else
				echo "Please input project, Retry: "
				continue
			fi
		fi
	done
fi

if [ "${PROJ_PLAT}"x = "H20RN181"x ];then
	echo "Start build snmp: H20RN181..."
	PATH=$PATH:/opt/mips/eldk4.1/usr/bin/
	CROSS_CC="mips_4KC"
	ARCH_TMP="mipsel-linux"
	MY_ENDIANNESS="big"
elif [ "${PROJ_PLAT}"x = "H20RN2000"x ];then
	echo "Start build snmp: H20RN2000/H20RN1000..."
	PATH=$PATH:/opt/ppc/eldk4.2/usr/bin/
	CROSS_CC="ppc_85xxDP"
	ARCH_TMP="powerpc-linux-gnu"
	MY_ENDIANNESS="big"
elif [ "${PROJ_PLAT}"x = "HT201"x ];then
	echo "Start build snmp: HT201..."
	PATH=$PATH:/opt/arm/usr/bin:/opt/arm/
	CROSS_CC="arm-linux"
	ARCH_TMP="arm-linux"
	MY_ENDIANNESS="little"
elif [ "${PROJ_PLAT}"x = "H9MOLMXE_VX"x ];then
	echo "Start build snmp: H9MOLMXE_VX..."
	PATH=$PATH:/opt/ppc/eldk4.2/usr/bin/
	CROSS_CC="ppc_85xxDP"
	ARCH_TMP="powerpc-linux-gnu"
	MY_ENDIANNESS="big"
else
	echo "project unknown, exit!"
	exit
fi
echo ""

PREPATH=${PWD}
CROSS_COMPILE=${CROSS_CC}

chmod +x configure
./configure \
	--build=i686-pc-linux-gnu \
	--host=${ARCH_TMP} \
	--with-cc=${CROSS_COMPILE}-gcc \
	--with-ar=${CROSS_COMPILE}-ar \
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
                            mib_spliter mib_stat mib_h3c_ospf mib_route mib_msdhoversnmp mib_vcgif mib_mstp mib_flowcontrol " \
	--with-default-snmp-version="3" \
	--with-sys-contact=" " \
	--with-sys-location="china" \
	--with-logfile="/var/net-snmp/snmpd.log" \
	--with-persistent-directory="/var/net-snmp" \
	--with-cflags="-I ${PREPATH}/../ -I ${PREPATH}/../lib/gpnDevInfo -I ${PREPATH}/../lib/gpnLog -I ${PREPATH}/../lib/gpnSocket -DNETSNMP_NO_INLINE " \
	--with-libs=" -L${PREPATH}/../Output/lib -lzebra -lpthread" 

make clean
make

cp ${PREPATH}/agent/snmpd ${PREPATH}/../Output/sbin


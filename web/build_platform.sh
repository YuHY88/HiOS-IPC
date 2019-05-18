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
export ZEBOS_PREPATH=${PWD}/../Output
export CROSS_COMPILE=${CROSS_CC}-

mkdir ${PREPATH}/output/
mkdir ${PREPATH}/web/
rm -f ${PREPATH}/output/*
rm -f ${PREPATH}/web/*.cgi

cd server
make clean
make
cd ../

cd cgi
make clean
make
cd ../

cp ${PREPATH}/output/* ${ZEBOS_PREPATH}/sbin/
mkdir -p ${ZEBOS_PREPATH}/sbin/web
cp ${PREPATH}/web/* ${ZEBOS_PREPATH}/sbin/web/


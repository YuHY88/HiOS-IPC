#!/bin/bash

PMT="IPRAN-PLATFORM-BUILD>"

#select project
echo "" 
echo "Please input project: " 
echo "  1: H20RN181" 
echo "  2: H20RN2000/H20RN1000" 
echo "  3: HT201" 
echo "  4: H9MOLMXE_VX" 

while true; do
	read -p ${PMT} PROJV
	if [ ! ${PROJV} ]; then
		echo "Please input project, Retry: "
		continue
	else
		if [ "${PROJV}"x = "1"x ];then
			echo "Start build: H20RN181..."
			echo ""
			export PROJ_PLAT="H20RN181"
			PATH=$PATH:/opt/mips/eldk4.1/usr/bin/
			CROSS_CC="mips_4KC-gcc"
			HOST_PARA="mipsel-linux"
			USEDTYPE="H20RN181"
			break
		elif [ "${PROJV}"x = "2"x ];then
			echo "Start build: H20RN2000/H20RN1000..."
			echo ""
			export PROJ_PLAT="H20RN2000"
			PATH=$PATH:/opt/ppc/eldk4.2/usr/bin/
			CROSS_CC="ppc_85xxDP-gcc"
			HOST_PARA="powerpc-linux-gnu"
			USEDTYPE="H20RN2000"
			break
		elif [ "${PROJV}"x = "3"x ];then
			echo "Start build: HT201..."
			echo ""
			export PROJ_PLAT="HT201"
			PATH=$PATH:/opt/arm/usr/bin:/opt/arm/
			CROSS_CC="arm-linux-gcc"
			HOST_PARA="arm-linux"
			USEDTYPE="HT201"
			break
		elif [ "${PROJV}"x = "4"x ];then
			echo "Start build: H9MOLMXE_VX..."
			echo ""
			export PROJ_PLAT="H9MOLMXE_VX"
			PATH=$PATH:/opt/ppc/eldk4.2/usr/bin/
			CROSS_CC="ppc_85xxDP-gcc"
			HOST_PARA="powerpc-linux-gnu"
			USEDTYPE="H20RN2000"
			break
		else
			echo "Please input project, Retry: "
			continue
		fi
	fi
done

PREPATH=${PWD}
rm ${PREPATH}/Output -rf
mkdir ${PREPATH}/Output

chmod +x configure
./configure \
	CC=${CROSS_CC} \
	--host=${HOST_PARA} \
	--prefix=${PREPATH}/Output \
	--enable-user=root \
	--enable-group=root \
	--enable-vty-group=root \
	--disable-rtadv \
	--disable-ribd	\
	--disable-ripngd	\
	--disable-ospfclient	\
	--disable-ripngd \
	--disable-doc

make
make install

#build snmp
cd snmp
chmod +x build_platform.sh
./build_platform.sh
cd ..

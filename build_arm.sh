#!/bin/bash

export PATH=$PATH:/opt/arm/usr/bin:/opt/arm/usr/
PREPATH=${PWD}
rm ${PREPATH}/Output -rf
mkdir ${PREPATH}/Output

chmod +x configure
#if false;then
./configure \
	CC=arm-linux-gcc \
	--host=arm-linux \
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
#fi

make
make install

#build snmp
cd snmp
#chmod +x build_platform.sh
#./build_platform.sh
cd ..


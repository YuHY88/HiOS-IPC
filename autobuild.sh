#!/bin/bash

rm ${ZEBOS_PREPATH} -rf
mkdir ${ZEBOS_PREPATH} -p

chmod +x configure

./configure \
	CC=${CROSS_COMPILE}gcc \
	--host=${ARCH_HOST} \
	--prefix=${ZEBOS_PREPATH} \
	--enable-user=root \
	--enable-group=root \
	--enable-vty-group=root \
	--disable-rtadv \
	--disable-bgpd	\
	--disable-ribd	\
	--disable-ripngd	\
	--disable-ospfclient	\
	--disable-aaa \
	--disable-ripngd\
	--disable-ntp \
	--disable-doc

make clean
make
make install




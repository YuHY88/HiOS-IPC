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
	--disable-ipmc \
	--disable-rtadv \
	--disable-dhcp \
	--disable-bgpd \
	--disable-isis \
	--disable-ripd \
	--disable-ripngd \
	--disable-ospf6d \
	--disable-ospfd \
	--disable-ospfclient \
	--disable-ntp \
	--disable-ribd \
	--disable-doc

make
make install




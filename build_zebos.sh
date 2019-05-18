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
	--enable-shared=no \
	--disable-rtadv \
	--disable-ribd	\
	--disable-ripngd	\
	--disable-ospfclient	\
	--disable-ripngd \
	--disable-doc

make clean
rm -f ./mpls/bfd/*.o
rm -f ./mpls/ldp/*.o
rm -f ./mpls/mpls_oam/*.o
rm -f ./ipmc/*.o
rm -f ./ipmc/igmp/*.o
rm -f ./ipmc/pim/*.o
make
make install


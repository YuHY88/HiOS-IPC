#!/bin/bash

rm ${ZEBOS_PREPATH} -rf
mkdir ${ZEBOS_PREPATH} -p

function clear_dir(){
    for element in `ls $1`
    do
        dir_or_file=$1"/"$element
        if [ -d $dir_or_file ]; then
            echo $dir_or_file
            rm -rf $dir_or_file/.libs
            rm -rf $dir_or_file/.deps
            rm -rf $dir_or_file/*.o
            rm -rf $dir_or_file/*.a
            clear_dir $dir_or_file
        fi  
    done
}
clear_dir $PWD

chmod +x configure

./configure \
	CC=${CROSS_COMPILE}gcc \
	--host=${ARCH_HOST} \
	--prefix=${ZEBOS_PREPATH} \
	--enable-user=root \
	--enable-group=root \
	--enable-vty-group=root \
	--enable-shared=no \
	--enable-coverage=no \
	--enable-release=yes \
	--enable-jemalloc=no \
	--disable-rtadv \
	--disable-ribd	\
	--disable-ripngd	\
	--disable-ospfclient	\
	--disable-ripngd \
	--disable-ptp \
	--disable-doc \
	--enable-kernel_3_0

make clean
rm -f ./mpls/bfd/*.o
rm -f ./mpls/ldp/*.o
rm -f ./mpls/mpls_oam/*.o
rm -f ./ipmc/*.o
rm -f ./ipmc/igmp/*.o
rm -f ./ipmc/pim/*.o
make
make install

if [ $? -ne 0 ]
then 
   exit 1
fi

#build snmp
cd snmp
chmod +x build.sh
./build.sh
cd ..

if [ $? -ne 0 ]
then 
   exit 1
fi



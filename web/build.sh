#!/bin/bash

PREPATH=${PWD}

mkdir ${PREPATH}/output/
mkdir ${PREPATH}/web/
rm -f ${PREPATH}/output/*
rm -f ${PREPATH}/web/*.cgi

cd server
make clean
make
cd ..

cd cgi
make clean
make
cd ..

cp ${PREPATH}/output/* ${ZEBOS_PREPATH}/sbin/
mkdir -p ${ZEBOS_PREPATH}/sbin/web
cp ${PREPATH}/web/* ${ZEBOS_PREPATH}/sbin/web/


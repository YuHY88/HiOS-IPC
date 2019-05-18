
.PHONY: all clean

LDIR = ${PWD}
LIBS = -lcrypt -lrt -lcap -L ${ZEBOS_PREPATH}/lib -lzebra -lpthread -lgcov
CFLAGS = -g -pipe -Wall -I ${LDIR}/../../

source = leftframe.c \
			index.c \
			banner.c \
			if_basic.c \
			if_attr.c \
			if_l3.c 

target = $(patsubst %.c, %, ${source})  
  
all: ${target}  

%:%.c  
	${CROSS_COMPILE}gcc   $^ cgic.c -o $@.cgi ${LIBS} ${CFLAGS}
	cp $@.cgi ${LDIR}/../web/
clean:
	rm -f *.cgi



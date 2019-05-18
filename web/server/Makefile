# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in,v 1.59 2002/03/24 22:20:19 jnelson Exp $

.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean depend all

GCC_FLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual\
  -Wtraditional\
  -Wshadow\
  -Wconversion\
  -Waggregate-return\
  -Wmissing-prototypes\
  -Wnested-externs\
  -Wall \
  -Wundef -Wwrite-strings -Wredundant-decls -Winline

LDIR = ${PWD}
srcdir = .
LDFLAGS = -g
LIBS = -lcrypt -lrt -lcap -L ${ZEBOS_PREPATH}/lib -lzebra -lpthread -lgcov
CFLAGS = -g -pipe -Wall -I ${LDIR}/../../

#YACC = bison -y 
#LEX = flex 
CC = ${CROSS_COMPILE}gcc 
CPP = ${CROSS_COMPILE}gcc -E

SOURCES = alias.c boa.c buffer.c cgi.c cgi_header.c config.c escape.c \
	get.c hash.c ip.c log.c mmap_cache.c pipe.c queue.c read.c \
	request.c response.c select.c signals.c util.c sublog.c boa_packet.c

OBJS = y.tab.o lex.yy.o $(SOURCES:.c=.o) timestamp.o 

all: webd

webd: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) ${CFLAGS}
	cp $@ ${LDIR}/../output/

#boa_indexer:	index_dir.o escape.o   
#	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
#	rm -f $(OBJS) webd core lex.yy.c y.tab.c y.tab.h *~ boa_indexer index_dir.o   
	rm -f $(OBJS) webd  *~ 

# parser dependencies
#y.tab.c y.tab.h: boa_grammar.y
#	$(YACC) -d $^

#lex.yy.c: boa_lexer.l
#	$(LEX) $^

# timestamp
#timestamp.o: $(SOURCES) boa_grammar.y boa_lexer.l

# depend stuff
.depend:
	$(CPP) -MM $(SOURCES) -I ${LDIR}/../../ > .depend

depend:
	-rm -f .depend
	$(MAKE) .depend

include .depend

# tags
tags: $(SOURCES)
	ctags -o tags $^ *.h
        
# object dump
boa.objdump: webd
	objdump --disassemble-all --source boa > $@


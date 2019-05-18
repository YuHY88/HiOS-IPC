CROSS_COMPILE=mips_4KC-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar 
LD=$(CROSS_COMPILE)ld

export CROSS_COMPILE CC AR LD

#CFLAGS += -I/path/to/net/
LDFLAGS += -static
SHARELIB = 
export LDFLAGS

TARGET = libnet.a
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

#$(OBJS): $(SRCS)
#	$(CC) $^ -o $@

all: $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS)

clean:
	@ rm -f *.o *.a






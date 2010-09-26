#
# Makefile for the kernel proper
#

CC      = gcc
AS	= as
LD      = ld
CFLAGS  = -c -fomit-frame-pointer -Wall -I. -I../include -mcpu=arm920t -MMD \
          -msoft-float
ASFLAGS	= -mcpu=arm920t -mapcs-32
LDFLAGS = -init main -Map kernel.map -N -T orex.ld \
          -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -L../lib

OBJS    = new-swi.o switch.o regdump.o
DEPS    = $(OBJS:.o=.d)
EXEC    = StrombolOS.elf

# So that ``all'' isn't considered a normal rule
.PHONY: all
# So that GNU Make doesn't delete our temporary files
.PRECIOUS: %.s %.o

all: ${EXEC}

# Use this to cancel the built-in rule
%.o : %.c

%.s: %.c
	${CC} -S ${CFLAGS} $^

%.o: %.s
	${AS} ${ASFLAGS} -o $@ $<

${EXEC}: ${OBJS}
	${LD} ${LDFLAGS} -o $@ ${OBJS} -lbwio -lgcc

clean:
	rm -rf ${DEPS} ${OBJS} ${EXEC} *.map

copy:
	cp ${EXEC} /u3/cs452/tftpboot/ARM/

-include ${DEPS}

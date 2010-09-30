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

OBJS    = kernel.o switch.o regdump.o user.o usyscall.o\
          create.o mytid.o myparenttid.o pass.o exit.o
DEPS    = $(OBJS:.o=.d)
ASMS	= $(OBJS:.o=.s)
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

# Use special built-ins to filter out the .s files we care about
clean:
	rm -rf ${DEPS} ${OBJS} ${EXEC} *.map $(filter-out switch.s regdump.s usyscall.s,$(ASMS))

jcopy:
	cp ${EXEC} /u3/cs452/tftpboot/ARM/StrombolOS/j3parker.elf
kcopy: 
	cp ${EXEC} /u3/cs452/tftpboot/ARM/StrombolOS/kspaans.elf

-include ${DEPS}

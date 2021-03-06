#!/bin/sh

#
# ``Makefile'' for StrombolOS
#

CFLAGS="-S -fomit-frame-pointer -Wall -Iinclude -mcpu=arm920t -msoft-float \
        -fno-builtin \
       "
ASFLAGS="-mcpu=arm920t -mapcs-32"
LDFLAGS="-init main -Map build/kernel.map -N -T build/orex.ld \
         -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -Llib \
	"
OBJS="build/kernel.o build/switch.o build/user.o build/usyscall.o \
      build/tasks.o build/create.o build/mytid.o build/myparenttid.o \
      build/pass.o build/exit.o build/send.o build/receive.o build/reply.o \
      build/names.o build/rps_server.o build/lib.o build/rps_client.o \
     "

if [ `basename $PWD` != "k2" ]; then
	cd ..
fi

if [ ! -d "build" ]; then
	mkdir build
fi

# Could probably improve this with for loops

gcc ${CFLAGS}  -o build/user.s user/user.c || exit 1
as  ${ASFLAGS} -o build/user.o build/user.s || exit 1
 
gcc ${CFLAGS}  -o build/lib.s user/lib.c || exit 1
as  ${ASFLAGS} -o build/lib.o build/lib.s || exit 1

gcc ${CFLAGS}  -o build/rps_client.s user/rps_client.c || exit 1
as  ${ASFLAGS} -o build/rps_client.o build/rps_client.s || exit 1

as  ${ASFLAGS} -o build/usyscall.o user/usyscall.s || exit 1
 
gcc ${CFLAGS}  -o build/kernel.s kernel/kernel.c || exit 1
as  ${ASFLAGS} -o build/kernel.o build/kernel.s || exit 1

as  ${ASFLAGS} -o build/switch.o kernel/switch.s || exit 1

gcc ${CFLAGS}  -o build/tasks.s kernel/tasks.c || exit 1
as  ${ASFLAGS} -o build/tasks.o build/tasks.s || exit 1

gcc ${CFLAGS}  -o build/create.s kernel/syscalls/create.c || exit 1
as  ${ASFLAGS} -o build/create.o build/create.s || exit 1

gcc ${CFLAGS}  -o build/exit.s kernel/syscalls/exit.c || exit 1
as  ${ASFLAGS} -o build/exit.o build/exit.s || exit 1

gcc ${CFLAGS}  -o build/pass.s kernel/syscalls/pass.c || exit 1
as  ${ASFLAGS} -o build/pass.o build/pass.s || exit 1

#gcc ${CFLAGS}  -o build/rand.s kernel/syscalls/rand.c || exit 1
#as  ${ASFLAGS} -o build/rand.o build/rand.s || exit 1

gcc ${CFLAGS}  -o build/names.s servers/names.c || exit 1
as  ${ASFLAGS} -o build/names.o build/names.s || exit 1

gcc ${CFLAGS}  -o build/rps_server.s servers/rps_server.c || exit 1
as  ${ASFLAGS} -o build/rps_server.o build/rps_server.s || exit 1

gcc ${CFLAGS}  -o build/send.s kernel/syscalls/send.c || exit 1
as  ${ASFLAGS} -o build/send.o build/send.s || exit 1

gcc ${CFLAGS}  -o build/reply.s kernel/syscalls/reply.c || exit 1
as  ${ASFLAGS} -o build/reply.o build/reply.s || exit 1

gcc ${CFLAGS}  -o build/receive.s kernel/syscalls/receive.c || exit 1
as  ${ASFLAGS} -o build/receive.o build/receive.s || exit 1

gcc ${CFLAGS}  -o build/mytid.s kernel/syscalls/mytid.c || exit 1
as  ${ASFLAGS} -o build/mytid.o build/mytid.s || exit 1

gcc ${CFLAGS}  -o build/myparenttid.s kernel/syscalls/myparenttid.c || exit 1
as  ${ASFLAGS} -o build/myparenttid.o build/myparenttid.s || exit 1

ld  ${LDFLAGS} -o build/StrombolOS.elf ${OBJS} -lbwio -lgcc

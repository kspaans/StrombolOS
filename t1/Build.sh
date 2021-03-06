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
OBJS=" build/kernel.o build/switch.o build/user.o build/usyscall.o build/boot.o \
      build/tasks.o build/create.o build/mytid.o build/myparenttid.o \
      build/pass.o build/exit.o build/send.o build/receive.o build/reply.o \
      build/names.o build/rps_server.o build/lib.o build/rps_client.o \
      build/tests.o build/wrappers.o build/clock_client.o build/clock.o \
      build/uart1.o build/uart2.o build/trains_ui.o build/timings.o \
      build/notifier_clock.o build/awaitevent.o build/notifier_uart1rx.o \
      build/notifier_uart1tx.o build/notifier_uart2rx.o build/wm.o \
      build/notifier_uart2tx.o build/trains.o build/bwio.o build/track.o \
     build/lock.o build/cali.o"

if [ `basename $PWD` != "t1" ]; then
	cd ..
fi

if [ ! -d "build" ]; then
	mkdir build
fi

# Could probably improve this with for loops

gcc ${CFLAGS}  -o build/cali.s servers/cali.c || exit 1
as  ${ASFLAGS} -o build/cali.o build/cali.s || exit 1

gcc ${CFLAGS}  -o build/lock.s servers/lock.c || exit 1
as  ${ASFLAGS} -o build/lock.o build/lock.s || exit 1

gcc ${CFLAGS}  -o build/bwio.s bwio/bwio.c || exit 1
as  ${ASFLAGS} -o build/bwio.o build/bwio.s || exit 1

gcc ${CFLAGS}  -o build/boot.s kernel/boot.c || exit 1
as  ${ASFLAGS} -o build/boot.o build/boot.s  || exit 1

gcc ${CFLAGS}  -o build/trains.s servers/trains.c || exit 1
as  ${ASFLAGS} -o build/trains.o build/trains.s  || exit 1

gcc ${CFLAGS}  -o build/wm.s user/wm.c || exit 1
as  ${ASFLAGS} -o build/wm.o build/wm.s  || exit 1

gcc ${CFLAGS}  -o build/user.s user/user.c || exit 1
as  ${ASFLAGS} -o build/user.o build/user.s || exit 1
 
gcc ${CFLAGS}  -o build/lib.s user/lib.c || exit 1
as  ${ASFLAGS} -o build/lib.o build/lib.s || exit 1

gcc ${CFLAGS}  -o build/wrappers.s user/wrappers.c || exit 1
as  ${ASFLAGS} -o build/wrappers.o build/wrappers.s || exit 1

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

gcc ${CFLAGS}  -o build/awaitevent.s kernel/syscalls/awaitevent.c || exit 1
as  ${ASFLAGS} -o build/awaitevent.o build/awaitevent.s || exit 1

gcc ${CFLAGS}  -o build/tests.s ktests/tests.c || exit 1
as  ${ASFLAGS} -o build/tests.o build/tests.s || exit 1

gcc ${CFLAGS}  -o build/clock_client.s user/clock_client.c || exit 1
as  ${ASFLAGS} -o build/clock_client.o build/clock_client.s || exit 1

gcc ${CFLAGS}  -o build/clock.s servers/clock.c || exit 1
as  ${ASFLAGS} -o build/clock.o build/clock.s || exit 1

gcc ${CFLAGS}  -o build/uart1.s servers/uart1.c || exit 1
as  ${ASFLAGS} -o build/uart1.o build/uart1.s || exit 1

gcc ${CFLAGS}  -o build/uart2.s servers/uart2.c || exit 1
as  ${ASFLAGS} -o build/uart2.o build/uart2.s || exit 1

gcc ${CFLAGS}  -o build/notifier_clock.s servers/notifier_clock.c || exit 1
as  ${ASFLAGS} -o build/notifier_clock.o build/notifier_clock.s || exit 1

gcc ${CFLAGS}  -o build/notifier_uart1tx.s servers/notifier_uart1tx.c || exit 1
as  ${ASFLAGS} -o build/notifier_uart1tx.o build/notifier_uart1tx.s || exit 1

gcc ${CFLAGS}  -o build/notifier_uart1rx.s servers/notifier_uart1rx.c || exit 1
as  ${ASFLAGS} -o build/notifier_uart1rx.o build/notifier_uart1rx.s || exit 1

gcc ${CFLAGS}  -o build/notifier_uart1tx.s servers/notifier_uart1tx.c || exit 1
as  ${ASFLAGS} -o build/notifier_uart1tx.o build/notifier_uart1tx.s || exit 1

gcc ${CFLAGS}  -o build/notifier_uart2rx.s servers/notifier_uart2rx.c || exit 1
as  ${ASFLAGS} -o build/notifier_uart2rx.o build/notifier_uart2rx.s || exit 1

gcc ${CFLAGS}  -o build/notifier_uart2tx.s servers/notifier_uart2tx.c || exit 1
as  ${ASFLAGS} -o build/notifier_uart2tx.o build/notifier_uart2tx.s || exit 1

gcc ${CFLAGS}  -o build/trains_ui.s user/trains_ui.c || exit 1
as  ${ASFLAGS} -o build/trains_ui.o build/trains_ui.s || exit 1

gcc ${CFLAGS}  -o build/timings.s user/timings.c || exit 1
as  ${ASFLAGS} -o build/timings.o build/timings.s || exit 1

gcc ${CFLAGS}  -o build/track.s servers/track.c || exit 1
as  ${ASFLAGS} -o build/track.o build/track.s || exit 1

ld  ${LDFLAGS} -o build/StrombolOS.elf ${OBJS} -lgcc

if [ "$#" = "1" ]; then
	cp build/StrombolOS.elf /u/cs452/tftpboot/ARM/StrombolOS/$1 || exit 1
	echo "cp build/StrombolOS.elf /u/cs452/tftpboot/ARM/StrombolOS/$1"
fi

 /*
 * iotest.c
 */

#include <bwio.h>
#include <ts7200.h>

#define YAWN(n) for (i = 0; i < n; ++i);

int main( int argc, char* argv[] ) {
	int i, buf;
	int *line;
	char const str[] = "Kyle spaans is totally the coolest person ever, in the world.\n\r";
	bwsetfifo( COM2, OFF );
	bwputstr( COM2, str );
	bwputw( COM2, 10, '*', str );/*
	bwprintf( COM2, "Hello world.\n\r" );
	bwprintf( COM2, "%s world%u.\n\r", "Well, hello", 23 );
	bwprintf( COM2, "%d worlds for %u person.\n\r", -23, 1 );
	bwprintf( COM2, "%x worlds for %d people.\n\r", -23, 723 );*/
	//str[0] = bwgetc( COM2 );
	//bwprintf( COM2, "%s", str );
/***********************************/
	bwsetfifo(COM1, OFF);
	                                        /* 14.7456 MHz */ // 14,745,600 assume this is the right reference clock freq
	bwsetspeed(COM1, 2400); // uhhh? need: (refernece_clock / (16 * 2400)) - 1
	                        // uhhh? need: (       14745600 /   ( 38400 )) - 1
				                  // = 0000 0001  0111 1111 (383) // added this to bwio.c
	// p 130, 131 ---- 14,745,600 / 6144 = 2400
	// clock is synched with HCLK
	// use TIMER3_BASE + CTRL_OFFSET to setup, put MODE to 0 fro free running and clock select to 0 for 2KHz
	/* now set the stop bits */
	line = (int *)( UART1_BASE + UART_LCRH_OFFSET );
	buf = *line;
	//bwprintf(COM2, "blah1 %x\r\n", buf);
	buf |= STP2_MASK;
	buf |= WLEN_MASK;
	//bwprintf(COM2, "blah2 %x\r\n", buf);
	*line = buf;
	/* should be good */
	//YAWN(50000);
	//bwputc(COM1, 0x60);
	YAWN(50000);
	bwputc(COM1, 0x10);
	bwputc(COM1, 0x17);
	YAWN(50000);
	return 0;
}


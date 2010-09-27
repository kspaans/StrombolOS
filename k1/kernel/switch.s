	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	str	lr, [sp, #-4]!	@ writes-back decr'd value of sp
	swi	#0
	ldr	pc, [sp], #4	@ also performs a write-back to sp
	.size	swtch, .-swtch
	.ident	"GCC: (GNU) 4.0.2"

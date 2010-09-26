	.section	.rodata
	.align	2
.LC0:
	.ascii	"Me handler?\012\000"
	.text
	.align  2
	.global handle
	.type	handle, %function
handle:
	mov	r0, #1
	ldr	r1, .L4
	bl	bwputstr
	mov pc, lr
.L5:
	.align	2
.L4:
	.word	.LC0
	.size	fuck, .-fuck
	.text
	.align	2
	.global install_handler
	.type	install_handler, %function
install_handler:
	stmfd	sp, {r0, r1}
	sub	sp, sp, #8
	mov	r0, #0x28	@ handler pointer goes in 0x00000028
	ldr	r1, .L0		@ give it address of ``handle''
	str	r1, [r0]
	ldmfd	sp, {r0, r1}
	add	sp, sp, #8
	mov	pc, lr
.L1:
	.align 2
.L0:
	.word handle
	.size	install_handler, .-install_handler
	.ident	"GCC: (GNU) 4.0.2"

	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	str	lr, [sp, #-4]!	@ writes-back decr'd value of sp
	swi	#0
	ldr	lr, [sp], #4	@ also performs a write-back to sp
	mov	pc, lr
	.size	swtch, .-swtch
	.text
	.align  2
	.global change_mode
	.type   change_mode, %function
change_mode:
	stmfd	sp!, {r1}
	mrs	r1, CPSR
	bic	r1, r1, #0x1F
	orr	r1, r1, r0
	stmfd	sp!, {r2, r3}
	mov	r2, r13
	mov	r3, r14
	msr	CPSR_c, r1
	mov	r14, r3
	mov	r13, r2
	ldmfd	sp!, {r2, r3}
	ldmfd	sp!, {r1}
	mov	pc, lr
	.size	change_mode, .-change_mode
	.text
	.align  2
	.global print_mode
	.type   print_mode, %function
print_mode:
	str	lr, [sp, #-4]!;
	mov	r0, #1
	mrs	r1, CPSR
	bic	r1, r1, #0xFFFFFFE0 @ lol, turned into AND
	bl	bwputr(PLT)
	ldr	pc, [sp], #4
	.size	print_mode, .-print_mode
	.text
	.align  2
	.global activate_lower
	.type   activate_lower, %function
activate_lower:
	@@
	@@ r0 is pointer to task descriptor
	@@
	@ Save kernel state
	stmfd	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14}
	sub	sp, sp, #60
	
	@ restore user state, we get the CORRECT stack pointer from the TD
        ldr	r2, [r0, #4] @ pull sp out of td

        @ Change to user mode
	mrs	r3, CPSR
	bic	r3, r3, #0x1F
	orr	r3, r3, #0x10
        msr     CPSR_c, r3

	@ Restore the sp - ugliness because of register banking
	mov	sp, r2
	@ Now restore user state...
	@ ???

	@ Jump into the user
	ldr	pc, [sp, #-4]  @ this jumps to first()   should there be a bang??? think carefully later!!

	@ should not get here: mov	pc, lr ?????
	.size	activate_lower, .-activate_lower
	.ident	"GCC: (GNU) 4.0.2"

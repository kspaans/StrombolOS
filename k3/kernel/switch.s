	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	swi	#0
	mov	pc, lr
	.size	swtch, .-swtch
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
	.global activate
	.type   activate, %function
activate:
	@@
	@@ r0 is pointer to task descriptor
	@@ r1 is return value to user task
	@@
	stmfd	sp!, {r0 - r14}		@ Save kernel state.
        ldr	r2, [r0, #4] 		@ Pull user sp out of td.
        str 	r1, [r2, #4]		@ Give the user a return value.
	ldmfd	r2!, {r0}		@ Load user CPSR.
	msr	CPSR_cxsf, r0 		@ Change to user mode.
	ldmfd	r2, {r0 - r14}		@ Restore user state.
	mov	pc, lr			@ Return to user code.
.HANDLE:
	stmfd	sp!, {r0, r4, r14}	@ Make room for scratch registers - will be able to shrink/remove this.
	mrs	r0, SPSR		@ Get users CPSR.

	mrs	r4, CPSR		@ Go to system state.
	orr	r4, r4, #0x1F
	msr     CPSR_c, r4

	stmfd	sp!, {r0 - r14}		@ Save regular state,r0,r4,r14 dirty.
        stmfd	sp!, {r0}		@ Save SPSR.
	str	sp, [sp, #-8]		@ Fix up stack pointer.

	mov	r5, sp			@ Sneak the sp into supervisor mode.

	mrs	r4, CPSR		@ Go to supervisor mode. Can shrink this to one instruction.
	bic	r4, r4, #0x1F
	orr	r4, r4, #0x13
	msr     CPSR_c, r4

	ldmfd	sp!, {r0, r4, r14}	@ Get back correct values for user's r0,r4,r14.
        str     r0, [r5, #4]		@ Store them back. Note that stack is pointing.
	str	r4, [r5, #20]		@ to saved sp, so we have to offset by 4.
        str     r14, [r5, #60]

	ldmfd	sp, {r0 - r14}		@ Restore kernel state.

	stmfd   sp!, {r4}		@ Need scratch register.

	mrs	r4, CPSR		@ Going into server mode to get user sp.
	orr	r4, r4, #0x1F
	msr     CPSR_c, r4

	str	sp, [r0, #4]		@ Update the td to have the new user stack pointer.
       
	mrs	r4, CPSR		@ Back to supervisor mode. Can shrink this to one instruction.
	bic	r4, r4, #0x1F
	orr	r4, r4, #0x13
	msr     CPSR_c, r4

	ldmfd	sp!, {r4}		@ Get back kernel's r4.

        ldr	r0, [r0, #4]		@ This brain damage is to get which system call was called.
        ldr     r0, [r0, #60]
	ldr	r0, [r0, #-4]
	and	r0, r0, #0xFF

	mov	pc, lr			@ Go to kernel.

	.size	activate, .-activate
	.text
	.align	2
	.global	install_handler
	.type	install_handler, %function
install_handler:
	stmfd	sp!, {r0, r1}
	ldr	r0, .L1		@ r0 <- address of interrupt handler
	mov	r1, #0x28	@ Install for swi
	str	r0, [r1]	@ ...
        mov	r1, #0x38	@ Install for hw interrupts
	str	r0, [r1]	@ ...
	ldmfd	sp!, {r0, r1}
	mov	pc, lr
	.align 2
.L1:
	.word	.HANDLE
	.size	install_handler, .-install_handler

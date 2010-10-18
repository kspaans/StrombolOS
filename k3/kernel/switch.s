	.text
	.align	2
	.global	swtch
	.type	swtch, %function
swtch:
	swi	#0
	mov	pc, lr
	.size	swtch, .-swtch



	.text
	.align 2
	.global print_regs
	.type	print_regs, %function
print_regs:
	str	r0, .REGSCRATCH1
	str	r1, .REGSCRATCH2
	str	lr, .LRSCRATCH
	mov	r0, #1
	ldr	r1, .REGSCRATCH1
	bl	bwputr			@ output r0
	mov	r0, #1
	ldr	r1, .REGSCRATCH2
	bl	bwputr			@ output r1
	mov	r0, #1
	mov	r1, r2
	bl	bwputr			@ output r2
	mov	r0, #1
	mov	r1, r3
	bl	bwputr			@ output r3
	mov	r0, #1
	mov	r1, r4
	bl	bwputr			@ output r4
	mov	r0, #1
	mov	r1, r5
	bl	bwputr			@ output r5
	mov	r0, #1
	mov	r1, r6
	bl	bwputr			@ output r6
	mov	r0, #1
	mov	r1, r7
	bl	bwputr			@ output r7
	mov	r0, #1
	mov	r1, r8
	bl	bwputr			@ output r8
	mov	r0, #1
	mov	r1, r9
	bl	bwputr			@ output r9
	mov	r0, #1
	mov	r1, r10
	bl	bwputr			@ output r10
	mov	r0, #1
	mov	r1, r11
	bl	bwputr			@ output r11
	mov	r0, #1
	mov	r1, r12
	bl	bwputr			@ output r12
	mov	r0, #1
	mov	r1, r13
	bl	bwputr			@ output r13
	mov	r0, #1
	ldr	r1, .LRSCRATCH
	bl	bwputr			@ output r14
	ldr	r0, .REGSCRATCH1
	ldr	r1, .REGSCRATCH2
	ldr	lr, .LRSCRATCH
	mov	pc, lr
	.align 2
.REGSCRATCH1: .word 0
.REGSCRATCH2: .word 0
.LRSCRATCH:   .word 0
	.size print_regs, .-print_regs

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
	@@ r0 is the address to store user regs
	@@ r1 is cpu state
	@@ r2 is jump back location
	@@
	stmfd	sp!, {r1 - r14}		@ Save kernel state. Don't save r0, it will be the syscall number.
	msr	SPSR_cxsf, r1 		@ Put it into the SPSR.
	str	r0, .USERREG		@ HANDLE needs to know where the user registers go.	
	mov	lr, r2			@ Move entry point through the user state restore.
	ldmfd	r0, {r0 - r14}^		@ Restore user state.
	movs	pc, lr			@ Return to user code in user mode.
.IRQ:
	str	r0, .SCRATCH		@ Save r0.
	str	lr, .SCRATCH2		@ Sneak lr_irq through
	mrs	r0, SPSR		@ Get user SPSR
	str	r0, .IRQMAGICBIT	@ Put the user's state into the SPSR - this indicates that an interrupt happened.
	mrs	r0, CPSR		@ Load the CPSR.
	orr	r0, r0, #0x1		@ Switch to supervisor mode.	
	msr	CPSR_c, r0		@ Install CPSR.
	ldr	r0, .IRQMAGICBIT	@ Retrieve the user's CPU state.
	msr	SPSR_cxsf, r0		@ SPSR_irq is now copied over to SPSR_svc. 
	ldr	lr, .SCRATCH2		@ Restore lr.
	ldr	r0, .SCRATCH		@ Restore r0. Fall through into swi handler code
.HANDLE:
	str	r0, .SCRATCH		@ r0 used as a scratch register; put argument in storage.
	ldr	r0, .USERREG		@ Get address of user regs.
	str	lr, [r0, #-4]		@ Store the re-entry point in the TD.
	stmea	r0, {r0 - r14}^		@ Save user state, r0 dirty.
	ldr r2, .SCRATCH		@ Get back r0
	str r2, [r0]			@ Undirty it.
	mrs	r1, SPSR		@ Load user CPU state into r1 (don't need to worry about it being dirty; user state already saved.)
	str	r1, [r0, #-8]		@ Save SPSR onto user stack
	ldr	r0, .IRQMAGICBIT	@ Get the IRQMagicBit - 0 if we are handling a syscall, otherwise IRQ.
	mov	r2, #0			@ Now clear the IRQMagicBit
	str	r2, .IRQMAGICBIT	@ ...
	ldmfd	sp, {r1 - r14}		@ Restore kernel state. (Think about sp that was saved -> works. && r0 not restored on purpose.)
	mov	pc, lr			@ Go to kernel.
	.align 2
.IRQMAGICBIT: .word 0
.SCRATCH:     .word 0
.USERREG:     .word 0
.SCRATCH2:    .word 0
	.size	activate, .-activate
	.text
	.align	2
	.global	install_handler
	.type	install_handler, %function
install_handler:
	stmfd	sp!, {r0, r1}
	ldr	r0, .SWIHANDLE	@ r0 <- address of swi handler
	mov	r1, #0x28	@ SWI handler address goes here.
	str	r0, [r1]	@ install.
	ldr	r0, .IRQHANDLE	@ r0 <- address of irq handler
        mov	r1, #0x38	@ IRQ handler address goes here
	str	r0, [r1]	@ install.
	ldmfd	sp!, {r0, r1}
	mov	pc, lr
	.align 2
.SWIHANDLE:   .word .HANDLE
.IRQHANDLE:   .word .IRQ
	.size	install_handler, .-install_handler

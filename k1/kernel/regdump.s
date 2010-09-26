	@ TODO: make this save all registers to the stack first, then print those.
	@       else try inline this "function"...
	.section	.rodata
	.align	2
.LC0:
	.ascii	"\011r0\011%x\012\000"
	.align	2
.LC1:
	.ascii	"\011r1\011%x\012\000"
	.align	2
.LC2:
	.ascii	"\011r2\011%x\012\000"
	.align	2
.LC3:
	.ascii	"\011r3\011%x\012\000"
	.align	2
.LC4:
	.ascii	"\011r4\011%x\012\000"
	.align	2
.LC5:
	.ascii	"\011r5\011%x\012\000"
	.align	2
.LC6:
	.ascii	"\011r6\011%x\012\000"
	.align	2
.LC7:
	.ascii	"\011r7\011%x\012\000"
	.align	2
.LC8:
	.ascii	"\011r8\011%x\012\000"
	.align	2
.LC9:
	.ascii	"\011r9\011%x\012\000"
	.align	2
.LC10:
	.ascii	"\011r10\011%x\012\000"
	.align	2
.LC11:
	.ascii	"\011r11\011%x\012\000"
	.align	2
.LC12:
	.ascii	"\011r12\011%x\012\000"
	.align	2
.LC13:
	.ascii	"\011r13\011%x\012\000"
	.align	2
.LC14:
	.ascii	"\011r14\011%x\012\000"
	.align	2
.LC15:
	.ascii	"\011r15\011%x\012\000"
	.text
	.align	2
	.global	regdump
	.type	regdump, %function
regdump:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	mov	r2, r0
	mov	r0, #1
	ldr	r1, .L3
	@mov	r2, #5		@ r0
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+4
	mov	r2, r1
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+8
	mov	r2, r2
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+12
	mov	r2, r3
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+16
	mov	r2, r4
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+20
	mov	r2, r5
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+24
	mov	r2, r6
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+28
	mov	r2, r7
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+32
	mov	r2, r8
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+36
	mov	r2, r9
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+40
	mov	r2, r10
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+44
	mov	r2, r11
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+48
	mov	r2, r12
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+52
	mov	r2, r13
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+56
	mov	r2, r14
	bl	bwprintf
	mov	r0, #1
	ldr	r1, .L3+60
	mov	r2, r15
	bl	bwprintf
	ldr	pc, [sp], #4
.L4:
	.align	2
.L3:
	.word	.LC0
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.word	.LC4
	.word	.LC5
	.word	.LC6
	.word	.LC7
	.word	.LC8
	.word	.LC9
	.word	.LC10
	.word	.LC11
	.word	.LC12
	.word	.LC13
	.word	.LC14
	.word	.LC15
	.size	regdump, .-regdump
	.ident	"GCC: (GNU) 4.0.2"

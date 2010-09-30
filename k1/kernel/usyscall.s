@@
@@ Userspace stubs to call SWI with the correct syscall numbers
@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Create
	.type	Create, %function
Create:
	swi	#0
	mov	pc, lr
	.size	Create, .-Create
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	MyTid
	.type	MyTid, %function
MyTid:
	swi	#1
	mov	pc, lr
	.size	MyTid, .-MyTid
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	MyParentTid
	.type	MyParentTid, %function
MyParentTid:
	swi	#2
	mov	pc, lr
	.size	MyParentTid, .-MyParentTid
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Pass
	.type	Pass, %function
Pass:
	swi	#3
	mov	pc, lr
	.size	Pass, .-Pass
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Exit
	.type	Exit, %function
Exit:
	swi	#4
	mov	pc, lr
	.size	Exit, .-Exit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.ident	"GCC: (GNU) 4.0.2"

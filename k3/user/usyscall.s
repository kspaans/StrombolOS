@@
@@ Userspace stubs to call SWI with the correct syscall numbers
@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Create
	.type	Create, %function
Create:
	stmfd   sp!, {lr}
	swi	#0
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	Create, .-Create
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	MyTid
	.type	MyTid, %function
MyTid:
	stmfd   sp!, {lr}
	swi	#1
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	MyTid, .-MyTid
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	MyParentTid
	.type	MyParentTid, %function
MyParentTid:
	stmfd   sp!, {lr}
	swi	#2
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	MyParentTid, .-MyParentTid
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Pass
	.type	Pass, %function
Pass:
	stmfd   sp!, {lr}
	swi	#3
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	Pass, .-Pass
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Exit
	.type	Exit, %function
Exit:
	stmfd   sp!, {lr}
	swi	#4
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	Exit, .-Exit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Send
	.type	Send, %function
Send:
        swp     r5, r5, [sp]
	stmfd   sp!, {lr}
	swi	#5
	ldmfd   sp!, {lr}
	swp     r5, r5, [sp]
	mov	pc, lr
	.size	Send, .-Send
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Receive
	.type	Receive, %function
Receive:
	stmfd   sp!, {lr}
	swi	#6
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	Receive, .-Receive
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	Reply
	.type	Reply, %function
Reply:
	stmfd   sp!, {lr}
	swi	#7
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	Reply, .-Reply
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.text
	.align	2
	.global	AwaitEvent
	.type	AwaitEvent, %function
AwaitEvent:
	stmfd   sp!, {lr}
	swi	#8
	ldmfd   sp!, {lr}
	mov	pc, lr
	.size	AwaitEvent, .-AwaitEvent
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

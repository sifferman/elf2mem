	.file	1 "example.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=32
	.module	nooddspreg
	.abicalls
	.text
	.align	2
	.globl	__start
	.set	nomips16
	.set	nomicromips
	.ent	__start
	.type	__start, @function
__start:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	lw	$3,%got(out)($28)
	move	$2,$0
	li	$4,256			# 0x100
$L2:
	sw	$2,0($3)
	addiu	$2,$2,1
	bne	$2,$4,$L2
	addiu	$3,$3,4

$L3:
	.option	pic0
	b	$L3
	nop

	.option	pic2
	.set	macro
	.set	reorder
	.end	__start
	.size	__start, .-__start

	.comm	out,1024,4
	.globl	zero_array
	.section	.bss,"aw",@nobits
	.align	2
	.type	zero_array, @object
	.size	zero_array, 1024
zero_array:
	.space	1024
	.globl	const_array
	.rdata
	.align	2
	.type	const_array, @object
	.size	const_array, 1024
const_array:
	.word	-559038737
	.space	1020
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04) 9.4.0"

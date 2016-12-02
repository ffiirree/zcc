	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR6:
	.ascii "%d\0"
	.def	__main;	.scl	2;	.type	32;	.endef
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	call	___main
	movl	$9, -12(%ebp)
	movl	$6, -16(%ebp)
	movl	-16(%ebp), %eax
	imull	$89, %eax, %eax
	movl	%eax, %ebx
	movl	-16(%ebp), %eax
	imull	$4, %eax, %eax
	addl	%eax, %ebx
	movl	$6, %eax
	subl	%eax, %ebx
	movl	%ebx, -12(%ebp)
	movl	$.LLSTR6, 0(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

.LFE8:
	.ident "zcc 0.0.1"

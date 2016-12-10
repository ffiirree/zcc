	.file	"hello.s"
	.def	__main;	.scl	2;	.type	32;	.endef
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	.cfi_startproc
	pushl  %ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	call	___main
	movl	$8, -4(%ebp)
	movl	$9, -8(%ebp)
	movl	-4(%ebp), %eax
	movl	-8(%ebp), %edx
	imull	%edx, %eax
	movl	%eax, %ebx
	movl	-4(%ebp), %eax
	cltd
	movl	$2, %ecx
	idivl   %ecx
	movl	%eax, %ecx
	movl	%ebx, %eax
	addl	%ecx, %eax
	movl	%eax, -12(%ebp)
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.ident "zcc 0.0.1"

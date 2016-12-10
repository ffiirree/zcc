	.file	"hello.s"
	.section  .rdata,"dr"
.LLSTR8:
	.ascii "%d\n\0"
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
	subl	$64, %esp
	call	___main
	movl	$6, -40(%ebp)
	movl	$5, -36(%ebp)
	movl	$4, -32(%ebp)
	movl	$2, -28(%ebp)
	leal	-40(%ebp), %eax
	movl	%eax, -44(%ebp)
	movl	-44(%ebp), %eax
	addl	$4, %eax
	movl	%eax, -44(%ebp)
	movl	-44(%ebp), %eax
	addl	$8, %eax
	movl	$89, (%eax)
	movl	-44(%ebp), %eax
	addl	$8, %eax
	movl	(%eax), %eax
	movl	%eax, -48(%ebp)
	movl	$.LLSTR8, 0(%esp)
	movl	-48(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE10:
	.ident "zcc 0.0.1"

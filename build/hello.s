	.file	"hello.s"
	.section  .rdata,"dr"
.LLSTR6:
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
	subl	$36, %esp
	call	___main
	movl	$11, -16(%ebp)
	movl	$22, -12(%ebp)
	movl	$33, -8(%ebp)
	movl	$44, -4(%ebp)
	leal	-4(%ebp), %eax
	movl	$45, (%eax)
	movl	-4(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	$.LLSTR6, 0(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.ident "zcc 0.0.1"

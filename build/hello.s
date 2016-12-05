	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR3:
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
	subl	$56, %esp
	call	___main
	movl	$8, -40(%ebp)
	movl	$9, -36(%ebp)
	movl	$0, -32(%ebp)
	leal	-24(%ebp), %eax
	movl	$9, (%eax)
	movl	-24(%ebp), %eax
	movl	$.LLSTR3, 0(%esp)
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.ident "zcc 0.0.1"

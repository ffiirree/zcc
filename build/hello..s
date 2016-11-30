	.file	hello.qd
	.globl	_glo_var
	.data
	.align	4
_glo_var:
	.long	9

	.def	__main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	type	32;	.endf
_main:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$ - 16, %esp
	subl	$16, %esp
	call	___main
	movb	$0, 0(%esp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

	.globl	_foo
	.def	_foo;	.scl	2;	type	32;	.endf
_foo:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$33, %esp
	movb	$0, -1(%ebp)
	movl	$0, -5(%ebp)
	movl	$9, -9(%ebp)
	movl	$9, -13(%ebp)
	movl	$9, -17(%ebp)
	movl	$9, -21(%ebp)
	movl	$9, -25(%ebp)
	movl	$9, -29(%ebp)
	movl	$9, -33(%ebp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc


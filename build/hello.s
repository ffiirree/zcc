	.file	"hello.qd"
	.globl	_ch
	.data
	.align	1
_ch:
	.long	3

	.globl	_glo_var
	.data
	.align	4
_glo_var:
	.long	9

	.def	__main;	.scl	2;	.type	32;	.endef
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
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
	movb	$0, -1(%ebp)
	movb	$5, -1(%ebp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc


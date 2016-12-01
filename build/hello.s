	.file	"hello.qd"
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
	movl	$0, -4(%ebp)
	movl	$0, -8(%ebp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

	.globl	_whichDayofYear
	.def	_whichDayofYear;	.scl	2;	.type	32;	.endef
_whichDayofYear:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	$0, -4(%ebp)
	movl	$31, -4(%ebp)
	movl	$59, -4(%ebp)
	movl	$90, -4(%ebp)
	movl	$120, -4(%ebp)
	movl	$151, -4(%ebp)
	movl	$181, -4(%ebp)
	movl	$212, -4(%ebp)
	movl	$243, -4(%ebp)
	movl	$273, -4(%ebp)
	movl	$304, -4(%ebp)
	movl	$334, -4(%ebp)
	movl	$1, -8(%ebp)
	movl	$0, -8(%ebp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc


	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR2:
	.ascii "Hello World\n\0"
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
	andl	$ - 16, %esp
	call	___main
	movl	$.LLSTR2, 0(%esp)
	call	_pr
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

	.text
	.globl	_pr
	.def	_pr;	.scl	2;	.type	32;	.endef
_pr:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

.LFE5:
	.ident "zcc 0.0.1"

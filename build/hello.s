	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR4:
	.ascii "%d\n\0"
	.globl	_c
	.data
_c:
	.long	39
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
	movl	_c, %eax
	movl	%eax, -4(%ebp)
	movl	$.LLSTR4, 0(%esp)
	movl	_c, %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.ident "zcc 0.0.1"

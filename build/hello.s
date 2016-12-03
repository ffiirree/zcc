	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR3:
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
	subl	$16, %esp
	call	___main
	cltd
	movl	$.LLSTR3, 0(%esp)
	call	_printf
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

.LFE5:
	.ident "zcc 0.0.1"

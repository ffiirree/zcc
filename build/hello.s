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
	subl	$20, %esp
	call	___main
	movl	$7, -4(%ebp)
	movl	$5, -8(%ebp)
	movl	-4(%ebp), %eax
	cltd
	idivl	-8(%ebp)
	movl	%edx, -12(%ebp)
	movl	$.LLSTR3, 0(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

.LFE5:
	.ident "zcc 0.0.1"

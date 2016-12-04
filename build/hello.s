	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR4:
	.ascii "%d\n\0"
	.globl	_b
	.data
_b:
	.byte	8
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
	subl	$24, %esp
	call	___main
	movl	$_b, %eax
	movl	%eax, -4(%ebp)
	movl	%eax, %ebx
	movl	-4(%ebp), %eax
	movl	0(%eax), %eax
	movl	%eax, -8(%ebp)
	movl	$.LLSTR4, 0(%esp)
	movsbl	_b, %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.ident "zcc 0.0.1"

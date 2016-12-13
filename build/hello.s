	.file	"hello.s"
	.def	___main;	.scl	2;	.type	32;	.endef
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
	movl	$250, 0(%esp)
	movl	$45, 4(%esp)
	call	_max
	movl	%eax, -4(%ebp)
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
	.text
	.globl	_max
	.def	_max;	.scl	2;	.type	32;	.endef
_max:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	12(%ebp), %eax
	movl	8(%ebp), %ebx
	cmpl	%eax, %ebx
	jg	.Lift8
	jmp	.Liff9
.Lift8:
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	jmp	.Lsn7
.Liff9:
	movl	12(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
.Lsn7:
	.cfi_endproc
.LFE10:
	.ident "zcc 0.0.1"

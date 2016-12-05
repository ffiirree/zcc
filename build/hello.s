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
	subl	$36, %esp
	call	___main
	movl	$6, -4(%ebp)
	movl	$0, -8(%ebp)
	movl	-4(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_max@ii
	movl	%eax, -12(%ebp)
	movl	$.LLSTR3, 0(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
	.text
	.globl	_max@ii
	.def	_max@ii;	.scl	2;	.type	32;	.endef
_max@ii:
	.cfi_startproc
	pushl  %ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	12(%ebp), %eax
	movl	8(%ebp), %ebx
	cmpl	%eax, %ebx
	jg	.Lift7
	jmp	.Liff8
.Lift7:
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	jmp	.Lsn6
.Liff8:
	movl	12(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
.Lsn6:
	.cfi_endproc
	.text
	.globl	_max@cs
	.def	_max@cs;	.scl	2;	.type	32;	.endef
_max@cs:
	.cfi_startproc
	pushl  %ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movswl	9(%ebp), %eax
	movsbl	8(%ebp), %ebx
	cmpl	%eax, %ebx
	jg	.Lift11
	jmp	.Liff12
.Lift11:
	movsbl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	jmp	.Lsn10
.Liff12:
	movsbl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
.Lsn10:
	.cfi_endproc
.LFE14:
	.ident "zcc 0.0.1"

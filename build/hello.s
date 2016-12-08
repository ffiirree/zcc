	.file	"hello.s"
	.section  .rdata,"dr"
.LLSTR4:
	.ascii "hello\0"
	.globl	_line
	.data
_line:
	.long	7
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
	movl	$8, -4(%ebp)
	movl	$9, -8(%ebp)
	movl	-4(%ebp), %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	jg	.Lift6
	jmp	.Liff7
.Lift6:
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	jmp	.Lsn5
.Liff7:
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
.Lsn5:
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.ident "zcc 0.0.1"

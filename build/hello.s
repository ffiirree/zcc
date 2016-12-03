	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR17:
	.ascii "\n\0"
.LLSTR15:
	.ascii " \0"
.LLSTR14:
	.ascii "*\0"
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
	movl	$0, -4(%ebp)
.Lforb3:
	movl	$10, %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	jl	.Lfort5
	jmp	.Lforn2
.Lfe34:
	addl	$1, -4(%ebp)
	jmp	.Lforb3
.Lfort5:
	movl	$0, -8(%ebp)
.Lforb7:
	movl	$15, %eax
	movl	-8(%ebp), %ebx
	cmpl	%eax, %ebx
	jl	.Lfort9
	jmp	.Lforn6
.Lfe38:
	addl	$1, -8(%ebp)
	jmp	.Lforb7
.Lfort9:
	movl	$10, %eax
	movl	-8(%ebp), %ebx
	cmpl	%eax, %ebx
	jl	.Landt13
	jmp	.Liff12
.Landt13:
	movl	-8(%ebp), %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	je	.Lift11
	jmp	.Liff12
.Lift11:
	movl	$.LLSTR14, 0(%esp)
	call	_printf
	jmp	.Lsn10
.Liff12:
	movl	$.LLSTR15, 0(%esp)
	call	_printf
.Lsn10:
	jmp	.Lfe38
.Lforn6:
	movl	$.LLSTR17, 0(%esp)
	call	_printf
	jmp	.Lfe34
.Lforn2:
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

.LFE20:
	.ident "zcc 0.0.1"

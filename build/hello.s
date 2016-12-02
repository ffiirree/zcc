	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR19:
	.ascii "\n\0"
.LLSTR16:
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
	subl	$24, %esp
	call	___main
	movl	$0, -12(%ebp)
.Lforb3:
	movl	$10, %eax
	movl	-12(%ebp), %ebx
	cmpl	%eax, %ebx
	jl	.Lfort5
	jmp	.Lforn2
.Lfe34:
	movl	$1, %eax
	movl	-12(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -12(%ebp)
	jmp	.Lforb3
.Lfort5:
	movl	$0, -16(%ebp)
.Lforb8:
	movl	$15, %eax
	movl	-16(%ebp), %ecx
	cmpl	%eax, %ecx
	jl	.Lfort10
	jmp	.Lforn7
.Lfe39:
	movl	$1, %eax
	movl	-16(%ebp), %ecx
	addl	%eax, %ecx
	movl	%ecx, -16(%ebp)
	jmp	.Lforb8
.Lfort10:
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	cmpl	%eax, %edx
	jl	.Lift13
	jmp	.Lorf15
.Lorf15:
	movl	$10, %eax
	movl	-16(%ebp), %edx
	cmpl	%eax, %edx
	jg	.Lift13
	jmp	.Liff14
.Lift13:
	movl	$.LLSTR16, 0(%esp)
	call	_printf
.Liff14:
	jmp	.Lfe39
.Lforn7:
	movl	$.LLSTR19, 0(%esp)
	call	_printf
	jmp	.Lfe34
.Lforn2:
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc

.LFE22:
	.ident "zcc 0.0.1"

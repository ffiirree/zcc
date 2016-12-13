	.file	"hello.s"
	.section  .rdata,"dr"
.LLSTR80:
	.ascii "\n\0"
.LLSTR78:
	.ascii "\n\0"
.LLSTR73:
	.ascii "%6d\0"
.LLSTR65:
	.ascii "   \0"
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
	subl	$1512, %esp
	call	___main
	movl	$18, -16(%ebp)
	movl	$1, -4(%ebp)
.Lforb8:
	movl	-16(%ebp), %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lfort10
	jmp	.Lforn7
.Lfe39:
	movl	$1, %eax
	movl	-4(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -4(%ebp)
	jmp	.Lforb8
.Lfort10:
	movl	-4(%ebp), %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	-4(%ebp), %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	$1, (%eax)
	movl	-4(%ebp), %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	-4(%ebp), %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	(%eax), %eax
	movl	%eax, -1464(%ebp)
	movl	-4(%ebp), %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	$1, %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	-1464(%ebp), %ebx
	movl	%ebx, (%eax)
	jmp	.Lfe39
.Lforn7:
	movl	$3, -4(%ebp)
.Lforb24:
	movl	-16(%ebp), %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lfort26
	jmp	.Lforn23
.Lfe325:
	movl	$1, %eax
	movl	-4(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -4(%ebp)
	jmp	.Lforb24
.Lfort26:
	movl	$2, -8(%ebp)
.Lforb31:
	movl	$1, %eax
	movl	-4(%ebp), %ebx
	subl	%eax, %ebx
	movl	%ebx, %eax
	movl	-8(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lfort33
	jmp	.Lforn30
.Lfe332:
	movl	$1, %eax
	movl	-8(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -8(%ebp)
	jmp	.Lforb31
.Lfort33:
	movl	$1, %eax
	movl	-4(%ebp), %ebx
	subl	%eax, %ebx
	movl	%ebx, %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	-8(%ebp), %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	(%eax), %eax
	movl	%eax, -1468(%ebp)
	movl	$1, %eax
	movl	-4(%ebp), %ebx
	subl	%eax, %ebx
	movl	%ebx, %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	$1, %ebx
	movl	-8(%ebp), %ecx
	subl	%ebx, %ecx
	movl	%ecx, %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	(%eax), %eax
	movl	%eax, -1472(%ebp)
	movl	-4(%ebp), %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	-8(%ebp), %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	-1472(%ebp), %ebx
	movl	-1468(%ebp), %edx
	addl	%ebx, %edx
	movl	%edx, (%eax)
	jmp	.Lfe332
.Lforn30:
	jmp	.Lfe325
.Lforn23:
	movl	$1, -4(%ebp)
.Lforb52:
	movl	-16(%ebp), %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lfort54
	jmp	.Lforn51
.Lfe353:
	movl	$1, %eax
	movl	-4(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -4(%ebp)
	jmp	.Lforb52
.Lfort54:
	movl	$1, -12(%ebp)
.Lforb59:
	movl	-4(%ebp), %eax
	movl	-16(%ebp), %ebx
	subl	%eax, %ebx
	movl	%ebx, %eax
	movl	-12(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lfort61
	jmp	.Lforn58
.Lfe360:
	movl	$1, %eax
	movl	-12(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -12(%ebp)
	jmp	.Lforb59
.Lfort61:
	movl	$.LLSTR65, 0(%esp)
	movl	$1, 4(%esp)
	call	_printf
	jmp	.Lfe360
.Lforn58:
	movl	$1, -8(%ebp)
.Lforb68:
	movl	-4(%ebp), %eax
	movl	-8(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lfort70
	jmp	.Lforn67
.Lfe369:
	movl	$1, %eax
	movl	-8(%ebp), %ebx
	addl	%eax, %ebx
	movl	%ebx, -8(%ebp)
	jmp	.Lforb68
.Lfort70:
	movl	-4(%ebp), %eax
	movl	$19, %edx
	imull	%edx, %eax
	movl	-8(%ebp), %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	leal	-1460(%ebp), %eax
	imull	$4, %ecx
	addl	%ecx, %eax
	movl	(%eax), %eax
	movl	$.LLSTR73, 0(%esp)
	movl	%eax, 4(%esp)
	call	_printf
	jmp	.Lfe369
.Lforn67:
	movl	$.LLSTR78, 0(%esp)
	movl	$1, 4(%esp)
	call	_printf
	jmp	.Lfe353
.Lforn51:
	movl	$.LLSTR80, 0(%esp)
	movl	$1, 4(%esp)
	call	_printf
	.cfi_endproc
.LFE82:
	.ident "zcc 0.0.1"

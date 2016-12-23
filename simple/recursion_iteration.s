	.file	"D:/zcc/simple/recursion_iteration.s"
	.section  .rdata,"dr"
.LLSTR8:
	.ascii "The factorial(iteration) of %ld is %ld!\n\0"
.LLSTR6:
	.ascii "The factorial(recursion) of %ld is %ld!\n\0"
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
	subl	$52, %esp
	call	___main
	movl	$10, -4(%ebp)
	movl	-4(%ebp), %eax
	movl	%eax, 0(%esp)
	call	_factorial_recursion
	movl	%eax, -8(%ebp)
	movl	-4(%ebp), %eax
	movl	%eax, 0(%esp)
	call	_factorial_iteration
	movl	%eax, -12(%ebp)
	movl	$.LLSTR6, 0(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 8(%esp)
	call	_printf
	movl	$.LLSTR8, 0(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
	.text
	.globl	_factorial_recursion
	.def	_factorial_recursion;	.scl	2;	.type	32;	.endef
_factorial_recursion:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$0, %eax
	movl	8(%ebp), %ebx
	cmpl	%eax, %ebx
	jle	.Lift13
	jmp	.Liff14
.Lift13:
	movl	$1, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	jmp	.Lsn12
.Liff14:
	movl	$1, %eax
	movl	8(%ebp), %ebx
	subl	%eax, %ebx
	movl	%ebx, 0(%esp)
	call	_factorial_recursion
	movl	%eax, %ecx
	movl	8(%ebp), %eax
	movl	%ecx, %edx
	imull	%edx, %eax
	movl	%eax, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
.Lsn12:
	.cfi_endproc
	.text
	.globl	_factorial_iteration
	.def	_factorial_iteration;	.scl	2;	.type	32;	.endef
_factorial_iteration:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$1, -4(%ebp)
.Lwb20:
	movl	$1, %eax
	movl	8(%ebp), %ebx
	cmpl	%eax, %ebx
	jg	.Lwt22
	jmp	.Lsn21
.Lwt22:
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	imull	%edx, %eax
	movl	%eax, -4(%ebp)
	movl	$1, %eax
	movl	8(%ebp), %ebx
	subl	%eax, %ebx
	movl	%ebx, 8(%ebp)
	jmp	.Lwb20
.Lsn21:
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE26:
	.ident "zcc 0.0.1"

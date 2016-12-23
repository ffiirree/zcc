	.file	"D:/zcc/simple/test_enum.s"
	.section  .rdata,"dr"
.LLSTR11:
	.ascii "%d\n\0"
.LLSTR8:
	.ascii "%d\n\0"
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
	subl	$68, %esp
	call	___main
	movl	$0, -40(%ebp)
	movl	$9, -36(%ebp)
	movl	$8, -32(%ebp)
	leal	-28(%ebp), %eax
	movl	$11, (%eax)
	movl	$5, -44(%ebp)
	movl	$5, %eax
	movl	$10, %edx
	imull	%edx, %eax
	movl	$11, %ebx
	movl	%eax, %ecx
	addl	%ebx, %ecx
	movl	%ecx, -44(%ebp)
	movl	-28(%ebp), %eax
	movl	$.LLSTR8, 0(%esp)
	movl	%eax, 4(%esp)
	call	_printf
	movl	$.LLSTR11, 0(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE13:
	.ident "zcc 0.0.1"

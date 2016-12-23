	.file	"D:/zcc/simple/test_struct.s"
	.section  .rdata,"dr"
.LLSTR8:
	.ascii "%d\n\0"
.LLSTR5:
	.ascii "%d\n\0"
	.globl	_t
	.data
_t:
	.long	1
	.long	3
	.byte	1
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
	subl	$28, %esp
	call	___main
	movl	_t+4, %eax
	movl	%eax, -4(%ebp)
	movl	$.LLSTR5, 0(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, 4(%esp)
	call	_printf
	movl	$_t+4, %eax
	movl	$7, (%eax)
	movl	_t+4, %eax
	movl	$.LLSTR8, 0(%esp)
	movl	%eax, 4(%esp)
	call	_printf
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE11:
	.ident "zcc 0.0.1"

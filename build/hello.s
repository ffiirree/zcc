	.file	"hello.s"
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
	subl	$16, %esp
	call	___main
	movl	-8(%ebp), %eax
	movl	-4(%ebp), %ebx
	cmpl	%eax, %ebx
	je	.Lift5
	jmp	.Liff6
.Lift5:
	movl	$0, -4(%ebp)
.Liff6:
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.ident "zcc 0.0.1"

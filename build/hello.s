	.file	"hello.qd"
	.section  .rdata,"dr"
.LLSTR44:
	.ascii "\n\0"
.LLSTR41:
	.ascii " \0"
.LLSTR39:
	.ascii "*\0"
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
	subl	$32, %esp
	call	___main
	flds	.Lf6
	fstps	-4(%ebp)
.Lforb3:
	flds	.Lf8
	flds	-4(%ebp)
	fucompp
	fnstsw	%ax
	sahf
	ja	.Lfort5
	jmp	.Lforn2
.Lfe34:
	flds	-4(%ebp)
	flds	.Lf9
	fsubr	%st(1), %st(0)
	fstps	-4(%ebp)
	jmp	.Lforb3
.Lfort5:
	flds	.Lf16
	fstps	-8(%ebp)
.Lforb13:
	flds	.Lf18
	flds	-8(%ebp)
	fucompp
	fnstsw	%ax
	sahf
	jb	.Lfort15
	jmp	.Lforn12
.Lfe314:
	flds	.Lf19
	flds	-8(%ebp)
	faddp
	fstps	-8(%ebp)
	jmp	.Lforb13
.Lfort15:
	flds	-8(%ebp)
	flds	-8(%ebp)
	fmulp
	flds	-4(%ebp)
	flds	-4(%ebp)
	fmulp
	faddp
	flds	.Lf25
	fsubr	%st(1), %st(0)
	fstps	-12(%ebp)
	finit
	flds	-12(%ebp)
	flds	-12(%ebp)
	fmulp
	flds	-12(%ebp)
	fmulp
	flds	-8(%ebp)
	flds	-8(%ebp)
	fmulp
	flds	-4(%ebp)
	fmulp
	flds	-4(%ebp)
	fmulp
	flds	-4(%ebp)
	fmulp
	fsubr	%st(1), %st(0)
	flds	.Lf36
	fxch	%st(1)
	fucompp
	fnstsw	%ax
	sahf
	jbe	.Lift37
	jmp	.Liff38
.Lift37:
	movl	$.LLSTR39, 0(%esp)
	call	_printf
	jmp	.Lsn28
.Liff38:
	movl	$.LLSTR41, 0(%esp)
	call	_printf
.Lsn28:
	finit
	jmp	.Lfe314
.Lforn12:
	movl	$.LLSTR44, 0(%esp)
	call	_printf
	jmp	.Lfe34
.Lforn2:
	.cfi_endproc
.LFE48:
	.section .rdata,"dr"
.Lf36:
	.float 0.0
.Lf25:
	.float 1
.Lf19:
	.float 0.05
.Lf18:
	.float 1.5
.Lf16:
	.float -1.5
.Lf9:
	.float 0.1
.Lf8:
	.float -1.5
.Lf6:
	.float 1.5
	.ident "zcc 0.0.1"

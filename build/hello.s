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
	subl	$23, %esp
	call	___main
	flds	.Lf9
	fstps	-4(%ebp)
.Lforb5:
	flds	.Lf11
	flds	-4(%ebp)
	fucompp
	fnstsw	%ax
	sahf
	ja	.Lfort7
	jmp	.Lforn4
.Lfe36:
	flds	-4(%ebp)
	flds	.Lf12
	fsubr	%st(1), %st(0)
	fstps	-4(%ebp)
	jmp	.Lforb5
.Lfort7:
	flds	.Lf21
	fstps	-8(%ebp)
.Lforb17:
	flds	.Lf23
	flds	-8(%ebp)
	fucompp
	fnstsw	%ax
	sahf
	jb	.Lfort19
	jmp	.Lforn16
.Lfe318:
	flds	.Lf24
	flds	-8(%ebp)
	faddp
	fstps	-8(%ebp)
	jmp	.Lforb17
.Lfort19:
	flds	-8(%ebp)
	flds	-8(%ebp)
	fmulp
	flds	-4(%ebp)
	flds	-4(%ebp)
	fmulp
	faddp
	flds	.Lf31
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
	flds	.Lf41
	fxch	%st(1)
	fucompp
	fnstsw	%ax
	sahf
	jbe	.Ltot43
	jmp	.Ltof44
.Ltot43:
	movb	$42, 0(%esp)
	call	_putchar
	jmp	.Lsn42
.Ltof44:
	movb	$32, 0(%esp)
	call	_putchar
.Lsn42:
	finit
	jmp	.Lfe318
.Lforn16:
	movb	$10, 0(%esp)
	call	_putchar
	jmp	.Lfe36
.Lforn4:
	.cfi_endproc
.LFE49:
	.section .rdata,"dr"
.Lf41:
	.float 0.0
.Lf31:
	.float 1
.Lf24:
	.float 0.05
.Lf23:
	.float 1.5
.Lf21:
	.float -1.5
.Lf12:
	.float 0.1
.Lf11:
	.float -1.5
.Lf9:
	.float 1.5
	.ident "zcc 0.0.1"

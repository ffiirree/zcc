.inscope	.Lfun2
main:
.inscope	.LEnv3
[]=	0	a	0
[]=	4	a	9
[]=	8	a	8
clr
[]&	3	a	.Lvar5
=	THREE	.Lvar5
clr
=	5	b
clr
*	TWO	5	.Lvar6
+	THREE	.Lvar6	.Lvar7
=	.Lvar7	b
clr
[]	3	a	.Lvar9

param .Lvar9
param .LLSTR8
call	printf	2	.Lret10
clr

param b
param .LLSTR11
call	printf	2	.Lret12
clr
ret	0
clr
.outscope
.outscope
.end

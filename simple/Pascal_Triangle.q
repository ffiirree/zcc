.inscope	.Lfun2
main:
.inscope	.LEnv4
=	18	n
clr
.inscope	.Lfor11
=	1	i
.Lforb8:
if	i <= n	goto .Lfort10
goto .Lforn7
clr
.Lfe39:
+              1              i              .Linc12        
=              .Linc12        i              
clr
goto .Lforb8
.Lfort10:
.inscope	.LEnv13
*	19	i	.Lvar14
+	i	.Lvar14	.Lvar15
[]&	.Lvar15	a	.Lvar16
=	1	.Lvar16
clr
*	19	i	.Lvar17
+	i	.Lvar17	.Lvar18
[]	.Lvar18	a	.Lvar19
=	.Lvar19	t1
clr
*	19	i	.Lvar20
+	1	.Lvar20	.Lvar21
[]&	.Lvar21	a	.Lvar22
=	t1	.Lvar22
clr
.outscope
.outscope
goto .Lfe39
.Lforn7:
clr
.inscope	.Lfor27
=	3	i
.Lforb24:
if	i <= n	goto .Lfort26
goto .Lforn23
clr
.Lfe325:
+              1              i              .Linc28        
=              .Linc28        i              
clr
goto .Lforb24
.Lfort26:
.inscope	.LEnv29
.inscope	.Lfor34
=	2	j
.Lforb31:
-	1	i	.Lvar35
if	j <= .Lvar35	goto .Lfort33
goto .Lforn30
clr
.Lfe332:
+              1              j              .Linc36        
=              .Linc36        j              
clr
goto .Lforb31
.Lfort33:
.inscope	.LEnv37
-	1	i	.Lvar38
*	19	.Lvar38	.Lvar39
+	j	.Lvar39	.Lvar40
[]	.Lvar40	a	.Lvar41
=	.Lvar41	t1
clr
-	1	i	.Lvar42
*	19	.Lvar42	.Lvar43
-	1	j	.Lvar44
+	.Lvar44	.Lvar43	.Lvar45
[]	.Lvar45	a	.Lvar46
=	.Lvar46	t2
clr
*	19	i	.Lvar47
+	j	.Lvar47	.Lvar48
[]&	.Lvar48	a	.Lvar49
+	t2	t1	.Lvar50
=	.Lvar50	.Lvar49
clr
.outscope
.outscope
goto .Lfe332
.Lforn30:
clr
.outscope
.outscope
goto .Lfe325
.Lforn23:
clr
.inscope	.Lfor55
=	1	i
.Lforb52:
if	i <= n	goto .Lfort54
goto .Lforn51
clr
.Lfe353:
+              1              i              .Linc56        
=              .Linc56        i              
clr
goto .Lforb52
.Lfort54:
.inscope	.LEnv57
.inscope	.Lfor62
=	1	k
.Lforb59:
-	i	n	.Lvar63
if	k <= .Lvar63	goto .Lfort61
goto .Lforn58
clr
.Lfe360:
+              1              k              .Linc64        
=              .Linc64        k              
clr
goto .Lforb59
.Lfort61:

param 1
param .LLSTR65
call	printf	2	.Lret66
.outscope
goto .Lfe360
.Lforn58:
clr
.inscope	.Lfor71
=	1	j
.Lforb68:
if	j <= i	goto .Lfort70
goto .Lforn67
clr
.Lfe369:
+              1              j              .Linc72        
=              .Linc72        j              
clr
goto .Lforb68
.Lfort70:
*	19	i	.Lvar74
+	j	.Lvar74	.Lvar75
[]	.Lvar75	a	.Lvar76

param .Lvar76
param .LLSTR73
call	printf	2	.Lret77
.outscope
goto .Lfe369
.Lforn67:
clr

param 1
param .LLSTR78
call	printf	2	.Lret79
clr
.outscope
.outscope
goto .Lfe353
.Lforn51:
clr

param 1
param .LLSTR80
call	printf	2	.Lret81
clr
ret	0
clr
.outscope
.outscope
.end

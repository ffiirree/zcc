.inscope	.Lfun2
main:
.inscope	.LEnv3
.inscope	.Lfor8
=f	.Lf9	y	.Lvar10
.Lforb5:
if	y > .Lf11	goto .Lfort7
goto .Lforn4
clr
.Lfe36:
-f	.Lf12	y	.Lvar13
=f	.Lvar13	y	.Lvar14
clr
goto .Lforb5
.Lfort7:
.inscope	.LEnv15
.inscope	.Lfor20
=f	.Lf21	x	.Lvar22
.Lforb17:
if	x < .Lf23	goto .Lfort19
goto .Lforn16
clr
.Lfe318:
+f	.Lf24	x	.Lvar25
=f	.Lvar25	x	.Lvar26
clr
goto .Lforb17
.Lfort19:
.inscope	.LEnv27
*f	x	x	.Lvar28
*f	y	y	.Lvar29
+f	.Lvar29	.Lvar28	.Lvar30
-f	.Lf31	.Lvar30	.Lvar32
=f	.Lvar32	a	.Lvar33
clr
*f	a	a	.Lvar34
*f	a	.Lvar34	.Lvar35
*f	x	x	.Lvar36
*f	y	.Lvar36	.Lvar37
*f	y	.Lvar37	.Lvar38
*f	y	.Lvar38	.Lvar39
-f	.Lvar39	.Lvar35	.Lvar40
if	.Lvar40 <= .Lf41	goto .Ltot43
goto .Ltof44
.Ltot43:

param 42
call	putchar	1	.Lret45
goto	.Lsn42
.Ltof44:

param 32
call	putchar	1	.Lret47
.Lsn42:
clr
.outscope
.outscope
goto .Lfe318
.Lforn16:
clr

param 10
call	putchar	1	.Lret48
clr
.outscope
.outscope
goto .Lfe36
.Lforn4:
clr
.outscope
.outscope
.end

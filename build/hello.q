.inscope	.Lfun2
main:
.inscope	.LEnv3

param 45
param 250
call	max	2	.Lret4
=	.Lret4	a
clr
ret	a
clr
.outscope
.outscope
.end
.inscope	.Lfun5
max:
.inscope	.LEnv6
if	a > b	goto .Lift8
goto .Liff9
.Lift8:
clr
ret	a
goto	.Lsn7
.Liff9:
ret	b
.Lsn7:
clr
.outscope
.outscope
.end

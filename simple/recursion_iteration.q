.inscope	.Lfun2
main:
.inscope	.LEnv3
=	10	N
clr

param N
call	factorial_recursion	1	.Lret4
=	.Lret4	recursion_result
clr

param N
call	factorial_iteration	1	.Lret5
=	.Lret5	iteration_result
clr

param recursion_result
param N
param .LLSTR6
call	printf	3	.Lret7
clr

param iteration_result
param N
param .LLSTR8
call	printf	3	.Lret9
clr
ret	0
clr
.outscope
.outscope
.end
.inscope	.Lfun10
factorial_recursion:
.inscope	.LEnv11
if	n <= 0	goto .Lift13
goto .Liff14
.Lift13:
clr
ret	1
goto	.Lsn12
.Liff14:
-	1	n	.Lvar15

param .Lvar15
call	factorial_recursion	1	.Lret16
*	.Lret16	n	.Lvar17
ret	.Lvar17
.Lsn12:
clr
.outscope
.outscope
.end
.inscope	.Lfun18
factorial_iteration:
.inscope	.LEnv19
=	1	r
clr
.Lwb20:
if	n > 1	goto .Lwt22
goto .Lsn21
.Lwt22:
clr
.inscope	.LEnv23
*	n	r	.Lvar24
=	.Lvar24	r
clr
-              1              n              .Ldec25        
=              .Ldec25        n              
clr
.outscope
goto .Lwb20
.Lsn21:
clr
ret	r
clr
.outscope
.outscope
.end

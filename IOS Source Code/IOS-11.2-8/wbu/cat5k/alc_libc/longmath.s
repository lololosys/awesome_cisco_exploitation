| $Id: longmath.s,v 1.1.4.1 1996/05/09 14:55:54 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/longmath.s,v $
|------------------------------------------------------------------
| longmath.s -- Assembler long math library (from Catalyst 1200).
|
| September 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| /*
| $Log: longmath.s,v $
| Revision 1.1.4.1  1996/05/09  14:55:54  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:25  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:25  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:33  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  00:30:36  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:53:45  rlowe
| Placeholders for California.
|
|
|------------------------------------------------------------------
| $Endlog$
| */
|

.even
.text
|---------------------------------------
|	___mulsi3  	Long multiply
|	___umulsi3 	Unsigned Long multiply
|
| calling convention a * b a -> sp(4) b -> sp(8)

	.data
	.text
	.globl	___mulsi3
	.globl  ___umulsi3
___mulsi3:
___umulsi3:
	link	a6,#0
	movl	d2,a0  			| save d2 and d3 in scratch registers
	movl	d3,a1  
	movl	a6@(8),d0		| move a into d0
	movw	d0,d2
	movw	d0,d1
	extl	d1
	swap	d1
	swap	d0
	subw	d0,d1
	movw	a6@(14),d0
	movw	d0,d3
	extl	d3
	swap	d3
	subw	a6@(12),d3
	muls	d0,d1
	muls	d2,d3
	addw	d1,d3
	muls	d2,d0
	swap	d0
	subw	d3,d0
	swap	d0
	movl	a1  ,d3
	movl	a0  ,d2
	unlk	a6
	rts

|---------------------------------------
|
| ___divsi3 -> long signed devide called by gcc
|			   calls ldiv after setup.
|
|	entered with a -> sp(4) b-> sp(8)

	.data
	.text
	.globl	___divsi3
___divsi3:
		movl	sp@(4),d0
		movl	d2,a0		| save d2 in a0
		movl	sp@(8),d2	| get divisor in d2
		bpl		L$ldiv1
		negl	d2			| take absolute value
L$ldiv1:
		movl	d0,d1		| get dividend in d1
		bpl		L$ldiv2
		negl	d1			| take absolute value
L$ldiv2:
		eorl	d0,sp@(8)	| blast divisor with signs-different flag
		bsr		L$uldiv1	| do unsigned division
		tstb	sp@(8)
		bpl		L$ldiv3
		negl	d0			| negative quotient if signs different
L$ldiv3:
		rts

|---------------------------------------
| ___modsi3  - long signed remainder
|

	.data
	.text
	.globl	___modsi3		| remainder is negative if dividend is
___modsi3:	
		movl	sp@(4),d0
		movl	d2,a0		| save d2 in a0
		movl	sp@(8),d2	| get divisor in d2
		bpl		L$lrem1
		negl	d2			| take absolute value
L$lrem1:
		movl	d0,d1		| get dividend in d1
		bpl		L$lrem2
		negl	d1			| take absolute value
L$lrem2:
		movl	d0,sp@(8)	| blast divisor with dividend
		bsr		L$uldiv1	| do unsigned division
							| note: d2 is restored in __uldiv1
		movl	d1,d0		| result is remainder
		tstb	sp@(8)
		bpl		L$lrem3
		negl	d0			| negative dividend means negative remainder
L$lrem3:
		rts

|---------------------------------------
| ___udivsi3 - long unsigned division
|

	.data
	.text
	.globl	___udivsi3
___udivsi3:
		movl	sp@(4),d0
		movl	d2,a0		| save d2 in a0
		movl	sp@(8),d2	| get divisor in d2
L$uldiv0:
		movl	d0,d1		| get dividend in d1
L$uldiv1:
		cmpl	#65535,d2	| check if software divide required
		bhi		L$uldiv3
		movl	#0,d0
|
| see if instruction will work (divu quits quickly if it can't do it)
|

		divu	d2,d1		| NOTE: we trap here if divide by zero
		bvc		L$good		| if no overflow, we're ok
|
| it's necessary to do it in parts
|

		swap	d1			| can use hardware divide
		movw	d1,d0
		divu	d2,d0
		swap	d0
		movw	d0,d1
		swap	d1
L$uldiv2:
		divu	d2,d1
L$good:
		movw	d1,d0		| d0 = unsigned quotient
		clrw	d1
		swap	d1			| d1 = unsigned remainder
		movl	a0,d2		| restore d2
		rts

| The divisor is known to be >= 2^16 so only 16 cycles are needed.

L$uldiv3:
		movl	d1,d0
		clrw	d1
		swap	d1
		swap	d0
		clrw	d0

| d1 now             0,,hi(dividend)
| d0 now  lo(dividend),,0
|              this zero ^ shifts left 1 bit per cycle,
|              becoming top half of quotient



		movl	d3,a1		| save d3 in a1 across loop
		movw	#16-1,d3	| dbra counts down to -1
L$uldiv4:
		addl	d0,d0		| add is 2 cycles faster than shift or rotate
		addxl	d1,d1
		cmpl	d2,d1
		bls		L$uldiv5
		subl	d2,d1
		addw	#1,d0		| bottom bit changes from 0 to 1 (no carry)
L$uldiv5:
		dbra	d3,L$uldiv4
		movl	a1,d3		| restore d3
		movl	a0,d2		| restore d2
		rts

|---------------------------------------
| ___umodsi3 - long unsigned remainder

	.data
	.text
	.globl	___umodsi3
___umodsi3:
		movl	sp@(4),d0
		movl	d2,a0		| save d2 in a0
		movl	sp@(8),d2	| get digisor in d2
		bsr		L$uldiv0	| do unsigned division
		movl	d1,d0		| result is remainder
		rts

	.data
	.text
	.globl	___lshrsi3
___lshrsi3:
	link	a6,#0
	movl	a6@(8),d0
	movw	a6@(14),d1
	lsrl	d1,d0
	unlk	a6
	rts

	.data
	.text
	.globl	___lshlsi3
___lshlsi3:
	link	a6,#0
	movl	a6@(8),d0
	movw	a6@(14),d1
	lsll	d1,d0
	unlk	a6
	rts

	.data
	.text
	.globl	___ashrsi3
___ashrsi3:
	link	a6,#0
	movl	a6@(8),d0
	movw	a6@(14),d1
	asrl	d1,d0
	unlk	a6
	rts

	.data
	.text
	.globl	___ashlsi3
___ashlsi3:
	link	a6,#0
	movl	a6@(8),d0
	movw	a6@(14),d1
	asll	d1,d0
	unlk	a6
	rts

	.data
	.text
	.globl	___cmpdi2
___cmpdi2:
	link	a6,#0
	movl	a6@(12),d0
	cmpl	a6@(20),d0
	jlt	LY00001
	cmpl	a6@(20),d0
	jgt	LY00000
	movl	a6@(8),d0
	cmpl	a6@(16),d0
	jcc	L46
LY00001:
	moveq	#0,d0
	jra	LE42
L46:
	movl	a6@(8),d0
	cmpl	a6@(16),d0
	jls	L47
LY00000:
	moveq	#2,d0
	jra	LE42
L47:
	moveq	#1,d0
LE42:
	unlk	a6
	rts

	.data
	.text
	.globl	___ucmpdi2
___ucmpdi2:
	link	a6,#0
	movl	a6@(12),d0
	cmpl	a6@(20),d0
	jcs	LY00003
	cmpl	a6@(20),d0
	jhi	LY00002
	movl	a6@(8),d0
	cmpl	a6@(16),d0
	jcc	L53
LY00003:
	moveq	#0,d0
	jra	LE49
L53:
	movl	a6@(8),d0
	cmpl	a6@(16),d0
	jls	L54
LY00002:
	moveq	#2,d0
	jra	LE49
L54:
	moveq	#1,d0
LE49:
	unlk	a6
	rts


| END OF FILE **************************


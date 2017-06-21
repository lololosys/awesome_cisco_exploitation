| $Id: lo_longjmp.s,v 1.1.4.1 1996/05/09 14:59:46 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/lo_longjmp.s,v $
|-----------------------------------------------------------------
| lo_longjmp.s -- setjmp/longjmp routines for 680x0 code.
|
| These are assembly routines that implement setjmp and longjmp.
| This code is intended to be called by C (or assembly).  The longjmp
| handler must be bound to the current cpu exception table to have effect.
|
| November 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: lo_longjmp.s,v $
| Revision 1.1.4.1  1996/05/09  14:59:46  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:46  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:52  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:55  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:18  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:50  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
| setjmp/longjmp routines for 680x0 code.
| These are assembly routines that implement setjmp and longjmp.
| This code is intended to be called by C (or assembly).  The longjmp
| handler must be bound to the current cpu exception table to have effect.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	based on Catalyst, .asciz "@ ( # ) vector.s	1.4 10/22/93"
|	11/23/94  RKL	- Based on Catalyst Boot, modified for Synergy ACP Boot.
| ----------------------------------------------------------------------------
		.data
		.asciz "%W% %G%"

| ----------------------------------------------------------------------------
|	Implement setjmp call.
|
|	Save current register values to jmpbuf structure whose address
|	is passed on stack.  Structure jmpbuf is described in setjmp.h.
|
|	The format of the call is:
|		setjmp (jb)
|		jmpbuf  *jb;
|
|	Register d0 is used for return code; caller must save it if needed.
| ----------------------------------------------------------------------------
		.text
		.globl	_setjmp
		.globl	__setjmp
_setjmp		= __setjmp		| Set this for linker, external users.
__setjmp:
		movel	a6,sp@-		| Push reg a6 to free for scratch use.
		movel	sp@(8),a6	| Fetch address of jmpbuf.
		moveml	d0-d7/a0-a5,a6@	| Save most general-purpose regs.
		lea	a6@(56),a6	| Adjust pointer into jmpbuf.
		movel	sp@+,a6@+	| Pop original a6 into jmpbuf.
		movel	sp,a6@+		| Save original a7/sp into jmpbuf.
|
|	All original general-purpose register values have been saved.
|	Now save SR and PC as well.
|
		moveq	#(0),d0		| Zero out reg d0
		movew	sr,d0		|   and fetch status register to it
		movel	d0,a6@+		|   and save in jmpbuf.
		movel	sp@,a6@+	| Save return PC too for longjmp use.
|
|	Save done, recover original a6 value to register and return to caller.
|
		movel	a6@(-16),a6	| Recover original a6 value.
		moveq	#(0),d0		| Set zero return so that caller knows
		rts			|   return is from setjmp, not longjmp.

| ----------------------------------------------------------------------------
|	Implement longjmp call.
|
|	A longjmp call is issued by the user when they wish to return
|	to the instruction immediatly after the original call to setjmp.
|
|	The format of the call is:
|		longjmp (jb, val)
|		jmpbuf  *jb;
|		long    val;
|
|   The only operation we perform in this routine is to force a trap
|   to the longjmp handler.  We do the trap so we can restore the status
|   register in the trap handler.  This routine should never return.
| ----------------------------------------------------------------------------
		.globl	_longjmp
		.globl	__longjmp
_longjmp	= __longjmp		| Set this for linker, external users.
__longjmp:
|
|	Save arguments across trap.
|
		movel	sp@(4),a6	| Address of jumpbuf
|
|	Val argument is saved in jmpbuf itself at the first word.
|	This is where register d0 is saved, so when registers are
|	restored after the trap Val will be loaded into d0 (ie. return code).
|
		movel	sp@(8),a6@
		trap	#(3)		| Do trap to longjmp handler.
		rts			| Should never get here, but ...

| ----------------------------------------------------------------------------
|	This handler is entered via trap from longjmp.
|	Arguments to longjmp are passed as:
|		address of jmpbuf in a0
|		return code value in jmpbuf, for loading into d0
| ----------------------------------------------------------------------------
		.globl	__longjmp_hdlr
__longjmp_hdlr:
		moveml	a6@,d0-d7/a0-a4	| Restore most registers from jmpbuf.
|
|	Recover stack pointer from jmpbuf and dummy up an exception vector.
|
		movel	a6@(60),a5	| Recover jmpbuf sp.
		movew	sp@(6),a5@-	| Copy exception frame format word.
		movel	a6@(68),a5@-	| Push jmpbuf return address (PC).
		movew	a6@(66),a5@-	| Push jmpbuf Status Register.
|
|	Recover remaining registers from jmpbuf.
|
		movel	a5,sp		| Make jmpbuf stack current stack.
		moveml	a6@(52),a5-a6
|
|	Now fix d0 value if zero.
|
		tstl	d0		| Zero return code reserved for setjmp,
		jne	L_ljh_nzero	|   we cannot use it.
		moveq	#(1),d0		| Force non-zero return code.
L_ljh_nzero:
		rte			| Return from trap to restored context.


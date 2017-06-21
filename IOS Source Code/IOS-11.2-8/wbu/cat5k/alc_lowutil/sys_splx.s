| $Id: sys_splx.s,v 1.1.4.1 1996/05/09 14:59:50 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/sys_splx.s,v $
|-----------------------------------------------------------------
| sys_cache.S -- 680x0 System code processor level routines.
| These are assembly routines that adjust the CPU processor level
| (interrupt level).  This assembly code assumes that a valid
| stack is available.
|
| December 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: sys_splx.s,v $
| Revision 1.1.4.1  1996/05/09  14:59:50  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:50  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:57  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:12:00  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:23  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:54  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
|	680x0 System code processor level routines.
|	These are assembly routines that adjust the CPU processor level
|	(interrupt level).  This assembly code assumes that a valid
|	stack is available.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	based on Catalyst, .asciz "@(#)system.s    1.2 6/24/93"
|	12/08/94  RKL	- Modified Catalyst source to use with Synergy ATM-LC.
| ----------------------------------------------------------------------------
		.text
		.asciz	"%W% %G%"
		.align	1		| Align to even address.

| ----------------------------------------------------------------------------
|	These functions follow C linkage and are callable by C code.
|	In particular, d0.L is used for scratch and return values.
|	Return values and passed arguments are expected to be 32-bits.
| ----------------------------------------------------------------------------

| ----------------------------------------------------------------------------
|	The following functions set the CPU priority level to the
|	value implicit in their name.  They are just various entry
|	points to setup the actual routine (at the bottom) that does
|	all the work.
|
|	Typical usage:
|		long spl0 () ;
|		long splx (long) ;
|
|		long old, new ;
|		old = spl0 () ;		/* Sets level 0, returns cur level.*/
|		old = splx (new) ;	/* Sets level "new", returns cur lvl. */
| ----------------------------------------------------------------------------
		.text
		.globl	_spl0,_spl1,_spl2,_spl3,_spl4,_spl5,_spl6,_spl7,_splx
_spl0:
		moveq	#(0),d0		| Request new level 0.
		jra	L_splx_wrk
_spl1:
		moveq	#(1),d0		| Request new level 1.
		jra	L_splx_wrk
_spl2:
		moveq	#(2),d0		| Request new level 2.
		jra	L_splx_wrk
_spl3:
		moveq	#(3),d0		| Request new level 3.
		jra	L_splx_wrk
_spl4:
		moveq	#(4),d0		| Request new level 4.
		jra	L_splx_wrk
_spl5:
		moveq	#(5),d0		| Request new level 5.
		jra	L_splx_wrk
_spl6:
		moveq	#(6),d0		| Request new level 6.
		jra	L_splx_wrk
_spl7:
		moveq	#(7),d0		| Request new level 7.
		jra	L_splx_wrk
_splx:
		movel	sp@(4),d0	| Request new level as given.
		andil	#(0x0007),d0	| Groom new value to be in range.

L_splx_wrk:				| Main entry point.
		movel	d1,sp@-		| Save reg d1.
		movew	sr,d1		| Grab current SR.
		swap	d0		| Copy current SR
		movew	d1,d0		|   info for return
		swap	d0		|   code use.
		lslw	#(8),d0		| Position new level as SR bit pattern.
		andiw	#(0xf8ff),d1	| Clear the current interrupt level
		orw	d0,d1		|   and replace with new value.
		movew	d1,sr		| Write back to SR.
		swap	d0		| Retrieve previous interrupt level
		lsrw	#(8),d0		|   from SR and isolate value
		andil	#(0x0007),d0	|   for return code.
		movel	sp@+,d1		| Restore reg d1.
		rts			| Return to caller.


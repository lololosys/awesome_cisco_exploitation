| $Id: lo_memblks.s,v 1.1.4.1 1996/05/09 14:59:47 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/lo_memblks.s,v $
|-----------------------------------------------------------------
| lo_memblks.s -- 680x0 low-level utility routines.
|
| These are assembly routines that provide utility functionality.
| They can be called at any state, ie. even at boot time when no
| stack is available.  C wrapper routines are available for these
| utilities, for when a stack is available.
|
| Register use in the assembly routines is destructive, so callers
| must save registers (unless C wrappers are used).
|
| November 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: lo_memblks.s,v $
| Revision 1.1.4.1  1996/05/09  14:59:47  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:47  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:53  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:56  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:19  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:51  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
|	680x0 low-level utility routines.
|	These are assembly routines that provide utility functionality.
|	They can be called at any state, ie. even at boot time when no
|	stack is available.  C wrapper routines are available for these
|	utilities, for when a stack is available.
|
|	Register use in the assembly routines is destructive, so callers
|	must save registers (unless C wrappers are used).
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	11/22/94  RKL	- Wrote original code based on observations of Catalyst
|			  Boot code.
| ----------------------------------------------------------------------------
		.text
		.asciz	"%W% %G%"
		.align	1		| Align to even address.

| ----------------------------------------------------------------------------
|	Assembly wrapper for Byte Block Fill, for C usage.
|		long A_BlockFill (	unsigned char *	pByte,
|					unsigned long	MemSize,
|					unsigned char	TestByte,
|					void		(* WdogFcn)() )
| ----------------------------------------------------------------------------
		.text
		.globl	_A_BlockFill
_A_BlockFill:
		link	fp,#(0)		| Setup stack frame.
		moveml	d2-d7/a2-a6,sp@- |Save registers consumed by fill.
		moveal	fp@(0x08+0x00),a1 |Grab memory start address from stack.
		movel	fp@(0x08+0x04),d1 |Grab memory area size from stack.
		movel	fp@(0x08+0x08),d2 |Grab test byte pattern from stack.
		moveal	fp@(0x08+0x0c),a2 |Grab watchdog svc routine from stack.

		lea	L_A_BlkFil_done,a0
		jra	AR_BlockFill	| Set caller return address and go fill.
L_A_BlkFil_done:
		moveml	sp@+,d2-d7/a2-a6 |Restore registers consumed by fill.
		unlk	fp		| Pop stack frame.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Byte Block Fill.
|	Fill a memory area with a given single-byte pattern.
|	This routine avoids using a stack and will overwrite caller registers.
|
|	Register usage:
|		a7 (sp)		stack pointer
|		a6 (fp)		C stack frame pointer
|		d4-d7/a3-a5	memory test patterns, scratch
|		a2		any watchdog routine
|		a1		memory area base address
|		a0		return address, scratch
|		d3		save return address
|		d2		byte pattern to fill with
|		d1		byte size of memory area
|		d0		return code, scratch
|		d0-d3/a0-a2/a7	as given in general usage notes
| ----------------------------------------------------------------------------
		.globl	AR_BlockFill
AR_BlockFill:
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_AR_BkFil_1	|   Br. if not.
		lea	L_AR_BkFil_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
|
|	Fill with single bytes until the destination address is long-aligned.
|
L_AR_BkFil_1:
		movel	a1,d0		| Get destination address and
		andil	#(3),d0		|   find number of loose bytes, N.
		negl	d0		| Change count N to (-N)
		jeq	L_AR_BkFil_2	|   unless it is 0.

		addql	#(4),d0		| Change count to 4-N.
		subl	d0,d1		| Count of (4-N) bytes will be taken.
		subql	#(1),d0		| Enter loop from top, decr. count by 1.
L_AR_BkFil_1Loop:
		moveb	d2,a1@+
		dbf	d0,L_AR_BkFil_1Loop
|
|	Fill with sets of 8 registers (32 bytes) until (almost) done.
|
L_AR_BkFil_2:
		movel	d2,d0		| Get byte pattern
		lsll	#8,d2		|   and extend it into
		moveb	d0,d2		|   a short-word pattern.
		movew	d2,d0		| Then extend the short
		swap	d2		|   pattern into a 
		movew	d0,d2		|   longword pattern.

		movel	d2,d4		| Copy 2 of longword pattern.
		movel	d2,d5		| Copy 3 of longword pattern.
		movel	d2,d6		| Copy 4 of longword pattern.
		movel	d2,d7		| Copy 5 of longword pattern.
		moveal	d2,a3		| Copy 6 of longword pattern.
		moveal	d2,a4		| Copy 7 of longword pattern.
		moveal	d2,a5		| Copy 8 of longword pattern.

		movel	d1,d0		| Get remaining byte count and find how
		andil	#(0xffffffe0),d0 |  many can be handled by 32-byte loop.
		lea	a1@(d0:L:1),a0	| Adjust address pointers to end of copy
		moveal	a0,a1		|   area for pre-decrement copy.
		subl	d0,d1		| Decrement byte count for area copy.
		lsrl	#(3+2),d0	| Setup loop counter for 32-byte sets.
		jra	L_AR_BkFil_2Ent	| Enter block copy loop at bottom.
L_AR_BkFil_2Out:
		swap	d0		| Outer loop, restore LS loop counter.
L_AR_BkFil_2Inn:
		moveml	d2/d4-d7/a3-a5,a0@-
L_AR_BkFil_2Ent:
		dbf	d0,L_AR_BkFil_2Inn
|
|	Service watchdog if needed.
|
		movel	d0,d2		| Save off loop counter.
		moveal	a0,a5		| Save off current copy address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_AR_BkFil_2Mid	|   Br. if not.
		lea	L_AR_BkFil_2Mid,a0 |Get our own return address
		jra	a2@		|   and go feed watchdog.
L_AR_BkFil_2Mid:
		movel	d2,d0		| Restore loop counter.
		movel	d5,d2		| Restore data pattern.
		moveal	a5,a0		| Restore copy address.
		moveal	d5,a5		| Restore data pattern.

		swap	d0		| Outer loop, restore MS loop counter.
		dbf	d0,L_AR_BkFil_2Out
|
|	Fill with single longs and bytes until finished.
|
L_AR_BkFil_3:
		movel	d1,d0		| Get remaining byte count
		lsrl	#(2),d0		|   and get number of longwords.
		jra	L_AR_BkFil_3Ent	| Enter copy loop at bottom.
L_AR_BkFil_3Loop:
		movel	d2,a1@+		| Copy single longword
		subql	#(4),d1		|   and adjust remaining byte count.
L_AR_BkFil_3Ent:
		dbf	d0,L_AR_BkFil_3Loop

		jra	L_AR_BkFil_4Ent	| Enter single byte copy loop at bottom.
L_AR_BkFil_4Loop:
		moveb	d2,a1@+		| Copy a single byte.
L_AR_BkFil_4Ent:
		dbf	d1,L_AR_BkFil_4Loop

		moveq	#(0),d0		| Set success return code.
L_AR_BkFil_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.

| ----------------------------------------------------------------------------
|	Assembly wrapper for Memory Block Copy, for C usage.
|		long A_BlockCopy (	unsigned char *	pSrc,
|					unsigned char *	pDst,
|					unsigned long	MemSize,
|					void		(* WdogFcn)() )
| ----------------------------------------------------------------------------
		.text
		.globl	_A_BlockCopy
_A_BlockCopy:
		link	fp,#(0)		| Setup stack frame.
		moveml	d2-d7/a2-a6,sp@- |Save registers to be consumed.
		movel	fp@(0x08+0x00),d2 |Grab memory src address from stack.
		moveal	fp@(0x08+0x04),a1 |Grab memory dest address from stack.
		movel	fp@(0x08+0x08),d1 |Grab memory area size from stack.
		moveal	fp@(0x08+0x0c),a2 |Grab watchdog svc routine from stack.

		lea	L_A_BlkCpy_done,a0
		jra	AR_BlockCopy	| Set caller return address and go.
L_A_BlkCpy_done:
		moveml	sp@+,d2-d7/a2-a6 |Restore registers consumed.
		unlk	fp		| Pop stack frame.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Memory Block Copy.
|	Copy data from source area to dest area.
|	This routine avoids using a stack and will overwrite caller registers.
|
|	Register usage:
|		a7 (sp)		stack pointer
|		a6 (fp)		C stack frame pointer
|		d4-d7/a3-a5	memory test patterns, scratch
|		a2		any watchdog routine
|		a1		memory area dest address
|		a0		return address, scratch
|		d3		save return address
|		d2		memory block source address
|		d1		byte size of memory area
|		d0		return code, scratch
|		d0-d3/a0-a2/a7	as given in general usage notes
| ----------------------------------------------------------------------------
		.globl	AR_BlockCopy
AR_BlockCopy:
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_AR_BkCpy_1	|   Br. if not.
		lea	L_AR_BkCpy_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
|
|	Copy single bytes until the destination address is long-aligned.
|
L_AR_BkCpy_1:
		moveal	d2,a0		| Setup source memory pointer.
		movel	a1,d0		| Get destination address and
		andil	#(3),d0		|   find number of loose bytes, N.
		negl	d0		| Change count N to (-N)
		jeq	L_AR_BkCpy_2	|   unless it is 0.

		addql	#(4),d0		| Change count to 4-N.
		subl	d0,d1		| Count of (4-N) bytes will be taken.
		subql	#(1),d0		| Enter loop from top, decr. count by 1.
L_AR_BkCpy_1Loop:
		moveb	a0@+,a1@+
		dbf	d0,L_AR_BkCpy_1Loop
|
|	Copy sets of 8 registers (32 bytes) until (almost) done.
|
L_AR_BkCpy_2:
		movel	d1,d0		| Get remaining byte count and find how
		andil	#(0x0000001f),d1 |  many can be handled by 32-byte loop.
		lsrl	#(3+2),d0	| Setup loop counter for 32-byte sets.
		jra	L_AR_BkCpy_2Ent	| Enter block copy loop at bottom.
L_AR_BkCpy_2Out:
		swap	d0		| Outer loop, restore LS loop counter.
L_AR_BkCpy_2Inn:
		moveml	a0@+,d2/d4-d7/a3-a5
		moveml	d2/d4-d7/a3-a5,a1@
		addal	#(8*4),a1	| Increment dest pointer.
L_AR_BkCpy_2Ent:
		dbf	d0,L_AR_BkCpy_2Inn
|
|	Service watchdog if needed.
|
		movel	d0,d2		| Save off loop counter.
		moveal	a0,a5		| Save off current copy address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_AR_BkCpy_2Mid	|   Br. if not.
		lea	L_AR_BkCpy_2Mid,a0 |Get our own return address
		jra	a2@		|   and go feed watchdog.
L_AR_BkCpy_2Mid:
		movel	d2,d0		| Restore loop counter.
		moveal	a5,a0		| Restore copy address.

		swap	d0		| Outer loop, restore MS loop counter.
		dbf	d0,L_AR_BkCpy_2Out
|
|	Copy single longs and bytes until finished.
|
L_AR_BkCpy_3:
		movel	d1,d0		| Get remaining byte count
		lsrl	#(2),d0		|   and get number of longwords.
		jra	L_AR_BkCpy_3Ent	| Enter copy loop at bottom.
L_AR_BkCpy_3Loop:
		movel	a0@+,a1@+	| Copy single longword
		subql	#(4),d1		|   and adjust remaining byte count.
L_AR_BkCpy_3Ent:
		dbf	d0,L_AR_BkCpy_3Loop

		jra	L_AR_BkCpy_4Ent	| Enter single byte copy loop at bottom.
L_AR_BkCpy_4Loop:
		moveb	a0@+,a1@+	| Copy a single byte.
L_AR_BkCpy_4Ent:
		dbf	d1,L_AR_BkCpy_4Loop

		moveq	#(0),d0		| Set success return code.
L_AR_BkCpy_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.


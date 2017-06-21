| $Id: lo_memdiag.s,v 1.1.4.2 1996/05/31 01:03:19 venkat Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/lo_memdiag.s,v $
|-----------------------------------------------------------------
| lo_memdiag.s -- 680x0 Boot code memory diagnostic routines.
|
| These are assembly routines that test out any memory-mapped device
| that is both readable and writable.  The actual test routines are
| called with register linkage, in case there is no stack (ie. DRAM test).
| There are also C wrapper routines that use a stack for saving
| registers that are trashed by these diag routines.
|
| These routines are generic for use with ANY memory-mapped HW.
| They are destructive, overwriting existing data in tested areas.
|
| It is up to the caller to decide if any diags should be performed,
| or if the CPU instruction cache should be enabled.  Errors are
| reported back to the caller, which can take any recovery action.
|
| November 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: lo_memdiag.s,v $
| Revision 1.1.4.2  1996/05/31  01:03:19  venkat
| CSCdi59149:  Tactical fix of ATM image breakages from 96q1 tools.
| Branch: California_branch
| Assembly file and pmcc (cygnus.c) fixes to get builds to work with 96q1
| tool
| chain
|
| Revision 1.1.4.1  1996/05/09  14:59:48  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:48  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:54  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:57  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:20  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:52  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
|	680x0 Boot code memory diagnostic routines.
|	These are assembly routines that test out any memory-mapped device
|	that is both readable and writable.  The actual test routines are
|	called with register linkage, in case there is no stack (ie. DRAM test).
|	There are also C wrapper routines that use a stack for saving
|	registers that are trashed by these diag routines.
|
|	These routines are generic for use with ANY memory-mapped HW.
|	They are destructive, overwriting existing data in tested areas.
|
|	It is up to the caller to decide if any diags should be performed,
|	or if the CPU instruction cache should be enabled.  Errors are
|	reported back to the caller, which can take any recovery action.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	based on Catalyst, .asciz "@ ( # ) vector.s	1.4 10/22/93"
|	11/18/94  RKL	- Grabbed relevant Catalyst code for Synergy baseline.
|			  Modified Catalyst source to use as Synergy Boot.
| ----------------------------------------------------------------------------

		.text
		.asciz	"%W% %G%"
		.align	1		| Align to even address.

| ----------------------------------------------------------------------------
|	General register usage (regs must be saved/scratched by caller):
|	(a0/a1/d0/d1 are used as scratch by C, we do so too if possible)
|	(d0 is also C return code, so we do same here)
|
|		a7(sp)	- stack for C use
|		a6(fp)	- stack frame pointer for C use, scratch
|		a3(out)	- current memory test address
|		a2(inp)	- address of any watchdog service routine
|		a1(inp)	- memory test start address, scratch
|		a0(inp)	- caller return address, scratch
|		d3	- save caller return address
|		d2(inp)	- test pattern, if needed
|		d2(out)	- memory test data actually read
|		d1(inp)	- memory test size in bytes, if needed
|		d1(out)	- memory test data expected, scratch
|		d0(out)	- return code (0:pass, -1: fail), scratch
| ----------------------------------------------------------------------------

| ----------------------------------------------------------------------------
|	Assembly wrapper for Data Bus Test (Long), for C usage.
|		long A_MD_DataBusL (	unsigned long *	pLong,
|					void		(* WdogFcn)(),
|					unsigned long**	pLastTest,
|					unsigned long *	LastWrite,
|					unsigned long *	LastRead	)
| ----------------------------------------------------------------------------
		.text
		.globl	_A_MD_DataBusL
_A_MD_DataBusL:				| Wrapper for Data Bus Test, Long.
		moveml	d2-d3/a2-a3,sp@- |Save registers consumed by test.
		moveal	sp@(0x10+0x04),a1 |Grab test start address from stack.
		moveal	sp@(0x10+0x08),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_DBL_done,a0
		jra	ARMD_DataBusL	| Set caller return address and go test.
L_A_MD_DBL_done:
		movel	a3,sp@(0x10+0x0c)@(0) |Return last test address.
		movel	d1,sp@(0x10+0x10)@(0) |Return last test data written.
		movel	d2,sp@(0x10+0x14)@(0) |Return last test data read.
		moveml	sp@+,d2-d3/a2-a3 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Data Bus Test, Long.
|	This test crunches two longwords starting at the a1 start address.
| ----------------------------------------------------------------------------
		.globl	ARMD_DataBusL
ARMD_DataBusL:				| Data Bus Test, Long
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_DBL_1	|   Br. if not.
		lea	L_ARMD_DBL_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_DBL_1:
		moveal	a1,a3		| Begin with memory start address.
		moveq	#(0),d2		| Clear out data read value.
		moveq	#(0),d1		| Fill with 0x00000000 pattern.
		moveq	#(-1),d0	| Fill with 0xffffffff pattern.
		movel	d1,a3@		| Write $00000000 long pattern.
		movel	d0,a3@(4)	| Write $ffffffff long pattern,
					|   to take care of bus capacitance.
		movel	a3@,d2		| Read back what was written.
		cmpl	d1,d2		| Did it match ?
		jne	L_ARMD_DBL_fail	|   Br. if not.

		movel	d0,a3@		| Write $ffffffff long pattern.
		movel	d1,a3@(4)	| Write $00000000 long pattern,
					|   to take care of bus capacitance.
		movel	d0,d1		| d1 <- expected read result.
		movel	a3@,d2		| Read back what was written.
		cmpl	d1,d2		| Did it match ?
		jne	L_ARMD_DBL_fail	|   Br. if not.

		moveq	#1,d1		| 1-bit test on each data line.
L_ARMD_DBL_loop:
		movel	d1,a3@		| Write pattern,
		movel	a3@,d2		|   read it back,
		cmpl	d1,d2		|   did it match ?
		jne	L_ARMD_DBL_fail	| Br. if not.

		lsll	#1,d1		| Shift for next bit/data line.
		jne	L_ARMD_DBL_loop	|   Loop until all data lines done.

		moveq	#(0),d0		| Set success return code.
L_ARMD_DBL_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.
L_ARMD_DBL_fail:
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_DBL_exit

| ----------------------------------------------------------------------------
|	Assembly wrapper for Data Bus Test (Byte), for C usage.
|		long A_MD_DataBusB (	unsigned char *	pByte,
|					void		(* WdogFcn)(),
|					unsigned char**	pLastTest,
|					unsigned char *	LastWrite,
|					unsigned char *	LastRead	)
| ----------------------------------------------------------------------------
		.globl	_A_MD_DataBusB
_A_MD_DataBusB:				| Wrapper for Data Bus Test, Byte.
		moveml	d2-d3/a2-a3,sp@- |Save registers consumed by test.
		moveal	sp@(0x10+0x04),a1 |Grab test start address from stack.
		moveal	sp@(0x10+0x08),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_DBB_done,a0
		jra	ARMD_DataBusB	| Set caller return address and go test.
L_A_MD_DBB_done:
		movel	a3,sp@(0x10+0x0c)@(0) |Return last test address.
		moveb	d1,sp@(0x10+0x10)@(0) |Return last test data written.
		moveb	d2,sp@(0x10+0x14)@(0) |Return last test data read.
		moveml	sp@+,d2-d3/a2-a3 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Data Bus Test, Byte.
|	This test crunches two bytes starting at the a1 start address.
| ----------------------------------------------------------------------------
		.globl	ARMD_DataBusB
ARMD_DataBusB:				| Data Bus Test, Byte
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_DBB_1	|   Br. if not.
		lea	L_ARMD_DBB_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_DBB_1:
		moveal	a1,a3		| Begin with memory start address.
		moveq	#(0),d2		| Clear out data read value.
		moveq	#(0),d1		| Fill with 0x00000000 pattern.
		moveq	#(-1),d0	| Fill with 0xffffffff pattern.
		moveb	d1,a3@		| Write $00 byte pattern.
		moveb	d0,a3@(1)	| Write $ff byte pattern,
					|   to take care of bus capacitance.
		moveb	a3@,d2		| Read back what was written.
		cmpb	d1,d2		| Did it match ?
		jne	L_ARMD_DBB_fail	|   Br. if not.

		moveb	d0,a3@		| Write $ff byte pattern.
		moveb	d1,a3@(1)	| Write $00 byte pattern,
					|   to take care of bus capacitance.
		moveb	d0,d1		| d1 <- expected read result.
		moveb	a3@,d2		| Read back what was written.
		cmpb	d1,d2		| Did it match ?
		jne	L_ARMD_DBB_fail	|   Br. if not.

		moveq	#1,d1		| 1-bit test on each data line.
L_ARMD_DBB_loop:
		moveb	d1,a3@		| Write pattern,
		moveb	a3@,d2		|   read it back,
		cmpb	d1,d2		|   did it match ?
		jne	L_ARMD_DBB_fail	| Br. if not.

		lslb	#1,d1		| Shift for next bit/data line.
		jne	L_ARMD_DBB_loop	|   Loop until all data lines done.

		moveq	#(0),d0		| Set success return code.
L_ARMD_DBB_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.
L_ARMD_DBB_fail:
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_DBB_exit

| ----------------------------------------------------------------------------
|	Assembly wrapper for Address Bus Test (Long), for C usage.
|		long A_MD_AddrBusL (	unsigned long *	pLong,
|					unsigned long	MemSize,
|					void		(* WdogFcn)(),
|					unsigned long**	pLastTest,
|					unsigned long *	LastWrite,
|					unsigned long *	LastRead	)
| ----------------------------------------------------------------------------
		.globl	_A_MD_AddrBusL
_A_MD_AddrBusL:				| Wrapper for Address Bus Test, Long.
		moveml	d2-d5/a2-a3,sp@- |Save registers consumed by test.
		moveal	sp@(0x18+0x04),a1 |Grab test start address from stack.
		movel	sp@(0x18+0x08),d1 |Grab memory test size from stack.
		moveal	sp@(0x18+0x0c),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_ABL_done,a0
		jra	ARMD_AddrBusL	| Set caller return address and go test.
L_A_MD_ABL_done:
		movel	a3,sp@(0x18+0x10)@(0) |Return last test address.
		movel	d1,sp@(0x18+0x14)@(0) |Return last test data written.
		movel	d2,sp@(0x18+0x18)@(0) |Return last test data read.
		moveml	sp@+,d2-d5/a2-a3 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Address Bus Test, Long.
|	Write to addresses based on address line pairs.  Outer loop sets
|	one address line bit; inner loop tests all other address lines.
|	This routine uses registers d4/d5 for additional scratch.
| ----------------------------------------------------------------------------
		.globl	ARMD_AddrBusL
ARMD_AddrBusL:				| Address Bus Test, Long.
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_ABL_1	|   Br. if not.
		lea	L_ARMD_ABL_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_ABL_1:
		lea	a1@(d1:L:1),a0	| Get memory end address.
		moveal	a1,a2		| Setup outer loop address pointer.
		moveq	#(4),d4		| Setup outer loop address line.
		movel	#(0x55555555),d1 |Setup primary (outer loop) pattern.
		movel	#(0xAAAAAAAA),d0 |Setup secondary (inner loop) pattern.
		moveq	#(0),d2		| Clear out data read value.

L_ARMD_ABL_Outer:			| Outer address line test loop.
		moveal	a1,a3		| Setup inner loop address pointer.
		moveq	#(4),d5		| Setup inner loop address line.
		movel	d1,a2@		| Write primary pattern via outer ptr.

L_ARMD_ABL_Inner:			| Inner address line test loop.
		cmpal	a2,a3		| If inner address == outer address
		jeq	L_ARMD_ABL_Skip	|   br. around secondary pattern write.

		movel	d0,a3@		| Write secondary pattern via inner ptr.
L_ARMD_ABL_Skip:
		movel	a2@,d2		| Read back pattern from outer loop ptr.
		cmpl	d1,d2		| Did it match ?
		jne	L_ARMD_ABL_fail	|   Br. if not

		lea	a1@(d5:L:1),a3	| Get next inner loop address value
		lsll	#1,d5		|   and step for next address line.
		cmpal	a3,a0		| Have we exceeded memory yet ?
		jgt	L_ARMD_ABL_Inner |  -- if not, (inner) loop for more.

		lea	a1@(d4:L:1),a2	| Get next outer loop address value
		lsll	#1,d4		|   and step for next address line.
		cmpal	a2,a0		| Have we exceeded memory yet ?
		jgt	L_ARMD_ABL_Outer |  -- if not, (outer) loop for more.

		moveq	#(0),d0		| Set success return code.
L_ARMD_ABL_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.
L_ARMD_ABL_fail:
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_ABL_exit

| ----------------------------------------------------------------------------
|	Assembly wrapper for Address Bus Test (Byte), for C usage.
|		long A_MD_AddrBusB (	unsigned char *	pByte,
|					unsigned long	MemSize,
|					void		(* WdogFcn)(),
|					unsigned char**	pLastTest,
|					unsigned char *	LastWrite,
|					unsigned char *	LastRead	)
| ----------------------------------------------------------------------------
		.globl	_A_MD_AddrBusB
_A_MD_AddrBusB:				| Wrapper for Address Bus Test, Byte.
		moveml	d2-d5/a2-a3,sp@- |Save registers consumed by test.
		moveal	sp@(0x18+0x04),a1 |Grab test start address from stack.
		movel	sp@(0x18+0x08),d1 |Grab memory test size from stack.
		moveal	sp@(0x18+0x0c),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_ABB_done,a0
		jra	ARMD_AddrBusB	| Set caller return address and go test.
L_A_MD_ABB_done:
		movel	a3,sp@(0x18+0x10)@(0) |Return last test address.
		movel	d1,sp@(0x18+0x14)@(0) |Return last test data written.
		movel	d2,sp@(0x18+0x18)@(0) |Return last test data read.
		moveml	sp@+,d2-d5/a2-a3 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Address Bus Test, Byte.
|	Write to addresses based on address line pairs.  Outer loop sets
|	one address line bit; inner loop tests all other address lines.
|	This routine uses registers d4/d5 for additional scratch.
| ----------------------------------------------------------------------------
		.globl	ARMD_AddrBusB
ARMD_AddrBusB:				| Address Bus Test, Byte.
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_ABB_1	|   Br. if not.
		lea	L_ARMD_ABB_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_ABB_1:
		lea	a1@(d1:L:1),a0	| Get memory end address.
		moveal	a1,a2		| Setup outer loop address pointer.
		moveq	#(1),d4		| Setup outer loop address line.
		moveq	#(0x55),d1	| Setup primary (outer loop) pattern.
		moveb	#(0xAA),d0	| Setup secondary (inner loop) pattern.
		moveq	#(0),d2		| Clear out data read value.

L_ARMD_ABB_Outer:			| Outer address line test loop.
		moveal	a1,a3		| Setup inner loop address pointer.
		moveq	#(1),d5		| Setup inner loop address line.
		moveb	d1,a2@		| Write primary pattern via outer ptr.

L_ARMD_ABB_Inner:			| Inner address line test loop.
		cmpal	a2,a3		| If inner address == outer address
		jeq	L_ARMD_ABB_Skip	|   br. around secondary pattern write.

		moveb	d0,a3@		| Write secondary pattern via inner ptr.
L_ARMD_ABB_Skip:
		moveb	a2@,d2		| Read back pattern from outer loop ptr.
		cmpb	d1,d2		| Did it match ?
		jne	L_ARMD_ABB_fail	|   Br. if not

		lea	a1@(d5:L:1),a3	| Get next inner loop address value
		lsll	#1,d5		|   and step for next address line.
		cmpal	a3,a0		| Have we exceeded memory yet ?
		jgt	L_ARMD_ABB_Inner |  -- if not, (inner) loop for more.

		lea	a1@(d4:L:1),a2	| Get next outer loop address value
		lsll	#1,d4		|   and step for next address line.
		cmpal	a2,a0		| Have we exceeded memory yet ?
		jgt	L_ARMD_ABB_Outer |  -- if not, (outer) loop for more.

		moveq	#(0),d0		| Set success return code.
L_ARMD_ABB_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.
L_ARMD_ABB_fail:
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_ABB_exit

| ----------------------------------------------------------------------------
|	Assembly wrapper for Byte, Word Enable Test for C usage.
|		long A_MD_ByteWordEnb (	unsigned long *	pLong,
|					void		(* WdogFcn)(),
|					unsigned long**	pLastTest,
|					unsigned long *	LastWrite,
|					unsigned long *	LastRead	)
| ----------------------------------------------------------------------------
		.globl	_A_MD_ByteWordEnb
_A_MD_ByteWordEnb:			| Wrapper for Byte, Word Enable tests.
		moveml	d2-d3/a2-a3,sp@- |Save registers consumed by test.
		moveal	sp@(0x10+0x04),a1 |Grab test start address from stack.
		moveal	sp@(0x10+0x08),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_BWE_done,a0
		jra	ARMD_ByteWordEnb |Set caller return address and go test.
L_A_MD_BWE_done:
		movel	a3,sp@(0x10+0x0c)@(0) |Return last test address.
		movel	d1,sp@(0x10+0x10)@(0) |Return last test data written.
		movel	d2,sp@(0x10+0x14)@(0) |Return last test data read.
		moveml	sp@+,d2-d3/a2-a3 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Perform Byte/Word Enable tests.
|	Compare byte and word access results with long access results.
| ----------------------------------------------------------------------------
		.globl	ARMD_ByteWordEnb
ARMD_ByteWordEnb:			| Byte, Word Enable tests.
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_BWE_1	|   Br. if not.
		lea	L_ARMD_BWE_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_BWE_1:
		moveal	a1,a3		| Set access address to indicated mem.
		moveq	#(0),d2		| Zero comparison register (d2).
		movel	#(0x12345678),d1 |Setup source data pattern.
		moveq	#(4-1),d0	| Setup byte-per-long loop counter.
L_ARMD_BWE_B:
		roll	#(8),d1		| Rotate pattern to get next test byte.
		moveb	d1,a3@		| Write test source data.
		moveb	a3@,d2		| Read back test result.
		cmpb	d1,d2		| Did read match write ?
		jne	L_ARMD_BWE_bfail |  Br. if NOT.

		addql	#(1),a3		| Incr. for next byte address in long.
		dbf	d0,L_ARMD_BWE_B	| Loop to test next byte in long word.

		moveal	a1,a3		| Set access address to indicated mem.
		movel	a3@,d2		| Read entire pattern written above.
		cmpl	d1,d2		| Did read match write ?
		jne	L_ARMD_BWE_fail	|   Br. if NOT.

		moveq	#(0),d2		| Zero comparison register (d2).
		movel	#(0x87654321),d1 |Setup source data pattern.
		moveq	#(2-1),d0	| Setup word-per-long loop counter.
L_ARMD_BWE_W:
		swap	d1		| Rotate pattern to get next test word.
		movew	d1,a3@		| Write test source data.
		movew	a3@,d2		| Read back test result.
		cmpw	d1,d2		| Did read match write ?
		jne	L_ARMD_BWE_wfail |  Br. if NOT.

		addql	#(2),a3		| Incr. for next word address in long.
		dbf	d0,L_ARMD_BWE_W	| Loop to test next word in long word.

		moveal	a1,a3		| Set access address to indicated mem.
		movel	a3@,d2		| Read entire pattern written above.
		cmpl	d1,d2		| Did read match write ?
		jne	L_ARMD_BWE_fail	|   Br. if NOT.

		moveq	#(0),d0		| Set success return code.
L_ARMD_BWE_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.

L_ARMD_BWE_bfail:
		andiw	#(0x00ff),d1	| Isolate source test byte that failed.
L_ARMD_BWE_wfail:
		andil	#(0x0000ffff),d1 |Isolate source test word that failed.
L_ARMD_BWE_fail:
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_BWE_exit

| ----------------------------------------------------------------------------
|	Assembly wrapper for Pattern Read/Write Test, for C usage.
|		long A_MD_RW_Pattern (	unsigned long *	pLong,
|					unsigned long	MemSize,
|					unsigned long	TestPat,
|					void		(* WdogFcn)(),
|					unsigned long**	pLastTest,
|					unsigned long *	LastExpected,
|					unsigned long *	LastRead	)
| ----------------------------------------------------------------------------
		.globl	_A_MD_RW_Pattern
_A_MD_RW_Pattern:			| Wrapper for Pattern Read/Write Test.
		moveml	d2-d7/a2-a6,sp@- |Save registers consumed by test.
		moveal	sp@(0x2c+0x04),a1 |Grab test start address from stack.
		movel	sp@(0x2c+0x08),d1 |Grab memory test size from stack.
		movel	sp@(0x2c+0x0c),d2 |Grab test pattern from stack.
		moveal	sp@(0x2c+0x10),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_RWP_done,a0
		jra	ARMD_RW_Pattern	| Set caller return address and go test.
L_A_MD_RWP_done:
		movel	a3,sp@(0x2c+0x14)@(0) |Return last test address.
		movel	d1,sp@(0x2c+0x18)@(0) |Return test data expected.
		movel	d2,sp@(0x2c+0x1c)@(0) |Return test data read.
		moveml	sp@+,d2-d7/a2-a6 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Pattern Read/Write Test.
|	This test alternately writes a pattern and its inverse throughout
|	a memory area, and then reads back the results to check.
|	Typically this is called twice:  $55/$aa and $aa/$55.
|	This implementation requires that memory area size >= 32 bytes,
|	and be an integer multiple of 32 bytes.
|
|	Register usage:
|		d0-d3/a0-a3/a7	as given in general usage notes
|		d4-d7/a5-a6	memory test patterns, scratch
|		a4		memory area end address
| ----------------------------------------------------------------------------
		.globl	ARMD_RW_Pattern
ARMD_RW_Pattern:			| Read/Write Pattern test.
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_RWP_1	|   Br. if not.
		lea	L_ARMD_RWP_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
|
|	Format memory with the given test pattern.  This is done with
|	pre-decrementing, so start at the END of the given memory
|	address range.
|
L_ARMD_RWP_1:
		lea	a1@(d1:L:1),a4	| Find end of given memory area
		moveal	a4,a3		|   and set current access address.
		movel	d1,d0		| Setup loop count for number of bytes
		lsrl	#(3+2),d0	|   and change to sets of 8*4=32 bytes.

		movel	d2,d1		| Setup source data pattern A(1).
		movel	d2,d4		| Setup source data pattern A(2).
		movel	d2,d6		| Setup source data pattern A(3).
		moveal	d2,a5		| Setup source data pattern A(4).

		notl	d2		| Setup source data pattern B(1).
		movel	d2,d5		| Setup source data pattern B(2).
		movel	d2,d7		| Setup source data pattern B(3).
		moveal	d2,a6		| Setup source data pattern B(4).

		jra	L_ARMD_RWP_WrGo	| Enter loop at bottom for exact count.

L_ARMD_RWP_WrOut:
		swap	d0		| Restore loop counter LS word.
L_ARMD_RWP_WrInn:
		moveml	d1/d2/d4-d7/a5/a6,a3@-
					| Write 8 patterns downwards in memory.
L_ARMD_RWP_WrGo:
		dbf	d0,L_ARMD_RWP_WrInn	| Inner loop.
|
|	Service watchdog if needed.
|
		movel	d0,d2		| Save off loop counter.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_RWP_2	|   Br. if not.
		lea	L_ARMD_RWP_2,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_RWP_2:
		movel	d2,d0		| Restore loop counter.
		movel	d5,d2		| Restore source data pattern B(1).

		swap	d0		| Restore loop counter MS word.
		dbf	d0,L_ARMD_RWP_WrOut	| Outer loop.

		cmpal	a1,a3		| Verify that all memory was formatted.
		jeq	L_ARMD_RWP_Scan	| Br. if normal.
 
		movel	a1,d1		| Setup memory ptr expected value.
		swap	d0		| Restore loop counter LS word
		movel	d0,d2		|   and setup as bad address offset.
		jra	L_ARMD_RWP_fail	| Go indicate memory test error.
|
|	Now read back and verify the written data.  This is done with
|	post-incrementing, so start at the BEGINNING of the given
|	memory address range.
|
L_ARMD_RWP_Scan:
		moveal	a1,a3		| Begin read pointer at memory start.
		movel	a4,d0		| Find size of memory area
		subl	a1,d0		|   and setup loop count of sets
		lsrl	#(2),d0		|   of single longs = 4 bytes.
		moveq	#(0),d2		| Zero out data read back register.
		jra	L_ARMD_RWP_RdGo	| Enter loop at bottom for exact count.

L_ARMD_RWP_RdOut:
		swap	d0		| Restore loop counter LS word.
L_ARMD_RWP_RdInn:
		movel	a3@,d2		| Read a longword from memory.
		cmpl	d1,d2		| Did it match expected value ?
		jne	L_ARMD_RWP_fail	|   Br. if NOT.

		addql	#(4),a3		| Bump test address to next longword.
		notl	d1		| Flip to alternate pattern.
L_ARMD_RWP_RdGo:
		dbf	d0,L_ARMD_RWP_RdInn	| Inner loop.
|
|	Service watchdog if needed.
|
		movel	d0,d2		| Save off loop counter.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_RWP_3	|   Br. if not.
		lea	L_ARMD_RWP_3,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_RWP_3:
		movel	d2,d0		| Restore loop counter.
		moveq	#(0),d2		| Zero out data read back register.

		swap	d0		| Restore loop counter MS word.
		dbf	d0,L_ARMD_RWP_RdOut	| Outer loop.

		cmpal	a4,a3		| Verify that all memory was tested.
		jeq	L_ARMD_RWP_4	| Br. if normal.
 
		movel	a4,d1		| Setup memory ptr expected value.
		swap	d0		| Restore loop counter LS word
		movel	d0,d2		|   and setup as bad address offset.
		jra	L_ARMD_RWP_fail	| Go indicate memory test error.
L_ARMD_RWP_4:
		moveq	#(0),d0		| Set success return code.
L_ARMD_RWP_exit:
		moveal	d3,a0		| Recover caller return address
		jra	a0@		|   and leave.
L_ARMD_RWP_fail:
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_RWP_exit

| ----------------------------------------------------------------------------
|	Assembly wrapper for Read/Write Byte Patterns Test for C usage.
|		long A_MD_RW_Byte (	unsigned char *	pByte,
|					unsigned long	TestPat,
|					void		(* WdogFcn)(),
|					unsigned char**	pLastTest,
|					unsigned char *	LastWrite,
|					unsigned char *	LastRead	)
| ----------------------------------------------------------------------------
		.globl	_A_MD_RW_Byte
_A_MD_RW_Byte:				| Wrapper for Read/Write Byte Patterns.
		moveml	d2-d3/a2-a3,sp@- |Save registers consumed by test.
		moveal	sp@(0x10+0x04),a1 |Grab test byte address from stack.
		movel	sp@(0x10+0x08),d2 |Grab longword pattern from stack.
		moveal	sp@(0x10+0x0c),a2 |Grab watchdog svc routine from stack.
		lea	L_A_MD_RWB_done,a0
		jra	ARMD_RW_Byte	| Set caller return address and go test.
L_A_MD_RWB_done:
		movel	a3,sp@(0x10+0x10)@(0) |Return last test address.
		movel	d1,sp@(0x10+0x14)@(0) |Return last test data written.
		movel	d2,sp@(0x10+0x18)@(0) |Return last test data read.
		moveml	sp@+,d2-d3/a2-a3 |Restore registers consumed by test.
		rts			| Return to caller.

| ----------------------------------------------------------------------------
|	Read/Write Byte Patterns Test.
|	This test writes and verifies a set of patterns to a single
|	byte address.  It is suitable for testing accesses to devices
|	such as MFPs, UARTs, H/W I/Os etc. that have memory-mapped registers
|	that are both readable and writable.  A long argument allows up to
|	four separate byte patterns, and each pattern is tested along with
|	its inverse.
|
|	It is up to the caller to drop any resets so that bus timeouts
|	do not occur.
| ----------------------------------------------------------------------------
		.globl	ARMD_RW_Byte
ARMD_RW_Byte:				| Read/Write Pattern test.
		movel	a0,d3		| Save caller return address.
		tstl	a2		| Is there a watchdog service routine ?
		jeq	L_ARMD_RWB_1	|   Br. if not.
		lea	L_ARMD_RWB_1,a0	| Get our own return address
		jra	a2@		|   and go feed watchdog.
L_ARMD_RWB_1:
		moveal	a1,a3		| Get memory access address.
		moveq	#(0),d0		| Read the current contents
		moveb	a3@,d0		|   at the given memory access
		moveal	d0,a0		|   address and save it off.
		movel	d2,d1		| Setup the source test pattern
		moveq	#(0),d2		|   and zero the read back register.
		moveq	#(4-1),d0	| Loop for each byte in long pattern.
L_ARMD_RWB_Loop:
		roll	#(8),d1		| Rotate MS byte as next test pattern.
		moveb	d1,a3@		| Write test pattern byte.
| ***		nop			| Let things settle.
		moveb	a3@,d2		| Read back test value.
		cmpb	d1,d2		| Was there a match ?
		jne	L_ARMD_RWB_fail	|   Br. if NOT.

		notb	d1		| Invert source test pattern.
		moveb	d1,a3@		| Write test pattern byte.
| ***		nop			| Let things settle.
		moveb	a3@,d2		| Read back test value.
		cmpb	d1,d2		| Was there a match ?
		jne	L_ARMD_RWB_fail	|   Br. if NOT.

		dbf	d0,L_ARMD_RWB_Loop
|
|	Test is done, restore saved memory contents for test address.
|
		moveq	#(0),d0		| Set success return code.
L_ARMD_RWB_exit:
		exg	d3,a0		| Recover caller return address
		moveb	d3,a3@		|   and original data contents
		jra	a0@		|   and leave.
L_ARMD_RWB_fail:
		andil	#(0x000000ff),d1 |Isolate failed source test pattern.
		moveq	#(-1),d0	| Set failure return code.
		jra	L_ARMD_RWB_exit


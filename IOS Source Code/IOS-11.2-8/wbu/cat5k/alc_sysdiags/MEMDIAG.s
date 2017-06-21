| $Id: MEMDIAG.s,v 1.1.4.1 1996/05/09 15:00:44 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/MEMDIAG.s,v $
|-----------------------------------------------------------------
| MEMDIAG.s -- 
|
| ?
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: MEMDIAG.s,v $
| Revision 1.1.4.1  1996/05/09  15:00:44  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:16:42  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:42:02  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:12:53  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:53:36  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:43:13  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

|******************************************************************************/
|*                                                                            */
|* DIAGNOSTICS TEST SUITES                                                    */
|*                                                                            */
|* This file contains procedures related to "Memory Diagnostics Section" that */
|* are included as part of Diagnostics Test Suites.                           */
|*                                                                            */
|* NOTE TO THE REVIEWER:                                                      */
|* --------------------                                                       */
|* You may find that Comments & Remarks are kept to the minimum extent, which */
|* eventually would be updated in due course of time.  The author believes in */
|* "Self-documentation" by way of meaningful names for procedures, variables  */
|* etc. and "Hierarchical programming" style which would work out better in   */
|* terms of time & productivity.  In the meantime, if you encounter any       */
|* problems in going thru the code or have any comments/suggestions, do let   */
|* the author know about it.                                                  */
|*                                                                            */
|******************************************************************************/

	.text
|MEMDIAG	IDNT
|	SECTION	code,,C

|	XREF _sys_fail_address
|	XREF _sys_data_expected
|	XREF _sys_data_read

	.globl _faccess_test
|
| Register Usage: a0 - Start Address
|		  a1 - End Address
|		  a2 - Fail Address
|		  d0 - Return Code
|		  d1 - Data Written/Expected
|		  d2 - Data Read
|
_faccess_test:
	moveml  d1-d7/a0-a6,sp@-

	moveal	sp@(0x3C),a0	| Start Address
	moveal	sp@(0x40),a1	| End Address
	movel	sp@(0x44),d1	| Data

	cmpil	#0xfeedbac,d1
	bne	_memory_normal_test
	jsr	_memory_test_55AA
	bra	_memory_test_check

_memory_normal_test:
	jsr	_memory_test_normal

_memory_test_check:
	cmpil	#0x0,d0
	beq	_return

_error:
	movel	a2,_sys_fail_address
	movel	d1,_sys_data_expected
	movel	d2,_sys_data_read

_return:
	moveml	sp@+,d1-d7/a0-a6
	rts

|-----------------------------------------------------------------
_memory_test_55AA:
	movel	#0x1,d0

	moveal  a0,a2

|
| Perform Byte/Word Enable tests
|
	movel	 #0x12,d1
	moveb	 d1,a2@(0x0)
	moveb	 a2@(0x0),d2
	andil	 #0xFF,d2
	cmpl	 d1,d2
        bne      _error_55AA
	movel	 #0x34,d1
	moveb	 d1,a2@(0x1)
	moveb	 a2@(0x1),d2
	andil	 #0xFF,d2
	cmpl	 d1,d2
        bne      _error_55AA
	movel	 #0x56,d1
	moveb	 d1,a2@(0x2)
	moveb	 a2@(0x2),d2
	andil	 #0xFF,d2
	cmpl	 d1,d2
        bne      _error_55AA
	movel	 #0x78,d1
	moveb	 d1,a2@(0x3)
	moveb	 a2@(0x3),d2
	andil	 #0xFF,d2
	cmpl	 d1,d2
        bne      _error_55AA
	movel	 #0x12345678,d1
	movel	 a2@,d2
        cmpl     d1,d2
        bne      _error_55AA

	movel	 #0x8765,d1
	movew	 d1,a2@(0x0)
	movew	 a2@(0x0),d2
	andil	 #0xFFFF,d2
	cmpl	 d1,d2
        bne      _error_55AA
	movel	 #0x4321,d1
	movew	 d1,a2@(0x2)
	movew	 a2@(0x2),d2
	andil	 #0xFFFF,d2
	cmpl	 d1,d2
        bne      _error_55AA
	movel	 #0x87654321,d1
	movel	 a2@,d2
        cmpl     d1,d2
        bne      _error_55AA

	moveal	a0,a2
	movel	#0x55555555,d1
_wloop_55AA:
	movel	d1,a2@+
	notl	d1
	| * punch watch dog every 64k walk
	tstw	a2
	beq	_wloop1
	moveb   #1,0x38000017
	moveb   #0,0x38000017
_wloop1:
	cmpal	a2,a1
	bge	_wloop_55AA

	moveal	a0,a2
	movel	#0x55555555,d1
_cloop_55AA:
	movel	a2@,d2
	cmpl	d1,d2
	bne	_error_55AA
	notl	d1
	addql	#4,a2
	| * punch watch dog every 64k walk
	tstw	a2
	beq	_cloop1
	moveb   #1,0x38000017
	moveb   #0,0x38000017
_cloop1:
	cmpal	a2,a1
	bge	_cloop_55AA

	moveal	a0,a2
	movel	#0xAAAAAAAA,d1
_wloop_AA55:
	movel	d1,a2@+
	notl	d1
	| * punch watch dog every 64k walk
	tstw	a2
	beq	_wloop2
	moveb   #1,0x38000017
	moveb   #0,0x38000017
_wloop2:
	cmpal	a2,a1
	bge	_wloop_AA55

	moveal	a0,a2
	movel	#0xAAAAAAAA,d1
_cloop_AA55:
	movel	a2@,d2
	cmpl	d1,d2
	bne	_error_AA55
	notl	d1
	addql	#4,a2
	| * punch watch dog every 64k walk
	tstw	a2
	beq	_cloop2
	moveb   #1,0x38000017
	moveb   #0,0x38000017
_cloop2:
	cmpal	a2,a1
	bge	_cloop_AA55

	movel	#0x0,d0

_error_AA55:
_error_55AA:
	rts
|-----------------------------------------------------------------

|-----------------------------------------------------------------
_memory_test_normal:
	movel	#0x1,d0

	moveal  a0,a2
	movel	d1,d3
_wloop_normal:
	movel	d1,a2@+
	notl	d1
	| * punch watch dog every 64k walk
	tstw	a2
	beq	_wloopn1
	moveb   #1,0x38000017
	moveb   #0,0x38000017
_wloopn1:
	cmpal	a2,a1
	bge	_wloop_normal

	moveal	a0,a2
	movel	d3,d1
_cloop_normal:
	movel	a2@,d2
	cmpl	d1,d2
	bne	_error_normal
	notl	d1
	addql	#4,a2
	| * punch watch dog every 64k walk
	tstw	a2
	beq	_cloopn1
	moveb   #1,0x38000017
	moveb   #0,0x38000017
_cloopn1:
	cmpal	a2,a1
	bge	_cloop_normal

	movel	#0x0,d0

_error_normal:
	rts
|-----------------------------------------------------------------

_cpu_speed_test:
	movel	_system_tick_time,_cpu_speed_test_start_time

	movel	_cpu_speed_test_loop_count,d0
_cpu_speed_test_loop:
	subql	#1,d0
	bne	_cpu_speed_test_loop

	movel	_system_tick_time,_cpu_speed_test_end_time

	movel	_cpu_speed_test_end_time,d0
	subl	_cpu_speed_test_start_time,d0
	movel	d0,_cpu_speed_test_elapsed_time

	rts
	rts

	.data
_cpu_speed_test_loop_count:
	.long	1000000
_cpu_speed_test_start_time:
	.long	0
_cpu_speed_test_end_time:
	.long	0
_cpu_speed_test_elapsed_time:
	.long	0


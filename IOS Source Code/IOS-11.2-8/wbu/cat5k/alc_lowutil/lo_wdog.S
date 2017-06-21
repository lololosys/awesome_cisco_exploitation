| $Id: lo_wdog.S,v 1.1.4.1 1996/05/09 14:59:49 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/lo_wdog.S,v $
|-----------------------------------------------------------------
| lo_wdog.S -- 680x0 ACP watchdog routines.
|
| These routines feed the ACP watchdog.  Different routines are available
| depending on whether or not there is a stack.
|
| November 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: lo_wdog.S,v $
| Revision 1.1.4.1  1996/05/09  14:59:49  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:49  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:55  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:58  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:21  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:52  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
| 680x0 ACP watchdog routines.
| These routines feed the ACP watchdog.  Different routines are available
| depending on whether or not there is a stack.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	11/23/94  RKL	- Inspired by relevant Catalyst Boot code.
|			  Made for Synergy ACP Boot code.
| ----------------------------------------------------------------------------

#include "asm_mode_defs.h"
#include "boot_defs.h"

		.text
		.asciz	"%W% %G%"
		.align	1			| Align to even address.

| ----------------------------------------------------------------------------
|	Service watchdog by writing 1 followed by writing 0, to ensure
|	that a pulse is generated.  This routine uses register linkage
|	and is NOT callable from C.  This routine does not consume any
|	additional registers.
|	Register usage:  a0	- caller return address
| ----------------------------------------------------------------------------
		.text
		.globl	AR_SvcWatchdog
		.globl	_AR_SvcWatchdog
_AR_SvcWatchdog	= AR_SvcWatchdog
AR_SvcWatchdog:
		moveb	#(1),((BOOT_HW_IOREG_START)+(ASM_HW_WREG_WATCHDOG))
		moveb	#(0),((BOOT_HW_IOREG_START)+(ASM_HW_WREG_WATCHDOG))
		jra	a0@

| ----------------------------------------------------------------------------
|	Service watchdog by writing 1 followed by writing 0, to ensure
|	that a pulse is generated.  This routine expects a stack and
|	so is callable from C.
|	C prototype:	void A_SvcWatchdog () ;
|	Register usage:  a0	- scratch, watchdog HW pointer
| ----------------------------------------------------------------------------
		.globl	_A_SvcWatchdog
_A_SvcWatchdog:
		lea	((BOOT_HW_IOREG_START)+(ASM_HW_WREG_WATCHDOG)),a0
		moveb	#(1),a0@
		moveb	#(0),a0@
		rts


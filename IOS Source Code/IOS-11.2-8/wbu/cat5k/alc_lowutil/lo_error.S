| $Id: lo_error.S,v 1.1.4.1 1996/05/09 14:59:45 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/lo_error.S,v $
|-----------------------------------------------------------------
| lo_error.S -- Low-level 680x0 error handling.
|
| These routines perform low-level error handling.  They generally
| cannot assume that a stack is available.
|
| November 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: lo_error.S,v $
| Revision 1.1.4.1  1996/05/09  14:59:45  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:45  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:51  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:54  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:18  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:49  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.7  1995/10/13  17:51:27  rbau
| set status LED to solid red in case of diag failure. turn off PMD tx/rx LEDs.
| added cvs keywords in the header
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "mc68ec030_defs.h"
#include "boot_defs.h"
#include "bt_nvram.h"
#include "lo_error.h"

		.text
		.asciz	"%W% %G%"
		.align	1			| Align to even address.

| ----------------------------------------------------------------------------
|	Assembly wrapper for diags error routine, for C usage.
|		void A_DiagFail0 (	unsigned long	ErrTestIndex,
|					unsigned long	ErrCounterOfst,
|					void		(* WdogFcn)(),
|					unsigned long * pLastTest,
|					unsigned long   LastWrite,
|					unsigned long   LastRead	)
| ----------------------------------------------------------------------------
		.text
		.globl	_A_DiagFail0
_A_DiagFail0:
		link	fp,#(0)			| Setup stack frame.
		moveml	d2-d3/a2-a3,sp@-	| Save registers consumed.
		movel	fp@(0x08+0x00),d0	| Index of failed test.
		moveal	fp@(0x08+0x04),a1	| Offset to NVRAM error counter.
		moveal	fp@(0x08+0x08),a2	| Addr watchdog service routine.
		moveal	fp@(0x08+0x0c),a3	| Failed test address.
		movel	fp@(0x08+0x10),d1	| Failed test data expected.
		movel	fp@(0x08+0x14),d2	| Failed test data read.
		lea	L_A_DF0_1,a0		| Caller return address.
		jra	AR_DiagFail0		| Do diag failure processing.
L_A_DF0_1:					|   (Should NOT return !)
		moveml	sp@+,d2-d3/a2-a3	| Restore registers consumed.
		unlk	fp			| Release stack frame.
		rts				| Return to caller.

| ----------------------------------------------------------------------------
|	Handle general diag failures.  This routine uses register linkage
|	and can only be called by assembly.  It is meant to be called when
|	DRAM and UART fail diags or have not been checked.
|	Register usage:	a3 (in)	- error test address, scratch
|			a2 (in)	- addr watchdog service routine
|			a1 (in)	- Boot NVRAM area offset for error counter
|			a0 (in)	- scratch, caller return address.
|			d3	- save error test index (d0)
|			d2 (in)	- error test data read
|			d1 (in)	- error test data expected
|			d0 (in)	- error test index (for debug LED display)
| ----------------------------------------------------------------------------
		.globl	AR_DiagFail0
AR_DiagFail0:
		movel	d0,d3			| Save given error test index.
		tstl	a2			| Is there a wdog svc routine ?
		jeq	AR_DF0_1		|   Br. if not.
		lea	AR_DF0_1,a0		| Get our own return address
		jra	a2@			|   and go feed watchdog.
AR_DF0_1:
		movel	#(CPU030_DISAB_CACHES+CPU030_FLUSH_CACHES),d0
		movec	d0,cacr			| Disable/flush all CPU caches.
		pmove	(L_DF_DisabAcu).L,tt0	| Disable both CPU ACUs.
		pmove	(L_DF_DisabAcu).L,tt1
|
|	Save off diag failure info to Boot NVRAM area.
|
		ASM_ADDR_BOOT_NVRAM (a0)	| Get address of Boot NVRAM area
		addqw	#(1),a0@(a1:L:1)	| Incr given NVRAM error count.
		movel	a3,a0@(BTNVR_MEMERR_ADDR) |Save failure address.
		movel	d1,a0@(BTNVR_MEMERR_XPCT) |Save expected data value.
		movel	d2,a0@(BTNVR_MEMERR_READ) |Save actual error data read.
		movew	d3,a0@(BTNVR_MEMERR_INDX) |Save failed diag test index.
|
|	Display error test index on debug LEDs, and blink this (forever).
|	(Registers a2 and d3 are already valid here.)
|
		moveal	d3,a3			| Save off debug LED pattern.
AR_DF0_2:
		movel	a3,d3			| Recover saved debug LED pattern.
		movel	#(LOER_STD_LED_FLASH),d2 |Set main LED pattern.
		moveq	#(-1),d1		| Set blink loop buzz constant.
		lea	AR_DF0_3,a0		| Just in case we return here.
		jra	AR_FailLeds		| Go blink LEDs (forever).
AR_DF0_3:
		jra	AR_DF0_2		| Flip debug LEDs and buzz more.

| ----------------------------------------------------------------------------
|	Assembly wrapper for LED blink routine for general errors, for C usage.
|		void A_FailLeds (	unsigned long	MainLEDs,
|					unsigned long	DebugLEDs,
|					unsigned long	BuzzLoopCount,
|					void		(* WdogFcn)()	)
| ----------------------------------------------------------------------------
		.globl	_A_FailLeds
_A_FailLeds:
		link	fp,#(0)			| Setup stack frame.
		moveml	d2-d3/a2,sp@-		| Save registers consumed.
		movel	fp@(0x08+0x00),d2	| Get main LED pattern.
		movel	fp@(0x08+0x04),d3	| Get debug LED pattern.
		movel	fp@(0x08+0x08),d1	| Get buzz loop constant.
		moveal	fp@(0x08+0x0c),a2	| Get watchdog service function.
		lea	L_A_FL_1,a0		| Caller return address.
		jra	AR_FailLeds		| Do diag failure processing.
L_A_FL_1:					|   (Should NOT return !)
		moveml	sp@+,d2-d3/a2		| Restore registers consumed.
		unlk	fp			| Release stack frame.
		rts				| Return to caller.

| ----------------------------------------------------------------------------
|	Blink LEDs for general failures.  This routine uses register linkage
|	and can only be called by assembly.  It is meant to be called when
|	DRAM and UART fail diags or have not been checked, and when no stack
|	is available.
|
|	The debug LEDs blink with the indicated diag/error code.  The
|	main (status) LEDs also blink in a repeated pattern.  The blink
|	period is controlled by a buzz-loop constant.
|
|	This routine does NOT return; it is expected that a reset will
|	be used to regain control after entering this routine.  We service
|	the watchdog here to avoid watchdog-driven resets.
|
|	Register usage:	a2 (in)	- addr watchdog service routine
|			a0 (in)	- scratch, caller return address.
|			d3 (in)	- debug LED pattern
|			d2 (in)	- main LED pattern
|			d1 (in)	- buzz loop constant (length)
|			d0	- scratch
| ----------------------------------------------------------------------------
		.globl	AR_FailLeds
AR_FailLeds:
		movel	#(CPU030_DISAB_CACHES+CPU030_FLUSH_CACHES),d0
		movec	d0,cacr			| Disable/flush all CPU caches.
		pmove	L_DF_DisabAcu,tt0	| Disable both CPU ACUs.
		pmove	L_DF_DisabAcu,tt1
		notb	d3			| Debug LEDs are active low.
		andil	#(0x0000000f),d3	| Isolate debug LED bits.
		oril	#(0x00ff0000),d3	| Set mask to darken all debug LEDs.
#ifndef	ACP_REV1_HW
		movel	&((ACP_PMD_LEVL_LED_YEL<<16)+ACP_PMD_STAT_LED_RED),d2
						| Setup main LED display.
#endif

|
|	Service watchdog and then display debug and main LEDs.  Blink
|	the debug LEDs (forever) while alternately flashing the main LEDs.
|
L_AR_FL_Top:
		tstl	a2			| Is there a wdog svc routine ?
		jeq	L_AR_FL_Flip		|   Br. if not.
		lea	L_AR_FL_Flip,a0		| Get our own return address
		jra	a2@			|   and go feed watchdog.
L_AR_FL_Flip:
		ASM_ADDR_HW_REGS (a0)		| Get base addr for HW regs.
		ASM_DBG_WRREG (d3,a0)		| Write to debug LEDs then
		swap	d3			|   flip between blank/active.
#ifdef	ACP_REV1_HW
		movel	d2,a0@(ASM_HW_WREG_RED_ST_LED)
		eoril	&(0x01010101),d2	| Write then flip all main LEDs.
#else
		movew	d2,a0@(ACP_WRRG_PMD_STAT_LED)
						| Set status LED red or dark.
#endif
		movel	d1,d0			| Setup loop count for buzzing.
L_AR_FL_Buzz:
		nop				| Wait a bit ...
		dbf	d0,L_AR_FL_Buzz		| Spend some time ...

#if	0
		jra	_A_ResetCPU		| Force a reset.
#else
		jra	L_AR_FL_Top		| Service watchdog and flip LEDs.
#endif

| ----------------------------------------------------------------------------
|	Used to clear/disable 68ec030 ACU or 68030 MMU/TT registers.
|	Leave this in text section to be accessible when data section
|	has not yet been copied.
| ----------------------------------------------------------------------------

L_DF_DisabAcu:
		.long	(CPU030_DISAB_ACU)
					| Value to disable CPU ACU.

| ----------------------------------------------------------------------------
|	Used to force full board-level reset.  CPU starts back up via
|	Reset Vector in Primary Boot.
|
|	Sample C usage:
|		void A_ResetCPU () ;
| ----------------------------------------------------------------------------
		.globl	_A_ResetCPU
_A_ResetCPU:
		moveb	#(ON),(ACP_HW_IOREG_BASE+ACP_WRRG_SW_FORCE_RESET)
						| Force full board reset.
L_RCPU_0:					| If reset lines are broken,
		jra	_A_ResetCPU		|   let watchdog take us out.

| $Id: bt_powdiag.S,v 1.1.4.1 1996/05/09 15:00:00 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/bt_powdiag.S,v $
|-----------------------------------------------------------------
| bt_powdiag.S -- PROM Boot low-level power-up diags.
|		These diag routines are used at early start-up, before
|		a stack is available.  The diags contained here are:
|		LEDs blink, NVRAM, Boot code checksum, DRAM tests,
|		basic MFP test (MFP device access, not interrupt/timer/UART
|		tests).
|
|		Inspired by similar Catalyst 1000/1200 code.
|
|		This file should be portable to any other 68k platform
|		with minimal work.
|
| November 23, 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: bt_powdiag.S,v $
| Revision 1.1.4.1  1996/05/09  15:00:00  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:56  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:41:05  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:12:05  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:29  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:43:09  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.7.6.1  1996/01/18  05:02:13  rlowe
| Manufacturability changes to bootrom and flash boot code.
|
| Branch: ATM21_branch
|
| Revision 1.7  1995/10/20  05:04:43  rlowe
| Fixes for early LCP reset release, so that ACP does not generate
| unintended interrupts to the LCP.  Also clean up any stray LCP
| RxDone interrupts to the ACP once the Module Ready startup message
| is found.  Fix the LCP driver behaviour if HW guard timeout occurs
| during Tx to LCP.
|
| Revision 1.6  1995/10/14  08:46:50  rlowe
| Fixes to allow NMP/SCP manufacturing diags to be run on system powerup.
|
| Revision 1.5  1995/10/13  06:06:42  rlowe
| Enable and use BCS checksums in PROM and flash Boots.
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "boot_defs.h"
#include "bt_nvram.h"
#include "lo_error.h"


#define	btm_PUD_DISPATCH(name,targ,retn)	\
		lea	name,a1			/* Name string */ ; \
		lea	targ,a2			/* Target addr */ ; \
		lea	retn,a0			/* Return addr */ ; \
		jra	bt_PowDispatch		/* Get to it   */


| ----------------------------------------------------------------------------
|	Drive early power-up (non-C) diags for the target board.
|	These include LED checks, Boot code checksums, DRAM diags,
|	MFP/UART diags.  Power-up diags are skipped if the FASTBOOT
|	jumper is installed.  Initialize MFP/UART first off so that
|	operator can follow power-up progress.
|	Call various subroutines using register linkage (no stack).
| ----------------------------------------------------------------------------
		.text
		.globl	AR_PowerUpDiags
AR_PowerUpDiags:
		lea	L_bt_PD_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PD_0:
		jra	bt_PowMfpSetup		| Setup console MFP/UART.
bt_PD_MfpSDone:

|
|	Do LCP DPRAM diags.  In any case, once done ensure LCP release
|	from reset to open NMPs timeout window for NMP-driven 8051 diags.
|	Note that DPRAM is NOT zeroed out in FASTBOOT by-pass case.
|
		btm_PUD_DISPATCH (0, bt_PowDPRamChk, bt_LDPR_skip)
bt_PD_DPRDone:

|
|	Do power-up MFP/UART diags.
|
		btm_PUD_DISPATCH (bts_PUD_MFP, bt_PowMfpDiags, bt_PD_MfpDDone)
bt_PD_MfpDDone:

|
|	Do power-up LED checks.
|
		btm_PUD_DISPATCH (bts_PUD_Leds, bt_PowLedChecks, bt_PD_LedsDone)
bt_PD_LedsDone:

|
|	Do power-up NVRAM diags.
|
		btm_PUD_DISPATCH (bts_PUD_NVR, bt_PowNVRamChk, bt_PD_NVRamDone)
bt_PD_NVRamDone:

|
|	Do power-up DRAM diags.
|
		btm_PUD_DISPATCH (bts_PUD_Dram, bt_PowDramDiags, bt_PD_DramDone)
bt_PD_DramDone:

|
|	Tidy up and complete console output and return to our caller.
|	The C code we will run shortly will reset the MFP/UART.
|
		jra	bt_PowTxFinish
bt_PD_AllDone:
		lea	__Boot_main_1,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.

| ----------------------------------------------------------------------------
|	Drive initial setup for MFP/UART console.
|	Call various subroutines using register linkage (no stack).
|	Assume that subroutines will service watchdog as required.
|	Do this regardless of any jumper settings so that trace output
|	will appear on the console.  Note that MFP diags that follow
|	will be done on a LIVE MFP.
|	Defer greetings until AFTER LCP is initialized.
| ----------------------------------------------------------------------------
		.globl	bt_PowMfpSetup
bt_PowMfpSetup:
		lea	L_bt_MFP_S_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	Reset and initialize MFP.  Defer greeting to operator until after
|	LCP init is done.
|
L_bt_MFP_S_0:
		moveq	&(1),d0			| Indicate MFP should be reset.
		lea	L_bt_MFP_S_1,a0		| Get return address.
		jra	__MFP_Init		| Go initialize MFP.
L_bt_MFP_S_1:
		lea	bt_PD_MfpSDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.

| ----------------------------------------------------------------------------
|	Inform operator of FASTBOOT jumper state.  ACP jumpers are active LOW.
|	Also give operator a greeting statement.
|		Consumes: a1,a0,d0.
|		Transitively consumes: a5,a2,a1,d2,d0.
| ----------------------------------------------------------------------------
		.globl	bt_PowFBJmpInfo
bt_PowFBJmpInfo:
		lea	L_bt_FBJI_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_FBJI_0:
|
|	Note that the build info date string is from bt_buildinfo.c.
|
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_FBJI_1,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_1:
		lea	bts_PUD_Greet,a1	| Get address of output string.
		lea	L_bt_FBJI_2,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_2:
		lea	_BT_BuildInfo,a1	| Get address of output string.
		lea	L_bt_FBJI_3,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_3:
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_FBJI_4,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_4:
		lea	bts_PUD_ConsSetup,a1	| Get address of output string.
		lea	L_bt_FBJI_5,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_5:
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_FBJI_6,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_6:
		lea	bts_PUD_FBJmp,a1	| Get address of output string.
		lea	L_bt_FBJI_7,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_FBJI_7:
		ASM_ADDR_HW_REGS (a0)		| Base address of HW IO regs.
		ASM_DBG_RDREG (a0,d0)		| Read HW "fastboot" jumper
		ASM_CHKJPR_FASTBT (d0)		|   and check if set (low).
		jeq	L_bt_FBJI_8		| Br. if "fastboot" set.
		
		lea	_bts_PUD_Disabled,a1	| Get address of result string
		jra	L_bt_FBJI_9		|   and go print it.
L_bt_FBJI_8:
		lea	_bts_PUD_Enabled,a1	| Get address of result string.
L_bt_FBJI_9:
		lea	L_bt_FBJI_a,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_FBJI_a:
		lea	bt_PD_FBJIDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.

| ----------------------------------------------------------------------------
|	Dispatch to various low-level diags routines.  Print name of
|	diag to console if appropriate, and indicate bypass if FASTBOOT
|	jumper is set.
|		Inputs:	a0.l	- caller return address (for bypass)
|			a1.l	- diag name string address
|			a2.l	- target diag function address
|		Consumes: a1,a0,d5,d4,d3,d0.
|		Transitively consumes: a5,a2,a1,d2,d0.
| ----------------------------------------------------------------------------
		.globl	bt_PowDispatch
bt_PowDispatch:
		movel	a0,d3			| Save caller return address.
		movel	a1,d4			| Save diag name string.
		movel	a2,d5			| Save target diag address.

		lea	L_bt_PDS_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDS_0:
		tstl	d4			| Got a null ptr for name str ?
		jeq	L_bt_PDS_3		|   -- br. if so.

		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_PDS_1,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDS_1:
		moveal	d4,a1			| Restore addr of name string.
		lea	L_bt_PDS_2,a0		| Get our return address.
		jra	__MFP_puts		| Print name string to user.
L_bt_PDS_2:
		lea	L_bt_PDS_3,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDS_3:
		ASM_ADDR_HW_REGS (a0)		| Base address of HW IO regs.
		ASM_DBG_RDREG (a0,d0)		| Read HW "fastboot" jumper
		ASM_CHKJPR_FASTBT (d0)		|   and check if set (low).
		jeq	L_bt_PDS_4		| Br. if "fastboot" set.

		moveal	d5,a0			| Recover target diag address
		jra	a0@			|   and get to it.
L_bt_PDS_4:
		tstl	d4			| Got a null ptr for name str ?
		jeq	L_bt_PDS_5		|   -- br. if so.

		lea	_bts_PUD_Bypass,a1	| Get address of result string.
		lea	L_bt_PDS_5,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PDS_5:
		moveal	d3,a0			| Recover caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.

| ----------------------------------------------------------------------------
|	Inform operator we are draining UART Tx output.  This is done
|	because the C code will reset the MFP, so we should clean up here.
|		Consumes: a1,a0.
|		Transitively consumes: a5,a2,a1,d2,d0.
| ----------------------------------------------------------------------------
		.globl	bt_PowTxFinish
bt_PowTxFinish:
		lea	L_bt_PTF_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PTF_0:
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_PTF_1,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PTF_1:
		lea	bts_PUD_DrainTx,a1	| Get address of output string.
		lea	L_bt_PTF_2,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PTF_2:
		lea	_bts_PUD_NewLine0,a1	| Get address of output string.
		lea	L_bt_PTF_3,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PTF_3:
		lea	L_bt_PTF_4,a0		| Get our return address.
		jra	__MFP_WaitTxDone	| Wait for UART Tx to finish.
L_bt_PTF_4:
		lea	bt_PD_AllDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.

| ----------------------------------------------------------------------------
|	Centralize failure handling for any of our low-level memory diags.
|	Inform operator of situation and then hit low-level failure
|	routine to light red status led and halt further Boot activity.
|		Inputs:	a3.l	- failure access address
|			a4.l	- ptr to name of failed test
|			d1.l	- data written and expected back
|			d2.l	- actual (error) data read back
|			d4.l	- index for failed functional area
|			d5.l	- nvram boot structure offset to failure count
|		Consumes: a5-a0,d7-d0.
|		Transitively consumes: a5,a2,a1,a0,d3,d2,d1,d0.
| ----------------------------------------------------------------------------
		.globl	bt_PowDiagFail
bt_PowDiagFail:
		movel	d2,d7			| Save test data read back.
		movel	d1,d6			| Save test data written.
		lea	L_bt_PDF_a,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDF_a:
		lea	_bts_PUD_Failed,a1	| Get address of result string.
		lea	L_bt_PDF_b,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PDF_b:
		lea	_bts_PUD_Err_at,a1	| Get address of output string.
		lea	L_bt_PDF_c,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_c:
		movel	a3,d0			| Get failure address.
		lea	L_bt_PDF_d,a0		| Get our return address.
		jra	__MFP_WriteHex		| Print output value to user.
L_bt_PDF_d:
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_PDF_e,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_e:
		moveal	a4,a1			| Get address of result string.
		lea	L_bt_PDF_f,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PDF_f:
		lea	_bts_PUD_Err_wr,a1	| Get address of output string.
		lea	L_bt_PDF_g,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_g:
		movel	d6,d0			| Get value written.
		lea	L_bt_PDF_h,a0		| Get our return address.
		jra	__MFP_WriteHex		| Print output value to user.
L_bt_PDF_h:
		lea	_bts_PUD_Err_rd,a1	| Get address of output string.
		lea	L_bt_PDF_i,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_i:
		movel	d7,d0			| Get value read back.
		lea	L_bt_PDF_j,a0		| Get our return address.
		jra	__MFP_WriteHex		| Print output value to user.
L_bt_PDF_j:
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_PDF_k,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_k:
		lea	_bts_PUD_Hangup,a1	| Get address of output string.
		lea	L_bt_PDF_l,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_l:
		movel	d4,d0			| Get error test index.
		lea	L_bt_PDF_m,a0		| Get our return address.
		jra	__MFP_WriteHex		| Print output value to user.
L_bt_PDF_m:
		lea	_bts_PUD_NewLine0,a1	| Get address of output string.
		lea	L_bt_PDF_n,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_n:
		lea	_bts_PUD_NewLine0,a1	| Get address of output string.
		lea	L_bt_PDF_o,a0		| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDF_o:
		lea	L_bt_PDF_end,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDF_end:
		movel	d7,d2			| Restore test data read back.
		movel	d6,d1			| Restore test data written.
		movel	d4,d0			| Get index of failed test.
		moveal	d5,a1			| NVRAM offset to error count.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		lea	L_bt_PDF_end,a0		| Caller return address (unused)
		jra	AR_DiagFail0		| Use standard failure routine.

| ----------------------------------------------------------------------------
|	Drive power-up LCP DPRAM checks.
|	These must occur early in the Boot sequence to allow LCP to
|	interact with NMP, especially for manufacturing diags.  Also
|	zero out all DPRAM when done, and drop LCP from reset.
|	Registers consumed:  d0-d7, a0-a6.
| ----------------------------------------------------------------------------
		.global	bt_PowDPRamChk
bt_PowDPRamChk:
		lea	L_bt_LDPR_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_LDPR_0:
		ASM_ADDR_STORE (ACP_HW_LCP_RAM_BASE, a1)
						| Get address of LCP DPRAM area.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_LDPR_1,a0		| Set caller return address and
		jra	ARMD_DataBusB		|   do Data Bus (Byte) test.
L_bt_LDPR_1:
		lea	_bts_PUD_Err_DB,a4	| Name data bus test.
		tstl	d0			| Check test return code
		jne	L_bt_LDPR_fail		|   and branch on error.

		lea	L_bt_LDPR_2,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_LDPR_2:
		ASM_ADDR_STORE (ACP_HW_LCP_RAM_BASE, a1)
						| Get address of LCP DPRAM area.
		movel	&(ACP_HW_LCP_RAM_SIZE),d1
						| Get size of LCP DPRAM area.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_LDPR_3,a0		| Set caller return address and
		jra	ARMD_AddrBusB		|   do Addr Bus (Byte) test.
L_bt_LDPR_3:
		lea	_bts_PUD_Err_AB,a4	| Name addr bus test.
		tstl	d0			| Check test return code
		jne	L_bt_LDPR_fail		|   and branch on error.

		lea	L_bt_LDPR_4,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_LDPR_4:
		ASM_ADDR_STORE (ACP_HW_LCP_RAM_BASE, a1)
						| Get address of LCP DPRAM area.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_LDPR_5,a0		| Set caller return address and
		jra	ARMD_ByteWordEnb	|   do Byte/Word Enable test.
L_bt_LDPR_5:
		lea	_bts_PUD_Err_DW,a4	| Name data width test.
		tstl	d0			| Check test return code
		jne	L_bt_LDPR_fail		|   and branch on error.

		lea	L_bt_LDPR_6,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_LDPR_6:
		ASM_ADDR_STORE (ACP_HW_LCP_RAM_BASE, a1)
						| Get address of LCP DPRAM area.
		movel	&(ACP_HW_LCP_RAM_SIZE),d1
						| Get size of LCP DPRAM area.
		movel	&(BOOT_POWDIAG_PTRN_1),d2 |Setup test pattern.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_LDPR_7,a0		| Set caller return address and
		jra	ARMD_RW_Pattern		|   do Pattern Read/Write test.
L_bt_LDPR_7:
		lea	_bts_PUD_Err_P1,a4	| Name pattern-1 test.
		tstl	d0			| Check test return code
		jne	L_bt_LDPR_fail		|   and branch on error.

		lea	L_bt_LDPR_8,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_LDPR_8:
		ASM_ADDR_STORE (ACP_HW_LCP_RAM_BASE, a1)
						| Get address of LCP DPRAM area.
		movel	&(ACP_HW_LCP_RAM_SIZE),d1
						| Get size of LCP DPRAM area.
		movel	&(BOOT_POWDIAG_PTRN_2),d2 |Setup test pattern.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_LDPR_9,a0		| Set caller return address and
		jra	ARMD_RW_Pattern		|   do Pattern Read/Write test.
L_bt_LDPR_9:
		lea	_bts_PUD_Err_P2,a4	| Name pattern-2 test.
		tstl	d0			| Check test return code
		jne	L_bt_LDPR_fail		|   and branch on error.

		lea	L_bt_LDPR_pass,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	LCP DPRAM diag results known, zero out DPRAM and enable LCP.
|	Do NOT zero out DPRAM in failure case.  Release LCP from reset
|	unless BOTH FASTBOOT and MANUFACTURING jumpers are set.
|
L_bt_LDPR_fail:
		moveal	&(-1),a6		| Set flag register for fail.
		movel	d1,d3			| Save off a failure result.
		movel	d2,d4			| Save off a failure result.
		jra	L_bt_LDPR_b		| Groom LCP.
bt_LDPR_skip:
		moveq	&(0),d0			| Clear scratch register.
		moveb	(ACP_HW_MFP_BASE+MFP_RO_GPDR),d0
						| Read MFP IO lines
		andb	&(MFP_GPI_MFGJMP_),d0	|   and isolate MFG jumper
		jeq	bt_LDPR_skip2		|   which is active LOW.

		moveal	&(1),a6			| Set flag register for skip
		jra	L_bt_LDPR_a		|   but run LCP.
bt_LDPR_skip2:
		moveal	&(2),a6			| Set flag register for skip
		jra	L_bt_LDPR_d		|   but hold LCP reset.
L_bt_LDPR_pass:
		moveal	&(0),a6			| Set flag register for pass.
L_bt_LDPR_a:
		ASM_ADDR_STORE (ACP_HW_LCP_RAM_BASE, a1)
						| Get address of LCP DPRAM area.
		movel	&(ACP_HW_LCP_RAM_SIZE),d1
						| Get size of LCP DPRAM area.
		moveq	&(0),d2			| Fill with zeros.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_LDPR_b,a0		| Set caller return address.
		jra	AR_BlockFill		| Go fill LCP DPRAM area.
L_bt_LDPR_b:
		lea	L_bt_LDPR_c,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_LDPR_c:
		moveq	&(1),d1			| Set flag to force inactive
						|   all interrupts to LCP.
						|   Ok since LCP still in reset.
		lea	L_bt_LDPR_d,a0		| Set caller return address
		jra	AR_LCP_Release		|   and go release LCP reset.
L_bt_LDPR_d:
		lea	bt_PDInfo_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	Report test results now that LCP may be running.  Also give operator
|	a basic greeting.  This should be the FIRST console output done
|	by this file, to ensure LCP is running ASAP.
|	To support failure case, do NOT stomp over d3/d4/a3/a4/a6.
|
bt_PDInfo_0:
		lea	_bts_PUD_NewLine0,a1	| Get address of output string.
		lea	L_bt_PDInfo_1,a0	| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDInfo_1:
		jra	bt_PowFBJmpInfo		| Report FASTBOOT jumper state.
bt_PD_FBJIDone:
		lea	_bts_PUD_NewLine,a1	| Get address of output string.
		lea	L_bt_PDInfo_2,a0	| Get our return address.
		jra	__MFP_puts		| Print output string to user.
L_bt_PDInfo_2:
		lea	bts_PUD_Dpram1,a1	| Get addr of name string.
		lea	L_bt_PDInfo_3,a0	| Get our return address.
		jra	__MFP_puts		| Print name string to user.
L_bt_PDInfo_3:
		cmpaw	&(2),a6			| Is this a FASTBOOT case
		jeq	L_bt_PDInfo_4		|    where we hold LCP reset ?

		lea	bts_PUD_LCPrun,a1	| Get address of result string
		jra	L_bt_PDInfo_5		|    and go print it.
L_bt_PDInfo_4:
		lea	bts_PUD_LCPrst,a1	| Get address of result string.
L_bt_PDInfo_5:
		lea	L_bt_PDInfo_6,a0	| Get our return address.
		jra	__MFP_puts		| Print name string to user.
L_bt_PDInfo_6:
		lea	bts_PUD_Dpram2,a1	| Get addr of name string.
		lea	L_bt_PDInfo_7,a0	| Get our return address.
		jra	__MFP_puts		| Print name string to user.
L_bt_PDInfo_7:
		lea	L_bt_PDInfo_8,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDInfo_8:
		tstl	a6			| Check test result flag.
		jne	L_bt_PDInfo_9		|   -- br. if NOT passed.

		lea	_bts_PUD_Passed,a1	| Get address of result string.
		lea	L_bt_PDInfo_a,a0	| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PDInfo_9:
		jlt	L_bt_LDPR_fail2

		lea	_bts_PUD_Bypass,a1	| Get address of result string.
		lea	L_bt_PDInfo_a,a0	| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PDInfo_a:
		lea	0,a6			| Zap result flag register.
		lea	bt_PD_DPRDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	Assume that failures set d1/d2/a3 appropriately.
|	(Data expected, error data read, error access address.)
|	Upon jumping here reg a4 has pointer to name of failed test.
|	Set our local test index and nvram structure offset, then go
|	to common failure handler.
|
L_bt_LDPR_fail2:
		lea	0,a6			| Zap result flag register.
		movel	d3,d1			| Recover a failure result.
		movel	d4,d2			| Recover a failure result.
		moveq	&(PUD_8051_RAM),d4	| Get index of failed test.
		movel	&(BTNVR_LCPF_COUNT),d5	| NVRAM offset to error count.
		jra	bt_PowDiagFail		| Use central failure routine.

| ----------------------------------------------------------------------------
|	Drive power-up time LED checks.
|	Assume that subroutines will service watchdog as required.
|	Note for spin loops that instruction cache may or may not be enabled.
|	Registers consumed:  a0/a1/d0.
| ----------------------------------------------------------------------------
		.globl	bt_PowLedChecks
bt_PowLedChecks:
		lea	L_bt_PLC_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PLC_0:
		ASM_ADDR_HW_REGS (a1)		| Get base addr of HW I/Os.
		movew	&(ACP_PMD_STAT_LED_OFF),a1@(ACP_WRRG_PMD_STAT_LED)
						| Status LED is dark.
		movew	&(ACP_PMD_LEVL_LED_YEL),a1@(ACP_WRRG_PMD_BOT_LED)
						| Bottom LED is yellow.
		movew	&(ACP_PMD_LEVL_LED_YEL),a1@(ACP_WRRG_PMD_MID_LED)
						| Middle LED is yellow.
		movew	&(ACP_PMD_LEVL_LED_YEL),a1@(ACP_WRRG_PMD_TOP_LED)
						| Top LED is yellow.
		moveq	&(4),d0			| Spin loop to use some time.
L_bt_PLC_Spin1a:
		swap	d0			| Recover counter LS word.
L_bt_PLC_Spin1b:
		dbf	d0,L_bt_PLC_Spin1b	| Inner spin loop.

		swap	d0			| Recover counter MS word.
		dbf	d0,L_bt_PLC_Spin1a	| Outer spin loop.

		lea	L_bt_PLC_1,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PLC_1:
		movew	&(ACP_PMD_STAT_LED_YEL),a1@(ACP_WRRG_PMD_STAT_LED)
						| Status LED is yellow.
		movew	&(ACP_PMD_LEVL_LED_OFF),a1@(ACP_WRRG_PMD_BOT_LED)
						| Bottom LED is dark.
		movew	&(ACP_PMD_LEVL_LED_OFF),a1@(ACP_WRRG_PMD_MID_LED)
						| Middle LED is dark.
		movew	&(ACP_PMD_LEVL_LED_OFF),a1@(ACP_WRRG_PMD_TOP_LED)
						| Top LED is dark.
		moveq	&(4),d0			| Spin loop to use some time.
L_bt_PLC_Spin2a:
		swap	d0			| Recover counter LS word.
L_bt_PLC_Spin2b:
		dbf	d0,L_bt_PLC_Spin2b	| Inner spin loop.

		swap	d0			| Recover counter MS word.
		dbf	d0,L_bt_PLC_Spin2a	| Outer spin loop.

		lea	L_bt_PLC_2,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PLC_2:
		movew	&(ACP_PMD_STAT_LED_YEL),a1@(ACP_WRRG_PMD_STAT_LED)
						| Status LED is yellow.
		movew	&(ACP_PMD_LEVL_LED_OFF),a1@(ACP_WRRG_PMD_BOT_LED)
						| Bottom LED is dark.
		movew	&(ACP_PMD_LEVL_LED_OFF),a1@(ACP_WRRG_PMD_MID_LED)
						| Middle LED is dark.
		movew	&(ACP_PMD_LEVL_LED_OFF),a1@(ACP_WRRG_PMD_TOP_LED)
						| Top LED is dark.
|
|	LED check passed, inform operator.
|
		lea	_bts_PUD_Passed,a1	| Get address of result string.
		lea	L_bt_PLC_3,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PLC_3:
		lea	bt_PD_LedsDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.

| ----------------------------------------------------------------------------
|	Drive NVRAM Boot area diagnostic and check.
|	Call various subroutines using register linkage (no stack).
|	Assume that subroutines will service watchdog as required.
|	Once power-up diags done, format Boot NVRAM area as required.
| ----------------------------------------------------------------------------
		.globl	bt_PowNVRamChk
bt_PowNVRamChk:
		lea	L_bt_PNVRC_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PNVRC_0:
		ASM_ADDR_BOOT_NVRAM (a1)	| Get address of Boot NVRAM area
		lea	a1@(BTNVR_DIAG_AREA),a1	|   and find diag field in it.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PNVRC_1,a0		| Set caller return address and
		jra	ARMD_DataBusB		|   do Data Bus (Byte) test.
L_bt_PNVRC_1:
		lea	_bts_PUD_Err_DB,a4	| Name data bus test.
		tstl	d0			| Check test return code
		jne	L_bt_PNVRC_fail		|   and branch on error.

		ASM_ADDR_BOOT_NVRAM (a1)	| Get address of Boot NVRAM area
		lea	a1@(BTNVR_DIAG_AREA),a1	|   and find diag field in it.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PNVRC_2,a0		| Set caller return address and
		jra	ARMD_ByteWordEnb	|   do Byte/Word Enable test.
L_bt_PNVRC_2:
		lea	_bts_PUD_Err_DW,a4	| Name data width test.
		tstl	d0			| Check test return code
		jne	L_bt_PNVRC_fail		|   and branch on error.

		ASM_ADDR_BOOT_NVRAM (a1)	| Get address of Boot NVRAM area
		lea	a1@(BTNVR_DIAG_AREA),a1	|   and find diag field in it.
		movel	&(BOOT_POWDIAG_PTRN_3),d2 |Setup 4*1-byte test patterns.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PNVRC_3,a0		| Set caller return address, do
		jra	ARMD_RW_Byte		|   Read/Write Byte Patrns test.
L_bt_PNVRC_3:
		lea	_bts_PUD_Err_P3,a4	| Name pattern-3 test.
		tstl	d0			| Check test return code
		jne	L_bt_PNVRC_fail		|   and branch on error.
|
|	NVRAM power-up diags done, format Boot NVRAM area as required.
|
		ASM_ADDR_BOOT_NVRAM (a1)	| Get address of Boot NVRAM area
		cmpil	&(BTNVR_VFLG_VALUE),a1@(BTNVR_VALID_FLAG)
						| Was it previously formatted ?
		jeq	L_bt_PNVRC_5		|   Br. if so.
						| Got Boot NVRAM addr in a1 now.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d2			| Fill with zeros.
		movel	&(BOOT_NVRAM_USED),d1	| Get size of Boot NVRAM area.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PNVRC_4,a0		| Get caller return address.
		jra	AR_BlockFill		| Go fill Boot NVRAM area.
|
|	Ordering of the next few statements is important when formatting NVRAM.
|	Want to write valid flag to NVRAM after all other Boot data is written.
|
L_bt_PNVRC_4:
		ASM_ADDR_BOOT_NVRAM (a1)	| Get address of Boot NVRAM area
		movew	&(BOOT_NVRAM_FMT_VER),a1@(BTNVR_BT_FMT_VER)
						| Set Boot NVRAM area version.
		movel	a1,a1@(BTNVR_PTR_SELF)	| Ptr to base Boot NVRAM area.
		movel	&(BTNVR_VFLG_VALUE),a1@(BTNVR_VALID_FLAG)
						| Set Boot NVRAM formatted flag.
						| Do last for two-phase commit.
|
|	Increment Boot reset count in NVRAM.
|
L_bt_PNVRC_5:
		addqw	&(1),a1@(BTNVR_RESET_COUNT)
						| Increment Boot reset count.
		lea	L_bt_PNVRC_6,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PNVRC_6:
		lea	_bts_PUD_Passed,a1	| Get address of result string.
		lea	L_bt_PNVRC_7,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PNVRC_7:
		lea	bt_PD_NVRamDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	Assume that failures set d1/d2/a3 appropriately.
|	(Data expected, error data read, error access address.)
|	Upon jumping here reg a4 has pointer to name of failed test.
|	Set our local test index and nvram structure offset, then go
|	to common failure handler.
|
L_bt_PNVRC_fail:
		ASM_ADDR_BOOT_NVRAM (a1)	| Get address of Boot NVRAM area
		addqw	&(1),a1@(BTNVR_RESET_COUNT)
						| Incr Boot NVRAM Reset Count.
		moveq	&(PUD_NVRAM),d4		| Get index of failed test.
		movel	&(BTNVR_NVRMF_COUNT),d5	| NVRAM offset to error count.
		jra	bt_PowDiagFail		| Use central failure routine.

| ----------------------------------------------------------------------------
|	Drive early power-up DRAM diags for the target board.
|	Call various subroutines using register linkage (no stack).
|	Assume that subroutines will service watchdog as required.
| ----------------------------------------------------------------------------
		.globl	bt_PowDramDiags
bt_PowDramDiags:
		lea	L_bt_PDD_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDD_0:
		lea	BOOT_DRAM_START,a1	| Addr DRAM start.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PDD_1,a0		| Set caller return address and
		jra	ARMD_DataBusL		|   do Data Bus (Long) test.
L_bt_PDD_1:
		lea	_bts_PUD_Err_DB,a4	| Name data bus test.
		tstl	d0			| Check test return code
		jne	L_bt_PDD_fail		|   and branch on error.

		lea	BOOT_DRAM_START,a1	| Addr DRAM start.
		movel	&(BOOT_DRAM_SIZE),d1	| Size of DRAM area to test.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PDD_2,a0		| Set caller return address and
		jra	ARMD_AddrBusL		|   do Addr Bus (Long) test.
L_bt_PDD_2:
		lea	_bts_PUD_Err_AB,a4	| Name addr bus test.
		tstl	d0			| Check test return code
		jne	L_bt_PDD_fail		|   and branch on error.

		lea	BOOT_DRAM_START,a1	| Addr DRAM start.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PDD_3,a0		| Set caller return address and
		jra	ARMD_ByteWordEnb	|   do Byte/Word Enable test.
L_bt_PDD_3:
		lea	_bts_PUD_Err_DW,a4	| Name data width test.
		tstl	d0			| Check test return code
		jne	L_bt_PDD_fail		|   and branch on error.

		lea	BOOT_DRAM_START,a1	| Addr DRAM start.
		movel	&(BOOT_DRAM_SIZE),d1	| Size of DRAM area to test.
		movel	&(BOOT_POWDIAG_PTRN_1),d2 |Setup test pattern.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PDD_4,a0		| Set caller return address and
		jra	ARMD_RW_Pattern		|   do Pattern Read/Write test.
L_bt_PDD_4:
		lea	_bts_PUD_Err_P1,a4	| Name pattern-1 test.
		tstl	d0			| Check test return code
		jne	L_bt_PDD_fail		|   and branch on error.

		lea	BOOT_DRAM_START,a1	| Addr DRAM start.
		movel	&(BOOT_DRAM_SIZE),d1	| Size of DRAM area to test.
		movel	&(BOOT_POWDIAG_PTRN_2),d2 |Setup test pattern.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_PDD_5,a0		| Set caller return address and
		jra	ARMD_RW_Pattern		|   do Pattern Read/Write test.
L_bt_PDD_5:
		lea	_bts_PUD_Err_P2,a4	| Name pattern-2 test.
		tstl	d0			| Check test return code
		jne	L_bt_PDD_fail		|   and branch on error.
|
|	DRAM diag passed, inform operator.
|
		lea	L_bt_PDD_6,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_PDD_6:
		lea	_bts_PUD_Passed,a1	| Get address of result string.
		lea	L_bt_PDD_7,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_PDD_7:
		lea	bt_PD_DramDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	Assume that failures set d1/d2/a3 appropriately.
|	(Data expected, error data read, error access address.)
|	Upon jumping here reg a4 has pointer to name of failed test.
|	Set our local test index and nvram structure offset, then go
|	to common failure handler.
|
L_bt_PDD_fail:
		moveq	&(PUD_DRAM),d4		| Get index of failed test.
		movel	&(BTNVR_DRAMF_COUNT),d5	| NVRAM offset to error count.
		jra	bt_PowDiagFail		| Use central failure routine.

| ----------------------------------------------------------------------------
|	Drive power-up MFP/UART diags.
|	Call various subroutines using register linkage (no stack).
|	Assume that subroutines will service watchdog as required.
|	Do not clobber UART functionality of MFP so console will work.
| ----------------------------------------------------------------------------
		.globl	bt_PowMfpDiags
bt_PowMfpDiags:
		lea	L_bt_MFP_0,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
L_bt_MFP_0:
		ASM_ADDR_HW_REGS (a0)		| Get base addr of HW I/O regs.
		ASM_SET_HWREG (a0, ACP_WRRG_MFP_RESET_)
						| Ensure MFP/UART out of reset.
		ASM_ADDR_MFP (a1)		| Get address of MFP and
		ASM_CLR_HWREG (a1, MFP_RO_IERA)	|   ensure that all interrupt
		ASM_CLR_HWREG (a1, MFP_RO_IERB)	|   sources are disabled.

		lea	a1@(MFP_RO_IMRA),a1	| Test uses two consecutive
						|   bytes @ Intr Mask Reg A/B.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_MFP_1,a0		| Set caller return address and
		jra	ARMD_DataBusB		|   do Data Bus (Byte) test.
L_bt_MFP_1:
		ASM_ADDR_MFP (a1)		| Get address of MFP and
		ASM_CLR_HWREG (a1, MFP_RO_IMRA)	|   ensure that all interrupt
		ASM_CLR_HWREG (a1, MFP_RO_IMRB)	|   masks are disabled.

		lea	_bts_PUD_Err_DB,a4	| Name data bus test.
		tstl	d0			| Check test return code
		jne	L_bt_MFP_fail		|   and branch on error.

		lea	a1@(MFP_RO_IMRB),a1	| Addr of MFP Intr Mask Reg B.
		movel	&(BOOT_POWDIAG_PTRN_3),d2 |Setup 4*1-byte test patterns.
		lea	AR_SvcWatchdog,a2	| Addr watchdog service routine.
		moveq	&(0),d0			| Clear return code register.
		lea	L_bt_MFP_2,a0		| Set caller return address, do
		jra	ARMD_RW_Byte		|   Read/Write Byte Patrns test.
L_bt_MFP_2:
		ASM_ADDR_MFP (a1)		| Get address of MFP, ensure
		ASM_CLR_HWREG (a1, MFP_RO_IMRB)	|   interrupt mask B disabled.

		lea	_bts_PUD_Err_P3,a4	| Name pattern-3 test.
		tstl	d0			| Check test return code
		jne	L_bt_MFP_fail		|   and branch on error.

		lea	L_bt_MFP_3,a0		| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	MFP diags passed, assume UART still usable for console output.
|
L_bt_MFP_3:
		lea	_bts_PUD_Passed,a1	| Get address of result string.
		lea	L_bt_MFP_4,a0		| Get our return address.
		jra	__MFP_puts		| Print result string to user.
L_bt_MFP_4:
		lea	bt_PD_MfpDDone,a0	| Set caller return address
		jra	AR_SvcWatchdog		|   and go service watchdog.
|
|	Assume that failures set d1/d2/a3 appropriately.
|	(Data expected, error data read, error access address.)
|	Upon jumping here reg a4 has pointer to name of failed test.
|	Set our local test index and nvram structure offset, then go
|	to common failure handler.
|
L_bt_MFP_fail:
		moveq	&(PUD_MFP),d4		| Get index of failed test.
		movel	&(BTNVR_MFPF_COUNT),d5	| NVRAM offset to error count.
		jra	bt_PowDiagFail		| Use central failure routine.

| ----------------------------------------------------------------------------
|	Various output strings used by diags above.  Have them available
|	to the link map for ease of debugging reference.
| ----------------------------------------------------------------------------
		.globl	_bts_PUD_NewLine0, _bts_PUD_NewLine, _bts_PUD_Passed
		.globl	_bts_PUD_Failed, _bts_PUD_Bypass, _bts_PUD_Enabled
		.globl	_bts_PUD_Disabled, _bts_PUD_Hangup, _bts_PUD_Err_Unk
		.globl	_bts_PUD_Err_DB, _bts_PUD_Err_AB, _bts_PUD_Err_DW
		.globl	_bts_PUD_Err_P1, _bts_PUD_Err_P2, _bts_PUD_Err_P3
		.globl	_bts_PUD_Err_at, _bts_PUD_Err_wr, _bts_PUD_Err_rd

		.text			| Following globals known to C code.
_bts_PUD_NewLine0:
		.asciz	"\r\n"
_bts_PUD_NewLine:
		.asciz	"\r\nACP-ROM: "
_bts_PUD_Passed:
		.asciz	" passed"
_bts_PUD_Failed:
		.asciz	" FAILED"
_bts_PUD_Bypass:
		.asciz	" skipped"
_bts_PUD_Enabled:
		.asciz	" ENABLED"
_bts_PUD_Disabled:
		.asciz	" disabled"
_bts_PUD_Hangup:
		.asciz	"Hanging CPU, test error index "
_bts_PUD_Err_Unk:
		.asciz	"unspecified"
_bts_PUD_Err_DB:
		.asciz	"data bus"
_bts_PUD_Err_AB:
		.asciz	"addr bus"
_bts_PUD_Err_DW:
		.asciz	"data width"
_bts_PUD_Err_P1:
		.asciz	"pattern-55555555"
_bts_PUD_Err_P2:
		.asciz	"pattern-AAAAAAAA"
_bts_PUD_Err_P3:
		.asciz	"pattern-00553377"
_bts_PUD_Err_at:
		.asciz	" at $"
_bts_PUD_Err_wr:
		.asciz	" test wrote $"
_bts_PUD_Err_rd:
		.asciz	", read $"


		.globl	bts_PUD_Greet, bts_PUD_FBJmp, bts_PUD_DrainTx
		.globl	bts_PUD_ConsSetup, bts_PUD_MFP, bts_PUD_Dpram1
		.globl	bts_PUD_Dpram2, bts_PUD_LCPrun, bts_PUD_LCPrst
		.globl	bts_PUD_Leds, bts_PUD_NVR, bts_PUD_Dram

		.text			| Following globals only for link map.
bts_PUD_Greet:
		.asciz	"ACP ROM Boot of "
bts_PUD_FBJmp:
		.asciz	"FASTBOOT jumper"
bts_PUD_DrainTx:
		.asciz	"Draining console UART Tx"
bts_PUD_ConsSetup:
		.asciz	"Console initialized"
bts_PUD_MFP:
		.asciz	"MFP"
bts_PUD_Dpram1:
		.asciz	"LCP "
bts_PUD_Dpram2:
		.asciz	", DPRAM"
bts_PUD_LCPrun:
		.asciz	"running"
bts_PUD_LCPrst:
		.asciz	"RESET"
bts_PUD_Leds:
		.asciz	"LEDs"
bts_PUD_NVR:
		.asciz	"NVRAM"
bts_PUD_Dram:
		.asciz	"DRAM"


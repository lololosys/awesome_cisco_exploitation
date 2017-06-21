| $Id: lcp_low.S,v 1.1.4.1 1996/05/09 14:54:57 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/lcp_low.S,v $
| ----------------------------------------------------------------------------
| lcp_low.S -- Low-level routines for the LCP driver.
|		It is assumed that the MFP has already been initialized.
|		Uses standard 68K C register conventions (save d0-d1/a0-a1).
|
| March 15, 1995, Rick Lowe
|
| Copyright (c) 1995-1996 by cisco Systems, Inc.
| All rights reserved.
| ----------------------------------------------------------------------------
| $Log: lcp_low.S,v $
| Revision 1.1.4.1  1996/05/09  14:54:57  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:14:24  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:39:10  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:10:28  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:41:23  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:39:52  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.2  1995/10/20  05:04:48  rlowe
| Fixes for early LCP reset release, so that ACP does not generate
| unintended interrupts to the LCP.  Also clean up any stray LCP
| RxDone interrupts to the ACP once the Module Ready startup message
| is found.  Fix the LCP driver behaviour if HW guard timeout occurs
| during Tx to LCP.
|
| Revision 1.1  1995/03/15  10:38:50  rlowe
| Created LCP driver.
|
| ----------------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "acp_hw_defs.h"
#include "boot_defs.h"

		.text
		.align	1			| Align to even address.

| ----------------------------------------------------------------------------
|	These routines are entry points for LCP (8051) interrupts.
|	They come in to the ACP through the MFP.
| ----------------------------------------------------------------------------
		.text
		.globl	_A_LCP_Rx_Isr
_A_LCP_Rx_Isr:
		ASM_CALL_C_ROUTINE (_LCP_Rx_Isr)
		rte

		.globl	_A_LCP_Tx_Isr
_A_LCP_Tx_Isr:
		ASM_CALL_C_ROUTINE (_LCP_Tx_Isr)
		rte

| ----------------------------------------------------------------------------
|	Assembly wrapper for LCP Release from reset, for C usage.
|		void A_LCP_Release (unsigned long fCleanIntrsToLCP)
| ----------------------------------------------------------------------------
		.text
		.globl	_A_LCP_Release
_A_LCP_Release:
		link	fp,&(0)			| Setup stack frame.
		movel	fp@(0x08+0x00),d1	| Get flag parameter from stack.

		lea	L_A_LCP_RelDone,a0	| Set our caller return
		jra	AR_LCP_Release		|   address and go to it.
L_A_LCP_RelDone:
		unlk	fp			| Pop stack frame.
		rts				| Return to caller.

| ----------------------------------------------------------------------------
|	LCP Release from reset.
|	Routine to drop LCP from reset.  Groom any related HW lines.
|	This gives a single place where Boots, etc. can come.
|	Uses register linkage, NOT meant to be called from C code.
|		Regs:	a0 - return address
|			d0 - scratch
|			d1 - flag non-zero to groom HW lines
| ----------------------------------------------------------------------------
		.global	AR_LCP_Release
AR_LCP_Release:
		movel	a1,d0			| Save a1 which is used
		ASM_ADDR_HW_REGS (a1)		|   as ptr to HW I/Os.
		tstl	d1			| Check flag to groom
		jeq	L_AR_LCP_Rel1		|   extra HW lines.

|	Force inactive all ACP interrupt lines TO LCP.  This can ONLY
|	be done safely while the LCP is in reset, since the HW reset
|	state of these lines is active-low.  Making them inactive-high
|	causes an edge that the LCP sees, so we can only do this when
|	LCP itself is in reset.

#ifdef	ACP_REV0_LCP
		ASM_CLR_HWREG (a1,ACP_WRRG_RD_FR_LCP_)
		ASM_CLR_HWREG (a1,ACP_WRRG_TP_TO_LCP_)
#else
		ASM_SET_HWREG (a1,ACP_WRRG_RD_FR_LCP_)
		ASM_SET_HWREG (a1,ACP_WRRG_TP_TO_LCP_)
#endif

L_AR_LCP_Rel1:					| Drop LCP from HW reset.
		ASM_SET_HWREG (a1,ACP_WRRG_LCP_RESET_)
		moveal	d0,a1			| Restore saved a1.
		jra	a0@			| Return to caller.


| $Id: bt_mfp.S,v 1.1.4.1 1996/05/09 14:54:49 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bt_mfp.S,v $
|-----------------------------------------------------------------
| bt_mfp.S -- ACP low-level 68901 MFP/UART polling driver.
|
|		This MFP polling driver avoids making any scratch RAM accesses.
|		Used by low-level Boot for board bring-up or diags, where we
|		cannot assume that anything other than CPU/MFP works.
|		ie. No stacks, buffers, etc.
|
|		Routines are provided for initializing the MFP, reading and
|		writing the MFP UART.
|
| November 2, 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: bt_mfp.S,v $
| Revision 1.1.4.1  1996/05/09  14:54:49  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:14:17  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:39:02  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:10:19  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:41:16  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:39:45  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.5.8.1  1996/01/18  05:02:22  rlowe
| Manufacturability changes to bootrom and flash boot code.
|
| Branch: ATM21_branch
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "boot_defs.h"

#define	ASM_RTN_CALLER(how)	jra how

MFP_DIGIT_0 = '0'
MFP_DIGIT_a = 'a'


| ----------------------------------------------------------------------------
|	CPU register usage conventions:
|		a7 (isp, sp)	- CPU stack pointer
|		a6 (fp)		- stack frame pointer (unused here)
|		a5		- pointer to board HW I/O registers
|		a1, a0		- scratch (also consumed by C calls)
|
|		d1, d0		- scratch (also consumed by C calls)
|	RAM is not used, so no stack, so registers contents destroyed on use.
| ----------------------------------------------------------------------------

| ----------------------------------------------------------------------------
|	Initialize MFP.
|		Inputs:	a0.l	- caller return address.
|			d0.l	- non-zero (T) to reset MFP.
|		Consumes: a5,a1,d0.
| ----------------------------------------------------------------------------
		.text
		.globl	__MFP_Init		| Put this into link map.
__MFP_Init:
		ASM_ADDR_HW_REGS (a5)		| Base addr of HW IO registers.
|
|	Reset the MFP ?  If so, use a SW timing loop to wait for the MFP
|	reset to take and the MFP to settle.
|
		tstl	d0			| Decide to reset MFP ?
		jeq	L_MFPI_SetRegs		|   Br. if not.

		ASM_CLR_HWREG (a5,ASM_HW_WREG_MFP_RESET)
						| Activate MFP reset register.
		moveq	&(((2*BOOT_HW_CPU_CLK_MHZ)/(2+6))+1),d0
						| Get SW timing loop counter.
L_MFPI_RstLoop:					| Hold MFP reset >= 2 usec.
		nop				| Uses 2 clocks minimum.
		dbf	d0,L_MFPI_RstLoop	| Uses 6 clocks minimum.
|
|	Ensure MFP not reset, then touch any MFP registers than can be
|	cleared by reading.  Do NOT zero out all registers; that should
|	be done by requesting MFP reset (above).
|
L_MFPI_SetRegs:
		ASM_SET_HWREG (a5,ASM_HW_WREG_MFP_RESET)
		nop				| De-activate MFP reset reg.

		ASM_ADDR_MFP (a1)		| Get MFP base address.
		ASM_READ_HWREG (a1,MFP_RO_GPDR,d0)
		ASM_READ_HWREG (a1,MFP_RO_TSR,d0)
		ASM_READ_HWREG (a1,MFP_RO_RSR,d0)
		ASM_READ_HWREG (a1,MFP_RO_UDR,d0)
		ASM_READ_HWREG (a1,MFP_RO_IPRA,d0)
		ASM_READ_HWREG (a1,MFP_RO_IPRB,d0)
|
| Setup MFP VR, TCDCR, TDDR, UCR, TSR, RSR.  Use 9600 baud, 8 bit data, 1 stop,
| divide input clock by 16 (for async).  When programming baud rate clock, note
| prescaler (4) and that clock output is toggled every countdown period.  Thus
| countdown period is one-HALF the desired full clock period.
|
		ASM_WRIT_HWREG (&(MFP_VECREG_SET(MFP_BASE_VECTOR,0)),a1,MFP_RO_VR)
						| Setup MFP intr vector base.

		ASM_WRIT_HWREG (&(MFP_TMR_STOPPED),a1,MFP_RO_TCDCR)
						| Stop UART clk/baud rate gen.
		ASM_WRIT_HWREG (&(ACP_HW_MFP_CLK_HZ/MFP_BAUD_9600/MFP_UART_CLK_DIV_16/4/2),a1,MFP_RO_TDDR)
						| Set UART clk/brg constant.
		ASM_WRIT_HWREG (&(MFP_TMR_CTL_D(MFP_TMR_PSCALE_4)),a1,MFP_RO_TCDCR)
						| Start UART clk/baud rate gen.

		ASM_CLR_HWREG (a1,MFP_RO_RSR)	| Stop UART Rx.
		ASM_CLR_HWREG (a1,MFP_RO_TSR)	| Stop UART Tx.
		ASM_READ_HWREG (a1,MFP_RO_TSR,d0)
		ASM_READ_HWREG (a1,MFP_RO_RSR,d0)
		ASM_READ_HWREG (a1,MFP_RO_UDR,d0)
						| Drain remaining status/data.

		ASM_WRIT_HWREG (&(MFP_UCR_SETVAL(1,MFP_UCR_CHLEN_8,MFP_UCR_CHSTOP_1,0,0)),a1,MFP_RO_UCR)
						| Set 8 bit data, 1 stop, div 16
		ASM_WRIT_HWREG (&(MFP_TSR_TX_ENABLE+MFP_TSR_TXSO_HIGH),a1,MFP_RO_TSR)
						| Enable UART Tx.
		ASM_WRIT_HWREG (&(MFP_RSR_RX_ENABLE),a1,MFP_RO_RSR)
						| Enable UART Rx.

		moveq	&0,d0			| Set OK (pseudo-)return code.
		ASM_RTN_CALLER (a0@)		| Return to caller.

| ----------------------------------------------------------------------------
|	Read a character from MFP UART.
|		Usage:	a0.l	- caller return address.
|			d0.l	- non-zero (T) to flush any pending input.
|		Consumes: a5,a1,d0.  (d0 is character read from UART).
| ----------------------------------------------------------------------------

		.globl	__MFP_ReadCh		| Put this into link map.
__MFP_ReadCh:
		ASM_ADDR_HW_REGS (a5)		| Base addr of HW IO registers.
		ASM_ADDR_MFP (a1)		| Get MFP base address.
		tstl	d0			| Are we flushing pending Rx ?
		jeq	L_MFPR_Loop		|   Br. if not.

		ASM_READ_HWREG (a1,MFP_RO_RSR,d0)
		ASM_READ_HWREG (a1,MFP_RO_UDR,d0)
		moveq	&0,d0			| Clear out return register.
L_MFPR_Loop:					| d0.l is zero at this point !
		ASM_PET_WATCHDOG (a5)		| Keep doggie happy.
		ASM_READ_HWREG (a1,MFP_RO_RSR,d0)
		andib	&(MFP_RSR_RX_EVNTMSK),d0
		jeq	L_MFPR_Loop		| Loop if no Rx event found.

		swap	d0			| Save Rx status, get scr d0.w
		ASM_READ_HWREG (a1,MFP_RO_UDR,d0)
		swap	d0			| Read Rx ch, restore Rx status.
		andib	&(MFP_RSR_RX_ERR_MSK),d0
		jne	L_MFPR_Loop		| Br. if found Rx error events.

		swap	d0			| Restore Rx char.
		extl	d0			| Clear high word of d0.l
		ASM_RTN_CALLER (a0@)		| Return to caller.

| ----------------------------------------------------------------------------
|	Read a character from MFP UART and return without looping.
|		Usage:	a0.l	- caller return address.
|			d0.l	- non-zero (T) to flush any pending input.
|		Consumes: a5,a1,d0.  (d0 is character read from UART).
|		Returns:  d0[31:16] is char read,
|			  d0[15:0]  is flag non-zero if char available.
| ----------------------------------------------------------------------------

		.globl	__MFP_ReadChNL		| Put this into link map.
__MFP_ReadChNL:
		ASM_ADDR_HW_REGS (a5)		| Base addr of HW IO registers.
		ASM_PET_WATCHDOG (a5)		| Keep doggie happy.
		ASM_ADDR_MFP (a1)		| Get MFP base address.
		tstl	d0			| Are we flushing pending Rx ?
		jeq	L_MFPRN_Read		|   Br. if not.

		ASM_READ_HWREG (a1,MFP_RO_RSR,d0)
		ASM_READ_HWREG (a1,MFP_RO_UDR,d0)
		moveq	&(0),d0			| Clear out return register.
L_MFPRN_Read:					| d0.l is zero at this point !
		ASM_READ_HWREG (a1,MFP_RO_RSR,d0)
		andib	&(MFP_RSR_RX_EVNTMSK),d0
		jeq	L_MFPRN_Exit		| Exit if no Rx event found.

		swap	d0			| Save Rx status, get scr d0.w
		ASM_READ_HWREG (a1,MFP_RO_UDR,d0)
		swap	d0			| Read Rx ch, restore Rx status.
		andib	&(MFP_RSR_RX_ERR_MSK),d0
		seq	d0			| Set if got char and no errors
		negb	d0			|   and give simple 0/1 result
		jne	L_MFPRN_Exit		| Exit if got good char.

		moveq	&(0),d0			| Clear out error result.
L_MFPRN_Exit:
		ASM_RTN_CALLER (a0@)		| Return to caller.

| ----------------------------------------------------------------------------
|	Write a character to MFP UART.
|		Usage:	a0.l	- caller return address.
|			d0.l	- character to Tx.
|		Consumes: a5,a1,d0.
| ----------------------------------------------------------------------------

		.globl	__MFP_WriteCh		| Put this into link map.
__MFP_WriteCh:
		ASM_ADDR_HW_REGS (a5)		| Base addr of HW IO registers.
		ASM_PET_WATCHDOG (a5)		| Keep doggie happy.
|
|	If char Tx poll takes too long, watchdog will fire if enabled !
|
		ASM_ADDR_MFP (a1)		| Get MFP base address.
		swap	d0			| Save Tx char, get scr d0.w
		clr.w	d0
L_MFPW_Loop:
		ASM_READ_HWREG (a1,MFP_RO_TSR,d0)
		andib	&(MFP_TSR_BUF_EMPTY),d0
		jeq	L_MFPW_Loop		| Loop if Tx buf in use.

		ASM_PET_WATCHDOG (a5)		| Keep doggie happy.
		swap	d0			| Restore Tx char and write it.
		ASM_WRIT_HWREG (d0,a1,MFP_RO_UDR)
		extl	d0			| Clear high word of d0.l
		ASM_RTN_CALLER (a0@)		| Return to caller.

| ----------------------------------------------------------------------------
|	These routines are entry points for various (MFP) timer interrupts.
| ----------------------------------------------------------------------------
		.globl	__timer_a_hdlr
__timer_a_hdlr:
		ASM_CALL_C_ROUTINE (_c_timer_A_hdlr)
		rte

		.globl	__timer_b_hdlr
__timer_b_hdlr:
		ASM_CALL_C_ROUTINE (_c_timer_B_hdlr)
		rte

		.globl	__timer_c_hdlr
__timer_c_hdlr:
		ASM_CALL_C_ROUTINE (_c_timer_C_hdlr)
		rte

| ----------------------------------------------------------------------------
|	These routines are entry points for various (MFP) UART interrupts.
| ----------------------------------------------------------------------------
		.globl	__te_hdlr
__te_hdlr:
		ASM_CALL_C_ROUTINE (_c_te_hdlr)
		rte

		.globl	__tbe_hdlr
__tbe_hdlr:
		ASM_CALL_C_ROUTINE (_c_tbe_hdlr)
		rte

		.globl	__re_hdlr
__re_hdlr:
		ASM_CALL_C_ROUTINE (_c_re_hdlr)
		rte

		.globl	__rbf_hdlr
__rbf_hdlr:
		ASM_CALL_C_ROUTINE (_c_rbf_hdlr)
		rte

| ----------------------------------------------------------------------------
|	Print out a null-terminated string.
|		Inputs:	a0.l	- caller return address
|			a1.l	- output string address.
|		Consumes: a2,d2,d0.
|		Transitively consumes: a5,a1,d0.
| ----------------------------------------------------------------------------
		.globl	__MFP_puts
__MFP_puts:
		movel	a1,d0		| Is string pointer a null pointer ?
		jeq	L_MFP_ps_exit	|   Br. out if so.

		movel	a0,d2		| Save off our return address.
		moveal	a1,a2		| Save off our output string pointer.
		lea	L_MFP_ps_loop,a0 |Get Tx loop return address.
L_MFP_ps_loop:
		moveq	&0,d0		| Clear d0.l for character hold.
		moveb	a2@+,d0		| Get next output char.
		jne	__MFP_WriteCh	| Tx any non-NULL character.

		moveal	d2,a0		| Restore our return address.
L_MFP_ps_exit:
		ASM_RTN_CALLER (a0@)	| Return to caller.

| ----------------------------------------------------------------------------
|	Write out a 32-bit hex value.
|		Inputs:	a0.l	- caller return address
|			d0.l	- 32-bit output value
|		Consumes: a0,d3,d2,d1,d0.
|		Transitively consumes: a5,a1,d0.
| ----------------------------------------------------------------------------
		.globl	__MFP_WriteHex
__MFP_WriteHex:
		movel	d0,d2		| Save the 32-bit data.
		movel	a0,d1		| Save return address.
		moveq	&0,d3		| Bit offset for next nibble to print.
		lea	L_MFP_wh_Loop,a0 |Set return address for output routine.

L_MFP_wh_Loop:				| Per-digit nibble print loop.
		cmpib	&32,d3		| All done ?
		jge	L_MFP_wh_Exit	|   Br. if so.

		bfextu	d2{d3,&4},d0	| Extract next nibble left-to-right.
		addql	&4,d3		| Increment for next output nibble.
		cmpib	&10,d0		| Is value a hex digit ?
		jge	L_MFP_wh_Alpha	|   Br. if so.

		addib	&(MFP_DIGIT_0),d0
		jra	__MFP_WriteCh	| Write out numeric hex digit.

L_MFP_wh_Alpha:
		addib	&(MFP_DIGIT_a-10),d0
		jra	__MFP_WriteCh	| Write out alphabetic hex digit.

L_MFP_wh_Exit:
		moveal	d1,a0		| Restore return address.
		ASM_RTN_CALLER (a0@)	| Return to caller.

| ----------------------------------------------------------------------------
|	Wait for MFP/UART Tx done.
|		Inputs:	a0.l	- caller return address.
|		Consumes: a5,a1,d0.
| ----------------------------------------------------------------------------
		.globl	__MFP_WaitTxDone
__MFP_WaitTxDone:
		ASM_ADDR_HW_REGS (a5)		| Base addr of HW IO registers.
		ASM_PET_WATCHDOG (a5)		| Keep doggie happy.
|
|	If char Tx done poll takes too long, watchdog will fire if enabled !
|
		ASM_ADDR_MFP (a1)		| Get MFP base address.
		moveq	&(0),d0			| Scratch d0.l.
L_MWTD_Loop:
		ASM_READ_HWREG (a1,MFP_RO_TSR,d0)
		andib	&(MFP_TSR_UNDERRUN),d0
		jeq	L_MWTD_Loop		| Loop if Tx not done.

		ASM_PET_WATCHDOG (a5)		| Keep doggie happy.
		ASM_RTN_CALLER (a0@)		| Return to caller.


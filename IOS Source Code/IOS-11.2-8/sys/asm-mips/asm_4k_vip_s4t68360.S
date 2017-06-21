/* $Id: asm_4k_vip_s4t68360.S,v 3.6.28.3 1996/04/30 23:25:35 amcrae Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_s4t68360.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_s4t68360.S - RVIP Wrapper for Serial 4T PA
 *
 * 04/06/95   H.M. Robison 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_s4t68360.S,v $
| Revision 3.6.28.3  1996/04/30  23:25:35  amcrae
| CSCdi54862:  4T interrupt handler hogs cpu
| Branch: California_branch
| Return value of work-to-do register from ISR handler.
|
| Revision 3.6.28.2  1996/04/10  02:41:50  rramacha
| CSCdi52294:  Add 4T support for VIP2
| Branch: California_branch
| SVIP specific register usage
|
| Revision 3.6.28.1  1996/03/21  22:27:39  gstovall
| Branch: California_branch
| The ELC_branch now knows the joy of California dreaming.
|
| Revision 3.2.2.5  1996/02/10  00:25:49  mbeesley
| CSCdi48581:  Add VIP2 support to ELC_branch
| Branch: ELC_branch
|
| Revision 3.2.2.4  1996/02/08  06:44:24  gstovall
| Branch: ELC_branch
| Sync to ELC_baseline_960205.
|
| Revision 3.2.2.3  1996/01/18  22:10:16  gstovall
| Branch: ELC_branch
| Preadator now knows the joy and splendor of serial interfaces.
|
| Revision 3.2.2.2  1996/01/15  07:59:18  dcarroll
| Branch: ELC_branch
| Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
|
| Revision 3.2.2.1  1995/12/11  17:36:35  mbeesley
| Branch: ELC_branch
| Sync ELC_branch to ELC_baseline_120995.
|
| Revision 3.6  1996/01/13  03:23:53  amcrae
| CSCdi45724:  4T f/w is not answering mbx msgs during an interface reset
| CSCdi43260:  VIP 4T has no implementation for transmit-delay
| New interrupt scheme added. Firmware counter update added.
|
| Revision 3.5  1995/12/23  01:09:47  rharagan
| CSCdi45811:  4T interface linestate does not refletct modem control
| lines
| Groundwork for enabling interrupts on 4T
|
| Revision 3.4  1995/12/14  07:39:10  rharagan
| CSCdi42045:  tx0 output hung (800E - queue full) when pinging on serial
| 4T
| Clean up typo
|
| Revision 3.3  1995/12/02  05:49:25  rharagan
| CSCdi41966:  Messages between 4T PA & driver should be per interface
| Move to single interface commands, incorporate code review comments.
|
| Revision 3.2  1995/11/17  08:41:04  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:55:39  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.2  1995/11/08  20:49:10  shaker
| Merge Arkansas_branch into 11.1 mainline.
|
| Revision 2.1  1995/06/07  20:07:02  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

/*
 * Platform include file
 */
#if defined(RVIP)
# include "asm_4k_vip.h"
# include "asm_4k_rvip.h"
#endif	/* RVIP */

#if defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif	/* SVIP */

#if defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif	/* PREDATOR */

#define S4T_INTERRUPT_DRIVEN	TRUE

/*
 * s4t_interrupt_wrapper:
 * 
 * For RVIP & Predator
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(pa_jumptable[slot])
 *	    s1 = Return address
 *	    a0 = PA bay
 *	    a1 = PA base address
 *	    a2 = interrupt bits [0..7] 
 *
 * For SVIP:
 * Entry:   Moved onto stack
 *      Context has been saved
 *      s0 = &(pa_jumptable[slot])
 *      s1 = Return address
 *      s2 = PA base address  - garbaged by wrapper (not PR)
 *      a1 = Posted Read indication - garbaged by wrapper
 *      a2 = Interrupt bits [0..7]
 *
 * Return:  v0 = return register
 *               return boolean, true if more work to do on TX and/or RX rings
 */
 #
 # boolean s4t_interrupt_wrapper (void)
 #
FRAME(s4t_interrupt_wrapper,sp,16,s1)
	.set	noreorder
/*
 * If interrupt driven, then check for interrupt code first.
 */
#ifdef S4T_INTERRUPT_DRIVEN

	beq	a2,zero,s4t_exit		# is there any work ?
	move	v0,zero				# Assume there is no work to do (bd slot)

#endif /* S4T_INTERRUPT_DRIVEN */

#if defined(SVIP)
	move    a1,s2              	# get PA base address in arg 1
#endif

	jal	s4t_interrupt_handler	# call PA front end handler
	addiu	a0,s0,PA_IDB_OFS	# Point first argument to IDB table (bd slot)

s4t_exit:
	/*
	 * return value from handler will indicate more work to do if !0
	 */
	j	s1				# return
	nop

	.set	reorder
ENDFRAME(s4t_interrupt_wrapper)

/* end of file */

/* $Id: asm_4k_vip_epad.S,v 3.1.62.5 1996/09/11 07:32:04 ptran Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_epad.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_epad.S : Wrapper for Encryption Port Adaptor
 *
 * August 1995, Paulina Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_epad.S,v $
| Revision 3.1.62.5  1996/09/11  07:32:04  ptran
| CSCdi68764:  access list on rsp doesnt get forward to vip
| Branch: California_branch
|
| Revision 3.1.62.3  1996/05/28  18:13:47  ptran
| CSCdi58378:  Encryption PA need performance improvement.
| Copy the encryption session control block to descriptor ring entry to
| reduce number of DMA setup per packet.
| Uses the PCI mailboxes to post the ring position intead of updating
| ownership bit.
| Branch: California_branch
|
| Revision 3.1.62.2  1996/04/22  18:08:36  ptran
| CSCdi55272:  Port asm_4k_vip_epad.S to california branch
| Branch: California_branch
| Port asm_4k_vip_epad.S to california.  Remove stub function
| epad_get_particle
| from crypto_engine_vip_api.c.
|
| Revision 3.1.2.3  1996/01/19  06:53:07  xliu
| Branch: IosSec_branch
| Commit before the second sync.`
|
| Revision 3.1.2.2  1996/01/09  22:38:38  ptran
| Branch: IosSec_branch
|
| Revision 3.1.2.1  1995/12/06  23:47:17  rmontino
| Branch: IosSec_branch
|
| Revision 3.1  1995/11/09  10:55:36  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/08/23  02:28:48  ptran
| Placeholder file for new development.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#if defined(RVIP)
#include "asm_4k_vip.h"
#include "asm_4k_rvip.h"
#endif

#if defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif  /* SVIP */

#if defined(PREDATOR)
#include "asm_4k_c7100.h"
#endif	/* PREDATOR */

/*
 * Define offsets to mailbox, and intr. status bits
 */
#define EPAD_IMB4_OFS		0x1C
#define EPAD_INTCSR_OFS		0x38

/*
 * epad_wrapper:
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
 # boolean epad_wrapper (void)
 #
FRAME(epad_wrapper,sp,16,s1)
	.set	noreorder

	andi	a2,a2,0x01			# mask interrupt bit 0
	bne	a2,zero,go_epad			# is is pending ?
	nop					# (bd slot)

	j	s1				# return
	move	v0,zero				# no more work (bd slot)

go_epad:
	addiu	sp,sp,-16			# make a frame
	sw	s1,4(sp)			# save s1 (return addr)

#if defined(SVIP)
        move    a1,s2                           # get PA base address in arg 1
#endif 
	/*	
	 * Read and write  intcsr register to clear the interrupt
	 */
	lw	s1,EPAD_INTCSR_OFS(a1)
	sw	s1,EPAD_INTCSR_OFS(a1)

	/*
	 * Always call the packet interrupt handler
	 */
	jal	epad_interrupt_handler	        # call EPAD interrupt handler
	lw      a0,PA_IDB_OFS(s0)		# read first IDB (packets) (bd)

	/*
	 * Check for key interrupt
	 */
/*
	jal	epad_interrupt			# call EPAD interrupt handler
	lw	a0,(PA_IDB_OFS+4)(s0)		# read 2nd IDB (keys) (bd)
*/

	lw	s1,4(sp)			# restore return address
	addiu	sp,sp,16			# remove frame

	j	s1				# return
	move	v0,zero				# return FALSE (bd slot)

	.set	reorder
ENDFRAME(epad_wrapper)


/* end of file */


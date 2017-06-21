/* $Id: asm_4k_c4000_am79970.S,v 3.2 1995/11/17 08:40:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/asm_4k_c4000_am79970.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_am79970.S - sierra assembly wrappers for
 * Am79c970 based NIMs.
 *
 * Aug 5 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_am79970.S,v $
| Revision 3.2  1995/11/17  08:40:30  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:55:10  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:30  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * Am79970 CSR0_BITS:
 */
#define AM79970_RINT_MASK   0x0400    /* Bit 10 */
#define AM79970_TINT_MASK   0x0200    /* Bit 9  */

#define AM79970_ERRORS      0xF900  /* ERR, BABL, CERR, MISS, MERR, and IDON */
#define AM79970_MASK        0xFF00


/*
 * am79970_wrapper:
 * am79970 wrapper
 * 
 * Entry :  Moved onto stack
 *	    Context has been saved
 *	    s0 = &(lev2_jumptable[slot])
 *	    s1 = Return address
 *	    s2 = IACK register address
 *	    a0 = Slot number
 *	    a1 = Slot base address
 */

 #
 # void am79970_wrapper (void)
 #
FRAME(am79970_wrapper,sp,0,s1)
	.set	noreorder

	lbu	t0,0(s2)			# read IACK register
	move	t1,a1				# get slot base address

        

        li	t2,AM79970_A_CSR_OFS            #store base addresses   
        li      t3,LEV2_IDB_A

        # The CSRS of consecutive subunits are separated
        # by 100h. This implies a left shift of 8 on the subunit #
        # will yield the offset to be added to the base offset address to
        # give the complete offset of the CSR of that subunit.
        sll     t4,t0,8                         
        add     t2,t2,t4                        

        # The addresses of IDB pointers of consecutive subunits 
        # are separated by 4h. This implies a left shift of 2 
        # the subunit# will yield the offset to be added to the 
        # base offset address to give the complete offset for the
        # IDB ptr of that subunit.
        sll     t4,t0,2                        
        add     t3,t3,t4                        

        # Form the full effective addresses for CSR0, IDB ptr.
        add     t1,t1,t2                        #form full CSR0 address
        add     s0,s0,t3                        #form full IDB address

am79970_start:
	lhu	s3,0(t1)		        # read CSR0 for subunit
	lw	s2,0(s0)		        # read IDB for subunit
	sh	s3,0(t1)		        # clear interrupt bits
	andi	s3,s3,AM79970_MASK		# mask out unwanted bits
	andi	t3,s3,AM79970_RINT_MASK		# mask for Rx intr.

	beq	t3,zero,check_tx		# is Rx bit set ?
	andi	s0,s3,AM79970_TINT_MASK		# check for Tx intr (bd slot)

	jal	lance_RX_interrupt		# call Rx interrupt handler
	move	a0,s2				# load up idb param (bd slot)
	
check_tx:
	beq	s0,zero,check_err		# is Tx bit set ?
	andi	s0,s3,AM79970_ERRORS		# check for errors bd slot)

	jal	lance_TX_interrupt		# call Tx interrupt handler 
	move	a0,s2				# load up idb param (bd slot)

check_err:
	beq	s0,zero,am79970_done		# is an Err bit set ?
	move	a0,s3				# copy CSR0 to a0

	jal	lance_E_interrupt		# call Err interrupt handler
	move	a1,s2				# load up idb param (bd slot)

am79970_done:
	j	s1				# return
	nop					# (bd slot)

	.set	reorder
ENDFRAME(am79970_wrapper)

/* End of File */


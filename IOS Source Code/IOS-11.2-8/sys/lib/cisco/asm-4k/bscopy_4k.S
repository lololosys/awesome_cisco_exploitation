/* $Id: bscopy_4k.S,v 3.1.10.1 1996/03/18 20:50:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-4k/bscopy_4k.S,v $
 *------------------------------------------------------------------
 * bscopy_4k.S -- byte swapped copy
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bscopy_4k.S,v $
 * Revision 3.1.10.1  1996/03/18  20:50:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:24:47  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef mips
#define ASMINCLUDE

#include "../machine/cpu_4k.h"

/*
 * bscopy:
 *
 * Byte swapped copy. Will round up byte count to be even IE a word count.
 * Will handle misaligned src and dst pointers. Will not handle overlapping
 * source and destination address ranges.
 */
 #
 # void bscopy (unsigned char *src, unsigned char *dst, int bytes)
 #
FRAME(bscopy,sp,0,ra)
	.set	noreorder

	blez	a2,bs_exit	# check for negative or zero count
	addiu	t2,a2,1		# round up count (bd slot)

	srl	t2,t2,1		# convert to word count by multiplying by 2
bs_loop:
	/*
	 * We have at least one word
	 */
	lb	t0,0(a0)	# byte 0 = *(src)
	lb	t1,1(a0)	# byte 1 = *(src + 1)
	sb	t0,1(a1)	# *(dst+1) = byte 0
	sb	t1,0(a1)	# *(dst) = byte 1
	addiu	a0,a0,2		# src += 2

	addiu	t2,t2,-1	# wcount -= 1
	bgtz	t2,bs_loop	# branch if we have more to do
	addiu	a1,a1,2		# dst += 2 (bd slot)

bs_exit:
	j	ra		# return
	nop			# (bd slot)
	
	.set	reorder
ENDFRAME(bscopy)
#endif

/* $Id: bcmp_4k.S,v 3.1.10.1 1996/03/18 20:50:07 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-4k/bcmp_4k.S,v $
 *------------------------------------------------------------------
 * bcmp_4k.S -- byte comparison routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcmp_4k.S,v $
 * Revision 3.1.10.1  1996/03/18  20:50:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:24:45  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef mips
#define ASMINCLUDE

#include "../machine/cpu_4k.h"

/*
 * bcmp:
 *
 * Return 0 if comparison true, otherwise return 1.
 * Note : The M68K implementation returns the difference
 *        between the bytes that do not match rather than 1
 *        when the compare fails.
 */

 #
 # int bcmp (uchar *p1, uchar *p2, int bytes)
 #
FRAME(bcmp,sp,0,ra)
	.set	noreorder
	
	beq	a0,a1,bcmp_yes	# pointers are the same, so return true.
	addiu	t1,a0,1		# t1 = p1 + 1 (bd slot)

	blez	a2,bcmp_yes	# is count negative or zero?
	addiu	t2,a1,1		# t2 = p2 + 1 (bd slot)

	lb	t3,-1(t1)	# t3 = *(t1 - 1)  (preload the first byte)

bcmp_loop:
	lb	t4,-1(t2)	# t4 = *(t2 - 1)
	addiu	t1,t1,1		# t1 += 1  (load delay slot)
	
	bne	t3,t4,bcmp_no	# bytes are different
	addiu	t2,t2,1		# t2 += 1 (bd slot)

	addiu	a2,a2,-1	# bytes -= 1

	bgtz	a2,bcmp_loop	# finished ?
	lb	t3,-1(t1)	# t3 = *(t1 - 1) (bd slot)

bcmp_yes:
	j	ra		# return
	move	v0,zero		# with 0 as return value (bd slot)

bcmp_no:
	j	ra		# return
	ori	v0,zero,0x1	# with 1 as return value (bd_slot)

	.set 	reorder
ENDFRAME(bcmp)
#endif

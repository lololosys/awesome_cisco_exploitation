/* $Id: bcopy_4k.S,v 3.1.10.1 1996/03/18 20:50:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-4k/bcopy_4k.S,v $
 *------------------------------------------------------------------
 * bcopy_4k.S -- very fast byte copy
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcopy_4k.S,v $
 * Revision 3.1.10.1  1996/03/18  20:50:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:24:46  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef mips
#define ASMINCLUDE

#include "../machine/cpu_4k.h"

/*
 * bcopy:
 *
 * Very fast byte copy.  Will handle miss aligned sources and
 * destinations, although "double long" aligned (i.e. 64 bit boundary)
 * source and destination addresses will increase speed significantly.
 *
 * Notes: This routine vectors to bcopy_32 if the src address is above
 *	  the top of kuseg to handle cases where something is copied from
 *	  the monitor's ROM (which does not support 64bit cycles).
 *        
 * Limitation:
 *     1. This routine is currently processor specific rather than platform
 *        specific. However, non-64bit cycle memory boudaries are platform
 *        specific. Right now the kuseg boundary is defined as R4K_K0BASE 
 *        and it really should be platform dependent.  So please do not 
 *        use bcopy() to access NVRAM which does not support 64bit cycle
 *        but is within the R4K_K0BASE boundary on 4X00.
 *
 *     2. This routine only checks for one boundary. It does not support 
 *        non-contigous non-64bit cycle memory which requires multiple 
 *        boundaries checks.
 *
 *     3. This routine does not check for overlapping source and
 *        destination address ranges.
 *
 */

 #
 # void bcopy (uchar *src, uchar *dst, int bytes)
 #
FRAME(bcopy,sp,0,ra)
	.set	noreorder
	.set mips3

	/*
	 * If src address is above the top of kuseg, call bcopy_32
	 */
	li	t0,R4K_K0BASE	# get first address past kuseg
	sltu	t1,t0,a0	# set t1 if t0 < a0
	bgtz	t1,bcopy_32	# call bcopy_32 if t1 is set
	nop			# (bd slot)

	blez	a2,bc_exit	# check for negative or zero count
	move	t2,a2		# copy count

	/*
	 * If we have less than 32 bytes, we don't attempt to optimize.
	 */

	addiu	t3,a2,-32	# subtract 32 from count
	bltz	t3,bc_odd_loop	# if less than 32 bytes, do it byte by byte
	andi	t0,a0,0x7	# t0 = src & 0x7 (bd slot)

	/*
	 * If our source or destination addresses are not 64 bit aligned
	 * we have to use different copying loops for speed.
	 */

	bne	t0,zero,bc_odd_src	# src pointer is not word aligned
	andi	t1,a1,0x7		# t1 = dst & 0x7 (bd slot)
	bne	t1,zero,bc_odd_dst	# dst pointer not word aligned
	addiu	t2,t2,-32		# decrement the count (bd slot)

	/*
	 * We have at least 32 bytes, count has been pre-decremented
	 * and both src and dst are double word aligned
	 */
bc_loop1:
	ld	t4,0(a0)	# read *(src)
	ld	t5,8(a0)	# read *(src + 8)
	ld	t6,16(a0)	# read *(src + 16)
	ld	t7,24(a0)	# read *(src + 24)
	sd	t4,0(a1)	# save *(dst)
	sd	t5,8(a1)	# save *(dst + 8)
	sd	t6,16(a1)	# save *(dst + 16)
	sd	t7,24(a1)	# save *(dst + 24)

	addiu	a0,a0,32	# src += 32
	addiu	t3,t2,-32	# decrement copy of count
	bltz	t3,bc_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32	# dst += 32 (bd slot)

	b	bc_loop1	# do 32 byte loop again
	addiu	t2,t2,-32	# decrement real count (bd slot)

bc_odd_dst:
	/*
	 * We have at least 32 bytes, count is predecremented, src is
	 * double word aligned, dst is unaligned
	 */
	ld	t4,0(a0)	# read *(src)
	ld	t5,8(a0)	# read *(src + 8)
	ld	t6,16(a0)	# read *(src + 16)
	ld	t7,24(a0)	# read *(src + 24)

	sdl	t4,0(a1)	# save *(dst)
	sdr	t4,7(a1)
	sdl	t5,8(a1)	# save *(dst + 8)
	sdr	t5,15(a1)
	sdl	t6,16(a1)	# save *(dst + 16)
	sdr	t6,23(a1)
	sdl	t7,24(a1)	# save *(dst + 24)
	sdr	t7,31(a1)

	addiu	a0,a0,32	# src += 32
	addiu	t3,t2,-32	# decrement copy of count
	bltz	t3,bc_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32	# dst += 32 (bd slot)

	b	bc_odd_dst	# do 32 byte loop again
	addiu	t2,t2,-32	# decrement real count (bd slot)

bc_odd_src:
	bne	t1,zero,bc_odd_src_dst	# have odd src and dst
	addiu	t2,t2,-32		# decrement count (bd slot)

bc_loop2:
	/*
	 * We have at least 32 bytes, count pre-decremented, src is mis-aligned
	 * but the dst is double word aligned
	 * 
	 * Note : The MIPS 4.0.5F assembler does not like the scheduling
	 * of the following instructions with regard to the src register
	 * (a0) and the incrementing of this register. So for the time
	 * being, copy a0 to t0 and use t0 as the base register
	 */
	move	t0,a0		# stupid assembler !!

	ldl	t4,0(t0)	# read *(src)
	ldr	t4,7(t0)
	ldl	t5,8(t0)	# read *(src + 8)
	ldr	t5,15(t0)
	ldl	t6,16(t0)	# read *(src + 16)
	ldr	t6,23(t0)
	ldl	t7,24(t0)	# read *(src + 24)
	ldr	t7,31(t0)

	sd	t4,0(a1)	# save *(dst)
	sd	t5,8(a1)	# save *(dst + 8)
	sd	t6,16(a1)	# save *(dst + 16)
	sd	t7,24(a1)	# save *(dst + 24)

	addiu	t3,t2,-32	# decrement copy of count
	addiu	a0,a0,32	# src += 32

	bltz	t3,bc_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32	# dst += 32 (bd slot)

	b	bc_loop2	# do 32 byte loop again
	addiu	t2,t2,-32	# decrement real count (bd slot)

bc_odd_src_dst:
	/*
	 * We have at least 32 bytes, count has been pre-decremented
	 * and both source and destination pointers are misaligned
	 *
	 * Note : The MIPS 4.0.5F assembler does not like the scheduling
	 * of the following instructions with regard to the src register
	 * (a0) and the incrementing of this register. So for the time
	 * being, copy a0 to t0 and use t0 as the base register
	 */
	move	t0,a0		# stupid assembler !!

	ldl	t4,0(t0)	# read *(src)
	ldr	t4,7(t0)
	ldl	t5,8(t0)	# read *(src + 8)
	ldr	t5,15(t0)
	ldl	t6,16(t0)	# read *(src + 16)
	ldr	t6,23(t0)
	ldl	t7,24(t0)	# read *(src + 24)
	ldr	t7,31(t0)

	sdl	t4,0(a1)	# save *(dst)
	sdr	t4,7(a1)
	sdl	t5,8(a1)	# save *(dst + 8)
	sdr	t5,15(a1)
	sdl	t6,16(a1)	# save *(dst + 16)
	sdr	t6,23(a1)
	sdl	t7,24(a1)	# save *(dst + 24)
	sdr	t7,31(a1)

	addiu	a0,a0,32	# src += 32
	addiu	t3,t2,-32	# decrement copy of count

	bltz	t3,bc_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32	# dst += 32 (bd slot)

	b	bc_odd_src_dst	# do 32 byte loop again
	addiu	t2,t2,-32	# decrement real count (bd slot)

bc_odd_loop:
	addiu	t2,t2,-1	# decrement count
	bltz	t2,bc_exit	# any more ? no, exit
	nop			# (bd slot)

	lb	t3,0(a0)	# load a byte
	addiu	a0,a0,1		# increment src pointer
	sb	t3,0(a1)	# save a byte

	b	bc_odd_loop	# loop again
	addiu	a1,a1,1		# increment dst pointer (bd slot)
	
bc_exit:
	j	ra		# return
	move	v0,a2		# return count (bd slot)

	.set	reorder
ENDFRAME(bcopy)

/*
 * bcopy_32:
 *
 * Very fast byte copy.  Will handle miss aligned sources and
 * destinations, although "long" aligned (i.e. 32 bit boundary)
 * source and destination addresses will increase speed significantly.
 *
 * Notes: This routine assumes 32bit capable source and destination
 *        hardware. 
 *
 *        This routine does not check for overlapping source and
 *        destination address ranges.
 */

 #
 # void bcopy_32 (uchar *src, uchar *dst, int bytes)
 #
FRAME(bcopy_32,sp,0,ra)
	.set	noreorder
	.set mips3

	blez	a2,bc32_exit		# check for negative or zero count
	move	t2,a2			# copy count

	addiu	t3,a2,-32		# subtract 32 from count
	bltz	t3,bc32_odd_loop	# < 32 bytes? do it byte by byte
	andi	t0,a0,0x3		# t0 = src & 0x3 (bd slot)

	/*
	 * If our source or destination addresses are not 32 bit aligned
	 * we have to use different copying loops for speed.
	 */
	
	bne	t0,zero,bc32_odd_src	# src pointer not word aligned
	andi	t1,a1,0x3		# t1 = dst & 0x3 (bd slot)

	bne	t1,zero,bc32_odd_dst	# dst pointer not word aligned
	addiu	t2,t2,-32		# decrement the count (bd slot)

	/*
	 * We have at least 32 bytes, count has been pre-decremented
	 * and both src and dst are word aligned
	 */
bc32_loop1:
	lw	t4,0(a0)		# read *(src)
	lw	t5,4(a0)		# read *(src + 4)
	lw	t6,8(a0)		# read *(src + 8)
	lw	t7,12(a0)		# read *(src + 12)
	sw	t4,0(a1)		# save *(dst)
	sw	t5,4(a1)		# save *(dst + 4)
	sw	t6,8(a1)		# save *(dst + 8)
	sw	t7,12(a1)		# save *(dst + 12)

	lw	t4,16(a0)		# read *(src + 16)
	lw	t5,20(a0)		# read *(src + 20)
	lw	t6,24(a0)		# read *(src + 24)
	lw	t7,28(a0)		# read *(src + 28)
	sw	t4,16(a1)		# save *(dst + 16)
	sw	t5,20(a1)		# save *(dst + 20)
	sw	t6,24(a1)		# save *(dst + 24)
	sw	t7,28(a1)		# save *(dst + 28)

	addiu	a0,a0,32		# src += 32
	addiu	t3,t2,-32		# decrement copy of count
	bltz	t3,bc32_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32		# dst += 32 (bd slot)

	b	bc32_loop1		# do 32 byte loop again
	addiu	t2,t2,-32		# decrement real count (bd slot)

bc32_odd_dst:
	/*
	 * We have at least 32 bytes, count is predecremented, src is
	 * word aligned, dst is unaligned
	 */
	lw	t4,0(a0)		# read *(src)
	lw	t5,4(a0)		# read *(src + 4)
	lw	t6,8(a0)		# read *(src + 8)
	lw	t7,12(a0)		# read *(src + 12)

	swl	t4,0(a1)		# save *(dst)
	swr	t4,3(a1)
	swl	t5,4(a1)		# save *(dst + 4)
	swr	t5,7(a1)
	swl	t6,8(a1)		# save *(dst + 8)
	swr	t6,11(a1)
	swl	t7,12(a1)		# save *(dst + 12)
	swr	t7,15(a1)

	lw	t4,16(a0)		# read *(src + 16)
	lw	t5,20(a0)		# read *(src + 20)
	lw	t6,24(a0)		# read *(src + 24)
	lw	t7,28(a0)		# read *(src + 28)

	swl	t4,16(a1)		# save *(dst + 16)
	swr	t4,19(a1)
	swl	t5,20(a1)		# save *(dst + 20)
	swr	t5,23(a1)
	swl	t6,24(a1)		# save *(dst + 24)
	swr	t6,27(a1)
	swl	t7,28(a1)		# save *(dst + 28)
	swr	t7,31(a1)

	addiu	a0,a0,32		# src += 32
	addiu	t3,t2,-32		# decrement copy of count
	bltz	t3,bc32_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32		# dst += 32 (bd slot)

	b	bc32_odd_dst		# do 32 byte loop again
	addiu	t2,t2,-32		# decrement real count (bd slot)

bc32_odd_src:
	bne	t1,zero,bc32_odd_src_dst	# have odd src and dst
	addiu	t2,t2,-32			# decrement count (bd slot)

bc32_loop2:
	/*
	 * We have at least 32 bytes, count pre-decremented, src is mis-aligned
	 * but the dst is aligned
	 */
	lwl	t4,0(a0)		# read *(src)
	lwr	t4,3(a0)
	lwl	t5,4(a0)		# read *(src + 4)
	lwr	t5,7(a0)
	lwl	t6,8(a0)		# read *(src + 8)
	lwr	t6,11(a0)
	lwl	t7,12(a0)		# read *(src + 12)
	lwr	t7,15(a0)

	sw	t4,0(a1)		# save *(dst)
	sw	t5,4(a1)		# save *(dst + 4)
	sw	t6,8(a1)		# save *(dst + 8)
	sw	t7,12(a1)		# save *(dst + 12)

	lwl	t4,16(a0)		# read *(src + 16)
	lwr	t4,19(a0)
	lwl	t5,20(a0)		# read *(src + 20)
	lwr	t5,23(a0)
	lwl	t6,24(a0)		# read *(src + 24)
	lwr	t6,27(a0)
	lwl	t7,28(a0)		# read *(src + 28)
	lwr	t7,31(a0)

	sw	t4,16(a1)		# save *(dst + 16)
	sw	t5,20(a1)		# save *(dst + 20)
	sw	t6,24(a1)		# save *(dst + 24)
	sw	t7,28(a1)		# save *(dst + 28)

	addiu	a0,a0,32		# src += 32
	addiu	t3,t2,-32		# decrement copy of count
	bltz	t3,bc32_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32		# dst += 32 (bd slot)

	b	bc32_loop2		# do 32 byte loop again
	addiu	t2,t2,-32		# decrement real count (bd slot)

bc32_odd_src_dst:
	/*
	 * We have at least 32 bytes, count has been pre-decremented
	 * and both source and destination pointers are misaligned
	 */
	lwl	t4,0(a0)		# read *(src)
	lwr	t4,3(a0)
	lwl	t5,4(a0)		# read *(src + 4)
	lwr	t5,7(a0)
	lwl	t6,8(a0)		# read *(src + 8)
	lwr	t6,11(a0)
	lwl	t7,12(a0)		# read *(src + 12)
	lwr	t7,15(a0)

	swl	t4,0(a1)		# save *(dst)
	swr	t4,3(a1)
	swl	t5,4(a1)		# save *(dst + 4)
	swr	t5,7(a1)
	swl	t6,8(a1)		# save *(dst + 8)
	swr	t6,11(a1)
	swl	t7,12(a1)		# save *(dst + 12)
	swr	t7,15(a1)

	lwl	t4,16(a0)		# read *(src + 16)
	lwr	t4,19(a0)
	lwl	t5,20(a0)		# read *(src + 20)
	lwr	t5,23(a0)
	lwl	t6,24(a0)		# read *(src + 24)
	lwr	t6,27(a0)
	lwl	t7,28(a0)		# read *(src + 28)
	lwr	t7,31(a0)

	swl	t4,16(a1)		# save *(dst + 16)
	swr	t4,19(a1)
	swl	t5,20(a1)		# save *(dst + 20)
	swr	t5,23(a1)
	swl	t6,24(a1)		# save *(dst + 24)
	swr	t6,27(a1)
	swl	t7,28(a1)		# save *(dst + 28)
	swr	t7,31(a1)

	addiu	a0,a0,32		# src += 32
	addiu	t3,t2,-32		# decrement copy of count
	bltz	t3,bc32_odd_loop	# less than 32 left, do by byte
	addiu	a1,a1,32		# dst += 32 (bd slot)

	b	bc32_odd_src_dst	# do 32 byte loop again
	addiu	t2,t2,-32		# decrement real count (bd slot)
	
bc32_odd_loop:
	addiu	t2,t2,-1	# decrement count
	bltz	t2,bc32_exit	# any more ? no, exit
	nop			# (bd slot)

	lb	t3,0(a0)	# load a byte
	addiu	a0,a0,1		# increment src pointer
	sb	t3,0(a1)	# save a byte

	b	bc32_odd_loop	# loop again
	addiu	a1,a1,1		# increment dst pointer (bd slot)
	
bc32_exit:
	j	ra		# return
	move	v0,a2		# return count (bd slot)

	.set mips2
	.set	reorder
ENDFRAME(bcopy_32)
#endif

/* $Id: ipasm_4k.S,v 3.2.58.1 1996/07/20 00:22:06 skufer Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/ipasm_4k.S,v $
 *------------------------------------------------------------------
 * ipasm_4k.S - R4k asm support for IP
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipasm_4k.S,v $
 * Revision 3.2.58.1  1996/07/20  00:22:06  skufer
 * CSCdi60295:  7500 cannot fast-switch packets greater than 8192 bytes
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:41:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "cpu_4k.h"

/*
 * fastiphdrchk:
 * Check an Internet header for the low end fast switching support
 *  - Ensure version 4 and header length of 5
 *  - range check the length (.ge. 20 )
 *  - validate checksum
 *  - decrement ttl and update checksum
 *
 */

 #
 # ushort fastiphdrchk (ushort *ptr, ulong dest)
 #
FRAME(fastiphdrchk,sp,0,ra)
	.set	noreorder

	/*
	 * Read the first two longs of the header
	 */
	ldl	t0,0(a0)	# read 8 bytes
	ldr	t0,7(a0)	# into t0
	li	t6,0x45		# version/header length

	dsrl	t1,t0,32	# get first long (not sign extended)
	dsll	t2,t0,32	# get second long (not sign extended)
	dsrl	t2,t2,32

	andi	t4,t1,0xffff	# t4 = t1 & 0xffff (t4 = ip->total_length)
	addiu	t5,t4,-20	# t5 = t4 - 20 (subtract IP header length, 20)
	bltz	t5,fastfail1	# too short ?

	srl	t5,t1,24	# t5 = t1 >> 24 (bd slot) (t5 = IP Ver, Lenght)

	ldl	t0,8(a0)	# read first half of next 8 bytes
	bne	t5,t6,fastfail1	# version/header length wrong ?
	ldr	t0,15(a0)	# read second half of next 8 bytes

	dsrl	t3,t0,32	# get third long (not sign extended)
	dsll	t4,t0,32	# get fourth long (not sign extended)
	dsrl	t4,t4,32

	/*
	 * t1 = 1st long, t2 = 2nd long, t3 = 3rd long, t4 = 4th long
	 */
	lui	t5,0xff00		# t5 = 0xff000000
	lui	t6,0x0100		# t6 = 0x01000000
	and	t5,t5,t3		# t5 = t3 & 0xff000000

	beq	t5,t6,fastfail1		# TTL = 1 (TTL exceeded)
	nop

	beq	t5,zero,fastfail1	# TTL = 0 (TTL exceeded)
	dsubu	t3,t3,t6		# decrement TTL (bd slot)

	daddu	v0,t1,t2	# v0 = t1 + t2	[longs 1+2]
	daddu	v0,v0,t3	# v0 += t3	[longs 1+2+3]  
	daddu	v0,v0,t4	# v0 += t4	[longs 1+2+3+4] [0 C B A]

	lui	t1,0xffff
	dsrl	t1,t1,32		# load t1 with 0xffffffff

	and	t5,v0,t1		# t5 = v0 & 0xffffffff	 [BA]
	dsrl	v0,v0,32		# v0 >>= 32		 [DC]
	daddu	v0,v0,t5		# v0 = t5 + v0		 [BA + DC]

	and	a1,a1,t1		# a1 = a1 & 0xffffffff
	daddu	v0,v0,a1		# v0 = v0 + a1		 [BA+DC+Dest]
	
	and	t5,v0,t1		# t5 = v0 & 0xffffffff
	dsrl	v0,v0,32		# v0 >>= 32		 [Carry]
	daddu	v0,v0,t5		# v0 = t5 + v0		 [BA+DC+Dest+
					#			  Carry]

	andi	t5,v0,0xffff		# t5 = v0 & 0xffff
	srl	v0,v0,16		# v0 >>= 16
	daddu	v0,v0,t5		# v0 = v0 + t5

	andi	t5,v0,0xffff		# t5 = v0 & 0xffff
	srl	v0,v0,16		# v0 >>= 16
	daddu	v0,v0,t5		# 16 bit checksum

	/*
	 * Must ensure checksum is ~0x100 (0xfeff)
	 * v0 = checksum [0 0 0 A] (16 bits)
	 */
	ori	t0,zero,0xfeff	# 1s complement of 0x100
	bne	t0,v0,fastfail2	# new checksum did not equal ~0x100
	andi	t6,t3,0xffff	# t6 = old checksum (bd slot)

	dsubu	t0,t6,v0	# t0 = t6 - v0
	sltu	t5,t6,v0	# if t6 < v0, t5 = 1 else t5 = 0
	dsubu	t0,t0,t5	# subtract out the borrow if any
	andi	t0,t0,0xffff	# get 16 bits
				# t0 is now 16bit checksum

	lui	t2,0xffff	# t2 = 0xffff0000
	and	t3,t3,t2	# t3 = t3 & 0xffff0000 (strip out old checksum)
	or	t3,t3,t0	# t3 = t3 | t0 (put in new checksum)

	swl	t3,8(a0)	# write back new TTL and checksum
	swr	t3,11(a0)

	j	ra		# return
	move	v0,zero		# return success (bd slot)

fastfail2:
	swl	t3,8(a0)	# write new TTL and original checksum
	swr	t3,11(a0)
	
fastfail1:
	j	ra		# return
	li	v0,0x1		# return failure (bd slot)

	.set	reorder
ENDFRAME(fastiphdrchk)


/* $Id: vinesasm_4k.S,v 3.2 1995/11/17 08:41:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/vinesasm_4k.S,v $
 *------------------------------------------------------------------
 * vinesasm_4k.S - Vines R4k asm support
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vinesasm_4k.S,v $
 * Revision 3.2  1995/11/17  08:41:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "cpu_4k.h"

/*
 * vines_crc1:
 * Checksum a Vines packet
 */

 #
 # ushort vines_crc1 (ushort *data)
 #
FRAME(vines_crc1,sp,0,ra)
	.set	noreorder

	lhu	t0,2(a0)	# read length word
	addiu	t4,a0,6		# (load delay slot) start computation
				# past checksum, len, transport and
				# control

	# Clear the last byte in an odd-length packet

	andi	t1,t0,0xfffe	# strip bit zero of length
	beq	t1,t0,is_even	# is the length even ?
	addu	t2,a0,t0	# no, so get last byte addr and
	sb	zero,0(t2)	# zero out last byte
				# (no overrun due to buffer slop)

is_even:
	addiu	t2,t0,-5	# do not include checksum,len,transport,control
				# and add 1 (to round up to no. of shorts)
				# length = length - 6 + 1 = length - 5

	srl	t1,t2,1		# convert length to number of shorts

	lhu	v0,4(a0)	# read transport and type

	lhu	t2,0(t4)	# (load delay slot) get first data byte
				# to prime the loop

	andi	v0,v0,0xf0ff	# mask out hop count
	addu	v0,v0,t0	# add length in.
	
crc_loop:
	addu	v0,v0,t2	# add the data to our running checksum
	addiu	t4,t4,2		# increment address to next short word
	addiu	t1,t1,-1	# decrement short length counter

	bne	t1,zero,crc_loop # do we have more stuff to do ?
	lhu	t2,0(t4)	# load next word (*bd slot*)

	/*
	 * Fix up carries : v0 is [0 B A]
	 */
	andi	t0,v0,0xffff	# t0 = v0 & 0xffff    	[A]
	srl	t1,v0,16	# t1 = v0 >> 16		[B]
	addu	v0,t0,t1	# v0 = t0 + t1	 [Carry][A+B]
	andi	t0,v0,0xffff	# t0 = A+B		[A+B]
	srl	t1,v0,16	# t1 = carry		[Carry]
	addu	v0,t0,t1	# v0 = t0 + t1
				# which is 16bits

	li	t8,0xffff	# An 0xffff result is illegal, so check.
	beq	t8,v0,all_effs	# Is it?
	nop

	j	ra		# no, return result
	nop			# (bd slot)

all_effs:
	j	ra
	move	v0,zero		# yes, return zero

	
	.set	reorder
ENDFRAME(vines_crc1)

/* End of file */

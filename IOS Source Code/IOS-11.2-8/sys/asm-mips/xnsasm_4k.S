/* $Id: xnsasm_4k.S,v 3.2 1995/11/17 08:41:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/xnsasm_4k.S,v $
 *------------------------------------------------------------------
 * xnsasm_4k.S - XNS R4k asm support
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsasm_4k.S,v $
 * Revision 3.2  1995/11/17  08:41:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "cpu_4k.h"

/*
 * xns_crc1:
 * Checksum an XNS packet. We assume a short aligned packet.
 */

 #
 # ushort xns_crc1 (ushort *data)
 #
FRAME(xns_crc1,sp,0,ra)
	.set	noreorder

	lhu	t8,2(a0)	# get length word
	move	t2,zero		# zero out accumulator
	addiu	t8,t8,-1	# remove checksum length, and add one
	srl	t8,t8,1		# get length in words
	
	addiu	t0,a0,2		# skip checksum, start at (data + 2)
	
	lhu	t1,0(t0)	# load data word
xns_a:
	addiu	t0,t0,2		# increment address
	addu	t2,t2,t1	# add data word in
	srl	t3,t2,16	# get carry bit
	andi	t2,t2,0xffff	# clear the carry (would affect "srl" below)
	addu	t2,t2,t3	# get 16bit partial sum
	srl	t4,t2,15	# get bit 15 in bit 0 position
	sll	t2,t2,1		# shift left one position
	andi	t2,t2,0xffff	# clear any leftover bits from the shift
	or	t2,t2,t4	# or in bit 15 into bit 0 position

	addiu	t8,t8,-1	# decrement word counter

	bne	t8,zero,xns_a	# more to do ?
	lhu	t1,0(t0)	# read the next word (bd slot)

	li	t8,0xffff	# 0xffff is an illegal checksum, so check.
	bne	t8,t2,xns_b	# skip to end if legal..

	nop			# (bd slot)
	move	t2,zero		# zero out return value
	
xns_b:
	j	ra		# return
	move	v0,t2		# load return value (bd slot)



	.set	reorder
ENDFRAME(xns_crc1)

/*
 * xns_qcsum:
 * Checksum an XNS packet (quickly -;). Note we assume short aligned
 * packet.
 */

 #
 # void xns_qcsum (uchar *pak, ushort oldword, int num)
 #
FRAME(xns_qcsum,sp,0,ra)
	.set	noreorder

	lhu	t0,2(a0)	# get length of packet
	sll	t8,a2,1		# convert to byte index
	addu	t8,a0,t8	# get address of changed word
	addiu	t0,t0,1		# round it up
	srl	t0,t0,1		# make it a word length
	
	lhu	v0,0(a0)	# read checksum from packet
	li	t2,0xffff	# 0xffff means we are not checksuming
	beq	v0,t2,quick_c	# are we doing checksums ?
	lhu	t7,0(t8)	# fetch the word that changed (bd slot)
	
	subu	v1,t7,a1	# v1 = word - oldword
	sltu	t6,t7,a1	# if t7 < a1, t6 -> 1 (i.e. sum
				# operation would have carried)
	addu	v1,v1,t6	# compensate for possible carry
	andi	v1,v1,0xffff	# make it 16bits (may not need this)

	subu	t6,t0,a2	# subtract index of changed word
	andi	t6,t6,0xf	# modulo 16
	
	li	t5,0x10		# preload with 16
	subu	t5,t5,t6	# t5 = 16 - t6
	sll	t4,v1,t6	# shift v1 left by t6
	srl	v1,v1,t5	# shift v1 right by 16 - t6
	or	v1,v1,t4	# v1 rotated left by t6 (16bit rotate)
	andi	v1,v1,0xffff	# make it 16bits

	addu	v0,v0,v1	# add with checksum field
	srl	v1,v0,16	# get carry bit
	andi	v0,v0,0xffff	# and out carry field
	addu	v0,v0,v1	# add carry back in

quick_c:
	j	ra		# return
	sh	v0,0(a0)	# save new sum to checksum (bd slot)

	.set	reorder
ENDFRAME(xns_qcsum)

/*
 * xns_fqc_header:
 * Fast path checksum routine for XNS. Note, we assume a short aligned
 * packet.
 */

 #
 # void xns_fqc_header (char *ptr)
 #
FRAME(xns_fqc_header,sp,0,ra)
	.set	noreorder

	lhu	t0,2(a0)	# read length
	lhu	a1,0(a0)	# get checksum
	addiu	t0,t0,1		# round up length to words
	srl	t0,t0,1		# get length in words
	li	t1,2		# load t1 with index of changed word
	subu	t0,t0,t1	# t0 = t0 - 2, subtract index of changed word
	andi	t0,t0,0xf	# modulo 16
	li	t1,256		# load t1 with new minus old difference
	li	t5,0x10		# preload with 16
	subu	t5,t5,t0	# t5 = 16 - t0
	sll	t4,t1,t0	# shift t1 left by t0 amount
	srl	t1,t1,t5	# shift t1 right by 16 - t0
	or	t1,t1,t4	# t1 rotated left by t0 (16bit rotate)
	andi    t1,t1,0xffff
	addu	a1,a1,t1	# add with old checksum  [Carry][A+B]
	srl	t1,a1,16	# get carry bit			[Carry]
	andi	a1,a1,0xffff	# and out carry bit		[A+B]
	addu	a1,a1,t1	# add carry back in		[A+B+Carry]

	j	ra		# return
	sh	a1,0(a0)	# save the checksum (bd slot)

	.set	reorder
ENDFRAME(xns_fqc_header)

/* End of File */

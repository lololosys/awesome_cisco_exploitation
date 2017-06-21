/* $Id: asm_4k.S,v 3.2.54.9 1996/08/08 21:06:01 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k.S,v $
 *------------------------------------------------------------------
 * asm_4k.S - R4k asm support
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k.S,v $
 * Revision 3.2.54.9  1996/08/08  21:06:01  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.2.54.8  1996/06/24  21:25:23  smackie
 * Fix the assumption that the R4600 and 68K platforms always have a
 * "classic" Rom Monitor. (CSCdi61149)
 * Branch: California_branch
 *
 * Revision 3.2.54.7  1996/06/12  19:08:18  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits (partial commit part 1)
 * Branch: California_branch
 *
 * Revision 3.2.54.6  1996/06/07  05:10:32  getchell
 * CSCdi59206:  EMT_RELOAD, EMT_CPU_EXCEPTION does call svip_reload()
 * Branch: California_branch
 *
 * Revision 3.2.54.5  1996/05/22  14:34:48  getchell
 * Enable emt call support
 * CSCdi48894:  show hardware in if-con will not display proper memory size
 * Branch: California_branch
 *
 * Revision 3.2.54.4  1996/04/29  18:28:54  mbeesley
 * CSCdi55668:  r4k exception handler can let interrupts in
 * Branch: California_branch
 *
 * Revision 3.2.54.3  1996/03/29  20:02:05  mbeesley
 * CSCdi52977: Some r4k implementations handle parity exceptions
 *             incorrectly. Fix them.
 * Branch: California_branch
 *
 * Revision 3.2.54.2  1996/03/21  22:27:00  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.54.1  1996/03/18  18:51:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/02/20  00:32:45  mbeesley
 * CSCdi49338:  Predator external intr. controller is static - dont check
 *         o Add support for a static external interrupt controller
 *         o Remove some old dead debug code
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/10  00:25:18  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:24:51  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:48:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/25  17:44:28  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.1  1995/06/07  20:06:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#if 	defined(SIERRA)
# include "asm_4k_c4000.h"
#endif	/* SIERRA */

#if	defined(RSP)
# include "asm_4k_rsp.h"
# include "../src-rsp/rsp_pcmap.h"
#endif	/* RSP */

#if 	defined(RVIP)
# include "asm_4k_vip.h"
# include "asm_4k_rvip.h"
#endif	/* RVIP */

#if	defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif	/* SVIP */

#if	defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif

#include "../os/signal.h"

/*
 * Externs; allow gas to use gp relative addressing
 */
	.extern spurcnt, 4
	.extern forkx, 4
	.extern cpu_type, 4

	.extern NMI_STACK, 4
	.extern NMI_COUNTER, 4
	.extern NMI_SUBR, 4

/*
 * setjmp:
 *
 * R4K Jump buf : Uses the same structure as the 68K, but does
 * not use all the elements. Presently these routines are implemented
 * to support a 32bit CPU register context only.
 *
 *      ----------------
 * 	| 	PC	|    jmp_buf[0]
 *  	-----------------
 * 	| 	SP	|           [1]
 * 	-----------------
 *  	| 	S0	|           [2]
 * 	-----------------
 * 	| 	S1	|           [3]
 * 	-----------------
 * 	| 	S2	|           [4]
 * 	-----------------
 * 	| 	...	|           ...
 * 	-----------------
 * 	| 	S8	|           [10]
 * 	-----------------
 */

/*
 * Note : Only uses 32bit CPU context
 */

 #
 # int setjmp (jmp_buf *ptr)
 #
FRAME(setjmp,sp,0,ra)
	.set	noreorder

	sw	ra,(a0)           # save return address (PC) [0] 
	sw      sp,4(a0)          # save stack pointer       [1]
	sw      s0,8(a0)          # save reg s0              [2]
	sw      s1,12(a0)         # save reg s1              [3]
	sw      s2,16(a0)         # save reg s2              [4]
	sw      s3,20(a0)         # save reg s3              [5]
	sw      s4,24(a0)         # save reg s4              [6]
	sw      s5,28(a0)         # save reg s5              [7]
	sw      s6,32(a0)         # save reg s6              [8]
	sw      s7,36(a0)         # save reg s7              [9]
	sw	s8,40(a0)	  # save reg s8		     [10]
	j       ra
	move    v0,zero           # return 0 (bd slot)

	.set	reorder
ENDFRAME(setjmp)

/*
 * longjmp:
 *
 * Note : Only uses 32bit CPU context
 *
 */

 #
 # void longjmp (jmp_buf *ptr, int ret_val)
 #
FRAME(longjmp,sp,0,ra)
	.set	noreorder

	lw	ra,(a0)           # load return address (PC) [0] 
	lw      sp,4(a0)          # load stack pointer       [1]
	lw      s0,8(a0)          # load reg s0              [2]
	lw      s1,12(a0)         # load reg s1              [3]
	lw      s2,16(a0)         # load reg s2              [4]
	lw      s3,20(a0)         # load reg s3              [5]
	lw      s4,24(a0)         # load reg s4              [6]
	lw      s5,28(a0)         # load reg s5              [7]
	lw      s6,32(a0)         # load reg s6              [8]
	lw      s7,36(a0)         # load reg s7              [9]
	lw	s8,40(a0)	  # load reg s8		     [10]
	j       ra
	move    v0,a1             # return ret_val (bd slot)

	.set	reorder
ENDFRAME(longjmp)

/*
 * swap_16bit_word
 *
 * Byte swap routines for shorts
 */

 #
 # ushort swap_16bit_word (ushort num)
 #
	.globl	swap_16bit_word

FRAME(swap_16bit_word,sp,0,ra)
	.set	noreorder

	andi	t1,a0,0xff	# t1 = num & 0xff
	sll	t1,t1,8		# t1 = t1 << 8
	srl	t0,a0,8		# t0 = num >> 8
	andi	t0,t0,0xff	# t0 = t0 & 0xff

	j	ra		# return
	or	v0,t0,t1	# v0 = t0 | t1 (bd slot)

	.set	reorder
ENDFRAME(swap_16bit_word)

/*
 * swap_32bit_word
 *
 * Byte swap routines for longs (not vax order!!)  [ 1 2 3 4 -> 2 1 4 3 ]
 */

 #
 # swap_32bit_word (ulong num)
 #
	.globl	swap_32bit_word

FRAME(swap_32bit_word,sp,0,ra)
	.set	noreorder

	andi	t1,a0,0xff	# t0 = num & 0xff (byte 1)
	sll	t1,t1,8		# t0 = t0 << 8
	srl	t2,a0,8		# t1 = num >> 8 (byte 2)
	andi	t2,t2,0xff	# t1 = t1 & 0xff
	or	t1,t1,t2	# bytes 1 and 2 swapped

	srl	a0,a0,16	# num = num >> 16
	andi 	t3,a0,0xff	# t3 = num & 0xff (byte 3)
	sll	t3,t3,8		# t3 = t3 << 8
	srl	t4,a0,8		# t4 = num >> 8 (byte 4)
	andi	t4,t4,0xff	# t4 = t4 & 0xff
	or	t3,t3,t4	# bytes 3 and 4 swapped

	sll	t3,t3,16	# t3 = t3 << 16
	
	j	ra		# return
	or	v0,t1,t3	# v0 = t1 | t3 (bd slot)

	.set	noreorder
ENDFRAME(swap_32bit_word)

/*
 * vaxorder:
 *
 * VAX order byte swap for longs [ 1 2 3 4 -> 4 3 2 1 ]
 */

 #
 # ulong vaxorder (ulong num)
 #
FRAME(vaxorder,sp,0,ra)
	.set	noreorder

	andi	t0,a0,0xff	# t0 = num & 0xff	[0 0 0 4]
	or	v0,t0,zero	# v0 = t0		[0 0 0 4]
	sll	v0,v0,8		# v0 = v0 << 8		[0 0 4 0]
	andi	t0,a0,0xff00	# t0 = num & 0xff00	[0 0 3 0]
	srl	t0,t0,8		# t0 = t0 >> 8		[0 0 0 3]
	or	v0,v0,t0	# v0 = v0 | t0		[0 0 4 3]
	sll	v0,v0,8		# v0 = v0 << 8		[0 4 3 0]
	srl	a0,a0,16	# num = num >> 16	[0 0 1 2]
	andi	t0,a0,0xff	# t0 = num & 0xff	[0 0 0 2]
	or	v0,v0,t0	# v0 = v0 | t0		[0 4 3 2]
	sll	v0,v0,8		# v0 = v0 << 8		[4 3 2 0]
	andi	t0,a0,0xff00	# t0 = num & 0xff00	[0 0 1 0]
	srl	t0,t0,8		# t0 = t0 >> 8		[0 0 0 1]

	j	ra		# return
	or	v0,v0,t0	# v0 = v0 | t0		[4 3 2 1] (bd slot)

	.set	reorder
ENDFRAME(vaxorder)

/*
 * ipttl:
 *
 * Decrement IP TTL field and fixup checksum. Returns TRUE if TTL
 * exceeded (TTL <= 1) or FALSE otherwise.
 *
 * Note : We are assuming that ptr is *long* aligned. This will break 
 *        if the alignment changes. 
 *
 */

 #
 # boolean ipttl (uchar *ptr)
 #
FRAME(ipttl,sp,0,ra)
	.set	noreorder

	lw	t0,8(a0)	# t0 = IP packet[8..11]
	lui	t1,0xff00	# t1 = 0xff000000
	lui	t2,0x0100	# t2 = 0x01000000

	and	t3,t1,t0	# t3 = (IP packet[8..11] & 0xff000000)
	beq	t2,t3,ttl_bad	# TTL equal 1? (TTL exceeded)
	nop
	beq	zero,t3,ttl_bad # TTL equal 0? (TTL exceeded)
	subu	t0,t0,t2	# decrement TTL (bd slot)

	andi	t1,t0,0xffff	# t1 = IP checksum
	ori	t6,zero,0xfeff	# t6 = ~0x100

	subu	t2,t1,t6	# t1 = t1 - t6
	sltu	t3,t1,t6	# if t1 < t6, borrow = 1, else borrow = 0
	subu	t1,t2,t3	# subtract out borrow if any
	andi	t1,t1,0xffff	# mask checksum to 16 bits

	lui	t2,0xffff	# t2 = 0xffff0000
	and	t0,t0,t2	# t0 &= 0xffff0000, strip out old checksum
	or	t0,t0,t1	# t0 |= checksum, put in new checksum

	sw	t0,8(a0)	# write new IP Packet[8..11]

	j	ra		# return
	move	v0,zero		# return FALSE, everything OK (bd slot)

ttl_bad:
	j	ra		# return
	li	v0,0x1		# return TRUE, TTL exceeded (bd slot)

	.set	reorder
ENDFRAME(ipttl)

/*
 * IPCRC_8BYTE_CHUNK : The code to sum an aligned eight byte chunk
 */
#define IPCRC_8BYTE_CHUNK		\
	addiu	a0,a0,8;		\
	lwu	t4,-8(a0);		\
	lwu	t5,-4(a0);		\
	daddu	v0,v0,t4;		\
	daddu	v0,v0,t5;

/*
 * IPCRC_8BYTE_MISALIGNED : The code to sum a misaligned 8 byte chunk
 */
#define IPCRC_8BYTE_MISALIGNED		\
	addiu	a0,a0,8;		\
	lwl	t4,-8(a0);		\
	lwr	t4,-5(a0);		\
	lwl	t5,-4(a0);		\
	lwr	t5,-1(a0);		\
	and	t4,t4,t7;		\
	and	t5,t5,t7;		\
	daddu	v0,v0,t4;		\
	daddu	v0,v0,t5;		

/*
 * ipcrc:
 *
 * Return the 1s complement checksum of the buffer at ptr, 
 * n bytes long. Note the buffer may be misaligned.
 *
 */

 #
 # ushort ipcrc (ushort *ptr, int n)
 #
FRAME(ipcrc,sp,0,ra)
	.set	noreorder


	/*
	 * Check to see if we are long aligned.  If not, use slightly less 
	 * efficient algorithm.
	 */
	andi	t0,a0,0x3		# keep only the 2 least sig. bits
	bne	t0,zero,ipcrc_unaligned	# If they're not zero, we can't be
					# as fast. :-(
	move	v0,zero			# zero checksum (bd slot)

	srl	t2,a1,0x6		# t2 = no. of 64 byte chunks
	
	/*
	 * Strategy:
	 *	Copy using chunks of 64 bytes to keep the memory "bursting"
	 *	into the data cache.  Accumulate a 64 bit checksum and 
	 *	munge it down to 16 bits at the *end* instead of doing
	 *	it more expensively "on the fly."
	 *
	 * To stay efficient, we join the checksumming loop "already in
	 * progress" to make sure that when we complete the last 64
	 * byte chunk we'll have at most 3 bytes left to manually
	 * incorporate into the checksum.
	 *
	 * Here's how we calculate the place to jump into:
	 *
	 * Jump address = ip_a_loop + (all instructions [160]) -
	 * 		  (instructions per 8 bytes * no. of 8 byte chunks) -
	 *		  (odd no. of 4byte chunks * 3 instructions)
	 * Instructions per 8 bytes = 5 (20 bytes)
	 * Instructions per 4 bytes = 3 (12 bytes)
	 *
	 * t8 (jump address) = ip_a_loop + 160 - (a1&0x38)*2.5 - (a1&0x4)*3
	 */
	la	t8,ip_a_loop		# get the loop address
	daddiu	t8,t8,0xA0		# t8 = ip_a_loop + 160
	andi	t6,a1,0x38		# t6 = a1 & 0x38
	sll	t5,t6,1			# t5 = (t6 * 2)
	dsubu	t8,t8,t5		# t8 = ip_a_loop + 160 - (a1&0x38)*2
	srl	t6,t6,1			# t6 = (t6 * 1/2)
	dsubu	t8,t8,t6		# t8 = ip_a_loop + 160 - (a1&0x38)*2.5


	/*
	 * We may have 1/2 of an 8 byte chunk in the beginning, so

	 * we must adjust "jumping" into our loop to cover that initial case.
	 */

	andi	t1,a1,0x4		# if the 3rd bit is 1, we have
	bne	t1,zero,ip_a_4odd	# a straggler that needs to be adjusted
	move	t4,zero			# zero t4 just in case (bd slot)

	j	t8			# jump into the loop
	nop				# (bd slot)

ip_a_4odd:
	daddiu	t8,t8,-12		# decrement the jump addr to catch the
					# stragler in the preceding long word

	j	t8			# jump into the loop
	addiu	a0,a0,4			# pre-adjust src address (bd slot)
	
ip_a_loop:

	IPCRC_8BYTE_CHUNK		/* Bytes [0..7]      */
	IPCRC_8BYTE_CHUNK		/* Bytes [8..15]     */
	IPCRC_8BYTE_CHUNK		/* Bytes [16..23]    */
	IPCRC_8BYTE_CHUNK		/* Bytes [24..31]    */
	IPCRC_8BYTE_CHUNK		/* Bytes [32..39]    */
	IPCRC_8BYTE_CHUNK		/* Bytes [40..47]    */
	IPCRC_8BYTE_CHUNK		/* Bytes [48..55]    */
	IPCRC_8BYTE_CHUNK		/* Bytes [56..63]    */

	addiu	t2,t2,-1		# decrement 64 byte chunk count
	bgez	t2,ip_a_loop		# finished all the 64 byte chunks?
	nop				# (bd slot)

	/*
	 * We have finished 4 byte chunks and 64byte chunks
	 * Must now handle the remaining bytes, (0, 1, 2 or 3 bytes)
	 */
	andi	t1,a1,0x3		# t1 = no of single bytes
	beq	t1,zero,ip_a_done	# none ?
	nop				# (bd slot)

	lwu	t5,0(a0)		# load the single bytes in a single
					# speedy 32 bit read.
	/*
	 * Figure out how many bytes we had left to read and make sure
	 * we mask out any parts of the 32-bit word that don't contain
	 * valid checksum data.   We do this by shifting 0xff's into a
	 * masking position for each unused byte.
	 */

	addiu	t1,t1,-1		# t1 -= 1
	sll	t1,t1,3			# t1 = t1 * 8
	lui	t6,0xff00		# t6 = 0xff000000
	sra	t6,t6,t1		# t6 = (0xff000000 >> (bytes-1)*8)
	and	t5,t5,t6		# t5 = (bytes & t6)
	daddu	v0,v0,t5		# Add the result to our checksum

ip_a_done:
	/*
	 * We're almost done.  The 64 bit checksum is in v0.  We must
	 * "roll it back" to 16 bits and invert it.  Note, bits 63..48
	 * may not always be zero because ipcrc is also used to
	 * checksum large chunks of memory, like nvram.
         *
	 * First we'll break it up onto two 32-bit quantities (X & Y)
	 * and adjust for overflows.
	 *
	 * After that, we'll take the 32 bit result, break it up into two
         * 16 bit results (A & B) and do the same.  We'll wind up with a
	 * 16 bit number. 
	 */

	lui	t4,0xffff
	dsrl	t4,t4,32		# load t4 with 0xffffffff

	and	t5,v0,t4		# t5 = X (lower 32 bits)
	dsrl	v0,v0,32		# v0 = Y (upper 32 bits)
	daddu	v0,v0,t5		# v0 = X + Y

	/*
	 * At this point, X + Y may overflow 32 bits, so we'll have to 
	 * check and adjust for an addition "carry" in the result.
	 */

	and	t5,v0,t4		# t5 = lower 32 bits of X + Y
	dsrl	v0,v0,32		# v0 = upper bits of X + Y (the carry)

	daddu	v0,v0,t5		# v0 = X + Y + carry

	/* Now do it again, breaking the 32 bit result down to 16 bits. */

	andi	t5,v0,0xffff		# t5 = A (lower 16 bits)
	srl	v0,v0,16		# v0 = B (upper 16 bits)
	daddu	v0,v0,t5		# v0 = A + B

	/* Adjust for possible overflow (carry) resulting from A + B */

	andi	t5,v0,0xffff		# t5 = lower 16 bits of A + B
	srl	v0,v0,16		# v0 = upper bits of A + B (the carry)
	daddu	v0,v0,t5		# v0 = A + B + carry

	j	ra			# return 
	nor	v0,v0,zero		# complement the final checksum in
					# branch delay slot and skedaddle.

ipcrc_unaligned:
	srl	t2,a1,0x6		# t2 = no. of 64 byte chunks
	li	t7,-1			# load 0x00000000ffffffff into t7
	dsrl	t7,t7,32

	/*
	 * Strategy:
	 *	Copy using chunks of 64 bytes to keep the memory busy.
	 *	Accumulate a 64 bit checksum and munge it down to 16 bits at
	 *	the *end* instead of doing it more expensively "on the fly."
	 *
	 * To stay efficient, we join the checksumming loop "already in
	 * progress" to make sure that when we complete the last 64
	 * byte chunk we'll have at most 3 bytes left to manually
	 * incorporate into the checksum.
	 *
	 * Here's how we calculate the place to jump into:
	 *
	 * Jump address = ip_u_loop + (all instructions [288]) -
	 * 		  (instructions per 8 bytes * no. of 8 byte chunks) -
	 *		  (odd no. of 4byte chunks * 6 instructions)
	 * Instructions per 8 bytes = 9 (36 bytes)
	 * Instructions per 4 bytes = 6 (24 bytes)
	 *
	 * t8 = ip_u_loop + 288 - (a1&0x38)*4.5 - (a1&0x4)*6
	 */
	la	t8,ip_u_loop		# get the loop address
	daddiu	t8,t8,0x120		# t8 = ip_u_loop + 288
	andi	t6,a1,0x38		# t6 = a1 & 0x38
	sll	t5,t6,2			# t5 = (a1 & 0x38) * 4
	dsubu	t8,t8,t5		# t8 = ip_u_loop + 288 - (a1&0x38)*4
	srl	t6,t6,1			# t6 = (a1 & 0x38) / 2
	dsubu	t8,t8,t6		# t8 = ip_u_loop + 288 - (a1&0x38)*4.5

	/*
	 * We may have 1/2 of an 8 byte chunk in the beginning, so
	 * we must adjust "jumping" into our loop to cover that initial case.
	 */

	andi	t1,a1,0x4		# if the 3rd bit is 1, we have a
	bne	t1,zero,ip_u_4odd	# straggler that needs to be adjusted
	move	t4,zero			# zero t4 just in case (bd slot)

	j	t8			# jump into the loop
	nop				# (bd slot)

ip_u_4odd:
	daddiu	t8,t8,-24		# decrement the jump address to
					# catch the preceding long word

	j	t8			# jump into the loop
	addiu	a0,a0,4			# pre-adjust src address (bd slot)
	
ip_u_loop:

	IPCRC_8BYTE_MISALIGNED		/* Bytes [0..7]       	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [8..15]      	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [16..23]      	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [24..31]     	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [32..39]     	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [40..47]     	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [48..55]     	*/
	IPCRC_8BYTE_MISALIGNED		/* Bytes [56..63]	*/

	addiu	t2,t2,-1		# decrement 64byte chunk count
	bgez	t2,ip_u_loop		# finished all the 64 byte chunks?
	nop				# (bd slot)

	/*
	 * We have finished 4 byte chunks and 64 byte chunks
	 * Must now handle a single remaining bytes, (0,1,2 or 3 bytes)
	 */
	andi	t1,a1,0x3		# t1 = no of single bytes
	beq	t1,zero,ip_a_done	# none ?
	nop				# (bd slot)

	/*
	 * Figure out how many bytes we had left and make sure
	 * we mask out any parts of t5 that don't contain valid
	 * info (including bits 63..32).
	 */
	lwl	t5,0(a0)		# load the remaining bytes
	lwr	t5,3(a0)		# (note, this sign extends)
	addiu	t1,t1,-1		# t1 -= 1
	sll	t1,t1,3			# t1 = t1 * 8
	dsll	t5,t5,32		# shift left 32 bits (clear bits 63.32)
	dsrl	t5,t5,32		# shift right 32 bits

	lui	t6,0xff00		# t6 = 0xff000000
	sra	t6,t6,t1		# t6 = (0xff000000 >> (bytes-1)*8)
	and	t5,t5,t6		# t5 = (bytes & t6)
	daddu	v0,v0,t5		# Add the result to our checksum

	b	ip_a_done		# fix up checksum
	nop				# (bd slot)

	.set reorder
ENDFRAME(ipcrc)

/*
 * Layout of sprocess data structure for R4K :
 *
 * Context	Register	t0 offset
 * -------	--------	---------
 * reg[0] 	a0		8(t0)
 * reg[1] 	a1		12(t0)
 * reg[2] 	s0		16(t0)
 * reg[3] 	s1		20(t0)
 * reg[4] 	s2		24(t0)
 * reg[5] 	s3		28(t0)
 * reg[6] 	s4		32(t0)
 * reg[7] 	s5		36(t0)
 * reg[8] 	s6		40(t0)
 * reg[9] 	s7		44(t0)
 * reg[10] 	s8		48(t0)
 * reg[11]	sp		52(t0)
 * reg[12]	PC		56(t0)
 * reg[13]	ra		60(t0)
 * ps		Cause register	64(t0)
 */

/*
 * define some externs so the asm will use gp relative addressing
 */
	.extern schedflag,4

/*
 * resume:
 *
 * Switch from scheduler context back to process context
 *
 * Note:
 *	The context is a0,a1,s0..s8,sp,PC,ra (32 bit)
 *
 *	resume re-installs a0 and a1 as they are the process start
 *	function address and its argument. ps is copied into the
 *	cause register to force a gdb exception when one process
 *	is debugging another.
 */

 #
 # void resume (void)
 #
FRAME(resume,sp,96,ra)
	.set	noreorder

	/*
	 * Save the scheduler's context on the scheduler's stack.
	 */

	addiu	sp,sp,-96	# make some room on scheduler stack
	sw	ra,16(sp)	# save ra
	sw	s0,20(sp)	# save s0
	sw	s1,24(sp)	# save s1
	sw	s2,28(sp)	# save s2
	sw	s3,32(sp)	# save s3
	sw	s4,36(sp)	# save s4
	sw	s5,40(sp)	# save s5
	sw	s6,44(sp)	# save s6
	sw	s7,48(sp)	# save s7
	sw	s8,52(sp)	# save s8

	/*
	 * Restore the process' context from its own stack
	 */

	lw	t0,forkx	# get address of process data to be restored
	move	t1,sp		# make copy of scheduler stack pointer
				# in load-delay slot.

	lw	sp,4(t0)	# sp = forkx->savedsp (process stack)
	lw	a0,8(t0)	# restore a0
	lw	a1,12(t0)	# restore a1
	lw	s0,16(t0)	# restore s0
	lw	s1,20(t0)	# restore s1
	lw	s2,24(t0)	# restore s2
	lw	s3,28(t0)	# restore s3
	lw	s4,32(t0)	# restore s4
	lw	s5,36(t0)	# restore s5
	lw	s6,40(t0)	# restore s6
	lw	s7,44(t0)	# restore s7
	lw	s8,48(t0)	# restore s8
	lw	ra,60(t0)	# restore ra
	lw	t6,64(t0)	# get new processor status value

	sw	t1,4(t0)	# save the scheduler stack pointer
				# in forkx->savedsp

	sw	zero,schedflag	# Now leaving the scheduler

	mtc0	t6,CP0_CAUSE	# load ps value. If we are returning to a
				# process that is being debugged by gdb,
				# gdb will have set bit 8 of ps, which
				# will cause us to take a sw 0
				# interrupt here resulting in a SIGTRAP to gdb

	j	ra		# return in process context
	nop			# (bd slot)

	.set	reorder
ENDFRAME(resume)

/*
 * suspend:
 *
 * Switch from process context back to scheduler context
 *
 * Note:
 *	The context is s0..s8,sp,PC,ra (32bit)
 * 	8(t0) is forkx->regs[0]
 */

 #
 # void suspend (void)
 #
FRAME(suspend,sp,96,ra)
	.set	noreorder

	/*
	 * Save the processes context into the forkx process structure
	 * After switching stack pointers.
	 */

	li	t2,1
	lw	t0,forkx	# get address of process context structure
	sw	t2,schedflag	# Now entering the scheduler
	move	t1,sp		# make copy of stack pointer in load delay
				# slot.  We''ll use it later.

	lw	sp,4(t0)	# Restore scheduler''s stack pointer
				# from forkx->savedsp

	sw	s0,16(t0)	# save s0
	sw	s1,20(t0)	# save s1
	sw	s2,24(t0)	# save s2
	sw	s3,28(t0)	# save s3
	sw	s4,32(t0)	# save s4
	sw	s5,36(t0)	# save s5
	sw	s6,40(t0)	# save s6
	sw	s7,44(t0)	# save s7
	sw	s8,48(t0)	# save s8
	sw	ra,60(t0)	# save ra

	sw	t1,4(t0)	# save the process context''s stack pointer
				# into forkx->savedsp

	/*
	 * Restore the scheduler's context from the scheduler's own stack.
	 */

	lw	ra,16(sp)	# restore ra
	lw	s0,20(sp)	# restore s0
	lw	s1,24(sp)	# restore s1
	lw	s2,28(sp)	# restore s2
	lw	s3,32(sp)	# restore s3
	lw	s4,36(sp)	# restore s4
	lw	s5,40(sp)	# restore s5
	lw	s6,44(sp)	# restore s6
	lw	s7,48(sp)	# restore s7
	lw	s8,52(sp)	# restore s8
	addiu	sp,sp,96	# remove stack frame

	j	ra		# return in scheduler context
	nop			# (bd slot)

	.set	reorder
ENDFRAME(suspend)

/*
 * void process_forced_here (ulong routine_to_call)
 *
 * This routine is mainly a stack frame placeholder so that it is obvious how
 * a processes execution thread got from its normal code space to a completely
 * unrelated routine.  The stack will have been modified to transfer execution
 * to this routine either by the routine alter_suspended_process(), or by the
 * watchdog timer.  This routine saves the registers that C doesn't because it
 * might have been inserted into the execution stream at a point where its
 * parent routine wasn't expecting to call a subroutine, and thus may be
 * actively using these registers.
 */

	.globl	running_process_forced_here
	.globl	process_forced_here

FRAME(suspended_process_forced_here,sp,96,ra)
	.set	noreorder
	.set	noat

	la	k0,forkx	# get address of forkx
	lw	k0,0(k0)	# get contents of forkx
	beq	zero,zero,process_forced_here
	lw	ra,68(k0)	# the process executed the "j ra" at
				# the end of "resume" to get here so
				# the ra value is no longer pertinent.
				# Fudge it so that gdb does the right
				# thing when the backtrace command is
				# issued. (bd slot)

running_process_forced_here:
	la	k0,forkx	# get address of forkx
	lw	k0,0(k0)	# get contents of forkx

process_forced_here:
	subu	sp,sp,96
	sw	ra,92(sp)	# Save regs at,v0-v1,a0-a3,t0-t9,hi,lo
	sw	t9,88(sp)
	sw	t8,84(sp)
	sw	t7,80(sp)
	sw	t6,76(sp)
	sw	t5,72(sp)
	sw	t4,68(sp)
	sw	t3,64(sp)
	sw	t2,60(sp)
	sw	t1,56(sp)
	sw	t0,52(sp)
	sw	a3,48(sp)
	sw	a2,44(sp)
	sw	a1,40(sp)
	sw	a0,36(sp)
	sw	v1,32(sp)
	sw	v0,28(sp)
	sw	AT,24(sp)
	mfhi	t1
	mflo	t0
	sw	t1,20(sp)
	sw	t0,16(sp)


	lw	k1,72(k0)	# load address of inserted routine
	jal	k1		# call it
	nop			# (bd slot)

	lw	t0,16(sp)	# restore regs at,v0-v1,a0-a3,t0-t9,hi,lo
	lw	t1,20(sp)
	mtlo	t0
	mthi	t1
	lw	AT,24(sp)
	lw	v0,28(sp)
	lw	v1,32(sp)
	lw	a0,36(sp)
	lw	a1,40(sp)
	lw	a2,44(sp)
	lw	a3,48(sp)
	lw	t0,52(sp)
	lw	t1,56(sp)
	lw	t2,60(sp)
	lw	t3,64(sp)
	lw	t4,68(sp)
	lw	t5,72(sp)
	lw	t6,76(sp)
	lw	t7,80(sp)
	lw	t8,84(sp)
	lw	t9,88(sp)
	lw	ra,92(sp)	# load address of interrupted routine
	addu	sp,sp,96	# pop the stack
	j	ra		# go back to original routine
	lw	ra,68(k0)	# restore ra of interrupted routine (bd slot)

	.set	at
	.set	reorder
ENDFRAME(suspended_process_forced_here)

/*
 * void alter_suspended_process (sprocess *proc, ulong new_address)
 *
 * Change the execution address of a process.  This is used when
 * killing a process so the it has a chance to clean up before it is
 * completely destroyed.
 *
 * Note:
 * 	8(t0) is proc->regs[0]
 */

FRAME(alter_suspended_process,sp,0,ra)
	.set	noreorder
	lw	t0,4(a0)	# get stack pointer of process
	lw	t1,60(a0)	# get current return address

	sw	t1,68(a0)	# save original return address
	sw	a1,72(a0)	# save routine to call first
	
	la	t2,suspended_process_forced_here
	j	ra		# return
	sw	t2,60(a0)	# set ra to helper routine (bd slot)

	.set	reorder
ENDFRAME(alter_suspended_process)

/*
 * watchdog_timed_out    [Not callable from C.]
 *
 * Routine to fudge the stack of a running process.  This routine gets wedged
 * into the execution path when the clock interrupt code determines that the
 * current process has been running for too long.  The clock handler pulls the
 * original return address out of the exception frame and saves it in the
 * variable wd_save_return.  It then puts the address of this routine into the
 * exception frame, so that this code is called immediately upon return from the
 * clock interrupt.
 */
	.comm	wd_save_return,4

FRAME(watchdog_timed_out,sp,0,ra)
	.set	noreorder
	la	k0,forkx	# get address of forkx
	lw	k0,0(k0)	# get contents of forkx

	lw	k1,wd_save_return	# Push the original execution address
	sw	k1,68(k0)
	la	k1,signal_receive	# Push handler to be called
	sw	k1,72(k0)
	li	k1,SIGWDOG
	sw	k1,76(k0)		# forkx->pending_signal = SIGWDOG
	j	running_process_forced_here
	nop
	.set	reorder
ENDFRAME(watchdog_timed_out)



/*
 * caller_frame:
 *
 * Return the callers stack pointer
 */

 #
 # ulong *caller_frame (void)
 #
FRAME(caller_frame,sp,0,ra)
	.set	noreorder

	j	ra		# return
	move	v0,sp		# return sp (bd slot)

	.set	reorder
ENDFRAME(caller_frame)

/*
 *  current_pc:
 *
 * Return an approximation of the PC that called us.
 */

 #
 # ulong current_pc (void)
 #
FRAME(current_pc,sp,0,ra)
	.set	noreorder

	j	ra		# return to the caller
	move	v0,ra		# snarf their return address (bd slot)

	.set	reorder
ENDFRAME(current_pc)

/*
 * STORAGE DEFINITION:
 * 	ulong r4k_cpu_level 	 : Current processor level
 *	ulong r4k_1ms_pipe_ticks : Pipeline ticks for 1ms of time
 *	ulong r4k_4ms_pipe_ticks : Pipeline ticks for 4ms of time
 *	ulong r4k_second_wdog	 : True if 2 watchdogs (rsp2)
 *	ulong r4k_pipe_speed	 : CPU pipeline speed in MHz
 *	ulong r4k_count_period_ns: Number of ns each pipeline tick takes
 *
 * 	The level is maintained in bits[5..3] to allow efficient table lookup
 *
 *	r4k_4ms_pipe_ticks and r4k_second_wdog are both accessed
 *	in the  4msec handler.  Keeping them close means we've
 *      got a good chance of getting both for the price of one
 *      cache line fill.
 */

 #
 # ulong r4k_cpu_level, r4k_1ms_pipe_ticks, r4k_4ms_pipe_ticks, 
 #	 r4k_second_wdog,
 #	 r4k_pipe_speed, r4k_count_period_ns, level2_cache_present;
 #
	.globl	r4k_cpu_level
	.globl	r4k_intr_rec_count
	.globl	r4k_1ms_pipe_ticks
	.globl	r4k_4ms_pipe_ticks
	.globl	r4k_second_wdog
	.globl	r4k_pipe_speed
	.globl	r4k_count_period_ns
	.globl	level2_cache_present

	.sdata
	.align 4

r4k_cpu_level:
	.word 	0
r4k_intr_rec_count:
	.word	0
r4k_1ms_pipe_ticks:
	.word 	0
r4k_4ms_pipe_ticks:
	.word 	0
r4k_second_wdog:
	.word	0
r4k_pipe_speed:
	.word 	0
r4k_count_period_ns:
	.word	0
level2_cache_present:
	.word	0

/*
 * STORAGE DEFINITION REMINDER FOR NEVADA LOOKUP TABLE:
 *
 * extern struct levels {
 *	ulong s_reg;
 *	ulong ext_mask;
 * } r4k_level_table[8];
 */
	.globl	r4k_level_table
	.globl	interrupt_jump_table

	.text
	.align 2

/*
 * r4k_cpu_level_init:
 *
 * Initialize the level mechanism for R4k processors
 */

 #
 # void r4k_cpu_level_init (void)
 #
FRAME(r4k_cpu_level_init,sp,0,ra)
	.set	noreorder

	mfc0	t1,CP0_SREG		# get status register
	li	t2,~SREG_IE		# get mask for Interrupt Enable bit
	and	t2,t2,t1		# mask out IE bit
	mtc0	t2,CP0_SREG		# write back status register
					# (turning off interrupts)
	nop				# let status register settle

	li	t4,(LEVEL7 << R4K_LEVEL_SHIFT) # get level 7 constant
	sw	t4,r4k_cpu_level	# write it to the cpu level global

	/* Lookup status register bits and external mask for this level. */

	la	t0,r4k_level_table	# get level table address
	addu	t0,t4,t0		# get address of entries
	lw	t3,0(t0)		# load new status register bits
	lbu	t4,4(t0)		# load new external mask

	/* Store new external mask into external controller */

	la	t5,IO_IMASK_REG		# get mask reg address
	sb	t4,0(t5)		# set mask register

	/* Insert new status bits into processor status register */

	li	t6,~SREG_IMASK		# get mask for all 
	and	t1,t1,t6		# mask out all IP bits
	or	t1,t1,t3		# insert new IP bits

	lb	t4,0(t5)		# read back external reg. This is
					# needed to ensure that the
					# write buffering does not delay
					# setting of the external mask
					# past writing the status register

	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)
	
	.set	reorder
ENDFRAME(r4k_cpu_level_init)

/*
 * set_interrupt_level:
 *
 * Set interrupt level to specified value. Return current level in bits
 * 5..3 format. It sets the mask with all interrupts turned off.
 */

 #
 # leveltype set_interrupt_level (leveltype newlevel)
 #
FRAME(set_interrupt_level,sp,0,ra)
	.set	noreorder

	mfc0	t1,CP0_SREG		# get status register
	li	t2,~SREG_IE		# get mask for interrupt enable bit
	and	t2,t2,t1		# mask out interrupt enable bit
	mtc0	t2,CP0_SREG		# write back status register
					# (turning off interrupts)
	nop				# let status register settle

	lw	v0,r4k_cpu_level	# read old interrupt level
	andi	a0,a0,0x07		# make sure parameter is ok
	sll	a0,a0,R4K_LEVEL_SHIFT	# shift new level left 3
	la	t0,r4k_level_table	# get table address

	beq	a0,v0,no_change		# bail if it didn''t change
	addu	t2,t0,a0		# get new entry address (bd slot)

#if defined(R4K_STATIC_EXTERNAL_INTR_CONTROLLER)

	/*
	 * If this platform has a static external interrupt controller
	 * IE the state of the external interrupt controller does not change
	 * as the interrupt level goes up and down (like Predator)
	 * we do not have to do so much processing
	 */
	lw	t0,0(t2)		# read new status reg mask
	sw	a0,r4k_cpu_level	# save new level

not_external:
	li	t3,~SREG_IMASK		# get mask for all IP bits
	and	t1,t1,t3		# mask out all IP bits
	or	t1,t1,t0		# insert new IP bits

no_change:
	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one

#else	/* Dynamic external interrupt controller */

	/*
	 * This platform has a dynamic external interrupt controller
	 * IE the external interrupt mask changes as the CPU's
	 * interrupt level goes up and down.
	 */
	addu	a2,t0,v0		# get old entry address

	lbu	a2,4(a2)		# read old external mask
	lbu	t4,4(t2)		# read new external mask
	lw	t0,0(t2)		# read new status reg mask

	beq	a2,t4,not_external	# dont change external mask
	sw	a0,r4k_cpu_level	# save new level (bd slot)

	la	t5,IO_IMASK_REG		# get external mask reg address
	sb	t4,0(t5)		# set external mask register

	li	t3,~SREG_IMASK		# get mask for all IP bits
	and	t1,t1,t3		# mask out all IP bits

	/*
	 * If the new status register has the external interrupt
	 * controller pin negated, we do not have to 
	 * read back the external interrupt mask register, saving us
	 * some time.
	 */
	andi	t6,t0,CAUSE_EXT_BIT	# mask external interrupt cont. bit
	bne	t6,zero,sync_write	# are external interrupts enabled ?
	or	t1,t1,t0		# insert new IP bits (bd slot)

	/*
	 * Nevada interrupts are all disabled
	 */
	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

sync_write:
	/*
	 * External interrupts are enabled and the external
	 * interrupt mask register has changed, so we must sync
	 * the write cycle to the external interrupt controller
	 */
	lbu	t4,0(t5)		# read back extrnal mask. This is
					# needed to ensure that the
					# write buffering does not delay
					# setting of the external mask
					# past writing the Status register

	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

not_external:
	li	t3,~SREG_IMASK		# get mask for all IP bits
	and	t1,t1,t3		# mask out all IP bits
	or	t1,t1,t0		# insert new IP bits

no_change:
	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

#endif /* R4K_STATIC_EXTERNAL_INTR_CONTROLLER */

	.set	reorder
ENDFRAME(set_interrupt_level)
	
/*
 * get_interrupt_level:
 *
 * Return the present interrupt mask level
 * Returns level in [0 to 7] format
 */

 #
 # leveltype get_interrupt_level (void)
 #
FRAME(get_interrupt_level,sp,0,ra)
	.set	noreorder
	
	/*
	 * Return r4k_cpu_level in bits[2..0] format
	 */
	lw	v0,r4k_cpu_level	# get present interrupt level
	j	ra			# return
	srl	v0,v0,R4K_LEVEL_SHIFT	# shift right 3 (bd slot)

	.set	reorder
ENDFRAME(get_interrupt_level)

/*
 * raise_interrupt_level:
 *
 * Raise processor level to specified level, or keep it
 * there if it is that high already. Return present level in bits [5..3]
 */

 #
 # leveltype raise_interrupt_level (leveltype newlevel)
 #
FRAME(raise_interrupt_level,sp,0,ra)
	.set	noreorder
	
	lw	t1,r4k_cpu_level	# get current level
	sll	t0,a0,R4K_LEVEL_SHIFT	# shift left 3
	slt	t2,t1,t0		# t2 = 1 if r4k_cpu_level < newlevel
	
	beq	t2,zero,set_no_change	# raise the level ?
	move	v0,t1			# setup return value (bd slot)

	j	set_interrupt_level
	nop

set_no_change:
	j	ra			# return
	nop				# bd slot
	
	.set	reorder
ENDFRAME(raise_interrupt_level)

/*
 * reset_interrupt_level:
 *
 * Re-instate a level previously returned by raise_interrupt_level.
 */

 #
 # void reset_interrupt_level (leveltype level)
 #
FRAME(reset_interrupt_level,sp,0,ra)
	.set	noreorder

	mfc0	t1,CP0_SREG		# get status register
	li	t2,~SREG_IE		# get mask for interrupt enable bit
	and	t2,t2,t1		# get status reg with IE off
	mtc0	t2,CP0_SREG		# set status reg, intr locked out
	nop				# let status register settle

	lw	t0,r4k_cpu_level	# get current level
	la	t4,r4k_level_table	# get table address
	addu	t3,a0,t4		# get new level table entry address

#if defined(R4K_STATIC_EXTERNAL_INTR_CONTROLLER)

	/*
	 * If this platform has a static external interrupt controller
	 * IE the state of the external interrupt controller does not change
	 * as the interrupt level goes up and down (like Predator)
	 * we do not have to do so much processing
	 */
	lw	a1,0(t3)		# get new status reg
	sw	a0,r4k_cpu_level	# save new level

not_external1:
	li	t3,~SREG_IMASK		# get mask for all IP bits
	and	t1,t1,t3		# mask out all IP bits
	or	t1,t1,a1		# insert new IP bits

	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

#else	/* Dynamic external interrupt controller */

	/*
	 * This platform has a dynamic external interrupt controller
	 * IE the external interrupt mask changes as the CPU's
	 * interrupt level goes up and down.
	 */
	addu	t5,t0,t4		# get current level table entry address

	lbu	t5,4(t5)		# get current external mask	
	lbu	a2,4(t3)		# get new external mask
	lw	a1,0(t3)		# get new status reg

	beq	t5,a2,not_external1	# don''t change external mask
	sw	a0,r4k_cpu_level	# save new level

	la	t5,IO_IMASK_REG		# get external mask reg address
	sb	a2,0(t5)		# set external mask register

	li	t3,~SREG_IMASK		# get mask for all IP bits
	and	t1,t1,t3		# mask out all IP bits

	/*
	 * If the new status register has the external interrupt
	 * bit negated, we do not have to 
	 * read back the external interrupt mask register, saving us
	 * some time.
	 */
	andi	t6,a1,CAUSE_EXT_BIT	# mask external interrupt cont. not
	bne	t6,zero,sync_write1	# are external interrupts enabled ?
	or	t1,t1,a1		# insert new IP bits (bd slot)

	/*
	 * External interrupts are all disabled
	 */
	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

sync_write1:
	/*
	 * External interrupts are enabled and the external
	 * interrupt mask register has changed, so we must sync
	 * the write cycle to external interrupt controller
	 */
	lbu	t4,0(t5)		# read back mask reg. This is
					# needed to ensure that the
					# write buffering does not delay
					# setting of the external Mask
					# past writing the Status register

	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

not_external1:
	li	t3,~SREG_IMASK		# get mask for all IP bits
	and	t1,t1,t3		# mask out all IP bits
	or	t1,t1,a1		# insert new IP bits

	j	ra			# return
	mtc0	t1,CP0_SREG		# set the status register
					# set new mask if there is one
					# and re-instate IE bit (bd slot)

#endif /* R4K_STATIC_EXTERNAL_INTR_CONTROLLER */

	.set	reorder
ENDFRAME(reset_interrupt_level)

/*
 * Profiling variables
 */
	.globl	mcount_handler
	.globl	mcount_recurse_count
	.globl	mcount_count

	.sdata
	.align	2

mcount_handler:
	.word	0			# pointer to handler

mcount_recurse_count:
	.word	0			# recusion count

mcount_count:
	.word	0			# total call count

	.text
	.align	2

/*
 * mcount - Low level interface for profiling
 * At the start of every routine compiled with -p flag,
 * the following code exists :
 *
 * 	move	at,ra
 *	jal	_mcount
 * 	addiu	sp,sp,-8
 *
 * Note : Profiling may "break" the stack dumping abilities of
 *	  both the monitor and the system. Needs investigation !!
 *	  For the time being, just increment the counter and return
 *
 */

 #
 # void mcount (void)
 #
FRAME(_mcount,sp,0,ra)
	.set	noreorder

	la	t0,mcount_count		# get mcount_count address
	lw	t1,0(t0)		# read mcount
	addiu	t1,t1,1			# increment count

	j	ra			# return
	sw	t1,0(t0)		# save new count (bd slot)

	.set	reorder
ENDFRAME(_mcount)

/*
 * exception_code_start
 *
 * The following code gets copied to low core when the system
 * initializes. It is responsible for dereferencing through
 * the exception table to the handler
 */

 #
 # ulong exception_code_start[]
 #
FRAME(exception_code_start,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * TLB error handler : 0x80000000
	 */
	dmtc1	AT,FPU_R2		# save AT
	dmtc1	k0,FPU_R0		# save k0
	dmtc1	k1,FPU_R1		# save k1

	li	AT,R4K_EXCEPTION_TABLE
	lw	AT,0x84(AT)		# get handler address

	mfc0	k0,CP0_CAUSE		# read cause register
	mfc0	k1,CP0_SREG		# read status register

	j	AT			# jump to the handler
	andi	k1,k1,CAUSE_IMASK	# get IP bits of status reg (bd slot)

	.space 0x58			# pad with nops

	/*
	 * XTLB error handler : 0x80000080
	 */
	dmtc1	AT,FPU_R2		# save AT
	dmtc1	k0,FPU_R0		# save k0
	dmtc1	k1,FPU_R1		# save k1

	li	AT,R4K_EXCEPTION_TABLE
	lw	AT,0x88(AT)		# get handler address

	mfc0	k0,CP0_CAUSE		# read cause register
	mfc0	k1,CP0_SREG		# read status register

	j	AT			# jump to the handler
	andi	k1,k1,CAUSE_IMASK	# get IP bits of status reg (bd slot)
	
	.space 0x58			# pad with nops

	/*
	 * Cache error handler : 0x80000100
	 */
	dmtc1	AT,FPU_R2		# save AT
	dmtc1	k0,FPU_R0		# save k0
	dmtc1	k1,FPU_R1		# save k1

	/*
	 * Must turn off ERL here, otherwise xkuseg becomes a 2 power-of 31
	 * uncached, unmapped address space which will then cause a TLB
	 * miss on the j AT below
	 */
	mfc0	AT,CP0_SREG		# read status register

	addiu	k0,zero,~SREG_IE	# get IE mask
	and	AT,AT,k0		# mask off IE
	mtc0	AT,CP0_SREG		# write sreg
	nop				# let it settle

	addiu	k0,zero,~SREG_ERL	# get ERL mask
	and	AT,AT,k0		# mask off ERL
	mtc0	AT,CP0_SREG		# write sreg
	nop				# let it settle

	li	AT,R4K_EXCEPTION_TABLE
	lw	AT,0x80(AT)		# get handler address

	mfc0	k0,CP0_CAUSE		# read cause register
	mfc0	k1,CP0_SREG		# read status register

	j	AT			# jump to the handler
	andi	k1,k1,CAUSE_IMASK	# get IP bits of status reg (bd slot)
	
	.space 0x34			# pad with nops

	/*
	 * General exception handler : 0x80000180
	 */
	dmtc1	AT,FPU_R2		# save AT
	dmtc1	k0,FPU_R0		# save k0
	dmtc1	k1,FPU_R1		# save k1

	mfc0	k0,CP0_CAUSE		# read Cause register

	la	AT,R4K_EXCEPTION_TABLE	# get table address
	andi	k1,k0,CAUSE_MASK	# get Cause [5..0]
	addu	AT,AT,k1		# get address in table
	lw	AT,0(AT)		# read the handler address

	mfc0	k1,CP0_SREG		# read status register

	j	AT			# call the handler
	andi	k1,k1,CAUSE_IMASK	# get status reg IP bits (bd slot)

	.space 0x10			# pad with nops

	/*
 	 * Exception Table : 0x800001C0
	 */
	.globl	r4k_exception_table

r4k_exception_table:

	.word	r4k_interrupt_handler	/* 0  : interrupt		*/
	.word	r4k_exception_handler	/* 1  : TLB modification	*/
	.word	r4k_exception_handler	/* 2  : TLB exception (load)	*/
	.word	r4k_exception_handler	/* 3  : TLB exception (store)	*/
	.word	r4k_exception_handler	/* 4  : Address error (load)	*/
	.word	r4k_exception_handler	/* 5  : Address error (store)	*/
	.word	r4k_exception_handler	/* 6  : Bus Error (instr) 	*/
	.word	r4k_exception_handler	/* 7  : Bus Error (data)	*/
	.word	r4k_exception_handler	/* 8  : System call		*/
	.word	r4k_exception_handler	/* 9  : Breakpoint		*/
	.word	r4k_exception_handler	/* 10 : Illegal op		*/
	.word	r4k_exception_handler	/* 11 : Coprocessor unusable	*/
	.word	r4k_exception_handler	/* 12 : Arithmetic overflow	*/
	.word	r4k_exception_handler	/* 13 : Trap			*/
	.word	r4k_exception_handler	/* 14 : Reserved		*/
	.word	r4k_exception_handler	/* 15 : Floating FPU exception	*/
	.word	r4k_exception_handler	/* 16 : Reserved		*/
	.word	r4k_exception_handler	/* 17 : Reserved		*/
	.word	r4k_exception_handler	/* 18 : Reserved		*/
	.word	r4k_exception_handler	/* 19 : Reserved 		*/
	.word	r4k_exception_handler	/* 20 : Reserved		*/
	.word	r4k_exception_handler	/* 21 : Reserved		*/
	.word	r4k_exception_handler	/* 22 : Reserved		*/
	.word	r4k_exception_handler	/* 23 : Reserved		*/
	.word	r4k_exception_handler	/* 24 : Reserved		*/
	.word	r4k_exception_handler	/* 25 : Reserved		*/
	.word	r4k_exception_handler	/* 26 : Reserved		*/
	.word	r4k_exception_handler	/* 27 : Reserved		*/
	.word	r4k_exception_handler	/* 28 : Reserved		*/
	.word	r4k_exception_handler	/* 29 : Reserved		*/
	.word	r4k_exception_handler	/* 30 : Reserved		*/
	.word	r4k_exception_handler	/* 31 : Reserved		*/
	.word	r4k_cache_handler	/* 32 : Cache Error		*/
	.word	r4k_tlb_handler		/* 33 : TLB exception		*/
	.word	r4k_xtlb_handler	/* 34 : XTLB exception		*/

	/*
	 * Pointer to crashinfo data structure. This is here to provide
	 * a constant location within a core file of the pointer to
	 * the crash information
	 */
	.space 0x04
	.globl r4k_crash_info_ptr

r4k_crash_info_ptr:

	.word	crash_ptr		/* crashinfo pointer (0x80000250) */

	.set 	reorder
	.set	at
ENDFRAME(exception_code_start)
	
/*
 * Marker to get the size of the exception handling code
 */

 #
 # ulong exception_code_end[]
 #
	.globl	exception_code_end
	.text
	.align 2	

exception_code_end:
	.word	0

/*
 * r4k_fake_parity_error:
 * Little fake parity error generator
 */
FRAME(r4k_fake_parity_error,sp,0,ra)
	.set	noreorder
	.set	noat

	mfc0	t0,CP0_SREG		# read status register
	la	t1,R4K_K1BASE+0x100	# load parity exception handler addr.
	ori	t0,t0,SREG_ERL		# set ERL in copy

	/*
	 * Jump to the handler, setting the ERL bit on in the status
	 * register.
	 */
	j	t1			# jump to handler
	mtc0	t0,CP0_SREG		# write copy to sreg (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_fake_parity_error)

/*
 * r4k_cache_handler:
 *
 * Handles cache error exceptions
 *
 * Entry:      k0 is saved in FPU_R0
 *	       k1 is saved in FPU_R1
 *	       AT is saved in FPU_R2
 *	       k0 has Cause register
 *	       k1 has Status [IP] field mask
 */

FRAME(r4k_cache_handler,sp,0,ra)
	.set	noreorder
	.set	noat
	
	/*
	 * If we are not already on it, move onto the 
	 * exception stack, make a frame and call the signal dispatch
	 */
	move	AT,sp				# save copy of sp
	la	k0,exception_stack_guard	# get guard address
	subu	k0,AT,k0			# k0 = AT - k0
	blez	k0,move_stack1			# must we move stacks ?
	nop					# (bd slot)

	la	k0,exception_stack		# get stack address
	subu	k0,k0,AT			# k0 = k0 - AT
	blez	k0,move_stack1			# must we move stacks ?
	nop					# (bd slot)

	b	nomove1				# do not move the stack
	nop					# (bd slot)

move_stack1:
	la	sp,exception_stack		# get new stack pointer

nomove1:
	la	k0,r4k_context_space		# get context space
	sd	AT,SP_OFS(REG_SP)(k0)		# save old sp

	/*
	 * Set up parameters for signal dispatch
	 */
	mfc0	AT,CP0_CACHERR			# read cache error register

	j	r4k_sig_dispatch		# call signal dispatcher
	ori	k1,zero,SIGCACHE		# cache error signal (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_cache_handler)

/*
 * r4k_tlb_handler:
 *
 * Handles TLB miss exceptions
 *
 * Entry:      k0 is saved in FPU_R0
 *	       k1 is saved in FPU_R1
 *	       AT is saved in FPU_R2
 *	       k0 has Cause register
 *	       k1 has Status [IP] field masked
 */

FRAME(r4k_tlb_handler,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * If we are not on it already, move onto the exception stack, 
	 * make a frame and call the signal dispatch
	 */
	move	AT,sp				# save copy of sp
	la	k0,exception_stack_guard	# get guard address
	subu	k0,AT,k0			# k0 = AT - k0
	blez	k0,move_stack2			# must we move stacks ?
	nop					# (bd slot)

	la	k0,exception_stack		# get stack address
	subu	k0,k0,AT			# k0 = k0 - AT
	blez	k0,move_stack2			# must we move stacks ?
	nop					# (bd slot)

	b	nomove2				# do not move the stack
	nop					# (bd slot)

move_stack2:
	la	sp,exception_stack		# get new stack pointer

nomove2:
	la	k0,r4k_context_space		# get context space
	sd	AT,SP_OFS(REG_SP)(k0)		# save old sp

	/*
	 * Set up parameters for signal dispatch
	 */
	mfc0	AT,CP0_CAUSE			# reread Cause register

	j	r4k_sig_dispatch		# call signal dispatcher
	ori	k1,zero,SIGSEGV			# Seg Violation (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_tlb_handler)

/*
 * r4k_xtlb_handler:
 *
 * Handles xtlb miss exceptions
 *
 * Entry:      k0 is saved in FPU_R0
 *	       k1 is saved in FPU_R1
 *	       AT is saved in FPU_R2
 *	       k0 has Cause register
 *	       k1 has Status [IP] field masked
 */

FRAME(r4k_xtlb_handler,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * If we are not on it already, move onto the exception stack, 
	 * make a frame and call the signal dispatch
	 */
	move	AT,sp				# save copy of sp
	la	k0,exception_stack_guard	# get guard address
	subu	k0,AT,k0			# k0 = AT - k0
	blez	k0,move_stack3			# must we move stacks ?
	nop					# (bd slot)

	la	k0,exception_stack		# get stack address
	subu	k0,k0,AT			# k0 = k0 - AT
	blez	k0,move_stack3			# must we move stacks ?
	nop					# (bd slot)

	b	nomove3				# do not move the stack
	nop					# (bd slot)

move_stack3:
	la	sp,exception_stack		# get new stack pointer

nomove3:
	la	k0,r4k_context_space		# get context space
	sd	AT,SP_OFS(REG_SP)(k0)		# save old sp

	/*
	 * Set up parameters for signal dispatch
	 */
	mfc0	AT,CP0_CAUSE			# reread Cause register

	j	r4k_sig_dispatch		# call signal dispatcher
	ori	k1,zero,SIGSEGV			# Seg Violation (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_xtlb_handler)

/*
 * r4k_exception_handler:
 *
 * Handles all other exceptions
 *
 * Entry     : k0 is saved in FPU_R0
 *	       k1 is saved in FPU_R1
 *	       AT is saved in FPU_R2
 *	       k0 has Cause register
 *	       k1 has Status [IP] field masked
 */

FRAME(r4k_exception_handler,sp,0,ra)
	.set	noreorder
	.set	noat

	.globl	r4k_cause_sig_map

	/*
	 * If we are not on it already, move onto the exception stack, 
	 * make a frame and call the signal dispatch
	 */
	move	k1,k0				# copy cause register
	move	AT,sp				# save copy of sp

	la	k0,exception_stack_guard	# get guard address
	subu	k0,AT,k0			# k0 = AT - k0
	blez	k0,move_stack4			# must we move stacks ?
	nop					# (bd slot)

	la	k0,exception_stack		# get stack address
	subu	k0,k0,AT			# k0 = k0 - AT
	blez	k0,move_stack4			# must we move stacks ?
	nop					# (bd slot)

	b	nomove4				# do not move the stack
	nop					# (bd slot)

move_stack4:
	la	sp,exception_stack		# get new stack pointer

nomove4:
	la	k0,r4k_cause_sig_map		# get cause/signal map address
	andi	k1,k1,CAUSE_MASK		# and out cause bits
	addu	k1,k1,k0			# get address of signal

	la	k0,r4k_context_space		# get context space
	sd	AT,SP_OFS(REG_SP)(k0)		# save old sp
	nop					# load delay

	/*
	 * Set up parameters for signal dispatch
	 */
	move	AT,k1				# move signal address to AT
	lw	k1,0(AT)			# get signal number
	nop					# load delay

	mfc0	AT,CP0_CAUSE			# re-read cause register
	
	j	r4k_sig_dispatch		# call signal dispatcher
	andi	AT,AT,CAUSE_MASK		# code = cause[0..5] (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_exception_handler)

/*
 * statically allocated exception stack
 */
	.data
	.align	3

exception_stack_guard:
	.word	-1			# bottom of our stack
	.word	-1

	.space	0x800			# 2K worth of space for stack
exception_stack:			# which grows down
	.word	-1
	.word	-1			# top of stack

	.extern exception_stack_guard
	.extern exception_stack

	.text
	.align 	2

/*
 * r4k_interrupt_handler:
 *
 * Overall interrupt handler for R4k systems.
 * Priority of interrupts is done by software. See the 
 * interrupt_jump_table in the platform specific file
 * for the prioritization of the interrupts
 *
 * Entry:      k0 is saved in FPU_R0
 *	       k1 is saved in FPU_R1
 *	       AT is saved in FPU_R2
 *	       k0 has Cause register
 *	       k1 has Status [IP] field masked
 */

 #
 # void r4k_interrupt_handler (void)
 #
FRAME(r4k_interrupt_handler,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Only 1 interrupt source gets served per invocation
	 */
	lw	AT,r4k_intr_rec_count	# read count
	addiu	AT,AT,1			# increment count
	sw	AT,r4k_intr_rec_count	# save count

	and	AT,k1,k0		# get valid IP bits
	la	k1,interrupt_jump_table	# get address of jump table
	andi	AT,AT,(~CAUSE_IP01_MASK & 0xffff)	
	srl	AT,AT,CAUSE_ISHIFT	# shift right by 8 and mask
					# out INT0 & INT1 (sw interrupts)

	addu	k1,k1,AT		# get address of table entry
	lwu	k1,0(k1)		# get handler address from table

	move	AT,sp			# copy sp to AT

	j	k1			# call the handler
	move	k1,zero			# zero out k1 (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_interrupt_handler)

/*
 * r4k_nmi_intr:
 *
 * Handle internal timer interrupts.  This is at NMI level and IE will
 * never be disabled for most boxes. On the distributed IOS IP's for
 * the 7500 and 7000, this can be interrupted by DBus, Malu Attention
 *
 * Note: write bus error interrupts are masked while this handler executes.
 *
 * Note-2: This is *not* the R4k hardware NMI.  It is the s/w "NMI" tick.
 */

FRAME(r4k_nmi_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the "NMI_STACK" stack
	 * It dereferences off "NMI_SUBR" subr to the wrapper.
	 * Note: sp has already been copied to AT
	 */
	lw	sp,NMI_STACK			# move onto "NMI" stack
	la	k1,NMI_COUNTER			# get address of "NMI" counter
	lw	k0,0(k1)			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,0(k1)			# save the counter
	
	sd	v0,SP_OFS(REG_V0)(sp)		# save v0
	sd	v1,SP_OFS(REG_V1)(sp)		# save v1
	sd 	a0,SP_OFS(REG_A0)(sp)		# save a0
	sd 	a1,SP_OFS(REG_A1)(sp)		# save a1
	sd 	a2,SP_OFS(REG_A2)(sp)		# save a2
	sd 	a3,SP_OFS(REG_A3)(sp)		# save a3
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp)	# save copy of k0
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp)	# save copy of k1
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp)	# save copy of AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	sd	t0,SP_OFS(REG_EPC)(sp)		# save EPC reg
	sw	t1,SP_OFS(REG_SREG)(sp)		# save Status reg
	sd	t2,SP_OFS(REG_LO)(sp)		# save LO
	sd	t3,SP_OFS(REG_HI)(sp)		# save HI
	sd	t4,SP_OFS(REG_T4)(sp)		# save t4
	sd	t5,SP_OFS(REG_T5)(sp)		# save t5
	sd	t6,SP_OFS(REG_T6)(sp)		# save t6
	sd	t7,SP_OFS(REG_T7)(sp)		# save t7
	sd	t8,SP_OFS(REG_T8)(sp)		# save t8
	sd	t9,SP_OFS(REG_T9)(sp)		# save t9
	sd	ra,SP_OFS(REG_RA)(sp)		# save ra

	/*
	 * Get the wrapper routine address and call it.
	 * We have not de-asserted EXL or installed our level
	 * priorities or masks, so interrupts are still locked out. 
	 * Count/Compare interrupt source is still active. The 
	 * wrapper needs to install the appropriate level and de-assert
	 * EXL. (We allow reentrant NMI interrupts but they will cause
	 * the box to crash.  This is a feature.)
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	lw	t4,NMI_SUBR			# read wrapper address
	move	a1,t0				# setup param 2 (EPC reg)

	jal	t4				# call the wrapper
	move	a0,k0				# setup param 1 (bd slot)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * sections.
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	ld	v0,SP_OFS(REG_V0)(sp)		# restore v0
	ld	v1,SP_OFS(REG_V1)(sp)		# restore v1
	ld 	a0,SP_OFS(REG_A0)(sp)		# restore a0
	ld 	a1,SP_OFS(REG_A1)(sp)		# restore a1
	ld 	a2,SP_OFS(REG_A2)(sp)		# restore a2
	ld 	a3,SP_OFS(REG_A3)(sp)		# restore a3

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0
	ld	t1,SP_OFS(REG_T1)(sp)		# restore t1
	ld	t2,SP_OFS(REG_T2)(sp)		# restore t2
	ld	t3,SP_OFS(REG_T3)(sp)		# restore t3

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg (EXL=1,IE=1)
	nop					# let sreg settle

	ld	t4,SP_OFS(REG_EPC)(sp)		# load EPC reg
	ld	t6,SP_OFS(REG_LO)(sp)		# load LO
	ld	t7,SP_OFS(REG_HI)(sp)		# load HI

	dmtc0	t4,CP0_EPC			# restore EPC reg
	mtlo	t6				# restore LO
	mthi	t7				# restore HI

	ld	t4,SP_OFS(REG_T4)(sp)		# restore t4
	ld	t5,SP_OFS(REG_T5)(sp)		# restore t5
	ld	t6,SP_OFS(REG_T6)(sp)		# restore t6
	ld	t7,SP_OFS(REG_T7)(sp)		# restore t7
	ld	t8,SP_OFS(REG_T8)(sp)		# restore t8
	ld	t9,SP_OFS(REG_T9)(sp)		# restore t9
	ld	ra,SP_OFS(REG_RA)(sp)		# restore ra

	/*
	 * We need to restore the interrupted level and we must 
	 * re-instate the correct external mask for that level.
	 * Once it has been guaranteed that no set_interrupt_level/
	 * reset_interrupt_level calls get made during NMI processing, 
	 * we can optimize out the write to the external interrupt mask
	 * register.
	 */
	sw	k0,r4k_cpu_level	# restore old level
	la	k1,r4k_level_table	# get level table address
	addu	k1,k1,k0		# compute address of table entry
	lbu	AT,4(k1)		# read external mask from, table
	la	k0,IO_IMASK_REG		# get intr mask reg addr
	sb	AT,0(k0)		# set the mask
					# Note: we dont have to sync this
					# as we will always be enabling
					# something rather than disabling
					# something, which can happen
					# whenever

	lw	AT,r4k_intr_rec_count	# read interrupt recursion count
	ld	k1,SP_OFS(REG_K1)(sp)	# restore k1
	addiu	AT,AT,-1		# decrement count
	sw	AT,r4k_intr_rec_count	# store new interrupt recursion count

	ld	k0,SP_OFS(REG_K0)(sp)	# restore k0
	ld	AT,SP_OFS(REG_AT)(sp)	# restore AT

	/*
	 * move back onto the original stack
	 * We dont have to increment the stack pointer
	 */
	ld	sp,SP_OFS(REG_SP)(sp)	# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret				# return
	nop				# does not get executed

	.set	reorder
	.set	at
ENDFRAME(r4k_nmi_intr)

/*
 * r4k_spur_intr:
 *
 * Handle software interrupts 0 and 1, as well as spurious interrupts.
 * On some platforms (Checkers), sw 1 is used. On these platforms,
 * a platform handler calls into this routine.
 */
	.globl	r4k_spurious_callback

FRAME(r4k_spur_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine moves onto the exception stack. sw interrupt
	 * 0 is the gdb mechanism for debugging a process.
	 * Note : sp has already been copied to AT
	 */
	mfc0	k1,CP0_SREG			# re-read status register
	nop					# load delay

r4k_spurious_callback:

	andi	k1,k1,CAUSE_IMASK		# get interrupt enable bits
	and	k1,k1,k0			# get interrupt asserted bits

	la	k0,exception_stack_guard	# get guard address
	subu	k0,AT,k0			# k0 = AT - k0
	blez	k0,move_stack7			# must we move stacks ?
	nop					# (bd slot)

	la	k0,exception_stack		# get stack address
	subu	k0,k0,AT			# k0 = k0 - AT
	blez	k0,move_stack7			# must we move stacks ?
	nop					# (bd slot)

	b	nomove7				# do not move the stack
	nop					# (bd slot)

move_stack7:
	la	sp,exception_stack		# get new stack pointer

nomove7:
	la	k0,r4k_context_space		# get context space
	sd	AT,SP_OFS(REG_SP)(k0)		# save old sp

	/*
	 * check for s\w interrupt 0
	 */
	andi	AT,k1,CAUSE_IBIT0		# check for sw interrupt 0
	beq	AT,zero,not_sw_0		# branch if not
	nop					# (bd slot)

	/*
	 * De-assert sw interrupt 0 and call 
	 * the signal dispatcher
	 */
	mfc0	k1,CP0_CAUSE			# read cause register
	li	AT,~CAUSE_IBIT0			# get sw 0 mask
	and	k1,k1,AT			# get new cause register
	mtc0	k1,CP0_CAUSE			# write new cause register
	nop					# load delay

	ori	k1,zero,SIGTRAP			# s/w 0 interrupt (breakpoint)
	j	r4k_sig_dispatch		# call signal dispatcher
	move	AT,zero				# setup code of zero (bd slot)

not_sw_0:
	/*
	 * check for s\w interrupt 1
	 */
	andi	AT,k1,CAUSE_IBIT1		# check for sw interrupt 1
	beq	AT,zero,not_sw_1		# branch if not
	nop					# (bd slot)

	/*
	 * De-assert sw interrupt 1 and call 
	 * the signal dispatcher
	 */
	mfc0	k1,CP0_CAUSE			# read cause register
	li	AT,~CAUSE_IBIT1			# get sw 1 mask
	and	k1,k1,AT			# get new cause register
	mtc0	k1,CP0_CAUSE			# write new cause register
	nop					# load delay

	ori	k1,zero,SIGUSR2			# s\w 1 interrupt
	j	r4k_sig_dispatch		# call signal dispatcher
	move	AT,zero				# setup code of zero (bd slot)

not_sw_1:
	/*
	 * We took a spurious interrupt
	 * increment the spurious interrupt count
	 */
	lw	k1,spurcnt		# k1 = spurcnt
	ld	sp,SP_OFS(REG_SP)(k0)	# get old stack pointer
	addiu	k1,k1,1			# increment spurious count
	sw	k1,spurcnt		# spurcnt = k1

	/*
	 * Decrement interrupt recursion count
	 */
	lw	AT,r4k_intr_rec_count	# read count
	addiu	AT,AT,-1		# decrement by one
	sw	AT,r4k_intr_rec_count	# save new count

	/*
	 * restore context and return
	 */
	dmfc1	AT,FPU_R2		# restore AT
	dmfc1	k1,FPU_R1		# restore k1
	dmfc1	k0,FPU_R0		# restore k0

	eret				# return from exception
	nop				# does not get executed

	.set	reorder
	.set	at
ENDFRAME(r4k_spur_intr)

/*
 * r4k_sig_dispatch:
 *
 * Handle strange events that require a signal handler to be
 * dispatched.
 *
 * Entry:  Stack has been moved if required
 *	   Old stack pointer has been saved
 *	   k0 has context pointer
 *	   k1 has the signal number
 *	   AT has code
 */

FRAME(r4k_sig_dispatch,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This is most likely an error condition so we will 
	 * save complete context
	 */
	sd	v0,SP_OFS(REG_V0)(k0)		# save v0
	sd	v1,SP_OFS(REG_V1)(k0)		# save v1
	sd 	a0,SP_OFS(REG_A0)(k0)		# save a0
	sd 	a1,SP_OFS(REG_A1)(k0)		# save a1
	sd 	a2,SP_OFS(REG_A2)(k0)		# save a2
	sd 	a3,SP_OFS(REG_A3)(k0)		# save a3
	sd	t0,SP_OFS(REG_T0)(k0)		# save t0
	sd	t1,SP_OFS(REG_T1)(k0)		# save t1
	sd	t2,SP_OFS(REG_T2)(k0)		# save t2
	sd	t3,SP_OFS(REG_T3)(k0)		# save t3
	sd	t4,SP_OFS(REG_T4)(k0)		# save t4
	sd	t5,SP_OFS(REG_T5)(k0)		# save t5
	sd	t6,SP_OFS(REG_T6)(k0)		# save t6

	sdc1	FPU_R0,SP_OFS(REG_K0)(k0)	# save copy of k0
	sdc1	FPU_R1,SP_OFS(REG_K1)(k0)	# save copy of k1
	sdc1	FPU_R2,SP_OFS(REG_AT)(k0)	# save copy of AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	dmfc0	t4,CP0_ERR_EPC			# get Error EPC
	mfc0	t5,CP0_CAUSE			# get Cause register
	dmfc0	t6,CP0_BADVADDR			# get Bad Vaddr

	sd	t0,SP_OFS(REG_EPC)(k0)		# save EPC reg
	sw	t1,SP_OFS(REG_SREG)(k0)		# save Status reg
	sd	t2,SP_OFS(REG_LO)(k0)		# save LO
	sd	t3,SP_OFS(REG_HI)(k0)		# save HI
	sd	t4,SP_OFS(REG_ERR_EPC)(k0)	# save Error EPC
	sw	t5,SP_OFS(REG_CAUSE)(k0)	# save Cause
	sd	t6,SP_OFS(REG_BADVADDR)(k0)	# save Bad Vaddr

	sd	t7,SP_OFS(REG_T7)(k0)		# save t7
	sd	t8,SP_OFS(REG_T8)(k0)		# save t8
	sd	t9,SP_OFS(REG_T9)(k0)		# save t9
	sd	ra,SP_OFS(REG_RA)(k0)		# save ra
	sd	s0,SP_OFS(REG_S0)(k0)		# save s0
	sd	s1,SP_OFS(REG_S1)(k0)		# save s1
	sd	s2,SP_OFS(REG_S2)(k0)		# save s2
	sd	s3,SP_OFS(REG_S3)(k0)		# save s3
	sd	s4,SP_OFS(REG_S4)(k0)		# save s4
	sd	s5,SP_OFS(REG_S5)(k0)		# save s5
	sd	s6,SP_OFS(REG_S6)(k0)		# save s6
	sd	s7,SP_OFS(REG_S7)(k0)		# save s7
	sd	s8,SP_OFS(REG_S8)(k0)		# save s8
	sd	zero,SP_OFS(REG_ZERO)(k0)	# save zero
	sd	gp,SP_OFS(REG_GP)(k0)		# save gp

	/*
	 * Setup the status register. We need to ensure that
	 * both EXL and ERL are de-asserted and IE is off.
	 * Hence we can take recursive exceptions but do not take any
	 * interrupts. t1 has a copy of the status register.
	 * We install Error Level interrupts that allows the 
	 * timer interrupt to be serviced so the watchdog does not bark.
	 *
	 * Note: The code used to de-assert all three bits with one
	 * write to the sreg. This caused the ORION to turn off CP1,
	 * and take a cache error exception (while running uncached
	 * with parity exceptions disabled!). Similar weirdness can probably
	 * occur on the R4400. So three writes to the sreg are used.
	 */
	addiu	t2,zero,~SREG_IE		# IE mask
	and	t1,t2,t1			# de-assert IE
	mtc0	t1,CP0_SREG			# write sreg
	nop					# let sreg settle

	addiu	t2,zero,~SREG_ERL		# ERL mask
	and	t1,t2,t1			# de-assert ERL
	mtc0	t1,CP0_SREG			# write sreg
	nop					# let sreg settle

	addiu	t2,zero,~SREG_EXL		# EXL mask
	and	t1,t2,t1			# de-assert EXL
	mtc0	t1,CP0_SREG			# load new status register
	nop					# let sreg settle

	/*
	 * Setup parameters to the signal handler
	 */
	move	a0,k1				# setup signal number
	move	a1,AT				# setup code
	move	a2,k0				# setup context pointer

	/*
	 * Install Error Level interrupt masks if present level is lower
	 */
	lw	k1,r4k_cpu_level		# get current CPU level
	mfc0	t1,CP0_SREG			# read status register
	sltu	t0,k1,(SIG_LEVEL << R4K_LEVEL_SHIFT) # level < SIG_LEVEL
	
	beq	t0,zero,above_sig_level		# are we equal or above ?
	nop					# (bd slot)

	li	t3,(SIG_LEVEL << R4K_LEVEL_SHIFT) # get new level
	sw	t3,r4k_cpu_level		# set new level

	la	t0,r4k_level_table		# get level table address
	la	t2,IO_IMASK_REG			# get external register address
	lbu	t3,(4+(8*SIG_LEVEL))(t0)	# read "Sig Level" ext. mask
	lw	t4,(8*SIG_LEVEL)(t0)		# read "Sig Level" Status reg
	sb	t3,0(t2)			# write external mask

	li	t5,~CAUSE_IMASK			# get IP[7..0] mask
	and	t1,t1,t5			# mask out IP[7..0]
	or	t1,t1,t4			# "or" in new IP[7..0]
	ori	t1,t1,SREG_IE			# set IE
	lbu	t2,0(t2)			# read back (sync write)

	j	r4k_call_handler		# jump to r4k_call_handler
	mtc0	t1,CP0_SREG			# set new status register (bd)

above_sig_level:
	ori	t1,t1,SREG_IE			# enable IE
	mtc0	t1,CP0_SREG			# set new status register

r4k_call_handler:
	/*
	 * Call the signal handler
	 */
	jal	r4k_signal_handler		# call signal handler
	nop					# (bd slot)

r4k_sig_dispatch_return:
	/*
	 * If the handler resumes, or the user types cont at
	 * the monitor prompt, we will come back here.
	 * We restore the stacked status register which guarantees
	 * that EXL/ERL will be asserted through critical section
	 * and the state of the IE bit will be restored
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	ld	s0,SP_OFS(REG_S0)(k0)		# restore s0
	ld	s1,SP_OFS(REG_S1)(k0)		# restore s1
	ld	s2,SP_OFS(REG_S2)(k0)		# restore s2
	ld	s3,SP_OFS(REG_S3)(k0)		# restore s3
	ld	s4,SP_OFS(REG_S4)(k0)		# restore s4
	ld	s5,SP_OFS(REG_S5)(k0)		# restore s5
	ld	s6,SP_OFS(REG_S6)(k0)		# restore s6
	ld	s7,SP_OFS(REG_S7)(k0)		# restore s7
	ld	s8,SP_OFS(REG_S8)(k0)		# restore s8
	ld	gp,SP_OFS(REG_GP)(k0)		# restore gp

	ld	v0,SP_OFS(REG_V0)(k0)		# restore v0
	ld	v1,SP_OFS(REG_V1)(k0)		# restore v1
	ld 	a0,SP_OFS(REG_A0)(k0)		# restore a0
	ld 	a1,SP_OFS(REG_A1)(k0)		# restore a1
	ld 	a2,SP_OFS(REG_A2)(k0)		# restore a2
	ld 	a3,SP_OFS(REG_A3)(k0)		# restore a3

	lw	t5,SP_OFS(REG_SREG)(k0)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(k0)		# restore t0
	ld	t1,SP_OFS(REG_T1)(k0)		# restore t1
	ld	t2,SP_OFS(REG_T2)(k0)		# restore t2
	ld	t3,SP_OFS(REG_T3)(k0)		# restore t3

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg (EXL=1)
	nop					# let sreg settle

	/*
	 * We must now install the interrupt level that was in effect
	 * when we took the exception. It is stored in k1.
	 */
	la	t4,r4k_level_table		# get level table address
	addu	t4,t4,k1			# get address
	lbu	t5,4(t4)			# read external interrupt mask
	la	t6,IO_IMASK_REG			# get interrupt mask address
	sb	t5,0(t6)			# write new mask
	sw	k1,r4k_cpu_level		# re-instate level
	lbu	t5,0(t6)			# read back (sync write)

	ld	t4,SP_OFS(REG_EPC)(k0)		# load EPC reg
	ld	t6,SP_OFS(REG_LO)(k0)		# load LO
	ld	t7,SP_OFS(REG_HI)(k0)		# load HI
	ld	t8,SP_OFS(REG_ERR_EPC)(k0)	# load Error EPC reg

	dmtc0	t4,CP0_EPC			# restore EPC reg
	mtlo	t6				# restore LO
	mthi	t7				# restore HI
	dmtc0	t8,CP0_ERR_EPC			# restore Error EPC reg

	ld	t4,SP_OFS(REG_T4)(k0)		# restore t4
	ld	t5,SP_OFS(REG_T5)(k0)		# restore t5
	ld	t6,SP_OFS(REG_T6)(k0)		# restore t6
	ld	t7,SP_OFS(REG_T7)(k0)		# restore t7
	ld	t8,SP_OFS(REG_T8)(k0)		# restore t8
	ld	t9,SP_OFS(REG_T9)(k0)		# restore t9
	ld	ra,SP_OFS(REG_RA)(k0)		# restore ra

	move	sp,k0				# sp = k0

	ld	k1,SP_OFS(REG_K1)(sp)		# restore k1
	ld	k0,SP_OFS(REG_K0)(sp)		# restore k0
	ld	AT,SP_OFS(REG_AT)(sp)		# restore AT
	ld	sp,SP_OFS(REG_SP)(sp)		# restore old sp

	/*
	 * dismiss the exception/interrupt
	 */
	eret					# return
	nop					# not executed

	.set	reorder
	.set	at
ENDFRAME(r4k_sig_dispatch)

/*
 * return_from_exception:
 *
 * We get called when the system wants to return from an exception.
 * The CPU context will be in r4k_context_space.  We must setup the k0
 * pointer and jump to the r4k_sig_dispatch_return point above.
 *
 */

 #
 # void return_from_exception (void)
 #
FRAME(return_from_exception,sp,0,ra)
	.set	noreorder

	la	k0,r4k_context_space		# get context pointer
	j	r4k_sig_dispatch_return		# call the return point
	nop					# (bd slot)

	.set	reorder
ENDFRAME(return_from_exception)

/*
 * exception_jump:
 *
 * We are a running process that took a debuggable exception. We must
 * move off the exception stack back onto the process stack and call
 * the entry point. Our context has been saved in the r4k_context_space.
 *
 * The exception handling code has installed level 7 interrupt mask
 * priority. Call the entry point with this interrupt level.
 */

 #
 # exception_jump (void *new_pc)
 #
FRAME(exception_jump,sp,0,ra)
	.set	noreorder

	la	t1,r4k_context_space	# get context pointer
	ld	sp,SP_OFS(REG_SP)(t1)	# read old stack pointer

	j	a0			# jump to the new address
	nop

	.set	reorder
ENDFRAME(exception_jump)

/*
 * r4k_settlb:
 *
 * Program the selected TLB entry.
 *
 * NOTE: IF THIS ROUTINE CHANGES PLEASE CHECK THE DATA STRUCTURES FOR
 *       TLB IN CPU_R4K.H AND VICA VERSA.  
 *
 * On the Orion chip, the entry_hi,lo0,lo1 registers are all 64bit.
 *
 */

 #
 # void r4k_settlb (struct r4k_tlb_t *ptr, int index, boolean cache_flush)
 #
FRAME(r4k_settlb,sp,8,ra)
	.set	noreorder

	addiu	sp,sp,-8		# make a frame
	sw	ra,4(sp)		# save return address

	mfc0	t1,CP0_SREG		# t1 = sreg
	li	t2,~SREG_IE		# get IE bit mask
	sw	t1,0(sp)		# save old sreg
	and	t1,t1,t2		# t1 = sreg & ~SREG_IE
	mtc0	t1,CP0_SREG		# sreg = t1, turn off interrupts

	lw	t2,0(a0)		# t2 = ptr->pmask
	lw	t3,4(a0)		# t3 = ptr->hi
	lw	t4,8(a0)		# t4 = ptr->lo0
	lw	t5,12(a0)		# t5 = ptr->lo1
	mtc0	t2,CP0_PAGEMASK		# write Page Mask register
	dmtc0	t3,CP0_TLB_HI		# write Entry Hi register
	dmtc0	t4,CP0_TLB_LO_0		# write Entry Lo 0 register
	dmtc0	t5,CP0_TLB_LO_1		# write Entry Lo 1 register
	mtc0	a1,CP0_INDEX		# TLB index = index

	beq	a2,zero,$1		# skip cache flush?
	nop
	jal	cache_flush		# flush the caches
	nop

$1:	tlbwi				# write TLB entry
	nop
	nop

	lw	ra,4(sp)		# restore ra
	lw	t0,0(sp)		# restore sreg
	addiu	sp,sp,8			# remove stack frame

	j	ra			# return
	mtc0	t0,CP0_SREG		# restore status register

	.set 	reorder
ENDFRAME(r4k_settlb)

/*
 * r4k_gettlb:
 *
 * Get index TLB entry. Return 0 for success, 1 for fail
 * On the Orion chip, the entry_hi,lo0,lo1 registers are all 64bit.
 *
 */

 #
 # int r4k_gettlb (struct tlb_t *tlb, int index)
 #
FRAME(r4k_gettlb,sp,0,ra)
	.set	noreorder

	mfc0	t0,CP0_SREG		# save status register
	li	t7,~SREG_IE		# get IE bit mask

	move	t1,t0			# get copy of status register
	dmfc0	t2,CP0_TLB_HI		# get current ASID
	and	t0,t0,t7		# disable interrupts
	mtc0	t0,CP0_SREG

	mtc0	a1,CP0_INDEX		# write TLB index
	nop

	tlbr				# read TLB entry
	nop

	mfc0	t3,CP0_PAGEMASK		# read page mask
	dmfc0	t4,CP0_TLB_HI		# read entry Hi
	sw	t3,0(a0)		# tlb->pmask = CP0_PAGEMASK
	sw	t4,4(a0)		# tlb->hi = CP0_TLB_HI
	dmfc0	t3,CP0_TLB_LO_0		# read entry lo 0
	dmfc0	t4,CP0_TLB_LO_1		# read entry lo 1
	sw	t3,8(a0)		# tlb->lo0 = CP0_TLB_LO_0
	sw	t4,12(a0)		# tlb->lo1 = CP0_TLB_LO_1
	dmtc0	t2,CP0_TLB_HI		# restore current ASID
	mtc0	t1,CP0_SREG		# restore status register

	j	ra			# return
	move	v0,zero			# return success (bd slot)

	.set 	reorder
ENDFRAME(r4k_gettlb)

/*
 * r4k_setasid:
 *
 * Set the ASID (Address Space Identifier)
 *
 */

 #
 # void r4k_setasid (int asid)
 #
FRAME(r4k_setasid,sp,0,ra)
	.set	noreorder
	and	a0,TLB_HI_ASIDMASK
	mtc0	a0,CP0_TLB_HI
	j	ra
	nop
	.set	reorder
ENDFRAME(r4k_setasid)

/*
 * r4k_tlbprobe:
 *
 * Probe the TLB for a match with vaddr.  Return the actual index into
 * the TLB, or -1 if no match.
 *
 */

 #
 # int r4k_tlbprobe (struct entryhi_t *hireg)
 #
FRAME(r4k_tlbprobe,sp,0,ra)
	.set 	noreorder

	mfc0	t0,CP0_SREG		# get status register
	mfc0	t3,CP0_TLB_HI		# save Entry Hi
	li	t7,~SREG_IE		# get IE bit mask

	move	t1,t0			# save copy of CPU status register
	and	t0,t0,t7		# disable interrupts
	lw	t2,0(a0)		# t2 = *hireg
	mtc0	t0,CP0_SREG		# set status register
	mtc0	t2,CP0_TLB_HI		# set Entry Hi register

	nop
	tlbp     			# probe tlb
	nop

	mfc0	v1,CP0_INDEX		# get index register
	li	v0,-1			# set up return value
	bltz	v1,failed		# branch if neg. IE probe failed
	nop

	move	v0,v1			# store index in return register

failed:
	mtc0	t3,CP0_TLB_HI		# restore original entry hi
	mtc0	t1,CP0_SREG		# restore original status register

	j	ra 			# return
	nop

	.set	reorder
ENDFRAME(r4k_tlbprobe)

/*
 * setcp0_config:
 *
 * Program the R4K config register.  Flush the caches before hand
 * to handle the case where KSeg0 cache attribute is changing
 *
 */

 #
 # void setcp0_config (ulong config)
 #
FRAME(setcp0_config,sp,8,ra)
	.set	noreorder

	addiu	sp,sp,-8	# make a frame
	sw	ra,4(sp)	# save return address
	sw	a0,0(sp)	# save a0

	jal	cache_flush	# flush the caches
	nop			# (bd slot)

	lw	a0,0(sp)	# restore a0
	nop			# load delay

	mtc0	a0,CP0_CONFIG	# CP0 Config = config
	nop			# let the KSeg0 cache attribute change settle
	nop

	lw	ra,4(sp)	# restore ra

	j	ra		# return
	addiu	sp,sp,8		# remove stack frame (bd slot)

	.set	reorder
ENDFRAME(setcp0_config)

/*
 * setcp0_compare:
 *
 * Program the R4K compare register
 */

 #
 # void setcp0_compare (ulong compare)
 #
FRAME(setcp0_compare,sp,0,ra)
	.set	noreorder

	mtc0	a0,CP0_COMPARE	# CP0 Compare = compare
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(setcp0_compare)

/*
 * setcp0_count:
 *
 * Program the R4K count register
 */

 #
 # void setcp0_count (ulong count)
 #
FRAME(setcp0_count,sp,0,ra)
	.set	noreorder

	mtc0	a0,CP0_COUNT	# CP0 Count = count
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(setcp0_count)

/*
 * setcp0_sreg:
 *
 * Program the R4K status register
 */

 #
 # void setcp0_sreg (ulong sreg)
 #
FRAME(setcp0_sreg,sp,0,ra)
	.set	noreorder

	mtc0	a0,CP0_SREG	# CP0 Status = sreg
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(setcp0_sreg)

/*
 * getcp0_config:
 *
 * Return the R4K config register
 */

 #
 # ulong getcp0_config (void)
 #
FRAME(getcp0_config,sp,0,ra)
	.set	noreorder

	mfc0	v0,CP0_CONFIG	# v0 = CP0 Config
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(getcp0_config)

/*
 * getcp0_sreg:
 *
 * Return the R4K config register
 */

 #
 # ulong getcp0_sreg (void)
 #
FRAME(getcp0_sreg,sp,0,ra)
	.set	noreorder

	mfc0	v0,CP0_SREG	# v0 = CP0 Status
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(getcp0_sreg)

/*
 * getcp0_cause:
 *
 * Return the R4K cause register
 */

 #
 # ulong getcp0_cause (void)
 #
FRAME(getcp0_cause,sp,0,ra)
	.set	noreorder

	mfc0	v0,CP0_CAUSE	# v0 = CP0 Cause
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(getcp0_cause)

/*
 * getcp0_count:
 * Return the R4K count register
 */

 #
 # ulong getcp0_count (void)
 #
FRAME(getcp0_count,sp,0,ra)
	.set	noreorder

	mfc0	v0,CP0_COUNT	# v0 = CP0 Count
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(getcp0_count)

/*
 * getcp0_compare:
 *
 * Return the R4K compare register
 */

 #
 # ulong getcp0_compare (void)
 #
FRAME(getcp0_compare,sp,0,ra)
	.set	noreorder

	mfc0	v0,CP0_COMPARE	# v0 = CP0 Compare
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(getcp0_compare)

/*
 * getcp0_prid:
 *
 * Return the R4K PRId (Processor Revision Identifier) register
 */

 #
 # long getcp0_prid (void)
 #
FRAME(getcp0_prid,sp,0,ra)
	.set	noreorder

	mfc0	v0,CP0_PRID	# v0 = CP0 PRId
	j	ra		# return
	nop			# (bd slot)

	.set	reorder
ENDFRAME(getcp0_prid)

/*
 * level2_cache_flush:
 * Flush the level2 cache
 */
 #
 # void level2_cache_flush (void)
 #
FRAME(level2_cache_flush,sp,0,ra)
 	.set	noreorder
 
 	/*
 	 * Check if the cache is present
 	 */
 	lw	t0,level2_cache_present		# load level2_cache_present
 	beq	zero,t0,level2_cache_missing	# is the cache present ?
 	nop					# (bd slot)
 	
 	/*
 	 * Disable interrupts
 	 */
 	mfc0	t0,CP0_SREG			# read status register
 	li	t1,~SREG_IE			# get IE mask
 	and	t1,t1,t0			# disable IE
 	mtc0	t1,CP0_SREG			# write to status register
 	nop					# let it settle
 
 	/*
 	 * Put the processor in 64bit mode, with no parity
 	 */
 	li	t2,SREG_DE|SREG_KX		# get DE,KX mask
 	or	t1,t1,t2			# assert DE,KX
 	mtc0	t1,CP0_SREG			# write status register
 	nop					# let it settle
 
 	/*
 	 * Do an uncached read of the level2 cache controller
 	 * with TagOP[1] High, TagOP[0] Low. The address needs
 	 * to be in the following format :
 	 *
 	 * 0x9000 000X XXX0 0000 - Where XX XX is defined by platform
 	 * header file.
	 *
	 * Note: On platforms that do not have the cache installed, this
	 * results in a benign uncached read of address 0x0.
 	 */
 	li	t1,0x9000			# get base address
 	dsll	t1,t1,48			# 0x9000 0000 0000 0000
 	li	t2,LVL2_CACHE_TAGLINES_10	# load XXXX
 	dsll	t2,t2,20			# 0x0000 000X XXX0 0000
 	or	t1,t1,t2			# 0x9000 000X XXX0 0000
 
 	lw	t2,0(t1)			# flush the cache
 	nop					# load delay
 
 	/*
 	 * Restore the status register
 	 */
 	mtc0	t0,CP0_SREG			# write status register
 	nop					# let it settle
 
level2_cache_missing:
 
 	j	ra				# return
 	nop					# (bd slot)
 
 	.set reorder
ENDFRAME(level2_cache_flush)

#define CACHE_PATTERN1	0x12345678
#define CACHE_PATTERN2	0xabcdefab
 
/*
 * level2_cache_detect
 * Detect if a level2 cache is present
 */
 #
 # boolean level2_cache_detect (void)
 #
FRAME(level2_cache_detect,sp,32,ra)
 	.set	noreorder
 
 	/*
 	 * The algorithm for detecting the level2 cache is as
 	 * follows :
 	 * 	- Disable interrupts
 	 *	- Flush the primary caches
 	 * 	- Write pattern 1 uncached
 	 *	- Read cached
 	 * 	- Write pattern 2 uncached
 	 *	- Read cached and ensure data is still pattern 1 (cached kseg0)
 	 * 	- Flush primary caches
 	 *	- Read cached (should be pattern 1 if level2 cache hit)
 	 *	- If not pattern1, no level2 cache
 	 * 	- Flush level2 cache
 	 *	- Flush primary caches
 	 */
 	addiu	sp,sp,-32			# make a stack frame
 	sw	ra,8(sp)			# save ra
 	sw	s0,12(sp)			# save s0
 	sw	s1,16(sp)			# save s1
 	sw	s2,20(sp)			# save s2
 	sw	s3,24(sp)			# save s3
 
 	/* 
 	 * Disable interrupts
 	 */
 	mfc0	s0,CP0_SREG			# read status register
 	li	t0,~SREG_IE			# get IE mask	
 	and	t0,s0,t0			# disable IE
 	mtc0	t0,CP0_SREG			# write back status register
 	nop					# let it settle
 
 	/*
 	 * Flush the primary caches
 	 */
 	jal	cache_flush			# flush primary caches
 	nop
 
 	/*
 	 * Get test location (KSeg0 and KSeg1). We use the
	 * magic crash dump information pointer location (0x250)
 	 */
	la	s3,R4K_K1BASE+0x250		# get KSeg1 address
	la	s2,R4K_K0BASE+0x250		# get KSeg0 address

 	/*
 	 * Get the patterns
 	 */	
 	li	t1,CACHE_PATTERN1		# t1 = pattern 1
 	li	t2,CACHE_PATTERN2		# t2 = pattern 2
 
 	/*
 	 * Write pattern1 uncached, read cached
 	 */
 	sw	t1,0(s3)			# write pattern 1 uncached
 	lw	t3,0(s2)			# read cached (fault into
 						# level2 cache and D cache)
 	/* 
 	 * Write a pattern2 uncached and read it cached to ensure kseg0 cached
 	 */
 	sw	t2,0(s3)			# write uncached (Kseg1)
 	lw	t3,0(s2)			# read back cached
 
 	bne	t3,t1,kseg0_uncached		# is it changed (should be #1)
 	nop					# (bd slot)
 
 	/*
 	 * Flush the primary caches
 	 */
 	jal	cache_flush			# flush primary caches
 	nop					# (bd slot)
 
 	/*
 	 * Now, pattern2 is in memory, pattern1 should be in the level2
 	 * cache and the primary caches should be empty. So if we read
 	 * the variable cached, we should get pattern1
 	 */
 	lw	t3,0(s2)			# read cached
 	li	t1,CACHE_PATTERN1		# t1 = pattern1
 	
 	bne	t3,t1,no_level2_cache		# is it there ?
 	nop					# (bd slot)
 
 	/*
 	 * Flush the primary caches
 	 */
 	jal	cache_flush			# flush primary caches
 	nop					# (bd slot)
  	
 	/*
 	 * Flush the secondary cache
 	 */
 	jal	level2_cache_flush		# flush level2 cache
 	nop					# (bd slot)
 
 	/*
 	 * Level 2 cache is present and works
 	 */
 	li	v0,1				# return code
 	sw	v0,level2_cache_present		# level2 cache present
 	b	detect_return			# jump to return code
 	nop					# (bd slot)
 
no_level2_cache:
 	sw	zero,level2_cache_present	# no level2 cache
 
 	b 	detect_return			# return processing
 	move	v0,zero				# set zero return (bd slot)
 	
kseg0_uncached:
 	/*
 	 * Kseg0 dram is not cached, so the level2 cache is inactive
 	 */
 	move	v0,zero				# return FALSE
 	sw	zero,level2_cache_present	# no level2 cache
 	
detect_return:
 
 	mtc0	s0,CP0_SREG			# restore status register
 	lw	ra,8(sp)			# restore ra
 	lw	s0,12(sp)			# restore s0
 	lw	s1,16(sp)			# restore s1
 	lw	s2,20(sp)			# restore s2
 	lw	s3,24(sp)			# restore s3
 
 	addiu	sp,sp,32			# remove stack frame
 
 	j	ra				# return
 	nop					# (bd slot)
 
 	.set reorder
ENDFRAME(level2_cache_detect)

/*
 * cache_flush:
 *
 * Flush both the Instruction and Data caches. Note, only the primary
 * caches are flushed. The level2 cache is flushed by a seperate routine.
 */
 #
 # void cache_flush (void)
 #
FRAME(cache_flush,sp,0,ra)
	.set	noreorder

	/*
	 * First job is to disable interrupts. The Orion erratta
	 * sheet recommends this to avoid a bug in the two way set
	 * associative cache (which kills gdb debugging sometimes).
	 * This is not be a problem with regard to 4ms timer tick
	 * or watchdog as worst case, D cache write back causes
	 * 512 cache line writes (512 * 340ns) plus some I cache fills
	 * (max 8 fills) == 200us.
	 */
	mfc0	t8,CP0_SREG		# read status register
	li	t7,~SREG_IE		# get IE mask
	and	t7,t8,t7		# disable IE
	mtc0	t7,CP0_SREG		# write back status register
	nop				# let it settle

	/*
	 * Next, write back invalidate the D cache
	 */
	li	t0,R4K_K0BASE		# get K0 base address
	li	t1,R4K_DCACHE_LINES	# get no. of lines in D cache
	li	t2,R4K_DLINE_SHIFT	# get the line shift for D cache

dcache_loop:

	blez	t1,do_icache	# finished ?
	nop			# (bd slot)	
	
	cache	1,0(t0)		# Index Write back invalidate D cache line
	nop			# (paranoid)

	addiu	t1,t1,-1	# t1 -= 1
	addu	t0,t0,t2	# increment the line index

	b	dcache_loop	# do it again
	nop			# (bd slot)

do_icache:

	/*
	 * And the next thing is to do an Index Invalidate on I cache
	 */
	li	t0,R4K_K0BASE		# get K0 base address
	li	t1,R4K_ICACHE_LINES	# get no. of lines in I cache
	li	t2,R4K_ILINE_SHIFT	# get the line shift for I cache

icache_loop:

	blez	t1,cache_done	# finished ?
	nop			# (bd slot)

	cache	0,0(t0)		# Index Invalidate I cache line
	nop			# (paranoid)

	addiu	t1,t1,-1	# t1 -= 1
	addu	t0,t0,t2	# increment the line index

	b	icache_loop	# do it again
	nop			# (bd slot)

cache_done:

	j	ra		# return
	mtc0	t8,CP0_SREG	# restore original status register (bd slot)

	.set	reorder
ENDFRAME(cache_flush)

/* End of file */

| $Id: asm_hes.S,v 3.2.60.1 1996/03/18 18:50:52 gstovall Exp $
| $Source: /release/111/cvs/Xsys/asm-m68k/asm_hes.S,v $
|------------------------------------------------------------------
| asm2.s -- Assembler routines specific to the 68020 (CSC2) processor
| 
| March 1989, Kirk Lougheed
|
| Copyright (c) 1988-1996 by cisco Systems, Inc.
| All rights reserved.
|
| Registers a0, a1, d0, and d1 are always available.
| Any other registers must be preserved for the caller.
|------------------------------------------------------------------
| /*
| $Log: asm_hes.S,v $
| Revision 3.2.60.1  1996/03/18  18:50:52  gstovall
| Branch: California_branch
| Elvis has left the building.  He headed out to California, and took the
| port ready changes with him.
|
| Revision 3.2.26.1  1996/03/09  05:03:45  hampton
| Slim down the kernel of the router.
| Branch: DeadKingOnAThrone_branch
|
| Revision 3.2  1995/11/17  08:40:02  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:47  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:05  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
| */
|

#include "../../boot/cpu.h"

|
 | Assembler support routines for the Signetics S68562 DUSCC UART:

 |
 | Channel A (Console Port) Receive Interrupt:
 |
 | void sig_a_receive (void)
 |
	.globl _sig_a_receive

_sig_a_receive:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_a_receive_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel A (Console Port) Transmit Interrupt:
 |
 | void sig_a_transmit (void)
 |
	.globl _sig_a_transmit

_sig_a_transmit:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_a_transmit_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel A (Console Port) Status Interrupt:
 |
 | void sig_a_status (void)
 |
	.globl _sig_a_status

_sig_a_status:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_a_status_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel A (Console Port) Spurious Interrupt:
 |
 | void sig_a_spurious (void)
 |
	.globl _sig_a_spurious

_sig_a_spurious:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_a_spurious_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel B (Auxillary Port) Receive Interrupt:
 |
 | void sig_b_receive (void)
 |
	.globl _sig_b_receive

_sig_b_receive:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_b_receive_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel B (Auxillary Port) Transmit Interrupt:
 |
 | void sig_b_transmit (void)
 |
	.globl _sig_b_transmit

_sig_b_transmit:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_b_transmit_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel B (Auxillary Port) Status Interrupt:
 |
 | void sig_b_status (void)
 |
	.globl _sig_b_status

_sig_b_status:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_b_status_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte

 |
 | Channel B (Auxillary Port) Spurious Interrupt:
 |
 | void sig_b_spurious (void)
 |
	.globl _sig_b_spurious

_sig_b_spurious:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	jsr	_sig_b_spurious_int	| dispatch to C code
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte
|
 | void mci2buffer(volatile void *mcisrc, void *dst, int bytes)
 |

	.globl _mci2buffer

_mci2buffer:
	jmp	_m2lcopy		| always use fast code


 | void mci2buffer2(volatile void *mcisrc, void *dst, int bytes, ulong seed)

	.globl _mci2buffer2

_mci2buffer2:
	jmp	_m2lcopy2


 | void buffer2mci(void *src, volatile void *mcidst, int bytes)

	.globl _buffer2mci

_buffer2mci:
	jmp	_l2mcopy		| always use fast code


 | void buffer2mcis(void *src, volatile void *mcidst, int bytes)

	.globl _buffer2mcis

_buffer2mcis:
	jmp	_s2mcopy		| always use fast code

|
 |
 | Very fast byte copy.  This routine modifies everything to be short
 | aligned, and performs short moves.  It will therefore copy one byte
 | too many if an odd transfer is requested, so there must be an extra
 | byte available in the buffer.  This routime *must* be used when
 | referencing the SBE 16MB Token Ring board instead of the bcopy routine.
 | 
 | void scopy(void *src, void *dst, int bytes)
 |
	.globl _scopy

_scopy:	movl	sp@(8),a0	| destination
	movl	sp@(4),a1	| source
	movl	sp@(12),d1	| count
	ble	scopyx		| catch bogus count now
	btst	#0,d1		| if odd, pad out to a 16 bit word copy
	beq	scopye
	addl	#1,d1
scopye:	asrl	#1,d1		| convert bytes to shorts
	cmpl	#12,d1		| small move, less than 6 longs?
	bgt	scopy2		| yes, do simple short mov

scopys:	subqw	#1,d1		| decrement for loop, anything to do?
	blt	scopyx		| no, exit
scopy6:	movw	a1@+,a0@+	| copy any residual bytes
	dbra	d1,scopy6	| while byte count
scopyx:	movl	sp@(12),d0	| just return the count
	rts

 | this is the unaligned move routine. This could be made somewhat faster by
 | doing 32k and larger moves in 32k blocks. You lose.

scpyod:	cmpl	#4000,d1	| over 32K?
	blt	scopys		| no, use slightly faster lossage
	subql	#1,d1
scopy9:	movw	a1@+,a0@+	| copy any residual bytes
	subql	#1,d1
	bge	scopy9		| while byte count
	bra	scopyx

 | this is the main longword move loop. Come here when we have more than 8
 | bytes to move and the pointers are at least word aligned.
scopy2:	movl	d1,d0		| copy remaining count
	andl	#~1,d0		| count div 2 is number of long words
	beq	scopys		| are there any?
	subl	d0,d1		| residue can only be 0-1, faster than and
	asrl	#1,d0		| number of long words
	negl	d0		| negate for dispatch
	bra	scopy5		| enter bottom of loop
scopy3:	asll	#1,d0		| compute offset into jump table in bytes
	jmp	pc@(2,d0:w)	| move residue of longs
scopy4:	movl	a1@+,a0@+	| 32 longs
	movl	a1@+,a0@+	| 31 longs
	movl	a1@+,a0@+	| 30 longs
	movl	a1@+,a0@+	| 29 longs
	movl	a1@+,a0@+	| 28 longs
	movl	a1@+,a0@+	| 27 longs
	movl	a1@+,a0@+	| 26 longs
	movl	a1@+,a0@+	| 25 longs
	movl	a1@+,a0@+	| 24 longs
	movl	a1@+,a0@+	| 23 longs
	movl	a1@+,a0@+	| 22 longs
	movl	a1@+,a0@+	| 21 longs
	movl	a1@+,a0@+	| 20 longs
	movl	a1@+,a0@+	| 19 longs
	movl	a1@+,a0@+	| 18 longs
	movl	a1@+,a0@+	| 17 longs
	movl	a1@+,a0@+	| 16 longs
	movl	a1@+,a0@+	| 15 longs
	movl	a1@+,a0@+	| 14 longs
	movl	a1@+,a0@+	| 13 longs
	movl	a1@+,a0@+	| 12 longs
	movl	a1@+,a0@+	| 11 longs
	movl	a1@+,a0@+	| 10 longs
	movl	a1@+,a0@+	|  9 longs
	movl	a1@+,a0@+	|  8 longs
	movl	a1@+,a0@+	|  7 longs
	movl	a1@+,a0@+	|  6 longs
	movl	a1@+,a0@+	|  5 longs
	movl	a1@+,a0@+	|  4 longs
	movl	a1@+,a0@+	|  3 longs
	movl	a1@+,a0@+	|  2 longs
	movl	a1@+,a0@+	|  1 long
scopy5:	addl	#32,d0		| decrement count of longs
	bmi	scopy4		| more than 32, execute whole table
	bcs	scopy3		| less than 32, do dispatch
	bra	scopys		| no, copy up to 3 residual bytes
|
 | m2lcopy
 | Do a long-word copy from an MCI register to an on-board memory buffer.
 | Moves only long word increments of data.  Data count is in bytes. 
 | Processor has been checked for byte alignment requirements.
 | 
 | void m2lcopy(volatile void *mcisrc, void *dst, int bytes)
 |
	.globl _m2lcopy

_m2lcopy:
	movl	sp@(8),a0	| destination
	movl	sp@(4),a1	| mci source
	movl	sp@(12),d0	| count
	ble	m2lcpx		| catch bogus count now
	addql	#3,d0		| round up byte count
	asrl	#2,d0		| convert to long words
	negl	d0		| negate for dispatch
	bra	m2lcp2		| enter bottom of loop
m2lcp0:	asll	#1,d0		| compute offset into jump table in bytes
	jmp	pc@(2,d0:w)	| move residue of longs
m2lcp1:	movl	a1@,a0@+	| 32 longs
	movl	a1@,a0@+	| 31 longs
	movl	a1@,a0@+	| 30 longs
	movl	a1@,a0@+	| 29 longs
	movl	a1@,a0@+	| 28 longs
	movl	a1@,a0@+	| 27 longs
	movl	a1@,a0@+	| 26 longs
	movl	a1@,a0@+	| 25 longs
	movl	a1@,a0@+	| 24 longs
	movl	a1@,a0@+	| 23 longs
	movl	a1@,a0@+	| 22 longs
	movl	a1@,a0@+	| 21 longs
	movl	a1@,a0@+	| 20 longs
	movl	a1@,a0@+	| 19 longs
	movl	a1@,a0@+	| 18 longs
	movl	a1@,a0@+	| 17 longs
	movl	a1@,a0@+	| 16 longs
	movl	a1@,a0@+	| 15 longs
	movl	a1@,a0@+	| 14 longs
	movl	a1@,a0@+	| 13 longs
	movl	a1@,a0@+	| 12 longs
	movl	a1@,a0@+	| 11 longs
	movl	a1@,a0@+	| 10 longs
	movl	a1@,a0@+	|  9 longs
	movl	a1@,a0@+	|  8 longs
	movl	a1@,a0@+	|  7 longs
	movl	a1@,a0@+	|  6 longs
	movl	a1@,a0@+	|  5 longs
	movl	a1@,a0@+	|  4 longs
	movl	a1@,a0@+	|  3 longs
	movl	a1@,a0@+	|  2 longs
	movl	a1@,a0@+	|  1 long
m2lcp2:	addl	#32,d0		| decrement count of longs
	bmi	m2lcp1		| more than 32, execute whole table
	bcs	m2lcp0		| less than 32, do dispatch
m2lcpx:	rts			| all done

|
 |
 | Not so fast byte copy. Used when the destination packet would
 | otherwise be odd-aligned.
 |
 | This routine does long moves ... earlier problems have been fixed.
 |
 | void scopy2(void *src, void *dst, int bytes, ulong seed)
 |
	.globl _scopy2

_scopy2:
	movl	sp@(12),d0	| count
	ble	sc2_ret		| catch bogus count now
	addql	#3,d0		| round up ...
	asrl	#2,d0		| ... to longword boundary

	movl	sp@(4),a1	| source
	movl	sp@(8),a0	| destination
	movl	sp@(16),d1	| seed

	movl	d2,-(sp)	| get working register

	roll	#8,d1		| seed: shift 3 low bytes up

| Convert the count to a quad word count. Enter the quad word loop at the
| right place. Yes, this is a little wierd.

	lsrl	#1,d0		| Divide by 2
	bcc	sc2_bit0_clr	| Bit 0 clr, check bit 1

sc2_bit0_set:
	movl	d1,d2		| move seed to expected register
	lsrl	#1,d0		| Divide by 2
	bcc	sc2_loop1	| bit1:bit0=01
	bcs	sc2_loop3	| bit1:bit0=11

sc2_bit0_clr:
	lsrl	#1,d0		| Divide by 2
	bcs	sc2_loop2	| bit1:bit0=10
	;bcc	sc2_loop4	| bit1:bit0=00

sc2_loop4:
	movl	(a1)+,d2	| read next longword
	roll	#8,d2		| shift 3 low bytes up, high byte down
	movb	d2,d1		| form realigned longword
	movl	d1,(a0)+	| write it out

sc2_loop3:
	movl	(a1)+,d1	| next longword
	roll	#8,d1
	movb	d1,d2
	movl	d2,(a0)+

sc2_loop2:
	movl	(a1)+,d2	| next longword
	roll	#8,d2
	movb	d2,d1
	movl	d1,(a0)+

sc2_loop1:
	movl	(a1)+,d1	| next longword
	roll	#8,d1
	movb	d1,d2
	movl	d2,(a0)+

	subql	#1,d0		| transfer complete ?
	bpl	sc2_loop4	| no, continue processing
sc2_exit:
	movl	(sp)+,d2	| restore working register
sc2_ret:
	rts
|
 | m2lcopy2
 | Do a long-word copy from an MCI register to an on-board memory buffer.
 | Moves only long word increments of data.  Data count is in bytes. 
 | Realigns source stream by one byte per longword.
 |
 | Note: further optimization is possible.
 | 
 | void m2lcopy2(volatile void *mcisrc, void *dst, int bytes, ulong seed)
 |
	.globl _m2lcopy2

_m2lcopy2:
	movl	sp@(12),d0	| count
	ble	m2lc2_ret	| catch bogus count now
	addql	#3,d0		| round up byte count
	asrl	#2,d0		| convert to long words

	movl	sp@(4),a1	| mci source
	movl	sp@(8),a0	| destination
	movl	sp@(16),d1	| seed

	movl	d2,-(sp)	| get working register

	roll	#8,d1		| seed: shift 3 low bytes up

| Convert the count to a quad word count. Enter the quad word loop at the
| right place. Yes, this is a little wierd.

	lsrl	#1,d0		| Divide by 2
	bcc	m2lc2_bit0_clr	| Bit 0 clr, check bit 1

m2lc2_bit0_set:
	movl	d1,d2		| move seed to expected register
	lsrl	#1,d0		| Divide by 2
	bcc	m2lc2_loop1	| bit1:bit0=01
	bcs	m2lc2_loop3	| bit1:bit0=11

m2lc2_bit0_clr:
	lsrl	#1,d0		| Divide by 2
	bcs	m2lc2_loop2	| bit1:bit0=10
	;bcc	m2lc2_loop4	| bit1:bit0=00

m2lc2_loop4:
	movl	(a1),d2		| read next longword
	roll	#8,d2		| shift 3 low bytes up, high byte down
	movb	d2,d1		| form realigned longword
	movl	d1,(a0)+	| write it out

m2lc2_loop3:
	movl	(a1),d1		| next longword
	roll	#8,d1
	movb	d1,d2
	movl	d2,(a0)+

m2lc2_loop2:
	movl	(a1),d2		| next longword
	roll	#8,d2
	movb	d2,d1
	movl	d1,(a0)+

m2lc2_loop1:
	movl	(a1),d1		| next longword
	roll	#8,d1
	movb	d1,d2
	movl	d2,(a0)+

	subql	#1,d0		| transfer complete ?
	bpl	m2lc2_loop4	| no, continue processing
m2lc2_exit:
	movl	(sp)+,d2	| yes, restore working register
m2lc2_ret:
	rts			| all done

|
 | l2mcopy
 | Do a long-word copy to an MCI register from an on-board memory buffer.
 | Moves only long word increments of data.  Data count is in bytes. 
 | Processor has been checked for byte alignment requirements.
 | 
 | void l2mcopy(void *src, volatile void *mcidst, int bytes)

	.globl _l2mcopy

_l2mcopy:
	movl	sp@(8),a0	| mci destination
	movl	sp@(4),a1	| source
	movl	sp@(12),d0	| count
	ble	l2mcpx		| catch bogus count now
	addql	#3,d0		| round up byte count
	asrl	#2,d0		| convert to long words
	negl	d0		| negate for dispatch
	bra	l2mcp2		| enter bottom of loop
l2mcp0:	asll	#1,d0		| compute offset into jump table in bytes
	jmp	pc@(2,d0:w)	| move residue of longs
l2mcp1:	movl	a1@+,a0@	| 32 longs
	movl	a1@+,a0@	| 31 longs
	movl	a1@+,a0@	| 30 longs
	movl	a1@+,a0@	| 29 longs
	movl	a1@+,a0@	| 28 longs
	movl	a1@+,a0@	| 27 longs
	movl	a1@+,a0@	| 26 longs
	movl	a1@+,a0@	| 25 longs
	movl	a1@+,a0@	| 24 longs
	movl	a1@+,a0@	| 23 longs
	movl	a1@+,a0@	| 22 longs
	movl	a1@+,a0@	| 21 longs
	movl	a1@+,a0@	| 20 longs
	movl	a1@+,a0@	| 19 longs
	movl	a1@+,a0@	| 18 longs
	movl	a1@+,a0@	| 17 longs
	movl	a1@+,a0@	| 16 longs
	movl	a1@+,a0@	| 15 longs
	movl	a1@+,a0@	| 14 longs
	movl	a1@+,a0@	| 13 longs
	movl	a1@+,a0@	| 12 longs
	movl	a1@+,a0@	| 11 longs
	movl	a1@+,a0@	| 10 longs
	movl	a1@+,a0@	|  9 longs
	movl	a1@+,a0@	|  8 longs
	movl	a1@+,a0@	|  7 longs
	movl	a1@+,a0@	|  6 longs
	movl	a1@+,a0@	|  5 longs
	movl	a1@+,a0@	|  4 longs
	movl	a1@+,a0@	|  3 longs
	movl	a1@+,a0@	|  2 longs
	movl	a1@+,a0@	|  1 long
l2mcp2:	addl	#32,d0		| decrement count of longs
	bmi	l2mcp1		| more than 32, execute whole table
	bcs	l2mcp0		| less than 32, do dispatch
l2mcpx:	rts			| all done
|
 | s2mcopy
 | Do a short-word copy to an MCI register from an on-board memory buffer.
 | Moves only short word increments of data.  Data count is in bytes. 
 | Processor has been checked for byte alignment requirements.
 | 
 | void s2mcopy (void *src, volatile void *mcidst, int bytes)
 |
	.globl _s2mcopy

_s2mcopy:
	movl	sp@(8),a0	| mci destination
	movl	sp@(4),a1	| source
	movl	sp@(12),d0	| count
	ble	s2mcpx		| catch bogus count now
	asrl	#2,d0		| convert to long words
	negl	d0		| negate for dispatch
	bra	s2mcp2		| enter bottom of loop
s2mcp0:	asll	#1,d0		| compute offset into jump table in bytes
	jmp	pc@(2,d0:w)	| move residue of longs
s2mcp1:	movl	a1@+,a0@	| 32 longs
	movl	a1@+,a0@	| 31 longs
	movl	a1@+,a0@	| 30 longs
	movl	a1@+,a0@	| 29 longs
	movl	a1@+,a0@	| 28 longs
	movl	a1@+,a0@	| 27 longs
	movl	a1@+,a0@	| 26 longs
	movl	a1@+,a0@	| 25 longs
	movl	a1@+,a0@	| 24 longs
	movl	a1@+,a0@	| 23 longs
	movl	a1@+,a0@	| 22 longs
	movl	a1@+,a0@	| 21 longs
	movl	a1@+,a0@	| 20 longs
	movl	a1@+,a0@	| 19 longs
	movl	a1@+,a0@	| 18 longs
	movl	a1@+,a0@	| 17 longs
	movl	a1@+,a0@	| 16 longs
	movl	a1@+,a0@	| 15 longs
	movl	a1@+,a0@	| 14 longs
	movl	a1@+,a0@	| 13 longs
	movl	a1@+,a0@	| 12 longs
	movl	a1@+,a0@	| 11 longs
	movl	a1@+,a0@	| 10 longs
	movl	a1@+,a0@	|  9 longs
	movl	a1@+,a0@	|  8 longs
	movl	a1@+,a0@	|  7 longs
	movl	a1@+,a0@	|  6 longs
	movl	a1@+,a0@	|  5 longs
	movl	a1@+,a0@	|  4 longs
	movl	a1@+,a0@	|  3 longs
	movl	a1@+,a0@	|  2 longs
	movl	a1@+,a0@	|  1 long
s2mcp2:	addl	#32,d0		| decrement count of longs
	bmi	s2mcp1		| more than 32, execute whole table
	bcs	s2mcp0		| less than 32, do dispatch
	andl	#3,sp@(12)	| leftover short word?
	beq	s2mcpx		| nope
	movw	a1@+,a0@	| do it
s2mcpx:	rts			| all done


 | mci2mci
 | Do a long-word copy from one MCI to another MCI.
 | Moves only long word increments of data.  Data count is in bytes. 
 | 
 | void mci2mci (volatile void *src, volatile void *dst, int bytes)
 |
	.globl _mci2mci

_mci2mci:
	movl	sp@(8),a0	| destination MCI
	movl	sp@(4),a1	| source MCI
	movl	sp@(12),d0	| byte count
	ble	m2mcpx		| catch bogus count now
	addql	#3,d0		| round up byte count
	asrl	#2,d0		| convert to long words
	negl	d0		| negate for dispatch
	bra	m2mcp2		| enter bottom of loop
m2mcp0:	asll	#1,d0		| compute offset into jump table in words
	jmp	pc@(2,d0:w)	| move residue of longs
m2mcp1:	movl	a1@,a0@		| 32 longs
	movl	a1@,a0@		| 31 longs
	movl	a1@,a0@		| 30 longs
	movl	a1@,a0@		| 29 longs
	movl	a1@,a0@		| 28 longs
	movl	a1@,a0@		| 27 longs
	movl	a1@,a0@		| 26 longs
	movl	a1@,a0@		| 25 longs
	movl	a1@,a0@		| 24 longs
	movl	a1@,a0@		| 23 longs
	movl	a1@,a0@		| 22 longs
	movl	a1@,a0@		| 21 longs
	movl	a1@,a0@		| 20 longs
	movl	a1@,a0@		| 19 longs
	movl	a1@,a0@		| 18 longs
	movl	a1@,a0@		| 17 longs
	movl	a1@,a0@		| 16 longs
	movl	a1@,a0@		| 15 longs
	movl	a1@,a0@		| 14 longs
	movl	a1@,a0@		| 13 longs
	movl	a1@,a0@		| 12 longs
	movl	a1@,a0@		| 11 longs
	movl	a1@,a0@		| 10 longs
	movl	a1@,a0@		|  9 longs
	movl	a1@,a0@		|  8 longs
	movl	a1@,a0@		|  7 longs
	movl	a1@,a0@		|  6 longs
	movl	a1@,a0@		|  5 longs
	movl	a1@,a0@		|  4 longs
	movl	a1@,a0@		|  3 longs
	movl	a1@,a0@		|  2 longs
	movl	a1@,a0@		|  1 long
m2mcp2:	addl	#32,d0		| decrement count of longs
	bmi	m2mcp1		| more than 32, execute whole table
	bcs	m2mcp0		| less than 32, do dispatch
m2mcpx:	rts			| all done
|
 | bwcopy
 | Byte to word copy.
 | Same arguments as bcopy, but each byte is stored twice.
 | 
 | void bwcopy (ushort *src, ushort *dst, int bytes)
 |
 | Is this used by anyone?
 |
	.globl _bwcopy

_bwcopy:
	movl	sp@(4),a0		| source pointer
	movl	sp@(8),a1		| destination pointer
	movl	sp@(12),d0		| byte count
bwcpy1: movb	a0@+,d1			| get a byte
	movb	d1,a1@+			| Store it once...
	addql	#1,a1			|  skip a byte
	dbra	d0,bwcpy1		| loop util done
	rts

 | word to byte copy.
 | Same arguments as bcopy, but only one byte of each word is stored.
 | 
 | void wbcopy (ushort *src, ushort *dst, int bytes)
 |
 | Is this used by anyone?
 |
	.globl _wbcopy

_wbcopy:
	movl	sp@(4),a0		| source pointer
	movl	sp@(8),a1		| destination pointer
	movl	sp@(12),d0		| byte count
wbcpy1:
| the order of the next two line may need to be reversed.
	movb	a0@+,d1			| get a byte	
	addql	#1,a0			| skip a byte
	movb	d1,a1@+			| Store it
	dbra	d0,wbcpy1		| loop util done
	rts

|
| memhammer - hammer a memory area
|
| boolean memhammer(address, size, iterations, read_on_fail)
|
| return true if no failures detected
|
| DEBUGGING CODE
|
	.globl	_memhammer

_memhammer:
	link	a6,#0
	movml	d2-d5,sp@-
	movl	a6@(16),d1		| number of iterations
	subql	#1,d1
	movl	#0x1577,d2		| datum to write
	movl	#1,d4			| success or failure
	movl	a6@(20),a1		| address to yank on failure

memhammer_cycle:
	movl	a6@(8),a0		| address to start with
	movl	a0,d2
	addql	#1,d2
	movl	a6@(12),d0		| size of block
	asrl	#2,d0			| number of words
	subql	#1,d0

memhammer_lp:
	movl	d2,a0@
	movl	a0@+,d3
	cmpl	d2,d3
	beq	mem_ok

	movl	#0,d4
	movw	a1@,d5			| read the trigger word
mem_ok:	addql	#1,d2
	dbra	d0,memhammer_lp

	dbra	d1,memhammer_cycle

	movl	d4,d0
	movml	sp@+,d2-d5
	unlk	a6
	rts

|
|
| boolean memhammer_word(address, size, iterations, read_on_fail)
|
| return true if no failures detected
|
| DEBUGGING CODE
|
	.globl	_memhammer_word

_memhammer_word:
	link	a6,#0
	movml	d2-d5,sp@-
	movl	a6@(16),d1		| number of iterations
	subql	#1,d1
	movl	#0x1577,d2		| datum to write
	movl	#1,d4			| success or failure
	movl	a6@(20),a1		| address to yank on failure

mhw_cycle:
	movl	a6@(8),a0		| address to start with
	movl	a0,d2
	addql	#1,d2
	movl	a6@(12),d0		| size of block
	asrl	#1,d0			| number of words
	subql	#1,d0

mhw_lp:
	movw	d2,a0@
	movw	a0@+,d3
	cmpw	d2,d3
	beq	mhw_ok

	movl	#0,d4
	movw	a1@,d5			| read the trigger word
mhw_ok:	addql	#1,d2
	dbra	d0,mhw_lp

	dbra	d1,mhw_cycle

	movl	d4,d0
	movml	sp@+,d2-d5
	unlk	a6
	rts

|
| memhammer_byte - hammer a memory area byte by byte
|
| boolean memhammer_byte(address, size, iterations, read_on_fail)
|
| return true if no failures detected
|
| DEBUGGING CODE
|
	.globl	_memhammer_byte

_memhammer_byte:
	link	a6,#0
	movml	d2-d5,sp@-
	movl	a6@(16),d1		| number of iterations
	subql	#1,d1
	movl	#0x1577,d2		| datum to write
	movl	#1,d4			| success or failure
	movl	a6@(20),a1		| address to yank on failure

mhb_cycle:
	movl	a6@(8),a0		| address to start with
	movl	a0,d2
	addql	#1,d2
	movl	a6@(12),d0		| size of block
	subql	#1,d0

mhb_lp:
	movb	d2,a0@
	movb	a0@+,d3
	cmpb	d2,d3
	beq	mhb_ok

	movl	#0,d4
	movw	a1@,d5			| read the trigger word
mhb_ok:	addql	#1,d2
	dbra	d0,mhb_lp

	dbra	d1,mhb_cycle

	movl	d4,d0
	movml	sp@+,d2-d5
	unlk	a6
	rts

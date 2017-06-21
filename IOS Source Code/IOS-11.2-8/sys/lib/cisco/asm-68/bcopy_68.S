/* $Id: bcopy_68.S,v 3.1.10.1 1996/03/18 20:50:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-68/bcopy_68.S,v $
 *------------------------------------------------------------------
 * bcopy_68.S -- very fase byte copy function
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcopy_68.S,v $
 * Revision 3.1.10.1  1996/03/18  20:50:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:34:15  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef mc68000
 | bcopy
 | Very fast byte copy.  By making the byte count a multiple of 2 or 4,
 | and having the pointers word or long aligned, it is possible to speed 
 | up the copy significantly, so fudge byte counts whenever possible.
 | If you are copying source and destination byte-misaligned, there is little
 | hope for speed.
 | 
 | void bcopy (uchor *src, uchar *dst, int bytes)
 |

	.globl _bcopy, bcopy

bcopy:
_bcopy:
	movl	%sp@(8),%a0	| destination
	movl	%sp@(4),%a1	| source
	movl	%sp@(12),%d1	| count
	ble	.bcopyx		| catch bogus count now
	cmpl	&24,%d1		| small move, less than 6 longs?
	ble	.bcopyb		| yes, do simple byte mov
	movl	%a0,%d0		| destination
	andl	&1,%d0		| word aligned?
	beq	.bcopy1		| yes, check other pointer
	movl	%a1,%d0		| no, check and maybe align
	andl	&1,%d0		| this one odd as well?
	beq	.bcpyod		| bad case, byte unaligned move
	movb	%a1@+,%a0@+	| move byte to get to word boundry
	subql	&1,%d1		| decrement remaining byte count
	bne	.bcopy2		| now word aligned, bytes to go
	bra	.bcopyx		| this was an expensive single byte move

.bcopyb:
	subqw	&1,%d1		| decrement for loop, anything to do?
	blt	.bcopyx		| no, exit
.bcopy6:
	movb	%a1@+,%a0@+	| copy any residual bytes
	dbra	%d1,.bcopy6	| while byte count
.bcopyx:
	movl	%sp@(12),%d0	| just return the count
	rts

 | this is the unaligned move routine. This could be made somewhat faster by
 | doing 32k and larger moves in 32k blocks. You lose.

.bcpyod:
	cmpl	&8000,%d1	| over 32K?
	blt	.bcopyb		| no, use slightly faster lossage
	subql	&1,%d1
.bcopy9:
	movb	%a1@+,%a0@+	| copy any residual bytes
	subql	&1,%d1
	bge	.bcopy9		| while byte count
	bra	.bcopyx

 | alignment checks continue here
.bcopy1:
	movl	%a1,%d0		| get source
	andl	&1,%d0		| word aligned?
	bne	.bcpyod		| bad case, byte unaligned move
 | this is the main longword move loop. Come here when we have more than 8
 | bytes to move and the pointers are at least word aligned.
.bcopy2:
	movl	%d1,%d0		| copy remaining count
	andl	&~3,%d0		| count div 4 is number of long words
	beq	.bcopyb		| are there any?
	subl	%d0,%d1		| residue can only be 0-3, faster than and
	asrl	&2,%d0		| number of long words
	negl	%d0		| negate for dispatch
	bra	.bcopy5		| enter bottom of loop
.bcopy3:
	asll	&1,%d0		| compute offset into jump table in bytes
	jmp	%pc@(2,%d0:w)	| move residue of longs
.bcopy4:
	movl	%a1@+,%a0@+	| 32 longs
	movl	%a1@+,%a0@+	| 31 longs
	movl	%a1@+,%a0@+	| 30 longs
	movl	%a1@+,%a0@+	| 29 longs
	movl	%a1@+,%a0@+	| 28 longs
	movl	%a1@+,%a0@+	| 27 longs
	movl	%a1@+,%a0@+	| 26 longs
	movl	%a1@+,%a0@+	| 25 longs
	movl	%a1@+,%a0@+	| 24 longs
	movl	%a1@+,%a0@+	| 23 longs
	movl	%a1@+,%a0@+	| 22 longs
	movl	%a1@+,%a0@+	| 21 longs
	movl	%a1@+,%a0@+	| 20 longs
	movl	%a1@+,%a0@+	| 19 longs
	movl	%a1@+,%a0@+	| 18 longs
	movl	%a1@+,%a0@+	| 17 longs
	movl	%a1@+,%a0@+	| 16 longs
	movl	%a1@+,%a0@+	| 15 longs
	movl	%a1@+,%a0@+	| 14 longs
	movl	%a1@+,%a0@+	| 13 longs
	movl	%a1@+,%a0@+	| 12 longs
	movl	%a1@+,%a0@+	| 11 longs
	movl	%a1@+,%a0@+	| 10 longs
	movl	%a1@+,%a0@+	|  9 longs
	movl	%a1@+,%a0@+	|  8 longs
	movl	%a1@+,%a0@+	|  7 longs
	movl	%a1@+,%a0@+	|  6 longs
	movl	%a1@+,%a0@+	|  5 longs
	movl	%a1@+,%a0@+	|  4 longs
	movl	%a1@+,%a0@+	|  3 longs
	movl	%a1@+,%a0@+	|  2 longs
	movl	%a1@+,%a0@+	|  1 long
.bcopy5:
	addl	&32,%d0		| decrement count of longs
	bmi	.bcopy4		| more than 32, execute whole table
	bcs	.bcopy3		| less than 32, do dispatch
	bra	.bcopyb		| no, copy up to 3 residual bytes

|
|Local Variables:
|comment-column: 40
|comment-start: "|"
|comment-end: ""
|comment-multi-line: nil
|comment-start-skip: "|"
|End:
|
#endif /* mc68000 */

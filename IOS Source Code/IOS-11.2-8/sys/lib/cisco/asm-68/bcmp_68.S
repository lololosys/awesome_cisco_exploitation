/* $Id: bcmp_68.S,v 3.1.10.1 1996/03/18 20:50:22 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-68/bcmp_68.S,v $
 *------------------------------------------------------------------
 * bcmp_68.S -- byte comparison routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcmp_68.S,v $
 * Revision 3.1.10.1  1996/03/18  20:50:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:34:14  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef mc68000
/*
 * bcmp returns 0 if comparison true, non-zero difference otherwise
 */
	.globl _bcmp, bcmp
bcmp:
_bcmp:
	movel	%sp@(4),%a0		| first pointer
	movel	%sp@(8),%a1		| second pointer
	movel	%sp@(12),%d0		| byte count
	cmpl	%a1,%a0			| same pointer?
	beq	.bcmp2			| then we are done
.bcmp0:
	subql	&1,%d0			| decrement counter
	blt	.bcmp2			| are we done?
	cmpmb	%a0@+,%a1@+		| compare a byte
	beq	.bcmp0			| loop if equal
	moveq	&0,%d0
	movb	%a0@-,%d0		| backup and get a character
	moveq	&0,%d1
	movb	%a1@-,%d1		| same for other ptr
	subl	%d1,%d0			| obtain difference
	bra	.bcmp3
.bcmp2:
	movel	&0,%d0			| match
.bcmp3:
	rts

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

/* $Id: bscopy_68.S,v 3.1.10.1 1996/03/18 20:50:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-68/bscopy_68.S,v $
 *------------------------------------------------------------------
 * bscopy_68.S -- byte swapped word copy
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bscopy_68.S,v $
 * Revision 3.1.10.1  1996/03/18  20:50:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:34:17  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef mc68000
 | bscopy
 | Byte swapped word copy.
 | Same arguments as bcopy, but assumes word transfers
 |
 | void bscopy (unsigned short *src, unsigned short *dst, int bytes)
 |

	.globl _bscopy, bscopy

bscopy:
_bscopy:
	movl	%sp@(4),%a0		| source pointer
	movl	%sp@(8),%a1		| destination pointer
	movl	%sp@(12),%d0		| byte count
	addql	&1,%d0			| round up byte count
	asrl	&1,%d0			| convert bytes to words
	subql	&1,%d0			| pre-decrement
	movl	%a0,%d1			| is source
	andl	&1,%d1			| address odd? (andi faster than btst)
	bne	.bscpy2			| yes, be more careful
	movl	%a1,%d1			| is destination
	andl	&1,%d1			| address odd?
	bne	.bscpy3			| yes, be more careful
.bscpy1:
	movw	%a0@+,%d1		| get word into d1, incr src ptr
	rolw	&8,%d1			| an 8-bit rotate is a byte swap
	movw	%d1,%a1@+		| put word into dst, incr ptr
	dbra	%d0,.bscpy1		| loop util done
	rts				| all done

.bscpy2:
	movb	%a0@+,%d1		| get byte into d1, incr src ptr
	lslw	&8,%d1			| move byte over
	movb	%a0@+,%d1		| get next byte
	rolw	&8,%d1			| now do the swap in register
	movw	%d1,%a1@+		| then stuff it
	dbra	%d0,.bscpy2		| loop util done
	rts

.bscpy3:
	movw	%a0@+,%d1		| get word into d1, incr src ptr
	movb	%d1,%a1@+		| get lower byte first
	lsrw	&8,%d1			| move second byte over
	movb	%d1,%a1@+		| then stuff it
	dbra	%d0,.bscpy3		| loop util done
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

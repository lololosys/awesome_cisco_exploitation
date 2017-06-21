| $Id: vinesasm.S,v 3.2 1995/11/17 08:40:13 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/asm-m68k/vinesasm.S,v $
|------------------------------------------------------------------
| vinesasm.s -- vines specific assembler routines
|
| June 1990, David R. Hampton
|
| Copyright (c) 1990-1994 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: vinesasm.S,v $
| Revision 3.2  1995/11/17  08:40:13  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:57  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:16  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

	.text

	.globl _vines_crc1

 | ushort vines_crc1 (vinesiptype *data)
 |
 | data is a pointer to the beginning of the VINES IP header
 |

_vines_crc1:
	linkw	a6,#0			|save stack frame
	moveml	d6-d7/a4-a5,sp@-	|and some registers
	movl	a6@(8),a5		|get address of data
	clrl	d7			|zero work register
	movw	a5@(2),d7		|get length word
	btst	#0,d7			|odd ?
	beq	vcrca

	movl	d7,d6			|yes - its odd.
	addl	a5,d6			|compute address of last byte so that
	movl	d6,a4			|  we can zero it. there is no overrun
	clrb	a4@			|  problem because of the buffer slop

vcrca:	subqw	#6,d7			|remove checksum, len,
					|transport, and control
	addqw	#1,d7			|add one
	asrw	#1,d7			|get length in shorts
	addql	#6,a5			|skip checksum, len,
					|transport, and control
	movw	a5@(-2), d0		|initialize to transport and type
	andw	#0xf0ff, d0		|mask out hop count
	addw	a5@(-4),d0		|add the length
	bcc	vcrcb			|did we carry
	addqw	#1,d0			|yes, add it back in
vcrcb:	addw	a5@+,d0			|start summing data
	bcc	vcrcc			|did we carry
	addqw	#1,d0			|yes, add it back in
vcrcc:	subqw	#1,d7			|decrement short length counter
	bgt	vcrcb			|do some more if necessary
	cmpw	#0xffff,d0		|is result all ones
	bne	vcrcd			|maybe, maybe not
	clrl	d0			|if so then make it zero
vcrcd:	moveml	sp@+,d6-d7/a4-a5
	unlk	a6
	rts
|
 | void vines_qcsum (uchar *pak, int oldword, int num)

	.globl  _vines_qcsum

_vines_qcsum:
	linkw	a6,#-32
	moveml	#14584,a6@(-32)
	movl	a6@(8),a5		|address of packet
	movl	a6@(12),d7		|old control word
	movl	a6@(16),d6		|which word changed
	movw	a5@(2),d0		|length of VINES packet
	extl	d0			|extend to long word
	addql	#1,d0			|round up length to shorts
	asrl	#1,d0			|rotate right by 1 (divide by 2)
	movl	d0,d4			|save short length
	movl	a5,a3			|address of packet. save it here
	cmpw	#0xffff,a3@		|are we doing checksums
	beq	quickc			|if it -1 then no way
	movl	a5,a4			|base address. save it for later
	movl	d6,d0			|get word index that changed
	asll	#1,d0			|convert to byte index
	addl	d0,a4			|get address of word that changed
	movw	a4@,d3			|fetch the word that changed
	subw	d7,d3			|get difference between new and old
	bcc	quicka			|any overflow
	addqw	#1,d3			|if so then add the carry back in
quicka:	movl	d4,d0			|get length of packet
	subl	d6,d0			|subtract index of changed word
	andl	#15,d0			|modulo 16
	movl	d0,d5			|save result
	rolw	d5,d3			|rotate difference
	addw	a3@,d3			|sum with checksum field
	bcc	quickb			|did we set carry
	addqw	#1,d3			|add it back in if so
quickb:	movw	d3,a3@			|move new sum to checksum field
quickc:	moveml	a6@(-32),#14584		|restore registers
	unlk	a6
	rts

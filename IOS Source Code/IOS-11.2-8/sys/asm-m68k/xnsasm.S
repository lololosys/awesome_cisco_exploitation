| $Id: xnsasm.S,v 3.2 1995/11/17 08:40:15 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/asm-m68k/xnsasm.S,v $
|------------------------------------------------------------------
| xnsasm.S -  XNS specific assembler routines
|
| March 1989, Kirk Lougheed
|
| Copyright (c) 1989-1995 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: xnsasm.S,v $
| Revision 3.2  1995/11/17  08:40:15  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:58  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:17  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

	.text

	.globl _xns_crc1

 | ushort xns_crc1 (ushort *data)
 |
 | data is a pointer to the beginning of the data portion
 | of the packet
 |

_xns_crc1:
	linkw	a6,#-8			|save stack frame
	moveml	#8320,a6@(-8)		|and some registers
	movl	a6@(8),a5		|get address of data
	clrl	d7			|zero work register
	movw	a5@(2),d7		|get length word
	subqw	#2,d7			|remove checksum length
	addqw	#1,d7			|add one
	asrw	#1,d7			|get length in shorts
	clrl	d0			|where to accumulate sum
	addql	#2,a5			|skip checksum
cspaka:	addw	a5@+,d0			|start summing data
	bcc	cspakb			|did we carry
	addqw	#1,d0			|yes, add it back in
cspakb:	rolw	#1,d0			|rotate result
	subqw	#1,d7			|decrement short length counter
	bgt	cspaka			|do some more if necessary
	cmpw	#0xffff,d0		|is result all ones
	bne	cspakc			|maybe, maybe not
	clrl	d0			|if so then make it zero
cspakc:	moveml	a6@(-8),#8320
	unlk	a6
	rts
 |
 | void xns_qcsum (uchar *pak, int oldword, int num)
 |
	.globl  _xns_qcsum

_xns_qcsum:
	linkw	a6,#-32
	moveml	#14584,a6@(-32)
	movl	a6@(8),a5		|address of packet
	movl	a6@(12),d7		|old control word
	movl	a6@(16),d6		|which word changed
	movw	a5@(2),d0		|length of XNS packet
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

	.globl  _xns_fqc_header
 | void xns_fqc_header (uchar *ptr)

_xns_fqc_header:
	clrl	d0
        movl	sp@(4),a0		| get pointer to checksum
	movw	a0@(2),d0		| get length
	movw	#256,d1			| load the difference
	addql	#1,d0			|round up length to shorts
	asrl	#1,d0			|rotate right by 1 (divide by 2)
	subl	#2,d0			|subtract index of changed word
	andl	#15,d0			|modulo 16
	rolw	d0,d1			|rotate difference
        movw	a0@,d0			| get checksum
	addw	d0,d1			|sum with checksum field
	bcc	quckbf_header		|did we set carry
	addqw	#1,d1			|add it back in if so
quckbf_header:
        movw	d1,a0@			| load checksum field
	rts


	.globl  _xns_fqc
 | ushort xns_fqc (ushort length, ushort checksum)

_xns_fqc:
	movl	sp@(4),d0		|get length
	movw	#256,d1			| load the difference
	lsrl	#8,d0			| flush out the low word
	lsrl	#8,d0			| flush the low word
	addql	#1,d0			|round up length to shorts
	asrl	#1,d0			|rotate right by 1 (divide by 2)
	subl	#2,d0			|subtract index of changed word
	andl	#15,d0			|modulo 16
	rolw	d0,d1			|rotate difference
	movw	sp@(10),d0		|checksum
	addw	d0,d1			|sum with checksum field
	bcc	quckbf			|did we set carry
	addqw	#1,d1			|add it back in if so
quckbf:
	movw	d1,d0			|move new sum to checksum field
	rts




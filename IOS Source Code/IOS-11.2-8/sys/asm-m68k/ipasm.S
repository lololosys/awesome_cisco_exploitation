| $Id: ipasm.S,v 3.2.60.1 1996/07/22 21:19:00 skufer Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/ipasm.S,v $
|------------------------------------------------------------------
| ipasm.S - IP support routines
|  
| November 27, 1991, Kevin Paul Herbert
|
| Copyright (c) 1991-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: ipasm.S,v $
| Revision 3.2.60.1  1996/07/22  21:19:00  skufer
| CSCdi63695:  68k platforms cannot fast switch datagrams greater than
| 8192 bytes
| Branch: California_branch
|
| Revision 3.2  1995/11/17  08:40:06  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:51  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:10  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

|
 | iphcrc
 | Return the 1s complement checksum of the word aligned IP packet header
 | at s. Length implied by the header.
 | 
 | ushort iphcrc (ushort *s)
 |	
	.globl	_iphcrc

_iphcrc:
	movl	sp@(4),a0		| get pointer to start of header
	clrl	d1			| zero the checksum
	clrl	d0			| zero the length
	movb	a0@,d0			| fetch vers.ihl
	andb	#0xF,d0			| leaving length in longs
	subql	#1,d0			| decrement loop count before entering
iphcrc1:
	addl	a0@+,d1			| sum the data
	dbcs	d0,iphcrc1		| tight loop if no carry and more data
	bcc	iphcrc2			| if no carry, done
	addql	#1,d1			| end around carry
	dbra	d0,iphcrc1		| loop, leaves d0 as 0.-1
iphcrc2:
	movw	d1,d0			| copy low checksum
	swap	d1			| get rest in low 16 bits
	addw	d1,d0			| fold them in
	bcc	iphcrc3			| another carry?
	addqw	#1,d0			| yes, provide it
iphcrc3:
	notw	d0
	rts
|
 | iphtcrc
 | Check an Internet header for the MCI fast switching support:
 |  - version 4 and header length of 5
 |  - range check length (.ge. 20 )
 |  - validate checksum
 |  - decrement ttl and update checksum
 |
 | ushort iphtcrc (ushort *s)
 |	
	.globl	_iphtcrc

_iphtcrc:
	movl	sp@(4),a0		| get pointer to start of header
	cmpb	#0x45,a0@		| is first byte 0x45?
	bne	iphtcrcfail		| no, fail now. 
	cmpw	#20,a0@(2)		| is total length .ge. 20 bytes
	blt	iphtcrcfail		| no, too short, fail
	movl	a0,a1			| extra copy of header pointer
	subqb	#1,a0@(8)		| decrement TTL field
	beq	iphtcrcfail		| jump if result .eq. zero
	movl	a0@+,d1			| initial checksum is first word
	moveq	#3,d0			| long words left to do
iphtcrc1:
	addl	a0@+,d1			| sum the data
	dbcs	d0,iphtcrc1		| tight loop if no carry and more data
	bcc	iphtcrc2		| if no carry, done
	addql	#1,d1			| end around carry
	dbra	d0,iphtcrc1		| loop
|   ..leaves d0 as 0.-1
iphtcrc2:
	movw	d1,d0			| copy low checksum
	swap	d1			| get rest in low 16 bits
	addw	d1,d0			| fold them in
	bcc	iphtcrc3		| another carry?
	addqw	#1,d0			| yes, provide it
iphtcrc3:
	notw	d0
	cmpw	#0x100,d0		| Checksum should be this
	bne	iphtcrcfail		| did not work...
	addw	a1@(10),d0		| subtract (!) out the old checksum
	bcc	iphtcrc4		| another carry?
	addqw	#1,d0			| yes, provide it
iphtcrc4:
	movw	d0,a1@(10)		| put the new checksum in
	clrl	d0			| return success
	rts
iphdrchk_ttl_fail:
 	addqb   #1,a0@(8)               | restore TTL field before returning.
iphtcrcfail:
	moveq	#1,d0			| return failure.
	rts	
|
 | iphdrchk
 | Check an Internet header for the MCI fast switching support:
 |  - version 4 and header length of 5
 |  - range check length (.ge. 20 and le. 8192))
 |  - validate checksum
 |  - decrement ttl and update checksum
 |
 | ushort iphdrchk (ushort *s)
 |	
	.globl	_iphdrchk

_iphdrchk:
	movl	sp@(4),a0		| get pointer to start of header
	cmpb	#0x45,a0@		| is first byte 0x45?
	bne	iphtcrcfail		| no, fail now. 
	cmpw	#20,a0@(2)		| is total length .ge. 20 bytes
	blt	iphtcrcfail		| no, too short, fail
	cmpw	#8192,a0@(2)		| is total length le. 8192 bytes
	bgt	iphtcrcfail		| no, too long, fail
	subqb	#1,a0@(8)		| decrement TTL field
	beq	iphdrchk_ttl_fail	| jump if result .eq. zero
	movl	a0@+,d1			| initial checksum is first word
	clrl	d0			| used for adding in carrys

	addl	a0@+,d1			| sum the data.  longword 2
	addxl	d0,d1			|   Carry bit
	addl	a0@+,d1			|  sum the data.  longword 3
	addxl	d0,d1			|    Carry bit
	addl	a0@+,d1			|   sum the data.  longword 4
	addxl	d0,d1			|     Carry bit
	addl	a0@+,d1			|    sum the data.  longword 5
	addxl	d0,d1			|      Carry bit

	movw	d1,d0			| copy low checksum
	swap	d1			| get rest in low 16 bits
	addw	d1,d0			| fold them in
	bcc	iphdr1			| another carry?
	addqw	#1,d0			| yes, provide it
iphdr1:
	notw	d0
	cmpw	#0x100,d0		| Checksum should be this
	bne	iphtcrcfail		| did not work...
	subql	#8,a0			| point back at checksum
	addw	a0@-,d0			| subtract (!) out the old checksum
	bcc	iphdr2			| another carry?
	addqw	#1,d0			| yes, provide it
iphdr2:
	cmpw    #0xFFFF, d0             | Check for illegal negative zero
	beq     iphdr_zero              | Optimise for common case (no jump)
	movw	d0,a0@			| put the new checksum in
	clrl	d0			| return success
	rts

iphdr_zero:
	clrw    a0@                     | Clear checksum to zero
	clrl    d0                      | return success
	rts

|
 | fastiphdrchk
 | Check an Internet header for the low-end fast switching support:
 |  - version 4 and header length of 5
 |  - range check length (.ge. 20 and le. 8192))
 |  - validate checksum
 |  - decrement ttl and update checksum
 |  - updated 11/12/92 for DOSXX shared memory performance JCG
 |  - DONT change anything without debugging with a logic analyzer
 |
 | This code needs to be different from the MCI support because the
 | accesses are into shared RAM, which is *extremely* expensive.
 | Therefore, the code uses the minimum number of accesses possible.
 |
 | ushort fastiphdrchk (ushort *s)
 |	
	.globl	_fastiphdrchk

_fastiphdrchk:
        movl    sp@(4),a0               | get pointer to start of header
        movl    d2,sp@(4)               | save d2
        movl    a0@+,d0                 | get first long word of header
        cmpw    #20,d0                  | is total length .ge. 20 bytes
        blt     fastipcrcfail1          | no, too short, fail
        cmpw    #8192,d0                | is total length le. 8192 bytes
        bgt     fastipcrcfail1          | no, too long, fail

        movl    d0,d1                   | save copy of d0 in d1
        roll    #8,d0	                | move memory byte 0 to LSB
        cmpb    #0x45,d0                | is first byte 0x45?
        bne     fastipcrcfail1          | no, fail now.
        movl    a0@+,d0                 | get second long word of header

        movl    a0@+,d2                 | get third long word of header
        roll    #8,d2	                | move memory byte 0 to LSB
	cmpb	#1,d2			| Invalid TTL field?
	bls     fastipcrcfail1          | jump if result <= 1
        subqb   #1,d2                   | decrement TTL field
        rorl    #8,d2	                | realign data in d2

	addl	d0,d1			| sum the data.  longword 2
        clrl    d0                      | used for adding in carrys
	addxl	d0,d1			|   Carry bit
	addl	d2,d1			|  sum the data.  longword 3
	addxl	d0,d1			|    Carry bit
	addl	a0@,d1			|   sum the data.  longword 4
	addxl	d0,d1			|     Carry bit
        addl    sp@(8),d1               |    sum in dest address
	addxl	d0,d1			|      Carry bit

	movw	d1,d0			| copy low checksum
	swap	d1			| get rest in low 16 bits
	addw	d1,d0			| fold them in
	bcc	fastiphdr1		| another carry?
	addqw	#1,d0			| yes, provide it
fastiphdr1:
	notw	d0
	cmpw	#0x100,d0		| Checksum should be this
	bne	fastipcrcfail2		| did not work...
	addw	d2,d0			| subtract (!) out the old checksum
	bcc	fastiphdr2		| another carry?
	addqw	#1,d0			| yes, provide it
fastiphdr2:
	cmpw	#0xFFFF, d0		| Check for illegal negative zero
	beq     fastiphdr_zero		| Optimise for common case (no jump)
fastiphdr3:
	movw	d0,d2			| put new checksum in d2
	movl	d2,a0@-			| store the new checksum and TTL
	clrl	d0			| return success
        movl    sp@(4),d2               | restore d2
	rts

fastipcrcfail2:
        movl    d2,a0@-                 | write the new TTL and original checksum
					| fall through!
fastipcrcfail1:
	moveq	#1,d0			| return failure.
        movl    sp@(4),d2               | restore d2
	rts	

fastiphdr_zero:
	clrw	d0			| Clear checksum to zero
	jmp     fastiphdr3		| Jump back into main path

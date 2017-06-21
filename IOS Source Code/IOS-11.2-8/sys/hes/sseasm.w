/* $Id: sseasm.w,v 3.2 1995/11/17 09:21:17 hampton Exp $		
 * $Source: /swtal/cherf.111/ios/sys/hes/sseasm.w,v $
 *------------------------------------------------------------------
 * sseasm.w - Assembly code for the SSE
 *
 * Sept., 1992  Tony Li
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sseasm.w,v $
 * Revision 3.2  1995/11/17  09:21:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:33:54  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/07/20  04:33:30  bbenson
 * CSCdi37386:  AIP fast instead of silicon switches aal5snap encaps.
 * - Don't check sum the SNAP header with OK!
 *
 * Revision 2.1  1995/06/07  20:45:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


.ORG	0								   ; Start generating
									   ; code at address 0		   
.FOR	1								   ; Use C structure formatting
.SYM	"../hes/ssesym.h"						   ; Save external
									   ; symbols to this file
.PRE	"SSE_GLOBAL_"							   ; Generate external
									   ; symbols as the
									   ; label with this prefix

.REQ	"1.2"								   ; We need at least
									   ; version 1.2 of the
									   ; assembler so
									   ; non-0x constants
									   ; are parsed as decimal
;
; NB: due to a bug in was, don't move this up.
;
#include "../h/encapstype.h"

;
; Editing this file is best done with a window that's at LEAST 100 bytes across.  Comments may
; be either C style, or use ';'.  This file is processed by cpp before being assembled, so you
; may also use cpp macros.  You should use C style comments within macros.
;

;
; Macro definitions to make our life easier.
; OK    == Get the next byte and clock it through the checksum engine
; MATCH == If the current byte doesn't match what we expect, reject it, otherwise get the
;          next byte.
;

#define OK 		RD_BYTE|ACC_CKSUM		  
#define MATCH		NOP,nextpkt 	RD_BYTE	 NOP,nextpkt

;
; Result registers 
;

#define OUTCLASS  	LD_RSLT_0					   /* Packet classification */
#define OUTICB		LD_RSLT_1					   /* Output I/F icb */
#define OUTRWLEN	LD_RSLT_3					   /* Len of outbound
									      MAC rewrite info */
#define OUTSTART	LD_RSLT_4					   /* Offset to start of packet */
									   /* IN.MACLEN */
#define OUTPTR1		LD_RSLT_6					   /* Pointer to rewrite memory */
#define OUTPTR2		LD_RSLT_7					   

;
; Incorrect definitions for IPX TC processing
;
; ------------------------- 
; |A |B |0 |ISO1 Len Offst|
; -------------------------
;  AB = 00  4->4 || 8->8, HC++
;  AB = 01  8->4, HC>>=4
;  AB = 10  4->8 && HC == 0x10, HC = 0x20 
;  AB = 11  4->8 && HC < 0x10, HC = (HC + 1) << 4
;

#define OUTIPXTC	LD_RSLT_8					   /* IPX TC type/SNAP length */	

#define OUTUSAGE1	LD_RSLT_8					   /* Usage counter pointer */
#define OUTUSAGE2	LD_RSLT_9	

#define OUTDST1		LD_RSLT_8					   /* Bean counters */
#define OUTDST2		LD_RSLT_9					   /* for transparent */
#define OUTSRC1		LD_RSLT_A					   /* bridging */
#define OUTSRC2		LD_RSLT_B

;
; Packet classification codes, shared with ucode and system code.
;
#define CLASS_UNKNOWN	0						   /* Send to RP */
#define CLASS_FLUSH	1						   /* Flush it */
#define CLASS_IP	2						   /* It's IP */
#define CLASS_AUTON	3						   /* Send to autonomous switching */
#define CLASS_IPX	4						   /* It's IPX */
#define CLASS_BRIDGE	5						   /* It's 802 bridging */
#define CLASS_SRB	6
#define CLASS_CLNP	0xB
 
;
; Routing protocol mask bits, shared with system code
;

#define ROUTING_ATALK	0x01
#define ROUTING_VINES	0x02
#define ROUTING_APOLLO	0x04
#define ROUTING_NOVELL	0x08
#define ROUTING_IP	0x10
#define ROUTING_DECNET	0x20
#define ROUTING_XNS	0x40
#define ROUTING_CLNS	0x80

;
; Register allocation map
;
; Type 	     Page.Number
; Relocation  	0.0	- IN.ICB
;		0.1	- IP TOS
;		0.2	- IP source address
;		0.3	- IP source address
;		0.4	- IP source address
;		0.5	- IP source address
;		0.6	- IP destination address or IPX hop count
;		0.7	- IP destination address or SRB RIF length
;		0.8	- IP destination address
;		0.9	- IP destination address
;		0.A	- IP fragment offset MSB
;		0.B	- IP fragment offset LSB
;		0.C	- IP protocol number
;		0.D	- IP TCP control bit information
;		0.E	- IP UDP/TCP src port MSB or ICMP/IGMP type
;		0.F	- IP UDP/TCP src port LSB or ICMP code
;		1.0	- IP UDP/TCP dst port MSB
;		1.1	- IP UDP/TCP dst port LSB
; Feedback     	0.0	- Bridging state (0 == dest, 1 == src)
;		0.1	- Routing on interface mask
;		0.2	- Routable (0 == not routable, 1 == routable)
;		0.7	- SRB RIF length (always 0)
; Count		0	- Length of RIF
;		1
;

;
; When we're reset, we need to jump around some of the startup stuff.
; This instruction needs to go here for the simulator since it always starts at location 0.
; On the real hardware, we will also kick the hardware to start at location 0.
;

reset:		<NOP,afterreset>					   

;
; At the end of a packet, we need to send a DONE signal to start the ucode running.  Then we 
; kick the DMA engine.  This has a separate entry point so that some routines can issue the
; DONE themselves.  We then get the machine into a reasonable state for switching the next
; packet.  We reset the checksum engine, disable decrementing the count register, and we reset
; a bunch of the result registers.  By default, we assume that the packet should be handed
; over for autonomous switching, that it should go to the RP, and that we should disable the
; MAC rewrite engine.
;

nextpkt:	<NEXT_DMA,0>						   ; Flush DMA
done:  		<DONE,0x80>						   ; Ok, done with
									   ; packet, use
									   ; reorder regs
		<RD_CKSUM>						   ; Reset checksum

afterreset:	<LD_COUNT0>						   ; XXX temp RST_DEC
;		<RST_DEC>						   ; Reset page
									   ; register to reorder
		<NOP>							   ; Fix timing problem
		<NOP>							   ; with stepping on regs
;
; At the start of every packet, we're fed the ICB of the inbound interface.  Save this in
; relocation register 0.0 for future use.  Jump to the input interface tree.
;

start: 		<RD_BYTE|LD_SREG_0>					   ; Get IN.ICB, save
									   ; in reloc 0
		<OUTCLASS,CLASS_AUTON>					   ; Reset classification
		<OUTICB,0>						   ; Reset packet to go to rp
		<OUTPTR1,0>						   ; Reset rewrite pointer
		<OUTPTR2,0>
		<LD_SPAGE,0>						   ; Use feedback registers
		<LD_SREG_2,inputif,0>					   ; Assume unroutable
;
; When we want to send a packet to the RP, we change the classification to
; CLASS_AUTON and fire it off.  Resetting the mac rewrite address and ICB is probably
; paranoid. 
;

rp:		<OUTPTR1,0>						   ; Reset rewrite pointer
		<OUTPTR2,0>						   ; to not corrupt packet
		<OUTICB,0>						   ; Point to RP
		<RD_CKSUM>						   ; Clear checksummer
    		<OUTCLASS,nextpkt,CLASS_AUTON>				   ; Send to RP

;
; Same as rp, but NEXT_DMA has already been done.
;
punt:		<OUTPTR1,0>						   ; Reset rewrite pointer
		<OUTPTR2,0>						   ; to not corrupt packet
		<OUTICB,0>						   ; Point to RP
		<RD_CKSUM>						   ; Clear checksummer
    		<OUTCLASS,done,CLASS_AUTON>				   ; Send to RP

;
; If we want to go to the rp, and we're in a subroutine, come here.  Instead of jumping to
; "nextpkt", we do a return.
;
deny:		<OUTPTR1,0>						   ; Reset rewrite pointer
		<OUTPTR2,0>						   ; to not corrupt packet
		<OUTICB,0>						   ; Point to RP
		<RD_CKSUM>						   ; Clear checksummer
    		<OUTCLASS,CLASS_AUTON>					   ; Send to RP
;
; Jump here to return from a subroutine.  This just saves us from continually replicating this
; instruction.  Accidentally doing a CALL to this instruction will cause the SSE to misbehave,
; so don't do that.
;

return:		<RET>							   ; Don't call this,
									   ; only jump here

;
; Jump here if something went violently wrong.  When the real hang instruction is implemented,
; use that instead.
;

hang:		<NOP, hang>


;
; Skip table used by transparent bridging if we get a bridging cache lookup failure.  This can
; be used if we're in the source or destination address.  It checks the state in feedback 0.0
; to figure out The Right Thing.  Rewrite the classification just to insure that it can't get
; bridged.  At the end, it does a return so that it can get back to the bridge group tree,
; which will then jump to the correct interface protocol classification tree.
;
tbskip6:	<RD_BYTE>
tbskip5:	<RD_BYTE>
tbskip4:	<RD_BYTE>
tbskip3:	<RD_BYTE>
tbskip2:	<RD_BYTE>
tbskip1:	<RD_BYTE|OUTCLASS,tbchksrc,CLASS_AUTON>
tbskip0:	<OUTCLASS,CLASS_AUTON>
tbchksrc:	<RD_SREG_0,0xff>
	 	<NOP>
	 0x1	RD_BYTE|LD_SREG_0,tbskip5,1	NOP,return		NOP,hang


;
; Ethernet Packets
; We already have the first byte of the MAC header in the pipeline.
; Hard coded tree to deal with the case where bridging is disabled.
;
enobridge:	<RD_BYTE|LD_SREG_2,1>					   ; Skip the rest of the MAC
	  	<RD_BYTE>						   ; header
	  	<RD_BYTE>						   ; Mark it routable,
	  	<RD_BYTE>						   ; as it passed the
	  	<RD_BYTE>						   ; card address filter
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>			  
     	  	<RD_BYTE, ethip>					   ; Now at start of encaps
;
; Start performing protocol classification for Ethernet here.  
; Once the value is decoded, issue an OUTSTART instruction with the entire length of the
; inbound encapsulation, including MAC information.
; Encapsulations supported:
; ARPA:		       0x06 0x00 (XNS)
;		       0x08 0x00 (IP)
;		       0x08 0x06 (ARP)
;		       0x0B 0xAD (Vines)
;		       0x60 0x03 (Decnet)
;		       0x80 0x05 (HP Probe)
;		       0x80 0x9B (Atalk)
;		       0x80 0xC4 (Vines2)
;		       0x80 0xF3 (AARP)
;                      0x81 0x37 (IPX)       
; Novell-Ether:        len  len  0xFF 0xFF (xns checksum for IPX)
; SAP (IEEE 802.2):    len  len  0x06 0x06 0x03 (IP)
;                      len  len  0xE0 0xE0 0x03 (IPX)
;		       len  len  0xFE 0xFE 0x03 (CLNP)
; SNAP:		       len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x06 0x00 (XNS)
;		       len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x08 0x00 (IP)
;                      len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x0B 0xAD (Vines)
;                      len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x60 0x03 (Decnet)
;                      len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0xC4 (Vines2)
;                      len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0xF3 (AARP phase II)
;                      len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x81 0x37 (IPX)
;                      len  len  0xAA 0xAA 0x03 0x08 0x00 0x07 0x80 0x9B (Atalk phase II)
;

;
; Ethernet bridging comes here.  This cycle is needed so that we don't have to
; promote the first compare back into the bridging group tree.  This costs a bridging cycle,
; but those we have to burn.
;
beth:	<NOP>

;
; Byte 0: First, look for ARPA encapsulated IP
;
ethip:	0x08 	NOP,ethxns	RD_BYTE				NOP,ethipx 
	0x00 	NOP,nextpkt	OUTSTART,iphd,0x0e 		NOP,etharp

;
; Next, look for ARPA encapsulated IPX
;
ethipx	0x81	NOP,ethatlk	RD_BYTE				NOP,nextpkt
	0x37	NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x0e	NOP,nextpkt

;
; Look for Atalk
;
ethatlk 0x80	NOP,ethdec	RD_BYTE				NOP,nextpkt
	0x9b	NOP,ethpro	RD_BYTE|OUTSTART,atalkhd,0x0e	NOP,ethvint

;
; Look for Decnet
;
ethdec	0x60	NOP,ethvin	RD_BYTE				NOP,nextpkt
	0x03	NOP,nextpkt	RD_BYTE|OUTSTART,decnethd,0x0e	NOP,nextpkt

;
; Look for Vines
;
ethvin	0x0b	MATCH
	0xad	NOP,nextpkt	RD_BYTE|OUTSTART,vineshd,0x0e	NOP,nextpkt

;
; Look for XNS
;
ethxns	0x06	NOP,ethlen	RD_BYTE				NOP,nextpkt
	0x00	NOP,nextpkt	RD_BYTE|OUTSTART,xnshd,0x0e	NOP,nextpkt			

;
; If we got here, it is either SAP or SNAP.  Make sure that the input length is something
; sensical.  Look for <= 1524
;
ethlen  0x05 	RD_BYTE,elenok 	RD_BYTE				NOP,nextpkt
     	0xf4 	RD_BYTE,sapip  	RD_BYTE,sapip 			NOP,nextpkt
elenok	  	<RD_BYTE,sapip>

;
; Byte 1: Look for ARP.  This guarantees that we never bridge ARP.
;
etharp	0x06	NOP,nextpkt	NOP,rp				NOP,nextpkt

;
; Look for HP probe
;
ethpro	0x05	NOP,nextpkt	NOP,rp				NOP,nextpkt

;
; Look for Vines 2
;
ethvint	0xc4	NOP,nextpkt	RD_BYTE|OUTSTART,vineshd,0x0e	NOP,ethaarp

;
; Look for AARP
;
ethaarp	0xf3	NOP,nextpkt	RD_BYTE|OUTSTART,atalkhd,0x0e	NOP,nextpkt

;
; Byte 2: Now look for SAP encapsulated IP
;
sapip: 	0x06 	NOP,nextpkt  	RD_BYTE				NOP,novsap
     	0x06 	MATCH
     	0x03 	NOP,nextpkt	OUTSTART,iphd,0x11		NOP,nextpkt; Encaps was 17 bytes

;
; Next, look for SAP encapsulated IPX
;
novsap  0xe0	NOP,ethsnap	RD_BYTE				NOP,osisap ; Look for Novell in
	0xe0	MATCH							   ; SAP 
	0x03	NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x11	NOP,nextpkt; Encaps was 17 bytes 

;
; Ok, mebbe it's CLNP.
;
osisap	0xfe	NOP,nextpkt	RD_BYTE				NOP,noveth ; Look for CLNP
	0xfe	MATCH
	0x03	NOP,nextpkt	OUTSTART,clnphd,0x11		NOP,nextpkt; Encaps was 17 bytes

;
; If we got here, it's Novell's broken Novell-Ether encapsulation.
;
noveth  0xff	NOP,nextpkt	RD_BYTE				NOP,nextpkt; Look for Novell
	0xff	NOP,nextpkt	RD_BYTE|OUTSTART,ipxcksum,0x0e	NOP,nextpkt; ether

;
; If we got here, we expect that it's SNAP encapsulated.  Skip to the ethertype.  All of these
; encapsulations are 22 bytes long.
;
ethsnap	0xaa 	MATCH
     	0xaa 	MATCH
     	0x03 	MATCH
     	0x00 	NOP,nextpkt	RD_BYTE				NOP,apple
     	0x00 	MATCH
     	0x00 	NOP,nextpkt	RD_BYTE,snapip			NOP,nextpkt

;
; Look for Apple's OUI
;
apple	0x08	MATCH
	0x00	MATCH
	0x07	MATCH
;
; Look for Atalk phase II
;
	0x80	MATCH
	0x9b	NOP,nextpkt	RD_BYTE|OUTSTART,atalkhd,0x16	NOP,nextpkt

;
; Byte 8: Ok, now look for IP.
; Warning: FDDI SNAP encapsulations also come here.
;
snapip:	0x08 	NOP,snapxns	RD_BYTE				NOP,snapnov
     	0x00 	NOP,nextpkt	OUTSTART,iphd,0x16		NOP,nextpkt

;
; Look for XNS
;
snapxns	0x06	MATCH
	0x00	NOP,nextpkt	RD_BYTE|OUTSTART,xnshd,0x16	NOP,nextpkt

;
; Look for IPX
;
snapnov 0x81	NOP,snapvin	RD_BYTE				NOP,nextpkt
	0x37	NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x16	NOP,nextpkt

;
; Look for Vines 2
;
snapvin	0x80	NOP,snapvi	RD_BYTE				NOP,nextpkt
	0xc4	NOP,nextpkt	RD_BYTE|OUTSTART,vineshd,0x16	NOP,snapaarp

;
; Look for Vines
;
snapvi	0x0b	NOP,nextpkt	RD_BYTE				NOP,snapdec
	0xad	NOP,nextpkt	RD_BYTE|OUTSTART,vineshd,0x16	NOP,nextpkt

;
; Look for Decnet
;
snapdec	0x60	MATCH
	0x03	NOP,nextpkt	RD_BYTE|OUTSTART,decnethd,0x16	NOP,nextpkt

;
; Byte 9: Look for AARP
;
snapaarp 0xF3	NOP,nextpkt	RD_BYTE|OUTSTART,atalkhd,0x16	NOP,nextpkt


;
; FDDI Packets
; We already have the first byte of the MAC header in the pipeline.
; Hard coded tree to support the case where bridging is disabled.
;

fnobridge:	<RD_BYTE|LD_SREG_2,1>					   ; Skip the rest of the MAC
	  	<RD_BYTE>						   ; header

		<RD_BYTE>						   ; Warning - kludge alert!
		<RD_BYTE>						   ; Offset two bytes
									   ; for ucode problem
	  	<RD_BYTE>						   ; Mark it routable
	  	<RD_BYTE>						   ; as it has passed
	  	<RD_BYTE>						   ; the interface
	  	<RD_BYTE>						   ; address filter
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>
	  	<RD_BYTE>			  
     	  	<RD_BYTE,fencap>					   ; Now at start of encaps

;
; Start performing protocol classification for FDDI here.  
; Once the value is decoded, issue an OUTSTART instruction with the entire length of the
; inbound encapsulation, including MAC information.
; Add two bytes to the inbound encapsulation length to ignore the leading FC and pad byte.
; All SNAP encapsulations are 22 bytes.
; Encapsulations supported:
; SAP:		       0xE0 0xE0 0x03 (IPX)
;		       0xFE 0xFE 0x03 (CLNP)
; Novell-FDDI:	       0xFF 0xFF (IPX)
; SNAP:		       0xAA 0xAA 0x03 0x00 0x00 0x00 0x06 0x00 (XNS)
;		       0xAA 0xAA 0x03 0x00 0x00 0x00 0x08 0x00 (IP)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x0B 0xAD (Vines)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x60 0x03 (Decnet)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0x9B (Atalk)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0xC4 (Vines2)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0xF3 (AARP both phases)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x81 0x37 (IPX)
;                      0xAA 0xAA 0x03 0x08 0x00 0x07 0x80 0x9B (Atalk phase II)
;

;
; FDDI bridging comes here
;
bfddi:	<NOP>
 
;
; Byte 0:  Assume SNAP
;
fencap:	0xaa 	NOP,nextpkt	RD_BYTE				NOP,fsap
     	0xaa 	MATCH
     	0x03 	MATCH
     	0x00	NOP,nextpkt	RD_BYTE				NOP,apple  ; Apple in ether
									   ; tree 
     	0x00 	MATCH
     	0x00 	NOP,nextpkt	RD_BYTE,snapip			NOP,nextpkt

;
; Byte 0: Look for SAP encapsulations.  Start with CLNP.
;
fsap	0xfe	NOP,fipx	RD_BYTE				NOP,fnov
	0xfe	MATCH
	0x03	NOP,nextpkt	OUTSTART,clnphd,0x11		NOP,nextpkt; Encaps was 15
									   ; bytes + 2 byte
									   ; fudge
;
; Look for IPX in raw encapsulation
;
fnov	0xff	MATCH
	0xff	NOP,nextpkt	OUTSTART,ipxcksum,0xe		NOP,nextpkt

;
; Look for IPX in SAP
;
fipx	0xe0	MATCH
	0xe0	MATCH
	0x03	NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x11	NOP,nextpkt


;
; ATM packets
; Only routing is supported.  AAL34SMDS, AAL5SNAP, AAL5NLPID, ATM LANE and AAL5MUX are supported.
; Encapsulations supported:
; Mux:          vci vci 0x08 0x00 (IP)
;               vci vci 0x81 0x37 (IPX)
; SNAP:         vci vci 0x00 0x00 0xAA 0xAA 0x03 0x00 0x00 0x00 0x08 0x00 (IP)
;               vci vci 0x00 0x00 0xAA 0xAA 0x03 0x00 0x00 0x00 0x81 0x37 (IPX)
; SMDS:		vci vci 0x00 0x01 [36 bytes of SMDS header] [SNAP header] 0x08 0x00 (IP) 
;               vci vci 0x00 0x01 [36 bytes of SMDS header] [SNAP header] 0x81 0x37 (IPX)
;               vci vci 0x00 0x01 [36 bytes of SMDS header] 0xFE 0xFE (CLNP)
; NLPID:	vci vci 0x00 0x02 0x03 0x81 (CLNP)
;		vci vci 0x00 0x02 0x03 0xCC (IP)
;		vci vci 0x00 0x02 0x03 0x00 0x80 0x00 0x00 0x00 0x81 0x37 (IPX)
; LANE-ARPA:    vcd vcd 0x00 0x06 leci leci (DA - 6 bytes) (SA - 6 bytes) 0x08 0x00 (IP)
;               vcd vcd 0x00 0x06 leci leci (DA - 6 bytes) (SA - 6 bytes) 0x81 0x37 (IPX)
;
; LANE-SNAP:    vcd vcd 0x00 0x06 leci leci (DA - 6 bytes) (SA - 6 bytes) 
;			len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x08 0x00 (IP) 
;               vcd vcd 0x00 0x06 leci leci (DA - 6 bytes) (SA - 6 bytes)
;			len  len  0xAA 0xAA 0x03 0x00 0x00 0x00 0x81 0x37 (IPX) 
;
; LANE-SAP:	vcd vcd 0x00 0x06 leci leci (DA - 6 bytes) (SA - 6 bytes)
;                       len  len  0xE0 0xE0 0x03 (IPX)
;
; LANE-NOVELL1: vcd vcd 0x00 0x06 leci leci (DA - 6 bytes) (SA - 6 bytes)
;			len  len  0xFF 0xFF (IPX)
;
; LANE_CNTL:    vcd vcd 0x00 0x06 0xFF leci (DATA)
; LANE_BUS:     vcd vcd 0x00 0x0E leci leci (DATA)
;
; NB: LANE_BUS and LANE_CNTL-marker packets are shot into the autonomous
; path for classification by the SSP ucode en route to the RP.
;

atm:            <RD_BYTE|LD_SREG_2,1>
                <RD_BYTE>

muxip		0x08  NOP,asnap       RD_BYTE			NOP,muxipx
		0x00  NOP,nextpkt     OUTSTART,iphd,4		NOP,nextpkt

muxipx		0x81  MATCH
		0x37  NOP,nextpkt     RD_BYTE|OUTSTART,ipxhd,4	NOP,nextpkt

asnap		0x00  MATCH
		0x00  NOP,nextpkt	RD_BYTE			NOP,atmsmds
		0xAA  NOP,nextpkt	RD_BYTE			NOP,atmclnp
		0xAA  MATCH
		0x03  MATCH
		0x00  MATCH
		0x00  MATCH
		0x00  MATCH

;
; Look for RFC 1483 SNAP IP
;
		0x08  NOP,nextpkt  	RD_BYTE			NOP,atmipx
		0x00  NOP,nextpkt  	OUTSTART,iphd,0xC	NOP,nextpkt

;
; Look for RFC 1483 SNAP IPX
;
atmipx		0x81  MATCH
		0x37  NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0xC	NOP,nextpkt

;
; Look for RFC 1483 SNAP CLNP
;
atmclnp		0xFE  MATCH
		0xFE  MATCH
		0x03  NOP,nextpkt	OUTSTART,clnphd,7	NOP,nextpkt

;
; Look for AAL34SMDS - the encapsulation that makes your DS3 look like DS1
;
atmsmds	  0x01	NOP,nextpkt	RD_BYTE				NOP,atmnlpid
;
; Skip all 36 bytes of the SMDS header
;
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
		<RD_BYTE>
;
; End of SMDS encap (whew!), now look for a SNAP header.
;
	  0xaa 	NOP,nextpkt	RD_BYTE				NOP,asmdsosi
     	  0xaa 	MATCH
     	  0x03 	MATCH
     	  0x00 	MATCH
     	  0x00 	MATCH
     	  0x00 	MATCH

;
; Look for IP
;
     	  0x08 	NOP,nextpkt  	RD_BYTE				NOP,asmdsipx
     	  0x00 	NOP,nextpkt  	OUTSTART,iphd,0x30		NOP,nextpkt

;
; Look for IPX
;
asmdsipx  0x81	MATCH
	  0x37  NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x30	NOP,nextpkt

;
; Look for CLNP
;
asmdsosi  0xfe	MATCH
	  0xfe	MATCH
	  0x3	NOP,nextpkt	OUTSTART,clnphd,0x2b		NOP,nextpkt

;
; Look for CLNP in NLPID encapsulation.  Do NOT count the NLPID as part of the encapsulation.
;
atmnlpid:	0x02  NOP,nextpkt	RD_BYTE			NOP,alane
		0x03  MATCH
		0x81  NOP,anlpidoui	OUTSTART,clnpnlpid,5	NOP,anlpidip

;
; Look for IP
;
anlpidip	0xCC  NOP,nextpkt	OUTSTART,iphd,6		NOP,nextpkt

;
; Look for IPX
;
anlpidoui	0x00  MATCH
		0x80  MATCH
		0x00  MATCH
		0x00  MATCH
		0x00  MATCH 
		0x81  MATCH
		0x37  NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0xC	NOP,nextpkt

alane  		0x06  NOP,nextpkt       RD_BYTE			NOP,alanebus

; If a LEC marker in this packet give it up to the RP
		0xFF  RD_BYTE         NOP,rp			RD_BYTE

; Otherwise, clock thru the MAC addresses and then try to switch
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>
                <RD_BYTE>

alaneip   	0x08  NOP,alanelen	RD_BYTE			NOP,alaneipx
		0x00  NOP,nextpkt	OUTSTART,iphd,0x14	NOP,nextpkt

alaneipx	0x81  MATCH
		0x37  NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x14     NOP,nextpkt

; Looks like 802.3.  Check the length <= 1524.
alanelen	0x05  RD_BYTE,alanelenok	RD_BYTE		NOP,nextpkt
		0xF4  RD_BYTE,alanesap	RD_BYTE,alanesap	NOP,nextpkt

alanelenok      <RD_BYTE,alanesap>

; IPX SAP encaps
alanesap	0xe0  NOP,alanesnap	RD_BYTE			NOP,alanenoveth
		0xe0  MATCH
		0x03  NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x17	NOP,nextpkt

; Handle the broken Novell raw encaps.  Pass encap length that doesn't count SAPs.
alanenoveth	0xFF  NOP,nextpkt	RD_BYTE			NOP,nextpkt
		0xFF  NOP,nextpkt	RD_BYTE|OUTSTART,ipxcksum,0x14	NOP,nextpkt

; Else, we sense a SNAP header coming on 
alanesnap	0xAA  MATCH
		0xAA  MATCH
		0x03  MATCH
		0x00  MATCH
		0x00  MATCH
		0x00  MATCH

alaneips	0x08  NOP,nextpkt	RD_BYTE			NOP,alaneipxs
		0x00  NOP,nextpkt	OUTSTART,iphd,0x1C	NOP,nextpkt

alaneipxs	0x81  MATCH
		0x37  NOP,nextpkt	RD_BYTE|OUTSTART,ipxhd,0x1C     NOP,nextpkt

alanebus	0x0E  NOP,nextpkt	NOP,rp			NOP,nextpkt


;
; IBM Channel (ESCON, Bus&Tag) packets
; Only routing is supported.  Packet format looks like AAL5MUX.
; Encapsulations supported:
; Mux:		vci vci 0x08 0x00 (IP)
;

channel:	<RD_BYTE|LD_SREG_2,1>
		<RD_BYTE>

channelip: 0x08	MATCH	
	   0x00 NOP,nextpkt	OUTSTART,iphd,4			NOP,nextpkt



;
; Token Wrong packets
; These are sufficiently odd that it's worth reproducing here:
; Frame header | Destination MAC | Source MAC | RIF (optional) | SNAP 
;	2		6		6	    2-18
; RIF is present if bit 0x80 of the 1st byte of source mac is set.
;

trbridge:	<RD_BYTE,trfcbyte>					   ; skip frame header
									   ; expect to bridge
tnobridge: 	<RD_BYTE|LD_SREG_2,1>					   ; skip frame header
									   ; (AC byte) 
trfcbyte	<RD_BYTE,trdestmac>					   ; skip FC byte	
trskip0:	<NOP,trsrcmac>						   ; no bytes to skip
trskip6:	<RD_BYTE>						   ; skip destination
									   ; MAC (1st byte)
trskip5:	<RD_BYTE>						   ; 2nd dest byte
trskip4:	<RD_BYTE>						   ; 3rd dest byte
trskip3:	<RD_BYTE>						   ; 4th dest byte
trskip2:	<RD_BYTE>						   ; 5th dest byte
trskip1:	<RD_BYTE>						   ; 6th dest byte
;
; Look for the RIF.  If bit 0x80 of the first byte of the source is set, then a RIF is
; present.
;
trsrcmac:0x80	RD_BYTE,tnorif	RD_BYTE,rif	RD_BYTE,rif		   ; 1st source byte

;
; No rif present.  Mark the packet routable as it must have (?) passed the interface address
; filter.  All hell breaks loose if transparent bridging is on the interface and we get here.
;
tnorif		<RD_BYTE>						   ; 2nd source byte
		<RD_BYTE|LD_SREG_7,0>					   ; 3rd source byte
		<RD_BYTE>						   ; 4th source byte
		<RD_BYTE>						   ; 5th source byte
		<RD_BYTE,tencap>					   ; 6th source byte

;
; Get to the start of the RIF.
;
rif:		<RD_BYTE>						   ; 2nd source byte
		<RD_BYTE>						   ; 3rd source byte
		<RD_BYTE>						   ; 4th source byte
		<RD_BYTE|LD_SPAGE,0x80>					   ; 5th source byte
									   ; use relocation
		<RD_BYTE|LD_SREG_7>					   ; 6th source byte
;
; Now at the start of the routing control field.  Look for bit pattern 0x0x xxxx in the first
; byte.  This indicates a packet that is using a real source route and is not using path
; control. 
;
	0x80	AND_PIPE,0x3f	NOP,rp		NOP,rp			   ; Bounce explorers
		<NOP>							   ; wait for the AND
	0x20	NOP		NOP,rp		NOP,rp			   ; Bounce path
									   ; control
;
; The length of the RIF sits in the 0x1f bits of the first byte of the two control bytes.  This
; is unfortunate since LD_COUNT won't do quite what we want.  Especially since the length
; includes the control bytes.  The hack here is fun: count the first control byte, but after a
; match, read one more byte after we're done.  If we auto-return between the match and saving
; the state, then we matched AFTER the RIF.  But the auto-return insures that we don't store a
; useful result.  We make a CALL into the SRB destination tree, and if the count expires we
; auto-return.    
;
		<LD_COUNT0>						   ; Set up the safety
									   ; net
		<SET_DEC>						   ; Start the count
									   ; register
		<DEC_COUNT>						   ; Count includes
									   ; this byte
	0x12	NOP		NOP		NOP,nextpkt		   ; > 18 is illegal
	0x6	NOP,nextpkt	AND_PIPE,0x1	AND_PIPE,0x1		   ; Bail if < 6, it's
									   ; illegal
		<NOP>							   ; time for and
	0x1	RD_BYTE|LD_SPAGE,0 NOP,nextpkt	NOP,nextpkt		   ; Bail if RIF length
									   ; is odd, skip 2nd
									   ; control byte.
									   ; Switch to feedback
									   ; regs 
;
; See if the packet is destined to one of our MAC addresses.  If it is, skip over the RIF
; and try to route it. The first decision that the SRB tree has to make is based on the input
; ICB.  Load that up now.  The decision tree first branches on the input ICB and then walks the
; RIF.  Note that at this point, NONE of the RIF has been seen yet.
;
 		<RD_SREG_2,1>						   ; See if it's for us
		<NOP>							   ; 
	0x1	LD_SPAGE,trnotus,0x80 LD_SPAGE,0x80 NOP,nextpkt	   	   ; Switch back to
									   ; reloc regs.  If
									   ; it's for us,
		<NOP>							   ; try to route it,
	0xff	CALL,skiprif	CALL,skiprif	NOP,tencap		  
									   ; else try SRB
trnotus:	<RD_SREG_0,0xff>					   ; Get input icb
	0xff	CALL,srbdest	CALL,srbdest	NOP,nextpkt		   ; Look at the RIF

goodsrb:	<OUTCLASS,CLASS_SRB>					   ; Come here if we
									   ; have a successful
									   ; RIF match.
;
; We have to rewrite the ACFC bytes at the start of the frame in case someone is setting ring
; monitor bits.  Point to the preloaded rewrite, starting at byte 4 (it needs to be shifted
; before sending to the rewrite engine).
;

		<OUTPTR1,1>						   ; Location 4
		<OUTPTR2,0>									   
		<OUTSTART,1>
skiprif:	<RD_BYTE,skiprif>					   ; Loop until end of RIF

;
; Token Ring encapsulations
; Try routing this packet.
;
; Encapsulations supported:
; SNAP:		       0xAA 0xAA 0x03 0x00 0x00 0x00 0x06 0x00 (XNS)
;		       0xAA 0xAA 0x03 0x00 0x00 0x00 0x08 0x00 (IP)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x0B 0xAD (Vines)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x60 0x03 (Decnet)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0xF3 (AARP)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x80 0xC4 (Vines)
;                      0xAA 0xAA 0x03 0x00 0x00 0x00 0x81 0x37 (IPX)
;                      0xAA 0xAA 0x03 0x08 0x00 0x07 0x80 0x9B (Atalk)
; Novell-Token:	       0xFF 0xFF (IPX)
; SAP:		       0x06 0x06 0x03 (IP)
;		       0xBC 0xBC 0x03 (Vines)
;		       0xE0 0xE0 0x03 (IPX)
; 		       0xFE 0xFE 0x03 (CLNP)
;
; Funky stuff happens here!  If there is a RIF, we might have a variable length encapsulation.
; The length of the RIF is stored in SREG 7.  Add the given length to SREG 7 and use that as
; the encapsulation size.
;
tencap	0xaa	NOP,trsapip	RD_BYTE			NOP,novtr
	0xaa	MATCH
	0x03	MATCH
     	0x00	NOP,nextpkt	RD_BYTE			NOP,tapple 
	0x00	MATCH
	0x00	MATCH

;
; Byte 6: Ok, now look for IP.
;
tsnapip:0x08 	NOP,tsnapxns	RD_BYTE			NOP,tsnapnov
     	0x00 	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP,iphd

;
; Look for XNS
;
tsnapxns 0x06	MATCH
	0x00	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,xnshd>

;
; Look for IPX
;
tsnapnov 0x81	NOP,tsnapvin	RD_BYTE			NOP,nextpkt
	0x37	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,ipxhd>

;
; Look for Vines 2
;
tsnapvin 0x80	NOP,tsnapvi	RD_BYTE			NOP,nextpkt
	0xc4	NOP,nextpkt	NOP			NOP,tsnapaarp
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,vineshd>

;
; Look for Vines
;
tsnapvi	0x0b	NOP,nextpkt	RD_BYTE			NOP,tsnapdec
	0xad	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,vineshd>

;
; Look for Decnet
;
tsnapdec 0x60	MATCH
	0x03	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,decnethd>

;
; Byte 7: Look for AARP
;
tsnapaarp 0xF3	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,atalkhd>

; Byte 3: Look for Apple's OUI
tapple	0x08	MATCH
	0x00	MATCH
	0x07	MATCH
	0x80	MATCH
	0x9B	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add22	CALL,add22		NOP
		<RD_BYTE,atalkhd>

;
; Byte 0: Look for Novell-TR
;
novtr	0xE0	NOP,trsapvin	RD_BYTE			NOP,trosi
	0xE0	MATCH
	0x03	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add17	CALL,add17		NOP
		<RD_BYTE,ipxhd>

;
; Look for CLNP
;
trosi	0xfe	NOP,nextpkt	RD_BYTE			NOP,trripx
	0xfe	MATCH
	0x03	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add17	CALL,add17		NOP,clnphd

;
; Look for raw IPX
;
trripx	0xff	MATCH
	0xff	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add16	CALL,add16		NOP,ipxcksum
	
;
; Look for IP in sap
;
trsapip	0x06	MATCH
	0x06	MATCH
	0x03	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add17	CALL,add17		NOP,iphd

;
; Look for Vines in SAP
;
trsapvin 0xbc	MATCH
	0xbc	MATCH
	0x03	NOP,nextpkt	NOP			NOP,nextpkt
	0xff	CALL,add17	CALL,add17		NOP,vineshd

;
;
; Mac address addition trees.  
; Perform the following calculation:
;	OUTSTART <- RD_SREG_7 + constant
; Note that these routines should have the correct value loaded into SREG 7, and that these
; will be called with either relocation or feedback registers, as appropriate.
; Acceptable lengths are even numbers between 6 and 18, as well as 0.  These trees are hand
; balanced, and slanted slightly for shorter RIFs.
;
; Possible values: 0 6 8 10 12 14 16 18 
;

adddone:	<LD_SPAGE,return,0>					   ; Force feedback registers

add22:		<RD_SREG_7,0x1f>					   ; Get length, mask
		<NOP>							   ; with to get len
									   ; bits
  	0xa	NOP		OUTSTART,adddone,0x20	NOP,add22a
	6	OUTSTART,adddone,0x16 OUTSTART,adddone,0x1c OUTSTART,adddone,0x1e
add22a	0xe	OUTSTART,adddone,0x22 OUTSTART,adddone,0x24	NOP
	0x10	NOP,rp		OUTSTART,adddone,0x26	OUTSTART,adddone,0x28

add17:		<RD_SREG_7,0x1f>					   ; Get length, mask
		<NOP>							   ; with to get len
									   ; bits
  	0xa	NOP		OUTSTART,adddone,0x1b	NOP,add17a
	6	OUTSTART,adddone,0x11 OUTSTART,adddone,0x17 OUTSTART,adddone,0x19
add17a	0xe	OUTSTART,adddone,0x1d OUTSTART,adddone,0x1f	NOP
	0x10	NOP,rp		OUTSTART,adddone,0x21	OUTSTART,adddone,0x23

add16:		<RD_SREG_7,0x1f>					   ; Get length, mask
		<NOP>							   ; with to get len
									   ; bits
  	0xa	NOP		OUTSTART,adddone,0x1a	NOP,add16a
	6	OUTSTART,adddone,0x10 OUTSTART,adddone,0x16 OUTSTART,adddone,0x18
add16a	0xe	OUTSTART,adddone,0x1c OUTSTART,adddone,0x1e	NOP
	0x10	NOP,rp		OUTSTART,adddone,0x20	OUTSTART,adddone,0x22


;
; MIP interface
; The MIP, like ATM, supports multiple encapsulations per hardware interface.  The hardware is
; nice enough to tell us which encapsulation it's using.  The following code expects HDLC, PPP,
; FR and FR-IETF.  Most of this is branching to other routines for parsing the "regular"
; encapsulation.  The exceptions are FR SNAP because of the stupid padding and FR CLNS due
; to the FFB.
;
; Encapsulation codes are taken from h/encapstype.h:
;	HDLC 	- ET_HDLC
;	PPP  	- ET_PPP
;	FR   	- ET_FRAME_RELAY
;	FR-IETF	- ET_FR_IETF
;
; The second byte is not significant.
;
; Note that the coding here is linear to make us truly independent of the values chosen for
; particular encapsulation types.  ;-(
;
mip:		<LD_SREG_2,1>						   ; Mark routable
		<NOP>
;
; First look for HDLC
;
	ET_HDLC NOP,mipfr	RD_BYTE			NOP,mipfr	   ; Check HDLC
		<RD_BYTE>
		<RD_BYTE|OUTSTART,0x6>
	  	<RD_BYTE>
;
; Look for CLNP.  Because of the FFB, we need to set outstart to 7 here.  However, if we fail
; here, we can go run the rest of the normal HDLC pattern
;
	0xfe	NOP,hdlcip	RD_BYTE				NOP,hdlcip
	0xfe	NOP,nextpkt	RD_BYTE|OUTSTART,clnphd,0x7	NOP,nextpkt; Fucking fudge byte

;
; Look for FR
;
mipfr ET_FRAME_RELAY NOP,mipfrietf RD_BYTE		NOP,mipfrietf	   ; Check FR
mipfrcont	<RD_BYTE|OUTSTART,0x6>					   ; skip control
		<RD_BYTE>
	  	<RD_BYTE>

;
; Byte 0: Look for IP
;
	0x03 	NOP,nextpkt	RD_BYTE			NOP,mipfrclnp
	0xCC 	NOP		NOP,iphd		NOP,nextpkt	   ; Ok, it's IP

;
; Byte 3: Look for SNAP (caution, fell through from above)
;
	0x80	NOP,mipfrpad	RD_BYTE|OUTSTART,frsnap,0xb 	NOP

;
; Look for CLNP (caution, fell through from above)
;
	0x81	NOP,nextpkt	NOP,clnphd		NOP,nextpkt	   ; Ok, it's CLNP

; 
; Look for a pad byte
;
mipfrpad 0x00	MATCH
	 0x80	NOP,nextpkt	RD_BYTE|OUTSTART,frsnap,0xc	NOP,nextpkt

;
; Look for CLNS as 0xFEFE
;
mipfrclnp 0xFE	NOP,frcisco     RD_BYTE				NOP,nextpkt
	  0xFE	NOP,nextpkt	OUTSTART|RD_BYTE,clnphd,0x7	NOP,nextpkt	   ; Fucking fudge byte


;
; Look for FR IETF
;
mipfrietf ET_FR_IETF NOP,mipppp	RD_BYTE,mipfrcont		NOP,mipppp	   ; Check FR IETF

;
; MIP PPP packets - A whole lot like normal PPP, but with different lengths.
; Only routing is supported
; Encapsulations supported:
; PPP:	       xxxx xxxx 0x00 0x21 (IP)
;	       xxxx xxxx 0x00 0x23 (CLNP)     Note: We don't look for the optional 1-3 null pads
;              xxxx xxxx 0x00 0x2b (IPX)

mipppp	ET_PPP	NOP,nextpkt	RD_BYTE			NOP,nextpkt	   ; Check PPP
		<RD_BYTE|OUTSTART,0x6>					   ; Skip VC
	     	<RD_BYTE,pppcont>					   ; Skip control

;
; HDLC packets
; Only routing is supported
; Encapsulations supported:
; HDLC:	       xxxx xxxx 0x08 0x00 (IP)
;              xxxx xxxx 0x81 0x37 (IPX)
;	       xxxx xxxx 0xFE 0xFE xxxx (CLNP)
;

hdlc:     	<LD_SREG_2,1>						   ; Mark routable
		<RD_BYTE|OUTSTART,0x4>					   ; Skip control
	  	<RD_BYTE>

;
; Look for IP
;
hdlcip	0x08 	NOP,nextpkt  	RD_BYTE  		NOP,hdlcipx
	0x00 	NOP,nextpkt	NOP,iphd		NOP,nextpkt	   ; Ok, it's IP

;
; Look for IPX
;
hdlcipx	0x81	NOP,nextpkt	RD_BYTE			NOP,hdlcosi
	0x37	NOP,nextpkt	RD_BYTE,ipxhd		NOP,nextpkt

;
; Look for CLNP
;
hdlcosi 0xfe	MATCH
	0xfe	NOP,nextpkt	RD_BYTE|OUTSTART,clnphd,0x5	NOP,nextpkt; Fucking fudge byte

;
; PPP packets - 
; Only routing is supported
; Encapsulations supported:
; PPP:	       xxxx xxxx 0x00 0x21 (IP)
;	       xxxx xxxx 0x00 0x23 (CLNP)  Note: We don't look for the optional 1-3 null pads
;              xxxx xxxx 0x00 0x2b (IPX)

ppp:		<LD_SREG_2,1>						   ; Mark routable
	     	<RD_BYTE|OUTSTART,0x4>					   ; Skip control
pppcont	  	<RD_BYTE>

;
; Look for IP
;
	0x00 	MATCH
	0x21 	NOP,nextpkt	NOP,iphd		NOP,pppipx	   ; Ok, it's IP

;
; Look for IPX
;
pppipx	0x2b	NOP,ppposi	RD_BYTE,ipxhd		NOP,nextpkt	   ; Ok, it's IPX

;
; Look for CLNP
;
ppposi	0x23	NOP,nextpkt	NOP,clnphd		NOP,nextpkt	   ; Ok, it's CLNP
;	0x00	NOP,nextpkt	RD_BYTE			OUTSTART,clnphd,0x4
;	0x00	NOP,nextpkt	RD_BYTE			OUTSTART,clnphd,0x5
;	0x00	NOP,nextpkt	RD_BYTE			OUTSTART,clnphd,0x6
;	0x00	NOP,nextpkt	RD_BYTE			OUTSTART,clnphd,0x7


;
; Frame Relay packets 
;
; cisco encapsulation
; Only routing is supported
; Encapsulations supported:
; FR cisco:    xxxx xxxx 0x08 0x00 (IP)
;	       xxxx xxxx 0x81 0x37 (IPX)
;	       xxxx xxxx 0xFE 0xFE (CLNP)
; FR IETF:     xxxx xxxx 0x03 0xCC (IP)
;	       xxxx xxxx 0x03 0x81 (CLNP)
;	       xxxx xxxx 0x03 0x80 0x00 0x00 0x00 0x81 0x37 (IPX)
;	       xxxx xxxx 0x03 0x00 (padded snap)
;	       xxxx xxxx 0x03 0x00 0x80 0x00 0x00 0x00 0x81 0x37 (IPX)
;

frame:		<LD_SREG_2,1>						   ; Mark routable
	     	<RD_BYTE|OUTSTART,0x4>					   ; Skip control
	  	<RD_BYTE>


;
; Byte 0: Look for IP
;
	0x03 	NOP,nextpkt	RD_BYTE			NOP,frcisco
	0xCC 	NOP		NOP,iphd		NOP,nextpkt	   ; Ok, it's IP

;
; Byte 3: Look for SNAP (caution, fell through from above)
;
	0x80	NOP,frpad	RD_BYTE|OUTSTART,frsnap,0x9 NOP

;
; Look for CLNP (caution, fell through from above)
;
	0x81	NOP,nextpkt	NOP,clnphd		NOP,nextpkt	   ; Ok, it's CLNP

; 
; Look for a pad byte
;
frpad	0x00	MATCH
	0x80	NOP,nextpkt	RD_BYTE|OUTSTART,0xa	NOP

;
; Look for the remainder of the snap header
;
frsnap		<NOP>
	0x00	MATCH
	0x00	MATCH
	0x00	MATCH

;
; Now look for IPX
;
	0x81	MATCH
	0x37	NOP,nextpkt	RD_BYTE,ipxhd		NOP,nextpkt	
;
; Look for IP
;
frcisco:0x08 	NOP,nextpkt	RD_BYTE			NOP,fripx
	0x00 	NOP,nextpkt	NOP,iphd		NOP,nextpkt	   ; Ok, it's IP

;
; Look for IPX as 0x8137
;
fripx	0x81	NOP,nextpkt	RD_BYTE			NOP,frclnp
	0x37	NOP,nextpkt	RD_BYTE,ipxhd		NOP,nextpkt

;
; Look for CLNS as 0xFEFE
;
frclnp  0xFE	MATCH
	0xFE	NOP,nextpkt	OUTSTART|RD_BYTE,clnphd,5 NOP,nextpkt	   ; Fucking Fudge Byte

;
; IP headers.
;

iphd: 		<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
ippatch:1	NOP,nextpkt 	RD_SREG_1,ROUTING_IP	NOP,nextpkt	   ; See if we route IP
		<NOP>							   ; on this i/f
	1	NOP,rp		OK			OK		   ; Get first byte
	0x45	NOP,rp      	LD_SPAGE,0x80		NOP,rp		   ; If v!=4||len!=5, pop
		<OK|LD_SREG_1>
		<OK|OUTCLASS,CLASS_IP>					   ; Skip TOS, say
									   ; that it's IP 
		<OK>							   ; Skip length
           	<OK>
           	<OK>							   ; Skip ID
           	<OK|LD_SREG_A>
           	<OK|LD_SREG_B>						   ; Skip flags and
           	<OK>							   ; fragment offset
	1     	NOP,rp      	NOP,rp			OK|LD_SREG_C	   ; If TTL <= 1, pop
           	<OK>							   ; Skip protocol
           	<OK>							   ; Skip checksum
           	<OK|LD_SREG_2>						   ; Use relocation
           	<OK|LD_SREG_3>						   ; Skip source
           	<OK|LD_SREG_4>						   ; address 
           	<OK|LD_SREG_5>
;
; Get to start of destination address and call the destination tree.  On return, we've seen the
; entire IP header. Call the input access-list tree, then get the checksum engine's result.  
;
	0xff  	CALL,ipdest 	CALL,ipdest		RD_SREG_0,0xff	   ; recall icb in case
									   ; we have in iacl tree
	0xff	CALL,ipiacl	CALL,ipiacl		RD_CKSUM,0x80
           	<NEXT_DMA>						   ; Wait for checksum
;
; Check the checksum result.  It's good if bit 0x80 is set.  If so, issue the done instruction
; and start the next packet.  Otherwise toss this.

        0x80  	NOP,punt   	DONE,afterreset,0x80 	DONE,afterreset,0x80

;
; IPX headers.  We need to screw around a bit to see which flavor of
; IPX checksumming/hop count is being done and dispatch accordingly.
;

ipxhd:		<RD_BYTE>						   ; Skip checksum
		<RD_BYTE>						   

;
; Come here instead if we read through the checksum in doing protocol classification
;

ipxcksum:	<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
ipxpatch:1	NOP,nextpkt 	RD_SREG_1,ROUTING_NOVELL NOP,nextpkt	   ; See if we route IPX
		<NOP>							   ; on this i/f
	1	NOP,rp		RD_BYTE|LD_SPAGE,0x80	RD_BYTE|LD_SPAGE,0x80; Get first byte
		<RD_BYTE|OUTCLASS,CLASS_IPX>						   
		<RD_BYTE|LD_SREG_6>					   ; save hop count
;		<RD_BYTE>
		<RD_BYTE>						   ; Skip packet type 
; Call the destination tree.  We'll jump to the appropriate hop count routine if successful.
	0xff	CALL,novdest	CALL,novdest	NOP, rp			   

;
; Perform the hop count check.
; If ((hopcount == 0xff) || ((hopcount >= 0x10) && (hopcount <= 0x1f)) then flush the packet
; The novell tree will jump to one of these

novigrphopcnt: 	<RD_SREG_6, 0xff>					   ; get back hop count (and maybe promote this up)
		<NOP>							   ; wait for read to complete

n1:	0x10	NOP,ipx4a	NOP,flushipx	NOP
n2:	0x20	NOP,flushipx	OUTIPXTC,ipxd,0	NOP
n3:	0xff	OUTIPXTC,ipxd,0	NOP,flushipx	NOP,flushipx

ipx4a:	0x0	OUTIPXTC,ipxd,2	OUTIPXTC,ipxd,2	OUTIPXTC,ipxd,3		   ; op 2, hc = 0x20 or
									   ; op 3, 4->8

novhopcnt:	<RD_SREG_6, 0xff>					   ; get back hop count (and maybe promote this up)
		<NOP>							   ; wait for read to complete

a1:	0x10	OUTIPXTC,ipxd,0	NOP,flushipx	NOP			   ; op 0, 4->4
a2:	0x20	NOP,flushipx	OUTIPXTC,ipxd,1	NOP			   ; op 1, 8->4
a3:	0xff	OUTIPXTC,ipxd,1	NOP,flushipx	NOP,flushipx		   ; op 1, 8->4

ipxd:		<NOP,nextpkt,0>						   ; all done, do rewrite, get next packet

flushipx:	<NOP,rp>

;
; CLNP headers.
;
; Since these are hard to find in nature, here's an example packet:
; 81 3C 01 40 BE 00 64 14 AB 14 47 00 05 80 FF EF
; 00 00 00 00 01 6C 07 13 11 08 00 71 92 00 14 47
; 00 05 80 FF EF 00 00 00 00 01 6C 07 13 11 08 00
; 70 26 00 36 80 00 00 00 64 C3 01 C0
;
; SREG 0 == segmentation flag -- bit 0x80 is set if segmentation information is present
; SREG 1 == checksum enable flag -- 0 if checksums disabled, non-zero otherwise
;

clnphd:		<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
clnppatch:1	NOP,nextpkt 	RD_SREG_1,ROUTING_CLNS	NOP,nextpkt	   ; See if we route CLNP
		<NOP>							   ; on this i/f
	  1	NOP,rp		OK			OK		   ; Get NLPID
	  0x81	NOP,rp		OK|OUTCLASS,clnplen,CLASS_CLNP	NOP,rp	   ; Get header len

;
; Come here if we used the NLPID in doing protocol classification.  Currently looking at the NLPID.
;

clnpnlpid:	<ACC_CKSUM>						   ; Checksum the NLPID.
		<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
	  1	NOP,nextpkt 	RD_SREG_1,ROUTING_CLNS	NOP,nextpkt	   ; See if we route CLNP
		<NOP>							   ; on this i/f
	  1	NOP,rp		OK|OUTCLASS,CLASS_CLNP	OK|OUTCLASS,CLASS_CLNP

clnplen		<LD_COUNT0>						   ; Save header len
	  0x6	NOP,rp		NOP,rp			SET_DEC		   ; Start counter
		<DEC_COUNT>						   ; Pop two cycles
									   ; earlier
		<LD_SPAGE,0x80>						   ; Use relocation
		<OK>
	  0x1	NOP,rp		OK		NOP,rp			   ; Version 
	  0x1	NOP,rp		NOP,rp		OK|LD_SREG_0		   ; Lifetime > 1
									   ; Save flags
		<OK>							   ; Skip seg len 1
		<OK>							   ; Skip seg len 2
		<OK|LD_SREG_1>						   ; Cksum 1 
	  0x0	NOP,rp		OK|LD_SREG_1	OK			   ; if 0 check 2
        	<OK|LD_COUNT1>						   ; Save dest length 
	  0x3   NOP,rp		SET_DEC		SET_DEC			   ; Die if too short
          0xff  CALL,clnpdest	CALL,clnpdest	ACC_CKSUM|LD_COUNT1	   ; Call destination tree
									   ; Checksum last byte
	  0x3   NOP,rp		SET_DEC		SET_DEC			   ; Die if too short
		<DEC_COUNT>						   ; End one byte early
		<NOP>							   ; Stall
	  0xff	CALL,spin	CALL,spin	ACC_CKSUM		   ; Skip source
	  0xff  CALL,clnprest	CALL,clnprest	RD_SREG_1,0xff		   ; Do rest of header,
		<NOP>							   ; Stall for RD_CKSUM
	  0x00  NOP,punt	RD_CKSUM,clnpnock RD_CKSUM,0x40		   ; Check checksum
		<NEXT_DMA>
clnpdone: 0x40	NOP,punt	DONE,afterreset,0x80 NOP,punt		   ; Pass the packet

clnpnock:	<NEXT_DMA>
		<DONE,afterreset,0x80>
;
; Remainder of header: the NSEL from the source addr, and then segmentation information and
; global QoS option. 
; QOS:	0xC3 0x01 XX 
;

clnprest:	<RD_SREG_0,0x80>					   ; Get segment bit
		<NOP>
	  0x80  OK,clnpopt	OK		OK			   ; Get segmentation
									   ; or option
		<OK>							   ; Skip segmentation 1
		<OK>							   ; seg 2
		<OK>							   ; seg 3
		<OK>							   ; seg 4
		<OK>							   ; seg 5
		<OK>							   ; Skip seg 6, at options
clnpopt:  0xc3  NOP,rp		OK		NOP,rp			   ; Look for global QOS
	  0x01	NOP,rp		OK		NOP,rp
		<AND_PIPE,0xd7>						   ; Mask off
									   ; congestion bits
		<NOP>							   ; Wait, promotion problem.
	  0xc0  NOP,rp		RD_BYTE		NOP,rp
		<NOP,rp>						   ; Too many bytes

spin:		<OK,spin>						   ; Spin in place till
									   ; counter pops

/*
 * Backpatch table for trees.
 */

inputif:	<NOP,inputiftemp>
inputiftemp	<RD_BYTE,enobridge>
trdestmac:	<NOP,trskip6>
ipdest:		<OK|LD_SREG_6,return>
ipiacl:		<NOP,return>
novdest:	<NOP,return>
srbdest:	<NOP,return>
clnpdest:	<NOP,rp>
xnshd:		<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
xnspatch:1	NOP,nextpkt 	NOP,rp	 		NOP,rp		   ; Backpatch
atalkhd:	<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
atpatch:1	NOP,nextpkt 	NOP,rp	 		NOP,rp		   ; Backpatch
apollohd:	<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
aplpatch:1	NOP,nextpkt 	NOP,rp	 		NOP,rp		   ; Backpatch
vineshd:	<RD_SREG_2,1>
		<NOP>
vinpatch:1	NOP,nextpkt 	NOP,rp	 		NOP,rp		   ; Backpatch
decnethd:	<RD_SREG_2,1>						   ; Recall if it's routable
		<NOP>
dnpatch:1	NOP,nextpkt 	NOP,rp	 		NOP,rp		   ; Backpatch

;
; Backpatch locations for actual bridge caches
;
etb0:		<NOP,tbskip5>
etb1:		<NOP,tbskip5>
etb2:		<NOP,tbskip5>
etb3:		<NOP,tbskip5>
etb4:		<NOP,tbskip5>
etb5:		<NOP,tbskip5>
etb6:		<NOP,tbskip5>
etb7:		<NOP,tbskip5>
etb8:		<NOP,tbskip5>
etb9:		<NOP,tbskip5>
etb10:		<NOP,tbskip5>
ftb0:		<NOP,tbskip5>
ftb1:		<NOP,tbskip5>
ftb2:		<NOP,tbskip5>
ftb3:		<NOP,tbskip5>
ftb4:		<NOP,tbskip5>
ftb5:		<NOP,tbskip5>
ftb6:		<NOP,tbskip5>
ftb7:		<NOP,tbskip5>
ftb8:		<NOP,tbskip5>
ftb9:		<NOP,tbskip5>
ftb10:		<NOP,tbskip5>
;
; Backpatch locations for bridge groups
;
ebridgegrp0:	<NOP,enobridge>
ebridgegrp1:	<NOP,enobridge>
ebridgegrp2:	<NOP,enobridge>
ebridgegrp3:	<NOP,enobridge>
ebridgegrp4:	<NOP,enobridge>
ebridgegrp5:	<NOP,enobridge>
ebridgegrp6:	<NOP,enobridge>
ebridgegrp7:	<NOP,enobridge>
ebridgegrp8:	<NOP,enobridge>
ebridgegrp9:	<NOP,enobridge>
ebridgegrp10:	<NOP,enobridge>
fbridgegrp0:	<NOP,fnobridge>
fbridgegrp1:	<NOP,fnobridge>
fbridgegrp2:	<NOP,fnobridge>
fbridgegrp3:	<NOP,fnobridge>
fbridgegrp4:	<NOP,fnobridge>
fbridgegrp5:	<NOP,fnobridge>
fbridgegrp6:	<NOP,fnobridge>
fbridgegrp7:	<NOP,fnobridge>
fbridgegrp8:	<NOP,fnobridge>
fbridgegrp9:	<NOP,fnobridge>
fbridgegrp10:	<NOP,fnobridge>

/*
Local Variables:
fill-column: 95
comment-column: 75
comment-start: "; "
comment-start-skip: "; "
comment-end: ""
End:
*/

| $Id: asm_cd2430.S,v 3.2.60.1 1996/06/17 08:29:25 jhernand Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm_cd2430.S,v $
|/* ------------------------------------------------------------------
| asm_cd2430.S - Low level code for Cirrus CD2430 interrupt handling
|  
| January 4, 1994, Andy Valencia
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|  ------------------------------------------------------------------
| $Log: asm_cd2430.S,v $
| Revision 3.2.60.1  1996/06/17  08:29:25  jhernand
| Commit Modular Cobra to California_branch
|
| Revision 3.2  1995/11/17  08:39:54  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:40  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:55  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------ */
| $Endlog$
|
	.text

|
| We need an interrupt handler for each of four types and for up to
| four chips.  Thus, we use macros to generate the code.
| table from the array of register tables for all controllers.
|
| Using the controller number, we get the register table and
| read the LICR register (which has an offset of 38 in the register
| table structure) to determine the interrupting channel. Using the
| this information, we index into the cd2430_channel_info array to get
| at the channel instance structure for that channel. Next, we put the
| active interface descriptor for that channel on the stack and call
| the appropriate interrupt service routine, a pointer to which is cached
| in the instance structure. Note that the cd2430_channel_inst_t
| structure is so defined that
|	Active interface descriptor      has an offset of 0
|	Receive ISR pointer              has an offset of 4
|       Modem signal change ISR pointer  has an offset of 8
|       Transmit ISR pointer             has an offset of 12
|	Rxgood ISR pointer               has an offset of 16
| 
| The value of LICR is used to index into an array to get at information
| corresponding to that channel. The index value has to be determined
| by how the LICR is programmed.
|
|         +--+--+--+--+--+--+--+--+
|         |C3|C2|C1|C0|L1|L0|C5|C4|
|         +--+--+--+--+--+--+--+--+
| Here the Cn's refer to the controller number, and the Ln's refer to
| the channel number. Note how the bits for the controller number are
| spread around in the LICR register. We have a limit of 64 controllers
| in the system for this scheme to work. This is not a problem for the
| foreseeable future.

#define RXGOODINT(ctrl) \
cd2430_##ctrl##_rxgoodint: \
	moveml  &0xc0c0,sp@- ; \
	movel   sp,_l2sp ; \
	movel   _l2stk,sp ; \
	addql   &1,_cd2430_istate ; \
	movel   _cd2430_reg_table+(ctrl*4), a0 ; \
	moveb   a0@(38), d0 ; \
	andl	&0xFF, d0; \
	rorb	&2, d0; \
	lslw	&2, d0; \
	lea     _cd2430_channel_info, a0 ; \
	movel   a0@(d0:l), a0 ; \
	movel   a0@, sp@- ; \
	movel   a0@(16), a0 ; \
	jbsr    a0@ ; \
	subql   &1,_cd2430_istate ; \
	movel   _l2sp,sp ; \
	moveml  sp@+,&0x0303 ; \
	addql   &1,_l2cnt ; \
	rte

#define RXINT(ctrl) \
cd2430_##ctrl##_rxint: \
	moveml  &0xc0c0,sp@- ; \
	movel   sp,_l2sp ; \
	movel   _l2stk,sp ; \
	addql   &1,_cd2430_istate ; \
	movel   _cd2430_reg_table+(ctrl*4), a0 ; \
	moveb   a0@(38), d0 ; \
	rorb	&2, d0; \
	lslw	&2, d0; \
	andl    &0x03FF, d0 ; \
	lea     _cd2430_channel_info, a0 ; \
	movel   a0@(d0:l), a0 ; \
	movel   a0@, sp@- ; \
	movel   a0@(4), a0 ; \
	jbsr    a0@ ; \
	subql   &1,_cd2430_istate ; \
	movel   _l2sp,sp ; \
	moveml  sp@+,&0x0303 ; \
	addql   &1,_l2cnt ; \
	rte

#define TXINT(ctrl) \
cd2430_##ctrl##_txint: \
	moveml  &0xc0c0,sp@- ; \
	movel   sp,_l1sp ; \
	movel   _l1stk,sp ; \
	addql   &1,_cd2430_istate ; \
	movel	_cd2430_reg_table+(ctrl*4), a0 ; \
	moveb	a0@(38), d0 ; \
	rorb	&2, d0; \
	lslw	&2, d0; \
	andl	&0x03FF, d0 ; \
	lea	_cd2430_channel_info, a0 ; \
	movel	a0@(d0:l), a0 ; \
	movel	a0@, sp@- ; \
	movel	a0@(12), a0 ; \
	jbsr	a0@ ; \
	subql   &1,_cd2430_istate ; \
	movel   _l1sp,sp ; \
	moveml  sp@+,&0x0303 ; \
	addql   &1,_l1cnt ; \
	rte

#define MODINT(ctrl) \
cd2430_##ctrl##_modint: \
	moveml  &0xc0c0,sp@- ; \
	movel   sp,_l3sp ; \
	movel   _l3stk,sp ; \
	addql   &1,_cd2430_istate ; \
	movel   _cd2430_reg_table+(ctrl*4), a0 ; \
	moveb   a0@(38), d0 ; \
	rorb	&2, d0; \
	lslw	&2, d0; \
	andl    &0x03FF, d0 ; \
	lea     _cd2430_channel_info, a0 ; \
	movel   a0@(d0:l), a0 ; \
	movel   a0@, sp@- ; \
	movel   a0@(8), a0 ; \
	jbsr    a0@ ; \
	subql   &1,_cd2430_istate ; \
	movel   _l3sp,sp ; \
	moveml  sp@+,&0x0303 ; \
	addql   &1,_l3cnt ; \
	rte
|
| Generate the actual code for the handlers
|
    RXGOODINT(0)
    RXINT(0)
    TXINT(0)
    MODINT(0)

    RXGOODINT(1)
    RXINT(1)
    TXINT(1)
    MODINT(1)

    RXGOODINT(2)
    RXINT(2)
    TXINT(2)
    MODINT(2)

    RXGOODINT(3)
    RXINT(3)
    TXINT(3)
    MODINT(3)

|
| Fill in pointers to the actual routines
|
	.data
	.globl	_cd2430_vectors
_cd2430_vectors:
	.long	cd2430_0_rxint
	.long	cd2430_0_modint
	.long	cd2430_0_txint
	.long	cd2430_0_rxgoodint
	.long	cd2430_1_rxint
	.long	cd2430_1_modint
	.long	cd2430_1_txint
	.long	cd2430_1_rxgoodint
	.long	cd2430_2_rxint
	.long	cd2430_2_modint
	.long	cd2430_2_txint
	.long	cd2430_2_rxgoodint
	.long	cd2430_3_rxint
	.long	cd2430_3_modint
	.long	cd2430_3_txint
	.long	cd2430_3_rxgoodint

| $Id: asm_as5200_cd2430.S,v 3.1.18.1 1996/06/16 21:10:57 vnguyen Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm_as5200_cd2430.S,v $
|------------------------------------------------------------------
| asm_as5200_cd2430.S AS5200 Async Interrupt Handler Wrappers.
|
| Feb 96, Eddy Lem
|
| Copyright (c) 1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm_as5200_cd2430.S,v $
| Revision 3.1.18.1  1996/06/16  21:10:57  vnguyen
| Finally committing Brasil to California_branch
|
| Revision 3.1  1996/02/20  20:41:01  elem
| Place holder for new development.
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_as5200_cd2430.h"

|
| Interrupt Handler must
|	- first verify the level of interrupt matches the handler. 
|         Because under stress, the hardware may generate the wrong
|	  vector, thus vectoring to the wrong service routine.  So
|	  currently the only viable fix was thru software.
|	- read the motherboard MB_ISRs first to determine which
|	  slot(s) need service
|	- read the modem boards MDBIAVRx to get the real interrupt vector
|

|
| Macro Expansions
|

| YYYY - patch to reject 0xff from IAVR registers

#define RXEXCEPSLOT(n) \
	btst	&BRASIL_RXEXCEP_SLOT##n##_BIT,d2 ; \
	beq	cd2430_rxexcep_slot##n##_complete ; \
	movew	ADRSPC_BRASIL_SLOT##n##_MDB_IAVRR,d0 ; \
	andl	&0x00fc,d0 ; \
	addl	_cd2430_controller_offset+(n*4),d0 ; \
	lea	_cd2430_ttyptr,a2 ; \
	addal	d0,a2 ; \
	movel	(a2),sp@-  ; \
	jsr	_cd2430_rxexcep  ; \
	addql	&4,sp

#define TXINTSLOT(n) \
	btst	&BRASIL_TXINT_SLOT##n##_BIT,d2 ; \
	beq	cd2430_txint_slot##n##_complete ; \
	movew	ADRSPC_BRASIL_SLOT##n##_MDB_IAVRT,d0 ; \
	andl	&0x00fc,d0 ; \
	addl	_cd2430_controller_offset+(n*4),d0 ; \
	lea	_cd2430_ttyptr,a2 ; \
	addal	d0,a2 ; \
	movel	(a2),sp@-  ; \
	jsr	_cd2430_txint  ; \
	addql	&4,sp

#define MODINTSLOT(n) \
	btst	&BRASIL_MODINT_SLOT##n##_BIT,d2 ; \
	beq	cd2430_modint_slot##n##_complete ; \
	movew	ADRSPC_BRASIL_SLOT##n##_MDB_IAVRM,d0 ; \
	andl	&0x00fc,d0 ; \
	addl	_cd2430_controller_offset+(n*4),d0 ; \
	lea	_cd2430_ttyptr,a2 ; \
	addal	d0,a2 ; \
	movel	(a2),sp@-  ; \
	jsr	_cd2430_modint  ; \
	addql	&4,sp

|
| Interrupts Handlers
|
cd2430_rxgood:				| should never get this interrupt
	rte

cd2430_rxexcep:
	moveml	#0xe0e0,sp@- 		| save registers
	movw	sr,d0			| check if it is really a
	andw	#0x0700,d0		| level 2 interrupt
cd2430_rxexcep_check_level:
	cmpw	#0x0200,d0
	bne	cd2430_txint_check_level

	movel	sp,_l2sp 		| save current stack pointer
	movel	_l2stk,sp 		| use level 2 intr stack
	addql	#1,_cd2430_istate 	| increment cd2430 active intr count
cd2430_rxexcep_slot2_complete:
	movw	ADRSPC_BRASIL_ISR2_REG,d2
	notl	d2
	andl	_cd2430_rx_mask,d2
	beq	cd2430_rxexcep_is_done
	RXEXCEPSLOT(0)
cd2430_rxexcep_slot0_complete:
	RXEXCEPSLOT(1)
cd2430_rxexcep_slot1_complete:
	RXEXCEPSLOT(2)
	bra	cd2430_rxexcep_slot2_complete
cd2430_rxexcep_is_done:
	subql	#1,_cd2430_istate 	| decrement cd2430 active intr count
	movel	_l2sp,sp 		| restore original stack
	moveml	sp@+,&0x0707 		| restore registers
	addql	#1,_l2cnt 		| increment level 2 intr count

	rte

cd2430_txint: 
	moveml	#0xe0e0,sp@- 
cd2430_txint_check_level: 		| level 1 is at bottom, no need to chk
	movel	sp,_l1sp 
	movel	_l1stk,sp 
	addql	#1,_cd2430_istate 
cd2430_txint_slot2_complete:
	movw    ADRSPC_BRASIL_ISR1_REG,d2
	notl	d2
	andl    _cd2430_tx_mask,d2
	beq     cd2430_txint_is_done
	TXINTSLOT(0)
cd2430_txint_slot0_complete:
	TXINTSLOT(1)
cd2430_txint_slot1_complete:
	TXINTSLOT(2)
	bra     cd2430_txint_slot2_complete
cd2430_txint_is_done:
	subql	#1,_cd2430_istate 
	movel	_l1sp,sp 
	moveml	sp@+,&0x0707 
	addql	#1,_l1cnt 
	rte

cd2430_modint: 
	moveml	#0xe0e0,sp@- 
	movw	sr,d0			| check if it is really a
	andw	#0x0700,d0		| level 3 interrupt
	cmpw	#0x0300,d0
	bne	cd2430_rxexcep_check_level

	movel	sp,_l3sp 
	movel	_l3stk,sp 
	addql	#1,_cd2430_istate 
cd2430_modint_slot2_complete:
	movw    ADRSPC_BRASIL_ISR2_REG,d2
	notl	d2
	andl    _cd2430_mod_mask,d2
	beq     cd2430_modint_is_done
	MODINTSLOT(0)
cd2430_modint_slot0_complete:
	MODINTSLOT(1)
cd2430_modint_slot1_complete:
	MODINTSLOT(2)
	bra     cd2430_modint_slot2_complete
cd2430_modint_is_done:
	subql	#1,_cd2430_istate 
	movel	_l3sp,sp 
	moveml	sp@+,&0x0707 
	addql	#1,_l3cnt 
	rte

|
| Fill in pointers to the actual routines
|
	.data
_as5200_cd2430_vectors:
	.globl	_as5200_cd2430_vectors
_as5200_cd2430_vectors:
	.long	cd2430_rxgood
	.long	cd2430_txint
	.long	cd2430_rxexcep
	.long	cd2430_modint


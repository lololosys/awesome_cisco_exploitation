| $Id: asm_c1000_serial.S,v 3.2 1995/11/17 18:39:45 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/src-36-c1000/asm_c1000_serial.S,v $
|------------------------------------------------------------------
| asm_c1000_serial.S  Catwoman serial driver assembly code
|
| May 1995, John Ng
|
| Copyright (c) 1995 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm_c1000_serial.S,v $
| Revision 3.2  1995/11/17  18:39:45  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:19:30  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.2  1995/11/08  21:26:32  shaker
| Merge Arkansas_branch into 11.1 mainline.
|
| Revision 2.1  1995/06/07  22:52:13  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|



#define ASMINCLUDE

#include "../src-36-c1000/pcmap.h"
#include "../src-36/quicc.h"
#include "../src-36/quicc_asm.h"
#include "../src-36-c1000/init_c1005.h"

|
|
| typedef struct quicc_cpic_interrupt {
|    hwidbtype *idb;		/* hwidb for the interface */
|    void (*func)(hwidbtype *);	/* the specific function handler */
|    ulong 	mask;		/* mask for this specific CPIC interrupt */
|    volatile ulong *cisr;	/* the cisr register */
| } quicc_cpic_interrupt_t;
|
|
|	This is a general assembly routine which will clear the
|	masks off the interrupt handler.  This routine expects
|	register a2 to be set up with the address of the 
|	quicc_cpic_intr_array.
|
| void	quicc_gen_asm_cpic_handler(void)
|
	.global _quicc_gen_asm_cpic_handler
_quicc_gen_asm_cpic_handler:
	
	movl	a2@+, sp@-		| push the idb on the stack
	movl	a2@+, a3		| move function pointer to a3
	jbsr	a3@			| Call function 
	
	movl	a2@+, d2		| Move mask to a2
	movl	a2@, a3			| Move the cisr pointer to a3
	movl	d2, a3@			| Clear the CISR bit 

	addql	#1, _l4cnt		| Count interrupt
	movl	_l4sp, sp		| Restore old stack
	moveml  sp@+, d0-d2/a0-a4	| Restore old registers
	rte

|
|	This routine will be called when a DSR signal change occurs.
|	It will check to determine the state of the line and then
|	call a general assembly routine, quicc_gen_asm_cpic_handler
|	which will handle the calling of the C interrupt routine for 
|	this interrupt, the scop of this general assembly routine
|	should local to this file only.  No C function should call it.
|	The clearing of the CISR and returning from interrupt will 
|	be done by quicc_gen_asm_cpic_handler.
|
| void quicc_serial_dsr_interrupt(void)

	.global _quicc_serial_dsr_interrupt
_quicc_serial_dsr_interrupt:
	moveml	d0-d2/a0-a4,sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer 

	lea	_quicc_cpic_intr_array, a2 | Get pointer to interrupt array
	addl	#DSR_INTR_OFFSET, a2	| Skip to DSR offset
	jmp	_quicc_gen_asm_cpic_handler

|
|	This routine will be called when a DCD signal change occurs.
|	It will check to determine the state of the line and then
|	call a general assembly routine, quicc_gen_asm_cpic_handler
|	which will handle the calling of the C interrupt routine for 
|	this interrupt, the scop of this general assembly routine
|	should local to this file only.  No C function should call it.
|	The clearing of the CISR and returning from interrupt will 
|	be done by quicc_gen_asm_cpic_handler.
|
| void quicc_serial_dcd_interrupt(void)

	.global _quicc_serial_dcd_interrupt
_quicc_serial_dcd_interrupt:
	moveml	d0-d2/a0-a4,sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer 

	lea	_quicc_cpic_intr_array, a2 | Get pointer to interrupt array
	addl	#DCD_INTR_OFFSET, a2	| Skip to DCD offset
	jmp	_quicc_gen_asm_cpic_handler


#undef ASMINCLUDE

/* $Id: tx.s,v 1.1.4.4 1996/07/02 23:45:14 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/tx.s,v $
 *-----------------------------------------------------------------
 * tx.s -- Transmit sar operation
 *
 # October 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tx.s,v $
 * Revision 1.1.4.4  1996/07/02  23:45:14  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 1.1.4.3  1996/06/18  16:46:21  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/29  18:02:26  sakumar
 * CSCdi58940:  SAR Builds generates assembler warnings
 * Branch: California_branch
 *
 * Added _start symbol to fix warning message from assembler.
 *
 * Revision 1.1.4.1  1996/05/09  15:01:39  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.3  1996/04/03  21:18:29  sakumar
 * LE-ARP indication from TSAR sends SRC address (as well as DEST address).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.2  1996/03/30  02:17:51  sakumar
 * Changes to support sending of FLUSH message on Cat5K ATM LANE module.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:13:46  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  03:09:58  sakumar
 * Apply ATM line card 2.1 fixes to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
| Revision 1.1  1996/03/11  07:44:40  rlowe
| Placeholders for California.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef COMMENTED_OUT
#define USE_M_GPRR 
#define TIMING_LED
#endif /* COMMENTED_OUT */

/* Use Global Pacing Register to insert idle cells     */
/* in a continuous stream of assigned cells to prevent */
/* NTC from throwing away cells due to flow control bug*/

 #------------------------------------------------------------------
 #
 #  FILE:  tx_sar.s
 #
 #  CONTENTS:
 #
 # 		This file contains the TX sar operation code.  
 #
 #		Assembler Coding Conventions:
 #
 # 			<>_REG for Processor Registers (s0-s7)
 # 			<>_AREG for ATMIZER Registers
 # 			<>_s for structure fields
 # 			<>_C for defined constants
 # 			<>_L for program labels
 #
 #		Assumptions:
 #		^^^^^^^^^^^
 #			- All sar registers have been initialized
 #			- VCR has been setup
 #			- 
 #		Algorithms:
 #			
 #			TX sar services data from the following sources:
 #				- The biga recv q
 #				- rx_sar to tx_sar q
 #				- acp data q
 #				- acp command q
 #
 #			The presence of work is indicated by polling a register.  If the
 #			register indicates presence of work then the above mentioned
 #			queues are polled in the following order:
 #
 #					- biga recv q
 #					- rx_sar to tx_sar q
 #					- acp data q
 #					- acp command q
 #
 #			The TX sar segments and transmits a packet exhaustively before 
 #			operating on the next request.  This implies that there is 
 #			only one active VC during packet trasnmission
 #			
 #			The TX sar attempts to operate at full rate, i.e., no pacing
 #			or traffic shaping is done.
 #			
 #			
 #	TX_SCHEDULAR:
 #
 #	loop:
 #			while(1) 
 #				if(biga_work()) 
 #					service_biga_q();
 #				
 #				if(Check_Work()) 
 #					if(rxsar_work()) 					exhaustive service
 #						service_rxsar_q();
 #						continue;
 #					
 #					if(command_work()) 
 #						service_command_q();		// exhaustive service
 #						if(acp_work()) 
 #							service_acp_q();			
 #						
 #						continue;
 #					
 #				
 #
 #		while
 #
 #
 #	SERVICE ROUTINES:
 #
 #	service_biga_q:
 #
 #		Service strategy:  One packet per per invocation
 #
 #		The biga recv desc has:
 #			<buffer pointer, buffer length, actual recv len, index>
 #
 #		The information needed before the packet can be processed is:
 #			dest mac address
 #			index
 #
 #	NOTE:  Make big_rd contiguous with data and DMA in biga_rd + sy_hdr+
 #			6 byte DA to location in VCR
 #			
 #			parse color from synergy header
 #			parse dest mac addrees from synergy header
 #			schedule cam lookup using color and mac address
 #			read index from biga rd
 #			read buffer_pointer
 #			read buffer_length
 #			use index to determine vc state from the vcstate table in vcr
 #
 #			if(vcstate == established) 
 #				cunstruct vcr map entry in vcr
 #    				load dma register = buffer_pointer
 #    				tlen;               total lenght of data 
 #    				slen;               ammount transmitted 
 #    				cell_hdr;           gfc:4, vpi:8, vci:16, pt:3, clp:1
 #				goto tx_segmentation_process;
 #			else 
 #				wait for cam lookup to complete
 #				le_arp or hold_q
 #			
 #
 #									
 #  OWNER:
 #
 #      Ratinder P. S. Ahuja
 #
 #	LOG:
 #		Created  3 Oct 1994
 #
 #


#ifndef LANGUAGE_ASSEMBLY
#define  LANGUAGE_ASSEMBLY
#endif

#include  "txsar.h"



    .globl _start
    .ent _start
 
_start:
#ifdef NO_PMON


###############################################################################
##
##	For operation directly on the SAR, the following initialization steps
##	are taken.
##
###############################################################################

 
#ifdef LR33300
#if 0 /* use IS0 */
CACHECFG = (BIU_IS1|BIU_IS0|BIU_IBLKSZ8|BIU_DS1|BIU_DS0|BIU_DBLKSZ8)
#else
CACHECFG = (BIU_IS1|BIU_IBLKSZ8|BIU_DS1|BIU_DS0|BIU_DBLKSZ8)
#endif
#endif
 
    .text
 
/*-------------------------------------------------------------------*
 * reset_execption()
 *-------------------------------------------------------------------*/
reset_exception_L:
        /* ATMizer must start with a full 32-bit jump */
    la  k0,main
    j   k0
 
    .align  8
/*-------------------------------------------------------------------*
 * utlb_miss_execption()
 *-------------------------------------------------------------------*/
utlb_miss_exception_L:
    la  k0,_exception_L
    li  k1,K1BASE
    or  k0,k1
    j   k0
 
    .align 7
/*-------------------------------------------------------------------*
 * general_exception()
 *-------------------------------------------------------------------*/
general_exception_L:
    la  k0,_exception_L
    li  k1,K1BASE
    or  k0,k1
    j   k0
 
    .align  9


	.end _start
/*-------------------------------------------------------------------*
 *  _exception:
 *-------------------------------------------------------------------*/
    .globl  _exception_L
    .ent _exception_L
_exception_L:
 
        li      t0,0xFF
        li      t1,0xA0E80040
        .set noreorder
        mfc0    k0,C0_CAUSE
        nop
        .set reorder
        and     k1,k0,CAUSE_EXCMASK
        srl     k1,0x2
#if  0  /* analyser trigger */
        sw      k1,0x10(t1)       /* output to 0x280050 to trigger analyzer */
#endif  /* analyser trigger */
        and     k0,k1,0x1
        bne     k0,zero,1f
        sw      t0,0xc(t1)
        b       e1
1:      sw      zero,0xc(t1)
e1:     and     k0,k1,0x2
        bne     k0,zero,2f
        sw      t0,0x8(t1)
        b       e2
2:      sw      zero,0x8(t1)
e2:     and     k0,k1,0x4
        bne     k0,zero,3f
        sw      t0,0x4(t1)
        b       e3
3:      sw      zero,0x4(t1)
e3:     and     k0,k1,0x8
        bne     k0,zero,4f
        sw      t0,0x0(t1)
        b       doom_L
4:      sw      zero,0x0(t1)

doom_L:
        b       doom_L
 
    .end _exception_L

#endif	/*NO_PMON*/

#ifndef NO_PMON
    .end _start
#endif /* NO NO_PMON */


###############################################################################
##	main()
##	{
##	    txsar_init();
##	    txsar_work();                       /* starts the tx sar scheduler  */
##	}
##	 
##	void
##	txsar_init()
##	{
##	 
##	    init_apu();
##	    init_vcr();
##	    init_pru();
##	    init_iram();
##	    init_aci();
##	}
###############################################################################


    .globl main
    .ent main
main:

#ifdef NO_PMON			
    # force kSeg1 in case control is passed here from Kseg0
    la  t0,1f
    or  t0,K1BASE
    j   t0
1:
 
    # set SR and CAUSE to something sensible
    li  v0,SR_BEV
    .set noreorder
    .set noat
    mtc0    v0,C0_SR
    mtc0    zero,C0_CAUSE
    .set at
    .set reorder
 
BIUCFG = (BIU_NOSTR|BIU_BGNT|BIU_NOPAD|BIU_INTP|BIU_IS1|BIU_DS|BIU_DBLKSZ4)
    li  t0,BIUCFG
    sw      t0,M_BIU
SCRCFG = (SCR_RO52|SCR_TO52|SCR_HH0|SCR_HH1|SCR_HH2|SCR_CBS4|SCR_BS32)
    li      t0,SCRCFG
    sw      t0,M_SCR
 
 
    /********************************************
     *  all processor specific setup has been done
     *********************************************/
setUpDone_L:
        /* dummy read to clear intr on ATM line card */
        li      a0,0xA0E80060
        lw      t0,0x0(a0)
 
        /* turn off diag led(code entry ) */
    li  t0,0xff
    li  a0,IOBASE
    sw  t0,LED1_OFFSET(a0)
    sw  t0,LED2_OFFSET(a0)
    sw  t0,LED3_OFFSET(a0)
    sw  t0,LED4_OFFSET(a0)
 
        /* visual delay for LEDs */
        li      t0,0x30000
1:      subu    t0,1
        bne     zero,t0,1b

#endif	/* NO_PMON*/

init_apu_L:
    ##
    ##  System control register effective address 0xfff04a00:
    ##      - set receive offset for 52 byte cells
    ##      - set transmit offset for 52 byte cells
    ##      - set utopia mode
    ##      - Set hec mode:
    ##          generate and insert hec on transmit
    ##          expect hec on recv
    ##          check hec errors on recv
    ##      - set cache block size to control burst sizes
    ##      - set recv cell holder size
	##
    ##	scr_reg = 0;
    ##	scr_reg |= ( 3 << SCR_ROSHFT);          receive offset = 3 
    ##	scr_reg |= ( 3 << SCR_TOSHFT);          transmit offset = 3
    ##	scr_reg |= SCR_HH0;                     generate hec on tx 
    ##	scr_reg |= SCR_HH1;                     expect hec on recv 
    ##	scr_reg |= SCR_HH2;                     check hec on recv 
    ##	scr_reg |= SCR_CBS4;                    cache block size = 4 
    ##	scr_reg |= SCR_BS4;                     recv cell holder = 4 cells
    ##	*((UINT32 *)M_SCR) = scr_reg;
	##

	li	t0,	0
	or	t0, ((3 << SCR_ROSHFT) | (3 << SCR_TOSHFT))
	or	t0,	(SCR_HH0 | SCR_HH1 | SCR_HH2 | SCR_CBS4 | SCR_BS4)
	
	la	t1,	M_SCR
	sw	t0,	(t1)

    ##
    ##  APU core registers:
    ##      BIU cache/configuration register:
    ##          - enable instruction streaming
    ##          - enable load scheduling
    ##
    ##      Status Register:
    ##          - enable co-processors
    ##          - disable interrupts
    ##
    ##      MSB substitution register:
    ##          - set 10 MS bits
    ##
    ##	biu_reg = 0;
    ##	*((UINT32 *)M_BIU) = biu_reg;

	la	t1,	M_BIU
	sw	zero,	(t1)

    ##
    ##
    ##  Set up status register
    ##      - enable co-processor usability
	##
    ##	C0_SR is $12 (Processor Status Register)
    ##	mtc0(C0_SR,mfc0(C0_SR)|SR_CU0|SR_CU1|SR_CU2|SR_CU3);
	##
 
	mfc0    t0, C0_SR
	or		t0,	(SR_CU0|SR_CU1|SR_CU2|SR_CU3)
	mtc0	t0,	C0_SR


    ##
    ##  Set MSB register
    ##	*((UINT32 *)M_ASUBR) = 0x282;
    ##
 
    la      t0, M_ASUBR
    li      t1, ASUBR_VALUE_C
    sw      t1, (t0)

    # Set the PRU Configuration Register to 0 => Timers use CLK and not
    # PRU_CLK (Figure 14.8, Pg 14-22)
    # Timer 9 (Used for Transmit Throttling), is unstalled below
    la		t0, M_CONFIG
    sw		zero, (t0)

    # Setup Count Initialization Register for Counter number 9
    li  t0, 5                           # init value of counter (line rate)
    sw  t0, CIR_REG9_AREG               # Write value to CIR
 
    # Unstall timer 9
    li  t0, ~(1<<9)
    sw  t0, STALL_AREG

init_vcr_L:

    ##
    ##  - Clear VCR
    ##
 
    la      t0, VCR_ADDR_C
    addu    t1, t0, VCRSIZE_C
1:  sw      zero, (t0)
    addu    t0, 4
    bne     t0, t1, 1b

#ifdef USE_M_GPRR
	## Set up Global Pacing Rate Register (send idle after 33 cont data cells)
	la t1, M_GPRR+0x80 /* Bit 7 enables the GPRR */
	li t2, 33
	sw t2, (t1)
#endif /* USE_M_GPRR */
 
 
init_pru_L:
 
    ##
    ##  Stall the PRU
	##	*((UINT32 *)M_STALL) = 0x01ff;
    ##	la      t0, M_STALL
	##
	li		t1,	0x01ff
    sw      t1, (t0)
 

#define IRAM
#ifdef IRAM

init_iram_L:
 
    ##
    ##  Move code to iram
    ##
    # Copy SAR code into IRAM
    bal     iramSize_L      # will return number of words to be copied in v0
 
    la      t0, M_BIU       # Setup BIU (temporarily -- for IRAM copy)
    li      t1, (BIU_NOSTR | BIU_LDSCH | BIU_NOPAD | BIU_IS1)
    sw      t1, (t0)
    mfc0    t0, C0_SR       # Get current Processor Status
    or      t1, t0, SR_ISC  # Invalidate cache bit
 
    la      t3, txsar_work    # SOURCE Addr
    move    t4, zero            # DESTINATION Addr (IRAM starts at 0)
 
    .set    noreorder
    .set    noat
 
iram_copy_loop_L:
    lw      t5, (t3)        # Fetch word from SP
    addi    t3, 4           # increment source pointer
    mtc0    t1, C0_SR       # enable isolate cache
    sub     v0, 1           # decrement word counter by 1
    sw      t5, (t4)        # store instruction into IRAM
    mtc0    t0, C0_SR       # disable isolate cache
    bne     v0, zero, iram_copy_loop_L
    addi    t4, 4           # increment destination pointer (DELAY slot)
 
    nop
    nop
    nop
 
    .set    reorder
    .set    at

#endif /* IRAM */
 
init_aci_L:

#ifdef TIMING_LED
	li	gp, IOBASE
#endif /* TIMING_LED */

    ##
    ##  Status register:
    ##      - set transmitter initialize bit in system register.
    ##	Enable the Transmitter 
    ##	scr_reg = * ((UINT32 *) M_SCR );
	##	 
    ##	scr_reg |= (SCR_TI);                Transmitter initialize
	##	 
	##	 * ((UINT32 *) M_SCR) = scr_reg;
 	##

	la	t1,	M_SCR
	lw	t0,	(t1)
	or	t0,	SCR_TI
	sw	t0,	(t1)
	
	bal		TX_INIT_L
    sw      zero, HOSTINT_AREG		# Notify Host

    # Setup BIU Cache Configuration Register
    #   - No Streaming
    #   - Enable Load Scheduling
    #   - No Wait State
    #   - Enable I-Cache Set 1 (ONLY IF RUNNING FROM IRAM)
    la      t0, M_BIU
    li      t1, (BIU_NOSTR | BIU_LDSCH | BIU_NOPAD)


#ifdef IRAM
    or      t1,  BIU_IS1
    sw      t1, (t0)
    move    t0,  zero           # Go to 0 which is start of code in IRAM
#else /* NOT IRAM */
    sw      t1, (t0)
    la      t0, (txsar_work) 	# enter main loop		
#endif /* NOT IRAM */
    j       t0
    nop
    nop


	.end main
###############################################################################



###############################################################################
##
##	INITIALIZATION
##		Called from C 
##		Load Sx registers as:
##			VCR_REG             s0      pointer to start of VCR 
##			BHEADREG_REG        s1      address of biga head register
##			pBTAIL_REG          s2      value of tail pointer   
##			pRXTOTXQ_REG        s3      rx to tx sar queue descriptor  
##			pACPTXQ_REG         s4      ACP to tx sar data queue des
##			pACPCMDQ_REG        s5      ACP command queue desc    
##
###############################################################################

		.text
		.globl	TX_INIT_L
		.ent	TX_INIT_L
TX_INIT_L:

		li	VCR_REG,		VCR_ADDR_C				# Load vcr address
		li	BIGA_BASE_REG,	BIGA_BASE_C				# biga head register 
													# initial biga head value
		li  t0,				TX_SAR_SPRAM_START_C		
													# initial hd/tail value
		lw	pBHEAD_REG,	BHEADOFF_C(BIGA_BASE_REG)	# current softhead
		lw	pBHEAD_REG,		txspram_softrhead_s(t0) # softhead

		# Store next pointer in VCR (used by BIGAQ Service)
		lw	t1, (pBHEAD_REG)
		sw	t1, bigard_pNext_s+vcr_bigard_s(VCR_REG)

		lw	pBTAIL_REG,		txspram_softrtail_s(t0)	# softtail
		lw	t1,				txspram_bigastart_s(t0)	# copy biga start to vcr
		sw	t1,				vcr_bigastart_s(VCR_REG)
		lw	t1,				txspram_bigaend_s(t0)	# copy biga end to vcr
		sw	t1,				vcr_bigaend_s(VCR_REG)
		lw	t1,				txspram_pStats_s(t0)	# Copy pointer to tx stats
		sw	t1,				vcr_txspram_pStats_s(VCR_REG)
		sw	zero,			vcr_txgl_packet_output_s(VCR_REG)
		sw	zero,			vcr_txgl_cell_output_s(VCR_REG)

		#Init values of regs that convert from VPC to VPI/VCI
		li ACTIVE_VCI_MASK_REG, (1<<VCI_BITS_C)-1
		li NUM_ACTIVE_VCI_BITS_REG, VCI_BITS_C

    	#	The biga rd is DMA'd to the vcr in a fixed location.  Construct
    	# 	effective addr 
    	#
												# byte count, read bit
    	li      t1, (bigard_dmasz_s<<DMA_BCSHFT)|M_DMACR|DMA_RD 
		la		t2,	vcr_bigard_s(VCR_REG)		# vcr location
    	and     t3, t2, 0xfff                   # Only lower 12 bits are valid
    	or      t1, t3                          # t1 now has effective addr
		sw		t1, vcr_bigard_ea_s(VCR_REG)	# store effective address

		lw		t1, txspram_pLecTable_s(t0)		# base of Lec table
		sw		t1, vcr_plectable_s(VCR_REG)	# address of base of Lec table

		lw	pACPCMDQ_REG,	txspram_pAcpCmdq_s(t0)
		la	VCRTXCELL_REG,	vcr_txbuf_s(VCR_REG)	# first cell build area
		la	ENDTXCELL_REG,	vcr_etxbuf_s(VCR_REG)	# end of tx cell area
													# in vcr
		# Copy SAR-to-SAR queue pointers to VCR
		li  t0,	SAR_SHRAM_START_C		
		lw  t1, shram_pRxToTxQ_s(t0)
    	sw  t1, vcr_pRxToTxQ_s(VCR_REG)
    	lw  t1, shram_pTxToRxQ_s(t0)
    	sw  t1, vcr_pTxToRxQ_s(VCR_REG)


		#
		#	Transmit idle cell
		#
		addu	t8, 	VCR_REG, vcr_idle_s
		sw		t8,		TXFIFO_AREG

		j		ra

		.end TX_INIT_L


###############################################################################


#
###############################################################################
#
#   FUNCTION:
#           txsar_work
#
#   PARAMETERS:
#           NONE
#
#   RETURN:
#           NONE
#
#   ALGORITHM:
#			Called from main
#			set up register usage
#   TX_SCHEDULAR_L:                             schdular loop
#           IF biga work
#               branch to biga service routine
#
#           IF co-processor condition true
#               IF rx_sar work
#                   brach to rx_sar service routine
#               Else IF acp_work
#                   branch to acp data service routine
#               Else IF acp_command
#                   branch to acp command service routine
#           branch to TX_SCHEDULAR_L
#
#
#
###############################################################################
#

		.text
		.globl	txsar_work
		.ent	txsar_work

txsar_work:

		#
		#	Set up registers
		#

TX_SCHEDULER_L:

		#
		#  Compare biga recv_head against the previous (softhead) value
		#	

1:
		lw	pTEMP_BHEAD_REG,BHEADOFF_C(BIGA_BASE_REG)		# check if head moved
 		bne	pBHEAD_REG, pTEMP_BHEAD_REG,BIGAQ_SERVICE_L		# if not eq, goto biga service

TX_SCHEDULER_CP_L:

		# Else
		#  Check co-processor 0 condition to see if messaging interrupt
		#	was generated.  If true, poll the RXSAR_TO_TXSAR queue, the
		#	ACP_TO_TXSAR queue and the ACP command q for work.  Branch to
		#	corresponding service routine
		#

		bc0f	TX_SCHEDULER_L			# if false, go back to TX_SCHEDULAR_L
										# else start polling queues	

		# Acknowledge the interrupt
    	# Reading Register 3 clears the contents (i.e. interrupt ack)
    	la  t1, SAR_INTR_REG_ADDR_C 	# Address of Register 3
    	lw  t0, (t1)


		# find out who interrupted the RSAR
    	and t1, t0, INTR_OTHER_SAR_BIT_C
    	beq t1, zero, Is_it_ACP_interrupt_L
        	# Process interrupt from RXSAR (BUS pkt transmission)
        	# DO NOT USE t0 in this code since the interrupt status must be
        	# preserved

BUS_loop_L:
			lw	a0, vcr_pRxToTxQ_s(VCR_REG)
			#get pointer to buffer DEQ returns pointer in v0
			bal     DEQ_L         # invoke deq_l, return addr in a2

			beq v0, zero, Is_it_ACP_interrupt_L # loop EXIT condition
				la  a0, vcr_cmd_s(VCR_REG)      # cmd record in vcr

				# Make command record in VCR
				lhu	t1, bufh_vpc_16s(v0)
    			sw  t1, txsend_vpc_s(a0)

				la	t1, bufh_payload_s(v0)
    			sw  t1, txsend_pbuf_s(a0)

				lhu	t1, bufh_length_16s(v0)
    			sw  t1, txsend_len_s(a0)

				sw  v0, vcr_pRSARsdatabuffer_s(VCR_REG)

    			bal ACP_DATAQ_SERVICE_L

				lw	a0, vcr_pTxToRxQ_s(VCR_REG)
				lw  a1, vcr_pRSARsdatabuffer_s(VCR_REG)
    			bal ENQ_L          # invoke enq_l, return addr in a2

				# OK so now interrupt the RSAR so it can get its buffer back
				li	a0, SAR_INTR_REG_ADDR_C
				li	a1, INTR_OTHER_SAR_BIT_C
				sw  a1, (a0)

                b BUS_loop_L		# Service this queue exhaustively


										
Is_it_ACP_interrupt_L:
		and t1, t0, INTR_BY_ACP_BIT_C
		beq t1, zero, TX_SCHEDULER_L

		#
		# check event respq from acp
		#

		bal	EVENT_RESP_L						
		
		#
		#	Check the acp command queue
		#

		lw	t0,	qdesc_pProducer_s(pACPCMDQ_REG)	# load producer to t0
		lw	t1,	qdesc_pConsumer_s(pACPCMDQ_REG)	# load consumer to t1
        bne	t0,	t1, ACPCMDQ_SERVICE_L 			# not equal, branch to service 
												# routine,else check next queue

		b	TX_SCHEDULER_L		

	.end	TX_SCHEDULER_L

###############################################################################


 
###############################################################################
##  BIGA_SERVICE_L:
##
##  Biga Recv buffer service
##      On entry to this label:
##		pTEMP_BHEAD_REG has current head pointer
 #		pBHEAD_REG is really the bigard_pNext field.  Therefore,
 #		*(pBHEAD_REG) is first recv descriptor that is to be serviced
 #		
 #      Service strategy:  One packet per per invocation
 #
 #      The biga recv desc has:
 #          <buffer pointer, buffer length, actual recv len, index>
 #			followed by 2 byte pad, 12 byte synergy header, 6 bytes DA
 #			6 byte SA and then the user data.
 #
 #      The information needed before the packet can be processed is:
 #          dest mac address
 #          vpc index
 #
 #  	DMA in biga_rd + sy_hdr + 6 byte DA to location in VCR
 #		
 #		Parse synergy header
##
##	INVOCATION OF TX_SEGMENTATION_PROCESS:
##		The data packet service routine ACP_DATAQ_SERVICE_L expects: 
##			a0 contains pointer to command record
##			a1 contains command header
## where the command record is:
##
##  	UINT8   cmd_length;            length of command + parmas   
##    	UINT8   cmd_wakeup;            completion
##    	UINT8   cmd_code;              request / response code    
##    	UINT8   cmd_status;            completion status         
##		UINT32  txsend_len;            length of data buffer  
##    	UINT32  txsend_vpc;            which vpc to send it on  
##		void    *txsend_pbuf;          pointer to data         
##	While DMA of bigard is in progress, construct the command.  After DMA 
##	of bigard is done, invoke the ACP_DATAQ_SERVICE_L 
##
##
##	EXIT steps:
##	
##  ** Update tail
##		softrtail = softrhead;
##		WRITERTAIL(softrtail);		update biga tail
##           
##	** Compute softrhead
##
## 
###############################################################################

	.globl	BIGAQ_SERVICE_L
	.ent	BIGAQ_SERVICE_L
BIGAQ_SERVICE_L:


	#
	#	DMA in the current recv biard + sy header + da to vcr
	#
    #   1) Compute effective address of DMAC register
    #   Construct effective address
    #       Local addr byte offset  = 0
    #       byte count              = 16 byte rd + 2 byte pad + 12 byte syhdr +
	#									6 byte da = 36 bytes
    #       read/write              = 1
    #       ghost bit               = 0
    #       local address           = vcr_bigard_s(VCR_REG) 
    #
    #   2)  Store the main memory start address to the effective address
    #       to start DMA
    #
   
 
    #
    #   Get ready for DMA of 44 bytes of Biga Receive Descriptor and DA

    # Load effective addr in t1
	lw		t1, vcr_bigard_ea_s(VCR_REG)

	#   pBHEAD_REG points to the pNext field of the bigard
    # 	get *(pBHEAD_REG), as that points to begining of the recv desc.
	#   *** Optimization ***
	#   We have to get a pointer to start of buffer. Instead of using
	#   *(pBHEAD_REG) and thus going out to Frame Buffer, we can use 
 	#   the Next field in the previous bigard already sitting in the VCR.
	#

	lw		t3, bigard_pNext_s+vcr_bigard_s(VCR_REG)


	# Start DMA
    sw      t3, (t1)               				# Store host addr to effective 
												# address.  This starts the DMA

	# DMA IN PROGRESS so do other stuff that does not need bigard info

	# Setup for potential CAM SET
    la  	a0, vcr_cmd_s(VCR_REG)      # cmd record in vcr
    lw  	a1, cmd_hdr_s(a0)           # get command header

	
    #
    #   Now update pBHEAD_REG, which till now is the softrhead, to point
    #   to the next desc.
    #
    move    pBHEAD_REG, t3                  # Now pBHEAD_REG is the current
                                            # descriptor
                                            # softhead update happens later

	#
    # Also, do some prep for first cell DMA: Construct effective addr in t0
    #
 
    li      t0, (48<<DMA_BCSHFT)|M_DMACR|DMA_RD # byte count, read bit
    addu	t8, VCRTXCELL_REG,vcrcell_payload_s # payload portion of cell
    and     t8, 0xfff                           # Only lower 12 bits are valid
    or      t0, t8                              # t0 now has effective addr

	# Setup for CAM lookup
	li		t9, SAR_WRITE_REG0_C
	li		t8, SAR_WRITE_REG1_C

	# Preload the crc generator with all 1's.  
	li		t5,		0xffffffff


	#
	#	Wait for DMA to complete.  After DMA is complete, we have the
	#	synergy header in vcr
	#

1:	bc3t	1b								# wait for co-proc 3 condition 

	# DMA done so setup CRC generator for new packet
	sw		t5,		CRC32_AREG			# All 1's to crc register


	# Start CAM lookup
BIGA_CAMGET_L:
	ulw		t7, bigard_da_s+vcr_bigard_s(VCR_REG)	# Load 1st 32 bits of MAC

	# Now load the next 16 bits of the mac addr, and 16 bits of VLANID
	lh		t6, bigard_da_s+vcr_bigard_s+4(VCR_REG)
	lh		t3, sy_vlanid_cl_s16+bigard_syhdr_s+vcr_bigard_s(VCR_REG)
	sll		t6, 16							# make space for VLANID
	srl		t3, 1							# lose the C/L bit
	or		t6, t3							# now we have MAC and VLANID

	# Start CAM lookup
	sw		t7, (t8)						# write to write_reg1
	sw		t6, (t9) 						# write to write_reg0


	# Setup for CAM SET
	sw		t7, camp_macaddr_s+vcr_cmd_s(VCR_REG)
	sw		t6, 4+camp_macaddr_s+vcr_cmd_s(VCR_REG)

	# Load address of start of buffer into t2
	lw	t2,  bigard_pData_s+vcr_bigard_s(VCR_REG)

    #
    # ** compute next tail, But write to tail reg *after* packet is processed
	# If we compute soft tail using the Prev pointer of the current buffer,
    # then it takes care of the case where the previous buffer may have 
    # been removed from the BIGA RBD chain for ACP IPC processing.
    #
	lw		pBTAIL_REG, bigard_pPrev_s+vcr_bigard_s(VCR_REG)
	addu	pBTAIL_REG, BIGARD_PNEXT_OFFSET


BIGA_MORE_L:
    #
    #   BIGA More bit processing
    #       If more bit set, drop packets upto and including the
    #       first descriptor which does not have more bit set
    #

	# length of received data from BIGA
    lw      t6, bigard_control_s+vcr_bigard_s(VCR_REG)  
    and     t7, t6, 0x8000              # Test more bit
    beqz    t7, BIGA_PARSESYHDR_L       # If not set, carry on
                                        # Else drop packets upto & including
    									# first bigard with more bit not set
	andi	t6, 0x7ff

	# load length of data buffer
    lw      t4, bigard_usable_length_s+vcr_bigard_s(VCR_REG) 


MORE_LOOP_L:

	sub		t6, t4	
	ble		t6, zero, MORE_UPDATE_L

	# Move head forward
	lw		pBHEAD_REG, bigard_pNext_s(pBHEAD_REG)

	b		MORE_LOOP_L

MORE_UPDATE_L:

	# Move tail forward 
	lw		pBTAIL_REG, bigard_pPrev_s(pBHEAD_REG)
	addu	pBTAIL_REG, BIGARD_PNEXT_OFFSET

	# BIGAQ service expects the VCR to contain Next, so update it
	lw		t6, bigard_pNext_s(pBHEAD_REG)
	sw		t6, bigard_pNext_s+vcr_bigard_s(VCR_REG)

	# since we started cam lookup, wait for it to finish
	bal	CAMLOOKUP_WAIT_L	
    b       BIGA_DROP_PACKET_L         # Exit



BIGA_PARSESYHDR_L:							# Parse synergy header

	#
	#	Conditions to determine:
	#		- flood to color because unknown (index) vpc for dest mac
	#		- flood to color because of a bcast dest mac
	#		- directed to ACP.
	#		- Known unicast index (vpc)
	#		- Cam state
	#

	#
	#	While CAM lookup is in progress, do further catagorization
	#

BIGA_GETBCASTVC_L:						# lookup busvpc for this vlan
										# if vlan 0, carry on
	beq		t3, 0, BIGA_VLAN_0			# t3 has the vlanid computed above
	lw		t4, vcr_plectable_s(VCR_REG)# address of base of Lec table
	sll		t3,	3						# multiply by sizeof(lecentry)
										# which is 4 bytes
	addu	t4, t3						# t4 now points to the lec entry
										# that has the busvpc and lecid

	lw      t9, lece_lecid_16s(t4) 		# load lecid and BUS VC into t9
	srl		t3,	3						# restore color
										# While t9 is being loaded, do
										# more work

1:
BIG_GETDINDEX_L:
	#
	#	Load the index field from the biga desc
	#	Examine:
	#		- If Empty bit set
	#			Error condition, card should be reset
	#		- If Flood bit set  
	#			If i/G bit set 	=> real mcast/bcast packet
	#				Send to bus vc
	#			Else			=> dest index is not known to earl
	#				Send unknown dest event to LEC 
	#				Send packet to bus vc
	#
	#

	lw		t5,	bigard_index_s+vcr_bigard_s(VCR_REG)		# load index field
	and		t6,	t5, INDEX_MASK_C		# t6 now has the vpc derived from index


1:
	#
	#	AT THIS STAGE, registers used are: 
	#		t3:	color
	#		t5: index from bigard
	#		t6: vpc as in the index
	#

BIGA_INDXEMPTY_L:

#	and		t7,	t5,	INDEX_EMPTY_BIT_C	# if empty bit *zero*, error
#	beq		t7,	zero, TXSAR_CRASH_L


	beq		t9, 0, BIGA_DROP_PACKET_L	# If lec entry is clear it means that 
                                        # VLAN is not configured so drop pkt

# NEW PVC CODE
    # Is this VLAN configured to run over a PVC?
    and     t8, t9, ALC_PVC_ENCAP_MASK_C  # Get PVC Encap mode in t8
    and     t9, t9, ~ALC_PVC_ENCAP_MASK_C # Clear PVC Enap from t9
	sw		t9, vcr_lecid_16s(VCR_REG)	  # store <lecid, busvpc> in order

    beq		t8, zero, first_cell_DMA_L  # Normal LANE, no PVC

    bne     t8, ALC_PVC_LLC_ENCAP_C, BIGA_DROP_PACKET_L
        # LLC SNAP bridged encoding
        add t2, (SYHDR_SIZE-10)             # this is where SNAP hdr starts
        # Write SNAP header in beginning of packet in Frame Buffer
		li  t1, 0xaaaa0300
		sw	t1, (t2)
		li  t1, 0x80c20007
		sw	t1, 4(t2)
		sh	zero, 8(t2)                     # write 0 to two pad bytes
        lh  t1, sy_length_16s+bigard_syhdr_s+vcr_bigard_s(VCR_REG)
        addu    t1, 6                       # Add SNAP hdr (10) sub FCS (4) so 6

		# Make command record in VCR
		sw  t2, txsend_pbuf_s(a0)
		sw  t1, txsend_len_s(a0)

		# Multi/Broad cast check
        lb	t7,  bigard_da_s+vcr_bigard_s(VCR_REG)
        and t7, 0x1              # Clear all but the Broadcast/Multicast bit
        bne t7, zero, PVC_FLOOD_L

        # Now check flood bit in index; if known then send packet on PVC in indx
        and    t8, t5, INDEX_FLOOD_BIT_C   # Test flood bit
        beq    t8, zero,  PVC_FLOOD_L      # If bit eq 1 then index is known

        # If packet is Unicast known, send it to PVC obtained from index
	    sw  t6, txsend_vpc_s(a0)		    # store it in cmd record
        bal ACP_DATAQ_SERVICE_L
        b   BIGA_SERVICE_DONE_L
   
        # If packet is Unicast but Unknown or Broad/Multicast, send it
        # to all the PVCs for this VLAN

PVC_FLOOD_L:
        lw	t1, lece_pvc_s(t4)              # Get PVC Info ptr from LEC Entry
        # Send packet on all the PVCs on this VLAN
1:      beq	t1, zero, BIGA_SERVICE_DONE_L
            lhu t9, pvc_vpc_16s(t1)				# Get VPC for this PVC
		    sw  t9, txsend_vpc_s(a0)		    # store it in cmd record
            bal ACP_DATAQ_SERVICE_L
            lw	t1, pvc_next_s(t1)				# Get next pvc ptr
            b	1b

            

first_cell_DMA_L:
# START FIRST CELL DMA
	# Pull length out of sy header
	lh		TXLENGTH_REG, sy_length_16s+bigard_syhdr_s+vcr_bigard_s(VCR_REG)

	# At this point, write the LECID back to the buffer start in Frame Buffer
	# t2 has start of buffer
	# t9 has lecid and bus VC
	add	t2,	(SYHDR_SIZE-LEC_HDRSZ_C)		# this is where lec hdr starts
	move t7, t9								# copy lecid, busvpc to t7
	srl	 t7, 16								# t7 now has lecid
	sh	 t7, (t2)							# write lecid to start of pkt

	# Packets from BIGA cannot be less than 1 cell long. if so, drop it
	subu	TXLENGTH_REG, 2					# substract CRC, add LEC HDR so 2
   	bleu    TXLENGTH_REG, AAL5_PAYLOADSZ_C, BIGA_DROP_PACKET_L
 
    #
    #   Start DMA of first cell
    #  
	#   Effective addr t0 was setup earlier during header DMA
    sw      t2, (t0)                        # Store host addr to effective addr
                                            # This starts the DMA

BIGA_CHECK_FOR_B_M_CAST_L:
	# Check the Broadcast/multicast bit in the first byte of DA and send
    # to BUS if set
	lb	t7,	bigard_da_s+vcr_bigard_s(VCR_REG)	
	and t7, 0x1              # Clear all but the Broadcast/Multicast bit
	beq	t7, zero, BIGA_CHECKFLOOD_L
		# Broadcast/Multicast bit set so send it to the BUS
		and t6, t9, 0xffff   # t9 contained lecid, busvc
		bal	CAMLOOKUP_WAIT_L # since CAM lookup started, wait for finish
		lw		t8, vcr_txspram_pStats_s(VCR_REG)
		lw		t1, txgl_bpacket_output_s(t8) 
		addu	t1, 1		 # txgl_bpacket_output_s++
		sw		t1,	txgl_bpacket_output_s(t8)
		b BIGA_XMIT_L


BIGA_CHECKFLOOD_L:						# Check if a flood is being requested
										# for the color
    #
    #   If dest index is the bus vc, send to bus and start learp
    #   else Wait for cam lookup to complete
	#		t5: index from bigard
    #       t6: vpc as in the index
    #       t9: contains lecid, busvc
    #
	li		t7,	TXSAR_START_LEARP_IND_C	# Potential start learp code
	andi	t9, 0xffff					# extract bus vc value
    beq     t6, t9, BIGA_UNKNOWN_L 		# if vc == bus vc, treat as unkown
 										# Else
	and		t8,	t5,	INDEX_FLOOD_BIT_C	# Test flood bit
	bne		t8,	zero,  BIGA_KNOWN_L		# If bit eq 1 then index is known
										#
	b		BIGA_UNKNOWN_L				# Else flood,

			#
			#	UNKNOWN INDEX PROCESSING
			#
										#
										# Else this is a flood because
BIGA_UNKNOWN_L:							# dest index uknown. Flood to color
										# subject to pacing counter and 
										# cam state.
										# Generate start_learp_event
										# for lec, if camstate so indicates
										#

										# Update flood count
	lw		t8, vcr_txspram_pStats_s(VCR_REG)
	lw		t1, txgl_unknownflood_output_s(t8) 
	addu	t1, 1						# txgl_unknownflood_output_s++
	sw		t1,	txgl_unknownflood_output_s(t8)


	#
	#	Wait for cam lookup completion
	#

	bal	CAMLOOKUP_WAIT_L

	#
	#	check v0 for return code, v1 for pointer to came entry.  
	#	If no cam entry 
	#		send start_learp_event to the ACP and send to BUS
	#	If entry exists
	#		 check state and bus pacing counter
	#
	#

	beq		v0,	SAR_CMD_SUCCESS_C, BIGA_INCAM_L # check for success
	

NOCAME_L:								# Else no came entry
										# Use the cam create command that
										# was built earlier
		#
		#	Registers used by cam functions: a2, a3, v0, v1, t2, t4, t5, t9
		#	Need to preserve t2,t3,t4,t5, t6, t7
		#

	sw  t2, vcr_save1_regt2_s(VCR_REG)
	sw  t3, vcr_save1_regt3_s(VCR_REG)
	sw  t4, vcr_save1_regt4_s(VCR_REG)
	sw  t5, vcr_save1_regt5_s(VCR_REG)
	sw  t6, vcr_save1_regt6_s(VCR_REG)
	sw  t7, vcr_save1_regt7_s(VCR_REG)

	la	t4, CAM_SET_L 					# Invoke set cam state
	jalr t4
	
		#
		#	Check return code in v0
		#		If cam set failed, crash the system 
		#		Else
		#			Cam index is in field camp_index_16s(a0)
		#			Send start arp event
		#			Send data on bus vc
		#	BIGA_DROP_PACKET_L

	bne	v0, SAR_CMD_FAIL_C, 1f
        # if CAM full, then send to BUS anyway
        lw  t2, vcr_save1_regt2_s(VCR_REG)
        b BIGA_SEND_ON_BUS_VPC_L
1:

	lh		a1, camp_index_16s(a0)		# retrieve the chosen cam index
	sll		a1,	3						# multiply by sizeof(camentry)
										
	li		t9,	TX_SAR_SPRAM_START_C	# get to 
	lw		t8, txspram_pCamTable_s(t9)	# base of cam table
 	addu	v1, a1,	t8					# This is the pointer to the 
										# cam entry.  Set state = 
										# LE_ARP_IN_PROGRESS_C
	li		t7, LE_ARP_IN_PROGRESS_C
	sb		t7, came_macstate_8s(v1)	# this completes the cam set command
	li		t7, PACE_COUNT_C			# set pacing counter
	sb		t7,	came_pacing_counter_8s(v1)	

		#
		#	Restore register context
		#

	lw  t2, vcr_save1_regt2_s(VCR_REG)
	lw  t3, vcr_save1_regt3_s(VCR_REG)
	lw  t4, vcr_save1_regt4_s(VCR_REG)
	lw  t5, vcr_save1_regt5_s(VCR_REG)
	lw  t7, vcr_save1_regt7_s(VCR_REG)
										# START_LEARP_L expects camentry ptr in v1

	bal	START_LEARP_L					# start_learp_event to acp
										# Pull out bus vpc from lecentry table
										# load to t6

BIGA_SEND_ON_BUS_VPC_L:
										# Send packet on bus vc for color
										# Pull out bus vpc from lecentry table
										# load to t6
	lhu t6, vcr_busvc_16s(VCR_REG)		# previously stored in VCR
	b	BIGA_XMIT_L						# transmit to bus vpc
										# Done

BIGA_INCAM_L:							# Dest has cam entry

		#
		# Check cam state:
		#	- loop test, send on LOOP_TEST vc
		#	- arp in progress: send to bus
		#	- hold state, place in holding queue
		#	- if active, then drop this packet
		#

	lbu		t8,	came_macstate_8s(v1)	# load and check cam state
	beq		t8,	CAME_LOOPTST_C, BIGA_LOOPT_L
	beq		t8,	LE_ARP_IN_PROGRESS_C, BIGA_ARP_INPROG_L
	beq		t8,	VC_ACTIVE_C, BIGA_SEND_ON_DD_VPC_L
	
	# If FLUSH in progress, drop packet
	beq		t8,	HOLDING_STATE_C, BIGA_DROP_PACKET_L
	b		BIGA_DROP_PACKET_L			# TXSAR_CRASH_L	unknown state for dest

# Send on the Data Direct VCC (since it came in as UNKNOWN, we pick up the
# Data Direct VCC from the CAM entry)
BIGA_SEND_ON_DD_VPC_L:
	lhu		t6,		came_vpc_16s(v1)	# Load Data Direct VPC from CAM tbl

	# Send packet if the Data Direct VC as learned from the CAM is active 
    la      t7,     vcr_txvcstate_s(VCR_REG)    # load address of vpcstate
    addu    t7,     t6                  # create index to vpc state entry
    lb      t8,     -1(t7)              # load vpc state entry (0 based)
    beq     t8, VPC_TX_ACTIVE_C, BIGA_XMIT_L # if vc is active, send packet

	# The Data Direct VPC in the CAM entry is not active so delete CAM 
    # entry and try again
	sw  t2, vcr_save1_regt2_s(VCR_REG)
	la	t4, CAM_CLEAR_L
	jalr t4
	lw  t2, vcr_save1_regt2_s(VCR_REG)

    li t7, TXSAR_CLEAR_MAC_IND_C
	b NOCAME_L


										# INDEX KNOWN PROCESSING
BIGA_KNOWN_L:							# index, hence vpc is known
#####
                                        # If vc == INVALID_VC, treat as unknown
    #
    #   Check vc state table, if vc inactive treate as unknown, but
    #   create a flag such that the start learp routine sets the event
    #   to TXSAR_CLEAR_MAC_IND_C
 
    la      t7,     vcr_txvcstate_s(VCR_REG)    # load address of vpcstate
    addu    t7,     t6                  # create index to vpc state entry
    lb      t8,     -1(t7)              # load vpc state entry (0 based)
    li      t7,     TXSAR_CLEAR_MAC_IND_C # This VC is no longer active
    and     t8,     VPC_TX_ACTIVE_C     # check if vc is active
    beqz    t8,     BIGA_UNKNOWN_L

	bal	CAMLOOKUP_WAIT_L

	#	check v0 for return code, v1 for pointer to cam table entry

		#
		#	If entry exists, Check cam state:
		#		- if arp_in_progress send to bus vc for vlan
		#		- if holding state, put in holding queue and return 
		#		- if established, send to vpc, goto BIGA_UCAST_L
		#
		#

	
	#	If entry does not exist, send to vpc
	bne		v0,	SAR_CMD_SUCCESS_C, BIGA_XMIT_L 		# check for success

	lbu		t8,	came_macstate_8s(v1)				#load and check cam state
	beq		t8,	VC_ACTIVE_C, BIGA_DEL_CAM_AND_XMIT_L
	beq		t8,	LE_ARP_IN_PROGRESS_C, BIGA_ARP_INPROG_L
	beq		t8,	CAME_LOOPTST_C, BIGA_LOOPT_L
	beq		t8,	HOLDING_STATE_C, BIGA_DROP_PACKET_L
	beq		t8,	CAME_DESTACP_C, BIGA_DROP_PACKET_L  # Shouldn't happen
	b		BIGA_DROP_PACKET_L			# TXSAR_CRASH_L	Else TRAP, invalid cam state

BIGA_LOOPT_L:
	li		t6,	LOOPT_VPC_C				# send to test vc
	b		BIGA_XMIT_L

BIGA_ARP_INPROG_L:						# arp in progress, send to bus
										# subject to pacing counter

	lbu		a3,	came_pacing_counter_8s(v1)	# load and check cam state

	# sakumar 9/5/95. On advice from Brad, if counter drops to zero, 
	# stop sending LE ARP, but forward the packet to the BUS.
	beqz	a3,	1f						# Skip LE ARP if counter 0

	li		t7, TXSAR_START_LEARP_IND_C
	bal		START_LEARP_L				# start_learp_event to acp

	# Don't decrement pacing count if LEARP failed
	beq	v0, SAR_CMD_FAIL_C, 1f
		subu	a3, 1
		sb		a3,	came_pacing_counter_8s(v1)	
1:
	lh		t6,	vcr_busvc_16s(VCR_REG)		# bus vpc to send on
	b		BIGA_XMIT_L

# We got a KNOWN packet, the CAM entry indicates active so delete CAM entry
# and transmit the packet
BIGA_DEL_CAM_AND_XMIT_L:

	sw  t2, vcr_save1_regt2_s(VCR_REG)
	sw  t6, vcr_save1_regt6_s(VCR_REG)
	la	t4, CAM_CLEAR_L
	jalr t4
	lw  t2, vcr_save1_regt2_s(VCR_REG)
	lw  t6, vcr_save1_regt6_s(VCR_REG)

	b	BIGA_XMIT_L

BIGA_XMIT_L:

    #
	#	NOTE:	
	#   at this point, t2: is start of packet
	#   	 TXLENGTH_REG: length of AAL5 packet
	#                  t6: contains vpc
	#
	#   Setup for BIGA_XMIT_PKT_L which expects the following:
	#   TXLENGTH_REG: length of AAL5 packet
	#             t4: pointer to buffer
	#             t6: VPC

	move	t4, t2 							# t4 now points to packet start

	#
	#	Invoke transmit procedure
	#
	
	bal	BIGA_XMIT_PKT_L						# call handler

	#
	#	Transmission Done.
	#	Packet processed, update tail.  This will allow biga to proceed
	#

BIGA_DROP_PACKET_L:							# update tail and return
BIGA_SERVICE_DONE_L:
	addu	pBHEAD_REG, BIGARD_PNEXT_OFFSET	# this is new softrhead
    sw      pBTAIL_REG, BTAILOFF_C(BIGA_BASE_REG)
	
	# if softhead (pBHEAD_REG) is not eq to actual head continue xmitting
	bne		pBHEAD_REG, pTEMP_BHEAD_REG, BIGAQ_SERVICE_L

	#
	# branch back to check co-processor conditions
	#

BIGA_SERVICE_EXIT_L:

	b		TX_SCHEDULER_CP_L

BIGA_VLAN_0:
	# Only IPC messages come on VLAN 0 so if it does not match our mac drop it
	bal CAMLOOKUP_WAIT_L
	bne     v0, SAR_CMD_SUCCESS_C, BIGA_DROP_PACKET_L
	lbu     t8, came_macstate_8s(v1)                #load and check cam state
	beq		t8,	CAME_LOOPTST_C, BIGA_LOOPT2_L
	bne     t8, CAME_DESTACP_C, BIGA_DROP_PACKET_L
	bal ACP_DATAIND_L                   # Give data indication event to ACP
    b       BIGA_SERVICE_DONE_L

# For Diagnostic Loopback test
BIGA_LOOPT2_L:
	li	t6, LOOPT_VPC_C          # send to test vc
	sw	t6, txsend_vpc_s(a0)
	add	t2, (SYHDR_SIZE-LEC_HDRSZ_C)# this is where lec hdr starts
	sw	t2, txsend_pbuf_s(a0)
	lh	TXLENGTH_REG, sy_length_16s+bigard_syhdr_s+vcr_bigard_s(VCR_REG)
	subu	TXLENGTH_REG, 2               # substract CRC, add LEC HDR so 2
	sw	TXLENGTH_REG, txsend_len_s(a0)
	bal	ACP_DATAQ_SERVICE_L
	b	BIGA_SERVICE_DONE_L

	.end	BIGAQ_SERVICE_L

###############################################################################




###############################################################################
##	ACPCMDQ_SERVICE_L:
##
##	ACP command queue service routine
##		On entry to this label, t0 has producer pointer, 
##		t1 has consumer pointer for the acp command queue
##		The command queue is exhaustively serviced
##	LOOP:	
##		Dereference command record from consumer pointer
##		Parse the command
##		branch to command handler
##		increment consumer index
##		If queue not empty, goto LOOP
##		
##
###############################################################################


	.globl	ACPCMDQ_SERVICE_L
	.ent	ACPCMDQ_SERVICE_L
ACPCMDQ_SERVICE_L:
		#
		#	If command queue empty, return
		#
		
        beq	t0,	t1, TX_SCHEDULER_L 			# equal, branch back to scheduler
		
		#
		#	Elese Get the command hdr from consumer pointer in t1
		#
		lw	a0,	(t1)						# get pointer to command record
		lw	a1,	cmd_hdr_s(a0)				# get command header
		and	t3, a1, CMDCODE_MASK_C			# mask off other info in cmd hdr

		#
		#  Now call the command handlers.  
		#	NOTE:  All Command handlers are leaf functions
		#  
	
		bne	t3,	TXSAR_SET_VPCMAP_C,	1f		# Check for SET vpc command
		bal	VPC_SET_L						# call handler
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_GET_VPCMAP_C,	1f		# Check for get vpc command
		bal	VPC_GET_L						# call handler
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_CLEAR_VPCMAP_C,	1f	# Check clear vpc command
		bal	VPC_CLEAR_L						# call handler
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_INIT_CAM_C,	1f		# Check for INIT cam command
		la	t4, CAM_INIT_L 					# call handler
		jalr t4
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_CREATE_CAM_C,	1f		# Check for CREATE cam command
		la	t4, CAM_SET_L 					# call handler
		jalr t4
        b ACP_CMD_DONE_L

        
        # Both the following commands are handled by the same function
1:	    beq	t3,	TXSAR_CHANGE_CAM_STATE_C, 2f# Check for CHANGE_CAM_SATE
		bne	t3,	TXSAR_GET_CAM_C,	1f		# Check for GET cam command
2:		la	t4, CAM_GET_OR_CHANGE_L 		# call handler
		jalr t4
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_CLEAR_CAM_C,	1f		# Check for Clear cam command
		la	t4, CAM_CLEAR_L 				# call handler
		jalr t4
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_SEND_REQUEST_C,	1f	# Check for send request
		bal	ACP_DATAQ_SERVICE_L				# call handler
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_SET_VPI_VCI_BITS_C,1f	# Check for set vci vpi bits command
		lw	ACTIVE_VCI_MASK_REG, tx_active_vci_mask_s(a0)
		lw	NUM_ACTIVE_VCI_BITS_REG, tx_num_active_vci_bits_s(a0)
		li	v0,	SAR_CMD_SUCCESS_C			# return value
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_RESTART_C,	1f		# Check for reset
#		bal	TXSAR_RST_L						# call handler
        b ACP_CMD_DONE_L

1:		bne	t3,	SAR_NOP_C,	1f				# Check for send request
		bal	TXSAR_NOP_L						# call handler
        b ACP_CMD_DONE_L

1:		bne	t3,	TXSAR_SET_THROTTLE_C,1f		# Check for send request
	    lw	t4, tx_throttle_parm_s(a0)		# Get throttle parameter
    	sw  t4, CIR_REG9_AREG               # Write value to CIR
		li	v0,	SAR_CMD_SUCCESS_C			# return value
        b ACP_CMD_DONE_L

1:		li	v0,	SAR_CMD_INVALID_C			# Unknown command
        b ACP_CMD_DONE_L

ACP_CMD_DONE_L:
		bal	DEQ_CMD_L						# update consumer for cmd q
        sw  zero, HOSTINT_AREG		        # Notify Host
		b	ACPCMDQ_SERVICE_L				# service more commands


	.end	ACPCMDQ_SERVICE_L

###############################################################################





###############################################################################
##	ACP_DATAQ_SERVICE_L:
##
##	ACP data queue service routine
##		On entring this procedure: 
##			a0 contains pointer to command record
##			a1 contains command header
##			t0 contains producer pointer
##			t1 contains consumer pointer
##			t3 contains command code
##		
##		Get lenght of data
##		if length <= 0 
##			invalid command
##		Get buffer
##		start fragmentation and cell transmission
##
###############################################################################

	.text
	.globl	ACP_DATAQ_SERVICE_L
	.ent	ACP_DATAQ_SERVICE_L

ACP_DATAQ_SERVICE_L:
###################
	
	li		v0,	SAR_CMD_SUCCESS_C				# return value
	lw		TXLENGTH_REG,	txsend_len_s(a0)
	blez	TXLENGTH_REG,	TXLEN_ERR_L			# Do error processing


first_cell_L:
############
	#
	# 	Start cell fragmentation
	#	TXLENGTH_REG has length of data
	#	NOTE: t4 holds current address to dma from
	#	t6 hold which vpc to send it on
	#


	lw		t4,		txsend_pbuf_s(a0)			# Load start of buffer
	lw		t6,		txsend_vpc_s(a0)			# Load vpc to send it on
	sub		t6,		1							# make to an index
	#
	#	Check vc state table, if vc inactive do error processing
	#

	la		t5,		vcr_txvcstate_s(VCR_REG)	# load address of vpcstate
	addu	t5,		t6							# index to vpc state entry
	lb		t7,		(t5)						# load vpc state entry	
	and		t7,		VPC_TX_ACTIVE_C				# check if vc is active
	beqz	t7,		INACTIVE_ERR_L

	add		t6,		1							# restore actual vpc value



	#	Preset the crc generator with all 1's.  
	#
	li		t5,		0xffffffff
	sw		t5,		CRC32_AREG			# All 1's to crc register

	#	DMA to first cell builder area
	#
	#	1) Compute effective address of DMAC register
	#	Construct effective address
	#		Local addr byte offset	= 0
	#		byte count				= 48
	#		read/write				= 1
	#		ghost bit				= 0
	#		local address			= vcr_txbuf_s(VCR_REG) i.e first cell 
	#														builder in vcr
	#
	#	2)  Store the main memory start address to the effective address 
	#		to start DMA
	#

	#
	# Construct effective addr in t5
	#

	li		t5,	(48<<DMA_BCSHFT)|M_DMACR|DMA_RD	# byte count, read bit
    addu	t7, VCRTXCELL_REG,vcrcell_payload_s # payload portion of cell
	and		t7,	0xfff							# Only lower 12 bits are valid
	or		t5,	t7								# t5 now has effective addr

	# If first cell is also last cell, skip starting DMA here since
    # it may not end in a 4 word boundary. Jump ahead to setup 
	# Cell header and increment stats though.
	bleu	TXLENGTH_REG, AAL5_PAYLOADSZ_C, 1f

	#
	#	Start DMA
	#	
	sw		t4,	(t5)						# Store host addr to effective addr
											#	This starts the DMA

1:

BIGA_XMIT_PKT_L:
	# The transmit from BIGA call enters here since the previous checks 
	# are needed only in the case of packet coming in from ACP
	# Note that DMA of the first cell has already been started
	# At this point the following registers must be set:
	# TXLENGTH_REG: length of AAL5 packet
	#           t4: pointer to buffer
	#           t6: VPC

	# In t7, get the vc stat offset for per-vc-stats
	move	t7, t6
	sll		t7, 3 		# since txvc_size_s is 8 bytes

	#	While DMA is in progress, build cell header
    #
    # Create a cell header in AAL5HDR_REG.  The vpc is in t6.
    # cell has:  gfc:4, vpi:8, vci:16, pt:3, clp:1
    # Construct vpi from vpc as:  vpc >> VCI_BITS_C
    # Construct vci form vpc as:    vpc & VCI_MASK_C
    #
 
   
    # NOTE: k0 and k1 used below if NO_PMON
#ifdef NO_PMON
    and     AAL5HDR_REG,  t6,  ACTIVE_VCI_MASK_REG      # get VCI
#else
    and     AAL5HDR_REG,  t6,  VCI_MASK_C       # get VCI
#endif /* NO_PMON */
    sll     AAL5HDR_REG,  4                     # shift to vci position
#ifdef NO_PMON
    srl     t6, NUM_ACTIVE_VCI_BITS_REG         # get VPI
#else
    srl     t6, VCI_BITS_C                      # get VPI
#endif /* NO_PMON */
    sll     t6, 20                              # shift to vpi position
    or      AAL5HDR_REG, t6                     # or into aal5 header
                                                # t6 is no longer valid


	# Increment Statistics: Per vc frame count as well as global frame count
	lw		t8, vcr_txspram_pStats_s(VCR_REG)
	addu	t8, txgl_size_s						# point to pervc stat area
	addu	t8, t7
	lw		t7, txvc_packet_output_s(t8) 

	# In delay slot while reading from SPRAM, incr global count
	# We keep the actual global count in the VCR, but write it to SPRAM.
	# This avoids an SPRAM read.
	lw		t9, vcr_txspram_pStats_s(VCR_REG)
	lw		t6, vcr_txgl_packet_output_s(VCR_REG)
	addu	t6, 1									# txgl_upacket_output_s++
	sw		t6,	txgl_upacket_output_s(t9)

	sw		t6, vcr_txgl_packet_output_s(VCR_REG)

	lw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count

	# Complete remaining per-vc increment of stats
	addu	t7, 1								# txvc_packet_output_s++
	sw		t7,	txvc_packet_output_s(t8)

	# Note that cell count is incremented elsewhere and won't include
    # THIS packets cells. Also the cell count is kept in a VCR variable
    # and is written out to SPRAM but never read from SPRAM.
	sw		t6, txgl_cell_xmit_s(t9)			# write cell count to spram 

	

	#
	#	Set up vc map entry for current data frame under going fragmentation
	#	Store actual length of user data, to be used later to compute crc
	#
	sw		TXLENGTH_REG,	vcr_omap_tlen_s(VCR_REG)

	#
	#	Check length to see if this is also the last cell
	#  	Before we can DMA in last cell, we have to check for 4 byte
	#	boundaries.
	#
	#   *** NOTE: ***
	#   We have already started DMA of the first cell, except if the packet 
	#   is less than or equal to 48 bytes, then we skipped starting DMA above
	#   since we need to do some more checks. 
	#   Also, if we arrrived here from BIGA service, the we have already 
    #   discarded any packet less than 64 bytes.
	bleu	TXLENGTH_REG, AAL5_PAYLOADSZ_C, last_cell_L	


	#
	#		Write atm header to vcr cell holder
	#		Compute cell address to be written to aci tx fifo in t8.
	#		Compute remaining length,
	#		Compute next host memory address
	#		Compute next VCR address, modulo number of tx buffers
	#		Compute next effective address
	#
											
    addu	t8, VCRTXCELL_REG,vcrcell_hdr_s # address to be written to txfifo
	sw		AAL5HDR_REG,	(t8)
	subu	TXLENGTH_REG,	AAL5_PAYLOADSZ_C# remaining length
	addu	t4,	AAL5_PAYLOADSZ_C			# next from dma shram address
	addu	VCRTXCELL_REG, 64				# next cell builder
	blt		VCRTXCELL_REG, ENDTXCELL_REG, 1f
	addu	VCRTXCELL_REG,	VCR_REG, vcr_txbuf_s # first cell build area
1:
											# Compute next effective address
	li		t5,	(48<<DMA_BCSHFT)|M_DMACR|DMA_RD	# byte count, read bit
	addu	t6,	VCRTXCELL_REG, vcrcell_payload_s# payload portion of cell
	and		t6,	0xfff						# Only lower 12 bits are valid
	or		t5,	t6							# t5 now has new effective addr
	
	#
	#	Fall through to continue cell processing
	#

continue_cell_L:
###############

	# increment cell count
	lw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count
	addu	t6, 1
	sw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count

	#
	#	Wait for DMA to complete
	#
1:	
	bc3t	1b							# wait for co-proc 3 condition 
	

	#
	#	Enable transmission of cell by writing vcr address from 
	#	register t8 to TX FIFO
	#

1:  bc2f	1b						# wait for PRU Timeout 
	lw		t6, CREDIT_AREG			# "Reading the CGCR clears the CGCR" Pg 7-3
	sw		t8,	TXFIFO_AREG


	#
	#	If remaining length <= 48 goto last_cell processig
	#
	bleu	TXLENGTH_REG, AAL5_PAYLOADSZ_C, last_cell_L	

	#
	#	Start next DMA, all address were computed before waiting
	#	for dma to complete
	#

	sw		t4,	(t5)						# Store host addr to effective addr


	#
    #   While DMA in progress, recompute next set of dma parameters:
    #       Write atm header to vcr cell holder
    #       Compute remaining length					  [TXLENGTH_REG]
    #       Compute cell address to be written to aci tx fifo in 	[t8].
    #       Compute next host memory address						[t4]
    #       Compute next VCR address, mod number of tx buffers [VCRTXCELL_REG]
    #       Compute next effective address							[t5]
    #   goto continue_cell_L
	#

    addu    t8, VCRTXCELL_REG, vcrcell_hdr_s# address to be written to txfifo
    sw      AAL5HDR_REG,	(t8)
    subu    TXLENGTH_REG,   AAL5_PAYLOADSZ_C# remaining length
    addu    t4, AAL5_PAYLOADSZ_C			# next from dma shram address
    addu    VCRTXCELL_REG, 64               # next cell builder, mod TXBUFSZ_C
	blt		VCRTXCELL_REG, ENDTXCELL_REG, 1f
	addu	VCRTXCELL_REG,	VCR_REG, vcr_txbuf_s# first cell build area
1:
                                            # Compute next effective address
    li      t5, (48<<DMA_BCSHFT)|M_DMACR|DMA_RD # byte count, read bit
    addu    t6, VCRTXCELL_REG, vcrcell_payload_s# payload portion of cell
    and     t6, 0xfff                       # Only lower 12 bits are valid
    or      t5, t6                          # t5 now has effective addr

	b		continue_cell_L					# continue cell processing	

last_cell_L:
###########
	#
	#	Check if user payload ends on 4 byte boundary.
	#	If not, write 0's to end of payload to force 4 byte allignment
	#	Start DMA with adjusted length
	
	and		t6,	TXLENGTH_REG, 3				# Compute misaligned bytes
	beqz	t6, 1f							# no mis alignment, start DMA
											# Else Compute address of last word
	move	t7,	t4							# t4 has starting address
	addu	t7,	TXLENGTH_REG				# address of end of user data

	#
	#	write out zeros to padd out (4 - remainder) no. of bytes
	#

	subu	t7,	t6							# Addr of last word in user data
	li		t9,	4
	subu	t6,	t9, t6						# 4 - remainder in t6
	sll		t6,	3							# number of bits to shift by
	lw		t9, (t7)						# read in last word to t9
	srl		t9, t6							# zero out (4 - rem) bytes
	sll		t9, t6							
	sw		t9,	(t7)						# write out last word
	srl		t6,	3							# get back 4 - remainder
	
1:
	# add 4-rem to leng and copmute 
	# effective dma address
											# Add (4-rem) to length
											# recompute dma effective address
	and		t5,	~DMA_BCMASK					# zero out byte count from dmac
	move	t7,	TXLENGTH_REG				# original length
	addu	t7,	t6							# add 4-rem to length
	sll		t7, DMA_BCSHFT					# shift left by 24
	or		t5, t7							# or into DMAC effective addr byte
											# count location. t5 now has the 
											# new valid DMA effective address
	
	#
	#	Start DMA
	#
	sw		t4,	(t5)						# Store host addr to effective addr

	#
	#	While DMA in progress:
	#	Compute required pad. TXLENGTH_REG has remaining length
	#
	#	If TXLENGTH_REG <= 40, AAL trailer can fit in last cell
	#		pad = 40 - TXLENGTH_REG
	#		Zero out pad bytes
	#		Write header with EOM
	#		Write length in trailer
	#		Wait for dma to complete
	#		Do ghost DMA over 2 octets of original user length to update crc.
	#		Wait for host DMA to finish
	#		Write crc in trailer crc
	#		Write to TX FIFO
	#		return
	#
	
	addu	t5, VCRTXCELL_REG, vcrcell_payload_s
	addu	t5, TXLENGTH_REG					# t5 now has begining of pad
												# in vcr cell builder
	addu	t5, t6								# add (4-rem)
	# t5 now points to the first pad byte that is on a word boundary
	

	#
	#	Check if trailer can fit in current cell
	#
	bgt		TXLENGTH_REG, 40, canned_cell_L
	
	# Compute pad in current cell

	li		t9,	40
	subu	TXLENGTH_REG, t9, TXLENGTH_REG		# NOTE:  TXLENGTH_REG now has 
												#	pad len
zero_pad1_L:
	subu	TXLENGTH_REG, t6					# sub (4-rem) for 4 byte align
	move	v1, TXLENGTH_REG					# save aligned pad len
1:												# zero out pad bytes in vcr
	sw		zero,	(t5)
	addu	t5, 4
	subu	TXLENGTH_REG, 4						# decrement pad length
	bgt		TXLENGTH_REG, zero, 1b
												# NOW PAD has been zeroed out

	or		AAL5HDR_REG,	AAL5_HDR_EOM_C		# Set EOM bit in header
												# write header
	addu	t8,		VCRTXCELL_REG, vcrcell_hdr_s
	sw		AAL5HDR_REG,	(t8)
												# write user data length 
	lw		TXLENGTH_REG,	vcr_omap_tlen_s(VCR_REG)
	sh		TXLENGTH_REG,	aal5tail_len_16s(VCRTXCELL_REG)

	sh		zero, aal5tail_uui_8s(VCRTXCELL_REG) # zero out uui and cpi

												# get address of cell header
	addu    t9,		VCRTXCELL_REG, aal5tail_crc_s
	#
	#	update VCRTXCELL_REG for next cell dma
	#
    addu    VCRTXCELL_REG, 64               # next cell builder, mod TXBUFSZ_C
	blt		VCRTXCELL_REG, ENDTXCELL_REG, 1f
	addu	VCRTXCELL_REG,	VCR_REG, vcr_txbuf_s# first cell build area

1:
												# Compute crc over length
	la		t6,	vcr_omap_tlen_s(VCR_REG)		# payload portion of cell
	#
	#  Substract 4 byte aligned pad from t6 to compesate for zeros 
	#	written to cell at label zero_pad_L
	#
	subu	t6, v1
	and		t6,	0xfff							# Only lower 12 bits are valid
	li		t5,	M_DMACR|DMA_WR|DMA_G			# write, ghost 
	addu	v1,	4								# count (pad + trailer)
	sll		v1, DMA_BCSHFT						
	or		t5, v1
	or		t5,	t6								# t5 now has new effective addr
	
												# wait for dma to complete
1:	bc3t	1b									# wait for co-proc 3 condition 
										
	#
	#	Start ghost dma to fake shram address
	#
	sw		t4,	(t5)

	# increment cell count
	lw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count
	addu	t6, 1
	sw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count

1:												# wait for dma to complete
	bc3t	1b									# wait for co-proc 3 condition 
	
	#
	#	Retrieve crc, and write to trailer address of which is in t9
	#
	lw      t5,		CRC32_AREG
	not		t5
	sw      t5,		(t9)

1:  bc2f	1b						# wait for PRU Timeout 
	lw		t6, CREDIT_AREG			# "Reading the CGCR clears the CGCR" Pg 7-3
	sw		t8,	TXFIFO_AREG     	# Write cell address to txfifo

	#
	#	Packet successfully fragmented and sent, return to caller

	j	ra

canned_cell_L:
############
    #   Else need to use canned last cell for trailer
    #       pad = 48 - TXLENGTH_REG
    #       Write header
    #       Zero out pad bytes
    #       Write header with EOM to canned cell
    #       Write length in trailer in canned cell
    #       Wait for dma to complete
    #       Write to TX FIFO
    #       Do ghost DMA over 2 octets of original user length to update crc.
    #       Wait for ghost DMA to finish
	#		Write pdu length in trailer
    #       Write crc in trailer crc
    #       Write canned cell to TX FIFO
    #       return
    #
	#	On entry to this label: 
	# 	t5 points to the first pad byte that is on a word boundary
	#

	li		t9,	48
	subu	TXLENGTH_REG, t9, TXLENGTH_REG		# NOTE:  TXLENGTH_REG now has 
                                                #   pad len
zero_pad2_L:
	subu	TXLENGTH_REG, t6					# add (4-rem) to go to 4 byte bndry
	move	v1, TXLENGTH_REG					# save aligned pad len
 
1:                                              # zero out pad bytes in vcr
	sw		zero,	(t5)
	add		t5, 4
    subu   	TXLENGTH_REG, 4                     # decrement pad length
    bgt     TXLENGTH_REG, zero, 1b
 
                                                # write header for current cell
	addu	t8,	VCRTXCELL_REG, vcrcell_hdr_s
    sw      AAL5HDR_REG, 	(t8)
	#
	#	update VCRTXCELL_REG
	#
    addu    VCRTXCELL_REG, 64               # next cell builder, mod TXBUFSZ_C
	blt		VCRTXCELL_REG, ENDTXCELL_REG, 1f
	addu	VCRTXCELL_REG,	VCR_REG, vcr_txbuf_s# first cell build area
1:


	#
	#	Now build last last cell that has the aal5 trailer
	#	

    # First zero out the 40 pad bytes
    la		t9, vcrcell_payload_s(VCRTXCELL_REG)
	addu    t6, t9, 40
1:	sw		zero, (t9)
    addu	t9, 4
	bne		t9, t6, 1b


	la		t9,		vcrcell_hdr_s(VCRTXCELL_REG)	
										
	or		AAL5HDR_REG,	AAL5_HDR_EOM_C		# Set EOM bit in header
												# write header
	sw		AAL5HDR_REG,	vcrcell_hdr_s(VCRTXCELL_REG)
												# Compute crc over length

    la      t6, vcr_omap_tlen_s(VCR_REG)        # address of length
	lw		TXLENGTH_REG,	(t6)				# store length in tail
	sh		TXLENGTH_REG,	aal5tail_len_16s(VCRTXCELL_REG)
	sh		zero, aal5tail_uui_8s(VCRTXCELL_REG)# zero out uui and cpi
    #
	#  Substract 4 byte aligned pad + canned cell size from t6 to compesate 
	# for zeros written to cell at label zero_pad2_L and the canned cell
    #
    subu    t6, v1
	subu	t6, 40
    and     t6, 0xfff                           # Only lower 12 bits are valid
    li      t5, M_DMACR|DMA_WR|DMA_G            # write, ghost
    addu    v1, 44                              # count (pad + trailer)
    sll     v1, DMA_BCSHFT
    or      t5, v1								# count
    or      t5, t6                              # t5 now has new effective addr


1:												# wait for previous dma complete
	bc3t	1b									# wait for co-proc 3 condition 
1:  bc2f	1b						# wait for PRU Timeout 
	lw		t6, CREDIT_AREG			# "Reading the CGCR clears the CGCR" Pg 7-3
	sw		t8,	TXFIFO_AREG 	    # Write cell address to txfifo

	#
	#	Start ghost dma to fake shram address
	#
	sw		t4,	(t5)
1:												# wait for dma to complete
	bc3t	1b									# wait for co-proc 3 condition 
	
	#
	#	Retrieve crc, and write to trailer
	#
	lw      t5,	CRC32_AREG
	not		t5
	sw      t5,	aal5tail_crc_s(VCRTXCELL_REG)

	# increment cell count
	lw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count
	addu	t6, 2									# real cell and canned cell
	sw		t6, vcr_txgl_cell_output_s(VCR_REG)     # load current cell count
1:  bc2f	1b						# wait for PRU Timeout 
	lw		t6, CREDIT_AREG			# "Reading the CGCR clears the CGCR" Pg 7-3
	sw		t9,	TXFIFO_AREG 		# Write cell address to txfifo

	#
	#	update VCRTXCELL_REG
	#
    addu    VCRTXCELL_REG, 64               # next cell builder, mod TXBUFSZ_C
	blt		VCRTXCELL_REG, ENDTXCELL_REG, 1f
	addu	VCRTXCELL_REG,	VCR_REG, vcr_txbuf_s# first cell build area
1:

	#
	#	Packet successfully fragmented and sent, return to caller

	j	ra

	#
	#	Error handling 
	#

TXLEN_ERR_L:

	li		v0,	TXSAR_INVALID_LEN_C				# return value
	j	ra

INACTIVE_ERR_L:
	
	li		v0,	TXSAR_INVALID_VPC_C				# return value
	j	ra

	.end	ACP_DATAQ_SERVICE_L

###############################################################################


 
 
 
###############################################################################
 


#							VPC  COMMANDS

###############################################################################
##
##	VPC_SET_L
##
##		On entring this procedure: 
##			a0 contains pointer to command record
##			a1 contains command header
##			t0 contains producer pointer
##			t1 contains consumer pointer
##			t3 conaints command code
##
##	Index into the vpc map entry in the VCR and set state to value in the 
##	comand parameter
##
###############################################################################
 
        .globl  VPC_SET_L
        .ent    VPC_SET_L
VPC_SET_L:

	li		v0,	SAR_CMD_SUCCESS_C		# return value
	lh		t4,	vpcp_vpc_16s(a0)		# load vpc value
	sub		t4, 1						# index goes from 0 - 1023
	#
	# check vpc range
	#
	bge		t4,	MAX_DEFAULT_VPC_C, VPC_SET_ERR_L# if (t4 >= MAX_DEFAULT_VPC) 

	lw		t5,	vpcp_vcstate_s(a0)				# load vpc state
	and		t5, 0x00ff
	la		t6,	vcr_txvcstate_s(VCR_REG)		# begining of vc state table
	addu	t6, t4								# index to vc entry
	sb		t5,	0(t6)

	j	ra

VPC_SET_ERR_L:
	li		v0,	TXSAR_INVALID_VPC_C
	j	ra

		.end	VPC_SET_L

###############################################################################


###############################################################################
##
##	VPC_GET_L
##
##      On entring this procedure:
##          a0 contains pointer to command record
##          a1 contains command header
##          t0 contains producer pointer
##          t1 contains consumer pointer
##          t3 conaints command code
##
##  Index into the vpc map entry in the VCR and get state value 
##
###############################################################################
 
        .globl  VPC_GET_L
        .ent    VPC_GET_L
VPC_GET_L:

    li      v0, SAR_CMD_SUCCESS_C       # return value
    lh      t4, vpcp_vpc_16s(a0)        # load vpc value
	sub		t4, 1						# index goes from 0 - 1023
 
    #
    # check vpc range
    #
    bge     t4, MAX_DEFAULT_VPC_C, VPC_GET_ERR_L # if (t4 >= MAX_DEFAULT_VPC)

    la      t6, vcr_txvcstate_s(VCR_REG)        # begining of vc state table
    addu    t6, t4                              # index to vc entry
    lb      t5, 0(t6)
	and		t5, 0x00ff
    sw      t5, vpcp_vcstate_s(a0)           	# store vpc state
 
    j   ra
 
VPC_GET_ERR_L:
    li      v0, TXSAR_INVALID_VPC_C
    j   ra


		.end	VPC_GET_L

###############################################################################


###############################################################################
##
##	VPC_CLEAR_L
##
##      On entring this procedure:
##          a0 contains pointer to command record
##          a1 contains command header
##          t0 contains producer pointer
##          t1 contains consumer pointer
##          t3 contains command code
##
##  Index into the vpc map entry in the VCR and clear state value
##
###############################################################################
 
        .globl  VPC_CLEAR_L
        .ent    VPC_CLEAR_L
VPC_CLEAR_L:

    li      v0, SAR_CMD_SUCCESS_C       # return value
    lh      t4, vpcp_vpc_16s(a0)        # load vpc value
	sub		t4, 1						# index goes from 0 - 1023
 
    #
    # check vpc range
    #
    bge     t4, MAX_DEFAULT_VPC_C, VPC_CLR_ERR_L # if (t4 >= MAX_DEFAULT_VPC)
 
    la      t6, vcr_txvcstate_s(VCR_REG)        # begining of vc state table
    addu    t6, t4                              # index to vc entry
	li		t5, VPC_INACTIVE_C
	sb		t5,	0(t6)							# vc inactive
 
    lb      t5, 0(t6)							# read it back and return
	andi	t5, 0x00ff
    sw      t5, vpcp_vcstate_s(a0)           	# store vpc state

    j   ra
 
VPC_CLR_ERR_L:
    li      v0, TXSAR_INVALID_VPC_C
    j   ra

		.end	VPC_CLEAR_L

###############################################################################



 
###############################################################################
##
##  CAMLOOKUP_WAIT_L
##		Called after a lookup has been initiated.  Returns completion code
##	in v0 and the pointer to the came entry in v1,
##		Registers used:
##			v0	
##			v1
##			t8
##			t9
##	
###############################################################################
 
        .globl  CAMLOOKUP_WAIT_L
        .ent    CAMLOOKUP_WAIT_L
CAMLOOKUP_WAIT_L:
 
	li		v0,	SAR_CMD_FAIL_C			# first assume no match

    li      t9, SAR_READ_REG0_C

1:
    lw      v1, (t9)
    andi    t8, v1, REG0_CAM_DONE_C
    bnez    t8, 1b                      # if bit set, then not done
 
										# now check if match happened
    and     t8, v1, REG0_CAM_MATCH_C
    bnez    t8, CAMLOOKUP_WAIT_ERR_L	# if bit set to 1, no match
 
    and     v1, REG0_CAM_MATCH_ADDR_C
    srl     v1, 17                      # v1 now has 10 bit index
	sll		v1,	3						# multiply by sizeof(camentry)
										
	li		t9,	TX_SAR_SPRAM_START_C	# get to 
	lw		t8, txspram_pCamTable_s(t9)	# base of cam table
 	addu	v1,	t8						# This is the pointer to the 
										# corresponding entry

	li		v0, SAR_CMD_SUCCESS_C
	j	ra

CAMLOOKUP_WAIT_ERR_L:

	j	ra

		.end    CAMLOOKUP_WAIT_L
 
###############################################################################




###############################################################################
##
##  TXSAR_NOP_L
##		Update the status and return
##		On entring this procedure: 
##			a0 contains pointer to command record
##			a1 contains command header
##			t0 contains producer pointer
##			t1 contains consumer pointer
##			t3 contains command code
##
###############################################################################
 
        .globl  TXSAR_NOP_L
        .ent    TXSAR_NOP_L
TXSAR_NOP_L:

	li		v0,	SAR_CMD_SUCCESS_C				# return value
	j		ra
 
        .end    TXSAR_NOP_L
 
###############################################################################


###############################################################################
##
##  ENQ_L:	Enque element on a requested queue
##		
##      On entry to this label:
##      a0 has pointer to queue descriptor
##      a1 has element that is to be enqueued
##      v0 has completion code 
##
##  ALGORITHM:
##
##      Put the element at the producer pointer of the requested queue
##	 	check producer pointer for wrap around
##		Update producer pointer for the queue
##
##	REGISTER USAGE:
##
##		In the sequence of events this is called, need to preserve:
##			t2, t3, t4, t5, t6
##
##		t7:  producer pointer
##		t8:	 last pointer
##		t9:	 consumer pointer
##		
##
############################################################################### .text
        .globl  ENQ_L
        .ent    ENQ_L
ENQ_L:

        lw      t7, qdesc_pProducer_s(a0)   # load producer in t7
        sw      a1, (t7)                    # place element in producer ptr
 
        #   check for wrap around
        lw      t8, qdesc_pLast_s(a0)
        bne     t7, t8, 1f					# compare producer and last ptr
        lw      t7, qdesc_pFirst_s(a0)		# if eq, set producer eq first
        b       2f
1:
        addu    t7, 4						# else increment producer ptr
2:
		#
		#	Before we update the producer pointer, check if it matches
		#	the consumer pointer.  If so, don't update the producer, 
		#	return with an error code.
		#

        lw      t9, qdesc_pConsumer_s(a0)   # load consumer in t9
		beq		t7,	t9,	ENQ_FULL_L			# if prod == cons, goto ENQ_FULL_L

        #   Else update producer pointer for response q
		sw      t7, qdesc_pProducer_s(a0)   # update producer pointer in spram

ENQ_OK_L:
		li		v0,	SAR_CMD_SUCCESS_C			
		j		ra

ENQ_FULL_L:
    	li		v0, SAR_CMD_FAIL_C
		j		ra

		.end 	ENQ_L


###############################################################################
##
##  DEQ_L:  Deq element from a requested queue
##
##      On entry to this label:
##      a0 has pointer to queue
##      v0 has dequeued element if successful, 0 otherwise
##
##  ALGORITHM:
##
##      deq from the requested queue
##
##		In the sequence of events this is called, need to preserve:
##			t2, t3, t4, t5, t6, t7
##
##		t8:  producer pointer
##		t9:	 consumer pointer
##		
##
###############################################################################

        .text
        .globl  DEQ_L
        .ent    DEQ_L
DEQ_L:

		lw	t8,	qdesc_pProducer_s(a0)		# load producer to t0
		lw	t9,	qdesc_pConsumer_s(a0)		# load consumer to t1
        beq	t8,	t9, DEQ_EMPTY_L 			# equal, queue empty

        #
        #   Else Get the element from consumer pointer in t1
        #
        lw  v0, (t9)                        

		#
		#	Update consumer pointer, wrap arround if needed
		#

    	lw  t8, qdesc_pLast_s(a0)			# check is consumer ptr == last
    	bne t9, t8, 1f
    	lw  t9, qdesc_pFirst_s(a0)			# if yes, set cons_ptr = first
    	b   2f
 
    										# Else increment Consumer Index
1:  	addu    t9, 4
 
2:  	sw  	t9, qdesc_pConsumer_s(a0)		# Update consumer ptr

    	j   	ra

DEQ_EMPTY_L:
		move	v0, zero
		j		ra

		.end	DEQ_L
###############################################################################




###############################################################################
##
##  UNLINK_BIGA_L
##
##	Unlink the biga descriptor from the doubly linked list.  The
##		biga desc is pointed to by: pBHEAD_REG.  pBHEAD_REG itself is not
##	changed.
##
##	Return address assumed to be in a2
##
##	Algorithm:
##		pBHEAD_REG->pPrev->pNext = pBHEAD_REG->pNext
##		pBHEAD_REG->pNext->pPrev = pBHEAD_REG->pPrev
##		pBHEAD_REG->pPrev = NULL
##
##	Registers used:
##		In the sequence of events this is called, need to preserve:
##			t2, t3, t4, t5, t6
##		USED:
##			t9: Previous rd
##			t8:	Next rd
##		
###############################################################################

		.text
		.globl  UNLINK_BIGA_L
        .ent    UNLINK_BIGA_L
UNLINK_BIGA_L:

	lw		t9,	bigard_pPrev_s(pBHEAD_REG)	# previous bigard
	lw		t8,	bigard_pNext_s(pBHEAD_REG)	# next bigard

	sw		t8,	bigard_pNext_s(t9)			# prev bigard's pnext field
											# points to pnext of current bigard

	sw		t9, bigard_pPrev_s(t8)			# next bigard's pPrev field points
											# to previous of current bigard

	j		a2

		.end	UNLINK_BIGA_L

###############################################################################


###############################################################################
##
##  LINK_BIGA_L
##
##	link the biga descriptor at the tail.  Read the biga registers for
##	the current tail.
##
##	This is called when an inband data indication event is processed 
##	by the the acp and the competion is given back to the tx sar
##	The biga descriptor is linked after the current tail.  The tail is
##	then updated
##
##	REGISTER USAGE:
##		a2:		return address
##		a0:		bigard to link 
##
##
###############################################################################

		.text
		.globl  LINK_BIGA_L
        .ent    LINK_BIGA_L
LINK_BIGA_L:

		#
		#	Read current tail from biga.  Substract offset to get to
		#	the descriptor.
		#

    lw      t9, BTAILOFF_C(BIGA_BASE_REG)	# t9 now has the tail bigard

		#
		#	t9 points to the pNext field of the tail descriptor.  Adjust
		#	it to get to the actual descriptor
		#

	sub		t9,	BIGARD_PNEXT_OFFSET

		#	a0 is the bigard that is to be linked in.
		#	(a1)->pNext = pTail->pNext
		#	(a0)->pPrev = pTail
		#	pTail->pNext->pPrev = a0
		#	pTail->pNext = a0
		#	pTail = a0+BIGARD_PNEXT_OFFSET
		#
	lw      t8, bigard_pNext_s(t9)  		# next bigard w.r.t pTail
	sw		t8,	bigard_pNext_s(a0)			# (a0)->pNext = pTail->pNext
	sw		t9,	bigard_pPrev_s(a0)			# (a0)->pPrev = pTail
	sw		a0,	bigard_pPrev_s(t8)			# pTail->pNext->pPrev = a0
	sw		a0,	bigard_pNext_s(t9)			#  pTail->pNext = a0

	add		a0,	BIGARD_PNEXT_OFFSET			# address of pNext field
	beq		a0,	pBHEAD_REG, 1f				# don't update tail if == 
											# current head, 
    sw      a0, BTAILOFF_C(BIGA_BASE_REG)	# update tail
	move	pBTAIL_REG, a0
1:
	j		a2

		.end	LINK_BIGA_L

###############################################################################



###############################################################################
##
##  DEQ_CMD_L
##
##		On entry to this label: 
##		t0 has producer pointer for acp command queue
##		t1 has consumer pointer for the acp command queue
##		v0 has completion code for the command
##		a0 has pointer to command
##		a1 has command header
##
##	ALGORITHM:
##		
##		update the command header with response code
##		enqueue the command buffer to the ACP response queue
##		update consumer pointer for the ACP command q
##
###############################################################################
 
 		.text
        .globl  DEQ_CMD_L
        .ent    DEQ_CMD_L
DEQ_CMD_L:
 
		or		a1,	v0						# update completion code
		sw		a1,	(a0)					# write it to commad header

		#
		#	Put command at producer pointer of txsar response queue
		#
		
		li		t4,	TX_SAR_SPRAM_START_C		
		lw		t5,	txspram_pAcpRespq_s(t4)	# Address of response q in t5
		lw		t6,	qdesc_pProducer_s(t5)	# load producer to t6

		sw		a0,	(t6)					# place response in producer ptr


        # 	check for wrap around
        lw      t7, qdesc_pLast_s(t5)
        bne     t6, t7, 1f
        lw      t6, qdesc_pFirst_s(t5)
		b		2f
1:
        addu	t6,	4
2:
		#	Update producer pointer for response q
 		sw		t6,	qdesc_pProducer_s(t5)	# update producer pointer in spram

 
		#
		# Now update the consumer pointer
        # Check wrap around case for consumer
		#
        lw      t7, qdesc_pLast_s(pACPCMDQ_REG)
        bne     t1, t7, 1f							# If consumer == last
        lw      t1, qdesc_pFirst_s(pACPCMDQ_REG)    # wrap around to first
		b		2f
1:
		#	Update consumer pointer for command queue
        addu    t1, 4								# Else increment cons_ptr
2:
		sw		t1,	qdesc_pConsumer_s(pACPCMDQ_REG)	# update consumer pointer

		j	ra

		#
		#	
		#
        .end DEQ_CMD_L
 
###############################################################################



###############################################################################
##
## 	START_LEARP_L
##		Send start_le_arp event to ACP
##			- dequeue event record from txsar event pool
##			- fill in dest macaddr, color in start_learp event
##			- enq event record on acp event indication q
##			- notify host
##
##	Called from biga service routine.  Preserve the following registers
##
##		t3: color
##      t4: lec entry
##      t5: index from bigard, also contains color
##      t6: vpc as in the index
##		t7: event code
##		v1: pointer to cam entry in sar space
##
##	REGISTER USAGE:
##		a0:	txspram_pTxToAcpEventPool_s
##		
##
###############################################################################

        .text
        .globl	START_LEARP_L
        .ent	START_LEARP_L
START_LEARP_L:
 
		#	Load address of event pool in a0

	la		a0,	TX_SAR_SPRAM_START_C
	lw		a0,	txspram_pTxToAcpEventPool_s(a0)

		#	invoke DEQ_L
	
	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal     DEQ_L                       # invoke deq
	lw		ra,	vcr_save1_regRA_s(VCR_REG)

		#	v0 has dequeued element if successful, 0 otherwise

	bne	v0,	zero, 1f
		# Couldn't get buffer so return error
    	li	v0, SAR_CMD_FAIL_C
		j	ra
1:

		#
		#	Else the event record is pointed to by v0
		#	Fill in the header, dest mac and color
		#	t7 has the event code TXSAR_START_LEARP_IND_C or 
		#	TXSAR_CLEAR_MAC_IND_C
		#

	sw		t7,	learpp_hdr_s(v0)

    # copy 12 bytes of DA and SA
    ulw     t7, bigard_da_s+vcr_bigard_s(VCR_REG)
    sw      t7, learpp_dmacaddr_s(v0)
    ulw     t7, bigard_da_s+vcr_bigard_s+4(VCR_REG)
	sw		t7,	learpp_dmacaddr_s+4(v0)
    ulw     t7, bigard_sa_s+vcr_bigard_s+2(VCR_REG)
	sw		t7,	learpp_smacaddr_s+2(v0)
										# The index is in t5, extract the
										# vlanid from it
										# Color is in t3
	sh		t3,	learpp_vlanid_16s(v0)	# this completes the mac and color
										# in the start learp event record
	sw		v1, learpp_came_s(v0)	    # store the ptr to cam entry

		#
		#	Enq the event record on the TxToAcpEventq
		#
	la		a0,	TX_SAR_SPRAM_START_C
	lw		a0,	txspram_pTxToAcpEventq_s(a0)
	move	a1,	v0						# because enq_l expects the element in 
										# a1

		#	Invoke ENQ_L
		#      a0 has pointer to queue descriptor
		#      a1 has element that is to be enqueued
		#      v0 will completion code

	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal 	ENQ_L                       # invoke enq, return addr in a2
	lw		ra,	vcr_save1_regRA_s(VCR_REG)

		#
		#	The result of the enq is in v0
		#

		#
		#	Interrupt the ACP to indicate presence of an event
		#

	sw      zero, HOSTINT_AREG

	lw		t8, vcr_txspram_pStats_s(VCR_REG)
	lw		t1, txgl_start_learp_s(t8) 
	addu	t1, 1						# txgl_start_learp_s++
	sw		t1,	txgl_start_learp_s(t8)

LEARP_ERR_EXIT_L:
        j       ra
 
        .end START_LEARP_L

###############################################################################
 


 
###############################################################################
##
##  ACP_DATAIND_L
##      Send data indication event to ACP
##          - dequeue event record from txsar event pool
##          - fill data indication event
##          - enq event record on acp event indication q
##          - notify host
##
##      t4: lec entry
##      t5: index from bigard, also contains color
##      t6: vpc as in the index
##
##  BIGA startegy:
##
##      Unlink the bigard from the biga recv list.  The event record
##  has a pointer back to the bigard.  When the event record is returned
##  by the acp, the bigard is linked back in at the tail.  The current
##  biga descritor is pointed at by : pBHEAD_REG.  This is the one that
##  will be delinked.
##
##
###############################################################################

        .text   
        .globl  ACP_DATAIND_L
        .ent	ACP_DATAIND_L
ACP_DATAIND_L:
 
        #   Load address of event pool in a0
 
    la      a0, TX_SAR_SPRAM_START_C
    lw      a0, txspram_pTxToAcpEventPool_s(a0)
 
        #   invoke DEQ_L
 
	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal     DEQ_L                       # invoke deq
	lw		ra,	vcr_save1_regRA_s(VCR_REG)
 
        #   v0 has dequeued element if successful, 0 otherwise
 
    beqz     v0, DIND_ERR_EXIT_L
 
        #
        #   Else the event record is pointed to by v0
        #   Fill in the data length and the data pointer
        #
 
 
	li      t1, TXSAR_DATA_IND_C
	sw		t1,	txdind_hdr_s(v0)		# store event code

										# pull the length out of the bigrd
    lw      t1, bigard_control_s+vcr_bigard_s(VCR_REG)    # actual length of recvd data
    and     t1, 0x7fff                  #
	sub		t1,	(SYHDR_SIZE_C)			# substract synergy header from len
	sw		t1,	txdind_len_s(v0)		# store data indication length

	lw  	t1,  bigard_pData_s+vcr_bigard_s(VCR_REG)     # bigard in vcr
                                        # t3 points to the synergy hdr
    add 	t1, (SYHDR_SIZE_C)        	# this is where data starts
										# data format is: sa,da,user_data
 
	sw		t1,	txdind_pbuf_s(v0)		# store data pointer
										# store pointer back to bigard
	sw		pBHEAD_REG,	txdind_pDesc_s(v0)	

DIND_HOSTINT_L:
		#
		#	Enq event record,pointed to by v0 on the txsar->acp event Q
		#
    la      a0, TX_SAR_SPRAM_START_C
    lw      a0, txspram_pTxToAcpEventq_s(a0)
	move	a1,	v0						# because enq_l expects the element in 
										# a1
 
        #   Invoke ENQ_L
        #      a0 has pointer to queue descriptor
        #      a1 has element that is to be enqueued
        #      v0 will completion code
 
	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal     ENQ_L                       # invoke enq,
	lw		ra,	vcr_save1_regRA_s(VCR_REG)
 
        #
        #   The result of the enq is in v0.  If Enq fails, free the
		#	event record back to the event pool and return
        #

 	beq		v0,	SAR_CMD_SUCCESS_C, 1f	# if success, signal acp

    la      a0, TX_SAR_SPRAM_START_C	# else free event record, which is in a1
    lw      a0, txspram_pTxToAcpEventPool_s(a0)
	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal     ENQ_L                       # invoke enq,
	lw		ra,	vcr_save1_regRA_s(VCR_REG)
	b		DIND_ERR_EXIT_L				# return error

1:
        #
        #   Interrupt the ACP to indicate presence of an event
        #
	sw      zero, HOSTINT_AREG

		#
		#	Unlink the bigard, pointed to by pBHEAD_REG
		#

    la      t9, UNLINK_BIGA_L		# load addrss of support function in t9
    jal     a2, t9					# invoke unlink, return addr in a2

	lw		t8, vcr_txspram_pStats_s(VCR_REG)
	lw		t1, txgl_packet_acp_s(t8) 
	addu	t1, 1						# txgl_packet_acp_s++
	sw		t1,	txgl_packet_acp_s(t8)

	li		v0,	SAR_CMD_SUCCESS_C
	j		ra

DIND_ERR_EXIT_L:

	li		v0,	SAR_CMD_FAIL_C			# failed
	j       ra
 
        .end ACP_DATAIND_L

###############################################################################
 



 
###############################################################################
##
##  EVENT_RESP_L
##		Service event responses from acp
##          - dequeue event record from txsar event resp q
##			- if data_ind response, update biga pointers
##			- link bigard back in the recv list
##          - return event record txsar event pool
##
###############################################################################

        .text   
        .globl  EVENT_RESP_L
        .ent	EVENT_RESP_L
EVENT_RESP_L:
 
        #   Load address of event respq in a0
 
1:
    la      a0, TX_SAR_SPRAM_START_C
    lw      a0, txspram_pAcpToTxEventRespq_s(a0)
 
        #   invoke DEQ_L
 
	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal     DEQ_L                       # invoke deq
	lw		ra,	vcr_save1_regRA_s(VCR_REG)
 
        #   v0 has dequeued element if successful, 0 otherwise
 
    beqz     v0, EVENT_RESP_ERR_L

        #	element is header + event parameters, 
		#   If event == tx data ind Link the biga descriptor at the tail
        #   else return event to event pool

    lw      a0, cmd_hdr_s(v0)			# load event header
	andi	a0,	CMDCODE_MASK_C			# check event code
	bne		a0,	TXSAR_DATA_IND_C, EVENT_RETURN_L
 
    lw      a0, txdind_pDesc_s(v0)      # bigard to link back
    la      t9, LINK_BIGA_L             # load addrss of support function in t9
    jal     a2, t9                      # invoke link, return addr in a2

		#
		#	Return the element back to the event pool, element is in v0
		#
EVENT_RETURN_L:
    la      a0, TX_SAR_SPRAM_START_C	# load pool address in a0
    lw      a0, txspram_pTxToAcpEventPool_s(a0)
	move	a1,	v0						# because enq wants element in a1
	sw		ra,	vcr_save1_regRA_s(VCR_REG)
    bal     ENQ_L                       # invoke enq,
	lw		ra,	vcr_save1_regRA_s(VCR_REG)
										# result is in v0
	b		1b							# service exhaustively

EVENT_RESP_ERR_L:						# no response from ACP

	j		ra
 
        .end EVENT_RESP_L
###############################################################################
 



###############################################################################
##
##  TXSAR_CRASH_L
##
###############################################################################
	.text	
    .globl TXSAR_CRASH_L
    .ent TXSAR_CRASH_L
TXSAR_CRASH_L:     

1:
        sw      zero, HOSTINT_AREG		# Notify Host
	b	1b

    .end TXSAR_CRASH_L


 
################################################################################
#
#
# THIS FUNCTION HAS TO COME AFTER THE LAST FUNCTION TO BE INCLUDED IN IRAM
#
# iramSize_L()
# return the size (in words) of the code to be copied to
# iram.
#
################################################################################
 
    .globl iramSize_L
    .ent iramSize_L
iramSize_L:
    la      v0,iramSize_L
    la  t0, txsar_work
    subu    v0,t0
    srl v0,2
    j   ra
    .end iramSize_L
 
 
 
###############################################################################



##############################################################################
##
##						C A M    C O M M A N D S
##
##	INIT, SET, CLEAR, GET
##
##	Support primitives: 
##		command_read, command_write, data_read, data_write
##
##############################################################################



###############################################################################
##
##	CAM_CDRW_L
##		command/data read/write primitive
##
##		Don't use a0, a1, t0, t1, t3
##		
##		On entry:
##			t2 has return address
##			a2 indicates command or data operation, 1: command, 0: data
##			a3 has requested operation 1: read, 0 write
##			if op == command write, 
##				v1 has 16 bit value to be written 
##			if op == data write
##			
##		On exit:
##			v1 has 16 bit value if a read operation was requested.
##
##		NOTE:  registers used t4, t5.  Not saved.  
##
###############################################################################

        .globl  CAM_CDRW_L
        .ent    CAM_CDRW_L
CAM_CDRW_L:

		li		t4, SAR_CAM_PLMODE_REG_C	# processor or lookup mode
		li		t5, 0				# set bit 0 of SAR_CAM_PLMODE_REG to 0
		sw		t5, (t4)			# i.e. in processor mode

		li		t4, SAR_CAM_DCMODE_REG_C	# data or command mode

		beqz	a2, CAM_D_L			# opcode = 0, data mode

CAM_C_L:							# else command mode
		li		t5,	1				# set bit 0 of SAR_CAM_DCMODE_REG to 1
		sw		t5, (t4)
		j		CAM_RW_L			# make read/write choice

CAM_D_L:							# put cam in data mode 
		li		t5,	0				# set bit 0 of SAR_CAM_DCMODE_REG to 0
		sw		t5, (t4)
		

CAM_RW_L:							# make read wite descision
		li		t4,	SAR_CAM_RWMODE_REG_C
		
		beqz	a3, CAM_W_L			# opcode = 0, requested write

CAM_R_L:							# else a command read operation
		li     	t5, 1      			# set bit 0 of SAR_CAM_RWMODE_REG to 1
		sw		t5,	(t4)

		li		t4,	SAR_WRITE_REG0_C
		sw		t5,	(t4)			# dumy write to WRITE_REG0 starts operation

		li		t4, SAR_READ_REG0_C
1:									# Loop to check cam completion
		lw		t5, (t4)
		andi	t5, CAM_IN_OPERATION_C
		bnez	t5,	1b

		lw		v1, (t4)
		and		v1,	v1, 0x7fff0000	# bits 16:30 contain the result
		srl		v1, 16				# v1 now has the result 
		j		CAM_CDRW_EXIT_L		# return 

CAM_W_L:							# Command write operation
		li    	t5, 0      			# set bit 0 of SAR_CAM_RWMODE_REG to 0
		sw		t5,	(t4)

		li		t4,	SAR_WRITE_REG0_C
		##
		sw		v1,	(t4)			# write 16 bit cmd in v1 to SAR_WRITE_REG0
		##

		li		t4, SAR_READ_REG0_C
1:									# Loop to check completion
		lw		t5, (t4)
		andi	t5, CAM_IN_OPERATION_C
		bnez	t5,	1b


CAM_CDRW_EXIT_L:
		#
		# return address is in t2, result of a read in v1
		#
		j		t2
 
        .end    CAM_CDRW_L
 
###############################################################################

		
###############################################################################
##
##  CAM_INIT_L
##
##      On entring this procedure:
##          a0 contains pointer to command record
##          a1 contains command header
##          t0 contains producer pointer
##          t1 contains consumer pointer
##          t3 conaints command code
##
##		This routine uses the CAM_CDRW_L  primitive to do command write/
##		sequence as specified in the doc: "CAM Access Operation"
##	
##	The sequence is labled C1, C2 etc. in this procedure
##
##	Invoke CAM_CDRW_L with:
##          t2 has return address
##          a2 indicates command or data operation, 1: command, 0: data
##          a3 has requested operation 1: read, 0 write
##          v1 has 16 bit value to be written if requested op = write
##
###############################################################################
 
        .globl  CAM_INIT_L
        .ent    CAM_INIT_L
CAM_INIT_L:
 
	li	v0,	SAR_CMD_SUCCESS_C
	la	t9,	CAM_CDRW_L					# load address of support function in t9

	li	a2,	CAM_COMMAND_REQ_C			# request a command operation
	li	a3,	CAM_WRITE_REQ_C				# request a command write

										# In the following sequnece, v1 has
										# the command value to be written
C1_L:
	li	v1,	CAM_TCO_DSREG_C				# Select device select register
	jal	t2,	t9							# return address in t2
	
C2_L:
	li	v1,	CAM_SELECT_ALLCAMS_C		# Select all cams
	jal	t2,	t9							# return address in t2
	
C3_L:
	li	v1,	CAM_TCO_CREG_C				# Select control register
	jal	t2,	t9							# return address in t2

C4_L:
	li	v1,	CAM_RESET_ALLCAMS_C			# Reset all selected cams
	jal	t2,	t9							# return address in t2

SET_ALLPAGES_L:
	li	t8,	CAM_PAGE0_C
	li	t7,	CAM_PAGE3_C

C5_L:
	li	v1,	CAM_TCO_PAREG_C				# Select page address register
	jal	t2,	t9							# return address in t2

C6_L:
	move v1,	t8						# Set fist device page numbers:0,1,2,3
	jal	t2,	t9							# return address in t2

C7_L:
	li	v1,	CAM_SET_FULLF_C				# Set full flag
	jal	t2,	t9							# return address in t2

	addi	t8,	1
	ble		t8, t7, C5_L				# repeat for all pages

C16_L:
	li	v1,	CAM_TCO_DSREG_C				# select device select register
	jal	t2,	t9							# return address in t2

C17_L:
	li	v1,	CAM_SELECT_ALLCAMS_C		# select all devices
	jal	t2,	t9							# return address in t2

C18_L:
	li	v1,	CAM_TCO_CREG_C				# select control register
	jal	t2,	t9							# return address in t2

C19_L:
	li	v1,	CAM_RESET_ALLCAMS_C			# select all devices
	jal	t2,	t9							# return address in t2


READ_ALL_PAGES_L:
	li	t8,	CAM_PAGE0_C
	li	t7,	CAM_PAGE3_C

C20_L:
	li	a3,	CAM_WRITE_REQ_C				# request a command write
	li	v1,	CAM_TCO_DSREG_C				# select device select register
	jal	t2,	t9							# return address in t2

C21_L:
	move v1, t8							# set device number: 0,1,2,3
	jal	t2,	t9							# return address in t2

C22_L:
	li	v1,	CAM_TCO_PAREG_C				# Select page address register
	jal	t2,	t9							# return address in t2

C23_L:
	li	a3,	CAM_READ_REQ_C				# request a command READ
	jal	t2,	t9							# return address in t2
	bne v1, t8, C36_L					# ** ***
#	bne v1, t8, CAM_INIT_ERR_L			# v1 has the result of the read
										# it should match page number

	addi	t8,	1
	ble		t8, t7, C20_L				# read all pages addr registers

C36_L:
	li	a3,	CAM_WRITE_REQ_C				# request a command write
	li	v1,	CAM_TCO_DSREG_C				# select device select register
	jal	t2,	t9							# return address in t2

C37_L:
	li	v1,	CAM_SELECT_ALLCAMS_C		# select all devices
	jal	t2,	t9							# return address in t2

C38_L:
	li	v1,	CAM_TCO_CREG_C				# select control register
	jal	t2,	t9							# return address in t2

C39_L:
	li	v1,	CAM_CONTROL_C				# No reset, enable match flag,
										# enable full flag, use all segments
										# as CAM banks, disable mask, register
										# enable address increment
	jal	t2,	t9							# return address in t2

C40_L:
	li	v1,	CAM_SPD_CR_C				# Select persistant destination,
										# use comparand register
	jal	t2,	t9							# return address in t2


C41_L:
	li	v1,	CAM_TCO_SC_C				# Select segment control register
	jal	t2,	t9							# return address in t2


	#
	#	C42 and C43 are exit procedures to leave cam in lookup mode
	#

C42_L:
	li	v1,	CAM_SC_VALUE_C				# Segment control: set dest ctr limit:
										# start 00, end 11;
										# set src counter limit: 
										# start 00, end 11;
										# load src and dest ctr with 0
	jal	t2,	t9							# return address in t2


C43_L:									# Put cam in lookup mode
	li	a3,	SAR_CAM_PLMODE_REG_C		# Processor / lookup register
	li	t8,	0x00000001
	sw	t8,	(a3)						# Set bit 0 to 1 for normal lookup 


CAM_INIT_OKEXIT_L:

	j	ra


CAM_INIT_ERR_L:
    li	v0, SAR_CMD_FAIL_C
	j	ra

        .end    CAM_INIT_L
 
###############################################################################




 
###############################################################################
##
##  CAM_SET_L
##
##      On entring this procedure:
##          a0 contains pointer to command record
##          a1 contains command header
##          t0 contains producer pointer
##          t1 contains consumer pointer
##          t3 conaints command code
##
##	CAM is addresses by the <mac address (48 bits), vlan index (16 bits)> 
##	The value stored is an index to the cam table
##	A cam table entry has the cam state, pacing counter and the holding queue
##	id.
##
##	The command structure is:
##			camp_hdr_s:         .word 0
##			camp_macaddr_s:     .space 6
##			camp_vlanid_16s:    .half 0
##			camp_state_16s:     .half 0
##			camp_index_16s:     .half 0
##			
##	The mac addr and vlanid form the cam address.  
##	The cam update procedure is:
##	(taken from filter_cam.[ch] )
##	
##		Select segment control register
##		write segement control value
##		set persistant destination
##		write data mac_addr_16lo
##		write data mac_addr_16mid
##		write data mac_addr_16hi
##		write data vlanid_16
##
##	
##		write data camindex_16 
##
##	Registers used: a2, a3, v0, v1, t2, t9.
##	The support functions use t4, t5
##
###############################################################################
 
        .globl  CAM_SET_L
        .ent    CAM_SET_L
CAM_SET_L:
 
	li	v0,	SAR_CMD_SUCCESS_C			

	la	t9,	CAM_CDRW_L					# load address of support function in t9

	li	a2,	CAM_COMMAND_REQ_C			# request a command operation
	li	a3,	CAM_WRITE_REQ_C				# request a command write
	li	v1,	CAM_TCO_SC_C				# Select segment control register
	jal	t2,	t9							# return address in t2

	li	v1,	CAM_SC_VALUE2_C				# write SC value
	jal	t2,	t9							# return address in t2

	#
	#	Select device 0
	#
	li	v1,	CAM_TCO_DSREG_C				# select device select register
	jal	t2,	t9							# return address in t2

	li	v1,	CAM_PAGE0_C					# select cam 0
	jal	t2,	t9							# return address in t2


	#
	# Now do 4 16-bit data writes to load cam.  By default, the dest of
	# a data write is the comparand register.  Change this by doing a
	# set persistant dest to the next free in the memory array.
	#
	li	v1,	CAM_SPD_MNF_C				# write persistant dest command
	jal	t2,	t9							# return address in t2

	li	a2,	CAM_DATA_REQ_C				# request a data operation
	li	a3,	CAM_WRITE_REQ_C				# request a write operation
mac_addr_L:

	la	t8, camp_vlanid_16s(a0)			# load address of vlan id field

	lh	v1,	(t8)						# load vlan id
	jal	t2,	t9							# do cam data write, return addr in t2

	sub t8, 2	
	lh 	v1, (t8)						# load upper 16 bits of mac address
	jal	t2,	t9							# do cam data write, return addr in t2

	sub t8, 2	
	lh	v1, (t8)						# load mid 16 bits of mac address
	jal	t2,	t9							# do cam data write, return addr in t2

	sub t8, 2	
	lh  v1, (t8)						# load lower 16 bits of mac address
	jal	t2,	t9							# do cam data write, return addr in t2

	#
	#	Now retrieve the location where the cam placed this entry.  This
	#	is done by writing the <mac address, vlanid> to the 
	#	comparend register.  Reading the status register then provides the 
	#	location where the match occured.
	#
	li	a2,	CAM_COMMAND_REQ_C			# request a command operation
	li	a3,	CAM_WRITE_REQ_C				# request a command write
	li	v1,	CAM_SPD_CMP_C				# write persistant dest command
	jal	t2,	t9							# return address in t2

	li	a2,	CAM_DATA_REQ_C				# request a data operation
	li	a3,	CAM_WRITE_REQ_C				# request a write operation

    la  t8, camp_vlanid_16s(a0)         # load address of vlan id field
 
    lh  v1, (t8)                        # load vlan id
    jal t2, t9                          # do cam data write, return addr in t2
 
    sub t8, 2
    lh  v1, (t8)                        # load upper 16 bits of mac address
    jal t2, t9                          # do cam data write, return addr in t2
 
    sub t8, 2
    lh  v1, (t8)                        # load mid 16 bits of mac address
    jal t2, t9                          # do cam data write, return addr in t2
 
    sub t8, 2
    lh  v1, (t8)                        # load lower 16 bits of mac address
    jal t2, t9                          # do cam data write, return addr in t2


		#
		#	A match should have happened, read the status register.  The
		#	default source of a command read is the status register.
		#
	
	li	a2,	CAM_COMMAND_REQ_C			# request a command operation
	li	a3,	CAM_READ_REQ_C				# request a read operation
	jal	t2,	t9							# return address in t2

		#
		# put the cam back in lookup mode
		#
	li	a3,	SAR_CAM_PLMODE_REG_C		# Processor / lookup register
	li	t8,	0x00000001
	sw	t8,	(a3)						# Set bit 0 to 1 for normal lookup 

		#
		#	v1 contains the result.  Bit0 is the match flag. Bit10-Bit1 is
		#	the 10 bit index of the entry.
		#
	
	sh	 v1, camp_index_16s(a0)			# store temporaryly

	andi v1, 0x01						# check match bit, which should
										# be 0 if match happens
	bnez v1, CAM_SET_ERR_L				# error if set
										# else fix the result
	lh	 v1, camp_index_16s(a0)			
	srl	 v1, 1							# shift right by 1 
	andi v1, 0x03ff						# only 10 bits

	sh  v1, camp_index_16s(a0)			# put the result in the response
		
CAM_SET_OK_L:
	j	ra

CAM_SET_ERR_L:
    li	v0, SAR_CMD_FAIL_C
	j	ra

        .end    CAM_SET_L
 
###############################################################################
 
 
###############################################################################
##
##  CAM_GET_OR_CHANGE_L
##      On entring this procedure:
##          a0 contains pointer to command record
##          a1 contains command header
##          t0 contains producer pointer
##          t1 contains consumer pointer
##          t3 conaints command code
##
##  CAM is addresses by the <mac address (48 bits), vlan index (10 bits)>
##
##  The command structure is:
##          camp_hdr_s:         .word 0
##          camp_macaddr_s:     .space 6
##          camp_vlanid_16s:    .half 0
##          camp_state_16s:     .half 0
##          camp_index_16s:     .half 0
##
##	Write lower 32 bits of mac addr to SAR_WRITE_REG1_C
##	Write upper 16 bit mac and 16 bit color to SAR_WRITE_REG0_C
##
##	Poll SAR_READ_REG0_C for CAM operation done (bit 2 == 0 if done)
##	If match bit (bit 0 == 0 if match)
##		Then bits 17-26 have cam match address (index)
##
##	Else no match
##
###############################################################################
 
        .globl  CAM_GET_OR_CHANGE_L
        .ent    CAM_GET_OR_CHANGE_L
CAM_GET_OR_CHANGE_L:
 
	li		v0,	SAR_CMD_SUCCESS_C			

# First thing, make sure this entry exists in the CAM
    # start CAM lookup
	li		t9, SAR_WRITE_REG1_C
	lw		t7,	camp_macaddr_s(a0)		# load lower 32 bits of mac address
	sw		t7, (t9)					# write to write_reg1

	li		t9, SAR_WRITE_REG0_C
	lw		t7,	camp_macaddr_s+4(a0)	# 16 bit vlanid
	sw		t7, (t9)					# write to write_reg0
										# this starts the cam lookup
	
	li		t9, SAR_READ_REG0_C
1:
	lw		t7, (t9)
	andi	t8, t7, REG0_CAM_DONE_C
	bnez	t8, 1b						# if bit set, then not done

    # now check if match happened
	and		t8, t7, REG0_CAM_MATCH_C		
	beq	    t8, zero, 1f				# if bit set to 1, no match
		li	v0, SAR_CMD_FAIL_C
		j	ra

1:

    # Now get the CAM Table Entry

	and		t7,	REG0_CAM_MATCH_ADDR_C
	srl		t7, 17						# t7 now has 10 bit index
	sh  	t7, camp_index_16s(a0)		# put the result in the response

    # get value of CAM state
    li		t8, TX_SAR_SPRAM_START_C
    lw		t9, txspram_pCamTable_s(t8) # base of CAM Table
    sll     t7, 3                       # multiply by sizeof camentry
    addu    t7, t9                      # pointer to cam table entry

    bne t3, TXSAR_GET_CAM_C, 1f
       # If command is TXSAR_GET_CAM_C
       lbu	t9, came_macstate_8s(t7)        # get CAM entry state
       sb  	t9, camp_state_8s(a0)           # store it in cmd block
       j	ra

1:
    # If command is TXSAR_CHANGE_CAM_STATE_C
    lbu	t9, camp_state_8s(a0)           # Get state from parameter block
    sb	t9, came_macstate_8s(t7)        # store it in CAM entry state

    lhu t9, camp_dd_vpc_16s(a0)			
	sh	t9,	came_vpc_16s(t7)
    j	ra


        .end    CAM_GET_OR_CHANGE_L
 
###############################################################################
 


###############################################################################
##
##	CAM_CLEAR_L
##      On entring this procedure:
##          a0 contains pointer to command record
##          a1 contains command header
##          t0 contains producer pointer
##          t1 contains consumer pointer
##          t3 conaints command code
##
##  CAM is addresses by the <mac address (48 bits), vlan id (16 bits)>
##
##  The command structure is:
##          camp_hdr_s:         .word 0
##          camp_macaddr_s:     .space 6
##          camp_vlanid_16s:    .half 0
##          camp_state_16s:     .half 0
##          camp_index_16s:     .half 0
##
##	Set persistent destination to comparand register
##	write macaddr+lanid
##	Write Compare command
##	Write validity bit control to empty
##
###############################################################################
 
        .globl  CAM_CLEAR_L
        .ent    CAM_CLEAR_L
CAM_CLEAR_L:

	li	v0,	SAR_CMD_SUCCESS_C			

# First thing, make sure this entry exists in the CAM
	# start CAM lookup
	li	a2, SAR_WRITE_REG1_C
	lw	a3, camp_macaddr_s(a0)			# 4 bytes of MAC addr
	sw	a3, (a2)

	li	a2, SAR_WRITE_REG0_C
	lw	a3, camp_macaddr_s+4(a0)		# 2 bytes MAC addr and 2 bytes VLAN ID
	sw	a3, (a2)						# This starts CAM lookup

	# Now wait for this lookup to complete
	li	a2, SAR_READ_REG0_C

1:
	lw	v1, (a2)
	andi a3, v1, REG0_CAM_DONE_C
	bnez a3, 1b							# if bit set, then not done

	# now check if match happened
	and a3, v1, REG0_CAM_MATCH_C
	beq a3, zero, 1f					# if bit set to 1, no match
    	li	v0, SAR_CMD_FAIL_C
		j	ra

1:
    # Set state of cam entry to inactive

    and     v1, REG0_CAM_MATCH_ADDR_C
####srl     v1, 17                      # v1 now has 10 bit index
####sll     v1, 3                       # multiply by sizeof(camentry)
    srl     v1, 14                      # do both of the above at once

    li      a3, TX_SAR_SPRAM_START_C    # get to
    lw      a2, txspram_pCamTable_s(a3) # base of cam table
    addu    v1, a2                      # This is the pointer to the
                                        # corresponding entry

    li      a2, CAME_FREE_C
    sb		a2, came_macstate_8s(v1)    # update State of CAM Entry



	la	t9,	CAM_CDRW_L					# load addrss of support function in t9


    #
    #   Issue validity control command
    #
    li  a2, CAM_COMMAND_REQ_C           # request a command operation
    li  a3, CAM_WRITE_REQ_C             # request a command write operation
    li  v1, CAM_VBC_HM_E                # set highest priority match empty
    jal t2, t9                          # return address in t2

 
    li  a2, CAM_DATA_REQ_C              # request a data operation
    li  a3, CAM_WRITE_REQ_C             # request a write operation
 
    la  t8, camp_vlanid_16s(a0)         # load address of vlan id field
 
    lh  v1, (t8)                        # load vlan id
    jal t2, t9                          # do cam data write, return addr in t2
 
    sub t8, 2
    lh  v1, (t8)                        # load upper 16 bits of mac address
    jal t2, t9                          # do cam data write, return addr in t2
 
    sub t8, 2
    lh  v1, (t8)                        # load mid 16 bits of mac address
    jal t2, t9                          # do cam data write, return addr in t2
 
    sub t8, 2
    lh  v1, (t8)                        # load lower 16 bits of mac address
    jal t2, t9                          # do cam data write, return addr in t2
 
CAM_CLEAR_OK_L:
#    lh   v1, camp_index_16s(a0)			# return the index
#    srl  v1, 1                          # shift right by 1
#    andi v1, 0x03ff                     # only 10 bits
#    sh  v1, camp_index_16s(a0)          # put the result in the response

	j	CAM_CLEAR_EXIT_L

CAM_CLEAR_ERR_L:
    li		v0, SAR_CMD_FAIL_C

CAM_CLEAR_EXIT_L:
    	#
    	#   Set SPD comparand register
    	#
    li  a2, CAM_COMMAND_REQ_C           # request a command operation
    li  a3, CAM_WRITE_REQ_C             # request a command write
    li  v1, CAM_SPD_CMP_C               # write persistant dest command
    jal t2, t9                          # return address in t2

        #
        # put the cam back in lookup mode
        #
    li  a3, SAR_CAM_PLMODE_REG_C        # Processor / lookup register
    li  t8, 0x00000001
    sw  t8, (a3)                        # Set bit 0 to 1 for normal lookup

	j		ra


		.end	CAM_CLEAR_L


###############################################################################


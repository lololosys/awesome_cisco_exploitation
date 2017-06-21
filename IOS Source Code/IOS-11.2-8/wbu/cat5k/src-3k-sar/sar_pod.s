/* $Id: sar_pod.s,v 1.1.4.1 1996/05/09 15:01:38 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/sar_pod.s,v $
 *-----------------------------------------------------------------
 * sar_pod.s --  SAR powerup diagnostic code
 *
 * January 1995, rbau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sar_pod.s,v $
 * Revision 1.1.4.1  1996/05/09  15:01:38  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:34  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:45  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  04:06:15  vkrishna
 * Changes to support SAR firmware build as part of Cat5K ATM Module
 * IOS build procedure.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:38  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
| Revision 1.1  1996/03/11  07:44:39  rlowe
| Placeholders for California.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

/* sarpod.s - SAR powerup diagnostic code 
 *   
 *  01/28/95   rlb
 *  - created for SAR diag based on PMON startup code
 *  02/09/95   rlb
 *  - created for power-up memory diagnostic in IRAM, test SP-ram, frame
 *    buffer
 *  02/20/95   rlb
 *  - included the DMA test 
 */

#include <mips.h>
#include "sar_hw.h"
#include "errors.h"
#include "dma_test.h"

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
reset_exception:
        /* ATMizer must start with a full 32-bit jump */
	la	k0,_start
	j	k0

	.align  8
/*-------------------------------------------------------------------*
 * utlb_miss_execption()                                          
 *-------------------------------------------------------------------*/
utlb_miss_exception:
	la	k0,_exception
	li	k1,K1BASE
	or	k0,k1
	j	k0

	.align 7
/*-------------------------------------------------------------------*
 * general_exception()                                          
 *-------------------------------------------------------------------*/
general_exception:
	la	k0,_exception
	li	k1,K1BASE
	or	k0,k1
	j	k0

	.align  9
/*-------------------------------------------------------------------*
 *  test_results:
 *	table of entry address used by clients to access PMON's
 *	internal routines. It's the main reason that PMON must
 *	be built with "-G 0" (no gp addressing).
 *	This table starts at PROM base + 0x200
 *-------------------------------------------------------------------*/
	.globl test_results
test_results:
_sys_fail_address:
        .word   0
_sys_data_expected:
        .word   0
_sys_data_read:
        .word   0
_sar_number:
        .word   0


#define STKSIZE (4 * 1024)      /* reserved 4K for stack */
	.comm	stack,STKSIZE	/* PMON's own stack */

/*-------------------------------------------------------------------*
 *  _start:
 *    This is the entry point of the entire PROM Monitor
 *-------------------------------------------------------------------*/
	.globl pod_ok
	.globl _start
	.ent _start
_start:
	# force kSeg1 in case control is passed here from Kseg0
	la	t0,1f
	or	t0,K1BASE
	j	t0
1:

	# set SR and CAUSE to something sensible
	li	v0,SR_BEV
	.set noreorder
	.set noat
	mtc0	v0,C0_SR
	mtc0	zero,C0_CAUSE
	.set at
	.set reorder

BIUCFG = (BIU_NOSTR|BIU_BGNT|BIU_NOPAD|BIU_INTP|BIU_IS1|BIU_DS|BIU_DBLKSZ4)
	li	t0,BIUCFG
	sw      t0,M_BIU
SCRCFG = (SCR_RO52|SCR_TO52|SCR_HH0|SCR_HH1|SCR_HH2|SCR_CBS4|SCR_BS32)
	li      t0,SCRCFG
	sw      t0,M_SCR


	/********************************************
	 *  all processor specific setup has been done
         *********************************************/
setUpDone:
        /* dummy read to clear intr on ATM line card */
        li      a0,0xA0E80060
        lw      t0,0x0(a0)

        /* turn off diag led(code entry ) */
	li	t0,0xff
	li	a0,IOBASE
	sw	t0,LED1_OFFSET(a0)
	sw	t0,LED2_OFFSET(a0)
	sw	t0,LED3_OFFSET(a0)
	sw	t0,LED4_OFFSET(a0)

        /* visual delay for LEDs */
        li      t0,0x30000
1:      subu    t0,1
        bne     zero,t0,1b

/*************************************************************IRAM***/

        li      t0, BIU_IRAM_WR
        la      t2, 0xfffe0130         /* BIU/Cache Config Register */
        sw      t0, (t2)               /* write to Config Reg */

        /******************************************************
         * program to copy data from the SP to the IRAM.
         ******************************************************/
        li      v0,0x00010000           /* bit 16=1->isolate cache */
        la      t0, pod_ok
        la      t3, SP_start          /* starting address */

        subu    a2, t0, t3             /* no. of bytes to write */
        srl	a2, a2, 2              /* no. of words to write */
	blt     a2,IRAM_SIZE,cp2iram
	nop

	j	IRAM_err_handler
        nop

        .set    noreorder
        .set    noat
cp2iram:
        la      t4, 0x0                /* starting address in IRAM */
loop:
        lw      v1, (t3)               /* fetch word from source */
        addi    t3,0x04                /* inc. src address */
        mtc0    v0,$12                  /* write to CP0, enable isolate ..
                                         * .. cache
                                         */
        sub     a2, 1                   /* dec. word count */
        sw      v1, (t4)
        mtc0    zero,$12                /* write to CP0, disable isolate..
                                         * .. cacahe
                                         */
        bne     a2,0,loop               /* fetch next word */
        addi    t4, 0x4                 /* inc. IRAM address */

        nop
        nop
        nop
        j       zero
        nop

	.set at
	.set reorder

/********************************************************************
 * start of the memory diag to be copied to IRAM 
 ********************************************************************/
SP_start:
        lui     a0,RAM_START_HI    /* start address in a0  */
        lui     a1,RAM_END_HI      /* end address in a1    */
        ori     a1,a1,0xFFFC

_RAM_memory_test:
        /* Perform Data Bus Test */
	li	t7,test_code1	   /* SP RAM data bus test */
        or      a2,a0,zero    
	lui     v0,0xFFFF
        ori     v0,v0,0xFFFF       
        or      v1,zero,zero
        sw      v1,0(a2)           /* write 0x0000.0000 */
        sw      v0,4(a2)           /* write 0xFFFF.FFFF */
        lw      a3,0(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop
        or      v0,zero,zero
        lui     v1,0xFFFF
        ori     v1,v1,0xFFFF
        sw      v1,0(a2)           /* write 0xFFFF.FFFF */
        sw      v0,4(a2)           /* write 0x0000.0000 */
        lw      a3,0(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        ori     v1,zero,0x0001
_RAM_dbus_loop:                   /* walk 1 test */
        sw      v1,0(a2)
        lw      a3,0(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop
        sll     v1,v1,1
        bne     v1,zero,_RAM_dbus_loop
        nop

        /* Perform Address Bus Test */
	li	t7,test_code2	 	/* SP RAM address bus test */
        or      t0,a0,zero              /* starts from first RAM location */
        ori     t4,zero,0x2             /* address offset starts at 0x2  */
        li      t5,SP_RAM_SIZE          /* max size of RAM 0x0002.0000 */
        lui     v0,0xAAAA
        ori     v0,v0,0xAAAA		/* V0=0xAAAA.AAAA */
        lui     v1,0x5555
        ori     v1,v1,0x5555		/* V0=0x5555.5555 */

_RAM_abus_loop1:
        ori     t3,zero,0x2
        sw      v1,0(t0)                /* write 0x5555.5555 */
        or      a2,a0,zero              /* a2 now holds test address */
_RAM_abus_loop2:
        beq     a2,t0,_RAM_abus_skip   
        nop
        sw      v0,0(a2)                /* write 0xAAAA.AAAA to current ..
					 .. address*/
_RAM_abus_skip:
        lw      a3,0(t0)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        sll     t3,t3,1
        or      a2,a0,t3		/* incremnet by 4 */
        bne     t3,t5,_RAM_abus_loop2   /* offset reaches the end of RAM */
        nop
        sll     t4,t4,1                 /* increment offset by 4 */
        or      t0,a0,t4                /* t0 holds the test address */
        bne     t4,t5,_RAM_abus_loop1
        nop

        /************************************
         * Perform Byte/Word Enable tests   
         ************************************/
        /* byte access */
        or      a2,a0,zero
        ori     v1,zero,0x12
        sb      v1,0(a2)
        lb      a3,0(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x34
        sb      v1,1(a2)
        lb      a3,1(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x56
        sb      v1,2(a2)
        lb      a3,2(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x78
        sb      v1,3(a2)
        lb      a3,3(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail

        /* 32-bit access */
        lui     v1,0x1234
        ori     v1,v1,0x5678

        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail

        /* 16-bit access */
        ori     v1,zero,0x8765
        sh      v1,0(a2)
        lh      a3,0(a2)
        nop
        andi    a3,a3,0xFFFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x4321
        sh      v1,2(a2)
        lh      a3,2(a2)
        nop
        andi    a3,a3,0xFFFF
        bne     v1,a3,_RAM_memory_test_fail
        lui     v1,0x8765
        ori     v1,v1,0x4321
        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail

        or      a2,a0,zero
        lui     v0,0xFFFF
        ori     v0,v0,0xFFFF
        lui     v1,0x5555
        ori     v1,v1,0x5555
_RAM_memory_test_wloop_55AA:
        sw      v1,(a2)
        addiu   a2,a2,4
        bne     a2,a1,_RAM_memory_test_wloop_55AA
        xor     v1,v1,v0
        sw      v1,(a2)

        or      a2,a0,zero
        lui     v1,0x5555
        ori     v1,v1,0x5555
_RAM_memory_test_cloop_55AA:
        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop
        addiu   a2,a2,4
        bne     a2,a1,_RAM_memory_test_cloop_55AA
        xor     v1,v1,v0
        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        or      a2,a0,zero
        lui     v1,0xAAAA
        ori     v1,v1,0xAAAA
_RAM_memory_test_wloop_AA55:
        sw      v1,(a2)
        addiu   a2,a2,4
        bne     a2,a1,_RAM_memory_test_wloop_AA55
        xor     v1,v1,v0
        sw      v1,(a2)

        or      a2,a0,zero
        lui     v1,0xAAAA
        ori     v1,v1,0xAAAA
_RAM_memory_test_cloop_AA55:
        lw      a3,(a2)
        sw      zero,(a2)
        bne     v1,a3,_RAM_memory_test_fail
        nop
        addiu   a2,a2,4
        bne     a2,a1,_RAM_memory_test_cloop_AA55
        xor     v1,v1,v0
        lw      a3,(a2)
        sw      zero,(a2)
        bne     v1,a3,_RAM_memory_test_fail
        nop
        nop

#if 1   
	/************************************************
	 * identify the SAR number for sync on DMA test 
         ************************************************/
        lui     a0,FRAME_BUF_START_HI
        or      a2,a0,zero

        li      t1,0xfeed5a5a           /* 1st SAR */
        lw      t5,0(a2)
        bne     t5,t1,_FRAME_BUF_test
	li	t1,0x01			/* 2nd SAR */
	la	a0,_sar_number
	sw	t1,(a0)
#endif  

/*---------------------------------------------------------------------*
 * FRAME_BUF_test:
 *---------------------------------------------------------------------*/
_FRAME_BUF_test:

        /* Sizeing the frame buffer */
        lui     a0,FRAME_BUF_START_HI
        or      a2,a0,zero
        lui     a1, FRAME_BUF_END_HI
        ori     a1,a1,0xFFFC

        sw      t1,0(a2)
 
        li      t5,FRAME_BUF_SIZE         /* max size of RAM 0x0010.0000 */

        /* Perform Data Bus Test: stuck on 1 or zero  */
	li	t7,test_code3	 	/* SP RAM address bus test */
        lui     v0,0xFFFF
        ori     v0,v0,0xFFFF
        or      v1,zero,zero
        sw      v1,0(a2)           /* write 0x0000.0000 */
        sw      v0,4(a2)           /* write 0xFFFF.FFFF */
        lw      a3,0(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop
        or      v0,zero,zero
        lui     v1,0xFFFF
        ori     v1,v1,0xFFFF
        sw      v1,0(a2)           /* write 0xFFFF.FFFF */
        sw      v0,4(a2)           /* write 0x0000.0000 */
        lw      a3,0(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        ori     v1,zero,0x0001
_FRAME_BUF_dbus_loop:                   /* walk 1 test */
        sw      v1,0(a2)
        lw      a3,0(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop
        sll     v1,v1,1
        bne     v1,zero,_FRAME_BUF_dbus_loop
        nop


        /* Perform Address Bus Test */
	li	t7,test_code4	 	/* SP RAM address bus test */
        or      t0,a0,zero              /* starts from first location */
        ori     t4,zero,0x4             /* start from address line A2 */
        lui     v0,0xAAAA
        ori     v0,v0,0xAAAA            /* V0=0xAAAA.AAAA */
        lui     v1,0x5555
        ori     v1,v1,0x5555            /* V0=0x5555.5555 */

_FRAME_BUF_abus_loop1:
        ori     t3,zero,0x4             /* start from address line A2 */
        sw      v1,0(t0)                /* write 0x5555.5555 */
        or      a2,a0,zero              /* a2 now holds test address */

_FRAME_BUF_abus_loop2:
        beq     a2,t0,_FRAME_BUF_abus_skip
        nop
        sw      v0,0(a2)                /* write 0xAAAA.AAAA to current ..
                                         .. address*/
_FRAME_BUF_abus_skip:
        lw      a3,0(t0)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        sll     t3,t3,1                 /* set next address line hi */
        or      a2,a0,t3
        bne     t3,t5,_FRAME_BUF_abus_loop2   /* offset reaches the end of RAM
*/
        nop
        sll     t4,t4,1                 /* set next address line hi */
        or      t0,a0,t4                /* t0 holds the test address */
        bne     t4,t5,_FRAME_BUF_abus_loop1
        nop

        /************************************
         * Perform Byte/Word Enable tests
         ************************************/
        /* byte access */
        or      a2,a0,zero
        ori     v1,zero,0x12
        sb      v1,0(a2)
        lb      a3,0(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x34
        sb      v1,1(a2)
        lb      a3,1(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x56
        sb      v1,2(a2)
        lb      a3,2(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x78
        sb      v1,3(a2)
        lb      a3,3(a2)
        nop
        andi    a3,a3,0xFF
        bne     v1,a3,_RAM_memory_test_fail

        /* 32-bit access */
        lui     v1,0x1234
        ori     v1,v1,0x5678

        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail

        /* 16-bit access */
        ori     v1,zero,0x8765
        sh      v1,0(a2)
        lh      a3,0(a2)
        nop
        andi    a3,a3,0xFFFF
        bne     v1,a3,_RAM_memory_test_fail
        ori     v1,zero,0x4321
        sh      v1,2(a2)
        lh      a3,2(a2)
        nop
        andi    a3,a3,0xFFFF
        bne     v1,a3,_RAM_memory_test_fail
        lui     v1,0x8765
        ori     v1,v1,0x4321
        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail


        /****************************************
         * Frame buffer test with checker board
         ****************************************/
        or      a2,a0,zero
        lui     a1,FRAME_BUF_END_HI
        or      a1,a1,0xFFF0

        lui     v0,0xFFFF
        ori     v0,v0,0xFFFF
        lui     v1,0x5555
        ori     v1,v1,0x5555

_FRAME_BUF_test_wloop_55AA:
        sw      v1,(a2)
        addiu   a2,a2,4
        bne     a2,a1,_FRAME_BUF_test_wloop_55AA
        xor     v1,v1,v0
        sw      v1,(a2)

        or      a2,a0,zero
        lui     v1,0x5555
        ori     v1,v1,0x5555
_FRAME_BUF_test_cloop_55AA:
        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        addiu   a2,a2,4
        bne     a2,a1,_FRAME_BUF_test_cloop_55AA
        xor     v1,v1,v0
        lw      a3,(a2)
        nop
        bne     v1,a3,_RAM_memory_test_fail
        nop

        or      a2,a0,zero
        lui     v1,0xAAAA
        ori     v1,v1,0xAAAA
_FRAME_BUF_test_wloop_AA55:
        sw      v1,(a2)
        addiu   a2,a2,4
        bne     a2,a1,_FRAME_BUF_test_wloop_AA55
        xor     v1,v1,v0
        sw      v1,(a2)

        or      a2,a0,zero
        lui     v1,0xAAAA
        ori     v1,v1,0xAAAA
_FRAME_BUF_test_cloop_AA55:
        lw      a3,(a2)
        sw      zero,(a2)
        bne     v1,a3,_RAM_memory_test_fail
        nop
        addiu   a2,a2,4
        bne     a2,a1,_FRAME_BUF_test_cloop_AA55
	nop
	nop
        xor     v1,v1,v0
        lw      a3,(a2)
        sw      zero,(a2)
        bne     v1,a3,_RAM_memory_test_fail
        nop

        li      SHMEM_REG, SHARED_MEM_BASE_C  
#if  1  /* sync for SAR DMA test */
	lui	a0,FRAME_BUF_START_HI
	or	a2,a0,zero
	la	a0,_sar_number
	lw	t1,(a0)
	bne	t1,zero,fed_sig      /* 2nd SAR do DAM test directly */
	li	t1,0xfeed5a5a   	/* feed5a5a signature */
	sw	t1,(a2)
        li      SHMEM_REG, SHARED_MEM_BASE_C  
	li	t1,0x5a5afed0   	/* feed5a5a signature fed */
syn_tst:
	lw	t5,0(a2)
	bne	t5,t1,syn_tst		
        sw      zero,(a2)
	b	do_rx_SAR_L
fed_sig:
	li	t1,0x5a5afed0   	/* feed5a5a signature fed */
	sw	t1,(a2)
        li      SHMEM_REG, SAR_2_OFFSET  
1:	lw	t5,0(a2)
	nop
	bne	t5,zero,1b
#endif

/*-------------------------------------------------------------------*
 * do_rx_SAR_L:
 * Note:  This is not a call-subroutine i.e. this will not return to caller
 *-------------------------------------------------------------------*/
do_rx_SAR_L:
        li      a0,IOBASE
        sw      zero,LED3_OFFSET(a0)
        sw      zero,(a2)

	.set noreorder
	mfc0	v0,C0_SR
	or	v0,SR_CU3
	mtc0	v0,C0_SR
	.set reorder

        /* Setup VCR_REG, SHMEM_REG */
        li      VCR_REG, VCR_ADDR_C

        /* Initialize DMA Address Substitution Register */
        la      t0, M_ASUBR
        li      t1, ASUBR_VALUE_C   
        sw      t1, (t0)

        /* Initialize first rcv Cell in VCR to all As */
        li      t3, 0xa
        addu	t1,VCR_REG,zero
        addu    t2, t1, CELL_SIZE_C
1:      sb      t3, (t1)
        addu    t1, 1
        bne     t1, t2, 1b

        /* Initialize counter Registers */
        move    seq_num_REG, zero
        move    dma_ok_REG, zero
        move    dma_bad_REG, zero

	/* Calculate DMA Control Register Effective Address */
        li      t0, (48<<DMA_BCSHFT)|M_DMACR|DMA_WR     # Size = 48, Set RD bit
        la      t6, 0x0(SHMEM_REG)       # load destination DMA addr
        addu	t5,VCR_REG,zero       /* Always DMA the first cell in VCR */
        and     t5, 0xfff                       # Mask everyting but LAC
        or      t5, t0             /* t5 contains the DMAC Effective Addr */

DMA_Loop_L:
        /* Increment sequence number */
        addu    seq_num_REG, 1

        /* Store seq number in SOURCE DMA buffer */
        sw      seq_num_REG, (VCR_REG)

        /* Start DMA */
        sw      t6, (t5)                /* Write to DMAC starts DMA */	
	nop				/* do not remove NOP lightly */
	nop
	nop

        /* wait for DMA to complete */
1:      bc3t    1b
	nop

        # Check to see if value at end of DMA is the same as sequence count
        lw      t2, (t6)
	nop
        beq     seq_num_REG, t2, 1f
	nop

        # Corruption during DMA
        addu    dma_bad_REG, 1
        b       2f
	nop

1:      /* DMA OK */
        addu    dma_ok_REG, 1

2:      /* Store results in shared memory */
        sw      seq_num_REG, TEST_COUNT_OFFSET(SHMEM_REG)
        sw      dma_ok_REG, OK_COUNT_OFFSET(SHMEM_REG)
        sw      dma_bad_REG, BAD_COUNT_OFFSET(SHMEM_REG)

#if  1
	li      t0,0x3000		/* let run 0x3000 laps */
	beq	seq_num_REG,t0,dma_err
	nop

#endif  /* forever test */
        b       DMA_Loop_L
	nop
dma_err:
	beq	dma_bad_REG,zero, led_tst
	li	t7,err_code6
	b 	wr_errcode_led

/*-------------------------------------------------------------------*
 * led_tst()
 * Note:  This is not a call-subroutine.
 *-------------------------------------------------------------------*/
led_tst:
        li      a0,HOST_INTR_REG_BASE
        sw	zero,0(a0) 
        li      a0,IOBASE
        sw      zero,LED1_OFFSET(a0)

	li	t7,err_codeC
	b	wr_errcode_led

/*-------------------------------------------------------------------*
 * RAM_memory_test_fail
 * Note:  This is not a call-subroutine i.e. this will not return to caller
 *-------------------------------------------------------------------*/
_RAM_memory_test_fail:
        /* TBD: the test results need to be put in the SP_RAM frome which
         *      the ACP can retrieve them.
         */
        la      a0,_sys_fail_address
        sw      a2,(a0)
	li	a0, 0xA0C00000
        sw      a2,(a0)
        la      a0,_sys_data_expected
        sw      v1,(a0)
	li	a0, 0xA0C00000+4
        sw      v1,(a0)
        la      a0,_sys_data_read
        sw      a3,(a0)
	li	a0, 0xA0C00000+8
        sw      a3,(a0)

        li      t0, 0xffffffff
        or	v0,t7,zero      
memory_fail_spin:
        b	wr_errcode_led

/*-------------------------------------------------------------------*
 * wr_errcode_led()
 *    Input:
 *          t7= error code
 *          s1= return address
 *    Note: this routine uses register s1 for its return address
 *          Not callable module(i.e. do not use jal )
 *    registers destroyed:
 *          t0, t1, t6, t7 s0 
 *-------------------------------------------------------------------*/
        .global wr_errcode_led
wr_errcode_led:
	li	a0, 0xA0C00000+12
        sw      t7,(a0)

	or	v0,t7,zero
        li      s0,0xFF
/*        li      t1,0xA0E80040  */
flash_err_led:
        li      t1,IOBASE
        and     t6,t7,0x1
        bne     t6,zero,1f
        sw      s0,LED4_OFFSET(t1)
        b       c1
1:      sw      zero,LED4_OFFSET(t1)
c1:     and     t6,t7,0x2
        bne     t6,zero,2f
        sw      s0,LED3_OFFSET(t1)
        b       c2
2:      sw      zero,LED3_OFFSET(t1)
c2:     and     t6,t7,0x4
        bne     t6,zero,3f
        sw      s0,LED2_OFFSET(t1)
        b       c3
3:      sw      zero,LED2_OFFSET(t1)
c3:     and     t6,t7,0x8
        bne     t6,zero,4f
        sw      s0,LED1_OFFSET(t1)
        b       c4
4:      sw      zero,LED1_OFFSET(t1)
c4:
/*    j       s1  */   /* do not jump back */

errspin:
        /* visual delay for LEDs */
        li      t0,0x200000
1:      subu    t0,1
        bne     zero,t0,1b
        nop
        xor     t7,v0,t7
        b       flash_err_led
        nop


        /**********************/
	/* POD Test Completed */
        /**********************/
pod_ok:

	.end _start

/*-------------------------------------------------------------------*
 *  _exception:
 *-------------------------------------------------------------------*/
	.globl	_exception
	.ent _exception
_exception:

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
        b       _doom
4:      sw      zero,0x0(t1)

_doom:        
        b       _doom

	.end _exception


/*************************************************************
*  flush_cache(type,adr)
*	Flush the designated cache (DCACHE, ICACHE, IADDR)
*	Note that this is not a real subroutine, it simply
*	transfers control to the routine pointed to by 'flush_ptr',
*	being very careful to not use Kseg0 just in case the caches
*	haven't been flushed yet.
*/
/*
	.globl flush_cache
	.ent flush_cache
flush_cache: # a0=type a1=adr
	la	t1,flush_ptr
	or	t1,K1BASE
	lw	t0,(t1)
	or	t0,K1BASE
	bne	t0,zero,1f
	j	ra
     1:	j	t0
	.end flush_cache
*/

#if 0
#ifdef CYGNUS
	.globl __main
	.ent __main
__main:
	j	ra
	.end __main
#endif
#endif  /* 0 */


/*-------------------------------------------------------------------*
 * IRAM_err_handler
 *-------------------------------------------------------------------*/
IRAM_err_handler:
        /* turn off diag led(code entry ) */
        la      s0,wr_errcode_led
        la      s1,err_1                  /* test is the return address */
        li      t7,err_code1              /* diag code bigger than IRAM */
        j       s0
err_1:

        /* visual delay for LEDs */
        li      t0,0x30000
1:      subu    t0,1
        bne     zero,t0,1b
	xor 	t7,t7,err_code1
        j       IRAM_err_handler          /* restart SAR */

/*---------------------------< END >---------------------------------*/

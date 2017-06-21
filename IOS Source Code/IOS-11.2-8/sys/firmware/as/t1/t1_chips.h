/* $Id: chips.h,v 1.10 1996/07/13 05:15:46 towu Exp $
 * $Source: /cscka/t1_neat/src/t1/chips.h,v $
 *------------------------------------------------------------------
 * chips.h  NEAT hardware definitions
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: chips.h,v $
 * Revision 1.10  1996/07/13 05:15:46  towu
 * o Due to the code size getting large, changed the stack pointer to 0x17000
 *
 * Revision 1.9  1996/07/10  06:44:19  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.8  1996/06/19  23:03:30  jliu
 *     o There was a sever bug found from devtest. This bug caused framer registers
 *       content changed. The reason was the stack area stepped over code space
 *     o we should use FrameType instead of frame_mode
 *
 * Revision 1.7  1996/06/07  21:40:20  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.6  1996/04/20  02:37:36  jliu
 *     o Changed timer inetrrupt resolution from 50ms to 1 ms
 *
 * Revision 1.5  1996/04/08  18:17:49  jliu
 *     o cleanup some code
 *     o modified code for digits collection
 *
 * Revision 1.4  1996/03/19  03:54:37  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.3  1996/03/02  03:51:16  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.2  1996/02/22  22:27:43  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_CHIPS_H
#define HAVE_CHIPS_H


/******************************************************
 *
 *          Define the Memory Map
 *
 * 0 - 20000 is the SRAM address range on T1 card which is 128k bytes size
 */

#define LOCAL_RAM_ADDR       0x0000
#define LOCAL_RAM_SIZE       (128 * 1024)
#define LOCAL_STACK_POINTER  0x17000
#define NEAT_SRAM_END	     LOCAL_RAM_SIZE

#define DPRAM_ADDR	     0xF0000               /* dual port RAM base */
                                                   /* address in MC68360 */
#define REGB_ADDR            (DPRAM_ADDR + 0x1000) /* start of internal regs */
#define SCC1_PARAM_ADDR      (DPRAM_ADDR + 0xC00)  /*start of SCC1 Parameter RAM*/
#define SCC2_PARAM_ADDR      (DPRAM_ADDR + 0xD00)  /*start of SCC2 Parameter RAM*/
#define MBAR_VALUE           (DPRAM_ADDR + 1)      /* turn on the valid bit */
#define MBAR_ADDR            0x3FF00               /* adress of the MBAR */


#define FRAMER_BASE_ADDR     0x80000	/* Framer chip base address */
#define FRAMER_GAP	     0x01000	/* Framer chips gap	    */
#define DIGIT_COL0_ADDR      0x84000    /* 8 bit, read/write */
#define DIGIT_COL1_ADDR      0x84010    /* 8 bit, read/write */
#define NEAT_LED_ADDR  	     0x84040	/* 8 bits Read/Write */
#define LIU_CONT_BASE_ADDR   0x84050	/* LIU controll registers base addr */
#define LIU_CONT_GAP	     0x00010	/* LIU control register gap */
#define LIU01_STAT_ADDR      0x84090    /* 8 bits Read only */
#define NEAT_INT_ADDR        0x840B0    /* 8 bits Write only*/


#define MAX_DTMF 	     0x02


/*
 * Digit collection register for DTMF 0 is at 0x84000 and for DTMF 1 is
 * at 0x84010.
 */
#define DIGIT_COL0 (*(unsigned volatile short *)DIGIT_COL0_ADDR)
#define DIGIT_COL1 (*(unsigned volatile short *)DIGIT_COL1_ADDR)
#define    D4_1         0x0f
#define    DV           0x10

/* LED control register */
#define NEAT_LED (*(unsigned volatile short *)NEAT_LED_ADDR)
#define  LOCAL_ALARM_0  0x01   /* write 1 to trun on LED & 0 to trun off LED */
#define  REM_ALARM_0    0x02   /* write 1 to trun on LED & 0 to trun off LED */
#define  LOOPBACK_0     0x04   /* write 1 to trun on LED & 0 to trun off LED */
#define  LOCAL_ALARM_1  0x08   /* write 1 to trun on LED & 0 to trun off LED */
#define  REM_ALARM_1    0x10   /* write 1 to trun on LED & 0 to trun off LED */
#define  LOOPBACK_1     0x20   /* write 1 to trun on LED & 0 to trun off LED */
#define  ULAW_ALAW      0x80   /* 0 for u-law & 1 for A-law */

#define NEAT_INT (*(volatile ushort *)NEAT_INT_ADDR)
#define   LEVEL_4 	0x01   /* generate level 4 interrupt to core CPU */
#define   LEVEL_6 	0x02   /* generate level 6 interrupt to core CPU */
#define   CLR_NEAT_INT  0x20   /* clear interrupt from core CPU */

/* Registers value definitions for QUICC */
#define QUICC_PEPAR_VAL      0x15     /* select IACK3,6,1,2,5 as ouput pins */
#define QUICC_SYPCR_NWD_VAL  0x0e     /* without watchdog */
#define QUICC_CPCR_RST       0x8000   /* software reset command */
#define QUICC_CPCR_SEMA      0x0001   /* command semaphore flag */
#define QUICC_CPCR_STOPTX    0x400    /* opcode = 0100 for stop transmit */
#define QUICC_CPCR_GRSTOPTX  0x500    /* opcode = 0101 for graceful stop */
                                      /* transmit */
#define QUICC_CPCR_RESTART   0x600    /* opcode = 0110 for restart transmit */
#define QUICC_CPCR_SCC2      0x40     /* channel number = 0100 for SCC2 */
#define QUICC_CICR_VAL       0xE49FA1 /* SCCd(11) - scc4, SCCc(10) - scc3, */
                                      /* SCCb(01) - scc2, SCCa(00) - scc1, */
                                      /* ILR - 100, HP - 11111, VBA - 101, */
                                      /* SPS - 1*/
#define QUICC_SDCR_VAL     0x0740     /* SDMA interupt service mask - 7, */
                                      /* SDMA arbitration ID - 4 */
#define QUICC_CIMR_SCC1    0x40000000 /* SCC1 interrupt */
#define QUICC_CIMR_SCC2    0x20000000 /* SCC2 interrupt */
#define QUICC_CIMR_TIMER1  0x02000000 /* timer 1 interrupt */
#define QUICC_CIMR_TIMER2  0x00040000 /* timer 2 interrupt */
#define QUICC_TGCR_TMR1_EN 0x0001     /* enable the corresponding timer */
#define QUICC_TGCR_TMR2_EN 0x0010     /* enable the corresponding timer */

#define TMR_TER_CAP_EVENT  0x0001
#define TMR_TER_REF_EVENT  0x0002
/*
 * The following timer is not exactly 1 ms tick. The reason because there are
 * some software overhead to process 1ms interrupt. That's the reason why we
 * chose to use value less than 1 ms. This is especially true when test 1 sec
 * ANSI PRM. If we use the value close to 1 ms, you will see one of 16 PRM is
 * missing from T-com because the timer drift
 */
#define TMR2_PRESCALE      0x3000  /* prescaler value 50 for timer mode reg */
#define TMR2_ICLK_DIV16    0x0004  /* internal general sys clock divided by 16 */
#define TMR2_TIME_1MS      23      /* generate a 1ms timer by using the*/
                                   /* formula here. 23 * 50 * 16 * 50ns */
#define TMR1_PRESCALE      0xc800  /* prescaler value 200 for timer mode reg */
#define TMR1_ICLK_DIV1     0x0002  /* internal general sys clock divided by 1 */
#define TMR1_TIME_250MS    0x6130  /* generate a 250ms timer by using the*/
                                   /* formula here. 25000 * 200 * 1 * 50ns */
#define TMR_INT_EN         0x0010  /* enable int upon reaching reference value */

/*
 * Offsets into SI RAM for the tonedata routing to TDM channel maps.
 * One TDM channel with static frames is chosen, each transmit and receive
 * routing RAM has size 64 x 16 bits
 */
#define QUICC_SIRAM_RX     0x0000          /* Offset for RX Active Map */
#define QUICC_SIRAM_TX     0x0040          /* Offset for TX Active Map */

#define SCC1_DIALBYTE      0x0042          /* Route SCC1 byte to TDM channel a */
#define SCC2_BUSYBYTE      0x0083          /* Route SCC2 byte to TDM channel a */
#define PORT_SCC1_SCC2     0x00004040      /* SCC1 & SCC2 are connected to TDM */
#define QUICC_SIGMR_ENABLE_A   0x04        /* Enable TDM channel a */
#define QUICC_SIGMR_ONE_STATIC 0x00        /* One TDM channel with static map */

#define QUICC_AVEC1 0x02  /* autovector response for level 1 external int */
#define QUICC_AVEC2 0x04  /* autovector response for level 2 external int */
#define QUICC_AVEC3 0x08  /* autovector response for level 3 external int */
#define QUICC_AVEC4 0x10  /* autovector response for level 4 external int */
#define QUICC_AVEC5 0x20  /* autovector response for level 5 external int */
#define QUICC_AVEC6 0x40  /* autovector response for level 6 external int */
#define QUICC_AVEC7 0x80  /* autovector response for level 7 external int */

#define SCC1_TxBD_OFFSET     0x10       /* the start of the SCC1 Tx BD table */
#define SCC2_TxBD_OFFSET     0x20       /* the start of the SCC2 Tx BD table */

#define TDM_RXDa     0x80  /* port A pin 7 is used for TDMa receive input */
#define TDM_TXDa     0x40  /* port A pin 6 is used for TDMa transmit output */
#define TDM_RCLKa    0x100 /* port A pin 8 is used for TDMa clock */
#define TDM_TCLKa    0x400 /* port A pin 10 is used for TDMa clock */
#define TDM_SYNCa    0xc00 /* port c pin 11 & 10 is used for TDMa sync pulse */

/******************************************************
 *
 *          Define the Interrupt Priorities
 */

#define PRIO_NA0        7
#define PRIO_FDL        6
#define PRIO_BACKPLANE  5
#define PRIO_TIMER      4
#define PRIO_FRAMER     3
#define PRIO_MPU        2
#define PRIO_NA1        1
#define ALL_INTERRUPTS  0

#define SR_IPL          0x0700           /* Interrupt Priority Level */
#define IPL_TO_SR(i)    ((i) << 8)       /* convert number to SR IPL */
#define IPL_FROM_SR(i)  (((i) >> 8) & 7) /* convert SR IPL to number */

#endif      /* HAVE_CHIPS_H */


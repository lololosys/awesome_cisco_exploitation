/* $Id: if_as5200_dsx1.h,v 3.1.14.3 1996/09/02 08:37:01 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_dsx1.h,v $
 *------------------------------------------------------------------
 * NEAT control registers definition.
 *
 * May 1996, Simon Chen
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_dsx1.h,v $
 * Revision 3.1.14.3  1996/09/02  08:37:01  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.1.14.2  1996/07/11  22:36:55  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *           o Added new fdl command for t1 controller in order to support
 *             CSU function
 *
 * Revision 3.1.14.1  1996/06/16  21:14:33  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/19  20:07:09  vnguyen
 * Placeholder file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Offset of the external registers on DSX1 card
 */

#define DSX1_PRI_CONTROL1    0x0006  /* offset to DSX1 PRI Control 1 register */
#define  DSX1_NON_NEAT_RST   0x01    /* reset non NEAT circuitry */
#define  DSX1_NEAT_RST       0x08    /* reset NEAT circuitry */
#define  DSX1_L4_INT_EN      0x20    /* level 4 interrupt enable */
#define  DSX1_L6_INT_EN      0x40    /* level 6 interrupt enable */

#define DSX1_PRI_STATUS      0x0008  /* offset to DSX1 PRI Status register */
#define  DSX1_NEAT_HALT      0x01    /* NEAT processor in halt condition */
#define  DSX1_NEAT_INT_L4    0x02    /* level 4 interrupt from NEAT processor */
#define  DSX1_NEAT_INT_L6    0x04    /* level 6 interrupt from NEAT processor */

#define DSX1_PRI_CONTROL2    0x000e  /* offset to DSX1 PRI Control 2 register */
#define  DSX1_ALL_RESET_EN   0x01    /* enable all DSX1 circuitry reset */
#define  DSX1_PRI_CLK_SEL    0x02    /* select primary clock from LIU 1 or 2 */
                                   /* 1 for LIU 1 and 0 for LIU 2 */
#define  DSX1_PRI_CLK_EN     0x04    /* enable sel primary clk from LIU 1 or 2 */
#define  DSX1_SEC_CLK_SEL    0x08    /* select secondary clock from LIU 1 or 2 */
                                   /* 1 for LIU 1 and 0 for LIU 2 */
#define  DSX1_SEC_CLK_EN     0x10    /* enable sel secondary clk from LIU 1 or 2 */

#define DSX1_PAGE            0x0010  /* offset to DSX1 page register */
#define PAGE_ADDR          0xff000 /* The bits 19-12 contains page address */
#define PAGE_ADDR_MASK	   0x000ff000
#define NEAT_SPACE         0x1000  /* bit 12=1 is used to indicate that core */
                                   /* CPU accesses the NEAT memory space */
#define NEAT_PAGE_SPACE_MASK (NEAT_SPACE - 1)
#define PAGE_CROSS         0xffe   /* 2k word (4k bytes)page size */
#define NEAT_ADDR          0xfff

#define NEAT_LEDS      0x84040    /* NEAT LED address */
#define NEAT_PLD_REV   0x840c0    /* NEAT PLD rev # address */

/* LED control register */
#define  LOCAL_ALARM_0  0x01   /* write 1 to trun on LED & 0 to trun off LED */
#define  REM_ALARM_0    0x02   /* write 1 to trun on LED & 0 to trun off LED */
#define  LOOPBACK_0     0x04   /* write 1 to trun on LED & 0 to trun off LED */
#define  LOCAL_ALARM_1  0x08   /* write 1 to trun on LED & 0 to trun off LED */
#define  REM_ALARM_1    0x10   /* write 1 to trun on LED & 0 to trun off LED */
#define  LOOPBACK_1     0x20   /* write 1 to trun on LED & 0 to trun off LED */
#define   BOARD_OK      0x40      /* indicate DSX1 card is ok */
#define   COMPAND       0x80      /* 1 for u-law and 0 for A-law */


#define LIU0_CONTROL   0x84050    /* 16 bits Read/Write */
#define LIU1_CONTROL   0x84060    /* 16 bits Read/Write */
/*
 * LIU control register for DSX1 0 is at 0x84050 and for DSX1 1 is at 0x84060.
 */
#define   EC4_1         0x0f   /* EC4-1 for pulse equalization and Line */
                               /* Build Out */
#define     L36_0DB     0x0    /* all the options here are determinated by */
#define     L26_0DB     0x1    /* values in EC4 to EC1 */
#define     L36_75DB    0x2
#define     L26_75DB    0x3
#define     L36_15DB    0x4
#define     L26_15DB    0x5
#define     L36_225DB   0x6
#define     L26_225DB   0x7
#define     SHORT_06DB  0xB

/*
 * LIU status register 
 */
#define	 LIU_STATUS			0x84090
#define  UNBALANCE_0		0x40
#define  UNBALANCE_1		0x80
#define  MPU_MSG_SEND_RAI	0x10
#define  MPU_MSG_HOMOL_TYPE	0x20



#define DSX1_NEAT_INT    0x840B0     /* NEAT processor interrupt control */
                                   /* reg location */
#define NEAT_INT_GEN      0x04 
#define CLR_LEVEL4        0x08
#define CLR_LEVEL6        0x10
#define NEW_REQ_CMD       0x8000 
#define NEW_REQ_OFFSET    0x02 
#define PHONE_NUM_OFFSET  0x06
#define TOTAL_NUM_OFFSET  0x02
#define MODEM_ID_GET      0xff
#define MAX_DIGITS        65


/* bit definition of ERROR message from NEAT processor */
#define MSG_DIG_MAX_ERR         0x01
#define MSG_BADFCS_ERR          0x02
#define MSG_ABORT_ERR           0x04
#define MSG_OVERRUN_ERR         0x08
#define MSG_RECEIVED            0x10
#define MSG_SENT                0x20


/* bit definition for MIB error information from NIM processor */
#define  MSG_ES_OCCURRED         0x01
#define  MSG_BES_OCCURRED        0x02
#define  MSG_SES_OCCURRED        0x04
#define  MSG_CSS_OCCURRED        0x08
#define  MSG_FR_LOSS_OCCURRED    0x10
#define  MSG_LES_OCCURRED        0x20

#define LOCAL_LOOP   0x01
#define REMOTE_LOOP  0x02

#define  MAX_DSX1_PER_SLOT   0x02    /* 2 DSX1 controller per slot */
#define  MAX_UPDATE_SIZE   0x20    /* buffer size to store full message from */
                                   /* Neat processor */
/*
 * Macros
 *
 * DSX1_PRIMARY_CLK_UNIT(reg) / DSX1_SECONDARY_CLK_UNIT(reg):
 * Description: Returns the unit number of the LIU currently configured 
 *              as primary/secondary line clk source in "PRI Control Reg 2". 
 * Parameter  : reg = value of "PRI Control Reg 2" register.
 * Return     : 0 for LIU1 
 *              1 for LIU2
 */
#define DSX1_PRIMARY_CLK_UNIT(reg)   ((reg & DSX1_PRI_CLK_SEL) ? 0 : 1)
#define DSX1_SECONDARY_CLK_UNIT(reg) ((reg & DSX1_SEC_CLK_SEL) ? 0 : 1)

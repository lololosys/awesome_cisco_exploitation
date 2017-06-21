/* $Id: quicc_bri.h,v 3.2 1995/11/17 18:44:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36-c1000/quicc_bri.h,v $
 *------------------------------------------------------------------
 * quicc_bri.h
 *
 * January 1995, Karuna Sabnis
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the BRI diagnostic for Sapphire.
 *------------------------------------------------------------------
 * $Log: quicc_bri.h,v $
 * Revision 3.2  1995/11/17  18:44:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:39:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* defines for channels */
#define D_CHAN     0                       /* Index for D chan */
#define B1_CHAN    1                       /* Index for B1 chan */
#define B2_CHAN    2                       /* Index for B2 chan */
#define SPI_CHAN   3                       /* Index for SPI chan */
#define MAX_CHAN   4                       /* Max number of chans */		   

#define POLL_MODE  0                       /* Poll mode */
#define INTR_MODE  1                       /* Interrupt mode */
#define MAX_MODE   2
#define BRI_TEST_ON  1
#define BRI_TEST_OFF 0
#define MFLR   0x2000                       /* maximum frame length */
#define BUF_SIZE  0x800
#define MRBLR BUF_SIZE                     /* maximum receive buffer length */
#define MRBLR1  0x10                       /* maximum receive buffer length for spi*/
#define TX_LENGTH BUF_SIZE-2               /* number of bytes transmitted, 
				              2 bytes reserved for CRC */
#define SID   0                            /* SID loop back mode */
#define SCCLP 1                            /* SCC loopback mode */

#define BCLK_256K 98                       /* for 256Khz for BCLK */

/* Memory location allocations for different channels */
#define ISDN_RX_BD1_MEM   0x2006000        /* D channel Rx */
#define ISDN_TX_BD1_MEM   0x2007000        /* D channel Tx */
#define ISDN_RX_BD2_MEM   0x2008000        /* B1 channel Rx */
#define ISDN_TX_BD2_MEM   0x2009000        /* B1 channel Tx */
#define ISDN_RX_BD3_MEM   0x200a000        /* B2 channel Rx */
#define ISDN_TX_BD3_MEM   0x200b000        /* B2 channel Tx */
#define ISDN_RX_BD4_MEM   0x200c000        /* SPI Rx */
#define ISDN_TX_BD4_MEM   0x200d000        /* SPI Tx */

#define SAP_QU_PT_A_BRI_DIR_CLR   0x313        /* Direction value */
#define SAP_QU_PT_B_BRI_ODR_CLR   0x31B0       /* ~ Open Drain clear value */
#define SAP_QU_PT_B_BRI_ODR_SET   0x0040       /* Open Drain init value */

/* Port B pins related values */
#define QU_PT_A_BRI_DIR_CLR   0x313        /* Direction value */
#define QU_PT_B_BRI_ODR_CLR   0x31B0       /* ~ Open Drain clear value */
#define QU_PT_B_BRI_ODR_SET   0x0040       /* Open Drain init value */

#define QUICC_CLR_PB4(dpr)	((dpr)->regs.pip_pbdat &= ~0x10)
#define QUICC_SET_PB4(dpr)	((dpr)->regs.pip_pbdat |=  0x10)

#define TP3420_ASSERT	QUICC_CLR_PB4
#define TP3420_NEGATE	QUICC_SET_PB4

/* BRI related register values */
#define QUICC_BRI_PSMR_SETTING      0x00
#define QUICC_BRI_REG_CLEAR         0xffff

/* BRI related parameters */

#define BRI_PRAM_C_PRES       0x0000FFFF; /* 16 bit CRC preset */
#define BRI_PRAM_C_MASK       0xF0B8;     /* 16 bit CRC */
#define BRI_PRAM_PADS         0x8888
#define BRI_PRAM_RET_LIM      0x000f
#define BRI_PRAM_MFLR         0x05ee

/* Defines buffer descriptors */

typedef unsigned short * mempointer;

#define ETH_SCC1_RX_BD_IDX   0x3       /* RxBD for the Ethernet */
#define ETH_SCC1_TX_BD_IDX   0x4       /* TxBD for the Ethernet */
#define ISDN_SCC2_RX_BD_IDX  0x5       /* RxBD for the ISDN_ B1 channel */
#define ISDN_SCC2_TX_BD_IDX  0x9       /* TxBD for the ISDN_ B1 channel */
#define ISDN_SCC3_RX_BD_IDX  0xD       /* RxBD for the ISDN_ D  channel */
#define ISDN_SCC3_TX_BD_IDX  0x11      /* TxBD for the ISDN_ D  channel */
#define ISDN_SCC4_RX_BD_IDX  0x15      /* RxBD for the ISDN_ B2 channel */
#define ISDN_SCC4_TX_BD_IDX  0x19      /* TxBD for the ISDN_ B2 channel */
#define ISDN_SPI_RX_BD_IDX   0x24      /* RxBD for the ISDN_SPI */
#define ISDN_SPI_TX_BD_IDX   0x25      /* TxBD for the ISDN_SPI */

/* Defines for Interrupt flag */

#define BRI_RX_ERROR               0x01       /* Error in receive */ 
#define BRI_TX_ERROR               0x02       /* Error in transmit */
#define BRI_BUSY_ERROR             0x04       /* Error - busy */
#define SPI_M_MASTER               0x08       /* Multi-Master Error */
#define SPI_TX_ERR                 0x10       /* SPI Tx Error */
#define SPI_BSY                    0x20       /* SPI Busy Condition */ 
#define SID_D_CHAN_COLLISION       0x40       /* SID: D-chan collision */
#define SID_ACTIVATION_INDICATION  0x80       /* SID: Activation Indication */
#define SID_LOSS_FR_ALIGN          0x100      /* SID: Loss of Frame Alignment */
#define RX_OV_FL                   0x200      /* RX Frame Length Violation */
#define NON_OCT_ALGN               0x400      /* Octate not aligned */
#define RX_ABORT                   0x800      /* Receive Abort */
#define CRC_ERR                    0x1000     /* CRC error */
#define OVR_ERR                    0x2000     /* Overrun error */
#define CARR_LOST                  0x4000     /* Carrier lost */  
#define SPURIOUS_INTR              0x8000     /* Spurious interrupt */

/* Defines for the National TP3420 ISDN chip */

/* Register Commands */

/* Activation/Deactivation */
#define TP3420_CRF_NOP          0xFF    /* NOP */
#define TP3420_CRF_PDN          0x00    /* Power-Down */
#define TP3420_CRF_PUP          0x20    /* Power-Up */
#define TP3420_CRF_DR           0x01    /* Deactivation request */
#define TP3420_CRF_FI2          0x02    /* Force INFO2 */
#define TP3420_CRF_MMA          0x1F    /* Monitor Mode Activation */
#define TP3420_CRF_AR           0x03    /* Activation Request */

/* Device Modes */
#define TP3420_CRF_NTA          0x04    /* NT Mode, Adaptive Sampling */
#define TP3420_CRF_NTF          0x05    /* NT Mode, Fixed Sampling */
#define TP3420_CRF_TES          0x06    /* TE Mode, DSI Slave */
#define TP3420_CRF_TEM          0x07    /* TE Mode, DSI Master */

/* Digital Interface Formats */
#define TP3420_CRF_DIF1         0x08    /* DSI Format 1 */
#define TP3420_CRF_DIF2         0x09    /* DSI Format 2 */
#define TP3420_CRF_DIF3         0x0A    /* DSI Format 3 */
#define TP3420_CRF_DIF4         0x0B    /* DSI Format 4 */

/* B Channel Exchange */
#define TP3420_CRF_BDIR         0x0C    /* B Chan Mapped Direct, B1-B1
					   B2-B2 */
#define TP3420_CRF_BEX          0x0D    /* B Chan Exchanged, B1->B2
					   B2->B1 */
/* D Channel Access */
#define TP3420_CRF_DREQ1        0x0E    /* D Chan Request, Class 1
					   Message */
#define TP3420_CRF_DREQ2        0x0F    /* D Chan Request, Class 2
					   Message */
#define TP3420_CRF_DCKE         0xF1    /* D Chan Clock enable */
#define TP3420_CRF_DACCE        0x90    /* Enable D chan access
					   mechanism */
#define TP3420_CRF_DACCD        0x91    /* Disable D chan access
					   mechanism */
/* End of Message Interrupt */
#define TP3420_CRF_EIE          0x10    /* EOM Interrupt Enabled */
#define TP3420_CRF_EID          0x11    /* EOM Interrupt Disabled */

/* Multiframe Circuit and Interrupt */
#define TP3420_CRF_MIE          0x12    /* Multiframe Circuit & Int
					   Enabl\ed */
#define TP3420_CRF_MID          0x13    /* Multiframe Circuit & Int
					   Disab\led */

/* Multiframe Receive Message 3X Checking */
#define TP3420_CRF_EN3X         0x28    /* Enable 3X Checking */
#define TP3420_CRF_DIS3X        0x29    /* Disable #x Checking */

/* Multiframe Transmit register */
#define TP3420_CRF_MFT          0x30    /* Write to Multiframe XMT Reg */

/* B1 Channel Enable/Disable */
#define TP3420_CRF_B1E          0x14    /* B1 Channel Enabled */
#define TP3420_CRF_B1D          0x15    /* B1 Channel Disabled */

/* B2 Channel Enable/Disable */
#define TP3420_CRF_B2E          0x16    /* B2 Channel Enabled */
#define TP3420_CRF_B2D          0x17    /* B2 Channel Disabled */

/* Loopback Test Modes */
#define TP3420_CRF_LBL1         0x18    /* Loopback B1 Towards Line IF */
#define TP3420_CRF_LBL2         0x19    /* Loopback B2 Towards Line IF */
#define TP3420_CRF_LBS          0x1A    /* Loopback 2B+D Towards
					   Digtal I\F */
#define TP3420_CRF_LBB1         0x1C    /* Loopback B1 Towards Digital
					   IF\ */
#define TP3420_CRF_LBB2         0x1D    /* Loopback B2 Towards Digital
					   IF\ */
#define TP3420_CRF_CAL          0x1B    /* Clear All Loopbacks */


/* Status Register Functions */
#define TP3420_SRF_NOC          0x00    /* No Change */
#define TP3420_SRF_LSD          0x02    /* Line Signal Detected from
					   Far-\End */
#define TP3420_SRF_AP           0x03    /* Activation Pending */
#define TP3420_SRF_EOM          0x06    /* End of Message */
#define TP3420_SRF_CON          0x07    /* Lost Contention */
#define TP3420_SRF_MFR          0x30    /* Multiframe RCV Buffer Full */
#define TP3420_SRF_AI           0x0C    /* Activation Indication */
#define TP3420_SRF_EI           0x0E    /* Error Indication */
#define TP3420_SRF_DI           0x0F    /* Deactivation Indication */


/* Q-Channel and S1-Channel Messages */

/* Receive messages - S1-Chan */
#define TP3420_S1CHAN_IDLE      0x0     /* Idle (Normal) */
#define TP3420_S1CHAN_LOP       0xF     /* Loss of Power */
#define TP3420_S1CHAN_STPP      0x2     /* STP Pass */
#define TP3420_S1CHAN_STPF      0x1     /* STP Fail */
#define TP3420_S1CHAN_STII      0x7     /* STI Indication */
#define TP3420_S1CHAN_DTSE_IN   0x8     /* DTSE-IN */
#define TP3420_S1CHAN_DTSE_OUT  0x4     /* DTSE-OUT */
#define TP3420_S1CHAN_DTSE_BOTH 0xC     /* DTSE-IN&OUT */
#define TP3420_S1CHAN_LB1I      0xD     /* LB1 Indication */
#define TP3420_S1CHAN_LB2I      0xB     /* LB2 Indication */
#define TP3420_S1CHAN_LBI       0x9     /* LB1/2 Indication */
#define TP3420_S1CHAN_LORSI     0xA     /* Loss-of-Received-Signal */

/* Send messages - Q-Chan */
#define TP3420_QCHAN_IDLE       0xF     /* Idle (Normal) */
#define TP3420_QCHAN_LOP        0x0     /* Loss of Power */
#define TP3420_QCHAN_STR        0x1     /* ST request */
#define TP3420_QCHAN_LB1R       0x7     /* LB1 request */
#define TP3420_QCHAN_LB2R       0xB     /* LB2 request */
#define TP3420_QCHAN_LBR        0x3     /* LB1/2 request */

/* Pin signal selection */
#define TP3420_PSS_PINDEF        0xE1   /* Redefine pin signals */

/* BCLK frequency setting */
#define TP3420_BCHAN_BCLK1       0x98   /* BCLK = 2.048 MHz */
#define TP3420_BCHAN_BCLK2       0x99   /* BCLK = 256 kHz */
#define TP3420_BCHAN_BCLK3       0x9A   /* BCLK = 512 kHz */
#define TP3420_BCHAN_BCLK4       0x9B   /* BCLK = 2.56 MHz */

#define QUICC_SET_PC10(dpr)	((dpr)->regs.pio_pcdat |=  0x400)
#define QUICC_CLR_PC10(dpr)	((dpr)->regs.pio_pcdat &= ~0x400)
#define QUICC_SET_PC11(dpr)	((dpr)->regs.pio_pcdat |=  0x800)
#define QUICC_CLR_PC11(dpr)	((dpr)->regs.pio_pcdat &= ~0x800)

/* =================================================================*
 *                      Function Prototypes                         *
 * =================================================================*/

extern int bri_test(int one);
extern void spi_isr(void);
extern void sid_diag_int(void);
extern void scc_int(void);
extern int bri_rx_intr(quicc_bd_t *rmd);
extern int bri_tx_intr(quicc_bd_t *tmd);
extern int bri_misc_intr(volatile ushort *scce_ptr, quicc_bd_t *rmd, quicc_bd_t *tmd);
extern int quicc_timer2_int(void);

/* end of module */


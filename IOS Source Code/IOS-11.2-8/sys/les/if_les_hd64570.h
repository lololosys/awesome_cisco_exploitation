/* $Id: if_les_hd64570.h,v 3.3.62.3 1996/09/05 01:16:48 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_hd64570.h,v $
 *------------------------------------------------------------------
 * if_les_hd64570.h -- Defintions for LES HD64570 serial controller
 *
 * August 1992, Greg Stovall
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_les_hd64570.h,v $
 * Revision 3.3.62.3  1996/09/05  01:16:48  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/08/28  12:58:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/04/03  01:59:31  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.3  1995/11/17  17:40:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/07  20:37:11  arothwel
 * CSCdi39913:  BSC Tx failure on Hitachi serial chip at low baud rate.
 * Increase the transmit delay time for bisync, to allow short length
 * frames to actually get onto the wire.
 *
 * Revision 2.2  1995/06/19  06:47:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:36:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct hd64570_instance hd64570_instance_t;
typedef struct qsim_regs qsim_regs_t;
typedef struct hd64570_reg hd64570_reg_t;
typedef struct hd64570_msci_reg hd64570_msci_reg_t;
typedef struct hd64570_dmac_reg hd64570_dmac_reg_t;
typedef struct hd64570_buf_descr hd64570_buf_descr_t;
typedef struct hd64570_buf_ring hd64570_buf_ring_t;
typedef void (*appl_periodic_t)(hwidbtype *);

#if defined(XX) | defined(SIERRA)
#include "if_c4000_hd64570.h"
#else   /* c3000 */
#include "if_c3000_hd64570.h"
#endif

#define HD64570_BUF_SIZE (((MAX_RING+1) * sizeof(hd64570_buf_descr_t)) + 16)
#define HD64570_BUF_2K_ALIGN 2048

/*
 * Half duplex states
 */
#define HDX64570_NULL        	        0
#define HDX64570_DCE_RX_START    	1
#define HDX64570_DCE_TX	        	2
#define HDX64570_DTE_TX_START    	3
#define HDX64570_DTE_TX		        4
#define HDX64570_DTE_TX_SENDING	        5
#define HDX64570_DTE_TX_DMA_DONE	6
#define HDX64570_DTE_TX_TRANSMITING	7
#define HDX64570_DTE_TX_TRANSMIT_DONE	8

/*
 * Fixed delay between transition from the TX_DMA_DONE
 * to TX_TRANSMITING states above. Approximately 2 ms.
 * We double this for bisync. That's because bisync sometimes
 * runs at low speeds, shipping short frames; and the data
 * doesn't get onto the wire fast enough, out of the chip's
 * fifo.
 */
#define HDX64570_TRANSMIT_DELAY         300
#define HDX64570_BSC_TRANSMIT_DELAY     600
 
/*
 * Fixed delay between TX_TRANSMITING state above and deassertion
 * of RTS signal. Set to approximately 3 ms.
 */
#define HDX64570_RTS_DROP_DELAY         450

/*
 * HD64570 NOTE:  The registers on the SCA are defined as bytes.  Because the
 * Dbus on the XX will only allow 16 bit accesses , the registers are accessed
 * as 16 bit values.  This means we access 2 registers at once.  There are,
 * however, 2 registers on the Hitachi that MUST be accessed in byte mode, 
 * DMER and PCR.  We have to do special magic to handle this.
 */

/*
 * Idle patterns
 */
#define HD64570_IDLE_FLAG     0x7E
#define HD64570_IDLE_MARK     0xFF
#define HD64570_BSC_IDLE_PATTERN  0xFF

/*
 * Low-Power Register (LPR) Defs
 */
#define HD64570_LPR_IOSTP	0x0001	/* Enter Power Stop mode */

/*
 * Interrupt Control Register (ITCR)
 */
#define HD64570_ITCR_VOS	0x0010	/* Select Int Modified reg */
#define HD64570_ITCR_IAK0	0x0020	/* Single ACK cycle */
#define HD64570_ITCR_IAK1	0x0040	/* Double ACK cycle */
#define HD64570_ITCR_IPC	0x0080	/* Reverse Int Priority */

/*
 * Interrupt Status Resgister 0 (ISR0)
 */
#define HD64570_ISR0_RXRDY0	0x0001	/* MSCI chan 0 RXRDY Int */
#define HD64570_ISR0_TXRDY0	0x0002	/* MSCI chan 0 TXRDY Int */
#define HD64570_ISR0_RXINT0	0x0004	/* MSCI chan 0 RXINT Int */
#define HD64570_ISR0_TXINT0	0x0008	/* MSCI chan 0 TXINT Int */
#define HD64570_ISR0_RXRDY1	0x0010	/* MSCI chan 1 RXRDY Int */
#define HD64570_ISR0_TXRDY1	0x0020	/* MSCI chan 1 TXRDY Int */
#define HD64570_ISR0_RXINT1	0x0040	/* MSCI chan 1 RXINT Int */
#define HD64570_ISR0_TXINT1	0x0080	/* MSCI chan 1 TXINT Int */

/*
 * Interrupt Status Register 1 (ISR1)
 */
#define HD64570_ISR1_DMIA0	0x0100	/* DMA chan 0 Interrupt A */
#define HD64570_ISR1_DMIB0	0x0200	/* DMA chan 0 Interrupt B */
#define HD64570_ISR1_DMIA1	0x0400	/* DMA chan 1 Interrupt A */
#define HD64570_ISR1_DMIB1	0x0800	/* DMA chan 1 Interrupt B */
#define HD64570_ISR1_DMIA2	0x1000	/* DMA chan 2 Interrupt A */
#define HD64570_ISR1_DMIB2	0x2000	/* DMA chan 2 Interrupt B */
#define HD64570_ISR1_DMIA3	0x4000	/* DMA chan 3 Interrupt A */
#define HD64570_ISR1_DMIB3	0x8000	/* DMA chan 3 Interrupt B */

/*
 * Interrupt Status Register 2 (ISR2)
 */
#define HD64570_ISR2_T0IRQ	0x0010	/* Timer Chan 0 Interrupt */
#define HD64570_ISR2_T1IRQ	0x0020	/* Timer Chan 1 Interrupt */
#define HD64570_ISR2_T2IRQ	0x0040	/* Timer Chan 2 Interrupt */
#define HD64570_ISR2_T3IRQ	0x0080	/* Timer Chan 3 Interrupt */

/*
 * Interrupt Enable Register 0 (IER0)
 */
#define HD64570_IER0_RXRDY0E	0x0001	/* MSCI chan 0 RXRDY Int Enable */
#define HD64570_IER0_TXRDY0E	0x0002	/* MSCI chan 0 TXRDY Int Enable */
#define HD64570_IER0_RXINT0E	0x0004	/* MSCI chan 0 RXINT Int Enable */
#define HD64570_IER0_TXINT0E	0x0008	/* MSCI chan 0 TXINT Int Enable */
#define HD64570_IER0_RXRDY1E	0x0010	/* MSCI chan 1 RXRDY Int Enable */
#define HD64570_IER0_TXRDY1E	0x0020	/* MSCI chan 1 TXRDY Int Enable */
#define HD64570_IER0_RXINT1E	0x0040	/* MSCI chan 1 RXINT Int Enable */
#define HD64570_IER0_TXINT1E	0x0080	/* MSCI chan 1 TXINT Int Enable */

/* 
 * Interrupt Enable Register 1 (IER1)
 */
#define HD64570_IER1_DMIA0E	0x0100	/* DMA chan 0 Interrupt A Enable */
#define HD64570_IER1_DMIB0E	0x0200	/* DMA chan 0 Interrupt B Enable */
#define HD64570_IER1_DMIA1E	0x0400	/* DMA chan 1 Interrupt A Enable */
#define HD64570_IER1_DMIB1E	0x0800	/* DMA chan 1 Interrupt B Enable */
#define HD64570_IER1_DMIA2E	0x1000	/* DMA chan 2 Interrupt A Enable */
#define HD64570_IER1_DMIB2E	0x2000	/* DMA chan 2 Interrupt B Enable */
#define HD64570_IER1_DMIA3E	0x4000	/* DMA chan 3 Interrupt A Enable */
#define HD64570_IER1_DMIB3E	0x8000	/* DMA chan 3 Interrupt B Enable */

/*
 * Interrupt Enable Register 2 (IER2)
 */
#define HD64570_IER2_T0IRQE	0x0010	/* Timer Chan 0 Interrupt Enable */
#define HD64570_IER2_T1IRQE	0x0020	/* Timer Chan 1 Interrupt Enable */
#define HD64570_IER2_T2IRQE	0x0040	/* Timer Chan 2 Interrupt Enable */
#define HD64570_IER2_T3IRQE	0x0080	/* Timer Chan 3 Interrupt Enable */

/*
 * MSCI Mode Register 0 (MD0)
 */
#define HD64570_MD0_CRC0	0x0001	/* Initialize to all 1's */
#define HD64570_MD0_CRC1	0x0002	/* CRC-CCITT mode */
#define HD64570_MD0_CRCCC	0x0004	/* CRC Enable */
#define HD64570_MD0_AUTO	0x0010	/* Auto-Enable Set */
#define HD64570_MD0_HDLC	0x0080	/* Bit-sync HDLC Mode */
#define HD64570_MD0_BiSYNC      0x0040  /* Byte-sync Bi-sync Mode */

/*
 * MSCI Mode Register 1 (MD1)
 */
#define HD64570_MD1_SINGLE_1	0x4000	/* Single Address 1 */
#define HD64570_MD1_SINGLE_2	0x8000	/* Single Address 2 */
#define HD64570_MD1_DUAL	0xC000	/* Dual Address */

/*
 * MSCI Mode Register 2 (MD2)
 */
#define HD64570_MD2_ECHO	0x0001	/* Auto Echo */
#define HD64570_MD2_LOOP	0x0003	/* Local Loop Back */
#define HD64570_MD2_ADPLLX16	0x0008	/* ADPLL Clock x 16 */
#define HD64570_MD2_ADPLLX32	0x0010	/* ADPLL Clock x 32 */
#define HD64570_MD2_NRZI	0x0020	/* NRZI */
#define HD64570_MD2_NRZFM	0x0080	/* FM Mode */

/*
 * MSCI Control Register (CTL)
 */
#define HD64570_CTL_RTS		0x0100	/* Set RTS High */
#define HD64570_CTL_GOP		0x0200	/* Enable Go Active On Poll */
#define HD64570_CTL_SYNCLD      0x0400  /* Enable storing SYNs in Rx buffer */
#define HD64570_CTL_IDLC	0x1000	/* TX Idle Pattern */
#define HD64570_CTL_UDRNC	0x2000	/* Enter Idle after FCS and Flag */

/*
 * MSCI RX Clock Source Register (RXS)
 */
#define HD64570_RXS_BAUD_2	0x0001	/* Divide Clock by 2 */
#define HD64570_RXS_BAUD_4	0x0002	/* Divide Clock by 4 */
#define HD64570_RXS_BAUD_8	0x0003	/* Divide Clock by 8 */
#define HD64570_RXS_BAUD_16	0x0004	/* Divide Clock by 16 */
#define HD64570_RXS_BAUD_32	0x0005	/* Divide Clock by 32 */
#define HD64570_RXS_BAUD_64	0x0006	/* Divide Clock by 64 */
#define HD64570_RXS_BAUD_128	0x0007	/* Divide Clock by 128 */
#define HD64570_RXS_BAUD_256	0x0008	/* Divide Clock by 256 */
#define HD64570_RXS_BAUD_512	0x0009	/* Divide Clock by 512 */
#define HD64570_RXS_RXC         0x0000  /* RXC Line Input */
#define HD64570_RXS_RXC_SUP	0x0020	/* RXC Line Input (Noise Suppression)*/
#define HD64570_RXS_BRG		0x0040	/* BRG Output */
#define HD64570_RXS_ADPLL_BRG	0x0060	/* ADPLL Output, BRG Output */
#define HD64570_RXS_ADPLL_RXC	0x0070	/* ADPLL Output, RXC Input */

/*
 * MSCI TX Clock Source Register (TXS)
 */
#define HD64570_TXS_BAUD_2	0x0100	/* Divide Clock by 2 */
#define HD64570_TXS_BAUD_4	0x0200	/* Divide Clock by 4 */
#define HD64570_TXS_BAUD_8	0x0300	/* Divide Clock by 8 */
#define HD64570_TXS_BAUD_16	0x0400	/* Divide Clock by 16 */
#define HD64570_TXS_BAUD_32	0x0500	/* Divide Clock by 32 */
#define HD64570_TXS_BAUD_64	0x0600	/* Divide Clock by 64 */
#define HD64570_TXS_BAUD_128	0x0700	/* Divide Clock by 128 */
#define HD64570_TXS_BAUD_256	0x0800	/* Divide Clock by 256 */
#define HD64570_TXS_BAUD_512	0x0900	/* Divide Clock by 512 */
#define HD64570_TXS_TXC         0x0000  /* TXC Line Input */
#define HD64570_TXS_BRG		0x4000	/* BRG Output */
#define HD64570_TXS_RX_Clock	0x6000	/* RX Clock */

/*
 * MSCI Command Register (CMD)
 */
#define HD64570_CMD_TX_RST	0x0001	/* TX Reset */
#define HD64570_CMD_TX_ENB	0x0002	/* TX Enable */
#define HD64570_CMD_TX_DIS	0x0003	/* TX Disable */
#define HD64570_CMD_TX_CRC_INIT	0x0004	/* TX CRC Initialization */
#define HD64570_CMD_TX_CRC_EX	0x0005	/* TX CRC Calculation Exclusion */
#define HD64570_CMD_TX_EOM	0x0006	/* TX End-of-message */
#define HD64570_CMD_TX_ABORT	0x0007	/* TX Abort Transmossion */
#define HD64570_CMD_TX_MP	0x0008	/* TX MP Bit On */
#define HD64570_CMD_TX_BUF_CLR	0x0009	/* TX Buffer Clear */
#define HD64570_CMD_RX_RST	0x0011	/* RX Reset */
#define HD64570_CMD_RX_ENB	0x0012	/* RX Enable */
#define HD64570_CMD_RX_DIS	0x0013	/* RX Disable */
#define HD64570_CMD_RX_CRC_INIT	0x0014	/* RX CRC Initialization */
#define HD64570_CMD_RX_REJ	0x0015	/* RX Message Reject */
#define HD64570_CMD_RX_MP	0x0016	/* RX Search MP Bit */
#define HD64570_CMD_RX_CRC_EX	0x0017	/* RX CRC Calculation Exclusion */
#define HD64570_CMD_RX_CRC_FOR	0x0018	/* Forcing RX CRC Calculation */
#define HD64570_CMD_CHAN_RST	0x0021	/* Channel Reset */
#define HD64570_CMD_SEARCH	0x0031	/* Enter Search Mode */

/*
 * MSCI Status Register 0 (ST0)
 */
#define HD64570_ST0_RX_RDY	0x0001	/* RX Ready */
#define HD64570_ST0_TX_RDY	0x0002	/* TX Ready */
#define HD64570_ST0_RX_INT	0x0040	/* RX Interrupt */
#define HD64570_ST0_TX_INT	0x0080	/* TX Interrupt */

/*
 * MSCI Status Register 1 (ST1)
 */
#define HD64570_ST1_IDLD	0x0100	/* Idle Start Detected */
#define HD64570_ST1_ABORT_GA	0x0200	/* Abort Start, GA Detected */
#define HD64570_ST1_CDCD	0x0400	/* DCD Changed */
#define HD64570_ST1_CCTS	0x0800	/* CTS Changed */
#define HD64570_ST1_FLGD	0x1000	/* Flag Detected */
#define HD64570_ST1_IDL		0x4000	/* TX Idle */
#define HD64570_ST1_UDRN	0x8000	/* Underrun Detected */

/*
 * MSCI Status Register 2 (ST2) 
 */
#define HD64570_ST2_CRCE	0x0004	/* CRC Error Detected */
#define HD64570_ST2_OVRN	0x0008	/* Overrun Detected */
#define HD64570_ST2_RBIT	0x0010	/* Residual Bit Frame Detected */
#define HD64570_ST2_ABT		0x0020	/* Frame with Abort End Detected */
#define HD64570_ST2_SHRT	0x0040	/* Short Frame Detected */
#define HD64570_ST2_EOM		0x0080	/* RX Frame End Detect */

/*
 * MSCI Status Register 3 (ST3)
 */
#define HD64570_ST3_RXENBL	0x0100	/* RX Enable */
#define HD64570_ST3_TXENBL	0x0200	/* TX Enable */
#define HD64570_ST3_DCD		0x0400	/* DCD Level High */
#define HD64570_ST3_CTS		0x0800	/* CTS Level High */
#define HD64570_ST3_SRCH	0x1000	/* ADPLL Search Mode */
#define HD64570_ST3_SLOOP	0x2000	/* TX MSCI Data On Loop */

/*
 * MSCI Frame Status Register (FST)
 */
#define HD64570_FST_CRCEF	0x0004	/* CRC Error Detected */
#define HD64570_FST_OVRNF	0x0008	/* Overrun Detected */
#define HD64570_FST_RBITF	0x0010	/* Residual Bit Frame Detected */
#define HD64570_FST_ABTF	0x0020	/* Frame with Abort End Detected */
#define HD64570_FST_SHRTF	0x0040	/* Short Frame Detected */
#define HD64570_FST_EOMF	0x0080	/* RX Frame End Detect */

/*
 * MSCI Interrupt Enable Register 0 (IE0)
 */
#define HD64570_IE0_RXRDYE	0x0001	/* RXRDY Interrupt Enable */
#define HD64570_IE0_TXRDYE	0x0002	/* TXRDY Interrupt Enable */
#define HD64570_IE0_RXINTE	0x0040	/* RXINT Interrupt Enable */
#define HD64570_IE0_TXINTE	0x0080	/* TXINT Interrupt Enable */

/*
 * MSCI Interrupt Enable Register 1 (IE1)
 */
#define HD64570_IE1_IDLDE	0x0100	/* Idle Interrupt Enable */
#define HD64570_IE1_ABORT_GAE	0x0200	/* Abort Start, GA Detected Int Enb */
#define HD64570_IE1_CDCDE	0x0400	/* DCD Changed Interrupt Enable */
#define HD64570_IE1_CCTSE	0x0800	/* CTS Changed Interrupt Enable */
#define HD64570_IE1_FLGDE	0x1000	/* Flag Detected Interrupt Enable */
#define HD64570_IE1_IDLE	0x4000	/* TX Idle Interrupt Enable */
#define HD64570_IE1_UDRNE	0x8000	/* Underrun Detected Int Enable */

/*
 * MSCI Interrupt Enable Register 2 (IE2)
 */
#define HD64570_IE2_CRCEE	0x0004	/* CRC Error Interrupt Enable */
#define HD64570_IE2_OVRNE	0x0008	/* Overrun Interrupt Enable */
#define HD64570_IE2_RBITE	0x0010	/* Residual Bit Frame Int Enable */
#define HD64570_IE2_ABTE	0x0020	/* Frame with Abort End Int Enable */
#define HD64570_IE2_SHRTE	0x0040	/* Short Frame Interrupt Enable */
#define HD64570_IE2_EOME	0x0080	/* RX Frame End Interrupt Enable */

/*
 * MSCI Frame Interrupt Enable Register (FIE)
 */
#define HD64570_FIE_EOMFE	0x8000	/* EOMF Interrupt Enable */

/*
 * MSCI Current Status Register 0 (CST0)
 */
#define HD64570_CST0_CDE0	0x0001	/* Data Exists */
#define HD64570_CST0_CRCEC0	0x0004	/* Current CRC Error Detected */
#define HD64570_CST0_OVRNC0	0x0008	/* Current Overrun Detected */
#define HD64570_CST0_RBITC0	0x0010	/* Curr Residual Bit Frame Detected */
#define HD64570_CST0_ABTC0	0x0020	/* Curr Frame Abort End Detected */
#define HD64570_CST0_SHRTC0	0x0040	/* Current Short Frame Detected */
#define HD64570_CST0_EOMC0	0x0080	/* Current RX Frame End Detect */

/*
 * MSCI Current Status Register 1 (CST1)
 */
#define HD64570_CST1_CDE1	0x0100	/* Data Exists */
#define HD64570_CST1_CRCEC1	0x0400	/* Current CRC Error Detected */
#define HD64570_CST1_OVRNC1	0x0800	/* Current Overrun Detected */
#define HD64570_CST1_RBITC1	0x1000	/* Curr Residual Bit Frame Detected */
#define HD64570_CST1_ABTC1	0x2000	/* Curr Frame Abort End Detected */
#define HD64570_CST1_SHRTC1	0x4000	/* Current Short Frame Detected */
#define HD64570_CST1_EOMC1	0x8000	/* Current RX Frame End Detect */

/*
 * DMA Status Register (DSR)
 */
#define HD64570_DSR_DWE		0x0001	/* DE Bit Write Disable */
#define HD64570_DSR_DE		0x0002	/* DMA Enable */
#define HD64570_DSR_COF		0x0010	/* Counter Overflow */
#define HD64570_DSR_BOF		0x0020	/* Buffer Overflow */
#define HD64570_DSR_EOM		0x0040	/* Frame Transfer Complete */
#define HD64570_DSR_EOT		0x0080	/* End of DMA Transfer */

/*
 * DMA Mode Register (DMR)
 */
#define HD64570_DMR_CNTE	0x0200	/* FCT Enable */
#define HD64570_DMR_NF		0x0400	/* Multi-Frame Mode */
#define HD64570_DMR_TMOD	0x1000	/* Chained-Block Transfer Mode */
#define DMR_NF_TMOD             HD64570_DMR_NF | HD64570_DMR_TMOD

/*
 * DMA Interrupt Enable Register (DIR)
 */
#define HD64570_DIR_COFE	0x0010	/* Counter Overflow Interrupt Enable */
#define HD64570_DIR_BOFE	0x0020	/* Buffer Over/Underflow Int Enable */
#define HD64570_DIR_EOME	0x0040	/* Frame Transfer End Int Enable */
#define HD64570_DIR_EOTE	0x0080	/* Transfer End Interrupt Enable */
#define DIR_BOFE_EOME_EOTE      HD64570_DIR_BOFE | HD64570_DIR_EOME | HD64570_DIR_EOTE

/*
 * DMA Command Register (DCR)
 */
#define HD64570_DCR_ABORT	0x0100	/* Software Abort */
#define HD64570_DCR_CLR_FCT	0x0200	/* FCT Interrupt Cleared */

/*
 * DMA Priority Control Register (PCR)   (NOTE: 8 BIT ACCESS ONLY)
 */
#define HD64570_PCR_CCC		0x08	/* Change Channel on No DMA Request */
#define HD64570_PCR_BRC		0x10	/* Release Bus After One Transfer */
#define HD64570_PCR_CH0         0x00  /* MSCI 0 > MSCI 1 */
#define HD64570_PCR_CH1         0x01  /* MSCI 1 > MSCI 0 */
#define HD64570_PCR_RX          0x02  /* RX > TX */
#define HD64570_PCR_TX          0x03  /* TX > RX */
#define HD64570_PCR_RR          0x04  /* Round robin */

/*
 * DMA Master Enable Register (DMER)  (NOTE: 8 BIT ACCESS ONLY)
 */
#define HD64570_DMER_DME	0x80	/* DMA Master Enable */

/*
 * RX Buffer Descriptor Status
 */
#define HD64570_RX_BUF_EOM	0x80	/* End of Message */
#define HD64570_RX_BUF_SF	0x40	/* Short Frame */
#define HD64570_RX_BUF_ABORT	0x20	/* Abort */
#define HD64570_RX_BUF_RB	0x10	/* Residual Bit */
#define HD64570_RX_BUF_OR	0x08	/* Overrun */
#define HD64570_RX_BUF_CRC	0x04	/* CRC Error */

/*
 * TX Buffer Descriptor Status
 */
#define HD64570_TX_BUF_EOM	0x80	/* End of Message */
#define HD64570_TX_BUF_EOT	0x01	/* End of Transmission */

/*
 * Timer Control/Status Register (TCSR)
 */
#define HD64570_TCSR_CMF	0x0080	/* Compare Match Flag */
#define HD64570_TCSR_ECMI	0x0040	/* CMF Interrupt Enable */
#define HD64570_TCSR_TME	0x0010	/* Timer Enable */

/*
 * Timer constants
 */
#define HD64570_TIMER_PRE	0x0300	/* Value to load into pre-scaler */

/*
 * MSCI registers
 */
struct hd64570_msci_reg {
    volatile ushort trb;      /* MSCI chan TX/RX buffer register */
    volatile ushort st1_st0;  /* MSCI chan status registers 1 and 0 */
    volatile ushort st3_st2;  /* MSCI chan status register 3  and 2 */
    volatile ushort fst;      /* MSCI chan frame status regr (MSB reserved) */
    volatile ushort ie1_ie0;  /* MSCI chan interrupt enable reg 1 and 0 */
    volatile ushort fie_ie2;  /* MSCI ch frame int enable & int enable reg 2*/
    volatile ushort cmd;      /* MSCI chan command register (MSB reserved) */
    volatile ushort md1_md0;  /* MSCI chan mode register 1 and 0 */
    volatile ushort ctl_md2;  /* MSCI ch control register & mode register 2 */
    volatile ushort sa1_sa0;  /* MSCI chan sync/address register 1 and 0 */
    volatile ushort tmc_idl;  /* MSCI chan time constant & idle pattern reg */
    volatile ushort txs_rxs;  /* MSCI chan TX and RX clock source registers */
    volatile ushort trc1_trc0;/* MSCI chan RX ready control register 1 and 0 */
    volatile ushort rrc;      /* MSCI ch RX ready control reg(MSB reserved) */
    volatile ushort cst1_cst0;/* MSCI chan current status register 1 and 0 */
    ushort res13;             /* 0x3e-0x3f - reserved */
};

/*
 * DMAC registers
 */
struct hd64570_dmac_reg {
    volatile ushort bar;     /* DMA chan buffer address register */
    volatile ushort barb;    /* DMA ch buffer addr reg base (MSB reserved) */
    volatile ushort re23;    /* reserved */
    volatile ushort cpb;     /* DMA chan chain pointer base (MSB reserved) */
    volatile ushort cda;     /* DMA chan current descriptor addr */
    volatile ushort eda;     /* DMA chan error descriptor addr */
    volatile ushort bfl;     /* DMA chan RX buffer length */
    volatile ushort bcr;     /* DMA chan byte count register */
    volatile ushort dmr_dsr; /* DMA chan DMA mode & status register */
    volatile ushort fct;     /* DMA ch end-of-frame int count (LSB reserved) */
    volatile ushort dcr_dir; /* DMA chan DMA command & interrupt enable reg */
    ushort res2[5];          /* reserved */
};

/*
 * HD64570 SCA memory map 
 */
struct hd64570_reg {
    /* Low-power Mode Control Registers */
    volatile ushort lpr;              /* Low power register (MSB reserved) */

    /* Wait Controller Registers */
    volatile ushort wait_pabr1_pabr0; /* physical addr boundry reg 1 and 0 */
    volatile ushort wait_wcrm_wcrl;   /* wait control reg mid and low */
    volatile ushort wait_wcrh;        /* wait cntrl reg high (MSB reserved) */

    /*
     * DMAC Registers Common to Channels 0 to 3
     * NOTE: 'dmer' and 'pcr' registers can only be accesed as 8 bit
     *       quantities, they should be accessed via hd64570_dmer_pcr_write()
     *       routine.
     */
    volatile ushort dmer_pcr;  /* DMA master enable register & 
                                  priority control register */
    ushort res3[3];            /* 0x0a-0x0f - reserved */

    /* Interrupt Control Registers */
    volatile ushort isr1_isr0; /* Interrupt status register 1 and 0 */
    volatile ushort isr2;      /* Interrupt status register 2 (MSB reserved) */
    volatile ushort ier1_ier0; /* Interrupt enable register 1 and 0 */
    volatile ushort ier2;      /* Interrupt enable register 2 (MSB reserved) */
    volatile ushort itcr;      /* Interrupt control Register (MSB reserved) */
    volatile ushort ivr;       /* Interrupt vector register (MSB reserved) */
    volatile ushort imvr;      /* Interrupt modified register (MSB reserved) */
    volatile ushort res9;      /* 0x1e-0x1f - reserved */

    /* MSCI Registers */
    hd64570_msci_reg_t msci_ch0;
    hd64570_msci_reg_t msci_ch1;

    /* Timer Registers */
    volatile ushort timer_0_tcnt;     /* 0x60-0x61 - ch 0 timer up-counter */
    volatile ushort timer_0_tconr;    /* 0x62-0x63 - ch 0 timer constant reg */
    volatile ushort timer_0_tepr_tcsr;/* ch 0 timer expand prescale & 
                                         control/status reg */
    ushort res18;                     /* 0x66-0x67 - reserved */
    volatile ushort timer_1_tcnt;     /* 0x68-0x89 - ch 1 timer up-counter */
    volatile ushort timer_1_tconr;    /* 0x6a-0x6b - ch 1 timer constant reg */
    volatile ushort timer_1_tepr_tcsr;/* ch 1 timer expand prescale reg &
                                         control/status reg */
    volatile ushort res19;            /* 0x6e-0x6f reserved */
    volatile ushort timer_2_tcnt;     /* 0x70-0x71 - ch 2 timer up-counter */
    volatile ushort timer_2_tconr;    /* 0x72-0x73 - cn 2 timer constant reg */
    volatile ushort timer_2_tepr_tcsr;/* ch 2 timer expand prescale reg & 
                                         control/status reg */
    volatile ushort res20;            /* 0x76-0x77 reserved */
    volatile ushort timer_3_tcnt;     /* 0x78-0x79 - ch 3 timer up-counter */
    volatile ushort timer_3_tconr;    /* 0x7a0x7b - ch 3 timer constant reg */
    volatile ushort timer_3_tepr_tcsr;/* ch 3 timer expand prescale reg & 
                                          control/status reg */
    ushort res21;                     /* 0x7e-0x7f reserved */

    /* DMA Registers Provided Separately for Channels 0 to 3 */
    hd64570_dmac_reg_t msci_ch0_rx_dma; /* DMA channel 0 */
    hd64570_dmac_reg_t msci_ch0_tx_dma; /* DMA channel 1 */
    hd64570_dmac_reg_t msci_ch1_rx_dma; /* DMA channel 2 */
    hd64570_dmac_reg_t msci_ch1_tx_dma; /* DMA channel 3 */
};

struct hd64570_buf_descr {
    volatile ushort chain_ptr;            /* Pointer to next descriptor */
    volatile ulong PACKED(buf_ptr);       /* Pointer to data buffer */
    volatile ushort data_len;             /* Data length */
    volatile ushort status;               /* Buffer status */
/*
 * The following are not part of the HD64570 BD, but 
 * are part of the structure to aid in processing 
 */
    ushort short_buf_ptr;        /* short pointer to this buffer */
    hd64570_buf_descr_t *next;   /* Pointer to next */
    paktype *pak;                /* Pointer to paktype *  */
}; 

struct hd64570_buf_ring {
    hd64570_buf_descr_t ring_entry[MAX_RING];
};

typedef struct hd64570_clocktype {
    int rate;
    uchar divider;
    uchar counter;
} hd64570_clocktype;

#define NHD64570_CLOCKS 19
extern const hd64570_clocktype hd64570_clock1[];
extern const hd64570_clocktype hd64570_clock2[];
extern const hd64570_clocktype *hd64570_clock;

#define HD64570_EVEN_BYTE_ACCESS    TRUE
#define HD64570_ODD_BYTE_ACCESS     FALSE
#define HD64570_EVEN(x)       (!((x) & 1))

#define NEXT_READ_PACKET(ds, rmd) { \
		 ds->rx_last = rmd->next; \
		 ds->dmac_rx_reg->eda = rmd->short_buf_ptr;\
		 goto next_RX_packet; \
				  }
				   

/*
 * Prototypes
 */
void hd64570_RX_interrupt(hwidbtype *idb);
void hd64570_RX_HDX_interrupt(hwidbtype *idb);
void hd64570_TX_interrupt(hwidbtype *idb);
void hd64570_TX_HDX_interrupt(hwidbtype *idb);
void hd64570_HDX_txint(hwidbtype *idb);
void hd64570_DMIA_int(hwidbtype *idb);
void hd64570_error_int(hwidbtype *idb);
void hd64570_tx_HDX_start(hwidbtype *idb);
void hd64570_init_idb_common(hwidbtype *idb);
void hd64570_init(hwidbtype *idb);

/*
 * Externs
 */
extern void hd64570_subsys_init(subsystype *subsys);
extern void hd64570_init_config(hwidbtype *idb);
extern void hd64570_enter_transparent_mode(hwidbtype *idb);
extern void hd64570_msci_reset(hwidbtype *idb);
extern void hd64570_set_fifo_limits(hwidbtype *idb);
extern void hd64570_enable_modem_interrupt(hwidbtype *idb);
extern void hd64570_disable_modem_interrupt(hwidbtype *idb);
extern void hd64570_show_cable_state(hwidbtype *idb);
extern void hd64570_setup_int(hwidbtype *idb);
extern boolean hd64570_dte_tx_ok(hd64570_instance_t *ds);
extern boolean hd64570_dte_tx_request(hwidbtype *idb);
extern boolean hd64570_dce_tx_ok(hd64570_instance_t *ds);
extern void hd64570_show_structure_info(hwidbtype *idb);

/* $Id: if_les_common_cd2430.h,v 3.1.2.4 1996/08/10 01:08:35 billw Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_common_cd2430.h,v $
 *------------------------------------------------------------------
 * if_les_common_cd2430.h - Definitions for the Cirrus CL-CD2430 
 *                          controller (common code)
 *
 * April 1996, Jose Hernandez
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_common_cd2430.h,v $
 * Revision 3.1.2.4  1996/08/10  01:08:35  billw
 * CSCdi54603:  make cirrus hardware gotchas more obvious in source code
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/07/04  02:07:11  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/20  16:16:14  widmer
 * CSCdi60890:  Bogus includes cause unnecessary compiles
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:52  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/04/22  21:43:36  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_LES_COMMON_CD2430_H__
#define __IF_LES_COMMON_CD2430_H__

/*
 * CD2430 register layout.
 * The names are taken from the Cirrus spec sheet.
 */


/*
 * CMR bits
 */
#define CMR_RXDMA (0x80)        /* Receive using DMA (else interrupt) */
#define CMR_TXDMA (0x40)        /*  ...transmit */
#define CMR_MODE_HDLC (0x0)     /* Set mode for sync HDLC */
#define CMR_MODE_BISYNC (0x1)   /* Set mode for Bisync */
#define CMR_MODE_ASYNC (0x2)    /* Set mode for async */
#define CMR_MODE_PPP (0x4)      /*  ...for async PPP */
#define CMR_MODE_SLIP (0x5)     /*  ...for async SLIP */
#define CMR_MODE_MNP4 (0x6)     /*  ...for MNP4 */
#define CMR_MODE_AUTO (0x7)     /*  ...for autobaud */

/*
 * COR1 bits
 */
#define COR1_PARITY_ODD (0x80)          /* Odd parity (else even) */
#define COR1_PARITY_NONE (0x0)          /* No parity */
#define COR1_PARITY_FORCE (0x20)        /* Force parity (odd=1, even=0) */
#define COR1_PARITY_NORM (0x40)         /* Normal parity */
#define COR1_PARITY_IGN (0x10)          /* Ignore parity on received */
#define COR1_5BIT (0x04)                /* Char length--5 bits */
#define COR1_6BIT (0x05)                /*  ...6 */
#define COR1_7BIT (0x06)                /*  ...7 */
#define COR1_8BIT (0x07)                /*  ...8 */
#define COR1_MAX_DELAY_FLAGS (0xF)  /* Max no of HDLC flags preceding a frame */

/*
 * COR2 bits
 **** REFERENCES TO RTS AND DTR are SWAPPED for HW flow ctrl ******
 * See comments in ts/tty_cd2430.c
 */
#define COR2_IXANY (0x80)       /* Any char serves as XON (else only XON) */
#define COR2_TXINBAND (0x40)    /* Transmit inband flow control enable */
#define COR2_ETC (0x20)         /* Embedded transmitter command enable */
#define COR2_LOOP (0x8)         /* Enable remote loopback */
#define COR2_RTS (0x4)          /* Auto-RTS */
#define COR2_CTS (0x2)          /* Auto-CTS */
#define COR2_DSR (0x1)          /* Auto-DSR */

#define COR2_LRC        (0x80)  /* LRC used for BCC (else CRC) */
#define COR2_APPEND_BCC (0x40)  /* BCC left at EOF (else stripped) */
#define COR2_EBCDIC     (0x20)  /* EBCDIC char-set used (else ASCII) */
#define COR2_CRC_NORMAL (0x10)  /* ie CRC-16 (else inverted, ie CRC V.41) */
#define COR2_NO_STRIP   (0x08)  /* no strip SYN-SYN or DLE-SYN */

/*
 * Values which can be sent once COR2_ETC is set
 */
enum ETC {
    ETC_ESC = 0x0,              /* Start escape sequence */
    ETC_BREAK = 0x81,           /* Send break */
    ETC_DELAY = 0x82,           /* Insert delay */
    ETC_ENDBREAK = 0x83         /* Stop sending break */
};
#define CD2430BREAKDATA 8       /* ETC cmd Data bytes needed to send BRK */

/*
 * COR3 bits
 */
#define COR3_ESCDE (0x80)       /* Extended special char detect enable */
#define COR3_RANGEDE (0x40)     /* Range detect enable */
#define COR3_FCT (0x20)         /* Flow control transparency */
#define COR3_SCDE (0x10)        /* Special char detect enable */
#define COR3_SCSTRIP (0x8)      /* Strip high bit for special char check */
#define COR3_1STOP (0x2)        /* 1 stop bit */
#define COR3_15STOP (0x3)       /*  ...1.5 */
#define COR3_2STOP (0x4)        /*  ...2 */
#define COR3_TXGEN (0x10)       /* PPP: generate CRC on transmit */
#define COR3_RXCK (0x20)        /*  ...check received CRC */
#define COR3_CRCPASS (0x40)     /*  ...pass CRC in packet (else strip) */
#define COR3_1PAD (0x1)         /* 1 PAD character sent */
#define COR3_2PAD (0x2)         /* 2 PAD character sent */
#define COR3_3PAD (0x3)         /* 3 PAD character sent */
#define COR3_4PAD (0x4)         /* 4 PAD character sent */
#define COR3_NRZ (0x40)         /* Select pad char for NRZ encoding */
#define COR3_NRZI (0x00)        /* Select pad char for NRZI encoding */

#define COR3_PRESYN_PAD  (0x80) /* send PAD before SYN */
#define COR3_PAD_55      (0x40) /* (else send AA) */
#define COR3_FCS_ZERO    (0x20) /* (else set all 1's) */
#define COR3_DISABLE_FCS (0x10) /* (else validates CRC) */
#define COR3_IDLE_MARK   (0x08) /* (else idle SYN) */

/*
 * COR4 bits
 */
#define COR4_DSRZD (0x80)       /* Detect 1->0 DSR transition */
#define COR4_CDZD (0x40)        /*  ...CD */
#define COR4_CTSZD (0x20)       /*  ...CTS */
#define COR4_FIFOMASK (0xF)     /* FIFO threshold value */

/*
 * Calculation for threshold based on target latency (thanks, kph!)
 * Since we're using DMA, we can assume a somewhat better latency than
 * byte-at-a-time interrupt handling.
 */
#define TARGET_INTERRUPT_LATENCY 250 /* Maximum interrupt latency in us */
/* #define COR4_RXTHRESH(baud) (15-((baud)/(10000000/TARGET_INTERRUPT_LATENCY)))
 */

/*
 * Nyet.  This is hard-coded, per advice from Cirrus.  The effective range
 * of buffering is much lower than expected, and their advice is to run
 * with the threshold half way between max and min values--8.
 */
#define COR4_RXTHRESH(baud) (8)

/*
 * COR5 bits
 */
#define COR5_DSROD (0x80)       /* Detect 0->1 DSR transition */
#define COR5_CDOD (0x40)        /*  ...CD */
#define COR5_CTSOD (0x20)       /*  ...CTS */
#define COR5_RXMASK (0xF)       /* Receive flow control threshold */

/*
 * COR6 bits
 */
#define COR6_IGNCR (0x80)       /* Ignore CR on input */
#define COR6_ICRNL (0x40)       /*  ...Map CR -> NL */
#define COR6_INLCR (0x20)       /*  ...NL -> CR */
#define COR6_IGNBRK (0x10)      /* Ignore input break */
#define COR6_NOBRKINT (0x8)     /* Inhibit break interrupt */
#define COR6_PARMRK (0x4)       /* Mark parity */
#define COR6_INPCK (0x2)        /* Check parity */
#define COR6_PARINT (0x1)       /* Interrupt on bad parity */

/*
 * COR7 bits
 */
#define COR7_ISTRIP (0x80)      /* Strip high bit */
#define COR7_LNE (0x40)         /* Enable literal-next quotiing */
#define COR7_FCERR (0x20)       /* Flow control on error chars */
#define COR7_ONLCR (0x2)        /* Map NL->CR on output */
#define COR7_OCRNL (0x1)        /*  ...CR->NL */

/*
 * RCOR bits
 */
#define RCOR_TLVAL (0x80)       /* Transmit line value */
#define RCOR_DPLL (0x20)        /* DPLL enable */
#define RCOR_NRZ (0x0)          /*  ...NRZ */
#define RCOR_NRZI (0x8)         /*  ...NRZI */
#define RCOR_MANCH (0x10)       /*  ...Manchester */
#define RCOR_CLK0 (0)           /* Receive clock source--clock 0 */
#define RCOR_CLK1 (1)           /*  ...1 */
#define RCOR_CLK2 (2)           /*  ...2 */
#define RCOR_CLK3 (3)           /*  ...3 */
#define RCOR_CLK4 (4)           /*  ...4 */
#define RCOR_EXT (6)            /*  ...external */

/*
 * TCOR bits
 */
#define TCOR_CLK0 (0)           /* Transmit clock source--clock 0 */
#define TCOR_CLK1 (0x20)        /*  ...1 */
#define TCOR_CLK2 (0x40)        /*  ...2 */
#define TCOR_CLK3 (0x60)        /*  ...3 */
#define TCOR_CLK4 (0x80)        /*  ...4 */
#define TCOR_EXT (0xc0)         /*  ...external */
#define TCOR_RX (0xe0)          /*  ...receive clock */
#define TCOR_EXT1X (0x8)        /* x1 external clock */
#define TCOR_LLM (0x2)          /* Local loopback mode */

/*
 * CCR bits
 */
#define CCR_SET0 (0)            /* Select bit set 0 */
#define CCR_CLRCH (0x40)        /* Clear channel */
#define CCR_INITCH (0x20)       /* Init channel */
#define CCR_RESET (0x10)        /* Reset all */
#define CCR_ENTX (0x8)          /* Enable transmitter */
#define CCR_DISTX (0x4)         /*  ...disable */
#define CCR_ENRX (0x2)          /* Enable receiver */
#define CCR_DISRX (0x1)         /*  ...disable */

#define CCR_SET1 (0x80)         /* Select bit set 1 */
#define CCR_CLRT1 (0x40)        /* Clear timer 1 */
#define CCR_CLRT2 (0x20)        /*  ...2 */
#define CCR_CLRRCV (0x10)       /* Clear receiver command */
#define CCR_CLRTX (0x08)        /*  ...transmitter */

/*
 * STCR bits
 */
#define STCR_ABORTTX (0x40)     /* Abort transmit */
#define STCR_APNDCOMPLT (0x20)  /* Append mode complete */
#define STCR_SNDSPEC (0x8)      /* Send special char: */
#define STCR_SPC1 (1)           /*  ...special char 1 */
#define STCR_SPC2 (2)           /*  ...special char 2 */
#define STCR_SPC3 (3)           /*  ...special char 3 */
#define STCR_SPC4 (4)           /*  ...special char 4 */
#define STCR_PPPSNDSPEC (0x4)   /* Send special char in async PPP mode */
#define STCR_PPPXON (0x2)       /*  ...send XON (0x13) */
#define STCR_PPPXOFF (0x1)      /*  ...send XOFF (0x11) */

/*
 * CSR bits
 */
#define CSR_RXEN (0x80)         /* Receiver enabled */
#define CSR_RXFLOFF (0x40)      /* Receive flow off requested */
#define CSR_RXFLON (0x20)       /* Receive flow on requested */
#define CSR_RXIDLE (0x10)       /* Receiver idle */
#define CSR_TXEN (0x8)          /* Transmit enabled */
#define CSR_TXFLOFF (0x4)       /* Transmit flow off seen */
#define CSR_TXFLON (0x2)        /* Transmit flow on seen */
#define CSR_TXIDLE (0x1)        /* Transmitter idle */

/*
 * MSVR bits
 **** REFERENCES TO RTS AND DTR are SWAPPED for HW flow ctrl ******
 * See comments in ts/tty_cd2430.c
 */
#define MSVR_DSR (0x80)         /* Value of DSR */
#define MSVR_CD (0x40)          /*  ...CD */
#define MSVR_CTS (0x20)         /*  ...CTS */
#define MSVR_DTROPT (0x10)      /* Output clock on DTR pin */
#define MSVR_PORTID (0x4)       /* Device is 2400 (else 2401) */
#define MSVR_DTR (0x2)          /* Value of DTR */
#define MSVR_RTS (0x1)          /*  ...RTS */

/*
 * LIVR bits
 */
#define LIVR_MASK (0x3)         /* Mask for interrupt reason: */
#define LIVR_MODEM (0x1)        /* Modem change interrupt */
#define LIVR_TX (0x2)           /*  ...transmit */
#define LIVR_RX (0x3)           /*  ...receive */
#define LIVR_RXEX (0)           /*  ...receive exception */

/*
 * IER bits
 */
#define IER_MDM (0x80)          /* Enable modem change interrupts */
#define IER_RET (0x20)          /*  ...receive exception timeouts */
#define IER_RXD (0x8)           /*  ...receive FIFO threshold */
#define IER_TIMER (0x4)         /*  ...timers */
#define IER_TXMPTY (0x2)        /*  ...transmit buffer entirely empty */
#define IER_TXD (0x1)           /*   ...transmit FIFO threshold */

/*
 * LICR bits
 */
#define LICR_MASK (0xc)         /* Mask for channel number bits */
#define LICR_CHAN0 (0x0)        /*  ...channel 0 */
#define LICR_CHAN1 (0x4)        /*  ...channel 1 */
#define LICR_CHAN2 (0x8)        /*  ...channel 2 */
#define LICR_CHAN3 (0xc)        /*  ...channel 3 */

/*
 * STK bits
 */
#define STK_MASK0 (0x81)        /* Mask for current level */
#define STK_NONE (0x0)          /*  ...not nested to interrupt */
#define STK_MODEM (0x1)         /*  ...in modem */
#define STK_TX (0x80)           /*  ...transmit */
#define STK_RX (0x81)           /*  ...receive */

/*
 * RIR bits
 */
#define RIR_VECT(x) (((x) & 0xc) >> 2)
                                /* Vector number of type of interrupt */
#define RIR_CHAN(x) ((x) & 0x3) /*  ...channel */

/*
 * RISR bits (16-bit register; could also be read byte low/high)
 */
#define RISR_BRK (0x1)          /* Break detected */
#define RISR_FE (0x2)           /* Framing error */
#define RISR_PE (0x4)           /* Parity error */
#define RISR_RESIND (0x4)       /* Residual indication error */
#define RISR_OE (0x8)           /* Overrun error */
#define RISR_SCMASK (0x70)      /* Mask for special char match: */
#define RISR_SC1 (0x10)         /*  ...match 1 */
#define RISR_SC2 (0x20)         /*  ...match 2 */
#define RISR_SC3 (0x30)         /*  ...match 3 */
#define RISR_SC4 (0x40)         /*  ...match 4 */
#define RISR_SCRNG (0x70)       /*  ...match with range */
#define RISR_TO (0x80)          /* Receive data timeout */
#define RISR_BUFB (0x800)       /* Status during B (else A) */
#define RISR_EOB (0x2000)       /* End of receive buffer */
#define RISR_BERR (0x8000)      /* Bus error */
#define RISR_ERROR (RISR_CRC|RISR_RXABT|RISR_OE|RISR_RESIND|RISR_BERR)

/*
 * RISR low bits during async PPP
 */
#define RISR_CRC (0x10)         /* CRC error */
#define RISR_RXABT (0x20)       /* Receiver abort */
#define RISR_EOF (0x40)         /* End of frame received */

/*
 * REOIR bits
 */
#define REOIR_TERM (0x80)       /* Terminate current DMA buffer */
#define REOIR_DISCEXC (0x40)    /* Discard exception character */
#define REOIR_NOTRANSF (0x8)    /* No data transferred */

/*
 * TIR bits
 */
#define TIR_VECT(x) (((x) & 0xc) >> 2)
                                /* Vector number of type of interrupt */
#define TIR_CHAN(x) ((x) & 0x3) /*  ...channel */

/*
 * TISR bits
 */
#define TISR_BERR (0x80)        /* Bus error */
#define TISR_EOF (0x40)         /* End of frame */
#define TISR_EOB (0x20)         /* End of buffer */
#define TISR_UE (0x10)          /* Transmitter underrun */
#define TISR_BUFB (0x8)         /* Status during B (else A) */
#define TISR_MPTY (0x2)         /* Transmit buffer empty */
#define TISR_TXD (0x1)          /* Transmit threshold reached */
#define TISR_ERROR (TISR_BERR | TISR_UE | TISR_TXD) 

/*
 * TEOIR bits
 */
#define TEOIR_TERM (0x80)       /* Terminate current DMA buffer */
#define TEOIR_NOTRANSF (0x8)    /* No data transferred */

/*
 * MIR bits
 */
#define MIR_VECT(x) (((x) & 0xc) >> 2)
                                /* Vector number of type of interrupt */
#define MIR_CHAN(x) ((x) & 0x3) /*  ...channel */

/*
 * MISR bits
 **** REFERENCES TO RTS AND DTR are SWAPPED for HW flow ctrl ******
 * See comments in ts/tty_cd2430.c
 */
#define MISR_DSR (0x80)         /* DSR changed */
#define MISR_CD (0x40)          /*  ...CD */
#define MISR_CTS (0x20)         /*  ...CTS */
#define MISR_RTS (0x80)
#define MISR_DTR (0x20)
#define MISR_TIMER1 (0x1)       /* Timer 1 popped */
#define MISR_TIMER2 (0x2)       /*  ...timer 2 */

/*
 * DMR bit
 */
#define DMR_BYTE (0x8)          /* Do byte DMA (else word) */
#define DMR_SSTEP (0x2)         /* BTCR mode--single step */
#define DMR_CLKDIS (0x1)        /*  ...clock disable */

/*
 * DMABSTS bits
 */
#define DMABSTS_RSTAPD (0x40)   /* Terminate append pending */
#define DMABSTS_APPEND (0x10)   /* Append mode for buffer A */
#define DMABSTS_NTBUF (0x8)     /* Next transmit buf (0=A, 1=B) */
#define DMABSTS_TBUSY (0x4)     /* Current transmit buffer active */
#define DMABSTS_NRBUF (0x2)     /* Next receive buf (0=A, 1=B) */
#define DMABSTS_RBUSY (0x1)     /* Current receive buffer active */

/*
 * [AB]RBSTS bits
 */
#define RBSTS_BERR (0x80)       /* Bus error */
#define RBSTS_EOF (0x40)        /* End of frame */
#define RBSTS_EOB (0x20)        /* End of buffer */
#define RBSTS_24OWN (0x1)       /* 2400 owns buffer */

/*
 * [AB]TBSTS bits
 */
#define TBSTS_BERR (0x80)       /* Bus error */
#define TBSTS_EOF (0x40)        /* End of frame */
#define TBSTS_EOB (0x20)        /* End of buffer */
#define TBSTS_APND (0x4)        /* Append */
#define TBSTS_MAP32 (0x4)       /* PPP async--use conservative ACCM */
#define TBSTS_INTR (0x2)        /* Interrupt after buffer sent */
#define TBSTS_24OWN (0x1)       /* 2400 owns buffer */

/*
 * CPSR bits
 */
#define CPSR_CRC16 0x1          /* Use CRC-16 calculation */
#define CPSR_V41 0x0            /*  ...V.41 polynomial instead */

/*
 * Masks for the nested interrupt level bits in the stack register.
 */
#define STK_CLVL_MASK   0x81  /* Current level mask */
#define STK_MLVL_MASK   0x42  /* Middle level mask */
#define STK_TLVL_MASK   0x24  /* Tertiary level mask */
 
/*
 * Defines for the codes that indicate whether the (possibly) nested
 * interrupts are transmit, receive or modem interrupts.
 */
#define STK_TX_CLVL             0x80
#define STK_TX_MLVL             0x40
#define STK_TX_TLVL             0x20
#define STK_RX_CLVL             0x81
#define STK_RX_MLVL             0x42
#define STK_RX_TLVL             0x24
#define STK_MODEM_CLVL          0x01
#define STK_MODEM_MLVL          0x02
#define STK_MODEM_TLVL          0x04

/*
 * Constants
 */
#define CD2430_BUFSIZE (16)     /* # bytes in transmit or receive FIFO */
#define CD2430_CHIPCHAN (4)     /* # channels per 2430 */
#define CD2430_DISABLE (3)      /* Disable all CD2430 interrupts */
#define CD2430T_DISABLE (1)     /*  ...transmit */
#define CD2430R_DISABLE (2)     /*  ...receive */
#define CD2430M_DISABLE (3)     /*  ...modem signals */

/* Clock rate 2430 driven at */
#define CD2430CLOCK (20*1024*1024)


/* Convert interrupt vector to its PILR value */
#define CD2430PILR(x) (((x) << 1) | 0xF1)

/* Extract low and high buffer address fields */
#define CD2430LO(addr) ((ulong)addr & 0xFFFF)
#define CD2430HI(addr) (((ulong)addr >> 16) & 0xFFFF)

/*
 * Macros for MISR Modem/Timer status change
 */
#define DCD_DSR_CHANGE(status) (status & (MISR_CD | MISR_DSR))
#define DTR_CHANGE(status) (status & MISR_CD)
#define RTS_CHANGE(status) (status & MISR_CTS)
#define TIMER1_TIMEOUT(status) (status & MISR_TIMER1)
#define TIMER2_TIMEOUT(status) (status & MISR_TIMER2)

/*
 * Structure that holds various pointers relating to the mode that a
 * particular channel is currently in. Among these pointers are -
 *    o A pointer to the interface's specific master structure.
 *      (This would be a tt_soc structure for an async interface, and
 *      an IDB for a sync interface)
 *    o A pointer to the transmit ISR for this mode of operation.
 *    o A pointer to the receive ISR for this mode of operation.
 *    o A pointer to the modem signal change ISR for this mode of operation.
 *    o A pointer to the rxgood ISR for this mode of operation.
 * Since the interrupt wrapper for the CD2430 uses these pointers to jump
 * into the appropriate ISR directly, the following structure definition
 * must NOT be changed without also modifying the wrapper to work with 
 * the changes.
 */
#define CD2430_CHANNEL_INST struct cd2430_channel_inst
typedef CD2430_CHANNEL_INST {
    void *aid;                        /* Active Interface Descriptor */
    void (*rxint_ptr)(void *aid);     /* Pointer to Rx ISR */
    void (*modint_ptr)(void *aid);    /* Pointer to Modem signal change ISR */
    void (*txint_ptr)(void *aid);     /* Pointer to Tx ISR */
    void (*rxgoodint_ptr)(void *aid); /* Pointer Rxgood ISR */
} cd2430_channel_inst_t;

/*
 * Possible modes for the CD2430 controller
 */
#define MODE_INVALID 0
#define MODE_HDLC    1
#define MODE_BISYNC  2
#define MODE_ASYNC   3

#ifdef PAN
#include "../les/if_c3000_cd2430.h"
#endif


/*
 * Inline function declarations
 * 	These are inline functions that are shared by the sync and async
 * driver code.
 */

/*
 * arm_rx_dma()
 *      Arm receive DMA to CD2430
 *
 * Reads the DMA status register to find out which buffer slot (A or B) is
 * to be used next, and then programs the buffer into that slot and enables
 * DMA (i.e., sets ownership to the 2430) for it.
 */
static inline void
arm_rx_dma (cd2430_quadart *reg_table, int chan, ulong addr, uint count)
{
    if (chan) {
        reg_table->brbadrl = CD2430LO(addr);
        reg_table->brbadru = CD2430HI(addr);
        reg_table->brbcnt = count;
        reg_table->brbsts = RBSTS_24OWN;
    }
    else {
        reg_table->arbadrl = CD2430LO(addr);
        reg_table->arbadru = CD2430HI(addr);
        reg_table->arbcnt = count;
        reg_table->arbsts = RBSTS_24OWN;
    }
}

extern uint cd2402_firmware_size;

typedef unsigned char pcr_rw, pcr_ro;
typedef unsigned short pcr_wrw, pcr_wro;

/*
 * For printing purposes, we take a snapshot of the per/channel registers
 * and store them into this structure.  Then use this structure inplace
 * of the live registers.
 */
typedef struct
{
    pcr_rw car;      /* Channel Access Register */
    pcr_rw cmr;      /* Channel Mode Register */
    pcr_rw cor1;     /* Channel Option Register 1 */
    pcr_rw cor2;     /* Channel Option Register 2 */
    pcr_rw cor3;     /* Channel Option Register 3 */
    pcr_rw cor4;     /* Channel Option Register 4 */
    pcr_rw cor5;     /* Channel Option Register 5 */
    pcr_rw cor6;     /* Channel Option Register 6 */
    pcr_rw cor7;     /* Channel Option Register 7 */
    pcr_rw schr1;    /* Special Character Register 1 */
    pcr_rw schr2;    /* Special Character Register 2 */
    pcr_rw schr3;    /* Special Character Register 3 */
    pcr_rw schr4;    /* Special Character Register 4 */
    pcr_rw scrl;     /* Special Character Range low */
    pcr_rw scrh;     /* Special Character Range high */
    pcr_rw lnxt;     /* Literal Next Character */
    pcr_rw cpsr;     /* CRC Polynomial Select Register */
    pcr_rw rbpr;     /* Receive Baud Rate Period Register */
    pcr_rw rcor;     /* Receive Clock Option Register */
    pcr_rw tbpr;     /* Transmit Baud Rate Period Register */
    pcr_rw tcor;     /* Transmit Clock Option Register */
    pcr_rw ccr;      /* Channel Command Register */
    pcr_rw stcr;     /* Special Transmit Command Register */
    pcr_ro csr;      /* Channel Status Register */
    pcr_rw msvr_rts; /* Modem Signal Value--RTS */
    pcr_rw msvr_dtr; /* Modem Signal Value--DTR */
    pcr_rw livr;     /* Local Interrupt Vector Register */
    pcr_rw ier;      /* Interrupt Enable Register */
    pcr_rw licr;     /* Local Interrupting Channel Register */
    pcr_ro dmabsts;  /* DMA Buffer Status */
    pcr_wrw arbadrl; /* A Receive Buffer Address lower */
    pcr_wrw arbadru; /* A Receive Buffer Address upper */
    pcr_wrw brbadrl; /* B Receive Buffer Address lower */
    pcr_wrw brbadru; /* B Receive Buffer Address upper */
    pcr_wrw arbcnt;  /* A Receive Buffer Byte Count */
    pcr_wrw brbcnt;  /* B Receive Buffer Byte Count */
    pcr_rw arbsts;   /* A Receive Buffer Status */
    pcr_rw brbsts;   /* B Receive Buffer Status */
    pcr_wro rcbadrl; /* Receive Current Buffer Address lower */
    pcr_wro rcbadru; /* Receive Current Buffer Address upper */
    pcr_wrw atbadru; /* A Transmit Buffer Address upper */
    pcr_wrw atbadrl; /* A Transmit Buffer Address lower */
    pcr_wrw btbadru; /* B Transmit Buffer Address upper */
    pcr_wrw btbadrl; /* B Transmit Buffer Address lower */
    pcr_wrw btbcnt;  /* B Transmit Buffer Byte Count */
    pcr_wrw atbcnt;  /* A Transmit Buffer Byte Count */
    pcr_rw btbsts;   /* B Transmit Buffer Status */
    pcr_rw atbsts;   /* A Transmit Buffer Status */
    pcr_wro tcbadrl; /* Transmit Current Buffer Address lower */
    pcr_wro tcbadru; /* Transmit Current Buffer Address upper */
    pcr_rw tpr;      /* Timer Period Register */
    pcr_wrw rtpr;    /* Receive Timeout Period Register */
    pcr_wrw gt1;     /* General Timer 1 */
    pcr_ro ttr;      /* Transmit Timer Register */
}per_channel_registers;

extern void cd2430_copy_per_channel_registers(cd2430_quadart *reg_table,
					      per_channel_registers *pcr);

extern void cd2430_show_registers_perchannel(per_channel_registers *pcr);

#endif  /* __IF_LES_COMMON_CD2430_H__ */

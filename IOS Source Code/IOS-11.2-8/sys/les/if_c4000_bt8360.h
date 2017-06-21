/* $Id: if_c4000_bt8360.h,v 3.3.60.1 1996/06/16 21:15:16 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_bt8360.h,v $
 *------------------------------------------------------------------
 * if_c4000_bt8360.h - C4XXX T1 framer defines. 
 *
 * Nov 27 1994, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_bt8360.h,v $
 * Revision 3.3.60.1  1996/06/16  21:15:16  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3  1995/11/17  17:38:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:33:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * External T1 interface control register
 */
#define T1_CTRL_JASEL      0x40        /* select jitter attenuation location */
#define T1_CTRL_INIT       0x20        /* 1 for framer initialization */
#define T1_CTRL_REMLOOP    0x08        /* 1 for Remote Loopback */
#define T1_CTRL_RLOOP      0x04        /* this bit is used with bit 1 to reset */
                                       /* transceiver */
#define T1_CTRL_LLOOP      0x02        /* 1 for Local Loopback */
#define T1_CLOCK_SOURCE    0x01        /* 0 for external recovery line clock */
                                       /* 1 for internal transmit clock */

/*
 * External T1 interface status register
 */
#define T1_STATUS_DPM	   0x02        /* driver performance monitor */
#define T1_STATUS_LOS      0x01        /* transceiver detect loss of signal */

/*
 * External T1 framer interrupt enable register
 */
#define T1_FINT_STATUS     0x04        /* 1 indicate T1 framer is requesting interrupt */
#define T1_FINT_MAP        0x02        /* 1 means level 3 interrupt priority */
                                       /* 0 means level 4 interrupt priority */
#define T1_FINT_ENABLE     0x01        /* 1 means enable framer interrupt to MPU */


/*
 *
 *            The Brooktree T-1 Primary Rate Framer Chip
 *
 */
#define T1_CFG_REG      0               /* Configuration Register */

#define  T1_CFG_ESF             0x01            /* enable ESF mode */
#define  T1_CFG_ZBTSI           0x02            /* enable ZBTSI mode */
#define  T1_CFG_B8ZS_TX         0x04            /* enable B8ZS TX mode */
#define  T1_CFG_B8ZS_RX         0x08            /* enable B8ZS RX mode */
#define  T1_CFG_VAZO            0x10            /* bizarre bit.  see manual */
#define  T1_CFG_T1DM            0x20            /* enable T1DM mode */
#define  T1_CFG_INTTXCLK        0x40            /* enable Internal TX clock */
#define  T1_CFG_CLKRECVY        0x80            /* enable clock recovery */

#define T1_CTRL_REG     1               /* Interface & Clk Ctrl Reg */

#define  T1_CTRL_BMC            0x03      	/* buffer mode control */
#define  T1_CTRL_BMC_193        0x00            /* 193 bit frame rate */
#define  T1_CTRL_BMC_BURST      0x01            /* Burst Mode */
#define  T1_CTRL_BMC_192        0x02            /* 192 bit frame rate */
#define  T1_CTRL_BMC_256        0x03            /* 256 bit frame rate */
#define  T1_CTRL_TXSYNC         0x04      	/* enable tx frame sync */
#define  T1_CTRL_SLIPSYNC       0x08      	/* slip buffer frame sync */
#define  T1_CTRL_INFREQ         0x30      	/* input clock frequency */
#define  T1_CTRL_INFREQ_24      0x00            /* 24.704 MHz */
#define  T1_CTRL_INFREQ_12      0x10            /* 12.352 MHz */
#define  T1_CTRL_INFREQ_6       0x20            /*  6.176 MHz */
#define  T1_CTRL_INFREQ_1       0x30            /*  1.544 MHz */
#define  T1_CTRL_OUTFREQ        0xc0      	/* output clock frequency */
#define  T1_CTRL_OUTFREQ_2      0x00            /* CLOCKI / 2 */
#define  T1_CTRL_OUTFREQ_4      0x40            /* CLOCKI / 4 */
#define  T1_CTRL_OUTFREQ_1      0x80            /*  1.544 MHz */
#define  T1_CTRL_OUTFREQ_8      0xc0            /*  8.000 kHz */

#define T1_FRAME_REG     2              /* Frame-bit, Loop Ctrl Reg */

#define  T1_FRAME_FBIT          0x01     /* enable external F/Ft bit */
#define  T1_FRAME_CRCBIT        0x02     /* enable external CRC bit */
#define  T1_FRAME_DLBIT         0x04     /* enable external DL/Fs bit */
#define  T1_FRAME_ZBIT          0x08     /* enable external Z bit */
#define  T1_FRAME_NETLOOP       0x10     /* enable network loopback */
#define  T1_FRAME_LINELOOP      0x20     /* enable line loopback */
#define  T1_FRAME_EQPMTLOOP     0x40     /* enable equipment loopback */
#define  T1_FRAME_PYLDLOOP      0x80     /* enable payload loopback */

#define T1_SIGCTRL_REG   3              /* Signaling Ctrl Reg */

#define  T1_SIGCTRL_REINSERT    0x1f     /* sig reinsertion frame offset */
#define  T1_SIGCTRL_FREEZE      0x20     /* freeze signaling */
#define  T1_SIGCTRL_DEBOUNCE    0x40     /* debounce signaling */
#define  T1_SIGCTRL_DISABLE     0x80     /* disable signaling freeze */

#define T1_FRAMCTRL_REG  4              /* Framing Control Register */

#define  T1_FRAMCTRL_CRCCHK     0x01     /* enable CRC check */
#define  T1_FRAMCTRL_FTFRAM     0x02     /* enable Ft-only framing */
#define  T1_FRAMCTRL_SFFRAM     0x04     /* enable SF tx framing */
#define  T1_FRAMCTRL_ESF        0x08     /* enable ESF tx framing */
#define  T1_FRAMCTRL_2OF5       0x10     /* enable 2 of 5 OOF */
#define  T1_FRAMCTRL_2OF6       0x20     /* enable 2 of 6 OOF */
#define  T1_FRAMCTRL_DISABLE    0x40     /* disable framing */
#define  T1_FRAMCTRL_FORCE      0x80     /* force framing */

#define T1_SLIP_REG     5               /* Slip Buffer Ctrl Register */

#define  T1_SLIP_SLPMVIP        0x01     /* Slip buffer MVIP mode */
#define  T1_SLIP_TXMVIP         0x02     /* transmit buffer MVIP mode */
#define  T1_SLIP_BPV            0x04     /* force bipolar violation */
#define  T1_SLIP_FBE            0x08     /* force frame bit error */
#define  T1_SLIP_UNICODE        0x10     /* UNICODE detection */
#define  T1_SLIP_SHORTDLY       0x20     /* force short slip-bufr dly */
#define  T1_SLIP_INTCLK         0x40     /* internal slip/tx buf clck */
#define  T1_SLIP_INHIBSIG       0x80     /* inhibit signal reinsert */

#define T1_ALARM_REG    6               /* Alarm Signal Control Register */

#define  T1_ALARM_TXAIS         0x01     /* enable tx AIS */
#define  T1_ALARM_TXYELLOW      0x02     /* enable tx yellow alarm */
#define  T1_ALARM_RXAIS         0x04     /* enable rx AIS */
#define  T1_ALARM_RXYELLOW      0x08     /* enable rx yellow alarm */
#define  T1_ALARM_OOF           0x10     /* enable oof integration */
#define  T1_ALARM_SFYELLOW      0x20     /* enable SF yellow alarm  */
#define  T1_ALARM_ZERO          0x40     /* enable excess zero counts */
#define  T1_ALARM_MONITOR       0x80     /* enable Fs/Ft monitoring */

#define T1_ALRMINT_REG  7               /* Alarm & Timing Int Enable Reg */

#define  T1_ALRMINT_LOS         0x01     /* LOS Interrupt */
#define  T1_ALRMINT_OOF         0x02     /* OOF Interrupt */
#define  T1_ALRMINT_AIS         0x04     /* AIS Interrupt */
#define  T1_ALRMINT_YELLOW      0x08     /* Yellow Alarm Interrupt */
#define  T1_ALRMINT_FREEZE      0x10     /* signalling freeze Int */
#define  T1_ALRMINT_ALATCH      0x20     /* Latch alarms */
#define  T1_ALRMINT_ELATCH      0x40     /* Latch errors */
#define  T1_ALRMINT_CLATCH      0x80     /* Latch counters */

#define T1_ERRINT_REG   8               /* Error and Counter Int Enable Reg. */

#define  T1_ERRINT_PDV          0x01     /* pulse density violation */
#define  T1_ERRINT_CRC          0x02     /* crc error */
#define  T1_ERRINT_SLIP         0x04     /* frame slip */
#define  T1_ERRINT_SEVERR       0x08     /* severly-errored superframe */
#define  T1_ERRINT_BPVCTR       0x10     /* BPV Counter overflow */
#define  T1_ERRINT_FBECTR       0x20     /* FBE Counter overflow */
#define  T1_ERRINT_CRCCTR       0x40     /* CRC Counter overflow */
#define  T1_ERRINT_COFACTR      0x80     /* COFA Counter overflow */

#define T1_TXCTRL_REG   9               /* Transmit Data Link Ctrl Register */

#define  T1_TXCTRL_IDLE         0x01     /* Send Idle */
#define  T1_TXCTRL_FCS          0x02     /* send FCS, etc. */
#define  T1_TXCTRL_ABORT        0x04     /* Send abort */
#define  T1_TXCTRL_MOP          0x08     /* enable MOP mode */
#define  T1_TXCTRL_MSGPTR       0xf0     /* Message Octet Pointer */

#define T1_RXCTRL_REG   0xa             /* Receive Data Link Control Reg */

#define  T1_RXCTRL_MOPINT       0x01     /* rx data link MOP interrupt */
#define  T1_RXCTRL_BOPINT       0x02     /* rx data link BOP interrupt */
#define  T1_RXCTRL_2KBSRX       0x04     /* 2 kbit/s rx data link */
#define  T1_RXCTRL_RXUNFORM     0x08     /* rx unformatted mode */
#define  T1_RXCTRL_G802         0x10     /* G.802 compatibility mode */
#define  T1_RXCTRL_PATTERN      0x20     /* rx out frm pattern insert */
#define  T1_RXCTRL_2KBSTX       0x40     /* 2 kbit/s tx data link */
#define  T1_RXCTRL_STACKINT     0x80     /* signaling-stack interrupt */

#define T1_LOOPGD_REG   0xb             /* Loopbak Code Gen and Detect Ctrl */

#define  T1_LOOPGD_CODEGEN      0x01     /* enable code generation */
#define  T1_LOOPGD_OVERWRITE    0x02     /* overwrite frame bit */
#define  T1_LOOPGD_CODELEN      0x0c     /* code generation length */
#define   T1_LOOPGD_CODELEN_4   0x00            /* 4 bit sequence */
#define   T1_LOOPGD_CODELEN_5   0x04            /* 5 bit sequence */
#define   T1_LOOPGD_CODELEN_6   0x08            /* 6 bit sequence */
#define   T1_LOOPGD_CODELEN_7   0x0c            /* 7 bit sequence */
#define  T1_LOOPGD_ACTDET       0x30     /* rx activate detect code len */
#define   T1_LOOPGD_ACTDET_4    0x00            /* 4 bit sequence */
#define   T1_LOOPGD_ACTDET_5    0x10            /* 5 bit sequence */
#define   T1_LOOPGD_ACTDET_6    0x20            /* 6 bit sequence */
#define   T1_LOOPGD_ACTDET_7    0x30            /* 7 bit sequence */
#define  T1_LOOPGD_DEACTDET     0xc0     /* rx deactivate detect code len */
#define   T1_LOOPGD_DEACTDET_4  0x00            /* 4 bit sequence */
#define   T1_LOOPGD_DEACTDET_5  0x40            /* 5 bit sequence */
#define   T1_LOOPGD_DEACTDET_6  0x80            /* 6 bit sequence */
#define   T1_LOOPGD_DEACTDET_7  0xc0            /* 7 bit sequence */

#define T1_LOOPCG_REG   0xc             /* Loopback Code Generation Register */

#define  T1_LOOPCG_SEQ          0xfe     /* The code gen sequence */

#define T1_LOOPCODE_ACT_REG     0xd     /* Loopback Activate Code Reg */

#define  T1_LOOPCODE_ACT_INT    0x01            /* Loopback-act code int en */
#define  T1_LOOPCODE_ACT_SEQ    0xfe            /* Loopback-act code seq */

#define T1_LOOPCODE_DEACT_REG   0xe     /* Loopback Deactivate Code Register */
#define  T1_LOOPCODE_DEACT_INT  0x01    /* Loopback-deact code int enable */
#define  T1_LOOPCODE_DEACT_SEQ  0xfe    /* Loopback-deact code sequence */

#define T1_MISCSTAT_REG         0x0f    /* Misc. Status */

#define  T1_MISCSTAT_RAM        0x40            /* RAM not ready */
#define  T1_MISCSTAT_SLC        0x80            /* SLC Spoiler Field OOF */

#define T1_ALSSTAT_REG          0x10    /* Alarm/Loopback/Signalling Status */

#define  T1_ALSSTAT_LOS         0x01            /* Loss of Signal */
#define  T1_ALSSTAT_OOF         0x02            /* Out of Frame */
#define  T1_ALSSTAT_AIS         0x04            /* alarm indication */
#define  T1_ALSSTAT_YELLOW      0x08            /* yellow alarm */
#define  T1_ALSSTAT_FREEZE      0x10            /* signaling freeze */
#define  T1_ALSSTAT_LOOPACT     0x20            /* loopback activate detect */
#define  T1_ALSSTAT_LOOPDEACT   0x40          /* loopback deactivate detect */
#define  T1_ALSSTAT_SIGSTACK    0x80            /* signaling stack interrupt */

#define T1_ERRSTAT_REG          0x11    /* Clear-channel/Error Status Register */

#define  T1_ERRSTAT_PDV         0x01      /* pulse density violation */
#define  T1_ERRSTAT_ZEROS       0x02      /* eight consecutive zeros rx'd */
#define  T1_ERRSTAT_B8ZS        0x04      /* B8ZS signature received */
#define  T1_ERRSTAT_ZBTS1       0x08      /* ZBTSI rx error */
#define  T1_ERRSTAT_CRC         0x10      /* CRC/Frame-bit error */
#define  T1_ERRSTAT_SEVERR      0x20      /* severley errored superfrm */
#define  T1_ERRSTAT_SLIPEV      0x40      /* frame slip event */
#define  T1_ERRSTAT_SLIPDIR     0x80      /* frame slip direction */

#define T1_RXSTATUS_REG         0x12   /* Receive Data Link Status Register */

#define  T1_RXSTATUS_IDLE       0x01      /* Data link Idle */
#define  T1_RXSTATUS_ABORT      0x02      /* abort sequence received */
#define  T1_RXSTATUS_BADFCS     0x04      /* invalid FCS received */
#define  T1_RXSTATUS_ENDMOP     0x01      /* end of MOP message */
#define  T1_RXSTATUS_MSGPTR     0xf0      /* message octet pointer */

#define T1_BPV_LSB              0x13    /* BPV lower byte */
#define T1_BPV_MSB              0x14    /* BPV High byte */
#define T1_FBE_CTR              0x15    /* FBE counter  */
#define T1_CRC_CTR              0x16    /* CRC counter  */

#define T1_COFA_CTR             0x17    /* COFA/CRC MSB */

#define  T1_COFA_CRC            0x80
#define  T1_COFA_MASK           0x7f

#define T1_INTSRC_REG           0x18    /* Interrupt Source Register */

#define  T1_INTSRC_ALS          0x01     /* Alarm/Loopback/Signaling */
#define  T1_INTSRC_ERROR        0x02     /* Error */
#define  T1_INTSRC_RXDATA       0x04     /* rx data link */
#define  T1_INTSRC_TXDATA       0x08     /* tx data link */
#define  T1_INTSRC_BPVCTR       0x10     /* BPV Counter */
#define  T1_INTSRC_FBECTR       0x20     /* Frame-bit Erorr Counter */
#define  T1_INTSRC_CRCCTR       0x40     /* CRC Error Counter */
#define  T1_INTSRC_COFACTR      0x80     /* COFA Counter */

#define T1_VERSION              0x19     /* Version Register */

#define  T1_VERSION_VERSION     0x0f     /* The Version Number */

#define  T1_VERSION_VERSION_UGA_360_1  0
#define  T1_VERSION_VERSION_UGA_360_2  1
#define  T1_VERSION_PART               0xf0            /* The Part Number */
#define  T1_VERSION_PART_UGA_360       0x30

#define T1_FRAMSTAT_REG         0x1a    /* Framing and Timing Status Register */

#define  T1_FRAMSTAT_ACTIVE     0x01     /* Framer Active */
#define  T1_FRAMSTAT_ACTIVE6    0x02     /* Framer Active > 6 */
#define  T1_FRAMSTAT_TXOOF      0x04     /* Tx signal OOF */
#define  T1_FRAMSTAT_TXYELLOW   0x08     /* Tx signal yellow */
#define  T1_FRAMSTAT_SLIP       0x10     /* slip buffer delay > 1 frm */
#define  T1_FRAMSTAT_RXCTR      0x20     /* rx frame counter MSB */
#define  T1_FRAMSTAT_TXBCTR     0x40     /* tx buffer frm counter MSB */
#define  T1_FRAMSTAT_TXFCTR     0x80     /* tx frame counter MSB */

#define T1_SIGCHG_REG           0x1b     /* Signaling-Change Stack Register */

#define  T1_SIGCHG_CHAN         0x1f     /* channel number */
#define  T1_SIGCHG_BBIT         0x20     /* B Bit */
#define  T1_SIGCHG_ABIT         0x40     /* A Bit */
#define  T1_SIGCHG_MORE         0x80     /* More stack contents */

/*
 * The following offset is actually 0x100, but hardware maps
 * the offset to 0x00.
 */
#define T1_RATE_CONTROL         0x00

#define  T1_RATE_TS_ACTIVE      0x01
#define  T1_RATE_AB_SIGNAL      0x02
#define  T1_RATE_TX_PCM_IDLE    0x04
#define  T1_RATE_TX_TS_INDICATE 0x08
#define  T1_RATE_RX_PCM_IDLE    0x10
#define  T1_RATE_RX_SIG_INSERT  0x20
#define  T1_RATE_LOOP_TX_CH     0x40
#define  T1_RATE_SLIP_BUFFER    0x80

#define T1_TX_PCM_CONTROL       0x21

#define  T1_TX_PCM_D_BIT        0x01
#define  T1_TX_PCM_C_BIT        0x02
#define  T1_TX_PCM_B_BIT        0x04
#define  T1_TX_PCM_A_BIT        0x08
#define  T1_TX_PCM_IDLE_OUT     0x10
#define  T1_TX_PCM_SIG_LOCAL    0x20
#define  T1_TX_PCM_LOOP         0x40
#define  T1_TX_PCM_INSERT_SIG   0x80

#define T1_FRAMING_TIMER        20
 
#define T1_FRAMER_OFFSET      0x0800      /* offset to T1 framer registers */
#define T1_RAM_OFFSET         0x0A00      /* offset to T1 framer registers */
#define T1_CONTROL_OFFSET     0x0080      /* offset to T1 external control register */
#define T1_STATUS_OFFSET      0x0082      /* offset to T1 external status register */
#define T1_INT_CONTROL_OFFSET 0x0084      /* offset to T1 external interrupt control
                                             register */
#define T1_LED_OFFSET         0x000e      /* offset to T1 external LED register */

/*
 * Prototypes
 */
void bt8360_analyze_mcn_port_adaptor(int, int, int);
void bt8360_txAIS(uint, boolean);
void bt8360_linecode(uint, short);
void bt8360_local_up(uint);
void bt8360_no_loop(uint);
void bt8360_framing(uint, short);
void bt8360_interrupt(dsx1_instance_t *);
void bt8360_clocksource(uint, short);
void bt8360_frame_process(dsx1_instance_t *);
void bt8360_sec_update(dsx1_instance_t *);
void bt8360_loopup_code(uint);

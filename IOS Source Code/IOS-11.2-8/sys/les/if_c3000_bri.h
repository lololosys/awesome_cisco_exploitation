/* $Id: if_c3000_bri.h,v 3.3.62.1 1996/07/17 18:30:39 etrehus Exp $ 
 * $Source: /release/112/cvs/Xsys/les/if_c3000_bri.h,v $
 *------------------------------------------------------------------
 * if_c3000_bri.h  Definitions for ISDN BRI Moosehead
 *
 * March 1994, Manoj Leelanivas.
 * Based on "if_les_bri.h", November 1991, Greg Stovall
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c3000_bri.h,v $
 * Revision 3.3.62.1  1996/07/17  18:30:39  etrehus
 * CSCdi62203:  D-channel collision race condition
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  17:38:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:32:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* General defines */
#define D_CHAN_MAX_RX_SIZE 2
#define D_CHAN_MAX_TX_SIZE 1
#define B_CHAN_MAX_RX_SIZE 8
#define B_CHAN_MAX_TX_SIZE 4

/* Defines for the MC68302 controller */

/* System Config registers */
#define MC68302_BAR		0x021000F2 /* Base Address Register */
#define MC68302_SCR		0x021000F4 /* System Control Register */

/* Base Addresses */
#define MC68302_BASE		0x02101000 /* Base of Control Ram */
/*
 * Set FC bits to 101 (binary) for supervisor data space,
 * and the CFC bit to 1 so that the FC bits are used for address
 * comparison.
 * The DPRAM base address is set to 0x001000.
 */
#define MC68302_BAR_BASE      0xB001     /* Base to load into BAR */

/* Ring Buffer Address */
#define MC68302_SCC1_RX_RING	(MC68302_BASE + 0x400) /* SCC1 RX Ring */
#define MC68302_SCC1_TX_RING	(MC68302_BASE + 0x440) /* SCC1 TX Ring */
#define MC68302_SCC2_RX_RING	(MC68302_BASE + 0x500) /* SCC2 RX Ring */
#define MC68302_SCC2_TX_RING	(MC68302_BASE + 0x540) /* SCC2 TX Ring */
#define MC68302_SCC3_RX_RING	(MC68302_BASE + 0x600) /* SCC3 RX Ring */
#define MC68302_SCC3_TX_RING	(MC68302_BASE + 0x640) /* SCC3 TX Ring */

/* Internal Register Addresses */
#define MC68302_SCP_BD		(MC68302_BASE + 0x67A) /* SCP Buffer Descr. */
#define MC68302_BERR_CHAN	(MC68302_BASE + 0x67C) /* BERR Chan Number */
#define MC68302_CP_CR		(MC68302_BASE + 0x860) /* Command Reg */

/* SMC Addresses */
#define MC68302_SMC1_RX_BD	(MC68302_BASE + 0x666) /* SMC1 RX BD */
#define MC68302_SMC1_TX_BD	(MC68302_BASE + 0x668) /* SMC2 RX BD */
#define MC68302_SMC2_RX_BD	(MC68302_BASE + 0x66A) /* SMC2 RX BD */
#define MC68302_SMC2_TX_BD	(MC68302_BASE + 0x66C) /* SMC2 TX BD */

/* Bus Error Channel Numbers */
#define SCC1_TX_CHAN		0
#define SCC1_RX_CHAN		1
#define SCC2_TX_CHAN		2
#define SCC2_RX_CHAN		3
#define SCC3_TX_CHAN		4
#define SCC3_RX_CHAN		5

/* System Control Register (SCR) */
#define MC68302_SCR_IPA		0x08000000 /* Interrupt Priority Active */
#define MC68302_SCR_HWT		0x04000000 /* HW Watchdog Timeout */
#define MC68302_SCR_WPV		0x02000000 /* Write Protect Violation */
#define MC68302_SCR_ADC		0x01000000 /* Address Decode Conflict */
#define MC68302_SCR_ERRE	0x00400000 /* External RISC Request Enable */
#define MC68302_SCR_VGE		0x00200000 /* Vector Generation Enable */
#define MC68302_SCR_WPVE	0x00100000 /* Write Protect Violation Enable */
#define MC68302_SCR_RMCST	0x00080000 /* Read-Write-Modify Cycle */
#define MC68302_SCR_EMWS	0x00040000 /* External Master Wait State */
#define MC68302_SCR_ADCE	0x00020000 /* Address Decode Conflict Enable */
#define MC68302_SCR_BCLM	0x00010000 /* Bus Clear Mask */
#define MC68302_SCR_FRZW	0x00008000 /* Freeze Watchdog Timer Enable */
#define MC68302_SCR_FRZ2	0x00004000 /* Freeze Timer 2 Enable */
#define MC68302_SCR_FRZ1	0x00002000 /* Freeze Timer 1 Enable */
#define MC68302_SCR_SAM		0x00001000 /* Synchronous Access Mode */
#define MC68302_SCR_HWDEN	0x00000800 /* Hardware Watchdog Enable */
#define MC68302_SCR_HWDCN	0x00000700 /* Hardware Watchdog Count */
#define MC68302_SCR_LPREC	0x00000080 /* Low-Power Recovery */
#define MC68302_SCR_LPP16	0x00000040 /* LP Clock Divide by 16*/
#define MC68302_SCR_LPEN	0x00000020 /* Low-Power Enable */
#define MC68302_SCR_LPCD	0x0000001F /* LP Clock Divider Selects */

#define SCR_NORMAL	(MC68302_SCR_VGE | MC68302_SCR_WPVE | \
			MC68302_SCR_LPREC | MC68302_SCR_LPP16 | \
			MC68302_SCR_LPEN | MC68302_SCR_LPCD)

/* Global Interrupt Mode Register (GIMR) */
#define MC68302_GIMR_MOD	0x8000	/* Mode */
#define MC68302_GIMR_IV7	0x4000	/* Level 7 Int Vector */
#define MC68302_GIMR_IV6	0x2000	/* Level 6 Int Vector */
#define MC68302_GIMR_IV1	0x1000	/* Level 1 Int Vector */
#define MC68302_GIMR_ET7	0x0400	/* IRQ7 Edge-/Level-Triggered */
#define MC68302_GIMR_ET6	0x0200	/* IRQ6 Edge-/Level-Triggered */
#define MC68302_GIMR_ET1	0x0100	/* IRQ1 Edge-/Level-Triggered */
#define MC68302_GIMR_IVB	0x00E0	/* Interrupt Vector Bits */

/* Interrupt Pending Register (IPR) */
#define MC68302_IPR_PB11	0x8000	/* General-Purpose Int 3 (PB11) */
#define MC68302_IPR_PB10	0x4000	/* Genreal-Purpose Int 2 (PB10) */
#define MC68302_IPR_SCC1	0x2000	/* SCC1 Interrupt */
#define MC68302_IPR_SDMA	0x1000	/* SDMA Interrupt */
#define MC68302_IPR_IDMA	0x0800	/* IDMA Interrupt */
#define MC68302_IPR_SCC2	0x0400	/* SCC2 Interrupt */
#define MC68302_IPR_T1		0x0200	/* Timer 1 Interrupt */
#define MC68302_IPR_SCC3	0x0100	/* SCC3 Interrupt */
#define MC68302_IPR_PB9		0x0080	/* General-Purpose Int 1 (PB9) */
#define MC68302_IPR_T2		0x0040	/* Timer 2 Interrupt */
#define MC68302_IPR_SCP		0x0020	/* SCP Interrupt */
#define MC68302_IPR_T3		0x0010	/* Timer 3 Interrupt */
#define MC68302_IPR_SMC1	0x0008	/* SMC1 Interrupt */
#define MC68302_IPR_SMC2	0x0004	/* SMC2 Interrupt */
#define MC68302_IPR_PB8		0x0002	/* General-Purpose Int 0 (PB8) */
#define MC68302_IPR_ERR		0x0001	/* Error */

/* Interrupt Mask Register (IMR) */
#define MC68302_IMR_PB11	0x8000	/* General-Purpose Int 3 (PB11) */
#define MC68302_IMR_PB10	0x4000	/* Genreal-Purpose Int 2 (PB10) */
#define MC68302_IMR_SCC1	0x2000	/* SCC1 Interrupt */
#define MC68302_IMR_SDMA	0x1000	/* SDMA Interrupt */
#define MC68302_IMR_IDMA	0x0800	/* IDMA Interrupt */
#define MC68302_IMR_SCC2	0x0400	/* SCC2 Interrupt */
#define MC68302_IMR_T1		0x0200	/* Timer 1 Interrupt */
#define MC68302_IMR_SCC3	0x0100	/* SCC3 Interrupt */
#define MC68302_IMR_PB9		0x0080	/* General-Purpose Int 1 (PB9) */
#define MC68302_IMR_T2		0x0040	/* Timer 2 Interrupt */
#define MC68302_IMR_SCP		0x0020	/* SCP Interrupt */
#define MC68302_IMR_T3		0x0010	/* Timer 3 Interrupt */
#define MC68302_IMR_SMC1	0x0008	/* SMC1 Interrupt */
#define MC68302_IMR_SMC2	0x0004	/* SMC2 Interrupt */
#define MC68302_IMR_PB8		0x0002	/* General-Purpose Int 0 (PB8) */

/* Interrupt In-Service Register (ISR) */
#define MC68302_ISR_PB11	0x8000	/* General-Purpose Int 3 (PB11) */
#define MC68302_ISR_PB10	0x4000	/* Genreal-Purpose Int 2 (PB10) */
#define MC68302_ISR_SCC1	0x2000	/* SCC1 Interrupt */
#define MC68302_ISR_SDMA	0x1000	/* SDMA Interrupt */
#define MC68302_ISR_IDMA	0x0800	/* IDMA Interrupt */
#define MC68302_ISR_SCC2	0x0400	/* SCC2 Interrupt */
#define MC68302_ISR_T1		0x0200	/* Timer 1 Interrupt */
#define MC68302_ISR_SCC3	0x0100	/* SCC3 Interrupt */
#define MC68302_ISR_PB9		0x0080	/* General-Purpose Int 1 (PB9) */
#define MC68302_ISR_T2		0x0040	/* Timer 2 Interrupt */
#define MC68302_ISR_SCP		0x0020	/* SCP Interrupt */
#define MC68302_ISR_T3		0x0010	/* Timer 3 Interrupt */
#define MC68302_ISR_SMC1	0x0008	/* SMC1 Interrupt */
#define MC68302_ISR_SMC2	0x0004	/* SMC2 Interrupt */
#define MC68302_ISR_PB8		0x0002	/* General-Purpose Int 0 (PB8) */

/* Parallel I/O Port A Pins */
#define MC68302_PIO_PA15	0x8000	/* PA15 */
#define MC68302_PIO_PA14	0x4000	/* PA14 */
#define MC68302_PIO_PA13	0x2000	/* PA13 */
#define MC68302_PIO_PA12	0x1000	/* PA12 */
#define MC68302_PIO_PA11	0x0800	/* PA11 */
#define MC68302_PIO_PA10	0x0400	/* PA10 */
#define MC68302_PIO_PA9		0x0200	/* PA9 */
#define MC68302_PIO_PA8		0x0100	/* PA8 */
#define MC68302_PIO_PA7		0x0080	/* PA7 */
#define MC68302_PIO_PA6		0x0040	/* PA6 */
#define MC68302_PIO_PA5		0x0020	/* PA5 */
#define MC68302_PIO_PA4		0x0010	/* PA4 */
#define MC68302_PIO_PA3		0x0008	/* PA3 */
#define MC68302_PIO_PA2		0x0004	/* PA2 */
#define MC68302_PIO_PA1		0x0002	/* PA1 */
#define MC68302_PIO_PA0		0x0001	/* PA0 */

/* Parallel I/O Port B Pins */
#define MC68302_PIO_PB15	0x8000	/* PB15 */
#define MC68302_PIO_PB14	0x4000	/* PB14 */
#define MC68302_PIO_PB13	0x2000	/* PB13 */
#define MC68302_PIO_PB12	0x1000	/* PB12 */
#define MC68302_PIO_PB11	0x0800	/* PB11 */
#define MC68302_PIO_PB10	0x0400	/* PB10 */
#define MC68302_PIO_PB9		0x0200	/* PB9 */
#define MC68302_PIO_PB8		0x0100	/* PB8 */
#define MC68302_PIO_PB7		0x0080	/* PB7 */
#define MC68302_PIO_PB6		0x0040	/* PB6 */
#define MC68302_PIO_PB5		0x0020	/* PB5 */
#define MC68302_PIO_PB4		0x0010	/* PB4 */
#define MC68302_PIO_PB3		0x0008	/* PB3 */
#define MC68302_PIO_PB2		0x0004	/* PB2 */
#define MC68302_PIO_PB1		0x0002	/* PB1 */
#define MC68302_PIO_PB0		0x0001	/* PB0 */

#define MC68302_PIO_IO	0	/* I/O Pin Type */
#define MC68302_PIO_PER	1	/* Peripheral Pin Type */

#define MC68302_PIO_IN		0	/* Input */
#define MC68302_PIO_OUT		1	/* Output */

/* CP Command Register (CR) */
#define MC68302_CR_RST		0x80	/* Software Reset */
#define MC68392_CR_GCI		0x40	/* GCI Opcodes Enables */
#define MC68392_CR_OPCODE	0x30	/* Opcode */
#define MC68302_CR_ST		0x00	/* Stop TX opcode */
#define MC68302_CR_RT		0x10	/* Restart TX opcode */
#define MC68302_CR_EHM		0x20	/* Enter Hunt Mode opcode */
#define MC68392_CR_CHNUM	0x06	/* Channel Number */
#define MC68392_CR_FLG		0x01	/* Command Semaphore Flag */
#define MC68302_CR_SCC1		0x00	/* SCC #1 */
#define MC68302_CR_SCC2		0x02	/* SCC #2 */
#define MC68302_CR_SCC3		0x04	/* SCC #3 */

/* SCC Configuartion Register (SCON) */
#define MC68302_SCON_WOMS	0x8000	/* Wired-OR Mode Select */
#define MC68302_SCON_EXTC	0x4000	/* External Clock Source */
#define MC68302_SCON_TCS	0x2000	/* Transmit Clock Source */
#define MC68302_SCON_RCS	0x1000	/* Receive Clock Source */
#define MC68302_SCON_CD		0x0FFE  /* Clock Divider */
#define MC68302_SCON_DIV4	0x0001	/* SCC Clock Divide by 4 */

#define SCON_NORMAL		0x7FFE	/* Normal value for SCON */

/* SCC Parameter RAM values */
#define MC68302_SCC_RFCR      0x10    /* FC bits = user data space */
#define MC68302_SCC_TFCR      0x10    /* FC bits = user data space */
#define MC68302_TXBD0_OFFSET  0x40    /* BD #0 offset from the SCC Base */

/* SCC Mode Register (SCM) */
#define MC68302_SCM_NOF3	0x8000	/* Number of Flags */
#define MC68302_SCM_NOF2	0x4000	/* Number of Flags */
#define MC68302_SCM_NOF1	0x2000	/* Number of Flags */
#define MC68302_SCM_NOF0	0x1000	/* Number of Flags */
#define MC68302_SCM_C32		0x0800	/* CRC16/CRC32 */
#define MC68302_SCM_FSE		0x0400	/* Flag Sharing Enabled */
#define MC68302_SCM_RTE		0x0100	/* Retransmit Enabled */
#define MC68302_SCM_FLG		0x0080	/* Transmit Flags */
#define MC68302_SCM_ENC		0x0040	/* Data Encoding Format */
#define MC68302_SCM_DIAG1	0x0020	/* Diagnostic Mode */
#define MC68302_SCM_DIAG0	0x0010	/* Diagnostic Mode */
#define MC68302_SCM_ENR		0x0008	/* Enable Receiver */
#define MC68302_SCM_ENT		0x0004	/* Enable Transmitter */
#define MC68302_SCM_MODE1	0x0002	/* Channel Mode */
#define MC68302_SCM_MODE0	0x0001	/* Channel Mode */

#define SCM_NORMAL		(MC68302_SCM_ENR | MC68302_SCM_ENT)
#define SCM3_NORMAL		(MC68302_SCM_DIAG1 | MC68302_SCM_DIAG0 | \
				MC68302_SCM_ENR | MC68302_SCM_ENT)

/* SCC HDLC Event Register (SCCE) */
#define MC68302_SCCE_CTS	0x80	/* Cleat-To-Send Status Changed */
#define MC68302_SCCE_CD		0x40	/* Carrier Detect Status Changed */
#define MC68302_SCCE_IDL	0x20	/* IDLE Sequence Status Changed */
#define MC68302_SCCE_TXE	0x10	/* TX Error */
#define MC68302_SCCE_RXF	0x08	/* RX Frame */
#define MC68302_SCCE_BSY	0x04	/* Busy Condition */
#define MC68302_SCCE_TXB	0x02	/* TX Buffer */
#define MC68302_SCCE_RXB	0x01	/* RX Buffer */

/* TX Buffer Control/Status Word */
#define MC68302_TCS_R		0x8000	/* Ready Bit */
#define MC68302_TCS_X		0x4000	/* External Buffer */
#define MC68302_TCS_W		0x2000	/* Wrap */
#define MC68302_TCS_I		0x1000	/* Interrupt */
#define MC68302_TCS_L		0x0800	/* Last in Frame */
#define MC68302_TCS_TC		0x0400	/* TX CRC */
#define MC68302_TCS_UN		0x0002	/* Underrun */
#define MC68302_TCS_CT		0x0001	/* CTS Lost */

/* RX Buffer Control/Status Word */
#define MC68302_RCS_E		0x8000	/* Empty */
#define MC68302_RCS_X		0x4000	/* External Buffer */
#define MC68302_RCS_W		0x2000	/* Wrap */
#define MC68302_RCS_I		0x1000	/* Interrupt */
#define MC68302_RCS_L		0x0800	/* Last in Frame */
#define MC68302_RCS_F		0x0400	/* First in Frame */
#define MC68302_RCS_LG		0x0020	/* RX Frame Length Violation */
#define MC68302_RCS_NO		0x0010	/* RX Nonoctet Aligned Frame */
#define MC68302_RCS_AB		0x0008	/* RX Abort Sequence */
#define MC68302_RCS_CR		0x0004	/* RX CRC Error */
#define MC68302_RCS_OV		0x0002	/* Overrun */
#define MC68302_RCS_CD		0x0001	/* Carrier Detect Lost */

/* SCP Mode Register (SPMODE) */
#define MC68302_SPM_STR		0x8000	/* Start Transmit */
#define MC68302_SPM_LOOP	0x4000	/* Loop Mode */
#define MC68302_SPM_CI		0x2000	/* Clock Invert */
#define MC68302_SPM_PM3		0x1000	/* Prescale Modulus Select */
#define MC68302_SPM_PM2		0x0800	/* Prescale Modulus Select */
#define MC68302_SPM_PM1		0x0400	/* Prescale Modulus Select */	
#define MC68302_SPM_PM0		0x0200	/* Prescale Modulus Select */
#define MC68302_SPM_EN		0x0100	/* Enable SCP */
#define MC68302_SPM_RES		0x0080	/* Reserved */
#define MC68302_SPM_SMD3	0x0040	/* SMC Mode Support bit 3 */
#define MC68302_SPM_SMD2	0x0020	/* SMC Mode Support bit 2 */
#define MC68302_SPM_SMD1	0x0010	/* SMC Mode Support bit 1 */
#define MC68302_SPM_SMD0	0x0008	/* SMC Mode Support bit 0 */
#define MC68302_SPM_SMC_LOOP	0x0004	/* SMC Local Loopback Mode */
#define MC68302_SPM_SMC_EN2	0x0002	/* SMC 2 Enable */
#define MC68302_SPM_SMC_EN1	0x0001	/* SMC 1 Enable */

/* SCP Buffer Descriptor */
#define MC68302_SCP_BD_DONE	0x8000	/* Done Bit */

/* Serial Interface Mode Register (SIMODE) */
#define MC68302_SIM_SETZ	0x8000	/* Set L1TXD to Zero */
#define MC68302_SIM_SYNC_SCIT	0x4000	/* SYNC Mode/SCIT Select Support */
#define MC68302_SIM_SDIAG1	0x2000	/* Serial I/F Diagnostic Mode 1 */
#define MC68302_SIM_SDIAG0	0x1000	/* Serial I/F Diagnostic Mode 0 */
#define MC68302_SIM_SDC2	0x0800	/* Serial Data Strobe Control 2 */
#define MC68302_SIM_SDC1	0x0400	/* Serial Data Strobe Control 1 */
#define MC68302_SIM_B2RB	0x0200	/* CH-3 Route */
#define MC68302_SIM_B2RA	0x0100	/* CH-3 Route */
#define MC68302_SIM_B1RB	0x0080	/* CH-2 Route */
#define MC68302_SIM_B1RA	0x0040	/* CH-2 Route */
#define MC68302_SIM_DRB		0x0020	/* CH-1 Route */
#define MC68302_SIM_DRA		0x0010	/* CH-1 Route */
#define MC68302_SIM_MSC3	0x0008	/* SCC3 Connection */
#define MC68302_SIM_MSC2	0x0004	/* SCC2 Connection */
#define MC68302_SIM_MS1		0x0002	/* Mode Supported */
#define MC68302_SIM_MS0		0x0001	/* Mode Supported */

/* SMC1 RX Buffer */
#define MC68302_SMC1_RX_E	0x8000	/* Empty Flag */
#define MC68302_SMC1_RX_L	0x4000	/* Last Flag (EOM) */
#define MC68302_SMC1_RX_ER	0x2000	/* Error Condition */
#define MC68302_SMC1_RX_MS	0x1000	/* Data Mismatch */
#define MC68302_SMC1_RX_RES1	0x0800	/* Reserved */
#define MC68302_SMC1_RX_RES2	0x0400	/* Reserved */
#define MC68302_SMC1_RX_AB	0x0200	/* Received A Bit */
#define MC68302_SMC1_RX_EB	0x0100	/* Received E Bit */
#define MC68302_SMC1_RX_DATA	0x00FF	/* Data Field */

/* SMC1 TX Buffer */
#define MC68302_SMC1_TX_R	0x8000	/* Ready Bit */
#define MC68302_SMC1_TX_L	0x4000	/* Last Bit (EOM) */
#define MC68302_SMC1_TX_AR	0x2000	/* Abort Request */
#define MC68302_SMC1_TX_RES1	0x1000	/* Reserved */
#define MC68302_SMC1_TX_RES2	0x0800	/* Reserved */
#define MC68302_SMC1_TX_RES3	0x0400	/* Reserved */
#define MC68302_SMC1_TX_AB	0x0200	/* Transmit A Bit Value */
#define MC68302_SMC1_TX_EB	0x0100	/* Transmit E Bit Value */
#define MC68302_SMC1_TX_DATA	0x00FF	/* Data Field */

/* SMC2 RX Buffer */
#define MC68302_SMC2_RX_E	0x8000	/* Empty Bit */
#define MC68302_SMC2_RX_DATA	0x003C	/* Data Field */

/* SMC2 TX Buffer */
#define MC68302_SMC2_TX_R	0x8000	/* Ready Bit */
#define MC68302_SMC2_TX_DATA	0x003C	/* Data Field */

/* Timer Mode Register (TMR) */
#define MC68302_TMR_OM		0x0020	/* Output Mode */

/* Timer Event Register (TER) */
#define MC68302_TER_REF		0x02	/* Output Reference Event */
#define MC68302_TER_CAP		0x01	/* Capture Event */

#define TIMER_2MS		30769	/* 2 ms delay counter */
#define TIMER_8S                30048   /* 8 second delay */
#define TIMER_600MS             2254    /* 600 ms timer */

/* Serial Interface Mask Register (SIMASK) */
#define MC68302_56KB_MASK       0x7f
#define MC68302_64KB_MASK       0xff


/* Paramameter Ram Structures */

/* Buffer Descriptor Struct */
typedef struct bri_buf_descr bri_buf_descr_t;
struct bri_buf_descr {
   ushort stat_control;         /* Status and Control Register */
   ushort data_len;             /* Data Length */
   uchar *buffer;               /* Tx or Rx Buffer */
};

/* Protocol Specific Area */
typedef struct prot_specific prot_specific_t;
struct prot_specific {
   ushort temp_rx_crc_low;       /* Temp Rx CRC Low */
   ushort temp_rx_crc_hi;        /* Temp Rx CRC High */
   ushort c_mask_low;            /* CRC Constant Low */
   ushort c_mask_hi;             /* CRC Constant High */
   ushort temp_tx_crc_low;       /* Temp Tx CRC Low */
   ushort temp_tx_crc_hi;        /* Temp Tx CRC High */
   ushort dis_frame_cnt;         /* Discard Frame Counter */
   ushort crc_err_cnt;           /* CRC Error Count */
   ushort abort_seq_cnt;         /* Abort Sequence Counter */
   ushort non_mat_add_rx_cnt;    /* Nonmatching Address Received Counter */
   ushort frame_retrans_cnt;     /* Frame Retransmission Counter */
   ushort max_frame_len;         /* Max Frame Length */
   ushort max_len_cnt;           /* Max Length Counter */
   ushort add_mask;              /* Address mask */
   ushort frame_add[4];          /* Frame Addresses */
};

/* SCC Struct */
typedef struct param_ram_scc param_ram_scc_t; 
struct param_ram_scc {
   bri_buf_descr_t rx_bd[8];     /* Receive BD's */
   bri_buf_descr_t tx_bd[8];     /* Transmit BD's */
   uchar rx_func_code;           /* Rx Function Code */
   uchar tx_func_code;           /* Tx Function Code */
   ushort max_rx_buff_len;       /* Max Rx Buffer length */
   ushort rx_internal_st;        /* Rx Internal State */
   uchar res1;                   /* Reserved */
   uchar rx_int_buf_num;         /* Rx Internal Buffer Number */
   uchar *rx_int_data_pt;        /* Rx Internal Data Pointer */
   ushort rx_int_byte_cnt;       /* Rx Internal Byte Count */
   ushort rx_temp;               /* Rx Temp */
   ushort tx_internal_st;        /* Tx Internal State */
   uchar res2;                   /* Reserved */
   uchar tx_int_buf_num;         /* Tx Internal Buffer Number */
   uchar *tx_int_data_pt;        /* Tx Internal Data Pointer */
   ushort tx_int_byte_cnt;       /* Tx Internal Byte Count */
   ushort tx_temp;               /* Tx Temp */
   prot_specific_t prot_spec;    /* Protocol Specific Area */
   uchar reserved[64];           /* Reserved (not implemented) */
};

/* Internal Register Structure */
typedef struct internal_reg internal_reg_t;
struct internal_reg {
   ushort res1;                  /* Reserved */
   ushort idma_cmr;              /* Internal Mode Register */
   uchar *idma_sapr;/* UNUSED */ /* Source Address Pointer */
   uchar *idma_dapr;/* UNUSED */ /* Destination Address Pointer */
   ushort idma_bcr;              /* Byte Count Register */
   uchar idma_csr;               /* Channel Status Register */
   uchar res2;                   /* Reserved */
   uchar idma_fcr;               /* Function Code Register */
   uchar res3;                   /* Reserved */
   ushort int_gimr;              /* Global Interrupt Mode Register */
   ushort int_ipr;               /* Interrupt Pending Register */
   ushort int_imr;               /* Interrupt Mask Register */
   ushort int_isr;               /* In-Service Register */
   ulong res4;                   /* Reserved */
   ushort pio_pacnt;             /* Port A Control Register */
   ushort pio_paddr;             /* Port A Data Direction Register */
   ushort pio_padat;             /* Port A Data Register */
   ushort pio_pbcnt;             /* Port B Control Register */
   ushort pio_pbddr;             /* Port B Data Direction Register */
   ushort pio_pbdat;             /* Port B Data Register */
   ushort res5;                  /* Reserved */
   ulong res6;                   /* Reserved */
   ushort cs_br0;                /* Base Register 0 */
   ushort cs_or0;                /* Option Register 0 */
   ushort cs_br1;                /* Base Register 1 */
   ushort cs_or1;                /* Option Register 1 */
   ushort cs_br2;                /* Base Register 2 */
   ushort cs_or2;                /* Option Register 2 */
   ushort cs_br3;                /* Base Register 3 */
   ushort cs_or3;                /* Option Register 3 */
   ushort timer_tmr1;            /* Timer Unit 1 Mode Register */
   ushort timer_trr1;            /* Timer Unit 1 Reference Register */
   ushort timer_tcr1;            /* Timer Unit 1 Capture Register */
   ushort timer_tcn1;            /* Timer Unit 1 Counter */
   uchar res7;                   /* Reserved */
   uchar timer_ter1;             /* Timer Unit 1 Event Register */
   ushort timer_wrr;             /* Watchdog Reference Register */
   ushort timer_wcn;             /* Watchdog Counter */
   ushort res8;                  /* Reserved */
   ushort timer_tmr2;            /* Timer Unit 2 Mode Register */
   ushort timer_trr2;            /* Timer Unit 2 Reference Register */
   ushort timer_tcr2;            /* Timer Unit 2 Capture Register */
   ushort timer_tcn2;            /* Timer Unit 2 Counter */
   uchar res9;                   /* Reserved */
   uchar timer_ter2;             /* Timer Unit 2 Event Register */
   ushort res10;                 /* Reserved */
   ulong res11;                  /* Reserved */
   uchar cp_cr;                  /* Command Register */
   uchar reserved[30];           /* Reserved (not implemented) */
};

/* SCC Internal Register Structure */
typedef struct scc_internal_reg scc_internal_reg_t;
struct scc_internal_reg {
   ushort res1;                  /* Reserved */
   ushort scon1;                 /* SCC1 Configuration Register */
   ushort scm1;                  /* SCC1 Mode Register */
   ushort dsr1;                  /* SCC1 Data Sync. Register */
   uchar scce1;                  /* SCC1 Event Register */
   uchar res2;                   /* Reserved */
   uchar sccm1;                  /* SCC1 Mask Regsiter */
   uchar res3;                   /* Reserved */
   uchar sccs1;                  /* SCC1 Status Register */
   uchar res4;                   /* Reserved */
   ulong res5;                   /* Reserved */
   ushort scon2;                 /* SCC2 Configuration Register */
   ushort scm2;                  /* SCC2 Mode Register */
   ushort dsr2;                  /* SCC2 Data Sync. Register */
   uchar scce2;                  /* SCC2 Event Register */
   uchar res6;                   /* Reserved */
   uchar sccm2;                  /* SCC2 Mask Register */
   uchar res7;                   /* Reserved */
   uchar sccs2;                  /* SCC2 Status Register */
   uchar res8;                   /* Reserved */
   ulong res9;                   /* Reserved */
   ushort scon3;                 /* SCC3 Configuration Register */
   ushort scm3;                  /* SCC3 Mode Register */
   ushort dsr3;                  /* SCC3 Data Sync. Register */
   uchar scce3;                  /* SCC3 Event Register */
   uchar res10;                  /* Reserved */
   uchar sccm3;                  /* SCC3 Mask Register */
   uchar res11;                  /* Reserved */
   uchar sccs3;                  /* SCC3 Status Register */
   uchar res12;                  /* Reserved */
   ushort res13;                 /* Reserved */
   ushort spmode;                /* SCP, SMC Mode and Clock Control Register */
   ushort simask;                /* Serial Interface Mask Register */
   ushort simode;                /* Serial Interface Mode Register */
};

/* MC68302 System Structure */
typedef struct mcc68302_system mcc68302_system_t;
struct mcc68302_system {
   uchar reserved[1024];                   /* Reserved */
   param_ram_scc_t scc1_param;             /* SCC1 Parameter Ram */
   param_ram_scc_t scc2_param;             /* SCC2 Parameter Ram */
   param_ram_scc_t scc3_param;             /* SCC3 Parameter Ram */
   uchar reserved1[256];                   /* Reserved */
   internal_reg_t internal_reg;            /* Internal Registers */
   scc_internal_reg_t scc_internal_reg;   /* SCC Internal Registers */
};

typedef struct bri_instance bri_instance_t;
struct bri_instance {
   
   mcc68302_system_t *config_reg; /* Pointer to 68302 config registers */
   ushort rx_size, tx_size;       /* RX and TX ring sizes */
   bri_buf_descr_t *rx_r;         /* Address of RX ring */
   paktype *rx_p[MAX_RING];	  /* Paktype * for buffers in rx ring */
   char rx_head;                  /* Next packet we expect to see */
   char tx_head;                  /* Next packet we expect transmitted */
   char tx_tail;                  /* Last packet in tx ring */
   char tx_count;                 /* Count of packets currently in TX queue */
   bri_buf_descr_t *tx_end;       /* End of the TX Ring */
   bri_buf_descr_t *tx_r;         /* Address of TX ring */
   paktype *tx_p[MAX_RING];       /* Paktype * for buffers in tx ring */
   ushort reset_mask;             /* Reset bit in control register */

   /* Count errors: */
   int d_chan_coll;               /* Number of d-chan collisions */
   int bad_frame_addr;		  /* Received bad frame address */

   uchar last_sid;                /* Last status seen in SID */
};

#define BRI_DS_SIZE (sizeof(bri_instance_t) + 8)
#define BRI_RING_SIZE (MAX_RING * sizeof(bri_buf_descr_t))


/* C/I Channel coding */
/* Control */
#define ST5421_CI_DR		0x0	/* Deactivation Request */
#define ST5421_CI_PDN		0x1	/* Power Down Request */
#define ST5421_CI_AR8		0x8	/* Activate Request Class 8 */
#define ST5421_CI_AR10		0x9	/* Activate Request Class 10 */
#define ST5421_CI_ARL		0xA	/* Activate request Loopback */
#define ST5421_CI_DC		0xF	/* Deactivation Control */

/* Indication*/
#define ST5421_CI_DP		0x0	/* Activation Pending Indication */
#define ST5421_CI_EOM		0x3	/* End of Message */
#define ST5421_CI_EI		0x4	/* Error Indication */
#define ST5421_CI_AP		0x8	/* Activation Pending */
#define ST5421_CI_CON		0x9	/* Contention Indication */
#define ST5421_CI_AI8		0xC	/* Activation Indication Class 8 */
#define ST5421_CI_AI10		0xD	/* Activation Indication Class 10 */
#define ST5421_CI_AIL		0xE	/* Activation Indication Loopback */
#define ST5421_CI_DI		0xF	/* Deactivation Indication */

/* Monitor Channel coding */
/* Device Mode */
#define ST5421_MC_NTA		0x04	/* NT Mode, Adaptive Sampling */
#define ST5421_MC_NTF		0x05	/* NT Mode, Fixed Sampling */
#define ST5421_MC_TES		0x06	/* TE Mode, DSI Slave */
#define ST5421_MC_TEM		0x07	/* TE Mode, DSI Master */
#define ST5421_MC_MMA		0x1F	/* Monitoring Mode Activation */

/* B Channel Configuration */
#define ST5421_MC_BDIR		0x0C	/* B Chan Mapped Direct, B1-B1 B2-B2 */
#define ST5421_MC_BEX		0x0D	/* B Chan Exchanged, B1->B2 B2->B1 */
#define ST5421_MC_B1E		0x14	/* B1 Channel Enabled */
#define ST5421_MC_B1D		0x15	/* B1 Channel Disabled */
#define ST5421_MC_B2E		0x16	/* B2 Channel Enabled */
#define ST5421_MC_B2D		0x17	/* B2 Channel Disabled */

/* End of Message Indication */
#define ST5421_MC_EIE		0x11	/* EOM Interrupt Enabled */
#define ST5421_MC_EID		0x10	/* EOM Interrupt Disabled */

/* Multiframe Processing */
#define ST5421_MC_MIE		0x12	/* Multiframe Circuit & Int Enabled */
#define ST5421_MC_MID		0x13	/* Multiframe Circuit & Int Disabled */
#define ST5421_MC_EN3X		0x28	/* Enable 3X Checking */
#define ST5421_MC_DIS3X		0x29	/* Disable #x Checking */
#define ST5421_MC_MFT		0x30	/* Write to Multiframe XMT Reg */

/* Loopback Test Mode */
#define ST5421_MC_CAL		0x1B	/* Clear All Loopbacks */
#define ST5421_MC_LB1E		0x18	/* Loopback B1 on Line Enable */
#define ST5421_MC_LB2E		0x19	/* Loopback B2 on Line Enable */
#define ST5421_MC_LBS		0x1A	/* Loopback 2B+D Enabled */
#define ST5421_MC_LBB1E		0x1C	/* Loopback B1 on GCI Enabled */
#define ST5421_MC_LBB2E		0x1D	/* Loopback B2 on GCI Enabled */

/*
 * Macro to update rmd and continue so that we look at the next packet:
 */
#define NEXT_READ_PACKET(ds) { \
	if (++(ds->rx_head) >= ds->rx_size) \
		ds->rx_head = 0; \
	goto next_RX_packet; \
	}

/* Prototypes */
void bri_interrupt(hwidbtype *);

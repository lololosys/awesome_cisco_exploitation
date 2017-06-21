/* $Id: pcmap.h,v 3.3 1996/02/23 17:16:09 cshen Exp $
 * $Source: /release/111/cvs/Xsys/src-36-c1000/pcmap.h,v $
 *------------------------------------------------------------------
 * pcmap.h - defines for the Sapphire procject
 *
 * November 1994, Greg Stovall
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcmap.h,v $
 * Revision 3.3  1996/02/23  17:16:09  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.2  1995/11/17  18:40:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:26:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/22  04:36:45  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:52:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Address space allocations and general defines:
 */
#define ADRSPC_END		0x10000000  /* 68360 has a 256mb address space */
#define ADRSPC_PROM		0x00000000  /* Start of EPROM */
#define ADRSPC_PROM_SPC_SIZ	0x00100000  /* Size of space allocated for EPROM */
#define ADRSPC_PCMCIA_ATTR      0x08000000  /* Start of PCMCIA -REG memory   */
#define ADRSPC_PCMCIA_ATSPC_SIZ 0x00010000  /* Size of PCMCIA -REG memory **/
#define ADRSPC_PCMCIA_MEM       0x08010000  /* Start of PCMCIA memory - 1st 64k unuseable */
/* Size of space allocated for PCMCIA memory - compensate for lost 64k */
#define ADRSPC_PCMCIA_MSPC_SIZ  (0x01000000 - 0x10000)
#define ADRSPC_RAM		0x02000000  /* Start of RAM */
#define ADRSPC_RAM_SPC_SIZ	0x02000000  /* Size of space allocated for RAM */
#define ADRSPC_PCMCIA_REGS	0x06000000  /* Start of PCMCIA register area */
#define ADRSPC_PCMCIA_RGSPC_SIZ	0x00010000  /* Size of space allocated for PCMCIA register area */
#define ADRSPC_PCMCIA_IO	0x0C000000  /* Start of PCMCIA IO area */
#define ADRSPC_PCMCIA_IOSPC_SIZ	0x00010000  /* Size of space allocated for PCMCIA IO area */
#define ADRSPC_NVRAM    	0x0E000000  /* Start of NVRAM area */
#define ADRSPC_NVRAM_SPC_SIZ   	0x00020000  /* Size of space allocated for NVRAM */
#define ADRSPC_QUICC_DPRAM	(ADRSPC_END - 0x00100000)      /* where we put it */
#define ADRSPC_QUICC_REGB	(ADRSPC_QUICC_DPRAM + 0x1000)  /* start of internal regs */
#define ADRSPC_BAD_ADDR         0x0badadd0          /* bad address for bus error test */

/*
 * NVRAM address definitions
 */
#define ADRSPC_NVRAM            0x0E000000  /* Start of NVRAM area */
#define NVRAM_MAGIC_OFFSET      0x06     /* Magic number NVRAM config offset */
#define NVRAM_START_OFFSET      0x08     /* NVRAM start offset. This
                                            MUST be LONG aligned */

#define SYSTEM_CLOCK		25000000                       /* 25 Mhz */
#define ADRSPC_SYS_VECT		(ADRSPC_RAM)                   /* system run-time excpt vectors */
#define MONITOR_FREE_MEM_BASE   (ADRSPC_RAM + 0x4000)          /* Load point for system image */
#define MONITOR_STACK           MONITOR_FREE_MEM_BASE
#define MONITOR_TMP_STACK       (ADRSPC_QUICC_DPRAM + 0x400)   /* POCT stack */
#define CLPD6720_DATA_16_REGISTER (*((volatile ushort*)(ADRSPC_PCMCIA_IO+0x3e0)))
#define CLPD6720_INDEX_REGISTER (*((volatile uchar *)(ADRSPC_PCMCIA_IO+0x3e0)))
#define CLPD6720_DATA_REGISTER  (*((volatile uchar *)(ADRSPC_PCMCIA_IO+0x3e1)))
/* QUICC chip defines for Sapphire used by the ROM Monitor */
#define QUICC_MBAR_VALUE        (ADRSPC_QUICC_DPRAM + 1)       /* turn on the valid bit */
#define QUICC_MCR_INIT_VAL      0x00007fff  /* Default MCR value, be sure SHEN on */
#define QUICC_MODCLK1_1_VAL	0xA31f	    /* MODCLK 1 = 1 XTL mode */
#define QUICC_PEPAR_VAL	        0x0480	    /* BCLRO,WE,OE,CAS,CS7 */
#define QUICC_AVR_VAL		0x0	    /* No  autovector */
#define QUICC_SWIV_VAL          0x0f        /* default (uninitialized vector) */
#define QUICC_SDCR_VAL          0x0740;     /* sdma intr serv mask 7, arb id 4 */
#define QUICC_GMR_1M_VAL        0x18cc0d20  /* 1m DRAM chips, 4m SIMM 1 CS or 8m SIMM 2 CS */
#define QUICC_GMR_4M_VAL        0x18d40d20  /* 4m DRAM chips, 16meg SIMM 1 Chip Select */
#define QUICC_BR0_VAL		(ADRSPC_PROM + 0x3)  /* ROM is write protected */
#define QUICC_OR0_VAL		0x3ff00002  /* ROM  00000000 - fffff (internal DSACK) */ 
#define QUICC_BR1_VAL		(ADRSPC_RAM + QUICC_BR_VALID)
#define QUICC_OR1_4MEG_VAL	0x2fc00001  /* DRAM 02000000 - 023fffff 4 Meg (int DSACK) */
#define QUICC_OR1_8MEG_VAL	0x2f800001  /* DRAM 02000000 - 027fffff 8 Meg (int DSACK) */
#define QUICC_OR1_16MEG_VAL	0x2f000001  /* DRAM 02000000 - 02ffffff 16 Meg (int DSACK) */
#define QUICC_BR2_VAL  	        (ADRSPC_RAM + 0x400000 + QUICC_BR_VALID)
#define QUICC_OR2_VAL 	        0x2fc00001  /* upper 4m of 8m SIMM */
#define QUICC_BR3_VAL		0x00080001  /* FIXME ??? Extra ROM socket */
#define QUICC_OR3_VAL		0x3ffe0004  /* FIXME ??? */
#define QUICC_BR4_VAL           (ADRSPC_PCMCIA_REGS + 0x1)
#define QUICC_OR4_VAL           0x0fff0006  /* PCMCIA Register space (06000000 - 0600ffff) */
#define QUICC_BR5_VAL           (ADRSPC_PCMCIA_MEM + 0x1)
#define QUICC_OR5_VAL           0x0f000006  /* PCMCIA common memory (08000000 - 08ffffff) */
#define QUICC_BR6_VAL           (ADRSPC_PCMCIA_IO + 0x1)
#define QUICC_OR6_VAL           0x0fff0006  /* PCMCIA I/O space (0C000000 - 0C00ffff) */
#define QUICC_BR7_VAL           (ADRSPC_NVRAM + 0x3)  /* NVRAM is write protected */
#define QUICC_OR7_32K_VAL       0x5fff8004  /* NVRAM space (0E000000 - 0E007fff) */
#define QUICC_OR7_8K_VAL        0x5fffe004  /* NVRAM space (0E000000 - 0E001fff) */
#define QUICC_CICR_VAL		0x00e49fa1  /* see below */
/*
** QUICC_CICR_VAL
** SCCd - 11 - scc4, SCCc - 10 - scc3, SCCb - 01 - scc2, SCCa - 00 - scc1,
** ILR - 100 - level 4, HP - 11111, VBA - 101 (vector 160), sps - 1 (spread scc's)
*/
#define QUICC_SYPCR_VAL         0xc7   /* see below */
/*
** QUICC_SYPCR_VAL
** Watchdog enabled, Watchdog causes sys reset, SWT = 2^18 input freq,
** enable the double bus fault monitor, disable external bus monitor,
** Bus Monitor Timing = 128 system clocks
*/
#define QUICC_SYPCR_NWD_VAL     0x47   /* without watchdog */

/* Sapphire QUICC SCC usage - these are index numbers, *not* SCC numbers */
#define ENET_SCC_IDX         0         /* use SCC1 for ethernet */
#define ISDN_B_CHAN1_SCC_IDX 1         /* use SCC2 for ISDN B chan 1 */
#define ISDN_D_CHAN_SCC_IDX  2         /* use SCC3 for ISDN D chan */
#define ISDN_B_CHAN2_SCC_IDX 3         /* use SCC4 for ISDN B chan 2 */

/* Other QUICC resource defines */
#define CONSOLE_SMC_IDX      0         /* index for SMC1, our console device */
#define PIT_VECTOR_NUM       31        /* autovector NMI */

/* Sapphire Buffer Descriptor indices in dual port ram */
#define MON_CON_RX_BD_IDX    0         /* Console receiver for the ROM Monitor */
#define MON_CON_TX_BD_IDX    1         /* Console transmitter for the ROM Monitor */
#define MON_SPI_RX_BD_IDX    2         /* SPI receiver */
#define MON_SPI_TX_BD_IDX    3         /* SPI transmitter */


/* QUICC port assignments for Sapphire */

/* Port A PAR (Pin Assignment Register) */
#define SAP_QU_PT_A_PAR_TESTCON  0x4000    /* card edge test connector present (act low) */
#define SAP_QU_PT_A_PAR_ID_RCLK  0x2000    /* ISDN D channel Rx clock */
#define SAP_QU_PT_A_PAR_ID_TCLK  0x1000    /* ISDN D channel Tx clock */
#define SAP_QU_PT_A_PAR_ET_TCLK  0x0800    /* Ethernet Tx clock */
#define SAP_QU_PT_A_PAR_ET_RCLK  0x0200    /* Ethernet Rx clock */
#define SAP_QU_PT_A_PAR_IB_CLK   0x0100    /* ISDN B channel clock */
#define SAP_QU_PT_A_PAR_IB_RDAT  0x0080    /* ISDN B channel receive data */
#define SAP_QU_PT_A_PAR_IB_TDAT  0x0040    /* ISDN B channel transmit data */
#define SAP_QU_PT_A_PAR_ID_TDAT  0x0020    /* ISDN D channel transmit data */
#define SAP_QU_PT_A_PAR_ID_RDAT  0x0010    /* ISDN D channel receive data */
#define SAP_QU_PT_A_PAR_ET_FDX   0x0008    /* Ethernet full duplex mode */
#define SAP_QU_PT_A_PAR_CON_DTR  0x0004    /* Console DTR (Data Terminal Ready) */
#define SAP_QU_PT_A_PAR_ET_TDAT  0x0002    /* Ethernet transmit data */
#define SAP_QU_PT_A_PAR_ET_RDAT  0x0001    /* Ethernet receive data */

#define SAP_QU_PT_A_PAR_INIT     0x3bf3    /* Pin Assignment init value */
#define SAP_QU_PT_A_DIR_INIT     0x84c8    /* Direction init value */
#define SAP_QU_PT_A_ODR_INIT     0x8440    /* Open Drain init value */

/* Port B PAR (Pin Assignment Register) */
#define SAP_QU_PT_B_PAR_RUNLED   0x20000   /* The system run lamp (act low) */
#define SAP_QU_PT_B_PAR_HWREV3   0x10000   /* Hardware rev bit 3 */
#define SAP_QU_PT_B_PAR_HWREV2   0x8000    /* Hardware rev bit 2 */
#define SAP_QU_PT_B_PAR_HWREV1   0x4000    /* Hardware rev bit 1 */
#define SAP_QU_PT_B_PAR_HWREV0   0x2000    /* Hardware rev bit 0 */
#define SAP_QU_PT_B_PAR_10B_POL  0x1000    /* 10BaseT polarity */
#define SAP_QU_PT_B_PAR_WRX_LED  0x0800    /* WAN Rx LED (act. low) */
#define SAP_QU_PT_B_PAR_WTX_LED  0x0400    /* WAN Tx LED (act. low) */
#define SAP_QU_PT_B_PAR_10B_AUT  0x0200    /* 10baseT auto port */
#define SAP_QU_PT_B_PAR_ET_LOOP  0x0100    /* Ethernet loopback */
#define SAP_QU_PT_B_PAR_CON_RXD  0x0080    /* Console read data */
#define SAP_QU_PT_B_PAR_CON_TXD  0x0040    /* Console xmit data */
#define SAP_QU_PT_B_PAR_ET_JABR  0x0020    /* Ethernet jabber signal */
#define SAP_QU_PT_B_PAR_ISDN_CS  0x0010    /* ISDN chip select */
#define SAP_QU_PT_B_PAR_SPI_SDI  0x0008    /* SPI serial data in */
#define SAP_QU_PT_B_PAR_SPI_SDO  0x0004    /* SPI serial data out */
#define SAP_QU_PT_B_PAR_SPI_CLK  0x0002    /* SPI clock */
#define SAP_QU_PT_B_PAR_NV_SEL   0x0001    /* NVRAM select */

#define SAP_QU_PT_B_PAR_INIT     0x000ce   /* Pin Assignment init value */
#define SAP_QU_PT_B_DIR_INIT     0x20f1f   /* Direction init value */
#define SAP_QU_PT_B_ODR_INIT     0x00000   /* Open Drain init value */
#define SAP_QU_PT_B_HWREV_MASK   (SAP_QU_PT_B_PAR_HWREV3 | SAP_QU_PT_B_PAR_HWREV2 | \
			      SAP_QU_PT_B_PAR_HWREV1 | SAP_QU_PT_B_PAR_HWREV0)
#define SAP_QU_PT_B_HWREV(x)     ((x & SAP_QU_PT_B_HWREV_MASK) >> 13)

/* Port C PAR (Pin Assignment Register) */
#define SAP_QU_PT_C_PAR_I_FSA    0x0800    /* ISDN Super Frame Sync */
#define SAP_QU_PT_C_PAR_TPSQEL   0x0200    /* ???input to NTH??? */
#define SAP_QU_PT_C_PAR_TPAPCE   0x0100    /* ???input to DSQE??? */
#define SAP_QU_PT_C_PAR_CON_DSR  0x0080    /* Console Data Set Ready
					  (hdwre says CD) */
#define SAP_QU_PT_C_PAR_SIMM_3   0x0040    /* SIMM type bit 3 */
#define SAP_QU_PT_C_PAR_ET_RENA  0x0020    /* Ethernet receiver enable */
#define SAP_QU_PT_C_PAR_ET_CLSN  0x0010    /* Ethernet collision */
#define SAP_QU_PT_C_PAR_SIMM_2   0x0008    /* SIMM type bit 2 */
#define SAP_QU_PT_C_PAR_SIMM_1   0x0004    /* SIMM type bit 1 */
#define SAP_QU_PT_C_PAR_SIMM_0   0x0002    /* SIMM type bit 0 */
#define SAP_QU_PT_C_PAR_ET_TENA  0x0001    /* Ethernet xmitter enable */

#define SAP_QU_PT_C_PAR_INIT     0x0800    /* Pin Assignment init value */
#define SAP_QU_PT_C_DIR_INIT     0x0780    /* Direction init value */
#define SAP_QU_PT_C_SO_INIT      0x0030    /* Special Options init value */

/* catwoman pin defs */

#define CAT_QU_PT_A_PAR_INIT     0x0000    /* Pin Assignment init value */
#define CAT_QU_PT_A_DIR_INIT     0x0000    /* Direction init value */
#define CAT_QU_PT_A_ODR_INIT     0x0000    /* Open Drain init value */
 
#define CAT_QU_PT_B_PAR_INIT     0x000c0   /* Pin Assignment init value */
#define CAT_QU_PT_B_DIR_INIT     0x20e00   /* Direction init value */
#define CAT_QU_PT_B_ODR_INIT     0x00000   /* Open Drain init value */

#define CAT_QU_PT_C_PAR_INIT     0x0000    /* Pin Assignment init value */
#define CAT_QU_PT_C_DIR_INIT     0x0080    /* Direction init value */
#define CAT_QU_PT_C_SO_INIT      0x0000    /* Special Options init value */


/* Umi platform specific defines are placed here     */

#define FLASH_BASE_UMI       0x8000000
#define QUICC_UMI_OR5_VAL    0x2f000002  /* 08000000 - 08ffffff) */
#define QU_PT_C_FIXED_VPP_CTRL_UMI 0x08

/* HANA platform specific defines are placed here     */

#define QU_PT_C_FIXED_VPP_CTRL_HANA  0x40   /* PC6 */
 
/* End Platform Specific defines */

/* Beginning of platform independent definitions for fixed on-board flash */

#define FLASH_BASE_FIXED      FLASH_BASE_UMI 
#define QUICC_FIXED_BR5_VAL   (FLASH_BASE_FIXED + 0x1) 
#define QUICC_FIXED_OR5_VAL   QUICC_UMI_OR5_VAL

#define ADRSPC_FLASH_BASE    FLASH_BASE_FIXED
#define ADRSPC_FLASH_END     ADRSPC_FLASH_BASE+ 0xffffff /* Assume 16 MB */


/* end of definitions for fixed on-board flash */

/* end of module */


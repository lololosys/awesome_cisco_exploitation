/* $Id: if_mci.h,v 3.1 1995/11/09 09:05:52 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/if_mci.h,v $
 *------------------------------------------------------------------
 * if_mci.h -- Multi-Port Communications Interface Hardware Definitions.
 *
 * 31-January-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_mci.h,v $
 * Revision 3.1  1995/11/09  09:05:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:43  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Location, size, and number of MCI controllers and interfaces
 */
#define MCISTART (ADRSPC_MULTIIO+0xC000)
#define MCIBYTES 0x40
#define MCI_MAXCTRLRS 8
#define MCI_MAXUNITS 4
#define FCI_MAXUNITS 24
#define FCI_MAXCTRLRS 4	
#define MCI_INTLEVEL 3

/*
 * Internal memory sizes
 */
#define MEMD_WORDS 0xFFFF		/* memd may be smaller */
#define MEMA_WORDS 0x07FF
#define AF_WORDS   0x07FF
#define AF_MASK	   0x07FF

/*
 * Applique memory (memx) constants
 */
#define MCI_NVRAM_ADDRESS	0	/* NVRAM base address */
#define MCI_ROM_ADDRESS		0	/* ROM base address */
#define MCI_NVRAM_SIZE	    32768	/* 32K bytes NV config memory */ 


/*
 * MCI controller data structures
 */

/*
 * Struct overlaying the actual MCI registers
 */
typedef struct mciregtype_ {
    volatile ushort cmdreg;
    volatile ushort argreg;
    volatile charlong readreg;
    volatile charlong write0reg;
    volatile charlong write1reg;
    volatile ushort rxstatus;
    volatile ushort rxlength;
    volatile ushort rxtype;
    volatile ushort notused;
    volatile charlong memxreg;	
    volatile ulong asr;
} mciregtype;

/*
 * Definitions for doing 32 and 16 bit register accesses
 */
#define readshort readreg.d.sword[0]
#define readlong readreg.d.lword
#define write0short write0reg.d.sword[0]
#define write0long write0reg.d.lword
#define write1short write1reg.d.sword[0]
#define write1long write1reg.d.lword
#define memxlong memxreg.d.lword

#define dcitype struct dcitype_

/*
 * The MCI/FCI controller data structure
 */
typedef struct mcitype_ {
    struct mcitype_ *next;
    mciregtype *regptr;
    int unit;
    int interface_count;
    int fddi_count;
    int ctrlr_count;
    ushort sw_version;
    ushort hw_version;
    ushort capability;
    ushort memd_size;
    ushort mema_size;
    int interfacetype[FCI_MAXUNITS];
    int protocoltype[FCI_MAXUNITS];
    char *applique[FCI_MAXUNITS];
    dcitype *dci[FCI_MAXCTRLRS];
} mcitype;

/*
 * FCI daughter controller structure.
 */
dcitype {
    mciregtype *regptr;
    mcitype *fci;
    ushort ctrlr_unit;
    ushort ctrlr_type;
    ushort sw_version;
    ushort hw_version;
    ushort capability;
    ushort mema_size;
    ushort interface_count;
};


/*
 * Global command codes
 */
#define MCI_CMD_VERSION		0	/* read version */
#define MCI_CMD_RESET		1	/* reset controller */
#define MCI_CMD_MEMSIZE		2	/* read memory size in words */
#define MCI_CMD_SELECT		3	/* select an interface */
#define MCI_CMD_MEMD_SELECT	4	/* select memd address */
#define MCI_CMD_RX_SELECT	5	/* select offset into RX */
#define MCI_CMD_TX0_SELECT	6	/* select offset into transmit 0 */
#define MCI_CMD_TX1_SELECT	7	/* select offset into transmit 1 */
#define MCI_CMD_RX_SETUP	8	/* setup RX window */
#define MCI_CMD_ASR		9	/* attention summary bit mask */
#define MCI_CMD_CONDITIONS	10	/* error, DCD, and TX done summary */
#define MCI_CMD_RX_MOVE		11	/* move RX buffer to TX1 window */
#define MCI_CMD_INTERRUPT	12	/* define interrupt level */
#define MCI_CMD_RX_FLUSH	13	/* move or flush current RX buffer */
#define MCI_CMD_TX0_RESERVE	14	/* reserve a buffer in transmit 0 */
#define MCI_CMD_TX1_RESERVE	15	/* reserve a buffer in transmit 1 */
#define MCI_CMD_RX_CHECK	16	/* validate special RX packet */
#define MCI_CMD_CAPABILITY	17	/* read controller capabilities */
#define MCI_CMD_MEMA_READ	18	/* read word from mema */
#define MCI_CMD_MEMA_WRITE	19	/* write word to mema */
#define MCI_CMD_MEMA_SELECT	20	/* select mema address */
#define MCI_CMD_OVERHEAD	21	/* read and write buffer overhead */
#define MCI_CMD_BUFFERSIZE	22	/* set buffer size for TX or RX */
#define MCI_CMD_TX_ALLOCATE	23	/* allocate a transmit buffer */
#define MCI_CMD_LIGHTS		24	/* write LED's */
#define MCI_CMD_BRIDGE		25	/* FSA bit mask for bridging */
#define MCI_CMD_BRIDGEPROTOCOL	26	/* protocol specific bridging ctl */
#define MCI_CMD_MEMX_SELECT_HIGH 27	/* high 16 bits of memx address */
#define MCI_CMD_MEMX_SELECT_LOW	28	/* low 16 bits of memx address */

/*
 * Interface specific command codes
 */
#define MCI_CMD_STATUS		32	/* read and write controller status */
#define MCI_CMD_STATION		33	/* read Ethernet station address */
#define MCI_CMD_AF_SELECT	34	/* select address filter address*/
#define MCI_CMD_AF_READ		35	/* read address filter mask */
#define MCI_CMD_AF_WRITE	36	/* write address filter mask */
#define MCI_CMD_CARRIER		37	/* read current DCD status */
#define MCI_CMD_ERRORS		38	/* read and reset error counter */
#define MCI_CMD_TX0_START	39	/* start transmitting TX0 */
#define MCI_CMD_TX1_START	40	/* start transmitting TX1 */
#define MCI_CMD_RX_ALLOCATE	41	/* allocate a receive buffer */
#define MCI_CMD_APPLIQUE	42	/* write and read serial applique */
#define MCI_CMD_CLOCKRATE	43	/* bit rate for generating clock */
#define MCI_CMD_CLOCKTYPE	44	/* external vs generated clock */
#define MCI_CMD_DELAY		45	/* transmitter delay counter */
#define MCI_CMD_QUEUE		46	/* maximum output queue length */
#define MCI_CMD_SLIMIT		47	/* min. queue length when switching */

/*
 * Response codes returned by reading command register
 */
#define MCI_RSP_OKAY	       0	/* successful operation */
#define MCI_RSP_ERR_UNK   0x8000	/* unknown error */
#define MCI_RSP_ERR_ILR   0x8001	/* illegal register read */
#define MCI_RSP_ERR_ILW   0x8002	/* illegal register write */
#define MCI_RSP_ERR_UNIT  0x8003	/* impossible unit number */
#define MCI_RSP_ERR_NPRS  0x8004	/* unit not present */
#define MCI_RSP_ERR_FNV   0x8005	/* function not valid */
#define MCI_RSP_ERR_IVO   0x8006	/* invalid offset */
#define MCI_RSP_ERR_NBF   0x8007	/* no available buffers */
#define MCI_RSP_ERR_CMD   0x8008	/* illegal command code */
#define MCI_RSP_ERR_MEM   0x8009	/* no more memory */
#define MCI_RSP_ERR_VLD   0x800A	/* RX_CHECK failed */
#define MCI_RSP_ERR_CHK   0x800B	/* RX_CHECK failed, bad checksum */
#define MCI_RSP_ERR_TTL   0x800C	/* RX_CHECK failed, TTL exceeded */
#define MCI_RSP_ERR_BFR   0x800D	/* buffer already in window */
#define MCI_RSP_ERR_QFL   0x800E	/* output queue full */
#define MCI_RSP_ERR_NCP   0x800F	/* no capability present */

/*
 * Arguments for MCI_CMD_VERSION
 */
#define MCI_SW_VERSION		0 	/* microcode version */
#define MCI_HW_VERSION		1	/* hardware type code */

/*
 * Arguments for MCI_CMD_MEMSIZE
 */
#define MCI_MEMSIZE_MEMA	0 	/* cache memory */
#define MCI_MEMSIZE_MEMD	1	/* main buffer memory */

/*
 * Defined capability bits
 */
#define MCI_CAP_BRIDGE	  0x1		/* bridging supported (per system) */
#define MCI_CAP_SPEED     0x40		/* high speed (per interface) */
					/* this has been shifted right */
					/* by one bit by the microcode */

/*
 * Interface type codes returned by MCI_CMD_SELECT
 */
#define MCI_TYPE_ETHER 0		/* 10MB Ethernet/IEEE 802.3 */
#define MCI_TYPE_SERIAL 1		/* 4MB synchronous serial */

/*
 * Function codes for MCI_CMD_STATUS command
 */
#define MCI_IF_ENABLE		0
#define MCI_IF_DISABLE		1
#define MCI_IF_LOOPBACK		2
#define MCI_IF_NOLOOPBACK	3
#define MCI_IF_PROMISCUOUS	4	/* Ethernet only */
#define MCI_IF_NOPROMISCUOUS	5	/* Ethernet only */

/*
 * Applique type codes
 */
#define MCI_DTE_RS232		0x2
#define MCI_DCE_RS232		0x3
#define MCI_DTE_V35		0x4
#define MCI_DCE_V35		0x5
#define MCI_DTE_RS449		0x6
#define MCI_DCE_RS449		0x7

/*
 * Applique result bits
 */
#define MCI_APP_SENSE1		0x8	/* type code bit */
#define MCI_APP_SENSE0		0x4	/* cisco applique present */
#define MCI_APP_RXC		0x2
#define MCI_APP_TXC		0x1

/*
 * CLOCKTYPE values
 */
#define MCI_CLK_EXT	0		/* external clock */
#define MCI_CLK_GEN2	2		/* generated clock, prescalar 2 */
#define MCI_CLK_GEN3	3		/* generated clock, prescalar 3 */

/*
 * Flags for MCI_CMD_TX?_START
 */
#define MCI_TX_HEARSELF		0x8000	/* Ethernet only */
#define MCI_TX_ODDALIGN		0x4000	/* Ethernet only */

/*
 * Definitions for word returned by MCI_CMD_CONDITIONS 
 */
#define MCI_COND_ERROR 0x8000		/* error counters need reading */
#define MCI_COND_DCD   0x4000		/* carrier detect changed state */
#define MCI_COND_MASK  0x00FF		/* count of TX buffers posted done */

/*
 * Error counter indices
 */
#define MCI_ERR_GIANTS		0	/* oversize datagrams */
#define MCI_ERR_NOBUFFERS	1	/* no internal buffering */
#define MCI_ERR_ACQUIRE		2	/* Ethernet - acquisition failure */
#define MCI_ERR_COLLISIONS	3	/* Ethernet - collisions */

/*
 * Status bits returned for serial interfaces
 */
#define MCI_SX_ABORT	0x2		/* ABORT received */
#define MCI_SX_OVERRUN	0x4		/* receiver overrun */
#define MCI_SX_FRAME	0x8		/* short frame or unexpected flag */
#define MCI_SX_CRC	0x10		/* CRC error */

#define MCI_SX_MASK (MCI_SX_CRC|MCI_SX_ABORT|MCI_SX_OVERRUN|MCI_SX_FRAME)

/*
 * Status bits returned for Ethernet interfaces
 */
#define MCI_EX_OVERRUN	0x1		/* receiver overrun */
#define MCI_EX_CRC	0x2		/* CRC error */
#define MCI_EX_FRAME	0x4		/* improper framing */
#define MCI_EX_RUNT	0x8		/* short packet */

#define MCI_EX_MASK (MCI_EX_CRC|MCI_EX_RUNT|MCI_EX_OVERRUN|MCI_EX_FRAME)

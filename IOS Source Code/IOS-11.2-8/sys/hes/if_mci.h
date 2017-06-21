/* $Id: if_mci.h,v 3.5.4.2 1996/08/12 16:01:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_mci.h,v $
 *------------------------------------------------------------------
 * if_mci.h -- Multi-Port Communications Interface Hardware Definitions.
 *
 * 31-January-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_mci.h,v $
 * Revision 3.5.4.2  1996/08/12  16:01:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5.4.1  1996/03/18  19:42:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  06:50:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/02/20  00:52:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/07  12:30:53  dkerr
 * CSCdi50949:  Customer can crash router getting cbus lowcore dump
 * Set lowcore capture trigger by default and have results displayable
 * from "show controller cxbus memd-core".
 *
 * Revision 3.4  1996/02/13  08:12:17  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3.26.1  1996/01/24  21:46:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:18:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:41:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_MCI_H__
#define __IF_MCI_H__

/*
 * Location, size, and number of MCI controllers and interfaces
 */
#define MCISTART (MULTIIOBASE+0xC000)
#define MCIBYTES 0x40
#define MCI_MAXCTRLRS 8
#define MCI_MAXUNITS 10
#ifdef C7000
#define FCI_MAXUNITS 80	     /* number of C-bus interfaces you can have */
#define FCI_MAX_DCI_CTRLRS 5 /* number of C-bus interface cards you can have */
#else
#define FCI_MAXUNITS 32
#define FCI_MAX_DCI_CTRLRS 4	
#endif

#define MCI_INTLEVEL 3

extern int fci_max_dci_ctrlrs;

extern int nMCI;

/*
 * Internal memory sizes
 */
#define MEMD_WORDS 0x3FFFF		/* memd may be smaller */
#define MEMA_WORDS 0xFFFF		/* mema may be smaller */
#define AF_WORDS   0x07FF
#define AF_MASK	   0x07FF

/*
 * Applique memory (memx) constants
 */
#define MCI_NVRAM_ADDRESS	0	/* NVRAM base address */
#define MCI_ROM_ADDRESS		0	/* ROM base address */
#define MCI_NVRAM_SIZE	    32768	/* 32K bytes NV config memory */ 

/*
 * Miscellaneous constants
 */
#define MCI_KEEP_COUNT	  3


/*
 * MCI controller data structures
 */

/*
 * Struct overlaying the actual MCI registers
 */
struct mciregtype_ {
    volatile ushort cmdreg;
    volatile ushort argreg;
    volatile charlong readreg;		/* all accesses           */
    volatile charlong write0reg;	/* process level access   */
    volatile charlong write1reg;	/* interrupt level access */
    volatile ushort rxstatus;
    volatile ushort rxlength;
    volatile ushort rxtype;
    volatile ushort rxsrbinfo;
    volatile charlong memxreg;	
    volatile ulong asr;
    volatile ulong largreg;
    volatile ushort rw_key;
    volatile ushort rw_cache;
    volatile charlong asr0;
    volatile ushort reg22;
    volatile ushort reg23;
    volatile ushort reg24;
    volatile ushort reg25; 
    volatile ushort reg26;
    volatile ushort reg27;
    volatile ushort reg28;
    volatile ushort reg29;
    volatile ushort reg30;
    volatile ushort reg31;
};

#define SNIFFBASE   0			/* zero offset of sniff buffer */

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
#define rx2tx1bcopy asr0.d.sword[0]
#define asrext0 asr0.d.lword
#define sp_debug reg31
#define rx_select asr0.d.sword[1]
#define tx1_select reg22
#define rxw_select reg23
#define asrext1 reg26

/*
 * The MCI/FCI controller data structure
 */
struct mcitype_ {
    struct mcitype_ *next;
    mciregtype *regptr;
    int unit;
    int interface_count;	 /* dimension of interface_list array--not
				  * necesarilly the same as the number of
				  * interfaces.  Some cards (ie., the MIP)
				  * hang multiple interfaces off of one
				  * applique.  This then is the count of
				  * appliques in use.  This is only supported
				  * for the cbus controller on the multibus.
				  * Other MCI controllers assume that there
				  * is a one to one mapping.
				  */
    int fddi_count;
    int hssi_count;
    int ether_count;
    int token_count;
    int aip_count;
    int cip_count;
    int feip_count;
    uint memd_size;
    uint mema_size;
    ushort sw_version;
    ushort hw_version;
    ushort capability;
    ulong largest_buffer;
    sys_timestamp lastreset;
    boolean restartdeferred;
    boolean resetting;		/* cbus only.  true if cbus is resetting */
    boolean configuring;	/* cbus only.  true if cbus is configuring */
    boolean tx1bug;
    boolean flags;				/* cbus only */
    int critical;
    int system_buffer_size;			/* cbus only */
    int system_buffer_pool;
    hwidbtype *interface_list[FCI_MAXUNITS];
    int bufferalloc[FCI_MAXUNITS];
    int rxbuffers[FCI_MAXUNITS];
    int txbuffers;
    int buffers[8];                 /* size of buffer pool in words */
    int if_sizes[8];                /* size of buffer pool in bytes */
    int if_sizes_count;             /* number of entries in if_sizes */
    ushort lights;
    ushort led_mask[FCI_MAXUNITS];
    ushort led_carrier[FCI_MAXUNITS];
    ushort led_transmit[FCI_MAXUNITS];
    ushort led_receive[FCI_MAXUNITS];
    ushort led_error[FCI_MAXUNITS];
    dcitype *dci[FCI_MAX_DCI_CTRLRS];
    int max_interfaces;
    boolean sse_present;
    struct ssetype_ *sseptr;		/* pointer to SSE info */
};

/*
 * FCI daughter controller structure.
 */
struct dcitype_ {
    dcitype* next;
    mciregtype *regptr;
    mcitype *fci;
    ushort ctrlr_unit;
    ushort ctrlr_type;
    ushort interface_count;   /* see comment for mcitype_->interface_count */
    hwidbtype *interface_list[FCI_MAXUNITS / FCI_MAX_DCI_CTRLRS];
    ushort sw_version;
    ushort hw_version;
    sys_timestamp lastreset;
    ushort led_mask;
    ushort led_carrier;
    ushort led_transmit;
    ushort led_receive;
    ushort led_error;
    boolean resyncable;   /* can controller resync after FCI_RSP_ERR_TMOUT */
    int ctrlr_sync_count; /* current sync token exchanged with controller */
    int ctrlr_sync_calls; /* number of FCI_RSP_ERR_TMOUTs for this controller */
    int ctrlr_sync_times; /* matches sync_calls if controller is resyncable */
    int ctrlr_sync_phase; /* high count of phase synchronization */
    int ctrlr_sync_fails; /* resynchronization failures */
    int pa_type[2];       /* type of port adapter on a VIP */
};


/*
 * Notes on buffer sizing:
 * The largest packet we must handle is an HDLC bridged, maximum sized
 * Ethernet datagram.  The header in this case is 4 bytes, and the data
 * may be up to 1517 bytes (European interpretation of IEEE).  If we
 * allocate a data buffer of 1520 bytes, this datagram will be received
 * just fine by the HDLC receiver.  The total size of this datagram is
 * 1521 bytes, so we should allow (MAXSERIALSIZE+ENCAPBYTES) == 1524
 * byte datagrams to be transmitted from process level.
 */

#define	MCI_BUFFERSIZE_DEF	760	/* 760 words/1520 bytes data */
#define	MCI_BUFFERSIZE_BYTES	(MCI_BUFFERSIZE_DEF << 1)
#define	MCI_MAXDGRAM_DEF	1524	/* bytes, for buffer size checking */
#define	MCI_OFFSETSIZE		50	/* 50 words encapsulation */
#define	MCI_DOUBLE_BUFFER	512	/* KBPS double buffering capacity */

/*
 * These defines should not be used by code throughout the system.
 * Most code should be referencing hwidb->rxoffset instead.
 *
 * These defines should also be moved into media specific high
 * end include files. For example, MCI_ETHER_OFFSET should really
 * live in 'hes/if_hes_ether.h'.
 */
#define MCI_ETHER_OFFSET    (MCI_OFFSETSIZE - 7) /* 43 words from zero */
#define MCI_SERIAL_OFFSET   (MCI_OFFSETSIZE - 2) /* 48 words from zero */
#define	MCI_SERIAL_SMDS_OFFSET (MCI_OFFSETSIZE - 23) /* 27 words from zero */


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
#define MCI_CMD_IPC0_START      28      /* queue lcl ipc pkt from process lvl */
#define MCI_CMD_IPC1_START      30      /* queue lcl ipc pkt from int lvl  */

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
#define MCI_CMD_CTS_DELAY       48      /* set cts delay timer for HDX sdlc */
#define MCI_CMD_CTS_WATCHDOG    49      /* set cts watchdog timer on DTE */

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
 * Actual HW return values are defined in if_fci.h
 */

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
#define MCI_TYPE_ETHER  0		/* 10MB Ethernet/IEEE 802.3 */
#define MCI_TYPE_SERIAL 1		/* 4MB synchronous serial */
					/* 2-7 used by cbus cards */

/*
 * Finite State Machine assignments.  These share the same space as the
 * address filter constants defined in if_fci.h.
 */
#define FSM_STATION	0		/* level 3 station address */
#define FSM_BROADCAST	1		/* the broadcast address */
#define FSM_2		2		/* level 3 multicast addresses - Not used anymore (now BROADCAST) */
#define FSM_3		3		/* MAC frames (tr), HSRP(ether,fddi) */
#define FSM_BRIDGE	4		/* promiscuous address (for MCI) */
#define FSM_5		5		/* unassigned */
#define FSM_6		6		/* unassigned */
#define FSM_7		7		/* unassigned */

#define MCI_BRIDGING_MASK		(1 << FSM_BRIDGE)

/*
 * Bits to indicate whether we are routing a specific protocol.
 */
#define MCI_ROUTING_IP 		0x01
#define MCI_ROUTING_DECNET 	0x02
#define MCI_ROUTING_XNS		0x04
#define MCI_ROUTING_CLNS	0x08
#define MCI_ROUTING_APPLE	0x10
#define MCI_ROUTING_NOVELL	0x20
#define MCI_ROUTING_APOLLO	0x40
#define MCI_ROUTING_VINES	0x80

/*
 * Function codes for MCI_CMD_STATUS command
 *
 * After a Disable 200mS needs to be burned waiting for the
 * interface to clean everything up.  With the larger number
 * of buffers floating around this can take a while.
 */
#define MCI_IF_ENABLE		0
#define MCI_IF_DISABLE		1
# define IF_DISABLE_TIME	500	/* 200 mS */
#define MCI_IF_LOOPBACK		2
#define MCI_IF_NOLOOPBACK	3

#define MCI_IF_PROMISCUOUS	4	/* Ethernet only */
#define MCI_IF_NOPROMISCUOUS	5	/* Ethernet only */

#define MCI_IF_BURST            4       /* SDLC HDX only */
#define MCI_IF_DCE              5       /* DCE/DTE indicator */
#define MCI_IF_HDX              6       /* SDLC HDX/FDX indicator */

#define MCI_IF_READ_STATUS_REG  7	/* SIP serial only */

/*
 *  Mask used to turn the MSB on/off for SDLC HDX support
 */
#define MCI_SDLC_ON_MASK        0x8000

/* mask for idb->status_reg */
#define DCD_MASK		0x0004
#define DSR_MASK		0x0008
#define CTS_MASK		0x0010

/*
 * Applique type codes
 */
#define MCI_APP_UNKNOWN         0
#define MCI_UNIVERSE_NO_CABLE	0x1
#define MCI_DTE_RS232		0x2
#define MCI_DCE_RS232		0x3
#define MCI_DTE_V35		0x4
#define MCI_DCE_V35		0x5
#define MCI_DTE_RS449		0x6
#define MCI_DCE_RS449		0x7
#define MCI_DTE_X21		0x8
#define MCI_DCE_X21		0x9
#define MCI_DTE_RS232_NRZI	0xA
#define MCI_DCE_RS232_NRZI	0xB
#define MCI_DXE_G703_UNBAL	0xC
#define MCI_DXE_G703_BAL	0xD
#define MCI_DTE_RS530		0xE
#define MCI_DCE_RS530		0xF
#define MCI_DTE_V35_NRZI	0x10
#define MCI_DCE_V35_NRZI	0x11

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
#define MCI_CLK_GEN1	1		/* generated clock, prescalar 1 */
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

#define MCI_ERR_RTS_TIMEOUT    3        /* Only true for half duplex */
#define MCI_ERR_HALF_DUPLEX_VIOLATION 4 /* Only true for half duplex */

/*
 * Status bits returned for Ethernet interfaces
 */
#define MCI_EX_OVERRUN	0x1		/* receiver overrun */
#define MCI_EX_CRC	0x2		/* CRC error */
#define MCI_EX_FRAME	0x4		/* improper framing */
#define MCI_EX_RUNT	0x8		/* short packet */

#define MCI_EX_MASK (MCI_EX_CRC|MCI_EX_RUNT|MCI_EX_OVERRUN|MCI_EX_FRAME)

/*
 * Transmitter-delay/Tx-burst-deferral defines for Ethernet interface
 */
#define ETH_TXBD_MASK     0xFFFE0000 /* mask to obtain tx-burst-deferral value*/
#define ETH_TXBD_SHIFT_GET  17       /* bits to shift to get tx-burst-deferral*/
#define ETH_TXBD_SHIFT_PUT  9        /* bits to shift to put tx-burst-deferral*/
#define ETH_TXD_MASK        0xFF
#define ETH_OLD_TXBD_MASK   0xFF00

#define SP_REGS        64       /* number of SP general purpose registers */ 
#define SP_REGS_DUMP   60       /* Cmd to dump SP general purpose registers */ 
#define DUMP_SPREGS     0
#define DUMP_MEMD       1
#define DUMP_MEMA       2

typedef struct dump_t_ {
    ushort *memd_dump_ptr;
    ushort *mema_dump_ptr;
    ushort *spreg_dump_ptr;
    int mema_alloced;
    int spreg_captured;
    int mema_captured;
    int memd_captured;
    int memd_capture_ok;
} dump_t;


/***********************************************************************
 *
 *			   Inline Functions
 *
 ***********************************************************************/

static inline boolean mci_supports_fastswitching (hwidbtype *idb)
{
    return(!((struct mcitype_ *)idb->devctl)->tx1bug);
}


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern queuetype cbusQ;
extern ushort mci_cbus_intflag;

/*
 * if_mci.c
 */
extern int cbus_mci_base_ether;
extern int cbus_mci_base_serial;
extern queuetype mciQ;

boolean mci_controller(mciregtype *);
boolean mci_init(mcitype *, boolean);
int mci_analyze(void);
void mci_input_error(hwidbtype *);
void mci_memx_select(mciregtype *, ulong);
void mci_warning(void);
int mci_txqcount(hwidbtype *);
void mci_set_transmit_delay(hwidbtype *idb, int value);
long mci_mc_read_buffer(hwidbtype *, uint, uchar *, long);
long mci_mc_write_buffer(hwidbtype *, void *, uint, long);
void mci_mc_write_short(hwidbtype *, ushort, uint); 
ushort mci_mc_read_short(hwidbtype *, uint); 

extern void mci_applique(hwidbtype *);

/*
 * mci_chain.c
 */
extern void mci_parser_init(void);

/*
 * "cbus" routines that reference mcitype, mciregtype
 */

extern void cbus_init(mcitype *, boolean);
extern void cbus_idb_rx_offset(mciregtype *, ushort);
extern void cbus_rx_buffer_parking(mciregtype *, int, ushort);
extern void cbus_idb_rx_max_byte_size(mciregtype *, ushort);
extern void cbus_idb_rx_error(mciregtype *, ushort);
extern void cbus_idb_assign_pool(mciregtype *, ushort);

extern void sp_eoir_quiescere(mcitype*);

#endif __IF_MCI_H__

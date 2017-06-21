/* $Id: if_les_mk5025.h,v 3.3.60.2 1996/08/28 12:58:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_mk5025.h,v $
 *------------------------------------------------------------------
 * if_les_mk5025.h -- Definitions for les High Speed Serial Port
 * 
 * March 1992, Marciano Pitargue
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_mk5025.h,v $
 * Revision 3.3.60.2  1996/08/28  12:58:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.1  1996/04/03  01:59:51  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.3  1995/11/17  17:41:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:37:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct mk5025_rx_descriptor mk5025_rx_descriptor_t;
typedef struct mk5025_rx_ring mk5025_rx_ring_t;
typedef struct mk5025_tx_descriptor mk5025_tx_descriptor_t;
typedef struct mk5025_tx_ring mk5025_tx_ring_t;
typedef struct mk5025_status_buffer mk5025_status_buffer_t;
typedef struct mk5025_initblock mk5025_initblock_t;


#if defined(XX) || defined(SIERRA)
#include "if_c4000_mk5025.h"
#endif /* XX and Sierra */

#if defined(PAN)
#include "if_c3000_mk5025.h"
#endif /* PAN */

/*
 * HDLC flag defines
 */
#define MAX_DELAY_FLAGS 62
#define MAX_FLAG_INDEX  31


/*
 * RAP:
 */
#define MK5025_RAP_BM8		0x0080	/* Put mk5025 in byte mode (UNUSED) */
#define MK5025_RAP_HBYTE	0x0001	/* Set to select high byte of word */
#define MK5025_RAP_NORMAL	0
#define MK5025_RAP(reg_num)	((reg_num << 1) | MK5025_RAP_NORMAL)

/*
 * MK5025_CSR0 (Selected when rap == MK5025_CSR0):
 * [These bits are valid in all modes]
 */
#define MK5025_CSR0		MK5025_RAP(0)
#define MK5025_CSR0_TDMD	0x8000 /* Start transmitter */
#define	MK5025_CSR0_STOP	0x4000 /* MK5025 is stopped (READ ONLY!) */
#define MK5025_CSR0_DTX		0x2000 /* Disable Transmit Ring activity */
#define MK5025_CSR0_DRX		0x1000 /* Disable Receive Ring activity */
#define MK5025_CSR0_TXON	0x0800 /* Transmitter on (read only) */
#define MK5025_CSR0_RXON	0x0400 /* Receiver on (read only) */
#define	MK5025_CSR0_INEA	0x0200 /* Interrupt enable */
#define	MK5025_CSR0_INTR	0x0100 /* Some interrupt occured */
#define	MK5025_CSR0_MERR	0x0080 /* Memory error */
#define	MK5025_CSR0_MISS	0x0040 /* Receiver lost a packet */
#define MK5025_CSR0_ROR		0x0020 /* Receiver overrun error */ 
#define MK5025_CSR0_TUR		0x0010 /* Transmitter underrun error */
#define MK5025_CSR0_PINT	0x0008 /* Primitive interrupt */
#define	MK5025_CSR0_TINT	0x0004 /* Transmitter interrupt */
#define MK5025_CSR0_RINT	0x0002 /* Receiver interrupt */
/* MK5025_CSR0 bit 0 is unused */
#define MK5025_CSR0_ERRORS \
 (MK5025_CSR0_MERR | MK5025_CSR0_MISS | MK5025_CSR0_ROR | MK5025_CSR0_TUR)
#define MK5025_CSR0_INTERRUPTS \
 (MK5025_CSR0_PINT | MK5025_CSR0_TINT | MK5025_CSR0_RINT)
#define MK5025_CSR0_NORMAL \
 (MK5025_CSR0_TXON | MK5025_CSR0_RXON | MK5025_CSR0_INEA)

/*
 * MK5025_CSR1 (Selected when rap == MK5025_CSR1).
 * [Most bits except PPARM are valid in all modes - Two user primitives
 *  and one provider primitive change meaning when in transparent mode]:
 */
#define MK5025_CSR1		MK5025_RAP(1)
#define MK5025_CSR1_UERR	0x8000 /* User primitive error */
#define MK5025_CSR1_UAV		0x4000 /* User primitive is available */
/*
 * MK5025_CSR1 bits 13 and 12 are UPARM - Parameters for User primitives.
 * The Connect and Reset primitives expect one of the following values,
 * all other primitives expect zero in this field:
 */
#define MK5025_CSR1_UPARM_RESEND	0x0000 /* Resend unacked frames */
#define MK5025_CSR1_UPARM_DISCARD	0x1000 /* Discard unacked frames */
/*
 * MK5025_CSR1 bits 11, 10, 09, and 08 are the User Primitive command:
 */
#define MK5025_CSR1_UPRIM_MASK		0x0F00 /* Mask to select UPRIM bits */
#define MK5025_CSR1_UPRIM_STOP		0x0000 /* Stop the mk5025 */
#define MK5025_CSR1_UPRIM_START		0x0100 /* Start the mk5025 */
#define MK5025_CSR1_UPRIM_INIT		0x0200 /* Initialize */
#define MK5025_CSR1_UPRIM_TRANS		0x0300 /* Enter Transparent mode */
#define MK5025_CSR1_UPRIM_STATUS	0x0400 /* Get link status */
#define MK5025_CSR1_UPRIM_SELF_TEST	0x0500 /* Perform mk5025 self test */
#define MK5025_CSR1_UPRIM_CONNECT_REQ	0x0600 /* Attempt Connect to remote */
#define MK5025_CSR1_UPRIM_CONNECT_RESP	0x0700 /* Respond willing to connect */
/* Note that the following two commands change meanings in transparent mode.
   (See below) */
#define MK5025_CSR1_UPRIM_RESET_REQ	0x0800 /* Attempt Reset logical link */
#define MK5025_CSR1_UPRIM_RESET_RESP	0x0900 /* Respond willing to reset */
#define MK5025_CSR1_UPRIM_XID_REQ	0x0A00 /* Send XID command to remote */
#define MK5025_CSR1_UPRIM_XID_RESP	0x0B00 /* Send XID Response to remote */
#define MK5025_CSR1_UPRIM_TEST_REQ	0x0C00 /* Send Test command to remote */
#define MK5025_CSR1_UPRIM_TEST_RESP	0x0D00 /* Send Test response to remote */
#define MK5025_CSR1_UPRIM_DISCON_REQ	0x0E00 /* Disconnect the logical link */
/*
 * These Two User Primitives change meanings when in transparent mode:
 */
#define MK5025_CSR1_UPRIM_START_T1	0x0800 /* Start T1 timer */
#define MK5025_CSR1_UPRIM_STOP_T1	0x0900 /* Stop T1 timer */


#define MK5025_CSR1_PLOST		0x0080 /* Provider primitive lost */
#define MK5025_CSR1_PAV			0x0040 /* Provider primitive available */

/*
 * MK5025_CSR1 bits 05 and 04 are PPARM - Provider Parameters
 * Provide additional information about the reason for the receipt of a
 * Disconnect, Reset, or Error Indication primitive (undefined for others):
 */
#define MK5025_CSR1_PPARM_UNSOLICITED	0x0000 /* Remote Initiated */
#define MK5025_CSR1_PPARM_TIMEOUT	0x0010 /* Timeout (SABM/DISC/Rec) */
#define MK5025_CSR1_PPARM_FRMR		0x0020 /* Frame Error */
#define MK5025_CSR1_T3_TIMEOUT		0x0030 /* T3 Timeout */
/*
 * MK5025_CSR1 bits 03, 02, 01, and 00 are the provider primitive command.
 * Written by the mk5025 to inform the user of link control conditions:
 */
#define MK5025_CSR1_PPRIM_MASK		0x000F /* Mask to select PPRIM bits */
#define MK5025_CSR1_PPRIM_INIT_CONF	0x0002 /* Initialization complete */
#define MK5025_CSR1_PPRIM_ERROR		0x0004 /* An Error occured (see PPARM) */
#define MK5025_CSR1_PPRIM_CONNECT_IND	0x0006 /* Remote site attempting Connection */
#define MK5025_CSR1_PPRIM_CONNECT_CONF	0x0007 /* Connect request succeeded */
/* Note that the following command changes meanings in transparent mode.
   (See below) */
#define MK5025_CSR1_PPRIM_RESET_IND	0x0008 /* Remote site attempting Reset */
#define MK5025_CSR1_PPRIM_RESET_CONF	0x0009 /* Reset request succeeded */
#define MK5025_CSR1_PPRIM_XID_IND	0x000A /* Recieved XID command */
#define MK5025_CSR1_PPRIM_XID_CONF	0x000B /* Recieved XID response */
#define MK5025_CSR1_PPRIM_TEST_IND	0x000C /* Recieved TEST command */
#define MK5025_CSR1_PPRIM_TEST_CONF	0x000D /* Recieved TEST response */
#define MK5025_CSR1_PPRIM_DISCON_IND	0x000E /* Received Disconnect request */
#define MK5025_CSR1_PPRIM_DISCON_CONF	0x000F /* Disconnect request succeeded */
/*
 * This User Primitives changes meanings when in transparent mode:
 */
#define MK5025_CSR1_PPRIM_T1_EXPIRED	0x0008 /* T1 Timer expired */

/*
 * MK5025_CSR2 (Selected when rap == MK5025_CSR2):
 */
#define MK5025_CSR2		MK5025_RAP(2)
/*
 * Bit definitions when not in transparent mode:
 */
#define MK5025_CSR2_FRMRD	0x2000 /* Disable sending FRMR frames */
#define MK5025_CSR2_T203E	0x1000 /* T3 Timer is LAPD T203 timer */
#define MK5025_CSR2_X75E	0x0800 /* Enable X.75 mode */
#define MK5025_CSR2_PROM	0x0400 /* Enable Promiscuous mode */
#define MK5025_CSR2_UIE		0x0200 /* Enable UI frame recongnition */
#define MK5025_CSR2_XIDE	0x0100 /* Enable XID frame recognition */
/*
 * Only the following bits are valid when in Transparent mode:
 */
#define MK5025_CSR2_PROM	0x0400 /* Enable Promiscuous mode */
#define MK5025_CSR2_ENGLB	0x0100 /* Enable Global address recognition */
#define MK5025_CSR2_NORMAL	(MK5025_CSR2_PROM|MK5025_CSR2_ENGLB)
/*
 * MK5025_CSR2 bits 07 through 00 are IADR<23:16>, the high byte of the address
 * of the initialization block.
 */



/*
 * MK5025_CSR3 (Selected when rap == MK5025_CSR3):
 */
#define MK5025_CSR3		MK5025_RAP(3)
/*
 * MK5025_CSR3 bits 15 through 00 are IADR<15:00>, the low short word
 * of the initialization block address:
 */

/*
 * MK5025_CSR4 (Selected when rap == MK5025_CSR4). Bits valid in all modes:
 */
#define MK5025_CSR4		MK5025_RAP(4)
#define MK5025_CSR4_FWM_25	0x0000 /* Set FIFO Watermark to 25 words */
#define MK5025_CSR4_FWM_17	0x0100 /* Set FIFO Watermark to 17 words */
#define MK5025_CSR4_FWM_9	0x0200 /* Set FIFO Watermark to 9 words */
#define MK5025_CSR4_FWM_1	0x0300 /* Set FIFO Watermark to 1 word */
#define MK5025_CSR4_BAEN	0x0080 /* Enable Address<23:20> when HOLD */
#define MK5025_CSR4_BUSR	0x0040 /* Set for pin 15 as *BUSREL */
#define MK5025_CSR4_BSWPC	0x0020 /* Byte Swap during non-data DMA */
#define MK5025_CSR4_BURST_1	0x0000 /* Burst size of one word */
#define MK5025_CSR4_BURST_8	0x0010 /* Burst size of eight words */
#define MK5025_CSR4_BURST_U	0x0008 /* Burst size is unlimited */
#define	MK5025_CSR4_BSWPD	0x0004 /* Byte Swap during data DMA */
#define	MK5025_CSR4_ACON	0x0002 /* ALE Control */
#define	MK5025_CSR4_BCON	0x0001 /* Byte Control */
#define MK5025_CSR4_NORMAL \
 (MK5025_CSR4_FWM_9 | MK5025_CSR4_BURST_8 | MK5025_CSR4_BSWPD)


/*
 * MK5025_CSR5 (Selected when rap == MK5025_CSR5). Bits valid in all modes:
 */
#define MK5025_CSR5		MK5025_RAP(5)
#define MK5025_CSR5_RTSEN	0x0010 /* pin 26 as *RTS and pin 30 as *CTS */
#define MK5025_CSR5_DTRD	0x0008 /* Set for DTR / *RTS as output pin */
#define MK5025_CSR5_DSRD	0x0004 /* Set for DSR / *CTS as output pin */
#define MK5025_CSR5_DTR_RTS_	0x0002 /* DTR / *RTS data bit (write if DTRD) */
#define MK5025_CSR5_DSR_CTS_	0x0001 /* DSR / *CTS data bit (write if DSRD) */
#define MK5025_CSR5_NORMAL	(MK5025_CSR5_DTRD)

/*
 * Decription of a Receive Ring Entry:
 */

struct mk5025_rx_descriptor {
    volatile uchar  status;		/* Status bits */
    volatile uchar  buffer_high;	/* High part of address */
    volatile ushort buffer_low;		/* Address of buffer */
    volatile ushort max_buffer_size;	/* Negative of buffer length */
    volatile ushort packet_size;	/* Negative of packet length */
};

/*
 * Receive descriptor ring pointer
 * First short has RLEN and high byte of ring address.
 * Second short has low two bytes of ring address:
 */
#define MK5025_GET_HI(first_word)	(first_word & 0xFF)
#define MK5025_GET_RLEN(first_word)	((first_word >> 12) & 7)
#define MK5025_GET_NEG_W(count)		((-count) & 0xFFFF)
#define MK5025_GET_NEG_B(count)		((-count) & 0xFF)

#define MK5025_MAKE_PTR(high,low)	(((high) << 16) | (low))

/*
 * Macros used to initialize the ring pointers:
 */
#define MK5025_BUFF_LO(addr) (ushort) ((ulong)(addr) & 0xFFFF)
#define MK5025_BUFF_HI(addr) (ushort) (((ulong)(addr) >> 16) & 0xFF)

struct mk5025_rx_ring {
    mk5025_rx_descriptor_t ring_entry[MAX_RING];
};

/*
 * Macro used to get actual 16Meg IO page
 */

#define MK5025_16M_PAGE(addr) ((ulong)(addr) & 0xFF000000)

/*
 * Definition of Receive status bits:
 */
#define	MK5025_RMD_OWNA		0x80 /* Buffer is owned by mk5025 */
#define MK5025_RMD_OWNB		0x40 /* Buffer is owned by I/O processor */
#define MK5025_RMD_SLF		0x20 /* Start of Long frame */
#define MK5025_RMD_ELF		0x10 /* End of Long frame */
#define MK5025_RMD_UIR		0x08 /* This buffer holds a UI frame */
#define MK5025_RMD_FRMRR	0x04 /* This buffer holds a FRMR I-field */
#define MK5025_RMD_RADR		0x02 /* Frame addressed to our address */
#define MK5025_RMD_RPF		0x01 /* State of P or F bit of frame */

/*
 * Transmit descriptor ring pointer
 * First short has TLEN, TWD, and high byte of ring address.
 * Second short has low two bytes of ring address:
 */
#define MK5025_GET_TLEN(first_word)	((first_word >> 12) & 7)
#define MK5025_GET_TWD(first_word)	((first_word >> 8) & 7)
#define MK5025_TWD	7	/* Transmit Window Size */

/*
 * Decription of a Transmit Ring entry:
 */
struct mk5025_tx_descriptor {
    volatile uchar  status;	       /* Status bits */
    volatile uchar  buffer_high;       /* High part of address */
    volatile ushort buffer_low;	       /* Address of buffer */
    volatile ushort max_buffer_size;   /* Negative of buffer length (UNUSED) */
    volatile ushort packet_size;       /* Negative of packet length */
};

struct mk5025_tx_ring {
    mk5025_tx_descriptor_t ring_entry[MAX_RING];
};

/*
 * Definition of Transmit status bits:
 */
#define	MK5025_TMD_OWNA		0x80 /* Buffer is owned by mk5025 */
#define MK5025_TMD_OWNB		0x40 /* Buffer is owned by I/O Processor */
#define MK5025_TMD_SLF		0x20 /* Start of Long Frame */
#define MK5025_TMD_ELF		0x10 /* End of Long Frame */
#define MK5025_TMD_TUI		0x08 /* Transmit a UI Frame */
#define MK5025_TMD_TINTD	0x04 /* Transmit Interrupt Disable */
#define MK5025_TMD_XPF		0x01 /* PF flag data value */



/*
 * Status Buffer Block:
 */
struct mk5025_status_buffer {
    volatile uchar rcv;		/* Receive Count Variable */
    volatile uchar tcv;		/* Transmit Count Variable */
    volatile uchar local_state; /* State of local station */
    volatile uchar remote_state; /* State of remote station */
    volatile uchar phase;	/* Phase of Operation */
    volatile uchar tac;		/* Transmit Acknowledge Count */
    volatile uchar unused1;	/* UNUSED */
    volatile uchar currd_hi;	/* Receive Descriptor address MSB */
    volatile ushort currd_lo;	/* Low Receive Descriptor address */
    volatile uchar unused2;	/* UNUSED */
    volatile uchar curxd_hi;	/* Transmit Descriptor address MSB */
    volatile ushort curxd_lo;	/* Low Transmit Descriptor address */
};

/*
 * Definition of the mk5025 MODE word:
 */

/*
 * Mode<15:11> are the Minimum Frame spacing field. This is the minimum
 * number of flags between succesive transmitted frames:
 */
#define MK5025_MODE_MFS_SHIFT 11

#define MK5025_MODE_EXTCF	0x0400 /* Extended Control Force */
#define MK5025_MODE_EXTAF	0x0200 /* Extended Address Force */
#define MK5025_MODE_DACE	0x0100 /* Disable Address/Control Extraction */
#define MK5025_MODE_EXTC	0x0080 /* Extended Control Field */
#define MK5025_MODE_EXTA	0x0040 /* Enable Extended address fields */
#define MK5025_MODE_DRFCS	0x0020 /* Disable Receiver FCS */
#define MK5025_MODE_DTFCS	0x0010 /* Disable Tranmitter FCS */
#define MK5025_MODE_FCSS	0x0008 /* Select 16 bit FCS, not 32 bit */
/*
 * Mode<02:00> defines the Loopback control field:
 */
#define MK5025_MODE_NO_LOOPBACK	0x0000 /* Normal Operation */
#define MK5025_MODE_SIMPLE_LOOP	0x0004 /* Loopback TX Clock & TX Data */
#define MK5025_MODE_NOCLK_LOOP	0x0005 /* Clockless Loopback (uses SYSCLOCK) */
#define MK5025_MODE_SILENT_LOOP	0x0006	/* Silent Loopback, TX Data = 1 */
#define MK5025_MODE_SILENT_NOCLK_LOOP 0x0007 /* Silent Clockless Loopback */

#define MK5025_MODE_NORMAL	(MK5025_MODE_DACE | MK5025_MODE_FCSS)

/*
 * Initialization block:
 */

struct mk5025_initblock {
    /*
     * MK5025 Initialization block - allocated in IO RAM
     * Defined by hardware - do not change!
     */
    volatile ushort mode;		/* Operational mode */
    volatile ushort local_addr;		/* Local station address */
    volatile ushort remote_addr;	/* Remote station address */
    volatile ushort N1;			/* Counter N1 (Max frame length) */
    volatile uchar  N2;			/* Counter N2 (Max retransmit count) */
    volatile uchar  scaler;		/* Scaler for T1, T3, and TP */
    volatile ushort T1;			/* T1 Timer (retransmission timer) */
    volatile ushort T3;			/* T3 Timer (link idle timer) */
    volatile ushort TP;			/* TP Timer (transmit polling timer) */
    volatile ushort mk5025_rx_ring[2];	/* Receive Ring Address */
    volatile ushort mk5025_tx_ring[2];	/* Transmit Ring Address */
    volatile mk5025_tx_descriptor_t xid_tx_desc; /* XID/TEST Transmit Descriptor */
    volatile mk5025_rx_descriptor_t xid_rx_desc; /* XID/TEST Receive Descriptor */
    volatile ushort sba[2];		/* Status Buffer Address */
    volatile ushort bad_frames;		/* Count of bad frames (FCS/Align) */
    volatile ushort frmrs;		/* Count of FRMR frames received */
    volatile ushort T1_timeouts;	/* Count of T1 timeouts */
    volatile ushort rej_rxs;		/* Count of recieved REJ frames */
    volatile ushort rej_txs;		/* Count of tranmitted REJ frames */
    volatile ushort runts;		/* Count of Runt frames */
};

#define MK5025_DS_SIZE (sizeof(mk5025_instance_t) + 8)
#define MK5025_IB_SIZE (sizeof(mk5025_initblock_t))
#define MK5025_RX_SIZE ((MAX_RING * sizeof(mk5025_rx_descriptor_t)) + 16)
#define MK5025_TX_SIZE ((MAX_RING * sizeof(mk5025_tx_descriptor_t)) + 16)

/*
 * Macro to update rmd and continue so that we look at the next packet:
 */
#define NEXT_READ_PACKET(ds) { \
	if (++(ds->rx_head) >= ds->rx_size) \
	        ds->rx_head = 0; \
		goto next_RX_packet; \
        }

/*
 * Prototypes
 */
void mk5025_init_idb_common(hwidbtype *idb);
void mk5025_init(hwidbtype *idb);
void mk5025_RX_interrupt(hwidbtype *idb);
void mk5025_TX_interrupt(hwidbtype *idb);
void mk5025_E_interrupt(ushort csr0_status, hwidbtype *idb);


/*
 * Externs
 */
extern void mk5025_subsys_init(subsystype *subsys);
extern void mk5025_init_config_options(hwidbtype *idb);
extern ushort mk5025_get_interface_state(hwidbtype *idb,
					 boolean transition_valid);
extern void mk5025_clockrate_command(parseinfo *csb);

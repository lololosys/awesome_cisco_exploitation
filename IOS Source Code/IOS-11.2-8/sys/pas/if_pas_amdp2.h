/* $Id: if_pas_amdp2.h,v 3.4.44.5 1996/05/24 04:11:19 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_amdp2.h,v $
 *------------------------------------------------------------------
 * if_pas_amdp2.h - 8e Presidio2 based PCI port adaptor support
 *		    (Platform independent components)
 *
 * April 1995, Dave Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_amdp2.h,v $
 * Revision 3.4.44.5  1996/05/24  04:11:19  mbeesley
 * CSCdi58493:  Tweak PCI settings for new arbiters
 * Branch: California_branch
 *
 * Revision 3.4.44.4  1996/05/07  01:22:37  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/04/23  16:28:51  feisal
 * CSCdi52937:  loss of carrier detected on 5EF/VIP2
 * Branch: California_branch
 *
 * Revision 3.4.44.2  1996/03/29  04:05:27  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.4.44.1  1996/03/21  23:23:19  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/02/19  05:25:33  dcarroll
 * CSCdi49299:  Fix 4e frame miss count handling
 * Branch: ELC_branch
 * Disable miss frame counter overflow interrupt.
 *
 * Revision 3.2.2.7  1996/02/12  23:32:39  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch. Fix support for
 * receiving single particle packets into memd.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.6  1996/02/10  00:30:41  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/01/18  05:41:53  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.2.2.4  1996/01/17  23:37:45  dcarroll
 * Branch: ELC_branch
 * Make 4e fully tx polled.
 *
 * Revision 3.2.2.3  1996/01/16  01:42:47  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/04  22:18:22  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.2.2.1  1995/11/30  02:05:05  dcarroll
 * Branch: ELC_branch
 * Revision 3.4  1995/12/19  20:24:24  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.3  1995/11/28  02:26:07  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:54:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include platform dependant header file here to pick up definitions.
 * They are included once more at the end of this file to pick
 * up static inline definitions.
 */
#if defined(RVIP) || defined(SVIP)
#include "if_vip_amdp2.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_amdp2.h"
#endif

/*
 * Define some useful constants
 */
#define AMDP2_MAX_PACKET	1518	/* Max size packet */

#define AMDP2_MAGIC_PAK_PTR	((paktype *)0x03)
#define AMDP2_CRC_SIZE		4

#define AMDP2_LINK_UP		1
#define AMDP2_LINK_DOWN		2

#define AMDP2_STOP_DELAY	1    	/* 1 ms */
#define AMDP2_RESET_DELAY	5	/* 5 us */
#define AMDP2_MAX_SUSPEND_LOOP	20	/* loop 20 times for a suspend */
#define AMDP2_MAX_INIT_POLL	20	/* loop 20 times for init done */

#define AMDP2_VENDOR_DEVICE_ID	0x20001022

/*
 * PCI Latency timer values
 */
#define AMDP2_LAT_TIMER_0	0x00000000
#define AMDP2_LAT_TIMER_4	0x00000400
#define AMDP2_LAT_TIMER_8	0x00000800
#define AMDP2_LAT_TIMER_16	0x00001000
#define AMDP2_LAT_TIMER_24	0x00001800
#define AMDP2_LAT_TIMER_255	0x0000FF00

/*
 * AMD P2 device register set and defintions
 */
typedef struct amdp2_regs_t_ {
    volatile unsigned short aprom[8];
    volatile unsigned long rdp;      	/* data port for CSR reg */
    volatile unsigned long rap;      	/* CSR/BCR reg # to be accessed */
    volatile unsigned long reset;
    volatile unsigned long bdp;      	/* data port for Bus Config Reg */
} amdp2_regs_t;

/*
 * Define register CSR 0
 */
#define AMDP2_CSR0		0      	/* CSR 0 */

#define AMDP2_CSR0_ERR          0x8000  /* Some error has occured */
#define AMDP2_CSR0_BABL         0x4000  /* Tranmitter is babbling */
#define AMDP2_CSR0_CERR         0x2000  /* Collision err after SQE test */
#define AMDP2_CSR0_MISS         0x1000  /* receiver lost a packet */
#define AMDP2_CSR0_MERR         0x0800  /* memory error */
#define AMDP2_CSR0_RINT         0x0400  /* receiver interrupt */
#define AMDP2_CSR0_TINT         0x0200  /* Transmitter interrupt */
#define AMDP2_CSR0_IDON         0x0100  /* Initialization done */
#define AMDP2_CSR0_INTR         0x0080  /* Some interrupt */
#define AMDP2_CSR0_IENA         0x0040  /* Interrupt enable */
#define AMDP2_CSR0_RXON         0x0020  /* Receiver on (read only) */
#define AMDP2_CSR0_TXON         0x0010  /* Transmitter on (read only) */
#define AMDP2_CSR0_TDMD         0x0008  /* Start transmitter */
#define AMDP2_CSR0_STOP         0x0004  /* Stop all external activity */
#define AMDP2_CSR0_STRT         0x0002  /* Start chip */
#define AMDP2_CSR0_INIT         0x0001  /* Initialize */

/*
 * CSR0 error summary
 */
#define AMDP2_CSR0_ERRBITS      				\
    (AMDP2_CSR0_ERR|AMDP2_CSR0_BABL|AMDP2_CSR0_CERR|		\
     AMDP2_CSR0_MISS|AMDP2_CSR0_MERR)


#define AMDP2_CSR0_NORMAL       				\
    (AMDP2_CSR0_IENA|AMDP2_CSR0_RXON|AMDP2_CSR0_TXON|		\
     AMDP2_CSR0_STRT)

/*
 * Define CSR1 - low 16 bits of init block address
 */
#define AMDP2_CSR1 		1

/*
 * Define CSR2 - high 16 bits of init block address
 */
#define AMDP2_CSR2 		2

/*
 * Define CSR3
 */
#define AMDP2_CSR3    		3

#define AMDP2_CSR3_BABLM       	0x4000	/* Babble Mask */
#define AMDP2_CSR3_MISSM       	0x1000  /* Missed Frame Mask */
#define AMDP2_CSR3_MERRM       	0x0800  /* Memory Error Mask */
#define AMDP2_CSR3_RINTM       	0x0400  /* Receive Interrupt Mask */
#define AMDP2_CSR3_TINTM       	0x0200  /* Transmit Interrupt Mask */
#define AMDP2_CSR3_IDONM       	0x0100  /* Initialization Done Mask */
#define AMDP2_CSR3_DXSUFLO     	0x0040  /* Disable Transmit Stop on 
					   Underflow error */
#define AMDP2_CSR3_LAPPEN      	0x0020  /* Look Ahead Packet Processing
					   Enable */
#define AMDP2_CSR3_DXMT2PD     	0x0010  /* Disable Transmit Two Part 
					   Deferral */
#define AMDP2_CSR3_EMBA        	0x0008  /* Enable Modified Back-off 
					   Algorithm */
#define AMDP2_CSR3_BSWP        	0x0004  /* Byte Swap ( = 1 => big Endian 
					   mode */
/*
 * Define CSR4 - Test and Feature control
 * Note : We use no interrupts from this register
 */
#define AMDP2_CSR4		4

#define AMDP2_CSR4_DMAPLUS     	0x4000  /* Disable DMA Burst Xfer Ctr (PCI) */
#define AMDP2_CSR4_DPOLL     	0x1000  /* Disable TX Polling */
#define AMDP2_CSR4_AUTO_PAD_TX  0x0800 	/* Enable automatic padding */
#define AMDP2_CSR4_AUTO_STRP_RX 0x0400 	/* Enable automatic pad stripping */
#define AMDP2_CSR4_MFCO        	0x0200 	/* Missed frame counter overflow */
#define AMDP2_CSR4_MFCOM       	0x0100 	/* Mask MFCO bit from setting INTR */
#define AMDP2_CSR4_RCVCCOM     	0x0020
#define AMDP2_CSR4_TXSTRTM     	0x0004
#define AMDP2_CSR4_JABM        	0x0001

/*
 * Defined CSR5 - Extended Control and Interrupt
 */
#define AMDP2_CSR5		5

#define AMDP2_CSR5_SINT	       	0x0800	/* System Interrupt (err during 
					   Bus Master Xfer) */
#define AMDP2_CSR5_SINT_ENABLE 	0x0400
#define AMDP2_CSR5_SUSPEND     	0x0001

/*
 * CSR8 - CSR11:  Logical address filter
 */
#define AMDP2_CSR8		8
#define AMDP2_CSR9		9
#define AMDP2_CSR10		10
#define AMDP2_CSR11		11

/*
 * CSR12 - CSR14: Physical address
 */
#define AMDP2_CSR12		12
#define AMDP2_CSR13		13
#define AMDP2_CSR14		14

/*
 * Define CSR15 -  Mode, copied from init block
 */
#define AMDP2_CSR15		15

#define AMDP2_MODE_PROM         0x8000 	/* Promiscuous Mode */
#define AMDP2_MODE_DRCVBC	0x4000  /* Disable Receive Broadcast */
#define AMDP2_MODE_DRCVPA	0x2000  /* Disable Receive Physical Address */
#define AMDP2_MODE_DLNKTST      0x1000  /* Disable Link Status (monitor 
					   Link Pulses) */
#define AMDP2_MODE_DAPC   	0x0800  /* Disable Automatic Polarity 
					   Correction */
#define AMDP2_MODE_MENDECL      0x0400  /* MENDEC Loopback Mode. */
#define AMDP2_MODE_LRT          0x0200  /* Low Receive Threshold (T-MAU) */
#define AMDP2_MODE_TSEL         0x0200  /* Transmit Mode Select  (AUI) */
#define AMDP2_MODE_PORTSEL_MASK 0x0180  /* Port Select bits valid if 
					   ASEL = 0 */
#define AMDP2_MODE_INTL         0x0040  /* Internal Loopback - See _LOOP */
#define AMDP2_MODE_DRTY         0x0020  /* Disable Retry */
#define AMDP2_MODE_FCOLL        0x0010  /* Force Collision */
#define AMDP2_MODE_DXMTFCS      0x0008  /* Disable Transmit CRC (FCS) */
#define AMDP2_MODE_LOOP         0x0004  /* Loopback Enable */
#define AMDP2_MODE_DTX          0x0002  /* Disable Transmit */
#define AMDP2_MODE_DRX          0x0001  /* Disable Receiver */

/*
 * Define CSR28 - Current Receive Descriptor Address Lower
 */
#define AMDP2_CSR28		28

/*
 * Define CSR29 - Current Receive Descriptor Address Upper
 */
#define AMDP2_CSR29		29

/*
 * Define CSR34 - Current Transmit Descriptor Address Lower
 */
#define AMDP2_CSR34		34

/*
 * Define CSR35 - Current Transmit Descriptor Address Upper
 */
#define AMDP2_CSR35		35

/*
 * Define CSR47 - TX Polling Interval.
 * The polling interval reg contains the 2's complement
 * of the polling period, where each bit is equal to
 * one clock period. For a polling time of 350 us:
 *
 * two_comp(25MHz * 350 us) = 0x2220
 */

#define AMDP2_CSR47		47
#define AMDP2_CSR47_POLLING_INTERVAL 0x2220

/*
 * Define CSR76 - Receive Ring Length
 * Note : contains two's complement of length
 */
#define AMDP2_CSR76		76

/*
 * Define CSR78 - Transmit Ring Length
 * Note : contains two's complement of length
 */
#define AMDP2_CSR78		78

/*
 * Define CSR80 - DMA Control
 */
#define AMDP2_CSR80		80

#define AMDP2_RXFIFO_16 	0x0000
#define AMDP2_RXFIFO_64 	0x1000
#define AMDP2_RXFIFO_128 	0x2000

#define AMDP2_TXFIFO_START_8	0x0000
#define AMDP2_TXFIFO_START_64	0x0400
#define AMDP2_TXFIFO_START_128  0x0800
#define AMDP2_TXFIFO_START_248	0x0C00

#define AMDP2_TXFIFO_WMARK_16	0x0000
#define AMDP2_TXFIFO_WMARK_64	0x0100
#define AMDP2_TXFIFO_WMARK_128	0x0200

/*
 * Define CSR100 - Bus Timeout interval
 */
#define AMDP2_CSR100           100
#define AMDP2_CSR100_BUS_TIMEOUT 0xf000
 
/*
 * Define CSR112 - Missed frame counter
 */
#define AMDP2_CSR112           112

/*
 * Define BCR2 - media specific bus configuration register.
 */
#define AMDP2_BCR2		2

#define AMDP2_BCR2_ASEL_ENABLE  0x0001	/* default is enabled */
#define AMDP2_BCR2_ASEL_DISABLE 0x0000

/*
 * Define BCR4 - Led specific.
 */
#define AMDP2_BCR4		4

#define AMDP2_BCR4_PSE      	0x0080	/* LED pulse stretcher enable */
#define AMDP2_BCR4_LNKSTE   	0x0040	/* Link Status Enable */
#define AMDP2_BCR4_LEDOUT   	0x8000

/*
 * Define BCR5 - LED1 status.
 */
#define AMDP2_BCR5		5

#define AMDP2_BCR5_LEDDIS   	0x2000	/* LED Disable */

/*
 * Define BCR6 - LED2 status.
 */
#define AMDP2_BCR6		6

#define AMDP2_BCR6_LEDDIS   	0x2000	/* LED Disable */
#define AMDP2_BCR6_LEDPOL	0x4000	/* LED pin is asserted high */
#define AMDP2_BCR6_FDLSE	0x0100  /* Full-duplex Link Status Enable */

/*
 * Define BCR7 - LED3 status.
 */
#define AMDP2_BCR7		7

#define AMDP2_BCR7_LEDDIS   	0x2000	/* LED Disable */

/*
 * Define BCR9 - Full duplex control
 */
#define AMDP2_BCR9		9

#define AMDP2_BCR9_FDEN		0x0001

/*
 * Define BCR18 - burst size/type of the bus interface.
 */
#define AMDP2_BCR18		18

#define AMDP2_BCR18_MEMCMD	0x0200
#define AMDP2_BCR18_EXTREQ    	0x0100
#define AMDP2_BCR18_DWIO   	0x0080
#define AMDP2_BCR18_BREADE 	0x0040
#define AMDP2_BCR18_BWRITE 	0x0020
#define AMDP2_BCR18_LINBC 	0x0001

/*
 * Define BCR18
 */
#define AMDP2_BCR19		19

/*
 * BCR20: Software Style
 */
#define AMDP2_BCR20		20

#define AMDP2_BCR20_LANCE      	0x0000
#define AMDP2_BCR20_CSRPCNET   	0x0200
#define AMDP2_BCR20_SSIZE32    	0x0100
#define AMDP2_BCR20_SW_STYLE3  	0x0103

/*
 * Init block structure. Big Endian format
 */
typedef struct amdp2_initblock_t_ {
    volatile ushort tlen_rlen;	      	/* TLEN in 15:12; RLEN in 7:4 */
    volatile ushort mode;	       	/* operational modes */
    volatile ushort phy_addr_31_16;   	/* Ethernet addr 31:16 */
    volatile ushort phy_addr_15_0;     	/* Ethernet addr 15:0  */
    volatile ushort reserve;          	/* Filler */
    volatile ushort phy_addr_47_32;    	/* Ethernet addr 47:32 */
    volatile ushort mc_filter_bits[4];	/* Multicast (Logical Addr Filter) */
    volatile ulong  rx_ring_addr;      	/* Receive Descriptor Ring Address */
    volatile ulong  tx_ring_addr;      	/* Transmit Descriptor Ring Address */
} amdp2_initblock_t;

/*
 * Receive ring data structures and definitions - Big endian format
 */
typedef struct amdp2_rx_desc_t_ {
    volatile uchar  rcc;       		/* receive collision count */
    volatile uchar  rpc;       		/* runt pkt count since last good rx */
    volatile ushort pkt_size;  		/* length of received data */
    volatile ushort status;    		/* status */
    volatile ushort buf_size;  		/* length of buffer (negative) */
    volatile ulong  buf_addr;  		/* pointer to data buffer */
    volatile ulong  reserved;  		/* filler */
} amdp2_rx_desc_t;

#define	AMDP2_RMD_OWN	0x8000 		/* Buffer is owned by device */
#define AMDP2_RMD_ERR	0x4000 		/* OR of various errors */
#define AMDP2_RMD_FRAM	0x2000 		/* Framing error */
#define AMDP2_RMD_OFLO	0x1000 		/* Rx FIFO overflowed -- no buffer */
#define AMDP2_RMD_CRC	0x0800 		/* CRC error */
#define AMDP2_RMD_BUFF	0x0400 		/* ran out of buffers */
#define AMDP2_RMD_STP	0x0200 		/* buffer contains start of packet */
#define AMDP2_RMD_ENP	0x0100 		/* buffer contains end of packet */
#define AMDP2_RMD_BPE	0x0080 		/* Bus parity error (when 
					   BCR20:10 enables) */
#define AMDP2_RMD_PAM	0x0040 		/* Physical address match */
#define AMDP2_RMD_LAFM	0x0020 		/* Logical address filter match */
#define AMDP2_RMD_BAM	0x0010 		/* Broadcast address match */
#define AMDP2_RMD_RES	0x000F 		/* Reserved bits */

/*
 * Summary of receive error bits
 */
#define AMDP2_RMD_ERRBITS  					\
    (AMDP2_RMD_ERR | AMDP2_RMD_FRAM | AMDP2_RMD_CRC | 		\
     AMDP2_RMD_BUFF | AMDP2_RMD_BPE)

/*
 * Transmit ring data structures and definitions - Big endian format
 */
typedef struct amdp2_tx_control_t_ {
    ushort status;     		/* Primary status */
    ushort buf_size;   		/* length of buffer (negative) */
} amdp2_tx_control_t;

typedef struct amdp2_tx_desc_t_ {
    volatile ulong  status2;   /* Secondary status */
    volatile union {
	amdp2_tx_control_t s; 	/* Primary status and buffer length */
	ulong              w;            
    } cntl;
    volatile ulong  buf_addr;  /* pointer to data buffer */
    volatile ulong  reserved;  /* filler */
} amdp2_tx_desc_t;

typedef struct amdp2_tx_shadow_t_ {
    paktype *pak;		/* pointer to pak */
    particletype *mp;		/* pointer to particle */
} amdp2_tx_shadow_t;

#define	AMDP2_TMD_OWN   0x8000  /* Buffer is owned by device */
#define AMDP2_TMD_ERR   0x4000  /* OR of various errors */
#define AMDP2_TMD_AFCS  0x2000  /* Dynamically control FCS generation */
#define AMDP2_TMD_MORE  0x1000  /* More than one retry was required */
#define AMDP2_TMD_ONE   0x0800  /* One retry */
#define AMDP2_TMD_DEF   0x0400  /* Had to defer */
#define AMDP2_TMD_STP   0x0200  /* Buffer contains start of packet */
#define AMDP2_TMD_ENP   0x0100  /* Buffer contains end of packet */
#define AMDP2_TMD_BPE   0x0080  /* Bus parity error (when BCR20:10 enables) */
#define AMDP2_TMD_RES   0x007F  /* Reserved bits */

#define AMDP2_TMD0_BUFF	 	0x80000000 	/* ENP never found */
#define AMDP2_TMD0_UFLO	 	0x40000000 	/* Underflow */
#define AMDP2_TMD0_EXDEF 	0x20000000 	/* Excessive deferral */
#define AMDP2_TMD0_LCOL	 	0x10000000 	/* Late collision */
#define AMDP2_TMD0_LCAR	 	0x08000000 	/* Loss of carrier */
#define AMDP2_TMD0_RTRY	 	0x04000000 	/* Max retries exceeded */
#define AMDP2_TMD0_TDR	 	0x03FF0000 	/* TDR count */
#define AMDP2_TMD0_TRC		0x0000000F	/* Tx retry count mask */
#define AMDP2_TDR_SHIFT  	16

/*
 * Summary if transmit errors, status
 */
#define AMDP2_TMD_ERR_SUMMARY					\
    (AMDP2_TMD_ONE | AMDP2_TMD_MORE |				\
     AMDP2_TMD_DEF | AMDP2_TMD_ERR)

/* 
 * Summary of transmit errors, from status2
 */
#define AMDP2_TMD0_ERR_SUMMARY					\
    (AMDP2_TMD0_BUFF | AMDP2_TMD0_UFLO | AMDP2_TMD0_LCOL | 	\
     AMDP2_TMD0_LCAR | AMDP2_TMD0_RTRY)

/*
 * Use LSB and vendor code byte as hash value
 */
#define AF_HASH(mac_addr) ((mac_addr[IEEEBYTES-4]) ^ (mac_addr[IEEEBYTES-1]))  

/*
 * AMDP2 software mac addr filter structure
 */
typedef struct af_entry_type_ {
    struct af_entry_type_ *next;
    union {
    uchar byte[8];
    ushort sword[4];
    ulong lword[2];
    } mac_addr;   	/* 48-bit IEEE MAC address */
    union {
    uchar byte[8];
    ushort sword[4];
    ulong lword[2];
    } mask;   	/* mask for multicast addr range */
    ulong type;		/* type of address & disposition of packets */
    ulong hits;   	/* count the times a match is found */
} af_entry_type;

/*
 * Instance data structure for 8e AMD P2 interfaces
 */
typedef struct amdp2_instance_t_ {

    pas_cbtype pacb;			/* Universal PA device control blk */
    
    amdp2_regs_t *amdp2_regaddr;      	/* Pointer to device registers */

    /*
     * Receive ring information
     */
    amdp2_rx_desc_t *rxr;		/* Rx ring pointer */
    amdp2_rx_desc_t *rxr_malloc;	/* Rx ring pointer, usable for free */
    uint rx_head, rx_tail;		/* Indexes into rx ring */
    particletype **rxr_shadow;		/* Rx ring shadow */
    paktype *rx_pak;			/* Rx temp paktype pointer */

    /*
     * Transmit ring
     */
    amdp2_tx_desc_t *txr;	       	/* Tx ring pointer */
    amdp2_tx_desc_t *txr_malloc;	/* Tx ring pointer, usable for free */
    uint tx_head, tx_tail;		/* Indexes into tx ring */
    uint tx_count;			/* Count of Tx ring entries used */
    boolean tx_limited;			/* Limit one tx pak at a time */
    amdp2_tx_shadow_t *txr_shadow;	/* Tx ring shadow */

    /*
     * Init block information
     */
    amdp2_initblock_t *ib;        	/* Init block ponter */
    amdp2_initblock_t *ib_malloc;	/* Init block pointer, freeable */

    /* 
     * Count errors
     */
    ulong spurious_idon;        	/* # spurious init done events */
    ulong filtered_pak;			/* Packets filtered in sw */

    /* 
     * Throttle statistics
     */
    ulong throttled;            	/* # times we have been throttled */
    ulong enabled;              	/* # times we have been enabled */
    ulong disabled;             	/* # times we have been disabled */
 
    /* 
     * Receive errors 
     */
    ulong rx_framing_err;       	/* # framing errors */
    ulong rx_overflow_err;      	/* # overflow errors */
    ulong rx_buffer_err;        	/* # buffer errors */
    ulong rx_bpe_err;           	/* # parity errors on bus during rx */
    ulong rx_soft_overflow_err;    	/* # no replacement errors */
    ulong rx_no_enp;			/* # packets without ENP set */
    ulong rx_discard;		       	/* flag for dropping rest of packet */
    ulong rx_miss_count;                /* flag for counting the miss reg */

    /* 
     * Transmit errors
     */
    ulong tx_one_col_err;       	/* # exactly one retry on frame */
    ulong tx_more_col_err;      	/* # more than one retry on frame */
    ulong tx_no_enp;	          	/* # times no ENP in buf & OWN=0 
					   in next */
    ulong tx_deferred_err;      	/* # times chip had to deferr tx */
    ulong tx_underrun_err;      	/* # tx underrun */
    ulong tx_late_collision_err;	/* # tx late collision */
    ulong tx_loss_carrier_err;  	/* # tx lost carrier */
    ulong tx_exc_collision_err; 	/* # excessive collisions */
    ulong tx_buff_err;			/* # of tx buffer errors */

    tinybool hsrp_configured;       	/* HSRP MAC addr is configured */

    tinybool fatal_tx_err;       	/* A fatal tx error occured,
					   requiring a device reset */

    tinybool need_af_check;		/* need to use soft adddr filter */
    af_entry_type **mac_filter;		/* af used when in promiscuous mode */
    af_entry_type **mc_mac_filter;	/* mc af used in prom mode */
} amdp2_instance_t;

/*
 * External globals
 */
extern boolean	ether_debug;		/* debug support */

/*****************************************************************
 * Macros
 *****************************************************************/

/*
 * AMDP2_ADVANCE_TX_RING_INDEX :
 * Advance a ring index for AMDP2 Tx ring
 */
#define AMDP2_ADVANCE_TX_RING_INDEX(index)			\
    index = ((index + 1) & AMDP2_TX_RING_INDEX_MASK);

/*
 * AMDP2_RETRACT_TX_RING_INDEX :
 * Move an AMDP2 Tx ring index backwards
 */
#define AMDP2_RETRACT_TX_RING_INDEX(index)			\
    index = ((index - 1) & AMDP2_TX_RING_INDEX_MASK);

/*
 * AMDP2_ADVANCE_RX_RING_INDEX :
 * Advance a ring index for AMDP2 Rx ring
 */
#define AMDP2_ADVANCE_RX_RING_INDEX(index)			\
    index = ((index + 1) & AMDP2_RX_RING_INDEX_MASK);

/*
 * AMDP2_RETRACT_RX_RING_INDEX :
 * Move an AMDP2 Rx ring index backwards
 */
#define AMDP2_RETRACT_RX_RING_INDEX(index)			\
    index = ((index - 1) & AMDP2_RX_RING_INDEX_MASK);

/*****************************************************************
 * Static inlines
 *****************************************************************/

/*
 * amdp2_rxring_flush :
 * Flush the Rx ring from the data cache
 */
static inline void amdp2_rxring_flush (amdp2_instance_t *ds)
{
    if (AMDP2_RX_RING_ATTRIBUTE == TLB_UNCACHED) {
	return;
    }
}

/*
 * amdp2_flush_pak:
 * NULL out the relevant entries in the paktype
 */
static inline void amdp2_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
}

/*
 * amdp2_cleanup_pak:
 * Return any particles and then NULL out relevant entries in the paktype
 */
static inline void amdp2_cleanup_pak (paktype *pak)
{
    particletype *mp;

    while (mp = particle_dequeue_inline(pak), mp) {
	retparticle(mp);
    }
    pak->datagramstart = NULL;
    pak->datagramsize = 0;
}

/*
 * Function prototypes
 */

/* if_pas_amdp2.c */

extern void 		amdp2_set_rx_discard(amdp2_instance_t *, boolean);
extern void		amdp2_throttle(hwidbtype *);
extern void 		amdp2_rx_error(ushort, hwidbtype *, particletype *);
extern void 		amdp2_tx_error(ushort, uint, hwidbtype *);
extern void 		amdp2_error_interrupt(ulong, hwidbtype *);
extern boolean		amdp2_init(hwidbtype *, boolean);
extern void             amdp2_ladrf_clear(hwidbtype *idb);
extern void             amdp2_add_multicast_address(idbtype *swidb,
                                                    uchar *mac, uchar fsm);
extern void             amdp2_add_multicast_range(idbtype *swidb,
                                                  uchar *mac, uchar *mask);

/* if_ [platform] _amdp2.c */

extern hwidbtype	*amdp2_idb_create(uint slot, uint port);
extern void		amdp2_deallocate(hwidbtype *);
extern boolean		amdp2_platform_init(hwidbtype *);
extern void		amdp2_show_instance(amdp2_instance_t *);
extern void		amdp2_snmp_link_trap(int, hwidbtype *);
extern void 		amdp2_lineproto_change(hwidbtype *idb, boolean);
extern void		amdp2_get_default_hw_address(hwidbtype *, uchar *);
extern boolean		amdp2_init_buffer_pools(hwidbtype *);
extern void		amdp2_tx_start(hwidbtype *);
extern void		amdp2_fastsend(hwidbtype *, paktype *);
extern particletype	*amdp2_get_particle(hwidbtype *);

/* asm wrapper file */

extern pa_wrapper_t	amdp2_eim_wrapper;

/*
 * Include platform dependant header file again to pick up static 
 * inline definitions. Note, this is the second inclusion of these files.
 */

#define AMDP2_STATIC_INLINES_ONLY

#if defined(RVIP) || defined(SVIP)
#include "if_vip_amdp2.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_amdp2.h"
#endif

#undef AMDP2_STATIC_INLINES_ONLY

/* end of file */

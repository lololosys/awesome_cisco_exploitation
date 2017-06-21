/* $Id: if_pas_dec21140.h,v 3.7.10.9 1996/08/26 18:23:53 feisal Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_dec21140.h,v $
 *------------------------------------------------------------------
 * if_pas_dec21140.h -- definitions for DEC 21140 100mb Ether PA
 *			(Platform indepenent parts)
 *
 * February 1995, Steve Jacobson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_dec21140.h,v $
 * Revision 3.7.10.9  1996/08/26  18:23:53  feisal
 * CSCdi45144:  4E/1FE error handling cleanup
 * Branch: California_branch
 *
 * Revision 3.7.10.8  1996/08/20  18:20:49  tboyle
 * CSCdi64994:  vip1 FE output stuck, lost packets, bad dual full-duplex
 * performance
 * Branch: California_branch
 *
 * Revision 3.7.10.7  1996/06/02  01:35:44  feisal
 * CSCdi55143:  Fast-Ethernet cable removal is not detected
 * Branch: California_branch
 *
 * Revision 3.7.10.6  1996/05/24  20:32:52  ssangiah
 * CSCdi58375:  Need to add ISL support for new platform.
 * Add ISL support for the new c7200 platform.
 * Branch: California_branch
 *
 * Revision 3.7.10.5  1996/05/13  22:53:05  mbeesley
 * CSCdi57397:  DEC21140 interrupt setting incorrect
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/05/07  23:48:16  mbeesley
 * CSCdi56954:  DEC21140 show controller not correct for IO card interface
 * Branch: California_branch
 *
 * Revision 3.7.10.3  1996/04/05  05:32:15  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.7.10.2  1996/03/29  00:45:19  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.7.10.1  1996/03/21  23:23:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.12  1996/03/18  23:28:51  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.11  1996/03/07  00:19:17  mbeesley
 * CSCdi50649:  DEC21140 in Store-and-Forward mode
 *         o Put device in tx cut-thru, 512 byte tx fifo threshold
 *         o Increase rx pool, eliminate use of fallback pool
 *         o Handle rx fifo overrun and error interrupt correctly
 * Branch: ELC_branch
 *
 * Revision 3.2.2.10  1996/03/03  08:40:19  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.2.2.9  1996/02/12  23:32:47  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch. Fix support for
 * receiving single particle packets into memd.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/10  00:30:47  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/08  08:47:56  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/29  19:04:33  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.2.2.5  1996/01/18  05:42:00  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.2.2.4  1996/01/16  01:42:56  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/04  22:18:29  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.2.2.2  1995/11/30  02:05:38  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:41:58  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.7  1996/02/23  20:02:17  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.6  1996/01/21  22:31:43  feisal
 * CSCdi45141:  Need to support 1FE replacement port adapter
 *
 * Revision 3.5  1995/12/19  20:24:29  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.4  1995/12/01  02:00:04  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.3  1995/11/28  02:26:13  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:54:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:02  hampton
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
#include "if_vip_dec21140.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_dec21140.h"
#endif

/*
 * Useful definitions
 */
#define DEC21140_MAX_PACKET		1518	/* Max size packet */
#define DEC21140_MIN_PACKET		64	/* Min size packet */

#define DEC21140_CRC_SIZE		4

#define DEC21140_LINK_UP		1
#define DEC21140_LINK_DOWN		2

#define DEC21140_STOP_DELAY		1    		/* 1 ms */
#define DEC21140_RESET_DELAY		50		/* 50 us */
#define DEC21140_DEVICE_VENDOR_ID	0x00091011

#define DEC21140_MAGIC_PAK_PTR		((paktype *)0x03)

#define DEC21140_STARTEND		1
#define DEC21140_START			2
#define DEC21140_END			3
#define DEC21140_MIDDLE			4

#define DEC21140_REV_MASK		0xF0
#define DEC21140_IS_A_21140		0x10
#define DEC21140_IS_A_21140A		0x20

/*
 * DEC21140 register definitions, PCI registers
 */
typedef struct dec21140_pci_regs_t_ {
    volatile ulong cfid;
    volatile ulong cfcs;
    volatile ulong cfrv;
    volatile ulong cflt;
    volatile ulong cbio;
    volatile ulong cbma;
    volatile ulong reserved[9];
    volatile ulong cfit;
    volatile ulong cfda;
} dec21140_pci_regs_t;

/*
 * DEC21140 register definitions, CSR registers
 */
typedef struct dec21140_regs_t_ {
    volatile ulong csr0;
    volatile ulong reserved0;
    volatile ulong csr1;
    volatile ulong reserved1;
    volatile ulong csr2;
    volatile ulong reserved2;
    volatile ulong csr3;
    volatile ulong reserved3;
    volatile ulong csr4;
    volatile ulong reserved4;
    volatile ulong csr5;
    volatile ulong reserved5;
    volatile ulong csr6;
    volatile ulong reserved6;
    volatile ulong csr7;
    volatile ulong reserved7;
    volatile ulong csr8;
    volatile ulong reserved8;
    volatile ulong csr9;
    volatile ulong reserved9;
    volatile ulong csr10;
    volatile ulong reserved10;
    volatile ulong csr11;
    volatile ulong reserved11;
    volatile ulong csr12;
    volatile ulong reserved12;
    volatile ulong csr13;
    volatile ulong reserved13;
    volatile ulong csr14;
    volatile ulong reserved14;
    volatile ulong csr15;
} dec21140_regs_t;

/*
 * Defines for control and status registers
 *
 * CSR0
 */
#define DEC21140_CSR0_TX_AUTO_POLL_200_US       0x00020000
#define DEC21140_CSR0_CACHE_ALIGN_32_BYTES      0x00004000
#define DEC21140_CSR0_CACHE_ALIGN_64_BYTES      0x00008000 
#define DEC21140_CSR0_CACHE_ALIGN_128_BYTES     0x0000C000 
#define DEC21140_CSR0_PROG_BURST_LEN_32_BYTES   0x00000800
#define DEC21140_CSR0_PROG_BURST_LEN_64_BYTES   0x00001000 
#define DEC21140_CSR0_PROG_BURST_LEN_128_BYTES  0x00002000
#define DEC21140_CSR0_BIG_ENDIAN                0x00000080
#define DEC21140_CSR0_DESCRPT_SKIP_LENGTH       0x00000000
#define DEC21140_CSR0_SOFTWARE_RESET_MASK   	0x00000001

/*
 * CSR1
 */
#define DEC21140_CSR1_TX_POLL_DEMAND		0x000000FF

/*
 * CSR5
 */
#define DEC21140_CSR5_BUS_ERROR_BITS_MASK     	0x03800000
#define DEC21140_CSR5_BUS_ERROR_BITS_SHIFT	23
#define DEC21140_CSR5_TX_PROCESS_STATE_MASK	0x00700000
#define DEC21140_CSR5_TX_PROCESS_STATE_SHIFT	20
#define DEC21140_CSR5_RX_PROCESS_STATE_MASK	0x000E0000
#define DEC21140_CSR5_RX_PROCESS_STATE_SHIFT	17
#define DEC21140_CSR5_NORMAL_INT_SUMMARY        0x00010000
#define DEC21140_CSR5_ABNORM_INT_SUMMARY        0x00008000
#define DEC21140_CSR5_FATAL_BUS_ERROR	   	0x00002000
#define DEC21140_CSR5_GP_TIMER_EXPIRED	   	0x00000800
#define DEC21140_CSR5_RX_WATCHDOG_EXPIRED	0x00000200
#define DEC21140_CSR5_RX_PROCESS_STOPPED        0x00000100
#define DEC21140_CSR5_RX_BUFFER_UNAVAILABLE	0x00000080
#define DEC21140_CSR5_RX_INTERRUPT_PENDING	0x00000040
#define DEC21140_CSR5_RX_FIFO_UNDERFLOW	   	0x00000020
#define DEC21140_CSR5_TX_JABBER_TIMEOUT	   	0x00000008
#define DEC21140_CSR5_TX_BUFFER_UNAVAILABLE	0x00000004
#define DEC21140_CSR5_TX_PROCESS_STOPPED        0x00000002
#define DEC21140_CSR5_TX_INTERRUPT_PENDING	0x00000001
#define DEC21140_CSR5_CLEAR_ALL_INTERRUPTS      0x0001FFFF

/*
 * CSR6
 */
#define DEC21140_CSR6_MUST_BE_ONE	        0x02000000
#define DEC21140_CSR6_MEDIA_PORT_SCRAMBLE       0x01000000
#define DEC21140_CSR6_MEDIA_PORT_MII_SYM_PCS    0x00800000
#define DEC21140_CSR6_STORE_AND_FORWARD         0x00200000
#define DEC21140_CSR6_HEARTBEAT_DISABLE         0x00080000
#define DEC21140_CSR6_PORT_SELECT_MII_SYM	0x00040000
#define DEC21140_CSR6_TX_TR_1024		0x0000C000
#define DEC21140_CSR6_TX_TR_512			0x00008000
#define DEC21140_CSR6_TX_TR_256			0x00004000
#define DEC21140_CSR6_TX_TR_128			0x00000000
#define DEC21140_CSR6_TRANSMIT_START            0x00002000
#define DEC21140_CSR6_TRANSMIT_STOP             0x00000000

#define DEC21140_CSR6_OPER_MODE_MASK            0x00000C00
#define DEC21140_CSR6_OPER_MODE_NORMAL          0x00000000
#define DEC21140_CSR6_OPER_MODE_INT_LOOPBACK    0x00000400
#define DEC21140_CSR6_OPER_MODE_EXT_LOOPBACK    0x00000800

#define DEC21140_CSR6_FULL_DUPLEX		0x00000200
#define DEC21140_CSR6_PASS_ALL_MULTICAST        0x00000080
#define DEC21140_CSR6_PROMISCUOUS_MODE          0x00000040
#define DEC21140_CSR6_INVERSE_FILTERING         0x00000010
#define DEC21140_CSR6_RECEIVE_START             0x00000002
#define DEC21140_CSR6_RECEIVE_STOP              0x00000000

/*
 * CSR7
 */
#define DEC21140_CSR7_ENB_NORM_INT_SUMMARY      0x00010000
#define DEC21140_CSR7_ENB_ABNORM_INT_SUMMARY    0x00008000
#define DEC21140_CSR7_ENB_FATAL_BUS_ERROR_INT   0x00002000
#define DEC21140_CSR7_ENB_GENL_PURP_TIMER_INT   0x00000800
#define DEC21140_CSR7_ENB_RX_WATCHDOG_TO_INT    0x00000200
#define DEC21140_CSR7_ENB_RX_STOPPED_INT        0x00000100
#define DEC21140_CSR7_ENB_RX_BUF_UNAVAIL_INT    0x00000080
#define DEC21140_CSR7_ENB_RX_INT                0x00000040
#define DEC21140_CSR7_ENB_TX_UNDERFLOW_INT      0x00000020
#define DEC21140_CSR7_ENB_TX_JABBER_TO_INT      0x00000008
#define DEC21140_CSR7_ENB_TX_BUF_UNAVAIL_INT    0x00000004
#define DEC21140_CSR7_ENB_TX_STOPPED_INT        0x00000002
#define DEC21140_CSR7_ENB_TX_INT                0x00000001

#define DEC21140_CSR7_DIS_GENL_PURP_TIMER_INT   0x00000000
#define DEC21140_CSR7_DIS_TX_BUF_UNAVAIL_INT    0x00000000

#define DEC21140_CSR7_DISABLE_ALL_INTERRUPTS	0x00000000

/*
 * CSR8
 */
#define DEC21140_CSR8_MISSED_FRAME_MASK         0x0001FFFF
#define DEC21140_CSR8_MISSED_FRAME_OVERFLOW     0x00010000
#define DEC21140_CSR8_OVERFLOW			0x10000000
#define DEC21140_CSR8_OVERFLOW_COUNT_MASK	0x0FFE0000

/*
 * CSR9
 */
#define	DEC21140_CSR9_MII_READ		   	0x00040000
#define	DEC21140_CSR9_MII_WRITE_1		0x00020000
#define	DEC21140_CSR9_MII_WRITE_0		0x00000000
#define	DEC21140_CSR9_MDC_CLOCK		   	0x00010000
#define	DEC21140_CSR9_NOP			0x00000000
#define	DEC21140_CSR9_READ			0x00004000
#define DEC21140_CSR9_WRITE			0x00002000
#define DEC21140_CSR9_SELECT_MII_PHY		0x00000000
#define	DEC21140_CSR9_SELECT_SERIAL_ROM	   	0x00000800
#define	DEC21140_CSR9_RX_BIT_SHIFT		19
#define	DEC21140_CSR9_TX_BIT_SHIFT		17

/*
 * CSR12
 */
#define DEC21140_CSR12_GPC			0x00000100

#define DEC21140_CSR12_LINK_SIGNAL_DETECT_IN	0x00000040
#define DEC21140_CSR12_MICRO_TX_ENABLE	   	0x00000020
#define DEC21140_CSR12_MICRO_TX_DISABLE	   	0x00000000
#define DEC21140_CSR12_PA_OSC_TX_CLK_SELECT	0x00000010
#define DEC21140_CSR12_PCI_TX_CLK_SEL_DEBUG	0x00000000
#define DEC21140_CSR12_LED_ENABLE		0x00000008
#define DEC21140_CSR12_LED_DISABLE		0x00000000

#define DEC21140_CSR12_MII                      0x00000004
#define DEC21140_CSR12_RJ45                     0x00000000
#define DEC21140_CSR12_MICRO_6671_NORMAL        0x00000002
#define DEC21140_CSR12_MICRO_6671_LOOPBACK      0x00000000
#define DEC21140_CSR12_MOT_68836_NORMAL         0x00000001
#define DEC21140_CSR12_MOT_68836_LOOPBACK       0x00000000
 
#define DEC21140_CSR12_OUTPUT_MASK		0x0000003F

#define DEC21140_CSR12_INITIAL_VALUE  (DEC21140_CSR12_RJ45 | \
				       DEC21140_CSR12_MICRO_TX_ENABLE | \
				       DEC21140_CSR12_PA_OSC_TX_CLK_SELECT | \
				       DEC21140_CSR12_LED_ENABLE | \
				       DEC21140_CSR12_MICRO_6671_NORMAL | \
				       DEC21140_CSR12_MOT_68836_NORMAL)

#define DEC21140_CSR12_MICE_B2B_PACKET_BIT      0x00000080
#define DEC21140_CSR12_MICE_FALSE_CRS_BIT       0x00000040
#define DEC21140_CSR12_MICE_RD_WR_PCI           0x00000020
#define DEC21140_CSR12_MICE_FALSE_CS_ENABLE     0x00000010
#define DEC21140_CSR12_MICE_LED_ENABLE          0x00000008
#define DEC21140_CSR12_MICE_MII                 0x00000004
#define DEC21140_CSR12_MICE_COUNTER_STROBE      0x00000002
#define DEC21140_CSR12_MICE_ISL_ENABLE          0x00000001
#define DEC21140_CSR12_MICE_OUTPUT_MASK         0x000000FF

#define DEC21140_CSR12_MICE_INITIAL_VALUE  (DEC21140_CSR12_MICE_MII | \
					    DEC21140_CSR12_MICE_LED_ENABLE | \
					    DEC21140_CSR12_MICE_ISL_ENABLE)

#define DEC21140_CSR12_MICE_IOFE_ISL_ENABLE	0x00000080
#define DEC21140_CSR12_MICE_IOFE_DET_LEDS	0x00000040
#define DEC21140_CSR12_MICE_IOFE_ISL_RESET	0x00000010

#define DEC21140_CSR12_MICE_IOFE_INITIAL_VALUE ( \
					DEC21140_CSR12_MICE_LED_ENABLE | \
					DEC21140_CSR12_MICE_IOFE_ISL_ENABLE)
/*
 * Define for the delay between the enabling of the ISL_ENABLE bit and 
 * bringing the MICE on the IO-card out of reset.
 */
#define DEC21140_CSR12_MICE_IOFE_DELAY		100 /* microseconds */

/*
 * Defines for MICE hardware on the 1FE_TX_ISL & 1FE_FX_ISL 
 */
#define MICE_ISL_CRC_MASK			0x1000
#define MICE_ISL_CRC_MASK_OFFSET		4

/*
 * Defines for DP83840 MII configuration registers
 */
#define	DP83840_CSR00_LOOPBACK		   	0x4000
#define	DP83840_CSR00_NO_LOOPBACK		0x0000
#define	DP83840_CSR00_SPEED_100MBIT		0x2000
#define	DP83840_CSR00_ISOLATE		   	0x0400
#define	DP83840_CSR00_NO_ISOLATE		0x0000
#define	DP83840_CSR00_FULL_DUPLEX		0x0100

/*
 * Defines for DP83840 MII status register
 */
#define DP83840_CSR01_LINK_STATUS		0x0004

/*
 * Defines for PCI configuration registers
 */
#define DEC21140_CFCS_MASTER_OPERATION    	0x00000004
#define DEC21140_CFCS_IO_SPACE_ACCESS		0x00000001
#define DEC21140_CFCS_MEM_SPACE_ACCESS		0x00000002
#define DEC21140_CFLT_LATENCY_TIMER		0x0000FF00

/* 
 * Offsets as seen from the primary PCI bus 
 */
#define    DEC21140_CBIO_PCI0_IO_SPACE	      	0x0000
#define    DEC21140_CBIO_PCI1_IO_SPACE	      	0x8000

#ifdef NOTYET

#define PA0_IO 0xa9000000
#define PA1_IO 0xa9008000

#endif

/*
 * Defines for PCI configuration register address offsets
 */
#define DEC21140_PCI_CFID_REG_OFFSET	    	0x00
#define DEC21140_PCI_CFCS_REG_OFFSET		0x04
#define DEC21140_PCI_CFRV_REG_OFFSET		0x08
#define DEC21140_PCI_CFLT_REG_OFFSET		0x0C
#define DEC21140_PCI_CBIO_REG_OFFSET		0x10
#define DEC21140_PCI_CBMA_REG_OFFSET		0x14
#define DEC21140_PCI_CFIT_REG_OFFSET		0x3C
#define DEC21140_PCI_CFDA_REG_OFFSET	       	0x40

/*
 * Defines for setup frame descriptor
 *
 * See p. 3-33, 4-17 - 4-18.  Or in filter modes to tdes1 word.
 */
#define DEC21140_SETUP_DESCRPT_STATUS	   	0x80000000
#define DEC21140_SETUP_DESCRPT_CNTL_LEN	   	0x09000000 | 192

/*
 * Init block
 */
#define    DEC21140_INITBLOCK_BYTES	      	192

typedef struct dec21140_initblock_t_ {

    volatile ulong init_buf[DEC21140_INITBLOCK_BYTES / 4];

} dec21140_initblock_t;

/*
 * Receive descriptor and defintions
 */
typedef struct dec21140_rx_desc_t_ dec21140_rx_desc_t;

struct dec21140_rx_desc_t_ {
    volatile ulong   rx_status;
    volatile ulong   control_length;
    volatile ulong   buf_addr;
    dec21140_rx_desc_t * volatile next_descrpt_ptr;
};

#define DEC21140_RMD_OWN	      		0x80000000
#define DEC21140_RMD_RX_COUNT_MASK              0x7FFF0000
#define DEC21140_RMD_MULTICAST_FRAME		0x0400
#define DEC21140_RMD_STP		   	0x0200
#define DEC21140_RMD_ENP		   	0x0100

#define DEC21140_RMD_FRAME_TYPE_MASK	       	0x0020
#define DEC21140_RMD_FRAME_TYPE_ETHERNET      	0x0020
#define DEC21140_RMD_FRAME_TYPE_802_3	   	0x0000

#define DEC21140_RMD_ERR_SUMMARY	      	0x8000
#define DEC21140_RMD_DESCRIPTOR_ERROR	   	0x4000
#define DEC21140_RMD_DATA_TYPE_MASK	       	0x3000
#define DEC21140_RMD_RUNT_FRAME_ERROR	   	0x0800
#define DEC21140_RMD_FRAME_TOO_LONG_ERROR      	0x0080
#define DEC21140_RMD_LATE_COLLISION_SEEN_ERROR  0x0040
#define DEC21140_RMD_RECEIVE_WATCHDOG_ERROR    	0x0010
#define DEC21140_RMD_DRIBBLE_BITS_ERROR	   	0x0004
#define DEC21140_RMD_CRC_ERROR		   	0x0002
#define DEC21140_RMD_FIFO_OVERFLOW_ERROR      	0x0001

#define DEC21140_RMD_ERROR_MASK   			\
    (DEC21140_RMD_DESCRIPTOR_ERROR |			\
     DEC21140_RMD_RUNT_FRAME_ERROR |			\
     DEC21140_RMD_FRAME_TOO_LONG_ERROR |		\
     DEC21140_RMD_LATE_COLLISION_SEEN_ERROR |		\
     DEC21140_RMD_RECEIVE_WATCHDOG_ERROR |		\
     DEC21140_RMD_DRIBBLE_BITS_ERROR |	       		\
     DEC21140_RMD_CRC_ERROR |				\
     DEC21140_RMD_FIFO_OVERFLOW_ERROR)

#define DEC21140_RMD_RX_END_OF_RING         	0x02000000
#define DEC21140_RMD_SECOND_ADDRESS_CHAINED	0x01000000

/*
 * Transmit descriptor and definitions
 */
typedef struct dec21140_tx_desc_t_ dec21140_tx_desc_t;

struct dec21140_tx_desc_t_ {
    volatile ulong   tx_status;
    volatile ulong   control_length;
    volatile ulong   buf_addr;
    dec21140_tx_desc_t * volatile next_descrpt_ptr;
};

/*
 * Defines for transmit descriptor errors in TDES0 (status word)
 */
#define DEC21140_TMD_OWN			0x80000000

#define DEC21140_TMD_ERR_SUMMARY                0x8000
#define DEC21140_TMD_JABBER_TO                  0x4000
#define DEC21140_TMD_CARRIER_LOSS               0x0800
#define DEC21140_TMD_NO_CARRIER                 0x0400
#define DEC21140_TMD_LATE_COLL                  0x0200
#define DEC21140_TMD_EXCESS_COLL                0x0100
#define DEC21140_TMD_HEART_FAIL                 0x0080
#define DEC21140_TMD_COLL_COUNT		   	0x0078
#define DEC21140_TMD_COLL_SHIFT		   	3
#define DEC21140_TMD_LINK_FAIL                  0x0004
#define DEC21140_TMD_UFLO_ERR                   0x0002
#define DEC21140_TMD_DEFERRED                   0x0001

/*
 * Defines for transmit descriptor control bits in TDES1 (control, length)
 */
#define DEC21140_TMD_COMPLETION_INT	       	0x80000000
#define DEC21140_TMD_ENP		       	0x40000000
#define DEC21140_TMD_STP		       	0x20000000
#define DEC21140_TMD_FT_PERFECT                 0x00000000
#define DEC21140_TMD_FT_HASH                    0x00400000
#define DEC21140_TMD_FT_INVERSE                 0x10000000
#define DEC21140_TMD_FT_HASHONLY                0x10400000

#define DEC21140_TMD_SETUP_PACKET	      	0x08000000
#define DEC21140_TMD_ADD_CRC_DISABLE	       	0x04000000
#define DEC21140_TMD_TX_END_OF_RING	       	0x02000000
#define DEC21140_TMD_SECOND_ADDRESS_CHAINED   	0x01000000

#define DEC21140_TMD_DISABLE_PADDING	      	0x00800000
#define DEC21140_TMD_FILTER_BIT_ZERO	      	0x00400000


/*
 * Defines for hardware perfect address filter.
 */
#define DEC21140_PERF_HWAF_BROAD_ENTRY		14
#define DEC21140_PERF_HWAF_UNI_ENTRY		15
#define DEC21140_PERF_HWAF_NUM_ENTRIES		16

#define station_uni_addr	addr[DEC21140_PERF_HWAF_UNI_ENTRY]

#define DEC21140_PERF_HWAF_ENTRY_IS_FREE(p)			\
    (((p).w[0].word == 0xFFFF) &&				\
     ((p).w[1].word == 0xFFFF) &&				\
     ((p).w[2].word == 0xFFFF))
#define DEC21140_PERF_HWAF_ENTRY_IS_EQUAL(mac,p)		\
    (((p).w[0].word == *((ushort *)mac)) &&			\
     ((p).w[1].word == *((ushort *)mac + 1)) &&			\
     ((p).w[2].word == *((ushort *)mac + 2)))

/*
 * Hardware address filtering setup frame buffer formats
 */
typedef struct dec21140_addr_entry_t_ {
    struct {
      ushort word;
      ushort pad;
    } w[3];
} dec21140_addr_entry_t;

typedef struct dec21140_perf_hwaf_t_ {
    dec21140_addr_entry_t addr[DEC21140_PERF_HWAF_NUM_ENTRIES];
} dec21140_perf_hwaf_t;

typedef dec21140_perf_hwaf_t dec21140_hwaf_t;

typedef struct dec21140_imperf_hwaf_t_ {
    struct {
      ushort hash;
      ushort pad;
    } multicast[32];
    ulong pad[7];
    dec21140_addr_entry_t unicast;
} dec21140_imperf_hwaf_t;

/*
 * Tx ring shadow data structure
 */
typedef struct dec21140_tx_shadow_t_ {
    paktype *pak;		/* pointer to pak */
    particletype *mp;		/* pointer to particle */
} dec21140_tx_shadow_t;

/*
 * Instance data structure for 1fe DEC21140 interfaces
 */

typedef struct dec21140_instance_t_ {

    pas_cbtype pacb;			/* Universal PA device control */

    /*
     * Control/Status registers and PCI registers
     */
    dec21140_regs_t *dec21140_regaddr;

    /*
     * Receive ring information
     */
    dec21140_rx_desc_t *rxr;  		/* RX ring pointer */
    dec21140_rx_desc_t *rxr_malloc;    	/* RX ring pointer, usable for free */
    uint rx_head, rx_tail;		/* Indexes into rx ring */
    particletype **rxr_shadow;		/* Rx ring shadow */
    paktype *rx_pak;			/* Rx temp paktype pointer */
    dec21140_hwaf_t *hwaf_buf_ptr;      /* Hardware addr filter frame ptr */

    /*
     * Transmit ring
     */
    dec21140_tx_desc_t *txr;  		/* TX ring pointer */
    dec21140_tx_desc_t *txr_malloc;  	/* Tx ring pointer, usable for free */
    uint tx_head, tx_tail;		/* Indexes into tx ring */
    uint tx_count;			/* Count of Tx ring entries used */
    boolean tx_limited;			/* Limit one tx_pak at a time */
    dec21140_tx_shadow_t *txr_shadow;	/* Tx ring shadow */

    /*
     * Init block information
     */
    dec21140_initblock_t *ib;    	/* Init block pointer */
    dec21140_initblock_t *ib_malloc;	/* Init block pointer, freeable */

    /*
     * Count errors
     */
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
    ulong rx_fifo_overflow;     	/* # of frame truncations due to
					   FIFO overflow */
    ulong rx_no_enp;		    	/* # packets without ENP set */
    ulong rx_discard;			/* flag for dropping rest of packet */
    ulong rx_late_collision;		/* # of rx late collisions */

    /*
     * Transmit errors
     */
    ulong tx_underrun_err;         	/* # of TX aborts, FIFO underrun */
    ulong tx_jabber_timeout;        	/* # of times TX caught jabbering */
    ulong tx_carrier_loss;          	/* # of frames affected by loss of
					   carrier during transmit */
    ulong tx_no_carrier;            	/* # of frames affected by no carrier
					   signal from Xceiver during TX */
    ulong tx_late_collision;        	/* # of frames with late collisions
					   during transmit */
    ulong tx_excess_coll;	  	/* # of frame transmissions aborted 
					   due to excessive collisions */
    ulong tx_collision_count;       	/* collision count for one packet */
    ulong tx_deferred;              	/* # of frames where TX deferred */
       
    tinybool fatal_tx_err;       	/* A fatal tx error occured. 
					   Reset dec21140 */
    tinybool addr_table_overflow;       /* HW AF addr overflow */

    tinybool rx_all_multicast;          /* true if we're in rec all multicast
                                           packet mode */

    ushort pa_type;			/* to distinguish whether it is 1FE,
                                           1FE-TX, 1FE-TX-NISL or IFE-FX */

    ushort pci_device_number;		/* Real PCI bus device number */

    tinybool phy_link_active;		/* the link status on the PHY */

    tinybool is_DEC21140A;		/* true if it is a "A" */
} dec21140_instance_t;
 
/*
 * External globals
 */
extern boolean ether_debug;  		/* debug flag for ethernet */

/*****************************************************************
 * Macros
 *****************************************************************/

/*
 * DEC21140_ADVANCE_TX_RING_INDEX :
 * Advance a ring index for DEC21140 Tx ring
 */
#define DEC21140_ADVANCE_TX_RING_INDEX(index)			\
    index = ((index + 1) & DEC21140_TX_RING_INDEX_MASK);

/*
 * DEC21140_RETRACT_TX_RING_INDEX :
 * Move an DEC21140 Tx ring index backwards
 */
#define DEC21140_RETRACT_TX_RING_INDEX(index)			\
    index = ((index - 1) & DEC21140_TX_RING_INDEX_MASK);

/*
 * DEC21140_ADVANCE_RX_RING_INDEX :
 * Advance a ring index for DEC21140 Rx ring
 */
#define DEC21140_ADVANCE_RX_RING_INDEX(index)			\
    index = ((index + 1) & DEC21140_RX_RING_INDEX_MASK);

/*
 * DEC21140_RETRACT_RX_RING_INDEX :
 * Move an DEC21140 Rx ring index backwards
 */
#define DEC21140_RETRACT_RX_RING_INDEX(index)			\
    index = ((index - 1) & DEC21140_RX_RING_INDEX_MASK);

/*
 * DEC21140_RX_CONT_LENGTH :
 * Create the rx descriptor control/length field
 */ 
#define DEC21140_RX_CONT_LENGTH(length)				\
    (DEC21140_RMD_SECOND_ADDRESS_CHAINED | length)

/*****************************************************************
 * Static inlines
 *****************************************************************/

/*
 * dec21140_rxring_flush :
 * Flush the Rx ring from the data cache
 */
static inline void dec21140_rxring_flush (dec21140_instance_t *ds)
{
    if (DEC21140_RX_RING_ATTRIBUTE == TLB_UNCACHED) {
	return;
    }
}

/*
 * dec21140_flush_pak:
 * NULL out the relevant entries in the paktype
 */
static inline void dec21140_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
}

/*
 * dec21140_cleanup_pak:
 * Return any particles and then NULL out relevant entries in the paktype
 */
static inline void dec21140_cleanup_pak (paktype *pak)
{
    particletype *mp;

    while (mp = particle_dequeue_inline(pak), mp) {
	retparticle(mp);
    }
    pak->datagramstart = NULL;
    pak->datagramsize = 0;
}

/*
 * dec21140_tx_cl:
 * Set the Tx descriptor control and length field. Note, since all callers
 * pass in a constant for the control field, the compiler optimizes
 * out the if, then else chain.
 */
static inline ulong dec21140_tx_cl (ushort length, ulong control)
{
    if (control == DEC21140_STARTEND) {
	return((DEC21140_TMD_SECOND_ADDRESS_CHAINED |
		DEC21140_TMD_COMPLETION_INT |
		DEC21140_TMD_STP |
		DEC21140_TMD_ENP) | length);

    } else if (control == DEC21140_START) {
	return((DEC21140_TMD_SECOND_ADDRESS_CHAINED |
		DEC21140_TMD_STP) | length);

    } else if (control == DEC21140_END) {
	return((DEC21140_TMD_SECOND_ADDRESS_CHAINED |
		DEC21140_TMD_COMPLETION_INT |
		DEC21140_TMD_ENP) | length);

    } else {
	/*
	 * Middle by default
	 */
	return((DEC21140_TMD_SECOND_ADDRESS_CHAINED) | length);
    }
}

/*
 * Function prototypes
 */

/* if_pas_dec21140.c */

extern boolean		dec21140_analyze_device(uint, uint);
extern void 		dec21140_set_rx_discard(dec21140_instance_t *, 
						boolean);
extern void		dec21140_throttle(hwidbtype *);
extern void             dec21140_cache_threshold (pooltype *pool);
extern void 		dec21140_rx_error(ulong, hwidbtype *, particletype *);
extern void		dec21140_link_down (hwidbtype *);
extern void 		dec21140_tx_error(ulong, hwidbtype *);
extern void 		dec21140_error_interrupt(ulong, hwidbtype *);
extern boolean		dec21140_init(hwidbtype *, boolean);
extern void		dec21140_set_hw_address(hwidbtype *idb, uchar *mac);
extern void		dec21140_add_hw_address(idbtype *swidb,
                                                       uchar *mac, uchar fsm);
extern void             dec21140_add_multicast_range(idbtype *swidb,
                                                     uchar *mac, uchar *mask);
extern void		dec21140_hwaf_clear(hwidbtype *idb);

/* if_ [platform] _dec21140.c */

extern hwidbtype	*dec21140_idb_create(uint, uint);
extern void		dec21140_deallocate(hwidbtype *);
extern boolean		dec21140_platform_init(hwidbtype *);
extern void		dec21140_show_instance(dec21140_instance_t *);
extern void		dec21140_snmp_link_trap(int, hwidbtype *);
extern void 		dec21140_lineproto_change(hwidbtype *idb, boolean);
extern void		dec21140_get_default_hw_address(hwidbtype *, uchar *);
extern boolean		dec21140_init_buffer_pools(hwidbtype *);
extern void		dec21140_tx_start(hwidbtype *);
extern void		dec21140_fastsend(hwidbtype *, paktype *);
extern boolean		dec21140_default_duplex(hwidbtype *);
extern int		dec21140_default_media(hwidbtype *);
extern particletype 	*dec21140_get_particle(hwidbtype *);

/* asm wrapper file */

extern pa_wrapper_t	dec21140_eim_wrapper;

/*
 * Include platform dependant header file again to pick up static 
 * inline definitions. Note, this is the second inclusion of these files.
 */

#define DEC21140_STATIC_INLINES_ONLY

#if defined(RVIP) || defined(SVIP)
#include "if_vip_dec21140.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_dec21140.h"
#endif

#undef DEC21140_STATIC_INLINES_ONLY

/* end of file */

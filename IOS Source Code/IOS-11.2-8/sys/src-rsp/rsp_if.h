/* $Id: rsp_if.h,v 3.12.4.8 1996/09/07 02:18:10 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_if.h,v $
 *------------------------------------------------------------------
 * rsp_if.h  --  Include file for Network interface device driver
 *               level and decision layer for the IRSP/1
 *
 * Chris Shaker, March, 1994
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_if.h,v $
 * Revision 3.12.4.8  1996/09/07  02:18:10  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.12.4.7  1996/08/29  22:50:42  ganesh
 * CSCdi62683:  HSA requires that both RSP2s have same amount of DRAM
 * Branch: California_branch
 * Defined HSA_MIN_MEMSIZE to 24mb, the minimum memory required for HSA.
 *
 * Revision 3.12.4.6  1996/08/19  19:56:00  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.12.4.5  1996/08/15  15:32:07  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.12.4.4  1996/08/07  18:02:19  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.12.4.3  1996/07/25  00:29:13  ganesh
 * CSCdi63845:  Slave OIR reload fix in CSCdi57076 fails on rsp1, rsp7000.
 * Branch: California_branch
 * Added RSP_RSP4 for enum rsp_flavor_t, and extern declaration
 * for rsp_dual_capable().
 *
 * Revision 3.12.4.2  1996/07/08  22:52:46  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.12.4.1  1996/03/21  23:50:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:35:06  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  09:29:56  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/23  04:41:54  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.12  1996/03/11  11:09:50  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.11  1996/03/07  11:14:17  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * TRIP rxoffset needs to be on a long word boundary.
 *
 * Revision 3.10  1996/03/04  15:29:17  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Add minimal support for optimum/flow switching of tokenring (without
 * rif) and atm (aal5-mux and llc-snap only).
 *
 * Revision 3.9  1996/02/13  20:25:32  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.8  1996/02/13  05:52:09  ogrady
 * CSCdi45887:  Router crash in rsp_fastsend
 *              Allocate memd buffers with extra padding for
 *              memdcopy() overrun.
 *
 * Revision 3.7  1996/02/08  22:22:37  wmay
 * CSCdi48458:  mip uses old scheduler - use watched queues for love
 * note handler
 *
 * Revision 3.6  1996/02/04  12:17:44  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Use portable array references instead of post incremented pointer
 * dereferences when passing arguments to errmsg(), because the order
 * of function argument evaluation varies.
 *
 * Revision 3.5  1996/02/04  02:29:39  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.4  1996/01/23  08:52:49  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.3  1996/01/15  18:05:11  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.2  1995/11/17  18:48:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:28:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/11  07:10:45  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.4  1995/09/17  20:55:31  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.3  1995/07/19  06:47:50  shaker
 * CSCdi36264:  fast and optimum switching dont prime returnq
 * Bring 11.0 code up to date with solution in 10.3 code.
 *
 * Revision 2.2  1995/07/17  08:22:04  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  22:58:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __RSP_IF_H__
#define __RSP_IF_H__

#include "../if/ether.h"
#include "../src-4k/signal_4k.h"
#include <ciscolib.h>

extern qa_hwq_t		*RawQ;
extern qa_hwq_t		*ReturnQ;
extern qa_hwq_t		*EventQ;
extern qa_hwq_t		*BufhdrQ;
extern qa_hwq_t		*IpcbufQ;
extern qa_hwq_t		*IpcbufQ_classic;


/*
 * Make this number big enough to receive love letters from all
 * IPs.  There's not much point making them larger than the
 * largest below; no need for 'room for expansion'.  If someone's
 * changing the size of their love letters than they need to 
 * build a new system image anyway, so they can fix this at the
 * same time.  If they fail to, they'll get a warning from 
 * rsp_common_ctrlr_init() reminding them.
 *
 * At the time of writing, the relevant IPs and their love
 * letter sizes are:
 *
 * MIP 1374
 * AIP 1024
 * CIP 1624
 */
#define RSP_MAX_LOVE_LETTER  1624

/*
 * The buffer size for IPC messages.
 */
#define RSP_MAX_IPC_BUFF_SIZE   4096
#define BUFF_PER_IPC_CARD       8

/*
 * extra padding required at the start of MEMD buffers
 * - HIP needs two bytes for "tx delay" data, or three bytes if the
 *   packet data starts with an odd byte address
 * When carving buffers we make sure encapbytes is rounded up to be even,
 * so this could be set to 2.
 *
 * However, we can copy to MEMD 50% faster using the MIPS sd (store double)
 * command than when using sdl/sdr;  in order to take advantage of this
 * memd_copy speedup, allow enough room at start of memd buffer to make it
 * double-word aligned.
 * (This assumes buffers start on double boundary)
 */
#define RSP_BUFSTART_PAD	8

/*
 * Ensure 8-byte alignment on the sum of RSP_BUFSTART_PAD and ENCAPBYTES.
 */
#define RSP_FLOOD_PAD	((RSP_BUFSTART_PAD + ENCAPBYTES + 7) & ~7)

/*
 * If the a transmit packet in MEMD is crosses fewer than four 32-bit word
 * boundaries, then the FIP DMA controller may get stuck.  The workaround
 * is to start the packet on an odd 16-bit boundary for 16-byte packets.
 *
 * Fip tx byte offset (for <= 16 byte packets)
 *  must be odd short word boundary
 *  should be <= RSP_BUFSTART_PAD for proper memd buffer size allocation
 *   round RSP_BUFSTART_PAD down to next lower odd short word 
 */
#define RSP_FIP_BUFSTART_PAD      (((RSP_BUFSTART_PAD+2)&~3)-2)

#define RSP_FIP_DMA_UNDERRUN_SIZE 16

/*
 * count of lost buffers after which we'll reset the cbus
 */
#define MAX_LOST_MEMD_BUFFERS	40
extern uint lost_memd_buffers;

/*
 * Used to convert a local queue pointer into a hardware IDB pointer
 * base-2 log of sizeof(qa_hwq_acc_t).
 *
 * Remember to change rsp_register_hwidb_for_local_free_queue() whenever
 * you change this macro.
 * The two routines must agree on which hardware IDB is associated with
 * which local free queue!
 */
#define RSP_MAX_LOCAL_FREE_QUEUES	256

#define LOCAL_QUEUE_SHIFT	3	 /* log2(sizeof(hwqueue)) */
#define LOCAL_QUEUE_MASK	((RSP_MAX_LOCAL_FREE_QUEUES - 1) \
				 << LOCAL_QUEUE_SHIFT)

#define LOCAL_QUEUE_TO_HW_IDB(local_queue_offset) \
	 local_queue_to_hw_idb[((local_queue_offset & LOCAL_QUEUE_MASK) >> \
				LOCAL_QUEUE_SHIFT)]

/*
 * The following RX offset values are optimized for the default
 * encapsulation on each media, for routing, for R4600 cacheline
 * considerations, and for IP ucode/hardware restrictions.
 *
 * Some IPs require the offset to be an even number of shorts, or an
 * odd number of shorts. Talk to the microcoder for your IP to find
 * out for sure.
 *
 * We want the type field and the network layer in the same 32 byte
 * cache line. We want the etype field as close as possible to the
 * beginning of that cache line, as long as the network layer starts
 * on a long word boundary. The IP header checksum routine does long word
 * memory references starting at the network layer, so the network
 * layer has to be long word aligned.
 *
 * These defines should not be used by code throughout the system.
 * All code should be referencing hwidb->rxoffset instead.
 *
 * These defines should be moved into media specific high
 * end include files. For example, RSP_ETHER_BYTE_RECEIVE_OFFSET
 * should really live in 'hes/if_hes_ether.h'.
 */


/*
 * This is how fddi SNAP/IP frames should look like when rsp_ipflow_fs()
 * gets called. Note that the IP header is aligned on a cache-line:
 *
 *      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 *0x40|               |               |               |               |
 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *0x50|           | FC DA0 DA1 DA2 DA3 DA4 DA5|SA0 SA1 SA2 SA3 SA4 SA5|
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 *0x60| AA AA  03  00  00  00 |08  00 |45 tos  ip len                 |
 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *0x70|ttl prt   cks  | IP SRC        |IP DST         | S-port  D-port|
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 */
#define RSP_FDDI_BYTE_RECEIVE_OFFSET	0x53
/*
 * Tokenring needs rxoffset aligned to 4-byte boundary.
 */
#define RSP_TOKEN_BYTE_RECEIVE_OFFSET	0x54

/*
 * This is how Ethernet IP frames should look like when rsp_ipflow_fs()
 * gets called. Note that the IP header is aligned on a cache-line:
 *
 *      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 *0x40|               |               |               |               |
 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *0x50|                                       |DA0 DA1 DA2 DA3 DA4 DA5|     
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 *0x60|SA0 SA1 SA2 SA3 SA4 SA5|08  00 |45 tos  ip len                 |
 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *0x70|ttl prt   cks  | IP SRC        |IP DST         | S-port  D-port|
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 */
#define RSP_ETHER_BYTE_RECEIVE_OFFSET	0x5A

/*
 * This is how HDLC IP frames should look like when rsp_ipflow_fs()
 * gets called. Note that the encaps is included in same cache line
 * as the IP header (since it's small enough), and the IP header
 * is aligned on an 8-byte boundary to simplify MAC rewrite.
 *
 *      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 *0x40|               |               |               |               |
 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *0x50|        
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 *0x60|               | 0F 00  08  00 |45 tos  ip len |               |
 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *0x70|ttl     cks    | IP SRC        |IP DST         | S-port  D-port|
 *    +===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+===+
 */
#define RSP_HDLC_BYTE_RECEIVE_OFFSET	0x64
#define RSP_SERIAL_PROTOCOL_OFFSET	(RSP_HDLC_BYTE_RECEIVE_OFFSET + 4)

#define RSP_AIP_BYTE_RECEIVE_OFFSET	0x60


/*
 * The maximum RECEIVE offset needs to be considered
 * when determining the amount buffer space in the beginning of 
 * a memd buffer. 
 */

#define RSP_MAX_BYTE_RECEIVE_OFFSET   RSP_SERIAL_PROTOCOL_OFFSET


/* Convert a MEMD Offset into a 32 bit address in writeback cached MEMD
   space for the RSP processor: */
#define MEMD_OFFSET_TO_ADDR(offset) \
    ( (void*) ( (ulong) memd_base | (ulong) (offset) ) )


/* Convert a QASIC Offset into a 32 bit adddress in uncached QASIC
   space for the RSP processor: */
#define QASIC_OFFSET_TO_ADDR(offset) \
    ( (void*) ( (ulong) qasic_base | (ulong) (offset) ) )


/* Convert a bufhdr offset in MEMD into a 32 bit adddress into write-through
   MEMD space for the RSP processor.  bufhdr segment is contiguous
   with qasic. */
#define BUFHDR_OFFSET_TO_ADDR(offset) \
    ( (void*) ( (ulong) bufhdr_base + ((ulong)(offset)-MEMD_BUFHDR_OFFSET) ) )


/* Convert a 32 bit RSP address to a 16 bit MEMD, buffer header,
   or QASIC offset */
#define ADDR_TO_MEMD_OFFSET_16(addr) \
	( (ulong) (addr) & (ulong) 0xffff)


/* Convert a 32 bit RSP address to a 24 bit MEMD, buffer header,
   or QASIC offset stored in 32 bits */
#define ADDR_TO_MEMD_OFFSET_24(addr) \
	( (ulong) (addr)  & (ulong) 0xffffff)

/*
 * Convert a 32 bit RSP MEMD address to a 32 bit RSP uncached MEMD address.
 */
#define ADDR_MEMD_TO_UNCACHED(addr)   \
	((void*) (ADRSPC_MEMD_UNCACHED + ADDR_TO_MEMD_OFFSET_24(addr)))

/*
 * Convert a MEMD offset to a 32 bit RSP uncached MEMD address.
 */
#define MEMD_OFFSET_TO_UNCACHED_ADDR(offset)   \
	((void*) (ADRSPC_MEMD_UNCACHED + (offset)))

typedef enum {
    SLAVE_UNPLUGGED,          /* Missing, RSP1, sole RSP2, or slave RSP2 */
    SLAVE_CRASHED,            /* Installed, but WCS_CNTRL_INSTR_ERROR set */
    SLAVE_NEEDS_BOOT,         /* Installed, above bit not set, not signed in */
    SLAVE_RUNNING,            /* Installed, bit not set, signed in */
} slave_state_t;


/*
 * rsp_hwidb_has_board_encap
 * Return TRUE if the interface is a MIP and has a board encapsulation.
 * Otherwise, return FALSE.
 */

static inline boolean
rsp_hwidb_has_board_encap (hwidbtype *hwidb) {

    if (hwidb->board_encap != NULL)
	return(TRUE);
    else
	return(FALSE);
}

/*
 * Some RSP Address filter defines, based on FCI defines
 */
#define RSP_ADDR_FILTER_SHIFT	8

#define RSP_STATION_MASK \
	(FCI_STATION_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_MULTICAST_MASK \
	(FCI_MULTICAST_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_BRIDGING_MASK \
	(FCI_BRIDGING_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_MAC_MASK \
	(FCI_MAC_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_HSRP_STATION_MASK \
	(FCI_HSRP_STATION_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_DOD_IP_PROTOCOL_MASK \
	(FCI_DOD_IP_PROTOCOL_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_DECNET_PROTOCOL_MASK \
	(FCI_DECNET_PROTOCOL_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_XNS_PROTOCOL_MASK \
	(FCI_XNS_PROTOCOL_MASK << RSP_ADDR_FILTER_SHIFT)
#define RSP_CLNS_PROTOCOL_MASK \
	(FCI_CLNS_PROTOCOL_MASK << RSP_ADDR_FILTER_SHIFT)

#define RSP_PROTOCOL_BIT_MASK \
	(FCI_PROTOCOL_BIT_MASK << RSP_ADDR_FILTER_SHIFT)

/*
 * memdcopy_aligned and memdcopy_unaligned have been removed. Reference 
 * CSCdi71609 for the flaws in the unaligned copy routines.
 *
 */

/*
 *
 * trail padding bytes needed in memdcopy() target memd buffers
 * actually 7 should suffice - round to 8 for good measure
 */
#define MEMDCOPY_PAD	8 



/*
 * Use Mips assembler instructions to copy.  Should be a little
 * faster than bcopy since we know that we don't have to worry
 * about a few bytes of trailing pad, don't have to worry about
 * overlap, and less icache thrashing.
 */
static inline void memdcopy (void *srcptr, void *dstptr, int bytes)
{
  /* 
   *  CSCdi71609: memdcopy_unaligned and memdcopy_aligned
   *    look up to 7 bytes past the end of the datagram.  This
   *    may cause another cache line to be brought in, but it 
   *    will not get invalidated later.  The next datagram
   *    may get transparently corrupted.
   *
   *  if ((int)dstptr & 7)
   *    memdcopy_unaligned(srcptr, dstptr, bytes);
   *  else
   *    memdcopy_aligned(srcptr, dstptr, bytes);
   */
   bcopy(srcptr, dstptr, bytes);
}

/*
 * Externals
 */
extern hwidbtype	*local_queue_to_hw_idb[];
extern uchar		*memd_base;	/* Base address of MEMD */
extern uint		memd_size;	/* size of MEMD in bytes */
extern ccbtype		*ccb_base; 	/* array of CCBs */
extern boolean          memd_carve_in_progress;
extern boolean          cbus_being_reset;
extern queuetype	fipclaimQ;
extern ulong            global_ip_counts_cache;
extern boolean		watch_action_crash;
extern boolean		memd_parity_error; /* global to indicate
					    * MEMD corrupt with
					    * bad parity
					    */

/*
 * Prototypes
 */
extern void	clear_cbus_info(void);
extern void	rsp_register_hwidb_for_local_free_queue(hwidbtype *);
extern void	rsp_fs_init(void);
extern int	rsp_qa_error_interrupt(r4k_context_t *cp);
extern void	writeback_inval_data(uchar *ptr, int bytes);
extern void	rsp_process_rawq(void);
extern void	rsp_process_eventq(void);
extern void	rsp_fastsend(hwidbtype *, paktype *);
extern void	rsp_tx_start(hwidbtype *);

extern void	rsp_memd_carve(boolean eoir);

extern boolean	rsp_assign_mac_addr(hwidbtype*);

extern void     rsp_check_for_pak_backing_store(hwidbtype *, paktype *);

/* rsp_queue_xxx_for_process() used by RSP vLAN Routing */

extern boolean  rsp_queue_ether_for_process(hwidbtype *,paktype *,ether_hdr *);
extern boolean  rsp_queue_fddi_for_process(hwidbtype *, paktype *);
extern boolean  rsp_queue_trip_for_process(hwidbtype *, paktype *, ushort);
extern void     rsp_fs_free_memd_pak (paktype *pak);

extern void     rsp_exception_init(void);
extern void     rsp_bus_init(void);
typedef enum {
    RSP_RSP1,
    RSP_RSP2,
    RSP_RSP7000,
    RSP_RSP4,
} rsp_flavor_t;
extern rsp_flavor_t rsp_flavor(void);
extern boolean  rsp_is_master(void);
extern boolean  rsp_is_slave(void);
extern void     promote_slave_state(void);
extern boolean  rsp_dual_capable(void);
extern void     rsp_print_global_queues(void);
extern void     rsp_print_if_queues(void);

/*
 * Warning: enable_cbus_grant() and disable_cbus_grant() are NOT nestable.
 * Any code that uses them cannot call routines which also use them.
 * It can call code that uses rsp_disable_cbuses()/rsp_restore_cbuses().
 */
extern void	enable_cbus_grant(void), disable_cbus_grant(void);

extern int	turbo_disabled(void);




#define FCI_TYPE_BASE	0x8000

/*
 * interrupt throttling definitions
 */

#define NETINT_MASK_MIN		100
#define NETINT_MASK_DEFAULT	200
#define NETINT_MASK_MAX		4000
#define NETINT_MIN		400
#define NETINT_DEFAULT		4000
#define NETINT_MAX		60000
#define NETINT_THROTTLE_DEFAULT	TRUE
#define NETINT_PKTS_PER_POLL	32

/*
 * Minimum memory size requirement for HSA
 */
 
#define HSA_MIN_MEMSIZE   (24*1024*1024)
 

extern void netint_throttle_show(void);
extern void sched_alloc_command(parseinfo*);

extern void rsp_serial_idb_init(hwidbtype *hwidb, boolean vip_style);
extern void rsp_ether_idb_init(hwidbtype *hwidb, boolean vip_style);
extern void rsp_fastether_idb_init(hwidbtype *hwidb, boolean vip_style);
extern void rsp_vip_4r_idb_init(hwidbtype *hwidb);
extern void rsp_fddi_idb_init(hwidbtype *hwidb, boolean vip_style);
extern void rsp_esa_idb_init(hwidbtype *hwidb, boolean vip_style);
extern void restore_idbs(slottype*);
extern void remove_idbs(slottype*);
extern void reset_ip(slottype*);
extern slave_state_t rsp_slave_state(void);


/**********************************************************************/
/* ReturnQ Support structures:                                        */
/**********************************************************************/

/* List of possible return queue action codes: */
#define RETQ_INVALID	0
#define RETQ_TX_RESTART	1

#define RETURNQ_SIZE	4096

typedef struct return_data_t_ {
    struct return_data_t_	*next;		/* Next one in queue */
    hwidbtype			*hwidb;		/* Saved Interface */
    ushort			type;		/* ReturnQ entry type */
    ushort			queue_ptr;	/* Saved queue MEMD offset */
    ushort			bufhdr_offset;	/* bufhdr for this struct */
    ushort			id;		/* ID for this struct */
						/* (also array index) */
} return_data_t;
#endif /* __RSP_IF_H__ */

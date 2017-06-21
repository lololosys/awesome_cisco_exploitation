/* $Id: rsp_cbus.c,v 3.16.6.23 1996/09/05 19:25:56 rramacha Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_cbus.c,v $
 *------------------------------------------------------------------
 * RSP cBus support
 *
 * March 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_cbus.c,v $
 * Revision 3.16.6.23  1996/09/05  19:25:56  rramacha
 * CSCdi66450:  CBUS-3-CMDTIMEOUT causes the fsip not to be detected
 * Branch: California_branch
 *
 * Revision 3.16.6.22  1996/08/28  13:13:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.6.21  1996/08/20  00:35:54  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.16.6.20  1996/08/15  15:32:04  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.16.6.19  1996/08/13  02:24:02  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.16.6.18  1996/08/12  06:12:39  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.16.6.17  1996/08/07  18:01:37  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.16.6.16  1996/07/26  01:11:05  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.16.6.15  1996/07/20  03:20:39  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.16.6.14  1996/07/08  22:52:28  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.16.6.13  1996/06/28  04:54:08  rramacha
 * CSCdi59063:  Cant AUTOINSTALL over RSP
 * Branch: California_branch
 *
 * Revision 3.16.6.12  1996/06/09  07:58:53  cheilek
 * CSCdi52427:  VIP1/VIP2 cardType enumeration labels should be vip/vip2
 * Branch: California_branch
 * Change MIBS: rvip to vip; svip to vip2.
 *
 * Revision 3.16.6.11  1996/06/04  05:04:30  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.16.6.10  1996/06/01  16:09:05  kao
 * CSCdi58054:  SegV exception, PC 0x60187794
 * Branch: California_branch
 *
 * Revision 3.16.6.9  1996/05/21  06:40:21  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.16.6.8  1996/05/13  14:47:33  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.16.6.7  1996/05/09  14:46:05  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.16.6.6  1996/04/29  16:05:43  fred
 * CSCdi54721:  tql set too low on fsip when no fancy queueing configured
 * Branch: California_branch
 *                 maxke sure cbus_mci_txlimit is called after changing
 *         the link's characteristic
 *
 * Revision 3.16.6.5.4.1  1996/04/27  07:25:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.16.6.5  1996/04/23  14:03:01  banderse
 * CSCdi50739:  CIP and other swdbus cards do not need extra reset
 * Branch: California_branch
 * use generic dbus_ctrlr_start() routine.
 *
 * Revision 3.16.6.4  1996/04/10  17:40:56  skufer
 * CSCdi50133:  cant set MTU larger than 8192 on 7500; error cant carve
 * anything
 * Branch: California_branch
 *
 * Revision 3.16.6.3  1996/04/06  08:43:54  ondiaye
 * CSCdi49252:  VIPER needs 16 tx queues in one status word: This is fixed
 *              by breaking the QAasic into two pools of txq and
 *              accumulators
 *              (poo1 and pool2). VIP2 txq's get allocated from pool2.
 *              VIP2
 *              txq's need to be continuous and their accumulators must at
 *              2 byte offset from their queues.
 *              This is temporary in a sense that it won't work well for
 *              the
 *              master/slave rsp model. Marty is working on that version.
 * Branch: California_branch
 *
 * Revision 3.16.6.2.6.1  1996/04/08  02:09:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.16.6.2  1996/03/21  23:49:41  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.16.6.1  1996/03/18  22:01:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.16.8.1  1996/03/28  17:36:13  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.16  1996/03/06  19:13:02  foster
 * CSCdi49134:  HSA : OIR event causes router to crash
 * don't reset the slave if it is running durning oir
 *
 * Revision 3.15  1996/03/05  04:01:50  kao
 * CSCdi50679:  IPC fast message is broadcasted to all IPC-capable boards
 *
 * Revision 3.14  1996/02/15  00:47:44  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Include our peer RSP in the chassis MIB.
 *
 * Revision 3.13  1996/02/14  17:04:48  skufer
 * CSCdi48055:  %CBUS-3-NOBUF: Buffer allocation failure: cant carve
 * anything
 *
 * Revision 3.12  1996/02/08  23:11:12  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.11  1996/02/04  02:29:27  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.10  1996/02/01  06:09:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/25  11:22:10  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.8.12.1  1996/01/24  23:03:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/06  08:52:43  dlobete
 * CSCdi46092:  %IPC_RSP_CBUS-3-NOBUF message on RSP with VIP under traffic
 * Fall back to other free queues if IPC queue is depleted.
 * Make no-ipc-buffer message have traceback and also rate-limited.
 *
 * Revision 3.7  1995/12/28  08:24:42  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.6  1995/12/26  23:11:36  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.5  1995/12/10  23:28:14  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.4  1995/12/02  05:50:42  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/27  21:34:05  yanke
 * Use cbus protection mechanism to guard slave rsp from crashing
 * during cbus stalled condition.
 * Recover ipc operation on master rsp when stall condition goes away.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  18:47:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:27:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/11  07:10:25  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.9  1995/09/26  21:32:51  motto
 * Decouple IPC from RSP subsystem.
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.8  1995/09/17  20:55:15  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.7  1995/08/10  20:34:03  schaefer
 * CSCdi38496: output hang timestamp used as future timer
 *
 * Revision 2.6  1995/08/09  00:21:51  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.5  1995/08/01  05:06:49  mcieslak
 * Added code review changes.
 * CSCdi37978:  Do not disable CyBus parity for HIP w/Rev.2 MD ASIC
 *
 * Revision 2.4  1995/08/01  02:30:02  mcieslak
 * If the board revision indicates that the Rev.2 ASIC is present,
 * don't disable the CyBus parity when a HIP is present.
 * CSCdi37978:  Do not disable CyBus parity for HIP w/Rev.2 MC ASIC
 *
 * Revision 2.3  1995/07/17  08:21:52  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.2  1995/07/16  22:30:51  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.1  1995/06/07  22:57:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "plural.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "sys_registry.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/network.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/dbus.h"
#include "../hes/ucode.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../src-4k/signal_4k.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"
#include "rsp_if.h"
#include "qa_hw.h"
#include "rsp_pcmap.h"
#include "rsp_commands.h"
#include "../hes/cbus_registry.regh"
#include "../hes/cbus_registry.regc"
#include "../hes/if_hes_common.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../ui/debug.h"
#include "../hes/cbus_debug.h"
#include "../snmp/chassismib_registry.h"
#include "../h/address.h"
#include "../if/atm.h"
#include "../hes/if_vip.h"
#include "../hes/dgbus.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_qa.c"		/* Not a typo, see logger.h */
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "rsp_common_if.h"

#include "../wan/smds.h"
#include "../ipc/ipc_seats.h"
#include "rsp_ipc_private.h"
#include "../pas/pas_id.h"

void cbus_mci_txlimit(hwidbtype *hwidb);


/*
 * count of "unsuitable" controllers (in need of hw upgrade)
 */
static uint unsuitable_ctrlrs;


/*
 * True during the carve phase.  In particular, true while executing the
 * ctrlr_analyze routines, but false by the time we are executing the
 * ctrlr_init and if_init (similar to the c7000's cbus->configuring)
 */
boolean memd_carve_in_progress;

/*
 * True throughout the entire bus resetting process.
 * (similar to the c7000's cbus->resetting)
 */
boolean cbus_being_reset;


/*
 * Pointers to Global queues
 * if you add more data objects, be sure to clear them
 * in the "clear global qa data" function
 */

qa_hwq_t	*RawQ;
qa_hwq_t	*ReturnQ;
qa_hwq_t	*EventQ;
qa_hwq_t	*BufhdrQ;
qa_hwq_t	*IpcbufQ;
qa_hwq_t	*IpcbufQ_classic; /* dual RSP and CIP pool */
static qa_hwq_t	*LovltrQ;

static int bufhdrq_elements;
static int lovltrq_elements;
static int lovltrq_bufsize;
static int ipcbufq_elements;
static int ipcbufq_elements_classic;  
static int ipcbufq_bufsize;
static int ipcbufq_bufsize_classic;

/*
 * this keeps track of how much MEMD has been "permanently" allocated
 * to survive across recarvings
 */
static int permanently_allocated;

/*
 * keep track of # of recarves, amount of MEMD wasted
 */
static ulong memd_unused, memd_recarves;

/*
 * count lost buffers
 */
uint lost_memd_buffers;

/*
 * count initial memd carve calls
 */
uint initial_carve_calls;

/*
 * Number of different buffer pools
 * includes buffers for receive & process level pool,
 */
#define MAX_RX_BUFFER_POOLS	4
#define CLASSIC_IPC_POOL        1  /* one pool for dual RSP and CIP ipc only */
#define TURBO_IPC_POOL          1  /* one pool primarily for VIP's */
#define NUM_IPC_POOLS           (CLASSIC_IPC_POOL + TURBO_IPC_POOL)
#define EXTRA_BUFFER_POOLS      2  /* just to be tolerant of future expansion */
#define NUM_PROCESS_POOLS	(2 + NUM_IPC_POOLS) /* minibuffer & full-size pools */
#define MAX_BUFFER_POOLS	(MAX_RX_BUFFER_POOLS + NUM_PROCESS_POOLS + \
				 1 /* lovenotes */ + 1 /* loveletters */ + \
				 EXTRA_BUFFER_POOLS )
#define MULTICAST_BUFHDRS	0

/* mnemonics to designate how to carve memd */
#define DONT_OVER_ALLOCATE  FALSE /* dont let smaller buffers get too much  */
#define OVER_ALLOCATE_OKAY  TRUE  /* see memd_alloc()                       */

/*
 * How many full-size buffers to allocate for process level output
 * sourced by us.  This is for our routing updates, etc., which are
 * high priority packets to output.
 */
#define PROCESS_TX_PACKETS	4

/*
 * the same, except for minibuffer packets
 * typically we'll get a lot more than this as these are allocated to
 * fill space until the end of a page
 */
#define PROCESS_MINIBUF_COUNT	8

#define PAGESIZE		4096
#define TWO_PAGESIZE    (PAGESIZE << 1)
#define THREE_PAGESIZE  (PAGESIZE * 3)

#define PAGEROOM		128

#define ROUND_UP(a,b)		(((a) + ((b) - 1)) & ~((b) - 1))
#define CBUSBUFROUND(m)		ROUND_UP(m, 32)
#define CBUSOFFSETSIZE           \
    CBUSBUFROUND(ROUND_UP(MAX(ENCAPBYTES,RSP_MAX_BYTE_RECEIVE_OFFSET),2) + \
 						RSP_BUFSTART_PAD)

#define PROCESS_MINIBUF_SIZE	256
#define CBUSBUFDELTA_START	256

/*
 * The queues with valid, non zero sizes should be inserted into this
 * array in ascending size order.
 * Also, the private process level free queue must follow all other
 * free queues.
 */
typedef struct bufpool_ {
    int		size;		/* size of buffers in this pool (in bytes)
				 * does not include CBUSOFFSETSIZE */
    int		real_size;	/* size (possibly including CBUFOFFSETSIZE) */
    qa_hwq_t	*qptr;		/* ptr to hardware queue */
    int		nb;		/* number of buffers in pool */
    boolean     two_pg_limit;    /* some alu-based IPs require at most 2pgs */
                                /*  starts bzero'd at FALSE                */
    boolean     three_pg_limit; /* some alu-based IPs require at most 3pgs */
} bufpool_t;

#define BUFPOOL_BADSIZE	(-1)
#define BUFPOOL_NONE	(-1)


/*
 * carve-time data about a particular buffer pool
 */
typedef struct bpdata_t_ {
    uint bandwidth_sum;		/* combined bandwidth of the sw interfaces */
    ushort ninterfaces;		/* number of hw interfaces sharing this pool */
    short nb;			/* number we decided to carve */
    bufpool_t pool;		/* buffer pool data */
    tinybool global;		/* copy non-rx pool to global table */
    tinybool no_extra;		/* don't carve extras in this pool */
    uchar orig_index;		/* original index */
    uchar type;			/* type of nonrx, non-global buf pool */
} bpdata_t;

#define RSP_LOVE_POOL_TYPE	0
#define RSP_IPC_POOL_TYPE_TURBO  	1
#define RSP_IPC_POOL_TYPE_CLASSIC	2

/*
 * carve-time data about the state of the carve procedure
 */
typedef struct carvdata_t_ {	/* data useful during buffer carving */
    uint totbw;			/* total bandwidth of all IPs */
    ushort maxbuf;		/* max buffer size of all pools */
    ushort extra_lovenotes;	/* special per-IP lovenote requirements */
    ushort lovltrs_wanted;	/* number of loveletters to allocate */
    ushort ninterfaces;		/* number of interfaces */
    ushort num_bp;		/* number of defined pools */

    bpdata_t bpdata[MAX_BUFFER_POOLS];
    uchar  bp_index[MAX_BUFFER_POOLS];

    ushort bufhdrs_left;	/* count of unallocated bufhdrs */
    tinybool quiet;		/* don't complain about buf alloc failures */
    uchar* bufdata_ptr;		/* ptr to start of unallocated buffer mem */
    uchar* bufdata_end;		/* ptr to end of buffer mem */
    uint memd_unused;		/* bytes of memd wasted */
    bpdata_t* bp_fallback;	/* bp allocated in place of requested bp */
    uint real_max_twopagesize;  /* max 2 page buffer mtu + buffer offset size */
    uint real_max_threepagesize; /*max 3 page buffer mtu + buffer offset size*/
} carvdata_t;


/*
 * post-carve data describing the buffer pools
 */
static bufpool_t bufpool[MAX_RX_BUFFER_POOLS + NUM_PROCESS_POOLS];



/*
 * complain if we find a card we can't analyze
 */
static void
default_ctrlr_analyze (slottype* slotp)
{
    char buf[32];

    slotp->flags |= BOARD_DISABLED;
    sprintf(buf, "slot %u, type %u", slotp->slot_num, slotp->ctrlr_type);
    errmsg(&msgsym(MODULE, CBUS), "controller analyze", buf);
    return;
}

/*
 * complain if we find a card we can't initialize
 */
static void
default_ctrlr_init (slottype* slotp)
{
    char buf[32];

    slotp->flags |= BOARD_DISABLED;
    sprintf(buf, "slot %u, type %u", slotp->slot_num, slotp->ctrlr_type);
    errmsg(&msgsym(MODULE, CBUS), "controller init", buf);
    return;
}

/*
 * complain if we find an interface we can't initialize
 */
static void
default_if_init (hwidbtype* hwidb)
{
    errmsg(&msgsym(MODULE, CBUS), "interface init", hwidb->hw_namestring);
    return;
}

#define RX_POOL(bp)	((bp)->bandwidth_sum > 0)

/*
 * round up to the next page boundary
 */
static void
memd_alloc_page_align (carvdata_t* cd)
{
    ulong next, bufaddr;

    bufaddr = (ulong) cd->bufdata_ptr;
    next = (bufaddr + (PAGESIZE - 1)) & ~(PAGESIZE - 1);
    cd->memd_unused += (next - bufaddr);
    cd->bufdata_ptr = (uchar*) next;
    return;
}

/*
 * try to allocate a buffer for the given pool
 */
static boolean
memd_alloc (carvdata_t* cd, bpdata_t* bp, boolean over_allocate_valid)
{
    uint size, pass, nbytes_within_pages;
    bufhdr_t* bufhdr;
    bpdata_t* orig_bp;
    boolean wont_fit;

    if (cd->bufhdrs_left == 0) {
	if (!cd->quiet)
	    errmsg(&msgsym(NOBUF, CBUS), "no buffer headers");
	return FALSE;
    }

    pass = 1;
    orig_bp = bp;
    if (cd->bp_fallback == orig_bp)
	cd->bp_fallback = NULL;
    do {
	size = bp->pool.size;
	/*
	 * Note: all pools are of the exact size required except for
	 * rx buffer pools.  With rx buffer pools we add CBUSOFFSETSIZE 
	 * in front of the datagram to allow larger encapsulations to be 
	 * prepended -- without having to copy the datagram into another
	 * buffer. 
	 */
	if (RX_POOL(bp))
	    size += CBUSOFFSETSIZE;
	size = CBUSBUFROUND(size);

	wont_fit = FALSE;
	if (size > 0) {
	    ulong bufaddr;

	    bufaddr = (ulong) cd->bufdata_ptr;
	    /*
	     * less than PAGEROOM bytes from next PAGESIZE boundary?
	     */
	    if (RX_POOL(bp) &&
		((bufaddr & (PAGESIZE - 1)) > (PAGESIZE - PAGEROOM)))
		wont_fit = TRUE;
  
	    if ((cd->real_max_twopagesize)  && (size > (PAGESIZE + 1))) {
	       /* 
		* real_max_twopagesize is zero if no trip or fip present.
		* We have a alu based IP (e.g. TRIP/FIP) and this 
		* buffer would span more than a full page, possibly cross
		* 3 pages boundaries.  
		*
		* Whichever is smaller, real_max_twopagesize or size,
		* make sure these don't cross three page boundaries.
		* But be sure to subtract 1 from nbytes so exactly
		* two page sizes will fit.
		*/
	       nbytes_within_pages = MIN(size, cd->real_max_twopagesize);

	       if (((bufaddr + (nbytes_within_pages -1 )) / PAGESIZE 
 		      - (bufaddr / PAGESIZE)) > 1) { 
 		     wont_fit = TRUE;
 		     }  
	       }
   
	    if ((cd->real_max_threepagesize)  && (size > (TWO_PAGESIZE + 1))) {
	       /* 
		* real_max_threepagesize is zero if no hip is present.
		* We have a alu based IP (HIP) and this  buffer would 
		* span more than two full pages, possibly cross
		* 4 pages boundaries.  
		*
		* Whichever is smaller, real_max_threepagesize or size,
		* make sure these don't cross four page boundaries.
		* 
 		*/
	       nbytes_within_pages = MIN(size, cd->real_max_threepagesize);

	       if (((bufaddr + (nbytes_within_pages -1)) / PAGESIZE 
		      - (bufaddr / PAGESIZE)) > 2) { 
			wont_fit = TRUE;
	       }
	    }
   
	    /*
	     * not enough buffer space left for this size?
	     */
	    if ((cd->bufdata_ptr + size) > cd->bufdata_end)
		wont_fit = TRUE;
	    /*
	     * pool is full?
	     */
	    if (bp->no_extra && (bp->pool.nb >= bp->nb))
		wont_fit = TRUE;
	    /*
	     * don't fallback to the same bp twice in a row
	     */
	    if (bp == cd->bp_fallback)
		wont_fit = TRUE;

            /*
             * When we can't allocate a buffer pool with entry bp_orig,
             * we fall back to a smaller buffer pool, hoping it will fit.
             * and thereby allocating memory that would have been unused.
             *
             * But we don't allocate to a smaller bufpool if that pool
             * has already allocated the number of buffer pools it needs.
             * This would cause its allocation to be exceeded and thereby
             * preventing other pools from getting their allocation.
             * The message "Buffer allocation failure: can't carve anything".
             * was indicicative of this situation.
             *
             * We pass in the parameter stating whether it is valid to
             * over allocate a fallback buffer.  Over allocating is only
             * valid when we are allocating leftover buffers.
             */
            if  ( !(over_allocate_valid) && (bp != orig_bp)  && 
		 ( bp->pool.nb  >=  bp->nb ) ) {
                wont_fit = TRUE;
                }

	    if (wont_fit) {
		/*
		 * get a smaller pool (but skip lovenote pool)
		 */
		--bp;
		if ((bp < cd->bpdata) || (bp->pool.size == 0)) {
		    /*
		     * no more pools, take it from the top after page aligning
		     */
		    if (pass == 2) {
			if (!cd->quiet)
			    errmsg(&msgsym(NOBUF, CBUS),
				   "can't carve anything");
			return FALSE;
		    }
		    bp = orig_bp;
		    ++pass;
		    memd_alloc_page_align(cd);
		}					/* no more pools */
	    }						/* wont_fit */
	}						/* size > 0 */
    } while (wont_fit);

    bufhdr = &bufhdr_base[--cd->bufhdrs_left];
    if (size == 0) {
	bufhdr->bf_buffer_ptr = 0;
	bufhdr->bf_tx_bytes = 0xffff;	/* poison love note length */
    } else {
	bufhdr->bf_buffer_ptr = ADDR_TO_MEMD_OFFSET_24(cd->bufdata_ptr);
    }
    hwq_enqueue(bp->pool.qptr, bufhdr);
    ++bp->pool.nb;
    if (bp != orig_bp)
	cd->bp_fallback = bp;
    else
	cd->bp_fallback = NULL;

    cd->bufdata_ptr += size;
    return TRUE;
}

#define FOR_ALL_POOLS(cd, bp) \
    for ((bp) = (cd)->bpdata; (bp) < &(cd)->bpdata[(cd)->num_bp]; ++(bp)) \

#define FOR_ALL_RX_POOLS(cd, bp) \
    FOR_ALL_POOLS((cd),(bp)) \
	if (!RX_POOL(bp)) \
	    continue; \
	else

/*
 * scan existing pools for a match with the given buffer size
 * return index of existing pool or index of pool to create
 */
static int
find_bufpool (carvdata_t* cd, int bufsiz, int bufdelta)
{
    int diff;
    bpdata_t* bp;

    FOR_ALL_POOLS(cd, bp) {
	diff = bp->pool.size - bufsiz;
	if ((diff >= 0) && (diff <= bufdelta)) {
	    break;
	}
	if ((diff < 0) && (diff >= -bufdelta)) {
	    bp->pool.size = bufsiz;
	    break;
	}
    }
    return bp - cd->bpdata;
}

/*
 * compute the bandwidth of the interfaces associated with the given hwidb
 */
static int
hwidb_bw (hwidbtype* hwidb)
{
    int bw;
    idbtype* idb;

    bw = 0;
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (idb->visible_bandwidth_def == 0)
	    continue;
	bw += idb->visible_bandwidth_def;
    }
    return bw;
}

/*
 * add the given interface to the accumulated bandwidth counts, etc.
 * account for specially configured interface buffer needs
 */
static void
accumulate_bufdata (carvdata_t* cd, bpdata_t* bp, hwidbtype* hwidb)
{
    uint bw;

    ++bp->ninterfaces;
    bw = hwidb_bw(hwidb);
    bp->bandwidth_sum += bw;
    cd->totbw += bw;

    return;
}

/*
 * figure out what size buffer pools to create, and which interfaces
 * should share which pools
 */
static void
discover_bufsizes (carvdata_t* cd, int bufdelta)
{
    bpdata_t* bp;
    slottype* sp;
    hwidbtype* hwidb;
    int ix, bufsiz, plug;

    FOR_ALL_ENABLED_IP_SLOTS(sp) {
	for (plug = 0; plug < sp->interface_count; ++plug) {
	    hwidb = sp->interface_list[plug];
	    if (hwidb == NULL)
		continue;

	    hwidb->buffer_pool = BUFPOOL_NONE;
	    if (hwidb->buffersize == 0)
		continue;
            /*
             * For ATM interfaces, reset the maxdgram and buffersize
             * based on the new MTU that has been set.
 
             * If anyone else is calling set_maxdgram function without going
             * through max_mtu_command, this code has to be hacked...
             */
            if (is_atm(hwidb)) {
                /*
                 * When there is a change in MTU, set the maxdgram size
                 * and the buffersize to more than the new MTU.
                 */
		if_maxdgram_adjust(hwidb, (hwidb->maxmtu +
					   ETHER_ARPA_ENCAPBYTES +
					   ATM_SMDS_HDRBYTES));
                hwidb->buffersize = hwidb->max_buffer_size;
 
            }
 

	    ++cd->ninterfaces;
	    bufsiz = CBUSBUFROUND(hwidb->buffersize);
	    if (cd->maxbuf < bufsiz)
		cd->maxbuf = bufsiz;
	    ix = find_bufpool(cd, bufsiz, bufdelta);
	    bp = &cd->bpdata[ix];
	    hwidb->buffer_pool = ix;
	    if (ix == cd->num_bp) {
		/* return early if too many buffer pools */
		if (++cd->num_bp > MAX_RX_BUFFER_POOLS)
		    return;

		bp->pool.size = bufsiz;

		cd->bp_index[ix] = ix;
		bp->orig_index = ix;
	    }
	    accumulate_bufdata(cd, bp, hwidb);

	    /* older ALU-based IPs require buffer to span 2 pgs max */
	    if ( (sp->ctrlr_type == FCI_CTR_CONTROLLER)   /* TRIP */ ||
		 (sp->ctrlr_type == FCI_FDDIT_CONTROLLER) /* FIP  */ ) {
		 /* if mtu is larger than a page, keep track of it */
		 if (CBUSBUFROUND(bp->pool.size + CBUSOFFSETSIZE) > (PAGESIZE + 1) ) { 
		     bp->pool.two_pg_limit = TRUE;
		 }
	    }

	    /* HIP's require buffer to span 3 pgs max */
	    if (sp->ctrlr_type == FCI_LINK_CONTROLLER) {
		/* if mtu + offsetsize is larger than three pages,
		 *keep track of it
		 */
		if (CBUSBUFROUND(bp->pool.size + CBUSOFFSETSIZE) > 
		   (TWO_PAGESIZE + 1)) {
		     bp->pool.three_pg_limit = TRUE;
		}
	    }

	}
    }
    return;
}

/*
 * handle special buffers needs on a per-IP basis
 */
static void
per_ip_requirements (carvdata_t* cd)
{
    slottype* slotp;

    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	switch (slotp->ctrlr_type) {
	  case FCI_FDDIT_CONTROLLER: {
	      hwidbtype* hwidb;

	      hwidb = slotp->interface_list[0];
	      if (hwidb == NULL)
		  continue;

	      if (hwidb->claimbuf)
		  continue;	      /* if we have one already, skip it */

	      /* try to get one previously allocated but currently unused */
	      hwidb->claimbuf = data_dequeue(&fipclaimQ);
	      if (hwidb->claimbuf == NULL) {
		  /* if none available, alloc new space */
		  hwidb->claimbuf = cd->bufdata_ptr;
		  cd->bufdata_ptr +=
		      CBUSBUFROUND(FDDI_CLAIM_BEACON_BUFFER_SIZE);
	      }

	      /*
	       * FIPs get three lovenote each, which means we want two extra
	       */
	      cd->extra_lovenotes += 2;
	      break;
	  }

	  case FCI_MIP_CONTROLLER: {
	      int plughole_count;

	      plughole_count = ((slotp->interface_list[0] != NULL) +
				(slotp->interface_list[1] != NULL));
	      cd->extra_lovenotes += plughole_count;
	      cd->lovltrs_wanted += plughole_count + 1;
	      break;
	  }

	  case FCI_AIP_CONTROLLER: {
	      /*
	       * Worst case is per-VC statistics:
	       * 2 for Rx/Tx In Progress, 2 for Rx/Tx being processed
	       * these are big, so assume we'll get to share some
	       */
	      cd->lovltrs_wanted += 2;
	      break;
	  }
	  case FCI_CIP_CONTROLLER: {
	      int plughole_count;

	      plughole_count = ((slotp->interface_list[0] != NULL) +
				(slotp->interface_list[1] != NULL));
	      cd->lovltrs_wanted += plughole_count * 2;
	      break;
	  }

	  case FCI_RVIP_CONTROLLER: 
	  case FCI_SVIP_CONTROLLER: {
	      /*
	       * Each interface needs one loveletter for answering the RP.
	       */
	      int i;
	      for (i=0; i < slotp->interface_count; i++) {
		  if (slotp->interface_list[i] != NULL)
		    cd->lovltrs_wanted++;
          }

          /* 
           * In case there are two FDDI Port adapters, add 4 more.
           */
           cd->lovltrs_wanted +=4; 

	      break;
	  }

	  case FCI_POSIP_CONTROLLER:	      
	  case FCI_FEIP_CONTROLLER: {
	      int plughole_count;

	      plughole_count = ((slotp->interface_list[0] != NULL) +
				(slotp->interface_list[1] != NULL));
	      cd->lovltrs_wanted += plughole_count * 2;
	      break;
	  }

	  default:
	    break;
	}
    }
    return;
}

/*
 * define buffer pools for other uses than received packets
 */
static void
define_nonrx_pools (carvdata_t* cd)
{
    bpdata_t* bp;
    int num_turbo, num_classic;

    /* define minibuffer pool */
    bp = &cd->bpdata[cd->num_bp++];
    bp->nb = PROCESS_MINIBUF_COUNT;
    bp->global = TRUE;
    bp->pool.size = PROCESS_MINIBUF_SIZE;

    if (cd->maxbuf > 0) {
	/* define full-size process level pool */
	bp = &cd->bpdata[cd->num_bp++];
	bp->nb = PROCESS_TX_PACKETS;
	bp->global = TRUE;
	bp->no_extra = TRUE;
	bp->pool.size = CBUSBUFROUND(RSP_BUFSTART_PAD + cd->maxbuf);
    }

    /* define lovenote pool */
    bp = &cd->bpdata[cd->num_bp++];
    bp->nb = (MULTICAST_BUFHDRS + cd->ninterfaces + cd->extra_lovenotes);
    bp->type = RSP_LOVE_POOL_TYPE;

    if (cd->lovltrs_wanted > 0) {
	/* define loveletter pool */
	bp = &cd->bpdata[cd->num_bp++];
	bp->nb = cd->lovltrs_wanted;
	bp->no_extra = TRUE;
	bp->pool.size = CBUSBUFROUND(RSP_MAX_LOVE_LETTER);
	bp->type = RSP_LOVE_POOL_TYPE;
    }

    /* define ipc buffer pool */
    num_turbo = 0;
    num_classic = 0;
    reg_invoke_ipc_cbus_num_of_cards( &num_turbo, &num_classic);
    if (num_turbo) {
	bp = &cd->bpdata[cd->num_bp++];
	bp->nb = BUFF_PER_IPC_CARD * num_turbo;
	bp->nb += BUFF_PER_IPC_CARD; /* master rsp not included in num_turbo */
	bp->no_extra = TRUE;
	bp->pool.size = CBUSBUFROUND(RSP_MAX_IPC_BUFF_SIZE);
	bp->type = RSP_IPC_POOL_TYPE_TURBO;
    }
    if (num_classic) {
	bp = &cd->bpdata[cd->num_bp++];
	bp->nb = BUFF_PER_IPC_CARD * num_classic;
	bp->no_extra = TRUE;
	bp->pool.size = CBUSBUFROUND(RSP_MAX_IPC_BUFF_SIZE);
	bp->type = RSP_IPC_POOL_TYPE_CLASSIC;
    }

    return;
}

/*
 * bubble-sort the buffer pools from smallest to largest
 * RX pools first if sizes are equal
 */
static void
sort_by_bufsize (carvdata_t* cd)
{
    int ix, jx;
    bpdata_t* bp;
    uint real_buf_size;

    /* sort the buffer pools by mtu */
    for (ix = 0; ix < (cd->num_bp - 1); ++ix) {
	for (jx = (ix + 1); jx < cd->num_bp; ++jx) {
	    bpdata_t tmp;

	    if (cd->bpdata[ix].pool.size < cd->bpdata[jx].pool.size)
		continue;
	    if ((cd->bpdata[ix].pool.size == cd->bpdata[jx].pool.size) &&
		RX_POOL(&cd->bpdata[ix]))
		continue;
	    /* swap the structs */
	    tmp = cd->bpdata[jx];
	    cd->bpdata[jx] = cd->bpdata[ix];
	    cd->bpdata[ix] = tmp;
	}
    }

    FOR_ALL_RX_POOLS(cd, bp) {
	/*
	 * if TRIP or FIP are present calculate the largest
	 * buffer that spans at most 2 pages.
	 */
	if ( bp->pool.two_pg_limit ) { 
	    real_buf_size = CBUSBUFROUND(bp->pool.size + CBUSOFFSETSIZE);
	    if (real_buf_size > cd->real_max_twopagesize) { 
	      cd->real_max_twopagesize = real_buf_size;
	   }
	}
	/*
	 * if HIP is present calculate the largest
	 * buffer that spans at most 3 pages.
	 */
	if ( bp->pool.three_pg_limit ) { 
	    real_buf_size = CBUSBUFROUND(bp->pool.size + CBUSOFFSETSIZE);
	    if (real_buf_size > cd->real_max_threepagesize) { 
	      cd->real_max_threepagesize = real_buf_size;
	   }
	}
    }

    /*
     * ensure the largest datagram  going to or from a
     * trip/fip does not span more than two  pages, and
     * that hip's do not cross more than 3 pages.
     *
     */
    if (cd->real_max_twopagesize > TWO_PAGESIZE) {
	cd->real_max_twopagesize = TWO_PAGESIZE;
    }

    if (cd->real_max_threepagesize > THREE_PAGESIZE) {
	cd->real_max_threepagesize = THREE_PAGESIZE;
    }


    /* map the original indexes to the swapped structs */
    FOR_ALL_RX_POOLS(cd, bp)
	cd->bp_index[bp->orig_index] = bp - cd->bpdata;

    return;
}

/*
 * allocate queues for the defined buffer pools
 */
static void
alloc_free_queues (carvdata_t* cd)
{
    bpdata_t* bp;

    /* allocate a global queue for all pools */
    FOR_ALL_POOLS(cd, bp) {
	int qindex;

	qindex = get_hwq(QT_GFREE, 1);
	if (qindex == QA_ALLOC_ERROR) {
	    errmsg(&msgsym(ALLOC, QA), "global free queue failure");
	    cd->num_bp = (bp - cd->bpdata);
	    break;
	}
	bp->pool.qptr = queue_ptr(qindex);
    }
    return;
}

/*
 * create buffers for a pool
 */
static void
carve_pool (carvdata_t* cd, bpdata_t* bp, boolean over_alloc_valid)
{
    while (bp->pool.nb < bp->nb) {
	if (!memd_alloc(cd, bp, over_alloc_valid))
	    break;
    }
    return;
}

/*
 * allocate buffers for zero-bandwith pools (process level)
 */
static void
alloc_nonrx_pools (carvdata_t* cd)
{
    bpdata_t* bp;

    /* allocate buffers for all non-bandwidth dependent pools */
    FOR_ALL_POOLS(cd, bp) {
	if (RX_POOL(bp))
	    continue;

	carve_pool(cd, bp, DONT_OVER_ALLOCATE);
    }
    return;
}

/*
 * calculate number of buffers per rx pool
 * adjust bandwidths so that each buffer pool gets at least 20% of
 * available buffer memory
 */

#define PERCENT(v, pc)	(((v) * (pc)) / 100)

static void
compute_rx_nb (carvdata_t* cd)
{
    bpdata_t* bp;
    uint bw_min, totbw_over_min, memd_avail, rx_pools;

    bw_min = PERCENT(cd->totbw, 20);
    totbw_over_min = 0;
    rx_pools = 0;
    FOR_ALL_RX_POOLS(cd, bp) {
	++rx_pools;
	if (bp->bandwidth_sum > bw_min)
	    totbw_over_min += (bp->bandwidth_sum - bw_min);
    }
    memd_avail = (cd->bufdata_end - cd->bufdata_ptr);
    /*
     * derate it by 2% to allow for memory used to align buffers
     */
    memd_avail -= PERCENT(memd_avail, 2);
    FOR_ALL_RX_POOLS(cd, bp) {
	uint mem;

	mem = PERCENT(memd_avail, 20);
	if (bp->bandwidth_sum > bw_min)
	    mem += ((PERCENT(memd_avail, 100 - 20 * rx_pools) /
		     totbw_over_min) *
		    (bp->bandwidth_sum - bw_min));
	bp->nb = mem / CBUSBUFROUND(bp->pool.size + CBUSOFFSETSIZE);
    }
    return;
}

/*
 * allocate buffers for rx buffer pools
 */
static void
alloc_rx_pools (carvdata_t* cd)
{
    int ix;

    if (cd->totbw == 0)
	return;			/* no interfaces */

    compute_rx_nb(cd);

    for (ix = cd->num_bp; ix > 0; --ix) {
	bpdata_t* bp;

	bp = &cd->bpdata[ix - 1];
	if (!RX_POOL(bp))
	    continue;

	carve_pool(cd, bp, DONT_OVER_ALLOCATE);
    }

    return;
}

/*
 * allocate any leftover buffer space
 */
static void
alloc_leftover_buffers (carvdata_t* cd)
{
    int phase;
    bpdata_t* bp;

    cd->quiet = TRUE;
    for (phase = 0; phase < 4; ++phase) {
	bp = &cd->bpdata[cd->num_bp];
	while (--bp >= cd->bpdata) {
	    switch (phase) {
	      case 0:			/* skip full pools */
		if (bp->pool.nb >= bp->nb)
		    continue;

	      case 1:			/* skip non-RX pools */
		if (!RX_POOL(bp))
		    continue;

	      case 2:			/* skip lovenote pool */
		if (bp->pool.size == 0)
		    continue;
	    }
	    while (memd_alloc(cd, bp, OVER_ALLOCATE_OKAY))
		/* keep allocating */;
	}
    }
    cd->quiet = FALSE;
    return;
}

/*
 * save pool data
 * uses somewhat fragile code to separate the lovenote and loveletter
 * pools from the others
 */
static void
write_bufpool (carvdata_t* cd, bufpool_t* pool)
{
    int pools;
    bpdata_t* bp;
    bufpool_t* pp;

    pools = 0;
    FOR_ALL_POOLS(cd, bp) {
	pp = &bp->pool;
	if (RX_POOL(bp)) {
	    pp->real_size = CBUSOFFSETSIZE + pp->size;
	    pool[pools++] = *pp;
	    continue;
	}
	if (bp->global) {
	    pp->real_size = pp->size;
	    pool[pools++] = *pp;
	    continue;
	}
	if (bp->type == RSP_IPC_POOL_TYPE_TURBO) {
	    IpcbufQ = pp->qptr;
	    ipcbufq_elements = pp->nb;
	    ipcbufq_bufsize = pp->size;
	} else if (bp->type == RSP_IPC_POOL_TYPE_CLASSIC) {
	    IpcbufQ_classic = pp->qptr;
	    ipcbufq_elements_classic = pp->nb;
	    ipcbufq_bufsize_classic = pp->size;
	} else {
	    if (bp->pool.size == 0) {
		BufhdrQ = pp->qptr;
		bufhdrq_elements = pp->nb;
	    } else {
		LovltrQ = pp->qptr;
		lovltrq_elements = pp->nb;
		lovltrq_bufsize = pp->size;
	    }
	}
    }
    /* mark end of buffer pools */
    if (pools < MAX_BUFFER_POOLS)
	pool[pools].size = BUFPOOL_BADSIZE;

    return;
}

/*
 * allocate and initialize global queue data structures
 */
static void
init_input_queues (carvdata_t* cd)
{
    int qindex;

    /*
     * Initialize the pointers to unique global queues
     */
    qindex = get_hwq(QT_RAW, 1);
    RawQ = queue_ptr(qindex);
    set_qa_rp_attention(qindex, QA_ATTN_NET);

    qindex = get_hwq(QT_RETURN, 1);
    ReturnQ = queue_ptr(qindex);
    set_qa_rp_attention(qindex, QA_ATTN_NET);

    qindex = get_hwq(QT_EVENT, 1);
    EventQ = queue_ptr(qindex);
    set_qa_rp_attention(qindex, QA_ATTN_CMD);

    return;
}

/*
 * remove an IP from the picture.
 * It has been unable to successfully complete its initialization.
 *
 * Remove the IDBs from the system IDB queues so that they are not
 * accessed. We don't want to mark the board as not analyzed otherwise
 * we will lose the configuration information during a "micro reload" or
 * complex restart.
 *
 * If a card is unresponsive or otherwise unable to successfully
 * complete the initialization sequence at complex restart, then
 * disable (reset?) the card and mark it disabled.
 *
 * Concern:
 * In the case of a card that is truly hosed, disabling it via
 * 'disable_slot()' may be ineffective. Hence, a reset should be issued.
 * If this is the case, then this function can be removed and calls to it
 * can be replaced with reset_ip() instead.
 *
 * Note that a call to disable_ip() without an associated start of the
 * card's crash timer, will render this board dormant until the next
 * microcode reload of all cards in the box.  There is no other mechanism
 * to bring this card back to life.
 */
static void
disable_ip (slottype* sp)
{
    disable_slot(sp->slot_num);
    remove_idbs(sp);
    return;
}
/*
 * Issue a hard reset to the card.  This will categorically bring
 * it out of a hung condition, yet leave it in an unenabled state.
 * 
 * The card will require a microcode reload before it is enabled.
 *
 * Since the card will be unable to respond to cbus requests,
 * remove the IDBs from the system IDB queues so that they are not
 * accessed. 
 *
 * Likewise, mark the slot as disabled, so the hwidbs in its
 * interface list will not be accessed.
 *
 * Remove the idbs and mark the slot DISABLED before resetting
 * the card, so that access to the card will not be made while
 * it is being reset.
 */
void
reset_ip (slottype* sp)
{    
    uint slot;
    dgtype *dbus;

    slot  = sp->slot_num;
    dbus = pslot_to_dbus(slot);

    remove_idbs(sp);
    sp->flags |= BOARD_DISABLED;
    dbus_hard_ctrlr_reset(dbus);
}

/*
 * initialize the controllers (interface processor cards)
 */
static void
invoke_ctrlr_init (void)
{
    slottype* slotp;

    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	if (cbus_debug)
	    buginf("\nController init %s%u",
		   slotp->card_name, slotp->slot_num);

	slotp->event_q_ptr = EventQ;
	slotp->love_note_free_q_ptr = BufhdrQ;
	slotp->love_letter_free_q_ptr = LovltrQ;
	reg_invoke_ctrlr_init(slotp->ctrlr_type, slotp);
	if (slotp->flags & BOARD_DISABLED)
	    disable_ip(slotp);

	if (cbus_debug)
	    buginf("\nController %s%u initialized",
		   slotp->card_name, slotp->slot_num);
    }
    return;
}

/*
 * set_cbus_txlimit
 *
 * for interfaces with defined transmit queue limits (interfaces with
 * supporting hardware which has been configured with a tql), make sure
 * the tql matches current interface characteristics.
 *
 * this is called as a registry function, as it applies only to the RSP and
 * the 7000.
 */
static void set_cbus_txlimit (hwidbtype *hwidb)
{
    if (hwidb->mci_txcount_default && hwidb->tx_acc_ptr)
	cbus_mci_txlimit(hwidb);
}
/*
 * initialize the interfaces (plugholes) on each controller
 */
static void
invoke_if_init (carvdata_t* cd)
{
    int plughole_num;
    slottype* slotp;
    int svip_tx_queue_base;

    /*
     * Note that some plugholes, for example the MIP, have multiple
     * hwidbs associated with them, but only one set of hardware queues
     * and accumulators.  For this reason, we need to iterate through
     * only the 'base' hwidbs, rather then use the more traditional
     * FOR_ALL_HWIDBS construct.  The first step is to iterate thru'
     * all the slots.
     */
    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	/*
	 * Now iterate through all the possible plugholes in this
	 * slot.
	 */
	if (cbus_debug)
	    buginf("\nInterface init %s%u", slotp->card_name, slotp->slot_num);

	/* 
	 * We need to allocate 16 transmit queues in a block for VIPER. 
	 * Indicate we have not yet allocated a block of 16
	 */
	svip_tx_queue_base = 0;
	if (slotp->ctrlr_type == FCI_SVIP_CONTROLLER) {
	    svip_tx_queue_base= reserve_txhwq(slotp->interface_count);
	    if (svip_tx_queue_base == QA_ALLOC_ERROR) {
	        errmsg(&msgsym(ALLOC, QA), "Svip tx queue failure");
		continue;
	    }
	}
	for (plughole_num = 0; plughole_num < slotp->interface_count;
	     plughole_num++) {
	    int index;
	    bpdata_t* bp;
	    hwidbtype* hwidb;

	    hwidb = slotp->interface_list[plughole_num];
	    if ((hwidb == NULL) || (hwidb->buffer_pool == BUFPOOL_NONE))
		continue;

	    hwidb->raw_q_ptr = RawQ;

	    bp = &cd->bpdata[cd->bp_index[hwidb->buffer_pool]];

	    hwidb->global_free_q_ptr = bp->pool.qptr;
	    hwidb->global_free_q_size = bp->pool.size;

	    hwidb->rx_limit_lo = min(bp->pool.nb / bp->ninterfaces, 4);
	    hwidb->rx_limit_hi = min(bp->pool.nb / bp->ninterfaces * 3,
				     bp->pool.nb);
	    hwidb->rx_currently_owned = 0;
	    hwidb->max_rx_currently_owned = 0;
	    hwidb->old_txcount = 0;
	    hwidb->mci_txcount_default = ((bp->pool.nb * 2) /
					  (bp->ninterfaces + 1));
	    if (!hwidb->mci_txcount_enabled)
		hwidb->mci_txcount = hwidb->mci_txcount_default;

	    index = get_hwq(QT_LFREE, 1);
	    if (index == QA_ALLOC_ERROR) {
		errmsg(&msgsym(ALLOC, QA), "local free queue failure");
		continue;
	    }
	    hwidb->local_free_q_ptr = queue_ptr(index);
	    rsp_register_hwidb_for_local_free_queue(hwidb);

	    if ( svip_tx_queue_base )  {
	        if (index == QA_ALLOC_ERROR) {
		    errmsg(&msgsym(ALLOC, QA), "local free queue failure");
		    continue;
		}
		index = svip_tx_queue_base++;
		hwidb->tx_acc_ptr = acc_ptr(index);
		hwidb->tx_q_ptr = queue_ptr(index);
                /* VIP Channelized interface does not need to have an */
                /* tx_acc for the base idb */
                if (hwidb->fci_type & FCI_TYPE_BASE) {
                    ret_hwacc(hwidb->tx_acc_ptr);
                    hwidb->tx_acc_ptr = NULL;
                }

	    } else {

		index = get_hwq(QT_TX, 1);
		if (index == QA_ALLOC_ERROR) {
		    errmsg(&msgsym(ALLOC, QA), "tx queue failure");
		    continue;
		}
		hwidb->tx_q_ptr = queue_ptr(index);

		/* MIP's accumulator is allocated per channel group */
		if (slotp->ctrlr_type != FCI_MIP_CONTROLLER) {
		    if ((hwidb->tx_acc_ptr = get_hwacc()) == NULL) {
			errmsg(&msgsym(ALLOC, QA), "tx accumulator failure");
			continue;
		    }
		} 
		else {
		    hwidbtype *vhwidb;
		
		    /*
		     * MIP channel interface's tx_acc is allocated 
		     * per channel.
		     */
		    vhwidb = hwidb->next_idb_on_physint;
		    while (vhwidb) {
			if ((vhwidb->tx_acc_ptr = get_hwacc()) == NULL) {
			    errmsg(&msgsym(ALLOC, QA), "tx accumulator failure");
			    break;
			}
			vhwidb = vhwidb->next_idb_on_physint;
		    }
		}
	    }

	    if (cbus_debug)
	      	buginf("\nslot %d plughole %d txq %x acc %x",
		       slotp->slot_num, plughole_num, hwidb->tx_q_ptr, hwidb->tx_acc_ptr);

	    reg_invoke_if_init(hwidb->slot_ptr->ctrlr_type, hwidb);
	}
	if (cbus_debug)
	    buginf("\n%s%u interfaces initialized",
		   slotp->card_name, slotp->slot_num);
    }
    return;
}
	    
/*
 * reset any CDBs associated with cbus interfaces
 */
static void
cdb_reset (boolean eoir)
{
    slottype* sp;

    FOR_ALL_ENABLED_IP_SLOTS(sp) {
	int unit;

	for (unit = 0; unit < sp->interface_count; ++unit) {
	    cdbtype* cdb;

	    cdb = controller_find(sp->slot_num, unit);
	    if (cdb) {

		/*
		 * Don't attempt to EOIR recently inserted cards:
		 */
		if (eoir && (! (sp->flags & BOARD_RELOADED))) {
		    cdb->cdb_status |= CDB_EOIR_RESET;
		    if (oir_debug) {
			buginf("\nRequesting eoir reset of %s",
			       cdb->cdb_namestring);
		    }
		} else {
		    if (cbus_debug) {
			buginf("\nResetting %s, eoir=%d, sp->flags=%#x",
			       cdb->cdb_namestring, eoir, sp->flags);
		    }
		}

		cdb->reset(cdb);
		cdb->cdb_status &= ~CDB_EOIR_RESET;	/* Clear it now */
	    }
	}
    }
    return;
}

/*
 * reset all CBUS interfaces
 */
static void
idb_reset (boolean eoir)
{
    hwidbtype* hwidb;

    FOR_ALL_HWIDBS(hwidb) {
	boolean board_reloaded;
	boolean board_eoir;

	if ((hwidb->status & IDB_CBUS) == 0)
	    continue;
	if (hwidb->status & IDB_NOHARDWARE)
	    continue;

	/*
	 * If the card was recently reinstalled, skip this interface
	 */
	if (hwidb->pre_hotstall_state != IDBS_ADMINDOWN)
	    continue;

	TIMER_STOP(hwidb->xmittimer);
	hwidb->tx_restart_active = FALSE;
	TIMER_STOP(hwidb->tx_restart_time);

	/*
	 * Don't EOIR this interface if the slot was (re)loaded!
	 */
	board_reloaded = slots[hwidb->slot].flags & BOARD_RELOADED;
	board_eoir = eoir && (!board_reloaded);

	if (board_eoir)
	    hwidb->oir_flags |= EOIR_RESET;

	if (cbus_debug)
	    buginf("\nResetting %s (flags %x), eoir=%d, board_reloaded=%d, board_eoir=%d",
		   hwidb->hw_namestring, hwidb->oir_flags, eoir,
		   board_reloaded, board_eoir);

	/*
	 * Is the current card a MIP? If so, we have to do yet another
	 * ugly 'except for the MIP' kludge.
	 *
	 * The MIP interface has been reset when its controller
	 * was reset early. So when the controller is unquiesced,
	 * the interface can be ready for serving packets. This
	 * only applies to an EIORing MIP.
	 */
	if ((!board_eoir) || 
	    (slots[hwidb->slot].ctrlr_type != FCI_MIP_CONTROLLER)) {

   	    hwidb->reset(hwidb);

	    if (cbus_debug)
	        buginf("\nReset %s, state %s",
		       hwidb->hw_namestring, print_hwidbstate(hwidb));
	} 
	hwidb->oir_flags &= ~EOIR_RESET;
	if (hwidb->counters.resets > 0)
	    hwidb->counters.resets--;
    }
    return;
}

/*
 * divide MEMD into buffers
 * called after configuration is parsed, after stall & "micro reload"
 */
void rsp_memd_carve (boolean eoir)
{
    carvdata_t cd;
    leveltype level;
    int bufdelta;
    sys_timestamp ttime;

#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"\nrsp_memd_carve starting\n");
#endif
    GET_TIMESTAMP(ttime);
    ++memd_recarves;
    lost_memd_buffers = 0;

    bufdelta = CBUSBUFDELTA_START;
    for (;;) {
	memset(&cd, 0, sizeof(cd));
	discover_bufsizes(&cd, bufdelta);
	if (cd.num_bp <= MAX_RX_BUFFER_POOLS)
	    break;
	bufdelta *= 2;
    }

    cd.bufhdrs_left = bufhdr_count;
    cd.bufdata_ptr = &memd_base[MEMD_BUFDATA_OFFSET] + permanently_allocated;
    cd.bufdata_end = &memd_base[memd_size];

    per_ip_requirements(&cd);

    permanently_allocated = cd.bufdata_ptr - &memd_base[MEMD_BUFDATA_OFFSET];

    define_nonrx_pools(&cd);

    /* Be tolerant of future expansion (extra pools) as opposed to   */
    /* corrupting stack variables if miscalculation occurs in num_bp */
    if (cd.num_bp > (MAX_BUFFER_POOLS - EXTRA_BUFFER_POOLS)) {
        errmsg(&msgsym(NOBUF, CBUS), "too many buffer pools");
        if (cd.num_bp > MAX_BUFFER_POOLS) {
            crashdump(1);
        }
    }
 
    sort_by_bufsize(&cd);

    alloc_free_queues(&cd);

    alloc_nonrx_pools(&cd);
    alloc_rx_pools(&cd);

    alloc_leftover_buffers(&cd);

    write_bufpool(&cd, bufpool);

    init_input_queues(&cd);

    memd_carve_in_progress = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    invoke_ctrlr_init();
    invoke_if_init(&cd);
    reg_invoke_invoke_ipc_init();

    reset_interrupt_level(level);

    cdb_reset(eoir);
    idb_reset(eoir);

    /* Clear all BOARD_RELOADED flags */
    {
	slottype* sp;

	FOR_ALL_ENABLED_SLOTS(sp) {
	    sp->flags &= ~BOARD_RELOADED;
	}
    }

    cbus_being_reset = FALSE;

    memd_unused = cd.memd_unused + (uint) (cd.bufdata_end - cd.bufdata_ptr);

    /*
     * Send the RSP's notion of hw_if_index to all VIPs.
     */
    reg_invoke_send_hwindex_to_vips(NULL);

    reg_add_onesec(dbus_error_scan, "dbus_error_scan");

    /* Send CCB_CMD_VIP_GET_CONFIG to VIPs */
    {
	slottype* sp;

	FOR_ALL_ENABLED_SLOTS(sp) {
	    if (vip_controller(sp->ctrlr_type)) {
		send_ip_cmd(sp, CCB_CMD_VIP_GET_CONFIG);
	    }
	}
    }

#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"rsp_memd_carve took %d\n",ELAPSED_TIME(ttime));
#endif
    return;
}

/*
 * invoke non-EOIR recarve after config is parsed
 */
static void
initial_memd_carve (void)
{
    ++initial_carve_calls;
    if (initial_carve_calls == 1) {
	rsp_memd_carve(FALSE /* not EOIR */);
    } else {
	platform_demand_complex_restart(NULL);
    }
    return;
}

/*
 * reset & start an IP after determining that appropriate microcode
 * has been downloaded, and that the hardware is up-to-date
 *
 * caller starts IP with dbus_ctrlr_start depending on return value.
 * return TRUE if this IP should be started, FALSE if no start required
 */
static boolean
start_slot (slottype* slotp)
{

    /*
     * initially mark as not disabled, wedged or unsuitable
     */
    slotp->flags &= ~(BOARD_DISABLED | BOARD_WEDGED | BOARD_UNSUITABLE);

    /*
     * Next, decide whether or not we want to allow this particular
     * card to come out and play.  It is through this that we may
     * perform IP specific hardware version checks etc, to prevent old
     * down-rev cards from coming onto the cbus.
     */
    if (!ctrlr_7500_compat(slotp->ctrlr_type, slotp->hw_version)) {
	slotp->flags |= (BOARD_DISABLED | BOARD_UNSUITABLE);
	++unsuitable_ctrlrs;
	errmsg(&msgsym(BADHWREV, RSP), slotp->card_name, slotp->slot_num,
	       slotp->hw_version >> 8, slotp->hw_version & 0xff);
	return FALSE; /* caller should not start controller */
    }

    /*
     * Disable CyBus ParityValid HW Problem workaround for HIP.
     */
    if (slotp->ctrlr_type == FCI_LINK_CONTROLLER) {
	dgbus_eeprom_t *idprom;
 
	/*
	 * Are we an RSP2 with an old MD ASIC? 
	 * RSP2 boards prior to "A0" had Rev.1 MD ASIC's and need the
	 * CyBus parity disabled when a HIP is present.
	 */
	idprom = (dgbus_eeprom_t*) ADRSPC_IDPROM;
	if ((idprom->eeprom_version == 1) &&
	     (idprom->controller_type == FCI_RSP2_CONTROLLER) &&
	     (idprom->board_revision < 0x50)) {
	    volatile uchar* reg;

	    reg = (uchar*) ADRSPC_CBUS0REG;
	    *reg |= CBUSX_PARITY_DISABLE;
	}
    }

    if (FCI_TYPE_RSP(slotp->ctrlr_type)) {
       if(rsp_slave_state() == SLAVE_RUNNING) 
	   return FALSE; /* caller should not start controller */
     }

    return TRUE; /* caller should start controller */

}

/*
 * find the right command queue for a slot, and send a CCB ptr
 * down to it
 */
static void
assign_cmdq (slottype* slotp)
{
    int qindex;

    qindex = get_hwq(QT_CMD, slotp->slot_num);
    slotp->cmd_q_ptr = queue_ptr(qindex);
    slotp->ccb_ptr = &ccb_base[slotp->slot_num];
    if (slotp->flags & BOARD_CCBNEEDED)
	send_ip_ccb_ptr(slotp);
    return;
}

/*
 * register a new card with the chassis MIB
 */
static void
chassismib_newcard (slottype *sp)
{
    char* cp;
    char* swver;
    char* hwver;
    char versions[48];
    char* card_descr;
    card_type_t card_type;
    boolean this_is_ssip_smip_pslot = FALSE;
    int bay, pa_type, pa_ifcount;
    pa_info_slots *pa_info_ptr;

    this_is_ssip_smip_pslot = ssip_smip_pslot(sp->slot_num);

    card_type_and_descr(sp->ctrlr_type, sp->interface_count,
			&card_type, &card_descr);

    if (this_is_ssip_smip_pslot) {
	switch (sp->ctrlr_type) {
	  case FCI_FSIP_CONTROLLER:
	    card_type = D_cardType_ssip;
	    card_descr = "Standard Serial";
	    break;

	  case FCI_MIP_CONTROLLER:
	    card_type = D_cardType_smip;
	    card_descr = "Service Provider Multichannel";
	    break;

	  default:
	    break;
	}
    }

    cp = versions;
    cp += sprintf(hwver = cp, "%u.%u",
		  sp->hw_version >> 8, sp->hw_version & 0xFF);
    cp += sprintf(swver = ++cp, "%u.%u",
		  sp->sw_version >> 8, sp->sw_version & 0xFF);

    switch (card_type) {
      case D_cardType_vip:
      case D_cardType_vip2: 
	/* Add VIP cardTable entry, including number of bay slots on VIP */
        reg_invoke_chassismib_add_sub_card_entry(card_type, sp, card_descr, 
						 sp->serial_number, hwver, 
						 swver, sp->slot_num, 0, 2);
	/* And AFTER ADDING VIP, add port adapter cardTable entries */
    	for (bay = 0; bay < SLOT_NUM_VIP_PAS ; bay++) {
	    pa_info_ptr = sp->pa_info[bay];
	    if (pa_info_ptr) {
		pa_type = pa_info_ptr->pa_type;
		pa_ifcount = pa_info_ptr->pa_ifcount;

		switch (pa_type) {
		  case FCI_TYPE_VIP_ETHER8:
		  case FCI_TYPE_VIP_FETHER:
		  case FCI_TYPE_VIP_FETHER_TX_NISL:
		  case FCI_TYPE_VIP_FETHER_TX_ISL:
		  case FCI_TYPE_VIP_FETHER_FX_ISL:
		  case FCI_TYPE_VIP_SERIAL4:
		  case FCI_TYPE_VIP_TRING4:
		  case FCI_TYPE_VIP_FDDI:
		  case FCI_TYPE_VIP_ESA:
		    sub_card_type_and_descr(pa_type, pa_ifcount,
					    &card_type, &card_descr);
		    /* pa_info needed in call to add_sub_card */
		    reg_invoke_chassismib_add_sub_card_entry(card_type, sp, 
				  card_descr, 0, "", "", bay,
				  reg_invoke_chassismib_find_slot_card_index(
                                                                 sp->slot_num),
				  0);
		    break;
		  default:
		    break;
		}
	    }
      	} 
	break;
      case D_cardType_feip:
      for (bay = 0; bay < 2 ; bay++) {
          pa_info_ptr = sp->pa_info[bay];
          if (pa_info_ptr) {
              switch (pa_info_ptr->controller_type) {
                  /*
                   * Initial TX FEIP PA's were shipped without being
                   * programmed (0xff) so recognize them as TX PA's.
                   */
              case PA_HARDWARE_UNKNOWN:
              case PA_HARDWARE_1FE:
              case PA_HARDWARE_1FE_TX_ISL:
                  card_type = D_cardType_feip;
                  /* card_descr already setup in card_type_and_descr() */
                  break;
              case PA_HARDWARE_1FE_TX_NISL:
                  card_type = D_cardType_feip_tx;
                  /* card_descr already setup in card_type_and_descr() */
                  break;
              case PA_HARDWARE_1FE_FX_ISL:
              case PA_HARDWARE_1FE_FX_NISL:
                  card_type = D_cardType_feip_fx;
                  /* card_descr already setup in card_type_and_descr() */
                  break;
              default:
                  card_type = D_cardType_unknown;
                  break;
              }
              break;
          }
      }
      /*
       * Fall through to default now
       */

      default:
	/* Add non-vip cards to cardTable normally */
        reg_invoke_chassismib_add_card_entry(card_type, sp, card_descr,
			sp->serial_number, hwver, swver, sp->slot_num);
	break;
    }
    return;
}

/*
 * returns 4K or 8K, depending which page size the IP has selected
 */
static uint
ip_pagesize (slottype* sp)
{
    uint slot_num;
    volatile uchar* reg;

    slot_num = sp->slot_num;
    if (slot_num >= 8) {
	reg = (uchar*) ADRSPC_PAGESIZE8_15;
	slot_num -= 8;
    } else {
	reg = (uchar*) ADRSPC_PAGESIZE0_7;
    }
    if ((*reg & (0x01 << slot_num)) == (PAGESIZE_4K << slot_num))
	return 4096;
    return 8192;
}

/*
 * attempt to analyze a single controller
 */
static void
ctrlr_analyze (slottype* slotp)
{
    ushort status;
    leveltype level;
    
    if (slotp->ccb_ptr == NULL)
	return;

    if (FCI_TYPE_RSP(slotp->ctrlr_type)) {
	slotp->flags |= BOARD_ANALYZED;
	return;
    }
    
    if (slotp->flags & BOARD_CCBNEEDED) {
	slotp->flags &= ~BOARD_CCBNEEDED;
	status = get_ip_ccb_status(slotp);
	if (status != FCI_RSP_OKAY) {
	    disable_ip(slotp);
	    return;
	}
    }

    set_qa_ip_attention(get_queue_number(slotp->cmd_q_ptr), slotp->slot_num);
    
    level = raise_interrupt_level(NETS_DISABLE); 
   
    if (send_version_cmd(slotp, CCB_VERSION_SW,
			 &slotp->sw_version) != CMD_RSP_OKAY) {
	disable_ip(slotp);
	reset_interrupt_level(level);
	return;
    }
    
    reset_interrupt_level(level);

    slot_enabled_led_on(slotp->slot_num);

    /* XXX invoke dci_memtest() equivalent here */

    if ((slotp->flags & BOARD_ANALYZED) == 0) {

	/* Prepare to acquire IDBs. */
	slotp->flags &= ~BOARD_IDBSREMOVED;
	slotp->interface_count = 0;

	reg_invoke_ctrlr_analyze(slotp->ctrlr_type, slotp);
	if (slotp->flags & BOARD_DISABLED) {
	    disable_ip(slotp);
	} else {
	    slotp->flags |= BOARD_ANALYZED;
	}
    } else {
	/* Recover the previous IDBs. */
	restore_idbs(slotp);
    }

    return;
}

/*
 * clear various data associated with MEMD state
 */
static void
clear_global_qa_data (void)
{
    int ix;

    RawQ = NULL;
    ReturnQ = NULL;
    EventQ = NULL;
    BufhdrQ = NULL;
    LovltrQ = NULL;
    IpcbufQ = NULL;

    bufhdrq_elements = 0;
    lovltrq_elements = 0;
    lovltrq_bufsize = 0;
    ipcbufq_elements = 0;
    ipcbufq_bufsize = 0;

    for (ix = 0; ix < RSP_MAX_LOCAL_FREE_QUEUES; ++ix)
	local_queue_to_hw_idb[ix] = nullidb->hwptr;

    return;
}

/*
 * disable cache modes inconsistent with traffic shaping on RSP
 */
static void rsp_shaping_support (hwidbtype *hwidb)
{
    if (hwidb->firstsw) {
	hwidb->ip_routecache &= ~IP_DISTRIBUTED_ROUTECACHE;
	reg_invoke_fast_setup(hwidb);
    }
}


/*
 * called after dbus subsystem has downloaded some microcode,
 * but not necessarily the final microcode (because when firsttime
 * is set the config hasn't been parsed yet).
 */
int cbus_analyze (boolean firsttime, 
		  boolean eoir_cycle)
{
    slottype *slotp;
    leveltype level;
    sys_timestamp etime, stime, ttime;
    ushort indx;
    ushort card_types[MAX_SLOTS];
    ushort card_index[MAX_SLOTS];
    ushort card_count = 0;
    dgtype* dgbus;
    volatile uchar *netmask;
    
#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"Starting cbus_analyze\n");
#endif
    GET_TIMESTAMP(ttime);
    /*
     * Prevent interrupts while we are resetting the QA ASIC
     */
    if (firsttime) {
	reg_add_interface_cfg_modified(set_cbus_txlimit, "set_cbus_txlimit");
	reg_add_shape_cache(rsp_shaping_support, "rsp_shaping_support");
    }

    level = raise_interrupt_level(NETS_DISABLE);

    clear_cbus_info();

    /*
     * Set this before calling the IP's controller analyze routines
     * so that deeper routines that they may call, can determine
     * the state of the machine.  In particular, this field may be
     * used to determine if hwidb's have valid buffer information
     * in them yet (for example, pool_index, dodgey, or what?).
     * This global is the RSP equivalent of cbus->configuring, on the 
     * c7000.
     */
    memd_carve_in_progress = TRUE;
    cbus_being_reset = TRUE;
    reg_delete_onesec(dbus_error_scan);

    /*
     * Each high end  platform needs to initialise this global,
     * so that common code can reference it without need for
     * checking cbus->flags etc.  Note, that while the following
     * line looks very portable, suggesting that it could be moved
     * to a more common location, this line is not appropriate on
     * CBUS-1 machines (see also cbus_analyze in if_ags_c7000.c)
     */
    cbus_max_ctrlr_interfaces = MAX_CTRLR_INTERFACES;

    controller_init(firsttime, MAX_SLOTS, MAX_CTRLR_INTERFACES);

    reg_invoke_ipc_cbus_init();

    disable_cbus_grant();

    qa_mgr_init();

    rsp_qa_init();			/* Put QA ASIC in known good state */

    /*
     * This restart may have been caused by the MEMD bad parity
     * condition so enable the QA network and command masks.
     * The MEMD bad parity condition disables them in the level 6
     * interrupt handler.  It is now time to enable them.
     * Throttling code could have been running when the reset was
     * was initiated. This is another reason to enable the masks.
     * Also, reset the flag indicating a MEMD bad parity error
     * since the restart complex is in process.
     */

    netmask = (uchar*) ADRSPC_QA_NETINT_MASK;
    *netmask = QA_INT_ENABLE;
    netmask = (uchar*) ADRSPC_QA_CMDINT_MASK;
    *netmask = QA_INT_ENABLE;
    memd_parity_error = FALSE;

    clear_global_qa_data();

    reset_interrupt_level(level);

    enable_cbus_grant();

    unsuitable_ctrlrs = 0;

    GET_TIMESTAMP(stime);
    FOR_ALL_NONEMPTY_SLOTS(slotp) {
#ifdef DEBUG_PARALLEL
	ttyprintf(CONTTY,"Looking at non empty slot %d\n",slotp->slot_num);
#endif
	reg_invoke_chassismib_remove_card_entry(slotp);
	if (slotp->ucode_source == UCODE_NONE) {
	    if (slotp->flags & BOARD_ANALYZED) {
		slotp->flags &= ~BOARD_ANALYZED;
		remove_idbs(slotp);
	    }
	    continue;
	}
	if (eoir_cycle) {
	    if ((slotp->flags & BOARD_CCBNEEDED) == 0)
		continue;
	} else {
	    slotp->flags |= BOARD_CCBNEEDED;
	}

	if (start_slot(slotp)) {
	    GET_TIMESTAMP(etime);
	    dgbus = pslot_to_dbus (slotp->slot_num);
	    card_index[card_count] = slotp->slot_num;
	    card_types[card_count] = slotp->ctrlr_type;
	    dbus_ctrlr_start_nowaited (dgbus, card_types[card_count]);
#ifdef DEBUG_PARALLEL
	    ttyprintf(CONTTY,"Started card %d type %d slot %d took %d\n",
		    card_count, card_types[card_count],
		    slotp->slot_num,ELAPSED_TIME(etime));
#endif
	    card_count++;
	} else {
#ifdef DEBUG_PARALLEL
	    ttyprintf(CONTTY,"slot %d returned FALSE (do not start)\n",slotp->slot_num);
#endif
	}
    }
#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"Total start time %d\n",ELAPSED_TIME(stime));
#endif

    /* wait for controllers started above */
    GET_TIMESTAMP(stime);
    for (indx = 0; indx < card_count; indx++) {
	GET_TIMESTAMP(etime);
	dgbus = pslot_to_dbus(card_index[indx]);
	dbus_ctrlr_start_nowaited_finish (dgbus, card_types[indx]);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY,"Waiting for card %d slot %d type %d took %d\n",
		 indx,card_index[indx],card_types[indx],ELAPSED_TIME(etime));
#endif
    }
#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY, "Total waits took %d\n",ELAPSED_TIME(stime));
#endif

    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	assign_cmdq(slotp);
    }

    DELAY(FCI_SOFT_RESET_TIME);

    FOR_ALL_NONEMPTY_SLOTS(slotp) {
	/* 
	 * If it's not disabled, go ahead and analyse it.
	 */
	if (!(slotp->flags & BOARD_DISABLED)) {
	    ctrlr_analyze(slotp);
	    chassismib_newcard(slotp);
	}
    }

    reg_invoke_ipc_ctrlr_analyze();

#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"cbus_analyze took %d\n",ELAPSED_TIME(ttime));
#endif
    return 1;
}

int cbus_slotunit2unit (int slot, int slotunit)
{
    return(slot * MAX_CTRLR_INTERFACES + slotunit);
}


boolean cbus_unit2slotunit (int unit, int *slot, int *slotunit)
{
    if (unit < 0 || unit >= MAX_CBUS_INTERFACES)
	return FALSE;

    *slot     = unit / MAX_CTRLR_INTERFACES;
    *slotunit = unit % MAX_CTRLR_INTERFACES;
    return TRUE;
}

/*
 * display count of UNSUITABLE controllers in the system
 */
static void
unsuitable_print_controllers (void)
{
    if (unsuitable_ctrlrs) 
	printf("%u controller%s requiring %s.\n",
	       ArgAndPlural(unsuitable_ctrlrs,"","s"),
	       "HARDWARE UPGRADE");
    return;
}

static ushort
cybus_hw_version (void* bus, uint indx)
{
    return dbus_hw_version((dgtype*) bus);
}

void cbus_platform_registry_init (void)
{
    memd_recarves = 0;
    permanently_allocated = 0;
    initial_carve_calls = 0;
    
    reg_add_assign_mac_addr(rsp_assign_mac_addr, "rsp_assign_mac_addr");
    reg_add_slot_enable_led_on(slot_enabled_led_on, "slot_enabled_led_on");
    reg_add_disable_slot(disable_slot, "disable_slot");
    reg_add_slot_enabled(slot_enabled, "slot_enabled");
    reg_add_get_hw_version(cybus_hw_version, "cybus_hw_version");

    reg_add_default_ctrlr_analyze(default_ctrlr_analyze,
				  "default_ctrlr_analyze");
    reg_add_default_ctrlr_init(default_ctrlr_init, "default_ctrlr_init");
    reg_add_default_if_init(default_if_init, "default_if_init");

    reg_add_system_configured(initial_memd_carve, "initial_memd_carve");

    reg_add_print_controllers(unsuitable_print_controllers,
			      "unsuitable_print_controllers");

    reg_add_bad_nvram_if(initial_memd_carve, "initial_memd_carve");

    cbus_debug_init();

    return;
}

/*
 * get_memd_buffer
 *
 * Search through all of the free queues looking for the first memd buffer
 * large enough to hold this output packet.
 *
 * We also have a small private reserve of buffers to fall back on
 * when all else fails. IPs are supposed to leave this 'private' queue
 * of MEMD buffers alone.
 *
 * We expect the free queues in bufpool[] to be sorted in ascending size order.
 * We also expect the process level private free queue to be after all
 * shareable free lists.
 */

bufhdr_t *get_memd_buffer (int size)
{
    int		ix;
    bufhdr_t	*bufhdr;
    qa_hwq_t	*qptr;

    for (ix = 0; ix < MAX_BUFFER_POOLS; ix++) {
	if (bufpool[ix].size == BUFPOOL_BADSIZE)
	    break;
	if (size <= bufpool[ix].real_size) {
	    qptr = bufpool[ix].qptr;
	    bufhdr = hwq_dequeue(qptr);
	    if (bufhdr) {
		/*
		 * Uses uncached buffer address, since we'll only read
		 * one field in the header (bf_buffer_ptr), which is cheaper
		 * than invalidating it and pulling in a whole cache line.
		 */
		bufhdr = ADDR_MEMD_TO_UNCACHED(bufhdr);

		/*
		 * Set up to have buffer returned to global free queue
		 * when done.
		 */
		bufhdr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(qptr);
		
		/*
		 * Also save global free queue address in case we later
		 * decide to have the buffer post processed on the return
		 * queue.
		 */
		bufhdr->bf_freeq_ptr = ADDR_TO_MEMD_OFFSET_16(qptr);

		return(bufhdr);
	    }
	}
    }
    return(NULL);
}

bufhdr_t *get_ipc_memd_buffer (ulong ipc_control)
{
    bufhdr_t *bufhdr;
    qa_hwq_t *qptr;
    
    if (ipc_control & IPC_CARD_PERMANENT_PORT) { 
        qptr = IpcbufQ;
    } else { 
        qptr = IpcbufQ_classic; /* dual rsp and  CIP ipc */
    }
    /*
     * IpcbufQ or IpcbufQ_classic may not be setup yet, try it later
     */
    if (!qptr)
	return(NULL);
    bufhdr = hwq_dequeue(qptr);
    if (bufhdr) {
	/*
	 * Uses uncached buffer address, since we'll only read
	 * one field in the header (bf_buffer_ptr), which is cheaper
	 * than invalidating it and pulling in a whole cache line.
	 */
	bufhdr = ADDR_MEMD_TO_UNCACHED(bufhdr);

	/*
	 * Set up to have buffer returned to global free queue
	 * when done.
	 */
	bufhdr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(qptr);
		
	/*
	 * Also save global free queue address in case we later
	 * decide to have the buffer post processed on the return
	 * queue.
	 */
	bufhdr->bf_freeq_ptr = ADDR_TO_MEMD_OFFSET_16(qptr);
    } else {
	bufhdr = get_memd_buffer(RSP_MAX_IPC_BUFF_SIZE);
    }
    return(bufhdr);
}

/*
 * handles the "show controller cbus" command
 */
void
show_cbus (parseinfo *csb)
{
    slottype* sp;
    bufpool_t* bp;

    /*
     * If we got a hotstall interrupt, wait until the process level
     * hotswap code gets to run and updates our view of reality
     */
    while (hotstall_flag)
	process_suspend();

    automore_enable(NULL);
    printf("\nMEMD at %x, %u bytes (unused %u, recarves %u, lost %u)",
	   (uint) memd_base, memd_size, memd_unused, memd_recarves,
	   lost_memd_buffers);
    printf("\n  RawQ %x, ReturnQ %x, EventQ %x", 
	   (uint) RawQ, (uint) ReturnQ, (uint) EventQ);
    printf("\n  BufhdrQ %x (%u items)", (uint) BufhdrQ, bufhdrq_elements);
    if (LovltrQ)
	printf(", LovltrQ %x (%u items, %u bytes)",
	       (uint) LovltrQ, lovltrq_elements, lovltrq_bufsize);
    if (IpcbufQ)
	printf("\n  IpcbufQ %x (%u items, %u bytes)",
	       (uint) IpcbufQ, ipcbufq_elements, ipcbufq_bufsize);
    if (IpcbufQ_classic)
	printf("\n  IpcbufQ_classic %x (%u items, %u bytes)",
	       (uint) IpcbufQ_classic, ipcbufq_elements_classic, ipcbufq_bufsize_classic);
    printf("\n  %u buffer headers (%x - %x)", bufhdr_count,
	   (uint) bufhdr_base, (uint) &bufhdr_base[bufhdr_count - 1]);
    for (bp = bufpool;
	 bp < &bufpool[sizeof(bufpool)/sizeof(bufpool[0])];
	 ++bp) {
	if (bp->size == BUFPOOL_BADSIZE)
	    break;
	printf("\n  pool%u: %u buffers, %u bytes, queue %x",
	       bp - bufpool, bp->nb, bp->size, (uint) bp->qptr);
    }
    FOR_ALL_NONEMPTY_SLOTS(sp) {
	int ix;
	boolean this_is_ssip_smip_pslot = FALSE;

	printf("\n  slot%u: ", sp->slot_num);

        this_is_ssip_smip_pslot = ssip_smip_pslot(sp->slot_num);
	if (this_is_ssip_smip_pslot) {
	    switch (sp->ctrlr_type) {
		case FCI_FSIP_CONTROLLER:
		  printf("SSIP");
		  break;

		case FCI_MIP_CONTROLLER:
		  printf("SMIP");
		  break;
	    
		default:
		  break;
	      }
	    printf(", hw %u.%u", sp->hw_version >> 8, sp->hw_version & 0xff);
	}
	else 
	    printf("%s, hw %u.%u", sp->card_name, sp->hw_version >> 8, 
		   sp->hw_version & 0xff);

	if (sp->flags & BOARD_UNSUITABLE) {
	    printf("\n    Requires %s.", "HARDWARE UPGRADE");
	    continue;
	}

	printf(", sw %u.%02u, ccb %x, cmdq %x, vps %u"
	       "\n    software loaded from %s %s",
	       sp->sw_version >> 8, sp->sw_version & 0xff,
	       (uint) sp->ccb_ptr, (uint) sp->cmd_q_ptr, ip_pagesize(sp),
	       get_ucode_src(sp->slot_num), get_ucode_src_file(sp->slot_num));

	reg_invoke_show_controller_specific(sp->ctrlr_type, sp->slot_num);

	for (ix = 0; ix < sp->interface_count; ++ix) {
	    hwidbtype *hwidb, *vhwidb;

	    hwidb = sp->interface_list[ix];
	    if (hwidb == NULL)
		continue;
	    mem_lock(hwidb);
	    if (hwidb->slotunit == 0 || 
		hwidb->slotunit == PA_VIRTUAL_SLOT_BOUNDARY)
		reg_invoke_show_pa_specific(hwidb->type, hwidb, sp->ctrlr_type);
	    printf("\n    %s", hwidb->hw_namestring);
	    if (hwidb->status & IDB_IEEEMASK)
		printf(", addr %e (bia %e)", hwidb->hardware, hwidb->bia);
	    if (hwidb->applique_name)
		printf(", applique is %s", hwidb->applique_name);
	    printf(
	"\n      gfreeq %x, lfreeq %x (%u bytes)"
	"\n      rxlo %u, rxhi %u, rxcurr %u, maxrxcurr %u",
		   (uint) hwidb->global_free_q_ptr,
		     (uint) hwidb->local_free_q_ptr,
		     hwidb->global_free_q_size,
		   hwidb->rx_limit_lo, hwidb->rx_limit_hi,
		   hwidb->rx_currently_owned, hwidb->max_rx_currently_owned);
/* No accmulator is allocated for the MIP plughole's hwidb. 
 * The accumulator is allocated per channel basis. All channels
 * share a common tx queue.
 */
	    if (hwidb->tx_acc_ptr)
	        printf(
	"\n      txq %x, txacc %x (value %u), txlimit %u",
		     (uint) hwidb->tx_q_ptr, (uint) hwidb->tx_acc_ptr,
		     hwacc_peek(hwidb->tx_acc_ptr),
		       calculate_cbus_txlimit(hwidb));


	    if (hwidb->xmitdelay)
		printf(", txdelay %u %ss", hwidb->xmitdelay,
		     ((hwidb->type == IDBTYPE_HSSI) || 
		      (hwidb->type == IDBTYPE_CBUSSERIAL)) ? "flag" : "usec");

	    vhwidb = hwidb->next_idb_on_physint;
	    while (vhwidb) {
		if (vhwidb->tx_acc_ptr)
		  printf(
	 "\n      %s, txq %x, txacc %x (value %u), txlimit %u",
		     vhwidb->hw_namestring,
		     (uint) vhwidb->tx_q_ptr, (uint) vhwidb->tx_acc_ptr,
		     hwacc_peek(vhwidb->tx_acc_ptr),
		     calculate_cbus_txlimit(vhwidb));
		vhwidb = vhwidb->next_idb_on_physint;
	    }

	    free(hwidb);
	}
    }
    automore_disable();
    return;
}


/* rsp_print_global_queues
 * 
 * print the addresses and lengths of all the global queues, including
 * all the per-buffer-pool free queues.  Use errvarmsg() because this is 
 * unsolicited output.
 */
void rsp_print_global_queues (void)
{
    bufpool_t* bp;

    errvarmsg(&msgsym(DIAG, QA), "Global queues:");

    errvarmsg(&msgsym(DIAG, QA), "    %u buffer headers", bufhdr_count);

    errvarmsg(&msgsym(DIAG, QA), "    RawQ %x, ReturnQ %x, EventQ %x", 
	      (uint) RawQ, (uint) ReturnQ, (uint) EventQ);

    errvarmsg(&msgsym(DIAG, QA), "    BufhdrQ %x",  (uint) BufhdrQ);

    if (LovltrQ) 
	errvarmsg(&msgsym(DIAG, QA), "    LovltrQ %x", (uint) LovltrQ);

    if (IpcbufQ) 
	errvarmsg(&msgsym(DIAG, QA), "    IpcbufQ %x", (uint) IpcbufQ);

    if (IpcbufQ_classic) 
	errvarmsg(&msgsym(DIAG, QA), "    IpcbufQ_classic %x", 
		  (uint) IpcbufQ_classic);

    for (bp = bufpool; bp < &bufpool[sizeof(bufpool)/sizeof(bufpool[0])];
	 ++bp) {
	if (bp->size == BUFPOOL_BADSIZE)
	    break;
	errvarmsg(&msgsym(DIAG, QA), 
		  "    Pool%u: %u buffers, %u bytes, queue %x",
		  bp - bufpool, bp->nb, bp->size, (uint) bp->qptr);
    }
    
    return;
}

/* rsp_print_if_queues
 * 
 * for each slot and interface, print the addresses and names of all the
 * per-interface queues.  Note that since this function is designed to be
 * called at interrupt level 6, it cannot lock the hardware IDB, so it's not
 * suitable for printing queue statistics etc which might change.  It uses
 * errvarmsg(), not buginf() because this is unsolicited diagnostic output.
 */
void rsp_print_if_queues (void)
{
    slottype* sp;

    FOR_ALL_NONEMPTY_SLOTS(sp) {
	int ix;

	errvarmsg(&msgsym(DIAG, QA), "Slot%u:", sp->slot_num);

	for (ix = 0; ix < sp->interface_count; ++ix) {
 	    hwidbtype *hwidb;

	    hwidb = sp->interface_list[ix];
	    if (hwidb == NULL)
		continue;
	    errvarmsg(&msgsym(DIAG, QA), 
		      "    %s: gfreeq %x, lfreeq %x (%u bytes)",
		      hwidb->hw_namestring,
		      (uint) hwidb->global_free_q_ptr,
		      (uint) hwidb->local_free_q_ptr,
		      hwidb->global_free_q_size);
	    errvarmsg(&msgsym(DIAG, QA),"        txq %x", 
		      (uint) hwidb->tx_q_ptr);
	}
    }
    return;
}

/*
 * verify that a given ucode rev is right for a certain card
 */

boolean
ucode_version_ck (ctrlr_type_t ctrlr, uint major, uint minor)
{
    if (((major >= 200) && (major <= 209)) || 	/* beta versions */
	((major >= 20)  && (major <= 29)))	/* production versions */
	return TRUE;
    return FALSE;
}

/*
 * platform_cbus_reset_in_progress
 *
 * Determine whether or not we're in the middle of resetting
 * the cbus.  On the RSP, this is indicated by the state
 * of the cbus_being_reset global flag.  We can therefore ignore the hwidb
 * passed in (it's only needed in the c7000 implementation
 * of this routine).
 */
boolean platform_cbus_reset_in_progress (hwidbtype *hwidb)
{
    return(cbus_being_reset);
}

#ifdef TEST

/*
 * Test code to verify our error logging code for these
 * very hard to reproduce errors
 */

void
rsp_test_cause_xbufhdr (void)
{
    qa_hwq_t	*qptr;
    bufhdr_t	*bufhdr;

    printf("\nCausing an XBUFHDR error...");
    qptr = bufpool[0].qptr;		/* Hardware free queue for pool 0 */
    bufhdr = hwq_dequeue(qptr);
    if (bufhdr) {
	hwq_enqueue(RawQ, bufhdr);
    }
}

extern queuetype	free_retdatQ;

void
rsp_test_cause_invrtn_type (void)
{
    qa_hwq_t		*qptr;
    bufhdr_t		*bufhdr;
    return_data_t	*retd;
    ushort		bcast_id;

    printf("\nCausing an INVRTN(type) error...");
    qptr = bufpool[0].qptr;		/* Hardware free queue for pool 0 */
    bufhdr = hwq_dequeue(qptr);
    if (bufhdr) {

	retd = dequeue(&free_retdatQ);	/* Get free return data struct */
	if (retd) {

	    /*
	     * Save our broadcast ID in the buffer header,
	     * and get our return queue data struct
	     */
	    bcast_id = retd->id;

	    /*
	     * save original q ptr and bufhdr in local struct
	     */
	    retd->queue_ptr  = bufhdr->bf_queue_ptr;
	    retd->bufhdr_offset = ADDR_TO_MEMD_OFFSET_16(bufhdr);

	    /*
	     * put return id into bufhdr (and original free q ptr - not needed
	     *   but may help in debugging dump)
	     *
	     * change bufhdr queue ptr to return queue
	     */
	    bufhdr->bf_return_info = ((bufhdr->bf_queue_ptr << 16) | bcast_id);
	    bufhdr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(ReturnQ);
  
	    retd->type = RETQ_INVALID;	/* CAUSE ERROR! */
	    retd->hwidb = NULL;

	}
	hwq_enqueue(ReturnQ, bufhdr);
    }
}

void
rsp_test_cause_invrtn_hdr (void)
{
    qa_hwq_t		*qptr;
    bufhdr_t		*bufhdr;
    return_data_t	*retd;
    ushort		bcast_id;

    printf("\nCausing an INVRTN(buffer header) error...");
    qptr = bufpool[0].qptr;		/* Hardware free queue for pool 0 */
    bufhdr = hwq_dequeue(qptr);
    if (bufhdr) {

	retd = dequeue(&free_retdatQ);	/* Get free return data struct */
	if (retd) {

	    /*
	     * Save our broadcast ID in the buffer header,
	     * and get our return queue data struct
	     */
	    bcast_id = retd->id;

	    /*
	     * save original q ptr and bufhdr in local struct
	     */
	    retd->queue_ptr  = bufhdr->bf_queue_ptr;
	    retd->bufhdr_offset = 0;

	    /*
	     * put return id into bufhdr (and original free q ptr - not needed
	     *   but may help in debugging dump)
	     *
	     * change bufhdr queue ptr to return queue
	     */
	    bufhdr->bf_return_info = ((bufhdr->bf_queue_ptr << 16) | bcast_id);
	    bufhdr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(ReturnQ);
  
	    retd->type = RETQ_TX_RESTART;
	    retd->hwidb = NULL;

	}
	hwq_enqueue(ReturnQ, bufhdr);
    }
}

void
rsp_test_cause_invrtn_bcastid (void)
{
    qa_hwq_t		*qptr;
    bufhdr_t		*bufhdr;
 
    printf("\nCausing an INVRTNBCASTID error...");
    qptr = bufpool[0].qptr;		/* Hardware free queue for pool 0 */
    bufhdr = hwq_dequeue(qptr);
    if (bufhdr) {

	bufhdr->bf_return_info = ((bufhdr->bf_queue_ptr << 16) | 0x7fff);
	bufhdr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(ReturnQ);

	hwq_enqueue(ReturnQ, bufhdr);
    }
}
#endif /* TEST */

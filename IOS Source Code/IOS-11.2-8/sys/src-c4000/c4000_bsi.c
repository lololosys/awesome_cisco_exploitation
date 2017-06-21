/* $Id: c4000_bsi.c,v 3.14.12.15 1996/09/13 20:54:01 hou Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_bsi.c,v $
 *------------------------------------------------------------------
 * C4000 BSI FDDI device driver
 *
 * September 1992, Chris Shaker, Scott Mackie
 * November 1993, Lele Nardin
 * March 1994, Walter R. Friedrich
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_bsi.c,v $
 * Revision 3.14.12.15  1996/09/13  20:54:01  hou
 * CSCdi68923:  OSPF adjacency cannot be formed using 802.10
 * Branch: California_branch
 *
 * Revision 3.14.12.14  1996/08/28  13:12:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.12.13  1996/07/23  22:40:59  vdukki
 * CSCdi44437:  FDDI driver causes HSRP packets to be process-switched
 * Branch: California_branch
 * Updated driver to call fast switching function while processing HSRP
 * packets.
 *
 * Revision 3.14.12.12  1996/07/23  18:48:11  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.14.12.11  1996/07/20  03:20:29  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.14.12.10  1996/07/11  14:21:10  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.14.12.9  1996/07/09  06:04:31  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.14.12.8  1996/06/28  00:08:51  nrvankay
 * CSCdi59723:  CSCdi44246 did not take care of PSP reuse error corner
 * cases
 * Branch: California_branch
 * When checking for psp reuse errors make sure that ptr is not going to
 * pass over limit
 *
 * Revision 3.14.12.7  1996/06/18  20:05:50  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.14.12.6  1996/05/29  05:31:05  wilber
 * CSCdi58733:  FDDI / NETBIOS bridging of SABME fddi/serial broken
 * Didn't initialize span_encapsize correctly for fddi
 * Branch: California_branch
 *
 * Revision 3.14.12.5  1996/05/17  11:54:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.14.2.3  1996/05/05  23:53:03  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.14.2.2  1996/04/26  15:24:09  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.1  1996/04/03  21:41:55  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.14.12.4  1996/04/17  13:47:49  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.14.12.3  1996/04/04  09:38:11  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.14.12.2  1996/03/21  23:49:01  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.14.12.1  1996/03/18  22:00:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/03/18  23:34:05  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.4  1996/03/03  08:40:27  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.3.2.3  1996/02/08  09:28:33  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.2  1996/01/16  02:34:32  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:23:49  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.14  1996/02/12  18:18:42  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.13  1996/02/06  05:44:15  vdukki
 * CSCdi42429:  fddi no shutdown command reboots router
 * Removed redundant call to les_fddi_cam_populate_inline() from
 * bsi_initialize() function.
 *
 * Revision 3.12  1996/02/02  21:03:15  nrvankay
 * CSCdi44246:  FDDI channel one doesnt receive any packets after sometime
 * Under heavy traffic, sometimes the BSI chip reuses the same PSP
 * buffer or skips at most 2 PSP buffers. A fix had been added to
 * check for the skipped buffers.
 *
 * Revision 3.11.12.1  1996/01/24  23:02:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.11  1996/01/06  03:23:17  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.10  1996/01/05  10:18:13  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.9  1996/01/05  04:56:53  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.8  1995/12/13  04:23:27  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.7  1995/12/07  22:42:15  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/06  22:16:37  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Forgot one.
 *
 * Revision 3.5  1995/12/02  02:01:06  nrvankay
 * CSCdi38185:  OSPF adjacency lost after a few minutes/hours
 * This one has two fixes. One in the firmware and one in the software.
 * Firmware: Not handling small packets properly
 * Software: When multicast packects are switched the buffers are
 *           not replinshed properly
 *
 * Revision 3.4  1995/11/28  09:01:23  tli
 * CSCdi43618:  Bus error at address 0xFA5005C3, 4500 with FDDI
 *
 * Revision 3.3  1995/11/17  18:45:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:27:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/08/15  23:05:45  wfried
 * CSCdi38123:  BSI bug (Zero IS on discarded frame) needs workaround
 * Ensure that a single PSP page is not used twice in a row on a given
 * receive channel. This allows us to use the PSP location check for
 * error conditions in a safe way.
 *
 * Revision 2.12  1995/08/08  16:48:51  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.11  1995/08/07 05:42:16  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.10  1995/08/04  17:31:56  wfried
 * CSCdi35936:  crash - an unknown failure, PC 0x600CA450[abort]
 * Avoid crashing the box when interface reset can recover from bad
 * descriptor
 *
 * Revision 2.9  1995/07/29  02:35:29  wilber
 * CSCdi37546:  HSRP + Bridging Has Problems when FDDI is reinintialized
 * When HSRP + bridging(Low-end box specific)
 *   Depopulate the corresponding cam entry for the virtual MAC address
 *   of the standby IP address when this router becomes active. Failed
 *   to do will cause the router to not take over the load when it
 *   becomes active since the fddi cam is filtering the traffic(Both dst
 *   and src MAC addresses are in the cam)
 *
 * Revision 2.8  1995/07/18  06:48:41  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.7  1995/06/21 09:00:45  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.6  1995/06/21  03:15:20  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.5  1995/06/20  07:18:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/19  07:17:08  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/13  03:29:27  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.2  1995/06/08  18:44:36  wilber
 * CSCdi34408:  turning on and off bridging causes duplicate ip packets in
 * 4500 FDDI
 * Remember to disable cam_learning(promiscuous mode) before each time
 * trying to trun it on for bridging
 *
 * Revision 2.1  1995/06/07  22:56:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define BSI_DEBUG(a)
#define DEBUG_COUNTERS(a)

#define DEBUG_PKT      		0
#define DEBUG_PKT_FAST 		0

/*
 * ------------------------------------------------------------------------
 * This module contains the device driver for National BSI FDDI
 * interfaces in the C4000 family of cisco routers/bridges.
 * ------------------------------------------------------------------------
 * $EndStuff$
 */

#include "master.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../les/msg_fddi.c"		/* Not a typo, see logger.h */
#include "../les/msg_les_fddi.c"	/* Not a typo, see logger.h */
#include <string.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/parser_defs_fddi.h"
#include "../if/ether.h"
#include "../if/rif_inline.h"
#include "../srt/rsrb.h"
#include "../srt/srb_multiring.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/network.h"
#include "../les/if_les.h"
#include "c4000_bsi.h"
#include "c4000_bsi_public.h"
#include "stacks.h"

#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/mibglbl.h"
#include "../xdi/fbmframe.h"
#include "../xdi/mibproto.h"
#include "../xdi/cspproto.h"
#include "../xdi/fbmproto.h"
#include "../if/if_fddi.h"
#include "../srt/span.h"
#include "../srt/fddisrb.h"
#include "../tbridge/tbridge_public.h"

#include "../les/cam.h"
#include "../os/old_timer_callbacks.h"
#include "../os/timer_generic.h"

#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"
#include "c4000_fddi_public.h"

#include "../if/fddi_inline.h"
#include "../les/cam_inline.h"
#include "../smf/smf_public.h"
#include "../ui/debug.h"
#include "../if/fddi_debug.h"
#include "../if/traffic_shape.h"

extern char bsi_xilinx_version[];
extern int smt_dropped[], smt_passed[];
extern void fddi_smt_debug(paktype *pak, boolean output);

/*
 * Local macros:
 */
#define fddibuginf if (fddi_debug|fddi_cmt_events_debug) buginf
#define bprintf    fddi_cbuginf

/*
 * Globals:
 */
const char msgtxt_fddi_illegal[] = "WARNING: Illegal FDDI Configuration"
    "This platform requires at least 4Mb of I/O memory to support FDDI"
    "Any inserted FDDI cards will be ignored by this system";

static uint     bsi_page_count;         /* Number of FDDI pages allocated */

static paktype *bsi_page_table[BSI_BUFFER_PAGES*MAX_STATION_COUNT];
                                        /* Array of allocated pages */
static pooltype *bsi_buffer_pool;	/* BSI buffer pool */
static ulong	bsi_buffer_offset;      /* Buffer Alignment offset */
static ulong	bsi_bsi_offset;		/* BSI loc offset in pak */
static ulong	bsi_start_offset;	/* Datagram Offset in pak (to fc) */
static boolean	bsi_reduced_memory;
static int	nmac, nphy;

static void *bsi_buffer_create(pooltype *pool, pool_item_type item);
static void bsi_buffer_destroy(pooltype *pool, void *item);

void cam_interrupt (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*rp;
    /* NB--not subinterface ready for bridging! */
    idbtype *swidb = idb->firstsw;
    uchar addr[IEEEBYTES];
    uchar baddr[IEEEBYTES];
	
    spantype *span;
    ulong hash;
    btetype *mac_ptr;

    BSI_DEBUG(buginf("cam_interrupt(): idb %#x\n", idb));
    ds = INSTANCE;
    rp = ds->module_regs;
    span = swidb->span_ptr;

    if ((!swidb->circ_canlearn) || (!span)) {
	rp->csr &= (~CAM_CSR_FIFO_GO & ~CAM_CSR_SA_MISS);
	return;
    }

    *(ushort *)&baddr[0] = rp->usar;
    *(ushort *)&baddr[2] = rp->msar;
    *(ushort *)&baddr[4] = rp->lsar;

    ieee_swap(baddr, addr);

    hash = (addr[MAC_HASH1]) ^ (addr[MAC_HASH2]);
    for (mac_ptr = span->bridge_tbl[hash]; mac_ptr;
	 mac_ptr = mac_ptr->bte_next) {
	if (mac_ptr->bte_mac_addr.sword[0] != *(ushort *)&addr[0]) {
	    continue;
	}
	if (mac_ptr->bte_mac_addr.sword[1] != *(ushort *)&addr[2]) {
	    continue;
	}
	if (mac_ptr->bte_mac_addr.sword[2] != *(ushort *)&addr[4]) {
	    continue;
	}
	break;
    }

    if (mac_ptr) {
	/* 
	 * We already have an entry for this station in the bridging database.
	 */
	if (swidb == mac_ptr->bte_interface) {
	    les_fddi_cam_populate_inline(idb, baddr);
	}
    } else {
	/* No entry for this station in the bridging database */
	mac_ptr = dequeue(&bteQ);
	if (mac_ptr) {
	    bte_count--;
	    mac_ptr->bte_mac_addr.sword[0] = *(ushort *)&addr[0];
	    mac_ptr->bte_mac_addr.sword[1] = *(ushort *)&addr[2];
	    mac_ptr->bte_mac_addr.sword[2] = *(ushort *)&addr[4];
	    mac_ptr->bte_next = (swidb->span_ptr)->bridge_tbl[hash];
	    (swidb->span_ptr)->bridge_tbl[hash] = mac_ptr;
	    mac_ptr->bte_action = BRIDGE_FORWARD;
	    if ((swidb->span_ptr)->acquire == FALSE)
	        mac_ptr->bte_action = BRIDGE_DISCARD;
	    mac_ptr->bte_interface = swidb;
	    mac_ptr->bte_txcount = 0;
	    mac_ptr->bte_prev_rxcount = 0; 
	    mac_ptr->bte_rxcount = 1;
	    
	    if (swidb->span_address_in || swidb->span_address_out)
	    {
	      reg_invoke_bridge_address_check(mac_ptr);
	    }

	    les_fddi_cam_populate_inline(idb, baddr);
	} else {
	    (swidb->span_ptr)->learned_entry_discards++;
	}
    }
    rp->csr &= (~CAM_CSR_FIFO_GO & ~CAM_CSR_SA_MISS);
}


/*
 * Provide an external entry point for process-level
 * invocations of the corresponding inline function.
 */

static void les_fddi_cam_depopulate (hwidbtype *hwidb,
				     uchar *mac_address,
				     hwidbtype *input_hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    les_fddi_cam_depopulate_inline(hwidb, mac_address);
    reset_interrupt_level(level);
}


/*
 * Fixed small delay function.
 */
static void empty_function (void)
{
    volatile int t;
    t = 3;
}

/*
 * hammer on low byte at address r, 
 * but don't write 1s to bits specified in mask
 */
static int hammer (volatile uchar *r, uchar mask)
{
    volatile uchar *cp = r;
    int i,rval = 0;
    for (i = 0x80; i > 0; i = i/2 ) {
	uchar t,v = (i & ~mask);
	*cp = v;
	empty_function();  /* stop to smell the roses */
	if ( (t=*cp) != v ) {  
	    char buf[80];
	    sprintf(buf,"chip failed, %x, wrote %x read %x\n",
		    r,v,t);
	    errmsg(&msgsym(FDDIFAIL, FDDI), "0",
		   buf, "", 0);
	    DELAY(ONESEC/2);
	    return(1);
	}
    }
    *cp = 0;
    return(rval);
}
	

static int regtestplayer (player_t *pr)
{
    int rval = 0;
    pr->mr = 0;

    /* note we do not test mode reg!  don't wanna do that! */
    rval += hammer(&pr->cr,0); 
    rval += hammer(&pr->icmr,0); 
    rval += hammer(&pr->ijtr,0); rval += hammer(&pr->isra,0); 
    rval += hammer(&pr->isrb,0);
    rval += hammer(&pr->rcmra,0); rval += hammer(&pr->rcmrb,0); 
    rval += hammer(&pr->ntr,0x80); 
    rval += hammer(&pr->nptr,0);
    rval += hammer(&pr->str,0x80); 
    rval += hammer(&pr->sptr,0);
    rval += hammer(&pr->letr,0); 
    /* don't test udr */
    rval += hammer(&pr->iccr,0); rval += hammer(&pr->ctscr,0);
    rval += hammer(&pr->rccra,0); rval += hammer(&pr->rccrb,0);
    return(rval);
}

static int regtestbmac (bmac_t *b)
{
    int rval = 0;
    b->mr = 0x80;  /* diagnose mode */
    rval += hammer(&b->option,0); 
    rval += hammer(&b->cmp,0);
    rval += hammer(&b->remr0,0x80);
    rval += hammer(&b->remr1,0x1c);
    rval += hammer(&b->temr0,0);
    rval += hammer(&b->cimr,0x80);
    rval += hammer(&b->comr,0x80);
    rval += hammer(&b->emr,0x9e);
    rval += hammer(&b->imr,0x30);
    b->mr = 0;
    return(rval);
}

static int regtestbsi (bsi_t *b)
{
    int rval = 0;

    /*
     * Attempt to set everything *but* the State
     * Notify Register (STRN).
     */
    b->mr = 0; 
    rval += hammer(&b->pcar,0);
    rval += hammer(&b->mnr,7); 
    rval += hammer(&b->snr,0xf0);
    rval += hammer(&b->nsnr,0);
    rval += hammer(&b->lar,6);
    rval += hammer(&b->ldr,0);
    rval += hammer(&b->rnr,0);
    rval += hammer(&b->r0cr,0);
    rval += hammer(&b->r1cr,0);
    rval += hammer(&b->r0efsr,0);
    rval += hammer(&b->r1efsr,0);
    rval += hammer(&b->inr,0xc0);
    rval += hammer(&b->icr,0x24);
    rval += hammer(&b->cmp,0);
    return(rval);
}


/*
 * bsi_typestring
 *
 * Use the NIM ID type supplied to return a string showing what type
 * of FDDI hardware is installed
 */

static char *bsi_typestring (int type)
{
    switch (type) {
      case XX_FDDI_DUAL:
      case XX_FDDI_DBRIDGE:
	return("DAS FDDI");
      case XX_FDDI_SINGLE:
      case XX_FDDI_SBRIDGE:
	return("SAS FDDI");
      default:
	return("Unknown");
    }
}

/* bsi_dump_packet: 
 * for fddi debug - dump the iso2 header and some data.
 */
static void bsi_dump_packet (char *str, uchar *address, int size)
{
    int i;
    buginf("%s: \n", str);
    buginf("data        : ");

    buginf("%2x dst %e src %e  ", address[0], &address[1], &address[7]);
    address += 13;
    for (i = 0; i < size; i++) {
	buginf("%x ", GETLONG(address));
	address += sizeof(long);   /* bump to next long word */
    }
    
    buginf("\n");
}

/*
 * bsi_linestate
 *
 * Tell whether or not the hardware thinks this interface is up.
 */
static boolean bsi_linestate (hwidbtype *hwidb)
{
    fddisb *fsb;

    fsb = idb_get_hwsb_inline(hwidb, HWIDB_SB_FDDI);
    return(TIMER_RUNNING(fsb->fddi_operational));
}

/*
 * bsi_stop
 * Safely stop the BSI
 */

static void bsi_stop (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;
    int			i;
    leveltype		level;
    BSI_DEBUG(SAVE_CALLER());

    ds = INSTANCE;
    BSI_DEBUG(buginf("bsi_stop(): idb %#x, caller %#x\n", 
		      idb, caller()));
    nim = ds->module_regs;
    level = raise_interrupt_level(NETS_DISABLE);
    ds->bsi_ok = FALSE;

    /*
     * Disable interrupts from stop conditions, stop the BSI
     * DMA interface, and then issue a BSI master reset:
     */
    nim->bsi.stnr &= ~(BSI_STNR_SPSTOP | BSI_STNR_RQSTOP | BSI_STNR_INSTOP);
    nim->bsi.star |= BSI_STAR_SPSTOP | BSI_STAR_RQSTOP | BSI_STAR_INSTOP;
    nim->bsi.mr |= BSI_MR_MRST;
    i = 0;
    while (nim->bsi.mr & BSI_MR_MRST) {
	if (i++ > 10) {
	    break;
	}
    }
    nim->bsi.mr |= BSI_MR_BIGEND;	/* 68k is Big Endian */

    /*
     * Save the BSI's silicon revision level:
     */
    nim->bsi.pcar = 0;			/* not set by reset */
    for (i = 0; i < 4; i++) {
	ds->bsi_rev.d.byte[i] = nim->bsi.mbar;
    }

    reset_interrupt_level(level);
}

/*
 * bsi_restart
 *
 * Restart the FDDI interface. Higher level routine.
 */

static void bsi_restart (hwidbtype *idb)
{
    bsi_instance_t *ds;
    leveltype status;

    SAVE_CALLER();
    
    ds = INSTANCE;
    BSI_DEBUG(buginf("bsi_restart(): idb = 0x%x\n", idb));
    if (fddi_debug|fddi_cmt_events_debug) {
	buginf("\nFDDI(%d): reset from %#x", idb->unit, caller());
	buginf("\nFDDI(%d): txtmr: %#Ta, msclk: %#Tn, qt: %x (%dms)", idb->unit,
	       idb->xmittimer, idb->lastoutput, ELAPSED_TIME(idb->lastoutput));
    }

    /*
     * Housekeeping
     */
    status = raise_interrupt_level(NETS_DISABLE);

    /*
     * Flush packets in hold queues
     */
    holdq_reset(idb);

    /*
     * Attempt to reinit interface:
     */
    (void)bsi_initialize(idb);

    reset_interrupt_level(status);
}

/*
 * bsi_write_pram
 * Write a value to the given address in the BSI internal Pointer RAM:
 */

static void bsi_write_pram (bsi_instance_t *ds,
			    uint            address,
			    uint            value)
{
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;
    int			count;
    leveltype		level;

    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;

    BSI_DEBUG(buginf(
	    "bsi_write_pram(): ds %#x, address %d, value %#x, idb %#x\n",
	    ds, address, value, ds->idb));
    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Make sure that SAR.PTOP is asserted (can't do a PTOP if not):
     */
    count = ds->delay_10_usec;
    while ((!(bsi->sar & BSI_SAR_PTOP)) && (count-- > 0));
    if (count < 0) {		/* Timed out */
	errmsg(&msgsym(FDDIFAIL, FDDI), ds->idb->hw_namestring,
	       "Timeout waiting for sar.ptop in bsi_write_pram",
	       "count", count);

	net_cstate(ds->idb, IDBS_DOWN);
	bsi_stop(ds->idb);

	reset_interrupt_level(level);
	return;
    }

    /* Put new value in mailbox */
    ds->ioram->mbox.d.lword = value;

    /* Set up pointer control and address register */
    bsi->pcar = address;

    BSI_DEBUG(buginf("bsi_write_pram(): writing - mailbox %#x, pcar %d, "
		     "wrote pcar %d\n",
		     ds->ioram->mbox.d.lword, bsi->pcar, address));

    /* Tell BSI to do PTOP */
    bsi->sar &= ~BSI_SAR_PTOP;

    reset_interrupt_level(level);
}


/*
 * bsi_write_lram
 * Write data to the given address in the BSI internal Limit RAM:
 */

static inline void bsi_write_lram_inline (bsi_instance_t *ds,
					  uint            address,
					  uint            data)
{
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;
    unsigned char	bsi_sar;
    int			count;
    uint		bit_d8;
    leveltype		level;

    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;

    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Make sure that SAR.LMOP is asserted (can't do a LMOP if not):
     */
    count = ds->delay_10_usec << 5;
    while ((bsi_sar = bsi->sar, !(bsi->sar & BSI_SAR_LMOP)) && (count-- > 0))
	;
    if (count < 0) {		/* Timed out */
	errmsg(&msgsym(FDDIFAIL, FDDI), ds->idb->hw_namestring,
	       "Timeout waiting for sar.lmop in bsi_write_lram",
	       "count", count);

	net_cstate(ds->idb, IDBS_DOWN);
	bsi_stop(ds->idb);

	reset_interrupt_level(level);
	return;
    }

    /*
     * Set up limit address register:
     */
    bit_d8 = (data & BSI_LCAR_D8) >> BSI_LCAR_D8_SHIFT;
    bsi->lar = (address << BSI_LCAR_ADDR_SHIFT) | bit_d8;

    /*
     * Set up limit data register:
     */
    bsi->ldr = (BSI_LCAR_LSB_DATA & data);

    BSI_DEBUG(buginf(
	    "bsi_write_lram(): writing - lar %#x, bit_d8 %d, ldr %#x\n",
	    bsi->lar, bit_d8, bsi->ldr));

    /*
     * Tell BSI to do LMOP:
     */
    bsi->sar = bsi_sar & ~BSI_SAR_LMOP;

    reset_interrupt_level(level);
}

static void bsi_write_lram (bsi_instance_t *ds,
			    uint            address,
			    uint            data)
{
    BSI_DEBUG(buginf(
	    "bsi_write_lram(): ds %#x, address %d, data %#x, idb %#x\n",
	    ds, address, data, ds->idb));
    bsi_write_lram_inline(ds, address, data);
}

/*
 * Return an unused page for the BSI to the private buffer cache
 *
 * Danger! Danger Will Robinson! Danger!
 * 
 * This routine expects the packet to be completely *untouched* due to the
 * FDDI NIMs paging hardware (which results in extra IDUD's pointing to
 * virgin packets).  Passing in *used* frames to this routine will most likely
 * result in a router screaming to the ground in flames...
 *
 * If the above diatribe means nothing to you, ask a grown-up before you
 * use this function or expect trouble to come looking for you in the form of
 * a large and angry Scotsman wielding blunt, ugly language...
 */

static inline void bsi_return_page_inline (bsi_instance_t *ds, paktype *pak)
{
    pooltype *pool = ds->idb->pool;

    pool->cache[pool->cache_end++] = pak;

    /* Mind the cache threshold callback. */

    if (pool->cache_item.threshold &&
	(pool->cache_end == pool->cache_threshold)) {
	pool->cache_threshold_count++;
	(*pool->cache_item.threshold)(pool);
    }
}

/*
 * Get a page for the BSI from the private buffer cache
 *
 * The current scheme of detecting the PSP reuse buffer is to verify
 * that the IDUD's location field contains the next expected value
 * (buffer address). This scheme can fail when the same buffer is used
 * twice in a sequence since we use a stack type cache scheme where the
 * last returned (used) buffer is the first one to be allocated (reused)
 * again.  Therefore, in order to ensure that the same buffer is not
 * used twice in a row, we will keep track of what page was last
 * allocated on a per receive channel and prevent it from being reused
 * next.
 */
static inline paktype *bsi_get_page_inline (bsi_instance_t *ds,
					    ind_chan_t *ic)
{
    static paktype *bsi_get_page(bsi_instance_t *, ind_chan_t *);
    paktype  *pak;
    pooltype *pool = ds->idb->pool;

    if (pool->cache_end) {
	pak = pool->cache[--pool->cache_end];
    } else {
	return(NULL);
    }

    return(pak);
}

static paktype *bsi_get_page (bsi_instance_t *ds, ind_chan_t *ic)
{
    return (bsi_get_page_inline(ds, ic));
}

/*
 * bsi_disable
 * Disable the receiver on RX channels 1 and 2 to throttle
 * the FDDI interface because of excessive inputs. Don't
 * throttle RX channel 0, which is used for SMT/CMT:
 */

static void bsi_disable (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;
    leveltype		level;

    ds = INSTANCE;
    BSI_DEBUG(buginf("bsi_disable(): idb 0x%#x\n", idb));
    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Disable COPY on ic1 and ic2 to drop routed and bridged traffic
     * while we are throttled:
     */
    bsi->icr &= BSI_ICR_CC0;
    ds->throttled++;

    reset_interrupt_level(level);

    idb_mark_throttled(idb);
}

/*
 * bsi_enable
 * Enable a throttled FDDI interface
 * Reenable the receiver on RX channels 1 and 2 after it was throttled.
 */

static void bsi_enable (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;
    leveltype		level;

    ds = INSTANCE;
    BSI_DEBUG(buginf("bsi_enable(): idb 0x%#x\n", idb));
    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Enable COPY on ic1 and ic2 to reenable all input traffic
     * after we were throttled:
     * Note that IC0 is used for MAC and SMT frames only,
     * IC1 is used for packets addressed to us (including keepalives),
     * and IC2 is where externally matched addresses go. IC2 is used
     * to receive multicasts when routing and to receive bridgeable
     * packets while bridging.  Only receive our own packets if we
     * have 'loopback' configured on the interface:
     */
    bsi->icr = (BSI_ICR_COPY_OURS | (BSI_ICR_COPY_OURS << BSI_ICR_CC0_SHIFT));
    if (idb->loopback) {
	bsi->icr |= (BSI_ICR_COPY_OURS_HEARSELF << BSI_ICR_CC1_SHIFT);
	idb->status |= IDB_HEARSELF;
    } else {
	bsi->icr |= (BSI_ICR_COPY_OURS << BSI_ICR_CC1_SHIFT);
	idb->status &= ~IDB_HEARSELF;
    }

    reset_interrupt_level(level);

    idb_mark_unthrottled(idb);
}

/*
 * Pass an input packet up to process level, copying the packet
 * into a new buffer if necessary to align the packet in memory
 * correctly for various FDDI encapsulations, ie ISO1, ISO2, or ISO3:
 */
static void bsi_queue_for_process (bsi_instance_t *ds, paktype *pak)
{
    ushort	scount;
    fddi_hdr    *fddi;
    snap_hdr    *snap;
    iqueue_t	iqueue;

    scount = pak->datagramsize;
    fddi = (fddi_hdr *)pak->datagramstart;
    snap = (snap_hdr *)fddi->data;
    
#if DEBUG_PKT
    buginf("bsi_queue_for_process(): ds %#x, pak %#x\n", ds, pak);
    bsi_debug_dump_packet((uchar *)fddi, 30);
#endif

    /*
     * Drop the packet and throttle the interface if the input
     * queue is full.
     */
    if (ds->idb->input_qcount > ds->idb->input_defqcount) {
	bsi_disable(ds->idb);
	datagram_done (pak);
	ds->idb->counters.input_drops++;
	return;
    }

    if (fddi_debug)
	bsi_dump_packet("bsi_queue_for_process", pak->datagramstart, 5);

    ds->idb->rxrifwordlen = 0;
    if (fddi->saddr[0] & FDDI_RII) {
	ds->idb->rxrifwordlen = RIF_LENGTH(fddi->data[0]) / 2;
    }

    /*
     * Check to see if we have an encapsulated bridge frame
     */
    if (pak->flags & PAK_TBRIDGE_IN) {
        /*
         * As we have, in effect, an Ethernet frame addressed to us, do
         * the required futzing around to make the upper levels respond
         * without barfing
         */
        iqueue = ether_parse_packet(ds->idb, pak, NULL);
	snap = (snap_hdr *)(((ether_hdr *)(pak->datagramstart))->data);
    } else {
        /*
         * See which FDDI encapsulation the packet is - ISO1, ISO2, or ISO3,
         * and then determine whether or not we have to copy the packet
         * to get the start of the packet in the correct place:
         */
        iqueue = (*ds->idb->parse_packet)(ds->idb, pak, NULL);
    }

    if (pak->datagramstart != (uchar *)fddi) {
	paktype *tmp;
	
	/*
	 * Yes, we need to copy the packet into a new buffer:
	 */
	tmp = pak;
	pak = input_getbuffer((uint)scount, ds->idb);
	if (pak) {
	    
	    /*
	     * Save the packet enctype:
	     */
	    pak->enctype = tmp->enctype;
	    
	    /*
	     * We don't have to worry about masking the buffer flags,
	     *  since they now live in buffer_flags:
	     */
	    pak->flags = tmp->flags;
    
	    /*
	     * Compute datagramstart address in new packet from start
	     * offset from old packet.  Call fddi_pakaling with the same
	     * data as before, only have it adjust the new packet this
	     * time.
	     */
            if (pak->flags & PAK_TBRIDGE_IN) {
                iqueue = ether_parse_packet(ds->idb, pak, (uchar *)snap);
            } else {
                iqueue = (*ds->idb->parse_packet)(ds->idb, pak, (uchar *)snap);
            }
            pak->datagramsize = scount;
            bcopy(fddi, pak->datagramstart, (uint)scount);
            datagram_done(tmp);
	} else {
	    /*
	     * Failed to get new buffer to copy fddi frame,
	     * so reclaim fddi page buffer:
	     */
	    datagram_done(tmp);
	    return;
	}
    }
    
    /*
     * Do packet accounting for packets going up to process level:
     */
    ds->idb->counters.rx_cumbytes += pak->datagramsize;
    ds->idb->counters.inputs++;
    
    /*
     * Pass the frame up to process level in an appropriate way:
     */
    if (fddi->saddr[0] & FDDI_RII)
	pak->flags |= PAK_SRB_IN;

    if (srb_multiring_any_enabled(ds->idb->firstsw) || (pak->enctype == ET_LLC2))
	rif_update(ds->idb, 0, fddi->saddr, NULL,
		   (srbroute_t *)(ds->idb->rxrifwordlen ? fddi->data:NULL),
		   RIF_AGED);
    if (pak->enctype == ET_LLC2) {
	fddi->saddr[0] &= ~FDDI_RII;
        llc_input(pak);
    } else {
	(*iqueue)(ds->idb, pak);
    }
}

/*
 * bsi_proc_idud_errors
 * Process idud errors
 */
static void bsi_proc_idud_errors (bsi_instance_t *ds, ushort size,
				  ulong idud_lw0, ulong idud_lw1)
{
    union {
	idud_descr_t  i;
	descr_t       d;
    } idud;

    idud.d.w0 = idud_lw0;
    idud.d.w1 = idud_lw1;

    /*
     * Some of the other status fields in the idud
     * are not valid unless the frame ended with an
     * ending delimiter:
     */
    if ((idud.i.fra_tc & BSI_TC_MASK) == BSI_TC_ED) {
	
	if (!idud.i.frs_vfcs) {
	    /* Count CRC errors: */
	    ds->idb->counters.input_crc++;
	    BSI_DEBUG(buginf("*I:CRC ERR*"));
	}
	if (!idud.i.frs_vdl) {
	    /* Count invalid data length errors */
	    ds->invalid_data_length++;
	    BSI_DEBUG(buginf("*I:BAD LEN*"));
	} else if (size < (FDDI_MINDGRAM + FDDI_FCS)) {
	    /* Count runts, since length is valid: */
	    ds->idb->counters.runts++;
	    BSI_DEBUG(buginf("*I:RUNT:%d bytes*", size));
	}
    } else {
	/*
	 * Decode the terminationm condition:
	 */
	switch (idud.i.fra_tc) {
	    
	case BSI_TC_OTHER:
	    ds->idb->counters.input_frame++;
	    ds->tc_other++;
	    BSI_DEBUG(buginf("*I:TC:OTHER*"));
	    break;
	    
	case BSI_TC_FORMAT_ERR:
	    ds->idb->counters.input_frame++;
	    ds->tc_format_err++;
	    BSI_DEBUG(buginf("*I:TC:FORMAT_ERR*"));
	    break;
	    
	case BSI_TC_FRAME_STRIPPED:
	    /*
	     * Not an input or output error.
	     * Just count it for couriosity. The BSI
	     * didn't want to copy this frame, but had
	     * already started, so it has to pass it
	     * back to us to be reused via an IDUD:
	     * FOR SPEED, change to return it directly
	     * to this channel's PSP queue! *CJS*
	     */
	    ds->tc_frame_stripped++;
	    BSI_DEBUG(buginf("*I:TC:FRAME_STRIPPED*"));
	    if (ds->idb->counters.input_error)
		ds->idb->counters.input_error--;
	    return;
	}
    }
    
    if (idud.i.is & BSI_IDUD_IS_COPY_ABORT) {
	
	switch (idud.i.is) {
	    
	case BSI_IDUD_IS_NO_DATA_SPACE:
	case BSI_IDUD_IS_NO_HEADER_SPACE:
	case BSI_IDUD_IS_NO_INFO_SPACE:
	case BSI_IDUD_IS_INCOMPLETE_INFO:
	    /* We ran out of space for the input frame */
	    ds->idb->counters.input_resource++;
	    BSI_DEBUG(buginf("*I:COPY_ABORT: %d*", idud.i.is));
	    break;
	    
	case BSI_IDUD_IS_FIFO_OVERRUN:
	    ds->idb->counters.input_overrun++;
	    BSI_DEBUG(buginf("*I:FIFO_OVERRUN*"));
	    break;
	    
	case BSI_IDUD_IS_BAD_FRAME:
	    /* We had a bad frame, because of a bad
	     * FCS, an invalid data length, or both:
	     */
	    ds->bad_frame++;
	    ds->idb->counters.input_frame++;
	    BSI_DEBUG(buginf("*I:IS:BAD FRAME*"));
	    break;
	    
        case BSI_IDUD_IS_PARITY_ERR:
	    ds->idb->counters.parity++;
	    BSI_DEBUG(buginf("*I:PARITY_ERR*"));
	    break;
	    
	case BSI_IDUD_IS_INTERNAL_ERR:
	    ds->internal_error++;
	    BSI_DEBUG(buginf("*I:INTERNAL_ERR*"));
	    break;
	}
    }
}

/*
 * bsi_proc_psp_error
 * PSP error has ocuured. Return true if the current IDUD
 * is not usable 
 *
 * Sometimes BSI is not using the PSP buffers sequentially.
 * It might copy the frame into the previous PSP buffer or next
 * PSP buffer and at most it will skip 2 PSP buffers.
 */
boolean bsi_proc_psp_error (bsi_instance_t *ds, ind_chan_t *ic, uint idu)
{
    uint psp;
    ulong psp_lw1;
    paktype *pak;
    bsi_ring_entry_t *psp_ring_entry, *psp_ring_limit;

    psp_ring_entry = ic->psp_ring.ptr;
    psp_ring_limit = ic->psp_ring.limit;

    /*
     * Check whether the IDUD is a previous PSP buffer
     */
    psp = (uint)(((psp_descr_t *)psp_ring_entry->prev->addr)->loc);
    if (idu == psp) {
        ds->psp_unknown_use_err = 0;
        return(TRUE);
    }

    /*
     * Make sure we are not going beyond the limit
     */
    if (psp_ring_entry == psp_ring_limit) {
        ds->psp_unknown_use_err++;
        return(TRUE);
    }

    /*
     * Check whether the IDUD is a next PSP buffer
     */
    psp = (uint)(((psp_descr_t *)psp_ring_entry->next->addr)->loc);
    if (idu == psp) {
        psp_lw1 = ((psp_descr_t *)(psp_ring_entry->addr))->loc;
        psp = (uint)BSI_TO_68K(psp_lw1);
        pak = (paktype *)(GETLONG((uchar *)BSI_4K_PAGE(psp)));
        bsi_return_page_inline(ds, pak);
        ic->psp.count--;
        ic->psp_ring.ptr = ic->psp_ring.ptr->next;
        return(FALSE);
    }

    /*
     * Make sure we are not going beyond the limit
     */
    if (psp_ring_entry->next == psp_ring_limit) {
        ds->psp_unknown_use_err++;
        return(TRUE);
    }

    /*
     * Check whether BSI missed two PSP buffers
     */
    psp = (uint)(((psp_descr_t *)psp_ring_entry->next->next->addr)->loc);
    if (idu == psp) {
        psp_lw1 = ((psp_descr_t *)(psp_ring_entry->addr))->loc;
        psp = (uint)BSI_TO_68K(psp_lw1);
        pak = (paktype *)(GETLONG((uchar *)BSI_4K_PAGE(psp)));
        bsi_return_page_inline(ds, pak);
        psp_lw1 = ((psp_descr_t *)(psp_ring_entry->next->addr))->loc;
        psp = (uint)BSI_TO_68K(psp_lw1);
        pak = (paktype *)(GETLONG((uchar *)BSI_4K_PAGE(psp)));
        bsi_return_page_inline(ds, pak);
        ic->psp.count -= 2;
        ic->psp_ring.ptr = ic->psp_ring.ptr->next->next;
        return(FALSE);
    }
    ds->psp_unknown_use_err ++;
    return(TRUE);
}

/*
 * bsi_proc_idud
 * Accept input data units from FDDI
 */
static void bsi_proc_idud (bsi_instance_t *ds, ind_chan_t *ic)
{
    uchar 		ff;
    ushort            	rxtype;
    ushort       	packets = 0;
    ulong		idud_lw0, idud_lw1;
    fddi_hdr 		*fddi;
    
    /*
     * Do we need to throttle the interface so that
     * process code can run?
     */
    if (sched_run_interval && AWAKE(sched_run_time)) {
	/*
	 * The scheduler has not run recently enough, so throttle
	 * the BSI, and then process the already received iduds so
	 * that we don't miss any SMT/CMT frames already received:
	 */
	bsi_disable(ds->idb);
    }

    idud_lw1 = ((idud_descr_t *)(ic->idud_ring.ptr->addr))->loc;
    idud_lw0 = *(ulong *)(ic->idud_ring.ptr->addr);

    while ((BSI_VALID_IDUD_DESC(idud_lw1)) &&
	   (++packets < ic->idud.max_iduds)) {

	paktype *new_pak_frag, *pak;
	uint   idu, psp;
	ushort size;
	
	/*
	 * Convert the BSI pointer to a 32 bit address for the CPU:
	 */
	idu = (uint)BSI_TO_68K(idud_lw1);
	new_pak_frag = (paktype *)(GETLONG((uchar *)BSI_4K_PAGE(idu)));
	idu = idu & 0xFFFFFFFC;
	
	/*
	 * Check for the BSI Erroneous PS Reuse bug:
	 */
	psp = (uint)(((psp_descr_t *)ic->psp_ring.ptr->addr)->loc);
	if (idu != psp) {
	    ds->psp_reuse_err++;
	    ds->idb->counters.input_error++;
            if (bsi_proc_psp_error(ds, ic, idu)) {
                new_pak_frag = NULL;
                goto next_idud; /* Current IDUD is not usable */
	    }
	}

	/*
	 * We know there is one less frame in the PSP queue on this
	 * channel, and that we can advance the PSP queue pointer:
	 */
	ds->buf_needed++;
	ic->psp.count--;
	ic->psp_ring.ptr = ic->psp_ring.ptr->next;
	
	/*
	 * Save the input interface and clear BSI_MAGIC so that
	 * we don't grab this page again in our page collect:
	 */
	new_pak_frag->if_input = ds->swidb;
	
	/*
	 * The first idud must have the FIRST bit set:
	 */
	if (idud_lw1 & BSI_FIRST) {
	    
	    got_first_idud:
	    
	    /*
	     * If we were waiting for the tail end of an incomplete
	     * packet, free the incomplete packet and clear the pointer:
	     */
	    if (ic->idud.pak) {
		/*
		 * Return the page with the IDUD to the free
		 * queue:
		 * FOR SPEED, change to return it directly
		 * to this channel's PSP queue! *CJS*
		 */
		bsi_return_page_inline(ds, ic->idud.pak);
		ic->idud.pak = NULL; /* No partial packet */
	    }
	    
	    pak = new_pak_frag;
	    size = BSI_IDUD_GET_LENGTH(idud_lw0);
	    
	    while (!(idud_lw1 & BSI_LAST)) {
		/*
		 * The current IDUD was not the end of the packet.
		 * Add the size of the current idud to the packet
		 * under construction. Then free the unused FDDI
		 * page buffer that this idud points to:
		 */
		/* NEXT_IDUD(); */
		
		/*
		 * Increment the limit pointer to point to the next
		 * entry in the idud ring
		 */
		ic->idud_ring.limit = ic->idud_ring.limit->next;
		
		/*
		 * Mark the the entry at the ptr value to be NULL
		 */
		((idud_descr_t *)(ic->idud_ring.ptr->addr))->loc = 
		    BSI_NULL_IDUD_LOC;
		
		/*
		 * Step on to the next IDUD in the ring and read it
		 * up into the union structure
		 */
		ic->idud_ring.ptr   = ic->idud_ring.ptr->next;
		idud_lw1 = ((idud_descr_t *)(ic->idud_ring.ptr->addr))->loc;
		idud_lw0 = *(ulong *)(ic->idud_ring.ptr->addr);

		if (BSI_VALID_IDUD_DESC(idud_lw1)) {
		    
		    /*
		     * Convert the BSI pointer to a 32 bit address
		     * for the CPU:
		     */
		    idu = (uint)BSI_TO_68K(idud_lw1);
		    new_pak_frag =
			(paktype *)(GETLONG((uchar *)BSI_4K_PAGE(idu)));
		    idu = idu & 0xFFFFFFFC;
		    
		    /*
		     * Check for the BSI Erroneous PS Reuse bug:
		     */
		    psp = (uint)(((psp_descr_t *)ic->psp_ring.ptr->addr)->loc);
		    if (idu != psp) {
                        ds->psp_reuse_err++;
			ds->idb->counters.input_error++;
                        if (bsi_proc_psp_error(ds, ic, idu)) {
                            new_pak_frag = NULL;
                            goto next_idud; /* Current IDUD is not usable */
	                }
		    }
		    
		    ds->psp_unknown_use_err = 0;
		    
		    /*
		     * We know there is one less frame in the PSP queue
		     * on this channel, and that we can advance the PSP
		     * queue pointer:
		     */
		    ds->buf_needed++;
		    ic->psp.count--;
		    ic->psp_ring.ptr = ic->psp_ring.ptr->next;
		    
		    /*
		     * Wipe BSI_MAGIC so that we don't grab another
		     * pointer to this frame during page collection:
		     */
		    new_pak_frag->if_input = NULL;
		    
		    /*
		     * Make sure that we don't see a first frame:
		     */
		    if (!(idud_lw1 & BSI_FIRST)) {
			continue_frame:

			size += BSI_IDUD_GET_LENGTH(idud_lw0);
			
			DEBUG_COUNTERS(ds->chained_iduds++);
			/*
			 * Return the unused page that the IDUD pointed
			 * to, since the BSI did not use it because of
			 * the page mapping hardware:
			 * FOR SPEED, change to return it directly
			 * to this channel's PSP queue! *CJS*
			 */
			bsi_return_page_inline(ds, new_pak_frag);
			new_pak_frag = NULL;
			
		    } else {
			
			/*
			 * Clean up the previous incomplete frame,
			 * and attempt to receive this new frame:
			 */
			ds->no_middle_last++;
			
			/*
			 * Free incomplete pak since we didn't get LAST!
			 */
			if (pak) {
			    bsi_return_page_inline(ds, pak);
			    pak = NULL;
			}
			goto got_first_idud;
		    }
		    
		} else {
		    
		    /*
		     * The LAST IDUD isn't ready for this frame yet,
		     * so, save a pointer to the packet under completion
		     * and return, so that we can finish it up later:
		     */
		    if (ic->idud.pak) {
			/*
			 * If there was already one saved, free it
			 * before saving another:
			 */
			ds->stacked_saved++;
			bsi_return_page_inline(ds, ic->idud.pak);
		    }
		    
		    ic->idud.pak = pak;
		    pak->datagramsize = size;
		    
		    ds->no_valid_idud++;
		    goto out_of_valid_iduds;
		}
	    }
	    
	    /*
	     * We DO now HAVE a LAST idud, so that we can now
	     * process pak:
	     */
	    /*
	     * The FDDI driver assumes that the incoming frame
	     * will be SNAP encapsulated, and sets up the
	     * hardware starting pointers appropriately.
	     */
	    fddi = (fddi_hdr *)pak->datagramstart;
	    
	    /*
	     * Only process the packet if the termination condition
	     * says that we got an ending delimiter, there are no
	     * CRC errors, there is a valid length field, valid
	     * length, no copy abort, no other IS errors:
	     */
	    if (((idud_lw0 & (BSI_IDUD_VALID_MASK)) == BSI_IDUD_VALID_MATCH) &&
		(size >= (FDDI_MINDGRAM + FDDI_FCS))) {
		
		ff = (fddi->fc & FC_FF_MASK) >> FC_FF_SHIFT;
		
		/*
		 * Subtract off the FCS bytes from the packet
		 * length:
		 */
		pak->datagramsize = size - FDDI_FCS;
		
		/*
		 * Use the ic->queue_number to differentiate
		 * between LLC frames addressed to us which
		 * should be received or fast switched/routed, 
		 * SMT/CMT/MAC frames, and packets
		 * which should be considered for bridging:
		 */ 
		if (ic->queue_number == 1) {
		    
		    /*
		     * We got a frame addressed to us. It should
		     * be an LLC frame. If so, we should fast
		     * switch it, route it, or receive it:
		     */
		    if (ff == FDDI_FF_LLC_FRAME) {
			
			DEBUG_COUNTERS(ds->llc++); /* Debugging */
			/*
			 * Put this packet on the input queue:
			 */
			pak->flags |= PAK_INPUTQ;
			ds->idb->input_qcount++;
			
			/*
			 * Decode FDDI encapsulation
			 */
			pak->rxtype = rxtype =
			    fddi_decode_encapsulation_inline(pak, fddi, FDDI_RII);
			
			/*
			 * Attempt to fast switch this packet:
			 */
			if (reg_invoke_fddi_fs(rxtype, pak)) {
#if DEBUG_PKT_FAST
			    buginf("bsi_proc_idud(): fastswitch pkt\n");
			    bsi_debug_dump_packet(pak->datagramstart, 30);
#endif
			    
			    DEBUG_COUNTERS(ds->switched++);
			    pak = NULL;
			    goto next_idud;
			}
			
			if (rxtype == RXTYPE_SRB) {
			    if (reg_invoke_nim_vring_input(ds->idb, pak))
				goto next_idud;
			}
			
			if (rxtype == RXTYPE_DODIP) {
			    iphdrtype *iph = (iphdrtype *)pak->network_start;
			    
			    if (iph->prot == GRE_PROT || 
			       iph->prot == IPINIP_PROT) {
				if (reg_invoke_tunnel_ip_les_fs(pak))
				    goto next_idud;
			    }
			}
			
			/*
			 * Do packet accounting:
			 */
			if (fddi->daddr[0] & FDDI_GROUP)
			    ds->idb->counters.input_broad++;
			
			/*
			 * Are we bridging on this interface?
			 */
			if (ds->idb->tbridge_on_hwidb) {
			    if (reg_invoke_tbridge_forward(pak)) {
				DEBUG_COUNTERS(ds->bridge++);
				pak = NULL;
				goto next_idud;
			    }
			}
			
			/*
			 * If we did not bridge the packet, pass it
			 * up to process level for processing. If we
			 * bridged the packet, it will be returned
			 * to the fddi free page queue after the packet
			 * is transmitted from whatever interface it
			 * was bridged through:
			 */
			
			/*
			 * Pass the input packet up to process
			 * level, first doing any necessary packet
			 * origin changes necessary for different
			 * FDDI encapsulations:
			 */
			bsi_queue_for_process(ds, pak);
			DEBUG_COUNTERS(ds->to_process++);
		    } else {
			DEBUG_COUNTERS(ds->not_llc++);
			if (pak) {
			    retbuffer(pak);
			    pak = NULL;
			}
			goto next_idud;
		    }
		    
		} else if (ic->queue_number == 2) {
		    /*
		     * Queue number 2: Consider frame for bridging:
		     */
		    
		    if (ff == FDDI_FF_LLC_FRAME) {
			DEBUG_COUNTERS(ds->llc++); /* Debugging */
			/*
			 * Put this packet on the input queue:
			 */
			pak->flags |= PAK_INPUTQ;
			ds->idb->input_qcount++;
			
			/*
			 * Decode FDDI encapsulation
			 */
			pak->rxtype = rxtype =
			    fddi_decode_encapsulation_inline(pak, fddi, FDDI_RII);
			if (pak->datagramstart[1] & FDDI_GROUP) {

			    /*
			     * Do packet accounting.
			     */
			    ds->idb->counters.input_broad++;
			}

			/*
			 * Are we bridging on this interface:
			 */
			if (ds->idb->tbridge_on_hwidb) {
			    
			    if (reg_invoke_tbridge_forward(pak)) {
				DEBUG_COUNTERS(ds->bridge++);
				pak = NULL;
				goto next_idud;
			    }
			}
			
			/*
			 * Check for IP and AppleTalk multicast
			 * packet and call unicast 
			 * fastswitching which in turn will call the IP 
			 * multicast fastswitching path.
			 */
			if (rxtype == RXTYPE_DODIP ||
			    rxtype == RXTYPE_APPLE ||
			    rxtype == RXTYPE_SNAP_APPLE ||
			    rxtype == RXTYPE_IEEE_802_10) {
			     if (reg_invoke_fddi_fs(rxtype, pak)) {
			         DEBUG_COUNTERS(ds->switched++);
			         pak = NULL;
			         goto next_idud;
			      }
			}

			/*
			 * If we did not bridge the packet, pass it
			 * up to process level for processing. If we
			 * bridged the packet, it will be returned
			 * to the fddi free page queue after the packet
			 * is transmitted from whatever interface it
			 * was bridged through:
			 */
			/*
			 * Pass the input packet up to process
			 * level, first doing any necessary packet
			 * origin changes necessary for different
			 * FDDI encapsulations:
			 */
			bsi_queue_for_process(ds, pak);
			DEBUG_COUNTERS(ds->to_process++);
		    } else {
			DEBUG_COUNTERS(ds->not_llc++);
			if (pak) {
			    retbuffer(pak);
			    pak = NULL;
			}
			goto next_idud;
		    }
		} else {
		    
		    /*
		     * Queue number 0: MAC/SMT frames:
		     */
		    if (ff == FDDI_FF_SMT_MAC_FRAME) {
			uchar rzzz;
			
			rzzz = fddi->fc & FC_RZZZ_MASK;
			/*
			 * Flush all Token, void, and MAC frames:
			 */
			if ((fddi->fc & FC_SYNCHRONOUS) || 
			    (rzzz == 0)) {
			    /*
			     * Return the page with the IDUD to
			     * the free queue:
			     * FOR SPEED, change to return it directly
			     * to this channel's PSP queue! *CJS*
			     */
			    DEBUG_COUNTERS(ds->mac++); /* Debugging */
			    if (pak) {
				retbuffer(pak);
				pak = NULL;
			    }
			    goto next_idud;
			    
			} else {
			    
			    uchar *bufp;
			    extern queuetype smtbufQ;
			    
			    /*
			     * It's not a MAC frame, it's an SMT frame!
			     */
			    DEBUG_COUNTERS(ds->smt++); /* Debugging */
			    
			    /*
			     * Reject SMT frames with corrupted FS.
			     */
			    if (BSI_IDUD_INVALID_FRS(idud_lw0)) {
				retbuffer(pak);
				pak = NULL;
				goto next_idud;
			    }
			    
			    /*
			     * Do packet accounting for this SMT frame:
			     */
			    if (fddi->daddr[0] & FDDI_GROUP)
				ds->idb->counters.input_broad++;
			    ds->idb->counters.rx_cumbytes +=
				pak->datagramsize;
			    ds->idb->counters.inputs++;
			    
			    /*
			     * Pass SMT/CMT frames up to XDI software.
			     * Flush the frame if the queue is full:
			     */
			    bufp = p_dequeue(&smtbufQ);
			    if ( !bufp ) {
				bprintf("noFBMbuf, ");
				smt_dropped[ds->idb->fddi_stn_id]++;
				retbuffer(pak);
				pak = NULL;
				goto next_idud;
			    } else {
				/* note that the 3 extra FC bytes */
				/* must be passed to FBM also. */
				uchar eac_bits;
				bcopy(-NATL_EXTRA_FC_BYTES +
				      pak->datagramstart,
				      bufp,
				      pak->datagramsize +
				      NATL_EXTRA_FC_BYTES);
				smt_passed[ds->idb->fddi_stn_id]++;
				eac_bits =
				    *(1+(uchar *)(ic->idud_ring.ptr->addr));
				fddi_smt_frame(ds->idb,
					       bufp,
					       pak->datagramsize,
					       eac_bits);
			    }
			    
			    if ( fddi_smt_packet_debug ) 
				fddi_smt_debug(pak,FALSE);
			    retbuffer(pak);
			    pak = NULL;
			    goto next_idud;
			}
		    }
		}

	    } else {

		/*
		 * We got one of the assorted errors. Count it,
		 * and reuse the frame:
		 */
		ds->idb->counters.input_error++;
		bsi_proc_idud_errors(ds, size, idud_lw0, idud_lw1);
		retbuffer(pak);
		pak = NULL;
		goto next_idud;
	    }
	    
	} else {
	    
	    /*
	     * We were waiting for a FIRST idud, or were attempting
	     * to complete an incomplete frame:
	     */
	    
	    /*
	     * If we were waiting for the tail end of an incomplete
	     * packet, let's see if we can finish it up:
	     */
	    if (ic->idud.pak) {
		pak = ic->idud.pak;
		size = pak->datagramsize;
		ic->idud.pak = NULL;
		ds->waited_valid_idud++;
		
		goto continue_frame;
	    } else {
		/*
		 * We expected a FIRST idud, so return this page,
		 * and advance to inspect the next idud:
		 */
		if (new_pak_frag) {
		    /*
		     * Return the page with the IDUD to the free
		     * queue:
		     * FOR SPEED, change to return it directly
		     * to this channel's PSP queue! *CJS*
		     */
		    bsi_return_page_inline(ds, new_pak_frag);
		    new_pak_frag = NULL;
		}
		ds->no_first++;
		goto next_idud;
	    }
	}
	next_idud:
	/* NEXT_IDUD(); */
	
	/*
	 * Mark this IDUD descriptor as invalid (unused):
	 */
	((idud_descr_t *)(ic->idud_ring.ptr->addr))->loc = 
	    BSI_NULL_IDUD_LOC;
	
	/*
	 * Advance our pointer to the next IDUD descriptor,
	 * and save the new ptr and limit registers:
	 */
	ic->idud_ring.limit = ic->idud_ring.limit->next;
	ic->idud_ring.ptr   = ic->idud_ring.ptr->next;
	idud_lw1 = ((idud_descr_t *)(ic->idud_ring.ptr->addr))->loc;
	idud_lw0 = *(ulong *)(ic->idud_ring.ptr->addr);
    }
    
    out_of_valid_iduds:
    {
	uint wrote_value;
	
	/*
	 * Save our advanced pointers, and update the IDUD Q limit ptr,
	 * so bsi can write more idud descriptors in queue:
	 */
	wrote_value = BSI_PTR_TO_LIMIT(ic->idud_ring.limit->addr);
	
	bsi_write_lram_inline(ds, ic->idud.lmt_index, wrote_value);
    }
}

/*
 * Disable the low PSP data space interrupts:
 */

static void bsi_disable_psp_int (bsi_instance_t *ds, ind_chan_t *ic)
{
    bsi_t	*bsi;

    bsi = &ds->module_regs->bsi;
    BSI_DEBUG(buginf("bsi_disable_psp_int(): ds %#x, ic%d@%#x, "
		     "name %s, bsi->nsnr %#x\n",
		     ds, ic->queue_number,
		     ic, ic->psp.name, bsi->nsnr));

    switch(ic->queue_number) {
      case 0:
	bsi->nsnr &= ~BSI_NSNR_LDI0N;
	break;
      case 1:
	bsi->nsnr &= ~BSI_NSNR_LDI1N;
	break;
      case 2:
	bsi->nsnr &= ~BSI_NSNR_LDI2N;
	break;
    }
}

static void bsi_enable_psp_int (bsi_instance_t *ds, ind_chan_t *ic)
{
    bsi_t	*bsi;

    bsi = &ds->module_regs->bsi;
    BSI_DEBUG(buginf("bsi_enable_psp_int(): ds %#x, ic%d@%#x, "
		     "name %s, bsi->nsnr %#x\n",
		     ds, ic->queue_number,
		     ic, ic->psp.name, bsi->nsnr));

    switch(ic->queue_number) {
      case 0:
	bsi->nsnr |= BSI_NSNR_LDI0N;
	break;
      case 1:
	bsi->nsnr |= BSI_NSNR_LDI1N;
	break;
      case 2:
	bsi->nsnr |= BSI_NSNR_LDI2N;
	break;
    }
}

/*
 * Attempt to top off the PSPQ with free pages, and update
 * the limit RAM for the queue, if we changed the limit:
 */

static inline void bsi_fill_psp_inline (bsi_instance_t *ds, ind_chan_t *ic)
{
    register paktype	*pak;
    psp_descr_t		*last_limit;
    int			added;	/* # of new packets added to this PSP queue */
    pooltype		*pool;
    int			quota; /* Max # of pages to allocate to this channel */ 

    /*
     * Add frames to the PSP Queue until the queue is full, we've added
     * our quantum's worth on this queue, or our count has exceeded our
     * max_psps count.
     *
     * The quota value is assigned such that no indicate channel will
     * starve another one, which can happen since there is a fixed
     * sequence (priority) in which they are serviced.
     */
    last_limit = (psp_descr_t *)NULL;
    added = 0;
    pool = ds->idb->pool;
    quota = ic->psp.max_psps - ic->psp.count;
    if (pool->cache_end < ds->buf_needed)
	quota = (quota * pool->cache_end) / ds->buf_needed;

    while (!(BSIQ_FULL(ic->psp_ring)) &&
	   (added < ic->psp.quantum) &&
	   (quota--)) {

	pak = bsi_get_page_inline(ds, ic);
	if (pak) {
	    added++;
	    pak->datagramstart = pak->data_area + bsi_start_offset;
	    pak->enctype = ET_SNAP;
	    pak->if_input = (idbtype *)BSI_MAGIC + ds->idb->unit; /* Flag for page cleanup */
	    ((psp_descr_t *)ic->psp_ring.limit->addr)->loc =
		(ulong)pak->data_area + bsi_bsi_offset;
	    
	    last_limit = (psp_descr_t *)(ic->psp_ring.limit->addr);
	    ds->buf_needed--;
	    ic->psp.count++;
	    ic->psp_ring.limit = ic->psp_ring.limit->next; 

	} else {
	    break;		/* break out of loop */
	}
    }

    /*
     * Only update the limit RAM if we changed the queue limit:
     */
    if (last_limit) {
	uint wrote_value;
	wrote_value = BSI_PTR_TO_LIMIT(last_limit);
	bsi_write_lram_inline(ds, ic->psp.lmt_index, wrote_value);
    } else {
	
	/*
	 * Either the BSI psp queue was full or 
         * we could not get a new page, or
	 * the queue max_psps value was reached.
	 */
	
	/*
	 * Was the psp queue full?
	 */
	if (BSIQ_FULL(ic->psp_ring)) {
	    ds->full_pspQ_err++;
	    bsi_proc_idud(ds, ic);
	}

	/*
	 * Was the max_psps limit reached?
	 */
	if (ic->psp.max_psps == ic->psp.count) {
	    ds->max_pspQ_err++;
	    bsi_proc_idud(ds, ic);
	}

	/*
	 * If we could not get a new FDDI page, disable this interrupt
	 * so that we don't get the same interrupt again forever:
	 */
	if (!added) {
	    ds->no_page++;
	    bsi_disable_psp_int(ds, ic);
	}
    }
}

static void bsi_fill_psp (bsi_instance_t *ds, ind_chan_t *ic)
{
    BSI_DEBUG(buginf(
	    "bsi_fill_psp(): ds %#x, ic@%#x, number %d, name %s, count %d\n",
	    ds, ic, ic->queue_number, ic->psp.name, ic->psp.count));
    bsi_fill_psp_inline(ds, ic);
}

static void bsi_proc_cnf(bsi_instance_t *, req_chan_t *);

void bsi_interrupt (uchar mar, hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;

    ds = INSTANCE;
    nim = ds->module_regs;

    bsi = (bsi_t *)&nim->bsi;

    /*
     * Is any bit in the Indicate Attention register set?
     */
    if (mar & BSI_MAR_INA) {
	uchar iar;

	/*
	 * Handle and clear interrupt:
	 */
	iar = bsi->iar & bsi->inr;
	bsi->iar = 0;		/* Clear interrupt conditions */

	/*
	 * Did we get an interrupt telling us that we have
	 * an input data unit descriptor (IDUD) to process?
	 */
	if (iar & BSI_IAR_BRKI1) {
	    bsi_proc_idud(ds, &ds->ic1); /* Drain the IDUD queue */
	    /*
	     * If we don't have enough pages in the queue for another
	     * round of iduds, add free pages to this queue:
	     */
	    if (ds->ic1.psp.count <= ds->ic1.psp.quantum)
		bsi_fill_psp_inline(ds, &ds->ic1); /* Refill the PSP queue */
	}

        /* Check if there is anything else to process on the input channels */
        if (iar & (BSI_IAR_BRKI2 | BSI_IAR_BRKI0 |
                   BSI_IAR_EXCI0 | BSI_IAR_EXCI1 | BSI_IAR_EXCI2)) {
            /*
             * Check for frames on the other channels
	     */

	    if (iar & BSI_IAR_BRKI2) {
	        bsi_proc_idud(ds, &ds->ic2);
	        if (ds->ic2.psp.count <= ds->ic2.psp.quantum)
		    bsi_fill_psp(ds, &ds->ic2);
	    }
	    if (iar & BSI_IAR_BRKI0) {
	        bsi_proc_idud(ds, &ds->ic0);
	        if (ds->ic0.psp.count <= ds->ic0.psp.quantum)
		    bsi_fill_psp(ds, &ds->ic0);
	    }

	    /*
	     * Check for input exceptions, and count them:
	     */
	    if (iar & BSI_IAR_EXCI0)
		ds->iar_exci0++;
	    if (iar & BSI_IAR_EXCI1)
		ds->iar_exci1++;
	    if (iar & BSI_IAR_EXCI2)
		ds->iar_exci2++;
	}
    }

    /*
     * Is any bit in the Request Attention register set?
     */
    if (mar & BSI_MAR_RQA) {
	uchar rar;

	/*
	 * Read and clear interrupt:
	 */
	rar = bsi->rar & bsi->rnr;
	bsi->rar = 0;		/* Clear interrupts */

	/*
	 * Process any completed requests, or full confirmation queues:
	 */

	if (rar & (BSI_RAR_BRKR1 | BSI_RAR_USRR1)) {
	    bsi_proc_cnf(ds, &ds->rq1);
	}
	if (rar & (BSI_RAR_BRKR0 | BSI_RAR_USRR0)) {
	    bsi_proc_cnf(ds, &ds->rq0);
	}

        /*
         * Check for errors
         */
        if (rar & (BSI_RAR_EXCR1 | BSI_RAR_EXCR0 |
                   BSI_RAR_EXCR1 | BSI_RAR_EXCR0)) {
            /*
             * Did request processing shut down for some reason?
             */
            if (rar & BSI_RAR_USRR0)
                ds->rar_usrr0++;
            if (rar & BSI_RAR_USRR1)
                ds->rar_usrr1++;
            /*
             * If any fatal errors occured, mark us down,
             * and disable further interrupts from the BSI:
             */
            if (rar & BSI_RAR_EXCR1)
                ds->rar_excr1++;
            if (rar & BSI_RAR_EXCR0)
                ds->rar_excr0++;
        }
    }

    /*
     * Avoid further unfrequent processing
     */
    if (!(mar & (BSI_MAR_STA | BSI_MAR_NSA | BSI_MAR_SVA))) {
        return;
    }

    /*
     * Is any error bit in the State Attention register set?
     */
    if (mar & BSI_MAR_STA) {
	uchar star;

	/*
	 * Check only unmasked interrupts:
	 */
	star = bsi->star & bsi->stnr;

	/*
	 * If any fatal errors occured, mark us down,
	 * and disable further interrupts from the BSI:
	 */
	if (star & (BSI_STAR_ERR |
		    BSI_STAR_INSTOP | BSI_STAR_RQSTOP | BSI_STAR_SPSTOP |
		    BSI_STAR_BPE | BSI_STAR_CPE | BSI_STAR_CMDE)) {

	    net_cstate(idb, IDBS_DOWN);
	    bsi_stop(idb);
	    errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		   "BSI Error", "star", star);
	    if (star & BSI_STAR_INSTOP)
		errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		       "Indicate Stop", "star", star);
	    if (star & BSI_STAR_RQSTOP)
		errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		       "Request Stop", "star", star);
	    if (star & BSI_STAR_SPSTOP)
		errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		       "Status/Space Stop", "star", star);
	    if (star & BSI_STAR_BPE)
		errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		       "BMAC parity error", "star", star);
	    if (star & BSI_STAR_CPE)
		errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		       "Control bus parity error", "star", star);
	    if (star & BSI_STAR_CMDE)
		errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		       "Command error", "star", star);
	}
    }

    /*
     * Is any bit in the No Space Attention register set?
     */
    if (mar & BSI_MAR_NSA) {
	uchar nsar;
	uchar nsar_mask;

	/*
	 * Handle and clear unmasked interrupts:
	 */
	nsar_mask = 0;
	nsar = bsi->nsar & bsi->nsnr;

	if (nsar & BSI_NSAR_NSR1) {
	    bsi_proc_cnf(ds, &ds->rq1);
	    nsar_mask |= BSI_NSAR_NSR1;
	}

	if (nsar & BSI_NSAR_LDI0) {
	    ds->empty_pspQ0++;
	    bsi_fill_psp(ds, &ds->ic0);
	}

	if (nsar & BSI_NSAR_NSI0) {
	    ds->full_idudQ0++;
	    bsi_proc_idud(ds, &ds->ic0); /* Drain the full IDUD queue */
	    /*
	     * If we don't have enough pages in the queue for another
	     * round of iduds, add free pages to this queue:
	     */
	    if (ds->ic0.psp.count <= ds->ic0.psp.quantum)
		bsi_fill_psp(ds, &ds->ic0); /* Refill the PSP queue */
	    nsar_mask |= BSI_NSAR_NSI0;
	}
        /*
         * Swap the priority of channel 1 and channel 2 so that multicast
         * packets would be received when there are few fddi i/f buffers left
         */
        if (nsar & BSI_NSAR_LDI2) {
            ds->empty_pspQ2++;
            bsi_fill_psp(ds, &ds->ic2);
        }
        if (nsar & BSI_NSAR_NSI2) {
            ds->full_idudQ2++;
            bsi_proc_idud(ds, &ds->ic2); /* Drain the full IDUD queue */
            if (ds->ic2.psp.count <= ds->ic2.psp.quantum)
                bsi_fill_psp(ds, &ds->ic2);      /* Refill the PSP queue */
            nsar_mask |= BSI_NSAR_NSI2;
        }
	if (nsar & BSI_NSAR_LDI1) {
	    ds->empty_pspQ1++;
	    bsi_fill_psp(ds, &ds->ic1);
	}
	if (nsar & BSI_NSAR_NSI1) {
	    ds->full_idudQ1++;
	    bsi_proc_idud(ds, &ds->ic1); /* Drain the full IDUD queue */
	    if (ds->ic1.psp.count <= ds->ic1.psp.quantum)
		bsi_fill_psp(ds, &ds->ic1); /* Refill the PSP queue */
	    nsar_mask |= BSI_NSAR_NSI1;
	}
	bsi->nsar &= ~nsar_mask; /* Clear bits we've taken care of */
    }

    /*
     * Is any bit in the Service Attention register set?
     */
    if (mar & BSI_MAR_SVA) {
	uchar sar;

	/*
	 * Handle and clear interrupts:
	 */
	sar = bsi->sar;		/* If these ints get used, mask with snr */
	bsi->snr = 0;		/* mask further sar interrupts */
	errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
	       "Unexpected SAR interrupt", "sar", sar);
    }
}

static char *bsi_packet_header = "*Packet Header*";
static char *bsi_packet_data   = "*FDDI Packet Data*";

/*
 * bsi_buffer_create
 *
 * Allocate a new IO buffer
 */

static void *bsi_buffer_create (pooltype       *pool,
				pool_item_type type)
{
    paktype      *pak;
    pakdata      *data;
    uchar        *block;

    pak = malloc(sizeof(paktype));

    if (!pak)
	return(NULL);

    /*
     * We actually allocate far more space than the nominal size of
     * the pool.
     */
    block = malloc_iomem_aligned(BSI_BUFFER_SIZE, BSI_BUFFER_PAGE_SIZE);

    if (block) {
	name_memory(pak, bsi_packet_header);
	name_memory(block, bsi_packet_data);

	pak->pool      = pool;
	pak->data_area = block + bsi_buffer_offset;
	data           = data_area_to_pakdata(pak->data_area);
	
	/*
	 * Add another backpointer on the page boundary for
	 * IDU back references 
	 */
	PUTLONG(block, (ulong)pak);
	pak->buffer_flags |= PAK_BUF_PERMANENT;
	pool->perm++;
	pool->total++;
    } else {
	free(pak);
	pak = NULL;
    }

    bsi_page_table[bsi_page_count++] = pak;

    return(pak);
}

/*
 * bsi_buffer_destroy
 *
 * Hand back a FDDI page into the io memory pool
 */

static void bsi_buffer_destroy (pooltype *pool, void *item)
{
    buginf("bsi_buffer_destroy(): Illegal pool destruction!\n");
    crashdump(0);
}

/*
 * Check to see if the FDDI page queue has free pages. If so, check to see
 * if any of the data low exceptions on the input psp queues are disabled.
 * If so, reenable them:
 */

static void bsi_check_disabled_psp (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    ds = INSTANCE;

    BSI_DEBUG(buginf("bsi_check_disabled_psp(): idb %#x\n", idb));

    if (idb->pool->cache_end >= 2) {
	bsi_enable_psp_int(ds, &ds->ic1);
    }
    if (idb->pool->cache_end >= 2) {
	bsi_enable_psp_int(ds, &ds->ic0);
    }
    if (idb->pool->cache_end >= 2) {
	bsi_enable_psp_int(ds, &ds->ic2);
    }
}

/*
 * bsi_cache_threshold
 *
 * Callback routine from the buffer management code. We get a callback
 * when the cache for the fddi buffer pool reaches our threshold value.
 * Search for any interfaces that have had the psp empty interrupt(s)
 * disabled due to lack of buffers, and re-enable the interrupts.
 */
static void bsi_cache_threshold (pooltype *pool)
{
    hwidbtype *idb;
    int i, j;

    for (i = 0; i < MAX_STATION_COUNT; i++) {
        for (j=0; j<MAX_MAC_COUNT; j++) {
	    idb = STADATA (i, hwIDB[j]);
	    if (idb) {
		bsi_check_disabled_psp(idb);
	    }
	}
    }
}


/*
 * bsi_build_ring
 *
 * Builds an empty linked list ring that can be used to implement virtual
 * descriptor rings for the bsi chip.
 */
static void bsi_build_ring (bsi_instance_t *ds, bsi_ring_t *ring)
{
    int count, end;
    bsi_ring_entry_t *ring_entry;

    if (ring->ring)
	free(ring->ring);

    /* Don't waste SAM space for BSI channels 0 */
    if ((ring == &ds->rq0.req_ring)  || (ring == &ds->rq0.cnf_ring) ||
        (ring == &ds->rq0.odud_ring) || (ring == &ds->rq0.pak_ring) ||
        (ring == &ds->ic0.idud_ring) || (ring == &ds->ic0.psp_ring))
        ring_entry = malloc_named(ring->size * sizeof(bsi_ring_entry_t),
				  "BSI Ring Entry");
    else
        ring_entry = malloc_named_fast(ring->size * sizeof(bsi_ring_entry_t),
				  "BSI Ring Entry");

    if (!ring_entry) {
	crashdump(0);
    }

    ring->ring = ring_entry;

    end = ring->size - 1;

    /*
     * Run up through the ring array from the 2nd element to the
     * 2nd last one
     */
    for (count = 1; count < end ; count++) {
	/*
	 * Build the doubly linked ring
	 */
	ring_entry[count].next = &ring_entry[count+1];
	ring_entry[count].prev = &ring_entry[count-1];
    }
    
    /*
     * Handle corner cases (first and last)
     */
    ring_entry[0].next   = &ring_entry[1];
    ring_entry[0].prev   = &ring_entry[end];
    ring_entry[end].next = &ring_entry[0];
    ring_entry[end].prev = &ring_entry[end-1];
}


/*
 * bsi_init_req_ring
 * 
 * Initialise a bsi request ring
 */
static void bsi_init_req_ring (bsi_instance_t *ds,
			       req_chan_t     *req)
{
    bsi_ring_entry_t *ring_entry;
    int count;

    /*
     * Request Queue
     */
    {
	/*
	 * Build ring and setup pointer structures
	 */
	req_descr_t *base = (req_descr_t *)req->req.base;
	
	req->req_ring.size = BSI_REQ_QSIZE;
	bsi_build_ring(ds, &req->req_ring);
	
	for (count = 0, ring_entry = req->req_ring.ring;
	     count < BSI_REQ_QSIZE; 
	     count++, ring_entry = ring_entry->next)
	    ring_entry->addr = (paktype *)&base[count];
    }

    /*
     * Confirmation Queue
     */
    {
	/*
	 * Build ring and setup pointer structures
	 */
	cnf_descr_t *base = (cnf_descr_t *)req->cnf.base;

	req->cnf_ring.size = BSI_CNF_QSIZE;
	bsi_build_ring(ds, &req->cnf_ring);

	for (count = 0, ring_entry = req->cnf_ring.ring;
	     count < BSI_CNF_QSIZE; 
	     count++, ring_entry = ring_entry->next)
	    ring_entry->addr = (paktype *)&base[count];
    }

    /*
     * Output descriptor Queue
     */
    {
	/*
	 * Build ring and setup pointer structures
	 */
	txd_t *base = (txd_t *)ds->ioram->oduds;

	req->odud_ring.size = REQUEST_LIMIT;
	bsi_build_ring(ds, &req->odud_ring);

	for (count = 0, ring_entry = req->odud_ring.ring;
	     count < REQUEST_LIMIT;
	     count++, ring_entry = ring_entry->next)
	    ring_entry->addr = (paktype *)&base[count];
    }

    /*
     * Packet Queue
     */
    req->pak_ring.size = REQUEST_LIMIT;
    bsi_build_ring(ds, &req->pak_ring);
}

/*
 * bsi_init_req_channel
 * 
 * Initialise a bsi request channel
 */
void bsi_init_req_channel (bsi_instance_t *ds,
			   req_chan_t     *req)
{
    bsi_ring_entry_t *ring_entry;
    int count;

    /*
     * Reset ring pointers
     */
    req->req_ring.ptr   = req->req_ring.ring;
    req->req_ring.limit = req->req_ring.ring;

    /*
     * Initialise request queue and set it to empty
     */
    req->req.ptr = (req_descr_t *)req->req_ring.ptr->addr;
    bsi_write_pram(ds, req->req.ptr_index, (uint)req->req.ptr);
    req->req.limit = (req_descr_t *)req->req_ring.ptr->addr;

    /*
     * Don't update request limit register until we have a packet to TX
     */
    memset(req->req.base, 0, sizeof(req_descrQ_t));

    /*
     * Reset ring pointers
     */
    req->cnf_ring.ptr   = req->cnf_ring.ring;
    req->cnf_ring.limit = req->cnf_ring.ptr->prev->prev->prev;

    req->cnf.ptr = (cnf_descr_t *)req->cnf_ring.ptr->addr;
    bsi_write_pram(ds, req->cnf.ptr_index, (uint)req->cnf.ptr);
    req->cnf.limit = (cnf_descr_t *)req->cnf_ring.limit->addr;

    bsi_write_lram(ds, req->cnf.lmt_index,
		   BSI_PTR_TO_LIMIT(req->cnf.limit));

    memset(req->cnf.base, 0, sizeof(cnf_descrQ_t));

    /*
     * Reset ring pointers
     */
    req->odud_ring.ptr   = req->odud_ring.ring;
    req->odud_ring.limit = req->odud_ring.ring;

    req->pak_ring.ptr   = req->pak_ring.ring;
    req->pak_ring.limit = req->pak_ring.ring;

    for (count = 0, ring_entry = req->pak_ring.ring;
	 count < REQUEST_LIMIT;
	 count++, ring_entry = ring_entry->next) {
	
	if (ring_entry->addr)
	    datagram_done(ring_entry->addr);
	ring_entry->addr = NULL;
    }

    /*
     * Reinitialize the TX housekeeping stuff:
     */
    req->req.tx_reqs = 0;
    req->req.my_ndx = 0;

}

/*
 * bsi_init_ind_ring
 * 
 * Initialise a bsi indication ring
 */
static void bsi_init_ind_ring (bsi_instance_t *ds,
			       ind_chan_t     *ind)
{
    bsi_ring_entry_t *ring_entry;
    int count;

    /*
     * Indication Queue
     */
    if (!ind->idud_ring.ring) {

	idud_descr_t *base = (idud_descr_t *)ind->idud.base;;

	ind->idud_ring.size = BSI_IDUD_QSIZE;
	bsi_build_ring(ds, &ind->idud_ring);

	for (count = 0, ring_entry = ind->idud_ring.ring;
	     count < BSI_IDUD_QSIZE; 
	     count++, ring_entry = ring_entry->next)
	    ring_entry->addr = (paktype *)&base[count];
    }

    /*
     * Pool Space Queue
     */
    {
	/*
	 * Build ring and setup pointer structures
	 */
	psp_descr_t *base = (psp_descr_t *)ind->psp.base;
	
	ind->psp_ring.size = BSI_PSP_QSIZE;
	bsi_build_ring(ds, &ind->psp_ring);
	
	for (count = 0, ring_entry = ind->psp_ring.ring;
	     count < BSI_PSP_QSIZE; 
	     count++, ring_entry = ring_entry->next)
	    ring_entry->addr = (paktype *)&base[count];
    }
}

/*
 * bsi_init_ind_channel
 * 
 * Initialise a bsi indication channel
 */
static void bsi_init_ind_channel (bsi_instance_t *ds,
				  ind_chan_t     *ind)
{
    bsi_ring_entry_t *ring_entry;
    int count;

    for (count = 0, ring_entry = ind->idud_ring.ring;
	 count < BSI_IDUD_QSIZE; 
	 count++, ring_entry = ring_entry->next)
	((idud_descr_t *)ring_entry->addr)->loc = BSI_NULL_IDUD_LOC;

    /*
     * Reset ring pointers
     */
    ind->idud_ring.ptr   = ind->idud_ring.ring;
    ind->idud_ring.limit = ind->idud_ring.ptr->prev->prev->prev;

    ind->idud.ptr = (idud_descr_t *)ind->idud_ring.ptr->addr;
    bsi_write_pram(ds, ind->idud.ptr_index, (uint)ind->idud.ptr);
    ind->idud.limit = (idud_descr_t *)ind->idud_ring.limit->addr;
    bsi_write_lram(ds, ind->idud.lmt_index,
		   BSI_PTR_TO_LIMIT(ind->idud.limit));

    /*
     * If we were waiting for the tail end of an incomplete
     * packet, free the incomplete packet and clear the pointer:
     */
    if (ind->idud.pak) {
	/*
	 * Return the page with the IDUD to the free
	 * queue:
	 */
	retbuffer(ind->idud.pak);
	ind->idud.pak = NULL; /* No partial packet */
    }


    /*
     * Reset ring pointers
     */
    ind->psp_ring.ptr   = ind->psp_ring.ring;
    ind->psp_ring.limit = ind->psp_ring.ring;

    ind->psp.ptr = (psp_descr_t *)ind->psp_ring.ptr->addr;;
    bsi_write_pram(ds, ind->psp.ptr_index, (uint)ind->psp.ptr);
    ind->psp.limit = (psp_descr_t *)ind->psp_ring.limit->addr;

    memset(ind->psp.base, 0, sizeof(psp_descrQ_t));
    ind->psp.count = 0;
 
    switch (ind->queue_number) {
      case 0:
	ind->idud.max_iduds = FDDI_IDUD0_MAX_IDUDS;
	ind->psp.quantum    = FDDI_PSPQ0_QUANTUM;
	ind->psp.max_psps   = FDDI_PSPQ0_MAX_PSPS;
	break;

      case 1:
	ind->idud.max_iduds = FDDI_IDUD1_MAX_IDUDS;
	ind->psp.quantum    = FDDI_PSPQ1_QUANTUM;
	ind->psp.max_psps   = FDDI_PSPQ1_MAX_PSPS;
	break;

      case 2:
	{
	    /*
	     * If we have spanning tree on, then we'll either be
	     * receiving promiscuously on channel 2 or picking
	     * up encapsulated traffic via multicasts. Either way
	     * we'll need more descriptors
	     */
	    if (ds->idb->tbridge_on_hwidb) {
		ind->idud.max_iduds = FDDI_IDUD2_BRI_MAX_IDUDS;
		ind->psp.quantum    = FDDI_PSPQ2_BRI_QUANTUM;
		ind->psp.max_psps   = FDDI_PSPQ2_BRI_MAX_PSPS;
	    } else {
		ind->idud.max_iduds = FDDI_IDUD2_MAX_IDUDS;
		ind->psp.quantum    = FDDI_PSPQ2_QUANTUM;
		ind->psp.max_psps   = FDDI_PSPQ2_MAX_PSPS;
	    }
	}
	break;
    }
    ds->buf_needed += ind->psp.max_psps;

    /* Fill PSP with some free pages, and then update the limit RAM */
    bsi_fill_psp(ds, ind);
}

/*
 * bsi_proc_cnf_errors
 * Process errors during request confirmations
 */
static void bsi_proc_cnf_errors (bsi_instance_t *ds,
				 cnf_descr_t *cnf_descr_ptr)
{
    switch (cnf_descr_ptr->rs) {

    case BSI_CNF_RS_NONE:
	errmsg(&msgsym(FDDIFAIL, FDDI), ds->idb->hw_namestring,
	       "bad request confirmation status",
	       "rs", cnf_descr_ptr->rs);
	ds->idb->counters.output_error++;
	ds->rs_none++;
	break;

    case BSI_CNF_RS_PREEMPTED:
	ds->rs_preempted++;	/* status */
	BSI_DEBUG(buginf("*O:PREEMPTED*"));
	break;

    case BSI_CNF_RS_PART_DONE:
	ds->rs_part_done++;	/* status */
	BSI_DEBUG(buginf("*O:PART_DONE*"));
	break;

    case BSI_CNF_RS_SERV_LOSS:
	ds->idb->counters.output_error++;
	ds->rs_service_loss++;
	BSI_DEBUG(buginf("*O:SERV_LOSS*"));
	break;

    case BSI_CNF_RS_COMPLETED_BC:
	/* Got my_beacon while in beacon state,	*/
	/* or won claim while in claim state 	*/
	ds->rs_beacon_or_claim++; /* status */
	BSI_DEBUG(buginf("*O:COMPLETED_BC*"));
	break;

    case BSI_CNF_RS_BAD_CONF:
	ds->idb->counters.output_error++;
	ds->rs_bad_confirm++;
	BSI_DEBUG(buginf("*O:BAD_CONF*"));
	break;

    case BSI_CNF_RS_UNDERRUN:
	ds->idb->counters.output_error++;
	ds->idb->counters.output_underrun++;
	BSI_DEBUG(buginf("*O:UNDERRUN*"));
	break;

    case BSI_CNF_RS_HOST_ABORT:
	ds->idb->counters.output_error++;
	ds->rs_host_abort++;
	BSI_DEBUG(buginf("*O:HOST_ABORT*"));
	break;

    case BSI_CNF_RS_BAD_RINGOP:
	ds->idb->counters.output_error++;
	ds->rs_bad_ringop++;
	BSI_DEBUG(buginf("*O:BAD_RINGOP*"));
	break;

    case BSI_CNF_RS_MAC_ABORT:
	/* Will need more work here to deal with receiving an */
	/* unexpected beacon, or losing the claim state.      */
	/* We need to ignore these errors if we are in the    */
	/* claim or beacon states. See page 2-322 of the      */
	/* BSI User's guide. *CJS*                            */
	ds->idb->counters.output_error++;
	ds->rs_bmac_abort++;
	BSI_DEBUG(buginf("*O:MAC_ABORT*"));
	bprintf("MACABORT ");
	break;

    case BSI_CNF_RS_TIMEOUT:
	ds->idb->counters.output_error++;
	ds->rs_trt_timeout++;
	BSI_DEBUG(buginf("*O:TIMEOUT*"));
	break;

    case BSI_CNF_RS_MAC_RESET:
	ds->rs_bmac_reset++;
	BSI_DEBUG(buginf("*O:BMAC_RESET*"));
	break;

    case BSI_CNF_RS_CON_FAILURE:
	ds->idb->counters.output_error++;
	ds->rs_consistency_fail++;
	BSI_DEBUG(buginf("*O:CONSISTENCY_FAIL*"));
	break;

    case BSI_CNF_RS_FATAL_ERROR:
	ds->idb->counters.output_error++;
	ds->rs_fatal_bsi_error++;
	BSI_DEBUG(buginf("*O:FATAL_ERROR*"));
	net_cstate(ds->idb, IDBS_DOWN);
	bsi_stop(ds->idb);
	break;

    default:
	BSI_DEBUG(buginf("*O:DEFAULT HIT %x\n", cnf_descr_ptr->rs));
    }
}

/*
 * bsi_proc_cnf
 * Process request confirmations
 */
static void bsi_proc_cnf (bsi_instance_t *ds, req_chan_t *rqchn)
{
    cnf_descr_t	 *cnf_descr_ptr;
    bsi_ring_t   *ring;
    paktype      *pak;
    ulong 	 cnf_lw1, cnf_lw2;

    ring = &rqchn->cnf_ring;
    cnf_descr_ptr = (cnf_descr_t *)ring->ptr->addr;
    BSI_DEBUG(buginf("proc_cnf\n"));

    while (cnf_lw2 = *(ulong *)((ulong)cnf_descr_ptr+4),
	   (cnf_lw2 & BSI_ADDRFL)) {

	/*
	 * Complete reading the whole descriptor if it is a valid one
	 */
	cnf_lw1 = *(ulong *)cnf_descr_ptr;

	/*
	 * Count transmit errors, etc here:
	 */
	BSI_DEBUG(buginf("ptr %x, error %d\n",
			  cnf_descr_ptr,BSI_ERROR_CNF_DESC(cnf_descr_ptr)));
      
	if ((cnf_lw1 & BSI_CNF_RS_VALID_MASK) != BSI_CNF_RS_VALID_MATCH) {
	    bsi_proc_cnf_errors(ds, cnf_descr_ptr);
	}

	/*
	 * Unlock packet, return it to the appropriate packet pool,
	 * and clear the pak ptr, which we also use as a boolean:
	 */
	pak = (paktype *)rqchn->pak_ring.limit->addr;

	if (pak) {

	    /*
	     * Return buffer to its respective free pool
	     */
	    datagram_done(pak);

	    /*
	     * Mark this CNF descriptor as invalid (unused).
	     */
	    *((double *)cnf_descr_ptr) = 0;

	    rqchn->pak_ring.limit->addr = NULL;
	    rqchn->pak_ring.limit = rqchn->pak_ring.limit->next;

	} else {
	    ds->bad_tx_pak++;

	    /*
	     * Print out the error message using polled IO
	     * and reset interface to recover from bogus descriptor
	     */
	    buginf("\n%%FDDI-1-FDDIFAIL: Interface %s, "
		   "Bad packet pointer, limit addr = %#x, uid = %#x\n",
		   ds->idb->hw_namestring,
		   rqchn->pak_ring.limit->addr,
		   cnf_descr_ptr->uid);
	    bsi_initialize(ds->idb);
	    return;
	}

	/*
	 * Decrement # of TX requests, and Update the request queue ptr:
	 */
	rqchn->req.tx_reqs--;
	rqchn->req_ring.ptr = rqchn->req_ring.ptr->next; 

	/*
	 * Advance our pointer to the next CNF descriptor,
	 * and update our limit pointer:
	 */
	ring->limit = ring->limit->next;
 	ring->ptr = ring->ptr->next; 
	cnf_descr_ptr = (cnf_descr_t *)ring->ptr->addr;
    }

    {
	uint wrote_value;

	/*
	 * Save our advanced pointers, and update the CNF Q limit ptr,
	 * so bsi can write more cnf descriptors in queue:
	 */
	wrote_value = BSI_PTR_TO_LIMIT(ring->limit->addr); 
	bsi_write_lram_inline(ds, rqchn->cnf.lmt_index, wrote_value);
    }

    /*
     * The latest output time:
     */
    GET_TIMESTAMP(ds->idb->lastoutput);
    TIMER_STOP(ds->idb->xmittimer);
}


/*
 * bsi_txqlength
 *
 * Returns the number of packets queued on the main ODUD queue
 */
static int bsi_txqlength (hwidbtype *idb)
{
    bsi_instance_t *ds = INSTANCE;

    return(ds->REQ_CHAN.req.tx_reqs);
}




#if (DEBUG_PKT | DEBUG_PKT_FAST)
/*
 * bsi_dump_packet_data
 *
 * Dumps out a section of the packet buffer to the screen with a space
 * every 8 characters
 */
static void bsi_debug_dump_packet (char *address, int size)
{
    int i;

    buginf("data        : ");
    
    for (i = 0; i < size; i++) {
	if (!(i & 0x07))
	    buginf(" ");

	buginf("%02x", address[i]);
    }
    
    buginf("\n");
}
#endif




/*
 * bsi_get_default_hw_address
 * Get the default hardware address of network interface.
 * Note that the cookie only contains the hardware ethernet address
 * for Ethernet unit number 0. Increment the 24 bit host number for
 * each additional unit.
 */

static void bsi_get_default_hw_address (hwidbtype *idb, uchar *addr)
{
    BSI_DEBUG(buginf("bsi_get_default_hw_address(): idb=%#x\n", idb));
    nim_get_slot_hardware_address(idb, addr);
}

/*
 * bsi_reset_multicast_addresses
 *
 * Clear out all existing Multicast addresses for this interface.
 */

static void bsi_reset_multicast_addresses (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    
    ds = INSTANCE;
    ds->cam_all_multicasts = FALSE;

    BSI_DEBUG(buginf("bsi_reset_multicast_addresses(): idb=%#x\n", 
		      idb));
}

/*
 * bsi_set_cam_mode
 *
 * Set the frame reception mode and interrupts for the XIlinx logic
 */

static void bsi_set_cam_mode (bsi_instance_t *ds)
{
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);

    /* 
     * Enable CAM interrupts (if present)
     */
    if (ds->cam_present) {
	/*
	 * Disable compares whilst we're dorking
	 */
	CAM_CSR_CLEAR_BIT(ds->module_regs->csr, CAM_CSR_CAM_COMPARE_ENABLE);

	if (ds->cam_learning) {
	    /*
	     * If we're transparent bridging, we want the CAM to act as an
	     * active filter (i.e. block frames from addresses that we add).
	     * So we want to copy a frame when we miss a CAM match.
	     */
	    CAM_CSR_CLEAR_BIT(ds->module_regs->csr, CAM_CSR_CAM_COPY_HIT);
	    CAM_CSR_SET_BIT(ds->module_regs->csr, CAM_CSR_SA_MISS_INTR_EN);

	    cam_disable_promiscuous_multicast(ds->module_regs);

	} else {
	    /*
	     * This is the default mode for the interface when routing.
	     * In this mode, the interface will copy all the frames
	     * that get a CAM hit. In this mode, the CAM acts as an
	     * extended interesting address list and is populated with
	     * the multicasts the interface intends to receive.
	     */
	    CAM_CSR_SET_BIT(ds->module_regs->csr, CAM_CSR_CAM_COPY_HIT);
	    
	    /*
	     * We never want a miss interrupt enabled
	     */
	    CAM_CSR_CLEAR_BIT(ds->module_regs->csr, CAM_CSR_SA_MISS_INTR_EN);
	    
	    /*
	     * If this is TRUE, we only do compares against the
	     * OUI of incoming frames. This mode is used by
	     * IP multicast.
	     */
	    if (ds->cam_all_multicasts)
		cam_enable_promiscuous_multicast(ds->module_regs);
	    else
		cam_disable_promiscuous_multicast(ds->module_regs);
	}

	/*
	 * Re-enable compares
	 */
	CAM_CSR_SET_BIT(ds->module_regs->csr, CAM_CSR_CAM_COMPARE_ENABLE);
    }

    reset_interrupt_level(level);
}

/*
 * bsi_add_multicast_address
 *
 * Adds a multicast address to the enabled table for this interface.
 * Calls the necessary routines to manipulate the cam for this
 * address.
 */

static void bsi_add_multicast_address (idbtype *swidb, uchar *address,
				       uchar fsm)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t      *rp;
    hwidbtype           *idb = swidb->hwptr;
    leveltype status;
    
    status = raise_interrupt_level(NETS_DISABLE);
    ds = INSTANCE;
    rp = ds->module_regs;

    BSI_DEBUG(buginf("bsi_add_multicast_address(): idb=%#x\n", idb));

    /*
     * If ds->cam_learning is FALSE, then we're either routing and/or
     * encapsulated bridging. The CAM miss interrupt is disabled for
     * this process.
     */
    if (ds->cam_present) {
	if (!ds->cam_learning) {
	    uchar bitswapped_address[IEEEBYTES];
	    
	    while (rp->csr & CAM_CSR_FIFO_GO)
		;
	    
	    ieee_swap(address, bitswapped_address);
	    
	    les_fddi_cam_populate_inline(idb, bitswapped_address);
	    while (rp->csr & CAM_CSR_FIFO_GO)
		;
	} else {
            /*
             * Bridging case.  CAM is filtering MAC addresses, but we want
             * to receive this one locally.  We may have previously learned
             * this address from a neighbor.  In this case, we are actually
             * worried about unicast MAC addresses.
             */
            les_fddi_cam_depopulate(idb, address, NULL);
	}
    }

    reset_interrupt_level(status);
}

/*
 * bsi_add_multicast_range
 *
 * Adds a multicast address range to the enabled table for this interface.
 *
 * This call will kick the CAM into a promiscuous multicast mode, which means
 * that'll see most multicasts on the network.
 */

static void bsi_add_multicast_range (idbtype *swidb, uchar *address,
				     uchar *mask)
{
    bsi_instance_t *ds;
    hwidbtype      *idb;

    idb = swidb->hwptr;
    ds = INSTANCE;
    ds->cam_all_multicasts = TRUE;

    if ((ds->cam_present) && !(ds->cam_learning)) {
	bsi_add_multicast_address(swidb, address, 0);
	cam_enable_promiscuous_multicast(ds->module_regs);
    }
}

/*
 * Set an fddi group address into bmac
 */
static void bmac_set_gla (xx_fddi_regs_t *fddi_regs, uchar *ga)
{
    int pgmindex, pgmbitpos;
    uchar *pgmp = (uchar *)&fddi_regs->bmac.gam0;  

    /*
     * Disable group short addrs
     */
    fddi_regs->bmac.gsa0 = 0x7f;

    fddi_regs->bmac.gla0 = ga[0];
    fddi_regs->bmac.gla1 = ga[1]; 
    fddi_regs->bmac.gla2 = ga[2];
    fddi_regs->bmac.gla3 = ga[3];
    fddi_regs->bmac.gla4 = ga[4];
    
    pgmindex = ga[5] / 8;
    pgmbitpos = ga[5] % 8;

    pgmp += pgmindex*2;
    *pgmp = *pgmp | (1 << pgmbitpos);
}

/*
 * bsi_bmac_initialize()
 *
 * init the bmac if the instance bsi_bmac_reset flag is set,
 * if the mac addr has changed.
 */
static void bsi_bmac_initialize (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;
    uchar	         old_mr, wrote, read;
    uchar                gla[6];
    BSI_DEBUG(SAVE_CALLER());

    ds = INSTANCE;
    bprintf("\nbsi_bmac_initialize: ");

    BSI_DEBUG(buginf("bmac_initialize(): from %#x, idb=%#x, idb->state=%d\n",
		     caller(), idb, idb->state));
    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;

    if ( !ds->bsi_bmac_reset &&
	(GETSHORT(&idb->hardware[4]) == ds->bmac_address[2]) &&
	(GETSHORT(&idb->hardware[2]) == ds->bmac_address[1]) &&
	(GETSHORT(&idb->hardware[0]) == ds->bmac_address[0])) 
      /* mac addr has not changed.  hooray! */
      return;

    if (ds->bsi_bmac_reset) {
	bprintf("bsi_bmac_reset %d\n", ds->bsi_bmac_reset);
	ds->bsi_bmac_reset = FALSE;
    }
    else 
      bprintf("changing MAC addr from %e to %e\n",
	      ds->bmac_address, idb->hardware);

    /*
     * Update our copy of the active node address
     */
    ds->bmac_address[0] = GETSHORT(&idb->hardware[0]);
    ds->bmac_address[1] = GETSHORT(&idb->hardware[2]);
    ds->bmac_address[2] = GETSHORT(&idb->hardware[4]);

    /*
     * Stop the BMAC, and then wait a while so the BMAC can stop:
     * aw heck, Scott, you don't have to wait, the bmac is fast.
     * i am commenting it out for now.  no wastetimes in level4.
     */
    old_mr = nim->bmac.mr;
    bprintf("mr %#x, stopping BMAC...", old_mr);
    nim->bmac.mr = old_mr & (~BMAC_MR_RUN);
    
    /*
     * Bit swap each byte of canonical MAC address for
     * FDDI wire order address:
     */
    wrote = BITSWAP(idb->hardware[0]);
	if (wrote & 0x80) {
	    errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		   "Error: MS bit of bitswapped idb->hardware was set",
		   "wrote", wrote);
	    /*
	     * Bit 47 of my_long_address must always be clear:
	     */
	    wrote &= 0x7F;
	}
    nim->bmac.mla0 = wrote;
    read = nim->bmac.mla0;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.mla0",
	       "wrote", wrote, "read", read);
    }
    wrote = BITSWAP(idb->hardware[1]);
    nim->bmac.mla1 = wrote;
    read = nim->bmac.mla1;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.mla1",
	       "wrote", wrote, "read", read);
    }
    wrote = BITSWAP(idb->hardware[2]);
    nim->bmac.mla2 = wrote;
    read = nim->bmac.mla2;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.mla2",
	       "wrote", wrote, "read", read);
    }
    nim->bmac.mla3 = BITSWAP(idb->hardware[3]);
    wrote = BITSWAP(idb->hardware[3]);
    nim->bmac.mla3 = wrote;
    read = nim->bmac.mla3;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.mla3",
	       "wrote", wrote, "read", read);
    }
    wrote = BITSWAP(idb->hardware[4]);
    nim->bmac.mla4 = wrote;
    read = nim->bmac.mla4;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.mla4",
	       "wrote", wrote, "read", read);
    }
    wrote = BITSWAP(idb->hardware[5]);
    nim->bmac.mla5 = wrote;
    read = nim->bmac.mla5;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.mla5",
	       "wrote", wrote, "read", read);
    }
    wrote = BMAC_OPTION_ELA;
    nim->bmac.option = wrote;
    read = nim->bmac.option;
    if (read != wrote) {
	errmsg(&msgsym(FDDIFAIL2, FDDI), idb->hw_namestring,
	       "Error while writing bmac.option",
	       "wrote", wrote, "read", read);
    }
    
    /*
     * The following group address setup is required to work around some
     * bloody braindead design "decisions" by airhead National Semiconductor
     * chip engineers. Touch these settings at your peril.
     */

    /*
     * Enable reception of the broadcast address:
     * databook p 2-182
     */
    nim->bmac.fgm1 |= 0x80;
   
    /*
     * Set the BMAC GLA so it becomes passive (we use the CAM as a group
     * address filter as the GLA on the BMAC is worse than useless)
     */
    gla[0] = gla[1] = gla[2] = gla[3] = gla[4] = gla[5] = 0xFF;
    bmac_set_gla(ds->module_regs, gla);

#if BMAC_LOOPBACK
    nim->bmac.mr |= BMAC_MR_ILB; /* If BMAC loopback desired */
    BSI_DEBUG(buginf("  BMAC LOOPBACK ENABLED\n");
#endif
    /*
     * Only start up the BMAC if it was already running:
     */
    if (old_mr & BMAC_MR_RUN) {
	bprintf("starting BMAC\n");
	nim->bmac.mr |= BMAC_MR_RUN;
    } else {
	bprintf("leaving BMAC stopped\n");
    }
}

/*
 * bsi_setup_mac
 * Update SMT MIB MAC address for correct SMT operation.
 */

static void bsi_setup_mac (hwidbtype *idb)
{
    SMTMessage msg;

    BSI_DEBUG(buginf("bsi_setup_mac(): idb 0x%#x\n", idb));
    msg.destination = MIB_MSG_ID;
    msg.source = MAP_MSG_ID;
    msg.type = MIB_ACTION_CHANGE_MACADDR;
    msg.stnID = idb->fddi_stn_id;
    msg.entity = 0;
    MCopyAddress (msg.data.addr, &idb->bit_swapped_hardware);

    SendMAPMessage(&msg);
}

/*
 * Initialize an FDDI interface. Lower level routine.
 * This routine checks for the administratively down state.
 */

boolean bsi_initialize (hwidbtype *idb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;
    int			i;
    leveltype		level;
    idbtype            *swidb;
    boolean filter    = FALSE;  /* Stupid compiler... */
    ushort		stn_id;

    BSI_DEBUG(SAVE_CALLER());
        
    stn_id = idb->fddi_stn_id;
    ds = INSTANCE;
    bprintf("**bsi init** ");
    BSI_DEBUG(buginf("bsi_initialize(): from %#x, idb=%#x, idb->state=%d\n",
		     caller(), idb, idb->state));
    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;
    ds->board_reset = FALSE;

    /*
     * Only overwrite idb->hardware if it is NULL:
     */
    if (GETSHORT(&idb->hardware[4]) == 0 &&
	GETSHORT(&idb->hardware[2]) == 0 &&
	GETSHORT(&idb->hardware[0]) == 0) {
	ieee_copy(idb->bia, idb->hardware);
    }

    /*
     * Set up the bitswapped address for the fastswitching code
     */
    ieee_swap(idb->hardware, idb->bit_swapped_hardware);

    idb->counters.resets++;		/* Count the resets */

    /*
     * Let the SMT MIB know about the address
     */
    bsi_setup_mac(idb);

    /*
     * If we are administratively shut down, return:
     */
    if (idb->state == IDBS_ADMINDOWN) {
	BSI_DEBUG(buginf("bsi_initialize(): Interface marked "
			  "as IDBS_ADMINDOWN\n"));
	return(TRUE);
    }
    
    /*
     * If we are already initializing, return:
     */
    if (idb->state == IDBS_INIT) {
	BSI_DEBUG(buginf("bsi_initialize(): Interface marked "
			  "as IDBS_INIT\n"));
	return(TRUE);
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Safely shut down FDDI DMA hardware:
     */
    bsi_stop(idb);		/* This also reads bsi_rev */
    TIMER_STOP(idb->xmittimer);

    /*
     * If the driver was down, reset the whole NIM,
     * unless we are beaconing! 
     */
    if ( (idb->state == IDBS_DOWN) && !ds->beacontype ) {
	fddibuginf("\nFDDI(%d): network interface module reset", idb->unit);
	bprintf("\nfddi nim reset\n");
	/* be nice and nuke CMT before resetting nim! */
        fddi_disconnect (stn_id);
	nim->csr0 |= CSR0_NIM_RESET;
	WASTETIME(ds->delay_10_usec);
	nim->csr0 &= ~CSR0_NIM_RESET;
	WASTETIME(ds->delay_10_usec);
	/* set local flag indicating bsi reset done */
 	ds->board_reset = TRUE;
	/* set instance flag so bmac gets reset iff necessary */
	ds->bsi_bmac_reset = TRUE;
    }

    /* i just moved this from before the bsi_stop() above!  eli */
    net_cstate(idb, IDBS_INIT);

    /*
     * Set the MAC address and/or reinit CMT if needed.
     */
    bsi_bmac_initialize(idb);

    /*
     * Set up the saved IDB variable, the 68k interrupt vector,
     * and initialize the vector register on the NIM:
     */
    if (!init_bsi_dev(idb) ) {
        errmsg(&msgsym(BADSLOT, FDDI), idb->unit, idb->slot);
        reset_interrupt_level(level);
        return(FALSE);
    }
    /*
     * Are we running transparent bridging? 
     * This is true if the media type is BR_FDDI and
     * we are a member of a spanning tree
     */
    ds->cam_learning = FALSE;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	if (!(system_loading | bootstrap_enable)) {
	    filter = RUNNING_IF(LINK_BRIDGE,swidb);
	    if (idb->firstsw->tbridge_media == TBR_FDDI) {
		if (filter && bridge_enable)
		    ds->cam_learning = TRUE;
		reg_invoke_tbridge_smf_update(idb->hardware,
					      swidb,
					      SMF_MACTYP_OWN_UNICAST |
					      SMF_ACTION_RECEIVE);
	    } else if (idb->firstsw->tbridge_media == TBR_FDDI_TRANSIT) {
		reg_invoke_tbridge_smf_update(idb->hardware,
					      swidb,
					      SMF_MACTYP_OWN_UNICAST |
					      SMF_ACTION_RECEIVE);
	    }
	}
    }
    
    /*
     * Tell the BSI where the mailbox is:
     */
    {
	charlong		mbox_ptr;
	
	mbox_ptr.d.lword = (int)&ds->ioram->mbox.d.lword;
	bsi->pcar = 0;
	for (i = 0; i < 4; i++) {
	    bsi->mbar = mbox_ptr.d.byte[i];
	}
    }
    
    /*
     * Initialize Mode register:
     * Big Endian for 68k, small (1 KB) queue size, and small bursts:
     */
    bsi->mr = (BSI_MR_BIGEND | BSI_MR_SMLQ | BSI_MR_SMLB);
    
    /*
     * Reset queue pointers, BSI pointer RAM, and BSI limit RAM:
     */
    
    /*
     * Recollect FDDI page buffers from the BSI iff pak->if_input is BSI_MAGIC:
     */
    {
	paktype	*pak;

	for (i = 0; i < bsi_page_count; i++) {
	    /*
	     * Reclaim all pages that are not already on the free
	     * queue, and are not on an input queue:
	     */
	    pak = bsi_page_table[i];
	    if (pak->if_input == (idbtype *)BSI_MAGIC + idb->unit) {
		pak->if_input = NULL;
		retbuffer(pak);
	    }
	}
    }

    /*
     * Initialise Request channels
     */
    bsi_init_req_channel(ds, &ds->rq0);
    bsi_init_req_channel(ds, &ds->rq1);
    
    /*
     * Initialise Indicate channels
     */
    ds->buf_needed = 0;
    bsi_init_ind_channel(ds, &ds->ic0);
    bsi_init_ind_channel(ds, &ds->ic1);
    bsi_init_ind_channel(ds, &ds->ic2);
    
    /*
     ********************************
     * Set the Event Notify registers:
     ********************************
     */
    
    /*
     * Enable all types of attention events, except service attentions:
     */
    bsi->mnr = BSI_MNR_STAN | BSI_MNR_NSAN |
	BSI_MNR_RQAN | BSI_MNR_INAN;
    
    /*
     * Enable state attention events for non recoverable errors,
     * and command errors:
     */
    bsi->stnr = BSI_STNR_ERR | BSI_STNR_CMDE;
    
    /*
     * Disable all service notify events:
     */
    bsi->snr = 0;
    
    /*
     * Enable applicable no space attention events:
     */
    bsi->nsnr = BSI_NSNR_LDI0N | BSI_NSNR_NSI0N |
	        BSI_NSNR_LDI1N | BSI_NSNR_NSI1N |
		BSI_NSNR_LDI2N | BSI_NSNR_NSI2N;
    bsi->nsnr |= BSI_NSNR_NSR1N;
    
    /*
     * Enable applicable request attention events for req 0:
     */
    bsi->rnr = BSI_RNR_USRR0N | BSI_RNR_EXCR0N | BSI_RNR_BRKR0N;

    /*
     * Configure req 0 for Source Address Transparency, Hold and No prestage.
     */
    bsi->r0cr = BSI_RxCR_SAT | BSI_RxCR_FCT | BSI_RxCR_HLD;

    switch (ds->bsi_rev.d.lword) {
      case BSI_SILICON_BSI_1A:
      case BSI_SILICON_BSI_1B:
	/*
	 * We have a BSI-1. Set the Transmit Threshold to 16 words
	 */
	bsi->r0cr |= BSI_RxCR_TT_16;
	break;
      case BSI_SILICON_BSI_2A:
      case BSI_SILICON_BSI_2B:
	/*
	 * We have a BSI-2. Take advantage of the larger Transmit
	 * Threshold to avoid underruns when transmiting large packets
	 */
	bsi->r0cr |= BSI_RxCR_TT_128;
	break;
      default:
	break;
    }

    /*
     * Enable applicable request attention events for req 1:
     */
    bsi->rnr |= BSI_RNR_USRR1N | BSI_RNR_EXCR1N | BSI_RNR_BRKR1N;

    /*
     * Configure req 1 for Void Stripping, Source Address Transparency,
     * and Frame Control Transparency
     *
     * Turning off the HLD bit means that we enable Pre-staging on
     * this channel, which will potentially reduce the risk of
     * the BSI/BMAC transmit sleep bug. Set Tx fifo watermark to 16 words
     * for BSI-1, or 128 for BSI-2
     */
    bsi->r1cr = BSI_RxCR_VST | BSI_RxCR_SAT | BSI_RxCR_FCT;
    
    switch (ds->bsi_rev.d.lword) {
      case BSI_SILICON_BSI_1A:
      case BSI_SILICON_BSI_1B:
	/*
	 * We have a BSI-1. Set the Transmit Threshold to 16 words
	 */
	bsi->r1cr |= BSI_RxCR_TT_16;
	break;
      case BSI_SILICON_BSI_2A:
      case BSI_SILICON_BSI_2B:
	/*
	 * We have a BSI-2. Take advantage of the larger Transmit
	 * Threshold to avoid underruns when transmiting large packets
	 */
	bsi->r1cr |= BSI_RxCR_TT_128;
	break;
      default:
	break;
    }

    /*
     * Enable indicate attention events:
     */
    bsi->inr = BSI_INR_EXC0N | BSI_INR_BRK0N |
	       BSI_INR_EXC1N | BSI_INR_BRK1N |
               BSI_INR_EXC2N | BSI_INR_BRK2N;
    
    /*
     * Disable the request expected frame status mechanism,
     * since we're not using full confirmation:
     */
    bsi->r0efsr = bsi->r1efsr = 0;
    
    /*
     * Set the Indicate Configuration register:
     * Note that IC0 is used for MAC and SMT frames only,
     * IC1 is used for packets addressed to us (including keepalives),
     * and IC2 is where externally matched addresses go. IC2 is used
     * to receive multicasts when routing and encapsulated bridging and to
     * receive bridgeable packets whilst transparent bridging. Only receive
     * our own packets if we have 'loopback' configured on the interface:
     */
    bsi->icr = (BSI_ICR_COPY_OURS | (BSI_ICR_COPY_OURS << BSI_ICR_CC0_SHIFT));
    if (idb->loopback) {
	bsi->icr |= (BSI_ICR_COPY_OURS_HEARSELF << BSI_ICR_CC1_SHIFT);
	idb->status |= IDB_HEARSELF;
    } else {
	bsi->icr |= (BSI_ICR_COPY_OURS << BSI_ICR_CC1_SHIFT);
	idb->status &= ~IDB_HEARSELF;
    }
    
    /*
     * Set the Indicate Mode register
     * We're using the external/internal sort mode, so MAC and SMT frames
     * go into ICH0, internally matched packets go into ICH1, and externally
     * matched packets go into ICH2. Frame per page mode is enabled, so each
     * frame is on its own page.  Skip mode is disabled, so duplicate MAC
     * or SMT frames will be received. We've also enabled the
     * indicate thresholds for channels 1 and 2:
     */
    bsi->imr = ((BSI_IMR_SM_EXTRN_INTRN << BSI_IMR_SM_SHIFT) |
		BSI_IMR_FRAM_PAGE | BSI_IMR_BOT1 | BSI_IMR_BOT2 |
		BSI_IMR_BOS);

    /*
     * Set the Indicate Threshold register to 1 for now:  *CJS*
     */
    bsi->itr = 1;
    
    /*
     * Read and then clear the Attention Registers:
     */
    i = bsi->rar;
    bsi->rar = 0x00;
    i = bsi->iar;
    bsi->iar = 0x00;
    i = bsi->nsar;
    bsi->nsar = 0;

    /*
     * Put the BSI into Run Mode:
     */
    i = bsi->star;
    bsi->star = 0x00;
    WASTETIME(ds->delay_10_usec); /* Give the BSI a while to start up */
    
    /*
     * If BSI stayed running, we're up, so enable interrupts if we stop:
     */
    {
	uchar	star;
	
	star = bsi->star;
	if (star & BSI_STAR_STOPBITS) {
	    /*
	     * BSI didn't stay running. We're down:
	     */
	    net_cstate(idb, IDBS_DOWN);
	    errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
		   "BSI failed to run", "star", star);
	} else {
	    ds->bsi_ok = TRUE;
            if (CSPDATA (stn_id, stationData.cfState) != CF_isolated) {
                net_cstate(idb, IDBS_UP);
            } else {
	        /*
	         * Use this state so that further calls to bsi_initialize
	         * can succeed, and so that we won't send packets through
	         * the interface until the ring comes up:
	         */
	        net_cstate(idb, IDBS_TESTING);
	        bsi->stnr |= BSI_STNR_STOPBITS;	/* Enable interrupt on stops */
	    }
	}
    }

    if (ds->cam_present) {
	if (ds->board_reset) {
	    /*
	     * If we're downloading from serial ROM then it'll still
	     * be downloading. This is an ugly fix but we only use 
	     * serial ROM for hardware development.
	     */
	    if (ds->cam_state == CAM_FIRMWARE_ROM) {
		/*
		 * Delay for a second
		 */
		DELAY(ONESEC);
		
		if (!touch_device((void *)&ds->module_regs->lcr))
		    crashdump(0);
	    }

	    config_xilinx(ds->module_regs);
	}
	config_cam(ds->module_regs);  
    }

    /*
     * Clear out all the old multicasts for this interface
     */
    bsi_reset_multicast_addresses(idb);

    /*
     * Configure multicast addresses for interface
     */
    if (!(system_loading | bootstrap_enable)) {
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    reg_invoke_media_interesting_addresses(swidb);
	}
    }

    /*
     * Tell the Xilinx CAM FPGA about the current state
     */
    bsi_set_cam_mode(ds);

    if (ds->board_reset && STADATA (stn_id, smtUp))
	init_cmt(idb); 

    reset_interrupt_level(level);
    return(TRUE);
}


/*
 * bsi_shutdown
 * Shut down the FDDI interface
 */

static void bsi_shutdown (hwidbtype *idb, int newstate)
{
    leveltype level;

    SAVE_CALLER();

    BSI_DEBUG(buginf("bsi_shutdown(): idb = 0x%x\n", idb));
    level = raise_interrupt_level(NETS_DISABLE);
    holdq_reset(idb);
    bsi_stop(idb);
    TIMER_STOP(idb->xmittimer);
    fddi_disconnect (idb->fddi_stn_id);

    fddibuginf("\nFDDI(%d): interface shutdown to state %s, from %#x\n",
	       idb->unit, print_hwidbstate(idb), caller());

    net_cstate(idb, newstate);

    BypassRequestMulti (idb->fddi_stn_id, INSERTED);

    reset_interrupt_level(level);
}

/*
 * bsi_output
 * Start or restart output to the FDDI interface
 * Called from process level *and* from interrupt level
 */

static void bsi_output (hwidbtype *idb)
{
    bsi_instance_t	*ds;	/* Driver instance structure */
    paktype		*pak;	/* Current packet being output */
    int			size = 0; /* Size of current packet */
    leveltype		level;	/* Original interrupt level */

    ds = INSTANCE;

    /*
     * If the transmitter is hung, reset the interface before beginning:
     */
    level = raise_interrupt_level(NETS_DISABLE); /* Lock out net interrupts */
    if (TIMER_RUNNING_AND_AWAKE(idb->xmittimer)) {
	BSI_DEBUG(buginf("AWAKE: idb->xmittimer %d\n",
			 ELAPSED_TIME(idb->xmittimer)));
	buginf("\n%s: output hung, restarting fddi%d controller, count %d",
	       idb->hw_namestring, idb->unit, ds->tx_restarts);
	GET_TIMESTAMP(idb->lastreset);
	ds->tx_restarts++;
	bsi_initialize(idb);
    }
    reset_interrupt_level(level);
    
    /*
     * Keep outputing as long as the interface is up,
     * we have more packets to transmit, and there is room
     * in the output request queue:
     */
    while (idb->state == IDBS_UP) {
	/*
	 * Find a legal sized packet to send.
	 * Please note that we use raise_interrupt_level and
	 * reset_interrupt_level to renable network interrupts between each
	 * output packet so that we don't block out processing of input
	 * network traffic while draining a large output hold queue. We have
	 * interrupts disabled while we dequeue each packet. This way, if we
	 * are interrupted after processing a packet, and if bsi_output() is
	 * called again from interrupt level, it will get the correct next
	 * packet to process. This allows us to drain the output hold queue
	 * at interrupt level while we are processing a large burst of fast
	 * switched traffic that is going out on this interface via the call
	 * to bsi_output in bsi_fastsend().
	 */
	level = raise_interrupt_level(NETS_DISABLE); /*No network interrupts */
	while ((pak = (*idb->oqueue_dequeue)(idb))) {
	    /* 
	     * Prevent too large packets from being sent:
	     */
	    size = pak->datagramsize;
	    if (size > idb->buffersize) {
		errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, size);
		datagram_done(pak);
		continue;
	    }
	    break;
	}
	if (pak == NULL) {	/* None to transmit */
	    reset_interrupt_level(level);
	    return;
	}

	{
	    req_descr_t	*req_descr;	/* Current output descriptor */
	    txd_t	*oduds_ptr;

	    if (fddi_debug) 
		bsi_dump_packet("bsi_output", pak->datagramstart, 5);
									    
	    /*
	     * Make sure we've got room in the request descriptor
	     * queue and in the oduds structure/pak ptr array:
	     */
	    if ((!BSIQ_FULL(ds->REQ_CHAN.req_ring)) &&
		(!ds->REQ_CHAN.pak_ring.ptr->addr)) {
		uint	pak_orig, pak_end;
		uint	orig_pg, end_pg;
		union {
		    req_descr_t	rd;
		    uint	lword[2];
		} u;
		
		/*
		 * Save a pointer to this packet (also marks slot as in use):
		 */
		ds->REQ_CHAN.pak_ring.ptr->addr = pak;

		/*
		 * Initialize request descriptor for odud1 in oduds:
		 */
		u.lword[0] = u.lword[1] = 0;	/* start clean */
		/*
		 * UID is index into oduds
		 */
		u.rd.uid = ds->REQ_CHAN.req.my_ndx;
		u.rd.cnfcls = BSI_REQ_CNFCLS_TEND;
		u.rd.rqcls = BSI_REQ_RQCLS_ASYN;
		u.rd.size = 1;

		oduds_ptr = (txd_t *)ds->REQ_CHAN.odud_ring.ptr->addr;

		SET_BSI_ONLY_DUD(u.rd.loc, &oduds_ptr->odud1);
		ds->REQ_CHAN.req.tx_reqs++; /* another request outstanding */
		
		/*
		 * Get pointer to our request descriptor:
		 */
		req_descr  = (req_descr_t *)ds->REQ_CHAN.req_ring.limit->addr;
		*req_descr = u.rd;
		
		/*
		 * See if this packet spans one or two page boundaries.
		 * If it does span page(s), we'll have to use two
		 * or three ODUDs to transmit this packet:
		 */
		pak_orig = (uint)pak->datagramstart;
		pak_end = pak_orig + (uint)size;
		orig_pg = BSI_4K_PAGE(pak_orig);
		end_pg = BSI_4K_PAGE(pak_end);
		
		/*
		 * Do we have the simple one page case?
		 */
		if (orig_pg == end_pg) {
		    /*
		     * Yes, construct only odud for this packet:
		     */
		    DEBUG_COUNTERS(ds->tx_1odud++);
		    SET_BSI_ONLY_DUD(oduds_ptr->odud1.loc, pak_orig);
		    oduds_ptr->odud1.cnt = (uint)size;
		} else {
		    uint size1, size2, size3, next_pg;
		    
		    /*
		     * No, we must build multiple ODUDs to describe this
		     * output packet, because it spans one or two 4 KB
		     * page boundaries:
		     */
		    next_pg = orig_pg + BSI_4KB_PAGE_SIZE;
		    if (next_pg == end_pg) {
			/*
			 * We've got the two ODUD case:
			 */
			DEBUG_COUNTERS(ds->tx_2odud++);

			/* size of packet fragment before page boundary */
			size1 = end_pg - (uint)pak_orig;

			/* size of packet fragment after page boundary */
			size2 = pak_end - end_pg;

			/*
			 * An ODUD with the LAST bit set with zero length
			 * will cause a consistency failure error message:
			 */
			if (size2 != 0) {

			    /*
			     * Set up the first ODUD:
			     */
			    SET_BSI_FIRST_DUD(oduds_ptr->odud1.loc, pak_orig);
			    oduds_ptr->odud1.cnt = size1;
		    
			    /*
			     * Set up the last ODUD:
			     */
			    SET_BSI_LAST_DUD(oduds_ptr->odud2.loc, end_pg);
			    oduds_ptr->odud2.cnt = size2;

			} else {

			    /*
			     * This will cause a consistency failure
			     * output error, so we must fix it:
			     */

			    SET_BSI_ONLY_DUD(oduds_ptr->odud1.loc, pak_orig);
			    oduds_ptr->odud1.cnt = (uint)size;
			}
		    } else {

			/*
			 * We've got the three ODUD case:
			 */
			DEBUG_COUNTERS(ds->tx_3odud++);

			/* Size of first packet fragment */
			size1 = next_pg - (uint)pak_orig;

			/* Second ODUD will always be a full page */
			size2 = BSI_4KB_PAGE_SIZE;

			/* Size of packet fragment on last page */
			size3 = pak_end - end_pg;


			/*
			 * An ODUD with the LAST bit set with zero length
			 * will cause a consistency failure error message:
			 */
			if (size3 != 0) {
			    
			    /*
			     * Set up the first ODUD:
			     */
			    SET_BSI_FIRST_DUD(oduds_ptr->odud1.loc, pak_orig);
			    oduds_ptr->odud1.cnt = size1;
			    
			    /*
			     * Set up the middle ODUD:
			     */
			    SET_BSI_MIDDLE_DUD(oduds_ptr->odud2.loc, next_pg);
			    oduds_ptr->odud2.cnt = size2;
			    
			    /*
			     * Set up the last ODUD:
			     */
			    SET_BSI_LAST_DUD(oduds_ptr->odud3.loc, end_pg);
			    oduds_ptr->odud3.cnt = size3;
			    
			} else {
			    
			    /*
			     * This will cause a consistency failure output
			     * error, so we must fix it:
			     */
			    /*
			     * Set up the first ODUD:
			     */
			    SET_BSI_FIRST_DUD(oduds_ptr->odud1.loc, pak_orig);
			    oduds_ptr->odud1.cnt = size1;
			    
			    /*
			     * Set up the last ODUD:
			     */
			    SET_BSI_LAST_DUD(oduds_ptr->odud2.loc, end_pg);
			    oduds_ptr->odud2.cnt = size2;
			}
		    }
		}
		
		/*
		 * Do housekeeping after giving hardware the packet:
		 */
		idb->counters.outputs++;	/* count total output */
		idb->counters.tx_cumbytes += size; /* total bytes sent */
		/*
		 * If not already set, start timer
		 * */
		if (!TIMER_RUNNING(idb->xmittimer))
		    TIMER_START(idb->xmittimer, TIMEOUT); /* three seconds */

		/*
		 * Account for output broadcasts
		 */
		if (((char *)pak_orig)[1] & FDDI_GROUP)
		    idb->counters.output_broad++;

		/*
		 * Update limit RAM so BSI knows it has more work to do:
		 */
		bsi_write_lram_inline(ds, ds->REQ_CHAN.req.lmt_index,
				      BSI_PTR_TO_LIMIT(ds->REQ_CHAN.req_ring.limit->addr));
		ds->REQ_CHAN.req_ring.limit =
		    ds->REQ_CHAN.req_ring.limit->next;

		/*
		 * Point my_ndx to next oduds record. Handle wrap:
		 */
		ds->REQ_CHAN.req.my_ndx =
		    ++ds->REQ_CHAN.req.my_ndx & BSI_REQ_NDX_SIZE;
		ds->REQ_CHAN.pak_ring.ptr = ds->REQ_CHAN.pak_ring.ptr->next;
		ds->REQ_CHAN.odud_ring.ptr = ds->REQ_CHAN.odud_ring.ptr->next;
		
	    } else {
		/*
		 * oduds[] is full!
		 * We failed to transmit it, so requeue the packet
		 * on the head of the queue, and return, so that
		 * we don't waste any time trying to fill a full
		 * output request queue:
		 */
		pak_requeue_head(idb, pak);
		reset_interrupt_level(level);
		return;
	    }
	}
	reset_interrupt_level(level);
    }
}

/*
 * bsi_fastsend
 * Transmit this packet immediately if there are no packets already
 * queued for this interface, or queue it at the end of the output
 * hold queue. Called from interrupt level.
 */

static void bsi_fastsend (hwidbtype *idb, paktype *pak)
{
    bsi_instance_t	*ds;		/* Driver instance structure */
    req_descr_t		*req_descr;	/* Current output descriptor */
    txd_t		*oduds_ptr;
    traffic_shape_t     *shape;		/* shaping bucket */
    ushort              bytes;
    ulong               acct_proto;
    
    ds = INSTANCE;

    bytes = pak->datagramsize;
    acct_proto = (ulong)pak->acct_proto;

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * If no packets are queued for output on this interface,
     * and if the interface is up, immediately attempt to
     * transmit the packet:
     */
    if ((idb->state == IDBS_UP) && (!idb->output_qcount)) {

#if DEBUG_PKT_FAST
	buginf("bsi_fastsend(): idb %#x, pak %#x\n", idb, pak);
	bsi_debug_dump_packet(pak->datagramstart, 30);
#endif
	if (fddi_debug) 
	    bsi_dump_packet("bsi_fastsend", pak->datagramstart, 5);

	/*
	 * Make sure we've got room in the request descriptor
	 * queue and in the oduds structure/pak ptr array:
	 */
	if (!(BSIQ_FULL(ds->REQ_CHAN.req_ring)) &&
	    (!ds->REQ_CHAN.pak_ring.ptr->addr)) {

	    uint	pak_orig, pak_end;
	    uint	orig_pg, end_pg;
	    union {
		req_descr_t	rd;
		uint		lword[2];
	    } u;
	    
	    /*
	     * Save a pointer to this packet (also marks slot as in use):
	     */
	    ds->REQ_CHAN.pak_ring.ptr->addr = pak;

	    /*
	     * Initialize request descriptor for odud1 in oduds:
	     */
	    u.lword[0] = u.lword[1] = 0;	/* start clean */
	    u.rd.uid = ds->REQ_CHAN.req.my_ndx; /* UID is unique 6-bit stamp */
	    u.rd.cnfcls = BSI_REQ_CNFCLS_TEND;
	    u.rd.rqcls = BSI_REQ_RQCLS_ASYN;
	    u.rd.size = 1;

	    oduds_ptr = (txd_t *)ds->REQ_CHAN.odud_ring.ptr->addr;
	    SET_BSI_ONLY_DUD(u.rd.loc, &oduds_ptr->odud1);

	    ds->REQ_CHAN.req.tx_reqs++; /* Another request outstanding */
	    
	    /*
	     * Get pointer to our request descriptor:
	     */
	    req_descr  = (req_descr_t *)ds->REQ_CHAN.req_ring.limit->addr;
	    *req_descr = u.rd;
	    
	    /*
	     * See if this packet spans one or two page boundaries.
	     * If it does span page(s), we'll have to use two
	     * or three ODUDs to transmit this packet:
	     */
	    pak_orig = (uint)pak->datagramstart;
	    pak_end = pak_orig + (uint)bytes;
	    orig_pg = BSI_4K_PAGE(pak_orig);
	    end_pg = BSI_4K_PAGE(pak_end);
	    
	    /*
	     * Do we have the simple one page case?
	     */
	    if (orig_pg == end_pg) {
		/*
		 * Yes, construct only odud for this packet:
		 */
		DEBUG_COUNTERS(ds->tx_1odud++);
		SET_BSI_ONLY_DUD(oduds_ptr->odud1.loc, pak_orig);
		SET_BSI_DUD_CNT(&oduds_ptr->odud1, (uint)bytes);

	    } else {
		uint size1, size2, size3, next_pg;
		
		/*
		 * No, we must build multiple ODUDs to describe this
		 * output packet, because it spans one or two 4 KB
		 * page boundaries:
		 */
		next_pg = orig_pg + BSI_4KB_PAGE_SIZE;
		if (next_pg == end_pg) {
		    /*
		     * We've got the two ODUD case:
		     */
		    DEBUG_COUNTERS(ds->tx_2odud++);

		    /* size of packet fragment before page boundary */
		    size1 = end_pg - pak_orig;

		    /* size of packet fragment after page boundary */
		    size2 = pak_end - end_pg;

		    /*
		     * An ODUD with the LAST bit set with zero length
		     * will cause a consistency failure error message:
		     */
		    if (size2 != 0) {

			/*
			 * Set up the first ODUD:
			 */
			SET_BSI_FIRST_DUD(oduds_ptr->odud1.loc, pak_orig);
			SET_BSI_DUD_CNT(&oduds_ptr->odud1, size1);
		
			/*
			 * Set up the last ODUD:
			 */
			SET_BSI_LAST_DUD(oduds_ptr->odud2.loc, end_pg);
			SET_BSI_DUD_CNT(&oduds_ptr->odud2, size2);

		    } else {

			/*
			 * This will cause a consistency failure output
			 * error, so we must fix it:
			 */
			SET_BSI_ONLY_DUD(oduds_ptr->odud1.loc, pak_orig);
			SET_BSI_DUD_CNT(&oduds_ptr->odud1, (uint)bytes);

		    }
		} else {

		    /*
		     * We've got the three ODUD case:
		     */
		    DEBUG_COUNTERS(ds->tx_3odud++);

		    /* Size of packet fragment before first page boundary */
		    size1 = next_pg - pak_orig;

		    /* Second ODUD will always be a full page in this case */
		    size2 = BSI_4KB_PAGE_SIZE;

		    /* Size of packet fragment on last page */
		    size3 = pak_end - end_pg;

		    /*
		     * An ODUD with the LAST bit set with zero length
		     * will cause a consistency failure error message:
		     */
		    if (size3 != 0) {

			/*
			 * Set up the first ODUD:
			 */
			SET_BSI_FIRST_DUD(oduds_ptr->odud1.loc, pak_orig);
			SET_BSI_DUD_CNT(&oduds_ptr->odud1, size1);
		
			/*
			 * Set up the middle ODUD:
			 */
			SET_BSI_MIDDLE_DUD(oduds_ptr->odud2.loc, next_pg);
			SET_BSI_DUD_CNT(&oduds_ptr->odud2, size2);

			/*
			 * Set up the last ODUD:
			 */
			SET_BSI_LAST_DUD(oduds_ptr->odud3.loc, end_pg);
			SET_BSI_DUD_CNT(&oduds_ptr->odud3, size3);

		    } else {

			/*
			 * This will cause a consistency failure output
			 * error, so we must fix it:
			 */
			/*
			 * Set up the first ODUD:
			 */
			SET_BSI_FIRST_DUD(oduds_ptr->odud1.loc, pak_orig);
			SET_BSI_DUD_CNT(&oduds_ptr->odud1, size1);
		
			/*
			 * Set up the last ODUD:
			 */
			SET_BSI_LAST_DUD(oduds_ptr->odud2.loc, end_pg);
			SET_BSI_DUD_CNT(&oduds_ptr->odud2, size2);

		    }
		}
	    }
	    
	    /*
	     * Do housekeeping after giving hardware the packet:
	     */
	    idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
	    idb->counters.outputs_fast[acct_proto]++;

	    /*
	     * Increment the ring pointers and ndx id stamp
	     */
	    ds->REQ_CHAN.req.my_ndx =
		++ds->REQ_CHAN.req.my_ndx & BSI_REQ_NDX_SIZE;

	    ds->REQ_CHAN.pak_ring.ptr = ds->REQ_CHAN.pak_ring.ptr->next;
	    ds->REQ_CHAN.odud_ring.ptr = ds->REQ_CHAN.odud_ring.ptr->next;

	    /*
	     * Account for output broadcasts
	     */
	    if (((char *)pak_orig)[1] & FDDI_GROUP)
		idb->counters.output_broad++;

	    /*
	     * Update limit RAM so BSI knows it has more work to do:
	     */
	    bsi_write_lram_inline(ds, ds->REQ_CHAN.req.lmt_index,
				  BSI_PTR_TO_LIMIT(ds->REQ_CHAN.req_ring.limit->addr));
	    ds->REQ_CHAN.req_ring.limit = ds->REQ_CHAN.req_ring.limit->next;
	    
	} else {
	    /*
	     * oduds[] is full! So, queue the packet on the output
	     * hold queue for later transmission:
	     */
	    DEBUG_COUNTERS(ds->tx_full++);

	    /*
	     * We failed to transmit it, so queue the packet
	     * at the tail of the queue:
	     */
	    holdq_enqueue_fast(idb, pak);

	    /*
	     * Process any completed TX requests to make room for this packet:
	     */
	    bsi_proc_cnf(ds, &ds->REQ_CHAN);
	    bsi_output(idb);
	}
    } else {
	
	/*
	 * There are already packets on the output queue, or the
	 * interface is not yet up, so add this packet at the end
	 * of the queue to minimize out of order situations:
	 */
	holdq_enqueue_fast(idb, pak);
	bsi_output(idb);
    }
}

/*
 * bsi_setup_bridging
 * Setup the FDDI interface to know whether it is bridging or
 * routing each protocol.
 */

static void bsi_setup_bridging (hwidbtype *idb)
{
    BSI_DEBUG(buginf("bsi_setup_bridging(): idb 0x%#x\n", idb));

    bsi_initialize(idb);
    if (idb->counters.resets > 0)
	idb->counters.resets--;
}

/*
 * bsi_periodic
 * Perform background tasks needed for bsi FDDI driver
 */

static void bsi_periodic (hwidbtype *idb)
{
    bsi_instance_t *ds;

    BSI_DEBUG(buginf("bsi_periodic(), idb=%#x\n", idb));

    ds = INSTANCE;

    /*
     * If not shut down, do periodic functions:
     */
    if (idb->state != IDBS_ADMINDOWN) {
	bsi_check_disabled_psp(idb);
	if (!ds->bsi_ok)
	    (*idb->reset)(idb);	/* Re-initialize the interface */
        if (ds->psp_unknown_use_err > 5) {
            (*idb->reset)(idb);
            ds->psp_unknown_use_err = 0;
	}
    }
}


static void bsi_show_Req_chan (req_chan_t *rc, char *name)
{
    printf("\n  %s Conf 0x%08x, ptr 0x%08x, limit 0x%08x",
	   name, rc->cnf.base, rc->cnf_ring.ptr->addr,
	   rc->cnf_ring.limit->addr);
    printf("\n     VRing 0x%08x, ptr 0x%08x, limit 0x%08x",
	   rc->cnf_ring.ring, rc->cnf_ring.ptr, rc->cnf_ring.limit);
    printf("\n     ptr_index %#x, lmt_index %#x",
	   rc->cnf.ptr_index, rc->cnf.lmt_index);
    printf("\n     Req 0x%08x, ptr 0x%08x, limit 0x%08x, tx_reqs %d, my_ndx %d",
	   rc->req.base, rc->req_ring.ptr->addr, rc->req_ring.limit->addr,
	   rc->req.tx_reqs, rc->req.my_ndx);
    printf("\n     VRing 0x%08x, ptr 0x%08x, limit 0x%08x",
	   rc->req_ring.ring, rc->req_ring.ptr, rc->req_ring.limit);
    printf("\n     ptr_index %#x, lmt_index %#x",
	   rc->req.ptr_index, rc->req.lmt_index);
}

static void bsi_show_Ind_chan (ind_chan_t *ic, char *name)
{
    printf("\n  %s Idud 0x%08x, ptr 0x%08x, limit 0x%08x",
	   name, ic->idud.base, ic->idud_ring.ptr->addr,
	   ic->idud_ring.limit->addr);
    printf("\n     VRing 0x%08x, ptr 0x%08x, limit 0x%08x",
	   ic->idud_ring.ring, ic->idud_ring.ptr, ic->idud_ring.limit);
    printf("\n     ptr_index %#x, lmt_index %#x, max_iduds %d",
	   ic->idud.ptr_index, ic->idud.lmt_index, ic->idud.max_iduds);
    printf("\n     Psp  0x%08x, ptr 0x%08x, limit 0x%08x, count %d",
	   ic->psp.base, ic->psp.ptr, ic->psp.limit, ic->psp.count);
    printf("\n     VRing 0x%08x, ptr 0x%08x, limit 0x%08x",
	   ic->psp_ring.ring, ic->psp_ring.ptr, ic->psp_ring.limit);
    printf("\n     ptr_index %#x, lmt_index %#x, quantum %d, max_psps %d",
	   ic->psp.ptr_index, ic->psp.lmt_index, ic->psp.quantum,
	   ic->psp.max_psps);
}

static void bsi_show_cam (bsi_instance_t *ds)
{
    xx_fddi_regs_t	*nim;

    if (!ds)
	return;
    nim = ds->module_regs;

    switch (ds->cam_state) {
      case CAM_FIRMWARE_ROM:
	printf("\n CAM hardware present (Serial ROM)");
	printf("\n Registers: csr = %#x", nim->csr);
	break;
	
      case CAM_FIRMWARE_SOFT:
	printf("\n CAM hardware present (Software: %s)", &bsi_xilinx_version);
	printf("\n Registers: csr = %#x", nim->csr);
	break;

      case CAM_FIRMWARE_NONE:
	printf("\n CAM hardware not present");
	break;

      case CAM_FIRMWARE_FAIL:
	printf("\n CAM hardware present but failed to initialize");
	break;

      default:
	printf("\n Unknown CAM hardware state");
	break;
    }

    printf ("\n");
}

static void bsi_show_bsi (bsi_instance_t *ds)
{
    xx_fddi_regs_t	*nim;
    bsi_t		*bsi;

    if (!ds)
	return;
    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;

    printf("\n BSI silicon revision Number 0x%08x",
	   ds->bsi_rev.d.lword);
    switch (ds->bsi_rev.d.lword) {
      case BSI_SILICON_BSI_1A:
	printf(" (BSI-1 Rev-A)");
	break;
      case BSI_SILICON_BSI_1B:
	printf(" (BSI-1 Rev-B)");
	break;
      case BSI_SILICON_BSI_2A:
	printf(" (BSI-2 Rev A)");
	break;
      case BSI_SILICON_BSI_2B:
	printf(" (BSI-2 Rev B)");
	break;
      default:
	printf(" (UNKNOWN)");
	break;
    }
    printf("\n Registers: mr %#x, pcar %#x, mbar %#x, mar %#x",
	   bsi->mr, bsi->pcar,
	   bsi->mbar, bsi->mar);
    printf("\n mnr %#x, star %#x, stnr %#x, sar %#x, snr %#x, nsar %#x",
	   bsi->mnr, bsi->star, bsi->stnr,
	   bsi->sar, bsi->snr, bsi->nsar);
    printf("\n nsnr %#x, lar %#x, ldr %#x, rar %#x, rnr %#x, r0cr %#x",
	   bsi->nsnr, bsi->lar, bsi->ldr,
	   bsi->rar, bsi->rnr, bsi->r0cr);
    printf("\n r1cr %#x, r0efsr %#x, r1efsr %#x, iar %#x, inr %#x",
	   bsi->r1cr, bsi->r0efsr, bsi->r1efsr,
	   bsi->iar, bsi->inr);
    printf("\n itr %#x, imr %#x, icr %#x, ihlr %#x, cmp %#x",
	   bsi->itr, bsi->imr, bsi->icr,
	   bsi->ihlr, bsi->cmp);

    printf("\nStructures: mailbox 0x%08x, @0x%08x\n Queues:",
	   ds->ioram->mbox.d.lword, &ds->ioram->mbox.d.lword);

    bsi_show_Req_chan(&ds->rq0, "Rq chn 0");
    bsi_show_Req_chan(&ds->rq1, "Rq chn 1");

    bsi_show_Ind_chan(&ds->ic0, "Ind chn 0");
    bsi_show_Ind_chan(&ds->ic1, "Ind chn 1");
    bsi_show_Ind_chan(&ds->ic2, "Ind chn 2");
}

static void bsi_show_bmac (bsi_instance_t *ds)
{
    xx_fddi_regs_t	*nim;
    bmac_t		*bmac;
    ulong frct,eict,lfct,fcct,fnct,ftct,tkct,rlct;
    ushort stnid;
    
    if (!ds)
	return;
    nim = ds->module_regs;
    bmac = (bmac_t *)&nim->bmac;
    stnid = ds->idb->fddi_stn_id;

    printf("\n BMAC Registers: mr %#x, option %#x, function %#x, rev %#x",
	   bmac->mr, bmac->option, bmac->function, bmac->rev);
    printf("\n cmp %#x, crs0 %#x, cts0 %#x, relr0 %#x, remr0 %x", 
	   bmac->cmp, bmac->crs0, bmac->cts0, bmac->relr0, bmac->remr0);
    printf("\n relr1 %#x, remr1 %#x, telr0 %#x, temr0 %#x, cilr %#x",
	   bmac->relr1, bmac->remr1, bmac->telr0, bmac->temr0, bmac->cilr);
    printf("\n cimr %#x, colr %#x, comr %#x, ielr %#x, esr %#x",
	   bmac->cimr, bmac->colr, bmac->comr, bmac->ielr, bmac->esr);
    printf("\n emr %#x, icr %#x, imr %#x",
	   bmac->emr, bmac->icr, bmac->imr);
    printf("\n mla %02x%02x:%02x%02x:%02x%02x, msa %02x%02x",
	   bmac->mla0, bmac->mla1, bmac->mla2, bmac->mla3,
	   bmac->mla4, bmac->mla5, bmac->msa0, bmac->msa1);
    printf("\n gla %02x%02x%02x%02x%02x, gsa0 %02x",
	   bmac->gla0, bmac->gla1, bmac->gla2,
	   bmac->gla3, bmac->gla4, bmac->gsa0);
    printf("\n tneg0 %#x, tneg1 %#x, tneg2 %#x, tneg3 %#x tmax %#x",
           bmac->tneg0, bmac->tneg1, bmac->tneg2, bmac->tneg3,
           bmac->tmax);
    printf("\n tbt0 %#x, tbt1 %#x, tbt2 %#x, tbt3 %#x",
	   bmac->tbt0, bmac->tbt1, bmac->tbt2, bmac->tbt3);
    printf("\n sgm0 %#x, sgm1 %#x",
	   bmac->fgm0, bmac->fgm1);

    /* must read low order first, according to databook 2-190 */
    /* similar code in xdi.c getmacattr */
    frct = (ulong)(bmac->frct3);
    frct += (ulong)((bmac->frct1 << 16) + (bmac->frct2 << 8));
    eict = (ulong)bmac->eict3;
    eict += (ulong)((bmac->eict1 << 16) + (bmac->eict2 << 8));
    lfct = (ulong)bmac->lfct3;
    lfct += (ulong)((bmac->lfct1 << 16) + (bmac->lfct2 << 8));
    fcct = (ulong)bmac->fcct3;
    fcct += (ulong)((bmac->fcct1 << 16) + (bmac->fcct2 << 8));
    fnct = (ulong)bmac->fnct3;
    fnct += (ulong)((bmac->fnct1 << 16) + (bmac->fnct2 << 8));
    ftct = (ulong)bmac->ftct3;
    ftct += (ulong)((bmac->ftct1 << 16) + (bmac->ftct2 << 8));
    tkct = (ulong)bmac->tkct3;
    tkct += (ulong)((bmac->tkct1 << 16) + (bmac->tkct2 << 8));

    printf("\n\n  BMAC counts: frames %u, isolated errs %u, lost %u, \n"
	   "               copied %u, notcopied %u, "
	   " transmit %u, token %u\n",
           MIBDATA (stnid, mib->macBase[0].hwData.Frame_Ct) + frct,
           MIBDATA (stnid, mib->macBase[0].hwData.Error_Ct) + eict,
           MIBDATA (stnid, mib->macBase[0].hwData.Lost_Ct) + lfct,
           MIBDATA (stnid, mib->macBase[0].hwData.Copied_Ct) + fcct,
           MIBDATA (stnid, mib->macBase[0].hwData.NotCopied_Ct) + fnct,
           MIBDATA (stnid, mib->macBase[0].hwData.Transmit_Ct) + ftct,
           MIBDATA (stnid, mib->macBase[0].hwData.Token_Ct) + tkct);

    rlct = (ulong)(bmac->rlct3);
    rlct += (ulong)((bmac->rlct1 << 16) + (bmac->rlct2 << 8));

    printf("               late count %d, ring latency count %d\n",
	   bmac->ltct,rlct);

    /* above is hard coded for single mac eli hack attn */
}

static void bsi_show_player (player_t *p, int n, bsi_instance_t *ds)
{
    if (!p)
	return;

    printf("\n  mr %02x, cr %02x, p_icr %02x, icmr %02x, ctsr %02x, "
	   "ijtr %02x, isra %02x, isrb %02x",
	   p->mr,p->cr,p->icr,p->icmr,p->ctsr,p->ijtr,p->isra,p->isrb);
    printf("\n  crsr %02x, rcra %02x, rcrb %02x, rcmra %02x, rcmrb %02x, "
	   "ntr %02x, nptr %02x", p->crsr,p->rcra,p->rcrb,p->rcmra,
	   p->rcmrb,p->ntr,p->nptr);
    printf("\n  cncr %02x, cnpcr %02x, str %02x, sptr %02x, cscr %02x, "
	   "cspcr %02x, letr %02x, clecr %02x", p->cncr,p->cnpcr,
	   p->str,p->sptr,p->cscr,p->cspcr,p->letr,p->clecr);
    printf("\n  udr %02x, idr %02x, cijcr %02x, iccr %02x, ctscr %02x, "
	   "rccra %02x, rccrb %02x\n",
	   p->udr,p->idr,p->cijcr,p->iccr,p->ctscr,p->rccra,p->rccrb);
    printf("  rr0 %02x, rr1 %02x\n",   p->rr0,p->rr1);
    printf("  rcr events:   LSUPI %d,  LSUPV %d, EBOU %d\n",
	   ds->cmt_lsupi[n],ds->cmt_lsupv[n],ds->cmt_ebou[n]);
}

static void bsi_show_players (bsi_instance_t *ds)
{
    xx_fddi_regs_t	*nim;

    if (!ds)
	return;
    nim = ds->module_regs;

    printf("\n PLAYER0 ");
    bsi_show_player((player_t *)&nim->player0,0,ds);
    if (nim->csr0 & CSR0_DAS) {
	printf("\n PLAYER1 ");
	bsi_show_player((player_t *)&nim->player1,1,ds);
    }
}


static void bsi_show_counters (bsi_instance_t *ds)
{
    if (!ds)
	return;

    DEBUG_COUNTERS(printf("\n RX counts: llc %d, not_llc %d, bridge %d, "
			  "mac %d, smt %d",
			  ds->llc, ds->not_llc, ds->bridge, ds->mac, ds->smt));
    DEBUG_COUNTERS(printf("\n  chained iduds %d, switched %d, to_process %d",
			  ds->chained_iduds, ds->switched, ds->to_process));
    DEBUG_COUNTERS(printf("\n TX counts: tx_full %d, tx_1odud %d, "
			  "tx_2odud %d, tx_3odud %d",
			  ds->tx_full, ds->tx_1odud, ds->tx_2odud, 
			  ds->tx_3odud));

    printf("\n RX Error Status: no_page %d, full_pspQ_err %d, max_pspQ_err %d",
	   ds->no_page, ds->full_pspQ_err, ds->max_pspQ_err);
    printf(", psp_reuse_err %d", ds->psp_reuse_err);
    printf("\n  empty_pspQ0 %d, empty_pspQ1 %d, empty_pspQ2 %d",
	   ds->empty_pspQ0, ds->empty_pspQ1, ds->empty_pspQ2);
    printf("\n  full_idudQ0 %d, full_idudQ1 %d, full_idudQ2 %d",
	   ds->full_idudQ0, ds->full_idudQ1, ds->full_idudQ2);
    printf("\n  invalid_data_length %d internal error %d",
	   ds->invalid_data_length, ds->internal_error);
    printf("\n  waited_valid_idud %d, no_valid_idud %d, stacked_saved %d",
	   ds->waited_valid_idud, ds->no_valid_idud, ds->stacked_saved);
    printf("\n  no_first %d, no_middle_last %d",
	   ds->no_first, ds->no_middle_last);
    printf("\n  tc_other %d, tc_format_err %d, tc_frame_stripped %d",
	   ds->tc_other, ds->tc_format_err, ds->tc_frame_stripped);
    printf("\n  bad_frame %d, crc %d, runts %d, frame errors %d",
	   ds->bad_frame, ds->idb->counters.input_crc, ds->idb->counters.runts,
	   ds->idb->counters.input_frame);
    printf("\n  iar_exci0 %d, iar_exci1 %d, iar_exci2 %d",
	   ds->iar_exci0, ds->iar_exci1, ds->iar_exci2);
    printf("\n  throttled %d",
	   ds->throttled);
    printf("\n TX Error Status: rs_none %d, preempted %d, part_done %d",
	   ds->rs_none, ds->rs_preempted, ds->rs_part_done);
    printf("\n  service_loss %d, beacon_or_claim %d, bad_confirm %d",
	   ds->rs_service_loss, ds->rs_beacon_or_claim, ds->rs_bad_confirm);
    printf("\n  output_underrun %d, host_abort %d, bad_ringop %d",
	   ds->idb->counters.output_underrun, ds->rs_host_abort, ds->rs_bad_ringop);
    printf("\n  bmac_abort %d, trt_timeout %d, bmac_reset %d",
	   ds->rs_bmac_abort, ds->rs_trt_timeout, ds->rs_bmac_reset);
    printf("\n  consistency_fail %d, fatal_bsi_error %d",
	   ds->rs_consistency_fail, ds->rs_fatal_bsi_error);
    printf("\n  rar_excr0 %d, rar_excr1 %d, rar_usrr0 %d, rar_usrr1 %d",
	   ds->rar_excr0, ds->rar_excr1, ds->rar_usrr0, ds->rar_usrr1);
    printf("\n  bad_tx_pak %d, tx_restarts %d\n",
	   ds->bad_tx_pak, ds->tx_restarts);
}

/* 
 * modestring(p)
 * return string describing player pointed at by p
 */
static char *modestring (player_t *p)
{
    if ( p->udr & SINGLEMODE ) 
      return("single mode");
    return("multi mode");
}

/*
 * bsi_show_controller
 * support for `show controller fddi X'
 */

static boolean bsi_show_controller (hwidbtype *idb, parseinfo *csb)
{
    bsi_instance_t	*ds;
    xx_fddi_regs_t	*nim;

    ds = INSTANCE;
    if (!ds) {
	printf(not_initialized);
	return(FALSE);
    }
    nim = ds->module_regs;

    printf("\nFDDI: unit %d, ", idb->unit);
    printf("NIM slot %d, NIM type code %d, NIM version %d",
	   idb->slot, idb->nim_type, idb->nim_version);
    printf("\n %s attach, state %d, idb %#x, ds %#x, module_regs %#x",
	   (nim->csr0 & CSR0_DAS) ? "dual" : "single", idb->state,
	   idb, ds, ds->module_regs);
    if ( ((nim->id_prom[0] & 0xff) != XX_FDDI_DBRIDGE) &&
	((nim->id_prom[0] & 0xff) != XX_FDDI_DUAL) )
      printf("\n Phy-M is %s",modestring(&nim->player0));
    else
      printf("\n Phy-A is %s, Phy-B is %s",
	     modestring(&nim->player0),modestring(&nim->player1));
    printf("\n hardware address: %e (wire order %e)\n default hardware address: %e",
	   idb->hardware, idb->bit_swapped_hardware, idb->bia);
    printf("\n loopback %s", (idb->loopback) ? "enabled" : "disabled");

    BSI_DEBUG(printf("\n\n DEBUG info: &id_prom %#x, csr0 %#x, csr1 %#x",
		     nim->id_prom, BYTE(nim->csr0), BYTE(nim->csr1)));
    BSI_DEBUG(printf("\n &csr0 %#x, &csr1 %#x, &player0 %#x, &player1 %#x",
		     &nim->csr0, &nim->csr1, &nim->player0.mr, 
		     &nim->player1.mr));
    BSI_DEBUG(printf("\n &bsi %#x, &bmac %#x, bsi_bsi_offset %#x\n",
		     &nim->bsi.mr, &nim->bmac.mr, bsi_bsi_offset));
    
    bsi_show_cam(ds);
    bsi_show_bsi(ds);
    bsi_show_counters(ds);
    bsi_show_bmac(ds);
    bsi_show_players(ds);
    printf("\n");
    show_pool(idb->pool);

    printf("\n");
    return(FALSE);
}

/* 
 * begin using repeat mode to blast packet pointed at by pak
 * the wire.  return and let csp interrupt handle BCNR,
 * or the xdi code will time out and call SetBeacon()
 * after 370 us, which will terminate repeat mode.
 */
void bsi_repeat (hwidbtype *idb, paktype *pak)
{
    bsi_instance_t	*ds;		/* Driver instance structure */
    req_descr_t		*req_dscr,
                        *req_dscr2;	/* we don't need this, really */
    txd_t		*oduds_ptr;
    uchar               i;
    xx_fddi_regs_t      *nim;
    bsi_t               *bsi;
    bmac_t              *bmac;

    ds = INSTANCE;
    nim = ds->module_regs;
    bsi = (bsi_t *)&nim->bsi;
    bmac = (bmac_t *)&nim->bmac;

    bprintf("bsi_repeat idb %#x, pak %#x, ds %#x\n", idb, pak, ds);
    bprintf("\nrepeat this %x: ",pak);

    for (i=0; i<=pak->datagramsize; i++)
	 bprintf("%x ",pak->datagramstart[i]);
    bprintf("\n");

    /* according to robert macomber @ natl: */
    /* req needs 2 parts!  not just one!  first and last. */
    /* only 1 odud needed though. */

    /* out of page 32 in natl user inf book rev J */
    /* 1 -- ensure target channel is empty */
    
    bsi_initialize(idb);

    /* 2 -- config r0cr enable frame control transparency */
    bsi->r0cr = /* BSI_RxCR_VST | superstitious */
      BSI_RxCR_SAT | BSI_RxCR_FCT | BSI_RxCR_HLD | BSI_RxCR_PRE;
    /* prestaging is always enabled for chan 0 anyway */
    /* another factoid from macomber! */
    
    /* 3 -- build request object in bsi addressable mem 
     *      note that it must be 2 descriptors, first and last .
     *      an "only" will not work.   
     *      code originally lifted from bsi_fastsend  
     *      2nd desc can be 0 length, p 30 in Rev J
     */
    /*
     * Make sure we've got room in the request descriptor
     * queue and in the oduds structure/pak ptr array:
     */
    if ( !(BSIQ_FULL_1K(&ds->BEAC_CHAN.req_ring)) && 
	(!ds->BEAC_CHAN.pak_ring.ptr->addr )) {
	union {
	    req_descr_t	rd;
	    uint	lword[2];
	} u,u2;

	/* we don't need to mark pak_ring in use, ring is down */

	/* Initialize request descriptor for odud1 in oduds: */
	u.lword[0] = u.lword[1] = 0;	
	u2.lword[0] = u2.lword[1] = 0;	

	/* see p 31 of natl rev j addenda */
	u.rd.cnfcls = BSI_REQ_CNFCLS_NONER;  
	u.rd.rqcls = BSI_REQ_RQCLS_IMM;
	u.rd.size = 1;
	u.rd.uid = 6;
	u.rd.fc = 0x51;  /* cannot use a claim or a beacon fc here! */

	/* this really doesn't matter but we're doing it now anyway */
	u2.rd.cnfcls = BSI_REQ_CNFCLS_TENDR;
	u2.rd.rqcls = BSI_REQ_RQCLS_IMM;
	u2.rd.size = 1;
	u2.rd.uid = 7;
	u2.rd.fc = 0x51; /* anything but beacon or claim fc */

	/* bsi will fetch the first descriptor, then will keep fetching */
	/* the odud until abort.  never progresses to next desc, */
	/* according to macomber.  but i'm setting the next desc */
	/* to be a LAST anyway, because i don't trust anything about */
	/* repeat mode.  we can delete the 2nd one after 9.14 */

	oduds_ptr = (txd_t *)ds->BEAC_CHAN.odud_ring.ptr->addr;
	SET_BSI_FIRST_DUD(u.rd.loc, &oduds_ptr->odud1);
	SET_BSI_LAST_DUD(u2.rd.loc, &oduds_ptr->odud2);
	ds->BEAC_CHAN.req.tx_reqs++;

	/* Get pointer to our request descriptor:*/
	req_dscr = (req_descr_t *)ds->BEAC_CHAN.req_ring.limit->addr;
	*req_dscr = u.rd;

	/* get ptr to the next request descriptor */
	req_dscr2 = (req_descr_t *)ds->BEAC_CHAN.req_ring.limit->next;
	*req_dscr2 = u2.rd;

	bprintf("READ req_dscr: uid %d, size %d, cnfcls %d, rqcls %d,"
		"fc %x loc 0x%08x\n",
		req_dscr->uid, req_dscr->size, req_dscr->cnfcls,
		req_dscr->rqcls, req_dscr->fc, req_dscr->loc);
	bprintf("RAW req_dscr@ %#x is 0x%08x:%08x\n",
		req_dscr, *((uint *)req_dscr),
		*((uint *)((uint)req_dscr+4)));

	/*
	 * Set up the ODUD:
	 */
	SET_BSI_ONLY_DUD(oduds_ptr->odud1.loc, pak->datagramstart);
	oduds_ptr->odud1.cnt = 23;

	/* does zeroing these reserved bits really matter? */
	oduds_ptr->odud1.res1a = 0;
	oduds_ptr->odud1.res1b = 0;
    } 	    
    else {
	errmsg(&msgsym(NOMEMORY, FDDI), idb->unit, 
	       "beacon request (channel 0 mem)");
	return;
    }
    
    /* increment ring ptrs and ndx id stamp */
    ds->BEAC_CHAN.req.my_ndx = ++ds->BEAC_CHAN.req.my_ndx & 
      BSI_REQ_NDX_SIZE;
    ds->BEAC_CHAN.pak_ring.ptr = ds->BEAC_CHAN.pak_ring.ptr->next;
    ds->BEAC_CHAN.odud_ring.ptr = ds->BEAC_CHAN.odud_ring.ptr->next;

    /* 4 - stop bmac */
    bmac->mr &= ~BMAC_MR_RUN;   
    bprintf("BMAC mode  %x ",bmac->mr);

    /* 5 - alter tmax */
    ds->origtmax = bmac->tmax;
    bmac->tmax = 0x0f;

    bprintf("about to beacon");

    /* 6 - inhibit recovery required */
    bmac->option |= BMAC_OPTION_IRR;

    /* 7 -- start bmac */ 
    bmac->mr |= BMAC_MR_RUN; 

    /* 8 -- mac reset */  
    /* confused because MCRST clears tmax and IRR */
    bmac->function |= BMAC_FUNCTION_MCRST; 
    while (bmac->function & BMAC_FUNCTION_MCRST);

    /* 9 - update limit ram */
    ds->BEAC_CHAN.req.limit = 
      (req_descr_t *)ds->BEAC_CHAN.req_ring.limit->next;
    bsi_write_lram(ds, ds->BEAC_CHAN.req.lmt_index,
		   BSI_PTR_TO_LIMIT(ds->BEAC_CHAN.req_ring.limit->addr));
    /* another statement we don't really need */
    ds->BEAC_CHAN.req.limit = 
      (req_descr_t *)ds->BEAC_CHAN.req_ring.limit->next;

    bprintf("\nbeaconing started\n");
}


/*
 * init_bsi_idb
 *
 * Set up the idb for a BSI FDDI network interface module (NIM).
 * Note that malloc return zero'd memory, so we only initilize those
 * entries that have non-zero values.
 */

static hwidbtype *init_bsi_idb (int slot, int unit)
{
    bsi_instance_t	*ds;
    hwidbtype		*idb;
    pooltype            *pool;
    int                  size;
    int                  cache_size;
    fddisb              *fsb;
    BSI_DEBUG(SAVE_CALLER());

    /* 
     * Turn all debugging flags on if compiling in debug mode
     */
    BSI_DEBUG(fddi_debug = TRUE);
    BSI_DEBUG(fddi_smt_packet_debug = TRUE);
    BSI_DEBUG(fddi_cmt_events_debug = TRUE);

    BSI_DEBUG(buginf("init_bsi_idb(): slot=%d, unit=%d - called from %#x\n",
		     slot, unit, caller()));
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    idb->unit = unit;
    idb->fddi_stn_id = unit;
    idb->type = IDBTYPE_BSI;

    /*
     * We'll set IDB_HEARSELF only when we expect to be pinging ourselves:
     */
    idb->status = (IDB_FDDI|IDB_BCAST|IDB_BRIDGE);
    idb->slot = slot;
    idb->subunit = 0;

    fddi_idb_init_les(idb);
    idb->counters.resets = 0;

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

    if (idb->bridge_transit)
	idb->span_encapsize = FDDI_SNAP_ENCAPBYTES;
    else
	idb->span_encapsize = FDDI_FC;

    idb->fddi_options |= FDDI_LEM_SUPPORTED;
    idb->fddi_options |= FDDI_CMT_ASSIST_SUPPORTED;
    fsb->fddi_burst_count = FDDI_DEFAULT_BURST_COUNT;
    fsb->fddi_tvx = XX_DEFAULT_TVX;

    idb->error_threshold = 5000;
    idb->error_count = 0;

    idb->soutput = bsi_output;
    idb->reset = bsi_restart;
    idb->system_configured = fddi_system_configured;
    idb->periodic = bsi_periodic;
    idb->shutdown = bsi_shutdown;
    idb->enable = bsi_enable;
    idb->listen = bsi_add_multicast_address;
    idb->listen_range = bsi_add_multicast_range;
    idb->setup_bridging = bsi_setup_bridging;
    idb->firstsw->tbridge_media = TBR_FDDI;
    idb->device_periodic = bsi_periodic;
    idb->show_controller = bsi_show_controller;
    idb->fastsend = bsi_fastsend;
    idb->clear_support = xxfddi_clearcounters;
    idb->linestate = bsi_linestate;
    idb->txqlength = bsi_txqlength;

    TIMER_STOP(idb->lastinput);
    TIMER_STOP(idb->lastoutput);
    TIMER_STOP(idb->lastreset);

    fddi_idb_enqueue(idb);
    idb_enqueue(idb);

    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);

    /*
     * Allocate structure for this instance of the driver:
     */
    ds = malloc_named_fast(sizeof(bsi_instance_t), "BSI Instance");
    if (!ds) {
	crashdump(0);
    }
    ds->idb   = idb;		/* save backpointer to hardware idb */
    /* NB--not subinterface ready for bridging! */
    ds->swidb = idb->firstsw;	/* save backpointer to first software idb */
    INSTANCE = ds;
    ds->module_regs = (xx_fddi_regs_t *)NULL;
    ds->module_regs = (xx_fddi_regs_t *)GET_SLOT_BASE(slot); /* NIM regs */
    if (ds->module_regs == (xx_fddi_regs_t *)NULL) {
	errmsg(&msgsym(BADUNIT, FDDI), idb->unit);
	net_cstate(idb, IDBS_DOWN);
	return(FALSE);
    }

    /*
     * Get NIM information:
     */
    idb->nim_type    = (ushort)((ds->module_regs->id_prom[0]) & 0xFF);
    idb->nim_version = (ushort)((ds->module_regs->id_prom[1]) & 0xFF);
    idb->typestring  = bsi_typestring(idb->nim_type);

    /*
     * Get the default FDDI MAC address from the cookie.
     */
    bsi_get_default_hw_address(idb, idb->bia);

    /*
     * Malloc the shared mailbox and TX info structure in IO memory:
     */
    ds->ioram = malloc_named_iomem_aligned((sizeof(shrd_ram_t) + 16),
				  BSI_4KB_PAGE_SIZE, "BSI TX Info");
    if (!ds->ioram) {
	crashdump(0);
    }
    ds->tx_restarts = 0;

    /*
     ********************************************************************
     * Allocate the BSI Queues in IO memory allocated on 4 KB boundaries:
     ********************************************************************
     */

    /*
     * Request channel 0:
     */
    ds->rq0.queue_number = 0;

    /* Request queue */
    ds->rq0.req.name = "rq0 req";
    ds->rq0.req.ptr_index = BSI_RQPR0;
    ds->rq0.req.lmt_index = BSI_RQLR0;
    ds->rq0.req.base = malloc_named_iomem_aligned(sizeof(req_descrQ_t),
					 BSI_4KB_PAGE_SIZE, "BSI RX0 Req");
    if (!ds->rq0.req.base) {
	crashdump(0);
    }

    /* Confirmation queue */
    ds->rq0.cnf.name = "rq0 cnf";
    ds->rq0.cnf.ptr_index = BSI_CQPR0;
    ds->rq0.cnf.lmt_index = BSI_CQLR0;
    ds->rq0.cnf.base = malloc_named_iomem_aligned(sizeof(cnf_descrQ_t),
					 BSI_4KB_PAGE_SIZE, "BSI RX0 Conf");
    if (!ds->rq0.cnf.base) {
	crashdump(0);
    }

    bsi_init_req_ring(ds, &ds->rq0);

    /*
     * Request channel 1:
     */
    ds->rq1.queue_number = 1;

    /* Request queue */
    ds->rq1.req.name = "rq1 req";
    ds->rq1.req.ptr_index = BSI_RQPR1;
    ds->rq1.req.lmt_index = BSI_RQLR1;
    ds->rq1.req.base = malloc_named_iomem_aligned(sizeof(req_descrQ_t),
					 BSI_4KB_PAGE_SIZE, "BSI RX1 Req");
    if (!ds->rq1.req.base) {
	crashdump(0);
    }

    /* Confirmation queue */
    ds->rq1.cnf.name = "rq1 cnf";
    ds->rq1.cnf.ptr_index = BSI_CQPR1;
    ds->rq1.cnf.lmt_index = BSI_CQLR1;
    ds->rq1.cnf.base = malloc_named_iomem_aligned(sizeof(cnf_descrQ_t),
					 BSI_4KB_PAGE_SIZE, "BSI RX1 Conf");
    if (!ds->rq1.cnf.base) {
	crashdump(0);
    }

    bsi_init_req_ring(ds, &ds->rq1);

    /*
     * Indicate channel 0:
     */
    ds->ic0.queue_number = 0;

    /* Input data unit queue */
    ds->ic0.idud.name = "ic0 idud";
    ds->ic0.idud.ptr_index = BSI_IQPI0;
    ds->ic0.idud.lmt_index = BSI_IQLI0;
    ds->ic0.idud.base = malloc_named_iomem_aligned(sizeof(idud_descrQ_t) + BSI_QPAD,
					  BSI_4KB_PAGE_SIZE, "BSI IC0 Idud");
    if (!ds->ic0.idud.base) {
	crashdump(0);
    }

    /* Pool space pointer queue */
    ds->ic0.psp.name = "ic0 psp";
    ds->ic0.psp.ptr_index = BSI_PQPI0;
    ds->ic0.psp.lmt_index = BSI_PQLI0;
    ds->ic0.psp.base = malloc_named_iomem_aligned(sizeof(psp_descrQ_t) + BSI_QPAD,
					 BSI_4KB_PAGE_SIZE, "BSI IC0 Psp");
    if (!ds->ic0.psp.base) {
	crashdump(0);
    }

    bsi_init_ind_ring(ds, &ds->ic0);

    /*
     * Indicate channel 1:
     */
    ds->ic1.queue_number = 1;

    /* Input data unit queue */
    ds->ic1.idud.name = "ic1 idud";
    ds->ic1.idud.ptr_index = BSI_IQPI1;
    ds->ic1.idud.lmt_index = BSI_IQLI1;
    ds->ic1.idud.base = malloc_named_iomem_aligned(sizeof(idud_descrQ_t) + BSI_QPAD,
					  BSI_4KB_PAGE_SIZE, "BSI IC1 Idud");
    if (!ds->ic1.idud.base) {
	crashdump(0);
    }

    /* Pool space pointer queue */
    ds->ic1.psp.name = "ic1 psp";
    ds->ic1.psp.ptr_index = BSI_PQPI1;
    ds->ic1.psp.lmt_index = BSI_PQLI1;
    ds->ic1.psp.base = malloc_named_iomem_aligned(sizeof(psp_descrQ_t) + BSI_QPAD,
					 BSI_4KB_PAGE_SIZE, "BSI IC1 Psp");
    if (!ds->ic1.psp.base) {
	crashdump(0);
    }

    bsi_init_ind_ring(ds, &ds->ic1);

    /*
     * Indicate channel 2:
     */
    ds->ic2.queue_number = 2;

    /* Input data unit queue */
    ds->ic2.idud.name = "ic2 idud";
    ds->ic2.idud.ptr_index = BSI_IQPI2;
    ds->ic2.idud.lmt_index = BSI_IQLI2;
    ds->ic2.idud.base = malloc_named_iomem_aligned(sizeof(idud_descrQ_t) + BSI_QPAD,
					  BSI_4KB_PAGE_SIZE, "BSI IC2 Idud");
    if (!ds->ic2.idud.base) {
	crashdump(0);
    }

    /* Pool space pointer queue */
    ds->ic2.psp.name = "ic2 psp";
    ds->ic2.psp.ptr_index = BSI_PQPI2;
    ds->ic2.psp.lmt_index = BSI_PQLI2;
    ds->ic2.psp.base = malloc_named_iomem_aligned(sizeof(psp_descrQ_t) + BSI_QPAD,
					 BSI_4KB_PAGE_SIZE, "BSI IC2 Psp");
    if (!ds->ic2.psp.base) {
	crashdump(0);
    }

    bsi_init_ind_ring(ds, &ds->ic2);

    /*
     * WARNING: BSI chickenbone pointer magic ahead
     *
     * The BSI chipset requires that the RX DMA address be aligned to a 32
     * byte boundary. The size of the encapsulation that we align to, back
     * from The Line is BSI_PADDED_ENCAPBYTES (24 bytes).
     *
     * So, the actual paktype structure is offset from the start of the
     * io_memalign() data block by the size bsi_buffer_offset in order for the
     * point, 24 bytes back from The Line, to be on a 32 byte boundary.
     *
     * The following calculation works out what it'll take to align The Line to
     * a 32 byte boundary, and then shims the whole structure up by the size of
     * MAC encapsulation so that the aligned point is in fact the start of
     * RX DMA.
     *
     * That was fun, wasn't it?
     */
    bsi_buffer_offset = (32 - (ENCAPBYTES % 32)) + BSI_PADDED_ENCAPBYTES;

    /*
     * Work out the two offsets that drive the packet structure. One's used
     * by the CPU, the other by the BSI (as it's longword aligned). Note 
     * that these are now relative to the start of the data area, not 
     * the start of the real allocated memory block. Big difference.
     */
    bsi_bsi_offset = ENCAPBYTES - BSI_PADDED_ENCAPBYTES;
    bsi_start_offset = ENCAPBYTES - FDDI_SNAP_ENCAPBYTES;

    BSI_DEBUG(buginf("\nBSI buffer parameters"));
    BSI_DEBUG(buginf("\nbsi_buffer_offset = %d", bsi_buffer_offset));
    BSI_DEBUG(buginf("\nsizeof(paktype)   = %d", sizeof(paktype)));
    BSI_DEBUG(buginf("\nbsi_bsi_offset    = %d", bsi_bsi_offset));
    BSI_DEBUG(buginf("\nbsi_start_offset  = %d\n", bsi_start_offset));

    /*
     * Find the public pool that matches the MTU for FDDI
     */
    pool = pak_pool_find_by_size(MAXSNAPFDDISIZE);

    /*
     * Paranoid check in case there isn't a pool big enough. Very unlikely.
     */
    if (!pool) {
	errmsg(&msgsym(FDDIFAIL, FDDI), idb->hw_namestring,
	       "No public buffer pool large enough for FDDI frames");
	crashdump(0);
    }

    /*
     * Be increasingly paranoid and conservative and present a buffer size
     * equal to the smaller of the two possible pools (public and private).
     * This has no real effect on the operation of the system but gives more
     * marginal warm fuzzies for size copy errors.
     */
    size = BSI_BUFFER_SIZE - TRAILBYTES - bsi_buffer_offset;
    size = (size < pool->size) ? size : pool->size;

    /*
     * Initialize private buffer pool (for 1st FDDI NIM only)
     */
    cache_size = BSI_BUFFER_PAGES;

    if (!bsi_buffer_pool) {
        idb->pool_group = pool_create_group();
	bsi_buffer_pool = pak_pool_create("Fddi", idb->pool_group,
					  size, POOL_SANITY, NULL); 

	if (!bsi_buffer_pool) {
	    errmsg(&msgsym(NOMEMORY, FDDI), idb->unit, "private buffer pool");
	    crashdump(0);
	}

	/*
	 * Customize pool parameters for driver
	 */
	bsi_buffer_pool->item.create  = bsi_buffer_create;
	bsi_buffer_pool->item.destroy = bsi_buffer_destroy;
	/*
         * bsi_buffer_pool->buffer_validate uses default routine
         */
	bsi_buffer_pool->fallback       = pool;
	bsi_page_count = 0;

	/*
	 * Add buffer cache to pool
	 */
	if (!pak_pool_create_cache(bsi_buffer_pool,
				   BSI_BUFFER_PAGES * MAX_STATION_COUNT)) {
	    crashdump(0);
	}
	pool_set_cache_threshold(bsi_buffer_pool,
				 BSI_BUFFER_THRESHOLD,
				 bsi_cache_threshold);
    }

    /*
     * (Re)Fill pool and its cache with buffers.
     *
     * All buffers are shared among the FDDI NIMs through a common
     * buffer pool. The first NIM (unit 0) will allocate BSI_BUFFER_PAGES
     * buffers, while additional NIMs will allocate I/O memory based on
     * the amount of I/O memory available in the box.
     *
     * If I/O memory is "reduced", only (1.5*BSI_BUFFER_PAGES) buffers
     * will be allocated in the case of multiple FDDI NIMs, which will
     * limit the reserved I/O memory to approximately
     * (1.5*BSI_BUFFER_PAGES*8kB) (~3MB in the case where BSI_BUFFER_PAGES
     * is 256).
     *
     * If I/O memory is not "reduced", each additional FDDI NIM will allocate 
     * BSI_BUFFER_PAGES more to the shared pool.
     */
    if (unit) {
	/* more than one FDDI NIM present (unit 0 already handled above) */
	if (bsi_reduced_memory)
	    cache_size = BSI_BUFFER_PAGES + (BSI_BUFFER_PAGES >> 1);
	else
	    cache_size = BSI_BUFFER_PAGES * (unit + 1);
    }

    pool_adjust(bsi_buffer_pool, 0, cache_size, cache_size, TRUE);
    pool_adjust_cache(bsi_buffer_pool, cache_size);

    /*
     * Calibrate the spin wait routine:
     */
    ds->delay_10_usec = timer_calibrate(10, FALSE);

    /*
     * Register FDDI common buffer pool with IDB
     */
    idb->pool = bsi_buffer_pool;

    /*
     * Try and kickstart the Xilinx hardware needed for
     * bridging support and multicasts. Try and recover gracefully
     * if the Xilinx wouldn't play ball. Only try and the start the
     * Xilinx if we're running a system image
     */
    if (!(system_loading | bootstrap_enable)) {
	/*
	 * We're running a full system image, so go ahead and try
	 * and start the Xilinx
	 */
	if (touch_device((void *)&ds->module_regs->lcr)) {
	    /*
	     * Download the Xilinx and check the state
	     */
	    ds->cam_state   = config_xilinx(ds->module_regs);
	    ds->cam_present = (ds->cam_state != CAM_FIRMWARE_FAIL);
	    
	} else {
	    /*
	     * We've got a cheapo board. Don't crash the image, but do
	     * set up the flags so we won't bus error
	     */
	    ds->cam_state   = CAM_FIRMWARE_NONE;
	    ds->cam_present = FALSE;
	}

  	/*
 	 * Reset some internal CAM state variables
 	 */
 	ds->cam_all_multicasts = FALSE;
 	ds->cam_learning       = FALSE;
 
	/*
	 * Print an error to the console if we couldn't find the Xilinx
	 * (either the board is an SAS board, or it's scooped). Only
	 * do this for full system images.
	 */
	if (!ds->cam_present)
	    errmsg(&msgsym(NOCAM, FDDI), idb->unit);

    } else {
	/*
	 * We've got a bootstrap image. Don't try to start the Xilinx as
	 * there might not be a Xilinx software download image installed
	 */
	ds->cam_state   = CAM_FIRMWARE_NONE;
	ds->cam_present = FALSE;
    }

    /*
     * Make sure that the SMT/CMT stuff doesn't start until NVRAM is parsed,
     * and init the BSI driver software. The interface status won't go up
     * until the SMT code says that the ring is up too:
     */
    fddi_disconnect (idb->fddi_stn_id);
    bsi_initialize(idb);

    return(idb);
} 

/*
 * bsi_memory_notify
 *
 * Print a warning to let the user know he's being cheap and should put
 * more I/O ram in the box if he wants it to work. Print the message
 * at startup and during "show version".
 */

static void bsi_memory_notify (emergency_msg_time_type emergency_msg_time)
{
    if (emergency_msg_time == EMERGENCY_SYS_STARTUP) {
	errmsg(&msgsym(ILLEGAL, LES_FDDI));
	return;
    } 

    if (emergency_msg_time == EMERGENCY_SHOW_VERSION) {
	printf("\n%%FDDI-2-ILLEGAL: ");
	printf("%s\n", msgtxt_fddi_illegal);
    }
}

/*
 * bsi_analyze_interface
 *
 * Initialize BSI NIM for the given slot.
 */

static void bsi_analyze_interface (int slot)
{
    ushort type;
    int iosize;
    xx_fddi_regs_t *rp;

    bsi_check_version(slot);

    /*
     * Don't accept more than the maximum number of
     * supported FDDI interfaces.
     */
    if (nfddinets >= MAX_STATION_COUNT)
	return;

    type = nim_get_type(slot);
    rp   = (xx_fddi_regs_t *)GET_SLOT_BASE(slot);

    /*
     * If this box doesn't have enough memory, squeal at the user and
     * ignore these interfaces
     */
    iosize = mempool_get_total_bytes(MEMPOOL_CLASS_IOMEM,
				     MEMPOOL_ALTERNATE_IGNORE);
    
    if ((type == XX_FDDI_SINGLE) || (type == XX_FDDI_SBRIDGE) ||
	(type == XX_FDDI_DUAL)   || (type == XX_FDDI_DBRIDGE)) {
	if (iosize < BSI_IOMEMORY_MIN) {
	    reg_add_emergency_message(bsi_memory_notify,
				      "bsi_memory_notify");
	    return;
	} else if (iosize < BSI_IOMEMORY_MIN * MAX_STATION_COUNT)
	    bsi_reduced_memory = TRUE;
    }

    if ((type == XX_FDDI_SINGLE) ||
	(type == XX_FDDI_SBRIDGE)) {
	int eplay, ebmac, ebsi;
	nphy = 0;
	if ((eplay = regtestplayer(&rp->player0)) | 
	    (ebmac = regtestbmac(&rp->bmac)) | 
	    (ebsi  = regtestbsi(&rp->bsi)) ) {
	    return;
	}
	rp->bmac.function = BMAC_FUNCTION_MARST; /* master reset */
	STADATA (nmac, smtCfg) = &SASSMTCfg;
	STADATA (nmac, macCfg) = &SingleBMACMACCfg;
	STADATA (nmac, pathCfg) = &SASPATHCfg;
	STADATA (nmac, portCfg) = &MSASPORTCfg;
	STADATA (nmac, xferRegs[0]) = &rp->bsi;
	STADATA (nmac, macRegs[0]) = &rp->bmac;
	STADATA (nmac, portRegs[nphy++]) = &rp->player0;
	STADATA (nmac, hwIDB[0]) = init_bsi_idb(slot, nfddinets++);
	STADATA (nmac, xferData[0]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, macData[0]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, portData[0]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, hwPresent) = TRUE;
	STADATA (nmac, TLMin) = TL_MIN;
	nmac++;
    }
    if ((type == XX_FDDI_DUAL) || 
	(type == XX_FDDI_DBRIDGE) ) {
	int eplay0, eplay1, ebmac, ebsi;
	nphy = 0;
	if ((eplay0 = regtestplayer(&rp->player0)) | 
	    (eplay1 = regtestplayer(&rp->player1)) | 
	    (ebmac = regtestbmac(&rp->bmac)) | 
	    (ebsi  = regtestbsi(&rp->bsi)) ) {
	    return;
	}
	rp->bmac.function = BMAC_FUNCTION_MARST; /* master reset */
	STADATA (nmac, smtCfg) = &DASSMTCfg;
	STADATA (nmac, macCfg) = &SingleBMACMACCfg;
	STADATA (nmac, pathCfg) = &DASPATHCfg;
	STADATA (nmac, portCfg) = &MMDASPORTCfg;
	STADATA (nmac, xferRegs[0]) = &rp->bsi;
	STADATA (nmac, macRegs[0]) = &rp->bmac;
	STADATA (nmac, portRegs[nphy++]) = &rp->player0;
	STADATA (nmac, portRegs[nphy++]) = &rp->player1;
	STADATA (nmac, hwIDB[0]) = init_bsi_idb(slot, nfddinets++);
	STADATA (nmac, xferData[0]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, macData[0]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, portData[0]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, portData[1]) = (void *)HWIDBDATA (nmac, 0, devctl);
	STADATA (nmac, hwPresent) = TRUE;
	STADATA (nmac, TLMin) = TL_MIN;
	nmac++;
    }
}

/*
 * bsi_if_final_init
 *
 * Called by if_final_init() - starts FDDI up if required
 */

static void bsi_if_final_init (void)
{
    if (nfddinets) {
        MIBStartup ();
        CSPStartup ();
        FBMStartup ();
	reg_add_media_delete_hardware_address_filter
	    (les_fddi_cam_depopulate, "les_fddi_cam_depopulate");

	if (cookie->processor == PROCESSOR_XX)
            createlevel (LEVEL7, NULL, "FDDI NMI stack");
    }
}  


/*
 * bsi_bad_nvram_if()
 * Try to bring up interface on bad NVRAM
 */
static void bsi_bad_nvram_if (void)
{
    int i;

    for (i = 0; i < MAX_STATION_COUNT; i++) {
	if (STADATA(i, hwIDB[0]) != NULL) {
	    fddi_system_configured(STADATA(i, hwIDB[0]));
	}
    }
}
 

/*
 * BSI subsystem initialisation entry point
 */

static void bsi_subsys_init (subsystype *subsys)
{
    int i, j;

    /*
     * Register the NIM IDs for the various BSI FDDI NIMs
     */
    reg_add_analyze_interface(XX_FDDI_SINGLE,
			      bsi_analyze_interface,
			      "bsi_analyze_interface");
    reg_add_analyze_interface(XX_FDDI_SBRIDGE,
			      bsi_analyze_interface,
			      "bsi_analyze_interface");
    reg_add_analyze_interface(XX_FDDI_DUAL,
			      bsi_analyze_interface,
			      "bsi_analyze_interface");
    reg_add_analyze_interface(XX_FDDI_DBRIDGE,
			      bsi_analyze_interface,
			      "bsi_analyze_interface");

    /*
     * Register the c4000 specific parser points
     */
    fddi_c4000_parser_init();

    /*
     * Add a FDDI final startup routine
     */
    reg_add_if_final_init(bsi_if_final_init, "bsi_if_final_init");

    /*
     * Add bad NVRAM routine
     */
    reg_add_bad_nvram_if(bsi_bad_nvram_if, "bsi_bad_nvram_if");

    /*
     * Initialize and reset global FDDI states
     */
    nphy = nmac = 0;
    bsi_buffer_pool = NULL;
    bsi_reduced_memory = FALSE;
    
    /*
     * Reset FDDI SMT data structure
     */
    for (i = 0; i < MAX_STATION_COUNT; i++) {
        for (j=0; j<MAX_MAC_COUNT; j++)
            STADATA (i, hwIDB[j]) = 0;
        for (j=0; j<MAX_PORT_COUNT; j++)
            STADATA (i, portRegs[j]) = 0;
        STADATA (i, hwPresent) = FALSE;
        STADATA (i, prevCFState) = CF_isolated;
    }
}

/*
 * BSI subsystem header
 */

#define BSI_MAJVERSION   1
#define BSI_MINVERSION   0
#define BSI_EDITVERSION  1

SUBSYS_HEADER(bsi, BSI_MAJVERSION, BSI_MINVERSION, BSI_EDITVERSION,
	      bsi_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);


/*
 * Send Connect action message to PCM.
 */
void cmt_connect (hwidbtype *idb, int which)
{ 
    leveltype SR;

    if (which & FDDI_PHY_A) {
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Enable, 0, (uInt32) 0);
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Start, 0, (uInt32) 0);
    }
    if (which & FDDI_PHY_B) {
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Enable, 1, (uInt32) 0);
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Start, 1, (uInt32) 0);
    }
    
    /* Process signals */
    if (!EmptySignalQueue ()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}

/*
 * Send Disconnect action message to ECM.
 */
void cmt_disconnect (hwidbtype *idb, int which)
{
    leveltype	SR;

    if (which & FDDI_PHY_A) {
        SendSignalMulti(idb->fddi_stn_id, SIG_PC_Disable, 0, (uInt32) 0);
    }
    if (which & FDDI_PHY_B) {
        SendSignalMulti(idb->fddi_stn_id, SIG_PC_Disable, 1, (uInt32) 0);
    }

    /* Process signals */
    if (!EmptySignalQueue()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}

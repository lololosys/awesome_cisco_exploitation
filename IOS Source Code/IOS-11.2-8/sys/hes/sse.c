/* $Id: sse.c,v 3.9.8.12 1996/09/11 17:38:49 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse.c,v $
 *------------------------------------------------------------------
 * sse.c - Support routines for the SSE
 *
 * Sept., 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse.c,v $
 * Revision 3.9.8.12  1996/09/11  17:38:49  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.9.8.11  1996/08/28  12:46:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.8.10  1996/08/16  21:23:53  rbadri
 * CSCdi64807:  lane: ipx packets not forwarded when sse is enabled
 * Branch: California_branch
 * Force re-write of 802.3 length for ATM-LANE interfaces as well
 *
 * Revision 3.9.8.9  1996/07/30  14:41:39  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.9.8.8  1996/06/28  23:06:24  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.8.7  1996/06/06  23:24:26  gchristy
 * CSCdi42802:  enabling SSE for IPX blocks SPX packets (case B50178)
 * Branch: California_branch
 *   Change the way we count back from the line to insert the 802.3 length
 *   field from words to bytes.  Requires ssp215-1 or later ucode.
 *
 * Revision 3.9.8.6  1996/05/14  06:58:34  gchristy
 * CSCdi56016:  SSE: mixing IP and IPX traffic can cause IP to fast-switch
 * Branch: California_branch
 * - Checksum only IP and CLNS headers, if you please.
 *
 * Revision 3.9.8.5  1996/05/09  14:29:07  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.8.4  1996/04/30  05:43:54  gchristy
 * CSCdi56163:  SSE: move low_mem function to system registry
 * Branch: California_branch
 *
 * Revision 3.9.8.3  1996/04/29  09:20:37  gchristy
 * CSCdi50875:  SSE: handle low memory conditions more gracefully
 * Branch: California_branch
 *
 * Revision 3.9.8.2.10.1  1996/04/27  07:02:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.9.8.2  1996/04/05  01:02:17  hampton
 * One of the files included by these file may not exist when dependencies
 * are built.  To prevent these files from being re-compiled with every
 * make, use a full path name in the include statement.  [CSCdi53732]
 * Branch: California_branch
 *
 * Revision 3.9.8.1.8.1  1996/04/08  01:47:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.9.8.1  1996/03/18  19:43:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.12.1  1996/03/22  09:36:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.9  1996/02/28  09:32:54  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.8.8.1  1996/03/05  06:11:39  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.2.1  1996/02/27  20:42:19  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8  1996/02/07  16:11:39  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/01/29  07:27:47  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6  1996/01/25  11:18:00  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.5  1996/01/24  20:13:53  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4.2.1  1996/01/24  21:48:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/18  22:47:36  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3.2.1  1996/02/15  19:13:21  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.3  1995/12/22  00:14:06  gchristy
 * CSCdi43446:  SSE walks ICB tree-- Falls off limb-Commits hari-kari
 * - Punt if sse_icbtree_add_node() fails.
 *
 * Revision 3.2  1995/11/17  09:20:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/09/28  21:57:54  gchristy
 * CSCdi41169:  SSE: always issue an errmsg before hanging
 *
 * Revision 2.8  1995/09/25  08:33:49  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/08/08  16:47:03  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.6  1995/08/08 08:55:55  tli
 * CSCdi37030:  SSE: input i/f MIP fails to SSE switch
 * Be sure to set the routing capability bits for all channel groups,
 * unless CRB is enabled.
 *
 * Revision 2.5  1995/06/21  06:41:57  gchristy
 * CSCdi35154:  SSE: SRB can sometimes fast-switch
 *   - don't assume bridge numbers are symmetric
 *   - only skip RCF bytes on a failure
 *   - restart RCF parsing with node immediately following ICB compare
 *   - allow SSE switching when AST is configured
 *
 * Revision 2.4  1995/06/18  23:17:49  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/13  18:05:40  mikel
 * minor fix for mispelling (under CSCdi35696)
 *
 * Revision 2.2  1995/06/09  13:04:54  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:44:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "if_mci.h"
#include "if_fci.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../util/range.h"
#include "../ip/ip_registry.h"
#include "../wan/frame_relay.h"
#include "../ip/ip.h"
#include "../ip/tftp.h"
#include "../if/tring.h"
#include "../os/free.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../lane/lane.h"

#include "../parser/parser_defs_sse.h"
#include "../hes/ssesym.h"
#include "sse_public.h"
#include "sse.h"
#include "sse_private.h"
#include "sse_registry.h"

/*
 * Global storage
 */
boolean sse_manager_running;
static boolean sse_manager_wakeup;
static sys_timestamp sse_periodic, sse_minsleep;
static int sse_manager_period, sse_manager_quiet;
static uint spin_malloced, spin_freed;
#if 0
static boolean sse_multipath_duplicate_found;
static rwdbtype* sse_multipath_search_target;
#endif
spintype *sse_compiletree;
boolean sse_manager_blocked;
static ulong sse_quantum;		/* runtime accumulated as of last
					   suspend */
static ulong sse_max_quantum;		/* max quantum ever */
static ulong sse_max_quantum_pc;	/* location of max quantum */
static ulong sse_max_iteration;		/* ms in the longest cache
					   calculation */ 
static int sse_tree_nodecount;
static queuetype spinfreeQ;		/* free nodes */
static int sse_manager_pid;
static boolean sse_sanity_checks;	/* whether to perform sanity checks */

static shadowwordtype boottree[] = {
#include "../hes/sseasm.h"
};    

const treeaddrtype sse_backpatch_table[SSE_MAXINDEX] = {
    SSE_GLOBAL_HANG,			/* Not used */
    SSE_GLOBAL_IPDEST,			/* IP destination */
    SSE_GLOBAL_NOVDEST,			/* Novell destination */
    SSE_GLOBAL_SRBDEST,			/* SRB RIF */
    SSE_GLOBAL_CLNPDEST,		/* CLNP destination */
};

static const char *sse_reinitializing =
    "\nThe SSE is currently being reinitialized.  Please wait.";

static forktype sse_manager(void);
static void sse_rw_gc(ssetype *sse);
static void sse_match_gc(ssetype *sse);
static void sse_build_icbtree(ssetype *sse);
static void show_sse_summary(void);
static void show_sse_memfree(void);




/*
 *			  Memory access routines
 */


/*
 * sse_peek_treemem
 * Read a word from tree memory. 
 */

inline void
sse_peek_treemem (ssetype *sse, int addr, shadowwordtype *sw)
{
    shadowwordtype *treeptr;
    
    if ((addr < 0) || (addr > sse->words_avail)) {
	errmsg(&msgsym(SWFAILURE, SSE), "bad program memory read, addr", 
	       addr);
	return;
    }

    if (sse->real) {
	treeptr = (shadowwordtype *)(SSE_TREE + (addr<<4));
	*sw = *treeptr;
    }
}

/*
 * sse_peek_rwmem
 * Read a word from rwmem.
 */

ushort sse_peek_rwmem (ssetype *sse, int addr)
{
    if ((addr < 0) || (addr >= sse->rwbytes_avail) || (addr & 1)) {
	errmsg(&msgsym(SWFAILURE, SSE), "bad rewrite memory read for",
	       addr); 
	return(0);
    }
    
    if (sse->real) 
	return(*(ushort *)&SSE_REWRITE[addr]);
    else
	return(0);
}

/*
 * sse_poke_rwmem
 * Bcopy a string into rewrite memory
 */

void sse_poke_rwmem (ssetype *sse, char *source, int addr, int len)
{
    if ((addr < 0) || (addr + len > sse->rwbytes_avail) || (len < 0)) {
	errmsg(&msgsym(SWFAILURE, SSE), "bad rewrite memory write", addr);
	return;
    }
    
    if (len == 0)
	return;
    if (sse->real) {
	volatile ushort *sp;

	/*
	 * Deal with odd start.
	 */
	if (addr & 1) {
	    sp = (ushort*)&SSE_REWRITE[addr - 1];
	    *sp = (*sp & 0xff00) | *source++;
	    len--;
	    addr++;
	}

	/*
	 * Do number of words.
	 */
	if (len > 1) {
	    wcopy((ushort*)source, (ushort*)&SSE_REWRITE[addr], len>>1);
	    addr += (len & 0xfffffffe);
	    source += (len & 0xfffffffe);
	    len &= 1;
	}
	
	/*
	 * Deal with odd tail.
	 */
	if (len) {
	    sp = (ushort*)&SSE_REWRITE[addr];
	    *sp = (*sp & 0xff) | (*source << 8);
	}
    }
}



/*
 *			 Initialization routines.
 */



/*
 * sse_ssetype_init
 * Allocate and initialize the components of the ssetype structure.  If
 * there is an error, return FALSE.
 */

static boolean sse_ssetype_init (mcitype *cbus, ssetype *sse)
{
    int bootwords, i;
    uint low;
    shadowcelltype cell;
    rangelist *freetreememory, *freerwmemory;
    shadowwordtype word;
    long *memptr;

    static uchar preloadedrewrite[] =
	{0,				/* indicates no rewrite */
	 0, 0, AC_PRI4, 4};		/* token ring SRB rewrite */

    /*
     * Create memory management structures.
     */
    freetreememory = sse->freetreememory;
    if (!freetreememory)
	freetreememory = rangelist_create();
    freerwmemory = sse->freerwmemory;
    if (!freerwmemory)
	freerwmemory = rangelist_create();
    rangelist_init(freetreememory, 0,
		   sse->words_avail - 1);
    rangelist_init(freerwmemory, 0,
		   sse->rwbytes_avail - 1); 
    process_may_suspend();

    if (sse->real)
	memset(SSE_REWRITE, 0, sse->rwbytes_avail);
    
    /*
     * Allocate location 0 for the empty rewrite string.  Allocate location
     * 1 for the rewrite string for SRB.
     */
    if (!rangelist_malloc_block(freerwmemory, sizeof(preloadedrewrite),
				&low)) {
	errmsg(&msgsym(NOMEMORY, SSE), "free RW memory management");
	goto erred;
    }
    if (low != 0) {
	errmsg(&msgsym(NOTZERO, SSE));
	goto erred;
    }
    sse_poke_rwmem(sse, preloadedrewrite, 0, sizeof(preloadedrewrite));
    sse->rwbytes_used += sizeof(preloadedrewrite);
    sse->overheadstat.rwbytes_used += sizeof(preloadedrewrite);
    
    /*
     * Copy the assembly level bootstrap into place.
     */
    bootwords = sizeof(boottree) / sizeof(shadowwordtype);
    if (!rangelist_malloc_block(freetreememory, bootwords, &low)) {
	errmsg(&msgsym(NOMEMORY, SSE), "initial program memory");
	goto erred;
    }
    if (low != 0) {
	errmsg(&msgsym(NOTZERO, SSE));
	goto erred;
    }
    
    sse->words_used += bootwords;
    sse->overheadstat.words_used += bootwords;
    
    sse->freetreememory = freetreememory;
    sse->freerwmemory = freerwmemory;
    
    cell.ptr = 0;
    cell.value = 0;
    cell.inst = SSE_INST_HANG;
    word.lt = cell;
    word.eq = cell;
    word.gt = cell;
    process_may_suspend();
    for (i = 0; i < sse->words_avail; i++) {
	sse_poke_treemem(sse, i, &word);
	process_may_suspend();
    }
    for (i = 0; i < bootwords; i++) {
	sse_poke_treemem(sse, i, &boottree[i]);
	process_may_suspend();
    }
    sse->state &= ~SSE_STATE_SWRESET;

    if (sse->real) {
      /*
       * Enable the hardware.
       */
      memptr = (long *)SSE_INITREG;
      *memptr = (SSE_GLOBAL_RESET << 16);
    }

    return(TRUE);
    
 erred:
    if (freetreememory)
	rangelist_destroy(freetreememory);
    if (freerwmemory)
	rangelist_destroy(freerwmemory);
    sse->freetreememory = NULL;
    sse->freerwmemory = NULL;
    return(FALSE);
}

/*
 * sse_set_mtu
 * Tell the SSE about the MTU for an interface.
 */

void sse_set_mtu (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    mcitype *cbus;
    mciregtype *regptr;
    int indx;
    ushort mtu;
    idbtype *idb;
    leveltype level;

    if (!hwidb)
	return;
    if (!(hwidb->status & IDB_CBUS))
	return;
    if (hwidb->status & IDB_NOHARDWARE)
	return;
    cbus = hwidb->devctl;
    if (!cbus->sseptr)
	return;

    level = raise_interrupt_level(NETS_DISABLE);
    regptr = cbus->regptr;
    for (indx = 0; indx < cbus->max_interfaces; indx++) {
	if (hwidb == cbus->interface_list[indx]) {
	    regptr->argreg = indx;
	    regptr->cmdreg = FCI_CMD_SELECT;
	    break;
	}
    }
    if (indx >= cbus->max_interfaces) {
	reset_interrupt_level(level);
	return;
    }
    idb = firstsw_or_null(hwidb);
    if (!idb) {
        reset_interrupt_level(level);
        return;
    }
    mtu = idb->ip_mtu;
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) 
	mtu = min(mtu, idb->ip_mtu);
    regptr->largreg = (mtu << 16) + FCI_SET_IP_MTU;
    regptr->cmdreg = FCI_CMD_SET_MTU;

    idb = firstsw_or_null(hwidb);
    if (!idb) {
        reset_interrupt_level(level);
        return;
    }
    mtu = idb->clns_mtu;
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) 
	mtu = min(mtu, idb->clns_mtu);
    regptr->largreg = (mtu << 16) + FCI_SET_CLNP_MTU;
    regptr->cmdreg = FCI_CMD_SET_MTU;
    regptr->largreg = (hwidb->maxmtu << 16) + FCI_SET_IPX_MTU;
    regptr->cmdreg = FCI_CMD_SET_MTU;
    reset_interrupt_level(level);
}

/*
 * sse_hw_off
 * Disable SSE hardware.
 */

void sse_hw_off (mcitype *cbus)
{
    mciregtype *regptr;
    ssetype *sse;
    int response;
    leveltype level;

    if (!cbus)
	return;
    sse = cbus->sseptr;
    if (!sse || !sse->real)
	return;

    level = raise_interrupt_level(NETS_DISABLE);
    regptr = cbus->regptr;
    regptr->argreg = SSE_OFF;
    regptr->cmdreg = FCI_SSE_CONTROL;
    response = regptr->cmdreg;
    reset_interrupt_level(level);
    if (response != MCI_RSP_OKAY)
	errmsg(&msgsym(HWFAILURE, SSE), "sse_hw_off", response);
    sse->state &= ~SSE_STATE_UCODE_ON;
    sse_manager_wakeup = TRUE;
}

/*
 * sse_hw_on
 * Enable SSE hardware.
 */

static void sse_hw_on (mcitype *cbus)
{
    mciregtype *regptr;
    int response;
    leveltype level;
    ssetype *sse;

    if (!cbus)
	return;
    sse = cbus->sseptr;
    if (!sse)
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr = cbus->regptr;
    regptr->argreg = SSE_ON;
    regptr->cmdreg = FCI_SSE_CONTROL;
    response = regptr->cmdreg;
    reset_interrupt_level(level);
    if (response != MCI_RSP_OKAY)
	errmsg(&msgsym(HWFAILURE, SSE), "sse_hw_on", response);
    sse->state |= SSE_STATE_UCODE_ON;
}

/*
 * sse_hw_init
 * Make sure the hardware is ready to go.
 */

static void sse_hw_init (void)
{
    mcitype *cbus;
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if (!(idb->status & IDB_CBUS))
	    continue;
	if (idb->status & IDB_NOHARDWARE)
	    continue;
	cbus = idb->devctl;
	if (!cbus->sseptr)
	    continue;

	sse_set_mtu(idb, idb->firstsw, 0);
    }

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) 
	sse_hw_on(cbus);
}

/*
 * sse_soft_reset
 * Shut down the hardware, tell everyone to recompute, and flag the manager
 * to start things up again.
 */

static void sse_soft_reset (mcitype *cbus)
{
    ssetype *sse;

    if (sse_debug)
	bugtrace();
    sse = cbus->sseptr;
    if (!sse)
	return;
    sse_hw_off(cbus);
    reg_invoke_sse_protocols_invalid(&sse->protoinfo);
    sse->state |= SSE_STATE_SWRESET;
}

/*
 * sse_sw_reset
 * Conditionally reinitialize if we have to.
 */

void sse_sw_reset (ssetype *sse, uint treememsize, uint rwmemsize)
{
    int i, j;
    rwdbtype *rwdb, *next;
    
    /*
     * Zero all fields out nicely.
     */
    if (sse_debug)
	buginf("\nSSE: software reset");
    sse->state &= ~SSE_STATE_INITIALIZED;
    sse_slow_free_tree(sse, sse->inputif, &sse->overheadstat);
    sse->inputif = NULL;
    for (i = 0; i < SSE_MAXINDEX; i++) {
	if (sse_debug && (sse->stats[i].words_used != 0)) {
	    buginf("SSE: freeing %d words of memory for %s",
		   sse->stats[i].words_used, sse_protocol_name(i));
	}
	sse_slow_free_tree(sse, sse->protocol_tree[i], &sse->stats[i]);
	sse->protocol_tree[i] = NULL;
	sse->depth[i] = 0;
	memset(&sse->stats[i], 0, sizeof(ssestattype));
    }    
    memset(&sse->overheadstat, 0, sizeof(ssestattype));
    for (i = 0; i < NETHASHLEN; i ++) {
	rwdb = sse->rewrite_table[i];
	sse->rewrite_table[i] = NULL;
	for (; rwdb; rwdb = next, next = rwdb->next) {
	    next = rwdb->next;
	    j = mem_refcount(rwdb);
	    while (j--)
		free(rwdb);
	}
    }

    /*
     * Clean up any cruft lying around.
     */
    while (!QUEUEEMPTY(&spinfreeQ))
	free(dequeue(&spinfreeQ));

    reg_invoke_sse_reinit(sse, &sse->protoinfo);

    /*
     * Start memory allocation all over again.
     */
    if (sse->freetreememory) {
	rangelist_destroy(sse->freetreememory);
	sse->freetreememory = NULL;
    }
    if (sse->freerwmemory) {
	rangelist_destroy(sse->freerwmemory);
	sse->freerwmemory = NULL;
    }
}	

/*
 * sse_init
 * Initialize an sse.  If the SSE manager isn't running, start it.  Can be
 * called from interrupt or process level.
 */

void sse_init (mcitype *cbus)
{
    ssetype *sse;

    if (!cbus)
	return;
    cbus->sse_present = reg_invoke_sse_present();
    sse = cbus->sseptr;

    if (sse_debug) {
	buginf("\nSSE: SSE initializing");
	bugtrace();
    }

    if (sse) {
	sse->state |= (SSE_STATE_HWRESET|SSE_STATE_SWRESET);
	sse->state &= ~(SSE_STATE_UCODE_ON|SSE_STATE_INITIALIZED);
    }

    sse_manager_wakeup = TRUE;
    
    if (sse_manager_running)
	return;
    if (!cbus->sse_present)
	return;

    /*
     * All of this stuff only happens once, at boot time.
     */
    sse_manager_pid = cfork(sse_manager, 0, 0, "SSE Manager", 0);
    if (sse_manager_pid == NO_PROCESS)
	return;
    sse_manager_running = TRUE;
}

/*
 * sse_finish_reset
 *
 * Finish the reset process after the board has been initialized.
 */

void
sse_finish_reset (mcitype *cbus, uint treememsize, uint rwmemsize, boolean
		  real)
{
    ssetype *sse;

    if (sse_debug)
	bugtrace();

    if (!cbus->sseptr) {
	sse = malloc(sizeof(ssetype));
	if (!sse) {
	    return;
	}
	cbus->sseptr = sse;
	sse->state = SSE_STATE_HWRESET|SSE_STATE_SWRESET;
	reg_invoke_sse_data_init(&sse->protoinfo);
	sse->protoinfo.sseptr = sse;
    } else 
	sse_sw_reset(cbus->sseptr, treememsize, rwmemsize);
    sse = cbus->sseptr;
    sse->cbus = cbus;
    sse->state &= ~SSE_STATE_HWRESET;

    /*
     * Save memory sizes.
     */
    sse->words_avail = treememsize;
    sse->words_used = 0;
    sse->rwbytes_avail = rwmemsize;
    sse->rwbytes_used = 0;
    sse->real = real;
    process_may_suspend();

    if (!sse_ssetype_init(cbus, sse))
	return;
    
    /*
     * Ok, memory has now been written, tell the protocols that they need
     * to recompute.
     */
    reg_invoke_sse_protocols_invalid(&sse->protoinfo);
    sse->state |= SSE_STATE_INITIALIZED;
}

/*
 * sse_error
 * Called if the SSE has looped or wedged.
 */

void sse_error (mcitype *cbus)
{
    ssetype *sse;

    sse_hw_off(cbus);
    sse = cbus->sseptr;
    if (!sse)
	return;

    reg_invoke_auto_sw_cache_preinit_callback();
    sse->hangcount++;
    if (sse->hangcount >= SSE_MAXHANG) {
	errmsg(&msgsym(HUNG, SSE), "disabling");
	sse_manager_blocked = TRUE;
	reg_invoke_sse_bus_reset(cbus, cbus->critical);
	return;
    }
    errmsg(&msgsym(HUNG, SSE), "resetting");
    reg_invoke_sse_bus_reset(cbus, cbus->critical);
    reg_invoke_sse_protocols_invalid(&sse->protoinfo);
    sse->state |= SSE_STATE_HWRESET;
    sse_manager_wakeup = TRUE;
}

/*
 * sse_soft_error
 * Used during software exceptions to disable the SSE.  Looks similar to a
 * true SSE hang.  The caller should either buginf or errmsg.
 */

static void sse_soft_error (ssetype *sse)
{
    sse_hw_off(sse->cbus);
    sse->state |= SSE_STATE_UCODE_DISABLED;
    sse_manager_blocked = TRUE;
    sse->hangcount++;
    if (sse_debug)
	buginf("\nSSE: manager disabled on soft error");
}

/*
 * sse_hwif_goingdown
 * Called here if we see an interface shutdown manually.
 */

static void sse_hwif_goingdown (hwidbtype *hwidb)
{
    mcitype *cbus;
    ssetype *sse;
    
    if (!hwidb) {	/* if hwidb is NULL, flush everything */
	if (sse_debug) {
	    buginf("\nSSE: flushing cache");
	    bugtrace();
	}
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    sse = cbus->sseptr;
	    if (!sse)
		continue;
	    sse_hw_off(cbus);
	    reg_invoke_sse_idb_invalid(&sse->protoinfo);
	}
    } else {
	if (!(hwidb->status & IDB_CBUS))
	    return;
	if (hwidb->status & IDB_NOHARDWARE)
	    return;
	cbus = hwidb->devctl;
	sse = cbus->sseptr;
	if (!sse)
	    return;
	sse_hw_off(cbus);
	if (sse_debug) {
	    idbtype *idb;
	    idb = firstsw_or_null(hwidb);
	    buginf("\nSSE: interface invalidation for %s", idb ?
		   idb->namestring : "all interfaces");
	}
	reg_invoke_sse_idb_invalid(&sse->protoinfo);
    }
}

/*
 * sse_bump_cache_version
 * Make like the hardware is going down if we're asked to invalidate
 * immediately.
 */

static void sse_bump_cache_version (hwidbtype *hwidb, 
			     idbtype *swidb,
			     boolean immediately)

{
    if (!immediately)
	return;				/* manager will pick up */
    
    sse_hwif_goingdown(hwidb);
}

/*
 * sse_protoinfo
 *
 * Stub routine for data hiding.
 */

static sse_protoinfo *sse_proto_info (ssetype *sse)
{
    return(&sse->protoinfo);
}



/*
 *		      Manager process and scheduling
 */


/*
 * sse_wake_manager
 * Wake up the SSE manager.  Called from other modules via a registry callback.
 */

static void sse_wake_manager (void)
{
    sse_manager_wakeup = TRUE;
}

/*
 * sseBLOCK
 * Dispatch routine for sse_manager.  Wake up if our timer went off or
 * if some protocol thinks it needs to be updated.  
 */

static boolean sseBLOCK (void)
{
    mcitype *cbus;
    ssetype *sse;

    /*
     * Block if we've been turned off.
     */
    if (sse_manager_blocked)
	return(TRUE);

    /*
     * Don't block if something is seriously wrong.
     */
    if (sse_manager_wakeup)
	return(FALSE);

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	if (reg_invoke_sse_fast_dispatch(&sse->protoinfo))
	    return(FALSE);
    }

    /*
     * Block if we haven't had some sleep since the last time we ran.
     */
    if (SLEEPING(sse_minsleep)) 
	return(TRUE);
    
    /*
     * Don't block if a protocol needs updating.
     */
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	if (reg_invoke_sse_dispatch(&sse->protoinfo))
	    return(FALSE);
    }
    
    /*
     * Don't block if we've slept a long while.  This guarantees that we
     * periodically redo things to optimize the tree balancing.
     */
    if (SLEEPING(sse_periodic)) 
	return(TRUE);
    if (sse_debug)
	buginf("\nSSE: periodic update");
    return(FALSE);
}

/*
 * sse_manager
 * Process to manage the SSE.
 */
static forktype sse_manager (void)
{
    sys_timestamp start;
    mcitype *cbus;
    ssetype *sse;
    ulong iter_runtime;

    edisms((blockproc *)systeminitBLOCK, 0);
    while (TRUE) {
	TIMER_START(sse_periodic, sse_manager_period);
	TIMER_START(sse_minsleep, sse_manager_quiet);
	edisms((blockproc *)sseBLOCK, 0);
	sse_manager_wakeup = FALSE;
	sse_quantum = iter_runtime = runtime();
	GET_TIMESTAMP(start);

	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    sse = cbus->sseptr;
	    if (!sse || !cbus->sse_present ||
		(sse->state & SSE_STATE_ATTENTION)) {
		if ((cbus->sse_present && !sse) ||
		    (!cbus->sse_present && sse) ||
		    (sse && (sse->state & SSE_STATE_ATTENTION))) {
		    reg_invoke_sse_board_reset(cbus);
		    process_may_suspend();
		}
		if (sse_debug)
		    buginf("\nSSE: reset took %ums, elapsed %ums",
			   runtime() - iter_runtime, ELAPSED_TIME(start));
		sse = cbus->sseptr;
		if (!sse)
		    continue;
		iter_runtime = runtime();
	    } 
	    
	    /*
	     * Update the cache for this SSE for all supported protocols if
	     * they think they need updates.  
	     */
	    reg_invoke_sse_update(sse, &sse->protoinfo);
	    process_may_suspend();
	    sse_build_icbtree(sse);
	    process_may_suspend();
	    
	    /*
	     * Clean up anything left over in the rewrite cache.
	     */
	    sse_match_gc(sse);
	    process_may_suspend();
	    sse_rw_gc(sse);
	    process_may_suspend();

	    /*
	     * If we haven't tweaked the SSE before, then this actually
	     * gets it going.
	     */
	    if ((sse->state & (SSE_STATE_UCODE_ON|
			       SSE_STATE_UCODE_DISABLED)) == 0) {

		/*
		 * Kick the board to start it.  Then kick the ucode.
		 */
		if (sse->real) 
		    sse_hw_init();
	    }
	}
	
	/*
	 * Make sure that we gradually return unused memory.
	 */
	if (!QUEUEEMPTY(&spinfreeQ))
	    free(dequeue(&spinfreeQ));

	iter_runtime = runtime() - iter_runtime;
	if (iter_runtime > sse_max_iteration)
	    sse_max_iteration = iter_runtime;
	if (sse_debug)
	    buginf("\nSSE: cache update took %ums, elapsed %ums",
		   iter_runtime, ELAPSED_TIME(start));
    }
}


/*
 * sse_real
 * Silly interface function.  For use by clients so that they don't have to
 * include our private data structures.
 */

boolean sse_real (ssetype *sse)
{
    if (!sse)
	return(FALSE);
    return(sse->real);
}



/*
 *			 Malloc and free routines
 */


/*
 * sse_malloc_spin
 * Malloc and initialize an spintype node.
 */

inline spintype *sse_malloc_spin (int protocol, ssestattype *stat)
{
    spintype *spin;
    
    if (QUEUEEMPTY(&spinfreeQ)) {
	spin = malloc(sizeof(spintype));
	if (!spin) {
	    return (NULL);
	}
    } else {
	spin = dequeue(&spinfreeQ);
	memset(spin, 0, sizeof(spintype));
    }
    spin->type = SPIN_COMPARE;
    spin->physaddr = SSE_UNMAPPED;
    spin->parents = 1;
    spin->protocol = protocol;
    spin_malloced++;
    if (stat)
	stat->spin_used++;
    return(spin);
}

/*
 * sse_free_spin
 * Free a spintype node.
 */

static inline void sse_free_spin (spintype *spin)
{
    spin->walk = NULL;
    enqueue(&spinfreeQ, spin);
}

/*
 * sse_malloc_treeaddr
 * Allocate a tree address.
 */

inline treeaddrtype sse_malloc_treeaddr (ssetype *sse, ssestattype *stat)
{
    uint addr;
    
    if (!rangelist_malloc_one(sse->freetreememory, &addr)) {
	if (sse_debug)
	    buginf("\nSSE: out of program memory");
	sse->outofmem_count++;
	return(0);
    }
    sse->words_used++;
    if (stat)
	stat->words_used++;
    return((treeaddrtype) addr);
}

/*
 * sse_free_treeaddr
 * Free a tree address.
 */

void sse_free_treeaddr (ssetype *sse, ssestattype *stat, int addr)
{
    if (addr == SSE_UNMAPPED) {
	errmsg(&msgsym(SWFAILURE, SSE), "unmapped address returned",
	       -1);
	return;
    }
    if (!rangelist_return_one(sse->freetreememory, addr)) {
	errmsg(&msgsym(SWFAILURE, SSE), "address return failure -- address",
	       addr);
	sse_soft_error(sse);
	return;
    }
    sse->words_used--;
    if (stat)
	stat->words_used--;
}



/*
 *		  Data structure initialization routines
 */


/*
 * sse_init_spin
 * Set up instructions and values for a cell.
 */

inline void sse_init_spin (spintype *spin, treecmptype value,
			   treeinsttype lt_inst, treeinsttype eq_inst,
			   treeinsttype gt_inst) 
{
    spin->value = value;
    spin->nodeinfo.compare.lt.inst = lt_inst;
    spin->nodeinfo.compare.eq.inst = eq_inst;
    spin->nodeinfo.compare.gt.inst = gt_inst;
}



/*
 *			 Rewrite table maintenance
 */


/*
 * sse_find_rwdb
 * Find or create a rewrite entry in the table.
 */

static rwdbtype *sse_find_rwdb (ssetype *sse, int protocol, void *encaps,
				uint len)
{
    int hashvalue, i;
    ulong *ep;
    rwdbtype *rwdb;
    
    /*
     * Grovel the hash table, looking for a matching entry.  If we find
     * one, increase the ref count and return a pointer.
     */
    hashvalue = protocol;
    for (i = len >> 2, ep = (ulong *) encaps; i; i--, ep++)
	hashvalue ^= nethash(*ep);
    for (rwdb = sse->rewrite_table[hashvalue]; rwdb;
	 rwdb = rwdb->next) {
	if ((rwdb->len == len) &&
	    (rwdb->proto == protocol)) {
	    if (!bcmp(rwdb->encaps, encaps, len)) {
		mem_lock(rwdb);
		return(rwdb);
	    }
	}
    }
    
    /*
     * If we're here, we didn't find an entry.  Go ahead and create one.
     */
    rwdb = malloc(sizeof(rwdbtype) + len);
    if (!rwdb) {
	return (NULL);
    }
    rwdb->len = len;
    rwdb->proto = protocol;
    rwdb->physaddr = SSE_UNMAPPED;
    bcopy(encaps, rwdb->encaps, len);
    rwdb->next = sse->rewrite_table[hashvalue];
    sse->rewrite_table[hashvalue] = rwdb;
    mem_lock(rwdb);
    return(rwdb);
}

/*
 * sse_rw_gc
 * Garbage collect the rewrite cache.
 */

static void sse_rw_gc (ssetype *sse)
{
    int i;
    rwdbtype *curr, *pred;
    
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (rwdbtype *) &sse->rewrite_table[i];
	for (curr = pred->next; curr; pred = curr, curr = pred->next) {
	    
	    /*
	     * Entries with only one reference aren't used by anything in
	     * any tree.  Throw them out.
	     */
	    if (mem_refcount(curr) == 1) {
		if (curr->physaddr != SSE_UNMAPPED) {
		    rangelist_return_block(sse->freerwmemory, curr->start,
					   curr->start + curr->len + 4 - 1); 
		    sse->rwbytes_used -= curr->len + 4;
		    sse->stats[curr->proto].rwbytes_used -= curr->len + 4;
		}
		pred->next = curr->next;
		curr->next = NULL;
		free(curr);
		curr = pred;
	    }
	}
    }
}

/*
 * sse_map_rwdb
 * Walk the rwdb.  For each entry which is not currently in rewrite memory,
 * copy it down to the board.  Current format:
 * [pad] encaps len 
 * OUTPTR points to the len field, which must be at the start of an even
 * word (i.e., longword aligned).
 * If we run out of memory, don't map that entry.  Return FALSE if there was
 * a failure, TRUE if everything went well.
 * Optimiation to be done: After calculating the offset of the rewrite
 * entry, there will be three bytes which can be freed.  
 */

static boolean sse_map_rwdb (ssetype *sse, int protocol)
{
    int i;
    uint start, offset, end;
    rwdbtype *curr, *pred;
    boolean success;
    
    success = TRUE;
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (rwdbtype *) &sse->rewrite_table[i];
	for (curr = pred->next; curr; pred = curr, curr = pred->next) {
	    if (curr->physaddr != SSE_UNMAPPED) 
		continue;
	    if (curr->proto != protocol)
		continue;
	    if (curr->len > 255) {
	        success = FALSE;
		continue;
	    }
	    if (!rangelist_malloc_block(sse->freerwmemory, curr->len + 4, 
					&start)) {
		success = FALSE;
		continue;
	    }
	    curr->start = start;
	    
	    /*
	     * The length byte needs to be longword aligned.  Do this by
	     * calculating the natural end of the rewrite and then figuring
	     * out the necessary change in the start address to get the
	     * correct alignment.
	     */
	    end = start + curr->len - 1;/* end of rewrite */
	    end = end & 0x3;		/* low order bits indicate
					   alignment */ 
	    offset = 3 - end;		/* how much to shift */
	    start += offset;
	    sse_poke_rwmem(sse, curr->encaps, start, curr->len);
	    start += curr->len;
	    curr->physaddr = start;	/* point to len */
	    sse_poke_rwmem(sse, &curr->len, start, 1);
	    sse->rwbytes_used += curr->len + 4;
	    sse->stats[protocol].rwbytes_used += curr->len + 4;
	}
    }
    return(success);
}




/*
 *		       Match node cache maintenance
 */


/*
 * sse_find_match
 * Find or create a match cache entry
 */

static spintype *sse_find_match (ssetype *sse, spintype *matchproto)
{
    int hashvalue;
    matchtype *curr;
    spintype *spin;

    /*
     * Grovel the hash table, looking for a matching entry.  If we find
     * one, increase the ref count and return a pointer.  Hash based on the
     * output ICB and rewrite entry address.
     */
    hashvalue = nethash((int)
			((int)matchproto->OUTRW ^ (int)matchproto->value));
    for (curr = sse->match_cache[hashvalue]; curr;
	 curr = curr->next) {
	spin = curr->node;
	if ((spin->protocol == matchproto->protocol) &&
	    (spin->protoinfo == matchproto->protoinfo) &&
	    (spin->value == matchproto->value) &&
	    (spin->type == matchproto->type) &&
	    (spin->depth == matchproto->depth) &&
	    (spin->OUTRW == matchproto->OUTRW) &&
	    (spin->EXTRACOUNT == matchproto->EXTRACOUNT) &&
	    (spin->REMAINDER == matchproto->REMAINDER) &&
	    (spin->nodeinfo.match.remainder ==
	     matchproto->nodeinfo.match.remainder)) {

	    /*
	     * If we found a match, the sse_add locked matchproto->OUTRW.
	     * This bolixes the reference count, so clean that up now.
	     */
	    free(matchproto->OUTRW);
	    matchproto->OUTRW = NULL;
	    sse_spinref_plus1(spin);
	    return(spin);
	}
    }
    
    /*
     * If we're here, we didn't find an entry.  Go ahead and create one.
     */
    curr = malloc(sizeof(matchtype));
    if (!curr)
	return(NULL);
    curr->node = sse_malloc_spin(matchproto->protocol,
				 &sse->stats[matchproto->protocol]);
    if (!curr->node) {
	free(curr);
	return(NULL);
    }
    matchproto->physaddr = SSE_UNMAPPED;	
    matchproto->parents = 1;
    matchproto->type = SPIN_MATCH;
    *curr->node = *matchproto;
    mem_lock(curr->node);
    curr->next = sse->match_cache[hashvalue];
    sse->match_cache[hashvalue] = curr;
    return(curr->node);
}

/*
 * sse_match_gc
 * Garbage collect the match cache.
 */

static void sse_match_gc (ssetype *sse)
{
    int i;
    matchtype *curr, *pred;
    ssestattype *stat;
    spintype *spin;

    stat = NULL;
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (matchtype *) &sse->match_cache[i];
	for (curr = pred->next; curr; pred = curr, curr = pred->next) {
	    
	    /*
	     * Entries with only one reference aren't used by anything in
	     * any tree.  Throw them out.
	     */
	    spin = curr->node;
	    if (mem_refcount(spin) == 1) {
		if (spin->physaddr != SSE_UNMAPPED) {
		    stat = &sse->stats[spin->protocol];
		    if (spin->OUTRW) {
			free(spin->OUTRW);
			spin->OUTRW = NULL;
		    }
		    for (i = 0; i < spin->EXTRACOUNT; i++) {
			if (spin->EXTRA[i] != SSE_UNMAPPED) {
			    sse_free_treeaddr(sse, stat, spin->EXTRA[i]);
			    spin->EXTRA[i] = SSE_UNMAPPED;
			}
		    }
		    spin->EXTRACOUNT = 0;
		    sse_free_treeaddr(sse, stat, spin->physaddr);
		}
		pred->next = curr->next;
		curr->next = NULL;
		if (stat)
		    stat->spin_used--;
		sse_free_spin(spin);
		free(curr);
		curr = pred;
	    }
	}
    }
}



/*
 *			    Tree walk routines
 */


/*
 * sse_tree_walk
 * Walk a spin tree.  On each node, call the function that we've been
 * passed.  Warning: the callback routine MUST NOT block.  If you want
 * that, use sse_safe_tree_walk.
 * MAINTAIN sse_safe_tree_walk IDENTICALLY!
 */

int sse_tree_walk (ssetype *sse, spintype *spin, sse_callback func,
		   sse_recalltype recall, ssestattype *stat, queuetype *queue)
{
    static uint time = 47;
    uint localtime;
    sse_tree_walk_opcodes opcode;
    
    localtime = time++;
    while (spin) {	
	
	/*
	 * Deal with the current node first.
	 */
	if (spin->time != localtime) {
	    opcode = func(sse, spin, stat, queue);
	    spin->time = localtime;
	    
	    switch (opcode) {
	    case OPCODE_WALK:
		break;
	    case OPCODE_ABORT:
		return(OPCODE_ABORT);
	    case OPCODE_PARENT:
		spin = spin->walk;
		continue;
	    case OPCODE_CALL:
		if (recall)
		    spin = recall(sse, spin, localtime, stat);
		continue;
	    }
	}
	
	/*
	 * The only way to walk from a MATCH node is to go up.
	 */
	if (spin->type == SPIN_MATCH) {
	    spin = spin->walk;
	    continue;
	}
	
	/*
	 * Otherwise, we're walking the tree.  Find a child that we haven't
	 * walked yet and walk in that direction.
	 */
	if (spin->LTADDR && (spin->LTADDR->time != localtime)) {
	    spin->LTADDR->walk = spin;
	    spin = spin->LTADDR;
	    continue;
	} else if (spin->EQADDR && (spin->EQADDR->time != localtime)) {
	    spin->EQADDR->walk = spin;
	    spin = spin->EQADDR;
	    continue;
	} else if (spin->GTADDR && (spin->GTADDR->time != localtime)) {
	    spin->GTADDR->walk = spin;
	    spin = spin->GTADDR;
	    continue;
	} else 
	    spin = spin->walk;
    }
    return(OPCODE_WALK);
}

/*
 * sse_safe_tree_walk
 * Walk a spin tree.  On each node, call the function that we've been
 * passed.  This is protected by mem_lock() and free() and can be used if
 * the callback routing would block.  MAINTAIN sse_tree_walk IDENTICALLY!
 */

int sse_safe_tree_walk (ssetype *sse, spintype *spin, sse_callback func,
			sse_recalltype recall, ssestattype *stat, queuetype
			*queue) 
{
    static uint time = 47;
    uint localtime;
    sse_tree_walk_opcodes opcode;
    
    localtime = time++;
    while (spin) {	
	
	/*
	 * Deal with the current node first.
	 */
	if (spin->time != localtime) {
	    mem_lock(spin);
	    opcode = func(sse, spin, stat, queue);
	    if (mem_refcount(spin) == 1) {
		free(spin);
		return(OPCODE_ABORT);
	    }
	    free(spin);
	    spin->time = localtime;
	    
	    switch (opcode) {
	    case OPCODE_WALK:
		break;
	    case OPCODE_ABORT:
		return(OPCODE_ABORT);
	    case OPCODE_PARENT:
		spin = spin->walk;
		continue;
	    case OPCODE_CALL:
		if (recall)
		    spin = recall(sse, spin, localtime, stat);
		continue;
	    }
	}
	
	/*
	 * The only way to walk from a MATCH node is to go up.
	 */
	if (spin->type == SPIN_MATCH) {
	    spin = spin->walk;
	    continue;
	}
	
	/*
	 * Otherwise, we're walking the tree.  Find a child that we haven't
	 * walked yet and walk in that direction.
	 */
	if (spin->LTADDR && (spin->LTADDR->time != localtime)) {
	    spin->LTADDR->walk = spin;
	    spin = spin->LTADDR;
	    continue;
	} else if (spin->EQADDR && (spin->EQADDR->time != localtime)) {
	    spin->EQADDR->walk = spin;
	    spin = spin->EQADDR;
	    continue;
	} else if (spin->GTADDR && (spin->GTADDR->time != localtime)) {
	    spin->GTADDR->walk = spin;
	    spin = spin->GTADDR;
	    continue;
	} else 
	    spin = spin->walk;
    }
    return(OPCODE_WALK);
}

/*
 * sse_free_recall
 * This is called back to really free the node.  Nodes can't be REALLY
 * freed while spp_tree_walk has a hold of them as it would disrupt the
 * walk. 
 */

spintype *sse_free_recall (ssetype *sse, spintype *spin, uint localtime,
			   ssestattype *stat)
{
    spintype *parent;
    int i;

    parent = spin->walk;
    if (parent) {
	
	/*
	 * Note: only free one pointer at a time here because we have one
	 * refcount per pointer.  Other routines depend on the ordering
	 * here.
	 */
	if (parent->LTADDR == spin) 
	    parent->LTADDR = NULL;
	else if (parent->EQADDR == spin) 
	    parent->EQADDR = NULL;
	else if (parent->GTADDR == spin) 
	    parent->GTADDR = NULL;
	else {
	    /*
	     * We've hit an inconsistent tree.
	     */
	    errmsg(&msgsym(SWFAILURE, SSE), "sse_free_recall unconnected "
		   "node");
	    crashdump(10);
	    return(NULL);
	}

	/*
	 * Remember to go back and walk the parent again, as it's state has
	 * just changed.  We may want to free it.
	 */
	parent->time = localtime - 1;
    }
    
    /*
     * Clean up any pointers and actually free the spin.
     */
    if (1 == spin->parents) {
	if (spin->physaddr != SSE_UNMAPPED) {
	    sse_free_treeaddr(sse, stat, spin->physaddr);
	    spin->physaddr = SSE_UNMAPPED;
	}
	switch(spin->type) {
	case SPIN_MATCH:
	    if (spin->OUTRW) {
		free(spin->OUTRW);
		spin->OUTRW = NULL;
	    }
	    for (i = 0; i < spin->EXTRACOUNT; i++) {
		if (spin->EXTRA[i] != SSE_UNMAPPED) {
		    sse_free_treeaddr(sse, stat, spin->EXTRA[i]);
		    spin->EXTRA[i] = SSE_UNMAPPED;
		}
	    }
	    spin->EXTRACOUNT = 0;
	    break;
	default:
	    spin->LTADDR = NULL;
	    spin->GTADDR = NULL;
	    spin->EQADDR = NULL;
	    break;
	}
	spin_freed++;
	if (stat)
	    stat->spin_used--;
	sse_tree_nodecount++;
	spin->parents--;
	if (spin->type == SPIN_MATCH) {
	    free(spin);			/* let the match database worry */
	} else {
	    sse_free_spin(spin);
	}
    } else {
	spin->time = localtime - 1;	/* possibly return here again */
	spin->parents--;
	if (mem_refcount(spin) == 1) {
	    errmsg(&msgsym(SWFAILURE, SSE), "spin refcount bad", spin);
	    crashdump(10);
	}
	free(spin);
    }
    return(parent);
}

/*
 * sse_free_nodes
 * Free a spin node in the current tree.  This function is used to walk the
 * tree.  To actually free the node, it forces a callback (via OPCODE_CALL)
 * of another routine to do the work.  So all it really has to do is to
 * decide if a node can be freed.
 */

static int sse_free_nodes (ssetype *sse, spintype *spin, ssestattype *stat,
			   queuetype *queue)
{
    switch(spin->type) {
    case SPIN_MATCH:
	return(OPCODE_CALL);
    default:
	if (!spin->LTADDR && !spin->EQADDR && !spin->GTADDR)
	    return(OPCODE_CALL);
	break;
    }
    return(OPCODE_WALK);
}

/*
 * sse_slow_free_nodes
 * Free a spin node in the current tree.  This function is used to walk the
 * tree.  To actually free the node, it forces a callback (via OPCODE_CALL)
 * of another routine to do the work.  So all it really has to do is to
 * decide if a node can be freed.  Differs from the above in that it will
 * suspend. 
 */

static int sse_slow_free_nodes (ssetype *sse, spintype *spin,
				ssestattype *stat, queuetype *queue)
{
    switch(spin->type) {
    case SPIN_MATCH:
	return(OPCODE_CALL);
    default:
	if (!spin->LTADDR && !spin->EQADDR && !spin->GTADDR)
	    return(OPCODE_CALL);
	break;
    }
    process_may_suspend();
    return(OPCODE_WALK);
}

/*
 * sse_free_tree
 * Free an entire tree.  Just call the tree walk with appopriate arguments. 
 */

void sse_free_tree (ssetype *sse, spintype *spin, ssestattype *stat)
{
    if (!spin)
	return;
    sse_tree_walk(sse, spin, sse_free_nodes, sse_free_recall, stat, NULL); 
}

/*
 * sse_slow_free_tree
 * Free an entire tree.  Just call the tree walk with appopriate arguments.
 * Differs from the above because it suspends.
 */

void sse_slow_free_tree (ssetype *sse, spintype *spin, ssestattype *stat)
{
    if (!spin)
	return;
    sse_tree_walk(sse, spin, sse_slow_free_nodes, sse_free_recall, stat,
		  NULL);
}

/*
 * sse_gc_tree
 * Garbage collect any nodes in the spin tree that point to Hilbert 
 * space.  Compare nodes which aren't mapped to tree memory, or don't point
 * anywhere, or match nodes
 * which point to rewrite entries that haven't been mapped to the rewrite
 * cache are all good things to get rid of.
 */

int sse_gc_tree (ssetype *sse, spintype *spin, ssestattype *stat, 
		 queuetype *queue) 
{
    int i;
    
    switch(spin->type) {
    case SPIN_MATCH:
	if (spin->physaddr == SSE_UNMAPPED) {
	    if (spin->OUTRW) {
		free(spin->OUTRW);
		spin->OUTRW = NULL;
	    }
	    return(OPCODE_CALL);
	}
	if (spin->OUTRW &&
	    (spin->OUTRW->physaddr == SSE_UNMAPPED)) {
	    sse_free_treeaddr(sse, stat, spin->physaddr);
	    spin->physaddr = SSE_UNMAPPED;
            for (i = 0; i < spin->EXTRACOUNT; i++) {
	        if (spin->EXTRA[i] != SSE_UNMAPPED) {
		    sse_free_treeaddr(sse, stat, spin->EXTRA[i]);
		    spin->EXTRA[i] = SSE_UNMAPPED;
		}
		if (spin->OUTRW) {
		    free(spin->OUTRW);
		    spin->OUTRW = NULL;
		}
	    }
	    spin->EXTRACOUNT = 0;
	    return(OPCODE_CALL);
	}
	break;
    case SPIN_COMPARE:
    case SPIN_RANDOM:
	if ((spin->LTADDR == 0) && (spin->EQADDR == 0) &&
	    (spin->GTADDR == 0)) {
	    if (spin->physaddr != SSE_UNMAPPED) {
		sse_free_treeaddr(sse, stat, spin->physaddr);
		spin->physaddr = SSE_UNMAPPED;
	    }
	    return(OPCODE_CALL);
	}
	break;
    default:
	if (sse_debug)
	    buginf("\nSSE: sse_gc_program unknown type");
	break;
    }
    return(OPCODE_WALK);
}

/*
 * sse_address_node
 * Give a physical address to a spin node.
 */

int sse_address_node (ssetype *sse, spintype *spin, ssestattype *stat, 
		      queuetype *queue)
{
    uint addr;
    int i, j;
    
    if (spin->physaddr != SSE_UNMAPPED)
	return(OPCODE_WALK);
    addr = sse_malloc_treeaddr(sse, stat);
    if (!addr)
	return(OPCODE_ABORT);
    spin->physaddr = addr;
    if (spin->type != SPIN_MATCH) 
	return(OPCODE_WALK);
    
    /*
     * For match nodes, we have to allocate extra addresses for
     * instructions to write result registers.
     */
    if (spin->EXTRACOUNT > SSE_MAXEXTRA) {
	errmsg(&msgsym(SWFAILURE, SSE), "bad request for extra addresses",
	       spin->EXTRACOUNT);
	return(OPCODE_ABORT);
    }
    for (i = 0; i < spin->EXTRACOUNT; i++) {
        addr = sse_malloc_treeaddr(sse, stat);
        if (!addr) {
	    errmsg(&msgsym(SWFAILURE, SSE), "can't allocate memory for"
		   "match node", spin->EXTRACOUNT);
	    sse_soft_error(sse);
   	    sse_free_treeaddr(sse, stat, spin->physaddr);
   	    spin->physaddr = SSE_UNMAPPED;
	    i--;
            for (j = 0; j < i; j++) {
	        sse_free_treeaddr(sse, stat, spin->EXTRA[j]);
		spin->EXTRA[j] = SSE_UNMAPPED;
	    }
	    spin->EXTRACOUNT = 0;
	    return(OPCODE_ABORT);
        }
        spin->EXTRA[i] = addr;
    }
    return(OPCODE_PARENT);
}

/*
 * sse_calc_compare
 * Calculate the code for a compare node.
 */

static inline boolean
sse_calc_compare (spintype *spin, shadowwordtype *sw)
{
    sse_init_sw(sw, 0, SSE_INST_NOP, spin->physaddr); /* default is hang */
    sw->lt.inst = spin->nodeinfo.compare.lt.inst;
    sw->eq.inst = spin->nodeinfo.compare.eq.inst;
    sw->gt.inst = spin->nodeinfo.compare.gt.inst;
    if (spin->LTADDR) {
	if (spin->LTADDR->physaddr == SSE_UNMAPPED)
	    return(FALSE);
	sw->lt.ptr = spin->LTADDR->physaddr;
	sw->lt.value = spin->LTADDR->value;
    } else
	sw->lt.ptr = SSE_GLOBAL_RP;    
    if (spin->EQADDR) {
	if (spin->EQADDR->physaddr == SSE_UNMAPPED)
	    return(FALSE);
	sw->eq.ptr = spin->EQADDR->physaddr;
	sw->eq.value = spin->EQADDR->value;
    } else
	sw->eq.ptr = SSE_GLOBAL_RP;    
    if (spin->GTADDR) {
	if (spin->GTADDR->physaddr == SSE_UNMAPPED)
	    return(FALSE);
	sw->gt.ptr = spin->GTADDR->physaddr;
	sw->gt.value = spin->GTADDR->value;
    } else
	sw->gt.ptr = SSE_GLOBAL_RP;    
    return(TRUE);
}

/*
 * sse_gen_compare
 * Generate code for a compare and poke it.  Return FALSE on errors.
 */

static inline boolean
sse_gen_compare (ssetype *sse, spintype *spin)
{
    shadowwordtype sw;

    if (spin->physaddr == SSE_UNMAPPED) 
	return(FALSE);
    if (!sse_calc_compare(spin, &sw))
	return(FALSE);
    sse_poke_treemem(sse, spin->physaddr, &sw);
    return(TRUE);
}

/*
 * sse_backpatch
 * Fix the backpatch node for a protocol.
 */

static inline void
sse_backpatch (ssetype *sse, int protocol)
{
    shadowwordtype sw;
    spintype *spin;

    spin = sse->protocol_tree[protocol];
    sse_peek_treemem(sse, sse_backpatch_table[protocol], &sw);
    if (spin) {
	/*
	 * This is the new root of the tree.
	 * Backpatch the appropriate jump point into this table.
	 */
	sw.lt.ptr = spin->physaddr;
	sw.lt.value = spin->value;
	sw.eq.ptr = spin->physaddr;
	sw.eq.value = spin->value;
	sw.gt.ptr = spin->physaddr;
	sw.gt.value = spin->value;
	sse_poke_treemem(sse, sse_backpatch_table[protocol], &sw);
    } else {
	/*
	 * No tree.  Point everything back to the RP.
	 */
	sw.lt.ptr = SSE_GLOBAL_RP;
	sw.eq.ptr = SSE_GLOBAL_RP;
	sw.gt.ptr = SSE_GLOBAL_RP;
	sse_poke_treemem(sse, sse_backpatch_table[protocol], &sw);
    }
}

/*
 * sse_code_gen
 *
 * Generate code for a single spin node.  Be sure to change
 * SSE_*_EXTRA_INST defines to match instructions generated here.
 */

static int sse_code_gen (ssetype *sse, spintype *spin, ssestattype *stat, 
			 queuetype *queue)
{
    treeinsttype inst;
    uint remainder;
    shadowwordtype sw;
    physaddrtype addr;
    int i;

    addr = spin->physaddr;
    i = 0;
    switch (spin->type) {
    case SPIN_MATCH:

	remainder = spin->REMAINDER;
	/*
	 * If necessary, generate instructions to load the rest of the
	 * destination IP address.
	 */
	if (remainder) {
	    if (spin->protocol != SSE_PROTOCOL_IP) {
		if (sse_debug)
		    buginf("\nSSE: remainder set for protocol %d", spin->protocol);
		return(OPCODE_ABORT);
	    }
	    inst = (SSE_INST_OK|SSE_INST_LD_IPDST) +
		(sizeof(ipaddrtype) - remainder);
	    sse_init_sw(&sw, 0, inst, spin->EXTRA[i]);
	    if (addr == SSE_UNMAPPED) 
		return(OPCODE_ABORT);
	    sse_poke_treemem(sse, addr, &sw);
	    if (--remainder) {
		inst = (SSE_INST_OK|SSE_INST_LD_IPDST) +
		    (sizeof(ipaddrtype) - remainder);
		sse_init_sw(&sw, 0, inst, spin->EXTRA[i+1]);
		if (spin->EXTRA[i] == SSE_UNMAPPED) 
		    return(OPCODE_ABORT);
		sse_poke_treemem(sse, spin->EXTRA[i++], &sw);
		remainder--;
	    }
	    /*
	     * At this point, we should be done.
	     */
	    if (remainder) {
		if (sse_debug)
		    buginf("\nSSE: extra remainder bytes");
		return(OPCODE_ABORT);
	    }
	    addr = spin->EXTRA[i++];
	}
	/*
	 * Generate four nodes for a match node.  First, do the OUTICB,
	 * then the two OUTPTR nodes, and the OUTRWLEN.
	 */
	inst = SSE_INST_OUTICB;
	sse_init_sw(&sw, spin->value, inst, spin->EXTRA[i]);
	if (addr == SSE_UNMAPPED) 
	    return(OPCODE_ABORT);
	sse_poke_treemem(sse, addr, &sw);

	/*
	 * Start of OUTPTR
	 */
	inst = SSE_INST_OUTPTR1;
	sse_init_sw(&sw, (spin->OUTRW->physaddr >> SSE_ICB_SHIFT) & 0xff,
		    inst, spin->EXTRA[i+1]);
	if (spin->EXTRA[i] == SSE_UNMAPPED) 
	    return(OPCODE_ABORT);
	sse_poke_treemem(sse, spin->EXTRA[i++], &sw);
	
	/*
	 * End of OUTPTR
	 */
	inst = SSE_INST_OUTPTR2;
	sse_init_sw(&sw, ((spin->OUTRW->physaddr >> SSE_ICB_SHIFT) &
			   0xff00) >> 8, inst, spin->EXTRA[i+1]); 
	if (spin->EXTRA[i] == SSE_UNMAPPED) 
	    return(OPCODE_ABORT);
	sse_poke_treemem(sse, spin->EXTRA[i++], &sw);
	
	/*
	 * Do the OUTRWLEN
	 */
	inst = SSE_INST_OUTRWLEN;
	
	switch (spin->protocol) {
	case SSE_PROTOCOL_IP:
	    sse_init_sw(&sw, spin->OUTRW->len, inst, spin->protoinfo);
	    break;

	case SSE_PROTOCOL_CLNS:
	    sse_init_sw(&sw, spin->OUTRW->len, inst, spin->EXTRA[i+1]); 
	    if (spin->EXTRA[i] == SSE_UNMAPPED) 
		return(OPCODE_ABORT);
	    sse_poke_treemem(sse, spin->EXTRA[i++], &sw);
	    
	    /*
	     * Emit instruction to load length field rewrite pointer
	     */
	    sse_init_sw(&sw, spin->protoinfo, SSE_INST_OUTSNAPLEN,
			SSE_GLOBAL_SPIN);
	    if (sse_debug)
		buginf("\nSSE: sse_code_gen(CLNP) used snaplen of %d",
		       spin->protoinfo);

	    break;
	case SSE_PROTOCOL_IPX:
	    sse_init_sw(&sw, spin->OUTRW->len, inst, spin->EXTRA[i+1]); 
	    if (spin->EXTRA[i] == SSE_UNMAPPED) 
		return(OPCODE_ABORT);
	    sse_poke_treemem(sse, spin->EXTRA[i++], &sw);

	    /*
	     * Emit instruction to load length field rewrite pointer
	     */
	    sse_init_sw(&sw, spin->protoinfo, SSE_INST_OUTSNAPLEN,
			spin->EXTRA[i+1]); 
	    if (spin->EXTRA[i] == SSE_UNMAPPED)
		return(OPCODE_ABORT);
	    sse_poke_treemem(sse, spin->EXTRA[i++], &sw);

	    /*
	     * Emit instruction to get back hop count and
	     * jump to code to load TC register
	     */
	    sse_init_sw(&sw, 0, SSE_INST_RD_SREG_6, 
			 spin->nodeinfo.match.protoinfo2 ?
			 SSE_GLOBAL_NOVIGRPHOPCNT :
			 SSE_GLOBAL_NOVHOPCNT);
	    /* final enqueue handled below outside switch */
	    /*
	     * Debugging code for CWU.
	     */
	    if (!addr) {
		errmsg(&msgsym(SWFAILURE, SSE), "IPX write to location 0",
		       spin->EXTRACOUNT);
		sse_soft_error(sse);
		return(OPCODE_ABORT);
	    }
	    break;
	default:
	    break;
	}
	if (spin->EXTRA[i] == SSE_UNMAPPED) 
	    return(OPCODE_ABORT);
	sse_poke_treemem(sse, spin->EXTRA[i++], &sw);
	break;
	
    case SPIN_COMPARE:
    case SPIN_RANDOM:
	if (!sse_gen_compare(sse, spin))
	    return(OPCODE_ABORT);
	break;
    default:
	break;
    }
    return(OPCODE_WALK);
}

/*
 * sse_tree_depth
 * Calculate the depth of the tree.
 */

int sse_tree_depth (ssetype *sse, spintype *spin, ssestattype *stat, 
		    queuetype *queue)
{
    switch (spin->type) {
    case SPIN_MATCH:
	spin->depth = 0;		/* no accounting for output regs */
	break;
    case SPIN_COMPARE:
    case SPIN_RANDOM:
	spin->depth = 1;
	if (spin->LTADDR)
	    spin->depth = max(spin->depth, spin->LTADDR->depth + 1);
	if (spin->EQADDR)
	    spin->depth = max(spin->depth, spin->EQADDR->depth + 1);
	if (spin->GTADDR)
	    spin->depth = max(spin->depth, spin->GTADDR->depth + 1);
	break;
    default:
	break;
    }
    if (spin->walk)
	spin->walk->time--;
    return(OPCODE_WALK);
}

#ifdef NOTDEF
/*
 * sse_tree_count
 * Count the number of nodes in the tree.
 */

static int sse_tree_count (ssetype *sse, spintype *spin, ssestattype *stat, 
		    queuetype *queue)
{
    sse_tree_nodecount++;
    return(OPCODE_WALK);
}
#endif

/*
 * sse_sanity_walker
 * Check to see if a node is sane.
 */

static int sse_sanity_walker (ssetype *sse, spintype *spin, ssestattype *stat, 
			      queuetype *queue) 
{
    treeinsttype inst;

    switch(spin->type) {
    case SPIN_COMPARE:
	if (spin->EQADDR) {
	    if ((spin->EQADDR->type == SPIN_COMPARE)) {
		if ((spin->protocol == SSE_PROTOCOL_IP) ||
		    (spin->protocol == SSE_PROTOCOL_CLNS))
		    inst = SSE_INST_OK;
		else
		    inst = SSE_INST_RD_BYTE;
		if (spin->nodeinfo.compare.eq.inst != inst) {
		    errmsg(&msgsym(BOGUSEQ, SSE), 
			   spin, spin->EQADDR, spin->EQADDR->type,
			   spin->nodeinfo.compare.eq.inst); 
		    sse_soft_error(sse);
		    return(OPCODE_ABORT);
		}
	    }
	}
    case SPIN_MATCH:
    case SPIN_RANDOM:
	return(OPCODE_WALK);
    default:
	errmsg(&msgsym(SWFAILURE, SSE), "sse_sanity_walker bogus node",
	       spin);	
	sse_soft_error(sse);
	return(OPCODE_ABORT);
    }
}

/*
 * sse_sanity_check
 * Sanity check a tree.
 */

static boolean sse_sanity_check (ssetype *sse, spintype *spin)
{
    int retval;

    if (!sse_sanity_checks)
	return(TRUE);

    retval = sse_tree_walk(sse, spin, sse_sanity_walker, NULL, NULL, NULL);
    if (retval == OPCODE_ABORT)
	return(FALSE);
    else
	return(TRUE);
}



/*
 *		     Main routines for tree generation
 */

#if 0
/*
 * sse_find_multipath_duplicate
 * XXX What does this do?
 */

static int sse_find_multipath_duplicate (ssetype *sse, spintype *spin,
					 ssestattype *stat, queuetype *queue)
{
    if (spin->OUTRW == sse_multipath_search_target) {
	sse_multipath_duplicate_found = TRUE;
	return(OPCODE_ABORT);
    } else
	return(OPCODE_WALK);
}


/*
 * sse_alternate_match
 * In the case of protocols which do load balancing, add nodes to create a
 * subtree that branches based on a random number.  Return TRUE on success,
 * FALSE on failure.
 */
static boolean sse_alternate_match (ssetype *sse, int protocol, spintype
				    **pred, spintype *spin, idbtype *idb,
				    void *encaps, uint encapslen, uchar
				    cookie, boolean cookieflag)
{
    rwdbtype* rwdb;
    spintype* randomspin = NULL;
    spintype* nopspin = NULL;
    spintype* match = NULL;
    spintype* nopspin2 = NULL;
    ssestattype *stat;
    spintype *temp;

    /*
     * First locate the rewrite entry in the table.
     */
    rwdb = sse_find_rwdb(sse, protocol, encaps, encapslen);
    if (!rwdb)
	return(FALSE);
    if (sse_debug)
	buginf("\nSSE: sse_alternate_match rwdb %#x", rwdb); 
    stat = &sse->stats[protocol];
    if (spin->type == SPIN_MATCH) {
	
	/*
	 * This is the first time we've encountered a branch here.
	 * Splice in a random node and then branch on it.
	 */
	randomspin = sse_malloc_spin(protocol, stat);
	if (!randomspin) 
	    goto memfail;
	sse_init_spin(randomspin, 0x80, SSE_INST_RD_RAND, SSE_INST_RD_RAND,
		      SSE_INST_RD_RAND); 
	randomspin->type = SPIN_RANDOM;
	
	/*
	 * Splice in a NOP
	 */
	nopspin = sse_malloc_spin(protocol, stat);
	if (!nopspin) 
	    goto memfail;
	sse_init_spin(nopspin, 0x80, SSE_INST_NOP, SSE_INST_NOP, SSE_INST_NOP);
	nopspin->type = SPIN_COMPARE;
	sse_init_spin_links(randomspin, nopspin);
	/*
	 * Set up counter for number of match nodes
	 */ 
	randomspin->value = 0;
	
	nopspin2 = sse_malloc_spin(protocol, stat);
	if (!nopspin2)
	    goto memfail;
	/*
	 * XXX Where did 0x63 come from?
	 */
	sse_init_spin(nopspin2, 0x63, SSE_INST_NOP, SSE_INST_NOP,
		      SSE_INST_NOP); /* XXX */
	nopspin2->type = SPIN_COMPARE;
	sse_init_spin_links(nopspin, nopspin2);

	/* XXX this is a bug!  we clobber the downlinks if we do this! */
	/*	randomspin->nodeinfo.match.usage =
		spin->nodeinfo.match.usage;*/
	/* if we ever do balancing */ 
    } else {
	
	/*
	 * This is a SPIN_RANDOM node.
	 * Walk subtree searching for duplicate
	 * XXX I'm not convinced that this only walks the subtree.  I think
	 * it walks the WHOLE tree.
	 */
	sse_multipath_duplicate_found = FALSE; /* XXX look at opcodes
						  instead */
	sse_multipath_search_target = rwdb; /* XXX fudge this into the
					       queue ptr? */ 
	temp = spin->walk;
	spin->walk = NULL;
	sse_tree_walk(sse, spin, sse_find_multipath_duplicate, NULL,
		      stat, NULL);
	spin->walk = temp;
	if (sse_multipath_duplicate_found) {
	    if (sse_debug)
		buginf("\nSSE: sse_alternate_match ignoring duplicate");
	    free(rwdb);		/* free the encaps info */
	    return(TRUE);
	}
	randomspin = spin;
    }
    
    /*
     * At this point, we know we need to add a match node and fiddle the
     * comparison ranges
     */
    randomspin->value++;	/* another path is available */
    nopspin = randomspin->EQADDR;
    nopspin = nopspin->EQADDR;
    
    /*
     * nopspin is the second compare node in the link.
     */

    /*
     * We do the comparison on this instruction, and branch below it
     */
    sse_spinref_plus1(spin);	/* XXX is this double-counting? */
    nopspin->LTADDR = spin;
    
    /*
     * Add a match node
     */
    match = sse_malloc_spin(protocol, stat);
    if (!match)
	goto memfail;
    match->type = SPIN_MATCH;
    match->OUTRW = rwdb;
    match->nodeinfo.match.usage = 1;
    if (sse->real)
	match->value = (idb->hwptr->cbus_icb_address >> 1) && 0xff;
    else 
	match->value = 0x45;
    
    /*
     * Splice into tree, possibly creating another compare node
     */
    nopspin->EQADDR = nopspin->GTADDR = match;
    sse_spinref_plus1(match);
    *pred = randomspin;
    
    return(TRUE);
 memfail:
    if (randomspin)
	free(randomspin);
    if (rwdb)
	free(rwdb);
    if (nopspin)
	free(nopspin);
    if (nopspin2)
	free(nopspin2);
    if (match)
	free(match);
    return(FALSE);
}

#endif

/*
 * sse_init_tree
 * Initialize the intermediate tree for the protocol.
 */

void sse_init_tree (ssetype *sse, int protocol)
{
    if (protocol > MAX_SSE_PROTOCOL) {
	errmsg(&msgsym(SWFAILURE, SSE), "sse_init_tree requested protocol",
	       protocol); 
	return;
    }
    sse_compiletree = NULL;
    sse->rebuilds[protocol]++;
#ifdef NOTDEF
    if (sse_debug) {
	sse_tree_nodecount = 0;
	if (sse->protocol_tree[protocol])
	    (void) sse_tree_walk(sse, sse->protocol_tree[protocol],
				 sse_tree_count, NULL, NULL, NULL);
	buginf("\nSSE: At init, old start %#x, count %d",
	       sse->protocol_tree[protocol], sse_tree_nodecount);
    }
#endif
}

/*
 * sse_incremental_compile
 * 
 * Perform an incremental compilation step.  Return TRUE on success, FALSE
 * on errors. 
 */

static inline boolean
sse_incremental_compile (ssetype *sse, int protocol, spintype *tree,
			 spintype **pred, spintype *parent)
{
    ssestattype *stat;
    int retval;

    if (!sse_sanity_check(sse, sse->protocol_tree[protocol])) {
	buginf("\nSSE: sanity failure at start of sse_incremental_compile");
	return(FALSE);
    }

    /*
     * First, temporarily disconnect the nodes we just added.  Note
     * that parent, pred and tree are sufficient to
     * reconstruct everything.  Much of the rest of this is taken from
     * sse_compile(). 
     */
    if (pred)
	*pred = NULL;
    tree->walk = NULL;
    
    /*
     * Make sure that the rewrite entry is in place.
     */
    if (!sse_map_rwdb(sse, protocol)) {
	if (pred)
	    *pred = tree;
	tree->walk = parent;
	errmsg(&msgsym(COMPERR, SSE), "mapping rewrites");
	return(FALSE);
    }
	
    /*
     * Give addresses to everything.
     */
    stat = &sse->stats[protocol];
    retval = sse_tree_walk(sse, tree, sse_address_node, NULL, stat,
			   NULL);
    if (retval == OPCODE_ABORT) {
	if (pred)
	    *pred = tree;
	tree->walk = parent;
	errmsg(&msgsym(COMPERR, SSE), "addressing error");
	return(FALSE);
    }
    
    /*
     * Now generate the code.
     */
    retval = sse_tree_walk(sse, tree, sse_code_gen, NULL, stat, NULL); 
    if (retval == OPCODE_ABORT) {
	if (pred)
	    *pred = tree;
	tree->walk = parent;
	errmsg(&msgsym(COMPERR, SSE), "code generation");
	return(FALSE);
    }

    /*
     * Reconnect the node and patch it in.
     */
    if (pred)
	*pred = tree;
    tree->walk = parent;
    if (parent) {
	if (!sse_gen_compare(sse, parent)) {
	    if (sse_debug)
		buginf("\nsse_incremental_compile tried to "
		       "patch unmapped node %#x %#x", tree,
		       tree->physaddr);
	    errmsg(&msgsym(COMPERR, SSE), "backpatching");
	    return(FALSE);
	}
    } else {
	sse_backpatch(sse, protocol);
    }
    if (!sse_sanity_check(sse, sse->protocol_tree[protocol])) {
	buginf("\nSSE: sanity failure at end of sse_incremental_compile");
	return(FALSE);
    }

    return(TRUE);
}

/*
 * sse_spin_balance
 *
 * Passed one pointer to a pointer to a spin node (yes, that's right),
 * rotate the subtree to balance it.  This isn't _quite_ an AVL tree, but
 * it steals the basic ideas.  The primary difference here is that we have
 * annoying down links which can't balance out, but which ARE important
 * performance wise, and that when we add to the tree, we don't add just
 * one single node, so the tree may not really be only slightly out of
 * balance.  Return TRUE if we balanced, FALSE if we needed no rotation.
 */

static inline boolean
sse_spin_balance (ssetype *sse, spintype **spin, boolean codegen,
		  spintype *parent, int protocol)
{
    int ldepth, rdepth, edepth;
    spintype *curr, *child, *grandchild;
    treeaddrtype newcurr, newchild, newgrandchild;

    if (!spin || !*spin)
	return(FALSE);
    curr = *spin;
    if (curr->type != SPIN_COMPARE)
	return(FALSE);
    grandchild = NULL;
    newcurr = SSE_UNMAPPED;		/* stupid compiler */
    newchild = SSE_UNMAPPED;
    newgrandchild = SSE_UNMAPPED;

    /*
     * Fancy balancing work goes here.  Using -1 for empty 
     */
    ldepth = -1;
    rdepth = -1;
    edepth = 0;
    if (curr->GTADDR)
	rdepth = curr->GTADDR->depth;
    if (curr->LTADDR)
	ldepth = curr->LTADDR->depth;
    if (curr->EQADDR) {
	edepth = curr->EQADDR->depth;
	/*
	 * Rotation can only affect the depth by one.  If the equal depth
	 * is dominant, don't bother.
	 */
	if (edepth >= max(rdepth, ldepth))
	    return(FALSE);
    }
    rdepth -= ldepth;
    if (rdepth > 1) {
	/*
	 * Shift nodes to the left.  Note that this is mirrored below.
	 */
	child = curr->GTADDR;		/* save right node */
	if (!child || (child->type != SPIN_COMPARE))
	    return(FALSE);
	ldepth = 0;
	rdepth = 0;
	if (child->GTADDR)
	    rdepth = child->GTADDR->depth;
	grandchild = child->LTADDR;
	if (grandchild) {
	    if (grandchild->type == SPIN_COMPARE)
		ldepth = grandchild->depth;
	    else
		grandchild = NULL;
	}
	rdepth -= ldepth;
	if ((rdepth >= 0) || !grandchild) {
	    /*
	     * Single rotation right.  Yes, the direction of the above
	     * compare is correct.  Even in the mirrored version.
	     */

	    if (codegen) {
		/*
		 * Need two nodes to generate code into.  Get both or bail.
		 */
		newcurr = sse_malloc_treeaddr(sse, NULL);
		newchild = sse_malloc_treeaddr(sse, NULL);
		if (!newcurr || !newchild) {
		    if (newcurr)
			sse_free_treeaddr(sse, NULL, newcurr);
		    else if (newchild)
			sse_free_treeaddr(sse, NULL, newchild);
		    return(FALSE);
		}
	    }

	    *spin = child;		/* right is now root */
	    curr->GTADDR = child->LTADDR;
	    child->LTADDR = curr;
	    child->walk = curr->walk;	/* inherit backpointers */
	    curr->walk = child;
	    if (curr->GTADDR)
		curr->GTADDR->walk = curr;
	    
	    if (codegen) {
		/*
		 * Free the old words, fix up the physaddr pointers, and
		 * then fix the parent.
		 */
		if (curr->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, curr->physaddr);
		curr->physaddr = newcurr;
		if (child->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, child->physaddr);
		child->physaddr = newchild;

		/*
		 * Swapping curr and child while running is hazardous.
		 * Replicate them and then swing the parent over.  Do curr
		 * first. 
		 */
		sse_gen_compare(sse, curr);
		
		/*
		 * Now do child.  The only fix is to point child->LTADDR to
		 * curr, which is known not to be null.  And use the new
		 * address, not the old one.
		 */
		sse_gen_compare(sse, child);

		if (parent) {
		    sse_gen_compare(sse, parent);
		} else {
		    if (sse->protocol_tree[protocol] != child) {
			if (sse_debug) 
			    buginf("\nSSE: balance rotate right one error");
		    }
		    sse_backpatch(sse, protocol);
		}
	    }
	} else {
	    /*
	     * Double rotation on the right.  Grandchild becomes root.
	     */

	    if (codegen) {
		/*
		 * Need three nodes to generate code into.  Get all or bail.
		 */
		newcurr = sse_malloc_treeaddr(sse, NULL);
		newchild = sse_malloc_treeaddr(sse, NULL);
		newgrandchild = sse_malloc_treeaddr(sse, NULL);
		if (!newcurr || !newchild || !newgrandchild) {
		    if (newcurr)
			sse_free_treeaddr(sse, NULL, newcurr);
		    if (newchild)
			sse_free_treeaddr(sse, NULL, newchild);
		    if (newgrandchild)
			sse_free_treeaddr(sse, NULL, newgrandchild);
		    return(FALSE);
		}
	    }

	    *spin = grandchild;		
	    child->LTADDR = grandchild->GTADDR; 
	    curr->GTADDR = grandchild->LTADDR;
	    grandchild->GTADDR = child;
	    grandchild->LTADDR = curr;
	    grandchild->walk = curr->walk;
	    curr->walk = grandchild;
	    child->walk = grandchild;
	    if (child->LTADDR)
		child->LTADDR->walk = child;
	    if (curr->GTADDR)
		curr->GTADDR->walk = curr;

	    if (codegen) {
		/*
		 * Free the old words, fix up the physaddr pointers.
		 */
		if (curr->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, curr->physaddr);
		curr->physaddr = newcurr;
		if (child->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, child->physaddr);
		child->physaddr = newchild;
		if (grandchild->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, grandchild->physaddr);
		grandchild->physaddr = newgrandchild;

		/*
		 * Swapping nodes while running is hazardous.
		 * Replicate them and then swing the parent over.  Do curr
		 * first. 
		 */
		sse_gen_compare(sse, curr);
		sse_gen_compare(sse, child);
		sse_gen_compare(sse, grandchild);

		if (parent) {
		    sse_gen_compare(sse, parent);
		} else {
		    if (sse->protocol_tree[protocol] != grandchild) {
			if (sse_debug) 
			    buginf("\nSSE: balance rotate right two error");
		    }
		    sse_backpatch(sse, protocol);
		}
	    }
	}
    } else if (rdepth < -1) {
	/*
	 * Shift nodes to the right.  Note that this is mirrored above.
	 */
	child = curr->LTADDR;		/* save left node */
	if (!child || (child->type != SPIN_COMPARE))
	    return(FALSE);
	ldepth = 0;
	rdepth = 0;
	if (child->LTADDR)
	    rdepth = child->LTADDR->depth;
	grandchild = child->GTADDR;
	if (grandchild) {
	    if (grandchild->type == SPIN_COMPARE)
		ldepth = grandchild->depth;
	    else
		grandchild = NULL;
	}
	rdepth -= ldepth;
	if ((rdepth >= 0) || !grandchild) {
	    /*
	     * Single rotation left.  Yes, the direction of the above
	     * compare is correct.  Even in the mirrored version.
	     */

	    if (codegen) {
		/*
		 * Need two nodes to generate code into.  Get both or bail.
		 */
		newcurr = sse_malloc_treeaddr(sse, NULL);
		newchild = sse_malloc_treeaddr(sse, NULL);
		if (!newcurr || !newchild) {
		    if (newcurr)
			sse_free_treeaddr(sse, NULL, newcurr);
		    else if (newchild)
			sse_free_treeaddr(sse, NULL, newchild);
		    return(FALSE);
		}
	    }

	    *spin = child;		/* left is now root */
	    curr->LTADDR = child->GTADDR;
	    child->GTADDR = curr;
	    child->walk = curr->walk;	/* inherit backpointers */
	    curr->walk = child;
	    if (curr->LTADDR)
		curr->LTADDR->walk = curr;
	    
	    if (codegen) {
		/*
		 * Free the old words, fix up the physaddr pointers, and
		 * then fix the parent.
		 */
		if (curr->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, curr->physaddr);
		curr->physaddr = newcurr;
		if (child->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, child->physaddr);
		child->physaddr = newchild;

		/*
		 * Swapping curr and child while running is hazardous.
		 * Replicate them and then swing the parent over.  Do curr
		 * first. 
		 */
		sse_gen_compare(sse, curr);
		
		/*
		 * Now do child.  The only fix is to point child->LTADDR to
		 * curr, which is known not to be null.  And use the new
		 * address, not the old one.
		 */
		sse_gen_compare(sse, child);

		if (parent) {
		    sse_gen_compare(sse, parent);
		} else {
		    if (sse->protocol_tree[protocol] != child) {
			if (sse_debug) 
			    buginf("\nSSE: balance rotate left one error");
		    }
		    sse_backpatch(sse, protocol);
		}
	    }
	} else {
	    /*
	     * Double rotation on the left.  Grandchild becomes root.
	     */

	    if (codegen) {
		/*
		 * Need three nodes to generate code into.  Get all or bail.
		 */
		newcurr = sse_malloc_treeaddr(sse, NULL);
		newchild = sse_malloc_treeaddr(sse, NULL);
		newgrandchild = sse_malloc_treeaddr(sse, NULL);
		if (!newcurr || !newchild || !newgrandchild) {
		    if (newcurr)
			sse_free_treeaddr(sse, NULL, newcurr);
		    if (newchild)
			sse_free_treeaddr(sse, NULL, newchild);
		    if (newgrandchild)
			sse_free_treeaddr(sse, NULL, newgrandchild);
		    return(FALSE);
		}
	    }

	    *spin = grandchild;
	    child->GTADDR = grandchild->LTADDR;
	    curr->LTADDR = grandchild->GTADDR;
	    grandchild->LTADDR = child;
	    grandchild->GTADDR = curr;
	    grandchild->walk = curr->walk;
	    curr->walk = grandchild;
	    child->walk = grandchild;
	    if (child->GTADDR)
		child->GTADDR->walk = child;
	    if (curr->LTADDR)
		curr->LTADDR->walk = curr;

	    if (codegen) {
		/*
		 * Free the old words, fix up the physaddr pointers.
		 */
		if (curr->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, curr->physaddr);
		curr->physaddr = newcurr;
		if (child->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, child->physaddr);
		child->physaddr = newchild;
		if (grandchild->physaddr != SSE_UNMAPPED)
		    sse_free_treeaddr(sse, NULL, grandchild->physaddr);
		grandchild->physaddr = newgrandchild;

		/*
		 * Swapping nodes while running is hazardous.
		 * Replicate them and then swing the parent over.  Do curr
		 * first. 
		 */
		sse_gen_compare(sse, curr);
		sse_gen_compare(sse, child);
		sse_gen_compare(sse, grandchild);

		if (parent) {
		    sse_gen_compare(sse, parent);
		} else {
		    if (sse->protocol_tree[protocol] != grandchild) {
			if (sse_debug) 
			    buginf("\nSSE: balance rotate left two error");
		    }
		    sse_backpatch(sse, protocol);
		}
	    }
	}
    } else
	return(FALSE);

    /*
     * Rotations complete, now fix up depths.  Note that depths hopefully
     * decrease. 
     */
    sse_tree_depth(sse, curr, NULL, NULL);
    sse_tree_depth(sse, child, NULL, NULL);
    if (grandchild) {
	/*
	 * If we did a double rotation, fix some more...
	 */
	sse_tree_depth(sse, grandchild, NULL, NULL);
    }
    return(TRUE);
}

/*
 * sse_branch_balance
 * Given a node, balance along the branch leading down to this node.
 */

static void
sse_branch_balance (ssetype *sse, spintype *spin, int protocol,
		    boolean codegen)
{
    spintype *parent, **pred;

    pred = NULL;			/* stupid compiler */
    while (spin) {
	parent = spin->walk;
	if (!parent)			/* point at root */
	    pred = &sse->protocol_tree[protocol];
	else if (parent->GTADDR == spin)
		pred = &parent->GTADDR;
	else if (parent->EQADDR == spin)
	    pred = &parent->EQADDR;
	else if (parent->LTADDR == spin)
	    pred = &parent->LTADDR;
	else if (sse_debug) {
	    errmsg(&msgsym(SWFAILURE, SSE),
		   "Structure failure during balancing", -1);
	    break;
	}
	sse_spin_balance(sse, pred, codegen, parent, protocol); 
	if (parent)
	    sse_tree_depth(sse, parent, NULL, NULL);
	else
	    sse->depth[protocol] = spin->depth;
	spin = parent;
    }
}

/*
 * sse_add 
 * Add an address to the current protocol tree that is in progress.
 *
 * Add an entry to the routing tree under construction.  The entry is added
 * to the tree for the protocol.  Start points to a protocol address of
 * bytecount bytes which indicates the start of the address range. End is
 * the value of the last byte in the address at the end of the range.  For 
 * example, the range 131.108.6.1 - 131.108.6.10 would be passed with start
 * as a pointer to 131.108.6.1, bytecount 4, and end 10.  Idb is the
 * outbound interface on a match, encaps and encapslen describe the MAC
 * rewrite string to use.  This function returns TRUE if the add is
 * successful (or if it duplicates an existing entry).  It returns FALSE if
 * there is a problem (and generates a debug message).  Usage is the usage
 * count to add or seed the entry with.  Cookie is a value to be loaded into
 * the pipeline register on a successful match, if the flag SSE_ADD_COOKIE
 * is passed.
 */

boolean sse_add (ssetype *sse, int protocol, uint bytecount,
		 void *start, uchar high, idbtype *idb, 
		 void *encaps, uint encapslen, uint usage,
		 int enctype, uchar cookie, int flags)
{
    spintype *spin, **pred, *lowspin, *highspin, *startrif, *parent;
    spintype *firstparent, **firstpred, *firstnew; 
    spintype matchproto;
    uchar *byte, low;
    int bytesmatched, bytesread;
    int depth, olddepth;
    ssestattype *stat;
    treeinsttype inst;
    boolean balance;

    if (protocol > MAX_SSE_PROTOCOL) {
	errmsg(&msgsym(SWFAILURE, SSE), "sse_add requested protocol",
	       protocol); 
	return(FALSE);
    }
    if (bytecount == 0) {
	errmsg(&msgsym(SWFAILURE, SSE), "sse_add supplied bytecount",
	       0); 
	return(FALSE);
    }
#ifdef NOTDEF
    if (sse_debug) {
	buginf("\nSSE: proto %d byte %d addr %i high %d", protocol, bytecount,
	       *((ipaddrtype *) start), high);
	buginf("\nSSE: %s %x %d %d", idb->namestring, *(uint *)encaps,
	       encapslen, usage);
    }
#endif
    
    /*
     * Check here to make sure that the outbound interface is on the SSE.
     * If not, just bail.  If there are multiple SP's in the box, then
     * only accept interfaces on the current SP.  
     */
    if (sse->real) {
	if (!(idb->hwptr->status & IDB_CBUS))
	    return(FALSE);
	if (idb->hwptr->devctl != sse->cbus)
	    return(FALSE);
    }

    /*
     * Incremental changes while we're not ready are nasty.
     */
    if (!(sse->state & SSE_STATE_INITIALIZED) || sse_manager_blocked)
	return(FALSE);

    /*
     * Walk the specified tree.  Compare each byte of the address as we go.
     * If there is already a leaf which matches this address, add to the
     * usage. 
     */
    stat = &sse->stats[protocol];
    if (flags & SSE_ADD_INCREMENTAL)
	pred = &sse->protocol_tree[protocol];
    else
	pred = &sse_compiletree;
    spin = *pred;
    startrif = NULL;
    byte = start;
    bytesmatched = 0;
    /*
     * Already have read first byte of destination address into pipeline
     * register.
     */
    bytesread = 1;
    parent = NULL;
    highspin = NULL;
    firstnew = NULL;

    while (spin) {
	switch (spin->type) {
	case SPIN_COMPARE:
	    if (spin->value == *byte) {

		/*
		 * For SRB, track the spin node that matches the second
		 * byte in the address.  This is the first node in the RIF.
		 */
		if ((SSE_PROTOCOL_SRB == protocol) &&
		    (0 == bytesmatched))
		    startrif = spin;
		bytesmatched++;
		bytesread++;
		byte++;
		pred = &spin->EQADDR;
	    } else if (*byte > spin->value) 
		pred = &spin->GTADDR;
	    else if (*byte < spin->value) 
		pred = &spin->LTADDR;
	    parent = spin;
	    spin = *pred;
	    break;
	case SPIN_MATCH:
	    if (bytesmatched == bytecount) {
		spin->nodeinfo.match.usage += usage;
#if 0
		if (protocol == SSE_PROTOCOL_IPX) 
		    return(sse_alternate_match(sse, protocol, pred, spin, idb,
					       encaps, encapslen, cookie,
					       flags & SSE_ADD_COOKIE)); 
#endif
		return(TRUE);
	    }

	    /*
	     * We've gotten two addresses, a more specific and a less
	     * specific.  For now, we don't understand how to handle this.
	     * Someday...  
	     */
	    errmsg(&msgsym(NESTADDR, SSE), protocol,
		   *(ipaddrtype *)start); 
	    sse_soft_error(sse);
	    return(FALSE);
	case SPIN_RANDOM:
	    if (bytesmatched != bytecount) {
		if (sse_debug)
		    buginf("\nSSE: nested addresses when load balancing");
		return(FALSE);
	    }
	    /* NO! Do NOT do this! You'll clobber the links! */
	    /*	    spin->nodeinfo.match.usage += usage;*/
#if 0
	    return(sse_alternate_match(sse, protocol, pred, spin, idb, encaps,
				       encapslen, cookie,
				       flags & SSE_ADD_COOKIE));
#endif
	default:
	    if (sse_debug)
		buginf("\nSSE: unknown spin type %d, protocol %d",
		       spin->type, protocol);
	    return(FALSE);
	}
    }
    
    /*
     * Record where we're adding in new stuff.  We'll have to come back
     * here.  Fix up the depth here if necessary.
     */
    firstparent = NULL;
    balance = FALSE;
    if (parent) {
	depth = bytecount - bytesmatched;
	olddepth = parent->depth;
	parent->depth = max(olddepth, depth + 1);
	firstparent = parent;
	if (olddepth != parent->depth)
	    balance = TRUE;		/* remember to come back */

	/*
	 * Fix up the instruction in the parent.
	 */
	if (pred == &parent->EQADDR) {
	    if (depth == 0)
		parent->nodeinfo.compare.eq.inst = SSE_INST_NOP;
	    else {
		switch (parent->protocol) {
		case SSE_PROTOCOL_IP:
		case SSE_PROTOCOL_CLNS:
		    parent->nodeinfo.compare.eq.inst = SSE_INST_OK;
		    break;
		default:
		    parent->nodeinfo.compare.eq.inst = SSE_INST_RD_BYTE;
		    break;
		}
	    }
	}
    }
    firstpred = NULL;

    /*
     * If we got here, the address is not in the tree.  Add any remaining
     * bytes to the tree and then add a match node.
     */
    while (bytesmatched < (bytecount - 1)) {
	spin = sse_malloc_spin(protocol, stat);
	if (!spin) 
	    return(FALSE);
	if (protocol == SSE_PROTOCOL_IP) {
	    /*
	     * Save away next byte of dst IP address for possible access
	     * list check.
	     */
	    inst = (SSE_INST_OK|SSE_INST_LD_IPDST) + bytesread++;
	} else if (protocol == SSE_PROTOCOL_CLNS) {
	    inst = SSE_INST_OK;
	} else {
	    inst = SSE_INST_RD_BYTE;
	}

	sse_init_spin(spin, *byte, SSE_INST_NOP, inst, SSE_INST_NOP);
	*pred = spin;

	if (!firstnew) {
	    firstnew = spin;
	    firstpred = pred;
	}

	/*
	 * For SRB, track the spin node that matches the second
	 * byte in the address.  This is the first node in the RIF.
	 */
	if (SSE_PROTOCOL_SRB == protocol) {
	    if (0 == bytesmatched)
		startrif = spin;
	    spin->protoinfo = bytesmatched;
	}
	bytesmatched++;
	spin->depth = bytecount - bytesmatched;
	spin->walk = parent;
	byte++;
	pred = &spin->EQADDR;
	parent = spin;
    }
    
    lowspin = parent;
    if (bytesmatched < bytecount) {
	/*
	 * The last byte in the address is somewhat special.  It can indicate a
	 * range.
	 */
	low = *byte;
	if (low > high) {
	    if (sse_debug)
		buginf("\nSSE: backwards range [%d, %d], protocol %d", *byte,
		       high, protocol);
	    low = high;
	    high = *byte;
	}
	
	/*
	 * Allocate a node to match the low byte of the range.
	 */
	lowspin = sse_malloc_spin(protocol, stat);
	if (!lowspin) 
	    return(FALSE);
	if (!firstnew) {
	    firstnew = lowspin;
	    firstpred = pred;
	}
	inst = SSE_INST_NOP;
	lowspin->depth = 1;
	lowspin->walk = parent;
	if (protocol == SSE_PROTOCOL_IP) {
	    /*
	     * Save away dst IP address for possible access list check.
	     */
	    if (bytesread < sizeof(ipaddrtype))
		inst = (SSE_INST_OK|SSE_INST_LD_IPDST) + bytesread++;
	}
	sse_init_spin(lowspin, low, SSE_INST_NOP, inst, SSE_INST_NOP);
	*pred = lowspin;
	if (SSE_PROTOCOL_SRB == protocol)
	    lowspin->protoinfo = bytesmatched;
	
	/*
	 * If there is a range, allocate a node for the high byte of the range.
	 */
	if (low != high) {
	    highspin = sse_malloc_spin(protocol, stat);
	    if (!highspin) 
		return(FALSE);
	    highspin->depth = 1;
	    highspin->walk = lowspin;
	    highspin->protoinfo = bytesmatched;
	    sse_init_spin(highspin, high, SSE_INST_NOP, SSE_INST_NOP,
			  SSE_INST_NOP); 
	    lowspin->GTADDR = highspin;
	    lowspin->depth = 2;		/* it's above highspin */
	    if (SSE_PROTOCOL_SRB == protocol)
		highspin->protoinfo = bytesmatched;
	}
	bytesmatched++;
    } else {
	if (!lowspin) {
	    buginf("\nSSE: sse_add overlap failure");
	    return(FALSE);
	}
    }

    /*
     * Find a match node.
     */
    memset(&matchproto, 0, sizeof(spintype));
    matchproto.type = SPIN_MATCH;

    /*
     * Add code here to save the outbound idb and mac info.  Generate the
     * result instructions for pointers to rewrite memory, output
     * accounting registers.
     */
    if (encaps)
	matchproto.OUTRW = sse_find_rwdb(sse, protocol, encaps, encapslen);
    else
	matchproto.OUTRW = NULL;
    if (sse->real) 
	matchproto.value = (idb->hwptr->cbus_icb_address >> 1) & 0xff;
    else
	matchproto.value = SSE_SIMULATOR_ICB; /* fake out the simulator */

    /*
     * All sorts of protocol specific stuff can go here.
     */
    switch (protocol) {
    case SSE_PROTOCOL_IP:
	/*
	 * Make room for extra instructions to save the remainder of the
	 * destination address when we generate the code for the match
	 * node. 
	 */
	matchproto.REMAINDER = sizeof(ipaddrtype) - bytesread;
	matchproto.EXTRACOUNT = matchproto.REMAINDER + SSE_IP_EXTRA_INST; 
	matchproto.protoinfo = SSE_GLOBAL_RETURN;
	if (idb->ip_output_accesslist) {
	    treeaddrtype patchpoint;
	    
	    patchpoint = (treeaddrtype)
		reg_invoke_sse_access_get_patchpoint(sse, idb->ip_output_accesslist);
	    if (patchpoint)
		matchproto.protoinfo = patchpoint;
	    else 
		matchproto.protoinfo = SSE_GLOBAL_DENY;
	    if (sse_debug)
		buginf("\nSSE: patching to use %#x",
		       matchproto.protoinfo); 
	}
	break;
	
    case SSE_PROTOCOL_IPX:
	/* XXX Handle output of IPX TC/Hopcount crap */
	/*
	 * If we are not routing novell with IGRP,
	 * we always increment a 4 bit hopcount. (op = 0)
	 * If we *are* routing novell with IGRP, then
	 * if this is an igrp routed output interface, 
	 *   and the current hopcount is less than 15 
	 *     then do 4->8 transformation and increment
	 *       op 2 if current hopcount == 0
	 *       op 3 otherwise
	 * if this is not an igrp routed output interface
	 *   and the current hopcount is less than 15
	 *    then increment 4 bit hopcount (op = 0)
	 *   else
	 *    do 8->4 compression (op = 1)
	 * What an ugly hack!
	 */
	matchproto.nodeinfo.match.protoinfo2 = cookie;
	if (sse_debug)
	    buginf("\nSSE: IPX %s hop count handling", cookie ? "EIGRP" :
		   "normal");
	
	/*
	 * Handle output of IPX length field on ether interfaces using
	 * Novell-ether encapsulation.  Argument is #bytes back from
	 * the line to insert the 802.3 length, or 0 for no operation.
	 */
	if ((idb->hwptr->status & IDB_ETHER) || 
	    (is_atm_lane(idb))) {
	    if (enctype == ET_NOVELL_ETHER)
		matchproto.protoinfo = 2;
	    else if (enctype == ET_SAP)
		matchproto.protoinfo = 5;
	    else if (enctype == ET_SNAP)
		matchproto.protoinfo = 10;
	    else		/* ARPA */
		matchproto.protoinfo = 0;
	} else
	    matchproto.protoinfo = 0;

	matchproto.EXTRACOUNT = SSE_IPX_EXTRA_INST;
	break;
	
    case SSE_PROTOCOL_SRB:
	/*
	 * SRB needs three extra nodes per address.  Since RIF's must be read
	 * in byte pairs, after failing on the first byte, we must skip the
	 * second byte.  The extra node skips and then jumps back to the
	 * start of the RIF.  Save the start of RIF pointer in the
	 * protoinfo field.  We also need two extra nodes to compensate for
	 * a mask operation on the last byte of the RIF match.
	 */
	matchproto.EXTRACOUNT = SSE_SRB_EXTRA_INST;
	matchproto.protoinfo = (int) startrif;
	break;
	
    case SSE_PROTOCOL_CLNS:
	/* 
	 * Handle output of length field on ether interfaces.  Argument is
	 * #words back from line to insert length, or 0 for no operation.
	 */
	if (idb->hwptr->status & IDB_ETHER)
	    matchproto.protoinfo = 4;
	else
	    matchproto.protoinfo = 0;
	matchproto.EXTRACOUNT = SSE_CLNS_EXTRA_INST;
	break;
	
    default:
	break;
    }
    
    /*
     * Look up the match node in the database.  If we get back NULL, then
     * it failed.
     * Create links to it.  If there was a range, create links from the
     * high node.
     */
    matchproto.protocol = protocol;
    spin = sse_find_match(sse, &matchproto);
    if (!spin)
	return(FALSE);
    
    if (!firstnew) {
	firstnew = spin;
	firstpred = pred;
    }
    lowspin->EQADDR = spin;
    if (highspin) {
	highspin->EQADDR = highspin->LTADDR = spin;
	sse_spinref_plus2(spin);
    }

    /*
     * Perform incremental code generation if requested.
     */
    if ((flags & SSE_ADD_INCREMENTAL) && firstnew) {

	/*
	 * Go do an incremental compilation.
	 */
	if (!sse_incremental_compile(sse, protocol, firstnew, firstpred,
				     firstparent)) {
	    if (!sse_manager_blocked) {
		/*
		 * Flush the tree.  If we hit a soft error, we would have
		 * disabled the manager.
		 */
		*firstpred = NULL;
		firstnew->walk = NULL;
		sse_free_tree(sse, firstnew, stat);
		return(FALSE);
	    }
	}

	/*
	 * Fix up the depths in the ancestors.
	 */
	if (!firstparent) {
	    sse->depth[protocol] = firstnew->depth;
	} else {
	    spin = firstnew;
	    while (spin) {
		parent = spin->walk;
		if (parent)
		    sse_tree_depth(sse, parent, NULL, NULL);
		else
		    sse->depth[protocol] = spin->depth;
		spin = parent;
	    }
	}
	balance = TRUE;
    }

    /*
     * Balance the tree if necessary.  Note that adding can only
     * increase the depth of our ancestors.  Further, we can stop walking
     * up the tree if the depth stops changing.  The logic here is somewhat
     * twisted as the root of the tree requires special care.
     */
    if (balance) 
	sse_branch_balance(sse, firstnew, protocol,
			   flags & SSE_ADD_INCREMENTAL);

    return(TRUE);
}

/*
 * sse_delete_dogleg
 *
 * The current spin node has no EQ, and only one the LT or GT branches.
 * Delete the current spin node by promoting its single child.
 */

static spintype *
sse_delete_dogleg (ssetype *sse, int protocol, spintype *spin, spintype
		   **spinptr, ssestattype *stat)
{
    spintype *root, **pred, *child;

    child = *spinptr;
    *spinptr = NULL;

    if (!spin->walk) {
	/*
	 * No parent implies that we're at the root.  Backpatch the root,
	 * tell the child it's now root, free the current node, and return
	 * the child.
	 */
	sse_free_recall(sse, spin, 0, stat);
	sse->protocol_tree[protocol] = child;
	sse_backpatch(sse, protocol);
	child->walk = NULL;
	return(child);
    }

    /*
     * Figure out the pointer to spin.
     */
    pred = NULL;
    root = spin->walk;
    if (root->GTADDR == spin) {
	pred = &root->GTADDR;
    } else if (root->EQADDR == spin) {
	pred = &root->EQADDR;
    } else if (root->LTADDR == spin) {
	pred = &root->LTADDR;
    }

    /*
     * Point the parent at the child, free the node, and regenerate the
     * parent.
     */
    sse_free_recall(sse, spin, 0, stat);
    *pred = child;
    child->walk = root;
    sse_gen_compare(sse, root);
    return(child);
}

/*
 * sse_delete_fork
 *
 * The current spin node has LT, GT but no EQ branch.  We want to delete it
 * from the tree.  Tweak the tree so that we can rotate the node out of the
 * way and delete it.  Several possible strategies here.  Return NULL on
 * errors. 
 */

static spintype *
sse_delete_fork (ssetype *sse, int protocol, spintype *spin, ssestattype
		 *stat)
{
    spintype *right, *left, *parent, *root, **pred, *descendant;
    treeaddrtype newcurr;

    right = spin->GTADDR;
    left = spin->LTADDR;

    root = NULL;
    pred = NULL;
    if (spin->walk) {
	root = spin->walk;
	if (root->GTADDR == spin) {
	    pred = &root->GTADDR;
	} else if (root->EQADDR == spin) {
	    pred = &root->EQADDR;
	} else if (root->LTADDR == spin) {
	    pred = &root->LTADDR;
	}
    }

    if (right->LTADDR == NULL) {
	/*
	 * Paste the left branch onto the left half of the right branch.
	 */
	right->LTADDR = left;
	left->walk = right;
	spin->LTADDR = NULL;

	/*
	 * Delete the dead node.  
	 */
	sse_free_recall(sse, spin, 0, stat);

	/*
	 * Do the code generation. Fix the parent.
	 */
	sse_gen_compare(sse, right);
	if (root) {
	    *pred = right;
	    sse_gen_compare(sse, root);
	    right->walk = root;
	} else {
	    sse->protocol_tree[protocol] = right;
	    sse_backpatch(sse, protocol);
	    right->walk = NULL;
	}

	/*
	 * Fix up depths, and return the right node, which may now need
	 * rotation.
	 */
	sse_tree_depth(sse, right, NULL, NULL);
	return(right);
    } else if (left->GTADDR == NULL) {
	/*
	 * Paste the right branch onto the right half of the left branch.
	 */
	left->GTADDR = right;
	right->walk = left;
	spin->GTADDR = NULL;

	/*
	 * Delete the dead node.
	 */
	sse_free_recall(sse, spin, 0, stat);

	/*
	 * Do the code generation.  Fix the parent.
	 */
	sse_gen_compare(sse, left);
	if (root) {
	    *pred = left;
	    sse_gen_compare(sse, root);
	    left->walk = root;
	} else {
	    sse->protocol_tree[protocol] = left;
	    sse_backpatch(sse, protocol);
	    left->walk = NULL;
	}

	/*
	 * Fix up depths, and return the left node, which may now need
	 * rotation.
	 */
	sse_tree_depth(sse, left, NULL, NULL);
	return(left);
    } 

    /*
     * Now we're in the nasty situation where we have interior
     * grandchildren on both sides.  One one side, we should have a closest
     * grandchild which does not have a link on an interior pointer.  Try
     * the right side first.
     */

    /*
     * Need a node for code generation.
     */
    newcurr = sse_malloc_treeaddr(sse, NULL);
    if (!newcurr) {
	return(NULL);
    }

    /*
     * Walk down the left side of the right branch.
     */
    parent = right;
    descendant = right->LTADDR;
    while ((descendant->type == SPIN_COMPARE) &&
	   (descendant->LTADDR != NULL)) {
	parent = descendant;
	descendant = descendant->LTADDR;
    }

    if (descendant->type == SPIN_COMPARE) {
	/*
	 * Promote the descendent on the right branch to replace spin.
	 */

	descendant->walk = spin->walk;
	parent->LTADDR = NULL;
	sse_gen_compare(sse, parent);
	parent->LTADDR = descendant->GTADDR;
	if (descendant->GTADDR) {
	    descendant->GTADDR->walk = parent;
	}
	descendant->LTADDR = left;
	descendant->GTADDR = right;
	spin->GTADDR = NULL;
	spin->LTADDR = NULL;
	right->walk = descendant;
	left->walk = descendant;
	
	/*
	 * Toss useless node.
	 */
	sse_free_recall(sse, spin, 0, stat);

	/*
	 * Do the code generation.
	 */
	if (descendant->physaddr != SSE_UNMAPPED) {
	    sse_free_treeaddr(sse, NULL, descendant->physaddr);
	}
	descendant->physaddr = newcurr;
	sse_gen_compare(sse, descendant);
	sse_gen_compare(sse, parent);
	if (root) {
	    *pred = descendant;
	    sse_gen_compare(sse, root);
	} else {
	    sse->protocol_tree[protocol] = descendant;
	    sse_backpatch(sse, protocol);
	}
	
	/*
	 * Fix up depths, and return a pointer to the parent, which now can
	 * possibly be further fixed.
	 */
	sse_tree_depth(sse, parent, NULL, NULL);
	sse_tree_depth(sse, descendant, NULL, NULL);
	return(parent);
    }
 
    /*
     * Walk down the right side of the left branch.
     */
    parent = left;
    descendant = left->GTADDR;
    while ((descendant->type == SPIN_COMPARE) &&
	   (descendant->GTADDR != NULL)) {
	parent = descendant;
	descendant = descendant->GTADDR;
    }

    if (descendant->type == SPIN_COMPARE) {
	/*
	 * Promote the descendent on the left branch to replace spin.
	 */
	
	descendant->walk = spin->walk;
	parent->GTADDR = NULL;
	sse_gen_compare(sse, parent);
	parent->GTADDR = descendant->LTADDR;
	if (descendant->LTADDR) {
	    descendant->LTADDR->walk = parent;
	}
	descendant->LTADDR = left;
	descendant->GTADDR = right;
	spin->GTADDR = NULL;
	spin->LTADDR = NULL;
	right->walk = descendant;
	left->walk = descendant;

	/*
	 * Toss useless node.
	 */
	sse_free_recall(sse, spin, 0, stat);

	/*
	 * Do the code generation.
	 */
	if (descendant->physaddr != SSE_UNMAPPED) {
	    sse_free_treeaddr(sse, NULL, descendant->physaddr);
	}
	descendant->physaddr = newcurr;
	sse_gen_compare(sse, descendant);
	sse_gen_compare(sse, parent);
	if (root) {
	    *pred = descendant;
	    sse_gen_compare(sse, root);
	} else {
	    sse->protocol_tree[protocol] = descendant;
	    sse_backpatch(sse, protocol);
	}
	
	/*
	 * Fix up depths, and return a pointer to the parent, which now can
	 * possibly be further fixed.
	 */
	sse_tree_depth(sse, parent, NULL, NULL);
	sse_tree_depth(sse, descendant, NULL, NULL);
	return(parent);
    } 

    /*
     * If we got here, we're truly screwed.
     */
    errmsg(&msgsym(SWFAILURE, SSE), "fork delete failure", 0); 
    sse_soft_error(sse);
    return(NULL);
}

/*
 * sse_delete
 * Delete a prefix from the tree.  Return TRUE if something is messed up.
 * Always called for incremental operations.
 */

enum STATE {STATE_LOW, STATE_HIGH, STATE_DONE};

boolean
sse_delete (ssetype *sse, int protocol, uint bytecount, void *start, uchar
	    high, boolean debug)
{
    ssestattype *stat;
    spintype **pred, *spin, *parent, *tmp;
    uint count, olddepth;
    uchar *byteptr, byte;
    enum STATE state;
    int pass = 0, deletes = 0;
    boolean progress = FALSE;

    state = STATE_LOW;			/* Do the low byte first. */

    /*
     * Incremental changes while we're not ready are nasty.
     */
    if (!(sse->state & SSE_STATE_INITIALIZED) || sse_manager_blocked)
	return(TRUE);

    if (!sse_sanity_check(sse, sse->protocol_tree[protocol])) {
	buginf("\nSSE: sanity failure at start of sse_delete");
	return(FALSE);
    }

    /*
     * Walk the tree, looking for the nodes to delete.
     */
    stat = &sse->stats[protocol];
    while (state != STATE_DONE) {
	parent = NULL;
	pred = &sse->protocol_tree[protocol];
	spin = *pred;
	if (!spin)			/* No tree, no problem */
	    return(FALSE);
	byteptr = start;
	byte = *byteptr;
	count = bytecount;
	if (count == 1) {
	    if (state == STATE_HIGH)
		byte = high;
	    else if (state == STATE_LOW) {
		if (byte == high)
		    state = STATE_HIGH;
	    }
	}

	if (!sse_sanity_check(sse, sse->protocol_tree[protocol])) {
	    buginf("\nSSE: sanity failure in sse_delete state %d", state);
	    return(FALSE);
	}
	
	while (count) {
	    spin->walk = parent;
	    switch (spin->type) {
	    case SPIN_COMPARE:
		if (spin->value == byte) {
		    count--;
		    byteptr++;
		    byte = *byteptr;
		    pred = &spin->EQADDR;
		    if (count == 1) {
			if (state == STATE_HIGH)
			    byte = high;
			else if (state == STATE_LOW) {
			    if (byte == high)
				state = STATE_HIGH;
			}
		    }
		} else if (byte > spin->value) 
		    pred = &spin->GTADDR;
		else if (byte < spin->value) 
		    pred = &spin->LTADDR;
		parent = spin;
		spin = *pred;
		break;
	    case SPIN_MATCH:
	    case SPIN_RANDOM:
		goto leaf;
		break;
	    default:
		if (sse_debug) {
		    uint *ptr;
		    blocktype *block;

		    buginf("\nSSE: sse_delete unknown spin type %d, "
			   "protocol %d node %x stack %x pred %x parent %x\n",
			   spin->type, protocol, spin, &stat, pred, parent);
		    if (parent) {
			ptr = (uint *)parent;
			while (ptr <= (uint *)&parent->protocol)
			    buginf("%8x ", *ptr++);
			block = (blocktype *)parent;
			block--;		/* point to block header */
			buginf("\nSSE: block data- alloc pc %x block magic %x",
			       block->allocator_pc, block->blockmagic);
			if ((block->blockmagic == BLOCKMAGIC) &&
			    (block->allocator_name)) {
			    buginf(" block process %s", block->allocator_name);
			}
		    }
		}
		return(TRUE);
	    }
	    if (!spin)
		break;
	}
	
    leaf:
	if (!spin)			/* Not in tree, so don't care */
	    goto iter;
	spin->walk = parent;
	if (spin->type != SPIN_MATCH) {
	    errmsg(&msgsym(NOMATCH, SSE), sse_node_type(spin->type), 
		   spin->value, *(ipaddrtype *)start, bytecount, high,
		   state);
	    sse_soft_error(sse);
	    return(TRUE);
	}
	if (count) {
	    errmsg(&msgsym(SWFAILURE, SSE),
		   "delete didn't consume all bytes", 0);
	    sse_soft_error(sse);
	    return(TRUE);
	}
	
	/*
	 * We've now located the branch in the tree to delete.  Walk back up
	 * the tree and prune it.  
	 * First free the match node and all of its extra addresses.
	 */
	if (!parent) {
	    /*
	     * This shouldn't happen.
	     */
	    sse_free_recall(sse, spin, 0, stat);
	    if (sse_debug)
		buginf("\nSSE: sse_delete found a match node with no parent");
	    return(TRUE);
	}

	/*
	 * If this is part of a range entry, then the lt or gt pointers can
	 * also map to the match node.  Clean 'em up.  Note that the order
	 * here MUST match sse_free_recall().
	 */
	if (parent->LTADDR == spin) {
	    sse_free_recall(sse, spin, 0, stat); /* free the match node */
	    deletes++;
	}
	if (parent->EQADDR == spin) {
	    sse_free_recall(sse, spin, 0, stat); /* free the match node */
	    deletes++;
	}
	if (parent->GTADDR == spin) {
	    sse_free_recall(sse, spin, 0, stat); /* free the match node */
	    deletes++;
	}
	spin = parent;

	do {
	    progress = FALSE;

	    /*
	     * spin now points to the bottom compare node that needs to be
	     * change.  We need to walk back up the tree to find the key point
	     * to delete the branch.  Any nodes below this node may be removed.
	     */
	    while (spin) {
		if (spin->EQADDR)
		    break;
		if (spin->LTADDR && spin->GTADDR) {
		    spin = sse_delete_fork(sse, protocol, spin, stat);
		    if (!spin) {		/* error */
			if (sse_debug)
			    buginf("\nSSE: delete fork error for %i",
				   *(ipaddrtype *)start);
			return(TRUE);
		    }
		    deletes++;
		} else if (spin->LTADDR) {
		    spin = sse_delete_dogleg(sse, protocol, spin,
					     &spin->LTADDR, stat);
		    if (!spin) {		/* error */
			if (sse_debug)
			    buginf("\nSSE: delete left dogleg error for %i",
				   *(ipaddrtype *)start);
			return(TRUE);
		    }
		    deletes++;
		} else if (spin->GTADDR) {
		    spin = sse_delete_dogleg(sse, protocol, spin,
					     &spin->GTADDR, stat);
		    if (!spin) {		/* error */
			if (sse_debug)
			    buginf("\nSSE: delete right dogleg error for %i",
				   *(ipaddrtype *)start);
			return(TRUE);
		    }
		    deletes++;
		}
		if (spin->EQADDR || spin->LTADDR || spin->GTADDR)
		    break;
		tmp = spin->walk;
		sse_free_recall(sse, spin, 0, stat);
		deletes++;
		spin = tmp;
	    }
	    
	    if (!spin) {
		sse->protocol_tree[protocol] = NULL; /* erased tree */
		sse_backpatch(sse, protocol);
		goto iter;
	    }
	    
	    /*
	     * We have changed pointers around for this node.  Regenerate
	     * all of the code for this node.
	     */
	    if (!sse_gen_compare(sse, spin)) {
		if (sse_debug)
		    buginf("\nSSE: sse_delete coding error 1");
		return(TRUE);
	    }
	    
	    if (state != STATE_LOW) {
		/*
		 * Finally, this node itself (and as a result, all above it)
		 * may be way out of balance.  Try to correct this
		 * somewhat.  Don't do any balancing if this the first of
		 * two passes that we have to make, because balancing
		 * shifts a pointer from the high node to the low node, and
		 * we then don't delete it.
		 */
		sse_tree_depth(sse, spin, NULL, NULL);
		
		olddepth = spin->depth;
		sse_branch_balance(sse, spin, protocol, TRUE);
		if (!spin->EQADDR && !spin->LTADDR && !spin->GTADDR)
		    progress = TRUE;
		else if (olddepth > spin->depth)
		    progress = TRUE;
	    }
	} while (progress);

    iter:
	/*
	 * Go to the next iteration.
	 */
	switch (state) {
	case STATE_LOW:
	    state = STATE_HIGH;
	    break;
	case STATE_HIGH:
	    state = STATE_DONE;
	    break;
	case STATE_DONE:
	default:
	    if (sse_debug)
		buginf("\nSSE: sse_delete didn't exit correctly");
	    return(TRUE);
	}
	pass++;
    }

    if (debug) {
	buginf("\nSSE: sse_delete took %d passes for %d deletes"
	       ", address %i high %d bytecount %d", pass,
	       deletes, *(ipaddrtype *)start, high, bytecount);
    }
    if (!sse_sanity_check(sse, sse->protocol_tree[protocol])) {
	buginf("\nSSE: sanity failure at end of sse_delete");
	return(TRUE);
    }


    return(FALSE);
}

/*
 * sse_compile
 * Compile the tree into actual SSE instructions.  Download it and
 * backpatch the running program.  Perform useful optimizations on the tree
 * here.  Someday this will do neat things like usage based balancing.  But
 * not yet.
 *
 * This function should convert NULL pointers from the tree to jumps to
 * SSE_GLOBAL_RP, which will pass it to the route processor.
 * Walking past a MATCH node should cause us to return.
 */

void sse_compile (ssetype *sse, int protocol)
{
    int retval;
    boolean gc_needed;
    ssestattype *stat;
    spintype *tmptree;

    if (protocol > MAX_SSE_PROTOCOL) {
	errmsg(&msgsym(SWFAILURE, SSE), "sse_compile requested protocol",
	       protocol); 
	return;
    }
    
    /*
     * Map any new entries in the rewrite database to rewrite memory.
     */
    gc_needed = FALSE;
    if (!sse_map_rwdb(sse, protocol))
	gc_needed = TRUE;
	
    process_may_suspend();
    
    /*
     * Assign addresses to instructions.
     */
    stat = &sse->stats[protocol];
    retval = sse_tree_walk(sse, sse_compiletree,
			   sse_address_node, NULL, stat, NULL); 
    if (OPCODE_ABORT == retval)
	gc_needed = TRUE;
    process_may_suspend();
    
    /*
     * Handle out of memory or out of rewrite memory errors by pruning the
     * tree. 
     */
    if (gc_needed) {
	if (sse_debug)
	    buginf("\nSSE: garbage collecting for %s",
		   sse_protocol_name(protocol));
	sse_tree_walk(sse, sse_compiletree, sse_gc_tree, 
		      sse_free_recall, stat, NULL); 
	process_may_suspend();
    }
    
    /*
     * Second pass at the tree.  
     */
    retval = sse_tree_walk(sse, sse_compiletree,
			   sse_code_gen, NULL, stat, NULL);
    process_may_suspend();
    if ((OPCODE_ABORT == retval) ||
	(sse_compiletree == NULL)) {
	
	/*
	 * There's no routing tree.  Point everything back to the RP.
	 */
	sse_purge(sse, protocol);

	/*
	 * Free the temporary queue and the old tree.  
	 */
	sse_free_tree(sse, sse_compiletree, stat);
	return;
    }
    
    /*
     * Now, free the old tree and save the new memory in
     * its place.  We can't free the old tree before this, because it was
     * being used until we backpatched the new tree into place.  Note that
     * we just lose any incremental changes that happen while we're doing a
     * rebuild. 
     */
#ifdef NOTDEF
    if (sse_debug) {
	sse_tree_nodecount = 0;
	if (sse->protocol_tree[protocol])
	    (void) sse_tree_walk(sse, sse->protocol_tree[protocol],
				 sse_tree_count, NULL, NULL, NULL);
	buginf("\nSSE: At compile, old start %#x, count %d",
	       sse->protocol_tree[protocol], sse_tree_nodecount);
	sse_tree_nodecount = 0;
	if (sse_compiletree)
	    (void) sse_tree_walk(sse, sse_compiletree,
				 sse_tree_count, NULL, NULL, NULL);
	buginf("\nSSE: At compile, new start %#x, count %d",
	       sse_compiletree, sse_tree_nodecount);
	sse_tree_nodecount = 0;
    }
#endif
    tmptree = sse->protocol_tree[protocol];
    sse->protocol_tree[protocol] = sse_compiletree;
    sse_compiletree = NULL;
    sse->depth[protocol] = sse->protocol_tree[protocol]->depth;

    /*
     * Backpatch the appropriate jump point into this table.
     */
    sse_backpatch(sse, protocol);

    process_may_suspend();

    /*
     * Now that no one is using it, we can free the old tree more leisurely.
     */
    sse_slow_free_tree(sse, tmptree, stat);
    process_may_suspend();
}

/*
 * sse_purge
 * Instead of compiling a tree, first call sse_init_tree and then sse_purge
 * to generate an empty tree. This is useful for protocols that are doing
 * incremental updates.
 */

void sse_purge (ssetype *sse, int protocol)
{
    spintype *tmptree;
    ssestattype *stat;

    if (sse_debug)
	buginf("\nSSE: no program for %s",
	       sse_protocol_name(protocol));
    tmptree = sse->protocol_tree[protocol];
    sse->protocol_tree[protocol] = NULL;
    sse_backpatch(sse, protocol);
    sse->depth[protocol] = 0;

    /*
     * Don't suspend before this, as it guarantees that process level could
     * not have performed an incremental add to the tree that we're just
     * about to throw away.
     */
    process_may_suspend();

    /*
     * Now that no one is using it, we can free the old tree more leisurely.
     */
    stat = &sse->stats[protocol];
    sse_slow_free_tree(sse, tmptree, stat);
    process_may_suspend();
}


/*
 *			   ICB tree calculation
 *
 * 1) Initial ICB tree.  Branch on the ICB to the appropriate bridge
 *    group tree (per i/f type) or null bridging tree (e.g., enobridge). 
 * 2) The bridge group tree CALLs the mac address tree for that bridge
 *    group and interface type for both destination and source addresses.
 *    If bridging would be successful, it stores an OUT.ICB and an
 *    appropriate mac classification.  The rewrite is set to 0 at the
 *    start of the SSE tree.
 * 3) The mac layer tree then branches to a protocol classification tree.
 *    If the protocol can not be classified, then we punt.
 * 4) We then branch to a protocol type/ICB decision.  Should this
 *    protocol type get routed on this interface?  If so, it branches to
 *    the protocol trees.  In fact, since we either route or bridge the
 *    protocol for the entire box, we could just put a hook into the protocol
 *    tree. 
 */

/*
 * sse_icbbranch
 * Figure out the MAC parsing tree for this interface.  This depends on
 * whether or not bridging is enabled and the media type.  Return an
 * address of a global routine to run.
 */

static int sse_icbbranch (hwidbtype *hwidb)
{
    int result;
    idbtype *idb;

    idb = firstsw_or_null(hwidb);
    if (!idb)
	return(SSE_GLOBAL_RP);
    if (reg_invoke_sse_icbtree_branch(hwidb, &result))
	return(result);

    /*
     * If no function deigned to tell us what to use, select a default.
     */
    if (hwidb->status & IDB_FDDI) 
	return(SSE_GLOBAL_FNOBRIDGE);
    else if (hwidb->status & IDB_ETHER) 
	    return(SSE_GLOBAL_ENOBRIDGE);
    else if (hwidb->status & IDB_SERIAL) {
	if ((hwidb->type == IDBTYPE_T1) ||
	    (hwidb->type == IDBTYPE_E1))
	    return(SSE_GLOBAL_MIP);
	else if (hwidb->enctype == ET_HDLC)
	    return(SSE_GLOBAL_HDLC);
	else if (hwidb->enctype == ET_PPP)
	    return(SSE_GLOBAL_PPP);
	else if ((hwidb->enctype == ET_FRAME_RELAY) ||
		 (hwidb->enctype == ET_FR_IETF)) {
	    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
		if (RUNNING_IF(LINK_FR_SWITCH, idb))
		    return(SSE_GLOBAL_RP);
	    }
	    return(SSE_GLOBAL_FRAME);
	} else
	    return(SSE_GLOBAL_RP);
    } else if (hwidb->status & IDB_TR) {
	if (RUNNING_IF(LINK_SRB, idb))
	    return(SSE_GLOBAL_TRBRIDGE);
	else
	    return(SSE_GLOBAL_TNOBRIDGE);
	}
    else if (hwidb->status & IDB_ATM)
	return(SSE_GLOBAL_ATM);
    else if (hwidb->status & IDB_CHANNEL)
	return(SSE_GLOBAL_CHANNEL);
    else 
	return(SSE_GLOBAL_RP);
}

/*
 * sse_protomask
 * Return the mask of routing protocols for this interface.
 */


static int sse_protomask (hwidbtype *hwidb)
{
    int mask = 0;
    idbtype *swidb;

    /*
     * Kludge alert: If the interface is on a MIP, each of the
     * channel groups could have a different protomask.  However, all of
     * the channel groups share a single ICB.  The correct thing to do is
     * to build a tree and parse the channel group number (which is in the
     * packet) and then set the protomask based on that.  This is a wee bit
     * o' work.  To simplify our lives, recall that we can never SSE bridge
     * something on the MIP, so the alternatives here would be to route
     * something or punt it.  We say that everything is routable for now.
     * If CRB is enabled on the box, this guess may well be wrong, so put
     * everything. 
     */
    if (hwidb->status & IDB_SERIAL) {
	if ((hwidb->type == IDBTYPE_T1) ||
	    (hwidb->type == IDBTYPE_E1)) {
	    if (global_crb_enable)
		return(0);
	    else
		return(SSE_ROUTING_ALL);
	}
    }

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (ROUTING_IF(swidb, LINK_IP))
	    mask |= SSE_ROUTING_IP;
	if (ROUTING_IF(swidb, LINK_APPLETALK))
	    mask |= SSE_ROUTING_ATALK;
	if (ROUTING_IF(swidb, LINK_VINES))
	    mask |= SSE_ROUTING_VINES;
	if (ROUTING_IF(swidb, LINK_APOLLO))
	    mask |= SSE_ROUTING_APOLLO;
	if (ROUTING_IF(swidb, LINK_NOVELL))
	    mask |= SSE_ROUTING_NOVELL;
	if (ROUTING_IF(swidb, LINK_DECNET))
	    mask |= SSE_ROUTING_DECNET;
	if (ROUTING_IF(swidb, LINK_XNS))
	    mask |= SSE_ROUTING_XNS;
	if (ROUTING_IF(swidb, LINK_CLNS))
	    mask |= SSE_ROUTING_CLNS;
    }
    return(mask);
}

/*
 * sse_icbtree_add_node
 * Add a node to the icb tree.
 */

static boolean sse_icbtree_add_node (ssetype *sse, spintype **tree,
				     hwidbtype *hwidb)
{
    spintype **pred, *spin;
    uchar byte;
    ssestattype *stat;
    
    /*
     * Add the address to the tree.
     */
    stat = &sse->overheadstat;
    pred = tree;
    spin = *pred;
    byte = (hwidb->cbus_icb_address >> 1) & 0xff;
    while (spin) {
	switch (spin->type) {
	case SPIN_COMPARE:
	    if (spin->value == byte) 
		return(TRUE);		/* duplicate icbs -- legal! */
	    else if (byte > spin->value) 
		pred = &spin->GTADDR;
	    else if (byte < spin->value) 
		pred = &spin->LTADDR;
	    spin = *pred;
	    break;
	default:
	    if (sse_debug)
		buginf("\nSSE: sse_icbprogram_add_node unknown type %d",
		       spin->type); 
	    return(FALSE);
	}
    }
    
    /*
     * The icb is not in the tree.
     */
    spin = sse_malloc_spin(0, stat);
    if (!spin) 
	return(FALSE);
    sse_init_spin(spin, byte, SSE_INST_NOP, SSE_INST_RD_BYTE, SSE_INST_NOP);
    *pred = spin;
    pred = &spin->EQADDR;
    
    /*
     * Add the match node.  If we ever route more than 8 protocols in the
     * box, we have to add another bit here so that we can filter them.  In
     * that case, load two feedback registers with halves of the mask.
     */
    spin = sse_malloc_spin(0, stat);
    if (!spin) 
	return(FALSE);
    spin->type = SPIN_MATCH;
    spin->EXTRACOUNT = 0;
    spin->protoinfo = sse_icbbranch(hwidb);
    spin->nodeinfo.match.protoinfo2 = sse_protomask(hwidb);
    *pred = spin;
#ifdef NOTDEF
    if (sse_debug) {
	idbtype *idb;
	idb = firstsw_or_null(hwidb);
	buginf("\nSSE: interface %s icb %#x addr %#x status %#x"
	       " protos %#x", idb ? idb->namestring : "" , byte,
	       spin->protoinfo, hwidb->status,
	       spin->nodeinfo.match.protoinfo2);
    }
#endif
    return(TRUE);
}

/*
 * sse_icbtree_code_gen
 * Generate (and install!) code for the ICB tree.
 */

static int sse_icbtree_code_gen (ssetype *sse, spintype *spin,
				 ssestattype *stat, queuetype *queue)
{
    shadowwordtype tmp;
    
    switch (spin->type) {
    case SPIN_COMPARE:
	tmp.lt.inst = spin->nodeinfo.compare.lt.inst;
	tmp.eq.inst = spin->nodeinfo.compare.eq.inst;
	tmp.gt.inst = spin->nodeinfo.compare.gt.inst;
	if (spin->LTADDR) {
	    tmp.lt.ptr = spin->LTADDR->physaddr;
	    tmp.lt.value = spin->LTADDR->value;
	} else
	    tmp.lt.ptr = SSE_GLOBAL_RP;    
	if (spin->EQADDR) 
	    tmp.eq.ptr = spin->EQADDR->physaddr;
	else {
	    errmsg(&msgsym(SWFAILURE, SSE), "icb spin node without match",
		   spin);
	    crashdump(10);
	    tmp.eq.ptr = SSE_GLOBAL_RP;
	}
	if (spin->GTADDR) {
	    tmp.gt.ptr = spin->GTADDR->physaddr;
	    tmp.gt.value = spin->GTADDR->value;
	} else
	    tmp.gt.ptr = SSE_GLOBAL_RP;    
	sse_poke_treemem(sse, spin->physaddr, &tmp);
	break;
    case SPIN_MATCH:
	sse_init_sw(&tmp, spin->nodeinfo.match.protoinfo2,
		    SSE_INST_LD_SREG_1, spin->protoinfo);
	sse_poke_treemem(sse, spin->physaddr, &tmp);
	break;
    default:
	break;
    }
    return(OPCODE_WALK);
}

/*
 * sse_build_icbtree
 * Build the input interface tree
 */

static void sse_build_icbtree (ssetype *sse)
{
    hwidbtype *hwidb;
    spintype *newtree;
    int retval;
    shadowwordtype word;
    
    newtree = NULL;
    FOR_ALL_HWIDBS(hwidb) {
	if (!(hwidb->status & IDB_CBUS))
	    continue;			/* look for cbus interfaces*/
	if (hwidb->status & IDB_NOHARDWARE) /* ignore removed interfaces */
	    continue;
	if (sse->real) 
	    if (hwidb->devctl != sse->cbus)
		continue;		/* look for ones on this cbus */
	if (!sse_icbtree_add_node(sse, &newtree, hwidb)) {
	    /*
	     * If an add failed, keep the existing tree and punt.
	     */
	    if (sse_debug)
		buginf("\nSSE: icb tree add failure");
	    sse_free_tree(sse, newtree, &sse->overheadstat);
	    return;
	}
    }

    /*
     * Clean up the tree just generated and install it.
     */
    retval = sse_tree_walk(sse, newtree, sse_address_node, NULL,
			   &sse->overheadstat, NULL); 
    if (OPCODE_ABORT == retval) {

	/*
	 * Something is very wrong.  Just stay with the tree we have.
	 */
	if (sse_debug)
	    buginf("\nSSE: icb tree addressing failure");
	sse_free_tree(sse, newtree, &sse->overheadstat);
    } else if (newtree) {
	
	/*
	 * Generate the tree and backpatch.
	 */
	sse_tree_walk(sse, newtree, sse_icbtree_code_gen, NULL, NULL, NULL);
	sse_peek_treemem(sse, SSE_GLOBAL_INPUTIF, &word);
	word.lt.ptr = newtree->physaddr;
	word.eq.ptr = newtree->physaddr;
	word.gt.ptr = newtree->physaddr;
	word.lt.value = newtree->value;
	word.eq.value = newtree->value;
	word.gt.value = newtree->value;
	sse_poke_treemem(sse, SSE_GLOBAL_INPUTIF, &word);
	sse_free_tree(sse, sse->inputif, &sse->overheadstat);
	sse->inputif = newtree;
    }
}



/*
 *			  User interface routines
 */



/*
 * clear_sse
 * Clear an SSE and reset it.
 */

void clear_sse (parseinfo *csb)
{
    mcitype *cbus;
    ssetype *sse;
    
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	if (!(sse->state & SSE_STATE_INITIALIZED)) {
	    printf(sse_reinitializing);
	    continue;
	}
	if (csb->which != SSE_PROTOCOL_ANY)
	    reg_invoke_sse_protocol_reset(csb->which, &sse->protoinfo);
	else {
	    if (sse->real)
		sse_soft_reset(cbus);
	    else
		reg_invoke_sse_load(csb);
	}
    }
    sse_manager_wakeup = TRUE;
}

/*
 * sse_node_type
 * Return a string to the usage of this node
 */

char sse_node_type (spin_type type)
{
    switch(type) {
    case SPIN_COMPARE:
	return('c');
    case SPIN_COMPARE_FINAL_TERM:
	return('f');
    case SPIN_COMPARE_NXT_TERM:
	return('n');
    case SPIN_MATCH:
	return('m');
    case SPIN_RANDOM:
	return('r');
    default:
	return('?');
    }
}

/*
 * sse_show_spin
 * Display a single spin node.
 */

int sse_show_spin (ssetype *sse, spintype *spin, ssestattype *stat, queuetype
		   *queue)
{
    int base = (int) stat;		/* Tacky kludge */

    printf("\n%8x %c    %5d %8x ", spin, sse_node_type(spin->type),
	   spin->depth, spin->physaddr);
    switch(spin->type) {
    case SPIN_COMPARE:
	switch (base) {
	case BASE_DECIMAL:
	    printf("%4d", spin->value);
	    break;
	case BASE_HEX:
	default:
	    printf("%4x", spin->value);
	    break;
	}
	printf("%8x %2x %8x %2x %8x %2x", 
	       spin->LTADDR, spin->nodeinfo.compare.lt.inst,
	       spin->EQADDR, spin->nodeinfo.compare.eq.inst,
	       spin->GTADDR, spin->nodeinfo.compare.gt.inst);
	break;
    case SPIN_MATCH:
	printf("(%d %x %x %x %x)", spin->nodeinfo.match.usage,
	       spin->nodeinfo.match.remainder, spin->OUTRW,
	       spin->protoinfo, spin->nodeinfo.match.protoinfo2 );
	break;
    case SPIN_RANDOM:
	printf("     %8x    %8x    %8x", spin->LTADDR, spin->EQADDR,
	       spin->GTADDR);
	break;
    default:
	break;
    }
    if (automore_quit())
	return(OPCODE_ABORT);
    return(OPCODE_WALK);
}

/*
 * show_sse_internals
 * Given a spintype, display the results nicely.
 */
const char show_sse_internal_header[] =
    "\nAddress  Type Len   Physical Val LTaddr   In EQaddr   In GTaddr   In";

void show_sse_internals (spintype *spin, int base, sse_callback func)
{
    mcitype *cbus;
    ssetype *sse;

    automore_enable(show_sse_internal_header);
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	if (!(sse->state & SSE_STATE_INITIALIZED)) {
	    printf(sse_reinitializing);
	    continue;
	}
	sse_safe_tree_walk(sse, spin, func, NULL, (ssestattype *)
			   base, NULL);		/* Tacky kludge */
    }
    automore_disable();
}

/*
 * show_sse_ip_trace
 * Walk the spin tree for an IP address and print the nodes traversed. This
 * should really be generalized and moved out of here.
 */

static void show_sse_ip_trace (ssetype *sse, ipaddrtype addr)
{
    int count, curr;
    uchar bytes[4];
    spintype *spin;
    boolean found;

    spin = sse->protocol_tree[SSE_PROTOCOL_IP];
    PUTLONG(bytes, addr);
    automore_enable(show_sse_internal_header);
    for (count = 4; spin && count; count--) {
	curr = bytes[4 - count];
	found = FALSE;
	while (!found && spin) {
	    if (automore_quit())
		return;
	    switch (spin->type) {
	    case SPIN_COMPARE:
		sse_show_spin(sse, spin, (ssestattype *)BASE_DECIMAL, NULL);
		if (curr < spin->value)
		    spin = spin->LTADDR;
		else if (curr == spin->value) {
		    spin = spin->EQADDR;
		    found = TRUE;
		} else if (curr > spin->value)
		    spin = spin->GTADDR;
		break;
	    case SPIN_MATCH:
	    case SPIN_RANDOM:
		sse_show_spin(sse, spin, (ssestattype *)BASE_DECIMAL, NULL);
		count = 1;
		found = TRUE;
		return;
	    default:
		errmsg(&msgsym(SWFAILURE, SSE), "unknown node", spin); 
		printf("\nUnknown node type %d at %#x\n", spin->type, spin);
		count = 1;
		found = TRUE;
		return;
	    }
	}
    }
    if (count)
	printf("\nTrace failed");
    automore_disable();
}

/*
 * show_sse_proto_rewrite
 * Show the MAC rewrite cache for a protocol.
 */

static void show_sse_proto_rewrite (int protocol)
{
    int i, j, depth;
    rwdbtype *rwdb;
    mcitype *cbus;
    ssetype *sse;

    automore_enable("Hash   Address  Proto Ref MAC Header");

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	if (!(sse->state & SSE_STATE_INITIALIZED)) {
	    printf(sse_reinitializing);
	    continue;
	}

	for (i = 0; i < NETHASHLEN; i++) {
	    if (automore_quit())
		return;
	    rwdb = sse->rewrite_table[i];
	    depth = 1;
	    for (; rwdb; rwdb = rwdb->next, depth++) {
		if ((rwdb->proto != protocol) &&
		    (protocol != SSE_PROTOCOL_ANY))
		    continue;
		printf("\n%-02x/%3d %8x %5s %3d ", i, depth,
		       rwdb->physaddr, sse_protocol_name(rwdb->proto), 
		       mem_refcount(rwdb)-1);
		for (j = 0; j < rwdb->len; j++) {
		    printf("%-02x", rwdb->encaps[j]);
		    if (j == 21)
			printf("\n%30c",' ');
		}
	    }
	}
    }
    automore_disable();
}

/*
 * show_sse_match
 * Show the match node cache.
 */

static void show_sse_match (ssetype *sse)
{
    int i, depth;
    matchtype *curr;
    spintype *spin;

    automore_enable("Hash   Address  Proto Ref Rewrite  Parents Remainder");
    
    if (!sse)
	return;
    if (!(sse->state & SSE_STATE_INITIALIZED)) {
	printf(sse_reinitializing);
	return;
    }

    for (i = 0; i < NETHASHLEN; i++) {
	if (automore_quit())
	    return;
	curr = sse->match_cache[i];
	depth = 1;
	for (; curr; curr = curr->next, depth++) {
	    mem_lock(curr);
	    spin = curr->node;
	    mem_lock(spin);
	    printf("\n%-02x/%3d %8x %5s %3d %8x %7d %d", i, depth,
		   spin->physaddr, sse_protocol_name(spin->protocol), 
		   mem_refcount(spin) - 2, spin->OUTRW,
		   spin->parents,
		   spin->nodeinfo.match.remainder);
	    free(spin);
	    if (mem_refcount(curr) == 1) {
		free(curr);
		break;
	    }
	    free(curr);
	}
    }
    automore_disable();
}

/*
 * show_sse
 * Deal with all of the show commands.
 */

void show_sse (parseinfo *csb)
{
    spintype *spin;
    int protocol, command;
    int addr;
    mcitype *cbus;
    ssetype *sse;
    ipaddrtype ipaddr;

    protocol = csb->which;
    command = GETOBJ(int, 1);
    if (protocol > MAX_SSE_PROTOCOL) {
	errmsg(&msgsym(SWFAILURE, SSE), "requested protocol",
	       protocol); 
	return;
    }
    switch (command) {
    case SSE_CMD_ADDRESS:
	if (protocol != SSE_PROTOCOL_IP) {
	    printf("Unimplemented");
	    break;
	}
	ipaddr = GETOBJ(paddr,3)->ip_addr;
	printf("\nTrace for %i", ipaddr);
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		continue;
	    }
    	    show_sse_ip_trace(sse, ipaddr);
	}
	break;
    case SSE_CMD_CONSISTENCY:
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		continue;
	    }
	    if (!sse->real)
		continue;
	    reg_invoke_sse_consistency(protocol, sse, protocol);
	}
	break;
    case SSE_CMD_INTERFACE:
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		continue;
	    }
    	    show_sse_internals(sse->inputif, BASE_HEX, sse_show_spin);
	}
	break;
    case SSE_CMD_INTERNALS:
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		continue;
	    }
	    spin = sse->protocol_tree[protocol];
	    show_sse_internals(spin, protocol == SSE_PROTOCOL_IP ?
			       BASE_DECIMAL : BASE_HEX, sse_show_spin);
	}
	break;
    case SSE_CMD_MEMFREE:
	show_sse_memfree();
	break;
    case SSE_CMD_MEMORY:
	addr = GETOBJ(int, 2);
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		continue;
	    }

	    show_sse_memory(sse, addr);
	}
	break;
    case SSE_CMD_REWRITE:
	if (SSE_PROTOCOL_ANY == protocol) {
	    addr = GETOBJ(int, 2);
	    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
		if (!cbus->sseptr)
		    continue;
		sse = cbus->sseptr;
		if (!(sse->state & SSE_STATE_INITIALIZED)) {
		    printf(sse_reinitializing);
		    continue;
		}
		show_sse_rewrite(sse, addr);
	    }
	} else
	    show_sse_proto_rewrite(protocol);
	break;
    case SSE_CMD_SUMMARY:
	show_sse_summary();
	break;
    case SSE_CMD_MATCH:
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		continue;
	    }

	    show_sse_match(sse);
	}
	break;
    default:
	printf("%%Unknown command");
    }
}

/*
 * sse_protocol_name
 * Return a string pointing to the protocol name for each protocol.
 */

char *sse_protocol_name (int protocol)
{
    switch (protocol) {
    case SSE_PROTOCOL_IP:
	return("IP");
    case SSE_PROTOCOL_IPX:
	return("IPX");
    case SSE_PROTOCOL_SRB:
	return("SRB");
    case SSE_PROTOCOL_CLNS:
	return("CLNP");
    default:
	return("?");
    }
}

/*
 * show_sse_summary
 * Show summary statistics
 */

static void show_sse_summary (void)
{
    int i;
    mcitype *cbus;
    ssetype *sse;

    automore_enable(NULL);
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (!cbus->sseptr)
	    continue;
	sse = cbus->sseptr;
	if (!(sse->state & SSE_STATE_INITIALIZED)) {
	    printf(sse_reinitializing);
	    continue;
	}
	
	printf("SSE utilization statistics");
	printf("\n\n               Program words  Rewrite bytes  Internal"
	       " nodes  Depth  Rebuilds");
	printf("\nOverhead       ");
	printf(SSE_STAT_STRING SSE_INTERN_STRING, sse->overheadstat.words_used,
	       sse->overheadstat.rwbytes_used, sse->overheadstat.spin_used); 
	for (i = MIN_SSE_REAL_PROTOCOL; i <= MAX_SSE_PROTOCOL; i++) {
	    printf("\n%15s", sse_protocol_name(i));
	    printf(SSE_STAT_STRING SSE_INTERN_STRING SSE_DEPTH_STRING, 
		   sse->stats[i].words_used, sse->stats[i].rwbytes_used,
		   sse->stats[i].spin_used, sse->depth[i], sse->rebuilds[i]);  
	}
	reg_invoke_sse_print_stats(&sse->protoinfo);
	printf("\nTotal used     ");
	printf(SSE_STAT_STRING SSE_INTERN_STRING, sse->words_used,
	       sse->rwbytes_used, spin_malloced - spin_freed);
	printf("\nTotal free     ");
	printf(SSE_STAT_STRING, sse->words_avail - sse->words_used,
	       sse->rwbytes_avail - sse->rwbytes_used);
	printf("\nTotal available");
	printf(SSE_STAT_STRING, sse->words_avail, sse->rwbytes_avail);

	printf("\n\nInternals\n  %u internal nodes allocated, %u freed",
	       spin_malloced, spin_freed);  
	printf("\n  SSE manager process %sabled", sse_manager_blocked ?
	       "dis": "en");
	printf(", microcode %sabled and %srunning",
	       (sse->state & SSE_STATE_UCODE_DISABLED) ? "dis" : "en",
	       (sse->state & SSE_STATE_UCODE_ON) ? "" : "not ");
	printf(", %d hangs", sse->hangcount);
	if (sse->outofmem_count)
	    printf("\n  %d out of memory conditions",
		   sse->outofmem_count);
    }
    automore_disable();
}

/*
 * show_sse_memfree
 * Show memory map
 */

static void show_sse_memfree (void)
{
    mcitype *cbus;
    ssetype *sse;

    automore_enable(NULL);
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (!cbus->sseptr)
	    continue;
	sse = cbus->sseptr;
	if (!(sse->state & SSE_STATE_INITIALIZED)) {
	    printf(sse_reinitializing);
	    continue;
	}
	printf("SSE memory map");
	printf("\n\nFree program memory");
	show_rangelist(sse->freetreememory);
	printf("\nFree rewrite memory");
	show_rangelist(sse->freerwmemory);
    }
    automore_disable();
}

/*
 * test_sse
 * UI for "test sse" command
 */

void test_sse (parseinfo *csb)
{
    mcitype *cbus;
    ssetype *sse;

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (!cbus->sseptr)
	    continue;
	sse = cbus->sseptr;
	switch (csb->which) {
	case SSE_TEST_MANAGER:
	    sse_manager_blocked = !sse_manager_blocked;
	    printf("SSE manager %sabled", sse_manager_blocked ? "dis" :
		   "en");
	    if (sse_debug)
		buginf("\nSSE: manager %sabled", sse_manager_blocked ? "dis" :
		       "en");
	    if (!(sse->state & SSE_STATE_INITIALIZED)) {
		printf(sse_reinitializing);
		break;
	    }
	    if (!sse_manager_blocked)
		sse_soft_reset(cbus);
	    break;
	case SSE_TEST_MICROCODE:
	    if (!sse->real)
		continue;
	    sse->state ^= SSE_STATE_UCODE_DISABLED;
	    if (sse->state & SSE_STATE_UCODE_DISABLED)
		sse_hw_off(cbus);
	    else {
		sse->state |= (SSE_STATE_HWRESET|SSE_STATE_SWRESET);
		sse_manager_wakeup = TRUE;
	    }
	    printf("SSE microcode %sabled",
		   (sse->state & SSE_STATE_UCODE_DISABLED) ? "dis" : "en");
	    break;	    
	case SSE_TEST_SANITY:
	    sse_sanity_checks = !sse_sanity_checks;
	    printf("SSE sanity checks %sabled", sse_sanity_checks ? "en" :
		   "dis");
	    break;	
	}
    }
}

/*
 * sse_manager_command
 * Hidden command for benchmarking tuning parameters.
 */

void sse_manager_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case SSE_MANAGER_QUIET:
	    nv_write(SSE_MINSLEEP != sse_manager_quiet, "%s %d",
		     csb->nv_command, sse_manager_quiet/ONESEC); 
	    break;
	case SSE_MANAGER_PERIOD:
	    nv_write(SSE_REFRESH != sse_manager_period, "%s %d",
		     csb->nv_command, sse_manager_period/ONESEC); 
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case SSE_MANAGER_QUIET:
	if (csb->sense)
	    sse_manager_quiet = GETOBJ(int, 1)*ONESEC;
	else
	    sse_manager_quiet = SSE_MINSLEEP;
	break;
    case SSE_MANAGER_PERIOD:
	if (csb->sense)
	    sse_manager_period = GETOBJ(int, 1)*ONESEC;
	else
	    sse_manager_period = SSE_REFRESH;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * sse_gen_consist_walker
 * Tree walker routine for generic consistency checking.
 */

static ulong sse_consistency_nodes;	/* number of spin nodes found */
static ulong sse_consistency_addrs;	/* number of addresses found */

static int sse_gen_consist_walker (ssetype *sse, spintype *spin, ssestattype
				   *stat, queuetype *queue)
{
    int base = (int) stat;		/* Tacky kludge */
    shadowwordtype sw, mem;
    int *ptr;

    sse_consistency_nodes++;
    switch(spin->type) {
    case SPIN_COMPARE:
	if (spin->physaddr == SSE_UNMAPPED)
	    break;
	sse_consistency_addrs++;
	sse_peek_treemem(sse, spin->physaddr, &mem);
	if (!sse_calc_compare(spin, &sw))
	    break;
	if (!bcmp(&sw, &mem, sizeof(shadowwordtype)))
	    break;

	printf("\n%8x %c    %5d %8x ", spin, sse_node_type(spin->type),
	       spin->depth, spin->physaddr);
	switch (base) {
	case BASE_DECIMAL:
	    printf("%4d", spin->value);
	    break;
	case BASE_HEX:
	default:
	    printf("%4x", spin->value);
	    break;
	}
	printf("%8x %2x %8x %2x %8x %2x", 
	       spin->LTADDR, spin->nodeinfo.compare.lt.inst,
	       spin->EQADDR, spin->nodeinfo.compare.eq.inst,
	       spin->GTADDR, spin->nodeinfo.compare.gt.inst);
	ptr = (int *) &mem;
	printf("\n\tReal: %08x %08x %08x", *ptr++, *ptr++, *ptr);
	ptr = (int *) &sw;
	printf("\n\tCalc: %08x %08x %08x", *ptr++, *ptr++, *ptr);
	break;
    case SPIN_MATCH:
	if (spin->physaddr == SSE_UNMAPPED)
	    break;
	sse_consistency_addrs += 1 + spin->EXTRACOUNT;
	break;
    case SPIN_RANDOM:
	if (spin->physaddr == SSE_UNMAPPED)
	    break;
	sse_consistency_addrs += 1;
	break;
    default:
	break;
    }
    if (automore_quit())
	return(OPCODE_ABORT);
    return(OPCODE_WALK);
}

/*
 * sse_low_mem
 * Return TRUE if we are low on tree or RW memory.
 */

static boolean sse_low_mem (void)
{
    ssetype *sse;
    mcitype *cbus;

    if (sse_manager_blocked)
	return(FALSE);

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {

	sse = cbus->sseptr;

	if (!sse)
	    continue;

	if ((sse->words_avail - sse->words_used) < SSE_LOW_TREEMEM_THRESH) {
	    if (sse_debug)
		buginf("\nSSE: low mem threshold reached.");
	    return(TRUE);
	}

	if ((sse->rwbytes_avail - sse->rwbytes_used) < SSE_LOW_RWMEM_THRESH) {
	    if (sse_debug)
		buginf("\nSSE: low RW mem threshold reached.");
	    return(TRUE);
	}
    }

    return(FALSE);
}

/*
 * sse_generic_consistency
 * Generic consistency checker.  Only walks compare nodes, doesn't expect
 * anything funky.
 */

void sse_generic_consistency (ssetype *sse, int protocol)
{
    int base;

    automore_enable(show_sse_internal_header);
    if (protocol == SSE_PROTOCOL_IP)
	base = BASE_DECIMAL;
    else
	base = BASE_HEX;
    sse_consistency_nodes = 0;
    sse_consistency_addrs = 0;
    sse_safe_tree_walk(sse, sse->protocol_tree[protocol],
		       sse_gen_consist_walker, NULL, (ssestattype *) base,
		       NULL);
    printf("\nFound %d nodes and %d allocated addresses",
	   sse_consistency_nodes, sse_consistency_addrs);
    automore_disable();
}

/*
 * sse_register_thyself
 * Register all functions as appropriate.  Cannot be called before
 * scheduler fires up.
 */

static void sse_register_thyself (void)
{
    reg_add_media_change_mtu(sse_set_mtu, "sse_set_mtu");
    reg_add_hwif_goingdown(sse_hwif_goingdown, "sse_hwif_goingdown");
    reg_add_sse_protoinfo(sse_proto_info, "sse_proto_info");
    reg_add_bump_cache_version(sse_bump_cache_version,
			       "sse_bump_cache_version");
    reg_add_sse_wake_manager(sse_wake_manager, "sse_wake_manager");
    reg_add_fs_low_mem(sse_low_mem, "sse_low_mem");
}

/*
 * sse_subsys_init
 * Initialize the subsystem.  This is mostly creating registries.
 */

static void sse_subsys_init (subsystype* subsys)
{
    sse_register_thyself();
    sse_debug_init();
    sse_manager_period = SSE_REFRESH;
    TIMER_START(sse_periodic, sse_manager_period);
    sse_manager_quiet = SSE_MINSLEEP;
    TIMER_START(sse_minsleep, sse_manager_quiet);
    sse_manager_blocked = sse_manager_running = sse_manager_wakeup = FALSE;
    spin_malloced = spin_freed = sse_quantum = sse_max_quantum =
	sse_max_quantum_pc = sse_max_iteration = sse_tree_nodecount = 0;
    sse_sanity_checks = FALSE;
    queue_init(&spinfreeQ, 0);
}


/* 
 * SSE subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(sse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      sse_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

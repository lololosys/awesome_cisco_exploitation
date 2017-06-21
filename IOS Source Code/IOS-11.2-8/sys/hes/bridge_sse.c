/* $Id: bridge_sse.c,v 3.7.26.5 1996/08/28 12:43:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/bridge_sse.c,v $
 *------------------------------------------------------------------
 * bridge_sse.c -- SSE Transparent Bridge Support
 *
 * Jan 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bridge_sse.c,v $
 * Revision 3.7.26.5  1996/08/28  12:43:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.26.4  1996/05/17  11:16:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.14.3  1996/05/05  23:06:16  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.14.2  1996/04/26  07:53:30  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.14.1  1996/04/03  14:26:51  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.26.3  1996/05/04  00:44:10  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Revision 3.7.26.2  1996/04/30  00:43:03  gchristy
 * CSCdi54780:  SSE: adding bridge-group > 9 to FDDI or ether causes hang
 * Branch: California_branch
 * Bounce all packets received on ether or fddi interfaces configured
 * with unsupported bridge groups up to the RP.
 *
 * Revision 3.7.26.1  1996/03/18  19:39:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.1  1996/01/24  21:44:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/18  22:46:44  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/09  21:03:55  fred
 * CSCdi43225:  ipx ping fails with autonomous switching enabled on mip
 *              need to disable fair queuing on any interface that is
 *              autonomous switched, as SP/SSE don't do fancy queuin
 *
 * Revision 3.5  1995/12/27  22:45:52  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE: Regis
 * - Code review modification
 *
 * Revision 3.4  1995/12/27  18:58:49  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE:Registry 8 service 12 doesn't exist
 *              error message
 * - bgroup_sse parser node is removed from bgroup tree and dynamically
 *   added to the 7000 parser chain.
 *
 * Revision 3.3  1995/12/19  18:24:24  wilber
 * CSCdi45124:  bridging on non sse capable int disables sse bridging
 * If the bridging interface is neither fddi nor ether, just bypass the
 * tree building and don't disable SSE
 *
 * Revision 3.2  1995/11/17  09:14:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/25  11:43:38  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.5  1995/07/06 00:24:02  wilber
 * CSCdi36382:  Age field is not updated correctly for filtered on-net
 * traffic
 * - Create counters to account for filtered on-net traffic
 * - Show those counters only in verbose option of show bridge output
 * - Get timestamp when creating a bridge table entry
 * - Update the bte age based on rx counter plus filtered rx counter
 *
 * Revision 2.4  1995/06/28  09:24:19  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/18  23:16:25  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:04:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:37:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "fastswitch_registry.h"
#include "config.h"
#include "if_mci.h"
#include "if_fci.h"
#include "subsys.h"
#include "../srt/span.h"
#include "../tbridge/tbridge_public.h"
#include "../util/range.h"
#include "../if/network.h"
#include "../srt/srt_registry.h"
#include "../if/priority_private.h"
#include "../parser/parser_defs_sse.h"
#include "sse.h"
#include "../hes/ssesym.h"
#include "sse_private.h"
#include "../hes/sse_registry.h"
#include "bridge_sse_private.h"

static const treeaddrtype sse_group_ether_backpatches[MAX_SSE_BRIDGE_GROUPS] =
{
    SSE_GLOBAL_EBRIDGEGRP0,		/* 0 */
    SSE_GLOBAL_EBRIDGEGRP1,		/* 1 */
    SSE_GLOBAL_EBRIDGEGRP2,		/* 2 */
    SSE_GLOBAL_EBRIDGEGRP3,		/* 3 */
    SSE_GLOBAL_EBRIDGEGRP4,		/* 4 */
    SSE_GLOBAL_EBRIDGEGRP5,		/* 5 */
    SSE_GLOBAL_EBRIDGEGRP6,		/* 6 */
    SSE_GLOBAL_EBRIDGEGRP7,		/* 7 */
    SSE_GLOBAL_EBRIDGEGRP8,		/* 8 */
    SSE_GLOBAL_EBRIDGEGRP9,		/* 9 */
};

static const treeaddrtype sse_group_fddi_backpatches[MAX_SSE_BRIDGE_GROUPS] = {
    SSE_GLOBAL_FBRIDGEGRP0,		/* 0 */
    SSE_GLOBAL_FBRIDGEGRP1,		/* 1 */
    SSE_GLOBAL_FBRIDGEGRP2,		/* 2 */
    SSE_GLOBAL_FBRIDGEGRP3,		/* 3 */
    SSE_GLOBAL_FBRIDGEGRP4,		/* 4 */
    SSE_GLOBAL_FBRIDGEGRP5,		/* 5 */
    SSE_GLOBAL_FBRIDGEGRP6,		/* 6 */
    SSE_GLOBAL_FBRIDGEGRP7,		/* 7 */
    SSE_GLOBAL_FBRIDGEGRP8,		/* 8 */
    SSE_GLOBAL_FBRIDGEGRP9,		/* 9 */
};

static const treeaddrtype sse_ether_cache[MAX_SSE_BRIDGE_GROUPS] = {
    SSE_GLOBAL_ETB0,			/* 0 */
    SSE_GLOBAL_ETB1,			/* 1 */
    SSE_GLOBAL_ETB2,			/* 2 */
    SSE_GLOBAL_ETB3,			/* 3 */
    SSE_GLOBAL_ETB4,			/* 4 */
    SSE_GLOBAL_ETB5,			/* 5 */
    SSE_GLOBAL_ETB6,			/* 6 */
    SSE_GLOBAL_ETB7,			/* 7 */
    SSE_GLOBAL_ETB8,			/* 8 */
    SSE_GLOBAL_ETB9,			/* 9 */
};

static const treeaddrtype sse_fddi_cache[MAX_SSE_BRIDGE_GROUPS] = {
    SSE_GLOBAL_FTB0,			/* 0 */
    SSE_GLOBAL_FTB1,			/* 1 */
    SSE_GLOBAL_FTB2,			/* 2 */
    SSE_GLOBAL_FTB3,			/* 3 */
    SSE_GLOBAL_FTB4,			/* 4 */
    SSE_GLOBAL_FTB5,			/* 5 */
    SSE_GLOBAL_FTB6,			/* 6 */
    SSE_GLOBAL_FTB7,			/* 7 */
    SSE_GLOBAL_FTB8,			/* 8 */
    SSE_GLOBAL_FTB9,			/* 9 */
};

static const treeaddrtype sse_failaddr[STATIONLEN_IEEE48+1] = {
    SSE_GLOBAL_TBSKIP0,			/* 0 */
    SSE_GLOBAL_TBSKIP0,			/* 0 */
    SSE_GLOBAL_TBSKIP1,			/* 1 */
    SSE_GLOBAL_TBSKIP2,			/* 2 */
    SSE_GLOBAL_TBSKIP3,			/* 3 */
    SSE_GLOBAL_TBSKIP4,			/* 4 */
    SSE_GLOBAL_TBSKIP5,			/* 5 */
};

static treeaddrtype bridge_cache_address; /* Kludge to pass address of top */
					  /* of tree into code gen */ 

/*
 * Forward declerations
 */
static boolean bridge_sse_add(ssetype *sse, sse_protoinfo *protoinfo,
			      uchar *macaddr, int bce_addr, int group,
			      idbtype *idb, boolean self);
static int bridge_sse_gen(ssetype *sse, spintype *spin, ssestattype
			  *stat, queuetype *queue);
static void bridge_group_tree(ssetype *sse, sse_protoinfo *protoinfo, uint
			      index, int media);
static void sse_bridge_on(ssetype *sse);
static void sse_bridge_off(void);
static void sse_bridging_setup(ssetype *sse);


void bridgegroup_sse_command (parseinfo *csb)
{
    spantype *span;
    idbtype *idb;
    hwidbtype *hwidb;
    int index;

    idb = csb->interface;
    hwidb = idb->hwptr;

    if (csb->nvgen) {
	index = idb->span_index;
	if ((!index) || (index != GETOBJ(int,1))) {
	    return;
	}
	nv_write(idb->hwptr->sse_bridge_forwarding, "%s",
		 csb->nv_command);
	return;
    }
    if (!bridge_enable) {
        printf("\nBridging%s", notsupp);
	return;
    }
    
    if ((hwidb->status & IDB_BRIDGE) == 0) {
        printf(nobridging, idb->namestring);
	return;
    }

    /*
     * Get index and make sure that it is in range.
     * Do not allow index to be changed to other than zero.
     */
    index = GETOBJ(int,1);

    if ((index) && ((index < 1) || (index >= SPAN_MAXINDEX))) {
	printf(badspan);
	return;
    }

    if (idb->span_index && index && (index != idb->span_index)) {
	printf("\n\nInterface already configured within Bridge Group %d.\n",
	       idb->span_index);
	return;
    }

    span = span_lookup(index, FALSE, 0);
    if (!span) {
	printf(nomemory);
	return;
    }
    
    if (span->protocol == SPAN_PROTO_IBM) {
	printf("\nBridge %d is running AST.", index);
	printf("\nBridge group command will not be accepted.");
	return;
    }

    if (csb->sense) {
        if (!reg_invoke_sse_idb_bridging_capable(idb->hwptr)) {
	    printf("SSE bridging not supported on this interface.");
	    return;
	  }
    }

    /*
     * Currently SSE Bridging supported only for Bridge Groups 1 - 9.
     * Therefore do not allow configuration for higher Group numbers.
     */
    if (span->spanindex >= MAX_SSE_BRIDGE_GROUPS) {
        printf("\n\nSilicon Switch Bridging supported only for "
	       "Bridge Groups 1 - 9.\n");
	return;
    }
    
    if (idb->hwptr->sse_bridge_forwarding != csb->sense) {
        if (csb->sense) {
	    span->sse_interfaces++;
	    idb->hwptr->sse_bridge_forwarding = TRUE;
	    delete_fair_queue(idb->hwptr);
	    reg_invoke_srt_config_check(idb, span);
	} else {
	    span->sse_interfaces--;
	    reg_invoke_auto_sw_cache_version(LINK_BRIDGE);
	    idb->hwptr->sse_bridge_forwarding = FALSE;
      }
    }
}

/*
 * bridge_sse_dispatch
 * Return TRUE if something is seriously wrong and trees need to be
 * recalculated quickly.
 */

static boolean bridge_sse_dispatch (sse_protoinfo *protoinfo)
{
    sse_tbinfo *tbinfo = protoinfo->tbinfo;

    if (!tbinfo)
	return(FALSE);
    return(tbinfo->rp_version != tbinfo->bce_version);
}

/*
 * bridge_sse_idb_ok
 * Return true if it's ok to do SSE bridging on this idb.
 */

static inline boolean bridge_sse_idb_ok (ssetype *sse, idbtype *idb)
{
    mcitype *cbus;

    if (!idb ||	!idb->hwptr->sse_bridge_forwarding ||
	!interface_up(idb))
	return(FALSE);
    if (sse->real) {
	cbus = idb->hwptr->devctl;
	if (cbus->sseptr != sse)
	    return(FALSE);
	if (idb->span_address_in ||
	    idb->span_address_out ||
	    idb->span_pattern_in ||
	    idb->span_pattern_out ||
	    idb->span_flag_in ||
	    idb->span_flag_out)
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * bridge_sse_update
 * Update the bridging trees.
 */

static void bridge_sse_update (ssetype *sse, sse_protoinfo *protoinfo)
{
    int index, i, retval, version, j;
    spantype *span;
    btetype *mac;
    idbtype *idb;
    spintype *oldether, *oldfddi;
    ssestattype *stat;
    shadowwordtype sw;
    treeaddrtype addr;
    treecmptype value;
    sse_tbinfo *tbinfo;
    mcitype *cbus;
    sys_timestamp temp_timer;

    oldether = NULL;
    oldfddi = NULL;
    tbinfo = protoinfo->tbinfo;
    if (!tbinfo)
	return;
    version = tbinfo->bce_version;
    if (tbinfo->setup || tbinfo->reset)
	sse_bridging_setup(sse);
    for (index = 0; index < MAX_SSE_BRIDGE_GROUPS; index++) {
	span = spanarray[index];
	if (!span)
	    continue;

	/*
	 * Save the old trees for later deallocation.
	 */
	oldfddi = tbinfo->fddi_bridge_tree[index];
	oldether = tbinfo->ether_bridge_tree[index];
	stat = &tbinfo->bridge_stats[index];
	tbinfo->fddi_bridge_tree[index] = NULL;
	tbinfo->ether_bridge_tree[index] = NULL;

	/*
	 * First install the addresses for our own stations.
	 */
	for (j = 0; j < span->port_count; j++) {
	    idb = span->port[j];
	    if (!interface_up(idb))
		continue;
	    if (sse->real) {
		cbus = idb->hwptr->devctl;
		if (cbus->sseptr != sse)
		    continue;
	    }
	    if (sse_debug)
		buginf("\nSSE: adding %s %e to bridge group %d",
		       idb->namestring, idb->hwptr->hardware, index);
	    if (!bridge_sse_add(sse, protoinfo, idb->hwptr->hardware,
				0, index, idb, TRUE))
		goto done;
	}
	/*
	 * Suspend now, in case that took a while
	 */				
	process_may_suspend();			
	
	/*
	 * Walk through the list of stations for this bridge group.  Build
	 * trees for Ethernet and for FDDI interfaces that have SSE
	 * bridging enabled.
	 */
	for (i = 0; i < NETHASHLEN; i++) {
	    
	    /*
	     * Suspend here, while we hold no pointers
	     */
	    process_may_suspend();		
	    mac = ((btetype *) span->bridge_tbl[i]);
	    for (; mac; mac = mac->bte_next) {
		idb = mac->bte_interface;
		if (!bridge_sse_idb_ok(sse, idb))
		    continue;
		if (sse->real) {
		    if (mac->bte_action != BRIDGE_FORWARD)
			continue;
		    if (!mac->bte_bce_addr) {
			reg_invoke_srt_bce_add(span, mac, idb->hwptr);
			if (!mac->bte_bce_addr) 
			    continue;
		    }

		    /*
		     * Collect the SP-maintained statistics on demand
		     * when a BTE appears to have expired.  Reevaluate
		     * the age of the entry afterwards.
		     */
		    if (mac->bte_bce_addr &&
			!(mac->bte_attributes & BTE_STATIC)) {
			temp_timer = TIMER_SUB_DELTA(mac->bte_timer, ONEMIN);
			if (CLOCK_OUTSIDE_INTERVAL(temp_timer,
			      span->bridge_aging_time * ONESEC)) {
			    reg_invoke_srt_bce_statistics(mac);
			    if ((mac->bte_prev_rxcount != 
				 (mac->bte_rxcount + mac->bte_filtered_rxcount)) &&
				(mac->bte_interface) &&
				(mac->bte_interface->port_state > PORT_LISTENING)) { 
				GET_TIMESTAMP(mac->bte_timer);
				mac->bte_prev_rxcount = (mac->bte_rxcount +
							 mac->bte_filtered_rxcount);
			    }
			}
		    }
		    
		    if (!(mac->bte_attributes & BTE_STATIC)) {
			temp_timer = TIMER_SUB_DELTA(mac->bte_timer, ONEMIN);
			if (CLOCK_OUTSIDE_INTERVAL(temp_timer,
			      span->bridge_aging_time * ONESEC)) {
			    continue;
			}
		    }
		}
		if (!bridge_sse_add(sse, protoinfo, (uchar *)
				    &mac->bte_mac_addr.sword[0],
				    mac->bte_bce_addr, index, idb,
				    FALSE))
		    goto done;
	    }
	}

    done:
	/*
	 * Clean up the trees just generated.  Install them.  First do the
	 * Ethernet tree.
	 */
	process_may_suspend();
	retval = sse_tree_walk(sse, tbinfo->ether_bridge_tree[index], 
			       sse_address_node, NULL, stat, NULL);
	if (OPCODE_ABORT == retval)
	    sse_tree_walk(sse, tbinfo->ether_bridge_tree[index], sse_gc_tree,
			  sse_free_recall, stat, NULL);		      
	process_may_suspend();
	bridge_cache_address = sse_ether_cache[index];
	sse_tree_walk(sse, tbinfo->ether_bridge_tree[index], bridge_sse_gen,
		      NULL, stat, NULL);

	/*
	 * Now do the FDDI tree.
	 */
	process_may_suspend();
	retval = sse_tree_walk(sse, tbinfo->fddi_bridge_tree[index], 
			       sse_address_node, NULL, stat, NULL);
	if (OPCODE_ABORT == retval)
	    sse_tree_walk(sse, tbinfo->fddi_bridge_tree[index], sse_gc_tree,
			  sse_free_recall, stat, NULL);		      
	process_may_suspend();
	bridge_cache_address = sse_fddi_cache[index];
	sse_tree_walk(sse, tbinfo->fddi_bridge_tree[index], bridge_sse_gen,
		      NULL, stat, NULL);
	
	/*
	 * Last chance to say no: if the version number has changed, simply
	 * make sure that bridging is disabled.  The check below will
	 * not enable it and we'll come back around and recalculate a tree
	 * later.  Note that the Truly Clever Hack would be to clean up
	 * this mess and recalculate now.  It's not clear that the
	 * complexity is warranted.
	 */

	if (version != tbinfo->bce_version) 
	    sse_bridge_off();

	/*
	 * Backpatch the trees into place.  Cool hack: we're actually
	 * promoting the comparison value of the tree into the backpatch
	 * instruction. 
	 */
	if (!sse_ether_cache[index]) {
	    errmsg(&msgsym(SWFAILURE, SSE), "broken ether backpatch", index);
	    return;
	}
	sse_peek_treemem(sse, sse_ether_cache[index], &sw);
	if (tbinfo->ether_bridge_tree[index]) {
	    addr = tbinfo->ether_bridge_tree[index]->physaddr;
	    sw.lt.ptr = addr;
	    sw.eq.ptr = addr;
	    sw.gt.ptr = addr;
	    value = tbinfo->ether_bridge_tree[index]->value;
	    sw.lt.value = value;
	    sw.eq.value = value;
	    sw.gt.value = value;
	} else {
	    sw.lt.ptr = SSE_GLOBAL_TBSKIP5;
	    sw.eq.ptr = SSE_GLOBAL_TBSKIP5;
	    sw.gt.ptr = SSE_GLOBAL_TBSKIP5;
	}
	sse_poke_treemem(sse, sse_ether_cache[index], &sw);

	if (!sse_fddi_cache[index]) {
	    errmsg(&msgsym(SWFAILURE, SSE), "broken fddi backpatch", index);
	    return;
	}
	sse_peek_treemem(sse, sse_fddi_cache[index], &sw);
	if (tbinfo->fddi_bridge_tree[index]) {
	    addr = tbinfo->fddi_bridge_tree[index]->physaddr;
	    sw.lt.ptr = addr;
	    sw.eq.ptr = addr;
	    sw.gt.ptr = addr;
	    value = tbinfo->fddi_bridge_tree[index]->value;
	    sw.lt.value = value;
	    sw.eq.value = value;
	    sw.gt.value = value;
	} else {
	    sw.lt.ptr = SSE_GLOBAL_TBSKIP5;
	    sw.eq.ptr = SSE_GLOBAL_TBSKIP5;
	    sw.gt.ptr = SSE_GLOBAL_TBSKIP5;
	}
	sse_poke_treemem(sse, sse_fddi_cache[index], &sw);

	/*
	 * Free the old trees.
	 */
	process_may_suspend();
	sse_free_tree(sse, oldether, stat);
	sse_free_tree(sse, oldfddi, stat);
	oldether = NULL;
	oldfddi = NULL;
    }

    /*
     * This is actually here so that it's easier to determine which half of
     * the process is taking too long.
     */
    process_may_suspend();

    /*
     * If the version number is the same as the version number when we
     * started, then no one has futzed with the bridge cache, so all of the
     * BCE pointers that we just kicked out are valid.  Turn this on as
     * soon as possible.
     */
    if (version == tbinfo->bce_version) 
	sse_bridge_on(sse);

    /*
     * Build or free the group trees.
     */
    for (index = 0; index < MAX_SSE_BRIDGE_GROUPS; index++) {

	/*
	 * Suspend now, while we have no pointers.
	 */
	process_may_suspend();

	span = spanarray[index];
	if (!span)
	    continue;

	stat = &tbinfo->bridge_stats[index];
	if (!span) {
	    sse_free_tree(sse, tbinfo->ether_bridge_tree[index], stat);
	    sse_free_tree(sse, tbinfo->fddi_bridge_tree[index], stat);
	    tbinfo->fddi_bridge_tree[index] = NULL;
	    tbinfo->ether_bridge_tree[index] = NULL;
	    sse_free_tree(sse, tbinfo->fddi_group_tree[index], stat);
	    tbinfo->fddi_group_tree[index] = NULL;
	    sse_free_tree(sse, tbinfo->ether_group_tree[index], stat);
	    tbinfo->ether_group_tree[index] = NULL;
	    continue;
	}

	if ((tbinfo->ether_bridge_tree[index] &&
	    !tbinfo->ether_group_tree[index]) ||
	    tbinfo->reset)
	    bridge_group_tree(sse, protoinfo, index, SSE_BRIDGE_ETHER);
	else if (!tbinfo->ether_bridge_tree[index] &&
		 tbinfo->ether_group_tree[index]) {
	    sse_free_tree(sse, tbinfo->ether_group_tree[index], stat);
	    tbinfo->ether_group_tree[index] = NULL;
	}
	if ((tbinfo->fddi_bridge_tree[index] &&
	    !tbinfo->fddi_group_tree[index]) ||
	    tbinfo->reset)
	    bridge_group_tree(sse, protoinfo, index, SSE_BRIDGE_FDDI);
	else if (!tbinfo->fddi_bridge_tree[index] &&
		 tbinfo->fddi_group_tree[index]) {
	    sse_free_tree(sse, tbinfo->fddi_group_tree[index], stat);
	    tbinfo->fddi_group_tree[index] = NULL;
	}
    }

    tbinfo->reset = FALSE;
    if (version == tbinfo->bce_version)
	tbinfo->rp_version = tbinfo->bce_version;
}

/*
 * bridge_sse_add
 * Add an entry to the tree for the given group.
 */

static boolean bridge_sse_add (ssetype *sse, sse_protoinfo *protoinfo, uchar
			       *macaddr, int bce_addr, int group, idbtype *idb,
			       boolean self) 
{
    spintype *spin, **pred;
    int bytecount;
    uchar *byte;
    ssestattype *stat;
    sse_tbinfo *tbinfo = protoinfo->tbinfo;

    bytecount = STATIONLEN_IEEE48;
    stat = &tbinfo->bridge_stats[group];
    pred = &tbinfo->ether_bridge_tree[group];
    if (sse->real) {
	if (idb->hwptr->status & IDB_FDDI)
	    pred = &tbinfo->fddi_bridge_tree[group];
	else if (!(idb->hwptr->status & IDB_ETHER))
	    return(TRUE);
    }

    /*
     * Add the address to the tree.
     */
    spin = *pred;
    byte = macaddr;
    while (spin) {
	switch (spin->type) {
	case SPIN_COMPARE:
	    if (spin->value == *byte) {
		bytecount--;
		byte++;
		pred = &spin->EQADDR;
	    } else if (*byte > spin->value) 
		pred = &spin->GTADDR;
	    else if (*byte < spin->value) 
		pred = &spin->LTADDR;
	    spin = *pred;
	    break;
	case SPIN_MATCH:
	    return(TRUE);
	default:
	    if (sse_debug)
		buginf("\nSSE: bridge_sse_add unknown type %d", spin->type);
	    return(FALSE);
	}
    }
    if (!bytecount)
	return(TRUE);
    
    /*
     * If we got here, the address is not in the tree.  Add any remaining
     * bytes to the tree.
     */
    while (bytecount) {
	spin = sse_malloc_spin(0, stat);
	if (!spin) 
	    return(FALSE);
	sse_init_spin(spin, *byte, SSE_INST_NOP, SSE_INST_RD_BYTE,
		      SSE_INST_NOP); 
	if (1 == bytecount)
	    spin->nodeinfo.compare.eq.inst = SSE_INST_NOP;
	spin->depth = bytecount;
	*pred = spin;
	bytecount--;
	byte++;
	pred = &spin->EQADDR;
    }

    /*
     * Add the match node.
     */
    spin = sse_malloc_spin(0, stat);
    if (!spin) 
	return(FALSE);
    spin->type = SPIN_MATCH;
    spin->protoinfo = bce_addr;
    spin->nodeinfo.match.protoinfo2 = self;
    if (self)
	spin->EXTRACOUNT = 3;
    else
	spin->EXTRACOUNT = 10;

    *pred = spin;
    if (sse->real) 
	spin->value = (idb->hwptr->cbus_icb_address >> 1) & 0xff;
    else
	spin->value = 0x45;		/* fake out the simulator */
    return(TRUE);
}

/*
 * bridge_sse_gen
 * Generate code for the bridging tree and install it.
 */

static int bridge_sse_gen (ssetype *sse, spintype *spin, ssestattype *stat,
			   queuetype *queue)
{
    shadowwordtype sw;
    uint addr;

    if (spin->physaddr == SSE_UNMAPPED) {
	errmsg(&msgsym(SWFAILURE, SSE), "tried to enqueue unmapped node",
	       spin);  
	return(OPCODE_ABORT);
    }
    switch (spin->type) {
    case SPIN_COMPARE:
        if (spin->LTADDR) {
   	    sw.lt.ptr = spin->LTADDR->physaddr;
	    sw.lt.value = spin->LTADDR->value;
	} else
            sw.lt.ptr = sse_failaddr[spin->depth];
	sw.lt.inst = spin->nodeinfo.compare.lt.inst;	
        if (spin->EQADDR) {
   	    sw.eq.ptr = spin->EQADDR->physaddr;
	    sw.eq.value = spin->EQADDR->value;
	} else
            sw.eq.ptr = sse_failaddr[spin->depth];
	sw.eq.inst = spin->nodeinfo.compare.eq.inst;	
        if (spin->GTADDR) {
   	    sw.gt.ptr = spin->GTADDR->physaddr;
	    sw.gt.value = spin->GTADDR->value;
	} else
            sw.gt.ptr = sse_failaddr[spin->depth];
	sw.gt.inst = spin->nodeinfo.compare.gt.inst;	
	sse_poke_treemem(sse, spin->physaddr, &sw);
	break;   

    case SPIN_MATCH:
	if (!spin->nodeinfo.match.protoinfo2) {
	    /*
	     * This is not our MAC address.
	     *
	     * Add in additional instructions after matching the address.
	     * Start looking at the destination address
	     * (reloc register 0 == 0)
	     *
	     *phys		<RD_SREG_0,0xff>
	     *0			<NOP>
	     *1	0x1		LD_SREG_0,1  NOP,src  NOP,hang
	     *
	     * For a destination address, load what we know about the
	     * destination.  0x45 here represents the outbound ICB.  
	     * Use relocation registers, read back IN.ICB, and see if it
	     * matches the outbound ICB, if so, flush the packet
	     *
	     *2			<LD_SPAGE,0x80>
	     *3			<RD_SREG_0,0xff>
	     *4			<OUTICB,0x45>
	     *5	0x45   		OUTDST1,8 OUTCLASS,nextpkt,CLASS_FLUSH
	     *					OUTDST1,8
	     *6			<OUTDST2,0x09>
	     *7			<RD_BYTE|LD_SPAGE,tbcache,0>
	     *
	     *
	     *8	src		<OUTSRC1,0x05>
	     *9			<OUTSRC2,return,0x03>
	     */
	    
	    addr = spin->protoinfo; /* Address of BCE */
	    
	    sse_init_sw(&sw, 0xff, SSE_INST_RD_SREG_0, spin->EXTRA[0]);
	    sse_poke_treemem(sse, spin->physaddr, &sw);
	    sse_init_sw(&sw, 1, SSE_INST_NOP, spin->EXTRA[1]);
	    sse_poke_treemem(sse, spin->EXTRA[0], &sw);
	    sse_init_sw(&sw, 1, SSE_INST_NOP, spin->EXTRA[2]);
	    sw.lt.inst = SSE_INST_LD_SREG_0;
	    sw.eq.ptr = spin->EXTRA[8];
	    sw.gt.ptr = SSE_GLOBAL_HANG;
	    sse_poke_treemem(sse, spin->EXTRA[1], &sw);
	    sse_init_sw(&sw, 0x80, SSE_INST_LD_SPAGE, spin->EXTRA[3]);
	    sse_poke_treemem(sse, spin->EXTRA[2], &sw);
	    sse_init_sw(&sw, 0xff, SSE_INST_RD_SREG_0, spin->EXTRA[4]);
	    sse_poke_treemem(sse, spin->EXTRA[3], &sw);
	    sse_init_sw(&sw, spin->value, SSE_INST_OUTICB, spin->EXTRA[5]);
	    sse_poke_treemem(sse, spin->EXTRA[4], &sw);
	    sse_init_sw(&sw, addr & 0xff, SSE_INST_OUTDST1, spin->EXTRA[6]);
	    /*	sw.eq.ptr = SSE_GLOBAL_NEXTPKT;*/
	    sw.eq.ptr = SSE_GLOBAL_RP;	/* XXX microcode bug workaround */
	    sw.eq.inst = SSE_INST_OUTCLASS;
	    sw.eq.value = SSE_CLASS_FLUSH;
	    sse_poke_treemem(sse, spin->EXTRA[5], &sw);
	    sse_init_sw(&sw, (addr >> 8) & 0xff, SSE_INST_OUTDST2,
			spin->EXTRA[7]);
	    sse_poke_treemem(sse, spin->EXTRA[6], &sw);
	    sse_init_sw(&sw, 0, SSE_INST_RD_BYTE|SSE_INST_LD_SPAGE,
			bridge_cache_address);
	    sse_poke_treemem(sse, spin->EXTRA[7], &sw);
	    
	    /*
	     * Generate the code for a source address match.
	     */
	    sse_init_sw(&sw, addr & 0xff, SSE_INST_OUTSRC1, spin->EXTRA[9]); 
	    sse_poke_treemem(sse, spin->EXTRA[8], &sw);
	    sse_init_sw(&sw, (addr >> 8) & 0xff, SSE_INST_OUTSRC2,
			SSE_GLOBAL_RETURN);
	    sse_poke_treemem(sse, spin->EXTRA[9], &sw);
	} else {
	    /*
	     * This is our own MAC address.  Do some funny things.
	     *
	     * Start looking at the destination address
	     * (reloc register 0 == 0)
	     *
	     *phys		<RD_SREG_0,0xff>
	     *0			<NOP>
	     *1	0x1		LD_SREG_0,1 OUTCLASS,return,CLASS_AUTON
	     *				NOP,hang
	     *
	     * If we're the destination, then mark the packet routable.
	     * It's clearly NOT necessary to bridge the packet, so assume
	     * it should be passed up if need be.
	     *
	     *2			<LD_SREG_2,tbskip6,1>
	     *
	     */
	    sse_init_sw(&sw, 0xff, SSE_INST_RD_SREG_0, spin->EXTRA[0]);
	    sse_poke_treemem(sse, spin->physaddr, &sw);
	    sse_init_sw(&sw, 1, SSE_INST_NOP, spin->EXTRA[1]);
	    sse_poke_treemem(sse, spin->EXTRA[0], &sw);
	    sse_init_sw(&sw, 1, SSE_INST_NOP, spin->EXTRA[2]);
	    sw.lt.inst = SSE_INST_LD_SREG_0;
	    sw.eq.inst = SSE_INST_OUTCLASS;
	    sw.eq.value = SSE_CLASS_AUTON;
	    sw.eq.ptr = SSE_GLOBAL_RETURN;
	    sw.gt.ptr = SSE_GLOBAL_HANG;
	    sse_poke_treemem(sse, spin->EXTRA[1], &sw);
	    sse_init_sw(&sw, 1, SSE_INST_LD_SREG_2, SSE_GLOBAL_TBSKIP6);
	    sse_poke_treemem(sse, spin->EXTRA[2], &sw);
	}
	break;
    default:
	break;
    }
    return(OPCODE_WALK);
}

/*
 * bridge_group_tree
 * Build the tree for the bridging group and install it.
 */

static void bridge_group_tree (ssetype *sse, sse_protoinfo *protoinfo,
			       uint index, int media)
{
    spintype *spin;
    ssestattype *stat;
    int retval, inst;
    shadowwordtype sw;
    treeaddrtype addr;
    sse_tbinfo *tbinfo = protoinfo->tbinfo;

    stat = &tbinfo->bridge_stats[index];
    spin = sse_malloc_spin(0, stat);
    if (!spin) 
	return;

    /*
     * Generate this code for the group.
     * If the media is FDDI, stick RD_BYTE into instructions 0 and 1.  This
     * drops the pre-mac address cruft on the floor.
     *
     *phys	<LD_SREG_0,0>			    ; State <- dest
     *0		<OUTCLASS,CLASS_BRIDGE>
     *1		<NOP>				    ; Avoid promotion
     *2  0xff	CALL,tbcache	CALL,tbcache	NOP ; No RD_BYTE on a CALL
     *3 	<RD_BYTE,beth or bfddi>
     */
    spin->type = SPIN_MATCH;
    spin->EXTRACOUNT = 4;

    /*
     * Get addresses.
     */
    retval = sse_tree_walk(sse, spin, sse_address_node, NULL, stat, NULL);
    if (OPCODE_ABORT == retval) {
	sse_free_tree(sse, spin, stat);
	return;
    }

    /*
     * Generate the instructions.
     */
    sse_init_sw(&sw, 0, SSE_INST_LD_SREG_0, spin->EXTRA[0]);
    sse_poke_treemem(sse, spin->physaddr, &sw);
    inst = SSE_INST_OUTCLASS;
    if (SSE_BRIDGE_FDDI == media)
	inst |= SSE_INST_RD_BYTE;
    sse_init_sw(&sw, SSE_CLASS_BRIDGE, inst, spin->EXTRA[1]);
    sse_poke_treemem(sse, spin->EXTRA[0], &sw);
    if (SSE_BRIDGE_FDDI == media)
	inst = SSE_INST_RD_BYTE;
    else
	inst = SSE_INST_NOP;
    sse_init_sw(&sw, 0xff, inst, spin->EXTRA[2]);
    sse_poke_treemem(sse, spin->EXTRA[1], &sw);
    sse_init_sw(&sw, 0x80, SSE_INST_CALL, spin->EXTRA[3]);
    if (SSE_BRIDGE_FDDI == media)
	addr = sse_fddi_cache[index];
    else				/* Ether */
	addr = sse_ether_cache[index];
    sw.lt.ptr = addr;
    sw.eq.ptr = addr;
    sw.gt.inst = SSE_INST_NOP;
    sse_poke_treemem(sse, spin->EXTRA[2], &sw);
    sse_init_sw(&sw, 0, SSE_INST_RD_BYTE, SSE_GLOBAL_BETH);
    if (SSE_BRIDGE_FDDI == media) {
	sw.lt.ptr = SSE_GLOBAL_BFDDI;
	sw.eq.ptr = SSE_GLOBAL_BFDDI;
	sw.gt.ptr = SSE_GLOBAL_BFDDI;
    }
    sse_poke_treemem(sse, spin->EXTRA[3], &sw);

    /*
     * Free the old tree and install this one in it's place.
     */
    if (media == SSE_BRIDGE_FDDI) {
	sse_peek_treemem(sse, sse_group_fddi_backpatches[index], &sw);
	sw.lt.ptr = spin->physaddr;
	sw.eq.ptr = spin->physaddr;
	sw.gt.ptr = spin->physaddr;
	sse_poke_treemem(sse, sse_group_fddi_backpatches[index], &sw);
	if (sse_debug)
	    buginf("\nSSE: group %d fddi group tree at %#x from %#x",
		   index, spin->physaddr,
		   sse_group_fddi_backpatches[index]);
	sse_free_tree(sse, tbinfo->fddi_group_tree[index], stat);
	tbinfo->fddi_group_tree[index] = spin;
    } else {
	sse_peek_treemem(sse, sse_group_ether_backpatches[index], &sw);
	sw.lt.ptr = spin->physaddr;
	sw.eq.ptr = spin->physaddr;
	sw.gt.ptr = spin->physaddr;
	sse_poke_treemem(sse, sse_group_ether_backpatches[index], &sw);
	if (sse_debug)
	    buginf("\nSSE: group %d ether group tree at %#x from %#x",
		   index, spin->physaddr,
		   sse_group_ether_backpatches[index]);
	sse_free_tree(sse, tbinfo->ether_group_tree[index], stat);
	tbinfo->ether_group_tree[index] = spin;
    }
}

/*
 * sse_bridge_on
 * Enable bridging.
 */

static void sse_bridge_on (ssetype *sse)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;

    if (!sse->real)
	return;
    cbus = sse->cbus;
    if (!cbus)
	return;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = FCI_BRIDGE_ON;
    regptr->cmdreg = FCI_SSE_BRIDGE_CONTROL;
    response = regptr->cmdreg;
    reset_interrupt_level(level);

    if (response != MCI_RSP_OKAY)
	errmsg(&msgsym(HWFAILURE, SSE), "sse_bridge_on", response);
}

/*
 * sse_bridge_off
 * Disable bridging quickly so that memd isn't corrupted
 */

static void sse_bridge_off (void)
{
    int response;
    mcitype *cbus;
    ssetype *sse;
    sse_tbinfo *tbinfo;
    mciregtype *regptr;
    leveltype level;
    
    if (sse_debug)
	buginf("\nSSE: bridging disabled");

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;

	if (!sse->cbus)
	    continue;

	regptr = cbus->regptr;
	
	level = raise_interrupt_level(NETS_DISABLE);
	regptr->argreg = FCI_BRIDGE_OFF;
	regptr->cmdreg = FCI_SSE_BRIDGE_CONTROL;
	response = regptr->cmdreg;
        reset_interrupt_level(level);
	if (response != MCI_RSP_OKAY)
	    errmsg(&msgsym(HWFAILURE, SSE), "sse_bridge_off", response);

	tbinfo = sse->protoinfo.tbinfo;
	if (!tbinfo)
	    continue;

	tbinfo->bce_version++;
    }
}


/*
 *			Protocol backpatch routines
 */


/*
 * sse_ip_routing
 * Enable or disable IP routing.  Patch the IP tree appropriately.
 */

static void sse_ip_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;

    sse_peek_treemem(sse, SSE_GLOBAL_IPPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else 
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_IPPATCH, &sw);
}

/*
 * sse_ipx_routing
 * Enable or disable IPX routing.  Patch the IPX tree appropriately.
 */

static void sse_ipx_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;

    sse_peek_treemem(sse, SSE_GLOBAL_IPXPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else 
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_IPXPATCH, &sw);
}

/*
 * sse_clnp_routing
 * Enable or disable CLNP routing.  Patch the CLNP tree appropriately.
 */

static void sse_clnp_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;

    sse_peek_treemem(sse, SSE_GLOBAL_CLNPPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_CLNPPATCH, &sw);
}

/*
 * sse_xns_routing
 * Enable or disable XNS routing.  Patch the XNS backpatch point.
 */

static void sse_xns_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;

    sse_peek_treemem(sse, SSE_GLOBAL_XNSPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_XNSPATCH, &sw);
}

/*
 * sse_atalk_routing
 * Enable or disable ATALK routing.  Patch the ATALK backpatch point.
 */

static void sse_atalk_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;
    
    sse_peek_treemem(sse, SSE_GLOBAL_ATPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_ATPATCH, &sw);
}

/*
 * sse_apollo_routing
 * Enable or disable APOLLO routing.  Patch the APOLLO backpatch point.
 */

static void sse_apollo_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;
    
    sse_peek_treemem(sse, SSE_GLOBAL_APLPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_APLPATCH, &sw);
}

/*
 * sse_vines_routing
 * Enable or disable VINES routing.  Patch the VINES backpatch point.
 */

static void sse_vines_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;
    
    sse_peek_treemem(sse, SSE_GLOBAL_VINPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_VINPATCH, &sw);
}

/*
 * sse_decnet_routing
 * Enable or disable DECNET routing.  Patch the DECNET backpatch point.
 */

static void sse_decnet_routing (ssetype *sse, boolean on)
{
    shadowwordtype sw;
    
    sse_peek_treemem(sse, SSE_GLOBAL_DNPATCH, &sw);
    if (on) 
	sw.lt.ptr = SSE_GLOBAL_RP;
    else
	sw.lt.ptr = SSE_GLOBAL_NEXTPKT;
    sse_poke_treemem(sse, SSE_GLOBAL_DNPATCH, &sw);
}

/*
 * sse_bridging_setup
 * Backpatch routing trees appopriately.
 */

static void sse_bridging_setup (ssetype *sse)
{
    idbtype	*swidb;
    boolean	rte_ap, rte_at, rte_cl, rte_dn,
    		rte_ip, rte_nv, rte_vn, rte_xn;

    if (!sse)
	return;

    rte_ap = rte_at = rte_cl = rte_dn = \
	rte_ip = rte_nv = rte_vn = rte_xn = FALSE;

    FOR_ALL_SWIDBS(swidb) {
	if (!rte_ap)
	    rte_ap = ROUTING_IF(swidb, LINK_APOLLO);
	if (!rte_at)
	    rte_at = ROUTING_IF(swidb, LINK_APPLETALK);
	if (!rte_cl)
	    rte_cl = ROUTING_IF(swidb, LINK_CLNS);
	if (!rte_dn)
	    rte_dn = ROUTING_IF(swidb, LINK_DECNET);
	if (!rte_ip)
	    rte_ip = ROUTING_IF(swidb, LINK_IP);
	if (!rte_nv)
	    rte_nv = ROUTING_IF(swidb, LINK_NOVELL);
	if (!rte_vn)
	    rte_vn = ROUTING_IF(swidb, LINK_VINES);
	if (!rte_xn)
	    rte_xn = ROUTING_IF(swidb, LINK_XNS);
    }

    sse_apollo_routing(sse, rte_ap);
    sse_atalk_routing(sse, rte_at);
    sse_clnp_routing(sse, rte_cl);
    sse_decnet_routing(sse, rte_dn);
    sse_ip_routing(sse, rte_ip);
    sse_ipx_routing(sse, rte_nv);
    sse_vines_routing(sse, rte_vn);
    sse_xns_routing(sse, rte_xn);
}

/*
 * sse_bridging_setup_enqueue
 * Record a request for the manager to call sse_bridging_setup.
 */

static void sse_bridging_setup_enqueue (void)
{
    mcitype *cbus;
    ssetype *sse;
    sse_tbinfo *tbinfo;

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	tbinfo = sse->protoinfo.tbinfo;
	if (!tbinfo)
	    return;
	
	tbinfo->setup = TRUE;
	tbinfo->rp_version = tbinfo->bce_version - 1;
    }
}

/*
 * bridge_sse_init
 * Initialize sse information for bridging.
 */

static void bridge_sse_init (sse_protoinfo *protoinfo)
{
    sse_tbinfo *tbinfo;

    tbinfo = malloc(sizeof(sse_tbinfo));
    if (tbinfo) 
	protoinfo->tbinfo = tbinfo;
    tbinfo->reset = TRUE;
}

/*
 * bridge_sse_reinit
 * Clean up all of the information that we've stored.  Something is wildly
 * broken.
 */

static void bridge_sse_reinit (ssetype *sse, sse_protoinfo *protoinfo)
{
    int index;
    sse_tbinfo *tbinfo;
    spantype *span;
    ssestattype *stat;
    
    sse_bridging_setup(sse);
    tbinfo = protoinfo->tbinfo;
    for (index = 0; index < MAX_SSE_BRIDGE_GROUPS; index++) {
	span = spanarray[index];

	stat = &tbinfo->bridge_stats[index];
	
	sse_free_tree(sse, tbinfo->ether_bridge_tree[index], stat);
	sse_free_tree(sse, tbinfo->fddi_bridge_tree[index], stat);
	tbinfo->fddi_bridge_tree[index] = NULL;
	tbinfo->ether_bridge_tree[index] = NULL;
	sse_free_tree(sse, tbinfo->fddi_group_tree[index], stat);
	tbinfo->fddi_group_tree[index] = NULL;
	sse_free_tree(sse, tbinfo->ether_group_tree[index], stat);
	tbinfo->ether_group_tree[index] = NULL;
    }
}

/*
 * show_sse_bridge_tree
 * Show a single tree
 */
static void show_sse_bridge_tree (ssetype *sse, spintype *spin, char *title,
				  int index)
{
    if (!spin)
	return;
    automore_enable(NULL);
    printf("\nBridge group %d %s\n", index, title);
    automore_header(show_sse_internal_header);
    sse_safe_tree_walk(sse, spin, sse_show_spin, NULL,
		       (ssestattype *)BASE_HEX, NULL); 
    automore_disable();
}

/*
 * show_sse_bridge
 * Show the bridge tree for the specified group.  0 means all groups.
 */

void show_sse_bridge (parseinfo *csb)
{
    int index;
    ssetype *sse;
    mcitype *cbus;
    int group;
    sse_tbinfo *tbinfo;

    group = GETOBJ(int,1);                  /* 0 means all groups */
    sse = NULL;				/* stupid compiler */
    for (index = 1; index < MAX_SSE_BRIDGE_GROUPS; index++) {
	if ((group) && (group != index))
	    continue;
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    sse = cbus->sseptr;
	    if (!sse)
		continue;
	    tbinfo = sse->protoinfo.tbinfo;
	    if (!tbinfo)
		continue;
	    show_sse_bridge_tree(sse, tbinfo->ether_bridge_tree[index],
				 "Ethernet", index);
	    show_sse_bridge_tree(sse, tbinfo->fddi_bridge_tree[index], "FDDI",
				 index); 
	}
    }
}

/*
 * show_sse_bridge_stats
 * Display the statistics for each bridge group for this sse.
 */

static void show_sse_bridge_stats (sse_protoinfo *protoinfo)
{
    int index;
    spantype *span;
    sse_tbinfo *tbinfo = protoinfo->tbinfo;

    for (index = 0; index < MAX_SSE_BRIDGE_GROUPS; index++) {
	span = spanarray[index];
	if (!span)
	    continue;

 	if (tbinfo->ether_bridge_tree[index] ||
	    tbinfo->ether_group_tree[index] ||
	    tbinfo->fddi_bridge_tree[index] ||
	    tbinfo->fddi_group_tree[index]) {
	    printf("\nBridge group %2d", index);
	    printf(SSE_STAT_STRING SSE_INTERN_STRING,
		   tbinfo->bridge_stats[index].words_used, 
		   tbinfo->bridge_stats[index].rwbytes_used,
		   tbinfo->bridge_stats[index].spin_used);
	}
    }
}

/*
 * bridge_sse_groupaddr
 * Return the backpatch location for a bridge group.
 */

static boolean bridge_sse_groupaddr (hwidbtype *hwidb, int *result)
{
    int index;

    index = hwidb->firstsw->span_index;

    /*
     * On interfaces configured for bridge groups we can't handle,
     * send all packets up to the RP.
     */

    if (hwidb->status & IDB_FDDI) {
	if (index >= MAX_SSE_BRIDGE_GROUPS)
	    *result = SSE_GLOBAL_RP;
	else
	    *result = sse_group_fddi_backpatches[index];
	return(TRUE);
    } else if (hwidb->status & IDB_ETHER) {
	if (index >= MAX_SSE_BRIDGE_GROUPS)
	    *result = SSE_GLOBAL_RP;
	else
	    *result = sse_group_ether_backpatches[index];
	return(TRUE);
    }
    return(FALSE);
}

/*
 * bridge_clear_sse
 * Force us to recalculate bridging SSE tables.
 */

static void bridge_clear_sse (sse_protoinfo *protoinfo)
{
    protoinfo->tbinfo->rp_version = protoinfo->tbinfo->bce_version - 1;
    protoinfo->tbinfo->reset = TRUE;
}

/*
 * tbridge_sse_register_thyself
 * Register this subsystem.
 */

static void tbridge_sse_register_thyself (void)
{
    reg_add_sse_update(bridge_sse_update, "bridge_sse_update");
    reg_add_sse_fast_dispatch(bridge_sse_dispatch, "bridge_sse_dispatch");
    reg_add_sse_data_init(bridge_sse_init, "bridge_sse_init");
    reg_add_sse_reinit(bridge_sse_reinit, "bridge_sse_reinit");
    reg_add_sse_protocols_invalid(bridge_clear_sse, "bridge_clear_sse");
    reg_add_sse_idb_invalid(bridge_clear_sse, "bridge_clear_sse");
    reg_add_sse_icbtree_branch(bridge_sse_groupaddr, "bridge_sse_groupaddr");
    reg_add_sse_print_stats(show_sse_bridge_stats, "show_sse_bridge_stats");
    reg_add_bridge_setup(sse_bridging_setup_enqueue,
			 "sse_bridging_setup_enqueue");
    reg_add_auto_sw_cache_version(LINK_BRIDGE, sse_bridge_off,
				  "sse_bridge_off");
    bridge_sse_chain_init();
}

/*
 * tbridge_sse_init
 * Initialize SSE transparent bridging.
 */
static void tbridge_sse_init (subsystype *subsys)
{
    tbridge_sse_register_thyself();
}

/* 
 * tbridge_sse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(tbridge_sse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      tbridge_sse_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: tbridge, iphost, ipx, xns, atalk, apollo, clns, "
	      "vines, decnet", 
	      "req: sse, tbridge");

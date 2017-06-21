/* $Id: srb_sse.c,v 3.4.4.5 1996/08/28 12:45:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/srb_sse.c,v $
 *------------------------------------------------------------------
 * srb_sse.c -- SSE Source Route Bridge Support
 *
 * Jan 1993     Tony Li, Percy P. Khabardar, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb_sse.c,v $
 * Revision 3.4.4.5  1996/08/28  12:45:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.4.4  1996/08/26  15:07:32  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/07/09  05:51:32  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.4.4.2  1996/05/17  11:19:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.8  1996/04/29  17:28:41  ppearce
 * IBU modularity: missed file during SRB subblock commit
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.7  1996/04/26  15:19:07  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.6  1996/04/26  07:53:41  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.14.5  1996/04/03  14:32:15  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.14.4  1996/03/20  17:54:03  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.3.14.3  1996/03/19  02:04:21  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.3.14.2  1996/03/17  17:49:51  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.3.14.1  1996/03/16  12:09:10  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.4.4.1  1996/03/18  19:43:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:51:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/13  01:22:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.1  1996/01/24  21:48:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/03/07  06:33:58  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.3  1996/01/18  22:47:32  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  09:20:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/06/28  09:25:09  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/21  06:41:53  gchristy
 * CSCdi35154:  SSE: SRB can sometimes fast-switch
 *   - don't assume bridge numbers are symmetric
 *   - only skip RCF bytes on a failure
 *   - restart RCF parsing with node immediately following ICB compare
 *   - allow SSE switching when AST is configured
 *
 * Revision 2.3  1995/06/18  23:17:43  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:04:50  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:44:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "subsys.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "../hes/if_mci.h"
#include "../util/range.h"
#include "../ibm/netbios_sb.h"
#include "../srt/span.h"
#include "../srt/rsrb.h"
#include "../srt/srt_registry.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../srt/netbios_srb.h"

#include "../parser/parser_defs_sse.h"
#include "../hes/sse.h"
#include "../hes/sse_private.h"
#include "../hes/sse_registry.h"
#include "srb_sse_private.h"
#include "../hes/ssesym.h"

static const treeaddrtype srb_failaddr[STATIONLEN_IEEE48+1] = {
    SSE_GLOBAL_TRSRCMAC,		/* skip 0 */
    SSE_GLOBAL_TRSRCMAC,		/* skip 0 */
    SSE_GLOBAL_TRSKIP1,			/* skip 1 */
    SSE_GLOBAL_TRSKIP2,			/* skip 2 */
    SSE_GLOBAL_TRSKIP3,			/* skip 3 */
    SSE_GLOBAL_TRSKIP4,			/* skip 4 */
    SSE_GLOBAL_TRSKIP5			/* skip 5 */
    };

static void srb_sse_compile(ssetype *sse);
static int srb_sse_code_gen(ssetype *sse, spintype *spin, ssestattype *stat,
			    queuetype *queue); 
static boolean srb_mac_add(ssetype *sse, spintype **macaddrtree,
			   char *macaddr);
static void srb_mac_compile(ssetype *sse, sse_srbinfo *srb, spintype
			    *macaddrtree, boolean mactreeok);
static boolean srb_sse_configed(hwidbtype *idb);



/*
 * srb_sse_dispatch
 * Return TRUE if srb trees need to be recalculated.  FALSE otherwise.
 * NOTE - a config change on ANY CTR port will cause a recalculation 
 * of srb trees on EVERY CTR port.
 * sse_srbupdate_needed is set to TRUE when there is a change in the srb 
 * configuration.  It is set to FALSE in srb_sse_update() after all the
 * interface trees have been calculated.
 */

static boolean srb_sse_dispatch (sse_protoinfo *protoinfo)
{
   return(protoinfo->srbinfo->update_needed);
}

/*
 * srb_sse_form_rifstring
 * Concatenates the input icb, ring and bridge numbers to make it look like
 * routing descriptors (in a rif) in a packet. 
 * Each routing descriptor is 2 bytes long - 12 bits are allocated
 * for the ring number and 4 bits for the bridge number.
 * The length of the rif match string will be either five bytes
 * (dual-port) or seven bytes (multi-port/virtual-ring).  The rif string
 * will be written to a static internal buffer and a pointer returned, so
 * this routine is NOT REENTRANT.
 *
 * iring   - input ring-number 
 * ibridge - input bridge-number 
 * vring   - virtual ring-number if configured.  Non-zero for multi-port
 *           bridging, zero for dual-port bridging.
 * oring   - output ring-number
 * obridge - output bridge-number
 * idb     - input interface 
 */

static uchar *srb_sse_form_rifstring (ushort iring, ushort ibridge,
				      ushort vring, ushort oring,
				      ushort obridge, hwidbtype *idb)
{
    ushort *rifptr;
    uchar *start;
    static uchar rif[SRB_SSE_MAXRIF+1];	/* +1 for input ICB */

    /*
     * Start the RIF string with the input ICB.  This is the first byte in
     * the comparison.
     */
    start = rif;
    *start++ = (idb->cbus_icb_address >> 1) & 0xff; /* input ICB */ 

    /*
     * Now add a word that is the input routing descriptor (i.e.,
     * ring/bridge pair).
     */
    rifptr = (ushort *)start;
    *rifptr++ = (iring << 4) | ibridge;	/* input ring-bridge */

    /*
     * If a virtual ring is in use, then add it to the RIF string.
     */
    if (vring)				/* multi-port bridge */
	*rifptr++ = (vring << 4) | obridge; /* virtual ring-bridge */

    /*
     * Finally, add the output ring number.
     */
    *rifptr++ = (oring << 4);            /* output ring, br = don't care */

    if (sse_debug) {
	buginf("\nSSE: %s rif string -> ", idb->firstsw->namestring);
	for (start = rif; start < (uchar *) rifptr; start++)
	    buginf("%02x ", *start);
    }
    return(rif);
}

/*
 * srb_sse_update
 * Update the srb tree.
 * The rif tree will contain both forward and backward rif's for all
 * possible rif's between the src ring as configured on the interface
 * and output rings as configured on other CTR ports.
 * Rif trees are not created if srb is not configured or access filters are
 * set on interfaces.
 */

static void srb_sse_update (ssetype *sse, sse_protoinfo *protoinfo)
{
    
    hwidbtype *iidb, *oidb;
    srb_triplet_t isrb_triplet;
    srb_triplet_t osrb_triplet;
    ushort src, br, tgt;
    uchar *rif;
    mcitype *cbus;
    boolean ok, mactreeok;
    sse_srbinfo *srb = protoinfo->srbinfo;
    spintype *macaddrtree;
    srb_vector_table_t *srbV_p;

    if (!srb_sse_dispatch(protoinfo))
	return;

    sse_init_tree(sse, SSE_PROTOCOL_SRB);
    macaddrtree = NULL;
    mactreeok = TRUE;

    srbV_p = reg_invoke_srb_get_vector_table();

    FOR_ALL_HWIDBS(iidb) {

	/*
	 * Suspend now, while we have no pointers
	 */
	process_may_suspend();			

	/*
	 * Don't update real interfaces that are not on the cBus, not token
	 * ring, or don't belong to this SSE.
	 */
	if (sse->real) {
	    if (!(iidb->status & IDB_CBUS))
		continue;
	    if (!(iidb->status & IDB_TR))
		continue;
	    if (iidb->status & IDB_NOHARDWARE)
		continue;
	    cbus = iidb->devctl;
	    if (cbus->sseptr != sse)
		continue;
	}	

	/* 
	 * If srb sse switching not configured skip this interface.
	 */
	if (!srb_sse_configed(iidb))
	    continue;

        if (srbV_p) {
            srbV_p->srb_sb_get_triplet(iidb->firstsw, &isrb_triplet);
            src = isrb_triplet.localRing;
            br  = isrb_triplet.bridgeNum;
            tgt = isrb_triplet.remoteRing;
        } else {
            src = SRB_INVALID_RING_NUM;
            br  = SRB_INVALID_BRIDGE_NUM;
            tgt = SRB_INVALID_RING_NUM;
        }
	
	mactreeok = mactreeok && srb_mac_add(sse, &macaddrtree,
					     iidb->hardware);

	FOR_ALL_HWIDBS(oidb) {

	    if (iidb == oidb)
		continue;

	    /*
	     * Don't update real interfaces that are not on the cBus, not
	     * token ring, or don't belong to this SSE.
	     */
	    if (sse->real) {
		if (!(oidb->status & IDB_CBUS))
		    continue;
		if (!(oidb->status & IDB_TR))
		    continue;
		if (oidb->status & IDB_NOHARDWARE)
		    continue;
		cbus = oidb->devctl;
		if (cbus->sseptr != sse)
		    continue;
	    }	
	    
	    /*
	     * If srb sse switching not configured skip this interface.
	     */
	    if (!srb_sse_configed(oidb))
		continue;

	    if (sse_debug)
		buginf("\nSSE: RIF for %s to %s",
		       iidb->firstsw->namestring,
		       oidb->firstsw->namestring);

	    /*
	     * RIF calculations are different for dual and multi-port
	     * bridging - check if system configured for dual or multi-port
	     * bridging. 
	     */
            if (!srbV_p)
		continue;
            
            srbV_p->srb_sb_get_triplet(oidb->firstsw, &osrb_triplet);
	    if (vrfind_ringgroup(tgt)) {
		if ((tgt == osrb_triplet.remoteRing) && 
		    (src != osrb_triplet.localRing) ) {

		    /*
		     * Form fwd rif and add to srb interface tree
		     */
		    rif = srb_sse_form_rifstring(src, br, tgt,
						 osrb_triplet.localRing,
						 osrb_triplet.bridgeNum,
						 iidb);

		    ok = sse_add(sse, SSE_PROTOCOL_SRB, SRB_SSE_MPRIF + 1,
				 rif, rif[SRB_SSE_MPRIF] + 0xf,
				 oidb->firstsw, NULL, 0, 1, 0, 0, 0);
		    if (!ok)
			goto done;
		    
		    /*
		     * Form reverse rif and add to srb interface tree
		     */ 
		    srb_sse_form_rifstring(osrb_triplet.localRing,
					   osrb_triplet.bridgeNum, 
                                           tgt, src, br, iidb); 
		    ok = sse_add(sse, SSE_PROTOCOL_SRB, SRB_SSE_MPRIF + 1,
				 rif, rif[SRB_SSE_MPRIF] + 0xf,
				 oidb->firstsw, NULL, 0, 1, 0, 0, 0);  
		    if (!ok)
			goto done;
		}
	    } else {
		if (osrb_triplet.remoteRing == src) {

		    /*
		     * Form fwd rif and add to srb interface tree
		     */
		    rif = srb_sse_form_rifstring(src, br, 0,
						 osrb_triplet.localRing,
						 osrb_triplet.bridgeNum, iidb);
		    ok = sse_add(sse, SSE_PROTOCOL_SRB, SRB_SSE_DPRIF + 1,
				 rif, rif[SRB_SSE_DPRIF] + 0xf,
				 oidb->firstsw, NULL, 0, 1, 0, 0, 0); 
		    if (!ok)
			goto done;
		    
		    /*
		     * Form reverse rif and add to srb interface tree
		     */
		    rif = srb_sse_form_rifstring(osrb_triplet.localRing,
						 osrb_triplet.bridgeNum,
						 0, src, br, iidb); 
		    ok = sse_add(sse, SSE_PROTOCOL_SRB, SRB_SSE_DPRIF + 1,
				 rif, rif[SRB_SSE_DPRIF] + 0xf,
				 oidb->firstsw, NULL, 0, 1, 0, 0, 0);
		    if (!ok)
			goto done;
		}
	    }
	}
    }
 done:
    srb_sse_compile(sse);
    srb_mac_compile(sse, srb, macaddrtree, mactreeok);
    srb->update_needed = FALSE;
}

/*
 * srb_sse_compile
 * Compile the tree into actual SSE instructions.  Download it and
 * backpatch the running program. 
 */

static void srb_sse_compile (ssetype *sse)
{

    spintype *spin;
    shadowwordtype sw;
    int retval, protocol;
    boolean gc_needed = FALSE;
    ssestattype *stat;

    /*
     * Assign addresses to instructions.
     */
    protocol = SSE_PROTOCOL_SRB;
    stat = &sse->stats[protocol];
    retval = sse_tree_walk(sse, sse_compiletree,
			   sse_address_node, NULL, stat, NULL); 
    if (OPCODE_ABORT == retval) 
	gc_needed = TRUE;

    /*
     * Handle out of memory errors by pruning the tree.
     */
    if (gc_needed) {
       sse_tree_walk(sse, sse_compiletree, sse_gc_tree,
		     sse_free_recall, stat, NULL);
    }

    /*
     * Second pass at the tree.
     */
    retval = sse_tree_walk(sse, sse_compiletree, 
			   srb_sse_code_gen, NULL, stat, NULL);
    if ((retval == OPCODE_ABORT) ||
	(sse_compiletree == NULL)) {

        /*
	 * The new tree is messed up.  Flush it and put the old tree back.
         */
	if (sse_debug)
	    buginf("\nSSE: no program for %s", sse_protocol_name(protocol)); 
	if (sse_compiletree)
	    sse_free_tree(sse, sse_compiletree, stat);
	return;
    }

    /*
     * Backpatch the appropriate jump point into this table.
     */
    sse_peek_treemem(sse, sse_backpatch_table[protocol], &sw);
    spin = sse_compiletree;
    sw.lt.ptr = spin->physaddr;
    sw.lt.value = spin->value;
    sw.eq.ptr = spin->physaddr;
    sw.eq.value = spin->value;
    sw.gt.ptr = spin->physaddr;
    sw.gt.value = spin->value;
    sse_poke_treemem(sse, sse_backpatch_table[protocol], &sw);

    /*
     * Now, free the old tree.
     */
    sse_free_tree(sse, sse->protocol_tree[protocol], stat);
    sse_tree_walk(sse, sse_compiletree, sse_tree_depth, NULL,
		  stat, NULL);
    sse->depth[protocol] = sse_compiletree->depth;
    sse->protocol_tree[protocol] = sse_compiletree;
}

/*
 * srb_sse_code_gen
 * Generate code for a single spin node.
 * XXX Generate additional code to check for the lan out duplication
 */
static int srb_sse_code_gen (ssetype *sse, spintype *spin, ssestattype *stat,
			     queuetype *queue)
{
    shadowwordtype sw;
    spintype *startrif;
    treeaddrtype failaddr;
    int failval, address;
    
    address = spin->physaddr;
    switch (spin->type) {
    case SPIN_MATCH:

	/*
	 * If we failed to match on an odd RIF byte, we have to skip a byte
	 * in the RIF and go back to the start.  Allocate a special word in
	 * EXTRA[0] which just does a RD_BYTE and then jumps to the start
	 * of the RIF.	We want to restart RIF parsing with the node
	 * hanging off the EQ branch of the ICB compare node.
	 */
	startrif = ((spintype *) spin->protoinfo)->EQADDR;
	sse_init_sw(&sw, startrif->value, SSE_INST_RD_BYTE,
		    startrif->physaddr); 
	sse_poke_treemem(sse, spin->EXTRA[0], &sw);

	sse_init_sw(&sw, spin->value, SSE_INST_OUTICB, SSE_GLOBAL_GOODSRB);
	break;
	
    case SPIN_COMPARE:

	/*
	 * First, figure out the address to jump to if we fail to match on
	 * this node.  The rules depend on the bytecount, as stored in the
	 * protoinfo field.  If the bytecount is 0, then this is an ICB compare
	 * node.  Failures here go to SSE_GLOBAL_RP.
	 */
	failaddr = SSE_GLOBAL_RETURN;
	failval = 0;
	if (spin->protoinfo) {

	    /*
	     * Find the match node directly below.
	     */
	    startrif = spin->EQADDR;
	    while (startrif && (SPIN_MATCH != startrif->type)) 
		startrif = startrif->EQADDR;

	    if (startrif) {
		if (spin->protoinfo & 1) {
		    /*
		     * If the bytecount is odd, we failed on the first 
		     * byte of an RCF. The extra address found in
		     * EXTRA[0] on the match node will skip the second
		     * byte of the RCF before restarting RIF parsing.
		     */
		    failaddr = startrif->EXTRA[0];
		} else {
		    /*
		     * If the bytecount is even, we failed on the
		     * second byte of an RCF.  We want to restart RIF
		     * parsing with the node hanging off the
		     * EQ branch of the ICB compare node.
		     */
		    startrif = ((spintype *) startrif->protoinfo)->EQADDR; 
		    failaddr = startrif->physaddr;
		    failval = startrif->value;
		}
	    }
	}
	/*
	 * Now generate the node.
	 */
	sse_init_sw(&sw, failval, SSE_INST_RD_BYTE, failaddr);
	if (spin->LTADDR) {
	    sw.lt.inst  = SSE_INST_NOP;
	    sw.lt.ptr   = spin->LTADDR->physaddr;
	    sw.lt.value = spin->LTADDR->value;
	}	
	if (spin->EQADDR) {
	    if (SPIN_MATCH != spin->EQADDR->type)
		sw.eq.inst  = SSE_INST_RD_BYTE;
	    else
		sw.eq.inst  = SSE_INST_NOP;
	    sw.eq.ptr   = spin->EQADDR->physaddr;
	    sw.eq.value = spin->EQADDR->value;
	}
	if (spin->GTADDR) {
	    sw.gt.inst  = SSE_INST_NOP;
	    sw.gt.ptr   = spin->GTADDR->physaddr;
	    sw.gt.value = spin->GTADDR->value;
	} 
	break;
	
    default:
	errmsg(&msgsym(SWFAILURE, SSE), "invalid spin type", spin);
	break;
    }
    sse_poke_treemem(sse, address, &sw);
    return (OPCODE_WALK);
}

/*
 * srb_mac_add
 * Add a mac address to the tree of our own mac addresses.
 */

static boolean srb_mac_add (ssetype *sse, spintype **macaddrtree,
			    char *macaddr)
{
    spintype *spin, **pred;
    int bytecount;
    uchar *byte;
    ssestattype *stat;

    bytecount = STATIONLEN_IEEE48;
    stat = &sse->stats[SSE_PROTOCOL_SRB];
    pred = macaddrtree;

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
	    if (sse_debug)
		buginf("\nSSE: srb_mac_add duplicate mac addr %e", macaddr);
	    return(FALSE);
	default:
	    if (sse_debug)
		buginf("\nSSE: srb_mac_add unknown type %d", spin->type);
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
	spin = sse_malloc_spin(SSE_PROTOCOL_SRB, stat);
	if (!spin) 
	    return(FALSE);
	sse_init_spin(spin, *byte, SSE_INST_NOP, SSE_INST_RD_BYTE,
		      SSE_INST_NOP); 
	if (1 == bytecount)
	    spin->nodeinfo.compare.eq.inst = SSE_INST_NOP;
	spin->protoinfo = bytecount;
	*pred = spin;
	bytecount--;
	byte++;
	pred = &spin->EQADDR;
    }

    return(TRUE);
}

/*
 * srb_mac_code_gen
 * Generate code for the mac address tree.
 */

static int srb_mac_code_gen (ssetype *sse, spintype *spin, ssestattype *stat,
			     queuetype *queue)
{
    shadowwordtype sw;

    if (spin->physaddr == SSE_UNMAPPED) {
	errmsg(&msgsym(SWFAILURE, SSE), "tried to enqueue unmapped node",
	       spin);
	return(OPCODE_ABORT);
    }

    /*
     * All but the last node have a successor.
     */
    if (spin->LTADDR) {
	sw.lt.ptr = spin->LTADDR->physaddr;
	sw.lt.value = spin->LTADDR->value;
	sw.lt.inst = spin->nodeinfo.compare.lt.inst;	
    } else {
	sw.lt.ptr = srb_failaddr[spin->protoinfo];
	sw.lt.inst = SSE_INST_RD_BYTE;
	/*
	 * Initialize value for RIF check in case we jump directly to
	 * TRSRCMAC.
	 */
	sw.lt.value = 0x80;
    }
    if (spin->GTADDR) {
	sw.gt.ptr = spin->GTADDR->physaddr;
	sw.gt.value = spin->GTADDR->value;
	sw.gt.inst = spin->nodeinfo.compare.gt.inst;	
    } else {
	sw.gt.ptr = srb_failaddr[spin->protoinfo];
	sw.gt.inst = SSE_INST_RD_BYTE;
	sw.gt.value = 0x80;
    }
    if (spin->EQADDR) {
	sw.eq.ptr = spin->EQADDR->physaddr;
	sw.eq.value = spin->EQADDR->value;
	sw.eq.inst = spin->nodeinfo.compare.eq.inst;	
    } else {

	/*
	 * This is the last node in the branch, indicating our own MAC
	 * address.  Generate: 
	 *
	 *	... RD_BYTE|LD_SREG_2,trsrcmac,1 ...
	 *
	 */
	sw.eq.ptr = SSE_GLOBAL_TRSKIP0;
	sw.eq.value = 1;
	sw.eq.inst = SSE_INST_LD_SREG_2|SSE_INST_RD_BYTE;
    }
    sse_poke_treemem(sse, spin->physaddr, &sw);
    return(OPCODE_WALK);
}

/*
 * srb_mac_compile
 * Build the mac address tree
 */

static void srb_mac_compile (ssetype *sse, sse_srbinfo *srb, spintype
			     *macaddrtree, boolean mactreeok)
{
    int retval;
    ssestattype *stat;
    uint addr;
    shadowwordtype sw;
    uchar value;

    stat = &sse->stats[SSE_PROTOCOL_SRB];
    value = 0xff;			/* no broadcast addresses, please */

    if (mactreeok) {
	retval = sse_tree_walk(sse, macaddrtree, sse_address_node, NULL, stat,
			       NULL);  
	if (OPCODE_ABORT == retval) {
	    
	    /*
	     * Handle out of memory errors by pruning the tree.
	     */
	    sse_tree_walk(sse, macaddrtree, sse_gc_tree,
			  sse_free_recall, stat, NULL);
	}
    }

    /*
     * Second pass at the tree for code generation.
     */
    if (mactreeok && macaddrtree && (macaddrtree->physaddr != SSE_UNMAPPED)) {
	sse_tree_walk(sse, macaddrtree, srb_mac_code_gen, NULL, stat, NULL);
	addr = macaddrtree->physaddr;
	value = macaddrtree->value;
    } else {
	sse_free_tree(sse, macaddrtree, stat);
	macaddrtree = NULL;
	addr = SSE_GLOBAL_TRSKIP6;
    }
    sse_peek_treemem(sse, SSE_GLOBAL_TRDESTMAC, &sw);
    sw.lt.ptr = addr;
    sw.eq.ptr = addr;
    sw.gt.ptr = addr;
    sw.lt.value = value;
    sw.eq.value = value;
    sw.gt.value = value;
    sse_poke_treemem(sse, SSE_GLOBAL_TRDESTMAC, &sw);
    if (srb->tr_dest_mac_tree) 
	sse_free_tree(sse, srb->tr_dest_mac_tree, stat); 
    srb->tr_dest_mac_tree = macaddrtree;
}

/*
 * srb_sse_configchange
 * Invoked upon a config change - either srb is configured or srb config
 * is removed.  Same thing for access filters. 
 * Also invoked upon exec command to recalculate trees. 
 */
static void srb_sse_configchange (void)
{
    sse_srbinfo *srb;
    mcitype *cbus;
    ssetype *sse;

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (!cbus->sseptr)
	    continue;
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	srb = sse->protoinfo.srbinfo;
	if (!srb)
	    continue;
	srb->update_needed = TRUE;
	if (sse_debug)
	    buginf("\nSSE: srb update needed");
    }
}

/*
 * srb_sse_configed
 * Returns TRUE if srb sse switching is currently configured and the srb sse
 * route-cache is enabled.  No filters can be configured.
 */

static boolean srb_sse_configed (hwidbtype *idb)
{
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;

    if (!(idb->srb_routecache & SRB_SSE_ROUTECACHE)) {
	return(FALSE);
    }

    /*
     * No SRB if TB enabled, but IBM spanning tree is OK.
     */
    if (idb->firstsw->span_index) {
	spantype *span;

	span = idb->firstsw->span_ptr;
	if (!span || (span->protocol != SPAN_PROTO_IBM))
	    return(FALSE);
    }

    srbV_p = reg_invoke_srb_get_vector_table();
    if (!srbV_p) {
        return(FALSE);
    }
    srbV_p->srb_sb_get_triplet(idb->firstsw, &srb_triplet);
    if (srb_triplet.localRing > SRB_MAX_RING_NUM)
	return(FALSE);
    if (srb_triplet.localRing <= 0)
	return(FALSE);
    if (srb_triplet.bridgeNum > SRB_MAX_BRIDGE_NUM)
	return(FALSE);
    if (srb_triplet.bridgeNum <= 0)
	return(FALSE);
    if (srb_triplet.remoteRing > SRB_MAX_RING_NUM)
	return(FALSE);
    if (srb_triplet.remoteRing <= 0)
	return(FALSE);
    if ((*srb_access_in_p)(idb))
        return(FALSE);
    if ((*srb_access_out_p)(idb))
        return(FALSE);  
    return(TRUE);
}

/*
 * srb_sse_init
 * Initialize sse information for SRB.
 */

static void srb_sse_init (sse_protoinfo *protoinfo)
{
    sse_srbinfo *srb;

    srb = malloc(sizeof(sse_srbinfo));
    if (srb) {
	srb->update_needed = TRUE;
	protoinfo->srbinfo = srb;
    }
}

/*
 * srb_sse_reinit
 * Clean up and go back to the initial state.  Memory is already
 * deallocated by the manager.  Just clean up the backpatches.
 */

static void srb_sse_reinit (ssetype *sse, sse_protoinfo *protoinfo)
{
    shadowwordtype sw;
    ssestattype *stat;
    sse_srbinfo *srb;

    stat = &sse->stats[SSE_PROTOCOL_SRB];
    srb = sse->protoinfo.srbinfo;
    sse_peek_treemem(sse, SSE_GLOBAL_TRDESTMAC, &sw);
    sw.lt.ptr = SSE_GLOBAL_TRSKIP6;
    sw.eq.ptr = SSE_GLOBAL_TRSKIP6;
    sw.gt.ptr = SSE_GLOBAL_TRSKIP6;
    sse_poke_treemem(sse, SSE_GLOBAL_TRDESTMAC, &sw);
    if (!srb)
	return;
    if (srb->tr_dest_mac_tree) {
	sse_free_tree(sse, srb->tr_dest_mac_tree, stat);
	srb->tr_dest_mac_tree = NULL;
    }
}

/*
 * srb_sse_clear
 * Force an SSE calculation.  Pretend it's a configuation change.
 */

static void srb_sse_clear (sse_protoinfo *protoinfo)
{
    srb_sse_configchange();
}

/*
 * srb_sse_srt_configchange
 * Check a configuration change to see if we care.
 */

static void srb_sse_srt_configchange (idbtype *idb, spantype *span)
{
    if (!(idb->hwptr->srb_routecache & SRB_SSE_ROUTECACHE))
	return;

    srb_sse_configchange();
}

/*
 * srb_sse_register_thyself
 * Called to perform registration for this file.
 */

static void srb_sse_register_thyself (subsystype *subsys)
{
    reg_add_sse_update(srb_sse_update, "srb_sse_update");
    reg_add_sse_fast_dispatch(srb_sse_dispatch, "srb_sse_dispatch");
    reg_add_sse_data_init(srb_sse_init, "srb_sse_init");
    reg_add_sse_protocols_invalid(srb_sse_clear, "srb_sse_clear");
    reg_add_sse_idb_invalid(srb_sse_clear, "srb_sse_clear");
    reg_add_srb_setup(srb_sse_configchange, "srb_sse_configchange");
    reg_add_srt_config_check(srb_sse_srt_configchange, "srb_sse_configchange");
    reg_add_sse_reinit(srb_sse_reinit, "srb_sse_reinit");
    srb_sse_chain_init();
}

/* 
 * srb_sse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(srb_sse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      srb_sse_register_thyself, SUBSYS_CLASS_PROTOCOL,
	      "seq: srb",
	      "req: sse, srb");

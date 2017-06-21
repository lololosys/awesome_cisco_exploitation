/* $Id: smrp_fast_les.c,v 3.1.2.2 1996/08/26 19:57:22 kleung Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_fast_les.c,v $
 *------------------------------------------------------------------
 * SMRP fastswitching support for the low-end platforms
 *
 * March 1996, Kent Leung
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_fast_les.c,v $
 * Revision 3.1.2.2  1996/08/26  19:57:22  kleung
 * CSCdi67164:  Need to free packet if not able to forward SMRP packet.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/02  08:44:43  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/04/16  23:03:46  kleung
 * Placeholder for CA commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "media_registry.h"
#include "subsys.h"
#include "packet.h"
#include "smrp_private.h"
#include "smrp_fast.h"

/* 
 * Private forward declarations
 */
static void smrpfast_les_init (subsystype* subsys);


/*
 * SMRP LES FS subsystem header
 */

#define SMRPLESFS_MAJVERSION 1
#define SMRPLESFS_MINVERSION 0
#define SMRPLESFS_EDITVERSION  1

SUBSYS_HEADER(smrp_les_fs, SMRPLESFS_MAJVERSION, SMRPLESFS_MINVERSION, SMRPLESFS_EDITVERSION,
	      smrpfast_les_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: smrp",
	      "req: fastswitch, smrp");

/* 
 *------------------------------------------------------------------
 * smrpfast_les_init
 *
 * Initialize SMRP fast switching globals, called once at startup.
 *------------------------------------------------------------------
 */
void
smrpfast_les_init (subsystype* subsys)
{
}

/*
 *------------------------------------------------------------------
 * smrpfast_fastswitch
 * 
 * This the the entry point for SMRP fast switching.  The network
 * layer fast switch code hands the packet over.
 *
 * When a packet comes in,
 * the SMRP group address is retrieved and used to look up the cache
 * entry.  If there is no entry or the entry is invalid, let process
 * level handle it.  But for valid entries, duplicate the packet
 * if not last port, rewrite the MAC and network header, and send it
 * out the fast switching child ports.  Skip non-fast switching ports.
 * If there are non-fast switching ports, return the packet to process 
 * level.  Otherwise, work is done in fast switching mode.
 *------------------------------------------------------------------
 */
boolean
smrpfast_fastswitch (paktype* pak)
{
    SMRPGroupID gid;
    SMRPPort *cport = NULL;
    SMRPForward* fwd;
    SMRPChildPortLink* child, *nextchild;
    paktype* np = NULL;
    MCHeader *hdr;
    tinybool return_to_process = FALSE;
    treeKey key;

    /* 
     * FUTURE - we could add a routine to do more extensive checking
     * on the header and faster via assembly code.  For now, do a 
     * quick and simple check.
     */

    /* 
     * Basic checks - let process level account for the packet if check
     * failed 
     */
    hdr = (MCHeader*)pak->transport_start;
    if (hdr == NULL)
	return FALSE;

    if (hdr->PacketType != MCMulticastDatagram)
	return FALSE;

    if (hdr->ProtocolVersion != SMRPVersion)
	return FALSE;

    if (pak->datagramsize - (pak->transport_start - pak->network_start)
	< SMRP_HEADER_SIZE) 
	return FALSE;

    gid = GETLONG(&hdr->GroupAddress);

    key.l = gid;
    fwd = (SMRPForward*) smrpfast_RBTreeSearch_inline(smrp_GroupForwardingTable, 
						  key);
    if (fwd == NULL) 
	return FALSE;

    /* Must be a valid cache entry */
    if (!fwd->FastEnabled || !fwd->CacheValid)
	return FALSE;

    smrp_stat[SMRP_FASTFWD_IN]++;

    for (child = fwd->ChildPorts; child; child = child->Next) {
	if (child->FastCache == NULL) {
#ifdef SMRP_FAST_DEBUG
	    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
		ttyprintf(CONTTY,"\nDEBUG: process switch");
#endif
	    return_to_process = TRUE;
	    continue;
	}

        key.l = child->ChildPortID;
        cport = (SMRPPort*) smrpfast_RBTreeSearch_inline(smrp_PortTable, key);
	if (cport == NULL) {
	    /* Can't send to port, drop pkt because process level can't
	       handle it either */
	    smrp_stat[SMRP_NO_FORWARD]++;
	    continue;
	}
	
#ifdef SMRP_FAST_DEBUG 
	if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	    ttyprintf(CONTTY,"\nDEBUG: fast switch");
#endif

	nextchild = child->Next;
	if (nextchild || return_to_process) {
	    /* 
	     * Duplicate pkt if there is another child port or
	     * some previous port were not fast switchable. 
	     */
	    np = pak_duplicate(pak);
	    if (np == NULL) {
		smrp_stat[SMRP_NO_BUFFERS]++;
		return FALSE;
	    }
	    smrp_stat[SMRP_FORWARD_DUPS]++;
	
	} else {
	    np = pak;
	    pak = NULL;
	}

	if ((*cport->Fastswitch)(np, child->FastCache)) {
	    smrp_stat[SMRP_FASTFWD_OUT]++;
	} else {
	    datagram_done(np);	    
	}
	
	smrp_stat[SMRP_FORWARDS]++;
    }
    
    /* The packet remains if there were any non-f/s child ports */
    if (pak)
	return FALSE;

    /* This makes sure that the input pkt is accounted for once */
    smrp_stat[SMRP_INPUT]++; 
    smrp_stat[SMRP_MCDATA_IN]++;

    return TRUE;
}


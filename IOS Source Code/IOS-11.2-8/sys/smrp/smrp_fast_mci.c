/* $Id: smrp_fast_mci.c,v 3.1.2.1 1996/05/02 08:44:45 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_fast_mci.c,v $
 *------------------------------------------------------------------
 * cBus SMRP fast switching support
 *
 * March 1996, Kent Leung
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_fast_mci.c,v $
 * Revision 3.1.2.1  1996/05/02  08:44:45  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/04/16  23:03:45  kleung
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
#include "../hes/if_mci.h"
#include "smrp_private.h"
#include "smrp_fast.h"

#ifdef SMRP_FAST_DEBUG
#include "ttysrv.h"
#endif

/* 
 * Private forward declarations
 */
static void smrpfast_mci_init (subsystype* subsys);


/*
 * SMRP MCI FS subsystem header
 */

#define SMRPMCIFS_MAJVERSION 1
#define SMRPMCIFS_MINVERSION 0
#define SMRPMCIFS_EDITVERSION  1

SUBSYS_HEADER(smrp_mci_fs, SMRPMCIFS_MAJVERSION, SMRPMCIFS_MINVERSION, 
	      SMRPMCIFS_EDITVERSION,
	      smrpfast_mci_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: smrp",
	      "req: fastswitch, smrp");

/* 
 *------------------------------------------------------------------
 * smrpfast_mci_init
 *
 * Initialize SMRP fast switching globals, called once at startup.
 *------------------------------------------------------------------
 */
static void
smrpfast_mci_init (subsystype* subsys)
{
}

/*
 *------------------------------------------------------------------
 * smrpfast_mci_fastswitch
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
smrpfast_mci_fastswitch (hwidbtype *input, mciregtype *mci_regptr,
			 ushort trans_offset, ushort unaligned)
{
    SMRPGroupID gid;
    SMRPPort *cport = NULL;
    SMRPForward* fwd;
    SMRPChildPortLink* child, *nextchild;
    MCHeader *hdr;
    tinybool return_to_process = FALSE;
    tinybool notdone = TRUE;
    treeKey key;
    ulong sniffbuf1[2];
    ulong sniffbuf2;

    /* 
     * FUTURE - we could add a routine to do more extensive checking
     * on the header and faster via assembly code.  For now, do a 
     * quick and simple check.
     */

    /*
     * Get SMRP header info.
     *
     * Move to the start of the SMRP header.  If unaligned, then move
     * one more over since there was one byte before the DDP header and
     * DDP header size itself is odd too.  Else, move to one byte
     * before the SMRP header.
     */
    mci_regptr->argreg = trans_offset;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * Here's what the SMRP frame looks like from the trans_offset
     * on...
     *
     * Unaligned packet:
     *
     * +-------+-------+-------+-------+-------+-------+-------+-------+
     * | Proto | Pkt   |               |                               |
     * | Vers- | Type  |  Sequence #   |          Group Address        |
     * |  ion  |       |               |                               |
     * +-------+-------+-------+-------+-------+-------+-------+-------+
     * {        first long sniff      }{     second long sniff        }
     *
     * Aligned packet:
     *
     * +-------+-------+-------+-------+-------+-------+-------+-------+-------+
     * | DDP   | Proto | Pkt   |               |                               |
     * | Type  | Vers- | Type  |  Sequence #   |          Group Address        |
     * |       |       |       |               |                               |
     * +-------+-------+-------+-------+-------+-------+-------+-------+-------+
     * {  first short }{       second long sniff      }{      third long sniff ..}
     *
     */
    if (unaligned) {
	sniffbuf1[0] = mci_regptr->readlong;
	sniffbuf1[1] = mci_regptr->readlong;
    } else {
	sniffbuf2 = mci_regptr->readshort;
	sniffbuf1[0] = mci_regptr->readlong;
	sniffbuf1[1] = mci_regptr->readlong;
	sniffbuf1[1] = (sniffbuf1[1] >> 8) | (sniffbuf1[0] << 24);
	sniffbuf1[0] = (sniffbuf1[0] >> 8) | (sniffbuf2 << 24);
    }
        
    /* 
     * Basic checks - let process level account for the packet if check
     * failed 
     */
    hdr = (MCHeader*)sniffbuf1;
    if (hdr == NULL)
	return FALSE;

    if (hdr->PacketType != MCMulticastDatagram)
	return FALSE;

    if (hdr->ProtocolVersion != SMRPVersion)
	return FALSE;
    
    /* Group in forwarding table? */
    gid = GETLONG(&hdr->GroupAddress);
    key.l = gid;
    fwd = (SMRPForward*) smrpfast_RBTreeSearch_inline(smrp_GroupForwardingTable, 
						      key);
    if (fwd == NULL)
	return FALSE;
    
    /* Must be a valid cache entry */
    if (!fwd->FastEnabled || !fwd->CacheValid) {
	return FALSE;
    }
    
    smrp_stat[SMRP_FASTFWD_IN]++;

    for (child = fwd->ChildPorts; child; child = child->Next) {
	if (child->FastCache == NULL) {
#ifdef SMRP_FAST_DEBUG
	    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
		fprintf(CONTTY,"\nDEBUG: process switch");
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
	    fprintf(CONTTY,"\nDEBUG: fast switch");
#endif

	nextchild = child->Next;
	if (nextchild || return_to_process) {
	    /* 
	     * Duplicate pkt if there is another child port or
	     * some previous port were not fast switchable. 
	     */
	    smrp_stat[SMRP_FORWARD_DUPS]++;

	} else {
	    notdone = FALSE;
	}

	(*cport->MCIFastswitch)(input, child->FastCache, notdone);
	smrp_stat[SMRP_FORWARDS]++;
	smrp_stat[SMRP_FASTFWD_OUT]++;
    }
    
    /* The packet remains if there were any non-f/s child ports */
    if (notdone) {
	return FALSE;
    }

    /* This makes sure that the input pkt is accounted for once */
    smrp_stat[SMRP_INPUT]++; 
    smrp_stat[SMRP_MCDATA_IN]++;

    return TRUE;
}

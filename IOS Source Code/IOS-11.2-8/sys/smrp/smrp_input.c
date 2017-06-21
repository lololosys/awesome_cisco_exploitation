/* $Id: smrp_input.c,v 3.3.62.2 1996/08/28 13:09:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_input.c,v $
 *------------------------------------------------------------------
 * Input processing routines for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_input.c,v $
 * Revision 3.3.62.2  1996/08/28  13:09:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:47:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:42:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:10:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:53:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:13:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:25:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/05  21:39:09  kleung
 * CSCdi36489:  Routes should pt to next available router when next hop is
 * gone.
 *
 * Revision 2.3  1995/06/23  16:34:24  slin
 * CSCdi36267: SMRP traceback - %SYS-2-LINKED: Bad p_enqueue of 63C124 in
 *             queue.
 *
 * Revision 2.2  1995/06/14  22:55:03  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "../os/signal.h"
#include "interface.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

extern pid_t smrp_input_pid;

/*
 *------------------------------------------------------------------
 * Module private data and functions
 *------------------------------------------------------------------
 */
watched_queue *smrpQ;	/* Watched Queue for input process */

/*
 *------------------------------------------------------------------
 * smrp_input_Initialize		(Module Initialization)
 *
 * Module initialization.  Should be called once at startup.
 *------------------------------------------------------------------
 */
void
smrp_input_Initialize (void)
{
}

static inline void
smrp_bg_enqueue (paktype *pak)
{
    process_enqueue_pak(smrpbgQ, pak);
}

static inline void
smrp_route_enqueue (paktype *pak)
{
    process_enqueue_pak(smrpRouteQ, pak);
}

static inline void
smrp_maint_enqueue (paktype *pak)
{
    process_enqueue_pak(smrpNodeQ, pak);
}

static inline void
smrp_input_enqueue (paktype *pak)
{
    process_enqueue_pak(smrpQ, pak);
}

/*
 *------------------------------------------------------------------
 * smrp_ValidPacket			(Module Private Function)
 *
 * Validate the packet header and return the packet type.  Returns
 * the packet type MCBadDatagram if the packet is invalid.
 *------------------------------------------------------------------
 */
static uchar
smrp_ValidPacket (paktype* pak)
{
    uchar *hdr;
    uchar pt;
    
    if (pak->datagramsize - (pak->transport_start - pak->network_start)
	< SMRP_HEADER_SIZE) {
	smrp_stat[SMRP_RUNT]++;
	return MCBadDatagram;
    }

    hdr = pak->transport_start;

    if (*hdr != SMRPVersion) {
	smrp_stat[SMRP_BADVERS]++;
	return MCBadDatagram;
    }

    pt = *(hdr + SMRP_PAKTYPE_OFFSET);
    
    /* Do some basic sanity checking of protocol type */
    if (smrp_GetPacketModifiers(pt) == MCDatagram) {
	if (smrp_GetPacketType(pt) != MCData &&
	    smrp_GetPacketType(pt) != MCHello) {
  	    smrp_stat[SMRP_BAD_PKTTYPE]++;
	    return MCBadDatagram;
	}
	if (GETSHORT(hdr + SMRP_SEQNUM_OFFSET) != 0) {
  	    smrp_stat[SMRP_BAD_PKTTYPE]++;
	    return MCBadDatagram;
	}
    } else if (smrp_GetPacketType(pt) == MCData) {
	smrp_stat[SMRP_BAD_PKTTYPE]++;
	return MCBadDatagram;
    }
    
    return pt;
}
    
/*
 *------------------------------------------------------------------
 * smrp_Forward				(Module Private Function)
 *
 * Forward MC datagrams packets received by input.  Called by
 * the input process, this routine duplicates the packet as
 * required and queues them to the appropriate ports.  Since
 * ports may involve different network protocols, shared packets
 * cannot be used.
 *
 * Lookup the group forward entry.  If none, this node is not
 * in its distribution tree.  If found, insure that data arrived
 * on the parent port, that entry is in state MCForward and
 * has children. For each valid child port, duplicate source 
 * packet and send.
 *------------------------------------------------------------------
 */
static void
smrp_Forward (SMRPPort* port, paktype* pak)
{
    SMRPGroupID gid;
    SMRPPort *cport = NULL;
    SMRPForward* fwd;
    int cnt;
    SMRPChildPortLink* child, *nextchild;
    paktype* np;
    MCHeader *hdr;
    boolean validcache;
    
    SMRPASSERT(pak != NULL);
    SMRPASSERT(port != NULL);
    
    hdr = (MCHeader *)pak->transport_start;
    gid = GETLONG(&hdr->GroupAddress);
    fwd = smrp_FindForward(gid);
    if (fwd == NULL) {
	smrp_Dispose(SMRP_NO_FORWARD,pak);
	return;
    }

    if (fwd->ParentPortID != port->PortID) {
	smrp_Dispose(SMRP_NO_FORWARD,pak);
	return;
    }
    cnt = fwd->ChildPortCount;
    if ((fwd->State != MCForwarding)|| (cnt == 0)) {
	smrp_Dispose(SMRP_NO_FORWARD,pak);
	return;
    }

#ifdef OLD
    clst = fwd->ChildPorts;
    while (cnt-- > 1 && clst) {
	cport = smrp_FindPort(clst->ChildPortID);
	clst = clst->Next;
	if (cport == NULL) {
	    smrp_stat[SMRP_NO_FORWARD]++;
	    continue;
	}
	np = pak_duplicate(pak);
	if (!np) {
	    smrp_stat[SMRP_NO_BUFFERS]++;
	    continue;
	}
        /* #### Grp MC index calc */
	if (PORTCB_SendMCPacket(cport,np,gid)) {
   	    smrp_stat[SMRP_FORWARD_DUPS]++;
	    smrp_stat[SMRP_FORWARDS]++;
        } else
	    smrp_stat[SMRP_NO_FORWARD]++;
    }

    if (clst)
        cport = smrp_FindPort(clst->ChildPortID);
    if (!cport) {
	buginf("\nSMRP: Pkt discarded, forwarding port doesn't exist");
	smrp_Dispose(-1, pak);
	return;
    }
    if (PORTCB_SendMCPacket(cport,pak,gid)) 
        smrp_stat[SMRP_FORWARDS]++;
    else 
        smrp_stat[SMRP_NO_FORWARD]++;

#endif

    validcache = fwd->CacheValid;
    
    for (child = fwd->ChildPorts; child; child = child->Next) {
	if (validcache && child->FastCache) 
	    continue;

	cport = smrp_FindPort(child->ChildPortID);
	if (cport == NULL) {
	    smrp_stat[SMRP_NO_FORWARD]++;
	    continue;
	}
	
	/* 
	 * This is a bit bizarre since the child is moved within
	 * the for loop.  Probably not good practice - but efficient
	 * in this case.    The child is actually moved
	 * right before the next non-f/s port.  This allows the
	 * for loop to incr to the correct port.  The f/s ports
	 * are skipped.
	 */
	nextchild = child->Next;
	while (nextchild) {
	    if (validcache && nextchild->FastCache) {
		child = nextchild;
		nextchild = child->Next;
		continue;
	    }
	    break;
	}

	/*
	 * Duplicate the packet only if there is another non-f/s
	 * to forward.
	 */
	if (nextchild) {
	    np = pak_duplicate(pak);
	    if (np == NULL) {
		smrp_stat[SMRP_NO_BUFFERS]++;
		continue;
	    }
	    smrp_stat[SMRP_FORWARD_DUPS]++;
	} else {
	    np = pak;
	    pak = NULL;
	}

	if (PORTCB_SendMCPacket(cport,np,gid)) 
	    smrp_stat[SMRP_FORWARDS]++;
	else 
	    smrp_stat[SMRP_NO_FORWARD]++;
    }

    /* 
     * If packet was not used, release the buffer.  This could
     * happen when packets are queued up while setting up the
     * the cache is occurring.  But when the packets
     * arrive, the cache is already validate and f/s'able.  So
     * the packet is not used at all - dropped here.  Not much
     * we can do, except make sure the buffer is released and
     * re-usable.
     */
    if (pak)
	datagram_done(pak);
    
    /* If f/s'able, then add fwd entry to cache */
    if (fwd->CacheUpdated) 
	smrpfast_AddEntry(fwd);
}

/*
 *------------------------------------------------------------------
 * smrp_FastProcessForward
 * 
 * Attempt to quickly forward Multicast Datagram packets.  Returns
 * TRUE if packet was processed, FALSE otherwise.
 *------------------------------------------------------------------
 */
boolean
smrp_FastProcessForward (paktype *pak)
{
    SMRPPort *port;
    uchar pt;

    if (!pak)
	return TRUE;		/* Pretend we swallowed packet */

    smrp_stat[SMRP_INPUT]++;

    pt = smrp_ValidPacket(pak);

    if (pt == MCBadDatagram) {
	/* smrp_ValidPacket already did stats */
	buginf("\nSMRP: Pkt failed ValidPacket, disposed");
	smrp_Dispose(-1, pak);
	return TRUE;
    }

    port = smrp_FindPortFromPak(pak);
    if (port == NULL) {
	smrp_Dispose(SMRP_BAD_PORTID, pak);
	return TRUE;
    }
    
    if (!smrp_IsPortActive(port)) {
	smrp_Dispose(SMRP_PORT_DOWN, pak);
	return TRUE;
    }
    
    if (pt == MCMulticastDatagram) {

	smrp_stat[SMRP_MCDATA_IN]++;
	
    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
        buginf("\nDEBUG: fast process fwd");
	
	if (smrp_test_running) {
	    if (reg_invoke_smrp_intercept_data_pkt(port, pak))
		return TRUE;
	}
	
	/* Forward datagram */
	smrp_Forward(port, pak);
	return TRUE;
    }
    /* Stick it on the Input process queue */
    smrp_input_enqueue(pak);
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * smrp_CreatePacketWithData		(SMRP Internal Function)
 *
 * Creates an SMRP packet with a data size of psize.  Once
 * created, it copies the data into it using the 
 * pak->transport_start data pointer.
 *------------------------------------------------------------------
 */
paktype*
smrp_CreatePacketWithData (SMRPPort* port, MCHeader* pdata, uint psize)
{
    paktype* pak = PORTCB_CreatePacket(port, psize);
    uchar* pp;

    if (pak == NULL)
	return NULL;

    pp = pak->transport_start;
    SMRPASSERT(pp != NULL);
    bcopy(pdata, pp, psize);
    pak->datagramsize = psize + (pak->transport_start - pak->datagramstart);
    return pak;
}

/*
 *------------------------------------------------------------------
 * smrp_CreatePacketWithHeader		(SMRP Internal Function)
 *
 * Creates an SMRP packet with a data size of psize.  Once
 * created, it copies only the header into it using the 
 * pak->transport_start data pointer. The data may be added
 * later and the pak->transport_size should be updated to the
 * actual data length prior to sending.
 *------------------------------------------------------------------
 */
paktype*
smrp_CreatePacketWithHeader (SMRPPort* port, MCHeader* pdata, uint psize)
{
    paktype* pak = PORTCB_CreatePacket(port, psize + SMRP_HEADER_SIZE);
    uchar* pp;

    if (pak == NULL)
	return NULL;

    pp = pak->transport_start;
    SMRPASSERT(pp != NULL);
    bcopy(pdata, pp, SMRP_HEADER_SIZE);
    return pak;
}

/*
 *------------------------------------------------------------------
 * smrp_Dispose				(SMRP Internal Function)
 *
 * Discard a packet and update the specified traffic statistic if
 * code is greater than zero.
 *------------------------------------------------------------------
 */
void
smrp_Dispose (int code, paktype* pak)
{
    /* return buffer before possible error msg: */
    if (pak != NULL)
	datagram_done(pak);

    if (code < 0) {
	;		/* no stat update requested */
    } else if (code < SMRP_MAXTRAFFIC)
	smrp_stat[code]++;
}

static void
smrp_input_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;
    
    reg_invoke_smrp_routing_change(FALSE);
    process_watch_queue(smrpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(smrpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&smrpQ);
    smrp_input_pid = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_input				(SMRP Input Process)
 *
 * Process packets as they are enqueued by the network layer.
 *
 * Only minimal packet verification is performed, the background
 * process performs additional checks before further processing.
 * Multicast datagrams are duplicated and forwarded if appropriate.
 * Hello packets and transaction packets are queued for processing
 * by the background process.  
 *
 * Note: Unlike network layer input processes, this input process
 * does not permit packets to be delivered at interrupt level.
 *------------------------------------------------------------------
 */
process
smrp_input (void)
{
    ulong major, minor;
    paktype *pak;
    uchar pt;
    int count;
    SMRPPort *port;

    /*
     * Startup
     */
    signal_permanent(SIGEXIT, smrp_input_teardown);
    smrpQ = create_watched_queue("SMRP input queue", 0, 0);
    process_watch_queue(smrpQ, ENABLE, RECURRING);

    reg_invoke_smrp_routing_change(TRUE);
    
    while (TRUE) {
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch(major) {
	    case QUEUE_EVENT:
		count = PAK_SWITCHCOUNT;
		while (TRUE) {
		    /*
		     * Process only a limited number of packets per pass.
		     */
		    if (--count == 0)
			break;

		    pak = process_dequeue(smrpQ);
		    if (!pak)
			break;

		    smrp_stat[SMRP_INPUT]++; /* count a packet seen */

		    pt = smrp_ValidPacket(pak);
		    if (pt == MCBadDatagram) {
			/* smrp_ValidPacket already did stats */
			buginf("\nSMRP: Pkt failed ValidPacket, disposed");
			smrp_Dispose(-1,pak);
			continue;
		    }
		    
		    port = smrp_FindPortFromPak(pak);
		    if (port == NULL) {
			smrp_Dispose(SMRP_BAD_PORTID, pak);
			continue;
		    }
		    
		    if (!smrp_IsPortActive(port)) {
			smrp_Dispose(SMRP_PORT_DOWN, pak);
			continue;
		    }
	    
		    if (pt == MCMulticastDatagram) {

    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
        buginf("\nDEBUG: smrp input data");
	
			smrp_stat[SMRP_MCDATA_IN]++;

			/* TESTMODE - Intercept data packets */
			if (smrp_test_running) {
			    if (reg_invoke_smrp_intercept_data_pkt(port,pak))
				continue;
			}
			
			/* Forward datagram */
			smrp_Forward(port, pak);
			continue;
		    }

		    if (pt == MCHelloDatagram) {				
			/* Queue packet for maint process */
			smrp_maint_enqueue(pak);
			continue;
		    }

		    if (smrp_GetPacketType(pt) == MCDistanceVector) {

			/* TESTMODE - Don't process an DV packets */
			if (smrp_test_running) {
			    smrp_Dispose(-1, pak);
			    continue;
			}
			
			/* Queue packet for routing process */
			smrp_route_enqueue(pak);
			continue;
		    }
		    
		    /* Queue packet for background process */
		    smrp_bg_enqueue(pak);
		}
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}

    

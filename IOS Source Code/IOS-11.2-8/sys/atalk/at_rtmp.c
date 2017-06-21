/* $Id: at_rtmp.c,v 3.6.18.6 1996/08/28 12:36:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_rtmp.c,v $
 *------------------------------------------------------------------
 * at_rtmp.c -- Appletalk RTMP protocol support
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_rtmp.c,v $
 * Revision 3.6.18.6  1996/08/28  12:36:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.5  1996/07/10  22:18:49  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.18.4  1996/05/02  08:40:36  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.6.18.3  1996/04/11  20:40:56  slin
 * CSCdi54305:  crash in interface_up/rtmp_IsValid
 * Branch: California_branch
 * - NULL pointer dereference
 *
 * Revision 3.6.18.2  1996/03/23  01:27:26  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6.18.1  1996/03/18  18:53:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  08:29:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  00:17:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  23:08:10  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.5  1996/02/01  05:59:11  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/24  04:56:28  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.3  1995/11/17  08:43:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  20:50:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/09/19  23:54:50  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.9  1995/08/11  00:12:49  kleung
 * CSCdi38405:  7k fails to learn path to net 50 via rtmp
 *
 * Revision 2.8  1995/07/24  22:30:34  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.7  1995/07/13 06:50:05  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.6  1995/07/05 22:24:52  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.5  1995/06/23  02:08:24  dwong
 * CSCdi35967:  AT fast-switch entries stay in route-cache too long
 * Invalidate the fast cache to remove old entries that may cause a
 * loop.
 *
 * Revision 2.4  1995/06/10  12:55:29  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.3  1995/06/09  19:24:13  kleung
 * CSCdi35457:  Atalk Phase 1 RTMP packet not handled properly.
 *
 * Revision 2.2  1995/06/09  12:58:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:09:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include <mgd_timers.h>
#include <interface_private.h>
#include "packet.h"
#include "ieee.h"
#include "../if/fddi.h"
#include "config.h"
#include "plural.h"
#include "../os/signal.h"
#include "atalk_private.h"
#include "at_maint.h"
#include "at_registry.h"
#include "sys_registry.h"
#include "parser_defs_atalk.h"
#include "at_domain.h"


/*
 * Private storage/declarations.
 */
static mgd_timer        atalk_rtmp_timer;
static mgd_timer	rtmp_pathAgerTimer;	/* timer for aging RTMP */
static mgd_timer        rtmp_pathGCTimer;

static watched_queue   *atalkrtmpQ;

enum {
    ATTIMER_RTMP_PATH,
    ATTIMER_RTMP_GC
};

/*
 * The following struct is used to keep the msclock timestamps
 * from various large operations blocks in the RTMP router process.
 */
typedef struct rtmp_cpu_t_ {
    ulong	pass_total;
    ulong	pak_time;
    ulong	path_time;
    ulong	gc_time;
    ulong	update_total;
} rtmp_cpu_t;

static rtmp_cpu_t	current_ticks;
static rtmp_cpu_t	highest_ticks;


static boolean	rtmp_IsValid(atalk_pdb_t *);
static void 	rtmp_AcquirePortInfo(paktype* pak);

static void 	rtmp_ReplaceEntry(atalk_pdb_t *, uchar, atalkidbtype *,
				  neighborentry *);
static void	rtmp_UpdateNeighborEntry(neighborentry*, atalkidbtype*,
					 boolean, boolean);

static void	rtmp_pathAger(void);
static boolean	atrtmp_SetPathState(atalk_pdb_t *, at_state_t);
static atalk_rdb_t *rtmp_GetNextRoute(rbTree *, atalkidbtype *, boolean,
				      boolean, atalk_rdb_t*);
static void     rtmp_UpdateEqualPath(atalk_pdb_t *, idbtype *, neighborentry *);


/* inline declarations */
static inline atalk_rdb_t*
rtmp_Cast (treeLink* link) { return(atalk_rdb_t*)link; }


/*
 * atrtmp_config_command
 *
 * Handle RTMP-specific commands and configuration.
 */
void
atrtmp_config_command (parseinfo *csb)
{
    ulong	update, valid, invalid;
#ifdef NOT_USED_YET
    boolean	doTimers;
#endif

    if (csb == NULL)
	return;

    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {
	  case ATALK_RTMP_BW:		/* move to atrtmp_config_command */
#ifdef NOT_USED_YET
	    nv_write((atalk_rtmp_bw != RTMP_BANDWIDTH), "%s %d",
		     csb->nv_command, atalk_rtmp_bw);
#endif
	    break;

	  case ATALK_RTMP_GC_INTERVAL:
	    nv_write((atalk_rtmpGCInterval != ONEMIN),
		     "%s %d", csb->nv_command, atalk_rtmpGCInterval);
	    break;

	  case ATALK_RTMP_JITTER:	/* move to atrtmp_config_command */
	    nv_write((atalk_rtmp_jitter != 0), "%s %d",
		     csb->nv_command, atalk_rtmp_jitter);
	    break;
	    
	  case ATALK_RTMP_TIMERS:
#ifdef NOT_USED_YET
	    doTimers = ((atalk_rtmpInvalidTime != RTMP_INVALIDTIME) ||
			(atalk_rtmpValidTime != RTMP_VALIDTIME) ||
			(atalk_rtmpUpdateTime != RTMP_UPDATETIME));
	    nv_write(doTimers,"%s %d %d %d", csb->nv_command,
		     (atalk_rtmpUpdateTime == RTMP_UPDATETIME) ?
		     0 : atalk_rtmpUpdateTime/ONESEC,
		     (atalk_rtmpValidTime == (atalk_rtmpUpdateTime*RTMP_VALIDMULT) ?
		      0 : atalk_rtmpValidTime/ONESEC),
		     (atalk_rtmpInvalidTime == (atalk_rtmpValidTime*RTMP_INVALIDMULT) ?
		      0 : atalk_rtmpInvalidTime/ONESEC));
#endif
	    break;
	    
	  default:
	    break;
	}
	return;
    }

    /*
     * Actual command parsing follows
     */
    switch (GETOBJ(int,1)) {
      case ATALK_RTMP_BW:
	atalk_rtmp_bw = csb->sense ? GETOBJ(int,2) : RTMP_BANDWIDTH;
	break;

      case ATALK_RTMP_GC_INTERVAL:
	atalk_rtmpGCInterval = csb->sense ? GETOBJ(int,2) : ONEMIN;
	break;

      case ATALK_RTMP_JITTER:
	atalk_rtmp_jitter = csb->sense ? GETOBJ(int,2) : 0;
	break;

      case ATALK_RTMP_TIMERS:
	if (csb->sense && GETOBJ(int,2)) {
	    update = GETOBJ(int,2) * ONESEC;
	} else {
	    update = RTMP_UPDATETIME;
	}
	if (csb->sense && GETOBJ(int,3)) {
	    valid = GETOBJ(int,3) * ONESEC;
	} else {
	    valid = update * RTMP_VALIDMULT;
	}
	if (valid <= update) {
	    printf("\n%% Error: Valid timer (%d) must be greater than "
		   "Update timer (%d)", valid, update);
	    return;
	}
	if (csb->sense && GETOBJ(int,4)) {
	    invalid = GETOBJ(int,4) * ONESEC;
	} else {
	    invalid = valid * RTMP_INVALIDMULT;
	}
	if (invalid < (valid + update)) {
	    printf("\n%% Error: Invalid timer (%d) must be greater than or"
		   " equal to\n%%        sum of Update timer and"
		   " Valid timer (%d)", invalid, valid + update);
	    return;
	}
	atalk_rtmpUpdateTime  = update;
	atalk_rtmpValidTime   = valid;
	atalk_rtmpInvalidTime = invalid;
 	break;

      default:
	break;
    }
}

/*
 * atalk_rtmp_set_timer
 * Set the timer on the current path. Mark it 'not to age' if snapshot
 * is running.
 */
void atalk_rtmp_set_timer (atalk_pdb_t *p)
{
    GET_TIMESTAMP(p->timer);
    p->snapshot_no_age =
	reg_invoke_snapshot_aging_denied(p->idb->phys_idb);
}

/*
 * Callback routines. These are called from the central routing module
 * when something happens to a route.
 */

/*
 * rtmp_HandleXRequest
 * Handle requests for the ports cable range.
 */
static void
rtmp_HandleXRequest (paktype* pak)
{
    paktype *rsppak;
    rtmpxrsp *rsp;
    ushort srcnet;
    rtmptuple* netend;
    atalkidbtype *idb;

    idb = atalk_getidb(pak->if_input);
    atalk_pak_printprocess(pak, atalkrtmp_debug, "RTMP(xnet Req)");
    rsppak = atalk_pak_reply(pak,DDP_TYPE_RTMPRsp,RTMPXRSPBYTES);
    if (rsppak == NULL) 
	return;
    if (atalkif_HalfRouter(idb))
	atalk_pak_setsnet(pak,0);
    rsp = (rtmpxrsp *)rsppak->transport_start;
    netend = (rtmptuple*)
	(rsppak->transport_start + RTMPHDRBYTES + RTMPTUPLEBYTES);
    srcnet = atalkif_MyNet(idb);
    if (atalkif_HalfRouter(idb)) {
	atalk_pak_setsnet(pak,0);
	srcnet = 0;
    }
    PUTSHORT(rsp->hdr.srcnet, srcnet);
    rsp->hdr.idlen = 8;		/* we use 8 bit id's */
    rsp->hdr.id[0] = atalkif_MyNode(idb);
    rsp->netstart.dist = RTMPTUPLERANGE;
    PUTSHORT(rsp->netstart.net, idb->atalk_cablestart);
    netend->dist = RTMPTUPLEVERSION;
    PUTSHORT(netend->net, idb->atalk_cableend);
    atalk_DDPcksum(rsppak);
    if (atalk_send(rsppak))
	atalk_stat[ATALK_RTMP_OUT]++;
}

/*
 * rtmp_HandleRequest
 * send a response to an rtmp request
 * sends back an RTMP data packet directly to requestor, with no data
 * non-bridge nodes send reqs to find A-BRIDGE and THIS-NET
 */
static void
rtmp_HandleRequest (paktype* pak, ataddrtype srcaddr)
{
    paktype *rsppak;
    rtmpreq *req;
    rtmprsp *rsp;
    ushort srcnet = atalk_netonly(srcaddr); /* src network of incoming pak */
    ushort ournet;			/* our responding idb net # */
    uchar srcsock;
    atalkidbtype *idb;
    
    idb = atalk_getidb(pak->if_input);
    srcsock = atalk_pak_ssock(pak);
    atalk_stat[ATALK_RTMP_REQUEST]++;
    req = (rtmpreq*) pak->transport_start;
    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, pak->if_input))
	buginf("\nAT: RTMP req with cmd %d rcvd from %a via %s", req->cmd, 
	       srcaddr, idb_get_namestring(idb->phys_idb));
    switch (req->cmd) {
    case RTMPReq_SplitTable:
	atalk_pak_printprocess(pak, atalkrtmp_debug, "RTMP(split)");
	atrtmp_SendUpdate(idb ,srcnet,srcaddr,srcsock,TRUE);
	return;
    case RTMPReq_FullTable:
	atalk_pak_printprocess(pak, atalkrtmp_debug, "RTMP(full)");
	atrtmp_SendUpdate(idb ,srcnet,srcaddr,srcsock,FALSE);
	return;
    case RTMPReq_NetRequest:
	break;
    default:
	atalk_stat[ATALK_RTMP_BAD]++;
	atalk_pak_printfail(pak, atalkrtmp_debug, "unknown RTMP request");
	return;
    }
    if (atalkif_net_extended(idb) && !atalk_testToolCompat) {
	rtmp_HandleXRequest(pak);
	return;
    }
    atalk_pak_printprocess(pak, atalkrtmp_debug, "RTMP(netReq)");
    rsppak = atalk_pak_reply(pak,DDP_TYPE_RTMPRsp,RTMPRSPBYTES);
    if (rsppak == NULL) 
	return;
    rsp = (rtmprsp *)rsppak->transport_start;
    ournet = atalkif_MyNet(idb);
    if (atalkif_HalfRouter(idb)) {
	ournet = 0;
	atalk_pak_setsnet(rsppak,0);
    }
    PUTSHORT(rsp->srcnet, ournet);
    rsp->idlen = 8;			/* we use 8 bit id's */
    rsp->id[0] = atalkif_MyNode(idb);
    atalk_DDPcksum(rsppak);
    if (atalk_send(rsppak))
	atalk_stat[ATALK_RTMP_OUT]++;
}

/*
 * rtmp_ValidPacket
 * Insure the packet passes muster before processing routing data.
 * Checks include srcnet and id, packet length, src directly connected,
 * and not ours.
 */
static uchar
rtmp_ValidPacket (paktype* pak)
{
    rtmprsp *rsp;
    int nbytes;
    ataddrtype srcaddr;
    ushort srcnet;
    uchar pt;
    atalkidbtype* idb;


    idb = atalk_getidb(pak->if_input);
    nbytes = pak->atalk_datalen;
    rsp = (rtmprsp*) pak->transport_start;
    pt = atalk_pak_getpt(pak);

    if (nbytes < 1) {
	atalk_pak_printbad(pak,"RTMP packet without data", idb);
	atalk_stat[ATALK_RTMP_BAD]++;
	return DDP_TYPE_Invalid;
    }
    if (pt == DDP_TYPE_RTMPReq) {
	rtmpreq* req = (rtmpreq*) pak->transport_start;
	switch (req->cmd) {
	case RTMPReq_NetRequest:
	case RTMPReq_SplitTable:
	case RTMPReq_FullTable:
	    return pt;
	default:
	    atalk_pak_printbad(pak,"RTMP unknown request",idb);
	    atalk_stat[ATALK_RTMP_BAD]++;
	    return DDP_TYPE_Invalid;
	}
    } else if (pt != DDP_TYPE_RTMPRsp) {
	atalk_pak_printbad(pak,"RTMP ddp type unknown",idb);
	atalk_stat[ATALK_RTMP_BAD]++;
	return DDP_TYPE_Invalid;
    }

    if ((rsp->idlen != 8)||(nbytes < RTMPHDRBYTES)) {
	atalk_pak_printbad(pak,"RTMP invalid header",idb);
	atalk_stat[ATALK_RTMP_BAD]++;
	return DDP_TYPE_Invalid;
    }

    srcnet = GETSHORT(rsp->srcnet);
    srcaddr = atalk_address(srcnet, rsp->id[0]);
    if (srcnet == 0) {
	if (!atalkif_HalfRouter(idb) && !atalkif_CaymanTunnel(idb)) {
	    atalk_pak_printbad(pak,"RTMP source net invalid",idb);
	    atalk_stat[ATALK_RTMP_BAD]++;
	    return DDP_TYPE_Invalid;
	}
    } else {	    
	if (!atalk_validSrcAddress(idb, srcaddr)) {
	    if (atalkif_operational(idb) || !idb->atalk_discovery) {
		atalk_pak_printbad(pak,"RTMP source not valid",idb);
		idb->phys_idb->hwptr->atalk_inputerrs++;
		atalk_stat[ATALK_RTMP_BAD]++;
		return DDP_TYPE_Invalid;
	    }
	}
	if ((idb->atalk_enctype == ET_ETHERTALK) && 
	    (idb->aarp_enctype == ET_NULL) &&
	    (atalk_nodeonly(srcaddr) == idb->atalknode)) {
	    errmsg(AT_ERR_ADDRINUSE, idb_get_namestring(idb->phys_idb),
		   srcaddr);
	    idb->atalknode ^= idb->atalknode;
	    atalkif_RestartPort(idb);
	    return DDP_TYPE_Invalid;
	} else 	if (atalkif_IsMyAddress(idb,srcaddr)) {
	    atalk_pak_printbad(pak,"RTMP src from us?",idb);
	    return DDP_TYPE_Invalid;
	}
	/* At this point, the RTMP data packet source address 
	 * has been verified, so regardless of other actions taken
	 * we save this address as the GW of last resort.
	 */
	idb->atalk_defaultgw = srcaddr;
    }
    if (!atalkif_InterfaceUp(idb))
	return DDP_TYPE_Invalid;
    return (pt);
}


neighborentry*
rtmp_GetNeighbor (paktype* pak)
{
    rtmprsp *rsp;
    ataddrtype srcaddr;
    ushort srcnet;
    sys_timestamp now;
    neighborentry  *n;
    atalkidbtype *idb;

    idb = atalk_getidb(pak->if_input);
    rsp = (rtmprsp*) pak->transport_start;
    srcnet = GETSHORT(rsp->srcnet);
    srcaddr = atalk_address(srcnet, rsp->id[0]);
    GET_TIMESTAMP(now);
    n = atmaint_FindNeighbor(srcaddr, idb);
    if (n == NULL) {			/* didn't find it, so create */
	n = atmaint_CreateNeighbor(srcaddr, idb);
	if (n == NULL)
	    return (NULL);
	COPY_TIMESTAMP(now, n->rtmp_firstheard);
	n->rtmp_state = neighborUp;
	COPY_TIMESTAMP(now, n->rtmp_exptimer);
	COPY_TIMESTAMP(now, n->rtmp_restarted);
    } else {
	atmaint_ProtectNeighbor(n);
    }
    n->reachable_by |= ATALK_RTMP_ENABLED;
    return (n);
}

static void
rtmp_AgeNeighbor (neighborentry *n)
{
    int new_rtmp_state;
    
    if (n == NULL || !(n->reachable_by & ATALK_RTMP_ENABLED) ||
	n->rtmp_state == neighborDown || SLEEPING(n->rtmp_exptimer))
	return;

    /* Set timer for next time we should continue to below */
    TIMER_START(n->rtmp_exptimer, atalk_rtmpValidTime);

    switch(n->rtmp_state) {
    case neighborUp:
	new_rtmp_state = neighborOverdue;
	break;
    case neighborOverdue:
	new_rtmp_state = neighborDown;
	TIMER_STOP(n->rtmp_exptimer);
	if (at_debug(atalk_events, ATALK_INTERFACE_MASK, n->port->phys_idb))
	    errmsg(AT_ERR_LOSTNEIGHBOR,
		   idb_get_namestring(n->port->phys_idb), n->address);
	atalk_InvalidateCache("maint_NeighborAger");
	break;
    default:
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, n->port->phys_idb))
	    buginf("\nAT: bad RTMP state for neighbor %a",n->address);
	new_rtmp_state = neighborDown;
	break;
    }
    if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, n->port->phys_idb))
	buginf("\nAT: RTMP state change for neighbor %a: %s -> %s",
	       n->address,
	       atmaint_NeighborState(n->rtmp_state),
	       atmaint_NeighborState(new_rtmp_state));

    if (new_rtmp_state == neighborDown) {
	n->reachable_by &= ~(ATALK_RTMP_ENABLED);
    }
    n->rtmp_state = new_rtmp_state;
}


/*
 * rtmp_GetNextTuple
 * Grab the next complete tuple out of a RTMP packet, doing full validation.
 *
 * If this function returns FALSE, there will be no updating of the
 * parameters (rt, nbytes, rngstart, rngend, dist). This is to
 * improve the speed of this function; if the packet's information
 * is in error, we abort as quickly as possible with the minimum of
 * CPU expenditure.
 *
 * If the return is TRUE, then the caller can expect the parameters
 * to have been updated.
 *
 * Note to those looking at this code to optmize it:
 * This code is optmized to execute as few instructions possible from
 * entry to return; this has resulted in the TRUE return writing
 * the output parameters and the FALSE return not writing them at all, 
 * as well as the TRUE return writing *only* those parameters which
 * are valid at that particular return point.
 * In particular, don't modify the code in one "if () { ... return (TRUE);}"
 * clause without modifying all other such clauses; the compiler is able
 * to optimize out the redundant code IFF the sequence of lines in
 * the "return (TRUE)" clauses is identical.
 */
boolean
atrtmp_GetNextTuple (rtmptuple	**rt,
		     int	*nbytes_o,
		     ushort	*rngstart_o,
		     ushort	*rngend_o,
		     uchar	*dist_o,
		     uchar	version)
{
    rtmptuple	*tuple_ptr;
    ushort	dist;			/* we really need only a byte... */
    ushort	rngstart;
    ushort	rngend;
    ushort	nbytes;
    tinybool	range;
    
    /*
     * If we don't have enough left in the buffer for a full tuple,
     * it can't possibly be valid. Don't touch it.
     */
    if (*nbytes_o < RTMPTUPLEBYTES)
	return (FALSE);
    nbytes = *nbytes_o;
    tuple_ptr = *rt;
    rngstart = GETSHORT(&tuple_ptr->net);
    rngend = 0;				/* assume non-extended tuple for now */
    dist = tuple_ptr->dist;		/* assume: poison gives right mask */
    nbytes -= RTMPTUPLEBYTES;		/* account for tuple read, move ptr */
    tuple_ptr = (rtmptuple *)((uchar *)tuple_ptr + RTMPTUPLEBYTES);

    if (rngstart == 0) {
	if (dist == version) {
	    *dist_o = dist;
	    *rngstart_o = rngstart;
	    *rngend_o = rngend;
	    *nbytes_o = nbytes;
	    *rt = tuple_ptr;
	    return (TRUE);
	} else {
	    return (FALSE);
	}
    }
    /*
     * Validate the distance byte. First see if this is an extended
     * tuple and turn off bit 7 in the byte.
     */
    range = ((dist & RTMPTUPLERANGE) != 0);
    dist &= ~RTMPTUPLERANGE;		/* flip off the range bit */
    if (((dist & ~RTMPTUPLEDIST) != 0) && (dist != RTMPTUPLEPOISON))
	return (FALSE);

    if (rngstart >= ATALK_STARTUP_START	&& rngstart != ATALK_NET_UNNUMBERED)
	return (FALSE);

    if (!range) {			/* is this a non-extended tuple? */
	*dist_o = dist;
	*rngstart_o = rngstart;
	*rngend_o = rngend;
	*nbytes_o = nbytes;
	*rt = tuple_ptr;
	return (TRUE);			/* end non-extended tuple checking */
    }

    /*
     * From here down, we process extended route tuples.
     */
    if (nbytes < RTMPTUPLEBYTES)	/* can we read another tuple? */
	return (FALSE);

    if (tuple_ptr->dist != version)
	return (FALSE);

    rngend = GETSHORT(&tuple_ptr->net);	/* read the upper network # */
    nbytes -= RTMPTUPLEBYTES;
    tuple_ptr = (rtmptuple *)((uchar *)tuple_ptr + RTMPTUPLEBYTES);

    /*
     * If the start and ending network ranges are the 'net
     * unnumbered' net numbers and all the other RTMP
     * tuple info is valid, then we're probably talking to a 
     * half-router on the other end.
     */
    if (   ((rngstart <= rngend) && (rngend < ATALK_STARTUP_START))
	|| (   rngstart == ATALK_NET_UNNUMBERED
	    && rngend == ATALK_NET_UNNUMBERED)) {
	*dist_o = dist;
	*rngstart_o = rngstart;
	*rngend_o = rngend;
	*nbytes_o = nbytes;
	*rt = tuple_ptr;
	return (TRUE);
    }

    /* Catch-all case */
    return (FALSE);
}    

/*
 * rtmp_PreSeedTable
 * What's going on here is very significant for performance. Attend, please:
 * The underlying data structure for the routing table(s) is a Red-Black tree.
 * The worst-case performance of a RB-tree is realized when you insert
 * all the entries into the tree in ascending (sorted) order; this will
 * cause many tree splits and rotations as the entries go in.
 * This may not be a problem for packets coming into the router from non-cisco
 * routers, but in an all-cisco net, all cisco routers send their RTMP
 * updates in network-number sorted order. Therefore, worst-case
 * performance can be quickly realized by making all your AppleTalk routers
 * cisco routers, eh? Not a marketing plus...
 * 
 * As a result, the first time we hear from a neighboring router, we pre-seed
 * the routing table with every 7th tuple. This prevents the tree from
 * performing a worst-case insertion when the whole RTMP packet is processed.
 * dsa/920121
 */
static void
rtmp_PreSeedTable (rbTree *t,rtmptuple *rt,int nbytes, atalkidbtype *idb,
		   neighborentry* neighbor)
{
    ushort cnt = 0;
    ushort rngstart, rngend;		/* start/end of the net # range */
    ushort prev_rngstart = 0;
    ushort prev_rngend = 0;		/* previous range start/end */
    uchar dist;				/* rtmp metric (distance in hops) */
    uchar prev_dist = 0;
    boolean valid;			/* TRUE if the net# range is valid */


    while (nbytes > 0) {
	if (!atrtmp_GetNextTuple(&rt,&nbytes,&rngstart,&rngend,&dist,
				 RTMPTUPLEVERSION)) 
	    return;
	valid = FALSE;
	/*
	 * Don't insert entries which are already poisoned.
	 */
	if (dist <= DDP_MAX_HOPCOUNT) {
	    if (atroute_GetValidPath(t,rngstart,rngend,&valid,FALSE) == NULL) {
		cnt++;
		if (valid) {
		    if ((cnt % 7) == 0) {
			if (prev_rngstart == 0) {
			    prev_rngstart = rngstart;
			    prev_rngend   = rngend;
			    prev_dist     = dist;
			} else {
			    atrtmp_CreateEntry(t, rngstart, rngend, dist,
					       idb, neighbor);
			    atrtmp_CreateEntry(t, prev_rngstart, prev_rngend,
					       prev_dist, idb, neighbor);
			    prev_rngstart = prev_rngend = prev_dist = 0;
			}
		    }
		}
	    }
	}
    }
}

/*
 * rtmp_HandleResponse
 * update tables from an rtmp response/data packet which was received
 * from the specified neighbor.
 */
static void
rtmp_HandleResponse (paktype* pak, neighborentry *neighbor)
{
    rtmptuple	*rt;
    int		nbytes;
    atalk_pdb_t *p;
    atalk_rdb_t *r;
    rbTree	*p_tree, *r_tree;
    ushort	rngstart, rngend;
    boolean	extended, valid;
    boolean	preSeedTable = FALSE;
    uchar	dist;
    idbtype	*idb;
    atalkidbtype *atalkidb;
    ulong	standby_interval;
    atalk_metric_t metric;

    idb = pak->if_input;
    if (idb == NULL || neighbor == NULL)
	return;

    atalkidb = atalk_getidb(idb);
    if (atalkidb == NULL)
	return;
    
    if (!atalkif_operational(atalkidb)) {
	atalk_pak_printbad(pak,"port disabled",atalkidb);
	atalk_stat[ATALK_NOT_OPERATIONAL]++;
	return;
    }

    if (atalk_ftzPortCheck(atalkidb)) {
	atalk_pak_printbad(pak,"secure FTZ port",atalkidb);
	atalk_stat[ATALK_RTMP_IGNORED]++;
	return;
    }

    if (atalkidb->atalk_rtmp_stub) {
	rtmp_UpdateNeighborEntry(neighbor, atalkidb, TRUE, FALSE);
	neighbor->rtpacketsignored++;
	if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, atalkidb->phys_idb)) {
	    buginf("\nAT: Ignoring packet from %a - RTMP stub mode",
		   neighbor->address);
	}
	return;
    }
    
    if (rtmp_ValidPacket(pak) != DDP_TYPE_RTMPRsp) {
	atalk_pak_printbad(pak,"invalid RTMP data packet",atalkidb);
	atalk_stat[ATALK_RTMP_BAD]++;
	return;
    }

    reg_invoke_snapshot_update_received(idb, (paktype *) pak);

    /*
     * Standby routing. Thank ECS for this feature.
     *
     * if atalkidb->atalk_rtmp_standby is non-zero, then that is the
     * period after which if we have not heard any other neighbors handing
     * us a RTMP Response that we "turn on" our routing.
     *
     * atalkidb->atalk_rtmp_standby == 0 means that standby routing is
     * disabled and we should process everything normally.
     */
    if (atalkidb->atalk_rtmp_standby != 0) {
	standby_interval = atalkidb->atalk_rtmp_standby * ONESEC;
	if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, atalkidb->phys_idb)) {
	    buginf("\nAT: Testing for standby from t' of %#Ta w/ int of %lu",
		   atalkidb->atalk_rtmp_sb_lasthrd, standby_interval);
	}
	if (!TIMER_RUNNING(atalkidb->atalk_rtmp_sb_lasthrd) ||
	    CLOCK_IN_INTERVAL(atalkidb->atalk_rtmp_sb_lasthrd, standby_interval)) {
	    /*
	     * We're hearing this update w/in the standby interval.
	     * Set our "last heard", account for the packet and pitch it.
	     */
	    GET_TIMESTAMP(atalkidb->atalk_rtmp_sb_lasthrd);
	    rtmp_UpdateNeighborEntry(neighbor, atalkidb, TRUE, FALSE);
	    neighbor->rtpacketsignored++;
	    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, atalkidb->phys_idb)) {
		buginf("\nAT: Ignoring packet from %a at %#Ta",
		       neighbor->address, atalkidb->atalk_rtmp_sb_lasthrd);
	    }
	    return;
	}
	if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, atalkidb->phys_idb)) {
	    buginf("\nAT: Dropping out of standby mode");
	}
	TIMER_STOP(atalkidb->atalk_rtmp_sb_lasthrd);
    }

    /*
     * If we have not seen any routing packets from this neighbor previously,
     * then pre-seed the routing table.
     */
    if (CLOCK_IN_INTERVAL(neighbor->started, ONEMIN))
	preSeedTable = TRUE;

    rt = (rtmptuple *)(((uchar *)pak->transport_start) + RTMPHDRBYTES);
    nbytes = pak->atalk_datalen - RTMPHDRBYTES;
    extended = atalkif_net_extended(atalkidb);
    /* ??? In future, this will get the RTMP path tree for this world. */
    p_tree = atroute_GetPTree(AT_PATH_RTMP);
    r_tree = atroute_GetTree();

    atalk_pak_printprocess(pak, atalkrtmp_debug, "RTMP data");

    if (extended && !atalkif_CaymanTunnel(atalkidb)) {
	if (!atrtmp_GetNextTuple(&rt,&nbytes,&rngstart,&rngend,&dist,
				 RTMPTUPLEVERSION)) {
	    atalk_pak_printfail(pak,atalkrtmp_debug,"RTMP packet header bad");
	    atalk_stat[ATALK_RTMP_BAD]++;
	    rtmp_UpdateNeighborEntry(neighbor,atalkidb,FALSE,FALSE);
	    return;
	}
	if ((rngend == 0) && (rngstart == atalkidb->atalk_cablestart)) {
	    atmaint_MarkNeighborObsolete(neighbor);
	    rt = (rtmptuple *)(((uchar *)pak->transport_start) + RTMPHDRBYTES);
	    nbytes = pak->atalk_datalen - RTMPHDRBYTES;
	} else 	if ((atalkidb->atalk_cablestart != rngstart)||
		    (atalkidb->atalk_cableend != rngend)) {
	    atalk_pak_printfail(pak,atalkrtmp_debug,"RTMP cable mismatch");
	    atalk_stat[ATALK_RTMP_BAD]++;
	    rtmp_UpdateNeighborEntry(neighbor,atalkidb,FALSE,FALSE);
	    atmaint_NeighborConfigBad(neighbor);
	    return;
	}
    } else {
	if (GETSHORT(rt->net) != 0)
	    atmaint_MarkNeighborObsolete(neighbor);
	if (!neighbor->obsoleterouter) {
	    if (!atrtmp_GetNextTuple(&rt,&nbytes,&rngstart,&rngend,&dist,
				     RTMPTUPLEVERSION)) {
		atalk_pak_printfail(pak,atalkrtmp_debug,"RTMP header bad");
		atalk_stat[ATALK_RTMP_BAD]++;
		rtmp_UpdateNeighborEntry(neighbor,atalkidb,FALSE,FALSE);
		return;
	    }
	}
    }
    /*
     * "strict RTMP" means that we should not be routing RMTP packets;
     * ie, RTMP packets can only come from connected networks _and_ 
     * we should be seeing these packets coming in on the interface
     * configured for that network number.
     */
    if (atalk_strictRTMP)
	if (neighbor->port != atalkidb) {
	    atalk_pak_printfail(pak,atalkrtmp_debug,"routed RTMP data");
	    rtmp_UpdateNeighborEntry(neighbor,atalkidb,FALSE,FALSE);
	    atalk_stat[ATALK_RTMP_BAD]++;
	    return;
	}
    rtmp_UpdateNeighborEntry(neighbor,atalkidb,TRUE,FALSE);

    /*
     * If the neighbor is reachable by IGRP2 as well, then pitch
     * this update other than updating the neighbor information.
     */
    if (neighbor->reachable_by & ATALK_IGRP_ENABLED) {
	return;
    }

    if (preSeedTable && (atalkidb->atalk_route_filter_in == 0))
	rtmp_PreSeedTable(p_tree,rt,nbytes,atalkidb,neighbor);

    if ( atalk_DomainRouter && atalkidb->atalk_dminfo && atalkidb->atalk_dminfo->in) {
	if (atalk_DomainRouter_running)
	    atdomain_ProcessRTMP(pak);
	return;
    }

    /*
     * Loop through all routing tuples.
     */
    while (nbytes > 0) {
	if (!atrtmp_GetNextTuple(&rt,&nbytes,&rngstart,&rngend,&dist,
				 RTMPTUPLEVERSION)) {
	    atalk_pak_printfail(pak,atalkrtmp_debug,"RTMP bad tuple");
	    neighbor->rtpacketsaborted++;
	    break;
	}
	/*
	 * Check for admin distance and add it to provided distance.
	 */
	if (atalkidb->atalk_distance) {
	    dist += atalkidb->atalk_distance / RTMP_BANDWIDTH;
	}
	if (rngend == 0)
	    neighbor->rtnonextendedrts++;
	else
	    neighbor->rtextendedrts++;
	if (  atalkidb->atalk_route_filter_in &&
	    ! atalk_routecheck(rngstart, rngend,
			       atalkidb->atalk_route_filter_in))
	    continue;

	/*
	 * New idea on how processing of incoming RTMP updates works:
	 * 1. We consider that we have a current RTMP entry for a net
	 *    if we have an RTMP path to said net/net range.
	 * 2. If we don't have a RTMP path to said destination, then we
	 *    create one; if no route previously existed to that
	 *    net/net range, then one is created.
	 */
	valid = FALSE;
	r = NULL;
	p = NULL;
	if ((dist <= DDP_MAX_HOPCOUNT) || (dist == DDP_POISON_HOPCOUNT)) {
	    r = atroute_GetValidRoute(r_tree,rngstart,rngend,&valid, 
				      (dist == DDP_POISON_HOPCOUNT));
	}

	/* If we get an invalid route, then tuple conflicts with something
	 * in our routing table.  Skip this tuple.
	 */
	if (!valid) {
	    neighbor->rtbadrts++;
	    continue;
	}
	
	if (r && valid && atalk_net_connected(r)) {
	    if (dist == DDP_POISON_HOPCOUNT)
		neighbor->rtbadrts++;	/* poisoning one of our ifs */
	    else
		neighbor->rtignoredrts++;
	} else {			/* didn't have route or !connected */
	    /*
	     * If the tuple doesn't have the same width as the
	     * route entry but it has the same lower end as the route,
	     * then ignore this tuple. If the tuple really does
	     * represent the new path for this route, then
	     * we'll time out the route and re-learn from
	     * the new extended route.
	     */
	    if (   r
		&& ((r->dstrngstr == rngstart) && (r->dstrngend != rngend))) {
		neighbor->rtignoredrts++;
		continue;
	    }
	    /*
	     * ??? Rewrite this cruft.
	     */
	    p = atroute_GetValidPath(p_tree,rngstart,rngend,&valid,
				     (dist == DDP_POISON_HOPCOUNT));
	    if (p) {			/* found a RTMP path route */
		if ((dist == DDP_MAX_HOPCOUNT) &&
		    (atalkidb->atalk_compatibility)) {
		    dist = DDP_POISON_HOPCOUNT;
		    neighbor->oldcisco = TRUE;
		}
		if ((dist == DDP_POISON_HOPCOUNT) && 
		    (atrtmp_GetPathMetric(p) <= DDP_MAX_HOPCOUNT))
		    neighbor->rtpoisonedrts++;
		else
		    neighbor->rtoldrts++;

		if (dist != DDP_POISON_HOPCOUNT && dist < DDP_MAX_HOPCOUNT) {
		    bzero(&metric, sizeof(atalk_metric_t));
     		    metric.type = AT_PATH_RTMP;
		    atrtmp_SetMetric(&metric, dist + 1);
		    if (atroute_AddEqualPath(p, idb, rngstart, rngend, neighbor,
					     AT_PATH_RTMP, &metric) == FALSE)
		       rtmp_UpdateEntry(p, dist, atalkidb, neighbor);
		}
		else
		    rtmp_UpdateEntry(p, dist, atalkidb, neighbor);

	    } else {			/* didn't find current RTMP path */
		if (valid) {
		    /*
		     * cisco routers prior to 8.2 used max hops as poison
		     * but they also reported them in every RTMP packet so
		     * we ignore them since we didn't have a route anyway.
		     */
		    if (dist < DDP_MAX_HOPCOUNT) {
			/*
			 * Don't dare accept an RTMP path for a route if the
			 * current route is from EIGRP and we are in holddown.
			 * We holddown for a few minutes to avoid count to
			 * infinity loops between EIGRP/RTMP redistribution.
			 */
			if (r && (r->holddown != 0) &&
			    (r->rpath.type == AT_PATH_IGRP)) {
			    neighbor->rtignoredrts++;
			    continue;
			}
			atrtmp_CreateEntry(p_tree,rngstart,rngend,dist,
					   atalkidb, neighbor);
			neighbor->rtnewrts++;
		    } else {
			if (   (dist == DDP_MAX_HOPCOUNT)
			    && (neighbor->port->atalk_compatibility))
			    neighbor->oldcisco = TRUE;
			neighbor->rtignoredrts++;
		    }
		} else {
		    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK,
				 atalkidb->phys_idb))
			buginf("\nAT: Bad route tuple (%d,%d,%d) from %a",
			       rngstart, rngend, dist, neighbor->address);
		    neighbor->rtbadrts++;
		}
	    }
	}
    }					/* end loop through routing tuples */

    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, atalkidb->phys_idb))
	buginf("\nAT: RTMP from %a (new %d,old %d,bad %d,ign %d, dwn %d)",
	       neighbor->address, neighbor->rtnewrts, neighbor->rtoldrts,
	       neighbor->rtbadrts, neighbor->rtignoredrts, 
	       neighbor->rtpoisonedrts);
    neighbor->badroutes += neighbor->rtbadrts;
    neighbor->newroutes += neighbor->rtnewrts;
}

/*
 * Externally visible routines start here.
 */


/*
 * atrtmp_SendUpdate 
 * send out routing table update broadcast on a specific net and idb
 *
 * Updates are generated from the _route_ tree, not the _path_ tree.
 * In this way, we get the information out to the rest of the net
 * on the best way to reach a particular destination without
 * having to derive it from the path tree. (which isn't kept in
 * "best path" notation and doesn't contain the information from
 * the IGRP half of the net).
 *
 * ??? This routine should be turned "inside out" so that we don't
 * have to walk the route tree twice.
 */
void
atrtmp_SendUpdate (atalkidbtype* idb,ushort net,long addr, uchar sock,
		   boolean dosplit)
{
    paktype	*pak;
    rtmptuple	*tuple;
    atalk_rdb_t *entry;
    zipnetentry *zn;
    rbTree	*tree;
    ushort	temp;
    rtmprsp	*r;
    ulong	nbytes;
    int		rtcnt;
    ushort	dnet;
    uchar	dnode, dsock;
    boolean	done;
    int		pkt;
    ulong	standby_interval = 0;
    ushort	remap_net;

/*
 * IF routing table is not empty THEN
 *     BEGIN
 * 	Copy the network number and distance pairs of each Good or Suspect
 * 	entry of the routing table whose distance < 15 into the routing tuple
 * 	fields of the RTMP data packet;
 * 
 *         FOR each bridge port DO
 * 	    IF the port is connected to an AppleTalk AND its network no. is
 * 		non-zero THEN
 * 		BEGIN
 * 		    Packet's sender network number := port network number;
 * 		    Packet's sender node ID := port node ID;
 *     		    Call DDP to broadcast the packet to the RTMP socket;
 *     		END
 * 	    ELSE
 * 		IF the port is not connected to an AppleTalk THEN
 * 		    BEGIN
 * 			Packet's sender network number := 0;
 * 			Packet's sender node ID := port node ID;
 *     			Call DDP to broadcast the packet through the port's
 * 			LAP to the RTMP socket;
 * 		    END;
 * 	END;
 *
 * Special note: If this update is for a FTZ, all routes are omitted
 * from the updates. Each update contains only information about 
 * the local cable.
 */
    if (!(idb->atalk_rp & ATALK_RTMP_ENABLED))
	return;

    if ((idb->atalk_rtmp_standby != 0) &&
	TIMER_RUNNING(idb->atalk_rtmp_sb_lasthrd)) {
	standby_interval = idb->atalk_rtmp_standby * ONESEC;
	if (CLOCK_IN_INTERVAL(idb->atalk_rtmp_sb_lasthrd, standby_interval)) {
	    return;			/* don't send updates while in sb */
	}
    }

    done = FALSE;

    if (addr == 0) {
	if (atalkif_net_extended(idb))
	    dnet = 0;
	else
	    dnet = atalkif_MyNet(idb);
	
	dnode = ATALK_BROAD_ADDR;
	addr = atalk_address(dnet,dnode);
	dsock = ATALK_SOCK_RTMP;
    } else {
	dnet = atalk_netonly(addr);
	dnode = atalk_nodeonly(addr);
	dsock = sock;
    }

    tree = atroute_GetTree();		/* route tree, not RTMP path tree */
    pkt = 0;
    entry = NULL;
    temp = 0;
    while (!done) {			/* loop while we have more routes */
	pak = atalk_pak_create(idb, addr, ATALK_SOCK_RTMP, 
			       dsock, DDP_TYPE_RTMPRsp, 
			       DDP_MAX_DATABYTES);
	if (pak == NULL) {
	    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) 
		buginf("\nAT: rtmp_SendUpdate getbuffer() failure");
	    atalk_stat[ATALK_NO_BUFFERS]++;
	    return;
	}
	if (addr == 0) {
	    pak->if_output = idb->phys_idb;
	    atalk_pak_setbroadcast(pak);
	}
	nbytes = pak->atalk_datalen;
	r = (rtmprsp *)pak->transport_start;
	if (atalkif_HalfRouter(idb) || atalkif_CaymanTunnel(idb))
	    PUTSHORT(r->srcnet, 0);
	else
	    PUTSHORT(r->srcnet, atalkif_MyNet(idb));
	r->idlen = 8;		
	if (atalkif_CaymanTunnel(idb))
	    r->id[0] = 0;
	else
	    r->id[0] = atalkif_MyNode(idb);
	tuple = (rtmptuple *)(((uchar *)r) + RTMPHDRBYTES);
	nbytes -= RTMPHDRBYTES;
	if (atalkif_net_extended(idb) && !atalkif_CaymanTunnel(idb)) {
	    PUTSHORT(tuple->net,idb->atalk_cablestart);
	    tuple->dist = RTMPTUPLERANGE;
	    nbytes -= RTMPTUPLEBYTES;
	    tuple = (rtmptuple*)(((uchar*)tuple) + RTMPTUPLEBYTES);
	    PUTSHORT(tuple->net,idb->atalk_cableend);
	    tuple->dist = RTMPTUPLEVERSION;
	    nbytes -= RTMPTUPLEBYTES;
	    tuple = (rtmptuple*)(((uchar*)tuple) + RTMPTUPLEBYTES);
	} else {
	    PUTSHORT(tuple->net,0);
	    tuple->dist = RTMPTUPLEVERSION;
	    nbytes -= RTMPTUPLEBYTES;
	    tuple = (rtmptuple*)(((uchar*)tuple) + RTMPTUPLEBYTES);
	}
	rtcnt = 0;
	while (nbytes >= (RTMPTUPLEBYTES*2)) {
	    /*
	     * If FTZ or RTMP stub, just send a null body for update.
	     */
	    if (idb->atalk_rtmp_stub || atalk_ftzPortCheck(idb)) {
		done = TRUE;
		break;
	    }
	    entry = rtmp_GetNextRoute(tree, idb, TRUE, dosplit, entry);
	    if (entry == NULL) {	/* no more routes */
		done = TRUE;
		break;
	    }

	    /*
	     * We will now construct the update with the path that
	     * is currently the best (ie, top) path on the list.
	     */


	    /* Is this entry our outgoing interface?  Skip it. */
	    if (entry->dstrngstr == idb->atalk_cablestart)
		continue;

	    if (idb->atalk_route_filter && 
		!atalk_routecheck(entry->dstrngstr, entry->dstrngend, 
				  idb->atalk_route_filter))
	        continue;

	    zn = atalk_firstZone(entry);

	    /* If no zones for this route are known and
	     * 'require-route-zones' is active, skip this route.
	     */
	    if ((zn == NULL) && atalk_routeZones)
		continue;

	    if (idb->atalk_route_filter) {
		if (atalk_permitPartialZones) {
		    while (zn != NULL) {
			if (atalk_rtmpzonecheck(zn->zone,
						idb->atalk_route_filter))
			    break;
			zn = atalkzn_nextZone(zn);
		    }
		    if (zn == NULL)
			continue;
		} else {
		    while (zn != NULL) {
			if (!atalk_rtmpzonecheck(zn->zone,
						 idb->atalk_route_filter))
			    break;
			zn = atalkzn_nextZone(zn);
		    }
		    if (zn != NULL)
			continue;
		}
	    }

	    /* Check for ZIP Reply filtering.  If all zones for this route
             * are denied, then skip this route.  Prevents ZIP storms.
	     */
	    if (idb->atalk_zipreply_filter) {

		for (zn = atalk_firstZone(entry); zn != NULL;
		     zn = atalkzn_nextZone(zn)) {

		    /* If at least one zone is permitted, we can stop
		     * checking.
		     */
		    if (atalk_zonecheck(zn->zone, idb->atalk_zipreply_filter))
			break;
		}
		if (zn == NULL)
		    continue;		/* All zones were denied, so skip. */
	    }

	    rtcnt++;



	    PUTSHORT(tuple->net,entry->dstrngstr);
	    

	    if(atalk_DomainRouter && idb->atalk_dminfo && idb->atalk_dminfo->out) {
		remap_net = atdomain_GetRemapOut(idb, entry->dstrngstr);
		if(remap_net)
		    PUTSHORT(tuple->net,remap_net);
		else {
		    rtcnt--;
		    continue;
		}
	    }

	    /*
	     * If the route is valid, put the metric into the tuple.
	     * If the route isn't valid, (eg, we don't have an actual
	     * path, the state isn't at least as good as suspect, etc,
	     * then poison the route to neighbors.
	     *
	     * Routes to proxy networks shouldn't be considered.
	     */
	    if (rtmp_IsValid(entry->best_path)) {
		
		tuple->dist = atrtmp_GetRouteMetric(entry);
		
		if (!atalk_net_proxy(entry)) {
		    at_Domain *domain_src, *domain_dst;

		    domain_src = entry->rpath.idb->atalk_dminfo;
		    domain_dst = idb->atalk_dminfo;		
		    
		    if (domain_src != domain_dst) {
			
			if ((domain_src && domain_src->hop_rdctn) ||
			    (domain_dst && domain_dst->hop_rdctn))
			    if (tuple->dist != 0)
				tuple->dist = 1;
			else if ((domain_src && domain_src->hop_prsrv) ||
				 (domain_dst && domain_dst->hop_prsrv)) {
			    if (entry->rpath.metric.u.igrp.external)
				tuple->dist = 
					entry->rpath.metric.u.igrp.ext_hops + 1;
			    else
				tuple->dist = 1;
			}
		    }
		}
	    } else {
		tuple->dist = (idb->atalk_compatibility) ? DDP_MAX_HOPCOUNT
                                                         : DDP_POISON_HOPCOUNT;
	    }
	    if (atalk_net_extended(entry) &&
		!(idb->atalk_compatibility &&
		  (entry->dstrngstr == entry->dstrngend))) {
		tuple->dist |= RTMPTUPLERANGE;
		nbytes -= RTMPTUPLEBYTES;
		tuple = (rtmptuple*)(((uchar*)tuple) + RTMPTUPLEBYTES);
 		tuple->dist = RTMPTUPLEVERSION;

		PUTSHORT(tuple->net, entry->dstrngend);

		if(idb->atalk_domain && idb->atalk_dminfo) {
		    remap_net = atdomain_FindForwardRemap(idb->atalk_dminfo->out, 
							  entry->dstrngend);
		    if (remap_net)
			PUTSHORT(tuple->net, remap_net);
		}
	    }
	    nbytes -= RTMPTUPLEBYTES;
	    tuple = (rtmptuple *)(((uchar *)tuple) + RTMPTUPLEBYTES);
	}
	if (rtcnt || (pkt == 0)) {
	    atalk_pak_setlength(pak,(pak->atalk_datalen - nbytes));
	    nbytes = pak->atalk_datalen;
	    atalk_DDPcksum(pak); 
	    if (reg_invoke_snapshot_update_denied(idb->phys_idb, FALSE)) {
		datagram_done((paktype *) pak);
	    } else {
		reg_invoke_snapshot_notice_update_pak(pak, idb->phys_idb);
		if (atalk_send(pak)) {
		    atalk_stat[ATALK_RTMP_OUT]++;
		    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
			|| at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
			buginf("\nAT: src=%s, dst=%s, size=%d, %d rte%s, RTMP pkt sent",
			       atalkif_nodename(idb),
			       atalkif_netname(idb),
			       nbytes,
			       ArgAndPlural(rtcnt,"","s"));
		    }
		    pkt++;
		}
	    }
	} else {
	    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) 
		buginf("\nAT: suppressing null update for net=%d",net);
	    datagram_done(pak);
	}
    }
}

/*
 * atrtmp_SendUpdates
 * send out routing table updates
 */
void
atrtmp_SendUpdates (void)
{
    atalkidbtype *idb;
    sys_timestamp	update_begin;

    /*
     * ??? When we're all done with the connected path stuff,
     * don't walk the SWIDB chain anymore; walk the connected path tree.
     */
    GET_TIMESTAMP(update_begin);
    FOR_ALL_ATIDBS(idb) {
	if (   atalkif_operational(idb)
	    && (idb->atalk_rp & ATALK_RTMP_ENABLED)) {
	    if (idb->atalk_sendroutes) {
		atrtmp_SendUpdate(idb, atalkif_MyNet(idb), 0, 0,
				  idb->atalk_rtmp_split_horizon);
		process_may_suspend();
	    }
	}
    }
    current_ticks.update_total = ELAPSED_TIME(update_begin);
    if (highest_ticks.update_total < current_ticks.update_total) {
	highest_ticks.update_total = current_ticks.update_total;
    }
}

/*
 * rtmp_AcquirePortInfo
 * grab info needed to startup
 */
static void
rtmp_AcquirePortInfo (paktype* pak)
{
    rtmprsp* rsp;
    rtmptuple* rt;
    ushort snet,enet;
    uchar dist;
    idbtype *phys_idb;
    atalkidbtype* idb;
    uchar pt;
    int nb;
    fddi_hdr	*fddi;
    uchar	fddi_dst[IEEEBYTES];

    phys_idb = pak->if_input;
    idb = atalk_getidb(phys_idb);
    pt = rtmp_ValidPacket(pak);
    if (pt != DDP_TYPE_RTMPRsp)
	return;

    nb = pak->atalk_datalen;
    if (pak->linktype != LINK_APPLETALK)
	return;
    if (atalkif_net_extended(idb)) {
	rt = (rtmptuple*)(((uchar*)pak->transport_start) + RTMPHDRBYTES);
	nb -= RTMPHDRBYTES;
	if (!atrtmp_GetNextTuple(&rt,&nb,&snet,&enet,&dist,RTMPTUPLEVERSION))
	    return;
	/*
	 * Check to see if we've been booted into a pre-FDDItalk ring.
	 */
	if (idb_is_fddi(phys_idb)
	    && (phys_idb->hwptr->atalk_compat_flags != AT_COMPAT_FDDI)) {
	    fddi = (fddi_hdr *)(pak->datagramstart - FDDI_SNAP_ENCAPBYTES);
	    ieee_swap(fddi->daddr, fddi_dst);
	    if (ieee_equal(fddi_dst, apple_old_fddi_broadcast)) {
		errmsg(AT_ERR_FDDICOMPAT, atalk_pak_srcaddr(pak));
		if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		    buginf("\nAT: %s heard an old FDDI broadcast of RTMP",
			   idb_get_namestring(idb->phys_idb));
	    }
	}
    } else {
	rsp = (rtmprsp*) pak->transport_start;
	snet = GETSHORT(rsp->srcnet);
	enet = 0;
    }
    atalkif_CableInfoReport(idb,snet,enet,atalk_pak_srcaddr(pak));
}

/* 
 * rtmp_UpdateEntry
 * update routing table entry (now a path) from rtmp packet
 */
void
rtmp_UpdateEntry (atalk_pdb_t* p, uchar dist, atalkidbtype *idb,
		  neighborentry* n)
{
    ulong	old_metric;

    /*    IF (Entry's State == Bad) AND (tuple distance < 15) THEN
     *	      Replace-Entry
     *    ELSE IF Entry's distance >= (tuple distance+1) AND 
     *            (tuple distance < 15) THEN
     *	      Replace-Entry
     *	  ELSE IF Entry's next IR == data packet's sender node ID AND
     *	          Entry's port == P THEN
     *        IF tuple distance <> 31 THEN BEGIN
     *	      {If entry says we're forwarding to the IR who sent us this
     *         packet, the net's now further away than we thought}
     *	          Entry's distance := tuple distance + 1;
     *		  IF entry's distance < 16 THEN Entry's state := Good
     *		  ELSE Delete the entry
     *	      END;
     *        {Notify neighbor so force the state to BAD}
     *        ELSE
     *            Entry's state := Bad
     */
    old_metric = atrtmp_GetPathMetric(p);
    
    if ((p->state >= at_state_bad) && (dist < DDP_MAX_HOPCOUNT)) {
	/* CASE 1: Current path is bad but new tuple is good. */

	if ( p->gwptr != n) {
	    /* Current path has been used and new tuple was learned
	     * from a different neighbor, so flush the fast switching
	     * cache.
	     */
	    atalk_InvalidateCache("rtmp_UpdateEntry - path chg");
	}

	if (p->rdb->rtmp_equalcount)
	    atroute_DeleteEqualPathList(p);

	rtmp_ReplaceEntry(p, dist, idb, n);

    } else {
	/* Current path is not bad. */

	if (atrtmp_GetPathMetric(p) >= (dist+1) && dist < DDP_MAX_HOPCOUNT) {
	    /* CASE 2: Current path is no better or even possibly worse
	     *	       than new tuple, which has a valid distance.
	     */

	    if (atrtmp_GetPathMetric(p) != (dist + 1)) {
		/* Current path is in fact worse than new tuple, so
		 * flush route cache.
		 */
		atalk_InvalidateCache("rtmp_UpdateEntry - path chg");

	        if (p->rdb->rtmp_equalcount)
	            atroute_DeleteEqualPathList(p);

	    }
	    else {
	        /*
		 * For load balancing, we don't want to flop
		 * between parallel interfaces, just update
		 * the equal path
		 */
	        if (p->idb->phys_idb != idb->phys_idb && p->rdb->rtmp_equalcount) {
		    rtmp_UpdateEqualPath(p, idb->phys_idb, n);
		    return; 
		}
	    }

	    rtmp_ReplaceEntry(p, dist, idb, n);

            /* Done if same route came in from same router */
            if (p->gwptr && (p->gwptr == n) &&
                (p->metric.u.rtmp.hops == dist + 1)) {

                /* Only the route timer really needs to be updated, but
                   it doesn't hurt too much to update everything */
                atroute_UpdateRoute(p->rdb,p);
                return;
	    }

	} else {
	    /* Current path has better hopcount than new tuple.  We'll
             * ignore the new tuple, unless it was heard from the same
             * neighbor.
             */

	    if ((p->gwptr == n) && (p->gwptr != NULL)) {
		/* Unforunately, new tuple was heard from the same 
		 * neighbor as the current path which means:
		 * CASE 3: Current path just got farther away. 
		 */

		if (dist != DDP_POISON_HOPCOUNT) {
		    /* New tuple isn't poisoned. */

		    if (atrtmp_GetPathMetric(p) < DDP_MAX_HOPCOUNT) {
			/* Current path's hopcount can be incremented. */
	
			    atalk_InvalidateCache("rtmp_UpdateEntry, "
						  "path worse");

			    if (p->rdb->rtmp_equalcount)
			        atroute_DeleteEqualPathList(p);

			/* Note: this call will bump the current path's
			 * hopcount by one.
			 */
			rtmp_ReplaceEntry(p, old_metric, idb, n);

		    } else {
			/* Current path's hopcount cannot be incremented,
                         * so this means:
			 * CASE 3b: Path went to infinity, make it go away. 
			 */
			atroute_DeletePath(p);
		    }

		} else {
		    /* New tuple is poisoned. */

		    if (p->state < at_state_bad) {

			/* CASE 4: Remember 1st notification from neighbor. */
		        if (p->rdb->rtmp_equalcount)
			    atroute_DeleteEqualPathList(p);
			atrtmp_SetMetric(&p->metric, DDP_POISON_HOPCOUNT);
			atalk_rtmp_set_timer(p);
			if (p->path_action)
			    (*p->path_action)(p, at_state_bad);
			if (old_metric != dist + 1)
			    atroute_NewPathMetric(p->rdb, p);

		    }
		}
	    }
	}
    }
    atroute_BestPath(p);		/* recompute best path */
}

/*
 * atrtmp_CreateEntry
 *
 * Create an RTMP path entry.
 *
 * Note: 'tree' is the rtmp path tree.
 */
void
atrtmp_CreateEntry (rbTree *tree,
		    ushort rngstart, ushort rngend, uchar dist,
		    atalkidbtype *idb, neighborentry* n)
{
    atalk_pdb_t *p;
    atalk_metric_t	metric;

    memset(&metric, 0, sizeof(metric));
    metric.type = AT_PATH_RTMP;
    atrtmp_SetMetric(&metric, dist + 1);
    p = atroute_AddPath(tree, idb, rngstart, rngend, n,
			atrtmp_SetPathState, NULL,
			0, AT_PATH_RTMP, &metric);
    if (p == NULL)
	return;
    if (dist == DDP_POISON_HOPCOUNT) {
	atrtmp_SetMetric(&p->metric, DDP_POISON_HOPCOUNT);
	GET_TIMESTAMP(p->timer);
	atrtmp_SetPathState(p, at_state_bad);
	return;
    }
    rtmp_ReplaceEntry(p, dist, idb, n);
}

/*
 * this should probably be renamed to "update entry"
 */
static void
rtmp_ReplaceEntry (atalk_pdb_t* p, uchar dist, atalkidbtype *idb,
		   neighborentry* n)
{
    boolean	changed_metric = FALSE;
    boolean	worse_metric = FALSE;

    /* Entry's next IR := packet sender's ID;  IAT Page 5-23 */
    if (p->gwptr != n) {
	if (p->gwptr != NULL)
	    atmaint_UnprotectNeighbor(p->gwptr);
	atmaint_ProtectNeighbor(n);
	p->gateway = n->address;
	p->gwptr = n;

	/* Entry's port number := P;  IAT Page 5-23 */
	p->idb = idb;	/* n->port; */
    }
    atalk_rtmp_set_timer(p);

    /*
     * The bible says, "replace the entry even if metric is the same for 
     * two routes", this means we flop between redundant paths.  For fast
     * switching, this would constantly invalidate the cache so...
     * If the route changes to a different gateway but the metric is the
     * same, suppress invalidating the cache.  This is safe because if ANY
     * gateway goes away, we invalidate the cache (handled via neighbor code)
     */
    if (atrtmp_GetPathMetric(p) != (dist + 1)) {
	changed_metric = TRUE;
    }
    if (p->metric.u.rtmp.hops < (dist + 1)) {
	worse_metric = TRUE;
    }

    /* Entry's distance := tuple's distance + 1;  IAT Page 5-23 */
    atrtmp_SetMetric(&p->metric, dist + 1);
    
    /* Entry's state := Good;  IAT Page 5-23 */
    if (p->path_action)
	(*p->path_action)(p, at_state_good);

    if (changed_metric) {
      	if (p->rdb->rtmp_equalcount)
	    atroute_DeleteEqualPathList(p);
	atroute_NewPathMetric(p->rdb, p);
    }
    /*
     * If we are WORSE, DUAL might have something to say about this...
     */
    if (worse_metric) {
	atroute_validate_all(p->dstrngstr, p->dstrngend);
    }
}

void
atrtmp_redist_all (boolean distflag)
{
    rbTree	*tree;

    tree = atroute_GetPTree(AT_PATH_RTMP);
    if (tree == NULL)
	return;

    RBTreeForEachNode(atroute_path_redist_rbshell, (void *)distflag,
		      tree, FALSE);
}

/* 
 * atrtmp_Initialize
 * perform startup initializations for rtmp code
 */
void
atrtmp_Initialize (void)
{
    mgd_timer_init_parent(&atalk_rtmp_timer, NULL);
    mgd_timer_init_leaf(&rtmp_pathAgerTimer, &atalk_rtmp_timer,
			ATTIMER_RTMP_PATH, NULL, FALSE);
    mgd_timer_init_leaf(&rtmp_pathGCTimer, &atalk_rtmp_timer, ATTIMER_RTMP_GC,
			NULL, FALSE);

    atroute_InitPathTree(AT_PATH_RTMP, "AT RTMP Path Tree");
    atroute_RegisterProtocol(AT_PATH_RTMP,
			     AT_ROUTE_LEAK_RTMP,
			     AT_ROUTE_UNLEAK_RTMP,
			     AT_ROUTE_LEAKED_RTMP,
			     NULL, NULL,
			     atrtmp_redist_all,
			     NULL);
    reg_add_atalk_age_neighbor(rtmp_AgeNeighbor, "");
}

static void
atalk_rtmprouter_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&atalk_rtmp_timer, DISABLE);
    process_watch_queue(atalkrtmpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalkrtmpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalkrtmpQ);
    atalk_rtmp_pid = 0;
}

/*
 * atalk_RtmpRouter
 * Appletalk RTMP process.
 * Maintain routing tables and answer queries.
 *
 * Aging the routing RTMP portion of the table is done by walking
 * the list of RTMP paths. The list of paths is kept here in the
 * RTMP module.
 */
process
atalk_RtmpRouter (void)
{
    paktype	*pak;
    neighborentry	*neighbor = NULL;
    ushort		srcnet;
    ataddrtype		srcaddr;
    uchar		srcnode;
    sys_timestamp	pass_start;	/* clock @ start of pass */
    atalkidbtype 		*idb;
    ulong               major, minor;
    mgd_timer          *expired_timer;
    
    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, atalk_rtmprouter_teardown);
    atalkrtmpQ = create_watched_queue("RTMP packets", 0, 0);
    process_watch_queue(atalkrtmpQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&atalk_rtmp_timer, ENABLE);
    
    atalk_BindSocket(atalkrtmpQ, "RTMP", ATALK_SOCK_RTMP, NULL);

    mgd_timer_start(&rtmp_pathGCTimer, 0);
    mgd_timer_start(&rtmp_pathAgerTimer, atalk_rtmpUpdateTime);

    while (TRUE) {
	process_wait_for_event();

	GET_TIMESTAMP(pass_start);

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		while (mgd_timer_expired(&atalk_rtmp_timer)) {
		    expired_timer = mgd_timer_first_expired(&atalk_rtmp_timer);
		    switch (mgd_timer_type(expired_timer)) {
		    case ATTIMER_RTMP_PATH:
			rtmp_pathAger();
			current_ticks.path_time = ELAPSED_TIME(pass_start);
			mgd_timer_start(expired_timer, atalk_rtmpUpdateTime);
			break;
		    case ATTIMER_RTMP_GC:
			atroute_pathGC(AT_PATH_RTMP, "RTMP");
			current_ticks.gc_time = ELAPSED_TIME(pass_start);
			mgd_timer_start(expired_timer, atalk_rtmpGCInterval);
			break;
		    default:
			mgd_timer_stop(expired_timer);
			break;
		    }
		}
		break;
	    case QUEUE_EVENT:
		/* while packets... */
		while ((pak = process_dequeue(atalkrtmpQ)) != NULL) {
		    /* dequeue then checksum, but no DDP type check */
		    if (!pak ||
			!atalk_check(pak, DDP_TYPE_Invalid,ATALK_RTMP_IGNORED,
				     FALSE))
			continue; /* noone home, checksum bad or interf down */

		    idb = atalk_getidb(pak->if_input);
	    
		    /*
		     * This is something of a hack which should be thought
		     * out more thouroughly. If the packet came in on
		     * an interface which does not have RTMP enabled, pitch
		     * the packet and roll on. ???
		     */
		    if (!(idb->atalk_rp & ATALK_RTMP_ENABLED)) {
			datagram_done(pak);
			continue;
		    }
		    if (!atalkif_operational(idb))
			rtmp_AcquirePortInfo(pak);

		    if (atalkif_operational(idb) ||
			(atalkif_ErrForPeers(idb) == NULL)) {
			atalk_stat[ATALK_RTMP_IN]++;
			srcnet  = atalk_pak_snet(pak);
			srcnode = atalk_pak_snode(pak);
			srcaddr = atalk_address(srcnet, srcnode);
			if (atalk_pak_getpt(pak) == DDP_TYPE_RTMPReq) 
			    rtmp_HandleRequest(pak, srcaddr);
			else if (atalk_pak_getpt(pak) == DDP_TYPE_RTMPRsp) {
			    neighbor = rtmp_GetNeighbor(pak);
			    if (neighbor != NULL) {
				rtmp_HandleResponse(pak, neighbor);
			    }
			} else {
			    atalk_stat[ATALK_RTMP_BAD]++;
			    atalk_pak_printfail(pak, atalkrtmp_debug,
						"Not Req or Rsp");
			}
		    }
		    datagram_done(pak);
		    process_may_suspend();
		}
		current_ticks.pak_time = ELAPSED_TIME(pass_start);
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
	
	current_ticks.pass_total = ELAPSED_TIME(pass_start);
	if (highest_ticks.pass_total < current_ticks.pass_total) {
	    highest_ticks.pass_total = current_ticks.pass_total;
	    highest_ticks.pak_time   = current_ticks.pak_time;
	    highest_ticks.path_time  = current_ticks.path_time;
	    highest_ticks.gc_time    = current_ticks.gc_time;
	}
    }
}

static boolean
rtmp_IsValid (atalk_pdb_t *p)
{
    if (p == NULL) return (FALSE);

    if (p->state != at_state_good && p->state != at_state_suspect)
	return (FALSE);

    if ((!p->idb || !atalkif_operational(p->idb)) && !p->internal)
	return (FALSE);

    if (atrtmp_GetPathMetric(p) > DDP_MAX_HOPCOUNT)
	return (FALSE);

    return (TRUE);
}

/*
 * rtmp_UpdateNeighborEntry
 *
 * Update the RTMP fields in a neighbor entry.
 */
static void
rtmp_UpdateNeighborEntry (neighborentry* n, atalkidbtype* idb,
			  boolean goodpacket, boolean forceaging)
{
    sys_timestamp now;

    GET_TIMESTAMP(now);

    n->port = idb;
    n->snapshot_no_age = 
	(!forceaging &&
	 reg_invoke_snapshot_aging_denied(n->port->phys_idb));

    COPY_TIMESTAMP(now, n->lastheard);
    COPY_TIMESTAMP(now, n->rtmp_lastheard);

    n->rtextendedrts = 0;
    n->rtnonextendedrts = 0;
    n->rtnewrts = 0;
    n->rtoldrts = 0;
    n->rtignoredrts = 0;
    n->rtbadrts = 0;
    n->rtpoisonedrts = 0;

    if (!goodpacket) {
	n->rtpacketsbad++;
	return;
    }
    n->rtpacketsok++;
    if (n->rtmp_state == neighborOverdue) {
	n->rtmp_overdues++;
    } else if (n->rtmp_state == neighborDown) {
	atmaint_RestartNeighbor(n, now);
    }
    /*
     * Restart the RTMP neighbor expiry timer.
     */
    TIMER_START(n->rtmp_exptimer, atalk_rtmpValidTime);
    n->state = n->rtmp_state = neighborUp;
}

/*
 * atrtmp_SetPathState
 * Set the state of this path as requested.
 *
 * A pointer to this function is installed on all RTMP paths created by this
 * module.
 */
static boolean
atrtmp_SetPathState (atalk_pdb_t* p,  at_state_t state)
{
    atalk_rdb_t	*r;
    at_state_t	ostate;
    char const *idb_name;
    
    if (p == NULL || p->type != AT_PATH_RTMP)
	return (FALSE);

    idb_name = idb_get_namestring(p->idb->phys_idb);
    ostate  = p->state;
    r = p->rdb;

    /*
     * Unredistributing an already unredistributed route has no ill effect
     * We want to cover ALL cases of a redistributed path needing
     * unredistribution.
     */
    if (state >= at_state_bad) {
	atroute_path_unredistribute(p);
    }
    switch (state) {
      case at_state_good:
	if (p->state >= at_state_bad) {
	    ATASSERT(atrtmp_GetPathMetric(p) <= DDP_MAX_HOPCOUNT);
	    if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK)
		|| at_debug(atalk_events, ATALK_NULL_MASK))
		if (p->gwptr != NULL)
		    errmsg(AT_ERR_RTMPSTATE,
			   idb_name,
			   atalk_CableRange(r),
			   p->gwptr->address, 
			   atalk_state_name(state),
			   p->metric.metric, atrtmp_GetPathMetric(p));
	}
	TIMER_STOP(p->wentbad);
	/* drops into suspect */
	
      case at_state_suspect:
	p->state = state;
	/* Similarly, bring it back to redistribution if previously gone */
	if ((state <= at_state_suspect) && (ostate >= at_state_bad)) {
	    atroute_path_redistribute(p);
	}
	break;

      case at_state_bad:
	if (p->state >= at_state_bad)
	    return (FALSE);		/* already at least bad */
	p->state = state;
	GET_TIMESTAMP(p->wentbad);
	if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK)
	    || at_debug(atalk_events, ATALK_NULL_MASK)) {
	    if (atrtmp_GetPathMetric(p) >= DDP_POISON_HOPCOUNT)
		errmsg(AT_ERR_PATHNOTIFY,
		       idb_name,
		       atroute_rp_type(p->type),atalk_CableRange(p->rdb),
		       p->gateway);
	    else  {
		if (p->gwptr != NULL)
		    errmsg(AT_ERR_RTMPSTATE,
			   idb_name,
			   atalk_CableRange(r),
			   p->gwptr->address, 
			   atalk_state_name(state),
			   p->metric.metric, atrtmp_GetPathMetric(p));
	    }
	}
	if (atrtmp_GetPathMetric(p) < DDP_POISON_HOPCOUNT) {
	    atrtmp_SetMetric(&p->metric, DDP_POISON_HOPCOUNT);
	    atroute_NewPathMetric(p->rdb, p);
	}
	atalk_InvalidateCache("atroute_SetPathState");
	break;

	/*
	 * Called in the case where the path is about to be deleted...
	 */
      case at_state_delete:
	break;

      default:
	if (at_debug(atalkrtmp_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Bad state for RTMP path on route %#A",
		   atalk_CableRange(r));
	break;
    }
    if (p->state != ostate) {
	if (at_debug(appletalk_debug, ATALK_NULL_MASK)
	    && at_debug(atalkrtmp_debug, ATALK_NULL_MASK))
	    buginf("\nAT: State change for %#A: %s -> %s",
		   atalk_CableRange(r), atalk_state_name(ostate),
		   atalk_state_name(p->state));
	p->action = at_change_path;
    }
    atroute_BestPath(p);
    /* If it came from Notify Neighbor or Deleted state,
       then assume the new route and therefore delete 
       the zone list
     */  
    if(ostate > at_state_bad && !atalk_net_connected(p->rdb))
	atmaint_ResetRequestInfo(p->rdb);

    return (TRUE);
}

/*
 * rtmp_UpdateEqualPath -
 *
 */
static void
rtmp_UpdateEqualPath (atalk_pdb_t *p, idbtype *phys_idb, neighborentry *n)
{
    atalk_pdb_t *next;
    short count;

    next = p->equalpath;
    count = atroute_GetEqualPathCount(p);
    while (count--) {
        if (next->idb->phys_idb == phys_idb) {
	    if (next->gwptr != n) {
	        if (next->gwptr != NULL)
		    atmaint_UnprotectNeighbor(next->gwptr);
		atmaint_ProtectNeighbor(n);
                next->gateway = n->address;
	        next->gwptr = n;
            }
            next->state = at_state_good;
	    atalk_rtmp_set_timer(next);
	    break;
	}
	next = next->equalpath;
    }
}


static void
rtmp_AgeEqualPaths (atalk_pdb_t *p)
{
    atalk_pdb_t *next, *prev;
    short count;
    ulong age;

    count = p->rdb->rtmp_equalcount;
    if (!count)
        return;

    if ((count + 1) > atalk_MaxPaths) {
        atalk_InvalidateCache("rtmp_AgeEqualPaths");
        atroute_DeleteEqualPathList(p);
	return;
    }

    prev = p;
    next = p->equalpath;
    while (count--) {
        age = ELAPSED_TIME(next->timer);
	if ((age > atalk_rtmpInvalidTime && next->state >= at_state_bad) || 
	    !atalkif_InterfaceUp(next->idb)) {
	    atalk_InvalidateCache("rtmp_AgeEqualPaths");
	    if (p->rdb->next_equalpath == next)
	        p->rdb->next_equalpath = next->equalpath;
	    prev->equalpath = next->equalpath;
	    prev = next;
	    next = next->equalpath;
	    if (prev->gwptr)
	        atmaint_UnprotectNeighbor(prev->gwptr);
	    prev->equalpath = NULL;
	    free(prev);
	    p->rdb->rtmp_equalcount--;
	    atalk_equalpathFreed++;
	    continue;
	}
	if (!next->snapshot_no_age) {
	    if (age > atalk_rtmpValidTime) {
	        if (next->state < at_state_bad) {
	            next->state = at_state_bad;
	            atalk_InvalidateCache("rtmp_AgeEqualPaths");
	    
	            if (at_debug(atalklb_debug, ATALK_NULL_MASK))
	                buginf("Aging %u-%u from %s\n", next->dstrngstr,
                               next->dstrngend, 
			       idb_get_namestring(next->idb->phys_idb));
	        }
	    }
	    else {
	        if ((age > atalk_rtmpUpdateTime) && 
		    (next->state < at_state_bad))
	            next->state = at_state_suspect;
	    }
        }
	prev = next;
	next = next->equalpath;
    }
}

static boolean
rtmp_AgePath (treeLink *link, void *pdata)
{
    atalk_pdb_t	*p = (atalk_pdb_t *)link;
    ulong	age;

    ATASSERT(p != NULL);
    if (p == NULL)
	return (FALSE);
    if (p->rdb->rtmp_equalcount)
        rtmp_AgeEqualPaths(p);
    age = ELAPSED_TIME(p->timer);
    if (((age > atalk_rtmpInvalidTime) && TIMER_RUNNING(p->wentbad))) {
	atroute_DeletePath(p);
	return (TRUE);
    }
    if (!p->snapshot_no_age) {
	if (age > atalk_rtmpValidTime) {
	    atrtmp_SetPathState(p, at_state_bad);
	} else if ((age > atalk_rtmpUpdateTime) && (p->state < at_state_bad)) {
	    atrtmp_SetPathState(p, at_state_suspect);
	}
    }
    return (TRUE);
}

static void
rtmp_pathAger (void)
{
    rbTree	*p_tree;

    p_tree = atroute_GetPTree(AT_PATH_RTMP);
    if (p_tree)
	RBTreeForEachNode(rtmp_AgePath, NULL, p_tree, FALSE);
}

/*
 * GetNextRoute
 *
 * This routine should simply retrieve a route for a network number;
 * the calling routine, once having done so, can figure out the
 * derivation of the route and figure if it needs to walk the path list
 * to figure out how to leak.
 *
 * I'm assuming for the time being that this routine will be called from
 * the routing update generation routines/processes.
 *
 * parameters:
 * tree		-- the routing tree from which to extract the info.
 * idb		-- idb to match. must be passed.
 * fullupdate   -- TRUE if we should return a route which hasn't changed
 *		   recently
 * dosplit	-- TRUE is split-horizon processing should be done.
 *		   (against 'idb')
 * prev		-- a rdb pointer previously returned from this routine.
 */
static atalk_rdb_t*
rtmp_GetNextRoute (rbTree *tree, atalkidbtype *idb,boolean fullupdate,
		   boolean dosplit,atalk_rdb_t* prev)
{
    atalk_rdb_t	*r;
    atalkidbtype *r_idb;			/* route's idb */
    boolean	connected = FALSE;
    extern boolean at_testroute(ushort);

    if (idb == NULL)
	return (NULL);

    for (r = KeepNextRoute(tree,prev); r != NULL; r = KeepNextRoute(tree,r)) {
	connected = atalk_net_connected(r);
	r_idb = r->rpath.idb;
	if (!(r->rpath.type == AT_PATH_RTMP || (r->rpath.type == 
	    AT_PATH_STATIC && !at_testroute(r->dstrngstr)))
 	    || idb != r_idb || !dosplit) {
	    if (!fullupdate && !r->changed)
		continue;
	    if (connected && (!rtmp_NetIsValid(r)))
		continue;

	    /* Split Horizon based on domain */
	    if(atalk_DomainRouter_running)
		if(atdomain_FilterNet(idb,r->dstrngstr,ToDomain))
		    continue;

	    return (r);
	}
    }
    return (NULL);
}

/*
 * rtmp_NetIsValid
 * Check state of entry, returning TRUE if considered valid
 *
 */
boolean
rtmp_NetIsValid (atalk_rdb_t* r)
{
    if (r == NULL)
	return (FALSE);

    if (atalk_net_connected(r)) {
	if (r->rpath.metric.metric == AT_METRIC_PROXY) {
	    return(TRUE);
	}
	return (atalkif_operational(r->rpath.idb));
    }

    if (atrtmp_GetRouteMetric(r) > DDP_MAX_HOPCOUNT)
	return (FALSE);
    return (TRUE);
}

/*
 * atrtmp_show_private
 *
 * Called from the "show appletalk private" path.
 */
void
atrtmp_show_private (void)
{
    sys_timestamp	now;

    GET_TIMESTAMP(now);
    printf("\n  Atalk/RTMP private information"
	   "\n  CPU times in AT/RTMP process:"
	   "\n    Lass pass: total time:  %-8u"
	   "\n               pak time:    %-8u"
	   "\n               path time:   %-8u"
	   "\n               gc time:     %-8u"
	   "\n    High pass: total time:  %-8u"
	   "\n               pak time:    %-8u"
	   "\n               path time:   %-8u"
	   "\n               gc time:     %-8u",
	   current_ticks.pass_total,
	   current_ticks.pak_time,
	   current_ticks.path_time,
	   current_ticks.gc_time,
	   highest_ticks.pass_total,
	   highest_ticks.pak_time,
	   highest_ticks.path_time,
	   highest_ticks.gc_time);

    printf("\n  CPU time to generate routing updates:"
	   "\n    Lass pass: total time:    %-8u"
	   "\n    High pass: total time:    %-8u",
	   current_ticks.update_total,
	   highest_ticks.update_total);

}

/*
 * rtmp_StartAgePath
 * Start the aging on a path if the path is using the idb specified
 */
static boolean
rtmp_StartAgePath (treeLink *link, void *pdata)
{
    atalk_pdb_t	*p = (atalk_pdb_t *)link;
    rtmp_aging_t *rtmp_aging_params = (rtmp_aging_t *) pdata;

    ATASSERT(p != NULL);
    if (p == NULL)
	return (FALSE);
    if (TIMER_RUNNING(p->wentbad))
	return (TRUE);
    if (!p->snapshot_no_age)
        return (TRUE);
    if (p->idb == rtmp_aging_params->idb) {
	if ((rtmp_aging_params->pak == NULL) ||
	    (rtmp_aging_params->n == p->gwptr)) {
	    p->snapshot_no_age = FALSE;
	    GET_TIMESTAMP(p->timer);
	}
    }
    return (TRUE);
}

/*
 * atrtmp_start_aging
 * Start aging RTMP routes and neighbors learned on this interface
 */
void atrtmp_start_aging (atalkidbtype *idb, paktype *pak)
{
    rtmprsp *rsp;
    ataddrtype srcaddr;
    ushort srcnet;
    neighborentry  *n, *pak_neighbor;
    rtmp_aging_t rtmp_aging_params;
    rbTree	*p_tree;

    if (pak != NULL) {
	rsp = (rtmprsp*) pak->transport_start;
	srcnet = GETSHORT(rsp->srcnet);
	srcaddr = atalk_address(srcnet, rsp->id[0]);
	pak_neighbor = atmaint_FindNeighbor(srcaddr,
					    atalk_getidb(pak->if_input));
    } else {
	pak_neighbor = NULL;
    }

    if (!atalk_running)
	return;

    /*
     * Age this neighbor or all on IDB. The nested IFs are required for the
     * correct logic.
     */
    if (pak != NULL) {
	if (pak_neighbor != NULL) {
	    rtmp_UpdateNeighborEntry(pak_neighbor, pak_neighbor->port, TRUE,
				     TRUE);
	}
    } else {
	for (n=maint_GetFirstNeighbor(); n!=NULL; n=maint_GetNextNeighbor(n)) {
	    if (n->port == idb) {
		rtmp_UpdateNeighborEntry(n, n->port, TRUE, TRUE);
	    }
	}
    }    

    /* Age routes */
    p_tree = atroute_GetPTree(AT_PATH_RTMP);
    if (p_tree != NULL) {
	rtmp_aging_params.idb = idb;
	rtmp_aging_params.n = pak_neighbor;
	rtmp_aging_params.pak = pak;
	RBTreeForEachNode(rtmp_StartAgePath, (void *) &rtmp_aging_params,
			  p_tree, FALSE);
    }
}






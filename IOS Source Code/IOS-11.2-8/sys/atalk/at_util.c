/* $Id: at_util.c,v 3.7.6.11 1996/07/10 22:19:09 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_util.c,v $
 *------------------------------------------------------------------
 * AppleTalk general purpose subroutines
 *
 * July 1990, David Edwards
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_util.c,v $
 * Revision 3.7.6.11  1996/07/10  22:19:09  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.6.10  1996/07/08  07:09:26  dwong
 * CSCdi62125:  CSCdi60209 broke AppleTalk
 * Branch: California_branch
 *
 * Revision 3.7.6.9  1996/06/29  00:07:11  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.7.6.8  1996/06/28  22:30:36  kleung
 * CSCdi61658:  Router crash in idb_get_swsb() called from
 * atroute_FirstHop().
 * Branch: California_branch
 *
 * Revision 3.7.6.7  1996/06/21  16:05:28  wmay
 * CSCdi60209:  2500 Router Crash PPP Bridge Config - use new atalk
 * structures - have those routines use the dialer netcfg_idb when
 * dealing with ppp half bridged and cpp packets
 * Branch: California_branch
 *
 * Revision 3.7.6.6  1996/06/05  00:45:10  kleung
 * CSCdi57957:  Unsolicited message should use errmsg facility
 * Branch: California_branch
 *
 * Revision 3.7.6.5  1996/05/10  01:36:12  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.7.6.4  1996/05/04  00:34:02  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.7.6.3  1996/04/29  18:35:09  dwong
 * CSCdi55473:  appletalk tokenring misalignments
 * Branch: California_branch
 *
 * Revision 3.7.6.2  1996/03/23  01:27:51  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.6.1  1996/03/18  18:53:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.4  1996/03/16  06:27:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.16.3  1996/03/13  01:07:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.16.2  1996/03/07  08:29:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.16.1  1996/02/20  13:34:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/05  22:13:38  kleung
 * CSCdi49904:  atalk_upper vs atalk_cvu are equivalent
 *
 * Revision 3.6  1996/02/20  09:33:40  dwong
 * CSCdi47658:  appletalk protocol rtmp and eigrp are not valid
 * configuration
 * Disable rtmp and eigrp from the configuration list when dealing
 * with aurp tunnels
 *
 * Revision 3.5  1995/12/08  04:55:24  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.4  1995/12/07  22:27:23  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  08:44:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:58:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:50:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/19  18:35:38  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.7  1995/09/05  22:35:11  kleung
 * CSCdi39766:  Clean up atalk_print_network_header().
 *
 * Revision 2.6  1995/07/18  09:18:31  dstine
 * CSCdi37235:  Divide-by-zero error when configuring Atalk discovery mode
 *         - protect "%" operation with check for 0
 *
 * Revision 2.5  1995/07/13  06:50:13  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.4  1995/06/19 21:33:01  bbenson
 * CSCdi35519:  shutting down a LEC on an ELAN knocks out another LEC on
 *              diff ELAN.
 *         - Skip the reset of the hardware in atalkif_ResetMulticasts for
 *           the AIP.
 *
 * Revision 2.3  1995/06/14  22:51:01  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Change SMRP socket from 65 to 10.
 *
 * Revision 2.2  1995/06/10  12:55:33  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.1  1995/06/07  20:09:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef GLOBAL_DEBUG
#undef LOCK_DEBUG
#undef STATE_DEBUG
#undef ATYPE_DEBUG
#endif

#include "master.h"
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "config_register.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/tunnel.h"
#include "address.h"
#include "../os/chunk.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"
#include "macip_util.h"
#include "../atalk/atalk_nbp.h"
#include "../wan/smds.h"
#include "../wan/frame_relay.h"
#include "at_registry.h"
#include "../util/random.h"
#include "../if/if_vtemplate.h"


static chunk_type	*zipnet_chunk;	/* private allocation area */
static ulong		znCreated;	/* zipNet struct created */
static ulong		zifCreated;	/* interface-attached zone */
static ulong		pakDuplicated;	/* # of packets duplicated */
static ulong		pakCreated;	/* # of packets created */
static ulong		pakConverted;	/* # of packets created */
static ulong		at_assertline;
static char*		at_assertfile;
static boolean		at_badassert;

static void atalk_free_zipnet(zipnetentry* zn);

/* definitions for at_err(): */
#define	DEBUGSWITCH (at_debug(appletalk_debug, ATALK_NULL_MASK))

typedef struct _sockets {
    watched_queue *queue;
    char* name;
    const char* proc;
    ulong inputcnt;
    sockfunc_t sockfunc;
} sockets;

static sockets *AT_Sockets;

#ifdef DEAD

/*
 * atalk_bugreport - Report message to log devices with traceback
 *
 * Insures that msgs do not overrun logging by sending every 100th msg
 * unless debugging is enabled
 */
void
atalk_bugreport (int* counter,char* msg)
{
    if (at_debug(appletalk_debug, ATALK_NULL_MASK) || ((*counter % 100) == 0))
	errmsg(AT_ERR_BUGREPORT,msg);
    (*counter)++;
    atalk_stat[ATALK_BUGS]++;
}
#endif

void
atalk_debugmsg (char* msg)
{
    if (at_debug(appletalk_debug, ATALK_NULL_MASK)) {
	errmsg(AT_ERR_DEBUGMSG,msg);
	atalk_stat[ATALK_DEBUGMSGS]++;
    }
}

/*
 * create a single message from multiple args, and pass to
 * cisco logging system
 */
void
at_err (char *file, int line, int debugsw, char *msgin, ...)
{
#define	MAXMSG	160
    int *vargs = (int *) &msgin;
    static char msg[MAXMSG];
    static char fmt[MAXMSG];
    register int ii;
    register char *pp;

    /* get base file name: */
    pp = file;
    for (ii = strlen(file); ii >0; ii--) {
	if (file[ii] == '/') {
	    pp = &file[ii+1];
	    break;
        }
    }

    sprintf(fmt,"%s, %d: %s\n", pp, line, msgin);
    /* allow up to 10 args: */
    sprintf(msg, fmt,
	    vargs[1], vargs[2], vargs[3], vargs[4], vargs[5],
	    vargs[6], vargs[7], vargs[8], vargs[9], vargs[10]);

    /* print serious errors if "console logging errors" level: */
    errmsg(AT_ERR_SUBROUTINE_ERROR,msg);
}

/* 
 * atalk_assert - Check condition and report failures
 */
void
atalk_assert (char* file,int line)
{
#ifdef ALWAYS_CHECK_SANITY
    char *msg = string_getnext();
    sprintf(msg,"file \"%s\", line %d", file, line);
    at_badassert++;
    at_assertfile = file;		/* Keep record of last */
    at_assertline = line;
#ifndef QUIET_SANITY_CHECKS
    buginf("\nAT: Assertion failed at %s",msg);
#endif
    errmsg(AT_ERR_ASSERTFAILED, msg);
#endif
}

/*
 * Routine : atutil_RangesOverlap(start1, end1, start2, end2)
 *
 * Returns TRUE if ranges start1-end1 and start2-end2 overlap
 */
boolean 
atutil_RangesOverlap (ushort start1, ushort end1, ushort start2, ushort end2)
{
    if ( end2 == 0 || end1 ==0) {
	if (start1 == start2)
	    return (TRUE);
    } else {
	if ( (start1 >= start2 && end1 <= end2)   ||
	   (start1 <= start2 && end1 >= start2) ||
	   (start1 <= end2   && end1 >= end2)) 
	    return (TRUE);
    }
 
    return (FALSE);
}

/*
 * atutil_Initialize
 * Initialize module local storage.
 */
void
atutil_Initialize (void)
{
    znCreated = 0;
    zifCreated = 0;
    pakDuplicated = 0;
    pakCreated = 0;
    pakConverted = 0;
    at_badassert = 0;
    at_assertline = 0;
    at_assertfile = "";
    zipnet_chunk = chunk_create(sizeof(struct zipnetentry_),
				100,
				CHUNK_FLAGS_DYNAMIC,
				NULL, 0,
				"Atalk ZipNet");
    if (zipnet_chunk == NULL) {
	errmsg(AT_ERR_NOMEM, "zipnet chunk", __LINE__, __FILE__);
	return;
    }

    AT_Sockets = malloc_named(sizeof(sockets) * (ATALK_MAX_SOCKET + 1),
			      "AT Sockets");
    if (AT_Sockets == NULL) {
	errmsg(AT_ERR_NOMEM, "sockets",  __LINE__, __FILE__);
	return;
    }
	
}

/*
 * atutil_StatusReport
 * Generate a status report for the module.
 */
void
atutil_StatusReport (void)
{
    printf("\n  Utility data structures created: %d zipIf, %d zipNet",
	   zifCreated,
	   znCreated);
    printf("\n  Packet creations: %d new, %d duplicates, %d converted",
	   pakCreated, pakDuplicated, pakConverted);
    if (at_badassert != 0)
	printf("\n  %d bad assertions encountered (last:%s at %d)",at_badassert,
	       at_assertfile, at_assertline);
}

/*
 * Zipnet method enumerations
 *
 * Note: The method is named based upon the chain source and not the
 * zn field name which accounts for the possible confusion in the
 * zipnet handlers below.
 */
typedef enum {
    zipnet_viaZoneChain,	/* from zone->netlist through zn->nextNet */
    zipnet_viaNetChain		/* from net->zonelist through zn->nextZone */
} znMethod;

/*
 * util_FirstZipnet
 * Loop through the zipnet entries ignoring deleted entries if filtermethod
 * indicates we should.
 */
static zipnetentry*
util_FirstZipnet (zipnetentry* zn, znMethod method, int filtermethod)
{
    while (zn != NULL) {
	if (!zn->deleted || (filtermethod == AT_UTIL_ZIPNET_ACCEPT_ANY))
	    return (zn);
	if (method == zipnet_viaZoneChain)
	    zn = zn->nextNet;
	else
	    zn = zn->nextZone;
    }
    return (zn);
}

/*
 * util_NextZipnet
 * Get the next entry and then loop through the zipnet entries
 * ignoring deleted entries unless filtermethod says we should use them.
 */
static zipnetentry*
util_NextZipnet (zipnetentry* zn, znMethod method, int filtermethod)
{
    if (method == zipnet_viaZoneChain)
	zn = zn->nextNet;
    else
	zn = zn->nextZone;
    while (zn != NULL) {
	if ((!zn->deleted) || (filtermethod == AT_UTIL_ZIPNET_ACCEPT_ANY))
	    return zn;
	if (method == zipnet_viaZoneChain)
	    zn = zn->nextNet;
	else
	    zn = zn->nextZone;
    }
    return zn;
}

/*
 * util_DeleteZipnetChain
 * Mark each zipnet entry as deleted.
 */
static void
util_DeleteZipnetChain (zipnetentry* zn, znMethod method)
{
    while (zn != NULL) {
	zn->deleted = TRUE;
	if (method == zipnet_viaZoneChain)
	    zn = zn->nextNet;
	else
	    zn = zn->nextZone;
    }
}

/*
 * util_FreeZipnetChain
 * Free each zipnet entry on the chain.
 */
static void
util_FreeZipnetChain (zipnetentry** zn, znMethod method)
{
    zipnetentry* czn;
    
    while (*zn != NULL) {
	if (method == zipnet_viaZoneChain)
	    czn = (*zn)->nextNet;
	else
	    czn = (*zn)->nextZone;
	atalk_free_zipnet(*zn);
	ATASSERT(*zn == czn);
    }
}

/*
 * atalk_FreeZipifentry
 * Free a zifipentry structure
 */
static void
atalk_FreeZifnet (zipifentry* zif, atalkidbtype *idb)
{
    if (zif != NULL) {
	zif->zone->ifcount--;
	if ((zif->zone->ifcount == 0) && (atalk_firstNet(zif->zone) == NULL) &&
	    (zif->zone->acccount == 0))
	    atzip_NeedsGC();
	idb->atalk_zonecount--;
	zif->next = NULL;
	free(zif);
    }
}

/*
 * atalk_FreeZoneEntries
 * Free all zone entries on the chain
 */
void
atalk_FreeZoneEntries (atalk_rdb_t* p)
{
    if (p != NULL) {
	util_FreeZipnetChain(&p->zonelist,zipnet_viaNetChain);
    }
}

paktype*
atalk_pak_duplicate (paktype* pak)
{
    paktype* np;
    
#ifdef DEBUG
    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK))
	buginf("\nAT: packet duplicate original from %d.%d(%d), size %d",
	       atalk_pak_snet(pak), atalk_pak_snode(pak), atalk_pak_ssock(pak),
	       pak->atalk_datalen);
#endif
    ATASSERT(pak->atalk_datalen <= DDP_MAX_DATABYTES);
    if (pak->linktype == LINK_ATALK_SHORT) {
	ATASSERT(pak->datagramsize >= (DDPS_OVERHEADBYTES + pak->atalk_datalen));
	np = atalk_pak_create(atalk_getidb(pak->if_input),
			      atalk_pak_dstaddr(pak), 
			      atalk_pak_ssock(pak), atalk_pak_dsock(pak), 
			      atalk_pak_getpt(pak),
			      pak->atalk_datalen);
	if (np == NULL) {
	    atalk_stat[ATALK_NO_BUFFERS]++;
	    return NULL;
	}
	bcopy(pak->transport_start,np->transport_start,pak->atalk_datalen);
	pakConverted++;
	/* snet, dnode, dnet are set by atalk_pak_duplicate */
	/* snode was set to our but here we set it to theirs */
	atalk_pak_setsnode(np,atalk_pak_snode(pak));
	return np;
    }
    ATASSERT(pak->datagramsize >= (DDP_OVERHEADBYTES + pak->atalk_datalen));
    ATASSERT(pak->linktype == LINK_APPLETALK);
    ATASSERT((pak->network_start + DDP_OVERHEADBYTES) == pak->transport_start);
    np = pak_duplicate(pak);
    if (np == NULL) {
	atalk_stat[ATALK_NO_BUFFERS]++;
	return NULL;
    }
    np->atalk_datalen = pak->atalk_datalen;
    np->atalk_srcfqa = pak->atalk_srcfqa;
    np->atalk_dstfqa = pak->atalk_dstfqa;
    np->atalk_dstmcast = pak->atalk_dstmcast;
    pakDuplicated++;
    return (np);
}    

/*
 * atalk_pak_create
 * Create a packet and fill in the DDP header appropiately.  It
 * is here that short packets could be created.
 *
 * 'idb' may be NULL. If so, you'll have to set your own src net/node
 * and source FQA.
 */
paktype*
atalk_pak_create (atalkidbtype *idb,
		  ataddrtype dst,
		  sockTypes srcsock,
		  sockTypes dstsock,
		  ddpTypes pt,
		  ulong size)
{
    paktype* pak;
    ushort dstnet = atalk_netonly(dst);
    uchar dstnode = atalk_nodeonly(dst);

#ifdef DEBUG
    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK))
	buginf("\nAT: packet create for %d.%d(%d), type %d, size %d",
	       dstnet, dstnode, dstsock, pt, size);
#endif
    if (size > DDP_MAX_DATABYTES) /* Don't return illegal sized packet */
	return (NULL);
    
    pak = getbuffer(size+DDP_OVERHEADBYTES);
    if (pak == NULL) {
	atalk_stat[ATALK_NO_BUFFERS]++;
	return (NULL);
    }
    if (atalk_pak_newInit(pak, idb, size, dstnet, dstnode, srcsock, dstsock,
			  pt)) {
	atalk_stat[ATALK_DDP_CREATED]++;
	pakCreated++;
	return (pak);
    } else {
	datagram_done(pak);
	return (NULL);
    }
}

/*
 * atalk_pak_newInit
 * 
 * Sets the source and destination fields in a packet given an idb from which
 * said packet will be transmitted.
 */
boolean
atalk_pak_newInit (paktype *pak, atalkidbtype *idb, ulong size,
		   ushort dstnet, uchar dstnode,
		   uchar srcsock, uchar dstsock, uchar pt)
{
    ddplong	*ddp;

    if (pak == NULL)
	return (FALSE);

    if (idb && ((atalkif_srcnettype(idb,dstnet) == netStartupRange) ||
		(!atalkif_operational(idb)))) {
	pak->if_output = idb->phys_idb;
    } else {
	pak->if_output = NULL;
    }
    pak->atalk_dstmcast = ATALK_UNICAST;
    if (dstnet == ATALK_NET_UNNUMBERED) {
	dstnet = 0;
	dstnode = ATALK_BROAD_ADDR;
    }
    if (dstnet == 0) {
	pak->if_output = idb->phys_idb;
    }
    ddp = (ddplong *)pak->network_start;
    pak->enctype = ET_NULL;
    /*
     * set the input idb to agree with
     * the ddp source net parameters.
     * RT 7/28/92
     */
    if (idb && atalkif_MyAddress(idb) == 0) {
	if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    || at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    errmsg(AT_ERR_NOSRCADDR, idb_get_namestring(idb->phys_idb));
	atalk_stat[ATALK_NOSOURCE]++;
	return (FALSE);
    }
    pak->if_input = idb->phys_idb;
    pak->linktype = LINK_APPLETALK;
    pak->datagramstart = (uchar*)ddp;
    pak->datagramsize = size+DDP_OVERHEADBYTES;

    ddp->ssock = srcsock;
    ddp->dsock = dstsock;
    ddp->ddppt = pt;
    if (idb) {
	if (atalkif_CaymanTunnel(idb)) { /* all packets generated by us */
	    PUTSHORT(ddp->snet, 0);	/* use ddp src and dst addr of 0.0 */
	    PUTSHORT(ddp->dnet, 0);
	    ddp->dnode = 0;
	    ddp->snode = 0;
	} else {
	    PUTSHORT(ddp->snet, atalkif_MyNet(idb));
	    PUTSHORT(ddp->dnet, dstnet);
	    ddp->dnode = dstnode;
	    ddp->snode = atalkif_MyNode(idb);
	}
	pak->atalk_srcfqa = (!atalkif_CaymanTunnel(idb)
			     ? atutil_SetPakAddress(atalkif_MyNet(idb),
						    atalkif_MyNode(idb),
						    srcsock)
			     : atutil_SetPakAddress(0, 0, srcsock));
	pak->atalk_dstfqa = (!atalkif_CaymanTunnel(idb)
			     ? atutil_SetPakAddress(dstnet,
						    dstnode,
						    dstsock)
			     : atutil_SetPakAddress(0, 0, dstsock));
    } else {				/* idb == NULL */
	PUTSHORT(ddp->snet, 0);		/* use ddp src and dst addr of 0.0 */
	PUTSHORT(ddp->dnet, 0);
	ddp->dnode = 0;
	ddp->snode = 0;
	pak->atalk_srcfqa = 0;
	pak->atalk_dstfqa = 0;
    }
    pak->transport_start = (uchar*) ddp + DDP_OVERHEADBYTES;
    pak->atalk_datalen = size;
    PUTSHORT(ddp->length, DDP_OVERHEADBYTES + size);
    return (TRUE);
}
/*
 * atalk_pak_reply
 * Create a reply packet, filling in the header based on request.
 * NOTE:  this routine now looks up the IDB of the dst, and
 * returns in np->if_input (unless the dst was a bcast).
 */
paktype*
atalk_pak_reply (paktype* pak,ddpTypes pt,ulong size)
{
    paktype* np;
    u_short dnet = atalk_pak_dnet(pak);
    atalkidbtype *nidb;

    if (dnet == ATALK_CABLE_ALLNETS)
	nidb = atalk_getidb(pak->if_input); /* broadcast */
    else {
	nidb = atalk_findIDB(atalk_pak_dnet(pak));
	/* if can't find dest. interface, use input interface as src: */
	if (nidb == NULL) {
	    nidb = atalk_getidb(pak->if_input);
	    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, nidb->phys_idb)
		|| at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, nidb->phys_idb))
		buginf("\nAT: pak_reply: dubious reply creation, dst %a",
		    atalk_pak_dstaddr(pak));
	}
    }

    np = atalk_pak_create(nidb, atalk_pak_srcaddr(pak),
			  atalk_pak_dsock(pak),atalk_pak_ssock(pak), pt,size);
    if (np != NULL) {
	np->if_input = nidb->phys_idb;
	return np;
    } else {
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, nidb->phys_idb)
	    || at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, nidb->phys_idb))
	    buginf("\nAT: Unable to get a buffer for reply to %d.%d",
		   atalk_pak_snet(pak),atalk_pak_snode(pak));
	return NULL;
    }
}

/*
 * atalk_pak_dstaddr
 * Return the packet destination.
 */
ataddrtype
atalk_pak_dstaddr (paktype* pak)
{
    return atutil_DestAddr(pak);
}

/*
 * atalk_pak_srcaddr
 * Return the packet source.
 */
ataddrtype
atalk_pak_srcaddr (paktype* pak)
{
    ATAddress fqa;

    fqa.l = pak->atalk_srcfqa;
    return atalk_address(fqa.f.net,fqa.f.node);
}

/*
 * atalk_ValidNodeAddress
 * Check the node address to verify its reasonable.
 */
boolean
atalk_ValidNodeAddress (ataddrtype addr)
{
    switch (atalk_nodeonly(addr)) {
    case ATALK_ANY_ROUTER:
    case ATALK_BROAD_ADDR:
	return (FALSE);
    }
    if (   (atalk_netonly(addr) == 0)
	|| (atalk_netonly(addr) > ATALK_STARTUP_END))
	return (FALSE);
    return (TRUE);
}

/*
 * atalk_ValidPacket
 * Do some simple checks for a packet and return its type.
 */
ddpTypes
atalk_ValidPacket (paktype* pak, ushort minsize, boolean ifoper)
{
    atalkidbtype *idb = atalk_getidb(pak->if_input);
    
    if (ifoper) {
	if (!atalkif_operational(idb))
	    return DDP_TYPE_Invalid;
    } else {
	if (!atalkif_InterfaceUp(idb))
	    return DDP_TYPE_Invalid;
    }
    if (pak->atalk_datalen < minsize)
	return DDP_TYPE_Invalid;
    if (!atalk_ValidNodeAddress(atalk_pak_srcaddr(pak)))
	return DDP_TYPE_Invalid;
    return atalk_pak_getpt(pak);    
}

void
atalk_pak_hearself (paktype* pak)
{
    if (pak == NULL)
	return;
    pak->flags |= PAK_HEARSELF;
}

void
atalk_pak_setbroadcast (paktype* pak)
{
    if (pak == NULL)
	return;
    if (pak->atalk_dstmcast == ATALK_UNICAST) 
	pak->atalk_dstmcast = ATALK_ALL_MULTICAST;
}

void
atalk_pak_setzonemcast (paktype* pak,ushort zsum)
{
    if (pak == NULL)
	return;
    pak->atalk_dstmcast = zsum;
}

/*
 * atalk_BindSocket
 * associate a queue with a socket
 */
boolean
atalk_BindSocket (watched_queue *q, char* name, uchar sock,
		  sockfunc_t sockfunc)
{
    if (sock > ATALK_MAX_SOCKET)
	return (FALSE);
    AT_Sockets[sock].queue = q;
    AT_Sockets[sock].name = name;
    AT_Sockets[sock].inputcnt = 0;
    process_get_name(THIS_PROCESS, &AT_Sockets[sock].proc);
    AT_Sockets[sock].sockfunc = sockfunc;
    return (TRUE);
}

/*
 * atalk_UnBindSocket
 * disassociate a queue from a socket
 */
void
atalk_UnBindSocket (uchar sock)
{
    if (sock > ATALK_MAX_SOCKET)
	return;
    AT_Sockets[sock].queue = NULL;
    AT_Sockets[sock].name = (char *) NULL;
    AT_Sockets[sock].inputcnt = 0;
    AT_Sockets[sock].proc = (char *) NULL;
    AT_Sockets[sock].sockfunc = NULL;
}

/*
 * atalk_SocketPreQueue
 *
 * Check if there is a pre-processing function associated with this
 * socket. If there is, return the status that running that function
 * returns.
 *
 * If the socket pre-queue function returns TRUE, then the packet
 * should not be put on the queue in question, and should be considered
 * "handled."
 *
 */
boolean
atalk_SocketPreQueue (uchar sock, paktype *pak)
{
    if (sock > ATALK_MAX_SOCKET)
	return (FALSE);
    if (AT_Sockets[sock].sockfunc != NULL) {
	return ((AT_Sockets[sock].sockfunc)(pak));
    } else {
	return (FALSE);
    }
}

/*
 * atalk_SocketMarkInput
 */
void
atalk_SocketMarkInput (uchar sock)
{
    AT_Sockets[sock].inputcnt++;
}

/*
 * atalk_SocketQueue
 * return the queue associated with a socket (or NULL)
 * sideeffect: increments the input count
 */
watched_queue *
atalk_SocketQueue (uchar sock)
{
    if (sock > ATALK_MAX_SOCKET)
	return NULL;
    return (AT_Sockets[sock].queue);
}

/* true if socket has handler */
boolean
atalk_IsBoundSocket (sockTypes sock)
{
    if (sock > ATALK_MAX_SOCKET)
	return (FALSE);
    return (AT_Sockets[sock].queue != NULL);
}

/*
 * return a socket in the dynamic range that is not in use, 0 if none
 * available.
 */
sockTypes
atalk_SelectSocket (void)
{
    sockTypes ii;
    
    for (ii= ATALK_SOCK_DYN_LOWER; ii <= ATALK_SOCK_DYN_UPPER; ii++)
	if (!atalk_IsBoundSocket(ii))
	    return(ii);
    
    return(ATALK_INVALID_SOCKET);
}

/*
 * atalk_PrintSocket
 * print the socket description
 */
void
atalk_PrintSocket (sockTypes sock)
{
    if (sock > ATALK_MAX_SOCKET) {
	printf("\n%% Socket %d is not valid",sock);
	return;
    }
    if (AT_Sockets[sock].queue == NULL) {
	printf("\n%% Socket %d is not in use",sock);
	return;
    }
    printf("\n  %-3d	%8s	 %16s	%-3d %-6d",sock,
	   AT_Sockets[sock].name,
	   AT_Sockets[sock].proc,
	   process_queue_size(AT_Sockets[sock].queue),
	   AT_Sockets[sock].inputcnt);
}

/*
 * atalk_PrintActiveSockets
 * print the socket description
 */
void
atalk_PrintActiveSockets (void)
{
    int i;
    printf("\nSocket	Name    	Owner           	Waiting/Processed\n");
    for (i = 1; i < ATALK_MAX_SOCKET; i++) {
	if (AT_Sockets[i].queue != NULL)
	    atalk_PrintSocket(i);
    }
}

char*
at_sockname (int sock)
{
    if (sock > ATALK_MAX_SOCKET)
	return "Invalid";
    if (AT_Sockets[sock].queue == NULL)
	return "Unknown";
    return AT_Sockets[sock].name;
}

static void
at_pak_printit (paktype* pak, char* title, char* msg, atalkidbtype* idb)
{
    static char buff[128], ddpn[128];
    char* ssock;
    char* dsock;
    
    ssock = at_sockname(atalk_pak_ssock(pak));
    dsock = at_sockname(atalk_pak_dsock(pak));
    atalk_ddpname(atalk_pak_getpt(pak),ddpn);
    if (msg == NULL)
	sprintf(buff,"%s,src %a-%s,dst %a-%s,%s",
		title,atalk_pak_srcaddr(pak),ssock, atalk_pak_dstaddr(pak),
		dsock,ddpn);
    else
	sprintf(buff,"%s,src %a-%s,dst %a-%s,%s,%s",
		title,atalk_pak_srcaddr(pak),ssock, atalk_pak_dstaddr(pak),
		dsock,ddpn,msg);
    atalk_debugmsg(buff);
}


#define DUMPER_PRE_LINE_START 20

/*
 * at_pak_dumphdr
 *
 * Dump the header of an AppleTalk packet.
 * 20 bytes of data before datagram start and 20 bytes of data following
 * the network_start pointer. If the network_start pointer isn't set,
 * the dump 40 bytes following the datagram start pointer.
 *
 * This is done into a static local buffer and then buginf'ed to log it.
 */
void at_pak_dumphdr (paktype *pak, idbtype *idb, char *label)
{
    uchar	*pkt_ptr;		/* pointer into the packet */
    uchar	fmt_buffer[80];		/* buffer used for output */
    int i;


    if (pak->network_start)
	buginf("\n%s: %s AT packet: enctype %s, size %d",
	       idb->namestring, label, encstring(pak->enctype),
	       pak->datagramsize);
    else
	buginf("\n%s: %s AT packet: enctype %s, size %d, no network start",
	       idb->namestring, label, encstring(pak->enctype),
	       pak->datagramsize);

    pkt_ptr = pak->datagramstart - DUMPER_PRE_LINE_START;
    fmt_buffer[0] = '\n';
    i = 1;
    while (i < (sizeof(fmt_buffer) - 5)) {
	if (pkt_ptr == pak->datagramstart)
	    fmt_buffer[i++] = '|';
	else if (pak->network_start && pkt_ptr == pak->network_start)
	    fmt_buffer[i++] = '$';

	fmt_buffer[i++] = atalk_upper(tohexchar((*pkt_ptr & 0xf0) >> 4));
	fmt_buffer[i++] = atalk_upper(tohexchar(*pkt_ptr));
	pkt_ptr++;
    }
    fmt_buffer[i] = '\0';		/* terminate string */
    buginf(fmt_buffer);			/* print the first line of glop */

    fmt_buffer[0] = '\n';
    i = 1;
    while (i < (sizeof(fmt_buffer) - 5)) {
	if (pkt_ptr == pak->datagramstart)
	    fmt_buffer[i++] = '|';
	else if (pak->network_start && pkt_ptr == pak->network_start)
	    fmt_buffer[i++] = '$';

	fmt_buffer[i++] = atalk_upper(tohexchar((*pkt_ptr & 0xf0) >> 4));
	fmt_buffer[i++] = atalk_upper(tohexchar(*pkt_ptr));
	pkt_ptr++;
    }
    fmt_buffer[i] = '\0';		/* terminate string */
    buginf(fmt_buffer);			/* print the second line of glop */
}

void
at_pak_printIhdr (paktype* pak, atalkidbtype* idb,boolean verbose)
{
    if (pak->if_input) {
	at_pak_dumphdr(pak, pak->if_input, "input");
    }
}    

void
at_pak_printOhdr (paktype* pak, atalkidbtype* idb,boolean verbose)
{
    if (pak->if_output) {
	at_pak_dumphdr(pak, pak->if_output, "output");
    }
}    

/*
 * atalk_pak_printbad
 * Error handling a packet.
 */
void
atalk_pak_printbad (paktype* pak, char* msg, atalkidbtype* idb)
{
    if (idb && (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	|| at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))) {
	at_pak_printit(pak,"Packet discarded",msg,idb);
	at_pak_printIhdr(pak,idb, at_debug(atalkpkt_debug, ATALK_NULL_MASK));
    }
}

/*
 * atalk_pak_printgood
 * Finished processing a packet.
 */
void
atalk_pak_printgood (paktype* pak, char* msg, atalkidbtype* idb)
{
    if (idb && at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	at_pak_printit(pak,"Packet",msg,idb);
	at_pak_printIhdr(pak,idb,TRUE);
    }
}

/*
 * atalk_pak_printforus
 * Packet is at the crossroads, lets tell everyone.
 */
void
atalk_pak_printforus (paktype* pak, int forus)
{
    char* msg;
    switch (forus) {
    case ATALK_PITCHIT: msg = "not for us";break;
    case ATALK_ROUTEIT: msg = "will forward";break;
    case ATALK_CONSUMEIT: msg = "will process";break;
    case ATALK_COPYAND_ROUTEIT: msg = "will process and forward"; break;
    default: msg = "dropped";
    }
    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, pak->if_input))
	at_pak_printit(pak,"Packet received",msg, atalk_getidb(pak->if_input));
}

/*
 * atalk_pak_printprocess
 * Processing packet.  Only print if debugging process and packets.
 */
void
atalk_pak_printprocess (paktype* pak, atalkdebugtype *flagp, char* msg)
{
    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, pak->if_input)
	&& at_debug(flagp, ATALK_INTERFACE_MASK, pak->if_input))
	at_pak_printit(pak,"Processing packet",msg,
		       atalk_getidb(pak->if_input));
}

/*
 * atalk_pak_printfail
 * Failure during processing.  Only print if debugging process and packets.
 */
void
atalk_pak_printfail (paktype* pak, atalkdebugtype *flagp, char* msg)
{
    if ((at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, pak->if_input)
	 || at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, pak->if_input))
	&& at_debug(flagp, ATALK_INTERFACE_MASK, pak->if_input))
	at_pak_printit(pak,"Processing error",msg,
		       atalk_getidb(pak->if_input));
}


void
atalk_pak_setdnet (paktype* pak,ushort dnet)
{
    ATAddress a = atutil_DestFQA(pak);
    ddptype* ddp = (ddptype*)pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	PUTSHORT(ddp->l.dnet,dnet);
    a.f.net = dnet;
    pak->atalk_dstfqa = a.l;
}

void
atalk_pak_setsnet (paktype* pak,ushort snet)
{
    ATAddress a = atutil_SrcFQA(pak);
    ddptype* ddp = (ddptype*)pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	PUTSHORT(ddp->l.snet,snet);
    a.f.net = snet;
    pak->atalk_srcfqa = a.l;
}

ushort
atalk_pak_hops (paktype* pak)
{
    ddplong* ddp = (ddplong*)pak->network_start;
    ushort hc;
    if (pak->linktype != LINK_APPLETALK)
	return 0;
    hc = GETSHORT(ddp->length);
    hc = (hc & DDPHOPMASK) >> DDPHOPSHIFT;
    return hc;
}

void
atalk_pak_sethops (paktype* pak, ushort hc)
{
    ddplong* ddp = (ddplong*) pak->network_start;
    if (pak->linktype != LINK_APPLETALK)
	return;
    PUTSHORT(ddp->length,
	     (GETSHORT(ddp->length)&~DDPHOPMASK)|(hc<<DDPHOPSHIFT));
}

void
atalk_pak_setdnode (paktype* pak,uchar node)
{
    ATAddress a = atutil_DestFQA(pak);
    ddptype* ddp = (ddptype*)pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	ddp->l.dnode = node;
    a.f.node = node;
    pak->atalk_dstfqa = a.l;
}

void
atalk_pak_setsnode (paktype* pak,uchar node)
{
    ATAddress a = atutil_SrcFQA(pak);
    ddptype* ddp = (ddptype*)pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	ddp->l.snode = node;
    a.f.node = node;
    pak->atalk_srcfqa = a.l;
}

sockTypes
atalk_pak_ssock (paktype* pak)
{
    return (sockTypes)(atutil_SrcFQA(pak).f.spec);
}

sockTypes
atalk_pak_dsock (paktype* pak)
{
    return (sockTypes)(atutil_DestFQA(pak).f.spec);
}

void
atalk_pak_setssock (paktype* pak, sockTypes sock)
{
    ATAddress a = atutil_SrcFQA(pak);
    ddptype* ddp = (ddptype*)pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	ddp->l.ssock = sock;
    else
	ddp->s.ssock = sock;
    a.f.spec = sock;
    pak->atalk_srcfqa = a.l;
}

void
atalk_pak_setdsock (paktype* pak, sockTypes sock)
{
    ATAddress a = atutil_DestFQA(pak);
    ddptype* ddp = (ddptype*)pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	ddp->l.dsock = sock;
    else
	ddp->s.dsock = sock;
    a.f.spec = sock;
    pak->atalk_dstfqa = a.l;
}

/*
 * atalk_pak_setlength - Set the length and zero the hop field
 */
void
atalk_pak_setlength (paktype* pak,ulong size)
{
    uchar* ddp = pak->network_start;
    uint fsize;
    
    pak->atalk_datalen = size;
    if (pak->linktype == LINK_APPLETALK) {
	fsize = size + DDP_OVERHEADBYTES;
    } else {
	fsize = size + DDPS_OVERHEADBYTES;
    }
    /* First 2 bytes of DDP header is length field */
    PUTSHORT(ddp, fsize);
    pak->datagramsize = fsize;
}


ushort
atalk_pak_cksum (paktype* pak)
{
    uchar* ddp = pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	return GETSHORT(((ddplong*)ddp)->cksum);
    else
	return 0;
}

boolean
atalk_pak_needscksum (paktype* pak)
{
    if (!atalk_checksumming)
	return (FALSE);
    if (pak->linktype == LINK_APPLETALK)
	return (TRUE);
    return (FALSE);
}

void
atalk_pak_setcksum (paktype* pak, ushort cksum)
{
    uchar* ddp = pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	PUTSHORT(((ddplong*)ddp)->cksum,cksum);
}

/*
 * atalk_pak_inithdrptr
 *
 * This routine removes the encapsulation from the datagram pointers
 * which effectively eliminates the encapsulation.  This is done in a
 * very kludgy manner due to the number of HIDDEN assumptions with our
 * encapsulation support.
 *
 * The drivers should pass a datagram size/addr for the data portion
 * of the packet since they know this information.  We attempt to
 * correct that deficiency here in order to localize the hacks.
 *
 * Before calling this routine for an incoming packet, clear the
 * transport_start in the packet header.  This pointer will be
 * initialized by the atalk_initdataptr routine after we make the
 * necessary adjustments to the generic datagram information.  Never
 * reset transport_start once it is initialized for a packet.
 *
 * After calling this routine, an incoming packet will have its
 * datagramstart positioned at the first byte of the ddp header.  The
 * datagramsize will be the total size of the ddp header and data (and
 * sometimes filler bytes depending upon the encapsulation method.)
 *
 *
 * FURTHER: If the driver sets the packet encapsulation to
 * ET_ETHERTALK then it is indicating that the LLAP bytes end at the
 * line pak->network_start.  Otherwise, the interface encapsulation
 * must end at the line and this routine adjusts the datagramstart to
 * account for LLAP if appropriate.  After calling this routine, the
 * meaning of ET_ETHERTALK changes slightly.  That encapsulation
 * indicates that the three LLAP bytes can be accessed by backing up
 * the datagramstart pointer regardless of where it is (e.g. at or
 * below the line).  ET_ETHERTALK is a misnomer in the current
 * implementation, ET_LOCALTALK would be better and ET_LLAP would be
 * correct but less obvious.
 *
 * FINALLY: Be extremely careful when making changes that may effect
 * this aspect of the code.  Especially noteworthy is X.25 since it
 * may duplicate the AT packet during the encaps call.
 *
 */
void
atalk_pak_inithdrptr (paktype* pak)
{
    atalkidbtype *idb = atalk_getidb(pak->if_input);

    /* 
     * First, assume the standard case, encaps ends at the line 
     */
    ATASSERT(pak->transport_start == NULL);

    /*
     * Initialize the private AppleTalk packet fields.  
     * ...atalk_dstmcast - Force to unicast in case the source packet is
     *                     duplicated or modified and returned (eg., echo 
     *			   replies).
     * ...atalk_datalen  - Initialize for completeness.  We haven't set the 
     *			   dataptr yet so zero is correct.
     * ...atalk_srcfqa
     * ...atalk_dstfqa   - These fields are set later if we decode the packet
     *			   so for now just set an obvious value.
     */
    pak->atalk_dstmcast = ATALK_UNICAST;
    pak->atalk_srcfqa = 0;
    pak->atalk_dstfqa = 0;
    pak->atalk_datalen = 0;

    /*
     * If AT encaps is ETHERTALK and the packet encaps is not ETHERTALK,
     * then we have a situation where the driver did not account
     * for the LLAP header and we must do so now.
     */

    if (idb->atalk_enctype == ET_ETHERTALK &&
	pak->enctype != ET_ETHERTALK) {
	/*
	 * This is just a laugh riot. Encapsulations like X25 sneak the
	 * datagramstart pointer towards network_start and decrease
	 * datagramsize. This means you can't just delta pak->encsize.
	 * The safest way to deal with this is to work it out again.
	 */
        pak->network_start += LLAP_ENCAPBYTES;
        pak->encsize        = pak->network_start - pak->datagramstart;
        pak->enctype        = ET_ETHERTALK;
    }

    /*
     * Adjust the datagramstart pointer to now point to the beginning
     * of the ddp header (which is pointed at by network_start).
     * After this point, it is safe to call the assorted
     * atalk_pak_xxxx routines which access the header.  We need to
     * adjust the datagramsize first.
     */
    pak->datagramsize -= (pak->network_start - pak->datagramstart);
    pak->datagramstart = pak->network_start;

    /* 
     * Due to possible problems prior to our receipt of this packet,
     * filler bytes may now be incorrectly counted as data.  This
     * situation breaks older AT/VMS so we must now select the lesser
     * of ddp->length and framesize.  This is not done in fast
     * switching... the required packet validation to be able to
     * determine the proper data length would cost thousands of pps.
     */
    pak->datagramsize = min(pak->datagramsize, atalk_pak_rawlength(pak));
}    

/*
 * atalkif_DstAddrType
 * Determine information about a destination address.
 */
addrTypes
atalkif_DstAddrType (atalkidbtype* idb,ataddrtype addr)
{
    ushort net = atalk_netonly(addr);
    ushort node = atalk_nodeonly(addr);

    if (idb == NULL)
	return addrIndeterminate;
    if (!atalkif_InterfaceUp(idb))
	return cableUnavailable;
    if (atalkif_AURPTunnel(idb))
	return (dstNotForCable);
    if (idb->atalk_cableend == 0) {
	/*
	 * Non-extended network check
	 */
	if ((idb->atalk_cablestart == net) ||
	    ((idb->atalk_cablestart == 0) || (net == 0))) {
	    if (node == ATALK_BROAD_ADDR)
		return (dstCableBroadcast);
	    if (node == idb->atalknode)
		return (dstIsMyAddress);
	    if (node == ATALK_ANY_ROUTER)
		return ((net != 0) ? dstForCableRouter : addrIsInvalid);
	    return (dstForThisCable);
	}
	if (net >= ATALK_STARTUP_START)
	    return (addrIsInvalid);
    } else {
	/*
	 * Extended network check
	 */
	if ((idb->atalk_cablestart <= net) &&
	    (idb->atalk_cableend >= net)) {
	    if (node == ATALK_BROAD_ADDR)
		return (dstCableBroadcast);
	    if (addr == atalkif_MyAddress(idb))
		return (dstIsMyAddress);
#if USE_WANTALK			/* unfinished */
	    if (node == ATALK_MARSHAL_ADDR) 
		if (atalkif_Civilized(idb))
		    return (dstForCableMarshal);
		else
		    return (addrIsInvalid);
#endif
	    if (node == ATALK_ANY_ROUTER)
		return (dstForCableRouter);
	    return (dstForThisCable);
	}
	if (net == 0) {
	    if (node == ATALK_BROAD_ADDR)
		return (dstCableBroadcast);
#if USE_WANTALK			/* unfinished */
	    if ((node == ATALK_MARSHAL_ADDR) &&
		(atalkif_Civilized(idb)))
		return (dstForCableMarshal);
#endif
	    if (node == ATALK_ANY_ROUTER)
		return (dstForCableRouter);
	    return (addrIsInvalid);
	}
	if (net >= ATALK_STARTUP_START) {
	    if (net <= ATALK_STARTUP_END) {
		if ((node == ATALK_BROAD_ADDR) ||
		    (node == ATALK_ANY_ROUTER) ||
		    (node == ATALK_MARSHAL_ADDR))
		    return (addrIsInvalid); /* No startup bcasts or marshals */
		if (addr == atalkif_MyAddress(idb))
		    return (dstIsMyAddress);
		return (dstForThisCable);
	    }
	    return (addrIsInvalid);	/* net is beyond the startup range */
	}
    }
    if (!atalkif_operational(idb)) {
	if (node == ATALK_BROAD_ADDR) 
	    return (dstCableBroadcast);
	return (dstForThisCable);
    }
    return (dstNotForCable);
}

#ifdef ATYPE_DEBUG
static char*
addrTypeMsg (addrTypes addrt)
{
    switch(addrt) {
      case srcIsForeign: 	return "srcIsForeign"; 		break;
      case srcIsCableSpecific:	return "srcIsCableSpecific"; 	break;
      case srcIsCableMarshal:	return "srcIsCableMarshal"; 	break;
      case srcOnThisCable:	return "srcOnThisCable"; 	break;
      case dstNotForCable:	return "dstNotForCable"; 	break;
      case dstCableBroadcast:	return "dstCableBroadcast"; 	break;
      case dstForCableMarshal:	return "dstForCableMarshal"; 	break;
      case dstIsMyAddress:	return "dstIsMyAddress"; 	break;
      case dstForCableRouter:	return "dstForCableRouter"; 	break;
      case dstForThisCable:	return "dstForThisCable"; 	break;
      case addrErrorDetected:	return "addrErrorDetected"; 	break;
      case addrIndeterminate:	return "addrIndeterminate"; 	break;
      case addrIsInvalid:	return "addrIsInvalid"; 	break;
      case cableUnavailable:	return "cableUnavailable"; 	break;
      case srcBadForCable: 	return "srcBadForCable"; 	break;
    }
};
#endif ATYPE_DEBUG
/*
 * atalk_PrintAddrType
 * Print the address type for debugging.
 */
addrTypes
atalk_PrintAddrType (idbtype* idb, ataddrtype addr,addrTypes addrt)
{
#ifdef ATYPE_DEBUG
    ttyprintf(CONTTY, "\nPrintAddrType: %s:%a - %s",idb->namestring,addr,
	      addrTypeMsg(addrt));
#endif
    return (addrt);
}		   

/*
 * atalkif_SrcAddrType
 * Determine information about a source address.
 *
 * Note: srcBadForCable is returned when a valid address is specified
 *		but the address is cable specific and not valid for this
 *		cable.
 *	 addrIsInvalid is returned when an invalid address is specified.
 */
addrTypes
atalkif_SrcAddrType (atalkidbtype *idb,ataddrtype addr)
{
    ushort net = atalk_netonly(addr);
    ushort node = atalk_nodeonly(addr);

    if (idb == NULL)
	return addrIndeterminate;
    if (!atalkif_InterfaceUp(idb))
	return cableUnavailable;
    if ((node == ATALK_BROAD_ADDR) ||
	(node == ATALK_ANY_ROUTER))
	return addrIsInvalid;
    if (idb->atalk_cableend == 0) {
				/* Non-extended network check */
	if ((idb->atalk_cablestart == net) ||
	    ((idb->atalk_cablestart == 0) || (net == 0))) {
	    return ((net != 0) ? srcOnThisCable : srcIsCableSpecific);
	}
	if ((net >= ATALK_STARTUP_START) && 
	    (net <= ATALK_STARTUP_END))
	    return srcBadForCable;
    } else {
				/* Extended network check */
	if ((idb->atalk_cablestart <= net) &&
	    (idb->atalk_cableend >= net)) {
#if USE_WANTALK			/* unfinished */
	    if (node == ATALK_MARSHAL_ADDR) {
		if (atalkif_Civilized(idb))
		    return srcIsCableMarshal;
		return srcBadForCable; /* reserved address */
	    }
#endif
	    return srcOnThisCable;
	}
	if (net == ATALK_CABLE_ALLNETS) {
#if USE_WANTALK			/* unfinished */
	    if (node == ATALK_MARSHAL_ADDR) {
		if (atalkif_Civilized(idb))
		    return srcIsCableMarshal;
	    }
#endif
	    return srcBadForCable; /* Invalid network (zero is invalid) */
	}
	if ((net >= ATALK_STARTUP_START) &&
	    (net <= ATALK_STARTUP_END)) {
	    return srcIsCableSpecific;
	}
    }
    if (net >= ATALK_STARTUP_END)
	return addrIsInvalid;
    return srcIsForeign;
}

/*
 * atalkif_BroadcastForMe
 * Returns TRUE if this broadcast address should be seen by us.
 */
boolean
atalkif_BroadcastForMe (atalkidbtype* idb,ataddrtype addr)
{
    ushort net = atalk_netonly(addr);
    ATASSERT(atalk_nodeonly(addr) == ATALK_BROAD_ADDR);
    if (!atalkif_operational(idb))
	return TRUE;
    if ((net == ATALK_CABLE_ALLNETS) || (net == idb->atalknet))
	return TRUE;
    return FALSE;
}	

/*
 * atalkif_srcnettype - Determine net type if used as a source
 */
netTypes
atalkif_srcnettype (atalkidbtype* idb,ushort net)
{
    if (idb == NULL)
	return netInvalid;
    if (net == ATALK_NET_UNNUMBERED) {
	if (atalkif_HalfRouter(idb))
	    return netValidExtended;
	else
	    return netInvalid;
    }
    if (net > ATALK_STARTUP_END)
	return netInvalid;
    if ((net >= ATALK_STARTUP_START) && (net <= ATALK_STARTUP_END))
	return netStartupRange;
    if (atalkif_net_extended(idb)) {
	if (net == 0)
	    return netCableBroadcast;
	if ((net >= idb->atalk_cablestart)&&(net <= idb->atalk_cableend))
	    return netValidExtended;
    } else {
	if (net == idb->atalknet)
	    return netValidNonExtended;
	if (net == 0)
	    return netValidNonExtended;
    }
    return netBadForCable;
}

/*
 * atalkif_dstnettype - Determine net type if used as a dest
 */
netTypes
atalkif_dstnettype (atalkidbtype* idb,ushort net)
{
    if (idb == NULL)
	return netInvalid;
    if (idb->atalk_cableend == 0) {
				/* Non-extended network check */
	if (idb->atalk_cablestart == net)
	    return netLocalNet;
    } else {
				/* Extended network check */
	if ((idb->atalk_cablestart <= net) &&
	    (idb->atalk_cableend >= net))
	    return netLocalNet;
    }
    if (net == ATALK_NET_UNNUMBERED) {
				/* Half-router destination */
	if (atalkif_HalfRouter(idb))
	    return netLocalNet;
	else
	    return netInvalid;
    } 
    if (net > ATALK_STARTUP_END)
	return netInvalid;	/* Network number is too large */
    if (net == 0) {
				/* Local cable network */
	if (atalkif_net_extended(idb))
	    return netCableBroadcast;
	else
	    return netLocalNet;
    } else if ((net >= ATALK_STARTUP_START) && (net <= ATALK_STARTUP_END))
	return netStartupRange;
    return netValidForRouting;
}

/*
 * atalkif_ClearLocalConfig
 * Deletes zone and resets net info to prepare for an acquire.
 */
void
atalkif_ClearLocalConfig (atalkidbtype* idb)
{
    if (idb == NULL)
	return;
    atalkif_DisablePort(idb);
    atalkif_ClearNames(idb);
    if (atalkif_HalfRouter(idb))
	return;
    atalkif_clear_route_zones(idb);
    if (idb->atalk_discovery)
	atalkif_clear_zones(idb);
    if (idb->atalk_cableend == 0) {
	idb->atalknet = 0;
	idb->atalk_cablestart = 0;
	return;
    } else {
	idb->atalknet = atalk_random(ATALK_STARTUP_END - ATALK_STARTUP_START)
	    + ATALK_STARTUP_START;
	idb->atalk_cablestart = 0;
	idb->atalk_cableend = ATALK_STARTUP_END;
	return;
    }
}

uchar
atalk_upper (uchar c)
/* map c to it's appletalk uppercase equivalent */
{
    if (islower(c))
	return(_toupper(c));
    
    /* apple "special" char mappings, appendix D, Inside Appletalk, 2nd ed. */
    switch(c) {
    case 0x88:	return(0xcb);
    case 0x8a:	return(0x80);
    case 0x8b:	return(0xcc);
    case 0x8c:	return(0x81);
    case 0x8d:	return(0x82);
    case 0x8e:	return(0x83);
    case 0x96:	return(0x84);
    case 0x9a:	return(0x85);
    case 0x9b:	return(0xcd);
    case 0x9f:	return(0x86);
    case 0xbe:	return(0xae);
    case 0xbf:	return(0xaf);
    case 0xcf:	return(0xce);
    default:
	return(c);
    }
}

/* 
 * copy <src> to <dst> for up to dlen chars, converting
 * any hex input specifications to binary. e.g., ":ff" ==> 255
 *
 * return the length of <dst>.
 *
 * NOTE:  <src> must be NULL terminated,  but <dst> is only NULL terminated
 * if it's length is less than <dlen>.
 */
int
atalk_hex_filter (char *src, char *dst, int dlen)
{
    register int ii, zz, len=strlen(src);

    memset(dst, 0, dlen);

    for (ii=0, zz=0; ii<len && zz < dlen; ii++, zz++) {
        dst[zz] = src[ii];
	/* if possible hex sequence and dst has room... */
	if (dst[zz] == ':' && (ii+2 < len)) {
	    /* if definite hex sequence... */
	    if (ISHEXDIGIT(src[ii+1]) && ISHEXDIGIT(src[ii+2])) {
		/* ...then convert ":XX" to binary: */
		dst[zz] = 16*HEXTOBIN(src[ii+1]);
		dst[zz] += HEXTOBIN(src[ii+2]);
		ii += 2;	/* skip extra 2 chars */
	    }
	}
    }
    return(zz);
}

/*
 * if <str> contains non-printing chars, map hex chars to
 * ':XX' form and return ptr to new string.  o'wise return
 * pointer to original
 */
char *
at_hex_display (char *str)
{
#define	MAX_AT_HEX_BUFF	256
#define	MAXNEST	4		/* max number of nested calls */
    static char buff[MAXNEST][MAX_AT_HEX_BUFF+1];
    static int selector=0;
    register char *pp = &buff[(selector++)%MAXNEST][0];
    register int ii, jj;
    int len = strlen(str);
    boolean has_hex = FALSE;
    
    /* copy and map with length checks: */
    for (ii=0,jj=0; ii<len && jj < MAX_AT_HEX_BUFF; ii++) {
	if (ISDISPLAY_ASCII(str[ii]))
	    pp[jj++] = str[ii];
	else
	    if (jj < MAX_AT_HEX_BUFF-3) {
		has_hex = TRUE;
		pp[jj++] = ':';
		pp[jj++] = BINTOHEX(HI_NIBBLE(str[ii]));
		pp[jj++] = BINTOHEX(LO_NIBBLE(str[ii]));
	    }
	    else
		/* no more room, truncate and return what we have: */
		break;
    }
    pp[jj] = '\0';	/* make sure dst is truncated */
    
    return(has_hex?pp:str);
}

char cisco_NBP_type[] = "ciscoRouter";
static char oem_NBP_type[] = "systemRouter";

/*
 * private routine to generate an NBP name for interface <idb>
 */
static void
atalkif_NBPname (nbpEntity *nve, atalkidbtype *idb)
{
    char *ntype;
    ziptabletype *ztab;
    str32 ifzone;
    char ifname[100];

    /* formulate interface name: */
    sprintf(ifname,"%s.%s", hostname, idb_get_namestring(idb->phys_idb));
    ifname[32] = '\0';

    /* set entity type name: */
    ntype = (configregister&CFG_OEM) ? oem_NBP_type : cisco_NBP_type;

    /* make primary zone name a str32: */
    ztab = atalkif_primary_zone(idb);
    str32ninit(&ifzone, (char *) ztab->name, ztab->length);

    /* create name, type, zone: */
    atnbp_mkname(nve, ifname, ntype, STR32PTR(&ifzone));
}

/*
 * atalkif_register -
 *	private routine to register NBP port name
 */
static boolean
atalkif_register (atalkidbtype *idb)
{
    nbpEntity	nve;
    nbp_result	err;
    ataddress	src;

    /* generate NBP name for this interface: */
    atalkif_NBPname(&nve, idb);
    if (at_debug(atalknbp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
        buginf("\nAT: atalkif_NBPname is '%s'\n", atnbp_entityStr(&nve));

    /*
     * set the entity source address.  register on the NBP
     * socket, since we have no real service available.
     */
    src.net  = idb->atalknet;
    src.node = idb->atalknode;
    src.spec = ATALK_SOCK_OURNAME;

    err = atnbp_register(&nve, &src, TRUE);
    if (err != NBP_REG_PENDING) {
	/* CSCdi06171 - don't fuss, the interface flopped on the way up. */
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("AT:  couldn't register '%s'@[%s]:  %s\n",
		atnbp_entityStr(&nve), ddpAddrStr(&src), atnbp_resultStr(err));
        return (FALSE);
    } else {
	if (at_debug(atalk_events, ATALK_INTERFACE_MASK, idb->phys_idb))
	    errmsg(AT_ERR_REGPENDING, atnbp_entityStr(&nve), ddpAddrStr(&src));
    }

    if (at_debug(atalknbp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
        atnbp_nve_dump();

    return (TRUE);
}

/*
 * atalkif_unregister -
 *	unregister all NBP entities on <idb>
 */
boolean
atalkif_unregister (atalkidbtype *idb)
{
    ataddress	ddp;

    ddp.node = atalkif_MyNode(idb);
    ddp.net  = atalkif_MyNet(idb);
    ddp.spec = 0;

    /* unregister everything on interface: */
    return (atnbp_DDPunregister(&ddp) == NBP_OK);
}

/*
 * atalkif_InterfaceValid
 * Verify that the port is compatible with the current AT internet.
 */
boolean
atalkif_InterfaceValid (atalkidbtype* idb)
{
    boolean     valid;

    atfast_SetupIF(idb);		/* set up the fast-switch cache */

    if (!interface_up(idb->phys_idb))
	return (FALSE);
    if ((idb->atalk_cablestart == ATALK_NET_UNNUMBERED) ||
	(idb->atalk_cablestart == 0))
	return (TRUE);			/* discovery-style interface */

    
    valid = TRUE;
    if (idb->atalk_cablestart >= ATALK_STARTUP_START)
	valid = FALSE;
    if (atalkif_net_extended(idb)) {
	if ((idb->atalk_cableend < idb->atalk_cablestart) ||
	    (idb->atalk_cableend >= ATALK_STARTUP_START))
	    valid = FALSE;
    }
    if (!valid) {
	errmsg(AT_ERR_NETINVALID, idb_get_namestring(idb->phys_idb));
    }
    return (valid);
}

/*
 * atalkif_CreateConnectedPath
 *
 * Create a 'connected' path for the specified IDB.
 */
atalk_pdb_t*
atalkif_CreateConnectedPath (atalkidbtype *idb)
{
    atalk_rdb_t *r = NULL;
    atalk_pdb_t *p = NULL;
    rbTree	*p_tree;
    atalk_metric_t metric;
    boolean	valid, addit = FALSE;
    ushort	from = idb->atalk_cablestart;
    ushort	to   = idb->atalk_cableend;

    if (!atalkif_InterfaceConfigured(idb))
	return (NULL);
    /*
     * See if there is already a connected path with this cable range
     * in the path tree. If there is, then we need to check 
     * our new configuration against what is already there.
     */
    p_tree = atroute_GetPTree(AT_PATH_CONNECTED);
    p = atroute_GetValidPath(p_tree, from, to, &valid, FALSE);

    if (!valid) {
	errmsg(AT_ERR_NETINVALID, idb_get_namestring(idb->phys_idb));
	return (NULL);
    }
    /*
     * If we find an existing entry, it had better be a) connected and b)
     * the entry for this interface.
     * If it isn't, then what we have is another interface configured
     * with the same network # or cable range. A no-no.
     *
     */
    if (p) {
	if (p->idb != idb) {
	    errmsg(AT_ERR_IFCONFLICT, idb_get_namestring(idb->phys_idb),
		   idb_get_namestring(p->idb->phys_idb));
	    return (NULL);
	}
    } else
	addit = TRUE;

    r = atroute_GetValidRoute(atroute_GetTree(), from, to, &valid, FALSE);
    if (!valid) {
	errmsg(AT_ERR_NETINVALID, idb_get_namestring(idb->phys_idb));
	return (NULL);
    }

    /* If route is pending removal, delete it. */

    if (r && r->action == at_delete_route) {
	atroute_DeleteRoute(r);
	r = NULL;
    }

    if (r) {
	/*
	 * Check that zone lists match.  But only if the route's zonelist
	 * is not being updated and one exists.
	 */
	if (!idb->atalk_discovery && !r->zoneupdate && r->zonelist &&
	    !atmaint_ZonesMatch(idb, r) && !atalk_ignoreVerifyErrs) {
	    printf("\n%s: AppleTalk zone list incompatible with active "
		   "internet", idb_get_namestring(idb->phys_idb));
	    return (NULL);
	} 
	if (!p) {
	    /*
	     * If there was no connected path, then create one.  Delete
	     * this route, though, because it was learned from the net.
	     * Actually, it may not be really necessary to delete it.
	     */
	    addit = TRUE;
	    atroute_DeleteRoute(r);
	}
    }
    
    if (addit) {
	memset(&metric, 0, sizeof(metric));
	metric.metric = AT_METRIC_CONNECTED;
	metric.type = AT_PATH_CONNECTED;
	p = atroute_AddPath(p_tree, idb,
			    idb->atalk_cablestart,idb->atalk_cableend,
			    NULL, atroute_DefaultPathAction, NULL,
			    0, AT_PATH_CONNECTED,
			    &metric);
	
    }
    return (p);
}

/*
 * atalkif_SetOperational - Do the final tasks necessary to go 'online''
 */
void
atalkif_SetOperational (atalkidbtype* idb)
{
    ataddrtype src;
    src = idb->atalk_confaddr;
    ATASSERT(idb != NULL);
    ATASSERT(atalkif_opstatus(idb) == opValid);
    atalkif_InstallZones(idb);
    idb->atalk_operational = TRUE;
    atfast_SetupIF(idb);

    /* register NBP port name: */
    if (!atalkif_CaymanTunnel(idb) && !atalkif_AURPTunnel(idb))
	atalkif_register(idb);	

    /* register SNMP servers */
    if (!atalkif_CaymanTunnel(idb))
        atalk_snmp_register();

/*    atalkif_ResetMulticasts(idb);  */
    if (atalk_nodeonly(src) != 0) {
	errmsg(AT_ERR_CONFIGOK, idb_get_namestring(idb->phys_idb), src);
    } else {
	if (!atalkif_CaymanTunnel(idb) && !atalkif_AURPTunnel(idb))
	    errmsg(AT_ERR_ONLYROUTER, idb_get_namestring(idb->phys_idb));
    }

    /*
     * Make sure that this falls after the idb has been marked operational.
     */
    reg_invoke_atalk_if_operational(idb);

    /* Redistribute this connected path to all protocols */
    if (atalkif_GetIdbPath(idb))
	atroute_path_redist_rbshell((treeLink *)atalkif_GetIdbPath(idb),
				    FALSE);

}

/*
 * atalkif_opstatusmsg - AppleTalk port status message.
 */
char*
atalkif_opstatusmsg (atalkidbtype* idb)
{
    opStatus op;
    op = atalkif_opstatus(idb);
    switch (op) {
      case opValid:
	return "Operational";
      case opNoAddress:
	return "Unable to obtain node address";
      case opRestartPort:
	return "Restart port pending";
      case opAcquire:
	return "Acquiring port net information";
      case opAcquirePeer:
	return "Acquiring remote peer";
      case opAcquireZones:
	return "Acquiring port zone information";
      case opVerify:
	return "Verifying port net information";
      case opVerifyZones:
	return "Verifying port zone information";
      case opValidate:
	return "Validating port configuration";
      case opCompatBad:
	return "Port violates internet compatibility";
      case opConfigBad:
	return "Port configuration error";
      case opCableMismatch:
	return "Port configuration mismatch";
      case opProbing:
	return "Probing for node address";
      case opNetDown:
	return "Line protocol is down";
      case opHRSupportErr:
	return "Port configuration error";
      case opRemotePeerErr:
	return "Remote peer configuration mismatch";
      case opLineProtoErr:
	return "Incompatible with line protocol";
      case opNetConflict:
	return "Conflicts with existing AT internet";
      case opInternalError:
	return "Internal error (recoverable)";
      case opInternalBug:
	return "Internal failure (irrecoverable)";
      case opUnknown:
	return "Unknown status";
      case opResExhausted:
	return "Insufficient free buffers";
    }
    return "Unknown status";
}

/*
 * atalkif_ShortStatus - Status descriptors for debug msgs
 */
char*
atalkif_ShortStatus (atalkidbtype* idb)
{
    opStatus op;
    op = atalkif_opstatus(idb);
    switch (op) {
      case opValid:
	return "operational";
      case opNoAddress:
	return "no addresses";
      case opRestartPort:
	return "restarting";
      case opAcquire:
	return "acquiring";
      case opAcquirePeer:
	return "peer wait";
      case opAcquireZones:
	return "requesting zones";
      case opVerify:
	return "verifying";
      case opVerifyZones:
	return "checking zones";
      case opValidate:
	return "final validation";
      case opCompatBad:
	return "not compatible";
      case opConfigBad:
	return "invalid config";
      case opCableMismatch:
	return "config mismatch";
      case opProbing:
	return "probing";
      case opNetDown:
	return "line down";
      case opHRSupportErr:
	return "HR error";
      case opRemotePeerErr:
	return "peer mismatch";
      case opLineProtoErr:
	return "bad line proto";
      case opNetConflict:
	return "conflicts with internet";
      case opInternalError:
	return "bug";
      case opInternalBug:
	return "permanent bug";
      case opUnknown:
	return "unknown";
      case opResExhausted:
	return "no buffers";
    }
    return "unhandled";
}

/* 
 * atalkif_addrstatemsg
 */
char*
atalkif_addrstatemsg (atalkidbtype* idb)
{
    switch (idb->aarp_tentative) {
      case AARP_ADDR_UNKNOWN:
	return "Unknown";
      case AARP_ADDR_SET:
	return "Valid";
      case AARP_ADDR_TENTATIVE:
	return "Probing";
      case AARP_ADDR_FAILED:
	return "Bad";
      case AARP_ADDR_INVALID:
	return "Invalid";
    }
    return "Unknown";
}		    

char*
atalk_state_name (at_state_t code)
{
    switch (code) {
      case at_state_good:
	return ("Good");
      case at_state_suspect:
	return ("Suspect");
      case at_state_bad:
      case at_state_bad1:
	return ("Bad");
      case at_state_poisoned:
	return ("Poisoned");
      case at_state_invalid:
      default:
	return ("Unknown");
    }
}

/*
 * atalkif_IsMyAddress
 * Check if the address matches my interface but only if AARP is up
 */
boolean
atalkif_IsMyAddress (atalkidbtype* idb,ataddrtype addr)
{
    if (addr == 0)		/* special check for error ret value */
	return (FALSE);
    if (atalkif_MyAddress(idb) == addr)
	return (TRUE);
    return (FALSE);
}


/*
 * appletalk_zonename
 * Return AppleTalk zone names to handle funny characters
 */
void
appletalk_zonename (uchar *src, uint len, char *dst)
{
    uint i = 0;

    while (i < len) {
	if (src[i] >= 0x7f || src[i] < 0x20) {
	    *dst++ = ':';
	    *dst++ = tohexchar(src[i] >> 4);
	    *dst++ = tohexchar(src[i]);
	} else
	    *dst++ = src[i];
	i++;
    }
    *dst = '\0';
}

/*
 * If the first character is non-graphic (i.e. whitespace, control, or 
 * 8 bit), print it out using the special sequence.
 * Used for writing zone names into nvram.
 * It also takes care of the last character if it is a white one 
 * in the same manner.
 */
void
atalk_nv_printzone (char *zonename)
{
   
    unsigned char zonechar = zonename[0];
    unsigned short length = strlen(zonename);

    /* First, print out all but last character, using the
       special notation for the first character, if necessary. */
    if (!ISGRAPHIC_ASCII(zonechar))
        nv_add(TRUE, ":%2x%*s", zonechar, length - 2, zonename + 1);
    else
        nv_add(TRUE, "%*s", length - 1, zonename);
    
    /* Second, print out the last character, using the special notation,
       if necessary. */
    if (!ISGRAPHIC_ASCII(zonechar = zonename[length - 1]))
        nv_add(TRUE, ":%2x", zonechar);
    else
        nv_add(TRUE, "%c", zonechar);
}

/*
 * AppleTalk zone/net data structure routines
 */

/* 
 * atalk_PrimaryZone
 * Get a pointer to the primary zone for a route.
 */
ziptabletype*
atalk_PrimaryZone (atalk_rdb_t* net)
{
    zipnetentry* zn;
    if (net == NULL)
	return (NULL);
    zn = atalk_firstZone(net);
    if (zn != NULL)
	return zn->zone;
    return (NULL);
}

/*
 * atalk_findIDB
 *
 * Find an IDB given a network #, which meets the following criteria:
 * - the network number is valid (either non-extended, extended, valid
 *   for routing)
 * - the interface on which the network # is visible is operational
 *   (ie, idb is up and Atalk is operational on the interface)
 */
atalkidbtype* 
atalk_findIDB (ushort net)
{
    atalkidbtype *idb;
    
    FOR_ALL_ATIDBS(idb) {
	if (!atalkif_operational(idb))
	    continue;
	if (atalkif_srcnettype(idb,net) >= netAllValid)
	    return idb;
    }
    return (NULL);
}

/* atalkif_primary_zone - Get a pointer to the primary zone for interface */
ziptabletype*
atalkif_primary_zone (atalkidbtype* idb)
{
    zipifentry* z = idb->zonelist;
    if (z == NULL)
	return (NULL);
    return (z->zone);		/* first zone is primary */
}

zipifentry*
atalkif_firstZone (atalkidbtype* idb)
{
    zipifentry* z = idb->zonelist;
    if (z == NULL)
	return (NULL);
    return (z);
}

zipifentry*
atalkif_nextZone (zipifentry* z)
{
    if (z != NULL)
	return (z->next);
    else
	return (NULL);
}

boolean
atalkif_hasSingleZone (atalkidbtype* idb)
{
    zipifentry* z = atalkif_firstZone(idb);
    if (z == NULL)
	return (FALSE);
    if (z->next != NULL)
	return (FALSE);
    return (TRUE);
}

/*
 * atalkif_findZone
 * Find a zone associated with this interface.
 */
zipifentry*
atalkif_findZone (atalkidbtype* idb,ziptabletype* zone)
{
    zipifentry* z = atalkif_firstZone(idb);
    ATASSERT(zone != NULL);
    while (z != NULL) {
	if (z->zone == zone)
	    return (z);
	z = atalkif_nextZone(z);
    }
    return (NULL);
}

/*
 * atalkif_addZone
 * Add a zone to an interface zone list
 */
boolean
atalkif_addZone (atalkidbtype* idb,ziptabletype* zone)
{
    zipifentry* zfirst;
    zipifentry* zprev;
    zipifentry* z = atalkif_findZone(idb,zone);
    ATASSERT(zone != NULL);
    if (z != NULL)
	return (z != NULL);
    z = malloc(sizeof(struct zipifentry_));
    if (z == NULL)
	return (FALSE);
    zifCreated++;
    z->zone = zone;
    z->next = NULL;
    zfirst = atalkif_firstZone(idb);
    if (zfirst == NULL) {
	idb->zonelist = z;
	atalkif_SetOurZone(idb,zone);
    } else {
	/*
	 * Put zone at END of list so 'write term' shows zones in order
	 * created.
	 */
	for (zprev = zfirst; zprev->next != NULL; zprev = zprev->next)
	  ;
	zprev->next = z;
    }
    zone->ifcount++;
    idb->atalk_zonecount++;
    return (TRUE);
}

/*
 * atalkif_addPrimary
 * Add a zone as the primary to an interface zone list
 */
static boolean
atalkif_addPrimary (atalkidbtype* idb,ziptabletype* zone)
{
    zipifentry* z = atalkif_findZone(idb,zone);
    if ((z != NULL) || (zone == NULL))
	return (FALSE);
    z = malloc(sizeof(struct zipifentry_));
    if (z == NULL)
	return (FALSE);
    zifCreated++;
    z->zone = zone;
    z->next = idb->zonelist;
    idb->zonelist = z;
    atalkif_SetOurZone(idb,zone);
    zone->ifcount++;
    idb->atalk_zonecount++;
    return (TRUE);
}

/*
 * atalkif_SetOurZone
 * Set the zone which this interface will consider the default and the zone
 * used to register any interface entities.
 */
void 
atalkif_SetOurZone (atalkidbtype* idb,ziptabletype* zone)
{
    uchar const *mc;
    
    if ((zone == NULL) && (idb->atalk_ifinit))
	return;
    if (zone == NULL)
	mc = zeromac;
    else 
	mc = atalk_zone_multicast(idb,atalk_zonesum(zone->name,zone->length));
    atalkif_SetOurMulticasts(idb,mc);
}

/*
 * atalkif_ResetMulticasts
 * If necessary, we perform an interface reset to reload the address filters.
 */
void
atalkif_ResetMulticasts (atalkidbtype* idb)
{
    hwidbtype *hwidb;
    
    if (idb->atalk_ifinit)
	return;

    hwidb = idb->phys_idb->hwptr;
    
    /* Don't reset if we're an async or virtual access interface */
    if (is_async(hwidb) || IS_VACCESS(hwidb))
	return;

    if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\n%s: AT: Resetting interface address filters",
	       idb_get_namestring(idb->phys_idb));
    if (hwidb->set_extra_addrs != NULL) 
	idb->atalk_ifinit = (*hwidb->set_extra_addrs)(hwidb);
    if (idb->atalk_ifinit)
	return;
    if (at_debug(atalk_events, ATALK_INTERFACE_MASK, idb->phys_idb))
	errmsg(AT_ERR_INTRESTART, idb_get_namestring(idb->phys_idb));
    if (!is_atm(hwidb)) {
	(*hwidb->reset)(hwidb);
    }
    idb->atalk_ifinit = TRUE;
}

/*
 * atalkif_ClearOurMulticasts
 * This routine determines if an interface needs its address filters
 * reset.
 */
void
atalkif_ClearOurMulticasts (atalkidbtype* idb)
{
    if (idb->atalk_enabled && atalk_running)
	return;
    idb->atalk_ifinit = FALSE;
}

/*
 * atalkif_SetOurMulticasts
 * Set the zone multicast address for an interface.  This only needs to happen
 * initially (dictated by atalk_ifinit) and when it changes (compared to the
 * last init value in atalk_zone_multicast).  If we are attemtping to disable
 * an address, we do not perform that operation unless appletalk is not running
 * or it is disabled for this interface.  Otherwise we can loop during 
 * discovery mode.
 */
void
atalkif_SetOurMulticasts (atalkidbtype* idb, uchar const *mc)
{
    atalkif_ClearOurMulticasts(idb);
    if (mc == NULL) {
	/* If we are init'd, ignore clearing the mc */
	if (idb->atalk_ifinit)
	    return;
	ieee_zero(idb->atalk_zone_multicast);
    } else {
	/* If the mc is unchanged AND we are init */
	if (ieee_equal((uchar *)mc, idb->atalk_zone_multicast) &&
	    (idb->atalk_ifinit))
		return;
	    ieee_copy(mc, idb->atalk_zone_multicast);
    }
    /* Finally, we really need to setup the if */
    idb->atalk_ifinit = FALSE;
    atalkif_ResetMulticasts(idb);
}

/*
 * atalkif_clear_zone
 * Remove specified zone from interface zone list.
 */
void
atalkif_clear_zone (atalkidbtype* idb,ziptabletype* zone)
{
    zipifentry* zprev = NULL;
    zipifentry* z = atalkif_firstZone(idb);
    ziptabletype* idbzone;
    
    if (zone == NULL)
	return;
    while (z != NULL) {
	if (z->zone == zone) {
	    if (zprev == NULL)
		idb->zonelist = z->next;
	    else
		zprev->next = z->next;
	    atalk_FreeZifnet(z,idb);
	    idbzone = atalkif_primary_zone(idb);
	    atalkif_SetOurZone(idb,idbzone); /* NULL will reset */
	    return;
	}
	zprev = z;
	z = z->next;
    }
}

void
atalkif_clear_zones (atalkidbtype *idb)
{
    zipifentry* znext = NULL;
    zipifentry* z = atalkif_firstZone(idb);
    while (z != NULL) {
	znext = z->next;
	atalk_FreeZifnet(z,idb);
	z = znext;
    }
    idb->zonelist = NULL;
    atalkif_SetOurZone(idb,NULL);
}

boolean
atalkif_set_primary (atalkidbtype* idb,ziptabletype* zone)
{
    zipifentry* zfirst;
    atalk_rdb_t* r;

    ATASSERT(idb != NULL);
    ATASSERT(zone != NULL);
    r = atroute_GetIdbRoute(idb);
    atalkif_clear_route_zones(idb);
    atalkif_clear_zone(idb,zone);
    atalkif_addPrimary(idb,zone);
    zfirst = atalkif_firstZone(idb);
    if ((r != NULL) && (zfirst != NULL))
	atalk_add_zonelist(r,zfirst);
    return (TRUE);
}

void
atalk_add_zonelist (atalk_rdb_t* net,zipifentry* z)
{
    if ((net == NULL) || (z == NULL))
	return;
    atalk_clear_zones(net);
    util_FreeZipnetChain(&net->zonelist,zipnet_viaNetChain);
    while (z != NULL) {
	if (z->zone != NULL)
	    atalk_join_zipnet(net,z->zone);
	z = z->next;
    }
}

static void
atalkif_add_zonelist (atalkidbtype* idb,zipnetentry* z)
{
    ATASSERT(idb != NULL);
    ATASSERT(z != NULL);
    atalkif_clear_zones(idb);
    while (z != NULL) {
	if (z->zone != NULL)
	    atalkif_addZone(idb,z->zone);
	z = atalkzn_nextZone(z);
    }
}

void
atalkif_AcquireZones (atalkidbtype* idb)
{
    zipnetentry *zfirst;
    atalk_rdb_t *r;
    
    ATASSERT(idb != NULL);
    r = atroute_GetIdbRoute(idb);
    if (r == NULL)
	return;
    zfirst = atalk_firstZone(r);
    if (zfirst != NULL)
	atalkif_add_zonelist(idb,zfirst);
}

boolean
atalkif_CheckZones (atalkidbtype* idb)
{
    zipnetentry* z;
    atalk_rdb_t* r;
    int zcnt = 0;
    
    ATASSERT(idb != NULL);
    r = atroute_GetIdbRoute(idb);
    if (r == NULL)
	return (FALSE);
    z = atalk_firstZone(r);
    while (z != NULL) {
	if (atalkif_findZone(idb,z->zone))
	    zcnt++;
	else
	    return (FALSE);
	z = atalkzn_nextZone(z);
    }
    return (zcnt == idb->atalk_zonecount);
}

void
atalkif_InstallZones (atalkidbtype* idb)
{
    zipifentry* zfirst;
    atalk_rdb_t* r;

    ATASSERT(idb != NULL);
    r = atroute_GetIdbRoute(idb);
    atalkif_clear_route_zones(idb);
    zfirst = atalkif_firstZone(idb);
    if ((r != NULL) && (zfirst != NULL))
	atalk_add_zonelist(r,zfirst);
}

void
atalkif_clear_route_zones (atalkidbtype *idb)
{
    atalk_rdb_t* net = atroute_GetIdbRoute(idb);
    if (net != NULL)
	atalk_clear_zones(net);
}

/*
 * atalk_firstZone
 * Get a pointer to the first zone 
 */
zipnetentry*
atalk_firstZone (atalk_rdb_t* net)
{
  if (net != NULL)
    return (util_FirstZipnet(net->zonelist,zipnet_viaNetChain,
			     AT_UTIL_ZIPNET_NO_DELETED));
  return (NULL);
}

/* 
 * atalk_firstNet
 * Get a pointer to the first valid net 
 */
zipnetentry*
atalk_firstNet (ziptabletype* zone)
{
    zipnetentry* zn;
    
    if (zone == NULL) 
	return (NULL);
    zn = util_FirstZipnet(zone->netlist,zipnet_viaZoneChain,
			  AT_UTIL_ZIPNET_NO_DELETED);
    while (zn != NULL) {
	if (zn->net != NULL)
	    return (zn);
	zn = util_NextZipnet(zn,zipnet_viaZoneChain,
			     AT_UTIL_ZIPNET_NO_DELETED);
    }
    return (NULL);
}

/*
 * atalkif_GetIdbPath
 * Return the pdb which is stashed in the idb for quick access.
 */
atalk_pdb_t*
atalkif_GetIdbPath (atalkidbtype* idb)
{
    if (idb->atalk_path != NULL)
	return idb->atalk_path;
    return (NULL);
}

/* 
 * atalk_only_zone
 * Get a pointer to a zone iff there is one. 
 */
zipnetentry*
atalk_only_zone (atalk_rdb_t* net)
{
    zipnetentry* zn = NULL;
    
    if (net != NULL) {
	zn = util_FirstZipnet(net->zonelist,zipnet_viaNetChain,
			      AT_UTIL_ZIPNET_NO_DELETED);
	if (util_NextZipnet(zn,zipnet_viaNetChain,
			    AT_UTIL_ZIPNET_NO_DELETED) == NULL)
	    return zn;
    }
    return NULL;
}

/* 
 * atalkzn_nextZone
 * Get the next pointer to a zone iff there is one. 
 */
zipnetentry*
atalkzn_nextZone (zipnetentry* zn)
{
    return util_NextZipnet(zn,zipnet_viaNetChain, AT_UTIL_ZIPNET_NO_DELETED);
}

/* 
 * atalk_firstNet
 * Get a pointer to the first valid net 
 */
zipnetentry*
atalkzn_nextNet (zipnetentry* zn)
{
    while (zn != NULL) {
	zn = util_NextZipnet(zn,zipnet_viaZoneChain,
			     AT_UTIL_ZIPNET_NO_DELETED);
	if (zn && zn->net != NULL)
	    return zn;
    }
    return NULL;
}

/*
 * atalk_CountAndFilterZones (atalk_rdb_t *net)
 *
 * Return the count of zones on the network, excluding those filtered out
 * by access control (no effect if list == 0).
 */
int atalk_CountAndFilterZones (atalk_rdb_t *net, int list)
{
    zipnetentry *zn;
    int count;
    for (count = 0, zn = atalk_firstZone(net); zn != NULL;
	 zn = atalkzn_nextZone(zn))
	if (atalk_zonecheck(zn->zone, list))
	    count++;
    return(count);
}

/*
 * atalk_ZoneListLengthAndCount (atalk_rdb_t *net)
 *
 * Return the number and total length of a net's zone list, excluding those
 * filtered out by access control (no effect if list == 0).
 */
void
atalk_FilteredZoneListLengthAndCount (atalk_rdb_t *net, int *length,
				      int *count, int list)
{
    zipnetentry *zn;

    *count = atalk_CountAndFilterZones(net, list);
    
    for (*length = 0, zn = atalk_firstZone(net); zn != NULL;
	 zn = atalkzn_nextZone(zn)) {
	if (atalk_zonecheck(zn->zone, list))
	    *length += zn->zone->length;
    }
}

/*
 * atalkif_PrimaryZoneName
 * Returns a printable string for the primary zone of an interface.
 */
char*
atalkif_PrimaryZoneName (atalkidbtype* idb)
{
    ziptabletype* z;
    if (idb == NULL)
	return "?";
    z = atalkif_primary_zone(idb);
    if (z != NULL)
	return z->pname;
    else
	return "not set";
}


/*
 * atalk_path_internal
 * Check if a path has the internal flag set 
 */
boolean
atalk_path_internal(atalk_pdb_t* p)
{
    if (p == NULL)
	return (FALSE);
    return(p->internal);
}

static boolean
atalkzn_net_extended (zipnetentry* zn)
{
    if (zn == NULL)
	return (FALSE);
    return atalk_net_extended(zn->net);
}

boolean
atalkif_ValidAddress (atalkidbtype* idb, ataddrtype addr)
{
    uchar node;
    netTypes stat;

    ATASSERT(idb != NULL);
    if ((!idb->atalk_enabled) || (!atalk_running))
	return (FALSE);
    if (atalkif_HalfRouter(idb))
	return (TRUE);
    node = atalk_nodeonly(addr);
    switch (node) {
      case ATALK_ANY_ROUTER:
      case ATALK_BROAD_ADDR:
	return (FALSE);
      case ATALK_ETHER_BADADDR:
	if (atalkif_net_extended(idb))
	    return (FALSE);
    }
    stat = atalkif_dstnettype(idb,atalk_netonly(addr));
    switch (stat) {
      case netStartupRange:
      case netLocalNet:
	return (TRUE);
      default:
	break;
    }
    return (FALSE);
}

/*
 * atalkif_ValidFQAddress
 * Check validity of fully qualified address
 */
boolean
atalkif_ValidFQAddress (atalkidbtype* idb, ataddrtype addr)
{
    ATAddress fqa;
    netTypes stat;
    fqa.l = addr;

    switch (fqa.f.node) {
      case ATALK_ANY_ROUTER:
      case ATALK_BROAD_ADDR:
	return (FALSE);
      case ATALK_ETHER_BADADDR:
	if (atalkif_net_extended(idb))
	    return (FALSE);
    }
    stat = atalkif_dstnettype(idb,fqa.f.net);
    switch (stat) {
      case netLocalNet:
	return (TRUE);
      default:
	break;
    }
    return (FALSE);
}

/*
 * atalk_LookupNodeName
 * Return the name for an address if any and handle special cases
 */
static char*
atalk_LookupNodeName (ataddrtype addr)
{
    if (atalk_nameLookupInterval == 0)
	return (NULL);
    if (atalk_nodeonly(addr) == ATALK_BROAD_ADDR)
	if (atalk_netonly(addr) == 0)
	    return ("cable-broadcast");
	else
	    return (NULL);
    return (atmaint_GetNodeName(addr));
}

char *
atalkif_nodename (atalkidbtype* idb)
{
    if (idb->atalk_nodename == NULL) {
	char buffer[MAX_ATNODE_NAME];
	int len = sprintf(buffer,"%s:%a", idb_get_namestring(idb->phys_idb),
			  (atalkif_CaymanTunnel(idb) ? 0
			   : atalkif_MyAddress(idb)));
	idb->atalk_nodename = malloc(len+1);
	if (idb->atalk_nodename != NULL) {
	    bcopy((uchar*)buffer,(uchar*)idb->atalk_nodename,len);
	    idb->atalk_nodename[len] = '\0';
	}
    }
    return (idb->atalk_nodename);
}

char *
atalkif_netname (atalkidbtype* idb)
{
    if (idb->atalk_netname == NULL) {
	char buffer[MAX_ATNET_NAME];
	int len;
	ushort start = idb->atalk_cablestart;
	ushort end = idb->atalk_cableend;
	if (atalkif_CaymanTunnel(idb))
	    len = sprintf(buffer, "Cayman-Tunnel");
	else if (end != 0) {
	    if (end == ATALK_RANGE_UNKNOWN)
	        end = 0;
	    if (atalk_alternateAddrs)
		len = sprintf(buffer,"%d-%d (%d.%d-%d.%d)",start, end,
			      start >> 8, start & 0xff,
			      end >> 8, end & 0xff);
	    else
		len = sprintf(buffer,"%d-%d",start, end);
	}
	else if (start == ATALK_NET_UNNUMBERED)
	    len = sprintf(buffer,"half-router");
	else if (atalk_alternateAddrs)
	    len = sprintf(buffer,"%d (%d.%d)",start, start>>8, start & 0xff);
	else
	    len = sprintf(buffer,"%d",start);
	idb->atalk_netname = malloc(len+1);
	if (idb->atalk_netname != NULL) {
	    bcopy((uchar*)buffer,(uchar*)idb->atalk_netname,len);
	    idb->atalk_netname[len] = '\0';
	}
    }
    return (idb->atalk_netname);
}

/*
 * atalkif_HalfRouter - Return flag indicating this is a half router.
 */
boolean
atalkif_HalfRouter (atalkidbtype* idb)
{
    if ((idb->atalk_cablestart == ATALK_NET_UNNUMBERED) &&
	(idb->atalk_cableend == ATALK_NET_UNNUMBERED))
	return (TRUE);
    return (FALSE);
}

/*
 * atalkif_CaymanTunnel - Return flag indicating this is a Cayman Tunnel.
 */
boolean
atalkif_CaymanTunnel (atalkidbtype *idb)
{
    tunnel_info *tinfo;

    if (!idb || !idb->phys_idb->hwptr)
	return (FALSE);

    tinfo = idb->phys_idb->hwptr->tunnel;

    if (tinfo && (tinfo->mode == TUN_MODE_CAYMAN_UDP))
	return (TRUE);
    return (FALSE);
	
}

/*
 * atalkif_IPTalkTunnel - Return flag indicating this is a IPTalk Tunnel.
 */
boolean
atalkif_IPTalkTunnel (atalkidbtype *idb)
{
    tunnel_info *tinfo;

    if (!idb || !idb->phys_idb->hwptr)
	return (FALSE);

    tinfo = idb->phys_idb->hwptr->tunnel;

    if (tinfo && (tinfo->mode == TUN_MODE_IPTALK_UDP))
	return (TRUE);
    return (FALSE);
	
}

/*
 * atalkif_AURPTunnel - Return flag indicating this is an AURP Tunnel.
 */
boolean
atalkif_AURPTunnel (atalkidbtype *idb)
{
    tunnel_info *tinfo;

    if (!idb || !idb->phys_idb->hwptr)
	return (FALSE);

    tinfo = idb->phys_idb->hwptr->tunnel;

    if (tinfo && tinfo->aurp)
	return (TRUE);
    return (FALSE);
}

boolean
atalkif_ForAURP (idbtype *idb)
{
    tunnel_info *tinfo;

    if (!idb || !idb->hwptr)
	return (FALSE);

    tinfo = idb->hwptr->tunnel;

    if (tinfo && tinfo->aurp)
	return (TRUE);

    if (tinfo && (tinfo->mode == TUN_MODE_AURP_UDP))
	return (TRUE);

    return (FALSE);
}

/*
 * atalkif_ClearNames - Clear the dynamically allocated AppleTalk names.
 */
void
atalkif_ClearNames (atalkidbtype* idb)
{
    uchar* tmp = NULL;
    if ((tmp = (uchar*)idb->atalk_netname) != NULL)  {
	idb->atalk_netname = NULL;
	free(tmp);
    }
    if ((tmp = (uchar*)idb->atalk_nodename) != NULL)  {
	idb->atalk_nodename = NULL;
	free(tmp);
    }
}

void
atalk_ddpname (ddpTypes pt,char* buffer)
{
    switch (pt) {
    case DDP_TYPE_RTMPRsp:
	sprintf(buffer,"%s","RTMPRsp");
	break;
    case DDP_TYPE_RTMPReq:
	sprintf(buffer,"%s","RTMPReq");
	break;
    case DDP_TYPE_NBP:
	sprintf(buffer,"%s","NBP");
	break;
    case DDP_TYPE_ATP:
	sprintf(buffer,"%s","ATP");
	break;
    case DDP_TYPE_ECHO:
	sprintf(buffer,"%s","AEP");
	break;
    case DDP_TYPE_ZIP:
	sprintf(buffer,"%s","ZIP");
	break;
    case DDP_TYPE_ADSP:
	sprintf(buffer,"%s","ADSP");
	break;
    case DDP_TYPE_SNMP:
	sprintf(buffer,"%s","SNMP");
	break;
    case DDP_TYPE_SMRP:
	sprintf(buffer,"%s","SMRP");
	break;
    default:
	sprintf(buffer,"pt 0x%-04x",pt);
    }
}

void
atalk_netname (ushort start, ushort end, char* buffer)
{
    buffer[0] = '\0';
    if (end != 0) {
        if (end == ATALK_RANGE_UNKNOWN)
	    end = 0;
	if (atalk_alternateAddrs)
	    sprintf(buffer,"%d-%d (%d.%d-%d.%d)",start, end,
		    start >> 8, start & 0xff,
		    end >> 8, end & 0xff);
	else
	    sprintf(buffer,"%d-%d",start, end);
    }
    else if (start == ATALK_NET_UNNUMBERED)
	sprintf(buffer,"half-router");
    else if (atalk_alternateAddrs)
	sprintf(buffer,"%d (%d.%d)",start, start>>8, start & 0xff);
    else
	sprintf(buffer,"%d",start);
    return;
}

/*
 * atalkif_CableBroadcast
 * Returns the AppleTalk cable broadcast address for an interface.
 */
ataddrtype
atalkif_CableBroadcast (atalkidbtype* idb)
{
    if (atalkif_net_extended(idb))
	return atalk_address(ATALK_CABLE_ALLNETS,ATALK_BROAD_ADDR);
    return atalk_address(atalkif_MyNet(idb),ATALK_BROAD_ADDR);
}

zipnetentry*
atalkzn_netname (zipnetentry* zn,char* buffer)
{
    buffer[0] = '\0';
    if (zn == NULL)
	return NULL;
    if (atalk_alternateAddrs) {
	if (atalkzn_net_extended(zn))
	    sprintf(buffer,"%d-%d (%d.%d-%d.%d)",
		    (zn->net)->dstrngstr, (zn->net)->dstrngend,
		    (zn->net)->dstrngstr >> 8, (zn->net)->dstrngstr & 0xff,
		    (zn->net)->dstrngend >> 8, (zn->net)->dstrngend & 0xff);
	else
	    sprintf(buffer,"%d (%d.%d)",(zn->net)->dstrngstr,
		    (zn->net)->dstrngstr >> 8, (zn->net)->dstrngstr & 0xff);
    } else {
	if (atalkzn_net_extended(zn))
	    sprintf(buffer,"%d-%d",
		    (zn->net)->dstrngstr, (zn->net)->dstrngend);
	else
	    sprintf(buffer,"%d",(zn->net)->dstrngstr);
    }
    return zn;
}

/* 
 * atalk_find_and_use_zipnet
 * Get a pointer to zipnetentry if available, and set it for being used.
 * If currently in zipnet structure, but deleted, mark it as undeleted.
 */
zipnetentry*
atalk_find_and_use_zipnet (atalk_rdb_t* net,ziptabletype* zone)
{
    zipnetentry* zn = NULL;
    if ((net == NULL) || (zone == NULL))
	return NULL;
    zn = util_FirstZipnet(net->zonelist,zipnet_viaNetChain,
			  AT_UTIL_ZIPNET_ACCEPT_ANY);
    while (zn != NULL) {
	if ((zn->zone == zone) && (zn->net == net)) {
	    zn->deleted = FALSE;
	    return zn;
	}
	zn = util_NextZipnet(zn,zipnet_viaNetChain, AT_UTIL_ZIPNET_ACCEPT_ANY);
    }
    return NULL;
}

/* 
 * atalk_join_zipnet
 * Join zone and net.
 */
zipnetentry*
atalk_join_zipnet (atalk_rdb_t* net,ziptabletype* zone)
{
    zipnetentry* zn = atalk_find_and_use_zipnet(net,zone);
    zipnetentry* nzn = NULL;

    if ((zn != NULL) || (net == NULL) || (zone == NULL))
	return zn;
    zn = chunk_malloc(zipnet_chunk);
    if (zn == NULL)
	return zn;
    znCreated++;
    zn->zone = zone;
    zn->net = net;

    /*
     * First zone is special so usually skip it
     */
    nzn = net->zonelist;		
    if (nzn != NULL) {
	zn->prevZone = nzn;
	zn->nextZone = nzn->nextZone;
	nzn->nextZone = zn;
	if (zn->nextZone != NULL)
	    (zn->nextZone)->prevZone = zn;
    } else {
	net->zonelist = zn;
	zn->prevZone = NULL;
	zn->nextZone = NULL;
    }
    zn->nextNet = zone->netlist;
    zn->prevNet = NULL;
    if (zn->nextNet != NULL)
	(zn->nextNet)->prevNet = zn;
    zone->netlist = zn;
    return zn;
}

/*
 * atalk_free_zipnet - Unlink and free a zipnet entry
 */
static void
atalk_free_zipnet (zipnetentry* zn)
{
    zipnetentry* nzn = NULL;
    zipnetentry* pzn = NULL;

    if (zn == NULL)
	return;
    nzn = zn->nextZone;
    pzn = zn->prevZone;
    if (pzn == NULL) {
	(zn->net)->zonelist = nzn;
    } else {
	pzn->nextZone = nzn;
    }
    if (nzn != NULL)
	nzn->prevZone = pzn;
    nzn = zn->nextNet;
    pzn = zn->prevNet;
    if (pzn == NULL) {
	if (zn->zone) {
	    (zn->zone)->netlist = nzn;
	    if (   (nzn == NULL)
		&& ((zn->zone)->ifcount == 0)
		&& ((zn->zone)->acccount == 0))
		atzip_NeedsGC();
	}
    } else {
	pzn->nextNet = nzn;
    }
    if (nzn != NULL)
	nzn->prevNet = pzn;
    chunk_free(zipnet_chunk, zn);
}

/* 
 * atalk_clear_zones
 * Delete any zone links associated with this net 
 */
void
atalk_clear_zones (atalk_rdb_t* net)
{
    if (net == NULL)
	return;
    if (net->zonelist != NULL)
	util_DeleteZipnetChain(net->zonelist,zipnet_viaNetChain);
    net->zoneupdate = FALSE;
    net->zoneaddr = 0;
    net->zonecnt = 0;
    atzip_NeedsGC();
}

/*
 * The old FDDI Apple broadcast is simply the bitswap of the token-ring
 * broadcast address. The new (FDDItalk) FDDI broadcast looks like the
 * EtherTalk bcast address, but will be bitswapped before getting out on
 * the wire.
 */
uchar apple_tr_broadcast[IEEEBYTES] =
    {0xC0, 0x00, 0x40, 0x00, 0x00, 0x00};
uchar apple_old_fddi_broadcast[IEEEBYTES] =
    {0x03, 0x00, 0x02, 0x00, 0x00, 0x00};
static const uchar apple_fddi_broadcast[IEEEBYTES] =
    {0x09, 0x00, 0x07, 0xff, 0xff, 0xff};
static const uchar apple_fddi_multicast[IEEEBYTES] =
    {0x09, 0x00, 0x07, 0x00, 0x00, 0x00};
static const uchar apple_tr_multicast[MAX_TR_APPLE_MC][IEEEBYTES] = {
    {0xC0, 0x00, 0x00, 0x00, 0x08, 0x00},
    {0xC0, 0x00, 0x00, 0x00, 0x10, 0x00},
    {0xC0, 0x00, 0x00, 0x00, 0x20, 0x00},
    {0xC0, 0x00, 0x00, 0x00, 0x40, 0x00},
    {0xC0, 0x00, 0x00, 0x00, 0x80, 0x00},
    {0xC0, 0x00, 0x00, 0x01, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x02, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x04, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x08, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x10, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x20, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x40, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x80, 0x00, 0x00},
    {0xC0, 0x00, 0x01, 0x00, 0x00, 0x00},
    {0xC0, 0x00, 0x02, 0x00, 0x00, 0x00},
    {0xC0, 0x00, 0x04, 0x00, 0x00, 0x00},
    {0xC0, 0x00, 0x08, 0x00, 0x00, 0x00},
    {0xC0, 0x00, 0x10, 0x00, 0x00, 0x00},
    {0xC0, 0x00, 0x20, 0x00, 0x00, 0x00}};
const uchar apple_ether_multicast[IEEEBYTES] = {0x09, 0x00, 0x07, 
                                                0x00, 0x00, 0x00};
static const uchar apple_ether_broadcast[IEEEBYTES] = {0x09, 0x00, 0x07, 
                                                       0xff, 0xff, 0xff};

ulong
atalk_tr_broadcasts (atalkidbtype* idb)
{
    uchar* p;
    ulong func;
    p = atalk_broadcast(idb);
    p += 2;
    func = GETLONG(p);                 /* TR only wants the functionals */
    p = idb->atalk_zone_multicast;
    p += 2;
    func |= GETLONG(p);                /* TR only wants the functionals */
    return func;
}

uchar*
atalk_broadcast (atalkidbtype* swidb)
{
    hwidbtype *idb = swidb->phys_idb->hwptr;
    static uchar tempMC[IEEEBYTES];
    if (!atalkif_net_extended(swidb))
	return (uchar*) baddr;
    if (idb_is_ether_like(swidb->phys_idb) || is_bvi(idb) ||
	(is_ppp(idb) && idb->ppp_bridge_atalk) ||
	(is_cpp(idb))) {
	ieee_copy(apple_ether_broadcast,tempMC);
    } else if (idb_is_fddi(swidb->phys_idb)) {
	if (idb->atalk_compat_flags == AT_COMPAT_FDDI)
	    ieee_copy(apple_old_fddi_broadcast,tempMC);
	else
	    ieee_copy(apple_fddi_broadcast,tempMC);
    } else {
	ieee_copy(apple_tr_broadcast,tempMC);
    }
    return (uchar*) tempMC;
}

uchar*
atalk_zone_multicast (atalkidbtype* idb,uint zonesum)
{
    static uchar tempMC[IEEEBYTES];
    idbtype *phys_idb = idb->phys_idb;
    
    if (!atalkif_net_extended(idb))
	return (uchar*) baddr;
    if (idb_is_ether_like(phys_idb) || is_bvi(phys_idb->hwptr) ||
	(is_ppp(phys_idb->hwptr) && phys_idb->hwptr->ppp_bridge_atalk) ||
	(is_cpp(phys_idb->hwptr))) {
	ieee_copy(apple_ether_multicast,tempMC);
	tempMC[IEEEBYTES -1] = zonesum % MAX_ETHER_APPLE_MC;
    } else if (idb_is_fddi(phys_idb)) {
	if (idb->phys_idb->hwptr->atalk_compat_flags == AT_COMPAT_FDDI)
	    ieee_copy(apple_old_fddi_broadcast,tempMC);
	else {
	    ieee_copy(apple_fddi_multicast,tempMC);
	    tempMC[IEEEBYTES -1] = zonesum % MAX_ETHER_APPLE_MC;
	}
    }
    else
	ieee_copy(apple_tr_multicast[zonesum % MAX_TR_APPLE_MC],tempMC);
    return (uchar*) tempMC;
}

ushort
rotate_short (ushort val)
{
    ulong temp = val << 1;
    return ((temp & 0xffff)|(temp >> 16));
}

/*
 * atalk_zonesum
 * compute the sum for zone name (case-insensitive) by summing characters
 */
uint 
atalk_zonesum (uchar *name, int len)
{
    ushort sum = 0;
    
    while (len-- > 0) {
	sum += atalk_upper(*name++);
	sum = rotate_short(sum);
    }
    return sum;
}

/*
 * atalk_random
 * return an int in the range of (0 <= n < range)
 */
int
atalk_random (int range)
{
    if (range <= 0)
	return (0);
    return (random_gen() % range);
}

/*
 * atalkif_static_addresses
 * return TRUE if AT address is statically assigned (eg serial)
 */
boolean
atalkif_static_addresses (atalkidbtype* idb)
{
    if (atalkif_HalfRouter(idb))
	return (TRUE);
    if (idb->aarp_enctype == ET_NULL)
	return (TRUE);
    return (FALSE);
}

/*
 * Safe List Support
 */

/*
 * SListSearch
 * Lookup a key in a list, starting at first
 */
listItem*
SListSearch (ulong key, listItem* first)
{
    listItem* item;
    for (item=first;item != NULL;item = item->next) {
	if (item->deleted)
	    continue;
	if (item->key == key)
	    return item;
    }
    return NULL;
}

/*
 * ProtectListItem - Prevent item from being freed when deleted.
 */
void
ProtectListItem (listItem* n)
{
    n->busy++;			/* Mark node in use */
}

/*
 * UnprotectListItem - Allow item to be freed if deleted.
 */
boolean
UnprotectListItem (listItem* n)
{
    if ((n->busy--) <= 0)
	return (TRUE);		/* Indicate node no longer in use */
    return (FALSE);
}

/*
 * GetNextItem
 * Step down our item list skipping deleted nodes.
 */
listItem*
GetNextItem (listItem* c)
{
    if (c == NULL)
	return NULL;
    for (c=c->next;c!=NULL;c=c->next) {
	if (c->deleted)
	    continue;
	return c;
    }
    return NULL;
}

/*
 * UnlinkListItem
 * Unlink an item from a list, even if the item is busy.
 */
void
UnlinkListItem (listItem* i,listItem** r)
{
    listItem* x = *r;
    i->key = 0;
    if (x == NULL)
	return;
    if (x == i) {
	*r = i->next;
	i->next = NULL;
	return;
    }
    while (x->next != i) {
	x = x->next;
	if (x == NULL) break;
    }
    if (x != NULL) {
	x->next = i->next;
	i->next = NULL;
	return;
    }
}

void*
ListItemData (listItem* item)
{
    return item->data;
}

void
SetListItemData (listItem* item,void* data)
{
    item->data = data;
}

void
InitListItem (listItem* item)
{
    item->next = NULL;
    item->deleted = FALSE;
    item->busy = 0;
    item->key = 0;
    item->data = NULL;
}

void
InsertListItem (ulong key,listItem* item,listItem** root)
{
    item->next = *root;
    item->deleted = FALSE;
    item->key = key;
    *root = item;
}

char *
atalk_statStr (int nn)
/* return name of appletalk stat */
{
    switch(nn) {
    case ATALK_INPUT:			return("ATALK_INPUT");
    case ATALK_LOCALDEST:		return("ATALK_LOCALDEST");
    case ATALK_NO_HANDLER:		return("ATALK_NO_HANDLER");
    case ATALK_MARTIAN:			return("ATALK_MARTIAN");
    case ATALK_IGNORED:			return("ATALK_IGNORED");
    case ATALK_BCASTIN:			return("ATALK_BCASTIN");
    case ATALK_FORWARDED:		return("ATALK_FORWARDED");
    case ATALK_BCASTOUT:		return("ATALK_BCASTOUT");
    case ATALK_CKSUMERR:		return("ATALK_CKSUMERR");
    case ATALK_HOPCOUNT:		return("ATALK_HOPCOUNT");
    case ATALK_NOACCESS:		return("ATALK_NOACCESS");
    case ATALK_NOROUTE:			return("ATALK_NOROUTE");
    case ATALK_ENCAPSFAILED:		return("ATALK_ENCAPSFAILED");
    case ATALK_NOSOURCE:		return("ATALK_NOSOURCE");
    case ATALK_OUTPUT:			return("ATALK_OUTPUT");
    case ATALK_LOOPBACK:		return("ATALK_LOOPBACK");
    case ATALK_INMULT:			return("ATALK_INMULT");
    case ATALK_RTMP_IN:			return("ATALK_RTMP_IN");
    case ATALK_RTMP_OUT:		return("ATALK_RTMP_OUT");
    case ATALK_RTMP_REQUEST:		return("ATALK_RTMP_REQUEST");
    case ATALK_RTMP_BAD:		return("ATALK_RTMP_BAD");
    case ATALK_IGRP_IN:			return("ATALK_IGRP_IN");
    case ATALK_IGRP_OUT:		return("ATALK_IGRP_OUT");
    case ATALK_IGRP_OUT_FAILED:		return("ATALK_IGRP_OUT_FAILED");
    case ATALK_RTMP_IGNORED:		return("ATALK_RTMP_IGNORED");
    case ATALK_IGRP_BAD:		return("ATALK_IGRP_BAD");
    case ATALK_IGRP_IGNORED:		return("ATALK_IGRP_IGNORED");
    case ATALK_IGRP_HELLO_IN:		return("ATALK_IGRP_HELLO_IN");
    case ATALK_IGRP_REQUEST_IN:		return("ATALK_IGRP_REQUEST_IN");
    case ATALK_IGRP_UPDATE_IN:		return("ATALK_IGRP_UPDATE_IN");
    case ATALK_IGRP_QUERY_IN:		return("ATALK_IGRP_QUERY_IN");
    case ATALK_IGRP_REPLY_IN:		return("ATALK_IGRP_REPLY_IN");
    case ATALK_IGRP_HELLO_OUT:		return("ATALK_IGRP_HELLO_OUT");
    case ATALK_IGRP_REQUEST_OUT:	return("ATALK_IGRP_REQUEST_OUT");
    case ATALK_IGRP_UPDATE_OUT:		return("ATALK_IGRP_UPDATE_OUT");
    case ATALK_IGRP_QUERY_OUT:		return("ATALK_IGRP_QUERY_OUT");
    case ATALK_IGRP_REPLY_OUT:		return("ATALK_IGRP_REPLY_OUT");
    case ATALK_NBP_IN:			return("ATALK_NBP_IN");
    case ATALK_NBP_OUT:			return("ATALK_NBP_OUT");
    case ATALK_NBP_FAILED:		return("ATALK_NBP_FAILED");
    case ATALK_NBP_LKUP:		return("ATALK_NBP_LKUP");
    case ATALK_NBP_FWD:			return("ATALK_NBP_FWD");
    case ATALK_NBP_PROXY:		return("ATALK_NBP_PROXY");
    case ATALK_NBP_BAD:			return("ATALK_NBP_BAD");
    case ATALK_SNMP_IN:			return("ATALK_SNMP_IN");
    case ATALK_SNMP_OUT:		return("ATALK_SNMP_OUT");
    case ATALK_ATP:			return("ATALK_ATP");
    case ATALK_ZIP_IN:			return("ATALK_ZIP_IN");
    case ATALK_ZIP_OUT:			return("ATALK_ZIP_OUT");
    case ATALK_ZIP_NETINFO:		return("ATALK_ZIP_NETINFO");
    case ATALK_ECHO:			return("ATALK_ECHO");
    case ATALK_ECHO_SENT:		return("ATALK_ECHO_SENT");
    case ATALK_ECHO_FAILED:		return("ATALK_ECHO_FAILED");
    case ATALK_ECHO_REPLIES:		return("ATALK_ECHO_REPLIES");
    case ATALK_ECHO_DROPPED:		return("ATALK_ECHO_DROPPED");
    case ATALK_ECHO_ILL:		return("ATALK_ECHO_ILL");
    case ATALK_DDP_SHORT:		return("ATALK_DDP_SHORT");
    case ATALK_DDP_LONG:		return("ATALK_DDP_LONG");
    case ATALK_NO_BUFFERS:		return("ATALK_NO_BUFFERS");
    case ATALK_DDP_WRONGSIZE:		return("ATALK_DDP_WRONGSIZE");
    case ATALK_ARP_REQUEST:		return("ATALK_ARP_REQUEST");
    case ATALK_ARP_REPLY:		return("ATALK_ARP_REPLY");
    case ATALK_ARP_PROBE:		return("ATALK_ARP_PROBE");
    case ATALK_ARP_UNKNOWN:		return("ATALK_ARP_UNKNOWN");
    case ATALK_ARP_BOGON:		return("ATALK_ARP_BOGON");
    case ATALK_ARP_BADENCS:		return("ATALK_ARP_BADENCS");
    case ATALK_ARP_DELAYS:		return("ATALK_ARP_DELAYS");
    case ATALK_ARP_DROPS:		return("ATALK_ARP_DROPS");
    case ATALK_ARP_FAILED:		return("ATALK_ARP_FAILED");
    case ATALK_ARP_SENT:		return("ATALK_ARP_SENT");
    case ATALK_BCAST_NOTMINE:		return("ATALK_BCAST_NOTMINE");
    case ATALK_SADDR_INVALID:		return("ATALK_SADDR_INVALID");
    case ATALK_USED_DEFGW:		return("ATALK_USED_DEFGW");
    case ATALK_DADDR_INVALID:		return("ATALK_DADDR_INVALID");
    case ATALK_DADDR_IGNORED:		return("ATALK_DADDR_IGNORED");
    case ATALK_ROUTER_BADVERS:		return("ATALK_ROUTER_BADVERS");
    case ATALK_BAD_ISO2OUI:		return("ATALK_BAD_ISO2OUI");
    case ATALK_UNKNOWN:			return("ATALK_UNKNOWN");
    case ATALK_FASTFORWARD:		return("ATALK_FASTFORWARD");
    case ATALK_XFASTFORWARD:		return("ATALK_XFASTFORWARD");
    case ATALK_NOT_OPERATIONAL:		return("ATALK_NOT_OPERATIONAL");
    case ATALK_DEBUGMSGS:		return("ATALK_DEBUGMSGS");
    case ATALK_BUGS:			return("ATALK_BUGS");
    case AT_RESPONDER_IN:		return("AT_RESPONDER_IN");
    case AT_RESPONDER_BAD_ATP:		return("AT_RESPONDER_BAD_ATP");
    case AT_RESPONDER_BAD_OPCODE:	return("AT_RESPONDER_BAD_OPCODE");
    case AT_RESPONDER_CREATE_ERROR:	return("AT_RESPONDER_CREATE_ERROR");
    case AT_RESPONDER_OUT:		return("AT_RESPONDER_OUT");
    case AT_RESPONDER_SEND_FAIL:	return("AT_RESPONDER_SEND_FAIL");
    case MACIP_DDP_IN:			return("MACIP_DDP_IN");
    case MACIP_DDP_IP_OUT:		return("MACIP_DDP_IP_OUT");
    case MACIP_DDP_IP_FORWARD_FAIL:	return("MACIP_DDP_IP_FORWARD_FAIL");
    case MACIP_DDP_BAD_DDP_TYPE:	return("MACIP_DDP_BAD_DDP_TYPE");
    case MACIP_DDP_NO_CLIENT_SERVICE:	return("MACIP_DDP_NO_CLIENT_SERVICE");
    case MACIP_DDP_CLIENT_CREATE_ERROR:	return("MACIP_DDP_CLIENT_CREATE_ERROR");
    case MACIP_IP_IN:			return("MACIP_IP_IN");
    case MACIP_IP_DDP_OUT:		return("MACIP_IP_DDP_OUT");
    case MACIP_IP_DDP_CREATE_ERROR:	return("MACIP_IP_DDP_CREATE_ERROR");
    case MACIP_IP_DDP_SEND_FAIL:	return("MACIP_IP_DDP_SEND_FAIL");
    case MACIP_SERVER_IN:		return("MACIP_SERVER_IN");
    case MACIP_SERVER_OUT:		return("MACIP_SERVER_OUT");
    case MACIP_SERVER_SEND_FAIL:	return("MACIP_SERVER_SEND_FAIL");
    case MACIP_SERVER_BAD_ATP:		return("MACIP_SERVER_BAD_ATP");
    case MACIP_SERVER_ASSIGN_IN:	return("MACIP_SERVER_ASSIGN_IN");
    case MACIP_SERVER_ASSIGN_OUT:	return("MACIP_SERVER_ASSIGN_OUT");
    case MACIP_SERVER_ASSIGN_ERROR:	return("MACIP_SERVER_ASSIGN_ERROR");
    case MACIP_SERVER_INFO_IN:		return("MACIP_SERVER_INFO_IN");
    case MACIP_SERVER_INFO_OUT:		return("MACIP_SERVER_INFO_OUT");
    case MACIP_SERVER_INFO_ERROR:	return("MACIP_SERVER_INFO_ERROR");
    case MACIP_SERVER_STATIC_IN:	return("MACIP_SERVER_STATIC_IN");
    case MACIP_SERVER_STATIC_OUT:	return("MACIP_SERVER_STATIC_OUT");
    case MACIP_SERVER_STATIC_ERROR:	return("MACIP_SERVER_STATIC_ERROR");
    case MACIP_SERVER_RELEASE_IN:	return("MACIP_SERVER_RELEASE_IN");
    case MACIP_SERVER_RELEASE_OUT:	return("MACIP_SERVER_RELEASE_OUT");
    case MACIP_SERVER_RELEASE_ERROR:	return("MACIP_SERVER_RELEASE_ERROR");
    case MACIP_SERVER_BAD_OPCODE:	return("MACIP_SERVER_BAD_OPCODE");
    case ATALK_MAXTRAFFIC:		return("ATALK_MAXTRAFFIC");
    default:				return("unknown atalk stat!");
    }
}

/*
 * atalk_on_idb
 *
 * Return TRUE if Appletalk is enabled on the given interface
 */
boolean
atalk_on_idb (idbtype *swidb)
{
    atalkidbtype *idb = atalk_getidb(swidb);
    
    if (atalk_running && is_async(swidb->hwptr)) {
	if (idb && idb->atalk_allow_clients)
		return(TRUE);
	   else
		return(FALSE);
    }

    if ((!atalk_running) || !idb || idb->atalk_enabled == FALSE)
	return (FALSE);
    else
	return (TRUE);
}

/*
 * atalk_idb_hwaddr
 *
 * get the hw address from an interface depending on the encap/mode currently
 * in use on the idb.
 *
 * Returns: TRUE if a valid address could be found, FALSE if not.
 */
boolean
atalk_idb_hwaddr (idbtype *swidb, uchar *hwaddr, ushort *hwaddr_len)
{
    smds_pdb_t		*spd;
    fr_idb_struct_type 	*frpd;
    hwidbtype *idb = swidb->hwptr;

    *hwaddr_len = 0;

    if (idb_is_ieeelike(swidb) || is_bvi(swidb->hwptr)) {
	ieee_copy(idb->hardware, hwaddr);
	*hwaddr_len = IEEEBYTES;
	return (TRUE);
    } else if (idb->status & IDB_SERIAL) {
	if (idb->enctype == ET_SMDS && idb->smds_pdb) {
	    spd = (smds_pdb_t *)idb->smds_pdb;
	    bcopy(spd->smds_hardware, hwaddr, sizeof(spd->smds_hardware));
	    *hwaddr_len = sizeof(spd->smds_hardware);
	    return (TRUE);
	} else if (idb->enctype == ET_FRAME_RELAY && idb->frame_relay_stuff) {
	    frpd = idb->frame_relay_stuff;
	    bcopy(&frpd->fr_local_dlci, hwaddr, sizeof(frpd->fr_local_dlci));
	    *hwaddr_len = sizeof(frpd->fr_local_dlci);
	    return (TRUE);
	} else if (is_x25(idb) && idb->x25_address) {
	    bcopy(idb->x25_address->x121_addr, hwaddr,
		  idb->x25_address->length);
	    *hwaddr_len = idb->x25_address->length;
	    return (TRUE);
	} else if ((is_ppp(idb) && idb->ppp_bridge_atalk) ||
		   (is_cpp(idb))) {
	    ieee_copy(default_mac_addr, hwaddr);
	    *hwaddr_len = IEEEBYTES;
	    return(TRUE);
	} else {
	    return (FALSE);
	}
    }
    return (FALSE);		/* ??? what other kinds of idb's are there? */
}

void
atalkif_SetAarpTimeout (atalkidbtype *idb, ulong timeout)
{
    if (idb) {
	idb->atalk_aarp_timeout = timeout;
    }
}
	    
ulong atalkif_ArpTimeout (atalkidbtype *idb)
{
    return (idb->atalk_aarp_timeout);
}

/*
 * Get first entry in a tree and protect it.
 */
treeLink*
atutil_KeepFirstEntry (rbTree *tree)
{
    return ((tree != NULL) ? atutil_KeepNextEntry(tree, NULL) : NULL);
}

/*
 * atutil_KeepNextEntry - Search for the next entry and protect it
 */
treeLink*
atutil_KeepNextEntry (rbTree *tree, treeLink* prev)
{
    treeLink *e = prev;

    if (e == NULL) {
	if (tree == NULL)
	    return (NULL);
	e = RBTreeFirstNode(tree);
    } else {
	RBTreeNodeProtect(prev, FALSE); /* unprotect previous entry */
	e = RBTreeNextNode(prev);
    }
    if (e != NULL)
	RBTreeNodeProtect(e, TRUE);

    return (e);
}

/*
 * atutil_ReleaseEntry
 * Terminate GetNext processing and release the entry.
 */
void
atutil_ReleaseEntry (treeLink* e)
{
    if (e != NULL)
	RBTreeNodeProtect(e, FALSE);
}

/*
 * Get first entry in a tree and DO NOT protect it.
 */
treeLink*
atutil_GetFirstEntry (rbTree *tree)
{
    return ((tree != NULL) ? atutil_GetNextEntry(tree, NULL) : NULL);
}

/*
 * atutil_GetNextEntry - Search for the next entry and DO NOT protect it
 */
treeLink*
atutil_GetNextEntry (rbTree *tree, treeLink* prev)
{
    treeLink *e = prev;

    if (e == NULL) {
	if (tree == NULL)
	    return (NULL);
	e = RBTreeFirstNode(tree);
    } else {
	e = RBTreeNextNode(prev);
    }
    return (e);
}

void
atutil_ProtectEntry (treeLink *e)
{
    if (e)
	RBTreeNodeProtect(e, TRUE);
}
void
atutil_UnprotectEntry (treeLink *e)
{
    if (e)
	RBTreeNodeProtect(e, FALSE);
}

char const *
atutil_statusString (opStatus status)
{
    static char const *statusStr[opValid+1] =
	{
	    "opUnknown", "opNoAddress", "opProbing", "opCableMismatch",
	    "opConfigBad", "opLineProtoErr", "opAcquire", "opAcquirePeer",
	    "opAcquireZones", "opVerify", "opVerifyZones", "opValidate",
	    "opNetConflict", "opNetDown", "opResExhausted", "opHRSupportErr",
	    "opRemotePeerErr", "opInternalError", "opInternalBug",
	    "opCompatBad", "opRestartPort", "opValid"
	};

    if (status > opValid)
	return (NULL);

    return (statusStr[status]);
}

/*
 * atalk_validSrcAddress
 * This is a special variant of atalkif_ValidAddress in that it checks the
 * _source_ network type, whereas atalkif_ValidAddress checks the 
 * destination network type.
 *
 */
boolean
atalk_validSrcAddress (atalkidbtype* idb, ataddrtype addr)
{
    uchar node;
    ushort net;

    ATASSERT(idb != NULL);
    if (!(idb->atalk_enabled && atalk_running))
	return (FALSE);
    if (atalkif_HalfRouter(idb))	/* This is an "unnumberd" interface */
	return (TRUE);
    node = atalk_nodeonly(addr);
    switch (node) {
      case ATALK_ANY_ROUTER:
      case ATALK_BROAD_ADDR:
	return (FALSE);
      case ATALK_ETHER_BADADDR:
	if (atalkif_net_extended(idb))
	    return (FALSE);
    }

    net = atalk_netonly(addr);
    if (net == ATALK_NET_UNNUMBERED) {
	if (atalkif_HalfRouter(idb))
	    return (TRUE);		/* this is good */
	else
	    return (FALSE);		/* shouldn't ever happen */
    }
    if (net > ATALK_STARTUP_END)
	return (FALSE);
    if ((net >= ATALK_STARTUP_START) && (net <= ATALK_STARTUP_END))
	return (FALSE);			/* RTMP's can't come from startup */
    return (TRUE);
}


/*
 * at_printf_littlea
 * a prints a dotted AppleTalk address.  Takes an integer argumet.
 *   a - Prints the preferred form for the AppleTalk address.
 *	l - indicates that the address is fully qualified (includes socket).
 *      # - indicates that the textual node name should be used if known.
 */

int at_printf_littlea (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
		       int rjust, int width, int size, int hash, int precision)
{
    ulong n;
    int length, d, charcount;
    ushort a1,a2,a3=0,a4=0;
    char* f;

    charcount = 0;
    n = va_arg(*argp,ipaddrtype);
    if (size)
        n = (n >> 8);
    n = (n & 0xffffff);
    a1 = (n >> 8) & 0xffff;
    a2 = (n & 0xff);
    a3 = (n >> 16) & 0xff;
    a4 = (n >> 8) & 0xff;
    if (hash)		/* # flag = Use node name if known */
	f = atalk_LookupNodeName(n);
    else
	f = NULL;
    if (*cp) {
	if (f != NULL)
	    length = sprintf(*cp,"%s",f);
	else if (atalk_alternateAddrs)
	    length = sprintf(*cp,"%d.%d.%d",a3,a4,a2);
	else
	    length = sprintf(*cp,"%d.%d",a1,a2);
	*cp += length;
    } else {
	if (f != NULL)
	    length = _ttyprintf(tty,"%s",f);
	else if (atalk_alternateAddrs)
	    length = _ttyprintf(tty,"%d.%d.%d",a3,a4,a2);
	else
	    length = _ttyprintf(tty,"%d.%d",a1,a2);
    }
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

/*
 * at_printf_biga
 * A prints a dotted AppleTalk address.  Takes an integer argumet.
 *   A - Prints both the standard and alternate forms if appropriate.
 *	l - indicates that the address is fully qualified (includes socket).
 *	# flag indicates the value is a network range or number.
 */

int at_printf_biga (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
		    int rjust, int width, int size, int hash, int precision)
{
    int length, d, charcount;
    ushort a1,a2,a3=0,a4=0,a5=0;
    char* f;
    ulong host;

    charcount = 0;
    host = va_arg(*argp,ipaddrtype);
    if (hash) {
	if (atalk_alternateAddrs) {
	    a1 = host >> 24;
	    a2 = (host >> 16) & 0xff;
	    a3 = (host >> 8) & 0xff;
	    a4 = host & 0xff;
	    f = ((host & 0xffff) != 0) ? "%d.%-03d-%d.%-03d" : "%d.%-03d";
	} else {
	    a1 = (host >> 16) & 0xffff;
	    a2 = (host & 0xffff);
	    f = (a2 != 0) ? "%d-%d" : "%d";
	}
    } else {
	if (atalk_alternateAddrs) {
	    if (size) {
		f = "%d.%-03d.%d-%d";
		a1 = (host >> 24)&0xff;
		a2 = (host >> 16)&0xff;
		a3 = (host >>  8)&0xff;
		a4 = host & 0xff;
	    } else {
		f = "%d.%-03d.%d (%d.%d)";
		a1 = (host >> 16) & 0xff;
		a2 = ((host >> 8) & 0xff);
		a3 = host & 0xff;
		a4 = (host >> 8) & 0xffff;
		a5 = host & 0xff;
	    }
	} else {
	    if (size) {
		f = "%d.%d-%d";
		a1 = (host >> 16) & 0xffff;
		a2 = (host >> 8) & 0xff;
		a3 = (host & 0xff);
	    } else {
		f = "%d.%d";
		a1 = host >> 8;
		a2 = host & 0xff;
	    }
	}
    }
    if (*cp) {
	length = sprintf(*cp,f,a1,a2,a3,a4,a5);
	*cp += length;
    } else {
	length = _ttyprintf(tty,f,a1,a2,a3,a4,a5);
    }
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}


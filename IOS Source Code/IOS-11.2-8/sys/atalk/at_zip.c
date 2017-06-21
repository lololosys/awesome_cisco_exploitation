/* $Id: at_zip.c,v 3.4.44.4 1996/07/10 22:19:12 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_zip.c,v $
 *------------------------------------------------------------------
 * at_zip.c -- Appletalk ZIP protocol support
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_zip.c,v $
 * Revision 3.4.44.4  1996/07/10  22:19:12  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/06/28  22:30:31  kleung
 * CSCdi61658:  Router crash in idb_get_swsb() called from
 * atroute_FirstHop().
 * Branch: California_branch
 *
 * Revision 3.4.44.2  1996/03/23  01:27:59  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4.44.1  1996/03/18  18:53:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  08:29:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  13:34:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/07  22:27:25  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  08:44:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:58:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:50:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/07/24  22:30:39  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.4  1995/07/13 06:50:16  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.3  1995/07/05 22:25:01  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.2  1995/06/18  06:17:49  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 20:09:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <interface.h>
#include "packet.h"
#include "ieee.h"
#include "string.h"
#include "../os/signal.h"
#include "atalk_private.h"
#include "at_domain.h"
#include "at_registry.h"

/* local storage */
/* static */ ziptabletype *zip_table[ZIPHASHLEN]; /* zone name table */
static ulong zip_zones;
static ulong zip_gcNeeded;
static ulong zip_created;
static ulong zip_freed;
static ulong zip_gcRuns;
static ulong zip_hashCollisions;

static watched_queue *atalkzipQ;

static boolean zip_ValidatePrimary(atalkidbtype*,uchar*);
static void zip_MergeReply(paktype*);
static void zip_MergeXReply(paktype*);
static void send_query_xreply (paktype* ,atalk_rdb_t* ,neighborentry*);

static void zip_GZL(paktype*);
static boolean atzip_GC(void);
static gccontext_t *zip_gccontext;


static void
zip_Shutdown (void)
{
    atzip_GC();
}

/* 
 * atzip_Initialize
 * initialize data structures used by ZIP
 */
void
atzip_Initialize (void)
{
    int i;
    
    for (i=0;i < ZIPHASHLEN;i++)
	zip_table[i] = NULL;
    zip_zones = 0;		/* no zones yet */
    zip_gcNeeded = 0;
    zip_created = 0;
    zip_freed = 0;
    zip_gcRuns = 0;
    zip_hashCollisions = 0;
    
    /* Set up context for ZIP GC */
    zip_gccontext = atmaint_AddGCContext(atzip_GC, 15);

    reg_add_atalk_shutdown(zip_Shutdown, "zip_Shutdown");
}

/*
 * atzip_StatusReport
 * Generate a status report about ZIP.
 */
void
atzip_StatusReport (void)
{
    printf("\n  ZIP Statistics: %d created, %d freed, %d GC runs, %d collisions",
	   zip_created,zip_freed,zip_gcRuns,zip_hashCollisions);
    if (zip_gcNeeded)
	printf("\n  ZIP needs a garbage collection run.");
}

/*
 * atzip_ZoneCount
 * Return the number of zones.
 */
ulong
atzip_ZoneCount (void)
{
    return (zip_zones);
}

/*
 * zip_FinishMerge
 * Finish the merge process for a route.
 */
void
zip_FinishMerge (atalk_rdb_t* r, sys_timestamp now)
{
    if (r->zoneupdate && TIMERS_EQUAL(r->zonetimer, now)) {
	r->zoneupdate = FALSE;
	r->zoneretries = 0;
	if (atalk_net_connected(r) && (atalk_firstZone(r) != 0))
	    atalkif_RouteZonesReady(r->rpath.idb); /* ??? */
    }
}

static void
atalk_zip_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_queue(atalkzipQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalkzipQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalkzipQ);
    atalk_zip_pid = 0;
}

/*
 * atalk_zip
 * Appletalk Zone Information Protocol processing
 */
process
atalk_zip (void)
{
    paktype *pak;
    atptype *atp;
    neighborentry* neighbor;
    ziptype *zip;

    process_wait_on_system_init();
    
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, atalk_zip_teardown);
    atalkzipQ = create_watched_queue("AT ZIP packets", 0, 0);
    process_watch_queue(atalkzipQ, ENABLE, RECURRING);
    
    atalk_BindSocket(atalkzipQ,"ZIP",ATALK_SOCK_ZIP, NULL);
    while (TRUE) {
	process_wait_for_event();
	
	/* while packets... */
	while ((pak = process_dequeue(atalkzipQ)) != NULL) {
	    atalkidbtype *idb;
	    
	    /* dequeue then checksum, but no DDP type check */
	    if (!pak || !atalk_check(pak, DDP_TYPE_Invalid, ATALK_UNKNOWN, 
					FALSE))
		continue;	/* noone home, checksum bad or interf down */

	    idb = atalk_getidb(pak->if_input);
	    
	    if (atalk_pak_getpt(pak) == DDP_TYPE_ZIP) { /* handle ZIP stuff */
		atalk_stat[ATALK_ZIP_IN]++;
		zip = (ziptype *)pak->transport_start;
		if (!idb->atalk_operational && 
		    ((zip->cmd != ZIP_GetNetInfoReply) &&
		     (zip->cmd != ZIP_GetNetInfo) &&
		     (zip->cmd != ZIP_Reply) &&
		     (zip->cmd != ZIP_ExtendedReply))) {
		    atalk_dispose(ATALK_NOT_OPERATIONAL,pak);
		    continue;
		}
		if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		    buginf("\nAT: Recvd ZIP cmd %d from %a-%d",zip->cmd,
			   atalk_pak_srcaddr(pak),
			   atalk_pak_ssock(pak));
		neighbor = atmaint_GetNeighbor(atalk_pak_srcaddr(pak),
					       idb,
					       FALSE);
		switch (zip->cmd) {
		case ZIP_Query:
		    if (neighbor)
			neighbor->zipqueries++;
		    zip_ans_query(pak);	/* send off answer */
		    break;
		case ZIP_Reply:
		    if (neighbor)
			neighbor->zipreplies++;
		    zip_MergeReply(pak); /* merge data into tables */
		    break;
		case ZIP_GetNetInfo:
		    if (neighbor)
			neighbor->zipother++;
		    zip_ans_getnetinfo(pak);
		    break;
		case ZIP_GetNetInfoReply:
		    if (neighbor)
			neighbor->zipother++;
		    zip_validate_netinfo(pak);
		    break;
		case ZIP_Notify:
		    if (neighbor)
			neighbor->zipnotifies++;
		    zip_notify(pak);
		    break;
		case ZIP_ExtendedReply:
		    if (neighbor)
			neighbor->zipxreplies++;
		    zip_MergeXReply(pak);
		    break;
		case ZIP_Takedown:
		    if (neighbor)
			neighbor->zipobsolete++;
		    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
			|| at_debug(atalkzip_debug, ATALK_NULL_MASK))
			buginf("\nAT: ZIP Takedown rcvd from %a, ignored",
			   atalk_pak_srcaddr(pak));
		    break;
		case ZIP_Bringup:
		    if (neighbor)
			neighbor->zipobsolete++;
		    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
			|| at_debug(atalkzip_debug, ATALK_NULL_MASK))
			buginf("\nAT: ZIP Bringup rcvd from %a, ignored",
			       atalk_pak_srcaddr(pak));
		    break;
		default:
		    if (neighbor)
			neighbor->zipunknown++;
		    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
			|| at_debug(atalkzip_debug, ATALK_NULL_MASK))
			buginf("\nAT: ZIP unknown function (%d) from %a, ignored",
		       zip->cmd, atalk_pak_srcaddr(pak));
		    break;
		}
	    } else {		/* handle ZIP/ATP stuff */
		atp = (atptype *) pak->transport_start;
		if (atp->ccf == GZLReq1)
		    zip_GZL(pak);
	    }
	    datagram_done(pak);
	}
    }
}

/*
 * zip_notify
 * Handle the zip notify command.
 */
void
zip_notify (paktype* pak)
{
    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
	|| at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: ZIP Notify rcvd from %a, ignored",
	       atalk_pak_srcaddr(pak));
}

/*
 * atzip_SendGetNetInfo
 * Send a GetNetInfo request to the specified interface.
 */
boolean
atzip_SendGetNetInfo (atalkidbtype* idb)
{
    paktype* pak;
    int nbytes;
    zipnetinfo* zip;
    ziptabletype* zone;
    ushort dnet;
    uchar dnode;
    if (idb->atalk_confaddr == 0) {
	dnet = ATALK_CABLE_ALLNETS;
	dnode = ATALK_BROAD_ADDR;
    } else {
	dnet = atalk_netonly(idb->atalk_confaddr);
	dnode = atalk_nodeonly(idb->atalk_confaddr);
    }
    pak = atalk_pak_create(idb,atalk_address(dnet,dnode),ATALK_SOCK_ZIP,
			    ATALK_SOCK_ZIP,DDP_TYPE_ZIP,ZIP_MAX_GETNETINFO);
    if (pak == NULL)
	return (FALSE);

    pak->if_output = idb->phys_idb;
    zip = (zipnetinfo*) pak->transport_start;
    nbytes = pak->atalk_datalen;
    zip->cmd = ZIP_GetNetInfo;
    zip->flags = 0;
    PUTSHORT(zip->netstart,0);
    PUTSHORT(zip->netend,0);
    if (atalkif_opstatus(idb) == opValidate)
	zone = NULL;
    else
	zone = atalkif_primary_zone(idb);
    if (zone != NULL) {
	zip->zonelen = zone->length;
	bcopy(zone->name,zip->zonename,zone->length);
    } else
	zip->zonelen = 0;
    nbytes -= (1 + 1 + DDPNETBYTES + DDPNETBYTES + 1 + zip->zonelen);
    atalk_pak_setlength(pak,ZIP_MAX_GETNETINFO - nbytes);
    atalk_DDPcksum(pak);
    if (atalk_send(pak)) {
	atalk_stat[ATALK_ZIP_OUT]++;
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    if (idb->atalk_confaddr == 0)
		buginf("\nAT: Sent GetNetInfo request broadcast on %s",
		       idb_get_namestring(idb->phys_idb));
	    else
		buginf("\nAT: Sent GetNetInfo request via %s to %#a",
		       idb_get_namestring(idb->phys_idb), idb->atalk_confaddr);
	return (TRUE);
    }
    return (FALSE);
}

/*
 * zip_ans_getnetinfo
 *
 * Answer a GetNetInfo request.
 */
void
zip_ans_getnetinfo (paktype* pak)
{
    paktype* rpak;
    int nbytes;
    zipnetinfo* zip = (zipnetinfo*) pak->transport_start;
    zipnetinfo* rzip;
    zipifentry* zif;
    ziptabletype* zt;
    ziptabletype* idbzt;
    uchar* zonename;
    uchar zonelen;
    uchar* zptr;
    ataddrtype srcaddr = atalk_pak_srcaddr(pak);
    netTypes snettype;
    atalkidbtype* idb;
    boolean BadPacket;
    
    /* Validate packet. */
    BadPacket = FALSE;
    if (zip->flags != 0 || GETSHORT(zip->netstart) != 0
	|| GETSHORT(zip->netend) != 0
	|| zip->zonelen > pak->atalk_datalen - ZIP_NETINFO_HDRSIZE)
	BadPacket = TRUE;
    
    if (BadPacket == TRUE) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Bad GetNetInfo Request");
	return;
    }
    
    rpak = atalk_pak_reply(pak,DDP_TYPE_ZIP,ZIP_MAX_GETNETINFO);
    if (rpak == NULL) {
	if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
	    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: zip_ans_getnetinfo getbuffer() failure");
	return;
    }
    idb = atalk_getidb(rpak->if_input);	/* pak_reply looks up idb for dst! */

    atalk_stat[ATALK_ZIP_NETINFO]++;
    if (!atalkif_net_extended(idb)) {
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    || at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: ZIP GetNetInfo from %a discarded, %s not extended",
		   srcaddr, idb_get_namestring(idb->phys_idb));
	datagram_done(rpak);
	return;
    }
    if (!idb->atalk_operational) {
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    || at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: ZIP GetNetInfo from %a, %s %s",
		   srcaddr, idb_get_namestring(idb->phys_idb),
		   atalkif_opstatusmsg(idb));
	atalkif_RouterProbe(idb,srcaddr);
	datagram_done(rpak);
	return;
    }
    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	|| at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: Answering ZIP GetNetInfo rcvd from %a via %s",
	       srcaddr, idb_get_namestring(idb->phys_idb));
    /*
     * If this was a request sent to broadcast, then we must check the
     * validity of it's source.  If it is not valid, we must reply via
     * a cable broadcast.  This permits routed requests, while handling
     * startup cases.
     */
    if (atalk_pak_dnet(pak) == ATALK_CABLE_ALLNETS) {
	snettype = atalkif_srcnettype(idb,atalk_netonly(srcaddr));
	if (snettype == netBadForCable) {
	    atalk_pak_setbroadcast(rpak);
	    atalk_pak_setdnet(rpak,ATALK_CABLE_ALLNETS);
	    atalk_pak_setdnode(rpak,ATALK_BROAD_ADDR);
	    rpak->if_output = idb->phys_idb;
	    errmsg(AT_ERR_NODEWRONG, idb_get_namestring(idb->phys_idb),
		   srcaddr);
	    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\nAT: ZIP GetNetInfo from %a invalid for %s",
		       srcaddr, idb_get_namestring(idb->phys_idb));
	}
    }
    zif = NULL;
    if (zip->zonelen != 0 && zip->zonename[0] != '*') {
	zt = atzip_FindZone(zip->zonename,zip->zonelen);
	if (zt != NULL)
	    zif = atalkif_findZone(idb,zt);
    } else 
	zt = NULL;
    if (zif != NULL) {
	zonename = zip->zonename;
	zonelen = zip->zonelen;
    } else {
	idbzt = atalkif_primary_zone(idb);
	if (idbzt == NULL) {
	    zonename = (uchar*)"*";
	    zonelen = 1;
	} else {
	    zonename = idbzt->name;
	    zonelen = idbzt->length;
	}
    }

    rzip = (zipnetinfo*) rpak->transport_start;
    nbytes = rpak->atalk_datalen;
    rzip->cmd = ZIP_GetNetInfoReply;
    rzip->flags = noNetFlags;
    PUTSHORT(rzip->netstart,idb->atalk_cablestart);
    PUTSHORT(rzip->netend,idb->atalk_cableend);
    rzip->zonelen = zip->zonelen;
    if (zip->zonelen != 0)
	bcopy(zip->zonename,rzip->zonename,zip->zonelen);
    zptr = ((uchar*)rzip->zonename) + rzip->zonelen;
    *zptr++ = IEEEBYTES;
    bcopy(atalk_zone_multicast(idb,atalk_zonesum(zonename,zonelen)),
	  zptr,IEEEBYTES);
    zptr += IEEEBYTES;
    nbytes -= (1 + 1 + DDPNETBYTES + DDPNETBYTES + 1 + rzip->zonelen + IEEEBYTES + 1);
    if (zif == NULL) {
	*zptr++ = zonelen;
	bcopy(zonename,zptr,zonelen);
	nbytes -= (zonelen + 1);
	rzip->flags |= zoneInvalid;
    }
    if (idb->atalk_zonecount == 1)
	rzip->flags |= onlyOneZone;
    atalk_pak_setlength(rpak,ZIP_MAX_GETNETINFO - nbytes);
    atalk_DDPcksum(rpak);
    if (atalk_send(rpak)) {
	atalk_stat[ATALK_ZIP_OUT]++;
	if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: Sent GetNetInfo reply to %a via %s",
		   srcaddr, idb_get_namestring(idb->phys_idb));
    }
}

/*
 * zip_validate_netinfo
 *
 * Check the reply to a GetNetInfo request for validity.
 */
void
zip_validate_netinfo (paktype* pak)
{
    ushort netstart, netend;
    atalkidbtype* idb = atalk_getidb(pak->if_input);
    uchar* zdefault = NULL;
    uchar* zmc = NULL;
    ziptabletype* zone;
    zipnetinfo* zip = (zipnetinfo*) pak->transport_start;

    atalk_pak_printprocess(pak, appletalk_debug,"NetInfoReply validation");
    if (atalkif_operational(idb)) {
	atalk_pak_printfail(pak, appletalk_debug,
			    "NetInfoReply for operational net ignored");
	return;
    }
    if (!atalkif_net_extended(idb) || !atalkif_InterfaceUp(idb)) {
	atalk_pak_printfail(pak, appletalk_debug,
			    "NetInfoReply from invalid port");
	return;
    }
    switch (atalkif_opstatus(idb)) {
    case opVerify:
    case opValidate:
    case opAcquire:
    case opAcquireZones:
    case opVerifyZones:
	break;
    default:
	atalk_pak_printfail(pak,appletalk_debug,
			    "unexpected NetInfoReply ignored");
	return;
    }
    zmc = ((uchar*)zip->zonename) + zip->zonelen;
    if ((zip->flags & zoneInvalid) != 0)
	zdefault = zmc + ((*zmc) + 1);
    if ((zip->zonelen > ZONELEN) ||
	(((*zmc) != IEEEBYTES) && ((*zmc) != 0)) ||
	((zdefault != NULL) && ((*zdefault) > ZONELEN))) {
	atalk_pak_printfail(pak,appletalk_debug,"NetInfoReply format is invalid");
	return;
    }
    if (atalkif_opstatus(idb) == opValidate) {
	if (zip->zonelen != 0) {
	    atalk_pak_printfail(pak,appletalk_debug,"NetInfoReply not for me");
	    return;
	}
	if (atalk_pak_srcaddr(pak) != idb->atalk_confaddr) {
	    atalk_pak_printfail(pak,appletalk_debug,"NetInfoReply ignored");
	    return;
	}
	if (zip_ValidatePrimary(idb,zdefault))
	    atalkif_PrimaryConfirmed(idb);
	else {
	    errmsg(AT_ERR_DEBUGMSG, "Cannot establish primary zone");
	    atalkif_ConfigError(idb,"could not establish primary zone");
	    atalk_pak_printfail(pak,appletalk_debug,
				"cannot establish primary zone");
	}
	return;
    }
    zone = atalkif_primary_zone(idb);
    if ((zone == NULL) &&
	(!idb->atalk_discovery)) {
	errmsg(AT_ERR_DEBUGMSG, "No primary zone setup but expected");
	atalk_pak_printfail(pak,appletalk_debug,"no primary has been setup");
	atalkif_ConfigError(idb,"no primary zone");
	return;
    }
    if (zone != NULL)
	if ((zip->zonelen == 0) ||
	    (zip->zonelen != zone->length) ||
	    (bcmp(zone->name,zip->zonename,zip->zonelen) != 0)) {
	    atalk_pak_printfail(pak,appletalk_debug,"NetInfoReply not for me");
	    return;
	}
    idb->atalk_confaddr = atalk_pak_srcaddr(pak);
    if (zone) {
	if ((zip->flags & zoneInvalid) != 0) {
	    errmsg(AT_ERR_ZONEDISAGREES, idb_get_namestring(idb->phys_idb),
		   idb->atalk_confaddr);
	    atalkif_ConfigError(idb,"primary zone invalid");
	    atalk_pak_printfail(pak,appletalk_debug,"primary zone invalid");
	    return;
	}
    } else {
	if (zdefault != NULL) {
	    ziptabletype* zt = atzip_FindZone(&zdefault[1],zdefault[0]);
	    if (zt != NULL)
		atalkif_addZone(idb,zt);
	}
    }
    netstart = GETSHORT(zip->netstart);
    netend = GETSHORT(zip->netend);
    if ((netstart != idb->atalk_cablestart) ||
	(netend != idb->atalk_cableend)) {
	errmsg(AT_ERR_NETDISAGREES, idb_get_namestring(idb->phys_idb),
	       idb->atalk_confaddr);
	atalkif_ConfigError(idb,"net information mismatch");
	atalk_pak_printfail(pak,appletalk_debug,"net information mismatch");
	return;
    }
    if ((zip->flags & useBroadcast) != 0) {
	atalkif_SetOurMulticasts(idb,atalk_broadcast(idb));
    } else {
	if ((zip->flags & zoneInvalid) == 0)
	    if ((zmc[0] != IEEEBYTES) || 
		!ieee_equal(&zmc[1],idb->atalk_zone_multicast)) {
		errmsg(AT_ERR_MCMISMATCH,
		       idb_get_namestring(idb->phys_idb), &zmc[1],
		       idb->atalk_zone_multicast);
		atalk_pak_printfail(pak,appletalk_debug,"multicast mismatch");
	    }
    }
    atalkif_NetConfirmed(idb);
    
    if (((zip->flags & onlyOneZone) != 0) &&
	((zip->flags & zoneInvalid) == 0)) {
	if (idb->atalk_zonecount == 1) {
	    atalkif_PrimaryConfirmed(idb);
	} else {
	    errmsg(AT_ERR_ZONEDISAGREES, idb_get_namestring(idb->phys_idb),
		   idb->atalk_confaddr);
	    atalkif_ConfigError(idb,"zones disagree");
	    atalk_pak_printfail(pak,appletalk_debug,"zones disagree");
	}
    }
}

/* The following old code is a brain-dead implemenation of ZIP Query
 * handling.  It sends the zonelist for each extended cable-range or
 * multi-zoned network number in a separate Extended ZIP Reply.
 * This is extremely ineffecient. 
 */
#if OLD_ZIP_QUERY_IMPLEMENTATION
/*
 * zip_ans_query
 * send off answers, if any, to Zip Query packet
 */
void
zip_ans_query (paktype *pak)
{
    paktype     *rpak;
    int 	   nbytes, sbytes, count, cnt;
    uchar 	  *p;
    ziptype 	  *z;
    zipentry 	  *ze;
    ushort 	   net;
    atalk_rdb_t	  *rt;
    ataddrtype     shost;
    zipnetentry	  *zn;
    neighborentry *neighbor;
    rbTree	  *tree;
    idbtype 	  *idb;			/* Used to access filtering */

    tree = atroute_GetTree();
    idb = pak->if_input;
    ATASSERT(idb != NULL);
    z = (ziptype *) pak->transport_start;
    p = ((uchar *)z)+ZIPTYPEBYTES; /* ptr to first byte of first net in pkt */
    sbytes = pak->atalk_datalen - ZIPTYPEBYTES;
    count = z->cnt;			/* Number of nets in packet */
    shost = atalk_pak_srcaddr(pak);
    neighbor = atmaint_GetNeighbor(shost, pak->if_input, FALSE);
    if (neighbor != NULL)
	neighbor->zipnetcount += count;
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d networks in ZIPquery pkt from %a",
	       count, shost);
    rpak = NULL;
    nbytes = 0;
    ze = NULL;
    cnt = 0;
    rt = NULL;
    for ( /* Empty */ ;(count > 0) && (sbytes > 0);
	 count--,p += DDPNETBYTES, sbytes -= DDPNETBYTES) {
	
	net = GETSHORT(p);	
	if (rt != NULL)
	    ReleaseRoute(rt);
	rt = atroute_FindRoute(tree,net,FALSE);
	if (rt == NULL) 
	    continue;
	if (rt->zoneupdate || (atalk_firstZone(rt) == NULL)) 
	    continue;
	
	/* Before allocating a pak, check if extended */
	if ((atalk_net_extended(rt) && (neighbor != NULL) &&
	    !neighbor->obsoleterouter) || (atalk_only_zone(rt) == NULL)) {
	    /*
	     * send Xreply if ATp2 neighbor OR >1 zone
	     */
	    send_query_xreply(pak,rt,neighbor);
	    continue;
	}

	/*
	 * send packet if full
	 */
	if ((nbytes < (ZONELEN + ZIPENTRYBYTES)) && (rpak != NULL)) {
	    z->cnt = cnt;
	    atalk_pak_setlength(rpak, rpak->atalk_datalen - nbytes);
	    atalk_DDPcksum(rpak);
	    if (atalk_send(rpak)) {
		atalk_stat[ATALK_ZIP_OUT]++;
		if (neighbor)
		    neighbor->zipreplies++;
		if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		    buginf("\nAT: Sent ZIP answer with %d nets to %a",
			   cnt, shost);
	    }
	    rpak = NULL;
	    nbytes = 0;
	    cnt = 0;
	}
	if (rpak == NULL) {
	    rpak = atalk_pak_create(pak->if_input,
				    shost,
				    atalk_pak_dsock(pak),
				    atalk_pak_ssock(pak), DDP_TYPE_ZIP,
				    DDP_MAX_DATABYTES);
	    if (rpak == NULL) {
		if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
		    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
		    buginf("\nAT: zip_ans_query getbuffer() failure");
		atalk_stat[ATALK_NO_BUFFERS]++;
		ReleaseRoute(rt);
		return;
	    }
	    nbytes = rpak->atalk_datalen;
	    nbytes -= ZIPTYPEBYTES;
	    z = (ziptype *)rpak->transport_start;
	    z->cnt = 0;
	    z->cmd = ZIP_Reply;
	    ze = (zipentry *)(((uchar *)z) + ZIPTYPEBYTES);
	}

	zn = atalk_firstZone(rt);
	if (zn == NULL)
	    continue;
	if ((zn->zone->length > ZONELEN)||(zn->zone->length == 0))
	    continue;
	if (idb->atalk_zipreply_filter)
	    if (!atalk_zonecheck(zn->zone, idb->atalk_zipreply_filter))
		continue;
#ifdef DEBUG
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: net %d, rt=%x, zn=%x", net, rt, zn);
#endif 
	PUTSHORT(ze->net, net);	/* put in network # */
	ze->len = (zn->zone)->length;	/* zone name length */
	bcopy((zn->zone)->name, ze->name, (zn->zone)->length); /* copy zone name */
	cnt++;				/* counter for msgs/neighbor */
	nbytes -= ZIPENTRYBYTES + ze->len - 1;
	ze = (zipentry *)(((uchar *)ze) + ZIPENTRYBYTES + ze->len - 1);
    }
    if (rt != NULL)
	ReleaseRoute(rt);
    if (rpak == NULL)			/* probably only extended nets */
	return;
    if (cnt == 0) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: null zip reply to %a suppressed",shost);
	datagram_done(rpak);
	return;
    }
    z->cnt = cnt;
    atalk_pak_setlength(rpak, rpak->atalk_datalen - nbytes);
    atalk_DDPcksum(rpak);
    if (atalk_send(rpak)) {
	atalk_stat[ATALK_ZIP_OUT]++;
	if (neighbor)
	    neighbor->qreplies++;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Sent ZIP answer with %d nets to %a",
		   cnt, shost);
    }
}
#else /* OLD_ZIP_QUERY_IMPLEMENTATION */
static void
zip_FinishReply(paktype *pak, int cnt, int nbytes, neighborentry *neighbor)
{
    ziptype *z;
    
    if (cnt == 0) {		/* No zones in packet! */
	datagram_done(pak);
	return;
    }
	
    z = (ziptype *) pak->transport_start;
    
    z->cmd = ZIP_Reply;
    z->cnt = cnt;
    atalk_pak_setlength(pak, pak->atalk_datalen - nbytes);
    atalk_DDPcksum(pak);
    if (atalk_send(pak)) {
	atalk_stat[ATALK_ZIP_OUT]++;
	if (neighbor)
	    neighbor->qreplies++;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Sent ZIP answer with %d nets to %a",
		   cnt, atalk_pak_dstaddr(pak));
    }
}

/*
 * zip_ans_query
 *
 * Send ZIP Replies to a Query contained in pak.
 *
 */
void
zip_ans_query (paktype *pak)
{
    rbTree *      tree;
    atalkidbtype *     idb;
    neighborentry *
	          neighbor;
    paktype *     newpak;
    ataddrtype    shost;
    atalk_rdb_t * route;
    int           filter_list;	/* Used for access filtering */

    ziptype *     ZipHdrPtr;	/* Points to ZIP data */

    int           ZipQueryDataLen; /* # of unprocessed bytes in Zip Query */
    int           incnt;	/* # of networks in ZIP Query */
    uchar *       ZipQueryNetPtr; /* Points to network # in ZIP Query */

    int           ZipReplyFreeBytes; /* # of free space left in ZIP Reply */
    int           outcnt;	/* # of networks in ZIP Reply */

    int           AllZonesLen;	/* Length of all zones for a network */
    int           NumZones;	/* Number of zones for a network */
    int           ZoneListLen;	/* Amt. of packet space needed to store list */
    zipentry *    ze;		/* Used to write zonelists into Zip Reply */
    zipnetentry * zn;
    
    ushort        query_net, remap_net;

    tree = atroute_GetTree();
    idb = atalk_getidb(pak->if_input);
    ZipHdrPtr = (ziptype *) pak->transport_start;
    ZipQueryNetPtr = pak->transport_start + ZIPTYPEBYTES; /* ptr to first net */
    ZipQueryDataLen = pak->atalk_datalen - ZIPTYPEBYTES;
    incnt = ZipHdrPtr->cnt;
    shost = atalk_pak_srcaddr(pak);
    neighbor = atmaint_GetNeighbor(shost, idb, FALSE);
    if (neighbor != NULL)
	neighbor->zipnetcount += incnt;
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d networks in ZIPquery pkt from %a", incnt, shost);
    newpak = NULL;
    outcnt = 0;
    ZipReplyFreeBytes = 0;
    ze = NULL;
        

    /* The following for loop goes through each network in the ZIP
     * Reply.
     *
     * Obtain the total number and length of zones for the next
     * network in the ZIP Query.  If the zone list is too long to fit
     * into a single ZIP Reply packet, use Extended ZIP Replies.
     * Otherwise, if there isn't enough free space left in the current
     * ZIP Reply packet, send the packet out.
     *
     * If this is the first time through the for loop or if we just
     * sent a filled-up ZIP Reply packet, we need to request a new
     * packet to build the ZIP Reply.
     *
     * Finally, write the zone list into packet.
     */
    for (;incnt > 0 && ZipQueryDataLen > 0;
	 ZipQueryNetPtr += DDPNETBYTES, ZipQueryDataLen -= DDPNETBYTES,
	 incnt--) {

	query_net = GETSHORT(ZipQueryNetPtr);

	if(idb->atalk_domain && idb->atalk_dminfo) {
	    remap_net = atdomain_FindBackwardRemap(idb->atalk_dminfo->out,query_net);
	    if(remap_net)
		query_net = remap_net;
	}
/*	route = atroute_FindRoute(tree, GETSHORT(ZipQueryNetPtr), FALSE);*/
	route = atroute_FindRoute(tree, query_net, FALSE);
	
	if (route == NULL)
	    continue;
	
	if (route->zoneupdate || (atalk_firstZone(route) == NULL)) {
	    ReleaseRoute(route);
	    continue;
	}

	filter_list = idb->atalk_zipreply_filter;
	
	/* Find total length and number of zones. */
	atalk_FilteredZoneListLengthAndCount(route, &AllZonesLen, &NumZones,
					     filter_list);

	if (NumZones == 0 || AllZonesLen == 0) {
	    ReleaseRoute(route);
	    continue;
	}

	/* Calculate space needed to put zonelist into packet */
	ZoneListLen = AllZonesLen + NumZones * (ZIPENTRYBYTES - 1);
	
	if (ZoneListLen > DDP_MAX_DATABYTES) {
	    /* Zone list won't fit into a single packet, so send
	     * several Extended ZIP Replies.
	     */
	    send_query_xreply(pak, route, neighbor);
	    ReleaseRoute(route);
	    continue;
	}

	if (newpak && ZipReplyFreeBytes < ZoneListLen) {
	    /* No room left in packet to fit zonelist for this route,
	     * so kick off this packet.
	     */
	    zip_FinishReply(newpak, outcnt, ZipReplyFreeBytes, neighbor);
	    newpak = NULL;
	    ZipReplyFreeBytes = 0;
	    outcnt = 0;
	}
	if (newpak == NULL) {
	    newpak = atalk_pak_create(atalk_getidb(pak->if_input),
				      shost,
				      atalk_pak_dsock(pak),
				      atalk_pak_ssock(pak), DDP_TYPE_ZIP,
				      DDP_MAX_DATABYTES);
	    if (newpak == NULL) {
		ReleaseRoute(route);
		continue;
	    }
	    ZipReplyFreeBytes = newpak->atalk_datalen - ZIPTYPEBYTES;
	    outcnt = 0;
	    ze = (zipentry *) (newpak->transport_start + ZIPTYPEBYTES);
	}

	for (zn = atalk_firstZone(route); zn; zn = atalkzn_nextZone(zn)) {
	    /* check for access filtering */
	    if (!atalk_zonecheck(zn->zone, filter_list))
		continue;

	    if(idb->atalk_domain && idb->atalk_dminfo) {
		remap_net = atdomain_FindForwardRemap(idb->atalk_dminfo->out,
						      route->dstrngstr);
		if (remap_net)
		    PUTSHORT(ze->net, remap_net);
		else
		    PUTSHORT(ze->net, route->dstrngstr);
	    } else
		PUTSHORT(ze->net, route->dstrngstr);

	    ze->len = zn->zone->length;
	    bcopy(zn->zone->name, ze->name, zn->zone->length);
	    outcnt++;
	    ZipReplyFreeBytes -= ZIPENTRYBYTES + ze->len - 1;
	    ze = (zipentry *) (((uchar *) ze) + ZIPENTRYBYTES + ze->len - 1);
	}
	ReleaseRoute(route);
    }
    if (newpak)
	zip_FinishReply(newpak, outcnt, ZipReplyFreeBytes, neighbor);
    
}
#endif /* OLD_ZIP_QUERY_IMPLEMENTATION */

/*
 * send_query_xreply
 * send off extended answers, if any, to Zip Query packet
 */
static void
send_query_xreply (paktype* pak,atalk_rdb_t* rt,neighborentry* neighbor)
{
    paktype   *rpak;
    int 	 nbytes, count, totalzones;
    ziptype 	*z;
    zipentry 	*ze;
    ataddrtype 	 shost;
    zipnetentry *tzn;
    atalkidbtype	*idb;
    int		 filter_list;
    ushort      remap_net;

    idb = atalk_getidb(pak->if_input);
    filter_list = idb->atalk_zipreply_filter;

    /* Count the zones, excluding any which are access filtered. */
    totalzones = atalk_CountAndFilterZones(rt, filter_list);

    count = 0;
    shost = atalk_pak_srcaddr(pak);
    rpak = NULL;
    nbytes = 0;
    ze = NULL;
    for (tzn=atalk_firstZone(rt);tzn;tzn = atalkzn_nextZone(tzn)) {

	/* check for access filtering */
	if (!atalk_zonecheck(tzn->zone, filter_list))
	    continue;

	/* send packet if full */
	if ((nbytes < (tzn->zone->length + ZIPENTRYBYTES - 1)) && (rpak != NULL)) {
	    atalk_pak_setlength(rpak, DDP_MAX_DATABYTES - nbytes);
	    atalk_DDPcksum(rpak);
	    if (atalk_send(rpak)) {
		atalk_stat[ATALK_ZIP_OUT]++;
		if (neighbor)
		    neighbor->qxreplies++;
		if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		    buginf("\nAT: Sent extended ZIP reply with %d of %d zones to %a",
			   count, totalzones, shost);
	    }
	    rpak = NULL;
	    nbytes = 0;
	    count = 0;
	}
	if (rpak == NULL) {
	    rpak = atalk_pak_create(idb,
				    shost,
				    atalk_pak_dsock(pak),
				    atalk_pak_ssock(pak), DDP_TYPE_ZIP,
				    DDP_MAX_DATABYTES);
	    if (rpak == NULL) {
		if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
		    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
		    buginf("\nAT: zip_ans_query getbuffer() failure");
		atalk_stat[ATALK_NO_BUFFERS]++;
		return;
	    }
	    nbytes = rpak->atalk_datalen;
	    nbytes -= ZIPTYPEBYTES;
	    z = (ziptype *)rpak->transport_start;
	    z->cnt = totalzones;
	    z->cmd = ZIP_ExtendedReply;
	    ze = (zipentry *)(((uchar *)z) + ZIPTYPEBYTES);
	}

	if(idb->atalk_domain && idb->atalk_dminfo)  {
	    remap_net = atdomain_FindForwardRemap(idb->atalk_dminfo->out,
						  rt->dstrngstr);
	    if (remap_net)
		PUTSHORT(ze->net, remap_net); /* put in network # */
	    else
		PUTSHORT(ze->net, rt->dstrngstr); /* put in network # */
	} else
	    PUTSHORT(ze->net, rt->dstrngstr); /* put in network # */

	ze->len = (tzn->zone)->length;	/* zone name length */
	bcopy((tzn->zone)->name, ze->name, (tzn->zone)->length);
	count++;
	nbytes -= ZIPENTRYBYTES + ze->len - 1;
	ze = (zipentry *)(((uchar *)ze) + ZIPENTRYBYTES + ze->len - 1);
    }
    if (rpak == NULL)		/* probably only extended nets */
	return;
    if (count == 0) {
	datagram_done(rpak);
	return;
    }
    atalk_pak_setlength(rpak, DDP_MAX_DATABYTES - nbytes);
    atalk_DDPcksum(rpak);
    if (atalk_send(rpak)) {
	atalk_stat[ATALK_ZIP_OUT]++;
	if (neighbor)
	    neighbor->qxreplies++;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Sent ZIP final extended reply with %d of %d zones to %a",
		   count, totalzones, shost);
    }
}

/*
 * zip_merge_reply
 * merge data from ZIP reply packet into our tables
 */
static void
zip_MergeReply (paktype* pak)
{
    int cnt;
    ziptype *zhdr;
    zipentry *zip;
    atalk_rdb_t *r;
    atalk_rdb_t *zlist;
    sys_timestamp now;
    ataddrtype zipaddr;
    ushort net;
    rbTree	*tree;
    ushort remap_net;
    atalkidbtype *idb;
    
    idb = atalk_getidb(pak->if_input);
    
    if(pak->if_input && idb->atalk_dminfo) {
	if(atdomain_ProcessZIP(pak)) {
	    atdomain_ZIPReply(pak);
	    return;
	}
    }

    GET_TIMESTAMP(now);
    tree = atroute_GetTree();
    zipaddr = atalk_pak_srcaddr(pak);
    zlist = NULL;		/* Changed route list is empty */
    zhdr = (ziptype *)pak->transport_start;
    cnt = zhdr->cnt;		/* get network count */
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d zones in ZIPreply pkt, src %a", cnt,zipaddr);
    if (cnt == 0)
	return;
    zip = (zipentry *)(((uchar *)zhdr) + ZIPTYPEBYTES); /* skip ZIP header */
    while (cnt > 0) {
	net = GETSHORT(zip->net);


	if (atalk_DomainRouter && idb->atalk_dminfo) {
	    remap_net = atdomain_FindForwardRemap(idb->atalk_dminfo->in, net);
	    if (remap_net)
		net = remap_net;
	}

	if ((zip->len > ZONELEN) || (zip->len == 0))
	    break;
	r = atroute_FindRoute(tree,net, FALSE);
	if (r != NULL)	{	/* found it */
	    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		buginf("\nAT: net %d, zonelen %d, name %*s",
		       net, zip->len, zip->len, zip->name);
	    /* Cayman Tunnels, half-routers and similar beasts have no
	     * zipaddr.  If we don't check for a zero zipaddr,
	     * routes won't get properly released if the Reply contains
	     * multiple zones for a single network.
	     */
	    if (!r->zoneupdate || ((r->zoneaddr == 0) && zipaddr)) {
		atalk_clear_zones(r);
		r->zoneupdate = TRUE;
		r->zoneaddr = zipaddr;
		r->zonetimer = now;
		r->zonecnt = 0;
		r->znext = zlist;
		zlist = r;
	    } else {
		/*
		 * This case releases the route for additional zones in the 
		 * same packet and for routes which will not be updated
		 * at all because we are ignoring it otherwise we would 
		 * lock it too many times and not know how many calls to
		 * release to make when we finish processing.
		 */
		ReleaseRoute(r);
	    }
	    if (r->zoneaddr == zipaddr) {
		(void) atzip_AddZone(r, zip->name, zip->len);
		if (at_debug(atalk_events, ATALK_NULL_MASK))
		    errmsg(AT_ERR_NEWZONE, zip->len, zip->name);
	    }
	} else {			/* not there?  hmm... */
	    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
		|| at_debug(atalkzip_debug, ATALK_NULL_MASK))
		buginf("\nAT: rcvd zone for net %d, but no route!",net);
	}
	zip = (zipentry *)(((uchar *)zip) + ZIPENTRYBYTES + zip->len - 1);
	cnt--;
    }
    if ((cnt != 0) && at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: Invalid zone in reply from %a",zipaddr);
    while (zlist != NULL) {
	r = zlist;
	zlist = zlist->znext;
	r->znext = NULL;
	zip_FinishMerge(r, now);
	/* 
	 * For now, only redist best_path.  Eventually, we will have to
         * redist all equalpaths, too.
	 */
	atroute_path_redist_rbshell((treeLink *)r->best_path, FALSE);
	ReleaseRoute(r);
    }
}

/*
 * zip_MergeXReply
 * Merge an extended ZIP reply.
 */
static void
zip_MergeXReply (paktype* pak)
{
    int cnt;
    ziptype *zhdr;
    zipentry *zip;
    atalk_rdb_t *r;
    sys_timestamp now;
    ushort net;
    ataddrtype zipaddr = atalk_pak_srcaddr(pak);
    boolean err = FALSE;
    int len;
    rbTree	*tree;
    ushort remap_net = 0;
    atalkidbtype *idb;
    
    idb = atalk_getidb(pak->if_input);
    
    if(pak->if_input && idb->atalk_dminfo) {
	if(atdomain_ProcessZIP(pak)) {
	    atdomain_ZipXReply(pak);
	    return;
	}
    }

    GET_TIMESTAMP(now);
    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK))
	buginf("\nAT: ZIP Extended reply rcvd from %a", zipaddr);
    tree = atroute_GetTree();
    zhdr = (ziptype *)pak->transport_start;
    cnt = zhdr->cnt;			/* get network count */
    zip = (zipentry *)(((uchar *)zhdr) + ZIPTYPEBYTES); /* skip ZIP header */
    net = GETSHORT(zip->net);
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d zones for %d, ZIP XReply, src %a",cnt,net,
	       zipaddr);

    if (idb->atalk_domain && idb->atalk_dminfo) {
	remap_net = atdomain_FindForwardRemap(idb->atalk_dminfo->in, net);
    }
    if (remap_net)
	r = atroute_FindRoute(tree,remap_net, FALSE);
    else
	r = atroute_FindRoute(tree,net, FALSE);

    if (r == NULL) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Got names for net %d but no route",zip->net);
	return;
    }
    if (r->zoneupdate && (r->zonecnt != 0)) {
	if (r->zoneaddr != zipaddr) {
	    ReleaseRoute(r);
	    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		buginf("\nAT: Ignoring ZIP from %a, updating via %a",
		       zipaddr,r->zoneaddr);
	    return;
	}
    } else {
	atalk_clear_zones(r);
	r->zoneupdate = TRUE;
	r->zonecnt = cnt;
	r->zoneaddr = zipaddr;
    }
    r->zonetimer = now;
    len = pak->atalk_datalen;
    len -= ZIPTYPEBYTES;
    while ((len >= ZIPENTRYBYTES) && (r->zonecnt > 0)) {
	if ((GETSHORT(zip->net) != net)||(zip->len == 0)||
	    (zip->len > ZONELEN)) {
	    err = TRUE;
	    break;
	}
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: net %d, zonelen %d, name %*s",net,zip->len, 
		   zip->len, zip->name);
	(void) atzip_AddZone(r, zip->name, zip->len);
	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_NEWZONE, zip->len, zip->name);
	r->zonecnt--;
	len -= ZIPENTRYBYTES + zip->len - 1;
	zip = (zipentry *)(((uchar *)zip) + ZIPENTRYBYTES + zip->len - 1);
    }
    if (err) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Bad reply for %d, ZIPxreply pkt, src %a",net,
		   zipaddr);
	atalk_clear_zones(r);
    }
    if ((r->zonecnt <= 0)||err) {
	zip_FinishMerge(r, now);
    }
    /* 
     * For now, only redist best_path.  Eventually, we will have to
     * redist all equalpaths, too.
     */
    atroute_path_redist_rbshell((treeLink *)r->best_path, FALSE);
    ReleaseRoute(r);
}


/*
 * zip_GetNextZone 
 * get next zone entry in zip hash table
 */
ziptabletype* 
zip_GetNextZone (atalkidbtype* idb,ziptabletype* zone,int* bucket)
{
    if (*bucket >= ZIPHASHLEN)
	return (NULL);
    if (zone == NULL && *bucket == 0)	/* must have been first call */
	zone = zip_table[0];		/* get start of 1st chain, if avail. */
    else if (zone != NULL)		/* go to next entry and then search */
	zone = zone->next;
    /* now try to get a useful entry */
    while ((*bucket < (ZIPHASHLEN-1))||(zone != NULL)) {
	if (zone == NULL)		/* no zone... go to the next bucket */
	    zone = zip_table[++(*bucket)];
	if (zone == NULL)		/* empty bucket... restart */
	    continue;
	if (atalk_firstNet(zone) != NULL)
	    break;		/* finished if zone has active routes */
	zone = zone->next;	/* inactive zone, try next in chain */
    }
    return (zone);		/* ran out of buckets or found a zone */
}

/*
 * zip_GZLGetNextZone
 * get next zone entry, performing any access checks, if necessary.
 */
static ziptabletype *
zip_GZLGetNextZone (atalkidbtype *idb, ziptabletype *zone, int *bucket)
{
    while (1) {
	zone = zip_GetNextZone(idb, zone, bucket);
	if (zone == NULL)		/* no more zones?  stop looking. */
	    break;
	if (idb->atalk_route_filter != 0)
	    if (!atalk_gzlzonecheck(zone, idb->atalk_route_filter)
		|| !atalk_rtmpzonecheck(zone, idb->atalk_route_filter)) {
		continue;
	    }
	if (idb->atalk_gzlfilter != 0)
	    if (!atalk_zonecheck(zone,idb->atalk_gzlfilter)) {
		continue;
	    }
	break;				/* zone passed muster.  stop looking */
    }
    return (zone);
}

/*
 * zip_SendGMZ
 * send get my zone
 */
static void
zip_SendGMZ (paktype* pak)
{
    paktype *rpak;
    atptype *atp, *ratp;
    uchar* p;
    zipifentry* zif;
    int len;
    atalkidbtype* idb = atalk_getidb(pak->if_input);

    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	|| at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: GetMyZone request rcvd from %a via %s", 
	       atalk_pak_srcaddr(pak), idb_get_namestring(idb->phys_idb));

    if (atalkif_net_extended(idb)) {
	if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: GetMyZone request on %s ignored - extended net", 
		   idb_get_namestring(idb->phys_idb));
	return;
    }

    rpak = atalk_pak_reply(pak, atalk_pak_getpt(pak), 
			   ATP_OVERHEADBYTES + ZONELEN + 1);
    if (rpak == NULL) {
	if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
	    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: GetMyZone getbuffer() failure");
	atalk_stat[ATALK_NO_BUFFERS]++;
	return;
    }
    idb = atalk_getidb(rpak->if_input);	/* pak_reply looks up idb for dst! */

    zif = atalkif_firstZone(idb);
    ratp = (atptype *)rpak->transport_start;
    ratp->ccf = GZLRep1;
    ratp->bitmap = GZLRep2; 
    atp = (atptype *)pak->transport_start;
    ratp->tidlo = atp->tidlo;
    ratp->tidhi = atp->tidhi;
    ratp->userbytes[0] = 0;	/* GMZ */
    ratp->userbytes[1] = 0;	/* always 0 */
    PUTSHORT(&ratp->userbytes[2], 1); /* one zone in packet */
    p = ((uchar *)ratp) + ATP_OVERHEADBYTES;
    len = 0;
    if (zif != NULL) 
	len += (zif->zone)->length;
    *p++ = len;
    if (zif != NULL)
	bcopy((zif->zone)->name, p, len);
    len += ATP_OVERHEADBYTES + 1;
    atalk_pak_setlength(rpak,len);
    atalk_DDPcksum(rpak);
    if (atalk_send(rpak))
	atalk_stat[ATALK_ZIP_OUT]++;
}

/*
 * zip_SendGLZ
 * Send Get Local Zones reply.
 */
static void
zip_SendGLZ (paktype* pak)
{
    paktype *rpak;
    atptype *atp, *ratp;
    uchar *p;
    int len, nbytes, startindex;
    int cnt, zonecnt;
    ziptabletype* zone;
    zipifentry* zif;
    atalkidbtype* idb;

    atp = (atptype *)pak->transport_start;
    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, pak->if_input)
	|| at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, pak->if_input))
	buginf("\nAT: GetLocalZones request rcvd from %a via %s",
	       atalk_pak_srcaddr(pak), idb_get_namestring(pak->if_input));

    rpak = atalk_pak_reply(pak, atalk_pak_getpt(pak), DDP_MAX_DATABYTES);

    if (rpak == NULL) {
	if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
	    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: GetLocalZones getbuffer() failure");
	atalk_stat[ATALK_NO_BUFFERS]++;
	return;
    }
    idb = atalk_getidb(rpak->if_input);	/* pak_reply looks up idb for dst! */

    ratp = (atptype *)rpak->transport_start;
    nbytes = rpak->atalk_datalen;
    nbytes -= ATP_OVERHEADBYTES;
    ratp->tidlo = atp->tidlo;
    ratp->tidhi = atp->tidhi;
    ratp->ccf = GZLRep1;
    ratp->bitmap = GZLRep2; 
    ratp->userbytes[0] = 1;	/* lastflag is true, no more zones */
    ratp->userbytes[1] = 0;	/* always 0 */
    
    p = &atp->userbytes[2];
    startindex = GETSHORT(p);
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: GetLocalZones startindex %d, src %a",
	       startindex, atalk_pak_srcaddr(pak));
    p = ((uchar *)ratp) + ATP_OVERHEADBYTES;
    len = 0;
    zone = NULL;
    cnt = 0;
    zif = atalkif_firstZone(idb);
    zonecnt = 0;
    while (nbytes >= ZONELEN+1 && (zif != NULL)) {
	zone = zif->zone;
	zif = atalkif_nextZone(zif);
	cnt++;
	if (cnt < startindex) /* skip this one, they already have it */
	    continue;
	*p++ = zone->length;
	len += zone->length + 1;
	bcopy(zone->name, p, zone->length);
	p += zone->length;
	nbytes -= zone->length + 1;
	zonecnt++;
    }
    len += ATP_OVERHEADBYTES;
    atalk_pak_setlength(rpak,len);
    if (cnt < idb->atalk_zonecount)
	ratp->userbytes[0] = 0; /* clear LastFlag - more zones to come */
    p = &ratp->userbytes[2];
    PUTSHORT(p, zonecnt);
#ifdef DEBUG
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: GetLocalZones reply userbytes %d %d %d %d",
	       ratp->userbytes[0], ratp->userbytes[1], ratp->userbytes[2],
	       ratp->userbytes[3]);
#endif
    atalk_DDPcksum(rpak);
    if (atalk_send(rpak))
	atalk_stat[ATALK_ZIP_OUT]++;
}

/*
 * zip_SendGZL
 * Send Get Zone List reply.
 */
static void
zip_SendGZL (paktype* pak)
{
    paktype *rpak;
    atptype *atp, *ratp;
    uchar *p;
    int len, nbytes, startindex;
    int bucket, cnt, zonecnt;
    ziptabletype* zone;
    atalkidbtype* idb;

    atp = (atptype *)pak->transport_start;
    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, pak->if_input)
	|| at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, pak->if_input))
	buginf("\nAT: GetZoneList request rcvd from %a via %s",
	       atalk_pak_srcaddr(pak), idb_get_namestring(pak->if_input));

    rpak = atalk_pak_reply(pak, atalk_pak_getpt(pak), DDP_MAX_DATABYTES);
    if (rpak == NULL) {
	if (at_debug(atalkpkt_debug, ATALK_NULL_MASK)
	    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: GetZoneList getbuffer() failure");
	atalk_stat[ATALK_NO_BUFFERS]++;
	return;
    }
    idb = atalk_getidb(rpak->if_input);	/* pak_reply looks up idb for dst! */

    ratp = (atptype *)rpak->transport_start;
    nbytes = rpak->atalk_datalen;
    nbytes -= ATP_OVERHEADBYTES;
    ratp->tidlo = atp->tidlo;
    ratp->tidhi = atp->tidhi;
    ratp->ccf = GZLRep1;
    ratp->bitmap = GZLRep2; 
    ratp->userbytes[0] = 0;	/* lastflag is false, probably more zones */
    ratp->userbytes[1] = 0;	/* always 0 */
    
    p = &atp->userbytes[2];
    startindex = GETSHORT(p);
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: GetZoneList startindex %d, src %a",
	       startindex, atalk_pak_srcaddr(pak));
    p = ((uchar *)ratp) + ATP_OVERHEADBYTES;
    bucket = 0;
    len = 0;
    zone = NULL;
    cnt = 0;
    zonecnt = 0;
    while (nbytes >= ZONELEN+1) {
	zone = (ziptabletype *)zip_GZLGetNextZone(idb,zone, &bucket);
#ifdef DEBUG
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK) && (zone != NULL))
	    buginf("\nAT: zone %#x, name %*s, bucket %d, cnt %d",
		   zone, zone->length, zone->name, bucket, cnt);
#endif
	if (zone == NULL) 
	    break;
	cnt++;
	if (cnt < startindex) /* skip this one, they already have it */
	    continue;
	*p++ = zone->length;
	len += zone->length + 1;
	bcopy(zone->name, p, zone->length);
	p += zone->length;
	nbytes -= zone->length + 1;
	zonecnt++;
    }
    len += ATP_OVERHEADBYTES;
    atalk_pak_setlength(rpak,len);
    if ((zone == NULL) || (zip_GZLGetNextZone(idb,zone, &bucket) == NULL))
	ratp->userbytes[0] = 1; /* set LastFlag - no more zones to come */
    p = &ratp->userbytes[2];
    PUTSHORT(p, zonecnt);
#ifdef DEBUG
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: GetZoneList reply userbytes %d %d %d %d", ratp->userbytes[0],
	       ratp->userbytes[1], ratp->userbytes[2], ratp->userbytes[3]);
#endif
    atalk_DDPcksum(rpak);
    if (atalk_send(rpak))
	atalk_stat[ATALK_ZIP_OUT]++;
}

/*
 * zip_GZL
 * handle GetZoneList or GetMyZone requests
 */
static void
zip_GZL (paktype* pak)
{
    atptype *atp;

    atp = (atptype *)pak->transport_start;

    if (atp->userbytes[1] != 0) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: unused field is nonzero in GZL/GMZ user byte");
	return;
    }
    
    switch (atp->userbytes[0]) {
    case GLZ:			/* GetLocalZones */
	zip_SendGLZ(pak);
	break;
    case GMZ:			/* GetMyZone */
	zip_SendGMZ(pak);
	break;
    case GZL:			/* GetZoneList */
	zip_SendGZL(pak);
	break;
    default:
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK)
	    || at_debug(atalkpkt_debug, ATALK_NULL_MASK))
	    buginf("\nAT: illegal GZL/GMZ user byte = %d", atp->userbytes[0]);
	break;
    }
}

static boolean
zip_ValidatePrimary (atalkidbtype* idb,uchar* zdefault)
{
    ziptabletype* zone;
    ziptabletype* zoneentry;

    zone = atalkif_primary_zone(idb);
    if ((zone == NULL) || (zdefault == NULL)) {
	atalkif_ConfigError(idb,"internal error");
	errmsg(AT_ERR_ZONEDISAGREES, idb_get_namestring(idb->phys_idb),
	       idb->atalk_confaddr);
	return (FALSE);
    }
    zoneentry = atzip_FindZone(&zdefault[1],zdefault[0]);
    if (zoneentry == NULL) {
	errmsg(AT_ERR_ZONEDISAGREES,idb_get_namestring(idb->phys_idb),
	       idb->atalk_confaddr);
	return (FALSE);
    }
    if (zone != zoneentry) {
	if (idb->atalk_discovery) {
	    if (atalkif_findZone(idb,zoneentry)) {
		atalkif_set_primary(idb,zoneentry);
		return (TRUE);
	    }
	}
	errmsg(AT_ERR_DEFZONEERR, idb_get_namestring(idb->phys_idb),
	       idb->atalk_confaddr);
	return (TRUE);
    }
    return (TRUE);
}

/*
 * atzip_FindZone
 * scan ZIP table looking for zone entry matching given zone
 */
ziptabletype*
atzip_FindZone (uchar* name, uint len)
{
    int bucket;
    ziptabletype *p;

    bucket = atalk_zonesum(name, len) % ZIPHASHLEN;
    p = zip_table[bucket];
    while (p) {
	if (atnbp_namematch(name, p->name, len, p->length, NBP_ZONE_WILD) == TRUE)
	    return (p);
	p = p->next;
    }
    return (NULL);
}

/*
 * zip_GC
 * scan ZIP table looking for zones that no longer have nets in the
 * routing table and delete them
 */
static boolean
atzip_GC (void)
{
    int i;
    ziptabletype *z, *prev, *next;
    zipnetentry *entry;

    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: atzip_GC() called");
    /*
     * If someone is sitting at a "more" prompt or other such in the middle
     * of a "show appletalk zone" command, don't start a GC pass right now.
     * Just back out and get rescheduled.
     */
    if (atalk_InRouteZoneWeb > 0) {
	return (FALSE);
    }

    zip_gcNeeded = 0;
    zip_gcRuns++;
    zip_zones = 0;
    for (i = 0; i < ZIPHASHLEN; i++) {
	z = zip_table[i];
	prev = z;
	while (z) {
	    if (((z->netlist == NULL)
		 || ((z->netlist->deleted == TRUE)
		     && atalkzn_nextNet(z->netlist) == NULL))
		&& (z->ifcount == 0) && (z->acccount == 0)) {
		if (z != prev) 	/* head of chain? */
		    prev->next = z->next; /* no, splice out */
		else {
		    zip_table[i] = z->next; /* yes, anchor rest of chain */
		    prev = z->next; /* fix prev to point at start of chain */
		}
		next = z->next;
		if (at_debug(atalk_events, ATALK_NULL_MASK))
		    errmsg(AT_ERR_ZONEGC, z->pname);
		if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		    buginf("\nAT: freeing zip entry for %*s, %#x",
		       z->length, z->name, z);
		/* First, detach netlist from table entry. */
		entry = z->netlist;
		while (entry != NULL) {
		    entry->zone = NULL;
		    entry = entry->nextNet;
		}
		/* Second, free the table entry. */
		free(z);
		zip_freed++;
		z = next;
	    } else {
		zip_zones++;	/* count up another zone */
		prev = z;
		z = z->next;
	    }
	}
    }
    return (TRUE);
}

/*
 * atzip_CreateZone
 * Create a new zone.
 */
ziptabletype*
atzip_CreateZone (uchar* zone, uint len)
{
    ziptabletype *z;
    int bucket;

    z = atzip_FindZone(zone,len);
    if (z != NULL)
	return (z);
    z = malloc_named(sizeof(ziptabletype), "Atalk Zone");
    if (z == NULL)
	return (NULL);
    zip_created++;
    bcopy(zone, z->name, len);
    z->length = len;
    z->ifcount = 0;
    z->acccount = 0;
    z->hashval =  atalk_zonesum(zone, len);
    bucket = z->hashval % ZIPHASHLEN;
    z->next = zip_table[bucket];
    if (z->next != NULL)
	zip_hashCollisions++;
    zip_table[bucket] = z;
    z->pname[0] = '\0';
    appletalk_zonename(z->name,z->length,z->pname);
    zip_zones++;
    return (z);
}

/*
 * atzip_AddZone
 * add a zonename to an existing rtmp_table entry and zip hash table
 */
ziptabletype*
atzip_AddZone (atalk_rdb_t* r,uchar* zone,uint len)
{
    ziptabletype* z = atzip_CreateZone(zone,len);

    if (z != NULL)  {
	if (atalk_find_and_use_zipnet(r, z) == NULL) {
	    if (atalk_firstZone(r) != NULL) {
		if (atalk_obsoleteRouterActive && !atalk_extendedInternet)
		    errmsg(AT_ERR_COMPATERR2,atalk_CableRange(r));
		atalk_extendedInternet = TRUE;
	    }
	    atalk_join_zipnet(r,z);
	}
    }

    if(atalk_DomainRouter_running)
	atdomain_CheckForLoop(r,zone,len);

    return (z);
}


/*
 * atzip_SendQuery
 * build and send a zip query packet for network
 */
void
atzip_SendQuery (atalk_rdb_t* net)
{
    paktype *pak;
    ziptype *zip;
    ataddrtype dhost;
    atalkidbtype	*pak_idb = NULL;
    idbtype	*if_output = NULL;
    ushort      domain_net;


    pak_idb = net->rpath.idb;
    /* Route may have been poisoned. */
    if (!pak_idb)
        return;

    if (atalk_net_connected(net) || (net->zoneaddr == 0)) {
	dhost = atalkif_CableBroadcast(pak_idb);
	if_output = pak_idb->phys_idb;
    } else {
	dhost = net->zoneaddr;
    }
    if (at_debug(atalkzip_debug, 0)) {
	buginf("\nAT: atzip_SendQuery, sending query for 1 net, %d",
	       net->dstrngstr);
    }
    pak = atalk_pak_create(pak_idb, dhost,
			   ATALK_SOCK_ZIP, ATALK_SOCK_ZIP, DDP_TYPE_ZIP, 
			   ZIPTYPEBYTES+DDPNETBYTES);
    if (pak == NULL) {
	if (at_debug(atalkerr_debug, ATALK_NULL_MASK)
	    || at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: atzip_SendQuery getbuffer() failure");
	atalk_stat[ATALK_NO_BUFFERS]++;
	return;
    }
    pak->if_output = if_output;
    zip = (ziptype *)pak->transport_start;
    zip->cmd = ZIP_Query;
    zip->cnt = 1;		/* only ask for one network/query  */
    PUTSHORT(((uchar *)zip+ZIPTYPEBYTES), net->dstrngstr);


    if(atalk_DomainRouter_running) {
	atalkidbtype *atalkidb_o;

	atalkidb_o = atalk_getidb(if_output);
	
	if(atalkidb_o && atalkidb_o->atalk_dminfo) {
	    domain_net = atdomain_FindBackwardRemap(atalkidb_o->atalk_dminfo->in,
						    net->dstrngstr);
	    if(domain_net)
		PUTSHORT(((uchar *)zip+ZIPTYPEBYTES), domain_net);
	}
    }


    atalk_DDPcksum(pak);
    if (atalk_send(pak)) {
	atalk_stat[ATALK_ZIP_OUT]++;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: sent ZIP query for net %#A to %a",
		   atalk_CableRange(net), dhost);
    }
}

/*
 * atzip_NeedsGC
 * Indicates that a GC would be productive.
 */
void
atzip_NeedsGC (void)
{
    if (zip_gcNeeded == 0)
	atmaint_ScheduleGCRun(zip_gccontext);
    
    zip_gcNeeded++;
}

/* $Id: ntp_peer.c,v 3.2.62.3 1996/08/19 18:55:04 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_peer.c,v $
 *------------------------------------------------------------------
 * NTP peer management
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_peer.c,v $
 * Revision 3.2.62.3  1996/08/19  18:55:04  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.2  1996/04/16  19:03:18  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:12:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:06:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:49:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:51:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:28:45  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:45:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) David L. Mills 1992, 1993, 1994
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name University of Delaware not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The University of Delaware
 * makes no representations about the suitability this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Authors
 *
 * Dennis Ferguson <dennis@mrbill.canet.ca> (foundation code for NTP
 *      Version 2 as specified in RFC-1119)
 * Lars H. Mathiesen <thorinn@diku.dk> (adaptation of foundation code for
 *      Version 3 as specified in RFC-1305)
 * Louis A. Mamakos <louie@ni.umd.edu> (support for md5-based 
 *      authentication)
 * Craig Leres <leres@ee.lbl.gov> (port to 4.4BSD operating system,
 *	ppsclock, Maganavox GPS clock driver)
 * Nick Sayer <mrapple@quack.kfu.com> (SunOS streams modules)
 * Frank Kardel <Frank.Kardel@informatik.uni-erlangen.de>
 *      (PARSE (GENERIC) driver, STREAMS module for PARSE, support scripts,
 *       reference clock configuration scripts, Makefile cleanup)
 * Rainer Pruy <Rainer.Pruy@informatik.uni-erlangen.de> (monitoring/trap
 *      scripts, statistics file handling)
 * Glenn Hollinger <glenn@herald.usask.ca> (GOES clock driver)
 * Kenneth Stone <ken@sdd.hp.com> (port to HPUX operating system)
 * Dave Katz <dkatz@cisco.com> (port to RS/6000 AIX operating system)
 * William L. Jones <jones@hermes.chpc.utexas.edu> (RS/6000 AIX
 *	modifications, HPUX modifications)
 * John A. Dundas III <dundas@salt.jpl.nasa.gov> (Apple A/UX port)
 * David L. Mills <mills@udel.edu> (Spectractom WWVB, Austron GPS,
 *	Heath, ATOM, ACTS, KSI/Odetics IRIG-B clock drivers; pps support)
 * Jeffrey Mogul <mogul@pa.dec.com> (ntptrace utility)
 * Steve Clift (clift@ml.csiro.au) OMEGA clock driver)
 * Mike Iglesias (iglesias@uci.edu) (DEC Alpha changes)
 * Mark Andrews <marka@syd.dms.csiro.au> (Leitch atomic clock controller)
 * George Lindholm <lindholm@ucs.ubc.ca> (port to SunOS 5.1 operating system)
 * Jeff Johnson <jbj@chatham.usdesign.com> (massive prototyping overhaul)
 * Tom Moore <tmoore@fievel.daytonoh.ncr.com> (port to i386 svr4)
 * Piete Brooks <Piete.Brooks@cl.cam.ac.uk> (MSF clock driver, Trimble PARSE
 * 	support)
 * Karl Berry <karl@owl.HQ.ileaf.com> (syslog to file option)
 * Torsten Duwe <duwe@immd4.informatik.uni-erlangen.de> (Linux Port)
 * Paul A Vixie <vixie@vix.com> (TrueTime GPS driver)
 * Jim Jagielski <jim@jagubox.gsfc.nasa.gov> (A/UX port)
 * Ray Schnitzler <schnitz@unipress.com> (First pass at a Unixware1 port.)
 * Ajit Thyagarajan <ajit@ee.udel.edu> (IP multicast support)
 * Jeff Steinman <jss@pebbles.jpl.nasa.gov> (Datum PTS clock driver)
 * Tomoaki TSURUOKA <tsuruoka@nc.fukuoka-u.ac.jp> (TRAK clock driver)
 */

/*
 * ntp_peer.c - management of data maintained for peer associations
 */
#define CISCO

#ifdef CISCO
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "config.h"
#else
#include <stdio.h>
#include <sys/types.h>

#endif /* CISCO */

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_refclock.h"
#include "ntp_registry_private.h"

/*
 * These routines manage the allocation of memory to peer structures
 * and the maintenance of the peer hash table.  The two main entry
 * points are findpeer(), which looks for corresponding peer data
 * in the peer list, newpeer(), which allocates a new peer structure
 * and adds it to the list, and unpeer(), which demobilizes the association
 * and deallocates the structure.
 */

/*
 * The peer hash table (imported by the protocol module).
 */
struct peer **peer_hash;
int *peer_hash_count;			/* count of peers in each bucket */

/*
 * The association ID hash table.  Used for lookups by association ID
 */
struct peer **assoc_hash;
static int *assoc_hash_count;

/*
 * The free list.  Clean structures only, please.
 */
static struct peer *peer_free;
int peer_free_count;

/*
 * Association ID.  We initialize this value randomly, the assign a new
 * value every time the peer structure is incremented.
 */
static u_short current_association_ID;

/*
 * Memory allocation watermarks.
 */
#define	INC_PEER_ALLOC		5	/* when we run out, add 5 more */

/*
 * Miscellaneous statistic counters which may be queried.
 */
u_long peer_timereset;		/* time stat counters were zeroed */
u_long findpeer_calls;		/* number of calls to findpeer */
u_long peer_allocations;	/* number of allocations from the free list */
u_long peer_demobilizations;	/* number of structs freed to free list */
int total_peer_structs;		/* number of peer structs in circulation */

/*
 * default interface.  Imported from the io module.
 */
extern struct interface *any_interface;

/*
 * Initialization data.  When configuring peers at initialization time,
 * we try to get their poll update timers initialized to different values
 * to prevent us from sending big clumps of data all at once.
 */
u_long init_peer_starttime;
extern int initializing;
extern int debug;

static	void	getmorepeermem	P((void));

/*
 * Limiter for peer creation rate
 */
static sys_timestamp ntp_lastpeer_create;
static ulong ntp_peercount;
ulong ntp_max_peercount;

/*
 * ntp_flush_srcaddrs
 *
 * Something happened that may have invalidated our cached source
 * addresses (an address change, or an interface state change).  We are
 * brute-force about it--simply go through and wipe out all of the
 * source addresses in each of the peers.  The next time we receive a
 * packet from the peer the field will be filled in again properly.
 * We may send out a packet or two with an incorrect source address (say,
 * the primary rather than the proper secondary) but this will correct
 * itself straight away.
 *
 * We wipe out the source interface as well (for those peers that did not
 * specify a source interface).
 */
void ntp_flush_srcaddrs (void)
{
    struct peer *peer;
    int hash;

    if (!peer_hash)
	return;

    for (hash = 0; hash < HASH_SIZE; hash++) {
	for (peer = peer_hash[hash]; peer != 0; peer = peer->next) {
	    peer->ip_destaddr = IPADDR_ZERO; /* It's *our* source address */
	    if (!(peer->flags & FLAG_SRCADR)) {	/* Zap source i/f as well. */
		peer->dstadr = any_interface;
	    }
	}
    }
}

/*
 * ntp_address_change
 *
 * An address change has occurred on one of the interfaces.  Clean
 * up the cached source addresses.
 */
void ntp_address_change (idbtype *idb, ipaddrtype addr, ipaddrtype mask,
			 boolean secondary, boolean add)
{
    ntp_flush_srcaddrs();
}

/*
 * ntp_int_statechange
 *
 * An interface has changed state.  Clean up the cached source addresses.
 */
void ntp_int_statechange (idbtype *idb)
{
    ntp_flush_srcaddrs();
}

/*
 * ntp_newpeer_ok
 *
 * Returns TRUE if OK to create a new peer association (time constraints and
 * total peer count OK).
 *
 * Note that the time limiter will have a false negative if no peers are
 * created for 49 days, but only for the limit period.
 */
boolean ntp_newpeer_ok (void)
{
    /* Not OK if we haven't passed enough time since the last one. */

    if (TIMER_RUNNING(ntp_lastpeer_create) && 
	(ELAPSED_TIME(ntp_lastpeer_create) < NTP_PEERCREATE_IVL)) {
	return(FALSE);
    }

    /* Not OK if we've reached the max number of peers. */

    if (ntp_peercount >= ntp_max_peercount)
	return(FALSE);

    /* OK to do it.  Grab the time to limit the next one. */

    GET_TIMESTAMP(ntp_lastpeer_create);
    return(TRUE);
}

/*
 * ntp_init_peer - initialize peer data structures and counters
 *
 * N.B. We use the random number routine in here.  It had better be
 *      initialized prior to getting here.
 */

boolean ntp_init_peer()
{
	register int i;

	/*
	 * Clear hash table and counters.
	 */
	peer_hash = malloc(HASH_SIZE * sizeof(struct peer *));
	if (!peer_hash)
	    return(FALSE);
	peer_hash_count = malloc(HASH_SIZE * sizeof(int *));
	if (!peer_hash_count)
	    return(FALSE);
	assoc_hash = malloc(HASH_SIZE * sizeof(struct peer *));
	if (!assoc_hash)
	    return(FALSE);
	assoc_hash_count = malloc(HASH_SIZE * sizeof(int *));
	if (!assoc_hash_count)
	    return(FALSE);
	for (i = 0; i < HASH_SIZE; i++) {
		peer_hash[i] = NULL;
		peer_hash_count[i] = 0;
		assoc_hash[i] = NULL;
		assoc_hash_count[i] = 0;
	}

	/*
	 * Clear stat counters
	 */
	findpeer_calls = peer_allocations = 0;
	peer_demobilizations = 0;

	/*
	 * Initialization counter.
	 */
	init_peer_starttime = 0;

	/*
	 * Initialize peer memory.
	 */
	peer_free = NULL;
	peer_free_count = 0;
	total_peer_structs = 0;
 	ntp_peercount = 0;
 	ntp_max_peercount = NTP_DEF_MAX_PEERCOUNT;

	/*
	 * Initialize our first association ID
	 */
	current_association_ID = (u_short)ranp2(16);
	if (current_association_ID == 0)
		current_association_ID = 1;
	return(TRUE);
}



/*
 * getmorepeermem - add more peer structures to the free list
 */
static void
getmorepeermem()
{
	register int i;
	register struct peer *peer;

	peer = (struct peer *)emalloc(INC_PEER_ALLOC*sizeof(struct peer));
#ifdef CISCO
	if (!peer)
	    return;
#endif /* CISCO */
	for (i = 0; i < INC_PEER_ALLOC; i++) {
		peer->next = peer_free;
		peer_free = peer;
		peer++;
	}

	total_peer_structs += INC_PEER_ALLOC;
	peer_free_count += INC_PEER_ALLOC;
}


/*
 * findexistingpeer - return a pointer to a peer in the hash table
 */
struct peer *
findexistingpeer(addr, start_peer)
	struct sockaddr_in *addr;
	struct peer *start_peer;
{
	register struct peer *peer;

	/*
	 * start_peer is included so we can locate instances of the
	 * same peer through different interfaces in the hash table.
	 */
	if (start_peer == 0)
		peer = peer_hash[HASH_ADDR(addr)];
	else
		peer = start_peer->next;
	
	while (peer != 0) {
		if (NSRCADR(addr) == NSRCADR(&peer->srcadr)
		    && NSRCPORT(addr) == NSRCPORT(&peer->srcadr))
			return peer;
		peer = peer->next;
	}

	return (struct peer *)0;
}


/*
 * findpeer - find and return a peer in the hash table.
 *
 * If "bcast" is TRUE, we return a broadcast peer that matches the
 * specified interface.
 */
struct peer *
#ifdef CISCO
findpeer(struct sockaddr_in *srcadr, struct interface *dstadr, boolean bcast)
#else
findpeer(struct sockaddr_in *srcadr, struct interface *dstadr, int fd)
#endif /* CISCO */
{
	register struct peer *any_inter_peer;
	register struct peer *peer;
	register struct peer *best = (struct peer *) 0;
	int hash;

	findpeer_calls++;

	any_inter_peer = 0;

	/* If broadcast, walk the entire peer tree looking for it. */
	
	if (bcast) {
	    for (hash = 0; hash < HASH_SIZE; hash++) {
		for (peer = peer_hash[hash]; peer != 0; peer = peer->next) {
		    if (peer->dstadr == dstadr && 
			peer->hmode == MODE_BROADCAST) {
			return peer;	/* got it! */
		    }
		}
	    }
	    return (NULL);
	}

	hash = HASH_ADDR(srcadr);
	for (peer = peer_hash[hash]; peer != 0; peer = peer->next) {
		if (NSRCADR(srcadr) == NSRCADR(&peer->srcadr)
		    && NSRCPORT(srcadr) == NSRCPORT(&peer->srcadr)) {
			if (peer->dstadr == dstadr) {
#ifdef CISCO
			    return peer; /* got it! */
#else
				int rfd = (peer->cast_flags & MDF_BCAST) ?
				    dstadr->bfd : dstadr->fd;

				if (rfd == fd)
					return peer;	/* got it! */
				best = peer;
#endif /* CISCO */
			}
			if (!(peer->flags & FLAG_SRCADR)) { /* Wild card */
				/*
				 * We shouldn't have more than one
				 * instance of the peer in the table,
				 * but I don't trust this.  Save this
				 * one for later and continue search.
				 */
				if (any_inter_peer == 0)
					any_inter_peer = peer;
#ifndef CISCO
				else
					syslog(LOG_ERR,
		"two instances of default interface for %s in hash table",
					    ntoa(srcadr));
#endif /* CISCO */
			}

			/* 
			 * Multicast hacks to determine peer when a 
			 * packet arrives and there exists an assoc.
			 * with src in client/server mode
			 */
 			if (((dstadr == any_interface) || (peer->cast_flags &
			     MDF_MCAST)) && peer->flags & FLAG_MCAST2)
				return peer;
		}
	}

 	if(best) {
 		return best;
 	}

	/*
	 * If we didn't find the specific peer but found a wild card,
	 * modify the interface and return him.  Only modify it if there
	 * is no global source interface.
	 */
	if (any_inter_peer != 0) {
	    if (!ntp_global_source_if)
		any_inter_peer->dstadr = dstadr;
	    return any_inter_peer;
	}

	/*
	 * Out of luck.  Return 0.
	 */
	return (struct peer *)0;
}

/*
 * findpeerbyassocid - find and return a peer using his association ID
 */
struct peer *
findpeerbyassoc(assoc)
	int assoc;
{
	register struct peer *peer;
	int hash;

	hash = assoc & HASH_MASK;
	for (peer = assoc_hash[hash]; peer != 0; peer = peer->ass_next) {
		if ((u_short)assoc == peer->associd)
			return peer;	/* got it! */
	}

	/*
	 * Out of luck.  Return 0.
	 */
	return (struct peer *)0;
}

/*
 * unpeer - remove peer structure from hash table and free structure
 */
void
unpeer(peer_to_remove)
	struct peer *peer_to_remove;
{
	int hash;

	hash = HASH_ADDR(&peer_to_remove->srcadr);
	peer_hash_count[hash]--;
	peer_demobilizations++;

#ifdef REFCLOCK
	/*
	 * If this peer is actually a clock, shut it down first
	 */
	if (peer_to_remove->flags & FLAG_REFCLOCK)
	    reg_invoke_ntp_refclock_unpeer(peer_to_remove);
#endif

	if (peer_hash[hash] == peer_to_remove)
		peer_hash[hash] = peer_to_remove->next;
	else {
		register struct peer *peer;

		peer = peer_hash[hash];
		while (peer != 0 && peer->next != peer_to_remove)
			peer = peer->next;
		
		if (peer == 0) {
			peer_hash_count[hash]++;
#ifndef CISCO
			syslog(LOG_ERR, "peer struct for %s not in table!",
			    ntoa(&peer->srcadr));
#endif /* CISCO */
		} else {
			peer->next = peer_to_remove->next;
		}
	}

	/*
	 * Remove him from the association hash as well.
	 */
	hash = peer_to_remove->associd & HASH_MASK;
	assoc_hash_count[hash]--;
	if (assoc_hash[hash] == peer_to_remove)
		assoc_hash[hash] = peer_to_remove->ass_next;
	else {
		register struct peer *peer;

		peer = assoc_hash[hash];
		while (peer != 0 && peer->ass_next != peer_to_remove)
			peer = peer->ass_next;
		
		if (peer == 0) {
			assoc_hash_count[hash]++;
#ifndef CISCO
			syslog(LOG_ERR,
			    "peer struct for %s not in association table!",
			    ntoa(&peer->srcadr));
#endif /* CISCO */
		} else {
			peer->ass_next = peer_to_remove->ass_next;
		}
	}

	TIMER_DEQUEUE(&peer_to_remove->event_timer);

	peer_to_remove->next = peer_free;
	peer_free = peer_to_remove;
	peer_free_count++;
	if (ntp_peercount > 0)
	    ntp_peercount--;

	/* If we're synced to this guy, bail out. */

	if (sys_peer == peer_to_remove) {
	    ntp_unsync();
	}
}


/*
 * peer_config - configure a new peer
 */
struct peer *
peer_config(srcadr, dstadr, hmode, version, minpoll, maxpoll, flags, ttl, key,
	    stratum)
	struct sockaddr_in *srcadr;
	struct interface *dstadr;
	int hmode;
	int version;
	int minpoll;
	int maxpoll;
	int flags;
	int ttl;
	u_long key;
	int stratum;
{
	register struct peer *peer;

#ifdef DEBUG
	if (debug)
		printf("peer_config: addr %s mode %d version %d minpoll %d maxpoll %d flags %d ttl %d key %lu\n",
		    ntoa(srcadr), hmode, version, minpoll, maxpoll, flags,
		    ttl, key);
#endif
	/*
	 * See if we have this guy in the tables already.  If
	 * so just mark him configured.
	 */
	peer = findexistingpeer(srcadr, (struct peer *)0);
	if (dstadr != 0) {
		while (peer != 0) {
			if (peer->dstadr == dstadr)
				break;
			peer = findexistingpeer(srcadr, peer);
		}
	}

	/*
	 * Torque the flags to make sure they're valid
	 */
	flags &= (FLAG_AUTHENABLE|FLAG_PREFER|FLAG_SRCADR|FLAG_BCASTDSTADR);

	/*
	 * If we found one, just change his mode and mark him configured.
	 */
	if (peer != 0) {
		peer->hmode = (u_char)hmode;
		peer->version = (u_char)version;
		peer->minpoll = (u_char)minpoll;
		peer->maxpoll = (u_char)maxpoll;
		peer->hpoll = peer->minpoll;
		peer->ppoll = peer->minpoll;
		peer->flags = (flags | FLAG_CONFIG) |
		    (peer->flags & FLAG_REFCLOCK);
		peer->cast_flags = (hmode == MODE_BROADCAST) ?
			IN_CLASSD(ntohl(srcadr->sin_addr.s_addr)) ? MDF_MCAST : MDF_BCAST : MDF_UCAST;
		peer->ttl = (u_char)ttl;
		peer->keyid = key;
		peer->stratum = stratum;
		return peer;
	}

	/*
	 * If we're here this guy is unknown to us.  Make a new peer
	 * structure for him.
	 */
	peer = newpeer(srcadr, dstadr, hmode, version, minpoll, maxpoll,
	    ttl, key, stratum);
	if (peer != 0) {
		peer->flags |= (flags|FLAG_CONFIG);

		/* If not broadcast, accelerate the first few polls. */

		if (hmode == MODE_CLIENT || hmode == MODE_ACTIVE)
		    peer->startup_polls = STARTUP_POLL_COUNT - 1;
	}
	return peer;
}


/*
 * newpeer - initialize a new peer association
 */
struct peer *
newpeer(srcadr, dstadr, hmode, version, minpoll, maxpoll, ttl, key, stratum)
	struct sockaddr_in *srcadr;
	struct interface *dstadr;
	int hmode;
	int version;
	int minpoll;
	int maxpoll;
	int ttl;
	u_long key;
        int stratum;
{
	register struct peer *peer;
	struct peer *curpeer;
	register int i;

	/*
	 * Some dirt here.  Some of the initialization requires
	 * knowlege of our system state.
	 */
	extern s_fp sys_bdelay;
	
	if (peer_free_count == 0)
		getmorepeermem();
#ifdef CISCO
	if (peer_free_count == 0) /* No more memory! */
	    return(NULL);
#endif /* CISCO */

	peer = peer_free;
	peer_free = peer->next;
	peer_free_count--;
	ntp_peercount++;

	/*
	 * Initialize the structure.  This stuff is sort of part of
	 * the receive procedure and part of the clear procedure rolled
	 * into one.
	 *
	 * Zero the whole thing for now.  We might be pickier later.
	 */
	memset(peer, 0, sizeof(struct peer));

	peer->srcadr = *srcadr;
	if (dstadr != 0)
		peer->dstadr = dstadr;
	else if (hmode == MODE_BROADCAST)
		peer->dstadr = findbcastinter(srcadr);
	else
		peer->dstadr = any_interface;
	peer->cast_flags = (hmode == MODE_BROADCAST) ?
		(IN_CLASSD(ntohl(srcadr->sin_addr.s_addr))) ? MDF_MCAST : MDF_BCAST :
		(hmode == MODE_BCLIENT || hmode == MODE_MCLIENT) ?
		(peer->dstadr->flags & INT_MULTICAST) ? MDF_MCAST : MDF_BCAST :
		MDF_UCAST;
	peer->hmode = (u_char)hmode;
	peer->version = (u_char)version;
	peer->minpoll = (u_char)minpoll;
	peer->maxpoll = (u_char)maxpoll;
	peer->hpoll = peer->minpoll;
	peer->ppoll = peer->minpoll;
	peer->ttl = ttl;
	peer->keyid = key;
	peer->estbdelay = sys_bdelay;
	peer->leap = LEAP_NOTINSYNC;
	peer->precision = DEFPRECISION;
	peer->dispersion = NTP_MAXDISPERSE;
	peer->stratum = stratum;
	peer->update = sys_clock;

	for (i = 0; i < NTP_SHIFT; i++) {
		peer->filter_order[i] = i;
		peer->filter_error[i] = NTP_MAXDISPERSE;
	}

	/*
	 * Assign him an association ID and increment the system variable
	 */
	peer->associd = current_association_ID;
	if (++current_association_ID == 0)
		++current_association_ID;

	/*
	 * Note time on statistics timers.
	 */
	peer->timereset = current_time;
	peer->timereachable = current_time;
	peer->timereceived = current_time;

#ifdef REFCLOCK
	if (ISREFCLOCKADR(&peer->srcadr)) {
		/*
		 * We let the reference clock support do clock
		 * dependent initialization.  This includes setting
		 * the peer timer, since the clock may have requirements
		 * for this.
		 */
		if (!reg_invoke_ntp_refclock_newpeer(peer)) {
			/*
			 * Dump it, something screwed up
			 */
			peer->next = peer_free;
			peer_free = peer;
			peer_free_count++;
			if (ntp_peercount > 0)
			    ntp_peercount--;
			return 0;
		}
	} else {
#endif
	/*
	 * Set up timer.  If initializing, just make sure we start polling
	 * in different 4 second intervals.
	 */
	peer->event_timer.peer = peer;
	peer->event_timer.event_handler = transmit;
#ifdef CISCO
	peer->event_timer.event_time = current_time + ranp2(NTP_MINPOLL) + 1;
#else
	if (initializing) {
		init_peer_starttime += (1 << EVENT_TIMEOUT);
		if (init_peer_starttime >= (1 << peer->minpoll))
			init_peer_starttime = (1 << EVENT_TIMEOUT);
		peer->event_timer.event_time = init_peer_starttime;
	} else {
		/*
		 * First expiry is set to eight seconds from now.
		 */
		peer->event_timer.event_time
		    = (1 << (peer->minpoll - 1)) + current_time;
	}
#endif /* CISCO */
	TIMER_ENQUEUE(timerqueue, &peer->event_timer);
#ifdef REFCLOCK
	}
#endif

	/*
	 * Put him in the hash tables.  Add to the end so that NVGENs
	 * stay consistent.
	 */
	i = HASH_ADDR(&peer->srcadr);
	curpeer = peer_hash[i];
	if (curpeer != NULL) {		/* Something's already in this list */
	    while (curpeer->next != NULL) {
		curpeer = curpeer->next;
	    }
	    curpeer->next = peer;
	} else {			/* Nothing on the list */
	    peer_hash[i] = peer;
	}
	peer->next = NULL;
	peer_hash_count[i]++;

	i = peer->associd & HASH_MASK;
	peer->ass_next = assoc_hash[i];
	assoc_hash[i] = peer;
	assoc_hash_count[i]++;

	return peer;
}


/*
 * peer_unconfig - remove the configuration bit from a peer
 *
 * If "bcast" is TRUE, any peer with hmode = BROADCAST that matches
 * the specified interface is removed.
 */
int
peer_unconfig(struct sockaddr_in *srcadr, struct interface *dstadr,
	      boolean bcast)
{
	register struct peer *peer;
	int num_found;
	int hash;

	num_found = 0;

    if (bcast) {
	/*
	 * Broadcast.  Search for any peer that matches the
	 * interface, since we can't rely on the address
	 * (the broadcast address moves around...)
	 */
	if (dstadr != NULL) {
	    for (hash = 0; hash < HASH_SIZE; hash++) {
	      start_over:
		for (peer = peer_hash[hash]; peer != 0; peer = peer->next) {
		    if ((peer->flags & FLAG_CONFIG) &&
			peer->hmode == MODE_BROADCAST && 
			peer->dstadr == dstadr) {
			unpeer(peer);
			num_found++;
			goto start_over;
		    }
		}
	    }
	}
    } else {
	peer = findexistingpeer(srcadr, (struct peer *)0);
	while (peer != 0) {
		if (peer->flags & FLAG_CONFIG
		    && (dstadr == 0 || peer->dstadr == dstadr)) {
			num_found++;
			/*
			 * Tricky stuff here.  If the peer is polling us
			 * in active mode, turn off the configuration bit
			 * and make the mode passive.  This allows us to
			 * avoid dumping a lot of history for peers we
			 * might choose to keep track of in passive mode.
			 * The protocol will eventually terminate undesirables
			 * on its own.
			 */
			if (peer->hmode == MODE_ACTIVE
			    && peer->pmode == MODE_ACTIVE) {
				peer->hmode = MODE_PASSIVE;
				peer->flags &= ~FLAG_CONFIG;
			} else {
				unpeer(peer);
				peer = 0;
			}
		}
		peer = findexistingpeer(srcadr, peer);
	}
    }
	return num_found;
}


/*
 * peer_clr_stats - clear peer module stat counters
 */
void
peer_clr_stats()
{
	findpeer_calls = 0;
	peer_allocations = 0;
	peer_demobilizations = 0;
	peer_timereset = current_time;
}

/*
 * peer_reset - reset stat counters in a peer structure
 */
void
peer_reset(peer)
	struct peer *peer;
{
	if (peer == 0)
		return;
	peer->sent = 0;
	peer->received = 0;
	peer->processed = 0;
	peer->badauth = 0;
	peer->bogusorg = 0;
	peer->oldpkt = 0;
	peer->seldisptoolarge = 0;
	peer->selbroken = 0;
	peer->seltooold = 0;
	peer->timereset = current_time;
}


/*
 * peer_all_reset - reset all peer stat counters
 */
void
peer_all_reset()
{
	struct peer *peer;
	int hash;

	for (hash = 0; hash < HASH_SIZE; hash++)
		for (peer = peer_hash[hash]; peer != 0; peer = peer->next)
			peer_reset(peer);
}

#ifdef CISCO
/*
 * Generate NVGEN commands for peers.
 */
void ntp_peer_nvgen(parseinfo *csb)
{
    struct peer *peer;
    int hash;
    char *peertype;

    if (!ntp_running)
	return;

    for (hash = 0; hash < HASH_SIZE; hash++) {
	for (peer = peer_hash[hash]; peer != 0; peer = peer->next) {
	    if (!(peer->flags & FLAG_CONFIG))
		continue;
	    if (peer->flags & FLAG_REFCLOCK)
		continue;
	    switch (peer->hmode) {
	    case MODE_ACTIVE:
		peertype = "peer";
		break;
	    case MODE_CLIENT:
		peertype = "server";
		break;
	    case MODE_BROADCAST:
		continue;
	    default:
		peertype = "unknown";
		break;
	    }
	    nv_write(TRUE, "%s %s %s", csb->nv_command, peertype,
		     ntoa(&peer->srcadr));
	    nv_add((peer->version != NTP_VERSION), " version %d", 
		   peer->version);
	    nv_add((peer->flags & FLAG_AUTHENABLE), " key %d", peer->keyid);
	    if (peer->dstadr && peer->dstadr->idb)
		nv_add((peer->flags & FLAG_SRCADR), " source %s",
		       peer->dstadr->idb->namestring);
	    nv_add((peer->flags & FLAG_PREFER), " prefer");
	}
    }
}
#endif /* CISCO */

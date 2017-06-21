/* $Id: arp.c,v 3.6.6.11 1996/09/10 01:06:44 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/arp.c,v $
 *------------------------------------------------------------------
 * arp.c -- Protocol Independent ARP Support
 *
 * 16-July-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arp.c,v $
 * Revision 3.6.6.11  1996/09/10  01:06:44  snyder
 * CSCdi68568:  more constant opportuniites
 *              140 from image, 52 from data
 * Branch: California_branch
 *
 * Revision 3.6.6.10  1996/08/28  12:47:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.9  1996/07/02  23:11:05  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.6.6.8  1996/06/28  23:06:49  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.6.7  1996/06/18  01:44:05  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.6.6.6  1996/05/22  20:54:48  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.6.6.5  1996/05/10  01:38:15  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/05/09  14:29:26  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.6.3.2.1  1996/04/27  07:03:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.6.3  1996/04/25  23:12:32  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/04/05  20:13:17  hampton
 * ARP should process more than one packet per invocation.  [CSCdi53707]
 * Branch: California_branch
 *
 * Revision 3.6.6.1.8.1  1996/04/08  01:54:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.6.1  1996/03/18  20:12:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.10.1  1996/03/22  09:36:59  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/03/01  20:20:40  rchandra
 * CSCdi50381:  IP: show ip arp command is not flexible
 * - make 'show ip arp [<address>] [<hostname>] [<mac>] [<interface>]' work
 *
 * Revision 3.5  1996/02/20  19:20:46  ahh
 * CSCdi44023:  memory leak in tty_background - arp_table_add
 * Always keep track of last entry in arp_table_lookup.
 *
 * Revision 3.4.38.1  1996/03/05  06:13:26  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.32.1  1996/02/27  21:08:02  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.8.1  1996/02/15  19:13:38  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.4  1995/11/30  22:57:44  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.3  1995/11/17  09:25:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:47:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:02:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/09  00:20:07  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.3  1995/08/08  16:47:36  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/08/01 00:19:19  tli
 * CSCdi37831:  IP: incomplete ARP entries display is confusing
 * Tell them it's incomplete rather than giving 'em a wierd address.
 *
 * Revision 2.1  1995/06/07  20:49:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "network.h"
#include "arp.h"
#include "arp_debug.h"
#include "address.h"
#include "logger.h"
#include "../wan/dialer.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

#include "media_registry.h"
/*
 * Storage
 */
arptype *arptable[NETHASHLEN];  /* table of arptype pointers */
static watched_queue *ARPpacketQ;	/* ARP input queue */
arp_traffic_t arp_traffic;

/*
 * Forward declarations
 */
static void arp_ager(boolean flushflag, idbtype *idb);


/*
 * arp_background
 * The process that receives ARP requests
 */

static process arp_background (void)
{
    paktype *pak;
    rfc_arp *arp;
    int count;
    sys_timestamp ageing_time;
    ulong major, minor;

    TIMER_START(ageing_time, ONEMIN);
    process_watch_timer(&ageing_time, ENABLE);
    process_watch_queue(ARPpacketQ, ENABLE, RECURRING);
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		TIMER_UPDATE(ageing_time, ONEMIN);
		arp_ager(FALSE, NULL);
		break;

	      case QUEUE_EVENT:
		/* Process only a limit number of packets per pass. */
		count = PAK_SWITCHCOUNT;
		while (count-- > 0) {
		    pak = process_dequeue(ARPpacketQ);
		    if (!pak)
			break;
		    /*
		     * Do some accounting.
		     */
		    idb_increment_rx_proto_counts(pak, ACCT_PROTO_ARP);

		    arp = (rfc_arp *)rfc_arp_headstart(pak);

		    /*
		     * Hand to protocol specific ARP code.
		     * IP, Apollo Domain, and Chaosnet use ARP.
		     * Default action is to give to bridging.
		     */
		    reg_invoke_arp_enq(arp->arpro, pak);
		}
		break;
	    
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * arp_enqueue
 * Enqueue an ARP packet from interrupt level
 */

static void arp_enqueue (paktype *pak)
{
    process_enqueue_pak(ARPpacketQ, pak);
}

/*
 * arp_table_add
 * Add an ARP entry to the hash table
 */

arptype *arp_table_add (idbtype *idb, long address, long linktype,
			hwaddrtype *hardware, int method, int flag)
{
    uint bucket;
    arptype *ptr;

    /*
     * See if we've heard of this guy before.  If we have, update his
     * hardware address and the reference time and return.
     */
    bucket = nethash(address);
    for (ptr = arptable[bucket]; ptr; ptr = ptr->next) {
	if ((ptr->type == linktype) && (ptr->address == address)) {
	    /*
	     * If an interface was specified, insist that it match
	     * that of the existing entry.  This allows us to have
	     * (for example) multiple interfaces with the same IP address,
	     * very useful in a bridge only configuration.
	     */
	    if (idb && ptr->idb && (idb != ptr->idb))
		continue;
	    ptr->hwlen = hardware->length;
	    ptr->hwtype = hardware->type;
	    bcopy(&hardware->addr, ptr->hardware, hardware->length);
	    ptr->idb = idb;
	    ptr->mode = (char) flag;            /* Set entry mode */
	    GET_TIMESTAMP(ptr->time);	 	/* update last reference */
	    ptr->flag = 0;			/* reset the flag field */
	    ptr->enctype = method;              /* Set encapsulation method */
	    return(ptr);			/* return now */
	}
    }

    /*
     * Create an entry if we've never heard of this guy before.
     */
    ptr = malloc(sizeof(arptype));
    if (!ptr)
	return(NULL);
    bcopy(&hardware->addr, ptr->hardware, hardware->length);
    ptr->idb  = idb;
    ptr->type = linktype;			/* set protocol type */
    ptr->mode = (char) flag;			/* set entry mode */
    ptr->flag = 0;				/* init flag field */
    ptr->enctype = method;			/* set encapsulation method */
    ptr->address = address;			/* set protocol address */
    GET_TIMESTAMP(ptr->time);			/* set reference time */
    ptr->next = arptable[bucket];		/* install new entry... */
    ptr->hwlen = hardware->length;		/* Set length in ARP table */
    ptr->hwtype = hardware->type;		/* Set the type of address */
    arptable[bucket] = ptr;			/* ...at head of chain */
    return(ptr);
}

/*
 * arp_table_remove
 * Remove a particular ARP entry.
 */

void arp_table_remove (long address, long linktype)
{
    uint bucket;
    arptype *ptr;
    arptype *pred;

    /*
     * See if address is in the table
     * Assume it may be in there more than once.
     */
    bucket = nethash(address);		/* determine hash bucket */
    ptr = arptable[bucket];		/* set pointer to hash chain */
    pred = (arptype *) &arptable[bucket];
    while (ptr) {
	if ((ptr->type == linktype) && (ptr->address == address)) {
	    pred->next = ptr->next;	/* link out current entry */
	    if (reg_invoke_arp_fscache_invalidate(ptr->type, ptr))
		reg_invoke_bump_cache_version(hwidb_or_null(ptr->idb),
					      NULL, 
					      FALSE);
	    free(ptr);			/* flush entry */
	    ptr = pred;		        /* adjust ptr to step to next */
	}
	pred = ptr;			/* remember predecessor */
	ptr = ptr->next;
    }
}

/*
 * arp_table_reload
 * Fixup the entries in the ARP table, presumably after a change
 * of hardware address on one or more Ethernet or Token Ring interfaces.
 */

void arp_table_reload (void)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb) {
	reg_invoke_arp_reload(idb);
    }
}

/*
 * arp_table_lookup
 * Probe the ARP table, return pointer or NULL
 * If we find an entry, move it to the head of the queue.
 */

arptype *arp_table_lookup (long linktype, long destination, idbtype *interface)
{
    int bucket;
    arptype *ptr, *pred;

    interface = get_netcfg_idb(interface);

    bucket = nethash(destination);
    pred = NULL;
    ptr = (arptype *) &arptable[bucket];
    while ((ptr = ptr->next)) {
	if ((ptr->address == destination) && (ptr->type == linktype)) {
	    /*
	     * If an interface was specified, make sure the interface
	     * matches that of the current table entry.
	     */
	    if (interface && ptr->idb && (interface != ptr->idb)) {
		pred = ptr;	/* Remember this entry for requeueing. */
		continue;
	    }
	    /*
	     * Move this entry to head of the list if it's
	     * not there already.
	     */
	    if (pred) {
		pred->next = ptr->next;
		ptr->next = arptable[bucket];
		arptable[bucket] = ptr;
	    }
	    return(ptr);
	}
	pred = ptr;	/* Remember this entry for requeueing. */
    }
    return(NULL);
}

/*
 * arp_hwaddr_lookup
 * Look for an ARP table entry by hardware address
 */
addrtype *arp_hwaddr_lookup (uchar *srcaddr, paktype *pak)
{
    int			i;
    arptype		*ptr, *pred;
    static addrtype	addrtype;
    uchar		*saddr;

    addrtype.type = ADDR_IP;
    addrtype.length = ADDRLEN_IP;

    saddr = (uchar *)srcaddr;

    for (i = 0; i < NETHASHLEN; i++) {
	pred = NULL;
	for (ptr = arptable[i] ; ptr ; pred = ptr , ptr = ptr->next) {
	    if (pak->if_input == ptr->idb &&
		bcmp(ptr->hardware, saddr, 6) == 0) {
		/*
		 * Move this entry to head of the list if it's
		 * not there already.
		 */
		if (pred) {
		    pred->next = ptr->next;
		    ptr->next = arptable[i];
		    arptable[i] = ptr;
		}
		addrtype.ip_addr = ptr->address;
		return(&addrtype);
	    }
	}
    }
    return(NULL);
}

/*
 * arp_send
 * Send an ARP request.  Datagram fields have already been set up.
 */

void arp_send (
    paktype *pak,
    int bytes,
    uchar *destination,
    long linktype,
    idbtype *interface,
    int method,
    long forlink)
{
    rfc_arp *rfc;

    rfc = (rfc_arp *)rfc_arp_headstart(pak);

    if (arp_debug) {
	ipaddrtype sipaddr, tipaddr;
	uchar *cp, *shardptr, *thardptr;
	ushort shard, thard;

	/*
	 * Check interface state to make sure we don't lie
	 * when debugging is on.
	 */
	if (!interface_up_simple(interface)) {
	    retbuffer(pak);
	    return;
	}

	/*
	 * Peel apart packet so we can print a useful debugging message.
	 * The opcode field is the last fixed field.  Everything else
	 * is calculated from there.  So far we have 2 and 6 byte hardware
	 * fields, so handle formatting appropriately.
	 */
	cp = rfc->ipshdw;
	if (rfc->arhln == 2) {
	    bcopy(cp, &shard, sizeof(ushort)); cp += sizeof(ushort);
	    bcopy(cp, &sipaddr, rfc->arpln); cp += rfc->arpln;
	    bcopy(cp, &thard, sizeof(ushort)); cp += sizeof(ushort);
	    bcopy(cp, &tipaddr, rfc->arpln);
	    buginf("\nIP %s: sent %s src %i %d,\n                 "
		   "dst %i %d %s",
		   linktype == LINK_ARP ? "ARP" : "RARP",
		   rfc->opcode == OP_REQ ? "req" : "rep",
		   sipaddr, shard,  tipaddr, thard,
		   pak->if_output == NULL ? interface->namestring :
		                            pak->if_output->namestring);
	} else {
	    shardptr = cp;  cp += rfc->arhln;
	    bcopy(cp, &sipaddr, rfc->arpln); cp += rfc->arpln;
	    thardptr = cp; cp += rfc->arhln;
	    bcopy(cp, &tipaddr, rfc->arpln);
	    buginf("\nIP %s: sent %s src %i %e,\n"
		   "                 dst %i %e %s",
		   linktype == LINK_ARP ? "ARP" : "RARP",
		   rfc->opcode == OP_REQ ? "req" : "rep",
		   sipaddr, shardptr,  tipaddr, thardptr,
		   interface->namestring);
	}
    }

    if ((pak->if_input == interface) &&
	(is_ddr(interface->hwptr)) &&
	(pak->dialer_input != NULL)) {
	pak->if_output = pak->dialer_input->firstsw;
    } else {
	pak->if_output = interface;
    }
    
    pak->linktype = linktype;
    pak->enctype = method;
    pak->acct_proto = ACCT_PROTO_ARP;
    
    /* Clear if_input residue for the arp response to pass the check
       in some media bridging code (e.g. atm) and to be sent out. */
    if (pak->if_input) {
        clear_if_input(pak);
    }

    if (pak_encap(interface->hwptr, pak, bytes, (long) destination))
	datagram_out(pak);
    else
	retbuffer(pak);
}

/*
 * arp_fscache_invalidate_default
 *
 * If we don't have a protocol-specific cache invalidation routine, then
 * make sure that we invalidate the entire cache if we have
 * protocols that can't just nuke part of their cache.  Ick.
 */
static boolean
arp_fscache_invalidate_default (arptype *ptr)
{
    return (TRUE);		/* cache needs nuclear bomb */
}

/*
 * arp_ager
 * Timeout old entries or flush ARP cache entries.
 */

static void arp_ager (boolean flushflag, idbtype *idb)
{
    int i;
    arptype *ptr, *pred;
    boolean bump_cache, delete;
    sys_timestamp expiration_time;
    ulong time_left;

    bump_cache = FALSE;
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (arptype *) &arptable[i]; /* assumes next is a zero offset */
	ptr = pred->next;		 /* really "ptr = arptable[i]" */
	while (ptr) {
	    if ((idb == NULL) || (idb == ptr->idb)) {
		delete = FALSE;
		switch (ptr->mode) {
		case ARP_DYNAMIC:
		    if (flushflag) {
			delete = TRUE;
		    } else if (ptr->idb->arp_timeout) {
			/*
			 * Get the time left for the entry before it expires.
			 * If timer already expired, delete the entry.
			 * If it is less than a minute to expire, 
			 * refresh the arp entry.
			 */
			expiration_time =
			    TIMER_ADD_DELTA64(ptr->time, 
					      ptr->idb->arp_timeout);
			time_left = TIME_LEFT_SLEEPING(expiration_time);
			if (time_left == 0) 
			    delete = TRUE;
			else if (time_left <= ONEMIN)
			    reg_invoke_arp_refresh(ptr->type, ptr);
		    }
		    break;
		case ARP_INTERFACE:
		    if (flushflag) {
			delete = TRUE;
		    }
		    break;
		case ARP_INCOMPLETE:
		    /*
		     * Note that deleting an incomplete entry need NOT
		     * change the cache.
		     */
		    if (flushflag ||
			CLOCK_OUTSIDE_INTERVAL(ptr->time,
					       ARP_THROTTLE_INTERVAL))
			delete = TRUE;
			break;
		case ARP_MOBILE:	/* aged elsewhere */
		case ARP_STATIC:
		case ARP_ALIAS:
		default:
		    break;
		}
		if (delete) {
		    pred->next = ptr->next; /* link out current entry */

		    if (ptr->mode != ARP_INCOMPLETE) {
			reg_invoke_arp_refresh(ptr->type, ptr);
			if (!flushflag)
			    bump_cache |=
			      reg_invoke_arp_fscache_invalidate(ptr->type, ptr);
		    }

		    free(ptr);		/* flush entry */
		    ptr = pred;		/* point to predecessor */
		}
	    }
	    pred = ptr;			/* remember predecessor */
	    ptr = ptr->next;		/* point to next entry, if any */
	}
 	/*
 	 * Don't be a CPU HOG. Share it.
 	 */
  	process_may_suspend();
    }

    /*
     * If we flushed all entries, be sure to reload the cache
     * with our current IEEE 802 interface addresses.  Also bump
     * the version number of any fast switching caches.
     */
    if (flushflag) {
	if (idb)
	    reg_invoke_arp_reload(idb);
	else
	    arp_table_reload();
	reg_invoke_bump_cache_version(hwidb_or_null(idb), NULL, FALSE);
    } else if (bump_cache)
	reg_invoke_bump_cache_version(NULL, NULL, FALSE);
}

/*
 * arp_flush_cache
 * Unconditionally flush the ARP cache.
 * Flush all entries if idb == NULL, otherwise
 * only entries associated with that idb.
 */

void arp_flush_cache (idbtype *idb)
{
    arp_ager(TRUE, idb);
}

/*
 * arp_clear_command
 * Perform the clear arp exec command
 */

void arp_clear_command (parseinfo *csb)
{
    reg_invoke_bump_cache_version(NULL, NULL, FALSE);
    reg_invoke_ar_cache_clear(NULL);    
}

/*
 * arp_show_traffic
 * Display ARP statistics
 */

void arp_show_traffic (void)
{
    printf("\n\nARP statistics:");
    printf("\n  Rcvd: %lu requests, %lu replies, %lu reverse, %lu other",
	   arp_traffic.reqrcv, arp_traffic.reprcv,
	   arp_traffic.revrcv, arp_traffic.otherrcv);
    printf("\n  Sent: %lu requests, %lu replies (%lu proxy), %lu reverse",
	   arp_traffic.reqsent, arp_traffic.repsent,
	   arp_traffic.proxyrep, arp_traffic.revsent);
}

/*
 * arp_display_one_entry
 * 
 * Display the guts of a single ARP entry.
 */

static const char arphead[] = {
"Protocol  Address          Age (min)  Hardware Addr   Type   Interface"
};

typedef int (*funccast)(arptype *);

static void arp_display_one_entry (arptype *ptr, void *func, boolean *onceflag)
{
    long time;
    int len;

    if (!*onceflag) {
	automore_header(arphead);
	*onceflag = TRUE;
    }

    /*
     * func prints out protocol name and address
     */
    len = func ? (*((funccast)func))(ptr) : 0;
    len = 27 - len;
    printf("%-*c", len, ' ');
    time = ELAPSED_TIME(ptr->time)/ONEMIN;
    if ((ptr->mode == ARP_DYNAMIC) ||
	(ptr->mode == ARP_MOBILE) ||
	(ptr->mode == ARP_INCOMPLETE))
	printf("%-9ld", time);
    else
	printf("        -");
    if (ptr->mode == ARP_INCOMPLETE) {
	printf("   Incomplete      %7s", encstring(ptr->enctype));
    } else {
	switch(ptr->hwtype) {
	    
	case STATION_ATMVC:
	    printf("    ");	/* Stationing code */
	    printf("VCD#%04d       %6s", GETSHORT(ptr->hardware), 
		   encstring(ptr->enctype));
	    break;
	    
	case STATION_SMDS64:
	    {
		char smdsaddr[20];
		
		sprintf(smdsaddr, "%E", ptr->hardware);
		if(strlen(smdsaddr) < 19) {
		    sprintf(smdsaddr, "%s    ", smdsaddr);
		}
		printf("%s %6s", smdsaddr, encstring(ptr->enctype));
	    }
	    break;
	default:
	    printf("   %e  %6s", ptr->hardware,
		   encstring(ptr->enctype));
	    break;
	}
	if (ptr->idb)
	    
	    printf(" %s", ptr->idb->namestring);
    }
}	
/*
 * arp_display
 * Print out contents of ARP hash table
 */

void arp_display (int typereq, void *func, idbtype *idb, long address, 
		  hwaddrtype *hwaddr)
{
    int i, j;
    arptype *ptr;
    boolean onceflag;
    
    onceflag = FALSE;
    automore_enable(NULL);
    
    if (address) {
	ptr = arp_table_lookup(typereq, address, NULL);
	if (ptr) {
	    mem_lock(ptr);
	    arp_display_one_entry(ptr, func, &onceflag);
	    free(ptr);
	}
	automore_disable();
	return;
    }

    for (i = 0; i < NETHASHLEN; i++) {
	for (j = 0; (ptr = queuepick(&arptable[i], j)); j++) {
	    if (ptr->type != typereq)
		continue;
	    if (idb && (idb !=ptr->idb))
		continue;
	    if (hwaddr && 
		(bcmp(hwaddr->addr, ptr->hardware, hwaddr->length) != 0))
		continue;
	    
	    mem_lock(ptr);
	    arp_display_one_entry(ptr, func, &onceflag);
	    free(ptr);
	}
    }
    automore_disable();
}

/*
 * arp_init
 * Initialize the generic ARP support
 */

static void arp_init (subsystype *subsys)
{
    /*
     * Clear the traffic array
     */
    memset(&arp_traffic, 0, sizeof(arp_traffic));

    /*
     * Set up debugging flags
     */
    arp_debug_init();

    /*
     * Initialize ARP parse chains
     */
    arp_parser_init();

    ARPpacketQ = create_watched_queue("ARP queue", 0, 0);
    process_create(arp_background, "ARP Input", NORMAL_STACK, PRIO_LOW);

    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_ARP, arp_enqueue, "arp_enqueue");
    reg_add_ar_cache_clear(arp_flush_cache, "arp_flush_cache");
    reg_add_default_arp_enq(net_bridge_enqueue, "net_bridge_enqueue");
    reg_add_default_arp_fscache_invalidate(arp_fscache_invalidate_default,
					   "arp_fscache_invalidate_default");
    reg_add_hwaddr_lookup(ET_ARPA, arp_hwaddr_lookup, "arp_hwaddr_lookup");
    reg_add_hwaddr_lookup(ET_SAP, arp_hwaddr_lookup, "arp_hwaddr_lookup");
    reg_add_hwaddr_lookup(ET_SNAP, arp_hwaddr_lookup, "arp_hwaddr_lookup");
}

/*
 * ARP subsystem header
 */

#define ARP_MAJVERSION 1
#define ARP_MINVERSION 0
#define ARP_EDITVERSION 1

SUBSYS_HEADER(arp, ARP_MAJVERSION, ARP_MINVERSION, ARP_EDITVERSION,
	      arp_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);

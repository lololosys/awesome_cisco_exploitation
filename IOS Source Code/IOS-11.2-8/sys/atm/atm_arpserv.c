/* $Id: atm_arpserv.c,v 3.8.20.16 1996/09/05 01:17:00 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_arpserv.c,v $
 *------------------------------------------------------------------
 * Server side for ATM ARP (RFC 1577)
 *
 * February 1995, Ronnie Kon
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_arpserv.c,v $
 * Revision 3.8.20.16  1996/09/05  01:17:00  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.8.20.15  1996/09/01  22:46:23  mdavison
 * CSCdi67350:  Getting error message: %UTIL-3-TREE: Data structure
 * error--received
 * Branch: California_branch
 * Test for existance of ARP server table before attempting to search it.
 *
 * Revision 3.8.20.14  1996/09/01  22:36:31  mdavison
 * CSCdi65842:  Issuing a sh atm arp-server crashes 7000
 * Branch: California_branch
 * Added semaphore protection to ARP server table.
 *
 * Revision 3.8.20.13  1996/08/16  02:13:12  lmercili
 * CSCdi65496:  rfc1577 failed ip routing
 * Branch: California_branch
 *
 * Revision 3.8.20.12  1996/07/30  17:20:37  dblair
 * CSCdi59050:  multilink broken when dialing Ascend with 2 phone numbers
 * Branch: California_branch
 * Convert htoa to tohexchar
 *
 * Revision 3.8.20.11  1996/07/10  22:19:57  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.8.20.10  1996/06/18  16:55:44  achopra
 * CSCdi60126:  ARP server builds duplicate table entries for same ATM
 * address
 * Branch: California_branch
 *
 * Revision 3.8.20.9  1996/06/05  21:11:07  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.8.20.8  1996/05/27  20:37:43  lmercili
 * CSCdi56075:  SVCs over multipoint signalling are torn down by 1577
 * Branch: California_branch
 *
 * Revision 3.8.20.7  1996/05/14  06:28:50  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.8.20.6  1996/05/01  14:44:58  achopra
 * CSCdi52007:  shut down an ATM ARP client on subinterface doesnt clear
 * map entry
 * Branch: California_branch
 *
 * Revision 3.8.20.5  1996/05/01  00:51:34  ronnie
 * CSCdi55875:  NULL pointer dereference causing alignment errors
 * Branch: California_branch
 *
 * Revision 3.8.20.4  1996/04/25  23:01:56  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.8.20.3  1996/04/09  19:05:59  ronnie
 * CSCdi50951:  Pb using the RS6000 as ATMARP server due to invalid nsap
 * in arp req
 * Branch: California_branch
 *
 * Revision 3.8.20.2  1996/04/08  22:48:28  ronnie
 * CSCdi50592:  ARP client does not register to arp-server with 2 ATM cards
 * Branch: California_branch
 *
 * Revision 3.8.20.1  1996/03/18  18:59:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  08:30:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:18:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/04  23:08:25  ronnie
 * CSCdi46955:  ATM ARP Server table out of sync with map list
 * (incorporate code review comments)
 *
 * Revision 3.7  1996/02/03  22:16:47  ronnie
 * CSCdi46955:  ATM ARP Server table out of sync with map list
 *
 * Revision 3.6  1996/01/31  02:13:01  snyder
 * CSCdi47864:  spellink errors
 *              inconsistancy -> inconsistency
 *
 * Revision 3.5  1996/01/24  22:56:11  ronnie
 * CSCdi45540:  ATM interface causes memory corruption on 7k
 *
 * Revision 3.4  1996/01/16  19:36:49  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Move problem registry services to new registry.
 *
 * Revision 3.3  1995/11/22  20:44:20  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:45:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:51:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/09/15  21:44:12  ronnie
 * CSCdi40277:  Crash in atm arp server code
 *
 * Revision 2.8  1995/09/14  21:07:04  ronnie
 * CSCdi40212:  Declaring a new ARP Server leaves old servers map lying
 * around
 *
 * Revision 2.7  1995/09/12  00:34:15  ronnie
 * CSCdi40108:  Input queue overflows on ARP Server interfaces
 *
 * Revision 2.6  1995/08/11  20:58:22  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.5  1995/07/19  23:49:37  ronnie
 * CSCdi36860:  show atm arp-server always requires an interface
 *
 * Revision 2.4  1995/07/05  18:34:30  ronnie
 * CSCdi36248:  ARP server could not resolve NSAP for client
 *
 * Revision 2.3  1995/06/22  17:03:05  ronnie
 * CSCdi36305:  ATM ARP Server NAK error message less than useful
 *
 * Revision 2.2  1995/06/09  16:55:01  ronnie
 * CSCdi33936:  Error changing from ARP client to server
 *
 * Revision 2.1  1995/06/07  20:11:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "stdarg.h"
#include "../util/wavl.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../if/atm_debug_flags.h"
#include "../if/arp.h"
#include "atm_arp.h"
#include "atmsig_api.h"
#include "atmsig_private.h"
#include "mgd_timers.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../ip/ip_registry.h"

/*
 * My internal database structure.  It is here instead of in a .h file
 * because nobody else has any business knowing about it.  We have to
 * search these three ways (based on IP address, ATM address, and Call ID)
 * so we thread onto 3 AVL trees.
 *
 * Note that the call to wavl_init() assumes that these are ordered as given.
 * Do not change the order without changing the wavl_init() as well.  (This
 * is why I have added the unnecessary explicit value assignments.)
 */
enum {
    AVL_VCD		= 0,
    AVL_IP_ADDR		= 1,
    AVL_ATM_ADDR	= 2,
    NUM_AVL_THREADS
};
typedef struct as_entry_ {
    wavl_node_type wavl[NUM_AVL_THREADS];	/* each node is threaded onto
						 * multiple trees.
						 */
    ipaddrtype	ipaddr;

    uchar	atm_addrlen;			/* atm address length */
    uchar	atm_addrtype;
    uchar	atm_addr[ATM_MAX_ADDR_LENGTH];	/* the ATM address */
    uchar	atm_subaddrlen;			/* atm subaddress length */
    uchar	atm_subaddrtype;
    uchar	atm_subaddr[ATM_MAX_SUBADDR_LENGTH];	/* ATM subaddress */

    mgd_timer	age_timer;
    enum {
	TIMER_NONE,
	TIMER_AGER,		/* our timer is for aging out this entry */
	TIMER_WAIT,		/* our timer is waiting for a InARP */
    } timer_type;
    short	retrans_count;	/* we retransmit InARPs */

    idbtype	*idb;
    vcd_t	vcd;
} as_entry;

#define NO_IP_ADDRESS		0

/*
 * We augment RFC1577 by retransmitting and aging InARP requests.  The
 * WAIT_TIME is the length of time we will wait for a response, the MAX_RETRANS
 * is the count of InARPs we will send before giving up.  (The first one
 * is the first retransmission).
 */
#define       INARP_WAIT_TIME    ONESEC  /* time to wait for an InARP Resp */
#define       INARP_MAX_RETRANS  3


/*
 * The max atm address string length--this is an ATM address written out in
 * ASCII, plus a byte for a '/' between the address and subaddress, and a
 * closing '\0'
 */
#define MAX_ATM_ADDR_STRLEN	(((ATM_MAX_ADDR_LENGTH + \
				   ATM_MAX_SUBADDR_LENGTH) * 2) + 2)

static boolean arp_serv_init = FALSE; /* have we init'ed ourselves even once */
static mgd_timer master_timer;

/*
 * We must also maintain a tree of pointers to interfaces acting as
 * ARP servers.  
 */
typedef struct as_int_ {
    wavl_node_type	wavl;
    idbtype		*idb;
} as_int;
static wavl_handle as_int_tree;

/* preserve strict typechecking */
static inline as_int *
wavl_to_as_int (wavl_node_type *wavl)
{
    return (as_int *)wavl;
}

/* the comparison routine for the as_int_tree */
static enum avl_compare
compare_int (avl_node_type *n1, avl_node_type *n2)
{
    as_int *node1, *node2;
    int name_comp;
    
    node1 = wavl_to_as_int(wavl_normalize(n1, 0));
    node2 = wavl_to_as_int(wavl_normalize(n2, 0));

    name_comp = strcmp(node1->idb->namestring, node2->idb->namestring);
    if (name_comp < 0)
	return(AVL_LT);
    if (name_comp > 0)
	return(AVL_GT);
    return(AVL_EQ);
}


/*
 * local inlines
 */

/*
 * idb_to_handle
 *
 * Convert an IDB into the corresponding wavl handle.  This is not hard, but
 * it happens frequently enough that it's probably worth making clear what
 * is happening.
 */
static inline wavl_handle *
idb_to_handle (idbtype *idb)
{
    if (!idb || !is_atm_arp(idb))
	return NULL;			/* This can never happen */
    
    return idb->atm_arp->as_data.server.handle;
}

/*
 *  Convert a wavl pointer to the as_entry which contains it.  Because
 *  these are guaranteed to be the same, we just do a cast.  This keeps
 *  the code rigorously typed.
 */
static inline as_entry *
wavl_to_entry (wavl_node_type *wavl)
{
    return((as_entry *)wavl);
}

/*
 * forward declarations
 */
static void sprint_atmaddr(char *, as_entry *);
static boolean show_one_arp_server(wavl_node_type *, boolean);
static boolean show_one_arp_server_va(wavl_node_type *, va_list);
static void process_arp_request(paktype *, vc_info_t *);
static void send_inarp(as_entry *);
static void send_arp_reply(paktype *, vc_info_t *, as_entry *);
static void send_nak(paktype *, vc_info_t *);
static void entry_destroy(as_entry *);
static as_entry *entry_create(idbtype *, vcd_t);
static void hangup(idbtype *, vcd_t, int);
static void update_entry(idbtype *, vc_info_t *, uchar, uchar, uchar *, uchar *, ipaddrtype);
static boolean show_entry(wavl_node_type *);
static boolean show_entry_va(wavl_node_type *, va_list);
static as_entry *locate_entry_by_ipaddr(wavl_handle *, ipaddrtype);
static as_entry *locate_entry_by_atmaddr(wavl_handle *, uchar, uchar *, uchar, uchar *);
static as_entry *locate_entry_by_vcd(wavl_handle *, vcd_t);
static enum avl_compare compare_ip_addr(avl_node_type *, avl_node_type *);
static enum avl_compare compare_atm_addr(avl_node_type *, avl_node_type *);
static enum avl_compare compare_vcd(avl_node_type *, avl_node_type *);
static void as_tree_insert(const wavl_handle *, as_entry *);
static void as_tree_delete(const wavl_handle *, as_entry *);
static void as_change_atmaddr_entry(const wavl_handle *, as_entry *, uchar, uchar *, uchar, uchar *);

/*
 * populate_hwaddr_from_entry
 *
 * fill in the passed hwaddr from the passed entry.  Return TRUE if this
 * could not be done.  (Because an ATM address can actually be bigger than
 * a hwaddrtype can hold due to subaddresses)
 */
static boolean
populate_hwaddr_from_entry (hwaddrtype *hwaddr, as_entry *entry)
{
    hwaddr->type = STATION_ATMNSAP;
    hwaddr->length = entry->atm_addrlen + entry->atm_subaddrlen;
    if (hwaddr->length > STATIONLEN_MAX) {
	/*
	 *  This little gem is here because a hwaddrtype at this writing was
	 *  big enough to hold an ATM Address and Subaddress.  So I want to
	 *  make sure not to overwrite more than I can.
	 */
	if (atm_errors_debug) {
	    char atm_addr_str[MAX_ATM_ADDR_STRLEN];

	    sprint_atmaddr(atm_addr_str, entry);
	    buginf("\nARPSERVER: ATM address for %i too big for hwaddrtype: %s",
		   entry->ipaddr, atm_addr_str);
	}
	return TRUE;
    }
    hwaddr->target_type = TARGET_UNICAST; /* pretty meaningless here, but this
					    is probably most correct. */
    /* copy the ATM address and subaddress */
    bcopy(entry->atm_addr, &hwaddr->addr[0], entry->atm_addrlen);
    bcopy(entry->atm_subaddr, &hwaddr->addr[entry->atm_addrlen],
	  entry->atm_subaddrlen);
    return FALSE;
}

/*
 * arpserv_lookup
 *
 * Before we do anything, let's make sure that we are willing to share our
 * information with our friends.  This routine will let outside routines
 * (notably NHRP) access our tables.  Passing a null idb means to return
 * a mapping from any of our tables.
 */
static hwaddrtype *
arpserv_lookup (idbtype *idb, ipaddrtype ipaddr)
{
    as_entry *entry;
    static hwaddrtype retval;

    if (idb == NULL) {
	/* check all our arp servers */
	as_int *arpserv;
    	
	if (!arp_serv_init) {
            return NULL;
	}

	for (entry = NULL,
		 arpserv = wavl_to_as_int(wavl_get_first(&as_int_tree, 0)) ;
	     arpserv != NULL ;
	     arpserv = wavl_to_as_int(wavl_get_next(&as_int_tree,
						    &arpserv->wavl, 0))) {
	    entry = locate_entry_by_ipaddr(idb_to_handle(arpserv->idb),ipaddr);
	    if (entry != NULL)
		break;
	}
    } else {
        wavl_handle *handle;

        if (NULL != (handle = idb_to_handle(idb))) {
            entry = locate_entry_by_ipaddr(idb_to_handle(idb), ipaddr);
        } else  {
            entry = NULL;
        }
    }

    if (entry == NULL) 
	return NULL;
    if (populate_hwaddr_from_entry(&retval, entry))
	return NULL;
    return &retval;
}

/*
 * atm_arp_serv_show
 *
 * Dump the contents of the ARP server.  This is called in response to a
 * show command
 */
void
atm_arp_serv_show (idbtype *idb)
{
    static const char msg[] = "\nNote that a '*' next to an IP address indicates an active call\n\n      IP Address\tTTL\tATM Address";

    if (!arp_serv_init) { 
        return;
    }

    /* if no IDB is passed, show all */
    if (idb == NULL) {
	/*
	 *  First question: are there any ARP Servers at all?
	 */
	if (!arp_serv_init ||
	    wavl_to_as_int(wavl_get_first(&as_int_tree, 0)) == NULL) {
	    printf("\nNo interfaces are functioning as ATM ARP Servers.");
	    return;
	}

	/*
	 *  OK then: display all of them.  We could do this efficiently, and
	 *  make use of the fact that we already have the first item.  But
	 *  CPU cycles are cheap.  We'll make the code look pretty instead.
	 */
	automore_enable(NULL);
	automore_header(msg);
	wavl_walk(&as_int_tree, 0, show_one_arp_server_va, TRUE);
	automore_disable();
    } else {
	as_int salt;

	if (!is_atm_arp_server(idb)) {
	    printf("\n%s is not functioning as an ATM ARP Server",
		   idb->namestring);
	    return;
	}
	salt.idb = idb;
	automore_enable(NULL);
	automore_header(msg);
	show_one_arp_server(&salt.wavl, FALSE);
	automore_disable();
    }
}

/*
 *  show_one_arp_server_va
 *
 *  The walker routine interface to show_one_arp_server
 */
static boolean
show_one_arp_server_va (wavl_node_type *w_node, va_list argptr)
{
    boolean show_idbname;

    show_idbname = va_arg(argptr, boolean);
    return(show_one_arp_server(w_node, show_idbname));
}

/*
 *  show_one_arp_server
 *
 *  Display the table for a single ARP server.
 */
static boolean
show_one_arp_server (wavl_node_type *w_node, boolean show_idbname)
{
    idbtype *idb;
    as_int *int_ptr;

    int_ptr = wavl_to_as_int(w_node);
    idb = int_ptr->idb;
    if (!is_atm_arp_server(idb))
	return(FALSE);
    if (show_idbname)
	printf("\n%s:", idb->namestring);

    wavl_walk(idb_to_handle(idb), AVL_IP_ADDR, show_entry_va);
    printf("\n");
    return(TRUE);
}

/*
 * show_entry_va
 *
 * Walker routine entry to show_entry.  Saves a cast.
 */
static boolean
show_entry_va (wavl_node_type *node, va_list varargs)
{
    return(show_entry(node));
}
 
/*
 * show_entry
 *
 * Displays a single entry in response to a show command.
 */
static boolean
show_entry (wavl_node_type *node)
{
    as_entry *entry;
    int left;
    char show_line[MAX_ATM_ADDR_STRLEN + 50];		/* long enough */
    
    entry = wavl_to_entry(node);
    
    if (entry->ipaddr == NO_IP_ADDRESS)
	return(TRUE);
    
    /*
     * Calculate the time left before aging.  We divide by 1000 to convert
     * from milliseconds to seconds, and divide and mod by 60 to convert
     * from seconds to minutes.  As these values are unlikely to change
     * in the future, magic numbers should be safe.
     */
    left = mgd_timer_left_sleeping(&entry->age_timer)/ 1000; /* seconds left */
    sprintf(show_line, "    %s %11i\t%-2d:%02d\t",
	    entry->vcd != 0 ? "*" : " ", entry->ipaddr,
	    left / 60, left % 60);
    sprint_atmaddr(&show_line[strlen(show_line)], entry);
				/* print the ATM address */
    printf("\n%s", show_line);
    return(TRUE);
}

/*
 * sprint_atmaddr
 *
 * Write an ATM addr onto a string
 */
static void
sprint_atmaddr (char *cur, as_entry *entry)
{
    if (entry->atm_addrlen == 0) {
	/*
	 *  This is a special case.  A client called us, but didn't leave
	 *  us his own NSAP.  So we'll display this in useful form.
	 */
	strcpy(cur, "<No ATM Address registered>");
    } else {
	int ix;
	
	for (ix = 0 ; ix < entry->atm_addrlen ; ix++) {
	    *cur++ = tohexchar((entry->atm_addr[ix] & 0xf0) >> 4);
	    *cur++ = tohexchar(entry->atm_addr[ix] & 0x0f);
	}
	
	/* print the ATM subaddress */
	if (entry->atm_subaddrlen) {
	    *cur++ = '/';
	
	    for (ix = 0 ; ix < entry->atm_subaddrlen ; ix++) {
		*cur++ = tohexchar((entry->atm_subaddr[ix] & 0xf0) >> 4);
		*cur++ = tohexchar(entry->atm_subaddr[ix] & 0x0f);
	    }
	}
	*cur++ = '\0';
    }
}

/*
 * atm_arp_serv_init
 *
 * Called to initialize the ATM ARP server on the passed interface.  Sets
 * the server timeout to the passed value, or the default if a 0 is passed.
 */
void
atm_arp_serv_init (idbtype *idb, int timeout)
{
    as_int *our_intptr;
    
    if (is_atm_arp_server(idb)) {
	/* We are already an ARP server.  Just set the new timeout */
	idb->atm_arp->as_data.server.timeout = timeout;
	return;
    }

    /*
     * If we were already configured, we shall use the existing structure.  If
     * we were originally a client, we need to do nothing special.  We will
     * continue to use the existing ARP servers as well as our own.
     */
    if (idb->atm_arp == NULL) {
	/* we must malloc a structure for ourselves */
	idb->atm_arp = malloc(sizeof(*idb->atm_arp));
	if (idb->atm_arp == NULL)
	    return;
    }

    /* we now have an ARP Server structure */
    idb->atm_arp->as_data.server.handle = malloc(sizeof(wavl_handle));
    if (idb->atm_arp->as_data.server.handle == NULL) {
	/* Oh well.  It would have been fun to be a server... */
	return;
    }
    
    /*
     *  The first time through, we must set up the master managed timer and
     *  the arp server list.
     */
    if (!arp_serv_init) {
	arp_serv_init = TRUE;
	mgd_timer_init_parent(&master_timer, NULL);
	wavl_init(&as_int_tree, 1, NULL, compare_int);
	reg_add_ip_atm_mapping_find(arpserv_lookup,
				    "RFC 1577 mapping information");
    }

    /*
     *  Now build an entry for our list of ARP Server interfaces
     */
    our_intptr = malloc(sizeof(*our_intptr));
    if (our_intptr == NULL) {
	free(idb->atm_arp->as_data.server.handle);
	return;
    }
    our_intptr->idb = idb;

    wavl_insert(&as_int_tree, &our_intptr->wavl);

    /*
     *  Prepare our trees
     */
    if (wavl_init(idb_to_handle(idb), NUM_AVL_THREADS, NULL,
		  compare_vcd, compare_ip_addr, compare_atm_addr)) {
	free(idb->atm_arp->as_data.server.handle);
	wavl_delete(&as_int_tree, &our_intptr->wavl);
	free(our_intptr);
	return;
    }

    idb->atm_arp->arp_flags |= ATM_ARP_SERVER;
    idb->atm_arp->as_data.server.timeout = timeout;
    mgd_timer_init_parent(&idb->atm_arp->as_data.server.timer, &master_timer);

    /*
     *  Because we will also act as our own client, we must populate the
     *  client side as well.
     */
    if (idb->atm_nsap_valid)
	atm_arp_client_init_common(idb, idb->nsap);
    else
	idb->hwptr->atm_db->check_nsap++;
}

/*
 * atm_arp_serv_off
 *
 * Stop running the ARP Server on the passed interface
 */
void
atm_arp_serv_off (idbtype *idb)
{
    as_entry *cur;
    wavl_handle *handle;
    wavl_node_type *our_ptr;
    as_int salt;
    
    if (!is_atm_arp_server(idb))
	return;		/* we were not on */

    /* stop all our timers */
    mgd_timer_stop(&idb->atm_arp->as_data.server.timer);

    /*
     * We must delete all our entries.  We might as well do this inefficiently,
     * as this will never really happen in the real world.  Note that because
     * we don't always have a thread on each tree, we will delete from all
     * of our trees.  In practice, almost all entries will have IP addresses.
     */

    handle = idb_to_handle(idb);
    while ((cur = wavl_to_entry(wavl_get_first(handle, AVL_VCD))) != NULL) {
	/* if there's a VC there's a call to hang up */
	hangup(cur->idb, cur->vcd, ATM_DEST_OUT_OF_ORDER);
	entry_destroy(cur);
    }
    while ((cur = wavl_to_entry(wavl_get_first(handle, AVL_IP_ADDR))) != NULL)
	entry_destroy(cur);

    while ((cur = wavl_to_entry(wavl_get_first(handle, AVL_ATM_ADDR))) != NULL)
	entry_destroy(cur);
    wavl_finish(handle);

    free(idb->atm_arp->as_data.server.handle);
    idb->atm_arp->as_data.server.handle = NULL;
    idb->atm_arp->arp_flags &= ~ATM_ARP_SERVER;

    /*
     * And take us off the list of ARP Servers
     */
    salt.idb = idb;
    our_ptr = wavl_search(&as_int_tree, &salt.wavl, 0);
    if (our_ptr != NULL) {
	wavl_delete(&as_int_tree, our_ptr);
	free(our_ptr);
    }

    idb->atm_arp->arp_flags &= ~ATM_ARP_SERVER;

    /*
     *  OK, the server side is gone.  Now make the client side forget
     *  about us too.
     */
    atm_arp_client_off(idb, idb->nsap);
}

/*
 * atm_arpserv_periodic
 *
 * Called once per second per ATM hwidb.  This means that if there are two ATM
 * interfaces, this will be called twice a second.  10 interfaces, 10 times
 * a second.  This may become a problem if we ever do more than check for
 * timer expiry.  As it is, it isn't worth the overhead to insure we are
 * only called once.
 */
void
atm_arp_serv_periodic (hwidbtype *hwidb)
{
    mgd_timer *expired_timer;
    as_entry *entry;
    idbtype *swidb;

    if (hwidb->atm_db == NULL)
	return;
    
    if (!arp_serv_init) {
	return;
    }

    /*
     *  First, check if any clients need to have their initialization
     *  finished.  This is needed because when the server was declared, if
     *  ILMI address negotiation had not yet been finished, we could not
     *  create a static map.
     */
    if (hwidb->atm_db->check_nsap) {
	boolean found_one = FALSE;
	    
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    if (is_atm_arp_server(swidb)) {
		/*
		 * If we already have an entry for ourselves, this can't be
		 * the one we are looking for.
		 */
		if (locate_arpserv_entry(swidb, swidb->nsap) != NULL)
		    continue;
		found_one = TRUE;
		if (swidb->atm_nsap_valid) {
		    atm_arp_client_init_common(swidb, swidb->nsap);
		    hwidb->atm_db->check_nsap--;
		}
	    }
	}

	/* This test can never succeed.  But it's worth doing, just in case */
	if (!found_one)
	    hwidb->atm_db->check_nsap = 0;
    }

    while (mgd_timer_expired(&master_timer)) {
	expired_timer = mgd_timer_first_expired(&master_timer);
	entry = mgd_timer_context(expired_timer);
	mgd_timer_stop(&entry->age_timer);

	switch (entry->timer_type) {
	case TIMER_NONE:
	    if (atm_arp_debug)
		buginf("\nARPSERVER (%s): unknown timer expiry (vcd %d, %i)",
		       entry->idb->namestring, entry->vcd, entry->ipaddr);
	    break;
	    
	case TIMER_AGER:
	    /*
	     * If we have a call in place, we'll InArp to see if they're still
	     * there.  We time the entry out in any case.  If they are still
	     * there, their response will create a new entry.
	     */
	    if (atm_arp_debug)
		buginf("\nARPSERVER (%s): entry for %i timed out",
		       entry->idb->namestring, entry->ipaddr);
	    
	    if (entry->vcd != 0)
		send_inarp(entry);
	    else 
		entry_destroy(entry);
	    break;

	case TIMER_WAIT:
	    /*
	     * The cretin on the other side doesn't want to talk to us.  So
	     * we'll just take our connection and go home.
	     */
	    if (entry->retrans_count > INARP_MAX_RETRANS) {
		if (atm_arp_debug)
		    buginf("\nARPSERVER (%s): No response on vc %d.  "
			   "Hanging up.",
			   entry->idb->namestring, entry->vcd);

		hangup(entry->idb, entry->vcd, ATM_NORMAL_UNSPECIFIED);
		entry_destroy(entry);
		break;
	    } else {
		/*
		 * We will send out a retransmission.  This is needed because
		 * sometimes someone along the path drops our first one.
		 */
		if (atm_arp_debug)
		    buginf("\nARPSERVER (%s): vc %d wait timer expiry.  "
			   "Retransmitting.",
			   entry->idb->namestring, entry->vcd);
		send_inarp(entry);
	    }
	}
    }
}

/*
 * hangup
 *
 * Hang up a call.
 */
static void
hangup (idbtype *idb, vcd_t vcd, int cause)
{
    sig_api_msg *sig_msg;
    sig_api_release *release;
    void *call_id;

    call_id = atmSig_findSvcBlockByVC(idb, vcd);
    if (call_id == NULL)
	return;			/* should never happen */

    sig_msg = malloc(sizeof(*sig_msg));
    if (sig_msg == NULL)
	return;
    
    sig_msg->opcode = ATM_RELEASE;
    release = (sig_api_release *)&(sig_msg->u.release);
    release->call_id = call_id;
    release->cause = cause;

    process_enqueue(atmSig_outputQ, sig_msg);
}

/*
 *
 * atm_arp_interested_in_vc
 * 
 *    - for now only checks for multipoint VCs
 *
 */
static boolean
atm_arp_interested_in_vc (idbtype *idb, vcd_t vcd)
{
    atm_db_t *atm;
    vc_info_t *vc;

    if ((idb == NULL) || (idb->hwptr == NULL))
        return(FALSE);

    atm = idb->hwptr->atm_db;

    if (atm == NULL)
        return(FALSE);

    vc = atm_getvc(atm, vcd);

    if (vc->flags & VC_FLAG_MULTIPOINT) 
        return(FALSE);

    return(TRUE);
}

/*
 * atm_arp_serv_connect
 *
 * Called whenever a call connects to an ARP server
 */
void
atm_arp_serv_connect (idbtype *idb, vcd_t vcd)
{
    as_entry *entry;

    if (!atm_arp_interested_in_vc(idb, vcd))
        return;
    
    /* and create our ARP entry */
    entry = entry_create(idb, vcd);
    if (NULL != entry) {
        send_inarp(entry);
    }
}

/*
 * atm_arp_serv_connect_ack
 *
 * Deal with the receipt of a connect ack.  This means the data path on
 * an incoming call is now established and I must send the inverse ARP.
 */
void
atm_arp_serv_connect_ack (idbtype *idb, vcd_t vcd)
{
    as_entry *entry;

    if (!atm_arp_interested_in_vc(idb, vcd))
        return;

    entry = entry_create(idb, vcd);
    if (NULL != entry) {
        send_inarp(entry);
    }
}

/*
 * atm_arp_serv_glommed_onto_vc
 *
 * Called to inform the arp server that we are now using a different VC to
 * a previously known IP address.
 */
void
atm_arp_serv_glommed_onto_vc (idbtype *idb,
			      vcd_t vc,
			      uint protocol,
			      addrtype *addr)
{
    as_entry *entry;

    /* none of these can ever happen */
    if (idb == NULL || addr == NULL || addr->ip_addr == NO_IP_ADDRESS)
	return;

    /* this gets called whether or not we're an arp server, so check */
    if (!is_atm_arp_server(idb))
	return;
    
    if (protocol != LINK_IP)
	return;			/* we are ip-centric right now */
    
    /*
     *  Before we get too excited about this development, let's see if we
     *  already have an entry for this IP address.
     */
    entry = locate_entry_by_ipaddr(idb_to_handle(idb), addr->ip_addr);
    if (entry != NULL) {
	if (atm_arp_debug)
	    buginf("\nARPSERVER (%s): %i already on VCD %d.  Ignoring VCD %d",
		   idb->namestring, entry->ipaddr, entry->vcd, vc);
	return;
    }

    entry = locate_entry_by_vcd(idb_to_handle(idb), vc);
    if (entry == NULL) {
	/*
	 *  Well, that's pretty unfair: this VC didn't get an entry created
	 *  when it connected.  We'll give him one now.
	 */
	entry = entry_create(idb, vc);
	if (entry == NULL) {
	    /*
	     *  Well, that's a problem.  Our data structures will get out
	     *  of sync if we let it stay connected.
	     */
	    if (atm_arp_debug || atm_errors_debug) {
		buginf("\nARPSERVER (%s): Cannot create database entry for VCD %d.", idb->namestring, vc);
		buginf("\nARPSERVER (%s): Disconnecting VCD %d to avoid database inconsistency", idb->namestring, vc);
	    }
	    hangup(idb, vc, ATM_TEMP_FAILURE);
	    return;
	}
    }
    if (entry->ipaddr != NO_IP_ADDRESS) {
	/* well, that shouldn't have happened */
	wavl_delete_thread(idb_to_handle(idb), AVL_IP_ADDR, entry->wavl);
    }
    entry->ipaddr = addr->ip_addr;
    wavl_insert_thread(idb_to_handle(idb), AVL_IP_ADDR, entry->wavl);

    send_inarp(entry);
}

/*
 * atm_arp_serv_disconnect
 *
 * Called whenever a call disconnects from an ARP server
 */
void
atm_arp_serv_disconnect (idbtype *idb, vcd_t vc)
{
    as_entry *entry;
    
    entry = locate_entry_by_vcd(idb_to_handle(idb), vc);
    if (entry == NULL) {
	return;		/* not a call we know about */
    }

    if (entry->ipaddr == NO_IP_ADDRESS && entry->atm_addrlen == 0) {
	/*
	 *  We don't know anything about this entry.  Might as well destroy it.
	 */
	entry_destroy(entry);
    } else {
	/*
	 * There is information here we have to save until we time it
	 * out.  Reflect the fact that there is no call, though.  We have to
	 * unthread by hand.
	 */
	wavl_delete_thread(idb_to_handle(idb), AVL_VCD, entry->wavl);
	entry->vcd = 0;
    }
}

/*
 * atmarp_rxpak
 *
 * This routine is called by the main ATM process to pass us all ARP Requests
 * and Inverse ARP Replies.  We will call datagram_out() directly with the
 * response (if any) to the request.  We will transmit the reply to the
 * pak->if_input interface.
 */
void
atmarp_rxpak (paktype *pak, vc_info_t *vc)
{
    atm_rfc_arp *arp;

    /*
     * First question: is the incoming interface an ARP server?  If not,
     * processing is easy
     */
    if (!is_atm_arp_server(pak->if_input)) {
	datagram_done(pak);
	return;
    }

    /*
     * Let's just do a couple of quick checks to make sure that this packet
     * belongs here.
     */
    arp = (atm_rfc_arp *)pak->network_start;
    if(GETSHORT(arp->arhrd) != ATM_ARHRD || GETSHORT(arp->arpro)!= ATM_ARPPRO){
	if (atm_arp_debug)
	    buginf("\nARPSERVER (%s): rx martian packet, vc %d",
		   pak->if_input->namestring, vc->vc);
	datagram_done(pak);
	return;		/* it was a martian */
    }

    switch (GETSHORT(arp->opcode)) {
    case ATM_ARP_REQ:
	if (atm_arp_debug)
	    buginf("\nARPSERVER (%s): rx ARP REQ from %i to %i (vc %d)",
		   pak->if_input->namestring,
		   extr_src_ipaddr(arp), extr_dst_ipaddr(arp), vc->vc);
	update_entry(pak->if_input, vc, arp->arshtl, arp->arsstl,
		     arsha(arp), arssa(arp), extr_src_ipaddr(arp));
	process_arp_request(pak, vc);
	break;
	
    case ATM_ARP_REPLY:
	/*
	 *  We don't update our tables on ARP replies, as they most likely
	 *  came from the ARP Server anyways, so the VCD is wrong.
	 */
	break;
	
    case ATM_INARP_REPLY:
	if (atm_arp_debug)
	    buginf("\nARPSERVER (%s): rx InARP REPLY from %i (vc %d)",
		   pak->if_input->namestring, extr_src_ipaddr(arp), vc->vc);
	update_entry(pak->if_input, vc, arp->arshtl, arp->arsstl,
		     arsha(arp), arssa(arp), extr_src_ipaddr(arp));
	break;

    default:
	update_entry(pak->if_input, vc, arp->arshtl, arp->arsstl,
		     arsha(arp), arssa(arp), extr_src_ipaddr(arp));
	break;
    }

    datagram_done(pak);
}

/*
 * process_arp_request
 *
 * Someone is asking us to look someone up
 */
static void
process_arp_request (paktype *pak, vc_info_t *vc)
{
    atm_rfc_arp *arp;
    as_entry	*entry;
    ipaddrtype	dest;

    arp = (atm_rfc_arp *)pak->network_start;

    /* find the target */
    dest = extr_dst_ipaddr(arp);
    entry = locate_entry_by_ipaddr(idb_to_handle(pak->if_input), dest);
    
    /*
     *  If we don't know about it, send a NAK.  Also NAK if the IP address
     *  called us but didn't leave a return address.
     */
    if (entry == NULL || entry->atm_addrlen == 0) {
	send_nak(pak, vc);
    } else {
	send_arp_reply(pak, vc, entry);
    }
}

/*
 * send_arp_reply
 *
 * send a reply for the passed arp message, using the data in the passed
 * entry.
 */
static void
send_arp_reply (paktype *in, vc_info_t *vc, as_entry *entry)
{
    paktype *out;
    atm_rfc_arp *in_head, *out_head;

    in_head = (atm_rfc_arp *)in->network_start;
    
    if (atm_arp_debug)
	buginf("\nARPSERVER (%s): tx ARP REPLY from %i to %i (vc %d)",
	       in->if_input->namestring, entry->ipaddr,
	       extr_src_ipaddr(in_head), vc->vc);

    out = getbuffer(ATM_ARP_MAX_SIZE);
    if (out == NULL) 
	return;

    out->datagramstart = atm_rfc_arp_headstart(out);

    /* build the datagram */
    out_head = (atm_rfc_arp *)out->datagramstart;
    PUTSHORT(out_head->arhrd, ATM_ARHRD);
    PUTSHORT(out_head->arpro, PR_IP);
    out_head->arshtl = entry->atm_addrlen | entry->atm_addrtype;
    out_head->arsstl = entry->atm_subaddrlen | entry->atm_subaddrtype;
    PUTSHORT(out_head->opcode, ATM_ARP_REPLY);
    out_head->arspln = sizeof(ipaddrtype);
    out_head->arthtl = in_head->arshtl;
    out_head->artstl = in_head->arsstl;
    out_head->artpln = in_head->arspln;

    /* now fill in the variable length elements */
    bcopy(entry->atm_addr, arsha(out_head),
	  out_head->arshtl & ATM_ADDR_LEN_MASK);
    bcopy(entry->atm_subaddr, arssa(out_head),
	  out_head->arsstl & ATM_ADDR_LEN_MASK);
    PUTLONG(arspa(out_head), entry->ipaddr);
    bcopy(arsha(in_head), artha(out_head),
	  out_head->arthtl & ATM_ADDR_LEN_MASK);
    bcopy(arssa(in_head), artsa(out_head),
	  out_head->artstl & ATM_ADDR_LEN_MASK);
    bcopy(arspa(in_head), artpa(out_head), out_head->artpln);
    
    out->datagramsize = atm_arp_datasize(out_head);

    /* now fill in the generic pak fields */
    out->if_input = NULL;
    out->if_output = in->if_input;
    out->linktype = LINK_ARP; 
    out->enctype = ET_ATM;
    out->acct_proto = ACCT_PROTO_ARP;
    atm_cpcs_encap(out, out->if_output->hwptr->atm_db, vc, zerosnap,
		   NULL, 0);
    atm_show_packet(out->if_output->hwptr, out, TRUE);
    datagram_out(out);
}

/*
 * send_nak
 *
 * send a NAK for the passed arp message.  we will not reuse the incoming
 * packet, even though all we need to do is change the op-code to a NAK,
 * because we do some funny pool stuff on datagram_done which I can't figure
 * out how to make happen if I datagram_out the packet.  And this is not going
 * to be a frequently run routine.
 */
static void
send_nak (paktype *in, vc_info_t *vc)
{
    atm_rfc_arp *out_head;
    paktype *out;

    out = pak_duplicate(in);
    if (out == NULL)
	return;			/* must be a malloc failure */
	
    out_head = (atm_rfc_arp *)out->network_start;

    if (atm_arp_debug)
	buginf("\nARPSERVER (%s): tx ARP NAK to %i for %i (vc %d)",
	       out->if_input->namestring, extr_src_ipaddr(out_head),
	       extr_dst_ipaddr(out_head), vc->vc);

    /*
     * Change the ARP REQ to an ARP NAK
     */
    PUTSHORT(out_head->opcode, ATM_ARP_NAK);

    /*
     *  We must cut back to allow for new encapsulation.
     */
    out->datagramstart = out->network_start;
    out->datagramsize = atm_arp_datasize(out_head);

    /* now fill in the generic pak fields */
    out->if_output = out->if_input;
    out->if_input = NULL;
    out->linktype = LINK_ARP; 
    out->enctype = ET_ATM;
    out->acct_proto = ACCT_PROTO_ARP;
    atm_cpcs_encap(out, out->if_output->hwptr->atm_db, vc, zerosnap,
		   NULL, 0);
    atm_show_packet(out->if_output->hwptr, out, TRUE);
    datagram_out(out);
}

/*
 * send_inarp
 * 
 * Send an InARP request to the passed entry
 */
static void
send_inarp (as_entry *entry)
{
    vc_info_t *vc;

    if (atm_arp_debug)
	buginf("\nARPSERVER (%s): tx InARP REQ on vc %d",
	       entry->idb->namestring, entry->vcd);

    /* we'll just leach off of the ARP client code here */
    vc = atm_getvc(entry->idb->hwptr->atm_db, entry->vcd);
    atm_send_inarp_req(vc, LINK_IP, NULL);

    /* now start the wait timer on our entry */
    entry->timer_type = TIMER_WAIT;
    entry->retrans_count++;
    mgd_timer_start(&entry->age_timer, INARP_WAIT_TIME);
}

/*
 * atm_addr_equal
 *
 * Compares an entry's ATM address with given address
 */
static boolean
atm_addr_equal (as_entry *entry,
		uchar atm_addr_len,
		uchar *atm_addr,
		uchar atm_subaddr_len,
		uchar *atm_subaddr)
{
    if (entry->atm_addrlen == ATM_ADDRLEN(atm_addr_len) &&
	entry->atm_addrtype == ATM_ADDRTYPE(atm_addr_len) &&
	!bcmp(atm_addr, entry->atm_addr, entry->atm_addrlen) &&
	entry->atm_subaddrlen == ATM_ADDRLEN(atm_subaddr_len) &&
	entry->atm_subaddrtype == ATM_ADDRTYPE(atm_subaddr_len) &&
	!bcmp(atm_subaddr, entry->atm_subaddr, entry->atm_subaddrlen))
	return TRUE;
    else
	return FALSE;
}

/*
 * entry_create
 *
 * Create a new ARP table entry
 */
static as_entry *
entry_create (idbtype *idb, vcd_t vcd)
{
    as_entry *entry;

    /*
     * We build our entry now because we have to squirrel away the call id
     */
    entry = malloc(sizeof(*entry));
    if (entry == NULL) 
	return NULL;

    entry->ipaddr = NO_IP_ADDRESS;
    entry->atm_addrlen = 0;
    entry->idb = idb;
    entry->vcd = vcd;

    mgd_timer_init_leaf(&entry->age_timer,
			&idb->atm_arp->as_data.server.timer,
			0, entry, FALSE);

    /* now add this entry to our tree */
    as_tree_insert(idb_to_handle(idb), entry);
    return(entry);
}

/*
 * entry_destroy
 *
 * Remove an ARP table entry.  Note that we will not hang up the call.
 */
static void
entry_destroy (as_entry *victim)
{
    hwaddrtype hwaddr;
    
    if (!populate_hwaddr_from_entry(&hwaddr, victim))
	reg_invoke_ip_atm_mapping_invalid(victim->idb,victim->ipaddr,&hwaddr);
    
    mgd_timer_stop(&victim->age_timer);
    as_tree_delete(idb_to_handle(victim->idb), victim);
    free(victim);
}

static void
write_atm_addr_to_entry (as_entry *entry,
			 uchar atm_addr_len,
			 uchar *atm_addr,
			 uchar atm_subaddr_len,
			 uchar *atm_subaddr)
{
    entry->atm_addrlen = atm_addr_len & ATM_ADDR_LEN_MASK;
    entry->atm_addrtype = atm_addr_len & ATM_E164_ADDR;
    bcopy(atm_addr, entry->atm_addr, entry->atm_addrlen);
    entry->atm_subaddrlen = atm_subaddr_len & ATM_ADDR_LEN_MASK;
    entry->atm_subaddrtype = atm_subaddr_len & ATM_E164_ADDR;
    bcopy(atm_subaddr, entry->atm_subaddr, entry->atm_subaddrlen);
}

/*
 * update_entry
 *
 * Called whenever we receive a mapping of ATM address to protocol address.
 * If we already have this information, we just restart the timer.  If it
 * is new, we create a new entry for it.
 */
static void
update_entry (idbtype *idb,
	      vc_info_t *vc,
	      uchar atm_addr_len,
	      uchar atm_subaddr_len,
	      uchar *atm_addr,
	      uchar *atm_subaddr,
	      ipaddrtype ip_address)
{
    as_entry *entry, *old_entry;
    boolean glom_flag; 

    /* this can never happen */
    if (idb->atm_arp ==	NULL)
	return;
    
    /* Yes, I'm being paranoid.  But am I being paranoid *enough*? */
    if (atm_addr_len == 0 || ip_address == NO_IP_ADDRESS) {
	if (atm_arp_debug) {
	    buginf("\nARPSERVER (%s): Insufficient information to update "
		   "table, VCD %d",
		   idb->namestring, vc->vc);
	}
	return;
    }

    /* Initialize the glom flag to false. */
    glom_flag = FALSE;

    /*
     *  Find the entry.  This is complicated by the fact that when we first
     *  create the entry it has no IP address.  So we first try to find by
     *  ip address (which is fast), and if that fails, we try by vc.
     */
    entry = locate_entry_by_ipaddr(idb_to_handle(idb), ip_address);
    if (entry == NULL) {
	/*
	 *  Either the first time on a new call, or an end station has changed
	 *  IP addresses.
	 */
	entry = locate_entry_by_vcd(idb_to_handle(idb), vc->vc);
	if (entry == NULL) {
	    /*
	     * There is actually a bizarre case where we can get here.  the
	     * other side had an existing connection, which it made before we
	     * were doing 1577, and then both sides switched over to classic
	     * IP, but the VC is still up, the other side will glom onto this
	     * connection to connect to us and we'll end up here.  So we must
	     * be willing to create an entry here also.
	     */
	    entry = entry_create(idb, vc->vc);
	    if (entry == NULL)
		return;		/* malloc must have failed */
	    if (atm_arp_debug)
		buginf("\nARPSERVER (%s): Create database entry for vcd %d",
		       idb->namestring, vc->vc);
	}

	/*
	 * OK, either this is the first time we see this, or the
	 * end station wants to change their IP address to one
	 * nobody else is using.  We are so accomodating that
	 * we'll let them do this. But we will not let any client 
	 * register an ATM address that is already active in our
	 * ARP tables.
	 */
	if (atm_arp_debug)
	    buginf("\nARPSERVER (%s): New IP address for vcd %d -- "
		   "was %i, now %i",
		   entry->idb->namestring, vc->vc, entry->ipaddr,
		   ip_address);
	if (entry->ipaddr != NO_IP_ADDRESS) 
	    wavl_delete_thread(idb_to_handle(idb), AVL_IP_ADDR, entry->wavl);
	else {
	    /*
	     * If an entry exists for incoming ATM address with an active VC
	     * then we should ignore this request, else delete that old entry.
	     */
	    old_entry = locate_entry_by_atmaddr(idb_to_handle(idb), 
		atm_addr_len, atm_addr, atm_subaddr_len, atm_subaddr);
	    if (old_entry != NULL) {
		if (old_entry->vcd != 0) {
		    if (atm_arp_debug) {
			char atm_addr_str[MAX_ATM_ADDR_STRLEN];

			sprint_atmaddr(atm_addr_str, old_entry);
			buginf("\nARPSERVER (%s): Ignoring duplicate ATM"
				" address %s on vc %d", entry->idb->namestring, 
				atm_addr_str, vc->vc);
		    }
		    return;
		} else
		    entry_destroy(old_entry);
	    }
	}
	entry->ipaddr = ip_address;
	wavl_insert_thread(idb_to_handle(idb), AVL_IP_ADDR, entry->wavl);
	as_change_atmaddr_entry(idb_to_handle(idb), entry, atm_addr_len, 
		atm_addr, atm_subaddr_len, atm_subaddr);

        glom_flag = TRUE; 

    } else {
	/*
	 * We have an entry for this IP address.  If it is on an inactive
	 * call, that means that a new call has come in with an old address.
	 * Delete the old entry and save the new one.
	 */
	if (entry->vcd == 0) {
	    as_entry *new_entry;

	    /*
	     *  Step 1: locate the new entry.
	     *
	     *  Do this before deleting the old entry, because if it fails,
	     *  we want to leave the old entry intact.
	     */
	    new_entry = locate_entry_by_vcd(idb_to_handle(idb), vc->vc);
	    if (new_entry == NULL) {
		/*
		 *  Almost certainly a response when we are the arp server
		 *  on the vc we are not using in out table.
		 */
		if (atm_arp_debug)
		    buginf("\nARPSERVER (%s): cannot locate entry for vcd %d",
			   idb->namestring, vc->vc);
		return;
	    }

	    /*
	     *  Step 2: delete the old entry
	     */
	    entry_destroy(entry);
	    
	    /*
	     *  Step 3: add new entry
	     *
	     *  Locate this as a new call.  If it isn't a new call, it could
	     *  be an old entry which just changed back to an old IP address.
	     */
	    as_tree_delete(idb_to_handle(idb), new_entry);
	    new_entry->ipaddr = ip_address;
	    write_atm_addr_to_entry(new_entry, atm_addr_len, atm_addr, 
		atm_subaddr_len, atm_subaddr);
	    as_tree_insert(idb_to_handle(idb), new_entry);
	    if (atm_arp_debug)
		buginf("\nARPSERVER (%s): refreshing IP address %i "
		       "(now on vcd %d)",
		       new_entry->idb->namestring, new_entry->ipaddr, vc->vc);
	    entry = new_entry;

            glom_flag = TRUE;

	} else {
	    /*
	     * the VC is still active.  If this comes from a different
	     * VC, we must silently ignore it.  Note: do not hang up
	     * this call.  When the client side of our interface calls
	     * us, we will end up with two vc's with the same IP
	     * address (and ATM address).  Hanging up here will keep
	     * us from talking to ourselves.
	     */
	    if (entry->vcd != vc->vc || entry->idb != vc->swidb) {
		as_entry *vc_entry;
		
		if (atm_arp_debug) {
		    buginf("\nARPSERVER (%s): Ignoring duplicate IP address %i"
			   " (have vcd %d, got vcd %d)",
			   entry->idb->namestring, entry->ipaddr,
			   entry->vcd, vc->vc);
		}

		/*
		 *  We are going to leave this entry in our arp server table
		 *  because we may end up glomming onto this VC later on and
		 *  we'll need to have this stuff lying around.  But note
		 *  that the IP address is not populated.  We'll populate
		 *  it on an update_entry() after we glom.  But we don't
		 *  start the timeout timer on it (obviously).
		 */
		vc_entry = locate_entry_by_vcd(idb_to_handle(idb), vc->vc);
		if (vc_entry == NULL) {
		    /* this should never happen */
		    ;
		} else {
		    vc_entry->timer_type = TIMER_NONE;
		    vc_entry->retrans_count = 0;
		    mgd_timer_stop(&vc_entry->age_timer);
		}
		
		/*
		 *  However, and this is truly hideous, if we have two
		 *  connections to the same remote side, and we are using one
		 *  to send on and the other side is using the other to
		 *  respond on, we must treat this response as coming in
		 *  on the other VC for timer purposes.
		 */
		if (atm_addr_equal(entry, atm_addr_len, atm_addr, 
			atm_subaddr_len, atm_subaddr)) {
		    /* it's from the same place.  let the timer restart */
		    if (atm_arp_debug)
			buginf("\nARPSERVER (%s): restarting age timer vcd %d",
			       entry->idb->namestring, entry->vcd);
		} else
		    return;	/* we can just ignore it */
	    }
	}
    }

    /*
     *  We will refresh ATM address if changed.  Before updating ATM address 
     *  though, we should make sure that we are not duplicating entries. If
     *  duplicate entries found and old one is active, no change is made. If
     *  old entry is inactive, it is destroyed and address is updated.
     */
    if (atm_addr_equal(entry, atm_addr_len, atm_addr, 
                        atm_subaddr_len, atm_subaddr) == FALSE) {
	old_entry = locate_entry_by_atmaddr(idb_to_handle(idb),
		atm_addr_len, atm_addr, atm_subaddr_len, atm_subaddr);
	if (old_entry != NULL) {
	    if (old_entry->vcd == 0) {
		entry_destroy(old_entry);
		as_change_atmaddr_entry(idb_to_handle(idb), entry, 
			atm_addr_len, atm_addr, atm_subaddr_len, atm_subaddr);
	    }
	} else
	    as_change_atmaddr_entry(idb_to_handle(idb), entry, atm_addr_len, 
			atm_addr, atm_subaddr_len, atm_subaddr);
    }

    /* Try to glom onto to a VC to the same destination NSAP. This needs
     * to be done here since client requires a static map with an active
     * VC before it can send route updates to remote clients. Data traffic
     * will set this up, but in a multiple LISs scenario this can be
     * be a problem.
     * 
     */

    if (glom_flag) {
         atm_arp_glom_by_serv(idb, atm_addr_len, atm_addr, ip_address);
    }

    /* restart the timer on this entry */
    entry->timer_type = TIMER_AGER;
    entry->retrans_count = 0;
    mgd_timer_start(&entry->age_timer,
		    idb->atm_arp->as_data.server.timeout);
}

/*
 * locate_entry_by_atmaddr
 *
 * Find an entry by ATM address
 */
static as_entry *
locate_entry_by_atmaddr (wavl_handle *handle,
                         uchar atm_addr_len,
                         uchar *atm_addr,
                         uchar atm_subaddr_len,
                         uchar *atm_subaddr)
{
    as_entry  goal;

    write_atm_addr_to_entry(&goal, atm_addr_len, atm_addr, atm_subaddr_len,
	 atm_subaddr);
    return (wavl_to_entry(wavl_search(handle, goal.wavl, AVL_ATM_ADDR)));
}

/*
 * locate_entry_by_ipaddr
 *
 * Find an entry by IP address
 */
static as_entry *
locate_entry_by_ipaddr (wavl_handle *handle, ipaddrtype ip_address)
{
    as_entry goal;

    goal.ipaddr = ip_address;
    return (wavl_to_entry(wavl_search(handle, goal.wavl, AVL_IP_ADDR)));
}

/*
 * locate_entry_by_vcd
 *
 * Find an entry by the vcd number.
 */
static as_entry *
locate_entry_by_vcd (wavl_handle *handle, vcd_t vcd)
{
    as_entry goal;

    goal.vcd = vcd;
    return (wavl_to_entry(wavl_search(handle, goal.wavl, AVL_VCD)));
}

/*
 *  compare_ip_addr
 *  compare_atm_addr
 *  compare_vcd
 *
 *  Comparison routines for the AVL tree to call.
 */
static enum avl_compare
compare_ip_addr (avl_node_type *n1, avl_node_type *n2)
{
    as_entry *node1, *node2;
    
    node1 = wavl_to_entry(wavl_normalize(n1, AVL_IP_ADDR));
    node2 = wavl_to_entry(wavl_normalize(n2, AVL_IP_ADDR));

    if (node1->ipaddr < node2->ipaddr)
	return(AVL_LT);
    if (node1->ipaddr > node2->ipaddr)
	return(AVL_GT);
    return(AVL_EQ);
}
static enum avl_compare
compare_atm_addr (avl_node_type *n1, avl_node_type *n2)
{
    int result;
    as_entry *node1, *node2;
   
    node1 = wavl_to_entry(wavl_normalize(n1, AVL_ATM_ADDR));
    node2 = wavl_to_entry(wavl_normalize(n2, AVL_ATM_ADDR));

    /* First compare ATM address lengths */
    if (node1->atm_addrlen < node2->atm_addrlen)
        return(AVL_LT);
    if (node1->atm_addrlen > node2->atm_addrlen)
        return(AVL_GT);

    /* Now if address lengths are equal, compare the ATM addresses */
    result = memcmp(node1->atm_addr, node2->atm_addr, node1->atm_addrlen);
    if (result < 0)
        return(AVL_LT);
    if (result > 0)
        return(AVL_GT);

    /* Return AVL_EQ if ATM address is same and no subaddress is present */
    if (node1->atm_subaddrlen == node2->atm_subaddrlen && 
	node1->atm_subaddrlen == 0)
	return(AVL_EQ);

    /* Now compare ATM subaddress length */
    if (node1->atm_subaddrlen < node2->atm_subaddrlen)
        return(AVL_LT);
    if (node1->atm_subaddrlen > node2->atm_subaddrlen)
        return(AVL_GT);

    /* If subaddress lengths are equal then compare the ATM subaddresses */
    result = memcmp(node1->atm_subaddr, node2->atm_subaddr, 
	node1->atm_subaddrlen);
    if (result < 0)
        return(AVL_LT);
    if (result > 0)
        return(AVL_GT);
    return(AVL_EQ);
}
static enum avl_compare
compare_vcd (avl_node_type *n1, avl_node_type *n2)
{
    as_entry *node1, *node2;

    node1 = wavl_to_entry(wavl_normalize(n1, AVL_VCD));
    node2 = wavl_to_entry(wavl_normalize(n2, AVL_VCD));

    if (node1->vcd < node2->vcd)
	return(AVL_LT);
    if (node1->vcd > node2->vcd)
	return(AVL_GT);
    return(AVL_EQ);
}

/*
 *  as_change_atmaddr_entry
 *
 */
static void
as_change_atmaddr_entry (const wavl_handle *handle, as_entry *entry,
		    uchar atm_addr_len,
		    uchar *atm_addr,
		    uchar atm_subaddr_len,
		    uchar *atm_subaddr)
{
    if (entry->atm_addrlen != 0)
	wavl_delete_thread(handle, AVL_ATM_ADDR, entry->wavl);
    write_atm_addr_to_entry(entry, atm_addr_len, atm_addr, atm_subaddr_len,
			atm_subaddr);
    wavl_insert_thread(handle, AVL_ATM_ADDR, entry->wavl);
}
/*
 *  as_tree_insert
 *  as_tree_delete
 *
 *  We frequently have items which are missing one or more of our searchable
 *  fields, so we must make sure that the placeholder values (NULL, 0, and
 *  NO_IP_ADDRESS) never get put into any of the trees.  All these routines
 *  do is block out those cases.
 */
static void
as_tree_insert (const wavl_handle *handle, as_entry *entry)
{
    if (entry->ipaddr != NO_IP_ADDRESS)
	wavl_insert_thread(handle, AVL_IP_ADDR, entry->wavl);
    if (entry->vcd != 0) 
	wavl_insert_thread(handle, AVL_VCD, entry->wavl);
    if (entry->atm_addrlen != 0) 
	wavl_insert_thread(handle, AVL_ATM_ADDR, entry->wavl);
}
static void
as_tree_delete (const wavl_handle *handle, as_entry *entry)
{
    if (entry->ipaddr != NO_IP_ADDRESS)
	wavl_delete_thread(handle, AVL_IP_ADDR, entry->wavl);
    if (entry->vcd != 0) 
	wavl_delete_thread(handle, AVL_VCD, entry->wavl);
    if (entry->atm_addrlen != 0) 
	wavl_delete_thread(handle, AVL_ATM_ADDR, entry->wavl);
}

/*
 * atm_clear_entry
 *
 * Remove an ARP table entry threaded by IP address. 
 * Send an INARP_REQ to rebuild the entry for existing VC. This would 
 * simulate the state where we have just received a connection.
 */
static void
atm_clear_entry (wavl_handle *handle, as_entry *entry)
{
    hwaddrtype hwaddr;

    if (!populate_hwaddr_from_entry(&hwaddr, entry))
	reg_invoke_ip_atm_mapping_invalid(entry->idb,entry->ipaddr,&hwaddr);
    mgd_timer_stop(&entry->age_timer);
    wavl_delete_thread(handle, AVL_IP_ADDR, entry->wavl);
    entry->ipaddr = NO_IP_ADDRESS;
    if (entry->atm_addrlen != 0) {
        wavl_delete_thread(handle, AVL_ATM_ADDR, entry->wavl);
	entry->atm_addrlen = 0;
    }
    send_inarp(entry);
}

/*
 *  atm_arp_clear
 *
 *  Clears ATM ARP entries.
 *  If IP address is specified that entry is deleted else all ARP entries are
 *  deleted.
 */
void atm_arp_clear (parseinfo *csb)
{
    idbtype     *idb;
    as_entry    *entry;
    wavl_handle *handle;
    ipaddrtype  ipaddr;

    idb = GETOBJ(idb,1);
    ipaddr = GETOBJ(paddr,1)->ip_addr;

    if ( !(idb->hwptr->status & IDB_ATM) ) {
        printf("\nInterface is not an ATM interface");
        return;
    }
    if (!is_atm_arp_server(idb)) {
	printf("\n%s is not functioning as an ATM ARP Server", idb->namestring);
	return;
    }

    handle = idb_to_handle(idb);

    /* If an IP address is specified GETOBJ(int,1) is set */
    if (GETOBJ(int,1)) {
	/* Locate the specified entry and delete if found */
	entry = locate_entry_by_ipaddr(handle,ipaddr);
	if (entry == NULL) {
	    printf("\nNo ARP entry exist for %i\n", ipaddr);
	    return;
	}
	/* 
	 * If a VC exists, then only delete entry from IP address thread 
	 * else delete from all threads and destroy entry.
	 */
	if (entry->vcd != 0) {
	    atm_clear_entry(handle, entry);
	} else {
	    entry_destroy(entry);
	}
    } else {
	/* Delete each existing entry */
	while ((entry = wavl_to_entry(wavl_get_first(handle, AVL_IP_ADDR))) 
	    != NULL) {
		if (entry->vcd != 0) {
		    atm_clear_entry(handle, entry);
		} else {
		    entry_destroy(entry);
		}
	}
    }
}
      

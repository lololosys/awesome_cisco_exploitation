/* $Id: latd.c,v 3.7.28.6 1996/09/11 17:39:07 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lat/latd.c,v $
 *------------------------------------------------------------------
 * latd.c -- cisco daemon level LAT code, interfaces to Meridian portable LAT 
 *
 * November 1989, Bill Westfield
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * LAT technology licensed from Meridian Technology, Copyright 1989.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latd.c,v $
 * Revision 3.7.28.6  1996/09/11  17:39:07  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.7.28.5  1996/08/28  12:56:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.28.4  1996/08/03  23:32:23  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.28.3  1996/07/29  19:02:18  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.7.28.2  1996/06/28  23:20:34  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.28.1  1996/03/18  20:39:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.1  1996/03/07  10:04:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/01/16  07:42:21  billw
 * CSCdi46868:  latgroup.o should not be in core subsystem
 *
 * Revision 3.6  1996/01/12  02:40:41  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.5  1996/01/10  06:49:34  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.4  1995/12/13  02:59:28  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.3  1995/11/17  17:33:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:56:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:26:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/20  05:06:30  smackie
 * Remove scary pointer arithmetic from LAT buffer handling. Use a direct
 * embedded pointer to the packet header instead. (CSCdi40672)
 *
 * Revision 2.4  1995/07/18  06:46:51  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/06/18 06:18:56  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 13:11:08  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:25:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mtcsys.h"
#include "registry.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "../h/auth_registry.regh"
#include "subsys.h"
#include "config.h"
#include "latls.h"
#include "latas.h"
#include "laths.h"
#include "latnd.h"
#include "latcb.h"
#include "latob.h"
#include "latslt.h"
#include "latfnc.h"
#include "latmsg.h"
#include "sysfnc.h"
#include "lat.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "latgroup.h"
#include "connect.h"
#include "latco.h"
#include "logger.h"
#include "../lat/msg_lat.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "../if/network.h"
#include "../iprouting/route.h"
#include "../snmp/sr_old_ltsmib.h"
#include "lat_public.h"
#include "lat_debug.h"
#include "aaa.h"


/*
 * Globals for use by LAT
 */

static watched_queue *latQ;		/* queue of input LAT packets */
static pid_t lat_timerpid;		/* Process number of LAT timers */
int lat_interfaces;			/* Count of interfaces running LAT */
connvector lat_connvector;		/* Connection dispatch vector */


/*
 * forward declarations
 */
static process lat_input(void);
static process lat_timer(void);
static void lat_enqueue(paktype *);

/*
 * externals
 */
extern boolean lat_enable;

#define LAT_PAKCOUNT 10		/* process this many packets at a time */

/*
 * LAT subsystem header
 */

#define LAT_MAJVERSION 1
#define LAT_MINVERSION 0
#define LAT_EDITVERSION  1

SUBSYS_HEADER(lat, LAT_MAJVERSION, LAT_MINVERSION, LAT_EDITVERSION,
	      lat_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

paktype *lat_currentpak;


/*
 * lat_create_process - Create the LAT process
 */

void lat_create_process (void)
{
    if (!lat_timerpid)
      lat_timerpid = process_create(lat_timer, "LAT Timers", NORMAL_STACK,
				    PRIO_NORMAL);
}

/*
 * lat_on_idb
 * Return TRUE if LAT is enabled on the given interface
 */

boolean lat_on_idb (idbtype *swidb)
{
    if (swidb && swidb->lat_running)
	return(TRUE);
    return(FALSE);
}

/*
 * lat_enable_test
 * Return TRUE if LAT is running anywhere in the system
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean lat_enable_test (void)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS(swidb) {
	if (swidb->lat_running)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * lat_accounting
 * fill in accounting info for AAA
 */
static void lat_accounting (conntype *conn, acctinfo *acct)
{
/*    sb_ptr sb = (sb_ptr )conn->tcb; */

    if (!(acct->flags & ACCT_F_FIRSTDONE)) {
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_PROTO, AAA_VAL_PROT_lat,
			     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_PROT_lat));
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_ADDR, conn->hostname,
				AAA_TYPE_STR, strlen(conn->hostname));
	acct->flags |= ACCT_F_FIRSTDONE;
    }
}


/*
 * lat_print_copyright
 */
static void lat_print_copyright (void)
{
    printf("SuperLAT software copyright 1990 by Meridian Technology Corp).\n");
}

/*
 * lat_register_thyself
 */
static void lat_register_thyself (void)
{
    lat_parser_init();
    lat_media_registry();

    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_LAT, lat_enqueue, "lat_enqueue");
    reg_add_proto_on_swidb(LINK_LAT, lat_on_idb, "lat_on_idb");
    reg_add_proto_running(LINK_LAT, lat_enable_test, "lat_enable_test");
    reg_add_setup_global(lat_setup_global, "lat_setup_global");
    reg_add_setup_int(lat_setup_int, "lat_setup_int");
    reg_add_setup_end(lat_setup_end, "lat_setup_end");
    reg_add_set_hostname(lat_sethost, "lat_sethost");
    reg_add_tty_show(lat_print_line, "lat_print_line");
    reg_add_default_lat_sx_get_char(term_sx_get_char, "term_sx_get_char");
    reg_add_default_lat_sx_put_char(term_sx_put_char, "term_sx_put_char");
    reg_add_lat_sx_session(SVC_CLASS_TERM, term_sx_session, "term_sx_session");
    reg_add_lat_ox_session(SVC_CLASS_TERM, term_ox_session, "term_ox_session");
    reg_add_print_copyright(lat_print_copyright, "lat_print_copyright");
    reg_add_match_group_name(match_lat_group_name, "match_lat_group_name");
}

/*
 * lat_init
 * Initialize the LAT data structures
 */

void lat_init (subsystype *subsys)
{
    idbtype *idb;

    latcom_init();
    lat_register_thyself();

    /*
     * Set up debugging flags
     */
    lat_debug_init();

    queue_init(&latgrpQ,0);

    conn_vector_init(&lat_connvector, "lat", "DEC LAT protocol");

    lat_connvector.snmp_sesstype = D_tslineSesType_lat;

    lat_connvector.vsettty = (boolean (*)(conntype *, tt_soc *)) return_true;	/* Set tty always OK */
    lat_connvector.vchanged = lat_datab;	/* Point to change routine */
    lat_connvector.vreport = lat_report;	/* Point to report routine */
    lat_connvector.valive = lat_alive;		/* Is connection alive? */
    lat_connvector.vrun = lat_run; 		/* Run the session */
    lat_connvector.vclose = lat_close;		/* Close the session */
    lat_connvector.vunread = lat_unread;	/* Return unread input count */
    lat_connvector.vopen = lat_multiproto_open;  /* Parse connect command */
    lat_connvector.vaccounting = lat_accounting; /* get accounting data */

    if (!lat_enable || system_loading) {
	lat_enable = FALSE;
	return;
    }

    /*
     * Set ourselves as the system default transport. This effects lines
     * for which no explicit preferred transport is set.
     */

    connect_sysdef_transport = lat_connvector.type;

    /*
     * If this is not a router, default all of the ethernet lines to
     * running LAT.
     *
     * CORRECTION - default all ethers, skip router check.  LAT not in router.
     *              this fixes CSCdi05466.
     *
     * CORRECTION - LAT is in the IGS with protocol translation.  Put back 
     *              the router enable check, but add a check for number of 
     *              tty lines.
     *
     */
   
    if ((!router_enable) || (nttylines > 0)) {
	FOR_ALL_SWIDBS(idb) {
	    if (idb->hwptr->status & IDB_ETHER) {
		idb->lat_running = TRUE;
		lat_interfaces++;
		(*(idb->hwptr->reset)) (idb->hwptr);
		lat_create_process();
	    }
	}
    }

    lat_host_init();
}

/*
 * lat_bridgeout - Send out packets in the transit bridging encapsulation
 *
 * This routine is used to send out a packet in the transit bridging
 * encapsulation on media that support it. Currently, this is just FDDI.
 *
 * On exit, we return NULL indicating that the I/O has been done, or
 * we return a packet pointer that should be sent out using the
 * native encapsulation. If this was a multicast the output will be
 * a pointer to a new packet.
 */

static paktype *lat_bridgeout (
     paktype *pak,
     idbtype *idb,
     uchar *addr)
{
    paktype *outpak = pak;

    if (idb->hwptr->status & IDB_FDDI) {

	pak->enctype = ET_BRIDGE;		/* Set bridge encapsulation */

	if (pak_encap(idb->hwptr, pak,
		      pak->lat_of_size & ~(OF_DELETE|OF_OUTPUT),
		      (long)addr)) {

	    /*
	     * Encapsulation worked. If this is a multicast, get a
	     * replacement buffer.
	     */

	    if (addr[0] & 0x01)
	      outpak = pak_duplicate(pak);
	    else
	      outpak = NULL;

	    datagram_out(pak);			/* Output the packet */
	}
    }
    return(outpak);
}

/*
 * lat_pakout - Encapsulate and output a LAT packet
 */

static void lat_pakout (
     paktype *pak,
     idbtype *idb,
     uchar *addr)
{
    pak->if_output = idb;
    pak->linktype = LINK_LAT;

    /*
     * Try to send out a packet using bridging for media with transit
     * encapsulation.
     */

    pak = lat_bridgeout(pak, idb, addr);
    if (!pak)
      return;				/* Worked */

    /*
     * Either the encapsulation failed or we were handed a replacement
     * buffer, so try sending out in the native encapsulation.
     */


    pak->enctype = ET_ARPA;

    if (pak_encap(idb->hwptr, pak, pak->lat_of_size & ~(OF_DELETE|OF_OUTPUT),
		  (long)addr)) {
	datagram_out(pak);
    } else {
	if (lat_debug) {
	    buginf("\nLAT: Encapsulation to %e failed", addr);
	}
	datagram_done(pak);
    }
}

/*
 * of_message(pak)
 * output a lat frame to the ethernet interface.  Possibly delete it when
 * the output is complete.
 */

void of_message (of)
    of_ptr of;
{
    paktype *pak = of->of_pak;
    paktype *opak;
    idbtype *idb;

    idb = pak->lat_of_idb;
    if (latpkt_debug)
	buginf("\nLAT: O dst=%e, int=%s, type=%x, M=%x, R=%x, len=%x, next %lx ref %d",
	       pak->lat_of_dst,
	       idb ? idb->namestring : "All",
	       pak->lat_of_data[0] >> CB_SERVER,
               (pak->lat_of_data[0] & CB_SERVER) >> CB_RRF,
               pak->lat_of_data[0] & CB_RRF,
               pak->lat_of_size & ~(OF_DELETE|OF_OUTPUT), 
               pak->lat_of_link, pak->refcount);


    /* Count the total transmissions */

    count_h(ch_total_xmit);

    /*
     * Check to see if this message is being sent to the null IDB. If so,
     * this is local I/O, so just put it on our local I/O queue.
     */

    if (idb == nullidb) {
	if (!(of->of_size & OF_OUTPUT)) {
	    of->of_size |= OF_OUTPUT;
	    pak->datagramstart = (uchar *)of->of_data;
	    if (of_local[0] == 0)
	      of_local[0] = of;
	    else
	      of_local[1]->of_i_o = of;
	    of_local[1] = of;
	    of->of_i_o = 0;
	}
	return;
    }

    /*
     * Not the null IDB, so we will be outputting the packet.
     * If the LAT engine does not want us to delete the packet on completion,
     * add an additional lock to the packet.
     */

    if (!(of->of_size & OF_DELETE))
      pak_lock(pak);

    if (idb) {
	if (idb->lat_running)
	  lat_pakout(pak, idb, (uchar *)pak->lat_of_dst);
	else
	  datagram_done(pak);
    } else {

	/*
	 * We do not have an output interface. If this message is
	 * a multicast message, we will output it on all interfaces
	 * with LAT enabled. Otherwise, we will report an error.
	 */

	if (((uchar *)pak->lat_of_dst)[0] & 0x01) {
	    FOR_ALL_SWIDBS(idb) {
		if (idb->lat_running) {
		    opak = pak_duplicate(pak);	/* Make a copy */
		    if (!opak)
		      break;			/* Get flakey... */
		    lat_pakout(opak, idb, (uchar *)pak->lat_of_dst);
						/* Write out the packet */
		}
	    }

	    /*
	     * Send ourselves a copy of the multicast as well
	     */

	    dst_addr[0] = pak->lat_of_dst[0];
	    dst_addr[1] = pak->lat_of_dst[1];
	    dst_addr[2] = pak->lat_of_dst[2];

	    src_addr[0] = ((ushort *)nullidb->hwptr->hardware)[0];
	    src_addr[1] = ((ushort *)nullidb->hwptr->hardware)[1];
	    src_addr[2] = ((ushort *)nullidb->hwptr->hardware)[2];

	    if_message(pak->lat_of_data,&src_addr[0],&dst_addr[0],nullidb);
	} else {
	    ether_hdr *ether = (ether_hdr *)(pak->network_start -
					     ETHER_ARPA_ENCAPBYTES);
	    errmsg(&msgsym(NULLIDB, LAT), ether->daddr);
	}
	datagram_done(pak);		/* Free the packet now */
    }
}

/*
 * lat_enqueue
 * Called from interrupt level to put a packet on the LAT input queue
 */ 

void lat_enqueue (paktype *pak)
{
    if ((pak->if_input->lat_running) && latQ)
	process_enqueue_pak(latQ, pak);
    else
	netinput_enqueue(pak);
}

/*
 * lat_timer - Handle the LAT timers
 *
 * This process runs periodically to run the LAT timers. This is where
 * the LAT process generates output for all of the virtual circuits.
 */

static process lat_timer (void)
{
    sys_timestamp lat_tictimer;
    sys_timestamp lat_toctimer;
    pid_t lat_inputpid;
    paktype *pak;

    process_wait_on_system_init();

    /*
     * Set up LAT input process' world.
     */
    latQ = create_watched_queue("LAT Input", 0, 0);
    lat_inputpid = process_create(lat_input, "LAT Input", NORMAL_STACK,
				  PRIO_HIGH);	/* Start the input process */

    GET_TIMESTAMP(lat_tictimer);
    TIMER_START(lat_toctimer, SECS_TOC * ONESEC);

    while (lat_interfaces || hs->hs_cb_current) { /* While running on any
						     interface, or while
						     any circuits are open */
	process_sleep_for(MSEC_TIC);

	blocking_disabled = TRUE;
	if (!SLEEPING(lat_tictimer)) {
	    tics_uncounted = ELAPSED_TIME(lat_tictimer) / MSEC_TIC;
						/* Calculate missed time */
	    tic_elapse();
	    TIMER_START(lat_tictimer, MSEC_TIC);
	}
	if (!SLEEPING(lat_toctimer)) {
	    toc_elapse(0);
	    TIMER_START(lat_toctimer, SECS_TOC * ONESEC);
	}
	blocking_disabled = FALSE;
    }

    /*
     * LAT is no longer running. Clean up after ourselves.
     */
    while ((pak = process_dequeue(latQ))) {
	netinput_enqueue(pak);
    }
    delete_watched_queue(&latQ);

    lat_timerpid = 0;
    ls_reset();				/* Forget learned services */
    nd_purge(0);			/* And remote nodes */
    process_kill(lat_inputpid);		/* Stop the world... */
    process_kill(THIS_PROCESS);		/* I want to get off... */
}

/*
 * lat_input - Handle LAT input from the network
 *
 * This is the process that runs all of the LAT code.  It passes any existing
 * input packets to LAT.
 */

static process lat_input (void)
{
    paktype *pak;
    int pak_count;

    process_watch_queue(latQ, ENABLE, RECURRING);

    while (TRUE) {
	blocking_disabled = FALSE;	/* Let us block just here */
	process_wait_for_event();
	blocking_disabled = TRUE;	/* Disable blocking */

	pak_count = LAT_PAKCOUNT;

	while (TRUE) {
	    /*
 	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    pak = process_dequeue(latQ);
	    if (pak == NULL) {
		break;
	    }

	    find_802addr(pak, (uchar *)dst_addr, GET_DEST);
	    find_802addr(pak, (uchar *)src_addr, GET_SOURCE);
	    if (latpkt_debug)
		buginf("\nLAT: I int=%s, src=%e, dst=%e, type=%x, M=%x, R=%x",
		       pak->if_input->namestring,
			src_addr, dst_addr,
		       *(uchar *)(pak->network_start) >> CB_SERVER,
                       (*(uchar *)(pak->network_start) & CB_SERVER) >> CB_RRF,
                       *(uchar *)(pak->network_start) & CB_RRF);


	    /*
	     * If this came in on token ring, bit-swap the address
	     */

	    if (pak->if_input->hwptr->status & IDB_TR) {
		((uchar *)src_addr)[0] = bit_swaps[(((uchar *)src_addr)[0])];
		((uchar *)src_addr)[1] = bit_swaps[(((uchar *)src_addr)[1])];
		((uchar *)src_addr)[2] = bit_swaps[(((uchar *)src_addr)[2])];
		((uchar *)src_addr)[3] = bit_swaps[(((uchar *)src_addr)[3])];
		((uchar *)src_addr)[4] = bit_swaps[(((uchar *)src_addr)[4])];
		((uchar *)src_addr)[5] = bit_swaps[(((uchar *)src_addr)[5])];

		((uchar *)dst_addr)[0] = bit_swaps[(((uchar *)dst_addr)[0])];
		((uchar *)dst_addr)[1] = bit_swaps[(((uchar *)dst_addr)[1])];
		((uchar *)dst_addr)[2] = bit_swaps[(((uchar *)dst_addr)[2])];
		((uchar *)dst_addr)[3] = bit_swaps[(((uchar *)dst_addr)[3])];
		((uchar *)dst_addr)[4] = bit_swaps[(((uchar *)dst_addr)[4])];
		((uchar *)dst_addr)[5] = bit_swaps[(((uchar *)dst_addr)[5])];
	    }

	    /*
	     * If this came in on a serial line, round up the packet
	     * length to an even length. LAT compression can cause us
	     * to send odd-length packets.
	     */

	    if (pak->if_input->hwptr->status & IDB_SERIAL &&
		pak->datagramsize & 1)
	      pak->datagramsize++;

	    if (if_check((if_byte)(pak->network_start), src_addr,
			 pak->datagramsize -
			 (pak->network_start - pak->datagramstart),
			 pak->if_input)) {

                lat_currentpak  = pak;
		pak->lat_of_pak = pak;
		if_message((if_byte)(pak->network_start),
			   src_addr, dst_addr,
			   pak->if_input);
	    }

	    /*
	     * Attempt to pass this packet off to be bridged.
	     */
	    net_bridge_enqueue_broadcast(pak);
	}
    }
}

/*
 * ix_error - Handle an illegal frame
 *
 * The LAT engine calls us with illegal frames here. 
 */

void ix_error (frame,src,len,err,idb)
    if_byte	frame;		/* Pointer to illegal frame		*/
    word	src[3];		/* Source address of frame		*/
    int		len;		/* Length of frame			*/
    int		err;		/* Error code (see latco.h).		*/
    idbtype	*idb;		/* Input IDB				*/
{
    if (lat_debug)
      buginf("\nLAT: Interface %s, Bad packet received from host %e, "
	     "len = %d, err = %x",
	     idb->namestring,
	     src,
	     len,
	     err);
}

/*
 * show_lat_as
 * Show our advertised services (services we provide to other places)
 */

void show_lat_as (void)
{
    as_ptr as;
    boolean flags_printed;
    byte last_service[SV_STR];
    char *autocommand;

    static const char lat_as_head[] = 
                              "Service Name         Rating     Rotary  Flags";

    /* If no services are being advertised, just exit */

    if ((as = as_list((byte *)"")) == NULL) {
	return;
    }

    automore_enable(lat_as_head);		/* Print the banner */

    for ( ; as ; as = as_list(last_service)) {

	/* Pick up fields we need after we may have blocked */

	str_move(last_service, as->as_service);
	autocommand = as->as_command;

	/* Print out the service name and rating */
	printf("\n%*s%*c  %-3d%s  ",
	       LATSTRING(as->as_service),((SV_STR-1) - *as->as_service), ' ',
	       as->as_rating,
	       (as->as_flags & AS_CLIENT) ? "(Dynamic)" : "(Static) ");

	/* Print out rotary membership or None */
	if (as->as_rotary) {
	    printf("%-5d  ",as->as_rotary);
	} else {
	    printf(" None  ");
	}

	flags_printed = FALSE;

	/* Announce whether the service is enabled */
	if (as->as_flags & AS_ENABLED) {
	    printf("Enabled");
	    flags_printed = TRUE;
	    if (!(as->as_flags & AS_OFFERED))
	      printf(", Unadvertised");
	}

	/* Announce whether this is a protocol translator service */

	if (as->as_aid) {
	    if (flags_printed)
		printf(", ");
	    printf("Translation");
	    flags_printed = TRUE;
	}

	/* Print out the service identification */

	if (*as->as_ident) {
	    printf("\n  Ident: %*s",LATSTRING(as->as_ident));
	}

	/* Print out the autocommand */

	if (autocommand) {
	    printf("\n  Autocommand: %s",autocommand);
	}
    }
    automore_disable();
}

/*
 * show_lat_ls
 * Show our learned services
 */

static const char lat_ls_head[] =
    "Service Name     Rating   Interface  Node (Address)";

void show_lat_ls (void)
{
    ls_ptr ls;
    nd_ptr nd;
    boolean header_printed = FALSE;
    byte last_service[SV_STR];

    last_service[0] = 0;

    automore_enable(NULL);
    while (TRUE) {
	ls = ls_list(last_service, (byte *)"", 0, 0);
	if (!ls)
	  break;
	nd = ls->ls_nd;

	str_move(last_service, ls->ls_service);

	if (lat_compare_groups(nd->nd_acl, stdio->tty_lat_groupmask)) {
	    if (!header_printed) {
		automore_header(lat_ls_head);
		header_printed = TRUE;
	    }
	    printf("\n%*s%*c  %-5d   ",
		   LATSTRING(ls->ls_service),
		   ((SV_STR-1) - *ls->ls_service), ' ',
		   ls->ls_rating);
	    if (nd->nd_idb == nullidb) {
		printf("Local");
	    } else {
		printf("%10s %*s (%e)",
		       nd->nd_idb->namestring,
		       LATSTRING(nd->nd_node),
		       nd->nd_host_addr);
	    }
	    if (*ls->ls_ident) {
		printf("\n  Ident: %*s", LATSTRING(ls->ls_ident));
	    }
	}
    }
    automore_disable();
}

/*
 * show_lat_nd
 * Show lat node structures
 */

void show_lat_nd (nd_ptr node)
{
    uchar name[] = {0,0};
    nd_ptr nd;
    byte last_node[ND_STR];
    boolean printed = FALSE, domore = FALSE;

    last_node[0] = 0;

    if (node)
	nd = node;
    else
	nd = nd_list(name);		/* get first service */

    if (!stdio->automore_enabled) {
	domore = TRUE;
	automore_enable(NULL);
    }

    for (; nd; nd = nd_list(last_node)) {

	str_move(last_node, nd->nd_node);
	if (lat_compare_groups(nd->nd_acl, stdio->tty_lat_groupmask)) {

	    printed = TRUE;
	    
	    /* Check to see if this will fit on the display */
	    
	    automore_conditional(6 + (nd->nd_ident[0] ? 1 : 0) +
				 print_lat_groups(nd->nd_classes, 
						  8, 12, FALSE) +
				 print_lat_groups(nd->nd_acl, 9, 16, FALSE));
	    
	    printf("\nNode \"%*s\", usage %d, ",
		   LATSTRING(nd->nd_node), nd->nd_usage);
	    if (nd->nd_idb == nullidb) {
		printf("Local");
	    } else {
		printf("Interface %s, Address %e",
		       nd->nd_idb->namestring,
		       nd->nd_host_addr);
	    }
	    if (*nd->nd_ident) {
		printf("\n  Ident: %*s", LATSTRING(nd->nd_ident));
	    }
	    printf("\n  Timer %d,  sequence %d,  changes %d,  flags %#x, protocol %d.%d",
		   nd->nd_timer, nd->nd_seq, nd->nd_changes,
		   nd->nd_flags, nd->nd_prot_ver, nd->nd_prot_eco);
	    
	    printf("\n  Facility %d,  Product code %d,  Product version %d",
		   nd->nd_facility, nd->nd_prod_code, nd->nd_prod_ver);
	    
	    printf("\n  Recv %d/%d/%d,  Xmit %d/%d/%d,  %d Dups, %d ReXmit",
		   nd->nd_stats.co_msgs_recv, nd->nd_stats.co_slot_recv,
		   nd->nd_stats.co_byte_recv,
		   nd->nd_stats.co_msgs_xmit, nd->nd_stats.co_slot_xmit,
		   nd->nd_stats.co_byte_xmit, nd->nd_stats.co_duplicates,
		   nd->nd_stats.co_retransmit);
	    
	    printf("\n  Bad messages: %d,  Bad slots: %d,  Solicits accepted: %d",
		   nd->nd_stats.co_msgs_bad, nd->nd_stats.co_slot_bad,
		   nd->nd_stats.co_solicit_accept);
	    
	    printf("\n  Solicits rejected: %d,  Multiple nodes: %d",
		   nd->nd_stats.co_solicit_reject, nd->nd_stats.co_multiple_node);
	    
	    if (*nd->nd_acl) {
		printf("\n  Groups:");
		print_lat_groups(nd->nd_acl, 9, 16, TRUE);
	    }
	    if (*nd->nd_classes) {
		printf("\n  Service classes:");
		print_lat_groups(nd->nd_classes, 18, 12, TRUE);
	    }
	}
	if (node)
	  break;
    } /* for nd */	
	
    if (domore)
	automore_disable();

    if (!printed) {
	printf("\nNo known LAT nodes.");
    }
}

/*
 * show_lat_traffic
 * Show all LAT packet statistics.
 */

void show_lat_traffic (void)
{
    printf("\nLocal host statistics:");
    printf("\n  %d/%d circuits, %d/%d sessions, %d/%d services",
	hs->hs_cb_current, hs->hs_cb_maximum,
	hs->hs_sb_current, hs->hs_sb_maximum,
	hs->hs_ls_current, hs->hs_ls_maximum);
    printf("\n  %d sessions/circuit, circuit timer %d, keep-alive timer %d",
	hs->hs_cb_max_ses, hs->hs_cb_vc_timer, hs->hs_cb_ka_timer);
    if (lat_host_delay !=0)
        printf("\n  Host delay %d msec\n", lat_host_delay);
    printf("\n\nRecv:   %d messages (%d duplicates),  %d slots,  %d bytes",
	hs->hs_stats.co_msgs_recv, hs->hs_stats.co_duplicates,
	hs->hs_stats.co_slot_recv, hs->hs_stats.co_byte_recv);
    printf("\n        %d bad circuit messages,  %d service messages (%d used)",
	hs->hs_stats.co_msgs_bad, hs->hs_count.ch_svc_recv,
	   hs->hs_count.ch_svc_used);
    printf("\nXmit:   %d messages (%d retransmit),  %d slots,  %d bytes",
	hs->hs_stats.co_msgs_xmit, hs->hs_stats.co_retransmit,
	hs->hs_stats.co_slot_xmit, hs->hs_stats.co_byte_xmit);
    printf("\n        %d circuit timeouts,  %d service messages",
	hs->hs_count.ch_total_timeouts, hs->hs_count.ch_svc_xmit);
    printf("\n\nTotal:  %d circuits created,  %d sessions",
	hs->hs_count.ch_total_circuits, hs->hs_count.ch_total_sessions);
}	

/*
 * show_lat_groups
 * show the currently defined group names, and which groups the include.
 */

void show_lat_groups (void)
{
    latgroup *latg;
    int width;

    if (latgrpQ.qhead)
	printf("\nGroup Name          Len   Groups");
    for (latg = (latgroup *) latgrpQ.qhead; latg; latg = latg->next) {
	width = 0;
	printf("\n%20s%3d  ", latg->name, latg->groups[0]);
	print_lat_groups(latg->groups, 25, 12, TRUE);
    }
}	

/*
 * lat_getbuffer
 * get an output buffer for LAT
 */

of_ptr lat_getbuffer (void)
{
    paktype *pak = getbuffer(DLL_MAX);
    if (pak) {
	pak->lat_of_idb = NULL;		/* No output interface initially */
	pak->lat_of_pak = pak;          /* Remember our own pointer */
	pak->lat_of_data = (of_byte)(pak->network_start);
	return((of_ptr) &pak->lat_of_link);
    }
    return(NULL);
}

/*
 * lat_retbuffer
 * Free a lat output buffer
 */

void lat_retbuffer (of)
    of_ptr of;
{
    paktype *pak = of->of_pak;

#if DEBUG
    if (latpkt_debug)
	buginf("\nLAT: retbuffer %lx, refcount %d", pak, pak->refcount);
#endif
    datagram_done(pak);
}

/*
 * lat_sethost ()
 *
 * Set a new host name
 *
 * This routine is called to keep the LAT hostname as maintained by LAT
 * in synch with the cisco hostname.
 */

void lat_sethost (void)
{
    if (hs) {

	idbtype *idb = SWIDBQ_HEAD;

	/*
	 * Guard against Null hostname string. It's a possibility
	 * when low-on-memory. Can cause Null Dereferencing.
	 */
	if (NULL != hostname) {
	    hs->hs_as_next_changes |= SVC_OTHER;
	    sstrncpy((char *)&hs->hs_node[1], hostname, ND_STR);
	    uppercase((char *)&hs->hs_node[1]);
	    hs->hs_node[0] = strlen(hostname);
	    if (str_check(hs->hs_node, ND_STR)) {
		return;
	    }
	    printf("\n%% Hostname \"%*s\" is not a legal LAT node name", 
		       LATSTRING(hs->hs_node));
	}
	hs->hs_node[0] = sprintf((char *)&hs->hs_node[1], 
				 "CISCO_%02x%02x%02x",
				 idb->hwptr->hardware[3],
				 idb->hwptr->hardware[4],
				 idb->hwptr->hardware[5]);
	printf(", Using \"%*s\"", LATSTRING(hs->hs_node));
    }
}

/*
 * lat_print_line (tty)
 *
 * This routine is used to print out the lat-specific terminal line
 * parameters. Currently, this is just group codes.
 */

void lat_print_line (tty)
     tt_soc *tty;
{
    if (!lat_enable)
      return;

    if (tty->tty_lat_groupmask[0]) {
	printf("\nGroup codes: ");
	print_lat_groups(tty->tty_lat_groupmask, 14, 16, TRUE);
    }
}

/*
 * lat_getdatabuf - Allocate a data buffer for a LAT connection
 *
 * This routine allocates a data buffer for a LAT connection, returning
 * TRUE for success and false for failure.
 */

boolean lat_getdatabuf (conn)
     conntype *conn;
{
    uchar *buf;

    buf = malloc(LAT_DATABUF);	/* Allocate a data buffer */
    if (buf) {				/* If we got one */
	conn->proto.lat.databuf = buf;	/* Set the pointer in connect block */
	conn->proto.lat.datahead = buf;	/* Set the head pointer */
	conn->proto.lat.datatail = buf;	/* And the tail pointer */
	conn->proto.lat.datacount = 0;	/* Initialize the count */
	return(TRUE);			/* Say it worked */
    }
    return(FALSE);			/* Allocation failure */
}

/*
 * lat_local_services_offered
 * 
 * Returns the number of local LAT services enabled 
 * 
 */
int lat_local_services_offered (void)
{
    as_ptr as;
    int count = 0;
    byte last_service[SV_STR];

    /* 
     * If no services are being advertised, return
     */
    if ((as = as_list((byte *)"")) == NULL) {
	return(0);
    }

    for ( ; as ; as = as_list(last_service)) {
      str_move(last_service, as->as_service);
      if (as->as_flags & AS_ENABLED)
        count++;
    }
    return(count);
}

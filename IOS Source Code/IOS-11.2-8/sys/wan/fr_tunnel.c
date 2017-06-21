/* $Id: fr_tunnel.c,v 3.1.22.11 1996/09/11 16:49:41 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_tunnel.c,v $
 *------------------------------------------------------------------
 * Frame Relay IP Tunneling
 *
 * January 1996, Charles Poe
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_tunnel.c,v $
 * Revision 3.1.22.11  1996/09/11  16:49:41  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.22.10  1996/08/28  13:21:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.22.9  1996/07/29  22:11:43  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.1.22.8  1996/07/23  13:31:24  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.22.7  1996/07/02  23:11:25  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.1.22.6  1996/06/28  23:32:06  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.22.5  1996/06/18  23:51:48  ccpoe
 * CSCdi60277:  priority-group command cannot be deleted from map-class
 * Branch: California_branch
 *
 * Revision 3.1.22.4  1996/05/31  18:19:36  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.1.22.3  1996/05/30  18:36:21  ccpoe
 * CSCdi58545:  FR Tunneling pvc was not shown by "show frame-relay pvc"
 * command
 * Branch: California_branch
 *
 * Revision 3.1.22.2  1996/05/15  05:54:47  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1.22.1  1996/04/25  23:24:00  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.30.1  1996/04/24  04:11:40  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1.2.4  1996/04/16  22:10:10  ccpoe
 * Code review comments from Shankar, Rene, and Paulina
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.3  1996/03/01  16:38:01  ccpoe
 * Create master timer in the parent process.
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.2  1996/02/22  06:58:21  ccpoe
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.1  1996/02/22  01:12:56  ccpoe
 * Frame Relay IP tunnel
 * Branch: FR_SVC_branch
 *
 * Revision 3.1  1996/02/13  23:41:46  ccpoe
 * Frame Relay IP Tunneling
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "serial.h"
#include "address.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "serial_debug.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../ip/ip.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "parser.h"
#include "parser_defs_frame.h"
#include "fr_switch.h"
#include "fr_lmi.h"
#include "fr_arp.h"
#include "fr_tunnel.h"
#include "../wan/fr_registry.h"
#include "mgd_timers.h"


/*
 * Tunnel process id
 */
static watched_queue *fr_tunnelQ;  /* Tunnel input packet queue */
fr_pvc_info_type *fr_tunnel_pvc[MAXDLCI];  /* store tunnel routes */

/*
 * LMI tunnel timer
 */
static mgd_timer *fr_tunnel_master_timer;
static mgd_timer *fr_tunnel_timer;

#define FR_TUNNEL_MAJVERSION 1
#define FR_TUNNEL_MINVERSION 0
#define FR_TUNNEL_EDITVERSION 1

SUBSYS_HEADER(fr_tunnel, FR_TUNNEL_MAJVERSION, FR_TUNNEL_MINVERSION,
	      FR_TUNNEL_EDITVERSION, fr_switch_tunnel_init,
	      SUBSYS_CLASS_PROTOCOL,
              "seq: frame_relay",
	      "req: frame_relay");


/*
 * fr_switch_tunnel_enqueue
 *
 * handle frame relay packets switched over an IP tunnel from interrupt
 * level and queue it for a process level func to work on
 */

static void fr_switch_tunnel_enqueue (paktype *pak)
{
    if (frame_relay_events_debug) {
	buginf("\n%s: FR Tunnel input",pak->if_input->namestring);
    }
    if (pak->if_input->hwptr->status & IDB_TUNNEL) {
	process_enqueue_pak(fr_tunnelQ, pak);
    } else {
	protocol_discard(pak, TRUE);
    }
}



/*
 * tunnel_search_fr_route
 *
 * special search routine for the special case of switching over tunnel
 * also make it inline to force the compiler to put the code where we want
 * it to be
 */

inline static fr_route_type *tunnel_search_fr_route (
    ushort in_dlci,
    hwidbtype *in_idb)
{
    fr_pvc_info_type *tunnel_pvc;

    tunnel_pvc = fr_tunnel_pvc[in_dlci];
    if (tunnel_pvc) {
	return(&tunnel_pvc->fr_route);
    }
    return(NULL);
}



/*
 * rcv_tunnel_lmi
 *
 * rcv the lmi msg from the tunnel to update my pvc status on the tunnel
 */

static void rcv_tunnel_lmi (
    hwidbtype *idb,		/* tunnel idb */
    paktype *pak)
{
    uchar *data_ptr;
    uchar *paksize;
    union frame_relay_ie_types *pvc_ie;
    fr_pvc_info_type *tunnel_pvc;
    ushort ansi_dlci, num_ansi_dlci;

    data_ptr = (uchar *)pak->datagramstart + 2;  /* point after the DLCI */
    paksize = pak->datagramstart + pak->datagramsize;
    if (frame_relay_lmi_debug(idb))
        buginf("\n%s(in): Status", idb->hw_namestring);

    while (IS_FRAME_RELAY_LMI_IE_PKT(*data_ptr) && data_ptr < paksize) {
	switch (*data_ptr) {
	    case PVC_IE_ID:
	        pvc_ie = (union frame_relay_ie_types *)data_ptr;
	        /*
	         * parse pvcs ie 
	         */
	        memcpy((uchar *)&ansi_dlci,
			(uchar *)&pvc_ie->ie_bytes[PVC_DLCI_INDEX],
			sizeof(ushort));
	        num_ansi_dlci = ANSI_DLCI_TO_NUM(ansi_dlci);
	        if ((tunnel_pvc = fr_tunnel_pvc[num_ansi_dlci])) 
		    tunnel_pvc->nni_pvc_status = 
				pvc_ie->ie_bytes[PVC_STATUS_INDEX];

                if (frame_relay_lmi_debug(idb))
                    print_pvc_ie(data_ptr, FR_ANSI_D);
	        data_ptr += ANSI_PVC_LEN;
	        break;
	    default:
	        data_ptr += pak->datagramsize;
	}
    }
}


/*
 * fr_tunnel_packets
 *
 * fork a background process to receive and process packets that were
 * switched over an IP tunnel.
 */

static void fr_tunnel_packets (void)
{
    ulong count;
    paktype *pak;
    hwidbtype *idb;
    fr_route_type *route;
    ushort q922_dlci, num_dlci, out_dlci, paksize;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc, *tunnel_fr_pvc;


    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	/*
	 * process a few per pass. Don't hog the processor
	 */
	pak = process_dequeue(fr_tunnelQ);
	if (!pak)
	    break;
	idb = pak->if_input->hwptr;   /* this should be the tunnel idb */
	/*
	 * tunnel gives us the datagramstart, so get the dlci
	 */
	(ushort)q922_dlci = *((ushort *)pak->datagramstart);
	/*
	 * can't use macro DLCI_TO_NUM. we don't have a fr_idb yet
	 */
	num_dlci = (((q922_dlci & 0xfc00) >> 6) |
		    ((q922_dlci & 0xf0) >> 4));
	/*	
	 * if this is the tunnel LMI
	 */
	if (num_dlci == FR_TUNNEL_DLCI) {
	    rcv_tunnel_lmi(idb, pak);
	    datagram_done(pak);
	    break;
	}
	/*
	 * now we have the "out idb" and "out dlci" of some route entry.
	 * do a reverse search thru all to find their "in idb" and
	 * "in dlci"
	 */
	if (!(tunnel_fr_pvc = fr_tunnel_pvc[num_dlci])) {
	    datagram_done(pak);
	    break;
	}
	
	/*
	 * Update input statistics
	 */
	paksize = pak->datagramsize;
	tunnel_fr_pvc->pvc_stats.num_bytes_rcv += paksize;
	tunnel_fr_pvc->pvc_stats.num_input_pkts++;
	if (ISFR_FECN(q922_dlci))
	    tunnel_fr_pvc->pvc_stats.num_rcvd_fecn_pkts++;
	if (ISFR_BECN(q922_dlci))
	    tunnel_fr_pvc->pvc_stats.num_rcvd_becn_pkts++;
	if (ISFR_DE(q922_dlci))
	    tunnel_fr_pvc->pvc_stats.num_rcvd_de_pkts++;
	idb_increment_rx_proto_counts(pak, ACCT_PROTO_FRS);
	
	if (!(route = tunnel_search_fr_route(num_dlci, idb))) {
	    datagram_done(pak);
	    tunnel_fr_pvc->pvc_stats.num_dropped_pkts++;
	    break;
	}
	/*
	 * so we have a route entry. So ship it out the interface
	 * specified by in_idb in the route entry and slam dunk
	 * in_dlci from the route entry into the packet.
	 */
	fr_idb = route->out_idb->frame_relay_stuff;
	if (!fr_idb) {
	    datagram_done(pak);
	    break;
	}
	fr_pvc = fr_idb->fr_dlci_tab[route->out_dlci];
	if (!fr_pvc) {
	    datagram_done(pak);
	    break;
	}
	if (!(route->out_idb->nokeepalive)) {
	    if (!IS_PVC_ACTIVE_BIT_SET(fr_pvc->pvc_status)) {
		fr_pvc->pvc_stats.num_dropped_pkts++;
		datagram_done(pak);
		break;
	    }
	}
	out_dlci = NUM_TO_DLCI(route->out_dlci);
	out_dlci |= FRAME_RELAY_EA_BITS;
	/*
	 * for the time being, just reproduce these bits as they are seen
	 */
	out_dlci |= (q922_dlci & FR_NON_DLCI_BITS);
	
	memcpy ((uchar *)pak->datagramstart, (uchar *)&out_dlci,
	       sizeof(ushort));
	pak->if_output = route->out_idb->firstsw;
	
	/*
	 * Update output statistics
	 */
	tunnel_fr_pvc->pvc_stats.num_pkts_switched++;
	fr_pvc->pvc_stats.num_output_pkts++;
	fr_pvc->pvc_stats.num_bytes_send += paksize;
	if (ISFR_FECN(out_dlci))
	    fr_pvc->pvc_stats.num_sent_fecn_pkts++;
	if (ISFR_BECN(out_dlci))
	    fr_pvc->pvc_stats.num_sent_becn_pkts++;
	if (ISFR_DE(out_dlci))
	    fr_pvc->pvc_stats.num_sent_de_pkts++;
	route->out_idb->counters.tx_frames[ACCT_PROTO_FRS]++;
	route->out_idb->counters.tx_bytes[ACCT_PROTO_FRS] += paksize;
	
	/*
	 * debug for outgoing pkts 
	 */
	if (frame_relay_debug) {
	    buginf("\nSwitching pkt rcvd on DLCI %d(0x%x) to DLCI %d(0x%x)",
		   num_dlci, q922_dlci, DLCI_TO_NUM(out_dlci), out_dlci);
	    buginf("\nSwitching from interface %s to interface %s",
		   idb->hw_namestring, pak->if_output->namestring);
	}

	if (route->out_idb->board_encap)
		(*route->out_idb->board_encap)(pak,route->out_idb);
	
	datagram_out(pak);
    }
}




/*
 * get_tunnel_pvcs
 */

int get_tunnel_pvcs (hwidbtype *tunnel_idb)
{
    int i;
    fr_pvc_info_type *tunnel_pvc;
    int count = 0;

    for (i = 0; i < MAXDLCI; i++) {
	tunnel_pvc = fr_tunnel_pvc[i];
	if (!tunnel_pvc)
	    continue;
	if (tunnel_pvc->fr_route.in_idb == tunnel_idb)
	    count++;
    }
    return(count);
}



/*
 * set_tunnel_pvc_status
 *
 * tunnel decides the PVC status for all pvc's that go thru the tunnel
 */

void set_tunnel_pvc_status (void)
{
    hwidbtype *out_idb;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *tunnel_pvc, *fr_pvc;
    ushort out_dlci;
    uchar out_status;
    int i;

    for (i = 0; i < MAXDLCI; i++) {
	if (!(tunnel_pvc = fr_tunnel_pvc[i]))
	    continue;

	out_idb = tunnel_pvc->fr_route.out_idb;
	out_dlci = tunnel_pvc->fr_route.out_dlci;
	out_status = tunnel_pvc->pvc_status;
        /*
         * if pvc route to other interface not defined : status = INACTIVE
         */
        if ((is_frame_relay(out_idb)) &&
             (fr_idb = out_idb->frame_relay_stuff) &&
              (fr_pvc = fr_idb->fr_dlci_tab[out_dlci]) &&
	       fr_pvc->route_flag) {
            /*
             * check if everything is OK on other side now
             */
            if (hw_interface_up(out_idb) &&
                  (IS_PVC_ACTIVE_BIT_SET(fr_pvc->nni_pvc_status) ||
		   out_idb->nokeepalive)) {
                SET_PVC_ACTIVE(tunnel_pvc->pvc_status);
	    } else {
                RESET_PVC_ACTIVE(tunnel_pvc->pvc_status);
	    }
	} else {
            RESET_PVC_ACTIVE(tunnel_pvc->pvc_status);
	}
    }
}


/*
 * send_tunnel_lmi
 *
 * send a hacked version of LMI over the tunnel to get the status of the
 * remote dlci
 */

static void send_tunnel_lmi (
    hwidbtype *idb,		/* tunnel idb to send the lmi */
    int num_tunnel_pvcs)
{
    paktype *pak;
    unsigned pkt_size;
    uchar *data_ptr;
    fr_pvc_info_type *tunnel_pvc;
    frame_relay_hdr *hdr;
    int i;

    /*
     * get the number of pvc's for a particular tunnel
     */
    pkt_size = 2 + (num_tunnel_pvcs*ANSI_PVC_LEN);
    pak = getbuffer(pkt_size);
    if (!pak)
        return;
    data_ptr = pak->network_start - 2;
    /*
     * point at location after dlci and add pvc's
     */
    for (i = 0; i < MAXDLCI; i++) {
	tunnel_pvc = fr_tunnel_pvc[i];
	if (!tunnel_pvc)
	    continue;
	if (tunnel_pvc->fr_route.in_idb != idb)
	    continue;
	data_ptr = add_ansi_ccitt_pvc(NULL, data_ptr, tunnel_pvc,
				      i,FR_ANSI_D);
    }
    if (frame_relay_lmi_debug(idb)) 
        buginf("\n%s(out): Status", idb->hw_namestring);
     
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_FR_SWITCH;
    pak->datagramstart = pak->network_start - sizeof(frame_relay_hdr);
    pak->datagramsize = pkt_size;

    hdr = (frame_relay_hdr *)pak->datagramstart;
    hdr->dlci = FR_TUNNEL_DLCI | FRAME_RELAY_EA_BITS;
    datagram_out(pak);
}



/*
 * fr_tunnel_timeout
 *	Send tunnel pvc status
 */
static void fr_tunnel_timeout (void)
{
    hwidbtype *hwidb;
    int num_tunnel_pvcs = 0;
    mgd_timer *expired_tunnel_timer;


    while (mgd_timer_expired(fr_tunnel_master_timer)) {
	expired_tunnel_timer = mgd_timer_first_expired(fr_tunnel_master_timer);
	mgd_timer_update(expired_tunnel_timer, TUNNEL_LMI_INTERVAL * ONESEC);
        /*
	 * update tunnel route status
	 */
	if (fr_switching == FALSE)
	    return;
	set_tunnel_pvc_status();
	FOR_ALL_HWIDBS(hwidb) {
	    if (hwidb->status & IDB_TUNNEL) {
		/*
		 * check our table for route entries on this tunnel
		 */
		num_tunnel_pvcs = get_tunnel_pvcs(hwidb);
		if (num_tunnel_pvcs) {
		    send_tunnel_lmi(hwidb, num_tunnel_pvcs);
		    num_tunnel_pvcs = 0;
		}
	    }
	}
    }
}



/*
 * fr_tunnel_switch_pvc
 *
 * With given input pvc, 
 * Return output pvc if the output interface is tunnel interface and
 * there is a output pvc for the intput pvc.
 */
static fr_pvc_info_type *fr_tunnel_switch_pvc(
				       fr_pvc_info_type *in_pvc)
{
    fr_pvc_info_type *out_pvc;
    
    if (in_pvc->fr_route.out_idb->status & IDB_TUNNEL) {
	if((out_pvc = fr_tunnel_pvc[in_pvc->fr_route.out_dlci])) {
	    return(out_pvc);
	}
    }
    return (NULL);
}


/*
 * fr_tunnel_timer_create
 *
 * Create the frame relay tunnel timer which expires every 10 sec and
 * send out tunnel LMI updates
 */
static void fr_tunnel_timer_create(void)
{
    if (fr_tunnel_timer) {
	return;
    }
    fr_tunnel_timer = malloc_named(sizeof(mgd_timer), "FR Tunnel Timer");
    if (fr_tunnel_timer == NULL) {
	printf("\n%% fail to malloc tunnel timer");
    }
    mgd_timer_init_leaf(fr_tunnel_timer, fr_tunnel_master_timer, 
			LMI_TUNNEL_TIMER, NULL, FALSE);
    mgd_timer_start(fr_tunnel_timer, 20 * ONESEC);
}


/*
 * fr_tunnel_create
 *
 */
static fr_pvc_info_type *fr_tunnel_create (fr_pvc_info_type *in_pvc)
{
    hwidbtype *out_idb, *in_idb;
    fr_pvc_info_type *tunnel_pvc;
    ushort out_dlci, in_dlci;

    if (in_pvc->route_flag != TRUE) {
	return NULL;
    }
    
    out_idb = in_pvc->fr_route.out_idb;
    out_dlci = in_pvc->fr_route.out_dlci;
    in_idb = in_pvc->fr_route.in_idb;
    in_dlci = in_pvc->fr_route.in_dlci;
    if (out_idb->status & IDB_TUNNEL) {
        in_pvc->pvc_usage = PVC_SWITCHED_IP_TUNNEL;
	if (fr_tunnel_pvc[out_dlci]) {
	    printf("\nDLCI already allocated");
	    return NULL;
	}
	tunnel_pvc = create_fr_pvc();
	if (!tunnel_pvc) {
	    printf ("\nno memory available to allocate pvc table");
	    return NULL; 
	}
	/*
	 * Start with tunnel pvc inactive to avoid transient conditions, 
	 * update thru the timer later
	 */
	tunnel_pvc->route_flag = TRUE;
	tunnel_pvc->map_flag = FALSE;  /* can never have a map on tunnel */
	tunnel_pvc->pvc_usage = PVC_SWITCHED_SERIAL;
	tunnel_pvc->fr_route.in_dlci = out_dlci;
	tunnel_pvc->fr_route.in_idb = out_idb;
	tunnel_pvc->fr_route.out_dlci = in_dlci;
	tunnel_pvc->fr_route.out_idb = in_idb;
	fr_tunnel_pvc[out_dlci] = tunnel_pvc;  /* add to tunnel pvc list */
	RESET_PVC_ACTIVE(tunnel_pvc->pvc_status);
	return tunnel_pvc;
    }
    return NULL;
}



static void fr_tunnel_delete (
		       fr_pvc_info_type *in_pvc)
{
    fr_pvc_info_type *tunnel_pvc;
    
    if (in_pvc->pvc_usage == PVC_SWITCHED_IP_TUNNEL) {
	tunnel_pvc = fr_tunnel_pvc[in_pvc->fr_route.out_dlci];
	if (tunnel_pvc) {
	    fr_tunnel_pvc[in_pvc->fr_route.out_dlci] = NULL;
	    free(tunnel_pvc);
	}
    }
}


static void fr_tunnel_switch (fr_pvc_info_type *in_pvc,
		       fr_pvc_info_type **out_pvc,
		       ushort sub_command)
{
    fr_pvc_info_type *tunnel_pvc = NULL;
    switch (sub_command) {
    case FR_TUNNEL_ADD:
	 tunnel_pvc = fr_tunnel_create(in_pvc);
	 *out_pvc = tunnel_pvc;
	break;
    case FR_TUNNEL_REMOVE:
	fr_tunnel_delete(in_pvc);
	break;
    case FR_TUNNEL_FIND:
	tunnel_pvc = fr_tunnel_switch_pvc(in_pvc);
	*out_pvc = tunnel_pvc;
	break;
    default:
	break;
    }
}
	    

/*
 * fr_init_tunnel_stats
 */
void fr_init_tunnel_stats (hwidbtype *idb)
{
    fr_pvc_info_type *fr_pvc;
    int i;

    if (idb->status & IDB_TUNNEL) {
	for (i = 0; i < MAXDLCI; i++) {
	    if ((fr_pvc = fr_tunnel_pvc[i])
		&& (fr_pvc->fr_route.in_idb == idb)) {
		clear_fr_pvc_stats(&fr_pvc->pvc_stats);
	    }
	}
    }
}


static void fr_print_tunnel_route (void)
{
    fr_pvc_info_type *fr_pvc;
    int i;
    fr_route_type fr_route;   /* copy on the stack before display */
    uchar status;
    
    for (i = 0; i < MAXDLCI; i++) {
	fr_pvc = fr_tunnel_pvc[i];
	if (!fr_pvc)
	    continue;
	fr_route = fr_pvc->fr_route;
	status = fr_pvc->pvc_status;

	printf("\n%16s%d \t\t%16s%d ",
	       fr_route.in_idb->hw_namestring,
	       fr_route.in_dlci,
	       fr_route.out_idb->hw_namestring,
	       fr_route.out_dlci);
	printf("\t\t");
	printf("%s",DLCI_ACTIVE(status) ? "active" : "inactive");

    }
}



/*
 * show pvc per tunnel idb stats
 */
static void fr_print_tunnel_pvc_status (hwidbtype *hwidb, ushort dlci)
{
    
    fr_pvc_info_type *fr_pvc;
    int i;

    if (!dlci) {
	for (i = 0; i < MAXDLCI ; i++) {
	    if (!(fr_pvc = fr_tunnel_pvc[i]))
	        continue;
            printf ("\n\nPVC Statistics for interface %s", 
		    fr_pvc->fr_route.in_idb->hw_namestring);
	    display_pvc_stats(fr_pvc, i, FALSE);
	    automore_conditional(0);
	}
    } else {          /* show stats for a particular dlci */
	fr_pvc = fr_tunnel_pvc[dlci];
	if (!fr_pvc) {
	    return;
	}
        printf ("\n\nPVC Statistics for interface %s", 
		fr_pvc->fr_route.in_idb->hw_namestring);
	display_pvc_stats(fr_pvc, dlci, FALSE);
    }
}



/*
 * fr_tunnel_background
 * 
 */
static forktype fr_tunnel_background (void)
{
    ulong major, minor;

    fr_tunnel_timer_create();
    fr_tunnelQ = create_watched_queue("FR Tunnel packet", 0, 0);
    process_watch_queue(fr_tunnelQ, ENABLE, RECURRING);
    process_watch_mgd_timer(fr_tunnel_master_timer, ENABLE);
    
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case QUEUE_EVENT:
		fr_tunnel_packets();
		break;
	    case TIMER_EVENT:
		fr_tunnel_timeout();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}



/*
 * fr_tunnel_start
 * 	Create the FR TUNNEL process and tunnel master timer
 */
static void fr_tunnel_start (void)
{
    
    (void) process_create(fr_tunnel_background, "FR TUNNEL",
				   NORMAL_STACK, PRIO_HIGH);
}

 

/*
 * fr_switch_tunnel_init
 *
 * initialize the queues for receiving frame relay switched packets
 * over an IP tunnel
 */

void fr_switch_tunnel_init (subsystype *subsys)
{
    
    fr_tunnel_master_timer = malloc_named(sizeof(mgd_timer), "FR Tunnel Master Timer");
    if (fr_tunnel_master_timer == NULL) {
	return;
    }
    mgd_timer_init_parent(fr_tunnel_master_timer, NULL);
    
    reg_add_fr_proc_create(fr_tunnel_start, "Create and start tunnel Process");
    reg_add_fr_tunnel_pvc(fr_tunnel_switch, "Create/Delete/Find a tunnel PVC");
    reg_add_clear_counters(fr_init_tunnel_stats, "zero tunnel ounters");
    reg_add_fr_show_route(fr_print_tunnel_route, "fr_print_tunnel_route");
    reg_add_fr_show_tunnel_pvc_stats(fr_print_tunnel_pvc_status,
				     "fr_print_tunnel_pvc_status");

    reg_add_raw_enqueue(LINK_FR_SWITCH, fr_switch_tunnel_enqueue,
		        "fr_switch_tunnel_enqueue");

}



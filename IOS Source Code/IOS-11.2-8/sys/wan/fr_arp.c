/* $Id: fr_arp.c,v 3.1.32.15 1996/09/11 16:49:34 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_arp.c,v $
 *------------------------------------------------------------------
 * Support for Frame Relay ARP, RARP, and Inverse ARP
 *
 * Feburary 1996, Charles Poe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_arp.c,v $
 * Revision 3.1.32.15  1996/09/11  16:49:34  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.32.14  1996/08/28  13:21:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.32.13  1996/08/08  18:01:03  ccpoe
 * CSCdi61581:  Redundant code in fr_arp.c.
 * Branch: California_branch
 *
 * Revision 3.1.32.12  1996/07/23  13:31:14  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.32.11  1996/07/02  23:11:21  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.1.32.10  1996/06/28  23:31:55  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.32.9  1996/06/18  23:21:40  widmer
 * CSCdi59768:  DLCI is not validated in EZSetup
 * Branch: California_branch
 *
 * Revision 3.1.32.8  1996/06/18  01:49:58  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1.32.7  1996/06/17  23:39:13  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.1.32.6  1996/06/01  01:32:56  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.1.32.5  1996/05/31  18:19:23  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.1.32.4  1996/05/17  21:27:49  ccpoe
 * CSCdi57540:  Frame relay inverse ARP is not working for IPX
 * Branch: California_branch
 *
 * Revision 3.1.32.3  1996/05/10  21:30:01  ccpoe
 * CSCdi57316:  Frame relay inverse ARP for IPX could lose buffers
 * Branch: California_branch
 *
 * Revision 3.1.32.2  1996/05/08  05:43:15  ccpoe
 * CSCdi56330:  serial_setencap in network.c broke modularity. Stop all
 * the timers and leave the encapulation untouched for later reuse.
 * Branch: California_branch
 *
 * Revision 3.1.32.1  1996/04/25  23:23:11  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.38.1  1996/04/24  04:11:14  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/02/10  00:42:26  ccpoe
 * Frame Relay ARP subsystem
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "config.h"
#include "../ui/debug.h"
#include "serial.h"
#include "dialer.h"
#include "address.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "serial_debug.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "parser.h"
#include "parser_defs_frame.h"
#include "frame_relay.h"
#include "fr_switch.h"
#include "fr_arp.h"
#include "../llc2/llc2.h"
#include "../wan/dialer_registry.h"
#include "../wan/fr_registry.h"
#include "mgd_timers.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */


/*
 * ARP process id
 */
static pid_t fr_arp_pid;

/*
 * ARP parent timer pointer
 */
mgd_timer *fr_inarp_timer;
 
/*
 * ARP queue
 */
static watched_queue *fr_arpQ;		/* ARP/RARP/INARP input Q */

/*
 * Storage for ARP/INARP 
 */
static addrtype fr_spro;
static ushort fr_sdlci;


#define FR_ARP_MAJVERSION 1
#define FR_ARP_MINVERSION 0
#define FR_ARP_EDITVERSION 1

SUBSYS_HEADER(fr_arp, FR_ARP_MAJVERSION, FR_ARP_MINVERSION,
	      FR_ARP_EDITVERSION, fr_arp_init, SUBSYS_CLASS_PROTOCOL,
              "seq: frame_relay",
	      "req: frame_relay");


static boolean ip_subsys;
static boolean ipx_subsys;
static boolean xns_subsys;
static boolean vines_subsys;
static boolean decnet_subsys;
static boolean atalk_subsys;


/*
 * fr_arp_idb_init
 *
 *	Initialize and create fr_idb arp specific data structures
 *      1. Creat and start the INarp mgd_timer for the hwidb
 *      2. Initialize
 *
 *	Inverse ARP timer for each hwidb will start with
 *	DEFAULT_INARP_INTERVAL
 *      fr_mgd_timer_interval() may set it to user configured value by demand.
 */
static void fr_arp_idb_init(hwidbtype *hwidb)
{

    mgd_timer *inarp_idb_timer;
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;
    hwidb->firstsw->fr_arp_enable = TRUE;
    hwidb->firstsw->fr_inarp_enable = TRUE;
    
    /*
     * Check if it's a rentry
     */
    if (fr_idb->fr_arp) {
	return;
    }
    inarp_idb_timer = malloc_named(sizeof(mgd_timer), "FR InArp Timer");
    fr_idb->fr_arp = malloc_named(sizeof(struct fr_idb_arp), "FR IDB Arp");
    
    if (inarp_idb_timer && fr_idb->fr_arp) {
	fr_idb->fr_arp->fr_inarp_interval = DEFAULT_INARP_INTERVAL;
	mgd_timer_init_leaf(inarp_idb_timer, fr_inarp_timer,
			    FR_INARP_TIMER, hwidb, FALSE);
	if (hwidb->state == IDBS_UP)
	    mgd_timer_start(inarp_idb_timer,
			fr_idb->fr_arp->fr_inarp_interval * ONESEC);
	fr_link_inarp_timer(hwidb, inarp_idb_timer);
    } else {
	buginf("\n%% FR arp timer allocated before fr_idb_arp");
	return;
    }

    fr_idb->fr_arp->extended_inarp = FALSE;
    fr_idb->fr_arp->skip_inarp = FALSE;
    
    if (subsys_find_by_name("iphost"))
	ip_subsys = TRUE;
    if (subsys_find_by_name("xns"))
	xns_subsys = TRUE;
    if (subsys_find_by_name("vines"))
	vines_subsys = TRUE;
    if (subsys_find_by_name("decnet"))
	decnet_subsys = TRUE;
    if (subsys_find_by_name("atalk"))
	atalk_subsys = TRUE;
    if (subsys_find_by_name("ipx"))
	ipx_subsys = TRUE;
}



/*
 * fr_arp_idb_uninit
 *	Remove all the created data structure when frame relay encaps is
 *	configured
 */
static void fr_arp_idb_uninit(hwidbtype *hwidb)
{
    mgd_timer *inarp_idb_timer;
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;
    if (fr_idb == NULL) {
	return;
    }
    
    fr_idb->fr_arp->fr_inarp_interval = 0;
    inarp_idb_timer = fr_get_inarp_timer(hwidb);
    if (inarp_idb_timer) {
	mgd_timer_stop(inarp_idb_timer);
	free(inarp_idb_timer);
    }
    free(fr_idb->fr_arp);
    fr_idb->fr_arp = NULL;
}



/*
 * fr_inarp_timer_interval
 *	Set the timer interval for inverse ARP timer
 */
static void fr_inarp_timer_interval(hwidbtype *hwidb, ulong *interval, ulong subtype)
{
    mgd_timer *inarp_idb_timer;
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;

    if (*interval == 0) {
	*interval = fr_idb->fr_arp->fr_inarp_interval;
	return;
    }
    inarp_idb_timer = fr_get_inarp_timer(hwidb);
    if (inarp_idb_timer) {
	mgd_timer_update(inarp_idb_timer, *interval * ONESEC);
	fr_idb->fr_arp->fr_inarp_interval = *interval;
    } else {
	/* We are dead meat. we are trying to set inarp timer interval
	 * but no inarp timer is found for this hwidb
	 */
	buginf("Set interval on non-exist INARP timer on %s\n",
	       hwidb->firstsw->namestring);
    }
}


static void fr_inarp_timer_onoff (hwidbtype *hwidb, boolean ON_OFF)
{
    fr_idb_struct_type *fr_idb;
    mgd_timer *target_timer;

    fr_idb = NULL;
    if (hwidb)
	fr_idb = hwidb->frame_relay_stuff;
    if (!fr_idb)
	return;
    target_timer =  fr_idb->fr_arp->fr_inarp_timer;
   
    if (ON_OFF) {
	if (!mgd_timer_running(target_timer)) {
	    mgd_timer_start(target_timer,
			    fr_idb->fr_arp->fr_inarp_interval * ONESEC);
	}	
    } else {
	if (mgd_timer_running(target_timer)) {
	    mgd_timer_stop(target_timer);
	}
    }
}


/*
 * fr_arp_start
 * 	Create the FR ARP process and arp master timer
 */
static void fr_arp_start(void)
{
    fr_arp_pid = process_create(fr_arp_background, "FR ARP", NORMAL_STACK,
				PRIO_LOW);
}


/*
 * fr_link_chk
 *
 * Return TRUE if link_type protocol subsys exists and running on the
 * specified interface; FALSE otherwise.
 */
static boolean fr_link_chk(enum LINK link, idbtype *idb)
{
    return RUNNING_IF(link, idb);
}


/*
 * fr_inarp_dlci_get
 *	Return the inverse arp status. e.g. if IP inverse ARP is enabled?
 */
static void fr_inarp_dlci_get(fr_idb_struct_type *fr_idb, ushort dlci,
			 ushort *status)
{
    
    *status = fr_idb->fr_arp->inarp_status[dlci];
}


/*
 * fr_inarp_dlci_set
 *	To set the inverse ARP status
 */
static void fr_inarp_dlci_set(fr_idb_struct_type *fr_idb, ushort dlci,
			       ushort *status)
{
    fr_idb->fr_arp->inarp_status[dlci] = *status;
}



/*
 * fr_arp_int_get
 *	Call to get interface status for fr arp and inverse arp
 */
static void fr_arp_int_get(idbtype *swidb, ulong arp_type, boolean *status)
{
    switch (arp_type) {
    case FR_CISCO_ARP:
    case FR_RFC_ARP:
	*status = swidb->fr_arp_enable;
	break;
    case FR_INARP:
	*status = swidb->fr_inarp_enable;
	break;
    default:
	*status = FALSE;
    }
}



/*
 * fr_arp_int_set
 *	To set the interface ARP or inverse ARP status
 */
static void fr_arp_int_set(idbtype *swidb, ulong arp_type, boolean *status)
{
    switch (arp_type) {
    case FR_CISCO_ARP:
    case FR_RFC_ARP:
	swidb->fr_arp_enable = *status;
	break;
    case FR_INARP:
	swidb->fr_inarp_enable = *status;
	break;
    }
}
    

/*
 * fr_do_inarp_send
 * Check PVCs to see if inverse ARP packets need to be sent
 * - Upon entry, the argument idb points to the real interface for packets.
 */
static void fr_do_inarp_send (hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    idbtype *swidb;
    ushort inarp_status;
    boolean int_status;
    int i;
    
    fr_idb = idb->frame_relay_stuff;
    if (!fr_idb)
        return;

    if (!hw_interface_up(idb))
        return;

    for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
	    continue;
	swidb = fr_pvc->idb;	/* where this pvc lives */
	if (is_p2p(swidb))
	    continue;
	fr_arp_int_get(swidb, FR_INARP, &int_status);
	if (int_status != TRUE)
            continue;
	if (fr_pvc->pvc_type == SVC_DYNAMIC)
            continue;
	if (fr_pvc->pvc_type == PVC_AUTOINSTALL)
	    continue;
	if ((fr_pvc->pvc_usage != PVC_LOCAL ) &&
	    (fr_pvc->pvc_usage != PVC_NOTSET ))
	    continue;
	if (fr_pvc->fr_pr_enabled)
	    continue;
	if (!IS_PVC_ACTIVE_BIT_SET(fr_pvc->pvc_status))
	    continue;
	fr_inarp_dlci_get(fr_idb, i, &inarp_status);
	if (!(inarp_status & FR_ALL_IN_ARP_ENABLED))
	    continue;
	if (!fr_link_chk(LINK_IP, swidb) &&
	    (fr_idb->map_dynamic[i] & FR_IP_DYNAMIC)) {
	    fr_clear_dynamic_entries(idb, LINK_IP,i,FR_MAP);
	    fr_clear_dynamic_entries(idb, LINK_COMPRESSED_TCP,i,FR_MAP);
	} else 	if (fr_link_chk(LINK_IP, swidb) &&
		    !(fr_idb->map_dynamic[i] & FR_IP_DYNAMIC) &&
		    !(fr_idb->map_status[i] & FR_IP_MAPPED) &&
		    (inarp_status & FR_IP_IN_ARP_ENABLED) &&
		    !BRIDGING_IF(swidb, LINK_IP))
	    fr_send_inarp_req(swidb, idb, LINK_IP, i);
	if (!fr_link_chk(LINK_XNS, swidb) &&
	    (fr_idb->map_dynamic[i] & FR_XNS_DYNAMIC)) {
	    fr_clear_dynamic_entries(idb, LINK_XNS,i,FR_MAP);
	} else 	if (fr_link_chk(LINK_XNS, swidb) &&
		    !(fr_idb->map_dynamic[i] & FR_XNS_DYNAMIC) &&
		    !(fr_idb->map_status[i] & FR_XNS_MAPPED) &&
		    (inarp_status & FR_XNS_IN_ARP_ENABLED))
	    fr_send_inarp_req(swidb, idb, LINK_XNS, i);
	if (!fr_link_chk(LINK_NOVELL, swidb) &&
	    (fr_idb->map_dynamic[i] & FR_IPX_DYNAMIC)) {
	    fr_clear_dynamic_entries(idb, LINK_NOVELL,i,FR_MAP);
	} else 	if (fr_link_chk(LINK_NOVELL, swidb) &&
		    !(fr_idb->map_dynamic[i] & FR_IPX_DYNAMIC) &&
		    !(fr_idb->map_status[i] & FR_IPX_MAPPED) &&
		    (inarp_status & FR_IPX_IN_ARP_ENABLED))
	    fr_send_inarp_req(swidb, idb, LINK_NOVELL, i);
	if (!fr_link_chk(LINK_APPLETALK, swidb) &&
	    (fr_idb->map_dynamic[i] & FR_AT_DYNAMIC)) {
	    fr_clear_dynamic_entries(idb, LINK_APPLETALK,i,FR_MAP);
	} else if (fr_link_chk(LINK_APPLETALK, swidb) &&
		    !(fr_idb->map_dynamic[i] & FR_AT_DYNAMIC) &&
		    !(fr_idb->map_status[i] & FR_AT_MAPPED) &&
		    (inarp_status & FR_AT_IN_ARP_ENABLED))
	    fr_send_inarp_req(swidb, idb, LINK_APPLETALK, i);
	if (!fr_link_chk(LINK_DECNET, swidb) &&
	    (fr_idb->map_dynamic[i] & FR_DNET_DYNAMIC)) {
	    fr_clear_dynamic_entries(idb, LINK_DECNET,i,FR_MAP);
	} else if (fr_link_chk(LINK_DECNET, swidb) &&
		    !(fr_idb->map_dynamic[i] & FR_DNET_DYNAMIC) &&
		    !(fr_idb->map_status[i] & FR_DNET_MAPPED) &&
		    (inarp_status & FR_DNET_IN_ARP_ENABLED))
	    fr_send_inarp_req(swidb, idb, LINK_DECNET, i);
	if (!fr_link_chk(LINK_VINES, swidb) &&
	    (fr_idb->map_dynamic[i] & FR_VINES_DYNAMIC)) {
	    fr_clear_dynamic_entries(idb, LINK_VINES,i,FR_MAP);
	} else if (fr_link_chk(LINK_VINES, swidb) &&
		   !(fr_idb->map_dynamic[i] & FR_VINES_DYNAMIC) &&
		    !(fr_idb->map_status[i] & FR_VINES_MAPPED) &&
		   (inarp_status & FR_VINES_IN_ARP_ENABLED))
	    fr_send_inarp_req(swidb, idb, LINK_VINES, i);
    }
}


void fr_inarp_send (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;
    fr_idb->fr_arp->skip_inarp = TRUE;
    fr_do_inarp_send(hwidb);
}


/*
 * fr_inarp_timeout
 *	Periodically, frame relay needs to check if inverse ARP needs to
 *	send out. 
 */
static void fr_inarp_timeout(void)
{
    mgd_timer *expired_idb_timer;
    hwidbtype *hwidb;
    fr_idb_struct_type *fr_idb;
    ulong interval;

    while (mgd_timer_expired(fr_inarp_timer)) {
	expired_idb_timer = mgd_timer_first_expired(fr_inarp_timer);
	if (expired_idb_timer == NULL) {
	    /*
	     * shouldn't come here since we woke up by a timer event and
	     * no timer expired. Stop the timer and return.
	     */
	    buginf("fr_arp: INarp timer disturbed! \n");
	    mgd_timer_stop(expired_idb_timer);
	    return;
	}
	hwidb = mgd_timer_context(expired_idb_timer);
	if (if_is_ddr(hwidb)) {
	    /* get the real hwidb depending on which is connected */
	    hwidb = reg_invoke_dialer_get_conn_member(hwidb);
	}
	fr_idb = NULL;
	if (hwidb && (fr_idb = hwidb->frame_relay_stuff)) {
	    interval = fr_idb->fr_arp->fr_inarp_interval;
	    mgd_timer_update(expired_idb_timer, interval * ONESEC);
	    if (!fr_idb->fr_arp->skip_inarp) {
		fr_do_inarp_send(hwidb);
	    }
	    fr_idb->fr_arp->skip_inarp = FALSE;
	} else {
	    /*
	     * Some one change or remove the hwidb without deencapsulation
	     * Stop the timer, this is all what we can do 
	     */
	    mgd_timer_stop(expired_idb_timer);
	}
    }
}



/*
 * link_2_fr_arp_type
 */
static int link_2_fr_arp_type (int linktype)
{
    switch (linktype) {
    case LINK_IP:                  return(FR_PR_IP);
    case LINK_DECNET:              return(FR_PR_DECNET);
    case LINK_DECNET_ROUTER_L1:    return(FR_PR_DECNET);
    case LINK_DECNET_ROUTER_L2:    return(FR_PR_DECNET);
    case LINK_DECNET_PRIME_ROUTER: return(FR_PR_DECNET);
    case LINK_DECNET_NODE:         return(FR_PR_DECNET);
    case LINK_XNS:                 return(FR_PR_XNS);
    case LINK_APPLETALK:           return(FR_PR_APPLETALK);
    case LINK_NOVELL:              return(FR_PR_NOVELL);
    case LINK_APOLLO:              return(FR_PR_APOLLO);
    case LINK_VINES:               return(FR_PR_VINES);
    case LINK_CLNS:                return(FR_PR_CLNS);
    case LINK_CLNS_BCAST:          return(FR_PR_CLNS);
    case LINK_CLNS_ALL_ES:         return(FR_PR_CLNS);
    case LINK_CLNS_ALL_IS:         return(FR_PR_CLNS);
    }
    return(FR_PR_NONE);
}

/*
 * fr_arp_2_link_type
 */
static int fr_arp_2_link_type (int linktype)
{
    switch (linktype) {
      case FR_PR_IP:            return(LINK_IP);
      case FR_PR_DECNET:        return(LINK_DECNET);
      case FR_PR_XNS:           return(LINK_XNS);
      case FR_PR_APPLETALK:     return(LINK_APPLETALK);
      case FR_PR_NOVELL:        return(LINK_NOVELL);
      case FR_PR_APOLLO:        return(LINK_APOLLO);
      case FR_PR_VINES:         return(LINK_VINES);
      case FR_PR_CLNS:          return(LINK_CLNS);
    }
      return(LINK_ILLEGAL);
}

/*
 * fr_ip_link_subtype
 *
 * Given a DLCI and an hwidb and knowing the protocol address of the 
 * source interface, determine the details of the link. This info is then 
 * used to fill in the protocol field during the inverse arp process. The 
 * receiving end will then create a dynamic map with characteristics based 
 * on the information provided. 
 */
uint fr_ip_link_subtype (
			 hwidbtype *idb,
			 ushort dlci, 
			 ipaddrtype *proto_addr)
{
    frmaptype *p;
    int i;
    uint flags;

    for (i=0; i<RELAYHASH; i++) {
	for (p = relaytable[i]; p; p = p->next) {
	    if ((idb == p->idb->hwptr) &&
		(p->dlci_num == dlci) &&
	    	((bcmp((((uchar *)(&p->proto_addr))+2), (uchar *)proto_addr,
		       sizeof(ipaddrtype)) == 0))) {
		flags = p->compression_flags;
		if (p->link == LINK_ILLEGAL)
		    return(LINK_IP);
		if (p->link == LINK_IP) {
		    if (!flags || (flags & FR_MAP_NO_COMPRESS)) {
			return(LINK_IP);
		    } else {
			if ((flags & FR_MAP_INHERIT_PASSIVE)) {
			    return(LINK_IP_INHERIT_PASSIVE_COMPR);
			} else if (flags & FR_MAP_INHERIT_COMPRESSION) {
			    return(LINK_IP_INHERIT_COMPR);
			} else {
			    return(LINK_IP);
			}
		    }
		} else if (p->link == LINK_COMPRESSED_TCP) {
		    if (!flags)
			return(LINK_COMPRESSED_TCP);
		    if ((flags & FR_MAP_PASSIVE) ||
			(flags & FR_MAP_INHERIT_PASSIVE)) {
			return(LINK_COMPR_TCP_AND_PASSIVE);
		    } else if (flags & FR_MAP_ACTIVE) {
			return(LINK_COMPR_TCP_AND_ACTIVE);
		    } else {
			return(LINK_COMPRESSED_TCP);
		    }
		}
	    }
	}
    }
    return(LINK_IP);
}


/*
 * fr_process_std_arp
 * Make a standard ARP packet look like cisco FR ARP
 */
static boolean fr_process_std_arp (paktype *pak)
{
    fr_rfc_arp *fr_rfc = NULL;
    fr_decnet_inarptype *fr_decnet;
    fr_xns_inarptype *fr_xns;
    fr_at_inarptype *fr_at;
    fr_nov_inarptype *fr_novell;
    fr_vines_inarptype *fr_vines;
    idbtype *idb;
    hwidbtype *hwidb;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;
    uint link = LINK_ILLEGAL, options = NO_OPTIONS, encaps;
    ulong novell_net;
    frame_relay_hdr *hdr = NULL;
    snap_hdr *snap;
    ushort opcode;
    addrtype temp;
    frmaptype *map;
    long blankaddr;
    boolean valid_addr;
    uint dlci;

    blankaddr = 0;
    valid_addr = TRUE;
    fr_sdlci = GETSHORT(pak->datagramstart);
    idb = pak->if_input;
    fr_idb = idb->hwptr->frame_relay_stuff;
    fr_rfc = (fr_rfc_arp *)pak->network_start;
    encaps = fr_idb->fr_ietf_encaps;
    opcode = fr_rfc->opcode;
    /*
     * If this PVC isn't the HIGH priority PVC and is part of a DLCI priority
     * group, it shouldn't be responding to inverse ARP.  Drop the request.
     * This condition arises when the remote box is running a release that
     * doesn't support DLCI prioritization.
     * However, accept a reply since we have no idea what DLCI an outbound
     * request (and the corresponding reply) might have been prioritized on.
     */
    dlci = DLCI_TO_NUM(fr_sdlci);
    if (!(fr_pvc = fr_idb->fr_dlci_tab[dlci]))
	return(FALSE);
    if (fr_pvc->fr_pr_enabled && (opcode == FR_INARP_REQ))
	return(FALSE);
    idb = fr_pvc->idb;

    switch (fr_rfc->arpro) {
      case FR_PR_IP_INHERIT_COMPR:
      case FR_PR_IP_NP_COMPR:
      case FR_PR_P_TCP_COMPR:
      case FR_PR_IP_P_COMPR:
      case FR_PR_NP_TCP_COMPR:
      case FR_PR_TCP_COMP:
      case FR_PR_IP:
	if (!fr_link_chk(LINK_IP, idb))
	    return(FALSE);
	fr_spro.type = ADDR_IP;
	fr_spro.length = ADDRLEN_IP;
	memcpy(&fr_spro.addr, &fr_rfc->ipspro, sizeof(fr_rfc->ipspro));
	if ((fr_rfc->opcode == FR_INARP_REQ) ||
	    (fr_rfc->opcode == OP_REQ)) {
	    fr_rfc->opcode++;
	    link = fr_ip_link_subtype(idb->hwptr, dlci, &fr_rfc->ipspro);
	    memcpy(&fr_rfc->iptpro, &fr_spro.addr, sizeof(fr_rfc->ipspro));
	    memcpy(&fr_rfc->ipspro, &idb->ip_address, sizeof(fr_rfc->ipspro));
 
 	    switch (link) {
 		/*
 		 * NOTE: 
 		 * After "fr_rfc->arpro" is set in each of the 
 		 * following cases, the variable "link" must be
 		 * reset to a known value or else a bogus
 		 * entry will be entered into the map table. 
 		 */
 		case LINK_IP:
 		    fr_rfc->arpro = FR_PR_IP;
 		    break;
 		case LINK_COMPRESSED_TCP:
 		    fr_rfc->arpro = FR_PR_TCP_COMP;
 		    break;
 		case LINK_IP_INHERIT_COMPR:
 		    fr_rfc->arpro = FR_PR_IP_INHERIT_COMPR;
 		    link = LINK_IP;
 		    break;
		case LINK_IP_INHERIT_ACTIVE_COMPR:
 		    fr_rfc->arpro = FR_PR_IP_NP_COMPR;
 		    link = LINK_IP;
 		    break;
 		case LINK_IP_INHERIT_PASSIVE_COMPR:
 		    fr_rfc->arpro = FR_PR_IP_P_COMPR;
 		    link = LINK_IP;
 		    break;
 		case LINK_COMPR_TCP_AND_PASSIVE:
 		    fr_rfc->arpro = FR_PR_P_TCP_COMPR;
 		    link = LINK_COMPRESSED_TCP;
 		    break;
 		case LINK_COMPR_TCP_AND_ACTIVE:
 		    fr_rfc->arpro = FR_PR_NP_TCP_COMPR;
 		    link = LINK_COMPRESSED_TCP;
 		    break;
 		default: 
		    return(FALSE);
	    }
 	} else if (fr_rfc->opcode == FR_INARP_REPLY) {
 
 		switch(fr_rfc->arpro) {
 		    case FR_PR_TCP_COMP: 
 			link = LINK_COMPRESSED_TCP;
 			options = 0;
 			encaps = FR_ENCAPS_CISCO;
 			break;
 		    case FR_PR_IP:
 			link = LINK_IP;
 			options = 0;
 			break;
 		    case FR_PR_IP_INHERIT_COMPR:
 			link = LINK_IP;
 			options |= FR_MAP_INHERIT_COMPRESSION; 
 			encaps = FR_ENCAPS_CISCO;
 			break;
 		    case FR_PR_IP_NP_COMPR:
 			link = LINK_IP;
 			options |= (FR_MAP_INHERIT_COMPRESSION | 
 						FR_MAP_ACTIVE); 
 			encaps = FR_ENCAPS_CISCO;
 			break;
 		    case FR_PR_IP_P_COMPR:
 			link = LINK_IP;
 			options |= (FR_MAP_INHERIT_COMPRESSION | 
 						FR_MAP_PASSIVE); 
 			encaps = FR_ENCAPS_CISCO;
 			break;
 		    case FR_PR_P_TCP_COMPR: 
 			link = LINK_COMPRESSED_TCP;
 			options |= FR_MAP_PASSIVE; 
 			encaps = FR_ENCAPS_CISCO;
 			break;
 		    case FR_PR_NP_TCP_COMPR:		
 			link = LINK_COMPRESSED_TCP;
 			options |= FR_MAP_ACTIVE; 
 			encaps = FR_ENCAPS_CISCO;
 			break;
 		    default: 
 			return(FALSE);
 		}
 	}
 	break;
      case TYPE_DECNET:
	fr_decnet = (fr_decnet_inarptype *)pak->network_start;
	if (!fr_link_chk(LINK_DECNET, idb))
	    return(FALSE);
	link = LINK_DECNET;
	fr_spro.type = ADDR_DECNET;
	fr_spro.length = ADDRLEN_DECNET;
	memcpy((uchar *)&fr_spro.addr,
		(uchar *)&fr_decnet->dnspro, sizeof(fr_decnet->dnspro));
	if (fr_decnet->opcode == FR_INARP_REQ) {
	    fr_decnet->opcode = FR_INARP_REPLY;
	    reg_invoke_proto_address(LINK_DECNET, idb, pak, &temp);
	    memcpy((uchar *)&fr_decnet->dntpro, 
			(uchar *)&fr_spro.addr, sizeof(fr_decnet->dnspro));
	    memcpy((uchar *)&fr_decnet->dnspro,
			(uchar *)&temp.addr.decnet_address, sizeof(fr_decnet->dnspro));
	}
	break;
      case TYPE_XNS:
	fr_xns = (fr_xns_inarptype *)pak->network_start;
	if (!fr_link_chk(LINK_XNS, idb) || !fr_xns->xnsspro.net)
	    return(FALSE);
	link = LINK_XNS;
	fr_spro.type = ADDR_XNS;
	fr_spro.length = ADDRLEN_XNS;
	ieee_copy((uchar *)&fr_xns->xnsspro.host,
		  (uchar *)&fr_spro.xns_addr.host);
	fr_spro.xns_addr.net = fr_xns->xnsspro.net;
	if (fr_xns->opcode == FR_INARP_REQ) {
	    fr_xns->opcode = FR_INARP_REPLY;
	    memcpy(&fr_xns->xnstpro, &fr_spro.addr, sizeof(fr_xns->xnsspro));
	    reg_invoke_proto_address(LINK_XNS, idb, pak, &temp);

	    fr_xns->xnsspro.net = temp.xns_addr.net;
	    ieee_copy(temp.xns_addr.host, (uchar *)&fr_xns->xnsspro.host);
	}
	break;
      case TYPE_NOVELL1:
	fr_novell = (fr_nov_inarptype *)pak->network_start;
	if (!fr_link_chk(LINK_NOVELL, idb) || !fr_novell->novspro.net)
	    return(FALSE);
	novell_net = idb_sw_get_ipxnet(idb);
	if (novell_net == 0)
	    return(FALSE);
	link = LINK_NOVELL;
	fr_spro.type = ADDR_NOVELL;
	fr_spro.length = ADDRLEN_NOVELL;
	ieee_copy((uchar *)&fr_novell->novspro.host,
		  (uchar *)&fr_spro.nov_addr.host);
	fr_spro.nov_addr.net = fr_novell->novspro.net;
	if (fr_novell->opcode == FR_INARP_REQ) {
	    fr_novell->opcode = FR_INARP_REPLY;
	    memcpy(&fr_novell->novtpro, &fr_spro.addr, sizeof(fr_novell->novspro));
	    fr_novell->novspro.net = novell_net;
	    ieee_copy(idb_sw_get_ipxnode(idb),
		      (uchar *)&fr_novell->novspro.host);
	}
	break;
      case TYPE_ETHERTALK:
	fr_at = (fr_at_inarptype *)pak->network_start;
	if (!fr_link_chk(LINK_APPLETALK, idb))
	    return(FALSE);
	link = LINK_APPLETALK;
	fr_spro.type = ADDR_APPLE;
	fr_spro.length = ADDRLEN_APPLE;
	memcpy((uchar *)&fr_spro.addr, (uchar *)&fr_at->atspro, ADDRLEN_APPLE);
	if (fr_at->opcode == FR_INARP_REQ) {
	    fr_at->opcode = FR_INARP_REPLY;
	    memcpy((uchar *)&fr_at->attpro, (uchar *)&fr_at->atspro, ADDRLEN_APPLE);
	    reg_invoke_proto_address(LINK_APPLETALK, idb, pak, &temp);
	    fr_at->atspro.host = temp.apple_addr.host;
	    fr_at->atspro.net = temp.apple_addr.net;
	}
	break;
      case TYPE_VINES:
	fr_vines = (fr_vines_inarptype *)pak->network_start;
	if (!fr_link_chk(LINK_VINES, idb))
	    return(FALSE);
	link = LINK_VINES;
	fr_spro.type = ADDR_VINES;
	fr_spro.length = ADDRLEN_VINES;
	fr_spro.vines_addr.net = GETLONG(&fr_vines->vnspro.net);
	fr_spro.vines_addr.host = GETSHORT(&fr_vines->vnspro.host);
	if (fr_vines->opcode == FR_INARP_REQ) {
	    fr_vines->opcode = FR_INARP_REPLY;
	    memcpy(&fr_vines->vntpro, &fr_spro.addr, sizeof(fr_vines->vnspro));
	    reg_invoke_proto_address(LINK_VINES, idb, pak, &temp);
	    PUTLONG(&fr_vines->vnspro.net, temp.vines_addr.net);
	    PUTSHORT(&fr_vines->vnspro.host, temp.vines_addr.host);
	}
	    
	break;
      default:
	return(FALSE);
    }
/* Drop packet since other end is misconfigured */
    if (BRIDGING_IF(idb, link))
	return(FALSE); 
    /* 
     * If a point-to-point link we already know the mapping
     * Bogus address check is done to make sure that bogus autoinstall
     * maps do not get propagated across the network. Also, in the case
     * of inarps relating to unnumbered interfaces, there is a chance that 
     * we will pick up a bogus map. 
     */
    if (bcmp(((uchar *)&(fr_spro.addr)),
			(uchar *)&blankaddr, sizeof(ipaddrtype)) == 0) {
	valid_addr = FALSE;
    }
    if ((!is_p2p(idb)) && valid_addr) {
	if (!(map = frame_relay_addr_lookup(idb, &fr_spro, link))) {
	    	fr_addr_enter(idb, &fr_spro, dlci, 
			      TRUE, encaps, 
			      FR_MAP_DYNAMIC,PVC_DYNAMIC,link,
			      options, NULL);
	} 
	else {
	    if (map->dlci_num != dlci && (map->map_type != FR_MAP_STATIC)) {
		frame_relay_map_delete(map);
		fr_addr_enter(idb, &fr_spro, dlci,
			      TRUE, encaps,
			      FR_MAP_DYNAMIC,PVC_DYNAMIC,link,
			      options, NULL);
	    }
	}
    }
    if (((opcode == FR_INARP_REQ) ||
	(opcode == OP_REQ)) && valid_addr) {
	/*
	 * This frame is guaranteed to be SNAP on input, and it needs to
	 * be SNAP on output.  Datagramstart is easy to compute, but
	 * datagramsize is a real pain, so just leave them as they were.
	 *
	 * We might have a problem here with SNAP0PAD.  We might
	 * have to do this correctly - adjust datagramstart and size
	 * (can subtract off pak->encapsize to get datagramstart)
	 * This might work here:
	 *
	 * pak->datagramstart = pak->network_start - SNAPENCAPBYTES;
	 * pak->datagramsize += SNAP_ENCAPBYTES - pak->encsize;
	 *
	 * Plus, make sure that you write the whole encapsulation
	 */
	if (frame_relay_packet_debug(idb, dlci))
	    buginf("\nFR: Sending INARP Reply on interface %s dlci %d for link %d(%s)",
	       idb->namestring, dlci, link, link2name(link));
	hdr = (frame_relay_hdr *)pak->datagramstart;
    	PUTSHORT(&hdr->dlci, fr_sdlci);
	PUTSHORT(&hdr->type, FR_SNAP_NLPID);
	snap = (snap_hdr *)hdr->snap_start;
	snap->control = FR_SNAP_CONTROL;
	ieee_copy_oui(zerosnap, snap->oui);
	if (if_is_ddr(idb->hwptr)) {
	    /* get the real hwidb depending on which is connected */
	    hwidb = reg_invoke_dialer_get_conn_member(idb->hwptr);
	    if (hwidb)
		idb = hwidb->firstsw;
		/* note that in case of fr-over-isdn idb is changed */
	}
	pak->if_output = idb;
	pak->fr_flags &= ~PAK_FR_BROADCAST;
	if (idb->hwptr->board_encap)
	    (*idb->hwptr->board_encap)(pak, idb->hwptr);
	datagram_out(pak);
    } else {
	datagram_done(pak);
    }
    return(TRUE);
}


/*
 * Handle any received ARP and INarp packets for this process
 */
static void fr_bkgnd_packets(void)
{

    ulong count;
    fr_arptype *fr_arpptr;
    fr_rfc_arp *fr_rfc;
    addrtype proto_addr;
    ushort s_dlci = 0;
    ushort fr_arppro;
    idbtype *idb;
    uint link = 0;
    frmaptype *map;
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *hdr;
    paktype *pak;

    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	pak = process_dequeue(fr_arpQ);
	if (pak == NULL) {
	    break;
	}
	idb_increment_rx_proto_counts(pak, ACCT_PROTO_ARP);

	fr_arpptr = (fr_arptype *)pak->network_start;
	idb = pak->if_input;
	fr_idb = (idb && hwidb_or_null(idb)) ?
		idb->hwptr->frame_relay_stuff : NULL;
	if (!fr_arpptr || !idb || !fr_idb) {
	    retbuffer(pak);
	    continue;
	}

	if (frame_relay_events_debug)
	    hexdump_packet(pak, (ulong)IDB_SERIAL);

	fr_rfc = (fr_rfc_arp *)fr_arpptr;
	memset(&fr_spro, 0, sizeof(addrtype));
	fr_sdlci = 0;
	
	/*
	 * Is this a Frame Relay INARP/ARP Packet?  The formats are
	 * different, so branch out here.
	 */
	if ((pak->linktype == LINK_ARP) &&
	    ((fr_rfc->opcode == FR_INARP_REQ) ||
	     (fr_rfc->opcode == FR_INARP_REPLY) ||
	     (fr_rfc->opcode == OP_REQ))) {
	    if (frame_relay_packet_debug(idb, IGNORE_DLCI))
		buginf("\n%s: frame relay INARP received", idb->namestring);
	    if (!fr_process_std_arp(pak))
		retbuffer(pak);
	    continue;
	}
	
	/*
	 * No, its a Frame Relay ARP Packet.
	 */
	if ((fr_idb->fr_multicast_status == FR_MULTICAST_NOTSET) ||
	    (fr_idb->fr_local_dlci == FR_UNDEFINED_DLCI)) {
	    retbuffer(pak);
	    if (frame_relay_packet_debug(idb, IGNORE_DLCI))
		buginf("\n%s: frame relay ARP dropped", idb->namestring);
	    continue;
	}

	switch (fr_arpptr->opcode) {
	case FR_OP_REQ:
	    fr_traffic.arpreqrcvd++;
	    memset(&proto_addr, 0, sizeof(addrtype));
	    
	    link = fr_arp_2_link_type(fr_arpptr->arpro);
	    if ((link == LINK_ILLEGAL) || !RUNNING(link)) {
		retbuffer(pak);
		break;
	    }
	    reg_invoke_proto_address(link, idb, pak, &proto_addr);
	    if (proto_addr.type == ADDR_ILLEGAL) {
		retbuffer(pak);
		break;
	    }
	    if (bcmp(&fr_arpptr->tpro, &proto_addr.addr, proto_addr.length) != 0) {
		retbuffer(pak);
		break;
	    }
	    
	    /*
	     * Extract the source protocol address, and swap
	     * addresses for the reply.  Note that the protocol
	     * address type and length fields were already set up
	     * when we got our own address.
	     */
	    s_dlci = fr_arpptr->shdw;
	    fr_arpptr->thdw = fr_arpptr->shdw;
	    fr_arpptr->shdw = DLCI_TO_NUM(fr_idb->fr_local_dlci);
	    memcpy(&proto_addr.addr, &fr_arpptr->spro, sizeof(union addresses));
	    memcpy(&fr_arpptr->spro, &fr_arpptr->tpro, sizeof(union addresses));
	    memcpy(&fr_arpptr->tpro, &proto_addr.addr, sizeof(union addresses));
	    
	    /*
	     * Do the rest of the work for the reply.
	     */
	    pak->linktype = LINK_FR_ARP;
	    pak->acct_proto = ACCT_PROTO_ARP;
	    fr_arpptr->opcode = FR_OP_REP;
	    pak->datagramstart=(uchar *)&fr_arpptr->arhrd -
		sizeof(frame_relay_hdr);
	    pak->datagramsize = FR_ARPBYTES + sizeof(frame_relay_hdr);
	    hdr = (frame_relay_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dlci, (NUM_TO_DLCI(fr_arpptr->thdw) | 
				  FRAME_RELAY_EA_BITS));
	    PUTSHORT(&hdr->type, link2serial(LINK_FR_ARP));
	    pak->if_output = idb;
	    if (frame_relay_events_debug) {
		buginf("\n%s(i): request rcvd %i %d",idb->namestring,
		       proto_addr.ip_addr, s_dlci);
		printf("\nsending reply");
		print_fr_arp(fr_arpptr);
	    }
	    pak->fr_flags &= ~PAK_FR_BROADCAST; 
	    if (frame_relay_debug)
		buginf("\n%s(o): dlci %d(0x%x), pkt type 0x%x, "
		       "datagramsize %d", idb->namestring,
		       DLCI_TO_NUM(GETSHORT(&hdr->dlci)), 
		       GETSHORT(&hdr->dlci), GETSHORT(&hdr->type), 
		       pak->datagramsize);
	    if (idb->hwptr->board_encap)
		(*idb->hwptr->board_encap)(pak,idb->hwptr);
	    datagram_out(pak);
	    fr_traffic.arprepsent++;
	    
	    /*
	     * Now record the senders info.
	     */
	    if (!frame_relay_addr_lookup(idb, &proto_addr, link)) {
		fr_addr_enter(idb, &proto_addr, s_dlci, FALSE,
			      FR_ENCAPS_CISCO, FR_MAP_DYNAMIC,
			      PVC_DYNAMIC, link, NO_OPTIONS, NULL);
	    }
	    break;
	case FR_OP_REP:
	    fr_traffic.arpreprcvd++;
	    fr_idb = pak->if_input->hwptr->frame_relay_stuff;
	    if (NUM_TO_DLCI(fr_arpptr->thdw) != fr_idb->fr_local_dlci) {
		retbuffer(pak);
		continue;
	    }
	    
	    memset(&proto_addr, 0, sizeof(addrtype));
	    memcpy((uchar *)&proto_addr.addr, (uchar *)&fr_arpptr->spro,
		  sizeof(union addresses));
	    s_dlci = fr_arpptr->shdw;
	    fr_arppro = fr_arpptr->arpro;
	    switch (fr_arppro) {
	    case FR_PR_IP:
		link = LINK_IP;
		proto_addr.type = ADDR_IP;
		proto_addr.length = ADDRLEN_IP;
		break;
	    case FR_PR_NOVELL:
		link = LINK_NOVELL;
		proto_addr.type = ADDR_NOVELL;
		proto_addr.length = ADDRLEN_NOVELL;
		break;
	    case FR_PR_DECNET:
		link = LINK_DECNET;
		proto_addr.type = ADDR_DECNET;
		proto_addr.length = ADDRLEN_DECNET;
		break;
	    case FR_PR_XNS:
		link = LINK_XNS;
		proto_addr.type = ADDR_XNS;
		proto_addr.length = ADDRLEN_XNS;
		break;
	    case FR_PR_APOLLO:
		link = LINK_APOLLO;
		proto_addr.type = ADDR_APOLLO;
		proto_addr.length = ADDRLEN_APOLLO;
		break;
	    case FR_PR_APPLETALK:
		link = LINK_APPLETALK;
		proto_addr.type = ADDR_APPLE;
		proto_addr.length = ADDRLEN_APPLE;
		break;
	    case FR_PR_VINES:
		link = LINK_VINES;
		proto_addr.type = ADDR_VINES;
		proto_addr.length = ADDRLEN_VINES;
		break;
	    default:
		link = LINK_ILLEGAL;
		break;
	    }
	    if (frame_relay_events_debug)
		buginf("\n%s(i): reply rcvd %i %d",idb->namestring,
		       proto_addr.ip_addr,s_dlci);
	    idb = get_netcfg_idb(idb);   /* Get configuration swidb */
	    map = frame_relay_addr_lookup(idb, &proto_addr, link);
	    if (! map)
		fr_addr_enter(idb, &proto_addr, s_dlci, FALSE,
			      FR_ENCAPS_CISCO, FR_MAP_DYNAMIC,
			      PVC_DYNAMIC, link, NO_OPTIONS, NULL);
	    else {
		map->dlci_num =  s_dlci;
		map->dlci = NUM_TO_DLCI(s_dlci);
	    }
	    retbuffer(pak);
	    break;
	}
    }
}


/*
 * fr_arp_background
 * handle address lookup on the Frame Relay network
 */
forktype fr_arp_background (void)
{
    ulong major, minor;

    fr_arpQ = create_watched_queue("FR ARP packet", 0, 0);

    process_watch_queue(fr_arpQ, ENABLE, RECURRING);
    process_watch_mgd_timer(fr_inarp_timer, ENABLE);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case QUEUE_EVENT:
		fr_bkgnd_packets();
		break;
	    case TIMER_EVENT:
		fr_inarp_timeout();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
  * print_fr_arp
  * Print a frame relay ARP packet
  */
void print_fr_arp (fr_arptype *ptr)
{
    buginf("\n  OP %x, hdw %x, protocol %x, hdw addr length %x, PR length %x",
	   ptr->opcode, ptr->arhrd, ptr->arpro, ptr->arhln, ptr->arpln);
    buginf("\n  src hdw %x, dest hdw %x src addr %x, dest addr %x",
	   ptr->shdw, ptr->thdw, ptr->spro, ptr->tpro);
}


/*
 * fr_dlci_inarp_init
 * Initialize a DLCI for inverse ARP
 */
void fr_dlci_inarp_init (
    idbtype *idb,
    ushort dlci,
    uint link,
    boolean enable)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    ushort inarp_status;

    if (dlci == 0 && link == 0) {
        if (enable)
            idb->fr_inarp_enable = TRUE;
        else
            idb->fr_inarp_enable = FALSE;
        return;
    } 
    fr_idb = idb->hwptr->frame_relay_stuff;

    if (!fr_idb)
        return;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (!fr_pvc) { /* initialize the PVC struct */
	fr_pvc = fr_pvc_init(idb,dlci,PVC_LOCAL,PVC_STATIC,FALSE, NULL);
	inarp_status = FR_ALL_IN_ARP_ENABLED;
	fr_inarp_dlci_set(fr_idb, dlci, &inarp_status);
    } else {
	if (fr_pvc->idb && fr_pvc->idb != idb) {
	    printf("\n%% PVC already assigned to interface %s",
		   fr_pvc->idb->namestring);
	    return;
	}
	fr_pvc->pvc_type = PVC_STATIC;
    }
    /* PVC now exists so just update in_arp status */
    fr_inarp_dlci_get(fr_idb, dlci, &inarp_status);
    switch ( link ) {
    case LINK_COMPRESSED_TCP:
    case LINK_IP:
	if ( enable )
	    inarp_status |= FR_IP_IN_ARP_ENABLED;
	else 
	    inarp_status &= ~FR_IP_IN_ARP_ENABLED; 
	break; 
    case LINK_NOVELL:
	if ( enable )
	    inarp_status |= FR_IPX_IN_ARP_ENABLED;
	else
	    inarp_status &= ~FR_IPX_IN_ARP_ENABLED; 
	break;
    case LINK_APPLETALK:
	if ( enable )
	    inarp_status |= FR_AT_IN_ARP_ENABLED;
	else
	    inarp_status &= ~FR_AT_IN_ARP_ENABLED; 
	break;
    case LINK_XNS:
	if ( enable )
	    inarp_status |= FR_XNS_IN_ARP_ENABLED;
	else
	    inarp_status &= ~FR_XNS_IN_ARP_ENABLED; 
	break;
    case LINK_DECNET:
	if ( enable )
	    inarp_status |= FR_DNET_IN_ARP_ENABLED;
	else
	    inarp_status &= ~FR_DNET_IN_ARP_ENABLED; 
	break; 
    case LINK_VINES:
	if ( enable )
	    inarp_status |= FR_VINES_IN_ARP_ENABLED;
	else
	    inarp_status &= ~FR_VINES_IN_ARP_ENABLED; 
	break;
    default:
	break;
    }
    fr_inarp_dlci_set(fr_idb, dlci, &inarp_status);
}


/*
 * fr_send_inarp_req
 * Send an inverse ARP request
 *
 * - For the case of FR-over-ISDN, at function entry,
 *	the sw idb parameter is where the dlci lives, and
 *	the hw idb parameter is where the packet goes out.
 */
void fr_send_inarp_req (
    idbtype *idb,
    hwidbtype *hwidb,
    uint link,
    ushort dlci)
{
    paktype *pak = NULL;
    paktype *null_pak = NULL;
    fr_rfc_arp *rfc;
    fr_xns_inarptype *xns;
    fr_at_inarptype *at;
    fr_nov_inarptype *novell;
    fr_decnet_inarptype *dnet;
    fr_vines_inarptype *vines;
    fr_idb_struct_type *fr_idb;
    ulong novell_net;
    addrtype temp;

    fr_idb = hwidb ? hwidb->frame_relay_stuff : NULL;
    if (!hwidb || !idb || !fr_idb) 
	return; 

    pak = getbuffer(FR_ARP_MAX_SIZE);
    if ( !pak ) {
	return;
    }

    switch (link) {
      case LINK_IP:
	reg_invoke_proto_address(LINK_IP, idb, pak, &temp);
	rfc = (fr_rfc_arp *)fr_rfc_arp_headstart(pak);
	rfc->arhrd = FR_ARHRD;
	rfc->arpro = PR_IP;
	rfc->arhln = ADDRLEN_FR;
	rfc->arpln = PL_IP;
	rfc->opcode = FR_INARP_REQ;
	rfc->ipshdw = 0;
	memcpy(&rfc->ipspro, &temp.ip_addr, sizeof(ipaddrtype));
	rfc->ipthdw  = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS;
	memset(&rfc->iptpro, 0, sizeof(ipaddrtype));
 	pak->datagramsize = RFBYTES;
	break;

    case LINK_XNS:
	reg_invoke_proto_address(LINK_XNS, idb, pak, &temp);
	xns = (fr_xns_inarptype *)fr_rfc_arp_headstart(pak);
	xns->arhrd = FR_ARHRD;
	xns->arpro = TYPE_XNS;
	xns->arhln = ADDRLEN_FR;
	xns->arpln = IEEEBYTES;
	xns->opcode = FR_INARP_REQ;
	xns->xnsshdw = 0;
	xns->xnsspro.net = temp.xns_addr.net;
	ieee_copy(temp.xns_addr.host, xns->xnsspro.host);
	xns->xnsthdw = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS;
	memset(&xns->xnstpro, 0, ADDRLEN_XNS);
	pak->datagramsize = XNS_INARPBYTES;
	break;

    case LINK_NOVELL:
	novell_net = idb_sw_get_ipxnet(idb);
	if (novell_net == 0) {
	    retbuffer(pak);
	    return;
	}
	reg_invoke_proto_address(LINK_NOVELL, idb, null_pak, &temp);
	novell = (fr_nov_inarptype *)fr_rfc_arp_headstart(pak);
	novell->arhrd = FR_ARHRD;
	novell->arhln = ADDRLEN_FR;
	novell->opcode = FR_INARP_REQ;
	novell->novshdw = 0;
	novell->novthdw = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS;
	memset(&novell->novtpro, 0, ADDRLEN_XNS);
	memset(&novell->novspro, 0, ADDRLEN_XNS);
	novell->arpln = ADDRLEN_XNS;
	novell->arpro = TYPE_NOVELL1;
	novell->novspro.net = novell_net;
	ieee_copy(idb_sw_get_ipxnode(idb), (uchar *)&novell->novspro.host);
	pak->datagramsize = XNS_INARPBYTES;
	break;

    case LINK_APPLETALK:
	at = (fr_at_inarptype *)fr_rfc_arp_headstart(pak);
	reg_invoke_proto_address(LINK_APPLETALK, idb, pak, &temp);
	at->arhrd = FR_ARHRD;
	at->arpro = TYPE_ETHERTALK;
	at->arhln = ADDRLEN_FR;
	at->arpln = ADDRLEN_APPLE;
	at->opcode = FR_INARP_REQ;
	at->atshdw = 0;
	at->atspro.host = temp.apple_addr.host;
	at->atspro.net = temp.apple_addr.net;
	at->atthdw = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS;
	memset(&at->attpro, 0, ADDRLEN_APPLE);
	pak->datagramsize = AT_INARPBYTES;
	break;

    case LINK_DECNET:
	reg_invoke_proto_address(LINK_DECNET, idb, pak, &temp);
	dnet = (fr_decnet_inarptype *)fr_rfc_arp_headstart(pak);
	dnet->arhrd = FR_ARHRD;
	dnet->arpro = TYPE_DECNET;
	dnet->arhln = ADDRLEN_FR;
	dnet->arpln = ADDRLEN_DECNET;
	dnet->opcode = FR_INARP_REQ;
	dnet->decnetshdw = 0;
	dnet->dnspro = temp.decnet_addr;
	dnet->decnetthdw = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS;
	memset(&dnet->dntpro, 0, ADDRLEN_DECNET);
	pak->datagramsize = DNET_INARPBYTES;
	break;

    case LINK_VINES:
	reg_invoke_proto_address(LINK_VINES, idb, pak, &temp);
	vines = (fr_vines_inarptype *)fr_rfc_arp_headstart(pak);
	vines->arhrd = FR_ARHRD;
	vines->arpro = TYPE_VINES;
	vines->arhln = ADDRLEN_FR;
	vines->arpln = ADDRLEN_VINES;
	vines->opcode = FR_INARP_REQ;
	vines->vnshdw = 0;
	vines->vnspro.net = temp.vines_addr.net;
	vines->vnspro.host = temp.vines_addr.host;
	vines->vnthdw = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS;
	memset(&vines->vntpro, 0, ADDRLEN_VINES);
	pak->datagramsize = VINES_INARPBYTES;
	break;

      default:
	break;
    }

    /* for fr-over-isdn get the real interface for if_output */
    pak->if_output = is_ddr(hwidb) ? hwidb->firstsw : idb;
    pak->linktype = LINK_ARP; 
    pak->datagramstart = fr_rfc_arp_headstart(pak);
    pak->enctype = ET_FRAME_RELAY;
    pak->acct_proto = ACCT_PROTO_ARP;
    fr_doencaps(idb, pak, NUM_TO_DLCI(dlci), pak->linktype, FR_ENCAPS_IETF,
		NULL_L3_PID, NULL); /* board_encap has been done when return */

    if (frame_relay_packet_debug(idb,dlci))
	buginf("\nFR: Sending INARP Request on interface %s dlci %d for link %d(%s)",
	       idb->namestring, dlci, link,link2name(link));

    if (!QUEUEFULL(&fr_idb->fr_br_packetQ))
	p_enqueue(&fr_idb->fr_br_packetQ,pak);
    else
	retbuffer(pak);
}


/*
 * Handle Frame Relay ARP from interrupt level
 */
void fr_arp_enqueue (paktype *pak)
{
    if (frame_relay_events_debug) {
	buginf("\n%s: FR ARP input",pak->if_input->namestring);
/*  	hexdump_packet(pak, (ulong)IDB_SERIAL);*/
    }
    if (fr_arpQ && pak->if_input->hwptr->enctype == ET_FRAME_RELAY)
	process_enqueue_pak(fr_arpQ, pak);
    else {
	protocol_discard(pak, TRUE);
    }
}


/*
 * send_fr_arp
 * Send a Frame Relay ARP request.
 */

static void send_fr_arp (
    idbtype *idb,
    paktype *pak,
    long linktype,
    uint forlink)
{
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *hdr;

    fr_idb = idb->hwptr->frame_relay_stuff;
    
    /*
     * Check interface state to make sure we don't lie.  If it isn't
     * up then we won't send anything.  datagram_out checks this.
     */
    if (!interface_up_simple(idb)) {
	retbuffer(pak);
	return;
    }

    pak->datagramsize += sizeof(frame_relay_hdr);
    pak->datagramstart -= sizeof(frame_relay_hdr);
    hdr = (frame_relay_hdr *)pak->datagramstart;
    PUTSHORT(&hdr->dlci, (fr_idb->fr_multicast_dlci | FRAME_RELAY_EA_BITS));
    PUTSHORT(&hdr->type, link2serial(LINK_FR_ARP));

    pak->if_output = idb;
    pak->enctype = idb->hwptr->enctype;
    pak->acct_proto = ACCT_PROTO_ARP;
    pak->fr_flags &= ~PAK_FR_BROADCAST; 
    if (frame_relay_debug)
	buginf("\n%s(o): dlci %d(0x%x), pkt type 0x%x, datagramsize %d",
	       idb->namestring, DLCI_TO_NUM(GETSHORT(&hdr->dlci)), 
	       GETSHORT(&hdr->dlci), GETSHORT(&hdr->type), 
	       pak->datagramsize);

    if (idb->hwptr->board_encap)
	(*idb->hwptr->board_encap)(pak,idb->hwptr);
    datagram_out((paktype *)pak);
    fr_traffic.arpreqsent++;
}

/*
 * fr_do_arp
 * do a Frame Relay ARP
 */
static void fr_do_arp (
	idbtype *idb, 
	addrtype *proto_addr, 
	uint protocol, 
	paktype *pak)
{
    paktype *arp_pak;
    addrtype source;
    fr_arptype *arp;
    fr_idb_struct_type *fr_idb;
    idbtype *netidb;
    int type;

    /*
     * For ISDN and dialers, idb points to the physical interface (B-ch, Ser),
     * but we need the network protocol information that resides on the rotary
     * group leader (D-ch, Dialer), so get it here.
     */
    netidb = get_pak_netidb(pak);
    type = link_2_fr_arp_type(protocol);
    reg_invoke_proto_address(protocol, netidb, pak, &source);
    if ((type == FR_PR_NONE) || (source.type == ADDR_ILLEGAL))
	return;

    fr_idb =  idb->hwptr->frame_relay_stuff;
    if (!(arp_pak = getbuffer(sizeof(fr_arptype))))
	return;
    arp = (fr_arptype *)arp_pak->network_start;
    memset(arp, 0, sizeof(fr_arptype));
    arp->arhrd = FR_SERIAL;
    arp->arhln = FR_PL;
    arp->arpro = type;
    arp->arpln = sizeof(union addresses);
    arp->opcode = FR_OP_REQ;
    arp->shdw = DLCI_TO_NUM(fr_idb->fr_local_dlci);
    memcpy(&arp->spro, &source.addr, sizeof(union addresses));
    arp->thdw  = 0;
    memcpy(&arp->tpro, &proto_addr->addr, sizeof(union addresses));
    arp_pak->linktype = LINK_FR_ARP;
    arp_pak->datagramstart = (uchar *)arp;
    arp_pak->datagramsize = sizeof(fr_arptype);
    send_fr_arp(idb, arp_pak, LINK_FR_ARP, protocol);
}


/*
 * fr_clear_inarp
 * Clear map entries which were created with INARP
 */
void clear_fr_inarp (parseinfo *csb)
{
    int i;
    frmaptype *p, *ptemp;

    for (i = 0; i < RELAYHASH; i++) {
        p = relaytable[i];
        while (p) {
            ptemp = p->next;
            if ( p->map_type == FR_MAP_DYNAMIC )
                frame_relay_map_delete(p);
            p = ptemp;
        }
    }
}


/*
 * fr_arp_commands
 *
 */
void fr_arp_commands (parseinfo *csb)
{
    idbtype *swidb = csb->interface;
    hwidbtype *idb = swidb->hwptr;
    fr_idb_struct_type *fr_idb = idb->frame_relay_stuff;
    fr_pvc_info_type *fr_pvc = NULL;

    if (idb->enctype != ET_FRAME_RELAY) {
	if (!csb->nvgen) {
	    printf("\n%%%s :Frame Relay not configured on interface", 
		   idb->hw_namestring);
	}
	return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
	case FR_IN_ARP:
	    {
		int i;
		ulong interval;
		ushort inarp_status;
		boolean int_status;
		
		if (!is_p2p(csb->interface)) {
		    fr_arp_int_get(swidb, FR_INARP, &int_status);
		    if (int_status == FALSE) {
			nv_write(TRUE, "no %s", csb->nv_command);
			break;
		    }
		    interval = 0;
		    fr_inarp_timer_interval(idb, &interval, 0);
		    if (interval != DEFAULT_INARP_INTERVAL && 
			!is_subinterface(swidb)) {
			nv_write(TRUE, "%s interval %d",
				 csb->nv_command, interval);
		    }
		    for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
			if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
			    continue;
			if (fr_pvc->idb != csb->interface)
			    continue;
			fr_inarp_dlci_get(fr_idb, i, &inarp_status);
			if (inarp_status == FR_ALL_IN_ARP_ENABLED )
			    continue;
			if (fr_pvc->fr_pr_enabled &&
			    (fr_pvc->fr_pr_dlci_group == 0))
			    continue;
			if (fr_pvc->route_flag)
			    continue;
			if (ip_subsys &&
			    !((inarp_status & FR_IP_IN_ARP_ENABLED ) ||
			      (fr_idb->map_status[i] & FR_IP_MAPPED )))
			    nv_write(TRUE, "no %s IP %d",
				     csb->nv_command, i);
			if (ipx_subsys &&
			    !((inarp_status & FR_IPX_IN_ARP_ENABLED ) ||
			      (fr_idb->map_status[i] & FR_IPX_MAPPED )))
			    nv_write(TRUE, "no %s NOVELL %d",
				     csb->nv_command, i);
			if (atalk_subsys &&
			    !((inarp_status & FR_AT_IN_ARP_ENABLED ) ||
			      (fr_idb->map_status[i] & FR_AT_MAPPED )))
			    nv_write(TRUE, "no %s APPLETALK %d",
				     csb->nv_command,i);
			if (xns_subsys &&
			    !((inarp_status & FR_XNS_IN_ARP_ENABLED ) ||
			      (fr_idb->map_status[i] & FR_XNS_MAPPED )))
			    nv_write(TRUE, "no %s XNS %d",
				     csb->nv_command, i);
			if (decnet_subsys &&
			    !((inarp_status & FR_DNET_IN_ARP_ENABLED) ||
			      (fr_idb->map_status[i] & FR_DNET_MAPPED )))
			    nv_write(TRUE, "no %s DECNET %d",
				     csb->nv_command, i);
			if (vines_subsys &&
			    !((inarp_status & FR_VINES_IN_ARP_ENABLED ) ||
			      (fr_idb->map_status[i] & FR_VINES_MAPPED )))
			    nv_write(TRUE, "no %s VINES %d",
				     csb->nv_command, i);
		    }
		}
	    }
	    break;
	}
	return;
    }
    
    switch (csb->which) {
    case FR_IN_ARP:
	{
	    ulong interval;
	    if (GETOBJ(int, 1) == FR_ARP_INTV) {
		if (!csb->sense) {
		    interval = DEFAULT_INARP_INTERVAL;
		} else {
		    interval = GETOBJ(int, 2);
		}
		fr_inarp_timer_interval(idb, &interval, 0);
	    } else {
		fr_dlci_inarp_init(swidb, GETOBJ(int,3), GETOBJ(int,10),
				   csb->set_to_default || csb->sense);
	    }
	}
	break;
    }
}

    

/*
 * fr_arp_init
 * Initialize the ARP support
 */
void fr_arp_init (subsystype *subsys)
{
    fr_inarp_timer = malloc_named(sizeof(mgd_timer), "FR Arp Init Timer");
    mgd_timer_init_parent(fr_inarp_timer, NULL);

    reg_add_fr_proc_create(fr_arp_start, "Create and start ARP Process" );
    reg_add_fr_idb_init(fr_arp_idb_init, "fr_arp_idb_init");
    reg_add_fr_idb_uninit(fr_arp_idb_uninit, "fr_arp_idb_uninit");
    reg_add_fr_mgd_timer_interval(FR_INARP_TIMER, fr_inarp_timer_interval,
				  "fr_inarp_timer_interval");
    reg_add_fr_inarp_send(fr_inarp_send, "fr_inarp_send");
    reg_add_fr_arp_send(fr_do_arp, "fr_do_arp");
    reg_add_raw_enqueue(LINK_FR_ARP, fr_arp_enqueue, "fr_arp_enqueue");
    reg_add_fr_inarp_dlci(FR_INARP_DLCI_GET, fr_inarp_dlci_get,
			   "fr_inarp_dlci_get");
    reg_add_fr_inarp_dlci(FR_INARP_DLCI_SET, fr_inarp_dlci_set,
			   "fr_inarp_dlci_set");
    reg_add_fr_arp_int(FR_ARP_INT_GET, fr_arp_int_get, "fr_arp_int_get");
    reg_add_fr_arp_int(FR_ARP_INT_SET, fr_arp_int_set, "fr_arp_int_set");
    reg_add_fr_mgd_timer_onoff(fr_inarp_timer_onoff, "fr_inarp_timer_onoff");
    fr_arp_parser_init();

}

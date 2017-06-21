/* $Id: fr_lmi.c,v 3.1.22.11 1996/09/11 16:49:40 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_lmi.c,v $
 *------------------------------------------------------------------
 * Frame Relay LMI
 *
 * January 1996, Charles Poe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fr_lmi.c,v $
 * Revision 3.1.22.11  1996/09/11  16:49:40  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.22.10  1996/08/28  13:21:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.22.9  1996/07/29  22:11:30  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.1.22.8  1996/07/23  13:31:19  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.22.7  1996/06/28  23:32:00  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.22.6  1996/06/10  18:01:14  ccpoe
 * CSCdi59147:  CPUHOG messages when I configure many dlcis on the 4000
 * Branch: California_branch
 *
 * Revision 3.1.22.5  1996/06/05  02:12:29  shankar
 * CSCdi58767:  FR LMI sometimes failed to come up, problem at SITA
 * Branch: California_branch
 *
 * Revision 3.1.22.4  1996/05/31  18:19:28  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.1.22.3  1996/05/09  14:49:09  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.22.2  1996/04/30  23:27:27  chifei
 * CSCdi52339:  Frame relay DTE/DCE switching across ip tunnel broken
 * Branch: California_branch
 * Enabled the full functionality of FR switching over IP tunnel,
 * also fixed regular switching function which was broken by improper
 * handling of the "frame intf-type" command.
 *
 * Revision 3.1.22.1.2.1  1996/04/27  07:39:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.22.1  1996/04/25  23:23:25  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.30.1  1996/04/24  04:11:22  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1.2.18  1996/04/22  18:36:19  ccpoe
 * CSCdi55254:  lmi timer running while interface is admin down
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.17  1996/04/16  22:10:05  ccpoe
 * Code review comments from Shankar, Rene, and Paulina
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.16  1996/03/27  23:36:05  shankar
 * Branch: FR_SVC_branch
 * One more restored chunk of code.
 *
 * Revision 3.1.2.15  1996/03/27  22:24:27  shankar
 * Branch: FR_SVC_branch
 * When parameter negotiation fails, display the cause string. Fix
 * a compile error in fr_lmi.c as well.
 *
 * Revision 3.1.2.14  1996/03/27  17:50:02  shankar
 * Branch: FR_SVC_branch
 * Restoring code that vanished. LMI Status report must have nothing to
 * do with SVCs.
 *
 * Revision 3.1.2.13  1996/03/19  19:47:41  fowler
 * Support broadcast queue on all PVCs
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.12  1996/03/19  17:54:18  ccpoe
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.11  1996/03/13  23:04:44  ccpoe
 * inverse arp now negotiates with LMI on send out inverse-arp request.
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.10  1996/03/12  18:59:31  ccpoe
 * LMI keepalive is now sent and status checked by the lmi_idb_timer.
 * FR_MIN_PKT_SIZE is checked after the svc mgt packets. fr_svc_periodic
 * is created.
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.9  1996/03/06  05:57:55  ccpoe
 * no keepalive on DCE side crashes the router.
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.8  1996/03/05  06:37:41  shankar
 * Branch: FR_SVC_branch
 * Sync between FR_SVC_baseline_020396 and FR_SVC_bseline_030396
 *
 * Revision 3.1.2.7  1996/03/03  15:46:48  shankar
 * Branch: FR_SVC_branch
 * Fix compile errors caused by not including fr_l2.h and fr_svc_registry.h
 *
 * Revision 3.1.2.6  1996/02/29  20:47:49  chifei
 * Branch: FR_SVC_branch
 * Resumed the missing code for SVC LAPF in set_fr_keepalive()
 *
 * Revision 3.1.2.5  1996/02/29  19:04:36  ccpoe
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.4  1996/02/29  00:47:37  shankar
 * Branch: FR_SVC_branch
 * Bug in FR LMI autosense, found courtesy of ccpoe
 *
 * Revision 3.1.2.3  1996/02/22  06:58:19  ccpoe
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.2  1996/02/22  01:13:09  ccpoe
 * Frame Relay IP tunnel
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.1  1996/02/21  23:06:01  ccpoe
 * Frame Relay Local Management Interface module
 * Branch: FR_SVC_branch
 *
 * Revision 3.1  1996/02/13  23:31:08  ccpoe
 * Frame Relay LMI
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
#include "dialer.h"
#include "../wan/dialer_registry.h"
#include "address.h"
#include "parser.h"
#include "serial_debug.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../wan/msg_frame_relay.c"	/* Not a typo, see logger.h */
#include "../if/network.h"
#include "frame_relay.h"
#include "fr_l2.h"
#include "fr_debug.h"
#include "fr_switch.h"
#include "fr_arp.h"
#include "fr_lmi.h"
#include "fr_tunnel.h"
#include "fr_svc.h"
#include "media_registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/ifmib_registry.h"
#include "fr_registry.h"
#include "fr_svc_registry.h"
#include "mgd_timers.h"

/*
 * LMI process id
 */
static pid_t fr_lmi_pid; 

/*
 * LMI parent timer pointer
 */
static mgd_timer *fr_lmi_timer;

static const fr_lmi_protocol_discriminator fr_lmi_prot_discriminator[] = {
	{FR_CISCO_LMI, LMI_PROT_DISC},
	{FR_CCITT_LMI, ANSI_LMI_PROT_DISC},
	{FR_ANSI_D, ANSI_LMI_PROT_DISC},
	{0, 0}
};

static const fr_ie_length ie_length[] = {
{MULTICAST_IE_ID, FR_MULTICAST_IE_LENGTH, 0, 0, TRUE, FALSE },
{ PVC_IE_ID, PVC_IE_LENGTH, 0, 0, TRUE, FALSE},
{ CCITT_PVC_IE_ID, ANSI_PVC_IE_LENGTH, 0, 0, TRUE, FALSE},
{ REPORT_TYPE_IE_ID, FR_REPORT_TYPE_IE_LENGTH, 0, 0, TRUE, FALSE_BUT_CONTINUE },
{ CCITT_REPORT_IE_ID, FR_REPORT_TYPE_IE_LENGTH, 0, 0, TRUE, FALSE_BUT_CONTINUE },
{ 0, 0, 0, 0, FALSE, FALSE }
};

/*
 * LMI input queue
 */

watched_queue *fr_lmiQ;


/*
 * fr_init_lmi_stats
 *
 * initialize the switch lmi statistics on the idb if configured for
 * frame relay DCE or DTE
 */

static void fr_init_lmi_stats (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;
    fr_lmi_stats_type *lmi_stats = NULL;
    
    if (!(is_frame_relay(hwidb) && (fr_idb = hwidb->frame_relay_stuff))) {
	return;
    }
    if (!lmi_idb->lmi_stats) {
	lmi_stats = (fr_lmi_stats_type *)
	    malloc_named(sizeof(struct fr_lmi_stats_type_), "FR Init LMI Stats");
	if (!lmi_stats) {
	    return;
	}
	lmi_idb->lmi_stats = lmi_stats;
    }
    lmi_stats = lmi_idb->lmi_stats;
    lmi_stats->invalid_ui = 0;
    lmi_stats->invalid_protocol_discriminator = 0;
    lmi_stats->invalid_dummy_call_ref = 0;
    lmi_stats->invalid_msg_type = 0;
    lmi_stats->invalid_locking_shift = 0;
    lmi_stats->invalid_ie_id = 0;
    lmi_stats->invalid_report_type_ie_len = 0;
    lmi_stats->invalid_report_request = 0;
    lmi_stats->invalid_keepalive_ie_len = 0;
    lmi_stats->num_enquiries_rcvd = 0;
    lmi_stats->num_enquiries_sent = 0;
    lmi_stats->num_status_rcvd = 0;
    lmi_stats->num_status_sent = 0;
    lmi_stats->num_upd_status_rcvd = 0;
    lmi_stats->num_upd_status_sent = 0;
    lmi_stats->num_full_status_rcvd = 0;
    lmi_stats->num_full_status_sent = 0;
    lmi_stats->num_dte_timeouts = 0;
    lmi_stats->num_dce_timeouts = 0;
}


/*
 * fr_lmi_idb_init
 *	initialize idb data structure related to lmi
 */
static void fr_lmi_idb_init(hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;

    /*
     * Check if it's a rentry
     */
    if (fr_idb->fr_lmi) {
	return;
    }

    fr_idb->fr_lmi = malloc_named(sizeof(struct fr_idb_lmi), "FR IDB LMI");

    if (!fr_idb->fr_lmi) {
	return;
    }
    
    lmi_idb->fr_lmi_type = FR_CISCO_LMI;
    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI(FRAME_RELAY_DEFAULT_LMI);
    lmi_idb->fr_report_id = REPORT_TYPE_IE_ID;
    lmi_idb->fr_keepalive_id = KEEPALIVE_IE_ID;
    lmi_idb->fr_dte_mineseen = 0;
    lmi_idb->fr_dte_yoursseen = 0;
    lmi_idb->fr_dte_myseq = 0;
    lmi_idb->fr_dce_mineseen = 0;
    lmi_idb->fr_dce_yoursseen = 0;
    lmi_idb->fr_dce_myseq = 0;

    lmi_idb->lmi_autosense = FALSE;
    lmi_idb->lmi_autosense_retry_count = 0;

    /*
     * lmi configurable parameters
     */
    lmi_idb->route_timer = NULL;   /* create it only when a DCE */
    lmi_idb->err_timer = NULL;        /* start with no error correction */
    lmi_idb->fr_lmi_n391dte = DEFAULT_LMI_N391DTE;
    lmi_idb->fr_lmi_n392dte = DEFAULT_LMI_N392DTE;
    lmi_idb->fr_lmi_n392dce = DEFAULT_LMI_N392DCE;
    lmi_idb->fr_lmi_n393dte = DEFAULT_LMI_N393DTE;
    lmi_idb->fr_lmi_n393dce = DEFAULT_LMI_N393DCE;
    lmi_idb->fr_lmi_t392dce = DEFAULT_LMI_T392DCE;

    /*
     * init lmi parameters to be updated and cleared periodically
     */
    lmi_idb->lmi_n391dte = 1;
    lmi_idb->lmi_n392[FR_INTF_DTE] = 0;
    lmi_idb->lmi_n392[FR_INTF_DCE] = 0;

    /*
     * Initialize events delay line and pointers
     */
    fr_reset_lmi_events(fr_idb, FR_INTF_DTE,FR_LMI_PASS,0,DEFAULT_LMI_N393DTE);
    fr_reset_lmi_events(fr_idb, FR_INTF_DCE,FR_LMI_PASS,0,DEFAULT_LMI_N393DCE);

    lmi_idb->status_rcvd = TRUE;
    lmi_idb->full_status_req = FALSE;
    fr_idb->fr_intf_type = FR_DTE;               /* start being a DTE */
    fr_init_lmi_stats(hwidb);
    if (fr_lmi_timer_create(hwidb, LMI_IDB_TIMER) == FALSE) {
	errmsg(&msgsym(LMITIMER, FR_LMI),
	       hwidb->hw_namestring, "lmi_idb_timer");
    }
}


/*
 * fr_lmi_idb_uninit
 *	uninitialize idb data structure related to lmi
 */
static void fr_lmi_idb_uninit(hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;

    if (!fr_idb->fr_lmi) {
	/* print the error message */
	return;
    }

    fr_lmi_timer_delete(hwidb, LMI_ERR_TIMER);
    fr_lmi_timer_delete(hwidb, LMI_IDB_TIMER);
    fr_lmi_timer_delete(hwidb, LMI_ROUTE_TIMER);

    lmi_idb->lmi_autosense = FALSE;
    lmi_idb->lmi_autosense_retry_count = 0;
    lmi_idb->fr_lmi_type_configured = FALSE;
    free(lmi_idb->lmi_stats);
    lmi_idb->lmi_stats = NULL;

    if (lmi_idb) {
	free(lmi_idb);
    }
    fr_idb->fr_lmi = NULL;
}


/*
 * fr_is_looped
 * Check if the FR access line is looped
 */
static boolean fr_is_looped (
    fr_idb_struct_type *fr_idb,
    paktype *pak,
    uchar *data_ptr, hwidbtype *idb)
{
    fr_report_type_struct *report_ie;
    struct frame_relay_keepalive_struct *keep_ie;
    uchar *paksize;

    /*
     * Point data_ptr to an IE for any type of LMI 
     */
    if (lmi_idb->fr_lmi_type == FR_ANSI_D) {
	if (*data_ptr != LOCK_SHIFT)
	    return (FALSE);
	data_ptr++;
    }
    /*
     * Return FALSE if LMI packet doesn't have right Report id.
     */
    if (*data_ptr == lmi_idb->fr_report_id) {

	if (frame_relay_lmi_debug(idb))
	    print_report_ie(data_ptr);
	report_ie = (fr_report_type_struct *) data_ptr;
	/*
	 * verify report IE length and report type to be the one sent last
	 */
	if ((report_ie->report_type_ie_length != FR_REPORT_TYPE_IE_LENGTH) &&
	   (report_ie->report_type_ie_req != lmi_idb->fr_dte_rpt_type_sent))
	    return (FALSE);
	data_ptr += (FRAME_RELAY_IE_OVERHEADBYTES + FR_REPORT_TYPE_IE_LENGTH);
    } else {
        return (FALSE);
    }

    /*
     * Return FALSE if LMI packet doesn't have right keepalive IE.
     */
    if (*data_ptr == lmi_idb->fr_keepalive_id) {

	if (frame_relay_lmi_debug(idb))
	    print_ka_ie(data_ptr);


	keep_ie = (struct frame_relay_keepalive_struct *) data_ptr;
        paksize = pak->datagramstart + pak->datagramsize;
	/*
	 * Return FALSE if the keepalive IE length is incorrect or the 
	 * length of LMI packet is incorrect
	 */
	if ((keep_ie->keepalive_ie_length !=
	   KEEPALIVE_IE_LENGTH) &&
           (data_ptr + FRAME_RELAY_IE_OVERHEADBYTES+KEEPALIVE_IE_LENGTH 
	   < paksize)) {
	    return (FALSE);
	}

	/*
	 * Check if the send and receive sequence numbers are identical as
	 * sent in the last status enquiry message, if yes return TRUE
	 */
        if ((keep_ie->current_seq == lmi_idb->fr_dte_myseq) && 
	    (keep_ie->received_seq == lmi_idb->fr_dte_yoursseen)) {
	    return (TRUE);
	}
    }
    return (FALSE);

}


/*
 * fr_mark_pvcs_as_inactive
 * Set the pvc_status to INACTIVE for FR DTE interfaces.
 */
static void fr_mark_pvcs_as_inactive (fr_idb_struct_type *fr_idb)
{
    int i;
    fr_pvc_info_type *fr_pvc;
	
	/* 
	 * Only a FR_DTE will be acted upon. 
	 */
	if(fr_idb->fr_intf_type != FR_DTE)
	    return;

	for (i = 0; i < MAXDLCI; i++) {
	    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
		continue;
	    fr_pvc->pvc_upd_seq = 0;
	    fr_set_pvc_status(fr_pvc, DLCI_PRESENT);
	    fr_set_subif_state(fr_pvc->idb, FR_PVC_STATE, fr_pvc->pvc_status);
	}
}


/*
 * fr_update_pvc_status
 * Update the status of the SWITCHED PVCs configured on the DTE
 * interface periodically. LOCAL PVCs will be updated through the 
 * LMI. If keepalive is not configured, the status will be 
 * displayed as STATIC, if the interface/line-protocol is up. 
 */
static void fr_update_pvc_status (hwidbtype *idb)
{
    int i;
    hwidbtype *out_idb; 
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    boolean interface_is_up, keepalive_is_set;

    fr_idb = idb->frame_relay_stuff;

    interface_is_up = hw_interface_up(idb);
    keepalive_is_set = !idb->nokeepalive;

    /*
     * Force all LOCAL PVCs on the DCE/NNI IF to active.
     * This allows PVCs to terminate on DCE/NNI IF.
     */
    if (interface_is_up) {
    	if (fr_idb->fr_intf_type != FR_DTE) {
    	    for (i = MIN_DLCI; i < MAXDLCI; i++) {
    		if ((fr_pvc = fr_idb->fr_dlci_tab[i])) {
    		    if (fr_pvc->pvc_usage == PVC_LOCAL) {
    			fr_set_pvc_status(fr_pvc, DLCI_ADDED);
    		    }
    		}
    		continue;
    	    }
    	}
    }
    /*
     * Update the status for switched PVCs on interfaces configured 
     * as FR_DTE. Switched PVCs configured on FR DCEs will be handled 
     * within route_status_timer(). 
     */
    if(fr_idb->fr_intf_type != FR_DTE)
	return;

    for (i = MIN_DLCI; i < MAXDLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]) || !fr_pvc->route_flag)
	    continue;
	out_idb = fr_pvc->fr_route.out_idb;
    	if (interface_is_up &&
	    hw_interface_up(out_idb) &&
	    (is_frame_relay(out_idb) || 
		fr_pvc->pvc_usage == PVC_SWITCHED_IP_TUNNEL) && 
	    (IS_PVC_ACTIVE_BIT_SET(fr_pvc->nni_pvc_status) ||
		!keepalive_is_set))
	    fr_set_pvc_status(fr_pvc, DLCI_ADDED);
    	else 
	    fr_set_pvc_status(fr_pvc, DLCI_PRESENT);
    }
}


/*
 * Send a keepalive packet
 */
void send_frame_relay_keepalive (hwidbtype *idb)
{
    paktype *pak;
    struct frame_relay_lmi *lmi_ptr;
    uchar *data_ptr;
    union frame_relay_ie_types *ka_ptr;
    unsigned keep_size;
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *hdr;

    fr_idb = idb->frame_relay_stuff;

    if ( !system_configured && !fr_idb->fr_bootp->fr_ai_initiated)
	return;

    if (!fr_idb)
	return;

    /*
     * If keepalive is turned off, force line up and
     * make sure interface is entered in routing table.
     */
    if (idb->nokeepalive) {
	if (fr_idb->fr_lineup == FALSE) {
	    fr_idb->fr_lineup = TRUE;
	    idb->reason = "LMI disabled";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	    route_adjust(idb);
	}
	return;
    }
    if (lmi_idb->fr_lmi_type == FR_ANSI_D ||
	lmi_idb->fr_lmi_type == FR_CCITT_LMI)
        keep_size = FRAME_RELAY_ANSI_LMI_OVERHEADBYTES
	            +2*FRAME_RELAY_IE_OVERHEADBYTES+FR_REPORT_TYPE_IE_LENGTH
	            +KEEPALIVE_IE_LENGTH+1;
    else
        keep_size = FRAME_RELAY_LMI_OVERHEADBYTES
	            +2*FRAME_RELAY_IE_OVERHEADBYTES+FR_REPORT_TYPE_IE_LENGTH
	            +KEEPALIVE_IE_LENGTH+1;

    pak = getbuffer(keep_size);
    if (!pak) {
	return;
    }
    /*
     * sanjeev begins
     */
    if (lmi_idb->fr_lmi_type == FR_ANSI_D ||
	lmi_idb->fr_lmi_type == FR_CCITT_LMI) {
	data_ptr = pak->network_start - 2;
	*data_ptr++ = UNNUMBERED_FRAME_ID;
	*data_ptr++ = ANSI_LMI_PROTOCOL_DISCRIMINATOR;
	*data_ptr++ = LMI_CALL_REF;
	*data_ptr++ = LMI_STATUS_ENQUIRY;
	if (lmi_idb->fr_lmi_type == FR_ANSI_D)
	    *data_ptr++ = LOCK_SHIFT;
	if (lmi_idb->fr_lmi_type == FR_CCITT_LMI)
	    *data_ptr++ = CCITT_REPORT_IE_ID;
	else
	    *data_ptr++ = REPORT_TYPE_IE_ID;
	*data_ptr++ = FR_REPORT_TYPE_IE_LENGTH;
	if (lmi_idb->lmi_n391dte < lmi_idb->fr_lmi_n391dte)
	    *data_ptr++ = FR_REPORT_KEEPALIVE_ONLY;
	else 
	    *data_ptr++ = FR_REPORT_FULL_STATUS;
	lmi_idb->fr_dte_rpt_type_sent = *(data_ptr-1);
	if (lmi_idb->fr_lmi_type == FR_CCITT_LMI)
	    *data_ptr++ = CCITT_KEEP_IE_ID;
	else
	    *data_ptr++ = KEEPALIVE_IE_ID;
	*data_ptr++ = KEEPALIVE_IE_LENGTH;
	if (++lmi_idb->fr_dte_myseq == 0)
	    lmi_idb->fr_dte_myseq = 1;
        *data_ptr++ = lmi_idb->fr_dte_myseq;
        *data_ptr++ = lmi_idb->fr_dte_yoursseen;
    }
    /*
     * sanjeev end s
     */
    else {
	lmi_ptr = (struct frame_relay_lmi *) (pak->network_start -
					      FRAME_RELAY_ENCAPBYTES);
	lmi_ptr->unnumbered_frame_id = UNNUMBERED_FRAME_ID;
	lmi_ptr->lmi_protocol_discriminator = LMI_PROTOCOL_DISCRIMINATOR;
	lmi_ptr->lmi_call_ref = LMI_CALL_REF;
	lmi_ptr->message_type_id = LMI_STATUS_ENQUIRY;
	lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_id =
	    REPORT_TYPE_IE_ID;
	lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_length =
	    FR_REPORT_TYPE_IE_LENGTH;
	if (lmi_idb->lmi_n391dte < lmi_idb->fr_lmi_n391dte)
	    lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_req =
	      FR_REPORT_KEEPALIVE_ONLY;
	else 
	    lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_req =
	      FR_REPORT_FULL_STATUS;
	lmi_idb->fr_dte_rpt_type_sent =
	    lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_req;
	ka_ptr = (union frame_relay_ie_types *)
	         ((uchar *)&lmi_ptr->frame_relay_ies.ie_bytes[3]);
	ka_ptr->frame_relay_keepalive_ie.keepalive_ie_id = KEEPALIVE_IE_ID;
	ka_ptr->frame_relay_keepalive_ie.keepalive_ie_length = 
	                                          KEEPALIVE_IE_LENGTH;
	if (++lmi_idb->fr_dte_myseq == 0)
	    lmi_idb->fr_dte_myseq = 1;
	ka_ptr->frame_relay_keepalive_ie.current_seq = lmi_idb->fr_dte_myseq;
	ka_ptr->frame_relay_keepalive_ie.received_seq = 
	                          lmi_idb->fr_dte_yoursseen;
    }

    if (frame_relay_lmi_debug(idb) || serial_debug)
        buginf("\n%s(out): StEnq, myseq %u, yourseen %u, DTE %s", 
	    idb->hw_namestring, lmi_idb->fr_dte_myseq,
            lmi_idb->fr_dte_yoursseen,
	    lmi_idb->lmi_lineup[FR_INTF_DTE] ? "up" : "down");
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_ADDRESS;
    pak->flags |= PAK_PRIORITY;	/* Mark to go to head of queue */
    pak->datagramstart = pak->network_start - sizeof(frame_relay_hdr);
    if (lmi_idb->fr_lmi_type == FR_CCITT_LMI)
        pak->datagramsize = keep_size - 1;
    else
        pak->datagramsize = keep_size; 
    hdr = (frame_relay_hdr *)pak->datagramstart;
    PUTSHORT(&hdr->dlci, (lmi_idb->fr_lmi_dlci | FRAME_RELAY_EA_BITS));
    if (frame_relay_lmi_debug(idb))
	hexdump_packet(pak, (ulong)IDB_SERIAL);
    if ( idb->board_encap)
	(*idb->board_encap)(pak,idb);
    pak->fr_flags &= ~PAK_FR_BROADCAST;  
    datagram_out(pak);
    lmi_idb->fr_lmi_state = FR_LMI_SENT; 
}


/*
 * fr_reset_lmi_events
 * Reset LMI events delay line
 */
void fr_reset_lmi_events (
	fr_idb_struct_type *fr_idb, 
	ushort intf, 
	uchar lmi_event, 
	ushort n392, 
	ushort n393)
{
    int i;
 
    for (i = 0; i < MAX_LMI_EVENTS; i++)
        lmi_idb->lmi_events[intf][i] = lmi_event;
    lmi_idb->lmi_old[intf] = 0;
    lmi_idb->lmi_new[intf] = n393;
    lmi_idb->lmi_n392[intf] = n392;
    lmi_idb->lmi_lineup[intf] = lmi_event;
}


/*
 * fr_lmi_event
 * Record an event in the LMI delay line and check line status
 */
void fr_lmi_event (
    fr_idb_struct_type *fr_idb,
    ushort intf,
    uchar lmi_event,
    ushort n392,
    ushort n393)
{
    ushort *new_ptr;
    ushort *old_ptr;
    ushort *n392_ptr;

    /*
     * Record new event in delay line, update error counter
     */
    new_ptr = &(lmi_idb->lmi_new[intf]);
    old_ptr = &(lmi_idb->lmi_old[intf]);
    n392_ptr = &(lmi_idb->lmi_n392[intf]);
    lmi_idb->lmi_events[intf][*new_ptr] = lmi_event;
    if (lmi_event != lmi_idb->lmi_events[intf][*old_ptr]) {
	if (lmi_event == FR_LMI_PASS) {
	    /*
	     * New = PASS, old = FAIL : decrement error counter
	     */
            if (*n392_ptr > 0)
	        (*n392_ptr)--;
	} else {
            /*
             * New = FAIL, old = PASS : increment error counter
             */
            if (*n392_ptr < n393)
                (*n392_ptr)++;
	}
    }
    /*
     * Update pointers
     */
    if (++*old_ptr >= MAX_LMI_EVENTS)
	*old_ptr = 0;
    if (++*new_ptr >= MAX_LMI_EVENTS)
        *new_ptr = 0;

    /*
     * Update LMI status
     */
    if (lmi_idb->lmi_lineup[intf]) {
        if (*n392_ptr >= n392) {
            lmi_idb->lmi_lineup[intf] = FALSE;
	    if (fr_idb->fr_intf_type == FR_DCE)
		lmi_idb->fr_dce_myseq = 0;
	    else
		if (fr_idb->fr_intf_type == FR_NNI) {
		    lmi_idb->fr_dce_myseq = 0;
		    lmi_idb->fr_dte_myseq = 0;
		}
		else
		    lmi_idb->fr_dte_myseq = 0;
	}
    } else {
        if (*n392_ptr == 0)
            lmi_idb->lmi_lineup[intf] = TRUE;
    }
}

/*
 * lmi_fr_hdr_check
 * Check content of the LMI header
 */
int fr_lmi_hdr_check (
    hwidbtype *idb,
    paktype *pak)
{
    uchar *ptr;
    fr_idb_struct_type *fr_idb;
    const fr_lmi_protocol_discriminator *fr_pd;
    int lmi_type;

    lmi_type = FR_CISCO_LMI;
    fr_idb = idb->frame_relay_stuff;
    if ( !fr_idb )
	return(FALSE);
    ptr = pak->datagramstart+ADDRLEN_FR;
    ptr++;			/* point to the protocol discriminator field */
    if (*ptr == ANSI_LMI_PROT_DISC)
        lmi_type = FR_CCITT_LMI;

    fr_pd = fr_lmi_prot_discriminator;
    for ( ;  (!fr_pd->lmi_type && !fr_pd->prot_disc); fr_pd++) {
	if ((lmi_idb->fr_lmi_type == fr_pd->lmi_type) &&
			(fr_pd->prot_disc != *ptr)) {
	if ( frame_relay_lmi_debug(idb))
	    buginf("\n%s: Invalid protocol discriminator in LMI %d",
		   idb->hw_namestring, *ptr);
    	    return(FALSE);
    	}
    }
    ptr++;

    if ((*ptr != LMI_CALL_REF) && (*ptr != FR_SVC_CALL_REF)) {
	if ( frame_relay_lmi_debug(idb))
	    buginf("\n%s: Invalid call reference value in LMI %d",
			idb->hw_namestring, *ptr);
	return(INVALID_CALL_REF_VALUE);
    }
    ptr++;			/* point to message type */

    if (*(ptr+1) == LOCK_SHIFT)
        lmi_type = FR_ANSI_D;

    if ((lmi_idb->lmi_autosense) && (lmi_idb->fr_lmi_type != lmi_type) &&
                        (fr_idb->fr_intf_type == FR_DTE))
        fr_lmi_set(idb,fr_idb,lmi_type,TRUE, FALSE);

    return(TRUE);
}


/*
 * frame_relay_receive_keepalive
 */

static enum FR_LMI_RTN_VAL frame_relay_receive_keepalive (
    hwidbtype *idb,
    paktype *pak,
    uchar *data_ptr)
{
    union frame_relay_ie_types *ka_ptr;
    fr_idb_struct_type *fr_idb;

    fr_idb =  idb->frame_relay_stuff;
    
    ka_ptr = (union frame_relay_ie_types *)data_ptr;
    lmi_idb->fr_dte_tmp_yoursseen = ka_ptr->frame_relay_keepalive_ie.current_seq;
    lmi_idb->fr_dte_mineseen = ka_ptr->frame_relay_keepalive_ie.received_seq;
    if (lmi_idb->fr_dte_mineseen != lmi_idb->fr_dte_myseq) {
	fr_idb->fr_err_dlci = lmi_idb->fr_lmi_dlci;
	return(FR_LMI_IE_FAIL);
    }
    if (lmi_idb->fr_dte_tmp_yoursseen == 0) {
	return(FR_LMI_FAIL);
    }
    lmi_idb->fr_liv_seen = TRUE;
    return(FR_LMI_PASS);
}


/*
 * frame_relay_parse_ie_pkt
 */

static uchar *frame_relay_parse_ie_pkt (
    hwidbtype *idb,
    paktype *pak,
    uchar *data_ptr)
{
    union frame_relay_ie_types *multicast_ie;
    ushort temp_dlci;
    ushort temp_status;
    uchar *paksize;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    hwidbtype *netidb;
    enum FR_LMI_RTN_VAL rtn_val;
    int length_chk;

    fr_idb =  idb->frame_relay_stuff;
    if (!fr_idb)
        return(data_ptr);
    length_chk = fr_ie_length_check(*data_ptr, *(data_ptr+1));
    if (length_chk == FALSE) {
	if (frame_relay_lmi_debug(idb))
	    buginf("\nLMI Length problem found %d, %d", *data_ptr, *(data_ptr+1));
	return (NULL);
    } else if (length_chk == FALSE_BUT_CONTINUE) {
	if (frame_relay_lmi_debug(idb))
	    buginf("\nLMI Length problem found %d, %d, continuing", 
					*data_ptr, *(data_ptr+1));
    }

    lmi_idb->full_status_rcvd = FALSE;	/* assume async status rcvd */
    paksize = pak->datagramstart + pak->datagramsize;
    while (data_ptr < paksize) {
	switch (*data_ptr) {
	case MULTICAST_IE_ID:
 	    if ( *(data_ptr+1) != FR_MULTICAST_IE_LENGTH ) {
 		if ( frame_relay_lmi_debug(idb))
 		    buginf("\n%s: Invalid multicast IE length %d",
 			   idb->hw_namestring, *(data_ptr+1));
		return(NULL);
 	    }
	    if (frame_relay_lmi_debug(idb))
		print_mc_ie(data_ptr);
	    multicast_ie = (union frame_relay_ie_types *)data_ptr;
	    data_ptr += FRAME_RELAY_IE_OVERHEADBYTES;
	    if (multicast_ie->ie_bytes[MULTICAST_DLCI_STATUS_INDEX] &
		FRAME_RELAY_LMI_DBIT) {
		fr_idb->fr_local_dlci = FRAME_RELAY_DEFAULT_LOCAL_DLCI;
		fr_mc_delete_dlci(idb,fr_idb->fr_multicast_dlci); 
	    } else {
		netidb = get_netidb(idb);
		memcpy((uchar *)&temp_dlci, &(multicast_ie->ie_bytes[MULTICAST_DLCI_INDEX]), 
			sizeof(temp_dlci));
		if (fr_idb->fr_multicast_status != FR_MULTICAST_DYNAMIC)
		    fr_add_mc_dlci(netidb,FR_MULTICAST_DYNAMIC,temp_dlci);
		lmi_idb->fr_mc_seen = TRUE; 
		if ((fr_pvc = fr_idb->fr_dlci_tab[temp_dlci])) {
		    temp_status =
			multicast_ie->ie_bytes[MULTICAST_DLCI_STATUS_INDEX];
		    fr_pvc->pvc_status = temp_status;
		    fr_idb->fr_multicast_gid =
			multicast_ie->frame_relay_multicast_ie.multicast_gid;
/*
 * Handle the multicast source DLCI setup
 */ 
		    memcpy((uchar *)&temp_dlci, 
			&multicast_ie->ie_bytes[MULTICAST_SOURCE_DLCI_INDEX],
			sizeof(temp_dlci));
		    if (!(fr_pvc = fr_idb->fr_dlci_tab[temp_dlci]))
			fr_pvc = fr_pvc_init(netidb->firstsw, temp_dlci,
					     PVC_NOTSET,PVC_DYNAMIC, 
					     FALSE, NULL);
		    if (fr_pvc) {
			fr_idb->fr_local_dlci = NUM_TO_DLCI(temp_dlci);
			fr_idb->map_status[temp_dlci] = FR_ALL_MAPPED;
		    }
		}
	    }
	    data_ptr += FR_MULTICAST_IE_LENGTH;
	    break;
	case PVC_IE_ID:
	case CCITT_PVC_IE_ID:
 	    if ((( lmi_idb->fr_lmi_type == FR_CISCO_LMI ) &&
 		 ( *(data_ptr+1) != PVC_IE_LENGTH )) ||
 		((lmi_idb->fr_lmi_type == FR_ANSI_D ) &&
 		 ( *(data_ptr+1) != ANSI_PVC_IE_LENGTH ))) {
 		if ( frame_relay_lmi_debug(idb))
 		    buginf("\n%s: Invalid PVC IE length %d",
 			   idb->hw_namestring, *(data_ptr+1));
		return(NULL);
 	    }
	    data_ptr = frame_relay_pvc_ie_parse(idb, pak, data_ptr);
	    if (data_ptr == NULL)
		return(NULL); 
	    break;
	case REPORT_TYPE_IE_ID:
	case CCITT_REPORT_IE_ID:
 	    if ( *(data_ptr+1) != FR_REPORT_TYPE_IE_LENGTH ) {
 		if ( frame_relay_lmi_debug(idb))
 		    buginf("\n%s: Invalid report type IE length %d",
 			   idb->hw_namestring, *(data_ptr+1));
		return(NULL);
 	    }
	    if (frame_relay_lmi_debug(idb))
		print_report_ie(data_ptr);
	    data_ptr += FRAME_RELAY_IE_OVERHEADBYTES;
/* Handle the case of duplicated report type IEs */
	    if (lmi_idb->fr_rpt_type_processing != FR_REPORT_NOTSEEN) {
		data_ptr += FR_REPORT_TYPE_IE_LENGTH;
		break;
	    }
	    if ((lmi_idb->fr_dte_rpt_type_sent == FR_REPORT_FULL_STATUS) &&
		(*data_ptr != FR_REPORT_FULL_STATUS))
		return(NULL); 
	    if ((*data_ptr == FR_REPORT_FULL_STATUS) ||
		(*data_ptr == FR_REPORT_KEEPALIVE_ONLY))
		lmi_idb->fr_rpt_type_processing = *data_ptr;
	    else { /* Skip a bogus report type to handle dup IE */
		data_ptr += FR_REPORT_TYPE_IE_LENGTH;
		break;
	    }
	    data_ptr += FR_REPORT_TYPE_IE_LENGTH;
	    break;
	case KEEPALIVE_IE_ID:
	case CCITT_KEEP_IE_ID:
 	    if ( *(data_ptr+1) != KEEPALIVE_IE_LENGTH ) {
 		if ( frame_relay_lmi_debug(idb))
 		    buginf("\n%s: Invalid keepalive IE length %d",
 			   idb->hw_namestring, *(data_ptr+1));
		data_ptr += FRAME_RELAY_IE_OVERHEADBYTES;
		data_ptr += *(data_ptr-1);
		break; 
 	    }
	    if (frame_relay_lmi_debug(idb))
		print_ka_ie(data_ptr);
	    if ((rtn_val = frame_relay_receive_keepalive(idb,pak,data_ptr))
		== FR_LMI_FAIL)
		return(NULL);
	    data_ptr += FRAME_RELAY_IE_OVERHEADBYTES;
	    data_ptr += KEEPALIVE_IE_LENGTH;
	    break;
	default:
	    if (frame_relay_lmi_debug(idb))
		buginf("\n%s: ie type %d length %d",
		       idb->hw_namestring,*data_ptr,*(data_ptr+1));
	    return(NULL);
	    break;
	}
    }
    return(data_ptr);
}


/*
 * frame_relay_pvc_ie_parse
 */
uchar *frame_relay_pvc_ie_parse (
    hwidbtype *idb,
    paktype *pak,
    uchar *ptr)
{
    fr_pvc_info_type *fr_pvc;
    fr_idb_struct_type *fr_idb;
    hwidbtype *netidb;
    ushort ansi_dlci;
    ushort num_dlci;
    union frame_relay_ie_types *pptr;
    uchar *pak_size;
    addrtype tempaddr;
    uchar tmp_status;

    tempaddr.type = 0;
    tempaddr.length = 0;
    bzero(&tempaddr.addr, sizeof(ADDRLEN_MAX));
    fr_idb =  idb->frame_relay_stuff;
    lmi_idb->fr_last_pvc_seen = MIN_DLCI;

    /*
     * For ISDN and dialers, idb points to the physical interface (B-ch, Ser),
     * but we need the network protocol information that resides on the rotary
     * group leader (D-ch, Dialer), so get it here.
     */
    netidb = get_netidb(idb);

    pak_size = pak->datagramstart + pak->datagramsize;
    while (IS_FRAME_RELAY_PVC_IE(*ptr) && ptr < pak_size) {
	pptr = (union frame_relay_ie_types *) ptr;

	switch (lmi_idb->fr_lmi_type) {

	case FR_ANSI_D :
	case FR_CCITT_LMI :
	    memcpy((uchar *)&ansi_dlci,
		(uchar *)&pptr->ie_bytes[PVC_DLCI_INDEX],
		sizeof(ushort));
	    if ((ansi_dlci & FR_ANSI_VALID_PVC_MASK) != FR_ANSI_EA_BIT)
		return(NULL);
	    num_dlci = ANSI_DLCI_TO_NUM(ansi_dlci);

	    /* Check to be sure of ascending order & range */
	    if ((lmi_idb->fr_last_pvc_seen > num_dlci) ||
		(num_dlci < MIN_DLCI) || (num_dlci > MAX_DLCI))
		return(NULL);
	    else
		lmi_idb->fr_last_pvc_seen = num_dlci;
	    if (frame_relay_lmi_debug(idb))
	        print_pvc_ie(ptr, lmi_idb->fr_lmi_type);
	    fr_pvc = fr_idb->fr_dlci_tab[num_dlci];
	    if (!fr_pvc) { 
		if (fr_idb->fr_intf_type == FR_DTE) {
		    /*
		     * Create PVC only if DTE !
		     */
		    if ((fr_pvc = fr_pvc_init(netidb->firstsw, num_dlci,
					     PVC_NOTSET, PVC_DYNAMIC, 
					     FALSE, NULL))) {
			fr_idb->map_dynamic[num_dlci] = FR_NOT_DYNAMIC;
                	fr_idb->map_status[num_dlci] = 0;
	 		fr_pvc->idb = netidb->firstsw;
			/* 
			 * for the case of autoinstall, lets create an IP map 
			 * so that the outgoing BOOTP request can be sent. 
			 */
			if (idb->serial_autoinstall) {
			    fr_addr_enter(netidb->firstsw, &tempaddr, 
					  num_dlci, TRUE, FR_ENCAPS_CISCO, 
					  FR_MAP_AUTOINSTALL, PVC_AUTOINSTALL,
					  LINK_IP, NO_OPTIONS, NULL);
			}
		    }
		}
	    }
	    if (fr_pvc) {
		tmp_status = pptr->ie_bytes[PVC_STATUS_INDEX];

		if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
		    (fr_pvc->pvc_usage == PVC_NOTSET)) {
		    /*
		     * For local PVC store and check incoming status
		     * and verify no extra bits set
		     */
		    if ((tmp_status & FR_ANSI_VALID_STATUS_MASK) != FR_ANSI_EA_BIT)
			return(NULL);
		    tmp_status &= ~FR_ANSI_EA_BIT;
		    fr_set_pvc_status(fr_pvc, tmp_status);
		    fr_set_subif_state(fr_pvc->idb, FR_PVC_STATE, tmp_status);

		} else if ((fr_pvc->pvc_usage == PVC_SWITCHED_IP_TUNNEL) &&
		     (fr_idb->fr_intf_type == FR_DTE)) {
		    if ((tmp_status & FR_ANSI_VALID_STATUS_MASK) != FR_ANSI_EA_BIT)
			return(NULL);
		    tmp_status &= ~FR_ANSI_EA_BIT;
		    fr_set_pvc_status(fr_pvc, tmp_status);
		    fr_pvc->nni_pvc_status = tmp_status;

		} else {
	            /*
		     * For PVC switching just store incoming status, it'll be
		     * taken care of later in route_status_timer()
	 	     */
		    fr_pvc->nni_pvc_status = tmp_status & ~FR_ANSI_EA_BIT;
		}
		fr_pvc->pvc_upd_seq = lmi_idb->fr_dte_myseq;
	    }

	    ptr += FRAME_RELAY_IE_OVERHEADBYTES;
	    ptr += ANSI_PVC_IE_LENGTH;
	    break;

	case FR_CISCO_LMI:

	    memcpy((uchar *)&num_dlci,(uchar *)&pptr->ie_bytes[PVC_DLCI_INDEX],
		  	sizeof(ushort));
	    if (frame_relay_lmi_debug(idb))
	        print_pvc_ie(ptr, lmi_idb->fr_lmi_type);
	    fr_pvc = fr_idb->fr_dlci_tab[num_dlci];
	    if (!fr_pvc) { 
                if (fr_idb->fr_intf_type == FR_DTE) {
                    /*
                     * Create PVC only if DTE !
                     */
		    if ((fr_pvc = fr_pvc_init(netidb->firstsw, num_dlci,
					     PVC_NOTSET, PVC_DYNAMIC, 
					      FALSE, NULL))) {
			fr_idb->map_dynamic[num_dlci] = FR_NOT_DYNAMIC;
                	fr_idb->map_status[num_dlci] = 0;
			fr_pvc->idb = netidb->firstsw;
			/* 
			 * for the case of autoinstall, lets create an IP map 
			 * so that the outgoing BOOTP request can be sent. 
			 */
			if (idb->serial_autoinstall) {
			    fr_addr_enter(netidb->firstsw, &tempaddr, 
					  num_dlci, TRUE, FR_ENCAPS_CISCO, 
					  FR_MAP_AUTOINSTALL, PVC_AUTOINSTALL,
					  LINK_IP, NO_OPTIONS, NULL);
			}
		    }
		}
	    }
            if (fr_pvc) {
		tmp_status = pptr->ie_bytes[PVC_STATUS_INDEX];
		if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
		    (fr_pvc->pvc_usage == PVC_NOTSET)) {
		    fr_pvc->pvc_bw =
			GETLONG(&pptr->ie_bytes[PVC_BANDWIDTH_INDEX - 1])
			& 0xffffff;
		    fr_set_pvc_status(fr_pvc, tmp_status);
		    fr_set_subif_state(fr_pvc->idb, FR_PVC_STATE, tmp_status);

		} else if ((fr_pvc->pvc_usage == PVC_SWITCHED_IP_TUNNEL) &&
			   (fr_idb->fr_intf_type == FR_DTE)) {
		    fr_pvc->pvc_bw =
			GETLONG(&pptr->ie_bytes[PVC_BANDWIDTH_INDEX - 1])
			& 0xffffff;
		    fr_set_pvc_status(fr_pvc, tmp_status);
                    fr_pvc->nni_pvc_status = tmp_status;

		} else {
                    /*
                     * For NNI just store incoming status, it'll be taken
                     * care of later in route_status_timer()
                     */
                    fr_pvc->nni_pvc_status = tmp_status;
		}
		fr_pvc->pvc_upd_seq = lmi_idb->fr_dte_myseq;
	    }

	    ptr += FRAME_RELAY_IE_OVERHEADBYTES;
	    ptr += PVC_IE_LENGTH;
	    break;

	default :
	    ptr = pak->datagramstart + pak->datagramsize;
	}
    }
    if (fr_idb && fr_idb->fr_bootp && fr_idb->fr_bootp->fr_ai_need_fs)
    	fr_idb->fr_bootp->fr_ai_need_fs = FALSE;
    return(ptr);
}

/*
 * fr_lmi_set
 * Change the LMI type to the specified type
 */
void fr_lmi_set (
    hwidbtype *idb,
    fr_idb_struct_type *fr_idb,
    enum FR_LMI_TYPE lmi_type,
    boolean sense,
    boolean LMI_configured)
{
	if (!idb->nokeepalive) {
	    if (!sense) {
		lmi_idb->fr_lmi_type = FR_CISCO_LMI;
		lmi_idb->fr_lmi_dlci = NUM_TO_DLCI(FRAME_RELAY_DEFAULT_LMI);
		lmi_idb->fr_report_id = REPORT_TYPE_IE_ID;
		lmi_idb->fr_keepalive_id = KEEPALIVE_IE_ID;
                lmi_idb->fr_lmi_type_configured = FALSE;
	    } else {
		if (LMI_configured)
                    lmi_idb->fr_lmi_type_configured = TRUE;
		else 
                    lmi_idb->fr_lmi_type_configured = FALSE;
 
		lmi_idb->fr_lmi_type = lmi_type;
		if (lmi_idb->fr_lmi_type == FR_CISCO_LMI) {
                    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI(FRAME_RELAY_DEFAULT_LMI);
                    lmi_idb->fr_report_id = REPORT_TYPE_IE_ID;
                    lmi_idb->fr_keepalive_id = KEEPALIVE_IE_ID;
		} else if (lmi_idb->fr_lmi_type == FR_ANSI_D) {
		    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI((ushort)0);
                    lmi_idb->fr_report_id = REPORT_TYPE_IE_ID;
                    lmi_idb->fr_keepalive_id = KEEPALIVE_IE_ID;
		} else { /* FR_CCITT_LMI */
                    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI((ushort)0);
                    lmi_idb->fr_report_id = CCITT_REPORT_IE_ID;
                    lmi_idb->fr_keepalive_id = CCITT_KEEP_IE_ID;
		}
	    }
	}
	if (!idb->serial_autoinstall) {
	    lmi_idb->fr_dte_mineseen = 0;
	    lmi_idb->fr_dte_yoursseen = 0;
	    lmi_idb->fr_dte_myseq = 0;
	}
}


/*
 * fr_update_n393
 */
void fr_update_n393 (
    fr_idb_struct_type *fr_idb,
    ushort new_n393,
    ushort *cur_n393,
    ushort intf)
{
    int i;
    ushort *indx_ptr;
    ushort *n392_ptr = &(lmi_idb->lmi_n392[intf]);

    /*
     * If n393 has increased extend delay line by bumping up lmi_new
     * pointer and adding PASS events
     */
    if (new_n393 > *cur_n393) {
	indx_ptr = &(lmi_idb->lmi_new[intf]);
        for (i = 0; i < (new_n393 - *cur_n393); i++) {
            lmi_idb->lmi_events[intf][*indx_ptr] = FR_LMI_PASS;
            if (*indx_ptr >= MAX_LMI_EVENTS)
                *indx_ptr = 0;
            else
                (*indx_ptr)++; 
        }
    /*
     * Else if n393 has decreased shorten delay line by bumping up
     * lmi_old pointer and incrementing n392 for each FAIL removed
     */
    } else if (new_n393 < *cur_n393) {
        indx_ptr = &(lmi_idb->lmi_old[intf]);
        for (i = 0; i < (*cur_n393 - new_n393); i++) {
            if (lmi_idb->lmi_events[intf][*indx_ptr] == FR_LMI_FAIL)
                if (*n392_ptr > 0)
                    (*n392_ptr)--;
            if (*indx_ptr >= MAX_LMI_EVENTS)
                *indx_ptr = 0;
            else
                (*indx_ptr)++;
        }
    }
    *cur_n393 = new_n393;
    if (*n392_ptr > *cur_n393)
        *n392_ptr = *cur_n393;
}


static void fr_lmi_timer_onoff (hwidbtype *hwidb, boolean ON_OFF)
{
    boolean more_timer;
    fr_idb_struct_type *fr_idb;
    mgd_timer *target_timer;
    ulong interval;

    
    fr_idb = hwidb->frame_relay_stuff;
    more_timer = TRUE;
    target_timer = NULL;
   
    while (more_timer) {
	if (fr_idb->fr_intf_type == FR_DTE) {
	    target_timer = lmi_idb->idb_timer;
	    interval = hwidb->keep_period * ONESEC;
	    more_timer = FALSE;
	} else {
	    if (target_timer == lmi_idb->route_timer) {
		target_timer = lmi_idb->err_timer;
		interval = lmi_idb->fr_lmi_t392dce * ONESEC;
		more_timer = FALSE;
	    }
	    target_timer = lmi_idb->route_timer;
	    interval = LMI_ROUTE_INTERVAL * ONESEC;
	}

	if (target_timer == NULL) {
	    return;
	}
	if (ON_OFF) {
	    if (!mgd_timer_running(target_timer)) {
		mgd_timer_start(target_timer, interval);
	    }
	} else {
	    if (mgd_timer_running(target_timer)) {
		mgd_timer_stop(target_timer);
	    }
	}
    }
}
    
/*
 * fr_lmi_timer_interval
 *	Set the interval of lmi timers
 */
static void fr_lmi_timer_interval(hwidbtype *hwidb, ulong *interval, ulong timertype)
{
    mgd_timer *target_timer;
    fr_idb_struct_type *fr_idb;
    
    fr_idb = hwidb->frame_relay_stuff;
    
    switch (timertype) {
    case LMI_IDB_TIMER:
	target_timer = lmi_idb->idb_timer;
	break;
    case LMI_ERR_TIMER:
	target_timer = lmi_idb->err_timer;
	break;
    case LMI_ROUTE_TIMER:
	target_timer = lmi_idb->route_timer;
	break;
    default:
	target_timer = NULL;
    }
    if (target_timer != NULL) {
	mgd_timer_update(target_timer, *interval);
    }
}



/*
 * fr_lmi_chk
 * 	check to determine lmi line should be up or down
 *      and act accordingly.
 */
static void fr_lmi_chk(hwidbtype *hwidb)
{
    hwidbtype *netidb;
    fr_idb_struct_type *fr_idb;
    boolean lmi_lineup;

    fr_idb = hwidb->frame_relay_stuff;

    /*
     * For ISDN and dialers, idb points to the physical interface (B-ch, Ser),
     * but we need the network protocol information that resides on the rotary
     * group leader (D-ch, Dialer), so get it here.
     */
    netidb = get_netidb(hwidb);

    if (hwidb->state != IDBS_UP) {
	fr_idb->fr_idb_is_up = FALSE;
	return;
    } else {
	fr_idb->fr_idb_is_up = TRUE; 
    }

    if (fr_idb->fr_idb_is_up)
    	fr_update_pvc_status(hwidb);

    if (!hwidb->nokeepalive) {
	/*
	 * LMI is up only if both DTE and DCE LMI are up
	 */
	lmi_lineup = lmi_idb->lmi_lineup[FR_INTF_DTE] && 
			lmi_idb->lmi_lineup[FR_INTF_DCE];

	/*
	 * If line up and either DTE or DCE LMI line down: line goes down
         */
	if (fr_idb->fr_lineup) {
	    if (!lmi_lineup) {
	        fr_idb->fr_lineup = FALSE;

		/*
		 * Line protocol is down, mark all PVCs as INACTIVE. If 
		 * line is down hard, fr_cstate will mark them as DELETED. 
		 * Also clear all dynamically entered PVCs for all
		 * link types and for all dlci, because the other end 
		 * may have been deliberately taken down. 
		 */
		fr_mark_pvcs_as_inactive(fr_idb);

	    	/*
	     	 * Clear all dynamically entered PVCs for ALL
	     	 * link types and for ALL dlci, because the line is 
	     	 * down. 
	     	 *
 	     	 * note: 
 	     	 * 	link of LINK_ILLEGAL implies ALL link types
 	     	 * 	dlci of MAX_DLCIVALUES implies ALL dlci. 
	     	 */ 
		fr_clear_dynamic_entries(netidb, LINK_ILLEGAL,
				      MAXDLCI_VALUES,FR_BOTH);
	        hwidb->reason = "LMI failed";
		reg_invoke_Lapf_sys_event(FR_MDL_REMOVE, fr_idb);
	        (*hwidb->reset)(hwidb);
		reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, hwidb);
	    }
        /*
         * Else if line down and no LMI line down: line goes up
         */
	} else {
	    if (lmi_lineup) { 
		fr_idb->fr_lineup = TRUE;

		/*
		 * Send a full status to the DTE
		 */
		if (fr_idb->fr_intf_type != FR_DTE)
            	    lmi_idb->full_status_req = TRUE;

		hwidb->reason = "LMI RESTARTED";
		reg_invoke_Lapf_sys_event(FR_MDL_ASSIGN, fr_idb);
		reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	    }
	}	    
    } else {	/* if keepalive is removed .. */
	if (!fr_idb->fr_lineup) {	/* but lineup is false */
	    fr_idb->fr_lineup = TRUE;	/* bring it up */
	    reg_invoke_Lapf_sys_event(FR_MDL_ASSIGN, fr_idb);
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	}
    }

}


/*
 * FRAME RELAY SWITCH LMI ROUTINES FOLLOW
 *
 * the PVC status for the route entries will be handled by a timer routine 
 * which will update the PVC status based on interface status.
 */



/*
 * rcv_fr_switch_keepalive
 *
 * receive and verify keepalives with a connected frame relay DTE
 */

static boolean rcv_fr_switch_keepalive (
    fr_idb_struct_type *fr_idb,
    uchar *data_ptr)
{
    union frame_relay_ie_types *ka_ptr;
    
    ka_ptr = (union frame_relay_ie_types *)data_ptr;
    lmi_idb->fr_dce_yoursseen = ka_ptr->frame_relay_keepalive_ie.current_seq;
    lmi_idb->fr_dce_mineseen = ka_ptr->frame_relay_keepalive_ie.received_seq;
    return (lmi_idb->fr_dce_mineseen == lmi_idb->fr_dce_myseq);
}

/*
 * parse_fr_switch_lmi
 *
 * parse the status enquiry pkt received by the switch for report type 
 * requested and the keepalive exchange.
 */

static void parse_fr_switch_lmi (
    hwidbtype *idb,
    paktype *pak,
    uchar *data_ptr)
{
    union frame_relay_ie_types *report_ie;
    union frame_relay_ie_types *keep_ie;
    boolean full_status = FALSE;
    fr_idb_struct_type *fr_idb;
    uchar *paksize;
    int i;
    fr_pvc_info_type *fr_pvc;

    fr_idb =  idb->frame_relay_stuff;
    if (frame_relay_lmi_debug(idb) || serial_debug)
        buginf("\n%s(in): StEnq, myseq %u",
            idb->hw_namestring, lmi_idb->fr_dce_myseq);

    /*
     * Restart LMI timer
     */
    mgd_timer_start(lmi_idb->err_timer, lmi_idb->fr_lmi_t392dce * ONESEC);
        
    /*
     * at this current location of data_ptr 4-company LMI has a IE while
     * Annex d has an extra byte called locking shift for using codeset 5
     * for IE's.
     */
    if (lmi_idb->fr_lmi_type == FR_ANSI_D) {
	if (*data_ptr != LOCK_SHIFT) {
	    lmi_idb->lmi_stats->invalid_locking_shift++;
            fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
            if (frame_relay_lmi_debug(idb))
                buginf("\n%s: Invalid locking shift value 0x%x", 
			idb->hw_namestring, *data_ptr);
	    return;
	}
	data_ptr++;  /* lock shift OK , point to an IE */
    }
    paksize = pak->datagramstart + pak->datagramsize;

    /*
     *  We expect a Report Type followed by a Keepalive IE, and nothing else.
     */
    if (*data_ptr == lmi_idb->fr_report_id) {

	if (frame_relay_lmi_debug(idb))
	    print_report_ie(data_ptr);
	report_ie = (union frame_relay_ie_types *) data_ptr;
	/*
	 * verify report ie's and update stats if
	 * anything is invalid 
	 */
	if (report_ie->fr_report_type_ie.report_type_ie_length != 
		                          FR_REPORT_TYPE_IE_LENGTH) {
	    lmi_idb->lmi_stats->invalid_report_type_ie_len++;
            fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
                            lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
            if (frame_relay_lmi_debug(idb))
                buginf("\n%s: Invalid report type ie length 0x%x",
                        idb->hw_namestring, 
			report_ie->fr_report_type_ie.report_type_ie_length);
	    return;
	}
	if ((report_ie->fr_report_type_ie.report_type_ie_req !=
		     FR_REPORT_FULL_STATUS) &&
	    (report_ie->fr_report_type_ie.report_type_ie_req !=
		     FR_REPORT_KEEPALIVE_ONLY)) {
	    lmi_idb->lmi_stats->invalid_report_request++;
            fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
                            lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
            if (frame_relay_lmi_debug(idb))
                buginf("\n%s: Invalid report type ie value 0x%x",
                        idb->hw_namestring, 
                        report_ie->fr_report_type_ie.report_type_ie_req);
	    return;
	}
	if (report_ie->fr_report_type_ie.report_type_ie_req ==
		    FR_REPORT_FULL_STATUS)
	    full_status = TRUE;
	data_ptr += FRAME_RELAY_IE_OVERHEADBYTES;
	data_ptr += FR_REPORT_TYPE_IE_LENGTH;
    } else {
        /*
         * if the IE ID is not Report Type return
         */
        lmi_idb->lmi_stats->invalid_ie_id++;
        fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
                        lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
        if (frame_relay_lmi_debug(idb))
            buginf("\n%s: Invalid IE ID 0x%x, expected 0x%x", 
	    idb->hw_namestring, *data_ptr, lmi_idb->fr_report_id);
        return;
    }
    if (*data_ptr == lmi_idb->fr_keepalive_id) {

	if (frame_relay_lmi_debug(idb))
	    print_ka_ie(data_ptr);
	/*
	 * verify keepalive ie and update stats accordingly
	 */
	 keep_ie = (union frame_relay_ie_types *) data_ptr;
	if (keep_ie->frame_relay_keepalive_ie.keepalive_ie_length !=
		    KEEPALIVE_IE_LENGTH) {
	    lmi_idb->lmi_stats->invalid_keepalive_ie_len++;
            fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
			 lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
            if (frame_relay_lmi_debug(idb))
                buginf("\n%s: Invalid keepalive ie length 0x%x",
                        idb->hw_namestring, 
                        keep_ie->frame_relay_keepalive_ie.keepalive_ie_length);
	    return;
	}
    } else {
        /*
         * if the IE ID is not Keepalive return
         */
        lmi_idb->lmi_stats->invalid_ie_id++;
        fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
		     lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
        if (frame_relay_lmi_debug(idb))
            buginf("\n%s: Invalid IE ID 0x%x, expected 0x%x", 
	    	idb->hw_namestring, *data_ptr, lmi_idb->fr_keepalive_id);
        return;
    }
    if (data_ptr + FRAME_RELAY_IE_OVERHEADBYTES+KEEPALIVE_IE_LENGTH < paksize) {
        /*
	 * not the end of the message : discard
	 */
        data_ptr += FRAME_RELAY_IE_OVERHEADBYTES + KEEPALIVE_IE_LENGTH;
        lmi_idb->lmi_stats->invalid_ie_id++;
        fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
                        lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
        if (frame_relay_lmi_debug(idb))
           buginf("\n%s: Unexpected IE ID 0x%x", idb->hw_namestring, *data_ptr);
	return;
    }

    /*
     * Message is fine: process keepalive IE
     */
    if (rcv_fr_switch_keepalive(fr_idb, data_ptr)) {
        fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_PASS, 
                        lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
        /*
         * check to see if i have to reset NEW bit
         */
        for (i = MIN_DLCI; i < MAXDLCI; i++) {
            if ((fr_pvc = fr_idb->fr_dlci_tab[i]) &&
		    IS_PVC_NEW_BIT_SET(fr_pvc->pvc_status)) {
		fr_pvc->pvc_status &= ~PVC_NEW_BIT;
                GET_TIMESTAMP(fr_pvc->pvc_change_time);
	    }
        }
    } else {
        fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
                        lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
    }

    fr_lmi_chk(idb);
    lmi_idb->lmi_stats->num_status_sent++;
    if (full_status || lmi_idb->full_status_req) {
	lmi_idb->lmi_stats->num_full_status_sent++;
	lmi_idb->full_status_req = FALSE;
	send_fr_switch_lmi_status(idb, TRUE, LMI_STATUS);
    } else {
        send_fr_switch_lmi_status(idb, FALSE, LMI_STATUS);
    }
}


/*
 * send_fr_switch_lmi_status
 *
 * send FULL STATUS msg to DTE if flag is set, otherwise exchange keepalives
 */

void send_fr_switch_lmi_status (
    hwidbtype *idb,
    boolean flag,
    uchar msg_type)
{
    paktype *pak;
    struct frame_relay_lmi *lmi_ptr;
    uchar *data_ptr;
    union frame_relay_ie_types *ka_ptr;
    unsigned pkt_size;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    ushort num_pvcs, pvcie_cnt;
    frame_relay_hdr *hdr;
    int i;

    fr_idb =  idb->frame_relay_stuff;
    num_pvcs = fr_idb->num_idb_pvcs;

    if (lmi_idb->fr_lmi_type == FR_ANSI_D ||
	lmi_idb->fr_lmi_type == FR_CCITT_LMI) {
	if (msg_type == LMI_STATUS) {
	    if (flag)
	        pkt_size = FRAME_RELAY_ANSI_LMI_OVERHEADBYTES
	                  +2*FRAME_RELAY_IE_OVERHEADBYTES
			  +FR_REPORT_TYPE_IE_LENGTH
	                  +KEEPALIVE_IE_LENGTH
		          +(num_pvcs*ANSI_PVC_LEN)+1;
	    else
	        pkt_size = FRAME_RELAY_ANSI_LMI_OVERHEADBYTES
	                  +2*FRAME_RELAY_IE_OVERHEADBYTES
			  +FR_REPORT_TYPE_IE_LENGTH
	                  +KEEPALIVE_IE_LENGTH+1;
	}
	else /* update status msg */
	    pkt_size = FRAME_RELAY_ANSI_LMI_OVERHEADBYTES+ANSI_PVC_LEN+1;
    }
    else {
	if (msg_type == LMI_STATUS) {
	    if (flag)
	        pkt_size = FRAME_RELAY_LMI_OVERHEADBYTES
		          +2*FRAME_RELAY_IE_OVERHEADBYTES
			  +FR_REPORT_TYPE_IE_LENGTH
	                  +KEEPALIVE_IE_LENGTH
		          +(num_pvcs*CISCO_PVC_LEN)+1;
	    else
	        pkt_size = FRAME_RELAY_LMI_OVERHEADBYTES
	                  +2*FRAME_RELAY_IE_OVERHEADBYTES
			  +FR_REPORT_TYPE_IE_LENGTH
	                  +KEEPALIVE_IE_LENGTH+1;
	}
	else /* lmi update msg */
	    pkt_size = FRAME_RELAY_LMI_OVERHEADBYTES+CISCO_PVC_LEN+1;    
    }
    pak = getbuffer(pkt_size);
    if (!pak)
	return;
    if (lmi_idb->fr_lmi_type == FR_ANSI_D ||
	lmi_idb->fr_lmi_type == FR_CCITT_LMI) {
	data_ptr = pak->network_start - 2;  /* after DLCI bytes */
	/*
	 * the stupid compiler is adding an extra byte to align the
	 * union in the ansi_lmi struct, so can't use
	 * the structure nor can we pack it.
	 * so write it byte by byte.
	 */
	*data_ptr++ = UNNUMBERED_FRAME_ID;
	*data_ptr++ = ANSI_LMI_PROTOCOL_DISCRIMINATOR;
	*data_ptr++ = LMI_CALL_REF;
	*data_ptr++ = msg_type;
	if (lmi_idb->fr_lmi_type == FR_ANSI_D)
	    *data_ptr++ = LOCK_SHIFT;
	/*
	 * now check if the status was update status. If so, avoid following
	 * ie's and send only pvc ie's
	 */
	if (msg_type == LMI_UPDATE_STATUS)
	    goto add_pvc;
	if (lmi_idb->fr_lmi_type == FR_CCITT_LMI)
	    *data_ptr++ = CCITT_REPORT_IE_ID;
	else
	    *data_ptr++ = REPORT_TYPE_IE_ID;
	*data_ptr++ = FR_REPORT_TYPE_IE_LENGTH;
	if (flag)
	    *data_ptr++ = FR_REPORT_FULL_STATUS;
	else
	    *data_ptr++ = FR_REPORT_KEEPALIVE_ONLY;

	if (lmi_idb->fr_lmi_type == FR_CCITT_LMI)
	    *data_ptr++ = CCITT_KEEP_IE_ID;
	else
	    *data_ptr++ = KEEPALIVE_IE_ID;
	*data_ptr++ = KEEPALIVE_IE_LENGTH;
	if (++lmi_idb->fr_dce_myseq == 0)
	    lmi_idb->fr_dce_myseq = 1;
	*data_ptr++ = lmi_idb->fr_dce_myseq;
        *data_ptr++ = lmi_idb->fr_dce_yoursseen;
    }
    else {
	lmi_ptr = (struct frame_relay_lmi *) (pak->network_start -
					      FRAME_RELAY_ENCAPBYTES);
	lmi_ptr->unnumbered_frame_id = UNNUMBERED_FRAME_ID;
	lmi_ptr->lmi_protocol_discriminator = LMI_PROTOCOL_DISCRIMINATOR;
	lmi_ptr->lmi_call_ref = LMI_CALL_REF;
	lmi_ptr->message_type_id = msg_type;

	if (msg_type == LMI_UPDATE_STATUS) {
	    data_ptr = (uchar *)&(lmi_ptr->message_type_id) + 1;
	    goto add_pvc;
	}

	lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_id =
	                                REPORT_TYPE_IE_ID;
	lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_length =
	                                FR_REPORT_TYPE_IE_LENGTH;
	if (flag)
	    lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_req =
	                                FR_REPORT_FULL_STATUS;
	else
	    lmi_ptr->frame_relay_ies.fr_report_type_ie.report_type_ie_req =
	                                FR_REPORT_KEEPALIVE_ONLY;

        ka_ptr = (union frame_relay_ie_types *)
	         ((uchar *)&lmi_ptr->frame_relay_ies.ie_bytes[3]);
	ka_ptr->frame_relay_keepalive_ie.keepalive_ie_id = KEEPALIVE_IE_ID;
	ka_ptr->frame_relay_keepalive_ie.keepalive_ie_length = 
	                                 KEEPALIVE_IE_LENGTH;
	if (++lmi_idb->fr_dce_myseq == 0)
	    lmi_idb->fr_dce_myseq = 1;
	ka_ptr->frame_relay_keepalive_ie.current_seq = lmi_idb->fr_dce_myseq;

	ka_ptr->frame_relay_keepalive_ie.received_seq = 
	                                 lmi_idb->fr_dce_yoursseen;
	data_ptr = (uchar *) (&ka_ptr->frame_relay_keepalive_ie.received_seq)+1;
    }
    /*
     * the differences have been added. Now add the pvc status ie's
     */

add_pvc:

    if (flag && num_pvcs) { /* if not num_pvcs avoid looping */
	pvcie_cnt = 0;

	for (i = 0; i < MAXDLCI ; i++) {
	    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
	        continue;
	    /*
             * Do not Report SVCs. This is a problem when we test
             * back-back routers.
             */
            if ((fr_pvc->pvc_type == SVC_DYNAMIC) ||
		(fr_pvc->pvc_usage == PVC_NOTSET)) { 
                continue;
	    }
	    /*
	     * if this is an update msg then only send pvc info for
	     * pvc's that are marked deleted
	     */
	    if (msg_type == LMI_UPDATE_STATUS) {
		if (fr_idb->fr_intf_type == FR_DCE) {
		    if (!(IS_DLCI_DELETED(fr_pvc->pvc_status)))
		    continue;
		}
		else if (fr_idb->fr_intf_type == FR_NNI) {
		    if (IS_PVC_ACTIVE_BIT_SET(fr_pvc->pvc_status))
		        continue;
		}
	    }
	    if (lmi_idb->fr_lmi_type == FR_ANSI_D ||
		lmi_idb->fr_lmi_type == FR_CCITT_LMI)
	        data_ptr = add_ansi_ccitt_pvc(fr_idb, data_ptr, fr_pvc,
					      i, lmi_idb->fr_lmi_type);
	    else
	        data_ptr = add_cisco_pvc(fr_idb, data_ptr, fr_pvc, 
					 i, lmi_idb->fr_lmi_type);
	    if (IS_PVC_NEW_BIT_SET(fr_pvc->pvc_status))
	        fr_pvc->pvc_upd_seq = lmi_idb->fr_dce_yoursseen;
	    pvcie_cnt++;
	}
	if (msg_type == LMI_STATUS) {
	    /* see if need to adjust packet size */
	    if (num_pvcs > pvcie_cnt) {
		if (lmi_idb->fr_lmi_type == FR_CISCO_LMI)
		    pkt_size -= (num_pvcs - pvcie_cnt) * CISCO_PVC_LEN;
		else
		    pkt_size -= (num_pvcs - pvcie_cnt) * ANSI_PVC_LEN;
	    } else if (num_pvcs < pvcie_cnt) {
		if (frame_relay_lmi_debug(idb))
		    buginf("\n%s:Full Status missing %d PVCs",
			idb->hw_namestring, (pvcie_cnt - num_pvcs));
	    }
	}
    }
	 
    if (frame_relay_lmi_debug(idb) || serial_debug)
        buginf("\n%s(out): %s, myseq %u, yourseen %u"
            ", DCE %s", idb->hw_namestring,
	    (msg_type == LMI_STATUS) ? "Status" : "UpSta",
	    lmi_idb->fr_dce_myseq, lmi_idb->fr_dce_yoursseen,
            lmi_idb->lmi_lineup[FR_INTF_DCE] ? "up" : "down");
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_ADDRESS;
    pak->datagramstart = pak->network_start - sizeof(frame_relay_hdr);
    hdr = (frame_relay_hdr *)pak->datagramstart;
    hdr->dlci = lmi_idb->fr_lmi_dlci | FRAME_RELAY_EA_BITS;
    if (lmi_idb->fr_lmi_type == FR_CCITT_LMI)
        pak->datagramsize = pkt_size - 1;
    else
        pak->datagramsize = pkt_size;
    pak->flags |= PAK_PRIORITY;	/* Mark to go to head of queue */
    pak->fr_flags &= ~PAK_FR_BROADCAST;  
    if (idb->board_encap)
	(*idb->board_encap)(pak,idb); /* don't check ret - we know we are ok */
    datagram_out(pak);
}


/*
 * add_ansi_ccitt_pvc
 *
 * add ansi or ccitt pvc ie at the location provided
 */

uchar *add_ansi_ccitt_pvc (
    fr_idb_struct_type *fr_idb,
    uchar *data_ptr,
    fr_pvc_info_type *fr_pvc,
    int index, uchar lmi_type)
{
    union frame_relay_ie_types *pvc_ie;

    /*
     * add ansi as well as ccitt pvc
     */
    pvc_ie = (union frame_relay_ie_types *) (uchar *)data_ptr;
    if (lmi_type == FR_CCITT_LMI)
        pvc_ie->ansi_pvc_ie.pvc_id_ie = CCITT_PVC_IE_ID;
    else
        pvc_ie->ansi_pvc_ie.pvc_id_ie = PVC_IE_ID;
    pvc_ie->ansi_pvc_ie.pvc_id_length = ANSI_PVC_IE_LENGTH;
    index = NUM_TO_ANSI_DLCI((ushort)index);
    PUTSHORT(&pvc_ie->ansi_pvc_ie.pvc_dlci, (ushort)index);
    pvc_ie->ansi_pvc_ie.pvc_status = fr_pvc->pvc_status;
    pvc_ie->ansi_pvc_ie.pvc_status |= FR_ANSI_EA_BIT;  /* set MSB */
    data_ptr += (FRAME_RELAY_IE_OVERHEADBYTES + ANSI_PVC_IE_LENGTH);
    return(data_ptr);
}


/*
 * add_cisco_pvc
 *
 * add the pvc ie in 4-company format
 */


uchar *add_cisco_pvc (
    fr_idb_struct_type *fr_idb,
    uchar *data_ptr,
    fr_pvc_info_type *fr_pvc,
    int index, uchar lmi_type)
{
    union frame_relay_ie_types *pvc_ie;

    pvc_ie = (union frame_relay_ie_types *) (uchar *)data_ptr;
    pvc_ie->frame_relay_pvc_ie.pvc_id_ie = PVC_IE_ID;
    pvc_ie->frame_relay_pvc_ie.pvc_id_length = PVC_IE_LENGTH;
    PUTSHORT(&pvc_ie->frame_relay_pvc_ie.pvc_dlci,(ushort)index);
    pvc_ie->frame_relay_pvc_ie.pvc_status = fr_pvc->pvc_status;
    data_ptr += FR_SWITCH_PVC_LEN;
    memcpy (data_ptr, (uchar *)&fr_pvc->pvc_bw, 3);
    data_ptr += sizeof(fr_pvc->pvc_bw) - 1;
    return(data_ptr);
}


/*
 * route_status_timer
 *
 * timer to maintain the pvc status of each route entry on a FR DCE/NNI.
 * periodically update the active/inactive/new/add/defined status.
 */

static void route_status_timer (hwidbtype *idb)
{
    hwidbtype *out_idb;
    fr_idb_struct_type *fr_in_idb, *fr_out_idb;
    fr_pvc_info_type *fr_pvc, *fr_out_pvc;
    ushort out_dlci;
    int i;
    boolean out_status;

    if (!is_frame_relay(idb) || !(fr_in_idb = idb->frame_relay_stuff))
        return;

    /*
     * if no PVCs configured on this interface return
     */
    if (!fr_in_idb->num_idb_pvcs)
	return;

    /*
     * Now i have the idb, go thru all routes on this to update the
     * status of each pvc. A pvc has two statuses:
     *   nni_pvc_status is the status as received from the remote FR device
     *   pvc_status     is the status sent to the remote device, reflecting
     *                  the status of the path back to the source
     * For all pvcs on this idb :
     *   if route defined
     *     if other_end UP and other_end lineup
     *       pvc_status = ACTIVE
     *     else
     *       pvc_status = INACTIVE
     *   else
     *     pvc_status = INACTIVE
     */
    for (i = MIN_DLCI; i < MAXDLCI; i++) {
        if (!(fr_pvc = fr_in_idb->fr_dlci_tab[i]) || !fr_pvc->route_flag)
            continue;

        out_idb = fr_pvc->fr_route.out_idb;
        out_dlci = fr_pvc->fr_route.out_dlci;
        out_status = fr_pvc->pvc_status;

        /*
         * if pvc route to other interface not defined : status = INACTIVE
         */
	fr_out_pvc = NULL;
	reg_invoke_fr_tunnel_pvc(fr_pvc, &fr_out_pvc, FR_TUNNEL_FIND);
	if (fr_out_pvc ||
	    (is_frame_relay(out_idb) &&
	     (fr_out_idb = out_idb->frame_relay_stuff) &&
	     (fr_out_pvc = fr_out_idb->fr_dlci_tab[out_dlci]) &&
	     fr_out_pvc->route_flag)) {
            /*
             * if everything OK on other side : status = ACTIVE
             *                           else : status = INACTIVE
             */
            if (hw_interface_up(out_idb) &&
                  (IS_PVC_ACTIVE_BIT_SET(fr_out_pvc->nni_pvc_status) ||
		   (is_frame_relay(out_idb) && out_idb->nokeepalive)))
                fr_set_pvc_status(fr_pvc, DLCI_ADDED);
            else
                fr_set_pvc_status(fr_pvc, DLCI_PRESENT);
	} else
            fr_set_pvc_status(fr_pvc, DLCI_PRESENT);

        if (out_status != fr_pvc->pvc_status) {
            GET_TIMESTAMP(fr_pvc->pvc_change_time);
            fr_in_idb->fr_lmi->full_status_req = TRUE;
        }
    }
}


/*
 * show lmi statistics
 * these strings should correspond to the enums (FR_LMI_TYPE)
 * found in fr_lmi.h
 */
static char *const disp_lmi[3] = {"CISCO", "CCITT", "ANSI"};

void show_lmi_stats (parseinfo *csb)
{
    fr_idb_struct_type *fr_idb;
    hwidbtype *idb;
    boolean screenfull;

    screenfull = FALSE;

    automore_enable(NULL);
    if (GETOBJ(idb,1)) {
	idb = GETOBJ(idb,1)->hwptr;
	if (is_frame_relay(idb) && !idb->nokeepalive) {
	    fr_idb =  idb->frame_relay_stuff;
	    printf("\n\nLMI Statistics for interface %s (Frame Relay %s) LMI TYPE = %s",
		    idb->hw_namestring, disp_fr_intf[fr_idb->fr_intf_type],
		    disp_lmi[lmi_idb->fr_lmi_type-1]);
	    show_lmi_idb_stats(fr_idb);
	} else {
	    printf("\n Invalid interface specified");
	    return;
	}

    } else {
	FOR_ALL_HWIDBS(idb) {
	    if (is_frame_relay(idb) && !idb->nokeepalive) {
		if (screenfull) {
		    automore_conditional(0);
		} else {
		    screenfull = TRUE;
		}
		if (is_ddr(idb) && reg_invoke_dialer_huntgroup_member(idb))
		    continue;	/* go by the master only, so skip the member */
		fr_idb =  idb->frame_relay_stuff;
		printf("\n\nLMI Statistics for interface %s (Frame Relay %s) LMI TYPE = %s",
			idb->hw_namestring, disp_fr_intf[fr_idb->fr_intf_type],
			disp_lmi[lmi_idb->fr_lmi_type-1]);
		show_lmi_idb_stats(fr_idb);
	    }
	}
    }
    automore_disable();
}


/*
 * show_lmi_idb_stats
 */

void show_lmi_idb_stats (fr_idb_struct_type *fr_idb)
{
    fr_lmi_stats_type lmi_stats;

    lmi_stats = *(lmi_idb->lmi_stats);

    printf ("\n  Invalid Unnumbered info %d",
	    lmi_stats.invalid_ui);
    printf ("\t\tInvalid Prot Disc %d",
	    lmi_stats.invalid_protocol_discriminator);
    printf ("\n  Invalid dummy Call Ref %d",
	    lmi_stats.invalid_dummy_call_ref);
    printf ("\t\tInvalid Msg Type %d",
	    lmi_stats.invalid_msg_type);
    printf ("\n  Invalid Status Message %d",
	    lmi_stats.invalid_msg_type);
    printf ("\t\tInvalid Lock Shift %d",
	    lmi_stats.invalid_locking_shift);
    printf ("\n  Invalid Information ID %d",
	    lmi_stats.invalid_ie_id);
    printf ("\t\tInvalid Report IE Len %d",
	    lmi_stats.invalid_report_type_ie_len);
    printf ("\n  Invalid Report Request %d",
	    lmi_stats.invalid_report_request);
    printf ("\t\tInvalid Keep IE Len %d",
	    lmi_stats.invalid_keepalive_ie_len);

    switch (fr_idb->fr_intf_type) {

    case FR_DTE :
	printf ("\n  Num Status Enq. Sent %d",
		lmi_stats.num_enquiries_sent);
	printf ("\t\tNum Status msgs Rcvd %d",
		lmi_stats.num_status_rcvd);
	printf ("\n  Num Update Status Rcvd %d",
		lmi_stats.num_upd_status_rcvd);
	printf ("\t\tNum Status Timeouts %d",
		lmi_stats.num_dte_timeouts);
	break;

    case FR_DCE :
	printf ("\n  Num Status Enq. Rcvd %d",
		lmi_stats.num_enquiries_rcvd);
	printf ("\t\tNum Status msgs Sent %d",
		lmi_stats.num_status_sent);
	printf ("\n  Num Update Status Sent %d",
		lmi_stats.num_upd_status_sent);
        printf ("\t\tNum St Enq. Timeouts %d",
                lmi_stats.num_dce_timeouts);
	break;

    case FR_NNI :
	printf ("\n  Num Status Enq. Rcvd %d",
		lmi_stats.num_enquiries_rcvd);
	printf ("\t\tNum Status msgs Sent %d",
		lmi_stats.num_status_sent);
        printf ("\n  Num Update Status Rcvd %d",
                lmi_stats.num_upd_status_rcvd);
        printf ("\t\tNum St Enq. Timeouts %d",
                lmi_stats.num_dce_timeouts);
        printf ("\n  Num Status Enq. Sent %d",
                lmi_stats.num_enquiries_sent);
        printf ("\t\tNum Status msgs Rcvd %d",
                lmi_stats.num_status_rcvd);
        printf ("\n  Num Update Status Sent %d",
                lmi_stats.num_upd_status_sent);
        printf ("\t\tNum Status Timeouts %d",
                lmi_stats.num_dte_timeouts);
	break;
    }
}



/*
 * fr_lmi_start
 *      Create the FR LMI process and LMI master timer
 */
static void fr_lmi_start(void)
{
    fr_lmi_pid = process_create(fr_lmi_process, "FR LMI", NORMAL_STACK,
				PRIO_HIGH);
    if (fr_lmi_pid == NO_PROCESS) {
	return;
    }
}


/*
 * fr_lmi_init
 *	initialize FR LMI support
 */
void fr_lmi_init()
{
    fr_lmi_timer = malloc_named(sizeof(mgd_timer), "FR LMI Init Timer");
    mgd_timer_init_parent(fr_lmi_timer, NULL);
    
    if (fr_lmi_timer == NULL) {
	buginf("\n%% Failed to create FR LMI TIMER");
    }
    reg_add_fr_proc_create(fr_lmi_start, "Create and start LMI Process" );
    reg_add_fr_idb_init(fr_lmi_idb_init, "fr_lmi_idb_init");
    reg_add_fr_idb_uninit(fr_lmi_idb_uninit, "fr_lmi_idb_uninit");
    reg_add_fr_mgd_timer_interval(FR_LMI_TIMER, fr_lmi_timer_interval,
				  "fr_lmi_timer_interval");
    reg_add_fr_mgd_timer_onoff(fr_lmi_timer_onoff, "fr_lmi_timer_onoff");
    reg_add_clear_counters(fr_init_lmi_stats, "init the lmi counters");
}


/*
 * fr_lmi_timer_create
 *	create lmi timers. return TRUE if it is successful, FALSE otherwise.
 */
boolean fr_lmi_timer_create(hwidbtype *hwidb, ulong timertype)
{
    fr_idb_struct_type *fr_idb;
    mgd_timer *new_lmi_timer;

    fr_idb = NULL;
    if (hwidb)
	fr_idb = hwidb->frame_relay_stuff;
    if (!hwidb || !fr_idb)
	return FALSE;

    new_lmi_timer = malloc_named(sizeof(mgd_timer), "FR LMI Timer");
    if (!new_lmi_timer) {
	buginf("\n%% fail to create fr lmi idb timer");
	return FALSE;
    }
    
    switch (timertype) {
    case LMI_IDB_TIMER:
	/* don't create a duplicate timer */
	if (lmi_idb->idb_timer != NULL)
	    break;
 
	mgd_timer_init_leaf(new_lmi_timer, fr_lmi_timer, LMI_IDB_TIMER,
			    hwidb, FALSE);
	if (hwidb->state == IDBS_UP && hwidb->keep_period > 0) {
	    mgd_timer_start(new_lmi_timer, hwidb->keep_period * ONESEC);
	}
	lmi_idb->idb_timer = new_lmi_timer;
	break	;
    case LMI_ERR_TIMER:
	/* don't create a duplicate timer */
	if (lmi_idb->err_timer != NULL)
	    break;
 
	mgd_timer_init_leaf(new_lmi_timer, fr_lmi_timer, LMI_ERR_TIMER,
			    hwidb, FALSE);
	if (hwidb->state == IDBS_UP)
	    mgd_timer_start(new_lmi_timer, lmi_idb->fr_lmi_t392dce * ONESEC);
	lmi_idb->err_timer = new_lmi_timer;
	break;
    case LMI_ROUTE_TIMER:
	/* don't create a duplicate timer */
	if (lmi_idb->route_timer != NULL)
	    break;
 
	mgd_timer_init_leaf(new_lmi_timer, fr_lmi_timer, LMI_ROUTE_TIMER,
			    hwidb, FALSE);
	if (hwidb->state == IDBS_UP)
	    mgd_timer_start(new_lmi_timer, LMI_ROUTE_INTERVAL * ONESEC);
	lmi_idb->route_timer = new_lmi_timer;
	break;
    default:
	free(new_lmi_timer);
	return FALSE;
    }
    return TRUE;
}


/*
 * fr_lmi_timer_delete
 * 	Delete LMI timers
 */
void fr_lmi_timer_delete (hwidbtype *hwidb, ulong timertype)
{
    fr_idb_struct_type *fr_idb;
    mgd_timer **dead_timer;

    fr_idb = hwidb->frame_relay_stuff;
    
    switch (timertype) {
    case LMI_IDB_TIMER:
	dead_timer = &lmi_idb->idb_timer;
	break;
    case LMI_ERR_TIMER:
	dead_timer = &lmi_idb->err_timer;
	break;
    case LMI_ROUTE_TIMER:
	dead_timer = &lmi_idb->route_timer;
	break;
    default:
	dead_timer = NULL;
    }
    if (dead_timer != NULL) {
	mgd_timer_stop(*dead_timer);
	free(*dead_timer);
	*dead_timer = NULL;
    }
}


/*
 * lmi_idb_timeout
 *	DTE lmi keepalive timer timeout routine
 */
static void lmi_idb_timeout(mgd_timer *expired_idb_timer)
{
    hwidbtype *hwidb;
    fr_idb_struct_type 	*fr_idb;
    ulong interval;
    
    hwidb = mgd_timer_context(expired_idb_timer);
    if (!hwidb || hwidb->state != IDBS_UP ||  /* don't bother if line's down */
	!(fr_idb = hwidb->frame_relay_stuff)) {
	mgd_timer_stop(expired_idb_timer);
	return;
    }
    if (is_ddr(hwidb)) {
	/* get the real hwidb depending on which is connected, for lmi */
	if (!(hwidb = reg_invoke_dialer_get_conn_member(hwidb))) {
	    mgd_timer_stop(expired_idb_timer);
	    return;
	}
    }
    /* 
     * Update the status for switched PVCs on an interface 
     * configured as a FR DTE. Switched PVCs on an interface 
     * configured as a FR DCE are updated within route_status_timer(). 
     */

    interval = hwidb->keep_period;
    mgd_timer_update(expired_idb_timer, interval * ONESEC);

    /*
     * Verify that we have received a Status message
     */
    if (fr_idb->fr_intf_type != FR_DCE) {
	if (lmi_idb->status_rcvd) {
          lmi_idb->status_rcvd = FALSE;
          fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_PASS, 
                       lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
      } else {
          fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
                       lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
          lmi_idb->lmi_stats->num_dte_timeouts++;
      }
    }

    fr_lmi_chk(hwidb);

    /*
     * During autoinstall, let autoinstall control the lmi
     * till the right lmi type is found.
     */
    if (fr_idb->fr_bootp->fr_ai_need_fs == TRUE)
	return;
    /* 
     * Intelligent retries for LMI autosense. 
     * Do it only once a minute(by default). If N391 changes, 
     * this interval will change. The user is obviously confortable 
     * with the N391 interval. 
     */
    if ((fr_idb->fr_intf_type == FR_DTE) &&
	(!(lmi_idb->lmi_autosense_retry_count++ 
	   % lmi_idb->fr_lmi_n391dte))) {
	if ((lmi_idb->fr_lmi_type_configured == FALSE) 
	    && (hwidb->state == IDBS_UP)
	    && (fr_idb->fr_lineup == FALSE)) {
	    fr_initiate_autosense(hwidb);
	}
    }
    
    send_frame_relay_keepalive(hwidb); /* if autosense is not on */
    lmi_idb->lmi_stats->num_enquiries_sent++;
    lmi_idb->lmi_n391dte++;
}



/*
 * lmi_err_timeout
 *	DCE/NNI interface checks if a full status request is received.
 */
static void lmi_err_timeout(mgd_timer *expired_err_timer)
{
    fr_idb_struct_type *fr_idb;
    ulong interval;
    hwidbtype *hwidb;

    hwidb = mgd_timer_context(expired_err_timer);
        
    if (!hwidb || !(fr_idb = hwidb->frame_relay_stuff) ||
	(fr_idb->fr_intf_type == FR_DTE)) {
	mgd_timer_stop(expired_err_timer);
        return;
    }

    /*
     * If we get here the LMI DCE timer expired and we did not receive
     * a Status Enquiry: log an error.
     */
    if (frame_relay_lmi_debug(hwidb))
        buginf("\n%s(%s): DCE LMI timeout", hwidb->hw_namestring,
	       hw_interface_up(hwidb) ? "up" : "down");
    fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
		 lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
    lmi_idb->lmi_stats->num_dce_timeouts++;
    
    interval = lmi_idb->fr_lmi_t392dce;
    mgd_timer_update(expired_err_timer, interval * ONESEC);    
    fr_lmi_chk(hwidb);
}


/*
 * lmi_route_timeout
 *	Timeout routine for lmi_route_timer (route status timer)
 */
static void lmi_route_timeout(mgd_timer *expired_route_timer)
{
    hwidbtype *hwidb;
    
    hwidb = mgd_timer_context(expired_route_timer);
    mgd_timer_update(expired_route_timer, 10 * ONESEC);
    route_status_timer(hwidb);
}


/*
 * fr_lmi_timeout
 *	Handle all the lmi timeout here. Find out the type of timer which
 *      went off and invoke that specific timer timeout routine.
 */
static void fr_lmi_timeout(void)
{
    mgd_timer *expired_timer;
    
    while (mgd_timer_expired(fr_lmi_timer)) {
	expired_timer = mgd_timer_first_expired(fr_lmi_timer);
	if (expired_timer == NULL) {
	    /*
	     * shouldn't come here since we woke up by a timer event and
	     * no timer expired. Stop the timer and return.
	     */
	    buginf("INVarp timer disturbed! \n");
	    return;
	}
	switch (mgd_timer_type(expired_timer)) {
	case LMI_IDB_TIMER:
	    lmi_idb_timeout(expired_timer);
	    break;
	case LMI_ERR_TIMER:
	    lmi_err_timeout(expired_timer);
	    break;
	case LMI_ROUTE_TIMER:
	    lmi_route_timeout(expired_timer);
	    break;
	default:
	    buginf("UNEXPECTED TIMER TYPE\n");
	}
    }
}


/*
 * fr_lmi_packets
 *	LMI packets handling routin
 */
static void fr_lmi_packets(void)
{
    hwidbtype *hwidb;
    paktype *pak;
    int count;
    
    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	/*
	 * Only process a limited number per pass.
	 */
	pak = process_dequeue(fr_lmiQ);
	if ( !pak )
	    break;
	hwidb = hwidb_or_null(pak->if_input);
	if (!hwidb) {
	    buginf("\nfr_lmi_packets: source idb not set, PC %#x", current_pc());
	    datagram_done(pak);
	    continue;
	}
	if (hwidb->nokeepalive) {
	    datagram_done(pak);
	    continue;
	}
	GET_TIMESTAMP(hwidb->lastinput);      /* remember time of last input */
	pak->inputtime = hwidb->lastinput;    /* save it in packet too */
	if (hwidb->state != IDBS_UP) {	/* if hardware not up */
	    protocol_discard(pak, TRUE);
	    continue;		       	/* and return */
	}
	lmi_parse(hwidb,pak);
    }
}



/*
 * fr_lmi_process
 * handle incoming LMI packets
 */
forktype fr_lmi_process (void)
{
    ulong major, minor;
    
    fr_lmiQ = create_watched_queue("FR LMI packet", 0, 0);

    process_watch_queue(fr_lmiQ, ENABLE, RECURRING);
    process_watch_mgd_timer(fr_lmi_timer, ENABLE);
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case QUEUE_EVENT:
		fr_lmi_packets();
		break;
	    case TIMER_EVENT:
		fr_lmi_timeout();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * lmi_parse
 */

void lmi_parse (
    hwidbtype *idb,
    paktype *pak)
{
    uchar *data_ptr;
    uchar msg_type;
    struct frame_relay_lmi *lmi;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    hwidbtype *netidb;
    int i;

    fr_idb =  idb->frame_relay_stuff;
    if (!fr_idb) { /* this should never happen */
	datagram_done(pak);
        return;
    }
    if (!fr_lmi_hdr_check(idb, pak)) { 
	if (fr_idb->fr_intf_type == FR_DCE)
            fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
	else
            fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
 	datagram_done(pak);
 	return;
    }

    /*
     * For ISDN and dialers, idb points to the physical interface (B-ch, Ser),
     * but we need the network protocol information that resides on the rotary
     * group leader (D-ch, Dialer), so get it here.
     */
    netidb = get_netidb(idb);

    /* Initialize the temp status for all subifs to down
     * We only care about multipoint interfaces but it's faster
     * not to check since the values are unused in the non-multipoint case.
     * Any active DLCI means the multipoint subif is up
     */

    lmi = (struct frame_relay_lmi *)((uchar *)&pak->datagramstart);
    data_ptr = pak->datagramstart+FRAME_RELAY_LMI_ENCAPBYTES;
    data_ptr += FRAME_RELAY_LMI_OVERHEADBYTES-FRAME_RELAY_LMI_ENCAPBYTES;
    lmi_idb->fr_liv_seen =  FALSE;
    lmi_idb->fr_dlci_status_change = FALSE;
    lmi_idb->fr_mc_seen = FALSE;
    lmi_idb->fr_rpt_type_processing = FR_REPORT_NOTSEEN; /*Track async updates*/
    msg_type = *data_ptr++;

/* Don't accept unsolicited duplicate LMI responses */
    if ((msg_type == LMI_STATUS) && (lmi_idb->fr_lmi_state != FR_LMI_SENT)) {
	datagram_done(pak);
	return;
    }
    switch (msg_type) {
    case LMI_STATUS:
        lmi_idb->lmi_stats->num_status_rcvd++;
	break;
    case LMI_STATUS_ENQUIRY:
	if (fr_switching && (fr_idb->fr_intf_type != FR_DTE)) {
	    /*
	     * if we received a STATUS_ENQUIRY message then if
	     * switching is enabled and the interface is a frame relay DCE
	     * we need to respond with a STATUS msg. But before that
	     * we need to parse this msg and make sure that there are no
	     * errors
	     */
	    parse_fr_switch_lmi(idb, pak, data_ptr);
            if (idb->inloopback == TRUE)
                idb->inloopback = FALSE;
	} else {
	    if (fr_is_looped(fr_idb, pak, data_ptr,idb))
		    idb->inloopback = TRUE;
	    if (frame_relay_lmi_debug(idb)) 
                buginf("\n%s(in): Unexpected StEnq", idb->hw_namestring);
	}
        lmi_idb->lmi_stats->num_enquiries_rcvd++;
	datagram_done(pak);
	return;
	
    case LMI_UPDATE_STATUS:
	lmi_idb->lmi_stats->num_upd_status_rcvd++;
	break;
    default:
	if (frame_relay_lmi_debug(idb)) {
	    buginf("\n%s(i):Unknown LMI ", idb->hw_namestring);
	    hexdump_packet(pak, (ulong)IDB_SERIAL);
	}
	lmi_idb->lmi_stats->invalid_msg_type++;
	fr_idb->fr_err_dlci = lmi_idb->fr_lmi_dlci;
        if (fr_idb->fr_intf_type == FR_DCE)
            fr_lmi_event(fr_idb, FR_INTF_DCE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dce, lmi_idb->fr_lmi_n393dce);
        else
            fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
 	datagram_done(pak);
 	return;
    }
    if (fr_idb->fr_intf_type != FR_DCE) { /* do this if DTE or NNI */
        if (idb->inloopback == TRUE)
            idb->inloopback = FALSE;
	if (lmi_idb->fr_lmi_type == FR_ANSI_D) {
	    if (*data_ptr != LOCK_SHIFT) {
                lmi_idb->lmi_stats->invalid_locking_shift++;
		if (frame_relay_lmi_debug(idb))
		    buginf("\n%s: Invalid locking shift value 0x%x", 
			   idb->hw_namestring, *data_ptr);
		fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
				lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
		datagram_done(pak);
		return;
	    }
	    data_ptr++;
	}
	if (frame_relay_lmi_debug(idb) || serial_debug)
            buginf("\n%s(in): %s, myseq %u", idb->hw_namestring, 
		(msg_type == LMI_STATUS) ? "Status" : "UpSta", 
		lmi_idb->fr_dte_myseq);
	if (frame_relay_parse_ie_pkt(idb,pak,data_ptr) == NULL)
	    fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
	else 
	    if ((msg_type == LMI_UPDATE_STATUS) && (lmi_idb->fr_liv_seen ||
			(lmi_idb->fr_rpt_type_processing != FR_REPORT_NOTSEEN)))
		fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
	else
	    if ((msg_type == LMI_STATUS ) &&
		(!lmi_idb->fr_liv_seen ||
		 (lmi_idb->fr_rpt_type_processing == FR_REPORT_NOTSEEN)))
		fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
	else
	    if (lmi_idb->fr_liv_seen && (lmi_idb->fr_dte_tmp_yoursseen == 0))
		fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_FAIL, 
			    lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
	    else {
		fr_lmi_event(fr_idb, FR_INTF_DTE, FR_LMI_PASS, 
			     lmi_idb->fr_lmi_n392dte, lmi_idb->fr_lmi_n393dte);
		lmi_idb->status_rcvd = TRUE;
		lmi_idb->fr_dte_yoursseen = lmi_idb->fr_dte_tmp_yoursseen;
		if ( lmi_idb->fr_rpt_type_processing == FR_REPORT_FULL_STATUS )
		    lmi_idb->full_status_rcvd = TRUE;
/* We have a good LMI. Change to state waiting to send */
		if (msg_type == LMI_STATUS)
		    lmi_idb->fr_lmi_state = FR_LMI_RCVD;
		/*
		    * don't mark PVC deleted if LMI_UPDATE_STATUS msg
			*/
		if (msg_type == LMI_STATUS) {
		    
		    /*
		     * now check the pvc table to see if the PVC is deleted
		     */
		    if (lmi_idb->full_status_rcvd) {
			for (i = MIN_DLCI; i < MAXDLCI; i++) {
			    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
				continue;
			    if (i == DLCI_TO_NUM(fr_idb->fr_multicast_dlci))
				continue;
			    if (fr_pvc->pvc_type == SVC_DYNAMIC)
                                continue;
			    if (fr_pvc->pvc_upd_seq != lmi_idb->fr_dte_myseq) {
				if (fr_idb->fr_intf_type != FR_DTE)
				    RESET_PVC_ACTIVE(fr_pvc->nni_pvc_status);
				else
				    if (!IS_DLCI_DELETED(fr_pvc->pvc_status)) {
 					fr_set_pvc_status(fr_pvc, DLCI_DELETED);
					fr_set_subif_state(fr_pvc->idb,
							   FR_PVC_STATE,
							   fr_pvc->pvc_status);
				    }
			    }

/* Clear PVC and maps that are no longer active or if PVC doesn't exist */
			    if ((fr_pvc->pvc_status == DLCI_DELETED) ||
				!DLCI_ACTIVE(fr_pvc->pvc_status))
				fr_clear_dynamic_entries(netidb, LINK_ILLEGAL,
							 i, FR_BOTH |
							 FR_LMI_CLEAR);

/*
 * Free the PVC struct and stats info
*/

			if ((fr_pvc->pvc_status == DLCI_DELETED) &&
                            ((fr_pvc->pvc_type == PVC_DYNAMIC) ||
                            (fr_pvc->pvc_usage == PVC_NOTSET)))   
			fr_pvc_clear(fr_pvc->idb->hwptr, i);      
 


			}
			reg_invoke_fr_inarp_send(idb);
			if (lmi_idb->fr_dlci_status_change == TRUE)
			    reg_invoke_bump_cache_version(idb, NULL, TRUE);
/*
 * Only reset our counter if we requested the full status
 */
			if (lmi_idb->fr_dte_rpt_type_sent == FR_REPORT_FULL_STATUS)
			    lmi_idb->lmi_n391dte = 1;
			if ((fr_idb->fr_multicast_status == FR_MULTICAST_DYNAMIC)
			    && !lmi_idb->fr_mc_seen)
			    fr_mc_delete_dlci(idb,
					      DLCI_TO_NUM(fr_idb->fr_multicast_dlci));
		    }
		    
		}
	    }
    }
    datagram_done(pak);
}


/*
 * set_fr_keepalive
 * Here on SERVICE_KEEPALIVE_CHANGED
 */

void set_fr_keepalive (hwidbtype *hwidb)
{
    fr_pvc_info_type *fr_pvc;
    int i;
    
    fr_idb_struct_type *fr_idb = hwidb->frame_relay_stuff;
    
    /*
     * handle frame relay keepalives 
     */
    if (hwidb->enctype != ET_FRAME_RELAY)
	return;
    if (hwidb->nokeepalive) {
	if (!fr_idb->fr_lineup) {
	    fr_idb->fr_lineup = TRUE;
	    reg_invoke_Lapf_sys_event(FR_MDL_ASSIGN, fr_idb);
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	}
	fr_reset_lmi_events(fr_idb, FR_INTF_DTE, FR_LMI_PASS,
			    0, lmi_idb->fr_lmi_n393dte);
	fr_reset_lmi_events(fr_idb, FR_INTF_DCE, FR_LMI_PASS,
			    0, lmi_idb->fr_lmi_n393dce);
	for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
	    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
		continue;
	    fr_set_subif_state(fr_pvc->idb, FR_HWIF_STATE, (uchar)null);
	    fr_set_pvc_status(fr_pvc, DLCI_ADDED);
	}
    }
}

/* 
 * fr_initiate_autosense
 * Send STATUS enquiries in all 3 flavors and attempt to 
 * establish the LMI flavor. Use FR_N391_COUNTER_MAX so that 
 * irrespective of the user-configured N391 value, a full 
 * status is requested. 
 */
void fr_initiate_autosense (
	hwidbtype 	*hwidb)
{
    int i;
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;
    for (i = FR_ANSI_D; i > 0 ; i--) {
	lmi_idb->lmi_n391dte = FR_N391_COUNTER_MAX;
	lmi_idb->lmi_autosense = TRUE;
	fr_lmi_set(hwidb, fr_idb, i, TRUE, FALSE);
	lmi_idb->fr_dte_rpt_type_sent = FR_REPORT_FULL_STATUS;
	send_frame_relay_keepalive(hwidb);
    }
}


/* 
 * fr_ie_length_check
 * Check length of the appropriate LMI message. 
 */
int fr_ie_length_check(ushort msg_type, ushort length)
{
    const fr_ie_length *ptr;

    ptr = ie_length;

    while (ptr && ptr->ie_type && (ptr->ie_type != msg_type)) {
    	ptr++;
	continue;
    }
    if (ptr->ie_type == 0)
	return(FALSE);
   
    if ((ptr->ie_length == length) || ((!ptr->ie_length) &&
		(length >= ptr->ie_min_length) &&
			(length <=  ptr->ie_max_length)))
	return(ptr->case_success);
    return(ptr->case_failure);
}

      

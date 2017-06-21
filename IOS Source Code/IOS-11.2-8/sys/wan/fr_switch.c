/* $Id: fr_switch.c,v 3.9.10.14 1996/09/11 16:49:15 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_switch.c,v $
 *------------------------------------------------------------------
 * frame relay switch for serial/HSSI and IP tunnel.
 *
 * Summer 92, Sanjeev Newarikar
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *  fr_switch.c -- frame relay switching over serial and IP tunnel.
 *                 configuration for switch and switch LMI in 4-company
 *                 ANSI Annex D and CCITT formats. A myriad of statistics and
 *                 error correction for LMI and PVC's.
 *                 LMI across the tunnel for the status of remote pvc's
 *------------------------------------------------------------------
 * $Log: fr_switch.c,v $
 * Revision 3.9.10.14  1996/09/11  16:49:15  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.9.10.13  1996/07/29  22:11:39  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.9.10.12  1996/07/17  06:12:18  fowler
 * CSCdi57463:  Frame relay broadcast queue should use new IDB list
 * functions
 *              Remove several instances of FOR_ALL_HWIDBS and replace
 *              with walks of the FR idb list
 * Branch: California_branch
 *
 * Revision 3.9.10.11  1996/07/10  07:37:41  chifei
 * CSCdi60658:  Clear counters clears frame relay MIB objects
 * Branch: California_branch
 *
 * Revision 3.9.10.10  1996/07/02  23:11:23  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.9.10.9  1996/06/28  23:32:03  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.10.8  1996/06/18  23:53:03  ccpoe
 * CSCdi60277:  priority-group command cannot be deleted from map-class
 * Branch: California_branch
 *
 * Revision 3.9.10.7  1996/05/31  18:19:31  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.9.10.6  1996/05/30  18:36:07  ccpoe
 * CSCdi58545:  FR Tunneling pvc was not shown by "show frame-relay pvc"
 * command
 * Branch: California_branch
 *
 * Revision 3.9.10.5  1996/05/18  13:36:39  ccpoe
 * CSCdi57877:  Frame relay traffic shaping breaks modular images
 * Branch: California_branch
 *
 * Revision 3.9.10.4  1996/05/15  05:54:43  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.9.10.3  1996/04/30  23:27:25  chifei
 * CSCdi52339:  Frame relay DTE/DCE switching across ip tunnel broken
 * Branch: California_branch
 * Enabled the full functionality of FR switching over IP tunnel,
 * also fixed regular switching function which was broken by improper
 * handling of the "frame intf-type" command.
 *
 * Revision 3.9.10.2  1996/04/25  23:23:55  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.9.10.1.2.1  1996/04/24  04:11:36  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.9.10.1  1996/03/18  22:47:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.2  1996/03/16  07:55:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.1  1996/03/13  02:11:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9  1996/02/26  18:56:46  rtio
 * CSCdi48935:  enum FR_LMI_TYPE out of sync with disp_lmi
 *
 * Added comments to frame_relay.h pointing to static variables in
 * fr_switch.c.  Fixed order of strings in disp_lmi[] in fr_switch.c.
 *
 * Revision 3.8  1996/01/22  07:45:31  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/05  10:18:24  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/13  20:04:18  ccpoe
 * CSCdi43914:  tunnel interface does not come up. Alignment error in
 * add_ansi_ccitt_pvc.
 *
 * Revision 3.5  1995/12/12  06:19:01  shankar
 * CSCdi45206:  Router crash when show frame pvc was executed
 *
 * Revision 3.4  1995/11/30  06:23:51  ccpoe
 * CSCdi44651:  ALIGN-3-SPURIOUS: Spurious memory access when tunneling
 *
 * Revision 3.3  1995/11/17  18:02:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:35:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/12  21:00:03  ccpoe
 * CSCdi41674:  bad getbuffer, bytes= 524293
 *
 * Revision 2.10  1995/09/13  21:00:01  fowler
 * CSCdi36199:  Frame relay IETF encapsulation for OSI doesnt comply with
 * 1490
 *
 * Revision 2.9  1995/08/23  17:14:42  ccpoe
 * CSCdi38822:  Frame relay status is not accurate in show frame PVC/map.
 * fr_set_pvc_status() is now used to change the pvc_status.
 *
 * Revision 2.8  1995/08/08  16:50:26  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.7  1995/08/04 21:30:42  fowler
 * CSCdi37783:  FR broadcast queue count incremented for for all broadcasts
 *
 * Revision 2.6  1995/08/02  21:26:33  ccpoe
 * CSCdi37746:  FR PVCs may become out of sync with subif state.
 * Subinterface comes back after it is deleted. fr_if_statechange_complete
 * is implemented to make subif state in sync with PVC state.
 *
 * Revision 2.5  1995/08/01  16:59:38  ccpoe
 * CSCdi37323:  Frame relay not counting FECN/BECN/DE incoming when fast
 * switching. Since we don't set FECN/BECN bit unless the packet is
 * frame-relay switched, counting incoming FECN/BECN/DE suffices the
 * requirement.
 *
 * Revision 2.4  1995/06/21  03:30:51  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.3  1995/06/20  20:38:11  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/18  06:19:30  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 23:17:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/old_timer_callbacks.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "frame_relay.h"
#include "fr_lmi.h"
#include "fr_tunnel.h"
#include "fr_class.h"
#include "address.h"
#include "../if/network.h"
#include "fr_switch.h"
#include "../wan/fr_registry.h"
#include "../wan/sr_frmib.h"
#include "../wan/dialer_registry.h"


/*
 * local storage
 */
char *const disp_fr_intf[3] = {"DTE", "DCE", "NNI"};

/*
 * create_fr_pvc
 *
 * create fr pvc and return a pointer to pvc or else return null
 */

fr_pvc_info_type * create_fr_pvc (void)
{
    fr_pvc_info_type *fr_pvc;
    fr_pvc_stats_type *pvc_stats;


    fr_pvc = malloc_named(sizeof(fr_pvc_info_type), "FR Create PVC");
    if (!fr_pvc) {
	printf (nomemory);
	return(NULL);
    }
    /*
     * try to allocate for the stats. if not return
     */
    pvc_stats = &fr_pvc->pvc_stats;
    if (!pvc_stats) {
	printf ("\ncan not allocate memory for pvc statistics");
	free (fr_pvc);   /* free pvc too */
	return(NULL);
    }
    /*
     * init stats
     */
    clear_fr_pvc_stats(pvc_stats);
    fr_pvc->stats_save = 0;

    fr_pvc->pvc_intime = 0;
    fr_pvc->pvc_outtime = 0;
    GET_TIMESTAMP(fr_pvc->pvc_create_time);
    GET_TIMESTAMP(fr_pvc->pvc_change_time);

    /*
     * temporarily default. Switch will update us only through cisco LMI. 
     */
    fr_pvc->pvc_bw = FR_PVC_BW_NOTSET;
    return(fr_pvc);

}

/*
 * parse_fr_route
 *
 * parse the route command for details of routing over serial or IP
 * the command is [no] fr route <in dlci> <out intf> <out dlci>
 */

void parse_fr_route (parseinfo *csb, idbtype *idb)
{
    hwidbtype *in_idb;
    hwidbtype *out_idb = NULL;
    ushort in_dlci, out_dlci;
    fr_route_type *route;

    in_idb = hwidb_or_null(csb->interface);
    in_dlci = (ushort)GETOBJ(int,1);          /* this is the incoming DLCI */
    route = dsearch_fr_route(in_dlci, in_idb);

    if (!csb->sense) {
	if (!route) 
	    printf("\nNo such entry in the route table");
	else
	    delete_fr_route(route);
    } else {   /* if not a no route entry */
	if (route) {
	    printf("\nCan't use same dlci on overlapping circuits");
	} else {
    	    out_idb = hwidb_or_null(GETOBJ(idb,1));
    	    if (out_idb == in_idb) {
        	printf("\nCan not switch on same interface");
        	return;
    	    }
    	    out_dlci = (ushort)GETOBJ(int,2);    /* this is the outgoing DLCI */
	    add_fr_route(in_dlci, in_idb, out_dlci, out_idb);
	}
    } /* end of else */
}

/*
 * dsearch_fr_route
 *
 * search a entry in the route table based on DLCI & idb
 */

fr_route_type *dsearch_fr_route (
    ushort in_dlci,
    hwidbtype *in_idb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;

    fr_idb = in_idb->frame_relay_stuff;
    if (!fr_idb) {
        return(NULL);
    }
    fr_pvc = fr_idb->fr_dlci_tab[in_dlci];
    if (fr_pvc) {
	if (fr_pvc->route_flag) {
	    if (fr_pvc->fr_route.in_idb == in_idb)
	        return(&fr_pvc->fr_route);
	}
    }
    return(NULL);
}

/*
 * add_fr_route
 *
 * add a new route entry in the hash table
 */

void add_fr_route (
    ushort in_dlci,
    hwidbtype *in_idb,
    ushort out_dlci,
    hwidbtype *out_idb)
{
    fr_pvc_info_type *fr_pvc, *out_pvc;
    fr_idb_struct_type *fr_idb;

    if (!(fr_idb = in_idb->frame_relay_stuff))
        return;
    fr_pvc = fr_idb->fr_dlci_tab[in_dlci];
    if (!fr_pvc) {
	fr_pvc = create_fr_pvc ();
	if (!fr_pvc)
	    return;

	fr_pvc->pvc_bw = FR_PVC_BW_NOTSET;
	fr_pvc->route_flag = FALSE;
	fr_pvc->map_flag = FALSE;
	fr_pvc->idb = in_idb->firstsw;    /* added for fr MIB */
	fr_pvc->this_dlci = in_dlci;  /* added for fr MIB */
	/*
	 * make assignments at the end
	 */
	fr_idb->fr_dlci_tab[in_dlci] = fr_pvc;
	if (fr_pvc->pvc_type != SVC_DYNAMIC)
	    fr_idb->num_idb_pvcs++;  /* increment no. of idb pvc's */
	reg_invoke_fr_dlci_class_inht(fr_pvc->idb, fr_pvc->this_dlci, NULL,
				      ADD_CLASS, fr_pvc->pvc_inht);
    }
    if (fr_pvc->map_flag) {
	printf ("\n can't route on this dlci as a map exists");
	return;
    }
    if (fr_pvc->route_flag) {
	printf ("\nroute already exists for input dlci");
	return;
    }
    /*
     * set the pvc to new and inactive
     */
    fr_pvc->pvc_status = PVC_NEW_BIT;
    if (fr_idb->fr_intf_type == FR_DCE)
        SET_PVC_ACTIVE(fr_pvc->nni_pvc_status);
    else
        RESET_PVC_ACTIVE(fr_pvc->nni_pvc_status);
    fr_pvc->pvc_congestion = NOT_CONGESTED;

    if ((out_idb->status & IDB_SERIAL) != 0)
	fr_pvc->pvc_usage = PVC_SWITCHED_SERIAL;
    fr_pvc->pvc_type = PVC_DYNAMIC;  /* always consider route pvc dynamic */
 
    fr_pvc->fr_route.in_dlci = in_dlci;
    fr_pvc->fr_route.in_idb = in_idb;
    fr_pvc->fr_route.out_dlci = out_dlci;
    fr_pvc->fr_route.out_idb = out_idb;
    fr_pvc->route_flag = TRUE;
    /*
     * check if we are routing on the tunnel. If so, create the tunnel pvc
     */
    reg_invoke_fr_tunnel_pvc(fr_pvc, &out_pvc, FR_TUNNEL_ADD);
    
    /*
     * there is a new route: set full_status_req flag to force transmission
     * of Full Status LMI as response to the next Status Enquiry
     */
    if (fr_idb->fr_intf_type != FR_DTE)
	lmi_idb->full_status_req = TRUE;
}

/*
 * delete_fr_route
 *
 * delete a particular route entry as specified by the user
 */

void delete_fr_route (fr_route_type *del_route)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;

    if (!(fr_idb = del_route->in_idb->frame_relay_stuff))
        return;
    if (!(fr_pvc = fr_idb->fr_dlci_tab[del_route->in_dlci]))
        return;
    if (!fr_pvc->route_flag)
        return;

    if (fr_pvc->fr_route.in_idb == del_route->in_idb &&
	fr_pvc->fr_route.in_dlci == del_route->in_dlci) {
	/*
	 * if this was a tunnel route, remove entry from tunnel pvc too
	 */
	reg_invoke_fr_tunnel_pvc(fr_pvc, NULL, FR_TUNNEL_REMOVE);
	/*
	 * remove the pvc whether the interface is a DCE, DTE or NNI 
	 */
	fr_idb->fr_dlci_tab[del_route->in_dlci] = NULL;
	if (fr_pvc->pvc_type != SVC_DYNAMIC)
	    fr_dec_pvc_count(fr_idb, fr_pvc->idb->hwptr);
	if (fr_idb->fr_intf_type != FR_DTE)
            lmi_idb->full_status_req = TRUE;
	if (fr_pvc->stats_save)
	    free((frmib_pvc_stats_type *)fr_pvc->stats_save);
	free(fr_pvc);
    }
}


/*
 * show_fr_route
 *
 * display routing entries present in the hash table.
 */

void show_fr_route (void)
{
    int i;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    fr_route_type fr_route;   /* copy on the stack before display */
    hwidbtype *idb;
    uchar status;
    list_element *element;

    printf("\nInput Intf \tInput Dlci \tOutput Intf \tOutput Dlci \tStatus");
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, idb) {
	if (!(fr_idb = idb->frame_relay_stuff))
	        continue;
	    for (i = 0; i < MAXDLCI; i++) {
		if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
		    continue;
		if (!fr_pvc->route_flag)
		    continue;
		if (fr_pvc->pvc_usage == PVC_LOCAL)
		    continue;
		fr_route = fr_pvc->fr_route;
		status = fr_pvc->pvc_status;
	
		printf("\n%16s%d \t\t%16s%d ",
		       fr_route.in_idb->hw_namestring,
		       fr_route.in_dlci,
		       fr_route.out_idb->hw_namestring,
		       fr_route.out_dlci);
		printf("\t\t");
		if (!idb->nokeepalive)
		    printf("%s",DLCI_ACTIVE(status) ? "active" : "inactive");
		else
		    printf("%s", "static");

	    }
    }
    /*
     * now print the tunnel routes if we have any
     */
    reg_invoke_fr_show_route();
}

/*
 * Nv_fr_route
 *
 * write out frame relay configuration
 */

void nv_fr_route (idbtype *idb, parseinfo *csb)
{
    int i;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;

    fr_idb =  idb->hwptr->frame_relay_stuff;
    if (!fr_idb)
        return;

    for (i = 0; i < MAXDLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
	    continue;
	if (!fr_pvc->route_flag)
	    continue;

	nv_write(TRUE, "%s %d interface %s %d",
		 csb->nv_command,
		 fr_pvc->fr_route.in_dlci,
		 fr_pvc->fr_route.out_idb->hw_namestring, 
		 fr_pvc->fr_route.out_dlci);
    }
}

/*
 * FRAME RELAY SWITCH LMI ROUTINES FOLLOW
 *
 * the PVC status for the route entries will be handled by a timer routine 
 * which will update the PVC status based on interface status.
 */
/*
 * show pvc statistics
 */

void show_pvc_stats (parseinfo *csb)
{
    hwidbtype *idb;
    idbtype   *swidb;
    ushort dlci = 0;
    list_element *element;

    automore_enable(NULL);
    swidb = GETOBJ(idb,1);
    if (swidb) {
	idb = swidb->hwptr;
	if (GETOBJ(int,1) != -1)
	    dlci = (ushort) GETOBJ(int,1);
	if (is_frame_relay(idb)) 
	    show_pvc_idb_stats(swidb, dlci);
	else if (idb->status & IDB_TUNNEL)
	    reg_invoke_fr_show_tunnel_pvc_stats(idb, dlci);
	else 
	    printf("\n Invalid interface specified");
    } else {
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, idb) {
	    if (is_ddr(idb) && reg_invoke_dialer_huntgroup_member(idb))
		continue;	/* go by the leader only, so skip the member */
	    if (GETOBJ(int,1) != -1)
		dlci = (ushort) GETOBJ(int,1);
	    show_pvc_idb_stats(idb->firstsw, dlci);
	}
	reg_invoke_fr_show_tunnel_pvc_stats(NULL, dlci);
    }
    automore_disable();
}

/*
 * show pvc per idb stats
 */

void show_pvc_idb_stats (idbtype *swidb, ushort dlci)
{
    hwidbtype *idb, *hwidb;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i;

    idb = swidb->hwptr;
    if (!(fr_idb = idb->frame_relay_stuff))
        return;

    if (!dlci) {
        printf ("\n\nPVC Statistics for interface %s", swidb->namestring);
	if (is_ddr(idb) && (hwidb = reg_invoke_dialer_get_conn_member(idb)))
	    printf(" (%s)", hwidb->hw_namestring);
	printf(" (Frame Relay %s)", disp_fr_intf[fr_idb->fr_intf_type]);
	for (i = 0; i < MAXDLCI ; i++) {
	    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
	        continue;
            if ( (swidb != idb->firstsw) && (swidb != fr_idb->fr_dlci_tab[i]->idb))
                continue;
	    mem_lock(fr_pvc);
	    display_pvc_stats(fr_pvc, i, 
		(idb->nokeepalive && (fr_pvc->pvc_usage == PVC_LOCAL)));
	    reg_invoke_fr_show_pvc(idb, dlci);
	    free(fr_pvc);
	    automore_conditional(0);
	}
    } else {          /* show stats for a particular dlci */
	if ((fr_pvc = fr_idb->fr_dlci_tab[dlci])) {
            printf ("\n\nPVC Statistics for interface %s (Frame Relay %s)",
                    swidb->namestring, disp_fr_intf[fr_idb->fr_intf_type]);
	    mem_lock(fr_pvc);
	    display_pvc_stats(fr_pvc, dlci, 
		(idb->nokeepalive && (fr_pvc->pvc_usage == PVC_LOCAL)));
	    reg_invoke_fr_show_pvc(idb, dlci);
	    free(fr_pvc);
	} 
    }
}

/*
 * display pvc stats
 *
 * actually display particular pvc stats
 */

void display_pvc_stats (
    fr_pvc_info_type *fr_pvc,
    ushort dlci,
    boolean static_pvc)
{
    fr_pvc_stats_type pvc_stats;
    uchar  pvc_status;
    hwidbtype *hwidb;

    pvc_stats = fr_pvc->pvc_stats;

    printf ("\n\nDLCI = %d, DLCI USAGE = %s, PVC STATUS = ", dlci, 
	    fr_pvc_usage_string(fr_pvc->pvc_usage));
    if (static_pvc) 
        printf ("STATIC");
    else {
	pvc_status = fr_pvc->pvc_status;
	if (IS_DLCI_DELETED(pvc_status))
	    printf ("DELETED");
	else {
	    hwidb = fr_pvc->idb->hwptr;
	    printf ("%s%s", DLCI_ACTIVE(pvc_status) ? "ACTIVE" : "INACTIVE",
		(is_ddr(hwidb) && !reg_invoke_dialer_get_conn_member(hwidb)) ?
		" (spoofing)" : "");
	}
    }
    printf(", INTERFACE = %s", (((fr_pvc->fr_route.in_idb) && 
		(fr_pvc->fr_route.in_idb->status & IDB_TUNNEL)) ? 
		fr_pvc->fr_route.in_idb->hw_namestring : 
		fr_pvc->idb->namestring));
    printf ("\n\n  input pkts %10lu    output pkts %10lu   in bytes %10lu",
	    fr_pvc_input_pkts(&pvc_stats), fr_pvc_output_pkts(&pvc_stats),
            fr_pvc_input_bytes(&pvc_stats));
    printf ("\n  out bytes %10lu     dropped pkts %10lu  in FECN pkts %10lu",
            fr_pvc_output_bytes(&pvc_stats), pvc_stats.num_dropped_pkts,
            fr_pvc_input_fecn_pkts(&pvc_stats));
    printf ("\n  in BECN pkts %10lu  out FECN pkts %10lu out BECN pkts %10lu",
	    fr_pvc_input_becn_pkts(&pvc_stats), pvc_stats.num_sent_fecn_pkts,
	    pvc_stats.num_sent_becn_pkts);
    printf ("\n  in DE pkts %10lu    out DE pkts %10lu",
	    fr_pvc_input_de_pkts(&pvc_stats), pvc_stats.num_sent_de_pkts);
    printf ("\n  out bcast pkts %10lu out bcast bytes %10lu",
	    pvc_stats.num_pkts_broad,pvc_stats.num_bytess_broad);
    printf ("\n  pvc create time %TE, last time pvc status changed %TE",
	    fr_pvc->pvc_create_time, fr_pvc->pvc_change_time);
    if (fr_pvc->pvc_usage != PVC_LOCAL)
        printf ("\n  Num Pkts Switched %10lu", pvc_stats.num_pkts_switched);
    if (fr_pvc->fr_pr_dlci_group)
	printf("\n  Priority DLCI Group %1d, DLCI %4d(HIGH), DLCI %4d(MEDIUM)"
	       "\n  DLCI %4d(NORMAL), DLCI %4d(LOW)",fr_pvc->fr_pr_dlci_group,
	       dlci,fr_pvc->fr_pr_dlcis[PRIORITY_MEDIUM],
	       fr_pvc->fr_pr_dlcis[PRIORITY_NORMAL],
	       fr_pvc->fr_pr_dlcis[PRIORITY_LOW]);
}

/*
 * fr_pvc_usage_string
 * Return the string indicating the PVC usage
*/

char *fr_pvc_usage_string (enum PVC_USAGE_TYPE usage)

{
    switch (usage) {
    case PVC_LOCAL:
	return("LOCAL");
    case PVC_SWITCHED_SERIAL:
    case PVC_SWITCHED_IP_TUNNEL:
	return("SWITCHED");
    case PVC_NOTSET:
	return("UNUSED");
    default:
	return("UNKNOWN");
    }
}

/*
 * FRAME RELAY ROUTINES FOR SLOW SWITCH FROM SERIAL TO SERIAL OR HSSI 
 * and OVER IP USING IP TUNNELING FOLLOW
 */

/*
 * fr_switch_process
 *
 * called from frame_relay_input. Decide whether it can be switched or not.
 */

boolean fr_switch_process (hwidbtype *idb, paktype *pak)
{
    fr_idb_struct_type *fr_idb, *out_fr_idb;
    fr_pvc_info_type *fr_pvc, *out_fr_pvc;
    ushort q922_dlci, num_dlci, out_dlci, paksize;

    fr_idb =  idb->frame_relay_stuff;
    if (!fr_idb)
        return(FALSE);
    /*
     * extract the dlci in q.922 format. and then proceed
     * there is no need to validate the dlci because it is already validated.
     * only thing is that we need to reproduce FECN, BECN and DE bits
     * for the time being because we don't do congestion control.
     */
    q922_dlci = GETSHORT(pak->datagramstart);
    num_dlci = DLCI_TO_NUM(q922_dlci);
    if (idb->nokeepalive) {
	if (q922_dlci == (lmi_idb->fr_lmi_dlci | FRAME_RELAY_EA_BITS)) {
	    retbuffer(pak);
	    return(TRUE);
	}
    }

    fr_pvc = fr_idb->fr_dlci_tab[num_dlci];
    if (!fr_pvc)
        return(FALSE);

    /*
     * pvc exists, check if the route exists. Then this pvc is switchable
     * otherwise return FALSE and process locally.
     */
    if (!idb->nokeepalive) {
	if (!IS_PVC_ACTIVE_BIT_SET(fr_pvc->pvc_status)) {
	    fr_pvc->pvc_stats.num_dropped_pkts++;
	    datagram_done(pak);
	    return(TRUE);
	}
    }
    if (!fr_pvc->route_flag)
        return(FALSE);
    /*
     * route does exist, so get the info and slam dunk new dlci and ship it
     */
    out_dlci = (NUM_TO_DLCI(fr_pvc->fr_route.out_dlci)) | FRAME_RELAY_EA_BITS;
    /*
     * for the time being, just reproduce these bits as they are seen
     */
    out_dlci |= (q922_dlci & FR_NON_DLCI_BITS);  /* get FECN, BECN etc */
    memcpy ((uchar *)pak->datagramstart, (uchar *)&out_dlci, sizeof(ushort));
    pak->if_output = fr_pvc->fr_route.out_idb->firstsw;

    /*
     * Update input statistics
     */
    paksize = pak->datagramsize;
    idb_increment_rx_proto_counts(pak, ACCT_PROTO_FRS);
    
    if (!(fr_pvc->fr_route.out_idb->status & IDB_TUNNEL)) {
	if (!(out_fr_idb = fr_pvc->fr_route.out_idb->frame_relay_stuff)) {
	    retbuffer(pak);
	    return(TRUE);
	}
	if (!(out_fr_pvc = 
	      out_fr_idb->fr_dlci_tab[fr_pvc->fr_route.out_dlci])){
	    retbuffer(pak);
	    return(TRUE);
	}
	if (!fr_pvc->fr_route.out_idb->nokeepalive) {
	    if (!IS_PVC_ACTIVE_BIT_SET(out_fr_pvc->pvc_status)) {
		out_fr_pvc->pvc_stats.num_dropped_pkts++;
		datagram_done(pak);
		return(TRUE);
	    }
	}
    } else { /* out idb is a tunnel */
	reg_invoke_fr_tunnel_pvc(fr_pvc, &out_fr_pvc, FR_TUNNEL_FIND);
	if (out_fr_pvc &&
	    !IS_PVC_ACTIVE_BIT_SET(out_fr_pvc->pvc_status)) {
	    out_fr_pvc->pvc_stats.num_dropped_pkts++;
	    datagram_done(pak);
	    return(TRUE);
	}	
    }

    /*
     * debug for outgoing pkts 
     */
    if (frame_relay_debug) {
	    buginf("\nSwitching pkt rcvd on DLCI %d(0x%x) to DLCI %d(0x%x)",
		   num_dlci, q922_dlci, DLCI_TO_NUM(out_dlci), out_dlci);
	    buginf("\nSwitching from interface %s to interface %s",
		   idb->hw_namestring, pak->if_output->namestring);
    }

    /*
     * Update output statistics
     */
    fr_pvc->pvc_stats.num_pkts_switched++;
    out_fr_pvc->pvc_stats.num_bytes_send += paksize;
    out_fr_pvc->pvc_stats.num_output_pkts++;
    if (ISFR_FECN(out_dlci))
	out_fr_pvc->pvc_stats.num_sent_fecn_pkts++;
    if (ISFR_BECN(out_dlci))
	out_fr_pvc->pvc_stats.num_sent_becn_pkts++;
    if (ISFR_DE(out_dlci))
	out_fr_pvc->pvc_stats.num_sent_de_pkts++;
    fr_pvc->fr_route.out_idb->counters.tx_frames[ACCT_PROTO_FRS]++;
    fr_pvc->fr_route.out_idb->counters.tx_bytes[ACCT_PROTO_FRS] += paksize;

    pak->linktype = LINK_FR_SWITCH;
    if (idb->board_encap)
	(*idb->board_encap)(pak,idb);
    datagram_out(pak);
    return(TRUE);
}

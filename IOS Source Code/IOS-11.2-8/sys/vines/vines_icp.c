/* $Id: vines_icp.c,v 3.2.62.1 1996/08/07 09:03:33 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_icp.c,v $
 *------------------------------------------------------------------
 * vines_icp.c - Support for the vines Internet Control Protocol (ICP).
 *
 * February 1993, David Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_icp.c,v $
 * Revision 3.2.62.1  1996/08/07  09:03:33  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:57:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../h/address.h"
#include "config.h"
#include "vines_private.h"
#include "vines_ipc.h"

char *const vicp_types[] = {"Error", "Metric"};

/**********************************************************************
 *
 *			    MISCELLANEOUS
 *
 **********************************************************************/

/*
 * vicp_getbuffer
 *
 * Allocate a buffer for sending by a routine in this file.  The length
 * parameter specifies the number of data bytes after the ICP heaer.
 */
static inline paktype *vicp_getbuffer (int length)
{
    paktype *pak;

    pak = vip_getbuffer(sizeof(vinesicptype) + length);
    if (pak) {
	/* no need to set any higher starting points */
	pak->datagramsize = sizeof(vinesicptype) + length;
    }
    return(pak);
}

/**********************************************************************
 *
 *			  ICP ERROR MESSAGES
 *
 **********************************************************************/

/*
 * vicp_send_error
 *
 * Handle the sending of a VINES error message.  This routine creates a
 * new message to send back to the source.  Up to V_IPC_COPIEDBYTES
 * (currently 40) bytes of the original message will be copied into the
 * error message.
 *
 * For a full listing of Vines networking error codes see the file
 * vines.errors.
 */
void vicp_send_error (paktype *pak, int code, boolean doit)
{
    paktype *errpak;
    vinesiptype *vip;
    vinesicptype *icp;
    vinesidbtype *idb;
    int length;

    /*
     * Responses are generated only when specifically requested by the
     * sender.  Never send an ICP message in response to another ICP
     * message.  Also check to make sure that the bad packet was not an
     * internally generated packet.
     */
    vip = (vinesiptype *)pak->network_start;
    if (!doit &&					/* not forced    */
	(!(vip->tc & VINES_ERROR) ||			/* not requested */
	 (vip->protocol == VINES_ICP) ||		/* ICP           */
	 (pak->if_input == NULL) || 			/* internal      */
	 (vip->len < VINES_MINSIZE))) {			/* runt          */

	return;
    }

    length = min(vip->len, V_ICP_COPIEDBYTES);
    errpak = vicp_getbuffer(length);
    if (errpak) {
	if (do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost)) {
	    buginf("\nVINES: sending IP error notification to %#z",
		   GET2WD(vip->ssrcnet), vip->srchost);
	}	
	icp = (vinesicptype *)errpak->transport_start;
	icp->type = VICP_EXCEPTION;
	icp->info = code;
	bcopy(pak->network_start, icp->oldpak, length);
	idb = vip_send_pak(errpak, GET2WD(vip->ssrcnet), vip->srchost,
			       VINES_ICP, 0, VINES_HOPS);
	if (idb)
	    idb->tx_icp_error++;
    }
}

/*
 * vicp_rcv_error
 *
 * Handle the receipt of a vines error message.  Hopefully it was
 * received because the user was running a 'ping' or 'trace' command.
 *
 * DRH.  This should also cause sockets to be declared dead.  This will
 * interact with the vines ping code that gets one socket and uses it for
 * all pings.
 */
static void vicp_rcv_error (paktype *pak)
{
    vinesiptype *vip, *orig_vip;
    vinesicptype *icp;
    vinesipctype *orig_ipc;

    vip = (vinesiptype *)pak->network_start;
    icp = (vinesicptype *)pak->transport_start;
    orig_vip = (vinesiptype *)icp->oldpak;
    if (orig_vip->protocol != VINES_IPC)
	return;
    orig_ipc = (vinesipctype *)orig_vip->data;
    if ((orig_ipc->srcport == VIPC_PORT_ECHO) ||
	(orig_ipc->dstport == VIPC_PORT_ECHO)) {
	vines_icp_echo_sink(pak);
	return;
    }
    if ((icp->info == VINES_ERR_HOST_UNR) ||
	(icp->info == VINES_ERR_NO_SOCKET)) {
	vipc_network_error(orig_vip, orig_ipc);
	return;
    }
}

/**********************************************************************
 *
 *			 ICP METRIC MESSAGES
 *
 **********************************************************************/

/*
 * vicp_send_metric
 *
 * Handle the sending of a VINES metric notification message.  Up to
 * V_IPC_COPIEDBYTES (currently 40) bytes of the original packet will be
 * copied into the metric message.
 *
 * Note: The metric must be converted to network form.  Since there is
 * no way to tell if the sender of the original packet is SRTP
 * capable, send an RTP style metric (aka no fractional component).
 */
void vicp_send_metric (paktype *pak, int metric)
{
    paktype *notify;
    vinesiptype *vip;
    vinesicptype *icp;
    vinesidbtype *idb;
    int length;

    vip = (vinesiptype *)pak->network_start;
    length = min(vip->len, V_ICP_COPIEDBYTES);
    notify = vicp_getbuffer(length);
    if (notify) {
	if (do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost)) {
	    buginf("\nVINES: sending metric notification for %#z to %#z",
		   GET2WD(vip->ddstnet), vip->dsthost, GET2WD(vip->ssrcnet),
		   vip->srchost);
	}	
	icp = (vinesicptype *)notify->transport_start;
	icp->type = VICP_METRIC;
	icp->info =  vmetric_to_rtp(metric);
	bcopy(pak->network_start, icp->oldpak, length);
	
	idb = vip_send_pak(notify, GET2WD(vip->ssrcnet), vip->srchost,
			       VINES_ICP, 0, VINES_HOPS);
	if (idb)
	    idb->tx_icp_metric++;
    }
}

/*
 * vicp_rcv_metric
 *
 * Handle the sending of a VINES metric notification message.  This
 * message was received because the router requested metric information
 * on a new IPC connection.  Pass off this information so that IPC can
 * correctly update its internal state.
 *
 * Note: The metric must be converted to network form.  Since there is
 * no way to tell if the sender of the original packet is SRTP
 * capable, we must accept a SRTP style metric.  This insures that we
 * don't end up converting a fractional only metric to a metric of zero.
 * An RTP metric will also convert with this routine, so there aren't
 * any possible problems.
 */
static void vicp_rcv_metric (vinesicptype *icp)
{
    vinesiptype *original;

    original = (vinesiptype *)icp->oldpak;
    vipc_set_metric(GET2WD(original->ddstnet), original->dsthost,
		    vmetric_from_srtp(icp->info));
}

/**********************************************************************
 *
 *		    INTERFACE FROM VINES IP LAYER
 *
 **********************************************************************/

/*
 * vines_icp_receive
 *
 * Process a received ICP message.
 */
void vines_icp_receive (paktype *pak, vinesidbtype *idb)
{
    vinesiptype *vip;
    vinesicptype *icp;

    vip = (vinesiptype *)pak->network_start;
    icp = (vinesicptype *)pak->transport_start;
    if (do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost)) {
	if (icp->type >= VICP_LAST) {
	    buginf("\nVINES: received unknown(%x) packet from %#z",
		   icp->type, GET2WD(vip->ssrcnet), vip->srchost);
	} else {
	    buginf("\nVINES: received %s packet (info %d) from %#z",
		   vicp_types[icp->type], icp->info, GET2WD(vip->ssrcnet),
		   vip->srchost);
	}
    }

    switch (icp->type) {
      case VICP_EXCEPTION:
	idb->rx_icp_error++;
	vicp_rcv_error(pak);
	break;

      case VICP_METRIC:
	idb->rx_icp_metric++;
	vicp_rcv_metric(icp);
	break;

      default:
	idb->rx_icp_illegal++;
	break;
    }
}

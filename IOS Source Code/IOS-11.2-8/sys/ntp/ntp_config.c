/* $Id: ntp_config.c,v 3.2.62.4 1996/08/19 18:54:50 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_config.c,v $
 *------------------------------------------------------------------
 * NTP configuration routines
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_config.c,v $
 * Revision 3.2.62.4  1996/08/19  18:54:50  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.3  1996/07/02  00:09:30  rbadri
 * CSCdi55849:  LANE: IOS still believe it is non-broadcast interface
 * Branch: California_branch
 * While configuring NTP, treat sub-interfaces with LEC configured on
 * them to be broadcast interfaces.
 *
 * Revision 3.2.62.2  1996/04/16  19:03:07  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:12:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:06:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:49:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:50:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:28:40  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:44:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ip/ip.h"
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "parser_defs_ntp.h"
#include "ttysrv.h"
#include "../os/async.h"

/*
 * Periodic process to update the hardware calendar
 */
void calendar_update_process(void)
{
    /*
     * If the NTP clock is synced, and we're enabled, update the hardware 
     * calendar.
     */
    if (ntp_update_calendar && ntp_clock_is_synchronized()) {
	do_calendar_update();
    }
}

/*
 * no ntp
 */
void no_ntp_command (parseinfo *csb)
{
    ntp_running = FALSE;	/* Straightforward enough... */
    ntp_clock_is_synced = FALSE;
    ntp_clock_status = '\0';
}

/*
 * Process all NTP global configuration commands
 */

void ntp_global_config_cmd(parseinfo *csb)
{
    struct sockaddr_in peer_addr;
    struct peer *peer;

    /* NVGENs first. */
    
    if (csb->nvgen) {

	switch (csb->which) {

	case NTP_TRUSTED_KEY_CMD:
	    /*
	     * [no] ntp trusted-key ##
	     */
	    ntp_nv_trust(csb);
	    break;

	case NTP_AUTH_KEY_CMD:
	    /*
	     * ntp authentication-key ## md5 value [cryptotype]
	     * no ntp authentication-key ##
	     */
	    ntp_nv_authkey(csb);
	    break;

	case NTP_AUTHENTICATE_CMD:
	    /*
	     * [no] ntp authenticate
	     */
	    nv_write(sys_authenticate, "%s", csb->nv_command);
	    break;

	case NTP_SOURCE_CMD:
	    /*
	     * ntp source interface
	     * no ntp source
	     */
	    if (ntp_running && (any_interface->idb != NULL)) {
		nv_write(TRUE, "%s %s", csb->nv_command, 
			 any_interface->idb->namestring);
	    }
	    break;

	case NTP_MASTER_CMD:
	    /*
	     * [no] ntp master [##]
	     */
	    if (ntp_running) {
		memset(&peer_addr, 0, sizeof(peer_addr));
		peer_addr.sin_port = NTP_PORT;
		peer_addr.sin_addr.s_addr = REFCLOCK_ADDR |
		    (REFCLK_MASTER << 8) | 1;
		peer = findpeer(&peer_addr, any_interface, FALSE);
		if (peer) {
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_add(peer->stratum != STRATUM_MASTER_DEFAULT - 1, " %d", 
			   peer->stratum + 1);
		}
	    }
	    break;

	case NTP_MAXASS_CMD:
	    /*
	     * [no] ntp max_associations [##]
	     */
	    nv_write((ntp_running &&
		      ntp_max_peercount != NTP_DEF_MAX_PEERCOUNT),
		     "%s %d", csb->nv_command, ntp_max_peercount);
	    break;

	case NTP_CLOCK_PERIOD_CMD:
	    /*
	     * ntp clock-period nnn
	     * no ntp clock-period
	     */
	    {
		ulong dummy, incr, basic_incr, divisor;
		
		/* Get the current clock rate. */
		
		get_clock_params(&dummy, &dummy, &dummy, &incr, 
				     &basic_incr, &divisor);
		nv_write(ntp_running && incr != basic_incr, "%s %d", 
			 csb->nv_command, incr);
	    }
	    break;

	case NTP_PEER_CMD:
	    /*
	     * ntp {peer|server} ipaddr [version #] [key #] [source interface]
	     *                                                       [prefer]
	     * no ntp {peer|server} ipaddr
	     */
	    if (ntp_running)
		ntp_peer_nvgen(csb);
	    break;

	case NTP_BROAD_DELAY_CMD:
	    /*
	     * ntp broadcastdelay n
	     * no ntp broadcastdelay
	     */
	    nv_write(ntp_running && (sys_bdelay != DEFBROADDELAY), "%s %d", 
		     csb->nv_command, sys_bdelay / PIPS_PER_USEC);
	    break;

	case NTP_UPDATE_CALENDAR_CMD:
	    /*
	     * [no] ntp update-calendar
	     */
	    nv_write(ntp_update_calendar && ntp_running, csb->nv_command);
	    break;
	}
	return;
    }

    /* Fire up NTP. */

    init_ntp_process(NULL);

    /* Process the individual commands. */

    switch (csb->which) {
	
    case NTP_TRUSTED_KEY_CMD:
	/*
	 * [no] ntp trusted-key ##
	 *
	 * OBJ(int,1) = key number
	 */
	authtrust(GETOBJ(int,1), csb->sense);
	break;
	
    case NTP_AUTH_KEY_CMD:
	/*
	 * ntp authentication-key ## md5 value [cryptotype]
	 * no ntp authentication-key ##
	 *
	 * OBJ(int,1) = key number
	 * OBJ(int,2) = value encryption type
	 * OBJ(int,3) = TRUE if cryptotype present
	 * OBJ(string,1) = key text
	 */
	if (csb->sense) {	/* positive form */
	    MD5auth_setkey(GETOBJ(int,1), GETOBJ(string,1), GETOBJ(int,3),
			   GETOBJ(int,2));
	} else {
	    if (!ntp_delete_key(GETOBJ(int,1)))
		printf("\n%%NTP: No such key number");
	}
	break;
	
    case NTP_AUTHENTICATE_CMD:
	/*
	 * [no] ntp authenticate
	 */
	ntp_proto_config(PROTO_AUTHENTICATE, csb->sense);
	break;
	
    case NTP_SOURCE_CMD:
	/*
	 * ntp source interface
	 * no ntp source
	 *
	 * OBJ(idb,1) = source interface
	 */
	if (csb->sense) {
	    any_interface->idb = GETOBJ(idb,1);
	} else {
	    any_interface->idb = NULL;
	}
	ntp_global_source_if = csb->sense;
	ntp_flush_srcaddrs();	/* Grab back any wanderers */
	break;
	
    case NTP_MASTER_CMD:
	/*
	 * [no] ntp master [##]
	 */
	memset(&peer_addr, 0, sizeof(peer_addr));
	peer_addr.sin_port = NTP_PORT;
	peer_addr.sin_addr.s_addr = REFCLOCK_ADDR |
	    (REFCLK_MASTER << 8) | 1;
	if (csb->sense) {
	    
	    /* Create the dummy reference clock to do the job. */
	    
	    peer = peer_config(&peer_addr, NULL, MODE_ACTIVE,
			       NTP_VERSION, NTP_MINDPOLL,
			       NTP_MAXDPOLL, 0, 0, 0, GETOBJ(int,1) - 1);
	    if (!peer || !peer->procptr) {
		printf("\n%%NTP: Couldn't configure master clock");
		return;
	    }
	    
	} else {
	    
	    /* Turning it off.  Find the peer and delete it. */
	    
	    if (!peer_unconfig(&peer_addr, NULL, FALSE))
		printf("\n%%NTP: No master clock");
	}
	break;
	
    case NTP_MAXASS_CMD:
	/*
	 * [no] ntp max_associations [##]
	 *
	 * OBJ(int,1) = association count
	 */
	if (csb->sense)
	    ntp_max_peercount = GETOBJ(int,1);
	else
	    ntp_max_peercount = NTP_DEF_MAX_PEERCOUNT;
	break;

    case NTP_CLOCK_PERIOD_CMD:
	/*
	 * ntp clock-period nnn
	 * no ntp clock-period
	 *
	 * OBJ(int,1) = clock period
	 */
	{
	    ulong dummy, incr, basic_incr, diff;
	    
	    /* Get the current clock rate. */
	    
	    get_clock_params(&dummy, &dummy, &dummy, &incr, 
				 &basic_incr, &dummy);

	    /* If "no", reset the clock rate to the fundamental rate. */
	    
	    if (!csb->sense) {
		set_clock_increment(basic_incr);
		ntp_set_drift(basic_incr, basic_incr);
	    } else {
		
		/* Set to requested value if within 1% of the basic value. */
		
		if (GETOBJ(int,1) > basic_incr)
		    diff = GETOBJ(int,1) - basic_incr;
		else
		    diff = basic_incr - GETOBJ(int,1);
		
		if (diff > basic_incr	/* Avoid overflow problems */
		    || (diff * 100) > basic_incr) {
		    printf("\n%%NTP: unreasonable value");
		} else {
		    set_clock_increment(GETOBJ(int,1));
		    ntp_set_drift(basic_incr, GETOBJ(int,1));
		}
	    }
	}
	break;
	
    case NTP_PEER_CMD:
	/*
	 * ntp {peer|server} ipaddr [version #] [key #] [source interface]
	 *                                                       [prefer]
	 * no ntp {peer|server} ipaddr
	 *
	 * OBJ(int,1) = mode
	 * OBJ(int,2) = version
	 * OBJ(int,3) = TRUE if "key" present
	 * OBJ(int,4) = key number
	 * OBJ(int,5) = TRUE if "prefer" present
	 * OBJ(idb,1) = source interface or NULL
	 * OBJ(paddr,1) = peer address
	 */
	{
	    struct interface *intf = NULL;
	    ulong flags = 0;

	    memset(&peer_addr, 0, sizeof(peer_addr));
	    peer_addr.sin_port = NTP_PORT;
	    peer_addr.sin_addr.s_addr = (*GETOBJ(paddr,1)).ip_addr;
	    if (!csb->sense) {
		/*
		 * Handle the "no" case.
		 */
		if (!peer_unconfig(&peer_addr, NULL, FALSE))
		    printf("\n%%NTP: unrecognized peer");
	    } else {
		/*
		 * Configure a peer.
		 */
		if (GETOBJ(idb,1) != NULL) {
		    intf = &inter_list[GETOBJ(idb,1)->if_number];
		    flags |= FLAG_SRCADR;
		}
		
		if (GETOBJ(int,5)) /* "Prefer" specified */
		    flags |= FLAG_PREFER;

		if (GETOBJ(int,3)) /* "Key" specified */
		    flags |= FLAG_AUTHENABLE;

		if (ISREFCLOCKADR(&peer_addr)) {
		    printf("\n%%NTP: Illegal peer address");
		} else {
		    peer = peer_config(&peer_addr, intf, GETOBJ(int,1), 
				       GETOBJ(int,2), NTP_MINDPOLL,
				       NTP_MAXDPOLL, flags, 0, GETOBJ(int,4),
				       STRATUM_UNSPEC);
		    if (!peer) {
			printf("\n%%NTP: Couldn't configure peer");
		    }
		}
	    }
	}
	break;
	
    case NTP_BROAD_DELAY_CMD:
	/*
	 * ntp broadcastdelay n
	 * no ntp broadcastdelay
	 *
	 * OBJ(int,1) = delay value
	 */
	{
	    int value;

	    /* Set the delay value to the default if "no" is specified. */
	    
	    if (csb->sense) {
		value = (GETOBJ(int,1) * PIPS_PER_USEC) + (PIPS_PER_USEC / 2);
	    } else {
		value = DEFBROADDELAY;
	    }
	    ntp_proto_config(PROTO_BROADDELAY, value);
	}
	break;
	    
    case NTP_UPDATE_CALENDAR_CMD:
	/*
	 * [no] ntp update-calendar
	 */
	ntp_update_calendar = csb->sense;
	break;
    }
}


/*
 * NTP Interface commands
 */

void ntp_if_config_cmd(parseinfo *csb)
{
    struct ntp_idb_info *info;

    /* First NVGENs, of course. */

    if (csb->nvgen) {
	if (!ntp_running)
	    return;
	info = csb->interface->ntp_info;
	if (!info)
	    return;

	switch (csb->which) {

	case NTP_BROAD_IF_CMD:
	    /*
	     * ntp broadcast [version #] [key #] [destination ipaddr]
	     * no ntp broadcast
	     */
	    {
		struct sockaddr_in peer_addr;
		struct interface *intf = NULL; /* Shut the compiler up */
		struct peer *peer;

		memset(&peer_addr, 0, sizeof(peer_addr));
		peer_addr.sin_port = NTP_PORT;
		peer_addr.sin_addr.s_addr = -1L;

		intf = &inter_list[csb->interface->if_number];
		peer = findpeer(&peer_addr, intf, TRUE);
		if (peer) {
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_add(peer->version != NTP_VERSION, " version %d", 
			   peer->version);
		    nv_add(peer->flags & FLAG_AUTHENABLE, " key %d",
			   peer->keyid);
		    nv_add(peer->flags & FLAG_BCASTDSTADR, " destination %s",
			   ntoa(&peer->srcadr));
		}
	    }
	    break;

	case NTP_DISABLE_IF_CMD:
	    /*
	     * [no] ntp disable
	     */
	    nv_write (!(info->ntp_enabled & NTP_ENAB_RCV), "%s", 
		      csb->nv_command);
	    break;

	case NTP_BROAD_CLIENT_IF_CMD:
	    /*
	     * [no] ntp broadcast client
	     */
	    nv_write((info->ntp_enabled & NTP_ENAB_BCASTCLIENT), "%s", 
		     csb->nv_command);
	    break;

	}
	return;
    }

    /* Process the commands. */

    init_ntp_process(csb->interface);
    info = csb->interface->ntp_info;
    if (!info)
	return;

    switch (csb->which) {

    case NTP_BROAD_IF_CMD:
	/*
	 * ntp broadcast [version #] [key #] [destination ipaddr]
	 * no ntp broadcast
	 *
	 * OBJ(int,1) = version
	 * OBJ(int,2) = TRUE if "key" is specified
	 * OBJ(int,3) = key ID
	 * OBJ(int,4) = TRUE if "destination is specified
	 * OBJ(paddr,1) = destination address
	 */
	{
	    struct sockaddr_in peer_addr;
	    struct interface *intf = NULL; /* Shut the compiler up */
	    struct peer *peer;
	    ulong flags;

	    /*
	     * Pick up the destination address from either the command
	     * (if specified), or the interface.
	     */
	    memset(&peer_addr, 0, sizeof(peer_addr));
	    peer_addr.sin_port = NTP_PORT;
	    peer_addr.bcast = TRUE;
	    flags = FLAG_SRCADR;
	    if (csb->sense && GETOBJ(int,4)) {
		peer_addr.sin_addr.s_addr = (*GETOBJ(paddr,1)).ip_addr;
		flags |= FLAG_BCASTDSTADR;
	    } else {
		peer_addr.sin_addr.s_addr = csb->interface->ip_broadcast;
		peer_addr.if_bcast = TRUE;
	    }

	    intf = &inter_list[csb->interface->if_number];
	    
	    /* Process the command. */
	    
	    if (!csb->sense) {
		/*
		 * Handle the "no" case.
		 */
		if (!peer_unconfig(&peer_addr, intf, TRUE))
		    printf("\n%%NTP: Broadcast not enabled");
		else
		    info->ntp_enabled &= ~NTP_ENAB_BCAST;
	    } else {
		/*
		 * Configure a broadcast peer.
		 */
		info->ntp_enabled |= NTP_ENAB_BCAST;
		if (GETOBJ(int,2)) {	/* "key" specified */
		    flags |= FLAG_AUTHENABLE;
		}
		peer = peer_config(&peer_addr, intf, MODE_BROADCAST,
				   GETOBJ(int,1), NTP_MINDPOLL, NTP_MAXDPOLL,
				   flags, 0, GETOBJ(int,3), STRATUM_UNSPEC);
		if (!peer) {
		    printf("\n%%NTP: Couldn't configure broadcast peer");
		}
	    }
	}
	break;

    case NTP_DISABLE_IF_CMD:
    case NTP_ENABLE_IF_CMD:
	/*
	 * [no] ntp disable
	 */
	{
	    boolean rcv_enabled;

	    /* Combine csb->which and csb->sense to figure it out. */
	    
	    rcv_enabled = !csb->sense;	/* [no] ntp disable */
	    if (csb->which == NTP_ENABLE_IF_CMD)
		rcv_enabled = !rcv_enabled;
	    if (rcv_enabled) {
		info->ntp_enabled |= NTP_ENAB_RCV;
	    } else {
		info->ntp_enabled &= ~NTP_ENAB_RCV;
	    }
	}
	break;

    case NTP_BROAD_CLIENT_IF_CMD:
	/*
	 * [no] ntp broadcast client
	 */

	/* If positive, turn on system flag, as well as interface flag. */

	if (csb->sense) {
	    ntp_proto_config(PROTO_BROADCLIENT, TRUE);
	    info->ntp_enabled |= NTP_ENAB_BCASTCLIENT;
	} else {
	    
	    /* Negative.  Turn off the interface flag. */
	    
	    if (!(info->ntp_enabled & NTP_ENAB_BCASTCLIENT))
		printf("\n%%NTP: Broadcast client not enabled");
	    else
		info->ntp_enabled &= ~NTP_ENAB_BCASTCLIENT;
	}
	break;
    }
}

/*
 * ntp leap {add|delete}
 *
 * OBJ(int,1) = leap warning bit value
 */
void ntp_leap_command(parseinfo *csb)
{
    if (!leap_setleap(~0, GETOBJ(int,1)))
	printf("\n%%NTP: Can't set leap warning right now");
}

/*
 * ntp_reset_clock - reset clock when time base is perturbed
 */
void ntp_reset_clock(void)
{
    if (ntp_task_initialized) {
	init_systime();
	clear_all();
    }
}

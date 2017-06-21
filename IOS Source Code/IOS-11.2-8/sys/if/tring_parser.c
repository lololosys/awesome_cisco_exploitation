/* $Id: tring_parser.c,v 3.5.18.3 1996/06/01 06:15:38 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/tring_parser.c,v $
 *------------------------------------------------------------------
 * tring_parser.c -- routines for dealing with Token Ring  parser commands
 *
 * Sept. 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tring_parser.c,v $
 * Revision 3.5.18.3  1996/06/01  06:15:38  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/05/17  11:24:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.2  1996/05/02  22:04:48  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.1  1996/04/03  14:39:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.18.1  1996/03/18  20:15:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/02/20  00:57:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:12:47  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:04:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.12.1  1996/01/24  22:19:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/30  00:28:00  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.2  1995/11/16  23:39:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:53:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  23:06:06  gstovall
 * Placeholders for Talisker_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "media_registry.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_tring.h"
#include "tring.h"
#include "tring_private.h"
#include "tring_common.h"


#define trebuginf if (tokenevent_debug || tokenring_debug) buginf 


void tr_etheroui_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    int nvgen = csb->nvgen;
    
    if (nvgen) {
	switch (GETOBJ(int,1)) {
	  case PARSER_ETHER_90COMPAT:
            nv_write(idb->tr_transit_oui == ETHEROUI, csb->nv_command);
	    break;
	  case PARSER_ETHER_STANDARD:
	    /* Default case. Write nothing */
	    break;
	  case PARSER_ETHER_CISCO:
            nv_write(idb->tr_transit_oui == CISCOOUI, csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	return;
    }
    if (!csb->sense) {
	idb->tr_transit_oui = DEFAULT_OUI;
	return;
    }
    switch (GETOBJ(int,1)) {
      case PARSER_ETHER_90COMPAT:
        idb->tr_transit_oui = ETHEROUI;
	break;
      case PARSER_ETHER_STANDARD:
        idb->tr_transit_oui = OLD_ETHEROUI;
	break;
      case PARSER_ETHER_CISCO:
        idb->tr_transit_oui = CISCOOUI;
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}


void tr_etr_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    boolean oldstate;
    int nvgen = csb->nvgen;

    if (nvgen) {
	nv_write(idb->tr_ring_mode & RINGMODE_ETR, csb->nv_command);
	return;
    }

    oldstate = (idb->tr_ring_mode & RINGMODE_ETR) ? TRUE : FALSE;
    if (csb->sense)
	idb->tr_ring_mode |= RINGMODE_ETR;
    else 
	idb->tr_ring_mode &= ~RINGMODE_ETR;

    if ((idb->tr_ring_speed == 16) && (oldstate != csb->sense))
	(*idb->reset)(idb);		/* make it active */
}


/*
 * Allow the token ring speed to be configurable. The form is:
 *	int token N
 *	ring-speed 4|16
 */
void tr_set_ring_speed (parseinfo *csb)
{
    hwidbtype *idb;
    ushort oldspeed;
    
    idb = csb->interface->hwptr;

    if (csb->nvgen) {
	if (idb->tr_ring_speed && idb->tr_ring_speed == GETOBJ(int,1)) {
	    nv_write(TRUE, csb->nv_command);
	}
	return;
    }

    if (!csb->sense) {
	if (idb->state != IDBS_ADMINDOWN) {
	    printf("\nInterface must be shutdown to remove the ring speed.");
	    return;
	}
	idb->tr_ring_speed = 0;
	return;
    }

    oldspeed = idb->tr_ring_speed;
    if (GETOBJ(int,1) != oldspeed) {
	new_tring_speed(idb, GETOBJ(int,1), FALSE);
	trebuginf("\n%s: Interface ring speed is %d",
		  idb->hw_short_namestring, idb->tr_ring_speed);
    }
}

/* $Id: at_igrp2_show.c,v 3.7.18.3 1996/07/10 22:18:17 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_igrp2_show.c,v $
 *------------------------------------------------------------------
 * Show support for AppleTalk EIGRP
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_igrp2_show.c,v $
 * Revision 3.7.18.3  1996/07/10  22:18:17  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/06/29  00:06:56  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.7.18.1  1996/04/02  05:35:44  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.7  1996/02/08  23:08:02  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.6  1996/02/07  16:09:49  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  05:59:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:07  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/17  08:42:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:35  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "plural.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_igrp2.h"
#include "parser_defs_atalk.h"

void
atigrp2_show_cmd (parseinfo *csb)
{

    if (!atalk_running) {
#ifdef ERRMSG_FOR_ERRORS
	errmsg(AT_ERR_NOTRUNNING);
#else
	printf(atalk_not_running);
#endif
	return;
    }
    automore_enable(NULL);
    switch (csb->which) {
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
      case SHOW_APPLE_EIGRP_TOP:	/* show apple eigrp topology */
      case SHOW_APPLE_EIGRP_NBR:	/* show apple eigrp neighbor */
      case SHOW_APPLE_EIGRP_EVENT:	/* show apple eigrp event */
      case SHOW_APPLE_EIGRP_SIAEVENT:	/* show apple eigrp sia-event */
      case SHOW_APPLE_EIGRP_TRAFFIC:	/* show apple eigrp traffic */
	if (!atalk_igrp_running) {
	    printf("\n%%AppleTalk/EIGRP is not running");
	    break;
	}
	switch (csb->which) {
	  case SHOW_APPLE_EIGRP_TOP:
	    atigrp2_show_topology(csb);
	    break;
	  case SHOW_APPLE_EIGRP_NBR:
	    atigrp2_show_neighbors(csb);
	    break;
	  case SHOW_APPLE_EIGRP_EVENT:
	  case SHOW_APPLE_EIGRP_SIAEVENT:
	    atigrp2_show_event(csb);
	    break;
	  case SHOW_APPLE_EIGRP_TRAFFIC:
	    atigrp2_show_traffic(csb);
	    break;

	  default:			/* shouldn't get here (nyuk) */
	    break;
	}
	break;
    }
}

void
atigrp2_show_nbr_add (neighborentry *r)
{
    if (!r)
	return;

    if (r->reachable_by & ATALK_IGRP_ENABLED) {
	printf("\n        EIGRP router ID is: %u", r->igrp_rtr_id);
	printf("\n        EIGRP reachability statistics:");
	printf("\n          EIGRP connectivity first obtained %TE ago",
	       r->igrp_firstheard);
	printf("\n          EIGRP connectivity last heard %lu sec%s ago",
	       ArgAndPlural(ELAPSED_TIME(r->igrp_lastheard)/ONESEC,"","s"));
	printf("\n          This EIGRP neighbor is currently %s",
	       atmaint_NeighborState(r->igrp_state));
	printf("\n          We have received %d hello packet%s",
	       ArgAndPlural(r->igrp_hellos_heard,"","s"));
	printf("\n          The last hello was heard %u sec%s ago.",
	       ArgAndPlural(ELAPSED_TIME(r->igrp_last_hello)/ONESEC,"","s"));
	
	printf("\n          We have sent %u query packet%s for %u net%s",
	       ArgAndPlural(r->igrp_queries_sent, "", "s"),
	       ArgAndPlural(r->igrp_qsndcount, "", "s"));
	printf("\n          We have received %u query packet%s for %u net%s",
	       ArgAndPlural(r->igrp_queries_sent, "", "s"),
	       ArgAndPlural(r->igrp_qrcvcount, "", "s"));
	
	printf("\n          We have sent %u update packet%s for %u net%s",
	       ArgAndPlural(r->igrp_updates_sent, "", "s"),
	       ArgAndPlural(r->igrp_usndcount, "", "s"));
	printf("\n          We have received %u update packet%s for %u net%s",
	       ArgAndPlural(r->igrp_updates_heard, "", "s"),
	       ArgAndPlural(r->igrp_urcvcount, "", "s"));
	printf("\n          We have sent %u reply packet%s for %u net%s",
	       ArgAndPlural(r->igrp_replies_sent, "", "s"),
	       ArgAndPlural(r->igrp_qrcvcount, "", "s"));
	printf("\n          We have received %u reply packet%s for %u net%s",
	       ArgAndPlural(r->igrp_replies_heard, "", "s"),
	       ArgAndPlural(r->igrp_qsndcount, "", "s"));
    }
}

void
atigrp2_show_tfc_add (void)
{
    if (!atalk_igrp_running)
	return;

    printf("\n  EIGRP: %d received, %d hellos, %d updates, %d replies, %d queries",
	   atalk_stat[ATALK_IGRP_IN],
	   atalk_stat[ATALK_IGRP_HELLO_IN],
	   atalk_stat[ATALK_IGRP_UPDATE_IN],
	   atalk_stat[ATALK_IGRP_REPLY_IN],
	   atalk_stat[ATALK_IGRP_QUERY_IN]);
    printf("\n         %d sent,     %d hellos, %d updates, %d replies, %d queries",
	   atalk_stat[ATALK_IGRP_OUT], atalk_stat[ATALK_IGRP_HELLO_OUT],
	   atalk_stat[ATALK_IGRP_UPDATE_OUT], atalk_stat[ATALK_IGRP_REPLY_OUT],
	   atalk_stat[ATALK_IGRP_QUERY_OUT]);
    printf("\n         %d invalid, %d ignored", 
	   atalk_stat[ATALK_IGRP_BAD],
	   atalk_stat[ATALK_IGRP_IGNORED]);
}

void
atigrp2_show_globals_add (void)
{
    if (atalk_igrp_running) {
	printf("\n  EIGRP router id is: %d", atalk_rtr_id);
 	printf("\n  EIGRP maximum active time is %d minute%s",
	       ArgAndPlural(atigrp2_get_active_time(),"","s"));
	if (atalk_rtmp_bw != RTMP_BANDWIDTH)
	    printf("\n  RTMP redistribution bandwidth is %d.", atalk_rtmp_bw);
    } else {
	printf("\n  AppleTalk EIGRP is not enabled.");
    }
}

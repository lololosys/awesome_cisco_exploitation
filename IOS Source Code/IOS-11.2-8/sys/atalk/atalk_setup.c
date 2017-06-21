/* $Id: atalk_setup.c,v 3.3.54.5 1996/06/29 00:07:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atalk_setup.c,v $
 *------------------------------------------------------------------
 * Appletalk setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: atalk_setup.c,v $
 * Revision 3.3.54.5  1996/06/29  00:07:17  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.3.54.4  1996/06/28  23:04:54  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.54.3  1996/05/21  19:31:32  dwong
 * CSCdi58212:  router crashes during setup with appletalk
 * Branch: California_branch
 * Create a sub-idb from the setup if one hasn't allocated
 *
 * Revision 3.3.54.2  1996/03/23  01:28:10  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.54.1  1996/03/18  18:53:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.2  1996/03/07  08:29:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  13:34:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/28  03:58:21  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  08:44:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:10:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <interface.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ui/setup.h"
#include "atalk_private.h"

typedef struct atalk_setup_struct_ {
    boolean run_atalk;
    boolean run_atalk2;
} atalk_setup_struct;

static char *setup_optstr(char *, char *, char *);
static const char atalk_proto_name[] = "AppleTalk";

void atalk_setup_global (setup_struct *sp)
{
    atalk_setup_struct *as;

    if (system_loading || !router_enable) {
	sp->args[LINK_APPLETALK] = FALSE;
	return;
    }

    if ((as = malloc(sizeof(atalk_setup_struct))) == NULL) {
	return;
    }

    sp->args[LINK_APPLETALK] = as;
    as->run_atalk = FALSE;
    as->run_atalk2 = FALSE;

    if (atalk_enable) {
	as->run_atalk = system_protocol(sp, atalk_proto_name,
					RUNNING(LINK_APPLETALK));
	if (as->run_atalk) {
	    as->run_atalk2 = yes_or_no("    Multizone networks",
				       atalk_extendedInternet, TRUE);
	}
    }
}

void atalk_setup_int (setup_struct *sp)
{
    ushort astart, aend, anet, anode;
    zipifentry *zip;
    boolean aextended;
    char *zonebuffer = NULL;
    char buffer[MAXBUF], *cp;
    atalk_setup_struct *as = sp->args[LINK_APPLETALK];
    boolean first_time = TRUE;
    
    if (system_loading || !router_enable || !sp->args[LINK_APPLETALK]) {
	return;
    }

    if (as->run_atalk) {
	atalkidbtype *idb;

	idb = atalk_getidb(sp->idb);
	/*
	 * Create a sub-idb if one hasn't allocated.
	 */
	if (idb == NULL) {
	    atalk_init_idb(sp->idb);
	    if ((idb=atalk_getidb(sp->idb)) == NULL) {
	        setup_printf(sp, "\n%% Unable to create data structures for atalk interface "
		       "information.");
		return;
	    }
	    atalk_rxtypes_update(sp->idb);
	}

	if (interface_protocol(atalk_proto_name, idb->atalk_enabled)) {
	    if (atalkif_ErrForPeers(idb) != NULL) {
		anet = idb->atalknet ? idb->atalknet : 0;
	    } else {
		anet = idb->atalknet ? idb->atalknet : sp->defnetwork;
	    }
	    if (anet >= ATALK_STARTUP_START) {
		anet = 0;
	    }
	    astart = idb->atalk_cablestart ? idb->atalk_cablestart : anet;
	    aend = idb->atalk_cablestart ? idb->atalk_cableend : astart;
	    if (atalkif_ErrForNonExtended(idb) == NULL) {
		if (atalkif_ErrForExtended(idb) == NULL) {
		    aextended = yes_or_no("    Extended AppleTalk network",
					  (aend != 0), TRUE);
		} else {
		    aextended = FALSE;
		    aend = 0;
		}
	    } else {
		aextended = TRUE;
	    }
	    if (aextended) {
		cp = "    AppleTalk starting cable range";
	    } else {
		cp = "    AppleTalk network number";
	    }
	    astart = get_num_in_range(cp, 10, 0, ATALK_STARTUP_START - 1,
				      astart);
	    if (aextended && (astart != 0)) {
		aend = get_num_in_range("    AppleTalk ending cable range",
					10, astart,
					ATALK_STARTUP_START - 1,
					astart ?
					(aend >= astart ? aend : astart)
					: 0);
	    }
	    anode = idb->atalknode ? idb->atalknode :
		(!atalkif_ErrForPeers(idb) ? anet : 0);
	    if (aextended) {
		static const char cmd[] = "appletalk cable-range";
		if ((anet >= astart) && (anet <= aend)) {
		    setup_printf(sp, "\n%s %d-%d %d.%d",
				 cmd, astart, aend, anet, anode);
		} else {
		    setup_printf(sp, "\n%s %d-%d", cmd, astart, aend);
		}
	    } else {
		setup_printf(sp, "\nappletalk address %d.%d", astart, anode);
	    }
	    if (astart == 0) {
		setup_printf(sp, "\nappletalk discovery");
	    } else {
		if (idb->atalk_discovery) {
		    setup_printf(sp, "\nno appletalk discovery");
		}
		cp = "myzone";
		zip = atalkif_firstZone(idb);
		do {
		    if (zip) {
			zonebuffer = malloc(ZONELEN);
			if (zonebuffer) {
			    bcopy(zip->zone->name, zonebuffer,
				  zip->zone->length);
			    cp = zonebuffer;
			}
		    }

		    if (first_time) {
			cp = setup_optstr(buffer,"    AppleTalk zone name", cp);
			first_time = FALSE;
		    } else
			cp = setup_optstr(buffer, 
				"    AppleTalk additional zone name", cp);

		    if (null(cp)) {
			break;
		    }
		    setup_printf(sp, "\nappletalk zone %s", cp);
		    if (zip) {
			free(zonebuffer);
		    }
		    if (as->run_atalk2 && zip) {
			zip = atalkif_nextZone(zip);
		    }
		    cp = "";
		} while (as->run_atalk2 && aextended);
	    }
	} else {
	    if (idb->atalknet) {
		setup_printf(sp, "\nno appletalk address");
	    }
	}
    }
}


/*
 * setup_optstr
 * Handle an optional string which is allowed to return a null default
 * which is then used to quit prompting when multiple strings may be needed.
 */
static char *setup_optstr (char *buffer, char *prompt, char *defstring)
{
    char *cp;

    while (!(stdio->statbits & CONTROLC)) {
	if (null(defstring)) {
	    printf("\n%s: ", prompt);
	} else {
	    printf("\n%s [%s]: ", prompt, defstring);
	}
	if (!rdtty(buffer, MAXBUF)) {
	    break;
	}
	cp = deblank(buffer);
	if (null(cp)) {
	    return(defstring);
	} else {
	    if (*cp != '?') {
		return(cp);
	    }
	    printf("Type a string, then press RETURN");
	}
    }
    return(NULL);
}

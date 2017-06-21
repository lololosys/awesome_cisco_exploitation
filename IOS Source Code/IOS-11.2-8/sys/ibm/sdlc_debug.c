/* $Id: sdlc_debug.c,v 3.3.26.1 1996/07/25 09:45:23 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sdlc_debug.c,v $
 *------------------------------------------------------------------
 * sdlc_debug.c -- Debugging routines for SDLC and STUN
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc_debug.c,v $
 * Revision 3.3.26.1  1996/07/25  09:45:23  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/17  22:44:33  vrawat
 * CSCdi47060:  problem with stun-tg debugging
 *
 * Revision 3.2  1995/11/17  09:23:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../ibm/ibm.h"
#include "../ibm/ibm_externs.h"
#include "../ibm/ibm_public.h"
#include "../ibm/sdlc_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "sdlc_debug_flags.h"

/* Other initialized storage */
int slack_debug = 0;
int sdlc_packet_debug = 0;
boolean stun_packet_debug = FALSE;
boolean stun_tg_debug = FALSE;
uchar stun_debug_group = 0;
uchar stun_debug_addr[STUN_MAX_ADDR_LENGTH];
boolean stun_debug_addr_significant = FALSE;

/* Forward declaration */
static void stun_packet_debug_work(boolean sense, ulong u,
			    boolean addr_set, uchar *addr);

/*
 * sdlc_debug_command
 * Handles debug commands related to SDLC and STUN
 */

void sdlc_debug_command (parseinfo *csb)
{
    switch (csb->which) {

    case DEBUG_SDLC_LACK:
	if (csb->sense) {
	    slack_debug = GETOBJ(int,1);
	} else {
	    slack_debug = 0;
	}
	printf ("SDLC Local Ack events debugging is %s",
		slack_debug ? "on" : "off");
	break;

    case DEBUG_SDLC_PACKETS:
	if (csb->sense) {
	    sdlc_packet_debug = GETOBJ(int,1);
	} else {
	    sdlc_packet_debug = 0;
	}
	printf ("SDLC packet debugging is %s",
		sdlc_packet_debug ? "on" : "off");
	break;

    case DEBUG_STUNPACKET:
	stun_packet_debug_work(csb->sense, (ulong) GETOBJ(int,1),
			       (boolean) GETOBJ(int,2),
			       GETOBJ(paddr,1)->stun_addr);
	printf ("STUN packet debugging is %s",
		stun_packet_debug ? "on" : "off");
	break;

    case DEBUG_STUNTG:
        stun_tg_debug = csb->sense;
        printf ("STUN Transmission Groups debugging is %s",
                stun_tg_debug ? "on" : "off");
        break;

    default:
	/* If there's no special case, use the generic handler */
	debug_command(csb);
	break;
    }
}


/*
 * sdlc_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the SDLC debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void sdlc_debug_all (boolean flag)
{

    if (!flag) {
	slack_debug = 0;
	sdlc_packet_debug = 0;
	stun_packet_debug = FALSE;
        stun_tg_debug = FALSE;
	stun_debug_group = 0;
	stun_debug_addr_significant = FALSE;
    } else {
	/* slack_debug contains bitmapped options... */
	slack_debug = ~0;
	stun_packet_debug = TRUE;
        stun_tg_debug = TRUE;
    }

    generic_debug_all(sdlc_debug_arr, flag);
}

/*
 * sdlc_debug_show is called to display the values of all the SDLC
 * debugging variables.
 */

static void sdlc_debug_show (void)
{
    char buff[STUN_MAX_ADDR_LENGTH];     /* For formatting STUN addrs */
    
    /* First show the flags that follow the standard scheme */
    debug_show_flags(&(sdlc_debug_arr[0]), "SDLC and STUN");

    /* Now the nonstandard ones... */
    if (!(slack_debug) != !(debug_all_p()))
	printf("\n  SDLC Local Ack events debugging is %s",
	       slack_debug ? "on" : "off");

    if (!(sdlc_packet_debug) != !(debug_all_p()))
	printf("\n  SDLC packet debugging is %s",
	       sdlc_packet_debug ? "on" : "off");

    if (!(stun_tg_debug) != !(debug_all_p()))
	printf("\n  STUN TG debugging is %s",
	       stun_tg_debug ? "on" : "off");

    if (((!stun_packet_debug) != (!debug_all_p())) ||
	(stun_packet_debug && stun_debug_group) ||
	(stun_packet_debug && stun_debug_addr_significant)) {
	printf("\n  STUN packet debugging is %s",
	       stun_packet_debug ? "on" : "off");
	if (stun_debug_group)
	    printf(" group %u", stun_debug_group);
	if (stun_debug_addr_significant) {
	    stun_sprintf_addr(stun_debug_addr, FALSE, (int) stun_debug_group,
			      buff);
	    printf(" address %s", buff);
	}
    }
}

/*
 * sdlc_debug_init is called at SDLC subsystem startup. It registers
 * the routines to maintain and display the SDLC debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SDLC is started.
 */

void sdlc_debug_init (void)
{
    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(sdlc_debug_all, "sdlc_debug_all");
    reg_add_debug_show(sdlc_debug_show, "sdlc_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SDLC is initialized, we want to turn on
     * all the SDLC debugging right now.
     */
    sdlc_debug_all(debug_all_p());
}


static void stun_packet_debug_work (
    boolean	sense,
    ulong	u,
    boolean	addr_set,
    uchar	*addr)
{
    if (u == 0) {
	stun_debug_group = 0;
	stun_debug_addr_significant = FALSE;
	stun_packet_debug = sense;
	return;
    } else {
	if (stun_pinfo[u] == NULL) {
	    printf("\nno such stun group -- stun-packet debugging disabled");
	    stun_packet_debug = FALSE;
	    return;
	}
	stun_debug_group = u;
	if (addr_set == FALSE) {
	    stun_debug_addr_significant = FALSE;
	    stun_packet_debug = sense;
	    return;
	}
	if (stun_pinfo[stun_debug_group]->onlyall) {
	    printf("\nno addresses in the %s protocol. stun-packet debugging disabled",
		   stun_pinfo[u]->name);
	    stun_debug_group = 0;
	    stun_packet_debug = FALSE;
	    return;
	}
	stun_debug_addr_significant = TRUE;
	if (!stun_parse_addr_core(stun_debug_group, addr, stun_debug_addr)) {
	    printf("\nmalformed address for this group. stun-packet debugging disabled");
	    stun_debug_group = 0;
	    stun_debug_addr_significant = FALSE;
	    stun_packet_debug = FALSE;
	} else {
	    stun_packet_debug = sense;
	}
    }
}

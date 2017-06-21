/* $Id: dhcp_debug.c,v 3.3.20.3 1996/07/25 09:47:12 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/dhcp_debug.c,v $
 *------------------------------------------------------------------
 * dhcp_debug.c - DHCP proxy client debugging support
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dhcp_debug.c,v $
 * Revision 3.3.20.3  1996/07/25  09:47:12  snyder
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
 * Revision 3.3.20.2  1996/07/23  18:31:56  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/03/18  20:15:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:37:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  00:58:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/29  07:28:27  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:32:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:56:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "config.h"
#include "dhcp_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "dhcp_debug_flags.h"

/* "dhcp_deb_int" is used to restrict the output of DHCP debug */

static idbtype *dhcp_deb_int = NULL;

boolean dhcp_debug;

/*
 * dhcp_debug_command
 * This is the routine the parser calls for Dhcp debug commands. It
 * handles the special case of a line number on "debug dhcp line",
 * and punts everything else to the generic handler.
 */

void dhcp_debug_command (parseinfo *csb)
{

    idbtype *idb = GETOBJ(idb,1);

    /*
     * When turning on dhcp  debugging for an interface, make sure
     * that the interface is a point to point interface.
     */

    if (csb->sense  && idb && !(is_p2p(idb))) {

	printf("\n%%Cannot debug DHCP on this interface");
	return;
    }

    dhcp_deb_int = idb;

    (const) SETOBJ(pdb,1) = dhcp_debug_arr;

    /* Everything else is generic */
    debug_command(csb);

    if (csb->sense){
	dhcp_debug = TRUE;
	if(dhcp_deb_int)
	    printf(" for interface %s", dhcp_deb_int->namestring);
    }
    else {
	dhcp_debug = FALSE;
	dhcp_deb_int = NULL;
    }
}

/*
 * dhcp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the dhcp debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void dhcp_debug_all (boolean flag)
{

    dhcp_deb_int = NULL;

    generic_debug_all(dhcp_debug_arr, flag);
}

/*
 * dhcp_debug_show is called to display the values of all the dhcp
 * debugging variables.
 */

void dhcp_debug_show (void)
{
    debug_show_flags(&(dhcp_debug_arr[0]), "DHCP");
    if (dhcp_debug && dhcp_deb_int)
	printf(" for interface %s", dhcp_deb_int->namestring);
}

/*
 * dhcp_debug_init is called at dhcp subsystem startup. It registers
 * the routines to maintain and display the dhcp debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when dhcp is started.
 */

void dhcp_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(dhcp_debug_all, "dhcp_debug_all");
    reg_add_debug_show(dhcp_debug_show, "dhcp_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when dhcp is initialized, we want to turn on
     * all the dhcp debugging right now.
     */
    dhcp_debug_all(debug_all_p());
}

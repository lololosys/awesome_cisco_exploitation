/* $Id: tcp_debug.c,v 3.4.4.2 1996/07/25 09:55:27 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcp_debug.c,v $
 *------------------------------------------------------------------
 * tcp_debug.c -- Debugging routines for General TCP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcp_debug.c,v $
 * Revision 3.4.4.2  1996/07/25  09:55:27  snyder
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
 * Revision 3.4.4.1  1996/07/23  18:32:33  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/09  22:57:45  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1996/01/24  20:41:10  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.2  1995/11/17  18:48:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "../tcp/tcp_debug.h"
#include "../ip/ip.h"
#include "../parser/parser_defs_tcpip.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "tcp_debug_flags.h"

/* "tcplineno" is used to restrict the output of TCP packet debugging */
int tcplineno = 0;

/* These are also used to further restrict output */
ushort tcp_dbg_port = 0;
addrtype tcp_dbg_addr = { ADDR_ILLEGAL };
tcp_dbg_dir_type tcp_dbg_dir = TCP_DIR_DISABLED;



/*
 * tcp_debug_explicate
 * Print out the extra debugging options active for "debug ip tcp packet"
 */

static void tcp_debug_explicate (void)
{
    int opts;

    opts = 0;

    if (tcplineno) {
	printf("%s line number %d", opts ? "," : " for", tcplineno);
	opts++;
    }

    if (tcp_dbg_addr.type != ADDR_ILLEGAL) {
	printf("%s address %s", opts ? "," : " for",
	       address_string(&tcp_dbg_addr));
	opts++;
    }

    if (tcp_dbg_port) {
	printf("%s port number %d", opts ? "," : " for", tcp_dbg_port);
	opts++;
    }

    if (tcp_dbg_dir != TCP_DIR_DISABLED) {
	printf("%s %s packets", opts ? "," : " for",
	       (tcp_dbg_dir == TCP_DIR_IN) ? "incoming" : "outgoing");
	opts++;
    }
}

/*
 * tcp_debug_command
 * This is the routine the parser calls for TCP debug commands. It
 * handles the special case of a line number on "debug tcp packet",
 * and punts everything else to the generic handler.
 */

void tcp_debug_command (parseinfo *csb)
{
    if (csb->which == DEBUG_TCPPKT) {
	tcplineno = GETOBJ(int,1);
	tcp_dbg_port = GETOBJ(int,2);
	address_copy(&tcp_dbg_addr, GETOBJ(paddr,1));
	switch (GETOBJ(int,3)) {
	case TCPPKT_DBG_IN:  tcp_dbg_dir = TCP_DIR_IN; break;
	case TCPPKT_DBG_OUT: tcp_dbg_dir = TCP_DIR_OUT; break;
	default:	     tcp_dbg_dir = TCP_DIR_DISABLED; break;
	}
    }
    (const) SETOBJ(pdb,1) = tcp_debug_arr;

    /* Everything else is generic */
    debug_command(csb);

    if (csb->which == DEBUG_TCPPKT)
	tcp_debug_explicate();
}

/*
 * tcp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the TCP debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void tcp_debug_all (boolean flag)
{

    if (!flag) {
	/* Disable debugging filters. */
	tcplineno = 0;
	tcp_dbg_port = 0;
	tcp_dbg_addr.type = ADDR_ILLEGAL;
	tcp_dbg_dir = TCP_DIR_DISABLED;
    }

    generic_debug_all(tcp_debug_arr, flag);
}

/*
 * tcp_debug_show is called to display the values of all the TCP
 * debugging variables.
 */
static void tcp_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE;	/* Printed the heading yet? */
    boolean willprint;		/* Will we print anything on this line? */

    for (i = 0; tcp_debug_arr[i].var != (boolean *) NULL; i++) {

	/*
	 * Normally, we only print stuff that doesn't match the current
	 * status of "debug all". However, if something restricts the
	 * output associated with a flag that's turned on, we need to
	 * talk about that flag even if "debug all" is turned on.
	 */
	willprint = (!*(tcp_debug_arr[i].var) != !debug_all_p());
	if ((i == DEBUG_TCPPKT) && tcppkt_debug &&
	    (tcplineno || tcp_dbg_addr.type != ADDR_ILLEGAL ||
	     tcp_dbg_port || tcp_dbg_dir != TCP_DIR_DISABLED))
	    willprint = TRUE;

	if (willprint) {
	    if (!hdisplayed) {
		printf("\nTCP:");
		hdisplayed = TRUE;
	    }

	    printf("\n  %s debugging is %s",
		   tcp_debug_arr[i].txt,
		   *(tcp_debug_arr[i].var) ? "on" : "off");

	    if (i == DEBUG_TCPPKT)
		tcp_debug_explicate();
	}
    }
}

/*
 * tcp_debug_init is called at TCP subsystem startup. It registers
 * the routines to maintain and display the TCP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when TCP is started.
 */

void tcp_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(tcp_debug_all, "tcp_debug_all");
    reg_add_debug_show(tcp_debug_show, "tcp_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when TCP is initialized, we want to turn on
     * all the TCP debugging right now.
     */
    tcp_debug_all(debug_all_p());
}

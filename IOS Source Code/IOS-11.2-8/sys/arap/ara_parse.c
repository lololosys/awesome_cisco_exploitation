/* $Id: ara_parse.c,v 3.5 1996/02/07 16:09:25 widmer Exp $
 * $Source: /release/111/cvs/Xsys/arap/ara_parse.c,v $
 *------------------------------------------------------------------
 * ara_parse.c -
 *	parse ara commands and startup server
 *
 * September 1993, Russ Tremain, Tremain Laboratories, under contract
 *  for cisco Systems, Inc.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ara_parse.c,v $
 * Revision 3.5  1996/02/07  16:09:25  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1995/12/09  00:43:21  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.3  1995/11/28  03:58:13  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  08:38:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:48:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/09  12:58:13  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:04:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../h/config.h"
#include "ttysrv.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_registry.h"
#include "../atalk/macip_util.h"
#include "../atalk/at_fsm.h"
#include "arap_public.h"
#include "ara_srvr_fsm.h"
#include "ara_conn_fsm.h"
#include "ara_input.h"
#include "ara_db.h"
#include "ara_stats.h"
#include "ara_server.h"
#include "config.h"
#include "../ui/debug.h"
#include "ara_debug.h"
#include "../parser/parser_defs_autoselect.h"
#include "../h/cs_registry.regh"
#include "arap_mnp4.h"

static void arap_setup_alllines (setup_struct *);


void ara_init(subsystype *);

/* definitions for at_err(): */
#define DEBUGSWITCH (at_debug(appletalk_debug, ATALK_NULL_MASK))

#define ARAP_MAJVERSION 1
#define ARAP_MINVERSION 0
#define ARAP_EDITVERSION 1

SUBSYS_HEADER(arap, ARAP_MAJVERSION, ARAP_MINVERSION, ARAP_EDITVERSION,
	      ara_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk",
	      "req: atalk");

/*************************  PRIVATE FUNCTIONS  ************************/

#define ARAP1_AUTOSELECT	0x00161002
#define ARAP1_AUTOSELECT_MASK	0x00ffffff
#define ARAP2_AUTOSELECT	0x00011b02
#define ARAP2_AUTOSELECT_MASK	0x00ffffff

static boolean autoselect_arap(tt_soc *tty, uint sample)
{

    if ((sample & ARAP1_AUTOSELECT_MASK) == ARAP1_AUTOSELECT) {
	tty->arap_version = ARAP_VERSION1;
	return(TRUE);
    }

    if ((sample & ARAP2_AUTOSELECT_MASK) == ARAP2_AUTOSELECT) {
	tty->arap_version = ARAP_VERSION2;
	return(TRUE);
    }

    return(FALSE);
}

static boolean ara_start_process (tt_soc *tty, uint sample)
{
    if (tty->capabilities & MUSTARAP) { 
	process_sleep_for(3*ONESEC);
	tty->tiptop = cfork ((forkproc *) arap, (long) tty, 2000,
			     "ARAP", tty->ttynum);
	tty->statbits &= ~CARDROP;
	CLAIM_TTY(tty);
       	return (TRUE);
    } 

    if ((tty->autoselectmask & PARSER_AUTOSELECT_ARAP) 
	&& (tty->capabilities & MAYARAP)) {

	if ((tty->acflags & ACF_CALLBACK_ARAP) ||
            autoselect_arap(tty, sample)) {
	    tty->tiptop = cfork ((forkproc *) arap, (long) tty, 2000,
				 "ARAP", tty->ttynum);
	    tty->statbits &= ~CARDROP;
	    CLAIM_TTY(tty);
       	    return (TRUE);
    	}

	/* otherwise, fall through and go with EXEC */
    }
    return (FALSE);
}

/*
 * arap_return_nothing
 * Just return for TTY lines
 */
static void arap_return_nothing (tt_soc *tty)
{
    return;
}

/*
 * ara_register_thyself
 */
void ara_register_thyself (void)
{
    ara_parser_init();
}

/*************************  EXPORTED FUNCTIONS  ***********************/

boolean ara_initialized;

/*
 * ara_init -
 *	initialize AppleTalk Remote Access data structures
 */
void
ara_init (subsystype *subsys)
{
    if (ara_initialized)
	return;			/* already initialized */

    init_arap_globals();

    /* debugging init */
    ara_debug_init();
    ara_register_thyself();
    reg_add_exec_process(ara_start_process, "ara_start_process");
    reg_add_proto_send_message(ara_send_message, "ara_send_message");
    reg_add_atalk_intercept(arap_intercept, "arap_intercept");
    reg_add_alllines_setup(arap_setup_alllines, "arap_setup_alllines");
    reg_add_default_arap_start_pt_helper_process(arap_return_nothing,
						 "arap_return_nothing");
    reg_add_arap_callback_enabled(arap_callback_enabled, "arap_callback_enabled");
    reg_add_death(arap_cleanup, "arap_cleanup");
    ara_initialized = TRUE;
}
#ifdef	DELETE

/*
 * ara_command -
 *	parse appletalk remote command
 *
 *	statement    --> 'no'? 'appletalk' 'remote' 'lines' '<int>' '<int>'
 *						   ^
 *				you are here in the parse.
 *	
 *	"no appletalk remote" ==> shutdown
 */
void
ara_command (parseinfo *csb)
{
    if (csb->nvgen) {
	if (ara_configured)
	    aradb_nv_ara(csb);
	return;
    }

    /* if not yet initialized... */
    if (!ara_initialized) {
        at_err(MODID,"ara_parse:  ARA not initialized!");
        return;
    }

    switch (GETOBJ(int,1)) {
      case ARA_DISABLE:
	/* "no appletalk remote" ==> SHUTDOWN */
	if (!csb->sense) {
	    ara_shutdown();
	}
	break;

      case ARA_LINES:
        (void) ara_lines(csb);
	break;

      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
        break;
    }
}
#endif	DELETE

#include "../ui/setup.h"

static void arap_setup_alllines (setup_struct *sp)
{
    char *zone_name, buffer[MAXBUF];
    long atnet;

    if (sp->args[LINK_APPLETALK])
	if (yes_or_no("    Configure for Appletalk Remote Access",
		      TRUE, TRUE)) {
	    atnet = get_num_in_range("      AppleTalk Network for ARAP clients",
				     10, 1, 65279,
				     arap_net != 65535 ? arap_net : 1);
	    zone_name = setup_string(buffer,
			"      Zone name for ARAP clients",
			arap_zonelen ? (char *) arap_zone : "ARA Dialins");
	    setup_printf(sp,
			 "\n!"
			 "\narap network %d %s",
			 atnet, zone_name);
	    setup_printf(sp,
			 "\nline 1 %t"
			 "\narap enable"
			 "\nautoselect",
			 AuxBase-1);
	}
}

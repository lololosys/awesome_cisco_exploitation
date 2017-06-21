/* $Id: logger_chain.c,v 3.6.10.6 1996/06/28 23:24:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/logger_chain.c,v $
 *------------------------------------------------------------------
 * Logger parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: logger_chain.c,v $
 * Revision 3.6.10.6  1996/06/28  23:24:49  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.10.5  1996/06/19  04:44:01  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/06/17  23:35:26  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  09:58:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/05/21  06:36:36  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.10.1  1996/03/18  21:30:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:20:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/09  05:09:54  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.14.1  1996/02/20  16:40:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:36:44  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:07:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:13  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:34:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:48:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:56:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "logger_private.h"
#include "ttysrv.h"
#include "connect.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_logger.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for Logging show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_logging.h"
LINK_POINT(logging_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Logging clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_logging.h"
LINK_POINT(logging_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Logging config commands
 */
#define ALTERNATE       NONE
#include "cfg_logging.h"
LINK_POINT(logging_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Logging line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_monitor.h"
LINK_POINT(logging_line_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Logging
 */
static parser_extension_request logger_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(logging_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(logging_clear_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(logging_config_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(logging_line_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * monitor_command
 * Enable logging on the specified terminal lines
 */

void monitor_command (parseinfo *csb)
{
    ttylisttype *tp;
    tt_soc *tty = NULL;
    conntype *conn;
    int i;

    if (csb->nvgen) {
	nv_write(MODEMS[csb->line1]->def_capabilities & LOGGING,
		 "%s", csb->nv_command);
	return;
    }
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (csb->sense == FALSE) {
	    /* This will never disable the console's logging */
	    logger_ttydel(tty);
	    continue;
	}
	if (tty == (tt_soc *) console) {
	    if (log_console == LOG_DISABLED)
		printf("\n%% Use \"logging console\" instead");
	    else
		printf("\n%% Console already monitors");
	    continue;
	}
	if (tty->type == VTY_LINE && (tty->statbits & HASPROC) == 0) {
	    printf("\nVTY must be active ");
	    continue;
	}
	for (tp = (ttylisttype *) logttyQ.qhead; tp; tp = tp->next) {
	    if (tp->tty == tty)
		break;
	}
	if (tp)
	    continue;
	tp = malloc(sizeof(ttylisttype));
	if (tp) {
	    tty->capabilities |= LOGGING;
	    if (csb->permanent)
		tty->def_capabilities |= LOGGING;
	    tp->tty = tty;
	    conn = tty->conntop;		/* Get connection block */
	    if (conn) {
		tp->addr = conn->address.ip_addr; /* Get any IP address */
	    } else {
		tp->addr = 0L;
	    }
	    enqueue(&logttyQ, tp);
	} 
    }
}

/*
 * logger_level
 * Return string about what level we are
 */

char *
logger_level (int level)
{
    switch (level) {
    case LOG_EMERG:
	return ("emergencies");
    case LOG_ALERT:
	return ("alerts");
    case LOG_CRIT:
	return ("critical");
    case LOG_ERR:
	return ("errors");
    case LOG_WARNING:
	return ("warnings");
    case LOG_NOTICE:
	return ("notifications");
    case LOG_INFO:
	return ("informational");
    case LOG_DEBUG:
	return ("debugging");
    }
    return ("Unknown");
}

/*
 * logger_nvgen_level
 *
 * Write out the logging level (if applicable) for the specified device
 */

static void logger_nvgen_level (parseinfo *csb,
			 int        level,
			 int        default_level)
{
    if (level != default_level) {
	if (level == LOG_DISABLED) {
	    nv_write(TRUE, "no %s", csb->nv_command);
	} else {
	    nv_write(TRUE, "%s %s", csb->nv_command,
		     logger_level(level));
	}
    }
}


/*
 * logger_command
 *
 * Called by configurator to setup logging
 */

void logger_command (parseinfo *csb)
{
    uint default_size, size;
    int  default_level, level;

    /*
     * Get the defaults for buffer size and level
     */
    default_size  = logger_get_default_buffer_size(TRUE);
    default_level = logger_get_default_level(GETOBJ(int,3));

    /*
     * If LOG_DEFAULT is passed through, we use the default logging level
     * for this device instead
     */
    level = (GETOBJ(int, 2) == LOG_DEFAULT) ? default_level : GETOBJ(int,2);

    /*
     * Handle configuration generation
     */
    if (csb->nvgen) {
	/*
	 * If we've no logging, return
	 */
	if (!log_init)
	    return;

	switch (csb->which) {
	  case P_LOG_ON:
	    /*
	     * nothing
	     */
	    break;
	  case P_LOG_LEVEL:
	    switch (GETOBJ(int,3)) {
	      case P_LOG_CONSOLE:
		logger_nvgen_level(csb, log_console, default_level);
		break;
	      case P_LOG_MONITOR:
		logger_nvgen_level(csb, log_monitor, default_level);
		break;
	      case P_LOG_TRAP:
		logger_nvgen_level(csb, log_traps, default_level);
		break;
	      case P_LOG_BUFFERED:
		if ((log_buffer_size != default_size) ||
		    (log_buffer != default_level)) {
		    if (log_buffer == LOG_DISABLED) {
			nv_write(TRUE, "no %s", csb->nv_command);
		    } else {
			nv_write(TRUE, "%s", csb->nv_command);
			nv_add((log_buffer_size != default_size),
			       " %u", log_buffer_size);
			nv_add(TRUE, " %s", logger_level(log_buffer));
		    }
		}
		break;
	    }
	    break;
	  case P_LOG_HOST:
	  case P_LOG_SOURCE:
	    reg_invoke_log_config(csb);
	    break;
	  case P_LOG_FACILITY:
	    nv_write (log_facility != P_LOG_FAC_LOCAL7 && 
		      log_facility == GETOBJ(int,4), csb->nv_command);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case P_LOG_ON:		/* no log on */
	log_init = csb->sense;
	break;
      case P_LOG_LEVEL:
	switch (GETOBJ(int,3)) {
	  case P_LOG_CONSOLE:
	    if (csb->set_to_default)
		log_console = default_level;
	    else 
		log_console = (csb->sense ? level : LOG_DISABLED);
	    break;
	  case P_LOG_MONITOR:
	    if (csb->set_to_default)
		log_monitor = default_level;
	    else 
		log_monitor = (csb->sense ? level : LOG_DISABLED);
	    break;
	  case P_LOG_TRAP:
	    if (csb->set_to_default)
		log_traps = default_level;
	    else 
		log_traps = (csb->sense ? level : LOG_DISABLED);
	    break;
	  case P_LOG_BUFFERED:
	    if (csb->set_to_default) {
		log_buffer = default_level;
		size       = default_size;
	    } else {
		log_buffer = (csb->sense ? level : LOG_DISABLED);
	        size = GETOBJ(int,4) ? GETOBJ(int,4) : default_size;
	    }
	    /*
	     * We enable the buffer if we're setting the default and the
	     * default_size is non-zero or csb->sense is TRUE
	     */
	    logger_buffer_init(size,
			       (csb->set_to_default && default_size) ||
			       csb->sense,
			       FALSE);
	    break;
	}
	break;
      case P_LOG_HOST:
      case P_LOG_SOURCE:
	reg_invoke_log_config(csb);
	break;
      case P_LOG_FACILITY:
	if (csb->sense) {
	    log_facility = GETOBJ(int,4);
	} else {
	    log_facility = P_LOG_FAC_LOCAL7;
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * logger_clear_command
 *
 * Called by user to clear logging buffer
 */

void logger_clear_command (parseinfo *csb)
{
    if (confirm("Clear logging buffer ")) {
	/*
	 * Reset the end pointer back to the start of the buffer
	 */
        logger_clear ();
    }
}

/*
 * show_logging
 *
 * Display information about logging
 */

void display_logger (parseinfo *csb)
{
    ttylisttype *tp;

    automore_enable(NULL);
    printf("\nSyslog logging: %s (%u messages dropped, %u flushes, "
	   "%u overruns)", log_init ? "enabled" : "disabled",
	   log_drop_count, log_flush_count, log_overrun_count);
    
    /*
     * Print console statistics
     */
    printf("\n    Console logging: ");
    if (log_console >= 0)
	printf("level %s, %u messages logged", logger_level(log_console),
	    log_console_count);
    else
	printf("disabled");

    /*
     * Print monitor statistics
     */
    printf("\n    Monitor logging: ");
    if (log_monitor >= 0) {
	printf("level %s, %u messages logged", logger_level(log_monitor),
	       log_monitor_count);
	if (logttyQ.qhead) {
	    printf("\n        Logging to:");
	    for (tp = (ttylisttype *)logttyQ.qhead; tp; tp = tp->next)
		printf(" %s%t(%u)", tp->tty->type == VTY_LINE ? "vty" : "tty",
		    tp->tty->ttynum, tp->count);
	}
    } else {
	printf("disabled");
    }

    /*
     * Print trap statistics
     */
    printf("\n    Trap logging: ");
    if (log_traps >= 0) {
	printf("level %s, %u message lines logged", logger_level(log_traps),
	       log_traps_count);
	(void) reg_invoke_log_show();
    } else
	printf("disabled");

    /*
     * Print buffer statistics and log
     */
    printf("\n    Buffer logging: ");
    if ((log_buffer >= 0) && (log_buffer_addr)) {
	printf("level %s, %u messages logged", logger_level(log_buffer),
	       log_buffer_count);
	automore_conditional(0);
	printc('\n');
	printf("\nLog Buffer (%d bytes):", log_buffer_size);
	logger_print_buffer(stdio, TRUE);
    } else {
	printf("disabled");
    }

    automore_disable();
}

/*
 * logger_parser_init - Initialize Logging parser support
 */
void logger_parser_init (void)
{
    parser_add_command_list(logger_chain_init_table, "logger");
    parser_add_link_point(PARSE_ADD_CFG_LOG_CMD,
                        "logging configuration",
                        &pname(conf_log_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_LOG_CMD,
                        "show logging",
                        &pname(show_logging_extend_here));
}





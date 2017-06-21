/* $Id: registry_chain.c,v 3.7.10.2 1996/05/21 09:58:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/registry_chain.c,v $
 *------------------------------------------------------------------
 * Parser chain information and support for Registries
 *
 * November 1993, Scott Mackie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: registry_chain.c,v $
 * Revision 3.7.10.2  1996/05/21  09:58:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/03/18  21:31:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:21:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/07  10:19:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  16:41:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  14:36:50  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:07:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:29:17  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/25  11:20:36  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1995/12/17  18:34:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/07  22:01:49  smackie
 * Remove between 50 and 85K of text segment space by compiling out the
 * little-used registry service and function strings. (CSCdi36802)
 *
 * Revision 2.3  1995/07/02  10:51:18  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/20  00:44:33  ahh
 * CSCdi36133:  CPU Hog during show registry
 * Handle quit at more prompt.
 *
 * Revision 2.1  1995/06/07  21:58:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "logger.h"
#include "subsys.h"
#include "registry.h"
#include "registry_private.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_registry.h"


/*
 * Parser chains for "show registry" and the like.
 */
#define ALTERNATE NONE
#include "exec_show_registry.h"
LINK_POINT(registry_show_commands, ALTERNATE);
#undef ALTERNATE



/*
 * Registry parse chain table
 */
static parser_extension_request registry_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(registry_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * registry_ui_init
 *
 * Initialize registry parser support.
 *
 */

static void registry_ui_init (subsystype *subsys)
{
    parser_add_command_list(registry_chain_init_table, "registry");
}


/*
 * registry_function_show
 *
 * Helper routine for registry_show()
 */

static void registry_show_function (regservicetype *serv,
				    functype       *func,
				    boolean         overflow)
{
    char *name;

    name = REGISTRY_STRING(func->name);

    if ((serv->servtype == SERVICE_CASE) || (serv->servtype == SERVICE_VALUE))
	printf("\n%-10d", func->index);
    else
	printf("\n%-10c", ' ');
    if (name)
	printf("  %s", name);
    else
	printf("  0x%08x", func->j.routine);
    if (overflow)
	printf("  <-- overflow");
}

/*
 * registry_show_name
 * Given a registry type, return its name string.
 * Helper routine for registry_show().
 */

static char *registry_show_name (int code)
{
    switch (code) {
      case SERVICE_LIST:  return("List");
      case SERVICE_CASE:  return("Case");
      case SERVICE_STUB:  return("Stub");
      case SERVICE_VALUE: return("Value");
      case SERVICE_PID_LIST: return("Pid List");
      case SERVICE_PID_CASE: return("Pid Case");
    }
    return("Unknown");
}

/*
 * registry_show_summary
 *
 * Show a summary of registry names and service counts
 */

static void registry_show_summary (void)
{
    int           regcode;
    registrytype *reg;

    printf("\n");
    for (regcode = REG_MINCOUNT; regcode < REG_MAXCOUNT; regcode++) {
	reg = &registry_array[regcode];
	if (!registry_populated(regcode))
	    continue;
	
	printf("\n    Registry %-2d: %s (%d services)",
	       regcode, reg->name, reg->servicecount);
    }
}

/*
 * registry_show_statistics
 *
 * Show item and byte statistics for the registry support
 */

void registry_show_statistics (parseinfo *csb)
{
    int service_bytes, service_items, servtype;

    service_bytes = 0;
    service_items = 0;

    printf("\nRegistry statistics:");

    for (servtype = SERVICE_MIN; servtype < SERVICE_MAX; servtype++) {
	service_bytes += registry_service_statistics[servtype].bytes;
	service_items += registry_service_statistics[servtype].items;
    }

    printf("\n    %d registries (%d total bytes)",
	   registry_statistics.items,
	   registry_statistics.bytes +
	   service_bytes +
	   registry_function_statistics.bytes);
    printf("\n    %d services (%d bytes), %d functions (%d bytes)",
	   service_items,
	   service_bytes,
	   registry_function_statistics.items,
	   registry_function_statistics.bytes);
    printf("\n");

    for (servtype = SERVICE_MIN; servtype < SERVICE_MAX; servtype++) {
	printf("\n    %-3d %s services (%d bytes)",
	       registry_service_statistics[servtype].items,
	       registry_show_name(servtype),
	       registry_service_statistics[servtype].bytes);
    }
    printf("\n");
}

/*
 * registry_show
 * Show a list of registries, services, and functions registered
 */
void registry_show (parseinfo *csb)
{
    char           *regname;
    char           *servname;
    char           *name;
    boolean         brief;
    boolean         found_regmatch;
    boolean         found_servmatch;
    registrytype   *reg;
    regservicetype *serv;
    functype       *func;
    regtype_t       regcode;
    int             servcode, index, regtarget, servtarget;
    boolean         overflow, dummy;

    regname         = GETOBJ(string,1);
    servname        = GETOBJ(string,2);
    brief           = GETOBJ(int,1);
    found_regmatch  = FALSE;
    found_servmatch = FALSE;

    if (strlen(regname)) {
	regtarget = parse_unsigned(regname, &dummy);
	if (dummy)
	    regname = NULL;
    } else {
	regtarget = -1;
    }

    if (strlen(servname)) {
	servtarget = parse_unsigned(servname, &dummy);
	if (dummy)
	    servname = NULL;
    } else {
	servtarget = -1;
    }

    automore_enable(NULL);

    if (!brief && (regtarget == -1) && (servtarget == -1) && null(regname))
	registry_show_summary();

    for (regcode = REG_MINCOUNT; regcode < REG_MAXCOUNT; regcode++) {
	/*
	 * Match the registry.
	 */
	reg = &registry_array[regcode];
	if (!registry_populated(regcode))
            continue;
	
	if (regtarget != -1) {
	    if (regcode != regtarget)
		continue;
	} else {
	    if (!null(regname) && !partialmatch(reg->name, regname))
		continue;
	}
    
	found_regmatch = TRUE;

	if (automore_quit())
	    break;

	printf("\n\n--\nRegistry %d: %s (%d services)",
	       regcode, reg->name, reg->servicecount);
	found_servmatch = FALSE;

	for (servcode = 0; servcode < reg->servicecount; servcode++) {
	    serv = &registry_array[regcode].services[servcode];
	    name = REGISTRY_STRING(serv->name);

	    if (serv->servtype != SERVICE_NONE) {
		if (servtarget != -1) {
		    if (servtarget != servcode)
			continue;
		} else {
		    if (!null(servname) && !partialmatch(name,servname))
			continue;
		}

		found_servmatch = TRUE;
		
		if (automore_quit())
		    break;

		printf("\n  Service %d/%d:", regcode, servcode);
		if (name)
		    printf(" service_%s", name);

		printf("\n      %s service with %d argument%s",
		       registry_show_name(serv->servtype),
		       serv->numargs, serv->numargs != 1 ? "s" : "");
		
		if (brief)
		    continue;

		/*
		 * Print out any case table first, then the linked list.
		 */
		if (serv->function_table) {
		    printf(", %d maximum cases", serv->maxentries);
		    for (index = 0; index < serv->maxentries; index++) {
			func = &serv->function_table[index];
			if (func->i.assigned)
			    registry_show_function(serv, func, FALSE);
		    }
		}
		
		/*
		 * An overflow means we have a case table, but there
		 * wasn't enough room for certain functions, so we added
		 * them to the linked list.  This makes for robust, but
		 * potentially slower operation.  People should fix
		 * overflow cases.
		 */
		func = serv->function_queue;
		overflow = (serv->function_table && func);
		for (; func; func = func->i.next)
		    registry_show_function(serv, func, overflow);

		if (serv->function_default) {
		    if (serv->servtype == SERVICE_CASE) {
			name = REGISTRY_STRING(serv->function_default->name);
			if (name) {
			    printf("\n    default %s", name);
			} else {
			    printf("\n    default 0x%08x",
				   serv->function_default->j.routine);
			}
		    } else {
			registry_show_function(serv, serv->function_default,
					       FALSE);
		    }
		}
	    }
	}
	if (!found_servmatch) {
	    printf("\n\n  No services found matching \"%s\"",
		   GETOBJ(string, 2));
	}
    }
    if (!found_regmatch) {
	printf("\n\nNo registries found matching \"%s\"", GETOBJ(string, 1));
    }

    automore_disable();
}

/*
 * registry_ui subsystem header
 */
 
#define REGISTRY_UI_MAJVERSION    1
#define REGISTRY_UI_MINVERSION    0
#define REGISTRY_UI_EDITVERSION   1
 
SUBSYS_HEADER(registry_ui, REGISTRY_UI_MAJVERSION, REGISTRY_UI_MINVERSION,
              REGISTRY_UI_EDITVERSION,
              registry_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);


/* $Id: subsys_chain.c,v 3.4.10.3 1996/09/06 01:11:49 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/subsys_chain.c,v $
 *------------------------------------------------------------------
 * subsys_chain.c - Parser support for subsystems
 *
 * June 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: subsys_chain.c,v $
 * Revision 3.4.10.3  1996/09/06  01:11:49  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/05/21  09:58:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.1  1996/03/18  21:31:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.14.3  1996/03/16  07:21:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.14.2  1996/03/07  10:20:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.14.1  1996/02/20  16:42:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/22  14:36:55  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1996/02/16  00:48:43  hampton
 * Migrate files out of the parser directory.  [CSCdi49139]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/12/17  18:34:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/09  12:48:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:19  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/28  09:18:50  smackie
 * Finally make subsystem sequencing robust with multiple hierarchy
 * sequences. Tidy up subsystem code into the bargain and split the
 * parser code into a new file. Make the subsystem code far noisier
 * about bogusness. (CSCdi23568)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "logger.h"
#include "subsys.h"
#include "subsys_private.h"
#include "packet.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "os_debug.h"


/*
 * Parse chains for subsystem exec commands
 */
#define	ALTERNATE	NONE
#include "exec_show_subsystem.h"
LINK_POINT(show_subsystem_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Debug chains for subsystem exec commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_subsys.h"
LINK_POINT(debug_subsystem_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Subsys
 */
static parser_extension_request subsystem_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_subsystem_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_subsystem_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * subsys_ui_init - Initialize show subsystem parser support
 */
static void subsys_ui_init (subsystype *subsys)
{
    parser_add_command_list(subsystem_chain_init_table, "subsystem");
}

/*
 * subsys_ui subsystem header
 */
 
#define SUBSYS_UI_MAJVERSION    1
#define SUBSYS_UI_MINVERSION    0
#define SUBSYS_UI_EDITVERSION   1
 
SUBSYS_HEADER(subsys_ui, SUBSYS_UI_MAJVERSION, SUBSYS_UI_MINVERSION,
              SUBSYS_UI_EDITVERSION,
              subsys_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);


/*
 * Local Storage
 */
static const char subsys_show_header[] = "\n\n                    Class         Version   Required Subsystems";


/*
 * subsys_show_item
 * Helper routine to display a single subsystem
 */

static void subsys_show_item (subsystype *subsys)
{
    char property_list[SUBSYS_PROPERTIES_LIST_SIZE];

    printf("\n%20s%10s%-3d.%-03d.%-03d%    ",
	   subsys->namestring,
	   subsys_get_class_string(subsys->class),
	   subsys->subsys_majversion,
	   subsys->subsys_minversion,
	   subsys->subsys_editversion);

    /*
     * Check for a "required" property for this subsystem
     */
    subsys_get_property_list(subsys, subsys_property_req, property_list);
    printf("%s", property_list);

    if (subsys_debug) {
	subsys_get_property_list(subsys,
				 subsys_property_seq,
				 property_list);

	printf("\n%-9d  %-08x%               [%s]",
	       subsys->ID,
	       subsys->class,
	       property_list);
    }
}

/*
 * subsys_match
 * Help routine to match subsystem names with wildcarding
 */

static boolean subsys_match (subsystype *subsys, char *prefix,
			     boolean wildcard)
{
    if (null(prefix) ||
	(wildcard ?
	 partialmatch(subsys->namestring,prefix) :
	 !strcasecmp(subsys->namestring,prefix))) 
	return(TRUE);
    else
	return(FALSE);
}

/*
 * subsys_show_name
 * Display the sorted list of subsyss
 */

void subsys_show_name (parseinfo *csb)
{
    list_header  *list;
    list_element *element;
    subsystype   *subsys;
    char         *prefix = GETOBJ(string,1);
    size_t        len;
    boolean       wildcard = FALSE;
    subsys_class  class;
    
    len = strlen(prefix);
    if (len) {
	lowercase(prefix);             /* Make lower case for compare */
	if (prefix[len-1] == '*') {
	    prefix[len-1] = '\0';      /* Remove asterisk */
	    wildcard = TRUE;
	}
    }

    automore_enable(NULL);

    printf(subsys_show_header);

    for (class = 0; class < SUBSYS_CLASS_MAX; class++) {
	/*
	 * Get a pointer to the start of the required class queue
	 */
	list = &subsyslist[class];

	/*
	 * If there's any subsystems on this list, print them
	 */
	if (!LIST_EMPTY(list)) {
	    FOR_ALL_DATA_IN_LIST(list, element, subsys) {
		if (subsys_match(subsys, prefix, wildcard))
		    subsys_show_item(subsys);
	    }
	}
    }

    automore_disable();
}

/*
 * subsys_show_class
 * Display all the subsystems for a given class
 */

void subsys_show_class (parseinfo *csb)
{
    list_header  *list;
    list_element *element;
    subsystype   *subsys;
    subsys_class  class = GETOBJ(int,1);
    
    automore_enable(NULL);

    printf(subsys_show_header);

    /*
     * Get a pointer to the start of the required class queue
     */
    list = &subsyslist[class];
    
    /*
     * If there's any subsystems on this list, print them
     */
    if (!LIST_EMPTY(list)) {
	FOR_ALL_DATA_IN_LIST(list, element, subsys) {
	    subsys_show_item(subsys);
	}
    }

    automore_disable();
}




/* $Id: list_chain.c,v 3.5.10.2 1996/05/21 09:58:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/list_chain.c,v $
 *------------------------------------------------------------------
 * list_chain.c - Parse chains and code for list manager support
 *
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: list_chain.c,v $
 * Revision 3.5.10.2  1996/05/21  09:58:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/03/18  21:30:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:20:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/07  10:18:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  16:40:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/22  14:36:42  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:29:12  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:34:16  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:48:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:56:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include "logger.h"
#include "subsys.h"
#include "registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "list_private.h"


/*
 * Parser chains for "show list"
 */
#define ALTERNATE NONE
#include	"exec_show_list.h"
LINK_POINT(list_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * List parse chain table
 */
static parser_extension_request list_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(list_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * list_ui_init
 *
 * Initialize list parser support.
 *
 */

static void list_ui_init (subsystype *subsys)
{
    parser_add_command_list(list_chain_init_table, "list");
}


/*
 * list_display
 *
 * Entry point for parser exec commands
 */

void list_display (parseinfo *csb)
{
    int listID;

    listID = GETOBJ(int,1);

    /*
     * If we have a listID, the use wants information on a particular
     * list. Otherwise, it's just the summary they're after.
     */
    if (listID) {
	list_display_detail(listID);
    } else {
	list_display_summary();
    }	    
}


/*
 * list_display_summary
 *
 * Print out the list status overview
 */

void list_display_summary (void)
{
    list_element *element;
    list_header  *list;

    /*
     * Turn on the "more" prompting
     */
    automore_enable(NULL);

    /*
     * Print overall listQ status
     */
    printf("\nList Manager:\n");
    printf("     %d lists known, %d lists created\n", listQ.count, list_ID);

    printf("\n   ID   Address  Size/Max   Name\n");

    /*
     * Run down our listQ looking for registered lists
     */
    FOR_ALL_DATA_IN_LIST(&listQ, element, list) {
	printf(" %-4d  %-8x  %-4d/",
	       list->ID, list, list->count);

	/*
	 * If maximum is zero, just print a dash
	 */
	if (list->maximum)
	    printf("%4d", list->maximum);
	else
	    printf("-   ");

	printf("  %s\n", list->name);
    }

    /*
     * Don't forget to turn "more" off
     */
    automore_disable();
}


/*
 * list_display_detail
 *
 * Print out an individual list
 */

void list_display_detail (int listID)
{
    list_element   *element;
    list_header    *list;
    void           *data;
    boolean         found = FALSE;
    char            buffer[LIST_MAX_INFO];
    uint            count = 0;

    /*
     * Check to see if we have a list in the listQ for this ID
     */
    FOR_ALL_DATA_IN_LIST(&listQ, element, list) {
	if (list->ID == listID) {
	    found = TRUE;
	    break;
	}
    }

    /*
     * Doesn't look like it...
     */
    if (!found) {
	printf("\n\n  No list found matching ID %d", listID);
	return;
    }

    automore_enable(NULL);

    /*
     * Print up the header for our newly found list
     */
    printf("\n");
    printf("list ID is %d, size/max is %d/",
	   list->ID, list->count);
    if (list->maximum)
	printf("%d\n", list->maximum);
    else
	printf("-\n");

    /*
     * Call the info handler to supply a name. Note that we don't
     * supply an element - it's not required
     */
    printf("list name is %s\n", list->name);

    printf("enqueue is 0x%x, dequeue is 0x%x, requeue is 0x%x\n",
	   list->action->enqueue, list->action->dequeue,
	   list->action->requeue);
    printf("insert is 0x%x, remove is 0x%x, info is 0x%x\n",
	   list->action->insert, list->action->remove, list->info);
    printf("head is 0x%x, tail is 0x%x, flags is 0x%x\n",
	   list->head, list->tail, list->flags);

    printf("\n    #   Element      Prev      Next      Data  Info\n");

    /*
     * Run down the list and spew out information on it's contents
     */
    FOR_ALL_DATA_IN_LIST(list, element, data) {
	printf(" %-4d  %-8x  %-8x  %-8x  %-8x",
	       count, element, element->prev, element->next, data);

	/*
	 * Call the info handler to supply information on the element
	 * specified, if any is forthcoming (this is optional). Note
	 * that the list is required here.
	 */
	if (list_info(list, element, buffer))
	    printf("  %s", buffer);
	printf("\n");
	count++;
    }
    automore_disable();
}

/*
 * list_ui subsystem header
 */
 
#define LIST_UI_MAJVERSION    1
#define LIST_UI_MINVERSION    0
#define LIST_UI_EDITVERSION   1
 
SUBSYS_HEADER(list_ui, LIST_UI_MAJVERSION, LIST_UI_MINVERSION,
              LIST_UI_EDITVERSION,
              list_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);


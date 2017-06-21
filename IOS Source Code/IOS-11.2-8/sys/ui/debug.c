/* $Id: debug.c,v 3.3.18.4 1996/07/25 09:56:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ui/debug.c,v $
 *------------------------------------------------------------------
 * debug.c -- debug/undebug command support
 *
 * September 1987, Kirk Lougheed
 * Completely reimplemented May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.c,v $
 * Revision 3.3.18.4  1996/07/25  09:56:11  snyder
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
 * Revision 3.3.18.3  1996/07/23  18:32:49  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.3.18.2  1996/06/05  21:47:14  sdurham
 * CSCdi59502:  Need warning about affects of debug all on network
 * performance
 * Branch: California_branch
 *
 * Revision 3.3.18.1  1996/03/18  22:28:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  01:36:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  21:25:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/07  16:16:42  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.2  1995/11/17  17:47:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "sys_registry.h"
#include "debug.h"
#include "../if/network_debug.h"
#include "../os/os_debug.h"
#include "../parser/parser_debug.h"

/*
 * debug_all_flag is set whenever "debug all" is in effect. Should be
 * static, but I believe that screws up run-from-ROM in Cisco images...
 * anyway, external users should use debug_all_p() to interrogate this
 * flag.
 */

static boolean debug_all_flag;

/*
 * debug_list_command
 * Allow attach idb and access list to the debug command
 * following debug list command.
 */
static queuetype *debug_listQ;
static debug_list_t *debug_current_list;

void debug_list_command (parseinfo *csb)
{

    if (debug_current_list != NULL) {
	free(debug_current_list);
	debug_current_list = NULL;
    }
    if ((GETOBJ(idb,1) == NULL) &&
	 (GETOBJ(int, 1) ==  0)) {
	return;
    }
    debug_current_list = malloc(sizeof(debug_list_t));
    if (debug_current_list == NULL) {
	bad_parser_subcommand(csb, csb->which);
	return;
    }
    debug_current_list->idb    = GETOBJ(idb, 1);
    debug_current_list->access_list = GETOBJ(int, 1);
    if (debug_listQ == NULL) {
	debug_listQ = malloc(sizeof(queuetype));
	if (debug_listQ) {
	    queue_init(debug_listQ, 0);
	}
	else {
	    free(debug_current_list);
	    debug_current_list = NULL;
	    bad_parser_subcommand(csb, csb->which);
	    return;
	}
    }
}


/*
 * debug_listq_find 
 */
debug_list_t *debug_listq_find (boolean *pflag)
{
    debug_list_t *p;
    
    if (debug_listQ == NULL) {
	return(NULL);
    }
    p = (debug_list_t *)debug_listQ->qhead;	/* first member */
    while (p) {
	if (p->pflag == pflag) 			/* match? */
	    return(p);		/* yes, return TRUE */
	else
	    p = p->next;	/* no, cdr the list */
    }
    return(NULL);	
}


/*
 * debug_show_listq 
 */
static void debug_show_listq (boolean *pflag, boolean always, char *text) 
{
   debug_list_t *p;
   char *idbstring = NULL;
   idbtype *idb;

   if (always || (!*pflag != !debug_all_flag)) {
       p = debug_listq_find(pflag);
       if (p == NULL) {
	   return;
       }
   } else {
       return;
   }
   idb = p->idb;
   if (idb != NULL) {
       if (validmem(idb) && validmem(idb->namestring)) { 
	   idbstring = idb->namestring;
       } else {
	   idbstring = "corrupt";
       }
   }
   printf("\n\tfor ");
   if (idbstring) {
       printf ("interface: %10s  ", idbstring);
   }
   if (p->access_list) {
       if (idbstring) {
	   printf(" and ");
       }
       printf ("access list: %5d", p->access_list);
   } 
   printf("\n");
}


/*
 * debug_listq_flush
 */
static void debug_listq_flush (void)
{
    void *p;

    if (debug_listQ == NULL) {
	return;
    }
    while ((p = p_dequeue(debug_listQ)) != NULL) {
	free(p);
    }
}

/*
 * debug_listq_unqueue
 */
static void debug_listq_unqueue (boolean *pflag)
{
    void *p;
    p = debug_listq_find(pflag);
    if (p) {
	p_unqueue(debug_listQ, p);
	free(p);
    }
}

/*
 * debug_listq_enqueue
 */
static void debug_listq_enqueue (boolean *pflag)
{
    if (debug_current_list == NULL) {
	return;
    }
    if (debug_listQ == NULL) {
	return;
    }
    debug_current_list->pflag = pflag;
    p_enqueue(debug_listQ, debug_current_list);
    debug_current_list = NULL;
}


/*
 * debug_init
 * Initialize the debugging system
 */

void debug_init (void)
{
    debug_all_flag = FALSE;
    os_debug_init();		/* Debugging for OS primitives */
    network_debug_init();	/* Debugging for generic interfaces */
    debug_parser_init();        /* parser debug commands */
}

/*
 * debug_all_p
 * Tells the caller whether "debug all" is in effect. Used (at least) at
 * subsystem initialization time to determine whether a subsystem should
 * turn on all its debug flags.
 */

boolean debug_all_p (void)
{
    return(debug_all_flag);
}

/*
 * debug_command
 * Generic handler for all manner of "debug" and "undebug" commands.
 * Any subsystem can use this routine to handle its debug flags as long
 * as it uses an array of debug_item_type to describe them and as long
 * as it sets up its parse chains correctly.
 *
 * csb->sense = TRUE for "debug", FALSE for "undebug".
 * csb->which = index into flag definition array.
 * GETOBJ(pdb,1) = mempointer to beginning of array... they really should
 *                 have called it "ptr1"...
 */

void debug_command (parseinfo *csb)
{
    int i;
    leveltype level;
    const debug_item_type *debugarr = GETOBJ(pdb,1);

    /*
     * Sanity check the index -- paranoia is truly a terrible scourge in
     * these troubled times.
     */
    for (i=0; i < csb->which; i++) {
	if (debugarr[i].var == NULL) {
	    bad_parser_subcommand(csb, csb->which);
	    return;
	}
    }
    /* Set up the flag */
    level = raise_interrupt_level(ALL_DISABLE);
     debug_listq_unqueue(debugarr[csb->which].var);
    *(debugarr[csb->which].var) = csb->sense;
    if (csb->sense) {
	debug_listq_enqueue(debugarr[csb->which].var);
    }
    reset_interrupt_level(level);
    if (debug_current_list != NULL) {
	free(debug_current_list);
	debug_current_list = NULL;
    }
    /* Confirm the change to the user */
    debug_show_flag(*(debugarr[csb->which].var), TRUE,
		    debugarr[csb->which].txt);
    debug_show_listq((debugarr[csb->which].var), TRUE,
		    debugarr[csb->which].txt);
}

/*
 * debug_all_cmd
 * Handler for "debug all" and "undebug all". csb->sense is TRUE for "debug",
 * FALSE for "undebug"
 */

void debug_all_cmd (parseinfo *csb)
{
    /* If we're about to start spewing huge amounts of debugging crap
     * that might make the system unusable, make sure the user really
     * wants it to happen...
     */
    if ((csb->sense) && (!confirm("\nThis may severely impact network performance. Continue? "))) return;

    /* Set up the global flag */
    debug_listq_flush();
    debug_all_flag = csb->sense;
    /* Declare an event so the subsystems can set their local flags */
    reg_invoke_debug_all(debug_all_flag);
    /* Confirm to the user */
    printf("\nAll possible debugging has been turned %s",
	   debug_all_flag ? "on" : "off");
}

/*
 * show_debug
 * Show the state of debugging everywhere in the system.
 */

void show_debug (void)
{
    if (debug_all_p()) {
	printf ("\n\"debug all\" is in effect.\n");
    }
    automore_enable(NULL);
    reg_invoke_debug_show();
    automore_disable();
}

/*
 * debug_show_flag
 * Show the value of a single debugging flag in a standardized format.
 * Used by subsystems that need to display individual flags, and when
 * a flag is set or reset. The "flag" argument is the boolean value of
 * the flag to display. "always" is set if the flag should be shown even
 * if its value doesn't match that of debug_all_flag. "text" is a
 * description of the debugging the flag controls. 
 */

void debug_show_flag (boolean flag, boolean always, char *text)
{
    if (always || (!flag != !debug_all_flag)) {
	printf ("\n%s debugging is %s", text, flag ? "on" : "off");

    }
}

/*
 * debug_show_flags
 * Show a whole array of flags. Called by subsystems do do their parts
 * of "show debug". The "heading" argument contains a string that describes
 * the category into which all the flags to be displayed fall. It will
 * be displayed if and only if one or more of the flags is true. If the
 * heading is non-null, all the flags under it will be indented.
 */

void debug_show_flags (const debug_item_type *array, char *heading)
{
    int i;
    boolean hasheading = FALSE;
    boolean hdisplayed = FALSE;

    if ((heading != NULL) && (*heading != '\0')) hasheading = TRUE;
    for (i = 0; array[i].var != (boolean *) NULL; i++) {
	if (!(*(array[i].var)) != !debug_all_flag) {
	    if (hasheading && !hdisplayed) {
		    printf("\n%s:", heading);
		    hdisplayed = TRUE;
		}
	    printf("\n%s%s debugging is %s",
                      hasheading ? "  " : "",
		    array[i].txt, (*array[i].var) ? "on" : "off");
	    debug_show_listq(array[i].var, TRUE, array[i].txt);
	}
    }
}

/*
 * generic_debug_all
 *
 * a generic routine that <routine_name>_debug_all can call
 * whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the debug_items flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void generic_debug_all (const debug_item_type *debug_items, boolean flag)
{
    while (debug_items->var) {
        *(debug_items->var) = flag;
        debug_items++;
    }
}


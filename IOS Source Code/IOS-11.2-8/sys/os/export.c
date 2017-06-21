/* $Id: export.c,v 3.3.62.1 1996/08/28 13:03:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/export.c,v $
 *------------------------------------------------------------------
 * export.c - Symbol export/import facility
 *
 * December 22, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This module contains the export/import facility. It is used for
 * the communication of symbol tables between the core module and
 * load modules.
 *------------------------------------------------------------------
 * $Log: export.c,v $
 * Revision 3.3.62.1  1996/08/28  13:03:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  18:47:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/export.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * Local storage
 */

export_list *exportQ;

/*
 * export_list_init - Initialize an export list
 *
 * This routine is used to initialize an export list with the system. It
 * is called automatically by the generated code invoked when calling
 * the init routine for a specific export list.
 *
 */

void export_list_init (export_list *list)
{
    export_list *temp;

    /*
     * Ensure that there is not already an export list of this name
     */

    temp = export_find_list(list->name); /* Look for a name match */
    if (temp) {
	errmsg(&msgsym(DUPEXPORT, SYS), list->name);
	return;
    }

    /*
     * Link this into the current list
     */

    list->next = exportQ;	/* Save previous head pointer */
    exportQ = list;		/* Make this the head of the list */
}

/*
 * export_find_list - Find an existing export list
 *
 * This routine is used to return the export header for an exported module.
 */

export_list *export_find_list (char *name)
{
    export_list *list;

    for (list = exportQ; list; list = list->next) {
	if (strcmp(name, list->name) == 0)
	  return(list);
    }
    return(NULL);
}

/*
 * export_import - Handle an import request from a load module
 *
 * This routine is used to handle an import request from a load module.
 * We will look up the specified export list, ensure that there is
 * compatibility, and return the pointer to the list itself.
 *
 * If the request is not compatible with the export list, we will print
 * out an error and signal failure to our caller. This will cause the
 * load module to signal load failure back to the core.
 */

void *export_import (
    char *name,			/* Name of facility to import */
    int len)			/* Expected vector length */
{
    export_list *list;

    list = export_find_list(name); /* Look for the named export list */
    if (!list) {
	printf("\n%% Export vector %s not found", name);
	return(NULL);
    }

    if (list->nitems < len) {
	printf("\n%% Importing %s - export vector length is %x expecting %x",
	       name, list->nitems, len);
	return(NULL);
    }

    return(list->vector);	/* Return the vector pointer */
}


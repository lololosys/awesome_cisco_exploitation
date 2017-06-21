/* $Id: tn3270show.c,v 3.2.62.3 1996/05/13 07:18:28 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/tn3270show.c,v $
 *------------------------------------------------------------------
 * TN3270SHOW - show commands associated with tn3270
 * 
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tn3270show.c,v $
 * Revision 3.2.62.3  1996/05/13  07:18:28  bjthomas
 * CSCdi55624:  show keymap <specific> fails to find aliased names
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/04/24  00:15:46  bjthomas
 * CSCdi06846:  show ttycap and show keymap do not respect term length
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:19:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:54:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:22:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:51:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "packet.h"
#include "connect.h"
#include <ciscolib.h>

#include "tn3270.h"			/* Defines CISCO */
#include "vt_interface.h"
#include "tn3270_private.h"

#include "config.h"
#include "../termcap/termcap_public.h"

static void lock_print_3270def (tty_3270def *km)
{
    mem_lock(km);
    printf("%s    ", km->def_name);
    printf("%s\n", km->def);
    free(km);
}

static tty_3270def *pick_3270def (tty_3270def *list, int index)
{
    tty_3270def *t;

    for (t = list; t ; t = (tty_3270def *)t->next_def) {
	if (index-- == 0)
	    break;
    } 
    return(t);
}

void show_all_ttycap (void)
{
    char *name;
    tty_3270def *t;
    int index;

    if (!(t = termcap_list))
        printf("No ttycaps in the configuration\n");

    for (index = 0; (t = pick_3270def(termcap_list, index)); index++) {
	mem_lock(t);
        name = t->def;
        printf("%s    ", t->def_name);
        while (*name && (*name != ':')) {
            printf("%c", *name);
            name++;
        }
        printf("\n");
	free(t);
    }
}

void show_ttycap_specific (char *ttycapname)
{
    tty_3270def *t;

    t = termcap_list;
    while (t) {
        if ((strcmp(ttycapname, t->def_name) == 0) || 
	    tnamatch(ttycapname, t->def, FALSE)) {
	    lock_print_3270def(t);
            return;
        }
    t = (tty_3270def *)t->next_def;
    }
    printf("TTYCAP %s not found in configuration\n", ttycapname);
}

void show_ttycap_in_use (void)
{
    tty_3270def *t;

    if (stdio->termtype) {
        t = termcap_list;
        while (t) {
            if ((strcmp(stdio->termtype, t->def_name) == 0) || 
                 tnamatch(stdio->termtype, t->def, FALSE)) {
		 lock_print_3270def(t);
                 return;
            }
            t = (tty_3270def *)t->next_def;
        }
    }
    t = termcap_list;
    while (t) {
         if ((strcmp("default", t->def_name) == 0) ||
              tnamatch("default", t->def, FALSE)) {
	      lock_print_3270def(t);
              return;
         }
         t = (tty_3270def *)t->next_def;
    }
    printf("%s\n", default_termcap);
}

void show_ttycap (parseinfo *csb)
{

    automore_enable(NULL);
    if (GETOBJ(int,1)) {
	show_all_ttycap();
    } else {
	if (*GETOBJ(string,1)) {
	    show_ttycap_specific(GETOBJ(string,1));
	} else {
	    show_ttycap_in_use();
	}
    }
    automore_disable();
}

void show_all_keymaps (void)
{
    char *name;
    tty_3270def *t;
    int index;

    if (!(t = keymap_list))
        printf("No keymaps in the configuration\n");
    
    for (index = 0; (t = pick_3270def(keymap_list, index)); index++) {
	mem_lock(t);
        name = t->def;
        printf("%s    ", t->def_name);
        while (*name && (*name != '{')) {
            printf("%c", *name);
            name++;
        }
        printf("\n");
	free(t);
    }
}

void show_keymap_specific (char *keymapname)
{
    tty_3270def *km;

    km = keymap_list;
    while (km) {
        if (strcmp(keymapname, km->def_name) == 0) {
	     lock_print_3270def(km);
             return;
        }
	if (tnamatch(keymapname, km->def, TRUE)) {
	    lock_print_3270def(km);
	    return;
	}
        if (km)
            km = km->next_def;
    }
    printf("KEYMAP %s not found in configuration\n", keymapname);
}

void show_keymap_in_use (void)
{
    char *name;
    tty_3270def *km;
    char **kg;

    name = stdio->keymaptype ? stdio->keymaptype : stdio->termtype;
    if (name) {
        km = keymap_list;
        while (km) {
            if (strcmp(name, km->def_name) == 0) {
		lock_print_3270def(km);
                return;
            }
	    if (tnamatch(name, km->def, TRUE)) {
		lock_print_3270def(km);
		return;
	    }
            if (km)
                km = km->next_def;
        } 
    }
    km = keymap_list;
    while (km) {
        if (strcmp("default", km->def_name) == 0) {
	     lock_print_3270def(km);
             return;
        }
	if (tnamatch("default", km->def, TRUE)) {
	    lock_print_3270def(km);
	    return;
	}
        if (km)
            km = km->next_def;
    }
    kg = keysgeneric;
    while (*kg) {
        printf("%s\n", *kg);
        kg++;
    }
}

void show_keymap (parseinfo *csb)
{
    automore_enable(NULL);
    if (GETOBJ(int,1)) {
	show_all_keymaps();
    } else {
	if (*GETOBJ(string,1)) {
	    show_keymap_specific(GETOBJ(string,1));
	} else {
	    show_keymap_in_use();
	}
    }
    automore_disable();
}

/* $Id: tn3270config.c,v 3.3.20.1 1996/06/28 23:30:50 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/tn3270config.c,v $
 *------------------------------------------------------------------
 * TN3270CONFIG -- Tn3270 specific configuration support
 * 
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tn3270config.c,v $
 * Revision 3.3.20.1  1996/06/28  23:30:50  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/25  11:06:03  irfan
 * CSCdi47499:  memory leaks in tn3270
 *
 * Revision 3.2  1995/11/17  18:51:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "connect.h"
#include "ttysrv.h"
#include "config.h"
#include "../h/parser.h"

#include "tn3270.h"			/* Defines CISCO */
#include "vt_interface.h"
#include "tn3270_private.h"


/*
 * keymaptype_command
 * Set our terminal keymap type
 */

void keymaptype_command (parseinfo *csb)
{
    if (csb->nvgen) {
	tt_soc *tty;

	tty = MODEMS[csb->line1];
	nv_write((boolean)tty->def_keymaptype, "%s %s", csb->nv_command,
		 tty->def_keymaptype);
	return;
    }

    if (csb->permanent) {
	if (csb->sense) {
	    settoken(csb, GETOBJ(string,1), &console->def_keymaptype);
	} else {
	    settoken(csb, NULL, &console->def_keymaptype);
	}
    }
    if (csb->sense) {
	settoken(csb, GETOBJ(string,1), &console->keymaptype);
    } else {
	settoken(csb, NULL, &console->keymaptype);
    }
}

void nv_3270save (list, s)
    tty_3270def *list;
    char *s;
{
    register tty_3270def *t;
    
    t = list;
    while (t) {
	nv_write((boolean)t->def, "%s %s \\\n%s", s, t->def_name, t->def);
	t = (tty_3270def *)t->next_def;
    }
}

static void dumpit (parseinfo *csb, tty_3270def **list)
{
    tty_3270def *buffertofree, *parent;

    if (*list == NULL)
        return;
    parent = *list;
    /* special case of deleting first entry */
    if (strcmp(parent->def_name, GETOBJ(string,1)) == 0) {
        *list = (*list)->next_def;
        free(parent);
        return;
    }
    while (parent->next_def) {
        if ((strcmp(parent->next_def->def_name, GETOBJ(string,1)))==0) {
            buffertofree = parent->next_def;
            parent->next_def = buffertofree->next_def;
            free(buffertofree);
            return;
        }
        parent = parent->next_def;
    }
    printf ("%s not found\n", GETOBJ(string,1));
}

static tty_3270def *get3270def(parseinfo *);

void map3270keys_command (parseinfo *csb)
{
    extern tty_3270def *keymap_list;
    tty_3270def *keymap_buffer, *parent, *buffertofree;

    if (csb->nvgen) {
	nv_3270save(keymap_list, csb->nv_command);
	return;
    }

    /* eliminate "no" case */
    if (csb->sense == FALSE) {
        dumpit(csb, &keymap_list);
        return;
    }

    /* get the keymap name and text */
    keymap_buffer = get3270def(csb);
    if (keymap_buffer) {
        (tty_3270def *)keymap_buffer->next_def = keymap_list;
        keymap_list = keymap_buffer;

	/* was this definition a duplicate of a previous definitions ?*/
	/* prepare to eliminate duplications */
        parent = keymap_buffer;
        /* find end of names */
        while (parent->next_def) {
             if (strcmp(parent->next_def->def_name,
                        keymap_buffer->def_name) == 0) {
                 buffertofree = parent->next_def;
                 parent->next_def = buffertofree->next_def;
                 free(buffertofree);
                 return;
             }
             parent = parent->next_def;
        }
    }
}

void termcap_command (parseinfo *csb)
{
    extern tty_3270def *termcap_list;
    tty_3270def *termcap_buffer, *buffertofree, *parent;

    if (csb->nvgen) {
	nv_3270save(termcap_list, csb->nv_command);
	return;
    }

    if (csb->sense == FALSE) {
        dumpit(csb, &termcap_list);
        return;
    }
    termcap_buffer = get3270def(csb);
    if (termcap_buffer) {
        (tty_3270def *)termcap_buffer->next_def = termcap_list;
        termcap_list = termcap_buffer;
        /* eliminate duplications */
        parent = termcap_buffer;
        /* find end of names in new termcap */
        while (parent->next_def) {
            if ((strcmp(parent->next_def->def_name,
                        termcap_buffer->def_name))==0) {
                 buffertofree = parent->next_def;
                 parent->next_def = buffertofree->next_def;
                 free(buffertofree);
                 return;
             }
             parent = parent->next_def;
        }
    }
}

static tty_3270def *get3270def (parseinfo *csb)
{
    char *start, *temp;
    int count, i;
    char lastchar;
    tty_3270def *buffer;
    char foundlastcr;
    char *buff;
    char *tmpbuff;
    boolean first;

    foundlastcr = FALSE;

    if ((tmpbuff = malloc(SABUFSIZE)) == NULL) {
	printf(nomemory);
	return(NULL);
    }
    buff = GETOBJ(string,2);
    if ((*buff == '\\') && (*(buff+1) == '\0')) {
	buff++;
    }
    strcpy(tmpbuff, buff);
    buff = tmpbuff;
    start = tmpbuff;
    temp = tmpbuff;
    first = TRUE;
    csb->flags &= ~CONFIG_DONE;
    while (!foundlastcr) {
	if (*buff == '\0') {
	    /*
	     * If we've parsed at least one character and it wasn't
	     * a '\\' then we have a match
	     */
	    if ((buff > tmpbuff) && (*(buff - 1) != '\\')) {
		foundlastcr = TRUE;
		continue;
	    } else {
		/*
		 *  If we've parsed at least character, then
		 *  put a newline in the buffer
		 */
		temp = buff;
		if (buff > tmpbuff) {
		    *(temp++) = '\n';
		}
		if (csb->flags & CONFIG_TERM) {
		    if (first && !csb->nvgen) {
			printf("\nEnter termcap; end with CTRL/Z\n");
			first = FALSE;
		    }
		    i = texti(temp, &start[SABUFSIZE]-(temp+100),
			      "\r\n", &lastchar);
		} else {
		    i = batch_input(csb, temp, &start[SABUFSIZE]-(temp+100),
				    "\r\n", &lastchar);
		}
		if (i == FALSE) {
		    csb->flags |= CONFIG_ABORT;
		    free(tmpbuff);
		    return(NULL);
		} else {
		    if (lastchar == '\032') {
			csb->flags |= CONFIG_DONE;
			foundlastcr = TRUE;
		    } else {
			if (*buff != '\0') {
			    continue;
			}
		    }
		}
	    }
	}
	buff++;
    }
    count = 1 + (int)(buff - start);
    buffer = malloc(sizeof(tty_3270def) + count);
    if (!buffer) {
	printf(nomemory);
	free(tmpbuff);
	return(NULL);
    }
    for (i = 0; i <= MAX3270DEFNAME; i++) {
	buffer->def_name[i] = GETOBJ(string,1)[i];
    }
    temp = buffer->def;
    buff = tmpbuff;
    for (i = 1; i != count; i++) {
	*temp++ = *start++;
    }
    *temp = '\0';
    free(tmpbuff);
    return(buffer);
}

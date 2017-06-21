/* $Id: x25_actions.c,v 3.4.46.3 1996/08/28 13:23:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_actions.c,v $
 *------------------------------------------------------------------
 * X25 specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: x25_actions.c,v $
 * Revision 3.4.46.3  1996/08/28  13:23:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.46.2  1996/05/21  10:10:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.46.1  1996/03/18  22:50:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  11:19:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  21:55:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/17  18:41:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/14  13:08:08  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:07:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:20:13  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:21:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <ciscolib.h>
#include "config.h"
#include "logger.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_actions.h"
#include "../x25/x25.h"
#include "../x25/x25_actions.h"

#include "name.h"

/*
 * Forward declarations
 */
static boolean match_digits(char *, int, int *);

char *x121_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    if (flag & ADDR_NAME_LOOKUP) {
	return("Hostname or X.121 Addr");
    } else {
	return("X.121 Addr");
    }
}

char *x121_hwaddr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("X.121 Addr");
}

/*
 * X121 ADDRESS: X121_MAX_DIGITS digits, as a string.
 */

boolean match_x121_addr (parseinfo *csb, char *buf, int *pi,
			 void *addr_pointer, int in_help, uint flag,
			 idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    int old_pi = *pi;
    int len;

    if (match_digits(buf, X121_MAX_DIGITS, pi)) {

        len = *pi - old_pi;
        addrp->type = ADDR_X121;
        addrp->length = len;
        bcopy(buf, addrp->x121_addr, len);
        addrp->x121_addr[len] = '\0';
        return(TRUE);

    } else {
        if (flag & ADDR_NAME_LOOKUP) {
            char *name;
            nametype *ptr;
            boolean dummy = FALSE;

            name = malloc(PARSEBUF);
            if (name == NULL) {
                return(FALSE);
            }
            len = copy_varstring(buf, name, PARSEBUF, STRING_QUOTED);
            if (in_help) {
                *pi += len;
                free(name);
                return(TRUE);
            }
            ptr = (nametype *) name_lookup(name, &dummy, NAM_PAD);
            if (ptr) {
                *pi += len;
                addrp->type = ptr->v.x121.adr[0].type;
                addrp->length = ptr->v.x121.adr[0].length;
                strcpy((char *)addrp->x121_addr,
                       (char *)ptr->v.x121.adr[0].x121_addr);
                free(name);
                return(TRUE);
            } else {
                printf("\n%% Bad x25 host address '%s'", name);
            }
            free(name);
        }
    }
    return(FALSE);
}

/*
 * X121 ADDRESS: X121_MAX_DIGITS digits, as a string.
 */

boolean match_x121_hwaddr (parseinfo *csb, char *buf, int *pi,
			   void *addr_pointer, int in_help, uint flag,
			   idbtype *idb)
{
    hwaddrtype *addrp = (hwaddrtype *)addr_pointer;
    int old_pi = *pi;
    int len;

    if (match_digits(buf, X121_MAX_DIGITS, pi)) {

        len = *pi - old_pi;
        addrp->type = STATION_X121;
        addrp->length = len;
        addrp->target_type = TARGET_UNICAST;
        bcopy(buf, addrp->addr, len);
        addrp->addr[len] = '\0';

        return(TRUE);
    }
    return(FALSE);
}


/* Check that the next <num> characters of buf are digits */
static boolean match_digits (char *buf, int num, int *pi)
{
    int i;

    if (num <= 0) {
        return(FALSE);
    }

    buf += *pi;
    i = 0;
    while ((i < num) && isdigit(buf[i])) {
        i++;
    }
    /* pi needs to reflect # of good chars */
    (*pi) += i;
    if (i == 0) {
        return(FALSE);
    }
    return(TRUE);
}

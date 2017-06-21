/* $Id: parser_util.c,v 3.5.4.3 1996/08/28 13:05:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_util.c,v $
 *------------------------------------------------------------------
 * P A R S E R _ U T I L . C
 *
 * Utility functions for the new router parser.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_util.c,v $
 * Revision 3.5.4.3  1996/08/28  13:05:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.4.2  1996/06/03  23:24:26  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.5.4.1  1996/03/18  21:33:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  07:22:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/07  10:40:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  16:49:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/09  22:57:12  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.4  1996/02/07  21:15:29  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.3  1995/11/28  20:26:41  widmer
 * CSCdi44264:  Change save_line() to use variable-length arguments
 * Remove dead code also.
 *
 * Revision 3.2  1995/11/17  18:53:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:16:00  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:32:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "msg_parser.c"			/* Not a typo, see logger.h */
#include "ttysrv.h"

static inline char netascii2cisco (char **buf)
{
    char ch;

    if ((**buf == '\r') &&
	(*(*buf+1) == '\n')) {
	*buf += 2;
	return('\n');
    }
    if ((**buf == '\r') &&
	(*(*buf+1) == '\0')) {
	*buf += 2;
	return('\r');
    }
    ch = **buf;
    (*buf)++;
    return(ch);
}

/*
 * BATCH_INPUT
 *
 * If there is an active batch input buffer which
 * contains more data, read a line from it and return TRUE.
 *
 * This function must operate much the same way as readline so
 * that the parser does not know the difference.
 */
boolean batch_input (parseinfo *csb, char *buf, int buflen,
		     char *breakset, char *lastcharp)
{
    char *cp = buf;

    *lastcharp = '\0';
    
    if (csb->batch_buf && *csb->batch_offset) {
        /* Copy a line, stopping at any breakset character, NULL, or a full buffer */
        while (*csb->batch_offset
          && ! strchr(breakset, *csb->batch_offset)
          && ((cp - buf) < (buflen - 1))) {
	    *(cp++) = netascii2cisco(&csb->batch_offset);
	}

	*lastcharp = netascii2cisco(&csb->batch_offset);
        *cp = '\0';
        return(TRUE);
    }
    return(FALSE);
}

/*
 * SAVE_LINE
 *
 * Save lines into a block buffer.  Used for comments, which are output at the
 * begining of the NV generation; generated command lines, which are output
 * after the comments; and help, which is output after a line is parsed.
 * For generation, make sure we have enough space to save both comments and
 * configuration.  If there isn't enough space in NV ram, truncate comments.
 *
 * Unlike an earlier version of save_line, newline characters aren't added
 * automatically - call save_line a second time if needed (since lines are
 * added sequentially into the buffer).  Tabs are used to delimit fields;
 * this feature is used only by the help strings.
 */

void save_line_init (save_buf *bp)
{
    if (bp->buf) {
        free(bp->buf);
    }
    bp->buf = (char *)0;
    bp->size = 0;
    bp->used = 0;
}

void save_line (parseinfo *csb, save_buf *bp, const char *fmt, ...)
{
    va_list args;
    int n;
    int newsize;
    
    if (!fmt) {
        return;
    }

    va_start(args, fmt);
    n = _doprnt(NULL, NULLTTY, fmt, args, FALSE);

    /* Add 1, for the null termination */
    while ((bp->used + n + 1) > bp->size) {
	char *mybuf;

	newsize = bp->size + SABUFSIZE;
	mybuf = malloc(newsize);
        if (mybuf) {
	    bp->size = newsize;
	    if (bp->buf == NULL) {
		*mybuf = '\0';
	    } else {
		strcpy(mybuf, bp->buf);
		free(bp->buf);
	    }
	} else {
	    va_end(args);
            return;
	}
        bp->buf = mybuf;
    }
    n = _doprnt(bp->buf + bp->used, NULL, fmt, args, FALSE);
    bp->used += n;
    va_end(args);
}

/*
 * lookup_id_string
 * Lookup a named item from an array of id_string items.
 */
char *lookup_id_string (id_string const idarray[], ulong id)
{
    id_string const *idstr;

    for (idstr = idarray; idstr->str; idstr++) {
	if (idstr->id == id)
	    return(idstr->str);
    }
    return(NULL);
}

/*
 * nv_add_id_string
 * Do an nv_add of the string if it's in the table, or the number if it's
 * not.
 */

void nv_add_id_string (id_string const idarray[], ulong id)
{
    id_string const *idstr;

    for (idstr = idarray; idstr->str; idstr++) {
	if (idstr->id == id) {
	    nv_add(TRUE, "%s", idstr->str);
	    return;
	}
    }
    nv_add(TRUE, "%u", id);
}

/*
 * printf_id_string
 * Do a printf of the string if it's in the table, or the number if it's
 * not.
 */

int printf_id_string (id_string const idarray[], ulong id)
{
    id_string const *idstr;

    for (idstr = idarray; idstr->str; idstr++) {
	if (idstr->id == id) {
	    return(printf("%s", idstr->str));
	}
    }
    return(printf("%d", id));
}

/*
 * lookup_id_subid_string
 * Lookup a named item from an array of id_subid_string items.
 */
char *lookup_id_subid_string (id_subid_string const idsubidarray[],
			      ulong id, ulong subid)
{
    id_subid_string const *id_subid_str;

    for (id_subid_str = idsubidarray; id_subid_str->str; id_subid_str++) {
	if ((id_subid_str->id == id) && (id_subid_str->subid == subid))
	    return(id_subid_str->str);
    }
    return(NULL);
}

/*
 * nv_add_id_subid_string
 * Do an nv_add of the string if it's in the table, or the number if it's
 * not.
 */

void nv_add_id_subid_string (id_subid_string const idarray[],
			     ulong id, ulong subid)
{
    id_subid_string const *id_subid_str;

    for (id_subid_str = idarray; id_subid_str->str; id_subid_str++) {
	if ((id_subid_str->id == id) && (id_subid_str->subid == subid)) {
	    nv_add(TRUE, "%s", id_subid_str->str);
	    return;
	}
    }
    nv_add(TRUE, "%d %d", id, subid);
}

/*
 * printf_id_subid_string
 * Do a printf of the string if it's in the table, or the number if it's
 * not.
 */

int printf_id_subid_string (id_subid_string const idarray[],
			    ulong id, ulong subid)
{
    id_subid_string const *id_subid_str;

    for (id_subid_str = idarray; id_subid_str->str; id_subid_str++) {
	if ((id_subid_str->id == id) && (id_subid_str->subid == subid)) {
	    return(printf("%s", id_subid_str->str));
	}
    }
    return(printf("%d %d", id, subid));
}

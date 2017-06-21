/* $Id: parser_privilege.h,v 3.2 1995/12/17 18:35:22 thille Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/parser_privilege.h,v $
 *------------------------------------------------------------------
 * P A R S E R _ P R I V I L E G E . H  (formerly P R I V I L E G E . H)
 *
 * Defines and prototypes for changing privilege level of parser commands
 *
 * November 1994, Robert Widmer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_privilege.h,v $
 * Revision 3.2  1995/12/17  18:35:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/16  07:15:01  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD_LOG as privilege.h:
 * -----------------------
 * Revision 3.2  1995/11/17  18:53:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:32:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct privq_ {
    struct privq_ *next;        	/* Next queue entry */
    trans_priv *priv;			/* Priv pointer */
} privq;

typedef struct priv_nvgenq_ {
    struct priv_nvgenq_ *next;		/* Next queue entry */
    uint level;				/* Priv level */
    char command[PARSEBUF];		/* Current command */
} priv_nvgenq;


extern void priv_push(parseinfo *, trans_priv *);
extern boolean nvgen_privilege (parseinfo *, trans_priv *, const char *);

/* $Id: parser_modes.h,v 3.2 1995/12/17 18:35:21 thille Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/parser_modes.h,v $
 *------------------------------------------------------------------
 * P A R S E R _ M O D E S . H  (formerly M O D E . H)
 *
 * Parser Mode definitions
 *
 * October 1994, Robert Widmer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_modes.h,v $
 * Revision 3.2  1995/12/17  18:35:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/15  03:46:20  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as mode.h:
 * ------------------
 * Revision 3.2  1995/11/17  18:48:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	PARSER_MODE_NONE	0x0001
#define	PARSER_MODE_ALIASES	0x0002
#define	PARSER_MODE_PRIVILEGE	0x0004

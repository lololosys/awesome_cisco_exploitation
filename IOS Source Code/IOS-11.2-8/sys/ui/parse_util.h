/* $Id: parse_util.h,v 3.3.46.1 1996/03/18 22:29:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ui/parse_util.h,v $
 *------------------------------------------------------------------
 * parse_util.h -- externs for parse_util.c
 *
 * October 1991, William Miskovetz
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parse_util.h,v $
 * Revision 3.3.46.1  1996/03/18  22:29:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/14  01:36:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.14.1  1996/02/20  21:26:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/17  18:40:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:48:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__PARSE_UTIL_H__
#define	__PARSE_UTIL_H__

#include "../h/address.h"

extern void print_proto_addr(idbtype *, addrtype *, uint, char * );
extern boolean parse_proto_addr(parseinfo *, uint, addrtype *,
				addrtype *, uint);
extern const char *print_protocol(uint link_type);
extern boolean get_source_file(char *, int, char *, boolean);
extern boolean get_host_address(addrtype *, char *, int);
extern void parser_set_keyword(parseinfo *csb);

#endif	__PARSE_UTIL_H__

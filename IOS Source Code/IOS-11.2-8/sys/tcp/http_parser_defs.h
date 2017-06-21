/* $Id: http_parser_defs.h,v 3.2.4.1 1996/05/06 17:39:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http_parser_defs.h,v $
 *------------------------------------------------------------------
 * HTTP parser definitions
 *
 * February 1996, Robert Widmer
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_parser_defs.h,v $
 * Revision 3.2.4.1  1996/05/06  17:39:59  widmer
 * CSCdi56492:  Need to limit access to http server with access-list
 * Branch: California_branch
 * Add "ip http access-class" command
 *
 * Revision 3.2  1996/03/09  22:57:43  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/12  13:39:19  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {
    HTTP_SERVER,
    HTTP_PORT,
    HTTP_ACCESS,
};

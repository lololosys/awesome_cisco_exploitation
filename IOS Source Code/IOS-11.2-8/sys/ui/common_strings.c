/* $Id: common_strings.c,v 3.2.32.2 1996/07/06 05:57:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/ui/common_strings.c,v $
 *------------------------------------------------------------------
 * C O M M O N _ S T R I N G S . C
 *
 * February, 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: common_strings.c,v $
 * Revision 3.2.32.2  1996/07/06  05:57:37  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.32.1  1996/05/11  16:52:01  thille
 * CSCdi51398:  Duplicate strings in ping / trace
 * Branch: California_branch
 * Every 116 bytes counts.
 *
 * Revision 3.2  1996/02/08  20:13:37  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.1  1996/02/08  00:12:07  thille
 * CVS:
 * ----------------------------------------------------------------------
 *
 * Add placeholder for the shared strings work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "common_strings.h"

/*
 * common words that are complete strings
 */

const char common_str_empty_str[]       = "";
const char common_str_interface[]       = "interface";
const char common_str_unknown[]         = "unknown";
const char common_str_protocol[]        = "protocol";
const char common_str_invalid[]         = "invalid";
const char common_str_unreachable[]     = "unreachable";

/*
 * common strings
 */
const char ping_sending_msg[]       = "\nSending %d, %d-byte %s%s Echoes to ";
const char ping_timeout_msg[]           = ", timeout is %d seconds:";
const char type_escape_seq_abort_msg[]  = "\nType escape sequence to abort.";
const char tracing_the_route_to_msg[]   = "\nTracing the route to ";
const char if_is_line_proto_is_msg[]    = "\n%s is %s, line protocol is %s";
const char subsys_req_iphost[]		= "req: iphost";
const char subsys_seq_iphost[]		= "seq: iphost";

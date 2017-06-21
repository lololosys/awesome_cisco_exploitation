/* $Id: appn_parser.h,v 3.2 1995/11/17 08:37:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/appn/appn_parser.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_parser.h,v $
 * Revision 3.2  1995/11/17  08:37:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:03:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef APPN_PARSER_H_INCLUDED
#define APPN_PARSER_H_INCLUDED

void appn_init (subsystype *subsys);
void appn_show_command(parseinfo *);
void startappn_command(parseinfo *csb);
void dump_appn_commands(parseinfo *csb);
void debug_appn_commands(parseinfo *csb);
void appn_trace_cmd(parseinfo *csb);
void appn_command(parseinfo *csb);
void exit_appn_command (parseinfo *csb);

#endif

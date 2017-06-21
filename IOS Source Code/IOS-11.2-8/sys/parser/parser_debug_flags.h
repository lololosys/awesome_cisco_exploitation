/* $Id: parser_debug_flags.h,v 3.3 1996/03/09 22:57:05 widmer Exp $
 * $Source: /release/111/cvs/Xsys/parser/parser_debug_flags.h,v $
 *------------------------------------------------------------------
 * parser_debug_flags.h -- Debugging flag declarations for parser
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_debug_flags.h,v $
 * Revision 3.3  1996/03/09  22:57:05  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  18:48:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from parser code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(parser_debug_arr)

DEBUG_FLAG(parser_ambig_debug,DEBUG_PARSER_AMBIG,"Parser ambiguity")
DEBUG_FLAG(parser_help_debug,DEBUG_PARSER_HELP,"Help message")
DEBUG_FLAG(parser_mode_debug,DEBUG_PARSER_MODE,"Parser mode")
DEBUG_FLAG(parser_alias_debug,DEBUG_PARSER_ALIAS,"Parser aliases")
DEBUG_FLAG(parser_priv_debug,DEBUG_PARSER_PRIV,"Privilege level")
DEBUG_FLAG(parser_http_debug,DEBUG_PARSER_HTTP,"Parser HTTP processing")

DEBUG_ARRDONE

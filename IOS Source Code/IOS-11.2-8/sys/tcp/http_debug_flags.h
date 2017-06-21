/* $Id: http_debug_flags.h,v 3.2 1996/03/09 22:57:41 widmer Exp $
 * $Source: /release/111/cvs/Xsys/tcp/http_debug_flags.h,v $
 *------------------------------------------------------------------
 * http_debug_flags.h -- Debugging flag declarations for HTTP
 *
 * July 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_debug_flags.h,v $
 * Revision 3.2  1996/03/09  22:57:41  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:56  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from HTTP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(http_debug_arr)

DEBUG_FLAG(http_transaction_debug, DEBUG_HTTP_TRANSACTION, "HTTP transactions")
DEBUG_FLAG(http_token_debug, DEBUG_HTTP_TOKEN, "HTTP tokens")
DEBUG_FLAG(http_ezsetup_debug, DEBUG_HTTP_EZSETUP, "HTTP EZSetup")
DEBUG_FLAG(http_uri_debug, DEBUG_HTTP_URI, "HTTP URL")

DEBUG_ARRDONE

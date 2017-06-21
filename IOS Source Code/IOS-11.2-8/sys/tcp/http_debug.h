/* $Id: http_debug.h,v 3.2 1996/03/09 22:57:40 widmer Exp $
 * $Source: /release/111/cvs/Xsys/tcp/http_debug.h,v $
 *------------------------------------------------------------------
 * http_debug.h -- Debugging declarations for HTTP
 *
 * Jul 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_debug.h,v $
 * Revision 3.2  1996/03/09  22:57:40  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:55  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up HTTP debugging */
extern void http_debug_init(void);

/*
 * The actual debugging flags are defined in http_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../tcp/http_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../tcp/http_debug_flags.h"

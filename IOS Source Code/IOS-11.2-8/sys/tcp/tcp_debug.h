/* $Id: tcp_debug.h,v 3.2 1995/11/17 18:48:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcp/tcp_debug.h,v $
 *------------------------------------------------------------------
 * tcp_debug.h -- Debugging declarations for General TCP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcp_debug.h,v $
 * Revision 3.2  1995/11/17  18:48:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up TCP debugging */
extern void tcp_debug_init(void);

/* Line number to restrict TCP packet debugging */
extern int tcplineno;

/* These are also used to further restrict output */
typedef enum {
    TCP_DIR_DISABLED, TCP_DIR_IN, TCP_DIR_OUT
} tcp_dbg_dir_type;

extern ushort tcp_dbg_port;
addrtype tcp_dbg_addr;
tcp_dbg_dir_type tcp_dbg_dir;

/*
 * The actual debugging flags are defined in tcp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../tcp/tcp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../tcp/tcp_debug_flags.h"

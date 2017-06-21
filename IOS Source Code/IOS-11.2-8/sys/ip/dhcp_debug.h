/* $Id: dhcp_debug.h,v 3.2 1995/11/17 09:32:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/dhcp_debug.h,v $
 *------------------------------------------------------------------
 * dhcp_debug.c - DHCP proxy client debugging support..Header
 *                include file
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dhcp_debug.h,v $
 * Revision 3.2  1995/11/17  09:32:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:56:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
extern void dhcp_debug_command (parseinfo *);
extern void dhcp_debug_all (boolean);
extern void dhcp_debug_show (void);
extern void dhcp_debug_init(void);

extern boolean dhcp_debug;

/*
 * The actual debugging flags are defined in dhcp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ip/dhcp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ip/dhcp_debug_flags.h"

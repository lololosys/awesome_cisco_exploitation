/* $Id: sdlc_debug.h,v 3.2 1995/11/17 09:23:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sdlc_debug.h,v $
 *------------------------------------------------------------------
 * sdlc_debug.h -- Debugging declarations for SDLC and STUN
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc_debug.h,v $
 * Revision 3.2  1995/11/17  09:23:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up SDLC debugging */
extern void sdlc_debug_init(void);

/* Parser function for SDLC debugging commands */
extern void sdlc_debug_command(parseinfo *csb);

/*
 * slack_debug is an int full of bitmapped options... sort of an
 * abberant debugging flag. It's desperately in need of some serious
 * cleanup.
 */
extern int slack_debug;

extern int sdlc_packet_debug;

/*
 * stun_packet_debug is a boolean flag with a bunch of other stuff around it.
 */
extern uchar stun_debug_group;
extern uchar stun_debug_addr[STUN_MAX_ADDR_LENGTH];
boolean stun_debug_addr_significant;

/*
 * DEBUG_SDLC_LACK is the parser code corresponding to slack_debug. We
 * make it negative to keep it out of the space allocated by sdlc_debug_flags.h
 */
#define DEBUG_SDLC_LACK -1

/* DEBUG_STUNPACKET is the parser code corresponding to stun_packet_debug */
#define DEBUG_STUNPACKET -2

/* DEBUG_STUNTG is the parser code corresponding to stun_tg_debug */
#define DEBUG_STUNTG -3

#define DEBUG_SDLC_PACKETS -4
/*
 * The actual debugging flags are defined in sdlc_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ibm/sdlc_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ibm/sdlc_debug_flags.h"

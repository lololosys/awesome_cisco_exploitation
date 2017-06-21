/* $Id: rsvp_debug.h,v 1.1.4.2 1996/08/09 00:18:08 asastry Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_debug.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_debug.h,v $
 * Revision 1.1.4.2  1996/08/09  00:18:08  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:18  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Function to fire up IP multicast debugging.
 */
extern void rsvp_debug_commands (parseinfo *);
extern void rsvp_debug_all (boolean);
extern void rsvp_debug_init (void);
extern void rsvp_debug_show (void);

extern int rsvp_debug;
extern int rsvp_acl;
extern int rsvp_detail_flag;
extern boolean rsvp_special_flag;
extern boolean rsvp_acl_debug(boolean debug_flag, ipaddrtype address);

/*
 * Bit mask used to define options for 'debug ip rsvp'
 */

#define RSVP_RESV_DEBUG_FLAG 0x1
#define RSVP_PATH_DEBUG_FLAG 0x2
#define RSVP_DEBUG_ENABLE 0x3


/*
 * The actual debugging flags are defined in rsvp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../rsvp/rsvp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../rsvp/rsvp_debug_flags.h"

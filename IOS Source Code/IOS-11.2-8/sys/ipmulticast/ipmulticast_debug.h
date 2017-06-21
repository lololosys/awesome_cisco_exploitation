/* $Id: ipmulticast_debug.h,v 3.3 1995/12/03 18:06:29 dino Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipmulticast/ipmulticast_debug.h,v $
 *------------------------------------------------------------------
 * ipmulticast_debug.h - Debugging functions for IP multicast routing.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipmulticast_debug.h,v $
 * Revision 3.3  1995/12/03  18:06:29  dino
 * CSCdi44874:  no ip dvmrp debug detail turns on debugging.
 *
 * Revision 3.2  1995/11/17  17:29:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  07:32:42  asastry
 * CSCdi37082:  Enhance 'debug ip dvmrp detail' to allow '[in | out]'.
 *
 * Revision 2.1  1995/06/07  21:03:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* 
 * Function to fire up IP multicast debugging.
 */
extern void ipmulticast_debug_commands(parseinfo *);
extern void ipmulticast_debug_all(boolean);
extern void ipmulticast_debug_init(void);
extern void ipmulticast_debug_show(void);

extern ipaddrtype mcache_group_debug;
extern ipaddrtype mroute_group_debug;
extern ipaddrtype pim_group_debug;
extern uchar dvmrp_debug_detail_flag;

/*
 * Bit mask used to define options for 'debug ip dvmrp detail'
 */
#define DVMRP_INCOMING_DETAIL_DEBUG 0x1
#define DVMRP_OUTGOING_DETAIL_DEBUG 0x2
#define DVMRP_DETAIL_DEBUG          0x3
#define DVMRP_PRUNING_DEBUG         0x4

/*
 * The actual debugging flags are defined in ipmulticast_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ipmulticast/ipmulticast_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ipmulticast/ipmulticast_debug_flags.h"



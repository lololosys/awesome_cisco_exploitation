/* $Id: at_debug.h,v 3.2.60.1 1996/05/02 08:39:19 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_debug.h,v $
 *------------------------------------------------------------------
 * at_debug.h -- Appletalk debugging support
 *
 * September 1992, Steven Lin
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: at_debug.h,v $
 * Revision 3.2.60.1  1996/05/02  08:39:19  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2  1995/11/17  08:41:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:07:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_DEBUG_H__
#define __AT_DEBUG_H__

#define ATALK_LOGLEVEL_MASK  0x00000002 /* Used for debugging level */
#define ATALK_INTERFACE_MASK 0x00000001	/* Used for debugging interfaces. */
#define ATALK_NULL_MASK      0x0        /* Used for general debugging. */
#define ATALK_DEBUG_INTERFACE 1

extern void atalk_debug_init(void);
extern void atalk_debug_command(parseinfo *);
extern void atalk_debug_all(boolean);
extern void atalk_debug_show(void);
extern void atalk_debug_setflag(parseinfo const *, atalkdebugtype *);
extern boolean at_debug(atalkdebugtype const *, int, ...);
extern void atalk_debug_show_flag(atalkdebugtype const *, boolean, boolean);

extern atalkdebugtype *atalk_events;
extern tinybool atalk_events_logged;
extern atalkdebugtype *atalknbp_debug;
extern atalkdebugtype *atalkpkt_debug;
extern atalkdebugtype *atalkrsp_debug;
extern atalkdebugtype *atalkrtmp_debug;
extern atalkdebugtype *atalkigrp_debug;
extern atalkdebugtype *atalkzip_debug;
extern atalkdebugtype *atalkerr_debug;
extern atalkdebugtype *appletalk_debug;
extern atalkdebugtype *atalkredist_debug;
extern atalkdebugtype *aarp_debug;
extern atalkdebugtype *atalkmacip_debug;
extern atalkdebugtype *atalklb_debug;

/* Keyword codes for the parser */
enum {
    DEBUG_ZIP,
    DEBUG_ATALKROUTE,
    DEBUG_ATALKREDIST,
    DEBUG_ATALKRTMP,
    DEBUG_ATALKPKT,
    DEBUG_ATALKRSP,
    DEBUG_NBP,
    DEBUG_ATALKFS,
    DEBUG_APPLEEVT,
    DEBUG_APPLEERRS,
    DEBUG_APPLEARP,
    DEBUG_ATALK_LB
};

#endif

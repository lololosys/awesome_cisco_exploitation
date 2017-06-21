/* $Id: atip_debug.h,v 3.1.60.1 1996/05/02 08:41:08 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atip_debug.h,v $
 *------------------------------------------------------------------
 * Appletalk IP-services debugging support
 *
 * March 1994, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 * 
$Endlog$
 */

#ifndef __ATIP_DEBUG_H__
#define __ATIP_DEBUG_H__

void atip_debug_init(void);

extern atalkdebugtype *atalkiptalk_debug;
extern atalkdebugtype *atalkmacip_debug;

/* Keyword codes for the parser */
enum {
    DEBUG_ATALK_IPTALK,
    DEBUG_ATALK_MACIP,
    DEBUG_ATIP_MAX			/* don't use this in the parser */
};

#endif

/* $Id: ataurp_debug.h,v 3.1.8.1 1996/05/02 08:41:01 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/ataurp_debug.h,v $
 *------------------------------------------------------------------
 * Appletalk AURP-service debugging support
 *
 * March 1996, Dean Wong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ataurp_debug.h,v $
 * Revision 3.1.8.1  1996/05/02  08:41:01  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/03/14  00:35:38  dwong
 * Placeholder for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATAURP_DEBUG_H__
#define __ATAURP_DEBUG_H__

void ataurp_debug_init(void);

extern atalkdebugtype *atalkaurp_update_debug;
extern atalkdebugtype *atalkaurp_packet_debug;
extern atalkdebugtype *atalkaurp_connection_debug;

/* Keyword codes for the parser */
enum {
    DEBUG_AURP_UPDATE,
    DEBUG_AURP_REDIST,
    DEBUG_AURP_PACKET,
    DEBUG_AURP_CONNECTION,
    DEBUG_ATAURP_MAX			/* don't use this in the parser */
};

#endif


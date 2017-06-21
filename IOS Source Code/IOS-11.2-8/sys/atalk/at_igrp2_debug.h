/* $Id: at_igrp2_debug.h,v 3.2 1995/11/17 08:42:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_igrp2_debug.h,v $
 *------------------------------------------------------------------
 * Debugging support for AppleTalk EIGRP
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_igrp2_debug.h,v $
 * Revision 3.2  1995/11/17  08:42:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:34  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __AT_IGRP2_DEBUG_H__
#define __AT_IGRP2_DEBUG_H__

extern atalkdebugtype *atalkigrp_events;
extern atalkdebugtype *atalkigrp_debug;
extern atalkdebugtype *atalkigrp_update_debug;
extern atalkdebugtype *atalkigrp_query_debug;
extern atalkdebugtype *atalkigrp_request_debug;
extern atalkdebugtype *atalkigrp_hello_debug;
extern atalkdebugtype *atalkigrp_external_debug;
extern atalkdebugtype *atalkigrp_redist_debug;
extern atalkdebugtype *atalkigrp_packet_debug;
extern atalkdebugtype *atalkigrp_target_debug;

/* Keyword codes for the parser */
enum {
    DEBUG_ATALKIGRP_EVENTS,
    DEBUG_ATALKIGRP_EXTERNAL,
    DEBUG_ATALKIGRP_REQUEST,
    DEBUG_ATALKIGRP_HELLO,
    DEBUG_ATALKIGRP_QUERY,
    DEBUG_ATALKIGRP_UPDATE,
    DEBUG_ATALKIGRP_REDIST,
    DEBUG_ATALKIGRP_PACKET,
    DEBUG_ATALKIGRP_TARGET,
    DEBUG_ATALKIGRP_PEER,
    DEBUG_ATALKIGRP_ALL,
    DEBUG_ATALKIGRP_MAX			/* don't use this in the parser */
};

extern void atigrp2_debug_init(void);

#endif

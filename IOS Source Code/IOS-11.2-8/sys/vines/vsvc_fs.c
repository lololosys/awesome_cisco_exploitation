/* $Id: vsvc_fs.c,v 3.2 1995/11/17 17:59:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/vsvc_fs.c,v $
 *------------------------------------------------------------------
 * vsvc_fs.c - Support for the `VINES Files' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_fs.c,v $
 * Revision 3.2  1995/11/17  17:59:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:30:20  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  23:15:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"

#include "vsvc_fs.h"
#include "vsvc_fs.rpch"
#include "vsvc_fs.rpcc"

/**************************************************
 *
 *		     VFS - Status
 *
 **************************************************/

/*
 * vfs_status_rcv_call
 *
 * This routine is invoked when the vines whatz command attempts to talk
 * directly to the server providing "routing service" to learn it's name
 * and the name of the local file service.
 */
static const char vfs_not_here[] = "-- File Service is not local --";

ushort vfs_status_rcv_call (
    vinesipcsocket *socket,
    vfs_status_query *dummy,
    vfs_status_reply *reply)
{
    sstrncpy(reply->service_name, vfs_not_here, VST_ITEMLEN);
    reply->category = 2;
    sstrncpy(reply->server_name, vsvc_hostname(), VST_DOMAINLEN);
    reply->active_sessions = 0;
    reply->total_sessions = 0;
    reply->uptime = system_uptime_seconds();
    reply->service_status = 1;
    return(VSVC_NO_ERROR);
}


/**************************************************
 *
 *            FS - Search / ReturnAddress
 *
 **************************************************/

/*
 * vfs_rcv_search
 *
 * A NetRPC search message has been received on the VINES Files port.
 */
void vfs_rcv_search (
    vinesipcsocket *socket,
    ushort user_num,
    uchar *user_string,
    uchar *info1,
    uchar *info2,
    uchar *info3)
{
    /* Do nothing */
}

/*
 * vfs_rcv_returnaddress
 *
 * A NetRPC returnaddress message has been received on the VINES Files
 * port.  
 */
void vfs_rcv_returnaddress (
    vinesipcsocket *socket,
    ushort proto_rev,
    ulong  service_rev,
    vinesaddress *service_address)
{
    /* Do nothing */
}


/**************************************************
 *
 *	        VS - Miscellaneous
 *
 **************************************************/

/*
 * vfs_init
 *
 * Boot time initializion of information that is maintained by the VINES
 * Files Service.  This routine will only ever be called once by the
 * router.
 */
void vfs_init (void)
{
    vsvc_install_svc(&vsvc_fs_info);
}

/*
 * vfs_start
 *
 * Run time initialize of information that is maintained by the VINES
 * Files Service.  This routine will be called each time the 'vines
 * routing' command is issued.
 */
void vfs_start (void)
{
}

/*
 * vfs_stop
 *
 * Purge all information that is maintained by the VINES Files Service.
 * This routine will be called each time the 'no vines routing' command
 * is issued.
 */
void vfs_stop (void)
{
}

void vfs_display (void)
{
}

/* $Id: ibmappnmibrequest.h,v 3.2 1995/11/17 08:37:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/appn/ibmappnmibrequest.h,v $
 *------------------------------------------------------------------
 * SNMP request header used by APPN compoents.
 *
 * April 1995, Mark Regan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmappnmibrequest.h,v $
 * Revision 3.2  1995/11/17  08:37:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:03:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _IBMAPPNMIBREQUEST_H_
#define _IBMAPPNMIBREQUEST_H_



/*
 * The ibmappnGeneralInfoAndCaps family. 
 */

typedef
struct _RequestHeader_t {
        int             command_type;
} RequestHeader_t;

typedef
struct _Request_t {
        RequestHeader_t         header;
        void                    *instance_p;
} Request_t;


#endif

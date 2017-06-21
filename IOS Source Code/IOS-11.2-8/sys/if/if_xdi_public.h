/* $Id: if_xdi_public.h,v 3.2 1995/11/17 09:29:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/if_xdi_public.h,v $
 *------------------------------------------------------------------
 * Global FDDI definitions used by the Cisco router software.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: if_xdi_public.h,v $
 * Revision 3.2  1995/11/17  09:29:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_XDI_PUBLIC_H__
#define __IF_XDI_PUBLIC_H__

extern void SendMAPMessage (SMTMessage * msg);

#endif

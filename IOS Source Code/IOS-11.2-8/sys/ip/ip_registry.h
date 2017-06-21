/* $Id: ip_registry.h,v 3.2.60.1 1996/05/17 11:24:36 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_registry.h,v $
 *------------------------------------------------------------------
 * ip_registry.h -- Registration Points for IP support
 *
 * 13-May-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ip_registry.h,v $
 * Revision 3.2.60.1  1996/05/17  11:24:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/26  07:55:49  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2  1995/11/17  09:33:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:57:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__IP_REGISTRY_H__
#define	__IP_REGISTRY_H__

typedef void (*tcpdriver_readf)(tcp_encaps *, tcbtype *, int, void *);

#include "address.h"
#include "file.h"

#include "../h/registry.h"
#include "../clns/isis_nlsp_proc_nums.h"  /* needed now for ip_registry - gcox*/
#include "../ip/ip_registry.regh"

#endif	/* __IP_REGISTRY_H__ */

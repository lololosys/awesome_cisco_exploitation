/* $Id: ppp_registry.h,v 3.1.2.3 1996/04/29 11:31:33 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_registry.h,v $
 *------------------------------------------------------------------
 * ppp_registry.h
 *
 * April 1996, Shoou Yiu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp_registry.h,v $
 * Revision 3.1.2.3  1996/04/29  11:31:33  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Use relative path in include of ppp_registry.regh
 *
 * Revision 3.1.2.2  1996/04/29  09:46:33  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.1.2.1  1996/04/27  06:38:10  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.4.1  1996/04/17  00:02:12  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1  1996/04/06  02:10:07  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __PPP_REGISTRY_H__
#define __PPP_REGISTRY_H__

#include "../h/registry.h"
#include "../h/aaa.h"
#include "../wan/ppp_registry.regh"

#endif  /* __PPP_REGISTRY_H__ */

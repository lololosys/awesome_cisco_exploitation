/* $Id: ntp_registry_private.h,v 3.1.2.1 1996/08/19 18:55:16 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_registry_private.h,v $
 *------------------------------------------------------------------
 * NTP private registry include files
 *
 * July 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_registry_private.h,v $
 * Revision 3.1.2.1  1996/08/19  18:55:16  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1  1996/07/28  02:42:08  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NTP_REGISTRY_PRIVATE_H__
#define __NTP_REGISTRY_PRIVATE_H__

extern long ntp_private_regcode;

#include "../h/registry.h"
#include "../ntp/ntp_registry_private.regh"

#endif /* __NTP_REGISTRY_PRIVATE_H__ */

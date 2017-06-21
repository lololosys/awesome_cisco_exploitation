/* $Id: srb_core_registry.h,v 3.1.4.2 1996/08/26 15:13:20 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_core_registry.h,v $
 *------------------------------------------------------------------
 * srb_core_registry.h -- Service Points for SRB_CORE registry
 *
 * August 1996, Paul Pearce
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: srb_core_registry.h,v $
 * Revision 3.1.4.2  1996/08/26  15:13:20  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/08/12  02:16:20  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.1  1996/08/08  16:10:08  ppearce
 * Placeholder for SRB core registry (CSCdi64450)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__SRB_CORE_REGISTRY_H__
#define	__SRB_CORE_REGISTRY_H__

#include "interface.h"
#include "registry.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.regh"

#endif	/* __SRB_CORE_REGISTRY_H__ */

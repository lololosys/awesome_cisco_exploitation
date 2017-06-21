/* $Id: svip_debug.h,v 3.1.58.1 1996/03/21 22:49:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/svip_debug.h,v $
 *------------------------------------------------------------------
 * svip_debug.h -- Debugging declarations for SVIP
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_debug.h,v $
 * Revision 3.1.58.1  1996/03/21  22:49:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.6.1  1996/02/10  00:05:05  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1996/01/02  06:27:09  getchell
 * Placeholder for VIPER development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up Viper debugging */
extern void svip_debug_init(void);

/*
 * The actual debugging flags are defined in svip_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/svip_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/svip_debug_flags.h"

/* end of file */

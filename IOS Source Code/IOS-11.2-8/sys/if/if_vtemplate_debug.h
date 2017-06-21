/* $Id: if_vtemplate_debug.h,v 3.1.34.1 1996/04/27 05:49:48 syiu Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vtemplate_debug.h,v $
 *------------------------------------------------------------------
 * if_vtemplate_debug.h - Debug support for Virtual Template
 * 
 * February 1996, Perry Leung 
 *
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vtemplate_debug.h,v $
 * Revision 3.1.34.1  1996/04/27  05:49:48  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.1  1996/02/10  02:19:42  perryl
 * Branch: Dial1_branch
 * Add debug support for Virtual Template
 *
 * Revision 3.1  1996/02/10  00:27:09  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void vtemplate_debug_init(void);
/*
 * The actual debugging flags are defined in vtemplate_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/if_vtemplate_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/if_vtemplate_debug_flags.h"

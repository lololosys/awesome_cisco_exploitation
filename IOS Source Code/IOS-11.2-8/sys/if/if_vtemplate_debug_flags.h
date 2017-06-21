/* $Id: if_vtemplate_debug_flags.h,v 3.1.34.1 1996/04/27 05:49:49 syiu Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vtemplate_debug_flags.h,v $
 *------------------------------------------------------------------
 * if_vtemplate_debug_flags.h - Debug support for Virtual Template
 * 
 * February 1996, Perry Leung 
 *
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vtemplate_debug_flags.h,v $
 * Revision 3.1.34.1  1996/04/27  05:49:49  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.1  1996/02/10  02:19:43  perryl
 * Branch: Dial1_branch
 * Add debug support for Virtual Template
 *
 * Revision 3.1  1996/02/10  00:27:11  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(vtemplate_debug_arr)

DEBUG_FLAG(vtemplate_debug,DEBUG_VTEMPLATE,"Virtual Template")

DEBUG_ARRDONE

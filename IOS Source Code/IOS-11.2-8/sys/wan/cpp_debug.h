/* $Id: cpp_debug.h,v 3.1.2.1 1996/05/10 01:44:25 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/cpp_debug.h,v $
 *------------------------------------------------------------------
 * cpp_debug.h -- Debugging declarations for cpp
 *
 * October, 1995, Bill May
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpp_debug.h,v $
 * Revision 3.1.2.1  1996/05/10  01:44:25  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:38  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up cpp debugging */
extern void cpp_debug_init(void);

/*
 * The actual debugging flags are defined in cpp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/cpp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/cpp_debug_flags.h"

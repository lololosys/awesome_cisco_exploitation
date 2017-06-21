/* $Id: mlpvt_debug.h,v 1.1.42.1 1996/04/27 06:34:28 syiu Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_debug.h,v $
 *------------------------------------------------------------------
 * mlpvt_debug.h -- Debugging declarations for MLPVT
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_debug.h,v $
 * Revision 1.1.42.1  1996/04/27  06:34:28  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.1  1996/01/26  20:28:41  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:39  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void mlpvt_debug_init(void);

/*
 * The actual debugging flags are defined in mlpvt_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../mlpvt/mlpvt_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../mlpvt/mlpvt_debug_flags.h"

/* $Id: csu_dsu_debug.h,v 3.2 1996/03/29 03:49:35 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/csu_dsu_debug.h,v $
 *------------------------------------------------------------------
 * csu_dsu_debug.h -- Debugging declarations for General
 *				service-module
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csu_dsu_debug.h,v $
 * Revision 3.2  1996/03/29  03:49:35  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:14  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up service-module debugging */
extern void csu_dsu_debug_init(void);

/*
 * The actual debugging flags are defined in csu_dsu_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "csu_dsu_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "csu_dsu_debug_flags.h"

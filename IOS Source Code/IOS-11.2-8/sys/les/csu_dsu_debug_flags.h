/* $Id: csu_dsu_debug_flags.h,v 3.2 1996/03/29 03:49:36 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/csu_dsu_debug_flags.h,v $
 *------------------------------------------------------------------
 * csu_dsu_debug_flags.h -- Debugging flag declarations for
 *                                      service-module
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csu_dsu_debug_flags.h,v $
 * Revision 3.2  1996/03/29  03:49:36  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:16  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from service-module code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(csu_dsu_debug_arr)

DEBUG_FLAG(csu_dsu_debug,DEBUG_SERVICE_MODULE,"Service module")

DEBUG_ARRDONE

/* $Id: mlpvt_debug_flags.h,v 1.1.42.3 1996/05/22 03:41:50 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_debug_flags.h,v $
 *------------------------------------------------------------------
 * mlpvt_debug_flags.h -- Debugging flag declarations for MLPVT
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_debug_flags.h,v $
 * Revision 1.1.42.3  1996/05/22  03:41:50  perryl
 * CSCdi58388:  SGBP is missing an debug SGBP error option
 * Branch: California_branch
 *
 * Revision 1.1.42.2  1996/05/18  22:34:50  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.1  1996/04/27  06:34:30  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.1  1996/01/26  20:28:42  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:41  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) to define debugging flags.
 * For this, we use some macros that get defined  
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(mlpvt_debug_arr)

DEBUG_FLAG(mlpvt_debug_errors,DEBUG_MLPVT_ERRORS,"SGBP errors")
DEBUG_FLAG(mlpvt_debug_msgs,DEBUG_MLPVT_MSGS,"SGBP messages")
DEBUG_FLAG(mlpvt_debug_queries,DEBUG_MLPVT_QUERIES,"SGBP mastership queries")
DEBUG_FLAG(mlpvt_debug_events,DEBUG_MLPVT_EVENTS,"SGBP events")
DEBUG_FLAG(mlpvt_debug_hellos,DEBUG_MLPVT_HELLOS,"SGBP hellos")

DEBUG_ARRDONE

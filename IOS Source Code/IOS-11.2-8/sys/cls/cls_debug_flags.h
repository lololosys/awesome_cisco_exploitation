/* $Id: cls_debug_flags.h,v 3.2.60.1 1996/05/17 10:41:44 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/cls_debug_flags.h,v $
 *------------------------------------------------------------------
 * cls_debug_flags.h -- Debugging flag declarations for CLS
 *
 * April 1994, Steve Berl
 *
 * Copyright (c) 1994-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cls_debug_flags.h,v $
 * Revision 3.2.60.1  1996/05/17  10:41:44  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  14:35:29  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/03/28  08:19:11  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.2.4.1  1995/12/17  09:49:53  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:54:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:19:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from CLS code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(cls_debug_arr)

DEBUG_FLAG(cls_debug_err,DEBUG_CLS_ERROR,"CLS Errors")
DEBUG_FLAG(cls_debug_message,DEBUG_CLS_MESSAGE,"CLS Messages")
DEBUG_FLAG(cls_debug_vdlc,DEBUG_CLS_VDLC,"VDLC")
DEBUG_FLAG(cls_debug_sdlc,DEBUG_CLS_SDLC,"CLS SDLC")
DEBUG_FLAG(cls_debug_state,DEBUG_CLS_STATE,"CLS State")
DEBUG_FLAG(cls_debug_test,DEBUG_CLS_TEST,"CLS Internal Testing")

DEBUG_ARRDONE

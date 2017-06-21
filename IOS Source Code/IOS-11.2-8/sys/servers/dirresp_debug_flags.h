/* $Id: dirresp_debug_flags.h,v 3.1.18.1 1996/07/16 08:01:26 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/dirresp_debug_flags.h,v $
 *------------------------------------------------------------------
 * dirresp_debug_flags.h -- Debugging flag declarations for director-responder
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dirresp_debug_flags.h,v $
 * Revision 3.1.18.1  1996/07/16  08:01:26  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1  1996/02/20  22:46:01  pst
 * Initial incorporation of distributed directory agent support
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from interface code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(dirresp_debug_arr)
DEBUG_FLAG(dirresp_debug,DEBUG_DIRRESP,"Director Response Protocol")
DEBUG_ARRDONE

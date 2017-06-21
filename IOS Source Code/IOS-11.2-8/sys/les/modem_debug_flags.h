/* $Id: modem_debug_flags.h,v 3.1.64.1 1996/06/16 21:16:19 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_debug_flags.h,v $
 *------------------------------------------------------------------
 * modem_debug_flags.h -- Debugging flag declarations for Modem Management
 *
 * December, 1995 - Viet Nguyen
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_debug_flags.h,v $
 * Revision 3.1.64.1  1996/06/16  21:16:19  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:32  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from Modem Mgmt code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(modem_debug_arr)

DEBUG_FLAG(modem_csm_debug,DEBUG_MODEM_CSM,
           "Modem Management Call Switching Module")
DEBUG_FLAG(modem_oob_debug,DEBUG_MODEM_OOB,
           "Modem Management Out Of Band Port")
DEBUG_FLAG(modem_trace_debug,DEBUG_MODEM_TRACE,
           "Modem Call Trace")

DEBUG_ARRDONE


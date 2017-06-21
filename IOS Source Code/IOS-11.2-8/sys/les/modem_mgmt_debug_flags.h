/* $Id: modem_mgmt_debug_flags.h,v 3.1.68.1 1996/06/16 21:16:30 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_mgmt_debug_flags.h,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * modem_mgmt_debug_flags.h -- Debugging flag declarations for Modem Management
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_mgmt_debug_flags.h,v $
 * Revision 3.1.68.1  1996/06/16  21:16:30  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:25  jchlin
 * Placeholders for new development.
 *
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
DEBUG_ARRDECL(modem_mgmt_debug_arr)

DEBUG_FLAG(modem_mgmt_csm_debug,DEBUG_MODEM_MGMT_CSM,"Modem Management Call Switching Module")

DEBUG_ARRDONE


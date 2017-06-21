/* $Id: modem_mgmt_debug.h,v 3.1.68.2 1996/07/11 01:11:42 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_mgmt_debug.h,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * modem_mgmt_debug.h -- Debugging declarations for Modem Management
 *
 * October 1995, Joel P. Bion
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_mgmt_debug.h,v $
 * Revision 3.1.68.2  1996/07/11  01:11:42  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.68.1  1996/06/16  21:16:28  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:23  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up modem management debugging */
extern void modem_mgmt_debug_init(void);

/*
 * The actual debugging flags are defined in modem_mgmt_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "modem_mgmt_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "modem_mgmt_debug_flags.h"

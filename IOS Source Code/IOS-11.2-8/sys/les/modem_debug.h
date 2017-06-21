/* $Id: modem_debug.h,v 3.1.64.2 1996/07/11 01:11:36 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_debug.h,v $
 *------------------------------------------------------------------
 * modem_debug.h -- Debugging declarations for Modem Management
 *
 * December 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_debug.h,v $
 * Revision 3.1.64.2  1996/07/11  01:11:36  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.64.1  1996/06/16  21:16:17  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:30  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up modem management debugging */
extern void modem_debug_init(void);

/*
 * The actual debugging flags are defined in modem_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "modem_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "modem_debug_flags.h"

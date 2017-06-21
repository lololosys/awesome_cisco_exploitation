/* $Id: sntp_debug.h,v 3.1.2.1 1996/06/23 04:40:25 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp_debug.h,v $
 *------------------------------------------------------------------
 * Placeholder
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp_debug.h,v $
 * Revision 3.1.2.1  1996/06/23  04:40:25  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:55  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up SNTP debugging */
extern void sntp_debug_init(void);

/*
 * The actual debugging flags are defined in sntp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ntp/sntp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ntp/sntp_debug_flags.h"

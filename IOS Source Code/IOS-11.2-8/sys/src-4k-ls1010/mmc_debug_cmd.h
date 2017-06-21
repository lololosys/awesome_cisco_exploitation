/* $Id: mmc_debug_cmd.h,v 3.1 1995/11/09 13:23:44 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-ls1010/mmc_debug_cmd.h,v $
 *------------------------------------------------------------------
 * parser commands for mmc driver 
 *
 * September 1995, Ram Bhide.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mmc_debug_cmd.h,v $
 * Revision 3.1  1995/11/09  13:23:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/27  14:56:38  jwilliam
 * initail checkin
 *
 * Revision 2.2  1995/08/03  00:28:55  jjohnson
 * CSCdi38141:  template files contain bogus log information
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up ATM OAM debugging */
extern void mmc_debug_init(void);

/*
 * The actual debugging flags are defined in atmsig_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../src-4k-ls1010/mmc_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../src-4k-ls1010/mmc_debug_flags.h"

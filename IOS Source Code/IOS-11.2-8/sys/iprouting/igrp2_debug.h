/* $Id: igrp2_debug.h,v 3.2.60.1 1996/04/02 05:37:07 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp2_debug.h,v $
 *------------------------------------------------------------------
 * IGRP2 debug header file 
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: igrp2_debug.h,v $
 * Revision 3.2.60.1  1996/04/02  05:37:07  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:34:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:06:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The actual debugging flags are defined in iprouting_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../iprouting/igrp2_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../iprouting/igrp2_debug_flags.h"

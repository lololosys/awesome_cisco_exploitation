/* $Id: igrp2_debug_flags.h,v 3.2.60.1 1996/04/02 05:37:08 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp2_debug_flags.h,v $
 *------------------------------------------------------------------
 * IGRP2 debugging flag declarations
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: igrp2_debug_flags.h,v $
 * Revision 3.2.60.1  1996/04/02  05:37:08  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:34:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:06:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from IGRP2 code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(igrp2_debug_arr)

DEBUG_FLAG(igrp2_fsm_debug,DEBUG_IGRP2_FSM,"EIGRP FSM Events/Actions")
DEBUG_FLAG(igrp2_packet_debug,DEBUG_IGRP2_PKT,"EIGRP Packets")
DEBUG_FLAG(igrp2_nbrs_debug,DEBUG_IGRP2_NBRS,"EIGRP Neighbors")
DEBUG_FLAG(igrp2_xmit_debug,DEBUG_IGRP2_XMIT,"EIGRP Transmission Events")
DEBUG_ARRDONE

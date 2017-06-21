/* $Id: rtt_debug.h,v 1.1.4.2 1996/06/07 22:26:00 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_debug.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the definitions for
 * the RTT Monitor debug parse chain.
 *------------------------------------------------------------------
 * $Log: rtt_debug.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:00  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:24  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:33  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:25  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_DEBUG_H_
#define _RTT_DEBUG_H_

#include "rtt_debug_ext.h"

/*
 * The actual debugging flags are defined in rttmon_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "rtt_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "rtt_debug_flags.h"

#endif /* _RTT_DEBUG_H_ */

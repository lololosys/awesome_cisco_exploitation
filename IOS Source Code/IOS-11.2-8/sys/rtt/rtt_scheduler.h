/* $Id: rtt_scheduler.h,v 1.1.4.2 1996/06/07 22:26:23 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_scheduler.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor scheduler interface Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * scheduler interface calls used by the components of the
 * RTT Monitor. 
 *------------------------------------------------------------------
 * $Log: rtt_scheduler.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:23  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/03/22  18:41:47  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:34  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_SCHEDULER_H_
#define _RTT_SCHEDULER_H_

#include "rtt_lib.h"

extern void start_rtt_scheduler SR_PROTOTYPE((void));

#endif /* _RTT_SCHEDULER_H_ */

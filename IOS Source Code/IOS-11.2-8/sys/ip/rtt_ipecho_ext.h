/* $Id: rtt_ipecho_ext.h,v 3.1.4.1 1996/05/17 11:25:13 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ip/rtt_ipecho_ext.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor external IP probe Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the definitions for
 * the registration of the IP probe implementation for the RTT
 * Monitor.
 *------------------------------------------------------------------
 * $Log: rtt_ipecho_ext.h,v $
 * Revision 3.1.4.1  1996/05/17  11:25:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/14  06:19:30  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.1  1996/03/22  18:54:02  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:55:57  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_IPECHO_EXT_H_
#define _RTT_IPECHO_EXT_H_

#include "../snmp/snmp_api.h"

extern void
    register_rtt_mon
    SR_PROTOTYPE((void));

#endif /* _RTT_IPECHO_EXT_H_ */



      

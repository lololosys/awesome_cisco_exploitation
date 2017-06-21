/* $Id: rtt_ipecho.h,v 3.1.4.2 1996/06/07 22:25:18 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/ip/rtt_ipecho.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor IP probe Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the  definitions for
 * the IP probe implementation for the RTT Monitor.
 *------------------------------------------------------------------
 * $Log: rtt_ipecho.h,v $
 * Revision 3.1.4.2  1996/06/07  22:25:18  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:25:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/14  06:19:28  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.1  1996/03/22  18:54:01  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:55:57  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_IPECHO_H_
#define _RTT_IPECHO_H_

#include "master.h"
#include "../snmp/snmp_api.h"

extern int rtt_ip_echo_exists SR_PROTOTYPE((void));

#endif /* _RTT_IPECHO_H_ */
      

/* $Id: rtt_ipecho.c,v 3.1.4.3 1996/07/31 21:42:19 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/ip/rtt_ipecho.c,v $
 *------------------------------------------------------------------
 * IP Echo Probe Application.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the probe implementation that is specific
 *  to IP for the Round Trip Time Monitor Application. 
 *------------------------------------------------------------------
 * $Log: rtt_ipecho.c,v $
 * Revision 3.1.4.3  1996/07/31  21:42:19  lmetzger
 * CSCdi64744:  Move Registry Initialization to the RTT subsystem
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/07  22:25:16  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:25:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/05/14  06:19:27  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.2  1996/04/19  02:40:13  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:54:00  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:55:56  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "rtt_ipecho.h"
#include "../rtt/rtt_registry.h"

boolean rtt_ip_echo_exists (void)
{
    return (TRUE);
}

void register_rtt_mon (void)
{
    if (registry_populated(REG_RTT)) {
	reg_add_rtt_echo_exists(D_rttMonEchoAdminProtocol_ipIcmpEcho,
				rtt_ip_echo_exists,
				"rtt_ip_echo_exists");
    }
    return;
}

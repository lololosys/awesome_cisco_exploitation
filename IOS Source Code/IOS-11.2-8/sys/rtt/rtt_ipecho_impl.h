/* $Id: rtt_ipecho_impl.h,v 1.1.4.2 1996/06/07 22:26:12 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_ipecho_impl.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor IP Echo Probe Implementation.
 *
 * Apr 1996, Larry Metzger
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtt_ipecho_impl.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:12  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/05/14  06:20:39  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1  1996/05/14  00:34:50  lmetzger
 * Placeholder for Response Time Reporter code review updates
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_IPECHO_IMPL_H_
#define _RTT_IPECHO_IMPL_H_

#include "../rtt/rtt_lib.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../util/random.h"
#include "../snmp/snmp_api.h"

#define NPROBES  3
#define MAXTTL   30
#define MINTTL   1
#define WAITSECS 3
#define PING_PATTERN 0xABCD

#define D_echoTarget_timeout            1
#define D_echoTarget_connectionLost     2
#define D_echoTarget_internalError      3
#define D_echoTarget_ayyOK              4

#define MIN_ICMP_PKT    IPHEADERBYTES(NOPT) + sizeof(icmptype) + sizeof(sys_timestamp)
#define MAX_ICMP_PKT    MAXDATAGRAMSIZE

extern void register_rtt_mon_impl SR_PROTOTYPE((void));

extern void rtt_ip_echo_activate SR_PROTOTYPE((rttMonCtrlAdminQItem
					       * inRttMonCtrlAdminQItem));

extern int rtt_ip_echo_validate SR_PROTOTYPE((rttMonCtrlAdminEntry_t
					      * ctrlAdminEntry,
					      void
					      *specificEntry,
					      int
					      rttObject));

#endif /* _RTT_IPECHO_IMPL_H_ */
      

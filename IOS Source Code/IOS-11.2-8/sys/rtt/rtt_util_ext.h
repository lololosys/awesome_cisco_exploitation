/* $Id: rtt_util_ext.h,v 1.1.4.2 1996/06/07 22:26:34 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_util_ext.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor dump interface Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * dump interface calls used by the components of the
 * RTT Monitor. 
 *------------------------------------------------------------------
 * $Log: rtt_util_ext.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:34  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:42:01  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:57  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:41  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_UTIL_EXT_H_
#define _RTT_UTIL_EXT_H_

#include "master.h"
#include "../snmp/snmp_api.h"
#include "../rtt/rtt_show.h"

/*
 * MAXULONG_BITS is the number of bits of the 
 *   RTT_XXXXX_FLAG that we will support for
 *   debug and trace information.
 */
#define MAXULONG_BITS 32

extern unsigned long RTT_ERROR_FLAG;

extern unsigned long RTT_TRACE_FLAG;

extern void turn_rtt_debug SR_PROTOTYPE((boolean flag,
					 long entryNumber));

extern void turn_rtt_trace SR_PROTOTYPE((boolean flag,
					 long entryNumber));

extern void rttMonOutputAppl SR_PROTOTYPE((rttMonOutputFlag outFlag));

extern void rttMonOutputCtrlTable SR_PROTOTYPE((long ctrlAdminIndex,
						boolean indexExists,
						rttMonOutputFlag outFlag));

extern void rttMonOutputOperTable SR_PROTOTYPE((long ctrlAdminIndex,
						boolean indexExists,
						rttMonOutputFlag outFlag));

extern void rttMonOutputHistoryTable SR_PROTOTYPE((long ctrlAdminIndex,
						   boolean indexExists,
						   rttMonOutputFlag outFlag));

extern void rttMonOutputStatsCollTable SR_PROTOTYPE((long ctrlAdminIndex,
						     boolean indexExists,
						     rttMonOutputFlag outFlag));

extern void rttMonOutputStatsDistTable SR_PROTOTYPE((long ctrlAdminIndex,
						     boolean indexExists,
						     rttMonOutputFlag outFlag));

extern void rttMonOutputStatsTotalsTable SR_PROTOTYPE((long ctrlAdminIndex,
						       boolean indexExists,
						       rttMonOutputFlag outFlag));

extern void rttMonOutputReactTriggerTable SR_PROTOTYPE((long ctrlAdminIndex,
							boolean indexExists,
							rttMonOutputFlag outFlag));

/*
 *  Configuration Routines
 */

extern void rttmon_config_base_command SR_PROTOTYPE((parseinfo * csb));

extern void rttmon_config_entry_command SR_PROTOTYPE((parseinfo * csb));

extern void rttmon_exit_config_submode SR_PROTOTYPE((parseinfo * csb));

extern boolean is_rttmon_snaLU2EchoAppl_configured SR_PROTOTYPE((void));

extern boolean is_rttmon_snaLU0EchoAppl_configured SR_PROTOTYPE((void));

extern boolean is_rttmon_snaRUEcho_configured SR_PROTOTYPE((void));

extern boolean is_rttmon_ipIcmpEcho_configured SR_PROTOTYPE((void));

#endif /* _RTT_UTIL_EXT_H_ */
      

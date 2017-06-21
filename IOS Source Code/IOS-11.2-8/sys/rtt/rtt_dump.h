/* $Id: rtt_dump.h,v 1.1.4.2 1996/06/07 22:26:07 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_dump.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor dump interface Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * dump interface calls used by the components of the
 * RTT Monitor. 
 *------------------------------------------------------------------
 * $Log: rtt_dump.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:07  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:36  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:31  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:38  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:28  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTT_DUMP_H_
#define _RTT_DUMP_H_

#include "rtt_lib.h"
#include "../rtt/rtt_show.h"

extern char *getRttMonCtrlOperStateString SR_PROTOTYPE((long state));

extern char *getRttMonAdminStatusString SR_PROTOTYPE((long state));

extern void dumpRttMonEchoAdminTargetAddress SR_PROTOTYPE((long
							   rttMonCtrlAdminIndex,
							   OctetString
							   * rttMonEchoAdminTargetAddress));

extern void dumpRttMonCtrlAdminRttType SR_PROTOTYPE((long
						     rttMonCtrlAdminIndex,
						     long
						     rttMonCtrlAdminRttType));

extern void dumpRttMonCtrlAdminQItem SR_PROTOTYPE((rttMonCtrlAdminQItem
						   * inRttMonCtrlAdminQItem));

extern void dumpRttMonReactTriggerAdminEntry SR_PROTOTYPE((rttMonReactTriggerAdminEntry_t
							   * inRttMonReactTriggerAdminEntry));

extern void dumpRttMonStatsCaptureEntry SR_PROTOTYPE((rttMonStatsCaptureEntry_t
						      * inRttMonStatsCaptureEntry));

extern void dumpRttMonStatsCollectEntry SR_PROTOTYPE((rttMonStatsCollectEntry_t
						      * inRttMonStatsCollectEntry));

extern void dumpRttMonStatsTotalsEntry SR_PROTOTYPE((rttMonStatsTotalsEntry_t
						     * inRttMonStatsTotalsEntry));

extern void dumpRttMonHistoryCollectionEntry SR_PROTOTYPE((rttMonHistoryCollectionEntry_t
							   * inRttMonHistoryCollectionEntry));

extern void dumpRttMonLibReturnCode SR_PROTOTYPE((long
						  rttMonCtrlAdminIndex,
						  long
						  rttMonLibReturnCode));

extern void dumpRttMonProcessHeaderString SR_PROTOTYPE((long
							rttMonCtrlAdminIndex,
							char *reasonStr));

extern void dumpRttMonProcessHeader SR_PROTOTYPE((long
						  rttMonCtrlAdminIndex,
						  long reasonID));

extern void dumpRttMonErrorReturnCode SR_PROTOTYPE((long
						    rttMonCtrlAdminIndex,
						    long functionNameID,
						    long callNameID));

/* Show command support */

extern void outputRttMonApplHead SR_PROTOTYPE((void));

extern void outputRttMonAppl SR_PROTOTYPE((rttMonAppl_t
					   * rttMonApplPtr));

extern void outputRttMonApplHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonApplCompact SR_PROTOTYPE((rttMonAppl_t
						  * rttMonApplPtr));

extern void outputRttMonApplSupportedRttTypesHead SR_PROTOTYPE((void));

extern void outputRttMonApplSupportedRttTypesEntry SR_PROTOTYPE((rttMonApplSupportedRttTypesEntry_t
								 * inRttMonApplSupportedRttTypesEntry));

extern void outputRttMonApplSupportedRttTypesHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonApplSupportedRttTypesEntryCompact SR_PROTOTYPE((rttMonApplSupportedRttTypesEntry_t
									* inRttMonApplSupportedRttTypesEntry));

extern void outputRttMonApplSupportedProtocolsHead SR_PROTOTYPE((void));

extern void outputRttMonApplSupportedProtocolsEntry SR_PROTOTYPE((rttMonApplSupportedProtocolsEntry_t
								  * inRttMonApplSupportedProtocolsEntry));

extern void outputRttMonApplSupportedProtocolsHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonApplSupportedProtocolsEntryCompact SR_PROTOTYPE((rttMonApplSupportedProtocolsEntry_t
									 * inRttMonApplSupportedProtocolsEntry));

extern void outputRttMonStatsCaptureTableHead SR_PROTOTYPE((void));

extern void outputRttMonStatsCaptureEntry SR_PROTOTYPE((rttMonStatsCaptureEntry_t
							* inRttMonStatsCaptureEntry));

extern void outputRttMonStatsCaptureTableHeadCompact SR_PROTOTYPE((void));


extern void outputRttMonStatsCaptureEntryCompact SR_PROTOTYPE((rttMonStatsCaptureEntry_t
							       * inRttMonStatsCaptureEntry));

extern void outputRttMonStatsCollectTableHead SR_PROTOTYPE((void));

extern void outputRttMonStatsCollectEntry SR_PROTOTYPE((rttMonEchoAdminEntry_t
							* inEchoAdminEntry,
							rttMonStatsCollectEntry_t
							* inRttMonStatsCollectEntry));

extern void outputRttMonStatsCollectTableHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonStatsCollectEntryCompact SR_PROTOTYPE((rttMonStatsCollectEntry_t
							       * inRttMonStatsCollectEntry));

extern void outputRttMonStatsTotalsTableHead SR_PROTOTYPE((void));

extern void outputRttMonStatsTotalsEntry SR_PROTOTYPE((rttMonStatsTotalsEntry_t
						       * inRttMonStatsTotalsEntry));

extern void outputRttMonStatsTotalsTableHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonStatsTotalsEntryCompact SR_PROTOTYPE((rttMonStatsTotalsEntry_t
							      * inRttMonStatsTotalsEntry));

extern void outputRttMonHistoryCollectionTableHead SR_PROTOTYPE((void));

extern void outputRttMonHistoryCollectionEntry SR_PROTOTYPE((rttMonEchoAdminEntry_t
							     * inEchoAdminEntry,
							     rttMonHistoryCollectionEntry_t
							     * inRttMonHistoryCollectionEntry));

extern void outputRttMonHistoryCollectionTableHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonHistoryCollectionEntryCompact SR_PROTOTYPE((rttMonHistoryCollectionEntry_t
								    * inRttMonHistoryCollectionEntry));

extern void outputRttMonReactTriggerTableHead SR_PROTOTYPE((void));

extern void outputRttMonReactTriggerEntry SR_PROTOTYPE((rttMonReactTriggerAdminEntry_t
							* inRttMonReactTriggerAdminEntry));

extern void outputRttMonReactTriggerTableHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonReactTriggerEntryCompact SR_PROTOTYPE((rttMonReactTriggerAdminEntry_t
							       * inRttMonReactTriggerAdminEntry));

extern void outputRttMonAdminTableHead SR_PROTOTYPE((void));

extern void outputRttMonAdminEntry SR_PROTOTYPE((rttMonCtrlAdminEntry_t
						 * inRttMonCtrlAdminEntry,
						 rttMonEchoAdminEntry_t
						 * inRttMonEchoAdminEntry));

extern void outputRttMonAdminTableHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonAdminEntryCompact SR_PROTOTYPE((rttMonCtrlAdminEntry_t
							* inRttMonCtrlAdminEntry,
							rttMonEchoAdminEntry_t
							* inRttMonEchoAdminEntry));

extern void outputRttMonOperTableHead SR_PROTOTYPE((void));

extern void outputRttMonOperEntry SR_PROTOTYPE((rttMonEchoAdminEntry_t
						* inRttMonEchoAdminEntry,
						rttMonCtrlAdminEntry_t
						* inRttMonCtrlAdminEntry));

extern void outputRttMonOperTableHeadCompact SR_PROTOTYPE((void));

extern void outputRttMonOperEntryCompact SR_PROTOTYPE((rttMonCtrlAdminEntry_t
						       * inRttMonCtrlAdminEntry));

#endif /* _RTT_DUMP_H_ */

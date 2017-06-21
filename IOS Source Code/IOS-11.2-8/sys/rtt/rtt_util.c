/* $Id: rtt_util.c,v 1.1.4.4 1996/08/28 13:08:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_util.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Utilities.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor utilities.
 *------------------------------------------------------------------
 * $Log: rtt_util.c,v $
 * Revision 1.1.4.4  1996/08/28  13:08:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.3  1996/06/07  22:26:30  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/22  17:51:47  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Occurances -> Occurrences
 * Occurences -> Occurrences
 * hundreths  -> hundredths
 * "unknown -> unkown" -> unknown
 * Varify -> Verify
 * Currrent -> Current
 * Occured -> Occurred
 * Sequece -> Sequence
 * millliseconds -> milliseconds
 * unkown -> unknown
 * soure -> source
 * Unkown -> Unknown
 *
 * Revision 1.1.4.1  1996/05/17  11:41:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:53  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:59  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:55  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:39  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../rtt/msg_rtt.c"		/* Not a typo, see logger.h */
#include "rtt_lib.h"
#include "rtt_dump.h"
#include "rtt_util.h"
#include "rtt_registry.h"
#include "rtt_scheduler.h"
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_snmp.h"
#include "rtt_debug_ext.h"


/*
 * Debug Support Routines
 */
void turn_rtt_debug (boolean flag,
		     long entryNumber)
{
    /* turn on or off all debuging */
    if (entryNumber < 0) {
	if (flag) {		/* on all */
	    RTT_ERROR_FLAG = MAXULONG;
	} else {		/* off all */
	    RTT_ERROR_FLAG = 0x00;
	}
    } else {
	if (flag) {		/* on one */
	    RTT_ERROR_FLAG |= (1 << entryNumber);
	} else {		/* off one */
	    RTT_ERROR_FLAG &= ~(1 << entryNumber);
	}
    } 
    return;
} 

void turn_rtt_trace (boolean flag,
		     long entryNumber)
{
    /* turn on or off all debuging */
    if (entryNumber < 0) {
	if (flag) {		/* on all */
	    /* ERROR is required to TRACE */
	    RTT_ERROR_FLAG = MAXULONG;
	    RTT_TRACE_FLAG = MAXULONG;
	} else {		/* off all */
	    RTT_TRACE_FLAG = 0x00;
	}
    } else {
	if (flag) {		/* on one */
	    /* ERROR is required to TRACE */
	    RTT_ERROR_FLAG |= (1 << entryNumber);
	    RTT_TRACE_FLAG |= (1 << entryNumber);
	} else {		/* off one */
	    RTT_TRACE_FLAG &= ~(1 << entryNumber);
	}
    } 
    return;
} 

boolean is_rttmon_snaLU2EchoAppl_configured (void)
{
    return (reg_invoke_rtt_echo_exists(D_rttMonEchoAdminProtocol_snaLU2EchoAppl));
} 

boolean is_rttmon_snaLU0EchoAppl_configured (void)
{
    return (reg_invoke_rtt_echo_exists(D_rttMonEchoAdminProtocol_snaLU0EchoAppl));
} 

boolean is_rttmon_snaRUEcho_configured (void)
{
    return (reg_invoke_rtt_echo_exists(D_rttMonEchoAdminProtocol_snaRUEcho));
} 

boolean is_rttmon_ipIcmpEcho_configured (void)
{
    return (reg_invoke_rtt_echo_exists(D_rttMonEchoAdminProtocol_ipIcmpEcho));
}				

/*
 * Show Support Routines
 */ 

void rttMonOutputAppl (rttMonOutputFlag outFlag)
{
    rttMonAppl_t *rttMonApplPtr;
    rttMonApplSupportedRttTypesEntry_t *rttMonApplSupportedRttTypesEntryPtr;
    rttMonApplSupportedProtocolsEntry_t *rttMonApplSupportedProtocolsEntryPtr;
    long supportedTypes = 0;
    long supportedProtocols = 0;

    rttMonApplPtr = getRttMonApplPtr();
    if (outFlag == rtt_output_full) {
	outputRttMonApplHead();
    } else if (outFlag == rtt_output_brief) {
	outputRttMonApplHeadCompact();
    }
    if (outFlag == rtt_output_full) {
	outputRttMonAppl(rttMonApplPtr);
    } else {
	outputRttMonApplCompact(rttMonApplPtr);
    } /* Supported RttTypes */ 
    if (outFlag == rtt_output_full) {
	outputRttMonApplSupportedRttTypesHead();
    } else if (outFlag == rtt_output_brief) {
	outputRttMonApplSupportedRttTypesHeadCompact();
    }
    rttMonApplSupportedRttTypesEntryPtr =
	getNextRttMonApplSupportedRttTypesEntryPtr(supportedTypes);
    while (rttMonApplSupportedRttTypesEntryPtr != NULL) {
	if (rttMonApplSupportedRttTypesEntryPtr->
	    rttMonApplSupportedRttTypesValid ==
	    D_rttMonApplSupportedRttTypesValid_true) {
	    if (outFlag == rtt_output_full) {
		outputRttMonApplSupportedRttTypesEntry(rttMonApplSupportedRttTypesEntryPtr);
	    } else {
		/* 
		 * Add a tab to each output to 
		 *  uniquely identify the Rtt Types
		 */
		if (outFlag == rtt_output_raw) {
		    printf("\t");
		}
		outputRttMonApplSupportedRttTypesEntryCompact(
				       rttMonApplSupportedRttTypesEntryPtr);
	    }
	} /* If type is supported */ 
	supportedTypes =
	    rttMonApplSupportedRttTypesEntryPtr->
	    rttMonApplSupportedRttTypes;
	supportedTypes++;
	rttMonApplSupportedRttTypesEntryPtr =
	    getNextRttMonApplSupportedRttTypesEntryPtr(supportedTypes);
    } /* Supported Protocols */ 
    if (outFlag == rtt_output_full) {
	outputRttMonApplSupportedProtocolsHead();
    } else if (outFlag == rtt_output_brief) {
	outputRttMonApplSupportedProtocolsHeadCompact();
    }
    rttMonApplSupportedProtocolsEntryPtr =
	getNextRttMonApplSupportedProtocolsEntryPtr(supportedProtocols);
    while (rttMonApplSupportedProtocolsEntryPtr != NULL) {
	if (rttMonApplSupportedProtocolsEntryPtr->
	    rttMonApplSupportedProtocolsValid ==
	    D_rttMonApplSupportedProtocolsValid_true) {
	    if (outFlag == rtt_output_full) {
		outputRttMonApplSupportedProtocolsEntry(rttMonApplSupportedProtocolsEntryPtr);
	    } else {
		/* 
		 * Add a two tabs to each output to 
		 *  uniquely identify the Protocols
		 */
		if (outFlag == rtt_output_raw) {
		    printf("\t\t");
		}
		outputRttMonApplSupportedProtocolsEntryCompact(rttMonApplSupportedProtocolsEntryPtr);
	    }
	} /* if entry is supported */ 
	supportedProtocols =
	    rttMonApplSupportedProtocolsEntryPtr->
	    rttMonApplSupportedProtocols;
	supportedProtocols++;
	rttMonApplSupportedProtocolsEntryPtr =
	    getNextRttMonApplSupportedProtocolsEntryPtr(supportedProtocols);
    } 
    return;
} 

static void rttMonOutputHistoryTableData (rttMonEchoAdminEntry_t
					  * outEchoAdminEntry,
					  long ctrlAdminIndex,
					  rttMonHistoryCollectionEntry_t
					  * historyCollectionEntryBuff,
					  rttMonOutputFlag outFlag)
{
    int return_code;
    long tmpCtrlAdminIndex;
    long tmpLifeIndex;
    long tmpBucketIndex;
    long tmpSampleIndex;

    return_code = D_Return_Successfull;
    tmpCtrlAdminIndex = ctrlAdminIndex;
    tmpLifeIndex = 0;
    tmpBucketIndex = 0;
    tmpSampleIndex = 0;

    while (return_code == D_Return_Successfull &&
	   tmpCtrlAdminIndex == ctrlAdminIndex) {
	return_code =
	    getNextRttMonHistoryCollectionEntry(tmpCtrlAdminIndex,
						tmpLifeIndex,
						tmpBucketIndex,
						tmpSampleIndex,
						historyCollectionEntryBuff);
	if ((return_code != D_Return_Successfull) &&
	    (return_code != D_EntryNotFound)) {
	    dumpRttMonProcessHeader(ctrlAdminIndex,
				    RttErrorReturnCode);
	    RTT_BUGINF3(ctrlAdminIndex,
			"\t in rttMonOutputHistoryTableData with rc = %u\n",
			return_code);
	    continue;
	}
	tmpCtrlAdminIndex =
	    historyCollectionEntryBuff->rttMonCtrlAdminIndex;
	tmpLifeIndex =
	    historyCollectionEntryBuff->rttMonHistoryCollectionLifeIndex;
	tmpBucketIndex =
	    historyCollectionEntryBuff->rttMonHistoryCollectionBucketIndex;
	tmpSampleIndex =
	    historyCollectionEntryBuff->rttMonHistoryCollectionSampleIndex;
	tmpSampleIndex++;

	if ((return_code != D_Return_Successfull) ||
	    (tmpCtrlAdminIndex != ctrlAdminIndex)) {
	    continue;
	}
	if (outFlag == rtt_output_full) {
	    outputRttMonHistoryCollectionEntry(outEchoAdminEntry,
					       historyCollectionEntryBuff);
	} else {
	    outputRttMonHistoryCollectionEntryCompact(historyCollectionEntryBuff);
	}
    } 
    return;
} 

static void rttMonOutputStatsCollTableData (rttMonEchoAdminEntry_t
					    * outEchoAdminEntry,
					    long ctrlAdminIndex,
					    rttMonStatsCollectEntry_t
					    * statsCollectEntryBuff,
					    rttMonOutputFlag outFlag)
{
    int return_code;
    long tmpCtrlAdminIndex;
    unsigned long tmpStartTime;
    long tmpPathIndex;
    long tmpHopIndex;

    return_code = D_Return_Successfull;
    tmpCtrlAdminIndex = ctrlAdminIndex;
    tmpStartTime = 0;
    tmpPathIndex = 0;
    tmpHopIndex = 0;

    while (return_code == D_Return_Successfull &&
	   tmpCtrlAdminIndex == ctrlAdminIndex) {
	return_code = getNextRttMonStatsCollectEntry(tmpCtrlAdminIndex,
						     tmpStartTime,
						     tmpPathIndex,
						     tmpHopIndex,
						     statsCollectEntryBuff);
	if ((return_code != D_Return_Successfull) &&
	    (return_code != D_EntryNotFound)) {
	    dumpRttMonProcessHeader(ctrlAdminIndex,
				    RttErrorReturnCode);
	    RTT_BUGINF3(ctrlAdminIndex,
			"\t in rttMonOutputHistoryTableData with rc = %u\n",
			return_code);
	    continue;
	}
	tmpCtrlAdminIndex =
	    statsCollectEntryBuff->rttMonCtrlAdminIndex;
	tmpStartTime =
	    statsCollectEntryBuff->rttMonStatsCaptureStartTimeIndex;
	tmpPathIndex =
	    statsCollectEntryBuff->rttMonStatsCapturePathIndex;
	tmpHopIndex =
	    statsCollectEntryBuff->rttMonStatsCaptureHopIndex;
	tmpHopIndex++;

	if ((return_code != D_Return_Successfull) ||
	    (tmpCtrlAdminIndex != ctrlAdminIndex)) {
	    continue;
	}
	if (outFlag == rtt_output_full) {
	    outputRttMonStatsCollectEntry(outEchoAdminEntry,
					  statsCollectEntryBuff);
	} else {
	    outputRttMonStatsCollectEntryCompact(statsCollectEntryBuff);
	}
    } 
    return;
} 

static void rttMonOutputStatsDistTableData (long ctrlAdminIndex,
					    rttMonStatsCaptureEntry_t
					    * statsCaptureEntryBuff,
					    rttMonOutputFlag outFlag)
{
    int return_code;
    long tmpCtrlAdminIndex;
    unsigned long tmpStartTime;
    long tmpPathIndex;
    long tmpHopIndex;
    long tmpDistIndex;

    return_code = D_Return_Successfull;
    tmpCtrlAdminIndex = ctrlAdminIndex;
    tmpStartTime = 0;
    tmpPathIndex = 0;
    tmpHopIndex = 0;
    tmpDistIndex = 0;
    while (return_code == D_Return_Successfull &&
	   tmpCtrlAdminIndex == ctrlAdminIndex) {
	return_code =
	    getNextRttMonStatsCaptureDistEntry(tmpCtrlAdminIndex,
					       tmpStartTime,
					       tmpPathIndex,
					       tmpHopIndex,
					       tmpDistIndex,
					       statsCaptureEntryBuff);
	if ((return_code != D_Return_Successfull) &&
	    (return_code != D_EntryNotFound)) {
	    dumpRttMonProcessHeader(ctrlAdminIndex,
				    RttErrorReturnCode);
	    RTT_BUGINF3(ctrlAdminIndex,
		      "\t in rttMonOutputStatsDistTableData with rc = %u\n",
			return_code);

	    continue;
	}
	tmpCtrlAdminIndex =
	    statsCaptureEntryBuff->rttMonCtrlAdminIndex;
	tmpStartTime =
	    statsCaptureEntryBuff->rttMonStatsCaptureStartTimeIndex;
	tmpPathIndex =
	    statsCaptureEntryBuff->rttMonStatsCapturePathIndex;
	tmpHopIndex =
	    statsCaptureEntryBuff->rttMonStatsCaptureHopIndex;
	tmpDistIndex =
	    statsCaptureEntryBuff->rttMonStatsCaptureDistIndex;
	tmpDistIndex++;

	if ((return_code != D_Return_Successfull) ||
	    (tmpCtrlAdminIndex != ctrlAdminIndex)) {
	    continue;
	}
	if (outFlag == rtt_output_full) {
	    outputRttMonStatsCaptureEntry(statsCaptureEntryBuff);
	} else {
	    outputRttMonStatsCaptureEntryCompact(statsCaptureEntryBuff);
	}
    } 
    return;
} 

static void rttMonOutputStatsTotalsTableData (long ctrlAdminIndex,
					      rttMonStatsTotalsEntry_t
					      * statsTotalsEntryBuff,
					      rttMonOutputFlag outFlag)
{
    int return_code;
    long tmpCtrlAdminIndex;
    unsigned long tmpStartTime;

    return_code = D_Return_Successfull;
    tmpCtrlAdminIndex = ctrlAdminIndex;
    tmpStartTime = 0;

    while (return_code == D_Return_Successfull &&
	   tmpCtrlAdminIndex == ctrlAdminIndex) {
	return_code =
	    getNextRttMonStatsTotalsEntry(tmpCtrlAdminIndex,
					  tmpStartTime,
					  statsTotalsEntryBuff);
	if ((return_code != D_Return_Successfull) &&
	    (return_code != D_EntryNotFound)) {
	    dumpRttMonProcessHeader(ctrlAdminIndex,
				    RttErrorReturnCode);
	    RTT_BUGINF3(ctrlAdminIndex,
		    "\t in rttMonOutputStatsTotalsTableData with rc = %u\n",
			return_code);

	    continue;
	}
	tmpCtrlAdminIndex =
	    statsTotalsEntryBuff->rttMonCtrlAdminIndex;
	tmpStartTime =
	    statsTotalsEntryBuff->rttMonStatsCaptureStartTimeIndex;
	tmpStartTime++;

	if ((return_code != D_Return_Successfull) ||
	    (tmpCtrlAdminIndex != ctrlAdminIndex)) {
	    continue;
	}
	if (outFlag == rtt_output_full) {
	    outputRttMonStatsTotalsEntry(statsTotalsEntryBuff);
	} else {
	    outputRttMonStatsTotalsEntryCompact(statsTotalsEntryBuff);
	}
    } 
    return;
} 

static void rttMonOutputReactTriggerTableData (long ctrlAdminIndex,
					       rttMonReactTriggerAdminEntry_t
					       * reactTriggerAdminEntryBuff,
					       rttMonOutputFlag outFlag)
{
    int return_code;
    long tmpCtrlAdminIndex;
    long tmpReactTriggerAdminRttMonCtrlAdminIndex;

    return_code = D_Return_Successfull;
    tmpCtrlAdminIndex = ctrlAdminIndex;
    tmpReactTriggerAdminRttMonCtrlAdminIndex = 0;

    while (return_code == D_Return_Successfull &&
	   tmpCtrlAdminIndex == ctrlAdminIndex) {
	return_code =
	    getNextRttMonReactTriggerAdminEntry(tmpCtrlAdminIndex,
				   tmpReactTriggerAdminRttMonCtrlAdminIndex,
						reactTriggerAdminEntryBuff);
	if ((return_code != D_Return_Successfull) &&
	    (return_code != D_EntryNotFound)) {
	    dumpRttMonProcessHeader(ctrlAdminIndex,
				    RttErrorReturnCode);
	    RTT_BUGINF3(ctrlAdminIndex,
		   "\t in rttMonOutputReactTriggerTableData with rc = %u\n",
			return_code);

	    continue;
	}
	tmpCtrlAdminIndex =
	    reactTriggerAdminEntryBuff->rttMonCtrlAdminIndex;
	tmpReactTriggerAdminRttMonCtrlAdminIndex =
	    reactTriggerAdminEntryBuff->rttMonReactTriggerAdminRttMonCtrlAdminIndex;
	tmpReactTriggerAdminRttMonCtrlAdminIndex++;

	if ((return_code != D_Return_Successfull) ||
	    (tmpCtrlAdminIndex != ctrlAdminIndex)) {
	    continue;
	}
	if (outFlag == rtt_output_full) {
	    outputRttMonReactTriggerEntry(reactTriggerAdminEntryBuff);
	} else {
	    outputRttMonReactTriggerEntryCompact(reactTriggerAdminEntryBuff);
	}
    } 
    return;
}

#define RTT_DUMP_CTRLADMIN_TABLE          1
#define RTT_DUMP_OPER_TABLE               2
#define RTT_DUMP_HISTORY_TABLE            3
#define RTT_DUMP_COLLECT_TABLE            4
#define RTT_DUMP_DIST_TABLE               5
#define RTT_DUMP_TOTALS_TABLE             6
#define RTT_DUMP_REACT_TRIGGER_TABLE      7 

static void rttMonOutputTable (long tableType,
			       long ctrlAdminIndex,
			       rttMonOutputFlag outFlag)
{
    int rc;
    boolean all = FALSE;

    long tmpCtrlAdminIndex;
    rttMonCtrlAdminEntry_t *outCtrlAdminEntry = NULL;
    rttMonEchoAdminEntry_t *outEchoAdminEntry = NULL;
    rttMonStatsTotalsEntry_t *statsTotalsEntryBuff = NULL;
    rttMonStatsCaptureEntry_t *statsCaptureEntryBuff = NULL;
    rttMonStatsCollectEntry_t *statsCollectEntryBuff = NULL;
    rttMonHistoryCollectionEntry_t *historyCollectionEntryBuff = NULL;
    rttMonReactTriggerAdminEntry_t *reactTriggerAdminEntryBuff = NULL;

    outCtrlAdminEntry = newRttMonCtrlAdminEntry();
    if (outCtrlAdminEntry == NULL) {
	printf("Failed:  Low Memory\n");
	return;
    }
    outEchoAdminEntry = newRttMonEchoAdminEntry();
    if (outEchoAdminEntry == NULL) {
	printf("Failed:  Low Memory\n");
	freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	return;
    }
    /* allocate needed buffers upfront */ 
    switch (tableType) {
    case RTT_DUMP_HISTORY_TABLE:
	historyCollectionEntryBuff = newRttMonHistoryCollectionEntry();
	if (historyCollectionEntryBuff == NULL) {
	    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	    freeRttMonEchoAdminEntry(outEchoAdminEntry);
	    return;
	}
	break;
    case RTT_DUMP_COLLECT_TABLE:
	statsCollectEntryBuff = newRttMonStatsCollectEntry();
	if (statsCollectEntryBuff == NULL) {
	    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	    freeRttMonEchoAdminEntry(outEchoAdminEntry);
	    freeRttMonHistoryCollectionEntry(historyCollectionEntryBuff);
	    return;
	}
	break;
    case RTT_DUMP_DIST_TABLE:
	statsCaptureEntryBuff = newRttMonStatsCaptureDistEntry();
	if (statsCaptureEntryBuff == NULL) {
	    printf("Failed:  Low Memory\n");
	    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	    freeRttMonEchoAdminEntry(outEchoAdminEntry);
	    freeRttMonStatsCollectEntry(statsCollectEntryBuff);
	    freeRttMonHistoryCollectionEntry(historyCollectionEntryBuff);
	    return;
	}
	break;
    case RTT_DUMP_TOTALS_TABLE:
	statsTotalsEntryBuff = newRttMonStatsTotalsEntry();
	if (statsTotalsEntryBuff == NULL) {
	    printf("Failed:  Low Memory\n");
	    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	    freeRttMonEchoAdminEntry(outEchoAdminEntry);
	    freeRttMonStatsCaptureDistEntry(statsCaptureEntryBuff);
	    freeRttMonStatsCollectEntry(statsCollectEntryBuff);
	    freeRttMonHistoryCollectionEntry(historyCollectionEntryBuff);
	    return;
	}
	break;
    case RTT_DUMP_REACT_TRIGGER_TABLE:
	reactTriggerAdminEntryBuff =
	    newRttMonReactTriggerAdminEntry();
	if (reactTriggerAdminEntryBuff == NULL) {
	    printf("Failed:  Low Memory\n");
	    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	    freeRttMonEchoAdminEntry(outEchoAdminEntry);
	    freeRttMonStatsCaptureDistEntry(statsCaptureEntryBuff);
	    freeRttMonStatsCollectEntry(statsCollectEntryBuff);
	    freeRttMonHistoryCollectionEntry(historyCollectionEntryBuff);
	    freeRttMonReactTriggerAdminEntry(reactTriggerAdminEntryBuff);
	    return;
	}
	break;
    default:
	break;
    }
    if (ctrlAdminIndex > 0) {
	tmpCtrlAdminIndex = ctrlAdminIndex;
    } else {
	tmpCtrlAdminIndex = 0;
	all = TRUE;
    } 
    if (!all) {
	rc = getRttMonCtrlAdminEntry(tmpCtrlAdminIndex,
				     outCtrlAdminEntry);
	if (rc != D_Return_Successfull) {
	    printf("Failed: Unknown entry number\n");
	    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
	    freeRttMonEchoAdminEntry(outEchoAdminEntry);
	    return;
	}
    } else {
	rc = getNextRttMonCtrlAdminEntry(tmpCtrlAdminIndex,
					 outCtrlAdminEntry);
    }
    /*
     * No -more- when in raw mode
     */ 
    if (outFlag != rtt_output_raw) {
	automore_enable(NULL);
    } else {
	automore_disable();
    } 
    rc = getNextRttMonEchoAdminEntry(tmpCtrlAdminIndex,
				       outEchoAdminEntry);
    switch (tableType) {
    case RTT_DUMP_CTRLADMIN_TABLE:
	/* 
	 * No headers in raw mode
	 */
	if (outFlag == rtt_output_full) {
	    outputRttMonAdminTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonAdminTableHeadCompact();
	}
	break;
    case RTT_DUMP_OPER_TABLE:
	/*
	 * No headers in raw mode
	 */
	if (outFlag == rtt_output_full) {
	    outputRttMonOperTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonOperTableHeadCompact();
	}
	break;
    case RTT_DUMP_HISTORY_TABLE:
	if (outFlag == rtt_output_full) {
	    outputRttMonHistoryCollectionTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonHistoryCollectionTableHeadCompact();
	}
	break;
    case RTT_DUMP_COLLECT_TABLE:
	if (outFlag == rtt_output_full) {
	    outputRttMonStatsCollectTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonStatsCollectTableHeadCompact();
	}
	break;
    case RTT_DUMP_DIST_TABLE:
	if (outFlag == rtt_output_full) {
	    outputRttMonStatsCaptureTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonStatsCaptureTableHeadCompact();
	}
	break;
    case RTT_DUMP_TOTALS_TABLE:
	if (outFlag == rtt_output_full) {
	    outputRttMonStatsTotalsTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonStatsTotalsTableHeadCompact();
	}
	break;
    case RTT_DUMP_REACT_TRIGGER_TABLE:
	if (outFlag == rtt_output_full) {
	    outputRttMonReactTriggerTableHead();
	} else if (outFlag == rtt_output_brief) {
	    outputRttMonReactTriggerTableHeadCompact();
	}
	break;
    }
    while (rc == D_Return_Successfull) {
	switch (tableType) {
	case RTT_DUMP_CTRLADMIN_TABLE:
	    if (outFlag == rtt_output_full) {
		outputRttMonAdminEntry(outCtrlAdminEntry,
				       outEchoAdminEntry);
	    } else {
		outputRttMonAdminEntryCompact(outCtrlAdminEntry,
					      outEchoAdminEntry);
	    }
	    break;
	case RTT_DUMP_OPER_TABLE:
	    if (outFlag == rtt_output_full) {
		outputRttMonOperEntry(outEchoAdminEntry,
				      outCtrlAdminEntry);
	    } else {
		outputRttMonOperEntryCompact(outCtrlAdminEntry);
	    }
	    break;
	case RTT_DUMP_HISTORY_TABLE:
	    rttMonOutputHistoryTableData(outEchoAdminEntry,
				    outCtrlAdminEntry->rttMonCtrlAdminIndex,
				       historyCollectionEntryBuff, outFlag);
	    break;
	case RTT_DUMP_COLLECT_TABLE:
	    rttMonOutputStatsCollTableData(outEchoAdminEntry,
				    outCtrlAdminEntry->rttMonCtrlAdminIndex,
					   statsCollectEntryBuff, outFlag);
	    break;
	case RTT_DUMP_DIST_TABLE:
	    rttMonOutputStatsDistTableData(outCtrlAdminEntry->rttMonCtrlAdminIndex,
					   statsCaptureEntryBuff, outFlag);
	    break;
	case RTT_DUMP_TOTALS_TABLE:
	    rttMonOutputStatsTotalsTableData(outCtrlAdminEntry->rttMonCtrlAdminIndex,
					     statsTotalsEntryBuff, outFlag);
	    break;
	case RTT_DUMP_REACT_TRIGGER_TABLE:
	    rttMonOutputReactTriggerTableData(outCtrlAdminEntry->rttMonCtrlAdminIndex,
				       reactTriggerAdminEntryBuff, outFlag);
	    break;
	} 
	if (!all) {
	    break;
	}
	tmpCtrlAdminIndex =
	    outCtrlAdminEntry->
	    rttMonCtrlAdminIndex + 1;
	rc = getNextRttMonCtrlAdminEntry(tmpCtrlAdminIndex,
					 outCtrlAdminEntry);
	rc = getNextRttMonEchoAdminEntry(tmpCtrlAdminIndex,
					 outEchoAdminEntry);
    } 
    if (outFlag != rtt_output_raw) {
	automore_disable();
    }
    freeRttMonCtrlAdminEntry(outCtrlAdminEntry);
    freeRttMonEchoAdminEntry(outEchoAdminEntry);
    freeRttMonStatsTotalsEntry(statsTotalsEntryBuff);
    freeRttMonStatsCaptureDistEntry(statsCaptureEntryBuff);
    freeRttMonStatsCollectEntry(statsCollectEntryBuff);
    freeRttMonHistoryCollectionEntry(historyCollectionEntryBuff);
    freeRttMonReactTriggerAdminEntry(reactTriggerAdminEntryBuff);

    return;

} 

void rttMonOutputCtrlTable (long ctrlAdminIndex,
			    boolean indexExists,
			    rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_CTRLADMIN_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

void rttMonOutputOperTable (long ctrlAdminIndex,
			    boolean indexExists,
			    rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_OPER_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

void rttMonOutputHistoryTable (long ctrlAdminIndex,
			       boolean indexExists,
			       rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_HISTORY_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

void rttMonOutputStatsCollTable (long ctrlAdminIndex,
				 boolean indexExists,
				 rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_COLLECT_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

void rttMonOutputStatsDistTable (long ctrlAdminIndex,
				 boolean indexExists,
				 rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_DIST_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

void rttMonOutputStatsTotalsTable (long ctrlAdminIndex,
				   boolean indexExists,
				   rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_TOTALS_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

void rttMonOutputReactTriggerTable (long ctrlAdminIndex,
				    boolean indexExists,
				    rttMonOutputFlag outFlag)
{
    rttMonOutputTable(RTT_DUMP_REACT_TRIGGER_TABLE,
		      (indexExists) ? ctrlAdminIndex : 0,
		      outFlag);
    return;
} 

/*  real registry functions */ 

static boolean rtt_exists_default (void)
{
    return (FALSE);
} 

static int rtt_validate_default (rttMonCtrlAdminEntry_t * echoAdminEntry,
				 void *specificEntry,
				 int rttObject)
{
    return (D_INVALID_RttEntry);
} 

void registerRtt ()
{
    rtt_init_debug();
    reg_add_default_rtt_echo_validate(rtt_validate_default,
				      "rtt_validate_default");
    reg_add_default_rtt_echo_exists(rtt_exists_default,
				    "rtt_exists_default");
    reg_add_dumpRttMonLibReturnCode(dumpRttMonLibReturnCode,
				    "dumpRttMonLibReturnCode");
    reg_add_dumpRttMonProcessHeader(dumpRttMonProcessHeader,
				    "dumpRttMonProcessHeader");
    reg_add_dumpRttMonErrorReturnCode(dumpRttMonErrorReturnCode,
				      "dumpRttMonErrorReturnCode");
    reg_add_dumpRttMonCtrlAdminRttType(dumpRttMonCtrlAdminRttType,
				       "dumpRttMonCtrlAdminRttType");
    reg_add_dumpRttMonEchoAdminTargetAddress(dumpRttMonEchoAdminTargetAddress,
					"dumpRttMonEchoAdminTargetAddress");
    reg_add_dumpRttMonCtrlAdminQItem(dumpRttMonCtrlAdminQItem,
				     "dumpRttMonCtrlAdminQItem");
    reg_add_default_rttMonStartLife(rttMonStartLife,
				    "rttMonStartLife");
    reg_add_rttMonConvertAsciiToEbcdic(rttMonConvertAsciiToEbcdic,
				       "rttMonConvertAsciiToEbcdic");
    reg_add_rttMonIPToOctetString(rttMonIPToOctetString,
				  "rttMonIPToOctetString");
    reg_add_rttMonFreeOctetStringBuffer(rttMonFreeOctetStringBuffer,
					"rttMonFreeOctetStringBuffer");
    reg_add_default_rttMonPreMallocOctetString(rttMonPreMallocOctetString,
					       "rttMonPreMallocOctetString");
    reg_add_default_rttMonNewAddressBuffer(rttMonNewAddressBuffer,
					   "rttMonNewAddressBuffer");
    reg_add_default_getRttMonCtrlAdminRttType(getRttMonCtrlAdminRttType,
					      "getRttMonCtrlAdminRttType");
    reg_add_default_getRttMonEchoAdminProtocol(getRttMonEchoAdminProtocol,
					       "getRttMonEchoAdminProtocol");
    reg_add_default_getRttMonEchoAdminTargetAddress(getRttMonEchoAdminTargetAddress,
					 "getRttMonEchoAdminTargetAddress");
    reg_add_default_getRttMonEchoAdminPktDataRequestSize(getRttMonEchoAdminPktDataRequestSize,
				    "getRttMonEchoAdminPktDataRequestSize");
    reg_add_default_getRttMonEchoAdminPktDataResponseSize(getRttMonEchoAdminPktDataResponseSize,
				   "getRttMonEchoAdminPktDataResponseSize");
    reg_add_default_rttMonUpdateResponse(rttMonUpdateResponse,
					 "rttMonUpdateResponse");
    reg_add_default_rttMonCollectPath(rttMonCollectPath,
				      "rttMonCollectPath");
    reg_add_rttMonStartUpdatePath(rttMonStartUpdatePath,
				  "rttMonStartUpdatePath");
    reg_add_default_rttMonUpdatePath(rttMonUpdatePath,
				     "rttMonUpdatePath");
    reg_add_default_rttMonEndUpdatePath(rttMonEndUpdatePath,
					"rttMonEndUpdatePath");
    reg_add_default_rttMonGetFirstStoredHopAddress(rttMonGetFirstStoredHopAddress,
					  "rttMonGetFirstStoredHopAddress");
    reg_add_default_rttMonGetNextStoredHopAddress(rttMonGetNextStoredHopAddress,
					   "rttMonGetNextStoredHopAddress");
    reg_add_default_rttMonSetupASCIIDataPortion(rttMonSetupASCIIDataPortion,
					     "rttMonSetupASCIIDataPortion");
    reg_add_default_rttMonSetupEBCDICDataPortion(rttMonSetupEBCDICDataPortion,
					    "rttMonSetupEBCDICDataPortion");
    reg_add_default_rttMonVerifyASCIIDataPortion(rttMonVerifyASCIIDataPortion,
					    "rttMonVerifyASCIIDataPortion");
    reg_add_default_rttMonVerifyEBCDICDataPortion(rttMonVerifyEBCDICDataPortion,
					   "rttMonVerifyEBCDICDataPortion");
    reg_add_default_getRttMonCtrlAdminFrequency(getRttMonCtrlAdminFrequency,
					     "getRttMonCtrlAdminFrequency");
    reg_add_default_getRttMonCtrlAdminTimeout(getRttMonCtrlAdminTimeout,
					      "getRttMonCtrlAdminTimeout");
    reg_add_default_getRttMonOperState(getRttMonOperState,
				       "getRttMonOperState");
    reg_add_default_rttMonTransitionTransitionState(rttMonTransitionTransitionState,
					 "rttMonTransitionTransitionState");
    reg_add_rttMonTimeStampEchoStart(rttMonTimeStampEchoStart,
				     "rttMonTimeStampEchoStart");
    reg_add_rttMonUpdateNumberOfEchosAttempted(rttMonUpdateNumberOfEchosAttempted,
				      "rttMonUpdateNumberOfEchosAttempted");
    reg_add_default_rttMonHopConnected(rttMonHopConnected,
				       "rttMonHopConnected");
    reg_add_default_rttMonSetHopConnectedState(rttMonSetHopConnectedState,
					       "rttMonSetHopConnectedState");
    reg_add_default_rttMonSetConnectionHandle(rttMonSetConnectionHandle,
					      "rttMonSetConnectionHandle");
    reg_add_default_rttMonGetConnectionHandle(rttMonGetConnectionHandle,
					      "rttMonGetConnectionHandle");
    reg_add_rttMonFreeConnectionHandleList(rttMonFreeConnectionHandleList,
					   "rttMonFreeConnectionHandleList");
    reg_add_default_rttMonNewConnectionHandleList(rttMonNewConnectionHandleList,
					   "rttMonNewConnectionHandleList");
    reg_add_default_rttMonGetConnectionHandleList(rttMonGetConnectionHandleList,
					   "rttMonGetConnectionHandleList");
    reg_add_default_rttMonSetDiagText(rttMonSetDiagText,
				      "rttMonSetDiagText");
    reg_add_default_rttMonGetVerifyDataFlag(rttMonGetVerifyDataFlag,
					    "rttMonGetVerifyDataFlag");
    reg_add_default_rttMonGetPendingBoolean(rttMonGetPendingBoolean,
					    "rttMonGetPendingBoolean");
    reg_add_default_rttMonCalculateResponseTime(rttMonCalculateResponseTime,
					     "rttMonCalculateResponseTime");
    reg_add_rttMonProbePanic(rttMonProbePanic,
			     "rttMonProbePanic");
    reg_add_rttMonProbePanicInternal(rttMonProbePanicInternal,
				     "rttMonProbePanicInternal");
    reg_add_rttMonSetSelfID(rttMonSetSelfID,
			    "rttMonSetSelfID");

    return;
}

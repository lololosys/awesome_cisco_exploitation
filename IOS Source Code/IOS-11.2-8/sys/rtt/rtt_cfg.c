/* $Id: rtt_cfg.c,v 1.1.4.5 1996/08/28 13:08:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_cfg.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Configuration Command Utilities.
 *
 * March 1996, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor Parser Configuration Command
 *   utilities.
 *------------------------------------------------------------------
 * $Log: rtt_cfg.c,v $
 * Revision 1.1.4.5  1996/08/28  13:08:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.4  1996/06/07  22:25:55  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/06/04  18:28:21  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.2  1996/05/27  05:48:51  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:24  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:19  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:31  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:23  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>
#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "logger.h"
#include "../rtt/msg_rtt.c"		/* Not a typo, see logger.h */
#include "rtt_util_ext.h"
#include "rtt_lib.h"
#include "parser.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_priority.h"
#include "../parser/parser_actions.h"
#include "rtt_show.h"
#include "rtt_debug_ext.h"
#include "parser_defs_rtt.h"
#include "../h/ttysrv.h"
#include "../ip/ip.h"

static boolean NoBuffers;
static rttMonCtrlAdminEntry_t *rttMonCtrlAdminEntryData;
static rttMonEchoAdminEntry_t *rttMonEchoAdminEntryData;

static watched_semaphore *rttMonConfigSemaphore;

void rttmon_init_config_buffers (void)
{
    rttMonCtrlAdminEntryData =
	newRttMonCtrlAdminEntry();
    if (rttMonCtrlAdminEntryData == NULL) {
	errmsg(&msgsym(BufferInitFailed, RTT));
	NoBuffers = TRUE;
	return;
    }
    rttMonEchoAdminEntryData =
	newRttMonEchoAdminEntry();
    if (rttMonEchoAdminEntryData == NULL) {
	errmsg(&msgsym(BufferInitFailed, RTT));
	freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryData);
	NoBuffers = TRUE;
	return;
    }
    rttMonConfigSemaphore =
	create_watched_semaphore("rttMonConfigSemaphore", 0);
    if (rttMonConfigSemaphore == NULL) {
	errmsg(&msgsym(SemaphoreInitFailed, RTT));
	rttMonConfigSemaphore = NULL;
    }
    NoBuffers = FALSE;
    return;
}

/*
 * Configuration Routines
 */

#define MAXLEN 80 

void rttmon_config_base_command (parseinfo * csb)
{
    rttMonReactTriggerAdminEntry_t rttMonReactTriggerAdminEntryData;
    long rttMonReactTriggerCtrlAdminIndex;
    long rttMonCtrlAdminIndex;
    int return_code;
    char *hostname;
    char *applname;
    char *modename;
    long OctetStringLen;
    clock_epoch start_epoch_time, last_midnight, boot_epoch, now_epoch;
    timeval startTv;
    long startTime = 0;
    boolean secondaryMemoryForTables;
    boolean parsed = FALSE;
    static char buff[MAXLEN];
    rttMonCtrlAdminQItem *rttMonCtrlAdminQItemPtr;

    return_code = D_Return_Successfull;

    if (NoBuffers) {
	errmsg(&msgsym(MissingInit, RTT));
    }
    if (csb->nvgen) {
	if (csb->which == RTR_CFG_ENTRY_CMD) {
	    /*
	     *  Do all the Base Config 
	     *
	     * Get Next CtrlAdminEntry and EchoAdminEntry
	     *  starting from zeroth entry, ie first one,
	     *  use these values to generate the following
	     *  string.  Make sure this string is the same
	     *  as the one used when not generating NVRAM.
	     */
	    rttMonCtrlAdminIndex = 0;
	    return_code =
		getNextRttMonCtrlAdminEntry(rttMonCtrlAdminIndex,
					    rttMonCtrlAdminEntryData);
	    if (return_code == D_Return_Successfull) {
		/* It is ok to overwrite the return_code */
		return_code =
		    getRttMonEchoAdminEntry(rttMonCtrlAdminEntryData->
					    rttMonCtrlAdminIndex,
					    rttMonEchoAdminEntryData);
	    } else {
		return;
	    }
	    if (return_code == D_Return_Successfull) {
		nv_write(TRUE, "!");
	    } else {
		errmsg(&msgsym(MissingEchoStruct, RTT));
	    }
	    
	    while (return_code == D_Return_Successfull) {
		nv_write(TRUE, "%s %u",
			 csb->nv_command,
			 rttMonCtrlAdminEntryData->
			 rttMonCtrlAdminIndex);
		nv_write(TRUE, " %s %s %s ",
			 D_rttMonCtrlAdminRttType_string,
			 (rttMonCtrlAdminEntryData->
			  rttMonCtrlAdminRttType ==
			  D_rttMonCtrlAdminRttType_echo) ?
			 D_rttMonCtrlAdminRttType_echo_string :
			 D_rttMonCtrlAdminRttType_pathEcho_string,
			 D_rttMonEchoAdminProtocol_string);
		switch (rttMonEchoAdminEntryData->
			rttMonEchoAdminProtocol) {
		case D_rttMonEchoAdminProtocol_ipIcmpEcho:
		    nv_add(TRUE, "%s %i",
			   D_rttMonEchoAdminProtocol_ipIcmpEcho_string,
			   reg_invoke_OctetStringToIP(rttMonEchoAdminEntryData->
						      rttMonEchoAdminTargetAddress));
		    break;
		case D_rttMonEchoAdminProtocol_snaRUEcho:
		    nv_add(TRUE, "%s ",
			   D_rttMonEchoAdminProtocol_snaRUEcho_string);
		    OctetStringLen = 0;
		    while (OctetStringLen <
			   rttMonEchoAdminEntryData->
			   rttMonEchoAdminTargetAddress->
			   length) {
			nv_add(TRUE, "%c",
			       rttMonEchoAdminEntryData->
			       rttMonEchoAdminTargetAddress->
			       octet_ptr[OctetStringLen]);
			OctetStringLen++;
		    } 
		    break;
		case D_rttMonEchoAdminProtocol_snaLU0EchoAppl:
		case D_rttMonEchoAdminProtocol_snaLU2EchoAppl:
		    reg_invoke_rtt_depend_lu_split_address(rttMonEchoAdminEntryData->
							   rttMonEchoAdminTargetAddress,
							   &hostname,
							   &OctetStringLen,
							   &applname,
							   &modename,
							   rttMonCtrlAdminEntryData->
							   rttMonCtrlAdminIndex);
		    nv_add(TRUE, "%s %s",
			   (rttMonEchoAdminEntryData->
			    rttMonEchoAdminProtocol ==
			    D_rttMonEchoAdminProtocol_snaLU0EchoAppl) ?
			   D_rttMonEchoAdminProtocol_snaLU0EchoAppl_string :
			   D_rttMonEchoAdminProtocol_snaLU2EchoAppl_string,
			   hostname);
		    if (strcmp(applname, DEF_rttMonDependLU_App_Name)) {
			nv_add(TRUE, " %s %s",
			       D_rttMonEchoAdminProtocolSnaApplName_string,
			       applname);
		    }
		    if (strcmp(modename, "        ")) {
			nv_add(TRUE, " %s %s",
			       D_rttMonEchoAdminProtocolSnaModeName_string,
			       modename);
		    }
		    free(hostname);
		    free(applname);
		    free(modename);
		    break;
		} /* switch */ 
		if ((strlen(DEF_rttMonCtrlAdminOwner) !=
		     rttMonCtrlAdminEntryData->
		     rttMonCtrlAdminOwner->
		     length) ||
		    ((strlen(DEF_rttMonCtrlAdminOwner) ==
		      rttMonCtrlAdminEntryData->
		      rttMonCtrlAdminOwner->
		      length) &&
		     strncmp(rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminOwner->
			     octet_ptr,
			     DEF_rttMonCtrlAdminOwner,
			     strlen(DEF_rttMonCtrlAdminOwner)))) {
		    nv_write(TRUE, " %s ",
			     D_rttMonCtrlAdminOwner_string);

		    OctetStringLen = 0;
		    while (OctetStringLen <
			   rttMonCtrlAdminEntryData->
			   rttMonCtrlAdminOwner->
			   length) {
			nv_add(TRUE, "%c",
			       rttMonCtrlAdminEntryData->
			       rttMonCtrlAdminOwner->
			       octet_ptr[OctetStringLen]);
			OctetStringLen++;

		    }
		} /* if (owner */ 
		if ((strlen(DEF_rttMonCtrlAdminTag) !=
		     rttMonCtrlAdminEntryData->
		     rttMonCtrlAdminTag->
		     length) ||
		    ((strlen(DEF_rttMonCtrlAdminTag) ==
		      rttMonCtrlAdminEntryData->
		      rttMonCtrlAdminTag->
		      length) &&
		     strncmp(rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminTag->
			     octet_ptr,
			     DEF_rttMonCtrlAdminTag,
			     strlen(DEF_rttMonCtrlAdminTag)))) {
		    nv_write(TRUE, " %s ",
			     D_rttMonCtrlAdminTag_string);
		    OctetStringLen = 0;
		    while (OctetStringLen <
			   rttMonCtrlAdminEntryData->
			   rttMonCtrlAdminTag->
			   length) {
			nv_add(TRUE, "%c",
			       rttMonCtrlAdminEntryData->
			       rttMonCtrlAdminTag->
			       octet_ptr[OctetStringLen]);
			OctetStringLen++;
		    }
		} /* if (tag */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonCtrlAdminThreshold !=
		    DEF_rttMonCtrlAdminThreshold) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonCtrlAdminThreshold_string,
			     rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminThreshold);
		} /* if (threshold */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonCtrlAdminFrequency !=
		    DEF_rttMonCtrlAdminFrequency) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonCtrlAdminFrequency_string,
			     rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminFrequency);
		} /* if (frequency */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonCtrlAdminTimeout !=
		    DEF_rttMonCtrlAdminTimeout) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonCtrlAdminTimeout_string,
			     rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminTimeout);
		} /* if (timeout */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonCtrlAdminVerifyData !=
		    DEF_rttMonCtrlAdminVerifyData) {
		    nv_write(TRUE, " %s",
			     D_rttMonCtrlAdminVerifyData_string);
		} /* if (verify-data */ 
		if (rttMonEchoAdminEntryData->
		    rttMonEchoAdminPktDataRequestSize !=
		    DEF_rttMonEchoAdminPktDataRequestSize) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonEchoAdminPktDataRequestSize_string,
			     rttMonEchoAdminEntryData->
			     rttMonEchoAdminPktDataRequestSize);
		} /* if (request-data-size */ 
		if (rttMonEchoAdminEntryData->
		    rttMonEchoAdminPktDataResponseSize !=
		    DEF_rttMonEchoAdminPktDataResponseSize) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonEchoAdminPktDataResponseSize_string,
			     rttMonEchoAdminEntryData->
			     rttMonEchoAdminPktDataResponseSize);
		} /* if (response-data-size */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonStatisticsAdminNumHourGroups !=
		    DEF_rttMonStatisticsAdminNumHourGroups) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonStatisticsAdminNumHourGroups_string,
			     rttMonCtrlAdminEntryData->
			     rttMonStatisticsAdminNumHourGroups);
		} /* if (number-of-hours-of-statistics */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonStatisticsAdminNumPaths !=
		    DEF_rttMonStatisticsAdminNumPaths) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonStatisticsAdminNumPaths_string,
			     rttMonCtrlAdminEntryData->
			     rttMonStatisticsAdminNumPaths);
		} /* if (number-of-paths-of-statistics */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonStatisticsAdminNumHops !=
		    DEF_rttMonStatisticsAdminNumHops) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonStatisticsAdminNumHops_string,
			     rttMonCtrlAdminEntryData->
			     rttMonStatisticsAdminNumHops);
		} /* if (number-of-hops-of-statistics */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonStatisticsAdminNumDistBuckets !=
		    DEF_rttMonStatisticsAdminNumDistBuckets) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonStatisticsAdminNumDistBuckets_string,
			     rttMonCtrlAdminEntryData->
			     rttMonStatisticsAdminNumDistBuckets);
		} /* if (number-of-distributions-of-statistics */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonStatisticsAdminDistInterval !=
		    DEF_rttMonStatisticsAdminDistInterval) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonStatisticsAdminDistInterval_string,
			     rttMonCtrlAdminEntryData->
			     rttMonStatisticsAdminDistInterval);
		} /* if (statistics-distribution-interval */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonHistoryAdminNumLives !=
		    DEF_rttMonHistoryAdminNumLives) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonHistoryAdminNumLives_string,
			     rttMonCtrlAdminEntryData->
			     rttMonHistoryAdminNumLives);
		} /* if (number-of-lives-of-history */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonHistoryAdminNumBuckets !=
		    DEF_rttMonHistoryAdminNumBuckets) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonHistoryAdminNumBuckets_string,
			     rttMonCtrlAdminEntryData->
			     rttMonHistoryAdminNumBuckets);
		} /* if (number-of-buckets-of-history */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonHistoryAdminNumSamples !=
		    DEF_rttMonHistoryAdminNumSamples) {
		    nv_write(TRUE, " %s %u",
			     D_rttMonHistoryAdminNumSamples_string,
			     rttMonCtrlAdminEntryData->
			     rttMonHistoryAdminNumSamples);
		} /* if (number-of-samples-of-history */ 
		if (rttMonCtrlAdminEntryData->
		    rttMonHistoryAdminFilter !=
		    DEF_rttMonHistoryAdminFilter) {
		    nv_write(TRUE, " %s ",
			     D_rttMonHistoryAdminFilter_string);
		    switch (rttMonCtrlAdminEntryData->
			    rttMonHistoryAdminFilter) {
		    case D_rttMonHistoryAdminFilter_none:
			nv_add(TRUE, "%s",
			       D_rttMonHistoryAdminFilter_none_string);
			break;
		    case D_rttMonHistoryAdminFilter_all:
			nv_add(TRUE, "%s",
			       D_rttMonHistoryAdminFilter_all_string);
			break;
		    case D_rttMonHistoryAdminFilter_overThreshold:
			nv_add(TRUE, "%s",
			       D_rttMonHistoryAdminFilter_overThreshold_string);
			break;
		    case D_rttMonHistoryAdminFilter_failures:
			nv_add(TRUE, "%s",
			       D_rttMonHistoryAdminFilter_failures_string);
			break;
		    }
		} /* if (filter-for-history */
		/*
		 * Do the schedule Config
		 */ 
		secondaryMemoryForTables = FALSE;
		/*
		 *  Ignore the Return Code for Secondary Mem because, if
		 *   if fails we have already set the result to FALSE in
		 *   the line above, and that is the default behavior
		 */
		rttMonCtrlAdminQItemSecondaryMemory(rttMonCtrlAdminEntryData->
						    rttMonCtrlAdminIndex,
						    &secondaryMemoryForTables);
		/* 
		 * If no secondaryMem exists then the config for this
		 *  entry failed due to a lack of memory.  Don't
		 *  output any schedule time, since it cannot start
		 */
		if (secondaryMemoryForTables) {
		    nv_write(TRUE, "%s %s %u",
			     D_rttMon_string,
			     D_rttMonSchedule_string,
			     rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminIndex);
		    if (rttMonCtrlAdminEntryData->
			rttMonScheduleAdminRttLife !=
			DEF_rttMonScheduleAdminRttLife) {
			nv_add(TRUE, " %s %u",
			       D_rttMonScheduleAdminRttLife_string,
			       rttMonCtrlAdminEntryData->
			       rttMonScheduleAdminRttLife);
		    }
		    nv_add(TRUE, " %s ",
			   D_rttMonScheduleAdminRttStartTime_string);
		    if (rttMonCtrlAdminEntryData->
			rttMonScheduleAdminRttStartTime == 1) {
			nv_add(TRUE, "%s",
			       D_rttMonScheduleAdminRttStartTime_now_string);
		    } else if (rttMonCtrlAdminEntryData->
			       rttMonScheduleAdminRttStartTime == 0) {
			nv_add(TRUE, "%s",
			       D_rttMonScheduleAdminRttStartTime_pending_string);
		    } else {
    			rttMon_get_boot_epoch(&boot_epoch);
			/* 
			 * StartTime is sysuptime (centiseconds) since
			 *  the router booted, thus convert to seconds 
			 *  eg. 500 centiseconds / 100 = 5 seconds 
			 */
			start_epoch_time.epoch_secs =
			    rttMonCtrlAdminEntryData->
				rttMonScheduleAdminRttStartTime / 
				    SYSUPTIME_UNITS;
			start_epoch_time.epoch_secs += boot_epoch.epoch_secs;
			start_epoch_time.epoch_frac = 0;
			nv_add(TRUE, "%CC", "%H:%M %tn %td",
			       &start_epoch_time);
		    }
		    if (rttMonCtrlAdminEntryData->
			rttMonScheduleAdminConceptRowAgeout !=
			CLI_DEF_rttMonScheduleAdminConceptRowAgeout) {
			nv_add(TRUE, " %s %u",
			       D_rttMonScheduleAdminConceptRowAgeout_string,
			       rttMonCtrlAdminEntryData->
			       rttMonScheduleAdminConceptRowAgeout);
		    }
		} /* if (secondaryMemoryForTables) { */
		/*
		 * Do the reaction-configuration Config
		 */
		if ((rttMonCtrlAdminEntryData->
		     rttMonReactAdminConnectionEnable !=
		     DEF_rttMonReactAdminConnectionEnable) ||
		    (rttMonCtrlAdminEntryData->
		     rttMonReactAdminTimeoutEnable !=
		     DEF_rttMonReactAdminTimeoutEnable) ||
		    (rttMonCtrlAdminEntryData->
		     rttMonReactAdminThresholdFalling !=
		     DEF_rttMonReactAdminThresholdFalling) ||
		    (rttMonCtrlAdminEntryData->
		     rttMonReactAdminThresholdType !=
		     DEF_rttMonReactAdminThresholdType) ||
		    (rttMonCtrlAdminEntryData->
		     rttMonReactAdminActionType !=
		     DEF_rttMonReactAdminActionType)) {
		    nv_write(TRUE, "%s %s %u ",
			     D_rttMon_string,
			     D_rttMonReactAdmin_string,
			     rttMonCtrlAdminEntryData->
			     rttMonCtrlAdminIndex);
		    if (rttMonCtrlAdminEntryData->
			rttMonReactAdminConnectionEnable !=
			DEF_rttMonReactAdminConnectionEnable) {
			nv_add(TRUE, "%s ",
			       D_rttMonReactAdminConnectionEnable_string);
		    } /* timeout enable */ 
		    if (rttMonCtrlAdminEntryData->
			rttMonReactAdminTimeoutEnable !=
			DEF_rttMonReactAdminTimeoutEnable) {
			nv_add(TRUE, "%s ",
			       D_rttMonReactAdminTimeoutEnable_string);
		    } /* timeout enable */ 
		    if (rttMonCtrlAdminEntryData->
			rttMonReactAdminThresholdFalling !=
			DEF_rttMonReactAdminThresholdFalling) {
			nv_add(TRUE, "%s %u ",
			       D_rttMonReactAdminThresholdFalling_string,
			       rttMonCtrlAdminEntryData->
			       rttMonReactAdminThresholdFalling);
		    }
		    if (rttMonCtrlAdminEntryData->
			rttMonReactAdminThresholdType !=
			DEF_rttMonReactAdminThresholdType) {
			nv_add(TRUE, "%s ",
			       D_rttMonReactAdminThresholdType_string);
			switch (rttMonCtrlAdminEntryData->
				rttMonReactAdminThresholdType) {
			case D_rttMonReactAdminThresholdType_never:
			    nv_add(TRUE, "%s ",
				   D_rttMonReactAdminThresholdType_never_string);
			    break;
			case D_rttMonReactAdminThresholdType_immediate:
			    nv_add(TRUE, "%s ",
				   D_rttMonReactAdminThresholdType_immediate_string);
			    break;
			case D_rttMonReactAdminThresholdType_consecutive:
			    nv_add(TRUE, "%s ",
				   D_rttMonReactAdminThresholdType_consecutive_string);
			    if (rttMonCtrlAdminEntryData->
				rttMonReactAdminThresholdCount !=
				DEF_rttMonReactAdminThresholdCount) {
				nv_add(TRUE, "%u ",
				       rttMonCtrlAdminEntryData->
				       rttMonReactAdminThresholdCount);
			    } /* count ie connsecutive-occurances */ 
			    break;
			case D_rttMonReactAdminThresholdType_xOfy:
			    nv_add(TRUE, "%s ",
				   D_rttMonReactAdminThresholdType_xOfy_string);
			    if ((rttMonCtrlAdminEntryData->
				 rttMonReactAdminThresholdCount !=
				 DEF_rttMonReactAdminThresholdCount) ||
				(rttMonCtrlAdminEntryData->
				 rttMonReactAdminThresholdCount2 !=
				 DEF_rttMonReactAdminThresholdCount2)) {
				nv_add(TRUE, "%u %u ",
				       rttMonCtrlAdminEntryData->
				       rttMonReactAdminThresholdCount,
				       rttMonCtrlAdminEntryData->
				       rttMonReactAdminThresholdCount2);
			    } /* either x or y value has changed */ 
			    break;
			case D_rttMonReactAdminThresholdType_average:
			    nv_add(TRUE, "%s ",
				   D_rttMonReactAdminThresholdType_average_string);
			    if (rttMonCtrlAdminEntryData->
				rttMonReactAdminThresholdCount !=
				DEF_rttMonReactAdminThresholdCount) {
				nv_add(TRUE, "%u ",
				       rttMonCtrlAdminEntryData->
				       rttMonReactAdminThresholdCount);
			    } /* count ie n-attempts */ 
			    break;
			} /* switch */ 
		    } /* threshold type */ 
		    if (rttMonCtrlAdminEntryData->
			rttMonReactAdminActionType !=
			DEF_rttMonReactAdminActionType) {
			nv_add(TRUE, "%s ",
			       D_rttMonReactAdminActionType_string);
			switch (rttMonCtrlAdminEntryData->
				rttMonReactAdminActionType) {
			case D_rttMonReactAdminActionType_none:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_none_string);
			    break;
			case D_rttMonReactAdminActionType_trapOnly:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_trapOnly_string);
			    break;
			case D_rttMonReactAdminActionType_nmvtOnly:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_nmvtOnly_string);
			    break;
			case D_rttMonReactAdminActionType_triggerOnly:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_triggerOnly_string);
			    break;
			case D_rttMonReactAdminActionType_trapAndNmvt:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_trapAndNmvt_string);
			    break;
			case D_rttMonReactAdminActionType_trapAndTrigger:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_trapAndTrigger_string);
			    break;
			case D_rttMonReactAdminActionType_nmvtAndTrigger:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_nmvtAndTrigger_string);
			    break;
			case D_rttMonReactAdminActionType_trapNmvtAndTrigger:
			    nv_add(TRUE, "%s",
				   D_rttMonReactAdminActionType_trapNmvtAndTrigger_string);
			    break;
			} /* switch */ 
		    } /* action-type */ 
		} /* if (reaction-configuration != defaultS */
		/*
		 * Do the reaction-trigger Config
		 */ 
		rttMonReactTriggerCtrlAdminIndex = 0;
		return_code =
		    getNextRttMonReactTriggerAdminEntry(rttMonCtrlAdminEntryData->
							rttMonCtrlAdminIndex,
							rttMonReactTriggerCtrlAdminIndex,
							&rttMonReactTriggerAdminEntryData);
		while ((return_code == D_Return_Successfull) &&
		       (rttMonReactTriggerAdminEntryData.
			rttMonCtrlAdminIndex ==
			rttMonCtrlAdminEntryData->
			rttMonCtrlAdminIndex)) {
		    nv_write(TRUE, "%s %s %u %u",
			     D_rttMon_string,
			     D_rttMonReactTriggerAdmin_string,
			     rttMonReactTriggerAdminEntryData.
			     rttMonCtrlAdminIndex,
			     rttMonReactTriggerAdminEntryData.
			     rttMonReactTriggerAdminRttMonCtrlAdminIndex);
		    rttMonReactTriggerCtrlAdminIndex =
			rttMonReactTriggerAdminEntryData.
			    rttMonReactTriggerAdminRttMonCtrlAdminIndex;
		    rttMonReactTriggerCtrlAdminIndex++;
		    return_code =
			getNextRttMonReactTriggerAdminEntry(rttMonCtrlAdminEntryData->
							    rttMonCtrlAdminIndex,
							    rttMonReactTriggerCtrlAdminIndex,
							    &rttMonReactTriggerAdminEntryData);
		} /* while */ 
		rttMonCtrlAdminIndex =
		    rttMonCtrlAdminEntryData->
			rttMonCtrlAdminIndex;
		rttMonCtrlAdminIndex++;
		return_code =
		    getNextRttMonCtrlAdminEntry(rttMonCtrlAdminIndex,
						rttMonCtrlAdminEntryData);
		if (return_code == D_Return_Successfull) {
		    /* It is ok to overwrite the return_code */
		    return_code =
			getRttMonEchoAdminEntry(rttMonCtrlAdminEntryData->
						rttMonCtrlAdminIndex,
						rttMonEchoAdminEntryData);
		    if (return_code != D_Return_Successfull) {
			errmsg(&msgsym(MissingEchoStruct, RTT));
		    }
		} /* return_code == D_Return_Successfull */ 
	    } /* while (return_code == D_Return_Successfull) */ 
	} /* if (csb->which == RTR_CFG_ENTRY_CMD) */ 
	return;
    } /* if (csb->nvgen) */ 
    switch (csb->which) {
    case RTR_CFG_RESET_CMD:
	if (csb->sense) {
	    setRttMonApplReset();
	}
	break;
    case RTR_CFG_SCHEDULE_CMD:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem((long) GETOBJ(int, 1));

	if (rttMonCtrlAdminQItemPtr != NULL) {
	    if (!csb->sense) {
		/* If Probe is created, attempt to reset it */
		return_code =
		    rttMonCtrlAdminQItemSecondaryMemory((long) GETOBJ(int, 1),
							&secondaryMemoryForTables);

		if (return_code != D_Return_Successfull) {
		    printf("%%RTR: failed to reset runtime entry (cannot find secondary memory)");
		    /* set all values to defaults */
		    setRttMonScheduleAdminRttLife((long) GETOBJ(int, 1),
						  DEF_rttMonScheduleAdminRttLife);
		    setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
						       DEF_rttMonScheduleAdminRttStartTime);
		    setRttMonScheduleAdminConceptRowAgeout((long) GETOBJ(int, 1),
							   CLI_DEF_rttMonScheduleAdminConceptRowAgeout);

		    break;
		}
		if (secondaryMemoryForTables) {
		    /* 
		     * Check Oper State, if it is active, reset and wait for 
		     *   reset to take effect.
		     */
		    /*
		     * If the probe is active, stop and restart
		     */
		    if (getCurrentOperState((long) GETOBJ(int, 1)) ==
			D_rttMonCtrlOperState_active) {
			printf("%%RTR: The current probe is active, to change you will need to wait\n");
			printf("\t until the probe transitions (Up to %u seconds) [(C)ontinue]: ",
			       getCurrentFrequency((long) GETOBJ(int, 1)));

			flush();
			parsed = rdtty(buff, MAXLEN);
			if (parsed && (null(buff) || toupper(*buff) == 'C')) {
			    if (setRttMonCtrlOperState((long) GETOBJ(int, 1),
						       D_rttMonCtrlOperState_reset) !=
				D_Return_Successfull) {
				printf("%%RTR:  Transition failed, probe in unknown state (try again)\n");
				break;
			    }
			    while (getCurrentOperState((long) GETOBJ(int, 1)) !=
				   D_rttMonCtrlOperState_pending) {
				printf(".");
				flush();
				process_sleep_for(ONESEC);
			    } 
			    printf("[ok]\n");
			} else {
			    break;
			}
			setRttMonCtrlAdminStatus((long) GETOBJ(int, 1),
						 D_rttMonCtrlAdminStatus_notInService);

			/* set all values to defaults */
			setRttMonScheduleAdminRttLife((long) GETOBJ(int, 1),
						      DEF_rttMonScheduleAdminRttLife);
			setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
							   DEF_rttMonScheduleAdminRttStartTime);
			setRttMonScheduleAdminConceptRowAgeout((long) GETOBJ(int, 1),
							       CLI_DEF_rttMonScheduleAdminConceptRowAgeout);
			setRttMonCtrlAdminStatus((long) GETOBJ(int, 1),
						 D_rttMonCtrlAdminStatus_active);
		    }
		}
	    } else {
		/* If Probe is created, attempt to reset it */
		return_code =
		    rttMonCtrlAdminQItemSecondaryMemory((long) GETOBJ(int, 1),
							&secondaryMemoryForTables);

		if (return_code != D_Return_Successfull) {
		    printf("%%RTR: failed to create runtime entry (cannot find secondary memory)");
		    /* set all values to defaults */
		    setRttMonScheduleAdminRttLife((long) GETOBJ(int, 1),
						  DEF_rttMonScheduleAdminRttLife);
		    setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
						       DEF_rttMonScheduleAdminRttStartTime);
		    setRttMonScheduleAdminConceptRowAgeout((long) GETOBJ(int, 1),
							   CLI_DEF_rttMonScheduleAdminConceptRowAgeout);

		    break;
		}
		if (secondaryMemoryForTables) {
		  /* 
		   * Check Oper State, if it is active, reset and wait for 
		   *   reset to take effect.
		   */
		  if (getCurrentOperState((long) GETOBJ(int, 1)) ==
		      D_rttMonCtrlOperState_active) {
		    printf("%%RTR: The current probe is active, to change you will need to wait\n");
		    printf("\t until the probe transitions (Up to %u seconds) [(C)ontinue]: ",
			   getCurrentFrequency((long) GETOBJ(int, 1)));
		  
		    flush();
		    parsed = rdtty(buff, MAXLEN);
		    if (parsed && (null(buff) || toupper(*buff) == 'C')) {
		      if (setRttMonCtrlOperState((long) GETOBJ(int, 1),
						 D_rttMonCtrlOperState_reset) !=
			  D_Return_Successfull) {
			printf("%%RTR:  Transition failed, probe in unknown state (try again)\n");
			break;
		      }
		      while (getCurrentOperState((long) GETOBJ(int, 1)) !=
			     D_rttMonCtrlOperState_pending) {
			printf(".");
			flush();
			process_sleep_for(ONESEC);
		      } 
		      printf("[ok]\n");
		    } else {
		      break;
		    }
		  }
		}
		setRttMonCtrlAdminStatus((long) GETOBJ(int, 1),
					 D_rttMonCtrlAdminStatus_notInService);

		/* set life first */
		if (GETOBJ(int, 3) == 1) {
		    setRttMonScheduleAdminRttLife((long) GETOBJ(int, 1),
						  GETOBJ(int, 2));
		}
		/* set ageout */ 
		if (GETOBJ(int, 10) == 1) {
		    setRttMonScheduleAdminConceptRowAgeout((long) GETOBJ(int, 1),
							   GETOBJ(int, 9));
		}
		if (GETOBJ(int, 4) != 0) {
		    if (GETOBJ(int, 4) == 1) {
			setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
							   1);
		    } else if (GETOBJ(int, 4) == 3) {
			setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
							   0);
		    } else {
			if (!clock_is_probably_valid()) {
			    printf("%%The date and time must be set first.\n");
			    return;
			}
			/* 
			 * Calculate the epoch of the midnight past.
			 *  Use start_epoch_time as a tmp variable
			 */ 
			clock_get_time(&start_epoch_time);
			clock_epoch_to_timeval(&start_epoch_time, &startTv,
					       clock_timezone_offset());
			startTv.hour = 0;
			startTv.minute = 0;
			startTv.second = 0;
			startTv.millisecond = 0;
			clock_timeval_to_epoch(&startTv,
					       &last_midnight);

			/*
			 * If the date has been specified, use it,
			 *  and bump year if near new year.  Thus,
			 *  we need to roll over the value to the 
			 *  next year if needed else roll over the
			 *  value to the next day if needed.
			 */
			if (GETOBJ(int, 7)) {
			    startTv.month = GETOBJ(int, 7);
			    startTv.day = GETOBJ(int, 8);

			    clock_timeval_to_epoch(&startTv, &start_epoch_time);
			    if (start_epoch_time.epoch_secs < last_midnight.epoch_secs) {
				startTv.year++;
			    }
			    startTv.hour = GETOBJ(int, 5);
			    startTv.minute = GETOBJ(int, 6);
			} else {
			    startTv.hour = GETOBJ(int, 5);
			    startTv.minute = GETOBJ(int, 6);

			    clock_timeval_to_epoch(&startTv, &start_epoch_time);
			    if (start_epoch_time.epoch_secs < last_midnight.epoch_secs) {
				startTv.day++;
			    }
			}
			clock_timeval_to_epoch(&startTv, &start_epoch_time);
			/* 
			 * start_epoch_time now holds the absolute start
			 *  time of the probe since ... 1900 
			 */
			rttMon_get_boot_epoch(&boot_epoch);
			clock_get_time(&now_epoch);
			if (now_epoch.epoch_secs >
			    start_epoch_time.epoch_secs) {
			    printf("%%Bad specified time, it is too far in the past.\n");
			    return;
			}
			/* 
			 * With the boot time and the time to start on
			 *  on the same start base, just subtract to get
			 *  how many seconds into the future to set the
			 *  starttime.
			 */
			startTime = start_epoch_time.epoch_secs -
			    boot_epoch.epoch_secs;
			/* 
			 * If the start time is past, set
			 *   it to a valid start time
			 */ 
			if (startTime < 1) {
			    startTime = 2;
			}
			/* 
			 * startTime is in seconds from boot and now 
			 *  needs to be converted to centiseconds to
			 *  match sysuptime.
			 *  eg 5 seconds = 500 centiseconds
			 */
			startTime = startTime * SYSUPTIME_UNITS;
			/*
			 * Make sure we can handle this far into the furture.
			 */
			if (startTime > RTT_MAX_DELAY_CENTISECONDS) {
			    printf("%%Bad specified time, it is too far in the future.\n");
			    return;
			}
			setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
							   startTime);
		    }
		}
		/* 
		 * Start the probe, now that it has been scheduled.
		 */ 
		if (!secondaryMemoryForTables) {
		    return_code =
			createRttMonCtrlAdminQItemSecondaryMemory((long) GETOBJ(int, 1));

		    if (return_code != D_Return_Successfull) {
			printf("%%RTR: failed to create runtime entry");
			setRttMonScheduleAdminRttStartTime((long) GETOBJ(int, 1),
							   DEF_rttMonScheduleAdminRttStartTime);

			return;
		    }
		}
		setRttMonCtrlAdminStatus((long) GETOBJ(int, 1),
					 D_rttMonCtrlAdminStatus_active);
	    } /* else not no command */ 
	} else {
	    printf("%%Invalid Entry Number, Either wrong value entered or not configured\n");
	} 
	break;
    case RTR_CFG_REACT_CONFIG_CMD:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem((long) GETOBJ(int, 1));

	if (rttMonCtrlAdminQItemPtr != NULL) {
	    if (!csb->sense) {
		setRttMonReactAdminConnectionEnable((long) GETOBJ(int, 1),
						    DEF_rttMonReactAdminConnectionEnable);
		setRttMonReactAdminTimeoutEnable((long) GETOBJ(int, 1),
						 DEF_rttMonReactAdminTimeoutEnable);
		setRttMonReactAdminThresholdFalling((long) GETOBJ(int, 1),
						    DEF_rttMonReactAdminThresholdFalling);
		setRttMonReactAdminThresholdCount2((long) GETOBJ(int, 1),
						   DEF_rttMonReactAdminThresholdCount2);
		setRttMonReactAdminThresholdCount((long) GETOBJ(int, 1),
						  DEF_rttMonReactAdminThresholdCount);
		setRttMonReactAdminThresholdType((long) GETOBJ(int, 1),
						 DEF_rttMonReactAdminThresholdType);
		setRttMonReactAdminActionTypes((long) GETOBJ(int, 1),
					       DEF_rttMonReactAdminActionType);
	    } else {
		return_code =
		    rttMonCtrlAdminQItemSecondaryMemory((long) GETOBJ(int, 1),
							&secondaryMemoryForTables);

		if (return_code == D_Return_Successfull) {
		    if (!secondaryMemoryForTables) {
			printf("%%Warning:  Entry Not Yet Scheduled, This command was accepted\n");
		    }
		}
		if (GETOBJ(int, 2) == 1) {
		    setRttMonReactAdminConnectionEnable((long) GETOBJ(int, 1),
							(long) GETOBJ(int, 3));
		}
		if (GETOBJ(int, 4) == 1) {
		    setRttMonReactAdminTimeoutEnable((long) GETOBJ(int, 1),
						     (long) GETOBJ(int, 5));
		}
		if (GETOBJ(int, 6) == 1) {
		    setRttMonReactAdminThresholdFalling((long) GETOBJ(int, 1),
							(long) GETOBJ(int, 7));
		}
		if (GETOBJ(int, 8) == 1) {
		    if (GETOBJ(int, 9) ==
			D_rttMonReactAdminThresholdType_xOfy) {
			if (GETOBJ(int, 12) == 1) {
			    setRttMonReactAdminThresholdCount2((long) GETOBJ(int, 1),
							       (long) GETOBJ(int, 13));
			}
		    }
		    if ((GETOBJ(int, 9) ==
			 D_rttMonReactAdminThresholdType_consecutive) ||
			(GETOBJ(int, 9) ==
			 D_rttMonReactAdminThresholdType_average) ||
			(GETOBJ(int, 9) ==
			 D_rttMonReactAdminThresholdType_xOfy)) {
			if (GETOBJ(int, 10) == 1) {
			    setRttMonReactAdminThresholdCount((long) GETOBJ(int, 1),
							      (long) GETOBJ(int, 11));
			}
		    }
		    setRttMonReactAdminThresholdType((long) GETOBJ(int, 1),
						     (long) GETOBJ(int, 9));
		}
		if (GETOBJ(int, 14) == 1) {
		    setRttMonReactAdminActionTypes((long) GETOBJ(int, 1),
						   (long) GETOBJ(int, 15));
		}
	    }
	} else {
	    printf("%%Invalid Entry Number, Either wrong value entered or not configured\n");
	} 
	break;
    case RTR_CFG_REACT_TRIG_CMD:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem((long) GETOBJ(int, 1));

	if (rttMonCtrlAdminQItemPtr != NULL) {
	    if (!csb->sense) {
		return_code =
		    getRttMonReactTriggerAdminEntry((long) GETOBJ(int, 1),
						    (long) GETOBJ(int, 2),
						    &rttMonReactTriggerAdminEntryData);

		if (return_code == D_Return_Successfull) {
		    setRttMonReactTriggerAdminStatus((long) GETOBJ(int, 1),
						     (long) GETOBJ(int, 2),
						     D_rttMonReactTriggerAdminStatus_destroy);
		} else {
		    printf("%%reaction-trigger does not exists\n");
		}
	    } else {
		return_code =
		    getRttMonReactTriggerAdminEntry((long) GETOBJ(int, 1),
						    (long) GETOBJ(int, 2),
						    &rttMonReactTriggerAdminEntryData);

		if (return_code != D_Return_Successfull) {
		    return_code =
			rttMonCtrlAdminQItemSecondaryMemory((long) GETOBJ(int, 1),
							    &secondaryMemoryForTables);

		    if (return_code == D_Return_Successfull) {
			if (!secondaryMemoryForTables) {
			    printf("%%Warning:  Entry Not Yet Scheduled, This command was accepted\n");
			}
		    }
		    rttMonReactTriggerAdminEntryData.rttMonCtrlAdminIndex =
			GETOBJ(int, 1);

		    rttMonReactTriggerAdminEntryData.rttMonReactTriggerAdminRttMonCtrlAdminIndex =
			GETOBJ(int, 2);
		    addUpdateRttMonReactTriggerAdminEntry((long) GETOBJ(int, 1),
							  &rttMonReactTriggerAdminEntryData);
		    setRttMonReactTriggerAdminStatus((long) GETOBJ(int, 1),
						     (long) GETOBJ(int, 2),
						     D_rttMonReactTriggerAdminStatus_active);
		}
	    }
	} else {
	    printf("%%Invalid Entry Number, Either wrong value entered or not configured\n");
	} 
	break;
    case RTR_CFG_ENTRY_CMD:
	/* 
	 * Switch into entry config mode.
	 *  Else (no) delete the entry, and react entry pointing to it.
	 *
	 */
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem((long) GETOBJ(int, 1));

	if (rttMonCtrlAdminQItemPtr != NULL) {
	    if (!csb->sense) {
		setRttMonCtrlAdminStatus((long) GETOBJ(int, 1),
					 D_rttMonCtrlAdminStatus_destroy);

		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = FALSE;
		return;
	    } else {
		return_code =
		    rttMonCtrlAdminQItemSecondaryMemory((long) GETOBJ(int, 1),
							&secondaryMemoryForTables);

		if (return_code == D_Return_Successfull) {
		    if (secondaryMemoryForTables) {
			printf("%%Entry Already Exists and cannot be modified\n");
			printf("\t(only can delete (no) and start over)\n");
			printf("\t(check to see if the probe has finished exiting)\n");
			return;
		    }
		}
	    }
	} else {
	    if (!csb->sense) {
		printf("%%Invalid Entry Number, Either wrong value entered or not configured\n");
		return;
	    }
	    if (getRttMonCtrlAdminQSize() > SYS_MAX_rttMonApplNumCtrlAdminEntry) {
		printf("%%Already at System Maximum Number of Entries of %u\n",
		       SYS_MAX_rttMonApplNumCtrlAdminEntry);
		return;
	    }
	} /* switch into config-entry mode */ 
	if (!set_mode_byname(&csb->mode,
			     D_rttMon_string,
			     MODE_VERBOSE)) {
	    printf("%%RTR: failed to enter config-rtr submode\n");
	    if (rttMonCtrlAdminQItemPtr != NULL) {
		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = FALSE;
	    }
	} else {
	    csb->userDataBlock[0] = (void *) GETOBJ(int, 1);

	    if (rttMonCtrlAdminQItemPtr != NULL) {
		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	    }
	} 
	break;
    } 
    return;
} 

void rttmon_exit_config_submode (parseinfo * csb)
{
    if (csb->nvgen) {
	/* should never get here */
	errmsg(&msgsym(BadLoc, RTT));
	return;
    }
    switch (csb->which) {
    case RTR_ENTRY_CFG_EXIT:
	if (!set_mode_byname(&csb->mode,
			     "configure",
			     MODE_VERBOSE)) {
	    printf("%%RTR: failed to exit config-rtr submode\n");
	}
	break;
    case RTR_ENTRY_CFG_END:
	if (!set_mode_byname(&csb->mode,
			     "exec",
			     MODE_VERBOSE)) {
	    printf("%%RTR: failed to end config-rtr submode\n");
	}
	break;
    } 
    return;
} 

void rttmon_config_entry_command (parseinfo * csb)
{
    long return_code;
    rttMonCtrlAdminQItem *rttMonCtrlAdminQItemPtr;
    long oldSize;
    ipaddrtype tmpIPAddr;
    unsigned char sizeByte;
    boolean parsed = FALSE;
    static char buff[MAXLEN];
    long entry_being_added;

    if (NoBuffers) {
	errmsg(&msgsym(MissingInit, RTT));
    }
    if (csb->nvgen) {
	/* should never get here */
	errmsg(&msgsym(BadLoc, RTT));
	return;
    }
    entry_being_added = (long) csb->userDataBlock[0];

    rttMonCtrlAdminQItemPtr =
	getRttMonCtrlAdminQItem(entry_being_added);
    if ((rttMonCtrlAdminQItemPtr == NULL) &&
	(csb->which != RTR_ENTRY_CFG_TYPE)) {
	printf("%%type command required first\n");
	return;
    }
    switch (csb->which) {
    case RTR_ENTRY_CFG_TYPE:
	if (!csb->sense) {
	    if (rttMonCtrlAdminQItemPtr &&
		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade) {
		printf("This will revert all config for this entry to default values [(C)ontinue]: ");
		flush();
		parsed = rdtty(buff, MAXLEN);
		if (parsed && (null(buff) || toupper(*buff) == 'C')) {
		    freeRttMonCtrlAdminQItemByIndex(entry_being_added);
		}
	    } else {
		freeRttMonCtrlAdminQItemByIndex(entry_being_added);
	    }
	    return;
	}
	/* 
	 * If the entry already exists, check for modifications 
	 */ 
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (rttMonCtrlAdminQItemPtr != NULL) {
	    if (rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade) {
		printf("This will revert all config for this entry to default values [(C)ontinue]: ");
		flush();
		parsed = rdtty(buff, MAXLEN);
		if (parsed && (null(buff) || toupper(*buff) == 'C')) {
		    freeRttMonCtrlAdminQItemByIndex(entry_being_added);
		} else {
		    return;
		}
	    } else {
		freeRttMonCtrlAdminQItemByIndex(entry_being_added);
	    }
	}
	/* NOTE WELL!!!!!!!!!!!
	 * semaphore protect usage of either ..EntryData structs 
	 *  All returns from the following code MUST MUST MUST
	 *  clear the semaphore first!!!!!!!!!!!!!!
	 */ 
	process_lock_semaphore(rttMonConfigSemaphore, MAXULONG);

	/* create the entry */
	setRttMonCtrlAdminEntryDefaults(rttMonCtrlAdminEntryData);
	setRttMonEchoAdminEntryDefaults(rttMonEchoAdminEntryData);

	rttMonCtrlAdminEntryData->rttMonCtrlAdminRttType =
	    GETOBJ(int, 1);

	rttMonCtrlAdminEntryData->rttMonCtrlAdminIndex =
	    entry_being_added;
	rttMonCtrlAdminEntryData->rttMonScheduleAdminConceptRowAgeout =
	    CLI_DEF_rttMonScheduleAdminConceptRowAgeout;
	rttMonEchoAdminEntryData->rttMonCtrlAdminIndex =
	    entry_being_added;
	rttMonEchoAdminEntryData->rttMonEchoAdminProtocol =
	    GETOBJ(int, 2);

	/*
	 *  The following call will update the default
	 *    response size value for the specific
	 *    protocol type.
	 */
	return_code =
	    reg_invoke_rtt_echo_validate(rttMonEchoAdminEntryData->
					 rttMonEchoAdminProtocol,
					 rttMonCtrlAdminEntryData,
					 (void *) rttMonEchoAdminEntryData,
					 D_Check_Response_Size);

	switch (GETOBJ(int, 2)) {
	case D_rttMonEchoAdminProtocol_ipIcmpEcho:
	    /* get it into an inaddr */
	    if (!parse_hostname(GETOBJ(string, 1), &tmpIPAddr)) {
		printf("%%Bad IP Protocol Address\n");
		process_unlock_semaphore(rttMonConfigSemaphore);
		return;
	    }
	    populateOctetString(entry_being_added,
				rttMonEchoAdminEntryData->
				rttMonEchoAdminTargetAddress,
				(char *) &tmpIPAddr,
				4);
	    break;
	case D_rttMonEchoAdminProtocol_snaRUEcho:
	    populateOctetString(entry_being_added,
				rttMonEchoAdminEntryData->
				rttMonEchoAdminTargetAddress,
				GETOBJ(string, 1),
				strlen(GETOBJ(string, 1)));
	    break;
	case D_rttMonEchoAdminProtocol_snaLU0EchoAppl:
	case D_rttMonEchoAdminProtocol_snaLU2EchoAppl:
	    sizeByte = (unsigned char) strlen(GETOBJ(string, 1));
	    populateOctetString(entry_being_added,
				rttMonEchoAdminEntryData->
				rttMonEchoAdminTargetAddress,
				&sizeByte,
				1);
	    appendOctetString(entry_being_added,
			      rttMonEchoAdminEntryData->
			      rttMonEchoAdminTargetAddress,
			      GETOBJ(string, 1),
			      sizeByte);
	    if (GETOBJ(int, 3) == 1) {
		sizeByte = (unsigned char) strlen(GETOBJ(string, 2));
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  &sizeByte,
				  1);
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  GETOBJ(string, 2),
				  sizeByte);
	    } else {
		sizeByte = (unsigned char) strlen(DEF_rttMonDependLU_App_Name);
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  &sizeByte,
				  1);
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  DEF_rttMonDependLU_App_Name,
				  sizeByte);
	    } 
	    if (GETOBJ(int, 4) == 1) {
		sizeByte = (unsigned char) strlen(GETOBJ(string, 3));
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  &sizeByte,
				  1);
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  GETOBJ(string, 3),
				  strlen(GETOBJ(string, 3)));
	    } else {
		sizeByte = 0;
		appendOctetString(entry_being_added,
				  rttMonEchoAdminEntryData->
				  rttMonEchoAdminTargetAddress,
				  &sizeByte,
				  1);
	    }
	    break;
	} /* validate the input fields */ 
	return_code =
	    reg_invoke_rtt_echo_validate(GETOBJ(int, 2),
					 rttMonCtrlAdminEntryData,
					 (void *) rttMonEchoAdminEntryData,
					 D_Check_Protocol);

	if (return_code != D_VALID_RttEntry) {
	    printf("%%Bad Protocol Value (protocol may not support type)\n");
	    process_unlock_semaphore(rttMonConfigSemaphore);
	    return;
	}
	return_code =
	    reg_invoke_rtt_echo_validate(GETOBJ(int, 2),
					 rttMonCtrlAdminEntryData,
					 (void *) rttMonEchoAdminEntryData,
					 D_Check_RttType);
	if (return_code != D_VALID_RttEntry) {
	    printf("%%Bad Type for Protocol\n");
	    process_unlock_semaphore(rttMonConfigSemaphore);
	    return;
	}
	return_code =
	    reg_invoke_rtt_echo_validate(GETOBJ(int, 2),
					 rttMonCtrlAdminEntryData,
					 (void *) rttMonEchoAdminEntryData,
					 D_Check_Address);
	if (return_code != D_VALID_RttEntry) {
	    printf("%%Bad Protocol Address for Type\n");
	    process_unlock_semaphore(rttMonConfigSemaphore);
	    return;
	}
	return_code = newRttMonCtrlAdminQItem(rttMonCtrlAdminEntryData,
					      rttMonEchoAdminEntryData);
	if (return_code != D_Return_Successfull) {
	    printf("%%Failed to create configuration entry\n");
	    process_unlock_semaphore(rttMonConfigSemaphore);
	    return;
	}
	process_unlock_semaphore(rttMonConfigSemaphore);
	break;
    case RTR_ENTRY_CFG_OWNER:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    populateOctetString(entry_being_added,
				rttMonCtrlAdminQItemPtr->
				ctrlAdminEntry->
				rttMonCtrlAdminOwner,
				DEF_rttMonCtrlAdminOwner,
				strlen(DEF_rttMonCtrlAdminOwner));
	} else {
	    populateOctetString(entry_being_added,
				rttMonCtrlAdminQItemPtr->
				ctrlAdminEntry->
				rttMonCtrlAdminOwner,
				GETOBJ(string, 1),
				strlen(GETOBJ(string, 1)));
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_TAG:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    populateOctetString(entry_being_added,
				rttMonCtrlAdminQItemPtr->
				ctrlAdminEntry->
				rttMonCtrlAdminTag,
				DEF_rttMonCtrlAdminTag,
				strlen(DEF_rttMonCtrlAdminTag));

	} else {
	    populateOctetString(entry_being_added,
				rttMonCtrlAdminQItemPtr->
				ctrlAdminEntry->
				rttMonCtrlAdminTag,
				GETOBJ(string, 1),
				strlen(GETOBJ(string, 1)));
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_THRESHOLD:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonCtrlAdminThreshold =
			DEF_rttMonCtrlAdminThreshold;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonCtrlAdminThreshold =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_FREQUENCY:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonCtrlAdminFrequency =
			DEF_rttMonCtrlAdminFrequency;
	    rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	} else {
	    /* Frequency is seconds, shift to same as timeout
	       (milliseconds). eg. 5 seconds is 5000 milliseconds */
	    if ((GETOBJ(int, 1) * 1000) <
		rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		rttMonCtrlAdminTimeout) {
		printf("%%Illegal Value:  Cannot set Frequency to be less than Timeout\n");
	    } else {
		if (GETOBJ(int, 1) < DEF_rttMonCtrlAdminFrequency) {
		    printf("Setting frequency below %u seconds may be undesirable.\n",
			   DEF_rttMonCtrlAdminFrequency);
		}
		rttMonCtrlAdminQItemPtr->
		    ctrlAdminEntry->
			rttMonCtrlAdminFrequency =
			    GETOBJ(int, 1);

		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	    }
	} 
	break;
    case RTR_ENTRY_CFG_TIMEOUT:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonCtrlAdminTimeout =
			DEF_rttMonCtrlAdminTimeout;
	    rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	} else {
	    /* Frequency is seconds, shift to same as timeout
	       (milliseconds). eg. 5 seconds is 5000 milliseconds */
	    if (GETOBJ(int, 1) >
		(rttMonCtrlAdminQItemPtr->
		 ctrlAdminEntry->
		 rttMonCtrlAdminFrequency * 1000)) {
		printf("%%Illegal Value:  Cannot set Timeout to be greater than Frequency\n");
	    } else {
		rttMonCtrlAdminQItemPtr->
		    ctrlAdminEntry->
			rttMonCtrlAdminTimeout =
			    GETOBJ(int, 1);

		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	    }
	} 
	break;
    case RTR_ENTRY_CFG_VERIFY_DATA:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonCtrlAdminVerifyData =
			DEF_rttMonCtrlAdminVerifyData;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonCtrlAdminVerifyData =
			D_rttMonCtrlAdminVerifyData_true;
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_REQUEST_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		echoAdminEntry->
		    rttMonEchoAdminPktDataRequestSize =
			DEF_rttMonEchoAdminPktDataRequestSize;
	} else {
	    oldSize =
		rttMonCtrlAdminQItemPtr->
		    echoAdminEntry->
			rttMonEchoAdminPktDataRequestSize;
	    rttMonCtrlAdminQItemPtr->
		echoAdminEntry->
		    rttMonEchoAdminPktDataRequestSize =
			GETOBJ(int, 1);

	    return_code =
		reg_invoke_rtt_echo_validate(rttMonCtrlAdminQItemPtr->
					     echoAdminEntry->
					     rttMonEchoAdminProtocol,
					     rttMonCtrlAdminQItemPtr->
					     ctrlAdminEntry,
					     (void *) rttMonCtrlAdminQItemPtr->
					     echoAdminEntry,
					     D_Check_Response_Size);
	    if (return_code != D_VALID_RttEntry) {
		printf("%%Bad Request Size for Type/Protocol\n");
		rttMonCtrlAdminQItemPtr->
		    echoAdminEntry->
			rttMonEchoAdminPktDataRequestSize =
			    oldSize;
		return;
	    }
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_RESPONSE_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		echoAdminEntry->
		    rttMonEchoAdminPktDataResponseSize =
			DEF_rttMonEchoAdminPktDataResponseSize;
	} else {
	    oldSize =
		rttMonCtrlAdminQItemPtr->
		    echoAdminEntry->
			rttMonEchoAdminPktDataResponseSize;
	    rttMonCtrlAdminQItemPtr->
		echoAdminEntry->
		    rttMonEchoAdminPktDataResponseSize =
			GETOBJ(int, 1);

	    return_code =
		reg_invoke_rtt_echo_validate(rttMonCtrlAdminQItemPtr->
					     echoAdminEntry->
					     rttMonEchoAdminProtocol,
					     rttMonCtrlAdminQItemPtr->
					     ctrlAdminEntry,
					     (void *) rttMonCtrlAdminQItemPtr->
					     echoAdminEntry,
					     D_Check_Response_Size);
	    if (return_code != D_VALID_RttEntry) {
		printf("%%Bad Response Size for Type/Protocol\n");
		rttMonCtrlAdminQItemPtr->
		    echoAdminEntry->
			rttMonEchoAdminPktDataResponseSize =
			    oldSize;
		return;
	    } else if (rttMonCtrlAdminQItemPtr->
		       echoAdminEntry->
		       rttMonEchoAdminPktDataResponseSize !=
		       oldSize) {
		printf("%%Bad Response Size for Type/Protocol, value modified\n");
	    }
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_HOURS_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminNumHourGroups =
			DEF_rttMonStatisticsAdminNumHourGroups;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminNumHourGroups =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_PATHS_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminNumPaths =
			DEF_rttMonStatisticsAdminNumPaths;
	    rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	} else {
	    if ((rttMonCtrlAdminQItemPtr->
		 ctrlAdminEntry->rttMonCtrlAdminRttType ==
		 D_rttMonCtrlAdminRttType_echo) &&
		GETOBJ(int, 1) != 1) {
		printf("%%When type is echo, %s must be 1\n",
		       D_rttMonStatisticsAdminNumPaths_string);
	    } else {
		rttMonCtrlAdminQItemPtr->
		    ctrlAdminEntry->
			rttMonStatisticsAdminNumPaths =
			    GETOBJ(int, 1);

		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	    }
	} 
	break;
    case RTR_ENTRY_CFG_HOPS_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminNumHops =
			DEF_rttMonStatisticsAdminNumHops;
	    rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	} else {
	    if ((rttMonCtrlAdminQItemPtr->
		 ctrlAdminEntry->rttMonCtrlAdminRttType ==
		 D_rttMonCtrlAdminRttType_echo) &&
		GETOBJ(int, 1) != 1) {
		printf("%%When type is echo, %s must be 1\n",
		       D_rttMonStatisticsAdminNumHops_string);
	    } else {
		rttMonCtrlAdminQItemPtr->
		    ctrlAdminEntry->
			rttMonStatisticsAdminNumHops =
			    GETOBJ(int, 1);

		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	    }
	} 
	break;
    case RTR_ENTRY_CFG_DIST_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminNumDistBuckets =
			DEF_rttMonStatisticsAdminNumDistBuckets;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminNumDistBuckets =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_DIST_INTERVAL_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminDistInterval =
			DEF_rttMonStatisticsAdminDistInterval;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonStatisticsAdminDistInterval =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_LIFE_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminNumLives =
			DEF_rttMonHistoryAdminNumLives;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminNumLives =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_BUCKET_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminNumBuckets =
			DEF_rttMonHistoryAdminNumBuckets;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminNumBuckets =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    case RTR_ENTRY_CFG_SAMPLE_SIZE:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminNumSamples =
			DEF_rttMonHistoryAdminNumSamples;
	    rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	} else {
	    if ((rttMonCtrlAdminQItemPtr->
		 ctrlAdminEntry->rttMonCtrlAdminRttType ==
		 D_rttMonCtrlAdminRttType_echo) &&
		GETOBJ(int, 1) != 1) {
		printf("%%When type is echo, %s must be 1\n",
		       D_rttMonHistoryAdminNumSamples_string);
	    } else {
		rttMonCtrlAdminQItemPtr->
		    ctrlAdminEntry->
			rttMonHistoryAdminNumSamples =
			    GETOBJ(int, 1);

		rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	    }
	} 
	break;
    case RTR_ENTRY_CFG_HIST_FILTER:
	rttMonCtrlAdminQItemPtr =
	    getRttMonCtrlAdminQItem(entry_being_added);
	if (!csb->sense) {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminFilter =
			DEF_rttMonHistoryAdminFilter;
	} else {
	    rttMonCtrlAdminQItemPtr->
		ctrlAdminEntry->
		    rttMonHistoryAdminFilter =
			GETOBJ(int, 1);
	} 
	rttMonCtrlAdminQItemPtr->modificationsHaveBeenMade = TRUE;
	break;
    }
    return;
}

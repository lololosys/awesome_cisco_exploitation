/* $Id: rtt_dump.c,v 1.1.4.8 1996/07/29 22:30:57 snyder Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_dump.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Dump Library.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor dump libraries.
 *------------------------------------------------------------------
 * $Log: rtt_dump.c,v $
 * Revision 1.1.4.8  1996/07/29  22:30:57  snyder
 * CSCdi64483:  declare arrays const in sub__rtt_mon
 *              788 out of data, 12 out of image
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/06/10  17:18:39  lmetzger
 * CSCdi59985:  Memory leak when sending alerts/resolves
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/06/07  22:26:06  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/04  18:28:30  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.4  1996/06/04  01:10:07  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * connectionfor -> connection for
 * notconnected -> not connected
 *
 * Revision 1.1.4.3  1996/06/04  00:23:24  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * verifyerror -> verify error
 * sequenceerror -> sequence error
 * Mayhave -> May have
 *
 * Revision 1.1.4.2  1996/05/22  17:51:33  snyder
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
 * Revision 1.1.4.1  1996/05/17  11:41:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:30  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:28  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:37  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:28  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "rtt_dump.h"

static char *const rtt_ProcessHeader_msgs[] =
{
    "IP Echo Application - PANIC and will EXIT",
    "echoTarget called",
    "Woke on timeout event",
    "Woke on receive event",
    "setupPathInfo called",
    "Error Return Code",
    "Calling rttMonEndUpdatePath",
    "performEcho called",
    "Calling rttMonGetFirstStoredHopAddress",
    "returned from echoTarget",
    "Calling rttMonUpdateResponse (timeout)",
    "Calling rttMonUpdateResponse (verify error)",
    "Corrupted reply to request",
    "Calling rttMonUpdateResponse (sequence error)",
    "Calling rttMonUpdateResponse (dropped)",
    "Calling rttMonUpdateResponse (ok)",
    "native response code",
    "Calling rttMonGetNextStoredHopAddress",
    "Entering Process",
    "Bad Rtt Type",
    "Calling getRttMonOperState",
    "Waiting to Start",
    "Calling rttMonTransitionTransitionState",
    "Transitioning to inactive",
    "Calling rttMonStartLife",
    "Calling rttMonUpdateNumberOfEchosAttempted",
    "Starting An Echo Operation",
    "Frequency Overrun",
    "Calling rttMonUpdateResponse (busy)",
    "Calling getRttMonOperState (check active)",
    "Going to Sleep",
    "Calling getRttMonOperState (check pending)",
    "Transitioning to pending",
    "Calling getRttMonOperState (check death)",
    "OperState is no longer active",
    "Calling getRttMonOperState (check inactive)",
    "failed to start IP Echo Application",
    "Dependant Lu Echo Application - PANIC and will EXIT",
    "Bad Target Address Length",
    "rtt_closeIndication called",
    "setting receiveFinished to TRUE",
    "openConnection called",
    "calling luT0orT2Open",
    "Dumping luT0orT2Open Result",
    "Calling rttMonSetConnectionHandle (placeholder)",
    "Calling rttMonSetHopConnectedState to FALSE",
    "Calling rttMonSetConnectionHandle",
    "Calling rttMonSetHopConnectedState to TRUE",
    "bindDataLen shorter than expected",
    "Calling rttMonSetDiagText",
    "receiveIndication called",
    "buffer receiver overflow",
    "Calling luReceiveContinue",
    "sendLogOff called",
    "Calling luReceive",
    "Calling luSend",
    "receiveFinished is FALSE",
    "Calling rttMonUpdatePath",
    "Calling luClose",
    "Calling rttMonUpdateResponse (disconnected)",
    "Calling rttMonVerifyEBCDICDataPortion",
    "setting receiveFinished to FALSE",
    "Calling rttMonUpdateResponse (not connected)",
    "SSCP RU Echo Application - PANIC and will EXIT",
    "calling sscpLuOpen",
    "Dumping sscpLuOpen Result",
    "Calling sscpLuReceiveContinue",
    "Calling sscpLuSend",
    "AckBackToHost called",
    "Sending First Ack",
    "Echo Ack 1 Send Failed",
    "Sending Second Ack",
    "Echo Ack 2 Send Failed",
    "Calling sscpLuClose",
    "Echo Test not Granted by SSCP",
    "failed to start SSCP Ru Echo Application",
    "failed to start Dependant Lu 0 Echo Application",
    "failed to start Dependant Lu 2 Echo Application",
    "Finding Route",
    "Resolution not supported",
    "Alert not supported"
    };

static char *const rtt_ErrorReturnCodes_msgs[] =
{
    "setupPathInfo",
    "rttMonUpdatePath",
    "performEcho",
    "rttMonGetFirstStoredHopAddress",
    "rttMonUpdateResponse (timeout)",
    "rttMonUpdateResponse (verify error)",
    "rttMonUpdateResponse (sequence error)",
    "rttMonUpdateResponse (dropped)",
    "rttMonUpdateResponse (ok)",
    "malloc (dataSendBuffer)",
    "ipEchoApplication",
    "rttMonUpdateResponse (busy)",
    "openConnection",
    "sscpLuOpen",
    "rttMonSetConnectionHandle",
    "rttMonSetHopConnectedState",
    "receiveIndication",
    "sscpLuReceiveContinue",
    "echoTarget",
    "sscpLuReceive",
    "sscpLuSend",
    "rttMonSetupEBCDICDataPortion",
    "rttMonGetConnectionHandle",
    "sscpLuClose",
    "rttMonUpdateResponse (disconnected)",
    "rttMonVerifyEBCDICDataPortion",
    "sscpRuEchoApplication",
    "malloc (watchedBooleanName)",
    "create_watched_boolean",
    "malloc (dataRespBuffer)",
    "malloc (destOctetString)",
    "malloc (targetOctetString)",
    "malloc (aHostName)",
    "rttMonUpdateResponse (not connected)",
    "malloc (*myHostName)",
    "getMyHostName",
    "splitLUTargetAddress",
    "malloc (*aHostName)",
    "malloc (*aApplName)",
    "malloc (*aModeName)",
    "malloc (eApplName)",
    "malloc (eModeName)",
    "malloc (initSelfBuffer)",
    "malloc (bindDataBuffer)",
    "luT0orT2Open",
    "malloc (diagTextOS)",
    "malloc (diagTextOS->octet_ptr)",
    "rttMonSetDiagText",
    "luReceiveContinue",
    "sendLogOff",
    "luReceive",
    "luSend",
    "rttMonEndUpdatePath",
    "luClose",
    "dependLuEchoApplication",
    "AckBackToHost"
    };

static char *getRttMonProtocolString (long rttMonEchoAdminProtocol)
{
    switch (rttMonEchoAdminProtocol) {
    case D_rttMonEchoAdminProtocol_notApplicable:
	return (D_rttMonEchoAdminProtocol_notApplicable_string);
	break;
    case D_rttMonEchoAdminProtocol_ipIcmpEcho:
	return (D_rttMonEchoAdminProtocol_ipIcmpEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_ipUdpEchoAppl:
	return (D_rttMonEchoAdminProtocol_ipUdpEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_snaRUEcho:
	return (D_rttMonEchoAdminProtocol_snaRUEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_snaLU0EchoAppl:
	return (D_rttMonEchoAdminProtocol_snaLU0EchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_snaLU2EchoAppl:
	return (D_rttMonEchoAdminProtocol_snaLU2EchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_snaLU62Echo:
	return (D_rttMonEchoAdminProtocol_snaLU62Echo_string);
	break;
    case D_rttMonEchoAdminProtocol_snaLU62EchoAppl:
	return (D_rttMonEchoAdminProtocol_snaLU62EchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_appleTalkEcho:
	return (D_rttMonEchoAdminProtocol_appleTalkEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_appleTalkEchoAppl:
	return (D_rttMonEchoAdminProtocol_appleTalkEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_decNetEcho:
	return (D_rttMonEchoAdminProtocol_decNetEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_decNetEchoAppl:
	return (D_rttMonEchoAdminProtocol_decNetEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_ipxEcho:
	return (D_rttMonEchoAdminProtocol_ipxEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_ipxEchoAppl:
	return (D_rttMonEchoAdminProtocol_ipxEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_isoClnsEcho:
	return (D_rttMonEchoAdminProtocol_isoClnsEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_isoClnsEchoAppl:
	return (D_rttMonEchoAdminProtocol_isoClnsEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_vinesEcho:
	return (D_rttMonEchoAdminProtocol_vinesEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_vinesEchoAppl:
	return (D_rttMonEchoAdminProtocol_vinesEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_xnsEcho:
	return (D_rttMonEchoAdminProtocol_xnsEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_xnsEchoAppl:
	return (D_rttMonEchoAdminProtocol_xnsEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_apolloEcho:
	return (D_rttMonEchoAdminProtocol_apolloEcho_string);
	break;
    case D_rttMonEchoAdminProtocol_apolloEchoAppl:
	return (D_rttMonEchoAdminProtocol_apolloEchoAppl_string);
	break;
    case D_rttMonEchoAdminProtocol_netbiosEchoAppl:
	return (D_rttMonEchoAdminProtocol_netbiosEchoAppl_string);
	break;
    default:
	return (D_Unknown_string);
    }
    return (NULL);
} 

static void dumpRttMonEchoAdminProtocol (long rttMonCtrlAdminIndex,
					 long rttMonEchoAdminProtocol)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonEchoAdminProtocol is %s\n",
		getRttMonProtocolString(rttMonEchoAdminProtocol));
    return;
} 

void dumpRttMonEchoAdminTargetAddress (long rttMonCtrlAdminIndex,
				       OctetString
				       * rttMonEchoAdminTargetAddress)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonEchoAdminTargetAddress is ");
    for (i = 0; i < rttMonEchoAdminTargetAddress->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%2X ",
		    rttMonEchoAdminTargetAddress->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\n");
    return;
} 

static void dumpRttMonEchoAdminPktDataRequestSize (long rttMonCtrlAdminIndex,
						   long
						   rttMonEchoAdminPktDataRequestSize)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonEchoAdminPktDataRequestSize (bytes) is %u\n",
		rttMonEchoAdminPktDataRequestSize);
    return;
} 

static void dumpRttMonEchoAdminPktDataResponseSize (long rttMonCtrlAdminIndex,
						    long
						    rttMonEchoAdminPktDataResponseSize)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonEchoAdminPktDataResponseSize (bytes) is %u\n",
		rttMonEchoAdminPktDataResponseSize);
    return;
} 

static void dumpRttMonCtrlAdminIndex (long rttMonCtrlAdminIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminIndex is %u\n",
		rttMonCtrlAdminIndex);
    return;
} 

static void dumpRttMonEchoAdminEntry (rttMonEchoAdminEntry_t
			       * inRttMonEchoAdminEntry)
{
    RTT_BUGINF2(inRttMonEchoAdminEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonEchoAdminEntry: \n");
    dumpRttMonEchoAdminProtocol(inRttMonEchoAdminEntry->
				rttMonCtrlAdminIndex,
				inRttMonEchoAdminEntry->
				rttMonEchoAdminProtocol);
    dumpRttMonEchoAdminTargetAddress(inRttMonEchoAdminEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonEchoAdminEntry->
				     rttMonEchoAdminTargetAddress);
    dumpRttMonEchoAdminPktDataRequestSize(inRttMonEchoAdminEntry->
					  rttMonCtrlAdminIndex,
					  inRttMonEchoAdminEntry->
					  rttMonEchoAdminPktDataRequestSize);
    dumpRttMonEchoAdminPktDataResponseSize(inRttMonEchoAdminEntry->
					   rttMonCtrlAdminIndex,
					   inRttMonEchoAdminEntry->
					   rttMonEchoAdminPktDataResponseSize);
    dumpRttMonCtrlAdminIndex(inRttMonEchoAdminEntry->
			     rttMonCtrlAdminIndex);

    return;
} 

static void dumpRttMonCtrlAdminOwner (long rttMonCtrlAdminIndex,
				      OctetString
				      * rttMonCtrlAdminOwner)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminOwner is ");
    for (i = 0; i < rttMonCtrlAdminOwner->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex, "%c",
		    rttMonCtrlAdminOwner->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex, "\n");

    return;
} 

static void dumpRttMonCtrlAdminTag (long rttMonCtrlAdminIndex,
				    OctetString
				    * rttMonCtrlAdminTag)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminTag is ");
    for (i = 0; i < rttMonCtrlAdminTag->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex, "%c",
		    rttMonCtrlAdminTag->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex, "\n");
    return;
} 

static char *getRttMonCtrlAdminRttTypeString (long rttMonCtrlAdminRttType)
{
    switch (rttMonCtrlAdminRttType) {
    case D_rttMonCtrlAdminRttType_echo:
	return (D_rttMonCtrlAdminRttType_echo_string);
	break;
    case D_rttMonCtrlAdminRttType_pathEcho:
	return (D_rttMonCtrlAdminRttType_pathEcho_string);
	break;
    case D_rttMonCtrlAdminRttType_fileIO:
	return (D_rttMonCtrlAdminRttType_fileIO_string);
	break;
    case D_rttMonCtrlAdminRttType_script:
	return (D_rttMonCtrlAdminRttType_script_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

void dumpRttMonCtrlAdminRttType (long rttMonCtrlAdminIndex,
				 long
				 rttMonCtrlAdminRttType)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminRttType is %s\n",
		getRttMonCtrlAdminRttTypeString(rttMonCtrlAdminRttType));

    return;
} 

static void dumpRttMonCtrlAdminThreshold (long rttMonCtrlAdminIndex,
					  long
					  rttMonCtrlAdminThreshold)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminThreshold (milliseconds) is %u\n",
		rttMonCtrlAdminThreshold);
    return;
} 

static void dumpRttMonCtrlAdminFrequency (long rttMonCtrlAdminIndex,
					  long
					  rttMonCtrlAdminFrequency)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminFrequency (seconds) is %u\n",
		rttMonCtrlAdminFrequency);
    return;
} 

static void dumpRttMonCtrlAdminTimeout (long rttMonCtrlAdminIndex,
					long
					rttMonCtrlAdminTimeout)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminTimeout (milliseconds) is %u\n",
		rttMonCtrlAdminTimeout);
    return;
} 

static char *getRttMonTrueFalseBooleanString (long rttMonTrueFalseBoolean)
{
    switch (rttMonTrueFalseBoolean) {
    case 1:
	return ("TRUE");
	break;
    case 2:
	return ("FALSE");
	break;
    default:
	return ("Unknown");
    } return (NULL);
} 

static void dumpRttMonCtrlAdminVerifyData (long rttMonCtrlAdminIndex,
					   long
					   rttMonCtrlAdminVerifyData)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminVerifyData is %s\n",
		getRttMonTrueFalseBooleanString(rttMonCtrlAdminVerifyData));
    return;
}
 
char *getRttMonAdminStatusString (long rttMonAdminStatus)
{
    switch (rttMonAdminStatus) {
    case D_rttMonReactTriggerAdminStatus_active:
	return (D_rttMonCtrlAdminStatus_active_string);
	break;
    case D_rttMonReactTriggerAdminStatus_notInService:
	return (D_rttMonCtrlAdminStatus_notInService_string);
	break;
    case D_rttMonReactTriggerAdminStatus_notReady:
	return (D_rttMonCtrlAdminStatus_notReady_string);
	break;
    case D_rttMonReactTriggerAdminStatus_createAndGo:
	return (D_rttMonCtrlAdminStatus_createAndGo_string);
	break;
    case D_rttMonReactTriggerAdminStatus_createAndWait:
	return (D_rttMonCtrlAdminStatus_createAndWait_string);
	break;
    case D_rttMonReactTriggerAdminStatus_destroy:
	return (D_rttMonCtrlAdminStatus_destroy_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

static void dumpRttMonCtrlAdminStatus (long rttMonCtrlAdminIndex,
				       long
				       rttMonCtrlAdminStatus)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlAdminStatus is %s\n",
		getRttMonAdminStatusString(rttMonCtrlAdminStatus));

    return;
} 

static void dumpRttMonScheduleAdminRttLife (long rttMonCtrlAdminIndex,
					    long
					    rttMonScheduleAdminRttLife)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonScheduleAdminRttLife (seconds) is %u\n",
		rttMonScheduleAdminRttLife);
    return;
} 

static void dumpRttMonScheduleAdminRttStartTime (long rttMonCtrlAdminIndex,
						 unsigned long
						 rttMonScheduleAdminRttStartTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonScheduleAdminRttStartTime (hundredths of seconds) is %u\n",
		rttMonScheduleAdminRttStartTime);
    return;
} 

static void dumpRttMonScheduleAdminConceptRowAgout (long rttMonCtrlAdminIndex,
						    long
						    rttMonScheduleAdminConceptRowAgeout)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonScheduleAdminConceptRowAgeout (seconds) is %u\n",
		rttMonScheduleAdminConceptRowAgeout);
    return;
} 

static void dumpRttMonReactAdminConnectionEnable (long rttMonCtrlAdminIndex,
						  long
						  rttMonReactAdminConnectionEnable)
{
    if (rttMonReactAdminConnectionEnable ==
	D_rttMonReactAdminConnectionEnable_true) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonReactAdminConnectionEnable is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonReactAdminConnectionEnable));
    } else if (rttMonReactAdminConnectionEnable ==
	       D_rttMonReactAdminConnectionEnable_false) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonReactAdminConnectionEnable is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonReactAdminConnectionEnable));
    } else {
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t rttMonReactAdminConnectionEnable is Unknown\n");
    } 
    return;
} 

static void dumpRttMonReactAdminTimeoutEnable (long rttMonCtrlAdminIndex,
					       long
					       rttMonReactAdminTimeoutEnable)
{
    if (rttMonReactAdminTimeoutEnable ==
	D_rttMonReactAdminTimeoutEnable_true) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonReactAdminTimeoutEnable is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonReactAdminTimeoutEnable));
    } else if (rttMonReactAdminTimeoutEnable ==
	       D_rttMonReactAdminTimeoutEnable_false) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonReactAdminTimeoutEnable is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonReactAdminTimeoutEnable));
    } else {
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t rttMonReactAdminTimeoutEnable is Unknown\n");
    } 
    return;
} 

static char *getRttMonReactAdminThresholdTypeString (long rttMonReactAdminThresholdType)
{
    switch (rttMonReactAdminThresholdType) {
    case D_rttMonReactAdminThresholdType_never:
	return (D_rttMonReactAdminThresholdType_never_string);
	break;
    case D_rttMonReactAdminThresholdType_immediate:
	return (D_rttMonReactAdminThresholdType_immediate_string);
	break;
    case D_rttMonReactAdminThresholdType_consecutive:
	return (D_rttMonReactAdminThresholdType_consecutive_string);
	break;
    case D_rttMonReactAdminThresholdType_xOfy:
	return (D_rttMonReactAdminThresholdType_xOfy_string);
	break;
    case D_rttMonReactAdminThresholdType_average:
	return (D_rttMonReactAdminThresholdType_average_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

static void dumpRttMonReactAdminThresholdType (long rttMonCtrlAdminIndex,
					       long
					       rttMonReactAdminThresholdType)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactAdminThresholdType is %s\n",
		getRttMonReactAdminThresholdTypeString(rttMonReactAdminThresholdType));
    return;
} 

static void dumpRttMonReactAdminThresholdFalling (long rttMonCtrlAdminIndex,
						  long
						  rttMonReactAdminThresholdFalling)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactAdminThresholdFalling (milliseconds) is %u\n",
		rttMonReactAdminThresholdFalling);
    return;
} 

static void dumpRttMonReactAdminThresholdCount (long rttMonCtrlAdminIndex,
						long
						rttMonReactAdminThresholdCount)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactAdminThresholdCount is %u\n",
		rttMonReactAdminThresholdCount);
    return;
} 

static void dumpRttMonReactAdminThresholdCount2 (long rttMonCtrlAdminIndex,
						 long
						 rttMonReactAdminThresholdCount2)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactAdminThresholdCount2 is %u\n",
		rttMonReactAdminThresholdCount2);
    return;
} 

static char *getRttMonReactAdminActionTypeString (long rttMonReactAdminActionType)
{
    switch (rttMonReactAdminActionType) {
    case D_rttMonReactAdminActionType_none:
	return (D_rttMonReactAdminActionType_none_string);
	break;
    case D_rttMonReactAdminActionType_trapOnly:
	return (D_rttMonReactAdminActionType_trapOnly_string);
	break;
    case D_rttMonReactAdminActionType_nmvtOnly:
	return (D_rttMonReactAdminActionType_nmvtOnly_string);
	break;
    case D_rttMonReactAdminActionType_triggerOnly:
	return (D_rttMonReactAdminActionType_triggerOnly_string);
	break;
    case D_rttMonReactAdminActionType_trapAndNmvt:
	return (D_rttMonReactAdminActionType_trapAndNmvt_string);
	break;
    case D_rttMonReactAdminActionType_trapAndTrigger:
	return (D_rttMonReactAdminActionType_trapAndTrigger_string);
	break;
    case D_rttMonReactAdminActionType_nmvtAndTrigger:
	return (D_rttMonReactAdminActionType_nmvtAndTrigger_string);
	break;
    case D_rttMonReactAdminActionType_trapNmvtAndTrigger:
	return (D_rttMonReactAdminActionType_trapNmvtAndTrigger_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

static void dumpRttMonReactAdminActionType (long rttMonCtrlAdminIndex,
					    long
					    rttMonReactAdminActionType)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactAdminActionType is %s\n",
		getRttMonReactAdminActionTypeString(rttMonReactAdminActionType));
    return;
} 

static void dumpRttMonStatisticsAdminNumHourGroups (long rttMonCtrlAdminIndex,
						    long
						    rttMonStatisticsAdminNumHourGroups)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatisticsAdminNumHourGroups is %u\n",
		rttMonStatisticsAdminNumHourGroups);
    return;
} 

static void dumpRttMonStatisticsAdminNumPaths (long rttMonCtrlAdminIndex,
					       long
					       rttMonStatisticsAdminNumPaths)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatisticsAdminNumPaths is %u\n",
		rttMonStatisticsAdminNumPaths);
    return;
} 

static void dumpRttMonStatisticsAdminNumHops (long rttMonCtrlAdminIndex,
					      long
					      rttMonStatisticsAdminNumHops)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatisticsAdminNumHops is %u\n",
		rttMonStatisticsAdminNumHops);
    return;
} 

static void dumpRttMonStatisticsAdminNumDistBuckets (long rttMonCtrlAdminIndex,
						     long
						     rttMonStatisticsAdminNumDistBuckets)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatisticsAdminNumDistBuckets is %u\n",
		rttMonStatisticsAdminNumDistBuckets);
    return;
} 

static void dumpRttMonStatisticsAdminDistInterval (long rttMonCtrlAdminIndex,
						   long
						   rttMonStatisticsAdminDistInterval)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatisticsAdminDistInterval (milliseconds) is %u\n",
		rttMonStatisticsAdminDistInterval);
    return;
} 

static void dumpRttMonHistoryAdminNumLives (long rttMonCtrlAdminIndex,
					    long
					    rttMonHistoryAdminNumLives)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryAdminNumLives is %u\n",
		rttMonHistoryAdminNumLives);
    return;
} 

static void dumpRttMonHistoryAdminNumBuckets (long rttMonCtrlAdminIndex,
					      long
					      rttMonHistoryAdminNumBuckets)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryAdminNumBuckets is %u\n",
		rttMonHistoryAdminNumBuckets);
    return;
} 

static void dumpRttMonHistoryAdminNumSamples (long rttMonCtrlAdminIndex,
					      long
					      rttMonHistoryAdminNumSamples)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryAdminNumSamples is %u\n",
		rttMonHistoryAdminNumSamples);
    return;
} 

static char *getRttMonHistoryAdminFilterString (long rttMonHistoryAdminFilter)
{
    switch (rttMonHistoryAdminFilter) {
    case D_rttMonHistoryAdminFilter_none:
	return (D_rttMonHistoryAdminFilter_none_string);
	break;
    case D_rttMonHistoryAdminFilter_all:
	return (D_rttMonHistoryAdminFilter_all_string);
	break;
    case D_rttMonHistoryAdminFilter_overThreshold:
	return (D_rttMonHistoryAdminFilter_overThreshold_string);
	break;
    case D_rttMonHistoryAdminFilter_failures:
	return (D_rttMonHistoryAdminFilter_failures_string);
	break;
    default:
	return ("Unknown");
    } 
    return (NULL);
} 

static void dumpRttMonHistoryAdminFilter (long rttMonCtrlAdminIndex,
					  long
					  rttMonHistoryAdminFilter)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryAdminFilter is %s\n",
		getRttMonHistoryAdminFilterString(rttMonHistoryAdminFilter));
    return;
} 

static void dumpRttMonCtrlOperModificationTime (long rttMonCtrlAdminIndex,
						unsigned long
						rttMonCtrlOperModificationTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperModificationTime (hundredths of seconds) is %u\n",
		rttMonCtrlOperModificationTime);
    return;
} 

static void dumpRttMonCtrlOperDiagText (long rttMonCtrlAdminIndex,
					OctetString
					* rttMonCtrlOperDiagText)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperDiagText is ");
    for (i = 0; i < rttMonCtrlOperDiagText->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%c",
		    rttMonCtrlOperDiagText->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex, "\n");
    return;
} 

static void dumpRttMonCtrlOperResetTime (long rttMonCtrlAdminIndex,
					 unsigned long
					 rttMonCtrlOperResetTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperResetTime (hundredths of seconds) is %u\n",
		rttMonCtrlOperResetTime);
    return;
} 

static void dumpRttMonCtrlOperOctetsInUse (long rttMonCtrlAdminIndex,
					   unsigned long
					   rttMonCtrlOperOctetsInUse)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperOctetsInUse is %u\n",
		rttMonCtrlOperOctetsInUse);
    return;
} 

static void dumpRttMonCtrlOperConnectionLostOccurred (long rttMonCtrlAdminIndex,
						      long
						      rttMonCtrlOperConnectionLostOccurred)
{
    if (rttMonCtrlOperConnectionLostOccurred ==
	D_rttMonCtrlOperConnectionLostOccurred_true) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperConnectionLostOccurred is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonCtrlOperConnectionLostOccurred));
    } else if (rttMonCtrlOperConnectionLostOccurred ==
	       D_rttMonCtrlOperConnectionLostOccurred_false) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperConnectionLostOccurred is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonCtrlOperConnectionLostOccurred));
    } else {
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperConnectionLostOccurred is Unknown\n");
    } 
    return;
} 

static void dumpRttMonCtrlOperTimeoutOccurred (long rttMonCtrlAdminIndex,
					       long
					       rttMonCtrlOperTimeoutOccurred)
{
    if (rttMonCtrlOperTimeoutOccurred ==
	D_rttMonCtrlOperTimeoutOccurred_true) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperTimeoutOccurred is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonCtrlOperTimeoutOccurred));
    } else if (rttMonCtrlOperTimeoutOccurred ==
	       D_rttMonCtrlOperTimeoutOccurred_false) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperTimeoutOccurred is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonCtrlOperTimeoutOccurred));
    } else {
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperTimeoutOccurred is Unknown\n");
    } 
    return;
} 

static void dumpRttMonCtrlOperOverThresholdOccurred (long rttMonCtrlAdminIndex,
						     long
						     rttMonCtrlOperOverThresholdOccurred)
{
    if (rttMonCtrlOperOverThresholdOccurred ==
	D_rttMonCtrlOperOverThresholdOccurred_true) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperOverThresholdOccurred is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonCtrlOperOverThresholdOccurred));
    } else if (rttMonCtrlOperOverThresholdOccurred ==
	       D_rttMonCtrlOperOverThresholdOccurred_false) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperOverThresholdOccurred is %s\n",
		    getRttMonTrueFalseBooleanString(rttMonCtrlOperOverThresholdOccurred));
    } else {
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t rttMonCtrlOperOverThresholdOccurred is Unknown\n");
    } 
    return;
} 

static void dumpRttMonCtrlOperNumRtts (long rttMonCtrlAdminIndex,
				       long
				       rttMonCtrlOperNumRtts)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperNumRtts is %u\n",
		rttMonCtrlOperNumRtts);
    return;
} 

static void dumpRttMonCtrlOperRttLife (long rttMonCtrlAdminIndex,
				       long
				       rttMonCtrlOperRttLife)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperRttLife (seconds) is %u\n",
		rttMonCtrlOperRttLife);
    return;
} 

char *getRttMonCtrlOperStateString (long state)
{
    switch (state) {
    case D_rttMonCtrlOperState_reset:
	return (D_rttMonCtrlOperState_reset_string);
	break;
    case D_rttMonCtrlOperState_orderlyStop:
	return (D_rttMonCtrlOperState_orderlyStop_string);
	break;
    case D_rttMonCtrlOperState_immediateStop:
	return (D_rttMonCtrlOperState_immediateStop_string);
	break;
    case D_rttMonCtrlOperState_pending:
	return (D_rttMonCtrlOperState_pending_string);
	break;
    case D_rttMonCtrlOperState_inactive:
	return (D_rttMonCtrlOperState_inactive_string);
	break;
    case D_rttMonCtrlOperState_active:
	return (D_rttMonCtrlOperState_active_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

static void dumpRttMonCtrlOperState (long rttMonCtrlAdminIndex,
				     long
				     rttMonCtrlOperState)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonCtrlOperState is %s\n",
		getRttMonCtrlOperStateString(rttMonCtrlOperState));
    return;
} 

static void dumpRttMonLatestRttOperCompletionTime (long rttMonCtrlAdminIndex,
						   unsigned long
						   rttMonLatestRttOperCompletionTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonLatestRttOperCompletionTime (milliseconds) is %u\n",
		rttMonLatestRttOperCompletionTime);
    return;
} 

static char *getRttMonOperSenseString (long rttMonOperSense)
{
    switch (rttMonOperSense) {
    case D_rttMonLatestRttOperSense_ok:
	return (D_rttMonLatestRttOperSense_ok_string);
	break;
    case D_rttMonLatestRttOperSense_disconnected:
	return (D_rttMonLatestRttOperSense_disconnected_string);
	break;
    case D_rttMonLatestRttOperSense_overThreshold:
	return (D_rttMonLatestRttOperSense_overThreshold_string);
	break;
    case D_rttMonLatestRttOperSense_timeout:
	return (D_rttMonLatestRttOperSense_timeout_string);
	break;
    case D_rttMonLatestRttOperSense_busy:
	return (D_rttMonLatestRttOperSense_busy_string);
	break;
    case D_rttMonLatestRttOperSense_notConnected:
	return (D_rttMonLatestRttOperSense_notConnected_string);
	break;
    case D_rttMonLatestRttOperSense_dropped:
	return (D_rttMonLatestRttOperSense_dropped_string);
	break;
    case D_rttMonLatestRttOperSense_sequenceError:
	return (D_rttMonLatestRttOperSense_sequenceError_string);
	break;
    case D_rttMonLatestRttOperSense_verifyError:
	return (D_rttMonLatestRttOperSense_verifyError_string);
	break;
    case D_rttMonLatestRttOperSense_applicationSpecific:
	return (D_rttMonLatestRttOperSense_applicationSpecific_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

static void dumpRttMonLatestRttOperSense (long rttMonCtrlAdminIndex,
					  long
					  rttMonLatestRttOperSense)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonLatestRttOperSense is %s\n",
		getRttMonOperSenseString(rttMonLatestRttOperSense));
    return;
} 

static void dumpRttMonLatestRttOperApplSpecificSense (long rttMonCtrlAdminIndex,
						      long
						      rttMonLatestRttOperApplSpecificSense)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonLatestRttOperApplSpecificSense is %u\n",
		rttMonLatestRttOperApplSpecificSense);
    return;
} 

static void dumpRttMonLatestRttOperSenseDescription (long rttMonCtrlAdminIndex,
						     OctetString
						     * rttMonLatestRttOperSenseDescription)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonLatestRttOperSenseDescription is ");
    for (i = 0; i < rttMonLatestRttOperSenseDescription->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%c",
		    rttMonLatestRttOperSenseDescription->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\n");

    return;
} 

static void dumpRttMonLatestRttOperTime (long rttMonCtrlAdminIndex,
					 unsigned long
					 rttMonLatestRttOperTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonLatestRttOperTime (hundredths of seconds) is %u\n",
		rttMonLatestRttOperTime);
    return;
} 

static void dumpRttMonLatestRttOperAddress (long rttMonCtrlAdminIndex,
					    OctetString
					    * rttMonLatestRttOperAddress)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonLatestRttOperAddress is ");
    for (i = 0; i < rttMonLatestRttOperAddress->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%2X ", rttMonLatestRttOperAddress->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\n");
    return;

} 

static void dumpRttMonCtrlAdminEntry (rttMonCtrlAdminEntry_t
			       * inRttMonCtrlAdminEntry)
{

    RTT_BUGINF2(inRttMonCtrlAdminEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonCtrlAdminEntry: \n");
    dumpRttMonCtrlAdminIndex(inRttMonCtrlAdminEntry->
			     rttMonCtrlAdminIndex);
    dumpRttMonCtrlAdminOwner(inRttMonCtrlAdminEntry->
			     rttMonCtrlAdminIndex,
			     inRttMonCtrlAdminEntry->
			     rttMonCtrlAdminOwner);
    dumpRttMonCtrlAdminTag(inRttMonCtrlAdminEntry->
			   rttMonCtrlAdminIndex,
			   inRttMonCtrlAdminEntry->
			   rttMonCtrlAdminTag);
    dumpRttMonCtrlAdminRttType(inRttMonCtrlAdminEntry->
			       rttMonCtrlAdminIndex,
			       inRttMonCtrlAdminEntry->
			       rttMonCtrlAdminRttType);
    dumpRttMonCtrlAdminThreshold(inRttMonCtrlAdminEntry->
				 rttMonCtrlAdminIndex,
				 inRttMonCtrlAdminEntry->
				 rttMonCtrlAdminThreshold);
    dumpRttMonCtrlAdminFrequency(inRttMonCtrlAdminEntry->
				 rttMonCtrlAdminIndex,
				 inRttMonCtrlAdminEntry->
				 rttMonCtrlAdminFrequency);
    dumpRttMonCtrlAdminTimeout(inRttMonCtrlAdminEntry->
			       rttMonCtrlAdminIndex,
			       inRttMonCtrlAdminEntry->
			       rttMonCtrlAdminTimeout);
    dumpRttMonCtrlAdminVerifyData(inRttMonCtrlAdminEntry->
				  rttMonCtrlAdminIndex,
				  inRttMonCtrlAdminEntry->
				  rttMonCtrlAdminVerifyData);
    dumpRttMonCtrlAdminStatus(inRttMonCtrlAdminEntry->
			      rttMonCtrlAdminIndex,
			      inRttMonCtrlAdminEntry->
			      rttMonCtrlAdminStatus);
    dumpRttMonScheduleAdminRttLife(inRttMonCtrlAdminEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonCtrlAdminEntry->
				   rttMonScheduleAdminRttLife);
    dumpRttMonScheduleAdminRttStartTime(inRttMonCtrlAdminEntry->
					rttMonCtrlAdminIndex,
					inRttMonCtrlAdminEntry->
					rttMonScheduleAdminRttStartTime);
    dumpRttMonScheduleAdminConceptRowAgout(inRttMonCtrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   inRttMonCtrlAdminEntry->
					   rttMonScheduleAdminConceptRowAgeout);
    dumpRttMonReactAdminConnectionEnable(inRttMonCtrlAdminEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonCtrlAdminEntry->
					 rttMonReactAdminConnectionEnable);
    dumpRttMonReactAdminTimeoutEnable(inRttMonCtrlAdminEntry->
				      rttMonCtrlAdminIndex,
				      inRttMonCtrlAdminEntry->
				      rttMonReactAdminTimeoutEnable);
    dumpRttMonReactAdminThresholdType(inRttMonCtrlAdminEntry->
				      rttMonCtrlAdminIndex,
				      inRttMonCtrlAdminEntry->
				      rttMonReactAdminThresholdType);
    dumpRttMonReactAdminThresholdFalling(inRttMonCtrlAdminEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonCtrlAdminEntry->
					 rttMonReactAdminThresholdFalling);
    dumpRttMonReactAdminThresholdCount(inRttMonCtrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       inRttMonCtrlAdminEntry->
				       rttMonReactAdminThresholdCount);
    dumpRttMonReactAdminThresholdCount2(inRttMonCtrlAdminEntry->
					rttMonCtrlAdminIndex,
					inRttMonCtrlAdminEntry->
					rttMonReactAdminThresholdCount2);
    dumpRttMonReactAdminActionType(inRttMonCtrlAdminEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonCtrlAdminEntry->
				   rttMonReactAdminActionType);
    dumpRttMonStatisticsAdminNumHourGroups(inRttMonCtrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   inRttMonCtrlAdminEntry->
					   rttMonStatisticsAdminNumHourGroups);
    dumpRttMonStatisticsAdminNumPaths(inRttMonCtrlAdminEntry->
				      rttMonCtrlAdminIndex,
				      inRttMonCtrlAdminEntry->
				      rttMonStatisticsAdminNumPaths);
    dumpRttMonStatisticsAdminNumHops(inRttMonCtrlAdminEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonCtrlAdminEntry->
				     rttMonStatisticsAdminNumHops);
    dumpRttMonStatisticsAdminNumDistBuckets(inRttMonCtrlAdminEntry->
					    rttMonCtrlAdminIndex,
					    inRttMonCtrlAdminEntry->
					    rttMonStatisticsAdminNumDistBuckets);
    dumpRttMonStatisticsAdminDistInterval(inRttMonCtrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  inRttMonCtrlAdminEntry->
					  rttMonStatisticsAdminDistInterval);
    dumpRttMonHistoryAdminNumLives(inRttMonCtrlAdminEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonCtrlAdminEntry->
				   rttMonHistoryAdminNumLives);
    dumpRttMonHistoryAdminNumBuckets(inRttMonCtrlAdminEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonCtrlAdminEntry->
				     rttMonHistoryAdminNumBuckets);
    dumpRttMonHistoryAdminNumSamples(inRttMonCtrlAdminEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonCtrlAdminEntry->
				     rttMonHistoryAdminNumSamples);
    dumpRttMonHistoryAdminFilter(inRttMonCtrlAdminEntry->
				 rttMonCtrlAdminIndex,
				 inRttMonCtrlAdminEntry->
				 rttMonHistoryAdminFilter);
    dumpRttMonCtrlOperModificationTime(inRttMonCtrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       inRttMonCtrlAdminEntry->
				       rttMonCtrlOperModificationTime);
    dumpRttMonCtrlOperDiagText(inRttMonCtrlAdminEntry->
			       rttMonCtrlAdminIndex,
			       inRttMonCtrlAdminEntry->
			       rttMonCtrlOperDiagText);
    dumpRttMonCtrlOperResetTime(inRttMonCtrlAdminEntry->
				rttMonCtrlAdminIndex,
				inRttMonCtrlAdminEntry->
				rttMonCtrlOperResetTime);
    dumpRttMonCtrlOperOctetsInUse(inRttMonCtrlAdminEntry->
				  rttMonCtrlAdminIndex,
				  inRttMonCtrlAdminEntry->
				  rttMonCtrlOperOctetsInUse);
    dumpRttMonCtrlOperConnectionLostOccurred(inRttMonCtrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     inRttMonCtrlAdminEntry->
					     rttMonCtrlOperConnectionLostOccurred);
    dumpRttMonCtrlOperTimeoutOccurred(inRttMonCtrlAdminEntry->
				      rttMonCtrlAdminIndex,
				      inRttMonCtrlAdminEntry->
				      rttMonCtrlOperTimeoutOccurred);
    dumpRttMonCtrlOperOverThresholdOccurred(inRttMonCtrlAdminEntry->
					    rttMonCtrlAdminIndex,
					    inRttMonCtrlAdminEntry->
					    rttMonCtrlOperOverThresholdOccurred);
    dumpRttMonCtrlOperNumRtts(inRttMonCtrlAdminEntry->
			      rttMonCtrlAdminIndex,
			      inRttMonCtrlAdminEntry->
			      rttMonCtrlOperNumRtts);
    dumpRttMonCtrlOperRttLife(inRttMonCtrlAdminEntry->
			      rttMonCtrlAdminIndex,
			      inRttMonCtrlAdminEntry->
			      rttMonCtrlOperRttLife);
    dumpRttMonCtrlOperState(inRttMonCtrlAdminEntry->
			    rttMonCtrlAdminIndex,
			    inRttMonCtrlAdminEntry->
			    rttMonCtrlOperState);
    dumpRttMonLatestRttOperCompletionTime(inRttMonCtrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  inRttMonCtrlAdminEntry->
					  rttMonLatestRttOperCompletionTime);
    dumpRttMonLatestRttOperSense(inRttMonCtrlAdminEntry->
				 rttMonCtrlAdminIndex,
				 inRttMonCtrlAdminEntry->
				 rttMonLatestRttOperSense);
    dumpRttMonLatestRttOperApplSpecificSense(inRttMonCtrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     inRttMonCtrlAdminEntry->
					     rttMonLatestRttOperApplSpecificSense);
    dumpRttMonLatestRttOperSenseDescription(inRttMonCtrlAdminEntry->
					    rttMonCtrlAdminIndex,
					    inRttMonCtrlAdminEntry->
					    rttMonLatestRttOperSenseDescription);
    dumpRttMonLatestRttOperTime(inRttMonCtrlAdminEntry->
				rttMonCtrlAdminIndex,
				inRttMonCtrlAdminEntry->
				rttMonLatestRttOperTime);
    dumpRttMonLatestRttOperAddress(inRttMonCtrlAdminEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonCtrlAdminEntry->
				   rttMonLatestRttOperAddress);

    return;
} 

void dumpRttMonCtrlAdminQItem (rttMonCtrlAdminQItem
			       * inRttMonCtrlAdminQItem)
{
    RTT_BUGINF2(inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonCtrlAdminIndex,
		"\t Dumping CtrlAdminEntry\n");
    dumpRttMonCtrlAdminEntry(inRttMonCtrlAdminQItem->
			     ctrlAdminEntry);
    RTT_BUGINF2(inRttMonCtrlAdminQItem->
		echoAdminEntry->
		rttMonCtrlAdminIndex,
		"\t Dumping EchoAdminEntry\n");
    dumpRttMonEchoAdminEntry(inRttMonCtrlAdminQItem->
			     echoAdminEntry);
    return;
} 

#ifdef NOBODY_CALLS_ME
static void dumpRttMonReactTriggerAdminRttMonCtrlAdminIndex (long rttMonCtrlAdminIndex,
							     long
							     rttMonReactTriggerAdminRttMonCtrlAdminIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactTriggerAdminRttMonCtrlAdminIndex is %u\n",
		rttMonReactTriggerAdminRttMonCtrlAdminIndex);
    return;
} 

static void dumpRttMonReactTriggerAdminStatus (long rttMonCtrlAdminIndex,
					long
					rttMonReactTriggerAdminStatus)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactTriggerAdminStatus is %s\n",
		getRttMonAdminStatusString(rttMonReactTriggerAdminStatus));

    return;
} 
#endif

static char *getRttMonReactTriggerOperStateString (long 
						   rttMonReactTriggerOperState)
{
    switch (rttMonReactTriggerOperState) {
    case D_rttMonReactTriggerOperState_active:
	return (D_rttMonReactTriggerOperState_active_string);
	break;
    case D_rttMonReactTriggerOperState_pending:
	return (D_rttMonReactTriggerOperState_pending_string);
	break;
    default:
	return (D_Unknown_string);
    } 
    return (NULL);
} 

#ifdef NOBODY_CALLS_ME
static void dumpRttMonReactTriggerOperState (long rttMonCtrlAdminIndex,
				      long
				      rttMonReactTriggerOperState)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonReactTriggerOperState is %s\n",
		getRttMonReactTriggerOperStateString(rttMonReactTriggerOperState));
} 
#endif

#ifdef NOBODY_CALLS_ME
void dumpRttMonReactTriggerAdminEntry (rttMonReactTriggerAdminEntry_t
				       * inRttMonReactTriggerAdminEntry)
{
    RTT_BUGINF2(inRttMonReactTriggerAdminEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonReactTriggerAdminEntry: \n");
    dumpRttMonReactTriggerAdminRttMonCtrlAdminIndex(inRttMonReactTriggerAdminEntry->
						    rttMonCtrlAdminIndex,
						    inRttMonReactTriggerAdminEntry->
						    rttMonReactTriggerAdminRttMonCtrlAdminIndex);
    dumpRttMonReactTriggerAdminStatus(inRttMonReactTriggerAdminEntry->
				      rttMonCtrlAdminIndex,
				      inRttMonReactTriggerAdminEntry->
				      rttMonReactTriggerAdminStatus);
    dumpRttMonReactTriggerOperState(inRttMonReactTriggerAdminEntry->
				    rttMonCtrlAdminIndex,
				    inRttMonReactTriggerAdminEntry->
				    rttMonReactTriggerOperState);
    dumpRttMonCtrlAdminIndex(inRttMonReactTriggerAdminEntry->
			     rttMonCtrlAdminIndex);

    return;
} 
#endif

#ifdef NOBODY_CALLS_ME
static void dumpRttMonStatsCaptureStartTimeIndex (long rttMonCtrlAdminIndex,
						  unsigned long
						  rttMonStatsCaptureStartTimeIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureStartTimeIndex (hundredths of seconds) is %u\n",
		rttMonStatsCaptureStartTimeIndex);
    return;
} 

static void dumpRttMonStatsCapturePathIndex (long rttMonCtrlAdminIndex,
				      long
				      rttMonStatsCapturePathIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCapturePathIndex is %u\n",
		rttMonStatsCapturePathIndex);
    return;
} 

static void dumpRttMonStatsCaptureHopIndex (long rttMonCtrlAdminIndex,
					    long
					    rttMonStatsCaptureHopIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureHopIndex is %u\n",
		rttMonStatsCaptureHopIndex);
    return;
} 

static void dumpRtMonStatsCaptureDistIndex (long rttMonCtrlAdminIndex,
					    long
					    rttMonStatsCaptureDistIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureDistIndex is %u\n",
		rttMonStatsCaptureDistIndex);
    return;
} 

static void dumpRttMonStatsCaptureCompletions (long rttMonCtrlAdminIndex,
					       long
					       rttMonStatsCaptureCompletions)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureCompletions is %u\n",
		rttMonStatsCaptureCompletions);
    return;
} 

static void dumpRttMonStatsCaptureOverThresholds (long rttMonCtrlAdminIndex,
						  long
						  rttMonStatsCaptureOverThresholds)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureOverThresholds is %u\n",
		rttMonStatsCaptureOverThresholds);
    return;
} 

static void dumpRttMonStatsCaptureSumCompletionTime (long rttMonCtrlAdminIndex,
						     unsigned long
						     rttMonStatsCaptureSumCompletionTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureSumCompletionTime (milliseconds) is %u\n",
		rttMonStatsCaptureSumCompletionTime);
    return;
} 

static void dumpRttMonStatsCaptureSumCompletionTime2Low (long rttMonCtrlAdminIndex,
							 unsigned long
							 rttMonStatsCaptureSumCompletionTime2Low)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureSumCompletionTime2Low (milliseconds) is %u\n",
		rttMonStatsCaptureSumCompletionTime2Low);
    return;
} 

static void dumpRttMonStatsCaptureSumCompletionTime2High (long rttMonCtrlAdminIndex,
							  unsigned long
							  rttMonStatsCaptureSumCompletionTime2High)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureSumCompletionTime2High (milliseconds) is %u\n",
		rttMonStatsCaptureSumCompletionTime2High);
    return;
} 

static void dumpRttMonStatsCaptureCompletionTimeMax (long rttMonCtrlAdminIndex,
						     unsigned long
						     rttMonStatsCaptureCompletionTimeMax)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureCompletionTimeMax (milliseconds) is %u\n",
		rttMonStatsCaptureCompletionTimeMax);
    return;
} 

static void dumpRttMonStatsCaptureCompletionTimeMin (long rttMonCtrlAdminIndex,
						     unsigned long
						     rttMonStatsCaptureCompletionTimeMin)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCaptureCompletionTimeMin (milliseconds) is %u\n",
		rttMonStatsCaptureCompletionTimeMin);
    return;
} 

void dumpRttMonStatsCaptureEntry (rttMonStatsCaptureEntry_t
				  * inRttMonStatsCaptureEntry)
{
    RTT_BUGINF2(inRttMonStatsCaptureEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonStatsCaptureEntry: \n");
    dumpRttMonStatsCaptureStartTimeIndex(inRttMonStatsCaptureEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonStatsCaptureEntry->
					 rttMonStatsCaptureStartTimeIndex);
    dumpRttMonStatsCapturePathIndex(inRttMonStatsCaptureEntry->
				    rttMonCtrlAdminIndex,
				    inRttMonStatsCaptureEntry->
				    rttMonStatsCapturePathIndex);
    dumpRttMonStatsCaptureHopIndex(inRttMonStatsCaptureEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonStatsCaptureEntry->
				   rttMonStatsCaptureHopIndex);
    dumpRtMonStatsCaptureDistIndex(inRttMonStatsCaptureEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonStatsCaptureEntry->
				   rttMonStatsCaptureDistIndex);
    dumpRttMonStatsCaptureCompletions(inRttMonStatsCaptureEntry->
				      rttMonCtrlAdminIndex,
				      inRttMonStatsCaptureEntry->
				      rttMonStatsCaptureCompletions);
    dumpRttMonStatsCaptureOverThresholds(inRttMonStatsCaptureEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonStatsCaptureEntry->
					 rttMonStatsCaptureOverThresholds);
    dumpRttMonStatsCaptureSumCompletionTime(inRttMonStatsCaptureEntry->
					    rttMonCtrlAdminIndex,
					    inRttMonStatsCaptureEntry->
					    rttMonStatsCaptureSumCompletionTime);
    dumpRttMonStatsCaptureSumCompletionTime2Low(inRttMonStatsCaptureEntry->
						rttMonCtrlAdminIndex,
						inRttMonStatsCaptureEntry->
						rttMonStatsCaptureSumCompletionTime2Low);
    dumpRttMonStatsCaptureSumCompletionTime2High(inRttMonStatsCaptureEntry->
						 rttMonCtrlAdminIndex,
						 inRttMonStatsCaptureEntry->
						 rttMonStatsCaptureSumCompletionTime2High);
    dumpRttMonStatsCaptureCompletionTimeMax(inRttMonStatsCaptureEntry->
					    rttMonCtrlAdminIndex,
					    inRttMonStatsCaptureEntry->
					    rttMonStatsCaptureCompletionTimeMax);
    dumpRttMonStatsCaptureCompletionTimeMin(inRttMonStatsCaptureEntry->
					    rttMonCtrlAdminIndex,
					    inRttMonStatsCaptureEntry->
					    rttMonStatsCaptureCompletionTimeMin);
    dumpRttMonCtrlAdminIndex(inRttMonStatsCaptureEntry->
			     rttMonCtrlAdminIndex);

    return;
} 
#endif

#ifdef NOBODY_CALLS_ME
static void dumpRttMonStatsCollectNumDisconnects (long rttMonCtrlAdminIndex,
						  long
						  rttMonStatsCollectNumDisconnects)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectNumDisconnects is %u\n",
		rttMonStatsCollectNumDisconnects);
    return;
} 

static void dumpRttMonStatsCollectTimeouts (long rttMonCtrlAdminIndex,
					    long
					    rttMonStatsCollectTimeouts)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectTimeouts is %u\n",
		rttMonStatsCollectTimeouts);
    return;
} 

static void dumpRttMonStatsCollectBusies (long rttMonCtrlAdminIndex,
					  long
					  rttMonStatsCollectBusies)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectBusies is %u\n",
		rttMonStatsCollectBusies);
    return;
} 

static void dumpRttMonStatsCollectNoConnections (long rttMonCtrlAdminIndex,
						 long
						 rttMonStatsCollectNoConnections)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectNoConnections is %u\n",
		rttMonStatsCollectNoConnections);
    return;
} 

static void dumpRttMonStatsCollectDrops (long rttMonCtrlAdminIndex,
					 long
					 rttMonStatsCollectDrops)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectDrops is %u\n",
		rttMonStatsCollectDrops);
    return;
} 

static void dumpRttMonStatsCollectSequenceErrors (long rttMonCtrlAdminIndex,
						  long
						  rttMonStatsCollectSequenceErrors)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectSequenceErrors is %u\n",
		rttMonStatsCollectSequenceErrors);
    return;
} 

static void dumpRttMonStatsCollectVerifyErrors (long rttMonCtrlAdminIndex,
						long
						rttMonStatsCollectVerifyErrors)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectVerifyErrors is %u\n",
		rttMonStatsCollectVerifyErrors);
    return;
} 

static void dumpRttMonStatsCollectAddress (long rttMonCtrlAdminIndex,
					   OctetString
					   * rttMonStatsCollectAddress)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonStatsCollectAddress is ");
    for (i = 0; i < rttMonStatsCollectAddress->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%2X ", rttMonStatsCollectAddress->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\n");
    return;

} 

void dumpRttMonStatsCollectEntry (rttMonStatsCollectEntry_t
				  * inRttMonStatsCollectEntry)
{
    RTT_BUGINF2(inRttMonStatsCollectEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonStatsCollectEntry: \n");
    dumpRttMonStatsCollectNumDisconnects(inRttMonStatsCollectEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonStatsCollectEntry->
					 rttMonStatsCollectNumDisconnects);
    dumpRttMonStatsCollectTimeouts(inRttMonStatsCollectEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonStatsCollectEntry->
				   rttMonStatsCollectTimeouts);
    dumpRttMonStatsCollectBusies(inRttMonStatsCollectEntry->
				 rttMonCtrlAdminIndex,
				 inRttMonStatsCollectEntry->
				 rttMonStatsCollectBusies);
    dumpRttMonStatsCollectNoConnections(inRttMonStatsCollectEntry->
					rttMonCtrlAdminIndex,
					inRttMonStatsCollectEntry->
					rttMonStatsCollectNoConnections);
    dumpRttMonStatsCollectDrops(inRttMonStatsCollectEntry->
				rttMonCtrlAdminIndex,
				inRttMonStatsCollectEntry->
				rttMonStatsCollectDrops);
    dumpRttMonStatsCollectSequenceErrors(inRttMonStatsCollectEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonStatsCollectEntry->
					 rttMonStatsCollectSequenceErrors);
    dumpRttMonStatsCollectVerifyErrors(inRttMonStatsCollectEntry->
				       rttMonCtrlAdminIndex,
				       inRttMonStatsCollectEntry->
				       rttMonStatsCollectVerifyErrors);
    dumpRttMonStatsCollectAddress(inRttMonStatsCollectEntry->
				  rttMonCtrlAdminIndex,
				  inRttMonStatsCollectEntry->
				  rttMonStatsCollectAddress);
    dumpRttMonCtrlAdminIndex(inRttMonStatsCollectEntry->
			     rttMonCtrlAdminIndex);
    dumpRttMonStatsCaptureStartTimeIndex(inRttMonStatsCollectEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonStatsCollectEntry->
					 rttMonStatsCaptureStartTimeIndex);
    dumpRttMonStatsCapturePathIndex(inRttMonStatsCollectEntry->
				    rttMonCtrlAdminIndex,
				    inRttMonStatsCollectEntry->
				    rttMonStatsCapturePathIndex);
    dumpRttMonStatsCaptureHopIndex(inRttMonStatsCollectEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonStatsCollectEntry->
				   rttMonStatsCaptureHopIndex);

    return;
} 
#endif

#ifdef NOBODY_CALLS_ME
static void dumpRttMonStatsTotalsElapsedTime (long rttMonCtrlAdminIndex,
					      unsigned long
					      rttMonStatsTotalsElapsedTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsTotalsElapsedTime (hundredths of seconds) is %u\n",
		rttMonStatsTotalsElapsedTime);
    return;
} 

static void dumpRttMonStatsTotalsInitiations (long rttMonCtrlAdminIndex,
					      long
					      rttMonStatsTotalsInitiations)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonStatsTotalsInitiations is %u\n",
		rttMonStatsTotalsInitiations);
    return;
} 

void dumpRttMonStatsTotalsEntry (rttMonStatsTotalsEntry_t
				 * inRttMonStatsTotalsEntry)
{
    RTT_BUGINF2(inRttMonStatsTotalsEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonStatsTotalsEntry: \n");
    dumpRttMonStatsTotalsElapsedTime(inRttMonStatsTotalsEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonStatsTotalsEntry->
				     rttMonStatsTotalsElapsedTime);
    dumpRttMonStatsTotalsInitiations(inRttMonStatsTotalsEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonStatsTotalsEntry->
				     rttMonStatsTotalsInitiations);
    dumpRttMonCtrlAdminIndex(inRttMonStatsTotalsEntry->
			     rttMonCtrlAdminIndex);
    dumpRttMonStatsCaptureStartTimeIndex(inRttMonStatsTotalsEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonStatsTotalsEntry->
					 rttMonStatsCaptureStartTimeIndex);

    return;
} 
#endif

#ifdef NOBODY_CALLS_ME
static void dumpRttMonHistoryCollectionLifeIndex (long rttMonCtrlAdminIndex,
						  long
						  rttMonHistoryCollectionLifeIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionLifeIndex is %u\n",
		rttMonHistoryCollectionLifeIndex);
    return;
} 

static void dumpRttMonHistoryCollectionBucketIndex (long rttMonCtrlAdminIndex,
						    long
						    rttMonHistoryCollectionBucketIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionBucketIndex is %u\n",
		rttMonHistoryCollectionBucketIndex);
    return;
} 

static void dumpRttMonHistoryCollectionSampleIndex (long rttMonCtrlAdminIndex,
						    long
						    rttMonHistoryCollectionSampleIndex)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionSampleIndex is %u\n",
		rttMonHistoryCollectionSampleIndex);
    return;
} 

static void dumpRtMonHistoryCollectionSampleTime (long rttMonCtrlAdminIndex,
						  unsigned long
						  rttMonHistoryCollectionSampleTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionSampleTime (hundredths of seconds) is %u\n",
		rttMonHistoryCollectionSampleTime);
    return;
} 

static void dumpRttMonHistoryCollectionAddress (long rttMonCtrlAdminIndex,
						OctetString
						* rttMonHistoryCollectionAddress)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionAddress is ");
    for (i = 0; i < rttMonHistoryCollectionAddress->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%2X ", rttMonHistoryCollectionAddress->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\n");
    return;

} 

static void dumpRttMonHistoryCollectionCompletionTime (long rttMonCtrlAdminIndex,
						       unsigned long
						       rttMonHistoryCollectionCompletionTime)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionCompletionTime (milliseconds) is %u\n",
		rttMonHistoryCollectionCompletionTime);
    return;
} 

static void dumpRttMonHistoryCollectionSense (long rttMonCtrlAdminIndex,
					      long
					      rttMonHistoryCollectionSense)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionSense is %s\n",
		getRttMonOperSenseString(rttMonHistoryCollectionSense));
} 

static void dumpRttMonHistoryCollectionApplSpecificSense (long rttMonCtrlAdminIndex,
							  long
							  rttMonHistoryCollectionApplSpecificSense)
{
    RTT_BUGINF3(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionApplSpecificSense is %u\n",
		rttMonHistoryCollectionApplSpecificSense);
    return;
} 

static void dumpRttMonHistoryCollectionSenseDescription (long rttMonCtrlAdminIndex,
							 OctetString
							 * rttMonHistoryCollectionSenseDescription)
{
    long i;

    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t rttMonHistoryCollectionSenseDescription is ");
    for (i = 0; i < rttMonHistoryCollectionSenseDescription->length; i++) {
	RTT_BUGINF3(rttMonCtrlAdminIndex,
		    "%c", rttMonHistoryCollectionSenseDescription->octet_ptr[i]);
    }
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\n");
    return;
} 

void dumpRttMonHistoryCollectionEntry (rttMonHistoryCollectionEntry_t
				       * inRttMonHistoryCollectionEntry)
{
    RTT_BUGINF2(inRttMonHistoryCollectionEntry->
		rttMonCtrlAdminIndex,
		"\t RttMonHistoryCollectionEntry: \n");
    dumpRttMonHistoryCollectionLifeIndex(inRttMonHistoryCollectionEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonHistoryCollectionEntry->
					 rttMonHistoryCollectionLifeIndex);
    dumpRttMonHistoryCollectionBucketIndex(inRttMonHistoryCollectionEntry->
					   rttMonCtrlAdminIndex,
					   inRttMonHistoryCollectionEntry->
					   rttMonHistoryCollectionBucketIndex);
    dumpRttMonHistoryCollectionSampleIndex(inRttMonHistoryCollectionEntry->
					   rttMonCtrlAdminIndex,
					   inRttMonHistoryCollectionEntry->
					   rttMonHistoryCollectionSampleIndex);
    dumpRtMonHistoryCollectionSampleTime(inRttMonHistoryCollectionEntry->
					 rttMonCtrlAdminIndex,
					 inRttMonHistoryCollectionEntry->
					 rttMonHistoryCollectionSampleTime);
    dumpRttMonHistoryCollectionAddress(inRttMonHistoryCollectionEntry->
				       rttMonCtrlAdminIndex,
				       inRttMonHistoryCollectionEntry->
				       rttMonHistoryCollectionAddress);
    dumpRttMonHistoryCollectionCompletionTime(inRttMonHistoryCollectionEntry->
					      rttMonCtrlAdminIndex,
					      inRttMonHistoryCollectionEntry->
					      rttMonHistoryCollectionCompletionTime);
    dumpRttMonHistoryCollectionSense(inRttMonHistoryCollectionEntry->
				     rttMonCtrlAdminIndex,
				     inRttMonHistoryCollectionEntry->
				     rttMonHistoryCollectionSense);
    dumpRttMonHistoryCollectionApplSpecificSense(inRttMonHistoryCollectionEntry->
						 rttMonCtrlAdminIndex,
						 inRttMonHistoryCollectionEntry->
						 rttMonHistoryCollectionApplSpecificSense);
    dumpRttMonHistoryCollectionSenseDescription(inRttMonHistoryCollectionEntry->
						rttMonCtrlAdminIndex,
						inRttMonHistoryCollectionEntry->
						rttMonHistoryCollectionSenseDescription);
    dumpRttMonCtrlAdminIndex(inRttMonHistoryCollectionEntry->
			     rttMonCtrlAdminIndex);

    return;
} 
#endif

void dumpRttMonLibReturnCode (long rttMonCtrlAdminIndex,
			      long rttMonLibReturnCode)
{
    RTT_BUGINF2(rttMonCtrlAdminIndex,
		"\t RTR lib return code is ");
    switch (rttMonLibReturnCode) {
    case D_Return_Successfull:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Return_Successfull\n");
	break;
    case D_Invalid_Command:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Invalid_Command\n");
	break;
    case D_Invalid_DataPtr:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Invalid_DataPtr\n");
	break;
    case D_Invalid_DataSize:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Invalid_DataSize\n");
	break;
    case D_Path_Too_Long:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Path_Too_Long\n");
	break;
    case D_All_Paths_In_Use:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_All_Paths_In_Use\n");
	break;
    case D_Bad_Data_Portion:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Bad_Data_Portion\n");
	break;
    case D_Hop_Not_Found:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Hop_Not_Found\n");
	break;
    case D_Conn_Handle_List_Invalid:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Conn_Handle_List_Invalid\n");
	break;
    case D_History_Buckets_Full:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_History_Buckets_Full\n");
	break;
    case D_Bad_Sequence_Number:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Bad_Sequence_Number\n");
	break;
    case D_Conn_Handle_List_Full:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Conn_Handle_List_Full\n");
	break;
    case D_BadOperState:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_BadOperState\n");
	break;
    case D_MemoryAllocationFailed:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_MemoryAllocationFailed\n");
	break;
    case D_DataFieldIsInvalid:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_DataFieldIsInvalid\n");
	break;
    case D_InvalidParameter:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_InvalidParameter\n");
	break;
    case D_QueueFull:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_QueueFull\n");
	break;
    case D_EntryNotFound:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_EntryNotFound\n");
	break;
    case D_DataNotBeingStored:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_DataNotBeingStored\n");
	break;
    case D_OperStateInTransition:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_OperStateInTransition\n");
	break;
    case D_Illegal_Transition:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "D_Illegal_Transition\n");
    default:
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "unknown return code\n");
    } /* switch (rtt_lib_return_code) */ 
    return;
} /* dumpRttMonLibReturnCode */ 

void dumpRttMonProcessHeaderString (long rttMonCtrlAdminIndex,
				    char *reasonStr)
{
    char *myProcName;

    myProcName = NULL;
    process_get_name(THIS_PROCESS, (const char **) &myProcName);
    RTT_BUGINF5(rttMonCtrlAdminIndex,
		"RTR %u: %s - %s \n",
		rttMonCtrlAdminIndex,
		reasonStr, myProcName);

    return;
} /* dumpRttMonProcessHeader */ 

void dumpRttMonProcessHeader (long rttMonCtrlAdminIndex,
			      long reasonID)
{
    char *myProcName;

    myProcName = NULL;
    process_get_name(THIS_PROCESS, (const char **) &myProcName);
    RTT_BUGINF5(rttMonCtrlAdminIndex,
		"RTR %u: %s - %s \n",
		rttMonCtrlAdminIndex,
		rtt_ProcessHeader_msgs[reasonID], myProcName);

    return;
} /* dumpRttMonProcessHeader */ 

void dumpRttMonErrorReturnCode (long rttMonCtrlAdminIndex,
				long functionNameID,
				long callNameID)
{
    RTT_BUGINF4(rttMonCtrlAdminIndex,
		"\t in %s on call %s\n",
		rtt_ErrorReturnCodes_msgs[functionNameID],
		rtt_ErrorReturnCodes_msgs[callNameID]);

    return;
} /* dumpRttMonErrorReturnCode */

/*
 * Output Routines for Show Commands
 */

static clock_epoch * getRealEpochTimeFromSysUpTime (unsigned long hseconds)
{
    /* the following static is not good for preemtion */
    static clock_epoch tmp_time;
    unsigned long tmp_secs;

    clock_get_time(&tmp_time);          /* current time */
    tmp_secs = snmp_sysUpTime();	/* in hundredths of seconds */
    /* next get boot time */
    tmp_time.epoch_secs = tmp_time.epoch_secs - (tmp_secs /
						 SYSUPTIME_UNITS);
    /* next get time since boot in SysUpTime */
    tmp_time.epoch_secs = tmp_time.epoch_secs + (hseconds / 100L);
    tmp_time.epoch_frac = 0;

    return (&tmp_time);

} 

static void outputRttMonAdminStatus (long status)
{
    printf("Status of Entry (SNMP RowStatus): %s\n",
	   getRttMonAdminStatusString(status));
} 

static void outputRttMonCtrlOperState (long state)
{
    printf("Operational State of Entry: %s\n",
	   getRttMonCtrlOperStateString(state));
    return;
} 

static void outputOctetString (OctetString * inOctetString)
{
    long i;

    if (inOctetString == NULL) {
	return;
    }
    for (i = 0; i < inOctetString->length; i++) {
	printf("%c", inOctetString->octet_ptr[i]);
    } 
    return;
} 

static void outputHexOctetString (OctetString * inOctetString)
{
    long i;

    if (inOctetString == NULL) {
	return;
    }
    for (i = 0; i < inOctetString->length; i++) {
	printf("%2X ", inOctetString->octet_ptr[i]);
    } 
    return;
} 

static void outputRttMonTargetAddress (long protocolType,
				       OctetString * targetAddress,
				       long rttMonCtrlAdminIndex)
{
    char *hostname;
    char *applname;
    char *modename;
    long OctetStringLen;

    printf("Target Address: ");

    if (targetAddress == NULL) {
	return;
    }
    switch (protocolType) {
    case D_rttMonEchoAdminProtocol_ipIcmpEcho:
	printf("%i", reg_invoke_OctetStringToIP(targetAddress));
	break;
    case D_rttMonEchoAdminProtocol_snaLU0EchoAppl:
    case D_rttMonEchoAdminProtocol_snaLU2EchoAppl:
	reg_invoke_rtt_depend_lu_split_address(targetAddress,
					       &hostname,
					       &OctetStringLen,
					       &applname,
					       &modename,
					       rttMonCtrlAdminIndex);
	printf("%s %s %s",
	       hostname,
	       applname,
	       modename);
	free(hostname);
	free(applname);
	free(modename);

	break;
    case D_rttMonEchoAdminProtocol_snaRUEcho:
    default:
	outputOctetString(targetAddress);
    } 
    return;
} 

void outputRttMonApplHead (void)
{
    printf("\n\tResponse Time Reporter\n");
    return;
} 

void outputRttMonAppl (rttMonAppl_t
		       * rttMonApplPtr)
{
    if (rttMonApplPtr == NULL) {
	RTT_BUGINF2(0,
		    "Bad param to outputRttMonAppl\n");
	return;
    }
    printf("Version: ");
    outputOctetString(rttMonApplPtr->rttMonApplVersion);
    printf("\n");
    printf("Max Packet Data Size (ARR and Data): %u\n",
	   rttMonApplPtr->rttMonApplMaxPacketDataSize);
    printf("Time of Last Change in Whole RTR: %CC", "%v%B\n",
	   getRealEpochTimeFromSysUpTime(rttMonApplPtr->
					 rttMonApplTimeOfLastSet));
    printf("System Max Number of Entries: %u\n",
	   rttMonApplPtr->rttMonApplNumCtrlAdminEntry);
    return;
} 

void outputRttMonApplHeadCompact (void)
{
    printf("\n\tResponse Time Reporter\n");
    printf("\t  Multiple Lines per Entry\n");
    printf("Line 1\n");
    printf("MaxDS   = Max Packet Data Size (ARR and Data)\n");
    printf("LastCh  = Time of Last Change in Whole RTR\n");
    printf("MaxEnt  = System Max Number of Entries\n");
    printf("Line 2 has Version String\n");
    /*80 Wd 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
    /*      12345b1234567890b123 */
    printf("MaxDS LastCh     MaxEnt\n");

    return;
} 

void outputRttMonApplCompact (rttMonAppl_t
			      * rttMonApplPtr)
{
    if (rttMonApplPtr == NULL) {
	RTT_BUGINF2(0,
		    "Bad param to outputRttMonApplCompact\n");
	return;
    }
    printf("%5u %10u %3u\n",
	   rttMonApplPtr->rttMonApplMaxPacketDataSize,
	   rttMonApplPtr->rttMonApplTimeOfLastSet,
	   rttMonApplPtr->rttMonApplNumCtrlAdminEntry);
    outputOctetString(rttMonApplPtr->rttMonApplVersion);

    return;
} 

void outputRttMonApplSupportedRttTypesHead (void)
{
    printf("\n\tSupported Operation Types\n");
    return;
} 

void outputRttMonApplSupportedRttTypesEntry (rttMonApplSupportedRttTypesEntry_t
					     * inRttMonApplSupportedRttTypesEntry)
{
    if (inRttMonApplSupportedRttTypesEntry == NULL) {
	RTT_BUGINF2(0,
		    "Bad param to outputRttMonApplSupportedRttTypesEntry\n");
	return;
    }
    printf("Type of Operation to Perform:  %s\n",
	   getRttMonCtrlAdminRttTypeString(inRttMonApplSupportedRttTypesEntry->
					   rttMonApplSupportedRttTypes));
    return;
} 

void outputRttMonApplSupportedRttTypesHeadCompact (void)
{
    printf("\n\tSupported Operation Types\n");
    printf("Type = Type of Operation to Perform\n");
    /*      1bbbbb */
    printf("Type\n");
    return;
} 

void outputRttMonApplSupportedRttTypesEntryCompact (rttMonApplSupportedRttTypesEntry_t
						    * inRttMonApplSupportedRttTypesEntry)
{
    if (inRttMonApplSupportedRttTypesEntry == NULL) {
	RTT_BUGINF2(0,
		    "Bad param to outputRttMonApplSupportedRttTypesEntryCompact\n");
	return;
    }
    printf(" %1u\n",
	   inRttMonApplSupportedRttTypesEntry->
	   rttMonApplSupportedRttTypes);
    return;
} 

void outputRttMonApplSupportedProtocolsHead (void)
{
    printf("\n\tSupported Protocols\n");
    return;
} 

void outputRttMonApplSupportedProtocolsEntry (rttMonApplSupportedProtocolsEntry_t
					      * inRttMonApplSupportedProtocolsEntry)
{
    if (inRttMonApplSupportedProtocolsEntry == NULL) {
	RTT_BUGINF2(0,
		    "Bad param to outputRttMonApplSupportedProtocolsEntry\n");
	return;
    }
    printf("Protocol Type: %s\n",
	   getRttMonProtocolString(inRttMonApplSupportedProtocolsEntry->
				   rttMonApplSupportedProtocols));
    return;
} 

void outputRttMonApplSupportedProtocolsHeadCompact (void)
{
    printf("\n\tSupported Protocols\n");
    printf("Prt = Protocol Type\n");
    /*      12bbb */
    printf("Prt\n");
    return;
} 

void outputRttMonApplSupportedProtocolsEntryCompact (rttMonApplSupportedProtocolsEntry_t
						     * inRttMonApplSupportedProtocolsEntry)
{
    if (inRttMonApplSupportedProtocolsEntry == NULL) {
	RTT_BUGINF2(0,
		    "Bad param to outputRttMonApplSupportedProtocolsEntryCompact\n");
	return;
    }
    printf(" %2u\n",
	   inRttMonApplSupportedProtocolsEntry->
	   rttMonApplSupportedProtocols);
    return;
} 

void outputRttMonAdminTableHead (void)
{
    printf("\n\tComplete Configuration Table (includes defaults)\n");
    return;
} 

void outputRttMonAdminEntry (rttMonCtrlAdminEntry_t
			     * inRttMonCtrlAdminEntry,
			     rttMonEchoAdminEntry_t
			     * inRttMonEchoAdminEntry)
{
    if ((inRttMonCtrlAdminEntry == NULL) ||
	(inRttMonEchoAdminEntry == NULL)) {
	RTT_BUGINF2(inRttMonCtrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonAdminEntry\n");
	return;
    }
    /*
     * Main Ctrl Admin Table
     */ 
    printf("Entry Number: %u\n", inRttMonCtrlAdminEntry->
	   rttMonCtrlAdminIndex);
    printf("Owner: ");
    outputOctetString(inRttMonCtrlAdminEntry->rttMonCtrlAdminOwner);
    printf("\n");
    printf("Tag: ");
    outputOctetString(inRttMonCtrlAdminEntry->rttMonCtrlAdminTag);
    printf("\n");
    printf("Type of Operation to Perform: %s\n",
	   getRttMonCtrlAdminRttTypeString(inRttMonCtrlAdminEntry->
					   rttMonCtrlAdminRttType));
    printf("Reaction and History Threshold (milliseconds): %u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminThreshold);
    printf("Operation Frequency (seconds): %u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminFrequency);
    printf("Operation Timeout (milliseconds): %u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminTimeout);
    printf("Verify Data: %s\n",
	   getRttMonTrueFalseBooleanString(inRttMonCtrlAdminEntry->
					   rttMonCtrlAdminVerifyData));
    outputRttMonAdminStatus(inRttMonCtrlAdminEntry->
			    rttMonCtrlAdminStatus);
    /*
     * Echo Admin Table 
     */
    printf("Protocol Type: %s\n",
	   getRttMonProtocolString(inRttMonEchoAdminEntry->
				   rttMonEchoAdminProtocol));
    outputRttMonTargetAddress(inRttMonEchoAdminEntry->
			      rttMonEchoAdminProtocol,
			      inRttMonEchoAdminEntry->
			      rttMonEchoAdminTargetAddress,
			      inRttMonCtrlAdminEntry->
			      rttMonCtrlAdminIndex);
    printf("\n");
    printf("Request Size (ARR data portion): %u\n",
	   inRttMonEchoAdminEntry->rttMonEchoAdminPktDataRequestSize);
    printf("Response Size (ARR data portion): %u\n",
	   inRttMonEchoAdminEntry->rttMonEchoAdminPktDataResponseSize);
    /*
     * Schedule Admin Table
     */
    printf("Life (seconds): %u\n",
	   inRttMonCtrlAdminEntry->rttMonScheduleAdminRttLife);
    printf("Next Scheduled Start Time: ");
    if (inRttMonCtrlAdminEntry->
	rttMonScheduleAdminRttStartTime == 0) {
	printf("Pending Trigger\n");
    } else if ((inRttMonCtrlAdminEntry->
		rttMonScheduleAdminRttStartTime == 1) ||
	       (inRttMonCtrlAdminEntry->
		rttMonScheduleAdminRttStartTime <
		snmp_sysUpTime())) {
	printf("Start Time already passed\n");
    } else {
	printf("at %CC", "%v%B\n",
	       getRealEpochTimeFromSysUpTime(inRttMonCtrlAdminEntry->
					     rttMonScheduleAdminRttStartTime));
    }
    printf("Entry Ageout");
    if (inRttMonCtrlAdminEntry->
	rttMonScheduleAdminConceptRowAgeout == 0) {
	printf(": never\n");
    } else {
	printf(" (seconds): %u\n",
	       inRttMonCtrlAdminEntry->
	       rttMonScheduleAdminConceptRowAgeout);
    }		

    /*
     * Reaction Admin Table
     */ 
    printf("Connection Loss Reaction Enabled: %s\n",
	   getRttMonTrueFalseBooleanString(inRttMonCtrlAdminEntry->
					   rttMonReactAdminConnectionEnable));
    printf("Timeout Reaction Enabled: %s\n",
	   getRttMonTrueFalseBooleanString(inRttMonCtrlAdminEntry->
					   rttMonReactAdminTimeoutEnable));
    printf("Threshold Reaction Type: %s\n",
	   getRttMonReactAdminThresholdTypeString(inRttMonCtrlAdminEntry->
						  rttMonReactAdminThresholdType));
    printf("Threshold Falling (milliseconds): %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonReactAdminThresholdFalling);
    printf("Threshold Count: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonReactAdminThresholdCount);
    printf("Threshold Count2: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonReactAdminThresholdCount2);
    printf("Reaction Type: %s\n",
	   getRttMonReactAdminActionTypeString(inRttMonCtrlAdminEntry->
					       rttMonReactAdminActionType));
    /*
     * Statistics Admin
     */
    printf("Number of Statistic Hours kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonStatisticsAdminNumHourGroups);
    printf("Number of Statistic Paths kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonStatisticsAdminNumPaths);
    printf("Number of Statistic Hops kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonStatisticsAdminNumHops);
    printf("Number of Statistic Distribution Buckets kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonStatisticsAdminNumDistBuckets);
    printf("Number of Statistic Distribution Intervals (milliseconds): %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonStatisticsAdminDistInterval);
    /*
     * History Admin
     */
    printf("Number of History Lives kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonHistoryAdminNumLives);
    printf("Number of History Buckets kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonHistoryAdminNumBuckets);
    printf("Number of History Samples kept: %u\n",
	   inRttMonCtrlAdminEntry->
	   rttMonHistoryAdminNumSamples);
    printf("History Filter Type: %s\n",
	   getRttMonHistoryAdminFilterString(inRttMonCtrlAdminEntry->
					     rttMonHistoryAdminFilter));
    printf("\n");
    return;
} 

void outputRttMonAdminTableHeadCompact (void)
{
    printf("\n\tComplete Configuration Table (includes defaults)\n");
    printf("\t  Multiple Lines per Entry\n");
    printf("Line 1\n");
    printf("Entry   = Entry Number\n");
    printf("Type    = Type of Operation to Perform\n");
    printf("Thresh  = Reaction and History Threshold (milliseconds)\n");
    printf("Freq    = Operation Frequency (seconds)\n");
    printf("Timeout = Operation Timeout (milliseconds)\n");
    printf("Ver     = Verify Data\n");
    printf("Sts     = Status of Entry (SNMP RowStatus)\n");
    printf("Prt     = Protocol Type\n");
    printf("ReqSz   = Request Size (ARR data portion)\n");
    printf("RspSz   = Response Size (ARR data portion)\n");
    printf("Life    = Life (seconds)\n");
    printf("Line 2\n");
    printf("Start   = Next Scheduled Start Time (hundredths of seconds)\n");
    printf("Aging   = Entry Ageout (seconds)\n");
    printf("CE      = Connection Loss Reaction Enabled\n");
    printf("TE      = Timeout Reaction Enabled\n");
    printf("TT      = Threshold Reaction Type\n");
    printf("ThrFall = Threshold Falling (milliseconds)\n");
    printf("TC      = Threshold Count\n");
    printf("TC2     = Threshold Count2\n");
    printf("RT      = Reaction Type\n");
    printf("SHrs    = Number of Statistic Hours kept\n");
    printf("SPts    = Number of Statistic Paths kept\n");
    printf("SHps    = Number of Statistic Hops kept\n");
    printf("SDB     = Number of Statistic Distribution Buckets kept\n");
    printf("SDI     = Number of Statistic Distribution Intervals\n");
    printf("Line 3\n");
    printf("HL      = Number of History Lives kept\n");
    printf("HBuck   = Number of History Buckets kept\n");
    printf("HS      = Number of History Samples kept\n");
    printf("HFlt    = History Filter Type\n");
    printf("Line 4 has Owner String\n");
    printf("Line 5 has Tag String\n");
    printf("Line 6 has Target Address\n");
    /*80 Wd 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
    /*      123bbbb1bbb1234567890b123456b123456789bbb1bb1bb12bb12345b12345b1234567890 */
    printf("Entry Type Thresh     Freq   Timeout   Ver Sts Prt ReqSz RspSz Life\n");
    /*      bb1234567890b1234567b1bb1bb1bb1234567890b12b12bb1bb123bb123bb123bb123b123 */
    printf("  Start      Aging   CE TE TT ThrFall    TC TC2 RT SHrs SPts SHps SDB SDI\n");
    /*      bbbb123b1234567890b123bb1bb */
    printf("    HL  HBuck      HS  HFlt\n");
    return;
} 

void outputRttMonAdminEntryCompact (rttMonCtrlAdminEntry_t
				    * inRttMonCtrlAdminEntry,
				    rttMonEchoAdminEntry_t
				    * inRttMonEchoAdminEntry)
{
    if ((inRttMonCtrlAdminEntry == NULL) ||
	(inRttMonEchoAdminEntry == NULL)) {
	RTT_BUGINF2(inRttMonCtrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonAdminEntryCompact\n");
	return;
    }
    printf("%3u    %1u   %10u %6u %9u   %1u  %1u  %2u  %5u %5u %10u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminIndex,
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminRttType,
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminThreshold,
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminFrequency,
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminTimeout,
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminVerifyData,
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminStatus,
	   inRttMonEchoAdminEntry->rttMonEchoAdminProtocol,
	   inRttMonEchoAdminEntry->rttMonEchoAdminPktDataRequestSize,
	   inRttMonEchoAdminEntry->rttMonEchoAdminPktDataResponseSize,
	   inRttMonCtrlAdminEntry->rttMonScheduleAdminRttLife);
    printf("  %10u %7u %1u  %1u  %1u  %10u %2u %2u  %1u  %3u  %3u  %3u  %3u  %3u\n",
	   inRttMonCtrlAdminEntry->rttMonScheduleAdminRttStartTime,
	   inRttMonCtrlAdminEntry->rttMonScheduleAdminConceptRowAgeout,
	   inRttMonCtrlAdminEntry->rttMonReactAdminConnectionEnable,
	   inRttMonCtrlAdminEntry->rttMonReactAdminTimeoutEnable,
	   inRttMonCtrlAdminEntry->rttMonReactAdminThresholdType,
	   inRttMonCtrlAdminEntry->rttMonReactAdminThresholdFalling,
	   inRttMonCtrlAdminEntry->rttMonReactAdminThresholdCount,
	   inRttMonCtrlAdminEntry->rttMonReactAdminThresholdCount2,
	   inRttMonCtrlAdminEntry->rttMonReactAdminActionType,
	   inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumHourGroups,
	   inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumPaths,
	   inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumHops,
	   inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumDistBuckets,
	   inRttMonCtrlAdminEntry->rttMonStatisticsAdminDistInterval);
    printf("    %3u %10u %3u %1u\n",
	   inRttMonCtrlAdminEntry->rttMonHistoryAdminNumLives,
	   inRttMonCtrlAdminEntry->rttMonHistoryAdminNumBuckets,
	   inRttMonCtrlAdminEntry->rttMonHistoryAdminNumSamples,
	   inRttMonCtrlAdminEntry->rttMonHistoryAdminFilter);

    printf("  ");
    outputOctetString(inRttMonCtrlAdminEntry->rttMonCtrlAdminOwner);
    printf("\n");
    printf("  ");
    outputOctetString(inRttMonCtrlAdminEntry->rttMonCtrlAdminTag);
    printf("\n");
    printf("  ");
    outputHexOctetString(inRttMonEchoAdminEntry->
			 rttMonEchoAdminTargetAddress);
    printf("\n");

    return;
} 

void outputRttMonOperTableHead (void)
{

    printf("\n\tCurrent Operational State\n");

    return;
} 

void outputRttMonOperEntry (rttMonEchoAdminEntry_t
			    * inRttMonEchoAdminEntry,
			    rttMonCtrlAdminEntry_t
			    * inRttMonCtrlAdminEntry)
{
    if ((inRttMonCtrlAdminEntry == NULL) ||
	(inRttMonEchoAdminEntry == NULL)) {
	RTT_BUGINF2(inRttMonCtrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonOperEntry\n");
	return;
    }
    printf("Entry Number: %u\n", inRttMonCtrlAdminEntry->
	   rttMonCtrlAdminIndex);
    printf("Modification Time: %CC", "%v%B\n",
	   getRealEpochTimeFromSysUpTime(inRttMonCtrlAdminEntry->
					 rttMonCtrlOperModificationTime));
    printf("Diagnostics Text: ");
    outputOctetString(inRttMonCtrlAdminEntry->rttMonCtrlOperDiagText);
    printf("\n");
    if (inRttMonCtrlAdminEntry->
	rttMonCtrlOperResetTime == 0) {
	printf("Last Time this Entry was Reset: Never\n");
    } else {
	printf("Last Time this Entry was Reset: %CC", "%v%B\n",
	       getRealEpochTimeFromSysUpTime(inRttMonCtrlAdminEntry->
					     rttMonCtrlOperResetTime));
    }
    printf("Number of Octets in use by this Entry: %u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlOperOctetsInUse);
    printf("Connection Loss Occurred: %s\n",
	   getRttMonTrueFalseBooleanString(inRttMonCtrlAdminEntry->
					   rttMonCtrlOperConnectionLostOccurred));
    printf("Timeout Occurred: %s\n",
	   getRttMonTrueFalseBooleanString(inRttMonCtrlAdminEntry->
					   rttMonCtrlOperTimeoutOccurred));
    printf("Over Thresholds Occurred: %s\n",
	   getRttMonTrueFalseBooleanString(inRttMonCtrlAdminEntry->
					   rttMonCtrlOperOverThresholdOccurred));
    printf("Number of Operations Attempted: %u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlOperNumRtts);
    printf("Current Seconds Left in Life: %u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlOperRttLife);
    outputRttMonCtrlOperState(inRttMonCtrlAdminEntry->rttMonCtrlOperState);
    if (inRttMonCtrlAdminEntry->rttMonCtrlOperNumRtts == 0) {
      printf("Latest Completion Time (milliseconds): Unknown\n");
      printf("Latest Operation Return Code: Unknown\n");
      printf("Latest Operation Start Time: Unknown\n");
      printf("Latest ");
      outputRttMonTargetAddress(inRttMonEchoAdminEntry->
				rttMonEchoAdminProtocol,
				NULL,
				inRttMonCtrlAdminEntry->
				rttMonCtrlAdminIndex);
      printf("Unkown");
    } else {
      printf("Latest Completion Time (milliseconds): %u\n",
	     inRttMonCtrlAdminEntry->
	     rttMonLatestRttOperCompletionTime);
      printf("Latest Operation Return Code: %s\n",
	     getRttMonOperSenseString(inRttMonCtrlAdminEntry->
				      rttMonLatestRttOperSense));
      printf("Latest Operation Start Time: %CC", "%v%B\n",
	     getRealEpochTimeFromSysUpTime(inRttMonCtrlAdminEntry->
					   rttMonLatestRttOperTime));
      printf("Latest ");
      outputRttMonTargetAddress(inRttMonEchoAdminEntry->
				rttMonEchoAdminProtocol,
				inRttMonCtrlAdminEntry->
				rttMonLatestRttOperAddress,
				inRttMonCtrlAdminEntry->
				rttMonCtrlAdminIndex);
    }
    printf("\n\n");

    return;
}

void outputRttMonOperTableHeadCompact (void)
{

    printf("\n\tCurrent Operational State\n");
    printf("\t  Multiple Lines per Entry\n");
    printf("Line 1\n");
    printf("Entry    = Entry Number\n");
    printf("ModTime  = Modification Time (hundredths of seconds)\n");
    printf("ResetT   = Last Time this Entry was Reset (hundredths of seconds)\n");
    printf("Octets   = Number of Octets in use by this Entry\n");
    printf("CL       = Connection Loss Occurred\n");
    printf("TO       = Timeout Occurred\n");
    printf("OT       = Over Thresholds Occurred\n");
    printf("NumRtts  = Number of Operations Attempted\n");
    printf("RttLife  = Current Seconds Left in Life\n");
    printf("OS       = Operational State of Entry\n");
    printf("Line 2\n");
    printf("CompT    = Latest Completion Time (milliseconds)\n");
    printf("RC       = Latest Operation Return Code\n");
    printf("StartT   = Latest Operation Start Time\n");
    printf("Line 3 has Diagnostic Text\n");
    printf("Line 4 has Latest Target Address\n");
    /*80 Wd 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
    /*      123bbb1234567890b1234567890b1234567890b1bb1bb1bb1234567890b1234567890b1b */
    printf("Entry ModTime    ResetT     Octets     CL TO OT NumRtts    RttLife    OS\n");
    /*      bb1234567890b1bb1234567890b */
    printf("  CompT      RC StartT\n");
    return;
} 

void outputRttMonOperEntryCompact (rttMonCtrlAdminEntry_t
				   * inRttMonCtrlAdminEntry)
{
    if (inRttMonCtrlAdminEntry == NULL) {
	RTT_BUGINF2(inRttMonCtrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonOperEntryCompact\n");
	return;
    }
    printf("%3u   %10u %10u %10u %1u  %1u  %1u  %10u %10u %1u\n",
	   inRttMonCtrlAdminEntry->rttMonCtrlAdminIndex,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperModificationTime,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperResetTime,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperOctetsInUse,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperConnectionLostOccurred,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperTimeoutOccurred,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperOverThresholdOccurred,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperNumRtts,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperRttLife,
	   inRttMonCtrlAdminEntry->rttMonCtrlOperState);
    printf("  %10u %1u  %10u\n",
	   inRttMonCtrlAdminEntry->rttMonLatestRttOperCompletionTime,
	   inRttMonCtrlAdminEntry->rttMonLatestRttOperSense,
	   inRttMonCtrlAdminEntry->rttMonLatestRttOperTime);
    printf("  ");
    outputOctetString(inRttMonCtrlAdminEntry->
		      rttMonCtrlOperDiagText);
    printf("\n");
    printf("  ");
    outputHexOctetString(inRttMonCtrlAdminEntry->
			 rttMonLatestRttOperAddress);
    printf("\n");

    return;
} 

void outputRttMonStatsCaptureTableHead (void)
{
    printf("\n\tCaptured Statistics\n");
    return;
} 

void outputRttMonStatsCaptureEntry (rttMonStatsCaptureEntry_t
				    * inRttMonStatsCaptureEntry)
{
    if (inRttMonStatsCaptureEntry == NULL) {
	RTT_BUGINF2(inRttMonStatsCaptureEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonStatsCaptureEntry\n");
	return;
    }
    printf("Entry Number: %u\n", inRttMonStatsCaptureEntry->
	   rttMonCtrlAdminIndex);
    printf("Start Time Index: %CC", "%v%B\n",
	   getRealEpochTimeFromSysUpTime(inRttMonStatsCaptureEntry->
					 rttMonStatsCaptureStartTimeIndex));
    printf("Path Index: %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCapturePathIndex);
    printf("Hop in Path Index: %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureHopIndex);
    printf("Time Distribution Index: %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureDistIndex);
    printf("Operations Completed: %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureCompletions);
    printf("Operations Completed Over Thresholds: %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureOverThresholds);
    printf("Sum of Completion Times (milliseconds): %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureSumCompletionTime);
    printf("Sum of Completion Times Squared Low 32 Bits (milliseconds): %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureSumCompletionTime2Low);
    printf("Sum of Completion Times Squared High 32 Bits (milliseconds): %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureSumCompletionTime2High);
    printf("Completion Time Maximum (milliseconds): %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureCompletionTimeMax);
    printf("Completion Time Minimum (milliseconds): %u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureCompletionTimeMin);
    printf("\n");

    return;
} 

void outputRttMonStatsCaptureTableHeadCompact (void)
{
    printf("\n\tCaptured Statistics\n");
    printf("\t  Multiple Lines per Entry\n");
    printf("Line 1\n");
    printf("Entry    = Entry Number\n");
    printf("StartT   = Start Time of Entry (hundredths of seconds)\n");
    printf("Pth      = Path Index\n");
    printf("Hop      = Hop in Path Index\n");
    printf("Dst      = Time Distribution Index\n");
    printf("Comps    = Operations Completed\n");
    printf("OvrTh    = Operations Completed Over Thresholds\n");
    printf("SumCmp   = Sum of Completion Times (milliseconds)\n");
    printf("Line 2\n");
    printf("SumCmp2L = Sum of Completion Times Squared Low 32 Bits (milliseconds)\n");
    printf("SumCmp2H = Sum of Completion Times Squared High 32 Bits (milliseconds)\n");
    printf("TMax     = Completion Time Maximum (milliseconds)\n");
    printf("TMin     = Completion Time Minimum (milliseconds)\n");
    /*80 Wd 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
    /*      123bbb1234567890b123b123b123b1234567890b1234567890b1234567890 */
    printf("Entry StartT     Pth Hop Dst Comps      OvrTh      SumCmp\n");
    /*      bb1234567890b1234567890b1234567890b1234567890 */
    printf("  SumCmp2L   SumCmp2H   TMax       TMin\n");

    return;
} 

void outputRttMonStatsCaptureEntryCompact (rttMonStatsCaptureEntry_t
					   * inRttMonStatsCaptureEntry)
{
    if (inRttMonStatsCaptureEntry == NULL) {
	RTT_BUGINF2(inRttMonStatsCaptureEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonStatsCaptureEntryCompact\n");
	return;
    }
    printf("%3u   %10u %3u %3u %3u %10u %10u %10u\n",
	   inRttMonStatsCaptureEntry->rttMonCtrlAdminIndex,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureStartTimeIndex,
	   inRttMonStatsCaptureEntry->rttMonStatsCapturePathIndex,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureHopIndex,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureDistIndex,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureCompletions,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureOverThresholds,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureSumCompletionTime);
    printf("  %10u %10u %10u %10u\n",
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureSumCompletionTime2Low,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureSumCompletionTime2High,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureCompletionTimeMax,
	   inRttMonStatsCaptureEntry->rttMonStatsCaptureCompletionTimeMin);

    return;
} 

void outputRttMonStatsCollectTableHead (void)
{
    printf("\n\tCollected Statistics\n");
    return;
} 

void outputRttMonStatsCollectEntry (rttMonEchoAdminEntry_t
				    * inEchoAdminEntry,
				    rttMonStatsCollectEntry_t
				    * inRttMonStatsCollectEntry)
{
    if ((inRttMonStatsCollectEntry == NULL) ||
	(inEchoAdminEntry == NULL)) {
	RTT_BUGINF2(inRttMonStatsCollectEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonStatsCollectEntry\n");
	return;
    }
    printf("Entry Number: %u\n",
	   inRttMonStatsCollectEntry->rttMonCtrlAdminIndex);
    printf("Start Time Index: %CC", "%v%B\n",
	   getRealEpochTimeFromSysUpTime(inRttMonStatsCollectEntry->
					 rttMonStatsCaptureStartTimeIndex));
    printf("Path Index: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCapturePathIndex);
    printf("Hop in Path Index: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCaptureHopIndex);
    printf("Number of Failed Operations due to a Disconnect: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectNumDisconnects);
    printf("Number of Failed Operations due to a Timeout: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectTimeouts);
    printf("Number of Failed Operations due to a Busy: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectBusies);
    printf("Number of Failed Operations due to a No Connection: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectNoConnections);
    printf("Number of Failed Operations due to an Internal Error: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectDrops);
    printf("Number of Failed Operations due to a Sequence Error: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectSequenceErrors);
    printf("Number of Failed Operations due to a Verify Error: %u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectVerifyErrors);
    outputRttMonTargetAddress(inEchoAdminEntry->
			      rttMonEchoAdminProtocol,
			      inRttMonStatsCollectEntry->
			      rttMonStatsCollectAddress,
			      inEchoAdminEntry->
			      rttMonCtrlAdminIndex);
    printf("\n\n");
    return;
} 

void outputRttMonStatsCollectTableHeadCompact (void)
{
    printf("\n\tCollected Statistics\n");
    printf("\t  Multiple Lines per Entry\n");
    printf("Line 1\n");
    printf("Entry    = Entry Number\n");
    printf("StartT   = Start Time Index\n");
    printf("Pth      = Which Path Index\n");
    printf("Hop      = Which Hop in Above Path Index\n");
    printf("Disc     = Number of Disconnects\n");
    printf("TimeOut  = Number of Timeouts\n");
    printf("Busies   = Number of Busies\n");
    printf("NoConn   = Number of No Connections\n");
    printf("Line 2\n");
    printf("Drops    = Number of Drops\n");
    printf("SeqEr    = Number of Sequence Errors\n");
    printf("VerEr    = Number of Verify Errors\n");
    printf("Line 3 has the Target Address\n");
    /*80 Wd 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
    /*      123bbb1234567890b123b123b1234567890b1234567890b1234567890b1234567890 */
    printf("Entry StartT     Pth Hop Disc       TimeOut    Busies     NoConn\n");
    /*      bb1234567890b1234567890b1234567890 */
    printf("  Drops      SeqEr     VerEr\n");

    return;
} 

void outputRttMonStatsCollectEntryCompact (rttMonStatsCollectEntry_t
					   * inRttMonStatsCollectEntry)
{
    if (inRttMonStatsCollectEntry == NULL) {
	RTT_BUGINF2(inRttMonStatsCollectEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonStatsCollectEntryCompact\n");
	return;
    }
    printf("%3u   %10u %3u %3u %10u %10u %10u %10u\n",
	   inRttMonStatsCollectEntry->rttMonCtrlAdminIndex,
	   inRttMonStatsCollectEntry->rttMonStatsCaptureStartTimeIndex,
	   inRttMonStatsCollectEntry->rttMonStatsCapturePathIndex,
	   inRttMonStatsCollectEntry->rttMonStatsCaptureHopIndex,
	   inRttMonStatsCollectEntry->rttMonStatsCollectNumDisconnects,
	   inRttMonStatsCollectEntry->rttMonStatsCollectTimeouts,
	   inRttMonStatsCollectEntry->rttMonStatsCollectBusies,
	   inRttMonStatsCollectEntry->rttMonStatsCollectNoConnections);
    printf("  %10u %10u %10u\n",
	   inRttMonStatsCollectEntry->rttMonStatsCollectDrops,
	   inRttMonStatsCollectEntry->rttMonStatsCollectSequenceErrors,
	   inRttMonStatsCollectEntry->rttMonStatsCollectVerifyErrors);
    printf("  ");
    outputHexOctetString(inRttMonStatsCollectEntry->
			 rttMonStatsCollectAddress);
    printf("\n");

    return;
} 

void outputRttMonStatsTotalsTableHead (void)
{
    printf("\n\tStatistic Totals\n");

    return;
} 

void outputRttMonStatsTotalsEntry (rttMonStatsTotalsEntry_t
				   * inRttMonStatsTotalsEntry)
{
    if (inRttMonStatsTotalsEntry == NULL) {
	RTT_BUGINF2(inRttMonStatsTotalsEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonStatsTotalsEntry\n");
	return;
    }
    printf("Entry Number: %u\n",
	   inRttMonStatsTotalsEntry->rttMonCtrlAdminIndex);
    printf("Start Time Index: %CC", "%v%B\n",
	   getRealEpochTimeFromSysUpTime(inRttMonStatsTotalsEntry->
					 rttMonStatsCaptureStartTimeIndex));
    printf("Age of Statistics Entry (hundredths of seconds): %u\n",
	   inRttMonStatsTotalsEntry->
	   rttMonStatsTotalsElapsedTime);
    printf("Number of Initiations: %u\n",
	   inRttMonStatsTotalsEntry->rttMonStatsTotalsInitiations);
    printf("\n");

    return;
} 

void outputRttMonStatsTotalsTableHeadCompact (void)
{
    printf("\n\tStatistic Totals\n");
    printf("Entry        = Entry Number\n");
    printf("StartT       = Start Time Index\n");
    printf("EntryAgeT    = Age of Statistics Entry (hundredths of seconds)\n");
    printf("Initiations  = Number of Initiations\n");
    /*      123bbb1234567890b1234567890b1234567890 */
    printf("Entry StartT     EntryAgeT  Initiations\n");

    return;
} 

void outputRttMonStatsTotalsEntryCompact (rttMonStatsTotalsEntry_t
					  * inRttMonStatsTotalsEntry)
{
    if (inRttMonStatsTotalsEntry == NULL) {
	RTT_BUGINF2(inRttMonStatsTotalsEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonStatsTotalsEntryCompact\n");
	return;
    }
    printf("%3u   %10u %10u %10u\n",
	   inRttMonStatsTotalsEntry->
	   rttMonCtrlAdminIndex,
	   inRttMonStatsTotalsEntry->
	   rttMonStatsCaptureStartTimeIndex,
	   inRttMonStatsTotalsEntry->
	   rttMonStatsTotalsElapsedTime,
	   inRttMonStatsTotalsEntry->
	   rttMonStatsTotalsInitiations);

    return;
} 

void outputRttMonHistoryCollectionTableHead (void)
{
    printf("\n\tPoint by point History\n");

    return;
} 

void outputRttMonHistoryCollectionEntry (rttMonEchoAdminEntry_t
					 * inEchoAdminEntry,
					 rttMonHistoryCollectionEntry_t
					 * inRttMonHistoryCollectionEntry)
{
    if ((inEchoAdminEntry == NULL) ||
	(inRttMonHistoryCollectionEntry == NULL)) {
	RTT_BUGINF2(inRttMonHistoryCollectionEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonHistoryCollectionEntry\n");
	return;
    }
    printf("Entry Number: %u\n",
	   inRttMonHistoryCollectionEntry->rttMonCtrlAdminIndex);
    printf("Life Index: %u\n",
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionLifeIndex);
    printf("Bucket Index: %u\n",
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionBucketIndex);
    printf("Sample Index: %u\n",
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionSampleIndex);
    printf("Sample Time: %CC", "%v%B\n",
	   getRealEpochTimeFromSysUpTime(inRttMonHistoryCollectionEntry->
					 rttMonHistoryCollectionSampleTime));
    printf("Completion Time (milliseconds): %u\n",
	   inRttMonHistoryCollectionEntry->
	   rttMonHistoryCollectionCompletionTime);
    printf("Response Return Code: %s\n",
	   getRttMonOperSenseString(inRttMonHistoryCollectionEntry->
				    rttMonHistoryCollectionSense));
    outputRttMonTargetAddress(inEchoAdminEntry->
			      rttMonEchoAdminProtocol,
			      inRttMonHistoryCollectionEntry->
			      rttMonHistoryCollectionAddress,
			      inEchoAdminEntry->
			      rttMonCtrlAdminIndex);
    printf("\n\n");

    return;
}

void outputRttMonHistoryCollectionTableHeadCompact (void)
{
    printf("\n\tPoint by point History\n");
    printf("\t  Multiple Lines per Entry\n");
    printf("Line 1\n");
    printf("Entry    = Entry Number\n");
    printf("LifeI    = Life Index\n");
    printf("BucketI  = Bucket Index\n");
    printf("SampleI  = Sample Index\n");
    printf("SampleT  = Sample Start Time\n");
    printf("CompT    = Completion Time (milliseconds)\n");
    printf("Sense    = Response Return Code\n");
    printf("Line 2 has the Target Address\n");
    /*80 Wd 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
    /*      123bbb1234567890b1234567890b1234567890b1234567890b1234567890bb1 */
    printf("Entry LifeI      BucketI    SampleI    SampleT    CompT      Sense\n");

    return;
} 

void outputRttMonHistoryCollectionEntryCompact (rttMonHistoryCollectionEntry_t
						* inRttMonHistoryCollectionEntry)
{
    if (inRttMonHistoryCollectionEntry == NULL) {
	RTT_BUGINF2(inRttMonHistoryCollectionEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonHistoryCollectionEntryCompact\n");
	return;
    }
    printf("%3u   %10u %10u %10u %10u %10u  %1u\n",
	   inRttMonHistoryCollectionEntry->rttMonCtrlAdminIndex,
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionLifeIndex,
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionBucketIndex,
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionSampleIndex,
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionSampleTime,
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionCompletionTime,
	   inRttMonHistoryCollectionEntry->rttMonHistoryCollectionSense);
    printf("  ");
    outputHexOctetString(inRttMonHistoryCollectionEntry->
			 rttMonHistoryCollectionAddress);
    printf("\n");

    return;
} 

static void outputRttMonReactTriggerOperState (long state)
{
    printf("Operational State: %s\n",
	   getRttMonReactTriggerOperStateString(state));
    return;
} 

void outputRttMonReactTriggerTableHead (void)
{
    printf("\n\tReaction Table\n");
    return;
} 

void outputRttMonReactTriggerEntry (rttMonReactTriggerAdminEntry_t
				    * inRttMonReactTriggerAdminEntry)
{
    if (inRttMonReactTriggerAdminEntry == NULL) {
	RTT_BUGINF2(inRttMonReactTriggerAdminEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonReactTriggerEntry\n");
	return;
    }
    printf("Entry Number: %u\n", inRttMonReactTriggerAdminEntry->
	   rttMonCtrlAdminIndex);
    printf("Target Entry Number: %u\n", inRttMonReactTriggerAdminEntry->
	   rttMonReactTriggerAdminRttMonCtrlAdminIndex);
    outputRttMonAdminStatus(inRttMonReactTriggerAdminEntry->
			    rttMonReactTriggerAdminStatus);
    outputRttMonReactTriggerOperState(inRttMonReactTriggerAdminEntry->
				      rttMonReactTriggerOperState);
    printf("\n");

    return;
} 

void outputRttMonReactTriggerTableHeadCompact (void)
{
    printf("\n\tReaction Table\n");
    printf("Entry   = Entry Number\n");
    printf("Target  = Target Entry Number\n");
    printf("Status  = Status of Entry (SNMP RowStatus)\n");
    printf("State   = Operational State of Entry\n");
    /*      1234bbb1234bbbb1bbbbb1b */
    printf("Entry Target Status State\n");
    return;
} 

void outputRttMonReactTriggerEntryCompact (rttMonReactTriggerAdminEntry_t
					   * inRttMonReactTriggerAdminEntry)
{
    if (inRttMonReactTriggerAdminEntry == NULL) {
	RTT_BUGINF2(inRttMonReactTriggerAdminEntry->
		    rttMonCtrlAdminIndex,
		    "Bad param to outputRttMonReactTriggerEntryCompact\n");
	return;
    }
    printf("%4u   %4u    %1u     %1u\n",
	   inRttMonReactTriggerAdminEntry->
	   rttMonCtrlAdminIndex,
	   inRttMonReactTriggerAdminEntry->
	   rttMonReactTriggerAdminRttMonCtrlAdminIndex,
	   inRttMonReactTriggerAdminEntry->
	   rttMonReactTriggerAdminStatus,
	   inRttMonReactTriggerAdminEntry->
	   rttMonReactTriggerOperState);
    return;
}

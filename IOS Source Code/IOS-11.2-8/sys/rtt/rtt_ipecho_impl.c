/* $Id: rtt_ipecho_impl.c,v 1.1.4.8 1996/08/28 13:08:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_ipecho_impl.c,v $
 *------------------------------------------------------------------
 * IP Echo Probe Application.
 *
 * Apr 1996, Larry Metzger
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rtt_ipecho_impl.c,v $
 * Revision 1.1.4.8  1996/08/28  13:08:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.7  1996/07/29  22:30:59  snyder
 * CSCdi64483:  declare arrays const in sub__rtt_mon
 *              788 out of data, 12 out of image
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/06/28  23:25:30  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/07  22:26:10  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/04  18:28:38  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.3  1996/06/04  00:23:25  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * verifyerror -> verify error
 * sequenceerror -> sequence error
 * Mayhave -> May have
 *
 * Revision 1.1.4.2  1996/05/22  17:51:36  snyder
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
 * Revision 1.1.4.1  1996/05/17  11:41:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/05/14  06:20:38  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1  1996/05/14  00:34:46  lmetzger
 * Placeholder for Response Time Reporter code review updates
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../rtt/msg_rtt.c"		/* Not a typo, see logger.h */
#include "rtt_ipecho_impl.h"

#define TMPBUFF_SIZE 80

static void iAmPanicking (rttMonCtrlAdminQItem
			  * inRttMonCtrlAdminQItem,
			  long IDofMySelf)
{
    reg_invoke_rttMonProbePanic(inRttMonCtrlAdminQItem);
    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
				       RttIPPanicMsg);
    process_kill(THIS_PROCESS);

    return;
}

/* NOTE: this return code has 4 possible values: 
 *       D_echoTarget_timeout = 1, 
 *       Not Used Here D_echoTarget_connectionLost = 2,
 *       Not Used Here D_echoTarget_internalError = 3,
 *       D_echoTarget_ayyOK = 4
 */ 

static long echoTarget (long payloadSize,
			echotype * inEchoPtr,
			long echoTimeout,
			ipaddrtype sourceAddress,
			ipaddrtype targetAddress,
			long IDofMySelf)
{
    boolean return_code;

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttEchoTargetCalled);
	RTT_BUGINF3(IDofMySelf,
		    "\t echoTimeout (milliseconds) = %u\n", echoTimeout);
	RTT_BUGINF2(IDofMySelf,
		    "\t calling send_echo...\n");
    }
    TIMER_START(inEchoPtr->timeout, echoTimeout);
    /* echo_send */
    return_code =
	reg_invoke_ip_send_echo(targetAddress,
				inEchoPtr,
				MIN_ICMP_PKT + payloadSize,
				PING_PATTERN,
				sourceAddress,
				FALSE,
				NULL);
    if (!return_code) {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttWokeOnTimeout);
	}
	return (D_echoTarget_timeout);
    } else {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttWokeOnRealEvent);
	}
	return (D_echoTarget_ayyOK);
    } /* compiler doesn't like it, but cannot get here */ 
    return (D_echoTarget_timeout);

} /* echoTarget */ 

static void setupPathInfo (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
			   long echoType,
			   OctetString * targetOctetString,
			   long IDofMySelf)
{
    OctetString *tmpOS;
    long return_code = D_Return_Successfull;
    tracetype *data;
    ipaddrtype source;			/* source IP address */
    ipaddrtype destination;		/* destination of trace route */
    ushort probe;			/* current probe */
    ushort port;			/* start of dest port range */
    int ttl;


    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttSetupPathInfoCalled);
	RTT_BUGINF2(IDofMySelf,
		    "\t Calling rttMonStartUpdatePath\n");
    }
    reg_invoke_rttMonStartUpdatePath(inRttMonCtrlAdminQItem);

    destination = reg_invoke_OctetStringToIP(targetOctetString);

    if (echoType == D_rttMonCtrlAdminRttType_pathEcho) {
	/* 
	 *  Do TraceRoute and call rttMonUpdatePath for
	 *    each hop.
	 */
	/*
	 * Set default parameters
	 */
	source = 0;
	port = 33434;

	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf, RttFindingRoute);
	    RTT_BUGINF3(IDofMySelf, "\t Destination of %i\n",
			destination);
	}
	for (ttl = MINTTL; ttl <= MAXTTL; ttl++) {
	    for (probe = 0; probe < NPROBES; ++probe) {
		data = malloc(sizeof(tracetype));
		if (data) {
		    data->interactive = FALSE; /* This function is never interactive */
		    data->sport = random_gen() | 0x8000; /* make sure port is not low */
		    data->ttl = ttl;
		    data->dport = port++;
		    data->options = NULL;
		    data->whichoptions = 0;
		    if (RTT_TRACE(IDofMySelf)) {
			RTT_BUGINF4(IDofMySelf,
				    "\t Sending Packet to %i with ttl = %u\n",
				    destination,
				    data->ttl);
		    }
		    TIMER_START(data->timeout, WAITSECS * ONESEC);
		    /*
		     * Actually send a packet
		     */
		    reg_invoke_ip_trace_sendprobe(source, destination, data);
		    switch (data->how) {
		    case 'Q':		/* source quench received */
		    case '!':		/* success ! */
			if (RTT_TRACE(IDofMySelf)) {
			    RTT_BUGINF4(IDofMySelf,
					"\t Return Value of %c for hop %i\n",
					data->how,
					data->who);
			    RTT_BUGINF2(IDofMySelf,
					"\t Calling Update Path, then will look for next hop\n");
			}
			tmpOS = reg_invoke_rttMonPreMallocOctetString(RTT_RETVAL_NO_OP,
								      4L);
			reg_invoke_rttMonIPToOctetString(data->who, tmpOS);
			return_code =
			    reg_invoke_rttMonUpdatePath(RTT_RETVAL_NO_OP,
							inRttMonCtrlAdminQItem,
							tmpOS);
			if (return_code != D_Return_Successfull) {
			    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							       RttErrorReturnCode);
			    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
								 RttSetupPathInfoFName,
								 RttMonUpdatePathName);
			    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
							       return_code);
			}
			reg_invoke_rttMonFreeOctetStringBuffer(tmpOS);
			probe = NPROBES; /* find once */
			break;
		    case 'U':		/* got to the dest - quit now */
		    case 'A':
		    case 'N':
		    case 'H':
		    case 'P':
			if (RTT_TRACE(IDofMySelf)) {
			    switch (data->how) {
			    case 'U':
				RTT_BUGINF2(IDofMySelf,
					    "\t Got There, (Port Unreachable) ");
				break;
			    case 'A':
				RTT_BUGINF2(IDofMySelf,
					    "\t Cannot get There, (Administratively Unreachable) ");
				break;
			    case 'N':
				RTT_BUGINF2(IDofMySelf,
					    "\t Cannot get There, (Unreachable Network) ");
				break;
			    case 'H':
				RTT_BUGINF2(IDofMySelf,
					    "\t Cannot get There, (Unreachable Host) ");
				break;
			    case 'P':
				RTT_BUGINF2(IDofMySelf,
					    "\t May have got There, (Unreachable Protocol) ");
				break;
			    default:
				RTT_BUGINF2(IDofMySelf,
					    "\t Cannot get There, ");
				break;
			    }
			    RTT_BUGINF4(IDofMySelf,
					"\t Return Value of %c for target %i\n",
					data->how,
					data->who);
			    RTT_BUGINF2(IDofMySelf,
					"\t Calling Update Path, then am done\n");
			}
			tmpOS = reg_invoke_rttMonPreMallocOctetString(RTT_RETVAL_NO_OP,
								      4L);
			reg_invoke_rttMonIPToOctetString(data->who, tmpOS);
			return_code =
			    reg_invoke_rttMonUpdatePath(RTT_RETVAL_NO_OP,
							inRttMonCtrlAdminQItem,
							tmpOS);
			if (return_code != D_Return_Successfull) {
			    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							       RttErrorReturnCode);
			    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
								 RttSetupPathInfoFName,
								 RttMonUpdatePathName);
			    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
							       return_code);
			}
			reg_invoke_rttMonFreeOctetStringBuffer(tmpOS);
			probe = NPROBES; /* find once */
			ttl = MAXTTL;	/* got to the target */
			break;
		    case 'T':		/* timeout */
			if (RTT_TRACE(IDofMySelf)) {
			    RTT_BUGINF2(IDofMySelf,
					"\t Received a timeout\n");
			    RTT_BUGINF2(IDofMySelf,
					"\t Will Try Again\n");
			}
			break;
		    default:
			if (RTT_TRACE(IDofMySelf)) {
			    RTT_BUGINF4(IDofMySelf,
					"\t Received an unknown Return Code of %u for %i\n",
					data->how,
					data->who);
			    RTT_BUGINF2(IDofMySelf,
					"\t Will Try Again\n");
			}
			break;
		    } /* switch */ 
		    free(data);
		} /* malloc of data failed */ 
	    } /* for  probe */ 
	} /* for  ttl */ 
    } else {				/* source to target echo */
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingUpdatePath);
	    RTT_BUGINF3(IDofMySelf,
			"\t for Target %i\n", destination);
	}
	return_code =
	    reg_invoke_rttMonUpdatePath(RTT_RETVAL_NO_OP,
					inRttMonCtrlAdminQItem,
					targetOctetString);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
						 RttSetupPathInfoFName,
						 RttMonUpdatePathName);
	    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
					       return_code);
	}
    }
    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttCallingEndUpdatePath);
    }
    return_code =
	reg_invoke_rttMonEndUpdatePath(RTT_RETVAL_NO_OP,
				       inRttMonCtrlAdminQItem);
    if (return_code != D_Return_Successfull) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSetupPathInfoFName,
					     RttMonEndUpdatePathName);
	reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
					   return_code);
    }
    return;
} /* setupPathInfo */ 

static void performEcho (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
			 OctetString * destOctetString,
			 unsigned short *sequenceNum,
			 long echoTimeout,
			 long payloadSize,
			 boolean verifyDataFlag,
			 boolean InitialAttempt,
			 long IDofMySelf)
{
    unsigned long responseTime;
    long return_code;
    long hopReturncode;
    long echo_return_code;
    boolean goodResponse;
    echotype tmpEchoStruct;
    ipaddrtype targetAddress;
    ipaddrtype sourceAddress;
    ulong echoTypeId;

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttPerformEchoCalled);
	RTT_BUGINF4(IDofMySelf,
		    "\t echoTimeout (milliseconds) = %u  sequenceNum = %u\n",
		    echoTimeout, *sequenceNum);
	RTT_BUGINF4(IDofMySelf,
		    "\t verifyDataFlag = %s  InitialAttempt = %s\n",
		    verifyDataFlag ? "True" : "False",
		    InitialAttempt ? "True" : "False");
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttCallingGetFirstStoredHop);
    }
    hopReturncode =
	reg_invoke_rttMonGetFirstStoredHopAddress(RTT_RETVAL_NO_OP,
						  inRttMonCtrlAdminQItem,
						  destOctetString);
    if (hopReturncode != D_Return_Successfull) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttPerformEchoFName,
					     RttMonGetFirstStoredHopAddressName);
	reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
					   hopReturncode);
	return;
    }
    (*sequenceNum)++;
    /* Setup echotype struct here */
    tmpEchoStruct.validate = FALSE;
    tmpEchoStruct.sequence = *sequenceNum;
    tmpEchoStruct.broadcast = FALSE;
    tmpEchoStruct.interactive = FALSE;
    tmpEchoStruct.pattern = PING_PATTERN;
    tmpEchoStruct.rotating_pattern = FALSE;
    tmpEchoStruct.tos = 0;
    tmpEchoStruct.ttl = TTL_DEFAULT;
    tmpEchoStruct.options = NULL;
    echoTypeId = 1;

    while (hopReturncode == D_Return_Successfull) {
	tmpEchoStruct.id = echoTypeId++;
	tmpEchoStruct.how = ' ';
	targetAddress =
	    reg_invoke_OctetStringToIP(destOctetString);
	sourceAddress =
	    ip_best_local_address(targetAddress,
				  FALSE);
	responseTime = 0;
	/*
	 *  The following is called right before the
	 *    echo attempt.  
	 *  This can also be gotten from the ping code below, but
	 *    we are doing our own, in an attempt to make the
	 *    statistic the same for all protocols.
	 */
	reg_invoke_rttMonTimeStampEchoStart(inRttMonCtrlAdminQItem);
	/* NOTE: this return code hase 2 possible 
	 *  values: 
	 *  D_echoTarget_timeout = 1, 
	 *  D_echoTarget_ayyOK = 4 */
	echo_return_code =
	    echoTarget(payloadSize,
		       &tmpEchoStruct,
		       echoTimeout,
		       sourceAddress,
		       targetAddress,
		       IDofMySelf);
	/* calculate response_time */
	responseTime =
	    reg_invoke_rttMonCalculateResponseTime(RTT_RETVAL_NO_OP,
						   inRttMonCtrlAdminQItem);
	/* handle echo return_code */
	if ((echo_return_code == D_echoTarget_timeout) ||
	    (responseTime > echoTimeout)) {
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttReturnedFromEchoTarget);
		RTT_BUGINF2(IDofMySelf,
			    "\t with D_echoTarget_timeout return_code\n");
		RTT_BUGINF3(IDofMySelf,
			    "\t and rtr responseTime (milliseconds) = %u\n",
			    responseTime);
		RTT_BUGINF3(IDofMySelf,
			    "\t with native responseTime (milliseconds) = %d\n",
			    tmpEchoStruct.deltatime);
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttCallingUpdateResponseTimeout);
	    }
	    return_code =
		reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						inRttMonCtrlAdminQItem,
						0,
						destOctetString,
						D_rttMonHistoryCollectionSense_timeout,
						InitialAttempt);
	    if (return_code != D_Return_Successfull) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
						     RttPerformEchoFName,
						     RttMonUpdateResponseTimeoutName);
		reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						   return_code);
	    }
	} else {
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttReturnedFromEchoTarget);
		RTT_BUGINF2(IDofMySelf,
			    "\t with D_echoTarget_ayyOK return_code\n");
		RTT_BUGINF3(IDofMySelf,
			    "\t and rtr responseTime (milliseconds) = %u\n",
			    responseTime);
		RTT_BUGINF3(IDofMySelf,
			    "\t with native responseTime (milliseconds) = %d\n",
			    tmpEchoStruct.deltatime);
	    }
	    goodResponse = TRUE;
	    /* if verify data, verify data */
	    if (verifyDataFlag) {
		if (tmpEchoStruct.how == 'C') {
		    goodResponse = FALSE;
		    if (RTT_TRACE(IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							   RttCallingUpdateResponseVerifyError);
		    }
		    return_code =
			reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
							inRttMonCtrlAdminQItem,
							0,
							destOctetString,
							D_rttMonHistoryCollectionSense_verifyerror,
							InitialAttempt);
		    if (return_code != D_Return_Successfull) {
			reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
							     RttPerformEchoFName,
							     RttMonUpdateResponseVerifyErrorName);
			reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
							   return_code);
		    }
		    if (RTT_TRACE(IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							   RttCorruptedReplyToRequest);
			RTT_BUGINF5(IDofMySelf,
				    "\t pattern = %-0#6x, offset %d, %d words corrupted",
				    tmpEchoStruct.corruption,
				    tmpEchoStruct.corruption_offset,
				    tmpEchoStruct.corrupted_words);
		    }
		}
	    } /* if (verifyDataFlag) */ 
	    if (goodResponse &&
		verifyDataFlag &&
		(n2h(tmpEchoStruct.sequence) != *sequenceNum)) {
		goodResponse = FALSE;
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingUpdateResponseSequenceError);
		}
		return_code =
		    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						    inRttMonCtrlAdminQItem,
						    0,
						    destOctetString,
						    D_rttMonHistoryCollectionSense_sequenceerror,
						    InitialAttempt);
		if (return_code != D_Return_Successfull) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttErrorReturnCode);
		    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
							 RttPerformEchoFName,
							 RttMonUpdateResponseSequenceErrorName);
		    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						       return_code);
		}
	    } else if (goodResponse && (tmpEchoStruct.how != '!')) {
		goodResponse = FALSE;
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingUpdateResponseDropped);
		}
		return_code =
		    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						    inRttMonCtrlAdminQItem,
						    0,
						    destOctetString,
						    D_rttMonHistoryCollectionSense_dropped,
						    InitialAttempt);
		if (return_code != D_Return_Successfull) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttErrorReturnCode);
		    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
							 RttPerformEchoFName,
							 RttMonUpdateResponseDroppedName);
		    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						       return_code);
		}
	    } /* if (goodResponse && (tmpEchoStruct.how != '!')) */ 
	    if (goodResponse) {
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingUpdateResponseOk);
		}
		return_code =
		    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						    inRttMonCtrlAdminQItem,
						    responseTime,
						    destOctetString,
						    D_rttMonHistoryCollectionSense_ok,
						    InitialAttempt);
		if (return_code != D_Return_Successfull) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttErrorReturnCode);
		    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
							 RttPerformEchoFName,
							 RttMonUpdateResponseOkName);
		    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						       return_code);
		}
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttNativeResponseCode);
		    switch (tmpEchoStruct.how) {
		    case 'Q':
			RTT_BUGINF3(IDofMySelf,
				    "\t Source quence from %i\n", tmpEchoStruct.who);
			break;
		    case 'U':
			RTT_BUGINF3(IDofMySelf,
				    "\t Unreachable from %i\n", tmpEchoStruct.who);
			break;
		    case 'M':
			RTT_BUGINF4(IDofMySelf,
				    "\t Unreachable from %i, maximum MTU %u\n",
				    tmpEchoStruct.who,
				    tmpEchoStruct.mtu);
			break;
		    default:
			RTT_BUGINF4(IDofMySelf,
				    "\t %c received from %i\n", tmpEchoStruct.how,
				    tmpEchoStruct.who);
		    }
		}
	    } /* if (goodResponse) */ 
	} /* else part of return_code from echo target */ 
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingGetNextStoredHop);
	}
	hopReturncode =
	    reg_invoke_rttMonGetNextStoredHopAddress(RTT_RETVAL_NO_OP,
						     inRttMonCtrlAdminQItem,
						     destOctetString);
    } /* while more hops to echo */ 
    return;

} /* performEcho */ 

static void ipEchoApplication (void)
{
    static const char ipEchoApplication_FName[] =
	"ipEchoApplication";
    long IDofMySelf;
    rttMonCtrlAdminQItem *inRttMonCtrlAdminQItem;
    OctetString *targetOctetString;
    OctetString *destOctetString;
    long currentWakeInterval;
    long echoTimeout;
    long echoType;
    sys_timestamp busy_timer;
    sys_timestamp total_time;
    sys_timestamp total_time_end;
    unsigned long ran_time;
    long return_code;
    long requestPayloadSize;
    unsigned short sequenceNum;
    long long_time_count;
    boolean verifyDataFlag;
    watched_boolean *pendingBoolean;

    if (!process_get_arg_ptr((void **) &inRttMonCtrlAdminQItem)) {
	RTT_BUGINF3(0,
		    "RTR 0: %s - Failed to get arg pointer\n",
		    ipEchoApplication_FName);
	process_kill(THIS_PROCESS);
    }
    reg_invoke_rttMonSetSelfID(&IDofMySelf,
			       inRttMonCtrlAdminQItem);

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttEnteringProcess);
	reg_invoke_dumpRttMonCtrlAdminQItem(inRttMonCtrlAdminQItem);
	process_sleep_for(OUTPUT_SLEEP_TIME);
    }
    currentWakeInterval =
	reg_invoke_getRttMonCtrlAdminFrequency(RTT_RETVAL_NO_OP,
					       inRttMonCtrlAdminQItem);

    requestPayloadSize =
	reg_invoke_getRttMonEchoAdminPktDataRequestSize(RTT_RETVAL_NO_OP,
							inRttMonCtrlAdminQItem);

    destOctetString =
	reg_invoke_rttMonNewAddressBuffer(RTT_RETVAL_NO_OP);
    if (destOctetString == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttIpEchoApplicationFName,
					     RttMonMallocDestOctetString);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }
    targetOctetString =
	reg_invoke_rttMonNewAddressBuffer(RTT_RETVAL_NO_OP);
    if (targetOctetString == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttIpEchoApplicationFName,
					     RttMonMallocTargetOctetString);
	reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }
    targetOctetString =
	reg_invoke_getRttMonEchoAdminTargetAddress(RTT_RETVAL_NO_OP,
						   inRttMonCtrlAdminQItem,
						   targetOctetString);
    currentWakeInterval =
	currentWakeInterval * 1000;	/* in seconds, need milliseconds */

    echoTimeout =
	reg_invoke_getRttMonCtrlAdminTimeout(RTT_RETVAL_NO_OP,
					     inRttMonCtrlAdminQItem);

    echoType =
	reg_invoke_getRttMonCtrlAdminRttType(RTT_RETVAL_NO_OP,
					     inRttMonCtrlAdminQItem);
    if ((echoType != D_rttMonCtrlAdminRttType_echo) &&
	(echoType != D_rttMonCtrlAdminRttType_pathEcho)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttBadRttType);
	reg_invoke_dumpRttMonCtrlAdminRttType(IDofMySelf,
					      echoType);
	RTT_BUGINF2(IDofMySelf,
		    "\t will use D_rttMonCtrlAdminRttType_echo\n");
	echoType = D_rttMonCtrlAdminRttType_echo;
    }
    verifyDataFlag =
	reg_invoke_rttMonGetVerifyDataFlag(RTT_RETVAL_NO_OP,
					   inRttMonCtrlAdminQItem);

    pendingBoolean =
	reg_invoke_rttMonGetPendingBoolean(RTT_RETVAL_NO_OP,
					   inRttMonCtrlAdminQItem);

    sequenceNum = 0;

    /* if initialization takes a long time */
    process_suspend();

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttCallingGetOperState);
    }
    return_code =
	reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
				      inRttMonCtrlAdminQItem);

    while (return_code != D_rttMonCtrlOperState_active) {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttWaitingToStart);
	}
	if ((return_code == D_rttMonCtrlOperState_transition_to_pending) ||
	    (return_code == D_rttMonCtrlOperState_transition_to_inactive) ||
	    (return_code == D_rttMonCtrlOperState_pending)) {
	    if ((return_code == D_rttMonCtrlOperState_transition_to_pending) ||
		(return_code == D_rttMonCtrlOperState_transition_to_inactive)) {
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingTransitionState);
		}
		return_code =
		    reg_invoke_rttMonTransitionTransitionState(RTT_RETVAL_NO_OP,
							       inRttMonCtrlAdminQItem);
		if (return_code == D_rttMonCtrlOperState_inactive) {
		    /* 
		     * kill directly, since the scheduler already 
		     *  knows you should die
		     */
		    if (RTT_TRACE(IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							   RttTransitionToInactive);
		    }
		    reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
		    reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
		    process_kill(THIS_PROCESS);
		}
	    }
	    process_watch_boolean(pendingBoolean, ENABLE,
				  ONE_SHOT);
	    process_wait_for_event();
	    process_set_boolean(pendingBoolean, FALSE);
	} else if (return_code == D_rttMonCtrlOperState_inactive) {
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttTransitionToInactive);
	    }
	    reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
	    reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
	    process_kill(THIS_PROCESS);
	}
	/*
	 *  All possible return codes have been handled.
	 */ 
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingGetOperState);
	}
	return_code =
	    reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
					  inRttMonCtrlAdminQItem);
    } /* while */
    /*
     *  Ignore the return code below, because it only tells if the
     *  current state is not active, but the above call just did it.
     */ 
    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttCallingStartLife);
    }
    reg_invoke_rttMonStartLife(RTT_RETVAL_NO_OP,
			       inRttMonCtrlAdminQItem);


    /* 
     * Lets send the first one to see if we can.
     *  Force echo type to be point to point, don't
     *   need the path.
     * More importantly, it is a requirement to create
     *   a source-target path, for errors that have no
     *   path information!
     */
    setupPathInfo(inRttMonCtrlAdminQItem,
		  D_rttMonCtrlAdminRttType_echo,
		  targetOctetString,
		  IDofMySelf);
    process_may_suspend();
    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttCallingUpdateEchoAttempts);
    }
    reg_invoke_rttMonUpdateNumberOfEchosAttempted(inRttMonCtrlAdminQItem,
						  TRUE);
    performEcho(inRttMonCtrlAdminQItem,
		destOctetString,
		&sequenceNum,
		echoTimeout,
		requestPayloadSize,
		FALSE,
		TRUE,
		IDofMySelf);

    do {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttStartingAnEchoOperation);
	}
	GET_TIMESTAMP(total_time);
	TIMER_START(busy_timer, currentWakeInterval);

	setupPathInfo(inRttMonCtrlAdminQItem,
		      echoType,
		      targetOctetString,
		      IDofMySelf);

	process_may_suspend();

	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingUpdateEchoAttempts);
	}
	reg_invoke_rttMonUpdateNumberOfEchosAttempted(inRttMonCtrlAdminQItem,
						      FALSE);
	performEcho(inRttMonCtrlAdminQItem,
		    destOctetString,
		    &sequenceNum,
		    echoTimeout,
		    requestPayloadSize,
		    verifyDataFlag,
		    FALSE,
		    IDofMySelf);

	if (TIMER_RUNNING_AND_AWAKE(busy_timer)) {
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttFrequencyOverrun);
		RTT_BUGINF2(IDofMySelf,
			    "\t The echo took longer than the frequency, so we will increment\n");
		RTT_BUGINF2(IDofMySelf,
			    "\t counters and wait for the next frequency and start again.\n");
	    }
	    /* STILL NEED to check to see if we were
	     *   busy for more than one currentWakeInterval,
	     *   and update more than once 
	     */
	    /* For number of times we exceeded currentWakeInterval 
	     *  but have a realistic value incase the time values
	     *  get way out of sync, 30 looks good.
	     */ 
	    long_time_count = 0;
	    GET_TIMESTAMP(total_time_end);
	    process_may_suspend();
	    ran_time = CLOCK_DIFF_UNSIGNED(total_time,
					   total_time_end);
	    if (RTT_TRACE(IDofMySelf)) {
		RTT_BUGINF4(IDofMySelf,
			    "\t operation took %u, but should have %u\n",
			    ran_time, currentWakeInterval);
	    }
	    while ((ran_time >= currentWakeInterval) &&
		   (long_time_count < 30)) {
		long_time_count++;
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingUpdateResponseBusy);
		}
		return_code =
		    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						    inRttMonCtrlAdminQItem,
						    0,
						    targetOctetString,
						    D_rttMonHistoryCollectionSense_busy,
						    FALSE);
		if (return_code != D_Return_Successfull) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttErrorReturnCode);
		    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
							 RttIpEchoApplicationFName,
							 RttMonUpdateResponseBusyName);
		    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						       return_code);
		}
		ran_time = ran_time - (unsigned long) currentWakeInterval;
	    } /* 
	       *  Make a call to reset periodic timer, else
	       *    the attempts we are forgoing via the busy error
	       *    will be made up by the OS by sleeping at a
	       *    shorter periodic time than requested.
	       */ 
	    process_sleep_periodic(0);
	} /* if  (TIMER_RUNNING_AND_AWAKE(busy_timer)) */
	/* CheckStateChange */ 
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingGetOperStateCheckActive);
	}
	if ((return_code =
	     reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
					   inRttMonCtrlAdminQItem)) ==
	    D_rttMonCtrlOperState_active) {
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttGoingToSleep);
		RTT_BUGINF3(IDofMySelf,
			    "\t until next frequency time (delta milliseconds %u)\n",
			    currentWakeInterval);
	    }
	    process_sleep_periodic(currentWakeInterval);
	}
	/* CheckStateChange */ 
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingGetOperStateCheckPending);
	}
	return_code =
	    reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
					  inRttMonCtrlAdminQItem);

	while ((return_code == D_rttMonCtrlOperState_transition_to_pending) ||
	       (return_code == D_rttMonCtrlOperState_pending)) {
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttTransitionToPending);
	    }
	    if (return_code == D_rttMonCtrlOperState_transition_to_pending) {
		return_code =
		    reg_invoke_rttMonTransitionTransitionState(RTT_RETVAL_NO_OP,
							       inRttMonCtrlAdminQItem);
	    }
	    if (return_code == D_rttMonCtrlOperState_pending) {
		process_watch_boolean(pendingBoolean, ENABLE,
				      ONE_SHOT);
		process_wait_for_event();
		process_set_boolean(pendingBoolean, FALSE);
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingStartLife);
		}
		reg_invoke_rttMonStartLife(RTT_RETVAL_NO_OP,
					   inRttMonCtrlAdminQItem);
	    }
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttCallingGetOperStateCheckPending);
	    }
	    return_code =
		reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
					      inRttMonCtrlAdminQItem);
	} /* while */ 
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingGetOperStateCheckDeath);
	}
	return_code =
	    reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
					  inRttMonCtrlAdminQItem);
    } while (return_code == D_rttMonCtrlOperState_active);

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttOperStateNoLongerActive);
	RTT_BUGINF2(IDofMySelf,
		    "\t thus I will need to die\n");
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttCallingGetOperStateCheckInactive);
    }
    return_code =
	reg_invoke_getRttMonOperState(RTT_RETVAL_NO_OP,
				      inRttMonCtrlAdminQItem);
    if (return_code == D_rttMonCtrlOperState_transition_to_inactive) {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttTransitionToInactive);
	}
	return_code =
	    reg_invoke_rttMonTransitionTransitionState(RTT_RETVAL_NO_OP,
						       inRttMonCtrlAdminQItem);
    }
    reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
    reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);

    process_kill(THIS_PROCESS);

} 

static char *const ProcessName[] =
{
    "IP RTR Probe 1", "IP RTR Probe 2",
    "IP RTR Probe 3", "IP RTR Probe 4",
    "IP RTR Probe 5", "IP RTR Probe 6",
    "IP RTR Probe 7", "IP RTR Probe 8",
    "IP RTR Probe 9", "IP RTR Probe 10",
    "IP RTR Probe 11", "IP RTR Probe 12",
    "IP RTR Probe 13", "IP RTR Probe 14",
    "IP RTR Probe 15", "IP RTR Probe 16",
    "IP RTR Probe 17", "IP RTR Probe 18",
    "IP RTR Probe 19", "IP RTR Probe MaxName"
    };

void rtt_ip_echo_activate (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem)
{
    int entryNum;

    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlAdminIndex < 20) {
	entryNum =
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlAdminIndex - 1;
    } else {
	entryNum = 19;
    }
    inRttMonCtrlAdminQItem->pid =
	process_create(&ipEchoApplication,
		       ProcessName[entryNum],
		       NORMAL_STACK,
		       PRIO_LOW);

    if (inRttMonCtrlAdminQItem->pid != NO_PROCESS) {
	process_set_arg_ptr(
			    inRttMonCtrlAdminQItem->pid,
			    inRttMonCtrlAdminQItem);
    } else {
	reg_invoke_dumpRttMonProcessHeader(inRttMonCtrlAdminQItem->
					   ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   RttFailedToStartIPEchoApplication);
	reg_invoke_rttMonProbePanicInternal(inRttMonCtrlAdminQItem);
    }
    return;
} 

int rtt_ip_echo_validate (rttMonCtrlAdminEntry_t * ctrlAdminEntry,
			  void *specificEntry,
			  int rttObject)
{
    rttMonEchoAdminEntry_t *echoAdminEntry;

    echoAdminEntry = (rttMonEchoAdminEntry_t *)
	specificEntry;

    switch (rttObject) {
    case D_Check_RttType:
	if ((ctrlAdminEntry->rttMonCtrlAdminRttType !=
	     D_rttMonCtrlAdminRttType_echo) &&
	    (ctrlAdminEntry->rttMonCtrlAdminRttType !=
	     D_rttMonCtrlAdminRttType_pathEcho)) {
	    return (D_INVALID_RttEntry);
	}
	break;
    case D_Check_Protocol:
	if (((ctrlAdminEntry->rttMonCtrlAdminRttType !=
	      D_rttMonCtrlAdminRttType_echo) &&
	     (ctrlAdminEntry->rttMonCtrlAdminRttType !=
	      D_rttMonCtrlAdminRttType_pathEcho)) ||
	    (echoAdminEntry->rttMonEchoAdminProtocol !=
	     D_rttMonEchoAdminProtocol_ipIcmpEcho)) {
	    return (D_INVALID_RttEntry);
	}
	break;
    case D_Check_Address:
	if (echoAdminEntry->
	    rttMonEchoAdminTargetAddress->
	    length != ADDRLEN_IP) {
	    return (D_INVALID_RttEntry);
	}
	break;
    case D_Check_Response_Size:
	/*
	 * MIN_ICMP_PKT is the size of the
	 *   required header.
	 */
	if (echoAdminEntry->
	    rttMonEchoAdminPktDataRequestSize >
	    (MAX_ICMP_PKT - (MIN_ICMP_PKT + 4))) {
	    return (D_INVALID_RttEntry);
	}
	echoAdminEntry->
	    rttMonEchoAdminPktDataResponseSize =
		echoAdminEntry->
		    rttMonEchoAdminPktDataRequestSize;
	break;
    }
    return (D_VALID_RttEntry);
} 

void register_rtt_mon_impl (void)
{
    if (reg_invoke_rtt_echo_exists(D_rttMonEchoAdminProtocol_ipIcmpEcho)) {
	reg_add_rtt_echo_validate(D_rttMonEchoAdminProtocol_ipIcmpEcho,
				  rtt_ip_echo_validate,
				  "rtt_ip_echo_validate");
	reg_add_rtt_echo_activate(D_rttMonEchoAdminProtocol_ipIcmpEcho,
				  rtt_ip_echo_activate,
				  "rtt_ip_echo_activate");
    }
    return;
}

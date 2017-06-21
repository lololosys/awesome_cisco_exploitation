/* $Id: rtt_sscpruecho.c,v 3.1.4.7 1996/08/28 12:42:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_sscpruecho.c,v $
 *------------------------------------------------------------------
 * SSCP RU Echo Probe Application.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the probe implementation that is specific
 *  to SSCP RU's for the Round Trip Time Monitor Application. 
 *------------------------------------------------------------------
 * $Log: rtt_sscpruecho.c,v $
 * Revision 3.1.4.7  1996/08/28  12:42:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.6  1996/07/29  21:47:06  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/06/07  22:24:40  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/06/05  01:29:31  lmetzger
 * CSCdi59512:  Debug output for data wraps and for
 *                     sscpRu perform echo dump wraps
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/06/04  18:27:46  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 3.1.4.2  1996/05/21  21:53:00  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 *
 * Revision 3.1.4.1  1996/05/17  10:49:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/05/14  06:18:34  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.2  1996/04/19  02:39:13  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:50:56  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:49  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../rtt/msg_rtt.c"		/* Not a typo, see logger.h */
#include "rtt_dependluecho.h"
#include "rtt_sscpruecho.h"
#include "snapiu.h"

static void dumpOpenResult (applicationHandle_t   *applicationHandle,
			    char                  *aHostName,
			    unsigned char         locAddr)
{
    rtt_dumpApplicationHandle(applicationHandle);
    RTT_BUGINF4(applicationHandle->IDofMySelf,
		"\t aHostName = %s  locAddr = %u\n", 
		aHostName,
		locAddr);
  
    return;

} /* dumpOpenResult */

static void iAmPanicking (rttMonCtrlAdminQItem  *inRttMonCtrlAdminQItem,
			  long                  IDofMySelf)
{
    reg_invoke_rttMonProbePanic(inRttMonCtrlAdminQItem);
    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
				       RttSSCPRuPanicMsg);
    process_kill(THIS_PROCESS);

    return;
}

static void openConnection (OctetString         *hopOctetString,
			    char                *aHostName,
			    applicationHandle_t *applicationHandle)
{
    long                  return_code;
    LuApiReturnCode       lu_return_code;
    boolean               isHopConnected;
    unsigned char         locAddr;

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttOpenConnectionCalled);
	RTT_BUGINF2(applicationHandle->IDofMySelf,
		    "\t Calling rttMonHopConnected\n");
    }
    isHopConnected = 
	reg_invoke_rttMonHopConnected(RTT_RETVAL_NO_OP,
				      applicationHandle->inRttMonCtrlAdminQItem,
				      hopOctetString);
    if (!isHopConnected) {  
	/*  If any length is zero, then it failed */
	if (hopOctetString->length == 0) {
	    return;
	}

	locAddr = 0;                    

	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttCallingSscpLuOpen);
	}

	applicationHandle->closeIndicationCalled = FALSE;
	/* open Connection */
	lu_return_code =
	    sscpLuOpen((void *)applicationHandle,
		       &(applicationHandle->currentLUHandle),
		       aHostName,
		       &locAddr,
		       FALSE,
		       &rtt_closeIndication);
	if (RTT_TRACE(applicationHandle->IDofMySelf)) { 
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttCallingDumpSscpLuOpenResult);
	    dumpOpenResult(applicationHandle,
			   aHostName,
			   locAddr);
	}
	if (lu_return_code != Success) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttMonOpenConnectionFName,
						 RttMonSscpOpenName);
	    rtt_dump_lu_return_code(lu_return_code,
				    applicationHandle->IDofMySelf);
	    /* don't dump it twice */
	    if (!RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
						   RttCallingDumpSscpLuOpenResult);
		dumpOpenResult(applicationHandle,
			       aHostName,
			       locAddr);
	    }
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingSetConnectionHandlePlaceholder);
	    }
	    /* create a placeholder for the HopConnectedState */
	    return_code =
		reg_invoke_rttMonSetConnectionHandle(RTT_RETVAL_NO_OP,
						     applicationHandle->inRttMonCtrlAdminQItem,
						     hopOctetString,
						     applicationHandle->currentLUHandle);
	    if (return_code != D_Return_Successfull) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						     RttMonOpenConnectionFName,
						     RttMonSetConnectionHandleName);
		reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
						   return_code);
	    } else {
		if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						       RttCallingSetConnectedStateFalse);
		}
		return_code = 
		    reg_invoke_rttMonSetHopConnectedState(RTT_RETVAL_NO_OP,
							  applicationHandle->inRttMonCtrlAdminQItem,
							  hopOctetString,
							  FALSE);
		if (return_code != D_Return_Successfull) {
		    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
						       RttErrorReturnCode);
		    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							 RttMonOpenConnectionFName,
							 RttMonSetHopConnectedStateName);
		    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
						       return_code);
		}
	    }
	} else {			/* if (lu_return_code != Success) */
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingSetConnectionHandle);
	    }
	    return_code = 
		reg_invoke_rttMonSetConnectionHandle(RTT_RETVAL_NO_OP,
						     applicationHandle->inRttMonCtrlAdminQItem,
						     hopOctetString,
						     applicationHandle->currentLUHandle);
	    if (return_code != D_Return_Successfull) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						     RttMonOpenConnectionFName,
						     RttMonSetConnectionHandleName);
		reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
						   return_code);
	    } else {
		if (applicationHandle->closeIndicationCalled) {
		    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttCallingSetConnectedStateFalse);
		    }
		    return_code = 
			reg_invoke_rttMonSetHopConnectedState(RTT_RETVAL_NO_OP,
							      applicationHandle->inRttMonCtrlAdminQItem,
							      hopOctetString,
							      FALSE);
		    if (return_code != D_Return_Successfull) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							     RttMonOpenConnectionFName,
							     RttMonSetHopConnectedStateName);
			reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
							   return_code);
		    }
		} else {
		    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttCallingSetConnectedStateTrue);
		    }
		    return_code = 
			reg_invoke_rttMonSetHopConnectedState(RTT_RETVAL_NO_OP,
							      applicationHandle->inRttMonCtrlAdminQItem,
							      hopOctetString,
							      TRUE);
		    if (return_code != D_Return_Successfull) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							     RttMonOpenConnectionFName,
							     RttMonSetHopConnectedStateName);
			reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
							   return_code);
		    }
		}
	    }
	} /* else of if (lu_return_code != Success) */

    } /* if (!isHopConnected) */

    return;

} /* openConnection */

static void receiveIndication (void            *applicationHandle,
			       byte            *bufferPtr,
			       uint            bufferLen,
			       boolean         lastFrame,
			       hword           systemSense,
			       hword           userSense)
{
    static const char receiveIndication_FName [] =
	"receiveIndication";
    LuApiReturnCode   luAPIReturnCode;
    long              newBufferLen;
    unsigned char     *newBufferPtr;

    if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->
		  IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
					    applicationHandle)->IDofMySelf, 
					   RttReceiveIndicationCalled);
	rtt_dumpApplicationHandle((applicationHandle_t *)applicationHandle);
	rtt_dumpRespBuffer((applicationHandle_t *)applicationHandle,
			   bufferPtr,
			   bufferLen);
	RTT_BUGINF5(((applicationHandle_t *)
		     applicationHandle)->IDofMySelf,
		    "\t lastFrame = %s  systemSense = %u userSense = %u\n", 
		    (lastFrame ? "TRUE": "FALSE"),
		    systemSense,
		    userSense);
    } /* if (RTT_TRACE(applicationHandle->IDofMySelf)) */

    if (systemSense || userSense) {
	((applicationHandle_t *)(applicationHandle))->errorSense = TRUE;
	process_set_boolean(((applicationHandle_t *)(applicationHandle))->receiveFinished,
			    TRUE);
	if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->
		      IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf,
					       RttSettingReceiveFinishTrue);
	    RTT_BUGINF3(((applicationHandle_t *)applicationHandle)->IDofMySelf, 
			"\t in %s, because bad Sense Code\n", receiveIndication_FName);
	}
    } else if (lastFrame) {
	((applicationHandle_t *)applicationHandle)->receivedBufferLen =+
	    bufferLen;
	process_set_boolean(
			    ((applicationHandle_t *)(applicationHandle))->receiveFinished,
			    TRUE);
	if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->
		      IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf,
					       RttSettingReceiveFinishTrue);
	    RTT_BUGINF3(((applicationHandle_t *)applicationHandle)->IDofMySelf, 
			"\t in %s\n", receiveIndication_FName);
	}
    } else {				/* if (lastFrame) */
	/* Check of a BAD BAD error */
	
	if (((applicationHandle_t *)applicationHandle)->
	    receivedBufferLen + bufferLen >=
	    ((applicationHandle_t *)applicationHandle)->
	    maxRespBufferLen) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf, 
					       RttBufferReceiveOverflow);
	    RTT_BUGINF4(((applicationHandle_t *)applicationHandle)->IDofMySelf,
			"\t in %s with size: %u\n", 
			receiveIndication_FName,
			((applicationHandle_t *)applicationHandle)->
			receivedBufferLen);
	    /* Set it back to the Last unused Buffer Space */
	    newBufferLen = ((applicationHandle_t *)applicationHandle)->
		maxRespBufferLen -
		    ((applicationHandle_t *)applicationHandle)->
			receivedBufferLen;
	    newBufferPtr = bufferPtr;
	} else {
	    ((applicationHandle_t *)applicationHandle)->receivedBufferLen =+
		bufferLen;
	    newBufferLen = ((applicationHandle_t *)applicationHandle)->
		maxRespBufferLen -
		    ((applicationHandle_t *)applicationHandle)->
			receivedBufferLen;
	    newBufferPtr = &bufferPtr[bufferLen];
	}
	if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->
		      IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf,
					       RttCallingSscpLuReceiveContinue);
	    RTT_BUGINF4(((applicationHandle_t *)applicationHandle)->IDofMySelf,
			"\t in %s with handle: %u\n", 
			receiveIndication_FName,
			((applicationHandle_t *)applicationHandle)->currentLUHandle);
	}
	luAPIReturnCode  = 
	    sscpLuReceiveContinue(((applicationHandle_t *)
				   applicationHandle)->currentLUHandle,
				  newBufferLen,
				  newBufferPtr);
	if (luAPIReturnCode != Success) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf, 
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(((applicationHandle_t *)
						  applicationHandle)->IDofMySelf,
						 RttMonReceiveIndicationFName,
						 RttMonSscpReceiveContinue);
	    rtt_dump_lu_return_code(luAPIReturnCode,
				    ((applicationHandle_t *)
				     applicationHandle)->IDofMySelf);
	}
  
    } /* else of if (lastFrame) */

    return;

} /* receiveIndication */

/* 
 * NOTE: this return code has 4 possible 
 *        values: 
 *        D_echoTarget_timeout = 1, 
 *        D_echoTarget_connectionLost = 2,
 *        D_echoTarget_internalError = 3,
 *        D_echoTarget_ayyOK = 4 
 */
static long echoTarget (unsigned char            *dataRespBuffer,
			unsigned char            *dataSendBuffer,
			long                     sendBufferSize,
			long                     echoTimeout,
			applicationHandle_t      *applicationHandle)
{
    static const char echoTarget_FName [] = "echoTarget";
    LuApiReturnCode   luAPIReturnCode;
    boolean           blockState;
   
    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttEchoTargetCalled);
	rtt_dumpApplicationHandle(applicationHandle);
	RTT_BUGINF3(applicationHandle->IDofMySelf,
		    "\t echoTimeout (milliseconds) = %u\n", echoTimeout);
	rtt_dumpSendBuffer(dataSendBuffer,
			   (sendBufferSize + 
			    sizeof(SNA_SSCP_FRAME) - 2),
			   applicationHandle->IDofMySelf);
	RTT_BUGINF2(applicationHandle->IDofMySelf,
		    "\t calling sscpLuReceive...\n");
    }
    applicationHandle->receivedBufferLen = 0;
    applicationHandle->errorSense = FALSE;
    /* sscpLuReceive */
    luAPIReturnCode = 
	sscpLuReceive(applicationHandle->currentLUHandle,
		      applicationHandle->maxRespBufferLen,
		      dataRespBuffer,
		      &receiveIndication);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttMonEchoTargetFName,
					     RttMonSscpLuReceive);
	rtt_dump_lu_return_code(luAPIReturnCode,
				applicationHandle->IDofMySelf);
	if ((luAPIReturnCode == HostConnInactive) ||
	    (luAPIReturnCode == InvalidHandle)) {
	    return (D_echoTarget_connectionLost);
	} else {
	    return (D_echoTarget_internalError);
	}
    } /* if (luAPIReturnCode != Success) */

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttCallingSscpLuSend);
    }
    /* sscpLuSend */
    luAPIReturnCode = 
	sscpLuSend(applicationHandle->currentLUHandle,
		   (sendBufferSize + sizeof(SNA_SSCP_FRAME) - 2),
		   dataSendBuffer);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttMonEchoTargetFName,
					     RttMonSscpLuSend);
	rtt_dump_lu_return_code(luAPIReturnCode,
				applicationHandle->IDofMySelf);
	if (luAPIReturnCode == HostConnInactive) {
	    return (D_echoTarget_connectionLost);
	} else {
	    return (D_echoTarget_internalError);
	}
    } /* if (luAPIReturnCode != Success) */

    blockState = process_get_boolean(applicationHandle->receiveFinished);
    if (blockState == FALSE) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttReceiveFinishedIsFalse);
	    RTT_BUGINF4(applicationHandle->IDofMySelf,
			"\t in %s calling\n\t process_wait_for_event_timed( %u milliseconds )\n",
			echoTarget_FName, echoTimeout);
	}
	process_wait_for_event_timed((unsigned long)echoTimeout);
    }

    blockState = 
	process_get_boolean(applicationHandle->receiveFinished);
    if (blockState == FALSE) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttWokeOnTimeout);
	}
	return (D_echoTarget_timeout);
    } else {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttWokeOnRealEvent);
	}
	if (applicationHandle->closeIndicationCalled) {
	    applicationHandle->closeIndicationCalled = FALSE;
	    return (D_echoTarget_connectionLost);
	} else if (applicationHandle->errorSense == TRUE) {
	    return (D_echoTarget_internalError);
	} else {
	    return (D_echoTarget_ayyOK);
	}
    }

} /* echoTarget */

#define TH_RH_LEN 9

static boolean CheckSscpResponse (SNA_SSCP_FRAME *snaP,
				  long           *length,
				  applicationHandle_t      *applicationHandle)
{
    /* make sure length is enough */
    if (*length < TH_RH_LEN+1) {
        if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t CheckSscpResponse:  short SNA packet length\n");
	}
	return FALSE;
    }

    /* check the Rsp Type Ind. */
    if ((snaP->TH1 & RH1B3_RTI) == RH1B3_N) {
        if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t CheckSscpResponse:  Negative Response Bit Set\n");
	}
	return FALSE;
    }

    return TRUE;
}

#define swab(a,b) { byte c = b; b = a; a = c; }

static boolean ChangeSscpLuReqToPosRsp (SNA_SSCP_FRAME *snaP,
					long           *length,
					applicationHandle_t      *applicationHandle)
{

    /* make sure length is enough */
    if (*length < TH_RH_LEN+1) {
        if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t ChangeSscpLuReqToPosRsp:  short SNA packet length\n");
	}
	return FALSE;
    }
   
    /* Make sure it is a first or only segment. */
    if ((snaP->TH0 & TH0_BBIU) == 0) {
        if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t ChangeSscpLuReqToPosRsp:  No first Segment \n");
	}
	return FALSE;
    }
   
    /* Determine if the MU is already a response, in which case this is 
     * an error, and FALSE is returned.                                 
     */
    if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP) {
        if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t ChangeSscpLuReqToPosRsp:  MU is already a response \n");
	}
	return FALSE;
    }

    /* Make sure that the request is for definite response. */
    if ( ((snaP->RH1 & RH1B3_ERI) == RH1B3_ER) ||
	 (((snaP->RH1 & (RH1B0_DR1I|RH1B2_DR2I)) == 0)) ) {
        if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t ChangeSscpLuReqToPosRsp:  MU is not a definite respone packet \n");
	}
	return FALSE;
    }
   
    if ( ((snaP->RH0 & RH0B1B2_Category) == RH0B1B2_FMD) &&
        ((snaP->RH0 & RH0B4_FI) == RH0B4_NSH) &&
	( (snaP->RU0 == 0x01) ||
	 (snaP->RU0 == 0x41) ||
	 (snaP->RU0 == 0x81) ) ) {
	/* Response Category is FMD */
        if (*length < TH_RH_LEN+3) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	        RTT_BUGINF2(applicationHandle->IDofMySelf,
			    "\t ChangeSscpLuReqToPosRsp:  Response Category FMD \n");
	    }
	    return FALSE;
	}
	*length = TH_RH_LEN + 3;	/* 3 bytes for the request code */
    } else {
	*length = TH_RH_LEN + 1;	/* 1 byte for the request code
					 */
    }
   
    /* Set the RH and TH */
    snaP->RH0 = (snaP->RH0 | ( RH0B0_RSP /* Response                */
			      + RH0B6_BC /* First in chain          */
			      + RH0B7_EC /* Last in chain           */
			      )) & ~RH0B5_SD; /* Sense Data Not Included */

    snaP->RH1 = snaP->RH1 | RH1B3_P;	/* Positive Response       */

    snaP->RH1 &= ~RH1B3_ER; 

    snaP->TH0 = snaP->TH0 | ( TH0_BBIU	/* Whole BIU               */
			     + TH0_EBIU
			     );

    snaP->RH2 = 0;                                 
 
    /* Interchange OAF and DAF */  
    swab(snaP->OAF,snaP->DAF);

    return TRUE;
}                       


static void AckBackToHost (unsigned char            *dataRespBuffer,
			   unsigned char            *dataSendBuffer,
			   long                     sendBufferSize,
			   long                     echoTimeout,
			   applicationHandle_t      *applicationHandle)
{
    long     newBufferSize = sendBufferSize;
    LuApiReturnCode   luAPIReturnCode;

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttAckBackToHostCalled);
    }

    /* If this returns false then we don't need to continue */
    if (!ChangeSscpLuReqToPosRsp((SNA_SSCP_FRAME*)dataRespBuffer,
				 &newBufferSize,
				 applicationHandle)) {
	return;
    }

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttSendingFirstAck);
    }
    memcpy(dataSendBuffer, dataRespBuffer, newBufferSize);

    /* sscpLuSend */
    luAPIReturnCode = 
	sscpLuSend(applicationHandle->currentLUHandle,
		   newBufferSize,
		   dataSendBuffer);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttMonAckBackToHostFName,
					     RttMonSscpLuSend);
	rtt_dump_lu_return_code(luAPIReturnCode,
				applicationHandle->IDofMySelf);
    } /* if (luAPIReturnCode != Success) */
    
    return;

}


static void setupPathInfo (rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem,
			   char                    *aHostName,
			   OctetString             *targetOctetString,
			   long                    IDofMySelf)
{
    long                    return_code;

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, RttSetupPathInfoCalled);
	RTT_BUGINF2(IDofMySelf,
		    "\t Calling rttMonStartUpdatePath\n");
    }
    reg_invoke_rttMonStartUpdatePath(inRttMonCtrlAdminQItem);
    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, RttCallingUpdatePath);
	RTT_BUGINF3(IDofMySelf, "\t for Target %s\n", 
		    aHostName);
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
    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, RttCallingEndUpdatePath);
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

static void setupSNAFrame (SNA_SSCP_FRAME *SNA_ptr)
{
    SNA_ptr->TH0     =         0x2C;
    SNA_ptr->TH1     =         0x00;
    SNA_ptr->DAF     =         0x00;
    SNA_ptr->OAF     =         0x00;
    SNA_ptr->SNF     =         0x0000;
    SNA_ptr->RH0     =         0x0B;
    SNA_ptr->RH1     =         0x90;   /* Exception Response */
    SNA_ptr->RH2     =         0x00;
    SNA_ptr->RU0     =         0x81;   /* ReqEcho X'810387 */
    SNA_ptr->RU1     =         0x03;
    SNA_ptr->RU2     =         0x87;
    SNA_ptr->RU3     =         0x01;   /* repeat the following N times */

    return;

} /* setupSNAFrame */

static void performEcho (OctetString             *destOctetString,
			 char                    *aHostName,
			 unsigned short          *sequenceNum,
			 unsigned char           *dataRespBuffer,
			 unsigned char           *dataSendBuffer,
			 applicationHandle_t     *applicationHandle,
			 long                    echoTimeout,
			 boolean                 verifyDataFlag)
{
    unsigned long           responseTime;
    long                    sendBufferSize;
    long                    return_code;
    long                    echo_return_code;
    LuApiReturnCode         lu_return_code;
    unsigned char           *returnedBuffer;
    boolean                 goodResponse;
    SNA_SSCP_FRAME          *SNA_ptr;
    boolean                 positiveResponse = FALSE;
    long                    newBufferSize;
    char                    *tmpData;

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					   RttPerformEchoCalled);
	rtt_dumpApplicationHandle(applicationHandle);
	RTT_BUGINF4(applicationHandle->IDofMySelf,
		    "\t echoTimeout (milliseconds) = %u sequenceNum = %u\n", 
		    echoTimeout,
		    *sequenceNum);
	RTT_BUGINF4(applicationHandle->IDofMySelf,
		    "\t destination address is %s verifyDataFlag = %s\n",
		    aHostName,
		    verifyDataFlag ? "True" : "False");
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttCallingGetFirstStoredHop);
    }
    return_code = 
	reg_invoke_rttMonGetFirstStoredHopAddress(RTT_RETVAL_NO_OP,
						  applicationHandle->inRttMonCtrlAdminQItem,
						  destOctetString);
    if (return_code != D_Return_Successfull) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttPerformEchoFName,
					     RttMonGetFirstStoredHopAddressName);
	reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					   return_code);
	/* updateResponse with internal error */
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingUpdateResponseDropped);
	}
	return_code = 
	    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
					    applicationHandle->inRttMonCtrlAdminQItem,
					    0,
					    destOctetString,
					    D_rttMonHistoryCollectionSense_dropped,
					    FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonUpdateResponseDroppedName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
	return;
    }

    SNA_ptr = (SNA_SSCP_FRAME *)dataSendBuffer;
    setupSNAFrame(SNA_ptr);

    (*sequenceNum)++;
    return_code = 
	reg_invoke_rttMonSetupEBCDICDataPortion(RTT_RETVAL_NO_OP,
						applicationHandle->inRttMonCtrlAdminQItem,
						(unsigned char *)SNA_ptr->RUData,
						0,
						&sendBufferSize,
						*sequenceNum,
						D_rttMonDataPortionCommand_echoRequest);
    if (return_code != D_Return_Successfull) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttPerformEchoFName,
					     RttMonSetupEBCDICDataName);
	reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					   return_code);

	/* Timestamp this error */
	reg_invoke_rttMonTimeStampEchoStart(applicationHandle->inRttMonCtrlAdminQItem);
	/* updateResponse with internal error */
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingUpdateResponseDropped);
	}
	return_code = 
	    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
					    applicationHandle->inRttMonCtrlAdminQItem,
					    0,
					    destOctetString,
					    D_rttMonHistoryCollectionSense_dropped,
					    FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonUpdateResponseDroppedName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}

	return;
    } /* if (return_code != D_Return_Successfull) */

    SNA_ptr->RUDataSize = (unsigned char)sendBufferSize;

    return_code = 
	reg_invoke_rttMonGetConnectionHandle(RTT_RETVAL_NO_OP,
					     applicationHandle->inRttMonCtrlAdminQItem,
					     destOctetString,
					     &(applicationHandle->currentLUHandle));
    if (return_code != D_Return_Successfull) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttPerformEchoFName,
					     RttMonGetConnectionHandleName);
	reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					   return_code);

	/* Timestamp this error */
	reg_invoke_rttMonTimeStampEchoStart(applicationHandle->inRttMonCtrlAdminQItem);

	/* updateResponse with internal error */
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingUpdateResponseDropped);
	}
	return_code = 
	    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
					    applicationHandle->inRttMonCtrlAdminQItem,
					    0,
					    destOctetString,
					    D_rttMonHistoryCollectionSense_dropped,
					    FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonUpdateResponseDroppedName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}

	return;
    } /* if (return_code != D_Return_Successfull) */

    responseTime = 0;
    /*
     *  The following is called right before the
     *    echo attempt.  It was also called above
     *    when an error occured on the echo setup,
     *    which gave the echo attempt a timestamp
     *    for the error.  We want to call it here
     *    so we get the closest real value of the
     *    delay.
     */
    reg_invoke_rttMonTimeStampEchoStart(applicationHandle->inRttMonCtrlAdminQItem);
    /* NOTE: this return code hase 4 possible 
       values: D_echoTarget_timeout = 1, 
       D_echoTarget_connectionLost = 2,
       D_echoTarget_internalError = 3
       D_echoTarget_ayyOK = 4 */
    echo_return_code = 
	echoTarget(dataRespBuffer,
		   dataSendBuffer,
		   sendBufferSize,
		   echoTimeout,
		   applicationHandle);
    /* calculate response_time */
    responseTime =
	reg_invoke_rttMonCalculateResponseTime(RTT_RETVAL_NO_OP,
					       applicationHandle->inRttMonCtrlAdminQItem);
    newBufferSize = applicationHandle->receivedBufferLen;
    /* handle echo return_code */
    if ((echo_return_code == D_echoTarget_timeout) ||
	 (responseTime > echoTimeout)) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttReturnedFromEchoTarget);     
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t with D_echoTarget_timeout return_code\n");
	    RTT_BUGINF3(applicationHandle->IDofMySelf,
			"\t and responseTime (milliseconds) = %u\n", 
			responseTime);
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t Will close connection and start over\n");
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingUpdateResponseTimeout);
	}
	return_code = 
	    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
					    applicationHandle->inRttMonCtrlAdminQItem,
					    0,
					    destOctetString,
					    D_rttMonHistoryCollectionSense_timeout,
					    FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonUpdateResponseTimeoutName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingSscpLuClose);
	}
	lu_return_code = 
	    sscpLuClose(applicationHandle->
			currentLUHandle, 
			&returnedBuffer);
	if (lu_return_code != Success) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonSscpLuClose);
	    rtt_dump_lu_return_code(lu_return_code,
				    applicationHandle->IDofMySelf);
	}
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingSetConnectedStateFalse);
	}
	return_code = 
	    reg_invoke_rttMonSetHopConnectedState(RTT_RETVAL_NO_OP,
						  applicationHandle->inRttMonCtrlAdminQItem,
						  destOctetString,
						  FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonSetHopConnectedStateName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
    } else if (echo_return_code == D_echoTarget_connectionLost) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttReturnedFromEchoTarget);     
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t with D_echoTarget_connectionLost return_code\n");
	    RTT_BUGINF3(applicationHandle->IDofMySelf,
			"\t and responseTime (milliseconds) = %u\n", 
			responseTime);
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingUpdateResponseDisconnected);
	}
	return_code = 
	    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
					    applicationHandle->inRttMonCtrlAdminQItem,
					    0,
					    destOctetString,
					    D_rttMonHistoryCollectionSense_disconnected,
					    FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonUpdateResponseDisconnectedName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingSetConnectedStateFalse);
	}
	return_code = 
	    reg_invoke_rttMonSetHopConnectedState(RTT_RETVAL_NO_OP,
						  applicationHandle->inRttMonCtrlAdminQItem,
						  destOctetString,
						  FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonSetHopConnectedStateName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
    }
    else  if ((echo_return_code == D_echoTarget_internalError) ||
	      (!(positiveResponse = CheckSscpResponse((SNA_SSCP_FRAME*)dataRespBuffer,
						      &newBufferSize,
						      applicationHandle)))) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttReturnedFromEchoTarget);     
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t with D_echoTarget_internalError return_code\n");
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t or with the Echo Test Not Granted by the SSCP\n");
	    RTT_BUGINF3(applicationHandle->IDofMySelf,
			"\t and responseTime (milliseconds) = %u\n", 
			responseTime);
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingUpdateResponseDropped);
	}
	return_code = 
	    reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
					    applicationHandle->inRttMonCtrlAdminQItem,
					    0,
					    destOctetString,
					    D_rttMonHistoryCollectionSense_dropped,
					    FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonUpdateResponseDroppedName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingSscpLuClose);
	}
	lu_return_code = 
	    sscpLuClose(applicationHandle->
			currentLUHandle, 
			&returnedBuffer);
	if (lu_return_code != Success) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonSscpLuClose);
	    rtt_dump_lu_return_code(lu_return_code,
				    applicationHandle->IDofMySelf);
	}
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingSetConnectedStateFalse);
	}
	return_code = 
	    reg_invoke_rttMonSetHopConnectedState(RTT_RETVAL_NO_OP,
						  applicationHandle->inRttMonCtrlAdminQItem,
						  destOctetString,
						  FALSE);
	if (return_code != D_Return_Successfull) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttPerformEchoFName,
						 RttMonSetHopConnectedStateName);
	    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					       return_code);
	}
    } else {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttReturnedFromEchoTarget);     
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t with D_echoTarget_ayyOK return_code\n");
	    RTT_BUGINF3(applicationHandle->IDofMySelf,
			"\t and responseTime (milliseconds) = %u\n", 
			responseTime);
	}
	goodResponse = TRUE;
	/* if verify data, verify data */
	if (verifyDataFlag) {
	    SNA_ptr = (SNA_SSCP_FRAME *)dataRespBuffer;
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingVerifyEBCDICPortion);
	    }
	    /* 
	     *  Data response does not have the # of repeats included,
	     *    thus the response starts one byte earlier that
	     *    request.  So we will verify the data portion at one
	     *    byte short, RUDataSize and not RUData where it was
	     *    originally placed.
	     */
	    /* 
	     * Must fix up response so it looks like an ARR reponse
	     */
	    tmpData = (unsigned char *)&(SNA_ptr->RUDataSize);
	    tmpData[5] = RTT_ARR_ECHO_RESPONSE;
	    return_code = 
		reg_invoke_rttMonVerifyEBCDICDataPortion(RTT_RETVAL_NO_OP,
							 applicationHandle->inRttMonCtrlAdminQItem,
							 *sequenceNum,
							 tmpData);
	    if (return_code != D_Return_Successfull) {
		goodResponse = FALSE;
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						     RttPerformEchoFName,
						     RttMonVerifyEBCDICDataName);
		reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
						   return_code);
		if (return_code == D_Bad_Sequence_Number) {
		    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttCallingUpdateResponseSequenceError);
		    }
		    return_code = 
			reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
							applicationHandle->inRttMonCtrlAdminQItem,
							0,
							destOctetString,
							D_rttMonHistoryCollectionSense_sequenceerror,
							FALSE);
		    if (return_code != D_Return_Successfull) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							     RttPerformEchoFName,
							     RttMonUpdateResponseSequenceErrorName);
			reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
							   return_code);
		    }
		} else if (return_code == D_Bad_Data_Portion) {
		    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttCallingUpdateResponseVerifyError);
		    }
		    return_code = 
			reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
							applicationHandle->inRttMonCtrlAdminQItem,
							0,
							destOctetString,
							D_rttMonHistoryCollectionSense_verifyerror,
							FALSE);
		    if (return_code != D_Return_Successfull) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							     RttPerformEchoFName,
							     RttMonUpdateResponseVerifyErrorName);
			reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
							   return_code);
		    }
		} else {
		    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttCallingUpdateResponseDropped);
		    }
		    return_code = 
			reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
							applicationHandle->inRttMonCtrlAdminQItem,
							0,
							destOctetString,
							D_rttMonHistoryCollectionSense_dropped,
							FALSE);
		    if (return_code != D_Return_Successfull) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							     RttPerformEchoFName,
							     RttMonUpdateResponseDroppedName);
			reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
							   return_code);
		    }
		}
	    } /* if (return_code != D_Return_Successfull) */
	} /* if (verifyDataFlag) */
	if (goodResponse) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingUpdateResponseOk);
	    }
	    return_code = 
		reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						applicationHandle->inRttMonCtrlAdminQItem,
						responseTime,
						destOctetString,
						D_rttMonHistoryCollectionSense_ok,
						FALSE);
	    if (return_code != D_Return_Successfull) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						     RttPerformEchoFName,
						     RttMonUpdateResponseOkName);
		reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
						   return_code);
	    }
	} /* if (goodResponse) */
	/* Ack back to the host */
	if (positiveResponse) {
	    AckBackToHost(dataRespBuffer,
			  dataSendBuffer,
			  newBufferSize,
			  echoTimeout,
			  applicationHandle);
	}
	else if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttEchoTestNotGrantedBySscp);
	}
    } /* else part of return_code from echo target */
    return;
  
} /* performEcho */

static void sscpRuEchoApplication (void)
{
    static const char       sscpRuEchoApplication_FName [] =
	"sscpRuEchoApplication";
    long                    IDofMySelf;
    rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem;
    OctetString             *targetOctetString;
    OctetString             *destOctetString;
    long                    currentWakeInterval;
    long                    echoTimeout;
    long                    echoType;
    sys_timestamp           busy_timer;
    sys_timestamp           total_time;
    sys_timestamp           total_time_end;
    unsigned long           ran_time;
    long                    return_code;
    LuApiReturnCode         lu_return_code;
    boolean                 connectionState;
    unsigned char           *dataRespBuffer;
    unsigned char           *dataSendBuffer;
    unsigned char           *returnedBuffer;
    applicationHandle_t     applicationHandle;
    long                    requestDataSize;
    unsigned short          sequenceNum;
    boolean                 verifyDataFlag;
    char                    *watchedBooleanName;
    long                    long_time_count;
    watched_boolean         *pendingBoolean;
    char                    *aHostName;

    if (!process_get_arg_ptr((void **)&inRttMonCtrlAdminQItem)) {
	RTT_BUGINF3(0,
		    "RTR 0: %s - Failed to get arg pointer\n",
		    sscpRuEchoApplication_FName);
	process_kill(THIS_PROCESS);
    }

    reg_invoke_rttMonSetSelfID(&IDofMySelf,
			       inRttMonCtrlAdminQItem);

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, RttEnteringProcess);
	reg_invoke_dumpRttMonCtrlAdminQItem(inRttMonCtrlAdminQItem);
	process_sleep_for(OUTPUT_SLEEP_TIME); 
    }

    applicationHandle.IDofMySelf = IDofMySelf;
    applicationHandle.inRttMonCtrlAdminQItem =
	inRttMonCtrlAdminQItem;

    if ((watchedBooleanName = malloc(strlen("sscpRuEchoProbe") + 5)) 
	== NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSscpRuEchoApplicationFName,
					     RttMonMallocWatchedBooleanName);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

    watchedBooleanName[0] = 0x00;
    sprintf(watchedBooleanName, "sscpRuEchoProbe %u",
	    inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminIndex);
    if ((applicationHandle.receiveFinished = 
	 (watched_boolean *)create_watched_boolean(watchedBooleanName,
						   0)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSscpRuEchoApplicationFName,
					     RttMonCreateWatchedBooleanName);
	free(watchedBooleanName);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

    currentWakeInterval = 
	reg_invoke_getRttMonCtrlAdminFrequency(RTT_RETVAL_NO_OP,
					       inRttMonCtrlAdminQItem);

    requestDataSize = 
	reg_invoke_getRttMonEchoAdminPktDataRequestSize(RTT_RETVAL_NO_OP,
							inRttMonCtrlAdminQItem);

    applicationHandle.errorSense = FALSE;
    applicationHandle.closeIndicationCalled = FALSE;
    applicationHandle.receivedBufferLen = 0;
    applicationHandle.maxRespBufferLen = requestDataSize + 
	SSCP_HEADER_SIZE + ARR_HEADER_SIZE + 40;
    /* added 20 bytes for bad buffer overrun */
    if ((dataRespBuffer = malloc(applicationHandle.
				 maxRespBufferLen + 20)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSscpRuEchoApplicationFName,
					     RttMonMallocDataRespBuffer);
	delete_watched_boolean(&(applicationHandle.receiveFinished));
	free(watchedBooleanName);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

    if ((dataSendBuffer = malloc(requestDataSize + SSCP_HEADER_SIZE +
				 ARR_HEADER_SIZE + 40)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSscpRuEchoApplicationFName,
					     RttMonMallocDataSendBuffer);
	delete_watched_boolean(&(applicationHandle.receiveFinished));
	free(watchedBooleanName);
	free(dataRespBuffer);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

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
					     RttSscpRuEchoApplicationFName,
					     RttMonMallocTargetOctetString);
	reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
	delete_watched_boolean(&(applicationHandle.receiveFinished));
	free(watchedBooleanName);
	free(dataRespBuffer);
	free(dataSendBuffer);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

    targetOctetString = 
	reg_invoke_getRttMonEchoAdminTargetAddress(RTT_RETVAL_NO_OP,
						   inRttMonCtrlAdminQItem,
						   targetOctetString);

    if ((aHostName = malloc(targetOctetString->length + 1)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSscpRuEchoApplicationFName,
					     RttMonMallocaHostName);
	reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
	delete_watched_boolean(&(applicationHandle.receiveFinished));
	free(watchedBooleanName);
	free(dataRespBuffer);
	free(dataSendBuffer);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }
  
    memcpy(aHostName, targetOctetString->octet_ptr, 
	   targetOctetString->length);
    aHostName[targetOctetString->length] = 0x00;

    currentWakeInterval = 
	currentWakeInterval * 1000;	/* in seconds, need milliseconds */
    echoTimeout = 
	reg_invoke_getRttMonCtrlAdminTimeout(RTT_RETVAL_NO_OP,
					     inRttMonCtrlAdminQItem);

    echoType = 
	reg_invoke_getRttMonCtrlAdminRttType(RTT_RETVAL_NO_OP,
					     inRttMonCtrlAdminQItem);
    if (echoType != D_rttMonCtrlAdminRttType_echo) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttBadRttType);
	reg_invoke_dumpRttMonCtrlAdminRttType(IDofMySelf,
					      echoType);
	RTT_BUGINF2(IDofMySelf,
		    "\t will use D_rttMonCtrlAdminRttType_echo\n");
    }

    verifyDataFlag = 
	reg_invoke_rttMonGetVerifyDataFlag(RTT_RETVAL_NO_OP,
					   inRttMonCtrlAdminQItem);

    pendingBoolean = 
	reg_invoke_rttMonGetPendingBoolean(RTT_RETVAL_NO_OP,
					   inRttMonCtrlAdminQItem);

    process_watch_boolean(applicationHandle.receiveFinished, ENABLE,
			  RECURRING);
           
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

    while (return_code != D_rttMonCtrlOperState_active)	{
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttWaitingToStart);
	}

	if ((return_code == D_rttMonCtrlOperState_transition_to_pending) ||
	    (return_code == D_rttMonCtrlOperState_transition_to_inactive) ||
	    (return_code == D_rttMonCtrlOperState_pending)) {
	    if ((return_code == D_rttMonCtrlOperState_transition_to_pending) ||
		(return_code ==
		 D_rttMonCtrlOperState_transition_to_inactive)) {
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttCallingTransitionState);
		}
		return_code = 
		    reg_invoke_rttMonTransitionTransitionState(RTT_RETVAL_NO_OP,
							       inRttMonCtrlAdminQItem);
		if (return_code == D_rttMonCtrlOperState_inactive) {
		    /* kill directly, since the scheduler already 
		     *  knows you should die
		     */
		    if (RTT_TRACE(IDofMySelf)) {
			reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
							   RttTransitionToInactive);
		    }
		    free(dataRespBuffer);
		    free(dataSendBuffer);
		    reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
		    reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
		    delete_watched_boolean(&(applicationHandle.receiveFinished));
		    free(watchedBooleanName);
		    free(aHostName);
		    process_kill(THIS_PROCESS);
		}
	    }

	    process_watch_boolean(pendingBoolean, ENABLE,
				  ONE_SHOT);
	    process_wait_for_event();
	    process_set_boolean(pendingBoolean, FALSE);
	}
	else if (return_code == D_rttMonCtrlOperState_inactive)
	    {
		if (RTT_TRACE(IDofMySelf)) {
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttTransitionToInactive);
		}
		free(dataRespBuffer);
		free(dataSendBuffer);
		reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
		reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
		delete_watched_boolean(&(applicationHandle.receiveFinished));
		free(watchedBooleanName);
		free(aHostName);
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


    do {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttStartingAnEchoOperation);
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttSettingReceiveFinishedFalse);
	    RTT_BUGINF3(IDofMySelf,
			"\t in %s\n", sscpRuEchoApplication_FName);
	}
	process_set_boolean(applicationHandle.receiveFinished, FALSE);
	GET_TIMESTAMP(total_time);
	TIMER_START(busy_timer, currentWakeInterval);
	openConnection(targetOctetString,
		       aHostName,
		       &applicationHandle);

	process_may_suspend();

	setupPathInfo(inRttMonCtrlAdminQItem,
		      aHostName,
		      targetOctetString,
		      IDofMySelf);

	process_may_suspend();

	connectionState = 
	    reg_invoke_rttMonHopConnected(RTT_RETVAL_NO_OP,
					  inRttMonCtrlAdminQItem,
					  targetOctetString);
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingUpdateEchoAttempts);
	}
	reg_invoke_rttMonUpdateNumberOfEchosAttempted(applicationHandle.
						      inRttMonCtrlAdminQItem,
						      FALSE);
	if (connectionState == TRUE) {
	    performEcho(destOctetString,
			aHostName,
			&sequenceNum,
			dataRespBuffer,
			dataSendBuffer,
			&applicationHandle,
			echoTimeout,
			verifyDataFlag);
	} else {			/* if (connectionState == TRUE) */
	    /* Timestamp this error */
	    reg_invoke_rttMonTimeStampEchoStart(applicationHandle.
						inRttMonCtrlAdminQItem);
	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttCallingUpdateResponseNotConnected);
	    }
	    return_code = 
		reg_invoke_rttMonUpdateResponse(RTT_RETVAL_NO_OP,
						inRttMonCtrlAdminQItem,
						0,
						targetOctetString,
						D_rttMonHistoryCollectionSense_notconnected,
						FALSE);
	    if (return_code != D_Return_Successfull) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
						     RttSscpRuEchoApplicationFName,
						     RttMonUpdateResponseNotConnected);
		reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						   return_code);
	    }
	} /* else of if (connectionState == TRUE) */

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

	    setupPathInfo(inRttMonCtrlAdminQItem,
			  aHostName,
			  targetOctetString,
			  IDofMySelf);
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
							 RttSscpRuEchoApplicationFName,
							 RttMonUpdateResponseBusyName);
		    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,
						       return_code);
		}
		ran_time = ran_time - currentWakeInterval;
	    }
	    /* 
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
      
	    if (return_code ==
		D_rttMonCtrlOperState_transition_to_pending) {
		return_code = 
		    reg_invoke_rttMonTransitionTransitionState(RTT_RETVAL_NO_OP,
							       inRttMonCtrlAdminQItem);
	    }
      
	    if (return_code == D_rttMonCtrlOperState_pending) {
		process_watch_boolean(pendingBoolean, ENABLE,
				      ONE_SHOT);
	        /* 
		 * If the data has been received from the last request
		 *   then the receiveFinished watched boolean will be
		 *   set to true, and the process_wait_for_event will
		 *   thrash on the receiveFinished.  If receiveFinished
		 *   actually occurs while we are waiting and the
		 *   pendingBoolean is not TRUE, then go back to
		 *   sleep and wait for the pendingBoolean.
		 */
	        do {
	            process_set_boolean(applicationHandle.receiveFinished,
					FALSE);      

		    process_wait_for_event();
		} while (!process_get_boolean(pendingBoolean));
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
    }
  
    /* get the one connection handle and close and free it */
    connectionState = 
	reg_invoke_rttMonHopConnected(RTT_RETVAL_NO_OP,
				      inRttMonCtrlAdminQItem,
				      targetOctetString);
    if (connectionState == TRUE) { 
	/*
	 * The hop was found connected above, so the only
	 *  error that can be returned below is 
	 *  D_Hop_Not_Found, we can ignore it.
	 */
	reg_invoke_rttMonGetConnectionHandle(RTT_RETVAL_NO_OP,
					     inRttMonCtrlAdminQItem,
					     targetOctetString,
					     &(applicationHandle.currentLUHandle));
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttCallingSscpLuClose);
	}
	lu_return_code = 
	    sscpLuClose(applicationHandle.
			currentLUHandle, 
			&returnedBuffer);
	if (lu_return_code != Success) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
						 RttSscpRuEchoApplicationFName,
						 RttMonSscpLuClose);
	    rtt_dump_lu_return_code(lu_return_code,
				    IDofMySelf);
	}
    }

    if (RTT_TRACE(IDofMySelf)) {
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

    free(dataRespBuffer);
    free(dataSendBuffer);
    reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
    reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
    delete_watched_boolean(&(applicationHandle.receiveFinished));
    free(watchedBooleanName);
    free(aHostName);
    process_kill(THIS_PROCESS);

}


static char * const ProcessName[] = {
    "SSCP RU RTR Probe 1",  "SSCP RU RTR Probe 2",
    "SSCP RU RTR Probe 3",  "SSCP RU RTR Probe 4",
    "SSCP RU RTR Probe 5",  "SSCP RU RTR Probe 6",
    "SSCP RU RTR Probe 7",  "SSCP RU RTR Probe 8",
    "SSCP RU RTR Probe 9",  "SSCP RU RTR Probe 10",
    "SSCP RU RTR Probe 11", "SSCP RU RTR Probe 12",
    "SSCP RU RTR Probe 13", "SSCP RU RTR Probe 14",
    "SSCP RU RTR Probe 15", "SSCP RU RTR Probe 16",
    "SSCP RU RTR Probe 17", "SSCP RU RTR Probe 18",
    "SSCP RU RTR Probe 19", "SSCP RU RTR Probe MaxName"
    };

void rtt_sscp_ru_echo_activate (rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem)
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
	process_create(&sscpRuEchoApplication,
		       ProcessName[entryNum],
		       LARGE_STACK,	/* dspu */
		       PRIO_LOW);
  
    if (inRttMonCtrlAdminQItem->pid != NO_PROCESS) {
	process_set_arg_ptr(inRttMonCtrlAdminQItem->pid,
			    inRttMonCtrlAdminQItem);   
    } else {
	reg_invoke_dumpRttMonProcessHeader(inRttMonCtrlAdminQItem->
					   ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   RttFailedToStartSscpRuEchoApplication);
	reg_invoke_rttMonProbePanicInternal(inRttMonCtrlAdminQItem);
    }

    return;
}

boolean rtt_sscp_ru_echo_exists (void)
{
    return (TRUE);
}

int rtt_sscp_ru_echo_validate (rttMonCtrlAdminEntry_t *ctrlAdminEntry, 
			       void *specificEntry,
			       int rttObject)
{
    rttMonEchoAdminEntry_t *echoAdminEntry;
    char                   *aHostName;
    LuApiReturnCode         lu_api_return_code;

    echoAdminEntry = (rttMonEchoAdminEntry_t *)
	specificEntry;

    switch (rttObject)  {
    case D_Check_RttType:
	if (ctrlAdminEntry->rttMonCtrlAdminRttType !=
	    D_rttMonCtrlAdminRttType_echo) {
	    return (D_INVALID_RttEntry);
	}
	break;
    case D_Check_Protocol:
	if ((ctrlAdminEntry->rttMonCtrlAdminRttType !=
	     D_rttMonCtrlAdminRttType_echo) ||
	    (echoAdminEntry->rttMonEchoAdminProtocol != 
	     D_rttMonEchoAdminProtocol_snaRUEcho)) {
	    return (D_INVALID_RttEntry);
	}
	break;
    case D_Check_Address:
	/*  If any length is zero, then it failed */
	if (echoAdminEntry->
	    rttMonEchoAdminTargetAddress->
	    length == 0) {
	    return (D_INVALID_RttEntry);
	}

	if ((aHostName = malloc(echoAdminEntry->
				rttMonEchoAdminTargetAddress->
				length + 1)) == NULL) {
	    return (D_INVALID_RttEntry);
	}

	memcpy(aHostName, echoAdminEntry->
	       rttMonEchoAdminTargetAddress->
	       octet_ptr, 
	       echoAdminEntry->
	       rttMonEchoAdminTargetAddress->
	       length);
	aHostName[echoAdminEntry->
		  rttMonEchoAdminTargetAddress->
		  length] = 0x00;

	if (((lu_api_return_code = sscpLuQueryPu(aHostName)) != Success) && 
	    (lu_api_return_code != HostConnInactive)) {
	    free(aHostName);
	    return (D_INVALID_RttEntry);
	}
	free(aHostName);
	break;
    case D_Check_Response_Size:
	if (echoAdminEntry->
	    rttMonEchoAdminPktDataRequestSize >
	    (0xFF - ARR_HEADER_SIZE)) {
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

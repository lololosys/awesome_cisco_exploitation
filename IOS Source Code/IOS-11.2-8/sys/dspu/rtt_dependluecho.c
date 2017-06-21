/* $Id: rtt_dependluecho.c,v 3.1.4.9 1996/08/28 12:42:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_dependluecho.c,v $
 *------------------------------------------------------------------
 * Dependent LU Echo Probe Application.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the probe implementation that is specific
 *  to Dependent LU's for the Round Trip Time Monitor Application. 
 *------------------------------------------------------------------
 * $Log: rtt_dependluecho.c,v $
 * Revision 3.1.4.9  1996/08/28  12:42:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.8  1996/07/29  21:47:04  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.1.4.7  1996/06/30  20:49:02  lmetzger
 * CSCdi61489:  When a LU fails to close, the probe did not wait
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/06/28  23:05:58  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/06/07  22:24:34  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/06/05  01:29:28  lmetzger
 * CSCdi59512:  Debug output for data wraps and for
 *                     sscpRu perform echo dump wraps
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/06/04  18:27:41  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 3.1.4.2  1996/05/22  18:07:11  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * outputing -> outputting
 *
 * Revision 3.1.4.1  1996/05/17  10:49:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/05/14  06:18:31  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.2  1996/04/19  02:39:10  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:50:54  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:45  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../rtt/msg_rtt.c"		/* Not a typo, see logger.h */
#include "rtt_dependluecho.h"

void rtt_dump_lu_return_code (LuApiReturnCode lu_return_code,
			      long            IDofMySelf)
{
    RTT_BUGINF2(IDofMySelf, "\t LuApiReturnCode of ");
    switch (lu_return_code) {
    case Success :
	RTT_BUGINF2(IDofMySelf,
		    "Success\n");
	break;
    case InsufficientMemory :
	RTT_BUGINF2(IDofMySelf,
		    "InsufficientMemory\n");
	break;
    case InvalidHandle :
	RTT_BUGINF2(IDofMySelf,
		    "InvalidHandle - invalid host name or\n\t   API handle\n");
	break;
    case HostConnInactive :
	RTT_BUGINF2(IDofMySelf,
		    "HostConnInactive - host connection and/or\n\t   PU inactive\n");
	break;
    case NoLuAvailable :
	RTT_BUGINF2(IDofMySelf,
		    "NoLuAvailable - there is no active LU\n");
	RTT_BUGINF2(IDofMySelf,
		    "\t  and one could no be activated via the DDDLU feature\n");
	break;
    case HostApplInvalid :
	RTT_BUGINF2(IDofMySelf,
		    "HostApplInvalid - requested host application\n");
	RTT_BUGINF2(IDofMySelf,
		    "\t   was not available at the host\n");
	break;
    case BindFailure :
	RTT_BUGINF2(IDofMySelf,
		    "BindFailure\n");
	break;
    case FlowClosed :
	RTT_BUGINF2(IDofMySelf,
		    "FlowClosed - session traffic flow reset\n\t   (ie no SDT yet)\n");
	break;
    case IRMViolation :
	RTT_BUGINF2(IDofMySelf,
		    "IRMViolation - immediate request mode\n\t   violation by appl\n");
	break;
    default :
	RTT_BUGINF2(IDofMySelf,
		    "unknown\n");
    } /* switch (lu_return_code) */

    return;
} /* rtt_dump_lu_return_code */

void rtt_dumpApplicationHandle (applicationHandle_t   *applicationHandle)
{
    RTT_BUGINF3(applicationHandle->IDofMySelf,
		"\t applicationHandle:  inRttMonCtrlAdminQItem = %x\n", 
		applicationHandle->inRttMonCtrlAdminQItem);
    RTT_BUGINF4(applicationHandle->IDofMySelf,
		"\t receiveFinished = %s  currentLUHandle = %x\n", 
		(process_get_boolean(applicationHandle->receiveFinished) ?
		 "TRUE": "FALSE"),
		applicationHandle->currentLUHandle);
    RTT_BUGINF4(applicationHandle->IDofMySelf,
		"\t maxRespBufferLen = %u  receivedBufferLen = %u\n",
		applicationHandle->maxRespBufferLen,
		applicationHandle->receivedBufferLen);
    RTT_BUGINF4(applicationHandle->IDofMySelf,
		"\t IDofMyself = %#x  errorSense = %s\n",
		applicationHandle->IDofMySelf,
		(applicationHandle->errorSense) ? 
		"TRUE" :  "FALSE");
    RTT_BUGINF4(applicationHandle->IDofMySelf,
		"\t IDofMyself = %#x  closeIndicationCalled = %s\n",
		applicationHandle->IDofMySelf,
		(applicationHandle->closeIndicationCalled) ? 
		"TRUE" :  "FALSE");

    return;
} /* rtt_dumpApplicationHandle */

static void dumpBuffer(unsigned char *bufferPtr,
		       unsigned int  i,
		       long          IDofMySelf)
{
    unsigned int j;

    j = 0;
    RTT_BUGINF2(IDofMySelf,
		"\t ");
    for (; i > 0; i--) {
	if ((j != 0) &&
	    (j != 1) &&
	    ((j%8) == 0)) {
	    RTT_BUGINF2(IDofMySelf, " ");
	}

	if (++j >= 33) {
	    RTT_BUGINF2(IDofMySelf,
			"\n\t ");
	    j = 1;
	}
	if (*bufferPtr < 16) {
	    RTT_BUGINF2(IDofMySelf,"0");
	}
	RTT_BUGINF3(IDofMySelf,
		    "%x ", *(bufferPtr++));
    } /* for (; i > 0; i--) */
    RTT_BUGINF2(IDofMySelf,
		"\n");
}

void rtt_dumpRespBuffer (applicationHandle_t  *applicationHandle,
			 unsigned char        *bufferPtr,
			 unsigned int          bufferLen)
{
    unsigned int      i;

    RTT_BUGINF3(applicationHandle->IDofMySelf,
		"\t Data in Hex (Len = %u):  ", bufferLen);
    if (bufferLen <= applicationHandle->maxRespBufferLen) {
	i = bufferLen;
    } else {
	i = applicationHandle->maxRespBufferLen;
    } /* else of if (bufferLen <= ... */

    if (i > 160) {
	RTT_BUGINF2(applicationHandle->IDofMySelf,
		    "(Only outputting first 160 bytes)");
	i = 160;
    }

    RTT_BUGINF2(applicationHandle->IDofMySelf,
		"\n");

    if (bufferLen > applicationHandle->maxRespBufferLen) {
	RTT_BUGINF2(applicationHandle->IDofMySelf,
		    "\t (Received data larger than expected; truncated!\n");
    }

    dumpBuffer(bufferPtr, i, applicationHandle->IDofMySelf);

    return;
} /* rtt_dumpRespBuffer */

void rtt_dumpSendBuffer (unsigned char        *bufferPtr,
			 unsigned int          bufferLen,
			 long                  IDofMySelf)
{
    unsigned int      i;

    RTT_BUGINF3(IDofMySelf,
		"\t Data in Hex (Len = %u):  ", bufferLen);
    i = bufferLen;

    if (i > 160) {
	RTT_BUGINF2(IDofMySelf,
		    "(Only outputting first 160 bytes)");
	i = 160;
    }
    RTT_BUGINF2(IDofMySelf,
		"\n");

    dumpBuffer(bufferPtr, i, IDofMySelf);

    return;
} /* rtt_dumpSendBuffer */

void rtt_stripTrailingBlanks (char *instring)
{
    int string_length;

    if ((instring == NULL) || 
	((string_length = strlen(instring)) == 0)) {
	return;
    }

    string_length--;

    while ((string_length >= 0) &&
	   (instring[string_length] == ' ')) {
	string_length--;
    }

    string_length++;
    instring[string_length] = 0x00;

    return;
  
} /* rtt_stripTrailingBlanks */

static void getMyHostName (char **myHostName,
			   long   IDofMySelf)
{
    extern char *hostname;		/* defined in os/hostname.c */

    if ((*myHostName = malloc(strlen(hostname))) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttGetMyHostNameFName,
					     RttMonMallocMyHostName);
	return;
    }

    strcpy(*myHostName, hostname);

    return;

} /* getMyHostName */

static long fillInitSelfBuffer (unsigned char         *initSelfBuffer,
				unsigned char const   *initSelfSignature,
				unsigned char const   *aRouterName,
				unsigned int          versionNum)
{
    reg_invoke_rttMonConvertAsciiToEbcdic(initSelfSignature, 
					  &initSelfBuffer[0],
					  7);
    initSelfBuffer[7] = (unsigned char)versionNum;
    reg_invoke_rttMonConvertAsciiToEbcdic(aRouterName, 
					  &initSelfBuffer[8],
					  strlen(aRouterName));
  
    return (strlen(aRouterName) + 8);
} /* fillInitSelfBuffer */

static void dumpOpenResult (applicationHandle_t   *applicationHandle,
			    char                  *aHostName,
			    char                  *aApplName,
			    unsigned char         *eApplName,
			    unsigned char         *eModeName,
			    long                  initSelfBuffLen,
			    unsigned char         *initSelfBuffer,
			    unsigned char         *bindDataBuffer,
			    unsigned int          bindDataCount,
			    unsigned char         locAddr,
			    short                 sysSense,
			    short                 userSense)
{
    rtt_dumpApplicationHandle(applicationHandle);
    RTT_BUGINF5(applicationHandle->IDofMySelf,
		"\t aHostName = %s  locAddr = %u  eApplNameLen = %u\n", 
		aHostName,
		locAddr,
		strlen(aApplName));
    RTT_BUGINF10(applicationHandle->IDofMySelf,
		 "\t eApplName = %2x%2x%2x%2x%2x%2x%2x%2x  ",
		 (unsigned char)eApplName[0],
		 (unsigned char)eApplName[1],
		 (unsigned char)eApplName[2],
		 (unsigned char)eApplName[3],
		 (unsigned char)eApplName[4],
		 (unsigned char)eApplName[5],
		 (unsigned char)eApplName[6],
		 (unsigned char)eApplName[7]);
    RTT_BUGINF10(applicationHandle->IDofMySelf,
		 "eModeName = %2x%2x%2x%2x%2x%2x%2x%2x  ",
		 (unsigned char)eModeName[0],
		 (unsigned char)eModeName[1],
		 (unsigned char)eModeName[2],
		 (unsigned char)eModeName[3],
		 (unsigned char)eModeName[4],
		 (unsigned char)eModeName[5],
		 (unsigned char)eModeName[6],
		 (unsigned char)eModeName[7]);
    RTT_BUGINF3(applicationHandle->IDofMySelf,
		"userDataLen = %u\n", initSelfBuffLen);
    RTT_BUGINF10(applicationHandle->IDofMySelf,
		 "\t userData = %2x%2x%2x%2x%2x%2x%2x%2x  ",
		 (unsigned char)initSelfBuffer[0],
		 (unsigned char)initSelfBuffer[1],
		 (unsigned char)initSelfBuffer[2],
		 (unsigned char)initSelfBuffer[3],
		 (unsigned char)initSelfBuffer[4],
		 (unsigned char)initSelfBuffer[5],
		 (unsigned char)initSelfBuffer[6],
		 (unsigned char)initSelfBuffer[7]);
    RTT_BUGINF5(applicationHandle->IDofMySelf,
		"sysSense = %u  userSense = %u  bindDataLen = %u\n",
		sysSense,
		userSense,
		BINDDATA_BUFF_SIZE);
    RTT_BUGINF10(applicationHandle->IDofMySelf,
		 "\t bindData = %2x%2x%2x%2x%2x%2x%2x%2x  ",
		 (unsigned char)bindDataBuffer[0],
		 (unsigned char)bindDataBuffer[1],
		 (unsigned char)bindDataBuffer[2],
		 (unsigned char)bindDataBuffer[3],
		 (unsigned char)bindDataBuffer[4],
		 (unsigned char)bindDataBuffer[5],
		 (unsigned char)bindDataBuffer[6],
		 (unsigned char)bindDataBuffer[7]);
    RTT_BUGINF3(applicationHandle->IDofMySelf,
		"bindDataCount = %u\n", bindDataCount);
  
    return;

} /* dumpOpenResult */

static void iAmPanicking (rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem,
			  long                    IDofMySelf)
{
    reg_invoke_rttMonProbePanic(inRttMonCtrlAdminQItem);
    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
				       RttDependLuPanicMsg);
    process_kill(THIS_PROCESS);

    return;
}

static void splitLUTargetAddress (OctetString *hopOctetString,
				  char        **aHostName,
				  long        *aHostNameLen,
				  char        **aApplName,
				  long        *aApplNameLen,
				  char        **aModeName,
				  long        *aModeNameLen,
				  long        IDofMySelf)
{
    unsigned char *current_source_char;
    char *current_target_char;
    char *tmp_str;
    long current_length, section_length;
    long section1_length_in_OS;
    long section2_length_in_OS;
    long section3_length_in_OS;
    int  section;

    if (hopOctetString == NULL)	{
	return;
    }

    /*
     * Variable Descriptions:
     *  current_source_char   - as named
     *  current_length        - total length of OctetString (OS)
     *  current_target_char   - as named
     *  section               - portion of OS being split
     *  section_length        - count chars processed in OS
     *  section#_length_in_OS - how many chars suppose to be in OS
     *                           section (not null terminated)
     */
    current_source_char = hopOctetString->octet_ptr;
    section1_length_in_OS = *current_source_char;
    section2_length_in_OS = 
	current_source_char[section1_length_in_OS + 1];
    section3_length_in_OS = 
	current_source_char[section1_length_in_OS +
			    section2_length_in_OS + 2];
					      
    /* 
     *  Check for reality of length and be very very 
     *    strict, if not we could crash!!!!
     */
    if ((section1_length_in_OS > hopOctetString->length) ||
	(section2_length_in_OS > hopOctetString->length) ||
	((section3_length_in_OS != 0) &&
	 (section3_length_in_OS != 8)) ||
	((section1_length_in_OS +
	  section2_length_in_OS +
	  section3_length_in_OS + 3) !=
	 hopOctetString->length) ||
	(hopOctetString->length >
	 MAX_rttMonTargetAddress) ||
	(hopOctetString->length < 3) ||
	(section1_length_in_OS < 0) ||
	(section2_length_in_OS < 0) ||
	(section3_length_in_OS < 0)) {
	*aHostName = NULL;
	*aHostNameLen = 0;
	*aApplName = NULL;
	*aApplNameLen = 0;
	*aModeName = NULL;
	*aModeNameLen = 0;
	return;
    }

    *aHostNameLen = section1_length_in_OS;
    if ((tmp_str = malloc(section1_length_in_OS + 1)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSplitLUTargetAddressFName,
					     RttMonMallocPtraHostName);
	*aHostName = NULL;
	*aHostNameLen = 0;
	*aApplName = NULL;
	*aApplNameLen = 0;
	*aModeName = NULL;
	*aModeNameLen = 0;
	return;
    }
    memset(tmp_str, 'Z', section1_length_in_OS);
    tmp_str[section1_length_in_OS] = 0x00;
    *aHostName = tmp_str;

    *aApplNameLen = section2_length_in_OS;
    if ((tmp_str = malloc(section2_length_in_OS + 1)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSplitLUTargetAddressFName,
					     RttMonMallocPtraApplName);
	free(*aHostName);
	*aHostName = NULL;
	*aHostNameLen = 0;
	*aApplName = NULL;
	*aApplNameLen = 0;
	*aModeName = NULL;
	*aModeNameLen = 0;
	return;
    }
    memset(tmp_str, 'Z', section2_length_in_OS);
    tmp_str[section2_length_in_OS] = 0x00;
    *aApplName = tmp_str;

    *aModeNameLen = 8;
    /* Mode must be 8 characters */
    if ((tmp_str = malloc(9)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttSplitLUTargetAddressFName,
					     RttMonMallocPtraModeName);
	free(*aHostName);
	*aHostName = NULL;
	*aHostNameLen = 0;
	free(*aApplName);
	*aApplName = NULL;
	*aApplNameLen = 0;
	*aModeName = NULL;
	*aModeNameLen = 0;
	return;
    }
    memset(tmp_str, 'Z', 8);
    tmp_str[8] = 0x00;  
    *aModeName = tmp_str;
  
    current_length = hopOctetString->length;
    section = 1;         
    section_length = 0;  
    current_target_char = *aHostName;
    current_source_char++;
    /* Go through all characters in the OS  */
    while (current_length > 0) {
	if (section == 1) {
	    if (section_length < section1_length_in_OS) {
		*current_target_char = (char)*current_source_char;
		current_source_char++;
		current_target_char++;
		current_length--;
		section_length++;
	    } else {
		rtt_stripTrailingBlanks(*aHostName);
		section = 2;
		section_length = 0;
		current_target_char = *aApplName;
		current_length--;
		current_source_char++;
	    } /* else of if (section_length <... */
	} else if (section == 2) {
	    if (section_length < section2_length_in_OS) {
		*current_target_char = *current_source_char;
		current_source_char++;
		current_target_char++;
		current_length--;
		section_length++;
	    } else {
		rtt_stripTrailingBlanks(*aApplName);
		section = 3;
		section_length = 0;
		current_target_char = *aModeName;
		current_length--;
		current_source_char++;
	    } /* else of if (section_length... */
	} else {			/* else if (section == 2) */
	    if (section_length < section3_length_in_OS)	{
		*current_target_char = *current_source_char;
		current_source_char++;
		current_target_char++;
		current_length--;
		section_length++;
	    } else {
		if (current_length > 1) {
		    /*
		     *  Wrong length bound into OS.
		     */
		    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						       RttBadTargetAddressLength);
		    free(*aHostName);
		    *aHostName = NULL;
		    *aHostNameLen = 0;
		    free(*aApplName);
		    *aApplName = NULL;
		    *aApplNameLen = 0;
		    free(*aModeName);
		    *aModeName = NULL;
		    *aModeNameLen = 0;
		    return;
		} else {
		    /*
		     * Eat the last character
		     */
		    current_length--;
		}
	    }
	} /* else of else if (section == 2) */
    } /* while */

    /* Pad the Mode with blanks */
    if (section3_length_in_OS == 0) {
	section_length = 0;
	while (section_length < 8) {
	    *current_target_char = ' ';
	    current_target_char++;
	    section_length++;
	}
    }

    return;
} /* splitLUTargetAddress */

void rtt_closeIndication (void            *applicationHandle,
			  unsigned char   *bufferPtr)
{
    if (RTT_TRACE(((applicationHandle_t*)applicationHandle)->
		  IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
					    applicationHandle)->IDofMySelf,
					   RttCloseIndicationCalled);
	rtt_dumpApplicationHandle(applicationHandle);
	reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
					    applicationHandle)->IDofMySelf,
					   RttSettingReceiveFinishTrue);
	RTT_BUGINF2(((applicationHandle_t *)applicationHandle)->IDofMySelf, 
		    "\t in rtt_closeIndication\n");
    }
    process_set_boolean(((applicationHandle_t *)(applicationHandle))->
			receiveFinished,
			TRUE);
    ((applicationHandle_t *)(applicationHandle))->
	closeIndicationCalled = TRUE;

    return;
} /* rtt_closeIndication */

static void openConnection (OctetString 
			    *hopOctetString,
			    char *aHostName,
			    long  hostNameLen,
			    char *aApplName,
			    long  applNameLen,
			    char *aModeName,
			    long  modeNameLen,
			    applicationHandle_t                    
			    *applicationHandle,
			    unsigned short *sequenceNum)
{
    long                  return_code;
    LuApiReturnCode       lu_return_code;
    boolean               isHopConnected;
    unsigned char         *eApplName;
    unsigned char         *eModeName;
    char                  *aRouterName;
    unsigned char         *initSelfBuffer;
    unsigned char         *bindDataBuffer;
    unsigned int          bindDataCount;
    unsigned char         locAddr;
    short                 sysSense;
    short                 userSense;
    long                  initSelfBuffLen;
    OctetString           *diagTextOS;

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
	*sequenceNum = 1; /* starts at 1, zero for logoff only */
	if ((eApplName = malloc(applNameLen + 1)) == NULL) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttMonOpenConnectionFName,
						 RttMonMalloceApplName);
	    return;
	}
	if ((eModeName = malloc(modeNameLen + 1)) == NULL) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttMonOpenConnectionFName,
						 RttMonMalloceModeName);
	    free(eApplName);
	    return;
	}

	getMyHostName(&aRouterName, applicationHandle->IDofMySelf);
	if (aRouterName == NULL) {
	    free(eApplName);
	    free(eModeName);
	    return;
	}

	if ((initSelfBuffer = 
	     malloc(strlen(aRouterName) + 8)) == NULL) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttMonOpenConnectionFName,
						 RttMonMallocInitSelfBuffer);
	    free(eApplName);
	    free(eModeName);
	    free(aRouterName);
	    return;
	}

	if ((bindDataBuffer = malloc(BINDDATA_BUFF_SIZE)) == NULL) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttMonOpenConnectionFName,
						 RttMonMallocBindDataBuffer);
	    free(eApplName);
	    free(eModeName);
	    free(aRouterName);
	    free(initSelfBuffer);
	    return;
	}

	reg_invoke_rttMonConvertAsciiToEbcdic((unsigned char const *)aApplName, 
					      eApplName,
					      strlen(aApplName));
	reg_invoke_rttMonConvertAsciiToEbcdic((unsigned char const *)aModeName, 
					      eModeName,
					      strlen(aModeName));

	initSelfBuffLen = fillInitSelfBuffer(initSelfBuffer,
					     (unsigned char const *)INIT_SELF_SIGNATURE,
					     (unsigned char const *)aRouterName,
					     RTT_ARR_VERSION_NUM);

	locAddr = 0;                    

	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttCallingLuT0orT2Open);
	}
    
	applicationHandle->closeIndicationCalled = FALSE;
	/* open Connection */
	lu_return_code =
	    luT0orT2Open((void *)applicationHandle,
			 &(applicationHandle->currentLUHandle),
			 aHostName,
			 &locAddr,
			 strlen(aApplName),
			 eApplName,
			 eModeName,
			 initSelfBuffLen,
			 initSelfBuffer,
			 &rtt_closeIndication,
			 &sysSense,
			 &userSense,
			 BINDDATA_BUFF_SIZE,
			 bindDataBuffer,
			 &bindDataCount
			 );
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttDumpingLuT0orT2OpenResult);
	    dumpOpenResult(applicationHandle,
			   aHostName,
			   aApplName,
			   eApplName,
			   eModeName,
			   initSelfBuffLen,
			   initSelfBuffer,
			   bindDataBuffer,
			   bindDataCount,
			   locAddr,
			   sysSense,
			   userSense);
	}
	if (lu_return_code != Success) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttErrorReturnCode);
	    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						 RttMonOpenConnectionFName,
						 RttMonLuT0orT2Open);
	    rtt_dump_lu_return_code(lu_return_code,
				    applicationHandle->IDofMySelf);
	    /* don't dump it twice */
	    if (!RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttDumpingLuT0orT2OpenResult);
		dumpOpenResult(applicationHandle,
			       aHostName,
			       aApplName,
			       eApplName,
			       eModeName,
			       initSelfBuffLen,
			       initSelfBuffer,
			       bindDataBuffer,
			       bindDataCount,
			       locAddr,
			       sysSense,
			       userSense);
	    }
	    /* create a placeholder for the HopConnectedState */
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingSetConnectionHandlePlaceholder);
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
	    if (bindDataCount < 8) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttBindDataLengthShorterThanExpected);
		RTT_BUGINF3(applicationHandle->IDofMySelf,
			    "\t expecting 8 or more octets, got %u.\n",
			    bindDataCount);
	    } else {			/* if (bindDataCount < 8) */
		diagTextOS = malloc(sizeof(OctetString));
		if (diagTextOS == NULL) {
		    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						       RttErrorReturnCode);
		    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							 RttMonOpenConnectionFName,
							 RttMonMallocDiagTextOS);
		} else {		/* if (diagTextOS == NULL) */
		    diagTextOS->octet_ptr = malloc(bindDataCount);
		    if (diagTextOS->octet_ptr == NULL) {
			reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							   RttErrorReturnCode);
			reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
							     RttMonOpenConnectionFName,
							     RttMonMallocDiagTextOSOctetPtr);
			reg_invoke_rttMonFreeOctetStringBuffer(diagTextOS);
		    } else {		/* if (diagTextOS->octet_ptr == NULL) */
			memcpy(diagTextOS->octet_ptr, 
			       &bindDataBuffer[0], bindDataCount);
			diagTextOS->length = bindDataCount;
			if (RTT_TRACE(applicationHandle->IDofMySelf)) {
			    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							       RttCallingSetDiagText);
			}
			return_code = 
			    reg_invoke_rttMonSetDiagText(RTT_RETVAL_NO_OP,
							 applicationHandle->inRttMonCtrlAdminQItem,
							 diagTextOS);
			reg_invoke_rttMonFreeOctetStringBuffer(diagTextOS);
			if (return_code != D_Return_Successfull) {
			    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
							       RttErrorReturnCode);
			    reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
								 RttMonOpenConnectionFName,
								 RttMonSetDiagTextName);
			    reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
							       return_code);
			} /* if (return_code != D_Return_Successfull) */
		    } /* else of if (diagTextOS->octet_ptr == NULL) */
		} /* else of if (diagTextOS == NULL) */
	    } /* else of if (bindDataLen < 8) */
    
	} /* else of if (lu_return_code != Success) */

	free(eApplName);
	free(eModeName);
	free(aRouterName);
	free(initSelfBuffer);
	free(bindDataBuffer);

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
    static const char receiveIndication_FName[] = "receiveIndication";
    LuApiReturnCode   luAPIReturnCode;
    long              newBufferLen;
    unsigned char     *newBufferPtr;

    if (RTT_TRACE(((applicationHandle_t*)applicationHandle)->
		  IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
					    applicationHandle)->IDofMySelf,
					   RttReceiveIndicationCalled);
	rtt_dumpApplicationHandle((applicationHandle_t *)applicationHandle);
	rtt_dumpRespBuffer((applicationHandle_t *)applicationHandle,
			   bufferPtr,
			   bufferLen);
	RTT_BUGINF5(((applicationHandle_t *)applicationHandle)->IDofMySelf,
		    "\t lastFrame = %s  systemSense = %u userSense = %u\n", 
		    (lastFrame ? "TRUE": "FALSE"),
		    systemSense,
		    userSense);
    } /* if (RTT_TRACE(applicationHandle->IDofMySelf)) */

    if (systemSense || userSense) {
	((applicationHandle_t *)(applicationHandle))->errorSense = TRUE;
	process_set_boolean(((applicationHandle_t *)(applicationHandle))->receiveFinished,
			    TRUE);
	if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf,
					       RttSettingReceiveFinishTrue);
	    RTT_BUGINF3(((applicationHandle_t *)applicationHandle)->IDofMySelf, 
			"\t in %s, because base Sense Code\n",
			receiveIndication_FName);
	}
    } else if (lastFrame) {
	((applicationHandle_t *)applicationHandle)->receivedBufferLen =+
	    bufferLen;
	process_set_boolean(
			    ((applicationHandle_t *)(applicationHandle))->receiveFinished,
			    TRUE);
	if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->IDofMySelf)) {
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
	if (RTT_TRACE(((applicationHandle_t *)applicationHandle)->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(((applicationHandle_t *)
						applicationHandle)->IDofMySelf,
					       RttCallingLUReceiveContinue);
	    RTT_BUGINF4(((applicationHandle_t *)applicationHandle)->IDofMySelf,
			"\t in %s with handle: %u\n", 
			receiveIndication_FName,
			((applicationHandle_t *)applicationHandle)->currentLUHandle);
	}
	luAPIReturnCode = 
	    luReceiveContinue(((applicationHandle_t *)
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
						 RttMonLuReceiveContinue);
	    rtt_dump_lu_return_code(luAPIReturnCode,
				    ((applicationHandle_t *)
				     applicationHandle)->IDofMySelf);
	}
  
    } /* else of if (lastFrame) */

    return;

} /* receiveIndication */

static void sendLogOff (unsigned char            *dataRespBuffer,
			unsigned char            *dataSendBuffer,
			long                     echoTimeout,
			applicationHandle_t      *applicationHandle)
{
    static const char sendLogOff_FName[] = "sendLogOff";
    LuApiReturnCode   luAPIReturnCode;
    boolean           blockState;
    long              sendBufferSize;
    long              return_code;
    boolean           valid_response = FALSE;

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttSendLogoffCalled);
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttCallingLuReceive);
    }

    applicationHandle->receivedBufferLen = 0;
    applicationHandle->errorSense = FALSE;
    /* luReceive */
    luAPIReturnCode = 
	luReceive(applicationHandle->currentLUHandle,
		  applicationHandle->maxRespBufferLen,
		  dataRespBuffer,
		  &receiveIndication);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttSendLogOffFName,
					     RttMonLuReceive);
	rtt_dump_lu_return_code(luAPIReturnCode,
				applicationHandle->IDofMySelf);
	return;
    } /* if (luAPIReturnCode != Success) */
	
    /* 
     *  Remember the sequence number is zero for logoff
     */
    return_code = 
	reg_invoke_rttMonSetupEBCDICDataPortion(RTT_RETVAL_NO_OP,
						applicationHandle->inRttMonCtrlAdminQItem,
						dataSendBuffer,
						0,
						&sendBufferSize,
						0, 
						D_rttMonDataPortionCommand_logoff);
    if (return_code != D_Return_Successfull) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttSendLogOffFName,
					     RttMonSetupEBCDICDataName);
	reg_invoke_dumpRttMonLibReturnCode(applicationHandle->IDofMySelf,
					   return_code);
	return;
    }

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttCallingLuSend);
    }
    /* luSend */
    luAPIReturnCode = 
	luSend(applicationHandle->currentLUHandle,
	       sendBufferSize,
	       dataSendBuffer);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttSendLogOffFName,
					     RttMonLuSend);
	rtt_dump_lu_return_code(luAPIReturnCode,
				applicationHandle->IDofMySelf);
	return;
    } /* if (luAPIReturnCode != Success) */

    while (!valid_response) {
	blockState = process_get_boolean(applicationHandle->receiveFinished);
	if (blockState == FALSE) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttReceiveFinishedIsFalse);
		RTT_BUGINF4(applicationHandle->IDofMySelf,
			    "\t in %s calling\n\t process_wait_for_event_timed( %u milliseconds )\n",
			    sendLogOff_FName, echoTimeout + 5000);
	    }
	    /* 
	     * Give the Logoff extra time to complete, since it is 
	     *  not critical.  5 Seconds should be enough.  But,
	     *  a response is not required, so those implementations
	     *  will wait 5 + timeout every time.
	     */
	    process_wait_for_event_timed((unsigned long)echoTimeout + 5000);
	}

	blockState = process_get_boolean(applicationHandle->receiveFinished);
	if (blockState == FALSE) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
						   RttWokeOnTimeout);
	    }
	    /*
	     *  A timeout is considered valid response
	     */
	    valid_response = TRUE;  
	} else {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf, 
						   RttWokeOnRealEvent);
	    }
	    /*
	     *  If the data received is not the outstanding close
	     *    successfull,  go back and wait for the close.
	     */
	    if (applicationHandle->closeIndicationCalled == FALSE) {
		if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		    RTT_BUGINF2(applicationHandle->IDofMySelf,
				"\t still need to wait for the LU to close\n");
		    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						       RttSettingReceiveFinishedFalse);
		}
		process_set_boolean(applicationHandle->receiveFinished, FALSE);
		valid_response = FALSE;
	    } else {
		valid_response = TRUE;
	    }
	}
    } /* while (!valid_response) */

    return;

}

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
    static const char echoTarget_FName[] = "echoTarget";
    LuApiReturnCode   luAPIReturnCode;
    boolean           blockState;
   
    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttEchoTargetCalled); 
	rtt_dumpApplicationHandle(applicationHandle);
	RTT_BUGINF3(applicationHandle->IDofMySelf,
		    "\t echoTimeout (milliseconds) = %u\n", echoTimeout);
	rtt_dumpSendBuffer(dataSendBuffer,
			   sendBufferSize,
			   applicationHandle->IDofMySelf);
	RTT_BUGINF2(applicationHandle->IDofMySelf,
		    "\t calling luReceive...\n");
    }
    applicationHandle->receivedBufferLen = 0;
    applicationHandle->errorSense = FALSE;
    /* luReceive */
    luAPIReturnCode = 
	luReceive(applicationHandle->currentLUHandle,
		  applicationHandle->maxRespBufferLen,
		  dataRespBuffer,
		  &receiveIndication);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttMonEchoTargetFName,
					     RttMonLuReceive);
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
					   RttCallingLuSend);
    }
    /* luSend */
    luAPIReturnCode = 
	luSend(applicationHandle->currentLUHandle,
	       sendBufferSize,
	       dataSendBuffer);
    if (luAPIReturnCode != Success) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
					     RttMonEchoTargetFName,
					     RttMonLuSend);
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

static void setupPathInfo (rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem,
			   char                    *aHostName,
			   char                    *aApplName,
			   char                    *aModeName,
			   OctetString             *targetOctetString,
			   long                     IDofMySelf)
{
    long                    return_code;

    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, 
					   RttSetupPathInfoCalled);
	RTT_BUGINF2(IDofMySelf,
		    "\t Calling rttMonStartUpdatePath\n");
    }
    reg_invoke_rttMonStartUpdatePath(inRttMonCtrlAdminQItem);
    if (RTT_TRACE(IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, 
					   RttCallingUpdatePath);
	RTT_BUGINF5(IDofMySelf, "\t for Target %s %s %s\n", 
		    aHostName,
		    aApplName,
		    aModeName);
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

static void performEcho (OctetString             *destOctetString,
			 char                    *aHostName,
			 char                    *aApplName,
			 char                    *aModeName,
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

    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					   RttPerformEchoCalled); 
	rtt_dumpApplicationHandle(applicationHandle);
	RTT_BUGINF4(applicationHandle->IDofMySelf,
		    "\t echoTimeout (milliseconds) = %u  sequenceNum = %u\n", 
		    echoTimeout,
		    *sequenceNum);
	RTT_BUGINF5(applicationHandle->IDofMySelf,
		    "\t destination address is %s %s %s\n",
		    aHostName,
		    aApplName,
		    aModeName);
	RTT_BUGINF3(applicationHandle->IDofMySelf,
		    "\t verifyDataFlag = %s\n", 
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

    (*sequenceNum)++;
    /*
     * The value of zero for sequence number is reserved for logoffs.
     */
    if (*sequenceNum == 0) {
	*sequenceNum = 1;
    }
    return_code = 
	reg_invoke_rttMonSetupEBCDICDataPortion(RTT_RETVAL_NO_OP,
						applicationHandle->inRttMonCtrlAdminQItem,
						dataSendBuffer,
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
       D_echoTarget_internalError = 3,
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
    /* handle echo return_code */
    if ((echo_return_code == D_echoTarget_timeout) ||
	(responseTime > echoTimeout)) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttReturnedFromEchoTarget);
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t with D_echoTarget_timeout return_code\n");
	    RTT_BUGINF3(applicationHandle->IDofMySelf,
			"\t and responseTime (milliseconds) = %u\n", responseTime);
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
					       RttSettingReceiveFinishedFalse);
	}
	
	/*
	 * If the receiveFinished has not been set (in either
	 *  callback), then the request to the host is still
	 *  outstanding.  The protocol says that a request/response
	 *  must be in a pair, and it cannot handle multiple
	 *  outstanding requests.
	 */
	/*
	 * Wait for the LU to finish its data exchange
	 *   Wait for atleast 15 seconds to clear.
	 */
	if (!process_get_boolean(applicationHandle->receiveFinished)) {
	    process_wait_for_event_timed((unsigned long)echoTimeout + 15000);
	}	
	if (process_get_boolean(applicationHandle->receiveFinished)) {
	    process_set_boolean(applicationHandle->receiveFinished, FALSE);
	    sendLogOff(dataRespBuffer,
		       dataSendBuffer,
		       echoTimeout,
		       applicationHandle);
	} 

	/*
	 * If the close Indication did not happen because of the
	 *    LogOff, then the LU is still open and we must
	 *    close it.
	 */
	if (applicationHandle->closeIndicationCalled == FALSE) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingLuClose);
	    }
	    lu_return_code = 
		luClose(applicationHandle->
			currentLUHandle, 
			&returnedBuffer);
	    if (lu_return_code != Success) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						     RttPerformEchoFName,
						     RttMonLuCloseName);
		rtt_dump_lu_return_code(lu_return_code,
					applicationHandle->IDofMySelf);
	    }
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
    } else if (echo_return_code == D_echoTarget_connectionLost)	{
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
    } else if (echo_return_code == D_echoTarget_internalError) {
	if (RTT_TRACE(applicationHandle->IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
					       RttReturnedFromEchoTarget);
	    RTT_BUGINF2(applicationHandle->IDofMySelf,
			"\t with D_echoTarget_internalError return_code\n");
	    RTT_BUGINF3(applicationHandle->IDofMySelf,
			"\t and responseTime (milliseconds) = %u\n", responseTime);
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
					       RttSettingReceiveFinishedFalse);
	}
		
	/*
	 * If the receiveFinished has not been set (in either
	 *  callback), then the request to the host is still
	 *  outstanding.  The protocol says that a request/response
	 *  must be in a pair, and it cannot handle multiple
	 *  outstanding requests.
	 */
	/*
	 * Wait for the LU to finish its data exchange
	 *   Wait for atleast 15 seconds to clear.
	 */
	if (!process_get_boolean(applicationHandle->receiveFinished)) {
	    process_wait_for_event_timed((unsigned long)echoTimeout +
					 15000);
	}	
	if (process_get_boolean(applicationHandle->receiveFinished)) {
	    process_set_boolean(applicationHandle->receiveFinished, FALSE);
	    sendLogOff(dataRespBuffer,
		       dataSendBuffer,
		       echoTimeout,
		       applicationHandle);
	}
	/*
	 * If the close Indication did not happen because of the
	 *    LogOff, then the LU is still open and we must
	 *    close it.
	 */
	if (applicationHandle->closeIndicationCalled == FALSE) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingLuClose);
	    }
	    lu_return_code = 
		luClose(applicationHandle->
			currentLUHandle, 
			&returnedBuffer);
	    if (lu_return_code != Success) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(applicationHandle->IDofMySelf,
						     RttPerformEchoFName,
						     RttMonLuCloseName);
		rtt_dump_lu_return_code(lu_return_code,
					applicationHandle->IDofMySelf);
	    }
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
			"\t and responseTime (milliseconds) = %u\n", responseTime);
	}
	goodResponse = TRUE;
	/* if verify data, verify data */
	if (verifyDataFlag) {
	    if (RTT_TRACE(applicationHandle->IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(applicationHandle->IDofMySelf,
						   RttCallingVerifyEBCDICPortion);
	    }
	    return_code = 
		reg_invoke_rttMonVerifyEBCDICDataPortion(RTT_RETVAL_NO_OP,
							 applicationHandle->inRttMonCtrlAdminQItem,
							 *sequenceNum,
							 dataRespBuffer);
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
    } /* else part of return_code from echo target */
 
    return;

} /* performEcho */

static void dependLuEchoApplication (void)
{
    static const char       dependLuEchoApplication_FName[] = 
	"dependLuEchoApplication";
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
    long                    responseDataSize;
    unsigned short          sequenceNum;
    boolean                 verifyDataFlag;
    char                    *watchedBooleanName;
    long                    long_time_count;
    watched_boolean         *pendingBoolean;
    char                    *aHostName;
    long                    hostNameLen;
    char                    *aApplName;
    long                    applNameLen;
    char                    *aModeName;
    long                    modeNameLen;

    if (!process_get_arg_ptr((void **)&inRttMonCtrlAdminQItem)) {
	RTT_BUGINF3(0,
		    "RTR 0: %s - Failed to get arg pointer\n",
		    dependLuEchoApplication_FName);
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

    if ((watchedBooleanName = malloc(strlen("dependLuEchoProbe") + 5)) 
	== NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttDependLuApplicationFName,
					     RttMonMallocWatchedBooleanName);
	iAmPanicking(inRttMonCtrlAdminQItem, 
		     IDofMySelf);
    }

    watchedBooleanName[0] = 0x00;
    sprintf(watchedBooleanName, "dependLuEchoProbe %u",
	    inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminIndex);
    if ((applicationHandle.receiveFinished = 
	 (watched_boolean *)create_watched_boolean(watchedBooleanName,
						   0)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttDependLuApplicationFName,
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
    responseDataSize = 
	reg_invoke_getRttMonEchoAdminPktDataResponseSize(RTT_RETVAL_NO_OP,
							 inRttMonCtrlAdminQItem);

    applicationHandle.closeIndicationCalled = FALSE;
    applicationHandle.errorSense = FALSE;
    applicationHandle.receivedBufferLen = 0;
    applicationHandle.maxRespBufferLen = responseDataSize + ARR_HEADER_SIZE + 40;
    /* added 20 bytes for bad buffer overrun */
    if ((dataRespBuffer = malloc(applicationHandle.
				 maxRespBufferLen + 20)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf, 
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttDependLuApplicationFName,
					     RttMonMallocDataRespBuffer);
	delete_watched_boolean(&(applicationHandle.receiveFinished));
	free(watchedBooleanName);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

    if ((dataSendBuffer = malloc(requestDataSize + 
				 ARR_HEADER_SIZE + 40)) == NULL) {
	reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					   RttErrorReturnCode);
	reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
					     RttDependLuApplicationFName,
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
					     RttDependLuApplicationFName,
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
    splitLUTargetAddress(targetOctetString,
			 &aHostName,
			 &hostNameLen,
			 &aApplName,
			 &applNameLen,
			 &aModeName,
			 &modeNameLen,
			 IDofMySelf);
    /*  If any length is zero, then it failed */
    if (hostNameLen == 0) {
	reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
	reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
	delete_watched_boolean(&(applicationHandle.receiveFinished));
	free(watchedBooleanName);
	free(dataRespBuffer);
	free(dataSendBuffer);
	iAmPanicking(inRttMonCtrlAdminQItem,
		     IDofMySelf);
    }

    currentWakeInterval = 
	currentWakeInterval * 1000;	/* in seconds */
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
           
    sequenceNum = 1; /* the number 0 is going to be used for logoffs */

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
		    free(aApplName);
		    free(aModeName);

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
	    free(dataRespBuffer);
	    free(dataSendBuffer);
	    reg_invoke_rttMonFreeOctetStringBuffer(destOctetString);
	    reg_invoke_rttMonFreeOctetStringBuffer(targetOctetString);
	    delete_watched_boolean(&(applicationHandle.receiveFinished));
	    free(watchedBooleanName);
	    free(aHostName);
	    free(aApplName);
	    free(aModeName);

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
     * This is overkill, but we have a requirement to create
     *   a source-target path, for errors that have no
     *   path information!
     */
    setupPathInfo(inRttMonCtrlAdminQItem,
		  aHostName,
		  aApplName,
		  aModeName,
		  targetOctetString,
		  IDofMySelf);

    do {
	if (RTT_TRACE(IDofMySelf)) {
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttStartingAnEchoOperation);
	    reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
					       RttSettingReceiveFinishedFalse);
	    RTT_BUGINF3(IDofMySelf,
			"\t in %s\n", dependLuEchoApplication_FName);   
	}
	process_set_boolean(applicationHandle.receiveFinished, FALSE);
	GET_TIMESTAMP(total_time);
	TIMER_START(busy_timer, currentWakeInterval);
	openConnection(targetOctetString,
		       aHostName,
		       hostNameLen,
		       aApplName,
		       applNameLen,
		       aModeName,
		       modeNameLen,
		       &applicationHandle,
		       &sequenceNum);

	process_may_suspend();

	setupPathInfo(inRttMonCtrlAdminQItem,
		      aHostName,
		      aApplName,
		      aModeName,
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
			aApplName,
			aModeName,
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
						     RttDependLuApplicationFName,
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
			  aApplName,
			  aModeName,
			  targetOctetString,
			  IDofMySelf);

	    /* for number of times we exceeded currentWakeInterval */
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
							 RttDependLuApplicationFName,
							 RttMonUpdateResponseBusyName);
		    reg_invoke_dumpRttMonLibReturnCode(IDofMySelf,return_code);
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
					       RttSettingReceiveFinishedFalse);
	}
	
	/*
	 * If the receiveFinished has not been set (in either
	 *  callback), then the request to the host is still
	 *  outstanding.  The protocol says that a request/response
	 *  must be in a pair, and it cannot handle multiple
	 *  outstanding requests.
	 */
	/*
	 * Wait for the LU to finish its data exchange
	 *   Wait for atleast 15 seconds to clear.
	 */
	if (!process_get_boolean(applicationHandle.receiveFinished)) {
	    process_wait_for_event_timed((unsigned long)echoTimeout + 15000);
	}	
	if (process_get_boolean(applicationHandle.receiveFinished)) {
	    process_set_boolean(applicationHandle.receiveFinished, FALSE);
	    sendLogOff(dataRespBuffer,
		       dataSendBuffer,
		       echoTimeout,
		       &applicationHandle);
	}
	/*
	 * If the close Indication did not happen because of the
	 *    LogOff, then the LU is still open and we must
	 *    close it.
	 */
	if (applicationHandle.closeIndicationCalled == FALSE) {

	    if (RTT_TRACE(IDofMySelf)) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttCallingLuClose);
	    }
	    lu_return_code = 
		luClose(applicationHandle.
			currentLUHandle, 
			&returnedBuffer);
	    if (lu_return_code != Success) {
		reg_invoke_dumpRttMonProcessHeader(IDofMySelf,
						   RttErrorReturnCode);
		reg_invoke_dumpRttMonErrorReturnCode(IDofMySelf,
						     RttDependLuApplicationFName,
						     RttMonLuCloseName);
		rtt_dump_lu_return_code(lu_return_code,
					IDofMySelf);
	    }
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
    free(aApplName);
    free(aModeName);

    process_kill(THIS_PROCESS);

}

static char * const ProcessName1[] = {
    "LU0 RTR Probe 1",  "LU0 RTR Probe 2",
    "LU0 RTR Probe 3",  "LU0 RTR Probe 4",
    "LU0 RTR Probe 5",  "LU0 RTR Probe 6",
    "LU0 RTR Probe 7",  "LU0 RTR Probe 8",
    "LU0 RTR Probe 9",  "LU0 RTR Probe 10",
    "LU0 RTR Probe 11", "LU0 RTR Probe 12",
    "LU0 RTR Probe 13", "LU0 RTR Probe 14",
    "LU0 RTR Probe 15", "LU0 RTR Probe 16",
    "LU0 RTR Probe 17", "LU0 RTR Probe 18",
    "LU0 RTR Probe 19", "LU0 RTR Probe MaxName"
    };

void rtt_lu0_echo_activate (rttMonCtrlAdminQItem    
			    *inRttMonCtrlAdminQItem)
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
	process_create(&dependLuEchoApplication,
		       ProcessName1[entryNum],
		       LARGE_STACK,	/* dspu */
		       PRIO_LOW);

    if (inRttMonCtrlAdminQItem->pid != NO_PROCESS) {
	process_set_arg_ptr(inRttMonCtrlAdminQItem->pid,
			    inRttMonCtrlAdminQItem);   
    } else {
	reg_invoke_dumpRttMonProcessHeader(inRttMonCtrlAdminQItem->
					   ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   RttFailedToStartLu0EchoApplication);
	reg_invoke_rttMonProbePanicInternal(inRttMonCtrlAdminQItem);
    }

    return;
}

boolean rtt_lu0_echo_exists (void)
{
    return (TRUE);
}

void rtt_depend_lu_split_address (OctetString *target,
				  char **hostname,
				  long  *hostnamelen,
				  char **applname,
				  char **modename,
				  long   IDofMySelf)
{
    long                    applnamelen;
    long                    modenamelen;

    splitLUTargetAddress(target,
			 hostname,
			 hostnamelen,
			 applname,
			 &applnamelen,
			 modename,
			 &modenamelen,
			 IDofMySelf);
}
			    
int rtt_lu0_echo_validate (rttMonCtrlAdminEntry_t *ctrlAdminEntry, 
			   void *specificEntry,
			   int rttObject)
{
    rttMonEchoAdminEntry_t *echoAdminEntry;
    char                   *aHostName;
    long                    hostNameLen;
    char                   *aApplName;
    long                    applNameLen;
    char                   *aModeName;
    long                    modeNameLen;
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
	    ((echoAdminEntry->rttMonEchoAdminProtocol != 
	      D_rttMonEchoAdminProtocol_snaLU0EchoAppl) &&
	     (echoAdminEntry->rttMonEchoAdminProtocol != 
	      D_rttMonEchoAdminProtocol_snaLU2EchoAppl))) {
	    return (D_INVALID_RttEntry);
	}
	break;
    case D_Check_Address:
	splitLUTargetAddress(echoAdminEntry->rttMonEchoAdminTargetAddress,
			     &aHostName,
			     &hostNameLen,
			     &aApplName,
			     &applNameLen,
			     &aModeName,
			     &modeNameLen,
			     ctrlAdminEntry->rttMonCtrlAdminIndex);
	/*  If any length is zero, then it failed */
	if (hostNameLen == 0) {
	    free(aHostName);
	    free(aApplName);
	    free(aModeName);
	    return (D_INVALID_RttEntry);
	}

	if (((lu_api_return_code = luT0orT2QueryPu(aHostName)) != Success) && 
	    (lu_api_return_code != HostConnInactive)) {
	    free(aHostName);
	    free(aApplName);
	    free(aModeName);
	    return (D_INVALID_RttEntry);
	}

	free(aHostName);
	free(aApplName);
	free(aModeName);
    
	break;
    }

    return (D_VALID_RttEntry);
}

static char * const ProcessName2[] = {
    "LU2 RTR Probe 1",  "LU2 RTR Probe 2",
    "LU2 RTR Probe 3",  "LU2 RTR Probe 4",
    "LU2 RTR Probe 5",  "LU2 RTR Probe 6",
    "LU2 RTR Probe 7",  "LU2 RTR Probe 8",
    "LU2 RTR Probe 9",  "LU2 RTR Probe 10",
    "LU2 RTR Probe 11", "LU2 RTR Probe 12",
    "LU2 RTR Probe 13", "LU2 RTR Probe 14",
    "LU2 RTR Probe 15", "LU2 RTR Probe 16",
    "LU2 RTR Probe 17", "LU2 RTR Probe 18",
    "LU2 RTR Probe 19", "LU2 RTR Probe MaxName"
    };

void rtt_lu2_echo_activate (rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem)
{
    int entryNum;
  
    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlAdminIndex < 20) {
	entryNum = inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlAdminIndex - 1;
    } else {
	entryNum = 19;
    }
  
    inRttMonCtrlAdminQItem->pid =
	process_create(&dependLuEchoApplication,
		       ProcessName2[entryNum],
		       LARGE_STACK,	/* dspu */
		       PRIO_LOW);

    if (inRttMonCtrlAdminQItem->pid != NO_PROCESS) {
	process_set_arg_ptr(inRttMonCtrlAdminQItem->pid,
			    inRttMonCtrlAdminQItem);   
    } else {
	reg_invoke_dumpRttMonProcessHeader(inRttMonCtrlAdminQItem->
					   ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   RttFailedToStartLu2EchoApplication);
	reg_invoke_rttMonProbePanicInternal(inRttMonCtrlAdminQItem);
    }

    return;
}

boolean rtt_lu2_echo_exists (void)
{
    return (TRUE);
}

int rtt_lu2_echo_validate (rttMonCtrlAdminEntry_t *ctrlAdminEntry, 
			   void *specificEntry,
			   int rttObject)
{
    return (rtt_lu0_echo_validate(ctrlAdminEntry,
				  specificEntry,
				  rttObject));
}


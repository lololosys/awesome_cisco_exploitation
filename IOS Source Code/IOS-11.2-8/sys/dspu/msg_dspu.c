/* $Id: msg_dspu.c,v 3.3.10.2 1996/09/13 23:46:14 hampton Exp $
 * $Source: /release/112/cvs/Xsys/dspu/msg_dspu.c,v $
 *------------------------------------------------------------------
 * DSPU Run-time error messages.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This file declares and initializes all run-time error message structures.
 * If you add a new error message here, please update msg_dspu.h too.
 *------------------------------------------------------------------
 * $Log: msg_dspu.c,v $
 * Revision 3.3.10.2  1996/09/13  23:46:14  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.10.1  1996/08/28  12:42:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1996/02/26  17:33:05  ppearce
 * CSCdi49863:  DSPU reports info from CV 0x22 incorrectly
 *
 * Revision 3.2  1995/11/17  09:05:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/06  23:16:14  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.2  1995/06/18  21:45:21  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:29:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DSPU);
msgdef_section("Downstream physical unit error messages");


msgdef(CLSIBadLength, DSPU, LOG_ERR, 0,
       "CLSI bad length, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIBadPrimitiveType, DSPU, LOG_ERR, 0,
       "CLSI bad primitive type, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIBadPrimitiveCmd, DSPU, LOG_ERR, 0,
       "CLSI bad primitive command, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIBadIdentifierType, DSPU, LOG_ERR, 0,
       "CLSI bad identifier type, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIBadIdentifier, DSPU, LOG_ERR, 0,
       "CLSI bad identifier, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIBadReturnCode, DSPU, LOG_ERR, 0,
       "CLSI bad return code: %d, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIUnexpctActSapCnf, DSPU, LOG_ERR, 0,
       "CLSI unexpected ActivateSapCnf, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIUnexpctDeactSapCnf, DSPU, LOG_ERR, 0,
       "CLSI unexpected DeactivateSapCnf, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIUnexpctIdStnInd, DSPU, LOG_ERR, 0,
       "CLSI unexpected IdStnInd, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIUnexpctEnableCnf, DSPU, LOG_ERR, 0,
       "CLSI unexpected EnableCnf, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(CLSIBadAdptCorr, DSPU, LOG_ERR, 0,
       "CLSI bad Adaptor correlator, header= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(DSPU_BufferCorrupted, DSPU, LOG_ERR, 0,
       "DSPU could not free buffer - buffer corrupted");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(UnknownXid, DSPU, LOG_ERR, 0,
       "XID Format not supported, XID= %s");
msgdef_explanation(
	"Unsupported XID received from the remote station; connection
	with remote station could not be established.");
msgdef_recommended_action(
	"Configure the remote station to send supported XID (if
	possible). Supported XID formats are:

	XID Format 0 
	XID Format 3");
msgdef_ddts_component("");


msgdef(LSXIDNegotiationError, DSPU, LOG_ERR, 0,
       "XID Negotiation Error, bad byte= %d, bad bit= %d, sense data= %s");
msgdef_explanation(
	"XID3 with an error indication was received from the remote station;
	connection with the remote station could not be established.");
msgdef_recommended_action(
	"Determine, from the CV 22 byte or bit information, which field in the
	XID3 is rejected by the remote station, and correct the link station
	configuration parameters at either the router or the remote station.");
msgdef_ddts_component("");


msgdef(LSNoACTPUCapability, DSPU, LOG_ERR, 0,
       "Connection attempt failed from XID= %s, ACTPU not supported");
msgdef_explanation(
	"The remote station did not request PU 2.0 activation; connection with
	remote station is terminated.");
msgdef_recommended_action(
	"Correct link station configuration parameters at the remote station to
	request a system services control point (SSCP)-PU session with the
	router.");
msgdef_ddts_component("");


msgdef(LSInvalidMaxIFrame, DSPU, LOG_ERR, 0,
       "Connection attempt failed from XID= %s, Invalid MaxIFrame= %d");
msgdef_explanation(
	"An invalid MaxIFrame value in an exchange identification (XID) was
	received from the remote station; connection with remote station could
	not be established.");
msgdef_recommended_action(
	"Correct link station configuration at the remote station.");
msgdef_ddts_component("");


msgdef(LSInvalidWindowSize, DSPU, LOG_ERR, 0,
       "Connection attempt failed from XID= %s, Invalid WindowSize= %d");
msgdef_explanation(
	"An invalid WindowSize value in an exchange identification (XID) was
	received from the remote station; connection with remote station could
	not be established.");
msgdef_recommended_action(
	"Correct link station configuration at the remote station.");
msgdef_ddts_component("");


msgdef(LSConnOutToHostFailed, DSPU, LOG_ERR, 0,
       "Connect out to host %s failed.");
msgdef_explanation(
	"Connection with the remote host could not be established.");
msgdef_recommended_action(
	"Verify host configuration parameters at the router and at the host.
	Determine whether the host is active and responding to connection
	requests.");
msgdef_ddts_component("");


msgdef(LSConnOutToPuFailed, DSPU, LOG_ERR, 0,
       "Connect out to pu %s failed.");
msgdef_explanation(
	"Connection with the remote PU could not be established.");
msgdef_recommended_action(
	"Verify PU configuration parameters at the router and at the remote PU.
	Determine whether the remote PU is active and responding to connection
	requests.");
msgdef_ddts_component("");


msgdef(LSConnInFromHostFailed, DSPU, LOG_ERR, 0,
       "Connect in from host %s with remote address %s failed.");
msgdef_explanation(
	"Connection with the remote host could not be established.");
msgdef_recommended_action(
	"Verify host configuration parameters at the router and at the host.
	Determine whether the router is configured to accept connection from
	this host.");
msgdef_ddts_component("");


msgdef(LSConnInFromPuFailed, DSPU, LOG_ERR, 0,
       "Connect in from pu %s with remote address %s failed.");
msgdef_explanation(
	"Connection with the remote PU could not be established.");
msgdef_recommended_action(
	"Verify PU configuration parameters at the router and at the remote PU.
	Determine whether the router is configured to accept connectIn from
	this remote PU.");
msgdef_ddts_component("");


msgdef(LSConnInFailed, DSPU, LOG_ERR, 0,
       "Connect in from remote address %s failed.");
msgdef_explanation(
	"Connection with remote station could not be established.");
msgdef_recommended_action(
	"Verify configuration parameters at the router and at the remote
	station. Determine whether the router is configured to accept connectIn
	from this remote station.");
msgdef_ddts_component("");


msgdef(LSConnInFailedNoMem, DSPU, LOG_ERR, 0,
       "Connect in from remote address %s failed; insufficient memory.");
msgdef_explanation(
	"Connection with the remote station could not be established because of
	insufficient memory in the router.");
msgdef_recommended_action(
	"Correct low memory problem in the router. If the problem persists,
	call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(SnaBadData, DSPU, LOG_ERR, 0,
       "Bad SNA data received from PU %s, data= %s");
msgdef_explanation(
	"Unrecognized SNA data was received from the remote PU.");
msgdef_recommended_action(
	"Correct the problem at the remote PU. If the problem persists, call
	your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(ActpuNegResponse, DSPU, LOG_ERR, 0,
       "ACTPU Neg rsp received from PU %s");
msgdef_explanation(
	"Remote PU rejected the router's request for an SSCP-PU session.");
msgdef_recommended_action(
	"Verify PU configuration parameters at the router and at the remote PU.
	Determine whether the problem exists at the remote PU, and correct it.
	If the problem persists, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(NotifyNegRsp, DSPU, LOG_ERR, 0,
       "LU %d received a Notify Neg Resp from host %s");
msgdef_explanation(
	"Remote host rejected the router's request to activate/deactivate an
	SSCP-LU session.");
msgdef_recommended_action(
	"Verify host configuration parameters at the router and at the remote
	host. Determine whether the problem exists at the remote host and
	correct it. If the problem persists, call your technical support
	representative for assistance.");
msgdef_ddts_component("");

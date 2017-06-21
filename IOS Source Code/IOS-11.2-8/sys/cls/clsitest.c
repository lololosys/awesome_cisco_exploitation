/* $Id: clsitest.c,v 3.2.60.2 1996/05/17 10:42:12 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/clsitest.c,v $
 *------------------------------------------------------------------
 * CLSI test program file
 *
 * 12-September-1994	Steve Chiang
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains all functions used by clsitest.  It takes
 * commands from users from console and constructs CLS primitives
 * (Requests & Responses) then sends them to CLS.  It also gets 
 * Confirms and Indications from CLS and prints them on console.
 *
 *------------------------------------------------------------------
 * $Log: clsitest.c,v $
 * Revision 3.2.60.2  1996/05/17  10:42:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  01:52:50  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:54:25  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:10:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:41:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:41:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:55:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:53:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/11  04:20:11  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.3  1995/07/02  05:15:27  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.2  1995/06/09  12:59:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:20:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "address.h"
#include "interface_private.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "config.h"
#include "ieee.h"
#include "clsitest.h"

/*
 * Events which awaken the CLSItest Process
 */

#define CLSI_TEST_QUEUE "CLSI test queue"
#define CLSI_TEST_SHUT "CLSI test shutdown flag"

/* 
 *  minor codes for CLSItest events 
 */
typedef enum clsitest_events_ {
    CLSI_TEST_QUEUE_EVENT,
    CLSI_TEST_SHUT_EVENT
} clsitest_events;  

TCLSIQInit *clsitest_qinit_p;
TCLSIQueue *ClsiReadQ;
TCLSIQueue *ClsiWriteQ;
watched_boolean *ClsiTestShut;  /*Set TRUE to shut down test*/
boolean watch_ok = FALSE;       /*ClsiTestShut is created*/
u_char clsitest_debug = 1;
watched_queue  *ClsiInputQueue;
int reserveLengthType1;	/* from ACTIVATE_SAP, for ID_REQ & ID_RSP */
int reserveLengthType2;	/* from ACTIVATE_SAP, for ID_REQ & ID_RSP */
int p_sap_id;
int p_cep_id;



int
stoh(char *char_num, char *hex_num)
{
	int i;

	*hex_num = 0;

	for (i=0; i<2; i++) {
		*hex_num = *hex_num << 4;
		switch(char_num[i]) {
		case '0': break;
		case '1': *hex_num |= 1; break;
		case '2': *hex_num |= 2; break;
		case '3': *hex_num |= 3; break;
		case '4': *hex_num |= 4; break;
		case '5': *hex_num |= 5; break;
		case '6': *hex_num |= 6; break;
		case '7': *hex_num |= 7; break;
		case '8': *hex_num |= 8; break;
		case '9': *hex_num |= 9; break;
		case 'a': case 'A':  *hex_num |= 0xa; break;
		case 'b': case 'B':  *hex_num |= 0xb; break;
		case 'c': case 'C':  *hex_num |= 0xc; break;
		case 'd': case 'D':  *hex_num |= 0xd; break;
		case 'e': case 'E':  *hex_num |= 0xe; break;
		case 'f': case 'F':  *hex_num |= 0xf; break;
		default: buginf("\nInvalid hex value %c(0x%x)", char_num[i], char_num[i]);
		return(ERR);
		}
	}
	return(OK);
}


/*
 * REQUEST Primitives
 */

int
enable_req (parseinfo *csb)
{
	uint parmSize;
	EnableReq_t*   enableReq;
	CLSDLCType_e	type;

	type = GETOBJ(int,3);
	switch (type) {	/* grab param size for CreateHeader() below */

	case CLS_RSRB: 
	        parmSize = sizeof(EnableRSRBParms_t);
		break;

	case CLS_TBRIDGE:
		parmSize = sizeof(EnableTBRIDGEParms_t);
		break;

	case CLS_LLC: 
		parmSize = sizeof(EnableTRParms_t); 
		break;

	default:
		return (ERR);
	}
	enableReq = (EnableReq_t*) CLSIMsgCreateHeader(CLS_ENABLE | CLS_REQUEST,
		offsetof(EnableReq_t,fParms) - CLS_ENABLE_REQ_HDR_SZ + parmSize,
		CLS_LLC_LAYER,
		CLS_ENTITY_MGR,
		0);
    
	if (!enableReq)
		return (ERR);

        enableReq->fDLCType   = GETOBJ(int,3);
	enableReq->fCorrelator = GETOBJ(int,4);

	switch (enableReq->fDLCType) {	
        /* initialize port dependent header fields */
	case CLS_LLC: {
		EnableTRParms_t* trParms;

		trParms = (EnableTRParms_t*) &(enableReq->fParms);
		trParms->fVersion = CLSI_VERSION;

		/* Use defaults for everything, for now. */
		memset(&trParms->fLocalMac, 0, sizeof(hwaddrtype));
		trParms->fTrSpeed = 0;
		trParms->fMaxIFieldLength = 0;
		break;
		}

	case CLS_RSRB:
	case CLS_TBRIDGE:
	default:
		return (ERR);
	}
    
	if (clsitest_debug) {
		PrintClsiHdr(enableReq);
#ifdef CLS_NOT
		PrintPortNumber(enableReq);
#endif
		PrintPortType(enableReq);
		PrintCorrelator(enableReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) enableReq);

	return (OK);
}



int
disable_req(parseinfo *csb)
{
	DisableReq_t*   disableReq;

	disableReq = (DisableReq_t*) CLSIMsgCreateHeader(CLS_DISABLE | CLS_REQUEST,
		sizeof(DisableReq_t) - CLS_DISABLE_REQ_HDR_SZ,
		CLS_LLC_LAYER,
		CLS_ENTITY_MGR,
		0);
    
	if (!disableReq)
		return(ERR);

/* CLEAN UP LATER - PHIL 9/15/95 */
#ifdef CLS_NOT
        disableReq->fPortId.fTypeInfo.port_num.status = IDB_TR;
        disableReq->fPortId.fTypeInfo.port_num.unit =GETOBJ(int,2);
        disableReq->fPortId.fTypeInfo.port_num.slot = 0;
        disableReq->fPortId.fTypeInfo.port_num.sub_number = 0;
        disableReq->fPortId.type = CLS_P_TYPE_NUMBERS;
        disableReq->fDLCType   = GETOBJ(int,3);
        disableReq->fCorrelator = GETOBJ(int,4);
#endif

	if (clsitest_debug) {
		PrintClsiHdr(disableReq);
#ifdef CLS_NOT
		PrintPortNumber(disableReq);
#endif
		PrintPortType(disableReq);
		PrintCorrelator(disableReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) disableReq);
	return (OK);
}



int
activate_sap_req(parseinfo *csb)
{
	ActSapReq_t*   ActSapReq;
	ActSapLLCParms_t *ActSapLLCParms;

	ActSapReq = (ActSapReq_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_SAP | CLS_REQUEST,
		offsetof(ActSapReq_t,fParms) - CLS_ACT_SAP_REQ_HDR_SZ + sizeof(ActSapLLCParms_t),
		CLS_LLC_LAYER,
		CLS_ENTITY_MGR,
		0);
    
	if (!ActSapReq)
		return (ERR);

	ActSapReq->fCLSIHdr.fUSapId	= GETOBJ(int,16);
	ActSapReq->fVersion		= CLSI_VERSION;
	ActSapReq->fCorrelator		= GETOBJ(int,4);

/* CLEAN UP LATER - PHIL 9/15/95 */
#ifdef CLS_NOT
        ActSapReq->fPortId.fTypeInfo.port_num.status = IDB_TR;
        ActSapReq->fPortId.fTypeInfo.port_num.unit =GETOBJ(int,2);
        ActSapReq->fPortId.fTypeInfo.port_num.slot = 0;
        ActSapReq->fPortId.fTypeInfo.port_num.sub_number = 0;
        ActSapReq->fPortId.type = CLS_P_TYPE_NUMBERS;
        ActSapReq->fDLCType             = GETOBJ(int,3);
#endif

	ActSapLLCParms = (ActSapLLCParms_t *) & (ActSapReq->fParms);
	ActSapLLCParms->fSapValue	= GETOBJ(int,5);
	ActSapLLCParms->fUserClass	= GETOBJ(int,17);
	ActSapLLCParms->fSapFlags	= GETOBJ(int,18);
	ActSapLLCParms->fT1		= GETOBJ(int,6);
	ActSapLLCParms->fT2		= GETOBJ(int,7);
	ActSapLLCParms->fTi		= GETOBJ(int,8);
	ActSapLLCParms->fMaxOut		= GETOBJ(int,9);
	ActSapLLCParms->fMaxRetryCount	= GETOBJ(int,10);
	ActSapLLCParms->fMaxIn		= GETOBJ(int,11);
	ActSapLLCParms->fDynamicWindow	= 0;
	ActSapLLCParms->fMaxOutIncr	= GETOBJ(int,12);
	ActSapLLCParms->fMaxIField	= GETOBJ(int,13);
	ActSapLLCParms->fXIDRetries	= GETOBJ(int,14);
	ActSapLLCParms->fXIDTimer	= GETOBJ(int,15);

	if (clsitest_debug) {
		PrintClsiHdr(ActSapReq);
		PrintSapLLcParms(ActSapReq, ActSapLLCParms);
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) ActSapReq);

	return (OK);
}



int
deactivate_sap_req(parseinfo *csb)
{
	DeactSapReq_t*   DeactSapReq;

	DeactSapReq = (DeactSapReq_t *) CLSIMsgCreateHeader(CLS_DEACTIVATE_SAP | CLS_REQUEST,
		sizeof(DeactSapReq_t) - CLS_DEACT_SAP_REQ_HDR_SZ,
		CLS_LLC_LAYER,
		CLS_P_SAP_ID,
		p_sap_id);
    
	if (!DeactSapReq)
		return (ERR);

	DeactSapReq->fCLSIHdr.fUSapId	= GETOBJ(int,2);
	DeactSapReq->fVersion		= CLSI_VERSION;
	DeactSapReq->fCorrelator	= GETOBJ(int,3);

	if (clsitest_debug) {
		PrintClsiHdr(DeactSapReq);
		PrintUSapId(DeactSapReq);
		PrintVersion(DeactSapReq);
		PrintCorrelator(DeactSapReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) DeactSapReq);

	return (OK);
}



int
clsi_flow_req(parseinfo *csb)
{
	FlowReq_t*   FlowReq;

	FlowReq = (FlowReq_t*) CLSIMsgCreateHeader(CLS_FLOW | CLS_REQUEST,
		sizeof(FlowReq_t) - CLS_FLOW_REQ_HDR_SZ,
		CLS_LLC_LAYER,
		CLS_P_CEP_ID,
		p_cep_id);
    
	if (!FlowReq)
		return(ERR);

	FlowReq->fCLSIHdr.fFlowAction = GETOBJ(int,2);
	FlowReq->fCorrelator = GETOBJ(int,3);

	if (clsitest_debug) {
		PrintClsiHdr(FlowReq);
		PrintFlowReq(FlowReq);
		PrintCorrelator(FlowReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) FlowReq);
	return (OK);
}



int 
open_stn_req(parseinfo *csb)
{
	OpenStnReq_t*   OpenStnReq;

	OpenStnReq = (OpenStnReq_t *) CLSIMsgCreateHeader(CLS_OPEN_STN | CLS_REQUEST,
		sizeof(OpenStnReq_t) - CLS_OPEN_STN_REQ_HDR_SZ,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!OpenStnReq)
		return (ERR);

	OpenStnReq->fCLSIHdr.fUCepId	= GETOBJ(int,2);
	OpenStnReq->fVersion		= CLSI_VERSION;
	OpenStnReq->fCorrelator		= GETOBJ(int,3);

	if (clsitest_debug) {
		PrintClsiHdr(OpenStnReq);
		PrintUCepId(OpenStnReq);
		PrintCorrelator(OpenStnReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) OpenStnReq);

	return (OK);
}



int 
req_opnstn_req(parseinfo *csb)
{
	int i;
	int rif_len = GETOBJ(int,21);
	ReqOpnStnReq_t*   ReqOpnStnReq;
	ReqOpnStnLLCParms_t *ReqOpnStnLLCParms;

	ReqOpnStnReq = (ReqOpnStnReq_t*) CLSIMsgCreateHeader(
		CLS_REQ_OPNSTN | CLS_REQUEST, offsetof(ReqOpnStnReq_t,fParms) 
		- CLS_REQ_OPNSTN_REQ_HDR_SZ + sizeof(ReqOpnStnLLCParms_t) + 
		rif_len, CLS_LLC_LAYER, CLS_P_SAP_ID, p_sap_id);
    
	if (!ReqOpnStnReq)
		return (ERR);

	ReqOpnStnReq->fCLSIHdr.fPCepId		= CLS_NO_CEP;
	ReqOpnStnReq->fCLSIHdr.fUCepId		= GETOBJ(int,2);
	ReqOpnStnReq->fVersion			= CLSI_VERSION;
	ReqOpnStnReq->fCorrelator		= GETOBJ(int,4);

	ReqOpnStnLLCParms = (ReqOpnStnLLCParms_t *) & (ReqOpnStnReq->fParms);
	memcpy((char *) & (ReqOpnStnLLCParms->fDestAddr), 
	       (char *)&(*GETOBJ(hwaddr,1)), 
	       sizeof(hwaddrtype));
	memcpy((char *) & (ReqOpnStnLLCParms->fSrcAddr), 
	       (char *)&(*GETOBJ(hwaddr,2)), 
	       sizeof(hwaddrtype));
	ReqOpnStnLLCParms->fDestSap		= GETOBJ(int,5);
	ReqOpnStnLLCParms->fSrcSap		= GETOBJ(int,6);
	ReqOpnStnLLCParms->fT1			= GETOBJ(int,7);
	ReqOpnStnLLCParms->fT2			= GETOBJ(int,8);
	ReqOpnStnLLCParms->fTi			= GETOBJ(int,9);
	ReqOpnStnLLCParms->fMaxOut		= GETOBJ(int,10);
	ReqOpnStnLLCParms->fMaxRetryCount	= GETOBJ(int,11);
	ReqOpnStnLLCParms->fMaxIn		= GETOBJ(int,12);
	ReqOpnStnLLCParms->fDynamicWindow	= GETOBJ(int,17);
	ReqOpnStnLLCParms->fMaxOutIncr		= GETOBJ(int,13);
	ReqOpnStnLLCParms->fMaxIField		= GETOBJ(int,14);
	ReqOpnStnLLCParms->fXIDRetries		= GETOBJ(int,15);
	ReqOpnStnLLCParms->fXIDTimer		= GETOBJ(int,16);
	ReqOpnStnLLCParms->fExplorerOption	= GETOBJ(int,18);
	ReqOpnStnLLCParms->fExplorerRetries	= GETOBJ(int,19);
	ReqOpnStnLLCParms->fExplorerTimer	= GETOBJ(int,20);
	ReqOpnStnLLCParms->fRifLength		= GETOBJ(int,21);
	rif_len *= 2;
	if (strlen(GETOBJ(string, 1)) != rif_len) {
		buginf("\nInvalid length rif: should be %d instead %d\n",
			rif_len, strlen(GETOBJ(string, 1)));
		return(ERR);
	}
	for (i=0; i<rif_len; i += 2)
		if (stoh(&(GETOBJ(string, 1)[i]), 
			&(ReqOpnStnLLCParms->fRif[i/2])) == ERR)
			return(ERR);

	if (clsitest_debug) {
		PrintClsiHdr(ReqOpnStnReq);
		PrintUCepId(ReqOpnStnReq);
		PrintPCepId(ReqOpnStnReq);
		PrintVersion(ReqOpnStnReq);
		PrintCorrelator(ReqOpnStnReq);
		PrintReqOpnStnLLCParms(ReqOpnStnLLCParms);
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) ReqOpnStnReq);

	return (OK);
}



int 
close_stn_req(parseinfo *csb)
{
	CloseStnReq_t*   CloseStnReq;

	CloseStnReq = (CloseStnReq_t *) CLSIMsgCreateHeader(CLS_CLOSE_STN | CLS_REQUEST,
		sizeof(CloseStnReq_t) - CLS_CLOSE_STN_REQ_HDR_SZ,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!CloseStnReq)
		return (ERR);

	CloseStnReq->fCLSIHdr.fUCepId		= GETOBJ(int,2);
	CloseStnReq->fCLSIHdr.fActionCode	= GETOBJ(int,3);
	CloseStnReq->fVersion			= CLSI_VERSION;
	CloseStnReq->fCorrelator		= GETOBJ(int,4);

	if (clsitest_debug) {
		PrintClsiHdr(CloseStnReq);
		PrintUCepId(CloseStnReq);
		PrintActionCode(CloseStnReq);
		PrintVersion(CloseStnReq);
		PrintCorrelator(CloseStnReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) CloseStnReq);

	return (OK);
}



int
connect_req(parseinfo *csb)
{
    int i;
    ConnectReq_t*   ConnectReq;
    ReqOpnStnLLCParms_t *ConnectReqLLCParms;
    int flag_len;
    int rif_len = GETOBJ(int,19);
    
    ConnectReq = 
	(ConnectReq_t *) CLSIMsgCreateHeader(CLS_CONNECT | CLS_REQUEST,
		offsetof(ConnectReq_t,fParms) - CLS_CONNECT_REQ_HDR_SZ + 
		sizeof(ReqOpnStnLLCParms_t) + rif_len,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
    if (!ConnectReq)
	return (ERR);

    flag_len = strlen(GETOBJ(string, 1));
    if ((flag_len != 6) && (flag_len != 0)) {
	buginf("\nInvalid ModifyFlags");
	return(ERR);
    }
#ifdef NOTDEF
    if (flag_len == 0)
	/* default */
	memset((char *) &(ConnectReq->fCLSIHdr.fModifyFlags), 0, 3);
    else
	for (i=0; i<flag_len; i += 2)
	    if (stoh(&(GETOBJ(string, 1)[i]), 
		     &(ConnectReq->fCLSIHdr.fModifyFlags[i/2])) == ERR)
		return(ERR);
#endif

    ConnectReq->fVersion		= CLSI_VERSION;
    ConnectReq->fCorrelator		= GETOBJ(int,2);
    
    ConnectReqLLCParms = (ReqOpnStnLLCParms_t *) & (ConnectReq->fParms);
    memcpy((char *) & (ConnectReqLLCParms->fDestAddr), 
	   (char *)&(*GETOBJ(hwaddr,1)), 
	   sizeof(hwaddrtype));
    memcpy((char *) & (ConnectReqLLCParms->fSrcAddr), 
	   (char *)&(*GETOBJ(hwaddr,2)), 
	   sizeof(hwaddrtype));
    ConnectReqLLCParms->fDestSap		= GETOBJ(int,3);
    ConnectReqLLCParms->fSrcSap		= GETOBJ(int,4);
    ConnectReqLLCParms->fT1			= GETOBJ(int,5);
    ConnectReqLLCParms->fT2			= GETOBJ(int,6);
    ConnectReqLLCParms->fTi			= GETOBJ(int,7);
    ConnectReqLLCParms->fMaxOut		= GETOBJ(int,8);
    ConnectReqLLCParms->fMaxRetryCount	= GETOBJ(int,9);
    ConnectReqLLCParms->fMaxIn		= GETOBJ(int,10);
    ConnectReqLLCParms->fDynamicWindow	= GETOBJ(int,15);
    ConnectReqLLCParms->fMaxOutIncr		= GETOBJ(int,11);
    ConnectReqLLCParms->fMaxIField		= GETOBJ(int,12);
    ConnectReqLLCParms->fXIDRetries		= GETOBJ(int,13);
    ConnectReqLLCParms->fXIDTimer		= GETOBJ(int,14);
    ConnectReqLLCParms->fExplorerOption	= GETOBJ(int,16);
    ConnectReqLLCParms->fExplorerRetries	= GETOBJ(int,17);
    ConnectReqLLCParms->fExplorerTimer	= GETOBJ(int,18);
    ConnectReqLLCParms->fRifLength		= GETOBJ(int,19);
    
    rif_len *= 2;
    if (strlen(GETOBJ(string, 2)) != rif_len) {
	buginf("\nInvalid length rif: should be %d instead %d\n",
	       rif_len, strlen(GETOBJ(string, 1)));
	return(ERR);
    }
    for (i=0; i<rif_len; i += 2)
	if (stoh(&(GETOBJ(string, 2)[i]), 
		 &(ConnectReqLLCParms->fRif[i/2])) == ERR)
	    return(ERR);

#ifdef NOTDEF
    if (clsitest_debug) {
	PrintClsiHdr(ConnectReq);
	PrintModifyFlags(ConnectReq);
	PrintVersion(ConnectReq);
	PrintCorrelator(ConnectReq);
	PrintReqOpnStnLLCParms(ConnectReqLLCParms);
	NewLine;
    }
#endif
    CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) ConnectReq);
    
    return (OK);
}



int
disconnect_req(parseinfo *csb)
{
	DisconnectReq_t*   DisconnectReq;

	DisconnectReq = (DisconnectReq_t *) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_REQUEST,
		sizeof(DisconnectReq_t) - CLS_DISCONNECT_REQ_HDR_SZ,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!DisconnectReq)
		return (ERR);

	DisconnectReq->fVersion		= CLSI_VERSION;
	DisconnectReq->fCorrelator	= GETOBJ(int,2);

	if (clsitest_debug) {
		PrintClsiHdr(DisconnectReq);
		PrintVersion(DisconnectReq);
		PrintCorrelator(DisconnectReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) DisconnectReq);

	return (OK);
}



int
id_req(parseinfo *csb)
{
	IdReq_t*   IdReq;
	int	id_len;
	char	*xid;
	int	i;
	u_char	*data;

	id_len = strlen(GETOBJ(string,1));
	if (id_len %2) {
		buginf("\nInvalid XID: must be even number byte of string!");
		return(ERR);
	}
	/*
	buginf("\nCommand Type: %d", sizeof (CLSPrimCodeCommandType_e));
	buginf("\nTCLSIMsg: %d", sizeof (struct TCLSIMsg));
	buginf("\nTCLSICnfMsg: %d", sizeof (TCLSICnfMsg));
	buginf("\nIdReqh_t: %d", sizeof (IdReqh_t));
	buginf("\nIdReq_t: %d", sizeof (struct IdReq_t_));
	*/
	id_len /= 2;

	IdReq = (IdReq_t *) CLSIMsgCreateHeader(CLS_ID | CLS_REQUEST,
		offsetof(IdReq_t,fXID) - CLS_ID_REQ_HDR_SZ + reserveLengthType1 
		+ id_len, CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!IdReq)
		return (ERR);

	IdReq->fVersion		= CLSI_VERSION;
	IdReq->fCorrelator	= GETOBJ(int,2);
	/*
	IdReq->fReserveLength	= GETOBJ(int,3);
	*/
	IdReq->fReserveLength	= reserveLengthType1;

	xid = IdReq->fXID + IdReq->fReserveLength;
	for (i=0; i<id_len*2; i += 2)
		if (stoh(&(GETOBJ(string, 1)[i]), &(xid[i/2])) == ERR)
			return(ERR);

	if (clsitest_debug) {
		PrintClsiHdr(IdReq);
		PrintVersion(IdReq);
		PrintCorrelator(IdReq);
		PrintReserveLength(IdReq);
		PrintXID(IdReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) IdReq);

	return (OK);
}



int
signal_stn_req(parseinfo *csb)
{
	SignalStnReq_t*   SignalStnReq;

	SignalStnReq = (SignalStnReq_t *) CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
		sizeof(SignalStnReq_t) - CLS_SIGNAL_STN_REQ_HDR_SZ,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!SignalStnReq)
		return (ERR);

	SignalStnReq->fCLSIHdr.fActionCode	= GETOBJ(int,2);
	SignalStnReq->fVersion			= CLSI_VERSION;
	SignalStnReq->fCorrelator		= GETOBJ(int,3);

	if (clsitest_debug) {
		PrintClsiHdr(SignalStnReq);
		PrintActionCode(SignalStnReq);
		PrintVersion(SignalStnReq);
		PrintCorrelator(SignalStnReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) SignalStnReq);

	return (OK);
}



int
data_req(parseinfo *csb)
{
	DataReq_t*   DataReq;
	int	data_len;

	data_len = GETOBJ(int,4);
	DataReq = (DataReq_t *) CLSIMsgCreateHeader(CLS_DATA | CLS_REQUEST,
		offsetof(DataReq_t,fData) - CLS_DATA_REQ_HDR_SZ + data_len + 
		reserveLengthType2, CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!DataReq)
		return (ERR);

	DataReq->fVersion		= CLSI_VERSION;
	DataReq->fCorrelator		= GETOBJ(int,2);
	DataReq->fReserveLength		= reserveLengthType2;
	memset((char *) &(DataReq->fData), 0x12, data_len);

	if (clsitest_debug) {
		PrintClsiHdr(DataReq);
		PrintVersion(DataReq);
		PrintCorrelator(DataReq);
		PrintReserveLength(DataReq);
		PrintDataLen(DataReq);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) DataReq);

	return (OK);
}



/*
 * CONFIRM Primitives
 */

void
show_enable_cnf(TCLSIMsg *cls_msg)
{
	EnableCnf_t *EnableCnf = (EnableCnf_t *) cls_msg;

	buginf("\nPrimitive: Enable Confirm");
	PrintStatusCode(EnableCnf);
	if (clsitest_debug) {
		PrintClsiHdr(EnableCnf);
		PrintStatus(EnableCnf);
#ifdef CLS_NOT
		PrintPortNumber(EnableCnf);
#endif
		PrintPortType(EnableCnf);
		PrintCorrelator(EnableCnf);
	}

	switch (EnableCnf->fDLCType) {
	case CLS_LLC:
		/* The 68000 is halfword alignment.  We have to
		count the filler in structure to get correct size */
		if ((EnableCnf->fCLSIHdr.fDataSize + 1 /* Filler */ - 
		(sizeof(EnableCnf_t) - sizeof(EnableCnfh_t) - 1 )) <
		sizeof(EnableTRParms_t))
			buginf("\nData size too small");
		else {
			EnableTRParms_t *TR;

			TR = (EnableTRParms_t *) &(EnableCnf->fParms);
#ifdef NOTDEF
			if (clsitest_debug) {
				int i;
				PrintVersion(TR);
				PrintLocalMac(TR);
				PrintTrSpeed(TR);
				PrintMaxIFieldLength(TR);
				NewLine;
			}
#endif
		}
		break;

	case CLS_RSRB:
	case CLS_TBRIDGE:
#if 0
		if ((EnableCnf->fCLSIHdr.fDataSize - 
		(sizeof(EnableCnf_t) - sizeof(byte) - sizeof(CLSI_HEADER))) <
		sizeof(EnableRSRBParms_t))
			buginf("Data size too small\n");
		else {
			EnableRSRBParms_t *RSRB;
			int i;

			RSRB = (EnableRSRBParms_t *) EnableCnf->fParms;
			if (clsitest_debug) {
				PrintVersion(RSRB);
				PrintMacaddr(RSRB);
				PrintRsrbOptionFlags(RSRB);
				PrintMaxIFieldLength(RSRB);
				PrintLocalRing(RSRB);
				PrintBridgeNumber(RSRB);
				PrintTargetRing(RSRB);
				NewLine;
			}
		}
		break;
#endif
	case CLS_SDLC:
	case CLS_QLLC:
	case CLS_VDLC:
	case CLS_LOCAL:
	case CLS_FRAME_RELAY:
        case CLS_ATM_1483:
	default:
		buginf("\n\tUnsupported Port Type: 0x%x\n", EnableCnf->fDLCType);
		break;
	}
}



void
show_disable_cnf(TCLSIMsg *cls_msg)
{
	DisableCnf_t *DisableCnf = (DisableCnf_t *) cls_msg;

	buginf("\nPrimitive: Disable Confirm");
	PrintStatusCode(DisableCnf);
	if (clsitest_debug) {
		PrintClsiHdr(DisableCnf);
		PrintStatus(DisableCnf);
#ifdef CLS_NOT
		PrintPortNumber(DisableCnf);
#endif
		PrintPortType(DisableCnf);
		PrintCorrelator(DisableCnf);
		NewLine;
	}
}



void
show_activate_sap_cnf(TCLSIMsg *cls_msg)
{
	ActSapCnf_t *ActSapCnf = (ActSapCnf_t *) cls_msg;

	reserveLengthType1 = ActSapCnf->fType1Length;
	reserveLengthType2 = ActSapCnf->fType2Length;

	if (ActSapCnf->fCLSIHdr.fStatus.fRetCode == CLS_OK)
		p_sap_id = ActSapCnf->fCLSIHdr.fPSapId;

	buginf("\nPrimitive: Activate SAP Confirm");
	PrintStatusCode(ActSapCnf);
	if (clsitest_debug) {
		PrintClsiHdr(ActSapCnf);
		PrintStatus(ActSapCnf);
		PrintPSapId(ActSapCnf);
		PrintVersion(ActSapCnf);
		PrintCorrelator(ActSapCnf);
#ifdef CLS_NOT
		PrintPortNumber(ActSapCnf);
#endif
		PrintPortType(ActSapCnf);
		PrintType1Length(ActSapCnf);
		PrintType2Length(ActSapCnf);
		NewLine;
	}
}



void
show_deactivate_sap_cnf(TCLSIMsg *cls_msg)
{
	DeactSapCnf_t *DeactSapCnf = (DeactSapCnf_t *) cls_msg;

	buginf("\nPrimitive: Deactivate SAP Confirm");
	PrintStatusCode(DeactSapCnf);
	if (clsitest_debug) {
		PrintClsiHdr(DeactSapCnf);
		PrintStatus(DeactSapCnf);
		PrintVersion(DeactSapCnf);
		PrintCorrelator(DeactSapCnf);
		NewLine;
	}
}



void 
show_open_stn_cnf(TCLSIMsg *cls_msg)
{
	OpenStnCnf_t *OpenStnCnf = (OpenStnCnf_t *) cls_msg;

	buginf("\nPrimitive: Open Stn Confirm");
	PrintStatusCode(OpenStnCnf);
	if (clsitest_debug) {
		PrintClsiHdr(OpenStnCnf);
		PrintStatus(OpenStnCnf);
		PrintPCepId(OpenStnCnf);
		PrintVersion(OpenStnCnf);
		PrintCorrelator(OpenStnCnf);
		NewLine;
	}
}



void 
show_req_opnstn_cnf(TCLSIMsg *cls_msg)
{
	ReqOpnStnCnf_t *ReqOpnStnCnf = (ReqOpnStnCnf_t *) cls_msg;

	p_cep_id = ReqOpnStnCnf->fCLSIHdr.fPCepId;

	buginf("\nPrimitive: Req Opnstn Confirm");
	PrintStatusCode(ReqOpnStnCnf);
	if (clsitest_debug) {
		PrintClsiHdr(ReqOpnStnCnf);
		PrintStatus(ReqOpnStnCnf);
		PrintPCepId(ReqOpnStnCnf);
		PrintVersion(ReqOpnStnCnf);
		PrintCorrelator(ReqOpnStnCnf);
		NewLine;
	}
}



void 
show_close_stn_cnf(TCLSIMsg *cls_msg)
{
	CloseStnCnf_t *CloseStnCnf = (CloseStnCnf_t *) cls_msg;

	buginf("\nPrimitive: Close Stn Confirm");
	PrintStatusCode(CloseStnCnf);
	if (clsitest_debug) {
		PrintClsiHdr(CloseStnCnf);
		PrintStatus(CloseStnCnf);
		PrintVersion(CloseStnCnf);
		PrintCorrelator(CloseStnCnf);
		NewLine;
	}
}



void 
show_connect_cnf(TCLSIMsg *cls_msg)
{
	ConnectCnf_t *ConnectCnf = (ConnectCnf_t *) cls_msg;

	buginf("\nPrimitive: Connect Confirm");
	PrintStatusCode(ConnectCnf);
	if (clsitest_debug) {
		PrintClsiHdr(ConnectCnf);
		PrintStatus(ConnectCnf);
		PrintVersion(ConnectCnf);
		PrintCorrelator(ConnectCnf);
		NewLine;
	}
}



void 
show_disconnect_cnf(TCLSIMsg *cls_msg)
{
	DisconnectCnf_t *DisconnectCnf = (DisconnectCnf_t *) cls_msg;

	buginf("\nPrimitive: Disconnect Confirm");
	if (clsitest_debug) {
		PrintClsiHdr(DisconnectCnf);
		PrintVersion(DisconnectCnf);
		PrintCorrelator(DisconnectCnf);
		NewLine;
	}
}



void 
show_id_cnf(TCLSIMsg *cls_msg)
{
	IdCnf_t *IdCnf = (IdCnf_t *) cls_msg;
	int i;
	char *data;

	buginf("\nPrimitive: Id Confirm");
	PrintStatusCode(IdCnf);
	if (clsitest_debug) {
		PrintClsiHdr(IdCnf);
		PrintStatus(IdCnf);
		PrintVersion(IdCnf);
		PrintCorrelator(IdCnf);
		PrintReserveLength(IdCnf);
		PrintXID1(IdCnf);
		NewLine;
	}
}



void 
show_signal_stn_cnf(TCLSIMsg *cls_msg)
{
	SignalStnCnf_t *SignalStnCnf = (SignalStnCnf_t *) cls_msg;

	buginf("\nPrimitive: Signal Stn Confirm");
	if (clsitest_debug) {
		PrintClsiHdr(SignalStnCnf);
		PrintVersion(SignalStnCnf);
		PrintCorrelator(SignalStnCnf);
		NewLine;
	}
}



void 
show_data_cnf(TCLSIMsg *cls_msg)
{
	DataCnf_t *DataCnf = (DataCnf_t *) cls_msg;

	buginf("\nPrimitive: Data Confirm");
	PrintStatusCode(DataCnf);
	if (clsitest_debug) {
		PrintClsiHdr(DataCnf);
		PrintStatus(DataCnf);
		PrintVersion(DataCnf);
		PrintCorrelator(DataCnf);
		PrintReserveLength(DataCnf);
		PrintDataLen(DataCnf);
		NewLine;
	}
}



/*
 * INDICATION Primitives
 */

void
show_flow_ind(TCLSIMsg *cls_msg)
{
	FlowInd_t *FlowInd = (FlowInd_t *) cls_msg;

	buginf("\nPrimitive: Flow Indication");
	if (clsitest_debug) {
		PrintClsiHdr(FlowInd);
		PrintFlowInd(FlowInd);
		PrintVersion(FlowInd);
		PrintCorrelator(FlowInd);
		NewLine;
	}
}



void 
show_connect_ind(TCLSIMsg *cls_msg)
{
	ConnectInd_t *ConnectInd = (ConnectInd_t *) cls_msg;

	buginf("\nPrimitive: Connect Indication");
	if (clsitest_debug) {
		PrintClsiHdr(ConnectInd);
		PrintVersion(ConnectInd);
		PrintCorrelator(ConnectInd);
		NewLine;
	}
}



void 
show_connected_ind(TCLSIMsg *cls_msg)
{
	ConnectedInd_t *ConnectedInd = (ConnectedInd_t *) cls_msg;

	buginf("\nPrimitive: Connected Indication");
	if (clsitest_debug) {
		PrintClsiHdr(ConnectedInd);
		PrintActionCode(ConnectedInd);
		PrintVersion(ConnectedInd);
		PrintCorrelator(ConnectedInd);
		NewLine;
	}
}



void 
show_disconnect_ind(TCLSIMsg *cls_msg)
{
	DisconnectInd_t *DisconnectInd = (DisconnectInd_t *) cls_msg;

	buginf("\nPrimitive: Disconnect Indication");
	if (clsitest_debug) {
		PrintClsiHdr(DisconnectInd);
		PrintLinkStatus(DisconnectInd);
		PrintVersion(DisconnectInd);
		PrintCorrelator(DisconnectInd);
		NewLine;
	}
}



void 
show_id_ind(TCLSIMsg *cls_msg)
{
	IdInd_t *IdInd = (IdInd_t *) cls_msg;
	int i;
	char *data;

	buginf("\nPrimitive: Id Indication");
	if (clsitest_debug) {
		PrintClsiHdr(IdInd);
		PrintVersion(IdInd);
		PrintCorrelator(IdInd);
		PrintReserveLength(IdInd);
		PrintXID1(IdInd);
		NewLine;
	}
}



void 
show_id_stn_ind(TCLSIMsg *cls_msg)
{
	IdStnInd_t *IdStnInd = (IdStnInd_t *) cls_msg;
	char *data;
	int i;

	p_cep_id = IdStnInd->fCLSIHdr.fPCepId;

	buginf("\nPrimitive: Id Stn Indication");
	if (clsitest_debug) {
		PrintClsiHdr(IdStnInd);
		PrintPCepId(IdStnInd);
		PrintIdStn(IdStnInd);
		NewLine;
	}
}



void 
show_data_ind(TCLSIMsg *cls_msg)
{
	DataInd_t *DataInd = (DataInd_t *) cls_msg;
	int i;
	char *data;

	buginf("\nPrimitive: Data Indication");
	if (clsitest_debug) {
		PrintClsiHdr(DataInd);
		PrintVersion(DataInd);
		PrintCorrelator(DataInd);
		PrintReserveLength(DataInd);
		PrintData(DataInd);
		NewLine;
	}
}



/*
 * RESPONSE Primitives
 */

int 
connect_rsp(parseinfo *csb)
{
	int i;
	ConnectRsp_t*   ConnectRsp;
	ReqOpnStnLLCParms_t *ConnectRspLLCParms;
	int flag_len;
	int rif_len = GETOBJ(int,20);

	ConnectRsp = 
		(ConnectRsp_t *) CLSIMsgCreateHeader(CLS_CONNECT | CLS_RESPONSE,
		offsetof(ConnectRsp_t,fParms) - CLS_CONNECT_REQ_HDR_SZ + 
		sizeof(ReqOpnStnLLCParms_t) + rif_len,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!ConnectRsp)
		return (ERR);

	ConnectRsp->fCLSIHdr.fActionCode	= GETOBJ(int,2);

	flag_len = strlen(GETOBJ(string, 1));
	if ((flag_len != 6) && (flag_len != 0)) {
		buginf("\nInvalid ModifyFlags");
		return(ERR);
	}
#ifdef NOTDEF
	if (flag_len == 0)
	/* default */
		memset((char *) &(ConnectRsp->fCLSIHdr.fModifyFlags), 0, 3);
	else
		for (i=0; i<flag_len; i += 2)
			if (stoh(&(GETOBJ(string, 1)[i]), 
				&(ConnectRsp->fCLSIHdr.fModifyFlags[i/2])) == ERR)
				return(ERR);
#endif

	ConnectRsp->fVersion			= CLSI_VERSION;
	ConnectRsp->fCorrelator			= GETOBJ(int,3);

	ConnectRspLLCParms = (ReqOpnStnLLCParms_t *) & (ConnectRsp->fParms);
	memcpy((char *) & (ConnectRspLLCParms->fDestAddr),
	       (char *)&(*GETOBJ(hwaddr,1)), 
	       sizeof(hwaddrtype));
	memcpy((char *) & (ConnectRspLLCParms->fSrcAddr),
	       (char *)&(*GETOBJ(hwaddr,2)), 
	       sizeof(hwaddrtype));
	ConnectRspLLCParms->fDestSap		= GETOBJ(int,4);
	ConnectRspLLCParms->fSrcSap		= GETOBJ(int,5);
	ConnectRspLLCParms->fT1			= GETOBJ(int,6);
	ConnectRspLLCParms->fT2			= GETOBJ(int,7);
	ConnectRspLLCParms->fTi			= GETOBJ(int,8);
	ConnectRspLLCParms->fMaxOut		= GETOBJ(int,9);
	ConnectRspLLCParms->fMaxRetryCount	= GETOBJ(int,10);
	ConnectRspLLCParms->fMaxIn		= GETOBJ(int,11);
	ConnectRspLLCParms->fDynamicWindow	= GETOBJ(int,16);
	ConnectRspLLCParms->fMaxOutIncr		= GETOBJ(int,12);
	ConnectRspLLCParms->fMaxIField		= GETOBJ(int,13);
	ConnectRspLLCParms->fXIDRetries		= GETOBJ(int,14);
	ConnectRspLLCParms->fXIDTimer		= GETOBJ(int,15);
	ConnectRspLLCParms->fExplorerOption	= GETOBJ(int,17);
	ConnectRspLLCParms->fExplorerRetries	= GETOBJ(int,18);
	ConnectRspLLCParms->fExplorerTimer	= GETOBJ(int,19);
	ConnectRspLLCParms->fRifLength		= GETOBJ(int,20);

	rif_len *= 2;
	
	if (strlen(GETOBJ(string, 2)) != rif_len) {
		buginf("\nInvalid length rif: should be %d instead %d\n",
			rif_len, strlen(GETOBJ(string, 1)));
		return(ERR);
	}
	for (i=0; i<rif_len; i += 2)
		if (stoh(&(GETOBJ(string, 2)[i]), 
			&(ConnectRspLLCParms->fRif[i/2])) == ERR)
			return(ERR);

#ifdef NOTDEF
	if (clsitest_debug) {
		PrintClsiHdr(ConnectRsp);
		PrintModifyFlags(ConnectRsp);
		PrintVersion(ConnectRsp);
		PrintCorrelator(ConnectRsp);
		PrintReqOpnStnLLCParms(ConnectRspLLCParms);
	}
#endif

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) ConnectRsp);
	return (OK);
}



int 
disconnect_rsp(parseinfo *csb)
{
	DisconnectRsp_t*   DisconnectRsp;

	DisconnectRsp = (DisconnectRsp_t *) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_RESPONSE,
		sizeof(DisconnectRsp_t) - CLS_DISCONNECT_RSP_HDR_SZ,
		CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!DisconnectRsp)
		return (ERR);

	DisconnectRsp->fCLSIHdr.fActionCode	= GETOBJ(int,2);
	DisconnectRsp->fVersion			= CLSI_VERSION;
	DisconnectRsp->fCorrelator		= GETOBJ(int,3);
	if (clsitest_debug) {
		PrintClsiHdr(DisconnectRsp);
		PrintActionCode(DisconnectRsp);
		PrintVersion(DisconnectRsp);
		PrintCorrelator(DisconnectRsp);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) DisconnectRsp);

	return (OK);
}



int
id_rsp(parseinfo *csb)
{
	IdRsp_t*   IdRsp;
	int	id_len;
	char	*xid;
	int	i;
	u_char	*data;

	id_len = strlen(GETOBJ(string,1));
	if (id_len %2) {
		buginf("\nInvalid XID: must be even number byte of string!");
		return(ERR);
	}
	id_len /= 2;

	IdRsp = (IdRsp_t *) CLSIMsgCreateHeader(CLS_ID | CLS_RESPONSE,
		offsetof(IdRsp_t,fXID) - CLS_ID_REQ_HDR_SZ + reserveLengthType1 
		+ id_len, CLS_LLC_LAYER, CLS_P_CEP_ID, p_cep_id);
    
	if (!IdRsp)
		return (ERR);

	IdRsp->fVersion		= CLSI_VERSION;
	IdRsp->fCorrelator	= GETOBJ(int,2);
	IdRsp->fReserveLength	= reserveLengthType1;

	xid = IdRsp->fXID + IdRsp->fReserveLength;
	for (i=0; i<id_len*2; i += 2)
		if (stoh(&(GETOBJ(string, 1)[i]), &(xid[i/2])) == ERR)
			return(ERR);

	if (clsitest_debug) {
		PrintClsiHdr(IdRsp);
		PrintVersion(IdRsp);
		PrintCorrelator(IdRsp);
		PrintReserveLength(IdRsp);
		PrintXID1(IdRsp);
		NewLine;
	}

	CLSIQueuePut(ClsiWriteQ, (TCLSIMsg*) IdRsp);

	return (OK);
}



void
retrieve_clsi_msg(TCLSIQueue *aQueue, TCLSIMsg *cls_msg)
{
	unsigned int msg_size;

	msg_size = cls_msg->fHeaderLength + cls_msg->fDataSize;

	switch(cls_msg->fPrimitiveCode) {
	case CLS_ENABLE | CLS_CONFIRM:
		show_enable_cnf(cls_msg);
		break;

	case CLS_DISABLE | CLS_CONFIRM:
		show_disable_cnf(cls_msg);
		break;

	case CLS_ACTIVATE_SAP | CLS_CONFIRM:
		show_activate_sap_cnf(cls_msg);
		break;

	case CLS_DEACTIVATE_SAP | CLS_CONFIRM:
		show_deactivate_sap_cnf(cls_msg);
		break;

	case CLS_FLOW | CLS_INDICATION:
		show_flow_ind(cls_msg);
		break;

	case CLS_CONNECT | CLS_INDICATION:
		show_connect_ind(cls_msg);
		break;

	case CLS_CONNECTED | CLS_INDICATION:
		show_connected_ind(cls_msg);
		break;

	case CLS_DISCONNECT | CLS_INDICATION:
		show_disconnect_ind(cls_msg);
		break;

	case CLS_ID | CLS_INDICATION:
		show_id_ind(cls_msg);
		break;

	case CLS_ID_STN | CLS_INDICATION:
		show_id_stn_ind(cls_msg);
		break;

	case CLS_DATA | CLS_INDICATION:
		show_data_ind(cls_msg);
		break;

	case CLS_OPEN_STN | CLS_CONFIRM:
		show_open_stn_cnf(cls_msg);
		break;

	case CLS_REQ_OPNSTN | CLS_CONFIRM:
		show_req_opnstn_cnf(cls_msg);
		break;

	case CLS_CLOSE_STN | CLS_CONFIRM:
		show_close_stn_cnf(cls_msg);
		break;

	case CLS_CONNECT | CLS_CONFIRM:
		show_connect_cnf(cls_msg);
		break;

	case CLS_DISCONNECT | CLS_CONFIRM:
		show_disconnect_cnf(cls_msg);
		break;

	case CLS_ID | CLS_CONFIRM:
		show_id_cnf(cls_msg);
		break;

	case CLS_SIGNAL_STN | CLS_CONFIRM:
		show_signal_stn_cnf(cls_msg);
		break;

	case CLS_DATA | CLS_CONFIRM:
		show_data_cnf(cls_msg);
		break;

	default:
		buginf("\nUnsupport primitive: 0x%x\n", 
			cls_msg->fPrimitiveCode);
	}

	CLSIMsgDestroy(cls_msg);
}



int clsi_rput(TCLSIQueue* clsiReadQ, TCLSIMsg* theMsg) {
	process_enqueue(ClsiInputQueue, theMsg);
	return(CLS_OK);
}




int
clsitest_start(void)
{
	if ((clsitest_qinit_p = CLSIQInitCreate ("ClsiTest", clsi_rput)) == 0)
		return (ERR);

	if ((ClsiReadQ = CLSIQueueCreate (clsitest_qinit_p, NULL)) == 0)
		return (ERR);

	if ((ClsiWriteQ = CLSOpen (ClsiReadQ)) == 0)
		return (ERR);

	return (OK);
}



void
clsitest_stop(void)
{
	CLSClose(ClsiWriteQ);

	CLSIQueueDestroy(ClsiReadQ);

	CLSIQInitDestroy(clsitest_qinit_p);
}

process clsitest_main (void)
{
    boolean have_event;      /*loop control*/
    boolean terminate;       /*loop control*/
    ulong major, minor;      /*Event information*/
    TCLSIMsg *cls_msg;
    TCLSIMsg* aMsg;

	if (clsitest_start() != OK) {
		buginf("\nCLSItest fails to start");
		return;
	}

	reserveLengthType1 = 0;
	reserveLengthType2 = 0;
	p_sap_id = 0;
	p_cep_id = CLS_NO_CEP;

	process_set_boolean(ClsiTestShut, FALSE);
	ClsiInputQueue = create_watched_queue(CLSI_TEST_QUEUE,
					      0,
					      CLSI_TEST_QUEUE_EVENT);
        process_watch_queue(ClsiInputQueue, ENABLE, RECURRING);
        process_watch_boolean(ClsiTestShut, ENABLE, RECURRING);
    
        terminate = FALSE;  /*Get into the loop*/
	while (!terminate) {

	    process_wait_for_event();                     /* Any event */
	    have_event = process_get_wakeup(&major,&minor);  /*Event info*/

	    while (have_event) {
		switch(major) {
		case QUEUE_EVENT:
		    switch(minor) {
		    case CLSI_TEST_QUEUE_EVENT:
			if ((cls_msg = process_dequeue(ClsiInputQueue)) != 
			    NULL) {
			    retrieve_clsi_msg((TCLSIQueue *) &ClsiInputQueue, 
					      cls_msg);
			}
			break;
		    default:
			break;
		    }
		case BOOLEAN_EVENT:
		    switch (minor) {
		    case CLSI_TEST_SHUT_EVENT:
			process_set_boolean(ClsiTestShut, FALSE);
			terminate = TRUE;
			break;
		    default:
			break;
		    }
		default:
		    break;
		}
		have_event = process_get_wakeup(&major,&minor); /*next event*/
	    }
	}
	/*
	 * If we got here, then ClsiTestShut has been set to TRUE by
	 * something. Probably the "clsitest stop" command. Time to start
	 * cleaning up.
	 */
	clsitest_stop();

	while ((aMsg = process_dequeue(ClsiInputQueue)) != NULL) {
	    CLSIMsgDestroy(aMsg);
	}
	delete_watched_queue(&ClsiInputQueue);
	delete_watched_boolean(&ClsiTestShut);
	watch_ok = FALSE;
	process_kill(THIS_PROCESS);
	buginf("\nCLSI Test is stopped\n");
}




void 
clsitest_command (parseinfo *csb)
{
	pid_t clsitest_pid;
	int ret;
	char clsi_prim[20];

	if (!watch_ok) {
	    ClsiTestShut = create_watched_boolean(CLSI_TEST_SHUT,
						  CLSI_TEST_SHUT_EVENT);
	    watch_ok = TRUE;  /*watched boolean now exists*/
	    process_set_boolean(ClsiTestShut, TRUE); /*Shut down at first*/
	}
	if (process_get_boolean(ClsiTestShut) 
	    && (GETOBJ(int, 1) != CLSISTART)) {
		buginf("\nStart CLSI test before entering CLSI command!");
		return;
	}

	switch (GETOBJ(int,1)) {
	case CLSISTART:
		if (!process_get_boolean(ClsiTestShut)) {
			buginf("\nThe CLSI Test program is running\n");
			return;
		}
		clsitest_pid = process_create(clsitest_main,
					      "CLSITest program",
					      NORMAL_STACK,PRIO_NORMAL);
					      
		buginf ("\nThe CLSI Test program is started\n");
		return;

	case CLSISTOP:
		/*
		clsitest_stop();
		*/
		process_set_boolean(ClsiTestShut, TRUE); /*Tell test to shut*/
		return;

	case CLSIDEBUG:
		clsitest_debug = GETOBJ(int, 2);
		if (clsitest_debug)
			printf("CLSI test debug is on\n");
		else
			printf("CLSI test debug is off\n");
		return;
		
	case CLS_ENABLE | CLS_REQUEST:
		ret = enable_req(csb);
		sprintf(clsi_prim, "ENABLE REQ");
		break;
	
	case CLS_DISABLE | CLS_REQUEST:
		ret = disable_req(csb);
		sprintf(clsi_prim, "DISABLE REQ");
		break;
	
	case CLS_ACTIVATE_SAP | CLS_REQUEST:
		ret = activate_sap_req(csb);
		sprintf(clsi_prim, "ACTIVATE SAP REQ");
		break;
	
	case CLS_DEACTIVATE_SAP | CLS_REQUEST:
		ret = deactivate_sap_req(csb);
		sprintf(clsi_prim, "DEACTIVATE SAP REQ");
		break;
	
	case CLS_FLOW | CLS_REQUEST:
		ret = clsi_flow_req(csb);
		sprintf(clsi_prim, "FLOW REQ");
		break;
	
	case CLS_OPEN_STN | CLS_REQUEST:
		ret = open_stn_req(csb);
		sprintf(clsi_prim, "OPEN STN REQ");
		break;
	
	case CLS_REQ_OPNSTN | CLS_REQUEST:
		ret = req_opnstn_req(csb);
		sprintf(clsi_prim, "REQ OPNSTN REQ");
		break;
	
	case CLS_CLOSE_STN | CLS_REQUEST:
		ret = close_stn_req(csb);
		sprintf(clsi_prim, "CLOSE STN REQ");
		break;
	
	case CLS_CONNECT | CLS_REQUEST:
		ret = connect_req(csb);
		sprintf(clsi_prim, "CONNECT REQ");
		break;
	
	case CLS_DISCONNECT | CLS_REQUEST:
		ret = disconnect_req(csb);
		sprintf(clsi_prim, "DISCONNECT REQ");
		break;
	
	case CLS_ID | CLS_REQUEST:
		ret = id_req(csb);
		sprintf(clsi_prim, "ID REQ");
		break;
	
	case CLS_SIGNAL_STN | CLS_REQUEST:
		ret = signal_stn_req(csb);
		sprintf(clsi_prim, "SIGNAL STN REQ");
		break;
	
	case CLS_DATA | CLS_REQUEST:
		ret = data_req(csb);
		sprintf(clsi_prim, "DATA REQ");
		break;
	
	case CLS_CONNECT | CLS_RESPONSE:
		ret = connect_rsp(csb);
		sprintf(clsi_prim, "CONNECT RSP");
		break;
	
	case CLS_DISCONNECT | CLS_RESPONSE:
		ret = disconnect_rsp(csb);
		sprintf(clsi_prim, "DISCONNECT RSP");
		break;
	
	case CLS_ID | CLS_RESPONSE:
		ret = id_rsp(csb);
		sprintf(clsi_prim, "ID RSP");
		break;
	
	default:
		buginf("\nReceive unsupported CLSI primitive 0x%x\n", GETOBJ(int,1));
		return;
	}

	if (ret == ERR)
		buginf("\nFail to send %s\n", clsi_prim);
	else
		buginf("\nSend %s successfully\n", clsi_prim);
}

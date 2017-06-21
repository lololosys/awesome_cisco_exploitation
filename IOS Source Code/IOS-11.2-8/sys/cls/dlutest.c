/* $Id: dlutest.c,v 3.4.10.3 1996/05/17 10:43:02 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlutest.c,v $
 *------------------------------------------------------------------
 * 
 * 11-Feb-1994  Leo Pereira
 *
 * This file contains the functions for the DLU Test program.
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlutest.c,v $
 * Revision 3.4.10.3  1996/05/17  10:43:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.5  1996/05/05  22:51:25  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.2.4  1996/04/26  01:53:09  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.3  1996/04/24  06:28:22  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.4.2.2  1996/04/03  13:55:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.2.1  1996/03/28  08:19:43  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.4.10.2  1996/04/25  23:03:38  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.10.1.2.1  1996/03/24  03:46:44  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.4.10.1  1996/03/18  19:10:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  06:32:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/07  08:41:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:41:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/26  06:09:17  dmccowan
 * CSCdi49560:  APPN Connection Network over FDDI fails to activate
 * Fill in EnableTRParms_t structure for FDDI.
 * Remove Obsolete EnableFDDIParms_t structure.
 *
 * Revision 3.3.4.1  1995/12/17  09:50:16  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.3  1995/11/17  08:56:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:12:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:53:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/11  04:20:26  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.6  1995/07/19  20:58:33  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.5  1995/07/02  05:15:43  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.4  1995/06/18  19:40:07  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/09  19:27:11  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.2  1995/06/09  12:59:31  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:20:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "address.h"
#include "clsi_msg.h"
#include "clsi_queue.h"
#include "config.h"
#include "interface_private.h"
#include "../if/tring.h"
#include "dlutest.h"
#include "../ibm/sna_util.h"
#include "../ibm/libibm_externs.h"

/*
 * Per connection variables for dlutest
 */
STRUCTURE_REF(TDLUConn);

struct TDLUConn_
{
    TDLUConn*      fNext;
    word           fUCepId;
    word           fPCepId;
    dluRputStateT  fRputState;
    int            fEchoCounter;
    hwaddrtype     fDestAddr;
    int            fRole;
    int            fNumFrames;	/* number of I-frames to send before disconnecting */
    int		   fNumSent;
    int		   fCredits;
    ulong          fFlowLevel;
};

/*
 * Events which awaken the CLS Process
 */

#define DLU_STOP "DLU test stop flag"
#define DLU_INPUT_Q "DLU input queue"

/* 
 *  minor codes for dlutest events 
 */
typedef enum dlutest_events_ {
    DLU_STOP_EVENT,
    DLU_INPUT_Q_EVENT
} dlutest_events;  

/*
 * Global variables for all of dlutest
 */
static watched_boolean *gDLUStop;
static boolean 	   gDLURunning;
static TCLSIQInit* gQInit;
static TCLSIQueue* gDLUReadQ;
static TCLSIQueue* gCLSWriteQ;
static word        gDLUPSapId;
static queuetype   gDLUConnList;
static watched_queue *gDLUInputQueue;
static int         gDLURole;
static int	   gDLUCallOut;
static int         gDLUNumFrames;
static byte        gDLUDestAddr[IEEEBYTES];
static byte	   gDLUSDLCAddr = 0x1;
static byte	   gDLUAddrType = STATION_IEEE48;
static int         gDLUDLCI;
static idbtype	   *gDluIdb;		/* which interface to use */

static int 	   gDLUVCD;		/* ATM */
static int	   gDLUEcho;		/* secondary: echo packets */

/* Qllc specific parameters */

static boolean	   gDluQllcUseSubaddr;	/* vmac or subaddress */
static hwaddrtype  gDluQllcSapAddress;	/* subaddress for SAP */
static hwaddrtype  gDluQllcSapVmacStart;/* base for vmac range */

static hwaddrtype  gDluQllcRemote;	/* destination address */
static hwaddrtype  gDluQllcPartner;	/* fSrcAddr */

/* Smds specific paramter */
static hwaddrtype  gDluSmdsRemote;	/* destination address */

/* Vdlc specific parameters */

static ushort	   gDLUVrn;		/* Vdlc - Ring group number */
static hwaddrtype  gDLUVdlcLmac;	/* Local (Virtual) Mac Address */
static hwaddrtype  gDLUVdlcRmac;	/* Remote Mac Address */
static uchar	   gDLUVdlcLSap;	/* Local Sap */
static  uchar	   gDLUVdlcRSap;	/* Remote Sap */

void BuildQActSapReq(ActSapQLLCParms_t *fParms);
void BuildQROSR(ReqOpnStnQLLCParms_t *fParms, hwaddrtype *remote);

dluPortType gDLUPortType;

byte gDLUCannedXID[6] = { 0x02, 0x00, 0x01,  0x71, 0x23, 0x45 };
#define XIDSIZE 6

static byte gDLUCannedData[] = 
{
    "<This is some data that is being sent from the DLUTest program>"
};

static void SendEnable(void)
{
    uint parmSize;
    
    EnableReq_t*   enableReq;

    switch (gDLUPortType)	/* grab param size for CreateHeader() below */
    {
    case DLU_Rsrb: 
	parmSize = sizeof(EnableRSRBParms_t); 
	break;

    case DLU_EtherNet:
    case DLU_TokenRing: 
    case DLU_Smds:
	parmSize = sizeof(EnableTRParms_t); 
	break;

    case DLU_FrameRelay:
    case DLU_Ppp:
	parmSize = sizeof(EnableFRParms_t); 
	break;
	
    case DLU_Sdlc: 
	parmSize = sizeof(EnableSDLCParms_t); 
	break;

    case DLU_Qllc: 
	parmSize = sizeof(EnableQLLCParms_t); 
	break;

    case DLU_Vdlc: 
	parmSize = sizeof(EnableVDLCParms_t); 
	break;

    default:
	buginf("\nSendEnable: unknown port type %d", gDLUPortType);
	parmSize = 0;
	break;
    }
    
    enableReq = (EnableReq_t*) CLSIMsgCreateHeader(CLS_ENABLE | CLS_REQUEST,
						   offsetof(EnableReq_t,fParms) -
						   CLS_ENABLE_REQ_HDR_SZ +
						   parmSize,
						   CLS_LLC_LAYER,
						   CLS_ENTITY_MGR,
						   0);
    if (enableReq == NULL)
    {
	buginf("\nDLUTest: SendEnable() couldn't get a message");
	return;
    }
    
	enableReq->fPortId.type =  enableReq->fPortId.type = CLS_P_TYPE_SWIDB;
	enableReq->fPortId.fTypeInfo.swidb = gDluIdb;
    enableReq->fCorrelator = 0xdead;

    switch (gDLUPortType)	/* initialize port dependent header fields */
    {
    case DLU_Rsrb:
    {
	EnableRSRBParms_t* rsrbParms;
	static uchar dlu_rsrb_vmacaddr_primary[IEEEBYTES] = {0x00,0x00,0xde,0xad,0xee,0xef};
	static uchar dlu_rsrb_vmacaddr_secondary[IEEEBYTES] = {0x00,0x00,0xde,0xad,0xca,0xfe};
	enableReq->fDLCType = CLS_RSRB;

	rsrbParms = (EnableRSRBParms_t*) &(enableReq->fParms);
	rsrbParms->fVersion = 1;

	if (gDLURole == DLUPRIMARY)
	    snpa_init(&rsrbParms->fVMacAddr, STATION_IEEE48, TARGET_UNICAST,
		      dlu_rsrb_vmacaddr_primary);
	else
	    snpa_init(&rsrbParms->fVMacAddr, STATION_IEEE48, TARGET_UNICAST,
		      dlu_rsrb_vmacaddr_secondary);

	rsrbParms->fRsrbOptionFlags = 0;   /* LACK on/of, etc... */
				              /* setting is NO-OP for now. */
	rsrbParms->fMaxIFieldLength = 0;    /* size of payload, not including any headers */
	                                      /* setting is NO-OP for now. */
	rsrbParms->fLocalRing    = 0xB52;     /* CLS local virtual ring     */
	rsrbParms->fBridgeNumber = 0x01;      /* Bridge num to CLS local VR */
	rsrbParms->fTargetRing   = 0xB2;      /* SRB target virtual ring    */

	break;
    }
    case DLU_Sdlc:
    {
	EnableSDLCParms_t *sdlcParms;
	enableReq->fDLCType = CLS_SDLC;

	sdlcParms = (EnableSDLCParms_t *) &(enableReq->fParms);
	sdlcParms->fVersion = 1;
	break;
    }
    case DLU_Qllc:
    {
	EnableQLLCParms_t *qllcParms;
	enableReq->fDLCType = CLS_QLLC;

	qllcParms = (EnableQLLCParms_t *) &(enableReq->fParms);
	qllcParms->fVersion = 1;
	break;
    }
    case DLU_Vdlc:
    {
	EnableVDLCParms_t *vdlcParms;
	enableReq->fDLCType = CLS_VDLC;

	vdlcParms = (EnableVDLCParms_t *) &(enableReq->fParms);
	vdlcParms->fVersion = 1;
	vdlcParms->fRingGroup = gDLUVrn;
	snpa_copy(&gDLUVdlcLmac, &vdlcParms->fVMacAddr);
	vdlcParms->fVdlcOptionFlags = 0; /* no options set */
	vdlcParms->fMaxIFieldLength = 0; /* no limits on max packet size */

	break;
    }
    case DLU_EtherNet:
    {
	EnableTRParms_t* trParms;

	enableReq->fDLCType = CLS_LLC;

	trParms = (EnableTRParms_t*) &(enableReq->fParms);
	trParms->fVersion = 1;

	/* Use defaults for everything, for now. */
	memset(&trParms->fLocalMac, 0, IEEEBYTES);
	trParms->fTrSpeed = 0;
	trParms->fMaxIFieldLength = 0;
	break;
    }
    case DLU_TokenRing:
    {
	EnableTRParms_t* trParms;
	enableReq->fDLCType = CLS_LLC;

	trParms = (EnableTRParms_t*) &(enableReq->fParms);
	trParms->fVersion = 1;

	/* Use defaults for everything, for now. */
	memset(&trParms->fLocalMac, 0, IEEEBYTES);
	trParms->fTrSpeed = 0;
	trParms->fMaxIFieldLength = 0;
	break;
    }
    case DLU_FrameRelay:
    {
	enableReq->fDLCType = CLS_FRAME_RELAY;
	break;
    }
    case DLU_Ppp:
    {
	enableReq->fDLCType = CLS_PPP;
	break;
    }
#ifdef NOT
    case DLU_ATM_1483:
    {
	enableReq->fDLCType = CLS_ATM_1483;
	break;
    }
#endif
    case DLU_Fddi:
    {
	EnableTRParms_t* trParms;
	enableReq->fDLCType = CLS_LLC;

	trParms = (EnableTRParms_t*) &(enableReq->fParms);
	trParms->fVersion = 1;

	/* Use defaults for everything, for now. */
	snpa_clear(&trParms->fLocalMac);
	trParms->fTrSpeed = 0;
	trParms->fMaxIFieldLength = 0;
	break;
    }
    case DLU_Smds:
    {
	EnableTRParms_t* trParms;

	enableReq->fDLCType = CLS_LLC;

	trParms = (EnableTRParms_t*) &(enableReq->fParms);
	trParms->fVersion = 1;

	/* Use defaults for everything, for now. */
	memset(&trParms->fLocalMac, 0, IEEEBYTES);
	trParms->fTrSpeed = 0;
	trParms->fMaxIFieldLength = 0;
	break;
    }
    default:
	buginf("\nSendEnable: unknown port type %d", gDLUPortType);
	parmSize = 0;
	break;
    }
    
    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) enableReq);
    
}

static void SendActSap(void)
{
    ActSapReq_t* actSapReq;
    ActSapLLCParms_t* llcParms;
    ActSapSDLCParms_t* sdlcParms;
    int parmSize;

    if (gDLUPortType == DLU_Sdlc)
	parmSize = sizeof(ActSapSDLCParms_t);
    else if (gDLUPortType == DLU_Qllc) 
	parmSize = sizeof(ActSapQLLCParms_t);
    else if (gDLUPortType == DLU_Vdlc) 
	parmSize = sizeof(ActSapVDLCParms_t);
    else 
	parmSize = sizeof(ActSapLLCParms_t);

    actSapReq = (ActSapReq_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_SAP | CLS_REQUEST,
						   offsetof(ActSapReq_t, fParms) -
						   CLS_ACT_SAP_REQ_HDR_SZ +
						   parmSize,
						   CLS_LLC_LAYER,
						   CLS_ENTITY_MGR,
						   0);
    if (actSapReq == NULL)
    {
	buginf("\nDLUTest: SendActSap() couldn't get a message");
	return;
    }
    
    actSapReq->fCLSIHdr.fUSapId = 0x1234;
    actSapReq->fVersion = 1;
    actSapReq->fCorrelator = 0xdead;

    /* should be done by table lookup */

    switch (gDLUPortType) {
    case DLU_Rsrb:
	actSapReq->fDLCType = CLS_RSRB;
	break;
    case DLU_FrameRelay:
	actSapReq->fDLCType = CLS_FRAME_RELAY;
	break;
    case DLU_Ppp:
	actSapReq->fDLCType = CLS_PPP;
	break;
    case DLU_ATM_1483:
	actSapReq->fDLCType = CLS_ATM_1483;
	break;
    case DLU_Sdlc:
	actSapReq->fDLCType = CLS_SDLC;
	break;
    case DLU_Qllc:
	actSapReq->fDLCType = CLS_QLLC;
	break;
    case DLU_Vdlc:
	actSapReq->fDLCType = CLS_VDLC;
	break;
    case DLU_EtherNet:
	actSapReq->fDLCType = CLS_LLC;
	break;
    case DLU_Fddi:
	actSapReq->fDLCType = CLS_LLC;
	break;
    case DLU_TokenRing:
	actSapReq->fDLCType = CLS_LLC;
	break;
    case DLU_Smds:
	actSapReq->fDLCType = CLS_LLC;
	break;
    }

    if (gDluIdb != NULL) {
	actSapReq->fPortId.type = CLS_P_TYPE_SWIDB;
	actSapReq->fPortId.fTypeInfo.swidb = gDluIdb;
    } else {
	buginf("\nCLEAN UP - PHIL 9/15/95");
    }    
    if (gDLUPortType == DLU_Sdlc) {
	sdlcParms = (ActSapSDLCParms_t*) &(actSapReq->fParms);
	sdlcParms->fSapValue = gDLUSDLCAddr;
	sdlcParms->fAddrType = gDLUAddrType;
    } else  if (gDLUPortType == DLU_Qllc) {
	BuildQActSapReq( (ActSapQLLCParms_t*)(&actSapReq->fParms));
    } else  if (gDLUPortType == DLU_Vdlc) {
    	ActSapVDLCParms_t	*vdlcParms;
	vdlcParms = (ActSapVDLCParms_t *)(&actSapReq->fParms);
	vdlcParms->fSapValue = gDLUVdlcLSap ? gDLUVdlcLSap : 0x04;
    } else {
	llcParms = (ActSapLLCParms_t*) &(actSapReq->fParms);

	llcParms->fUserClass = 0x02;
	llcParms->fSapValue = 0x04;
	llcParms->fT1 = 100;
	llcParms->fT2 = 100;
	llcParms->fTi = 10000;
	llcParms->fMaxOut = 7;
	llcParms->fMaxRetryCount = 0;		/* no limit */
	llcParms->fMaxIn = 7;
	llcParms->fMaxIField = 521;
    }
    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) actSapReq);
}


static void SendReqOpnStn(word uCepId, hwaddrtype* remoteAddr, byte rSap,
			  byte options, byte rifLength, byte* rif)
{
    ReqOpnStnReq_t* reqOpnStnReq;
    ReqOpnStnLLCParms_t* llcParms;
    ReqOpnStnSDLCParms_t* sdlcParms;
    int parmSize;

    if (gDLUPortType == DLU_Sdlc) 
	parmSize = sizeof(ReqOpnStnSDLCParms_t);
    else if (gDLUPortType == DLU_Qllc) 
	parmSize = sizeof(ReqOpnStnQLLCParms_t);
    else if (gDLUPortType == DLU_Vdlc) 
	parmSize = sizeof(ReqOpnStnVDLCParms_t);
    else
	parmSize = sizeof(ReqOpnStnLLCParms_t) + rifLength;
    
    reqOpnStnReq = 
	(ReqOpnStnReq_t*) CLSIMsgCreateHeader(CLS_REQ_OPNSTN | CLS_REQUEST,
					      offsetof(ReqOpnStnReq_t,fParms)
					      -CLS_REQ_OPNSTN_REQ_HDR_SZ
					      + parmSize,
					      CLS_LLC_LAYER,
					      CLS_P_SAP_ID,
					      gDLUPSapId);

    if (reqOpnStnReq == NULL)
    {
	buginf("\nDLUTest: SendReqOpnStn() couldn't get a message");
	return;
    }
    
    reqOpnStnReq->fCLSIHdr.fUCepId = uCepId;
    reqOpnStnReq->fVersion = 1;
    reqOpnStnReq->fCorrelator = 0xdead;
    reqOpnStnReq->fQueuingType = CLS_QUEUING_DEFAULT;
    if (gDLUPortType == DLU_Sdlc) {
	sdlcParms = (ReqOpnStnSDLCParms_t*) &(reqOpnStnReq->fParms);

	/*
	 * If this is an incoming call, these came from the idStn.ind
	 */

	snpa_copy(remoteAddr, &sdlcParms->fDestAddr);
	snpa_clear(&sdlcParms->fSrcAddr);
    
	sdlcParms->fDestSap = rSap;
	sdlcParms->fSrcSap = 0;
	sdlcParms->fExplorerOption = options;
    
	sdlcParms->fExplorerRetries = 5;
	sdlcParms->fExplorerTimer = 2000;
    
	/*
	 * These are other configuration parameters. Mostly use defaults.
	 */
	sdlcParms->fXIDRetries = 10;
	sdlcParms->fXIDTimer = 5000;
    } else if (gDLUPortType == DLU_Qllc) {
	BuildQROSR((ReqOpnStnQLLCParms_t*) &(reqOpnStnReq->fParms), remoteAddr);
    } else {
	llcParms = (ReqOpnStnLLCParms_t*) &(reqOpnStnReq->fParms);

	/*
	 * If this is an incoming call, these came from the idStn.ind
	 */
	snpa_copy(remoteAddr, &llcParms->fDestAddr);
	snpa_clear(&llcParms->fSrcAddr);
    
	llcParms->fDestSap = rSap;
	llcParms->fSrcSap = 0;
	llcParms->fExplorerOption = options;
	llcParms->fRifLength = rifLength;
	if (rifLength > 0)
	    memcpy(llcParms->fRif, rif, rifLength);
    
	llcParms->fExplorerRetries = 5;
	llcParms->fExplorerTimer = 2000;
    
	/*
	 * These are other configuration parameters. Mostly use defaults.
	 */
	llcParms->fT1 = 0;
	llcParms->fT2 = 0;
	llcParms->fTi = 0;
	llcParms->fMaxOut = 0;
	llcParms->fMaxRetryCount = 0;
	llcParms->fMaxIn = 0;
	llcParms->fDynamicWindow = 0;
	llcParms->fMaxOutIncr = 0;
	llcParms->fMaxIField = 0;
	llcParms->fXIDRetries = 10;
	llcParms->fXIDTimer = 5000;

	/*
	 * This is specific to Frame Relay/RFC1490 connections
	 */
	llcParms->fL3Pid = CLS_L3PID_APPN;
    }

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) reqOpnStnReq);
}

void SendIDReq(word pCepId, byte* xidData, size_t xidLength)
{
    IdReq_t* idReq;
    
    idReq = (IdReq_t*) CLSIMsgCreateHeader(CLS_ID|CLS_REQUEST,
					   offsetof(IdReq_t, fXID) -
					   CLS_ID_REQ_HDR_SZ +
					   xidLength,
					   CLS_LLC_LAYER,
					   CLS_P_CEP_ID,
					   pCepId);
    if (idReq == NULL)
    {
	buginf("\nDLUTest: SendIdReq() couldn't get a message");
	return;
    }
    
    idReq->fVersion = CLSI_VERSION;
    idReq->fCorrelator = 0xdead;
    idReq->fReserveLength = 0;
    memcpy(&idReq->fXID[0], xidData, xidLength);
    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) idReq);
    
}


void SendIDRsp(word pCepId, byte* xidData, size_t xidLength)
{
    IdRsp_t* idRsp;

    idRsp = (IdRsp_t*) CLSIMsgCreateHeader(CLS_ID | CLS_RESPONSE,
					   offsetof(IdRsp_t, fXID) -
					   CLS_ID_RSP_HDR_SZ +
					   xidLength,
					   CLS_LLC_LAYER,
					   CLS_P_CEP_ID,
					   pCepId);
    if (idRsp == NULL)
    {
	buginf("\nDLUTest: SendIdRsp() couldn't get a message");
	return;
    }
    
    idRsp->fVersion = CLSI_VERSION;
    idRsp->fCorrelator = 0xdead;
    idRsp->fReserveLength = 0;
    memcpy(&idRsp->fXID[0], xidData, xidLength);
    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) idRsp);
    
}

void SendConnectReq(word pCepId)
{
    ConnectReq_t* connectReq;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    connectReq = (ConnectReq_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_REQUEST,
						     offsetof(ConnectReq_t, fParms) -
						     CLS_CONNECT_REQ_HDR_SZ +
						     parmSize,
						     CLS_LLC_LAYER,
						     CLS_P_CEP_ID,
						     pCepId);
    if (connectReq == NULL)
    {
	buginf("\nDLUTest: SendConnectReq() couldn't get a message");
	return;
    }
    
    memset(&connectReq->fCLSIHdr.fModifyFlags, 0,
	   sizeof(connectReq->fCLSIHdr.fModifyFlags));
    connectReq->fVersion = CLSI_VERSION;
    connectReq->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) connectReq);
}


void SendPongReq(word pCepId)
{
    PongReq_t* pongReq;
    
    pongReq = (PongReq_t*) CLSIMsgCreateHeader(CLS_PONG | CLS_REQUEST,
						     sizeof(PongReq_t) -
						     CLS_PONG_REQ_HDR_SZ,
						     CLS_LLC_LAYER,
						     CLS_P_CEP_ID,
						     pCepId);
    if (pongReq == NULL)
    {
	buginf("\nDLUTest: SendPongReq() couldn't get a message");
	return;
    }
    
    pongReq->fVersion = CLSI_VERSION;
    pongReq->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) pongReq);
}


void SendConnectRsp(word pCepId)
{
    ConnectRsp_t* connectRsp;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    connectRsp = (ConnectRsp_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_RESPONSE,
						     offsetof(ConnectRsp_t, fParms) -
						     CLS_CONNECT_RSP_HDR_SZ +
						     parmSize,
						     CLS_LLC_LAYER,
						     CLS_P_CEP_ID,
						     pCepId);
    if (connectRsp == NULL)
    {
	buginf("\nDLUTest: SendConnectRsp() couldn't get a message");
	return;
    }
    
    connectRsp->fCLSIHdr.fActionCode = 0;
    memset(&connectRsp->fCLSIHdr.fModifyFlags, 0,
	   sizeof(connectRsp->fCLSIHdr.fModifyFlags));
    connectRsp->fVersion = CLSI_VERSION;
    connectRsp->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) connectRsp);
}

void SendSignalStn(word pCepId)
{
    SignalStnReq_t* sigStnReq;
    
    sigStnReq = (SignalStnReq_t*)
	CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
			    sizeof(SignalStnReq_t) -
			    CLS_SIGNAL_STN_REQ_HDR_SZ,
			    CLS_LLC_LAYER,
			    CLS_P_CEP_ID,
			    pCepId);
    if (sigStnReq == NULL)
    {
	buginf("\nDLUTest: SendSignalStn() couldn't get a message");
	return;
    }
    
    sigStnReq->fCLSIHdr.fActionCode = 0; /* start data (exit local */
					 /* busy state) */
    sigStnReq->fVersion = CLSI_VERSION;
    sigStnReq->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) sigStnReq);
}

void SendDataReq(word pCepId, byte* theData, size_t dataLength)
{
    DataReq_t* dataReq;
    
    dataReq = (DataReq_t*) 
	CLSIMsgCreateHeader(CLS_DATA | CLS_REQUEST,
			    offsetof(DataReq_t, fData) +
			    CLS_DATA_REQ_HDR_SZ +
			    dataLength,
			    CLS_LLC_LAYER,
			    CLS_P_CEP_ID,
			    pCepId);
    if (dataReq == NULL)
    {
	buginf("\nDLUTest: SendDataReq() couldn't get a message");
	return;
    }
    
    dataReq->fVersion = CLSI_VERSION;
    dataReq->fCorrelator = 0xdead;
    dataReq->fReserveLength = 0;
    memcpy(&dataReq->fData[0], theData, dataLength);
    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) dataReq);
}

void SendDiscReq(word pCepId)
{
    DisconnectReq_t* disconnectReq;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    disconnectReq = (DisconnectReq_t*) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_REQUEST,
						     offsetof(DisconnectReq_t, fCorrelator) -
						     CLS_DISCONNECT_REQ_HDR_SZ +
						     parmSize,
						     CLS_LLC_LAYER,
						     CLS_P_CEP_ID,
						     pCepId);
    if (disconnectReq == NULL)
    {
	buginf("\nDLUTest: SendDiscReq() couldn't get a message");
	return;
    }
    
    disconnectReq->fVersion = CLSI_VERSION;
    disconnectReq->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) disconnectReq);
}

void SendDiscRsp(word pCepId)
{
    DisconnectRsp_t* disconnectRsp;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    disconnectRsp = (DisconnectRsp_t*) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_RESPONSE,
					    offsetof(DisconnectRsp_t, fCorrelator) -
					    CLS_DISCONNECT_RSP_HDR_SZ +
					    parmSize,
					    CLS_LLC_LAYER,
					    CLS_P_CEP_ID,
					    pCepId);
    if (disconnectRsp == NULL)
    {
	buginf("\nDLUTest: SendDiscRsp() couldn't get a message");
	return;
    }
    
    disconnectRsp->fVersion = CLSI_VERSION;
    disconnectRsp->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) disconnectRsp);
}

void SendFlowReq(word pCepId)
{
    FlowReq_t* flowReq;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    flowReq = (FlowReq_t*) 
	CLSIMsgCreateHeader(CLS_FLOW | CLS_REQUEST,
			    offsetof(FlowReq_t, fCorrelator) -
			    CLS_FLOW_REQ_HDR_SZ +
			    parmSize,
			    CLS_LLC_LAYER,
			    CLS_P_CEP_ID,
			    pCepId);

    if (flowReq == NULL)
    {
	buginf("\nDLUTest: SendFlowReq() couldn't get a message");
	return;
    }
    
    flowReq->fCLSIHdr.fFlowAction = CLS_ACTION_STOP_FLOW;
    flowReq->fVersion = CLSI_VERSION;
    flowReq->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) flowReq);
}

void SendEchoData(word pCepId, TCLSIMsg* dataMsg)
{
    
    CLSIMsgSetPrimitiveType(dataMsg, CLS_REQUEST);
    dataMsg->fIdentifierType = CLS_P_CEP_ID;
    dataMsg->fIdentifier = pCepId;
    
    CLSIQueuePut(gCLSWriteQ, dataMsg);
    
}

void SendCloseStnReq(word pCepId)
{
    CloseStnReq_t* closeStnReq;
    
    closeStnReq = (CloseStnReq_t*) CLSIMsgCreateHeader(CLS_CLOSE_STN | CLS_REQUEST,
						     offsetof(CloseStnReq_t, fCorrelator) -
						     CLS_CLOSE_STN_REQ_HDR_SZ,
						     CLS_LLC_LAYER,
						     CLS_P_CEP_ID,
						     pCepId);
    if (closeStnReq == NULL)
    {
	buginf("\nDLUTest: SendCloseStnReq() couldn't get a message");
	return;
    }
    
    closeStnReq->fCLSIHdr.fUCepId = 0x00abcdef;
    closeStnReq->fCLSIHdr.fActionCode = CLS_ACTION_FORCED;
    
    closeStnReq->fVersion = CLSI_VERSION;
    closeStnReq->fCorrelator = 0xdead;

    CLSIQueuePut(gCLSWriteQ, (TCLSIMsg*) closeStnReq);
}

static void dlutest_stop(void)
{
    TCLSIMsg *aMsg;

    CLSClose(gCLSWriteQ);
    CLSIQueueDestroy(gDLUReadQ);
    CLSIQInitDestroy(gQInit);

    while ((aMsg = process_dequeue(gDLUInputQueue)) != NULL)
    {
	CLSIMsgDestroy(aMsg);
    }
    delete_watched_queue(&gDLUInputQueue);
    delete_watched_boolean(&gDLUStop);
    process_kill(THIS_PROCESS);
}

void dlu_primary(TDLUConn* theConn, TCLSIMsg* theMsg)
{
    word  retCode;
    hword primitive;
    
    primitive = CLSIMsgGetPrimitiveCode(theMsg);

    switch(primitive)
    {
    case CLS_REQ_OPNSTN | CLS_CONFIRM:
	retCode = CLSIMsgGetReturnCode(theMsg);
	if (retCode == CLS_OK)
	{
	    ReqOpnStnCnf_t* reqOpnCnf;
	    
	    reqOpnCnf = (ReqOpnStnCnf_t*) theMsg;
	    theConn->fPCepId = reqOpnCnf->fCLSIHdr.fPCepId;
	    SendIDReq(theConn->fPCepId, gDLUCannedXID, XIDSIZE);
	    if (theConn->fRole == DLUPPRIM)
		theConn->fRputState = DLUrput_NullXidSent;
	}
	CLSIMsgDestroy(theMsg);	
	break;

    case CLS_ID | CLS_INDICATION:
	if (CLSIMsgGetReturnCode(theMsg) == CLS_OK) 
	{
	    if (theConn->fRputState == DLUrput_Disconnected)
	    {
		SendIDReq(theConn->fPCepId, NULL, 0);
		theConn->fRputState = DLUrput_NullXidSent;
	    } else if (theConn->fRputState == DLUrput_NullXidSent) {
		SendConnectReq(theConn->fPCepId);
		theConn->fRputState = DLUrput_ConnectCnfWait;
	    }
	}
	else
	{
	    SendCloseStnReq(theConn->fPCepId);
	    theConn->fRputState = DLUrput_Disconnected;
	}
	
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_ID | CLS_CONFIRM:
	if (CLSIMsgGetReturnCode(theMsg) == CLS_OK)
	{
	    if (theConn->fRputState == DLUrput_Disconnected)
	    {
		SendConnectReq(theConn->fPCepId);
		theConn->fRputState = DLUrput_ConnectCnfWait;
	    }
	}
	else
	{
	    SendCloseStnReq(theConn->fPCepId);
	    theConn->fRputState = DLUrput_Disconnected;
	}
	
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_CONNECT | CLS_CONFIRM:
	if (theConn->fRputState != DLUrput_ConnectCnfWait)
	    buginf("\ndlu_primary: connect.cnf in state %d",
		   theConn->fRputState);
	
	if (CLSIMsgGetReturnCode(theMsg) == CLS_OK)
	{
	    theConn->fRputState = DLUrput_Connected;
	    theConn->fEchoCounter = 0;
	    SendSignalStn(theConn->fPCepId);
	    SendPongReq(theConn->fPCepId);
	}
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_PONG | CLS_CONFIRM:
    	theConn->fCredits = CLSIMsgGetNextWindow((PongCnf_t *)theMsg);
	buginf("\nPong %d credits", theConn->fCredits);
	CLSIMsgDestroy(theMsg);
	while (theConn->fNumSent < theConn->fNumFrames) {
	    if (theConn->fCredits-- <= 0) {
		SendPongReq(theConn->fPCepId);
		break;
	    }
	
	    SendDataReq(theConn->fPCepId, gDLUCannedData,
			sizeof(gDLUCannedData)-1);
	    theConn->fNumSent++;
	}
	break;
	
    case CLS_DATA | CLS_INDICATION:

	if (theConn->fEchoCounter++ < theConn->fNumFrames) {
	    SendEchoData(theConn->fPCepId, theMsg);
	} else
	{
	    if (theConn->fRputState == DLUrput_Connected)
	    {
		theConn->fRputState = DLUrput_DiscCnfWait;
		SendDiscReq(theConn->fPCepId);
	    }	    
	    CLSIMsgDestroy(theMsg);
	}
	if ((theConn->fEchoCounter % 10) == 0) {
	    buginf("\nDLUTEST: Primary received %d ECHOs", 
		   theConn->fEchoCounter);
	}
	break;

    case CLS_DISCONNECT | CLS_CONFIRM:
	if (theConn->fRputState != DLUrput_DiscCnfWait)
	    buginf("\ndlu_primary: disconnect.cnf in state %d",
		   theConn->fRputState);

	CLSIMsgDestroy(theMsg);
	SendCloseStnReq(theConn->fPCepId);
	theConn->fRputState = DLUrput_Disconnected;
	break;

    case CLS_DISCONNECT | CLS_INDICATION:
	{
	    SendCloseStnReq(theConn->fPCepId);
	    theConn->fRputState = DLUrput_Disconnected;
	}
	
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_CLOSE_STN | CLS_CONFIRM:
	retCode = CLSIMsgGetReturnCode(theMsg);
#ifdef NOTDEF
	if (retCode == CLS_OK)
	{
	    SendReqOpnStn(theConn->fUCepId, &theConn->fDestAddr, 0x04,
			  EO_OUTBOUND_DEFAULT, 0, NULL);
	}
#else
	gDLURunning = FALSE;
	process_set_boolean(gDLUStop, TRUE);
#endif
	CLSIMsgDestroy(theMsg);
	free(theConn);
	/*
	 * This is bogus - we should really make sure that all
	 * connections have been cleaned up
	 * PHIL 12/17/95
	 */
	dlutest_stop();
	break;
	
    default:
	buginf("\nDLUTest: Unexpected CLSI message");
	CLSIMsgDestroy(theMsg);
	break;
    }
}

void dlu_secondary(TDLUConn* theConn, TCLSIMsg* theMsg)
{
    word  retCode;
    hword primitive;
    
    primitive = CLSIMsgGetPrimitiveCode(theMsg);

    switch(primitive)
    {
    case CLS_REQ_OPNSTN | CLS_CONFIRM:
	retCode = CLSIMsgGetReturnCode(theMsg);
	if (retCode == CLS_OK)
	{
	    ReqOpnStnCnf_t* reqOpnCnf;
	    
	    reqOpnCnf = (ReqOpnStnCnf_t*) theMsg;
	    theConn->fPCepId = reqOpnCnf->fCLSIHdr.fPCepId;
	    if (gDLURole == DLUPSEC) {
		SendIDReq(theConn->fPCepId, NULL, 0);
	    } else
		SendIDRsp(theConn->fPCepId, gDLUCannedXID, XIDSIZE);
	}
	CLSIMsgDestroy(theMsg);	
	break;

    case CLS_ID | CLS_INDICATION:
	if ((theConn->fRole == DLUSECONDARY) ||
	    (theConn->fRputState == DLUrput_NullXidSent))
	    /* Send full XID */
	    SendIDRsp(theConn->fPCepId, gDLUCannedXID, XIDSIZE);
	else {
	    /* Start PU2.0 XID exchange by sending null xid */
	    SendIDReq(theConn->fPCepId, NULL, 0);
	    theConn->fRputState = DLUrput_NullXidSent;
	}
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_CONNECT | CLS_INDICATION:
	SendConnectRsp(theConn->fPCepId);
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_CONNECTED | CLS_INDICATION:
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_DATA | CLS_INDICATION:
	if (gDLUEcho) {
	SendEchoData(theConn->fPCepId, theMsg);
	} else {
	CLSIMsgDestroy(theMsg);
	}
	break;

    case CLS_FLOW | CLS_INDICATION:
	buginf("\nDlutest: Received FLOW.INDICATION");
	CLSIMsgDestroy(theMsg);
	break;

    case CLS_DISCONNECT | CLS_INDICATION:
	SendDiscRsp(theConn->fPCepId);
	CLSIMsgDestroy(theMsg);
	SendCloseStnReq(theConn->fPCepId);
	break;

    default:
	buginf("\nDLUTest: Unexpected CLSI message");
	CLSIMsgDestroy(theMsg);
	break;
    }
}

int dlu_rput(TCLSIQueue* dluReadQ, TCLSIMsg* theMsg)
{
    process_enqueue(gDLUInputQueue, theMsg);
    return(CLS_OK);
}

TDLUConn* DLUConnCreate(void)
{
    TDLUConn* this = malloc(sizeof(TDLUConn));
    if (this != NULL)
    {
	this->fNext = NULL;
	this->fUCepId = (word) this;
	this->fPCepId = 0;
	this->fRputState = DLUrput_Disconnected;
	this->fEchoCounter = 0;
	snpa_clear(&this->fDestAddr);
	this->fRole = DLUPRIMARY;
	this->fNumFrames = 0;
    }
    return(this);
}

void CallOut(byte* destAddr, byte destSap, int role, int numFrames)
{
    TDLUConn* aConn = DLUConnCreate();
    if (aConn == NULL)
	return;

    p_enqueue(&gDLUConnList, aConn);
    
    switch (gDLUPortType)
    {
    case DLU_Sdlc:
	if (gDLUAddrType == STATION_SDLC)
	    snpa_init(&aConn->fDestAddr, STATION_SDLC,
		      TARGET_UNICAST, &gDLUSDLCAddr);
	else if (gDLUAddrType == STATION_IEEE48)
	    snpa_init(&aConn->fDestAddr, STATION_IEEE48, 
		      TARGET_UNICAST, destAddr);
	break;
    case DLU_Qllc:
	snpa_copy(&gDluQllcRemote, &aConn->fDestAddr);
	break;
    case DLU_Vdlc:
    	snpa_copy(&gDLUVdlcRmac, &aConn->fDestAddr);
	break;
    case DLU_FrameRelay:
	{
	    uchar temp[2];
	    temp[0] = gDLUDLCI >> 8;
	    temp[1] = gDLUDLCI & 0xFF;
	    
	    snpa_init(&aConn->fDestAddr, STATION_FR10,
		      TARGET_UNICAST, temp);
	}
	break;
    case DLU_Smds:
	snpa_copy(&gDluSmdsRemote, &aConn->fDestAddr);
	break;
    case DLU_Ppp:
        {
        uchar temp;
	snpa_init(&aConn->fDestAddr, STATION_PPP,
		  TARGET_UNICAST, &temp);
        }
	break;
#ifdef NOT
    case DLU_ATM_1483:
	{
	    uchar temp[2];
	    temp[0] = gDLUVCD >> 8;
	    temp[1] = gDLUVCD & 0xFF;
	    
	    snpa_init(&aConn->fDestAddr, STATION_ATMVC,
		      TARGET_UNICAST, temp);
	}
	break;
#endif
    case DLU_Rsrb:
    default:
	snpa_init(&aConn->fDestAddr, STATION_IEEE48, TARGET_UNICAST, destAddr);
	break;
    }
    
    aConn->fRole = role;
    aConn->fNumFrames = numFrames;
    SendReqOpnStn(aConn->fUCepId, &aConn->fDestAddr, destSap,
		  EO_OUTBOUND_DEFAULT, 0, NULL);
}    

TDLUConn* DLUFindConn(TCLSIMsg* theMsg)
{
    TDLUConn* theConn;
    
    switch (theMsg->fIdentifierType)
    {
    case CLS_U_CEP_ID:
	for (theConn = (TDLUConn*) gDLUConnList.qhead;
	     theConn != NULL;
	     theConn = theConn->fNext)
	{
	    if (theConn->fUCepId == theMsg->fIdentifier)
		break;
	}
	break;
    default:
	theConn = NULL;
	break;
    }
    return(theConn);
}

void DLUConnHandleMsg(TDLUConn* theConn, TCLSIMsg* theMsg)
{
    hword primitive;
    
    primitive = CLSIMsgGetPrimitiveCode(theMsg);
    
    switch(primitive)
    {
    case CLS_FLOW | CLS_INDICATION:
	{
	    FlowInd_t* flowInd = (FlowInd_t*) theMsg;
	    theConn->fFlowLevel = flowInd->fCLSIHdr.fFlowLevel;
	    buginf("\nDLUConnHandleMsg: Flow Ind, level %d", theConn->fFlowLevel);
	}
	CLSIMsgDestroy(theMsg);
	break;
	
    default:
	{
	    switch (theConn->fRole)
	    {
	    case DLUPRIMARY:
	    case DLUPPRIM:
		dlu_primary(theConn, theMsg);
		break;
	    case DLUSECONDARY:
	    case DLUPSEC:
		dlu_secondary(theConn, theMsg);
		break;
		
	    default:
		buginf("\nDLUConnHandleMsg: bad role");
		CLSIMsgDestroy(theMsg);
		break;
	    }
	}
	break;
    }
}

static void NewConnection(TCLSIMsg* theMsg)
{
    IdStnInd_t* idStn;
    TDLUConn* aConn;
    hwaddrtype remoteMac;
    byte* rifPtr;
    byte rifLength;
    
    aConn = DLUConnCreate();
    if (aConn == NULL)
	return;
    
    aConn->fRole = gDLURole;
    aConn->fNumFrames = gDLUNumFrames;

    p_enqueue(&gDLUConnList, aConn);

    idStn = (IdStnInd_t*) theMsg;

    aConn->fPCepId = idStn->fCLSIHdr.fPCepId;

    CLSIIdStnGetRemoteMac(idStn, &remoteMac);
    rifPtr = CLSIIdStnGetRoute(idStn, &rifLength);
    if (rifPtr)
      ((srbroute_t *) rifPtr)->rc_dlf ^= RC_D;
    SendReqOpnStn(aConn->fUCepId, &remoteMac,
		  idStn->fRSap, EO_INBOUND_DEFAULT,
		  rifLength, rifPtr);

    CLSIMsgDestroy(theMsg);
}
				   

process dlutest_main (void)
{
    TDLUConn	*aConn;
    boolean have_event;      /*loop control*/
    ulong major, minor;      /*Event information*/
    TCLSIMsg* theMsg;

    gQInit = CLSIQInitCreate("DLUTest", dlu_rput);
    if (gQInit == NULL)
	return;
    gDLUReadQ = CLSIQueueCreate(gQInit, NULL);
    if (gDLUReadQ == NULL)
	return;
    gCLSWriteQ = CLSOpen(gDLUReadQ);
    if (gCLSWriteQ == NULL)
	return;

    /*
     * Initialize DLU test global variables here...
     */
    gDLUStop = create_watched_boolean(DLU_STOP,DLU_STOP_EVENT);
    process_set_boolean(gDLUStop, FALSE);
    process_watch_boolean(gDLUStop, ENABLE, RECURRING);
    gDLURunning = TRUE;
    queue_init(&gDLUConnList, 0);
    gDLUInputQueue = create_watched_queue(DLU_INPUT_Q,0,
					  DLU_INPUT_Q_EVENT);
    process_watch_queue(gDLUInputQueue, ENABLE, RECURRING);

    /*
     * Get the ball rolling
     */
    SendEnable();

    while (gDLURunning == TRUE)
    {
	process_wait_for_event();
	have_event = process_get_wakeup(&major,&minor);  /*Event info*/

	theMsg = NULL;
	switch(major) {
	case QUEUE_EVENT:
	    switch(minor) {
	    case DLU_INPUT_Q_EVENT:
		theMsg = process_dequeue(gDLUInputQueue);
		break;
	    default:
		break;
	    }
	case BOOLEAN_EVENT:
	    switch(minor) {
	    case DLU_STOP_EVENT:
		gDLURunning = FALSE;
		process_set_boolean(gDLUStop, FALSE);
		break;
	    default:
		break;
	    }
	default:
	    break;
	}
	if (theMsg != NULL) {
	    /*
	     * Process message
	     */
	    word  retCode;
	    hword primitive;
      
	    primitive = CLSIMsgGetPrimitiveCode(theMsg);
	    
	    switch(primitive)
	    {
	    case CLS_ENABLE | CLS_CONFIRM:
		retCode = CLSIMsgGetReturnCode(theMsg);
		if (retCode == CLS_OK) {
		    CLSPortID	portId;

		    portId = ((EnableCnf_t *)theMsg)->fPortId;
		    gDluIdb = CLSIPortIdGetSwIdb(&portId);
		    SendActSap();
		} else
		    buginf("\nDLUTest Enable failed error 0x%x",
			   retCode);
		
		CLSIMsgDestroy(theMsg);
		break;
		
	    case CLS_ACTIVATE_SAP | CLS_CONFIRM:
		retCode = CLSIMsgGetReturnCode(theMsg);
		if (retCode == CLS_OK)
		{
		    ActSapCnf_t* actSapCnf;
		    uchar destSap = 0x04;
		    
		    actSapCnf = (ActSapCnf_t*) theMsg;
		    gDLUPSapId = actSapCnf->fCLSIHdr.fPSapId;
		    if (gDLUVdlcRSap != 0) {
		    	destSap = gDLUVdlcRSap;
		    }
		    if (gDLURole == DLUPRIMARY || gDLURole == DLUPSEC ||
			gDLUCallOut)
			CallOut(gDLUDestAddr, destSap, gDLURole,
				gDLUNumFrames);
		}
		else
		    buginf("\nDLUTest ActSap failed error 0x%x",
			   retCode);
		
		CLSIMsgDestroy(theMsg);
		break;
		
	    default:
		{
		    TDLUConn* theConn;
		    
		    theConn = DLUFindConn(theMsg);
		    if (theConn != NULL)
		    {
			DLUConnHandleMsg(theConn, theMsg);
		    }
		    else
		    {
			if (primitive == (CLS_ID_STN | CLS_INDICATION))
			{
			    NewConnection(theMsg);
			}
			else
			{
			    buginf("\nDLUTest error bad u_cep_id");
			}
		    }
		}
		break;
	    }
	}
    }

    /*
     * If we got here, then gDLURunning has been set to FALSE by
     * something. Probably the "dlutest stop" command. Time to start
     * cleaning up.
     */

    if (gDLUConnList.qhead != NULL) {
	while ((aConn = p_dequeue(&gDLUConnList)) != NULL) {
	    if (aConn->fRputState == DLUrput_Connected) {
		SendDiscReq(aConn->fPCepId);
	    } else {
		SendCloseStnReq(aConn->fPCepId);
	    }
	}
    } else {
	dlutest_stop();
    }
}

void dlutest_command (parseinfo *csb)
{
   int dlutest_pid;
   static ushort qllcPartner[] = {
       0x4000,
       0x4567,
       0x4567
   };

   switch (GETOBJ(int,1)) {
     case DLUSTART:
       ieee_copy((unsigned char*)GETOBJ(hwaddr,1)->addr, gDLUDestAddr);
       gDLURole = GETOBJ(int,2);
       gDLUNumFrames = GETOBJ(int,3);
       gDLUPortType = GETOBJ(int, 4);
       
       if (ISOBJ(idb,1)) {
	   gDluIdb = GETOBJ(idb,1);
       }

       gDLUEcho = GETOBJ(int,8);

       if (GETOBJ(int,10) &&  
	   !valid_XID(GETOBJ(string,1), &gDLUCannedXID[2], XID_SIZE) ) {
	   return;
       }
       gDLUCallOut = GETOBJ(int,11);


	switch (gDLUPortType) {

	case DLU_Rsrb:
	case DLU_TokenRing:
	case DLU_EtherNet:
	case DLU_Fddi:
	case DLU_Ppp:
	case DLU_ATM_1483:
	    break;

	case DLU_Sdlc:
	   gDLUSDLCAddr = GETOBJ(int, 5);
	   if (GETOBJ(int, 6))
	       gDLUAddrType = STATION_SDLC;
	   else gDLUAddrType = STATION_IEEE48;
	case DLU_Smds:
	   snpa_copy(GETOBJ(hwaddr,2), &gDluSmdsRemote);
	   break;
       case DLU_Qllc:
	   gDluQllcUseSubaddr = TRUE;

	   snpa_init(&gDluQllcPartner, STATION_IEEE48, TARGET_UNICAST,
		     (uchar *)qllcPartner);

	   snpa_init(&gDluQllcSapAddress, STATION_X121, TARGET_UNICAST,
		     GETOBJ(paddr,2)->x121_addr);

	   if (ISOBJ(hwaddr, 2)) {
	   snpa_copy(GETOBJ(hwaddr,2), &gDluQllcSapVmacStart);
	   } else {
	       snpa_clear(&gDluQllcSapVmacStart);	       
	   }
	   if (GETOBJ(int,7)) {
	       snpa_init(&gDluQllcRemote, 
			 STATION_X121, TARGET_UNICAST,
			 GETOBJ(paddr,3)->x121_addr);
	   } else {
	       snpa_copy(GETOBJ(hwaddr,3), &gDluQllcRemote);
	   }
	   break;

       case DLU_FrameRelay:
	   gDLUDLCI = GETOBJ(int, 6);
	   gDLUVCD = GETOBJ(int, 5);
	   break;
	   
       case DLU_Vdlc:
	   gDLUVrn = GETOBJ(int,5);
	   snpa_copy(GETOBJ(hwaddr,1), &gDLUVdlcLmac);
	   snpa_copy(GETOBJ(hwaddr,2), &gDLUVdlcRmac);
	   gDLUVdlcLSap = GETOBJ(int,6);
	   gDLUVdlcRSap = GETOBJ(int,7);
	   break;	
       }

       dlutest_pid = process_create(dlutest_main,"DLUTest program",
                                    LARGE_STACK,PRIO_NORMAL);
       break;

     case DLUSTOP:
       gDLURunning = FALSE;		/* Tell process to clean up */
       process_set_boolean(gDLUStop, TRUE); /* Tell process to stop*/
       break;
   } /* endSwitch DLUSTART/DLUSTOP */

}

/* extra routines for QLLC building blocks
 * Activate Sap, and 
 * Request Open Station Request
 */

void BuildQActSapReq(ActSapQLLCParms_t *fParms)
{
	QSapT	*qsap;
	
	qsap = &fParms->fQActSap;
	QClearAllFlags(&qsap->fQSapFlags);

	snpa_copy_to_buffer(&gDluQllcSapAddress, 
			    qsap->fDispatch.fQSapSubaddress);


	if (snpa_get_type(&gDluQllcSapVmacStart) == STATION_IEEE48) {
	QSetVmac(&qsap->fQSapFlags);	
	QSetPartner(&qsap->fQSapFlags);
	snpa_copy_to_buffer(&gDluQllcSapVmacStart, fParms->fVmacStart);
	snpa_copy(&gDluQllcPartner, &qsap->fPartner);
	fParms->fNumVmacs =1;
	}
	qsap->fMaxIfield = 0;
	return;
}

/* BuildQROSR
 *
 * Fill out the form for Request Open Station Request
 */
 
void BuildQROSR(ReqOpnStnQLLCParms_t *fParms, hwaddrtype *remoteAddr)
{
	QClearAllFlags(&fParms->fQReqOpenStnFlags);
	QSetTRSaps(&fParms->fQReqOpenStnFlags);
	fParms->fDestSap = SAP_IBM_SNA;
	fParms->fSrcSap = SAP_IBM_SNA;

	snpa_copy(remoteAddr, &fParms->fDestAddr);
	snpa_copy(&gDluQllcPartner, &fParms->fSrcAddr);
	return;
}

/* $Id: dlcvdlc_private.h,v 3.2.60.4 1996/08/30 20:31:28 pmorton Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcvdlc_private.h,v $
 *------------------------------------------------------------------
 * Internal CLS connections between two CLS-DLUS so that one DLU can
 * act as a DLC for the other DLU
 *
 * August, 1995		Phil Morton
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *
 * CLEARCASE SUPERCEDES THE LOG
 *
 * CSCdi71922 is a major design shift
 *
 * Here are some notes from the DDTs.
 *
 * To explain it briefly, I did vdlc as a stateless piece
 * of code.  To fix the ddts I need to define states so
 * that I can tell one DLU if the other has closed down
 * the connection.  You can't just pass DISC.IND, because
 * the CEPs may be in the process of XID negotiation,
 * or some other state other than connected.
 * So I added states, and hence a rather large diff.
 * 
 * Let me explain the way Close Stn Req works.
 *
 * If a Vcep (say VCep A) receives Close Stn. Req it does everything
 * it can to get its partner (say VCep B) to close too.  (It would be
 * so much easier with a CLS_CLOSE_STN_IND primitive.)  This is done
 * by calling VCepEventInput for VCep B with VDLC_CLOSE_STN_IND.  This
 * puts VCep B into CLOSING state, and then VCep A's Close Stn. Req
 * always completes with VCepDestroy, and it's no longer there.
 *
 * Note A.
 *
 * Since we don't have a Close Stn. Ind, Cep B needs to send different
 * messages to its DLU depending on what it's doing.  For instance if
 * it's in the middle of connecting, it wants to do a Connect Confirm
 * (error), if it's waiting for an XID response it needs to time that
 * out with NO_REM_STN, etc.
 *
 * This is why I needed to add states.  So there has to be a way to
 * add the states.  There are a numbe of things that make a VCep
 * change state.  Amongs opther things there are some messages that
 * come from the DLU that make that happen.  The code to do this is in
 * VCepSetStateFromClsOpCode.
 *
 * Another thing that makes a VCep change state is when its partner
 * gets a message.  In this case the state is different, to reflect
 * the it is sending a message to the DLU.  It uses the routine
 * VCepSetStateFromDlcOpcode.
 *
 * I suppose I could have made them both adjust states with one
 * routine but it would have tied them together more than I want to.
 * One of the fun things about VDLC is that theVCep's are largely
 * independent of each other.  I think that makes the code more
 * stable.
 * ****************************************************************
 *
 * $Log: dlcvdlc_private.h,v $
 * Revision 3.2.60.4  1996/08/30  20:31:28  pmorton
 * CSCdi67720:  LNM over DLSw broken in 11.2
 * Branch: California_branch
 * Merge in changes from 11.1 version.
 *
 * Revision 3.2.60.3  1996/08/16  07:05:40  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/17  10:42:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/04/26  01:53:02  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.2  1996/04/03  13:54:59  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.48.1  1996/03/28  08:19:41  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:10:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:41:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:41:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:56:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/25  21:19:20  pmorton
 * Add placeholder for new feature
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DLCVDLC_PRIVATE_H__
#define __DLCVDLC_PRIVATE_H__

static process                  vdlc_background (void);
static list_header              VdlcBgList;
static watched_boolean *VdlcBgList_look; /*TRUE when element on VdlcBgList*/
static mgd_timer		VdlcMasterTimer;

/*
 * Events 
 */
#define VDLCBGLIST "VdlcBgList flag"
/* 
 *  minor codes for events 
 */
typedef enum vdlc_events_ {
    VDLCBGLIST_EVENT
} vdlc_events;  

/* Types for VDLC */

STRUCTURE_REF(VPortT);
STRUCTURE_REF(VSapT);
STRUCTURE_REF(VCepT);

typedef enum {
    VDLC_IDLE,
    VDLC_NO_TEST,	/* wanted to explore, but couldn't get a packet */
    VDLC_OPENING,	/* Handling Request Open Station Request	*/
    VDLC_HALF_OPEN,	/* Received test Response, but not yet partnered */
    VDLC_OPEN,		/* Both partners present			*/
    VDLC_XID_RSP_PENDING, /* Cep received ID req from DLU, awaiting 
			   * ID Rsp from partner
			   */

    /* transitional states for connect/disconnect */

    VDLC_RX_CONNECT_REQ,	/* from DLU */
    VDLC_TX_CONNECT_IND,	/*   to DLU */
    VDLC_RX_DISC_REQ,		/* from DLU */
    VDLC_TX_DISC_IND,		/*   to DLU */

    VDLC_CONNECTED,		/*
				 * Connected, CONN.REQ/CONN.IND -
				 * CONN.RSP/CONN.CNF completed
				 */

    VDLC_CLOSING,		/* Partner is closed, we're waiting for DLU to close us */
    VDLC_NUM_STATES	/* counts the number of states			*/
} VdlcStateT;

/* Events for the VCep state machine */

typedef enum {

    /* events based on messages sent to partner */

    VDLC_CONNECT_IND,	/* partner got CONNECT REQ
			 *  - should send CONNECT_IND to DLU
			 *  - will expect CONNECT RSP
			 */

    VDLC_CONNECT_CNF,	/* partner got CONNECT RSP
			 * should send CONNECT_CNF to DLU
			 * and everything's CONNECTED
			 */

    VDLC_DISC_IND,	/* partner got DISCONNECT REQ
			 * should send DISC_IND to DLU,
			 * and get DISC RSP
			 */

    VDLC_DISC_CNF,	/* partner got DISCONNECT RSP
			 * should send DISC_CNF to DLU
			 * Connection is there, but CLOSED
			 */

    VDLC_CLOSE_STN_IND,	/* CLOSE STN REQ */

    /* timer */
    VCEP_TIMER,
} VCepEventT;

/*
 * vdlc timer types 
 */

enum {
    VDLC_TIMER
} ;

/*
 * Timer constants
 */

#define VDLC_DEFAULT_OPEN_TIMER 	(10 * ONESEC)
#define VDLC_BUFFER_WAIT		(ONESEC)
#define VDLC_DEFAULT_MAX_RETRIES	0 /* retry forever */

#define VDLC_DEFAULT_MAXMTU		RC_LF4472_VAL

typedef enum {
    VDLC_NORMAL_PRIORITY,
} VdlcPriorityT;

typedef boolean VPortWalkT(VPortT *port, va_list argptr);
typedef boolean VSapWalkT(VSapT *sap, va_list argptr);
typedef boolean VCepWalkT(VCepT *cep, va_list argptr);

static void VdlcWput(DlcPCepIdT pCepId, DlcOpcodeT opcode,
		     ClsDlcParamBlockT *clsParamBlock);

static void VdlcDefaultPort(tr_vgrouptype *vrg, uchar *vmac, boolean sense);
static void VdlcTellPartner(VCepT *partner, DlcOpcodeT dlcOpcode, 
			     ClsDlcParamBlockT *clsBlock);
static ClsUSapIdT VdlcGetBridgingSapId(hwidbtype *idb);

static void VPrintDteString(uchar *dtestring);
/*
 *
 * Objects:
 *
 * VrgT		Ring Group Extensions
 * VPortT	Vdlc Port - based on vmac address
 * VSapT	VDLC Sap  - addressed by a one-byte extension
 * VCepT	Connection end point - addressed the same wasy LLC2/TR
 *
 */

/************************************************************************
 *                                                                      *
 * VrgVdlc								*
 *									*
 * Extentions  to the Ring Group for VLDC's purposes			*
 *                                                                      *
 ************************************************************************
 */

struct VrgT_ {
    list_header		fList;		/* Anchor for VPorts		*/
    VPortT		*fDefaultPort;	/* Identified by source-bridge ring-group vmac */
    VSapT		*fBridgingSap;	/* ActivateRing	*/
    tr_vgrouptype	*fVrg;
};


static VrgT *VrgCreate(tr_vgrouptype *vrg);
#ifdef NOT_NEEDED
static void VrgDestroy(VrgT *vrg);
static void VPortDestroy(VPortT *port);
static VPortT *VrgLookupPortByIdb(VrgT *vrg, idbtype *idb);
#endif

static boolean VrgAddPort(VrgT *vrg, VPortT *port);
static VPortT *VrgLookupPortByPortId(CLSPortID* portID);

static INLINE VPortT *VrgGetDefaultPort(VrgT *vrg);
static inline void VrgRemovePort(VrgT *vrg, VPortT *port);
static INLINE VPortT *VrgLookupPortByVmac(VrgT *vrg, uchar *vmac);
static INLINE TCLSIMsg *VPortGetRingReq(idbtype *swidb);

/************************************************************************
 *                                                                      *
 * Port                                                                 *
 *                                                                      *
 * Identified by Virtual Mac Address                                    *
 * Anchor for a list of SAPs                                            *
 *                                                                      *
 ************************************************************************
 */

#define MAX_VDLCNAME	10		/* "VDLC"+ "ringnumber" + '\0' */
struct VPortT_ {
    list_element	fLink;		/* linked list to other VPorts  */
    uchar		fVmac[IEEEBYTES];
    idbtype		*fIdb;		/* used by DLSw+ and other DLUs	*/
    list_header		fList;		/* anchor for the Saps		*/
    VrgT		*fVrg;		/* anchor			*/
    uchar		fName[MAX_VDLCNAME];
};

/* Methods */

static VPortT *VPortCreate(uchar *vmac, VrgT *vdlc);

static int VPortCompareMac(void *runner, void *port);
static int VPortCompareIdb(void *runner, void *port);

static INLINE void VPortSetVmac(VPortT *port, uchar *vmac);
static INLINE uchar *VPortGetVmac(VPortT *port);
static INLINE boolean VPortAddSap(VPortT *port, VSapT *sap);
static INLINE void VPortRemoveSap(VPortT *port, VSapT *sap);
static INLINE VSapT *VPortLookupSapByValue(VPortT *port, uchar sapValue);
static inline VSapT *VportLookupSapByUSapId(VPortT *port, ClsUSapIdT uSapId);
static inline VPortT *VPortGetNext(VPortT *port);
static INLINE idbtype *VPortGetIdb(VPortT *port);
static INLINE void VPortSetVrg(VPortT *port, VrgT *vrg);
static INLINE VrgT *VPortGetVrg(VPortT *port);

/************************************************************************
 *									*
 * SAP                                                                  *
 *                                                                      *
 * Service Access Point                                                 *
 *                                                                      *
 * VDLC SAPs are one byte quantities, just like LLC2 SAPs               *
 *									*
 ************************************************************************
 */

struct VSapT_ {
    list_element	fLink;		/* linked list to other Saps 	*/
    avl_node_type	*fCepList;	/* Ceps owned by this Sap 	*/
    VPortT		*fPort;		/* port that owns this Sap	*/
    ClsUSapIdT		fUSapId;
    VdlcPriorityT	fPriority;
    uchar		fSapValue;
    ulong		fFlags;
};

/* Methods */

static VSapT *VSapCreate(ClsUSapIdT uSapId, uchar fSapValue, 
		  VPortT *port, VdlcPriorityT priority);
static void VSapDestroy(VSapT *sap);

static VCepT *VSapLookupCepByUCepId(VSapT *sap, ClsUCepIdT uCep);
static int VSapCompareValue(void *runner, void *sap);
static int VSapCompareUSapId(void *runner, void *sap);
static boolean VSapSendUControlStnInd(VSapT *sap, VCepT *cep, uchar control);
static void vdlcUDataStnReq(ClsDlcParamBlockT *clsBlock,
			    DlcUdataStnT *dlcUDataStn,
			    LlcUdataStnT *llcUDataStn);
static inline void VSapModify(VSapT *sap, VdlcPriorityT priority);

static INLINE void VSapSetPort(VSapT *sap, VPortT *port);
static INLINE VPortT *VSapGetPort(VSapT *sap);
static INLINE boolean VSapAddCep(VSapT *sap, VCepT *cep);
static INLINE boolean VSapRemoveCep(VSapT *sap, VCepT *cep);
static INLINE VCepT *VSapLookupCep(VSapT *sap, uchar *dtestring);
static INLINE VSapT *VSapGetNext(VSapT *sap);
static INLINE void VSapSetSapValue(VSapT *sap, uchar value);
static INLINE uchar VSapGetSapValue(VSapT *sap);
static INLINE VCepT *VSapGetFirstCep(VSapT *sap);
static INLINE ClsUSapIdT VSapGetUSapId(VSapT *sap);
static INLINE void VSapSetUSapId(VSapT *sap, ClsUSapIdT uSapId);
static INLINE void VSapSetPriority(VSapT *sap, VdlcPriorityT priority);

/************************************************************************
 *									*
 * Cep                                                                  *
 *                                                                      *
 * Connection End Point                                                 *
 *									*
 ************************************************************************
 */

struct VCepT_ {
    avl_node_type	fAvlLink;	/* for tree to organize CEPs */
    void		*fUCepId;	/* correlate to CLS core Cep */
    CepT		fCep;		/* common dlc info for any
                                         * kind of Cep
                                         */
    list_element	fLink;		/* on inbound queue for
                                         * Vdlc_background
                                         */
    VCepT		*fPartner;	/* the one you're with	     */
    queuetype		fTxQueue;	/* This CEP's Transmit Queue */
    queuetype		*fRxQueue;	/* partner Cep's Transmit Queue
					 * The actual queue is in the
                                         * partner's fCep
					 */
    VSapT		*fSap;		/* mommy		     */
    VdlcStateT		fState;
    mgd_timer		fTimer;		/* timeout if open fails     */
    ulong		fTimeout;	/* amount of time to wait
                                         * for each timeout
                                         */
    ushort		fRetryCount;	/* number of retries so far  */
    ushort		fMaxTries;	/* Max # of retries          */    
    paktype		*fPacket;
    VdlcPriorityT	fPriority;
    uchar		fDteString[LLC_MAX_DTE_SIZE]; /* dmac, dsap,
                                                       * ssap, smac
                                                       */
    llctype		*fLlc;		/* local TR connection	     */
    ulong		fFlags;		/* you always need flags     */
};

/* Methods */

static VCepT *VCepCreate(ClsUCepIdT uCepId, VSapT *sap, uchar *dtestring,
			 VdlcPriorityT priority, ulong flags);
static void  VCepDestroy(VCepT *cep);

static void VCepBuildPartnerDteString(uchar *dtestring, uchar *partnerString);
static void VCepSetState(VCepT *cep, VdlcStateT state);
static void VCepSetStateFromDlcOpcode(VCepT *cep, DlcOpcodeT opcode);
static void VCepSetStateFromClsOpcode(VCepT *cep, ClsOpcodeT opcode);

static boolean VCepRendezvous(VCepT *cep, VCepT *partnerCep);
static INLINE VdlcStateT VCepGetState(VCepT *cep);
#ifdef NOT_NEEDED
static void VCepExploreLocal(VCepT *cep);
#endif
static void VCepTimerStart(VCepT *cep, int interval);
static void VCepTimerStop(VCepT *cep);
static void VCepNoPartner(VCepT *cep);
static ClsDlcParamBlockT* VCepBuildParamBlock (VCepT *cep, ClsOpcodeT opcode,
					ClsDlcParamBlockT *paramBlock,
					ClsErrorCodeT ClsDlcErrorCode);
static enum avl_compare VCepCompare(avl_node_type *a, avl_node_type *b);
static inline boolean VCepSetLlc(VCepT *cep, llctype *llc);
static inline void VCepClearLlc(VCepT *cep);
static void VCepEventInput(VCepT *cep, VCepEventT event);
static void VCepProcessCloseStnInd(VCepT *cep);
static void VCepProcessTimer(VCepT *cep);
static INLINE void VCepHandleClosingMsgs(VCepT *cep, DlcOpcodeT opcode,
					 ClsDlcParamBlockT *clsBlock);	    
static boolean VCepHandleTestResponse(VCepT *cep, va_list argptr);
static boolean VCepSendToPartnerSap(VCepT *cep, DlcOpcodeT dlcOpcode,
				    ClsDlcParamBlockT *clsBlock);
static INLINE void VCepHandleClosingMsgs(VCepT *cep, DlcOpcodeT opcode,
					 ClsDlcParamBlockT *clsBlock);	    
static boolean VCepSendTest(VCepT *cep);
static char *VCepPrState(VdlcStateT state);
static inline void  VCepModify(VCepT *cep, VdlcPriorityT priority);
static INLINE VSapT *VCepGetSap(VCepT *cep);
static INLINE void VCepSetSap(VCepT *cep, VSapT *sap);
static INLINE VCepT *VCepGetPartner(VCepT *cep);
static INLINE void VCepSetPartner(VCepT *cep, VCepT *partner);
static INLINE llctype *VCepGetLlc(VCepT *cep);
static INLINE void VCepBgEnqueue(VCepT *cep);
static INLINE void VCepBgRemove(VCepT *cep);
static INLINE paktype *VCepGetRxPacket(VCepT *cep);
static INLINE CepT *VCepGetDlcCep(VCepT *cep);
static INLINE uchar *VCepGetDteString(VCepT *cep);
static INLINE ClsUCepIdT VCepGetUCepId(VCepT *cep);
static INLINE list_header *VCepPriorityToList(VCepT *cep);
static INLINE boolean VCepRxQueueEmpty(VCepT *cep);
static INLINE void VCepSetPriority(VCepT *cep, VdlcPriorityT priority);
static INLINE void VCepSetUCepId(VCepT *cep, ClsUCepIdT uCepId);
static INLINE void VCepSetDteString(VCepT *cep, uchar *dtestring);

/************************************************************************
 *									*
 *			INLINE Vrg Methods				*
 *									*
 ************************************************************************
 */

static INLINE VPortT *VrgGetDefaultPort (VrgT *vrg)
{
    return(vrg->fDefaultPort);
}

static inline void VrgRemovePort (VrgT *vrg, VPortT *port)
{
    list_remove(&vrg->fList, &port->fLink, NULL);
}


static INLINE VPortT *VrgLookupPortByVmac (VrgT *vrg, uchar *vmac)
{
    VPortT testPort;
	
    VPortSetVmac(&testPort, vmac);
    return ((VPortT *)list_lookup(&vrg->fList, &testPort.fLink,
				  &testPort, VPortCompareMac));
}


/************************************************************************
 *									*
 *			INLINE Vport Methods				*
 *									*
 ************************************************************************
 */

static INLINE void VPortSetVmac(VPortT *port, uchar *vmac)
{
    ieee_copy(vmac, port->fVmac);
}

static INLINE uchar *VPortGetVmac (VPortT *port)
{
    return(port ? port->fVmac : NULL);
}

/* VPortAddSap
 *
 * Add a VSap
 */

static INLINE boolean VPortAddSap (VPortT *port, VSapT *sap)
{	
    return(list_insert(&port->fList, &sap->fLink,
			       sap, VSapCompareValue));
}

static INLINE void VPortRemoveSap (VPortT *port, VSapT *sap)
{
    list_remove(&port->fList, &sap->fLink, NULL);
}

static INLINE VSapT *VPortLookupSapByValue (VPortT *port, uchar sapValue)
{
    VSapT	testSap;
	
    VSapSetSapValue(&testSap, sapValue);
    return(list_lookup(&port->fList, &testSap.fLink, &testSap, VSapCompareValue));
}

static inline VSapT *VportLookupSapByUSapId (VPortT *port, ClsUSapIdT uSapId)
{
    VSapT	testSap;
	
    VSapSetUSapId(&testSap, uSapId);
    return(list_lookup(&port->fList, &testSap.fLink, &testSap, VSapCompareUSapId));
}

static inline VPortT *VPortGetNext (VPortT *port)
{
    return( port ? (VPortT *)(LIST_NEXT_ELEMENT(&port->fLink)) : NULL);
}


static INLINE idbtype *VPortGetIdb (VPortT *port)
{
    return(port  ? port->fIdb : NULL);
}

static INLINE void VPortSetVrg (VPortT *port, VrgT *vrg)
{
    port->fVrg = vrg;
}


static INLINE VrgT *VPortGetVrg (VPortT *port)
{
    return (port? port->fVrg : NULL);
}

/************************************************************************
 *									*
 *			INLINE VSap Methods				*
 *									*
 ************************************************************************
 */

static inline void VSapModify (VSapT *sap, VdlcPriorityT priority)
{
    VSapSetPriority(sap, priority);
}


static INLINE void VSapSetPort (VSapT *sap, VPortT *port)
{
    sap->fPort = port;
}

static INLINE VPortT *VSapGetPort (VSapT *sap)
{
    return(sap? sap->fPort : NULL);
}

static INLINE boolean VSapAddCep (VSapT *sap, VCepT *cep)
{
    boolean balancing;

    return ((boolean)avl_insert(&sap->fCepList, &cep->fAvlLink, &balancing, 
				VCepCompare));
}

static INLINE boolean VSapRemoveCep (VSapT *sap, VCepT *cep)
{
    boolean balancing;

    return( (boolean)avl_delete(&sap->fCepList, &cep->fAvlLink, &balancing, 
				VCepCompare));
}

static INLINE VCepT *VSapLookupCep (VSapT *sap, uchar *dtestring)
{
    VCepT	testCep;
	
    VCepSetDteString(&testCep, dtestring);
    return ((VCepT *) avl_search(sap->fCepList, &testCep.fAvlLink,
				 VCepCompare));
}

static inline VSapT *VSapGetNext (VSapT *sap)
{
    return (sap ? (VSapT *)LIST_NEXT_ELEMENT(&sap->fLink) : NULL);
}

static INLINE void VSapSetSapValue (VSapT *sap, uchar value)
{
    sap->fSapValue = value;
}

static INLINE uchar VSapGetSapValue (VSapT *sap)
{
    return(sap ? sap->fSapValue : 0xFF);
}

static INLINE VCepT *VSapGetFirstCep (VSapT *sap)
{
    return (sap ?(VCepT *)(avl_get_first(sap->fCepList)) :
	    NULL);
}

static INLINE ClsUSapIdT VSapGetUSapId (VSapT *sap)
{
    return(sap ? sap->fUSapId : NULL);
}

static INLINE void VSapSetUSapId (VSapT *sap, ClsUSapIdT uSapId)
{
    sap->fUSapId = uSapId;
}

static INLINE void VSapSetPriority (VSapT *sap, VdlcPriorityT priority)
{
    sap->fPriority = priority;
}

/************************************************************************
 *									*
 *			INLINE VCep Methods				*
 *									*
 ************************************************************************
 */

static inline void  VCepModify (VCepT *cep, VdlcPriorityT priority)
{
    VCepSetPriority(cep, priority);
}

static INLINE VdlcStateT VCepGetState (VCepT *cep)
{
    return (cep->fState);
}

static INLINE VSapT *VCepGetSap (VCepT *cep)
{
    return (cep->fSap);
}

static INLINE void VCepSetSap (VCepT *cep, VSapT *sap)
{
    cep->fSap = sap;
}

static INLINE VCepT *VCepGetPartner (VCepT *cep)
{
    return (cep->fPartner);
}

static INLINE void VCepSetPartner (VCepT *cep, VCepT *partner)
{
    cep->fPartner = partner;
    cep->fRxQueue = &partner->fTxQueue;
}

static INLINE llctype *VCepGetLlc (VCepT *cep)
{
    return (cep->fLlc);
}

static inline boolean VCepSetLlc (VCepT *cep, llctype *llc)
{
    cep->fLlc = llc;
    llc->u_cep_id = cep->fUCepId;
    return (TRUE);
}

static inline void VCepClearLlc (VCepT *cep)
{
    if (cep->fLlc) {
	llc2_close(cep->fLlc);
	cep->fLlc = NULL;
    }
}

static INLINE void VCepBgEnqueue (VCepT *cep)
{
    list_header	*list;
    
    list = VCepPriorityToList(cep);
    if (!LIST_ELEMENT_QUEUED(&cep->fLink)) {
	list_enqueue(list, &cep->fLink, cep);
	process_set_boolean(VdlcBgList_look, TRUE); /* Awaken process */ 
    }
}

static INLINE void VCepBgRemove (VCepT *cep)
{
    list_header	*list;
    
    list = VCepPriorityToList(cep);
    if (LIST_ELEMENT_QUEUED(&cep->fLink)) {
	list_remove(list, &cep->fLink, cep);
    }
}

static INLINE paktype *VCepGetRxPacket (VCepT *cep)
{
    return (p_dequeue(cep->fRxQueue));
}

static INLINE CepT *VCepGetDlcCep (VCepT *cep)
{
    return (&cep->fCep);
}

static INLINE void VCepSetDteString (VCepT *cep, uchar *dtestring)
{
    memset(cep->fDteString, 0, sizeof(cep->fDteString));
    memcpy(cep->fDteString, dtestring, sizeof(cep->fDteString));
}

static INLINE uchar *VCepGetDteString (VCepT *cep)
{
    return (cep->fDteString);
}

static INLINE ClsUCepIdT VCepGetUCepId (VCepT *cep)
{
    return (cep->fUCepId);
}

/* VCepPriorityToList
 * 
 * As the priority scheme gets more involved we will develop
 * more sophisticated ideas about different lists for CEPs 
 */

static INLINE list_header *VCepPriorityToList (VCepT *cep)
{
    return(&VdlcBgList);
}

static INLINE boolean VCepRxQueueEmpty (VCepT *cep)
{
    return (QUEUEEMPTY(cep->fRxQueue));
}

static INLINE void VCepSetPriority (VCepT *cep, VdlcPriorityT priority)
{
	cep->fPriority = priority;
}


static INLINE void VCepSetUCepId(VCepT *cep, ClsUCepIdT uCepId)
{
    cep->fUCepId = uCepId;
}

/*
 * Constants to define the flow control characteristics of
 * a VDLC connection
 */

#define VDLC_DEFAULT_HOLDQ	10	/* maximum number of packets in the queue */
#define VDLC_TXQ_LOW_WATER	20	/* percentage */
#define VDLC_TXQ_HIGH_WATER	70	/* percentage */
#define VDLC_CLS_FLOW_IND_DAMP	2	/* damping zone - scalar */

#endif					/* __DLCVDLC_PRIVATE_H__ */

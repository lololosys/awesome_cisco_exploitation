/* $Id: qllc_private.h,v 3.2.62.5 1996/09/15 02:56:06 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc_private.h,v $
 *------------------------------------------------------------------
 * qllc_private.h - things only known to qllc and its acomplices
 *
 * February 1995			Phil Morton
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_private.h,v $
 * Revision 3.2.62.5  1996/09/15  02:56:06  rbatz
 * CSCdi60999:  QLLC: PS/2 using Link Station of Negotiable cannot connect.
 * Branch: California_branch
 *
 * Revision 3.2.62.4  1996/09/12  18:56:23  rbatz
 * CSCdi65595:  Traceback on multiple "qllc dlsw" commands.
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/21  13:15:22  rbatz
 * CSCdi62155:  LNX timers not working correctly. Result is TEST
 *              and NULL XID frames sent in the middle of
 *              XID(3) Negotiation.
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/17  12:13:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/06  01:06:05  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  21:59:02  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/18  22:10:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  02:02:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  18:48:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:55:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  21:06:34  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.1  1995/06/07  23:01:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QLLC_PRIVATE_H__
#define __QLLC_PRIVATE_H__

/************************************************************************
 *									*
 *				CLSI SUPPORT			       	*
 *									*
 * QLLC(CLSI) is implemented by a hieriarchy of two FSMs.  The first	*
 * FSM is used to manage the virtual circuit at the X.25 level; the	*
 * second FSM is used to manage the QLLC state of an established	*
 * virtual circuit.							*
 *									*
 * Another way to look at it that the X.25 level is like the physical	*
 * level, while the QLLC level corresponds more closely to a logical	*
 * link level.								*
 *									*
 * (Too bad that the expression Logical Link Control has been		*
 * pre-empted for a standard implementation - 802.2 or IBM Token Ring	*
 * Architecture - choose your poison)					*
 *									*
 ************************************************************************
 */


/************************************************************************
 *									*
 *				QLLC EVENTS				*
 *									*
 * QLLC expects to use a small number of event blocks.  These are	*
 * allocated when the qllctype is made, and are included in it.  It's	*
 * a an implementation failure to run out of event blocks.  Hmmm	*
 * ... eventually we may pool event blocks so that they can be shared	*
 * amongst a number of qllc's That's the advanced memory management	*
 * strategy.								*
 *									*
 ************************************************************************
 */

/* QLLC Managed timer types */

enum {
    QX25_TIMER,	/* For X.25 virtual circuit */
    QLLL_TIMER,	/* For Logical Link Layer */
    QLNX_TIMER, /* For legacy QLLC / RSRB */
};

/************************************************************************
 *									*
 *			CLSI SUPPORT					*
 *									*
 * QLLC(CLSI) is implemented by a hieriarchy of two FSMs.  The first	*
 * FSM is used to manage the virtual circuit at the X.25 level; the	*
 * second FSM is used to manage the QLLC state of an established	*
 * virtual circuit.							*
 *									*	
 ************************************************************************
 */


#define QLLC_DEFAULT_MAX_IFIELD	(265)
#define QLLC_NO_MAJOR	0		/* no opcode decode routine used */

/*
 * First FSM : X.25 connectivity management
 * 
 * States:
 */

/* States are defined in qllc.h */

/*
 * Inputs:
 */
 
typedef enum QXInputT_ {

	/* A	Clsi Inputs 					*/

    QX25ReqOpenStnReq,
    QX25CloseStnReq,
    
	/*  B	X.25 Inputs					*/

    QX25IncomingCall,
    QX25CallConfirm,
    QX25ClearIndication,
    QX25ClearConfirm,
    QX25ResetIndication,
    QX25ResetOutOfOrder,
    QX25ResetConfirm,
    QX25Data,
    QX25Delete,

	/* C Other Inputs					*/

    QX25TimerExpires,
    QX25OutOfOrderTimer,
    
    QX25NumInputs	/* Count the number of QLLC X25 Inputs		*/
} QXInputT;


/*
 * Second FSM : QLLC state management
 *
 * A QLLC CEP will only drive this state machine if the QLLC X25 state machine 
 * is in P4/D1.
 */

/* States are defined in qllc.h */

/*
 * Inputs:
 */
 
typedef enum QLInputT_ {

	/* A	Clsi Inputs 					*/

    CLSXID,
    CLSTest,
    ConnectRequest,
    ConnectResponse,
    SignalStationRequest,
    DisconnectRequest,
    DisconnectResponse,
    DataRequest,
    CLSQuenchOn,
    CLSQuenchOff,
    
	/*  B	QLLC (X.25) Inputs				*/

    QLXID,
    QLTest,
    QLSM,
    QLUA,   

	/*			Connected Events		*/

    QLRR,
    QLDISC,
    QLRD,
    QLDM,
    QLFRMR,
    QLData,
    QX25QuenchOn,
    QX25QuenchOff,
 

	/* C Other Inputs						*/

    QLTimerT1,		/* (retry count within limit)			*/
    QLTimerT1N2,	/* (retry count exceeded)			*/
    
    QLNumInputs	/* Count the number of QLLC CLSI FSM Inputs	*/
} QLInputT;

/********************************************************************************
 *										*
 *				QLLC FSM inlines and friends			*
 *										*
 ********************************************************************************
 */


/* Output Codes
 *
 * We eschew Code 0, just to make it a bit more robust
 *
 * We also try to avoid too much that's specific to X.25
 */
 
/* X.25 Codes */

typedef enum {
    OpX25None,
    
    /* X.25 VIrtual CIrcuit Op Codes		 	*/
    OpCallReq,
    OpAccept,
    OpClearRst,			/* Clear/Reset	- normal		*/
    OpClearRstE,		/* Clear/Reset w/ Error			*/
    OpClearRstConf,		/* Clear/Reset Confirm			*/
    
    /* QLLC FSM Op Codes				*/
    OpXid,			/* Send Qllc Xid	*/
    OpXNact,			/* forward non-activation XID only 	*/
    OpTest,
    OpSetMode,
    OpUA,
    OpDISC,
    OpRD,
    OpDM,
    OpRNR,			/* X.25 Packet level RNR 		*/
    OpRR,			/* X.25 Packet level RR 		*/
    OpDlcData,			/* Data Packet out to X.25 */
    OpDlcDrop,                  /* Received a packet when we shouldn't have - drop it */
    QXNops			/* Count the number of X.25 op Codes	*/
    
}  	QX25OpT;

/* CLSI indications to CLS core */

typedef enum {
    OpClsNone,

    /* X.25 VIrtual CIrcuit Op Codes 					*/
    OpReqOpenStnConf,	/* Request Open Station Confirm			*/
    OpReqOpenStnConfL,	/* Request Open Station Confirm	local error	*/
    OpReqOpenStnConfR,	/* Request Open Station Confirm	remote error	*/
    OpCloseStnConf,	/* Close Station Confirm 			*/
    OpCloseStnConfKill,	/* Close Station Confirm (kill qllc cb)		*/


    OpDiscInd,		/* Disconnect Indication			*/
    
    /* QLLC FSM Op Codes						*/
    OpIdInd,		/* ID Indication				*/
    OpLNact,		/* Non-activation ID indication			*/
    OpTestInd,
    OpConnInd,
    OpConnConf,
    OpCtdInd,		/* Time to send CONNECTED.IND			*/
    OpDiscConf,
    OpDiscConfE,
    OpQuenchOn,
    OpQuenchOff,
    OpDataInd,
    OpDataConf,			/* Negative Conf - link went down 	*/
    OpCleanup,
    OpZombie,
    OpLocalError,		/* Shouldn't happen			*/
    OpRemoteError,		/* Can happen - we don't have control	*/	
    QClsNops			/* Count the number of Clsi Op Codes	*/
    
}  	QClsOpT;

typedef enum {
	/* X.25 Timers */
    OpQTimerNone,
    OpQXTimerStart,
    OpQXTimerStop,
	/* CLSI - Logical Link Layer Timers */
    OpQLTimerStart,
    OpQLTimerIncr,
    OpQLTimerStop,
    QNTimers			/* Count the number of Timer Op Codes	*/

}	QTimerT ;


/* Structure for the QLLC parameter block
 *
 * Use this in QLLCAction
 */

typedef struct {
    QX25OpT		fDlcOp;
    QClsOpT		fClsOp;
    QTimerT		fTimerOp;
} QFsmParamBlockT;



/************************************************************************
 *									*
 *			QLLC PROVIDER SAPs				*
 *									*
 * The QPSapT is what is used internally in qllc, and consists of the	*
 * same information provided in the QSapT, PLUS various data theat are	*
 * reserved for internal use						*
 *									*
 ************************************************************************
 */

struct QPSapT_ {
    avl_node_type fAvlNode;

    QSapT	fQUSap;
    word 	fUSapId;
    void	*fQCepList;
    qllctype	*fQCepHit;
    idbtype	*idb;
    /* Range of virtual mac addresses that may be used
     * with this SAP 
     */
    QSapVmacRangeT	*fQVmacRange;
} ;

/* USap Methods */


static inline boolean IsSVCSap (QSapT *qsap)
{	return (!QTestPvc( &qsap->fQSapFlags));		}

static inline boolean QSapTestProxyXid (QSapT *qsap)
{	return (QTestProxyXid( &qsap->fQSapFlags));	}

static inline boolean QSapTestPartner (QSapT *qsap)
{	return (QTestPartner( &qsap->fQSapFlags));	}

static inline boolean QSapTestRemote (QSapT *qsap)
{	return (QTestRemote( &qsap->fQSapFlags));	}

static inline boolean QSapTestVmac (QSapT *qsap)
{	return (QTestVmac( &qsap->fQSapFlags));		}

static inline boolean QSapTestDlsw (QSapT *qsap)
{	return (QTestDlsw(&qsap->fQSapFlags));		}

static inline boolean QSapTestTRSaps (QSapT *qsap)
{	return (QTestTRSaps(&qsap->fQSapFlags));	}

static inline boolean QSapTestNpsiPoll (QSapT *qsap)
{	return (QTestNpsiPoll(&qsap->fQSapFlags));	}

static inline uchar *QSapGetPartner (QSapT *qsap)
{	return (&qsap->fPartner.addr[0]);		}

/* PSap Methods */

static inline boolean IsSVCPSap (QPSapT *qpsap)
{	return (!QTestPvc( &qpsap->fQUSap.fQSapFlags));	}

static inline uchar *QPSapGetSubaddress (QPSapT *qpsap)
{ return (qpsap->fQUSap.fDispatch.fQSapSubaddress); 	}

static inline QSapPVCRangeT *QPSapGetPvcRange (QPSapT *qpsap)
{ return (&qpsap->fQUSap.fDispatch.fQSapPvcRange); 	}

static inline uint QPSapGetPvcLow (QPSapT *qpsap)
{ return (qpsap->fQUSap.fDispatch.fQSapPvcRange.fPVCLow); }

static inline uint QPSapGetPvcHigh (QPSapT *qpsap)
{ return (qpsap->fQUSap.fDispatch.fQSapPvcRange.fPVCHigh); }

static inline boolean IsPSVCPSap (QPSapT *qpsap)
{ return (IsSVCSap(&qpsap->fQUSap)); }

static inline boolean QPSapTestPartner (QPSapT *qpsap)
{	return (QSapTestPartner(&qpsap->fQUSap));	}

static inline boolean QPSapTestProxyXid (QPSapT *qpsap)
{	return (QSapTestProxyXid(&qpsap->fQUSap));	}

static inline boolean QPSapTestVmac (QPSapT *qpsap)
{	return (QSapTestVmac(&qpsap->fQUSap));		}

static inline boolean QPSapTestRemote (QPSapT *qpsap)
{	return (QSapTestRemote( &qpsap->fQUSap));	}

static inline uchar *QPSapGetPartner(QPSapT *qpsap)
{	return (QSapGetPartner(&qpsap->fQUSap));	}


/************************************************************************
 *									*
 *			QLLC CEPs					*
 * Structure, Constants, Methods					*
 *									*
 ************************************************************************
 */


extern queuetype QCepFreeupQ;

typedef enum qllc2OpenFailureCode_
{
    QllcInUse  		 = -1,
    QllcNoMem  		 = -2,
    QllcNoRemoteAddress  = -3,
} qllcOpenFailureCode;

/* As usual the hash size must be a power of 2 */

#define QLLC_HASH_SIZE		64

/* Default values for the timers */

#define DEFAULT_X25_TIMER   (10 * ONESEC) /* X25 Timer */
#define DEFAULT_QL_TIMER    (8 * ONESEC) /* QLLC Timer */
#define DEFAULT_QL_RETRIES	(4)	/* Max Logical Link Layer retries */

/*
 * Constants to define the flow control characteristics of
 * a QLLC connection
 */
#define QLLC_TXQ_LOW_WATER	20	/* percentage */
#define QLLC_TXQ_HIGH_WATER	70	/* percentage */
#define QLLC_CLS_FLOW_IND_DAMP	2	/* damping zone - scalar */

void QCepDestroy(qllctype *qcep);
qllctype *QCepFindOpened (QPSapT *qpsap, QLLCReqOpenStnT *qllcReqOpenStn);
qllctype *QCepReqOpenStn (QPSapT *qpsap, ClsUCepIdT uCepId,
                          QLLCReqOpenStnT *qllcReqOpenStn, qllctype *qllc);
qllctype *QCepSearchByUCepId(ClsUCepIdT uCepId);
boolean QllcClsNotify(qllctype *qllc, ClsOpcodeT opcode,
		     ClsErrorCodeT errorCode, paktype *pak);
void QCepBindLci(qllctype *qllc, lcitype *lci);
void QCepUnbindLci(qllctype *qllc);
void QCepReset(qllctype *qllc);

boolean QCepSvcProc(qllctype *qllc, va_list argptr);
typedef boolean QCepWalkT(qllctype *qllc, va_list argptr);

static inline boolean QllcIsCls( qllctype *qcep)
{	return(qcep->fUCepId != NO_CLS);		}

/* QLLC SERVICE ACCESS POINT MANAGEMENT
 *
 * QLLC SAPs are organized by AVL trees.
 *
 * Three trees are used, and can be reached from the Sap Registry.
 *
 * The trees are:
 * 1. A tree for SVC's; SAPs are recognized by X.121 subaddresses
 * 2. A tree for PVC's; SAPs are identified by the LCN#, and each node is a rangetype.
 * 	A SAP is in one or the other of these trees.
 *
 * 3. A tree for a virtual mac range.  This is orthogonal to the other
 * two trees.  Nodes on this tree merely point to the QllcSapType.
 */


/* Vaious Constants */

#define NO_VMAC		-1		/* QSapFindFreeVmac: error return */
#define VMAC_FREE	0
#define VMAC_IN_USE	1

STRUCTURE_REF(QSapRegistryT);

/************************************************************************
 *									*
 *				QLLC SAP REGISTRY			*
 *									*
 ************************************************************************
 */

/* This is how SAPs are attached to a Port
 *
 * Details of how this is implemented are restricted to the implementation
 */

struct QSapRegistryT_ {
   QPSapT		*fQSRSvcList;	/* top of the SVC AVL Tree */
   QPSapT		*fQSRPvcList;	/* top of the PVC AVL Tree */
   QSapVmacRangeT	*fQSRVmacRList;	/* top of the Virtual Mac Range AVL tree */

	/* temporary storage for search hits */
   QPSapT		*fQSapHit;	/* Hit on Sap Search */
   QSapVmacRangeT	*fVmacRHit;	/* Hit on Virtual Mac Range Search */
} ;


/************************************************************************
 *									*
 *				QLLC DATA 				*
 *									*
 * This is the data that hangs of the idb.  We just leave one pointer	*
 * one the hwidb.							*
 *									*
 ************************************************************************
 */

#define QMSG_MAX	10		/* max # of messages in ActRing queue */
struct qllc_data_t_ {
    queuetype		fMsgQ;
    TCLSIMsg		*fQFirstPending; /* For DLSw */
    TCLSIMsg		*fQLastPending;
    boolean		lnx_active;
    qllctype		*fUnclaimedPvcs;
    tr_ventrytype	*vre;
};

typedef qllc_data_t QDataT;

QDataT *qllc_hwidb_get_data(hwidbtype *hwidb, boolean force);

static inline QDataT *qllc_idb_get_data (idbtype *idb, boolean force)
{
    return(qllc_hwidb_get_data(hwidb_or_null(idb), force) );
}



static inline void qllc_hwidb_set_lnx_active (hwidbtype *idb, boolean set)
{
    QDataT	*qData;

    /* don't need to force if we don't need to set lnx_active to TRUE */
    qData = qllc_hwidb_get_data(idb, set);
    if (qData != NULL) {
	qData->lnx_active = set;
    }
    return;
}
static inline void qllc_idb_set_lnx_active (idbtype *idb, boolean set)
{
    qllc_hwidb_set_lnx_active( hwidb_or_null(idb), set);
    return;
}

static inline boolean qllc_hwidb_test_lnx_active (hwidbtype *hwidb)
{
    QDataT	*qData;

    qData = qllc_hwidb_get_data(hwidb, FALSE);
    if (qData != NULL) {
	return(qData->lnx_active);
    }
    return (FALSE);
}


static inline boolean qllc_idb_test_lnx_active (idbtype *idb)
{
    return(qllc_hwidb_test_lnx_active(hwidb_or_null(idb)));
}

static inline tr_ventrytype *qllc_get_vre (hwidbtype *idb)
{
    QDataT	*qData;

    qData = qllc_hwidb_get_data(idb, FALSE);
    if (qData != NULL) {
	return(qData->vre);
    }
    return (NULL);
}

static inline void qllc_set_vre (hwidbtype *idb, tr_ventrytype *vre)
{
    QDataT	*qData;

    /* don't need to force if we don't need to set vre to a meaningful value*/
    qData = qllc_hwidb_get_data(idb, vre ? TRUE: FALSE);
    if (qData != NULL) {
	qData->vre = vre;
    }
    return;
}


/* QllcPortGetSapRegistry
 *
 * Get the sap registry associated with an idb
 */
static inline QSapRegistryT *QllcPortGetSapRegistry (idbtype *idb)
{
    return( (QSapRegistryT *) (idb->qsap_registry));
}

QPSapT *QSRSearchBySap(QSapRegistryT *qsr, QPSapT *qpsap);
boolean QSRAddQSap(QSapRegistryT *qsr, QPSapT *new);
boolean QSRRemoveQSap(QSapRegistryT *qsr, QPSapT *toast);
QPSapT *QSRSearchByUSap(QSapRegistryT *qsr, ClsUSapIdT uSapId);

/************************************************************************
 *									*
 *				QLLC SAP				*
 * Methods								*
 *									*
 ************************************************************************
 */

QPSapT *QSapCreate (ClsUSapIdT uSapId, QSapT *qActSap, idbtype *idb);
void QSapDestroy (QPSapT *qpsap);
QSapVmacRangeT *QVmacRangeCreate (QPSapT *qpsap, uchar *vmacStart, int numVmacs);
void QVmacRangeDestroy (QPSapT *qpsap);
boolean QSapClaimPvcs(QPSapT *qpsap);

/************************************************************************
 *									*
 *			Virtual Mac Address Range			*
 *									*
 * This uses integer addition/subraction on the mac addresses.		*
 * The alternative was to subtract one virtual mac address from another	*
 * which is messy, and leave one wondering what addition might could be	*
 *									*
 ************************************************************************
 */
 
#define QVMAC_TIMEOUT   (30 * ONESEC)

struct  QSapVmacRangeT_ {
    avl_node_type 	fAvlNode;
    uchar		fVmacStart[IEEEBYTES];	/* starting vmac Address */
    uint		fNumVmacs;	/* number in the range
					 * If this is zero, then any number of
					 * connections can use this for making
					 * Request Open Station Requests
					 */

    QPSapT 		*fQPsap; 	/* pointer to the Qllc Sap */
    uchar		fVmacMap[1];	/* The map of Vmac's in use/free */
} ;




/* FSM Methods */
void QXFsmExecute (qllctype *qllc, QXInputT event);
void QLFsmExecute (qllctype *qllc, QLInputT event, paktype *pak);

/* Events: */

/* QEventCreate
 *
 * Constructor
 *
 * Get an unused event from the free event queue; 
 * if it's empty our design is broken
 */

static inline QllcEventT *
QEventCreate (qllctype *qllc, int event, paktype *pak)
{
    QllcEventT	*qEvent;

    if (qllc_event_debug) {	
	buginf("\ncreate event %d, qllc 0x%x", event, qllc);
    }

if ((int)(qllc->fFreeEventQ.qhead) &1) { extern void crashdump(int); crashdump(0); }
    qEvent = p_dequeue(&qllc->fFreeEventQ);
    CLSASSERT_FATAL(qEvent);
	
    if (qEvent != NULL) {
	qEvent->fEvent = event;
	qEvent->fPacket = pak;
    }
	
    return (qEvent);
}

/* QEventDestroy
 *
 * Destructor
 *
 * put the event block back on the free event queue
 */
 
static inline void 
QEventDestroy (QllcEventT *qEvent, qllctype *qllc)
{
    paktype	*pak;
    
    pak = qEvent->fPacket;
    qEvent->fPacket = NULL;
    
    CLSASSERT(pak == NULL);
    if (pak != NULL) {
	datagram_done(pak);
    }
    p_enqueue(&qllc->fFreeEventQ, qEvent);
}

/* QExtractEvent
 *
 * Get the next event off the queue, and stash any packet that goes with it
 * call event destructor routine
 */

static inline int QExtractEvent (qllctype *qllc, queuetype *queue, 
				 paktype **packetStash)
{
    paktype		*pak;
    QllcEventT		*qEvent;
    QLInputT		event;	
	
    qEvent = p_dequeue(queue);
		

    if (qEvent == NULL) {
	return (FSM_NO_EVENT);
    }
	
    pak = qEvent->fPacket;
    qEvent->fPacket = NULL;
    if (packetStash != NULL) {
	*packetStash = pak;
    } else {
	if (pak != NULL) {
	    CLSASSERT(!"dropping a packet");
	    datagram_done(pak);
	}
    }
    event = qEvent->fEvent;
    QEventDestroy(qEvent, qllc);
    return (event);
}

void 
QFsmEventEnqueue(qllctype *qllc, queuetype *queue, int event, paktype *pak);

/* QXFsmEventEnqueue
 *
 * Put the Logical Link event on its input queue 
 */

static inline void QLFsmEventEnqueue (qllctype *qllc, QLInputT event, paktype *pak)
{
    QFsmEventEnqueue(qllc, &qllc->fQLeventQ, event, pak);
}

/* QXFsmEventEnqueue
 *
 * Put the X.25 Virtual Circuit event on its input queue 
 */

static inline void QXFsmEventEnqueue (qllctype *qllc, QXInputT event)
{
    QFsmEventEnqueue(qllc, &qllc->fQXeventQ, event, NULL);
}

/* QXFsmEventDequeue
 *
 * get the next pending X.25 Virtual Circuit event
 */
 
static inline QXInputT QXFsmEventDequeue (qllctype *qllc)
{
    return(QExtractEvent(qllc, &qllc->fQXeventQ, NULL));
}

/* QLFsmEventDequeue
 *
 * get the next pending Logical Link event
 */
 
static inline QLInputT QLFsmEventDequeue (qllctype *qllc)
{
    return(QExtractEvent(qllc, &qllc->fQLeventQ, &qllc->fPacket));
}

/* 
 * QEventCleanFsmQueues
 *
 * Clear all events from the XFSM and LFSM queues.
 * Events are returned to the Free Event queue.
 */
static inline void 
QEventCleanFsmQueues (qllctype *qllc)
{
    while (QXFsmEventDequeue(qllc) != FSM_NO_EVENT)
        ;

    do {
        datagram_done(qllc->fPacket);
        qllc->fPacket = NULL;
    } while (QLFsmEventDequeue(qllc) != FSM_NO_EVENT);
}

/* 
 * QEventCleanQueues
 *
 * Clear all events from the FSM Queues 
 * and from the Free Event Queue.
 */
static inline void 
QEventCleanQueues (qllctype *qllc)
{
    QEventCleanFsmQueues(qllc);
    while (p_dequeue(&qllc->fFreeEventQ) != NULL)
        ;
}

static inline boolean QCepTestPartner (qllctype *qcep)
{	return (QTestPartner( &qcep->fQCepFlags));	}

static inline boolean QCepTestVmac (qllctype *qcep)
{	return (QTestVmac( &qcep->fQCepFlags));	}

static inline boolean QCepTestNpsiPoll (qllctype *qcep)
{	return (QTestNpsiPoll(&qcep->fQCepFlags));	}

static inline uchar *QCepGetPartner (qllctype *qcep)
{	return ( &qcep->fPartner[0]);			}

static inline ushort QCepGetLcn (qllctype *qcep)
{
    if (qcep->lci != NULL) {
	return ( x25_get_lcn(qcep->lci) );
    }
    return (0);
}

static inline uchar *QCepGetRemX121Addr(qllctype *qcep)
{
    if (QTestRemote(&qcep->fQCepFlags)) {
	return(qcep->fRemote);		
    } else {
	return(NULL);
    }
}

static inline boolean QCepTestRemote(qllctype *qcep)
{	return(QTestRemote(&qcep->fQCepFlags));		}
    
static inline void QCepSetRemote(qllctype *qcep)
{	QSetRemote(&qcep->fQCepFlags);			}

static inline void QCepSetProxyXid(qllctype *qcep)
{	QSetProxyXid(&qcep->fQCepFlags);		}

static inline void QCepSetVmac(qllctype *qcep)
{	QSetVmac(&qcep->fQCepFlags);			}

/************************************************************************
 *									*
 *				AVL WRAPPERS				*
 *									*
 ************************************************************************
 */

/* AVL Access for SVC Sap Types */

enum avl_compare QSVCSapCompare (avl_node_type *node1, avl_node_type *node2);

/*
 * Inlines to do type conversion to AVL routines.
 */

/*
 * QSVCSapSearch
 *
 * Wrapper for avl_search
 */

static inline QPSapT *
QSVCSapSearch (QSapRegistryT *qsr, QPSapT *qpsap)
{
    return((QPSapT *) avl_search((avl_node_type *)qsr->fQSRSvcList,
				    (avl_node_type *)qpsap,
				    QSVCSapCompare));
}

/*
 * QSVCSapGetFirst
 *
 * Wrapper for avl_get_first
 */

static inline QPSapT *
QSVCSapGetFirst (QSapRegistryT *qsr)
{
    return((QPSapT *) avl_get_first((avl_node_type *)qsr->fQSRSvcList));
}

/*
 * QSVCSapGetNext
 *
 * Wrapper for avl_get_next.
 */

static inline QPSapT *
QSVCSapGetNext (QSapRegistryT *qsr, QPSapT *qpsap)
{
    return((QPSapT *) avl_get_next((avl_node_type *)qsr->fQSRSvcList,
				  (avl_node_type *)qpsap, QSVCSapCompare));
}

/*
 * QSVCSapInsert
 *
 * Wrapper for avl_insert.
 */
static inline QPSapT *
QSVCSapInsert (QSapRegistryT *qsr, QPSapT *new)
{
    boolean balancing;

    return((QPSapT *) avl_insert((avl_node_type **)&qsr->fQSRSvcList, 
    				(avl_node_type *)new,
				&balancing, QSVCSapCompare));
}

/*
 * QSVCSapDelete
 *
 * Wrapper for avl_delete.
 */

static inline QPSapT *
QSVCSapDelete (QSapRegistryT *qsr, QPSapT *toast)
{
    boolean balancing;

    return((QPSapT *) avl_delete((avl_node_type **)&qsr->fQSRSvcList,
				(avl_node_type *)toast, &balancing, 
				QSVCSapCompare));
}

/* AVL Access for PVC Sap Types */

enum avl_compare QPVCSapCompare (avl_node_type *node1, avl_node_type *node2);

/*
 * Inlines to do type conversion to AVL routines.
 */

/*
 * QPVCSapSearch
 *
 * Wrapper for avl_search
 */

static inline QPSapT *
QPVCSapSearch (QSapRegistryT *qsr, QPSapT *qpsap)
{
    return((QPSapT *) avl_search((avl_node_type *)qsr->fQSRPvcList,
				(avl_node_type *)qpsap,
				QPVCSapCompare));
}

/*
 * QPVCSapGetFirst
 *
 * Wrapper for avl_get_first
 */

static inline QPSapT *
QPVCSapGetFirst (QSapRegistryT *qsr)
{
    return((QPSapT *) avl_get_first((avl_node_type *)qsr->fQSRPvcList));
}

/*
 * QPVCSapGetNext
 *
 * Wrapper for avl_get_next.
 */

static inline QPSapT *
QPVCSapGetNext (QSapRegistryT *qsr, QPSapT *qpsap)
{
    return((QPSapT *) avl_get_next((avl_node_type *)qsr->fQSRPvcList,
				  (avl_node_type *)qpsap, QPVCSapCompare));
}

/*
 * QPVCSapInsert
 *
 * Wrapper for avl_insert.
 */

static inline QPSapT *
QPVCSapInsert (QSapRegistryT *qsr, QPSapT *new)
{
    boolean balancing;

    return((QPSapT *) avl_insert((avl_node_type **)&qsr->fQSRPvcList, 
    				(avl_node_type *)new,
				&balancing, QPVCSapCompare));
}

/*
 * QPVCSapDelete
 *
 * Wrapper for avl_delete.
 */

static inline QPSapT *
QPVCSapDelete (QSapRegistryT *qsr, QPSapT *toast)
{
    boolean balancing;

    return((QPSapT *) avl_delete((avl_node_type **)&qsr->fQSRPvcList,
				(avl_node_type *)toast, &balancing, 
				QPVCSapCompare));
}

/* AVL Access for Virtual Mac Range Types */

enum avl_compare QVmacRCompare (avl_node_type *node1, avl_node_type *node2);

/*
 * Inlines to do type conversion to AVL routines.
 */

/*
 * QVmacRSearch
 *
 * Wrapper for avl_search
 */

static inline QSapVmacRangeT *
QVmacRSearch (QSapRegistryT *qsr, QSapVmacRangeT *qvmr)
{
    return((QSapVmacRangeT *) avl_search((avl_node_type *)qsr->fQSRVmacRList,
				      (avl_node_type *)qvmr,
				      QVmacRCompare));
}

/*
 * QVmacRGetFirst
 *
 * Wrapper for avl_get_first
 */

static inline QSapVmacRangeT *
QVmacRGetFirst (QSapRegistryT *qsr)
{
    return((QSapVmacRangeT *) avl_get_first((avl_node_type *)qsr->fQSRVmacRList));
}

/*
 * QVmacRGetNext
 *
 * Wrapper for avl_get_next.
 */

static inline QSapVmacRangeT *
QVmacRGetNext (QSapRegistryT *qsr, QSapVmacRangeT *qvmr)
{
    return((QSapVmacRangeT *) avl_get_next((avl_node_type *)qsr->fQSRVmacRList,
					(avl_node_type *)qvmr, QVmacRCompare));
}

/*
 * QVmacRInsert
 *
 * Wrapper for avl_insert.
 */
static inline QSapVmacRangeT *
QVmacRInsert (QSapRegistryT *qsr, QSapVmacRangeT *new)
{
    boolean balancing;

    return((QSapVmacRangeT *) avl_insert((avl_node_type **)&qsr->fQSRVmacRList, 
				      (avl_node_type *)new,
				      &balancing, QVmacRCompare));
}

/*
 * QVmacRDelete
 *
 * Wrapper for avl_delete.
 */

static inline QSapVmacRangeT *
QVmacRDelete (QSapRegistryT *qsr, QSapVmacRangeT *toast)
{
    boolean balancing;

    return((QSapVmacRangeT *) avl_delete((avl_node_type **)&qsr->fQSRVmacRList,
				      (avl_node_type *)toast, &balancing, 
				      QVmacRCompare));
}

/* Virtual Mac Ranges */


/* QLLC Cep List Methods */

QCepListT *QCepListCreate(QPSapT *qpsap);
void QCepListDestroy(QPSapT *qpsap);

qllctype *QCepListInsert(QPSapT *qpsap, qllctype *qllc);
qllctype *QCepListGetFirst(QPSapT *qpsap);
qllctype *QCepGetNext(QPSapT *qpsap, qllctype *qllc);

/* QLLC Cep List SVC Methods 
 *
 * Wrappers for AVL tree functions
 */
enum avl_compare QCepSvcCompare  (avl_node_type *qcep1, avl_node_type *qcep2);

/*
 * QCepSvcInsert
 *
 * Wrapper for avl_delete.
 */

static inline qllctype *QCepSvcInsert(QPSapT* qpsap, qllctype *new)
{
    boolean balancing;

    return((qllctype *) avl_insert((avl_node_type **)&qpsap->fQCepList,
				   (avl_node_type *)new,
				   &balancing, QCepSvcCompare));
}

/*
 * QCepSvcListDelete
 *
 * Wrapper for avl_delete.
 */

static inline qllctype *QCepSvcListDelete(QPSapT* qpsap, qllctype *toast)
{
    boolean balancing;

    return((qllctype *) avl_delete((avl_node_type **)&qpsap->fQCepList,
				   (avl_node_type *)toast, &balancing, 
				   QCepSvcCompare));
}

/*
 * QCepSvcGetFirst
 *
 * Wrapper for avl_delete.
 */

static inline qllctype *QCepSvcGetFirst(QPSapT* qpsap)
{
    return((qllctype *) avl_get_first((avl_node_type *)qpsap->fQCepList));

}

/*
 * QCepSvcGetNext
 *
 * Wrapper for avl_delete.
 */

static inline qllctype *QCepSvcGetNext(QPSapT* qpsap, qllctype *qllc)
{
    return((qllctype *) avl_get_next((avl_node_type *)qpsap->fQCepList,
				     (avl_node_type *)qllc, QCepSvcCompare));

}

/* QLLC Cep List PVC Methods */

qllctype *QCepPvcInsert(QPSapT* qpsap, qllctype *qllc);
qllctype *QCepPvcListDelete(QPSapT* qpsap, qllctype *qllc);
qllctype *QCepPvcGetFirst(QPSapT* qpsap);
qllctype *QCepPvcGetNext(QPSapT* qpsap, qllctype *qllc);


void QllcShowCeps(void);

/* QLLC SAP Registry Methods */

QSapRegistryT *QSapRegistryCreate(idbtype *idb);
void QSapRegistryDestroy(idbtype *swidb);
void QSapRegistryShow(idbtype *swidb);


char *prQLState(QLStateT state);
char *prQXState(QX25StateT state);

/* 
 * dlcqllc.c
 */

void qllc_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
	       ClsDlcParamBlockT *clsParamBlock);

#endif __QLLC_PRIVATE_H__

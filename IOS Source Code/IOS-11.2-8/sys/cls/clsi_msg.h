/* $Id: clsi_msg.h,v 3.5.10.4 1996/09/11 04:45:24 kchiu Exp $
 * $Source: /release/112/cvs/Xsys/cls/clsi_msg.h,v $
 *------------------------------------------------------------------
 * Define CLSI Message Structure/Class
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Defines messages exchanged across the CLSI interface. These
 * messages are exchanged between the CLS user (DLU) and the CLS service
 * provider (CLS). 
 *
 * The purpose of CLSI is to allow all link-layer and sub-network 
 * transport services commonly available for the protocol stacks such 
 * as APPN.
 *------------------------------------------------------------------
 * $Log: clsi_msg.h,v $
 * Revision 3.5.10.4  1996/09/11  04:45:24  kchiu
 * CSCdi68756:  DLSw+/fddi is not working
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/09/03  22:27:02  rbatz
 * CSCdi67189:  Multiple connections for reverse QLLC (DLSW+) crashes.
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/08/10  01:03:01  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/05/17  10:42:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.6  1996/05/14  18:41:55  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.5  1996/05/03  22:06:06  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.4  1996/04/24  06:28:11  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.5.2.3  1996/04/11  14:35:55  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/05  06:05:21  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.5.2.1  1996/03/28  08:19:23  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.5  1996/02/26  06:09:12  dmccowan
 * CSCdi49560:  APPN Connection Network over FDDI fails to activate
 * Fill in EnableTRParms_t structure for FDDI.
 * Remove Obsolete EnableFDDIParms_t structure.
 *
 * Revision 3.4  1996/01/22  03:53:36  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.3.2.2  1996/03/03  00:26:12  ioakley
 * CSCdi50585:  Pick up Phil Morton's Vdlc_Rework and
 *              Mark Regan's Appn over DLSw updates.
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.3.2.1  1995/12/17  09:50:01  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.3  1995/11/20  23:21:27  fbordona
 * CSCdi43362:  DLSW fst transport not working.
 *              Add fReserveLength fields to IdStnInd_t and
 *              ConnectStnInd_t.
 *
 * Revision 3.2  1995/11/17  08:55:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  20:53:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/13  22:24:48  rnaderi
 * CSCdi41309:  router crashes when dlsw configured over frame-relay
 *
 * Revision 2.9  1995/10/11  04:20:08  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.8  1995/08/03  21:35:53  lpereira
 * CSCdi28887:  sanity_checker finds edisms at interrupt level
 * Added fields in the CLS header to accomodate a APPN queue
 * element.
 *
 * Revision 2.7  1995/07/31  18:58:22  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.6  1995/07/21  17:08:07  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.5  1995/07/19  20:58:22  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.4  1995/07/02  05:15:24  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.3  1995/06/15  22:59:49  ppearce
 * CSCdi35921:  CLS/RSRB virtual ring not removed after "appn stop"
 *
 * Revision 2.2  1995/06/09  19:26:57  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:19:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLSI_MSG_H__
#define __CLSI_MSG_H__

#include "../ibm/ibm_types.h"

/*
 * This is an enumeration of all the DLC types. It is used in the
 * fPortType field of the ENABLE and ACTIVATE_SAP primitives.
 * This enum must match up with the equivalent in the APPN OEM code.
 * If you add to it, do so only at the end.
 */

typedef enum 
{ 
    CLS_ILLEGAL_DLC,
    CLS_SDLC,
    CLS_SOCKET,
    CLS_LLC,
    CLS_LOCAL,
    CLS_RSRB,			/* Covers LACK and pass through case */
    CLS_TBRIDGE,
    CLS_FRAME_RELAY,
    CLS_ATM_1483,
    CLS_QLLC,
    CLS_VDLC,
    CLS_PPP,
    CLS_NCIA,
    CLS_MAXDLC
} CLSDLCType_e;

/*
 * Define for CLSI Version ID
 */

#define CLSI_VERSION     0x01		/* Current version of CLSI Interface */


/* Forward References For qllc - we'll obviously need to straighten out here
 * PHIL
 */

STRUCTURE_REF(QSapT);
STRUCTURE_REF(QSapPVCRangeT);
STRUCTURE_REF(QSapVmacRangeT);
STRUCTURE_REF(QCepListT);
STRUCTURE_REF(QTimersT);
STRUCTURE_REF(ReqOpnStnLLCParms_t);

typedef ReqOpnStnLLCParms_t ReqOpnStnNCIAParms_t;
typedef ReqOpnStnLLCParms_t ReqOpnStnSDLCParms_t; /* just a clone */
typedef ReqOpnStnLLCParms_t ReqOpnStnVDLCParms_t; 

STRUCTURE_REF(ReqOpnStnReq_t);
STRUCTURE_REF(IdReq_t);
STRUCTURE_REF(IdRsp_t);
STRUCTURE_REF(DataReq_t);
STRUCTURE_REF(DataInd_t);

/*
 * Definition of CLSI Identifier-Type codes
 */

typedef enum
{
    CLS_ENTITY_MGR = 0x00,
    CLS_U_CEP_ID   = 0x01,
    CLS_P_CEP_ID   = 0x02,
    CLS_U_SAP_ID   = 0x03,
    CLS_P_SAP_ID   = 0x04
} CLSIdentifierType_e;

/*
 * Special value for an illegal p_cep_id.
 * Use this in fPCepId of ReqOpnStnReqh_t when making an outgoing call.
 */
#define CLS_NO_CEP (-1)


#define TR_MAX_RIF_LENGTH 18

/*
 * Definition of CLSI Selector codes
 */

typedef enum
{
    CLS_LLC_LAYER =  0x60
} CLSSelectorCode_e;	

/******************************************************************************
 * Definition of CLSI primitive codes
 ******************************************************************************/

#define CLS_COMMAND_MASK      0x0FFF	/* Mask for command part of primitive */
#define CLS_TYPE_MASK         0xF000	/* Mask for type part of primitive */

/*
 * Values for type field of primitiveCode
 */

typedef enum
{
    CLS_REQUEST=	0x0000,		/* primitive is a request */
    CLS_CONFIRM=	0xC000,		/* primitive is a confirm */
    CLS_INDICATION=	0x4000,		/* primitive is a indication */
    CLS_RESPONSE=	0x8000		/* primitive is a response */
} CLSPrimCodeType_e;


/*
 * Values for command field of primitiveCode
 */

typedef enum
{
    CLS_ENABLE=		0x0C00,
    CLS_DISABLE=	0x0C01,
    CLS_REQ_OPNSTN=	0x0C09,
    CLS_OPEN_STN=	0x0C0A,
    CLS_CLOSE_STN=	0x0C0B,
    CLS_ACTIVATE_SAP=	0x0C0D,
    CLS_DEACTIVATE_SAP=	0x0C0E,
    CLS_ID=		0x0C20,
    CLS_ID_STN=		0x0C23,
    CLS_CONNECT_STN=	0x0C24,
    CLS_SIGNAL_STN=	0x0C71,
    CLS_CONNECT=	0x0D00,
    CLS_DISCONNECT=	0x0D01,
    CLS_CONNECTED=	0x0D02,
    CLS_DATA=		0x0D10,
    CLS_FLOW=		0x0D50,
    CLS_PONG          = 0x0D60,
    CLS_MODIFY_STN =    0x0D61,

    CLS_UDATA=          0x0D11,
    CLS_MODIFY_SAP =    0x0E11,
    CLS_TEST =          0x0F11,
    CLS_ACTIVATE_RING = 0x0F21,
    CLS_DEACTIVATE_RING=0x0F31,
    CLS_TEST_STN      = 0x0F41,
    CLS_UDATA_STN     = 0x0F51,
} CLSPrimCodeCommandType_e;

/*
 * Values for Flow Control Indications
 */
#define DLC_CONGESTION_INCR    0x01  /* congestion is increasing */
#define DLC_CONGESTION_LOW     0x02
#define DLC_CONGESTION_MEDIUM  0x04
#define DLC_CONGESTION_HIGH    0x08
#define DLC_CONGESTION_MAX     0x10

/*
 * Flags for indicating which parameters to modify in connect and
 * modify primitives.
 */
#define CLS_MOD_MAXOUT		0x0001	/* Send window size */
#define CLS_MOD_MAXIFIELD	0x0002	/* Not implemented yet */
#define CLS_MOD_T1		0x0004	/* Not implemented yet */
#define CLS_MOD_MAXRETRY	0x0008	/* Not implemented yet */
#define CLS_MOD_OPENBUSY        0x0010	/* RR or RNR at startup. Not */
					/* implemented yet */
#define CLS_MOD_QUEUING		0x0020	/* output queuing parameters */


/***************************************************************
** Defines for CLSI header sizes
***************************************************************/

#define CLS_ACT_SAP_REQ_HDR_SZ        sizeof (ActSapReqh_t)
#define CLS_ACT_SAP_CNF_HDR_SZ        sizeof (ActSapCnfh_t)
#define CLS_DEACT_SAP_REQ_HDR_SZ      sizeof (DeactSapReqh_t)
#define CLS_DEACT_SAP_CNF_HDR_SZ      sizeof (DeactSapCnfh_t)
#define CLS_ENABLE_REQ_HDR_SZ         sizeof (EnableReqh_t)
#define CLS_ENABLE_CNF_HDR_SZ         sizeof (EnableCnfh_t)
#define CLS_DISABLE_REQ_HDR_SZ        sizeof (DisableReqh_t)
#define CLS_DISABLE_CNF_HDR_SZ        sizeof (DisableCnfh_t)
#define CLS_FLOW_REQ_HDR_SZ           sizeof (FlowReqh_t)
#define CLS_FLOW_IND_HDR_SZ           sizeof (FlowIndh_t)
#define CLS_OPEN_STN_REQ_HDR_SZ       sizeof (OpenStnReqh_t)
#define CLS_OPEN_STN_CNF_HDR_SZ       sizeof (OpenStnCnfh_t)
#define CLS_REQ_OPNSTN_REQ_HDR_SZ     sizeof (ReqOpnStnReqh_t)
#define CLS_REQ_OPNSTN_CNF_HDR_SZ     sizeof (ReqOpnStnCnfh_t)
#define CLS_CLOSE_STN_REQ_HDR_SZ      sizeof (CloseStnReqh_t)
#define CLS_CLOSE_STN_CNF_HDR_SZ      sizeof (CloseStnCnfh_t)
#define CLS_CONNECT_REQ_HDR_SZ        sizeof (ConnectReqh_t)
#define CLS_CONNECT_CNF_HDR_SZ        sizeof (ConnectCnfh_t)
#define CLS_CONNECT_IND_HDR_SZ        sizeof (ConnectIndh_t)
#define CLS_CONNECT_RSP_HDR_SZ        sizeof (ConnectRsph_t)
#define CLS_CONNECTED_IND_HDR_SZ      sizeof (ConnectedIndh_t)
#define CLS_DISCONNECT_REQ_HDR_SZ     sizeof (DisconnectReqh_t)
#define CLS_DISCONNECT_CNF_HDR_SZ     sizeof (DisconnectCnfh_t)
#define CLS_DISCONNECT_IND_HDR_SZ     sizeof (DisconnectIndh_t)
#define CLS_DISCONNECT_RSP_HDR_SZ     sizeof (DisconnectRsph_t)
#define CLS_ID_REQ_HDR_SZ             sizeof (IdReqh_t)
#define CLS_ID_CNF_HDR_SZ             sizeof (IdCnfh_t)
#define CLS_ID_IND_HDR_SZ             sizeof (IdIndh_t)
#define CLS_ID_RSP_HDR_SZ             sizeof (IdRsph_t)
#define CLS_ID_STN_IND_HDR_SZ         sizeof (IdStnIndh_t)
#define CLS_ID_STN_RSP_HDR_SZ         sizeof (IdStnRsph_t)
#define CLS_SIGNAL_STN_REQ_HDR_SZ     sizeof (SignalStnReqh_t)
#define CLS_SIGNAL_STN_CNF_HDR_SZ     sizeof (SignalStnCnfh_t)
#define CLS_DATA_REQ_HDR_SZ           sizeof (DataReqh_t)
#define CLS_DATA_CNF_HDR_SZ           sizeof (DataCnfh_t)
#define CLS_DATA_IND_HDR_SZ           sizeof (DataIndh_t)

#define CLS_ACT_RING_REQ_HDR_SZ       sizeof (ActRngReqh_t)
#define CLS_ACT_RING_CNF_HDR_SZ       sizeof (ActRngCnfh_t)
#define CLS_DEACT_RING_REQ_HDR_SZ     sizeof (DeActRngReqh_t)
#define CLS_DEACT_RING_CNF_HDR_SZ     sizeof (DeActRngCnfh_t)
#define CLS_ID_STN_REQ_HDR_SZ         sizeof (IdStnReqh_t)
#define CLS_ID_STN_CNF_HDR_SZ         sizeof (IdStnCnfh_t)
#define CLS_UDATA_REQ_HDR_SZ          sizeof (UDataReqh_t)
#define CLS_UDATA_IND_HDR_SZ          sizeof (UDataIndh_t)
#define CLS_UDATA_CNF_HDR_SZ          sizeof (UDataCnfh_t)
#define CLS_TEST_REQ_HDR_SZ           sizeof (TestReqh_t)
#define CLS_TEST_RSP_HDR_SZ           sizeof (TestRsph_t)
#define CLS_TEST_IND_HDR_SZ           sizeof (TestIndh_t)
#define CLS_TEST_CNF_HDR_SZ           sizeof (TestCnfh_t)
#define CLS_TEST_STN_REQ_HDR_SZ       sizeof (TestStnReqh_t)
#define CLS_TEST_STN_RSP_HDR_SZ       sizeof (TestStnRsph_t)
#define CLS_TEST_STN_IND_HDR_SZ       sizeof (TestStnIndh_t)
#define CLS_TEST_STN_CNF_HDR_SZ       sizeof (TestStnCnfh_t)
#define CLS_UDATA_STN_REQ_HDR_SZ      sizeof (UDataStnReqh_t)
#define CLS_UDATA_STN_IND_HDR_SZ      sizeof (UDataStnIndh_t)
#define CLS_UDATA_STN_CNF_HDR_SZ      sizeof (UDataStnCnfh_t)
#define CLS_CONNECT_STN_IND_HDR_SZ    sizeof (ConnectStnIndh_t)
#define CLS_PONG_REQ_HDR_SZ           sizeof (PongReqh_t)
#define CLS_PONG_CNF_HDR_SZ           sizeof (PongCnfh_t)
#define CLS_MODIFY_STN_REQ_HDR_SZ     sizeof (ModifyStnReqh_t)
#define CLS_MODIFY_STN_CNF_HDR_SZ     sizeof (ModifyStnCnfh_t)

/***************************************************************
** Defines for COMMON_STATUS return codes
***************************************************************/

typedef enum  {
    CLS_OK =	            0x7000,
    CLS_OUT_OF_MEMORY,
    CLS_UNKNOWN_PORT,
    CLS_UNKNOWN_SAP,
    CLS_UNKNOWN_RING,
    CLS_UNKNOWN_CEP,
    CLS_UNKNOWN_IDENTIFIER_TYPE,
    CLS_SAP_BUSY,
    CLS_INVALID_Q_PTR,
    CLS_UNKNOWN_PRIMITIVE,
    CLS_UNKNOWN_DLC_TYPE,
    CLS_NO_REM_STN,
    CLS_NO_XID_RSP,
    CLS_DISC_ERROR,
    CLS_CONFIG_CONFLICT,
    CLS_TIMEOUT,
    CLS_CONNECTION_BUSY,
    CLS_ALREADY_ENABLED,
    CLS_NO_SAP_REGISTRY,
    CLS_INVALID_LOCAL_RING,
    CLS_INVALID_TARGET_RING,
    CLS_BAD_ADDRESS,
    CLS_INTERFACE_DOWN,
    CLS_VMAC_OVERLAP,
    CLS_LOCAL_ERROR,
    CLS_VDLC_STATE_ERROR,
    CLS_NULL_MSG,
    CLS_REQUEST_OVERRIDE,
    CLS_INACTIVITY_TIMEOUT
} ClsErrorCodeT;


/****************************************************************
** Common CLSI header structure
****************************************************************/

#define CLS_PRIVATE_SIZE 8		/* size of private data area in CLSI header */

#define CLSI_HEADER \
    TCLSIMsg* fLinkPtr;			/* For building linked lists of messages */\
    /* The next three fields along with fLinkPtr are used in APPN for queuing */ \
    void*  fPrevPtr; \
    void*  fDataPtr; \
    word   fFreeElement; \
    byte   fCLSIPrivate[CLS_PRIVATE_SIZE];\
    hword  fHeaderLength;		/* Length of CLSI header (including this field)  */ \
    hword  fPrimitiveCode;		/* Primitive code                          */ \
    hword  fDataSize;			/* Length of data area                     */ \
    byte   fSelector;			/* Selector value                          */ \
    byte   fIdentifierType;		/* Identifier type                         */ \
    word   fIdentifier;			/* Identifier value                        */

struct TCLSIMsg_
{
    CLSI_HEADER
};

/****************************************************************
** Structure for CLSPortIdentifier field
****************************************************************/
typedef enum {
    CLS_P_TYPE_ILLEGAL,
    CLS_P_TYPE_SWIDB
} CLSPortIdentifierType;

typedef struct {
    CLSPortIdentifierType type;
    union {
	idbtype *swidb;
    } fTypeInfo;
} CLSPortID;

idbtype *CLSIPortIdGetSwIdb(CLSPortID *clsPortID);
byte CLSIPortIdGetPortNumber(CLSPortID *clsPortID);
byte CLSIPortIdGetUnitNumber(CLSPortID *clsPortID);
byte CLSIPortIdGetSlotNumber(CLSPortID *clsPortID);
byte CLSIPortIdGetSubNumber(CLSPortID *clsPortID);
inline CLSDLCType_e get_cls_type(hwidbtype *idb);
CLSDLCType_e CLSIPortIdGetPortType(CLSPortID *clsPortID);
boolean CLSIPortIdEqual(CLSPortID* portA, CLSPortID* portB);
boolean CLSIPortIdCopy (CLSPortID* srcPort, CLSPortID* dstPort);



/****************************************************************
** Structure for COMMON_STATUS field in confirms (.cnf)
****************************************************************/

typedef struct
{
    word  fRetCode;		/* origin completion code            */
} Status_t;

/****************************************************************
** Common CLSI header structure for confirm
****************************************************************/

typedef struct
{
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)           */
} TCLSICnfMsg;

/*****************************************************************
** ENABLE.req
******************************************************************/

typedef struct
{
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
} EnableReqh_t;

typedef struct 
{
    EnableReqh_t  fCLSIHdr;	/* CLSI header                        */
			/*                                            */
			/* CLSI Data area                             */
			/* *************                              */

    CLSDLCType_e  fDLCType;     /* port type                          */
    word   fCorrelator;		/* Correlator for Enable              */
    CLSPortID  fPortId;         /* port number                        */
    byte   fParms[0];		/* enable parms                       */
} EnableReq_t;

/*
 * Token Ring Specific port level parameters.
 * On return, these values are filled with the actual values in use by
 * the interface.
 */

typedef struct
{
    word fVersion;			/* Always set to CLSI_VERSION */
    hwaddrtype fLocalMac;		/* Address of local TR interface */
    hword fTrSpeed;			/* Speed of the local TR interface */
    hword fMaxIFieldLength;		/* Max I-field length on interface */
} EnableTRParms_t;

/*
 * SDLC Specific port level parameters.
 */

typedef struct
{
    word  fVersion;			/* Always set to CLSI_VERSION */
} EnableSDLCParms_t;

/*
 * QLLC Specific port level parameters.
 */

typedef struct
{
    word  fVersion;			/* Always set to CLSI_VERSION */
} EnableQLLCParms_t;

/*
 * RSRB specific port level parameters. All these parameters must be
 * provided. There are no defaults for mac address, or ring numbers.
 */


/*
 * Defined values for fBridgeNumber
 *
 */
#define CLS_RSRB_DEFAULT_BRIDGE_NUM   0x01

/*
 * Values for fRsrbOptionFlags
 */
#define CLS_RSRB_ENABLE_LACK_ON    0x00000001

typedef struct
{
    word	fVersion;
    hwaddrtype	fVMacAddr;      	/* Virtual MAC address */
    word        fRsrbOptionFlags;       /* LACK on/of, etc... */
    hword	fMaxIFieldLength;       /* size of payload, not including any headers */
    hword       fLocalRing;	        /* CLS local virtual ring             */
    hword       fBridgeNumber;      	/* Bridge number to CLS virtual ring  */
    hword       fTargetRing;	        /* SRB target virtual ring            */
} EnableRSRBParms_t;

typedef struct
{
    word        fVersion;
    ushort	fRingGroup;
    hwaddrtype  fVMacAddr;              /* Virtual MAC address */
    word        fVdlcOptionFlags;       
    hword       fMaxIFieldLength;      
} EnableVDLCParms_t;

typedef struct
{
    word       fVersion;                /* Always set to CLSI_VERSION */
    ipaddrtype fIpAddr;                 /* NCIA ip address */
    uchar      fServerMac[IEEEBYTES];   /* Server Virtual Address */
    uchar      fVMacAddr[IEEEBYTES];    /* CLient Virtual Address */
    ushort     fVMacRange;              /* Mac address range */
    byte       fServerNum;		/* NCIA Server Number */
} EnableNCIAParms_t;


/*
 * Transparent BRIDGE (aka Ethernet) specific parameters Configure
 * parameters for virtual interface which will be connected to
 * transparent bridging group.  Fields:
 *        fVersion  - same as other primitives
 *
 *        fVMacAddr - MAC address used for vidb connected into
 *                    transparent bridge group.  Eventually will
 *                    be important when Spanning tree is to be
 *                    run across DLSw peers.
 *
 *        fTbridgeOptionFlags - currently undefined (1/23/95)
 *                              possible expansion for 80d5, etc...
 *
 *        *fOqueueFn - Legacy oqueue fn of form:
 *                  (*oqueue)(hwidbtype *idb, paktype *pak, enum HEADTAIL which)
 *                     to be used in vidb.  Allows other CLS clients (i.e. APPN,
 *                     DSPU, ...) to use TBRIDGE functionality to capture incoming
 *                     transparent packets.  Also allows transmission to transparent
 *                     bridge groups.  Packet direction (Rx/Tx) determined by
 *                     pak->linktype.  See cls_dlsw_oqueue() as example.
 *
 *        fSpanIndex - bridge group number to which virtual idb will be
 *                     connected.
 */

typedef struct
{
    word	fVersion;
    hwaddrtype	fVMacAddr;      	/* Virtual MAC address */
    word        fTbridgeOptionFlags;    /* To be determined... */
    hword	fMaxIFieldLength;       /* size of payload, not including any headers */
    void        *fOqueueFn;
    byte        fSpanIndex;
} EnableTBRIDGEParms_t;
  
typedef struct
{
    word  fVersion;			/* Always set to CLSI_VERSION */
    hword fMaxIFieldLength;		/* Max I-field length on interface */
} EnableFRParms_t;

/*****************************************************************
** ENABLE.cnf
******************************************************************/

typedef struct
{
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)               */
} EnableCnfh_t;

typedef struct
{
    EnableCnfh_t  fCLSIHdr;	/* CLSI Header                           */
				/*                                       */
				/* CLSI Data area                        */
				/* *************                         */
    CLSDLCType_e  fDLCType;     /* port type                             */
    word   fCorrelator;		/* Correlator for Enable                 */
    CLSPortID  fPortId;         /* port number                        */
    byte   fParms[1];		/* enable parms                          */
} EnableCnf_t;

/*****************************************************************
** DISABLE.req
******************************************************************/

typedef struct
{
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
} DisableReqh_t;

typedef struct
{
    DisableReqh_t  fCLSIHdr;	/* CLSI header                        */
			/*                                            */
			/* CLSI Data area                             */
			/* *************                              */

    CLSDLCType_e  fDLCType;   /* port type                          */
    word   fCorrelator;		/* Correlator for Disable              */
    CLSPortID  fPortId;       /* port number                        */
    byte   fParms[1];           /* disable parms                      */

} DisableReq_t;

/*****************************************************************
** DISABLE.cnf
******************************************************************/
typedef struct
{
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)               */
} DisableCnfh_t;

typedef struct
{
    DisableCnfh_t  fCLSIHdr;	/* CLSI Header                           */
				/*                                       */
				/* CLSI Data area                        */
				/* *************                         */
    CLSDLCType_e  fDLCType;     /* port type                             */
    word   fCorrelator;		/* Correlator for Disable                 */
    CLSPortID  fPortId;         /* port number                        */
    byte   fParms[1];           /* disable parms                      */
} DisableCnf_t;

/****************************************************************
** ACTIVATE_SAP.req
****************************************************************/

typedef struct
{
				/* CLSI header                           */
				/* **********                            */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    word   fUSapId;		/* User SAP id                           */
} ActSapReqh_t;

typedef struct
{
    ActSapReqh_t fCLSIHdr;	/*  CLSI Header                          */
				/*                                       */
				/* CLSI Data area                        */
				/* *************                         */
    word   fVersion;		/* Version number                        */
    word   fCorrelator;		/* Correlator for Activate SAP           */
    CLSDLCType_e  fDLCType;   /* port type */
    CLSPortID  fPortId;       /* port number                        */
    hword fReserved[2];
    byte   fParms[0];		/* Sap parms                             */
} ActSapReq_t;

typedef struct
{
				/* LLC Data                              */
				/* ***************                       */
/*
 * Flags to control whether the SAP will alutomatically send responses to 
 * various kinds of frames it might receive.
 *
 * IEEE XID has a first byte of 0x81. All others fall into the other XID 
 * category.
 */
#define CLS_TEST_RESPONSE      0x00000001 /* Sap should respond to tests */
#define CLS_IEEE_XID_RESPONSE  0x00000002 /* Sap should respond to IEEE XID */
#define CLS_XID_RESPONSE       0x00000004 /* Sap should respond to other XIDs */

/*
 * Flags to control whether the SAP will notify the DLU of the reception
 * of various kinds of frames it might receive
 */
#define CLS_TEST_NOTIFY        0x00000008 /* Notify DLU of received TEST */
#define CLS_IEEE_XID_NOTIFY    0x00000010 /* Notify DLU of received IEEE XID */
#define CLS_XID_NOTIFY         0x00000020 /* Notify DLU of received XID */

    ulong   fSapFlags;		/* bit 0 XID/TEST capable 1=yes          */

    /*
     * Define default parameters for LLC2 sessions on this SAP
     */
    word   fT1;			/* T1 - Response timer                   */
    word   fT2;			/* T2 - Ack timer                        */
    word   fTi;			/* Ti - Inactivity timer                 */
    word   fXIDTimer;		/* retry time in milliseconds */
    word   fExplorerTimer;
    hword  fMaxIField;		/* Maximum I-field length                */
    byte   fMaxOut;		/* Maxout                                */
    byte   fMaxRetryCount;	/* Max retry count                       */
    byte   fMaxIn;		/* Maxin                                 */
    byte   fDynamicWindow;	/* Reserved                              */
    byte   fMaxOutIncr;		/* Maxout increment                      */
    byte   fXIDRetries;		/* Number of times to try. 0 = infinity */
    byte   fExplorerRetries;
    byte   fL3Pid;		/* RFC1490 Level 3 Protocol ID value */
    byte   fSapValue;		/* LSAP value                            */

    /* 
     * This next field helps us build a response to an IEEE XID and
     * controls whether this sap will accept LLC2 frames
     */
    byte   fUserClass;		/* x'01' = LLC type 1                    */
				/* x'02' = LLC Type 1 and 2              */
} ActSapLLCParms_t;

typedef struct
{
    byte   fSapValue;		/* SAP value is SDLC station address */
    byte   fAddrType;		/* address type can be SDLC or IEEE */
} ActSapSDLCParms_t;

typedef struct
{
    ulong  fSapFlags;		
    byte   fSapValue;	
    byte   fUserClass;
    word   fT1;
    byte   fMaxRetryCount;
} ActSapVDLCParms_t;


typedef struct {
    byte   fSapValue;
    byte   fUserClass;
} ActSapNCIAParms_t;


/************************************************************************
 *									*
 *				QLLC SAP				*
 *									*
 ************************************************************************
 */

/* flags for QLLC SAP, and for Request Open Station Request */

#define	QLLC_PVC		0x001
#define	QLLC_VMACS		0x002
#define	QLLC_PARTNER		0x004
#define QLLC_PROXYXID		0x008
#define QLLC_TR_SAPS		0x010
#define QLLC_REMOTE		0x020
#define QLLC_TIMERS		0x040
#define QLLC_NPSI_POLL		0x080
#define QLLC_DLSW		0x100	/* Just for nv_gen */

struct QSapPVCRangeT_ {
    uint	fPVCLow;
    uint	fPVCHigh;
} ;

struct QTimersT_ {
    /* timers and retries 
     * all of this is up for grabs
     */	
    ulong	fX25Timer;
    ulong	fQLTimer;
    ulong	fQLRetries;
} ;

/* The QSapT is used both for passing in information from the DLU,
 * and for storing information locally.
 *
 * Hence the fields that are reserved for internal use.
 */

struct QSapT_ {
	/* dispatching support */
    union {
    	/* For SVCs the subaddress indentifies the disapatchee	 */
	uchar		fQSapSubaddress[ADDRLEN_X121];
    	/* For PVCs the LNC range indentifies the disapatchee	 */
	QSapPVCRangeT	fQSapPvcRange;

    } 		fDispatch;

    /* You always need flags */
    ulong	fQSapFlags;
    ulong	fMaxIfield;

	/* Proxy XID support */
    ulong	fQSapXid;			/* IDBLK/IDNUM */

    /* Fields to form a token-ring style packet - no rif */
    hwaddrtype 	fPartner;
    uchar	fDsap;		/* DSAP, SSAP, are the one byte fields used */
    uchar	fSsap;		/* in LLC on LANs and Frame Relay	    */
    
    /* Remote  address for callout					*/
    hwaddrtype	fRemote;
    
    /* If necessary, we put further parameters here for X.25 options    */
    QTimersT	fQTimers;    
} ;


typedef struct
{
    QSapT	fQActSap;
    
    /* fields for Virtual Mac Address Ranges
     * Only valid if the flag is set in fQActSap.fQSapFlags
     */
    uchar	fVmacStart[IEEEBYTES];	/* bottom virtual Mac Address */
    uint	fNumVmacs;		/* number of vmacs in the range */
} ActSapQLLCParms_t;


/*****************************************************************
** ACTIVATE_SAP.cnf
******************************************************************/

typedef struct
{
				/* CLSI header                             */
				/* **********                              */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                 */
    word   fPSapId;		/* Provider SAP id                         */
} ActSapCnfh_t;


typedef struct
{
    ActSapCnfh_t  fCLSIHdr;	/* CLSI Header                               */
				/*                                           */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Activate SAP               */
    CLSDLCType_e  fDLCType;   /* port type                                 */
    CLSPortID  fPortId;       /* port number                        */
    hword  fType1Length;	/* length of controller rsrvd area (LLC 1)   */
    hword  fType2Length;	/* length of controller rsrvd area (LLC 2)   */
} ActSapCnf_t;


/*****************************************************************
** DEACTIVATE_SAP.req
******************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    word   fUSapId;		/* User SAP id                               */
} DeactSapReqh_t;

typedef struct
{
    DeactSapReqh_t  fCLSIHdr;	/* CLSI header                               */
				/*                                           */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Deactivate                 */
} DeactSapReq_t;


/*****************************************************************
** DEACTIVATE_SAP.cnf
******************************************************************/

typedef struct
{
				/* CLSI header                              */
				/* **********                               */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                  */
} DeactSapCnfh_t;

typedef struct
{
    DeactSapCnfh_t  fCLSIHdr;	/* CLSI Header                              */
				/* CLSI Data area                           */
				/* *************                            */
    word   fVersion;		/* Version number                           */
    word   fCorrelator;		/* Correlator for Deactivate                */
} DeactSapCnf_t;

/*****************************************************************
** FLOW.req
******************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER

#define CLS_ACTION_STOP_FLOW         0x00
#define CLS_ACTION_RESUME_FLOW       0x01
    word   fFlowAction;		/* Flow control action                       */
} FlowReqh_t;

typedef struct
{
    FlowReqh_t   fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Flow                       */
} FlowReq_t;

/********************************************************************
* FLOW.ind
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    ulong fFlowLevel;		/* Flow control congestion level             */
} FlowIndh_t;

typedef struct
{
    FlowIndh_t   fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Flow                       */
} FlowInd_t;

/********************************************************************
** OPEN_STN.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    word   fUCepId;		/* User CEP id                               */
} OpenStnReqh_t;

typedef struct
{
    OpenStnReqh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Open Station               */
} OpenStnReq_t;

/********************************************************************
** OPEN_STN.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
    word   fPCepId;		/* Provider CEP id                           */
} OpenStnCnfh_t;

typedef struct
{
    OpenStnCnfh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Open Station               */
} OpenStnCnf_t;

/********************************************************************
** REQ_OPNSTN.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    word   fUCepId;		/* User CEP id                               */
    word   fPCepId;		/* CLS_NO_CEP, or value from xxxStn.ind	     */
} ReqOpnStnReqh_t;

typedef enum
{
    CLS_QUEUING_DEFAULT,
    CLS_QUEUING_PRIORITY,
    CLS_QUEUING_CUSTOM,
    CLS_QUEUING_FAIR
} CLSIQueuingType;

struct ReqOpnStnReq_t_
{
    ReqOpnStnReqh_t fCLSIHdr;	/* CLSI Header                               */

				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Req Open Station           */
    CLSIQueuingType fQueuingType; /* Type of queuing parameter */
    ulong  fQueuingParameter;   /* What priority should it have */
    byte   fParms[1];		/* Link Station Parms                        */
};

/*
 * READ ME:  If you make any modifications to the structure ReqOpnStnLLCParms_t_, 
 *           please make the same modifications to the structure ReqOpnStnQLLCParms_t;
 *           otherwise, you will break CLSI-compliant QLLC.  We need to clean this up!
 */

struct ReqOpnStnLLCParms_t_
{
    hwaddrtype   fDestAddr;	/* Remote MAC address */
    hwaddrtype   fSrcAddr;	/* Local MAC address. Use type == */
				/* STATION_ILLEGAL will use the MAC */
				/* address of the local interface */
    word   fT1;			/* T1 - response timer                       */
    word   fT2;			/* T2 - ack timer                            */
    word   fTi;			/* Ti - inactivity timer                     */
    ulong  fFlags;              /* General Flag to be used to pass parameters*/
    hword  fMaxIField;		/* Maximum I-frame length                    */
    byte   fMaxOut;		/* Maxout                                    */
    byte   fMaxRetryCount;	/* Max retry count                           */
    byte   fMaxIn;		/* Maxin                                     */
    byte   fDynamicWindow;	/* Dynamic window option                     */
    byte   fMaxOutIncr;		/* Maxout increment                          */
    byte   fDestSap;		/* Remote SAP address */
    byte   fSrcSap;		/* Local SAP address. 0 means that the */
				/* SAP specified in the ActSapReq */
				/* should be used */

/********************************************************************
** This group of parameters controls retransmission of XID commands.
********************************************************************/

    byte   fXIDRetries;		/* Number of times to try. 0 = infinity */
    word   fXIDTimer;		/* retry time in milliseconds */

/********************************************************************
** This group of parameters is specific to RFC1490 Frame Relay.
********************************************************************/

    byte   fL3Pid;			/* Layer 3 Protocol ID */
		/* See FRF.3 doc */
#define CLS_L3PID_FID4 0x81		/* SNA Subarea (FID4) */
#define CLS_L3PID_FID2 0x82		/* SNA Peripheral (FID2) */
#define CLS_L3PID_APPN 0x83		/* SNA APPN (FID2) */
#define CLS_L3PID_NETBIOS 0x84		/* NetBIOS */
#define CLS_BAN_FLAG      0xCC            /* used as a flag. L3PID will not be used in
                                           encapsulation for BAN. FRAS BAN will be
                                           set to this value*/
    ushort fDlci;                       /* for FRAS BAN implementation */

    
/********************************************************************
** This group of parameters controls behaviour in a SRB environment.
** They are ignored if the media is not using SRB.
********************************************************************/

/*
 * Define _E_xplorer _O_ption bits to modify route determination procedure
 */
#define EO_RIF_SPECIFIED    0x00000001 /* If set this parameter block
				        * contains a RIF. The length
				        * may be zero, but it is the
				        * RIF that should be used. If
				        * clear, explore. */

#define EO_VALIDATE_RIF     0x00000002 /* If EO_RIF_SPECIFIED, send 
					* a directed TEST to validate.
					* If not EO_RIF_SPECIFIED, then
				        * explorer for a route */
/*
 * The next group of flags control what type of explorer will be used
 * to look for a route to the destination. These only apply if
 * EO_VALIDATE_RIF and not EO_RIF_SPECIFIED.
 */

/* Not implemented yet. */
#define EO_EXPLORER_ARE     0x00000004 /* Use all routes explorer if 
				        * set, use spanning tree if
				        * not set. */
/* Not implemented yet. */
#define EO_EXPLORER_TEST    0x00000008 /* If set, use a TEST. If clear
				        * use an IEEE XID */
#define EO_TRANSPARENT_FIRST 0x00000010 /* If set try to find station
				         * on the local ring before
				         * sending and explorer. If
				         * clear, the start with an explorer. */
/* Not implemented yet. */
#define EO_LOCAL_LAN        0x00000020 /* no rif present (transparent) 
					* or RC == '0270' */

/* 
 * These default values are to be used by DLUs such as APPN and DSPU
 * which want to behave as end stations on the ring. 
 * 
 * For an outgoing call setup, we want to check the RIF cache first. If
 * it is there we are done, else send a directed TEST on the local ring.
 * If that times out, then send an all routes explorer TEST frame.
 *
 * For an incoming call, always use the rif that is provided. The DLU
 * is responsible for copying the RIF from the IdStn.ind that indicated
 * the incoming call.
 */

#define EO_OUTBOUND_DEFAULT  (EO_VALIDATE_RIF | EO_EXPLORER_ARE | \
			      EO_EXPLORER_TEST | EO_TRANSPARENT_FIRST )

#define EO_INBOUND_DEFAULT   (EO_RIF_SPECIFIED)

    word   fExplorerTimer;	/* retry time in milliseconds */
    byte   fExplorerOption;	/* See above comment for options */
    byte   fExplorerRetries;	/* Number of times to try. 0 = infinity */
    byte   fRifLength;	        /* routing info length     */
    byte   fRif[1];		/* routing info */
};



/************************************************************************
 *									*
 *			QLLC REQUEST OPEN STATION			*
 *									*
 * As far as possible Request Open Station will inherit values from	*
 * ActivateSap. However the design allows for overriding these values.	*
 *  If override is intended, the flag is set.				*
 *									*
 ************************************************************************
 */

/*
 * READ ME:  This structure ( ReqOpnStnQLLCParms_t) must match the
 *           structure ReqOpnStnLLCParms_t_ up to the field fQReqOpenStnFlags.
 *           Otherwise, CLSI-compliant QLLC will be broken.
 *           We need to do away with this QLLC-specific structure!
 */

typedef struct
{
    hwaddrtype	fDestAddr;  	/* Remote address.  If type is STATION_ILLEGAL
    				 * use the address specified in Activate.Sap
				 * Request.  Can either be X.121 address or
				 * mapped IEEE address
				 */
    hwaddrtype 	fSrcAddr;
    word   	fT1;			/* T1 - response timer                       */
    word   	fT2;			/* T2 - ack timer                            */
    word   	fTi;			/* Ti - inactivity timer                     */
    ulong       fFlags;                 /* General Flag to be used to pass parameters*/
    hword  	fMaxIField;		/* Maximum I-frame length                    */
    byte   	fMaxOut;		/* Maxout                                    */
    byte   	fMaxRetryCount;	/* Max retry count                           */
    byte   	fMaxIn;		/* Maxin                                     */
    byte   	fDynamicWindow;	/* Dynamic window option                     */
    byte   	fMaxOutIncr;		/* Maxout increment                          */
    byte   	fDestSap;		/* Remote SAP address */
    byte   	fSrcSap;		/* Local SAP address. 0 means that the */


/********************************************************************
** This group of parameters controls retransmission of XID commands.
********************************************************************/

    byte   fXIDRetries;		/* Number of times to try. 0 = infinity */
    word   fXIDTimer;		/* retry time in milliseconds */		

    ulong	fQReqOpenStnFlags;	/* always need these */


/*****************************************************************************
 *
 * This group of parameters sets flow control levels for managing output to
 * X.25
 * If the lci_holdq reachs the high water mark we quench input from DLU
 * When the lci_holdq gets back down to the low water mark we unquench 
 * DLU input
 *
 ****************************************************************************/

    byte   	fHighWater;	/* High Water mark on lci hold queue	     */
    byte   	fLowWater;	/* Low Water mark on lci hold queue	     */

    ulong	fProxyXid;		/* IDBLK/IDNUM  */
	/* For DLSw, and media conversion to LLC2 */
    
    QTimersT	fQTimers;    

} ReqOpnStnQLLCParms_t;

/********************************************************************
** REQ_OPNSTN.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
    word   fPCepId;		/* Provider CEP id                           */
} ReqOpnStnCnfh_t;

typedef struct
{
    ReqOpnStnCnfh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator for Request Open Station       */
} ReqOpnStnCnf_t;

/********************************************************************
** CLOSE_STN.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    word   fUCepId;		/* User CEP id                               */

#define CLS_ACTION_NORMAL            0x00
#define CLS_ACTION_FORCED            0x01
    word   fActionCode;		/* 0=normal 1=forced                         */
} CloseStnReqh_t;

typedef struct
{
    CloseStnReqh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} CloseStnReq_t;

/*********************************************************************
** CLOSE_STN.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
} CloseStnCnfh_t;

typedef struct
{
    CloseStnCnfh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} CloseStnCnf_t;

/********************************************************************
** CONNECT.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    hword   fModifyFlags;	/* connect parms to be modified */
} ConnectReqh_t;

typedef struct
{
    ConnectReqh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    CLSIQueuingType fQueuingType; /* Type of queuing parameter */
    ulong  fQueuingParameter;   /* What priority should it have */
    byte   fParms[1];		/* params to modify as per modify_flags      */
} ConnectReq_t;

/*********************************************************************
** CONNECT.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
} ConnectCnfh_t;

typedef struct
{
    ConnectCnfh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;
} ConnectCnf_t;

/*********************************************************************
** CONNECT.ind
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[4];
} ConnectIndh_t;

typedef struct
{
    ConnectIndh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;
} ConnectInd_t;

/********************************************************************
** CONNECT.rsp
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER

#define CLS_ACTION_CONNECT           0x00
#define CLS_ACTION_NO_CONNECT        0x01
    word   fActionCode;		/* 0=connect 1=do not connect                */
    hword   fModifyFlags;	/* parms modified; see connect_req           */
} ConnectRsph_t;

typedef struct
{
    ConnectRsph_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    byte   fFlag;               /* To RNR or not */
    byte   pad[3];              /* fparms must be word aligned               */
    CLSIQueuingType fQueuingType; /* Type of queuing parameter */
    ulong  fQueuingParameter;   /* What priority should it have */
    byte   fParms[1];		/*  parms to change as per fModifyFlags      */
} ConnectRsp_t;

/*********************************************************************
** CONNECTED.ind
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    word   fActionCode;		/* make the ind and req the same length      */
} ConnectedIndh_t;

typedef struct
{
    ConnectedIndh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
                                /* fCorrelator must be word aligned          */
    word   fCorrelator;		/* Correlator                                */
} ConnectedInd_t;

/********************************************************************
** DISCONNECT.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
} DisconnectReqh_t;

typedef struct
{
    DisconnectReqh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} DisconnectReq_t;

/*********************************************************************
** DISCONNECT.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
} DisconnectCnfh_t;

typedef struct
{
    DisconnectCnfh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} DisconnectCnf_t;

/********************************************************************
** DISCONNECT.ind
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER

#define CLS_NORMAL_DISC              0x00        
#define CLS_ERROR_DISC               0x01
#define CLS_TIMEOUT_DISC             0x02
    word   fLinkStatus;		/* Cause for the disconnect                  */
} DisconnectIndh_t;

typedef struct
{
    DisconnectIndh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} DisconnectInd_t;

/********************************************************************
** DISCONNECT.rsp
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER

#define CLS_ACTION_ACCEPT            0x00 /* send UA */
#define CLS_ACTION_REJECT            0x01 /* send DM */
    word   fActionCode;
} DisconnectRsph_t;

typedef struct
{
    DisconnectRsph_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} DisconnectRsp_t;

/********************************************************************
** ID.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte   fReserved[sizeof(Status_t)];
} IdReqh_t;

struct IdReq_t_
{
    IdReqh_t  fCLSIHdr;		/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    hword   fReserveLength;	/* Length of reserve area (rcvd on act_sap.cnf)  */
				/* The reserve area is the first (n) bytes of */
				/* the xid data area.                        */
    hword  pad;                 /* fXID must be word aligned                 */
    byte   fXID[1];		/* XID data to be sent - actual data starts at*/
				/* offset reserve_length.                    */
};

/********************************************************************
** IDSTN.req
********************************************************************/

typedef struct
{
                                /* CLSI header                               */
                                /* **********                                */
    CLSI_HEADER
    word   reserved;            /* make the id.req and id.stn the same len   */
} IdStnReqh_t;

typedef struct
{
    IdStnReqh_t  fCLSIHdr;      /* CLSI Header                               */
                                /* CLSI Data area                            */
                                /* *************                             */
    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area (rcvd on act_sap.cnf) */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fLMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;

                                /* The reserve area is the first (n) bytes of */
                                /* the xid data area.                        */
                                /* */
    byte   fXID[1];             /* XID data to be sent - actual data starts at*/
                                /* offset reserve_length.                    */
}IdStnReq_t;

/********************************************************************
** ID.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
} IdCnfh_t;

typedef struct
{
    IdCnfh_t     fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    hword  fReserveLength;
    byte   fXIDOffset;          /* Offset in bytes from the start of this    */
                                /* structure to the start of the XID data    */
    byte   fXIDLength;          /* Length in bytes of XID data received      */
    byte   fXID[1];		/* XID response data                         */
} IdCnf_t;

/********************************************************************
** IDSTN.cnf
********************************************************************/

typedef struct
{
                                /* CLSI header                               */
                                /* **********                                */
    CLSI_HEADER
    Status_t  fStatus;          /* Common status (retcode)                   */
} IdStnCnfh_t;

typedef struct
{
    IdStnCnfh_t     fCLSIHdr;   /* CLSI Header                               */
                                /* CLSI Data area                            */
                                /* *************                             */
    word   fVersion;            /* Version number                            */
    word   fCorrelator;          /* Correlator                                */
    hword  fReserveLength;
    byte   fXIDOffset;          /* Offset in bytes from the start of this    */
                                /* structure to the start of the XID data    */
    byte   fXIDLength;          /* Length in bytes of XID data received      */
                                /* fXID must be word aligned                 */
    byte   fXID[1];             /* XID response data                         */
} IdStnCnf_t;

/********************************************************************
** ID.ind
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte   fReserved[sizeof(Status_t)];
} IdIndh_t;

typedef struct
{
    IdIndh_t     fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    hword  fReserveLength;	/* Length of reserve area (rcvd on act_sap.cnf)  */
    byte   fXIDOffset;          /* Offset in bytes from the start of this    */
                                /* structure to the start of the XID data    */
    byte   fXIDLength;          /* Length in bytes of XID data received      */
				/* The reserve area is the first (n) bytes of */
				/* the xid data area.                        */
    byte   fXID[1];		/* XID data received                         */
} IdInd_t;


/********************************************************************
** ID_STN.ind
********************************************************************/

typedef struct
{
                                /* CLSI header                               */
                                /* **********                                */
  CLSI_HEADER
  word   fPCepId;             /* Provider CEP id                           */
} IdStnIndh_t;

typedef struct
{
    IdStnIndh_t fCLSIHdr;       /* CLSI Header                               */
                                /* CLSI Data area                            */
                                /* *************                             */
    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */

#define CLS_XID_CMD 0x04
#define CLS_XID_RSP 0x08

    byte   fXIDCmdRsp;          /* x'04' = command                           */
                                /* x'08' = response                          */
#define CLS_XID_POLL_ON  0x04
#define CLS_XID_POLL_OFF 0x08

    byte   fXIDPollFinal;       /* x'04' = P bit off                         */
                                /* x'08' = P bit on                          */
    byte   fMacType;            /* type of the mac layer addressing          */

    byte   fLocalMacLength;     /* length in bytes of local mac address      */
    byte   fLocalMacOffset;     /* Offset in bytes from start of this        */
                                /* structure to the start of the local       */
                                /* MAC address                               */
    byte   fRemoteMacLength;    /* length in bytes of remote mac address     */
    byte   fRemoteMacOffset;    /* Offset in bytes from start of this        */
                                /* structure to the start of the remote      */
                                /* MAC address                               */

    byte   fRoutingLength;      /* Routing information length                */
    byte   fRoutingOffset;      /* Offset from start of this structure to    */
                                /* the start of the routing information      */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fXIDOffset;          /* Offset in bytes from the start of this    */
                                /* structure to the start of the XID data    */
    byte   fXIDLength;          /* Length in bytes of XID data received      */
    byte   pad1;                /* short alignment for fDlci                 */
    ushort fDlci;               /* for Frame Relay BAN implementation.       */
    hword  fReserveLength;      /* Length of reserve area (used for alignment) */
    ushort pad2;                /* align fXIDData to word boundary           */
    byte   fXIDData[0];         /* DO NOT USE!  Start of the recieved packet */
} IdStnInd_t;

/********************************************************************
** ID.rsp
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte   fReserved[sizeof(Status_t)];
} IdRsph_t;

struct IdRsp_t_
{
    IdRsph_t     fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    hword  fReserveLength;	/* Length of reserve area (rcvd on act_sap.cnf)  */
				/* The reserve area is the first (n) bytes of */
				/* the xid data area.                        */
    hword  pad;                 /* fXID must be word aligned                 */
    byte   fXID[1];		/* XID data received                         */
};

/********************************************************************
** IDSTN.rsp
********************************************************************/

typedef struct
{
                                /* CLSI header                               */
                                /* **********                                */
    CLSI_HEADER
    word   fReserved;           /* make the id.ind the same len as the req   */
} IdStnRsph_t;

typedef struct
{
    IdStnRsph_t     fCLSIHdr;   /* CLSI Header                               */
                                /* CLSI Data area                            */
                                /* *************                             */
    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area (rcvd on act_sap.cnf) */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fLMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;
    byte   pad;                 /* fXID must be word aligned                 */

                                /* The reserve area is the first (n) bytes of */
                                /* the xid data area.                        */
    byte   fXID[1];             /* XID data received                         */
}IdStnRsp_t;

/*
 * Returns a pointer to the Local Mac address. 
 * If parameter localMac is not NULL, then also
 * builds a hwaddrtype structure containing the local
 * mac address.
 */
static inline byte* CLSIIdStnGetLocalMac(const IdStnInd_t* idStn, hwaddrtype* localMac);

/*
 * Returns a pointer to the Remote Mac address. 
 * If parameter remoteMac is not NULL, then also
 * builds a hwaddrtype structure containing the remote
 * mac address.
 */
byte* CLSIIdStnGetRemoteMac(const IdStnInd_t* idStn, hwaddrtype* remoteMac);

/*
 * Returns a pointer to the RIF. routingLength is a
 * pointer to a byte where the length of the rif is
 * returned.
 * If there is no RIF, then a pointer of NULL and 
 * a length of 0 will be returned.
 */
static inline byte* CLSIIdStnGetRoute(const IdStnInd_t* idStn, byte* routingLength);

/*
 * Returns a pointer to the XID data. The length of the
 * data is returned in byte addressed by xidLength.
 * If there is no data (NULL XID), then the length 
 * will be 0 and the pointer will be NULL.
 */
static inline byte* CLSIIdStnGetXID(const IdStnInd_t* idStn, byte* xidLength);

/*********************************************************************
** SIGNAL_STN.req
********************************************************************/



typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
#define CLS_ACTION_START             0x00 /* Start */
#define CLS_ACTION_STOP_ORDERLY      0x01 /* Stop orderly */
#define CLS_ACTION_STOP_IMMEDIATE    0x02 /* Stop immediate */
    word   fActionCode;
} SignalStnReqh_t;

typedef struct
{
    SignalStnReqh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} SignalStnReq_t;

/*********************************************************************
** SIGNAL_STN.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    word fReserved;
} SignalStnCnfh_t;

typedef struct
{
    SignalStnCnfh_t fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} SignalStnCnf_t;

/********************************************************************
** DATA.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte   fReserved[sizeof(Status_t)]; /* data ind = data req               */
} DataReqh_t;

struct DataReq_t_
{
    DataReqh_t   fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */

    /* 
     * These next 2 fields are here for use by the DLU. They are never
     * set or interpretted by CLS. Do whatever you want with them.
     * Added for the APPN folks 8/29/94 [sberl]
     */
    byte  *fDataPtr;
    hword  fDataLength;
    hword  fReserveLength;	/* Length of reserve area required by controller. */
                                /* fData must be word aligned                */
    byte   fData [1];		/* The first (n) bytes of the data area are  */
				/* reserved for use by the controller. The data  */
				/* begins at offset reserve_length.          */
};

/********************************************************************
** DATA.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
} DataCnfh_t;

typedef struct
{
    DataCnfh_t   fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    hword  fReserveLength;	/* Length of reserve area required by controller. */
    hword pad;
    byte   fData [1];		/* The first (n) bytes of the data area are  */
				/* reserved for use by the controller. The data  */
				/* begins at offset reserve_length.          */
} DataCnf_t;

/********************************************************************
** DATA.ind
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte   fReserved[sizeof(Status_t)]; /* reserved                          */
} DataIndh_t;

struct DataInd_t_
{
    DataIndh_t   fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator */

    /* 
     * These next 2 fields are here for use by the DLU. They are never
     * set or interpretted by CLS. Do whatever you want with them.
     * Added for the APPN folks 8/29/94 [sberl]
     */
    byte  *fDataPtr;
    hword  fDataLength;
    hword  fReserveLength;	/* Length of reserve area required by controller. */
    byte   fData[1];		/* The first (n) bytes of the data area are  */
				/* reserved for use by the controller. The data  */
				/* begins at offset reserve_length.          */
};

/********************************************************************
** PONG.req
********************************************************************/
typedef struct
{
    CLSI_HEADER;
    byte fReserved[sizeof(Status_t)];
    hword fOutboundQEmpty;
  } PongReqh_t;

typedef struct
{
    PongReqh_t   fCLSIHdr;
    byte   fVersion;         /* Version number                       */
    hword  fCorrelator;      /* Correlator                           */
} 
PongReq_t;


/********************************************************************
** PONG.cnf
********************************************************************/
typedef struct
{
    CLSI_HEADER;
    Status_t  fStatus;			/* Common status (retcode)  */
    hword     fNws;			/* Next window size         */
} PongCnfh_t;

typedef struct
{
    PongCnfh_t   fCLSIHdr;
    byte   fVersion;			/* Version number           */
    hword  fCorrelator;			/* Correlator               */
} PongCnf_t;

/*
**  P r o t o t y p e s 
**
**  Publicly declared methods of the TCLSIMsg class
**
** Methods for creating, destroying and manipulating CLSI messages.
*/

/*
 * Create a CLSI message. 
 * The size arguement is the total size of the
 * CLSI header and the data that follows.
 */

TCLSIMsg* CLSIMsgCreateInPak (size_t size);
void CLSIMsgDestroy (TCLSIMsg* theMsg);

static inline TCLSIMsg*  CLSIMsgCreate(size_t size)
{
    return(CLSIMsgCreateInPak(size));
}

/* 
 * Return the usable space available for CLSI message in this buffer.
 * The size returned is from the start of the CLSIMsg header to the
 * end of the buffer.
 */

size_t     CLSIMsgSpaceAvail(TCLSIMsg* theMsg);

/* Returns the space available between the start of the CLSI message */
/* and the start of the buffer. */

size_t     CLSIMsgPrefixAvail(TCLSIMsg* theMsg);

/* Returns the actual size of the CLSI message. This is from the start */
/* of the CLSI message header, to the end of the CLSI message data. */
/* It is the total of theMsg->fHeaderLength, theMsg->fDataSize, and */
/* the sizeof(fLinkPtr) and sizeof(fCLSIPrivate) */

size_t CLSIMsgGetSize(TCLSIMsg* theMsg);

void CLSIMsgSetSize(TCLSIMsg* theMsg, size_t size);

/* Declare a bunch of accessor methods here */

/* Deal with the primitiveCode field of the message */

static inline boolean CLSIMsgIsRequest(const TCLSIMsg* xthis);
static inline boolean CLSIMsgIsConfirm(const TCLSIMsg* xthis);
static inline boolean CLSIMsgIsIndication(const TCLSIMsg* xthis);
static inline boolean CLSIMsgIsResponse(const TCLSIMsg* xthis);

static inline hword CLSIMsgGetPrimitiveCommand(const TCLSIMsg* xthis);
					/* Get just command */

static inline hword CLSIMsgGetPrimitiveType(const TCLSIMsg* xthis); 
                                        /* Get just the type */

static inline hword CLSIMsgGetPrimitiveCode(const TCLSIMsg* xthis); 
                                        /* Get the whole thing */

/*static inline void CLSIMsgSetPrimitiveCommand(TCLSIMsg* xthis, hword command);*/
static inline void CLSIMsgSetPrimitiveType(TCLSIMsg* xthis, hword type);
/*static inline void CLSIMsgSetPrimitiveCode(TCLSIMsg* xthis, hword code);*/

static inline void CLSIMsgSetDestination(TCLSIMsg* xthis, 
					 CLSIdentifierType_e idType,
					 word id);
static inline CLSIdentifierType_e CLSIMsgGetIdentifierType(TCLSIMsg* xthis);
static inline word CLSIMsgGetIdentifier(TCLSIMsg* xthis);

static inline void CLSIMsgSetReturnCode(TCLSIMsg* xthis, word errorCode);
static inline word CLSIMsgGetReturnCode(const TCLSIMsg* xthis);

/* Deal with the link field of the CLSI message */
static inline TCLSIMsg* CLSIMsgGetLink (const TCLSIMsg* theMsg);
static inline void CLSIMsgSetLink (TCLSIMsg* theMsg, TCLSIMsg* nextMsg);

/*
 * Get the number of available slots in the send window from the 
 * PONG confirm message
 */
static inline int CLSIMsgGetNextWindow(PongCnf_t* this);

/*
 * THIS IS IMPORTANT! USE THESE UNLESS THERE IS GOOD REASON!
 *
 * Use these methods for getting at the data part of the DATA, 
 * and ID indication messages. Returns a pointer to the 
 * first byte of user data. Accounts for the reserved area, 
 * the CLSI headers, and will support data and header that are 
 * not contiguous.
 */

static inline byte* CLSIDataIndGetData(DataInd_t*, size_t* dataLength);
static inline byte* CLSIIdIndGetData(IdInd_t*, size_t* xidLength);

/*
 * Create the  CLSI message for the specified primitive type.
 * Allocates the buffer, and fills in the following  header fields.
 * headerLength, primitiveCode, dataSize, selector, identifierType,
 * and identifier. The rest of the header fields and the data area
 * are the callers responsibility. It is guarenteed that there are at
 * least dataSize bytes of space available for the user data.
 */

TCLSIMsg* CLSIMsgCreateHeader(hword primitiveCode,
			      hword dataSize,
			      byte selector,
			      byte identifierType,
                              word identifier);

/* Create a CLSIMsg from a packet received from the DLC */
 /* thePacket->datagramstart points at the start of the user data and */
 /* thePacket->datagramsize is the size of the user data. User data is */
 /* considered to start after the end of the MAC and LLC layer */
 /* headers. For an XID it is the byte after the 0xAF/0xBF. For I-frames */
 /* it is the start of the TH header. primitiveCode will be either */
 /* ID|INDICATION or DATA|INDICATION or ID_STN|INDICATION. If it is */
 /* ID_STN, then we need to allow space for variable length data in */
 /* message, so pass in the RIF length in the varSize parameter. */

TCLSIMsg* CLSIMsgCreateFromPak(paktype* thePacket,
			       hword primitiveCode,
			       byte selector,
			       byte identifierType,
			       word identifer,
			       size_t varSize,
			       hword* reservedSize);
			       
paktype* CLSIMsgGetPak(TCLSIMsg* xthis);

llctype* CLSIMsgGetLlc(TCLSIMsg* xthis);

size_t CLSIGetHdrSize(hword primitiveCode);

const char* CLSSelectorCodeToString(const CLSSelectorCode_e selectorCode);
const char* CLSIdentifierTypeToString(const CLSIdentifierType_e identifierType);
const char* CLSPrimitiveCommandToString(const hword primitiveCode);
const char* CLSPrimitiveTypeToString(const hword primitiveCode);
char* CLSIMsgSprintf(const TCLSIMsg* this, char* buffer);
char* CLSIMsgSprintfShort(const TCLSIMsg* this, char* buffer);
char* CLSReturnCodeToString(hword retCode);

static inline TCLSIMsg* CLSIMsgGetLink (const TCLSIMsg* theMsg)
{
    return theMsg->fLinkPtr;
}

static inline void CLSIMsgSetLink (TCLSIMsg* theMsg, TCLSIMsg* nextMsg)
{
    theMsg->fLinkPtr = nextMsg;
}

static inline hword CLSIMsgGetPrimitiveType (const TCLSIMsg* this)
{
    return(GETSHORT(&this->fPrimitiveCode) & CLS_TYPE_MASK);
}

static inline hword CLSIMsgGetPrimitiveCommand (const TCLSIMsg* this)
{
    return (GETSHORT(&this->fPrimitiveCode) & CLS_COMMAND_MASK);
}

static inline hword CLSIMsgGetPrimitiveCode (const TCLSIMsg* this)
{
    return(GETSHORT(&this->fPrimitiveCode));
}

static inline void CLSIMsgSetPrimitiveType (TCLSIMsg* this, hword theType)
{
    this->fPrimitiveCode &= ~CLS_TYPE_MASK;
    this->fPrimitiveCode |= theType;
}

static inline boolean	CLSIMsgIsRequest (const TCLSIMsg* this)
{
    if (CLSIMsgGetPrimitiveType(this) == CLS_REQUEST)
	return (TRUE);
    else
	return (FALSE);
}

static inline boolean	CLSIMsgIsConfirm (const TCLSIMsg* this)
{
    if (CLSIMsgGetPrimitiveType(this) == CLS_CONFIRM)
	return (TRUE);
    else
	return (FALSE);
}

static inline boolean	CLSIMsgIsIndication (const TCLSIMsg* this)
{
    if (CLSIMsgGetPrimitiveType(this) == CLS_INDICATION)
	return (TRUE);
    else
	return (FALSE);
}

static inline boolean	CLSIMsgIsResponse (const TCLSIMsg* this)
{
    if (CLSIMsgGetPrimitiveType(this) == CLS_RESPONSE)
	return (TRUE);
    else
	return (FALSE);
}

static inline void CLSIMsgSetReturnCode (TCLSIMsg* this, word errorCode)
{
    ((TCLSICnfMsg*)this)->fStatus.fRetCode = errorCode;
}

static inline word CLSIMsgGetReturnCode (const TCLSIMsg* this)
{
    return(((TCLSICnfMsg*)this)->fStatus.fRetCode);
}

static inline void CLSIMsgSetDestination(TCLSIMsg* xthis, 
					 CLSIdentifierType_e idType,
					 word id)
{
    xthis->fIdentifierType = idType;
    xthis->fIdentifier = id;
}

static inline CLSIdentifierType_e CLSIMsgGetIdentifierType(TCLSIMsg* xthis)
{
    return(xthis->fIdentifierType);
}
    
static inline word CLSIMsgGetIdentifier(TCLSIMsg* xthis)
{
    return(xthis->fIdentifierType);
}

/*
 * Returns a pointer to the Local Mac address. 
 * If parameter localMac is not NULL, then also
 * builds a hwaddrtype structure containing the local
 * mac address.
 */
static inline byte* CLSIIdStnGetLocalMac(const IdStnInd_t* idStn, hwaddrtype* localMac)
{
    byte* macPtr = ((byte*) idStn) + idStn->fLocalMacOffset;
    
    if (localMac != NULL)
    {
	snpa_init(localMac, idStn->fMacType, TARGET_UNICAST, macPtr);
    }
    return macPtr;
}

/*
 * Returns a pointer to the RIF. routingLength is a
 * pointer to a byte where the length of the rif is
 * returned.
 * If there is no RIF, then a pointer of NULL and 
 * a length of 0 will be returned.
 */
static inline byte* CLSIIdStnGetRoute(const IdStnInd_t* idStn, byte* routingLength)
{
    byte* rifPtr;

    *routingLength = idStn->fRoutingLength;
    if (*routingLength > 0)
	rifPtr = ((byte*) idStn) + idStn->fRoutingOffset;
    else
	rifPtr = NULL;
    
    return rifPtr;
}

/*
 * Returns a pointer to the XID data. The length of the
 * data is returned in byte addressed by xidLength.
 * If there is no data (NULL XID), then the length 
 * will be 0 and the pointer will be NULL.
 */
static inline byte* CLSIIdStnGetXID(const IdStnInd_t* idStn, byte* xidLength)
{
    byte* xidPtr;

    *xidLength = idStn->fXIDLength;
    if (*xidLength > 0)
	xidPtr = ((byte*) idStn) + idStn->fXIDOffset;
    else
	xidPtr = NULL;
    
    return xidPtr;
}    

/*
 * ACTIVATE_RING.req
 *
 * WARNING: fields must match those of ActRngCnfh_t and ActRngCnf_t
 *          in offset and length up to the fPortType field.
 */
 
typedef struct
{
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    word   fUSapId;             /* User SAP id                           */
} ActRngReqh_t;

 
typedef struct
{
    ActRngReqh_t fCLSIHdr;      
                                
    word   fVersion;            /* Version number                        */
    word   fCorrelator;         /* Correlator for Activate SAP           */
    CLSDLCType_e  fDLCType;     /* port type                             */
    CLSPortID  fPortId;         /* correlator for enable                 */
    hword  fLocalRing;	        /* local ring                            */
    hword  fBridgeNumber;       /* BridgeNumber                          */
    hword  fTargetRing;	        /* tring ring                            */
    hword  fReserved;           /* Reserved                              */
    byte   fParms[0];           /* Rng parms                             */
} ActRngReq_t;
 
typedef struct
{
    ulong  fSapFlags;           /* bit 0 XID/TEST caple 1=yes            */
    word   fT1;                 /* T1 - Response timer                   */
    word   fT2;                 /* T2 - Ack timer                        */
    word   fTi;                 /* Ti - Inactivity timer                 */
    word   fXIDTimer;           /* retry time in milliseconds            */
    word   fExplorerTimer; 
    hword  fMaxIField;          /* Maximum I-field length                */
    byte   fMaxOut;             /* Maxout                                */
    byte   fMaxRetryCount;      /* Max retry count                       */
    byte   fMaxIn;              /* Maxin                                 */
    byte   fDynamicWindow;      /* Reserved                              */
    byte   fMaxOutIncr;         /* Maxout increment                      */
    byte   fXIDRetries;         /* Number of times to try. 0 = infinity  */
    byte   fExplorerRetries;    
    byte   fSapValue;           /* LSAP value                            */
    byte   fUserClass;          /* x'01' = LLC type 1                    */
                                /* x'02' = LLC Type 1 and 2              */
 
} ActRngTRParms_t;

typedef struct
{
    byte   fOptions;            /* bit 0-2 Access priority               */
                                /* bit 3  reserved                       */
                                /* bit 4  802.2 SAP      1=no            */

    byte   fTestXidCapable;     /* bit 0 XID/TEST capable 1=yes          */
                                /* bit 1-7 reserved                      */
    word   fT1;                 /* T1 - Response timer                   */
    word   fT2;                 /* T2 - Ack timer                        */
    word   fTi;                 /* Ti - Inactivity timer                 */
    byte   fMaxOut;             /* Maxout                                */
    byte   fMaxRetryCount;      /* Max retry count                       */
    byte   fMaxIn;              /* Maxin                                 */
    byte   fDynamicWindow;      /* Reserved                              */
    byte   fMaxOutIncr;         /* Maxout increment                      */
    hword  fMaxIField;          /* Maximum I-field length                */

/********************************************************************
** This group of parameters controls retransmission of XID commands.
********************************************************************/

    byte   fXIDRetries;          /* Number of times to try. 0 = infinity */
    word   fXIDTimer;            /* retry time in milliseconds */
} ActRngLLCParms_t;

typedef ActRngLLCParms_t  ActRngSDLCParms_t;
typedef ActSapQLLCParms_t ActRngQLLCParms_t;
typedef ActSapVDLCParms_t ActRngVDLCParms_t;
typedef ActSapNCIAParms_t ActRngNCIAParms_t;

/*****************************************************************
** ACTIVATE_RING.cnf
**
** WARNING: fields must match those of ActRngReqh_t and ActRngReq_t
**          in offset and length up to the fPortType field.
**
******************************************************************/
 
typedef struct
{
    CLSI_HEADER
    Status_t  fStatus;          /* Common status (retcode)                 */
    word   fPSapId;             /* Provider SAP id                         */
} ActRngCnfh_t;
 
 
typedef struct
{
    ActRngCnfh_t  fCLSIHdr;     

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator for Activate SAP               */
    CLSDLCType_e  fDLCType;     /* port type                                 */
    CLSPortID  fPortId;         /* port number                               */
    hword  fLocalRing;	        /* local ring                            */
    hword  fBridgeNumber;       /* BridgeNumber                          */
    hword  fTargetRing;	        /* tring ring                                */
} ActRngCnf_t;
 

/*
 * DEACTIVATE_RING.req
 *
 * WARNING: Fields in DeActRngReqh_t and DeActRngReq_t up to and including
 *          the fPortNumber field must match the fields in DeActRngCnfh_t
 *          and DeActRngCnf_t in offset and length.
 */
 
typedef struct
{
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)]; 
    word fUSapId;
} DeActRngReqh_t;

 
typedef struct
{
    DeActRngReqh_t fCLSIHdr;    
    word   fVersion;            /* Version number                        */
    word   fCorrelator;         /* Correlator for Activate SAP           */
} DeActRngReq_t;
 
typedef struct
{
    byte   fOptions;            /* bit 0-2 Access priority               */
                                /* bit 3  reserved                       */
                                /* bit 4  802.2 SAP      1=no            */
 
    byte   fTestXIDCapable;        /* bit 0 XID/TEST capable 1=yes          */
                                /* bit 1-7 reserved                      */
    word   fT1;                 /* T1 - Response timer                   */
    word   fT2;                 /* T2 - Ack timer                        */
    word   fTi;                 /* Ti - Inactivity timer                 */
    byte   fMaxOut;             /* Maxout                                */
    byte   fMaxRetryCount;      /* Max retry count                       */
    byte   fMaxIn;              /* Maxin                                 */
    byte   fDynamicWindow;      /* Reserved                              */
    byte   fMaxOutIncr;         /* Maxout increment                      */
 
    hword  fMaxIField;          /* Maximum I-field length                */
 
/********************************************************************
** This group of parameters controls retransmission of XID commands.
********************************************************************/
 
    byte   fXIDRetries;                 /* Number of times to try. 0 = infinity */
    word   fXIDTimer;                   /* retry time in milliseconds */
} DeActRngTRParms_t;

/*****************************************************************
** DeACTIVATE_RING.cnf
**
** WARNING: Fields in DeActRngCnfh_t and DeActRngCnf_t up to and including
**          the fPortNumber field must match the fields in DeActRngReqh_t
**          and DeActRngReq_t in offset and length.
**
******************************************************************/
 
typedef struct
{
    CLSI_HEADER
    Status_t  fStatus;          /* Common status (retcode)                 */
    word fUSapId;
} DeActRngCnfh_t;
 

typedef struct
{
    DeActRngCnfh_t  fCLSIHdr; 

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator for Activate SAP               */
} DeActRngCnf_t;
 

typedef struct                  /* these are additional fields that follow the*/
{                               /* routing vector.                           */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fReserved;           /* Reserved                                  */
    byte   pad1;
    hword  fReserveLength;      /*                                           */
    hword  pad2;
    byte   fTEST[1];            /* TEST data received                        */
} TestParms_t;
 

/********************************************************************
** TEST.req
********************************************************************/
 
typedef struct
{
    CLSI_HEADER
    word   fReserved;           /* make the test.ind the same len as the req */
} TestReqh_t;
 
typedef struct
{
    TestReqh_t     fCLSIHdr;   

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    word   fRoutingLength;      /* Routing information length                */
    hword  fReserveLength;      /* Length of reserve area (rcvd on act_sap.cnf) */
    byte   fTEST[1];            /* TEST data received                         */
} TestReq_t;


/********************************************************************
** TESTSTN.req
********************************************************************/
 
typedef struct
{
    CLSI_HEADER
    word   fReserved;           /* make the test.ind the same len as the req */
} TestStnReqh_t;
 
typedef struct
{
    TestStnReqh_t     fCLSIHdr;   

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area (rcvd on act_sap.cnf) */
    byte   fRoutingLength;      /* Routing information length                */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fLMac[IEEEBYTES];	/* Address of local TR interface */
    byte   fRMac[IEEEBYTES];	/* Address of local TR interface */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;
    hword  pad;
                                /* The reserve area is the first (n) bytes of */
                                /* the xid data area.                        */
    byte   fTEST[1];            /* TEST data received                         */
} TestStnReq_t;


/********************************************************************
** TEST.rsp
********************************************************************/
 
typedef struct
{
    CLSI_HEADER
    word   fReserved;           /* make the test.ind the same len as the req */
} TestRsph_t;
 
typedef struct
{
    TestRsph_t     fCLSIHdr;    
    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area (rcvd on act_sap.cnf) */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fLMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;

                                /* The reserve area is the first (n) bytes of */
                                /* the xid data area.                        */
    byte   fTEST[1];            /* XID data received                         */
} TestRsp_t;

/********************************************************************
** TESTSTN.rsp
********************************************************************/
 
typedef struct
{
    CLSI_HEADER
    word   fReserved;           /* make the test.ind the same len as the req */
} TestStnRsph_t;
 
typedef struct
{
    TestStnRsph_t     fCLSIHdr;   

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area (rcvd on act_sap.cnf) */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fLMac[IEEEBYTES];	/* Address of local TR interface */
    byte   fRMac[IEEEBYTES];	/* Address of local TR interface */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;
                                /* The reserve area is the first (n) bytes of */
                                /* the xid data area.                        */
    byte   fTEST[1];            /* XID data received                         */
} TestStnRsp_t;


/*****************************************************************
** TEST.ind
******************************************************************/

typedef struct
{
    CLSI_HEADER
/*    word   fReserved;		* make the id.ind the same len as the req   */
    byte   fReserved[sizeof(Status_t)];
} TestIndh_t;

typedef struct
{
    TestIndh_t     fCLSIHdr;

    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    hword  fReserveLength;	/* Length of reserve area (rcvd on act_sap.cnf)  */ 
    byte   fTESTOffset;          /* Offset in bytes from the start of this    */
    byte   fTESTLength;		/* Test data received                        */
    byte   fTEST[0];		/* Test data received                        */
} TestInd_t;


/*****************************************************************
** TEST_STN.ind
******************************************************************/

typedef struct
{
    CLSI_HEADER
} TestStnIndh_t;
 
typedef struct
{
    TestStnIndh_t fCLSIHdr;  

    byte   fVersion;            /* Version number                            */
    byte   fMacPriority;        /* MAC level priority                        */
    word   fCorrelator;         /* Correlator                                */

 
#define CLS_TEST_CMD 0x04
#define CLS_TEST_RSP 0x08
 
    byte   fTESTCmdRsp;         /* x'04' = command                           */
                                /* x'08' = response                          */
#define CLS_TEST_POLL_ON  0x04
#define CLS_TEST_POLL_OFF 0x08
 
    byte   fTESTPollFinal;       /* x'04' = P bit off                         */
                                /* x'08' = P bit on                          */

    byte   fMacType;            /* type of the mac layer addressing          */
    byte   fLocalMacLength;     /* length in bytes of local mac address      */
    byte   fLocalMacOffset;     /* Offset in bytes from start of this        */
                                /* structure to the start of the local       */
                                /* MAC address                               */
    byte   fRemoteMacLength;    /* length in bytes of remote mac address     */
    byte   fRemoteMacOffset;    /* Offset in bytes from start of this        */
                                /* structure to the start of the remote      */
                                /* MAC address                               */
    byte   fRoutingLength;      /* Routing information length                */
    byte   fRoutingOffset;      /* Offset from start of this structure to    */
                                /* the start of the routing information      */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */

    byte   fTESTOffset;         /* Offset in bytes from the start of this    */
                                /* structure to the start of the XID data    */
/*    byte   pad;                  fTESTLength must be word aligned          */
    byte   fTESTLength;          /* Length in bytes of XID data received      */

} TestStnInd_t;

typedef struct                  /* these are additional fields that follow the*/
{                               /* routing vector.                           */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fReserved;           /* Reserved                                  */
    hword  fReserveLength;      /*                                           */
    byte   fTEST[1];            /* TEST data received                        */
} TestStnParms_t;


/*****************************************************************
** TEST.cnf
******************************************************************/
 
typedef struct
{
    CLSI_HEADER
    Status_t  fStatus;          /* Common status (retcode)                   */
} TestCnfh_t;
 
typedef struct
{
    TestCnfh_t     fCLSIHdr;  

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;
    byte   fTESTOffset;       
    byte   fTESTLength;       
    byte   fTEST[0];            /* TEST response data                        */
} TestCnf_t;
 

/*****************************************************************
** TEST_STN.cnf
******************************************************************/

typedef struct
{
    CLSI_HEADER
} TestStnCnfh_t;
 
typedef struct
{
    TestStnCnfh_t fCLSIHdr;  

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    byte   fMacPriority;        /* MAC level priority                        */
 
#define CLS_TEST_CMD 0x04
#define CLS_TEST_RSP 0x08
 
    byte   fTESTCmdRsp;         /* x'04' = command                           */
                                /* x'08' = response                          */
#define CLS_TEST_POLL_ON  0x04
#define CLS_TEST_POLL_OFF 0x08
 
    byte   fTESTPollFinal;       /* x'04' = P bit off                         */
                                /* x'08' = P bit on                          */

    byte   fMacType;            /* type of the mac layer addressing          */
    byte   fLocalMacLength;     /* length in bytes of local mac address      */
    byte   fLocalMacOffset;     /* Offset in bytes from start of this        */
                                /* structure to the start of the local       */
                                /* MAC address                               */
    byte   fRemoteMacLength;    /* length in bytes of remote mac address     */
    byte   fRemoteMacOffset;    /* Offset in bytes from start of this        */
                                /* structure to the start of the remote      */
                                /* MAC address                               */
    byte   fRoutingLength;      /* Routing information length                */
    byte   fRoutingOffset;      /* Offset from start of this structure to    */
                                /* the start of the routing information      */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */

    byte   fTESTOffset;         /* Offset in bytes from the start of this    */

                                /* structure to the start of the XID data    */
    byte   fTESTLength;         /* Length in bytes of XID data received      */

} TestStnCnf_t;


/*****************************************************************
** UDATA.req 
******************************************************************/
 
typedef struct
{
    CLSI_HEADER
    byte   fPacingIndicator;    /* bit 0   PRQ - pacing request out          */
                                /* bit 1   EWI - end window indicator out    */
                                /* bit 2   PRSP - pacing response in         */
                                /* bit 3   CWRI - change window request in   */
                                /* bit 4   RWI - reset window indicator in   */
                                /* bit 5   reserved                          */
                                /* bit 6.-7 PDU priority                     */
                                /*         00 - low                          */
                                /*         01 - medium                       */
                                /*         10 - high                         */
                                /*         11 - reserved                     */
    byte   fMoreIndicator;      /* More Data indicator outbound              */
                                /* x'00' - last or only segment              */
                                /* x'00' - last or only segment              */
                                /* x'01' - first or middle segment           */
                                /* x'02' - discard previous segment          */
    byte   fSuccessCount;       /* Success count inbound                     */
} UDataReqh_t;
 

typedef struct
{
    UDataReqh_t   fCLSIHdr;  

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area required by controller */
    byte   fUData [1];          /* The first (n) bytes of the data area are  */
                                /* reserved for use by the controller. The data */
                                /* begins at offset reserve_length.          */
} UDataReq_t;
 
 
/*****************************************************************
** UDATASTN.req 
******************************************************************/
 
typedef struct
{
    CLSI_HEADER
    byte   fPacingIndicator;    /* bit 0   PRQ - pacing request out          */
                                /* bit 1   EWI - end window indicator out    */
                                /* bit 2   PRSP - pacing response in         */
                                /* bit 3   CWRI - change window request in   */
                                /* bit 4   RWI - reset window indicator in   */
                                /* bit 5   reserved                          */
                                /* bit 6.-7 PDU priority                     */
                                /*         00 - low                          */
                                /*         01 - medium                       */
                                /*         10 - high                         */
                                /*         11 - reserved                     */
    byte   fMoreIndicator;      /* More Data indicator outbound              */
                                /* x'00' - last or only segment              */
                                /* x'00' - last or only segment              */
                                /* x'01' - first or middle segment           */
                                /* x'02' - discard previous segment          */
    byte   fSuccessCount;       /* Success count inbound                     */
    byte   pad;
} UDataStnReqh_t;
 

typedef struct
{
    UDataStnReqh_t   fCLSIHdr;  

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area required by controller */
    hword  fDLCType;            /* port type                                 */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
    byte   fLMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRMac[IEEEBYTES];    /* Address of local TR interface */
    byte   fRif[TR_MAX_RIF_LENGTH];
    byte   fRifLength;
    byte   pad;                 /* fUData must be word aligned               */

    byte   fUData [1];          /* The first (n) bytes of the data area are  */
                                /* reserved for use by the controller. The data */
                                /* begins at offset reserve_length.          */
} UDataStnReq_t;
 
 
typedef struct
{
    CLSI_HEADER
    byte   fMoreIndicator;      /* More Data Indicator (inbound)             */
                                /* x'00' - last or only data segment         */
                                /* x'01' - 1st or middle data segment        */
                                /* x'02' - discard previous segment          */
    byte   fPacingIndicator;    /* bit 0   PRQ - pacing request out          */
                                /* bit 1   EWI - end window indicator out    */
                                /* bit 2   PRSP - pacing response in         */
                                /* bit 3   CWRI - change window request in   */
                                /* bit 4   RWI - reset window indicator in   */
                                /* bit 5-7 reserved                          */
    byte   fSuccessCount;       /* Number of successful transmissions outbound*/
    byte   pad;
} UDataIndh_t;

typedef struct
{
    UDataIndh_t   fCLSIHdr;     

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area required by controller */
    byte  *fUDataPtr;
    hword   fUDataLength;       /* Length in bytes of Udata received         */
    hword  pad;                 /* fUData must be word aligned               */
    byte   fUData[1];           /* The first (n) bytes of the data area are  */
                                /* reserved for use by the controller. The data */
                                /* begins at offset reserve_length.          */
} UDataInd_t;


/*****************************************************************
** UDATASTN.ind 
******************************************************************/

typedef struct
{
    CLSI_HEADER
} UDataStnIndh_t;

typedef struct
{
    UDataStnIndh_t   fCLSIHdr;

    word   fVersion;            /* Version number                            */
    hword  fReserveLength;      /* Length of reserve area required by controller */

    byte   fMacType;            /* type of the mac layer addressing          */
    byte   fLocalMacLength;     /* length in bytes of local mac address      */
    byte   fLocalMacOffset;     /* Offset in bytes from start of this        */
                                /* structure to the start of the local       */
                                /* MAC address                               */
    byte   fRemoteMacLength;    /* length in bytes of remote mac address     */
    byte   fRemoteMacOffset;    /* Offset in bytes from start of this        */
                                /* structure to the start of the remote      */
                                /* MAC address                               */
    byte   fRoutingLength;      /* Routing information length                */
    byte   fRoutingOffset;      /* Offset from start of this structure to    */
                                /* the start of the routing information      */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */
 
    byte   fUDataOffset;        /* Offset in bytes from the start of this    */
                                /* structure to the start of the Udata       */
    hword  fUDataLength;         /* Length in bytes of Udata received         */

                                /* fUData must be word aligned               */
    byte   fUData[1];           /* The first (n) bytes of the data area are  */
                                /* reserved for use by the controller. The data
 */
                                /* begins at offset reserve_length.          */
} UDataStnInd_t;
 

/*****************************************************************
** UDATA.cnf 
******************************************************************/

typedef struct
{
    CLSI_HEADER
    byte   fMoreIndicator;      /* More Data Indicator (inbound)             */
                                /* x'00' - last or only data segment         */
                                /* x'01' - 1st or middle data segment        */
                                /* x'02' - discard previous segment          */
    byte   fPacingIndicator;    /* bit 0   PRQ - pacing request out          */
                                /* bit 1   EWI - end window indicator out    */
                                /* bit 2   PRSP - pacing response in         */
                                /* bit 3   CWRI - change window request in   */
                                /* bit 4   RWI - reset window indicator in   */
                                /* bit 5-7 reserved                          */
    byte   fSuccessCount;       /* Number of successful transmissions outbound*/
    byte   pad;
} UDataCnfh_t;

typedef struct
{
    UDataCnfh_t   fCLSIHdr;     

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area required by controller */
    byte   fUData[1];           /* The first (n) bytes of the data area are  */
                                /* reserved for use by the controller. The data */
                                /* begins at offset reserve_length.          */
} UDataCnf_t;


/*****************************************************************
** UDATASTN.cnf 
******************************************************************/

typedef struct
{
    CLSI_HEADER
    byte   fMoreIndicator;      /* More Data Indicator (inbound)             */
                                /* x'00' - last or only data segment         */
                                /* x'01' - 1st or middle data segment        */
                                /* x'02' - discard previous segment          */
    byte   fPacingIndicator;    /* bit 0   PRQ - pacing request out          */
                                /* bit 1   EWI - end window indicator out    */
                                /* bit 2   PRSP - pacing response in         */
                                /* bit 3   CWRI - change window request in   */
                                /* bit 4   RWI - reset window indicator in   */
                                /* bit 5-7 reserved                          */
    byte   fSuccessCount;       /* Number of successful transmissions outbound*/
    byte   pad;
} UDataStnCnfh_t;

typedef struct
{
    UDataStnCnfh_t   fCLSIHdr;     

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    hword  fReserveLength;      /* Length of reserve area required by controller */
    byte   fUData[1];           /* The first (n) bytes of the data area are  */
                                /* reserved for use by the controller. The data */
                                /* begins at offset reserve_length.          */
} UDataStnCnf_t;


/*****************************************************************
** CONNECT_STN.ind
******************************************************************/

typedef struct
{
    CLSI_HEADER
} ConnectStnIndh_t;
 
typedef struct
{
    ConnectStnIndh_t fCLSIHdr;  

    word   fVersion;            /* Version number                            */
    word   fCorrelator;         /* Correlator                                */
    byte   fMacPriority;        /* MAC level priority                        */
 
#define CLS_CONNECT_POLL_ON  0x04
#define CLS_CONNECT_POLL_OFF 0x08
 
    byte   fCONNECTPollFinal;   /* x'04' = P bit off                         */
                                /* x'08' = P bit on                          */

    byte   fMacType;            /* type of the mac layer addressing          */
    byte   fLocalMacLength;     /* length in bytes of local mac address      */
    byte   fLocalMacOffset;     /* Offset in bytes from start of this        */
                                /* structure to the start of the local       */
                                /* MAC address                               */
    byte   fRemoteMacLength;    /* length in bytes of remote mac address     */
    byte   fRemoteMacOffset;    /* Offset in bytes from start of this        */
                                /* structure to the start of the remote      */
                                /* MAC address                               */
    byte   fRoutingLength;      /* Routing information length                */
    byte   fRoutingOffset;      /* Offset from start of this structure to    */
                                /* the start of the routing information      */
    byte   fLSap;               /* Local SAP value                           */
    byte   fRSap;               /* Remote SAP value                          */

    byte   fCONNECTOffset;      /* Offset in bytes from the start of this    */
                                /* structure to the start of the XID data    */
    hword  fReserveLength;      /* Length of reserve area (used for alignment) */
    uchar  pad;                 /* maintain length of 4 byte multiple        */
    byte   fCONNECTLength;      /* Length in bytes of XID data received      */
                                /* fCONNECTLength must be last struct field  */
} ConnectStnInd_t;

/********************************************************************
** MODIFY_STN.req
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    byte fReserved[sizeof(Status_t)];
    hword   fModifyFlags;	/* connect parms to be modified */
    /* Use the same flags as ConnectReq */
} ModifyStnReqh_t;

typedef struct
{
    ModifyStnReqh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
    CLSIQueuingType fQueuingType; /* Type of queuing parameter */
    ulong  fQueuingParameter;   /* What priority should it have */
    byte   fParms[0];		/* params to modify as per modify_flags      */
} ModifyStnReq_t;

/*********************************************************************
** MODIFY_STN.cnf
********************************************************************/

typedef struct
{
				/* CLSI header                               */
				/* **********                                */
    CLSI_HEADER
    Status_t  fStatus;		/* Common status (retcode)                   */
} ModifyStnCnfh_t;

typedef struct
{
    ModifyStnCnfh_t  fCLSIHdr;	/* CLSI Header                               */
				/* CLSI Data area                            */
				/* *************                             */
    word   fVersion;		/* Version number                            */
    word   fCorrelator;		/* Correlator                                */
} ModifyStnCnf_t;


static inline byte* CLSIDataIndGetData(DataInd_t* dataInd,
				       size_t* dataLength)
{
    *dataLength = dataInd->fCLSIHdr.fDataSize - 
	((offsetof(DataInd_t,fData) - offsetof(DataInd_t,fVersion)) +
	 dataInd->fReserveLength);
    return( (byte *)dataInd->fData + dataInd->fReserveLength);
}

static inline byte* CLSIIdIndGetData(IdInd_t* idInd, size_t* xidLength)
{
    *xidLength = idInd->fCLSIHdr.fDataSize - 
	((offsetof(IdInd_t,fXID) - offsetof(IdInd_t,fVersion)) +
	 idInd->fReserveLength);
    return((byte *)idInd->fXID + idInd->fReserveLength);
}

static inline int CLSIMsgGetNextWindow(PongCnf_t* this)
{
    return(this->fCLSIHdr.fNws);
}

/*
 * Returns a pointer to the Local Mac address. 
 * If parameter localMac is not NULL, then also
 * builds a hwaddrtype structure containing the local
 * mac address.
 */
static inline byte* CLSIConnectStnGetLocalMac(const ConnectStnInd_t* connectStn, hwaddrtype* localMac)
{
    byte* macPtr = ((byte*) connectStn) + connectStn->fLocalMacOffset;
    
    if (localMac != NULL)
    {
	snpa_init(localMac, connectStn->fMacType, TARGET_UNICAST, macPtr);
    }
    return macPtr;
}

/*
 * Returns a pointer to the Remote Mac address. 
 * If parameter remoteMac is not NULL, then also
 * builds a hwaddrtype structure containing the remote
 * mac address.
 */
byte* CLSIConnectStnGetRemoteMac(const ConnectStnInd_t* connectStn, hwaddrtype* remoteMac);

/* 
 * QLLC Activate.SAP and RequestOpen Station Flag Accessor functions 
 *
 * Test, Set, and Clear Accessors are provided for each flag
 */

/* PVC */

static inline boolean QTestPvc (ulong *qflags)
{   return((*qflags) & QLLC_PVC);	}
static inline void QSetPvc (ulong *qflags)
{   *qflags |= QLLC_PVC;		}
static inline void QClearPvc (ulong *qflags)
{   *qflags &= ~QLLC_PVC;		}

/* VMACS */

static inline boolean QTestVmac (ulong *qflags)
{   return((*qflags) & QLLC_VMACS);	}
static inline void QSetVmac (ulong *qflags)
{   *qflags |= QLLC_VMACS;		}
static inline void QClearVmac (ulong *qflags)
{   *qflags &= ~QLLC_VMACS;		}
	
/* PARTNER */

static inline boolean QTestPartner (ulong *qflags)
{   return((*qflags) & QLLC_PARTNER);	}
static inline void QSetPartner (ulong *qflags)
{   *qflags |= QLLC_PARTNER;		}
static inline void QClearPartner (ulong *qflags)
{   *qflags &= ~QLLC_PARTNER;		}
	
/* PROXY XID */

static inline boolean QTestProxyXid (ulong *qflags)
{   return((*qflags) & QLLC_PROXYXID);	}
static inline void QSetProxyXid (ulong *qflags)
{   *qflags |= QLLC_PROXYXID;		}
static inline void QClearProxyXid (ulong *qflags)
{   *qflags &= ~QLLC_PROXYXID;		}
	
/* Token Ring SAPs */

static inline boolean QTestTRSaps (ulong *qflags)
{   return((*qflags) & QLLC_TR_SAPS);	}
static inline void QSetTRSaps (ulong *qflags)
{   *qflags |= QLLC_TR_SAPS;		}
static inline void QClearTRSaps (ulong *qflags)
{   *qflags &= ~QLLC_TR_SAPS;		}

/* REMOTE ADDRESS */

static inline boolean QTestRemote (ulong *qflags)
{   return((*qflags) & QLLC_REMOTE);	}
static inline void QSetRemote (ulong *qflags)
{   *qflags |= QLLC_REMOTE;		}
static inline void QClearRemote (ulong *qflags)
{   *qflags &= ~QLLC_REMOTE;		}

/* TIMERS */

static inline boolean QTestTimers (ulong *qflags)
{   return((*qflags) & QLLC_TIMERS);	}
static inline void QSetTimers (ulong *qflags)
{   *qflags |= QLLC_TIMERS;		}
static inline void QClearTimers (ulong *qflags)
{   *qflags &= ~QLLC_TIMERS;		}

/* NPSI-POLL */
static inline boolean QTestNpsiPoll (ulong *qflags)
{   return((*qflags) & QLLC_NPSI_POLL);	}
static inline void QSetNpsiPoll (ulong *qflags)
{   *qflags |= QLLC_NPSI_POLL;		}
static inline void QClearNpsiPoll (ulong *qflags)
{   *qflags &= ~QLLC_NPSI_POLL;		}

/* DLSW */

static inline boolean QTestDlsw (ulong *qflags)
{   return((*qflags) & QLLC_DLSW);	}

/* ALL Flags */

static inline void QClearAllFlags (ulong *qflags)
{   *qflags = 0;			}

#endif __CLSIMSG_H__

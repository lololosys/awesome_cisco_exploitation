/* $Id: dlc.h,v 3.2.60.3 1996/08/16 07:05:24 ioakley Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlc.h,v $
 *------------------------------------------------------------------
 * dlc.h - interface between cls and all dlc's
 *
 * March, 1994		Phil Morton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * All data types needed to specify the interface between the lower
 * layer of CLS and all the DLC protocols supported.
 *------------------------------------------------------------------
 * $Log: dlc.h,v $
 * Revision 3.2.60.3  1996/08/16  07:05:24  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/10  01:03:07  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:42:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.5  1996/05/03  22:06:09  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.4  1996/05/02  20:23:34  pmorton
 * CSCdi42678:  Router crashed after removing SDLC address
 * Branch: IbuMod_Calif_branch
 * Mark sdlc data block for deletion (zombie).  Don't remove
 * till CLS issues CLOSE Station request.
 *
 * Revision 3.2.48.3  1996/04/24  06:28:15  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.2.48.2  1996/04/05  06:05:25  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.2.48.1  1996/03/28  08:19:30  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.2.4.2  1996/03/03  00:26:15  ioakley
 * CSCdi50585:  Pick up Phil Morton's Vdlc_Rework and
 *              Mark Regan's Appn over DLSw updates.
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2.4.1  1995/12/17  09:50:06  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:55:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:53:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/11  04:20:15  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.4  1995/07/21  17:08:11  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.3  1995/07/19  20:58:26  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.2  1995/06/09  19:27:00  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:20:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * dlc.h
 *
 * CLS - DLC interface definitions
 *
 * A lot of data structures here are represented by unions
 * This lets us use a generic dlc type and still have type checking.
 *
 * Dlc unions are replicated for each field in a paremeter block
 * To make this work we need to include each specific dlc type
 * header.
 *
 *
 * All of this culminates in the parameter block ClsDlcParamBlock
 * which is a single unified structure whose members are specific
 * to each op code.
 * Finally, prototypes for subroutine calls llc_wput and cls_rput
 * are declared

 ************************************************************************
 *									*
 *				IMPORTANT NOTICE			*
 *									*
 * There are a number of routines that assume that corresponding fields	*
 * for IdStn and TestStn control blocks are are the same offsets.       *
 * Additionally, IdStn.ind must start with fields identical to Id.ind.  *
 * dlcsdlc.c in sdlcSendXid counts on this assumption. Update IdStn.ind *
 * and Id.ind with extreme caution. These structure assumptions reduce  *
 * the size of the code and duplicate functions and duplicate	        *
 * bugs.  But it means that until we remove duplicate structure 	*
 * definitions, changes made for one set of data structures must be	*
 * duplicated for other data structures					*
 *									*
 * DO NOT CHANGE OFFSETS FOR ID STATION IND, ID_IND, TEST STATION IND,  *
 * CONNECT STATION IND, OR UDATA STATION IND, WITHOUT BEING SURE THAT	*
 * THIS ALIGNMENT IS PRESERVED.						*
 * OTHERWISE DLSW and VDLC will break !!!				*
 *									*
 * See dlcsdlc.c, dlc.c, dlsw_work, and dlcvdlc.c for places where this *
 * is exploited.							*
 *									*
 ************************************************************************
 */
 

#if (!defined __DLC_H_)
#define __DLC_H_

/* 
 * Define forward references
 */
STRUCTURE_REF(ClsUSapT);        
STRUCTURE_REF(DlcPSapT);        
STRUCTURE_REF(ClsUCepT);       

/*
 * common u_cep_id values
 */
#define NO_CLS ((void *)NULL)

/*
 * ---------------------------------------------------------
 *
 *                        DLC to CLS 
 * Opcodes for the messages passed between major subsystems.
 * ---------------------------------------------------------
 */

typedef enum 
{
    CLS_NO_OP = -1,			/* Invalid */
    CLS_ENABLE_CNF,             
    CLS_DISABLE_CNF,             

    CLS_ACTIVATE_SAP_CNF,
    CLS_MODIFY_SAP_CNF,
    CLS_DEACTIVATE_SAP_CNF,

    CLS_REQ_OPNSTN_CNF,
    CLS_CLOSE_STN_CNF,
    CLS_MODIFY_STN_CNF,

    CLS_TEST_IND,
    CLS_ID_IND,			/* for ID ind, conf, ID_STN ind 	*/

    CLS_TEST_STN_IND,
    CLS_UDATA_STN_IND,
    CLS_UDATA_IND,
    CLS_CONNECT_STN_IND,
    CLS_ID_STN_IND,
    CLS_ACTIVATE_RING_CNF,
    CLS_DEACTIVATE_RING_CNF,

    CLS_CONNECT_IND,
    CLS_CONNECT_CNF,

    CLS_CONNECTED_IND,

    CLS_DISC_IND,
    CLS_DISC_CNF,

    CLS_DATA_IND,
    CLS_DATA_CNF,
    CLS_PONG_CNF,
    CLS_FLOW_IND,

    CLS_MIN_OP = CLS_ENABLE_CNF,
    CLS_MAX_OP = CLS_FLOW_IND,
} ClsOpcodeT;

/* --------------------------------------------------------
 *
 *                        CLS to DLC
 *
 * --------------------------------------------------------
 */

typedef enum 
{
    DLC_INIT_SYSTEM_REQ,

    DLC_ENABLE_REQ,
    DLC_DISABLE_REQ,

    DLC_ACTIVATE_SAP_REQ,
    DLC_MODIFY_SAP_REQ,
    DLC_DEACTIVATE_SAP_REQ,

    DLC_ACTIVATE_RING_REQ,
    DLC_DEACTIVATE_RING_REQ,

    DLC_REQ_OPNSTN_REQ,
    DLC_CLOSE_STN_REQ,
    DLC_MODIFY_STN_REQ,

    DLC_ID_REQ,
    DLC_ID_RSP,
    DLC_ID_STN_REQ,
    DLC_ID_STN_RSP,
    DLC_TEST_REQ,
    DLC_TEST_RSP,
    DLC_TEST_STN_REQ,
    DLC_TEST_STN_RSP,
    DLC_UDATA_REQ,
    DLC_UDATA_STN_REQ,

    DLC_CONNECT_REQ,
    DLC_CONNECT_RSP,
    DLC_SIGNAL_STN_REQ,

    DLC_DISC_REQ,
    DLC_DISC_RSP,

    DLC_DATA_REQ,
    DLC_PONG_REQ,
    DLC_FLOW_REQ,

    DLC_NUM_OPCODES
} DlcOpcodeT;

/*
 * Define major system types
 */

/* CLS's IDs */

typedef ClsUSapT	*ClsUSapIdT;
typedef DlcPSapT	*DlcPSapIdT;
typedef ClsUCepT	*ClsUCepIdT;   


/* DLC's types */

typedef ulong		DlcPCepIdT;	/* I'll cast it */
typedef idbtype		*DlcPPortIdT;

/* 
 * PARAMETER BLOCKS, LISTED BY OP CODE
 */

/* INIT */

typedef struct LlcInitT_
{
    uint		fLlc_maxSaps;
} LlcInitT;

/* parameter block types */

typedef union DlcInitT_
{
    LlcInitT		fLlcInit;
} DlcInitT;

/* 
 * Media specific parameter blocks
 */

typedef struct TREnableT_		/* Token Ring Media Parameters */
{
    uchar	fVersion;
    hwaddrtype	fLocalMac;              /* Local mac addr returned to cls */
    ushort	fTrSpeed;		/* Speed returned to cls */
    ushort	fMaxIFieldLength;       /* size of payload, not including any headers */
} TREnableT;

/*
 * Values for fRsrbOptionFlags
 */
#define DLC_RSRB_ENABLE_LACK_ON    0x00000001

typedef struct RSRBEnableT_		/* RSRB virtual media parameters */
{
    uchar	fVersion;
    hwaddrtype	fVMacAddr;       	/* Virtual MAC address */
    ulong       fRsrbOptionFlags;       /* LACK on/of, etc... */
    ushort	fMaxIFieldLength;       /* size of payload, not including any headers */
    ushort      fLocalRing;             /* CLS local virtual ring             */
    ushort      fBridgeNumber;          /* Bridge number to CLS local vRing   */
    ushort      fTargetRing;            /* SRB target virtual ring            */
} RSRBEnableT;

typedef struct VDLCEnableT_             
{
    uchar       fVersion;
    hwaddrtype  fVMacAddr;              /* Virtual MAC address */
    ulong       fVdlcOptionFlags;       
    ushort      fMaxIFieldLength;     
    ushort      fRingGroup;             
} VDLCEnableT;

typedef struct NCIAEnableT_
{
    word       fVersion;                /* Always set to CLSI_VERSION */
    ipaddrtype fIpAddr;                 /* NCIA ip address            */
    uchar      fServerMac[IEEEBYTES];   /* Virtual Address            */
    uchar      fVMacAddr[IEEEBYTES];    /* Virtual Address            */
    ushort     fVMacRange;              /* Mac address range          */
    byte       fServerNum;              /* NCIA Server number         */
} NCIAEnableT;


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
 *        *fOqueueFn - Legacy Oqueue fn of form:
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
typedef struct TBRIDGEEnableT_
{
    uchar	fVersion;
    hwaddrtype	fVMacAddr;       	/* Virtual MAC address */
    ulong       fTbridgeOptionFlags;       /* tbd */
    ushort	fMaxIFieldLength;       /* size of payload, not including any headers */
    void        *fOqueueFn;
    uchar       fSpanIndex;
} TBRIDGEEnableT;

typedef struct ENEnableT_		/* EtherNet Media Parameters */
{
    uchar	fVersion;
    hwaddrtype	fLMacAddr;		/* Local mac addr returned to cls */
    ushort	fMaxIFieldLength;       /* size of payload, not including any headers */
} ENEnableT;

typedef struct FDDIEnableT_		/* FDDI Media Parameters */
{
    uchar	fVersion;
    hwaddrtype	fLMacAddr;		/* Local mac addr returned to cls */
    ushort	fMaxIFieldLength;       /* size of payload, not including any headers */
} FDDIEnableT;

typedef struct FREnableT_		/* Frame Relay Media Parameters? */
{
    uchar	fVersion;
    hwaddrtype  fDLCIAddr;
    ushort	fMaxIFieldLength;       /* size of payload, not including any headers */
} FREnableT;

typedef struct SDLCEnableT_		/* SDLC Media Parameters */
{
    uchar	fVersion;
} SDLCEnableT;

typedef struct QLLCEnableT_		/* QLLC Media Parameters */
{
    uchar	fVersion;
} QLLCEnableT;

typedef struct
{
    ushort      fDLCType;	/* Token Ring, Ethernet etc 	*/
    CLSPortID	fPortId;	/* Unit number 			*/
    DlcPPortIdT	fPPortId;	/* Provider port id (usually idbtype *) */
    union
    {
	TREnableT        fTREnable;
	RSRBEnableT      fRSRBEnable;
	TBRIDGEEnableT   fTBRIDGEEnable;
	ENEnableT        fENEnable;
	FDDIEnableT      fFDDIEnable;
	FREnableT        fFREnable;	/* Should FR and SDLC combine
					 * as SerialEnableT? 
					 * Hey, QLLC too then
					 * PHIL
					 */
	SDLCEnableT      fSDLCEnable;
	QLLCEnableT      fQLLCEnable;
	VDLCEnableT      fVDLCEnable;
        NCIAEnableT      fNCIAEnable;
    } fMediaEnable;
    
/*    union
 *    {
 *	LLCProtoEnableT		fLLCProtoEnable;
 *	SDLCProtoEnableT	fSDLCProtoEnable;
 *	QLLCProtoEnableT	fQLLCProtoEnable;
 *	FRProtoEnableT		fFRProtoEnable;
 *    } fProtoEnable;
 */
} DlcEnableT;

typedef struct
{

    ushort      fDLCType;        /* Token Ring, Ethernet, etc.          */
    CLSPortID   fPortId;        /* Unit number                          */
    DlcPPortIdT fPPortId;       /* Provider port id (usually idbtype *) */

} DlcDisableT;


/* ACTIVATE SAP */

typedef struct LlcActSapT_
{
    uchar	fLSapValue;		/* local SAP address to */
					/* associate with this DLU */
    uchar	fUserClass;		/* Use this to determine */
					/* response to IEEE XID and */
					/* SABME commands */
					/* 0x01 LLC Type 1        	*/
					/* 0x02 LLC Types 1 and 2 	*/
					/* 0x03 LLC Types 1 and 3 	*/
					/* 0x04 LLC Types 1, 2 3  	*/
    ulong	fSapFlags;		/* Bits for response or report */
					/* to higher level. See flags */
					/* defined in clsi_msg.h */
} LlcActSapT;

typedef struct SdlcActSapT_
{
    uchar	fLSapValue;	
    uchar	fAddrType;
} SdlcActSapT;

typedef struct QLLCActSapT_
{
    QSapT	fQActSap;
    /* fields for Virtual Mac Address Ranges
     * Only valid if the flag is set in fQActSap.fQSapFlags
     */
    uchar	fVmacStart[IEEEBYTES];	/* bottom virtual Mac Address */
    uint	fNumVmacs;		/* number in the range */
} QLLCActSapT;

typedef struct VDLCActSapT_
{
    uchar       fLSapValue;            
    uchar       fUserClass;    		/* WHY ???? PHIL */        
    ulong       fSapFlags;              
} VdlcActSapT;

typedef LlcActSapT NciaActSapT;

typedef struct DlcActSapT_
{
    ushort              fDLCType;        /* Token Ring, Ethernet etc         */
    DlcPPortIdT		fPPortId;	/* provider port id (usually idbtype *) */
    ClsUSapIdT		fUSapId;	/* CLS's User SAP ID		*/
    DlcPSapIdT		fDlcPSapId;	/* DLC's Provider SAP ID 	*/
    union
	{
	    LlcActSapT	fLlcActSap;
	    SdlcActSapT	fSdlcActSap;
	    QLLCActSapT	fQLLCActSap;
	    VdlcActSapT	fVdlcActSap;
            NciaActSapT fNciaActSap;
	} fDlcSap;
} DlcActSapT;



/************************************************************************
 *									*
 *    PLEASE READ THE IMPORTANT NOTICE AT THE HEAD OF THIS FILE !!!	*
 *									*
 ************************************************************************
 */
 
/* ID and ID STATION INDICATION */

/* We use the numerical values to set Response Bit in XID */
typedef enum XidT_
{
    XID_COMMAND  = 0,
    XID_RESPONSE,
} XidT;

/* We use the numerical values to set Response Bit in Test */
typedef enum TestT_
{
    TEST_COMMAND  = 0,
    TEST_RESPONSE,
} TestT;

/* We use this field so that DlcUDataStnT alligns with XID and Test */

typedef enum UDataT_
{
    UDATA_COMMAND = 0,
    UDATA_RESPONSE,
} UDataT;

#define TR_MAX_RIF_LENGTH 18

/************************************************************************
 *									*
 *  MAKE SURE THAT THIS ALLIGNS WITH 					*
 *	TEST.STN, 							*
 *	CONNECT.STN and 						*
 *	UDATA STN							*
 *									*
 ************************************************************************
 */

typedef struct DlcXidIdStnT_
{
    /* Fields relevant only for IdStn.ind only (fClsUCepID == 0) */
    /* We need enough  info to build the TCLSCep object */

    uchar       fMacType;
    uchar       fLMacLength;
    uchar       *fLMacPtr;
    uchar       fRMacLength;
    uchar       *fRMacPtr;
    uchar       fRoutingLength;
    uchar       *fRoutingPtr;
    uchar       fRSap;			/* Source Sap of received XID */
    uchar       fLSap;			/* Destination Sap of received XID */
    ushort      fDlci;                  /* added for FRAS BAN implementation */ 
} DlcXidIdStnT;

typedef DlcXidIdStnT LlcXidIdStnT;
typedef DlcXidIdStnT SdlcXidIdStnT;

typedef struct DlcIdStnT_
{
    /********************************************
     * Start critical alignment section.
     *
     * This section of DlcIdStnT must reside at the start of
     * the structure and match DlcIdT exactly.
     * dlcsdlc.c, function sdlcSendXid(), relies on this relationship.
     *
     * Additionally, fDlcPSapId must be at the same offset in this
     * structure as it is in DlcConnectStnT, DlcTestStnT, and DlcUdataStnT.
     */
    ClsUSapIdT	fClsUSapId;	/* for cls_rput */
    DlcPCepIdT	fDlcPCepId;	/* because this is also used for Id.Ind	*/
    ClsUCepIdT	fClsUCepId;	/*  ditto				*/
    XidT	fXidT;		/* command/response  or ??? */
    uchar       fXIDLength;	/* length of XID data  */
    uchar       *fXIDPtr;	/* pointer to XID data in paktype */

    DlcPSapIdT	fDlcPSapId;	/* as passed back in from ActSap/Ring conf */
    /*
     * End critical allignment section.
     * Add new parameters below fDlcPSapId field.
     *******************************************/
    uchar       fLMac[IEEEBYTES];
    uchar       fRMac[IEEEBYTES];
    uchar       fLSap;
    uchar       fRSap;
    uchar       fRif[18];
    uchar       fRifLength;
    union
    {
	DlcXidIdStnT	fDlcXidStn;
    	LlcXidIdStnT	fLlcXidStn;
	SdlcXidIdStnT	fSdlcXidStn;
    } fDlcXid;
} DlcIdStnT;

typedef struct DlcIdT_
{
    /********************************************
     * Start critical allignment section.
     *
     * This section of DlcIdT must reside at the start of
     * This section of DLCIdT must match DlcIdStnT exactly.
     * dlcsdlc.c, function sdlcSendXid(), relies on this relationship.
     */
    ClsUCepIdT  fClsUCepId;
    ClsUSapIdT  fClsUSapId;
    DlcPCepIdT  fDlcPCepId;
    XidT        fXidT;          /* command/response  or ??? */

    uchar       fXIDLength;     /* length of XID data  */
    uchar       *fXIDPtr;       /* pointer to XID data in paktype */
    /*
     * End critical allignment section.
     * Add new parameters below fXIDPtr field.
     *******************************************/
} DlcIdT;

typedef struct LlcTestStnT_
{
    /* Fields relevant only for TestStn.ind only (fClsUCepID == 0) */
    /* We need enough  info to build the TCLSCep object */
    uchar       fMacType;
    uchar       fLMacLength;
    uchar       *fLMacPtr;
    uchar       fRMacLength;
    uchar       *fRMacPtr;
    uchar       fRoutingLength;
    uchar       *fRoutingPtr;

    uchar       fRSap;                  /* Source Sap of received XID */
    uchar       fLSap;                  /* Destination Sap of received XID */
    ushort      filler;			/* added for FRAS BAN implementation */ 

} LlcTestStnT;

typedef LlcTestStnT SdlcTestStnT;

/************************************************************************
 *									*
 *  MAKE SURE THAT THIS ALLIGNS WITH 					*
 *	XID.STN, 							*
 *	CONNECT.STN and 						*
 *	UDATA STN							*
 *									*
 ************************************************************************
 */

typedef struct DlcTestStnT_
{
    /********************************************
     * Start critical allignment section.
     *
     * fDlcPSapId must be at the same offset in this structure
     * as it is in DlcIdStnT, DlcConnectStnT, and DlcUdataStnT.
     */
    ClsUSapIdT	fClsUSapId;	/* for cls_rput */
    DlcPCepIdT	fDlcPCepId;	/* filler */
    ClsUCepIdT	fClsUCepId;	/*  ditto */

    TestT        fTestT;          /* command/response  or ??? */
    uchar       fTESTLength;     /* length of TEST data  */
    uchar       *fTESTPtr;       /* pointer to TEST data in paktype */

    DlcPSapIdT	fDlcPSapId;	/* as passed back in from ActSap/Ring conf */
    /*
     * End critical allignment section.
     * Add new parameters below fDlcPSapId field.
     *******************************************/

    uchar       fLMac[IEEEBYTES];
    uchar       fRMac[IEEEBYTES];
    uchar       fLSap;
    uchar       fRSap;
    uchar       fRif[18];
    uchar       fRifLength;

    union
    {
        LlcTestStnT             fLlcTestStn;
        SdlcTestStnT            fSdlcTestStn;
    }fDlcTest;

} DlcTestStnT;


typedef struct DlcTestT_
{
    ClsUCepIdT  fClsUCepId;
    ClsUSapIdT  fClsUSapId;
    DlcPCepIdT  fDlcPCepId;

    TestT       fTestT;          /* command/response  or ??? */
    uchar       fTESTLength;     /* length of XID data  */
    uchar       *fTESTPtr;       /* pointer to XID data in paktype */

} DlcTestT;

/************************************************************************
 *									*
 *  MAKE SURE THAT THIS ALLIGNS WITH 					*
 *	ID.STN, 							*
 *	TEST.STN and	 						*
 *	CONNECT.STN							*
 *									*
 ************************************************************************
 */

typedef struct LlcUdataStnT_
{
    /* Fields relevant only for TestStn.ind only (fClsUCepID == 0) */
    /* We need enough  info to build the TCLSCep object */
    uchar       fMacType;
    uchar       fLMacLength;
    uchar       *fLMacPtr;
    uchar       fRMacLength;
    uchar       *fRMacPtr;
    uchar       fRoutingLength;
    uchar       *fRoutingPtr;

    uchar       fRSap;                  /* Source Sap of received XID */
    uchar       fLSap;                  /* Destination Sap of received XID */
    ushort      filler;			/* added for FRAS BAN implementation */ 

} LlcUdataStnT;

typedef struct DlcUdataStnT_
{
    /********************************************
     * Start critical allignment section.
     *
     * fDlcPSapId must be at the same offset in this structure
     * as it is in DlcIdStnT, DlcTestStnT, and DlcConnectStnT.
     */
    ClsUSapIdT  fClsUSapId;
    DlcPCepIdT	fDlcPCepId;	/* filler */
    ClsUCepIdT	fClsUCepId;	/*  ditto */

    XidT	fillerXidT;	/* filler for allignment purposes */
    ushort      fUDataLength;     /* length of Udata  */
    uchar       *fUDataPtr;       /* pointer to Udata */

    DlcPSapIdT	fDlcPSapId;	/* as passed back in from ActSap/Ring conf */
    /*
     * End critical allignment section.
     * Add new parameters below fDlcPSapId field.
     *******************************************/

    uchar       fLMac[IEEEBYTES];
    uchar       fRMac[IEEEBYTES];
    uchar       fLSap;
    uchar       fRSap;
    uchar       fRif[18];
    uchar       fRifLength;


    union
    {
        LlcUdataStnT             fLlcUdataStn;
    }                           fDlcUdata;

} DlcUdataStnT;

/************************************************************************
 *									*
 *  MAKE SURE THAT THIS ALLIGNS WITH 					*
 *	ID.STN, 							*
 *	TEST.STN and	 						*
 *	UDATA.STN							*
 *									*
 ************************************************************************
 */
 
typedef struct LlcConnectStnT_
{
    /* Fields relevant only for TestStn.ind only (fClsUCepID == 0) */
    /* We need enough  info to build the TCLSCep object */
    uchar       fMacType;
    uchar       fLMacLength;
    uchar       *fLMacPtr;
    uchar       fRMacLength;
    uchar       *fRMacPtr;
    uchar       fRoutingLength;
    uchar       *fRoutingPtr;

    uchar       fRSap;                  /* Source Sap of received XID */
    uchar       fLSap;                  /* Destination Sap of received XID */
    ushort      filler;			/* added for FRAS BAN implementation */ 

} LlcConnectStnT;

typedef struct DlcConnectStnT_
{
    /********************************************
     * Start critical allignment section.
     *
     * fDlcPSapId must be at the same offset in this structure
     * as it is in DlcIdStnT, DlcTestStnT, and DlcUdataStnT.
     */
    ClsUSapIdT  fClsUSapId;
    DlcPCepIdT  fDlcPCepId;
    ClsUCepIdT  fClsUCepId;

    XidT	fillerXidT;	    /* filler for allignment purposes */
    uchar       fCONNECTLength;     /* length of XID data  */
    uchar       *fCONNECTPtr;       /* pointer to XID data in paktype */

    DlcPSapIdT	fDlcPSapId;	/* as passed back in from ActSap/Ring conf */
    uchar       fLMac[IEEEBYTES];
    uchar       fRMac[IEEEBYTES];
    uchar       fLSap;
    uchar       fRSap;
    uchar       fRif[18];
    uchar       fRifLength;
    union
    {
        LlcConnectStnT          fLlcConnectStn;
    }                           fDlcConnect;

} DlcConnectStnT;

typedef struct LlcActRngT_
{
    ulong       fSapFlags;              /* Bits for response or report */
                                        /* to higher level. See flags */
                                        /* defined in clsi_msg.h */
} LlcActRngT;

typedef LlcActRngT SdlcActRngT;
typedef LlcActRngT QLLCActRngT;
typedef LlcActRngT VdlcActRngT;
typedef LlcActRngT NciaActRngT;

typedef struct DlcActRngT_
{
    ushort              fDLCType;   /* Token Ring, Ethernet etc         */
    DlcPPortIdT         fPPortId;   /* provider port id (usually idbtype *) */
    ClsUSapIdT          fClsUSapId; /* CLS's User SAP ID            */
    DlcPSapIdT		fDlcPSapId; /* DLC's Provider SAP ID	*/
    ushort              fLocalRing;
    ushort              fBridgeNumber;
    ushort              fTargetRing;
    union
    {
        LlcActRngT      fLlcActRng;
	SdlcActRngT	fSdlcActRng;
	QLLCActRngT	fQLLCActRng;
	VdlcActRngT	fVdlcActRng;
        NciaActRngT     fNciaActRng;
    } fDlcRng;
} DlcActRngT;

typedef struct LlcDeActRngT_
{
    ulong       fSapFlags;              /* Bits for response or report */
                                        /* to higher level. See flags */
                                        /* defined in clsi_msg.h */
} LlcDeActRngT;

typedef struct VdlcDeActRngT_
{
    ulong       fSapFlags;              
} VdlcDeActRngT;

typedef LlcDeActRngT NciaDeActRngT;

typedef struct DlcDeActRngT_
{
    ushort              fDLCType;   /* Token Ring, Ethernet etc         */
    DlcPPortIdT         fPPortId;   /* provider port id (usually idbtype *) */
    ClsUSapIdT          fUSapId;        /* CLS's User SAP ID            */
    ushort              fLocalRing;
    ushort              fBridgeNumber;
    ushort              fTargetRing;
    union
    {
        LlcDeActRngT      fLlcDeActRng;
	SdlcActRngT	  fSdlcActRng;
	QLLCActRngT	  fQLLCActRng;
	VdlcDeActRngT	  fVdlcDeActRng;
        NciaDeActRngT     fNciaDeActRng;
    } fDlcRng;
} DlcDeActRngT;



/* REQUEST OPEN STATION */

typedef struct LlcReqOpenStnT_
{
    hwaddrtype	fRMacAddr;		/* Remote mac address 		*/
    hwaddrtype	fLMacAddr;		/* Local mac address 		*/
    uchar	fRSapValue;		/* Remote SAP address 		*/
    uchar	fLSapValue;		/* Local SAP address            */

    ulong	fTimerT1;		/* T1 timer for LLC2 session */
    ulong	fTimerT2;		/* T2 timer for LLC2 session */
    ulong	fTimerTi;		/* Ti timer for LLC2 session */
    ulong       fFlags;                 /* Genrl Flag to pass params*/
    uchar	fMaxout;		/* TW for LLC2 session */
    uchar	fMaxTries;		/* N2 counter for LLC2 session */
    uchar	fMaxin;

    ulong	fExplorerTimer;		/* Timeout for explorer	retries */
    uchar	fExplorerRetries; 	/* count for explorer 0 = */
					/* forever */

    ulong       fXIDTimer;		/* Timer for XID retries */
    uchar       fXIDRetries;		/* count for XID retries 0 = forever */

    uchar       fExplorerOption;	/* See comment in clsi_msg.h for options */
    uchar       fRifLength;		/* RIF field length in bytes  */

/* TRSRB_MAXRIF should be used here when includes are straightened out. */
    uchar       fRif[TR_MAX_RIF_LENGTH];	/* bytes 0 - */

    uchar       fL3Pid;			/* Value to use for RFC1490 */
					/* Level 3 Protocol ID */
    ushort      fDlci;                  /* DLCI number for BAN (at present) */
    
} LlcReqOpenStnT;

typedef struct VdlcReqOpenStnT_
{
    hwaddrtype	fRMacAddr;		/* Remote mac address 		*/
    hwaddrtype	fLMacAddr;		/* Local mac address 		*/
    uchar	fRSapValue;		/* Remote SAP address 		*/
    uchar	fLSapValue;		/* Local SAP address            */
    uchar	fTimerT1;		/* ReqOpenStnReq Timeout	*/
    uchar	fRetries;		/* Max retries on explorer	*/
} VdlcReqOpenStnT;

typedef struct NciaReqOpenStnT_
{
    hwaddrtype  fDestAddr;              /* Remote mac address */
    hwaddrtype  fSrcAddr;               /* Local mac address  */

    byte        fDestSap;               /* Remote SAP address */
    byte        fSrcSap;                /* Local SAP address. */

    ulong       fQReqOpenStnFlags;      /* always need these */

} NciaReqOpenStnT;

typedef struct SdlcReqOpenStnT_
{
    hwaddrtype	fRMacAddr;		/* Remote mac address 		*/
    hwaddrtype	fLMacAddr;		/* Local mac address 		*/
    uchar	fRSapValue;		/* Remote SAP address 		*/
    uchar	fLSapValue;		/* Local SAP address            */

    ulong	fExplorerTimer;		/* Timeout for explorer	retries */
    uchar	fExplorerRetries; 	/* count for explorer 0 = */
					/* forever */

    ulong       fXIDTimer;		/* Timer for XID retries */
    uchar       fXIDRetries;		/* count for XID retries 0 = forever */

    uchar       fExplorerOption;	/* See comment in clsi_msg.h for options */
} SdlcReqOpenStnT;

/*
 * Get the innards here from the clsi_msg.h structure 
 */
 
 
typedef struct QLLCReqOpenStnT_
{
	ReqOpnStnQLLCParms_t	fParms;
} QLLCReqOpenStnT;

typedef struct DlcReqOpenStnT_
{
    /* GO AWAY, PLEASE GO AWAY */
    /* IF WE WANT THIS TO GO AWAY, WE'LL HAVE TO HAVE A WAY TO
     * GET TO THE IDB FROM THE PSAP  --- PHIL
     */

    DlcPPortIdT	fPPortId;	/* provider port id (usually idbtype *) */

    DlcPSapIdT	fDlcPSapId;	/* as passed in by ActSap/Ring confirm DLC->CLS */
    ClsUCepIdT	fClsUCepId;	/* u_cep_id from DLC's view */
    DlcPCepIdT	fDlcPCepId;	/* p_cep_id from CLS's view */
    int         fQueuingType;	/* Should llc try to prioritize */
    ulong       fQueuingParameter; /* What priority should it have */
    union
	{
	    LlcReqOpenStnT	fLlcReqOpenStn;
	    SdlcReqOpenStnT	fSdlcReqOpenStn;
	    QLLCReqOpenStnT	fQLLCReqOpenStn;
	    VdlcReqOpenStnT	fVdlcReqOpenStn;
            NciaReqOpenStnT     fNciaReqOpenStn;
	} fDlcStn;
} DlcReqOpenStnT;

typedef struct DlcConnectT_
{
    DlcPPortIdT	fPPortId;	/* provider port id (usually idbtype *) */
    ClsUCepIdT	fClsUCepId;	/* u_cep_id from DLC's view */
    DlcPCepIdT	fDlcPCepId;	/* p_cep_id from CLS's view */
    uint        fLlcRetCode;	/* component specifc return value,
				   see llc2/llc2.h */
    ushort	fModifyFlags;
    uchar       fFlag;
    int         fQueuingType;	/* Should llc try to prioritize */
    ulong       fQueuingParameter; /* What priority should it have */
    union
    {
	LlcReqOpenStnT	fLlcReqOpenStn;
	SdlcReqOpenStnT	fSdlcReqOpenStn;
	QLLCReqOpenStnT	fQllcReqOpenStn;
	VdlcReqOpenStnT	fVdlcReqOpenStn;
        NciaReqOpenStnT fNciaReqOpenStn;
    } fDlcMod;
} DlcConnectT;

typedef struct DlcDisconnectT_
{
    DlcPPortIdT	fPPortId;	/* provider port id (usually idbtype *) */
    ClsUCepIdT	fClsUCepId;	/* u_cep_id from DLC's view */
    DlcPCepIdT	fDlcPCepId;	/* p_cep_id from CLS's view */
    uint        fLlcRetCode;	/* component specifc retun value,
				   see llc2/llc2.h
				 */
} DlcDisconnectT;

typedef struct DlcCloseStnT_
{
    ClsUCepIdT  fClsUCepId;		/* If CLS doesn't know the */
					/* DLCPCepId yet, it can close */
					/* the cep with the ucepId. */
					/* This happens when we are */
					/* using CloseStn to cancel a */
					/* pending ReqOpnStnReq. */

    DlcPSapIdT	fDlcPSapId;		/* A Dlc may need the P Sap
					 * in order to find the correct CEP
					 */

    uchar       fActionCode;		/* ??? */
} DlcCloseStnT;

/* --------------------------------------------------------
 *
 *                 THE PARAMETER BLOCK
 *
 * --------------------------------------------------------
 */

typedef struct
{
    ClsErrorCodeT	fResultError;	/* from enum ClsErrorCodeT */
    paktype		*fPakPtr;	/* Data req, ind, conf		*/
    CLSPortID           fPortId;
    ushort              fLoadLength;
    uchar               *fRawData;
    ushort              fRawPortType;
    /* 
     * For CLS_ID_IND:
     * packet header is partially parsed:
     * pak->datagramstart-> tr_header AC, FC
     * pak->addr_start -> dmacadr
     * pak->infostart-> dsap
     */

/* list of opcode-specific fields */
    union
    {
	DlcInitT	fDlcInit;
	DlcEnableT	fDlcEnable;
	DlcDisableT	fDlcDisable;
	DlcActSapT	fDlcActSap;	/* Activate SAP, Modify SAP 	*/
	DlcReqOpenStnT 	fDlcReqOpenStn;	/* Req Open Stn, Modify Stn 	*/
	DlcIdStnT	fDlcIdStn;	/* XID req,conf,ind,rsp, ID_STN ind */

        DlcActRngT      fDlcActRng;
        DlcDeActRngT    fDlcDeActRng;
        DlcTestStnT     fDlcTestStn;
        DlcConnectStnT  fDlcConnectStn;
        DlcUdataStnT    fDlcUdataStn;
        DlcTestT        fDlcTest;
        DlcIdT          fDlcId;         

	uchar           fSignalStnActionCode; /* SignalStn */
	boolean		fResumeFlow;	/* Flow Req */
	ulong		fFlowLevel;	/* Flow Ind - congestion level */
	DlcConnectT     fDlcConnect;    /* Connect or ModifyStn */
	DlcDisconnectT  fDlcDisconnect;	/* Disconnect Indicate */
	DlcCloseStnT    fDlcCloseStn;	/* CloseStn */
	boolean         fOutboundQEmpty; /* Pong request */
	short           fNextWindowSize; /* Pong confirm */
    } fParameters;	
}  ClsDlcParamBlockT;



/* --------------------------------------------------------
 *
 *                    DLC Private Parts
 *
 * --------------------------------------------------------
 */

/*
 * DlcToIdb
 *
 * Given port type and port number, find the hwidb
 * Returns NULL if it fails
 */
static inline idbtype *DlcToIdb (CLSPortID* portID)
{
    if (portID->type == CLS_P_TYPE_SWIDB)
        return(portID->fTypeInfo.swidb);
    return(NULL);
}

/*
 * handle dlc_wput operations
 */

typedef void clsLlcMapFnT;
typedef void clsSdlcMapFnT;
typedef void clsQllcMapFnT;
typedef void clsNciaMapFnT;

typedef struct ClsSdlcMapT_
{
    clsSdlcMapFnT (*fn)(sdlc_data_block *sdb, ClsDlcParamBlockT *clsDlcParamBlock);
} ClsSdlcMapT;

typedef struct ClsQLLCMapT_
{
    clsQllcMapFnT (*fn)(qllctype *qllc, ClsDlcParamBlockT *clsDlcParamBlock);
} ClsQLLCMapT;

typedef struct ClsDlcMapT_
{
    clsLlcMapFnT (*fn)(llctype *llc, ClsDlcParamBlockT *clsDlcParamBlock);
} ClsDlcMapT;

void dlc_wput(
	int                             clsDLCPortType,
	DlcPCepIdT			pCepId,
	DlcOpcodeT 			opcode, 
	ClsDlcParamBlockT 		*clsDlcPB);

void cls_rput(
	ClsUCepIdT			u_cep_id,
	ClsOpcodeT 			opcode, 
	ClsDlcParamBlockT 		*clsDlcPB);



/* --------------------------------------------------------
 *
 *        	         PROTOTYPES 
 *
 * --------------------------------------------------------
 */

void dlc_rput(ClsDlcParamBlockT *clsBlock, 
	       ClsOpcodeT opCode, 
	       ClsErrorCodeT error);

extern void llc_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
			ClsDlcParamBlockT *clsParamBlock);

paktype *Llc1GenUFrame(uchar *daddr, uchar *saddr, 
		       uchar dsap, uchar ssap, uchar control,
		       hwidbtype *interface);

void  DlcBounceTestStnInd(DlcPCepIdT pCepId, ClsDlcParamBlockT *clsBlock);

void DlcSendUControlStnInd(ClsDlcParamBlockT *pb, paktype *pak, 
			uchar control, boolean isResponse,
			ClsUSapIdT uSap, DlcPCepIdT pCep);

void DlcHandleTestReq(ClsUCepIdT UCepId, ClsDlcParamBlockT *clsBlock);
void DlcReqOpenStnConf (DlcPCepIdT pCepId, ClsUCepIdT uCepId,
			ClsErrorCodeT error);
void DlcBuildParmBlock (
		   DlcPCepIdT 		pCepId,
		   ClsUCepIdT 		uCepId,
		   ClsOpcodeT 		opcode,
		   ClsDlcParamBlockT 	*paramBlock,
		   ClsErrorCodeT 	errorCode,
		   paktype		*pak);

void CepFlowControlInd(void *uCepId, CepT *cep);
void CepHandlePong(void *uCepId, CepT *cep);
void CepPongRequest(void *uCepId, CepT *cep);
void CepFlowNotify(void *uCepId, CepT *cep);

void PrintDlcOpCode(DlcOpcodeT opcode);

/****************************************************************
 *								*
 *			SDLC PRIVATE PARTS			*
 *								*
 * These prototypes should really be in sdlc.h,  BUT		*
 * they include ClsErrorCodeT, and that breaks anything that	*
 * includes sdlc.h, but doesn't include dlc.h. Gnarrrh		*
 *								*
 ****************************************************************
 */

extern void sdlcClsNotify(sdlc_data_block *, ClsOpcodeT, ClsErrorCodeT);

/****************************************************************
 *								*
 *			LLC2 PRIVATE PARTS			*
 *								*
 * This prototype should really be in llc2.h,  BUT		*
 * it includes ClsErrorCodeT, and that breaks anything that	*
 * includes llc2.h, but doesn't include dlc.h. Gnarrrh		*
 *								*
 ****************************************************************
 */

extern llctype *LLcByPak (idbtype *idb, paktype *pak);

ClsDlcParamBlockT *llcBuildParmBlock(
	llctype 			*llc, 
	ClsOpcodeT 			opcode, 
	ClsDlcParamBlockT 		*clsDlcPB,
	ClsErrorCodeT                	ClsErrorCode);

#endif					/* __DLC_H_ */

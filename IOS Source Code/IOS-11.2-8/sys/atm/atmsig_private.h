/* $Id: atmsig_private.h,v 3.4.10.6 1996/09/06 16:40:08 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_private.h,v $
 *------------------------------------------------------------------
 *
 * A T M S I G _ P R I V A T E . H
 *
 * ATM Signalling private header file
 *
 * November 1993,  Percy P. Khabardar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_private.h,v $
 * Revision 3.4.10.6  1996/09/06  16:40:08  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.4.10.5  1996/09/04  05:11:07  schrupp
 * CSCdi67982:  ATMSIG: Dont suspend prematurely
 * Branch: California_branch
 *
 * Revision 3.4.10.4  1996/06/19  00:56:49  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/05/09  14:11:14  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.10.2.4.1  1996/04/27  06:33:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.10.2  1996/04/23  04:47:44  grobelch
 * - When we initiate an SVC with no traffic parameters specified,
 *     use a default value of all-ones. Still recognize 1 as a
 *     default.
 *   - Use the correct id-value to specify the FWD/BWD max-burst
 *     for CLP0&1 in the UCR IE. Use B0/B1, the old/bad values
 *     were C0/C1.
 *   - Correct error in initiating an SVC using the forward SCR
 *     value for CLP0 in UCR
 *
 *   ** Also fix for CSCdi55344:
 *   - Signalling uses whole-cell rates in installing VCs/specifying
 *      map-class.
 * CSCdi55196:  ATMSIG: UserCellRate Information Element changes needed
 * Branch: California_branch
 *
 * Revision 3.4.10.1.4.1  1996/04/10  23:35:37  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.10.1  1996/03/29  19:28:10  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/23  15:59:45  schrupp
 * CSCdi49685:  ATMSIG Use vcd_t for vcnum
 *
 * Revision 3.3  1995/11/22  20:44:34  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:46:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/01  17:48:33  percyk
 * CSCdi39722:  set sdu sizes per connection - part 2
 *
 * Revision 2.5  1995/08/18  14:23:13  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.4  1995/07/26  15:13:14  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.3  1995/07/19  21:39:36  schrupp
 * CSCdi37350:  Display protocol for ATM MUX call in show atm vc X
 *
 * Revision 2.2  1995/06/28  09:22:17  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:12:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATMSIG_PRIV_H__
#define __ATMSIG_PRIV_H__ 

#ifndef V31
#define V31 1
#endif


#define ATMSIG_SVCBLK_CNT        200
#define ATMSVC_HASH_SIZE         256 
#define ATMSIG_HASH_MASK         (ATMSVC_HASH_SIZE - 1) 
#define ATMSIG_ENDPOINT_SIZE     256

/* value in API/MAP-class/TOS that indicates that the quantity */
/* is not-defined. */
#define ATMSIG_TRAFFICPARM_NOT_DEFINED_API (-1)
#define ATMSIG_PCR01_NOT_DEFINED_IN_SETUP_1      1
#define ATMSIG_PCR01_NOT_DEFINED_IN_SETUP_ALL_1  0xffffff

/*
 * ATM Signalling States
 */
typedef enum _atmSig_statetype_ {
   ATMSIG_ST_NULL,                    /* Null/Idle State (U0) */
   ATMSIG_ST_CALL_INITIATED,          /* Call Initiated State (U1) */
   ATMSIG_ST_OUTCALL_PROCEEDING,      /* Outgoing Call Proceeding State (U3) */
   ATMSIG_ST_CALL_PRESENT,            /* Call Present State (U6) */ 
   ATMSIG_ST_CONNECT_REQ,             /* Connect Request State (U8) */
   ATMSIG_ST_INCALL_PROCEEDING,       /* Incoming Call Proceeding State (U9) */ 
   ATMSIG_ST_ACTIVE,                  /* Active State (U10) */ 
   ATMSIG_ST_RELEASE_REQ,             /* Release Request State (U11) */
   ATMSIG_ST_RELEASE_IND,             /* Release Indication State (U12) */ 
   ATMSIG_ST_RESTART_REQ,             /* Restart Request State (Rest 1) */
   /* Multipoint states */
   ATMSIG_PST_NULL,                    /* Null/Idle State */
   ATMSIG_PST_ADD_PARTY_INITIATED,     /* Call Initiated State */
   ATMSIG_PST_ADD_PARTY_RECEIVED,      /* Outgoing Call Proceeding State */ 
   ATMSIG_PST_DROP_PARTY_INITIATED,    /* Call Initiated State */
   ATMSIG_PST_DROP_PARTY_RECEIVED,     /* Outgoing Call Proceeding State */ 
   ATMSIG_PST_ACTIVE                   /* Active State */ 
} atmSig_statetype;

#define ATMISG_NUM_OF_STATES   ATMSIG_PST_ACTIVE_REQ + 1          

/*
 * Error codes returned for opening svc connections
 */
typedef enum _atmSig_opencodetype_ {
   ATMSIG_OPENSVC_OK, 
   ATMSIG_OPENSVC_INVALID_ADDR,
   ATMSIG_OPENSVC_IDB_ERROR, 
   ATMSIG_OPENSVC_OPENING,
   ATMSIG_OPENSVC_NOMEMORY, 
   ATMSIG_OPENSVC_NOSVCFOUND,
   ATMSIG_OPENSVC_SVCFOUND
} atmSig_opencodetype; 

/*
 * Error codes returned for closing svc connections
 */
typedef enum _atmSig_closecodetype_ {
   ATMSIG_CLOSESVC_OK,
   ATMSIG_CLOSESVC_INVALIDSVC,
   ATMSIG_CLOSESVC_CLOSED,
   ATMSIG_CLOSESVC_CLOSEFAILED,
   ATMSIG_CLOSESVC_NOTOPEN,
   ATMSIG_CLOSESVC_IDB_ERROR
} atmSig_closecodetype;

/*
 * Error codes returned for finding status of svc connections.
 */
typedef enum _atmSig_statuscodetype_ {
   ATMSIG_STATUSSVC_OK,
   ATMSIG_STATUSSVC_INVALIDSVC,
   ATMSIG_STATUSSVC_STATUSFAILED
} atmSig_statuscodetype;



/*
 * ATM Message Types - currently supported by ATM Forum
 */

/* Call Establishment Messages */
#define ATMSIG_MT_CALL_PROCEEDING      0x02
#define ATMSIG_MT_CONNECT              0x07
#define ATMSIG_MT_CONNECT_ACK          0x0f
#define ATMSIG_MT_SETUP                0x05

/* Call Clearing Messages */
#define ATMSIG_MT_RELEASE              0x4d
#define ATMSIG_MT_RELEASE_COMPLETE     0x5a
#define ATMSIG_MT_RESTART              0x46
#define ATMSIG_MT_RESTART_ACK          0x4e

/* Miscellaneous Messages */
#define ATMSIG_MT_STATUS               0x7d
#define ATMSIG_MT_STATUS_ENQUIRY       0x75

/* Point-to-Multipoint Messages */
#define ATMSIG_MT_ADD_PARTY            0x80
#define ATMSIG_MT_ADD_PARTY_ACK        0x81
#define ATMSIG_MT_ADD_PARTY_REJ        0x82
#define ATMSIG_MT_DROP_PARTY           0x83
#define ATMSIG_MT_DROP_PARTY_ACK       0x84


/*
 * ATM Message Types - currently NOT supported by ATM Forum.
 * (future support is promised)
 */

/* Call Establishment Messages */
#define ATMSIG_MT_ALERTING            0x01
#define ATMSIG_MT_PROGRESS            0x03
#define ATMSIG_MT_SETUP_ACK           0x0d

/* Call Information Phase Messages */
#define ATMSIG_MT_RESUME              0x26
#define ATMSIG_MT_RESUME_ACK          0x2e
#define ATMSIG_MT_RESUME_REJ          0x22
#define ATMSIG_MT_SUSPEND             0x25
#define ATMSIG_MT_SUSPEND_ACK         0x2d
#define ATMSIG_MT_SUSPEND_REJ         0x21

/* Miscellaneous Messages */
#define ATMSIG_MT_INFO                0x7b
#define ATMSIG_MT_NOTIFY              0x6e


/*
 * Incoming Events from SSCOP
 */
typedef enum _atmSig_eventtype_ {
   /* Call Establishment Messages */
   ATMSIG_RCV_CALL_PROCEEDING,
   ATMSIG_RCV_CONNECT,
   ATMSIG_RCV_CONNECT_ACK, 
   ATMSIG_RCV_SETUP, 

   /* Call Clearing Messages */
   ATMSIG_RCV_RELEASE,
   ATMSIG_RCV_RELEASE_COMPLETE, 
   ATMSIG_RCV_RESTART, 
   ATMSIG_RCV_RESTART_ACK, 

   /* Miscellaneous Messages */
   ATMSIG_RCV_STATUS, 
   ATMSIG_RCV_STATUS_ENQUIRY, 

   /* Point-to-Multipoint Messages */
   ATMSIG_RCV_ADD_PARTY, 
   ATMSIG_RCV_ADD_PARTY_ACK,
   ATMSIG_RCV_ADD_PARTY_REJ,
   ATMSIG_RCV_DROP_PARTY,
   ATMSIG_RCV_DROP_PARTY_ACK,

   /* Outgoing Call Establishment Events */
   ATMSIG_REQ_CONNECT,
   ATMSIG_REQ_CONNECT_ACK,
   ATMSIG_REQ_SETUP,
   ATMSIG_REQ_RELEASE,
   ATMSIG_REQ_RELEASE_COMPLETE,
   ATMSIG_REQ_ADD_PARTY,
   ATMSIG_REQ_ADD_PARTY_ACK,
   ATMSIG_REQ_ADD_PARTY_REJ,
   ATMSIG_REQ_DROP_PARTY,
   ATMSIG_REQ_DROP_PARTY_ACK,

   /* Invalid incoming events */
   ATMSIG_RCV_INVALIDMSG,
   ATMSIG_RCV_TOOSMALL,
   ATMSIG_RCV_INVPROTODISCR,
   ATMSIG_RCV_INVCII, 
   ATMSIG_RCV_INVMSGSIZE, 
   ATMSIG_RCV_BADMT
} atmSig_eventtype; 


/*
 * Protocol Discriminator - Q.93B User-Network Call Control Messages,
 * (first byte of every Q.93B message).
 */
#define Q93B_PROTO_DISCR        0x09

/* Compatibility Instruction Indicator - second byte of every message */
#define ATMSIG_CII              0x80

/* 
 * max number of svc's currently supported - this number is picked randomly 
 * to provide defensive mechanisms in some routines to prevent infinite looping.
 */
#define ATMSIG_MAX_SVC                  0xFFFF 

/* Max mtu size supported */
#define ATMSIG_MAX_MTU_SIZE       9188

/*
 * User/Network side of the link
 */
#define ATMSIG_SIDE_USER       0
#define ATMSIG_SIDE_NETWORK    1


/*
 * ATM Forum UNI Signalling version
 */
#define ATMSIG_VERSION_30   2 
#define ATMSIG_VERSION_31   3 

/*
 * Miscellaneous constants
 */
#define ATMSIG_CALLREF_LNGTH             0x03000000 
#define ATMSIG_CALLREF_FLAG              0x00800000
#define ATMSIG_CALLREF_VALUE             0x007FFFFF
#define ATMSIG_CALLREF_LNGTH_FLD         4 
#define ATMSIG_MSG_LNGTH                 2
#define ATM_SDU_LNGTH_FLD                3
#define ATM_MID_LNGTH_FLD                2
#define ATM_TYPEX_LNGTH_FLD              4

#define ATMSIG_EXTENSION_MASK            0x7f
#define ATMSIG_EXTENSION_BIT             0x80


/*
 * Error codes - returned after processing of Messages and IE's 
 */
#define ATMSIG_MSG_OK                   0x0000
#define ATMSIG_MSG_INVALID              0x0001
#define ATMSIG_IE_OK                    0x0002
#define ATMSIG_MANDATORY_IE_MISSING     0x0004
#define ATMSIG_IE_NONFATAL_ERROR        0x0008
#define ATMSIG_IE_FATAL_ERROR           0x0010
#define ATMSIG_IE_NOTREQUIRED           0x0020
#define ATMSIG_IE_VPCI_REUSED           0x0040

/* Call Reference constants */
#define ATM_CALLREF_FLAGBIT           0x80
#define ATM_CALLREF_FLAGMASK          0x00800000
#define ATM_CALLREF_VALUEMASK         0x007fffff
#define ATM_CALLREF_GLOBAL            0x00000000
#define ATM_CALLREF_DUMMY             0x007fffff  /* for UNI 3.1 */

/* Msg Compatibility Instruction Indicator constants */
#define ATM_CII_CCITT                 0x00
#define ATM_CII_RSRVD                 0x20
#define ATM_CII_NATSTD                0x40
#define ATM_CII_STNDRD                0x60
#define ATM_CII_NOTSIG                0x00
#define ATM_CII_EXPLICIT              0x10
#define ATM_CII_CLEARCALL             0x00
#define ATM_CII_DISCRDIG              0x01
#define ATM_CII_DISCRDREP             0x02

#define ATMSIG_GET_MSGLNGTH(x)    (x)



/*
 * ATMSIG TIMERS
 */
enum ATMSIG_TIMERS {
  ATMSIG_SHARED_TIMER,
  ATMSIG_SYNC_TIMER
};



/*
 * A T M   U s e r   C e l l   R a t e   S t r u c t u r e
 *
 */
typedef struct userCellRateType_ {
    long fwdPeakClp0;          /* Fwd Peak Cell Rate (CLP = 0) */
    long bwdPeakClp0;          /* Backwd Peak Cell Rate (CLP = 0) */
    long fwdPeakClp01;         /* Fwd Peak Cell Rate (CLP = 0 + 1) */
    long bwdPeakClp01;         /* Backwd Peak Cell Rate (CLP = 0 + 1) */
    long fwdSusClp0;           /* Fwd Sustainable Cell Rate (CLP = 0) */
    long bwdSusClp0;           /* Backwd Sustainable Cell Rate (CLP = 0) */
    long fwdSusClp01;          /* Fwd Sustainable Cell Rate (CLP = 0 + 1) */
    long bwdSusClp01;          /* Backwd Sustainable Cell Rate (CLP = 0 + 1) */
    long fwdMaxBurstClp0;      /* Fwd Max Burst Size (CLP = 0) */
    long bwdMaxBurstClp0;      /* Backwd Max Burst Size (CLP = 0) */
    long fwdMaxBurstClp01;     /* Fwd Max Burst Size (CLP = 0 + 1) */
    long bwdMaxBurstClp01;     /* Backwd Max Burst Size (CLP = 0 + 1) */
    long bestEffortIndicator;  /* Best Effort Indicator */
    int FwdTag;                 /* Traffic Mgmt Options Indicator - Fwd Tag */
    int BwdTag;                 /* Traffic Mgmt Options Indicator - Backwd Tag */
} userCellRateType;


/* 
 * A T M   T y p e   o f   S e r v i c e   S t r u c t u r e.
 *
 * All the information read from IE's or from config's is stored here - 
 * defaults are used when info is missing.  One such block exists for
 * every SVC. 
 */
typedef struct atmSvcTosType_ {
    uchar sscsType;
    ushort fwdSduSize;
    ushort bwdSduSize;
    union {
      ushort midSize;
      ushort lowestMid;    /* UNI 3.1 */
    } mid_size_u;
    ushort highestMid;     /* UNI 3.1 */
    uchar mode;
    uchar aalType;
    uchar bearerClass;
    uchar trafficType;
    uchar timingRequirements;
    uchar clipping;
    uchar usrPlaneAccessConfig;
    uchar rcvQosClassFwd;
    uchar rcvQosClassBackwd;
    uchar txQosClassFwd;
    uchar txQosClassBackwd;
    userCellRateType userCellRate;
    uchar broadBandHLType;      /* Broad Band Higher Layer Type */
    uchar highLayerInfo[8];
    uchar numberType;
    uchar numberingPlan;
    uchar presentationIndicator;
    uchar screeningIndicator; 
    uchar assocSig;
    /* AAL Type 1 and X - currently not supported */
    uchar subType;
    uchar constantBitRate;
    uchar multiplier;
    uchar clockRecoveryType;
    uchar errorCorrection;
    ushort structuredDataXfer;
    uchar partiallyFilledCells;
    uchar userInfo1;
    uchar userInfo2;
    uchar userInfo3;
    uchar userInfo4;
} atmSvcTosType;

#define svcTos_midSize     mid_size_u.midSize     /* uni 3.0 */
#define svcTos_lowestMid   mid_size_u.lowestMid   /* uni 3.1 */



/* 
 * Party structure
 */
typedef struct atmSvcPartyType_ {
    struct atmSvcType_ *svc;              /* Svc source struct  */
    struct atmSvcPartyType_ *next_party;  /* next party if multiPoint */
    struct atmSvcPartyType_ *next_timer;  /* next timer in timer Q */
    struct atmSvcPartyType_ *next_hash;   /* next hash in party hash table */
    hwaddrtype destNsap;                  /* destination nsap ATM address */
    int destNsapLength;                  
    int calledPartySubAddrLngth;
    uchar calledPartySubaddress[ATM_NSAP_ADDR_LNGTH];
    atmSig_statetype partyState;          /* party state */
    ushort retryCount;                    /* count on timer expiry */
    ushort n2;                            /* timer count */
    mgd_timer timeoutValue;               /* timeout value */
    short endpointRef;                    /* End Point Reference Id Value */
    uchar endpointState;                  /* End Point state */
    boolean freeParty;
    boolean conn_pending;             
} atmSvcPartyType;


/*
 * A T M   S i g n a l l i n g   M a i n   S V C   S t r u c t u r e.
 * 
 */
typedef struct atmSvcType_ {
    struct atmSvcType_ *next_hash;        /* next in hash list */
    idbtype *sig_idb;                     /* idb for all signalling messages */
    idbtype *idb;                         /* idb to send and receive all data */
    boolean locallyInitiatedCall;         /* TRUE if call locally initiated */
    ulong callRef;                        /* call reference value */
    hwaddrtype srcNsap;                   /* source nsap ATM address */
    int srcNsapLength;
    uchar subAddressType;
    int callingPartySubAddrLngth;
    uchar callingPartySubaddress[ATM_NSAP_ADDR_LNGTH];
    int netIdLength;
    uchar netId[ATM_NSAP_ADDR_LNGTH];
    uint vpci;                            /* virtual path channel identifier */
    uint vci;                             /* virtual channel identifier */
    vcd_t vcnum;
    user_vc_type_t user_vc_type;
    vc_info_t *vc;
    atmSig_statetype state;               /* svc state */
    paktype *inbuf;                       /* current input packet */
    uchar event;                          /* incoming event */
    atmSvcTosType tos;                    /* Type of Service details */ 
    uchar causeValue;
    int causeLocation;
    uchar restartClass;
    boolean freeSvc;
    boolean aal5mux;
    int layer2id;
    int protocol;
    int linktype;
    blli_type blli[ATM_MAX_BLLI_NUM];
    atmSvcPartyType *rootParty;           /* rootParty for call */
    atmSvcPartyType *svcPartyQ;           /* partyQ for multipoint call */
    boolean multiPointEvent;              /* multipoint msg */
    boolean multiPoint;                   /* multipoint call */
    ushort msgEventType;                  /* received msg type */
    short msgEndpointRef;                 /* received endpointRef */
    short lastEndpoint;                   /* last endpoint reference value */
    uchar cause_req;                      /* Signalling User's RELEASE req */
    void (*sig_user_callback)(void *);    /* ptr to sig user's callback func */
    staticmap_type *map_ptr;              /* ptr to map entry */
    uint flags;                           /* indicates snap or mux vc */
    boolean clientPresent;             
    boolean conn_pending;             
    boolean statusEnquiry; /* TRUE if need to out status enquiry message */
    mgd_timer           statusEnqTimer;   /* handle status enquiry only */
} atmSvcType;


/*
 * Signalling Message Header.  First part of every signalling message.
 * Cannot use ulong for 'callref' and ushort for 'length' otherwise pad byte will
 * be introduced causing packet misalignment - got to use uchar arrays to avoid 
 * this problem.  Use GETLONG, GETSHORT, PUTSHORT, PUTLONG to manipulate 'callref' 
 * and 'length' resp.
 */
typedef struct atmSig_msgHeader_ {
   uchar proto_discr;                        /* Protocol Discriminator */
   uchar callref[ATMSIG_CALLREF_LNGTH_FLD];  /* Call Reference */ 
   uchar msgtype;                            /* Message Type */
   uchar cii;                                /* Compatibility Instr Indicator */
   uchar length[ATMSIG_MSG_LNGTH];           /* Message Length */
} atmSig_msgHeader;

/* can't use sizeof(atmSig_ieHeader) as 68k gcc makes it even length - sigh!! */
#define ATMSIG_HDRSIZE          9 


/*
 * Default timer and retry limits
 */
/*#define ATMSIG_TIMER_DEFAULT  (30 * ONESEC)*/
#define ATMSIG_T303_DEFAULT   (4 * ONESEC)
#define ATMSIG_T308_DEFAULT   (30 * ONESEC)
#define ATMSIG_T309_DEFAULT   (10 * ONESEC)  /* default for uni 3.1 */
#define ATMSIG_T310_DEFAULT   (10 * ONESEC)
#define ATMSIG_T313_DEFAULT   (4 * ONESEC)
#define ATMSIG_T316_DEFAULT   (90 * ONESEC)
#define ATMSIG_T322_DEFAULT   (4 * ONESEC)
#define ATMSIG_T398_DEFAULT   (4 * ONESEC)
#define ATMSIG_T399_DEFAULT   (14 * ONESEC)
#define ATMSIG_RETRY_DEFAULT   10

#define ATMSIG_MAX_CONN_PENDING_DEFAULT   50


#define ATMSIG_STRICT_TRAFFIC_SHAPING_DEFAULT FALSE


/* Multipoint constants */
#define ATMSIG_MULTIPOINT                    0x01 /* BBBearerCap User Plane */ 
#define ATM_CAUSE_UNSUPPORTED_TRAFFIC_PARAMS 0x49
#define ATM_CAUSE_RESOURCE_UNAVAILABLE       0x2F

#define ATMSIG_CAUSE_RESOURCE_UNAVAILABLE 0x2F
#define ATMSIG_ENDPOINT_REF_ZERO 0x00
#define ATMSIG_MAX_ENDPOINT      0x7FFF       



/*
 *   E x t e r n s
 */

extern atmSvcType *atmSig_svcHashTable[];
extern atmSvcPartyType *atmSig_partyHashTable[];

extern watched_queue *atmSig_inputQ;
extern watched_queue *sscop_outputQ;

extern int atmSig_statsNoMemory;
extern int atmSig_statsIgnored;

extern boolean atmSig_debugPacket;
extern boolean atmSig_debugEvent;
extern boolean atmSig_debugIe;
extern boolean atmSig_debugError;
extern boolean atmSig_debugAll;

/* atmsig.c */

extern void atmSig_init(subsystype *);
extern void atmSig_fillTosInSvc(atmSvcType *, atm_params_type *);
extern process atmSig_inputProcess(void);
extern void atmSig_input(paktype *);
extern boolean atmSig_outputBLOCK(void);
extern forktype atmSig_outputProcess(void);
extern atmSvcType *atmSig_selectSvcBlock(paktype *);
extern int atmSig_hashCallRef(ulong);
extern boolean atmSig_allocateCallRef(atmSvcType *);
extern atmSvcType *atmSig_allocateSvcBlock(void);
extern void atmSig_freeSvcBlock(atmSvcType *);
extern boolean atmSig_reqVcFromDriver(atmSvcType *, vcd_t);
extern void atmSig_removeVcFromDriver(atmSvcType *);
extern atmSvcType *atmSig_findSvcBlockByCR(ulong, idbtype *, boolean);
extern atmSvcType *atmSig_findSvcBlockByAtmAddr(uchar *);
extern atmSvcType *atmSig_findSvcBlockByVC(idbtype *, vcd_t);
extern atmSvcType *atmSig_findSvcBlockByNsap(uchar *, int, boolean);
extern void atmSig_enterHashEntry(atmSvcType *);
extern boolean atmSig_removeHashEntry(atmSvcType *);
extern boolean atmSig_validateSvc(atmSvcType *);
extern void atmSig_globalReset(void);
extern void atmSig_interfaceReset(hwidbtype *);
extern void atmSig_subinterfaceReset(idbtype *);
extern boolean atmSig_nullNsap(uchar *);
extern boolean atmSig_nullPrefix(uchar *);
extern boolean atmSig_nullEsi(uchar *);
extern process atmSig_timerProcess(void);
extern void atmSig_startTimer (atmSvcPartyType *, ulong);
extern void atmSig_stopTimer (atmSvcPartyType *);
extern void atmSig_tickTock(void);
extern void atmSig_svcTimeout (atmSvcType *, atmSvcPartyType *);
extern long atmSig_cps_to_kbps(long);
extern long atmSig_kbps_to_cps(long);

/* multipoint */

extern atmSvcPartyType *atmSig_selectPartyBlock (paktype *, atmSvcType *);
extern boolean atmSig_enterPartyBlock (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_removePartyBlock (atmSvcType *, atmSvcPartyType *);
extern atmSvcPartyType *atmSig_allocatePartyBlock (void);
extern boolean atmSig_enterPartyHashEntry (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_removePartyHashEntry (atmSvcType *, atmSvcPartyType *);
extern atmSvcPartyType *atmSig_findPartyBlockByRef (atmSvcType *);
extern atmSvcPartyType *atmSig_findPartyBlockByEndpointRef (atmSvcType *, short);
extern boolean atmSig_allocateEndpointRef (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_validateParty (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_lastParty (atmSvcType *, atmSvcPartyType *);
extern atmSig_statuscodetype atmSig_findPartyStatus (atmSvcType *, atmSvcPartyType *);
extern void atmSig_freePartyBlock (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_flushPartyQ (atmSvcType *);
extern boolean atmSig_freeAllParties (atmSvcType *);
extern int atmSig_parseBroadbandLowLayerInfo (atmSvcType *);
extern boolean atmSig_fillBroadbandLowLayerInfo (staticmap_type *, sig_api_setup *); 


/* atmsig_api.c */

/* sig client messages to sig driver */
extern void atmSig_output(sig_api_msg *msg);
extern void atmSig_api_listen_nsap(sig_api_msg *);
extern void atmSig_api_cancel_nsap(sig_api_msg *);
extern atmSvcType *atmSig_api_setup_req(sig_api_msg *);
extern atmSvcType *atmSig_api_release_req(sig_api_msg *);
extern atmSvcType *atmSig_api_connect_req(sig_api_msg *);
extern atmSvcType *atmSig_api_release_complete_req(sig_api_msg *);
extern void atmsig_getSvcDetails (sig_api_msg *);

/* call back functions from Sig driver to it's clients */
extern void atmSig_notifySetupIndication(atmSvcType *);
extern void atmSig_notifyConnectEvent(atmSvcType *);
extern void atmSig_notifyConnectAckEvent(atmSvcType *);
extern void atmSig_notifyReleaseIndication(atmSvcType *);
extern void atmSig_notifyReleaseCompleteIndication(atmSvcType *);

/* multipoint api */

extern atmSvcType *atmSig_api_add_party_req(sig_api_msg *);
extern atmSvcType *atmSig_api_add_party_ack_req(sig_api_msg *);
extern atmSvcType *atmSig_api_add_party_rej_req(sig_api_msg *);
extern atmSvcType *atmSig_api_drop_party_req(sig_api_msg *);
extern atmSvcType *atmSig_api_drop_party_ack_req(sig_api_msg *);

extern void atmSig_notifyAddPartyEvent(atmSvcType *, atmSvcPartyType *);
extern void atmSig_notifyAddPartyAckEvent(atmSvcType *, atmSvcPartyType *);
extern void atmSig_notifyAddPartyRejEvent(atmSvcType *, atmSvcPartyType *);
extern void atmSig_notifyDropPartyEvent(atmSvcType *, atmSvcPartyType *);
extern void atmSig_notifyDropPartyAckEvent(atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_validateReqSvc (atmSvcType *, sig_api_msg *);

/* atmsig_state.c */

extern void atmSig_stateTable(atmSvcType *);
extern void atmSig_nullState(atmSvcType *);
extern void atmSig_callInitiatedState(atmSvcType *);
extern void atmSig_outCallProceedingState(atmSvcType *);
extern void atmSig_callPresentState(atmSvcType *);
extern void atmSig_connectReqState(atmSvcType *);
extern void atmSig_inCallProceedingState(atmSvcType *);
extern void atmSig_activeState(atmSvcType *);
extern void atmSig_releaseReqState(atmSvcType *);
extern void atmSig_releaseIndState(atmSvcType *);
extern void atmSig_restartReqState(atmSvcType *);

/* multipoint state */

extern void atmSig_multiPointStateTable (atmSvcType *, atmSvcPartyType *);
extern void atmSig_multiNullState (atmSvcType *, atmSvcPartyType *);
extern void atmSig_addPartyInitiatedState (atmSvcType *, atmSvcPartyType *);
extern void atmSig_addPartyReceivedState (atmSvcType *, atmSvcPartyType *);
extern void atmSig_multiPointActiveState (atmSvcType *, atmSvcPartyType *);
extern void atmSig_dropPartyInitiatedState (atmSvcType *, atmSvcPartyType *);
extern void atmSig_dropPartyReceivedState (atmSvcType *, atmSvcPartyType *);


/* atmsig_msg.c */

extern atmSig_eventtype atmSig_findMsgType(paktype *);

extern boolean atmSig_isMessageGlobal(atmSig_msgHeader *);
extern int atmSig_processSetupMsg(atmSvcType *);
extern int atmSig_processCallProceedingMsg(atmSvcType *);
extern int atmSig_processConnectMsg(atmSvcType *);
extern int atmSig_processConnectAckMsg(atmSvcType *);
extern int atmSig_processReleaseMsg(atmSvcType *);
extern int atmSig_processReleaseCompleteMsg(atmSvcType *);
extern int atmSig_processStatusMsg(atmSvcType *,int *, int *);
extern int atmSig_processStatusEnquiryMsg(atmSvcType *);
extern int atmSig_processRestartMsg(atmSvcType *);
extern int atmSig_processRestartAckMsg(atmSvcType *);
extern void atmSig_buildMsgHeader(atmSvcType *, uchar *, uint, uint);
extern boolean atmSig_txSetupMsg(atmSvcType *);
extern boolean atmSig_txCallProceedingMsg(atmSvcType *);
extern boolean atmSig_txConnectMsg(atmSvcType *);
extern boolean atmSig_txConnectAckMsg(atmSvcType *);
extern boolean atmSig_txReleaseMsg(atmSvcType *, uchar);
extern boolean atmSig_txReleaseCompleteMsg(atmSvcType *, uchar);
extern boolean atmSig_txStatusMsg(atmSvcType *, uchar);
extern boolean atmSig_txStatusEnquiryMsg(atmSvcType *);
extern boolean atmSig_txRestartMsg(atmSvcType *);
extern boolean atmSig_txRestartAckMsg(atmSvcType *);
extern int atmSig_sizeupUserCellRate(atmSvcType *);
extern int atmSig_sizeupLowerLayerIe(atmSvcType *svc);
extern void atmSig_sendToSscop(atmSvcType *, paktype *, long);

/* multipoint messages */

extern boolean atmSig_partyMsg (atmSig_eventtype);
extern short atmSig_findEndpointRef (atmSvcType *, paktype *);
extern int atmSig_processAddPartyMsg (atmSvcType *, atmSvcPartyType *);
extern int atmSig_processAddPartyAckMsg (atmSvcType *, atmSvcPartyType *);
extern int atmSig_processAddPartyRejMsg (atmSvcType *, atmSvcPartyType *);
extern int atmSig_processDropPartyMsg (atmSvcType *, atmSvcPartyType *);
extern int atmSig_processDropPartyAckMsg (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_txAddPartyMsg (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_txAddPartyAckMsg (atmSvcType *, atmSvcPartyType *);
extern boolean atmSig_txAddPartyRejMsg (atmSvcType *, atmSvcPartyType *, uchar);
extern boolean atmSig_txDropPartyMsg (atmSvcType *, atmSvcPartyType *, uchar);
extern boolean atmSig_txDropPartyAckMsg (atmSvcType *, atmSvcPartyType *, uchar);

/* atmsig_ie.c */

extern ushort atmSig_findIeOpCode(atmSig_ieHeader *);
extern int atmSig_processAalIe(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_findAALType(uchar);
extern int atmSig_processAALType1Ie(atmSvcType *, uchar *, int);
extern int atmSig_processAALType34Ie(atmSvcType *, uchar *, int);
extern int atmSig_processAALType5Ie(atmSvcType *, uchar *, int);
extern int atmSig_processAALTypeXIe(atmSvcType *, uchar *, int);
extern boolean atmSig_processUserCellRate(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processBbearerCapability(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processBroadbandHighLayerInfo(atmSvcType *, atmSig_ieHeader *);
extern boolean atmSig_processBroadbandLowLayerInfo(atmSvcType *, atmSig_ieHeader *);
extern boolean atmSig_processBbandRepeatIndicator(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processCalledPartyNumber(atmSvcType *, atmSig_ieHeader *);
extern boolean atmSig_processCalledPartySubAddr(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processCallingPartyNumber(atmSvcType *, atmSig_ieHeader *, atmSvcPartyType *);
extern boolean atmSig_processCallingPartySubAddr(atmSvcType *, atmSig_ieHeader *, atmSvcPartyType *);
extern int atmSig_processConnectionIdentifier(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processQosParameter(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processBroadbandSendCmplt(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processTransitNtwrkSel(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processCallState(atmSvcType *, atmSig_ieHeader *, int *);
extern int atmSig_processCause(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processRestartIndicator(atmSvcType *, atmSig_ieHeader *);
extern void atmSig_buildUserCellRate(atmSvcType *, uchar *, int);
extern void atmSig_buildBBearerCapability(atmSvcType *, uchar *);
extern void atmSig_buildCalledPartyNumber(atmSvcType *, uchar *,  atmSvcPartyType *);
extern void atmSig_buildCallingPartyNumber(atmSvcType *, uchar *);
extern void atmSig_buildQosParameter(atmSvcType *, uchar *);
extern void atmSig_buildAalType34(atmSvcType *, uchar *);
extern ushort atmSig_buildAalType5(atmSvcType *, uchar *);
extern void atmSig_buildBroadbandSendCmplt(atmSvcType *, uchar *);
extern void atmSig_buildCallState(atmSvcType *, uchar *);
extern void atmSig_buildRestartIndicator(atmSvcType *, uchar *);
extern void atmSig_buildCause(atmSvcType *, uchar *, uchar);
extern void atmSig_buildConnectionId(atmSvcType *, uchar *);
extern void atmSig_buildBBandLowLayerInfo(atmSvcType *, uchar *); 

/* multipoint IE's */

extern int atmSig_processEndPointReference(atmSvcType *, atmSig_ieHeader *);
extern int atmSig_processEndPointState(atmSvcType *, atmSig_ieHeader *, atmSvcPartyType *);
extern void atmSig_buildEndpointReference (atmSvcType *, uchar *, atmSvcPartyType *);
extern void atmSig_buildEndpointState (atmSvcType *, uchar *, atmSvcPartyType *);

/* atmsig_ui.c */

extern char *atmSig_printState(atmSig_statetype);
extern char *atmSig_printMsgType(int);
extern char *atmSig_printIeType(uint);
extern char *atmsig_printAalType(int);
extern char *atmsig_printAalSubtype(int);
extern char *atmSig_printAalConstBitRate(int);
extern char *atmSig_printClkRecovery(int);
extern char *atmSig_printErrorCorrection(int);
extern char *atmSig_printStructDataXfer(int);
extern char *atmSig_printSscsType(int);
extern char *atmSig_printCauseValue(int);
extern char *atmSig_printEventType(atmSig_eventtype);
extern char *atmSig_printProtocol(int);
extern void atmSig_exec_commands(parseinfo *);
extern void atmSig_closeCommand(idbtype *, int);
extern void atmSig_statusCommand(idbtype *, ushort);
extern void atmsig_show_commands(parseinfo *);
extern void atmSig_show_vc(int, hwidbtype *);
extern void atmSig_show_summary(parseinfo *csb);

/* other */

extern void atmSig_debug_init(void);
extern atmSvcPartyType *atmSig_openPartyConnection (idbtype *, staticmap_type *,
					atmSvcType *, atmSvcPartyType *);
extern atmSig_closecodetype atmSig_closePartyConnection (atmSvcType *, 
							 atmSvcPartyType *);


/* UNI 3.1 */
extern int atmSig_findUniVersion(atmSvcType *);
extern inline boolean atmSig_isV31(atmSvcType *);
extern int atmSig_checkQosClass(char, unsigned char);

#endif __ATMSIG_PRIV_H__








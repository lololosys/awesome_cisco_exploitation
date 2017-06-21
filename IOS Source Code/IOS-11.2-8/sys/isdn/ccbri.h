/* $Id: ccbri.h,v 3.4.4.2 1996/09/05 00:03:18 snyder Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbri.h,v $
 *------------------------------------------------------------------
 * ccbri.h  ISDN PRI Call Control include file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ccbri.h,v $
 * Revision 3.4.4.2  1996/09/05  00:03:18  snyder
 * CSCdi68122:  declare a bunch of things const in isdn
 *              660 out of data, 40 out of image,
 *              ain't this fun.
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/04/30  23:06:57  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/08  23:44:43  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.3  1996/02/15  17:27:22  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/28  15:24:29  dclare
 * CSCdi37836:  ISDN BRI switches DMS-100 and NI1 may need only one TEI.
 * One spid configured, use one TEI. Two spids configured, use two TEIs.
 *
 * Revision 2.1  1995/06/07  21:13:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccbri.h                                                 
DESCRIPTION:  ISDN PRI Call Control include file                      
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************

GENERAL NOTES

*/

#ifndef     CCBRI_H                       /* prevent nested inclusions */
#define     CCBRI_H                                     /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* NOTE: move to CCIE.H:
*/
#define     CHANNEL_STATUS_IE    0xF0
/* END
*/


/* default values for SAPI, CES, etc:
*/
#define     SIGNALING_SAPI       0
#define     DX25_SAPI           16

#define     BROADCAST_USID_NI1   0x7f
#define     BROADCAST_USID_5ESS  0xff

#define     BROADCAST_TID        0x7f
#define     BROADCAST_TID_NI1	0x3f
#define     EID_NOTASSIGNED     -1

#define     SIGNALING_CES        1

#define     DEFAULT_DSL          0

#define		MAX_EID			2		/* === */

/* call types:
*/
#define     VOICE_CALL           0x01
#define     CIRCUIT_DATA_CALL    0x02
#define     PACKET_DATA_CALL     0x04

/* event types: === */
#define     NON_CALL_EVENT        0x00
#define     NEW_CALL_EVENT        0x01
#define     EXISTING_CALL_EVENT   0x02
#define     INITIALIZE_TERM_EVENT 0x03
#define     ERROR_EVENT           0x04

/* length of the local directory number:
*/
#ifdef KDD_NTT
#define     PHONE_NUMB_LEN       32
#else
#define     PHONE_NUMB_LEN       16
#endif 
#define     NO_ACTIVE_CALLS      MAX_VOICE_CALLS
#define     NO_STATE_MATCH       MAX_VOICE_CALLS  /* No matching calls */
	
#define     DEFAULT_CR_LEN       1

#define     MESSAGE_TYPE_OFFSET  3
#define     PRIVATE_IE_INDEX     5
#define     MAX_ADDRESS_DIGITS   PHONE_NUMB_LEN
#define     MAX_BEARER_DATA_SIZE 3

#define     UNDEFINED_B_CHANNEL  0xff

/*
 * defines added for FIT (Fully Initializing Terminal
 */
#define     MAX_CES               2

/* These are parameters used in Terminal Initialization.
*/
#define     MAX_SPID_SIZE         (20+1)
#define     MAX_DN_SIZE           (16+1)
#define     MAX_EID_SIZE          2
#define     BEARER_TYPE_IE        0x82
#define     INITIALIZING_TERM_IE  0x83
#define     ACTIVATE_TERM_IE      0x84
#define     BEARER_ROUTING_IE     0x85
#define     DN_ROUTING_IE         0x86
#define     CES_IE                0x87
#define     DN_IE                 0x70
#define     SPID_IE               0x3A
#define     EID_IE                0x3B
/*
 * Terminal Initialization states
 */
#define     TERM_INITIALIZE      0x99               /* Used in the CCB */
#define     TERM_DOWN            0x01
#define     AWAIT_ESTABLISH      0x03
#define     AWAIT_INIT           0x04
#define     INIT                 0x05
#define     NOT_INIT             0x06
#define     INIT_NOT_SUPPORTED   0x07
#define     ESTABLISHED          0x08

#define     FIT_TIMEOUT    (TIMER_MASK_CC|0x22)      /* Term Init */
#define     FIT_TIMEOUT_DEFAULT_NI1 (UINT32) 20000   /* Term Init, 20 secs for ni1 */
#define     FIT_TIMEOUT_DEFAULT_DMS (UINT32) 15000   /* Term Init, 15 secs for dms100 */
#define     TERM_UNINITIALIZED   0x2F                /* Error Cause */

#ifdef ICLID
/* ICLID constants
*/
#define  ACTIVE_ICLID   0x01
#define  PASSIVE_ICLID  0x02
#define  SWITCH_ICLID   0x03

#define  VALID_ICLID    0xA1
#define  NO_ICLID       0xA2
#define  NO_CALLS       0xA3

#define  MAX_ICLID      16
#endif

#ifdef TASK_KB
/* The commands to hardware specific functions
*/
enum
   {
   SILENT_TONE,
   DIAL_TONE,         
   SPECIAL_DIAL_TONE, 
   RINGBACK_TONE,     
   BUSY_TONE,         
   VOICE_ALERTING,    
   CW_ALERTING
   };       
#endif

/* The states of B channels
*/
enum
   {
   CHANNEL_STATUS_REQUEST,
   CHANNEL_IDLE,
   CHANNEL_VOICE,
#ifdef USER
   CHANNEL_DATA
#else
   CHANNEL_DATA,
   CHANNEL_RESTART_REQ,
   CHANNEL_RESTART_CONF,
   CHANNEL_RESTART_ERR
#endif
   };

/* state used in search for any state (NOTE: the value of 0xFF CANNOT
*  be used by any of the switch dependent modules to indicate a valid
*  call state !!!!) 
*/
#define     DUMMY_STATE             0xFF

/* IDLE State (NOTE:  all switch dependednt modules MUST use the
*  value of 0x01 for the IDLE state !!!!)
*/
#define     IDLE_STATE              0x01

/* Used for various Call Control Timeout Functions 
*/
#define DISC_HOST_EVENT          (TIMER_MASK_CC|0x21) /* DISC sent to Host times out */
#define DISC_HOST_TIMEOUT        (UINT32) 3000     /* 3 second T/O to Host  */ 

#ifdef KDD_NTT
#define MAX_CHARGES_LEN 14 
#define DIGIT_COLLECT_TIMEOUT    (TIMER_MASK_CC|0x20) /* Used for enblock sending    */
#define DIGIT_COLLECT_DEFAULT    (UINT32) 30000  /* Digit Collect Time  */
#endif 

#ifdef BRI_1TR6
/* Following simplified state machine is used to control supported sub
*  service type activities
*/
enum  
   {
   DM_STATE_LOW_LIMIT,                              /* Siziing Element */
   DM_IDLE,                                       /* Nothing Happening */
   DM_RESPONSE_EXPECTED,                /* Response from Host expected */
   DM_REQUEST_OUTSTANDING,                   /* Waiting for a Response */
   DM_STATE_HIGH_LIMIT                               /* Sizing Element */
   };
  
#endif 

/* The names of the functions calling CCBRI_Error.
*/
enum
   {
   CCBRI_CALL_CONNECTED,
   CCBRI_CALL_CONNECTING,
   CCBRI_CALL_IDLE,
   CCBRI_CALL_DIALING,
   CCBRI_CALL_ALERTING,
   CCBRI_CALL_LOCKOUT,
   CCBRI_CALL_OFFERED,	/* === */
   CCBRI_CALL_ORIGINATE,
   CCBRI_CALL_RELEASING,
   CCBRI_CALL_ONHOOK_RELEASING,
   CCBRI_CALL_OFFHOOK_RELEASING,	/* === */
	CCBRI_CALL_PASSIVE, 
   CCBRI_CALL_REGISTER,	/* === */
   CCBRI_CALL_ROUTING,
   CCBRI_CALL_ROUTINGIN,
   CCBRI_CALL_ROUTINGOUT,
   CCBRI_CALL_CONFERENCING,
   CCBRI_DECODE_CALL_STATE,
   CCBRI_DIGIT_COLLECTION,
   CCBRI_DISCONNECTING_HOST,
   CCBRI_GET_OUT_PKT,
   CCBRI_GET_OUT_INFO,
   CCBRI_GO,
   CCBRI_SEND_TRACE,
   CCBRI_GET_HCB_INDEX,
   CCBRI_GET_L3_EVENT,
   CCBRI_GET_NEW_CCB,
   CCBRI_VOICE_CONNECTED,
   CCBRI_VOICE_CONFERENCING,
   CCBRI_VALID_HW_EVENT,
   CCBRI_INIT,
   CCBRI_MAIL_TO_L3,
   CCBRI_MAIL_TO_HOST,
   CCBRI_REMOVE_CCB,
   CCBRI_RELEASE_CHAN,
   CCBRI_RELEASE_CALL,
   CCBRI_RESIZE_INFO,
   CCBRI_SET_NEW_CHAN,
	CCBRI_REG_MAINTENANCE  
   };

/* Error codes reported to CCBRI_Error.
*/
enum
   {
   NO_ERROR,                              /* The "no error" error code */
   SEND_SBC_FAILED,
   SEND_PKT_FAILED,
   SEND_BUF_FAILED,
   UNKNOWN_EVENT,
   NO_CCB,
   INVALID_HW_EVENT,
   UNKNOWN_CALL,
   UNKNOWN_MSG_SOURCE,
   NO_CALL_SLOT,
   INVALID_STATE,
   INVALID_PAS_CCB,
   INVALID_SWITCH_TYPE,
   OUT_OF_PKTS,
   INITIALIZATION_FAILED,
   CCB_CORRUPTED,
   OUT_OF_CHANNELS,
   OUT_OF_INFO_BUFFERS,
   CCB_OR_CHAN_TABLE_CORRUPTED,
   CCB_LIST_CORRUPTED,
   OUT_OF_CCBS,
   EVENT_PENDING,
   INOPPORTUNE_MSG,
   HCB_CORRUPTED
   };


#ifdef VN2
/* Various VN2 constants.
*/
#define DIAL_KEY              0xEF          /* Request to dial on-hook */
#define EVENT_PENDING            1           /* Internal event pending */
#define VOICE_BEARER_DATA_LEN    3         /* Bearer capability length */
#define NO_ACTIVE_CALLS  MAX_VOICE_CALLS            /* No active calls */
#define NO_STATE_MATCH   MAX_VOICE_CALLS /* No calls of state specified */
#define NO_CALL          MAX_VOICE_CALLS        /* No calls identified */
#define NO_SERVICE_PENDING       0     /* CCB service function default */
#define INTERNAL_RECONNECT       1                   /* Internal event */

/* Ringing type definitions.
*/
#define SIMPLE_RING              1
#define CW_RING                  2

#define CANCEL_KEY            0xE6

/* VN2 error codes
*/
#define UNEXPECTED_MSG          -2
#define MISSING_MANDATORY_IE    -3
#define INVALID_INFO_ELEMENT    -4
#define UNACCEPTABLE_CHANNEL    -5
#define BAD_CALLED_ADDR         -9
#define BAD_BEARER_CAP         -10
#define MAND_IE_LEN_ERR        -12
#define DIGIT_BUFFER_OVERFLOW  -13

#endif                                                          /* VN2 */
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/* NOTE for Hcb: Active_Call == 0 mns there is no active call.  This
*                 implies that the index for Ccb_In_Use starts at 1.
*/
typedef struct                         /* Voice hardware control block */
   {
   BYTE    Hw_State;
   BOOLEAN Established;
   char    Ldn            [ MAX_ADDRESS_DIGITS ];     
   BYTE    Bearer_Data    [ MAX_BEARER_DATA_SIZE ];
   int     Active_Call;
   void   *Ccb_In_Use     [ MAX_VOICE_CALLS ]; 
   } HCB, *HCB_ID;

extern HCB Hcb[ MAX_DSL ];

/*
 * merging the BRI and PRI Call Control Block into one structrue
 * the CC_STATE stuff was in ccpri.h
 */
typedef void ( *CC_STATE )( int );  /* CCB state function type */
struct CallControlBlock
    {
    int     State;                               /* Call Control state */
    CC_STATE PriState;                            /* Call Control state */
    int     ChanType;                 /* B channel type - B, H0 or H11 */
#ifdef PRI_1TR6
    BYTE    Ics;                                     /* channel select */
    BOOLEAN DM_Substate;                            /* FAC outstanding */
#endif
    BOOLEAN Responded;                       /* CC_BROADCAST_RESP sent */
    int     CallType;                              /* Voice/Data/X25 ? */
    int     B_Chan;                                /* B channel in use */
    int     Call_ID;                            /* Call identification */
    int     CallRef;                                 /* Call reference */
    BYTE    Ces;                         /* Connection Endpoint Suffix */
    BYTE    Sapi;                    /* Suffix Access Point Identifier */
    int     CallAppearance;            /* Call Appearance, what else ? */
    BOOLEAN MakePrivateMsg;           /* Compose private IE messages ? */
    BOOLEAN CopyPublicMsg;      /* Copy CCITT compatible IE messages ? */
    BOOLEAN FirstDigit;      /* Does the dial tone require termination */
    int     HcbIndex;                      /* index to a list of HCB's */
    int     CallRefLen;                       /* Call reference length */
    int     Cause;                        /* save for Disconnect Host  */     
    int     Host_Specifier;                    /* index to Host_CB === */
    char   Address[ MAX_ADDRESS_DIGITS + 1 ];          /* Digit buffer */
    int     OtherCallIndex;                          /* OtherCallIndex */
    int     CCVNTimer;                     /* Timer for key collection */
    int     CCDMSTimer;                   /* Timer for Host Disconnect */
#ifdef BRI_1TR6
    int     CC1TR6Timer;                  /* Timer for Host Disconnect */
	int     DM_Substate;              /* Control subservice functions  */
#endif 
    int     CC5essTimer;                  /* Timer for Host Disconnect */
    int     NtkdTimer;                     /* Timer for key collection */
                                              /* and host disconnect   */
    BOOLEAN CollectAddress;                /* Collect called address ? */
    int     Service;                               /* service activity */
    BOOLEAN Eid_matched;                  /* see if eid matched or not */
/*
 * keep this ifdef around for a while ===
 */
#ifdef PRI
    int      Int_Id_Len;                        /* Interface ID length */
    BYTE     Int_Id[ 1 ];    /* Chan ID int ID storage, MAX_INT_ID_LEN */
    BOOLEAN  Exclusive;                           /* Exclusive channel */
    BOOLEAN  ClearHost;                   /* HOST knows about the call */
    BOOLEAN  Congested;                  /* UUI flow control in effect */
#endif
    struct  CallControlBlock *Next;                      /* Next block */
    };
typedef struct CallControlBlock *CCB_ID;

/* NOTE for HostControlBlock: this structure contains all IEs 
*    necessary to manage the message flow between the Host(s) and
*    Call Control.   
*/
typedef struct                         
   {
   char   DN [MAX_DN_SIZE];                                      /* DN */
   BOOLEAN DN_Routing;                  /* Incoming Calls Routed by DN */
   int     Bearer_Type;               /* Voice - 1; Circuit Data - 2;  */
                                                    /* Packet Data -3  */
                                               /* Bearer Capabilities  */
   BOOLEAN Bearer_Routing;    /* Incoming Calls Routed by Bearer Type  */
   INT16   Host_ID;                         /* ID of Destination Host  */
   int     Ces;                                                 /* CES */
   BYTE   SPID [MAX_SPID_SIZE];                                /* SPID */
   BOOLEAN Initializing_Terminal;         /* Requires an EID from Net  */
   EPID    epid;                                       /* Endpoint ID */
   int     Terminal_State;            /* 1-TERM_DOWN,3-AWAIT_ESTABLISH */
                                     /* 4-AWAIT_INIT,5-INIT,6-NO_INIT  */
                                                     /* 8-ESTABLISHED  */
   int     Timer_T415;                      /* 15 second Terminal Init */
   BOOLEAN First_T415;
   void    *SS;               /* Pointer to Sup Services Control Block */
   BYTE    *HostMsg;                        /* Pointer to Host Message */
   int     Msglen;                           /* Length of Host Message */
   BOOLEAN spid_valid;
   } HOST_CB, *H_ID;
extern HOST_CB Host_CB [MAX_DSL] [ MAX_HOSTS ]; /* === */

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/* CCB memory types for various applications
*/

/* 
 * None of this stuff is used outside of ccbri.c, so we do away with
 * the externs right here.
 */
#ifdef NEVER_REFERENCED
#ifdef BRI_5ESS
extern  int     Btype_5ESS_CCB;         /* buffer type for local CCB's */
#endif
#ifdef BRI_DMS100
extern  int     Btype_DMS_CCB;          /* Buffer type for local CCB's */
#endif
#ifdef KDD_NTT
extern  int     Btype_KDDNTT_CCB;       /* Buffer type for local CCB's */
#endif 
#ifdef VN2
extern  int     Btype_VN2_CCB;          /* Buffer type for local CCB's */
#endif
#ifdef BRI_1TR6
extern  int     Btype_1TR6_CCB;       	 /* Buffer type for local CCB's */
#endif 
#ifdef BRI_NI1
extern  int     Btype_NI1_CCB;          /* Buffer type for local CCB's === */
#endif
#ifdef BRI_NET3	/*===*/
extern int      Btype_NET3_CCB;          /* Buffer type for local CCB's ===*/
#endif
#ifdef BRI_TS013
extern int      Btype_TS013_CCB;          /* Buffer type for local CCB's ===*/
#endif
#endif /* NEVER_REFERENCED */


/* CCB heads, tails and the CCB 
*/
extern CCB_ID   CCB_Head[ MAX_DSL ];
#ifdef NEVER_REFERENCED
extern CCB_ID   CCB_Tail[ MAX_DSL ];
#endif /* NEVER_REFERENCED */
extern CCB_ID   CCB; 
extern BYTE     Dsl_ID;             /* Current digital subscriber loop */
extern int      Call_Id_Source;      /* Master "new" call_id generator */
extern int      Source;                       /* Source of current msg */
extern int      L3_State;                               /* State of L3 */
#ifdef NEVER_REFERENCED
extern int      SmlBufSize;    /* For re-size comparison in CCBRI_Mail */
#endif /* NEVER_REFERENCED */
extern int      ActiveCCBs[ MAX_DSL ];         /* CCBs in use counters */
extern int      AllocatedCCBs;                  /* CCBs in use counter */
#ifdef NEVER_REFERENCED
extern struct   CallControlBlock Dummy_Ccb;           /* The dummy CCB */
#endif /* NEVER_REFERENCED */
extern BYTE *   IE;                    /* Pointer to CCIE info element */
extern BYTE *   IE_Fields;     /* Pointer to CCIE buffer for IE unpack */
extern int      TheTrash;            /* use to avoid compiler warnings */
extern PKT_ID   InPkt;                     /* Internal packet pointers */
extern PKT_ID   OutPkt;                    /* Internal packet pointers */
extern BYTE *   InPkgInfo;                  /* Ptr to the package info */
extern int      InPkgInfoLen;                   /* Package info length */
extern BYTE *   OutPkgInfo;             /* Pointer to the package info */
extern int      OutPkgInfoLen;                  /* Package info length */
#ifdef NEVER_REFERENCED
extern BUFFER_TYPE OutPkgInfoType;                /* Package info type */
#endif /* NEVER_REFERENCED */

extern PUBLIC BYTE Mail_priority;    /* control mail priority for msgs */
extern int isdn_pu_establish;
extern me_activate_ok(int, int);

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/* This macro is intended to conditonally call the function CCBRI_SendTrace
*  if SEND_TR_MSG is defined, and to do nothing otherwise.  The SEND_TR_MSG
*  flag is enabled for applications using TRACKER.C.
*/
#ifdef SEND_TR_MSG
#define CCBRI_TRACE(s)  CCBRI_SendTrace(s)
#else
#define CCBRI_TRACE(s)                                          /* NOP */
#endif

/* Alternate versions of string functions provided on a compiler-basis.
*/
#define STRCPY(s1,s2)    strcpy((s1),(s2))
#define STRCAT(s1,s2)    strcat((s1),(s2))
#define STRLEN(s)        strlen((s))

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S               *
*************************************************************************
*/

PUBLIC     void       isdn_powerup               (int);
PUBLIC     void       CCBRI_Go                   (int, int, int, PKT_ID, int, BYTE, BYTE, BYTE);
PUBLIC     void       CCBRI_Init                 (void);

PUBLIC     int        CCBRI_AcceptBearerCap      (void);
PUBLIC     int        CCBRI_AcceptCalledNumb     (int *, BYTE);
PUBLIC     int        CCBRI_AcceptCallingNumb    (BYTE *, int *);
PUBLIC     int        CCBRI_AcceptCause          (BYTE *);
PUBLIC     int        CCBRI_AcceptChanId         (BYTE *, BYTE *);
#ifdef BRI_1TR6
PUBLIC     int        CCBRI_AcceptConnectedAddr  (int *, BYTE);
#endif
PUBLIC     int        CCBRI_AcceptDisplay        (char *);
PUBLIC     int        CCBRI_AcceptEndpointId     (int *, int *, int *);
PUBLIC     int        CCBRI_AcceptKeypad         (char *);
PUBLIC     int        CCBRI_AcceptProgress       (BYTE *);
PUBLIC     int        CCBRI_AcceptSendingComplete (BYTE *);
PUBLIC     int        CCBRI_AcceptSignal         (BYTE *);
PUBLIC     void       CCBRI_Error                (int, int, int, int, BOOLEAN);
#ifdef VN2
PUBLIC     int        CCBRI_GetCallIndex         (int, BYTE);
#endif
#ifdef BRI_5ESS
PUBLIC     int        CCB5E_DecodeCallState      (int *);
#endif 
#ifdef BRI_1TR6
PUBLIC     int        CCBRI_AcceptServiceInd     (BYTE *, BYTE *, BOOLEAN); 
PUBLIC     int        CCBRI_AcceptFacilities     (BYTE *, BOOLEAN, BOOLEAN);
#endif
PUBLIC     INT16      CCBRI_HostRouter           (void);
PUBLIC     void       CCBRI_DisconnectHostCall   (BYTE *, int);
PUBLIC     void       CCBRI_FreeCallBufs         (void);  
PUBLIC     BYTE *     CCBRI_GetLdn               (void);
PUBLIC     void       CCBRI_GetL3Event           (int *);
PUBLIC     int        CCBRI_GetNewCall_Id        (void);
PUBLIC     CCB_ID     CCBRI_GetNewCCB            (BYTE, int, BYTE, BYTE, int, int);
PUBLIC     BOOLEAN    CCBRI_GetOutInfo           (BYTE);
PUBLIC     BOOLEAN    CCBRI_GetOutPkt            (BYTE, BOOLEAN);
PUBLIC     CCB_ID     CCBRI_GetDummyCcb          (int, BYTE, BYTE);
PUBLIC     int        CCBRI_GetHcbIndex          (BYTE);
PUBLIC     BYTE *     CCBRI_GetPublicIE          (int, int);
PUBLIC     int        CCBRI_DecodeCallState      (int *);
PUBLIC     CCB_ID     CCBRI_GetL3CCB             (int, int, BYTE, int, BYTE);
PUBLIC     BOOLEAN    CCBRI_Match                (const BYTE *, int, BYTE);
PUBLIC     void       CCBRI_MailtoL3             (void);
PUBLIC     void       CCBRI_MailtoHost           (BOOLEAN);
PUBLIC     void       CCBRI_RejectCall           (int, int);
PUBLIC     void       CCBRI_RejectMsg            (int, int);
PUBLIC     void       CCBRI_RejectHostCall       (int);
PUBLIC     void       CCBRI_RejectHostMsg        (BYTE);
PUBLIC     void       CCBRI_RejectSetup          (int, int);
PUBLIC     void       CCBRI_ReleaseChan          (void);
PUBLIC     void       CCBRI_ReleaseCall          (void);
PUBLIC     void       CCBRI_ReleasedLink         (BYTE, BYTE);
PUBLIC     void       CCBRI_RemoveCCB            (CCB_ID);
PUBLIC     void       CCBRI_ResizeInfo           (BOOLEAN);
PUBLIC     void       CCBRI_SendSpid             (int, int);
#ifdef SEND_TR_MSG
PUBLIC     void       CCBRI_SendTrace            (int);
#endif
PUBLIC     int        CCBRI_SetNewChan           (int, int, BOOLEAN);
#ifdef COMP_TEST
PUBLIC     BOOLEAN    CCBRI_SpecialFunction      (int);
#endif
PUBLIC     void       CCBRI_ShutDown             (BYTE);
PUBLIC     BOOLEAN    CCBRI_ValidateHwEvent      (int, BYTE);
#ifdef TASK_KB
#ifdef VN2
#endif
PUBLIC     void       CCBRI_StartConnect         (int);
PUBLIC     void       CCBRI_StartDigitDetect     (void);
PUBLIC     void       CCBRI_StartRing            (BYTE);
PUBLIC     void       CCBRI_StartTone            (BYTE);
#endif                                                      /* TASK_KB */
PUBLIC     void       CCBRI_StatusInd            (void);
#ifdef TASK_KB
PUBLIC     void       CCBRI_StopConnect          (int);
PUBLIC     void       CCBRI_StopDigitDetect      (void);
PUBLIC     void       CCBRI_StopRing             (BYTE);
#endif
PUBLIC     void       CCBRI_HostDisconnect       (int);
PUBLIC     void       CCBRI_HostQuery            (void);
PUBLIC     void       CCBRI_HostInfo             (void);
PUBLIC     int        CCBRI_HostCallRequest      (void);
PUBLIC     void       CCBRI_HostConnect          (void);
PUBLIC     BYTE *     CCBRI_GetHost_IE           (BYTE *, BYTE);
PUBLIC     void       CCBRI_Restart              (int, BYTE);
PUBLIC     int        CCBRI_CopyPublic_IE        (BYTE);

PUBLIC     int        CCBFIT_AcceptEndpointId    (int *, int *, int *);
PUBLIC     int        CCBFIT_TerminalInit        (int, int, int);
PUBLIC     void       CCBFIT_TerminalParms       (int, int, BYTE *);
PUBLIC     void       CCBFIT_GetEpid             (int *, int *, int *, int);
PUBLIC     void       CCBFIT_TerminalDown        (BYTE, int);
PUBLIC     BOOLEAN    ccbfit_validspid           (int, int);

/* Decode Call State and Release Call functions are declared here:
*/
#ifdef BRI_5ESS
#ifdef USER
PUBLIC     int        CCB5E_DecodeCallState       (int *);
PUBLIC     void       CCB5E_ReleaseCall           (void);
PUBLIC     void       CCB5E_ReleasedLink          (BYTE, BYTE);
PUBLIC     void       CCB5E_Restart               (int, BYTE);
PUBLIC     void       CCB5E_Shutdown              (BYTE);
#else                                                       /* Network */
PUBLIC     int        CCB5EN_DecodeCallState      (int *);
PUBLIC     void       CCB5EN_ReleaseCall          (void);
PUBLIC     void       CCB5EN_ReleasedLink         (BYTE, BYTE);
PUBLIC     void       CCB5EN_Restart              (int, BYTE);
PUBLIC     void       CCB5EN_RestartReq           (int, int, BOOLEAN);
#endif
#endif
#ifdef BRI_DMS100
PUBLIC     int        CCBDMS_DecodeCallState      (int *);
PUBLIC     void       CCBDMS_ReleaseCall          (void);
PUBLIC     void       CCBDMS_ReleasedLink         (BYTE, BYTE);
PUBLIC     void       CCBDMS_Restart              (BYTE);
PUBLIC     void       CCBDMS_Shutdown             (BYTE);
#endif
#ifdef  KDD_NTT
PUBLIC     int        CCBNTKD_DecodeCallState     (int *);
PUBLIC     void       CCBNTKD_ReleaseCall         (void);
PUBLIC     void       CCBNTKD_Restart             (BYTE);
PUBLIC     void       CCBNTKD_Shutdown            (BYTE);
#endif 
#ifdef VN2
PUBLIC     int        CCBVN_DecodeCallState       (int *);
PUBLIC     void       CCBVN_ReleaseCall           (void);
PUBLIC     void       CCBVN_ReleasedLink          (BYTE, BYTE);
PUBLIC     void       CCBVN_Shutdown              (BYTE);
#endif
#ifdef BRI_NET3	/*===*/
PUBLIC     int        CCBNET3_DecodeCallState     (int *);
PUBLIC     void       CCBNET3_ReleaseCall         (void);
PUBLIC     void       CCBNET3_ReleasedLink        (BYTE, BYTE);
PUBLIC     void       CCBNET3_Shutdown            (BYTE);
#endif
#ifdef BRI_TS013
PUBLIC     int        CCBTS013_DecodeCallState    (int *);
PUBLIC     void       CCBTS013_ReleaseCall        (void);
PUBLIC     void       CCBTS013_ReleasedLink       (BYTE, BYTE);
PUBLIC     void       CCBTS013_Shutdown           (BYTE);
#endif
#ifdef BRI_1TR6
PUBLIC     int        CCB1TR6_DecodeCallState     (int *);
PUBLIC     void       CCB1TR6_ReleaseCall         (void);
PUBLIC     void       CCB1TR6_ReleasedLink        (BYTE, BYTE);
PUBLIC     void       CCB1TR6_Shutdown            (BYTE);
#endif
#ifdef  BRI_NI1	/*===*/
PUBLIC     void       CCBNI1_ReleaseCall          (void);
PUBLIC     void       CCBNI1_Restart              (BYTE);
PUBLIC     void       CCBNI1_Shutdown             (BYTE);
PUBLIC     int        CCBNI1_DecodeCallState      (BOOLEAN *);
PUBLIC     void       CCBNI1_ReleasedLink         (BYTE, BYTE);
#endif
PUBLIC     void       ME_got_mnl_est_conf         (int, int ,int);
PUBLIC     BOOLEAN    ME_got_mnl_mim_ind          (int, int, int, PKT_ID);
PUBLIC     void       CCBNTKD_ReleasedLink        (BYTE, BYTE);
PUBLIC     void       show_host_cb                (int);
#endif


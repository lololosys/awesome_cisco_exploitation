/* $Id: ccpri.h,v 3.3.4.1 1996/04/30 23:07:14 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccpri.h,v $
 *------------------------------------------------------------------
 * ccpri.h  ISDN PRI Call Control include file 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ccpri.h,v $
 * Revision 3.3.4.1  1996/04/30  23:07:14  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/08  23:44:49  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.2  1995/11/17  17:40:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:14:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __CCPRI_H__
#define __CCPRI_H__
/************************************************************************
*************************************************************************

MODULE:       ccpri.h
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

*************************************************************************
*************************************************************************
*/

#ifndef     CCPRI_H                       /* prevent nested inclusions */
#define     CCPRI_H                                     /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* The names of the functions calling CCPRI_Error.
*/
enum
   {
   CCPRI_ACCEPT_CHAN_ID,
   CCPMSG_CHAN_STATUS,
   CCPRI_FAC_ACK_OUT,
   CCPRI_GET_L3_EVENT,
   CCPRI_GET_NEW_CCB,
   CCPRI_GET_OUT_INFO,
   CCPRI_GET_OUT_PKT,
   CCPRI_GO,
   CCPRI_INIT,
   CCPRI_MAIL,
   CCPMSG_OUT_CALL,
   CCPRI_RELEASE_CALL,
   CCPRI_RESIZE_INFO,
   CCPMSG_RESTART,
   CCPMSG_RESTART_REQ,
   CCPCC_CALL_CONNECTED,
   CCPCC_CALL_IDLE,
   CCPCC_CALL_OFFERED,
   CCPCC_CALL_ORIGINATION,
   CCPCC_CALL_RELEASING,
   CCPCC_CALL_RINGING,
   CCPCC_CALL_IN_ROUTING,
   CCPCC_CALL_OUT_ROUTING,
   };

/* Error codes reported to CCPRI_Error.
*/
enum
   {
   P_SEND_PKT_FAILED,                              /* LIF_SendPkt failed */
   P_SEND_BUF_FAILED,                 /* Package info buffer mail failed */
   P_UNKNOWN_EVENT,                       /* Unrecognized event received */
   P_NO_CCB,                                              /* Out of CCBs */
   P_UNKNOWN_CALL,                  /* Can't locate CCB for a call event */
   P_UNKNOWN_MSG_SOURCE,                         /* Unknown sending task */
   P_NULL_CCB,                              /* Function found CCB = NULL */
   P_OUT_OF_INFO_BUFFERS,                        /* Out of package infos */
   P_OUT_OF_PKTS,                                /* Out of packet memory */
   CCPRI_INITIALIZATION_FAILED,        /* Module initialization failed */
   P_CCB_NOT_FOUND,                            /* CCB not on linked list */
   P_OUT_OF_CHANNELS,             /* Could not get a new CC_CHAN channel */
   P_BAD_DATA_FROM_HOST,                     /* Bad data in host message */
   P_DUMMY_CCB_FOUND,     /* Dummy CCB executing where it ain't outta be */
   P_BAD_SWITCHTYPE,                     /* SwitchType is not recognized */
   P_RESTART_ERR,           /* An error occurred in processing a restart */
   P_ADD_TIMER_FAILED,                 /* LIF_AddTimer returned an error */
   P_UPDATE_TIMER_FAILED,           /* LIF_UpdateTimer returned an error */
   P_INTERFACE_ID_OVERFLOW,     /* Rcvd Int ID exceeds CCB storage space */
   P_DSL_OUT_OF_RANGE              /* Bad Digital Subscriber Loop number */
   };

#ifdef PRI_1TR6 
#define PRI_1TR6_STD_CALL_REF_LEN  1   /* Default call ref byte length */
#endif 
#define PRI_STD_CALL_REF_LEN      2  /* Default call reference byte length */

#define ACCEPTED              0        /* Information element accepted */
#define DM_REQUEST_OUTSTANDING 7               /* 1TR6 sup service req */

/* for CCPRI_GetNewCall_Id() - to find free call id in range 0 - 0x8000 */
#define PRI_CALL_ID_MIN          0
#define PRI_CALL_ID_MAX     0x8000
#define SEARCH_CNT_INIT          0   /* Check to prevent runaway search */
#define SEARCH_CNT_MAX          30

/* Timers implemented in CC
*/
#define T316_EXPIRY           0x14B0                  /* Restart Timer */
#define T3M1_EXPIRY           0x14F0         /* AT&T Service Ack Timer */
#ifndef L3_TEST
#define CCPRI_4ESS_T316       (UINT32) 120000L          /* 120 seconds */
#define CCPRI_4ESS_T3M1       (UINT32) 120000L          /* 120 seconds */
#define CCPRI_5ESS_T316       (UINT32) 120000L          /* 120 seconds */
#define CCPRI_5ESS_T3M1       (UINT32) 120000L          /* 120 seconds */
#define CCPRI_DMS100_T316     (UINT32) 120000L          /* 120 seconds */
#define CCPRI_VN2_T316        (UINT32) 120000L          /* 120 seconds */
#define CCPRI_NTT_T316        (UINT32) 120000L          /* 120 seconds */
#define CCPRI_NET5_T316       (UINT32) 120000L          /* 120 seconds */
#define CCPRI_TS014_T316      (UINT32) 120000L          /* 120 seconds */
#else
#define CCPRI_4ESS_T316       (UINT32)   5000             /* 5 seconds */
#define CCPRI_4ESS_T3M1       (UINT32)   5000             /* 5 seconds */
#define CCPRI_5ESS_T316       (UINT32)   5000             /* 5 seconds */
#define CCPRI_5ESS_T3M1       (UINT32)   5000             /* 5 seconds */
#define CCPRI_DMS100_T316     (UINT32)   5000             /* 5 seconds */
#define CCPRI_VN2_T316        (UINT32)   5000             /* 5 seconds */
#define CCPRI_NTT_T316        (UINT32)   5000             /* 5 seconds */
#define CCPRI_NET5_T316       (UINT32)   5000             /* 5 seconds */
#define CCPRI_TS014_T316      (UINT32)   5000             /* 5 seconds */
#endif

/* Network vs User sensitive defines
*/
#define AM_I_THE_NETWORK_SIDE FALSE
#define MY_LOCATION           LOC_USER

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/

/*
 * Note:
 * the PRI call control block shares the BRI call control block, found
 * in ccbri.h
 *
 * typedef struct CallControlBlock *CCB_PTR;
 */

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S               *
*************************************************************************
*/

/* The following declarations are for the PUBLIC functions defined
*  in the C.O. switch-specific call state modules which are driven
*  by CCPRI.C.
*/

PUBLIC CC_STATE CCPCC_InitialState        (void );
#ifdef TRACING
PUBLIC BYTE     CCPCC_NumericState        (CC_STATE );
#endif

/* The following declarations are for the PUBLIC functions in CCPMSG.C.
*/
PUBLIC void     CCPMSG_AppendHostUUI       (void);
PUBLIC void     CCPMSG_ChanStatus          (void);
PUBLIC void     CCPMSG_DiscHost            (int);
PUBLIC BOOLEAN  CCPMSG_DiscPeer            (int);
PUBLIC void     CCPMSG_FacAckIn            (void);
PUBLIC void     CCPMSG_FacAckOut           (void);
PUBLIC void     CCPMSG_FacRejIn            (void);
PUBLIC void     CCPMSG_FacRejOut           (void);
PUBLIC void     CCPMSG_FacReqIn            (void);
PUBLIC void     CCPMSG_FacReqOut           (void);
#ifdef PRI_1TR6
PUBLIC int      CCPMSG_GetHostChannel      (void); 
#endif 
PUBLIC BOOLEAN  CCPMSG_InCall              (void);
PUBLIC void     CCPMSG_InfoIn              (void);
PUBLIC void     CCPMSG_NotifyIn            (void);
PUBLIC void     CCPMSG_NotifyOut           (void);
PUBLIC BOOLEAN  CCPMSG_OutCall             (void);
PUBLIC void     CCPMSG_RejectInCall        (int, int);
PUBLIC void     CCPMSG_RejectMsg           (int, int);
PUBLIC void     CCPMSG_RejectOutCall       (int, BOOLEAN);
PUBLIC void     CCPMSG_RejectResponse      (int, int);
PUBLIC void     CCPMSG_ReleaseInd          (void);
PUBLIC void     CCPMSG_Restart             (int, int);
PUBLIC void     CCPMSG_RestartReq          (int, int, BOOLEAN);
PUBLIC void     CCPMSG_SendHostChanStat    (int, int, int);
PUBLIC void     CCPMSG_SendHostCongest     (void);
PUBLIC void     CCPMSG_SendHostDisc        (int, BOOLEAN);
PUBLIC void     CCPMSG_SendHostInfo        (void);
PUBLIC void     CCPMSG_SendHostProgress    (int, BOOLEAN, BOOLEAN);
PUBLIC void     CCPMSG_SendHostUUI         (void);
PUBLIC void     CCPMSG_SendPeerCongest     (void);
PUBLIC void     CCPMSG_SendPeerUUI         (void);


/* The following declarations are for the PUBLIC functions in CC_SERV.C.
*/
PUBLIC BOOLEAN  CC_SERV_ChangeRequest      (BYTE, int, int, int, \
                                                  BOOLEAN, INT16, UINT32);
PUBLIC int      CC_SERV_ServiceMsg         (PKT_ID, BYTE, BYTE *);
PUBLIC void     CC_SERV_ServTimeout        (BYTE, int, int,
                                                  INT16, UINT32);

/* The following declarations are for the PUBLIC functions in CCPRI.C.
*/
PUBLIC int      CCPRI_AcceptBearerCap      (int *);
PUBLIC int      CCPRI_AcceptCdNumb         (void);
PUBLIC int      CCPRI_AcceptCdSubAddr      (void);
PUBLIC int      CCPRI_AcceptCause          (int *);
PUBLIC int      CCPRI_AcceptChanId         (int, BOOLEAN);
PUBLIC int      CCPRI_AcceptCrNumb         (void);
PUBLIC int      CCPRI_AcceptCrSubAddr      (void);
PUBLIC int      CCPRI_AcceptDisplay        (void);
PUBLIC int      CCPRI_AcceptNetFac         (void);
PUBLIC int      CCPRI_AcceptNotify         (int *);
PUBLIC int      CCPRI_AcceptProgressInd    (int *);
PUBLIC int      CCPRI_AcceptSignal         (void);
PUBLIC int      CCPRI_AcceptUserUser       (void);
PUBLIC void     CCPRI_CopyPublic           (int, int);
PUBLIC void     CCPRI_CopyPublicAll        (void);
PUBLIC void     CCPRI_CopyPublicShifted    (int, int, BOOLEAN);
PUBLIC void     CCPRI_Error                (int, int, int, BOOLEAN);
PUBLIC void     CCPRI_FreePkgInfo          (void);
PUBLIC CCB_ID   CCPRI_GetDummyCcb          (int);
PUBLIC CCB_ID   CCPRI_GetNewCCB            (int);
PUBLIC BYTE    *CCPRI_GetPrivate           (int, int);
#ifdef PRI_1TR6
PUBLIC     int      CCPRI_GetDate          (void);
PUBLIC     int      CCPRI_GetCharges       (void); 
PUBLIC     int      CCPRI_GetFacilities    (BOOLEAN);
PUBLIC     int      CCPRI_AcceptFacilities (BYTE *, BOOLEAN, BOOLEAN);
PUBLIC     int      CCPRI_AcceptServiceInd (BYTE *, BYTE *, BOOLEAN);
#endif 
#ifdef EXTENDED_HOST_MSG
PUBLIC BYTE    *CCPRI_GetPublic            (int, int);
#endif
PUBLIC BOOLEAN  CCPRI_GetOutInfo           (int);
PUBLIC BOOLEAN  CCPRI_GetOutPkt            (BYTE);
PUBLIC void     CCPRI_Go                   (int, int, PKT_ID, int, \
                                                  int, int, BYTE);
PUBLIC BOOLEAN  CCPRI_Init                 (void);
PUBLIC void     CCPRI_Mail                 (int, BOOLEAN);
PUBLIC int      CCPRI_MapIntId             (int, BOOLEAN);
PUBLIC int      CCPRI_RateToType           (int);
PUBLIC void     CCPRI_ReleaseCall          (void);
PUBLIC void     CCPRI_ReleaseChan          (void);

PUBLIC BYTE     CCPCC_VerifyDiscIEs        (BYTE *, int);
PUBLIC BYTE     CCPCC_VerifyRestartIEs     (BYTE *, int, int *);

/************************************************************************
*                    D A T A   A L L O C A T I O N                      *
*************************************************************************
*/
/* These elements allocate memory through CCPRI.C
*/
#ifdef NEVER_REFERENCED
/*
 * Previously public stuff not used outside of ccpri.c
 */
extern int          AddrLen;                /* Host msg address length */
extern int          Btype_PRI_CCB;      /* Buffer type for local CCB's */
extern CCB_ID       CCB_Tail[ MAX_DSL ];
extern int          CC_SwitchType;  /* Record of initialzed switchtype */
extern struct       CallControlBlock Dummy_Ccb;       /* The dummy CCB */
extern BOOLEAN      MultiPRI;                        /* Multiple DSLs? */
extern BUFFER_TYPE  OutInfoBufType;                /* Info buffer type */
extern int          SmlInfoSize;          /* Size of small info buffer */
#endif /* NEVER_REFERENCED */

extern int          AllocatedCCBs;
extern int          Call_Id_Source;        /* "Well" for new Call_ID's */
extern BYTE         Diag[ 2 ];              /* Cause diagnostic buffer */
extern BYTE         Dsl_Id;                    /* Interface identifier */
extern CCB_ID       CCB_Head[ MAX_DSL ];
extern CCB_ID       CCB;
extern int          Host_IEs_Len;                /* Length of HOST IEs */
extern BYTE        *IE;                /* Pointer to CCIE info element */
extern BYTE        *IE_Fields;     /* Ptr to CCIE buffer for IE unpack */
extern PKT_ID       InPkt, OutPkt;         /* Internal packet pointers */
extern BOOLEAN      MultiInterface;            /* Multiple interfaces? */
extern BYTE        *OutPkgInfo;             /* Ptr to the package info */
extern int          OutPkgInfoLen;              /* Package info length */
extern int          SourceState;                        /* State of L3 */
extern UINT32       T316_TimeOut;             /* Restart timer timeout */

#ifdef EXTENDED_HOST_MSG
#ifdef NEVER_REFERENCED
extern int          PrivInfoLenOffset;   /* Host private IE len offset */
extern int          Q931_IEs_Len;  /* Len of Q931 elements in HOST msg */
#endif /* NEVER_REFERENCED */
#endif                                            /* EXTENDED_HOST_MSG */
#endif                                                      /* CCPRI_H */

extern BOOLEAN Network_side;          /* allow PRI to run back-to-back */

#endif __CCPRI_H__

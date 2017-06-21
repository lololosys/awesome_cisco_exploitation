/* $Id: l2.h,v 3.3.42.1 1996/04/30 23:07:34 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l2.h,v $
 *------------------------------------------------------------------
 * l2.h  ISDN LAPD include file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: l2.h,v $
 * Revision 3.3.42.1  1996/04/30  23:07:34  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/22  05:23:28  dclare
 * CSCdi42578:  %SYS-3-HARIKARI: Process ISDN top-level routine exited.....
 * Make sure that Persistent deactivation does not get confused with a
 * timer, free the memory.
 *
 * Revision 3.2  1995/11/17  17:41:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: l2.h
VERSION: 3.21
DESCRIPTION: ISDN LAPD include file

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

This include file contains definitions that are shared between both the
L2 core module (l2.c) and the L2 SDL module (l2_sdl).

*************************************************************************
*************************************************************************
*/


#ifndef     L2_H                          /* prevent nested inclusions */
#define     L2_H                                        /* module name */

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/

/* Layer 2 States (see Annex B/Q.921)
*/
#define     TEI_UNASSIGNED                1
#define     ASSIGN_AWAITING_TEI           2
#define     ESTABLISH_AWAITING_TEI        3
#define     TEI_ASSIGNED                  4
#define     AWAITING_ESTABLISHMENT        5
#define     AWAITING_RELEASE              6
#define     MULTIPLE_FRAME_ESTABLISHED    7
#define     TIMER_RECOVERY                8

/* Connection Management States
*/
#define     CM_IDLE            0x00                         /* CM idle */
#define     CM_AWAIT_CONF      0x01             /* CM awaiting confirm */
#define     CM_TEI_MANAGE      0x02               /* LM tei management */

/* Internal Events -- 1st nibble is always 1
*/
#define     UI_FRAME_QUEUED_UP  0x1210       /* UI frame(s) are queued */
#define     I_FRAME_QUEUED_UP   0x1220        /* I frame(s) are queued */
#define     ACK_PENDING         0x1230            /* an ack is pending */
#define     T200_EXPIRY         0x1240       /* timer T200 has expired */
#define     T202_EXPIRY         0x1710       /* timer T202 has expired */
#define     T203_EXPIRY         0x1250       /* timer T203 has expired */

/* === #ifndef VN2 */
#define     T204_EXPIRY         0x1720        /* timer 204 has expired */
/* === #endif */

#define     PERSISTENT_DEACT    0x1170  /* L1 persistently deactivated */
#define     SET_OWN_REC_BUSY    0x1280        /* cannot receive signal */
#define     CLEAR_OWN_REC_BUSY  0x1290            /* clear 'busy flag' */
#define     CNTRL_FIELD_ERR     0x12A0          /* control field error */
#define     INFO_NOT_PERMITTED  0x12B0         /* no info is permitted */
#define     INCORRECT_LENGTH    0x12C0                   /* bad length */
#define     I_FRAME_TOO_LONG    0x12D0          /* Info field too long */
#define     INFO_REQUIRED       0x12E0       /* Info field is required */

/* Frame reject response errors
*/
#define     FRMR_W              0x01            /* control field error */
#define     FRMR_X              0x03/* no info permitted or bad length */
#define     FRMR_Y              0x04            /* info field too long */
#define     FRMR_Z              0x08                    /* invalid N_R */

#ifdef L2_FLOW_CTRL
/* Number of unacknowledged i frames layer 2 can send to layer 3
*/
#define     L3_QUEUE_SIZE       8
#endif   

#if 0		/* === */
#ifndef L2_TEST
/* System Parameters (see Section 5.9/Q.921)
*/
#define     DEFAULT_T200   (UINT32)1000 /* max msec between tran & ack */
#ifdef PRI
#define     DEFAULT_T203   (UINT32)30000   /* max msecs w/o activities */
#define     DEFAULT_K       7              /* max outstanding I frames */
#else
#ifdef BRI_5ESS
#define     DEFAULT_T203   (UINT32)30000   /* max msecs w/o activities */
#define     DEFAULT_K       1              /* max outstanding I frames */
#else
#define     DEFAULT_T203   (UINT32)10000   /* max msecs w/o activities */
#define     DEFAULT_K       1              /* max outstanding I frames */
#endif                                               /* endif BRI_5ESS */
#endif                                                    /* endif PRI */

#else                                                /* ifndef L2_TEST */
/* NOTE: When testing, allow more time for T200 and T203 to allow for
*        extra communication time and/or tracing/logging processing.
*        Also, make K a more interesting value than 1.
*/
#define     DEFAULT_T200   (UINT32)3000 /* max msec between tran & ack */
#ifdef PRI
#define     DEFAULT_T203   (UINT32)30000   /* max msecs w/o activities */
#define     DEFAULT_K       7              /* max outstanding I frames */
#else
#ifdef BRI_5ESS
#define     DEFAULT_T203   (UINT32)30000   /* max msecs w/o activities */
#define     DEFAULT_K       5              /* max outstanding I frames */
#else
#define     DEFAULT_T203   (UINT32)15000   /* max msecs w/o activities */
#define     DEFAULT_K       5              /* max outstanding I frames */
#endif                                               /* endif BRI_5ESS */
#endif                                                    /* endif PRI */

#endif                                               /* ifndef L2_TEST */
#endif	/* === */

extern unsigned long   DEFAULT_T200, DEFAULT_T203, DEFAULT_K;
extern BOOLEAN Network_side;  /* allow PRI to run back-to-back */

#define     DEFAULT_T202    (UINT32)2000 /* msecs between TEI req msgs */
/* NOTE: Specifications define the default values for N200 and N204 in
*  terms of retransmissions whereas they define the default value for
*  N202 in terms of transmissions.  Here, they are all defined in terms
*  of retransmissions.
*/
/* === #ifndef VN2 */
#define     DEFAULT_T204    (UINT32)2000 /* msecs between XID req msgs */
/* === #endif */

#define     DEFAULT_X25_K   3 /* max outstanding I frames for X25 link */
#define     DEFAULT_N201_X  DEFAULT_N201 /* max length of I field (tx) */
#define     DEFAULT_N201_R  DEFAULT_N201 /* max length of I field (rx) */
#define     DEFAULT_N202    2   /* max retransmissions of TEI requests */
#define     DEFAULT_N204    3   /* max retransmissions of XID requests */

#define     LAPD_MOD        128  /* I frame sequence number mod - LAPD */
#define     LAPB_MOD        8    /* I frame sequence number mod - LAPB */

/* Masks
*/
#define     SUPERVISORY     1                     /* supervisory frame */
#define     UNNUMBERED      3                      /* unnumbered frame */

/* General Info
*/
#define     SIGNAL_CHAN_ID  0              /* signaling DSL channel ID */
#define     LAPB_TEI        128                       /* fake LAPB TEI */
#define     LAPB_CES        128                       /* fake LAPB CES */
#define     MAX_CM_ERR      26          /* max connection mngmt errors */

/************************************************************************
*                              M A C R O S
*************************************************************************
*/

/* NOTE: State variables VA and VS are used as indirect indexes into the
*        I frame queue.  State variables wrap at 128 (or 8 for LAPB).
*        The I queue wraps at MAX_L2_QUEUE_LEN.  If MAX_L2_QUEUE_LEN is a
*        power of 2 between 2 and 128, the index into the I queue may be
*        calculated by masking off the top bits of the state variables.
*/
#define     INDEX(V)        ((V) & (MAX_L2_QUEUE_LEN-1)) /* index to Q */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S
*************************************************************************
*/

/* Transmission Queue definition
*
* NOTE:
*       At no time will there be BOTH UI and I frames in the transmit queue.
*       Therefore, the storage area (Queue) is shared; it may be used for
*       either UI or I frames.
*
*       Queue Design:
*
*       UI_Q    VS = next frame to send
*               Next entry = VS + UI_Queue_Len
*               No Retransmission
*               UI_Queue_Len = number of frames in UI queue
*
*       |___|___|___|___|___|___|___|___|___ ... ___|___|___|___|___|
*        0        ^           ^                                  MAX
*                 |           |                                  QUEUE
*                VS          Next entry into queue               LEN
*                 ^        ^
*                 |        |
*                 ----------
*                 UI_Queue_Len = 3
*
*
*       I_Q     VA = last frame acked (VA - 1 = N(S) of last ACKed I frame)
*               VS = next frame to send
*               Next entry = VS + I_Queue_Len
*               Retransmission = VS - VA - 1
*               I_Queue_Len = number of frames in I queue
*
*       |___|___|___|___|___|___|___|___|___ ... ___|___|___|___|___|
*        0        ^       ^           ^                          MAX
*                 |       |           |                          QUEUE
*                VA      VS          Next entry into queue       LEN
*                 ^   ^   ^        ^
*                 |   |   |        |
*                 -----   ----------
*        Retransmission   I_Queue_Len = 3
*             Queue = 2
*/
typedef struct
{
   PKT_ID       Pkt;         /* Pointer to queued packet holding frame */
   BOOLEAN      FreePktOk; /* call LIF_FreePkt() when pkt is disposed? */
}
TX_Q;

#ifdef L2_SREJ
/* Receive Queue definition
*
*  The receive queue (RxQ) is a list of received I frames that are not
*  contiguous and are being held until all are received before passing
*  them up to L3.  Each record in the list has 2 fields.  'Mrej' indicates
*  whether a frame has been received or not and if it has not been received,
*  how many times it has been rejected.  A value of 0 indicates it has
*  been received just fine.  A value > 0 indicates how many times this
*  frame sequence number has been selectively rejected.  'Pkt' indicates
*  a pointer to the packet that is the frame with this sequence number.
*  If the element has a non-zero 'Mrej', the 'Pkt' value will be NULL.
*  If the element has a non-NULL 'Pkt', the 'Mrej' value will be zero.
*
*       Queue Design:
*
*                0-------------------------------------max
*
*                --------------210002000001000----------
*                              |             |
*                              |<--- RxQ --->|
*                              |             |
*                             VR          VR_High
*
*                VR         - sequence number of next expected frame
*                VR_High    - sequence number of last received frame
*
*                Note that if no reject condition is in effect, VR is
*                equal to VR_High+1.
*
*                RxQ        - receive queue (pkts and their mrej values)
*
*                Mrej value - How many times this frame number has been
*                             rejected.  Note that an element in a
*                             queue has either a pkt pointer OR an mrej
*                             value; never both.
*
*                0 = received frame good
*                1 = received frame bad
*               >1 = received frame bad with multiple rejects
*
*/
typedef struct
{
   int    Mrej;
   PKT_ID Pkt;
}
RX_Q;
#endif

/* Layer 2 data link control block
*/
struct DataLinkControlBlock
{
   INT16      L2_Protocol;            /* L2P_DSS1, L2P_X25 or L2P_V120 */
   BYTE       Mod;                            /* protocol modulo value */
   BYTE       State;                                  /* current state */
   BYTE       Sapi;                 /* Service Access Point Identifier */
   BYTE       Tei;                     /* Terminal Endpoint Identifier */
   INT16      DSL_Chan;                              /* DSL channel ID */
   BYTE       Ces;                       /* Connection Endpoint Suffix */
   BYTE       Network;                       /* Network or User module */
   BYTE       K;                      /* max # of outstanding I frames */
   BYTE       N200;                   /* max retransmission of I frame */
   BYTE       RC;                            /* retransmission counter */
   BYTE       VA;                        /* acknowledge state variable */
   BYTE       VS;                               /* send state variable */
   BYTE       VR;                            /* receive state variable */
#ifdef L2_MREJ
   /* Recovery state variable V(M) is used to implement the optional
   *  retransmission of REJ response frames.  It denotes the sequence
   *  number of the last frame received which caused an N(S) sequence
   *  error condition.  It is used to determine if another REJ response
   *  frame should be sent on receipt of an N(S) sequence error while in
   *  the REJ exception condition.  See Appendix I to recommendation Q.921.
   */
   BYTE       VM;                           /* recovery state variable */
#endif
#ifdef L2_SREJ
   BYTE       VR_High;                          /* last frame received */
#endif
   BOOLEAN    Own_Receiver_Busy;            /* we can't receive frames */
   BOOLEAN    Peer_Receiver_Busy;         /* peer can't receive frames */
   BOOLEAN    Rej_Recovery;                        /* reject exception */
   BOOLEAN    Ack_Pending;            /* set to re-enter state machine */
#ifdef L2_FLOW_CTRL
   BOOLEAN    L3_Flow_Ctrl;        /* user flow control feature enable */
   BYTE       L3_RxAck;                     /* user flow control count */
#endif
   BOOLEAN    DL_Release_Request;  /* has link release been requested? */
   BOOLEAN    L3_Initiated;     /* our layer 3 initiated establishment */
   BOOLEAN    Able_to_Establish;         /* FFS able to receive frames */
   INT16      T200_Handle;                /* index to timer T200 entry */
   INT16      T203_Handle;                /* index to timer T203 entry */
/* === #ifndef VN2 */
   INT16      T204_Handle;              /* time between XID param msgs */
/* === #endif */
   INT16      I_Queue_Len;                /* # of I frames in tx queue */
   INT16      UI_Queue_Len;              /* # of UI frames in tx queue */
   TX_Q       TxQ[MAX_L2_QUEUE_LEN];        /* UI and I transmit queue */
#ifdef L2_SREJ
   RX_Q       RxQ[MAX_L2_QUEUE_LEN];                /* I receive queue */
#endif

   /*
   *  The Connection Management fields of the DLCB are provided for
   *  Management Entity use (parameter negotiation and error logging).
   *  (NOTE: The index to CM_Err_Count array = Err code - 'A' (ex. 'C' = 2)
   *  (Assume that the error code is a letter between 'A' and 'Z' inclusive.)
   */
   BYTE       CM_State;                             /* conn mgmt state */
   BYTE       CM_RC;                                    /* retry count */
   BYTE       CM_Err_Count [MAX_CM_ERR];                /* error count */
   INT16      N201_Rcv;                         /* receiver N201 value */
   INT16      N201_Xmit;                     /* transmitter N201 value */
   UINT32     T200;                                      /* T200 value */

   struct     DataLinkControlBlock *Next;          /* addr of next DLE */
};
typedef struct DataLinkControlBlock *DLCB_ID;


/************************************************************************
*                   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/* public data (defined in l2.c)
*/
extern unsigned long DEFAULT_N200;
extern PKT_ID  L2_PktIn;
#ifdef NEVER_REFERENCED
extern PKT_ID  L2_PktOut;
#endif /* NEVER_REFERENCED */
extern INT16   L2_Event;
extern BYTE    L2_Sapi;
extern BYTE    L2_Ces;
#ifdef NEVER_REFERENCED
extern BYTE    L2_Tei;
#endif /* NEVER_REFERENCED */
extern BYTE    L2_DSL_Id;
extern INT16   L2_DSL_Chan;
extern INT16   L2_Protocol;
extern BOOLEAN L2_Command;
extern BYTE    L2_NR;
extern BYTE    L2_NS;
extern BYTE    L2_F;
extern BYTE    L2_P;
#ifdef L2_SREJ
extern BYTE    L2_SREJ_NR;
#endif
#ifdef L2_FLOW_CTRL
extern int     L2_RxAck;
extern int     L2_TxAck;
#endif
extern BOOLEAN L2_NeedMail;
extern BOOLEAN L2_FreePktIn;
extern BOOLEAN L2_FreePktOk;
extern BYTE    L2_MDL_Error;
#ifdef NEVER_REFERENCED
extern BYTE    L2_FRMR_Control1;
extern BYTE    L2_FRMR_Control2;
extern BYTE    L2_FRMR_Command;
#endif /* NEVER_REFERENCED */
extern BYTE    L2_FRMR_Cause;
extern DLCB_ID DLCB;


/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
extern ulong L2_Discards[MAX_DSL];
extern ulong L2D_Task_Counter[MAX_DSL];
extern int CCBFIT_TerminalInit (int, int, int);
extern void me_set_activate_flag (int, int, boolean);

PUBLIC  void        L2_AddTeiCes    (int, BYTE, BYTE, INT16);
PUBLIC  void        L2_AdvanceVA    (boolean);
PUBLIC  void        L2_DiscardQs    (void);
#ifdef DEBUG
PUBLIC  void        L2_DispDLCB     (DLCB_ID);
#endif
PUBLIC  void        L2_Go           (INT16, INT16, INT16, int, BYTE, \
                                    BYTE, INT16, INT16, PKT_ID, BYTE, BOOLEAN);
PUBLIC  void        L2_Init         (void);
PUBLIC  void        L2_Mail         (INT16, INT16);
PUBLIC  void        L2_ProcessEvent (void);
PUBLIC  void        L2_RemoveTeiCes (BYTE, BYTE, INT16);
#ifdef EURO_ACT
PUBLIC  void        L2_ResetLink    (int);
#endif EURO_ACT
PUBLIC  DLCB_ID     L2_SearchDLCB   (int, BYTE, BYTE, INT16);
PUBLIC  void        L2_ShutDown     (void);
PUBLIC  void        show_dlcbs      (int);

#endif

/*eof*/

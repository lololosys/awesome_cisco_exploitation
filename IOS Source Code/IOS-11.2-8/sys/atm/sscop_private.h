/* $Id: sscop_private.h,v 3.2.56.5 1996/09/06 16:40:20 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_private.h,v $
 *----------------------------------------------------------------------
 *
 * S S C O P _ P R I V A T E . H
 *
 * Service Specific Connection Oriented Protocol private header file.
 *
 * November 1993     Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *---------------------------------------------------------------------
 * $Log: sscop_private.h,v $
 * Revision 3.2.56.5  1996/09/06  16:40:20  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.2.56.4  1996/08/05  22:12:49  jwjang
 * CSCdi65057:  print out err message when sscop detects unmatch uni
 * version
 * Branch: California_branch
 *         CSCdi64992, changed malloc() to malloc_nsmae() in atm_arp.c
 *         fix sscop_rcvQ_cnt and sscop_ackQ_cnt, fixed  no timer running
 *         in  sscop outErrorRecovery state
 *
 * Revision 3.2.56.3  1996/07/29  23:38:53  jwjang
 * CSCdi62882:  SSCOP: when vp-tunnel is shutdown the peers SSCOP remains
 * active
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/06/19  00:57:01  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/05/09  14:11:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  06:34:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.1  1996/04/10  23:36:02  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2  1995/11/17  08:47:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/18  14:23:24  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.3  1995/08/08  16:45:38  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/06/28 09:22:33  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:13:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *---------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SSCOP_PRIV_H__
#define __SSCOP_PRIV_H__



/*
 * SSCOP States.
 */
typedef enum _sscop_statetype {
   SSCOP_ST_IDLE,                       /* Idle */
   SSCOP_ST_OUTCONN_PENDING,            /* Outgoing Connection Pending */
   SSCOP_ST_INCONN_PENDING,             /* Incoming Connection Pending */
   SSCOP_ST_DATA_XFER_READY,            /* Data Transfer Ready */
   SSCOP_ST_OUTDISCONN_PENDING,         /* Outgoing Disconnection Pending */
   SSCOP_ST_OUTRESYNC_PENDING,          /* Outgoing Resync Pending */
   SSCOP_ST_INRESYNC_PENDING,           /* Incoming Resync Pending */
   SSCOP_ST_OUTRECOVERY_PENDING,         /* Outgoing Recovery Pending */
   SSCOP_ST_RECOVERYRSP_PENDING,         /* Recovery Response Pending */
   SSCOP_ST_INRECOVERY_PENDING,          /* Incoming Recovery Pending */
   SSCOP_ST_CONRESYNC_PENDING           /* Concurrent Resync Pending */
} sscop_statetype; 


/*
 * SSCOP Message Types.
 */
#define SSCOP_MT_BGN       0x01000000      /* Begin */
#define SSCOP_MT_BGAK      0x02000000      /* Begin Acknowledge */
#define SSCOP_MT_END       0x03000000      /* End */
#define SSCOP_MT_ENDAK     0x04000000      /* End Acknowledge */
#define SSCOP_MT_RS        0x05000000      /* Resynchronization */
#define SSCOP_MT_RSAK      0x06000000      /* Resynchronization Acknowledgment */
#define SSCOP_MT_BGREJ     0x07000000      /* Begin Reject */
#define SSCOP_MT_SD        0x08000000      /* Sequenced Data */
#define SSCOP_MT_SDP       0x09000000      /* Sequenced Data with Poll */
#define SSCOP_MT_POLL      0x0A000000      /* Poll/Status Request */
#define SSCOP_MT_STAT      0x0B000000      /* Solicited Status Response */
#define SSCOP_MT_USTAT     0x0C000000      /* Unsolicited Status Response */
#define SSCOP_MT_UD        0x0D000000      /* Unnumbered Data */
#define SSCOP_MT_MD        0x0E000000      /* Management Data */
#define SSCOP_MT_ER        0x09000000      /* Recovery Command - same as SDP */
#define SSCOP_MT_ERAK      0x0F000000      /* Recovery Ack */



/*
 * Management Error Indication, defined in Q.2110
 */
#define MAA_ERROR_A       1
#define MAA_ERROR_B       2
#define MAA_ERROR_C       3
#define MAA_ERROR_D       4
#define MAA_ERROR_E       5
#define MAA_ERROR_F       6
#define MAA_ERROR_G       7
#define MAA_ERROR_H       8
#define MAA_ERROR_I       9
#define MAA_ERROR_J       10
#define MAA_ERROR_K       11
#define MAA_ERROR_L       12
#define MAA_ERROR_M       13
#define MAA_ERROR_O       14
#define MAA_ERROR_P       15
#define MAA_ERROR_Q       16
#define MAA_ERROR_R       17
#define MAA_ERROR_S       18
#define MAA_ERROR_T       19
#define MAA_ERROR_U       20
#define MAA_ERROR_V       21
#define MAA_ERROR_W       22
#define MAA_ERROR_X       23


/*
 * SSCOP Incoming and Outgoing Events
 */
typedef enum _sscop_events {
   SSCOP_RCV_BGN,                     /* Rcvd Begin */
   SSCOP_RCV_BGAK,                    /* Rcvd Begin Ack */
   SSCOP_RCV_END,                     /* Rcvd End */
   SSCOP_RCV_ENDAK,                   /* Rcvd End Ack */
   SSCOP_RCV_RS,                      /* Rcvd Resynchronization */
   SSCOP_RCV_RSAK,                    /* Rcvd Resynchronization Ack */
   SSCOP_RCV_BGREJ,                   /* Rcvd Begin Reject */
   SSCOP_RCV_SD,                      /* Rcvd Sequenced Data */
   SSCOP_RCV_SDP,                     /* Rcvd Sequenced Data with Poll */
   SSCOP_RCV_POLL,                    /* Rcvd Poll/Status Request */
   SSCOP_RCV_STAT,                    /* Rcvd Solicited Status Response */
   SSCOP_RCV_USTAT,                   /* Rcvd Unsolicited Status Response */
   SSCOP_RCV_UD,                      /* Rcvd Unnumbered Data */
   SSCOP_RCV_MD,                      /* Rcvd Management Data */
   SSCOP_RCV_ER,                       /* Rcvd Recovery Command */
   SSCOP_RCV_ERAK,                     /* Rcvd Recovery Ack */
   SSCOP_TX_BEGIN,                    /* Tx Begin */
   SSCOP_TX_END,                      /* Tx End */
   SSCOP_TX_RESYNC,                   /* Tx End */
   SSCOP_TX_DATA,                     /* Tx Data */
   SSCOP_TX_KEEPALIVE,                /* Tx Keepalive */
   SSCOP_RCV_INVALIDPDU,              /* Rcvd Invalid PDU */
   SSCOP_RCV_TOOSMALL,                /* Rcvd Packet too small */
   SSCOP_RCV_UNKNOWNPDU               /* Rcvd Unknown Pdu */
} sscop_events;


/*
 * SSCOP Configuration Defaults
 */
#define SSCOP_POLLTIMER_DEFAULT             (ONESEC)
#define SSCOP_KEEPALIVETIMER_DEFAULT        (30 * ONESEC)
#define SSCOP_CCTIMER_DEFAULT               (ONESEC)
#define SSCOP_MAXCC_DEFAULT                 10
#define SSCOP_MAXRETRY_CNT_DEFAULT          10
#define SSCOP_SENDWINDOW_DEFAULT            10
#define SSCOP_RCVWINDOW_DEFAULT             10
#define SSCOP_MAX_LIST_ELEMENTS             67
#define SSCOP_IDLE_DEFAULT                  (10 * ONESEC)
#define SSCOP_NORESPONSE_DEFAULT            (30 * ONESEC)
#define SSCOP_KEEPALIVE_V31_DEFAULT         (5  * ONESEC)


/* initialize VR(MR) */
#define SSCOP_INIT_VRMR(v)                  (v = SSCOP_RCVWINDOW_DEFAULT)
#define SSCOP_INC_VTSQ(sq)                  (sq = (sq + 1) & SSCOP_NSQ_MASK)

/*
 * Miscellaneous constants
 */
#define SSCOP_HEADER_START  pak->datagramstart + pak->datagramsize - SSCOP_HEADER_SIZE

#define SSCOP_PADBYTES          4
#define SSCOP_PADBYTE_MASK      0xC0000000
#define SSCOP_PDUTYPE_MASK      0x0F000000
#define SSCOP_SEQNUM_MASK       0x00FFFFFF
#define SSCOP_RESERVED          0x00000000
#define SSCOP_RESULT_OK         TRUE
#define SSCOP_NSQ_MASK          0x000000ff
#define SSCOP_SBIT_MASK         0X10000000
#define SSCOP_SBIT_USER         0
#define SSCOP_SBIT_SCP          0x10000000  /* source bit if from sscop itself */
#define SSCOP_PDU_RETRANSMIT    -1
#define SSCOP_PDU_BAD           -2
#define SSCOP_PDU_NEW           -3


/* Max. number of list elements in a STAT Pdu */
#define SSCOP_MAX_LIST_ELEMENTS    67 

#define SSCOP_STATS(x)   ((x) == 0xFFFF ? (x) = 0 : ++(x))


#define SSCOP_INCR_CNTR(x)  (x = (x + 1) & 0xffffff)


/*
 * S s c o p    S t a t i s t i c s
 */
typedef struct sscopStats_ {
    uint retxCount;            /* number of retransmitted packets */
    uint Ignored;              /* rcvd packets ignored */
    uint NoMemory;             /* no memory for transmitting packets */
    uint Pdureceived;          /* count of all Pdu's received */
    uint Pdutransmitted;       /* count of all Pdu's transmitted */
    uint InBeginPdu;           /* count of incoming Begin Pdu's */
    uint InBeginAckPdu;        /* count of incoming Begin Ack Pdu's */
    uint InEndPdu;             /* count of incoming End Pdu's */
    uint InEndAckPdu;          /* count of incoming End Ack Pdu's */
    uint InResyncPdu;          /* count of incoming Resync Pdu's */
    uint InResyncAckPdu;       /* count of incoming Resync Ack Pdu's */
    uint InBeginRejPdu;        /* count of incoming Begin Rej Pdu's */
    uint InSdPdu;              /* count of incoming Sd Pdu's */
    uint InSdpPdu;             /* count of incoming Sdp Pdu's */
    uint InPollPdu;            /* count of incoming Poll Pdu's */
    uint InStatPdu;            /* count of incoming Stat Pdu's */
    uint InUstatPdu;           /* count of incoming Ustat Pdu's */
    uint InUdPdu;              /* count of incoming Ud Pdu's */
    uint InMdPdu;              /* count of incoming Md Pdu's */
    uint InUnknownPdu;         /* count of incoming Unknown Pdu's */
    uint InErrorRecPdu;        /* count of incoming Error Recovery Pdu's */
    uint InErrorRecAckPdu;     /* count of incoming Error Recovery Ack Pdu's */
    uint OutBeginPdu;          /* count of outgoing Begin Pdu's */
    uint OutBeginAckPdu;       /* count of outgoing Begin Ack Pdu's */
    uint OutEndPdu;            /* count of outgoing End Pdu's */
    uint OutEndAckPdu;         /* count of outgoing End Ack Pdu's */
    uint OutResyncPdu;         /* count of outgoing Resync Pdu's */
    uint OutResyncAckPdu;      /* count of outgoing Resync Ack Pdu's */
    uint OutBeginRejPdu;       /* count of outgoing Begin Rej Pdu's */
    uint OutSdPdu;             /* count of outgoing Sd Pdu's */
    uint OutSdpPdu;            /* count of outgoing Sdp Pdu's */
    uint OutPollPdu;           /* count of outgoing Poll Pdu's */
    uint OutStatPdu;           /* count of outgoing Stat Pdu's */
    uint OutUstatPdu;          /* count of outgoing Ustat Pdu's */
    uint OutUdPdu;             /* count of outgoing Ud Pdu's */
    uint OutMdPdu;             /* count of outgoing Md Pdu's */
    uint OutErrorRecPdu;       /* count of outgoing Error Recovery Pdu's */
    uint OutErrorRecAckPdu;    /* count of outgoing Error Recovery Ack Pdu's */
    uint LackOfCredit;         /* count of lack of credits */
} sscopStats;


/*
 * S s c o p   M a i n   D a t a   S t r u c t u r e.
 *
 * One Sscop control block per ATM interface.
 */
struct sscopType_ {
    hwidbtype *idb;                 /* ATM port identifier */
    sscop_statetype state;          /* current Sscop state */
    paktype *inbuf;                   /* current input packet */
    sscop_events event;             /* current event */
    ushort flags;		    /* state of the ctb, e.g. in use? */
    int index;
    sys_timestamp last_input;       /* time the last packet arrived */ 
    sys_timestamp last_output;      /* time the last packet sent */
    /* queues maintained by sscop */
    queuetype txQ;                  /* queues packets from Q.93B */
    paktype *ackQ;                  /* transmitted Sd's awaiting ack's */
    paktype *rcvQ;                  /* queues out of sequence rcvd Sd's */
    int sendWindow;
    int rcvWindow;
    int maxStat;
    int discReason;
    boolean txflag;
    ulong nuu;
    int maxPd;
    boolean q93bResetFlag;
    /* state variables at the transmitter side */
    ulong vms;                      /* max send state variable */
    ulong vs;                       /* current send state variable */
    ulong vps;                      /* current poll send state variable */
    ulong vta;                      /* current acknowledge state variable */
    ulong vtPa;                     /* current poll acknowledge state variable */
    ulong vtPd;                     /* poll data state variable */
    ulong vtCc;                     /* connection control state variable */
    ulong vtSq;                     /* connection seq val. sscop pdu param */
    /* state variables at the receiver side */
    ulong vr;                       /* next expected Pdu in Rcv sequence */
    ulong vrh;                      /* highest expected Rcv state variable */
    ulong vmr;                      /* max acceptable Rcv state variable */
    ulong vrSq;                     /* rx coneection sequence state variable */
    ulong incomingPs;               /* incoming ps = n(ps) of tx Stat pdu */
    /* timer variables */
    ushort retryCount;              /* retry count bumped on timer expiry */
    ushort maxRetryCount;           /* max retry count */
    mgd_timer timeoutValue;         /* sscop timer */
    ushort ccTimerValue;            /* connection control timeout */
    ushort pollTimerValue;          /* poll timeout */
    int maxCC;
    mgd_timer keepalive_timer;      /* sscop keepalive timer */
    ushort keepaliveTimerValue;     /* keepalive timeout */ 
    ushort idleTimerValue;          /* idle timer value - UNI 3.1 */
    ushort noResponseTimerValue;    /* Timer_NO-RESPONSE value */
    ushort uni_version;             /* get from atm_db_t during hard reset */
    ushort credit;                  /* 0 when tx window closed, 1 otherwise */
    ulong  poll_vps;                /* vps of poll pdu when no tx window */
    mgd_timer timerIdle;            /* Timer_IDLE */
    mgd_timer timerNoResponse;      /* Timer_NO-RESPONSE */
    mgd_timer timerPoll;
    mgd_timer timerKeepalive_v31;   /* used by UNI 3.1 code only */
    mgd_timer t309;                 /* signaling timer */
    sscopStats stats;               /* statistics */
    ushort begin_rej_count;         /* counters of receiving begin rej pdu */
    semaphore  sscopInUse;          /* semaphore to lock/unlock sscop block */
    tinybool quickPoll;             /* enable/disable Poll at interrupt */
};

#define SSCOP_CTB_NOT_INUSE	0   /* sscop ctb is not being used */
#define SSCOP_CTB_INUSE 	1   /* sscop ctb has already being used */
#define SSCOP_BEGINREJ_MAX      10   /* thershold of receiving begin rej pdu */



/*
 *  P d u       P a c k e t       F o r m a t s 
 */

/*
 * NOTE - All SSCOP headers are long word aligned - no need to worry about
 * padding bytes being introduced by certain compilers.
 */ 

/*
 * Begin Pdu
 */
typedef struct sscop_beginPdu_ {
  ulong nuu_nsq;           /* nuu-3.0, nsq-3.1 */
  ulong type_window;       /* msb - pdu type, 3 lsb's - nmr */
} sscop_beginPdu;



/*
 * Begin Ack Pdu
 */
typedef struct sscop_beginAckPdu_ {
   ulong nuu;               /* user to user info */
   ulong type_window;       /* msb - pdu type, 3 lsb's - nmr */
} sscop_beginAckPdu;


/*
 * Begin Rej Pdu
 */
typedef struct sscop_beginRejPdu_ {
   ulong nuu;               /* user to user info */
   ulong type;              /* msb - pdu type, 3 lsb's - reserved */
} sscop_beginRejPdu;


/*
 * Sequenced Data Pdu
 */
typedef struct sscop_sdPdu_ {
   ulong type_ns;       /* msb - pad length, pdu type, 3 lsb's - n(s) */
} sscop_sdPdu;


/*
 * Sequenced Data with Poll Pdu
 */
typedef struct sscop_sdpPdu_ {
   ulong ps;           /* msb - reserved, 3 lsb's - n(ps) */
   ulong type_ns;      /* msb - pad length, pdu type, 3 lsb's - n(s) */
} sscop_sdpPdu;


/*
 * Poll Pdu
 */
typedef struct sscop_pollPdu_ {
   ulong ps;           /* msb - reserved, 3 lsb's - n(ps) */
   ulong type_ns;      /* msb - pdu type, 3 lsb's - n(s) */
} sscop_pollPdu;


/*
 * Resync Pdu
 */
typedef struct sscop_resyncPdu_ {
  ulong nuu_nsq;
  ulong type;              /* msb - pdu type, 3 lsb's - reserved */
} sscop_resyncPdu;


/*
 * Resync Acknowledgment Pdu
 */
typedef struct sscop_resynsAckPdu_ {
   ulong type;       /* msb - pdu type, 3 lsb's - reserved */
} sscop_resyncAckPdu;


typedef struct sscop_resyncAck31Pdu_ {
  ulong res;
  ulong type;       /* msb - pdu type, 3 lsb's - reserved */
} sscop_resyncAck31Pdu;



/*
 * Stat Pdu 
 */
typedef struct sscop_statPdu_ {
   ulong ps;          /* n(ps) */
   ulong nmr;         /* n(mr) */
   ulong type_nr;     /* msb - pdu type, 3 lsb - n(r) */
} sscop_statPdu;


/*
 * Unsolicited Stat Pdu
 */
typedef struct sscop_ustatPdu_ {
   ulong list1;          /* list element 1 - a Sd pdu n(s) */
   ulong list2;          /* list element 2 -       "       */
   ulong nmr;            /* max acceptable receive sequence number */
   ulong type_nr;        /* msb - pdu type, 3 lsb's - n(r) */
} sscop_ustatPdu; 


/*
 * End Pdu
 */
typedef struct sscop_endPdu_ {
   ulong nuu;               /* user to user info */
   ulong type;              /* msb - pdu type, 3 lsb's - reserved */
} sscop_endPdu;


/*
 * End Acknowledgment Pdu
 */
typedef struct sscop_endAckPdu_ {
   ulong type;       /* msb - pdu type, 3 lsb's - reserved */
} sscop_endAckPdu;


typedef struct sscop_endAck31Pdu_ {  /* UNI 3.1 */
   ulong res;      /* reserved 4 bytes */
   ulong type;       /* msb - pdu type, 3 lsb's - reserved */
} sscop_endAck31Pdu;

/*
 * Error Recovery Pdu
 *
 * Only used by UNI 3.1 signaling
 */
typedef struct sscop_errRecPdu_ {
   ulong nsq;
   ulong type_nmr;
} sscop_errRecPdu;


/*
 * Error Recovery Ack Pdu
 *
 * Only used by UNI 3.1 signaling
 */
typedef struct sscop_errRecAckPdu_ {
   ulong res;
   ulong type_nmr;
} sscop_errRecAckPdu;



/*
 * Management Pdu
 */
typedef struct sscop_mgmtPdu_ {
   ulong type;       /* msb - pad lngth & pdu type, 3 lsb's - reserved */
} sscop_mgmtPdu;


/*
 * Unassured (Unit) Data Pdu
 */
typedef struct sscop_udPdu_ {
   ulong type;       /* msb - pad lngth & pdu type, 3 lsb's - reserved */
} sscop_udPdu;


/*
 * list - used by STAT pdu
 */
typedef struct sscop_list_ {
   ulong ns;       /* msb - pad, 3 lsb's - n(s) */
} sscop_list;
  



/*
 * Sscop Timer Types
 */
enum SSCOP_TIMERS {
  SSCOP_KEEPALIVE_TIMER,
  SSCOP_NO_RESPONSE_TIMER,
  SSCOP_IDLE_TIMER,
  SSCOP_POLL_TIMER,
  SSCOP_KEEPALIVE_TIMER_V31,
  SSCOP_ATMSIG_T309,   /* signaling timer, maintain by sscop */
  SSCOP_RETRY_TIMER
};

/*
 * E x t e r n s
 */

extern boolean sscopEnabled;

extern boolean sscop_debugEvent;
extern boolean sscop_debugPacket;
extern boolean sscop_debugError;
extern boolean sscop_debugSuspend;

extern void sscop_init(subsystype *);
extern void sscop_interfaceReset(hwidbtype *);
extern void sscop_initSscopCb(sscopType *);
extern void saal_enqueue(paktype *, vc_info_t *);
extern process sscop_inputProcess(void);
extern void sscop_input(paktype *);
extern process sscop_transmitProcess(void);
extern process sscop_outputProcess(void);
extern void sscop_output(sscopType *);
extern sscop_events sscop_findPduType(paktype *, sscopType *);
extern void sscop_processSdpPdu(sscopType *, boolean);
extern void sscop_processPollPdu(sscopType *);
extern boolean sscop_isSdInAckQ(sscopType *, ulong);
extern boolean sscop_isSdInRcvQ(sscopType *, ulong);
extern boolean sscop_isRtsSet(sscopType *, ulong);
extern long sscop_findPsInSd(sscopType *, ulong);
extern void sscop_queueSdOnTxQ(void);
extern void sscop_queueSdOnAckQ(sscopType *, paktype *);
extern void sscop_queueSdOnRcvQ (sscopType *, paktype *);
extern paktype *sscop_removeSdFromAckQ(sscopType *, ulong);
extern paktype *sscop_removeSdFromRcvQ (sscopType *, ulong);
extern void sscop_removeAllFromAckQ(sscopType *sscop);
extern void sscop_removeAllFromRcvQ(sscopType *sscop);
extern void sscop_sendSdPdu(sscopType *);
extern void sscop_resendSdPdu(sscopType *, ulong);
extern void sscop_clearStats(sscopType *sscop);
extern boolean sscop_transmitBeginPdu(sscopType *);
extern boolean sscop_transmitBeginAckPdu(sscopType *);
extern boolean sscop_transmitBeginRejPdu(sscopType *);
extern boolean sscop_transmitEndPdu(sscopType *);
extern boolean sscop_transmitEndAckPdu(sscopType *);
extern boolean sscop_transmitResynsPdu(sscopType *);
extern boolean sscop_transmitResyncAckPdu(sscopType *);
extern boolean sscop_transmitUstatPdu(sscopType *, ulong);
extern boolean sscop_transmitStatPdu(sscopType *);
extern boolean sscop_transmitPollPdu(sscopType *);
extern boolean sscop_transmitResyncPdu(sscopType *); 
extern int sscop_createSolicitedList(sscopType *, paktype *);
extern void atmSig_sendToAal(sscopType *, paktype *);
extern void sscop_resetReceiver(sscopType *);
extern void sscop_resetTransmitter(sscopType *);
extern int sscop_processBeginPdu(sscopType *);
extern boolean sscop_processBeginAckPdu(sscopType *);
extern boolean sscop_processBeginRejPdu(sscopType *);
extern boolean sscop_processEndPdu(sscopType *);
extern boolean sscop_processEndAckPdu(sscopType *);
extern void sscop_processUstatPdu(sscopType *);
extern void sscop_processStatPdu(sscopType *);
extern int sscop_processResyncPdu(sscopType *);
extern int sscop_processResyncAckPdu(sscopType *);
extern void sscop_stateTable (sscopType *);
extern void sscop_idleState(sscopType *);
extern void sscop_outConnPendingState(sscopType *);
extern void sscop_dataTransferState(sscopType *);
extern void sscop_outDisconnPendingState(sscopType *);
extern void sscop_outResyncPendingState(sscopType *);
extern void sscop_inResyncPendingState(sscopType *);
extern void sscop_concurrentResyncPendingState(sscopType *);
extern char *sscop_printState(sscop_statetype);
extern char *sscop_printPduType(ulong, ulong);
extern process sscop_timerProcess(void);
extern void sscop_tickTock(sscopType *);
extern void sscop_setNextWakeupTime(void);
extern void sscop_startTimer(sscopType *, ushort);
extern void sscop_stopTimer(sscopType *);
extern void sscop_interface_commands(parseinfo *);
extern void sscop_show_command(parseinfo *);
extern void sscop_showCommand(sscopType *);
extern void sscop_resetStatCounters(void);
extern void sscop_enableSscop(void);
extern void sscop_disableSscop(void);
extern void sscop_debug_init(void);
extern void sscop_parser_init(void);
extern void sscop_incVms(int vms, sscopType *);

/* sscop_ui.c */
extern char *sscop_printMaaErrorInd(int);

/* sscop.c */
extern void sscop_initStatistics(sscopType *);

/* sscop_state.c */
extern void sscop_outRecoveryPendingState(sscopType *);
extern void sscop_recoveryResponsePendingState(sscopType *);
extern void sscop_inRecoveryPendingState(sscopType *);


/* sscop_v31.c */
extern inline void sscop_startTimerPoll(sscopType *);
extern inline void sscop_startTimerIdle(sscopType *);
extern inline void sscop_startTimerKeepalive(sscopType *);
extern inline boolean sscop_timer309Running(sscopType *);
extern inline void sscop_startTimer309(sscopType *);
extern inline void sscop_stopTimer309(sscopType *);
extern inline void sscop_startTimerNoResponse(sscopType *);
extern inline void sscop_stopTimerPoll(sscopType *);
extern inline void sscop_stopTimerIdle(sscopType *);
extern inline void sscop_stopTimerKeepalive(sscopType *);
extern inline void sscop_stopTimerNoResponse(sscopType *);
extern inline boolean sscop_timerPollRunning(sscopType *);
extern inline boolean sscop_timerIdleRunning(sscopType *);
extern inline boolean sscop_timerKeepaliveRunning(sscopType *);
extern inline boolean sscop_timerNoResponseRunning(sscopType *);
extern inline int sscop_findUniVersion(sscopType *);
extern inline int sscop_getUniVersion(sscopType *);
extern inline boolean sscop_isV31(sscopType *);
extern void sscop_enterDataXferReady(sscopType *);
extern boolean sscop_transmitErPdu(sscopType *);
extern boolean sscop_transmitErAckPdu(sscopType *);
extern void sscop_aaReleaseIndication(sscopType *);
extern void sscop_sequenceError(sscopType *, int);
extern void sscop_maaErrorIndication(sscop_statetype, int, int);
extern void sscop_setXferReadyTimers(sscopType *);
extern void sscop_releaseBuffers(sscopType *);
extern void sscop_prepareRecovery(sscopType *);
extern void sscop_clearTransmitter(sscopType *);
extern void sscop_deliverData(sscopType *);
extern void sscop_initStateVars(sscopType *);
extern boolean sscop_detectRetransmission(sscopType *);
extern void sscop_setPollTimer(sscopType *);
extern void sscop_resetDataXferTimers(sscopType *);
extern void sscop_setDataXferTimers(sscopType *);
extern boolean sscop_processErAckPdu(sscopType *);
extern boolean sscop_processErPdu(sscopType *);
extern void sscop_requestConnection(sscopType *);
extern void sscop_makeConnInConnPending(sscopType *);
extern void sscop_ilmiCallback(void *);
extern boolean sscop_registerIlmi(void);
extern tinybool sscop_handle_poll_pdu (paktype *, vc_info_t *);

#endif __SSCOP_PRIV_H__

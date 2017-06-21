/* $Id: qllc.h,v 3.2.62.4 1996/09/15 02:56:04 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc.h,v $
 *------------------------------------------------------------------
 * qllc.h -- QLLC header file
 *
 * November 1992,   Percy P. Khabardar
 * May/June 1993,   Phil Morton
 * September 1993,  Ramin Naderi
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: qllc.h,v $
 * Revision 3.2.62.4  1996/09/15  02:56:04  rbatz
 * CSCdi60999:  QLLC: PS/2 using Link Station of Negotiable cannot connect.
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/21  14:52:42  rbatz
 * CSCdi66340:  QLLC over PVCs displays infinite error messages.
 *              - Do not try to requue same qllc control block.
 *              - Do not respond to a Reset with a Reset
 *                because this can result in a protocol loop.
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/21  13:15:19  rbatz
 * CSCdi62155:  LNX timers not working correctly. Result is TEST
 *              and NULL XID frames sent in the middle of
 *              XID(3) Negotiation.
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/17  12:13:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/06  01:06:04  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  21:58:59  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  18:55:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/08  16:49:26  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/19 21:06:33  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.2  1995/06/28  09:31:23  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:01:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QLLC_H__
#define __QLLC_H__

STRUCTURE_REF(QPSapT);
STRUCTURE_REF(QllcEventT);
STRUCTURE_REF(qllcmap_t);

/* mapping from X.25 virtual mac addresses to lnx types,
 * to avoid amibuous defintions with clsi structures
 */

struct qllcmap_t_ {
    qllcmap_t	*next;
    uchar	vmac[IEEEBYTES];
    x25map	*map;
    lnxtype	*lnx;

    /* additional fields for pvcs */
    qllctype	*qllc;
    QPSapT	*qpsap;
} ;


#define QLLC_CMD 0xFF	/* for command, anything else is response */
#define QLLC_RSP 0x00	/* Use some well-defined value for a response */

/* 
 * The command formats 
 */
#define QSM      0X93
#define QDISC    0X53
#define QUA      0X73
#define QDM      0X1F
#define QFRMR    0X97
#define QXID     0XBF
#define QTEST    0XF3
#define QRR      0XF1
#define QRD      0x53

/* packet ouput display buffer */
#define QLLC_MAX_PAKTYPE 30

/* artificial values */

#define DATA_PACKET 0x22
#define TOO_SHORT   0x24

/* 
  The states for original QLLC state machine
 */

typedef enum qllc_state_t_ {
    QST_ADM = 0,
    QST_SETUP,
    QST_RESET,
    QST_DISC,
    QST_NORMAL
} qllc_state_t;


/* 
 * Incoming data on the X.25 interface
 */

enum {
    QRAW_TOO_LOW = 0,
    REC_QXID_CMD,
    REC_QTEST_CMD,
    REC_QXID_RSP,
    REC_QTEST_RSP,
 
    REC_QSM_CMD,
    REC_QDISC_CMD,
    REC_QRD_RSP,
    REC_QUA_RSP,
    REC_QDM_RSP,
 
    REC_QFRMR_RSP,
 
    REC_QRR_CMD,
 
    REC_QIFRAME,
 
    REC_FRMR_EVENT,
    REC_TOO_QSMALL,
    QRAW_TOO_HIGH
    };

/* 
 * Definitions of status values 
 */
#define QLS_SUCCESS            1
#define QLS_DISCONNECT         2 
#define QLS_FAILED             3 
#define QLS_WAITING	       4 
#define QLS_INTDOWN            5 
#define QLS_TIMEOUT            6 
#define QLS_DETACHING	       7

/* 
 * Timer and retry limits 
 */
#define QT1_DEFAULT     (8 * ONESEC)
#define QRETRY_COUNT        6

typedef struct qllc_frmr_t_ qllc_frmr_t;

typedef struct QllcStat_ {
                    ulong   qllc_xidin;
                    ulong   qllc_xidout;
                    ulong   qllc_testin;
                    ulong   qllc_testout;
                    ulong   qllc_quenchoff;
                    ulong   qllc_quenchon;
                    ulong   qllc_inpaks;
                    ulong   qllc_outpaks;
                    ulong   qllc_inbytes;
                    ulong   qllc_outbytes;
                    ulong   qllc_numrcvqsms;
                    ulong   qllc_numsndqsms;
                    ulong   qllc_numrcvdiscs;
                    ulong   qllc_numsnddiscs;
                    ulong   qllc_numrcvdms;
                    ulong   qllc_numsnddms;
                    ulong   qllc_numrcvfrmrs;
                    ulong   qllc_numsndfrmrs;
                    ulong   qllc_numdrops;
                    ulong   qllc_numerrs;
} QllcStat;
 
/* Qllc states
 */

typedef enum QLStateT_ {
    QLClosed = 1,	
    QLLocalOpening,	/* Received a Connect.Request from CLSI		*/
    QLRemoteOpening,	/* Received a QSM from X.25			*/
    QLLocalClosing,	/* Received a Disconnect.Request from CLSI	*/
    QLRemoteClosing,	/* Remote Station sent QDISC			*/
    QLOpened,	/* QSM-QUA exchange has completed.			*/
    QLEnd,		/* Marker */
    QLNumStates = QLEnd-1	/* Count the number of QLLC CLSI FSM States */
} QLStateT;

 
typedef enum QX25StateT_ {
    QX25_P1 = 1,   /* No Virtual Circuit exists.			*/
    QX25_STARTUP,  /* Only for PVCs.  Used to kickstart CLS to start connection */
    QX25_P2_D2,	   /* Call/Reset Request placed, awaiting response	*/
    QX25_P3_D3,    /* Incoming Call/Reset Indication received, 
		    * await ReqOpenStn Request
		    */
    QX25_P4_D1,	   /* Virtual Circuit is up, available for data transport. */
    QX25_P6_D2,	  /* Clearing/Resetting Virtual Circuit in progress	       */
    QX25_P7_D3,	  /* Clearing/Resetting Virtual Circuit initiated from network */
    QX25_End,		/* Marker */
    QX25NumStates = QX25_End-1	/* Count the number of QLLC X25 States	     */
} QX25StateT;

/* QLLC EVENTS */

#define MAX_QLLC_EVENTS	3	/* If this isn't enough it ought to be!! */

#define FSM_NO_EVENT		-1

struct QllcEventT_ {
    QllcEventT	*fNextEvent;
    int		fEvent;
    paktype*	fPacket;
};


/* 
 * QLLC control block 
 */
struct qllctype_ {
    avl_node_type 	avl_node;	/* for holding in a Sap */
    qllctype 		*qllc_qnext;	/* next in any queue of qllc types */
    lnxtype 		*lnx;
    queuetype       	qllc_holdQ;	/* hold if output to X.25 is jammed */
    queuetype       	 qllc_lnxQ;	/* hold if output to lnx is jammed */
    idbtype  		*idb;		/* pointer to idb */
    boolean  		qllc_ispvc;	/* for PVC connections */
    boolean     	qllc_net_busy;	/* received quench on from lnx */
    boolean     	qllc_x25_busy;	/* true when quench-on to be sent */
    boolean     	primaryRole;	/* primaryRole == QSM sent */
    queuetype		qllc_reassmQ;	/* handle no packet at process level */


    /* manage XID command/response protocol
     * flag set means ROUTER received Command from net,
     * should send Response next time
     * only used in XID pass-through mode
     * 	     SET in qllc_type1event_lnx  when QXID Command received
     * 	     TEST and CLEAR in qllc_fwd_xid when QXID response pkt built
     */

    boolean		qllc_xid_rsp_pending;
    boolean             vc_kill_flag;

    boolean             xid32_from_remote;
    paktype             *saved_xid3pak;

    int      		linktype;	/* linktype for connection */
    int      		open_block;	/* zero if blocking else result */
    int      		adm_reason;	/* reason for going back to ADM  */
    boolean  		blocking;	/* true if non blocking open */
    ushort   		status_flags;

    lcitype 		*lci;		/* x.25 logical channel identifier */

    sdlc_data_block 	*sdlcp;
    hwidbtype		*sdlc_idb;	/* pointer to idb */
   
    /*MIBO*/
    QllcStat 		qllcStat;

    qllc_state_t 	qllc_state;
    boolean             out_of_order_pvc;

    /* current input buffer */
    paktype 		*inbuf;

    uchar 		retry_count;
    uchar 		event;		/* updated by stateswitch function */

    uchar s_flag;       /* mechanism to remember clashing QSM */

    /* lnx variable */
    boolean 		lnx_running;

    /* the values constructed when a frame reject is to be transmitted */
    qllc_frmr_t		*qllc_frmr;
    ushort 		n2;

	/* CLSI Fields */
    mgd_timer	fTimer;			/* running timer */
    int		fRetries;		/* current value */
    QTimersT 	fTimers;		/* configured value */
    void	*fUCepId;
    CepT	fCep;			/* Common abstract cep Type */
    QPSapT	*fQSap;
    ulong	fQCepFlags;
    uchar	fPartner[IEEEBYTES];
    uchar	fVmac[IEEEBYTES];
    int         fVmacIndex;
    uchar 	fRemote[ADDRLEN_X121];
    queuetype	fFreeEventQ;
    queuetype	fQLeventQ;
    queuetype	fQXeventQ;
    QllcEventT	fEvents[MAX_QLLC_EVENTS];
    ulong	fProxyXid;
    uchar	fDsap;
    uchar	fSsap;
    ulong	fX25Timeout;
    ulong	fCLSTimeout;
    ushort	fClsRetries;
    paktype	*fPacket;
    ushort	fQMaxBtu;
    semaphore	fQLFsmRunning;
    semaphore	fQXFsmRunning;
    boolean	fQClsXidRspPending;
    QX25StateT	fX25State;
    QLStateT	fLState;
    boolean	fQuench;
    boolean     fVmacReuseOk;
};

/* QCepListT
 *
 * Aggregation of qllctype in a Sap 
 *
 * For PVCs we use an array of pointers to the qllc types
 * For SVCs we use an AVL tree
 */
 
struct QCepListT_ {
    union {
	qllctype *fQCepSVCRoot;
	qllctype *fQCepPVCs[1];
    } fList;
};


struct qllc_frmr_t_ {
    uchar	q_ctl;
    uchar	q_cmdrsp;	/* would contain P(R) and P(S) it could	*/
    uchar	q_WXYZ;
} ;

/* bits for FRMR */

#define Q_RSP		0x10

#define Q_W		0x80	/* Control field is not recognized	*/
#define Q_X		0x40	/* I field not allowed - must set W too	*/
#define Q_WX		(Q_W | Q_X)
#define Q_Y		0x20	/* I frame - data is too long		*/
#define Q_Z		0x10	/* not used				*/

#define QFRMR_SIZE	3	/* size of QFRMR I field		*/

#ifdef NOBODY_READS_ME
extern int qllc_stats_ignored;          /* ignored frames */
#endif
extern queuetype qllc_cleanupQ;

extern boolean qllc_test_debug;
extern boolean qllc_state_debug;
extern boolean qllc_event_debug;
extern boolean qllc_packet_debug;
extern boolean qllc_timer_debug;
extern boolean qllc_x25_debug;

/* QLLC packet header */
typedef struct qllc_header_ {
   uchar addr;
   uchar cntrl;
} qllc_header;

/* don't run the risk of a structure padding out to a long word */
#define QLLC_HDRSIZE  2	

/*
 * Events for the scheduler.
 */
enum {
    QLLC_CEPQ_EVENT  = 1,
    QLLC_CLEANUPQ_EVENT,
    QLLC_DATAQ_EVENT,
    QLLC_CALLQ_EVENT,
};

/*
 * External References 
 */

/*
 * qllc.c
 */ 

extern qllctype *qllc_findfree(void);
extern void qllc_freeup(qllctype *qllc);
extern paktype * qllc_get_stored_xid3(qllctype *qllc);
extern forktype qllc_input_proc(void);
extern void qllc_input(paktype *pak);
extern uchar qllc_extract_ctl(paktype *pak);
extern void qllc_init(void);
extern boolean qllc_cleanupBLOCK(void);
extern void qllc_kill_lci(qllctype *qllc);
extern void qllc_periodic(qllctype *qllc);
extern void qllc_lci_notify(lcitype *lci, 
			    uchar event, uchar cause, uchar diag);
extern void qllc_clean_qllc_inputQ(lcitype *lci);
extern qllctype *qllc_setup(lcitype *lci);
extern void qllc_quench(qllctype *qllc, boolean quench_on);
extern qllctype *qllc_by_dte(uchar *macaddr);
extern boolean qllc_validate(qllctype *qllc);
extern qllctype *qllc_open(uchar *vmacaddr, idbtype *idb, boolean blocking);
extern void qllc_contact_accept(qllctype *qllc);
extern void qllc_contact_reject(qllctype *qllc);
extern boolean qllc_close(qllctype *qllc);
extern void qllc_detach(qllctype *qllc);
extern boolean qllc_fwd_xid(qllctype *qllc, paktype *pak);
extern void qllc_send(void *cookie, paktype *pak);
extern boolean qllc_psend(qllctype *qllc, paktype *pak);
extern void qllc_start(void);
extern void qllc_ind_connect(qllctype *qllc);
extern void qllc_ind_connect_snx(qllctype *qllc);
extern void qllc_ind_connect_lnx(qllctype *qllc);
extern qllctype *qllc_open_upstream_snx(uchar *vmacaddr, idbtype *idb,
 				 boolean blocking, sdlc_data_block *sdbp,
				 uchar *pw);
extern void qllc_fix_sdlc(qllctype *qllc);
extern qllctype *qllc_open_upstream_lnx(uchar *vmacaddr, idbtype *idb,
 				 boolean blocking, uchar *pw);
 
/* qllc map methods */

extern qllcmap_t *qllcmap_add(uchar *vmac, x25map *map);
extern boolean qllcmap_remove(uchar *vmac);
extern qllcmap_t *qllcmap_lookup(uchar *vmac);

/*
 * qllstate.c
 */ 

extern void qllc_setstate(qllctype *qllc, qllc_state_t newstate);
extern void qllc_stateswitch(qllctype *qllc, int new_qevent);
extern void qadmstate(qllctype *qllc);
extern void qsetupstate(qllctype *qllc);
extern void qind_concnf(qllctype *qllc, int status);
extern void qresetstate(qllctype *qllc);
extern void qd_connstate(qllctype *qllc);
extern void qnormalstate(qllctype *qllc);
extern void qindinfo(qllctype *qllc);
extern void qllc_type1event(qllctype *qllc, int newevent);
extern void qllc_type1event_snx(qllctype *qllc, int newevent);
extern void qllc_type1event_lnx(qllctype *qllc, int newevent);
extern void qllc_sendcontrol(qllctype *qllc, int ctype, boolean cmd);
extern void qllc_sendQFRMR(qllctype *qllc);
extern void qllc_goto_adm(qllctype *qllc, int reason, boolean kill_flag);
extern void qllc_sendRR(qllctype *qllc);
extern char *qllc_prstate(uchar state);
extern char *qllc_inevent(int event);
extern char *qllc_prstatus(int status);

void QPvcAddUnclaimed(qllctype *qllc);
void QPvcRemoveUnclaimed(qllctype *qllc);

extern void qllc_ui_init(void);
extern char *qllc_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_qllc_addr(parseinfo *, char *, int *, void *,
		      int, uint, idbtype *);
extern void QllcConfigDlsw(parseinfo *csb, idbtype *idb);
extern void QllcNvWriteDlsw(parseinfo *csb);

#endif __QLLC_H__

/* $Id: qllstate.c,v 3.6.12.9 1996/09/15 02:56:09 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllstate.c,v $
 *------------------------------------------------------------------
 * qllstate.c -- QLLC event/state machine
 *
 * November  1992,   Percy P. Khabardar
 * September 1993    Phil Morton
 * September 1993    Ramin Naderi
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllstate.c,v $
 * Revision 3.6.12.9  1996/09/15  02:56:09  rbatz
 * CSCdi60999:  QLLC: PS/2 using Link Station of Negotiable cannot connect.
 * Branch: California_branch
 *
 * Revision 3.6.12.8  1996/08/21  14:52:54  rbatz
 * CSCdi66340:  QLLC over PVCs displays infinite error messages.
 *              - Do not try to requue same qllc control block.
 *              - Do not respond to a Reset with a Reset
 *                because this can result in a protocol loop.
 * Branch: California_branch
 *
 * Revision 3.6.12.7  1996/08/21  13:15:25  rbatz
 * CSCdi62155:  LNX timers not working correctly. Result is TEST
 *              and NULL XID frames sent in the middle of
 *              XID(3) Negotiation.
 * Branch: California_branch
 *
 * Revision 3.6.12.6  1996/08/08  19:27:59  rbatz
 * CSCdi64333:  Serial input queue gets wedged because QLLC was
 *              not freeing QSM, QDISC, and QUA packets, plus
 *              clear_if_input() was not called.
 * Branch: California_branch
 *
 * Revision 3.6.12.5  1996/08/03  23:37:32  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.6.12.4  1996/07/23  13:29:12  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.12.3  1996/07/19  22:46:42  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.6.12.2  1996/05/17  12:13:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.3  1996/04/26  07:58:27  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.2.2  1996/04/11  14:38:23  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/04/03  21:59:04  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.12.1  1996/03/18  22:10:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  07:38:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/07  10:50:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  18:48:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/17  16:47:54  rbatz
 * CSCdi44435:  Outbound connection from FEP to QLLC device fails.
 *              Respond to XID Commands with XID Responses.
 *
 * Revision 3.5  1996/02/01  06:10:21  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/23  00:55:02  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.3  1995/12/14  07:50:29  lpereira
 * CSCdi45464:  appn stop while qllc link retrying crashes router
 * Handle the case where a NULL lci is returned by x25_place_call()
 * when the interface is down
 *
 * Revision 3.2  1995/11/17  18:55:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/19  08:10:36  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.6  1995/10/06  02:19:30  ppearce
 * CSCdi41663:  Memory leak when using DSPU/QLLC
 *   Release original packet after fragmenting into smaller frames
 *
 * Revision 2.5  1995/08/22  22:44:41  pmorton
 * CSCdi38937:  CLS QLLC sends doesnt handle ID_STN.ind timeout correctly
 * QLLC closes the connection and now tests for valid UCepId before
 * building a Close.Stn.Confirm message
 *
 * Revision 2.4  1995/08/08  16:49:29  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/08/07 05:43:53  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.2  1995/07/19  21:06:36  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.1  1995/06/07  23:01:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "linktype.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "media_registry.h"
#include "../x25/parser_defs_x25.h"
#include "../llc2/llc2.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_proto.h"
#include "../cls/dlc_registry.h"
#include "../cls/cls_assert.h"
#include "../cls/clsi_msg.h"		/* for declarations used in qllc.h */
#include "../cls/dlc.h"
#include "../util/avl.h"
#include "../srt/qllc.h"
#include "../srt/lnx.h"
#include "../srt/qllc_private.h"
#include "qllc_event.h"
#include "../util/fsm.h"
#include "../ibm/sna_util.h"
#include "../ibm/libibm_externs.h"

#ifdef DEBUG
#define inline
#endif

static inline int QllcFsmDecode (int minor, int max);
static inline QXInputT QXFsmEventDequeue(qllctype *qllc);
static ushort QFsmAction(qllctype *qllc, const QFsmParamBlockT *pb);
static ushort QFsmT1N2Action(qllctype *qllc, const QFsmParamBlockT *pb);
static inline boolean QLFsmTestAndSetRunning(qllctype *qllc);
static inline boolean QXFsmTestAndSetRunning(qllctype *qllc);
static inline void QLFsmClearRunning(qllctype *qllc);
static inline void QXFsmClearRunning(qllctype *qllc);

/* QLLC FSM OPERATIONS */

static inline void QCall(qllctype *qllc);
static inline void QAccept(qllctype *qllc);
static inline void QClearRst(qllctype *qllc, uchar diag);
static inline void QClearRstConf(qllctype *qllc);
static inline void QTxXid(qllctype *qllc);
static inline void QTxTest(qllctype *qllc);
static inline void QXTxNact(qllctype *qllc);
static inline void QLNact(qllctype *qllc);
static inline void QReportRemoteError(qllctype *qllc);
static inline void QTxSetMode(qllctype *qllc);
static inline void QTxUA(qllctype *qllc);
static inline void QTxDisc(qllctype *qllc);
static inline void QTxRD(qllctype *qllc);
static inline void QTxDM(qllctype *qllc);
static inline void QDlcQuench(qllctype *qllc, boolean quenchOn);
static inline void QDlcDataOut(qllctype *qllc);
static inline void QDlcDataDrop(qllctype *qllc);
static inline void QReqOpenStnConf(qllctype *qllc, ClsErrorCodeT error);
static inline void CloseStnConf(qllctype *qllc, boolean kill_it);
static inline void QIdInd(qllctype *qllc);
static inline void QTestInd(qllctype *qllc);
static inline void QConnInd(qllctype *qllc);
static inline void QConnConf(qllctype *qllc);
static inline void QConnectedInd(qllctype *qllc);
static inline void QDiscInd(qllctype *qllc);
static inline void QDiscConf(qllctype *qllc, ClsErrorCodeT error);
static inline void QClsQuench(qllctype *qllc, boolean quenchOn);
static inline void QDataInd(qllctype *qllc);
static inline void QDataConf(qllctype *qllc);
static inline void QCleanup(qllctype *qllc);
static inline void QZombie(qllctype *qllc);

static inline boolean QX25Up(qllctype *qllc);

static char *prClockAction(qllctype *qllc, QTimerT op);
static char *prDlcAction(QX25OpT op);
static char *prClsAction(QClsOpT op);
static char *prQLEvent(QLInputT event);
static char *prQXEvent(QXInputT event);



/*
 * q l l c _ s e t s t a t e
 *
 * one point for all qllc state transitions
 */
 
void qllc_setstate(qllctype *qllc, qllc_state_t newstate)
{
    if ( qllc->qllc_state != newstate ) {
	if ( qllc_state_debug )   /* if state changed */
	    buginf("\nQLLC: state %s -> %s",
 		qllc_prstate(qllc->qllc_state), qllc_prstate(newstate) );
	qllc->qllc_state = newstate;  
    }
}


/*
 * qllc_stateswitch
 * main state event dispatcher
 */

void qllc_stateswitch (qllctype *qllc, int new_qevent)
{

    register qllc_state_t prevstate;


    qllc->event = new_qevent;

    prevstate = qllc->qllc_state;

    switch (qllc->qllc_state) {

       case QST_ADM:
          qadmstate(qllc);
          break;

       case QST_SETUP:
          qsetupstate(qllc);
           break;

       case QST_RESET:
          qresetstate(qllc);
          break;

       case QST_DISC:
          qd_connstate(qllc);
          break;

       case QST_NORMAL:
          qnormalstate(qllc);
          break;

       default:
          if (lnx_error_debug)
          buginf("\nQLLCERR: invalid state");
          break;
    }

   if(!qllc->sdlcp)
    {
     if (prevstate != qllc->qllc_state) {
 	  qllc->retry_count = 0;
 	  if (qllc->qllc_state == QST_NORMAL)
	  	qllc_ind_connect(qllc);
       }
    }


}



/*
 * q a d m s t a t e
 *
 * qadmstate (Called LINK_CLOSED State in IBM's doc)
 * Only accept QSM in this state - ignore or send QDM for everythng else.
 */
void qadmstate (qllctype *qllc)
{
 lnxtype *lnxp;

    if (qllc_event_debug)
       buginf("\nQLLC: qadmstate: %s", qllc_inevent(qllc->event) );

    switch (qllc->event) {


    case REC_QIFRAME:
    case REC_QRR_CMD:
       qllc->qllcStat.qllc_numerrs++;
#ifdef NOBODY_READS_ME
       qllc_stats_ignored++;
#endif
       /* state remains ADM */
       break;

    case REC_QSM_CMD:
       lnxp = qllc->lnx;

	lnx_qllc_notify(lnxp, LNX_DLC_CONTACT_IND);
	/* pass event to lnx layer */

       if(!lnxp->xid_valid && lnxp->lnx_state == LNX_AW_NET_QLLC_SEC)
        {
        if (lnx_error_debug)
	    buginf("\nRx QSM going to AW_NET");
         /*return and start the other side, otherwise send QUA back */
         lnx_setstate(lnxp, LNX_AW_NET);
        }
       else if(!lnxp->xid_valid && lnxp->lnx_state == LNX_CONNECT)
        {
        }
       else if(qllc->sdlcp)
        {
     
	 break;				/* Can't have any quenching here
					 * It would break SDLC-QLLC conversion.
					 */
        }
       qllc_quench(qllc, TRUE);		/* for LACK2 compliance */
       break;

    case REC_QDISC_CMD:
       qllc->qllcStat.qllc_numsnddms++;
       qllc_sendcontrol(qllc, QDM, FALSE);
       /* state remains ADM */
       break;

    case REC_QUA_RSP:
       qllc->qllcStat.qllc_numerrs++;
#ifdef NOBODY_READS_ME
       qllc_stats_ignored++;
#endif
       break;

    case REC_QRD_RSP:
       qllc_goto_adm(qllc, QLS_DISCONNECT, TRUE);
       break;

    case REC_QDM_RSP:
    case REC_QFRMR_RSP:
#ifdef NOBODY_READS_ME
       qllc_stats_ignored++;
#endif
       /* state remains ADM */
       break;
       
    default:
       break;
    }

}



/*
 * q s e t u p s t a t e
 *
 * qsetupstate aka LINK_OPENING (IBM)
 * qsetupstate entered when QSM sent.  Enter qnormalstate when QUA recvd.  
 */
void qsetupstate (qllctype *qllc)
{
    lnxtype *lnx;

    if (qllc_event_debug)
        buginf("\nQLLC: qsetupstate: %s", qllc_inevent(qllc->event) );
   
    switch (qllc->event) {
 
    case REC_QIFRAME:
    case REC_QRR_CMD:
        qllc->qllcStat.qllc_numerrs++;
#ifdef NOBODY_READS_ME
        qllc_stats_ignored++;
#endif
        /* state remains SETUP */
        break;
 
    case REC_QSM_CMD:
        /* should be code here for TEST/XID response pending
         * in which case we should flag the error and Clear/Reset
         */
        qllc_sendcontrol(qllc, QUA, FALSE);
        qllc->s_flag = 1;
        /* state remains SETUP */
        break;
        lnx = qllc->lnx;
 
    case REC_QUA_RSP:
        lnx = qllc->lnx;
        qllc_setstate( qllc, QST_NORMAL );

        /* inform lnx layer that the qllc connection is established */
	if(!qllc->sdlcp) {
            lnx_qllc_notify(lnx, LNX_DLC_CONTACTED_IND);
	    qllc_quench(qllc, TRUE);		/* for LACK2 compliance */
            qllc_quench(qllc, FALSE);		/* for LACK2 compliance */
	  }
        break;
 
    case REC_QDISC_CMD:
        /* should be code here for TEST/XID response pending
         * in which case we should flag the error and Clear/Reset
         */
        qllc->qllcStat.qllc_numsnddms++;
        qllc_sendcontrol(qllc, QDM, TRUE);
        break;
 
    case REC_QDM_RSP:
        qllc_goto_adm(qllc, LC_DISC, TRUE);
        qind_concnf(qllc, LS_DISCONNECT); /* unblock qllc_open() */
        break;
 
    case REC_QFRMR_RSP:
        /* if OXrp or OTrp) THEN qllc_sendcontrol(qllc, QSM, TRUE); */
#ifdef NOBODY_READS_ME
        qllc_stats_ignored++;
#endif
        break;
 
    default:
        break;
    }
}



/*
 * q i n d _ c o n c n f
 *
 * unlock qllc_open() by filling in qllc->open_block.
 */
void qind_concnf (qllctype *qllc, int status)
{


    if (qllc_event_debug)
       buginf("\nQLLC: qind_concnf, status=%s", qllc_prstatus(status) );

    qllc->open_block = status;	/* unblock waiting process */
}



/*
 * q r e s e t s t a t e - aka LINK_RECOVERY (IBM)
 */
void qresetstate (qllctype *qllc)
{

   if (qllc_event_debug)
      buginf("\nQLLC: qresetstate: %s", qllc_inevent(qllc->event) );

    switch (qllc->event) {

    case REC_QSM_CMD:
        qllc->qllcStat.qllc_numsnddms++;
	qllc_sendcontrol(qllc, QDM, FALSE);
	break;

    case REC_QDISC_CMD:
	qllc_sendcontrol(qllc, QUA, FALSE);
	break;
	
    default:
	break;
    }

    qllc_goto_adm(qllc, LC_DISC, TRUE);
    qind_concnf(qllc, LS_DISCONNECT); /* unblock qllc_open() */
    return;
}



/*
 * q d _ c o n n s t a t e
 *
 * disconnect state  (aka LINK_CLOSING)
 */

void qd_connstate (qllctype *qllc)
{

    if (qllc_event_debug)
       buginf("\nQLLC: qd_connstate: %s", qllc_inevent(qllc->event) );

    switch (qllc->event) {

    case REC_QSM_CMD:
        qllc->qllcStat.qllc_numerrs++;
#ifdef NOBODY_READS_ME
	qllc_stats_ignored++;
#endif
	break;

    case REC_QDISC_CMD:
        qllc->qllcStat.qllc_numerrs++;
#ifdef NOBODY_READS_ME
	qllc_stats_ignored++;
#endif
	break;

    case REC_QUA_RSP:
    case REC_QDM_RSP:
	qllc_goto_adm(qllc, LC_DISC, TRUE);
	break;

    case REC_QIFRAME:
    case REC_QRR_CMD:
    case REC_QFRMR_RSP:
        qllc->qllcStat.qllc_numerrs++;
#ifdef NOBODY_READS_ME
	qllc_stats_ignored++;
#endif
	break;

    case REC_QRD_RSP:
        qllc_goto_adm(qllc, LC_DISC, TRUE);
        break;

    default:
	break;
    }
}



/*
 * qnormalstate
 */
void qnormalstate (qllctype *qllc)
{

    if (qllc_event_debug && qllc->event != REC_QIFRAME)
	/* don't display I frames */
       buginf("\nQLLC: qnormalstate: %s", qllc_inevent(qllc->event) );

    switch (qllc->event) {

       case REC_QSM_CMD:
       		qllc_goto_adm(qllc, LC_DISC, TRUE);
		break;
		
       case REC_QDISC_CMD:
       		qllc_sendcontrol(qllc, QUA, FALSE);
		break;
		
       case REC_QFRMR_RSP:
          /* NmEvent Hook 1 */
          QllcEvent(qllc, QLLC_FRMR_Rcvd);
          /* fall down to goto_adm */

       case REC_QUA_RSP:
       case REC_QDM_RSP:
          qllc_goto_adm(qllc, LC_DISC, TRUE);
          break;

       case REC_QIFRAME:
          qindinfo(qllc);
          /* state remains NORMAL */
          break;

/* We may be able to use a QRR packet to let the data start flowing to
 * the host.
 */
	
       case REC_QRR_CMD:
          /* state remains NORMAL */
          break;

      case REC_QRD_RSP:
	  qllc_close(qllc);		/* changes state */
	  break;

      case REC_FRMR_EVENT:
      	qllc_sendQFRMR (qllc);
	qllc_setstate (qllc, QST_RESET);
	break;
      
       default:
          break;;
    }

}

/*
 * q i n d i n f o
 *
 * recvd i-frame from qllc - pass it on to lnx to send to llc.
 *
 * NOT if we're being quenched from the llc side.
 */
void qindinfo (qllctype *qllc)
{
    paktype *pak;
    
    if ( (pak = qllc->inbuf) == NULL)
	return;
    
    /* send it to lnx or snx */
    if(qllc->sdlcp)
	snx_input(pak, qllc);

    else {
	/* If NEITHER the network side is busy NOR is there anything in lnxQ
	 * just pass the packet to lnx
	 * This is the normal case and should be handled first
	 */
	
	if ( !(qllc->qllc_net_busy) && !(qllc->qllc_lnxQ.qhead ) ) {
	    lnx_output (pak, qllc);
	} else {
	    
	    /* put the held-over packet in the lnxQ, at the tail */
	    p_enqueue (&qllc->qllc_lnxQ, pak );
	    
	    /* if qllc_net_busy is off
	     * use the qllc_quench OFF routine to drain the queue
	     * in the right order
	     */
	    if (!qllc->qllc_net_busy)
		qllc_quench (qllc, FALSE);
	}
    }

    qllc->inbuf = NULL;			/* mark consumed */

}


/*
 * qllc_type1event()
 */
void qllc_type1event(qllctype *qllc, int newevent)
{
    if (qllc->sdlcp) {
	qllc_type1event_snx(qllc, newevent);
    } else if (qllc->lnx) {
	qllc_type1event_lnx(qllc, newevent);
    }
    return;
}

/*
 * qllc_type1event_snx()
 * This routine will take care of the XID and TEST packets.
 */
void qllc_type1event_snx(qllctype *qllc, int newevent)
{
 uchar *frameptr;
 paktype *pak;
 
 boolean  cmd;
 
  if(!qllc)
   {
    if (qllc_event_debug)
     {
      buginf("\nQLLC: unknown qllc, failed");
     }
    return;
   }
 
   
 cmd = FALSE; /*deal with Response */
 if(newevent == REC_QXID_CMD)
  {
   /* get a buffer to send qllc frame whose maximum QLLC size is 2 */
   pak = getbuffer(0);
   if (!pak)
    {
     qllc->qllcStat.qllc_numdrops++;
     return;
    }
   qllc->qllcStat.qllc_xidout++;
 
   pak->datagramsize = QLLC_HDRSIZE + SNX_XID_SIZE;   /*for xid*/
   frameptr = pak->datagramstart;
 
   if (cmd)
    *frameptr = QLLC_CMD;
   else
    *frameptr = QLLC_RSP;

   *(frameptr + 1) = QXID;
   *(frameptr + 2) = 0x02;         /* PU2 end host,type 0 */
   *(frameptr + 3) = 0x00;         /* Fixed format - reserved */
 
   if (qllc->lnx->xid_valid)
    {
     *(frameptr + 4) = qllc->lnx->xid[0];
     *(frameptr + 5) = qllc->lnx->xid[1];
     *(frameptr + 6) = qllc->lnx->xid[2];
     *(frameptr + 7) = qllc->lnx->xid[3];
    }
   else
    {
     /* XID  0x01700017 */
     *(frameptr + 4) = 0x01;
     *(frameptr + 5) = 0x70;
     *(frameptr + 6) = 0x00;
     *(frameptr + 7) = 0x17;
    }
 
  }
 else if (newevent ==  REC_QTEST_CMD)
  {
   if( (pak = qllc->inbuf) == NULL)
    return;
 
   qllc->qllcStat.qllc_testout++;
   if(cmd)
    ((qllc_header *)pak->datagramstart)->addr  =  QLLC_CMD;
   else
    ((qllc_header *)pak->datagramstart)->addr  =  QLLC_RSP;
 
   ((qllc_header *)pak->datagramstart)->cntrl =  QTEST;
 
   /*
   pak->datagramsize = QLLC_HDRSIZE + SNX_XID_SIZE;
   */
 
   qllc->inbuf = NULL; /* don't release it in qllc code */
  }
 else  /* unknown, drop it */
  return;
 
 pak->x25_flags |= PAK_X25_QBIT;                /* yes */
 qllc_psend(qllc, pak);
 mgd_timer_start(&qllc->fTimer, QT1_DEFAULT);

}



/* qllc_type1event_lnx
 *
 * qllc receives a type 1 packet;
 * 
 * It ignores a TEST response, replies immediately to a TEST command,
 * and passes XIDs up the to the lnx level for forwarding
 */

void qllc_type1event_lnx (qllctype *qllc, int newevent)
{
    paktype *pak;
    boolean qllc_xid_response = TRUE;
 
    switch(newevent) {
 
    case REC_QTEST_CMD:
        qllc->qllcStat.qllc_testout++;
        qllc_sendcontrol(qllc, QTEST, FALSE);   /* send response */
        break;
 
    case REC_QXID_CMD:
        qllc_xid_response = FALSE;
    case REC_QXID_RSP:        
        pak = qllc->inbuf;
        qllc->inbuf = NULL;
 
        /* eat the address byte; it's specific to QLLC */
        pak->datagramstart++;
        pak->datagramsize--;
	
	/* clear it, just in case it would have an effect elsewhere */
	pak->x25_flags &= ~PAK_X25_QBIT;

        qllc->qllcStat.qllc_xidout++;
        lnx_net_fwd_xid(qllc->lnx, pak, qllc_xid_response);
        break;
    }
}



/*
 * qllc_sendcontrol
 * Send a QLLC (Q-bit set) frame
 */
void qllc_sendcontrol (qllctype *qllc, int ctype, boolean cmd)
{

    uchar *frameptr;
    paktype *pak;

    /* 
     * get a buffer to send qllc frame whose maximum QLLC size is 2 
     */
    pak = getbuffer(0);
    if (!pak) {
        qllc->qllcStat.qllc_numdrops++;
	return;
    }

    pak->datagramsize = QLLC_HDRSIZE;
    frameptr = pak->datagramstart;

    if (cmd)
       *frameptr = QLLC_CMD;
    else
       *frameptr = QLLC_RSP;

    *(frameptr + 1) = ctype;	
    
    pak->x25_flags |= PAK_X25_QBIT;		/* yes */
    qllc_psend(qllc, pak);
    mgd_timer_start(&qllc->fTimer, QT1_DEFAULT);
}



/*
 * qllc_sendQFRMR
 */
void qllc_sendQFRMR (qllctype *qllc)
{
    uchar *frameptr;
    paktype *pak;

    if (!qllc->qllc_frmr) {
        if (lnx_error_debug)
    	  buginf("\n NO QFRMR info");
	return;
    }
    qllc->qllcStat.qllc_numsndfrmrs++;

    /* NmEvent Hook 2 */
    QllcEvent(qllc, QLLC_FRMR_Sent); 
    
    /* get a buffer to send FRMR, max QLLC size is 5 */
    pak = getbuffer(0);

    if (!pak) {
        qllc->qllcStat.qllc_numdrops++;
	return;
    }
    
    pak->datagramsize = QLLC_HDRSIZE + 3;
    frameptr = pak->datagramstart;
    
    *frameptr       = 0x00;     /* response frame */
    *(frameptr + 1) = QFRMR;
    bcopy( qllc->qllc_frmr, frameptr+2, sizeof(qllc_frmr_t) );
    
    pak->x25_flags |= PAK_X25_QBIT;		/* yes */
    qllc_psend(qllc, pak);
    
    return;
}


/*
 * qllc_goto_adm
 */
void qllc_goto_adm (qllctype *qllc, int reason, boolean kill_flag)
{
    if (qllc && (checkqueue(&qllc_cleanupQ, qllc) == FALSE)) {
        qllc->adm_reason = reason;
        qllc->retry_count = 0;
        qllc->vc_kill_flag = kill_flag;
        qllc_setstate ( qllc, QST_ADM );

       /*
        * Put this qllc control block on the cleanup queue.
        */
        p_enqueue(&qllc_cleanupQ, qllc);
    }
}



/*
 * qllc_sendRR
 */
void qllc_sendRR (qllctype *qllc)
{

    /* only send RR's in normal state */
    if (qllc->qllc_state == QST_NORMAL)
       qllc_sendcontrol(qllc, QRR, TRUE);
    else
       if (qllc_event_debug)
          buginf("\nQLLC: qllc_sendRR - state %s", 
		 qllc_prstate(qllc->qllc_state) );

}

/*
 * qllc_prstate
 *
 * make the qllc state visible
 */
 
char *qllc_prstate(uchar state)
{
   static char invalid[] = "invalid    ";

   switch (state) {
	
     case QST_ADM:
	 return ("ADM");
	 
     case QST_SETUP:
	 return ("SETUP");

     case QST_RESET:
	 return ("RESET");
	 
     case QST_DISC:
	 return ("DISCONNECTING");

     case QST_NORMAL:
	 return ("NORMAL");

     default:
	 sprintf(&invalid[0], "invalid %d", state);
	 return (&invalid[0]);
     }
}



/*
 * qllc_inevent
 * Prints out the incoming event from x.25
 */
char *qllc_inevent (int event)
{

   switch (event) {

      case REC_QIFRAME:
         return("recvd i-frame");

      case REC_QSM_CMD:
         return("recvd qsm");

      case REC_QDISC_CMD:
         return("recvd qdisc");

      case REC_QXID_CMD:
         return("recvd qxid cmd");

      case REC_QTEST_CMD:
         return("recvd qtest cmd");

      case REC_QRR_CMD:
         return("recvd qrr cmd");

      case REC_QXID_RSP:
         return("recvd qxid rsp");

      case REC_QTEST_RSP:
         return("recvd qtest rsp");

      case REC_QUA_RSP:
         return("recvd qua rsp");

      case REC_QRD_RSP:
         return("recvd qrd rsp");

      case REC_QDM_RSP:
         return("recvd qdm rsp");

      case REC_QFRMR_RSP:
         return("recvd qfrmr rsp");

      case REC_FRMR_EVENT:
         return("recvd bad pdu");

      case REC_TOO_QSMALL:
         return("recvd too small frame");

      default:
         return("unknown event");
   }

}



/*
 * qllc_prstatus
 */
char *qllc_prstatus (int status)
{

   switch (status) {
   
      case QLS_SUCCESS:
         return("success");

      case QLS_DISCONNECT:
         return("disconnect");

      case QLS_FAILED:
         return("failed");

      case QLS_WAITING:
         return("waiting");

      case QLS_INTDOWN:
         return("interface down");
  
      case QLS_TIMEOUT:
         return("timeout");

      default:
         return("invalid");
   }

}


/************************************************************************
 *									*
 *				QLLC FSMs				*
 *									*
 ************************************************************************
 */

#define QLLC_FSM_BUFF_SIZE	120

/* we can define these statically, since we run single-threaded 
 * through the FSMs
 */
static uchar 		QFsmDbBuf[QLLC_FSM_BUFF_SIZE];
static uchar 		*QFsmDbPtr;

#define QAction		FSM_ACTION_ROUTINE(QFsmAction)
#define QT1N2Action	FSM_ACTION_ROUTINE(QFsmT1N2Action)
#define QT1TimerAct     FSM_ACTION_ROUTINE(QFsmT1TimerAct)
#define QLFsmDecode	FSM_DECODE_ROUTINE(QllcLFsmDecode)
#define QXFsmDecode	FSM_DECODE_ROUTINE(QllcXFsmDecode)
#define QFsmPB		const static QFsmParamBlockT

/*
 * QllcFsmDecode
 *
 * Given a major and minor, produce the majik index that drives all
 * the different array indicies.
 */

static inline int QllcFsmDecode (int minor, int max)
{
    if (minor < 0 || minor >= max)
	return(FSM_FAULT);
    return(minor );
}

/*
 * QllcLFsmDecode
 *
 * Input decode for the Logical Link Layer
 */

static int QllcLFsmDecode (qllctype *qllc, int major, int minor)
{
    return(QllcFsmDecode(minor, QLNumInputs));
}

/*
 * QllcXFsmDecode
 *
 * Input decode for the X.25 Layer
 */

static int QllcXFsmDecode (qllctype *qllc, int major, int minor)
{
    return(QllcFsmDecode(minor, QX25NumInputs));
}

/* QFsmEventEnqueue
 *
 * Make an event and put it on the event queue specifed
 */
 
void 
QFsmEventEnqueue (qllctype *qllc, queuetype *queue, int event, paktype *pak)
{
    QllcEventT *qEvent;
	
    qEvent = QEventCreate(qllc, event, pak);
    CLSASSERT(qEvent);
    if (qEvent != NULL) {
	p_enqueue(queue, qEvent);
    }
    return;
}

/* critical regions - atomic operations */

/*
 * QllcT1TimerAct
 *
 * T1 Timer expired in Link Closed State.
 */
static ushort QFsmT1TimerAct (qllctype *qllc, const QFsmParamBlockT *pb)
{
    paktype     *xid_pak;

    xid_pak = qllc_get_stored_xid3(qllc);
    if (xid_pak) {
        /*
         * We may have a situation where the XID(3)
         * negotiation with a PUT2.1 configured as
         * Link "Negotiable" on QLLC needs a "nudge".
         * Retransmit the XID(3) once in this case.
         */
        qllc_fwd_xid(qllc, xid_pak);
        mgd_timer_start(&qllc->fTimer, qllc->fTimers.fQLTimer);
    } else {
        QFsmAction(qllc, pb);   /*  Make the Logical Fsm output come out right */
    }

    return(FSM_OK);
}

/* X1QFsmT1N2Action
 *
 * Action when the timer hits max retries
 */

static ushort QFsmT1N2Action (qllctype *qllc, const QFsmParamBlockT *pb)
{
    QFsmAction(qllc, pb);	/* make the Logical Fsm output come out right */
    QXFsmExecute(qllc,QX25CloseStnReq);	/* we are AFU, clean up and GET OUT */
    return(FSM_OK);
}

/* QFsmAction
 *
 * This action routine is used by both the X25 FSMs and by the QLLC Clsi FSM
 */

static ushort QFsmAction (qllctype *qllc, const QFsmParamBlockT *pb)
{
    if (qllc == NULL)
	return (0);

    if (qllc_state_debug) {
    
	QFsmDbPtr += sprintf(QFsmDbPtr, " (%s,%s,%s) ", 
			     prDlcAction(pb->fDlcOp),
			     prClsAction(pb->fClsOp),
			     prClockAction(qllc, pb->fTimerOp));
    }

	/* Timer Actions - there's one time for QLLC - it's either
	 * used for the X.25 FSM, or for the Logical Link FSM
	 *
	 * If the timer expries in the X.25 FSM, that's the end of the story
	 * In the Logical Link FSM, retries are possible.
	 *
	 * When the timer pops,  qllc will figure out which fsm is 
	 * active at that time, and if it's the Logical Link FSM,
	 * the retry count is incremented.  If it reaches the maximum value
	 * then the second level timeout has expired and we go to a different
	 * input.
	 */
    switch (pb->fTimerOp) {
    case OpQTimerNone:
	break;
	
    case OpQXTimerStart:
	mgd_timer_start(&qllc->fTimer, qllc->fTimers.fX25Timer);
	break;
	
    case OpQXTimerStop:
	mgd_timer_stop(&qllc->fTimer);
	break;
	
    case OpQLTimerStart:
	qllc->fClsRetries =0;
	mgd_timer_start(&qllc->fTimer, qllc->fTimers.fQLTimer);
	break;
	
    case OpQLTimerIncr:
	CLSASSERT(qllc->fRetries < qllc->fTimers.fQLRetries); 
	mgd_timer_start(&qllc->fTimer, qllc->fTimers.fQLTimer);
	break;
	
    case OpQLTimerStop:
	mgd_timer_stop(&qllc->fTimer);
	break;
	
    default:
	CLSASSERT(!"Illegal QLLC Timer Opcode");
	break;
	
    }
    
    /* X.25 Output */
    switch (pb->fDlcOp) {

    case OpX25None:
	break;

	/* X.25 VIrtual CIrcuit Op Codes		 	*/
    case OpCallReq:
	/* if Format is Vmac Address, let X.25 look up
	 * else place call using X.121 address in the addrtype
	 *
	 * We have to anticipate this state/action - if the remote 
	 * address is invalid we had better just fail in the ReqOpenStn
	 * Request before we even get to the X.25 FSM.  ...  This
	 * inplies that we have to have some knowledge of the X.25 Qllc
	 * Map.  Sigh.
	 *
	 * PHIL 1/31/95
	 */
	QCall(qllc);
	break;
	 
	 
    case OpAccept:
	QAccept(qllc);
	break;
	 
    case OpClearRst:			/* Clear/Reset	- normal	*/
	QClearRst(qllc, X25_DIAG_NO_INFO);
	break;
    case OpClearRstE:			/* Clear/Reset w/ Error		*/
	QClearRst(qllc, X25_DIAG_REMOTE_PROTOCOL);
	break;
    case OpClearRstConf:			/* Clear/Reset Confirm		*/
	QClearRstConf(qllc);
	break;
	/* QLLC FSM Op Codes				*/
    case OpXid:
	QTxXid(qllc);
	break;
    case OpXNact:				/* Non-activation XID output */
	QXTxNact(qllc);
	break;
    case OpTest:
	QTxTest(qllc);
	break;
    case OpSetMode:
	QTxSetMode(qllc);
	break;
    case OpUA:
	QTxUA(qllc);
	break;
    case OpDISC:
	QTxDisc(qllc);
	break;
    case OpRD:
	QTxRD(qllc);
	break;
    case OpDM:
	QTxDM(qllc);
    case OpRNR:				/* X.25 Packet level RNR 	*/
	QDlcQuench(qllc, TRUE);
	break;
    case OpRR:				/* X.25 Packet level RR 	*/
	QDlcQuench(qllc, FALSE);
	break;
    case OpDlcData:
	QDlcDataOut(qllc);
	break;
    case OpDlcDrop:
        QDlcDataDrop(qllc);
        break;
    case OpLocalError:
	CLSASSERT(!"Local Procedure Error");
	break;
    case OpRemoteError:
	QReportRemoteError(qllc);
	break;
    default:
	CLSASSERT(!"QLLC Illegal X.25 Opcode");
	break;
    }
    
    /* CLSI Output to cls_rput */
    switch (pb->fClsOp) {

    case OpClsNone:
	break;

    case OpReqOpenStnConf:		/* Request Open Station Confirm		*/
	QReqOpenStnConf(qllc, CLS_OK);
	break;

    case OpReqOpenStnConfL:		/* Request Open Station Confirm	local error */
	CLSASSERT(FALSE);		/* THIS SHOULD NEVER HAPPEN */
	QReqOpenStnConf(qllc, CLS_LOCAL_ERROR);
	break;

    case OpReqOpenStnConfR:		/* Request Open Station Confirm	remote error */
	QReqOpenStnConf(qllc, CLS_NO_REM_STN);
	break;

    case OpCloseStnConf:                /* Close Station Confirm - QLLC cb lives. */
	CloseStnConf(qllc, FALSE);
	break;

    case OpCloseStnConfKill:            /* Close Station Confirm - QLLC cb dies. */
	CloseStnConf(qllc, TRUE);
	break;

    case OpDiscInd:			/* Disconnect Indication - Both levels of FSM
					 * can use this message - sounds dubious - PHIL
					 */
	QDiscInd(qllc);
	break;

	/* QLLC FSM Op Codes						*/
    case OpIdInd:			/* ID Indication				*/
	QIdInd(qllc);
	break;
    case OpLNact:
	QLNact(qllc);
	break;
    case OpTestInd:
	QTestInd(qllc);
	break;
    case OpConnInd:
	QConnInd(qllc);
	break;
    case OpConnConf:
	QConnConf(qllc);
	break;
    case OpCtdInd:
	QConnectedInd(qllc);
	break;
    case OpDiscConf:
	QDiscConf(qllc, CLS_OK);
	break;
    case OpDiscConfE:
	QDiscConf(qllc, CLS_DISC_ERROR);
	break;
    case OpQuenchOn:
	QClsQuench(qllc, TRUE);
	break;
    case OpQuenchOff:
	QClsQuench(qllc, FALSE);
	break;
    case OpDataInd:
	QDataInd(qllc);
	break;
    case OpDataConf:			/* Negative Conf - link went down */
	QDataConf(qllc);
	break;
    case OpCleanup:
        QCleanup(qllc);
        break;
    case OpZombie:
        QZombie(qllc);
        break;
			
    default:
	CLSASSERT(!"Illegal QLLC Cls Opcode");
	break;

    }
    return (FSM_OK);
}


/*
 *****************************************************************************
 *
 *			QLLC X.25 FSM
 *
 * This FSM implements the X.25 virtual circuit connectivity
 *
 *****************************************************************************
 */

/* QLLC X.25 FSM Parameter Blocks */

/* 				   X25 Op	  Cls Op	     Timer	 */

QFsmPB QXpbAccept_Rosc_Stop =    { OpAccept, 	  OpReqOpenStnConf,  OpQXTimerStop  };
QFsmPB QXpbCR_X_Start	= 	 { OpClearRst, 	  OpClsNone,	     OpQXTimerStart };
QFsmPB QXpbCR_ClsConf_Start =    { OpClearRst, 	  OpCloseStnConf,    OpQXTimerStart };
QFsmPB QXpbCR_X_X =		 { OpClearRst, 	  OpClsNone,	     OpQTimerNone   };
QFsmPB QXpbCRConf_Rosc_Stop =    { OpClearRstConf,OpReqOpenStnConf,  OpQXTimerStop  };
QFsmPB QXpbCRConf_RoscR_X =	 { OpClearRstConf,OpReqOpenStnConfR, OpQTimerNone   };
QFsmPB QXpbCRConf_RoscR_Start =  { OpClearRstConf,OpReqOpenStnConfR, OpQXTimerStart };
QFsmPB QXpbCRConf_Cleanup_Start ={ OpClearRstConf,OpCleanup,         OpQXTimerStart };
QFsmPB QXpbCRConf_ClsCnfK_Start ={ OpClearRstConf,OpCloseStnConfKill,OpQXTimerStart };
QFsmPB QXpbCRConf_ClsCnfK_Stop = { OpClearRstConf,OpCloseStnConfKill,OpQXTimerStop  };
QFsmPB QXpbCRConf_Id_Start =     { OpClearRstConf,OpIdInd,           OpQXTimerStart };
QFsmPB QXpbCRConf_X_Start = 	 { OpClearRstConf,OpClsNone,	     OpQXTimerStart };
QFsmPB QXpbCRConf_X_Stop = 	 { OpClearRstConf,OpClsNone,	     OpQXTimerStop  };
QFsmPB QXpbCRConf_X_X =	 	 { OpClearRstConf,OpClsNone,	     OpQTimerNone   };
QFsmPB QXpbCRe_Cleanup_Start =	 { OpClearRstE,	  OpCleanup,         OpQXTimerStart };
QFsmPB QXpbCRe_DiscInd_Start=    { OpClearRstE,	  OpDiscInd,	     OpQXTimerStart };
QFsmPB QXpbCRe_RoscR_Start =	 { OpClearRstE,	  OpReqOpenStnConfR, OpQXTimerStart };
QFsmPB QXpbCRe_RoscR_Stop =	 { OpClearRstE,	  OpReqOpenStnConfR, OpQXTimerStop  };
QFsmPB QXpbCRe_X_Start =	 { OpClearRstE,	  OpClsNone,	     OpQXTimerStart };
QFsmPB QXpbCRe_X_X =		 { OpClearRstE,	  OpClsNone,	     OpQTimerNone   };
QFsmPB QXpbCall_X_Start =	 { OpCallReq,	  OpClsNone,	     OpQXTimerStart };
QFsmPB QXpbQTest_X_Start =       { OpTest,        OpClsNone,         OpQXTimerStart };
QFsmPB QXpbX_ClsCnfK_Start =	 { OpX25None,	  OpCloseStnConfKill,OpQXTimerStart };
QFsmPB QXpbX_ClsConf_X =	 { OpX25None,	  OpCloseStnConf,    OpQTimerNone   };
QFsmPB QXpbX_ClsCnfK_X =	 { OpX25None,	  OpCloseStnConfKill,OpQTimerNone   };
QFsmPB QXpbX_DiscInd_Start =	 { OpX25None,	  OpDiscInd,	     OpQXTimerStart };
QFsmPB QXpbX_Id_Start =	 	 { OpX25None,	  OpIdInd,	     OpQXTimerStart };
QFsmPB QXpbX_RoscR_Start =       { OpX25None,	  OpReqOpenStnConfR, OpQXTimerStart };
QFsmPB QXpbX_RoscR_Stop =	 { OpX25None,	  OpReqOpenStnConfR, OpQXTimerStop  };
QFsmPB QXpbX_RoscL_Start =	 { OpX25None,	  OpReqOpenStnConfL, OpQXTimerStart  };
QFsmPB QXpbX_Rosc_Stop = 	 { OpX25None,	  OpReqOpenStnConf,  OpQXTimerStop  };
QFsmPB QXpbX_RoscL_X =		 { OpX25None,	  OpReqOpenStnConfL, OpQTimerNone   };
QFsmPB QXpbX_X_Stop =		 { OpX25None,	  OpClsNone,	     OpQXTimerStop  };
QFsmPB QXpbX_X_Start =		 { OpX25None,	  OpClsNone,	     OpQXTimerStart };
QFsmPB QXpbX_Cleanup_X =         { OpX25None,     OpCleanup,         OpQTimerNone   };
QFsmPB QXpbX_Cleanup_Start =     { OpX25None,     OpCleanup,         OpQXTimerStart };
QFsmPB QXpbX_Cleanup_Stop =      { OpX25None,     OpCleanup,         OpQXTimerStop  };
QFsmPB QXpbX_Zombie_X =          { OpX25None,     OpZombie,          OpQTimerNone   };

/* SVC State Tables */

/* P1 - Inoperative */

const static struct fsm_state QX25SvcP1St[QX25NumInputs] =
{

/*
 * rbatz -  In P1, we should never specify an X.25 FSM Parameter Block
 *          that stops the timer.  Otherwise a zombie QLLC control
 *          block might never get deleted.
 */

/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbCall_X_Start,	QX25_P2_D2 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbX_ClsCnfK_X,       FSM_NOCHANGE },	/* 1, QX25CloseStnReq  		*/
				    			/* X.25 Inputs	*/
{ QAction,	&QXpbX_Id_Start,	QX25_P3_D3 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRConf_X_X,	FSM_NOCHANGE },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbCRe_X_X,           FSM_NOCHANGE },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_X,       FSM_NOCHANGE }, /* 10,QX25Delete                */ 
  				     			/* Timer Inputs	*/
{ QAction,	&QXpbX_Zombie_X,        FSM_NOCHANGE },	/* 11,QX25TimerExpires          */
{ QAction,	&QXpbX_Zombie_X,        FSM_NOCHANGE },	/* 12,QX25OutOfOrderTimer       */
};

/*
 * This is a dummy state.  It only exists so that
 * SVCs and PVCs have the same number of states.
 * The SVC FSM should NEVER go to this state.
 */
const static struct fsm_state QX25SvcStartSt[QX25NumInputs] =
{ {0} 
};


/* P2 - Call Placed */

const static struct fsm_state QX25SvcP2St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				    			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_Start,	QX25_P6_D2 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCR_ClsConf_Start,	QX25_P6_D2 },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbX_Rosc_Stop,	QX25_P4_D1 },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRConf_RoscR_X,	QX25_P1 },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_RoscR_Stop,      QX25_P1    },   /* 10,QX25Delete                */ 
				     			/* Timer Inputs	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 12,QX25OutOfOrderTimer       */
};

/* P3 - Incoming Call Received */

const static struct fsm_state QX25SvcP3St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbAccept_Rosc_Stop,	QX25_P4_D1 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCR_ClsConf_Start,	QX25_P6_D2 },	/* 1, QX25CloseStnReq  		*/
				    			 /* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRConf_X_X,	QX25_P7_D3},	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1    },   /* 10, QX25Delete               */ 
							/* Timer Inputs	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 12,QX25OutOfOrderTimer       */
};

/* P4/D1 - Virtual Circuit is in place, available for data transport. */

const static struct fsm_state QX25SvcP4_D1St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
						     	/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_X,		FSM_NOCHANGE },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCR_ClsConf_Start,  QX25_P6_D2 },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_DiscInd_Start,QX25_P6_D2 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_DiscInd_Start,QX25_P6_D2 },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbX_DiscInd_Start,	QX25_P7_D3},	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_DiscInd_Start,QX25_P6_D2 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbX_DiscInd_Start,   FSM_NOCHANGE }, /* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbX_DiscInd_Start,   FSM_NOCHANGE },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbCRe_DiscInd_Start,QX25_P6_D2 },	/* 8, QX25ResetConfirm		*/
{ FSM_NOACTION,  FSM_NOARG,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction, 	&QXpbX_DiscInd_Start,	QX25_P1     },	/* 10,QX25Delete                */
				     			/* Timer Inputs	*/
{ QAction,	&QXpbX_X_Stop,		FSM_NOCHANGE },	/* 11,QX25TimerExpires	  	*/
{ QAction,	&QXpbX_X_Stop,          FSM_NOCHANGE },	/* 12,QX25OutOfOrderTimer       */
};

/* P6/D2 - Virtual Circuit clear - initiated from DLU/CLSI */

const static struct fsm_state QX25SvcP6_D2St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_X,		FSM_NOCHANGE },	/* 0, QX25ReqOpenStnReq         */
{ QAction,	&QXpbX_ClsConf_X,       FSM_NOCHANGE },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbX_Cleanup_Stop,	QX25_P1 },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbX_Cleanup_Stop,	QX25_P1 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbCRe_X_X,           FSM_NOCHANGE },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1 },      /* 10,QX25Delete                */ 
 				     			/* Timer Inputs	*/
{ QAction, 	&QXpbX_Cleanup_Stop,	QX25_P1 },	/* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbX_Cleanup_Stop,    QX25_P1 },	/* 12,QX25OutOfOrderTimer       */
};

/* P7/D3 - Virtual Circuit clear - initiated from X.remote X.25 device */

const static struct fsm_state QX25SvcP7_D3St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_X,		FSM_NOCHANGE },	/* 0, QX25ReqOpenStnReq         */
{ QAction,	&QXpbCRConf_ClsCnfK_Stop,QX25_P1 },     /* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 3, QX25CallConfirm  		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbCRe_X_X,           FSM_NOCHANGE },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1 },      /* 10, QX25Delete               */ 
  				     			/* Timer Inputs	*/
{ QAction, 	&QXpbCRConf_X_Stop,	QX25_P1	},	/* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbCRConf_X_Stop,	QX25_P1 },	/* 12,QX25OutOfOrderTimer       */
};

const static fsm_t QX25SvcFsmTable[QX25NumStates] = 
{
    /*  State        	Decoder		*/
      { QX25SvcP1St,	QXFsmDecode },
      { QX25SvcStartSt, QXFsmDecode },
      { QX25SvcP2St,	QXFsmDecode },
      { QX25SvcP3St,	QXFsmDecode },
      { QX25SvcP4_D1St,	QXFsmDecode },
      { QX25SvcP6_D2St,	QXFsmDecode },
      { QX25SvcP7_D3St,	QXFsmDecode },
};

/* PVC State Tables */

/* Quasi P1 - Inoperative 
 *
 * Of course we know that there is no such thing as P1 for a PVC
 * But it's useful to define an initial inoperative state.
 */

const static struct fsm_state QX25PvcP1St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbCR_X_Start,	QX25_P2_D2 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbX_ClsCnfK_Start,	FSM_NOCHANGE },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRe_X_X,           FSM_NOCHANGE },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 5, QX25ClearConfirm         	*/
{ QAction,	&QXpbCRConf_Id_Start,	QX25_P3_D3 },	/* 6, QX25ResetIndication	*/
{ QAction,	&QXpbCRConf_X_Start,    FSM_NOCHANGE },	/* 7, QX25ResetOutOfOrder       */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    FSM_NOCHANGE }, /* 10,QX25Delete                */ 
				     			/* Timer Inputs	*/
{ QAction,	&QXpbX_Id_Start,        QX25_STARTUP },	/* 11,QX25TimerExpires	  	*/
{ QAction,      &QXpbQTest_X_Start,     FSM_NOCHANGE },	/* 12,QX25OutOfOrderTimer       */
};

const static struct fsm_state QX25PvcStartSt[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbCR_X_Start,	QX25_P2_D2 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbX_ClsCnfK_Start,	QX25_P1 },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P1 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P1 },	/* 3, QX25CallConfirm           */
{ QAction, 	&QXpbCRe_X_Start,	QX25_P1 },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P1 },	/* 5, QX25ClearConfirm     	*/
{ QAction,	&QXpbCRConf_X_Start,	QX25_P3_D3 },	/* 6, QX25ResetIndication	*/
{ QAction,	&QXpbCRConf_X_Start,    QX25_P1 },	/* 7, QX25ResetOutOfOrder       */
{ QAction,      &QXpbX_X_Start,         QX25_P1 },      /* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P1 },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1 },      /* 10,QX25Delete                */ 
				     			/* Timer Inputs	*/
{ QAction,	&QXpbX_X_Start,         QX25_P1 },      /* 11,QX25TimerExpires	  	*/
{ QAction,	&QXpbX_X_Start,         QX25_P1 },	/* 12,QX25OutOfOrderTimer       */
};

/* P2 - Request Open Station sent - sent Reset Request */

const static struct fsm_state QX25PvcP2St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_Start,	QX25_P6_D2 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCR_ClsConf_Start,	QX25_P6_D2 },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P1 },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_RoscR_Stop,	QX25_P6_D2 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRConf_Rosc_Stop,	QX25_P4_D1 },	/* 6, QX25ResetIndication	*/
{ QAction,      &QXpbCRConf_RoscR_Start,QX25_P1 },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbX_Rosc_Stop,	QX25_P4_D1 },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_RoscR_Start,	QX25_P6_D2 },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_RoscR_Stop,      QX25_P1    },   /* 10,QX25Delete                */ 
				     			/* Timer Inputs	*/
{ QAction, 	&QXpbX_RoscR_Start,	QX25_P1 },	/* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbX_RoscR_Start,	QX25_P1 },	/* 12,QX25OutOfOrderTimer       */
};

/* P3 - Reset Indication Received */

const static struct fsm_state QX25PvcP3St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_Rosc_Stop,	QX25_P4_D1 },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCR_ClsConf_Start,	QX25_P6_D2 },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRConf_X_X,        FSM_NOCHANGE },	/* 6, QX25ResetIndication	*/
{ QAction,	&QXpbCRConf_Cleanup_Start,QX25_P1 },	/* 7, QX25ResetOutOfOrder       */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_Start,	QX25_P6_D2 },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1    },   /* 10,QX25Delete                */ 
				     			/* Timer Inputs	*/
{ QAction, 	&QXpbCR_X_Start,        QX25_P6_D2 },	/* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbCR_X_Start,        QX25_P6_D2 },	/* 12,QX25OutOfOrderTimer       */
};

/* P4/D1 - Virtual Circuit is in place, available for data transport. */

const static struct fsm_state QX25PvcP4_D1St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_X,		FSM_NOCHANGE },	/* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCR_ClsConf_Start,	QX25_P6_D2 },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_DiscInd_Start, QX25_P6_D2 },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_DiscInd_Start, QX25_P6_D2 },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRe_DiscInd_Start, QX25_P6_D2},	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_DiscInd_Start, QX25_P6_D2 },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbX_DiscInd_Start,	QX25_P7_D3 },	/* 6, QX25ResetIndication	*/
{ QAction, 	&QXpbX_DiscInd_Start,	QX25_P7_D3 },	/* 7, QX25ResetOutOfOrder       */
{ FSM_NOACTION, FSM_NOARG,		FSM_NOCHANGE },	/* 8, QX25ResetConfirm          */
{ FSM_NOACTION, FSM_NOARG,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_DiscInd_Start,   QX25_P1 },      /* 10,QX25Delete                */
				     			/* Timer Inputs	*/
{ QAction,	&QXpbX_X_Stop,		FSM_NOCHANGE },	/* 11,QX25TimerExpires	  	*/
{ QAction,	&QXpbX_X_Stop,          FSM_NOCHANGE },	/* 12,QX25OutOfOrderTimer       */
};

/* P6/D2 - Virtual Circuit reset - initiated from DLU/CLSI */

const static struct fsm_state QX25PvcP6_D2St[QX25NumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				     			/* CLSI Inputs	*/
{ QAction, 	&QXpbX_RoscL_X,		FSM_NOCHANGE },	/* 0, QX25ReqOpenStnReq         */
{ QAction,      &QXpbX_ClsConf_X,       FSM_NOCHANGE },	/* 1, QX25CloseStnReq  		*/
				     			/* X.25 Inputs	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 2, QX25IncomingCall     	*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 3, QX25CallConfirm  		*/
{ QAction, 	&QXpbCRe_X_X,           FSM_NOCHANGE },	/* 4, QX25ClearIndication     	*/
{ QAction, 	&QXpbCRe_X_X,           FSM_NOCHANGE },	/* 5, QX25ClearConfirm         	*/
{ QAction, 	&QXpbCRConf_Cleanup_Start,QX25_P1 },	/* 6, QX25ResetIndication	*/
{ QAction,	&QXpbCRConf_Cleanup_Start,QX25_P1 },	/* 7, QX25ResetOutOfOrder       */
{ QAction, 	&QXpbX_Cleanup_Start,   QX25_P1 },	/* 8, QX25ResetConfirm		*/
{ QAction, 	&QXpbCRe_X_X,		FSM_NOCHANGE },	/* 9, QX25Data			*/
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1 },      /* 10,QX25Delete                */
				     			/* Timer Inputs	*/
{ QAction, 	&QXpbX_Cleanup_Start,   QX25_P1 },      /* 11,QX25TimerExpires	  	*/
{ QAction, 	&QXpbX_Cleanup_Start,   QX25_P1 },	/* 12,QX25OutOfOrderTimer       */
};

/* P7/D3 - Virtual Circuit reset - initiated from Remote X.25 device */

const static struct fsm_state QX25PvcP7_D3St[QX25NumInputs] =
{
/*  Action      Param Blk               Next State      Index  Decoded Input            */
/* ------------------------------------------------------------------------------------ */
                                                        /* CLSI Inputs  */
{ QAction,      &QXpbX_RoscL_X,         FSM_NOCHANGE }, /* 0, QX25ReqOpenStnReq         */
{ QAction, 	&QXpbCRConf_ClsCnfK_Start,QX25_P1, },   /* 1, QX25CloseStnReq           */
                                                        /* X.25 Inputs  */
{ QAction,      &QXpbCRe_X_X,           FSM_NOCHANGE }, /* 2, QX25IncomingCall          */
{ QAction,      &QXpbCRe_X_X,           FSM_NOCHANGE }, /* 3, QX25CallConfirm           */
{ QAction,      &QXpbCRe_X_X, 		FSM_NOCHANGE }, /* 4, QX25ClearIndication       */
{ QAction,      &QXpbCRe_X_X, 		FSM_NOCHANGE},  /* 5, QX25ClearConfirm          */
{ QAction,      &QXpbCRConf_X_X,        FSM_NOCHANGE }, /* 6, QX25ResetIndication       */
{ QAction,      &QXpbCRConf_X_X,        FSM_NOCHANGE },	/* 7, QX25ResetOutOfOrder       */
{ QAction,      &QXpbCRe_X_X,           FSM_NOCHANGE }, /* 8, QX25ResetConfirm          */
{ QAction,      &QXpbCRe_X_X,           FSM_NOCHANGE }, /* 9, QX25Data                  */
{ QAction,      &QXpbX_Cleanup_Stop,    QX25_P1      }, /* 10,QX25Delete                */
                                                        /* Timer Inputs */
{ QAction,      &QXpbCRConf_Cleanup_Start,QX25_P1 },    /* 11,QX25TimerExpires          */
{ QAction,      &QXpbCRConf_Cleanup_Start,QX25_P1 },	/* 12,QX25OutOfOrderTimer       */
};

const static fsm_t QX25PvcFsmTable[QX25NumStates] = 
{
    /*  State        	Decoder		*/
      { QX25PvcP1St,	QXFsmDecode },
      { QX25PvcStartSt, QXFsmDecode },
      { QX25PvcP2St,	QXFsmDecode },
      { QX25PvcP3St,	QXFsmDecode },
      { QX25PvcP4_D1St,	QXFsmDecode },
      { QX25PvcP6_D2St,	QXFsmDecode },
      { QX25PvcP7_D3St,	QXFsmDecode },
};

/*
 *****************************************************************************
 *
 *			QLLC CLSI Logical Link FSM
 *
 *
 * This FSM implements the QLLC protocol
 *
 *****************************************************************************
 */


/* QLLC Logical Link FSM Parameter Blocks */

/* 				    X25 Op	Cls Op		Timer	     */


QFsmPB QLpbCR_DiscInd_Stop =	 { OpClearRst,	OpDiscInd,	OpQLTimerStop  };
QFsmPB QLpbDisc_DiscInd_Start =  { OpDISC,	OpDiscInd, 	OpQLTimerStart };
QFsmPB QLpbDisc_RE_Start =  	 { OpDISC,	OpDiscInd, 	OpQLTimerStart };
QFsmPB QLpbDisc_X_Incr = 	 { OpDISC,	OpClsNone, 	OpQLTimerIncr  };
QFsmPB QLpbData_X_X = 		 { OpDlcData, 	OpClsNone, 	OpQTimerNone   };
QFsmPB QLpbDisc_X_Start = 	 { OpDISC, 	OpClsNone, 	OpQLTimerStart };
QFsmPB QLpbDisc_X_Stop =         { OpDISC, 	OpClsNone, 	OpQLTimerStop  };
QFsmPB QLpbDrop_X_X =            { OpDlcDrop,   OpClsNone,      OpQTimerNone   };
QFsmPB QLpbNact_X_X = 		 { OpXNact,	OpClsNone, 	OpQTimerNone   };
QFsmPB QLpbQSM_X_Incr = 	 { OpSetMode,	OpClsNone,	OpQLTimerIncr  };
QFsmPB QLpbRNR_X_X = 		 { OpRNR, 	OpClsNone, 	OpQTimerNone   };
QFsmPB QLpbRR_X_X = 		 { OpRR, 	OpClsNone, 	OpQTimerNone   };
QFsmPB QLpbSM_X_Start = 	 { OpSetMode,	OpClsNone, 	OpQLTimerStart };
QFsmPB QLpbTestR_X_X = 		 { OpTest, 	OpClsNone, 	OpQTimerNone   };
QFsmPB QLpbUA_CtdInd_Stop =      { OpUA,	OpCtdInd,	OpQLTimerStop  };
QFsmPB QLpbUA_DiscConf_Stop =    { OpUA,	OpDiscConf,	OpQLTimerStop  };
QFsmPB QLpbUA_RE_X = 		 { OpUA, 	OpDiscInd, 	OpQTimerNone   };
QFsmPB QLpbUA_X_Start = 	 { OpUA,	OpClsNone,	OpQLTimerStop  };
QFsmPB QLpbUA_X_Stop = 	         { OpUA, 	OpClsNone, 	OpQLTimerStop  };
QFsmPB QLpbX_ConConf_Stop = 	 { OpX25None,   OpConnConf, 	OpQLTimerStop  };
QFsmPB QLpbX_ConInd_Start = 	 { OpX25None,	OpConnInd, 	OpQLTimerStart };
QFsmPB QLpbX_DataInd_X = 	 { OpX25None,	OpDataInd, 	OpQTimerNone   };
QFsmPB QLpbX_DiscConfE_Stop =    { OpX25None,	OpDiscConfE, 	OpQLTimerStop  };
QFsmPB QLpbX_DiscConf_Stop = 	 { OpX25None,	OpDiscConf,	OpQLTimerStop  };
QFsmPB QLpbX_DiscConf_X = 	 { OpX25None,	OpDiscConf, 	OpQTimerNone   };
QFsmPB QLpbX_DiscInd_Start = 	 { OpX25None,	OpDiscInd,	OpQLTimerStart };
QFsmPB QLpbX_DiscInd_Stop = 	 { OpX25None,	OpDiscInd, 	OpQLTimerStop  };
QFsmPB QLpbX_IdInd_Incr = 	 { OpX25None,	OpIdInd,	OpQLTimerIncr  };
QFsmPB QLpbX_IdInd_Start = 	 { OpX25None,	OpIdInd,	OpQLTimerStart };
QFsmPB QLpbX_Nact_X = 		 { OpX25None,	OpLNact, 	OpQLTimerStart };
QFsmPB QLpbX_LE_X =		 { OpX25None,   OpLocalError,	OpQTimerNone   };
QFsmPB QLpbX_RE_X = 		 { OpX25None,   OpLNact,	OpQTimerNone   };
QFsmPB QLpbX_Test_X =		 { OpX25None,   OpTestInd,	OpQTimerNone   };
QFsmPB QLpbX_QuenchOff_X =	 { OpX25None,   OpQuenchOff,	OpQTimerNone   };
QFsmPB QLpbX_QuenchOn_X =	 { OpX25None,   OpQuenchOn,	OpQTimerNone   };
QFsmPB QLpbX_X_Incr =		 { OpX25None,   OpClsNone,	OpQLTimerIncr  };
QFsmPB QLpbX_X_Start =	  	 { OpX25None,   OpClsNone,	OpQLTimerStart };
QFsmPB QLpbX_X_Stop =	  	 { OpX25None,   OpClsNone,	OpQLTimerStop  };
QFsmPB QLpbXid_X_X =		 { OpXid,	OpClsNone,	OpQTimerNone   };

/* QLLC Logical Link FSM State Tables */

/* QLClosed	- Initial state */

const static struct fsm_state QLClosedSt[QLNumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input		*/
/* ------------------------------------------------------------------------------------ */
				    			/* CLSI Inputs	*/
{ QAction,	&QLpbXid_X_X, 		FSM_NOCHANGE },	/*  0 CLSXID		*/
{ QAction, 	&QLpbX_Test_X, 		FSM_NOCHANGE },	/*  1 CLSTest		*/
{ QAction, 	&QLpbSM_X_Start, 	QLLocalOpening },/*  2 ConnReq		*/
{ QAction, 	&QLpbX_LE_X,		FSM_NOCHANGE },	/*  3 ConnRsp		*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  4 SignalStnReq	*/
{ QAction, 	&QLpbX_DiscConf_X,	FSM_NOCHANGE },	/*  5 DiscReq		*/
{ FSM_NOACTION, FSM_NOARG,		FSM_NOCHANGE },	/*  6 DiscRsp		*/
{ QAction,  	&QLpbDrop_X_X, 		FSM_NOCHANGE },	/*  7 DataReq		*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  8 CLSQuenchOn	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  9 CLSQuenchOff	*/
				     			/* X.25 Inputs	*/
{ QAction,  	&QLpbX_IdInd_Start, 	FSM_NOCHANGE },	/* 10 QLXID		*/
{ QAction,  	&QLpbTestR_X_X, 	FSM_NOCHANGE },	/* 11 QLTest		*/
{ QAction, 	&QLpbX_ConInd_Start, 	QLRemoteOpening }, /* 12 QLSM		*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	/* 13 QLUA		*/
{ QAction, 	&QLpbDisc_RE_Start,	FSM_NOCHANGE },	/* 14 QLRR		*/
{ QAction, 	&QLpbUA_RE_X, 		FSM_NOCHANGE },	/* 15 QLDISC		*/
{ QAction, 	&QLpbDisc_RE_Start, 	FSM_NOCHANGE },	/* 16 QLRD		*/
{ QAction, 	&QLpbDisc_RE_Start, 	FSM_NOCHANGE },	/* 17 QLDM		*/
{ QAction, 	&QLpbDisc_RE_Start, 	FSM_NOCHANGE },	/* 18 QLFRMR		*/
{ QAction, 	&QLpbDisc_RE_Start, 	FSM_NOCHANGE },	/* 19 QLData		*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	/* 20 QX25QuenchOn	*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	/* 21 Qx25QuenchOff	*/
							/* Timer Inputs */
{ QT1TimerAct,  &QLpbX_IdInd_Incr, 	FSM_NOCHANGE },	/* 22 QLTimerEvent1	*/
{ QT1N2Action, 	&QLpbCR_DiscInd_Stop,	FSM_NOCHANGE },	/* 23 QLTimerEvent2	*/
};

/* QLLocalOpening	- Received a Connect.Request from CLSI */

const static struct fsm_state QLLocalOpeningSt[QLNumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input	*/
/* ---------------------------------------------------------------------------- */
				    				 /* CLSI Inputs */
{ QAction,	&QLpbNact_X_X, 		FSM_NOCHANGE },	/*  0 CLSXID		*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  1 CLSTest		*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  2 ConnReq		*/
{ QAction, 	&QLpbX_LE_X,		FSM_NOCHANGE },	/*  3 ConnRsp		*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  4 SignalStnReq	*/
{ QAction, 	&QLpbDisc_X_Start, 	QLLocalOpening },/*  5 DiscReq		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/*  6 DiscRsp		*/
{ QAction,  	&QLpbDrop_X_X, 		FSM_NOCHANGE },	/*  7 DataReq		*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  8 CLSQuenchOn	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	/*  9 CLSQuenchOff	*/
				     			/* X.25 Inputs */
{ QAction,  	&QLpbX_Nact_X, 		FSM_NOCHANGE },	/* 10 QLXID		*/
{ QAction,  	&QLpbTestR_X_X, 	FSM_NOCHANGE },	/* 11 QLTest		*/
{ QAction, 	&QLpbUA_X_Start, 	FSM_NOCHANGE },	/* 12 QLSM		*/
{ QAction, 	&QLpbX_ConConf_Stop, 	QLOpened },	/* 13 QLUA		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 14 QLRR		*/
{ QAction, 	&QLpbX_DiscInd_Start,	QLRemoteClosing },/* 15 QLDISC		*/
{ QAction, 	&QLpbX_RE_X, 		QLClosed },	/* 16 QLRD		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 17 QLDM		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 18 QLFRMR		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 19 QLData		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 20 QX25QuenchOn	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	/* 21 Qx25QuenchOff	*/
							/* Timer Inputs */
{ QAction, 	&QLpbQSM_X_Incr,	FSM_NOCHANGE },	/* 22 QLTimerEvent1	*/
{ QAction, 	&QLpbX_DiscInd_Stop, 	FSM_NOCHANGE },	/* 23 QLTimerEvent2	*/
};

/* QLRemoteOpening	Received a QSM from X.25 */

const static struct fsm_state QLRemoteOpeningSt[QLNumInputs] =
{
/*  Action	Param Blk		Next State	   Index  Decoded Input	*/
/* -----------------------------------------------------------------------------*/
				    			   /* CLSI Inputs */
{ QAction,	&QLpbNact_X_X, 		FSM_NOCHANGE },	   /*  0 CLSXID		*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	   /*  1 CLSTest	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	   /*  2 ConnReq	*/
{ QAction, 	&QLpbUA_CtdInd_Stop,	QLOpened },	   /*  3 ConnRsp	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	   /*  4 SignalStnReq	*/
{ QAction, 	&QLpbDisc_X_Start, 	QLLocalOpening },  /*  5 DiscReq	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },    /*  6 DiscRsp	*/
{ QAction,  	&QLpbDrop_X_X, 		FSM_NOCHANGE },	   /*  7 DataReq	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	   /*  8 CLSQuenchOn	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	   /*  9 CLSQuenchOff	*/
				     			   /* X.25 Inputs */
{ QAction,  	&QLpbX_Nact_X, 		FSM_NOCHANGE },	   /* 10 QLXID		*/
{ QAction,  	&QLpbTestR_X_X, 	FSM_NOCHANGE },	   /* 11 QLTest		*/
{ QAction, 	&QLpbX_X_Start, 	FSM_NOCHANGE },	   /* 12 QLSM		*/
{ QAction, 	&QLpbX_X_Stop,	 	QLLocalClosing },  /* 13 QLUA		*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	   /* 14 QLRR		*/
{ QAction, 	&QLpbX_DiscInd_Start,	QLRemoteClosing }, /* 15 QLDISC		*/
{ QAction, 	&QLpbDisc_DiscInd_Start,QLRemoteClosing }, /* 16 QLRD		*/
{ QAction, 	&QLpbDisc_DiscInd_Start, QLClosed },	   /* 17 QLDM		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	   /* 18 QLFRMR		*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	   /* 19 QLData		*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	   /* 20 QX25QuenchOn	*/
{ QAction, 	&QLpbX_RE_X, 		FSM_NOCHANGE },	   /* 21 Qx25QuenchOff	*/
							   /* Timer Inputs */
{ QAction, 	&QLpbX_X_Incr,		FSM_NOCHANGE },	   /* 22 QLTimerEvent1	*/
{ QAction, 	&QLpbX_DiscInd_Stop, 	FSM_NOCHANGE },	   /* 23 QLTimerEvent2	*/
};

/*     QLLocalClosing  -  Received a Disconnect.Request from CLSI		*/

const static struct fsm_state QLLocalClosingSt[QLNumInputs] =
{
/*  Action	Param Blk		Next State	   Index  Decoded Input	*/
/* -----------------------------------------------------------------------------*/
				    			    /* CLSI Inputs */
{ QAction,	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  0 CLSXID	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  1 CLSTest	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  2 ConnReq	*/
{ QAction, 	&QLpbX_LE_X,		FSM_NOCHANGE },	    /*  3 ConnRsp	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  4 SignalStnReq	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  5 DiscReq	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },     /*  6 DiscRsp	*/
{ QAction,  	&QLpbDrop_X_X, 		FSM_NOCHANGE },	    /*  7 DataReq	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  8 CLSQuenchOn	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  9 CLSQuenchOff	*/
				     			    /* X.25 Inputs */
{ QAction,  	&QLpbX_DiscConfE_Stop,	QLClosed     },	    /* 10 QLXID		*/
{ FSM_NOACTION,	FSM_NOARG, 		FSM_NOCHANGE },	    /* 11 QLTest	*/
{ QAction,  	&QLpbX_DiscConfE_Stop,	QLClosed     },	    /* 12 QLSM		*/
{ QAction, 	&QLpbX_DiscConf_Stop, 	QLClosed     },	    /* 13 QLUA		*/
{ QAction,  	&QLpbX_DiscConfE_Stop,	QLClosed     },	    /* 14 QLRR		*/
{ QAction, 	&QLpbUA_DiscConf_Stop, QLClosed     },	    /* 15 QLDISC	*/
{ QAction,  	&QLpbX_DiscConfE_Stop,	QLClosed     },	    /* 16 QLRD		*/
{ QAction, 	&QLpbUA_DiscConf_Stop, QLClosed     },	    /* 17 QLDM		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 18 QLFRMR	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 19 QLData	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 20 QX25QuenchOn	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 21 Qx25QuenchOff	*/
							    /* Timer Inputs */
{ QAction, 	&QLpbDisc_X_Incr, 	FSM_NOCHANGE },	    /* 22 QLTimerEvent1	*/
{ QAction, 	&QLpbX_DiscConfE_Stop,	FSM_NOCHANGE },	    /* 23 QLTimerEvent2	*/
};

/*     QLRemoteClosing	- Remote Station sent QDISC				*/

const static struct fsm_state QLRemoteClosingSt[QLNumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input	*/
/* -----------------------------------------------------------------------------*/
				    			    /* CLSI Inputs */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /*  0 CLSXID	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /*  1 CLSTest	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  2 ConnReq	*/
{ QAction, 	&QLpbX_LE_X,		FSM_NOCHANGE },	    /*  3 ConnRsp	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  4 SignalStnReq	*/
{ QAction, 	&QLpbUA_DiscConf_Stop,	QLClosed     },	    /*  5 DiscReq	*/
{ QAction, 	&QLpbUA_X_Stop,		QLClosed     },	    /*  6 DiscRsp	*/
{ QAction,  	&QLpbDrop_X_X, 		FSM_NOCHANGE },	    /*  7 DataReq	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  8 CLSQuenchOn	*/
{ QAction,  	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  9 CLSQuenchOff	*/
				     			    /* X.25 Inputs */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 10 QLXID		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 11 QLTest	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 12 QLSM		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 13 QLUA		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 14 QLRR		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 15 QLDISC	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 16 QLRD		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 17 QLDM		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 18 QLFRMR	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 19 QLData	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 20 QX25QuenchOn	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 21 Qx25QuenchOff	*/
							    /* Timer Inputs */
{ QAction,	&QLpbX_X_Incr,		FSM_NOCHANGE },	    /* 22 QLTimerEvent1	*/
{ QAction, 	&QLpbUA_X_Stop, 	QLClosed     },	    /* 23 QLTimerEvent2	*/
};

/*     QLOpened		- QSM-QUA exchange has completed.			*/

const static struct fsm_state QLOpenedSt[QLNumInputs] =
{
/*  Action	Param Blk		Next State	Index  Decoded Input	*/
/* -----------------------------------------------------------------------------*/
				    			    /* CLSI Inputs */
{ QAction,	&QLpbNact_X_X,		FSM_NOCHANGE },	    /*  0 CLSXID	*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /*  1 CLSTest	*/
{ QAction, 	&QLpbX_LE_X, 		FSM_NOCHANGE },	    /*  2 ConnReq	*/
{ QAction, 	&QLpbX_LE_X,		FSM_NOCHANGE },	    /*  3 ConnRsp	*/
{ QAction, 	&QLpbRR_X_X, 		FSM_NOCHANGE },	    /*  4 SignalStnReq	*/
{ QAction, 	&QLpbDisc_X_Start,	QLLocalClosing },   /*  5 DiscReq	*/
{ QAction, 	&QLpbDisc_X_Stop,	QLClosed     },	    /*  6 DiscRsp	*/
{ QAction,  	&QLpbData_X_X, 		FSM_NOCHANGE },	    /*  7 DataReq	*/
{ QAction,  	&QLpbRNR_X_X, 		FSM_NOCHANGE },	    /*  8 CLSQuenchOn	*/
{ QAction,  	&QLpbRR_X_X, 		FSM_NOCHANGE },	    /*  9 CLSQuenchOff	*/
				     			    /* X.25 Inputs */
{ QAction,	&QLpbX_Nact_X,		FSM_NOCHANGE },	    /* 10 QLXID		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 11 QLTest	*/
{ QAction,	&QLpbX_DiscInd_Start,	FSM_NOCHANGE },     /* 12 QLSM		*/
{ QAction,	&QLpbX_DiscInd_Start,	FSM_NOCHANGE },     /* 13 QLUA		*/
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },	    /* 14 QLRR		*/
{ QAction, 	&QLpbX_DiscInd_Start,	QLRemoteClosing },  /* 15 QLDISC	*/
{ QAction, 	&QLpbDisc_DiscInd_Start,QLRemoteClosing },  /* 16 QLRD		*/
{ QAction, 	&QLpbX_DiscInd_Start,	FSM_NOCHANGE },     /* 17 QLDM		*/
{ QAction, 	&QLpbX_DiscInd_Start,	FSM_NOCHANGE },     /* 18 QLFRMR	*/
{ QAction,	&QLpbX_DataInd_X,	FSM_NOCHANGE },	    /* 19 QLData	*/
{ QAction,	&QLpbX_QuenchOn_X,	FSM_NOCHANGE },	    /* 20 QX25QuenchOn	*/
{ QAction,	&QLpbX_QuenchOff_X,	FSM_NOCHANGE },	    /* 21 Qx25QuenchOff	*/
							    /* Timer Inputs */
{ QAction,	&QLpbX_X_Stop,		FSM_NOCHANGE },	    /* 22 QLTimerEvent1	*/
{ QAction,	&QLpbX_X_Stop,		FSM_NOCHANGE },	    /* 23 QLTimerEvent2	*/
};


const static fsm_t QLFsmTable[QLNumStates] = 
{
    /*  State        		Decoder		*/
      { QLClosedSt,		QLFsmDecode },
      { QLLocalOpeningSt,	QLFsmDecode },
      { QLRemoteOpeningSt,	QLFsmDecode },
      { QLLocalClosingSt,	QLFsmDecode },
      { QLRemoteClosingSt,	QLFsmDecode },
      { QLOpenedSt,		QLFsmDecode },
};

/* Atomic Test And Set Routines */

/* QLFsmTestAndSetRunning
 *
 * Test and Set for the Logical Link Layer
 */
 
static inline boolean QLFsmTestAndSetRunning (qllctype *qllc)
{
    return(lock_semaphore(&qllc->fQLFsmRunning));
}

/* QXFsmTestAndSetRunning
 *
 * Test and Set for the X.25 Virtual CIrcuit Layer
 */
 
static inline boolean QXFsmTestAndSetRunning (qllctype *qllc)
{
    return(lock_semaphore(&qllc->fQXFsmRunning));
}

/* QLFsmClearRunning
 *
 * leaving the critical region
 */

static inline void QLFsmClearRunning (qllctype *qllc)
   {
   unlock_semaphore(&qllc->fQLFsmRunning);
   return;
   }


/* QXFsmClearRunning
 *
 * leaving the critical region
 */

static inline void QXFsmClearRunning (qllctype *qllc)
   {
   unlock_semaphore(&qllc->fQXFsmRunning);
   return;
   }

/* QLFsmExecute
 *
 * Execute the logical link FSM
 *
 * This is only allowed if the X25 FSM is already connected
 * If Virtual Circuit is not in place we discard the event
 *
 * If it's an input from CLSI Assert Failure
 */
 
void QLFsmExecute (qllctype *qllc, QLInputT event, paktype *pak)
{
    boolean 	runFsm;
    QLStateT	inputState = 0;		/* feed your compiler */

	if (!QX25Up(qllc)) {
	    if (pak) {
		datagram_done(pak);
	    }
	    return;
	}

    QLFsmEventEnqueue(qllc, event, pak);
    
    runFsm = QLFsmTestAndSetRunning(qllc);
    
    if (!runFsm) {
	return;
    } else {
	while ((event = QLFsmEventDequeue(qllc))  != FSM_NO_EVENT) {
if(qllc_state_debug) { 
                char    lcnBuf[8];

    QFsmDbPtr = QFsmDbBuf;
		inputState = qllc->fLState;
                if (qllc->lci) {
                    sprintf(lcnBuf, "%d", qllc->lci->lci_lci);
                } else {
                    sprintf(lcnBuf, "none");
                }
    
		QFsmDbPtr += sprintf(QFsmDbPtr, 
                                    "\nQLLC-LFSM lcn = %s, state %s, input %s: ",
                                    lcnBuf, prQLState(qllc->fLState), prQLEvent(event)); 
}

	    fsm_execute("QLLC Logical FSM", 
			QLFsmTable,(int *) &qllc->fLState, QLNumStates,
			qllc, QLLC_NO_MAJOR, event,			
			qllc_state_debug
			);
	    /* This can call QLFsmExecute if it needs to */

if (qllc_state_debug) {
		if (qllc->fLState != inputState) {
		    QFsmDbPtr += sprintf(QFsmDbPtr, "->%s", 
					 prQLState(qllc->fLState));
		}
    buginf(QFsmDbBuf);
}
	}
	QLFsmClearRunning(qllc);
	mgd_timer_set_type(&qllc->fTimer, (qllc->fX25State == QX25_P4_D1) ? 
			   QLLL_TIMER : QX25_TIMER);
    }
}

/* QXFsmExecute
 *
 * Execute the X.25 FSM
 *
 * This is only allowed if the X25 FSM is alrady connected
 * If Virtual Circuit is not in place we discard the event
 *
 * If it's an input from CLSI which assume X25 level already connected
 * Assert Failure
 */
 
void QXFsmExecute (qllctype *qllc, QXInputT event)
{
    boolean 	runFsm;
    const struct finite_state_machine	*machineTable;
    char	*fsmName;
    QX25StateT	inputState = 0;		/* feed your compiler */
    
    char *fSVCName  = "QLLC SVC FSM";
    char *fPVCName  = "QLLC PVC FSM";


    QXFsmEventEnqueue(qllc, event);

    runFsm = QXFsmTestAndSetRunning(qllc);
    
    if (!runFsm) {
	return;
    } else {
	if (qllc->qllc_ispvc) {
	    machineTable = QX25PvcFsmTable;
	    fsmName = fPVCName;
	} else {
	    machineTable = QX25SvcFsmTable;
	    fsmName = fSVCName;
	}

	while ((event = QXFsmEventDequeue(qllc))  != FSM_NO_EVENT) {
if (qllc_state_debug) {
                char    lcnBuf[8];

    QFsmDbPtr = QFsmDbBuf;
		inputState = qllc->fX25State;
                if (qllc->lci) {
                    sprintf(lcnBuf, "%d", qllc->lci->lci_lci);
                } else {
                    sprintf(lcnBuf, "none");
                }

		QFsmDbPtr += sprintf(QFsmDbPtr, 
                                    "\nQLLC-XFSM lcn = %s, state %s, input %s: ", lcnBuf,
                                    prQXState(qllc->fX25State), prQXEvent(event));
}

	    fsm_execute("QLLC X.25 FSM", 
			machineTable, (int *)&qllc->fX25State, QX25NumStates,
			qllc, QLLC_NO_MAJOR, event,
			qllc_state_debug
			);
	    /* This can call QLFsmExecute if it needs to */

if (qllc_state_debug) {
		if (qllc->fX25State != inputState) {
		    QFsmDbPtr += sprintf(QFsmDbPtr, "->%s", 
					 prQXState(qllc->fX25State));
		}
    buginf(QFsmDbBuf);
}
	}
	QXFsmClearRunning(qllc);
	mgd_timer_set_type(&qllc->fTimer, (qllc->fX25State == QX25_P4_D1) ? 
			   QLLL_TIMER : QX25_TIMER);
        }
}

/* Atomic Action routines for the QLLC FSMs */

/* ---------------------- X.25 Interface Action Routines ---------------------- */

/* Note:  In the first version of QLLC, calls to X.25 went through a registry
 * In fact since qllc requires the X.25 subsystem to make any sense at all
 * this code calls X.25 directly.
 *
 * On the other hand, calls from X.25 to QLLC must go through a registry, since 
 * X.25 cane be used without using QLLC.
 */

/* QCall
 *
 * Place a call;
 *
 * Input assumption:
 * The caller is using either a direct X121, or an address that is known to exist 
 * in the X.121 address map.
 * In practice this means a configured virtual MAC Address
 *
 * If we get here and there is no such map entry we're broken
 */
 
static inline void QCall (qllctype *qllc)
{
    addrtype 	addr;
    lcitype 	*lci;

    if (!QCepTestRemote(qllc)) {
	CLSASSERT(!"No remote address known for outgoing call");
	return;
    }	

    memset(&addr, 0, sizeof(addr));
    addr.type = ADDR_X121;
    addr.length = strlen(qllc->fRemote);
    memcpy(addr.addr.x121_address, qllc->fRemote, addr.length);

    lci = reg_invoke_media_x25_place_call(qllc->idb, 
			  &addr, QPSapGetSubaddress(qllc->fQSap),
			  X25_UNASSIGNED_PREC, LINK_QLLC, NULL, 0);
    if (lci == NULL) {
	QReqOpenStnConf(qllc, CLS_INTERFACE_DOWN);
	return;
    }
	
    QCepBindLci(qllc, lci);
    CepInit(&qllc->fCep, &lci->lci_holdQ, lci->lci_holdQ.maximum, 
	    QLLC_TXQ_LOW_WATER, QLLC_TXQ_HIGH_WATER, QLLC_CLS_FLOW_IND_DAMP);
    return;	
}

/* QAccept
 *
 * Accept Incoming Call
 *
 * Once P3 is a real state there will be real code here
 */
  
static inline void QAccept (qllctype *qllc) {}

/* QClearRst
 *
 * Clear if it's an SVC, Reset if it's a PVC
 */

static inline void QClearRst (qllctype *qllc, uchar diag)
{
    lcitype	*lci;
    leveltype 	level;
    uchar	ptype;

    lci = qllc->lci;
    CLSASSERT(lci);
    if (lci == NULL) {
	return;
    }
    if (qllc->qllc_ispvc) {
	ptype = X25_RESET;
    } else {
	ptype = X25_CLEAR;
    }


    level = raise_interrupt_level(NETS_DISABLE);
    x25_called_from_api = TRUE;

    x25_sendcause(qllc->idb->hwptr, lci, ptype,
		  X25_CLEAR_DTE_ORIGINATED, diag, NULL);

    x25_called_from_api = FALSE;
    reset_interrupt_level(level);
}

/* QClearRstConf
 *
 * Confirm Clear/Reset
 * Once P7/D3 are real states, there will be real code
 */
 
static inline void QClearRstConf (qllctype *qllc) {}

/* QTxXid
 *
 * Given a Generic XID packet, put it in QLLC
 * and forward to the link.
 * This routine is part of the Logical Link Layer (LLL)
 */
 
static inline void QTxXid (qllctype *qllc)
{
    paktype 	*pak;
    boolean     isxid3;
	
    pak = qllc->fPacket;
    qllc->fPacket = NULL;
    CLSASSERT(pak);
    CLSASSERT(qllc->lci);
	
    isxid3 = is_xid3(pak);
	
    pak_delta_adjust_start(pak, -1);
    *pak->datagramstart = QXID;
    if (isxid3 && qllc->xid32_from_remote) {
        datagram_done(qllc->saved_xid3pak);
        qllc->saved_xid3pak = pak_duplicate(pak);
        mgd_timer_start(&qllc->fTimer, qllc->fTimers.fQLTimer);
    }
    qllc_fwd_xid(qllc, pak);
}


/* QXTxNact
 *
 * Given a Generic Non-activation XID packet, put it in QLLC
 * and forward to the link.
 * This routine is part of the Logical Link Layer (LLL)
 */
 
static inline void QXTxNact (qllctype *qllc)
{
    paktype 	*pak;
	
    pak = qllc->fPacket;
    CLSASSERT(pak);
    CLSASSERT(qllc->lci);
    
    if (xid_is_nonactivation(pak->datagramstart)) {
	QTxXid(qllc);
    } else {
	qllc->fPacket = NULL;
	datagram_done(pak);
    }
    return;
}


/* QTxTest
 *
 * Given a TEST request packet, put it in QLLC
 * and forward to the link
 */
 
static inline void QTxTest (qllctype *qllc)
{
    paktype 	*pak;
    qllc_header	*qhdr;
	
    pak = qllc->fPacket;
    qllc->fPacket = NULL;
    qllc->out_of_order_pvc = FALSE;
    CLSASSERT(qllc->lci);
	
    if (pak == NULL) {
	pak = getbuffer(QLLC_HDRSIZE);
	if (pak == NULL) {
	    return;
	}
	pak->datagramsize = QLLC_HDRSIZE;
    }
	
    pak_delta_adjust_start(pak, -QLLC_HDRSIZE);
    qhdr = (qllc_header *)pak->datagramstart;
    pak->x25_flags |= PAK_X25_QBIT;
	
    qhdr->addr = QLLC_CMD;
    qhdr->cntrl = QTEST;
    qllc_psend(qllc, pak);
}

/* QTxSetMode
 *
 * Build control packet (QSM) and get it sent
 */
 
static inline void QTxSetMode (qllctype *qllc)
{	
    CLSASSERT(qllc->lci);
    qllc_sendcontrol(qllc, QSM, TRUE);
    return;
}
	
/* QTxUA
 *
 * Build QUA and get it sent
 */
 
static inline void QTxUA (qllctype *qllc)
{	
    CLSASSERT(qllc->lci);
    qllc_sendcontrol(qllc, QUA, FALSE);
    return;
}
	
/* QTxDisc
 *
 * Build QDISC and get it sent
 */
 
static inline void QTxDisc (qllctype *qllc)
{
    CLSASSERT(qllc->lci);
    qllc_sendcontrol(qllc, QDISC, TRUE);
    return;
}
	
/* QTxRD
 *
 * Build QRD and get it sent
 */
 
static inline void QTxRD (qllctype *qllc)
{
    CLSASSERT(qllc->lci);
    qllc_sendcontrol(qllc, QRD, FALSE);
    return;
}
	
/* QTxDM
 *
 * Build QDM and get it sent
 */
 
static inline void QTxDM (qllctype *qllc)
{
    CLSASSERT(qllc->lci);
    qllc_sendcontrol(qllc, QDM, FALSE);
    return;
}

/* QDlcQuench
 *
 * Exert flow control on the X.25 interface.
 */
 
static inline void QDlcQuench (qllctype *qllc, boolean quenchOn)
{
    qllc_quench(qllc, quenchOn);
    return;
}

/* QDlcDataOut
 *
 * Data packet - fragment if necessary, and give it to qllc_send
 */

static inline void QDlcDataOut (qllctype *qllc)
{
    paktype *pak;

    pak = qllc->fPacket;
    qllc->fPacket = NULL;

    if (pak->datagramsize > qllc->fQMaxBtu) {
          queuetype q;
          if (sna_th_frag(pak, qllc->fQMaxBtu,  &q))
               sendqueue(&q, qllc_send, qllc);
          else{
             buginf("\nQDlcDataOut, getbuffer failure");
             datagram_done(pak);       /* drop it to ground */
	     /* 
	      * **************** NOTE ****************
	      *
	      * Need to clean up here and clsoe the session
	      *
	      * CLEAN UP MEANS: Manufacture a QRD event, and pass
	      * it to the QLFSM
	      *
	      * **************** NOTED ***************
	      */
	 }
 
    } else {
	qllc_send(qllc, pak);
    }
}

/* 
 * QDlcDataDrop
 *
 * Received a Data Packet, but not in a Data forwarding
 * state in the LFSM - since session not up 
 * (dispose of it).
 */
static inline void QDlcDataDrop (qllctype *qllc)
{
    datagram_done(qllc->fPacket);
    qllc->fPacket = NULL;
}

/* ------------------------- CSLI Action Routines ------------------------- */

/* QReqOpenStnConf
 *
 * Reply to Request Open Station Request
 *
 * Take precautions against being called from interrupt level
 */
static inline void QReqOpenStnConf (qllctype *qllc, ClsErrorCodeT error)
 {
    if (qllc) {
        DlcReqOpenStnConf((DlcPCepIdT)qllc, qllc->fUCepId, error);
     if (error != CLS_OK) {
            qllc->fUCepId = (qllctype *) CLS_NO_CEP;
            QCleanup(qllc);
	 }
     }
     return;
 }
 
 
/* CloseStnConf
 *
 * Reply to Close Station Request
 *
 */
 
static inline void CloseStnConf (qllctype *qllc, boolean kill_it)
{
    if (qllc) {
        if (qllc->fUCepId != (qllctype *) CLS_NO_CEP) {
	QllcClsNotify(qllc, CLS_CLOSE_STN_CNF, CLS_OK, NULL);
        }        
        qllc->fUCepId = (qllctype *) CLS_NO_CEP;
        if (kill_it) {
            QCleanup(qllc);
        } 
    }
    return;
}
 
/* QIdInd
 *
 * Indicate an XID, before or after DLU has done
 * Request Open Station Request.
 *
 * This happens in state P3, to indicate an Incoming Call
 * A Request Open Station Request is now expected from the DLU,
 * BUT if the remote X.25 station sends a QXID, this will also
 * be sent to the DLU as an ID Station Indication.  The way to tell
 * the difference is by the fact that qllc->fPacket is populated if
 * We received a QXID.
 *
 * If there is a uCepId, then we use ID.Ind instead of ID.STN.IND
 *
 * This will be cleaned up once we have a real state P3, and
 * the DLU activates the call accept, rather than the X.25 
 * taking matters into its own hands.
 */
 
static inline void QIdInd (qllctype *qllc)
{
    paktype	*pak;
 	
    if (qllc == NULL)
	return;
    
    pak = qllc->fPacket;
    qllc->fPacket = NULL;

    /*
     * If using VMACs, but there is no partner,
     * then there is no sense in trying to send this.
     */
    if (QTestVmac(&qllc->fQCepFlags) && !QTestPartner(&qllc->fQCepFlags)) {
        datagram_done(pak);
    }

    if (pak != NULL) {
	pak_delta_adjust_start(pak, QLLC_HDRSIZE);	/* strip off QLLC header */
    } else {
	pak = getbuffer(0);
	if (pak == NULL) {
	    qllc->qllcStat.qllc_numdrops++;
	    return;
	}
	pak->datagramsize = 0;
    }

    /*
     * QLLC needs to look like NPSI and retransmit XIDs
     * after some timeout.  This is primarily to deal with
     * Comms Manager.
     */
    qllc->xid32_from_remote = (is_xid3(pak) && 
                              (xid_get_node_type(pak->datagramstart) == XID_TYPE_2));

    QllcClsNotify(qllc, CLS_ID_IND, CLS_OK, pak);
    return;
}

/* QLNact
 *
 * Received XID forward to DLU only if it's non-activation
 *
 */
 
static inline void QLNact (qllctype *qllc)
{
    paktype *pak;

    if (qllc == NULL)
	return;
	
    pak = qllc->fPacket;
    
    CLSASSERT(pak != NULL);
    if (xid_is_nonactivation(pak->datagramstart)) {
	QIdInd(qllc);
    } else {
	datagram_done(pak);
	qllc->fPacket = NULL;
    }
    return;
}
 
/* QTestInd
 *
 * Received QTEST - Let DLU know
 *
 */
 
static inline void QTestInd (qllctype *qllc)
{
    paktype *pak;

    if (qllc == NULL)
	return;
	
    pak = qllc->fPacket;
    
    CLSASSERT(pak != NULL);
    
    pak_delta_adjust_start(pak, QLLC_HDRSIZE);	/* strip off QLLC header */		
    QllcClsNotify(qllc, CLS_TEST_IND, CLS_OK, pak);
    return;
}
 

/* QConnInd
 *
 * Received QSM - Let DLU know
 *
 */

static inline void QConnInd (qllctype *qllc)
{
    if (qllc == NULL)
	return;
	
    QllcClsNotify(qllc, CLS_CONNECT_IND, CLS_OK, NULL);
    return;
}

/* QConnConf
 *
 * Received QUA in opening state - Let DLU know
 *
 */

static inline void QConnConf (qllctype *qllc)
{

    if (qllc == NULL)
	return;

    qllc->fVmacReuseOk = TRUE;      /* Only important for vmac from a pool */	
    QllcClsNotify(qllc, CLS_CONNECT_CNF, CLS_OK, NULL);
    return;
}

/* QConnectedInd
 *
 * Received anything from remote station - Let DLU know
 * For qllc this isn't particularly useful since qllc implies
 * that we already havea virtual circuit.
 *
 */

static inline void QConnectedInd (qllctype *qllc)
{
    if (qllc == NULL)
	return;

    qllc->fVmacReuseOk = TRUE;      /* Only important for vmac from a pool */
    QllcClsNotify(qllc, CLS_CONNECTED_IND, CLS_OK, NULL);
    return;
}

/* DiscInd
 *
 * Received QDISC/QRD - Let DLU know
 *
 */

static inline void QDiscInd (qllctype *qllc)
{
    if (qllc == NULL)
	return;
	
    QllcClsNotify(qllc, CLS_DISC_IND, CLS_OK, NULL);
    return;
}


/* QDiscConf
 *
 * Received QUA in closing state - Let DLU know
 *
 */

static inline void QDiscConf (qllctype *qllc, ClsErrorCodeT error)
{
    if (qllc == NULL || qllc->fUCepId == (void *)CLS_NO_CEP)
	return;
	
    QllcClsNotify(qllc, CLS_DISC_CNF, error, NULL);
    return;
}


/* QClsQuench
 *
 * Output to X.25 congested - Let DLU know
 *
 */

static inline void QClsQuench (qllctype *qllc, boolean quenchOn)
{
    if (qllc == NULL)
	return;
	
    CepFlowControlInd(qllc->fUCepId, &qllc->fCep);
    return;
}

/* QDataInd
 *
 * Received ordinary Data - Let DLU know
 *
 */

static inline void QDataInd (qllctype *qllc)
{
    paktype *pak;

    if (qllc == NULL)
	return;
	
    pak = qllc->fPacket;
    qllc->fPacket = NULL;
    
    CLSASSERT(pak != NULL);
    
    QllcClsNotify(qllc, CLS_DATA_IND, CLS_OK, pak);
    return;
}


/* QDataConf
 *
 * Connection went bad, pass packets back to DLU for 
 * possible retransmission on alternative link
 *
 */

static inline void QDataConf (qllctype *qllc)
{

    if (qllc == NULL)
	return;
	

/* MISSING CODE HERE:  PHIL 2/1/95
 * Algorithm: for all untransmitted packets 
 *		Untransmitted: in qllc hold q, or in X.25 lci hold q
 * 	      Build a CLS_DATA_CONF message and submit to CLS
 * Problem is that we probaly don't want to executer all this on an interrupt
 * thread
 */
    return;
}

/*
 * Cleanup QLLC CB.
 */
static inline void QCleanup (qllctype *qllc)
{
    if (!qllc) 
        return;

    QCepReset(qllc);
    if ( (qllc->fUCepId == (void *)CLS_NO_CEP) &&
         (!qllc->qllc_ispvc || (qllc->lci == NULL)) ) {
        /*
         * We are ready to kill this control block.
         * However, in the event that this qllc cb 
         * used a vmac from a pool, and the QSM/QUA
         * exchange never took place, we need to prevent
         * another station from using this vmac for
         * about 30 seconds to prevent a leftover SABME from 
         * going to the wrong device (QLLC end station can get 
         * connected to the wrong VTAM resource! - CSCdi86358).
         */
        if ((qllc->fVmacIndex != NO_VMAC) && (!qllc->fVmacReuseOk)) {
            mgd_timer_start(&qllc->fTimer, QVMAC_TIMEOUT);
        } else {
            mgd_timer_stop(&qllc->fTimer);
            if (qllc->fQSap && !(qllc->qllc_ispvc)) {
                QCepSvcListDelete(qllc->fQSap, qllc);  /* Remove Cep from sap's CepList */
            }
            p_enqueue(&QCepFreeupQ, qllc);  /* Put qllctype on free queue */
        }
    }
    return;
}

/*
 * Cleanup a Zombie qllc cb.
 */
static inline void QZombie (qllctype *qllc)
{
    if (!qllc) 
        return;

    qllc->fVmacReuseOk = TRUE;
    QCleanup(qllc);
    return;
}

static inline void QReportRemoteError (qllctype *qllc)
{
    buginf("\n QLLC: Remote Procedure Error,  do me an error message");
}


static inline boolean QX25Up(qllctype *qllc)
{
    if (qllc == NULL) {
	return (FALSE);
    }
    if (qllc->fX25State == QX25_P4_D1) {
	return (TRUE);
    } else {
	return (FALSE);
    }
}

/* Output routines for debugging */

#define MAXSTRING	20

static char *prClockAction (qllctype *qllc, QTimerT op)
{
    static char output[MAXSTRING];

    switch(op) {

    default: 
	sprintf(output,"?%d?", op);
	break;

    case OpQTimerNone:
	return("-");
    case OpQXTimerStop:
	return("xStop");
	break;
    case OpQLTimerStop:
	return("lStop");

    case OpQXTimerStart:
	sprintf(output, "XGo %d", qllc->fTimers.fX25Timer);
	break;
    case OpQLTimerStart:
	sprintf(output, "LGo %d", qllc->fTimers.fQLTimer);
	break;
    case OpQLTimerIncr:
	sprintf(output, "l+ %d,%d", qllc->fTimers.fQLTimer, qllc->fTimers.fQLRetries);
	break;
    }
    return(output);    
}

static char *const QXopName[QXNops] = {
    "-",
    "CallReq",
    "Accept",
    "ClearRst",
    "ClearRstE",
    "ClearRstConf",
    "XId",
    "XNact",
    "Test",
    "SetMode",
    "UA",
    "DISC",
    "RD",
    "DM",
    "RNR",
    "RR",
    "DlcData",
    "DlcDataDrop"
};

static char *prDlcAction (QX25OpT op)
{
    static char output[MAXSTRING];

    if (op >=  QXNops) {
	sprintf(output,"?%d?", op);
	return(output);    
    } else {
	return(QXopName[op]);
    }
}

static char *const ClsOpName[QClsNops] = {
    "-",
    "ReqOpenStnConf",
    "ReqOpenStnConfL",
    "ReqOpenStnConfR",
    "CloseStnConf",
    "CloseStnCnfK",
    "DiscInd",
    "IdInd",
    "LNact",
    "TestInd",
    "ConnInd",
    "ConnConf",
    "ConnectedInd",
    "DiscConf",
    "DiscConfE",
    "QuenchOn",
    "QuenchOff",
    "DataInd",
    "DataConf",
    "Cleanup",
    "Zombie",
    "LocalError",
    "RemoteError"
};

static char *prClsAction (QClsOpT op)
{
    static char output[MAXSTRING];

    if (op >=  QClsNops) {
	sprintf(output,"?%d?", op);
	return(output);    
    } else {
	return(ClsOpName[op]);
    }
}

static char *const LStateName[QLNumStates+1] = { /* fsm starts counts states at 1 */
    "-",
    "QLClosed",	
    "QLLocalOpening",	
    "QLRemoteOpening",	
    "QLLocalClosing",	
    "QLRemoteClosing",
    "QLOpened"
};

char *prQLState (QLStateT state)
{
    static char output[MAXSTRING];

    if (state < QLClosed || state >=  QLEnd) {
	sprintf(output,"?%d?", state);
	return(output);    
    } else {
	return(LStateName[state]);
    }
}

static char *const XStateName[QX25NumStates+1] = { /* fsm starts counting states at 1 */
    "-",
    "P1",
    "PVC_STARTUP",
    "P2/D2",
    "P3/D3",
    "P4/D1",
    "P6/D2",
    "P7/D3"
};


char *prQXState (QX25StateT state)
{
    static char output[MAXSTRING];

    if (state < QX25_P1 || state >=  QX25_End) {
	sprintf(output,"?%d?", state);
	return(output);    
    } else {
	return(XStateName[state]);
    }
}

static char *const LEventName[QLNumInputs] = {
    "CLSXID",
    "CLSTest",
    "ConnectRequest",
    "ConnectResponse",
    "SignalStationRequest",
    "DisconnectRequest",
    "DisconnectResponse",
    "DataRequest",
    "CLSQuenchOn",
    "CLSQuenchOff",
    "QLXID",
    "QLTest",
    "QLSM",
    "QLUA",   
    "QLRR",
    "QLDISC",
    "QLRD",
    "QLDM",
    "QLFRMR",
    "QLData",
    "QX25QuenchOn",
    "Qx25QuenchOff",
    "QLTimerT1",
    "QLTimerT1N2"    
} ;

static char *prQLEvent (QLInputT event)
{
    static char output[MAXSTRING];

    if (event >=  QLNumInputs) {
	sprintf(output,"?%d?", event);
	return(output);    
    } else {
	return(LEventName[event]);
    }
}

static char *const XEventName[QX25NumInputs] = {
    "QX25ReqOpenStnReq",
    "QX25CloseStnReq",
    "QX25IncomingCall",
    "QX25CallConfirm",
    "QX25ClearIndication",
    "QX25ClearConfirm",
    "QX25ResetIndication",
    "QX25ResetOutOfOrder",
    "QX25ResetConfirm",
    "QX25Data",
    "QX25Deleted",
    "QX25TimerExpires"
    "QX25OutOfOrderTimer"
};    

static char *prQXEvent (QXInputT event)
{
    static char output[MAXSTRING];

    if (event >=  QX25NumInputs) {
	sprintf(output,"?%d?", event);
	return(output);    
    } else {
	return(XEventName[event]);
    }
}


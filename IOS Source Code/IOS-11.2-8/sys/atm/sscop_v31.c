/* $Id: sscop_v31.c,v 3.1.4.7 1996/09/06 16:40:24 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_v31.c,v $
 *--------------------------------------------------------------------
 *
 * S S C O P _ v 3 1 . C
 *
 * used by UNI 3.1 only
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *--------------------------------------------------------------------
 * $Log: sscop_v31.c,v $
 * Revision 3.1.4.7  1996/09/06  16:40:24  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/08/28  12:37:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.5  1996/08/05  22:12:55  jwjang
 * CSCdi65057:  print out err message when sscop detects unmatch uni
 * version
 * Branch: California_branch
 *         CSCdi64992, changed malloc() to malloc_nsmae() in atm_arp.c
 *         fix sscop_rcvQ_cnt and sscop_ackQ_cnt, fixed  no timer running
 *         in  sscop outErrorRecovery state
 *
 * Revision 3.1.4.4  1996/07/29  23:38:56  jwjang
 * CSCdi62882:  SSCOP: when vp-tunnel is shutdown the peers SSCOP remains
 * active
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/07/10  22:20:56  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/19  00:57:05  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/09  14:11:48  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.52.2  1996/05/01  01:30:34  jwjang
 * CSCdi56131:  sscop is in timer_poll when line is idle
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.52.1  1996/04/27  06:34:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.40.1  1996/04/10  23:36:07  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1  1996/03/08  21:59:10  jwjang
 * placeholder. this file is for uni 3.1 signalling
 *
 *
 *
 *--------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "atmsig_private.h"
#include "atmsig_public.h"
#include "sscop_private.h"
#include "sscop_public.h"
#include "mgd_timers.h"




extern paktype *sscop_ackQ[];
extern paktype *sscop_rcvQ[];





/*
 *
 * start/stop timers used by UNI 3.1 code
 */

inline void sscop_startTimerPoll (sscopType *sscop)
{

  mgd_timer_start(&sscop->timerPoll, sscop->pollTimerValue);

}

inline void sscop_startTimerIdle (sscopType *sscop)
{

  mgd_timer_start(&sscop->timerIdle, sscop->idleTimerValue);

}

inline void sscop_startTimerKeepalive (sscopType *sscop)
{

  mgd_timer_start(&sscop->timerKeepalive_v31, sscop->keepaliveTimerValue);

}

inline void sscop_startTimerNoResponse (sscopType *sscop)
{

  mgd_timer_start(&sscop->timerNoResponse, sscop->noResponseTimerValue);

}

inline void sscop_startTimer309 (sscopType *sscop)
{
  if (sscop_debugError)
    buginf("\nSCP: start T309");

    mgd_timer_start(&sscop->t309, sscop->idb->atm_db->timer_t309);

}

inline void sscop_stopTimer309 (sscopType *sscop)
{

  if (sscop_debugError)
    buginf("\nSCP: stop T309");


  mgd_timer_stop(&sscop->t309);

}


inline void sscop_stopTimerPoll (sscopType *sscop)
{

  mgd_timer_stop(&sscop->timerPoll);

}

inline void sscop_stopTimerIdle (sscopType *sscop)
{

  mgd_timer_stop(&sscop->timerIdle);

}

inline void sscop_stopTimerKeepalive (sscopType *sscop)
{

  mgd_timer_stop(&sscop->timerKeepalive_v31);

}

inline void sscop_stopTimerNoResponse (sscopType *sscop)
{

  mgd_timer_stop(&sscop->timerNoResponse);

}

inline boolean sscop_timerPollRunning (sscopType *sscop)
{
  return ( mgd_timer_running(&sscop->timerPoll) );
}

inline boolean sscop_timerIdleRunning (sscopType *sscop)
{
  return ( mgd_timer_running(&sscop->timerIdle) );
}

inline boolean sscop_timerKeepaliveRunning (sscopType *sscop)
{
  return ( mgd_timer_running(&sscop->timerKeepalive_v31) );
}

inline boolean sscop_timerNoResponseRunning (sscopType *sscop)
{
  return ( mgd_timer_running(&sscop->timerNoResponse) );
}

inline boolean sscop_timer309Running (sscopType *sscop)
{
  return ( mgd_timer_running(&sscop->t309) );
}


/*
 *  returns TRUE if this is a UNI 3.1 sscop
 */
inline boolean sscop_isV31 (sscopType *sscop)
{

  return ( (sscop->uni_version == ATMUNI_VERSION_31) ? TRUE: FALSE );
}

inline int sscop_getUniVersion (sscopType *sscop)
{
  return (sscop->uni_version);
}

/*
 * Builds and transmits ER Pdu
 * Returns FALSE if no buffers available - state machine will
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitErPdu (sscopType *sscop)
{

   sscop_errRecPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_errRecPdu) );
   if (!pak) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: no memory to send ER pdu"); 
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_errRecPdu);

   pdu = (sscop_errRecPdu *)pak->datagramstart;

   pdu->type_nmr = SSCOP_MT_ER | (sscop->vmr & SSCOP_SEQNUM_MASK);
   pdu->nsq = sscop->vtSq;

   if (sscop_debugEvent)
      buginf("\nSSCOP(%d): o ER pdu, %s state", sscop->index,
	     sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutErrorRecPdu);

   return (TRUE);

}



/*
 * Builds and transmits recovery Ack Pdu.
 * Returns FALSE if no buffers available - state machine will
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitErAckPdu (sscopType *sscop)
{

   sscop_errRecAckPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_errRecAckPdu) );
   if (!pak) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: no memory to send End Ack pdu"); 
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_errRecAckPdu);

   pdu = (sscop_errRecAckPdu *)pak->datagramstart;

   /* msb - pdu type, remaining three bytes - zero */
   pdu->type_nmr = SSCOP_MT_ERAK | (sscop->vmr & SSCOP_SEQNUM_MASK);
   pdu->res = 0;

   if (sscop_debugEvent)
      buginf("\nSSCOP(%d): o ERAK pdu, %s state", sscop->index,
                               sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutErrorRecAckPdu); 

   return (TRUE);

}


/*
 * return false if bad pdu size or retransmission
 */
int sscop_processErPdu (sscopType *sscop)
{
  paktype *pak;
  int nsq, vms;
  sscop_errRecPdu *pdu;

  pak = (paktype *)sscop->inbuf;
  
  /* check for correct size */
  if ( pak->datagramsize != sizeof(sscop_errRecPdu) ) {
    if (sscop_debugEvent)
      buginf("\nSCPEVNT(%d): i ER Pdu, incorrect size %d", sscop->index,
	     pak->datagramsize);
    return (SSCOP_PDU_BAD);
  }  

  pdu = (sscop_errRecPdu *)pak->datagramstart;
  nsq = pdu->nsq & SSCOP_NSQ_MASK;
  if (nsq == sscop->vrSq) {
    if (sscop_debugEvent)
      buginf("\nSCPEVNT(%d) - i ER Pdu, retransmission",sscop->index);
    return (SSCOP_PDU_RETRANSMIT);
  }  
    
  sscop->vrSq = nsq;
  vms = pdu->type_nmr & SSCOP_SEQNUM_MASK;
  sscop_incVms(vms,sscop);

  SSCOP_STATS(sscop->stats.InErrorRecPdu);

  return (SSCOP_RESULT_OK);
}

/*
 * return false if bad size 
 */
boolean sscop_processErAckPdu (sscopType *sscop)
{
  paktype *pak;
  int nmr;
  sscop_errRecAckPdu *pdu;

  pak = (paktype *)sscop->inbuf;
  
  /* check for correct size */
  if ( pak->datagramsize != sizeof(sscop_errRecAckPdu) ) {
    if (sscop_debugEvent)
      buginf("\nSCPEVNT(%d): i ERAK Pdu, incorrect size %d", sscop->index,
	     pak->datagramsize);
    return (!SSCOP_RESULT_OK);
  }  

  pdu = (sscop_errRecAckPdu *)pak->datagramstart;

  nmr = pdu->type_nmr & SSCOP_SEQNUM_MASK;
  sscop_incVms(nmr,sscop);

  SSCOP_STATS(sscop->stats.InErrorRecAckPdu);

  return (SSCOP_RESULT_OK);
}


/*
 * this function does the common part before entering
 * the data xfer ready state
 */
void sscop_enterDataXferReady (sscopType *sscop)
{
  sscop_stopTimer(sscop);  /* stop Timer_CC */
  if ( sscop_timer309Running(sscop) ) {
    /* recover before T309 expired, tell signalling */
    sscop_stopTimer309(sscop);
    if (sscop_debugEvent)
      buginf("\nSSCOP: AAL-ESTABLISH-INDICATION");
  }
  process_send_message(atmsig_timer_pid, ATMSIG_SSCOP_ESTABLISH, sscop, 0);
  sscop_initStateVars(sscop);
  sscop_setDataXferTimers(sscop);
  sscop->state = SSCOP_ST_DATA_XFER_READY;
}


/*
 * this function goes thru the connection procedure in the incoming connection 
 * pending state for the AA_ESTABLISH_response.
 */
void sscop_makeConnInConnPending (sscopType *sscop)
{
  sscop_clearTransmitter(sscop);
  SSCOP_INIT_VRMR(sscop->vmr);
  /* if fail to send begin ack pdu due to no memory, we can still
     detect the retransmit begin pdu in the data xfer ready state and
     do the right thing, so no need to check the return code */
  sscop_transmitBeginAckPdu(sscop);
  sscop_enterDataXferReady(sscop);
}


/* 
 * aal failure, send aal-release-indication to the signalling 
 */
void sscop_aaReleaseIndication (sscopType *sscop)
{

  if (!sscop_timer309Running(sscop)) {
    if (sscop_debugError || sscop_debugEvent)
      buginf("\nSSCOP(%d): AAL-RELEASE-INDICATION",sscop->index);

    /* send a message to signaling to clear all the non-active svc */
    process_send_message(atmsig_timer_pid, ATMSIG_SSCOP_RELEASE, sscop, 0);
    /* enable T309 */
    sscop_startTimer309(sscop);
  }
}

/*
 * This function doing the necessary process from idle
 * state to outgoing connection state
 */
void sscop_requestConnection (sscopType *sscop)
{
  sscop_clearTransmitter(sscop);
  sscop_resetDataXferTimers(sscop);
  SSCOP_INIT_VRMR(sscop->vmr);
  SSCOP_INC_VTSQ(sscop->vtSq);
  sscop_transmitBeginPdu(sscop);
  sscop_startTimer(sscop, sscop->ccTimerValue);
  sscop->state = SSCOP_ST_OUTCONN_PENDING;
}


/****************************************************************************
 * MAA-ERROR_indication
 *  based on Q.2110
 ****************************************************************************/

/*
 * this function will only be called in data-xfer-ready-state.
 * it handles the error condition which the rcv packets has already
 * been received, in other words, there's a sequence error.
 * this is the function to handle the SDL-D
 */
void sscop_sequenceError (sscopType *sscop, int errInd)
{
  if (sscop_debugError)
    buginf("\nSCPERR(%d) sscop_sequenceError",sscop->index);

  sscop_resetDataXferTimers(sscop);
  sscop_maaErrorIndication(sscop->state, errInd, sscop->index);

  /* sscop->vtCc = 1;  */
  sscop->retryCount = 1; /* use retryCout, not  vtCc for historical reason */
  SSCOP_INC_VTSQ(sscop->vtSq);
  sscop->vmr = SSCOP_RCVWINDOW_DEFAULT;

  sscop_transmitErPdu(sscop);
  sscop_startTimer(sscop, sscop->ccTimerValue);

  /* prepare for recovery */
  sscop_clearTransmitter(sscop);
  sscop->state = SSCOP_ST_OUTRECOVERY_PENDING;

} 


/*
 * this function only prints out the error message for the time being
 */
void sscop_maaErrorIndication (sscop_statetype state, int error, int index)
{
  if (sscop_debugError)
    buginf("\nSCPERR(%d) - %s : %s",index,sscop_printState(state),
	   sscop_printMaaErrorInd(error));
    
}


/*
 * this function will set Timer_POLL, Timer_IDLE, Timer_NO-RESPONSE
 * or Timer_KEEP-ALIVE base upon the sscop timer phase.
 * This function does the SDL (L).
 */
void sscop_setXferReadyTimers (sscopType *sscop)
{
  if (sscop_debugError) {
    if ( sscop->vs < sscop->vms) {  /* tx window is closed */
      if (!sscop->credit) {
	sscop->credit = 1;
	/* sscop_maaErrorIndication(sscop->state,MAA_ERROR_X,sscop->index); */
      }
    }
    else if (sscop->credit) {
      sscop->credit = 0;
      /* sscop_maaErrorIndication(sscop->state,MAA_ERROR_W,sscop->index); */
      sscop->stats.LackOfCredit += 1;
    }
  }

  /* check for Timer_POLL. if it still running, restart Timer_NO-RESPONSE
   * if not but Timer_IDLE is running, let it expires. */
  if ( sscop_timerPollRunning(sscop) ) {
    /* reset Timer_NO-RESPONSE */
    sscop_startTimerNoResponse(sscop);
  }
  else if ( !sscop_timerIdleRunning(sscop) ) {
    /* if both Timer_POLL & Timer_IDLE are not running, Timer_KEEP-ALIVE
       must be running currently. enter the idle phase */
    sscop_stopTimerKeepalive(sscop);
    sscop_stopTimerNoResponse(sscop);
    sscop_startTimerIdle(sscop);
  }

}


/*****************************************************************************
 *  SSCOP MACROS
 *  based on Q.2110
 ****************************************************************************/

/*
 * this function free TxQ, ackQ, and rcvQ. it's
 * also the same as the SDL macro prepare retrieval
 */
void sscop_releaseBuffers (sscopType *sscop) 
{
  paktype *pak;

  /* clear tx queue & buffers */
  while ( (pak = pak_dequeue(&sscop->txQ) ) != NULL)
    datagram_done(pak);

  /* clear retransmission queue */
   sscop_removeAllFromAckQ(sscop); 
   sscop_ackQ[sscop->index] = (paktype *)NULL;

  /* clear receiver buffer */
   sscop_removeAllFromRcvQ(sscop);
   sscop_rcvQ[sscop->index] = (paktype *)NULL;



}


/*
 * clear txQ and AckQ. it can also be used
 * as prepare recovery macro.
 */
void sscop_clearTransmitter (sscopType *sscop)
{
  paktype *pak;

  /* clear transmission queue */
  while ( (pak = pak_dequeue(&sscop->txQ) ) != NULL)
    datagram_done(pak);

  sscop_removeAllFromAckQ(sscop); 
  sscop_ackQ[sscop->index] = (paktype *)NULL;
}

/*
 * clear the rcvQ
 */
void sscop_deliverData (sscopType *sscop) 
{

  /* clear receiver buffer */
   sscop_removeAllFromRcvQ(sscop);
   sscop_rcvQ[sscop->index] = (paktype *)NULL;
}

/*
 * vrSq and vtSq are not reset here
 */
void sscop_initStateVars (sscopType *sscop)
{
   sscop->begin_rej_count =
   sscop->vs         = 
   sscop->vps        = 
   sscop->vta        = 
   sscop->vtPd       = 
   sscop->vr         = 
   sscop->poll_vps   =
   sscop->vrh        = 0;

   sscop->credit     =
   sscop->vtPa       = 1;

}


void sscop_dataRetrieval (sscopType *sscop)
{

}


/*
 * this function checks N(SQ) in the rcv Pdu for the retransmission.
 * N(SQ) is in BEGIN, RS, and ER PDUs. In these 3 different PDUs,
 * N(SQ) is at the same place and all of them have the same size.
 * Therefore, instead of creating a new data structure, sscop_beginPdu
 * can be used just fine.
 * It returns TRUE if this is a retransmission. VR(SQ) will be updated
 * if this is NOT a retransmission.
 */
boolean sscop_detectRetransmission (sscopType *sscop)
{
   sscop_beginPdu *pdu;
   paktype *pak;
   ulong nsq;

   pak = (paktype *)sscop->inbuf;
   pdu = (sscop_beginPdu *)pak->datagramstart;

   nsq = pdu->nuu_nsq & SSCOP_NSQ_MASK;
   if ( nsq == sscop->vrSq )
     return (TRUE);

   sscop->vrSq = nsq;

   return (FALSE);
}

/*
 * this function start Timer_POLL or Timer_KEEP-ALIVE
 * base upon the current sscop timer phase
 */
void sscop_setPollTimer (sscopType *sscop)
{

  if ( (sscop->txQ.qhead) || (sscop->vs != sscop->vta) )
    sscop_startTimerPoll(sscop);
  else
    sscop_startTimerKeepalive(sscop);

}


void sscop_resetDataXferTimers (sscopType *sscop)
{
  sscop_stopTimerPoll(sscop);
  sscop_stopTimerKeepalive(sscop);
  sscop_stopTimerNoResponse(sscop);

  /* why do not stop timer idle ?????  */
  sscop_stopTimerIdle(sscop);
}

void sscop_setDataXferTimers (sscopType *sscop)
{

  sscop_startTimerPoll(sscop);
  sscop_startTimerNoResponse(sscop);

}








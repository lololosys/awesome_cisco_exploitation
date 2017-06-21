/* $Id: sscop_ui.c,v 3.5.18.3 1996/08/05 22:12:52 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_ui.c,v $
 *----------------------------------------------------------------------
 *
 * S S C O P _ U I . C
 *
 * Service Specific Connection Oriented Protocol (SSCOP) User Interface 
 *
 * November 1993,    Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *----------------------------------------------------------------------
 * $Log: sscop_ui.c,v $
 * Revision 3.5.18.3  1996/08/05  22:12:52  jwjang
 * CSCdi65057:  print out err message when sscop detects unmatch uni
 * version
 * Branch: California_branch
 *         CSCdi64992, changed malloc() to malloc_nsmae() in atm_arp.c
 *         fix sscop_rcvQ_cnt and sscop_ackQ_cnt, fixed  no timer running
 *         in  sscop outErrorRecovery state
 *
 * Revision 3.5.18.2  1996/06/21  02:08:39  jwjang
 * CSCdi60829:  unknow ie in connect messae
 *         also added txQ count to show sscop
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/05/09  14:11:46  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.48.1  1996/04/27  06:34:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.38.1  1996/04/10  23:36:05  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5  1996/02/07  16:10:11  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:00:00  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:26:28  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:47:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/18  14:23:26  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.2  1995/07/26  17:36:03  percyk
 * CSCdi34916:  sscop interface commands cause SegV exception during
 * initialization
 *
 * Revision 2.1  1995/06/07  20:13:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *----------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../if/atm.h"
#include "parser_defs_sscop.h"
#include "sscop_private.h"
#include "sscop_public.h"
#include "mgd_timers.h"





extern ulong sscop_ackQ_cnt;
extern ulong sscop_rcvQ_cnt;

/*
 *
 *    C o  n  f  i  g  u  r  a  t  i  o  n       S  e  c  t  i  o  n
 *
 */

/*
 * s s c o p _ i n t e r f a c e _ c o m m a n d s ( )
 *
 *
 * The default values used by sscop are adequate for most networks - i.e.  sscop
 * does not require to be configured in most cases - i.e it's ON by default.  
 * However the following config commands can be specified if the default values
 * are not adequate for the user.
 *
 * Exec command -
 * ==== =======
 *
 * [no] sscop enable
 *
 * Sscop is on by default on all ATM ports.  If the user wishes to disable sscop
 * for any reason the global command "no sscop enable" should be issued.  Note that
 * issuing this command will inhibit establishment of new sscop sessions on ALL
 * ATM ports.  Furthermore active sscop sessions will be pulled down.  To reenable
 * sscop (for ALL Atm ports) the global command "sscop enable" should be issued.
 *
 *
 * Interface specific configuration commands -
 * ========= ======== ============= ========
 *
 * [no] sscop poll-timer <time in milliseconds>
 *
 * The maximum time between transmissions of a Poll Pdu, when SD or SDP Pdu's are
 * queued for transmissions or are outstanding pending acknowledgment.
 *
 *
 * [no] sscop keepalive-timer <time in milliseconds>
 *
 * The maximum time between transmission of a Poll Pdu, when no SD or SDP Pdu's
 * are queued for transmission or are outstanding pending acknowledgment.  (In
 * general configure keepalive-timer >= poll-timer).
 *
 *
 * [no] sscop connection-control-timer <time in milliseconds>
 *
 * The connection control timer determines the time between retransmissions of Begin,
 * End or Resync Pdu's, as long as an acknowledgment PDU has not been received.
 * This timer should be set to a value greater than the roundtrip delay.
 *
 *
 * [no] sscop maxCC <number>
 *
 * Maximum number of retransmissions of a Begin, End or a Resync Pdu when no ack
 * has been received.  The session is pulled down once the retry count is exceeded.
 *
 *
 * [no] sscop send-window-size <number>
 *
 * Maxmimum number of Sd Pdu's sent before a Poll Pdu is transmitted.
 *
 *
 * [no] sscop receive-window-size <number>
 *
 * Maximum number of frames a remote station can send before waiting for ack.
 * This is the credit granted by the SSCOP receiver to allow the peer SSCOP
 * transmitter to transmit new data Pdu's.
 *
 *
 * [no] sscop maxStat <number>
 *
 * Maximum number of list elements permissible in a STAT Pdu.  (A STAT message can
 * be segmented into multiple STAT Pdu's if a single STAT Pdu becomes too large).
 *
 * Prefixing a "no" to any of the interface specific config commands would revert
 * back to the default value for that config command.
 *
 *
 * INPUT:
 *      csb = ptr to the parse block with command & arguments.
 *      csb->which = SSCOP command type
 *      csb->sense = [NO] value(0 = NO), (1 = !NO)
 *
 */
void sscop_interface_commands (parseinfo *csb)
{

   idbtype *swidb;
   hwidbtype *hwidb;
   atm_db_t *aip;
   sscopType *sscop, **sscop_ptr;
   ulong keepaliveTimerDefault;

   swidb = csb->interface;
   hwidb = swidb->hwptr;
   aip = hwidb->atm_db;
   sscop_ptr = &swidb->hwptr->sscop;
   sscop = *sscop_ptr;

   if (!sscop)
      return;

   if ( sscop_isV31(sscop) )
     keepaliveTimerDefault = SSCOP_KEEPALIVE_V31_DEFAULT;
   else
     keepaliveTimerDefault = SSCOP_KEEPALIVETIMER_DEFAULT;

   /*
    * csb->nvgen == TRUE when the 'write [term || mem || net]' is entered.
    * Otherwise, it's false to indicate that a command is being specified
    * by the user.
    */
   if (csb->nvgen) {

      switch (csb->which) {

         case SSCOP_POLL_TIMER_T:
            nv_write(sscop->pollTimerValue != SSCOP_POLLTIMER_DEFAULT, "%s %d",
                        csb->nv_command, sscop->pollTimerValue);
            break;

         case SSCOP_KEEPALIVE_TIMER_T:
            nv_write(sscop->keepaliveTimerValue != keepaliveTimerDefault, 
                             "%s %d", csb->nv_command, sscop->keepaliveTimerValue);
            break;

         case SSCOP_CC_TIMER_T:
            nv_write(sscop->ccTimerValue != SSCOP_CCTIMER_DEFAULT, "%s %d",
                        csb->nv_command, sscop->ccTimerValue);
            break;

         case SSCOP_MAX_CC:
            nv_write(sscop->maxCC != SSCOP_MAXCC_DEFAULT, "%s %d",
                        csb->nv_command, sscop->maxCC);
            break;

         case SSCOP_SEND_WINDOW:
            nv_write(sscop->sendWindow != SSCOP_SENDWINDOW_DEFAULT, "%s %d",
                        csb->nv_command, sscop->sendWindow);
            break;

         case SSCOP_RCV_WINDOW:
            nv_write(sscop->rcvWindow != SSCOP_RCVWINDOW_DEFAULT, "%s %d",
                        csb->nv_command, sscop->rcvWindow);
            break;

         case SSCOP_MAX_STAT:
            nv_write(sscop->maxStat != SSCOP_MAX_LIST_ELEMENTS, "%s %d",
                        csb->nv_command, sscop->maxStat);
            break;

         case SSCOP_QUICK_POLL:
	    nv_write(sscop->quickPoll == TRUE, "%s", csb->nv_command);
	    break;


         default:
            bad_parser_subcommand(csb, csb->which);

            break;

      }
      return;

   }

   switch (csb->which) {

      case SSCOP_POLL_TIMER_T:
         sscop->pollTimerValue = (csb->sense ? GETOBJ(int, 1) :
                                                 SSCOP_POLLTIMER_DEFAULT);
	 if ( sscop_isV31(sscop) ) {
	   if (sscop->keepaliveTimerValue <= sscop->pollTimerValue) {
	     sscop->keepaliveTimerValue = sscop->pollTimerValue + (4 * ONESEC);
	     sscop->idleTimerValue = sscop->pollTimerValue + (9 * ONESEC);
	     sscop->noResponseTimerValue = sscop->pollTimerValue + (29 * ONESEC);
	   }
	 }

         break;

      case SSCOP_KEEPALIVE_TIMER_T:
	 if ( sscop_isV31(sscop) ) {
	   sscop->keepaliveTimerValue = (csb->sense ? GETOBJ(int, 1) :
					 SSCOP_KEEPALIVE_V31_DEFAULT);
	   if (sscop->keepaliveTimerValue == SSCOP_KEEPALIVETIMER_DEFAULT)
	     sscop->keepaliveTimerValue = SSCOP_KEEPALIVE_V31_DEFAULT;
	   sscop->idleTimerValue = sscop->keepaliveTimerValue + (5 * ONESEC);
	   sscop->noResponseTimerValue = sscop->keepaliveTimerValue + (25 * ONESEC);
	   if (sscop->pollTimerValue >= sscop->keepaliveTimerValue) {
	     if (sscop->keepaliveTimerValue > 4)
	       sscop->pollTimerValue = sscop->keepaliveTimerValue - (4 * ONESEC);
	     else if (sscop->keepaliveTimerValue > 1)
	       sscop->pollTimerValue = sscop->keepaliveTimerValue - (ONESEC);
	     else
	       sscop->pollTimerValue = sscop->keepaliveTimerValue;
	   }
	 }
	 else
	   sscop->keepaliveTimerValue = (csb->sense ? GETOBJ(int, 1) :
                                                 SSCOP_KEEPALIVETIMER_DEFAULT);


         break;

      case SSCOP_CC_TIMER_T:
         sscop->ccTimerValue = (csb->sense ? GETOBJ(int, 1) : SSCOP_CCTIMER_DEFAULT);
         break;

      case SSCOP_MAX_CC:
         sscop->maxCC = (csb->sense ? GETOBJ(int, 1) : SSCOP_MAXCC_DEFAULT);
         break;

      case SSCOP_SEND_WINDOW:
         sscop->sendWindow = (csb->sense ? GETOBJ(int, 1) :
                                                 SSCOP_SENDWINDOW_DEFAULT);
         break;

      case SSCOP_RCV_WINDOW:
         sscop->rcvWindow = (csb->sense ? GETOBJ(int, 1) :
                                                 SSCOP_RCVWINDOW_DEFAULT);
         break;

      case SSCOP_MAX_STAT:
         sscop->maxStat = (csb->sense ? GETOBJ(int, 1) : 
                                                 SSCOP_MAX_LIST_ELEMENTS);
         break;

      case SSCOP_QUICK_POLL:
	 if (csb->sense) 
	   sscop->quickPoll = TRUE;
	 else 
	   sscop->quickPoll = FALSE; 
	 break;


      default:
        bad_parser_subcommand(csb, csb->which);
        break;

   }

}



/*
 * s s c o p _ g l o b a l R e s e t S t a t s ( )
 *
 * Resets sscop's statistics counters for all ATM interfaces.  Session 
 * establishment or termination will not clear/reset statistics counters.
 * Only the following exec command will clear sscop statistics -
 *
 * sscop reset-stats global 
 */
void sscop_globalResetStats (void)
{

   hwidbtype *hwidb;


   FOR_ALL_HWIDBS(hwidb) {

      /* sscop only for ATM interfaces */
      if ( !(hwidb->status & IDB_ATM) )
         continue;

      sscop_interfaceResetStats(hwidb);

   }

}



/*
 * s s c o p _ i n t e r f a c e R e s e t S t a t s ( )
 *
 * Resets sscop's statistics counters for a given ATM interface.  Session
 * establishment or termination will not clear/reset statistics counters. 
 * Only the following exec command will clear sscop statistics -
 *
 * sscop reset-stats <interface>
 *
 * Since this routine could be called directly from exec or indirectly from 
 * sscop_globalResetStats(), checking for a valid idb/sscop is essential. 
 */
void sscop_interfaceResetStats (hwidbtype *hwidb)
{

   sscopType *sscop;


   /* sscop only for ATM interfaces */
   if ( !(hwidb->status & IDB_ATM) )
      return;

   /* get the sscop control block, if it exists */
   if (!hwidb->sscop) {
      /* fatal error - every ATM idb is allocated a sscop at startup time */
      return;
   }

   sscop = (sscopType *)hwidb->sscop;
   sscop_initStatistics(sscop);

}



/*
 * s s c o p _ e n a b l e S s c o p ( )
 *
 * Enable SSCOP.  SSCOP is enabled by default - this routine is used to
 * reenable SSCOP after it was disabled by the exec command - "no sscop enable".
 * The exec command "sscop enable" will enable sscop by invoking this routine.
 */
void sscop_enableSscop (void)
{

   sscopEnabled = TRUE;

}



/*
 * s s c o p _ d i s a b l e S s c o p ( )
 *
 * Disables SSCOP.  SSCOP is enabled by default.  Note that disabling SSCOP
 * will pull down existing connections and inhibit establishment of new ones.
 */
void sscop_disableSscop (void)
{

   sscopEnabled = FALSE;

   /* Also pull down all active connections */


}



/*
 *
 *   D i s p l a y     S e c t i o n
 *
 */

/*
 * s s c o p _s h o w _ c o m m a n d ( )
 *
 * Displays the Sscop control block.
 */
void sscop_show_command (parseinfo *csb)
{

   sscopType *sscop;
   hwidbtype *hwidb;


   FOR_ALL_HWIDBS(hwidb) {

      /* sscop only for ATM interfaces */
      if ( !(hwidb->status & IDB_ATM) )
         continue;

      sscop = hwidb->sscop;

      if (sscop)
         sscop_showCommand(sscop);

   } 

}



/*
 * s s c o p _ s h o w C o m m a n d ( )
 *
 */
void sscop_showCommand (sscopType *sscop)
{


  atm_db_t *atm;

  /* be paranoid */
  if(!sscop->idb) {
    if (sscop_debugError)
      buginf("SSCOP: sscop_showCommand, no sscop->idb");
    return;
  }

  atm = sscop->idb->atm_db;

   printf("\nSSCOP details for interface %s", sscop->idb->hw_namestring);
   printf("\n   Current State = %s,   Uni version = %s", 
	  sscop_printState(sscop->state),
	  (sscop->uni_version == ATMUNI_VERSION_31) ?
          "3.1" : (sscop->uni_version == ATMUNI_VERSION_30) ?
          "3.0" : "Invalid");
   printf("\n   Send Sequence Number: Current = %d,  Maximum = %d",
                         sscop->vs, sscop->vms);
   printf("\n   Send Sequence Number Acked = %d", sscop->vta);
   printf("\n   Rcv Sequence Number: Lower Edge = %d, Upper Edge = %d, Max = %d",
                         sscop->vr, sscop->vrh, sscop->vmr);
   printf("\n   Poll Sequence Number = %d, Poll Ack Sequence Number = %d",
                         sscop->vps, sscop->vtPa); 
   printf("\n   Vt(Pd) = %d", sscop->vtPd);
   printf("\n   Connection Control:  timer = %d", sscop->ccTimerValue, sscop->vtCc);
   if ( !sscop_isV31(sscop) )
     printf("\n   Timer currently %s", mgd_timer_running(&sscop->timeoutValue) ? "Active":"Inactive");   
   printf("\n   Timer_Keepalive = %d", sscop->keepaliveTimerValue);
   if ( sscop_isV31(sscop) ) 
     printf("\n   Timer_Idle = %d,  Timer_NoResponse = %d",sscop->idleTimerValue,
	    sscop->noResponseTimerValue);
   printf("\n   Current Retry Count = %d, Maximum Retry Count = %d",
                         sscop->retryCount, sscop->maxRetryCount);
   /* Get rid of Q counts until they are verified accurate */
   printf("\n   AckQ count = %d, RcvQ count = %d, TxQ count = %d", 0,0,0);
   printf("\n   Local connections currently pending = %d",
                                              atm->conn_pending);
   printf("\n   Max local connections allowed pending = %d",
                                              atm->max_conn_pending);
   printf("\n   Statistics -");
   printf("\n      Pdu's Sent = %d, Pdu's Received = %d, Pdu's Ignored = %d",
                         sscop->stats.Pdureceived, sscop->stats.Pdutransmitted, 
                         sscop->stats.Ignored);
   printf("\n      Begin = %d/%d, Begin Ack = %d/%d, Begin Reject = %d/%d",
                         sscop->stats.InBeginPdu, sscop->stats.OutBeginPdu,
                         sscop->stats.InBeginAckPdu, sscop->stats.OutBeginAckPdu,
                         sscop->stats.InBeginRejPdu, sscop->stats.OutBeginRejPdu); 
   printf("\n      End = %d/%d, End Ack = %d/%d",
                         sscop->stats.InEndPdu, sscop->stats.OutEndPdu,
                         sscop->stats.InEndAckPdu, sscop->stats.OutEndAckPdu); 
   printf("\n      Resync = %d/%d, Resync Ack = %d/%d",
                         sscop->stats.InResyncPdu, sscop->stats.OutResyncPdu,
                         sscop->stats.InResyncAckPdu, sscop->stats.OutResyncAckPdu);
   printf("\n      Sequenced Data = %d/%d, Sequenced Poll Data = %d/%d", 
                         sscop->stats.InSdPdu, sscop->stats.OutSdPdu,
                         sscop->stats.InSdpPdu, sscop->stats.OutSdpPdu); 
   printf("\n      Poll = %d/%d, Stat = %d/%d, Unsolicited Stat = %d/%d",
                         sscop->stats.InPollPdu, sscop->stats.OutPollPdu,
                         sscop->stats.InStatPdu, sscop->stats.OutStatPdu,
                         sscop->stats.InUstatPdu, sscop->stats.OutUstatPdu);
   printf("\n      Unassured Data = %d/%d, Mgmt Data = %d/%d, Unknown Pdu's = %d", 
                         sscop->stats.InUdPdu, sscop->stats.OutUdPdu,
                         sscop->stats.InMdPdu, sscop->stats.OutMdPdu,
                         sscop->stats.InUnknownPdu);
}



/*
 * s s c o p _ p r i n t S t a t e ( )
 *
 * Text print of Sscop state.
 */
char *sscop_printState (sscop_statetype state)
{

   switch (state) {

      case SSCOP_ST_IDLE:
         return ("Idle");

      case SSCOP_ST_OUTCONN_PENDING:
         return ("Outgoing Connection Pending");

      case SSCOP_ST_INCONN_PENDING:
         return ("Incoming Connection Pending");

      case SSCOP_ST_DATA_XFER_READY:
         return ("Active");

      case SSCOP_ST_OUTDISCONN_PENDING:
         return ("Outgoing Disconnect Pending");

      case SSCOP_ST_OUTRESYNC_PENDING:
         return ("Outgoing Resync Pending");

      case SSCOP_ST_INRESYNC_PENDING:
         return ("Incoming Resync Pending");

      case SSCOP_ST_CONRESYNC_PENDING:
         return ("Concurrent Resync Pending");

      default:
         return ("Invalid");
   }

}



/*
 * s s c o p _ p r i n t P d u T y p e ( )
 *
 * Text print of Pdu type.
 */
char *sscop_printPduType (ulong type, ulong uni_version)
{

   switch (type & SSCOP_PDUTYPE_MASK) {

      case SSCOP_MT_BGN:
         return ("Begin");

      case SSCOP_MT_BGAK:
         return ("Begin Ack");

      case SSCOP_MT_END:
         return ("End");

      case SSCOP_MT_ENDAK:
         return ("End Ack");

      case SSCOP_MT_RS:
         return ("Resync");

      case SSCOP_MT_RSAK:
         return ("Resync Ack");

      case SSCOP_MT_BGREJ:
         return ("Begin Reject");

      case SSCOP_MT_SD:
         return ("Sd");

      case SSCOP_MT_SDP:
	 if (uni_version == ATMUNI_VERSION_31)
	   return ("Recovery");
	 else
	   return ("Sdp");

      case SSCOP_MT_POLL:
         return ("Poll");

      case SSCOP_MT_STAT:
         return ("Stat");

      case SSCOP_MT_USTAT:
         return ("Ustat");

      case SSCOP_MT_UD:
         return ("Unsequenced Data");

      case SSCOP_MT_MD:
         return ("Management Data");

       case SSCOP_MT_ERAK:
	 return ("RecoveryAck");

      default:
         return ("Invalid");

   }

}




char *sscop_printMaaErrorInd (int errInd)
{
  switch (errInd) {
  case MAA_ERROR_A:
    return ("maaError-A");

  case MAA_ERROR_B:
    return ("maaError-B");

  case MAA_ERROR_C:
    return ("maaError-C");

  case MAA_ERROR_D:
    return ("maaError-D");

  case MAA_ERROR_E:
    return ("maaError-E");

  case MAA_ERROR_F:
    return ("maaError-F");

  case MAA_ERROR_G:
    return ("maaError-G");

  case MAA_ERROR_H:
    return ("maaError-H");

  case MAA_ERROR_I:
    return ("maaError-I");

  case MAA_ERROR_J:
    return ("maaError-J");

  case MAA_ERROR_K:
    return ("maaError-K");

  case MAA_ERROR_L:
    return ("maaError-L");

  case MAA_ERROR_M:
    return ("maaError-M");

  case MAA_ERROR_O:
    return ("maaError-O");

  case MAA_ERROR_P:
    return ("maaError-P");

  case MAA_ERROR_Q:
    return ("maaError-Q");

  case MAA_ERROR_R:
    return ("maaError-R");

  case MAA_ERROR_S:
    return ("maaError-S");

  case MAA_ERROR_T:
    return ("maaError-T");

  case MAA_ERROR_U:
    return ("maaError-U");

  case MAA_ERROR_V:
    return ("maaError-V");

  case MAA_ERROR_W:
    return ("maaError-W");

  case MAA_ERROR_X:
    return ("maaError-X");

  default:
    return ("maaErrorUnknown");

  }
}



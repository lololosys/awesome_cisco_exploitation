/* $Id: sscop.c,v 3.3.10.17 1996/09/06 16:40:17 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop.c,v $
 *--------------------------------------------------------------------
 *
 * S S C O P . C
 *
 * Service Specific Connection Oriented Protocol (SSCOP) support
 *
 * November 1993,    Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *--------------------------------------------------------------------
 * $Log: sscop.c,v $
 * Revision 3.3.10.17  1996/09/06  16:40:17  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.3.10.16  1996/08/28  12:37:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.10.15  1996/08/15  22:07:50  jwjang
 * CSCdi66149:  clear interface atm causes unmatch uni version between peer
 * Branch: California_branch
 *
 * Revision 3.3.10.14  1996/08/14  17:10:20  jwjang
 * CSCdi54375:  ATMSIG: memory exhaustion 11.1(1.083) results in a crash
 * Branch: California_branch
 *
 * Revision 3.3.10.13  1996/08/09  16:28:44  jwjang
 * CSCdi56495:  uni 3.1 sscop sometimes send 2 poll pdus back to back
 * Branch: California_branch
 *
 * Revision 3.3.10.12  1996/08/05  22:12:43  jwjang
 * CSCdi65057:  print out err message when sscop detects unmatch uni
 * version
 * Branch: California_branch
 *         CSCdi64992, changed malloc() to malloc_nsmae() in atm_arp.c
 *         fix sscop_rcvQ_cnt and sscop_ackQ_cnt, fixed  no timer running
 *         in  sscop outErrorRecovery state
 *
 * Revision 3.3.10.11  1996/08/01  00:35:15  jwjang
 * CSCdi64555:  atmsig is running while no qsaal pvc
 *         CSCdi64664
 * Branch: California_branch
 *
 * Revision 3.3.10.10  1996/07/29  23:38:46  jwjang
 * CSCdi62882:  SSCOP: when vp-tunnel is shutdown the peers SSCOP remains
 * active
 * Branch: California_branch
 *
 * Revision 3.3.10.9  1996/07/23  13:20:12  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.10.8  1996/07/10  22:20:53  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.10.7  1996/07/02  21:34:51  jwjang
 * CSCdi61712:  ATM SIG Cause Router core dump
 * Branch: California_branch
 *
 * Revision 3.3.10.6  1996/06/19  00:56:58  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.3.10.5  1996/05/21  21:06:14  jwjang
 * CSCdi58013:  sscop should send end pdu when shut atm interface
 * Branch: California_branch
 *
 * Revision 3.3.10.4  1996/05/21  16:44:13  jwjang
 * CSCdi57875:  show int atm display wrong uni version
 * Branch: California_branch
 *
 * Revision 3.3.10.3  1996/05/11  19:31:42  jwjang
 * CSCdi57376:  sscop keepalive timer is 30 sec in uni 3.1 sometimes
 * Branch: California_branch
 *
 * Revision 3.3.10.2  1996/05/09  14:11:40  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.10.1.18.2  1996/05/01  01:30:29  jwjang
 * CSCdi56131:  sscop is in timer_poll when line is idle
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.10.1.18.1  1996/04/27  06:34:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.10.1.8.1  1996/04/10  23:35:59  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.10.1  1996/03/18  19:00:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:28:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  08:30:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:35:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/22  22:46:37  schrupp
 * CSCdi49275:  sscop does not get initialized properly
 *
 * Revision 3.2  1995/11/17  08:47:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/08/31  20:56:37  schrupp
 * CSCdi39665:  Stop SSCOP timer in default case
 *
 * Revision 2.11  1995/08/18  14:23:22  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.10  1995/08/10  00:59:58  awu
 * CSCdi37887:  ATM OAM cells are transmitted and received incorrect.
 *
 * Revision 2.9  1995/08/08  16:45:34  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.8  1995/08/08 01:06:25  percyk
 * CSCdi38366:  move processes to atmsig
 *
 * Revision 2.7  1995/08/07  19:14:30  percyk
 * CSCdi38338:  q messages on the correct output process.
 *
 * Revision 2.6  1995/08/04  22:48:39  percyk
 * CSCdi38274:  delete sig processes if no atm port present.
 *
 * Revision 2.5  1995/07/26  17:35:59  percyk
 * CSCdi34916:  sscop interface commands cause SegV exception during
 * initialization
 *
 * Revision 2.4  1995/07/11  06:05:10  schrupp
 * CSCdi36890:  sscop keepalive timer not working
 *
 * Revision 2.3  1995/06/28  09:22:29  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/14  15:16:52  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * cls/dlcqllc.c:         Unexepcted -> Unexpected
 * ipc/msg_ipc.c:         availble -> available
 * exec_debug_channel.h:  debuggeries -> debugging
 * filesys/fslib.c:       inconsistant -> inconsistent
 * atm/sscop.c:           initalize -> initialize
 * filesys/fslib_print.c: progresss -> progress
 * ts/callback.c:         unnexpected -> unexpecte
 *
 * Revision 2.1  1995/06/07  20:12:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *--------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "atmsig_private.h"
#include "atmsig_public.h"
#include "sscop_private.h"
#include "sscop_public.h"
#include "mgd_timers.h"
#include "ilmi_public.h"
#include "../atm/atm_registry.h"



/*
 * G l o b a l  S t o r a g e 
 */
watched_queue *sscop_inputQ;
watched_queue *sscop_outputQ;

paktype *sscop_ackQ[16];
paktype *sscop_rcvQ[16];

ulong sscop_ackQ_cnt = 0;
ulong sscop_rcvQ_cnt = 0;

/* Don't enable sscop until after sscop_init */
int sscopEnabled = FALSE;
int sscop_statsIgnored;

mgd_timer sscop_wakeupTime;
mgd_timer sscop_keepalives;

/*
 * sscop control blocks. 
 * 1) Static array is used, as we can't do malloc at interrup level
 * 2) one sscop control block per ATM port, so we need MAX_ATM_INTFC 
 *    elements
 */
static sscopType sscop_ctb[MAX_ATM_INTFC];

/*
 * s s c o p _ i n i t ( )
 *
 * One sscop control block allocated and initialized for every ATM port.
 * Initializes input, output queues and spawns Input, Output and Timer
 * processes.  This routine is called only once at system startup time.
 */
void sscop_init (subsystype *subsys)
{

   hwidbtype *hwidb;
   int i;


   /* common input queue for sscop - shared by all ATM interfaces. */
   sscop_inputQ = create_watched_queue("SSCOP Input", 0, 0);
   sscop_outputQ = create_watched_queue("SSCOP Output", 0, 0);

   /* initialize parent timers */
   mgd_timer_init_parent(&sscop_wakeupTime, NULL);
   mgd_timer_init_parent(&sscop_keepalives, NULL);

   /*
    * init the sscop block
    */
   for (i = 0; i < MAX_ATM_INTFC; i++) {
       
     /* 
      * initialize sscop's default configuration values.  Do NOT put this in
      * sscop_initSscopCb() as these are default values.  sscop_initSscopCb()
      * could be called repeatedly and the user may have changed timers and 
      * retry counts from their default values to something different.
      */ 
       sscop_ctb[i].pollTimerValue       = SSCOP_POLLTIMER_DEFAULT;
       sscop_ctb[i].keepaliveTimerValue  = SSCOP_KEEPALIVETIMER_DEFAULT;
       sscop_ctb[i].ccTimerValue         = SSCOP_CCTIMER_DEFAULT;
       sscop_ctb[i].maxCC                = SSCOP_MAXCC_DEFAULT;
       sscop_ctb[i].sendWindow           = SSCOP_SENDWINDOW_DEFAULT;
       sscop_ctb[i].rcvWindow            = SSCOP_RCVWINDOW_DEFAULT;
       sscop_ctb[i].maxStat              = SSCOP_MAX_LIST_ELEMENTS;
       sscop_ctb[i].maxRetryCount        = SSCOP_MAXRETRY_CNT_DEFAULT;
       sscop_ctb[i].maxPd                = SSCOP_SENDWINDOW_DEFAULT;
       sscop_ctb[i].quickPoll            = FALSE;
       /* initialize these v31 variables */
       sscop_ctb[i].maxCC                = SSCOP_MAXRETRY_CNT_DEFAULT;
       sscop_ctb[i].idleTimerValue       = SSCOP_IDLE_DEFAULT;
       sscop_ctb[i].noResponseTimerValue = SSCOP_NORESPONSE_DEFAULT;
       sscop_ctb[i].uni_version          = ATMUNI_VERSION_INVALID;

       sscop_ctb[i].index		 = i;
       sscop_ctb[i].idb 		 = NULL;

       sscop_ctb[i].flags	   = SSCOP_CTB_NOT_INUSE; /* it's free */
       queue_init(&sscop_ctb[i].txQ, 0);

       /* initialize leaf timers */
       mgd_timer_init_leaf(&sscop_ctb[i].timeoutValue, &sscop_wakeupTime, 
			   SSCOP_RETRY_TIMER, &sscop_ctb[i], TRUE);

       /* this timer is not used by uni 3.1 code */
       mgd_timer_init_leaf(&sscop_ctb[i].keepalive_timer, &sscop_keepalives, 
			   SSCOP_KEEPALIVE_TIMER, &sscop_ctb[i], TRUE);
       mgd_timer_init_leaf(&sscop_ctb[i].timerIdle, &sscop_wakeupTime,
                           SSCOP_IDLE_TIMER, &sscop_ctb[i], TRUE);
       mgd_timer_init_leaf(&sscop_ctb[i].timerPoll, &sscop_wakeupTime,
                           SSCOP_POLL_TIMER, &sscop_ctb[i], TRUE);
       mgd_timer_init_leaf(&sscop_ctb[i].timerKeepalive_v31, &sscop_wakeupTime,
                           SSCOP_KEEPALIVE_TIMER_V31, &sscop_ctb[i], TRUE);
       mgd_timer_init_leaf(&sscop_ctb[i].t309, &sscop_wakeupTime,
                           SSCOP_ATMSIG_T309, &sscop_ctb[i], TRUE);
       mgd_timer_init_leaf(&sscop_ctb[i].timerNoResponse, &sscop_keepalives,
                           SSCOP_NO_RESPONSE_TIMER, &sscop_ctb[i], TRUE);

   }

   /* Initialize Sscop block for each ATM interface */
   FOR_ALL_HWIDBS(hwidb) {
     if (hwidb->status & IDB_ATM)
       sscop_interfaceReset(hwidb);
   }


   sscop_debug_init();
   sscop_parser_init();
   sscop_registerIlmi();

   sscop_statsIgnored = 0;
   
   /* Register some functions */
   reg_add_media_hw_reset(sscop_interfaceReset, "sscop_interfaceReset"); 
   reg_add_hwif_goingdown(sscop_close, "sscop_close");

   sscopEnabled = TRUE;
}



/*
 * sscop_find_next_ctb ()
 * A static routine to find next free sscop Contol Block (ctb)
 * Return the addr of the free sscop ctb if find one; otherwise
 * return a NULL.
 */
static sscopType *sscop_find_next_ctb (void)
{

    ushort i;
    boolean found = FALSE;


    /* look for a free sscop control block */
    for (i = 0; i< MAX_ATM_INTFC; i++) {
       if (sscop_ctb[i].flags == SSCOP_CTB_NOT_INUSE) {
          found = TRUE;
          sscop_ctb[i].flags = SSCOP_CTB_INUSE;    /* mark it being used */
          break;
       }
    }

    if (found)
	return (&sscop_ctb[i]);
    else 
	return ( (sscopType *)NULL );

}



/*
 * s s c o p _ i n t e r f a c e R e s e t ( )
 *
 * reset's sscop and Q.93B - called by the ATM driver when it resets the 
 * interface. This function decide which uni version  to run. If our uni
 * version is set to 3.0 but the peer has 3.1, we will adjust to 3.1.
 */
void sscop_interfaceReset (hwidbtype *hwidb)
{

    sscopType *sscop, **sscop_ptr;
    int peer_uni;

    sscop_ptr = &hwidb->sscop;
    sscop     = *sscop_ptr;
 
    if (!sscop) {

       sscop = sscop_find_next_ctb();
       if (!sscop) {
          /* fatal error */
          if (sscop_debugEvent)
             buginf("\nSSCOPERR: sscop cntrl blk not found for %s", 
                                             hwidb->hw_namestring);
	    return;
       }
 
       *sscop_ptr = sscop;
       sscop->idb = hwidb;
    }

    if (hwidb->atm_db->ignore_peer_uni) {
      sscop->uni_version = hwidb->atm_db->uni_version;
      if ( sscop_isV31(sscop) ) 
	sscop->keepaliveTimerValue = SSCOP_KEEPALIVE_V31_DEFAULT;
      else
	sscop->keepaliveTimerValue = SSCOP_KEEPALIVETIMER_DEFAULT;
    }
    else {
      /* user defined version may be overwritten. 
       * 1. if ilmi has not received peer uni version,
       *    take the user defined uni version.
       * 2. if peer supports uni 3.1, we match it.
       */
      peer_uni = hwidb->atm_db->peeruni_version;
      if(peer_uni >=  ATMUNI_VERSION_31) {
        sscop->keepaliveTimerValue = SSCOP_KEEPALIVE_V31_DEFAULT;
        sscop->uni_version = hwidb->atm_db->uni_version = ATMUNI_VERSION_31;
      }
      else {
        sscop->keepaliveTimerValue = SSCOP_KEEPALIVETIMER_DEFAULT;
        sscop->uni_version = hwidb->atm_db->uni_version = ATMUNI_VERSION_30;
      }

      if (sscop_debugError || sscop_debugEvent) 
	buginf("\nsscop_interfaceReset, int %d, ignore==FALSE, uni %d\n",
	     sscop->index,sscop->uni_version);
    }

    /* this will reset both sscop and Q.2931 */ 
    if (sscop_debugEvent | sscop_debugError )
      buginf("\n\nSSCOP INIT: InterFace Reset");

    sscop_initStatistics(sscop);
    sscop_initSscopCb(hwidb->sscop);
}

 

/*
 * s s c o p _ i n i t S s c o p C b ( )
 *
 * Initialize sscop control block.  This routine is called each time sscop 
 * needs to be re/initialized - ab/normal disconnects, fatal protocol errors,
 * system timeouts etc.
 */
void sscop_initSscopCb (sscopType *sscop)
{

   paktype *pak;


   if (sscop_debugEvent)
      buginf("\nSSCOP: initialize sscop cb");

   /* first pull down all svc's */
   sscop->q93bResetFlag = TRUE;
   process_send_message(atmsig_timer_pid, ATMSIG_SSCOP_RESET, sscop, 0);

   /* initialize state to idle */
   sscop->state = SSCOP_ST_IDLE;

   if ( sscop_isV31(sscop) ) {
     /* init states variables */
     sscop->poll_vps   =
     sscop->vtSq       =
     sscop->vrSq       = 0;
     /* sscop->vtCc       =   */
     sscop->credit     = 1;
     sscop_resetDataXferTimers(sscop);
     sscop_stopTimer309(sscop);
   }
   else     /* uni 3.0 only */
     sscop->vtCc = SSCOP_MAXCC_DEFAULT;

   sscop->vs         = 
   sscop->vps        = 
   sscop->vta        = 
   sscop->vtPa       =
   sscop->vtPd       = 
   sscop->vr         = 
   sscop->vrh        = 
   sscop->retryCount =  0;

   /* start off by accepting a window worth of pdu's */ 
   sscop->vmr = SSCOP_RCVWINDOW_DEFAULT;

   sscop->vms = SSCOP_SENDWINDOW_DEFAULT;
   

   /* stop keepalive timer (if active) */
   mgd_timer_stop(&sscop->keepalive_timer);

   /* stop timer (if active) */
   sscop_stopTimer(sscop);

   /* empty transmit and receive queues */
 
   /* rcvQ - out of sequence Sd's or Sdp's are queued here */
   sscop_removeAllFromRcvQ(sscop);
   sscop_rcvQ[sscop->index] = (paktype *)NULL;
  
   /* ackQ - Sd's sent but not yet acknowledged are queued here */ 
   sscop_removeAllFromAckQ(sscop); 
   sscop_ackQ[sscop->index] = (paktype *)NULL;

   /* txQ - messages sent by Q.2931 are queued here */
   while ( (pak = pak_dequeue(&sscop->txQ) ) != NULL)
      datagram_done(pak);

   /* Initialize lock to FALSE */
   sscop->sscopInUse = FALSE;

}


/*
 * remove this from sscop_initSscopCb() is
 * because usually these statistics counters
 * should only be cleared when interface reset
 * or when user wants to clear them. sw should
 * not clear them just because of failure for
 * whatever reason.
 */
void sscop_initStatistics (sscopType *sscop)
{
  memset(&sscop->stats, 0, sizeof(sscopStats));
}

/*
 * Semaphores
 */
static tinybool sscopGetLock (sscopType *sscop)
{
  tinybool locked;

  locked = lock_semaphore(&sscop->sscopInUse);
  return(locked);
}

static void sscopClearLock (sscopType *sscop)
{
  unlock_semaphore(&sscop->sscopInUse);
  return;
}



/*
 * s a a l _ e n q u e u e ( )
 *
 * Arrive here at interrupt level.
 * If poll pdu, try to handle it here.
 * Else queue from ATM driver onto SSCOP input queue.
 * Don't want poll pdu to get out of order from other pdu's.
 * Need to check 2 things: 
 * 1) is inputQ empty?
 * 2) is this sscop block locked?
 */
void saal_enqueue (paktype *pak, vc_info_t *vc)
{

  idbtype *idb;
  sscopType *sscop;
  paktype *oldpak;


  if (!sscopEnabled) {
    datagram_done(pak);
    return;
  }

  oldpak = process_dequeue(sscop_inputQ);

  if (!oldpak) {
    idb = pak->if_input;
    if ( (idb) && (idb->hwptr) ) {
      sscop = idb->hwptr->sscop;

      if ( (sscop) && (sscop->quickPoll) ) {

	if (sscopGetLock(sscop)) {
	  /* Try to handle SSCOP poll pdu immediately */
	  if (sscop_handle_poll_pdu(pak, vc)) {
	    sscopClearLock(sscop);
	    return;
	  }
	  sscopClearLock(sscop);
	}

      }
    }
  }

  /* Requeue the pak we tested */
  if (oldpak) {
    process_requeue_pak(sscop_inputQ, oldpak);  
  }

  /* Enqueue the new pak */
  process_enqueue_pak(sscop_inputQ, pak);

}


/*
 * s s c o p _ i n p u t P r o c e s s ( )
 *
 * Input process blocks - waiting for incoming pdu's from the ATM driver.
 * Note that sscop pdu's from ALL ATM interfaces are queued on sscop_inputQ. 
 */
process sscop_inputProcess (void)
{
  ulong major, minor;
  paktype *pak;
  idbtype *idb;
  sscopType *sscop;
    
  /* wait for system to initialize */
  process_wait_on_system_init();
    
  process_watch_queue(sscop_inputQ, ENABLE, RECURRING);
    
  for (;;) {
    process_wait_for_event();
	
    while (process_get_wakeup(&major, &minor)) {
      switch (major) {
      case QUEUE_EVENT:
	while ((pak = process_dequeue(sscop_inputQ)) != NULL) {

	  idb = pak->if_input;
	  if (!idb) {
	    if (sscop_debugEvent)
	      buginf("\nSSCOPERR: input: no idb set in pak->if_input");
	    datagram_done(pak);
	    SSCOP_STATS(sscop_statsIgnored);
	    continue;
	  }

	  sscop = idb->hwptr->sscop;
	  if ( !sscop || !sscopEnabled || !atm_interface_up(idb) ) {
	    if (sscop_debugEvent)
	      buginf("\nSSCOPERR: input: sscop %s, idb state %s",
		     sscopEnabled ? "enabled":"disabled", 
		     atm_interface_up(idb) ? "UP":"DOWN");
	    datagram_done(pak);
	    SSCOP_STATS(sscop_statsIgnored);
	    continue;
	  }

	  /* LOCK the sscop block 
	   * Don't allow interrupt to access this block.
           * The interrupt will check the lock at its entry point.
	   */
	  sscopGetLock(sscop);
	  sscop_input(pak);
	  sscopClearLock(sscop);
	}
	break;
		
      default:
	errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
	break;
      }
    }
  }
}



/*
 * s s c o p _ i n p u t  ( )
 * 
 * ALL the received sscop pdu's (from all the ATM ports) come in here.
 * pak->if_input will tell us the input ATM port. 
 * Sanity checks already done in sscop_inputProcess.
 */
void sscop_input (paktype *pak)
{

    idbtype *idb;
    sscopType *sscop;
    sscop_events newevent;


    if (sscop_debugPacket)
       atmSig_hexout(pak->datagramstart, pak->datagramsize, "SSCOP(I)");

    idb = pak->if_input;  /* input ATM interface */
    sscop = idb->hwptr->sscop;

    /* remember time of last input, save it in sscop, idb and packet */
    GET_TIMESTAMP(pak->inputtime);
    COPY_TIMESTAMP(pak->inputtime, sscop->last_input);
    COPY_TIMESTAMP(pak->inputtime, idb->hwptr->lastinput);

    /* determine op-code of pdu received. */
    newevent = sscop_findPduType(pak, sscop);

    /* if pdu invalid - free packet and return (packet ignored).  */
    if (newevent >= SSCOP_RCV_INVALIDPDU) {
       datagram_done(pak);
       SSCOP_STATS(sscop_statsIgnored);
       return;
    }

    /* All sscop packets MUST be long word aligned, toss packet if not. */
    if ( (pak->datagramsize % SSCOP_PADBYTES) != 0 ) {
       if (sscop_debugEvent)
          buginf("\nSSCOPERR: input packet of size %d not long word aligned",
                                                           pak->datagramsize); 
       datagram_done(pak);
       SSCOP_STATS(sscop_statsIgnored);
       return;
    }

    sscop->inbuf = pak;
    sscop->event = newevent;

    /* feed the pdu to the Sscop state machine */
    sscop_stateTable(sscop);

    /*
     * free the received non-data pdu.  Data pdu's are either put on 
     * sscop->rcvQ or sent directly to Q.2931.
     */  
    if (sscop->inbuf) {
       datagram_done(sscop->inbuf);
       sscop->inbuf = NULL;
    }

    /* try sending packets out q'd on this sscop */
    if (sscop->state == SSCOP_ST_DATA_XFER_READY)
       sscop_sendSdPdu(sscop);


}



/*
 * s s c o p _ o u t p u t P r o c e s s ( )
 *
 * Output process blocks - waiting for messages from Q.2931
 */
process sscop_outputProcess (void)
{
    ulong major, minor;
    mgd_timer *expired_timer;
    sscopType *sscop;
    hwidbtype *hwidb;

    /* wait for system to initialize */
    process_wait_on_system_init();
    
    process_watch_mgd_timer(&sscop_keepalives, ENABLE);
    process_watch_queue(sscop_outputQ, ENABLE, RECURRING);    

    /* do forever */
    for (;;) {
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		while (mgd_timer_expired(&sscop_keepalives)) {
		    expired_timer = mgd_timer_first_expired(&sscop_keepalives);
		    sscop = mgd_timer_context(expired_timer);
		    switch (mgd_timer_type(expired_timer)) {

		      case SSCOP_KEEPALIVE_TIMER:
		      if (sscop_getUniVersion(sscop) != ATMUNI_VERSION_30) {
			mgd_timer_stop(expired_timer); 
			if (sscop_debugError)
			  buginf("\nSCPERR(%d),keepalive-timer expiry in uni 3.1",
				 sscop->index);

			break;
		      }
		      sscopGetLock(sscop);
		      sscop_output(sscop);
		      sscopClearLock(sscop);
		      break;

		      case SSCOP_NO_RESPONSE_TIMER:
			if (sscop_debugError)
			  buginf("\nSCP(%d) - no response timer expiry",sscop->index);
		        mgd_timer_stop(expired_timer); 
		        if (sscop->state != SSCOP_ST_DATA_XFER_READY) {
			  if (sscop_debugError)
			    buginf("\nSCPERR(%d), Timer_NO-RESPONSE expiry in %s\n",
				   sscop->index,sscop_printState(sscop->state));
			  /* restart sscop */
			  sscopGetLock(sscop);
			  sscop_initSscopCb(sscop);
			  sscopClearLock(sscop);
			  break;
			}	 
		        /* Timer_NO-RESPONSE expired, tear down sscop */
		        /* Get the Lock to avoid unpleasant effects */
		        sscopGetLock(sscop);
		        sscop_maaErrorIndication
			  (sscop->state,MAA_ERROR_P,sscop->index);
		        sscop_transmitEndPdu(sscop);
		        /* tell signalling clear unactive SVCs & start T309 */
		        sscop_aaReleaseIndication(sscop);
		        sscop_requestConnection(sscop);
		        sscopClearLock(sscop);

		        break;

		      default:
			errmsg(&msgsym(UNEXPECTEDTIMER, SCHED), expired_timer,
			       mgd_timer_type(expired_timer));
			mgd_timer_stop(expired_timer); 

			break;
		    }
		}
		break;

	      case QUEUE_EVENT:
		/* queue packets from Q.2931 onto appropriate sscop tx queue */
		sscop_queueSdOnTxQ();

		FOR_ALL_HWIDBS(hwidb) {

		  /* sscop only for ATM interfaces */
		  if ( !(hwidb->status & IDB_ATM) )
		    continue;

		  /* get the sscop control block */
		  sscop = hwidb->sscop;
		  if (!sscop) {
		    /* fatal error - ATM idb missing sscop block */

		    if (sscop_debugError)  
		    buginf("\nSSCOPERR: sscop not found on %s", 
			   hwidb->hw_namestring); 
		    continue;
		  }

		  /* check TxQ, keepalives checked above */
		  if (sscop->txQ.qhead != NULL) {
		    sscopGetLock(sscop);
		    sscop_output(sscop);
		    sscopClearLock(sscop);
		  }

		}
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;

	      }
	}
    }

}




/*
 * s s c o p _ o u t p u t  ( )
 *
 * Message queued by Q.93B.
 */
void sscop_output (sscopType *sscop)
{


   /* Bring up line if in idle state, else try sending data */
   if (sscop->state == SSCOP_ST_IDLE) {
      sscop->event = SSCOP_TX_BEGIN;
      sscop_stateTable(sscop);
      return;
    }

   if (sscop->state == SSCOP_ST_DATA_XFER_READY) {

     if (mgd_timer_expired(&sscop->keepalive_timer)) 
       sscop->event = SSCOP_TX_KEEPALIVE;
     else
       sscop->event = SSCOP_TX_DATA;

     /* feed the packet to the SSCOP state machine */
     sscop_stateTable(sscop); 

   }
   
}



/*
 * s s c o p _ f i n d P d u T y p e ( )
 *
 * Determine pdu type (opcode) - used for incoming packets.
 */
sscop_events sscop_findPduType (paktype *pak, sscopType *sscop)
{

   ulong *pduType;
   uint opcode;


   /* check for min size  - no point in going further if pdu too small. */ 
   if (pak->datagramsize < SSCOP_HEADER_SIZE) {
      if (sscop_debugEvent)
         buginf("\nSSCOPERR: incoming pdu of invalid size %d", 
                                                pak->datagramsize);
      SSCOP_STATS(sscop_statsIgnored); 
      return (SSCOP_RCV_TOOSMALL);
   }

   /* sscop pdu's have headers at END of the pdu!!  */
   pduType = (ulong *)(pak->datagramstart + pak->datagramsize - 
                                                 SSCOP_HEADER_SIZE);

   /* Return the pdu type */
   switch (*pduType & SSCOP_PDUTYPE_MASK) {

      /* Begin Pdu */
      case SSCOP_MT_BGN:
         opcode = SSCOP_RCV_BGN;
         break;

      /* Begin Ack Pdu */
      case SSCOP_MT_BGAK:
         opcode = SSCOP_RCV_BGAK;
         break;    

      /* End Pdu */ 
      case SSCOP_MT_END:
         opcode = SSCOP_RCV_END;
         break;    

      /* End Ack Pdu */ 
      case SSCOP_MT_ENDAK:
         opcode = SSCOP_RCV_ENDAK;
         break;  

      /* Resync Pdu */
      case SSCOP_MT_RS:
         opcode = SSCOP_RCV_RS;
         break;     

      /* Resync Ack Pdu */
      case SSCOP_MT_RSAK:
         opcode = SSCOP_RCV_RSAK;
         break;     

      /* Begin Reject Pdu */
      case SSCOP_MT_BGREJ:
         opcode = SSCOP_RCV_BGREJ;
         break;  

      /* Sequenced Data Pdu */
      case SSCOP_MT_SD:
         opcode = SSCOP_RCV_SD;
         break;   

      /* Sequenced Data with Poll Pdu */
      case SSCOP_MT_SDP:
	 /* in UNI 3.1, ER has the same opcode as sdp */
	 if ( sscop_isV31(sscop) )
	   opcode = SSCOP_RCV_ER;
	 else
	   opcode = SSCOP_RCV_SDP;
         break;    

      /* Poll Pdu */
      case SSCOP_MT_POLL:
         opcode = SSCOP_RCV_POLL;
         break;  

      /* Status Pdu */
      case SSCOP_MT_STAT:
         opcode = SSCOP_RCV_STAT;
         break;    

      /* Unsolicited Status Pdu */
      case SSCOP_MT_USTAT:
         opcode = SSCOP_RCV_USTAT;
         break;  

      /* Unassured Data Pdu */
      case SSCOP_MT_UD:
         opcode = SSCOP_RCV_UD;
         break;   

      /* Management Pdu */
      case SSCOP_MT_MD:
         opcode = SSCOP_RCV_MD;
         break;

       case SSCOP_MT_ERAK:
	 opcode = SSCOP_RCV_ERAK;
	 break;

      /* Unknown Pdu */
      default:
         if (sscop_debugEvent)
            buginf( "\nSSCOP: invalid opcode %x", (*pduType & SSCOP_PDUTYPE_MASK) );  
         opcode = SSCOP_RCV_UNKNOWNPDU;
         break;

   }

   if (sscop_debugEvent)
      buginf("\nSSCOP: i %s pdu, %s state, length = %d",
                      sscop_printPduType(*pduType & SSCOP_PDUTYPE_MASK, 
					 sscop->uni_version), 
                      sscop_printState(sscop->state), pak->datagramsize);

   return (opcode);


}      



/*
 * s s c o p _ p r o c e s s S d p P d u ( )
 *
 * Process received Sd (Sequenced Data) OR Sdp (Sequenced Data with Poll) Pdu.
 * The processing of a Sd and a Sdp Pdu is identical with the exception of an
 * extra step involved of processing the poll part of a Sdp Pdu. 
 * If 'poll' set to TRUE received pdu is Sdp, otherwise received pdu is a Sd.
 */
void sscop_processSdpPdu (sscopType *sscop, boolean poll)
{


   paktype *pak;
   uchar *frameptr;
   sscop_sdPdu *pdu;
   sscop_sdpPdu *ppdu;
   ulong ns, pollSeqNum, pad;
   boolean wraparound;


   /* Get the Sd header */
   pak = sscop->inbuf;
   frameptr = pak->datagramstart + pak->datagramsize - sizeof(sscop_sdPdu);
   pdu = (sscop_sdPdu *)frameptr;

   if (poll) {
     /* sanity check. will never happen. remove it if necessary */
     if (sscop_isV31(sscop) ) {
       if (sscop_debugError)
	 buginf("\nSCPERR(%d) - SDP PDU in uni 3.1",sscop->index);

       return;
     }

      ppdu = (sscop_sdpPdu *)( pak->datagramstart + pak->datagramsize -
                                                      sizeof(sscop_sdPdu) );
      ns = ppdu->type_ns & SSCOP_SEQNUM_MASK;
      pollSeqNum = ppdu->ps & SSCOP_SEQNUM_MASK;
      sscop_processPollPdu(sscop);
      SSCOP_STATS(sscop->stats.InSdpPdu);
   }
   else {
      pdu = (sscop_sdPdu *)( pak->datagramstart + pak->datagramsize - 
                                                      sizeof(sscop_sdPdu) );
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      SSCOP_STATS(sscop->stats.InSdPdu);
   }

   pad = pdu->type_ns & SSCOP_PADBYTE_MASK;

   if (sscop_debugEvent)
      buginf("\nSSCOP: i Sd(p) => ns = %d, vr = %d, vrh = %d, vmr = %d, pad = %d",
                           ns, sscop->vr, sscop->vrh, sscop->vmr, (pad >> 30) );

   if (sscop->vmr < sscop->vr)
      wraparound = TRUE;
   else
      wraparound = FALSE;

   /* 
    * Check if packet is within the bounds of the receive window by comparing the
    * sequence number parameter N(S) with the lower (vr) and upper edges (vmr) of
    * of the receive window.  If so, the Sd is processed, otherwise it is discarded.
    * Also need to take into account the wraparound condition - if (vr > vmr) then
    * a wraparound has just taken place - pk.
    */
   if ( sscop_isV31(sscop) ) {
     if ( (ns < sscop->vr) || ((ns >= sscop->vmr) && !wraparound) ) {  
       if ( (ns >= sscop->vr) && (sscop->vrh < sscop->vmr) )  {
	 /* send out ustat pdu */
	 sscop_transmitUstatPdu(sscop, ns); 
	 sscop->vrh = sscop->vmr;
       }

       if (sscop_debugEvent)
	 buginf("\nSCPEVNT(%d) - i SD, exceed rcv window ns = %d,vr = %d,vmr = %d",
		sscop->index,ns, sscop->vr, sscop->vmr);
       datagram_done(pak);
       sscop->inbuf = NULL; 
       return;
     }
   }
   else   if ( (ns < sscop->vr) || (ns >= sscop->vmr) ) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: Sd outside receive window ns = %d, vr = %d, vmr = %d",
                                               ns, sscop->vr, sscop->vmr);
      datagram_done(pak);
      sscop->inbuf = NULL; 
      return;
   }

   /*
    * In processing a received Sd pdu, a number of cases apply -
    */

   /*
    * a) Sd pdu is next expected in sequence and next highest expected.  This is
    * the error free case.  The Pdu is passed onto the next higher layer.
    *
    * b) Sd pdu is next expected in sequence, but not the next highest expected.
    * In this case the received packet is the next in sequence expected, but is not
    * the highest received i.e. the packet has been retransmitted.  In this case,
    * the pdu is passed onto the next higher layer.  The state variable V(R) a.k.a
    * sscop->vr is incremented.  An additional check is performed to
    * determine if this next highest Sd pdu is in the receive queue.  If so, this
    * packet is dequeued and also passed onto the higher layer, the state variable
    * V(R) is incremented, and the check is performed again.  This check, and if
    * appropriate the delivery of the Sd pdu and incrementing of V(R) is performed
    * until VR(R) is not in the receive queue.
    */
   if (sscop->vr == ns) {
      if (sscop->vrh == ns) {
         sscop_enqueue(pak, poll);  /* queue the packet for Q93B */
         sscop->vr = sscop->vrh = ns + 1;
      }
      else {
         do {
            sscop_enqueue(pak, poll);   /* queue the packet for Q93B */
            SSCOP_INCR_CNTR(sscop->vr);
         } while ( (pak = sscop_removeSdFromRcvQ(sscop, sscop->vr) ) );
      }
      sscop->inbuf = NULL;
      return;
   }

   /*
    * c) Sd pdu is not next expected in sequence but is next highest expected
    * In this case, the Sd pdu is saved in the receive queue, and vr(h) a.k.a.
    * sscop->vrh is incremented.
    */
   if (sscop->vrh == ns) {
      sscop_queueSdOnRcvQ(sscop, pak);
      SSCOP_INCR_CNTR(sscop->vrh);
      sscop->inbuf = NULL;
      return;
   }

   /*
    * d) Sd pdu is not next expected in sequence and is less than the next highest
    * expected.  In this case, the Sd pdu is saved in the receive queue.  If the 
    * Sd pdu is already in the receive queue, a fatal protocol error has occurred.
    */
   if (sscop->vrh >= ns) {
      if ( sscop_isSdInRcvQ(sscop, ns) ) {
         /* fatal protocol error - bring down the connection - log it!! */
         if (sscop_debugEvent) 
            buginf("\nSSCOP: i Sd ns = %d already in rcv Q - fatal error", ns);
	 if ( sscop_isV31(sscop) ) {
	   sscop_resetDataXferTimers(sscop);
	   sscop_sequenceError(sscop,MAA_ERROR_Q);
	 }
         datagram_done(pak);
      }
      else 
         sscop_queueSdOnRcvQ(sscop, pak);
      sscop->inbuf = NULL;
      return; 
   }

   /*
    * e) Sd Pdu is not next expected in sequence and is greater than the next 
    * highest expected.  In this case, the Sd pdu forms the boundary of a new 
    * sequence gap in the received data stream.  An unsolicited list is generated 
    * of the missing pdu's of this gap and is transmitted in a USTAT pdu.  The 
    * Sd pdu is saved in the receive buffer, and vr(h) is set to the sd.n(s) + 1.
    */ 
   if (sscop->vrh < ns) {
      sscop_queueSdOnRcvQ(sscop, pak);
      sscop_transmitUstatPdu(sscop, ns); 
      sscop->vrh = ns + 1;
      sscop->inbuf = NULL;
   }

}



/*
 * s s c o p _ p r o c e s s P o l l P d u ( )
 *
 * Process received Poll Pdu.  Since the processing of the poll part of a Sdp
 * pdu is identical to that of a Poll pdu, this routine is also called when the 
 * Poll is a part of a received Sdp Pdu.  sscop->event will tell us if the Pdu 
 * is a Poll or a Sdp.  We will transmit a Stat Pdu in response to a Poll Pdu.
 */
void sscop_processPollPdu (sscopType *sscop)
{

   paktype *pak;
   sscop_pollPdu *pdu;
   ulong ns;


   pak = sscop->inbuf;

   /*
    * sscop->event will tell us if the pdu is a Poll or a Sdp Pdu.  This info
    * is necessary to locate the poll header for a Sdp Pdu which is at the 
    * end of the packet.
    * NOTE - poll Pdu and Sdp Pdu have identical headers.
    */
   if (sscop->event == SSCOP_RCV_SDP) {
     /* just a sanity check, should never happen. remove it if need
        to tune up the performance or reduce the code size */
     if ( sscop_isV31(sscop) ) {  
       if (sscop_debugError)
	 buginf("\nSCPERR(%d), processPollPdu, RCV_SDP in uni 3.1\n",sscop->index);
       return;
     }
     else
       pdu = (sscop_pollPdu *)(pak->datagramstart + pak->datagramsize -
                                                        sizeof(sscop_pollPdu));
    }
   else {
      /* pdu received is a Poll pdu */
      pdu = (sscop_pollPdu *)pak->datagramstart;
   } 

   sscop->incomingPs = pdu->ps & SSCOP_SEQNUM_MASK;   /* incoming poll seq num */
   ns = pdu->type_ns & SSCOP_SEQNUM_MASK;  /* send sequence number */

   if (sscop_debugEvent)
      buginf("\nSSCOP: i Poll pdu, ns = %d, nps = %d, uni %d", ns, sscop->incomingPs
	     ,sscop->uni_version);

   /*
    * 'ns' is the seq number of the next frame the remote side will send us.
    * 'vrh' is the highest sequence number we have received so far.
    * Clearly 'ns' needs to be greater than 'vrh' for correct operations.  If
    * ns < vrh then the remote side's transmitter is badly broken and it would
    * best to reestablish the link.
    */ 
   if (sscop->vrh <= ns) {
     if (sscop->vmr < ns)
       sscop->vrh = sscop->vmr;
     else
       sscop->vrh = ns;
   } else {
      /* fatal protocol error - reestablish link */
      if (sscop_debugEvent) 
         buginf("\nSSCOP: ns %d is less than vrh %d - fatal protocol error",
                                        ns, sscop->vrh);
      if (sscop_isV31(sscop)) {
	sscop_sequenceError(sscop,MAA_ERROR_Q);
	return;
      }
   }
 
   SSCOP_STATS(sscop->stats.InPollPdu);

   /* transmit a Stat Pdu - below routine will also create a solicited list */
   sscop_transmitStatPdu(sscop);

}



/*
 * s s c o p _ i s S d I n A c k Q
 *
 * Returns TRUE if a queued packet's sequence number matches the given
 * sequence number i.e. sd.n(s) == seqnum?
 */
boolean sscop_isSdInAckQ (sscopType *sscop, ulong seqnum)
{

   paktype *pak, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;
   int index;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_ackQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize - 
                                                    sizeof(sscop_sdPdu));
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (ns == seqnum) {
         reset_interrupt_level(status);
         if (sscop_debugEvent)
            buginf("\nSSCOP: pak with ns %d in queue", seqnum);
         return(TRUE);
      }
      prev = &pak->next;
      pak = pak->next;
   }

   reset_interrupt_level(status);
   if (sscop_debugEvent)
      buginf("\nSSCOP: pak with ns %d not in queue", seqnum);
   return (FALSE);

}



/*
 * s s c o p _ i s S d I n R c v Q
 *
 * Returns TRUE if a queued packet's sequence number matches the given
 * sequence number i.e. sd.n(s) == seqnum?
 */
boolean sscop_isSdInRcvQ (sscopType *sscop, ulong seqnum)
{

   paktype *pak, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;
   int index;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_rcvQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize -
                                                    sizeof(sscop_sdPdu));
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (ns == seqnum) {
         reset_interrupt_level(status);
         if (sscop_debugEvent)
            buginf("\nSSCOP: pak with ns %d in queue", seqnum);
         return(TRUE);
      }
      prev = &pak->next;
      pak = pak->next;
   }

   reset_interrupt_level(status);
   if (sscop_debugEvent)
      buginf("\nSSCOP: pak with ns %d not in queue", seqnum);
   return (FALSE);

}


/*
 * s s c o p _ f i n d  P s I n S d 
 *
 * Find the Poll Sequence number for a queued Sd Pdu.
 * Returns n(ps) for a queued packet if found, else returns -1.
 */
long sscop_findPsInSd (sscopType *sscop, ulong seqnum)
{

   paktype *pak, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;
   int index;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_ackQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)( pak->datagramstart + pak->datagramsize -
                                                   sizeof(sscop_sdPdu) );
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
 
      if (ns == seqnum) {
         reset_interrupt_level(status);
         return (pak->sscop_nps);
      }
      prev = &pak->next;
      pak = pak->next;
   }

   reset_interrupt_level(status);
   return (-1);

}



/*
 * s s c o p _ q u e u e S d O n T x Q
 *
 *  queue packets on the appropriate sscop's transmit queue.
 *
 */
void sscop_queueSdOnTxQ (void)
{

   paktype *pak;
   hwidbtype *hwidb;
   sscopType *sscop;


   while ( (pak = process_dequeue(sscop_outputQ) ) != NULL) {

      hwidb = pak->if_output->hwptr;
      sscop = hwidb->sscop;

      /* log error and return */
      if (!sscop) {
         datagram_done(pak);
         if (sscop_debugEvent)
            buginf("\nSSCOPERR: no sscop blk for %s", 
		   sscop->idb->hw_namestring);
         return;
      }

     if (!pak_enqueue(&sscop->txQ, pak))
       datagram_done(pak);
   }

}



/*
 * s s c o p _ q u e u e S d O n A c k Q
 *
 *  place packet at end of requested queue.
 *
 */
void sscop_queueSdOnAckQ (sscopType *sscop, paktype *pak)
{

   int index;
   paktype *pak1;
   leveltype level; 


   /* sanity check */
   if (!pak) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: no packet to queue");
      return;
   }

   index = sscop->index;

   level = raise_interrupt_level(NETS_DISABLE);

   pak1 = sscop_ackQ[index];

   /* first entry? */
   if (!pak1) {
      sscop_ackQ[index] = pak;
      pak->next = NULL;
      reset_interrupt_level(level);
      sscop_ackQ_cnt++;
      return;
   }

   /* scan to the end */
   while (pak1) {
      if (pak1->next == NULL)
         break;
      else
         pak1 = pak1->next;
   }

   pak1->next = pak;
   pak->next = NULL;  

   reset_interrupt_level(level);
   sscop_ackQ_cnt++;

}



/*
 * s s c o p _ q u e u e S d O n R c v Q
 *
 *  place packet at end of requested queue.
 *
 */
void sscop_queueSdOnRcvQ (sscopType *sscop, paktype *pak)
{

   int index;
   paktype *pak1;
   leveltype level;


   /* sanity check */
   if (!pak) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: no packet to queue");
      return;
   }

   index = sscop->index;

   level = raise_interrupt_level(NETS_DISABLE);

   pak1 = sscop_rcvQ[index];

   /* first entry? */
   if (!pak1) {
      sscop_rcvQ[index] = pak;
      pak->next = NULL;
      reset_interrupt_level(level);
      sscop_rcvQ_cnt++;
      return;
   }
 
   /* scan to the end */
   while (pak1) {
      if (pak1->next == NULL)
         break;
      else
         pak1 = pak1->next;
   }
 
   pak1->next = pak;
   pak->next = NULL;

   reset_interrupt_level(level);
   sscop_rcvQ_cnt++;

}



/*
 * s s c o p _ r e m o v e S d F r o m A c k Q
 *
 * Removes a Sd from queue if Sd.n(s) matches the passed in sequence number.
 * Returns pak if found.
 */
paktype *sscop_removeSdFromAckQ (sscopType *sscop, ulong seqnum)
{

  
   int index;
   paktype *pak, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;

 
   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_ackQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize -
                                                   sizeof(sscop_sdPdu));
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (ns == seqnum) {
         *prev = pak->next;
         pak->next = (paktype *)NULL;
         if (sscop_debugEvent)
            buginf("\nSSCOP: pak with seqnum %d removed from AckQ", seqnum);
         reset_interrupt_level(status);
         sscop_ackQ_cnt--;
         return (pak);
      }
      prev = &pak->next;
      pak = pak->next;
   }

   reset_interrupt_level(status);
   if (sscop_debugEvent) 
     buginf("\nSSCOPERR: removeSdFromQ: no packet in AckQ with seqnum %d", seqnum);
   return (NULL);

}



/*
 * s s c o p _ r e m o v e S d F r o m R c v Q
 *
 * Removes a Sd from queue if Sd.n(s) matches the passed in sequence number.
 * Returns pak if found.
 */
paktype *sscop_removeSdFromRcvQ (sscopType *sscop, ulong seqnum)
{


   int index;
   paktype *pak, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_rcvQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize -
                                                   sizeof(sscop_sdPdu));
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (ns == seqnum) {
         *prev = pak->next;
         pak->next = (paktype *)NULL;
	 if (sscop_debugEvent) 
	   buginf("\nSSCOPERR: pak with seqnum %d removed from RcvQ", seqnum);
         reset_interrupt_level(status);
         sscop_rcvQ_cnt--;
         return (pak);
      }
      prev = &pak->next;
      pak = pak->next;
   }

   reset_interrupt_level(status);
   if (sscop_debugEvent) 
       buginf("\nSSCOPERR: removeSdFromQ: no packet in RcvQ with seqnum %d", seqnum);
   return (FALSE);

}



/*
 * s s c o p _ r e m o v e A l l F r o m A c k Q
 *
 * Removes all Sd's from Ack Q.  Used when the SSCOP link goes down to clear
 * out any queued buffers.
 */
void sscop_removeAllFromAckQ (sscopType *sscop)
{


   int index;
   paktype *pak, *pak1, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_ackQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize -
                                                   sizeof(sscop_sdPdu));
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (sscop_debugEvent)
         buginf("\nSSCOP: removeAllFromAckQ, pak with seqnum %d removed from AckQ", ns);
      pak1 = pak;
      pak = pak->next;
      pak1->next = NULL;
      datagram_done(pak1);
      sscop_ackQ_cnt--;
   }

   reset_interrupt_level(status);

}



/*
 * s s c o p _ r e m o v e A l l F r o m R c v Q
 *
 * Removes all Sd's from Rcv Q.  Used when the SSCOP link goes down to clear
 * out any queued buffers.
 */
void sscop_removeAllFromRcvQ (sscopType *sscop)
{


   int index;
   paktype *pak, *pak1, **prev;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_rcvQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize -
                                                   sizeof(sscop_sdPdu));
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (sscop_debugEvent) 
	buginf("\nSSCOPERR: removeAllFromRcvQ, pak with seqnum %d removed from RcvQ", ns);
      pak1 = pak;
      pak = pak->next;
      pak1->next = NULL;
      datagram_done(pak1);
      sscop_rcvQ_cnt--;
   }

   reset_interrupt_level(status);

}



/*
 *
 *  P r o g r a m m a t i c    I n t e r f a c e    t o    S s c o p
 *
 */


/*
 * s s c o p _ o p e n ( )
 *
 * Invoked by a higher layer to establish a sscop session.  The routine is
 * used in a non blocking fashion.  A Begin Pdu is transmitted and state changed
 * from idle to outgoing connection pending.  Another open is not attempted 
 * while a connection is in progress
 */
sscop_openCode sscop_open (idbtype *idb)
{

   sscopType *sscop;


   if (!idb || !idb->hwptr->sscop)
      return (SSCOP_OPEN_INVALIDPARAM);

   sscop = idb->hwptr->sscop;

   /* if open in progress, return */
   if (sscop->state != SSCOP_ST_IDLE)
      return (SSCOP_OPEN_INPROGRESS);

   /* transmit a Begin Pdu and change state to Outgoing Connection Pending */
   SSCOP_INC_VTSQ(sscop->vtSq);   
   /* transmission of a Begin Pdu will fail if system runs out of buffers */  
   if ( sscop_transmitBeginPdu(sscop) ) {
      /* change state to outgoing connection in progress */
      sscop->state = SSCOP_ST_OUTCONN_PENDING;
      sscop_startTimer(sscop, sscop->ccTimerValue);
      sscop->retryCount = 0;
      return (SSCOP_OPEN_OK);
   }
   else
      return (SSCOP_OPEN_NOMEMORY);


}



/*
 * this function makes sure that we send out
 * end pdu to the peer before entering the idle
 * state.
 */
void sscop_close (hwidbtype *hwidb)
{
  sscopType *sscop;

  if (!hwidb)
    return;

  if (!hwidb->sscop)
    return;

  sscop = hwidb->sscop;
  if ( (sscop->state == SSCOP_ST_IDLE) ||
      (sscop->state == SSCOP_ST_OUTDISCONN_PENDING) )
    return;

  if ( sscop_transmitEndPdu(sscop) == SSCOP_RESULT_OK ) {
    sscop->state = SSCOP_ST_OUTDISCONN_PENDING;
    sscop_startTimer(sscop,sscop->ccTimerValue);  
    sscop->retryCount = 0;
  }
  else {
    if (sscop_debugEvent || sscop_debugError)
      buginf("\nSCPEVNT: failed to tx End Pdu, sscop_close");
  }
}



/*
 *
 *   T  r  a  n  s  m  i  t      S  e  c  t  i  o  n
 *
 */

/*
 * s s c o p _ t r a n s m i t B e g i n P d u ( )
 *
 * Builds and transmits Begin Pdu.
 * Returns FALSE if no free buffers available - the state machine will
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitBeginPdu (sscopType *sscop)
{

   sscop_beginPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_beginPdu) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_beginPdu);

   pdu = (sscop_beginPdu *)pak->datagramstart;
   if ( sscop_isV31(sscop) ) 
     pdu->nuu_nsq = sscop->vtSq;
   else
     pdu->nuu_nsq = 0;

   /* msb - pdu type,  remaining three bytes - n(mr) */
   pdu->type_window = SSCOP_MT_BGN  | (sscop->vmr & SSCOP_SEQNUM_MASK);

   if (sscop_debugEvent)
      buginf("\nSSCOP: o Begin pdu, %s state, rcv window v(mr) = %d", 
                            sscop_printState(sscop->state), sscop->vmr);

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutBeginPdu);

   return (TRUE);

}




/*
 * s s c o p _ t r a n s m i t B e g i n A c k P d u ( )
 *
 * Builds and transmits Begin Ack Pdu.
 * Returns FALSE if no buffers available - state machine will
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitBeginAckPdu (sscopType *sscop)
{

   sscop_beginAckPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_beginAckPdu) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_beginAckPdu);

   pdu = (sscop_beginAckPdu *)pak->datagramstart;

   pdu->nuu = 0;    /* for now */

   /* msb - pdu type,  remaining three bytes - n(mr) */
   pdu->type_window = SSCOP_MT_BGAK | (sscop->vmr & SSCOP_SEQNUM_MASK);

   if (sscop_debugEvent)
      buginf("\nSSCOP: o Begin Ack pdu, %s state, rcv window v(mr) = %d", 
                             sscop_printState(sscop->state), sscop->vmr);

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutBeginAckPdu);

   return (TRUE);

}



/*
 * s s c o p _ t r a n s m i t B e g i n R e j P d u ( )
 *
 * Builds and transmits Begin Reject Pdu
 * Returns FALSE if no buffers available - the state machine will 
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitBeginRejPdu (sscopType *sscop)
{

   sscop_beginRejPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_beginRejPdu) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_beginRejPdu);

   pdu = (sscop_beginRejPdu *)pak->datagramstart;

   pdu->nuu = 0;    /* for now */

   /* msb - pdu type, remaining three bytes - zero */
   pdu->type = SSCOP_MT_BGREJ;

   if (sscop_debugEvent)
      buginf("\nSSCOP: o Begin Rej pdu, %s state", 
                                sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutBeginRejPdu);

   return (TRUE);

}
 


/*
 * s s c o p _ t r a n s m i t E n d P d u ( )
 *
 * Builds and transmits END Pdu
 * Returns FALSE if no buffers available - state machine will
 * only change state if the pdu is successfully transmitted.
 *
 * for UNI 3.1, sscop-uu is always NULL and padding is 0
 */
boolean sscop_transmitEndPdu (sscopType *sscop)
{

   sscop_endPdu *pdu;
   paktype *pak;

   /* get a buffer */
   pak = getbuffer( sizeof(sscop_endPdu) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_endPdu);

   pdu = (sscop_endPdu *)pak->datagramstart;

   if ( sscop_isV31(sscop) ) {
     pdu->nuu = 0;
     /* source bit always set to 1 */
     pdu->type = SSCOP_SBIT_SCP | SSCOP_MT_END;  /* PL == 0 */
   }
   else   {
     pdu->nuu = sscop->nuu;    /* for now */
     /* msb - pdu type, remaining three bytes - zero */
     pdu->type = SSCOP_MT_END;
   }

   if (sscop_debugEvent)
      buginf("\nSSCOP: o End pdu, %s state", sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutEndPdu);

   return (TRUE);

}



/*
 * s s c o p _ t r a n s m i t E n d A c k P d u ( )
 *
 * Builds and transmits End Ack Pdu.
 * Returns FALSE if no buffers available - state machine will
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitEndAckPdu (sscopType *sscop)
{

   sscop_endAckPdu *pdu30;
   sscop_endAck31Pdu *pdu31;
   paktype *pak;
   int pduSize;

   if ( sscop_isV31(sscop) )
     pduSize = sizeof(sscop_endAck31Pdu);
   else
     pduSize = sizeof(sscop_endAckPdu);

   /* get a buffer */
   pak = getbuffer( sizeof(pduSize) );
   if (!pak) {
      return (FALSE);
   }

   memset(pak, 0, pduSize);  /* to reset pdu->res if it's uni 3.1 */
   pak->datagramsize = pduSize;

   if ( sscop_isV31(sscop) ) {
       pdu31 = (sscop_endAck31Pdu *)pak->datagramstart;
       /* msb - pdu type, remaining three bytes - zero */
       pdu31->type = SSCOP_MT_ENDAK;
   }
   else {
      pdu30 = (sscop_endAckPdu *)pak->datagramstart;
      /* msb - pdu type, remaining three bytes - zero */
      pdu30->type = SSCOP_MT_ENDAK;
   }

   if (sscop_debugEvent)
      buginf("\nSSCOP: o End Ack pdu, %s state", 
                               sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutEndAckPdu); 

   return (TRUE);

}





/*
 * s s c o p _ t r a n s m i t R e s y n c P d u ( )
 *
 * Builds and transmits Resync Pdu.
 * Returns FALSE if no buffers available - state machine will
 * only change state if the pdu is successfully transmitted.
 */
boolean sscop_transmitResyncPdu (sscopType *sscop)
{

   sscop_resyncPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_resyncPdu) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_resyncPdu);

   pdu = (sscop_resyncPdu *)pak->datagramstart;

   if ( sscop_isV31(sscop) ) {
     pdu->nuu_nsq = sscop->vtSq;
     pdu->type = SSCOP_MT_RS | (sscop->vmr & SSCOP_SEQNUM_MASK);
   }
   else {
     pdu->nuu_nsq = 0;
     /* msb - pdu type, remaining three bytes - zero */
     pdu->type = SSCOP_MT_RS;
   }

   if (sscop_debugEvent)
      buginf( "\nSSCOP: o Resync pdu, %s state", 
                                 sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutResyncPdu);

   return (TRUE);

}



/*
 * s s c o p _ t r a n s m i t R e s y n c A c k P d u ( )
 *
 * Builds and transmits a Resync Ack Pdu.
 * Returns FALSE if no buffers available - since state machine 
 * will only change state if pdu is successfully transmitted.
 */
boolean sscop_transmitResyncAckPdu (sscopType *sscop)
{

   sscop_resyncAckPdu *pdu;
   paktype *pak;
   int pduSize;

   if ( sscop_isV31(sscop) )
     pduSize = sizeof(sscop_resyncAck31Pdu);
   else
     pduSize = sizeof(sscop_resyncAckPdu);

   /* get a buffer */
   pak = getbuffer( sizeof(pduSize) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = pduSize;

   pdu = (sscop_resyncAckPdu *)pak->datagramstart;

   if ( sscop_isV31(sscop) ) {
     /* do i need to do the & here ??? */
     pdu->type = SSCOP_MT_RSAK | (sscop->vmr & SSCOP_SEQNUM_MASK);
     ((sscop_resyncAck31Pdu *)pdu)->res = 0;
   }
   else
     /* msb - pdu type, remaining three bytes - zero */
     pdu->type = SSCOP_MT_RSAK;

   if (sscop_debugEvent)
      buginf( "\nSSCOP: o Resync Ack pdu, %s state", 
                              sscop_printState(sscop->state) );

   /*
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out.
    */
   atmSig_sendToAal(sscop, pak); 

   SSCOP_STATS(sscop->stats.OutResyncAckPdu);

   return (TRUE);

}



/*
 * s s c o p _ t r a n s m i t U s t a t P d u ( )
 *
 * Builds and transmits a Ustat pdu.  Transmitted when sscop detects 
 * a missing gap.  Returns FALSE if no free buffers available.
 * ns => sd.n(s) of the received pdu.
 */
boolean sscop_transmitUstatPdu (sscopType *sscop, ulong ns)
{

   paktype *pak;
   sscop_ustatPdu *pdu;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_ustatPdu) );
   if (!pak) {
      return (FALSE);
   }

   pdu = (sscop_ustatPdu *)pak->datagramstart;

   pdu->list1 = sscop->vrh & SSCOP_SEQNUM_MASK;
   pdu->list2 = ns & SSCOP_SEQNUM_MASK;
   pdu->type_nr = SSCOP_MT_USTAT | (sscop->vr & SSCOP_SEQNUM_MASK);

   /*
    * increment the sender's window by v(r) + SSCOP_RCVWINDOW_DEFAULT, 
    * don't increment our receive window till the previous window of
    * frames is fully received.  Make sure that v(mr) is still greater
    * than vr(h).
    */  
   sscop->vmr = (sscop->vr + SSCOP_RCVWINDOW_DEFAULT) & SSCOP_SEQNUM_MASK;
   pdu->nmr = sscop->vmr;

   pak->datagramsize = sizeof(sscop_ustatPdu);

   if (sscop_debugEvent)
      buginf("\nSSCOP: o Ustat pdu, list 1 = %d, list 2 = %d, v(r) = %d, v(mr) = %d",
                       pdu->list1, pdu->list2, sscop->vr, sscop->vmr); 

   /* 
    * Send it to ATM driver, it will encapsulate packet in AAL5 before
    * sending it out.
    */ 
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutUstatPdu);

   return (TRUE);

}



/*
 * s s c o p _ t r a n s m i t S t a t P d u ( )
 *
 * Builds and transmits a STAT Pdu in response to a Poll Pdu.
 * Returns FALSE if no free buffers available.
 */
boolean sscop_transmitStatPdu (sscopType *sscop)
{

   paktype *pak;
   int listLngth = 0;
   sscop_statPdu *pdu;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_statPdu) + SSCOP_MAX_LIST_ELEMENTS );
   if (!pak) {
      return (FALSE);
   }

   /* create Solicited list */
   listLngth = sscop_createSolicitedList(sscop, pak);
   pak->datagramsize = sizeof(sscop_statPdu) + listLngth;

   /* STAT Pdu header is after the List elements!! */
   pdu = (sscop_statPdu *)(pak->datagramstart + listLngth);

   pdu->ps = sscop->incomingPs;
   pdu->type_nr = SSCOP_MT_STAT | (sscop->vr & SSCOP_SEQNUM_MASK);

   /* 
    * increment sender's window by v(r) + SSCOP_RCVWINDOW_DEFAULT, don't 
    * increment our receive window till the previous window of frames is fully
    * received. Make sure that v(mr) is still greater than vr(h).
    */
   sscop->vmr = sscop->vr + SSCOP_RCVWINDOW_DEFAULT; 
   pdu->nmr = sscop->vmr & SSCOP_SEQNUM_MASK;

   if (sscop_debugEvent)
      buginf("\nSSCOP: o Stat pdu, state = %s, n(r) = %d, n(mr) = %d, n(ps) = %d",
        sscop_printState(sscop->state), sscop->vr, sscop->vmr, sscop->incomingPs);

   /* 
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation
    * before sending it out. 
    */ 
   atmSig_sendToAal(sscop, pak);

   SSCOP_STATS(sscop->stats.OutStatPdu);

   return (TRUE);
   

}



/*
 * s s c o p _ t r a n s m i t P o l l P d u ( )
 *
 */
boolean sscop_transmitPollPdu (sscopType *sscop)
{

   sscop_pollPdu *pdu;
   paktype *pak;


   /* get a buffer */
   pak = getbuffer( sizeof(sscop_pollPdu) );
   if (!pak) {
      return (FALSE);
   }

   pak->datagramsize = sizeof(sscop_pollPdu);

   pdu = (sscop_pollPdu *)pak->datagramstart;

   SSCOP_INCR_CNTR(sscop->vps);

   /* msb - zero, remaining 3 bytes - n(ps) */
   pdu->ps = sscop->vps;

   /* msb - pdu type, remaining three bytes - n(s) */
   pdu->type_ns = SSCOP_MT_POLL | (sscop->vs & SSCOP_SEQNUM_MASK);

   if (sscop_debugEvent)
      buginf("\nSSCOP: o Poll pdu, state = %s, n(s) = %d, n(ps) = %d", 
               sscop_printState(sscop->state), sscop->vs, sscop->vps);

   /* 
    * Send it to ATM driver, it will encap packet in AAL5 encapsulation 
    * before sending it out.
    */ 
   atmSig_sendToAal(sscop, pak);

   /* reset VT(PD) everytime poll pdu is sent */
   if (sscop_isV31(sscop))
     sscop->vtPd = 0;

   SSCOP_STATS(sscop->stats.OutPollPdu);

   return (TRUE);

}



/*
 * s s c o p _ s e n d S d P d u ( )
 *
 * Unqueue packets from the sscop transmit queue till the send window
 * is open, fill in the Sd headers and feed them to the ATM driver.
 */
void sscop_sendSdPdu (sscopType *sscop)
{

   paktype *pak, *dup;
   sscop_sdPdu *pdu;
   long pad;
 
   /*
    * As long as the send window is open, unqueue packets from sscop
    * transmit queue and send them out.
    */
   while (sscop->txQ.qhead) {

      /* send window closed? */
      if (sscop->vs >= sscop->vms) {
         if (sscop_debugEvent)
            buginf("\nSSCOP: sendSdPdu, send window closed, v(s) = %d, v(ms) = %d",
                                                  sscop->vs, sscop->vms);
         if ( sscop_isV31(sscop) ) {
           sscop->credit = 0;
           sscop->stats.LackOfCredit += 1;
           sscop_stopTimerIdle(sscop);

          /* when it happens, most likely we're busy. so don't send another
             poll pdu. also, there may be a stat pdu in the rcv buf which has
             not been processed yet. just make sure timerPoll is running */
          if ( !sscop_timerPollRunning(sscop) )
            sscop_startTimerPoll(sscop);

          return;
        }

	 if ( !mgd_timer_running(&sscop->timeoutValue) ) 
	   sscop_startTimer(sscop, sscop->pollTimerValue);
	 
         return;
      }

      pak = pak_dequeue(&sscop->txQ);

      /* sanity check */
      if (!pak) {
         if (sscop_debugEvent)
            buginf("\nSSCOPERR: sendSdPdu: no pak to send");
         return;
      }


      /* form Sd header - SSCOP headers are at the end of the packet */
      pdu = (sscop_sdPdu *)(pak->datagramstart + pak->datagramsize 
                                                  - SSCOP_HEADER_SIZE);
      /* insert pad byte */
      pad = pdu->type_ns;
      pdu->type_ns = (pad << 30) | SSCOP_MT_SD | (sscop->vs & SSCOP_SEQNUM_MASK);

      /* save the current poll sequence number in the packet */
      pak->sscop_nps = sscop->vps;

      /* packet being transmitted the first time, so rts = 0 */
      pak->flags &= ~PAK_SSCOP_RTS; 

      if (sscop_debugEvent)
         buginf("\nSSCOP: o Sd, %s state, v(s) = %d, v(ms) = %d, v(ps) = %d, pad=%d",
              sscop_printState(sscop->state), sscop->vs, sscop->vms,sscop->vps,pad); 

      /* THIS SHOULD ALSO APPLY TO 3.0 CODE AFTER WE'RE CONFIDENT TO IT */
      /* save pak till acknowledged */
      if ( sscop_isV31(sscop) ) {
	if (pak->refcount != 1)  /* remove this. it's only for debugging */
	  buginf("\nSCPERR(%d) - transmitProcess, refcount %d",sscop->index,
		 pak->refcount);
	pak_lock(pak);
	sscop_queueSdOnAckQ(sscop, pak);
      }
      else      {
	dup = pak_duplicate(pak);
	if (dup) 
	  sscop_queueSdOnAckQ(sscop, dup);
	else {
	  /* no buffers available - serious error!! */
	  datagram_done(pak);
	  return;
	}
      }

      /* 
       * Send it to ATM driver, it will encap the packet in AAL5 before
       * sending it out.
       */ 
      atmSig_sendToAal(sscop, pak);

      /* send sequence number incremented AFTER packet has been transmitted */
      SSCOP_INCR_CNTR(sscop->vs); 

      /*
       * vtPd is the Poll Data state variable and indicates the number of Sd Pdus
       * sent since the last transmission of a Poll Pdu.  It's incremented upon
       * transmission of a SD Pdu and reset to zero upon transmission of a Poll
       * Pdu.
       */
      SSCOP_INCR_CNTR(sscop->vtPd);

      if ( sscop_isV31(sscop) ) {
        if (!sscop_timerPollRunning(sscop)) {
          /* either Timer_IDLE or Timer_KEEP-ALIVE is running */
          if (sscop_timerIdleRunning(sscop)) {
            sscop_stopTimerIdle(sscop);
            sscop_startTimerNoResponse(sscop);
          }
          else  /* Timer_KEEP-ALIVE must be running */
            sscop_stopTimerKeepalive(sscop);
        }

        if (sscop->vtPd >= sscop->maxPd)
          sscop_transmitPollPdu(sscop);

        sscop_startTimerPoll(sscop);
        continue;
      }
      if (sscop->vtPd >= sscop->maxPd) {
         sscop_transmitPollPdu(sscop);
         sscop->vtPd = 0;
       }
      sscop_stopTimer(sscop);
      sscop_startTimer(sscop, sscop->pollTimerValue);
      sscop->retryCount = 0;


   } /* while */

}



/*
 * s s c o p _ R e s e n d S d P d u ( )
 *
 * Resend a Sd from ack Q - note pak not removed from the ack Q.
 * The packet gets removed only when it's ack'd by the other side.
 * The packet resent matches the given seqnum.
 */
void sscop_resendSdPdu (sscopType *sscop, ulong seqnum)
{

   paktype *pak, **prev, *dup;
   sscop_sdPdu *pdu;
   ulong ns;
   leveltype status;
   int index;
   int find = 0;


   index = sscop->index;
   status = raise_interrupt_level(NETS_DISABLE);
   prev = &sscop_ackQ[index];
   pak = *prev;

   while (pak) {
      pdu = (sscop_sdPdu *)( pak->datagramstart + pak->datagramsize -
                                                   sizeof(sscop_sdPdu) );
      ns = pdu->type_ns & SSCOP_SEQNUM_MASK;
      if (ns == seqnum) {
	if ( sscop_isV31(sscop) ) {
	  if (pak->refcount == 1) {
	    pak_lock(pak);
	    atmSig_sendToAal(sscop, pak);
	  }
	  else {
	    /* the first packet is still waiting to go out.
	       no need to resend it */
	    if (sscop_debugError)
	      buginf("\nSSCOP(%d) - o resendSdPdu, count %d, ns %d",
		     sscop->index,pak->refcount,ns);
	  }
	}
	else
	{
	  dup = pak_duplicate(pak);
	  if (dup)
            atmSig_sendToAal(sscop, dup);
	}
	find = 1;
	break;
      }
      prev = &pak->next;
      pak = pak->next;
   }
   reset_interrupt_level(status);


   /* can't find the pdu which needs to be retransmitted */
   if (!find) {
     if ( sscop_isV31(sscop) )
       sscop_sequenceError(sscop,MAA_ERROR_T);
     if (sscop_debugEvent) 
       buginf("\nSSCOPERR: resendSdPdu: no packet in AckQ with seqnum %d", seqnum);
   }


}



/*
 * s s c o p _ c r e a t e S o l i c i t e d L i s t ( )
 *
 * Create Solicited List.  The solicited list consists of missing pdu's in a bunch of
 * received pdu's.  Odd element of the list represents first pdu of a missing gap,
 * even elements represents first pdu of a received sequence.  There could be
 * multiple odd-even pairs.  Scan the list from v(r) to vr(h) to find all the pdu's
 * that did not make it through.
 * Returns the number of list elements added (used by STAT Pdu).
 */
int sscop_createSolicitedList (sscopType *sscop, paktype *pak)
{

   ulong ns;
   int listLength = 0;
   sscop_list *list;
   int loop = 0;


   list = (sscop_list *)pak->datagramstart;

   if (sscop_debugEvent)
      buginf("\nSSCOP: createSolicitedList: v(r) = %d, vr(h) = %d", sscop->vr,
                                                                  sscop->vrh);

   /* 'vr' is the next in sequence frame to be received */
   ns = sscop->vr;

start:
   if (ns == sscop->vrh)
      return (listLength);

   /* 'vrh' is next highest numbered frame to be rcvd.  scan from v(r) to vr(h) */

  /* 
   * Odd elements in STAT list represents first missing Pdu of a missing gap. 
   * Pick up first missing pdu between v(r) & vr(h) and quit below loop.
   */
  while (ns < sscop->vrh) {
      if (sscop_isSdInRcvQ(sscop, ns) )
         ++ns;
      else { 
         /* Append ns to list */
         list->ns = ns & SSCOP_SEQNUM_MASK;
         if (sscop_debugEvent)
            buginf("\nSSCOP: createSolicitedList: added n(s) = %d to Stat list", ns);
         ++list;
         listLength += sizeof(sscop_list);  
         break;
      }
      if (++loop > 100) {
         if (sscop_debugEvent)
            buginf("\nSSCOP: loop 1 in list creation");
         break;
      }
   }

   if (ns >= sscop->vrh) {
      list->ns = ns & SSCOP_SEQNUM_MASK;
      ++list;
      listLength += sizeof(sscop_list);
      if (sscop_debugEvent)
	buginf("\nSSCOP: createSolicitedList: added n(s) = %d to Stat list", ns);
      return (listLength);
   }

   ++ns;
   loop = 0;

   /* 
    * Even elements in STAT list represents first Pdu of a rcvd sequence
    * Pick up first rcvd pdu between v(r) and vr(h) and quit below loop.
    */
   while (ns < sscop->vrh) {
      if ( sscop_isSdInRcvQ(sscop, ns) ) {
         /* Append ns to list */
         list->ns = ns & SSCOP_SEQNUM_MASK;
         if (sscop_debugEvent)
            buginf("\nSSCOP: createSolicitedList: added n(s) = %d to Stat list", ns); 
         ++list;
         listLength += sizeof(sscop_list);  
         goto start;
      }
      else
         ++ns;

      if (++loop > 100) {
         if (sscop_debugEvent)
            buginf("\nSSCOP: loop 2 in list creation");
         break;
      }

   }

   if (ns >= sscop->vrh) {
     list->ns = ns & SSCOP_SEQNUM_MASK;
     ++list;
     listLength += sizeof(sscop_list);  
     if (sscop_debugEvent)
       buginf("\nSSCOP: createSolicitedList: added n(s) = %d to Stat list", ns); 
     return (listLength);
   }

   return (listLength);

 }



/*
 * a t m S i g _ s e n d T o A a l 
 * 
 * Sends the packet to the ATM driver for AAL5 encapsulation, after which the
 * packet is sent out on the line.
 */
void atmSig_sendToAal (sscopType *sscop, paktype *pak)
{

   vc_info_t *vc;
   atm_db_t *aip;


   if (sscop_debugSuspend) {
     datagram_done(pak);
     return;
   }

   pak->if_output = sscop->idb->firstsw;

   /* get the aip block */
   aip = pak->if_output->hwptr->atm_db;

   /* get the signalling vc block */
   vc = atm_getvc(aip, aip->sig_vcnum);

   /*
    * fatal error if signalling vc block not found - a default or configured
    * signalling vc is always present!!.
    */
   if (!vc) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: sendToAal: signalling pvc not found");
      datagram_done(pak);
      return; 
   }

   if (sscop_debugPacket)
      atmSig_hexout(pak->datagramstart, pak->datagramsize, "SSCOP(O)");

   /* 
    * encap the packet  - the routine will mostly return FALSE if the ATM hw
    * is not completely up and active.  In such a case just free the packet.
    */
   if ( !atm_saal_encap(pak, vc) ) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: sendToAal: encap failure");
      datagram_done(pak);
      return;
   }

   /* get it out!! */
   datagram_out(pak);

   /* UNI 3.1 use Timer_KEEP-ALIVE different from 3.0, so this timer
      is not used by the 3.1 code, it should not be started */
   /* remember time the last packet was sent for keepalives */
   if ( !sscop_isV31(sscop) )
     mgd_timer_start(&sscop->keepalive_timer, sscop->keepaliveTimerValue);

}



/*
 * s s c o p _ r e s e t R e c e i v e r ( )
 *
 * Reset Receiver.  
 * Empty out of sequence receive queue, reset receive sequence numbers.  A received 
 * Resync Pdu will cause us to reset all our Receiver queue and state variables.
 */
void sscop_resetReceiver (sscopType *sscop)
{

   paktype *pak, *pak1;


   sscop->vr = sscop->vrh = sscop->vmr = 0;

   /* free all the received out of sequence Sd's */
   pak = sscop->rcvQ;
   while (pak) {
      pak1 = pak;
      pak = pak->next;
      datagram_done(pak1);
      sscop_rcvQ_cnt--;
   }

}



/*
 * s s c o p _ r e s e t T r a n s m i t t e r ( )
 *
 * Reset Transmitter.
 */
void sscop_resetTransmitter (sscopType *sscop) 
{



}



/*
 *
 *   R  e  c  e  i  v  e      S  e  c  t  i  o  n 
 *
 */ 

/*
 * s s c o p _ p r o c e s s B e g i n P d u ( )
 *
 * Process an incoming Begin Pdu.
 * Returns FALSE for an invalid packet - incorrect size.
 */
int sscop_processBeginPdu (sscopType *sscop)
{

   sscop_beginPdu *pdu;
   paktype *pak;
   ulong nsq;


   pak = (paktype *)sscop->inbuf;

   /* there may be unknown UU field in 3.1 */
   if ( sscop_isV31(sscop) ) {
     pdu = (sscop_beginPdu *) (pak->datagramstart + pak->datagramsize -
                                                  sizeof(sscop_beginPdu));
   }
   else {
     pdu = (sscop_beginPdu *)pak->datagramstart;
     if ( pak->datagramsize != sizeof(sscop_beginPdu) ) {
       /* check for correct size, do this for 3.0 only */
       if (sscop_debugEvent)
	 buginf("\nSSCOP: incoming Begin pdu, incorrect size %d", pak->datagramsize);
       return (SSCOP_PDU_BAD);
     }
   }


   /*
    * Begin Pdu contains the max window of the receiver which is equal  
    * to the max number of frames we can transmit. 
    */
   sscop->vms = (pdu->type_window & SSCOP_SEQNUM_MASK);

   if ( sscop_isV31(sscop) ) {
     /* check for retransmission */
     nsq = pdu->nuu_nsq & SSCOP_NSQ_MASK;
     if ( (nsq == sscop->vrSq) && (sscop->state != SSCOP_ST_IDLE) ) {
       if (sscop_debugEvent)
         buginf("\nSCPEVNT(%d): incoming Begin pdu, retransmission, N(SQ) %d",
		sscop->index,nsq);
       if ( (nsq == 1) && (sscop->state == SSCOP_ST_DATA_XFER_READY) )
         return (SSCOP_PDU_NEW);
       else
         return (SSCOP_PDU_RETRANSMIT);
     }

     sscop->vrSq = nsq;
     if (sscop->vms > sscop->sendWindow)
       sscop->vms = sscop->sendWindow;
     /* remember the maxPd is initialized to 10 */
     if (sscop->vms < SSCOP_SENDWINDOW_DEFAULT)
       sscop->maxPd = sscop->vms;
   }
   else    {
     /* QSAAL.2, the LSB of NUU has to be 0 in UNI 3.0.
        possible unmatched uni version, print a warning message
	but still take it. */
     if (pdu->nuu_nsq != 0) 
       if (sscop_debugEvent)
         buginf("\nSCPEVNT(%d): rcv non-0 NUU in BeginPdu at UNI 3.0");
     if (sscop->vms > SSCOP_SENDWINDOW_DEFAULT)
       sscop->vms = SSCOP_SENDWINDOW_DEFAULT;
   }

   if (sscop_debugEvent)
      buginf("\nSSCOP: receive window in Begin Pdu = %d", sscop->vms);

   SSCOP_STATS(sscop->stats.InBeginPdu);

   return (TRUE);

}



/*
 * s s c o p _ p r o c e s s B e g i n A c k P d u ( )
 *
 * Process an incoming Begin Ack Pdu.
 * Returns FALSE for an invalid packet - incorrect size. 
 */
boolean sscop_processBeginAckPdu (sscopType *sscop)
{

   sscop_beginAckPdu *pdu;
   paktype *pak;


   pak = (paktype *)sscop->inbuf;

   if ( sscop_isV31(sscop) ) {
     pdu = (sscop_beginAckPdu *) (pak->datagramstart + pak->datagramsize -
                                                  sizeof(sscop_beginAckPdu));
   }
   else {
     pdu = (sscop_beginAckPdu *)pak->datagramstart;

     /* check for correct size */
     if ( pak->datagramsize != sizeof(sscop_beginAckPdu) ) {
       if (sscop_debugEvent)
	 buginf("\nSSCOP: i Begin Ack pdu, incorrect size %d", pak->datagramsize);
       return (FALSE);
     }
   }

   /*
    * Begin Ack Pdu contains the max window of the receiver which is
    * equal to the max number of frames we can transmit.
    */
   sscop->vms = pdu->type_window & SSCOP_SEQNUM_MASK;

   if ( sscop_isV31(sscop) ) {
     if (sscop->vms > sscop->sendWindow)
       sscop->vms = sscop->sendWindow;
     /* remember the maxPd is initialized to 10 */
     if (sscop->vms < SSCOP_SENDWINDOW_DEFAULT)
       sscop->maxPd = sscop->vms;
   }
   else
   {
     if (sscop->vms >= SSCOP_SENDWINDOW_DEFAULT)
       sscop->vms = SSCOP_SENDWINDOW_DEFAULT;
   }

   if (sscop_debugEvent)
      buginf("\nSSCOP: receive window in Begin Ack pdu = %d", sscop->vms);

   SSCOP_STATS(sscop->stats.InBeginAckPdu); 

   return (TRUE);

}



/*
 * s s c o p _ p r o c e s s B e g i n R e j P d u ( )
 *
 * Process an incoming Begin Reject Pdu.
 * Returns FALSE for an invalid packet - incorrect size.
 */
boolean sscop_processBeginRejPdu (sscopType *sscop)
{

   paktype *pak;


   pak = (paktype *)sscop->inbuf;

   /* check for correct size */
   if ( pak->datagramsize != sizeof(sscop_beginRejPdu) ) {
     /* there may be sscop-uu in uni 3.1. since we don't process
	the uu fields, so nothing should be done now */
     if ( !sscop_isV31(sscop) ) 
     {
       if (sscop_debugEvent)
         buginf("\nSSCOP: i Begin Rej pdu, incorrect size %d", pak->datagramsize);
       return (FALSE);
     }
   }

   /*
   if (sscop_debugEvent)
      buginf("\nSSCOP: i Begin Rej, %s state", sscop_printState(sscop->state) );
   */

   SSCOP_STATS(sscop->stats.InBeginRejPdu);

   return (TRUE);


}



/*
 * s s c o p _ p r o c e s s E n d P d u ( )
 *
 * Process an incoming End Pdu.
 * Returns FALSE for an invalid packet - incorrect size.
 */
boolean sscop_processEndPdu (sscopType *sscop)
{

   paktype *pak;

   pak = sscop->inbuf;

   /* check for correct size */
   if ( pak->datagramsize != sizeof(sscop_endPdu) ) {
     /* there may be sscop-uu in uni 3.1. since we don't process
	the uu fields, so nothing should be done now */
     if ( !sscop_isV31(sscop) ) 
     {
       if (sscop_debugEvent)
         buginf("\nSSCOP: i End pdu, incorrect size %d", pak->datagramsize);
       return (FALSE);
     }
   }

   /*
   if (sscop_debugEvent)
      buginf("\nSSCOP: i End pdu, %s state", sscop_printState(sscop->state) );
   */

   SSCOP_STATS(sscop->stats.InEndPdu);

   return (TRUE);

}



/*
 * s s c o p _ p r o c e s s E n d A c k P d u ( )
 *
 * Process an incoming End Ack Pdu.
 * Returns FALSE for an invalid packet - incorrect size. 
 */
boolean sscop_processEndAckPdu (sscopType *sscop)
{

   paktype *pak;
   int pduSize;

   pak = sscop->inbuf;


   if ( sscop_isV31(sscop) ) 
     pduSize = sizeof(sscop_endAck31Pdu);
   else
     pduSize = sizeof(sscop_endAckPdu);

   if ( pak->datagramsize != pduSize) {
     if (sscop_debugEvent)
       buginf("\nSSCOP: i End Ack pdu, incorrect size %d", pak->datagramsize);
     return (FALSE);
   }

   /*
   if (sscop_debugEvent)
      buginf("\nSSCOP: i End Ack pdu", sscop_printState(sscop->state) ); 
   */

   SSCOP_STATS(sscop->stats.InEndAckPdu);

   return (TRUE);

}



/*
 * s s c o p _ p r o c e s s U s t a t P d u ( )
 *
 */
void sscop_processUstatPdu (sscopType *sscop)
{

   sscop_ustatPdu *pdu;
   ulong seq1, seq2, nmr, nr;
   paktype *pak, *dup;


   pak = sscop->inbuf;
   pdu = (sscop_ustatPdu *)pak->datagramstart;

   /* exit if invalid format */
   if ( pak->datagramsize != sizeof(sscop_ustatPdu) ) {
      if (sscop_debugEvent)
         buginf("\nSSCOP: i Ustat pdu, incorrect size %d", pak->datagramsize);
      return;
   }

   /* list element 1 - a Sd pdu n(s) - represents first Pdu of a missing gap */
   seq1 = pdu->list1 & SSCOP_SEQNUM_MASK; 

   /* list element 2 - a Sd pdu n(s) - represents first Pdu of a received seq */
   seq2 = pdu->list2 & SSCOP_SEQNUM_MASK; 
 
   /* maximum acceptable receive sequence number */
   nmr = pdu->nmr & SSCOP_SEQNUM_MASK;

   /* sequence number of the next in-sequence Sd expected to be received */
   nr = pdu->type_nr & SSCOP_SEQNUM_MASK;

   if ( (sscop->vta > nr) || (sscop->vs < nr) ) {
      /* fatal error - disconnect the link and return */

     if ( sscop_isV31(sscop) )
       sscop_sequenceError(sscop,MAA_ERROR_T);

      return;
   }

   /* free Sd's from vta to (nr - 1) */
   while (sscop->vta < nr) {
      dup = sscop_removeSdFromAckQ(sscop, sscop->vta);
      if (dup)
         datagram_done(dup);
      SSCOP_INCR_CNTR(sscop->vta);
   } 

   sscop_incVms(nmr,sscop);

   /* if mark rts of SD.N(S) == seq1 set - exit */

   if ( (sscop->vta > seq1) || (seq1 > seq2) || (seq2 > sscop->vs) ) {
      /* fatal error - disconnect the link and return */
     if ( sscop_isV31(sscop) )
       sscop_sequenceError(sscop,MAA_ERROR_T);

     return;
   }

   /* retransmit all the missing SD's from seq1 to seq2 (not including seq2) */ 
   while (seq1 < seq2) {
      sscop_resendSdPdu(sscop, seq1);
      ++seq1;
   }

   SSCOP_STATS(sscop->stats.InUstatPdu);

   /* Poll after retransmission? */

}



/*
 * s s c o p _ p r o c e s s S t a t P d u ( )
 *
 */
void sscop_processStatPdu (sscopType *sscop)
{

   paktype *pak, *dup;
   sscop_statPdu *pdu;
   ulong ps, nmr, nr, seq1, seq2, vtPa;
   long nps;
   int listCount;    /* number of STAT list elements */
   ulong *list;
   boolean fatal_error = FALSE;

   pak = sscop->inbuf;
   pdu = (sscop_statPdu *)(pak->datagramstart + pak->datagramsize -
                                                       sizeof(sscop_statPdu));

   ps  = pdu->ps & SSCOP_SEQNUM_MASK;         /* poll state variable */ 
   nmr = pdu->nmr & SSCOP_SEQNUM_MASK;        /* max accpetable rcv seq number */ 
   nr  = pdu->type_nr & SSCOP_SEQNUM_MASK;    /* nr -1 frames acked by receiver */
   vtPa = sscop->vtPa;

   if (sscop_debugEvent) {
      buginf("\nSSCOP: processStatPdu: ps %d, nmr %d, nr %d", ps, nmr, nr);
      buginf("\nSSCOP: processStatPdu: vtPa %d, vps %d", sscop->vtPa, sscop->vps);
      buginf("\nSSCOP: processStatPdu: vta %d, vs %d", sscop->vta, sscop->vs);
   }

   if ( (ps < vtPa) || (ps > sscop->vps) ) {
      /* fatal error - disconnect link and return */
     /* COMBINE THE DEBUG MESSAGE AFTER REGRESSION TEST */
     if ( sscop_isV31(sscop) ) {
       if (sscop_debugError)
	 buginf("\nSCPERR(%d) - i processStatPdu: ps %d, vtps %d, vtpa %d",
		sscop->index,ps,sscop->vps,vtPa);
       sscop_sequenceError(sscop,MAA_ERROR_R);
     }
     else if (sscop_debugEvent) 
       buginf("\nSSCOP: processStatPdu: fatal error 1");

     fatal_error = TRUE;
   }

   if ( (sscop->vta > nr) || (sscop->vs < nr) ) {
     /* fatal error - disconnect the link and return */
     if ( sscop_isV31(sscop) ) {
       if (sscop_debugError)
	 buginf("\nSCPERR(%d): i processStatPdu, Nr %d, VtPa %d, VtS %d",
		sscop->index,nr,sscop->vta,sscop->vs);
       sscop_sequenceError(sscop, MAA_ERROR_S);
     }
     else if (sscop_debugEvent) 
       buginf("\nSSCOP: processStatPdu: fatal error 2");

     fatal_error = TRUE;
   }

   if (fatal_error) {
     sscop_initSscopCb(sscop);
     return;
   }

   if (sscop_debugEvent)
      buginf("\nSSCOP: processStatPdu: vta %d, nr %d", sscop->vta, nr);

   /* free Sd's from vta to (nr - 1) */
   while (sscop->vta < nr) {
      dup = sscop_removeSdFromAckQ(sscop, sscop->vta);
      if (dup)
         datagram_done(dup);
      SSCOP_INCR_CNTR(sscop->vta);
   } 
 
   sscop->vtPa = ps;

   sscop_incVms(nmr,sscop);

   /* find the number of list elements in STAT pdu */
   listCount = ( pak->datagramsize - sizeof(sscop_statPdu) ) / 4;

   if (sscop_debugEvent)
      buginf("\nSSCOP: processStatPdu: listCount = %d", listCount);

   if (!listCount) {
     /* everything cool. no missing sd pdu, redo the timers */
     if ( sscop_isV31(sscop) ) 
       sscop_setXferReadyTimers(sscop);

     return;
   }

   if (listCount == 1) {
     if (sscop_debugEvent)
       buginf("\nSSCOP: processStatPdu: listCount = %d", listCount);
     return;
   }

   list = (ulong *)pak->datagramstart;
   seq1 = *list & SSCOP_SEQNUM_MASK;
   --listCount;

   /* uni 3.1 may goto the outgoing recovery state while there's no such
      state in uni 3.0. it still can be merged however */
   if ( sscop_isV31(sscop) ) {
     if ( (seq1 > sscop->vs) && sscop_isV31(sscop) ) {
       sscop_sequenceError(sscop,MAA_ERROR_S);
       return;
     }

     do {
       ++list;
       seq2 = *list & SSCOP_SEQNUM_MASK;
       --listCount;
       /* seq1, seq2 represents the missing SD's from seq1 to (seq2 - 1) */
       if ( (seq1 >= seq2) || (seq1 > sscop->vs) || (seq2 > sscop->vs) ) {
	 /* fatal error - disconnect link and return */
	 if (sscop_debugError) {
	   buginf("\nSCPERR(%d): Vt(s) %d seq1 %d seq2 %d",sscop->index,sscop->vs,
		  seq1,seq2);
	 }

	 sscop_sequenceError(sscop,MAA_ERROR_S);
	 return;
       }

       while (seq1 < seq2) {
	 /* SD.N(S) = seq1 exists in ack Q? */
	 if ( sscop_findPsInSd(sscop, seq1) == -1 ) {
	   if (sscop_debugError)
	     buginf("\nSCPERR(%d): seq %d not in ackQ. Vt(PA) %d, Vt(PS) %d",
		    sscop->index,seq1,vtPa, ps);
	   sscop_sequenceError(sscop,MAA_ERROR_S);
	   return;
	 }

	 sscop_resendSdPdu(sscop, seq1);
	 ++seq1;
       }

       /*
	* seq1, seq2 now represent the SD's successfully received at the 
	* other side.
        */
       --listCount;
       if (listCount > 0) {
	 ++list;
	 seq2 = *list & SSCOP_SEQNUM_MASK;
	 if ( (seq1 >= seq2) || ( seq2 > sscop->vs) ) {
	   sscop_sequenceError(sscop,MAA_ERROR_S);
	   return;
	 }
	 
	 while (seq1 < seq2) {
	   pak = sscop_removeSdFromAckQ(sscop, seq1);
	   if (pak)
	     datagram_done(pak);
	   ++seq1;
	 }
       }
       
     } while (listCount > 0);

     sscop_maaErrorIndication(sscop->state,MAA_ERROR_V,sscop->index);
     sscop_setXferReadyTimers(sscop);
   }
   else {
start:
     ++list;
     seq2 = *list & SSCOP_SEQNUM_MASK;
     --listCount;

     if (sscop_debugEvent)
       buginf("\nSSCOP: processStatPdu: seq1 %d, seq2 %d", seq1, seq2);


     if ( (seq2 - seq1) > 100 ) {
       if (sscop_debugEvent)
         buginf("\nprocessStatPdu: seq2 > seq1 by 100");
       return;
     }

     while (seq1 < seq2) {
       
       nps = sscop_findPsInSd(sscop, seq1);
       
       if ( (nps != -1) && (nps < ps) && (ps <= sscop->vps) )
         sscop_resendSdPdu(sscop, seq1);

       ++seq1;
     }          

     /* 
      * seq1 now equals seq2, get the next item from the STAT list if it exists.
      * seq1, seq2 now represent the SD's successfully received at the other side. 
      */
     if (listCount) {

       ++list;
       seq2 = *list & SSCOP_SEQNUM_MASK;
       --listCount;

       if (sscop_debugEvent)
         buginf("\nSSCOP: processStatPdu: seq1 %d, seq2 %d", seq1, seq2);

       if ( (seq2 - seq1) > 100 ) {
         if (sscop_debugEvent)
	   buginf("\nprocessStatPdu: seq2 > seq1 by 100");
         return;
       }

       while (seq1 < seq2) {
         pak = sscop_removeSdFromAckQ(sscop, seq1);
         if (pak)
	   datagram_done(pak);
         ++seq1;
       }
     }

     /* seq1 now equals seq2 for the next round IF listCount is non-zero */
     
     if (listCount)
       goto start;
   }

   SSCOP_STATS(sscop->stats.InStatPdu);

} 



/*
 * s s c o p _ p r o c e s s R e s y n c P d u ( )
 * return 0 if it's a retransmission or bad, only
 * for UNI 3.1
 *
 */
int sscop_processResyncPdu (sscopType *sscop)
{
  paktype *pak;
  int nmr, nsq;
  sscop_resyncPdu *pdu;

  if ( sscop_isV31(sscop) ) {
    pak = sscop->inbuf;

    if (pak->datagramsize < sizeof(sscop_resyncPdu)) {
      if (sscop_debugError)
	buginf("\nSCPERR(%d) - i bad RS PDU, size %d",sscop->index,
	       pak->datagramsize);
      return (!SSCOP_RESULT_OK);
    }

    pdu = (sscop_resyncPdu *)(pak->datagramstart + pak->datagramsize -
                                                       sizeof(sscop_resyncPdu));

    nsq  = pdu->nuu_nsq & SSCOP_NSQ_MASK;
    if (nsq == sscop->vrSq) {
      if (sscop_debugEvent)
	buginf("\nSCPEVNT(%d) - i RS Pdu retransmission N(SQ) %d",sscop->index,nsq);

      return (!SSCOP_RESULT_OK);
    }

    sscop->vrSq = nsq;
    nmr = pdu->type & SSCOP_SEQNUM_MASK;
    sscop_incVms(nmr,sscop);

  }

  SSCOP_STATS(sscop->stats.InResyncPdu);
  return (1);


}



/*
 * s s c o p _ p r o c e s s R e s y n c A c k P d u ( )
 *
 */
int sscop_processResyncAckPdu (sscopType *sscop)
{
  paktype *pak;
  int nmr;
  sscop_resyncAck31Pdu *pdu;

  if ( sscop_isV31(sscop) ) {
    pak = sscop->inbuf;

    if ( pak->datagramsize != sizeof(sscop_resyncAck31Pdu) ) {
      if (sscop_debugError)
	buginf("\nSCPERR(%d) - i rcv bad uni 3.1 RSAK Pdu",sscop->index);

      return (0);
    }
    pdu = (sscop_resyncAck31Pdu *)(pak->datagramstart);

    nmr = pdu->type & SSCOP_SEQNUM_MASK;
    sscop_incVms(nmr,sscop);
  }


  SSCOP_STATS(sscop->stats.InResyncAckPdu);
  return (1);

}



/*
 *
 *    T i m e r    S e c t i o n 
 *
 */


/*
 * s s c o p _ t i m e r P r o c e s s ( )
 *
 * Sscop timer process.  Handles Sscop timeouts.
 */
process sscop_timerProcess (void)
{

  sscopType *sscop;
  mgd_timer *expired_timer;

   /* wait for system to initialize */
   process_wait_on_system_init();

   for (;;) {
     /* wait for any sscop interface to timeout */ 
     process_sleep_on_timer(&sscop_wakeupTime);

     /* process sscop's which have timed out */ 
     while(mgd_timer_expired(&sscop_wakeupTime)) {
       expired_timer = mgd_timer_first_expired(&sscop_wakeupTime);
       sscop = mgd_timer_context(expired_timer);

       switch (mgd_timer_type(expired_timer)) {

       case SSCOP_RETRY_TIMER:
	 sscopGetLock(sscop);
	 sscop_tickTock(sscop);
	 sscopClearLock(sscop);

	 break;

	 /* pjs *** check these out for LOCK ***/
       /* following 3 mgd timers are used by uni 3.1 code only */
       case SSCOP_IDLE_TIMER:
	 sscop_startTimerNoResponse(sscop);
	 /* let it fall through */

       case SSCOP_POLL_TIMER:
       case SSCOP_KEEPALIVE_TIMER_V31:
	 mgd_timer_stop(expired_timer); 

	 if (sscop->state != SSCOP_ST_DATA_XFER_READY) {
	   if (sscop_debugError || sscop_debugEvent)
	     buginf("\nSCPERR(%d)-timer %d expiry in %s state",
		    sscop->index,mgd_timer_type(expired_timer),
		    sscop_printState(sscop->state));
	   break;
	 }

	 sscopGetLock(sscop);
	 sscop_transmitPollPdu(sscop);
	 sscop_setPollTimer(sscop);
 	 sscopClearLock(sscop);

	 break;
	 
       case SSCOP_ATMSIG_T309:
	 mgd_timer_stop(expired_timer);
	 if (sscop_debugError || sscop_debugEvent) {
	   buginf("\nSCPERR(%d) - T309 expiry",sscop->index);
	   if (sscop->state != SSCOP_ST_OUTCONN_PENDING)
	     buginf("\n\nSCPERR(%d) - t309 expiry not in outConnState\n\n",
		  sscop->index);
	 }

	 /* it's time to tear down all the SVCs */
	 sscopGetLock(sscop);
	 sscop_initSscopCb(sscop);
 	 sscopClearLock(sscop);

	 break;

       default:
	 errmsg(&msgsym(UNEXPECTEDTIMER, SCHED), expired_timer,
		mgd_timer_type(expired_timer));

	 mgd_timer_stop(expired_timer); 
	 break;
       }

     }

   }

}
 


/*
 * s s c o p _ t i c k T o c k ( )
 *
 * Managed timers return sscop which  timed out. 
 * Retransmits pdu's in the appropriate state.  If retry count exceeded
 * transmits a End pdu and disconnects the session.
 * for UNI 3.1 code, this function is only called when Timer_CC expired.
 */
void sscop_tickTock (sscopType *sscop)
{

   switch (sscop->state) {

   /* 
    * If retry count not exceeded, send another Begin Pdu, else send an
    * End Pdu and disconnect the session (go back to Idle state).
    */ 
   case SSCOP_ST_OUTCONN_PENDING:
      sscop_stopTimer(sscop);  
      if (sscop->retryCount < sscop->maxRetryCount) {
         sscop_transmitBeginPdu(sscop);
         ++sscop->retryCount;
         sscop_startTimer(sscop, sscop->ccTimerValue);
       }
      else {
	if ( sscop_isV31(sscop) )
	  sscop_maaErrorIndication(sscop->state,MAA_ERROR_O,sscop->index);
         sscop_transmitEndPdu(sscop);
         sscop_initSscopCb(sscop);
      } 
      break;

    /*
     * If retry count not exceeded, send another Poll Pdu, else send an
     * End Pdu and disconnect the session (go back to Idle state).
     */
    case SSCOP_ST_DATA_XFER_READY:
      sscop_stopTimer(sscop);
      /* this should not happen in uni 3.1 code */
      if ( sscop_isV31(sscop) )   /* debugging only */ {
	buginf("\n=====\nSCRERR(%d) - Timer_CC expire in data xfer ready state\n====\n",
	       sscop->index);
	break;
      }

      if (sscop->retryCount < sscop->maxRetryCount) {
	sscop_transmitPollPdu(sscop);
	++sscop->retryCount;
	sscop_startTimer(sscop, sscop->pollTimerValue);
      }
      else {
	sscop_transmitEndPdu(sscop);
	sscop_initSscopCb(sscop);
      }
      break;

    /*
     * If retry count not exceeded, send another End Pdu, else disconnect 
     * the session (go back to the Idle state).
     */ 
    case SSCOP_ST_OUTDISCONN_PENDING:
      sscop_stopTimer(sscop);
      if (sscop->retryCount < sscop->maxRetryCount) {
	sscop_transmitEndPdu(sscop);
	++sscop->retryCount;
	sscop_startTimer(sscop, sscop->ccTimerValue);
      }
      else {
	if ( sscop_isV31(sscop) )
	  sscop_maaErrorIndication(sscop->state,MAA_ERROR_O,sscop->index);

	sscop_initSscopCb(sscop);
      }
      break;

    /*
     * If retry count not exceeded, send another Resync Pdu, else send an
     * End Pdu and disconnect the session (go back to Idle state).
     */
    case SSCOP_ST_OUTRESYNC_PENDING:
      sscop_stopTimer(sscop);
      if (sscop->retryCount < sscop->maxRetryCount) {
	sscop_transmitResyncPdu(sscop);
	++sscop->retryCount;
	sscop_startTimer(sscop, sscop->ccTimerValue);
      }
      else {
	if ( sscop_isV31(sscop) )
	  sscop_maaErrorIndication(sscop->state,MAA_ERROR_O,sscop->index);

	sscop_transmitEndPdu(sscop);
	sscop_initSscopCb(sscop);
      }
      break;

    case SSCOP_ST_OUTRECOVERY_PENDING:  /* only for UNI 3.1 */
      sscop_stopTimer(sscop);
      if (sscop->retryCount < sscop->maxRetryCount) {
	sscop_transmitErPdu(sscop);
	++sscop->retryCount;
	sscop_startTimer(sscop, sscop->ccTimerValue);
      }
      else {
	sscop_maaErrorIndication(sscop->state,MAA_ERROR_O,sscop->index);
	sscop_transmitEndPdu(sscop);
	sscop_aaReleaseIndication(sscop);
	sscop_requestConnection(sscop);
      }
      break;

      /* Invalid state for a timeout to occur - log it!! */
    default:
      sscop_stopTimer(sscop);

      break;

    } 


}



/*
 * s s c o p _ s t a r t T i m e r ( )
 *
 * Start sscop timer.  The timeout value is passed to the routine.
 * Only one timer per SSCOP control block should be active at a time.
 */
void sscop_startTimer (sscopType *sscop, ushort delta)
{

  mgd_timer_start(&sscop->timeoutValue, delta);
    
}



/*
 * s s c o p _ s t o p T i m e r ( )
 *
 */
void sscop_stopTimer (sscopType *sscop)
{

  mgd_timer_stop(&sscop->timeoutValue);

}


/*
 * it updates the sscop->vms from the rcv pdu
 */
void sscop_incVms (int vms, sscopType *sscop)
{
  int vms_default;

  if ( sscop_isV31(sscop) )
    vms_default = sscop->sendWindow;
  else
    vms_default = SSCOP_SENDWINDOW_DEFAULT;

  if ((sscop->vms + vms_default) < vms)
    sscop->vms += vms_default;
  else
    sscop->vms = vms;

}

/** ILMI start here **/
ilmiClient sscop_ilmiClient;

/*
 *
 * Register with ILMI.
 * Called (sscop_init) at initialization.
 */
boolean sscop_registerIlmi (void)
{

  static boolean registered = FALSE;

  extern ilmiClient sscop_ilmiClient;    /* declared extern */
  ilmiClientReq crq;

  if (registered) {
    if (sscop_debugEvent)
      buginf("\registerIlmi, already registered");
    return(TRUE);
  }

  sscop_ilmiClient = reg_invoke_atm_ilmi_register_client("sscop");
	
  if (!sscop_ilmiClient) {
    if (sscop_debugEvent)
      buginf("\registerIlmi, no ilmiClient");
    return(FALSE);
  }

  crq.client_id = sscop_ilmiClient;
  crq.notifyCB = sscop_ilmiCallback;

  crq.notify_type = ilmiUniVersionNotify;
  reg_invoke_atm_ilmi_register_service(&crq);

  if (sscop_debugEvent)
    buginf("\nSSCOP: registerIlmi, OK");

  return(TRUE);

}


/*
 * ILMI callback function
 * Only handle the services that we registered for
 */
void sscop_ilmiCallback (void *rq)
{
  ilmiClientReq *crq;
  hwidbtype *hwptr;
  atm_db_t *atm_db;
  sscopType *sscop;
  int peer_uni;
  
  crq = rq;
  if (sscop_debugEvent || sscop_debugError) 
    buginf("\nSCPEVNT: ilmi notify uni version %d, int %d",crq->myUniVersion,
	   crq->intf_id);

  switch (crq->notify_type) {

  case ilmiUniVersionNotify:
    FOR_ALL_HWIDBS(hwptr) {
      if (hwptr->hw_if_index == crq->intf_id) 
	break;
    }
    /* paranoia */
    if ( (!hwptr) || (!(hwptr->status & IDB_ATM)) ) {
      if (sscop_debugEvent || sscop_debugError)
	buginf("\nSCPERR: ilmiCallback, not ATM port, callback failed, %x",hwptr);
      return;
    }

    atm_db = hwptr->atm_db;
    /* paranoia */
    if (!atm_db) {
      if (sscop_debugEvent || sscop_debugError)
	buginf("\nSCPERR: ilmiCallback, no atm_db");
      return;
    }

    if (atm_db->ignore_peer_uni)  /* user want to force the uni version */ {
      if (atm_db->uni_version == ATMUNI_VERSION_31)
	hwptr->sscop->keepaliveTimerValue = SSCOP_KEEPALIVE_V31_DEFAULT;
      else if (atm_db->uni_version == ATMUNI_VERSION_30)
	hwptr->sscop->keepaliveTimerValue = SSCOP_KEEPALIVETIMER_DEFAULT;

      if (sscop_debugEvent || sscop_debugError) 
	buginf("\nSCPEVNT:ignore peer uni version, int %d\n",crq->intf_id);

      return;   
    }

    sscop = hwptr->sscop;
    if (!sscop) {
      if (sscop_debugEvent || sscop_debugError)
	buginf("\nSCPERR: ilmiCallback, no sscop");
      return;
    }


    /* get peer uni version from atm_db */

    peer_uni = crq->myUniVersion;

    switch (peer_uni) {
    case ATMUNI_VERSION_UNSUPPORT:
    case ATMUNI_VERSION_INVALID:
      if (sscop_debugEvent)
        buginf("\nSCPEVNT - ilmi notification invalid uni version");
        break;

    case ATMUNI_VERSION_40:
    case ATMUNI_VERSION_31:
    case ATMUNI_VERSION_30:
    default:
      if (peer_uni == sscop->uni_version)
	break;  /* nothing need to be done */

      /* sscop initialization has not completed, let interfaceReset
         do the job */
      if (sscop->uni_version == ATMUNI_VERSION_INVALID)
	break;

      if (sscop->state != SSCOP_ST_IDLE) {
	if (sscop_debugEvent || sscop_debugError)
	  buginf("\nSCPEVNT(%d): notify peer uni version, not in idle state",
		 sscop->index);
	/* tear down everything, send end to the other side */
	sscop_transmitEndPdu(sscop);
      }

      sscop_interfaceReset(hwptr);

      if (sscop_debugEvent || sscop_debugError)
	buginf("\nSCPEVNT(%d) - peer-uni %d, my uni %d",crq->intf_id,peer_uni,
	       sscop->uni_version);
      break;

    }


  default:
    if (sscop_debugError || sscop_debugEvent)
      buginf("\nSCPERR: ilmiCallback with invalid notify type");
    break;
  }

}

/*
 * Handle SSCOP Poll Pdu at interrupt here.
 * Enabled with the "sscop quick-poll" command.
 * Try to handle SSCOP Poll Pdu immediately when seen by ATM driver
 * This happens at interrupt level. So, make it and all called
 * routines as streamlined as possible.
 * Use tmp variables. Don't update sscop block until we know that we 
 * won't fail.
 * We only get here if this is a Poll Pdu.
 * Dispense with the standard Pdu checks.
 */
static tinybool sscop_quick_processPollPdu (sscopType *sscop)
{

  paktype *pak;
  sscop_pollPdu *pdu;
  ulong ns;
  ulong tmp_vrh;
  ulong tmp_incomingPs;


  pak = sscop->inbuf;
  pdu = (sscop_pollPdu *)pak->datagramstart;
  tmp_incomingPs = pdu->ps & SSCOP_SEQNUM_MASK;
  ns = pdu->type_ns & SSCOP_SEQNUM_MASK;

  /* Buginf @ interrrupt 
   * "*" in debug indicates interrupt level
   */
  if (sscop_debugEvent)
    buginf("\nSSCOP: * Poll pdu, ns = %d, nps = %d", ns, tmp_incomingPs);

  /*
   * 'ns' is the seq number of the next frame the remote side will send us.
   * 'vrh' is the highest sequence number we have received so far.
   * Clearly 'ns' needs to be greater than 'vrh' for correct operations.  If
   * ns < vrh then the remote side's transmitter is badly broken and it would
   * best to reestablish the link.
   */ 
  if (sscop->vrh > ns)
    return(FALSE);

  if (sscop->vmr < ns)
    tmp_vrh = sscop->vmr;
  else
    tmp_vrh = ns;

  /* Verify no missing Pdu's ("Solicited List") */
  if (sscop->vr != tmp_vrh)
    return (FALSE);     
 
  /* Looks good, no failures from this point on */
  sscop->vrh = tmp_vrh;
  sscop->incomingPs = tmp_incomingPs;

  SSCOP_STATS(sscop->stats.InPollPdu);

  return(TRUE);

}


/*
 * Reuse incoming Poll pak for Stat Pdu
 * By the time we get here, everything must be OK.
 * No failure cases.
 */
static void sscop_quick_transmitStatPdu (sscopType *sscop)
{

  paktype *pak;
  sscop_statPdu *pdu;


  /* Reuse incoming Poll Pdu buffer */
  pak = sscop->inbuf;

  /* *** Start writing pak here, can't fail later *** */
  pak->datagramsize = sizeof(sscop_statPdu);
  pdu = (sscop_statPdu *)(pak->datagramstart);
  pdu->ps = sscop->incomingPs;
  pdu->type_nr = (SSCOP_MT_STAT) | (sscop->vr & SSCOP_SEQNUM_MASK);

  /* 
   * increment sender's window by v(r) + SSCOP_RCVWINDOW_DEFAULT, don't 
   * increment our receive window till the previous window of frames is fully
   * received. Make sure that v(mr) is still greater than vr(h).
   */
  sscop->vmr = sscop->vr + SSCOP_RCVWINDOW_DEFAULT; 
  pdu->nmr = sscop->vmr & SSCOP_SEQNUM_MASK;

  /* Buginf @ interrrupt */
  if (sscop_debugEvent)
    buginf("\nSSCOP: o Stat pdu, n(r) = %d, n(mr) = %d, n(ps) = %d",
	   sscop->vr, sscop->vmr, sscop->incomingPs);

  SSCOP_STATS(sscop->stats.OutStatPdu);

  return;

}


/*
 * Send to driver
 * Sscop, vc, pak are known to be good.
 */
static void 
sscop_quick_sendToAal (sscopType *sscop, vc_info_t *vc, paktype *pak)
{

  pak->if_output = pak->if_input;

  /* Buginf @ interrrupt */
  if (sscop_debugPacket)
    atmSig_hexout(pak->datagramstart, pak->datagramsize, "SSCOP(O)");

  /* Encap of pak, reuse vc, won't fail */
  atm_saal_encap(pak, vc);

  /* Send it */
  datagram_out(pak);

  /* Remember time the last packet was sent for keepalives */
  if ( !sscop_isV31(sscop) )
    mgd_timer_start(&sscop->keepalive_timer, sscop->keepaliveTimerValue);

  return;

}



/*
 * Handle SSCOP Poll Pdu at interrupt
 */
tinybool sscop_handle_poll_pdu (paktype *pak, vc_info_t *vc)
{

  idbtype *idb;
  sscopType *sscop;
  ulong *pduType;


  idb = pak->if_input;
  if (!idb) {
    return(FALSE);
  }
    
  sscop = idb->hwptr->sscop;
  
  /* Don't need to check !atm_interface_up(idb) here. */
  if (!sscop || !sscopEnabled || (sscop->state != SSCOP_ST_DATA_XFER_READY)) {
    return(FALSE);
  }

  /* check for min size  - no point in going further if pdu too small. */ 
  if (pak->datagramsize < SSCOP_HEADER_SIZE) {
    return(FALSE);
  }

  /* All sscop packets MUST be long word aligned, toss packet if not. */
  if ( (pak->datagramsize % SSCOP_PADBYTES) != 0 ) {
    return(FALSE);
  }

  /* This replaces sscop_findPduType(pak, sscop); */
  pduType = (ulong *)(pak->datagramstart + pak->datagramsize - 
		      SSCOP_HEADER_SIZE);

  /* If not Poll Pdu, return FALSE immediately */
  if ( (*pduType & SSCOP_PDUTYPE_MASK) != SSCOP_MT_POLL)
    return(FALSE);

  /* It's a Poll. We'll take it. */ 
  sscop->inbuf = pak;

  /* Buginf @ interrrupt */
  if (sscop_debugPacket)
    atmSig_hexout(pak->datagramstart, pak->datagramsize, "SSCOP(I)");

  /* Verify good Poll */
  if (!sscop_quick_processPollPdu (sscop))
    return(FALSE);

  /* Can't fail from here */
  
  /* Build the Stat Pdu */
  sscop_quick_transmitStatPdu(sscop);

  /* Put timestamps at the end */
  GET_TIMESTAMP(pak->inputtime);
  COPY_TIMESTAMP(pak->inputtime, sscop->last_input);
  COPY_TIMESTAMP(pak->inputtime, idb->hwptr->lastinput);

  /* Send it out on same vc as incoming */
  sscop_quick_sendToAal(sscop, vc, pak);

  /* Null the inbuf. We don't have it anymore */
  sscop->inbuf = NULL;

  return(TRUE);
}


/*
 * SSCOP subsystem header
 */

#define SSCOP_MAJVERSION       10
#define SSCOP_MINVERSION       0
#define SSCOP_EDITVERSION      1

SUBSYS_HEADER(sscop, SSCOP_MAJVERSION, SSCOP_MINVERSION, SSCOP_EDITVERSION,
              sscop_init, SUBSYS_CLASS_PROTOCOL,
              "seq: atmsig",
              "req: atmsig");

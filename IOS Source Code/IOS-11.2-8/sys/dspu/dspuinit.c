/* $Id: dspuinit.c,v 3.4.4.5 1996/08/17 18:35:24 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspuinit.c,v $
 *------------------------------------------------------------------
 * DSPU Initialization.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module provides the DSPU Initialization code which is invoked
 * from the router boot initialization code.
 *------------------------------------------------------------------
 * $Log: dspuinit.c,v $
 * Revision 3.4.4.5  1996/08/17  18:35:24  ppearce
 * CSCdi65375:  Infinite retries for NSP over APPN overflows TTY proc stack
 * Branch: California_branch
 *   Don't register DspuTimeOut() routine to be invoked by TTY process
 *     via reg_invoke_onesec()
 *   Define DSPU background timer and use process_watch_timer() instead
 *
 * Revision 3.4.4.4  1996/08/16  18:32:29  lmetzger
 * CSCdi66255:  Add Modularity of RTR for DSPU/SNANM
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/07/31  21:41:47  lmetzger
 * CSCdi64744:  Move Registry Initialization to the RTT subsystem
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/05/17  10:48:17  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.6  1996/05/06  12:59:34  doner
 * CSCdi56333:  Thrashing messages at initialization of dspu
 * Branch: IbuMod_Calif_branch
 * Avoid thrashing messages by processing more queue entries.
 *
 * Revision 3.2.48.5  1996/04/19  02:39:06  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.2.48.4  1996/04/03  14:03:03  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.48.3  1996/03/30  08:10:33  doner
 * CSCdi52657:  IBU modularity - DSPU conversion to use new IOS scheduler
 * Branch: IbuMod_Calif_branch
 * Update to new scheduler interface
 *
 * Revision 3.2.48.2  1996/03/22  18:50:37  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2.48.1  1996/03/17  17:38:43  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.4.1  1996/03/18  19:33:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:40:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  08:45:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/11  18:24:47  ppearce
 * CSCdi51263:  Provide a more elegant fix for CSCdi49872
 *
 * Revision 3.3  1996/03/05  03:25:47  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.2  1995/11/17  09:05:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:56:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/11  04:20:50  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.6  1995/09/20  17:03:20  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.5  1995/09/14 06:15:20  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.4  1995/08/09  13:16:17  ppearce
 * CSCdi38432:  dspu subsystem causing confusion, rename it
 *   Change dspu subsystem name in subsytems that require dspu
 *
 * Revision 2.3  1995/08/08  22:42:10  richl
 * CSCdi38432:  dspu subsystem causing confusion, rename it
 *
 * Revision 2.2  1995/07/27  13:50:27  rbatz
 * CSCdi37660:  Focal Point frames get stuck in Transmit Queue.
 *
 * Revision 2.1  1995/06/07  20:28:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "subsys.h"

#include "dspu.h"
#include "dspu_pool.h"
#include "dspu_sap.h"
#include "bfrprim.h"
#include "dspu_itimer.h"
#include "dspu_fp.h"
#include "dspu_notification.h"
#include "mv_prodid.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
Dspu dspu;           /* global variable  */

static TCLSIQueue* gDLUReadQ;
static TCLSIQueue* gCLSWriteQ;

#define DSPU_MAIN_PROCESS    "DSPU Msg Proc"


/*
 * Events which awaken the DSPU main line
 */

#define DSPU_PRIORITY_READ_Q_NAME "DSPU Priority Read Queue"
#define DSPU_READ_Q_NAME "DSPU Read Queue"
#define DSPU_FP_HASDATA_NAME "DSPU Focal Point has data"

#define DSPU_READ_Q_LOOP_MAX 20 

/* 
 *  minor codes for dspu events 
 */
typedef enum dspu_events_ {
    DSPU_PRIORITY_READ_QUEUE_EVENT,
    DSPU_READ_QUEUE_EVENT,
    FP_HASDATA_EVENT
} dspu_events;  

/*
 * Watched items
 */
static watched_queue *pDspuPriorityReadQueue;
static watched_queue *pDspuReadQueue;
static watched_boolean *fpHasData;   /*Set TRUE only by FpHasDataToSend */

static int bad_event;       /*Indicates process awakened for an event
			      which is not expected - here for debugging*/

static void    DspuMain (void); 

/* 
 * Add message to the appropriate queue which wakes up DspuMain process 
 */
static int DspuRput(TCLSIQueue* dspuReadQ, TCLSIMsg* theMsg)
{
   /* Note: dspuReadQ is never used */

    process_enqueue((CLSIMsgGetPrimitiveCode (theMsg) == 
		     (CLS_FLOW | CLS_INDICATION))
		     ?pDspuPriorityReadQueue:pDspuReadQueue, 
		    (void *) theMsg);  

   return(CLS_OK);
}

/*============================================================================*
*  FpHasDataToSend
*=============================================================================*
*
* Description:
*   Focal point has sscp pu data to send. Set flag to drive dspu to send data
*
**/

void FpHasDataToSend (void)
{
    process_set_boolean(fpHasData, TRUE);
}

/*============================================================================*
* sna_pu_init ()
*=============================================================================*
*
* Description:
*   This entrypoint is called at boot/initialization time.
*
* Historical note.  The DSPU stack is used in images which support network
* management.  This was causing a lot of confusion with images that support
* IBM alerts, but not the DSPU feature.  For that reason, the DSPU subsystem
* is renamed to sna_pu.
*
**/
static void sna_pu_init (subsystype *subsys)
{

   InitChild(MULTIPLE,&dspu.adaptorRoot);
   InitChild(MULTIPLE,&dspu.upstrLsRoot);
   InitChild(DSPU_INDEX,GetDnlsRoot());
   InitChild(MULTIPLE,&dspu.upstrPuRoot);
   InitChild(DSPU_INDEX,GetInactDpuRoot());
   
   memset(&dspu.rsrb.virtualMac, 0, sizeof(MacAddrT));
   dspu.rsrb.targetRing = 0;
   dspu.rsrb.bridgeNum = 0;
   dspu.rsrb.localRing = 0;
   dspu.rsrb.snaOrDspu = FALSE;
   
   dspu.defaultPuP = 0;            /* no default config yet */
   dspu.debugLevel = 0;            /* no debug */

   SetDspuStarted(FALSE);         /* DSPU Main process not started */

   /* 
    * Initialize the static ProductSetId control vector for this platform
    *
    */
   ProductIdInit();

}  /* sna_pu_init. */


/*=========================================================================*
* dspu_cls_init ()
*========================================================================*
*
* Description:
*   Initializes CLS queues used for DSPU-to-CLS interprocess communication
*
*/
static void dspu_cls_init (void)
{

    TCLSIQInit* qInit = CLSIQInitCreate("DSPU", DspuRput); /*qname, 
							     define put*/
    if (qInit == NULL)
	return;

    gDLUReadQ = CLSIQueueCreate(qInit, NULL);
    if (gDLUReadQ == NULL) {
        CLSIQInitDestroy(qInit);
        return;
    }

    gCLSWriteQ = CLSOpen(gDLUReadQ);

    if (gCLSWriteQ == NULL) {
        CLSIQueueDestroy(gDLUReadQ);
        CLSIQInitDestroy(qInit);
	return;
    }

} /* End dspu_cls_init() */


/*=========================================================================*
* dspu_start
*========================================================================*
*
* Description:
*   Called when DSPU configured to start DSPU Main Process
*
*/
void dspu_start (void)
{
    int pid;

    SapSetActWindow(DEFAULT_ACTWINDOW);

    DspuBufferInit();               /* initialize dspu buffers */
    InitialiseItimerQueues();

    InitPoolRoot ();
   
    /*
     * Set up the queues and events for DspuMain process
     */
    pDspuPriorityReadQueue = create_watched_queue(DSPU_PRIORITY_READ_Q_NAME,
					    0, DSPU_PRIORITY_READ_QUEUE_EVENT);
    pDspuReadQueue = create_watched_queue(DSPU_READ_Q_NAME, 0,
					  DSPU_READ_QUEUE_EVENT);
    fpHasData = create_watched_boolean(DSPU_FP_HASDATA_NAME,
				       FP_HASDATA_EVENT);
    process_set_boolean(fpHasData, FALSE);  /*Initial flag off*/

    dspu_cls_init();               /* Initialization for CLS queues */

    initDspuNotificationQ();
 
    pid = process_create(DspuMain, 
			 DSPU_MAIN_PROCESS,
                         LARGE_STACK,
			 PRIO_NORMAL);

    SetDspuStarted(TRUE);

} /* End dspu_start() */

static void DspuMain (void)
{
   boolean have_event;          /* loop control      */
   ulong major, minor;          /* Event information */
   boolean gqHasData;           /* Extra Queue check */
   sys_timestamp dspu_bg_timer; /* DSPU background timer (one second) */


   /*---- Ok DSPU; this is your life. */

   /*
    * Inform the scheduler of the events which trigger this process
    */
   process_watch_queue(pDspuPriorityReadQueue, ENABLE, RECURRING);
   process_watch_queue(pDspuReadQueue,ENABLE,RECURRING);
   process_watch_boolean(fpHasData, ENABLE, RECURRING);
   process_watch_timer(&dspu_bg_timer, ENABLE);

   gqHasData = FALSE;                
   TIMER_START(dspu_bg_timer, ONESEC);

   for(;;)
   {

       /* Wait until an external event awakens us*/
       /* ------------------------------------ */
       process_wait_for_event();                        /* Any event */

       have_event = process_get_wakeup(&major, &minor); /*Event info*/

       /*Check list of events, set flags*/
       while (have_event) {
	   switch(major) {
	   case QUEUE_EVENT:
	       switch(minor) {
	       case DSPU_PRIORITY_READ_QUEUE_EVENT:
	       case DSPU_READ_QUEUE_EVENT:
		   /* Process entries in DSPU queue */
		   /* ----------------------------- */
		   ProcessDspuReceive();
		   break;
	       default:
		   bad_event++;          /* Debugging count unexpected event */
		   break;
	       }
	       break;
           case TIMER_EVENT:
               DspuTimeOut();
               TIMER_UPDATE(dspu_bg_timer, ONESEC);
               break;
	   case BOOLEAN_EVENT:
	       switch (minor) {
	       case FP_HASDATA_EVENT:
		   /* Check if notifications exist or focal point wants 
		    *  to send data
		    * If processes have more data, they need to invoke 
		    * FpHasDatatoSend 
		    */
		   { /*Extra Queue check */
		       /*extra check here for queue look*/
		       if (gqHasData) {
			   ProcessDspuReceive();
			   gqHasData = FALSE;
		       }		       
		   }
 		   process_set_boolean(fpHasData, FALSE);
		   ProcessDspuNotificationQ();
		   ProcessFpData();
		   { 
		       /* Extra queue check needed to make shutdown work,
			* without this we get errors and we end up in a
			* loop here since the fpHasData flag goes on every 
			* time.
		        * Theory is that if I have queued something here 
			* yes fpHasData is set and wakes us up
			* but should also see a QUEUE_EVENT in get_wakeup but
			* we do not.  So never do ProcessDspuReceive.
			* So we do an extra check 
			*/
		       if (process_get_boolean(fpHasData)) {
			   /*Yes - we have something to do */
  			   gqHasData = TRUE;  /*Set our own flag to look at
						queue*/
		       }
		   }
		   break;
	       default:
		   bad_event++;          /* Debugging count unexpected event */
		   break;
	       }
	       break;
	   default:
	       bad_event++;          /* Debugging count unexpected event */
	       break;
	   }
	   have_event = process_get_wakeup(&major,&minor);  /*Next event*/
       }
   }
} /* DspuMain () */

/*=====================================================================*
* ProcessDspuReceive
*======================================================================*
*
* Description:
* This routine drains the priority read queue first to process incoming
* FLOW.INDs, and then processes one regular read queue entry for other
* incoming signals.
* An indication of whether the read queue has been emptied is returned.
*
**/

void ProcessDspuReceive (void)
{

  TCLSIMsg* clsiMsgP;
  int i;

  /* Drain & process all messages from priority read queue */
  /* ----------------------------------------------------- */
  while ((clsiMsgP = process_dequeue(pDspuPriorityReadQueue)) != NULL) {
      DspuReceive(clsiMsgP);
  }

  /* Process one message from normal read queue */
  /* ------------------------------------------ */
  for (i=0; 
       (i<=DSPU_READ_Q_LOOP_MAX) && 
       ((clsiMsgP = process_dequeue(pDspuReadQueue)) != NULL); i++) { 
      DspuReceive(clsiMsgP);  /*Process one entry*/
  }
} /* End ProcessDspuReceive () */


void  DspuQueuePut (TCLSIMsg* theMsg)
{
    
   CLSIQueuePut(gCLSWriteQ, theMsg);
}



/*---- Typedefs used locally. */

typedef struct ClsiMsgStrucT {
        size_t clsiMsgSize;
        TCLSIMsg *clsiMsgP;
        } ClsiMsgStrucT;

/*=====================================================================*
* CLSIMsgCreateIlude: 
*======================================================================*
*
* Description:
*   Try to get clsi message buffer.
*
**/


static boolean
CLSIMsgCreateIlude (ClsiMsgStrucT *clsiMsgStrucP)

{

   clsiMsgStrucP->clsiMsgP = CLSIMsgCreate (clsiMsgStrucP->clsiMsgSize);
   return (clsiMsgStrucP->clsiMsgP == 0);  /* TRUE = no buffer */

}  /* CLSIMsgCreateIlude. */


TCLSIMsg*
BlockForBuffer(size_t size)
{
   ClsiMsgStrucT clsiMsgStruc;
   ulong sleep_time;  /*milleseconds for sleep*/
   boolean no_buffer;

   sleep_time = 10;      
   clsiMsgStruc.clsiMsgSize = size;
   no_buffer = TRUE;
   while (no_buffer) {
       no_buffer = CLSIMsgCreateIlude(&clsiMsgStruc); /*Get a buffer*/
       if (no_buffer) {
	   process_sleep_for(sleep_time); /* try later for the buffer*/
       }
   }
   return (clsiMsgStruc.clsiMsgP);
}

/*
 * SNA_PU subsystem header
 */

#define SNA_PU_MAJVERSION 1
#define SNA_PU_MINVERSION 0
#define SNA_PU_EDITVERSION  1

SUBSYS_HEADER(sna_pu, 
              SNA_PU_MAJVERSION, SNA_PU_MINVERSION, SNA_PU_EDITVERSION,
	      sna_pu_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: cls",
	      "req: cls");

